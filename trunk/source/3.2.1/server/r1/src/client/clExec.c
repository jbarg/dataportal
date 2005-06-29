/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/*-------------------------------------------------------------------------
 *
 * clExec.c-- Client routines for sending Function request and receiving results
 * to/from the SRB.
 *-------------------------------------------------------------------------
 */

#include <clExec.h>

/*
 * clClear -
 *    free's the memory associated with a srbResult.
 * Input - srbResult* res - the srbResult to be freed.
 * Output - None.
 *
 */
void
clClear(srbResult* res)
{
    if (!res)
	return;

    free(res);
}

/*
 * makeEmptysrbResult -
 * Input - srbConn *conn
 *	   int status
 * Output - returns a newly allocated, initialized srbResult
 *
 */

static srbResult*
makeEmptysrbResult(srbConn *conn, int status)
{
    srbResult *result;

    result = (srbResult*)malloc(sizeof(srbResult));

    result->conn = conn;
    result->resultStatus = status;
    return result;
}

/* ----------------
 *	clFunct -  Send a function call to the srbServer.
 *
 * Input -
 *      conn            : SRB connection
 *	fnid	       	: Function id
 * 	resultBuf      	: Pointer to result buffer (&int if integer)
 * 	resultLen	: Length of return value.
 *      actualResultLen	: Actual length returned. (differs from resultLen
 *			  for varlena structures.)
 *      resultIsInt   	: If the result is an integer, 1 - yes, 0 - no
 * 	args		: Pointer to a NULL terminated arg array.
 * 	nargs		: No. of arguments in args .
 *
 * Output -  returns NULL on failure.  clErrorMsg will be set.
 */

