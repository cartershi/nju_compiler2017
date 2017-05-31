#include "common.h"
#include "tree.h"
#include "semantic.h"

void semantic_traversal(treenode* root){
    #ifdef LAB3
    freopen("semantic.tt","w",stdout);
    #endif
    root->info=(senode*)malloc(sizeof(senode));
    root->info->type=(setype*)malloc(sizeof(setype));
    if (root->child!=NULL) semantic_traversal(root->child);
    //printf("%s %d\n",root->name,root->line);
    char* name=root->name;
    if (strcmp(name,"Program")==0){
    }
    if (strcmp(name,"ExtDefList")==0){
    }
    if (strcmp(name,"ExtDef")==0){
        if (strcmp(root->child->sibling->name,"FunDec")==0){ //Specifier FunDec CompSt
            if (root->child->info->type->kind==BASIC){
                int bb=0;
                if (strcmp(root->child->info->name,"int")==0) bb=0; else bb=1;
                newfunc(root->child->sibling,bb); 
                if (root->child->sibling->sibling->exp_type!=bb)
                    printf("Error type 8 at Line %d: Type mismatched for return.\n",root->child->sibling->sibling->returnline);
            }
            if (root->child->info->type->kind==STRUCTURE){
                //no case like this
            }

        }
        if (strcmp(root->child->sibling->name,"ExtDecList")==0){  
           if (root->child->info->type->kind==BASIC){
               if (strcmp(root->child->info->name,"int")==0)
                newkind(root,0);    //int
            else 
                newkind(root,1);    //float
        }
           if (root->child->info->type->kind==STRUCTURE){
               newdefst(root);
           }
        }
    }
    if (strcmp(name,"ExtDecList")==0){
    }
    if (strcmp(name,"Specifier")==0){
        if(strcmp(root->child->name,"TYPE")==0){
            root->info->name=root->child->character;//record int||float
            //printf("%s\n",root->info->name);
            root->info->type->kind=BASIC;
        }
        if(strcmp(root->child->name,"TYPE")!=0){ //struct
            root->info->name=root->child->info->name; //struct name
            root->info->type->kind=STRUCTURE;
            //waiting
        }
    }
    if (strcmp(name,"StructSpecifier")==0){
        //printf("StructSp\n");
        if (strcmp(root->child->sibling->name,"OptTag")==0){
            root->info->name=root->child->sibling->info->name;
            root->info->struct_name=root->info->name;
            //printf("%s\n",root->info->name);
            newst(root);
        }
        if (strcmp(root->child->sibling->name,"Tag")==0){
            treenode* Tagnode=root->child->sibling;
            if (hash_find(Tagnode->info->name)==NULL){
                root->exp_type=EMISS;
                printf("Error type 17 at Line %d: Undefined structure \"%s\".\n",Tagnode->line,Tagnode->info->name);
            }
            else{
                free(root->info);
                root->info=hash_find(Tagnode->info->name);
            }            
        }
    }
    if (strcmp(name,"OptTag")==0){
        if (root->child!=NULL){
            root->info->name=root->child->character;//record id
        }
    }
    if (strcmp(name,"Tag")==0){      
        root->info->name=root->child->character;//record id 
    }
    if (strcmp(name,"VarDec")==0){
        if(strcmp(root->child->name,"ID")==0){
            root->info->name=root->child->character;//record basic->type
            root->info->type->kind=BASIC;
        }
        if(strcmp(root->child->name,"VarDec")==0){
            root->info->name=root->child->info->name; //array's name
            root->info->type->kind=ARRAY;
            root->info->type->array.size=root->child->sibling->sibling->type_int; //size
        }
    }
    if (strcmp(name,"FunDec")==0){
        /*if ((root->child->sibling->sibling->name,"VarList")==0){

          }*/
        root->info->name=root->child->info->name;
    }
    if (strcmp(name,"VarList")==0){
    }
    if (strcmp(name,"ParamDec")==0){    //Specifier VarDec
        if (root->child->info->type->kind==BASIC){
            if (strcmp(root->child->info->name,"int")==0)
                newpara(root,0);    //int
            else 
                newpara(root,1);    //float
        }
        if (root->child->info->type->kind==STRUCTURE){

        }
    }
    if (strcmp(name,"CompSt")==0){
        root->exp_type=root->child->sibling->sibling->exp_type;
        root->returnline=root->child->sibling->sibling->returnline;
    }
    if (strcmp(name,"StmtList")==0){
        if (root->child!=NULL){
            if (root->child->sibling->child!=NULL){
                root->exp_type=root->child->sibling->exp_type;
                root->returnline=root->child->sibling->returnline;
            }
            else{
                root->exp_type=root->child->exp_type;
                root->returnline=root->child->line;
            }
        }
    }
    if (strcmp(name,"Stmt")==0){
        if (strcmp(root->child->name,"RETURN")==0){
            root->exp_type=root->child->sibling->exp_type;
            root->returnline=root->child->sibling->line;
        }

    }
    if (strcmp(name,"DefList")==0){ //nothing to do
    }
    if (strcmp(name,"Def")==0){
        if (root->child->info->type->kind==BASIC){
            if (strcmp(root->child->info->name,"int")==0)
                newkind(root,0);    //int
            else 
                newkind(root,1);    //float
        }
        if (root->child->info->type->kind==STRUCTURE){
            newdefst(root);
        }
    }
    if (strcmp(name,"DecList")==0){
        root->info->name=root->child->info->name;
    }
    if (strcmp(name,"Dec")==0){
        root->info->name=root->child->info->name;
        //printf("%s\n",root->info->name);
        root->info->type->kind=root->child->info->type->kind;
    }
    if (strcmp(name,"Exp")==0) handle_Exp(root);
    if (strcmp(name,"Args")==0); //handle_Args(root);
    if (root->sibling!=NULL) semantic_traversal(root->sibling);
}

