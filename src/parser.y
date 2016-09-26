%{
#include "Token.hpp"
#include <vector>
#include "ParseDriver.hpp"
#include "AST.hpp"
#include "Type.hpp"
#include "IdComp.hpp"
using namespace ParseDriver;
using namespace AST;

extern int yylex();
void yyerror(const char *msg){ pushError(msg); }
%}

%no-lines
%define parse.error verbose
%union {
  const Token* token;
  AST::Node* node;
  AST::listof<AST::Node*>* builder;
  Type* type;
  IdComp idComp;
  AST::listof<IdComp>* idCompList;
}

%token <token>
  ID NUMCONST CHARCONST BOOLCONST
  INT BOOL CHAR RECORD STATIC
  IF ELSE WHILE RETURN BREAK
  NOTEQ MULASS INC ADDASS DEC SUBASS DIVASS LESSEQ EQ GRTEQ NOT AND OR
  MISPLACED RECTYPE
  '(' ')' '[' ']' '{' '}' '<' '>'
  ';' '=' '+' '-' '*' '/' '%' '?' '.' ',' ':'

%type <builder> declarationList
%type <builder> declaration
%type <node> recDeclaration
%type <builder> varDeclaration
%type <builder> scopedVarDeclaration
%type <idCompList> varDeclList
%type <idComp> varDeclInitialize
%type <idComp> varDeclId
%type <type> scopedTypeSpecifier
%type <type> typeSpecifier
%type <type> returnTypeSpecifier
%type <node> funDeclaration
%type <node> params
%type <builder> paramList
%type <builder> paramTypeList
%type <idCompList> paramIdList
%type <idComp> paramId
%type <node> statement
%type <node> subStatement
%type <node> matchedStmt
%type <node> unmatchedStmt
%type <node> compoundStmt
%type <builder> localDeclarations
%type <builder> statementList
%type <node> expressionStmt
%type <node> returnStmt
%type <node> breakStmt
%type <node> expression
%type <node> simpleExpression
%type <node> andExpression
%type <node> unaryRelExpression
%type <node> relExpression
%type <token> relop
%type <node> sumExpression
%type <token> sumop
%type <node> term
%type <token> mulop
%type <node> unaryExpression
%type <token> unaryop
%type <node> factor
%type <node> mutable
%type <node> immutable
%type <node> call
%type <node> args
%type <builder> argList
%type <token> constant
%%

program : declarationList { rootAST(Siblings(*($1))); delete $1;} ;

declarationList : declarationList declaration { $$ = $1->addAll($2); delete $2; }
                | declaration { $$ = $1; }
                ;

declaration : varDeclaration
            | funDeclaration { $$ = (new listof<Node*>)->add($1); }
            | recDeclaration { $$ = (new listof<Node*>)->add($1); }
            ;

recDeclaration : RECORD ID '{' localDeclarations '}'
                 {
                   pushGlobal($2);
                   $$ = RecordNode($2, Siblings(*($4)));
                   delete $4;
                 }
               ;

varDeclaration : typeSpecifier varDeclList ';'
        {
          listof<Node*>* nodelist = new listof<Node*>();
          Type type = *$1;
          std::vector<IdComp> ids = *$2;
          delete $1;
          delete $2;

          for(int i=0; i<ids.size(); i++){
            IdComp id = ids.at(i);
            nodelist->add(
              VarDecl(
                id.id,
                (id.arraylen == -1)? type : type.mkArray(id.arraylen),
                id.init
              )
            );
          }
          $$ = nodelist;
        };

scopedVarDeclaration : scopedTypeSpecifier varDeclList ';'
        {
          listof<Node*>* nodelist = new listof<Node*>();
          Type type = *$1;
          std::vector<IdComp> ids = *$2;
          delete $1;
          delete $2;

          for(int i=0; i<ids.size(); i++){
            IdComp id = ids.at(i);
            nodelist->add(
              VarDecl(
                id.id,
                (id.arraylen == -1)? type : type.mkArray(id.arraylen),
                id.init
              )
            );
          }
          $$ = nodelist;
        };

varDeclList : varDeclList ',' varDeclInitialize { $$ = $1->add($3); }
            | varDeclInitialize                 { $$ = (new listof<IdComp>())->add($1); }
            ;

varDeclInitialize : varDeclId
                  | varDeclId ':' simpleExpression { $$ = $1; $$.init = $3; }
                  ;

varDeclId : ID                  { IdComp x = {$1, -1, NULL}; $$ = x; }
          | ID '[' NUMCONST ']' { IdComp x = {$1, ((NumConst*)$3)->value, NULL}; $$ = x; }
          ;

scopedTypeSpecifier : STATIC typeSpecifier { $$ = $2; $$->mkStatic(); }
                    | typeSpecifier
                    ;

typeSpecifier : returnTypeSpecifier
              | RECTYPE             { $$ = new Type(Type::RECORD()); }
              ;

returnTypeSpecifier : INT  { $$ = new Type(Type::INT); }
                    | BOOL { $$ = new Type(Type::BOOL); }
                    | CHAR { $$ = new Type(Type::CHAR); }
                    ;

funDeclaration : typeSpecifier ID '(' params ')' statement { $$ = FuncDecl($2, *$1, $4, $6); delete $1; }
               | ID '(' params ')' statement               { $$ = FuncDecl($1, Type::VOID, $3, $5); }
               ;

params : paramList   { $$ = Siblings(*($1)); delete $1; }
       | /* empty */ { $$ = Leaf(); }
       ;

