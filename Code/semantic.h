#ifndef _COMPLIERS_CODE_SEMANTIC
#define _COMPLIERS_CODE_SEMANTIC

#include "tree.h"
void newst(treenode*);
void newkind(treenode*,int);
void newarray(treenode*,int);
void newpara(treenode*,int);
void newfunc(treenode*,int);
void newdefst(treenode*);

void handle_Exp(treenode*);
void deal_func_use(treenode*,funcnode*);
void basicoutput(int);
void field_add(treenode*,senode*,int);
funcnode* hash_funcfind(char*);
senode* hash_find(char*);
int find_st_id(senode*,char*,int);
void hash_insert(senode*);
void hash_funcinsert(funcnode*);
uint32_t hash_pjw(char*);
#endif

