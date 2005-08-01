/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/* objMcatOprFunct.c - Routines to handle Operation involving MCAT metadata. 
 */

#include "objMcatOprFunct.h"

/* svrGetDatasetInfo - Entry point for the srbGetDatasetInfo SID.
 *
 *
 * Input : struct varlena *vObjID - The object ID
 *         struct varlena *vCollectionName - The collection name
 *	   int rowsWanted - The number of rows wanted.
 *
 * Output : Returns a mdasC_sql_result_struct in (struct varlena) format.
 */

struct varlena *
svrGetDatasetInfo (int catType, struct varlena *vObjID,
struct varlena *vCollectionName, int rowsWanted, struct varlena  *vMode)
{
    mdasC_sql_result_struct  *myresult = NULL;
    struct varlena * retVal;
    char *objID;
    char *collectionName;
    char *mode;
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;
 
    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    mode = varToStr (vMode);
 
    /* Check with MDAS for the loid info */
 
    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef DEMO
        elog (DEMOM,
	  "svrGetDatasetInfo: Local host is mdasEnabled. Do a local get_dataset_info");
#endif
#ifdef SRB_MDAS
        status = get_dataset_info(catType, objID, ClientUser->userName,
#ifdef foo
          "read", ClientUser->domainName, collectionName, &myresult, 
#endif
          mode, ClientUser->domainName, collectionName, &myresult, 
	  rowsWanted);
	commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            retVal = (struct varlena *)malloc(VAROUTHDRSZ);
            VARSIZE(retVal) = VAROUTHDRSZ;
            VAROUTSTAT(retVal) = htonl (status);

            return retVal;
        }
 
        /* Do a client call. Have to supply a mdasC_sql_result_struct. */
 
        myresult = (mdasC_sql_result_struct  *)
          malloc (sizeof (mdasC_sql_result_struct));
	memset (myresult, 0, sizeof (mdasC_sql_result_struct));
        if ((status = srbGetDatasetInfo (hostTabPtr->conn, catType, objID, 
	  collectionName, myresult, rowsWanted, mode)) < 0) {
	    if (status != DATA_NOT_IN_CAT) {
            elog(NOTICE, 
	     "mdasGetInfo: Can't srbGetDatasetInfo obj %s. stat=%d",
              objID, status);
	    }
	    free (myresult);
	    myresult = NULL;
        }
    }

    if (myresult == NULL) {
    	free (objID);
    	free (mode);
    	free (collectionName);
	retVal = (struct varlena *)malloc(VAROUTHDRSZ);
	VARSIZE(retVal) = VAROUTHDRSZ;
	VAROUTSTAT(retVal) = htonl (status);

	return retVal;
    }

#ifdef foo
    /* Convert results to Var format */

    retVal = sqlResultToVar (myresult, MULTI_ROW);

    freeSqlResult (myresult);
    free (objID);
    free (collectionName);

    if (retVal != NULL) {
	return (retVal);
    } else {
	retVal = (struct varlena *)malloc(VAROUTHDRSZ);
	VARSIZE(retVal) = VAROUTHDRSZ;
	VAROUTSTAT(retVal) = htonl (status);

	return retVal;
    }
#endif

    status = packMsg ((char *) myresult, &retVal, sqlResultStruct_PF,
     NULL);
    freeSqlResult (myresult);
    free (objID);
    free (mode);
    free (collectionName);

    if (status < 0) {
        return (genIntRet (status));
    }
    VAROUTSTAT(retVal) = htonl (status);
    return retVal;
}


/* sqlResultToVar - Convert a mdasC_sql_result_struct to the varlena
 * format so that it can be returned to the client.
 *
 * Input - (mdasC_sql_result_struct *myresult - The mdasC_sql_result_struct
 * 	   to be converted.
	   int rowFlag - SINGLE_ROW or MULTI_ROW. (no. of rows in the return).
 *
 * Output - The converted varlena.
 */
struct varlena *
sqlResultToVar (mdasC_sql_result_struct *myresult, int rowFlag)
{
    int resLen = SIZEOF32 * 3;	/* The length required  to store result_count, 
				 * row_count, continuation_index */
#if defined(PORTNAME_c90)
    struct int32Array *iptr;
#endif
    char *ptr;		/* a temp char pointer */
    struct varlena *retVal;	/* the return pointer */
    int i;
    int status;

    if (myresult == NULL) {
	status = 0;
	retVal = (struct varlena *)malloc(VAROUTHDRSZ);
	VARSIZE(retVal) = VAROUTHDRSZ;
	VAROUTSTAT(retVal) = htonl (status);

	return retVal;
    }

    /* Find the length required for the return data */

    for (i = 0; i < myresult->result_count ; i++){

	/* Add one for the NULL at end of str */
	resLen += (strlen (myresult->sqlresult[i].tab_name) + 1);
	resLen += (strlen (myresult->sqlresult[i].att_name) + 1);
	if (rowFlag == SINGLE_ROW)
            resLen += (strlen (myresult->sqlresult[i].values) + 1);
	else
            resLen += myresult->row_count * MAX_DATA_SIZE;
    }

    if ((VAROUTHDRSZ + resLen) > SQL_RESULT_LEN) {
	elog (NOTICE, "sqlResultToVar:The sql_result is too long: %d, max = %d",
	(VAROUTHDRSZ + resLen), SQL_RESULT_LEN);
	status = SQL_RSLT_TOO_LONG;
        retVal = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (status);

        return retVal;
    }
    retVal = (struct varlena *)malloc(VAROUTHDRSZ + resLen);
    VARSIZE(retVal) = resLen + VAROUTHDRSZ;
    VAROUTSTAT(retVal) = 0;		/* status */
    ptr = VAROUTDATA(retVal); 		/* the beginning of the data */
#if defined(PORTNAME_c90)
    iptr = (struct int32Array *) VAROUTDATA(retVal); 
#endif

    /* Now fill in the data */

    if (myresult->result_count == 0)
	myresult->continuation_index = -1;
 
#if defined(PORTNAME_c90)
    iptr->myInt0 = htonl (myresult->result_count);
    iptr->myInt1 = htonl (myresult->row_count);
    ptr += SIZEOF32;
    ptr += SIZEOF32;	/* skip updating the iptr at non 64 bit boundary */
    iptr = (struct int32Array *) ptr;
    iptr->myInt0 = htonl (myresult->continuation_index);
    ptr += SIZEOF32;
#else
    *(INT32 *) ptr = htonl (myresult->result_count);
    ptr += SIZEOF32;
    *(INT32 *) ptr = htonl (myresult->row_count);
    ptr += SIZEOF32;
    *(INT32 *) ptr = htonl (myresult->continuation_index);
    ptr += SIZEOF32;
#endif

    for (i = 0; i < myresult->result_count; i++){
 
        /* Add one for the NULL at end of str */

	strcpy (ptr, myresult->sqlresult[i].tab_name);
	ptr += strlen (myresult->sqlresult[i].tab_name);
	*ptr = '\0';
	ptr ++;
	strcpy (ptr, myresult->sqlresult[i].att_name);
	ptr += strlen (myresult->sqlresult[i].att_name);
	*ptr = '\0';
        ptr ++;
        if (rowFlag == SINGLE_ROW) {
	    strcpy (ptr, myresult->sqlresult[i].values);
	    ptr += strlen (myresult->sqlresult[i].values);
            *ptr = '\0';
            ptr ++;
	} else {
	    memcpy (ptr, myresult->sqlresult[i].values,
	      myresult->row_count * MAX_DATA_SIZE);
	    ptr += myresult->row_count * MAX_DATA_SIZE;
	}
    }
    return retVal;

errRet:
    retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    VARSIZE(retVal) = VAROUTHDRSZ;
    VAROUTSTAT(retVal) = htonl (status);

    return retVal;
}

/* mdasGetInfo - Get info for a Loid from MDAS and put the results in
 * infoOut.
 * 
 * Input : struct mdasInfoInp *infoInp - The input struct
 * 	   struct mdasInfoOut **infoOutHead - The output struct
 *
 * Output : Returns 0 - success, negative - failure
 */

int mdasGetInfo 
(struct mdasInfoInp *infoInp, struct mdasInfoOut **infoOutHead)
{
    mdasC_sql_result_struct  *myresult = NULL;
    char *objID, *resourceType, *dataPath, *resourceLoc, *resourceName, 
    *dataType, *replNum, *offset, *size, *phyResClass, *containerName, 
    *isDirty, *chksum;
    struct mdasInfoOut *infoOut, *prevInfoOut;
    int i, objTypeInx;
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Check with MDAS for the requested data */
 
    remoteFlag = getMdasEnabledHost (infoInp->collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
        return (remoteFlag);
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef DEMO
        elog (DEMOM,
	  "mdasGetInfo: Local host is mdasEnabled. Do a local get_dataset_info");
#endif
#ifdef SRB_MDAS
    	status = get_dataset_info(MDAS_CATALOG, infoInp->objID, 
	 infoInp->userName,
	  infoInp->accessMode, infoInp->domainName, infoInp->collectionName,
	   &myresult, DEF_NUM_ROWS);
	commit_db2_interaction (MDAS_CAT_COMMIT);
	if (status < 0) {
	    if (status != DATA_NOT_IN_CAT && status != MCAT_INQUIRE_ERROR) 
                elog(NOTICE,
                 "mdasGetInfo: Can't get_dataset_info obj\"%s\", status = %d",
              infoInp->objID, status);
	    if (myresult != NULL) 
		freeSqlResult (myresult);
            return status;
        }
#ifdef DEMO
        elog (DEMOM,
          "mdasGetInfo: Done with a local get_dataset_info");
#endif
#endif
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            return status;
        }

	/* Do a client call. Have to supply a mdasC_sql_result_struct. */

	myresult = (mdasC_sql_result_struct  *) 
	  malloc (sizeof (mdasC_sql_result_struct));

   	if (myresult == NULL) {
            elog(NOTICE,"mdasGetInfo: malloc error");
            return SYS_ERR_MALLOC;
        }
	memset (myresult, 0, sizeof (mdasC_sql_result_struct));

	myresult->result_count = 0;

        if ((status = srbGetDatasetInfo(hostTabPtr->conn, MDAS_CATALOG,
	 infoInp->objID, 
	  infoInp->collectionName, myresult, DEF_NUM_ROWS,
	   infoInp->accessMode)) < 0) {
            if (status != DATA_NOT_IN_CAT) {
                elog(NOTICE, 
	         "mdasGetInfo: Can't srbGetDatasetInfo obj %s, status = %d", 
	          infoInp->objID, status);
	    }
	    return status;
        }
    }

    if (myresult == NULL) {
        elog(NOTICE,
         "mdasGetInfo: get_dataset_info returns NULL result pointer");
	return SYS_ERR_MDAS_CAT_RET;
    }

    if (myresult->result_count <= 0) {
	free (myresult);
        elog(NOTICE,
         "mdasGetInfo: get_dataset_info returns no result ");
        return SYS_ERR_MDAS_CAT_RET;
    }

    /* A sanity check. Should not have more than one copy with "COPY=" */

    if (strstr(infoInp->objID, "&COPY=") != NULL && myresult->row_count > 1)  {
	elog(NOTICE,
         "mdasGetInfo: get_dataset_info returns > 1 row for a COPY= keywd");
        return SYS_ERR_MDAS_CAT_RET;
    }

    objID = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_NAME],
            dcs_aname[DATA_NAME]);
    resourceType = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[PHY_RSRC_TYP_NAME],
            dcs_aname[PHY_RSRC_TYP_NAME]);
    dataPath = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[PATH_NAME], 
	  dcs_aname[PATH_NAME]);
    resourceLoc = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[RSRC_ADDR_NETPREFIX], 
	  dcs_aname[RSRC_ADDR_NETPREFIX]);
    resourceName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[PHY_RSRC_NAME], 
	  dcs_aname[PHY_RSRC_NAME]);
    dataType = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, 
	    dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
    replNum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, 
	    dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
    offset = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, 
	    dcs_tname[OFFSET], dcs_aname[OFFSET]);
    size = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, 
	    dcs_tname[SIZE], dcs_aname[SIZE]);
    phyResClass = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, 
	    dcs_tname[RSRC_CLASS], dcs_aname[RSRC_CLASS]);
    containerName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, 
	    dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);

    isDirty = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, 
	    dcs_tname[IS_DIRTY], dcs_aname[IS_DIRTY]);

    chksum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, 
	    dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);

    if (objID == NULL || resourceType == NULL || dataPath == NULL || 
     resourceLoc == NULL || resourceName == NULL || dataType == NULL ||
      offset == NULL || size == NULL || phyResClass == NULL || 
       containerName == NULL || isDirty == NULL) {
	elog (NOTICE, "objID = %x, resourceType=%x, dataPath=%x, resourceLoc=%x, resourceName=%x, dataType=%x, replNum=%x, offset=%x, size=%x, phyResClass=%x, containerName=%x, isDirty=%x", 
	 objID, resourceType, dataPath, resourceLoc, resourceName, dataType,
	  replNum, offset, size, phyResClass, containerName, isDirty);
        freeSqlResult (myresult);
        elog(NOTICE,
         "mdasGetInfo: get_dataset_info returns NULL result ");
        return SYS_ERR_MDAS_CAT_RET;
    }
      
    *infoOutHead = infoOut = NULL;

    for (i = 0; i < myresult->row_count; i++) {
	char *tmpLoc;

    	objTypeInx = objTypeLookUp (&resourceType [i*MAX_DATA_SIZE]);
    	if (objTypeInx < 0) {
            freeSqlResult (myresult);
            freeInfoOut (*infoOutHead);
	    return (OBJ_ERR_RES_TYPE);
    	}

    	infoOut = (struct mdasInfoOut *) 
	    malloc (sizeof (struct mdasInfoOut));

	memset (infoOut, 0, sizeof (struct mdasInfoOut));

    	if (i == 0) {
	    *infoOutHead = infoOut;
    	} else  {
	    prevInfoOut->next = infoOut;
	}
	prevInfoOut = infoOut;
#ifdef foo      /* use fillInfoOut now */
	infoOut->objTypeInx = objTypeInx;

	infoOut->objID = strdup (&objID[i*MAX_DATA_SIZE]);
	infoOut->collection = strdup (infoInp->collectionName);

	infoOut->dataPath = strdup (&dataPath[i*MAX_DATA_SIZE]);

	infoOut->resourceLoc = strdup (&resourceLoc[i*MAX_DATA_SIZE]);

	infoOut->resourceName = strdup (&resourceName[i*MAX_DATA_SIZE]);

	infoOut->dataType = strdup (&dataType[i*MAX_DATA_SIZE]);

	infoOut->replNum = atoi (&replNum[i*MAX_DATA_SIZE]);

        /* XXXXXX need to get real segNum */
	infoOut->segNum = NO_INT_SEGMENTATION;

	infoOut->intReplNum = -1;
	infoOut->intSegNum = NO_INT_SEGMENTATION;

	infoOut->offset = strtoll (&offset[i*MAX_DATA_SIZE], 0, 0);

	infoOut->size = strtoll (&size[i*MAX_DATA_SIZE], 0, 0);

	infoOut->phyResClass = strdup (&phyResClass[i*MAX_DATA_SIZE]);

	infoOut->resClassInx = resClassLookup (infoOut->phyResClass);

	infoOut->containerName = strdup (&containerName[i*MAX_DATA_SIZE]);
	infoOut->containerMaxSize = 0;

	infoOut->isDirty = atoi (&isDirty[i*MAX_DATA_SIZE]);
	infoOut->chksum = atoi (&chksum[i*MAX_DATA_SIZE]);

	infoOut->logResName = NULL;	/* only in get_container_info */

	infoOut->tapeFileInfo = NULL;	/* only in get_container_info */
        status = fillInfoOut (infoOut, objID, resourceType,
         dataPath, resourceLoc, resourceName,
         dataType, replNum, offset, size, phyResClass,
         containerName, isDirty, infoInp->collectionName);
#endif  /* foo */

#ifdef FED_MCAT
	tmpLoc = getAddrWithZone (&resourceLoc[i*MAX_DATA_SIZE],
           infoInp->collectionName);
#else
	tmpLoc = &resourceLoc[i*MAX_DATA_SIZE];
#endif
        status = fillInfoOut (infoOut, &objID[i*MAX_DATA_SIZE],
         &resourceType[i*MAX_DATA_SIZE],
         &dataPath[i*MAX_DATA_SIZE],
	 tmpLoc,
	 &resourceName[i*MAX_DATA_SIZE],
         &dataType[i*MAX_DATA_SIZE], &replNum[i*MAX_DATA_SIZE],
         &offset[i*MAX_DATA_SIZE], &size[i*MAX_DATA_SIZE],
         &phyResClass[i*MAX_DATA_SIZE],
         &containerName[i*MAX_DATA_SIZE], &isDirty[i*MAX_DATA_SIZE],
	 &chksum[i*MAX_DATA_SIZE], infoInp->collectionName);
#ifdef FED_MCAT
	free (tmpLoc);
#endif
    }

    if (infoOut != NULL)
        infoOut->next = NULL;

    freeSqlResult (myresult);

    if (*infoOutHead == NULL) {
        return SYS_ERR_MDAS_CAT_RET;
    } else {
        return 0;
    }
}

