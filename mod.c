
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
 * mod.c
 */

#include <sigmae/base.h>
#include <sigmae/mem.h>
#include <sigmae/mod.h>
#include <sigmae/elem.h>


#define MAXFBSZ 16384

SgMod *sg_make_mod(SgFloat srate, SgSize buffsize, SgInt flags)
{
  SgMod *mod = sg_alloc(sizeof(SgMod));

  mod->_cid = 0;
  mod->_isready = FALSE;
  mod->flags = flags;
  mod->sample = 0;
  mod->srate = srate;
  mod->buffsize = buffsize;
  mod->buffs = NULL;
  mod->freebuffs = NULL;
  mod->fbn = 0;
  mod->iunbuff = (SgFloat*)sg_calloc(buffsize, sizeof(SgFloat));
  mod->ounbuff = (SgFloat*)sg_alloc(buffsize * sizeof(SgFloat));
  mod->elems = NULL;
  mod->oelems = sg_make_vector();

  return mod;
}

void sg_free_mod(SgMod *mod)
{
  sg_list_apply(mod->buffs, sg_free);
  sg_free_list(mod->buffs);
  sg_list_apply(mod->freebuffs, sg_free);
  sg_free_list(mod->freebuffs);
  sg_list_apply(mod->elems, (void (*)(void*))sg_remove_elem);
  sg_free_list(mod->elems);
  sg_free_vector(mod->oelems);
  sg_free(mod);
}

void sg_rmbuff(SgMod *mod, SgList *br)
{
  mod->buffs = sg_list_remlnk(mod->buffs, br);
  if ((mod->fbn + 1) * mod->buffsize > MAXFBSZ)
    sg_free_list(br);
  else {
    mod->freebuffs->prev = br;
    br->next = mod->freebuffs;
    mod->freebuffs = br;
    mod->fbn++;
  }
}

SgList *sg_newbuff(SgMod *mod)
{
  SgList *br;
  void *b;

  br = mod->freebuffs;
  if (br) {
    mod->freebuffs = sg_list_remlnk(br, br);
    mod->buffs->prev = br;
    br->next = mod->buffs;
    mod->buffs = br;
    mod->fbn--;
    return br;
  }

  b = sg_alloc(mod->buffsize *
               (mod->flags & SG_MODFLAG_USEDOUBLE ?
                sizeof(SgDouble) : sizeof(SgFloat)));
  mod->buffs = sg_list_insert(mod->buffs, b);
  return mod->buffs;
}

void sg_mod_prepare(SgMod *mod)
{
  mod->_isready = TRUE;
}

void sg_mod_add_output_elem(SgElem *elem)
{
  SgMod *mod = elem->_mod;

  if (sg_vector_find(mod->oelems, elem) == -1)
    sg_vector_push(mod->oelems, elem);
}

void sg_mod_remove_output_elem(SgElem *elem)
{
  SgMod *mod = elem->_mod;
  SgLong i = sg_vector_find(mod->oelems, elem);

  if (i != -1)
    sg_vector_remove(mod->oelems, i);
}

/* end */

