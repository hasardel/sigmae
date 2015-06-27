
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


/* chantrans.ladspa.c */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ladspa.h"

enum CNM_PORTS {
  CNM_IA_INP = 0,
  CNM_IA_MAX,
  CNM_OA_OUT
};

#define NUMPORTS (CNM_OA_OUT + 1)

struct CNM_ST {
  LADSPA_Data *ports[NUMPORTS];
};

static LADSPA_Handle cnm_make(const LADSPA_Descriptor *desc, unsigned long sr)
{
  struct CNM_ST *h = (struct CNM_ST*)malloc(sizeof(struct CNM_ST));
  if (!h)
    return NULL;
  return h;
}

static void cnm_connect(struct CNM_ST *h, unsigned long p, LADSPA_Data *buff)
{
  h->ports[p] = buff;
}

static void cnm_activate(struct CNM_ST *h) {}

static void cnm_run(struct CNM_ST *h, unsigned long sc)
{
  LADSPA_Data *ibuff = h->ports[CNM_IA_INP];
  LADSPA_Data *imbuff = h->ports[CNM_IA_MAX];
  LADSPA_Data *obuff = h->ports[CNM_OA_OUT];
  register LADSPA_Data r, max, min;
  unsigned int i;

  for (i = 0; i < sc; i++)
    obuff[i] = (r = ibuff[i]) < (min = (max = imbuff[i]) * -1.f) ?
               min : r > max ? max : r;

}

static void cnm_run_adding(LADSPA_Handle h, unsigned long sc)
{
}

static void cnm_set_run_adding_gain(LADSPA_Handle h, LADSPA_Data gain)
{
}

static void cnm_deactivate(LADSPA_Handle h)
{
}

static void cnm_cleanup(LADSPA_Handle h)
{
  free((void*)h);
}

static const char const *cnm_ports_names[] = {
  "in",
  "max",
  "out"
};

static const LADSPA_PortDescriptor cnm_ports[] = {
  LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO
};

static const LADSPA_PortRangeHint cnm_hints[] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

static const LADSPA_Descriptor cnm_pl_desc = {
  9978,
  "controlled_normalizer",
  LADSPA_PROPERTY_REALTIME,
  "AATK - Controlled Normalizer",
  "MrAmp",
  "",
  NUMPORTS,
  cnm_ports,
  cnm_ports_names,
  cnm_hints,
  (void*)0,
  cnm_make,
  (void (*)(LADSPA_Handle, unsigned long, LADSPA_Data*))cnm_connect,
  (void (*)(LADSPA_Handle))cnm_activate,
  (void (*)(LADSPA_Handle, unsigned long))cnm_run,
  cnm_run_adding,
  cnm_set_run_adding_gain,
  cnm_deactivate,
  cnm_cleanup
};

const LADSPA_Descriptor *ladspa_descriptor(unsigned long index)
{
  if (index == 0)
    return &cnm_pl_desc;
  return NULL;
}

