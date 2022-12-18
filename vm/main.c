#include "prog.h"

int main (void) {
    prog_t *p = prog_new();
    prog_add_num(p, 50);
    prog_add_num(p, 70);
    prog_add_op(p, SUB);
    prog_add_op(p, PRINT);
    exec_t *e = exec_new(p);
    exec_set_debuglvl(e, E_DEBUG3);
    exec_run(e);
}
