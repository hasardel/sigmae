
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


/* sn55.ladspa.c
   noise spreaded using the frequency of your choice;
   with rectangular, linear, or cossine interpolation
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ladspa.h"

enum SN_PORTS {
  SN_IA_FM = 0,
  SN_IC_FB0,
  SN_IC_FD,
  SN_IC_AMP,
  SN_IC_INTE,
  SN_IC_INST,
  SN_IC_INSTL,
  SN_IC_REG,
  SN_IC_MULT,
  SN_IC_RIG,
  SN_IC_NUMHARMOS,
  SN_IC_HARMO0,
  SN_IC_HARMO1,
  SN_IC_HARMO2,
  SN_IC_HARMO3,
  SN_IC_HARMO4,
  SN_IC_HARMO5,
  SN_IC_HARMO6,
  SN_IC_HARMO7,
  SN_IC_HARMO8,
  SN_IC_REN,
  SN_IC_ECHO,
  SN_IC_DELAY,
  SN_IC_DECAY,
  SN_IC_CEN,
  SN_OA_OUT
};

#define NUMPORTS (SN_OA_OUT + 1)
#define NUMHARMOS 9

#define MAXDELAY 150.f

#define RANDOM() (random() / (float)RAND_MAX)

struct SN_BASE {
  LADSPA_Data a;
  LADSPA_Data b;
  LADSPA_Data c;
  LADSPA_Data d;
  int sw;
  double p;
  double pst;
};

struct SN_ST {
  LADSPA_Data *ports[NUMPORTS];
  struct SN_BASE noise_elems[NUMHARMOS];
  int use_echo;
  LADSPA_Data *echobuff;
  LADSPA_Data delay;
  unsigned int maxebuffsz;
  unsigned int ebuffsz;
  unsigned int idxecho;
  LADSPA_Data decay;
  LADSPA_Data sr;
};

static LADSPA_Handle sn_make(const LADSPA_Descriptor *desc, unsigned long sr) {
  struct SN_ST *h = (struct SN_ST*)malloc(sizeof(struct SN_ST));
  if (!h)
    return NULL;
  h->maxebuffsz = MAXDELAY / 1000.f * sr;
  h->echobuff = (LADSPA_Data*)malloc(h->maxebuffsz * sizeof(float));
  if (!h->echobuff) {
    free(h);
    return NULL;
  }
  h->sr = sr;
  return h;
}

static void sn_connect(struct SN_ST *h, unsigned long p, LADSPA_Data *buff) {
  h->ports[p] = buff;
}

static void sn_activate(struct SN_ST *h) {
  LADSPA_Data amp = *h->ports[SN_IC_AMP];
  unsigned int i = 0;
  struct SN_BASE *nslm;

  for (i = 0; i < NUMHARMOS; i++) {
    nslm = &h->noise_elems[i];
    nslm->a = RANDOM() * 2.f - 1.f;
    nslm->b = RANDOM() * 2.f - 1.f;
    nslm->c = RANDOM();
    nslm->d = RANDOM();
    nslm->sw = 1;
    nslm->p = 0.f;
    nslm->pst = 0.f;
  }
  h->use_echo = 0;
}

static float compute_harmo (struct SN_BASE *nslm, LADSPA_Data sr,
                            LADSPA_Data harmo, int l,
                            int inte, LADSPA_Data inval,
                            LADSPA_Data freq0, LADSPA_Data freqd,
                            LADSPA_Data inst, LADSPA_Data instl,
                            LADSPA_Data reg, LADSPA_Data rig,
                            LADSPA_Data mult)
{
  LADSPA_Data a, b, c, d;
  int sw;
  double p;
  double pst;
  register LADSPA_Data r1, r2;
  double r3;

  a = nslm->a;
  b = nslm->b;
  c = nslm->c;
  d = nslm->d;
  sw = nslm->sw;
  p = nslm->p;
  pst = nslm->pst;

  if (harmo != 0.f) {
    switch (inte) {
      case 0: /* rectangular */
        r1 = a;
        r2 = sw ? 1.f : -1.f;
        break;
      case 1: /* linear */
        r1 = a + p * (b - a);
        r2 = (sw ? 1. - p : p) * 2. - 1. ;
        break;
      case 2: /* cossine */
        r1 = a + (cos(p * M_PI) - 1.) / -2. * (b - a);
        r2 = cos((sw ? p : 1. + p) * M_PI);
        break;
      default:
        r1 = 0.f;
        r2 = 0.f;
    }
    r1 = r1 * (1.f - reg) + r2 * reg;
    if (r1 < 0.f)
      r1 = powf(r1 * -1.f, mult) * -1.f;
    else
      r1 = powf(r1, mult);
    r1 = (r1 * (1.f - rig) + asinf(r1) / (M_PI / 2.f) * rig) * harmo;
  }

  r3 = (freq0 + inval * freqd) * (1<<l);
  p += r3 * ((double)inst *
             (c + (cos(pst * M_PI) - 1.) /
              -2. * (d - c) - .5) + 1.) / sr;
  pst += r3 / 2. / powf(2., (double)instl) / sr;

  if (p >= 1.) {
    nslm->a = b;
    nslm->b = RANDOM() * 2.f - 1.f;
    nslm->sw = !sw;
    p -= (int)p;
    nslm->p = p < 0. ? p++ : p;
  }
  if (pst >= 1.) {
    nslm->c = d;
    nslm->d = RANDOM();
    pst -= (int)pst;
    nslm->pst = pst < 0. ? pst++ : pst;
  }

  return r1;
}

