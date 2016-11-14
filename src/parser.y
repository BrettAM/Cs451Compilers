%{
#include "Token.hpp"
#include <vector>
#include "ParseDriver.hpp"
#include "AST.hpp"
#include "Type.hpp"
#include "IdComp.hpp"
#include <iostream>

using namespace ParseDriver;
using namespace AST;
using namespace std;
extern int yylex();
void yyerror(const char *msg){ parseError(msg); }
void errTr(const char *msg){ /*cerr << msg << endl;*/ }
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
%type <token> assignOp
%%

program : declarationList { rootAST(Siblings(*($1))); delete $1;} ;

declarationList : declarationList declaration { $$ = $1->addAll($2); delete $2; }
                | declaration { $$ = $1; }
                ;

declaration : varDeclaration
            | funDeclaration { $$ = (new listof<Node*>)->add($1); }
            | recDeclaration { $$ = (new listof<Node*>)->add($1); }
            | error          { errTr("DECLE"); $$ = (new listof<Node*>); }
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
               | error varDeclList ';' { errTr("VARDECLE1"); $$ = new listof<Node*>(); }
               | typeSpecifier error ';' { errTr("VARDECLE2"); yyerrok; $$ = new listof<Node*>(); }
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
                     | error varDeclList ';' { errTr("SVARDECLE1"); yyerrok; $$ = new listof<Node*>(); }
                     | scopedTypeSpecifier error ';' { errTr("SVARDECLE2"); yyerrok; $$ = new listof<Node*>(); }
                     ;

varDeclList : varDeclList ',' varDeclInitialize { yyerrok; $$ = $1->add($3); }
            | varDeclInitialize                 { $$ = (new listof<IdComp>())->add($1); }
            | varDeclList ',' error             { errTr("VARDECLLE1"); $$ = (new listof<IdComp>()); }
            | error                             { errTr("VARDECLLE2"); $$ = (new listof<IdComp>()); }
            ;

varDeclInitialize : varDeclId
                  | varDeclId ':' simpleExpression { $$ = $1; $$.init = $3; }
                  | varDeclId ':' error            { errTr("VARDECLIE1"); $$ = $1; }
                  | error     ':' simpleExpression { errTr("VARDECLIE2"); yyerrok; }
                  ;

varDeclId : ID                  { IdComp x = {$1, -1, NULL}; $$ = x; }
          | ID '[' NUMCONST ']' { IdComp x = {$1, ((NumConst*)$3)->value, NULL}; $$ = x; }
          | ID '[' error        { errTr("VARDECLIDE1"); IdComp x = {$1, -1, NULL}; $$ = x; }
          | error ']'           { errTr("VARDECLIDE2"); yyerrok; }
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
               | typeSpecifier error                       { errTr("IFE1"); }
               | typeSpecifier ID '(' error                { errTr("IFE2"); }
               | typeSpecifier ID '(' params ')' error     { errTr("IFE3"); }
               | ID '(' error                              { errTr("IFE4"); }
               | ID '(' params ')' error                   { errTr("IFE5"); }
               ;

params : paramList   { $$ = Siblings(*($1)); delete $1; }
       | /* empty */ { $$ = Leaf(); }
       ;

paramList : paramList ';' paramTypeList { yyerrok; $$ = $1->addAll($3); delete $3; }
          | paramTypeList
          | paramTypeList ';' error { errTr("paramList1"); }
          | error { errTr("paramList2"); }
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
              | typeSpecifier error { errTr("paramTypeList1"); }
              ;

paramIdList : paramIdList ',' paramId { yyerrok; $$ = $1->add($3); }
            | paramId                 { $$ = (new listof<IdComp>())->add($1); }
            | paramIdList ',' error   { errTr("paramIDList1"); }
            | error                   { errTr("paramIDList2"); }
            ;

paramId : ID         { IdComp x = {$1, -1, NULL}; $$ = x; }
        | ID '[' ']' { IdComp x = {$1,  0, NULL}; $$ = x; }
        | error  ']' { errTr("paramID"); yyerrok; }
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
            | IF '(' error { errTr("Matched1");}
            | IF error ')' matchedStmt ELSE matchedStmt { errTr("Matched2");yyerrok; }
            | WHILE error ')' matchedStmt {errTr("Matched3"); yyerrok; }
            | WHILE '(' error ')' matchedStmt {errTr("Matched4");yyerrok; }
            | WHILE error {errTr("Matched5"); }
            | error {errTr("Matched6"); }
            ;

unmatchedStmt : IF '(' simpleExpression ')' matchedStmt ELSE unmatchedStmt
                { $$ = IfNode($1, $3, $5, $7); }
              | IF '(' simpleExpression ')' matchedStmt
                { $$ = IfNode($1, $3, $5, Leaf()); }
              | IF '(' simpleExpression ')' unmatchedStmt
                { $$ = IfNode($1, $3, $5, Leaf()); }
              | WHILE '(' simpleExpression ')' unmatchedStmt
                { $$ = WhileNode($1, $3, $5); }
              | IF error { errTr("unmatched1"); }
              | IF error ')' matchedStmt                    { errTr("unmatched2"); yyerrok; }
              | IF error ')' matchedStmt ELSE unmatchedStmt { errTr("unmatched3"); yyerrok; }
              | WHILE error ')' unmatchedStmt               { errTr("unmatched4"); yyerrok; }
              | WHILE '(' error ')' unmatchedStmt           { errTr("unmatched5"); yyerrok; }
              ;

compoundStmt : '{' localDeclarations statementList '}'
               { yyerrok;
                 $$ = Compound($1, Siblings(*($2)), Siblings(*($3)));
                 delete $2;
                 delete $3;
               }
             | '{' error statementList '}'             { errTr("compound1"); yyerrok; }
             | '{' localDeclarations error '}'         { errTr("compound2"); yyerrok; }
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

expression : mutable assignOp expression { $$ = AssignNode($2, $1, $3); }
           | mutable INC               { yyerrok; $$ = AssignNode($2, $1, Leaf()); }
           | mutable DEC               { yyerrok; $$ = AssignNode($2, $1, Leaf()); }
           | simpleExpression
           | error INC                 { errTr("expression1"); yyerrok; }
           | error DEC                 { errTr("expression2"); yyerrok; }
           | error assignOp error      { errTr("expression3"); }
           ;

assignOp : '='| ADDASS | SUBASS | MULASS | DIVASS ;

simpleExpression : simpleExpression OR andExpression { $$ = OpNode($2, $1, $3); }
                 | andExpression
                 | simpleExpression OR error { errTr("simpleExpression"); }
                 ;

andExpression : andExpression AND unaryRelExpression { $$ = OpNode($2, $1, $3); }
              | unaryRelExpression
              | andExpression AND error { errTr("andExpression"); }
              ;

unaryRelExpression : NOT unaryRelExpression { $$ = OpNode($1, $2, Leaf()); }
                   | relExpression
                   | NOT error { errTr("unaryRel"); }
                   ;

relExpression : sumExpression relop sumExpression { $$ = OpNode($2, $1, $3); }
              | sumExpression
              | sumExpression relop error { errTr("rel1"); }
              | error relop sumExpression { errTr("rel2"); yyerrok; }
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
              | sumExpression sumop error { yyerrok; errTr("sum1"); }
              ;

sumop :'+'
      |'-'
      ;

term : term mulop unaryExpression { $$ = OpNode($2, $1, $3); }
     | unaryExpression
     | term mulop error { errTr("term1"); }
     ;

mulop :'*'
      |'/'
      |'%'
      ;

unaryExpression : unaryop unaryExpression { $$ = OpNode($1, $2, Leaf()); }
                | factor
                | unaryop error { errTr("unary1");}
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
          | '(' error { errTr("immutable1"); }
          | error ')' { yyerrok; errTr("immutable2"); }
          ;

call : ID '(' args ')' { $$ = CallNode($1, $3); }
     | error '(' { errTr("call1"); yyerrok; }
     ;

args : argList     { $$ = Siblings(*($1)); delete $1;}
     | /* empty */ { $$ = Leaf(); }
     ;

argList : argList ',' expression { yyerrok; $$ = $1->add($3); }
        | expression             { $$ = (new listof<Node*>())->add($1); }
        | argList ',' error      { errTr("arglist1"); }
        ;

constant : NUMCONST
         | CHARCONST
         | BOOLCONST
         ;
