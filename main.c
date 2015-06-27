
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


#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>

#include <sigmae/maths.h>
#include <sigmae/sigmae.h>


/**********************************************************************/

void on_run0(SgElem *elem, SgFrames s, SgSize ns)
{
  SgSize i = 0;
  SgFloat *obuff = elem->obuffs[0];
  SgVal *vars = elem->vars;
  SgFloat f = vars[0].fl;
  SgFloat d = vars[1].fl;
  SgFloat p = vars[2].fl;
  SgFloat a = vars[3].fl;
  while (i < ns)
  {
    obuff[i] = sg_saw(s + i, f, d, p) * a;
    i++;
  }
}

void on_connect0(SgElem *elem, SgUShort pnb) {}
void on_change0(SgElem *elem, SgUShort varn) {}
void on_make0(SgElem *elem) {}
void on_init0(SgElem *elem) {}
void on_reset0(SgElem *elem) {}
void on_remove0(SgElem *elem) {}

/**********************************************************************/

static SgAuDev audev;
static SgInt outffd;

void on_run1(SgElem *elem, SgFrames s, SgSize ns)
{
  SgFloat *ibuff = elem->ibuffs[0];
  SgSize i;
  SgUInt r;
  for(i=0;i<ns;i++)
    ((SgShort*)(elem->data))[i] = (SgUShort)((ibuff[i] + 1.f) / 2.f *
                                             65535.f) + 0x8000;
  audev.write(audev.handle, ibuff, ns);
  r = write(outffd, elem->data, sizeof(SgShort) * ns);
}

void on_init1(SgElem *elem)
{
  outffd = open("sgout_S16LE.raw",
                O_WRONLY | O_CREAT | O_TRUNC,
                S_IRUSR | S_IWUSR | S_IRGRP);
  if (outffd == -1)
    sg_error(SGERR_IO, NULL);
  if (sg_audev_open(&audev, SG_AUDEV_TYPE_ALSA,
                    SG_AUDEV_MODE_W, SG_FORMAT_FL32LE, 44100, 1) == -1)
    exit(-1);
  elem->data = sg_alloc(sizeof(SgShort) * elem->_mod->buffsize);
}

void on_remove1(SgElem *elem)
{
  close(outffd);
  audev.close(audev.handle);
  sg_free((void*)elem->data);
}

/**********************************************************************/

#define VAR_WIDTH 0
#define VAR_HEIGHT 1

struct term_data {
  SgUInt w, h, sz;
  SgUInt x;
  SgUInt a, b;
  SgFloat cmoy;
  SgUInt n;
  SgChar disp[];
};

void on_run2(SgElem *elem, SgFrames s, SgSize ns)
{
/*
  struct term_data *data = (struct term_data *)elem->data;
  SgFloat *ibuff = elem->ibuffs[0];
  char *disp = data->disp;
  SgUInt w = data->w, h = data->h, sz = data->sz;
  SgUInt x = data->x;
  SgUInt a = data->a, b = data->b;
  SgFloat cmoy = data->cmoy;
  SgUInt i = 0, j, k = 0;
  SgUInt n = data->n;
*/
/*
  if (a >= b) {
    for (; i < ns; i++) {
      n++;
      cmoy += ibuff[i];
      if (n == a) {
        cmoy /= a;
        n = 0;
        for (j = 0; j < h; j++) {
          disp[k + j * w] = ' ';
//  printf("%u ", k+j*w);

        }
        disp[i + (SgUInt)((cmoy + 1.0) / 2.0 * h) * w] = '+';
        cmoy = 0.0;
        k++;
      }
    }
    data->cmoy = cmoy;
  } else {
    for (; i < ns; i++) {
      for (; n; n--) {
      }
    }
    data->x = x;
  }
  data->n = n;
*/
//  printf("\ec%s", disp);
}

