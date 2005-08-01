/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * packMsgExtern.h
 */

#ifndef PACK_MSG_EXTERN_H
#define PACK_MSG_EXTERN_H

#include "srb.h"

/* This is the buf size for the sfo type proxy operation - may not be
enough. May need a way to calculate how much it needs */

#define RESULT_BUF_SZ   4194304

#define MAX_ARRAY_DIM	9	/* max array dimension handled */ 
#define NULL_TERM       -1      /* null terminated */

/* The name and value the marks the end of the FormatDefConst table */
#define END_DEF_NAME    "THE_END_OF_DEF"
#define END_DEF_VAL 999999     

typedef char* FormatDef;        /* struct definition string */
typedef char* IntDef;           /* struct definition string */

/* Type Table - data types recognized by the packMsg routines */

typedef enum {
    SRB_CHAR_TYPE,
    STR_TYPE,
    SRB_INT_TYPE,
    DOUBLE_TYPE,
    STRUCT_TYPE
} TypeInx;

typedef struct {
    char *typeKW;	/* the type keyword */
    TypeInx  index;	/* an index value */
    int size;		/* size in bytes of the type */		
} packTypes;


/* Define some global constant. An entry to GlobalDef[] should be made if 
 * an FormatDef or a constant want to be seen by another FormatDef */

typedef struct {
    char *name;
    char *value;
} FormatDefConst;

extern int
packMsg (char *inStr, struct varlena **outStr, FormatDef packFormat,
FormatDefConst *localDef);
extern int unpackMsg (char *inStr, char **outStr, FormatDef packFormat,
FormatDefConst *localDef);



#endif 	/* PACK_MSG_EXTERN_H */
