#include "common.h"
#include "tree.h"
#include "operand.h"
#include "semantic.h"

InterCodes* translate_Program(treenode* root){
    return translate_ExtDefList(root->child);
}

InterCodes* translate_ExtDefList(treenode* root){
    if (root->child!=NULL){
        InterCodes* code1=translate_ExtDef(root->child);
        InterCodes* code2=translate_ExtDefList(root->child->sibling);
        return bindCode(code1,code2);
    }
    return NULL;
}

InterCodes* translate_ExtDef(treenode* root){
    if (!strcmp(root->child->sibling->name,"FunDec")){
        InterCodes* code1=translate_FunDec(root->child->sibling);
        InterCodes* code2=translate_CompSt(root->child->sibling->sibling);
        return bindCode(code1,code2);
    }
    return NULL;
}

InterCodes* translate_FunDec(treenode* root){
    InterCodes* code1=new_InterCodes();
    code1->code.kind=FUNCTION;
    Operand* op1=new_id(root->child->character);
    code1->code.u.sigop.result=op1;
    if (!strcmp(root->child->sibling->sibling->name,"VarList")){
        InterCodes* code2=translate_VarList(root->child->sibling->sibling);
        return bindCode(code1,code2);
    }
    return code1;
}


InterCodes* translate_VarList(treenode* root){
    InterCodes* code1=translate_ParamDec(root->child);;
    if (root->child->sibling!=NULL){
        InterCodes* code2=translate_VarList(root->child->sibling->sibling);
        return bindCode(code1,code2);
    }
    return code1;
}

InterCodes* translate_ParamDec(treenode* root){
    InterCodes* code1=translate_VarDec(root->child->sibling);
    if (code1!=NULL){
        printf("Cannot translate: Code contains variables or parameters of structure type.\n");
        exit(0);
    }
    //printf("%s\n",root->child->sibling->info->name);
    Operand* op1=new_id(root->child->sibling->info->name);
    InterCodes* code2=new_InterCodes();
    code2->code.kind=PARAM;
    code2->code.u.sigop.result=op1;
    return code2;
}
InterCodes* translate_CompSt(treenode* root){
    InterCodes* code1=translate_DefList(root->child->sibling);
    InterCodes* code2=translate_StmtList(root->child->sibling->sibling);
    return bindCode(code1,code2);
}

InterCodes* translate_DefList(treenode* root){
    if (root->child!=NULL){
        InterCodes* code1=translate_Def(root->child);
        InterCodes* code2=translate_DefList(root->child->sibling);
        return bindCode(code1,code2);
    }
    return NULL;
}

InterCodes* translate_Def(treenode* root){
    return translate_DecList(root->child->sibling);
}

InterCodes* translate_DecList(treenode* root){
    InterCodes* code1=translate_Dec(root->child);
    if (root->child->sibling!=NULL){
        InterCodes* code2=translate_DecList(root->child->sibling->sibling);
        return bindCode(code1,code2);
    }
    return code1;
}

InterCodes* translate_Dec(treenode* root){
    if (root->child->sibling!=NULL){
        InterCodes* code1=new_InterCodes();
        Operand* op1=new_id(root->child->child->character);
        Operand* op2=new_const(root->child->sibling->sibling->exp_rec.exp_int);
        code1->code.kind=ASSIGN;
        code1->code.u.assign.left=op1;
        code1->code.u.assign.right=op2;
        return code1;
    }
    InterCodes* code1=translate_VarDec(root->child);
    return code1;
}

InterCodes* translate_VarDec(treenode* root){
    if (!strcmp(root->child->name,"VarDec")){
        return translate_VarDec(root->child);
    }
    if (!strcmp(root->child->name,"ID")){
        senode* goalnode=hash_find(root->child->character);
        if (goalnode->type->kind==ARRAY){
            setype* arraynode=goalnode->type->array.elem;
            if (arraynode->array.elem->kind!=BASIC){
                printf("Cannot translate: Code contains variables of multi-dimensional array type or parameters of array type.\n");
                exit(0);
            }
            InterCodes* code1=new_InterCodes();
            Operand* op1=new_id(root->child->character);
            Operand* op2=new_int(4*arraynode->array.size); 
            code1->code.kind=DEC;
            code1->code.u.assign.left=op1;
            code1->code.u.assign.right=op2;
            return code1;
        }
    }
    return NULL;
}
InterCodes* translate_StmtList(treenode* root){
    if (root->child!=NULL){
        InterCodes* code1=translate_Stmt(root->child);
        InterCodes* code2=translate_StmtList(root->child->sibling);
        return bindCode(code1,code2);
    }
    return NULL;
}

