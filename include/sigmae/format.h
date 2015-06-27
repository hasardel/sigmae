
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
 * format.h
 */

#ifndef SGFORMAT_H
#define SGFORMAT_H

#include <sigmae/base/types.h>

#define SG_FORMAT_LE 64
#define SG_FORMAT_BE 128
#define SG_FORMAT_U8 0
#define SG_FORMAT_S8 1
#define SG_FORMAT_U16 2
#define SG_FORMAT_S16 3
#define SG_FORMAT_U16LE (SG_FORMAT_LE | SG_FORMAT_U16)
#define SG_FORMAT_S16LE (SG_FORMAT_LE | SG_FORMAT_S16)
#define SG_FORMAT_U16BE (SG_FORMAT_BE | SG_FORMAT_U16)
#define SG_FORMAT_S16BE (SG_FORMAT_BE | SG_FORMAT_S16)
#define SG_FORMAT_U24 4
#define SG_FORMAT_S24 5
#define SG_FORMAT_U24LE (SG_FORMAT_LE | SG_FORMAT_U24)
#define SG_FORMAT_S24LE (SG_FORMAT_LE | SG_FORMAT_S24)
#define SG_FORMAT_U24BE (SG_FORMAT_BE | SG_FORMAT_U24)
#define SG_FORMAT_S24BE (SG_FORMAT_BE | SG_FORMAT_S24)
#define SG_FORMAT_U32 6
#define SG_FORMAT_S32 7
#define SG_FORMAT_U32LE (SG_FORMAT_LE | SG_FORMAT_U32)
#define SG_FORMAT_S32LE (SG_FORMAT_LE | SG_FORMAT_S32)
#define SG_FORMAT_U32BE (SG_FORMAT_BE | SG_FORMAT_U32)
#define SG_FORMAT_S32BE (SG_FORMAT_BE | SG_FORMAT_S32)
#define SG_FORMAT_FL32 8
#define SG_FORMAT_FL32LE (SG_FORMAT_LE | SG_FORMAT_FL32)
#define SG_FORMAT_FL32BE (SG_FORMAT_BE | SG_FORMAT_FL32)
#define SG_FORMAT_FL64 9
#define SG_FORMAT_FL64LE (SG_FORMAT_LE | SG_FORMAT_FL64)
#define SG_FORMAT_FL64BE (SG_FORMAT_BE | SG_FORMAT_FL64)
#define SG_FORMAT_UNKNOWN 10

#define SG_FORMAT_DEFAULT SG_FORMAT_S16LE

#define S_LE2BE(s) (s >> 8 | s << 8)
#define S_BE2LE(s) S_LE2BE

#define I_LE2BE(i) (i >> 24 | \
                    (i & 0x00FFOOOO) >> 8 | \
                    (i & 0x00OOFFOO) << 8 | \
                    i << 24)
#define I_BE2LE I_LE2BE

#define B_S2U(b) (b + 0x80)
#define B_U2S B_S2U

#define S_S2U(s) (s + 0x8000)
#define S_U2S S_S2U

#define I_S2U(l) (i + 0x80000000)
#define I_U2S I_S2U

#define F2US(f) (f * 0xFFFF)
#define F2S(f) S_U2S(F2US(f))

#endif

/* end */

