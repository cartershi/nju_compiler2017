%{
#include "common.h"
#include "tree.h"
int yylex();
void yyerror(char* msg);
extern int yylineno;
extern char* yytext;
int errorrec=0;
treenode* treeroot;
%}

%union{
    treenode* node;
}

%token <node>INT
%token <node>FLOAT
%token <node>ID WRITE READ
%token <node>SEMI COMMA ASSIGNOP RELOP
%token <node>PLUS MINUS STAR DIV
%token <node>AND OR DOT NOT TYPE
%token <node>LP RP LB RB LC RC
%token <node>STRUCT RETURN IF ELSE WHILE
%type <node> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier
OptTag Tag VarDec FunDec VarList ParamDec CompSt StmtList Stmt DefList Def
DecList Dec Exp Args

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left LP RP LB RB DOT


%%
/*High-level Definitions*/
Program:ExtDefList{
           $$=addnode("Program",1,$1);
           traversal($$,0);
           treeroot=$$;
      }
        ;
ExtDefList:{$$=addnode("ExtDefList",0);}
          |ExtDef ExtDefList{$$=addnode("ExtDefList",2,$1,$2);}
          ;
ExtDef:Specifier ExtDecList SEMI {$$=addnode("ExtDef",3,$1,$2,$3);}
      |Specifier SEMI {$$=addnode("ExtDef",2,$1,$2);}
      |Specifier FunDec CompSt {$$=addnode("ExtDef",3,$1,$2,$3);}
      |Specifier ExtDecList error SEMI {yyerror("the statement discarded");}
      |Specifier error SEMI {yyerror("the statement discarded");}
      ;
ExtDecList:VarDec {$$=addnode("ExtDecList",1,$1);}
          |VarDec COMMA ExtDecList {$$=addnode("ExtDecList",3,$1,$2,$3);}
          ;

/*Specifiers*/
Specifier:TYPE {$$=addnode("Specifier",1,$1);}
         |StructSpecifier{$$=addnode("Specifier",1,$1);}
         ;
StructSpecifier:STRUCT OptTag LC DefList RC {
               $$=addnode("StructSpecifier",5,$1,$2,$3,$4,$5);
               }
               |STRUCT Tag {$$=addnode("StructSpecifier",2,$1,$2);}
               |STRUCT OptTag LC DefList error RC{
               yyerror("there is someting wrong in { }");
               }
               ;
OptTag:ID {$$=addnode("OptTag",1,$1);}
      |{$$=addnode("OptTag",0);}
      ;
Tag:ID {$$=addnode("Tag",1,$1);}
   ;

/*Declarators*/
VarDec:ID {$$=addnode("VarDec",1,$1);}
      |VarDec LB INT RB {$$=addnode("VarDec",4,$1,$2,$3,$4);}
      |VarDec LB INT error RB{yyerror("the number is not integer");};
FunDec:ID LP VarList RP {$$=addnode("FunDec",4,$1,$2,$3,$4);}
      |ID LP RP {$$=addnode("FunDec",3,$1,$2,$3);}
      |ID LP VarList error RP{yyerror("the varlist is discarded");};
VarList:ParamDec COMMA VarList {$$=addnode("VarList",3,$1,$2,$3);}
       |ParamDec {$$=addnode("VarList",1,$1);}
       ;
ParamDec:Specifier VarDec {$$=addnode("ParamDec",2,$1,$2);}
        ;

/*Statements*/
CompSt:LC DefList StmtList RC {$$=addnode("CompSt",4,$1,$2,$3,$4);}
      |LC DefList StmtList error RC{
      yyerror("there is something wrong in {}");
      };
StmtList:Stmt StmtList {$$=addnode("StmtList",2,$1,$2);}
        |{$$=addnode("StmtList",0);}
        ;
