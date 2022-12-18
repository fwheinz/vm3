#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "prog.h"
#include "val.h"
#include "str.h"
#include "num.h"
#include "real.h"
#include "map.h"
#include "arr.h"

val_t *val_head;

struct val_ops val_ops[T_MAX];

val_t val_undef = {
  .type = T_UNDEF,
  .marked = 1
};

void val_init (void) {
  val_register_str();
  val_register_num();
  val_register_real();
  val_register_arr();
  val_register_map();
}

val_t *val_new (int type) {
  val_t *v;
  if (type == T_UNDEF)
    v = &val_undef;
  else {
    v = malloc(sizeof *v);
    v->type = type;
    v->marked = 0;
    v->next = val_head;
    val_head = v;
  }

  return v;
}

val_t *val_create (int type) {
  val_t *v;
  if (val_ops[type].create)
    v = val_ops[type].create();
  else
    v = val_new(type);

  return v;
}

void val_free (val_t *val) {
  int type = val->type;
  if (val_ops[type].free)
    val_ops[type].free(val);
  val->type = -1;
  free(val);
}

int read_int (FILE *f) {
  int i;
  int nr = fread(&i, sizeof(i), 1, f);
  assert (nr == 1);
  return i;
}

void write_int (FILE *f, int i) {
  fwrite(&i, sizeof(i), 1, f);
}

void val_serialize (FILE *f, val_t *val) {
  write_int(f, val->type);
  if (val_ops[val->type].serialize) {
    val_ops[val->type].serialize(f, val);
  }
}

val_t *val_deserialize (FILE *f) {
  int type = read_int(f);
  if (val_ops[type].deserialize)
    return val_ops[type].deserialize(f);
  else
    return &val_undef;
}

int val_len (val_t *val) {
  int type = val->type;
  if (val_ops[type].len)
    return val_ops[type].len(val);
  return 0;
}

val_t *val_dup (val_t *val) {
  int type = val->type;
  if (val_ops[type].dup)
    return val_ops[type].dup(val);
  else
    return val_create(val->type);
}

int val_cmp (val_t *v1, val_t *v2) {
  int type = v1->type;
  if (!val_ops[type].cmp)
    return -1;

  if (v2->type == v1->type) {
    return val_ops[type].cmp(v1, v2);
  }

  return -1;
}

int val_to_bool (val_t *v) {
  int type = v->type;
  if (!val_ops[type].to_bool)
    return 0;
  return val_ops[type].to_bool(v);
}


val_t *val_index (val_t *val, val_t *i) {
  int type = val->type;
  if (!val_ops[type].index)
    return &val_undef;
  else
    return val_ops[type].index(val, i);
}

val_t *val_index_assign (val_t *val, val_t *i, val_t *v2) {
  int type = val->type;
  if (!val_ops[type].index_assign)
    return &val_undef;
  else
    return val_ops[type].index_assign(val, i, v2);
}

val_t *val_to_string (val_t *val) {
  if (val_ops[val->type].to_string)
    return val_ops[val->type].to_string(val);
  else
    return v_str_new_cstr("UNDEF");
}

char *val_to_cstring (val_t *val) {
  return val_to_string(val)->u.str->buf;
}

val_t *val_conv (int type, val_t *val) {
  if (val->type == type)
    return val;
  if (val_ops[type].conv)
    return val_ops[type].conv(val);
  else
    return &val_undef;
}


val_t *val_add (val_t *v1, val_t *v2) {
  val_t *ret;

  switch (v1->type) {
    case T_STR:
      if (v2->type != T_STR)
        return &val_undef;
      ret = val_dup(v1);
      str_add_buf(ret->u.str, v2->u.str->buf, v2->u.str->len);
      return ret;

    case T_NUM:
      if (v2->type != T_NUM)
        return &val_undef;
      ret = v_num_new_int(v1->u.num + v2->u.num);
      return ret;
    
    case T_REAL:
      if (v2->type != T_REAL)
        return &val_undef;
      ret = v_real_new_double(v1->u.real + v2->u.real);
      return ret;
    
    case T_ARR:
      ret = val_dup(v1);
      if (v2->type == T_ARR) {
        arr_add(ret->u.arr, v2->u.arr);
      } else {
        arr_push(ret->u.arr, v2);
      }
      return ret;

    default:
      return &val_undef;
  }

  return &val_undef;
}

val_t *val_sub (val_t *v1, val_t *v2) {
  val_t *ret;

  switch (v1->type) {
    case T_NUM:
      if (v2->type != T_NUM)
        return &val_undef;
      ret = v_num_new_int(v1->u.num - v2->u.num);
      return ret;
    
    case T_REAL:
      if (v2->type != T_REAL)
        return &val_undef;
      ret = v_real_new_double(v1->u.real - v2->u.real);
      return ret;
    
    default:
      return &val_undef;
  }

  return &val_undef;
}

