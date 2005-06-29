/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/* functionHandler.c - Routines to handle function calls.
 */
 
#define SRB_SERVER
#include "functionHandler.h"
#undef SRB_SERVER

/*
 * sendFunctionResult - Send the result of the function call to the client.
 *
 * Input : FunctCall *myFunctCall - The current FunctCall.
 *         char *retval - The returned value of the function
 *
 * Output : None
 */

void
sendFunctionResult(svrComm_t *myComm, FunctCall *myFunctCall, char *retval)
{
    int authSchemeInx;

    if (myFunctCall->retlen > 0) {
        if ((int) retval == DONT_SEND_RETURN) {
            return;
	} else if ((int) retval == EXIT_WHEN_DONE) {
	    exitSrbServer (INIT_DONE);
	}
    } else {
        int myReturn;
	int *tmpPtr;

	tmpPtr = (int*) VARDATA(retval);
	myReturn = ntohl (*tmpPtr);
	if (myReturn == DONT_SEND_RETURN) 
	    return;
    }
    authSchemeInx = svrGetAuthScheme (CLIENT_MODE);
    commPutnchar(myComm, "V", 1, authSchemeInx);
 
    if (myFunctCall->retlen != 0) {
        commPutnchar(myComm, "G", 1, authSchemeInx);
        if (myFunctCall->retlen == SIZEOF32) {         /* by value - 4 bytes */
	    if ((int) retval == DONT_SEND_RETURN)
		return;
#ifdef ADDR_64BIT
            srb_long_t tmpLong;
            int tmpInt;
            tmpLong = (srb_long_t) retval;
            tmpInt = tmpLong;
            commPutint(myComm, myFunctCall->retlen, 4, authSchemeInx);
            commPutint(myComm, tmpInt, myFunctCall->retlen, authSchemeInx);
#else
            commPutint(myComm, myFunctCall->retlen, 4, authSchemeInx);
            commPutint(myComm, (int)retval, myFunctCall->retlen, authSchemeInx);
#endif
        } else if (myFunctCall->retlen == 2*SIZEOF32) { /* by value - 8 bytes */
            commPutint(myComm, myFunctCall->retlen, 4, authSchemeInx);
            commPutlong(myComm, (srb_long_t *)retval, myFunctCall->retlen, 
	     authSchemeInx);
        } else if (myFunctCall->retlen > 0) { /* by value - 8 bytes */
            commPutint(myComm, myFunctCall->retlen, 4, authSchemeInx);
            commPutnchar(myComm, retval, myFunctCall->retlen, authSchemeInx);
        } else {                /* by-reference ... */
            commPutint(myComm, VARSIZE(retval) - VARHDRSZ, 4, authSchemeInx);
            commPutnchar(myComm, VARDATA(retval), VARSIZE(retval) - VARHDRSZ,
	     authSchemeInx);
        }
    }
 
    /* seems to be needed with conversion to socket i/o (10/97)*/
    commFlush(myComm, authSchemeInx); 

    commPutnchar(myComm, "0", 1, authSchemeInx);
    commFlush(myComm, authSchemeInx);
}

int 
sendByteStream (svrComm_t *myComm, char *retval)
{
    int authSchemeInx;

    authSchemeInx = svrGetAuthScheme (CLIENT_MODE);
    commPutnchar(myComm, "V", 1, authSchemeInx);
    commPutnchar(myComm, "G", 1, authSchemeInx);
    commPutint(myComm, VARSIZE(retval) - VARHDRSZ, 4, authSchemeInx);
    commPutnchar(myComm, VARDATA(retval), VARSIZE(retval) - VARHDRSZ, 
     authSchemeInx);

    /* seems to be needed with conversion to socket i/o (10/97)*/
    commFlush(myComm, authSchemeInx);

    commPutnchar(myComm, "0", 1, authSchemeInx);
    commFlush(myComm, authSchemeInx);
    return (0);
}


