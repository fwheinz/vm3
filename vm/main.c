#include "prog.h"

int main (void) {
    prog_t *prog = prog_new();
    prog_add_num(prog, 20);
    prog_add_num(prog, 30);
    prog_add_op (prog, ADD);
    prog_add_num(prog, 65);
    prog_add_op (prog, SUB);
    prog_add_num(prog, 10);
    prog_add_op (prog, LESS);
    exec_t *exec = exec_new(prog);
    exec_run(exec);
    printstack(exec);
}

