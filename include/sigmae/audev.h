
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
 * audev.h
 */

#ifndef SGAUDEV_H
#define SGAUDEV_H

#include <sigmae/base/types.h>
#include <sigmae/format.h>

#define SG_AUDEV_MODE_R 1
#define SG_AUDEV_MODE_W 2
#define SG_AUDEV_MODE_RW 3

#define SG_AUDEV_TYPE_OSS 0
#define SG_AUDEV_TYPE_ALSA 1

typedef SgInt (*SgAuDevReadFun)(void *handle, void* data, SgSize size);
typedef SgInt (*SgAuDevWriteFun)(void *handle, void* data, SgSize size);
typedef SgInt (*SgAuDevCloseFun)(void *handle);

typedef struct _SgAuDev {
  SgInt type;
  SgInt mode;
  SgAuDevReadFun read;
  SgAuDevWriteFun write;
  SgAuDevCloseFun close;
  void *handle;
} SgAuDev;

/* open the audio device of type and mode defined by
   SG_AUDEV_TYPE_* and SG_AUDEV_MODE_* */
SgInt sg_audev_open(SgAuDev *audev, SgInt type, SgInt mode, SgInt format,
                                    SgUInt samplerate, SgUInt channels);

#endif

/* end */

