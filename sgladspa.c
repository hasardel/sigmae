
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
 * sgladspa.c
 */

#include <sigmae/base.h>
#include <sigmae/mem.h>
#include <sigmae/elem.h>
#include <sigmae/plugin.h>


void sg_make_elem_from_ladspa(SgElem *elem)
{
  SgLadspaPluginDesc *desc = elem->desc.ladspa;
  SgLadspaPortDesc *pdesc = desc->port_descs;
  SgUShort *pnbs, nip = 0, nop = 0, niv = 0, nov = 0;
  SgUShort a, b, c, d;
  SgUShort np = desc->port_count;

  for (; np; pdesc++, np--) {
    if (sg_ladspa_port_is_audio(*pdesc)) {
      if (sg_ladspa_port_is_input(*pdesc))
        nip++;
      else
        nop++;
    } else {
      if (sg_ladspa_port_is_input(*pdesc))
        niv++;
      else
        nov++;
    }
  }

  elem->inps_n = nip;
  elem->outs_n = nop;
  elem->vars_n = niv + nov;
  elem->ivars_n = niv;
  elem->ovars_n = nov;
  elem->flags = 0; 
  elem->_on_run = (void*)desc->run;

  if (sg_ladspa_is_inplace_broken(desc->properties))
    elem->flags |= SG_ELEMFLAG_NEEDOWNBUFFS;

  a = nip;
  b = a + nop;
  c = b + niv;
  d = c + nov;
  np = d;
  pnbs = sg_alloc(np * sizeof(SgShort));
  for (pdesc--; np; pdesc--) {
    np--;
    if (sg_ladspa_port_is_audio(*pdesc)) {
      if (sg_ladspa_port_is_input(*pdesc))
        pnbs[--a] = np;
      else
        pnbs[--b] = np;
    } else {
      if (sg_ladspa_port_is_input(*pdesc))
        pnbs[--c] = np;
      else
        pnbs[--d] = np;
    }
  }
  elem->data = (void*)pnbs;
}

void sg_init_elem_from_ladspa(SgElem *elem)
{
  SgLadspaPluginDesc *desc = elem->desc.ladspa;
  SgLadspaPortDesc *pdescs = desc->port_descs, pdesc;
  SgLadspaPortRangeHint *phints = desc->port_range_hints, phint;
  SgUShort iv = 0, ov = 0, i = 0, np = desc->port_count;
  SgLadspaData val;

  for (; i < np; i++) {
    pdesc = pdescs[i];
    if (sg_ladspa_port_is_control(pdesc)) {
      val = 0.f;
      phint = phints[i];
      if (sg_ladspa_hint_is_sr(phint.hint_desc)) {
          phint.lower_bound *= elem->srate;
          phint.upper_bound *= elem->srate;
      }
      if (sg_ladspa_hint_is_default_0(phint.hint_desc)) {
        ;
      } else if (sg_ladspa_hint_is_default_1(phint.hint_desc)) {
        val = 1.f;
      } else if (sg_ladspa_hint_is_default_min(phint.hint_desc)) {
        if (sg_ladspa_hint_is_bounded_below(phint.hint_desc))
          val = phint.lower_bound;
      } else if (sg_ladspa_hint_is_default_max(phint.hint_desc)) {
        if (sg_ladspa_hint_is_bounded_above(phint.hint_desc))
          val = phint.upper_bound;
      } else if (!sg_ladspa_hint_is_toggled(phint.hint_desc)) {
        if (sg_ladspa_hint_is_default_100(phint.hint_desc))
          val = 100.f;
        else if (sg_ladspa_hint_is_default_440(phint.hint_desc)) {
          val = 440.f;
        }
        else if (sg_ladspa_hint_is_default_middle(phint.hint_desc))
          val = phint.lower_bound + (phint.upper_bound - phint.lower_bound) / 2.f;
      }

      if (sg_ladspa_port_is_input(pdesc))
        elem->ivars[iv++].fl = val;
      else
        elem->ovars[ov++].fl = val;
    }
  }
}

/*end*/

