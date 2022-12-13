#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "prog.h"
#include "op_str.h"

char *lvlstr[] = {
  "err",
  "warn",
  "info",
  "debug",
  "debug2",
  "debug3",
};

int vmerror (int lvl, exec_t *ctx, char *format, ...) {
  char buf[1024];
  va_list args;
  va_start(args, format);
  vsnprintf(buf, sizeof(buf), format, args);
  va_end(args);
  if (!ctx || ctx->debuglvl >= lvl)
    fprintf(stderr, "%s: %s\n", lvlstr[lvl], buf);
  if (lvl == E_ERR)
    exit(EXIT_FAILURE);

  return 0;
}


prog_t *prog_new (void) {
  val_init();
  assert(MAX_OP <= 255);
  prog_t *prog = malloc(sizeof *prog);
  prog->ops = v_arr_create();
  prog->constants = v_arr_create();

  return prog;
}

static int _prog_add_raw (prog_t *prog, int val) {
  arr_push(prog->ops->u.arr, v_num_new_int(val));

  return prog->ops->u.arr->size-1;
}

static int _prog_set_raw (prog_t *prog, int pc, int val) {
  arr_set(prog->ops->u.arr, pc, v_num_new_int(val));

  return pc;
}

int prog_get_op (prog_t *prog, int pc) {
  val_t *v = arr_get(prog->ops->u.arr, pc);
  assert(v->type == T_NUM);

  return v->u.num;
}

int prog_nr_ops (prog_t *prog) {
  return prog->ops->u.arr->size;
}

int prog_set_op (prog_t *prog, int pc, int op) {
  if (op >= MAX_OP) {
    return -1;
  }
  return _prog_set_raw(prog, pc, MKOP(op));
}

int prog_set_num (prog_t *prog, int pc, int num) {
  if (OP(num)) {
    return -1;
  }
  return _prog_set_raw(prog, pc, num);
}


int prog_add_op (prog_t *prog, int op) {
  if (op >= MAX_OP) {
    return -1;
  }
  return _prog_add_raw(prog, MKOP(op));
}

int prog_add_num (prog_t *prog, int num) {
  if (OP(num)) {
    return -1;
  }
  return _prog_add_raw(prog, num);
}

void prog_set_constant (prog_t *prog, int idx, val_t *c) {
  arr_set(prog->constants->u.arr, idx, c);
}

int prog_new_constant (prog_t *prog, val_t *c) {
  int idx = prog->constants->u.arr->size;
  arr_set(prog->constants->u.arr, idx, c);

  return idx;
}

val_t *prog_get_constant (prog_t *prog, int idx) {
  return arr_get(prog->constants->u.arr, idx);
}

int prog_next_pc (prog_t *prog) {
  return prog_nr_ops(prog);
}

void prog_register_function (prog_t *prog, int idx, int pc) {
  arr_set(prog->constants->u.arr, idx, v_num_new_int(pc));
}

exec_t *exec_new (prog_t *prog) {

  exec_t *exec = malloc (sizeof *exec);
  exec->prog = prog;

  exec->vstack = vstack_new();
  exec->vars = vstack_new();
  val_t *vars = v_arr_create();
  vstack_push(exec->vars, vars);
  val_push(vars, v_num_new_int(0));
  val_push(vars, v_arr_create());

  exec->global_vars = v_arr_create();
  exec->pc = 0;
  exec->debuglvl = E_WARN;

  return exec;
}

void exec_set_debuglvl (exec_t *exec, int lvl) {
  if (lvl > E_DEBUG3)
    lvl = E_DEBUG3;
  exec->debuglvl = lvl;
}

OPCODE(invalid) {
  vmerror(E_ERR, exec, "Error: invalid opcode (0)");
}

OPCODE(add) {
  MINARGS(2);
  val_t *a1 = POP;
  val_t *a2 = POP;
  val_t *r = val_add(a1, a2);
  PUSH(r);
}

OPCODE(sub) {
  MINARGS(2);
  val_t *a1 = POP;
  val_t *a2 = POP;
  val_t *r = val_sub(a1, a2);
  PUSH(r);
}

OPCODE(divide) {
  MINARGS(2);
  val_t *a1 = POP;
  val_t *a2 = POP;
  val_t *r = val_div(a1, a2);
  PUSH(r);
}

OPCODE(modulo) {
  MINARGS(2);
  val_t *a1 = POP;
  val_t *a2 = POP;
  val_t *r = val_mod(a1, a2);
  PUSH(r);
}

OPCODE(discard) {
  MINARGS(1);
  POP;
}

OPCODE(mul) {
  MINARGS(2);
  val_t *a1 = POP;
  val_t *a2 = POP;
  val_t *r = val_mul(a1, a2);
  PUSH(r);
}

OPCODE(print) {
  MINARGS(1);
  val_t *a = POP;
  val_t *s = val_to_string(a);
  printf("> %s\n", cstr(s));
}

