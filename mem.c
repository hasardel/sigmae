
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
 * mem.c
 */

#include <stdlib.h>

#include <sigmae/mem.h>
#include <sigmae/error.h>


void *sg_alloc(SgSize size)
{
  register void *addr = NULL;

  if ((addr = malloc(size)) == NULL)
    sg_error(SGERR_ALLOC, NULL);

  return addr;
}

void *sg_calloc(SgSize nmemb, SgSize size)
{
  register void *addr = NULL;

  if ((addr = calloc(nmemb, size)) == NULL)
    sg_error(SGERR_ALLOC, NULL);

  return addr;
}

void *sg_realloc(void *addr, SgSize size)
{
  if (!addr)
    addr = sg_alloc(size);
  else if (!(addr = realloc(addr, size)))
    sg_error(SGERR_ALLOC, NULL);

  return addr;
}

void sg_free(void *addr)
{
  free(addr);
}

/* end */

