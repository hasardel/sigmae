
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

enum NM_PORTS {
  NM_IA_INP = 0,
  NM_IC_MAX,
  NM_OA_OUT
};

#define NUMPORTS (NM_OA_OUT + 1)

struct NM_ST {
  LADSPA_Data *ports[NUMPORTS];
};

static LADSPA_Handle nm_make(const LADSPA_Descriptor *desc, unsigned long sr)
{
  struct NM_ST *h = (struct NM_ST*)malloc(sizeof(struct NM_ST));
  if (!h)
    return NULL;
  return h;
}

static void nm_connect(struct NM_ST *h, unsigned long p, LADSPA_Data *buff)
{
  h->ports[p] = buff;
}

static void nm_activate(struct NM_ST *h) {}

static void nm_run(struct NM_ST *h, unsigned long sc)
{
  LADSPA_Data *ibuff = h->ports[NM_IA_INP];
  LADSPA_Data *obuff = h->ports[NM_OA_OUT];
  LADSPA_Data max = *h->ports[NM_IC_MAX];
  register LADSPA_Data r, imax = max * -1.f;
  unsigned int i;

  for (i = 0; i < sc; i++)
    obuff[i] = (r = ibuff[i]) < imax ? imax : r > max ? max : r;

}

static void nm_run_adding(LADSPA_Handle h, unsigned long sc)
{
}

static void nm_set_run_adding_gain(LADSPA_Handle h, LADSPA_Data gain)
{
}

static void nm_deactivate(LADSPA_Handle h)
{
}

static void nm_cleanup(LADSPA_Handle h)
{
  free((void*)h);
}

static const char const *nm_ports_names[] = {
  "in",
  "max",
  "out"
};

static const LADSPA_PortDescriptor nm_ports[] = {
  LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO
};

static const LADSPA_PortRangeHint nm_hints[] = {
  {0, 0, 0},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_1,
   0., 1.},
  {0, 0, 0}
};

static const LADSPA_Descriptor nm_pl_desc = {
  9979,
  "normalizer",
  LADSPA_PROPERTY_REALTIME,
  "AATK - Normalizer",
  "MrAmp",
  "",
  NUMPORTS,
  nm_ports,
  nm_ports_names,
  nm_hints,
  (void*)0,
  nm_make,
  (void (*)(LADSPA_Handle, unsigned long, LADSPA_Data*))nm_connect,
  (void (*)(LADSPA_Handle))nm_activate,
  (void (*)(LADSPA_Handle, unsigned long))nm_run,
  nm_run_adding,
  nm_set_run_adding_gain,
  nm_deactivate,
  nm_cleanup
};

const LADSPA_Descriptor *ladspa_descriptor(unsigned long index)
{
  if (index == 0)
    return &nm_pl_desc;
  return NULL;
}