OPCODE(mkarray) {
  MINARGS(1);
  val_t *n = POP;
  assert(n->type == T_NUM);
  int nr = n->u.num;
  MINARGS(nr);
  val_t *a = v_arr_create();
  for (int i = 0; i < nr; i++) {
    arr_set(a->u.arr, i, POP);
  }
  PUSH(a);
}

OPCODE(indexas) {
  MINARGS(3);
  val_t *a = POP;
  val_t *i = POP;
  val_t *v = POP;

  v_arr_index_assign(a, i, v);
  PUSH(v);
}

OPCODE(index1) {
  MINARGS(2);
  val_t *a = POP;
  val_t *i = POP;

  val_t *v = v_arr_index(a, i);
  PUSH(v);
}

OPCODE(constant) {
  MINARGS(1);
  val_t *i = POP;
  assert(i->type == T_NUM);
  int idx = i->u.num;
  val_t *c = prog_get_constant(exec->prog, idx);
  PUSH(c);
}

OPCODE(call) {
  MINARGS(2);

  val_t *idx = POP;
  assert(idx->type == T_NUM);
  val_t *new_pc = arr_get(exec->prog->constants->u.arr, idx->u.num);
  assert(new_pc->type == T_NUM);

  val_t *nrargs = POP;
  assert(nrargs->type == T_NUM);
  int nr = nrargs->u.num;
  MINARGS(nr);
  val_t *a = v_arr_create();
  arr_set(a->u.arr, 0, v_num_new_int(exec->pc)); // Return value
  arr_set(a->u.arr, 1, v_arr_create());          // Loop stack
  for (int i = 0; i < nr; i++) {                 // Arguments
    arr_set(a->u.arr, i+2, POP);
  }
  vstack_push(exec->vars, a);

  exec->pc = new_pc->u.num - 1; // Because we increment later
}

OPCODE(jump) {
  MINARGS(2);
  val_t *dest = POP;
  assert(dest->type == T_NUM);
  int new_pc = dest->u.num;
  val_t *cond = POP;
  assert(cond->type == T_NUM);
  if (cond->u.num != 0)
    exec->pc = new_pc - 1; // Because we increment later
}

OPCODE(not) {
  MINARGS(1);
  val_t *v = POP;
  PUSH(v_num_new_int(!v->u.num));
}

OPCODE(jumprel) {
  MINARGS(2);
  val_t *dest = POP;
  assert(dest->type == T_NUM);
  int new_pc = dest->u.num;
  val_t *cond = POP;
  assert(cond->type == T_NUM);
  if (cond->u.num != 0)
    exec->pc += new_pc - 1; // Because we increment later
}

OPCODE(ret) {
  val_t *v = vstack_pop(exec->vars);
  val_t *a = arr_get(v->u.arr, 0);
  assert(a->type == T_NUM);
  exec->pc = a->u.num;
}

OPCODE(loopbegin) {
  val_t *vars = vstack_peek(exec->vars);
  assert(vars->type == T_ARR);
  val_t *loopstack = arr_get(vars->u.arr, 1);
  assert(loopstack->type == T_ARR);
  val_push(loopstack, v_num_new_int(exec->pc));
}

OPCODE(loopend) {
  val_t *vars = vstack_peek(exec->vars);
  assert(vars->type == T_ARR);
  val_t *loopstack = arr_get(vars->u.arr, 1);
  assert(loopstack->type == T_ARR);
  val_t *pc = val_peek(loopstack);
  assert(pc->type == T_NUM);
  exec->pc = pc->u.num;
}

OPCODE(loopexit) {
  int count = 1;
  while (count > 0) {
    exec->pc++;
    if (exec->pc >= prog_nr_ops(exec->prog))
      break;
    if (prog_get_op(exec->prog, exec->pc) == MKOP(LOOPEND)) {
      count--;
    } else if (prog_get_op(exec->prog, exec->pc) == MKOP(LOOPBEGIN)) {
      count++;
    }
  }
  val_t *vars = vstack_peek(exec->vars);
  assert(vars->type == T_ARR);
  val_t *loopstack = arr_get(vars->u.arr, 1);
  assert(loopstack->type == T_ARR);
  val_pop(loopstack);
}

OPCODE(looprestart) {
  op_loopend(exec);
}

OPCODE(condbegin) {
  MINARGS(1);
  val_t *cond = POP;
  assert(cond->type == T_NUM);
  int v = cond->u.num;
  if (v)
    return;

  int count = 1;
  while (count > 0) {
    exec->pc++;
    if (exec->pc >= prog_nr_ops(exec->prog))
      break;
    if (prog_get_op(exec->prog, exec->pc) == MKOP(CONDBEGIN)) {
      count++;
    } else if (prog_get_op(exec->prog, exec->pc) == MKOP(CONDEND)) {
      count--;
    } else if (prog_get_op(exec->prog, exec->pc) == MKOP(CONDELSE) &&
        count == 1) {
      break;
    }
  }
}

