#include "prog.h"

int main (void) {
	prog_t *p = prog_new();

	prog_add_num(p, 1);
	int loop1 = prog_add_op(p, INC);
	prog_add_op(p, DUP);
	prog_add_op(p, PRINT);
	prog_add_op(p, DUP);
	prog_add_num(p, 1000);
	prog_add_op(p, LESS);
	int brkjmp = prog_add_num(p, -1);
	prog_add_op(p, JUMPT);
	prog_add_num(p, loop1);
	prog_add_op(p, JUMP);
	int brktgt = prog_add_op(p, HALT);

	prog_set_num(p, brkjmp, brktgt);

	exec_t *e = exec_new(p);
	exec_run(e);



	return 0;
}

