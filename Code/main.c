#include "common.h"
#include "tree.h"
#include "lex.yy.c"
extern treenode *treeroot;
extern int errorrec;
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
    if (errorrec==0){
#if DEBUG
        printf("semantic_begin\n");
#endif
        semantic_traversal(treeroot);
#if DEBUG
        printf("semantic_end\n");
#endif
        if (argc>2) freopen(argv[2],"w",stdout);
        operand_traversal(treeroot);
    }
    return 0;
}
