%{
#include <iostream>
#include "Token.hpp"
#include "ParseDriver.hpp"

using namespace ParseDriver;

extern int yylex();

void yyerror(const char *msg){
  pushError(msg);
}
%}

%union {
  Token* token;
}

%token <token> ID
%token <token> NUMCONST
%token <token> CHARCONST
%token <token> BOOLCONST
%token <token> STATIC
%token <token> INT
%token <token> BOOL
%token <token> CHAR
%token <token> IF
%token <token> ELSE
%token <token> WHILE
%token <token> RETURN
%token <token> BREAK
%token <token> RECORD
%token <token> NOTEQ
%token <token> MULASS
%token <token> INC
%token <token> ADDASS
%token <token> DEC
%token <token> SUBASS
%token <token> DIVASS
%token <token> LESSEQ
%token <token> EQ
%token <token> GRTEQ
%token <token> NOT
%token <token> AND
%token <token> OR
%token <token> MISPLACED
%token <token> '('
%token <token> ')'
%token <token> '['
%token <token> ']'
%token <token> '{'
%token <token> '}'
%token <token> ';'
%token <token> '='
%token <token> '<'
%token <token> '>'
%token <token> '+'
%token <token> '-'
%token <token> '*'
%token <token> '/'
%token <token> '%'
%token <token> '?'
%token <token> '.'
%token <token> ','
%token <token> ':'
%%

statementlist : statement statementlist
              | statement
              ;

statement : ID { pushToken($1); }
          | NUMCONST { pushToken($1); }
          | CHARCONST { pushToken($1); }
          | BOOLCONST { pushToken($1); }
          | STATIC { pushToken($1); }
          | INT { pushToken($1); }
          | BOOL { pushToken($1); }
          | CHAR { pushToken($1); }
          | IF { pushToken($1); }
          | ELSE { pushToken($1); }
          | WHILE { pushToken($1); }
          | RETURN { pushToken($1); }
          | BREAK { pushToken($1); }
          | RECORD { pushToken($1); }
          | NOTEQ { pushToken($1); }
          | MULASS { pushToken($1); }
          | INC { pushToken($1); }
          | ADDASS { pushToken($1); }
          | DEC { pushToken($1); }
          | SUBASS { pushToken($1); }
          | DIVASS { pushToken($1); }
          | LESSEQ { pushToken($1); }
          | EQ { pushToken($1); }
          | GRTEQ { pushToken($1); }
          | NOT { pushToken($1); }
          | AND { pushToken($1); }
          | OR { pushToken($1); }
          | MISPLACED { pushToken($1); }
          | '(' { pushToken($1); }
          | ')' { pushToken($1); }
          | '[' { pushToken($1); }
          | ']' { pushToken($1); }
          | '{' { pushToken($1); }
          | '}' { pushToken($1); }
          | ';' { pushToken($1); }
          | '=' { pushToken($1); }
          | '<' { pushToken($1); }
          | '>' { pushToken($1); }
          | '+' { pushToken($1); }
          | '-' { pushToken($1); }
          | '*' { pushToken($1); }
          | '/' { pushToken($1); }
          | '%' { pushToken($1); }
          | '?' { pushToken($1); }
          | '.' { pushToken($1); }
          | ',' { pushToken($1); }
          | ':' { pushToken($1); }
          ;