int 
resClassLookup (char *resClassKeyWd) 
{
    int i;

    for (i = 0; i < nResClass; i++) {
        if (strcmp (resClassKeyWd, resClassEntries[i].keyWard) == 0)
            return i;
    }
    return -1;
}

/* domainLookUp - Using the input domain name, search the srbDomainEntry
 * table for a match. Returns the domainInx of the struct if a match is
 * found.
 * XXXXXXX - this routine is currently not used.
 *
 * Input : char *domainName - The domain name
 *
 * Output : The srbDomainEntry[i].domainInx if a match is found
 *	    -1 - no match 
 */

int 
domainLookUp (char *domainName)
{
    int i;

    for (i = 0; i < nSrbDomain; i++) {
	if (strstr (domainName, srbDomainEntry[i].domainName) != NULL)
	    return srbDomainEntry[i].domainInx;
    }
    return -1;
}

/* openFlagLookUp - Using the input openflag, search the srbAccessModeEntry
 * table for a match. Returns the Mdas accessMode if a match is
 * found.
 *
 * Input : int oflag - The open flag
 *
 * Output : The srbAccessModeEntry[i].accessMode if a match is found
 *          NULL - no match
 */

char *
openFlagLookUp (int oflag)
{
    int i;
 
    for (i = 0; i < nSrbAccessMode; i++) {
        if (oflag & srbAccessModeEntry[i].flag)
            return srbAccessModeEntry[i].accessMode;
    }
    return srbAccessModeEntry[0].accessMode;	/* O_RDONLY */
}

/* parseCondInput - Use the keywordInx, search the condInputEntry
 * table for the keyword. Use the keyward to find a match in the condInput
 * string. Return the valueInx of the inputValue struct .
 * found.
 *
 * Input : char *condInput - The replication Condition string.
 *	   int keywordInx - The keyword index.
 *
 * Output : The inputValue[i].valueInx if a match is found
 *          OBJ_ERR_REPL_COND - no match
 */

int
parseCondInput (char *condInput, int keywordInx, 
struct condResult *condResult)
{
    int i, c;
    char *tmpPtr = NULL;
    int matchKey = -1;

    condResult->strResult[0] = '\0';
    condResult->intResult = -1;
    for (i = 0; i < ncondInput; i++) {
	if (keywordInx != condInputEntry[i].keywordInx)
	    continue;
	tmpPtr = strstr (condInput, condInputEntry[i].keyword);
	if (tmpPtr != NULL) { 
	    matchKey = 0;
	    tmpPtr += strlen (condInputEntry[i].keyword);
	    /* Skip the blank preceeding the operator */
	    while ((c = (int) *tmpPtr) == ' ') {
		tmpPtr++;
	    }
	    if (c == '\0') {	/* no value, assume default value */
		tmpPtr = condInputEntry[i].defValue;
		break;
	    }
	    if (strncmp (tmpPtr, condInputEntry[i].operator, 
	      strlen (condInputEntry[i].operator)) == 0)
		tmpPtr += strlen (condInputEntry[i].operator);
            /* Skip the blank preceeding the operator */
            while ((c = (int) *tmpPtr) == ' ') {
                tmpPtr++;
            }
	    if (c == '\0') {	/* no value, assume default value */
		tmpPtr = condInputEntry[i].defValue;
                break;
            }
	    /* tmpPtr++;	XXXX don't understand this */
	    break;
	} else {
	    tmpPtr = condInputEntry[i].defValue;
            break;
	}
    }
    if (tmpPtr == NULL) {
	if (i >= ncondInput) {
	    condResult->intResult = OBJ_ERR_REPL_COND;
	    condResult->inputValue = NULL;
	    return (OBJ_ERR_REPL_COND);
	}
	tmpPtr = condInputEntry[i].defValue;
    }

    condResult->inputValue = matchCondInpValue (
     &condInputEntry[i], tmpPtr);
    if (condResult->inputValue == NULL) {  /* No match */
        /* May be an integer */
	if ((condResult->inputValue = matchCondInpValue (&condInputEntry[i],
	 INT_INPUT)) != NULL && isdigit (tmpPtr[0])) {	
	    /* Integer input is allowed */
            condResult->intResult = atoi (tmpPtr);
        } else if ((condResult->inputValue = matchCondInpValue (
	 &condInputEntry[i], STR_INPUT)) != NULL) {
	    /* A string input is allowed */
	    int condStrLen;
	    condStrLen = getCondStrLen (tmpPtr);
            if (condStrLen > 0) {
                strncpy (condResult->strResult, tmpPtr, condStrLen);
		condResult->strResult[condStrLen] = '\0';
            } else {
	    	condResult->intResult = 0;
	    }
	} else {
            condResult->intResult = OBJ_ERR_REPL_COND;
	}
    } else {
        condResult->intResult = condResult->inputValue->valueInx;
    }
    return (matchKey);
}

struct inputValue *matchCondInpValue (struct condInputMap *condInputMap,
char *valueStr)
{
int valueInx;

    for (valueInx = 0; valueInx < condInputMap->nReplValue; valueInx ++) {
        if (strncmp (valueStr, condInputMap->inputValue[valueInx].value,
         strlen (condInputMap->inputValue[valueInx].value)) == 0) {
            return (&condInputMap->inputValue[valueInx]);
        }
    }
    return (NULL);
}

/* svrGetDataDirInfo -
 * Input : struct varlena *vObjID - The object ID
 * Output : Returns a mdasC_sql_result_struct in (struct varlena) format.
 */

struct varlena *
svrGetDataDirInfo (int cat_type, struct varlena *vQval, 
struct varlena *vSelVal, int rowsWanted)
{
    mdasC_sql_result_struct  *myresult = NULL;
    struct varlena * retVal;
    char *qval;
    int *mySelval;
    int status;
    int i;
    char *mcatName;
#ifdef PORTNAME_c90
    struct int32Array *selval;
#else
    int *selval;
#endif
    int my_dcs_num;
 

    qval = varToStr (vQval);

    mySelval = (int *) malloc (MAX_DCS_NUM * sizeof (int));
    memset (mySelval, 0, MAX_DCS_NUM * sizeof (int));
#ifdef PORTNAME_c90
    selval = (struct int32Array *) varToStr (vSelVal);
    for (i = 0; i < MAX_DCS_NUM / 2; i++) {
        mySelval[2 * i] = sToL (ntohl (selval[i].myInt0));
        mySelval[2 * i + 1] = sToL (ntohl (selval[i].myInt1));
    }
    if ((MAX_DCS_NUM / 2 * 2) != MAX_DCS_NUM)         /* odd */
	mySelval[MAX_DCS_NUM - 1] = 
	    sToL (ntohl (selval[MAX_DCS_NUM / 2].myInt0));
#else
    my_dcs_num = VARSIZE (vSelVal) / SIZEOF32 - 1;
    if (my_dcs_num > MAX_DCS_NUM) {
	my_dcs_num = MAX_DCS_NUM;
    } else if (my_dcs_num < MAX_DCS_NUM) {
	char *myqval;

	/* client qval is shorter. have to pat it with zero */
	myqval = malloc (MAX_DCS_NUM * MAX_TOKEN);
	memset (myqval, 0, MAX_DCS_NUM * MAX_TOKEN);
	memcpy (myqval, qval, my_dcs_num * MAX_TOKEN);
	free (qval);
	qval = myqval;
    }
    selval = (int *) varToStr (vSelVal);
    for (i = 0; i < my_dcs_num; i++) {
        mySelval[i] = ntohl (selval[i]);
    }
#endif
    free (selval);

    if (cat_type > 0) {
	mcatName = strdup (&qval[cat_type * MAX_TOKEN]);
	qval[cat_type * MAX_TOKEN] = '\0';
    } else {
	mcatName = NULL;
    }

    /* Check with MDAS for the loid info */
 
    status = queryDataInfo (mcatName, mySelval, (char (*) [MAX_TOKEN]) qval, 
     &myresult, rowsWanted);

    if (mcatName != NULL)
	free (mcatName);
    
    if (status < 0) {
    	free (qval);
	free (mySelval);
	if (status != MCAT_INQUIRE_ERROR) {
	    elog (NOTICE,
	     "svrGetDataDirInfo: queryDataInfo error. status = %d", status); 
	}
        retVal = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (status);

        return retVal;
    }


    /* Encrypt the field called "dencrypted" if included */

    /* If this server is built without SRB_SECURE_COMM,
       sscEncryptString will return a fixed string instead
       of the encrypted value, protecting the transfer of
       the field */
    for (i = 0; i < myresult->result_count ; i++){
        if (strcmp(myresult->sqlresult[i].att_name,"dencrypted")==0) {
            char *cp;
            int j;
            char *cp_value;
            cp_value = (char *)myresult->sqlresult[i].values;
            for (j=0;j<myresult->row_count;j++) {
                cp = sscEncryptString(cp_value);
                strncpy(cp_value, cp, MAX_DATA_SIZE);
                cp_value += MAX_DATA_SIZE;
                free(cp);
            }
        }
    }

    /* Convert results to Var format */

    retVal = sqlResultToVar (myresult, MULTI_ROW);

    freeSqlResult (myresult);
    free (qval);
    free (mySelval);

    if (retVal != NULL) {
	return (retVal);
    }

errRet:
    retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    VARSIZE(retVal) = VAROUTHDRSZ;
    VAROUTSTAT(retVal) = htonl (status);

    return retVal;
}

