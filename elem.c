
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
 * elem.c
 */

#include <string.h>
#include <stdio.h>

#include <sigmae/base.h>
#include <sigmae/mem.h>
#include <sigmae/mod.h>
#include <sigmae/elem.h>
#include <sigmae/plugin.h>
#include <sigmae/boundvar.h>

#include "elem.h"
#include "sgladspa.h"


static SgInpNode *
make_inpnode(SgElem *elem, SgUShort nb, SgUShort pnb)
{
  SgInpNode *i = sg_alloc(sizeof(SgInpNode));
  i->outnode = NULL;
  i->elem = elem;
  i->nb = nb;
  i->pnb = pnb;
  i->right = NULL;
  return i;
}

static void
free_inpnode(SgInpNode *inpn)
{
  sg_free(inpn);
}

static SgOutNode *
make_outnode(SgElem *elem, SgUShort nb, SgUShort pnb)
{
  SgOutNode *o = sg_alloc(sizeof(SgOutNode));
  o->inpnodes = sg_make_vector();
  o->elem = elem;
  o->buffref = NULL;
  o->nb = nb;
  o->pnb = pnb;
  o->left = NULL;
  return o;
}

static void
free_outnode(SgOutNode *outn)
{
  sg_free_vector(outn->inpnodes);
  sg_free(outn);
}

static void
connect_port(SgElem *elem, SgUShort pnb, void *buff)
{
  switch (elem->_type) {
    case SG_PLUGIN_SG:
      elem->desc.sg->on_connect(elem, pnb);
      break;
    case SG_PLUGIN_LADSPA:
      elem->desc.ladspa->connect_port(elem->data, pnb, buff);
  }
}

static void
connect_iport(SgElem *elem, SgUShort i)
{
  SgUShort pnb = elem->_inps[i]->pnb;
  connect_port(elem, pnb, elem->ibuffs[i]);
}

static void
connect_oport(SgElem *elem, SgUShort i)
{
  SgUShort pnb = elem->_outs[i]->pnb;
  connect_port(elem, pnb, elem->obuffs[i]);
}

static void
connect_vport(SgElem *elem, SgUShort i)
{
  SgUShort pnb = elem->bvars[i]->pnb;
  connect_port(elem, pnb, &elem->vars[i]);
}

static void
make_elem_from_sg(SgElem *elem)
{
  SgSgPluginDesc *desc = elem->desc.sg;
  SgUShort *pnbs, np;

  elem->inps_n = desc->iports_n;
  elem->outs_n = desc->oports_n;
  elem->vars_n = desc->ivars_n + desc->ovars_n;
  elem->ivars_n = desc->ivars_n;
  elem->ovars_n = desc->ovars_n;
  elem->flags = desc->flags;
  elem->_on_run = (void*)desc->on_run;
  elem->_on_run_double = (void*)desc->on_run_double;

  np = elem->inps_n + elem->outs_n + elem->vars_n;
  pnbs = sg_alloc(np * sizeof(SgShort));
  for (; np;) {
    np--;
    pnbs[np] = np;
  }
  elem->data = (void*)pnbs;
}

static SgElem *
make_elem(SgMod *mod, SgPluginDescP desc, SgInt type)
{
  SgElem *elem = sg_alloc(sizeof(SgElem));
  SgUShort nip, nop, niv, nov;
  SgUShort i;
  SgInt flags;
  SgUShort *pnbs;
  void *buff;

  elem->desc = desc;
  elem->_type = type;
  elem->_mod = mod;
  elem->_id = mod->_cid++;
  elem->srate = mod->srate;
  elem->data = NULL;
  elem->_xtra_params = NULL;

  switch (type) {
    case SG_PLUGIN_SG:
      make_elem_from_sg(elem);
      break;
    case SG_PLUGIN_LADSPA:
      sg_make_elem_from_ladspa(elem);
      break;
  }
  pnbs = (SgUShort*)elem->data;

  if (!(elem->_mod->flags & SG_MODFLAG_CANUSEDOUBLE))
    elem->flags &= ~SG_ELEMFLAG_USEDOUBLE;

  nip = elem->inps_n;
  nop = elem->outs_n;
  niv = elem->ivars_n;
  nov = elem->ovars_n;
  elem->_inps = sg_alloc(((nip + nop) * 2 + niv + nov) *
                         sizeof(void*));
  elem->_outs = &((SgOutNode**)elem->_inps)[nip];
  elem->ibuffs = &((void**)elem->_outs)[nop];
  elem->obuffs = &((void**)elem->ibuffs)[nip];
  buff = elem->_mod->iunbuff;
  for (i = 0; i < nip; i++) {
    elem->_inps[i] = make_inpnode(elem, i, *pnbs++);
    elem->ibuffs[i] = buff;
  }
  buff = elem->_mod->ounbuff;
  for (i = 0; i < nop; i++) {
    elem->_outs[i] = make_outnode(elem, i, *pnbs++);
    elem->obuffs[i] = buff;
  }
  elem->ibvars = &((SgBoundVar**)elem->obuffs)[nop];
  elem->obvars = &((SgBoundVar**)elem->obvars)[niv];
  elem->bvars = elem->ibvars;
  elem->ivars = sg_alloc((niv + nov) * sizeof(SgVal));
  elem->ovars = &elem->ivars[niv];
  elem->vars = elem->ivars;
  for (i = 0; i < niv + nov; i++) {
    flags = SG_BVARFLAG_ELEM_LOCKED;
    if (i >= niv)
      flags &= SG_BVARFLAG_ELEM_OUTPUT;
    elem->bvars[i] = sg_make_bvar(&elem->vars[i],
                                  elem, flags);
    elem->bvars[i]->pnb = *pnbs++;
  }
  elem->_isample = sg_alloc((nip + niv) *
                            sizeof(SgFrames));
  elem->_ineedrun = sg_alloc((nip + niv + nip + nop) *
                             sizeof(SgBool));
  elem->iconnected = &elem->_ineedrun[nip + niv];
  elem->oconnected = &elem->iconnected[nip];

  sg_free(elem->data);
  elem->data = NULL;

  mod->elems = sg_list_insert(mod->elems, elem);

  return elem;
}

