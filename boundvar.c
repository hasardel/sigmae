
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
 * boundvar.c
 */

#include <stdio.h>

#include <sigmae/mem.h>
#include <sigmae/elem.h>
#include <sigmae/boundvar.h>


SgBoundVar *sg_make_bvar(SgVal *val, SgElem *elem, SgInt flags)
{
  SgBoundVar *bvar = sg_alloc(sizeof(SgBoundVar));
  sg_init_bvar(bvar, val, elem, flags);
  return bvar;
}

void sg_init_bvar(SgBoundVar *bvar, SgVal *val, SgElem *elem, SgInt flags)
{
  bvar->pnb = 0;
  bvar->flags = flags;
  bvar->val = val;
  bvar->ivar = NULL;
  bvar->ovars = sg_make_vector();
  bvar->on_setvar = NULL;
  bvar->on_varset = NULL;
  bvar->elem = elem;
  bvar->firstelem = NULL;
}

void sg_setout_bvarfun(SgBoundVar *bvar, SgBVarFun on_setvar)
{
  bvar->on_setvar = on_setvar;
}

void sg_setinp_bvarfun(SgBoundVar *bvar, SgBVarFun on_varset)
{
  bvar->on_varset = on_varset;
}

void sg_free_bvar(SgBoundVar *bvar)
{
  sg_free_vector(bvar->ovars);
  sg_free(bvar);
}

static void set_firstelem(SgBoundVar *bvar, SgElem *elem)
{
  SgBoundVar **bvarxs, *bvarx;

  bvarxs = (SgBoundVar**)bvar->ovars->head;
  for (; (bvarx = *bvarxs); bvarxs++) {
    bvarx->firstelem = elem;
    if (bvarx->elem)
      break;
    set_firstelem(bvarx, elem);
  }
}

static void bound_bvars(SgBoundVar *v0, SgBoundVar *v1)
{
  if (!v1->ivar) {
    sg_vector_push(v0->ovars, v1);
    v1->ivar = v0;

    if (v0->elem)
      v1->firstelem = v0->elem;
    else {
      if (!v0->firstelem)
        return;
      v1->firstelem = v0->firstelem;
    }
    if (!v1->elem)
      set_firstelem(v1, v1->firstelem);
  }
}

void sg_unbound_bvars(SgBoundVar *v0, SgBoundVar *v1)
{
  SgLong i = sg_vector_find(v0->ovars, v1);

  if (i != -1) {
    sg_vector_remove(v0->ovars, i);
    v1->ivar = NULL;

    if (v1->firstelem) {
      v1->firstelem = NULL;
      if (!v1->elem)
        set_firstelem(v1, NULL);
    }
  }
}

void sg_unbound_bvar(SgBoundVar *bvar)
{
  SgBoundVar **bvarx;

  if (bvar->ivar)
    sg_unbound_bvars(bvar->ivar, bvar);
  for (bvarx = (SgBoundVar**)bvar->ovars->head; *bvarx; bvarx++)
    sg_unbound_bvars(bvar, *bvarx);
}

static void set_bvar(SgBoundVar *bvar, SgVal val)
{
  SgBoundVar **bvarx = (SgBoundVar**)bvar->ovars->head;

  if (bvar->on_varset)
    val = bvar->on_varset(&val);
  *(bvar->val) = val;

  if (bvar->on_setvar && *bvarx)
    val = bvar->on_setvar(bvar->val);
  for (; *bvarx; bvarx++)
    set_bvar(*bvarx, val);
}

void sg_set_bvar(SgBoundVar *bvar, SgVal val)
{
  *(bvar->val) = val;
  sg_update_bvar(bvar);
}

void sg_update_bvar(SgBoundVar *bvar)
{
  SgBoundVar **bvarx = (SgBoundVar**)bvar->ovars->head;
  SgVal val;

  if (*bvarx) {
    if (bvar->on_setvar)
      val = bvar->on_setvar(bvar->val);
    else
      val = *(bvar->val);
    for (; *bvarx; bvarx++)
      set_bvar(*bvarx, val);
  }
}

void sg_bvar_attach(SgBoundVar *bvar, SgElem *elem)
{
  if (bvar->elem || !elem)
    return;
  bvar->elem = elem;
  set_firstelem(bvar, elem);
}

void sg_bvar_detach(SgBoundVar *bvar)
{
  if (!bvar->elem)
    return;
  if (bvar->flags & SG_BVARFLAG_ELEM_LOCKED)
    return;
  bvar->elem = NULL;
  set_firstelem(bvar, bvar->firstelem);
}


/* end */

