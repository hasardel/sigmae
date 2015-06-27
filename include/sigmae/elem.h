
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
 * elem.h
 */

#ifndef SGELEM_H
#define SGELEM_H

#include <sigmae/base.h>
#include <sigmae/types.h>
#include <sigmae/boundvar.h>
#include <sigmae/plugin.h>

#define SG_ELEMFLAG_USEDOUBLE 1
#define SG_ELEMFLAG_NEEDOWNBUFFS 2

struct _SgOutNode {
  SgElem *elem; /* element that contain this output node*/
  SgUShort nb; /* output node number of the element */
  SgUShort pnb; /* port number */
  SgList *buffref;
  SgVector *inpnodes;
  SgInpNode *left;
};

struct _SgInpNode {
  SgElem *elem;
  SgUShort nb;
  SgUShort pnb;
  SgOutNode *outnode;
  SgOutNode *right;
};

struct _SgElem {
  SgPluginDescP desc;
  SgInt _type;
  SgMod *_mod;
  SgUInt _id;
  SgBool _rst; /* 'has run' status */
  SgFrames _sample;
  SgInt flags;
  SgInpNode **_inps;
  SgOutNode **_outs;
  SgUShort inps_n, outs_n;
  SgUShort vars_n, ivars_n, ovars_n;
  void **ibuffs, **obuffs;
  SgFrames *_isample;
  SgBool *_ineedrun;
  SgBool *iconnected;
  SgBool *oconnected;
  SgVal *vars;
  SgVal *ivars;
  SgVal *ovars;
  SgBoundVar **bvars;
  SgBoundVar **ibvars;
  SgBoundVar **obvars;
  SgFloat srate;
  void **data;
  void* _on_run; /* shortcut */
  void* _on_run_double;
  SgVal *_xtra_params;
};

#define sg_elem_get_buffsize(e) (e->_mod->buffsize)

SgElem *sg_make_elem(SgMod *mod, SgPluginDescP desc, SgInt type);
void sg_init_elem(SgElem *elem);
SgElem *sg_make_init_elem(SgMod *mod, SgPluginDescP desc, SgInt type);
void sg_reset_elem(SgElem *elem);
void sg_remove_elem(SgElem *elem);
void sg_set_iport_sample(SgElem *elem, SgUShort i, SgFrames s);
void sg_set_ivar_sample(SgElem *elem, SgUShort i, SgFrames s);
void sg_set_iport_runneed(SgElem *elem, SgUShort i, SgBool rn);
void sg_set_ivar_runneed(SgElem *elem, SgUShort i, SgBool rn);
SgBool sg_connect_elems(SgElem *elem0, SgElem *elem1, SgUShort o, SgUShort i);
SgBool sg_unconnect_elems(SgElem *elem, SgUShort i);
void sg_connect_elems_unchecked(SgElem *elem0, SgElem *elem1,
                                SgUShort o, SgUShort i);
void sg_unconnect_elems_unchecked(SgElem *elem, SgUShort i);
SgBool sg_elem_find_srcelem(SgElem *elem, SgElem *srcelem);

#endif

/* end */

