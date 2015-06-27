
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


/* sn5.ladspa.c
   noise spreaded using the frequency of your choice;
   with rectangular, linear, or cossine interpolation
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ladspa.h"

#define SN_IA_FM 0
#define SN_IC_FB0 1
#define SN_IC_FD 2
#define SN_IC_AMP 3
#define SN_IC_INTE 4
#define SN_IC_INST 5
#define SN_IC_INSTL 6
#define SN_IC_REG 7
#define SN_IC_DIST 8
#define SN_IC_MULT 9
#define SN_IC_RIG 10
#define SN_IC_HARMO0 11
#define SN_IC_HARMO1 12
#define SN_IC_HARMO2 13
#define SN_IC_HARMO3 14
#define SN_IC_HARMO4 15
#define SN_IC_HARMO5 16
#define SN_IC_HARMO6 17
#define SN_IC_HARMO7 18
#define SN_IC_HARMO8 19
#define SN_IC_REN 20
#define SN_IC_ECHO 21
#define SN_IC_DELAY 22
#define SN_IC_DECAY 23
#define SN_IC_CEN 24
#define SN_OA_OUT 25

#define NUMPORTS 26
#define NUMHARMOS 9

#define MAXDELAY 250.f

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

LADSPA_Data compute_noise_sample(
  struct SN_BASE *nslm, 
  unsigned int inte,
  LADSPA_Data reg,
  LADSPA_Data dist, 
  LADSPA_Data mult, 
  LADSPA_Data rig,
  LADSPA_Data sr)
{
  register LADSPA_Data r1;
  register LADSPA_Data r2;
  LADSPA_Data r;
  int n = 0;

  switch (inte) {
    case 0: /* rectangular */
      r1 = nslm->a;
      r2 = nslm->sw ? 1.f : -1.f;
      break;
    case 1: /* linear */
      r1 = nslm->a + nslm->p * (nslm->b - nslm->a);
      r2 = (nslm->sw ? 1. - nslm->p : nslm->p) * 2. - 1. ;
      break;
    case 2: /* cossine */
      r1 = nslm->a + (cos(nslm->p * M_PI) - 1.) / -2. * (nslm->b - nslm->a);
      r2 = cos((nslm->sw ? nslm->p : 1. + nslm->p) * M_PI);
      break;
    default:
      r1 = 0.f;
      r2 = 0.f;
  }
  r = r1 * (1.f - reg) + r2 * reg;
  if (r < 0.f) {
    n = 1;
    r *= -1.f;
  }
  r = powf(r, (1.f - dist) * mult);
  if (n) {
    r *= -1.f;
    n = 0;
  }
  r = r * (1.f - rig) + asinf(r) / (M_PI / 2.f) * rig;

  return r;
}