InterCodes* translate_Stmt(treenode* root){
    if (!strcmp(root->child->name,"Exp")){
        return translate_Exp(root->child,NULL);
    }
    if (!strcmp(root->child->name,"CompSt")){
        return translate_CompSt(root->child);
    }
    if (!strcmp(root->child->name,"RETURN")){
        Operand* op1=new_temp();
        InterCodes* code1=translate_Exp(root->child->sibling,op1);
        InterCodes* code2=new_InterCodes();
        code2->code.kind=RETURN_;
        code2->code.u.sigop.result=op1;
        return bindCode(code1,code2);
    }
    if (!strcmp(root->child->name,"IF")){
        treenode* expnode,*stmtnode1,*stmtnode2;
        expnode=root->child->sibling->sibling;
        stmtnode1=expnode->sibling->sibling;
        if (stmtnode1->sibling==NULL){      //IF LP Exp RP Stmt
            Operand* l1=new_label();
            Operand* l2=new_label();
            InterCodes* code1=translate_Cond(expnode,l1,l2);
            InterCodes* code3=translate_Stmt(stmtnode1);

            InterCodes* code2=new_InterCodes();
            code2->code.kind=LABEL_CODE;
            code2->code.u.sigop.result=l1;

            InterCodes* code4=new_InterCodes();
            code4->code.kind=LABEL_CODE;
            code4->code.u.sigop.result=l2;
            return bindCode(bindCode(bindCode(code1,code2),code3),code4);
        }
        else{   //IF LP Exp RP Stmt ELSE STMT
            stmtnode2=stmtnode1->sibling->sibling;
            Operand* l1=new_label();
            Operand* l2=new_label();
            Operand* l3=new_label();
            InterCodes* code1=translate_Cond(expnode,l1,l2);
            InterCodes* code3=translate_Stmt(stmtnode1);
            InterCodes* code6=translate_Stmt(stmtnode2);

            InterCodes* code2=new_InterCodes();
            code2->code.kind=LABEL_CODE;
            code2->code.u.sigop.result=l1;

            InterCodes* code4=new_InterCodes();
            code4->code.kind=LABEL_GOTO;
            code4->code.u.sigop.result=l3;

            InterCodes* code5=new_InterCodes();
            code5->code.kind=LABEL_CODE;
            code5->code.u.sigop.result=l2;

            InterCodes* code7=new_InterCodes();
            code7->code.kind=LABEL_CODE;
            code7->code.u.sigop.result=l3;
            code1=bindCode(bindCode(bindCode(code1,code2),code3),code4);
            return bindCode(bindCode(bindCode(code1,code5),code6),code7);
        }
    }
    if (!strcmp(root->child->name,"WHILE")){
        treenode* expnode,*stmtnode1;
        expnode=root->child->sibling->sibling;
        stmtnode1=expnode->sibling->sibling;
        Operand* l1=new_label();
        Operand* l2=new_label();
        Operand* l3=new_label();
        InterCodes* code2=translate_Cond(expnode,l2,l3);
        InterCodes* code4=translate_Stmt(stmtnode1);

        InterCodes* code1=new_InterCodes();
        code1->code.kind=LABEL_CODE;
        code1->code.u.sigop.result=l1;

        InterCodes* code3=new_InterCodes();
        code3->code.kind=LABEL_CODE;
        code3->code.u.sigop.result=l2;

        InterCodes* code5=new_InterCodes();
        code5->code.kind=LABEL_GOTO;
        code5->code.u.sigop.result=l1;

        InterCodes* code6=new_InterCodes();
        code6->code.kind=LABEL_CODE;
        code6->code.u.sigop.result=l3;
        code1=bindCode(bindCode(bindCode(code1,code2),code3),code4);
        return bindCode(bindCode(code1,code5),code6);
    }
}