val_t *val_mul (val_t *v1, val_t *v2) {
  val_t *ret;

  switch (v1->type) {
    case T_STR:
      if (v2->type != T_NUM)
        return &val_undef;
      int i = v2->u.num;
      ret = val_create(T_STR);
      while (i--) {
        str_add_buf(ret->u.str, v1->u.str->buf, v1->u.str->len);
      }
      return ret;

    case T_REAL:
      if (v2->type != T_REAL)
        return &val_undef;
      return v_real_new_double(v1->u.real * v2->u.real);

    case T_NUM:
      if (v2->type != T_NUM)
        return &val_undef;
      ret = v_num_new_int(v1->u.num * v2->u.num);
      return ret;
    
    case T_ARR:
      if (v2->type != T_STR)
        return &val_undef;
      ret = val_create(T_STR);
      for (int i = 0; i < v1->u.arr->size; i++) {
        val_t *item = val_conv(T_STR, arr_get(v1->u.arr, i));
        str_add_buf(ret->u.str, item->u.str->buf, item->u.str->len);
        if (i != v1->u.arr->size-1)
          str_add_buf(ret->u.str, v2->u.str->buf, v2->u.str->len);
      }
      return ret;
    
    default:
      return &val_undef;
  }
}

val_t *val_div (val_t *v1, val_t *v2) {
  val_t *ret;

  switch (v1->type) {
    case T_NUM:
      if (v2->type != T_NUM)
        return &val_undef;
      ret = v_num_new_int(v1->u.num / v2->u.num);
      return ret;

     case T_REAL:
      if (v2->type != T_REAL)
        return &val_undef;
      return v_real_new_double(v1->u.real / v2->u.real);
   
    default:
      return &val_undef;
  }
}

val_t *val_mod (val_t *v1, val_t *v2) {
  val_t *ret;

  switch (v1->type) {
    case T_NUM:
      if (v2->type != T_NUM)
        return &val_undef;
      ret = v_num_new_int(v1->u.num % v2->u.num);
      return ret;
    
    default:
      return &val_undef;
  }
}

val_t *val_neg (val_t *v) {
  switch (v->type) {
    case T_NUM:
      return v_num_new_int(-v->u.num);
    case T_REAL:
      return v_real_new_double(-v->u.real);
    
    default:
      return &val_undef;
  }
}



val_t *val_peek (val_t *v1) {
  val_t *r = &val_undef;
  switch (v1->type) {
    case T_ARR:
      r = arr_peek(v1->u.arr);
      break;
  }

  return r;
}

val_t *val_pop (val_t *v1) {
  val_t *r = &val_undef;
  switch (v1->type) {
    case T_ARR:
      r = arr_pop(v1->u.arr);
      break;
  }

  return r;
}

val_t *val_push (val_t *v1, val_t *v2) {
  val_t *r = &val_undef;
  switch (v1->type) {
    case T_ARR:
      arr_push(v1->u.arr, v2);
      r = v2;
      break;
  }

  return r;
}

int val_mark (val_t *val) {
  if (val->marked != 0) {
    return 0;
  }
  val->marked = 1;
  if (val_ops[val->type].mark != NULL) {
    return val_ops[val->type].mark(val) + 1;
  }
  return 1;
}

int vals_count (void) {
  int c = 0;
  for (val_t *p = val_head; p; p = p->next) {
    c++;
  }

  return c;
}

int vals_unmark (void) {
  int count = 0;

  for (val_t *p = val_head; p; p = p->next) {
    if (p->marked) {
      count++;
      p->marked = 0;
    }
  }
  return count;
}

int vals_mark (void *_exec) {
  int count = 0;
  exec_t *exec = _exec;

  for (int i = 0; i < exec->vstack->size; i++) {
    count += val_mark(exec->vstack->vals[i]);
  }
  for (int i = 0; i < exec->vars->size; i++) {
    count += val_mark(exec->vars->vals[i]);
  }
  count += val_mark(exec->global_vars);
  count += val_mark(exec->prog->constants);
  count += val_mark(exec->prog->functions);
  count += val_mark(exec->prog->ops);

  return count;
}

int vals_sweep (void) {
  int removed = 0;

  while (val_head && val_head->marked == 0) {
    val_t *v = val_head->next;
    val_free(val_head);
    removed++;
    val_head = v;
  }

  val_t *p = val_head;
  while (p && p->next) {
    if (p->next->marked == 0) {
      val_t *v = p->next->next;
      val_free(p->next);
      removed++;
      p->next = v;
    } else {
      p = p->next;
    }
  }

  return removed;
}

int vals_gc (void *exec) {
  static int last_count = 0;
  int count = vals_count();
  if (count - last_count < GC_THRESHOLD)
    return 0;
  vals_unmark();
  vmerror(E_DEBUG3, exec, "[GC] Vals:    %d", count);
  vmerror(E_DEBUG3, exec, "[GC] Marked:  %d", vals_mark(exec));
  vmerror(E_DEBUG3, exec, "[GC] Sweeped: %d", vals_sweep());
  last_count = vals_count();
  vmerror(E_DEBUG3, exec, "[GC] Vals:    %d", last_count);
  return 0;
}

