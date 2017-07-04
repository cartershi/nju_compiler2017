#include "assembly.h"

void writeback(Operand* op){
    if (op->kind==ADDRESS){
        printf("lw $s1, -%d($fp)\n",op->offset);            
        printf("sw $s0, 0($s1)\n");
    }
    else
        printf("sw $s0, -%d($fp)\n",op->offset);
}

void getreg(Operand* op,int regnow){
    regalloc[regnow]=0;
    switch (op->kind){
        case VARIABLE:
            {
                regnum++;
                printf("lw $s%d, -%d($fp)\n",regnum,op->offset);
                regalloc[regnow]=regnum;
                break;
            }
        case CONSTANT:
            {
                regnum++;
                regalloc[regnow]=regnum;
                printf("li $s%d, %d\n",regnum,op->u.value);
                    break;
            }
        case ADDRESS:
            {
                regnum++;
                printf("lw $s%d, -%d($fp)\n",regnum,op->offset);            
                regnum++;
                printf("lw $s%d, 0($s%d)\n",regnum,regnum-1);
                regalloc[regnow]=regnum;
                break;
            }
        case INTEGER:
            {
                break;
            }
        case LABEL:
            {
                break;
            }
        case ID_STR:
            {
                regnum++;
                printf("lw $s%d, -%d($fp)\n",regnum,op->offset);
                regalloc[regnow]=regnum;
                break;
            }
        case ARRAY_ID:
            {
                regnum++;
                printf("la $s%d, -%d($fp)\n",regnum,op->offset);
                regalloc[regnow]=regnum;
                break;
            }
        default: break;
    }
}
void assembly_traversal(InterCodes* codelist){
    //return;
    printf(".data\n");
    printf("_prompt: .asciiz \"Enter an integer:\"\n");
    printf("_ret: .asciiz \"\\n\"\n");
    printf(".globl main\n");
    printf(".text\n");
    int len;
    regnum=0;
    int argnum=0;
    InterCodes* code=codelist;
    do{
        switch (code->code.kind){
            case ASSIGN:
                {
                    if (code->code.u.assign.left!=NULL){
                        getreg(code->code.u.assign.right,1);
                        printf("move $s0, $s%d\n",regalloc[1]);
                        writeback(code->code.u.assign.left);
                    }
                    break;
                }
            case ADD:
                {
                    getreg(code->code.u.binop.op1,1);
                    getreg(code->code.u.binop.op2,2);
                    printf("add $s0, $s%d, $s%d\n",regalloc[1],regalloc[2]);
                    writeback(code->code.u.binop.result);
                    break;
                }
            case SUB:
                {
                    getreg(code->code.u.binop.op1,1);
                    getreg(code->code.u.binop.op2,2);
                    printf("sub $s0, $s%d, $s%d\n",regalloc[1],regalloc[2]);
                    writeback(code->code.u.binop.result);
                    break;
                }
            case MUL:
                {
                    getreg(code->code.u.binop.op1,1);
                    getreg(code->code.u.binop.op2,2);
                    printf("mul $s0, $s%d, $s%d\n",regalloc[1],regalloc[2]);
                    writeback(code->code.u.binop.result);
                    break;
                }
            case DIV_:
                {
                    getreg(code->code.u.binop.op1,1);
                    getreg(code->code.u.binop.op2,2);
                    printf("div $s%d, $s%d\n",regalloc[1],regalloc[2]);
                    printf("mflo $s0\n");
                    writeback(code->code.u.binop.result);
                    break;
                }
            case RETURN_:
                {
                    getreg(code->code.u.sigop.result,1);
                    printf("move $v0, $s%d\n",regalloc[1]);
                    printf("lw $ra, -4($fp)\n");
                    printf("lw $fp, -8($fp)\n");
                    printf("addi $sp, $sp, %d\n",len);
                    printf("jr $ra\n");
                    break;
                }
            case JUMP:
                {
                    getreg(code->code.u.jmp.op1,1);
                    getreg(code->code.u.jmp.op3,2);
                    switch (code->code.u.jmp.op2->u.op_num){
                        case GT: { 
                                printf("bgt $s%d, $s%d, label%d\n",regalloc[1],regalloc[2],
                                        code->code.u.jmp.goal->u.lab_no);
                                break;
                            }
                        case LT: {
                                printf("blt $s%d, $s%d, label%d\n",regalloc[1],regalloc[2],
                                        code->code.u.jmp.goal->u.lab_no);
                                break;
                            }
                        case EQ: {
                                printf("beq $s%d, $s%d, label%d\n",regalloc[1],regalloc[2],
                                        code->code.u.jmp.goal->u.lab_no);
                                break;
                            }
                        case GE: {
                                printf("bge $s%d, $s%d, label%d\n",regalloc[1],regalloc[2],
                                        code->code.u.jmp.goal->u.lab_no);
                                break;
                            }
                        case LE: {
                                printf("ble $s%d, $s%d, label%d\n",regalloc[1],regalloc[2],
                                        code->code.u.jmp.goal->u.lab_no);
                                break;
                            }
                        case NE: {
                                printf("bne $s%d, $s%d, label%d\n",regalloc[1],regalloc[2],
                                        code->code.u.jmp.goal->u.lab_no);
                                break;
                            }
                        default: break;
                    }
                    break;
                }
            case LABEL_CODE:
                {
                    printf("label%d:\n",code->code.u.sigop.result->u.lab_no);
                    break;
                }
            case LABEL_GOTO:
                {
                    printf("j label%d\n",code->code.u.sigop.result->u.lab_no);
                    break;
                }
            case READ:
                {
                    printf("li $v0, 4\n");
                    printf("la $a0, _prompt\n");
                    printf("syscall\n");
                    printf("li $v0, 5\n");
                    printf("syscall\n");
                    printf("move $s0, $v0\n");
                    writeback(code->code.u.sigop.result);
                    break;
                }
            case WRITE:
                {
                    getreg(code->code.u.sigop.result,1);
                    printf("li $v0, 1\n");
                    printf("move $a0, $s%d\n",regalloc[1]);
                    printf("syscall\n");
                    printf("li $v0, 4\n");
                    printf("la $a0, _ret\n");
                    printf("syscall\n");
                    break;
                }
            case CALLFUNC:
                {
                    argnum=0;
                    printf("jal %s\n",code->code.u.assign.right->u.name);
                    printf("move $s0, $v0\n");
                    writeback(code->code.u.assign.left);
                    break;
                }
            case FUNCTION:
                {
                    len=code->code.u.sigop.result->offset;
                    printf("\n%s:\n",code->code.u.sigop.result->u.name);
                    //if (!strcmp(code->code.u.sigop.result->u.name,"main"))
                      //      printf("li $sp, 0x7fffff00\n");
                    printf("subu $sp, $sp, %d\n",len);
                    printf("sw $ra, %d($sp)\n",len-4);
                    printf("sw $fp, %d($sp)\n",len-8);
                    printf("addi $fp, $sp, %d\n",len);
                    funcnode* goalnode=hash_funcfind(code->code.u.sigop.result->u.name);
                    if (goalnode!=NULL)
                    for (int i=0; i<=goalnode->par_num; i++)
                        printf("sw $a%d, -%d($fp)\n",i,4*(goalnode->par_num-i+1)+8);
                    break;
                }
            case ARG:
                {
                    getreg(code->code.u.sigop.result,1);
                    printf("move $a%d, $s%d\n",argnum,regalloc[1]);
                    argnum++;
                    break;
                }
            case PARAM:
                {
                    /*printf("PARAM ");
                    getreg(code->code.u.sigop.result,1);
                    printf("\n");*/
                    break;
                }
            case DEC:
                {
                    /*printf("DEC ");
                      getreg(code->code.u.assign.left);
                      printf(" ");
                      getreg(code->code.u.assign.right);
                      printf("\n");*/
                    break;
                }
            case REFASSIGN:
                {
                    int flag=0;
                    
                    if (code->code.u.assign.right->kind==ADDRESS){
                        code->code.u.assign.right->kind=VARIABLE;
                        flag=1;
                    }
                    getreg(code->code.u.assign.right,1);
                    if (flag==1){
                        code->code.u.assign.right->kind=ADDRESS;
                        flag=0;
                    }
                    printf("move $s0, $s%d\n",regalloc[1]);
                    if (code->code.u.assign.left->kind==ADDRESS){
                        code->code.u.assign.left->kind=VARIABLE;
                        flag=1;
                    }
                    writeback(code->code.u.assign.left);
                    if (flag==1){
                        code->code.u.assign.left->kind=ADDRESS;
                        flag=0;
                    }
                    break;
                }
            default: break;
        }
        code=code->next;
        regnum=0;
    }
    while(code!=codelist);
}