InterCodes* translate_Cond(treenode* root,Operand* l_true,Operand* l_false){
    if (!strcmp(root->child->name,"Exp")&&
            root->child->sibling!=NULL){
        treenode* expnode1,*expnode2;
        expnode1=root->child;
        expnode2=root->child->sibling->sibling;
        if (!strcmp(root->child->sibling->name,"RELOP")){  //RELOP
            Operand* op1=new_temp();
            Operand* op3=new_temp();
            InterCodes* code1=translate_Exp(expnode1,op1);
            InterCodes* code2=translate_Exp(expnode2,op3);
            Operand* op2=new_opera(get_relop(root->child->sibling));

            InterCodes* code3=new_InterCodes();
            code3->code.kind=JUMP;
            code3->code.u.jmp.op1=op1;
            code3->code.u.jmp.op2=op2;
            code3->code.u.jmp.op3=op3;
            code3->code.u.jmp.goal=l_true;

            InterCodes* code4=new_InterCodes();
            code4->code.kind=LABEL_GOTO;
            code4->code.u.sigop.result=l_false;
            return bindCode(bindCode(bindCode(code1,code2),code3),code4);
        }
        if (!strcmp(root->child->sibling->name,"AND")){    //Exp AND Exp
            Operand* op1=new_label();
            InterCodes* code1=translate_Cond(expnode1,op1,l_false);
            InterCodes* code3=translate_Cond(expnode2,l_true,l_false);
            InterCodes* code2=new_InterCodes();
            code2->code.kind=LABEL_CODE;
            code2->code.u.sigop.result=op1;
            return bindCode(bindCode(code1,code2),code3);
        }
        if (!strcmp(root->child->sibling->name,"OR")){     //Exp OR Exp
            Operand* op1=new_label();
            InterCodes* code1=translate_Cond(expnode1,l_true,op1);
            InterCodes* code3=translate_Cond(expnode2,l_true,l_false);
            InterCodes* code2=new_InterCodes();
            code2->code.kind=LABEL_CODE;
            code2->code.u.sigop.result=op1;
            return bindCode(bindCode(code1,code2),code3);
        }
    }
    if (!strcmp(root->child->name,"NOT")){      //NOT Exp
        return translate_Cond(root->child->sibling,l_false,l_true);
    }
    Operand* op1=new_temp();        //other
    InterCodes* code1=translate_Exp(root->child,op1);
    InterCodes* code2=new_InterCodes();
    code2->code.kind=JUMP;
    code2->code.u.jmp.op1=op1;
    code2->code.u.jmp.op2=new_opera(NE);
    code2->code.u.jmp.op3=new_const(0);
    code2->code.u.jmp.goal=l_true;

    InterCodes* code3=new_InterCodes();
    code3->code.kind=LABEL_GOTO;
    code3->code.u.sigop.result=l_false;
    return bindCode(bindCode(code1,code2),code3);
}

