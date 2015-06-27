
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
 * error.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <sigmae/error.h>


char const *sgerrs[] =
{
  "Unknown",
  "IO operation failed",
  "Allocation failed",
  "Intern",
  "PlugIn"
};

const SgUInt sgerrs_len = sizeof(sgerrs);


void sg_error(SgUInt sgerr, char *f, ...)
{
  va_list args;
  va_start(args, f);
  if (sgerr >= sgerrs_len)
    sgerr = SGERR_UNKNOWN;
  fprintf(stderr, "Error: %s", sgerrs[sgerr]);
  if (f) {
    fprintf(stderr, ": ");
    vfprintf(stderr, f, args);
  }
  fprintf(stderr, "\n");
  va_end(args);
  exit(-1);
}

void sg_warning(SgUInt sgerr, char *f, ...)
{
  va_list args;
  va_start(args, f);
  if (sgerr >= sgerrs_len)
    sgerr = SGERR_UNKNOWN;
  fprintf(stderr, "Warning: %s", sgerrs[sgerr]);
  if (f) {
    fprintf(stderr, ": ");
    vfprintf(stderr, f, args);
  }
  fprintf(stderr, "\n");
  va_end(args);
}

/* end */

