%{
#include "Token.hpp"
#include "Error.hpp"
#include "ParseDefines.hpp"
#include "ParseDriver.hpp"
using namespace ParseDriver;
/**
 * Logs the token with ParseDriver and sets necessary fields for bison
 * Pass in code for making a unique Token*. It will only by evaluated once.
 */
#define setlval(arg) \
    do { yylval.token=arg; return pushToken(yylval.token); } while (false);

%}

%option noyywrap
%option yylineno

%%

"return"         { setlval(new Token(RETURN, yylineno, yytext)); }
"static"         { setlval(new Token(STATIC, yylineno, yytext)); }
"int"            { setlval(new Token(INT, yylineno, yytext)); }
"bool"           { setlval(new Token(BOOL, yylineno, yytext)); }
"char"           { setlval(new Token(CHAR, yylineno, yytext)); }
"if"             { setlval(new Token(IF, yylineno, yytext)); }
"else"           { setlval(new Token(ELSE, yylineno, yytext)); }
"while"          { setlval(new Token(WHILE, yylineno, yytext)); }
"break"          { setlval(new Token(BREAK, yylineno, yytext)); }
"record"         { setlval(new Token(RECORD, yylineno, yytext)); }
"!="             { setlval(new Token(NOTEQ, yylineno, yytext)); }
"*="             { setlval(new Token(MULASS, yylineno, yytext)); }
"++"             { setlval(new Token(INC, yylineno, yytext)); }
"+="             { setlval(new Token(ADDASS, yylineno, yytext)); }
"--"             { setlval(new Token(DEC, yylineno, yytext)); }
"-="             { setlval(new Token(SUBASS, yylineno, yytext)); }
"/="             { setlval(new Token(DIVASS, yylineno, yytext)); }
"<="             { setlval(new Token(LESSEQ, yylineno, yytext)); }
"=="             { setlval(new Token(EQ, yylineno, yytext)); }
">="             { setlval(new Token(GRTEQ, yylineno, yytext)); }
"not"            { setlval(new Token(NOT, yylineno, yytext)); }
"and"            { setlval(new Token(AND, yylineno, yytext)); }
"or"             { setlval(new Token(OR, yylineno, yytext)); }
\`[^\`]*\`       { setlval(new Token(ASMLINE, yylineno, yytext)); }
("true"|"false") { setlval(new BoolConst(BOOLCONST, yylineno, yytext)); }
[\(\)\[\]\{\}\;\=\<\>\+\-\*\/\%\?\.\,\:] {
                   setlval(new Token(yytext[0], yylineno, yytext)); }
\'(.|\\.)\'      { setlval(new CharConst(CHARCONST, yylineno, yytext)); }
[0-9]+           { setlval(new NumConst(NUMCONST, yylineno, yytext)); }
([a-zA-Z][a-zA-Z0-9]*|\*dummy\*) {
                   yylval.idToken = new IdToken(
                       isRecord(yytext)? RECTYPE : ID,
                       yylineno,
                       yytext);
                   return pushToken(yylval.idToken);
                 }
([\n\t ]*)|([\/]{2}.*) {
                   /* ignored whitespace characters */; }
.                { Token* misplaced = new Invalid(MISPLACED, yylineno, yytext);
                   pushToken(misplaced);
                   pushError(Errors::invalidInputCharacter(misplaced));
                 }

%%