paramList : paramList ';' paramTypeList { $$ = $1->addAll($3); delete $3; }
          | paramTypeList
          ;

paramTypeList : typeSpecifier paramIdList
                {
                  //
                  $$ = (new listof<Node *>());
                  Type type = *$1;
                  std::vector<IdComp> ids = *$2;
                  delete $1;
                  delete $2;

                  for(int i=0; i<ids.size(); i++){
                    IdComp id = ids.at(i);
                    $$->add(
                      Parameter(
                        id.id,
                        (id.arraylen == -1)? type : type.mkArray(id.arraylen)
                      )
                    );
                  }
                };

paramIdList : paramIdList ',' paramId { $$ = $1->add($3); }
            | paramId                 { $$ = (new listof<IdComp>())->add($1); }
            ;

paramId : ID         { IdComp x = {$1, -1, NULL}; $$ = x; }
        | ID '[' ']' { IdComp x = {$1,  0, NULL}; $$ = x; }
        ;

statement : matchedStmt
          | unmatchedStmt
          ;

subStatement : expressionStmt
             | compoundStmt
             | returnStmt
             | breakStmt
             ;

matchedStmt : subStatement
            | IF '(' simpleExpression ')' matchedStmt ELSE matchedStmt
              { $$ = IfNode($1, $3, $5, $7); }
            | WHILE '(' simpleExpression ')' matchedStmt
              { $$ = WhileNode($1, $3, $5); }
            ;

unmatchedStmt : IF '(' simpleExpression ')' matchedStmt ELSE unmatchedStmt
                { $$ = IfNode($1, $3, $5, $7); }
              | IF '(' simpleExpression ')' matchedStmt
                { $$ = IfNode($1, $3, $5, Leaf()); }
              | IF '(' simpleExpression ')' unmatchedStmt
                { $$ = IfNode($1, $3, $5, Leaf()); }
              | WHILE '(' simpleExpression ')' unmatchedStmt
                { $$ = WhileNode($1, $3, $5); }
              ;

compoundStmt : '{' localDeclarations statementList '}'
               {
                 $$ = Compound($1, Siblings(*($2)), Siblings(*($3)));
                 delete $2;
                 delete $3;
               }
             ;

localDeclarations : localDeclarations scopedVarDeclaration { $$ = $1->addAll($2); delete $2; }
                  | /* empty */                            { $$ = (new listof<Node*>()); }
                  ;

statementList : statementList statement { $$ = $1->add($2); }
              | /* empty */             { $$ = (new listof<Node*>()); }
              ;

expressionStmt : expression ';'
               | ';' { $$ = Leaf(); }
               ;

returnStmt : RETURN ';'            { $$ = ReturnNode($1, Leaf()); }
           | RETURN expression ';' { $$ = ReturnNode($1, $2); }
           ;

breakStmt : BREAK ';' { $$ = BreakNode($1); }
          ;

expression : mutable '=' expression    { $$ = AssignNode($2, $1, $3); }
           | mutable ADDASS expression { $$ = AssignNode($2, $1, $3); }
           | mutable SUBASS expression { $$ = AssignNode($2, $1, $3); }
           | mutable MULASS expression { $$ = AssignNode($2, $1, $3); }
           | mutable DIVASS expression { $$ = AssignNode($2, $1, $3); }
           | mutable INC               { $$ = AssignNode($2, $1, Leaf()); }
           | mutable DEC               { $$ = AssignNode($2, $1, Leaf()); }
           | simpleExpression
           ;

simpleExpression : simpleExpression OR andExpression { $$ = OpNode($2, $1, $3); }
                 | andExpression
                 ;

andExpression : andExpression AND unaryRelExpression { $$ = OpNode($2, $1, $3); }
              | unaryRelExpression
              ;

unaryRelExpression : NOT unaryRelExpression { $$ = OpNode($1, $2, Leaf()); }
                   | relExpression
                   ;

relExpression : sumExpression relop sumExpression { $$ = OpNode($2, $1, $3); }
              | sumExpression
              ;

relop : LESSEQ
      | '<'
      | '>'
      | GRTEQ
      | EQ
      | NOTEQ
      ;

sumExpression : sumExpression sumop term { $$ = OpNode($2, $1, $3); }
              | term
              ;

sumop :'+'
      |'-'
      ;

term : term mulop unaryExpression { $$ = OpNode($2, $1, $3); }
     | unaryExpression
     ;

mulop :'*'
      |'/'
      |'%'
      ;

unaryExpression : unaryop unaryExpression { $$ = OpNode($1, $2, Leaf()); }
                | factor
                ;

unaryop : '-'
        | '*'
        | '?'
        ;

factor : immutable
       | mutable
       ;

mutable : ID                         { $$ = IdNode($1); }
        | mutable '[' expression ']' { $$ = OpNode($2, $1, $3); }
        | mutable '.' ID             { $$ = OpNode($2, $1, IdNode($3)); }
        ;

immutable : '(' expression ')' { $$ = $2; }
          | call
          | constant { $$ = ConstNode($1); }
          ;

call : ID '(' args ')' { $$ = CallNode($1, $3); }
     ;

args : argList     { $$ = Siblings(*($1)); delete $1;}
     | /* empty */ { $$ = Leaf(); }
     ;

argList : argList ',' expression { $$ = $1->add($3); }
        | expression             { $$ = (new listof<Node*>())->add($1); }
        ;

constant : NUMCONST
         | CHARCONST
         | BOOLCONST
         ;
