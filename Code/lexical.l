%{
#include "tree.h"
#include "syntax.tab.h"

int yycolumn=1;
YYLTYPE yylloc;
extern int errorrec;
#define YY_USER_ACTION {\
    yylloc.first_line=yylloc.last_line=yylineno;\
    yylloc.first_column=yycolumn;\
    yylloc.last_column=yycolumn+yyleng-1;\
    yycolumn+=yyleng;\
    }
%}
%option yylineno
digits [0-9]+
DEC [1-9][0-9]{0,31}
OCT 0[0-7]{0,15}
HEX 0[xX][0-9A-Fa-f]{0,8}
FLOAT (({digits}\.{digits}?)|({digits}?\.{digits}))([eE][+-]?{digits})?
ID [_a-zA-Z][_a-zA-Z0-9]*
linezhu \/\/.*$
blockzhu \/\*([^\*]|(\*[^\/]))*\*\/
%%
{DEC}   {addleaf("DEC"); return INT;}
{OCT}   {addleaf("OCT"); return INT;}
{HEX}   {addleaf("HEX"); return INT;}
{FLOAT} {addleaf("FLOAT"); return FLOAT;}
";"     {addleaf("SEMI"); return SEMI;}
","     {addleaf("COMMA"); return COMMA;}
"="     {addleaf("ASSIGNOP"); return ASSIGNOP;}
>|<|>=|<=|==|!= {addleaf("RELOP"); return RELOP;}
"+"     {addleaf("PLUS"); return PLUS;}
"-"     {addleaf("MINUS"); return MINUS;}
"*"     {addleaf("STAR"); return STAR;}
"/"     {addleaf("DIV"); return DIV;}
"&&"    {addleaf("AND"); return AND;}
"||"    {addleaf("OR"); return OR;}
"."     {addleaf("DOT"); return DOT;}
"!"     {addleaf("NOT"); return NOT;}
int|float {addleaf("TYPE"); return TYPE;}
"("     {addleaf("LP"); return LP;}
")"     {addleaf("RP"); return RP;}
"["     {addleaf("LB"); return LB;}
"]"     {addleaf("RB"); return RB;}
"{"     {addleaf("LC"); return LC;}
"}"     {addleaf("RC"); return RC;}
[ \t]+  {}
{linezhu} {yycolumn=1;}
{blockzhu} {}
\n      {yycolumn=1;}
struct  {addleaf("STRUCT"); return STRUCT;}
return  {addleaf("RETURN"); return RETURN;}
if      {addleaf("IF"); return IF;}
else    {addleaf("ELSE"); return ELSE;}
while   {addleaf("WHILE"); return WHILE;}
read    {addleaf("ID"); return ID;}
write   {addleaf("ID"); return ID;}
{ID}    {addleaf("ID"); return ID;}
.       {
        errorrec=1;
        printf("Error type A at Line %d:Mysterious characters\'%s\'\n",
        yylineno,yytext);
        }
%%