srbResult*
clFunct(srbConn *conn,
        int fnid,
        char *resultBuf,
        int *actualResultLen,
        int resultIsInt,
        clArgBlock *args,
        int nargs)
{
    int Pfin = conn->sock;
    int Pfout = conn->sock;

    int id;
    int i;
    char *errMsgPtr;
    int status;
#if defined(PORTNAME_c90)
    int tmpInt;
#endif
    srbResult *mySrbResult;


    /* clear the error string */

    conn->errorMessage[0] = '\0';

    clPuts("F ",Pfout, conn->commBuf, conn->authSchemeInx);    /* function */
    /* function id */
    clPutInt(fnid, 4, Pfout, conn->commBuf, conn->authSchemeInx); 
    /* No. of args */
    clPutInt(nargs, 4, Pfout, conn->commBuf, conn->authSchemeInx);	

    for (i = 0; i < nargs; ++i) { /*	len.int4 + contents	*/
	clPutInt(args[i].len, 4, Pfout, conn->commBuf, conn->authSchemeInx);
	if (args[i].isint) {
	    if (args[i].len == 2*SIZEOF32) {
	    	clPutLong ((srb_long_t *) args[i].u.ptr, 2*SIZEOF32, Pfout, 
		 conn->commBuf, conn->authSchemeInx);
	    } else {
	    	clPutInt((int) args[i].u.integer, 4, Pfout, conn->commBuf,
	     	 conn->authSchemeInx);
	    }
	} else {
	    clPutnchar((char *)args[i].u.ptr, args[i].len, Pfout, 
	     conn->commBuf, conn->authSchemeInx);
	}
    }
    clFlush(Pfout, conn->commBuf, conn->authSchemeInx);

    mySrbResult = clReadServerReturn (conn, resultBuf, actualResultLen,
     resultIsInt);

    return (mySrbResult);
#ifdef foo
    errMsgPtr = conn->errorMessage;
    while (1) {
    	id = clGetc(Pfin, conn->commBuf, conn->authSchemeInx);
    	if (id != 'V') {
	    if (id == 'E') {
	    	clGets(errMsgPtr, ERROR_MSG_LENGTH,Pfin, conn->commBuf,
	    	 conn->authSchemeInx);
	        return makeEmptysrbResult(conn,CLI_FATAL_ERROR);
	    } else if (id == 'N') {
	    	clGets(errMsgPtr,ERROR_MSG_LENGTH,Pfin, conn->commBuf, 
	     	 conn->authSchemeInx);
	    	errMsgPtr += strlen (errMsgPtr);
	    	continue;
	   } else if (id == 'A') {
		if (SrbServerMode == 0) {
		    /* client mode */
                    status = svrSendStatus (conn->port->sock, 
                     AUTH_ENCRYPT1_INX);
                    if (status < 0) {
                        fprintf(stderr,
                         "clFunct: Unable to send auth status msg to server\n");
                        return makeEmptysrbResult(conn, 
                         CLIENT_CROSS_ZONE_AUTH_FAILED);
                    }
	            status = clSendEncryptAuth (NULL, conn); 
		} else {
		    if (ClientCertInx >= 0) {
			/* I got the client proxy cert */
            		svrSendStatus (conn->port->sock, GSI_DELEGATE_INX);
            		status = svrSendGsiDelegAuth (ClientCertInx, conn);
		    } else {
		        status = initCliSvrAuthLink (SrbServerClientSock);
		        if (status == AUTH_ENCRYPT1_INX) {
			    svrSendStatus (conn->port->sock, AUTH_ENCRYPT1_INX);
        	            status = svrCliSvrEncryptAuth (conn->port->sock,
         	             SrbServerClientSock);
		        } else {
                            fprintf (stderr,
                             "clFunct: Auth inx %d from client not supported\n",
                             status);
                            if (status >= 0)
                                status = CLIENT_CROSS_ZONE_AUTH_FAILED;
			}
		    }
		}

		/* should get error return from the call */
		continue;
	    } else {
	    	sprintf(errMsgPtr,
	   	 "clFunct: Broken Conn- Possible cause : SRB internal error\n");
	    	return makeEmptysrbResult(conn,CLI_ERR_BROKEN_PIPE);
	    }
    	} else {
	    break;
	}
    }

    id = clGetc(Pfin, conn->commBuf, conn->authSchemeInx);
    for (;;) {
	int c;
	switch (id) {
	case 'G':		/* function returned properly */
	    clGetInt(actualResultLen,4,Pfin, conn->commBuf, 
	     conn->authSchemeInx);
	    if (resultIsInt) {
		if (*actualResultLen <= SIZEOF32) {
		    clGetInt((int *) resultBuf, SIZEOF32, Pfin, 
		     conn->commBuf, conn->authSchemeInx);
		    status = CLI_CONNECTION_OK;
		} else if (*actualResultLen == 2*SIZEOF32) {
		    clGetLong ((srb_long_t *) resultBuf,2*SIZEOF32, Pfin, 
		     conn->commBuf, conn->authSchemeInx);
		    status = CLI_CONNECTION_OK;
		} else {
                    /* Drain it */
                    clGetnchar((char *) &status, *actualResultLen + 1, Pfin,
                     conn->commBuf, conn->authSchemeInx);
                    *actualResultLen = 0;
                    return makeEmptysrbResult(conn, CLI_ERR_RETURN_LEN);
		}
	    } else {
		if (*actualResultLen < 4) {
		    /* Drain it */
		    clGetnchar((char *) &status, *actualResultLen + 1, Pfin,
		     conn->commBuf, conn->authSchemeInx);
		    *actualResultLen = 0;
		    return makeEmptysrbResult(conn, CLI_ERR_RETURN_LEN);
	    	}
		/* the first 4 bytes is the status */
#if defined(PORTNAME_c90)
		crGetInt((int *) &status, 4, Pfin, conn->commBuf, 
		 conn->authSchemeInx);
#else
		clGetnchar((char *) &status, 4, Pfin, conn->commBuf, 
		 conn->authSchemeInx);
		status = ntohl (status);
#endif
		*actualResultLen -= 4;
		if (*actualResultLen > 0) {
		    if (resultBuf == NULL) {	/* no buufer */
		    	*actualResultLen = 0;
                    	return makeEmptysrbResult(conn, CLI_ERR_RETURN_LEN);
		    } else {
			clGetnchar((char *) resultBuf, *actualResultLen,
		         Pfin, conn->commBuf, conn->authSchemeInx);
		    }
		}
	    }
	    /* get the last '0'*/
	    c = clGetc(Pfin, conn->commBuf, conn->authSchemeInx); 
	    return makeEmptysrbResult(conn, status);

	case 'E':
	    sprintf(conn->errorMessage,
		"clFunct: returned an error");
	    return makeEmptysrbResult(conn,CLI_FATAL_ERROR);

	case 'N':
	    if (clGets(conn->errorMessage, ERROR_MSG_LENGTH, Pfin, 
	     conn->commBuf, conn->authSchemeInx) == 1) {
		sprintf(conn->errorMessage,
		"Notice return detected, but error message cannot be read");
	    } else
		fprintf(stderr, "%s\n", conn->errorMessage);
	    break;

	case '0':		/* no return value */
	    return makeEmptysrbResult(conn,CLI_COMMAND_OK);

	default:
	    sprintf(conn->errorMessage,
	        "FATAL: clFunct: protocol error: id=%x\n", id);
	    return makeEmptysrbResult(conn,CLI_FATAL_ERROR);
	}
    }
#endif	/* foo */
}

