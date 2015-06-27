
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


#include <sigmae/sigmae.h>

/*
struct _SgVarDesc {
  SgULong mode;
  SgVal value;
  SgVal upper_limit;
  SgVal lower_limit;
};

typedef struct {
  char *id;
  char *inps_names, *outs_names, *vars_names;
  SgShort inps_nb, outs_nb, vars_nb;
  SgVarDesc *initvars;
  SgBool needownbuffs;
  SgRunFun _on_run;
  SgConnectFun _on_connect;
  SgChangeFun _on_change;
  SgMakeFun _on_make;
  SgInitFun _on_init;
  SgResetFun _on_reset;
  SgRemoveFun _on_remove;
  char *name;
  char *author;
  char *copyright;
  char *help;
} SgElemPluginDesc;
*/

char *outnames[] = {"out"};
char *varnames[] = {"frequency","shift","phase","amplitude"};
SgVarDesc vds[] = {
  {SG_VAR_TYPE_FLOAT, 440., 0., 100000.},
  {SG_VAR_TYPE_FLOAT, 0., 0., 1.},
  {SG_VAR_TYPE_FLOAT, 0., -3.14, 3.14},
  {SG_VAR_TYPE_FLOAT, 1., -10., 10.}
};

SgElemPluginDesc pdescs[5] = {
  {
    "saw_oscillator",
    NULL,
    outnames,
    varnames,
    0, 1, 4,
    vds,
    FALSE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "Saw oscillator",
    "",
    "",
    ""
  }
};

main ()
{

}

/* end */

