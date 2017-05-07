#ifndef _COMPILERS_CODE_TREE
#define _COMPILERS_CODE_TREE

typedef struct treenode {
    char* name;
    int line;
    struct treenode* child;
    struct treenode* sibling;
    union{
        char character[32];
        int type_int;
        float type_float;
    };
    char rawtext[32];
    struct senode_struct* info;
    struct funcnode_struct* funcinfo;
    int returnline;
    enum {EINT=0,EFLOAT,EMISS,EAARAY,ESTRUCT}exp_type;   //record the type of exp
    union{
        int exp_int;
        float exp_float;
    }exp_rec;
    //int depth;
}treenode;

typedef struct funcnode_struct{
    char* name;
    int kind;//int=0 float=1
    int par_num;
    struct senode_struct* para[20]; 
    struct funcnode_struct* next;
}funcnode;

typedef struct senode_struct{
    char* name;
    struct setype_struct* type;
    char* struct_name;
    struct senode_struct* next;
}senode;

typedef struct setype_struct{
    enum { BASIC,ARRAY, STRUCTURE } kind;
    union{
        int basic; //int=0 float=1
        struct {struct setype_struct* elem; int size; } array;
        struct fieldlist_struct* structure;
    };
}setype;

typedef struct fieldlist_struct{
    char* name; // 域的名字
    struct setype_struct* type; // 域的类型
    struct fieldlist_struct* tail; // 下一个域
}fieldlist;

senode hashtable[16384];
funcnode hashfunctable[16384];
void addleaf(char *name);
treenode* addnode(char* name,int child_num,...);
void traversal(treenode* root,int indent);
void semantic_traversal(treenode*);
void hash_init();
#endif