SgElem *
sg_make_elem(SgMod *mod, SgPluginDescP desc, SgInt type)
{
  SgElem *elem = make_elem(mod, desc, type);
  SgUShort i;

  switch (type) {
    case SG_PLUGIN_SG:
      desc.sg->on_make(elem);
      break;
    case SG_PLUGIN_LADSPA:
      elem->data =
        desc.ladspa->instantiate(desc.ladspa,
                                 (SgULong)elem->srate);
      /* we first need to connect all ports to allow the
         ladspa plugin to register the correspondig buffers */
      for (i = 0; i < elem->inps_n; i++)
        connect_iport(elem, i);
      for (i = 0; i < elem->outs_n; i++)
        connect_oport(elem, i);
      for (i = 0; i < elem->vars_n; i++)
        connect_vport(elem, i);
  }

  return elem;
}

static void
free_elem(SgElem *elem)
{
  SgUShort i;

  for (i = 0; i < elem->inps_n; i++)
    free_inpnode(elem->_inps[i]);
  for (i = 0; i < elem->outs_n; i++)
    free_outnode(elem->_outs[i]);
  for (i = 0; i < elem->vars_n; i++)
    sg_free_bvar(elem->bvars[i]);
  sg_free(elem->_inps); /* _inps, _outs, ibuffs, obuffs, ibvars, obvars */
  sg_free(elem->ivars); /* ivars, ovars */
  sg_free(elem->_isample);
  sg_free(elem->_ineedrun); /* _ineedrun, iconnected, oconnected */
  sg_free(elem);
}

void
sg_remove_elem(SgElem *elem)
{
  switch (elem->_type) {
    case SG_PLUGIN_SG:
      elem->desc.sg->on_remove(elem);
      break;
    case SG_PLUGIN_LADSPA:
      elem->desc.ladspa->cleanup(elem->data);
  }
  free_elem(elem);
}

static void
init_elem_from_sg(SgElem *elem)
{
  SgVarDesc *vdescs = elem->desc.sg->vars;
  SgVal *vars = elem->vars;
  SgUShort nv = elem->vars_n;

  for (; nv; nv--)
    *vars++ = vdescs++->value;
}

static void
init_elem(SgElem *elem)
{
  SgUShort i;

  elem->_rst = FALSE;
  elem->_sample = 0;
  i = elem->inps_n + elem->ivars_n;
  for (; i;) {
    elem->_isample[--i] = 0;
    elem->_ineedrun[i] = TRUE;
  }

  switch (elem->_type) {
    case SG_PLUGIN_SG:
      init_elem_from_sg(elem);
      break;
    case SG_PLUGIN_LADSPA:
      sg_init_elem_from_ladspa(elem);
  }
}

void
sg_init_elem(SgElem *elem)
{
  init_elem(elem);

  switch (elem->_type) {
    case SG_PLUGIN_SG:
      elem->desc.sg->on_init(elem);
      break;
    case SG_PLUGIN_LADSPA:
      elem->desc.ladspa->activate(elem->data);
  }
}

SgElem *
sg_make_init_elem(SgMod *mod, SgPluginDescP desc, SgInt type)
{
  SgElem *elem = sg_make_elem(mod, desc, type);
  sg_init_elem(elem);
  return elem;
}

