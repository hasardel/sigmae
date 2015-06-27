
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
 * boundvar.h
 */

#ifndef SGBOUNDVAR_H
#define SGBOUNDVAR_H

#include <sigmae/base.h>
#include <sigmae/types.h>

#define SG_BVARFLAG_ELEM_OUTPUT 1
#define SG_BVARFLAG_ELEM_LOCKED 2

typedef SgVal (*SgBVarFun)(SgVal *s);

struct _SgBoundVar {
  SgUShort pnb;
  SgInt flags;
  SgVal *val;
  SgBoundVar *ivar;
  SgVector *ovars;
  SgBVarFun on_setvar;
  SgBVarFun on_varset;
  SgElem *elem;
  SgElem *firstelem;
};

SgBoundVar *sg_make_bvar(SgVal *val, SgElem *elem, SgInt flags);
void sg_init_bvar(SgBoundVar *bvar, SgVal *val, SgElem *elem, SgInt flags);
void sg_free_bvar(SgBoundVar *bvar);
void sg_bound_bvars(SgBoundVar *v0, SgBoundVar *v1);
void sg_unbound_bvars(SgBoundVar *v0, SgBoundVar *v1);
void sg_unbound_bvar(SgBoundVar *bvar);
void sg_set_bvar(SgBoundVar *bvar, SgVal val);
void sg_update_bvar(SgBoundVar *bvar);
void sg_bvar_attach(SgBoundVar *bvar, SgElem *elem);
void sg_bvar_detach(SgBoundVar *bvar);

#endif

/* end */