srbResult*
clReadServerReturn (srbConn *conn, char *resultBuf, 
int *actualResultLen, int resultIsInt)
{
    int Pfin = conn->sock;
    int Pfout = conn->sock;

    int id;
    int i;
    char *errMsgPtr;
    int status;
#if defined(PORTNAME_c90)
    int tmpInt;
#endif

    errMsgPtr = conn->errorMessage;
    while (1) {
    	id = clGetc(Pfin, conn->commBuf, conn->authSchemeInx);
    	if (id != 'V') {
	    if (id == 'E') {
	    	clGets(errMsgPtr, ERROR_MSG_LENGTH,Pfin, conn->commBuf,
	    	 conn->authSchemeInx);
	        return makeEmptysrbResult(conn,CLI_FATAL_ERROR);
	    } else if (id == 'N') {
	    	clGets(errMsgPtr,ERROR_MSG_LENGTH,Pfin, conn->commBuf, 
	     	 conn->authSchemeInx);
	    	errMsgPtr += strlen (errMsgPtr);
	    	continue;
	   } else if (id == 'A') {
		if (SrbServerMode == 0) {
		    /* client mode */
                    status = svrSendStatus (conn->port->sock, 
                     AUTH_ENCRYPT1_INX);
                    if (status < 0) {
                        fprintf(stderr,
                         "clFunct: Unable to send auth status msg to server\n");
                        return makeEmptysrbResult(conn, 
                         CLIENT_CROSS_ZONE_AUTH_FAILED);
                    }
	            status = clSendEncryptAuth (NULL, conn); 
		} else {
		    if (ClientCertInx >= 0) {
			/* I got the client proxy cert */
            		svrSendStatus (conn->port->sock, GSI_DELEGATE_INX);
            		status = svrSendGsiDelegAuth (ClientCertInx, conn);
		    } else {
		        status = initCliSvrAuthLink (SrbServerClientSock);
		        if (status == AUTH_ENCRYPT1_INX) {
			    svrSendStatus (conn->port->sock, AUTH_ENCRYPT1_INX);
        	            status = svrCliSvrEncryptAuth (conn->port->sock,
         	             SrbServerClientSock);
		        } else {
                            fprintf (stderr,
                             "clFunct: Auth inx %d from client not supported\n",
                             status);
                            if (status >= 0)
                                status = CLIENT_CROSS_ZONE_AUTH_FAILED;
			}
		    }
		}

		/* should get error return from the call */
		continue;
	    } else {
	    	sprintf(errMsgPtr,
	   	 "clFunct: Broken Conn- Possible cause : SRB internal error\n");
	    	return makeEmptysrbResult(conn,CLI_ERR_BROKEN_PIPE);
	    }
    	} else {
	    break;
	}
    }

    id = clGetc(Pfin, conn->commBuf, conn->authSchemeInx);
    for (;;) {
	int c;
	switch (id) {
	case 'G':		/* function returned properly */
	    clGetInt(actualResultLen,4,Pfin, conn->commBuf, 
	     conn->authSchemeInx);
	    if (resultIsInt) {
		if (*actualResultLen <= SIZEOF32) {
		    clGetInt((int *) resultBuf, SIZEOF32, Pfin, 
		     conn->commBuf, conn->authSchemeInx);
		    status = CLI_CONNECTION_OK;
		} else if (*actualResultLen == 2*SIZEOF32) {
		    clGetLong ((srb_long_t *) resultBuf,2*SIZEOF32, Pfin, 
		     conn->commBuf, conn->authSchemeInx);
		    status = CLI_CONNECTION_OK;
		} else {
                    /* Drain it */
                    clGetnchar((char *) &status, *actualResultLen + 1, Pfin,
                     conn->commBuf, conn->authSchemeInx);
                    *actualResultLen = 0;
                    return makeEmptysrbResult(conn, CLI_ERR_RETURN_LEN);
		}
	    } else {
		if (*actualResultLen < 4) {
		    /* Drain it */
		    clGetnchar((char *) &status, *actualResultLen + 1, Pfin,
		     conn->commBuf, conn->authSchemeInx);
		    *actualResultLen = 0;
		    return makeEmptysrbResult(conn, CLI_ERR_RETURN_LEN);
	    	}
		/* the first 4 bytes is the status */
#if defined(PORTNAME_c90)
		crGetInt((int *) &status, 4, Pfin, conn->commBuf, 
		 conn->authSchemeInx);
#else
		clGetnchar((char *) &status, 4, Pfin, conn->commBuf, 
		 conn->authSchemeInx);
		status = ntohl (status);
#endif
		*actualResultLen -= 4;
		if (*actualResultLen > 0) {
		    if (resultBuf == NULL) {	/* no buufer */
		    	*actualResultLen = 0;
                    	return makeEmptysrbResult(conn, CLI_ERR_RETURN_LEN);
		    } else {
			clGetnchar((char *) resultBuf, *actualResultLen,
		         Pfin, conn->commBuf, conn->authSchemeInx);
		    }
		}
	    }
	    /* get the last '0'*/
	    c = clGetc(Pfin, conn->commBuf, conn->authSchemeInx); 
	    return makeEmptysrbResult(conn, status);

	case 'E':
	    sprintf(conn->errorMessage,
		"clFunct: returned an error");
	    return makeEmptysrbResult(conn,CLI_FATAL_ERROR);

	case 'N':
	    if (clGets(conn->errorMessage, ERROR_MSG_LENGTH, Pfin, 
	     conn->commBuf, conn->authSchemeInx) == 1) {
		sprintf(conn->errorMessage,
		"Notice return detected, but error message cannot be read");
	    } else
		fprintf(stderr, "%s\n", conn->errorMessage);
	    break;

	case '0':		/* no return value */
	    return makeEmptysrbResult(conn,CLI_COMMAND_OK);

	default:
	    sprintf(conn->errorMessage,
	        "FATAL: clFunct: protocol error: id=%x\n", id);
	    return makeEmptysrbResult(conn,CLI_FATAL_ERROR);
	}
    }
}