InterCodes* translate_Exp(treenode *root,Operand* op){
    if (!strcmp(root->child->name,"INT")){    //INT
        InterCodes* code3 = new_InterCodes();
        Operand* op1=new_const(root->exp_rec.exp_int);
        code3->code.kind = ASSIGN;
        code3->code.u.assign.right=op1;
        code3->code.u.assign.left=op;
        return code3;
    }
    if (!strcmp(root->child->name,"ID")){     //ID
        if (root->child->sibling==NULL){
            InterCodes* code1=new_InterCodes();
            Operand* op1=new_id(root->child->character);
            code1->code.kind=ASSIGN;
            code1->code.u.assign.left=op;
            code1->code.u.assign.right=op1;
            return code1;
        }
        else{
            treenode* args=root->child->sibling->sibling;
            if(!strcmp(args->name,"RP")){   //ID LP RP
                if (!strcmp(root->child->character,"read")){
                    InterCodes* code1=new_InterCodes();
                    code1->code.kind=READ;
                    code1->code.u.sigop.result=op;
                    return code1;
                }
                InterCodes* code1=new_InterCodes();
                code1->code.kind=CALLFUNC;
                code1->code.u.assign.left=op;
                code1->code.u.assign.right=new_id(root->child->character);
                return code1;
            }
            else{   //ID LP Args RP
                Operand* arg_list[20];
                int length=0;
                InterCodes* code1=translate_Args(args,arg_list,&length);
                if (!strcmp(root->child->character,"write")){
                    InterCodes* code2=new_InterCodes();
                    code2->code.kind=WRITE;
                    //printf("no %d\n",arg_list[1]->u.var_no);
                    code2->code.u.sigop.result=arg_list[1];
                    return bindCode(code1,code2);
                }
                for (int i=1; i<=length; i++){
                    InterCodes* code2=new_InterCodes();
                    code2->code.kind=ARG;
                    code2->code.u.sigop.result=arg_list[i];
                    code1=bindCode(code1,code2);
                }
                InterCodes* code2=new_InterCodes();
                code2->code.kind=CALLFUNC;
                code2->code.u.assign.left=op;
                code2->code.u.assign.right=new_id(root->child->character);
                return bindCode(code1,code2);
            }
        }
    }
    if ((!strcmp(root->child->name,"Exp")&&     //Exp RELOP/AND/OR Exp
                ((!strcmp(root->child->sibling->name,"RELOP")
                  ||!strcmp(root->child->sibling->name,"AND")
                  ||!strcmp(root->child->sibling->name,"OR"))))
            ||!strcmp(root->child->name,"NOT")){       
        Operand* l1=new_label();
        Operand* l2=new_label();
        InterCodes* code0=new_InterCodes();
        code0->code.kind=ASSIGN;
        code0->code.u.assign.left=op;
        code0->code.u.assign.right=new_const(0);

        InterCodes* code1=translate_Cond(root,l1,l2);

        InterCodes* code2=new_InterCodes();
        code2->code.kind=LABEL_CODE;
        code2->code.u.sigop.result=l1;

        InterCodes* code3=new_InterCodes();
        code3->code.kind=ASSIGN;
        code3->code.u.assign.left=op;
        code3->code.u.assign.right=new_const(1);
        return bindCode(bindCode(bindCode(code0,code1),code2),code3);
    }
    if (!strcmp(root->child->name,"Exp")){
        if (!strcmp(root->child->sibling->name,"STAR")){
            Operand* op1=new_temp();
            Operand* op2=new_temp();
            InterCodes* code1 = translate_Exp(root->child,op1);
            InterCodes* code2 = translate_Exp(root->child->sibling->sibling,op2);
            InterCodes* code3 = new_InterCodes();
            code3->code.kind = MUL;
            code3->code.u.binop.op1 = op1;
            code3->code.u.binop.op2 = op2;
            code3->code.u.binop.result=op;
            return bindCode(bindCode(code1,code2),code3);
        }
        if (!strcmp(root->child->sibling->name,"DIV")){
            Operand* op1=new_temp();
            Operand* op2=new_temp();
            InterCodes* code1 = translate_Exp(root->child,op1);
            InterCodes* code2 = translate_Exp(root->child->sibling->sibling,op2);
            InterCodes* code3 = new_InterCodes();
            code3->code.kind = DIV_;
            code3->code.u.binop.op1 = op1;
            code3->code.u.binop.op2 = op2;
            code3->code.u.binop.result=op;
            return bindCode(bindCode(code1,code2),code3);
        }
        if (!strcmp(root->child->sibling->name,"PLUS")){
            Operand* op1=new_temp();
            Operand* op2=new_temp();
            InterCodes* code1 = translate_Exp(root->child,op1);
            InterCodes* code2 = translate_Exp(root->child->sibling->sibling,op2);
            InterCodes* code3 = new_InterCodes();
            code3->code.kind = ADD;
            code3->code.u.binop.op1 = op1;
            code3->code.u.binop.op2 = op2;
            code3->code.u.binop.result=op;
            return bindCode(bindCode(code1,code2),code3);
        }
        if (!strcmp(root->child->sibling->name,"MINUS")){
            Operand* op1=new_temp();
            Operand* op2=new_temp();
            InterCodes* code1 = translate_Exp(root->child,op1);
            InterCodes* code2 = translate_Exp(root->child->sibling->sibling,op2);
            InterCodes* code3 = new_InterCodes();
            code3->code.kind = SUB;
            code3->code.u.binop.op1 = op1;
            code3->code.u.binop.op2 = op2;
            code3->code.u.binop.result=op;
            return bindCode(bindCode(code1,code2),code3);
        }
        if (!strcmp(root->child->sibling->name,"ASSIGNOP")){
            Operand* op1=new_temp();
            Operand* op2=new_temp();
            InterCodes* code1=translate_Exp(root->child->sibling->sibling,op1);
            InterCodes* code2=translate_Exp(root->child,op2);

            if (op2->kind==ADDRESS){
                InterCodes* code3=new_InterCodes();
                code3->code.kind=ASSIGN;
                code3->code.u.assign.left=op2;
                code3->code.u.assign.right=op1;                        

                InterCodes* code4=new_InterCodes();
                code4->code.kind=ASSIGN;
                code4->code.u.assign.left=op;
                code4->code.u.assign.right=op2;
                return bindCode(bindCode(bindCode(code1,code2),code3),code4);
            }
            else{
                code2=new_InterCodes();
                code2->code.kind=ASSIGN;
                op2=new_id(root->child->child->character);
                code2->code.u.assign.left=op2;
                code2->code.u.assign.right=op1;
                InterCodes* code3=new_InterCodes();
                code3->code.kind=ASSIGN;
                code3->code.u.assign.left=op;
                code3->code.u.assign.right=op2;
                return bindCode(bindCode(code1,code2),code3);
            }
        }
        if (!strcmp(root->child->sibling->name,"LB")){
            InterCodes* code1=new_InterCodes();
            Operand* op1=new_temp();
            Operand* op2=new_array(root->child->child->character);
            code1->code.kind=ASSIGN;
            code1->code.u.assign.left=op1;
            code1->code.u.assign.right=op2;

            Operand* op3=new_temp();
            InterCodes* code2=translate_Exp(root->child->sibling->sibling,op3);

            InterCodes* code3=new_InterCodes();
            code3->code.kind=MUL;
            code3->code.u.binop.result=op3;
            code3->code.u.binop.op1=op3;
            code3->code.u.binop.op2=new_const(4);

            InterCodes* code4=new_InterCodes();
            code4->code.kind=ADD;
            code4->code.u.binop.result=op1;
            code4->code.u.binop.op1=op1;
            code4->code.u.binop.op2=op3;

            op->kind=ADDRESS;
            InterCodes* code5=new_InterCodes();
            code5->code.kind=REFASSIGN;
            code5->code.u.assign.left=op;
            code5->code.u.assign.right=op1;
            return bindCode(bindCode(bindCode(bindCode(code1,code2),code3),code4),code5);
        }
    }
    if (!strcmp(root->child->name,"MINUS")){    //MINUS Exp
        Operand* op1=new_temp();
        InterCodes* code1=translate_Exp(root->child->sibling,op1);
        InterCodes* code2=new_InterCodes();
        code2->code.kind=SUB;
        code2->code.u.binop.result=op;
        code2->code.u.binop.op1=new_const(0);
        code2->code.u.binop.op2=op1;
        return bindCode(code1,code2);
    }
    if (!strcmp(root->child->name,"LP")){
        return translate_Exp(root->child->sibling,op);
    }
}

