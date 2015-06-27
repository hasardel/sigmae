
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
 * plugin.h
 */

#ifndef SGPLUGIN_H
#define SGPLUGIN_H

#include <sigmae/base/types.h>
#include <sigmae/types.h>
#include <sigmae/sgladspa.h>

#define SG_PLUGIN_ELEM 1
#define SG_PLUGIN_MOD 2
#define SG_PLUGIN_SG 4
#define SG_PLUGIN_LADSPA 8

/* var mode */
#define SG_VAR_TYPE_FLOAT 1
#define SG_VAR_TYPE_INT 2
#define SG_VAR_TYPE_BOOL 4
#define SG_VAR_TYPE_STRING 8
#define SG_VAR_TYPE_MASK 15

#define sg_var_type_is_float(t) (t & SG_VAR_TYPE_FLOAT)
#define sg_var_type_is_int(t) (t & SG_VAR_TYPE_INT)
#define sg_var_type_is_bool(t) (t & SG_VAR_TYPE_BOOL)
#define sg_var_type_is_string(t) (t & SG_VAR_TYPE_STRING)

struct _SgPortDesc {
  SgChar *name;
  SgInt type; /* unused for the moment */
  SgFloat upper_limit;
  SgFloat lower_limit;
};

struct _SgVarDesc {
  SgChar *name;
  SgInt type;
  SgVal value;
  SgVal upper_limit;
  SgVal lower_limit;
};

typedef void (*SgRunFun)(SgElem*, SgFrames s, SgSize ns);
typedef void (*SgConnectFun)(SgElem*, SgUShort port);
typedef void (*SgChangeFun)(SgElem*, SgUShort varnb);
typedef void (*SgMakeFun)(SgElem*);
typedef void (*SgInitFun)(SgElem*);
typedef void (*SgResetFun)(SgElem*);
typedef void (*SgRemoveFun)(SgElem*);

struct _SgElemPluginDesc {
};

struct _SgModPluginDesc {
};

struct _SgSgPluginDesc {
  SgChar *id;
  SgUShort iports_n;
  SgUShort oports_n;
  SgUShort ivars_n;
  SgUShort ovars_n;
  SgPortDesc *ports;
  SgVarDesc *vars;
  SgInt flags;
  SgRunFun on_run;
  SgRunFun on_run_double;
  SgConnectFun on_connect;
  SgChangeFun on_change;
  SgMakeFun on_make;
  SgInitFun on_init;
  SgResetFun on_reset;
  SgRemoveFun on_remove;
  SgChar *name;
  SgChar *author;
  SgChar *copyright;
  SgChar *help;
};

union _SgPluginDescP {
  SgSgPluginDesc *sg;
  SgLadspaPluginDesc *ladspa;
  void *other;
};

typedef SgPluginDescP (*SgPluginDescFun)(SgULong idx);

/* type for the structure describing a collection of plugins in a
   shared object file (corresponding to the symbol sg_plugins_info) */
typedef struct {
  SgUInt type;
  SgChar *id;
  SgPluginDescFun sg_plugin_desc_fun;
  SgChar *name;
  SgChar *author;
  SgChar *copyright;
  SgChar *help;
} SgPluginsInfo;

void sg_plugin_register(SgPluginDescP, SgInt type);
SgBool sg_plugin_check(SgPluginDescP, SgInt type);

#endif

/* end */

