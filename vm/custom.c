#include "prog.h"

// Add individual opcodes or native functions here

NATIVE(quux) {
  printf("Quux\n");
  return &val_undef;
}

OPCODE(foobar) {
  printf("foobar\n");
}
