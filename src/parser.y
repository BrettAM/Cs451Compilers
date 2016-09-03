%{
#include <iostream>
#include "Token.hpp"
int vars[26];

extern int yylex();

void yyerror(const char *msg){
}
%}

%union {
  int value;
}

%token <value> T

%%

statementlist : statement statementlist
              | statement
              ;

statement : T { std::cout << "Token"; }
          ;

%%
