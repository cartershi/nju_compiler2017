#include "common.h"
#include "lex.yy.c"

int main(int argc,char** argv){
    if (argc>1){
        if (!(yyin=fopen(argv[1],"r"))){
            perror(argv[1]);
            return 1;
        }
    }
    hash_init();
    operand_init();
    yyparse();
    return 0;
}
