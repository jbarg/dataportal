/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* clExecExtern.h - 
 */

#ifndef CL_EXEC_EXTERN_H
#define CL_EXEC_EXTERN_H

#include "clConnectExtern.h"

/* ----------------
 * clArgBlock --
 *      Argument structure required for the clFunct() call.
 * ----------------
 */
typedef struct {
    int len;
    int isint;
    union {
        char *ptr;       /* can't use void (dec compiler chock), can't
			 * use int, c90 chock  */
        int integer;
    } u;
} clArgBlock;

extern int clResultStatus(srbResult* res);
extern void clClear(srbResult* res);
extern srbResult* clFunct(srbConn* conn,
                      int fnid,
                      char *resultBuf,
                      int *resultLen,
                      int resultIsInt,
                      clArgBlock *args,
                      int nargs);

extern int
getByteStreamFromSvr (srbConn *conn, char **resultBuf, int *actualResultLen);
extern srbResult*
clReadServerReturn (srbConn *conn, char *resultBuf,
int *actualResultLen, int resultIsInt);
#endif	/* CL_EXEC_EXTERN_H */
