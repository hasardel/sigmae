
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
 * core.h
 */

#ifndef SGCORE_H
#define SGCORE_H

#include <sigmae/base/types.h>
#include <sigmae/mod.h>
#include <sigmae/elem.h>

void sg_run_mod(SgMod *mod, SgFrames sample, SgSize blocks);
void sg_run_elem(SgElem *elem, SgFrames sample);

#endif

/* end */

