/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* functionHandlerExtern.h - Definition and prototypes for functionHandler.c
 */

#ifndef FUNCTION_HANDLER_EXTERN_H
#define FUNCTION_HANDLER_EXTERN_H

/*
 *      Maximum number of arguments for a built-in function.
 */
#define MAXFMGRARGS     12

typedef struct {
    char *data[MAXFMGRARGS];
} FunctValues;

/* ptr to func returning (char *) */
typedef char *  ((*func_ptr)());

typedef struct {
    int         funcid;
    func_ptr    func;
    char*       funcName;
    int         retlen;         /* length of ret value in byte, -1 -> var len */
    int         nargs;
    int         arglen[MAXFMGRARGS];
} FunctCall;

extern int handleFunctionRequest(svrComm_t *myComm);
extern int
sendByteStream (svrComm_t *myComm, char *retval);
extern int
sendErrorRet (svrComm_t *myComm, FunctCall *myFunctCall, int errorNo);
void sendFunctionResult(svrComm_t *myComm, FunctCall *myFunctCall,
char *retval);



#endif	/* FUNCTION_HANDLER_EXTERN_H */
