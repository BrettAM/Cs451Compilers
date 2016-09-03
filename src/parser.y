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

%token <token> TOKEN

%%

statementlist : statement statementlist
              | statement
              ;

statement : TOKEN { pushToken($1); }
          ;

