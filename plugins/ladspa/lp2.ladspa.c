
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


/* lp.ladspa.c : low pass filter */

#include <stdlib.h>
#include <math.h>
#include "ladspa.h"

#define LP_IA_INP 0
#define LP_IC_FREQ 1
#define LP_OA_OUT 2

typedef struct _LP_ST {
  LADSPA_Data *ports[3];
  LADSPA_Data sr;
  LADSPA_Data z, p, freq;
} LP_ST;


LADSPA_Handle lp_make(const LADSPA_Descriptor *desc, unsigned long sr) {
  LP_ST *lp = (LP_ST*)malloc(sizeof(LP_ST));
  lp->sr = sr;
  return lp;
}

void lp_connect(LADSPA_Handle h, unsigned long p, LADSPA_Data *buff) {
  ((LP_ST*)h)->ports[p] = buff;
}

void lp_activate(LADSPA_Handle h) {
  ((LP_ST*)h)->freq = ((LP_ST*)h)->sr;
  ((LP_ST*)h)->z = 0.f;
}

void lp_run(LADSPA_Handle h, unsigned long sc) {
  LADSPA_Data *ibuff = ((LP_ST*)h)->ports[LP_IA_INP];
  LADSPA_Data *obuff = ((LP_ST*)h)->ports[LP_OA_OUT];
  LADSPA_Data freq = *((LP_ST*)h)->ports[LP_IC_FREQ];
  LADSPA_Data sr = ((LP_ST*)h)->sr;
  register LADSPA_Data z = ((LP_ST*)h)->z;
  LADSPA_Data p, x;
  unsigned long i;

  if (freq != ((LP_ST*)h)->freq) {
    ((LP_ST*)h)->freq = freq;
    x = 2.f * M_PI * freq / sr;
    p = 2.f - cosf(x) - sqrtf(powf(2.f - cos(x), 2.f) - 1.f);
    ((LP_ST*)h)->p = p;
  } else
    p = ((LP_ST*)h)->p;

  for (i = 0; i < sc; i++) {
    z = ibuff[i] * (1.f - p) + z * p;
    obuff[i] = z;
  }

  ((LP_ST*)h)->z = z;
}

void lp_run_adding(LADSPA_Handle h, unsigned long sc) {}

void lp_set_run_adding_gain(LADSPA_Handle h, LADSPA_Data gain) {}

void lp_deactivate(LADSPA_Handle h) {}

void lp_cleanup(LADSPA_Handle h) {}

static const char const *lp_ports_names[] = {
  "input",
  "frequency",
  "output"
};

static const LADSPA_PortDescriptor lp_ports[] = {
  LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO
};

static const LADSPA_PortRangeHint lp_hints[] = {
  {0,0,0},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_1,
   0.f, 44100.f},
  {0,0,0}
};

static const LADSPA_Descriptor lp_pl_desc = {
  9987,
  "lowpass2",
  LADSPA_PROPERTY_REALTIME |
  LADSPA_PROPERTY_HARD_RT_CAPABLE,
  "AATK - Low pass filter 2",
  "MrAmp",
  "",
  3,
  lp_ports,
  lp_ports_names,
  lp_hints,
  (void*)0,
  lp_make,
  lp_connect,
  lp_activate,
  lp_run,
  lp_run_adding,
  lp_set_run_adding_gain,
  lp_deactivate,
  lp_cleanup
};

const LADSPA_Descriptor *ladspa_descriptor(unsigned long index)
{
  if (index == 0)
    return &lp_pl_desc;
  return NULL;
}