void on_init2(SgElem *elem)
{
  SgUInt w = elem->vars[VAR_WIDTH].ui;
  SgUInt h = elem->vars[VAR_HEIGHT].ui;
  struct term_data *data = sg_alloc(sizeof(struct term_data) + w * h + 1);
  SgUInt bs = sg_elem_get_buffsize(elem);

  elem->data = (void*)data;
  data->w = w;
  data->h = h;
  data->sz = w * h;
  data->x = 0;
  data->cmoy = 0.;
  data->n = 0;
  data->disp[w*h] = 0;

  if (bs > w) {
    data->a = bs / w;
    data->b = 1;
  } else {
    data->a = 1;
    data->b = w / bs;
  }
}

void on_remove2(SgElem *elem)
{
  sg_free(elem->data);
}

/**********************************************************************/

SgPortDesc pl_saw_ports[] = {
  {"output",0,0.f,0.f}
};

SgVarDesc pl_saw_vars[] = {
  {"frequency(Hz)",SG_VAR_TYPE_FLOAT,{440.f},{0.f},{40000.f}},
  {"shift",SG_VAR_TYPE_FLOAT,{0.5f},{0.f},{1.f}},
  {"phase",SG_VAR_TYPE_FLOAT,{0.f},{0.f},{0.f}},
  {"amplitude",SG_VAR_TYPE_FLOAT,{1.f},{0.f},{1.f}}
};

SgSgPluginDesc pl_saw = {
  "saw_oscillator",
  0, 1, 4, 0,
  pl_saw_ports,
  pl_saw_vars,
  SG_ELEMFLAG_NEEDOWNBUFFS,
  on_run0,
  on_run0,
  on_connect0,
  on_change0,
  on_make0,
  on_init0,
  on_reset0,
  on_remove0,
  NULL,
  NULL,
  NULL,
  NULL
};

SgPortDesc pl_out_ports[] = {
  {"input",0,0.f,0.f}
};

SgSgPluginDesc pl_out = {
  "audio_output",
  1, 0, 0, 0,
  pl_out_ports,
  NULL,
  SG_ELEMFLAG_NEEDOWNBUFFS,
  on_run1,
  on_run1,
  on_connect0,
  on_change0,
  on_make0,
  on_init1,
  on_reset0,
  on_remove1,
  NULL,
  NULL,
  NULL,
  NULL
};

SgPortDesc pl_ter_ports[] = {
  {"input",0,0.f,0.f}
};

SgVarDesc pl_ter_vars[] = {
  {"width",SG_VAR_TYPE_INT,{80},{0},{1000}},
  {"height",SG_VAR_TYPE_INT,{24},{0},{1000}},
  {"duration(ms)",SG_VAR_TYPE_INT,{500},{0},{5000}}
};

SgSgPluginDesc pl_ter = {
  "terminal_output",
  1, 0, 3, 0,
  pl_ter_ports,
  pl_ter_vars,
  SG_ELEMFLAG_NEEDOWNBUFFS,
  on_run2,
  on_run2,
  on_connect0,
  on_change0,
  on_make0,
  on_init2,
  on_reset0,
  on_remove2,
  NULL,
  NULL,
  NULL,
  NULL
};

SgInt cmpf(void *data1, void *data2)
{
/*  if ((int)data1 < (int)data2)
    return 0;
  if ((int)data1 > (int)data2)
    return 2;
  return 1;*/
  return (int)data1 - (int)data2;
}

int compare(void *a, void *b) {
    return (int)a - (int)b;
}

