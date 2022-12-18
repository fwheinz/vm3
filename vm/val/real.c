#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "prog.h"
#include "val.h"
#include "str.h"


val_t *v_real_new_double (double i) {
  val_t *v = val_new(T_REAL);
  v->u.real = i;

  return v;
}


val_t *v_real_create (void) {
  val_t *v = val_new(T_REAL);
  v->u.real = 0;

  return v;
}

val_t *v_real_dup (val_t *v) {
  val_t *v2 = val_new(T_REAL);
  v2->u.real = v->u.real;

  return v;
}

int v_real_cmp (val_t *v1, val_t *v2) {
  return v1->u.real - v2->u.real;
}

int v_real_to_bool (val_t *v) {
  return v->u.real == 0.0;
}

val_t *v_real_to_string (val_t *real) {
  int l = snprintf(NULL, 0, "%f", real->u.real);
  char buf[l+1];
  snprintf(buf, sizeof(buf), "%f", real->u.real);
  return v_str_new_cstr(buf);
}

val_t *v_real_conv (val_t *v) {
 char *ptr;
 switch (v->type) {
   case T_STR:
     ptr = v->u.str->buf;
     double nr = atof(ptr);
     return v_real_new_double(nr);
   default:
    return &val_undef;
 }
}

void v_real_serialize (FILE *f, val_t *v) {
  fwrite(&v->u.real, sizeof(v->u.real), 1, f);
}

val_t *v_real_deserialize (FILE *f) {
  val_t *v = val_new(T_REAL);
  int st = fread(&v->u.real, sizeof(v->u.real), 1, f);
  assert(st == 1);
  return v;
}

void val_register_real (void) {
  val_ops[T_REAL] = (struct val_ops) {
    .create = v_real_create,
    .free   = NULL,
    .len    = NULL,
    .dup    = v_real_dup,
    .cmp    = v_real_cmp,
    .to_bool= v_real_to_bool,
    .index  = NULL,
    .index_assign = NULL,
    .to_string = v_real_to_string,
    .conv   = v_real_conv,
    .serialize = v_real_serialize,
    .deserialize = v_real_deserialize,
  };
}





