
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
 * sgladspa.h
 */

#ifndef SGLADSPA_H
#define SGLADSPA_H

#include "ladspa.h"

#include <sigmae/base/types.h>
#include <sigmae/types.h>

#define sg_ladspa_is_rt LADSPA_IS_REALTIME
#define sg_ladspa_is_inplace_broken LADSPA_IS_INPLACE_BROKEN
#define sg_ladspa_is_hard_rt_capable LADSPA_IS_HARD_RT_CAPABLE

#define sg_ladspa_port_is_input LADSPA_IS_PORT_INPUT
#define sg_ladspa_port_is_output LADSPA_IS_PORT_OUTPUT
#define sg_ladspa_port_is_control LADSPA_IS_PORT_CONTROL
#define sg_ladspa_port_is_audio LADSPA_IS_PORT_AUDIO

#define sg_ladspa_hint_is_bounded_below LADSPA_IS_HINT_BOUNDED_BELOW
#define sg_ladspa_hint_is_bounded_above LADSPA_IS_HINT_BOUNDED_ABOVE
#define sg_ladspa_hint_is_toggled LADSPA_IS_HINT_TOGGLED
#define sg_ladspa_hint_is_sr LADSPA_IS_HINT_SAMPLE_RATE
#define sg_ladspa_hint_is_log LADSPA_IS_HINT_LOGARITHMIC
#define sg_ladspa_hint_is_int LADSPA_IS_HINT_INTEGER
#define sg_ladspa_hint_has_default LADSPA_IS_HINT_HAS_DEFAULT
#define sg_ladspa_hint_is_default_min LADSPA_IS_HINT_DEFAULT_MINIMUM
#define sg_ladspa_hint_is_default_low LADSPA_IS_HINT_DEFAULT_LOW
#define sg_ladspa_hint_is_default_middle LADSPA_IS_HINT_DEFAULT_MIDDLE
#define sg_ladspa_hint_is_default_high LADSPA_IS_HINT_DEFAULT_HIGH
#define sg_ladspa_hint_is_default_max LADSPA_IS_HINT_DEFAULT_MAXIMUM
#define sg_ladspa_hint_is_default_0 LADSPA_IS_HINT_DEFAULT_0
#define sg_ladspa_hint_is_default_1 LADSPA_IS_HINT_DEFAULT_1
#define sg_ladspa_hint_is_default_100 LADSPA_IS_HINT_DEFAULT_100
#define sg_ladspa_hint_is_default_440 LADSPA_IS_HINT_DEFAULT_440

typedef SgSingle SgLadspaData;
typedef SgInt SgLadspaProperties;
typedef SgInt SgLadspaPortDesc;
typedef SgInt SgLadspaPortRangeHintDesc;
typedef struct {
  SgLadspaPortRangeHintDesc hint_desc;
  SgLadspaData lower_bound;
  SgLadspaData upper_bound;
} SgLadspaPortRangeHint;

typedef void *SgLadspaHandle;

typedef SgLadspaHandle (*SgLadspaInstantiateFun)(SgLadspaPluginDesc*, SgULong samplerate);
typedef void (*SgLadspaConnectFun)(SgLadspaHandle, SgULong port, SgLadspaData*);
typedef void (*SgLadspaActivateFun)(SgLadspaHandle);
typedef void (*SgLadspaRunFun)(SgLadspaHandle, SgULong samples);
typedef void (*SgLadspaRunAddingFun)(SgLadspaHandle, SgULong samples);
typedef void (*SgLadspaSetRunAddingGainFun)(SgLadspaHandle, SgLadspaData gain);
typedef void (*SgLadspaDeactivateFun)(SgLadspaHandle);
typedef void (*SgLadspaCleanupFun)(SgLadspaHandle);

struct _SgLadspaPluginDesc {
  SgULong unique_id;
  SgChar *label;
  SgLadspaProperties properties;
  SgChar *name;
  SgChar *maker;
  SgChar *copyright;
  SgULong port_count;
  SgLadspaPortDesc *port_descs;
  SgChar **port_names;
  SgLadspaPortRangeHint *port_range_hints;
  SgChar *implementation_data;
  SgLadspaInstantiateFun instantiate;
  SgLadspaConnectFun connect_port;
  SgLadspaActivateFun activate;
  SgLadspaRunFun run;
  SgLadspaRunAddingFun run_adding;
  SgLadspaSetRunAddingGainFun set_run_adding_gain;
  SgLadspaDeactivateFun deactivate;
  SgLadspaCleanupFun cleanup;
};

typedef SgLadspaPluginDesc *(*SgLadspaPluginDescFun)(SgULong idx);

SgBool sg_ladspa_plugin_check(SgLadspaPluginDesc*);

#endif

/* end */