void handle_Exp(treenode* root){
    //printf("name %s\n",root->child->character);
    if (strcmp(root->child->name,"Exp")==0){
        if (strcmp(root->child->sibling->name,"ASSIGNOP")==0){  //Exp ASSIGNOP Exp
            treenode* assnode=root->child->child;
            if (!((strcmp(assnode->name,"ID")==0&&assnode->sibling==NULL)
                        ||(strcmp(assnode->name,"Exp")==0&&strcmp(assnode->sibling->name,"LB")==0)
                        ||(strcmp(assnode->name,"Exp")==0&&strcmp(assnode->sibling->name,"DOT")==0))){
                printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n",root->child->line);
            }
            else{
                if (root->child->exp_type==EMISS
                        ||root->child->sibling->sibling->exp_type==EMISS
                        ||root->child->exp_type==root->child->sibling->sibling->exp_type){
                    root->exp_type=root->child->exp_type;
                    if (root->exp_type < root->child->sibling->sibling->exp_type)
                        root->exp_type=root->child->sibling->sibling->exp_type;
                }
                else
                    printf("Error type 5 at Line %d: Type mismatched for assignment.\n",root->line);
            }
            return;
        }   //Exp AND/OR Exp
        if (strcmp(root->child->sibling->name,"AND")==0
                ||strcmp(root->child->sibling->name,"OR")==0){
            if (root->child->exp_type!=EFLOAT&&root->child->sibling->sibling->exp_type!=EFLOAT){
                root->exp_type=root->child->exp_type;    //EINT or EEMISS
                if (root->exp_type > root->child->sibling->sibling->exp_type)
                    root->exp_type=root->child->sibling->sibling->exp_type;
                return;
            }
            printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->line);
            return;
        }
        if (strcmp(root->child->sibling->name,"LB")==0){
            treenode* expnode=root->child->sibling->sibling;
            if (expnode->exp_type!=EINT&&expnode->exp_type!=EMISS){
                //if (strcmp(expnode->child->name,"FLOAT")==0)
                printf("Error type 12 at Line %d: \"%s\" is not an integer.\n",expnode->line,expnode->child->rawtext);
            }
            if (root->child->exp_type!=EAARAY&&root->child->exp_type!=EMISS)
                printf("Error type 10 at Line %d: \"%s\" is not an array.\n",root->line,root->child->info->name);
            //waiting
            return;
        }
        if (strcmp(root->child->sibling->name,"DOT")==0){
            //waiting
            senode* goal=hash_find(root->child->info->name);
            if (goal->type->kind!=STRUCTURE)
                printf("Error type 13 at Line %d: Illegal use of \".\".\n",root->child->sibling->line);
            else
            {
                int findstre=find_st_id(goal,root->child->sibling->sibling->character,root->child->sibling->sibling->line);
                if (findstre==1){
                    goal=hash_find(root->child->sibling->sibling->character);
                    root->exp_type=goal->type->basic;
                    //printf(". %d\n",root->exp_type);
                }
                else
                    root->exp_type=EMISS;
            }
            return;
        }   //Exp RELOP/PLUS/MINUS/STAR/DIV Exp
        ////printf("%d\n",root->child->exp_type);
        if (root->child->exp_type==EMISS
                ||root->child->sibling->sibling->exp_type==EMISS
                ||root->child->exp_type==root->child->sibling->sibling->exp_type){
            root->exp_type=root->child->exp_type;
            if (root->exp_type < root->child->sibling->sibling->exp_type)
                root->exp_type=root->child->sibling->sibling->exp_type;
        }
        else
            printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->line);
        return;
    }
    //printf("pass1\n");
    if (strcmp(root->child->name,"LP")==0){    //LP Exp RP
        root->exp_type=root->child->sibling->exp_type;
        root->exp_rec=root->child->sibling->exp_rec;
    }
    if (strcmp(root->child->name,"MINUS")==0){ //MINUS Exp
        root->exp_type=root->child->sibling->exp_type;
        root->exp_rec=root->child->sibling->exp_rec;
    }
    if (strcmp(root->child->name,"NOT")==0){   //NOT Exp
        if (root->child->sibling->exp_type==EFLOAT){
            printf("Error type 7 at Line %d: Type mismatched for operands.\n",root->line);
            return; 
        }
        root->exp_type=root->child->sibling->exp_type;
        root->exp_rec=root->child->sibling->exp_rec;
    }
    //printf("pass2\n");
    if (strcmp(root->child->name,"ID")==0){
        if (root->child->sibling!=NULL){
            funcnode* goalnode=hash_funcfind(root->child->character);//find the func namedID
          
            if (goalnode!=NULL){
                deal_func_use(root,goalnode);
                root->exp_type=goalnode->kind;
            }
            else
            {
                if (hash_find(root->child->character)==NULL)
                    printf("Error type 2 at Line %d: Undefined function \"%s\".\n",root->line,root->child->character);
                else 
                    printf("Error type 11 at Line %d: \"%s\" is not a function.\n",root->line,root->child->character);
                root->exp_type=EMISS;

            }
        }
        else{   //ID
            senode* goalnode=hash_find(root->child->character);
            root->info->name=root->child->character;
            if (goalnode==NULL){
                printf("Error type 1 at Line %d: Undefined variable \"%s\".\n",root->child->line,root->child->character);                
                root->exp_type=EMISS;
                return;
            }
            if (goalnode->type->kind==BASIC)
                root->exp_type=goalnode->type->basic;
            if (goalnode->type->kind==ARRAY)
                root->exp_type=EAARAY;
            if (goalnode->type->kind==STRUCTURE)
                root->exp_type=ESTRUCT;
        }
    }
    if (strcmp(root->child->name,"INT")==0){   //INT
        root->exp_type=EINT;
        root->exp_rec.exp_int=root->child->type_int;
    }
    if (strcmp(root->child->name,"FLOAT")==0){ //FLOAT
        root->exp_type=EFLOAT;
        //printf("hhh\n");
        root->exp_rec.exp_float=root->child->type_float;
    }
    //printf("%s end\n",root->child->character);
}