int myResAttrLookup(char *mcatHostHint, int catType, char *resourceName,
                       char *domainName, char **resourceType,
                       char **resourceLoc)
{
  int i;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int selval[MAX_DCS_NUM];
  mdasC_sql_result_struct  *myresult = NULL;
  int status;
  char *tmpRsourceType, *tmpResourceLoc;    /* KESTOCK - mem leak fix */

  for (i = 0; i < MAX_DCS_NUM; i++)
         {
           sprintf(qval[i],"");
	   selval[i] = 0;
         }
  sprintf(qval[RSRC_NAME]," = '%s'",resourceName);
/*
  sprintf(qval[DOMAIN_DESC]," =  '%s'",domainName);
*/
  selval[RSRC_ADDR_NETPREFIX] = 1;
  selval[RSRC_TYP_NAME] = 1;

  status = queryDataInfo (mcatHostHint, selval, (char (*) [MAX_TOKEN]) qval, 
   &myresult, DEF_NUM_ROWS);

  if (status < 0)
    return status;

  tmpRsourceType = (char *)get_from_result_struct(
                  (mdasC_sql_result_struct *) myresult,
                  "MCAT.MDAS_TD_RSRC_TYP","rsrc_typ_name");
  tmpResourceLoc = (char *)get_from_result_struct(
                  (mdasC_sql_result_struct *) myresult,
                  "MCAT.MDAS_TD_LOCN","netprefix");

  *resourceType = strdup (tmpRsourceType);
  *resourceLoc = strdup (tmpResourceLoc);

  freeSqlResult (myresult);
  return(0);

}


int 
getCompoundResInfo (char *mcatHostHint, int catType, char *cmpdResName, 
struct resCache **resCacheInfo)
{
    int i, j;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  *myresult = NULL;
    int status;
    char *phyResType;
    char *phyResLoc;
    char *phyResName;
    char *phyDirPathName;
    char *logDirPathName;
    char *phyResClass;
    char *phyResMaxObjSize;
    struct mdasResInfo *tmpResInfo, *prevResInfo;
    int *randomArray;
    struct resCache *tmpResCache;
    struct hostElement  *hostTabPtr;
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, tmpResLoc[MAX_TOKEN];
#endif

    remoteFlag = getMdasEnabledHost (mcatHostHint, &hostTabPtr);

    if (remoteFlag < 0)
        return (remoteFlag);

    if (CompResCacheHead != NULL) {
        tmpResCache = CompResCacheHead;
        while (tmpResCache != NULL) {
            if (tmpResCache->mcatHostPtr == hostTabPtr &&
             strcmp (tmpResCache->resName, cmpdResName) == 0) {
                *resCacheInfo = tmpResCache;
                return (0);
            }
            tmpResCache = tmpResCache->next;
        }
    }


    for (i = 0; i < MAX_DCS_NUM; i++) {
	sprintf(qval[i],"");
	selval[i] = 0;
    }
    sprintf(qval[COMPOUND_RSRC_NAME]," = '%s'",cmpdResName);

    selval[PHY_RSRC_TYP_NAME] = 1;
    selval[RSRC_ADDR_NETPREFIX] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[PHY_RSRC_DEFAULT_PATH] = 1;
    selval[RSRC_DEFAULT_PATH] = 1;
    selval[RSRC_CLASS] = 1;
    selval[MAX_OBJ_SIZE] = 1;

    status = queryDataInfo (mcatHostHint, selval, (char (*) [MAX_TOKEN]) qval, 
      &myresult, DEF_NUM_ROWS);
   
    if (status < 0) 
	return (status);

    phyResType = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[PHY_RSRC_TYP_NAME], dcs_aname[PHY_RSRC_TYP_NAME]);
    phyResLoc = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[RSRC_ADDR_NETPREFIX], 
		    dcs_aname[RSRC_ADDR_NETPREFIX]);
    phyResName = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
    phyDirPathName = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[PHY_RSRC_DEFAULT_PATH], 
		    dcs_aname[PHY_RSRC_DEFAULT_PATH]);
    logDirPathName = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[RSRC_DEFAULT_PATH], dcs_aname[RSRC_DEFAULT_PATH]);
    phyResClass = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[RSRC_CLASS], dcs_aname[RSRC_CLASS]);
    phyResMaxObjSize = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[MAX_OBJ_SIZE], dcs_aname[MAX_OBJ_SIZE]);

    tmpResCache = malloc (sizeof (struct resCache));
    memset (tmpResCache, 0, sizeof (struct resCache));
    *resCacheInfo = tmpResCache;

    /* queue it */

    if (CompResCacheHead != NULL) {
        tmpResCache->next = CompResCacheHead;
    }
    CompResCacheHead = tmpResCache;
    tmpResCache->resName = strdup (cmpdResName);
    tmpResCache->mcatHostPtr = hostTabPtr;

    tmpResInfo = prevResInfo = NULL;

    getrandomArray (myresult->row_count, &randomArray);
    for (i = 0; i < myresult->row_count; i++) {

        tmpResInfo = (struct mdasResInfo *)
            malloc (sizeof (struct mdasResInfo));
	memset (tmpResInfo, 0, sizeof (struct mdasResInfo));
        if (i == 0) {
            tmpResCache->resInfoHead = tmpResInfo;
        } else  {
            prevResInfo->next = tmpResInfo;
        }
	j = randomArray[i];
        prevResInfo = tmpResInfo;
        tmpResInfo->phyResType = strdup (&phyResType[j*MAX_DATA_SIZE]);
#ifdef FED_MCAT
        tmpResInfo->phyResLoc = getAddrWithZone (&phyResLoc[j*MAX_DATA_SIZE],
	  mcatHostHint);
#else
	tmpResInfo->phyResLoc = strdup (&phyResLoc[j*MAX_DATA_SIZE]);
#endif
        tmpResInfo->phyResName = strdup (&phyResName[j*MAX_DATA_SIZE]);
        tmpResInfo->phyDirPathName = strdup (&phyDirPathName[j*MAX_DATA_SIZE]);
        tmpResInfo->logDirPathName = strdup (&logDirPathName[j*MAX_DATA_SIZE]);
        tmpResInfo->phyResClass = strdup (&phyResClass[j*MAX_DATA_SIZE]);
        tmpResInfo->phyResMaxObjSize = strtoll (&phyResMaxObjSize[j*MAX_DATA_SIZE],
	 0, 0);
#ifdef FED_MCAT
        remoteFlag = chkHostAndZone (tmpResInfo->phyResLoc, &hostTabPtr);
#else
        /* Parse the hostAddr */
        strcpy (tmpResLoc, tmpResInfo->phyResLoc);
        portNum = parseAddr (tmpResLoc);
        remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif
	tmpResInfo->flag = remoteFlag;
    }
    if (randomArray != NULL)
	free (randomArray);

    freeSqlResult (myresult);

    if (tmpResInfo != NULL) {
        tmpResInfo->next = NULL;
        return(0);
    } else {
        return (MCAT_INQUIRE_ERROR);
    }
}




int getResInfo (char *mcatHostHint, int catType, char *logResName, 
char *domainName, struct resCache **resCacheInfo)
{
    int i, j;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  *myresult = NULL;
    int status;
    char *phyResType;
    char *phyResLoc;
    char *phyResName;
    char *phyDirPathName;
    char *logDirPathName;
    char *phyResClass;
    char *phyResMaxObjSize;
    char *freeSpace;
    char *freeSpaceTime;
    struct mdasResInfo *tmpResInfo, *prevResInfo;
    int *randomArray;
    struct resCache *tmpResCache;
    struct hostElement  *hostTabPtr;
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, tmpStr[MAX_TOKEN];
#endif

    remoteFlag = getMdasEnabledHost (mcatHostHint, &hostTabPtr);

    if (remoteFlag < 0) 
	return (remoteFlag);

    if (ResCacheHead != NULL) {
	tmpResCache = ResCacheHead;
	while (tmpResCache != NULL) {
	    if (tmpResCache->mcatHostPtr == hostTabPtr &&
	     strcmp (tmpResCache->resName, logResName) == 0) {
		*resCacheInfo = tmpResCache;
		return (0);
	    }
	    tmpResCache = tmpResCache->next;
	}
    }

    if (remoteFlag == LOCAL_HOST) {
	/* try to match the physical res in LocalResInfo */

	tmpResInfo = matchLocalResInfo (logResName);
	if (tmpResInfo != NULL) {
	    /* have a match */
            tmpResCache = malloc (sizeof (struct resCache));
            memset (tmpResCache, 0, sizeof (struct resCache));
            *resCacheInfo = tmpResCache;

            /* queue it */

            if (ResCacheHead != NULL) {
                tmpResCache->next = ResCacheHead;
            }
            ResCacheHead = tmpResCache;
            tmpResCache->resName = strdup (logResName);
            tmpResCache->mcatHostPtr = hostTabPtr;
	    tmpResCache->resInfoHead = tmpResInfo;
	    return (0);
	}
    }

    for (i = 0; i < MAX_DCS_NUM; i++) {
	sprintf(qval[i],"");
	selval[i] = 0;
    }
    sprintf(qval[RSRC_NAME]," = '%s'",logResName);

    selval[PHY_RSRC_TYP_NAME] = 1;
    selval[RSRC_ADDR_NETPREFIX] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[PHY_RSRC_DEFAULT_PATH] = 1;
    selval[RSRC_DEFAULT_PATH] = 1;
    selval[RSRC_CLASS] = 1;
    selval[MAX_OBJ_SIZE] = 1;
    selval[FREE_SPACE] = 1;
    selval[FS_TIMESTAMP] = 1;


    status = queryDataInfo (mcatHostHint, selval, (char (*) [MAX_TOKEN]) qval, 
     &myresult, DEF_NUM_ROWS);
   
    if (status < 0) 
	return (status);

    phyResType = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[PHY_RSRC_TYP_NAME], dcs_aname[PHY_RSRC_TYP_NAME]);
    phyResLoc = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[RSRC_ADDR_NETPREFIX], 
		    dcs_aname[RSRC_ADDR_NETPREFIX]);
    phyResName = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
    phyDirPathName = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[PHY_RSRC_DEFAULT_PATH], 
		    dcs_aname[PHY_RSRC_DEFAULT_PATH]);
    logDirPathName = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[RSRC_DEFAULT_PATH], dcs_aname[RSRC_DEFAULT_PATH]);
    phyResClass = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[RSRC_CLASS], dcs_aname[RSRC_CLASS]);
    phyResMaxObjSize = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[MAX_OBJ_SIZE], dcs_aname[MAX_OBJ_SIZE]);
    freeSpace = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[FREE_SPACE], 
		  dcs_aname[FREE_SPACE]);
    freeSpaceTime = (char *)get_from_result_struct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[FS_TIMESTAMP], 
		  dcs_aname[FS_TIMESTAMP]);

    tmpResCache = malloc (sizeof (struct resCache));
    memset (tmpResCache, 0, sizeof (struct resCache));
    *resCacheInfo = tmpResCache;

    /* queue it */

    if (ResCacheHead != NULL) {
        tmpResCache->next = ResCacheHead;
    }
    ResCacheHead = tmpResCache;
    tmpResCache->resName = strdup (logResName);
    tmpResCache->mcatHostPtr = hostTabPtr;
    tmpResInfo = prevResInfo = NULL;

    getrandomArray (myresult->row_count, &randomArray);
    for (i = 0; i < myresult->row_count; i++) {

        tmpResInfo = (struct mdasResInfo *)
          malloc (sizeof (struct mdasResInfo));
	memset (tmpResInfo, 0, sizeof (struct mdasResInfo));

        if (i == 0) {
            tmpResCache->resInfoHead = tmpResInfo;
        } else  {
            prevResInfo->next = tmpResInfo;
        }
        j = randomArray[i];
        prevResInfo = tmpResInfo;
        tmpResInfo->phyResType = strdup (&phyResType[j*MAX_DATA_SIZE]);
        /* tmpResInfo->phyResLoc = strdup (&phyResLoc[j*MAX_DATA_SIZE]); */
#ifdef FED_MCAT
        tmpResInfo->phyResLoc = getAddrWithZone (&phyResLoc[j*MAX_DATA_SIZE],
	 mcatHostHint);
#else
        tmpResInfo->phyResLoc =  strdup (&phyResLoc[j*MAX_DATA_SIZE]);
#endif
        tmpResInfo->phyResName = strdup (&phyResName[j*MAX_DATA_SIZE]);
        if (strstr (tmpResInfo->phyResType, COMPOUND_RESC) != NULL) {
	    struct resCache *myCacheRes;

            status = getCompoundResInfo (mcatHostHint, catType, 
	     tmpResInfo->phyResName, &myCacheRes);
            if (status < 0) {
                elog (NOTICE, 
		 "getResInfo: getCompoundResInfo err for %s, status = %d",
                 tmpResInfo->phyResName, status);
		tmpResInfo->compoundLink = NULL;
            } else {
	        tmpResInfo->compoundLink = myCacheRes->resInfoHead;
	    }
        }
        tmpResInfo->phyDirPathName = strdup (&phyDirPathName[j*MAX_DATA_SIZE]);
        tmpResInfo->logDirPathName = strdup (&logDirPathName[j*MAX_DATA_SIZE]);
        tmpResInfo->phyResClass = strdup (&phyResClass[j*MAX_DATA_SIZE]);
        tmpResInfo->phyResMaxObjSize = 
	 strtoll (&phyResMaxObjSize[j*MAX_DATA_SIZE],
	 0, 0);
        tmpResInfo->freeSpace = atoi (&freeSpace[j*MAX_DATA_SIZE]);
        if (tmpResInfo->freeSpace > 0) {
            tmpResInfo->freeSpace *= MBYTES;
        }
        tmpResInfo->freeSpaceTime = atoi (&freeSpaceTime[j*MAX_DATA_SIZE]);

#ifdef FED_MCAT
        remoteFlag = chkHostAndZone (tmpResInfo->phyResLoc, &hostTabPtr);
#else
        /* Parse the hostAddr */
        strcpy (tmpStr, tmpResInfo->phyResLoc);
        portNum = parseAddr (tmpStr);
        remoteFlag = chkHost (tmpStr, portNum, &hostTabPtr);
#endif
	tmpResInfo->flag = remoteFlag;
    }
    if (randomArray != NULL)
        free (randomArray);

    freeSqlResult (myresult);

    if (tmpResInfo != NULL) {
        tmpResInfo->next = NULL;
        return(0);
    } else {
        return (-1);
    }
}