InterCodes* translate_Args(treenode* root, Operand** arg_list,int* length){
    if (root->child->sibling==NULL){    //Exp
        Operand* op1=new_temp();
        InterCodes* code1=translate_Exp(root->child,op1);
        (*length)++;
        for (int i=*length; i>1; i--) arg_list[i]=arg_list[i-1];
        //printf("%d\n",op1->u.var_no);
        arg_list[1]=op1;
        return code1;
    }
    else{   //Exp COMMA Args
        Operand* op1=new_temp();
        InterCodes* code1=translate_Exp(root->child,op1);
        (*length)++;
        for (int i=*length; i>1; i--) arg_list[i]=arg_list[i-1];
        arg_list[1]=op1;
        InterCodes* code2=translate_Args(root->child->sibling->sibling,arg_list,length);
        return bindCode(code1,code2);
    }
}

Operand* new_const(int value){
    Operand* op=(Operand*)malloc(sizeof(Operand));
    op->kind=CONSTANT;
    op->u.value=value;
    return op;
}
Operand* new_addr(){
    Operand* op=(Operand*)malloc(sizeof(Operand));
    op->kind=ADDRESS;
    return op;
}
Operand* new_int(int value){
    Operand* op=(Operand*)malloc(sizeof(Operand));
    op->kind=INTEGER;
    op->u.value=value;
    return op;
}
Operand* new_temp(){
    varnum++;
    Operand* op=(Operand*)malloc(sizeof(Operand));
    op->kind=VARIABLE;
    op->u.var_no=varnum;
    return op;
}
/*Operand* new_var(char* character){
  Operand* op=(Operand*)malloc(sizeof(Operand));
  op->kind=VARIABLE;
  op->u.name=character;
  return op;
  }*/