Stmt:Exp SEMI {$$=addnode("Stmt",2,$1,$2);}
    |WRITE Exp SEMI {$$=addnode("Stmt",3,$1,$2,$3);}
    |READ Exp SEMI {$$=addnode("Stmt",3,$1,$2,$3);}
    |CompSt {$$=addnode("Stmt",1,$1);}
    |RETURN Exp SEMI {$$=addnode("Stmt",3,$1,$2,$3);}
    |IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {$$=addnode("Stmt",5,$1,$2,$3,$4,$5);}
    |IF LP Exp RP Stmt ELSE Stmt {$$=addnode("stmt",7,$1,$2,$3,$4,$5,$6,$7);}
    |WHILE LP Exp RP Stmt {$$=addnode("Stmt",5,$1,$2,$3,$4,$5);}
    |Exp error SEMI {yyerror("the expression discarded;");}
    |RETURN Exp error SEMI {yyerror("the expression discarded");}
    |IF LP Exp error RP Stmt %prec LOWER_THAN_ELSE{ 
    yyerror("the expression is wrong");
    }
    |WHILE LP Exp error RP Stmt {
    yyerror("the expression is wrong");
    }
    ;

/*Local Definitions*/
DefList:Def DefList {$$=addnode("DefList",2,$1,$2);}
       |{$$=addnode("DefList",0);}
       ;
Def:Specifier DecList SEMI {$$=addnode("Def",3,$1,$2,$3);}
   |Specifier DecList error SEMI {yyerror("missing ;");}
   ;
DecList:Dec {$$=addnode("DecList",1,$1);}
       |Dec COMMA DecList {$$=addnode("DecList",3,$1,$2,$3);}
       ;
Dec:VarDec {$$=addnode("Dec",1,$1);}
   |VarDec ASSIGNOP Exp {$$=addnode("Dec",3,$1,$2,$3);}
   ;

/*Expressions*/
Exp:Exp ASSIGNOP Exp {$$=addnode("Exp",3,$1,$2,$3);}
   |Exp AND Exp {$$=addnode("Exp",3,$1,$2,$3);}
   |Exp OR Exp {$$=addnode("Exp",3,$1,$2,$3);}
   |Exp RELOP Exp {$$=addnode("Exp",3,$1,$2,$3);}
   |Exp PLUS Exp{$$=addnode("Exp",3,$1,$2,$3);}
   |Exp MINUS Exp {$$=addnode("Exp",3,$1,$2,$3);}
   |Exp STAR Exp {$$=addnode("Exp",3,$1,$2,$3);}
   |Exp DIV Exp {$$=addnode("Exp",3,$1,$2,$3);}
   |LP Exp RP {$$=addnode("Exp",3,$1,$2,$3);}
   |MINUS Exp {$$=addnode("Exp",2,$1,$2);}
   |NOT Exp {$$=addnode("Exp",2,$1,$2);}
   |ID LP Args RP {$$=addnode("Exp",4,$1,$2,$3,$4);}
   |ID LP RP {$$=addnode("Exp",3,$1,$2,$3);}
   |Exp LB Exp RB {$$=addnode("Exp",4,$1,$2,$3,$4);}
   |Exp DOT ID {$$=addnode("Exp",3,$1,$2,$3);}
   |ID {$$=addnode("Exp",1,$1);}
   |INT {$$=addnode("Exp",1,$1);}
   |FLOAT {$$=addnode("Exp",1,$1);}
   |LP Exp error RP{yyerror("the expression is wrong");}
   |ID LP Args error RP{yyerror("the expression is wrong");} 
   |ID LP error RP{yyerror("the expression is disscarded");}
   |Exp LB Exp error RB{yyerror("the expression is wrong");}
   ;
Args:Exp COMMA Args {$$=addnode("Args",3,$1,$2,$3);}
    |Exp {$$=addnode("Args",1,$1);}
    ;
%%
void yyerror(char* msg)
{
    printf("Error type B at line %d: %s\n",yylineno,msg);
    errorrec=1;
}
