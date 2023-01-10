#include "prog.h"

int main (void) {
    prog_t *p = prog_new();

    int f_print = prog_new_constant(p, v_str_new_cstr("print"));

    prog_add_num(p, 25);
    prog_add_num(p, 1);
    prog_add_num(p, f_print);
    prog_add_op(p, CONSTANT);
    prog_add_op(p, CALL);
    prog_add_op(p, DISCARD);
    prog_add_num(p, 0);
    prog_add_op(p, JUMP);


    exec_t *e = exec_new(p);
    exec_set_debuglvl(e, E_DEBUG3);
    exec_run(e);
}
