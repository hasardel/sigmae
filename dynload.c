
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
 * dynload.h
 */

#include <dlfcn.h>


SgDLib sg_dl_open(SgChar *path)
{
  SgDLib dl = dlopen(path, RTLD_LAZY);
  if (!dl)
    sg_warning(SGERR_INTERN, "Failed to open dynamic library \"%s\": %d\n",
                             path, dlerror());
  return dl;
}

SgDLProc sg_dl_loadsym(SgDlib dl, SgChar *sym)
{
  SgDLProc p = dlsym(dl, sym);
  SgChar *mess = dlerror();
  if (!mess)
    return dl;
  sg_warning(SGERR_INTERN, "Failed to load dynamic library symbol \"%s\": %s\n",
                           sym, mess);
  return NULL;
}

void sg_dl_close(SgDLib dl)
{
  dlclose(dl);
}




/* end */

