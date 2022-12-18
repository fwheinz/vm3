#ifndef _VAL_H
#define _VAL_H

#include <stdio.h>

struct val;
typedef struct val val_t;

#include "str.h"
#include "num.h"
#include "arr.h"
#include "map.h"
#include "real.h"

#define GC_THRESHOLD 20

enum valtype {
  T_UNDEF = 0,
  T_STR,
  T_NUM,
  T_REAL,
  T_ARR,
  T_MAP,
  T_MAX
};

typedef int num_t;
typedef double real_t;
typedef struct str str_t;
typedef struct arr arr_t;
typedef struct map map_t;

struct val {
  int type;
  int marked;
  union {
    num_t num;
    real_t real;
    str_t *str;
    arr_t *arr;
    map_t *map;
  } u;
  struct val *next;
};

typedef struct val val_t;

struct val_ops {
  val_t *(*create)(void);
  void   (*free  )(val_t *);
  int    (*len   )(val_t *);
  val_t *(*dup   )(val_t *);
  int    (*cmp   )(val_t *, val_t *);
  val_t *(*index )(val_t *v, val_t *i);
  val_t *(*index_assign)(val_t *v, val_t *i, val_t *v2);
  val_t *(*to_string)(val_t *v);
  val_t *(*conv)(val_t *v);
  int    (*mark)(val_t *v);
  void   (*serialize)(FILE *f, val_t *v);
  val_t *(*deserialize)(FILE *f);
};

extern struct val_ops val_ops[];
extern val_t val_undef;

void val_init (void);

val_t *val_new (int type);

int vals_count (void);
int vals_unmark (void);
int vals_mark (void *);
int vals_sweep (void);
int vals_gc (void *);


val_t *val_create (int type);
void   val_free (val_t *val);
int    val_len (val_t *val);
val_t *val_dup (val_t *val);
int    val_cmp (val_t *v1, val_t *v2);
val_t *val_index (val_t *val, val_t *i);
val_t *val_index_assign (val_t *val, val_t *i, val_t *v2);
val_t *val_to_string (val_t *val);
char  *val_to_cstring (val_t *val);
val_t *val_conv (int type, val_t *val);
void val_serialize (FILE *f, val_t *val);
val_t *val_deserialize (FILE *f);
int val_mark (val_t *val);

val_t *val_add (val_t *v1, val_t *v2);
val_t *val_sub (val_t *v1, val_t *v2);
val_t *val_mul (val_t *v1, val_t *v2);
val_t *val_div (val_t *v1, val_t *v2);
val_t *val_mod (val_t *v1, val_t *v2);
val_t *val_neg (val_t *v);
val_t *val_pop (val_t *v1);
val_t *val_peek (val_t *v1);
val_t *val_push (val_t *v1, val_t *v2);

void write_int (FILE *f, int i);
int read_int (FILE *f);


#endif
