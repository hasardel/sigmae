
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
 * dsaa.c
 */

#include <stdio.h>

#include <sigmae/base.h>
#include <sigmae/mem.h>
#include <sigmae/error.h>


#define VBLSZ 8

const SgBTreeNode last = {0, 0, 0, 0, 0};
#define LAST ((SgBTreeNode*)&last)


SgVector *sg_make_vector()
{
  SgVector *v = sg_alloc(sizeof(SgVector));
  v->head = sg_alloc(VBLSZ * sizeof(void*));
  v->head[0] = NULL;
  v->tail = v->head;
  v->_rtail = v->head + VBLSZ;
  v->size = 0;
  return v;
}

void sg_vector_push(SgVector *v, void *data)
{
  SgSize s;
  *v->tail = data;
  v->size++;
  if (++v->tail == v->_rtail) {
    s = v->size + VBLSZ;
    v->head = sg_realloc(v->head, s * sizeof(void*));
    v->_rtail = v->head + s;
    v->tail = v->_rtail - VBLSZ;
  }
  *v->tail = NULL;
}

void *sg_vector_pop(SgVector *v)
{
  void *p;
  if (!v->size)
    return NULL;
  if (v->_rtail - v->tail-- == VBLSZ) {
    v->head = sg_realloc(v->head, v->size * sizeof(void*));
    v->_rtail = v->head + v->size;
    v->tail = v->_rtail - 1;
  }
  p = *v->tail;
  *v->tail = NULL;
  v->size--;
  return p;
}

void sg_vector_remove(SgVector *v, SgSize i)
{
  if (i >= v->size || !v->size)
    return;
  memcpy(&v->head[i], &v->head[i+1], (v->size - i) * sizeof(void*));
  if (v->_rtail - v->tail-- == VBLSZ) {
    v->head = sg_realloc(v->head, v->size * sizeof(void*));
    v->_rtail = v->head + v->size;
    v->tail = v->_rtail - 1;
  }
  v->size--;
}

SgSize sg_vector_find(SgVector *v, void *data)
{
  void **p = v->head;
  for (; *p; p++)
    if (*p == data)
      return p - v->head;
  return -1;
}

void sg_free_vector(SgVector *v)
{
  sg_free(v->head);
  sg_free(v);
}

void sg_free_list(SgList *ls)
{
  SgList *n;
  for (;ls;) {
    n = ls->next;
    sg_free(ls);
    ls = n;
  }
}

SgList *sg_list_movef(SgList *ls, SgSize i)
{
  SgList *n = ls;
  for (; i && n; i--)
    n = n->next;
  return n;
}

SgList *sg_list_moveb(SgList *ls, SgSize i)
{
  SgList *p = ls;
  for (; i && p; i--)
    p = p->prev;
  return p;
}

SgList *sg_list_insert_cell(SgList *ls, SgList *c)
{
  if (!ls) {
    c->prev = NULL;
    c->next = NULL;
  } else {
    c->prev = ls->prev;
    c->next = ls;
    if (ls->prev)
      ls->prev->next = c;
    ls->prev = c;
  }
  return c;
}

SgList *sg_list_insert(SgList *ls, void *data)
{
  SgList *c = sg_alloc(sizeof(SgList) + sizeof(void*));
  c->data[0] = data;
  return sg_list_insert_cell(ls, c);
}

SgList *sg_list_insert_sorted(SgList *ls, SgCmpFun cmpf, void *data)
{
  SgList *c, *p, *q = sg_alloc(sizeof(SgList) + sizeof(void*));
  q->data[0] = data;
  for (c = ls, p = NULL; ; p = c) {
    if (!c) {
      q->prev = p;
      q->next = NULL;
      break;
    }
    if (cmpf(data, c->data[0]) < 0) {
      q->prev = c->prev;
      q->next = c;
      c->prev = q;
      break;
    }
  }
  if (!p)
    return q;
  return ls;
}

