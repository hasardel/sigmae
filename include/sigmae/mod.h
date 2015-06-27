
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
 * mod.h
 */

#ifndef SGMOD_H
#define SGMOD_H

#include <sigmae/base.h>
#include <sigmae/types.h>

#define SG_MODFLAG_CANUSEDOUBLE 1

struct _SgMod {
  SgUInt _cid;
  SgBool _isready;
  SgInt flags;
  SgFloat srate;
  SgFrames sample;
  SgSize buffsize;
  SgList *buffs;
  SgList *freebuffs;
  SgUInt fbn;
  void *iunbuff;
  void *ounbuff;
  SgBTree *public_elems_byid;
  SgBTree *public_elems_byname;
  SgBTree *private_elems_byid;
  SgList *elems;
  SgVector *oelems;
};

SgMod *sg_make_mod(SgFloat srate, SgSize buffsize, SgInt flags);
void sg_free_mod(SgMod *mod);
void sg_rmbuff(SgMod *mod, SgList *br);
SgList *sg_newbuff(SgMod *mod);
void sg_mod_prepare(SgMod *mod);
void sg_mod_add_output_elem(SgElem *elem);
void sg_mod_remove_output_elem(SgElem *elem);

#endif

/* end */

