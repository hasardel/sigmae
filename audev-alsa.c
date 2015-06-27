
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
 * audev-alsa.c
 */

#include <stdio.h>
#include <alsa/asoundlib.h>

#include <sigmae/base/types.h>
#include <sigmae/mem.h>
#include <sigmae/error.h>
#include <sigmae/audev.h>


typedef struct _SgAlsaDev {
  snd_pcm_t *pcm_hdle;
  SgChar *devname;
  SgInt sr;
  SgUInt chans;
  SgInt fmt;
} SgAlsaDev;


static SgInt get_alsa_format(SgInt sgfmt, SgInt *afmt)
{
  switch (sgfmt) {
    case SG_FORMAT_U8:
      *afmt = SND_PCM_FORMAT_U8;
      break;
    case SG_FORMAT_S8:
      *afmt = SND_PCM_FORMAT_S8;
      break;
    case SG_FORMAT_U16LE:
      *afmt = SND_PCM_FORMAT_U16_LE;
      break;
    case SG_FORMAT_S16LE:
      *afmt = SND_PCM_FORMAT_S16_LE;
      break;
    case SG_FORMAT_U16BE:
      *afmt = SND_PCM_FORMAT_U16_BE;
      break;
    case SG_FORMAT_S16BE:
      *afmt = SND_PCM_FORMAT_S16_BE;
      break;
    case SG_FORMAT_U24LE:
      *afmt = SND_PCM_FORMAT_U24_3LE;
      break;
    case SG_FORMAT_S24LE:
      *afmt = SND_PCM_FORMAT_S24_3LE;
      break;
    case SG_FORMAT_U24BE:
      *afmt = SND_PCM_FORMAT_U24_3BE;
      break;
    case SG_FORMAT_S24BE:
      *afmt = SND_PCM_FORMAT_S24_3BE;
      break;
    case SG_FORMAT_U32LE:
      *afmt = SND_PCM_FORMAT_U32_LE;
      break;
    case SG_FORMAT_S32LE:
      *afmt = SND_PCM_FORMAT_S32_LE;
      break;
    case SG_FORMAT_U32BE:
      *afmt = SND_PCM_FORMAT_U32_BE;
      break;
    case SG_FORMAT_S32BE:
      *afmt = SND_PCM_FORMAT_S32_BE;
      break;
    case SG_FORMAT_FL32LE:
      *afmt = SND_PCM_FORMAT_FLOAT_LE;
      break;
    case SG_FORMAT_FL32BE:
      *afmt = SND_PCM_FORMAT_FLOAT_BE;
      break;
    case SG_FORMAT_FL64LE:
      *afmt = SND_PCM_FORMAT_FLOAT64_LE;
      break;
    case SG_FORMAT_FL64BE:
      *afmt = SND_PCM_FORMAT_FLOAT64_BE;
      break;
    default:
      return -1;
  }
  return 0;
}

static SgInt audev_alsa_close(SgAlsaDev *hdle)
{
  if (snd_pcm_close(hdle->pcm_hdle))
    return -1;
  return 0;
}

static SgInt audev_alsa_write(SgAlsaDev *hdle, void *data, SgSize size)
{
  snd_pcm_sframes_t frames;

  frames = snd_pcm_writei(hdle->pcm_hdle, data, size);
  if (frames < 0)
    frames = snd_pcm_recover(hdle->pcm_hdle, frames, 0);
  if (frames < 0) {
    sg_warning(SGERR_IO, "Writing sound failed.");
    return -1;
  }

  return 0;
}

static SgInt audev_alsa_read(SgAlsaDev *hdle, void *data, SgSize size)
{
  sg_warning(SGERR_UNKNOWN, "Reading mode for alsa devices not implemented!");
  return -1;
}

SgInt sg_audev_alsa_open(SgAuDev *audev, SgInt mode, SgInt fmt,
                                         SgUInt sr, SgUInt chans)
{
  SgAlsaDev *hdle = sg_alloc(sizeof(SgAlsaDev));
  SgInt afmt;

  hdle->devname = "default";

  if (snd_pcm_open(&hdle->pcm_hdle, hdle->devname,
                   SND_PCM_STREAM_PLAYBACK, 0)) {
    sg_warning(SGERR_IO, "Can't open audio device");
    return -1;
  }

  if (get_alsa_format(fmt, &afmt) == -1) {
    sg_warning(SGERR_IO, "Invalid sound format");
    return -1;
  }

  if (snd_pcm_set_params(
       hdle->pcm_hdle,
       afmt,
       SND_PCM_ACCESS_RW_INTERLEAVED,
       1, sr,
       1, 500000)) {
    sg_warning(SGERR_IO, "Can't open audio device");
    return -1;
  }

  hdle->sr = sr;
  hdle->chans = chans;
  hdle->fmt = fmt;

  audev->read = (SgAuDevReadFun)audev_alsa_read;
  audev->write = (SgAuDevWriteFun)audev_alsa_write;
  audev->close = (SgAuDevCloseFun)audev_alsa_close;
  audev->handle = hdle;

  return 0;
}

/* end */