SgList *sg_list_append_cell(SgList *ls, SgList *q)
{
  SgList *c, *n;
  q->next = NULL;
  if (!ls) {
    q->prev = NULL;
    return q;
  }
  n = ls;
  for (;;) {
    c = n;
    n = c->next;
    if (!n) {
      q->prev = c;
      c->next = q;
      return ls;
    }
  }
}

SgList *sg_list_append(SgList *ls, void *data)
{
  SgList *c = sg_alloc(sizeof(SgList) + sizeof(void*));
  c->data[0] = data;
  return sg_list_append_cell(ls, c);
}

SgList *sg_list_remlnk(SgList *ls, SgList *c)
{
  SgList *p = c->prev, *n = c->next;
  if (p)
    p->next = n;
  else
    ls = n;
  if (n)
    n->prev = p;
  c->prev = NULL;
  c->next = NULL;
  return ls;
}

void sg_list_apply(SgList *ls, SgApplyFun f)
{
  for (;ls;) {
    f((void*)ls->data);
    ls = ls->next;
  }
}

SgList *sg_list_find(SgList *ls, void *data)
{
  for (;ls;) {
    if (ls->data[0] == data)
      return ls;
    ls = ls->next;
  }
  return NULL;
}

static inline void fix_mlevel(SgBTreeNode *n)
{
  SgSize lml, rml;

  for (;;) {
    lml = n->left->mlevel;
    rml = n->right->mlevel;
    lml = (lml > rml ? lml : rml) + 1;
    if (n->mlevel == lml)
      break;
    n->mlevel = lml;
    n = n->parent;
    if (n == LAST)
      break;
  }
}

static SgBTreeNode *rotate_left(SgBTreeNode *n)
{
  SgBTreeNode *r = n->right;
  SgBTreeNode *p = n->parent;
  SgSize lml, rml;

  n->right = r->left;
  if (r->left != LAST)
    r->left->parent = n;
  r->left = n;
  n->parent = r;
  r->parent = p;
  lml = n->left->mlevel;
  rml = n->right->mlevel;
  lml = (lml > rml ? lml : rml) + 1;
  n->mlevel = lml;
  rml = r->right->mlevel;
  r->mlevel = (lml > rml ? lml : rml) + 1;
  if (p != LAST) {
    if (p->left == n)
      p->left = r;
    else
      p->right = r;
    fix_mlevel(p);
  }
  return r;
}

static SgBTreeNode *rotate_right(SgBTreeNode *n)
{
  SgBTreeNode *l = n->left;
  SgBTreeNode *p = n->parent;
  SgSize lml, rml;

  n->left = l->right;
  if (l->right != LAST)
    l->right->parent = n;
  l->right = n;
  n->parent = l;
  l->parent = p;
  rml = n->right->mlevel;
  lml = n->left->mlevel;
  rml = (lml > rml ? lml : rml) + 1;
  n->mlevel = rml;
  lml = l->left->mlevel;
  l->mlevel = (lml > rml ? lml : rml) + 1;
  if (p != LAST) {
    if (p->right == n)
      p->right = l;
    else
      p->left = l;
    fix_mlevel(p);
  }
  return l;
}

static void equilibrate(SgBTreeNode *n, SgBTreeNode **root)
{
  SgBTreeNode *p, *f;
  SgSize lml, rml;
  SgSize r;

  for (;; n = p) {
    p = n->parent;
    f = NULL;
    lml = n->left->mlevel;
    rml = n->right->mlevel;
    if (lml < rml) {
      if ((r = rml - lml) == 1) {
        if (p->left == n)
          f = rotate_left(n);
      } else {
        f = rotate_left(n);
        if (r > 2) {
          equilibrate(n->right, &f);
          equilibrate(n, &f);
        }
      }
    } else if (lml > rml) {
      if ((r = lml - rml) == 1) {
        if (p->right == n)
          f = rotate_right(n);
      } else {
        f = rotate_right(n);
        if (r > 2) {
          equilibrate(n->left, &f);
          equilibrate(n, &f);
        }
      }
    } else
      break;
    if (n == *root) {
      if (f)
        *root = f;
      break;
    }
  }
}

