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
            | error          { cerr<<"DECLE"<<endl; $$ = (new listof<Node*>); }
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
               | error varDeclList ';' { cerr<<"VARDECLE1"<<endl; $$ = new listof<Node*>(); }
               | typeSpecifier error ';' { cerr<<"VARDECLE2"<<endl; yyerrok; $$ = new listof<Node*>(); }
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
                     | error varDeclList ';' { cerr<<"SVARDECLE1"<<endl; yyerrok; $$ = new listof<Node*>(); }
                     | scopedTypeSpecifier error ';' { cerr<<"SVARDECLE2"<<endl; yyerrok; $$ = new listof<Node*>(); }
                     ;

varDeclList : varDeclList ',' varDeclInitialize { yyerrok; $$ = $1->add($3); }
            | varDeclInitialize                 { $$ = (new listof<IdComp>())->add($1); }
            | varDeclList ',' error             { cerr<<"VARDECLLE1"<<endl; $$ = (new listof<IdComp>()); }
            | error                             { cerr<<"VARDECLLE2"<<endl; $$ = (new listof<IdComp>()); }
            ;

varDeclInitialize : varDeclId
                  | varDeclId ':' simpleExpression { $$ = $1; $$.init = $3; }
                  | varDeclId ':' error            { cerr<<"VARDECLIE1"<<endl; $$ = $1; }
                  | error     ':' simpleExpression { cerr<<"VARDECLIE2"<<endl; yyerrok; }
                  ;

varDeclId : ID                  { IdComp x = {$1, -1, NULL}; $$ = x; }
          | ID '[' NUMCONST ']' { IdComp x = {$1, ((NumConst*)$3)->value, NULL}; $$ = x; }
          | ID '[' error        { cerr<<"VARDECLIDE1"<<endl; IdComp x = {$1, -1, NULL}; $$ = x; }
          | error ']'           { cerr<<"VARDECLIDE2"<<endl; yyerrok; }
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
               | typeSpecifier error                       { cerr<<"IFE1"<<endl; }
               | typeSpecifier ID '(' error                { cerr<<"IFE2"<<endl; }
               | typeSpecifier ID '(' params ')' error     { cerr<<"IFE3"<<endl; }
               | ID '(' error                              { cerr<<"IFE4"<<endl; }
               | ID '(' params ')' error                   { cerr<<"IFE5"<<endl; }
               ;

params : paramList   { $$ = Siblings(*($1)); delete $1; }
       | /* empty */ { $$ = Leaf(); }
       ;

paramList : paramList ';' paramTypeList { yyerrok; $$ = $1->addAll($3); delete $3; }
          | paramTypeList
          | paramTypeList ';' error { cerr<<"paramList1"<<endl; }
          | error { cerr<<"paramList2"<<endl; }
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
              | typeSpecifier error { cerr<<"paramTypeList1"<<endl; }
              ;

paramIdList : paramIdList ',' paramId { yyerrok; $$ = $1->add($3); }
            | paramId                 { $$ = (new listof<IdComp>())->add($1); }
            | paramIdList ',' error   { cerr<<"paramIDList1"<<endl; }
            | error                   { cerr<<"paramIDList2"<<endl; }
            ;

paramId : ID         { IdComp x = {$1, -1, NULL}; $$ = x; }
        | ID '[' ']' { IdComp x = {$1,  0, NULL}; $$ = x; }
        | error  ']' { cerr<<"paramID"<<endl; yyerrok; }
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
            | IF '(' error { cerr<<"Matched1"<<endl;}
            | IF error ')' matchedStmt ELSE matchedStmt { cerr<<"Matched2"<<endl;yyerrok; }
            | WHILE error ')' matchedStmt {cerr<<"Matched3"<<endl; yyerrok; }
            | WHILE '(' error ')' matchedStmt {cerr<<"Matched4"<<endl;yyerrok; }
            | WHILE error {cerr<<"Matched5"<<endl; }
            | error {cerr<<"Matched6"<<endl; }
            ;