int
sendErrorRet (svrComm_t *myComm, FunctCall *myFunctCall, int errorNo)
{
    int authSchemeInx;

    authSchemeInx = svrGetAuthScheme (CLIENT_MODE);
    commPutnchar(myComm, "V", 1, authSchemeInx);

    if (myFunctCall == NULL) {
	/* assume retlen = 4 bytes */
	commPutint(myComm, SIZEOF32, 4, authSchemeInx);
        commPutint(myComm, (int)errorNo, SIZEOF32, authSchemeInx);
    } else if (myFunctCall->retlen != 0) {
        commPutnchar(myComm, "G", 1, authSchemeInx);
        if (myFunctCall->retlen == SIZEOF32) {         /* by value - 4 bytes */
            commPutint(myComm, myFunctCall->retlen, 4, authSchemeInx);
            commPutint(myComm, (int)errorNo, myFunctCall->retlen, 
             authSchemeInx);
        } else if (myFunctCall->retlen == 2*SIZEOF32) { /* by value - 8 bytes */
	    srb_long_t myLong;
	    myLong = errorNo;
            commPutint(myComm, myFunctCall->retlen, 4, authSchemeInx);
            commPutlong(myComm, (srb_long_t *)&myLong, myFunctCall->retlen,
             authSchemeInx);
        } else if (myFunctCall->retlen > 0) { /* by value - 8 bytes */
            commPutint(myComm, myFunctCall->retlen, 4, authSchemeInx);
            commPutnchar(myComm, (char *) &errorNo, myFunctCall->retlen, 
             authSchemeInx);
        } else {                /* by-reference ... */
	    errorNo = htonl (errorNo);
            commPutint(myComm, SIZEOF32, SIZEOF32, authSchemeInx);
            commPutnchar(myComm, (char *) &errorNo, SIZEOF32, authSchemeInx);
        }
    }

#ifdef foo
    /* compose a error msg */

    commPutnchar("V", 1, authSchemeInx);
    commPutnchar("G", 1, authSchemeInx);
    commPutint(SIZEOF32, SIZEOF32, authSchemeInx);
    commPutint(errorNo, SIZEOF32, authSchemeInx);
#endif

    commFlush(myComm, authSchemeInx); 

    commPutnchar(myComm, "0", 1, authSchemeInx);
    commFlush(myComm, authSchemeInx);
    return (0);
}

/*
 * handleFunctionRequest - Handle function calls from the client
 *
 * This corresponds to the protocol symbol "F".
 *
 * Input : None (read input from the input stream)
 *
 * Output : Returns  0 - success, 1 - failure.
 */