struct mdasResInfo *getResInfoByClass (struct mdasResInfo *resInfo, 
char *phyResClass) {
struct mdasResInfo *tmpResInfo;

    tmpResInfo = resInfo;

    while (tmpResInfo != NULL) {
	if (strcmp (tmpResInfo->phyResClass, phyResClass) == 0)
	    return (tmpResInfo);
	tmpResInfo = tmpResInfo->next;
    }
    return (NULL);
} 

/* svrRegisterDataset - Entry point for the srbRegisterDataset SID.
 *
 *
 * Input : int catType - The catalog type.
 *         struct varlena *vObjID - The object ID
 *         struct varlena *vDomainName - The domain name to which the object
 *         belongs. Valid domains - "sdsc".
 *         char *vDataTypeName - Data type. e.g. "generic"
 *         char *vResourceName - The storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         char *vCollectionName - The collection name.
 *         char *vPathName - The file/DB path of the data.
 *         srb_long_t* dataSize - The size of the dataset if known. 
			          0 = unknown.
 *
 *
 * Output : Returns 0 - success, negative  - failure
 */

int
svrRegisterDataset (int catType, struct varlena *vObjID, 
struct varlena *vDataTypeName, 
struct varlena *vResourceName, struct varlena *vCollectionName, 
struct varlena *vPathName, srb_long_t * dataSize)
{
    char *objID;
    char *dataTypeName;
    char *resourceName;
    char *collectionName;
    char *pathName;
    char *userName, *resourceType, *resourceLoc;
    int objInx, retVal;
    srb_long_t mySize;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    mySize = *dataSize;
    pathName = varToStr (vPathName);

    objID = varToStr (vObjID);
    dataTypeName = varToStr (vDataTypeName);
    resourceName = varToStr (vResourceName);
    collectionName = varToStr (vCollectionName);
    userName = ClientUser->userName;

    /* find the resourceType and resourceLoc associated with ResourceName
     * and domainName */

    if (myResAttrLookup (collectionName, MDAS_CATALOG, resourceName, 
     ClientUser->domainName,
      &resourceType, &resourceLoc) < 0) {
        elog (NOTICE,
          "svrRegisterDataset: %s and %s is not a registered resource",
          resourceName, ClientUser->domainName);
        retVal = OBJ_ERR_RES_NOT_REG;

	free (pathName);
	free (objID);
	free (dataTypeName);
	free (resourceName);
	free (collectionName);

	return retVal;
    }

    /* Call open at the next level */

    objInx = objTypeLookUp (resourceType);
    if (objInx < 0) {
        retVal = objInx;
	free (resourceType);
	free (resourceLoc);
        free (pathName);
        free (objID);
        free (dataTypeName);
        free (resourceName);
        free (collectionName);

        return retVal;
    }

    userName = ClientUser->userName;

    if ((retVal = ClientUser->privUserFlag) <= 0 &&
     getenv("ALLOW_NON_CHECKING_REGISTERY_OF_DATA") != NULL) {
	if (chkDataPerm (objInx, resourceLoc, pathName, O_RDWR, -1) < 0) {
            free (resourceType);
            free (resourceLoc);
            free (pathName);
            free (objID);
            free (dataTypeName);
            free (resourceName);
            free (collectionName);

            return retVal;
        }
    }

    /* Do the registration */

    retVal = _svrRegisterDataset (catType, objID, userName, DefAccess,
      ClientUser->domainName, dataTypeName, pathName,
      resourceName, collectionName, mySize, -1, NULL);

    free (resourceType);
    free (resourceLoc);

    free (pathName);
    free (objID);
    free (dataTypeName);
    free (resourceName);
    free (collectionName);

    return retVal;
}

int
svrRegDatasetInternal (int catType, struct varlena *vObjID, 
struct varlena *vUserName, struct varlena *vAccess, struct varlena *vDomainName,
struct varlena *vDataTypeName, 
struct varlena *vResourceName, struct varlena *vCollectionName, 
struct varlena *vPathName, srb_long_t * dataSize, 
int rmObjTypeInx, struct varlena *vRmObjResLoc)
{
    char *objID;
    char *dataTypeName;
    char *resourceName;
    char *collectionName;
    char *pathName;
    char *userName, *domainName, *access;
    char *rmObjResLoc;
    int retVal;
    srb_long_t mySize;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    if (ProxyUser->privUserFlag <= 0) {
        elog (NOTICE, 
	  "svrRegrDatasetInternal: User has no privilege for this operation");
        return (AUTH_ERR_PROXY_NOPRIV);
    }

    mySize = *dataSize;
    pathName = varToStr (vPathName);

    objID = varToStr (vObjID);
    dataTypeName = varToStr (vDataTypeName);
    resourceName = varToStr (vResourceName);
    collectionName = varToStr (vCollectionName);
    userName = varToStr (vUserName);
    domainName = varToStr (vDomainName);
    access =  varToStr (vAccess);
    rmObjResLoc = varToStr (vRmObjResLoc);

    /* Do the registration */

    retVal = _svrRegisterDataset (catType, objID, userName, access,
      domainName, dataTypeName, pathName,
      resourceName, collectionName, mySize, rmObjTypeInx, rmObjResLoc);

    free (pathName);
    free (objID);
    free (dataTypeName);
    free (resourceName);
    free (collectionName);
    free (userName);
    free (domainName);
    free (access);
    free (rmObjResLoc);

    return retVal;
}

int
_svrRegisterDataset (int catType, char *objID, char *userName, 
char *access, char *domainName, char *dataTypeName, char *pathName,
char *resourceName, char *collectionName, 
srb_long_t dataSize, int rmObjTypeInx, char *rmObjResLoc)
{
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = register_dataset_info (catType, objID, userName, access,
          domainName, dataTypeName, pathName,
          resourceName, collectionName, dataSize);

        if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        retVal = remoteRegisterDataset (catType, objID, userName, access,
          domainName, dataTypeName, pathName,
          resourceName, collectionName, dataSize, rmObjTypeInx, rmObjResLoc,
	  hostTabPtr);
    }

    if (retVal < 0 ) {
        /* don't print if COLLECTION_NOT_IN_CAT since Srsync does not
         * check for the existence of the collection */
        if (retVal != COLLECTION_NOT_IN_CAT &&
         retVal != DATA_NOT_IN_COLLECTION) {
            elog (NOTICE, 
	     "_svrRegisterDataset: Unable to reg obj %s with MDAS-%d",
	     objID, retVal);
	}
        if (rmObjTypeInx >= 0)
            _objUnlink (srbObjTypeEntry[rmObjTypeInx].objType,
              srbObjTypeEntry[rmObjTypeInx].systemType,
              rmObjResLoc, pathName, NO_CHKDIR);
    }

    return (retVal);
}

int
chkDataPerm (int tinx, char *resourceLoc, char *pathName, int oFlag, 
int chdirFlag)
{ 
    int retVal;
    char *thisResourceLoc, tmpResourceLoc[MAX_TOKEN];

    if (chdirFlag < 0) {
        if (srbObjTypeEntry[tinx].chdirFlag)
            chdirFlag = CHK_FILE_PATH;
        else
            chdirFlag = NO_CHKDIR;
    }

    if (strstr (resourceLoc, "&") != NULL) {
        thisResourceLoc = resourceLoc;
    } else {
        sprintf (tmpResourceLoc, "%s&%s", LocalMcat->serverAuthInfo->mcatName,
	  resourceLoc);
        thisResourceLoc = tmpResourceLoc;
    }

    switch (srbObjTypeEntry[tinx].objType) {
      case FILE_TYPE:
	/* This is not a real open at all. Just checking permission */
        retVal = _svrFileOpen (srbObjTypeEntry[tinx].systemType,
          thisResourceLoc, pathName, oFlag, 0640,
            chdirFlag, GENERIC_CONN);
	break;
      case DB_LOBJ_TYPE:
	/* cannot do _dbLobjOpen because it will hang if it is
         * called from a remote ObjCreate since there is a outstanding
         *_dbLobjCreate.
	 */

        retVal = 0;
	break;
      case DB_TABLE_TYPE:
      case MDRIVER_TYPE:
        retVal = 0;
        break;
      default:              /* shouldn't get here */
        elog(NOTICE, "chkDataPerm: error in srbObjTypeEntry mapping");
        retVal = OBJ_ERR_OBJ_TYPE;
    }
    return (retVal);
}

/* svrUnregisterDataset - Entry point for the srbUnregisterDataset SID.
 *
 * Input : struct varlena *vObjID - The object ID
 *         struct varlena *vDomainName - The domain name
 *
 * Output : Returns 0 - success, negative  - failure
 */

int
svrUnregisterDataset(struct varlena *vObjID,
struct varlena *vCollectionName)
{
    char *objID, *collectionName;
    int tinx, retVal, status;
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOut, *infoOutHead, *tmpInfoOut;
    char *tmpStr, *tmpStr1;
    int deleteFlag;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);

    if (strstr (objID, "&ADMIN=") != NULL) {
        if (ClientUser->privUserFlag != LOCAL_PRIV_USER) {
            elog (NOTICE,
              "svrUnregisterDataset: not a local admin user but the admin flag is on");
	    free (objID);
	    free (collectionName);
            return (AUTH_ERR_PROXY_NOPRIV);
        }
        deleteFlag = D_SU_DELETE_TRASH_ONE;
        infoInp.accessMode = "trash";
    } else {
        deleteFlag = D_DELETE_ONE;
        infoInp.accessMode = "all";
    }

    infoInp.domainName = ClientUser->domainName;
    infoInp.objID = objID;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = collectionName;

    /* Check with MDAS for the requested data */

    if ((retVal = mdasGetInfo (&infoInp, &infoOutHead)) < 0) {
	free (objID);
	free (collectionName);

	return (retVal);
    }

    infoOut = infoOutHead;
    while (infoOut != NULL) {

	/* check if it is in the vault. */

	tinx = infoOut->objTypeInx;
	if (ClientUser->privUserFlag != LOCAL_PRIV_USER) {
	    status = chkDataPerm  (tinx, infoOut->resourceLoc, 
	     infoOut->dataPath, O_RDWR, CHK_VAULT_PATH);

            if (status != SYS_ERR_FILE_NOT_IN_VAULT) {
		/* check whether the file exist */
		if (srbObjTypeEntry[tinx].objType == FILE_TYPE) {
		    struct srbStat statbuf;

		    status = _svrFileStat (srbObjTypeEntry[tinx].systemType,
		      infoOut->resourceLoc, NULL,
		      infoOut->dataPath, &statbuf);
		} else {
		    status = INP_ERR_UNREG_FILE_INVAULT;
		}
		if ((status != UNIX_ENOENT) && (status != HPSS_SRB_ENOENT)) {
                    elog (NOTICE,
                     "svrUnregisterDataset: Attempt to unreg file in vault");
                    if (retVal == 0)
                        retVal = INP_ERR_UNREG_FILE_INVAULT;
	            /* next one */
                    tmpInfoOut = infoOut->next;
                    freeInfoOut (infoOut);
                    infoOut = tmpInfoOut;       /* Try the next one */
	            continue;
		}
	    }
        }

        /* unregister the object */

        tmpStr = malloc (strlen (infoInp.objID) + 1);
        tmpStr1 = malloc (strlen (infoInp.objID) + 1);
        mySplitStr (objID, tmpStr, tmpStr1, '&');
        free (tmpStr1);

        status = _svrModifyDataset (MDAS_CATALOG, tmpStr,
          infoInp.collectionName, infoOut->resourceName, infoOut->dataPath,
          NULL, NULL, deleteFlag);
        free (tmpStr);

        if (status < 0) {
            elog (NOTICE, 
	     "svrUnregisterDataset: _svrModifyDataset error. status = %d",
              status);
            if (retVal == 0)
                retVal = status;
        }

        tmpInfoOut = infoOut->next;
        freeInfoOut (infoOut);
        infoOut = tmpInfoOut;       /* Try the next one */
    }


    free (objID);
    free (collectionName);

    return (retVal);
}

