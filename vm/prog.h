#ifndef _PROG_T_H
#define _PROG_T_H

#include <assert.h>

#include "stack.h"
#include "vstack.h"

#include "op_enum.h"

enum errors {
  E_ERR,
  E_WARN,
  E_INFO,
  E_DEBUG,
  E_DEBUG2,
  E_DEBUG3
};

extern char *lvlstr[];

struct prog {
  val_t *ops;
  val_t *constants;
  val_t *functions;
};

typedef struct prog prog_t;

struct exec {
  prog_t *prog;
  vstack_t *vstack;
  vstack_t *vars;
  val_t    *global_vars;
  int debuglvl;

  int pc;
};

typedef struct exec exec_t;

#define OP(x) ((x&0xFFFFFF00) == 0x80000000 ? x & 0xFF : 0)
#define MKOP(x) ((int)(0x80000000|(unsigned char)x))

#define MINARGS(n) assert(vstack_size(exec->vstack) >= (n))
#define OPCODE(name) void op_##name (exec_t *exec)
#define POP vstack_pop(exec->vstack)
#define PEEK vstack_peek(exec->vstack)
#define PUSH(x) vstack_push(exec->vstack, (x))
#define NATIVE(name) val_t *native_##name (exec_t *exec, val_t *args)
#define ARG(x) arr_get(args->u.arr, (x)+2)

prog_t *prog_new (void);
void    prog_dump (prog_t *p);
int     prog_add_op (prog_t *prog, int op);
int     prog_add_num (prog_t *prog, int num);
int     prog_set_op  (prog_t *prog, int pc, int op);
int     prog_set_num (prog_t *prog, int pc, int num);
void    prog_set_constant (prog_t *prog, int index, val_t *c);
int     prog_new_constant (prog_t *prog, val_t *c);
val_t  *prog_get_constant (prog_t *prog, int index);
void    prog_register_function (prog_t *prog, val_t *name, int pc);
int     prog_next_pc (prog_t *prog);

void    printstack (exec_t *exec);

val_t  *call_native (exec_t *, char *id, val_t *args);

prog_t *prog_read (char *filename);
void prog_write(prog_t *p, char *filename);

exec_t *exec_new  (prog_t *prog);
int     exec_step (exec_t *exec);
void    exec_run  (exec_t *exec);
void    exec_set_debuglvl (exec_t *exec, int lvl);

#define VAR_BLOCKBORDER (-1)

struct var {
  char *id;
  int global;
  int nr;
  int type;
};

struct varalloc {
  struct var *globals;
  int nrglobals;
  struct var *locals;
  int nrlocals;
};

struct var *var_add_global (char *id);
struct var *var_add_local(char *id);
struct var *var_get_or_addlocal(char *id);
void var_enter_block (void);
void var_leave_block (void);
void var_reset (void);

int vmerror (int lvl, exec_t *ctx, char *format, ...);

#endif