int find_st_id(senode* root,char* name,int line){
    fieldlist* nextlist=root->type->structure;
    int flag=0;
    while (nextlist!=NULL){
        if (strcmp(nextlist->name,name)==0) flag=1;
        nextlist=nextlist->tail;
    }
    if (flag==0)
        printf("Error type 14 at Line %d: Non-existent field \"%s\".\n",line,name);
    return flag;
}
void deal_func_use(treenode* root,funcnode* goalnode){
    int callfunc[20],call_num=-1,callequal=0,funcsize;
    treenode* Argsnode=root->child->sibling->sibling->child;
    while (Argsnode!=NULL){
        call_num++;
        callfunc[call_num]=Argsnode->exp_type;
        Argsnode=Argsnode->sibling;
        if (Argsnode!=NULL) Argsnode=Argsnode->sibling->child;
    }
    int i;
    funcsize=goalnode->par_num;
    if (funcsize>call_num) funcsize=call_num;
    if (call_num!=goalnode->par_num) callequal=1;
    for (i=0; i<=funcsize; i++)
        if (goalnode->para[i]->type->basic!=callfunc[i]) callequal=1;
    if (callequal==1){
        printf("Error type 9 at Line %d: Function \"%s(",root->line,root->child->character);
        for (i=0; i<goalnode->par_num; i++){
            basicoutput(goalnode->para[i]->type->basic);
            printf(",");
        }
        basicoutput(goalnode->para[goalnode->par_num]->type->basic);
        printf(")\" is not applicable for arguments\"(");
        for (i=0; i<call_num; i++){
            basicoutput(callfunc[i]);
            printf(",");
        }
        basicoutput(callfunc[call_num]);
        printf(")\".\n");
    }
}

