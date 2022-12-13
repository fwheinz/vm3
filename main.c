#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "prog.h"


void usage (void) {
  fprintf(stderr, "vm [-v] [-d] <file>\n"
      "        -v  Increase verbosity\n"
      "        -d  Dump file (no execution)\n"
      "    <file>  Bytecode filename\n"
      "\n"
      );

  exit(EXIT_FAILURE);
}

int main (int argc, char **argv) {
  int verbose = 0, dump = 0, opt;

  while ((opt = getopt(argc, argv, "vd")) != -1) {
    switch (opt) {
      case 'v':
        verbose++;
        break;
      case 'd':
        dump++;
        break;
      default:
        exit(EXIT_FAILURE);
    }
  }


  if (optind >= argc) {
    usage();
    exit(EXIT_FAILURE);
  }

  printf("Reading file %s...\n", argv[optind]);
  prog_t *prog = prog_read(argv[optind]);

  if (dump) {
    prog_dump(prog);
  } else {
    exec_t *exec = exec_new(prog);
    exec_set_debuglvl(exec, verbose);
    exec_run(exec);
  }

  return 0;
}

