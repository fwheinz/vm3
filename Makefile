CC=gcc
CFLAGS=-O2 -ggdb -Wall -I. -Werror

OBJ=prog.o stack.o vstack.o val/str.o val/num.o val/arr.o val/val.o vars.o
LIBS=

vm: libvm3.a main.o
	$(CC) -o $@ main.o libvm3.a $(LIBS)

libvm3.a: $(OBJ) op_enum.h op_switch.h op_str.h
	ar -rc libvm3.a $(OBJ)

prog.o: op_switch.h op_enum.h op_str.h

op_enum.h op_switch.h op_str.h: ops.txt
	./mkincludes.sh

clean:
	rm -f $(OBJ) vm libvm3.a main.o
