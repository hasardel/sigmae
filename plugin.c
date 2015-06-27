
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
 * plugin.c
 */

#include <ctype.h>
#include <string.h>

#include <sigmae/base.h>
#include <sigmae/error.h>
#include <sigmae/mem.h>
#include <sigmae/plugin.h>


typedef struct {
  char *id;
  SgBool lst;
  SgPluginsInfo *pinfo;
  SgList *plist;
  SgULong nb;
} PluginsContainer;


static SgBTree *sg_plugins = NULL;
static SgBTree *ladspa_plugins = NULL;


SgPluginDescP sg_make_sg_plugin_desc(char *name, SgUShort nip, SgUShort nop,
                                                 SgUShort niv, SgUShort nov)
{
  SgSgPluginDesc *desc;

  desc = sg_alloc(sizeof(SgSgPluginDesc));
  desc->name = name;
  desc->iports_n = nip;
  desc->oports_n = nop;
  desc->ivars_n = niv;
  desc->ovars_n = nov;
  desc->flags = 0;

  return (SgPluginDescP)desc;
}

void sg_free_sg_plugin_desc(SgSgPluginDesc *desc)
{
  sg_free(desc);
}

SgPluginDescP sg_find_plugin(SgInt type, char *id)
{
  SgPluginDescP pdesc;

  pdesc.other = NULL;
  switch (type) {
    case SG_PLUGIN_SG:
      pdesc.sg = (SgSgPluginDesc*)sg_btree_find(sg_plugins, id);
      break;
    case SG_PLUGIN_LADSPA:
      pdesc.ladspa = (SgLadspaPluginDesc*)sg_btree_find(ladspa_plugins, id);
  }

  return pdesc;
}

void sg_plugin_register(SgPluginDescP desc, SgInt type)
{
  SgInt r = -1;

  switch (type) {
    case SG_PLUGIN_SG:
      r = sg_btree_insert(sg_plugins, desc.sg->id, desc.other);
      break;
    case SG_PLUGIN_LADSPA:
      r = sg_btree_insert(ladspa_plugins, desc.ladspa->label, desc.other);
  }

  if (r == -1) {
    sg_warning(SGERR_PLUGIN, "Plugin double");
    return;
  }

}










/* end */

