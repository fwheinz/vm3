%{
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "vm/vm.h"

extern int yylineno;
extern FILE *yyin;
int yylex(void);
void yyerror (const char *msg);

typedef struct astnode astnode_t;
struct astnode {
  int type;
  union {
    int num;
    char *id;
    char *str;
  } v;
  struct astnode *child[5];
};
astnode_t *node (int type);

int compile_ast(astnode_t *root);

%}

%define parse.error detailed

%union {
  int num;
  char *id;
  char *str;
  struct astnode *ast;
}

%type <ast> STMTS STMT NUM APARAMS APARAM ID STR FPARAMS FPARAM ELSE

%token stmts funcall func aparams fparams defun _if _else _while _return
%token hello repeat
       <num> num
       <id>  id
       <str> str

%right _return
%right '='
%left '<'
%left '+' '-'
%left '*'

%start START

%%
START: STMTS { compile_ast($1); }

STMTS: STMTS STMT ';' { $$ = node(stmts); $$->child[0] = $1; $$->child[1] = $2; }
     | %empty { $$ = NULL; }

STMT: repeat '(' NUM ')' '{' STMTS '}'  { $$ = node(repeat); $$->child[0] = $3; $$->child[1] = $6; }
    | hello { $$ = node(hello); }
    | STMT '+' STMT { $$ = node('+'); $$->child[0] = $1; $$->child[1] = $3; }
    | STMT '*' STMT { $$ = node('*'); $$->child[0] = $1; $$->child[1] = $3; }
    | STMT '-' STMT { $$ = node('-'); $$->child[0] = $1; $$->child[1] = $3; }
    | STMT '<' STMT { $$ = node('<'); $$->child[0] = $1; $$->child[1] = $3; }
    | ID '(' APARAMS ')' { $$ = node(funcall); $$->child[0] = $1; $$->child[1] = $3; }
    | defun ID '(' FPARAMS ')' '{' STMTS '}'  { $$ = node(func); $$->child[0] = $2; $$->child[1] = $4; $$->child[2] = $7; }
    | ID '=' STMT { $$ = node('='); $$->child[0] = $1; $$->child[1] = $3; }
    | NUM | ID | STR
    | _while '(' STMT ')' '{' STMTS '}' { $$ = node(_while); $$->child[0] = $3; $$->child[1] = $6; }
    | _if '(' STMT ')' '{' STMTS '}' ELSE { $$ = node(_if); $$->child[0] = $3; $$->child[1] = $6; $$->child[2] = $8; }
    | _return STMT { $$ = node(_return); $$->child[0] = $2; }

ELSE: _else '{' STMTS '}' { $$ = $3;   }
    | %empty             { $$ = NULL; }

APARAMS: APARAM | %empty { $$ = NULL; }
APARAM: APARAM ',' STMT { $$ = node(aparams); $$->child[0] = $1; $$->child[1] = $3; }
       | STMT             { $$ = node(aparams); $$->child[0] = NULL; $$->child[1] = $1; }

FPARAMS: FPARAM | %empty { $$ = NULL; }
FPARAM: FPARAM ',' ID   { $$ = node(fparams); $$->child[0] = $1; $$->child[1] = $3; }
       | ID             { $$ = node(fparams); $$->child[0] = NULL; $$->child[1] = $1; }

NUM: num { $$ = node(num); $$->v.num = $1; }

ID: id { $$ = node(id); $$->v.id = $1; }

STR: str { $$ = node(str); $$->v.str = $1; }
%%

prog_t *p;

