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
void yyerror(const char *msg){ parseError(msg); }
%}

%no-lines
%define parse.error verbose
%union {
  const IdToken* idToken;
  const Token* token;
  AST::Node* node;
  AST::listof<AST::Node*>* builder;
  Type* type;
  IdComp idComp;
  AST::listof<IdComp>* idCompList;
}

%token <idToken> ID RECTYPE
%token <token>
  BOOLCONST CHARCONST NUMCONST
  INT BOOL CHAR RECORD STATIC
  IF ELSE WHILE RETURN BREAK
  NOTEQ MULASS INC ADDASS DEC SUBASS DIVASS LESSEQ EQ GRTEQ NOT AND OR
  MISPLACED
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
            | error { }
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
          yyerrok;
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
                (id.arraylen == -1)? type : type.asArray(id.arraylen),
                id.init
              )
            );
          }
          $$ = nodelist;
        }
               | error varDeclList ';' {  }
               | typeSpecifier error ';' { yyerrok; }
               ;

scopedVarDeclaration : scopedTypeSpecifier varDeclList ';'
        { yyerrok;
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
                (id.arraylen == -1)? type : type.asArray(id.arraylen),
                id.init
              )
            );
          }
          $$ = nodelist;
        }
                     | error varDeclList ';' { yyerrok; }
                     | scopedTypeSpecifier error ';' { yyerrok; }
                     ;

varDeclList : varDeclList ',' varDeclInitialize { yyerrok; $$ = $1->add($3); }
            | varDeclInitialize                 { $$ = (new listof<IdComp>())->add($1); }
            | varDeclList ',' error             { }
            | error                             { }
            ;

varDeclInitialize : varDeclId
                  | varDeclId ':' simpleExpression { $$ = $1; $$.init = $3; }
                  | varDeclId ':' error            { $$ = $1; }
                  | error     ':' simpleExpression { yyerrok; }
                  ;

varDeclId : ID                  { IdComp x = {$1, -1, NULL}; $$ = x; }
          | ID '[' NUMCONST ']' { IdComp x = {$1, ((NumConst*)$3)->value, NULL}; $$ = x; }
          | ID '[' error        { }
          | error ']'           { yyerrok; }
          ;

scopedTypeSpecifier : STATIC typeSpecifier { $$ = new Type($2->asStatic()); delete $2; }
                    | typeSpecifier
                    ;

typeSpecifier : returnTypeSpecifier
              | RECTYPE             { $$ = new Type(Type::RECORD()); }
              ;

returnTypeSpecifier : INT  { $$ = new Type(Type::INT); }
                    | BOOL { $$ = new Type(Type::BOOL); }
                    | CHAR { $$ = new Type(Type::CHAR); }
                    ;

funDeclaration : typeSpecifier ID '(' params ')' statement { $$ = FuncDecl($2, (*$1).asFunc(), $4, $6); delete $1; }
               | ID '(' params ')' statement               { $$ = FuncDecl($1, Type::VOID.asFunc(), $3, $5); }
               | typeSpecifier error                       { }
               | typeSpecifier ID '(' error                { }
               | typeSpecifier ID '(' params ')' error     { }
               | ID '(' error                              { }
               | ID '(' params ')' error                   { }
               ;

params : paramList   { $$ = Siblings(*($1)); delete $1; }
       | /* empty */ { $$ = Leaf(); }
       ;

paramList : paramList ';' paramTypeList { yyerrok; $$ = $1->addAll($3); delete $3; }
          | paramTypeList ';' error { }
          | error { }
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
                        (id.arraylen == -1)? type : type.asArray(id.arraylen)
                      )
                    );
                  }
                }
              | typeSpecifier error {}
              ;

paramIdList : paramIdList ',' paramId { yyerrok; $$ = $1->add($3); }
            | paramIdList ',' error   { }
            | error                   { }
            ;

paramId : ID         { IdComp x = {$1, -1, NULL}; $$ = x; }
        | ID '[' ']' { IdComp x = {$1,  0, NULL}; $$ = x; }
        | error  ']' { yyerrok; }
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
            | IF '(' error { }
            | IF error ')' matchedStmt ELSE matchedStmt { yyerrok; }
            | WHILE error ')' matchedStmt { yyerrok; }
            | WHILE '(' error ')' matchedStmt { yyerrok; }
            | WHILE error { }
            | error { }
            ;

