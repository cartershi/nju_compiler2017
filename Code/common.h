#ifndef _COMPILERS_CODE_COMMON
#define _COMPILERS_CODE_COMMON

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define DEBUG 1

typedef struct YYLTYPE  {  
    int first_line;  
    int first_column;  
    int last_line;  
    int last_column;  
} YYLTYPE;

typedef unsigned int uint32_t;
#ifndef YYLTYPE_IS_DECLARED
#define YYLTYPE_IS_DECLARED 1
#endif
#endif
