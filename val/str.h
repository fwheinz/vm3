#ifndef _STR_H
#define _STR_H

#include "val.h"

struct str {
  char *buf;
  int len;
};

typedef struct str str_t;

void val_register_str(void);

val_t *v_str_create (void);
val_t *v_str_new_cstr (char *cstr);
val_t *v_str_new_buf (char *buf, int len);

str_t *str_add_buf (str_t *str, char *buf, int len);


char *cstr (val_t *v);


#endif