static void remove_node_left(SgBTreeNode **root, SgBTreeNode *n)
{
  SgBTreeNode *l = n->left, *r = n->right, *p = n->parent;
  SgBTreeNode *lx = NULL;

  sg_free(n);

  l->parent = p;
  if (p != LAST) {
    if (p->left == n)
      p->left = l;
    else
      p->right = l;
  }

  if (r != LAST) {
    r->parent = l;
    if (l->right != LAST) {
      for (lx = r; lx->left != LAST; lx = lx->left) {}
      lx->left = l->right;
      l->right->parent = lx;
    }
    l->right = r;
    if (lx)
      fix_mlevel(lx);
    fix_mlevel(l);
  }

  if (lx)
    equilibrate(lx, &r);
  equilibrate(l, &l);
  if (p == LAST)
    *root = l;
  else {
    fix_mlevel(p);
    equilibrate(p, root);
  }
}

static void remove_node_right(SgBTreeNode **root, SgBTreeNode *n)
{
  SgBTreeNode *l = n->left, *r = n->right, *p = n->parent;
  SgBTreeNode *rx = NULL;

  sg_free(n);

  r->parent = p;
  if (p != LAST) {
    if (p->left == n)
      p->left = r;
    else
      p->right = r;
  }

  if (l != LAST) {
    l->parent = r;
    if (r->left != LAST) {
      for (rx = l; rx->right != LAST; rx = rx->right) {}
      rx->right = r->left;
      r->left->parent = rx;
    }
    r->left = l;
    if (rx)
      fix_mlevel(rx);
    fix_mlevel(r);
  }

  if (rx)
    equilibrate(rx, &l);
  equilibrate(r, &r);
  if (p == LAST)
    *root = r;
  else {
    fix_mlevel(p);
    equilibrate(p, root);
  }
}

static void remove_leaf(SgBTreeNode **root, SgBTreeNode *n)
{
  SgBTreeNode *p = n->parent;

  sg_free(n);

  if (p == LAST) {
    *root = LAST;
    return;
  }
  if (p->left == n)
    p->left = LAST;
  else
    p->right = LAST;
  fix_mlevel(p);
  equilibrate(p, root);
}

static void remove_node(SgBTreeNode **root, SgBTreeNode *n)
{
  SgSize lml, rml;

  lml = n->left->mlevel;
  rml = n->right->mlevel;
  if (!rml && !lml)
    remove_leaf(root, n);
  else if (rml > lml)
    remove_node_right(root, n);
  else if (rml < lml)
    remove_node_left(root, n);
  else {
    if (n->parent->left == n)
      remove_node_right(root, n);
    else
      remove_node_left(root, n);
  }
}

SgBTree *sg_make_btree(SgCmpFun cmpf)
{
  SgBTree *h = sg_alloc(sizeof(SgBTree));
  h->root = LAST;
  h->cmpf = cmpf;
  return h;
}

SgInt sg_btree_insert_node(SgBTree *h, SgBTreeNode *n)
{
  SgBTreeNode *p = LAST, *c = h->root;
  SgCmpFun const cmpf = h->cmpf;
  SgInt r;

  if (n == LAST)
    return 0;
  for (; c != LAST;) {
    p = c;
    r = cmpf(n->key, c->key);
    if (r < 0)
      c = c->left;
    else if (r > 0)
      c = c->right;
    else
      return -1;
  }

  n->parent = p;
  if (p == LAST)
    h->root = n;
  else {
    if (r < 0)
      p->left = n;
    else
      p->right = n;
    fix_mlevel(p);
    equilibrate(p, &h->root);
  }
  return 0;
}

SgInt sg_btree_insert(SgBTree *h, void *key, void *data)
{
  SgBTreeNode *p = LAST, *n;
  SgBTreeNode *c = h->root;
  SgCmpFun const cmpf = h->cmpf;
  SgInt r;

  for (; c != LAST;) {
    p = c;
    r = cmpf(key, c->key);
    if (r < 0)
      c = c->left;
    else if (r > 0)
      c = c->right;
    else
      return -1;
  }

  n = sg_alloc(sizeof(SgBTreeNode) + sizeof(void*));
  n->left = LAST;
  n->right = LAST;
  n->parent = p;
  n->mlevel = 1;
  n->key = key;
  n->data[0] = data;
  if (p == LAST)
    h->root = n;
  else {
    if (r < 0)
      p->left = n;
    else
      p->right = n;
    fix_mlevel(p);
    equilibrate(p, &h->root);
  }
  return 0;
}

