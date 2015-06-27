
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
 * misc.c
 */

#include <stdlib.h>

#include <sigmae/maths.h>
#include <sigmae/base/types.h>


SgFloat sg_sinusoide(SgFrames s, SgFloat f, SgFloat p)
{
  SgFloat x = (s / 44100.0 + p) * 2.0 * M_PI * f;
  return sin(x);
}

inline SgFloat sg_sin(SgFloat sr, SgFloat *p, SgFloat f)
{
  register SgFloat p2 = *p;
  SgFloat x = sin(p2);
  p2 += f / sr * TWO_PI;
  *p = ffmodf(p2, TWO_PI);
  return x;
}

SgFloat sg_saw(SgFrames s, SgFloat f, SgFloat d, SgFloat p)
{
  SgFloat x = s / 44100.0 * f + p / 2.0 / M_PI;
  x = x - (SgULong)x;
  if (x < d)
    x = (x / d);
  else
    x = (1.0 - x) / (1.0 - d);
  return  x * 2.0 - 1.0;
}

SgFloat sg_rect(SgFrames s, SgFloat f, SgFloat d, SgFloat p)
{
  SgFloat x = s / 44100.0 * f + p / 2.0 / M_PI;
  x = x - (SgULong)x;
  if (x < d)
    return 1.0;
  return -1.0;
}

SgFloat sg_harmos(SgFrames s, SgFloat f, SgFloat p, SgInt n)
{
  SgFloat r = 0.0;
  SgFloat m = 1.0 / n;
  for (;n;n--) {
    r += sg_sinusoide(s, f, p) * m;
    f /= 2.0;
  }
  return r;
}

SgFloat sg_fm(SgFrames s, SgFloat f1, SgFloat f2, SgFloat i)
{
  SgFloat x = s / 44100.0;
  x = sin(x * 2.0 * M_PI * f1 + i * sin(x * 2.0 * M_PI * f2));
  return x;
}

SgFloat noise()
{
  return random() / (float)RAND_MAX;
}

unsigned int mrand()
{
  static unsigned int r = 9999;
  r = r * 171259681 + 164327311;
  return r;
}

/* end */

