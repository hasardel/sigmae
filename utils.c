
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
 * utils.c
 */

#include <stdio.h>

#include <sigmae/base.h>
#include <sigmae/elem.h>


void sg_inspect_elem(SgElem *elem)
{
  SgUShort i, o;
  SgInpNode **inpn;
  SgOutNode *outn;
  SgElem *lm;
  SgUShort ni = elem->inps_n;
  SgUShort no = elem->outs_n;
  printf("elem=%p\n", elem);
  if (elem->_type & SG_PLUGIN_SG)
    printf("name=%s\n", ((char**)(elem->desc.sg))[0]);
  else
    printf("name=%s\n", elem->desc.ladspa->label);
  printf("type=%d\n", elem->_type);
  printf("mod=%p\n", elem->_mod);
  printf("id=%d\n", elem->_id);
  printf("smpl=%Lu\n", elem->_sample);
  printf("inpnodes=%p\n", elem->_inps);
  printf("outnodes=%p\n", elem->_outs);
  printf("inpnb=%d\n", elem->inps_n);
  printf("outnb=%d\n", elem->outs_n);
  printf("varnb=%d\n", elem->vars_n);
  printf("sr=%f\n", elem->srate);
  printf("--- -- -- -- -- -- ---\n");
  for (i=0;i<ni;i++) {
    outn = elem->_inps[i]->outnode;
    lm = NULL;
    if (outn)
      lm = outn->elem;
    printf("inp%u: buff=%p, outelem=%p\n", i, elem->ibuffs[i], lm);
  }
  for (o=0;o<no;o++) {
    printf("out%u: buff=%p\n", o, elem->obuffs[o]);
    inpn = (SgInpNode**)elem->_outs[o]->inpnodes->head;
    for (;*inpn;inpn++)
      printf("  inpelem=%p\n", (*inpn)->elem);
  }
  printf("\n");
}

/* end */

