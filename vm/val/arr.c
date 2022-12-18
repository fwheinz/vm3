#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "val.h"
#include "arr.h"

arr_t *arr_new (void) {
  arr_t *a = malloc (sizeof *a);
  a->size = 0;
  a->vals = NULL;

  return a;
}

void arr_free (arr_t *a) {
  free(a->vals);
  free(a);
}

void arr_push (arr_t *a, val_t *v) {
  if (v->type == T_UNDEF)
    return;
  a->vals = realloc(a->vals, (a->size+1) * sizeof *a->vals);
  a->vals[a->size++] = v;
}

val_t *arr_pop (arr_t *a) {
  if (a->size <= 0)
    return &val_undef;
  else
    return a->vals[--a->size];
}

val_t *arr_peek (arr_t *a) {
  if (a->size <= 0)
    return &val_undef;
  else
    return a->vals[a->size-1];
}

static void arr_resize (arr_t *a) {
  int maxidx = a->size - 1;
  while (maxidx >= 0) {
    if (a->vals[maxidx]->type == T_UNDEF)
      maxidx--;
    else
      break;
  }
  a->size = maxidx + 1;
}

val_t *arr_set (arr_t *a, int i, val_t *v) {
  if (v->type == T_UNDEF)
    return v;
  if (a->size <= i) {
    a->vals = realloc(a->vals, (i+1) * sizeof *a->vals);
    for (int j = a->size; j <= i; j++) {
      a->vals[j] = &val_undef;
    }
    a->size = i+1;
  }
  a->vals[i] = v;
  arr_resize(a);

  return v;
}

val_t *arr_get (arr_t *a, int i) {
  if (a->size <= i)
    return &val_undef;
  else
    return a->vals[i];
}

int arr_mark (arr_t *a) {
  int count = 0;
  for (int i = 0; i < a->size; i++) {
    count += val_mark(a->vals[i]);
  }

  return count;
}

int arr_len (arr_t *a) {
  return a->size;
}

arr_t *arr_dup (arr_t *a) {
  arr_t *r = arr_new();
  for (int i = 0; i < a->size; i++) {
    arr_set(r, i, a->vals[i]);
  }

  return r;
}

void arr_add (arr_t *a, arr_t *a2) {
  for (int i = 0; i < a2->size; i++) {
    arr_set(a, a->size+i, a2->vals[i]);
  }
}

val_t *v_arr_create (void) {
  val_t *a = val_new(T_ARR);
  a->u.arr = arr_new();
  return a;
}

void v_arr_free (val_t *a) {
  arr_free(a->u.arr);
}

int v_arr_len (val_t *a) {
  return arr_len(a->u.arr);
}

int v_arr_to_bool (val_t *a) {
  return arr_len(a->u.arr) != 0;
}

val_t *v_arr_dup (val_t *a) {
  val_t *r = val_new(T_ARR);
  r->u.arr = arr_dup(a->u.arr);

  return r;
}

val_t *v_arr_index (val_t *a, val_t *i) {
  if (i->type != T_NUM)
    return &val_undef;
  return arr_get(a->u.arr, i->u.num);
}

val_t *v_arr_index_assign (val_t *a, val_t *i, val_t *v) {
  if (i->type != T_NUM)
    return &val_undef;
  return arr_set(a->u.arr, i->u.num, v);
}

int v_arr_mark (val_t *a) {
  assert(a->type == T_ARR);
  return arr_mark(a->u.arr);
}

void v_arr_serialize (FILE *f, val_t *v) {
  write_int(f, v->u.arr->size);
  for (int i = 0; i < v->u.arr->size; i++) {
    val_serialize(f, arr_get(v->u.arr, i));
  }
}

val_t *v_arr_deserialize (FILE *f) {
  val_t *v = v_arr_create();
  int sz = read_int(f);

  for (int i = 0; i < sz; i++) {
    val_t *m = val_deserialize(f);
    arr_set(v->u.arr, i, m);
  }

  return v;
}

val_t *v_arr_to_string (val_t *v) {
  val_t *s = v_str_create();

  str_add_cstr(s->u.str, "[");
  for (int i = 0; i < v->u.arr->size; i++) {
    val_t *vstr = val_to_string(arr_get(v->u.arr, i));
    char buf[200];
    snprintf(buf, sizeof(buf), "%d : ", i);
    str_add_cstr(s->u.str, buf);
    str_add_cstr(s->u.str, vstr->u.str->buf);
    if (i != v->u.arr->size - 1)
      str_add_cstr(s->u.str, ", ");
  }
  str_add_cstr(s->u.str, "]");

  return s;
}


void val_register_arr (void) {
    val_ops[T_ARR] = (struct val_ops) {
    .create = v_arr_create,
    .free   = v_arr_free,
    .len    = v_arr_len,
    .dup    = v_arr_dup,
    .cmp    = NULL,
    .to_bool= v_arr_to_bool,
    .index  = v_arr_index,
    .index_assign = v_arr_index_assign,
    .to_string = v_arr_to_string,
    .conv   = NULL,
    .mark   = v_arr_mark,
    .serialize = v_arr_serialize,
    .deserialize = v_arr_deserialize,
  };
}