LADSPA_Handle sn_make(const LADSPA_Descriptor *desc, unsigned long sr) {
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

void sn_connect(struct SN_ST *h, unsigned long p, LADSPA_Data *buff) {
  h->ports[p] = buff;
}

void sn_activate(struct SN_ST *h) {
  LADSPA_Data amp = *(h->ports[SN_IC_AMP]);
  unsigned int i = 0;
  struct SN_BASE *nslm;

  for (i = 0; i < 9; i++) {
    nslm = &(h->noise_elems[i]);
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

void sn_run(struct SN_ST *h, unsigned long sc)
{
  LADSPA_Data *ibuff = h->ports[SN_IA_FM];
  LADSPA_Data *obuff = h->ports[SN_OA_OUT];
  LADSPA_Data freq0 = *(h->ports[SN_IC_FB0]) * 2.f;
  LADSPA_Data freqd = *(h->ports[SN_IC_FD]) * 2.f;
  LADSPA_Data amp = *(h->ports[SN_IC_AMP]);
  LADSPA_Data inst = *(h->ports[SN_IC_INST]);
  LADSPA_Data instl = *(h->ports[SN_IC_INSTL]);
  LADSPA_Data reg = *(h->ports[SN_IC_REG]);
  LADSPA_Data dist = *(h->ports[SN_IC_DIST]);
  LADSPA_Data mult = *(h->ports[SN_IC_MULT]);
  LADSPA_Data rig = *(h->ports[SN_IC_RIG]);
  LADSPA_Data **harmos = &(h->ports[SN_IC_HARMO0]);
  register LADSPA_Data harmo;
  LADSPA_Data ren = *(h->ports[SN_IC_REN]);
  unsigned int inte = *(h->ports[SN_IC_INTE]);
  struct SN_BASE *nslm, *nslms = h->noise_elems;
  int echo = *(h->ports[SN_IC_ECHO]);
  LADSPA_Data *echobuff = h->echobuff;
  LADSPA_Data delay;
  unsigned int ebuffsz;
  LADSPA_Data decay;
  int cen = *(h->ports[SN_IC_CEN]);
  double ifreq;
  unsigned long i, k, l;
  LADSPA_Data sum, inval;
  LADSPA_Data sr = h->sr;
  register LADSPA_Data r;

  if (echo) {
    delay = *(h->ports[SN_IC_DELAY]);
    if (delay != h->delay) {
      ebuffsz = delay / 1000.f * sr;
      if (!ebuffsz) ebuffsz = 1;
      h->ebuffsz = ebuffsz;
      echo = 0;
    } else
      ebuffsz = h->ebuffsz;
    decay = *(h->ports[SN_IC_DECAY]);
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
        echobuff[k] = 0.f;
      k = 0;
      echo = 1;
    } else
      k = h->idxecho;
  }

  for (sum = 0, l = 0; l < NUMHARMOS; l++)
    sum += harmos[l][0];

  for (i = 0; i < sc; i++) {
    inval = ibuff[i];

    for (r = 0., l = 0; l < NUMHARMOS; l++)
      r += (harmo = harmos[l][0]) == 0.f ? 0.f : compute_noise_sample(
            &(nslms[l]), inte, reg, dist, mult, rig, sr) * harmo;

    if (sum > 1.f)
      r /= sum;
    r *= 1.f - ren + RANDOM() * ren;
    r *= amp;

    if (echo) {
      r += echobuff[k] * decay;
      echobuff[k] = r;
      k++;
      k %= ebuffsz;
    }

    if (!cen) r = r - 1.f + amp;

    obuff[i] = r;

    for (l = 0; l < 9; l++) {
      nslm = &(nslms[l]);

      ifreq = (freq0 + inval * freqd) * (1<<l);
      nslm->p += ifreq * ((double)inst *
                          (nslm->c + (cos(nslm->pst * M_PI) - 1.) /
                          -2. * (nslm->d - nslm->c) - .5) + 1.) / sr;
      nslm->pst += ifreq / 2. / powf(2., (double)instl) / sr;

      if (nslm->p >= 1.) {
        nslm->a = nslm->b;
        nslm->b = RANDOM() * 2.f - 1.f;
        nslm->sw = !nslm->sw;
        nslm->p = fmod(nslm->p, 1.);
      }
      if (nslm->pst >= 1.) {
        nslm->c = nslm->d;
        nslm->d = RANDOM();
        nslm->pst = fmod(nslm->pst, 1.);
      }
    }
  }

  h->idxecho = k;
}

void sn_run_adding(LADSPA_Handle h, unsigned long sc) {}

void sn_set_run_adding_gain(LADSPA_Handle h, LADSPA_Data gain) {}

void sn_deactivate(LADSPA_Handle h) {}

void sn_cleanup(LADSPA_Handle h) {}

static const char const *sn_ports_names[] = {
  "FM",
  "lower bound(Hz)",
  "delta(Hz)",
  "amplification",
  "interpolation",
  "instability",
  "length of instability",
  "regularity",
  "distortion",
  "multiplication",
  "rigidify",
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
  9983,
  "sn_noise54",
  LADSPA_PROPERTY_REALTIME,
  "AATK - SN Noise 54",
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
//  sn_connect,
//  sn_activate,
//  sn_run,
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