/* clResultStatus - Get the resultStatus of a result.
 * Input - srbResult* res
 * Output - res->resultStatus
 */

int 
clResultStatus(srbResult* res)
{ 
    return res->resultStatus; 
}

int
getByteStreamFromSvr (srbConn *conn, char **resultBuf, int *actualResultLen)
{
    int status;
    int intBuf;
    int c;
    int Pfin = conn->sock;
    char *errMsgPtr;
    int id;

    errMsgPtr = conn->errorMessage;
    while (1) {
        id = clGetc(Pfin, conn->commBuf, conn->authSchemeInx);
        if (id != 'V') {
            if (id == 'E') {
                clGets(errMsgPtr, ERROR_MSG_LENGTH,Pfin, conn->commBuf,
                 conn->authSchemeInx);
                return (CLI_FATAL_ERROR);
            } else if (id == 'N') {
                clGets(errMsgPtr,ERROR_MSG_LENGTH,Pfin, conn->commBuf,
                 conn->authSchemeInx);
                errMsgPtr += strlen (errMsgPtr);
                continue;
            } else {
                sprintf(errMsgPtr,
                 "clFunct: Broken Conn- Possible cause : SRB internal error\n");
                return (CLI_ERR_BROKEN_PIPE);
            }
        } else {
            break;
        }
    }

    id = clGetc(Pfin, conn->commBuf, conn->authSchemeInx);
    for (;;) {
        switch (id) {
        case 'G':               /* function returned properly */
            clGetInt(actualResultLen,4,Pfin, conn->commBuf,
             conn->authSchemeInx);
            if (*actualResultLen < 4) {
                /* Drain it */
                clGetnchar((char *) &status, *actualResultLen + 1, Pfin,
                 conn->commBuf, conn->authSchemeInx);
                *actualResultLen = 0;
                return CLI_ERR_RETURN_LEN;
            }
            /* the first 4 bytes is the status */
#if defined(PORTNAME_c90)
            crGetInt((int *) &status, 4, Pfin, conn->commBuf,
             conn->authSchemeInx);
#else
            clGetnchar((char *) &status, 4, Pfin, conn->commBuf,
             conn->authSchemeInx);
            status = ntohl (status);
#endif
            *actualResultLen -= 4;
            if (*actualResultLen > 0) {
        	*resultBuf = malloc (*actualResultLen);
                clGetnchar((char *) *resultBuf, *actualResultLen,
                 Pfin, conn->commBuf, conn->authSchemeInx);
	    } else if (*actualResultLen == 0) {
		if (status >= 0)
		    status = CLI_ERR_RETURN_LEN;
            } else {
        	    status = CLI_ERR_RETURN_LEN;
        	*resultBuf = NULL;
            }
            /* get the last '0'*/
            c = clGetc(Pfin, conn->commBuf, conn->authSchemeInx);
            return status;
        case 'E':
            sprintf(conn->errorMessage,
                "clFunct: returned an error");
            return (CLI_FATAL_ERROR);

        case 'N':
            if (clGets(conn->errorMessage, ERROR_MSG_LENGTH, Pfin,
             conn->commBuf, conn->authSchemeInx) == 1) {
                sprintf(conn->errorMessage,
                "Notice return detected, but error message cannot be read");
            } else
                fprintf(stderr, "%s\n", conn->errorMessage);
            break;

        case '0':               /* no return value */
            return (CLI_COMMAND_OK);

        default:
            sprintf(conn->errorMessage,
                "FATAL: clFunct: protocol error: id=%x\n", id);
            return (CLI_FATAL_ERROR);
        }
    }
}
