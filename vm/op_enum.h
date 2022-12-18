typedef struct exec exec_t;
void op_foobar(exec_t *);
void op_add(exec_t *);
void op_sub(exec_t *);
void op_mul(exec_t *);
void op_divide(exec_t *);
void op_modulo(exec_t *);
void op_negate(exec_t *);
void op_inc(exec_t *);
void op_dec(exec_t *);
void op_equal(exec_t *);
void op_notequal(exec_t *);
void op_less(exec_t *);
void op_lessequal(exec_t *);
void op_greater(exec_t *);
void op_greaterequal(exec_t *);
void op_and(exec_t *);
void op_or(exec_t *);
void op_not(exec_t *);
void op_print(exec_t *);
void op_constant(exec_t *);
void op_getvar(exec_t *);
void op_setvar(exec_t *);
void op_getglobal(exec_t *);
void op_setglobal(exec_t *);
void op_jump(exec_t *);
void op_jumpt(exec_t *);
void op_jumpf(exec_t *);
void op_dup(exec_t *);
void op_discard(exec_t *);
void op_condbegin(exec_t *);
void op_condelse(exec_t *);
void op_condend(exec_t *);
void op_loopbegin(exec_t *);
void op_loopbody(exec_t *);
void op_loopend(exec_t *);
void op_loopexit(exec_t *);
void op_looprestart(exec_t *);
void op_halt(exec_t *);
void op_call(exec_t *);
void op_ret(exec_t *);
void op_createval(exec_t *);
void op_mkarray(exec_t *);
void op_index1(exec_t *);
void op_indexas(exec_t *);
void op_getint(exec_t *);
void op_jumprel(exec_t *);
void op_noop(exec_t *);
enum opcodes {
  INVALID,
  FOOBAR,
  ADD,
  SUB,
  MUL,
  DIVIDE,
  MODULO,
  NEGATE,
  INC,
  DEC,
  EQUAL,
  NOTEQUAL,
  LESS,
  LESSEQUAL,
  GREATER,
  GREATEREQUAL,
  AND,
  OR,
  NOT,
  PRINT,
  CONSTANT,
  GETVAR,
  SETVAR,
  GETGLOBAL,
  SETGLOBAL,
  JUMP,
  JUMPT,
  JUMPF,
  DUP,
  DISCARD,
  CONDBEGIN,
  CONDELSE,
  CONDEND,
  LOOPBEGIN,
  LOOPBODY,
  LOOPEND,
  LOOPEXIT,
  LOOPRESTART,
  HALT,
  CALL,
  RET,
  CREATEVAL,
  MKARRAY,
  INDEX1,
  INDEXAS,
  GETINT,
  JUMPREL,
  NOOP,
  MAX_OP
};