void
sg_reset_elem(SgElem *elem)
{
  switch (elem->_type) {
    case SG_PLUGIN_SG:
      elem->desc.sg->on_reset(elem);
      break;
    case SG_PLUGIN_LADSPA:
      elem->desc.ladspa->deactivate(elem->data);
  }
}

void
sg_set_iport_sample(SgElem *elem, SgUShort i, SgFrames s)
{
  if (i < elem->inps_n)
    elem->_isample[i] = s;
}

void
sg_set_ivar_sample(SgElem *elem, SgUShort i, SgFrames s)
{
  if (i < elem->ivars_n)
    elem->_isample[elem->inps_n + i] = s;
}

void
sg_set_iport_runneed(SgElem *elem, SgUShort i, SgBool rn)
{
  if (i < elem->inps_n)
    elem->_ineedrun[i] = rn;
}

void
sg_set_ivar_runneed(SgElem *elem, SgUShort i, SgBool rn)
{
  if (i < elem->ivars_n)
    elem->_ineedrun[elem->inps_n + i] = rn;
}

static SgBool
connect_nodes(SgOutNode *outn, SgInpNode *inpn)
{
  if (inpn->outnode)
    return FALSE;
  inpn->outnode = outn;
  sg_vector_push(outn->inpnodes, inpn);
  return TRUE;
}

static SgBool
unconnect_nodes(SgInpNode *inpn)
{
  SgOutNode *outn = inpn->outnode;

  if (!outn)
    return FALSE;
  inpn->outnode = NULL;
  sg_vector_remove(outn->inpnodes,
                   sg_vector_find(outn->inpnodes, inpn));
  return TRUE;
}

static SgBool
set_inpn_link(SgInpNode *inpn)
{
  SgElem *elem = inpn->elem;
  SgOutNode *outn;
  SgUShort no, i;

  if (!(elem->flags & SG_ELEMFLAG_NEEDOWNBUFFS)) {
    no = elem->outs_n;
    for (i = 0; i < no; i++) {
      outn = elem->_outs[i];
      if (!outn->left && outn->buffref) {
        inpn->right = outn;
        outn->left = inpn;
        return TRUE;
      }
    }
  }
  return FALSE;
}

static void
setbuff(SgOutNode *outn, SgList *br)
{
  SgElem *elem = outn->elem;
  SgInpNode *inpn;

  for (;;) {
    outn->buffref = br;
    elem->obuffs[outn->nb] = (SgFloat*)br->data;
    connect_oport(elem, outn->nb);
    if (outn->inpnodes->size != 1)
      break;
    inpn = outn->inpnodes->head[0];
    elem = inpn->elem;
    elem->ibuffs[inpn->nb] = (SgFloat*)br->data;
    connect_iport(elem, inpn->nb);
    outn = inpn->right;
    if (!outn || outn->buffref == br)
      break;
  }
}

static void
set_outn(SgOutNode *outn)
{
  SgElem *elem = outn->elem;
  SgInpNode *inpn;
  SgUShort ni = elem->inps_n, i = 0;
  SgList *br = NULL;

  if (!(elem->flags & SG_ELEMFLAG_NEEDOWNBUFFS))
    for (; i < ni; i++) {
     inpn = elem->_inps[i];
      if (inpn->outnode && !inpn->right &&
          inpn->outnode->inpnodes->size == 1) {
        inpn->right = outn;
        outn->left = inpn;
        br = inpn->outnode->buffref;
        if (outn->buffref)
          sg_rmbuff(elem->_mod, outn->buffref);
        break;
      }
    }
  if (!br && !outn->buffref)
    br = sg_newbuff(elem->_mod);
  if (br)
    setbuff(outn, br);
}

void
sg_connect_elems_intern(SgElem *elem0, SgElem *elem1,
                        SgUShort o, SgUShort i)
{
  SgOutNode *outn;
  SgInpNode *inpn, *inpnx;

  outn = elem0->_outs[o];
  inpn = elem1->_inps[i];
  if (!connect_nodes(outn, inpn))
    return;

  elem0->oconnected[o] = TRUE;
  elem1->iconnected[i] = TRUE;

  if (!outn->buffref) {
    /* first connection on this node */
    set_outn(outn);
    if (set_inpn_link(inpn)) {
      sg_rmbuff(elem1->_mod, inpn->right->buffref);
      setbuff(inpn->right, outn->buffref);
    }
  } else {
    elem1->ibuffs[i] = outn->buffref->data;
    connect_iport(elem1, i);
    if (outn->inpnodes->size == 2) {
      inpnx = outn->inpnodes->head[0];
      if (inpnx->right) {
        setbuff(inpnx->right,
                sg_newbuff(inpnx->elem->_mod));
        inpnx->right->left = NULL;
        inpnx->right = NULL;
      }
    }
  }
}

