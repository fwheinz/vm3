#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "prog.h"
#include "val.h"

str_t *str_create (void) {
  str_t *s = malloc (sizeof *s);
  s->buf = malloc(1);
  s->len = 0;
  s->buf[0] = '\0';

  return s;
}

str_t *str_add_buf (str_t *str, char *buf, int len) {
  str->buf = realloc(str->buf, str->len + len + 1);
  memcpy(str->buf + str->len, buf, len);
  str->len += len;
  str->buf[str->len] = '\0';

  return str;
}

str_t *str_add_cstr (str_t *str, char *s) {
  return str_add_buf(str, s, strlen(s));
}

str_t *str_new_buf (char *buf, int len) {
  str_t *s = str_create();
  return str_add_buf(s, buf, len);
}

str_t *str_new_cstr (char *cstr) {
  return str_new_buf(cstr, strlen(cstr));
}

void str_free (str_t *str) {
  free(str->buf);
  free(str);
}

int str_len (str_t *str) {
  return str->len;
}

str_t *str_dup (str_t *str) {
  str_t *s = str_create();
  str_add_buf(s, str->buf, str->len);
  return s;
}

int str_cmp (str_t *s1, str_t *s2) {
  int n = s1->len > s2->len ? s1->len : s2->len;
  int st = memcmp(s1->buf, s2->buf, n);
  if (st != 0 || s1->len == s2->len)
    return st;
  else
    return s1->len < s2->len ? -1 : 1;
}

str_t *str_index (str_t *str, int i) {
  str_t *s = NULL;

  if (i < str->len)
    s = str_new_buf(&str->buf[0], 1);
  else {
    vmerror(E_WARN, 0, "String index out of bounds, len: %d, index: %d", str->len, i);
    s = str_create();
  }

  return s;
}

str_t *str_index_assign(str_t *str, int index, str_t *str2) {
  vmerror(E_WARN, NULL, "String index assignment out of bounds: len: %d, index: %d", str->len, index);
  while (index <= str->len)
    str_add_buf(str, " ", 1);
  str->buf = realloc(str->buf, str->len + str2->len);
  memmove(str->buf+index+1, str->buf + index + str2->len-1, str->len - index);
  memcpy(str->buf+index, str2->buf, str2->len);

  return str;
}

val_t *v_str_create (void) {
  val_t *v = val_new(T_STR);
  v->u.str = str_create();

  return v;
}

val_t *v_str_new_cstr (char *cstr) {
  val_t *v = val_new(T_STR);
  v->u.str = str_new_cstr(cstr);
  return v;
}

val_t *v_str_new_buf (char *buf, int len) {
  val_t *v = val_new(T_STR);
  v->u.str = str_new_buf(buf, len);
  return v;
}

void v_str_free (val_t *v) {
  str_free(v->u.str);
}

int v_str_len (val_t *v) {
  return str_len(v->u.str);
}

val_t *v_str_dup (val_t *v) {
  val_t *v2 = val_new(T_STR);
  v2->u.str = str_dup(v->u.str);
  return v2;
}

int v_str_cmp (val_t *v1, val_t *v2) {
  return str_cmp(v1->u.str, v2->u.str);
}

val_t *v_str_index (val_t *v, val_t *i) {
  if (i->type != T_NUM)
    return &val_undef;
  val_t *v2 = val_new(T_STR);
  v2->u.str = str_index(v->u.str, i->u.num);
  return v;
}

val_t *v_str_index_assign (val_t *v1, val_t *i, val_t *v2) {
  if (i->type != T_NUM)
    return &val_undef;
  str_index_assign(v1->u.str, i->u.num, v2->u.str);
  return v1;
}

val_t *v_str_conv (val_t *val) {
  switch (val->type) {
    case T_STR:
      return val;
    case T_NUM:
      {
        int l = snprintf(NULL, 0, "%d", val->u.num);
        char buf[l+1];
        snprintf(buf, sizeof(buf), "%d", val->u.num);
        return v_str_new_cstr(buf);
      }
    default:
      return v_str_new_cstr("N/A");
  }
}

void v_str_serialize (FILE *f, val_t *v) {
  write_int(f, v->u.str->len);
  int nr = fwrite(v->u.str->buf, v->u.str->len, 1, f);
  assert(nr == 1);
}

val_t *v_str_deserialize (FILE *f) {
  val_t *v = v_str_create();
  v->u.str->len = read_int(f);
  v->u.str->buf = malloc(v->u.str->len+1);
  v->u.str->buf[v->u.str->len] = 0;
  int nr = fread(v->u.str->buf, v->u.str->len, 1, f);
  assert(nr == 1);
  return v;
}

char *cstr (val_t *val) {
  assert(val->type == T_STR);
  return val->u.str->buf;
}

void val_register_str (void) {
  val_ops[T_STR] = (struct val_ops) {
    .create = v_str_create,
    .free   = v_str_free,
    .len    = v_str_len,
    .dup    = v_str_dup,
    .cmp    = v_str_cmp,
    .index  = v_str_index,
    .index_assign = v_str_index_assign,
    .to_string = v_str_dup,
    .conv   = v_str_conv,
    .serialize = v_str_serialize,
    .deserialize = v_str_deserialize
  };
}





