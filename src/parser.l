%{
#include "Token.hpp"
#include "parser.hpp" // must be included last
%}

%option noyywrap
%option yylineno

%%

Token { yylval.token=new Token(TOKEN, yylineno, yytext); return TOKEN; }

%%
