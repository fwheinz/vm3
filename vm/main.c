#include "prog.h"

int main (void) {
    prog_t *p = prog_new();

    prog_add_num(p, 10);
    prog_add_num(p, 20);
    prog_add_num(p, 2);
    prog_add_op(p, MKARRAY);
    prog_add_num(p, 30);
    prog_add_num(p, 40);
    prog_add_num(p, 2);
    prog_add_op(p, MKARRAY);
    prog_add_op(p, ADD);
    prog_add_op(p, PRINT);


    exec_t *e = exec_new(p);
    exec_set_debuglvl(e, E_DEBUG3);
    exec_run(e);
}