OPCODE(condelse) {
  int count = 1;
  while (count > 0) {
    exec->pc++;
    if (exec->pc >= prog_nr_ops(exec->prog))
      break;
    if (prog_get_op(exec->prog, exec->pc) == MKOP(CONDBEGIN)) {
      count++;
    } else if (prog_get_op(exec->prog, exec->pc) == MKOP(CONDEND)) {
      count--;
    }
  }
}

OPCODE(condend) {
}

OPCODE(noop) {
  // Nothing to see here...
  // really...
}

OPCODE(getint) {
  char buf[100];
  char *s = fgets(buf, sizeof(buf), stdin);
  int nr = s ? atoi(s) : 0;
  val_t *v = v_num_new_int(nr);
  PUSH(v);
}

OPCODE(getvar) {
  MINARGS(1);
  val_t *vars = vstack_peek(exec->vars);
  assert(vars->type == T_ARR);
  val_t *nrvar = POP;
  assert(nrvar->type == T_NUM);
  val_t *var = arr_get(vars->u.arr, nrvar->u.num + 2);
  PUSH(var);
}

OPCODE(setvar) {
  MINARGS(2);
  val_t *vars = vstack_peek(exec->vars);
  assert(vars->type == T_ARR);
  val_t *nrvar = POP;
  assert(nrvar->type == T_NUM);
  val_t *val = POP;
  arr_set(vars->u.arr, nrvar->u.num + 2, val);
}

OPCODE(getglobal) {
  MINARGS(1);
  val_t *nrvar = POP;
  assert(nrvar->type == T_NUM);
  val_t *var = arr_get(exec->global_vars->u.arr, nrvar->u.num);
  PUSH(var);
}

OPCODE(setglobal) {
  MINARGS(2);
  val_t *nrvar = POP;
  assert(nrvar->type == T_NUM);
  val_t *val = PEEK;
  arr_set(exec->global_vars->u.arr, nrvar->u.num, val);
}

OPCODE(equal) {
  MINARGS(2);
  val_t *v1 = POP;
  val_t *v2 = POP;
  
  int eq = val_cmp(v1, v2) == 0;
  PUSH(v_num_new_int(eq));
} 

OPCODE(notequal) {
  MINARGS(2);
  val_t *v1 = POP;
  val_t *v2 = POP;
  
  int neq = val_cmp(v1, v2) != 0;
  PUSH(v_num_new_int(neq));
} 

OPCODE(less) {
  MINARGS(2);
  val_t *v1 = POP;
  val_t *v2 = POP;
  
  int less = val_cmp(v1, v2) < 0;
  PUSH(v_num_new_int(less));
} 

OPCODE(lessequal) {
  MINARGS(2);
  val_t *v1 = POP;
  val_t *v2 = POP;
  
  int lesseq = val_cmp(v1, v2) <= 0;
  PUSH(v_num_new_int(lesseq));
} 

OPCODE(greater) {
  MINARGS(2);
  val_t *v1 = POP;
  val_t *v2 = POP;
  
  int greater = val_cmp(v1, v2) > 0;
  PUSH(v_num_new_int(greater));
} 

OPCODE(greaterequal) {
  MINARGS(2);
  val_t *v1 = POP;
  val_t *v2 = POP;
  
  int greaterequal = val_cmp(v1, v2) >= 0;
  PUSH(v_num_new_int(greaterequal));
} 

void prog_dump (prog_t *prog) {
  for (int i = 0; i < prog_nr_ops(prog); i++) {
    int op = prog_get_op(prog, i);
    if (!OP(op)) {
      printf("%3d: PUSH %d\n", i, op);
    } else {
      printf("%3d: OP %s (%d)\n", i, opcodestr[OP(op)], OP(op));
    }
  }
}

void prog_write (prog_t *p, char *filename) {
  FILE *f = fopen(filename, "w");

  val_serialize(f, p->constants);
  val_serialize(f, p->ops);
}

prog_t *prog_read (char *filename) {
  FILE *f = fopen(filename, "r");
  if (!f) {
    printf("File %s not found!\n", filename);
    assert(0);
  }
  prog_t *p = prog_new();
  p->constants = val_deserialize(f);
  p->ops = val_deserialize(f);

  return p;
}


int exec_step (exec_t *exec) {
  if (exec->pc >= prog_nr_ops(exec->prog))
    return 0;

  int op = prog_get_op(exec->prog, exec->pc);
  
  if (!OP(op)) {
    vmerror(E_DEBUG, exec, "%3d: PUSH %d", exec->pc, op);
    vstack_push(exec->vstack, v_num_new_int(op));
  } else {
    vmerror(E_DEBUG, exec, "%3d: OP %s (%d)", exec->pc, opcodestr[OP(op)], OP(op));
    switch (OP(op)) {
#include "op_switch.h"
      default:
        vmerror(E_WARN, exec, "Warning: Unknown op: %d\n", OP(op));
        break;
    }
  }
  exec->pc++;
  vals_gc(exec);

  return 1;
}

void exec_run (exec_t *exec) {
  int st;

  do {
    st = exec_step(exec);
  } while (st > 0);
  vmerror(E_DEBUG, exec, "Stack size: %d\n", exec->vstack->size);
}