static void sn_run(struct SN_ST *h, unsigned long sc)
{
  struct SN_BASE *nslms = h->noise_elems;
  LADSPA_Data *ibuff = h->ports[SN_IA_FM];
  LADSPA_Data *obuff = h->ports[SN_OA_OUT];
  LADSPA_Data freq0 = *h->ports[SN_IC_FB0] * 2.f;
  LADSPA_Data freqd = *h->ports[SN_IC_FD] * 2.f;
  LADSPA_Data amp = *h->ports[SN_IC_AMP];
  LADSPA_Data inst = *h->ports[SN_IC_INST];
  LADSPA_Data instl = *h->ports[SN_IC_INSTL];
  LADSPA_Data reg = *h->ports[SN_IC_REG];
  LADSPA_Data mult = *h->ports[SN_IC_MULT];
  LADSPA_Data rig = *h->ports[SN_IC_RIG];
  LADSPA_Data **harmos = &h->ports[SN_IC_HARMO0], harmo;
  LADSPA_Data ren = *h->ports[SN_IC_REN], rren = 1.f - ren;
  unsigned int inte = *h->ports[SN_IC_INTE];
  unsigned int numharmos = *h->ports[SN_IC_NUMHARMOS];
  int echo = *h->ports[SN_IC_ECHO];
  LADSPA_Data delay;
  unsigned int ebuffsz;
  LADSPA_Data decay;
  int cen = *h->ports[SN_IC_CEN];
  unsigned long i, k, l;
  LADSPA_Data sum, inval;
  register LADSPA_Data r;

  if (echo) {
    delay = *h->ports[SN_IC_DELAY];
    if (delay != h->delay) {
      ebuffsz = delay / 1000.f * h->sr;
      if (!ebuffsz) ebuffsz = 1;
      h->ebuffsz = ebuffsz;
      echo = 0;
    } else
      ebuffsz = h->ebuffsz;
    decay = *h->ports[SN_IC_DECAY];
    if (decay != h->decay) {
      h->decay = decay;
      echo = 0;
    }
    if (!h->use_echo) {
      h->use_echo = 1;
      echo = 0;
    }
    if (!echo) { /* echo buffer must be reinitialized */
      for (k = 0; k < ebuffsz; k++)
        h->echobuff[k] = 0.f;
      k = 0;
      echo = 1;
    } else
      k = h->idxecho;
  }

  for (sum = 0.f, l = 0; l < numharmos; l++)
    sum += harmos[l][0];

  for (i = 0; i < sc; i++) {
    inval = ibuff[i];

    for (r = 0.f, l = 0; l < numharmos; l++)
      r += compute_harmo(&nslms[l], h->sr, harmo, l, inte, inval,
                         freq0, freqd, inst, instl, reg, rig, mult);

    if (sum > 1.f)
      r /= sum;
    r *= rren + RANDOM() * ren;
    r *= amp;

    if (echo) {
      r += h->echobuff[k] * decay;
      h->echobuff[k] = r;
      k++;
      k %= ebuffsz;
    }

    if (!cen) r = r - 1.f + amp;

    obuff[i] = r;
  }

  h->idxecho = k;
}

