
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


#include <sigmae/sigmae.h>


SgPortDesc wavsrc_ports[] = {
  {"output",SG_PORT_TYPE_OUTPUT,0,0,0}
};

SgVarDesc wavsrc_vars[] = {
  {"input file",SG_VAR_TYPE_STRING,"",0,0}
};

SgElemPluginDesc wavsrc_desc = {
  "wavsrc",
  1,
  1,
  0,
  wavsrc_ports,
  wavsrc_vars,
  FALSE,
  wavsrc_run,
  wavsrc_connect,
  wavsrc_change,
  wavsrc_make,
  wavsrc_init,
  wavsrc_reset,
  wavsrc_remove,
  "wav src",
  "Hevan",
  "",
  ""
};

SgPortDesc wavsink_ports[] = {
  {"input",SG_PORT_TYPE_INPUT,0,0,0}
};

SgVarDesc wavsink_vars[] = {
  {"output file",SG_VAR_TYPE_STRING,"",0,0}
};

SgElemPluginDesc wavsink_desc = {
  "wavsink",
  1,
  1,
  0,
  wavsink_ports,
  wavsink_vars,
  FALSE,
  wavsink_run,
  wavsink_connect,
  wavsink_change,
  wavsink_make,
  wavsink_init,
  wavsink_reset,
  wavsink_remove,
  "wav sink",
  "Hevan",
  "",
  ""
};

SgPluginDescP get_desc (SgULong i)
{
  if (i == 0)
    return &wavsrc_desc;
  else if (i == 1)
    return &wavsink_desc;
}

SgPluginsInfo sg_plugins_info = {
  SG_PLUGIN_ELEM,
  "wavfile",
  get_desc,
  "wav file",
  "Hevan",
  "",
  ""
};

