%{
#include "Token.hpp"
#include "parser.hpp" // must be included last
%}

%option noyywrap
%option yylineno

%%

"return"         { yylval.token=new Token(RETURN, yylineno, "RETURN"); return RETURN; }
"static"         { yylval.token=new Token(STATIC, yylineno, "STATIC"); return STATIC; }
"int"            { yylval.token=new Token(INT, yylineno, "INT"); return INT; }
"bool"           { yylval.token=new Token(BOOL, yylineno, "BOOL"); return BOOL; }
"char"           { yylval.token=new Token(CHAR, yylineno, "CHAR"); return CHAR; }
"if"             { yylval.token=new Token(IF, yylineno, "IF"); return IF; }
"else"           { yylval.token=new Token(ELSE, yylineno, "ELSE"); return ELSE; }
"while"          { yylval.token=new Token(WHILE, yylineno, "WHILE"); return WHILE; }
"break"          { yylval.token=new Token(BREAK, yylineno, "BREAK"); return BREAK; }
"record"         { yylval.token=new Token(RECORD, yylineno, "RECORD"); return RECORD; }
"!="             { yylval.token=new Token(NOTEQ, yylineno, "NOTEQ"); return NOTEQ; }
"*="             { yylval.token=new Token(MULASS, yylineno, "MULASS"); return MULASS; }
"++"             { yylval.token=new Token(INC, yylineno, "INC"); return INC; }
"+="             { yylval.token=new Token(ADDASS, yylineno, "ADDASS"); return ADDASS; }
"--"             { yylval.token=new Token(DEC, yylineno, "DEC"); return DEC; }
"-="             { yylval.token=new Token(SUBASS, yylineno, "SUBASS"); return SUBASS; }
"/="             { yylval.token=new Token(DIVASS, yylineno, "DIVASS"); return DIVASS; }
"<="             { yylval.token=new Token(LESSEQ, yylineno, "LESSEQ"); return LESSEQ; }
"=="             { yylval.token=new Token(EQ, yylineno, "EQ"); return EQ; }
">="             { yylval.token=new Token(GRTEQ, yylineno, "GRTEQ"); return GRTEQ; }
"not"            { yylval.token=new Token(NOT, yylineno, "NOT"); return NOT; }
"and"            { yylval.token=new Token(AND, yylineno, "AND"); return AND; }
"or"             { yylval.token=new Token(OR, yylineno, "OR"); return OR; }
("true"|"false") { yylval.token=new BoolConst(BOOLCONST, yylineno, yytext); return BOOLCONST; }
[\(\)\[\]\{\}\;\=\<\>\+\-\*\/\%\?\.\,\:] {
                   yylval.token=new Token(yytext[0], yylineno, yytext); return yytext[0]; }
\'(.|\\.)\'      { yylval.token=new CharConst(CHARCONST, yylineno, yytext); return CHARCONST; }
[0-9]+           { yylval.token=new NumConst(NUMCONST, yylineno, yytext); return NUMCONST; }
[a-zA-Z][a-zA-Z0-9]* {
                   yylval.token=new IdToken(ID, yylineno, yytext); return ID; }
([\n\t ]*)|([\/]{2}.*) {
                   /* igroned whitespace characters */; }
.                { yylval.token=new Invalid(MISPLACED, yylineno, yytext); return MISPLACED; }

%%
