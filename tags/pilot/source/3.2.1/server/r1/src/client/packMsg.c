/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * packMsg.c
 */

#include "packMsg.h"

int 
packMsg (char *inStr, struct varlena **outStr, FormatDef packFormat, 
FormatDefConst *localDef)
{
    struct parsedType *tmpType, *childHead;
    int status;
    packedMsg outMsg;
    char *inDataPtr;
    
    if (outStr == NULL) {
        fprintf (stderr, "packMsg: input outStr is NULL\n");
        return (PACKM_ERR_PARSE_FORMAT);
    }

    if (inStr == NULL) {
        fprintf (stderr, "packMsg: input inStr is NULL\n");
        return (PACKM_ERR_PARSE_FORMAT);
    }

    inDataPtr = inStr;
    status = initPackedMsg (&outMsg);
    if (status < 0)
	return (status);

    /* setup the structHead */
    status = parsePackFormat (packFormat, &childHead);

    if (status < 0)
	return status;

    tmpType = childHead;

    while (tmpType != NULL) {
	status = packParsedType (tmpType, &inDataPtr, &outMsg, localDef);
        if (status < 0)
             return status;

	tmpType = tmpType->next;
    }
    
    freeParsedType (childHead);
    *outStr = outMsg.msg;

    return 0;
}

int
unpackMsg (char *inStr, char **outStr, FormatDef packFormat, 
FormatDefConst *localDef)
{
    struct parsedType *tmpType, *childHead;
    int status;
    unpackedMsg outMsg;
    char *inDataPtr;
    
    if (outStr == NULL) {
        fprintf (stderr, "unpackMsg: input outStr is NULL\n");
        return (PACKM_ERR_PARSE_FORMAT);
    }

    if (inStr == NULL) {
        fprintf (stderr, "unpackMsg: input inStr is NULL\n");
        return (PACKM_ERR_PARSE_FORMAT);
    }

    inDataPtr = inStr;
    status = initUnpackedMsg (&outMsg, PACKED_MSG_LEN);
    if (status < 0)
	return (status);

    status = parsePackFormat (packFormat, &childHead);
    if (status < 0)
        return status;

    tmpType = childHead;

    while (tmpType != NULL) {
        status = unpackParsedType (tmpType, &inDataPtr, &outMsg, localDef);
        if (status < 0)
             return status;

        tmpType = tmpType->next;
    }
    *outStr = outMsg.msg;
    freeParsedType (childHead);

    return 0;
}

int
parsePackFormat (FormatDef packFormat, struct parsedType **childHead)
{
    struct parsedType *tmpParsedType, *prevParsedType;
    char buf[MAX_TOKEN]; 
    char *inForPtr, *outptr;
    int c;
    int outLen;
    int gotTypeFlag;
    int status;

    *childHead = NULL;	/* Initialize */
    tmpParsedType = prevParsedType = NULL;
    inForPtr = (char *) packFormat;
    outptr = buf;
    outLen = 0;
    gotTypeFlag = 0;

    while ((c = *inForPtr) != '\0') {
	if (c == '*') {		/* a pointer */
	    /* A type cast or a variable ?? */
	    if (gotTypeFlag == 1) {	/* A variable */
		if (outLen != 0) {	/* '*' following a variable name */
		    fprintf (stderr, "parsePackFormat: * following a variable name for %s\n", (char *) packFormat);
		    return (PACKM_ERR_PARSE_FORMAT);
		} else {
		    if (tmpParsedType == NULL) {
		        fprintf (stderr, "parsePackFormat: no type for %s\n", (char *) packFormat);
			return (PACKM_ERR_PARSE_FORMAT);
		    }
		    tmpParsedType->pointer = (char *) 1;
		}
	    } else {		/* a type cast */
		if (outLen == 0) {      /* '*' precede a type cast */
		    fprintf (stderr, "parsePackFormat: * precede a type cast for %s\n", (char *) packFormat);
		    return (PACKM_ERR_PARSE_FORMAT);
		} else {	/* process the type cast */
		    *outptr = '\0';
		    if (tmpParsedType == NULL) {
		        fprintf (stderr, "parsePackFormat: no type cast for %s\n", (char *) packFormat);
                        return (PACKM_ERR_PARSE_FORMAT);
		    }
		    tmpParsedType->pointer = (char *) 1;
		    if ((tmpParsedType->typeInx = lookupPackType (buf)) < 0) {
		        fprintf (stderr, "parsePackFormat: cannot find type cast for %s\n", (char *) packFormat);
			return (PACKM_ERR_PARSE_FORMAT);
		    }
		    /* reset a few things */ 
		    outptr = buf;
		    outLen = 0;
		    gotTypeFlag = 1;
		}
	    }
	    inForPtr ++;
	    continue;
 	}
		    

	if (c == ' ' || c == '\t' || c == '\n') {	/* white space */
	    if (outLen == 0) {
	    	inForPtr ++;
	    	continue;
	    }
	    
	    if (gotTypeFlag == 0) {	/* A type cast */
                *outptr = '\0';
                if (tmpParsedType == NULL) {
		    fprintf (stderr, "parsePackFormat: no type for %s\n", (char *) packFormat);
                    return (PACKM_ERR_PARSE_FORMAT);
		}
                if ((tmpParsedType->typeInx = lookupPackType (buf)) < 0) {
		    fprintf (stderr, "parsePackFormat: cannot find type for %s\n", (char *) packFormat);
                    return (PACKM_ERR_PARSE_FORMAT);
		}
                /* reset a few things */
                outptr = buf;
                outLen = 0;
                gotTypeFlag = 1;
	    }
	    inForPtr ++;
	    continue;
	}

	if (c == ';') { 	/* delimitor */
	    if (outLen <= 0) {
		if (gotTypeFlag > 0) {	/* nothing following type cast */
		    fprintf (stderr, "parsePackFormat: nothing following type cast for %s\n", (char *) packFormat);
		    return (PACKM_ERR_PARSE_FORMAT);
		}
            } else {		/* must be a variable */
	        if (gotTypeFlag == 0) { /* nothing following type cast */
		    fprintf (stderr, "parsePackFormat: nothing following type cast for %s\n", (char *) packFormat);
		    return (PACKM_ERR_PARSE_FORMAT);
		}
                *outptr = '\0';
                if (tmpParsedType == NULL) {
		    fprintf (stderr, "parsePackFormat: no ParsedType for %s\n", (char *) packFormat);
                    return (PACKM_ERR_PARSE_FORMAT);
		}
		tmpParsedType->name = strdup (buf);
                /* reset a few things */
                outptr = buf;
                outLen = 0;
                gotTypeFlag = 0;
		/* link the parsedType */
                if (prevParsedType != NULL) {
                    prevParsedType->next = tmpParsedType;
		    tmpParsedType->prev = prevParsedType;
                } else {
                    *childHead = tmpParsedType;
		}
                prevParsedType = tmpParsedType;
                tmpParsedType = NULL;
	    }
            inForPtr ++;
            continue;
	}

	/* This is just a normal character */

	if (tmpParsedType == NULL) {
	    tmpParsedType = allocParsedType ();
	}
	*outptr = c;
	outptr ++;
	inForPtr ++;
	outLen ++;
    }
    if (outLen > 0 && gotTypeFlag > 0) {	/* did not have the last ';' */
        *outptr = '\0';
        if (tmpParsedType == NULL) {
	    fprintf (stderr, "parsePackFormat: did not have the last ; for %s\n", (char *) packFormat);
            return (PACKM_ERR_PARSE_FORMAT);
	}
        tmpParsedType->name = strdup (buf);
        /* link the parsedType */
        if (prevParsedType != NULL) {
            prevParsedType->next = tmpParsedType;
        } else {
            *childHead = tmpParsedType;
        }
        prevParsedType = tmpParsedType;
        tmpParsedType = NULL;
    }

    return (0);
}
		
