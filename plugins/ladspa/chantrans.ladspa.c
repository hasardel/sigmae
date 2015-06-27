
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

enum CT_PORTS {
  CT_IA_LFT = 0,
  CT_IA_RGT,
  CT_IA_TR,
  CT_OA_LFT,
  CT_OA_RGT
};

#define NUMPORTS (CT_OA_RGT + 1)

struct CT_ST {
  LADSPA_Data *ports[NUMPORTS];
};

static LADSPA_Handle ct_make(const LADSPA_Descriptor *desc, unsigned long sr)
{
  struct CT_ST *h = (struct CT_ST*)malloc(sizeof(struct CT_ST));
  if (!h)
    return NULL;
  return h;
}

static void ct_connect(struct CT_ST *h, unsigned long p, LADSPA_Data *buff)
{
  h->ports[p] = buff;
}

static void ct_activate(struct CT_ST *h) {}

static void ct_run(struct CT_ST *h, unsigned long sc)
{
  LADSPA_Data *ilbuff = h->ports[CT_IA_LFT];
  LADSPA_Data *irbuff = h->ports[CT_IA_RGT];
  LADSPA_Data *itbuff = h->ports[CT_IA_TR];
  LADSPA_Data *olbuff = h->ports[CT_OA_LFT];
  LADSPA_Data *orbuff = h->ports[CT_OA_RGT];
  register LADSPA_Data r1, r2;
  LADSPA_Data cl, cr;
  unsigned int i;

  for (i = 0; i < sc; i++) {
    cl = ilbuff[i];
    cr = irbuff[i];
    r2 = itbuff[i];
    if (r2 > 1.0f)
      r2 = 1.f;
    else if (r2 < -1.0f)
      r2 = -1.f;
    r2 /= 2.f;
    r2 += 0.5f;
    r1 = 1.f;
    r1 -= r2;
    olbuff[i] = r1 * cl + r2 * cr;
    orbuff[i] = r2 * cl + r1 * cr;
  }

}

static void ct_run_adding(LADSPA_Handle h, unsigned long sc) {}

static void ct_set_run_adding_gain(LADSPA_Handle h, LADSPA_Data gain) {}

static void ct_deactivate(LADSPA_Handle h) {}

static void ct_cleanup(LADSPA_Handle h) {}

static const char const *ct_ports_names[] = {
  "left",
  "right",
  "trans",
  "left",
  "right"
};

static const LADSPA_PortDescriptor ct_ports[] = {
  LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO
};

static const LADSPA_PortRangeHint ct_hints[] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

static const LADSPA_Descriptor ct_pl_desc = {
  9980,
  "channel_trans",
  LADSPA_PROPERTY_REALTIME,
  "AATK - Channel Translator",
  "MrAmp",
  "",
  NUMPORTS,
  ct_ports,
  ct_ports_names,
  ct_hints,
  (void*)0,
  ct_make,
  (void (*)(LADSPA_Handle, unsigned long, LADSPA_Data*))ct_connect,
  (void (*)(LADSPA_Handle))ct_activate,
  (void (*)(LADSPA_Handle, unsigned long))ct_run,
  ct_run_adding,
  ct_set_run_adding_gain,
  ct_deactivate,
  ct_cleanup
};

const LADSPA_Descriptor *ladspa_descriptor(unsigned long index)
{
  if (index == 0)
    return &ct_pl_desc;
  return NULL;
}

