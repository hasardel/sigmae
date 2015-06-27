
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
 * audev-oss.c
 */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>

#include <sigmae/base/types.h>
#include <sigmae/mem.h>
#include <sigmae/error.h>
#include <sigmae/audev.h>


#define DEVNAME "/dev/dsp"

typedef struct _SgOssDev {
  SgInt fd;
  SgUInt sr;
  SgUInt chans;
  SgInt fmt;
  SgUInt fmtsz;
} SgOssDev;


static SgInt audev_oss_close(SgOssDev *hdle)
{
  SgInt r;

  r = close(hdle->fd);
  if (r)
    return -1;
  return 0;
}

static SgInt audev_oss_write(SgOssDev *hdle, void *data, SgSize size)
{
  if (write(hdle->fd, data, size * hdle->fmtsz) == -1)
    return -1;
  return 0;
}

static SgInt audev_oss_read(SgOssDev *hdle, void *data, SgSize size)
{
  if (read(hdle->fd, data, size * hdle->fmtsz) == -1)
    return -1;
  return 0;
}

SgInt sg_audev_oss_open(SgAuDev *audev, SgInt mode, SgInt fmt,
                                        SgUInt sr, SgUInt chans)
{
  SgOssDev *hdle = sg_alloc(sizeof(SgOssDev));
  SgUInt fmtsz;
  SgInt fd, p;

  switch (mode)
  {
    case SG_AUDEV_MODE_R:
      mode = O_RDONLY;
      break;
    case SG_AUDEV_MODE_W:
      mode = O_WRONLY;
      break;
    case SG_AUDEV_MODE_RW:
      mode = O_RDWR;
      break;
    default:
      return -1;
  }
  fd = open(DEVNAME, mode);
  if (fd == -1) {
    sg_warning(SGERR_IO, "Can't open audio device");
    return -1;
  }

  p = AFMT_S16_LE;
  if (ioctl(fd, SNDCTL_DSP_SETFMT, &p) == -1)
    sg_error(SGERR_IO, "Set sample format");
  fmtsz = 2;
  p = chans;
  if (ioctl(fd, SNDCTL_DSP_CHANNELS, &p) == -1)
    sg_error(SGERR_IO, "Set channels");
  p = sr;
  if (ioctl(fd, SNDCTL_DSP_SPEED, &p) == -1)
    sg_error(SGERR_IO, "Set sample rate");

  hdle->fd = fd;
  hdle->sr = sr;
  hdle->chans = chans;
  hdle->fmt = fmt;
  hdle->fmtsz = fmtsz;

  audev->read = (SgAuDevReadFun)audev_oss_read;
  audev->write = (SgAuDevWriteFun)audev_oss_write;
  audev->close = (SgAuDevCloseFun)audev_oss_close;
  audev->handle = hdle;

  return 0;
}

/* end */