/* svrRegisterReplica - Entry point for the srbRegisterReplica SID.
 *
 *
 * Input : int catType - The catalog type.
 *         struct varlena *vObjID - The object ID
 *         char *vCollectionName - The collection name.
 *         char *vOrigResourceName - The original storage resource name.
 *              e.g. "mda18-unix-sdsc" (This entry can be NULL)
 *         char *vOrigPathName - The orighnal file/DB path of the data.
 *		(This entry can be NULL).
 *         char *vNewResourceName - The new storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         char *vNewPathName - The new file/DB path of the data.
 *         char *vUserName - The new file/DB path of the data.
 *         struct varlena *vDomainName - The domain name to which the object
 *         belongs. Valid domains - "sdsc".
 *
 *
 * Output : Returns 0 - success, negative  - failure
 */

int
svrRegisterReplica (int catType, struct varlena *vObjID,
struct varlena *vCollectionName,
struct varlena *vOrigResourceName, struct varlena *vOrigPathName,
struct varlena *vNewResourceName, struct varlena *vNewPathName,
struct varlena *vUserName, struct varlena *vDomainName)
{
    char *objID;
    char *collectionName;
    char *origResourceName;
    char *origPathName;
    char *newResourceName;
    char *newPathName;
    char *domainName, *userName;
    int retVal;


    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    origResourceName = varToStr (vOrigResourceName);
    origPathName = varToStr (vOrigPathName);
    newResourceName = varToStr (vNewResourceName);
    newPathName = varToStr (vNewPathName);
    domainName = varToStr (vDomainName);
    userName = ClientUser->userName;

    /* Do the registration */

    retVal = _svrRegisterReplica (catType, objID, collectionName,
      origResourceName, origPathName, newResourceName, newPathName,
      0, 0, NULL,
      userName, domainName);

    free (objID);
    free (collectionName);
    free (origResourceName);
    free (origPathName);
    free (newResourceName);
    free (newPathName);
    free (domainName);

    return retVal;
}

int
_svrRegisterReplica (int catType, char *objID, char *collectionName,
char *origResourceName, char *origPathName, 
char *newResourceName, char *newPathName,
int rmObjFlag, int objTypeInx, char *rmObjResLoc,
char *userName, char *domainName)
{
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = copy_dataset (catType, objID, collectionName,
          origResourceName, origPathName, newResourceName, newPathName,
            userName, domainName);
	/**RAJA June 20, 2003  changed == to >=  **/
        if (retVal >= 0) 
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        retVal = remoteRegisterReplica (catType, objID, collectionName,
          origResourceName, origPathName, newResourceName, newPathName,
            userName, domainName, hostTabPtr);
    }

    if (retVal < 0 ) {
        elog
         (NOTICE, "_svrRegisterReplica: Unable to reg replica %s with MDAS-%d",
	 objID, retVal);
	if (rmObjFlag > 0) 
            _objUnlink (srbObjTypeEntry[objTypeInx].objType,
              srbObjTypeEntry[rmObjFlag].systemType,
              rmObjResLoc, newPathName, NO_CHKDIR);
    }

    return retVal;
}

int
svrModifyDataset (int catType, struct varlena *vObjID, 
struct varlena *vCollectionName,
struct varlena *vResourceName, struct varlena *vPathName, 
struct varlena *vDataValue1, struct varlena *vDataValue2,
int retractionType)
{
    char *objID;
    char *collectionName;
    char *resourceName;
    char *pathName;
    char *dataValue1;
    char *dataValue2;
    int retVal;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    pathName = varToStr (vPathName);

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    resourceName = varToStr (vResourceName);
    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);

    retVal = _svrModifyDataset (catType, objID, collectionName,
      resourceName, pathName, dataValue1, dataValue2, retractionType);

    free (pathName);
    free (objID);
    free (collectionName);
    free (resourceName);
    free (dataValue1);
    free (dataValue2);

    return retVal;
}

int _svrModifyDataset (int catType, char *objID, char *collectionName,
char *resourceName, char *pathName, char *dataValue1, char *dataValue2,
int retractionType)
{
    char *userName;
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    userName = ClientUser->userName;

    /* Do the modify_dataset_info */

    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        char *cp;
        cp = dataValue1;
        if (retractionType == D_UPDATE_DENCRYPTED) {
#if defined(SRB_SECURE_COMM)
           cp = sscDecryptString(dataValue1);
#endif
        }

        retVal = modify_dataset_info (catType, objID, collectionName,
              pathName, resourceName,
	      cp, dataValue2, retractionType, userName,
	      ClientUser->domainName);
	if (retVal == 0) {
            commit_db2_interaction (MDAS_CAT_COMMIT);
	}
        if (retractionType == D_UPDATE_DENCRYPTED) {
#if defined(SRB_SECURE_COMM)
            free(cp);  /* free the buffer returned by sscDecryptString */
#endif
	}
#endif
    } else {
        retVal = remoteModifyDataset (catType, objID, collectionName,
          pathName, resourceName,
            dataValue1, dataValue2, retractionType, userName, hostTabPtr);
    }
    return retVal;
}

int 
svrRegisterContainer (int catType, struct varlena *vContainerName,
 struct varlena  *vLogResName, srb_long_t *containerSize)
{
    char *containerName;
    char *logResName;
    int retVal;
    srb_long_t mySize;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    mySize = *containerSize;
    containerName =  varToStr (vContainerName);
    logResName = varToStr (vLogResName);
    /* Do the registration */

    remoteFlag = getMdasEnabledHost (containerName, &hostTabPtr);

    if (remoteFlag < 0) {
        retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = register_container_info (catType, containerName, 
	 ClientUser->userName, ClientUser->domainName, 
	  logResName, mySize);

        if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        retVal = remoteRegisterContainer (catType, containerName, 
         ClientUser->userName, ClientUser->domainName, 
          logResName, mySize, hostTabPtr);
    }

    free (containerName);
    free (logResName);

    return (retVal);
}

/* svrRegisterInContDataset - Entry point for the svrRegisterInContDataset SID.
 *
 *
 * Input : int catType - The catalog type.
 *         struct varlena *vObjID - The object ID
 *         char *vCollectionName - The collection name.
 *         struct varlena *vContainerName - The container name.
 *         char *vDataType- Data type. e.g. "generic"
 *         srb_long_t *dataSize - The size of the dataset if known. 0 = unknown.
 *         srb_long_t *baseOffset - The offset of the dataset.
 *
 *
 * Output : Returns 0 - success, negative  - failure
 */

int
svrRegisterInContDataset (int catType, struct varlena *vObjID,
struct varlena *vCollectionName, struct varlena *vContainerName, 
struct varlena *vDataType, srb_long_t *dataSize, srb_long_t *baseOffset)
{
    char *objID;
    char *collectionName;
    char *containerName;
    char *dataType;
    int retVal;
    srb_long_t mySize, myOffset;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    containerName = varToStr (vContainerName);
    dataType= varToStr (vDataType);
    mySize = *dataSize;
    myOffset = *baseOffset;

    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);
    
    if (remoteFlag < 0) {
        retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = register_incontainer_dataset (catType, objID, collectionName,
         ClientUser->userName, ClientUser->domainName, containerName,
          dataType, mySize, myOffset);
        if (retVal >= 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        retVal = remoteRegisterInContDataset (catType, objID, collectionName,
         ClientUser->userName, ClientUser->domainName, containerName,
          dataType, mySize, myOffset, hostTabPtr);
    }

    free (objID);
    free (collectionName);
    free (containerName);
    free (dataType);

    return (retVal);
}

/* svrGetContainerInfo - Entry point for the srbGetContainerInfo SID.
 *
 *
 *         struct varlena *vContainerName - The collection name
 *         int rowsWanted - The number of rows wanted.
 *
 * Output : Returns a mdasC_sql_result_struct in (struct varlena) format.
 */

struct varlena *
svrGetContainerInfo (int catType, struct varlena *vContainerName,
int rowsWanted)
{
    mdasC_sql_result_struct  *myresult = NULL;
    struct varlena * retVal;
    char *containerName;
    char outContName[MAX_TOKEN], contCollection[MAX_TOKEN],
     fullContName[MAX_TOKEN];
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    containerName = varToStr (vContainerName);

    status = svrParseContainerName (containerName, outContName, contCollection,
     ClientUser->userName, ClientUser->domainName);
    if (status < 0) {
        elog (NOTICE,
         "svrGetContainerInfo: Error parsing container %s. status = %d",
          containerName, status);
        retVal = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (status);
        return (retVal);
    }
    free (containerName);

    sprintf (fullContName, "%s/%s", contCollection, outContName);

    remoteFlag = getMdasEnabledHost (fullContName, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        status = get_container_info (catType, fullContName,
         ClientUser->userName, ClientUser->domainName, &myresult, DEF_NUM_ROWS);
#endif
    } else {

        status = remoteGetContainerInfo (catType, fullContName,
         ClientUser->userName, ClientUser->domainName, &myresult, DEF_NUM_ROWS,
	 hostTabPtr);
    }

    if (myresult == NULL) {
        retVal = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (status);
        return (retVal);
    } else if (status < 0) {
	free (myresult);
        retVal = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (status);
        return (retVal);
    }

    /* Convert results to Var format */

    retVal = sqlResultToVar (myresult, MULTI_ROW);

    freeSqlResult (myresult);
    if (retVal != NULL) {
        return (retVal);
    }

    retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    VARSIZE(retVal) = VAROUTHDRSZ;
    VAROUTSTAT(retVal) = htonl (status);

    return retVal;
}

/* svrGetResOnChoice - Given the Logigical resource name and the inputFlag,
 * return a physical resource name.
 * Input - int catType - catalog type - 0 - MCAT
 *	   struct varlena *vLogResName - The logical resource name.
 *	   struct varlena *vInputFlag - The Input flag, valid inputs are:
 *	    "RR" - Round Robin.
 *	    "RANDOM" - randomly selecting a physical resource from the
 *		       input logical resource.
 * Output - The Physical resource string.
 */     
struct varlena *
svrGetResOnChoice (int catType, struct varlena *vLogResName,
struct varlena *vInputFlag)
{
    struct varlena * retVal;
    char *logResName, *inputFlag;
    char phyResName[MAX_TOKEN];
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    logResName = varToStr (vLogResName);
    inputFlag = varToStr (vInputFlag);

    /* XXXXXX - potential problem. Does not know the mcat name */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        status = getResOnChoice (logResName, phyResName, inputFlag);
#endif
    } else {
        status = remoteGetResOnChoice (catType, logResName, phyResName, 
	 inputFlag, hostTabPtr);
    }

    if (status < 0) {
    	retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    	VARSIZE(retVal) = VAROUTHDRSZ;
    	VAROUTSTAT(retVal) = htonl (status);
    } else {
	int len;
	
	len = strlen (phyResName) + 1;
	retVal = malloc (VAROUTHDRSZ + len + 1);
        VARSIZE(retVal) = len + VAROUTHDRSZ;
	strcpy (VAROUTDATA(retVal), phyResName);
        VAROUTSTAT(retVal) = htonl (0);
    }
    return retVal;
}

struct mdasInfoOut *
selectInfoOutByResc (char *resourceName, struct mdasInfoOut **infoOutHead)
{
    struct mdasInfoOut *tmpInfoOut, *prevInfoOut = NULL;

    tmpInfoOut = *infoOutHead;
    if (tmpInfoOut == NULL) 
        return NULL;

    while (tmpInfoOut != NULL) {
	if (strcmp (tmpInfoOut->resourceName, resourceName) == 0) {
	    if (prevInfoOut == NULL) {	/* first time */
		*infoOutHead = tmpInfoOut->next;
	    } else {
		prevInfoOut->next = tmpInfoOut->next;
	    }
	    tmpInfoOut->next = NULL;
	    break;
	}
	prevInfoOut = tmpInfoOut;
	tmpInfoOut = tmpInfoOut->next;
    }
    return (tmpInfoOut);
}
  
int
matchRescWithContResult (char *resourceName, mdasC_sql_result_struct *myresult)
{
    int i;

    char *phyResName;

    phyResName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);

    for (i = 0; i < myresult->row_count; i++) {
	if (strcmp (resourceName, &phyResName[i*MAX_DATA_SIZE]) == 0) {
	    return 1;
	}
    }
    return (0);
}

char *chkContColl (char *collectionName)
{
    int i;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  *myresult = NULL;
    int status;
    char *myCollCont;

    for (i = 0; i < MAX_DCS_NUM; i++) {
    	sprintf(qval[i],"");
        selval[i] = 0;
    }

    sprintf(qval[COLLECTION_NAME]," = '%s'", collectionName);
    selval[CONTAINER_FOR_COLLECTION] = 1;

    status = queryDataInfo (collectionName, selval, (char (*) [MAX_TOKEN]) qval,
     &myresult, DEF_NUM_ROWS);

    if (status < 0)
    	return NULL;

    myCollCont = (char *)get_from_result_struct(
                  (mdasC_sql_result_struct *) myresult,
                  "MCAT.MDAS_TD_COLLCONT","container_name");
    free (myresult);
    return(myCollCont);
}

int 
resolveInfoPathName (char *inpObjID, char *collectionName, 
struct mdasInfoOut *infoOutHead)
{
    struct mdasInfoOut *infoOut;
    char *objID, *condition;

    if (!interPathCheck (inpObjID)) 
	return 0;

    objID = malloc (strlen (inpObjID) + 1);
    condition = malloc (strlen (inpObjID) + 1);

    if (objID == NULL || condition == NULL) {
        elog (NOTICE, "resolveInfoPathName: malloc error");
        return SYS_ERR_MALLOC;
    }

