#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "prog.h"
#include "val.h"
#include "str.h"


val_t *v_num_new_int (int i) {
  val_t *v = val_new(T_NUM);
  v->u.num = i;

  return v;
}


val_t *v_num_create (void) {
  val_t *v = val_new(T_NUM);
  v->u.num = 0;

  return v;
}

val_t *v_num_dup (val_t *v) {
  val_t *v2 = val_new(T_NUM);
  v2->u.num = v->u.num;

  return v;
}

int v_num_cmp (val_t *v1, val_t *v2) {
  return v1->u.num - v2->u.num;
}

val_t *v_num_to_string (val_t *num) {
  int l = snprintf(NULL, 0, "%d", num->u.num);
  char buf[l+1];
  snprintf(buf, sizeof(buf), "%d", num->u.num);
  return v_str_new_cstr(buf);
}

val_t *v_num_conv (val_t *v) {
 char *ptr;
 switch (v->type) {
   case T_STR:
     ptr = v->u.str->buf;
     int nr = strtoul(ptr, &ptr, 10);
     if (*ptr != '\0') {
       return &val_undef;
     }
     return v_num_new_int(nr);
   default:
    return &val_undef;
 }
}

void v_num_serialize (FILE *f, val_t *v) {
  write_int(f, v->u.num);
}

val_t *v_num_deserialize (FILE *f) {
  val_t *v = val_new(T_NUM);
  v->u.num = read_int(f);
  return v;
}

void val_register_num (void) {
  val_ops[T_NUM] = (struct val_ops) {
    .create = v_num_create,
    .free   = NULL,
    .len    = NULL,
    .dup    = v_num_dup,
    .cmp    = v_num_cmp,
    .index  = NULL,
    .index_assign = NULL,
    .to_string = v_num_to_string,
    .conv   = v_num_conv,
    .serialize = v_num_serialize,
    .deserialize = v_num_deserialize,
  };
}





