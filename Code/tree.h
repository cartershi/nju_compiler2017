#ifndef _COMPILERS_CODE_TREE
#define _COMPILERS_CODE_TREE

typedef struct treenode {
    char* name;
    int line;
    struct treenode* child;
    struct treenode* sibling;
    union{
        char* character;
        int type_int;
        float type_float;
    };
}treenode;
void addleaf(char *name);
treenode* addnode(char* name,int child_num,...);
void traversal(treenode* root,int indent);
#endif