void basicoutput(int bb){
    if (bb==0) printf("int");
    else printf("float");
}

void newexfst(treenode* root){

}
void newdefst(treenode* root){
    fieldlist* startlist=root->child->child->info->type->structure;
    treenode* Defnode=root->child->sibling;
    senode* goal;
    while (Defnode!=NULL){
        Defnode=Defnode->child;
        goal=hash_find(Defnode->info->name);
        if (goal==NULL){
            Defnode->info->type->structure=startlist;
            Defnode->info->type->kind=STRUCTURE;
            hash_insert(Defnode->info);
        }
        else
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",Defnode->line,Defnode->info->name);
        
        Defnode=Defnode->sibling;
        if (Defnode!=NULL) Defnode=Defnode->sibling;
    }
}
void newfunc(treenode* root,int bb){
    //printf("newfunc\n");
    root->funcinfo=(funcnode*)malloc(sizeof(funcnode));
    root->funcinfo->kind=bb;
    root->funcinfo->name=root->child->character;
    funcnode* goal=hash_funcfind(root->funcinfo->name);
    if (goal==NULL){
        if (strcmp(root->child->sibling->sibling->name,"VarList")==0){
            root->funcinfo->par_num=-1;
            treenode* Varnode=root->child->sibling;
            while (Varnode!=NULL){
                Varnode=Varnode->sibling;
                root->funcinfo->para[++root->funcinfo->par_num]=
                    hash_find(Varnode->child->child->sibling->info->name);
                Varnode=Varnode->child->sibling;
            }
            hash_funcinsert(root->funcinfo);
        }
    }
    else 
        printf("Error type 4 at Line %d: Redefined function \"%s\".\n",root->line,root->funcinfo->name);
}

void newst(treenode* root){
    treenode* Defnode=root->child->sibling->sibling->sibling;
    treenode* Decnode;
    senode* nextnode;
    while(Defnode->child!=NULL){
        Decnode=Defnode->child->child->sibling;
        while (Decnode!=NULL){
            Decnode=Decnode->child;
            //printf("%s\n",Decnode->info->name);
            nextnode=hash_find(Decnode->info->name);
            field_add(root,nextnode,Decnode->line);
            Decnode=Decnode->sibling;
            if(Decnode!=NULL) Decnode=Decnode->sibling;;
        }
        Defnode=Defnode->child->sibling;
    }
    /*fieldlist* jjjj=root->info->type->structure;
    while (jjjj!=NULL){
        printf("%d\n",jjjj->type->basic);
        jjjj=jjjj->tail;}*/
    if (root->info->struct_name!=NULL){
        if (hash_find(root->info->struct_name)!=NULL)
            printf("Error type 16 at Line %d: Duplicated name \"%s\".\n",root->line,root->info->struct_name);
        else 
            hash_insert(root->info);
    }
}

void field_add(treenode* root,senode* nextnode,int line){
    fieldlist* newfield;
    if (root->info->type->structure==NULL){
        newfield=(fieldlist*)malloc(sizeof(fieldlist));
        newfield->name=nextnode->name;
        newfield->type=nextnode->type;
        newfield->tail=NULL;
        root->info->type->structure=newfield;
    }
    else{
        int flag=0;
        fieldlist* travelfield=root->info->type->structure,*fatherfield;
        while (travelfield!=NULL){
            if (strcmp(travelfield->name,nextnode->name)==0){
                printf("Error type 15 at Line %d: Redefined field \"%s\".\n",line,nextnode->name);
                flag=1;
            }
            fatherfield=travelfield;
            travelfield=travelfield->tail;
        }
        if (flag==0){
            newfield=(fieldlist*)malloc(sizeof(fieldlist));
            newfield->name=nextnode->name;
            newfield->type=nextnode->type;
            newfield->tail=NULL;
            fatherfield->tail=newfield;
        }
    }
}
void newpara(treenode* root,int bb){
    treenode* Decnode=root->child->sibling;
    senode* goal=hash_find(Decnode->info->name);
    if (goal==NULL){
        if (Decnode->info->type->kind==BASIC){
            //Decnode->info->type->kind=BASIC;
            Decnode->info->type->basic=bb;
            hash_insert(Decnode->info);
        }
        if (Decnode->info->type->kind==ARRAY){
            newarray(Decnode->child,bb);
        }
    }
    else
        printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",Decnode->line,Decnode->info->name);
}

