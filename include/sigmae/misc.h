
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
 * gen.h
 */

#ifndef SGMISC_H
#define SGMISC_H

#include <sigmae/base/types.h>

/* sample, frequency, delta[0;1.0], phase */
SgFloat sg_sinusoide(SgFrames s, SgFloat f, SgFloat p);
SgFloat sg_saw(SgFrames s, SgFloat f, SgFloat d, SgFloat p);
SgFloat sg_rect(SgFrames s, SgFloat f, SgFloat d, SgFloat p);
SgFloat sg_harmos(SgFrames s, SgFloat f, SgFloat p, SgInt n);
SgFloat sg_fm(SgFrames s, SgFloat f1, SgFloat f2, SgFloat i);
SgFloat sg_noise();

#endif

/* end */

