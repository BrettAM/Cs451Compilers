%{
#include "Token.hpp"
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

"return"         { setlval(new Token(RETURN, yylineno, tIdx(), yytext)); }
"static"         { setlval(new Token(STATIC, yylineno, tIdx(), yytext)); }
"int"            { setlval(new Token(INT, yylineno, tIdx(), yytext)); }
"bool"           { setlval(new Token(BOOL, yylineno, tIdx(), yytext)); }
"char"           { setlval(new Token(CHAR, yylineno, tIdx(), yytext)); }
"if"             { setlval(new Token(IF, yylineno, tIdx(), yytext)); }
"else"           { setlval(new Token(ELSE, yylineno, tIdx(), yytext)); }
"while"          { setlval(new Token(WHILE, yylineno, tIdx(), yytext)); }
"break"          { setlval(new Token(BREAK, yylineno, tIdx(), yytext)); }
"record"         { setlval(new Token(RECORD, yylineno, tIdx(), yytext)); }
"!="             { setlval(new Token(NOTEQ, yylineno, tIdx(), yytext)); }
"*="             { setlval(new Token(MULASS, yylineno, tIdx(), yytext)); }
"++"             { setlval(new Token(INC, yylineno, tIdx(), yytext)); }
"+="             { setlval(new Token(ADDASS, yylineno, tIdx(), yytext)); }
"--"             { setlval(new Token(DEC, yylineno, tIdx(), yytext)); }
"-="             { setlval(new Token(SUBASS, yylineno, tIdx(), yytext)); }
"/="             { setlval(new Token(DIVASS, yylineno, tIdx(), yytext)); }
"<="             { setlval(new Token(LESSEQ, yylineno, tIdx(), yytext)); }
"=="             { setlval(new Token(EQ, yylineno, tIdx(), yytext)); }
">="             { setlval(new Token(GRTEQ, yylineno, tIdx(), yytext)); }
"not"            { setlval(new Token(NOT, yylineno, tIdx(), yytext)); }
"and"            { setlval(new Token(AND, yylineno, tIdx(), yytext)); }
"or"             { setlval(new Token(OR, yylineno, tIdx(), yytext)); }
("true"|"false") { setlval(new BoolConst(BOOLCONST, yylineno, tIdx(), yytext)); }
[\(\)\[\]\{\}\;\=\<\>\+\-\*\/\%\?\.\,\:] {
                   setlval(new Token(yytext[0], yylineno, tIdx(), yytext)); }
\'(.|\\.)\'      { setlval(new CharConst(CHARCONST, yylineno, tIdx(), yytext)); }
[0-9]+           { setlval(new NumConst(NUMCONST, yylineno, tIdx(), yytext)); }
[a-zA-Z][a-zA-Z0-9]* {
                   yylval.idToken = new IdToken(
                       isRecord(yytext)? RECTYPE : ID,
                       yylineno,
                       tIdx(),
                       yytext);
                   return pushToken(yylval.idToken);
                 }
([\n\t ]*)|([\/]{2}.*) {
                   /* ignored whitespace characters */; }
.                { pushToken(new Invalid(MISPLACED, yylineno, tIdx(), yytext)); }

%%
