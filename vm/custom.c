#include "prog.h"

// Add individual opcodes or native functions here

NATIVE(myadd) {
  val_t *v1 = ARG(0);
  val_t *v2 = ARG(1);

  return val_add(v1, v2);
}

OPCODE(mymul) {
  val_t *v1 = POP;
  val_t *v2 = POP;
  PUSH(val_mul(v1, v2));
}
