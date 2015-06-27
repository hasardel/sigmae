
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
 * error.h
 */

#ifndef SGERROR_H
#define SGERROR_H

#include <sigmae/base/types.h>

#define SGERR_UNKNOWN 0
#define SGERR_IO 1
#define SGERR_ALLOC 2
#define SGERR_INTERN 3
#define SGERR_PLUGIN 4

void sg_error(SgUInt sgerr, char *f, ...);
void sg_warning(SgUInt sgerr, char *f, ...);

#endif

/* end */