Operand* new_id(char* character){
    Operand* op=(Operand*)malloc(sizeof(Operand));
    op->kind=ID_STR;
    op->u.name=character;
    return op;
}
Operand* new_array(char* character){
    Operand* op=(Operand*)malloc(sizeof(Operand));
    op->kind=ARRAY_ID;
    op->u.name=character;
    return op;
}
Operand* new_label(){
    labelnum++;
    Operand* op=(Operand*)malloc(sizeof(Operand));
    op->kind=LABEL;
    op->u.lab_no=labelnum;
    return op;
}
Operand* new_opera(op_def value){
    Operand* op=(Operand*)malloc(sizeof(Operand));
    op->kind=OPERATOR;
    op->u.op_num=value;
    return op;
}

op_def get_relop(treenode* root){
    //printf("%s %s\n",root->name,root->character);
    if (!strcmp(root->character,">")) return GT;
    if (!strcmp(root->character,"<")) return LT;
    if (!strcmp(root->character,"==")) return EQ;
    if (!strcmp(root->character,">=")) return GE;
    if (!strcmp(root->character,"<=")) return LE;
    if (!strcmp(root->character,"!=")) return NE;
}

InterCodes* new_InterCodes(){
    InterCodes* code=(InterCodes*)malloc(sizeof(InterCodes));
    code->prev=code->next=code;
    return code;
}

InterCodes* bindCode(InterCodes* code1,InterCodes* code2){
    if (code1==NULL) return code2;
    if (code2!=NULL){
        code1->prev->next=code2;
        code1->prev=code2->prev;
        code2->prev->next=code1;
        code2->prev=code1;
    }
    return code1;
}