void
sg_unconnect_elems_intern(SgElem *elem, SgUShort i)
{
  SgOutNode *outn, *outnx;
  SgInpNode *inpn, *inpnx;
  SgElem *elemx;

  inpn = elem->_inps[i];
  outn = inpn->outnode;
  if (!unconnect_nodes(inpn))
    return;

  elem->iconnected[i] = FALSE;
  elem->ibuffs[i] = elem->_mod->iunbuff;
  connect_iport(elem, i);

  outnx = inpn->right;
  if (outnx) {
    inpn->right = NULL;
    outnx->left = NULL;
    if (!outn->inpnodes->size)
      outnx->buffref = NULL;
    set_outn(outnx);
  }

  if (!outn->inpnodes->size) {
    elemx = outn->elem;
    elemx->oconnected[outn->nb] = FALSE;
    elemx->obuffs[outn->nb] = elemx->_mod->ounbuff;
    connect_oport(elemx, outn->nb);
    inpnx = outn->left;
    if (inpnx) {
      outn->buffref = NULL;
      inpnx->right = NULL;
      outn->left = NULL;
      if (set_inpn_link(inpnx)) {
        sg_rmbuff(elemx->_mod, inpnx->right->buffref);
        setbuff(inpnx->right, inpnx->outnode->buffref);
      }
    } else {
      sg_rmbuff(elemx->_mod, outn->buffref);
      outn->buffref = NULL;
    }

  } else if (outn->inpnodes->size == 1) {
    inpnx = outn->inpnodes->head[0];
    if (set_inpn_link(inpnx)) {
      elemx = inpnx->elem;
      sg_rmbuff(elemx->_mod, inpnx->right->buffref);
      setbuff(inpnx->right, outn->buffref);
    }
  }
}

SgBool
sg_elem_find_srcelem(SgElem *elem0, SgElem *elem1)
{
  SgOutNode *outn;
  SgElem *elemx;
  SgUShort i;

  if (elem0 == elem1)
    return TRUE;

  for (i = elem->inps_n; i;) {
    outn = elem->_inps[--i]->outnode;
    if (outn && sg_elem_find_srcelem(outn->elem, elem1)
      return TRUE;
  }

  for (i = elem0->ivars_n; i;) {
    elemx = elem0->ibvars[--i]->firstelem;
    if (elemx && sg_elem_find_srcelem(elemx, elem1)
      return TRUE;
  }

  return FALSE;
}

void
sg_connect_elems(SgElem *elem0, SgElem *elem1,
                 SgUShort o, SgUShort i,
                 SgBool lcheck)
{
  SgSgPluginDesc *convpl;
  SgOutNode *outn;

  if (!elem0->_mod->_isready)
    return;
  if (o >= elem0->outs_n || i >= elem1->inps_n ||
      elem0->_mod != elem1->_mod)
    return;
  if (elem0->flags & SG_ELEMFLAG_IMPLICIT ||
      elem1->flags & SG_ELEMFLAG_IMPLICIT)
    return;
  if (lcheck && sg_elem_find_srcelem(elem0, elem1))
    return;

  if (elem0->flags & SG_ELEMFLAG_USEDOUBLE) {
    if (~(elem1->flags & SG_ELEMFLAG_USEDOUBLE)) {
      outn = elem0->_outs[o];
      if (outn->conv) {
        elem0 = outn->conv;
        o = 0;
      } else {
        convpl = sg_get_plugin(SG_PLUGIN_SG, "sg.base",
                               "convert.double->float");
        elem0 = sg_make_elem(elem0->_mod, convpl,
                             SG_PLUGIN_SG);
        elem0->flags |= SG_ELEMFLAG_IMPLICIT);
        sg_init_elem(elem0);
        outn->conv = elem0;
        o = 0;
      }
    }
  } else {
    if (elem1->flags & SG_ELEMFLAG_USEDOUBLE) {
      outn = elem0->_outs[o];
      if (outn->conv) {
        elem0 = outn->conv;
        o = 0;
      } else {
        convpl = sg_get_plugin(SG_PLUGIN_SG, "sg.base",
                               "convert.float->double");
        elem0 = sg_make_elem(elem0->_mod, convpl,
                             SG_PLUGIN_SG);
        elem0->flags |= SG_ELEMFLAG_IMPLICIT);
        sg_init_elem(elem0);
        outn->conv = elem0;
        o = 0;
      }
    }
  }

  sg_connect_elems_intern(elem0, elem1, o, i);
}

void
sg_unconnect_elems(SgElem *elem, SgUShort i)
{
  if (!elem->_mod->_isready)
    return;
  if (i >= elem->inps_n)
    return;
  if (elem->flags & SG_ELEMFLAG_IMPLICIT)
    return;

  sg_unconnect_elems_intern(elem, i);
}

/* end */

