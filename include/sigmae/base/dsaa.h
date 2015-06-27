
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
 * base/dsaa.h
 */

#ifndef SGDSAA_H
#define SGDSAA_H

#include <sigmae/base/types.h>

#define MAX(a, b) (a < b ? b : a)
#define MIN(a, b) (a < b ? a : b)

typedef struct _SgVector
{
  void **head;
  void **tail;
  void **_rtail;
  SgSize size;
} SgVector;

#define sg_vector_isempty(v) (!v->size)

SgVector *sg_make_vector();
void sg_vector_push(SgVector *v, void *data);
void *sg_vector_pop(SgVector *v);
void sg_vector_remove
(SgVector *v, SgSize i);
SgSize sg_vector_find(SgVector *v, void *data);
void sg_free_vector(SgVector *v);

typedef struct _SgList
{
  struct _SgList *prev;
  struct _SgList *next;
  void *data[];
} SgList;

typedef void (*SgApplyFun)(void *data);
typedef SgInt (*SgCmpFun)(void *key1, void *key2);

void sg_free_list(SgList *ls);
SgList *sg_list_insert_cell(SgList *ls, SgList *cell);
SgList *sg_list_insert(SgList *ls, void *data);
SgList *sg_list_append_cell(SgList *ls, SgList *cell);
SgList *sg_list_append(SgList *ls, void *data);
SgList *sg_list_remlnk(SgList *ls, SgList *cell);
SgList *sg_list_insert_sorted(SgList *ls, SgCmpFun cmpf, void *data);
void sg_list_apply(SgList *ls, SgApplyFun f);
SgList *sg_list_find(SgList *ls, void *data);

typedef struct _SgBTreeNode {
  struct _SgBTreeNode *parent;
  struct _SgBTreeNode *left;
  struct _SgBTreeNode *right;
  SgSize mlevel;
  void *key;
  void *data[];
} SgBTreeNode;

typedef struct _SgBTree {
  SgBTreeNode *root;
  SgCmpFun cmpf;
} SgBTree;

SgBTree *sg_make_btree(SgCmpFun cmpf);
SgBTreeNode *sg_btree_make_node(void *key, SgSize size);
SgBTreeNode *sg_btree_make_simple_node(void *key, void *data);
SgInt sg_btree_insert_node(SgBTree *head, SgBTreeNode *node);
SgInt sg_btree_insert(SgBTree *head, void *key, void *data);
SgBTreeNode *sg_btree_find(SgBTree *head, void *key);
void sg_btree_remove_node(SgBTree *head, SgBTreeNode *node);
SgInt sg_btree_remove(SgBTree *head, void *key);
void sg_btree_detach_subtree(SgBTree *head, SgBTreeNode *tree);
void sg_btree_free_subtree(SgBTreeNode *tree);
void sg_free_btree(SgBTree *head);

#endif

/* end */

