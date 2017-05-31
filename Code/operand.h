#ifndef _COMPLIERS_CODE_OPERAND
#define _COMPLIERS_CODE_OPERAND
#include "tree.h"
typedef enum {GT, LT, EQ, GE, LE, NE} op_def;
typedef struct Operand_ Operand;
typedef struct InterCode_ InterCode;
typedef struct InterCodes_ InterCodes;
struct Operand_ {
    enum { VARIABLE, CONSTANT, ADDRESS, LABEL, ID_STR, OPERATOR} kind;
    union {
        int var_no;
        int value;
        int lab_no;
        char* name;
        op_def op_num;
    } u;
};
struct InterCode_
{
    enum{
        ASSIGN, ADD, SUB, MUL,
        DIV_, RETURN_, JUMP, LABEL_CODE, LABEL_TRUE,
        LABEL_GOTO, READ, WRITE, CALLFUNC, FUNCTION,
        ARG, PARAM, REFASSIGN, DEC } kind;
    union {
        struct { Operand *right, *left; } assign;
        struct { Operand *result, *op1, *op2; } binop;
        struct { Operand *result; } sigop;
        struct { Operand *op1, *op2, *op3, *goal; } jmp;
    } u;
};

struct InterCodes_ { 
    struct InterCode_ code; 
    struct InterCodes_ *prev, *next;
};

InterCodes* bindCode(InterCodes*,InterCodes*);
InterCodes* new_InterCodes();
static int varnum;
static int labelnum;
Operand* new_temp();
Operand* new_id(char*);
//Operand* new_var(char*);
Operand* new_label();
Operand* new_opera(op_def);
Operand* new_const(int);
op_def get_relop(treenode*);

void operanddeal(Operand*);
void operandoutput(InterCodes*);
InterCodes* translate_Program(treenode*);
InterCodes* translate_ExtDefList(treenode*);
InterCodes* translate_ExtDef(treenode*);
InterCodes* translate_FunDec(treenode*);
InterCodes* translate_VarList(treenode*);
InterCodes* translate_CompSt(treenode*);
InterCodes* translate_DefList(treenode*);
InterCodes* translate_DefList(treenode*);
InterCodes* translate_StmtList(treenode*);
InterCodes* translate_Stmt(treenode*);
InterCodes* translate_Cond(treenode*,Operand*,Operand*);
InterCodes* translate_Exp(treenode*,Operand*);
InterCodes* translate_Args(treenode*,Operand**,int*);
#endif
