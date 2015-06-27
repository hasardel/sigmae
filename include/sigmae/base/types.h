
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
 * base/types.h
 */

#ifndef SGBASETYPES_H
#define SGBASETYPES_H

#include <sys/types.h>

typedef float SgFloat;
typedef float SgSingle;
typedef double SgDouble;

typedef unsigned short SgUShort;
typedef short SgShort;

typedef unsigned int SgUInt;
typedef int SgInt;

typedef unsigned long SgULong;
typedef long SgLong;

typedef char SgChar;

typedef char SgByte;

typedef char SgBool;
#define TRUE 1
#define FALSE 0

typedef unsigned int SgSize;

typedef int64_t SgFrames;

#define POINTER_SIZE (sizeof(void*))
#ifndef NULL
#define NULL ((void*)0)
#endif

//typedef long long SgVal;

#define SGVFL(v) ((SgFloat)(v))
#define SGVSF(v) ((SgSingle)(v))
#define SGVDF(v) ((SgDouble)(v))
#define SGVUS(v) ((SgUShort)(v))
#define SGVSS(v) ((SgShort)(v))
#define SGVUI(v) ((SgUInt)(v))
#define SGVSI(v) ((SgInt)(v))
#define SGVUL(v) ((SgULong)(v))
#define SGVSL(v) ((SgULong)(v))
#define SGVBL(v) ((SgBool)(v))
#define SGVST(v) ((SgChar*)(v))

typedef union _SgVal
{
  SgFloat fl;
  SgSingle sf;
  SgDouble df;
  SgUShort us;
  SgShort ss;
  SgUInt ui;
  SgInt si;
  SgULong ul;
  SgLong sl;
  SgBool bl;
  SgChar *st;
  union _SgVal *ot;
} SgVal;

#endif

/* end */

