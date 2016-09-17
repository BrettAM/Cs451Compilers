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

"return"         { setlval(new Token(RETURN, yylineno, "RETURN")); }
"static"         { setlval(new Token(STATIC, yylineno, "STATIC")); }
"int"            { setlval(new Token(INT, yylineno, "INT")); }
"bool"           { setlval(new Token(BOOL, yylineno, "BOOL")); }
"char"           { setlval(new Token(CHAR, yylineno, "CHAR")); }
"if"             { setlval(new Token(IF, yylineno, "IF")); }
"else"           { setlval(new Token(ELSE, yylineno, "ELSE")); }
"while"          { setlval(new Token(WHILE, yylineno, "WHILE")); }
"break"          { setlval(new Token(BREAK, yylineno, "BREAK")); }
"record"         { setlval(new Token(RECORD, yylineno, "RECORD")); }
"!="             { setlval(new Token(NOTEQ, yylineno, "NOTEQ")); }
"*="             { setlval(new Token(MULASS, yylineno, "MULASS")); }
"++"             { setlval(new Token(INC, yylineno, "INC")); }
"+="             { setlval(new Token(ADDASS, yylineno, "ADDASS")); }
"--"             { setlval(new Token(DEC, yylineno, "DEC")); }
"-="             { setlval(new Token(SUBASS, yylineno, "SUBASS")); }
"/="             { setlval(new Token(DIVASS, yylineno, "DIVASS")); }
"<="             { setlval(new Token(LESSEQ, yylineno, "LESSEQ")); }
"=="             { setlval(new Token(EQ, yylineno, "EQ")); }
">="             { setlval(new Token(GRTEQ, yylineno, "GRTEQ")); }
"not"            { setlval(new Token(NOT, yylineno, "NOT")); }
"and"            { setlval(new Token(AND, yylineno, "AND")); }
"or"             { setlval(new Token(OR, yylineno, "OR")); }
("true"|"false") { setlval(new BoolConst(BOOLCONST, yylineno, yytext)); }
[\(\)\[\]\{\}\;\=\<\>\+\-\*\/\%\?\.\,\:] {
                   setlval(new Token(yytext[0], yylineno, yytext)); }
\'(.|\\.)\'      { setlval(new CharConst(CHARCONST, yylineno, yytext)); }
[0-9]+           { setlval(new NumConst(NUMCONST, yylineno, yytext)); }
[a-zA-Z][a-zA-Z0-9]* {
                   setlval(new IdToken(ID, yylineno, yytext)); }
([\n\t ]*)|([\/]{2}.*) {
                   /* igroned whitespace characters */; }
.                { pushToken(new Invalid(MISPLACED, yylineno, yytext)); }

%%
