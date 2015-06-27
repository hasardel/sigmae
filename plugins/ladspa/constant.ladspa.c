
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

enum CONS_PORTS {
  CONS_IA_INP = 0,
  CONS_IC_CONS,
  CONS_OA_OUT
};

#define NUMPORTS (CONS_OA_OUT + 1)

struct CONS_ST {
  LADSPA_Data *ports[NUMPORTS];
};

static LADSPA_Handle cons_make(const LADSPA_Descriptor *desc, unsigned long sr)
{
  struct CONS_ST *h = (struct CONS_ST*)malloc(sizeof(struct CONS_ST));
  if (!h)
    return NULL;
  return h;
}

static void cons_connect(struct CONS_ST *h, unsigned long p, LADSPA_Data *buff)
{
  h->ports[p] = buff;
}

static void cons_activate(struct CONS_ST *h) {}

static void cons_run(struct CONS_ST *h, unsigned long sc)
{
  LADSPA_Data *ibuff = h->ports[CONS_IA_INP];
  LADSPA_Data *obuff = h->ports[CONS_OA_OUT];
  LADSPA_Data cons = *h->ports[CONS_IC_CONS];
  unsigned int i;

  for (i = 0; i < sc; i++)
    obuff[i] = cons;

}

static void cons_run_adding(LADSPA_Handle h, unsigned long sc)
{
}

static void cons_set_run_adding_gain(LADSPA_Handle h, LADSPA_Data gain)
{
}

static void cons_deactivate(LADSPA_Handle h)
{
}

static void cons_cleanup(LADSPA_Handle h)
{
  free((void*)h);
}

static const char const *cons_ports_names[] = {
  "in",
  "constant",
  "out"
};

static const LADSPA_PortDescriptor cons_ports[] = {
  LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO
};

static const LADSPA_PortRangeHint cons_hints[] = {
  {0, 0, 0},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   -10., 10.},
  {0, 0, 0}
};

static const LADSPA_Descriptor cons_pl_desc = {
  9977,
  "constant",
  LADSPA_PROPERTY_REALTIME,
  "AATK - Constant",
  "MrAmp",
  "",
  NUMPORTS,
  cons_ports,
  cons_ports_names,
  cons_hints,
  (void*)0,
  cons_make,
  (void (*)(LADSPA_Handle, unsigned long, LADSPA_Data*))cons_connect,
  (void (*)(LADSPA_Handle))cons_activate,
  (void (*)(LADSPA_Handle, unsigned long))cons_run,
  cons_run_adding,
  cons_set_run_adding_gain,
  cons_deactivate,
  cons_cleanup
};

const LADSPA_Descriptor *ladspa_descriptor(unsigned long index)
{
  if (index == 0)
    return &cons_pl_desc;
  return NULL;
}

