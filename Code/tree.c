#include "common.h"
#include <stdarg.h>
#include "syntax.tab.h"

extern char* yytext;
extern int yylineno;
void addleaf(char *name){
    treenode *newleaf=(treenode*)malloc(sizeof(treenode));
    //printf("%s\n",name);
    newleaf->name=name;
    newleaf->line=yylineno;
    newleaf->child=NULL;
    newleaf->sibling=NULL;
    yylval.node=newleaf;
    if (strcmp(name,"DEC")==0){
        newleaf->type_int=atoi(yytext);
        return;
    }
    if (strcmp(name,"OCT")==0){
        int i,octnum;
        i=1;
        octnum=0;
        while (yytext[i]!='\0'){
            octnum=octnum*8+yytext[i]-'0';
            i++;
        }
        newleaf->type_int=octnum;
        newleaf->name="INT";
        return;
    }
    if (strcmp(name,"HEX")==0){
        int i,hexnum,signum;
        i=2;
        hexnum=0;
        while (yytext[i]!='\0'){
            if (yytext[i]>='0'&&yytext[i]<='9') signum=yytext[i]-'0';
            if (yytext[i]>='a'&&yytext[i]<='f') signum=yytext[i]-'a'+10;
            if (yytext[i]>='A'&&yytext[i]<='F') signum=yytext[i]-'A'+10;
            //printf("%d\n",signum);
            hexnum=hexnum*16+signum;
            i++;
        }
        newleaf->type_int=hexnum;
        newleaf->name="INT";
        return;
    }
    if (strcmp(name,"FLOAT")==0){
        newleaf->type_float=atof(yytext);
        return;
    }
    if (strcmp(name,"ID")==0||strcmp(name,"TYPE")==0){
        strcpy(newleaf->character,yytext);
        //newleaf->character=chars;
        return;
    }
    return;
}

treenode* addnode(char* name,int child_num,...){
    treenode* newnode=(treenode*)malloc(sizeof(treenode));
    treenode* childnode;
    newnode->name=name;
    va_list valist;
    va_start(valist,child_num);
    newnode->child=NULL;
    newnode->line=0;    //only Neo will have line==0
    if (child_num==0) return newnode;
    childnode=va_arg(valist,treenode*);
    newnode->line=childnode->line;
    newnode->child=childnode;
    //printf("\n\n%s\n%s ",name,childnode->name);
    for (int i=2; i<=child_num; i++){
        childnode->sibling=va_arg(valist,treenode*);
        childnode=childnode->sibling;
        //printf("%s ",childnode->name);
    }
    return newnode;
}

void traversal(treenode* root,int indent){
    if (root->line!=0){ //Neo
        for (int i=1; i<=indent; i++) printf(" ");
        printf("%s",root->name);
        if (strcmp(root->name,"ID")==0) printf(": %s\n",root->character);
        else{
            if (strcmp(root->name,"TYPE")==0) printf(": %s\n",root->character);
            else{
                if (strcmp(root->name,"INT")==0) printf(": %d\n",root->type_int);
                else{
                    if (strcmp(root->name,"FLOAT")==0) printf(": %f\n",root->type_float);
                    else{ 
                        if (root->child!=NULL)
                            printf(" (%d)\n",root->line);
                        else printf("\n");
                    }
                }
            }
        }
    }
    if (root->child!=NULL) traversal(root->child,indent+2);
    if (root->sibling!=NULL) traversal(root->sibling,indent);
}
