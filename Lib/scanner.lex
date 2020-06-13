%{
  #include "parser.hpp"
  #include "parser.tab.hpp"
  #include <string.h>
  #include <stdarg.h>
  #define _(TOKEN) { return TOKEN; }
  #define _ERROR(NUM) { output::errorLex(NUM); exit(0); }
  #define ASSIGN_YYLVAL { yylval.STRING = new string(yytext); }
%}
%option noyywrap
%option yylineno
CR      (\x0D)
LF      (\x0A)
SPACE   (\x20)
TAB     (\x09)
%%
void                           _(VOID);
int                            _(INT);
byte                           _(BYTE);
b                              _(B);
bool                           _(BOOL);
and                            _(AND);
or                             _(OR);
not                            _(NOT);
true                           _(TRUE);
false                          _(FALSE);
return                         _(RETURN);
if                             _(IF);
else                           _(ELSE);
while                          _(WHILE);
break                          _(BREAK);
continue                       _(CONTINUE);
;                              _(SC);
,                              _(COMMA);
\(                             _(LPAREN);
\)                             _(RPAREN);
\{                             _(LBRACE);
\}                             _(RBRACE);
=                              _(ASSIGN);
>=|<=|<|>                      {
                                  ASSIGN_YYLVAL;
                                  _(RELOP);
                               }
==|!=                          {
                                  ASSIGN_YYLVAL;
                                  _(EQ_RELOP);
                               }
\+|-                           {
                                  ASSIGN_YYLVAL;
                                  _(PLUS_MINUS);
                               }
\*|\/                          {
                                  ASSIGN_YYLVAL;
                                  _(MUL_DIV);
                               }
[a-zA-Z][a-zA-Z0-9]*           {
                                  ASSIGN_YYLVAL;
                                  _(ID);
                               }
0|[1-9][0-9]*                  {
                                  yylval.TYPE = TYPE_INT;
                                  yylval.INT = atoi(yytext);
                                  _(NUM);
                               }
\"([^\n\r\"\\]|\\[rnt"\\])+\"  {
                                  yylval.TYPE = TYPE_STRING;
                                  ASSIGN_YYLVAL;
                                  yylval.INT = 1;
                                  _(STRING);
                               }
{CR}|{LF}|{TAB}|{SPACE}        /* ignore */;
"//"[^\r\n]*[\r|\n|\r\n]?      /* ignore */;
.                              _ERROR(yylineno);
%%