    infoOut = infoOutHead;
    while (infoOut != NULL) {
        char *tempStr;
        int status;

        if (mySplitStr (inpObjID, objID, condition + 1, '&') != NULL){
            condition[0] = '&';     /* put back the & */
            tempStr = infoOut->dataPath;
            infoOut->dataPath = malloc (MAX_TOKEN * 2);
            status = getFullPathName (tempStr, "",
              ClientUser->userName, ClientUser->domainName,
              condition, objID, collectionName, infoOut->dataPath);
            if (status < 0) {       /* Unable to interprete the pathname */
                elog (NOTICE, 
		 "resolveInfoPathName: getFullPathName of %s err status=%d",
		   inpObjID, status);
                /* Use the original path */
                free (infoOut->dataPath);
                infoOut->dataPath = tempStr;
            } else {
                free (tempStr);
            }
        }
        infoOut = infoOut->next;
    }
    free (objID);
    free (condition);

    return 0;
}

int 
matchRescWithInfo (struct mdasResInfo *resInfo, 
struct mdasInfoOut *infoOutHead)
{
    struct mdasInfoOut *tmpInfoOut;
    struct mdasResInfo *tmpResInfo;

    tmpResInfo = resInfo;
  
    while (tmpResInfo != NULL) {
	tmpInfoOut = infoOutHead;
	while (tmpInfoOut != NULL) {
	    if (strcmp 
             (tmpInfoOut->resourceName, tmpResInfo->phyResName) == 0) {
		tmpResInfo->phyResMaxObjSize = MATCHED_INFO;
		break;
	    }
	    tmpInfoOut = tmpInfoOut->next;
	}
	tmpResInfo = tmpResInfo->next;
    }
    return (0);
}

int
objCntIncontainer (char *fullContName)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int i , selval[MAX_DCS_NUM];
    mdasC_sql_result_struct *myresult = NULL;
    char *count;
    int retVal;

    for (i = 0; i < MAX_DCS_NUM; i++)  {
      sprintf(qval[i],"");
      selval[i] = 0;
    }

    sprintf(qval[CONTAINER_NAME]," = '%s'",fullContName);


    selval[DATA_ID] = 9;
    selval[CONTAINER_NAME] = 1;

  retVal = queryDataInfo (fullContName, selval, (char (*) [MAX_TOKEN]) qval, 
   &myresult, DEF_NUM_ROWS);

    if (retVal < 0)
        return retVal;

    count = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_ID], dcs_aname[DATA_ID]);


    retVal = atoi (count) - 1;
    freeSqlResult (myresult);

    return (retVal);
}

/* isResLocLocal - check whether the input resource loc is local
 * return 1 if it is local, otherwise zero
 */

int
isResLocLocal (char *resourceLoc)
{
    int remoteFlag;
    struct hostElement  *hostTabPtr;
#ifndef FED_MCAT
    char tmpResLoc[MAX_TOKEN];
    char *portNum;
#endif

    if (resourceLoc == NULL)
	return (0);
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (resourceLoc, &hostTabPtr);
#else
    strcpy (tmpResLoc, resourceLoc);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif
    if (remoteFlag < 0 || remoteFlag == REMOTE_HOST)
	return 0;
    else
	return 1;
}

int
checkCommonPathPermissionFromResultStruct(int catType, 
			  char *userName,
			  char *domainName,
			  char *rsrcName,
			  mdasC_sql_result_struct *inDataInfo)
{
   int retVal;
   char *commonPath  = NULL;
   char *pathNameSet = NULL;

   /* XXXXX changed by MW to take out hardwired index
   pathNameSet = (char *) inDataInfo->sqlresult[4].values;
   */
   pathNameSet = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inDataInfo,
            dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);

   if (pathNameSet == NULL) {
     elog (NOTICE, "checkCommonPathPermissionFromResultStruct: PATH_NAME in inDataInfo not defined");
     return (INP_ERR_FILENAME);
   }
   getCommonPathFromSqlResultStruct(pathNameSet,inDataInfo->row_count,
				    &commonPath);
   retVal = checkPermInPath(catType,userName,domainName,rsrcName,commonPath);
   free(commonPath);
   return(retVal);
}

int
getCommonPathFromSqlResultStruct(char *pathNameSet, int count,
				 char **commonPath)
{
    int i;
    for (i = 0; i < count; i++) {
      if (*commonPath == NULL) {
	*commonPath = strdup (pathNameSet);
      } else {
	char *tmpstr1, *tmpstr2, *lastslash = NULL;
	int c1, c2;
	int len = 0;
	
	tmpstr1 = *commonPath;
	tmpstr2 = pathNameSet;
	while ((c1 = *tmpstr1) == (c2 = *tmpstr2)) {
	  if (c1 == '\0')
	    break;
	  if (c1 == '/') {
	    lastslash = tmpstr1;
	  }
	  len ++;
	  tmpstr1++;
	  tmpstr2++;
	  continue;
	}
	if (c1 != c2) {
	  if (c2 == '\0') {       /* c2 is short */
	    if (c1 == '/') {
	      *tmpstr1 = '\0';
	    } else {
	      if (lastslash == *commonPath) /* root */
		lastslash ++;
	      *lastslash = '\0';
	    }
	  } else if (c1 != '\0' || c2 != '/') {
	    if (lastslash == *commonPath) /* root */
	      lastslash ++;
	    *lastslash = '\0';
	  }
	}
      }
      pathNameSet += MAX_DATA_SIZE;
    }
    return (0);
}

int
checkPermInPath(int catType, 
		char *userName,
		char *domainName,
		char *rsrcName,
		char *pathName)
{
  int objInx, retVal;
  char *resourceType, *resourceLoc;
  
  if (ClientUser->privUserFlag == 1)
    return 0;
  
  /* XXXXX use the local mcatname for now */
  if (myResAttrLookup (NULL, MDAS_CATALOG, rsrcName, ClientUser->domainName,
		       &resourceType, &resourceLoc) < 0) {
    elog (NOTICE, "checkPermInPath: %s is not a registered resource",
          rsrcName);
    retVal = OBJ_ERR_RES_NOT_REG;
  }
  objInx = objTypeLookUp (resourceType);
  if (objInx < 0) {
        retVal = objInx;
	free (resourceLoc);
	free (resourceType);
        return retVal;
    }

  if ((retVal = chkDataPerm (objInx, resourceLoc, pathName, O_RDWR, -1))
      < 0) {
    elog (NOTICE, "checkPermInPath: Path %s in Resource %s does not exist or user has no permission",pathName, rsrcName);
  }
  free (resourceLoc);
  free (resourceType);
  return retVal;;
  
}

int fillInfoOut (struct mdasInfoOut *infoOut, char *objID, char *resourceType, 
char *dataPath, char *resourceLoc, char *resourceName,
char *dataType, char *replNum, char *offset, char *size, char *phyResClass, 
char *containerName, char *isDirty, char *chksum, char *collection)
{
	int objTypeInx;

        objTypeInx = objTypeLookUp (resourceType);
        if (objTypeInx < 0) {
            return (OBJ_ERR_RES_TYPE);
        }

        infoOut->objTypeInx = objTypeInx;

        infoOut->objID = strdup (objID);

        infoOut->collection = strdup (collection);

        infoOut->dataPath = strdup (dataPath);

        infoOut->resourceLoc = strdup (resourceLoc);

        infoOut->resourceName = strdup (resourceName);

        infoOut->dataType = strdup (dataType);

        infoOut->replNum = atoi (replNum);

        /* XXXXXX need to get real segNum */
        infoOut->segNum = NO_INT_SEGMENTATION;

        infoOut->intReplNum = -1;
        infoOut->intSegNum = NO_INT_SEGMENTATION;

        infoOut->offset = strtoll (offset, 0, 0);

        infoOut->size = strtoll (size, 0, 0);

        infoOut->phyResClass = strdup (phyResClass);

        infoOut->resClassInx = resClassLookup (infoOut->phyResClass);

        infoOut->containerName = strdup (containerName);
        infoOut->containerMaxSize = 0;

        infoOut->isDirty = atoi (isDirty);
#ifdef foo
	/* XXXXX this is a temp fix for ticketuser. Have to bug 
	 * Raja to fix this */
	if ((unsigned long) chksum < MAX_DATA_SIZE * 20) 
	    infoOut->chksum = 0;
	else    
#endif
        infoOut->chksum = atoi (chksum);

        infoOut->logResName = NULL;     /* only in get_container_info */

        infoOut->tapeFileInfo = NULL;   /* only in get_container_info */

        return (0);
}

int
sortResForIngest (struct resCache *myCacheRes, srb_long_t dataSize)
{
    struct mdasResInfo *priCacheResInfo, *priArchResInfo,
     *cacheResInfo, *archResInfo;
    struct mdasResInfo *vacantCacheResInfo, *filledCacheResInfo, 
     *vacantArchResInfo, *filledArchResInfo;
    struct mdasResInfo *tmpResInfo;
    int vacantCnt = 0;
    srb_long_t mySize;
    time_t curTime;

    if (myCacheRes == NULL || myCacheRes->resInfoHead == NULL ||
     myCacheRes->resInfoHead->next == NULL) {
	return (0);
    }

    if (dataSize >= 0) {
        mySize = dataSize;
    } else {
        mySize = ASSUMED_SIZE;
    }

    curTime = time (0);
    sortResByClass (myCacheRes->resInfoHead, &priCacheResInfo, 
     &priArchResInfo, &cacheResInfo, &archResInfo);

#ifdef foo
    /* order - pri cache, cache, pri arch, arch */
    combineResInfo (&priCacheResInfo, &cacheResInfo);
    combineResInfo (&priCacheResInfo, &priArchResInfo);
    combineResInfo (&priCacheResInfo, &archResInfo);
    myCacheRes->resInfoHead = priCacheResInfo;
#endif
    /* order - pri cache, cache, pri arch, arch */
    combineResInfo (&priCacheResInfo, &cacheResInfo);
    combineResInfo (&priArchResInfo, &archResInfo);
    /* check the freeSpace */
    tmpResInfo = priCacheResInfo;
    while (tmpResInfo != NULL) {
	srb_long_t myFreeSpace;

	if (vacantCnt <= 0) {
	    myFreeSpace = getAndRegResSpace (tmpResInfo, curTime);
	    if (myFreeSpace >= SIZE_FACTOR * mySize) {
		vacantCnt ++;
	    }
	} else {
	    if (curTime - tmpResInfo->freeSpaceTime >= 
	     RES_FREE_SPACE_CHK_TIME) {
		/* invalidate the freespace */
		tmpResInfo->freeSpace = RES_NO_SPACE_DATA;
	    } else if (tmpResInfo->freeSpace >= SIZE_FACTOR * mySize) {
		vacantCnt ++;
	    }
	}
	tmpResInfo = tmpResInfo->next;
    }

    sortResBySpace (priCacheResInfo, &vacantCacheResInfo, &filledCacheResInfo,
     mySize);

    tmpResInfo = priArchResInfo;
    while (tmpResInfo != NULL) {
        srb_long_t myFreeSpace;

        if (vacantCnt <= 0) {
            myFreeSpace = getAndRegResSpace (tmpResInfo, curTime);
            if (myFreeSpace >= SIZE_FACTOR * mySize) {
                vacantCnt ++;
            }
        } else {
            if (curTime - tmpResInfo->freeSpaceTime >= 
             RES_FREE_SPACE_CHK_TIME) {
                /* invalidate the freespace */
                tmpResInfo->freeSpace = RES_NO_SPACE_DATA;
            }
        }
        tmpResInfo = tmpResInfo->next;
    }

    sortResBySpace (priArchResInfo, &vacantArchResInfo, &filledArchResInfo,
     mySize);
    combineResInfo (&vacantCacheResInfo, &vacantArchResInfo);
    combineResInfo (&vacantCacheResInfo, &filledCacheResInfo);
    combineResInfo (&vacantCacheResInfo, &filledArchResInfo);

    myCacheRes->resInfoHead = vacantCacheResInfo;

    return (0);
}

