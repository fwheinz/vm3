#ifndef _ARR_H
#define _ARR_H

struct arr {
  val_t **vals;
  int size;
};

typedef struct arr arr_t;

arr_t *arr_new (void);
void arr_free (arr_t *);
void arr_push (arr_t *, val_t *);
val_t *arr_pop (arr_t *);
val_t *arr_peek (arr_t *);
val_t *arr_set (arr_t *, int, val_t *);
val_t *arr_get (arr_t *, int);
int  arr_len (arr_t *);
int  arr_mark (arr_t *);
void arr_add (arr_t *a, arr_t *a2);

val_t *v_arr_create (void);
val_t *v_arr_index (val_t *a, val_t *i);
val_t *v_arr_index_assign (val_t *a, val_t *i, val_t *v);

void val_register_arr (void);



#endif /* _ARR_H */