TypeInx
lookupPackType (char *typeKW)
{
    int i;

    for (i = 0; i < nPackMsgTypes; i++) {
	if (strcmp (typeKW, packTypeEntries[i].typeKW) == 0) {
	    return ((TypeInx) i);
	}
    }
    return ((TypeInx) -1);
}

struct parsedType*
allocParsedType ()
{
    struct parsedType *myParsedType;

    myParsedType = (struct parsedType *) malloc (sizeof(struct parsedType));
    myParsedType->name = NULL;
    myParsedType->parent = myParsedType->next = myParsedType->prev = NULL;
    myParsedType->dim = NULL;
    myParsedType->pointer = NULL;
    myParsedType->value = 0;
    myParsedType->typeInx = (TypeInx) 0;

    return (myParsedType);
}

/* packParsedType - Pack the ParsedType link list and return the length
 * required to store the packed message.
 */
int
packParsedType (struct parsedType *myType, char **inDataPtr, packedMsg *outMsg,
FormatDefConst *localDef)
{
    struct parsedType *tmpType, *childHead;
    int myTypeInx;
    int i;
#ifdef ADDR_64BIT
    srb_long_t c;
#else
    int c;
#endif
    char *outMsgPtr;
    int outMsgLen;
    int numElement;	/* number of elements in array */
    char **inPtr;
    int *intInPtr;
#if defined(PORTNAME_c90)
    struct int32Array *intOutPtr;
#else
    int *intOutPtr;
#endif
    int *doubleInPtr, *doubleOutPtr;
    int outSize;
    int status;

    
    /* check for array */

    status = parseArrayInStruct (myType, localDef);

    if (status < 0)
	return status;

    /* if it is a pointer, set myType->pointer to the address */

    if (myType->pointer != 0) {
#if defined(PORTNAME_c90)
	*inDataPtr = (char *) doubleAlign (*inDataPtr);
#else
	*inDataPtr = (char *) intAlign (*inDataPtr);
#endif
	if (**(int **)inDataPtr != 0)
	    myType->pointer = (**(char ***)inDataPtr);
	*inDataPtr += sizeof (char *);
    }

    myTypeInx = packTypeEntries[myType->typeInx].index;

    /* Handle each types */

    numElement = getNumInArray (myType->dim);
    outSize = packTypeEntries[myType->typeInx].size;

    switch (myTypeInx) {
	case CHAR_TYPE:
	    outMsgLen = VARSIZE (outMsg->msg) + outSize * numElement;
    	    if (outMsgLen > outMsg->maxLen) { /* over run ? */
        	status = extendPackedMsg (outMsg, outMsgLen);
		if (status < 0)
		    return (status);
	    }
    	    outMsgPtr = (char *) outMsg->msg + VARSIZE (outMsg->msg);

	    if (myType->pointer != 0) {
		if (myType->pointer == (char *) 1) {
	            fprintf (stderr, "packParsedType: pointer error for %s\n", myType->name);
		    return PACKM_ERR_PARSE_FORMAT;
		}

		memcpy (outMsgPtr, (void *) myType->pointer, numElement);
	    } else {
		memcpy (outMsgPtr, (void *) *inDataPtr, numElement);
		*inDataPtr = (char *) (*inDataPtr + numElement * outSize);
	    }
	    VARSIZE (outMsg->msg) = VARSIZE (outMsg->msg) + 
	     numElement * outSize;
	    break;
	case STR_TYPE:
	{  
	    /* Rules for STR_TYPE. If the array dimension ends with
	     * NULL_TERM, it is an array of pointers. Otherwise, if it
	     * is a pointer, it is a pointer to a simple array.
	     */
	    int lastDimSz;	/* array size of the last dimension */
	    int numStr;
	    char *tmpInPtr;
	    int tmpLen;

	    if (myType->dim == NULL)
		lastDimSz = 1;
	    else
		lastDimSz = myType->dim[myType->dim[0]]; 

	    numStr = numElement / lastDimSz;

	    for (i = 0; i < numStr; i++) {
		if (myType->pointer != 0) {
		    if (myType->pointer == (char *) 1) {  /* A NULL pointer */
			/* compose a fake str */
                	outMsgLen = VARSIZE (outMsg->msg) + 2;
                	if (outMsgLen > outMsg->maxLen) {    
                    	    status = extendPackedMsg (outMsg, outMsgLen);
			    if (status < 0)
				return (status);
			}
                	/* cal outMsgPtr again. It could have been changed */
                	outMsgPtr = (char *) outMsg->msg + 
			 VARSIZE (outMsg->msg);
			outMsgPtr[0] = CNTL_A;	/* a cntl A */
			outMsgPtr[1] = 0;
                	VARSIZE (outMsg->msg) = VARSIZE (outMsg->msg) +
			 (2);
                        if (lastDimSz < 0) {     /* array of pointers */
			    if (**(int **)inDataPtr != 0)
                            	myType->pointer = (**(char ***)inDataPtr);
			    if (i +1 < numStr)	/* last pass ? */
                            	*inDataPtr += sizeof (char *);
			    continue;
                        } else {
			    break;
                    	}
		    }

		    tmpInPtr = (char *) myType->pointer;
		    tmpLen = strlen (tmpInPtr) + 1;
		    /* advance the in pointers */
		    if (lastDimSz < 0) {		/* array of pointers */
			if (**(int **)inDataPtr != 0)
                            myType->pointer = (**(char ***)inDataPtr);
			else 
			    myType->pointer = (char *) 1;    /* A NULL */
			if (i +1 < numStr)	/* last pass ? */
                            *inDataPtr += sizeof (char *);
		    } else {
			myType->pointer += lastDimSz;
		    }
		} else {
		    tmpInPtr = *inDataPtr;
		    tmpLen = strlen (tmpInPtr) + 1;
		    if (lastDimSz < 0) {         /* array of pointers */
			*inDataPtr += tmpLen;
		    } else {
			*inDataPtr += lastDimSz;
		    }
		}
		outMsgLen = VARSIZE (outMsg->msg) + tmpLen;
        	if (outMsgLen > outMsg->maxLen) { /* over run ? */
                    status = extendPackedMsg (outMsg, outMsgLen);
		    if (status < 0)
			return (status);
		}
		/* cal outMsgPtr again. It could have been changed */
		outMsgPtr = (char *) outMsg->msg + VARSIZE (outMsg->msg);

		strcpy (outMsgPtr, tmpInPtr);
		VARSIZE (outMsg->msg) = VARSIZE (outMsg->msg) + tmpLen;
	    }
	    break;
	}
	case INT_TYPE:
    	    outMsgPtr = (char *) outMsg->msg + VARSIZE (outMsg->msg);
	    outMsgPtr = intAlign (outMsgPtr); 
	    VARSIZE (outMsg->msg) = (int) (outMsgPtr - 
	     (char *) outMsg->msg);
	    outMsgLen = VARSIZE (outMsg->msg) + outSize * numElement;
    	    if (outMsgLen > outMsg->maxLen) { /* over run */
        	status = extendPackedMsg (outMsg, outMsgLen);
		if (status < 0)
		    return (status);
                outMsgPtr = (char *) outMsg->msg + VARSIZE (outMsg->msg);
                outMsgPtr = intAlign (outMsgPtr);
                VARSIZE (outMsg->msg) = (int) (outMsgPtr -
                (char *) outMsg->msg);
	    }

            if (myType->pointer != 0) {
                if (myType->pointer == (char *) 1) {
		    if (numElement == 0)
			return (0);
	            fprintf (stderr, "packParsedType: pointer error for %s\n", myType->name);
                    return PACKM_ERR_PARSE_FORMAT;
		}
#if defined(PORTNAME_c90)
		intInPtr = (int *) doubleAlign ((char *) myType->pointer);
#else
		intInPtr = (int *) intAlign ((char *) myType->pointer);
#endif
            } else {
#if defined(PORTNAME_c90)
                intInPtr = (int *) doubleAlign (*inDataPtr);
		*inDataPtr = (char *) intInPtr + numElement * sizeof(int);
#else
		intInPtr = (int *) intAlign (*inDataPtr);
		*inDataPtr = (char *) intInPtr + numElement * outSize;
#endif
            }
#if !defined(PORTNAME_c90)
	    intOutPtr = (int *) outMsgPtr;
#endif
            myType->value = *((int *) intInPtr);
	    for (i = 0; i < numElement; i++) {
#if defined(PORTNAME_c90)
		intOutPtr = (struct int32Array *) outMsgPtr;
		if (outMsgPtr == (char *) intOutPtr) {
		    intOutPtr->myInt0 = *intInPtr;
		} else {
		    intOutPtr->myInt1 = *intInPtr;
		}
		outMsgPtr += outSize;
#else
		*intOutPtr = htonl (*intInPtr);
		intOutPtr++;
#endif
		intInPtr++;
	    }
#if defined(PORTNAME_c90)
	    VARSIZE (outMsg->msg) = (int) (outMsgPtr - 
	     (char *) outMsg->msg);
#else
	    VARSIZE (outMsg->msg) = (int) ((char *) intOutPtr - 
	     (char *) outMsg->msg);
#endif
            break;
	case DOUBLE_TYPE:
            outMsgPtr = (char *) outMsg->msg + VARSIZE (outMsg->msg);
            doubleOutPtr = (int *) doubleAlign (outMsgPtr);
            VARSIZE (outMsg->msg) = (int) ((char *) doubleOutPtr - 
	     (char *) outMsg->msg);
	    outMsgLen = VARSIZE (outMsg->msg) + outSize * numElement;
    	    if (outMsgLen > outMsg->maxLen) {  /* over run ? */
        	status = extendPackedMsg (outMsg, outMsgLen);
		if (status < 0)
		    return (status);
                outMsgPtr = (char *) outMsg->msg + VARSIZE (outMsg->msg);
                doubleOutPtr = (int *) doubleAlign (outMsgPtr);
                VARSIZE (outMsg->msg) = (int) ((char *) doubleOutPtr -
                 (char *) outMsg->msg);
	    }

            if (myType->pointer != 0) {
                if (myType->pointer == (char *) 1) {
	            fprintf (stderr, "packParsedType: pointer error for %s\n", myType->name);
                    return PACKM_ERR_PARSE_FORMAT;
		}
#if defined(PORTNAME_linux)     /* no need for doubleAlign for linux for
				 * the input of packing */
#ifdef ADDR_64BIT
                doubleInPtr = (int *) doubleAlign ((char *)myType->pointer);
#else	/* ADDR_64BIT */
                doubleInPtr = (int *) intAlign ((char *)myType->pointer);
#endif	/* ADDR_64BIT */
#else	/* PORTNAME_linux */
                doubleInPtr = (int *) doubleAlign ((char *)myType->pointer);
#endif	/* PORTNAME_linux */
            } else {
#if defined(PORTNAME_linux)     /* no need for doubleAlign for linux for
                                 * the input of packing */
#ifdef ADDR_64BIT
                doubleInPtr = (int *) doubleAlign (*inDataPtr);
#else   /* ADDR_64BIT */
                doubleInPtr = (int *) intAlign (*inDataPtr);
#endif	/* ADDR_64BIT */
#else	/* PORTNAME_linux */
                doubleInPtr = (int *) doubleAlign (*inDataPtr);
#endif	/* PORTNAME_linux */
                *inDataPtr = (char *) doubleInPtr + numElement * outSize;
            }
#ifdef foo
#if defined(PORTNAME_c90)
            for (i = 0; i < numElement; i++) {
#else
            for (i = 0; i < numElement * 2; i++) {
#endif
                *doubleOutPtr = htonl (*doubleInPtr);
                doubleOutPtr++;
                doubleInPtr++;
            }
#endif	/* foo */

            for (i = 0; i < numElement; i++) {
#if defined(PORTNAME_c90)
                *doubleOutPtr = *doubleInPtr;
                doubleOutPtr+=1;
                doubleInPtr+=1;
#else
                *doubleOutPtr = *doubleInPtr;
                *(doubleOutPtr+1) = *(doubleInPtr + 1);
                srbNtohll ((u_longlong_t *) doubleOutPtr);
                doubleOutPtr+=2;
                doubleInPtr+=2;
#endif
            }

            VARSIZE (outMsg->msg) = (int) ((char *) doubleOutPtr - 
	     (char *) outMsg->msg);
            break;
	case STRUCT_TYPE:
            if (myType->pointer != 0) {
                if (myType->pointer == (char *) 1) {
	            fprintf (stderr, "packParsedType: pointer error for %s\n", myType->name);
                    return PACKM_ERR_PARSE_FORMAT;
		}
		inPtr = (char **) &myType->pointer;
            } else {
		inPtr = inDataPtr;
            }
	    /* Resolve the strct name */
	    c = relsoveFormatDef (myType->name, myType, localDef);
	    if (c == PACKM_ERR_PARSE_FORMAT) {
	        fprintf (stderr, "packParsedType: relsoveFormatDef error for %s, stat=%d\n", myType->name, c);
		return PACKM_ERR_PARSE_FORMAT;
	    }

	    for (i = 0; i < numElement; i++) {
		status = parsePackFormat ((FormatDef) *(char **) c, &childHead);
		if (status < 0)
		    return status;

		if (childHead != NULL)
		    childHead->parent = myType;

		tmpType = childHead;
    	    	while (tmpType != NULL) {
        	    status = packParsedType (tmpType, inPtr, outMsg, localDef);
		    if (status < 0)
			return status;
        	    tmpType = tmpType->next;
    	    	}

		/* We are done with childHead and its sibling. release them */
		freeParsedType (childHead);
	    }
            break;
        default:
	    break;
    }
    return (0);
}

/* unpackParsedType - Unpack the ParsedType link list.
 */
int
unpackParsedType (struct parsedType *myType, char **inDataPtr, 
unpackedMsg *outMsg, FormatDefConst *localDef)
{

    struct parsedType *tmpType, *childHead;
    int myTypeInx;
    int i;
#ifdef ADDR_64BIT
    srb_long_t c;
#else
    int c;
#endif
    char *outMsgPtr, *inMsgPtr;
    int outMsgLen;
    int numElement;     /* number of elements in array */
    char **inPtr;
    int *intOutPtr;
#ifdef ADDR_64BIT
    srb_long_t *longOutPtr;
#endif
#if defined(PORTNAME_c90)
    struct int32Array *intInPtr;
#else
    int *intInPtr;
#endif
    int *doubleInPtr, *doubleOutPtr;
    int outSize;
    int status;
    unpackedMsg myOutMsg, *thisOutMsg;

    
    /* check for array */

    status = parseArrayInStruct (myType, localDef);

    if (status < 0)
        return status;

    myTypeInx = packTypeEntries[myType->typeInx].index;

    /* Handle each types */

    numElement = getNumInArray (myType->dim);
    outSize = packTypeEntries[myType->typeInx].size;

    switch (myTypeInx) {
	case CHAR_TYPE:
            if (myType->pointer != 0) {
                status = addPointToUnpackMsg (myType, outMsg, 
		 outSize * numElement);
		if (status < 0)
		    return (status);
                memcpy ((void *) myType->pointer, (void *) *inDataPtr,
                 numElement);
            } else {
                outMsgLen = outMsg->len + outSize * numElement;
                if (outMsgLen > outMsg->maxLen) { /* over run ? */
                    status = extendUnpackedMsg (outMsg, outMsgLen);
		    if (status < 0)
			return status;
		}
                outMsgPtr = outMsg->msg + outMsg->len;
                memcpy (outMsgPtr, (void *) *inDataPtr, numElement);
		/* use sizeof rather than outSize because c90 word size is 8 */
                outMsg->len += numElement * outSize;
            }
            *inDataPtr = (char *) (*inDataPtr + numElement * outSize);
            break;
        case STR_TYPE:
        {
            /* Rules for STR_TYPE. If the array dimension ends with
             * NULL_TERM, it is an array of pointers. Otherwise, if it
             * is a pointer, it is a pointer to a simple array.
             */
            int lastDimSz;      /* array size of the last dimension */
            int numStr;
            char *tmpInPtr;
            int tmpLen;

            if (myType->dim == NULL)
                lastDimSz = 1;
            else
                lastDimSz = myType->dim[myType->dim[0]];

            numStr = numElement / lastDimSz;
	
	    thisOutMsg = NULL;
            for (i = 0; i < numStr; i++) {
		tmpLen = strlen (*inDataPtr) + 1;
	    	if (myType->pointer != 0) {
		    tmpInPtr = *inDataPtr;
		    /* Handle null pointer */
		    if (tmpInPtr[0] == CNTL_A && tmpInPtr[1] == 0) {
			status = addPointToUnpackMsg (myType, outMsg, 0);
			if (status < 0)
			    return (status);
			*inDataPtr = (char *) (*inDataPtr + 2);
			if (lastDimSz < 0)         /* array of pointers */
			    continue;
			else
			    break;
		    }

		    if (lastDimSz < 0) {                /* array of pointers */

/*
			addPointToUnpackMsg (myType, outMsg, tmpLen);
*/
			if (numElement > tmpLen) 
			    status = addPointToUnpackMsg (myType, outMsg, 
				numElement);
			else
			    status = addPointToUnpackMsg (myType, outMsg, 
				tmpLen);
			if (status < 0)
			    return (status);
		
			myOutMsg.msg = (char *) myType->pointer;
			myOutMsg.maxLen = tmpLen;
			myOutMsg.len = 0;
			thisOutMsg = &myOutMsg;
		    } else {
			if (thisOutMsg == NULL) {
			    int mySize;

			    if (tmpLen < numElement) {
				mySize = numElement;
			    } else {
				mySize = tmpLen;
			    }
			    if (mySize < PACKED_MSG_LEN) {
				mySize = PACKED_MSG_LEN;
			    }
			    status = addPointToUnpackMsg (myType, outMsg, 
				mySize);
			    if (status < 0)
				return (status);
			    myOutMsg.maxLen = mySize;
			     
			    myOutMsg.msg = (char *) myType->pointer;
			    myOutMsg.len = 0;
			    thisOutMsg = &myOutMsg;
			} else {
			    outMsgLen = thisOutMsg->len + tmpLen;
			    if (outMsgLen > thisOutMsg->maxLen) {
                    		status = extendUnpackedMsg (outMsg, outMsgLen);
				if (status < 0)
				    return (status);
			    }
/*
                    		extendUnpackedMsg (thisOutMsg, outMsgLen);
 */
			}
		    }
		} else {
		    if (thisOutMsg == NULL)
			thisOutMsg = outMsg;
		}
    		outMsgPtr = thisOutMsg->msg + thisOutMsg->len;
		if (lastDimSz < 0) 
		    thisOutMsg->len += tmpLen;
		else
		    thisOutMsg->len += lastDimSz;
		strcpy (outMsgPtr, *inDataPtr);
		*inDataPtr = (char *) (*inDataPtr + tmpLen);
	    }
	    break;
	}
	case INT_TYPE:
            if (myType->pointer != 0) {
		status = addPointToUnpackMsg (myType, outMsg, 
		 outSize * numElement);
		if (status < 0)
		    return (status);
#if defined(PORTNAME_c90)
		intOutPtr = (int *) doubleAlign ((char *) myType->pointer);
#else
		intOutPtr = (int *) intAlign ((char *) myType->pointer);
#endif
            } else {
    		outMsgPtr = outMsg->msg + outMsg->len;
#if defined(PORTNAME_c90)
	        intOutPtr = (int *) doubleAlign (outMsgPtr); 
#else
	        intOutPtr = (int *) intAlign (outMsgPtr); 
#endif
		outMsg->len = (char *) intOutPtr - outMsg->msg;
    		outMsgLen = outMsg->len + outSize * numElement;
    		if (outMsgLen > outMsg->maxLen) {	/* over run */
	    	    status = extendUnpackedMsg (outMsg, outMsgLen);
		    if (status < 0)
			return (status);
    		    outMsgPtr = outMsg->msg + outMsg->len;
	            intOutPtr = (int *) intAlign (outMsgPtr); 
		}
		/* use sizeof rather than outSize because c90 word size is 8 */
		outMsg->len += numElement * sizeof (int);
            }
#if defined(PORTNAME_c90)
	    inMsgPtr = intAlign (*inDataPtr);
            for (i = 0; i < numElement; i++) {
		intInPtr = (struct int32Array *) inMsgPtr;
		if (inMsgPtr == (char *) intInPtr) {
		    *intOutPtr = sToL (intInPtr->myInt0);
		} else {
		    *intOutPtr = sToL (intInPtr->myInt1);
		}
		if (i == 0)
		    myType->value = *intOutPtr;
                intOutPtr++;
		inMsgPtr += outSize;
            }
            *inDataPtr = inMsgPtr;
#else
	    intInPtr = (int *) intAlign (*inDataPtr);
            myType->value = ntohl (*((int *) intInPtr));
	    for (i = 0; i < numElement; i++) {

		*intOutPtr = ntohl (*intInPtr);
		intOutPtr++;
		intInPtr++;
	    }
	    *inDataPtr = (char *) intInPtr;
#endif
            break;
	case DOUBLE_TYPE:
            if (myType->pointer != 0) {
		status = addPointToUnpackMsg (myType, outMsg, 
		 outSize * numElement);
		if (status < 0)
		    return (status);
#if defined(PORTNAME_linux)     /* no need for doubleAlign for linux for
                                 * the output of unpacking */
#ifdef ADDR_64BIT
                doubleOutPtr = (int *) doubleAlign ((char *)myType->pointer);
#else	/* ADDR_64BIT */
                doubleOutPtr = (int *) intAlign ((char *)myType->pointer);
#endif	/* ADDR_64BIT */
#else	/* PORTNAME_linux */
                doubleOutPtr = (int *) doubleAlign ((char *)myType->pointer);
#endif	/* PORTNAME_linux */
            } else {
		outMsgPtr = outMsg->msg + outMsg->len;
#if defined(PORTNAME_linux)     /* no need for doubleAlign for linux for
                                 * the output of unpacking */
#ifdef ADDR_64BIT
            	doubleOutPtr = (int *) doubleAlign (outMsgPtr);
#else   /* ADDR_64BIT */
            	doubleOutPtr = (int *) intAlign (outMsgPtr);
#endif	/* ADDR_64BIT */
#else	/* PORTNAME_linux */
            	doubleOutPtr = (int *) doubleAlign (outMsgPtr);
#endif  /* PORTNAME_linux */
		outMsg->len = (char *) doubleOutPtr - outMsg->msg;
    		outMsgLen = outMsg->len + outSize * numElement;
    		if (outMsgLen > outMsg->maxLen) {	/* over run ? */
	    	    status = extendUnpackedMsg (outMsg, outMsgLen);
		    if (status < 0)
			return (status);
    		    outMsgPtr = outMsg->msg + outMsg->len;
#if defined(PORTNAME_linux)     /* no need for doubleAlign for linux for
                                 * the output of unpacking */
	            doubleOutPtr = (int *) intAlign (outMsgPtr); 
#else
	            doubleOutPtr = (int *) doubleAlign (outMsgPtr); 
#endif
		}
		outMsg->len += numElement * outSize;
	    }
            doubleInPtr = (int *) doubleAlign (*inDataPtr);

            for (i = 0; i < numElement; i++) {
#if defined(PORTNAME_c90)
                *doubleOutPtr = *doubleInPtr;
                doubleOutPtr+=1;
                doubleInPtr+=1;
#else
                *doubleOutPtr = *doubleInPtr;
                *(doubleOutPtr+1) = *(doubleInPtr + 1);
	        srbNtohll ((u_longlong_t *) doubleOutPtr);
                doubleOutPtr+=2;
                doubleInPtr+=2;
#endif
            }
	    *inDataPtr = (char *) doubleInPtr;
            break;
	case STRUCT_TYPE:
            if (myType->pointer != 0) {
		status = initUnpackedMsg (&myOutMsg, PACKED_MSG_LEN);
		if (status < 0)
		    return (status);
		thisOutMsg = &myOutMsg;
                outMsgPtr = outMsg->msg + outMsg->len;
#ifdef ADDR_64BIT
                longOutPtr = (srb_long_t *) doubleAlign (outMsgPtr);
                *longOutPtr = (srb_long_t) myOutMsg.msg;
                longOutPtr ++;
		intOutPtr = (int *) longOutPtr;
#else
                intOutPtr = (int *) intAlign (outMsgPtr);
                *intOutPtr = (int) myOutMsg.msg;
                intOutPtr ++;
#endif
                outMsg->len = (int) ((char *) intOutPtr - outMsg->msg);

            } else {
		thisOutMsg = outMsg;
            }
	    /* Resolve the strct name */
	    c = relsoveFormatDef (myType->name, myType, localDef);
	    if (c == PACKM_ERR_PARSE_FORMAT) {
	        fprintf (stderr, "unpackParsedType: relsoveFormatDef error %s, stat = %d\n", myType->name, c);
		return PACKM_ERR_PARSE_FORMAT;
	    }

	    for (i = 0; i < numElement; i++) {
		status = parsePackFormat ((FormatDef) *(char **) c, &childHead);
		if (status < 0)
		    return status;

		if (childHead != NULL)
		    childHead->parent = myType;

		tmpType = childHead;
    	    	while (tmpType != NULL) {
        	    status = unpackParsedType (tmpType, inDataPtr, thisOutMsg,
		     localDef);
		    
		    if (status < 0)
			return status;

        	    tmpType = tmpType->next;
    	    	}
		/* We are done with childHead and its sibling. release them */
		freeParsedType (childHead);
	    }
            /* XXXXX fix umd problem. If thisOutMsg has been expanded,
             * adjust outMsg too */
            if (thisOutMsg == &myOutMsg) {
                int *tmpIntPtr;
#ifdef ADDR_64BIT
		srb_long_t tmpLongIntPtr;
		tmpLongIntPtr = longOutPtr - 1;
                tmpIntPtr = (int *) tmpLongIntPtr;
#else
                tmpIntPtr = intOutPtr - 1;
#endif
                if (*tmpIntPtr != (int) myOutMsg.msg) {
                    *tmpIntPtr = (int) myOutMsg.msg;
                }
            }
            break;
        default:
	    break;
    }
    return (0);
}

#ifdef ADDR_64BIT
srb_long_t
#else
int 
#endif
relsoveFormatDef (char *intName, struct parsedType *myType, 
FormatDefConst *localDef)
{
    struct parsedType *tmpType;
    int i;
    int nLocalDef;

    /* Check int in the same struct first */

    tmpType = myType->prev;

    while (tmpType != NULL) {
	if (strcmp (intName, tmpType->name) == 0 && 
	 packTypeEntries[tmpType->typeInx].index == INT_TYPE) {
	    return (tmpType->value);
	}
	if (tmpType->prev == NULL && tmpType->parent != NULL)
	    tmpType = tmpType->parent;
	else 
	    tmpType = tmpType->prev;
    }

    /* Try the local def first */

    
    if (localDef != NULL) {
	i = 0;
	while (strcmp (localDef[i].name, END_DEF_NAME) != 0 && 
#ifdef ADDR_64BIT
	 (srb_long_t) localDef[i].value != END_DEF_VAL) {
#else
	 (int) localDef[i].value != END_DEF_VAL) {
#endif
            if (strcmp (intName, localDef[i].name) == 0) {
#ifdef ADDR_64BIT
            	return ((srb_long_t) localDef[i].value);
#else
            	return ((int) localDef[i].value);
#endif
            }
	    i++;
	}
    }

    /* Try the "global" constant */

    for (i = 0; i < nGlobalDef; i ++) {
	if (strcmp (intName, GlobalDef[i].name) == 0) {
#ifdef ADDR_64BIT
	    return ((srb_long_t) GlobalDef[i].value);
#else
	    return ((int) GlobalDef[i].value);
#endif
	}
    }

    fprintf (stderr, "relsoveFormatDef: error resolving %s\n", myType->name);
    return (PACKM_ERR_PARSE_FORMAT);
}

int
initPackedMsg (packedMsg *outMsg)
{
    int len;
    if (PACKED_MSG_LEN < VAROUTHDRSZ)
	len = VAROUTHDRSZ + 4;
    else
	len = PACKED_MSG_LEN;

    if ((outMsg->msg = (struct varlena *) malloc (len)) == NULL)
	return (CLI_ERR_MALLOC);

    memset (outMsg->msg, 0, len);
    outMsg->maxLen = len;
    VARSIZE(outMsg->msg) = VAROUTHDRSZ;

    return 0;
} 

int
initUnpackedMsg (unpackedMsg *outMsg, int maxLen)
{

    if ((outMsg->msg = malloc (maxLen)) == NULL)
        return (CLI_ERR_MALLOC);

    memset (outMsg->msg, 0, maxLen);

    outMsg->maxLen = maxLen;
    outMsg->len = 0;

    return 0;
}

int
freeParsedType (struct parsedType *head)
{
    struct parsedType *tmpType = head;
    struct parsedType *nextType;

    while (tmpType != NULL) {
	if (tmpType->name != NULL)
            free (tmpType->name);
	if (tmpType->dim != NULL)
	    free (tmpType->dim);
	nextType = tmpType->next;
	free (tmpType);
	tmpType = nextType;
    }
    return (0);
}
	
int 
extendPackedMsg (packedMsg *myPackedMsg, int minLen)
{
    int newLen;
    struct varlena *oldMsg;

    if (minLen > MAX_INC_MSG_LEN) {
	newLen = (minLen / PACKED_MSG_LEN) * PACKED_MSG_LEN +
	 MAX_INC_MSG_LEN;
    } else {
	newLen = (minLen * 2 / PACKED_MSG_LEN) * PACKED_MSG_LEN; 
    }

    oldMsg = myPackedMsg->msg;

    if ((myPackedMsg->msg = (struct varlena *) malloc (newLen)) == NULL)
        return (CLI_ERR_MALLOC);
 
    memset (myPackedMsg->msg, 0, newLen);

    if (VARSIZE (oldMsg) > 0)
    	memcpy ((void *) myPackedMsg->msg, (void *) oldMsg, 
   	 VARSIZE (oldMsg));
    else 
	VARSIZE (myPackedMsg->msg) = 0;

    myPackedMsg->maxLen = newLen;

    free (oldMsg);

    return 0;
}

int
extendUnpackedMsg (unpackedMsg *myMsg, int minLen)
{
    int newLen;
    char *oldMsg;

    if (minLen > MAX_INC_MSG_LEN) {
        newLen = (minLen / PACKED_MSG_LEN) * PACKED_MSG_LEN +
         MAX_INC_MSG_LEN;
    } else {
        newLen = (minLen * 2 / PACKED_MSG_LEN) * PACKED_MSG_LEN; 
    }

    oldMsg = myMsg->msg;

    if ((myMsg->msg = (char *) malloc (newLen)) == NULL)
        return (CLI_ERR_MALLOC);

    memset (myMsg->msg, 0, newLen);

    if (myMsg->len > 0)
    	memcpy ((void *) myMsg->msg, (void *) oldMsg, myMsg->len);
    myMsg->maxLen = newLen;

    free (oldMsg);

    return 0;
}

char *
intAlign (char *ptr)
{
#if defined(PORTNAME_c90)
    int *iptr, b;
    char *outPtr;

    iptr = (int *) ptr;

    b = ptr - (char *) iptr;
    if (b == 0) {
	return (ptr);
    } else if (b <= 4) {
	return ((char *) iptr + 4);
    } else {
	return ((char *) iptr + 8);
    }
#else
#ifdef ADDR_64BIT
    srb_long_t b, w;
    b = (srb_long_t) ptr;
#else
    int b, w;
    b = (int) ptr;
#endif

    w = b / SIZEOF32;

    if (w * SIZEOF32 != b) {
        if (w < 0)
            b = w *SIZEOF32;
        else
            b = (w + 1) * SIZEOF32;
    }

/*
    if (w * SIZEOF32 != b) 
	b = (w + 1) * SIZEOF32;
*/

    return ((char *) b);
#endif
}

char *
doubleAlign (char *ptr)
{
    int doubleSize = 2 * SIZEOF32;
#if defined(PORTNAME_c90)
    int *iptr, b;
    char *outPtr;

    iptr = (int *) ptr;

    b = ptr - (char *) iptr;
    if (b == 0) {
        return (ptr);
    } else {
        return ((char *) iptr + 8);
    }
#else
#ifdef ADDR_64BIT
    /* XXXX may be a problem with srb_long_t. need to be unsigned */
    srb_long_t b, w;
    b = (u_longlong_t) ptr;
#else
/*
    unsigned int b, w;
    b = (unsigned int) ptr;
*/
    int b, w;
    b = (int) ptr;
#endif

    w = b / doubleSize;

    if (w * doubleSize != b) {
	if (w < 0) 
	    b = w *doubleSize;
	else
            b = (w + 1) * doubleSize;
    }

    return ((char *) b);
#endif
}

int
getNumInArray (int *dim)
{
    int i;
    int num = 1;

    if (dim == NULL) 	/* not an array. Just a single value */
	return (num);	

    for (i = 0; i < dim[0]; i++) {
	num *= dim[i+1];
    }

    return (num);
}

int parseArrayInStruct (struct parsedType *myType, FormatDefConst *localDef) 
{
    char *tmpPtr, *bufPtr;
    char buf[MAX_TOKEN];
    int bufLen, c, i;
    int gotOpenB = 0;   /* got open bracket */
    int *dim;
    int arLen;

    /* check for array */

    tmpPtr = myType->name;
    bufPtr = buf;
    bufLen = 0;

    while ((c = *tmpPtr) != '\0') {
        if (c == '[') {
            if (gotOpenB != 0) {
               fprintf (stderr, "parseArrayInStruct: 2 { for %s\n", myType->name);
                return PACKM_ERR_PARSE_FORMAT;
            } else {
                *tmpPtr= '\0';
                gotOpenB = 1;
            }
            tmpPtr ++;
            continue;
        } else if (c == ']') {
            if (gotOpenB == 0 || bufLen <= 0) {
               fprintf (stderr, "parseArrayInStruct: no ] for %s\n", myType->name);
                return PACKM_ERR_PARSE_FORMAT;
            }
            /* We have an array here */
            *bufPtr = '\0';
            arLen = relsoveFormatDef (buf, myType, localDef);
#ifdef foo	/* what about zero len integer array */
            if (arLen <= 0 && 
	     !(packTypeEntries[myType->typeInx].index == STR_TYPE && 
	      arLen == NULL_TERM)) {
               fprintf (stderr, "parseArrayInStruct: 2 { for %s\n", myType->name);
                return PACKM_ERR_PARSE_FORMAT;
	    }
#endif
	    if (arLen == PACKM_ERR_PARSE_FORMAT) {
		fprintf 
		 (stderr, "parseArrayInStruct: relsoveFormatDef error=%d\n",
		 arLen);
		return PACKM_ERR_PARSE_FORMAT;
	    }

            if (myType->dim == NULL) {
                myType->dim = (int *) malloc ((MAX_ARRAY_DIM + 1) *
                 sizeof (int));
		memset (myType->dim, 0, (MAX_ARRAY_DIM + 1) * sizeof (int));
                myType->dim[0] = 0;
            }
            dim = myType->dim;
            dim[0]++;
            dim[dim[0]] = arLen;
            /* reset a few things */
            bufPtr = buf;
            bufLen = 0;
            gotOpenB = 0;
            tmpPtr ++;
            continue;
        } else {        /* A normal character */
            if (gotOpenB > 0) {
                *bufPtr = *tmpPtr;
                bufPtr ++;
                bufLen ++;
            }
            tmpPtr ++;
            continue;
        }
    }
    return (0);
}

/* addPointToUnpackMsg -Do a malloc of input size and put the pointer
 * at the end of outMsg->msg and adjust outMsg->len to include the
 * pointer.
 */
int
addPointToUnpackMsg (struct parsedType *myType, unpackedMsg *outMsg, int size)
{
    char *outMsgPtr;
#ifdef ADDR_64BIT
    srb_long_t *intOutPtr;
#else
    int *intOutPtr;
#endif
    int outMsgLen;
    int status;

    outMsgPtr = outMsg->msg + outMsg->len;
#if defined(PORTNAME_c90) || defined(ADDR_64BIT)
    intOutPtr = (srb_long_t *) doubleAlign (outMsgPtr);
#else
    intOutPtr = (int *) intAlign (outMsgPtr);
#endif
    outMsgLen = (int) ((char *) intOutPtr - outMsg->msg) + sizeof (int *);
    if (outMsgLen > outMsg->maxLen) { 	/* over run ? */
        status = extendUnpackedMsg (outMsg, outMsgLen);
	if (status < 0)
	    return status;
	outMsgPtr = outMsg->msg + outMsg->len;
#if defined(PORTNAME_c90) || defined(ADDR_64BIT)
	intOutPtr = (srb_long_t *) doubleAlign (outMsgPtr);
#else
	intOutPtr = (int *) intAlign (outMsgPtr);
#endif
    }

    if (size <= 0) {	/* add a null pointer */
	myType->pointer = (char *) 1;
	*intOutPtr = 0;
    } else {	
    	myType->pointer = malloc (size);
	if (myType->pointer == NULL)
	    return (CLI_ERR_MALLOC);
	memset (myType->pointer, 0, size);
#if defined(PORTNAME_c90) || defined(ADDR_64BIT)
    	*intOutPtr = (srb_long_t) myType->pointer;
#else
    	*intOutPtr = (int) myType->pointer;
#endif
    }
    outMsg->len = outMsgLen;
    
    return (0);
}


