
/*
 * Copyright (C) 2015 Florent Pouthier
 * Copyright (C) 2015 Emmanuel Pouthier
 *
 * This file is part of SIGMAE.
 *
 * Aye-Aye is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Aye-Aye is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/* SIGMAE
 * core.c
 */

#include <stdio.h>

#include <sigmae/base.h>
#include <sigmae/mod.h>
#include <sigmae/elem.h>


static void
run_elem(SgElem *elem, SgFrames sample)
{
  if (elem->_sample == sample && elem->_rst)
    return;

  SgUShort i;
  SgUShort ni = elem->inps_n;
  SgSize samples = elem->_mod->buffsize;
  SgOutNode *outn;
  SgBoundVar **bvarx = elem->ibvars;
  SgElem *elemx;

  elem->_rst = TRUE;
  elem->_sample = sample;

  for (i = 0; i < ni; i++) {
    outn = elem->_inps[i]->outnode;
    if (outn && elem->_ineedrun[i]) {
      elemx = outn->elem;
      run_elem(elemx, elem->_isample[i] + sample);
    }
  }

  ni += elem->ivars_n;
  for (; i < ni; bvarx++, i++) {
    elemx = (*bvarx)->firstelem;
    if (elemx && elemx != elem && elem->_ineedrun[i])
      run_elem(elemx, elem->_isample[i] + sample);
  }

  switch (elem->_type) {
    case SG_PLUGIN_SG:
      if (elem->flags & SG_ELEMFLAG_USEDOUBLE)
        ((SgRunFun)
         elem->_on_run_double)(elem, sample, samples);
      else
        ((SgRunFun)
         elem->_on_run)(elem, sample, samples);
      break;
    case SG_PLUGIN_LADSPA:
      ((SgLadspaRunFun)
       elem->_on_run)(elem->data, samples);
  }
}

void
sg_run_elem(SgElem *elem, SgFrames sample)
{
  run_elem(elem, sample);
}

void
sg_run_mod(SgMod *mod, SgFrames sample, SgSize blocks)
{
  SgSize i, bs = mod->buffsize;
  SgElem **oelems = (SgElem**)mod->oelems->head;
  SgElem **oelem = oelems;

  if (!*oelem)
    return;
  if (!mod->_isready)
    return;

  for (i = 0; i < blocks; i++) {
    oelem = oelems;
    for (; *oelem; oelem++)
      run_elem(*oelem, sample);
    sample += bs;
    mod->sample = sample;
  }
}

/* end */