SgInt main()
{
  void *hdle = dlopen("plugins/ladspa/sn55ladspa.so", RTLD_LAZY);
  SgLadspaPluginDescFun ldf = dlsym(hdle, "ladspa_descriptor");
  SgLadspaPluginDesc *ld = ldf(0);

  SgMod *mod = sg_make_mod(44100., 1024, SG_MODFLAG_USEDOUBLE);
//  SgElem *elem0 = sg_make_elem(mod, (SgPluginDescP)&pl_saw, SG_PLUGIN_SG);
  SgElem *elem0 = sg_make_elem(mod, (SgPluginDescP)ld, SG_PLUGIN_LADSPA);
  SgElem *elem1 = sg_make_elem(mod, (SgPluginDescP)ld, SG_PLUGIN_LADSPA);
  SgElem *elem2 = sg_make_elem(mod, (SgPluginDescP)&pl_out, SG_PLUGIN_SG);
//  SgElem *elem3 = sg_make_elem(mod, (SgPluginDescP)&pl_ter, SG_PLUGIN_ELEM);

  sg_init_elem(elem0);
  sg_init_elem(elem1);
  sg_init_elem(elem2);

//  printf("mod=%p\n", mod);
/*
  printf("iunbuff=%p\n", mod->iunbuff);
  printf("ounbuff=%p\n\n", mod->ounbuff);
*/
  sg_mod_prepare(mod);

  sg_connect_elems(elem0, elem1, 0, 0);
  sg_connect_elems(elem1, elem2, 0, 0);

//  sg_inspect_elem(elem0);
//  sg_inspect_elem(elem1);
//  sg_inspect_elem(elem2);

//  printf("x:%f\n", (double)pl_saw_vars[1].value.fl);

//  elem1->vars[0].fl = 440.f;
//  elem1->vars[1].fl = 440.f;
//  elem1->vars[2].fl = 2.f;
//  elem1->vars[3].fl = 20.f;

//  elem1->vars[0].fl = 7320.f;
//  elem1->vars[2].fl = 1.f;
/*
  elem0->vars[0].fl = 0.07f;
  elem0->vars[3].fl = 2.f;
  elem0->vars[4].fl = 0.8f;
  elem0->vars[5].fl = 0.5f;
  elem0->vars[6].fl = 0.7f;
  elem1->vars[0].fl = 4320.f;
  elem1->vars[1].fl = 3000.f;
  elem1->vars[2].fl = 0.8f;
  elem1->vars[3].fl = 2.f;
  elem1->vars[4].fl = 0.4f;
  elem1->vars[6].fl = 0.05f;
  elem1->vars[7].fl = 0.25f;
  elem1->vars[9].fl = 4.f;
  elem1->vars[10].fl = 0.5f;
  elem1->vars[11].fl = 0.1f;
  elem1->vars[12].fl = 0.2f;
  elem1->vars[13].fl = 0.1f;
  elem1->vars[19].fl = 0.2f;
  elem1->vars[20].fl = 1.f;
  elem1->vars[21].fl = 15.5f;
  elem1->vars[22].fl = 0.4f;
*//*
  SgUInt i = 0;
  for (; i < elem1->vars_n; i++) {
    printf ("var %%%02d: %s : %f\n", i, elem1->desc.ladspa->port_names[elem1->bvars[i]->pnb], elem1->vars[i].fl);
  }
*/
  sg_mod_add_output_elem(elem2);

  sg_run_mod(mod, 0, 250000);

  sg_free_mod(mod);

  dlclose(hdle);
/*
#include <stdlib.h>
#include <time.h>
void sg_display_btree(SgBTreeNode *n);
void sg_display_btree_list(SgBTreeNode *n);

  SgBTree *tree = sg_make_btree(cmpf);
  SgInt i, r;
  for (i = 0; i < 63; i++) {
    sg_btree_insert(tree, (void*)i, (void*)0);
  }
  sg_display_btree(tree->root);

  for (i = 0; i < 350; i++) {
    r = random() % 63;
    sg_btree_remove(tree, (void*)r);
    printf("remove %d\n", r);
    sg_display_btree(tree->root);
    sg_display_btree_list(tree->root);
    printf("\n");
    printf("\n");
  }

  sg_free_btree(tree);
*/
  return 0;
}



/* end */