/*
 * Make a node with a specified data size (in bytes).
 */
SgBTreeNode *sg_btree_make_node(void *key, SgSize size)
{
  SgBTreeNode *n = sg_alloc(sizeof(SgBTreeNode) + size);
  n->parent = LAST;
  n->left = LAST;
  n->right = LAST;
  n->mlevel = 1;
  n->key = key;
  return n;
}

/*
 * Make a node with a single pointer as data.
 */
SgBTreeNode *sg_btree_make_simple_node(void *key, void *data)
{
  SgBTreeNode *n = sg_btree_make_node(key, sizeof(void*));
  n->data[0] = data;
  return n;
}

SgBTreeNode *sg_btree_find(SgBTree *h, void *key)
{
  SgBTreeNode *c = h->root;
  SgCmpFun const cmpf = h->cmpf;
  SgInt r;

  for (; c != LAST;) {
    r = cmpf(key, c->key);
    if (r < 0)
      c = c->left;
    else if (r > 0)
      c = c->right;
    else
      return c;
  }
  return NULL;
}

void sg_btree_remove_node(SgBTree *h, SgBTreeNode *n)
{
  if (n != LAST)
    remove_node(&h->root, n);
}

SgInt sg_btree_remove(SgBTree *h, void *key)
{
  SgBTreeNode *n = sg_btree_find(h, key);

  if (n) {
    remove_node(&h->root, n);
    return 0;
  }
  return -1;
}

void sg_btree_detach_subtree(SgBTree *h, SgBTreeNode *n)
{
  SgBTreeNode *p = n->parent;

  if (n == LAST)
    return;
  if (p == LAST) {
    h->root = LAST;
    return;
  }
  n->parent = LAST;
  if (p->left == n)
    p->left = LAST;
  else
    p->right = LAST;
  fix_mlevel(p);
  equilibrate(p, &h->root);
}

static void free_btree(SgBTreeNode *n)
{
  if (n != LAST) {
    free_btree(n->left);
    free_btree(n->right);
    sg_free(n);
  }
}

void sg_btree_free_subtree(SgBTreeNode *n)
{
  if (n->parent == LAST)
    free_btree(n);
}

void sg_free_btree(SgBTree *h)
{
  free_btree(h->root);
  sg_free(h);
}

static void fill_buff(SgBTreeNode *n, SgChar *buff,
                      SgSize vl, SgSize hl, SgSize ml, SgSize ls)
{
  SgSize s;

  if (n == LAST)
    return;

  s = 2 << (ml - hl - 1);
  buff[hl*ls+s/2-1+s*vl] = '.';
  fill_buff(n->left, buff, vl << 1, hl + 1, ml, ls);
  fill_buff(n->right, buff, (vl << 1) + 1, hl + 1, ml, ls);
}

void sg_display_btree(SgBTreeNode *n)
{
  SgSize ml, ls, i, j;
  SgChar *buff;

  if (n == LAST)
    return;
  ml = n->mlevel;
  ls = (2 << ml) / 2;
  buff = sg_alloc(ls * ml);

  for (i = 0; i < ml; i++) {
    for (j = 0; j < (ls - 1); j++) {
      buff[i*ls+j] = ' ';
    }
    buff[i*ls+j] = 0;
  }

  fill_buff(n, buff, 0, 0, ml, ls);
  for (; ml;) {
    printf("%s\n", &buff[--ml*ls]);
  }
  sg_free(buff);
}

void sg_display_btree_list(SgBTreeNode *n)
{
  if (n == LAST) {
    printf("()");
    return;
  }
  printf("(%03d ", (int)n->key);
  sg_display_btree_list(n->left);
  printf(" ");
  sg_display_btree_list(n->right);
  printf(")");
}

/* end */

