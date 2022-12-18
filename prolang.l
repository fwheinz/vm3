%{
#include "prolang.tab.h"
%}

%option noinput nounput noyywrap yylineno

NEQ    "!="
HELLO  "hello"
REPEAT "repeat"
DEFUN  "defun"
IF     "if"
ELSE   "else"
WHILE  "while"
RETURN "return"
NUM    [0-9]+
ID     [a-z_]+
STR    \"[^\"]*\"

%%

{HELLO}  { return hello;  }
{DEFUN}  { return defun;  }
{NEQ}    { return neq;    }
{IF}     { return _if;    }
{ELSE}   { return _else;  }
{WHILE}  { return _while; }
{REPEAT} { return repeat; }
{RETURN} { return _return; }
{NUM}    { yylval.num =   atoi(yytext); return num; }
{ID}     { yylval.id  = strdup(yytext); return id;  }
{STR}    { yylval.str = strdup(yytext); return str; }
(\ |\n)
.        { return yytext[0]; }

%%