static void sn_run_adding(LADSPA_Handle h, unsigned long sc)
{
}

static void sn_set_run_adding_gain(LADSPA_Handle h, LADSPA_Data gain)
{
}

static void sn_deactivate(LADSPA_Handle h)
{
}

static void sn_cleanup(LADSPA_Handle h)
{
  if (h)
    free(h);
}

static const char const *sn_ports_names[] = {
  "FM",
  "lower bound(Hz)",
  "delta(Hz)",
  "amplification",
  "interpolation",
  "instability",
  "length of instability",
  "regularity",
  "multiplication",
  "rigidify",
  "harmonics",
  "harmonic 0",
  "harmonic 1",
  "harmonic 2",
  "harmonic 3",
  "harmonic 4",
  "harmonic 5",
  "harmonic 6",
  "harmonic 7",
  "harmonic 8",
  "renoising",
  "echo",
  "delay(ms)",
  "reduction",
  "centered",
  "output"
};

static const LADSPA_PortDescriptor sn_ports[] = {
  LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL,
  LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO
};

static const LADSPA_PortRangeHint sn_hints[] = {
  {0, 0, 0},
/* basic parameters */
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_LOGARITHMIC |
   LADSPA_HINT_SAMPLE_RATE |
   LADSPA_HINT_DEFAULT_440,
   0.f, .5f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_LOGARITHMIC |
   LADSPA_HINT_SAMPLE_RATE |
   LADSPA_HINT_DEFAULT_0,
   0.f, .5f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_1,
   0.f, 2.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_INTEGER |
   LADSPA_HINT_DEFAULT_0,
   0.f, 2.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 50.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_1,
   -10.f, 10.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_LOGARITHMIC |
   LADSPA_HINT_DEFAULT_1,
   0.f, 50.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
/* numbre of harmonics */
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_INTEGER |
   LADSPA_HINT_DEFAULT_1,
   1.f, 9.f},
/* harmonics */
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_1,
   0.f, 1.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
/* renoising */
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
/* echo */
  {LADSPA_HINT_TOGGLED |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, MAXDELAY},
  {LADSPA_HINT_BOUNDED_BELOW |
   LADSPA_HINT_BOUNDED_ABOVE |
   LADSPA_HINT_DEFAULT_0,
   0.f, 1.f},
  {LADSPA_HINT_TOGGLED |
   LADSPA_HINT_DEFAULT_1,
   0.f, 1.f},
  {0, 0, 0}
};

static const LADSPA_Descriptor sn_pl_desc = {
  9982,
  "sn_noise55",
//  LADSPA_PROPERTY_REALTIME,
  0,
  "AATK - SN Noise 55",
  "MrAmp",
  "",
  NUMPORTS,
  sn_ports,
  sn_ports_names,
  sn_hints,
  (void*)0,
  sn_make,
  (void (*)(LADSPA_Handle, unsigned long, LADSPA_Data*))sn_connect,
  (void (*)(LADSPA_Handle))sn_activate,
  (void (*)(LADSPA_Handle, unsigned long))sn_run,
  sn_run_adding,
  sn_set_run_adding_gain,
  sn_deactivate,
  sn_cleanup
};

const LADSPA_Descriptor *ladspa_descriptor(unsigned long index)
{
  if (index == 0)
    return &sn_pl_desc;
  return NULL;
}

