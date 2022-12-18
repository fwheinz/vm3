CC=gcc
CFLAGS=-ggdb -Wall -Ivm
OBJS=
TARGETS=prolang

all: $(TARGETS)

%: %.lex.o %.tab.o $(OBJS) vm/libvm3.a
	$(CC) -o $@ $^

%.tab.o: %.tab.c %.tab.h
	$(CC) -c $(CFLAGS) $< -o $@

%.lex.o: %.lex.c %.tab.h
	$(CC) -c $(CFLAGS) $< -o $@

%.tab.c %.tab.h: %.y
	bison --defines -t $^

%.lex.c: %.l
	flex -o $@ $^

vm/libvm3.a:
	make -C vm

clean:
	make -C vm clean
	rm -f *.lex.* *.tab.* *.o $(TARGETS)

.PHONY: vm/libvm3.a

.PRECIOUS: prolang.tab.h