int
sortResBySpace (struct mdasResInfo *resInfoHead,
struct mdasResInfo **ovacantResInfo, struct mdasResInfo **ofilledResInfo,
srb_long_t dataSize)
{
    struct mdasResInfo *vacantResInfo, *filledResInfo, *almostFilledResInfo;
    struct mdasResInfo *localVacantResInfo, *localFilledResInfo, 
     *localAlmostFilledResInfo;
    struct mdasResInfo *tmpResInfo, *nextResInfo;
    srb_long_t mySize;
    srb_long_t freeSpace;

    vacantResInfo = filledResInfo = almostFilledResInfo = 
     localVacantResInfo = localFilledResInfo = localAlmostFilledResInfo = NULL;

    tmpResInfo = resInfoHead;
    while (tmpResInfo != NULL) {

	if (dataSize >= 0) {
	    mySize = dataSize;
	} else {
	    mySize = ASSUMED_SIZE;
	}

	nextResInfo = tmpResInfo->next;
	if (tmpResInfo->freeSpace == RES_RESOURCE_DELETED) {
	    /* unque RES_RESOURCE_DELETED */
	    tmpResInfo = nextResInfo;
	    continue;
	} else if (tmpResInfo->freeSpace == RES_NO_SPACE_DATA) {
	    /* asuume 3 times */
	    freeSpace = 3 * mySize;
	} else {
	    freeSpace = tmpResInfo->freeSpace;
	}

        if (freeSpace >= SIZE_FACTOR * mySize) {
	    if (tmpResInfo->flag == LOCAL_HOST) {
		queResInfo (&localVacantResInfo, tmpResInfo, 
		 QUE_BOTTOM);
	    } else {
		queResInfo (&vacantResInfo, tmpResInfo, 
                 QUE_BOTTOM);
	    }
	} else if (freeSpace >= 2 * mySize) {
#ifdef foo	/* low on space. local/remote no longer matter */
            if (tmpResInfo->flag == LOCAL_HOST) {
                queResInfo (&localAlmostFilledResInfo, tmpResInfo, 
                 QUE_BY_FREE_SPACE);
            } else
#endif
                queResInfo (&almostFilledResInfo, tmpResInfo, 
                 QUE_BY_FREE_SPACE);
	} else {
#ifdef foo	/* low on space. local/remote no longer matter */
            if (tmpResInfo->flag == LOCAL_HOST) {
                queResInfo (&localFilledResInfo, tmpResInfo, 
                 QUE_BY_FREE_SPACE);
            } else
#endif
                queResInfo (&filledResInfo, tmpResInfo, 
                 QUE_BY_FREE_SPACE);
	}
	tmpResInfo = nextResInfo;
    }

    /* combine the vacant */
    combineResInfo (&localVacantResInfo, &vacantResInfo);
    combineResInfo (&localVacantResInfo, &localAlmostFilledResInfo);
    combineResInfo (&localVacantResInfo, &almostFilledResInfo);

   /* combine the filled */

    combineResInfo (&localFilledResInfo, &filledResInfo);

    *ovacantResInfo = localVacantResInfo;
    *ofilledResInfo = localFilledResInfo;

    return (0);
}

struct mdasResInfo *
matchLocalResInfo (char *resName)
{
    struct mdasResInfo *tmpResInfo, *prevResInfo;

    tmpResInfo = LocalResInfo;
    prevResInfo = NULL;
    while (tmpResInfo != NULL) {
	if (strcmp (resName, tmpResInfo->phyResName) == 0) {
	    /* we have a match. unqueue it  */
	    if (strstr (tmpResInfo->phyResType, COMPOUND_RESC) != NULL) {
		/* don't do compound res */
		return (NULL);
	    }
	    if (prevResInfo == NULL) {
		LocalResInfo = tmpResInfo->next;
	    } else {
		prevResInfo->next = tmpResInfo->next;
	    }
	    tmpResInfo->next = NULL;
	    return (tmpResInfo);
	}
	prevResInfo = tmpResInfo;
	tmpResInfo = tmpResInfo->next;
    }
    return (NULL);
}

srb_long_t
getAndRegResSpace (struct mdasResInfo *myResInfo, time_t curTime)
{
    int tinx;
    srb_long_t myFreeSpace;
    int status;
    int sizeMB;

    tinx = objTypeLookUp (myResInfo->phyResType);
    if (srbObjTypeEntry[tinx].systemType != UNIX_FT) {
	myResInfo->freeSpace = RES_NO_SPACE_DATA;
	return (RES_NO_SPACE_DATA);
    }
    if (curTime - myResInfo->freeSpaceTime >= RES_FREE_SPACE_CHK_TIME &&
     myResInfo->freeSpace != RES_RESOURCE_DELETED) {
	char tmpStr[MAX_TOKEN];
	char tmpStr1[MAX_TOKEN];
	char mcatName[MAX_TOKEN], tmpAddr[MAX_TOKEN];
	char *myMcatName;

	myFreeSpace = _svrGetFsFreeSpace (srbObjTypeEntry[tinx].systemType,
	 myResInfo->phyResLoc, myResInfo->phyDirPathName, 0);
	myResInfo->freeSpaceTime = curTime;
	if (myFreeSpace < 0) {
	    myResInfo->freeSpace = sizeMB = RES_RESOURCE_DOWN;
	} else if (myFreeSpace == 0) {
	    myResInfo->freeSpace = sizeMB = RES_NO_SPACE;
	} else {
	    myResInfo->freeSpace = myFreeSpace;
	    sizeMB = myFreeSpace / MBYTES;
	}
	/* register this */
        sprintf (tmpStr, "%d", curTime);
        sprintf (tmpStr1, "%d", sizeMB);
	if (mySplitStr (myResInfo->phyResLoc, mcatName, tmpAddr, '&') == NULL) {
	    myMcatName = NULL;
	} else {
	    myMcatName = mcatName;
	}

        status = _svrModifyRescInfo (MDAS_CATALOG, myMcatName, 
	 myResInfo->phyResName, R_INSERT_FREE_SPACE, tmpStr1, tmpStr, 
	 NULL, NULL);
	if (status < 0) {
           elog (NOTICE, 
	    "getAndRegResSpace: _svrModifyRescInfo failed, status = %d\n", 
	    status);
	}
    }
    return (myResInfo->freeSpace);
}

/* Parse the condition input keyword COPIES (valid values are an integer, "all"
"RR" (round robin) and "RANDOM", together with the logResName input, produce
out put in myCondResult and myResInfo. myResInfo gives a list of resources
that sorted by the order according to the keyword.
*/

int
parseCondResInfo (char *mcatHostHint, int catType, char *condInput,
char *logResName, struct condResult *condResult,
struct resCache **myCacheRes, srb_long_t dataSize)
{
    struct mdasResInfo *resInfo, *tmpResInfo, *prevResInfo;
#ifdef foo
    struct mdasResInfo *priCacheResInfo, *priArchResInfo,
     *cacheResInfo, *archResInfo;
#endif
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    int status;
   
    parseCondInput (condInput, COPIES, condResult);

    if (condResult->intResult < 0) {
        elog (NOTICE, "Problem with parseCondInput() for %s, status = %d",
         condInput, condResult->intResult);
        return (condResult->intResult);
    }

    /* get the mdasResInfo associated with a logResName and domainName */

    if (getResInfo (mcatHostHint, catType, logResName, ClientUser->domainName,
     myCacheRes) < 0) {
        elog (NOTICE,
          "parseCondResInfo: Input resource %s is not a registered resource",
          logResName);
        return (OBJ_ERR_RES_NOT_REG);
    }

    sortResForIngest (*myCacheRes, dataSize);

    resInfo = (*myCacheRes)->resInfoHead;
#ifdef foo
    sortResByClass (resInfo, &priCacheResInfo, &priArchResInfo,
     &cacheResInfo, &archResInfo);

    /* order - pri cache, cache, pri arch, arch */
    combineResInfo (&priCacheResInfo, &cacheResInfo);
    combineResInfo (&priCacheResInfo, &priArchResInfo);
    combineResInfo (&priCacheResInfo, &archResInfo);
    resInfo = priCacheResInfo;
#endif

    switch (condResult->intResult) {
        case RR:
#ifdef foo      /* already random */
        case RANDOM:
#endif
            remoteFlag = getMdasEnabledHost (mcatHostHint, &hostTabPtr);

            if (remoteFlag < 0) {
                status = remoteFlag;
            } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS

                status = getResOnChoice (logResName, condResult->strResult,
                 condResult->inputValue->value);
#endif
            } else {
                status = remoteGetResOnChoice (catType, logResName,
                 condResult->strResult, condResult->inputValue->value,
                 hostTabPtr);
            }

            if (status < 0) {
                elog (NOTICE,
                 "Unable to getresOnChoice(), logRes = %s, status = %d",
                  logResName, status);
                return status;
            }
            /* put resource on top */
            tmpResInfo = resInfo;
            prevResInfo = NULL;
            while (tmpResInfo != NULL) {
                if (strcmp (tmpResInfo->phyResName,
                 condResult->strResult) == 0) {
                    if (strcmp (tmpResInfo->phyResName,
                     resInfo->phyResName) == 0) { /* already on top */
                        break;
                    }
                    prevResInfo->next = tmpResInfo->next;
                    tmpResInfo->next = resInfo;
                    resInfo = tmpResInfo;
                    break;
                }
                prevResInfo = tmpResInfo;
                tmpResInfo = tmpResInfo->next;
            }
            condResult->intResult = 1;
            break;
        default:
            strcpy (condResult->strResult, logResName);
            break;
    }
    (*myCacheRes)->resInfoHead = resInfo;
    return (0);
}

/* svrGetMoreRows - Entry point for the srbGetMoreRows SID.
 *
 *
 * Input : int catalog - The catalog type. e.g., MDAS_CATALOG
 *         int contDesc - The continuation descriptor. This is a non negative
 *         integer returned from a srbGetDatasetInfo, srbGetDataDirInfo,
 *         srbListCollect or srbGetPrivUsers call as
 *         myresult->continuation_index.
 *         int rowsWanted - No. or rows to be returned.
 *
 * Output : success - Returns the content in struct varlena
 *          failure - Returns NULL string.
 */

struct varlena *
svrGetMoreRows (int catalog, int contDesc, int rowsWanted)
{
    struct varlena *retval;
    int status;
    mdasC_sql_result_struct  *myresult = NULL;

    /* XXXX this is a temp fix to get around overflow in sqlResultToVar */
    if (rowsWanted > 3*DEF_NUM_ROWS)
        rowsWanted = 3*DEF_NUM_ROWS;

    status = _svrGetMoreRows (catalog, contDesc, &myresult, rowsWanted);

    if (status == 0) {
        retval = sqlResultToVar (myresult, MULTI_ROW);
        freeSqlResult (myresult);
    } else {
        elog (NOTICE, "svrGetMoreRows: get_more_rows failed. Status = %d",
          status);
        retval = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retval) = VAROUTHDRSZ;
        VAROUTSTAT(retval) = htonl (status);
        if (myresult != NULL)
            free (myresult);
    }

    return retval;
}

/* svrGenGetMoreRows - Entry point for the srbGenGetMoreRows SID.
 * The more compact for of srbGetMoreRows.
 *
 *
 * Input : int catalog - The catalog type. e.g., MDAS_CATALOG
 *         int contDesc - The continuation descriptor. This is a non negative
 *         integer returned from a srbGetDatasetInfo, srbGetDataDirInfo,
 *         srbListCollect or srbGetPrivUsers call as
 *         myresult->continuation_index.
 *         int rowsWanted - No. or rows to be returned.
 *
 * Output : success - Returns the content in struct varlena
 *          failure - Returns NULL string.
 */

struct varlena *
svrGenGetMoreRows (int catalog, int contDesc, int rowsWanted)
{
    struct varlena *retval;
    int status;
    mdasC_sql_result_struct  *myresult = NULL;

    status = _svrGetMoreRows (catalog, contDesc, &myresult, rowsWanted);

    if (status == 0 && myresult != NULL) {
#ifdef foo
        retval = sqlResultToVar (myresult, MULTI_ROW);
        freeSqlResult (myresult);
#endif
       status = packMsg ((char *) myresult, &retval, sqlResultStruct_PF,
         NULL);
        freeSqlResult (myresult);

        if (status < 0) {
            return (genIntRet (status));
        }
        VAROUTSTAT(retval) = htonl (status);
    } else {
        if (status >= 0)
            status = MCAT_INQUIRE_ERROR;
        elog (NOTICE, "svrGetMoreRows: get_more_rows failed. Status = %d",
          status);
        retval = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retval) = VAROUTHDRSZ;
        VAROUTSTAT(retval) = htonl (status);
        if (myresult != NULL)
            free (myresult);
    }

    return retval;
}

#ifdef FED_MCAT
struct varlena *
svrGetMcatZone (struct varlena *vUserName, struct varlena *vDomainName)
{
    char *userName, *domainName;
    int status;
    struct varlena *retval;

    userName = varToStr (vUserName);
    domainName = varToStr (vDomainName);

    retval = (struct varlena *) malloc(MAX_TOKEN);
    status = _svrGetMcatZone (userName, domainName, VAROUTDATA(retval));
    if (status >= 0) {
        VARSIZE(retval) = strlen (VAROUTDATA(retval)) + 1 + VAROUTHDRSZ;
    } else {
        VARSIZE(retval) = VAROUTHDRSZ;
    }

    VAROUTSTAT(retval) = htonl (status);

    return retval;
}

int
_svrGetMcatZone (char *userName, char *domainName, char *mcatName)
{
    int i;
    int status;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    char *tmpMcatZone;
    mdasC_sql_result_struct  *myresult = NULL;

    if (userName != NULL && userName[0] != '\0' &&
     domainName != NULL && domainName[0] != '\0') {
        for (i = 0; i < MAX_DCS_NUM; i++) {
            sprintf(qval[i],"");
            selval[i] = 0;
        }
        selval[ZONE_NAME] = 1;
        sprintf(qval[USER_NAME]," = '%s'",userName);
        sprintf(qval[DOMAIN_DESC]," = '%s'",domainName);

        status = queryDataInfo (NULL, selval,
         (char (*) [MAX_TOKEN]) qval, &myresult, DEF_NUM_ROWS);

        if (status < 0)
            return status;

        tmpMcatZone = (char *)get_from_result_struct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[ZONE_NAME], dcs_aname[ZONE_NAME]);

        if (tmpMcatZone == NULL) {
            elog (NOTICE, "_svrGetMcatZone: Np mcatZone for %s.%s",
             userName, domainName);
            freeSqlResult (myresult);
            return (MCAT_INQUIRE_ERROR);
        }

        strcpy (mcatName, tmpMcatZone);
        freeSqlResult (myresult);
        return (0);
    } else {
        if (LocalMcat == NULL) {
            elog (NOTICE, "_svrGetMcatZone: LocalMcat == NULL");
            return (SYS_ERR_NO_MCAT_HOST);
        }
        strcpy (mcatName, LocalMcat->serverAuthInfo->mcatName);

        return (0);
    }
}
#endif