int
handleFunctionRequest(svrComm_t *myComm)
{
    int         fid;
    int         argsize;
    int         nargs;
    char        *arg[12];
    char        *retval;
    int                 i;
    UINT32              malloced;
    char                *p;
    int authSchemeInx;

    authSchemeInx = svrGetAuthScheme (CLIENT_MODE);
 
    fid = commGetint(myComm, 4, authSchemeInx);   /* function id */
    nargs = commGetint(myComm, 4, authSchemeInx);       /* # of arguments */
 
    curFunctCall = functionLookup(fid);
    if (curFunctCall == NULL || curFunctCall->func == NULL) {
	elog(NOTICE, 
	"handleFunctionRequest: Function id (%d) is not supported",
	fid);
	sendErrorRet (myComm, curFunctCall, CLI_FUNC_NOT_SUPPORTED);
	commFlushInp (myComm, authSchemeInx);
	return (1);
    }
 
    if (curFunctCall->nargs != nargs) {
        elog(NOTICE, 
	"handleFunctionRequest: call %d: received nargs (%d) != registered nargs (%d)",
             curFunctCall->funcid, nargs, curFunctCall->nargs);
	sendErrorRet (myComm, curFunctCall, CLI_NUM_ARG_MISMATCH);
	commFlushInp (myComm, authSchemeInx);
	return (1);
    }

 
    /*
     *  Copy arguments into arg vector. 
     */
    malloced = 0x0;
    for (i = 0; i < 12; ++i) {
        if (i >= nargs) {
            arg[i] = (char *) NULL;
        } else {
            argsize = commGetint(myComm, 4, authSchemeInx);
 
            Assert(argsize > 0);
            if (curFunctCall->arglen[i] == SIZEOF32) {     /* 32 bit int */
#ifdef ADDR_64BIT
                srb_long_t tmpLong;
                tmpLong = (srb_long_t) commGetint(myComm, argsize, 
		 authSchemeInx);
                arg[i] = (char *) tmpLong;
#else
                arg[i] = (char *) commGetint(myComm, argsize, authSchemeInx);
#endif
	    } else if (curFunctCall->arglen[i] == 2*SIZEOF32) { /* 64 bit */
                arg[i] = (char *) commGetlong(myComm, argsize, authSchemeInx);
		if (arg[i] == NULL) {
                    elog(WARN, "handleFunctionRequest: commGetlong failed");
                    return (1);
		}
		malloced |= (1 << i);
            } else {                    /* by-reference ... */
                if (!(p = malloc(argsize + VARHDRSZ))) {
                    elog(WARN, "handleFunctionRequest: malloc failed");
		    return (1);
		}
                VARSIZE(p) = argsize + VARHDRSZ;
                commGetnchar(myComm, VARDATA(p), 0, argsize, authSchemeInx);
                malloced |= (1 << i);
                arg[i] = p;
            }
        }
    }
 
    if (chkVersion == 0) {
	char *cliVersion, dummy1[STRING_LEN1], dummy2[STRING_LEN1],
	 cliAPIVersion[STRING_LEN1], svrAPIVersion[STRING_LEN1];

	if ((cliVersion = getenv("srbVersion")) != NULL) {
	    mySplitStr (cliVersion, dummy1, cliAPIVersion, '&');
	    mySplitStr (SRB_VERSION, dummy2, svrAPIVersion, '&');
	    if (strcmp (dummy1, dummy2) != 0 &&
	     strcmp (svrAPIVersion, cliAPIVersion) != 0 &&
	      fid != F_GET_SVR_VER) {
		elog (NOTICE, 
		 "Incompatible version. Client : %s, Server : %s\n", 
		  cliVersion, SRB_VERSION);
		sendErrorRet (myComm, curFunctCall, CLI_INCOMPATIBLE_VERSION);
		/* free malloc'ed arguments */
    		for (i = 0; i < nargs; ++i) {
        	if (malloced & (1 << i))
            	    free(arg[i]);
    		}
		return (1);
	    }
	} 
	if (fid != F_GET_SVR_VER) 
	    chkVersion = 1;
    }

    retval = functionRouter(myComm, curFunctCall,
                  arg[0], arg[1], arg[2], arg[3],
                  arg[4], arg[5], arg[6], arg[7],
                  arg[8], arg[9], arg[10], arg[11]);
 
    /* free malloc'ed arguments */
    for (i = 0; i < nargs; ++i) {
        if (malloced & (1 << i))
            free(arg[i]);
    }
 
    sendFunctionResult(myComm, curFunctCall, retval);
 
    if (curFunctCall->retlen < 0 || curFunctCall->retlen == 2*SIZEOF32)
        free(retval);
 
    return(0);
}

/*
 * functionRouter - Route the request to the compiled in function. 
 * Do not yet handle User defined functions (UDF).
 *
 * Input : FunctCall *myFunctCall - pointer to the FunctCall that handles
 *	   the request.
 *	   arg[0], arg[1], arg[2], ... - The arguments of the function
 *	   
 * Output :
 *      Returns the return value of the invoked function if succesful,
 *      0 if unsuccessful.
 */
char *
functionRouter(svrComm_t *myComm, FunctCall *myFunctCall, ... )
{
    va_list             pvar;
    int register    i;
    FunctValues  values;
 
    va_start(pvar, myFunctCall);
 
    for (i = 0; i < myFunctCall->nargs; ++i)
        values.data[i] = va_arg(pvar, char *);
    va_end(pvar);
 
    return(_functionRouter(myComm, myFunctCall->func, myFunctCall->funcid, 
	myFunctCall->nargs, &values));
}

