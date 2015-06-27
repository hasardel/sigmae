
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
 * audev.c
 */

#include <stdio.h>

#include <sigmae/mem.h>
#include <sigmae/error.h>
#include <sigmae/audev.h>

#include "audev-oss.h"
#include "audev-alsa.h"


SgInt sg_audev_open(SgAuDev *audev, SgInt type,
                    SgInt mode, SgInt fmt,
                    SgUInt sr, SgUInt chans)
{

  switch (type) {
    case SG_AUDEV_TYPE_ALSA:
      if (sg_audev_alsa_open(audev, mode, fmt, sr, chans) == -1)
        return -1;
      break;
    case SG_AUDEV_TYPE_OSS:
      if (sg_audev_oss_open(audev, mode, fmt, sr, chans) == -1)
        return -1;
      break;
    default:
      return -1;
  }
  if (!(mode & SG_AUDEV_MODE_R))
    audev->read = NULL;
  if (!(mode & SG_AUDEV_MODE_W))
    audev->write = NULL;

  audev->type = type;
  audev->mode = mode;

  return 0;
}

/* end */