void newarray(treenode* root,int bb){
    treenode* head=root;
    root->info->type->array.elem=(setype*)malloc(sizeof(setype));
    root->info->type->array.elem->kind=BASIC;
    root->info->type->array.elem->basic=bb;

    while (head->info->type->kind==ARRAY){
        head->child->info->type->array.elem=head->info->type;
        head=head->child;
    }
    head->info->type->kind=ARRAY;
    hash_insert(head->info);
}

void newkind(treenode* root,int bb){
    //printf("line %d\n",root->line);
    treenode* Decnode=root->child;
    senode* goal;
    while (Decnode!=NULL){
        Decnode=Decnode->sibling->child;
        goal=hash_find(Decnode->info->name);
        if (goal==NULL){
            if (Decnode->info->type->kind==BASIC){
                //Decnode->info->type->kind=BASIC;
                Decnode->info->type->basic=bb;
                hash_insert(Decnode->info);
            }
            if (Decnode->info->type->kind==ARRAY){
                newarray(Decnode->child,bb);
            }
        }
        else
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",Decnode->line,Decnode->info->name);
        Decnode=Decnode->sibling;
    }

}

funcnode* hash_funcfind(char* name){
    //printf("funcfind %s\n",name);
    funcnode* hashnode=hashfunctable[hash_pjw(name)].next;
    while (hashnode!=NULL&&strcmp(hashnode->name,name)!=0)
        hashnode=hashnode->next;
    return hashnode;
}

senode* hash_find(char* name){
    senode* hashnode=hashtable[hash_pjw(name)].next;
    while (hashnode!=NULL&&strcmp(hashnode->name,name)!=0)
        hashnode=hashnode->next;
    return hashnode;
}

void hash_funcinsert(funcnode* info){
    /*printf("%s %d\n",info->name,info->par_num);
      for (int i=0; i<=info->par_num; i++)
      printf("%s %d ",info->para[i]->name,info->para[i]->type->kind);*/
    uint32_t hash_loc=hash_pjw(info->name);
    info->next=hashfunctable[hash_loc].next;
    hashfunctable[hash_loc].next=info;
}

void hash_insert(senode* info){
    uint32_t hash_loc=hash_pjw(info->name);
    //printf("%s %d\n",info->name,info->type->basic);
    info->next=hashtable[hash_loc].next;
    hashtable[hash_loc].next=info;
}

uint32_t hash_pjw(char* name){
    //printf("%s\n",name);
    uint32_t val = 0, i;
    for (; *name; ++name){
        val = (val << 2) + *name;
        if (i = val & ~0x3fff) val = (val ^ (i >> 12)) & 0x3fff;
    }
    return val;
}

void add_read_write(){
    funcnode* funcread=(funcnode*)malloc(sizeof(funcnode));
    funcread->kind=0;
    funcread->par_num=-1;
    funcread->name="read";
    hash_funcinsert(funcread);
    funcnode* funcwrite=(funcnode*)malloc(sizeof(funcnode));
    funcwrite->name="write";
    funcwrite->kind=0;
    funcwrite->par_num=0;
    funcwrite->para[0]=(senode*)malloc(sizeof(senode));
    funcwrite->para[0]->type=(setype*)malloc(sizeof(setype));
    funcwrite->para[0]->type->kind=BASIC;
    funcwrite->para[0]->type->basic=0;
    hash_funcinsert(funcwrite);
}
void hash_init(){
    for (int i=0; i<16384; i++) hashtable[i].next=NULL;
    for (int i=0; i<16384; i++) hashfunctable[i].next=NULL;
    add_read_write();
}