void operanddeal(Operand* op){
    switch (op->kind){
        case VARIABLE:
            {
                printf("v%d",op->u.var_no);
                break;
            }
        case CONSTANT:
            {
                printf("#%d",op->u.value);
                break;
            }
        case ADDRESS:
            {
                printf("*v%d",op->u.var_no);
                break;
            }
        case INTEGER:
            {
                printf("%d",op->u.value);
                break;
            }
        case LABEL:
            {
                printf("label%d",op->u.lab_no);
                break;
            }
        case ID_STR:
            {
                printf("%s",op->u.name);
                break;
            }
        case ARRAY_ID:
            {
                printf("&%s",op->u.name);
                break;
            }
        case OPERATOR:
            {
                switch (op->u.op_num){
                    case GT: printf(">"); break;
                    case LT: printf("<"); break;
                    case EQ: printf("=="); break;
                    case GE: printf(">="); break;
                    case LE: printf("<="); break;
                    case NE: printf("!="); break;
                    default: break;
                }
                break;
            }
        default: break;
    }
}
void operandoutput(InterCodes* codelist){
    InterCodes* code=codelist;
    do{
        switch (code->code.kind){
            case ASSIGN:
                {
                    if (code->code.u.assign.left!=NULL){
                        operanddeal(code->code.u.assign.left);
                        printf(" := ");
                        operanddeal(code->code.u.assign.right);
                        printf("\n");
                    }
                    break;
                }
            case ADD:
                {
                    operanddeal(code->code.u.binop.result);
                    printf(" := ");
                    operanddeal(code->code.u.binop.op1);
                    printf(" + ");
                    operanddeal(code->code.u.binop.op2);
                    printf("\n");
                    break;
                }
            case SUB:
                {
                    operanddeal(code->code.u.binop.result);
                    printf(" := ");
                    operanddeal(code->code.u.binop.op1);
                    printf(" - ");
                    operanddeal(code->code.u.binop.op2);
                    printf("\n");
                    break;
                }
            case MUL:
                {
                    operanddeal(code->code.u.binop.result);
                    printf(" := ");
                    operanddeal(code->code.u.binop.op1);
                    printf(" * ");
                    operanddeal(code->code.u.binop.op2);
                    printf("\n");
                    break;
                }
            case DIV_:
                {
                    operanddeal(code->code.u.binop.result);
                    printf(" := ");
                    operanddeal(code->code.u.binop.op1);
                    printf(" / ");
                    operanddeal(code->code.u.binop.op2);
                    printf("\n");
                    break;
                }
            case RETURN_:
                {
                    printf("RETURN ");
                    operanddeal(code->code.u.sigop.result);
                    printf("\n");
                    break;
                }
            case JUMP:
                {
                    printf("IF ");
                    operanddeal(code->code.u.jmp.op1);
                    printf(" ");
                    operanddeal(code->code.u.jmp.op2);
                    printf(" ");
                    operanddeal(code->code.u.jmp.op3);
                    printf(" GOTO ");
                    operanddeal(code->code.u.jmp.goal);
                    printf("\n");
                    break;
                }
            case LABEL_CODE:
                {
                    printf("LABEL ");
                    operanddeal(code->code.u.sigop.result);
                    printf(" :\n");
                    break;
                }
            case LABEL_GOTO:
                {
                    printf("GOTO ");
                    operanddeal(code->code.u.sigop.result);
                    printf("\n");
                    break;
                }
            case READ:
                {
                    printf("READ ");
                    operanddeal(code->code.u.sigop.result);
                    printf("\n");
                    break;
                }
            case WRITE:
                {
                    printf("WRITE ");
                    operanddeal(code->code.u.sigop.result);
                    printf("\n");
                    break;
                }
            case CALLFUNC:
                {
                    operanddeal(code->code.u.assign.left);
                    printf(" := CALL ");
                    operanddeal(code->code.u.assign.right);
                    printf("\n");
                    break;
                }
            case FUNCTION:
                {
                    printf("FUNCTION ");
                    operanddeal(code->code.u.sigop.result);
                    printf(" :\n");
                    break;
                }
            case ARG:
                {
                    printf("ARG ");
                    operanddeal(code->code.u.sigop.result);
                    printf("\n");
                    break;
                }
            case PARAM:
                {
                    printf("PARAM ");
                    operanddeal(code->code.u.sigop.result);
                    printf("\n");
                    break;
                }
            case DEC:
                {
                    printf("DEC ");
                    operanddeal(code->code.u.assign.left);
                    printf(" ");
                    operanddeal(code->code.u.assign.right);
                    printf("\n");
                    break;
                }
            case REFASSIGN:
                {
                    int flag=0;
                    if (code->code.u.assign.left->kind==ADDRESS){
                        code->code.u.assign.left->kind=VARIABLE;
                        flag=1;
                    }
                    operanddeal(code->code.u.assign.left);
                    if (flag==1){
                        code->code.u.assign.left->kind=ADDRESS;
                        flag=0;
                    }
                    printf(" := ");
                    if (code->code.u.assign.right->kind==ADDRESS){
                        code->code.u.assign.right->kind=VARIABLE;
                        flag=1;
                    }
                    operanddeal(code->code.u.assign.right);
                    if (flag==1){
                        code->code.u.assign.right->kind=ADDRESS;
                        flag=0;
                    }
                    printf("\n");
                    break;
                }
            default: break;
        }
        code=code->next;
    }
    while(code!=codelist);
}
void operand_traversal(treenode* root){
    InterCodes* codelist=translate_Program(root);
    operandoutput(codelist);
}
void operand_init(){
    varnum=0;
    labelnum=0;
}