/* _functionRouter - The internal version of functionRouter.
 * 
 * Input : func_ptr user_fn - The function entry point.
 *	   int func_id - The function id.
 *	   int n_arguments - number of arguments
 *	   FunctValues *values - The input parameters
 * Output :
 *      Returns the return value of the invoked function if succesful,
 *      0 if unsuccessful.
 */

char *
_functionRouter(svrComm_t *myComm, func_ptr user_fn, int func_id, 
int n_arguments, FunctValues *values)
{
    char        *returnValue = (char *) NULL;
 
 
    if (user_fn == (func_ptr) NULL) {
	int *intRet;

        elog(WARN, "SRB system error: NULL function not supported.");
	intRet = malloc (sizeof (int));
	*intRet = SYS_ERR_NO_SUPPORT;
	return ((char *) intRet);
    }
 
    switch (n_arguments) {
    case 0:
        returnValue = (*user_fn)();
        break;
    case 1:
        returnValue = (*user_fn)(values->data[0], myComm);
        break;
    case 2:
        returnValue = (*user_fn)(values->data[0], values->data[1], myComm);
        break;
    case 3:
        returnValue = (*user_fn)(values->data[0], values->data[1],
                                 values->data[2], myComm);
        break;
    case 4:
        returnValue = (*user_fn)(values->data[0], values->data[1],
                                 values->data[2], values->data[3], myComm);
        break;
    case 5:
        returnValue = (*user_fn)(values->data[0], values->data[1],
                                 values->data[2], values->data[3],
                                 values->data[4], myComm);
        break;
    case 6:
        returnValue = (*user_fn)(values->data[0], values->data[1],
                                 values->data[2], values->data[3],
                                 values->data[4], values->data[5], myComm);
        break;
    case 7:
        returnValue = (*user_fn)(values->data[0], values->data[1],
                                 values->data[2], values->data[3],
                                 values->data[4], values->data[5],
                                 values->data[6], myComm);
        break;
    case 8:
        returnValue = (*user_fn)(values->data[0], values->data[1],
                                 values->data[2], values->data[3],
                                 values->data[4], values->data[5],
                                 values->data[6], values->data[7], myComm);
        break;
    case 9:
        /*
         */
        returnValue = (*user_fn)(values->data[0], values->data[1],
                                 values->data[2], values->data[3],
                                 values->data[4], values->data[5],
                                 values->data[6], values->data[7],
                                 values->data[8], myComm);
        break;
    case 10:
        /*
         */
        returnValue = (*user_fn)(values->data[0], values->data[1],
                                 values->data[2], values->data[3],
                                 values->data[4], values->data[5],
                                 values->data[6], values->data[7],
                                 values->data[8], values->data[9], myComm);
        break;
    case 11:
        /*
         */
        returnValue = (*user_fn)(values->data[0], values->data[1],
                                 values->data[2], values->data[3],
                                 values->data[4], values->data[5],
                                 values->data[6], values->data[7],
                                 values->data[8], values->data[9],
				 values->data[10], myComm);
        break;
    case 12:
        /*
         */
        returnValue = (*user_fn)(values->data[0], values->data[1],
                                 values->data[2], values->data[3],
                                 values->data[4], values->data[5],
                                 values->data[6], values->data[7],
                                 values->data[8], values->data[9],
				 values->data[10], values->data[11], myComm);
        break;
    default:
        elog(WARN, "_functionRouter: function %d: too many arguments (%d > %d)",
             func_id, n_arguments, MAXFMGRARGS);
        break;
    }
    return(returnValue);
}

/* functionLookup - Using the input function ID, lookup a function in the
 * functBuiltin[] table.
 *
 * Input : int funcid - The function ID to lookup
 *
 * Output : A pointer to the FunctCall struct with matching funcid.
 */

FunctCall *functionLookup(int funcid)
{
    int i;
    for (i=0;i<nBuiltinFunct;i++) {
        if (functBuiltin[i].funcid == funcid)
                return(&functBuiltin[i]);
    }
    return(NULL);
}

