#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "val.h"
#include "map.h"

map_t *map_new (void) {
  map_t *a = malloc (sizeof *a);
  a->size = 0;
  a->keys = NULL;
  a->vals = NULL;

  return a;
}

void map_free (map_t *a) {
  free(a->keys);
  free(a->vals);
  free(a);
}

val_t *map_set (map_t *a, val_t *k, val_t *v) {
  if (v->type == T_UNDEF)
    return v;
  int slot = -1;
  for (int i = 0; i < a->size; i++) {
    if (val_cmp(a->keys[i], k) == 0) {
      a->vals[i] = v;
      if (v == &val_undef)
        a->keys[i] = v;
      return v;
    }
    if (a->keys[i] == &val_undef && slot < 0)
      slot = i;
  }
  if (slot < 0) {
    a->keys = realloc(a->keys, (a->size+1) * sizeof *a->keys);
    a->vals = realloc(a->vals, (a->size+1) * sizeof *a->vals);
    a->keys[a->size] = k;
    a->vals[a->size] = v;
    a->size++;
  } else {
    a->keys[slot] = k;
    a->vals[slot] = v;
  }

  return v;
}

val_t *map_get (map_t *a, val_t *k) {
  for (int i = 0; i < a->size; i++) {
    if (val_cmp(a->keys[i], k) == 0) {
      return a->vals[i];
    }
  }
  return &val_undef;
}

val_t *map_get_key (map_t *a, int i) {
  if (i >= a->size)
    return &val_undef;
  else
    return a->keys[i];
}

val_t *map_get_val (map_t *a, int i) {
  if (i >= a->size)
    return &val_undef;
  else
    return a->vals[i];
}

int map_mark (map_t *a) {
  int count = 0;
  for (int i = 0; i < a->size; i++) {
    count += val_mark(a->keys[i]);
    count += val_mark(a->vals[i]);
  }

  return count;
}

int map_len (map_t *a) {
  return a->size;
}

map_t *map_copy (map_t *a) {
  map_t *r = map_new();
  for (int i = 0; i < a->size; i++) {
    map_set(r, a->keys[i], a->vals[i]);
  }

  return r;
}

void map_add (map_t *a, map_t *a2) {
  for (int i = 0; i < a2->size; i++) {
    map_set(a, a2->keys[i], a2->vals[i]);
  }
}

val_t *v_map_create (void) {
  val_t *a = val_new(T_MAP);
  a->u.map = map_new();
  return a;
}

void v_map_free (val_t *a) {
  map_free(a->u.map);
}

int v_map_len (val_t *a) {
  return map_len(a->u.map);
}

val_t *v_map_copy (val_t *a) {
  val_t *r = val_new(T_MAP);
  r->u.map = map_copy(a->u.map);

  return r;
}

val_t *v_map_index (val_t *a, val_t *k) {
  return map_get(a->u.map, k);
}

val_t *v_map_index_assign (val_t *a, val_t *k, val_t *v) {
  return map_set(a->u.map, k, v);
}

int v_map_mark (val_t *a) {
  assert(a->type == T_MAP);
  return map_mark(a->u.map);
}

void v_map_serialize (FILE *f, val_t *v) {
  write_int(f, v->u.map->size);
  for (int i = 0; i < v->u.map->size; i++) {
    val_serialize(f, map_get_key(v->u.map, i));
    val_serialize(f, map_get_val(v->u.map, i));
  }
}

val_t *v_map_deserialize (FILE *f) {
  val_t *v = v_map_create();
  int sz = read_int(f);

  for (int i = 0; i < sz; i++) {
    val_t *mk = val_deserialize(f);
    val_t *mv = val_deserialize(f);
    map_set(v->u.map, mk, mv);
  }

  return v;
}

val_t *v_map_to_string (val_t *v) {
  val_t *s = v_str_create();

  str_add_cstr(s->u.str, "{");
  for (int i = 0; i < v->u.map->size; i++) {
    val_t *kstr = val_to_string(map_get_key(v->u.map, i));
    val_t *vstr = val_to_string(map_get_val(v->u.map, i));
    str_add_cstr(s->u.str, kstr->u.str->buf);
    str_add_cstr(s->u.str, " : ");
    str_add_cstr(s->u.str, vstr->u.str->buf);
    if (i != v->u.map->size - 1)
      str_add_cstr(s->u.str, ", ");
  }
  str_add_cstr(s->u.str, "}");

  return s;
}


void val_register_map (void) {
    val_ops[T_MAP] = (struct val_ops) {
    .create = v_map_create,
    .free   = v_map_free,
    .len    = v_map_len,
    .copy    = v_map_copy,
    .cmp    = NULL,
    .index  = v_map_index,
    .index_assign = v_map_index_assign,
    .to_string = v_map_to_string,
    .conv   = NULL,
    .mark   = v_map_mark,
    .serialize = v_map_serialize,
    .deserialize = v_map_deserialize,
  };
}

