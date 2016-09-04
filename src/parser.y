%{
#include "Token.hpp"
#include <vector>
#include "ParseDriver.hpp"
using namespace ParseDriver;

extern int yylex();
void yyerror(const char *msg){ pushError(msg); }
%}

%no-lines
%union {
  Token* token;
  std::vector<Token*>* list;
}

%token <token>
  ID NUMCONST CHARCONST BOOLCONST
  INT BOOL CHAR RECORD STATIC
  IF ELSE WHILE RETURN BREAK
  NOTEQ MULASS INC ADDASS DEC SUBASS DIVASS LESSEQ EQ GRTEQ NOT AND OR
  MISPLACED
  '(' ')' '[' ']' '{' '}' '<' '>'
  ';' '=' '+' '-' '*' '/' '%' '?' '.' ',' ':'

%type <token> statement
%type <list> statementList
%%

statementList : statementList statement { $1->push_back($2); }
              | statement               { $$ = getResVec(); $$->push_back($1); }
              ;

statement :ID|NUMCONST|CHARCONST|BOOLCONST
          |INT|BOOL|CHAR|RECORD|STATIC
          |IF|ELSE|WHILE|RETURN|BREAK
          |NOTEQ|MULASS|INC|ADDASS|DEC|SUBASS|DIVASS|LESSEQ|EQ|GRTEQ|NOT|AND|OR
          |'('|')'|'['|']'|'{'|'}'|'<'|'>'
          |';'|'='|'+'|'-'|'*'|'/'|'%'|'?'|'.'|','|':'
          |MISPLACED
          ;
