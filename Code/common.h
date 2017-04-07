#ifndef _COMPILERS_CODE_COMMON
#define _COMPILERS_CODE_COMMON

#include "tree.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct YYLTYPE  {  
    int first_line;  
    int first_column;  
    int last_line;  
    int last_column;  
} YYLTYPE;

#ifndef YYLTYPE_IS_DECLARED
#define YYLTYPE_IS_DECLARED 1
#endif
#endif
