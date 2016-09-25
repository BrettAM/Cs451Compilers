%{
#include "Token.hpp"
#include <vector>
#include "ParseDriver.hpp"
#include "AST.hpp"
using namespace ParseDriver;
using namespace AST;

extern int yylex();
void yyerror(const char *msg){ pushError(msg); }
%}

%no-lines
%union {
  const Token* token;
  AST::Node* node;
  AST::listof<AST::Node*>* builder;
}

%token <token>
  ID NUMCONST CHARCONST BOOLCONST
  INT BOOL CHAR RECORD STATIC
  IF ELSE WHILE RETURN BREAK
  NOTEQ MULASS INC ADDASS DEC SUBASS DIVASS LESSEQ EQ GRTEQ NOT AND OR
  MISPLACED
  '(' ')' '[' ']' '{' '}' '<' '>'
  ';' '=' '+' '-' '*' '/' '%' '?' '.' ',' ':'

%type <node> declaration
%type <builder> declarationList
%type <token> constant
%type <token> other
%%

program : declarationList { rootAST(Siblings(*($1))); delete $1;} ;

declarationList : declarationList declaration { $$ = $1->add($2); }
                | declaration { $$ = (new listof<Node*>())->add($1); }
                ;

declaration : constant { $$ = ConstNode($1); }
            | other { $$ = ConstNode($1); }
            ;

constant : NUMCONST
         | CHARCONST
         | BOOLCONST
         ;

other     : ID
          | INT|BOOL|CHAR|RECORD|STATIC
          | IF|ELSE|WHILE|RETURN|BREAK
          | NOTEQ|MULASS|INC|ADDASS|DEC|SUBASS|DIVASS|LESSEQ|EQ|GRTEQ|NOT|AND|OR
          | '('|')'|'['|']'|'{'|'}'|'<'|'>'
          | ';'|'='|'+'|'-'|'*'|'/'|'%'|'?'|'.'|','|':'
          ;