unmatchedStmt : IF '(' simpleExpression ')' matchedStmt ELSE unmatchedStmt
                { $$ = IfNode($1, $3, $5, $7); }
              | IF '(' simpleExpression ')' matchedStmt
                { $$ = IfNode($1, $3, $5, Leaf()); }
              | IF '(' simpleExpression ')' unmatchedStmt
                { $$ = IfNode($1, $3, $5, Leaf()); }
              | WHILE '(' simpleExpression ')' unmatchedStmt
                { $$ = WhileNode($1, $3, $5); }
              | IF error { cerr<<"unmatched1"<<endl; }
              | IF error ')' matchedStmt                    { cerr<<"unmatched2"<<endl; yyerrok; }
              | IF error ')' matchedStmt ELSE unmatchedStmt { cerr<<"unmatched3"<<endl; yyerrok; }
              | WHILE error ')' unmatchedStmt               { cerr<<"unmatched4"<<endl; yyerrok; }
              | WHILE '(' error ')' unmatchedStmt           { cerr<<"unmatched5"<<endl; yyerrok; }
              ;

compoundStmt : '{' localDeclarations statementList '}'
               { yyerrok;
                 $$ = Compound($1, Siblings(*($2)), Siblings(*($3)));
                 delete $2;
                 delete $3;
               }
             | '{' error statementList '}'             { cerr<<"compound1"<<endl; yyerrok; }
             | '{' localDeclarations error '}'         { cerr<<"compound2"<<endl; yyerrok; }
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
           | error INC                 { cerr<<"expression1"<<endl; yyerrok; }
           | error DEC                 { cerr<<"expression2"<<endl; yyerrok; }
           ;

simpleExpression : simpleExpression OR andExpression { $$ = OpNode($2, $1, $3); }
                 | andExpression
                 | simpleExpression OR error { cerr<<"simpleExpression"<<endl; }
                 ;

andExpression : andExpression AND unaryRelExpression { $$ = OpNode($2, $1, $3); }
              | unaryRelExpression
              | andExpression AND error { cerr<<"andExpression"<<endl; }
              ;

unaryRelExpression : NOT unaryRelExpression { $$ = OpNode($1, $2, Leaf()); }
                   | relExpression
                   | NOT error { cerr<<"unaryRel"<<endl; }
                   ;

relExpression : sumExpression relop sumExpression { $$ = OpNode($2, $1, $3); }
              | sumExpression
              | sumExpression relop error { cerr<<"rel1"<<endl; }
              | error relop sumExpression { cerr<<"rel2"<<endl; yyerrok; }
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
              | sumExpression sumop error { yyerrok; cerr<<"sum1"<<endl; }
              ;

sumop :'+'
      |'-'
      ;

term : term mulop unaryExpression { $$ = OpNode($2, $1, $3); }
     | unaryExpression
     | term mulop error { cerr<<"term1"<<endl; }
     ;

mulop :'*'
      |'/'
      |'%'
      ;

unaryExpression : unaryop unaryExpression { $$ = OpNode($1, $2, Leaf()); }
                | factor
                | unaryop error { cerr<<"unary1"<<endl;}
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
          | '(' error { cerr<<"immutable1"<<endl; }
          | error ')' { cerr<<"immutable2"<<endl; }
          ;

call : ID '(' args ')' { $$ = CallNode($1, $3); }
     | error '(' { cerr<<"call1"<<endl; yyerrok; }
     ;

args : argList     { $$ = Siblings(*($1)); delete $1;}
     | /* empty */ { $$ = Leaf(); }
     ;

argList : argList ',' expression { yyerrok; $$ = $1->add($3); }
        | expression             { $$ = (new listof<Node*>())->add($1); }
        | argList ',' error      { cerr<<"arglist1"<<endl; }
        ;

constant : NUMCONST
         | CHARCONST
         | BOOLCONST
         ;