int compile_ast(astnode_t *root) {
  int c, nrparams, jmp, pc, jt;
  struct var *v;

  if (root == NULL)
    return 0;

  switch (root->type) {
    case stmts:
      compile_ast(root->child[0]);
      compile_ast(root->child[1]);
      prog_add_op(p, DISCARD);
      break;

    case hello:
      c = prog_new_constant(p, v_str_new_cstr("Hello World!"));
      prog_add_num(p, c);
      prog_add_op(p, CONSTANT);
      prog_add_op(p, PRINT);
      break;

    case num:
      prog_add_num(p, root->v.num);
      break;

    case id:
      v = var_get_or_addlocal(root->v.id);
      prog_add_num(p, v->nr);
      prog_add_op(p, GETVAR);
      break;

    case '<':
      compile_ast(root->child[1]);
      compile_ast(root->child[0]);
      prog_add_op(p, LESS);
      break;

    case '=':
      compile_ast(root->child[1]);
      v = var_get_or_addlocal(root->child[0]->v.id);
      prog_add_num(p, v->nr);
      prog_add_op(p, SETVAR);
      prog_add_num(p, v->nr);
      prog_add_op(p, GETVAR);
      break;

    case '+':
      compile_ast(root->child[1]);
      compile_ast(root->child[0]);
      prog_add_op(p, ADD);
      break;

    case '*':
      compile_ast(root->child[1]);
      compile_ast(root->child[0]);
      prog_add_op(p, MUL);
      break;

    case '-':
      compile_ast(root->child[1]);
      compile_ast(root->child[0]);
      prog_add_op(p, SUB);
      break;

    case str:
      c = prog_new_constant(p, v_str_new_cstr(root->v.str));
      prog_add_num(p, c);
      prog_add_op(p, CONSTANT);
      break;

    case repeat:
      prog_add_num(p, root->child[0]->v.num);
      pc = prog_add_op(p, DUP);
      jt = prog_add_num(p, 0);
      prog_add_op(p, JUMPF);
      prog_add_num(p, 1);
      prog_add_op(p, SUB);
      prog_add_num(p, 0);
      prog_add_op(p, SUB);
      compile_ast(root->child[1]);
      prog_add_num(p, pc);
      prog_add_op(p, JUMP);
      prog_set_num(p, jt, prog_next_pc(p)); 
      prog_add_op(p, DISCARD);
      break;

    case _if:
#ifdef METHOD1
      compile_ast(root->child[0]);
      dstelse = prog_add_num(p, 0);
      prog_add_op(p, JUMPF);
      compile_ast(root->child[1]);
      dstend = prog_add_num(p, 0);
      prog_add_op(p, JUMP);
      prog_set_num(p, dstelse, prog_next_pc(p));
      compile_ast(root->child[2]);
      prog_set_num(p, dstend, prog_next_pc(p));
#else
      compile_ast(root->child[0]);
      prog_add_op(p, CONDBEGIN);
      compile_ast(root->child[1]);
      prog_add_op(p, CONDELSE);
      compile_ast(root->child[2]);
      prog_add_op(p, CONDEND);
      prog_add_num(p, 0);
#endif
      break;

    case _while:
#ifdef METHOD1
      int loopstart = prog_next_pc(p);
      compile_ast(root->child[0]);
      int jumpend = prog_add_num(p, 0);
      prog_add_op(p, JUMPF);
      compile_ast(root->child[1]);
      prog_add_num(p, loopstart);
      prog_add_op(p, JUMP);
      prog_set_num(p, jumpend, prog_next_pc(p));
#else
      prog_add_op(p, LOOPBEGIN);
      compile_ast(root->child[0]);
      prog_add_op(p, LOOPBODY);
      compile_ast(root->child[1]);
      prog_add_op(p, LOOPEND);
      prog_add_num(p, 0);
#endif
      break;

    case funcall:
      nrparams = 0;
      if (root->child[1] != NULL)
        nrparams = compile_ast(root->child[1])+1;
      prog_add_num(p, nrparams);
      c = prog_new_constant(p, v_str_new_cstr(root->child[0]->v.id));
      prog_add_num(p, c);
      prog_add_op(p, CONSTANT);
      prog_add_op(p, CALL);
      break;

    case func:
      jmp = prog_add_num(p, 0);
      prog_add_op(p, JUMP);
      var_reset();
      compile_ast(root->child[1]);
      prog_register_function(p, v_str_new_cstr(root->child[0]->v.id), prog_next_pc(p));
      compile_ast(root->child[2]);
      prog_add_num(p, 0);
      prog_add_op(p, RET);
      prog_set_num(p, jmp, prog_next_pc(p));
      prog_add_num(p, 0);
      break;

    case _return:
      compile_ast(root->child[0]);
      prog_add_op(p, RET);
      break;

    case fparams:
      compile_ast(root->child[0]);
      var_add_local(root->child[1]->v.str);
      break;

    case aparams:
      if (root->child[0] != NULL)
        c = compile_ast(root->child[0]) + 1;
      else
        c = 0;
      compile_ast(root->child[1]);
      return c;
      break;

    default:
      printf("Unhandled AST node %d\n", root->type);
      assert(0);
      break;
  }

  return 0;
}

astnode_t *node (int type) {
  astnode_t *n = calloc(1, sizeof *n);
  n->type = type;
  return n;
}

void yyerror (const char *msg) {
  fprintf(stderr, "Line %d: %s\n", yylineno, msg);
}

int main (int argc, char **argv) {
  p = prog_new();
  prog_add_op(p, FOOBAR);

  yyin = fopen(argv[1], "r");
  int st = yyparse();

  prog_dump(p);
  prog_write(p, "foo.pc");
  exec_t *e = exec_new(p);
  exec_set_debuglvl(e, E_DEBUG2);
  exec_run(e);

  return st;
}