struct varlena *
svrGenQuery (int cat_type, struct varlena *vMyMcatZone,
struct varlena *vMyGenQuery, int rowsWanted)
{
    mdasC_sql_result_struct  *myresult = NULL;
    struct varlena * retVal;
    char *myMcatZone;
    genQuery_t *myGenQuery;
    int status;
    int i;


    status = unpackMsg (VAROUTDATA (vMyGenQuery), (char **) &myGenQuery,
     genQuery_PF, NULL);

    if (status < 0) {
        elog(NOTICE,
         "svrGenQuery: unpackMsg() of vMyGenQuery error, status = %d",
          status);
        retVal = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (status);
        return (retVal);
    }

    myMcatZone = varToStr (vMyMcatZone);

    /* Check with MDAS for the loid info */

    status = genQueryDataInfo (cat_type, myMcatZone, myGenQuery, &myresult,
     rowsWanted);

    if (status < 0) {
        free (myMcatZone);
        free (myGenQuery);
        if (status != MCAT_INQUIRE_ERROR) {
            elog (NOTICE,
             "svrGetDataDirInfo: queryDataInfo error. status = %d", status);
        }
        retVal = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (status);
        return retVal;
    }


    /* Encrypt the field called "dencrypted" if included */

    /* If this server is built without SRB_SECURE_COMM,
       sscEncryptString will return a fixed string instead
       of the encrypted value, protecting the transfer of
       the field */
    for (i = 0; i < myresult->result_count ; i++){
        if (strcmp(myresult->sqlresult[i].att_name,"dencrypted")==0) {
            char *cp;
            int j;
            char *cp_value;
            cp_value = (char *)myresult->sqlresult[i].values;
            for (j=0;j<myresult->row_count;j++) {
                cp = sscEncryptString(cp_value);
                strncpy(cp_value, cp, MAX_DATA_SIZE);
                cp_value += MAX_DATA_SIZE;
                free(cp);
            }
        }
    }

    /* Convert results to Var format */
#ifdef foo
    retVal = sqlResultToVar (myresult, MULTI_ROW);

    freeSqlResult (myresult);
    clearGenQuery (myGenQuery);
    free (myGenQuery);

    if (retVal != NULL) {
        return (retVal);
    }

    retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    VARSIZE(retVal) = VAROUTHDRSZ;
    VAROUTSTAT(retVal) = htonl (status);
#endif
    status = packMsg ((char *) myresult, &retVal, sqlResultStruct_PF,
     NULL);

    freeSqlResult (myresult);
    if (myGenQuery->selInx != NULL)
        free (myGenQuery->selInx);
    if (myGenQuery->selVal != NULL)
        free (myGenQuery->selVal);
    if (myGenQuery->qvalInx != NULL)
        free (myGenQuery->qvalInx);
    if (myGenQuery->qval != NULL)
        free (myGenQuery->qval);
    free (myGenQuery);

    if (status < 0) {
        return (genIntRet (status));
    }
    VAROUTSTAT(retVal) = htonl (status);
    return retVal;
}

/* _svrGenRegisterDataset - A general RegisterDataset routinne that deals
 * with compound objects.
 * If compResInfo is not NULL, then it is a compound obj.
 * regMode - NEW_FIRST_COPY - a new first copy
 *	     NEW_REPLICA -  A new replica.
 *	     EXISTING_COPY - An existing copy
 * srcSize - valid only for NEW_REPLICA and EXISTING_COPY.
 * srcResource, srcDataPath - valid only for NEW_REPLICA.
 * compPathName can be input or output. If strlen(compPathName) is 0,
 * will generate one.
 * The returned value is replNum is they are new.
 */
int
_svrGenRegisterDataset (int catType, char *destObjID, char *destCollection,
char *userName, char *domainName,
int regMode, struct mdasResInfo *destResInfo, struct mdasResInfo *compResInfo, 
char *fullPathName, srb_long_t size, 
char *dataType, int rmObjTypeInx,
char *srcResource, char *srcDataPath, srb_long_t srcSize,
char *compPathName, int *intReplNum, int *lockFd)
{
    char *myPathName;
    int replNum;

    *intReplNum = -1;

    if (compResInfo != NULL) {
        /* Create a fake pathName for the compound obj */
	if (strlen (compPathName) <= 0)
            genCompPath (compPathName, destObjID);
        myPathName = compPathName;
    } else {
        myPathName = fullPathName;
    }

    /* Do the registration */

    if (regMode == EXISTING_COPY) {
	replNum = _svrRegNewObjSize (catType,
         destObjID, destCollection, "", "", size);
#ifdef foo      /* done in _svrRegNewObjSize */
        if (srcSize != size) 
            char strSize[32];
#if defined(PORTNAME_osx)
            sprintf (strSize, "%-qd", size);
#else
            sprintf (strSize, "%-lld", size);
#endif
            replNum = _svrModifyDataset (catType,
             destObjID, destCollection,
             "", "", strSize, NULL, D_CHANGE_SIZE);
#endif	/* foo */
    } else if (regMode == NEW_FIRST_COPY) {      /* first copy */
        replNum = _svrRegisterDataset (catType, destObjID,
         userName, "all", domainName, dataType,
          myPathName, destResInfo->phyResName, destCollection,
           size, rmObjTypeInx, destResInfo->phyResLoc);
    } else if (regMode == NEW_REPLICA) {
        replNum = _svrRegisterReplica (catType, destObjID,
         destCollection, NULL, NULL, destResInfo->phyResName, myPathName,
          1, rmObjTypeInx, destResInfo->phyResLoc,
          userName, domainName);
        if (srcSize != size) {
            char strSize[32];
            replNum = _svrRegNewObjSize (catType, destObjID, destCollection,
             destResInfo->phyResName, myPathName, size);
#ifdef foo      /* done in _svrRegNewObjSize */
#if defined(PORTNAME_osx)
            sprintf (strSize, "%-qd", size);
#else
            sprintf (strSize, "%-lld", size);
#endif
            replNum = _svrModifyDataset (catType,
             destObjID, destCollection,
             destResInfo->phyResName, myPathName, strSize, NULL, D_CHANGE_SIZE);
#endif	/* foo */
        }
    } else {
        /* should not be here */
	elog (NOTICE, 
	 "_svrGenRegisterDataset: Illegal regMode %d", regMode);
	replNum = SYS_ERR_NULL_INPUT;
    }

    if (replNum < 0) {
#ifdef LOCK_OBJ
        if (*lockFd >= 0) {
            _svrObjLock (destObjID, destCollection,
              F_SETLKW, F_UNLCK, *lockFd);
            *lockFd = -1;
        }
#endif
	return (replNum);
    }

    if (compResInfo != NULL) {
        *intReplNum = _svrRegInternalCompObj (
          destObjID, destCollection, replNum, NO_INT_SEGMENTATION,
          compResInfo->phyResName, fullPathName, size, 0,
          NEW_INT_REPL_NUM, NO_INT_SEGMENTATION,
          rmObjTypeInx, compResInfo->phyResLoc,
          userName, domainName);
        if (*intReplNum < 0) {
#ifdef LOCK_OBJ
            if (*lockFd >= 0) {
                _svrObjLock (destObjID, destCollection,
                  F_SETLKW, F_UNLCK, *lockFd);
                *lockFd = -1;
            }
#endif
	    return *intReplNum;
        }
    }
    return (replNum);
}
 
int
_svrRegNewObjSize (int catType, char *objID, char *collection, 
char *resourceName, char *pathName, srb_long_t newSize)
{
    char strSize[MAX_TOKEN];
    int status;

#if defined(PORTNAME_osx)
    sprintf (strSize, "%-qd", newSize);
#else
    sprintf (strSize, "%-lld", newSize);
#endif
 
    status = _svrModifyDataset (catType, objID, collection, resourceName,
     pathName,
     strSize, NULL, D_CHANGE_SIZE);

    return (status);
}

int
_svrRegNewIntnernalCompsize (int catType, char *objID, char *collection, 
int replNum, int objSegNum, int intReplNum, int intSegNum, srb_long_t newSize)
{ 
    char strSize[MAX_TOKEN]; 
    int status; 
 
#if defined(PORTNAME_osx) 
    sprintf (strSize, "%-qd", newSize);
#else 
    sprintf (strSize, "%-lld", newSize);
#endif 

    status = _svrModInternalCompObj (objID, collection, replNum, objSegNum,
      intReplNum, intSegNum, ClientUser->userName, ClientUser->domainName,
      strSize, "1", NULL, NULL, D_COMPOUND_MODIFY_SIZE_AND_DIRTY);

    return (status);
}

int
svrModifyExtMetaData (int catType,  struct varlena *vDataName,
                      struct varlena *vCollName, struct varlena *vDataValue1,
                      struct varlena *vDataValue2, struct varlena *vDataValue3,
                      struct varlena *vDataValue4, struct varlena *vDataValue5,
                      int retractionType)
{
    char *dataValue1;
    char *dataValue2;
    char *dataValue3;
    char *dataValue4;
    char *dataValue5;
    char *dataName;
    char *collName;
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    dataName = varToStr (vDataName);
    collName = varToStr (vCollName);
    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);
    dataValue3 = varToStr (vDataValue3);
    dataValue4 = varToStr (vDataValue4);
    dataValue5 = varToStr (vDataValue5);

    /* XXXXXX potential problem. Don't know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        char *userName;

        userName = ClientUser->userName;
        status = modify_extensible_metadata (catType, dataName,  collName, dataValue1,
          dataValue2, dataValue3, dataValue4, dataValue5, retractionType, userName,
            ClientUser->domainName);

        if (status < 0) {
            elog (NOTICE,
              "svrModifyExtMetaData: modify_user_info failed status = %d",
              status);
        } else {
            commit_db2_interaction (MDAS_CAT_COMMIT);
        }
#endif
    } else {
      if ((status = remoteConnect (hostTabPtr)) < 0) {
            return status;
      }

      /* Do a client call to the remote host */

        status = srbModifyExtMetaData (hostTabPtr->conn, catType, dataName, collName,
                                       dataValue1, dataValue2, dataValue3, dataValue4,
                                       dataValue5, retractionType);

        if (status < 0) {       /* Failure */
            elog (NOTICE, "svrModifyExtMetaData failed: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }


    free (dataName);
    free (collName);
    free (dataValue1);
    free (dataValue2);
    free (dataValue3);
    free (dataValue4);
    free (dataValue5);

    return (status);
}

struct varlena *
svrGetUserByDn (int catType, int flag, struct varlena *vUserDn)
{
    int i;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  *myresult = NULL;
    char *userDn, *userName, *domainName, *zoneName;
    struct varlena *retVal;
    char tmpBuf[MAX_TOKEN]; 
    char *ptr;
    int status;

    memset (qval, 0, sizeof (qval));
    memset (selval, 0, sizeof (selval));

    userDn = varToStr (vUserDn);

    sprintf(qval[USER_DISTIN_NAME]," = '%s'", userDn);
    selval[USER_NAME] = 1;
    selval[DOMAIN_DESC] = 1;
    selval[ZONE_NAME] = 1;

    status = queryDataInfo (NULL, selval, (char (*) [MAX_TOKEN]) qval,
     &myresult, DEF_NUM_ROWS);

    if (status < 0) {
        retVal = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (status);
	return (retVal);
    }

    userName = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[USER_NAME],
            dcs_aname[USER_NAME]);

    domainName = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DOMAIN_DESC],
            dcs_aname[DOMAIN_DESC]);

    zoneName = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[ZONE_NAME],
            dcs_aname[ZONE_NAME]);

    for (i = 0; i < myresult->row_count; i++) {
	if (strcmp (userName, "groups") == 0 || 
	 strcmp (domainName, "groups") == 0) {
	    userName += MAX_DATA_SIZE;
	    domainName  += MAX_DATA_SIZE;
	    zoneName += MAX_DATA_SIZE;
	} else {
	    break;
	}
    }
	
    sprintf (tmpBuf, "%s:%s@%s", zoneName, userName, domainName);
    freeSqlResult (myresult);

    retVal = malloc (VAROUTHDRSZ + strlen (tmpBuf) + 1);
    VARSIZE(retVal) = strlen (tmpBuf) + 1 + VAROUTHDRSZ;
    VAROUTSTAT(retVal) = 0;             /* status */
    ptr = VAROUTDATA(retVal);           /* the beginning of the data */
    strcpy (ptr, tmpBuf);

    return (retVal);
}

struct mdasResInfo *
getCompResInfoForLoad (struct mdasResInfo *resInfo)
{
    struct mdasResInfo *compResInfo;

    if (resInfo == NULL) {
	return NULL;
    }

    compResInfo = resInfo->compoundLink;
    while (compResInfo != NULL) {
        int resClassInx;

        resClassInx = resClassLookup (compResInfo->phyResClass);
        if (resClassEntries[resClassInx].permanent == 0)
            break;
        compResInfo = compResInfo->next;
    }

    if (compResInfo == NULL)
        compResInfo = resInfo->compoundLink;

    return (compResInfo);
}
