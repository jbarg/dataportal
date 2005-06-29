/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * packMsg.h
 */

#ifndef PACK_MSG_H
#define PACK_MSG_H

#include "packMsgExtern.h"
#include "packDef.h"
#include "packStruct.h"

#define PACKED_MSG_LEN	1024	/* alloc in 1 k chunks */
#define MAX_INC_MSG_LEN	65536	/* max chunk increment size */
#define CNTL_A		1	/* cntl A char */

struct parsedType {
    TypeInx typeInx;		/* The type */
    char *name;
    char *pointer;		/* A pointer or value. 0 = value, 1 = pointer
				   - eventaully set to the addr of the array */
    int value;			/* INT_TYPE - value, no meaning for other */
    int *dim;			/* The dimension of this type.
				 * dim[0] = dimension of this array.
				 * dim[1] ... = length in each dim. 
				 * if dim == NULL ==> 0 dim */
    struct parsedType *parent;	/* struct parent within a struct */
    struct parsedType *next;	/* sibling - link list to next entry */
    struct parsedType *prev;	/* point back to the previous sibling node */
};
    
typedef struct {
    struct varlena *msg;
    int maxLen;
} packedMsg;

typedef struct {
    char *msg;
    int len;
    int maxLen;
} unpackedMsg;

packTypes packTypeEntries[] = {
        {"char", CHAR_TYPE, sizeof (char)},
        {"str", STR_TYPE, sizeof (char)},
        {"int", INT_TYPE, SIZEOF32},
        {"double", DOUBLE_TYPE, 8},
        {"struct", STRUCT_TYPE, -1},
};

static int nPackMsgTypes =
    (sizeof(packTypeEntries) / sizeof (packTypes));

extern int
parsePackFormat (FormatDef packFormat, struct parsedType **childHead);
extern TypeInx lookupPackType (char *typeKW);
extern struct parsedType* allocParsedType ();
extern int packParsedType (struct parsedType *myType, char **inDataPtr,
packedMsg *outMsg, FormatDefConst *localDef);
extern int unpackParsedType (struct parsedType *myType, char **inDataPtr, 
unpackedMsg *outMsg, FormatDefConst *localDef);
#ifdef ADDR_64BIT
srb_long_t
#else
int
#endif
relsoveFormatDef (char *intName, struct parsedType *myType,
FormatDefConst *localDef);
extern int freeParsedType (struct parsedType *head);
extern int extendPackedMsg (packedMsg *outMsg, int minLen);
extern int extendUnpackedMsg (unpackedMsg *myMsg, int minLen);
extern char *intAlign (char *ptr);
extern char *doubleAlign (char *ptr);
extern int getNumInArray (int *dim);
extern int parseArrayInStruct (struct parsedType *myType, 
FormatDefConst *localDef);
extern int addPointToUnpackMsg (struct parsedType *myType, 
unpackedMsg *outMsg, int size);
extern int initPackedMsg (packedMsg *outMsg);
extern int initUnpackedMsg (unpackedMsg *outMsg, int maxLen);

#endif 	/* PACK_MSG_H */