unmatchedStmt : IF '(' simpleExpression ')' matchedStmt ELSE unmatchedStmt
                { $$ = IfNode($1, $3, $5, $7); }
              | IF '(' simpleExpression ')' matchedStmt
                { $$ = IfNode($1, $3, $5, Leaf()); }
              | IF '(' simpleExpression ')' unmatchedStmt
                { $$ = IfNode($1, $3, $5, Leaf()); }
              | WHILE '(' simpleExpression ')' unmatchedStmt
                { $$ = WhileNode($1, $3, $5); }
              | IF error { }
              | IF error ')' matchedStmt                    { yyerrok; }
              | IF error ')' matchedStmt ELSE unmatchedStmt { yyerrok; }
              | WHILE error ')' unmatchedStmt               { yyerrok; }
              | WHILE '(' error ')' unmatchedStmt           { yyerrok; }
              ;

compoundStmt : '{' localDeclarations statementList '}'
               { yyerrok;
                 $$ = Compound($1, Siblings(*($2)), Siblings(*($3)));
                 delete $2;
                 delete $3;
               }
             | '{' error statementList '}'             { yyerrok; }
             | '{' localDeclarations error '}'         { yyerrok; }
             ;

localDeclarations : localDeclarations scopedVarDeclaration { $$ = $1->addAll($2); delete $2; }
                  | /* empty */                            { $$ = (new listof<Node*>()); }
                  ;

statementList : statementList statement { $$ = $1->add($2); }
              | /* empty */             { $$ = (new listof<Node*>()); }
              ;

expressionStmt : expression ';' { yyerrok; }
               | ';' { yyerrok; $$ = Leaf(); }
               ;

returnStmt : RETURN ';'            { yyerrok; $$ = ReturnNode($1, Leaf()); }
           | RETURN expression ';' { yyerrok; $$ = ReturnNode($1, $2); }
           ;

breakStmt : BREAK ';' { yyerrok; $$ = BreakNode($1); }
          ;

expression : mutable '=' expression    { $$ = AssignNode($2, $1, $3); }
           | mutable ADDASS expression { $$ = AssignNode($2, $1, $3); }
           | mutable SUBASS expression { $$ = AssignNode($2, $1, $3); }
           | mutable MULASS expression { $$ = AssignNode($2, $1, $3); }
           | mutable DIVASS expression { $$ = AssignNode($2, $1, $3); }
           | mutable INC               { yyerrok; $$ = AssignNode($2, $1, Leaf()); }
           | mutable DEC               { yyerrok; $$ = AssignNode($2, $1, Leaf()); }
           | simpleExpression
           | error INC                 { yyerrok; }
           | error DEC                 { yyerrok; }
           ;

simpleExpression : simpleExpression OR andExpression { $$ = OpNode($2, $1, $3); }
                 | andExpression
                 | simpleExpression OR error { }
                 ;

andExpression : andExpression AND unaryRelExpression { $$ = OpNode($2, $1, $3); }
              | unaryRelExpression
              | andExpression AND error { }
              ;

unaryRelExpression : NOT unaryRelExpression { $$ = OpNode($1, $2, Leaf()); }
                   | relExpression
                   | NOT error { }
                   ;

relExpression : sumExpression relop sumExpression { $$ = OpNode($2, $1, $3); }
              | sumExpression
              | sumExpression relop error { }
              | error relop sumExpression { yyerrok; }
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
              | sumExpression sumop error { yyerrok; }
              ;

sumop :'+'
      |'-'
      ;

term : term mulop unaryExpression { $$ = OpNode($2, $1, $3); }
     | unaryExpression
     | term mulop error { }
     ;

mulop :'*'
      |'/'
      |'%'
      ;

unaryExpression : unaryop unaryExpression { $$ = OpNode($1, $2, Leaf()); }
                | factor
                | unaryop error { }
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

immutable : '(' expression ')' { yyerrok; $$ = $2; }
          | call
          | constant { $$ = ConstNode($1); }
          | '(' error { }
          | error ')' { }
          ;

call : ID '(' args ')' { $$ = CallNode($1, $3); }
     | error '(' { yyerrok; }
     ;

args : argList     { $$ = Siblings(*($1)); delete $1;}
     | /* empty */ { $$ = Leaf(); }
     ;

argList : argList ',' expression { yyerrok; $$ = $1->add($3); }
        | expression             { $$ = (new listof<Node*>())->add($1); }
        | argList ',' error      { }
        ;

constant : NUMCONST
         | CHARCONST
         | BOOLCONST
         ;
