/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/* srbObjFunct - Routines to handle Object type SID.
 */
#include "srbObjFunct.h"
#ifdef _WIN32
#include "Unix2Nt.h"
#endif

#include "srbSecureComm.h"

int ParaCreateFlag = 0;

/* XXXX this section may be replaced by code from Mdas */

static int AuditSetting = AUDIT_TRAIL_OFF;

int set_audit_trail (int set_value)
{
    struct hostElement  *hostTabPtr;
    int remoteFlag;
    int status;

    /* Check with MDAS for the loid info */

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        return (remoteFlag);
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
    	if (set_value == AUDIT_TRAIL_OFF) {
	    AuditSetting = AUDIT_TRAIL_OFF;
	    audit_flag = AUDIT_TRAIL_OFF;
    	} else if (set_value == AUDIT_TRAIL_ON) {
	    AuditSetting = AUDIT_TRAIL_ON;
	    audit_flag = AUDIT_TRAIL_ON;
    	}
#endif
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            return status;
        }

	AuditSetting = srbSetAuditTrail (hostTabPtr->conn, set_value);
    }
        
    return AuditSetting;
}

/* XXXX - end of section */

/* initObjDesc - Initialize the SRB object descriptor.
 *
 * Input : None
 *
 * Output : None
 */

void initObjDesc()
{ 
    int i;
#if defined(PORTNAME_solaris)
    struct rlimit rlp;

    rlp.rlim_cur = 512;
    rlp.rlim_max = 512;

    if (setrlimit (RLIMIT_NOFILE, &rlp) < 0) {
	elog (NOTICE,"initObjDesc: setrlimit error, errno = %d. This error can be ignored", errno);
    }
#endif
 
    for (i = 0; i < MAX_SRBO_DESC; i++) {
	srbObjDesc[i].lowLevelDescInx = -1;
	srbObjDesc[i].nextDescInx = -1;
	srbObjDesc[i].objID = NULL;
    }
}

/* newObjDesc - Get a new SRB object descriptor.
 *
 * Input : None
 *
 * Output : Returns a object descriptor index.
 */

int
newObjDesc ()
{
    int i;
 
    for (i = 3; i < MAX_SRBO_DESC; i++) {
	if (srbObjDesc[i].lowLevelDescInx == -1) {
	    memset (&srbObjDesc[i], 0, sizeof (struct srbObjDesc));
	    srbObjDesc[i].nextDescInx = -1;
	    srbObjDesc[i].lowLevelDescInx = DESC_TAKEN;
            srbObjDesc[i].replNum = -1;
            srbObjDesc[i].segNum = -1;
	    srbObjDesc[i].objSize = -1;
	    srbObjDesc[i].contInx = -1;
#ifdef LOCK_OBJ
	    srbObjDesc[i].lockFd = -1;
#endif
	    srbObjDesc[i].isDir = IS_FILE;
	    srbObjDesc[i].intReplNum = -1;
	    srbObjDesc[i].intSegNum = NO_INT_SEGMENTATION;
	    return (i);
        }
    }
    elog(NOTICE,"newObjDesc: Unable to allocate srbObjDesc descriptor");
    return SYS_ERR_DESC_OVERFLOW;
}

void 
closeAllDesc ()
{
    int descInx;
    int tinx;

    for (descInx = 0; descInx < MAX_SRBO_DESC; descInx++) {
        if (srbObjDesc[descInx].lowLevelDescInx >= 0) {
	    tinx = srbObjDesc[descInx].objTypeInx;
	    _svrObjClose (descInx);
	}
    }
}

/* delObjDesc - Delete a SRB object descriptor.
 *
 * Input : int i - The object descriptor index to delete
 *
 * Output : None.
 */

static void 
delObjDesc (int i) {
    srbObjDesc[i].lowLevelDescInx = -1;
    if (srbObjDesc[i].objID != NULL)
	free (srbObjDesc[i].objID);
    if (srbObjDesc[i].dataPath != NULL)
	free (srbObjDesc[i].dataPath);
    if (srbObjDesc[i].compDataPath != NULL)
	free (srbObjDesc[i].compDataPath);
    if (srbObjDesc[i].collection != NULL)
	free (srbObjDesc[i].collection);
    if (srbObjDesc[i].resourceName != NULL)
	free (srbObjDesc[i].resourceName);
    if (srbObjDesc[i].contInfo != NULL) {
    	if (srbObjDesc[i].dirtyInfo != NULL)
	    freeAllInfoOut (srbObjDesc[i].dirtyInfo);
	free (srbObjDesc[i].contInfo);
    }
    if (srbObjDesc[i].collInfo!= NULL) {
        if (srbObjDesc[i].collInfo->dirent != NULL)
            free (srbObjDesc[i].collInfo->dirent);
        free (srbObjDesc[i].collInfo);
    }

}

int
parseObjDesc (int descInx, int *objTypeInx, int *lowLevelDescInx, 
int *contInx)
{
    int myDesc;
    int fileType;

    if (descInx < 0 || descInx >= MAX_SRBO_DESC ||
    srbObjDesc[descInx].lowLevelDescInx < 0) {
        elog(NOTICE,"parseObjDesc: Invalid desc (%d)", descInx);
        return INP_ERR_DESC;
    }

    if (srbObjDesc[descInx].contInx >= 0) {
        *contInx = myDesc = srbObjDesc[descInx].contInx;
	fileType = INCONTAINER_FILE_TYPE;
    } else if (srbObjDesc[descInx].contInfo != NULL) {
	*contInx = myDesc = descInx;
	fileType = CONTAINER_FILE_TYPE;
    } else {
        myDesc = descInx;
	*contInx = -1;
	fileType = NORMAL_FILE_TYPE;
    }
    *objTypeInx = srbObjDesc[myDesc].objTypeInx;
    *lowLevelDescInx = srbObjDesc[myDesc].lowLevelDescInx;

    return (fileType);
}

/* fillObjDescWithInfo - Fill out the ObjDesc using mdasGetInfoOut.
 * Warning XXXXX - The routine took objID, dataPath and resourceName from
 * infoOut.
 */
static void
fillObjDescWithInfo (int descInx, int lowLevelDescInx, int objTypeInx,
char *collection, struct mdasInfoOut *infoOut)
{
    srbObjDesc[descInx].lowLevelDescInx = lowLevelDescInx;
    srbObjDesc[descInx].objTypeInx = objTypeInx;
    srbObjDesc[descInx].objID = strdup (infoOut->objID);
    srbObjDesc[descInx].dataPath = strdup (infoOut->dataPath);
    srbObjDesc[descInx].collection = strdup (collection);
    srbObjDesc[descInx].resourceName = strdup (infoOut->resourceName);
    srbObjDesc[descInx].baseOffset = infoOut->offset;
    srbObjDesc[descInx].objSize = infoOut->size;
    srbObjDesc[descInx].replNum = infoOut->replNum;
    srbObjDesc[descInx].segNum = infoOut->segNum;
    srbObjDesc[descInx].intReplNum = infoOut->intReplNum;
    srbObjDesc[descInx].markedDirty = infoOut->isDirty;
    srbObjDesc[descInx].chksum = infoOut->chksum;
}

static void
fillObjDescWithContInfo (int descInx, srb_long_t curOffset, 
srb_long_t curContainerSize, srb_long_t containerMaxSize,
struct mdasInfoOut *dirtyInfo, int markedDirty, int replNum)
{
    srbObjDesc[descInx].contInfo = (struct containerInfo *) 
      malloc (sizeof (struct containerInfo));
    memset (srbObjDesc[descInx].contInfo, 0, sizeof (struct containerInfo));
    srbObjDesc[descInx].markedDirty = markedDirty;
    srbObjDesc[descInx].dirtyInfo = dirtyInfo;
    srbObjDesc[descInx].contInfo->curOffset = curOffset;
    srbObjDesc[descInx].contInfo->curContainerSize = curContainerSize;
    srbObjDesc[descInx].contInfo->containerMaxSize = containerMaxSize;
    /* default to cleanOnClose */
    srbObjDesc[descInx].contInfo->cleanOnClose = 1; 
    srbObjDesc[descInx].replNum = replNum; 
}

static void 
fillObjDescWithInContInfo (int descInx, char *objID, char *collectionName,
srb_long_t baseOffset, srb_long_t dataSize, int contInx, int lowLevelDescInx,
int chksum)
{
    srbObjDesc[descInx].collection = strdup (collectionName);
    srbObjDesc[descInx].objID = strdup (objID);
    srbObjDesc[descInx].baseOffset = baseOffset;
    srbObjDesc[descInx].objSize = dataSize;
    srbObjDesc[descInx].contInx = contInx;
    srbObjDesc[descInx].lowLevelDescInx = lowLevelDescInx;
    srbObjDesc[descInx].chksum = chksum;
}

static int
svrGetCollDesc (char *myPath)
{
    int descInx;

    descInx = newObjDesc ();

    if (descInx < 0)
	return descInx;

    srbObjDesc[descInx].isDir = IS_DIR_1;
    srbObjDesc[descInx].collection = strdup (myPath);
    srbObjDesc[descInx].collInfo = (struct collectionInfo *) 
     malloc (sizeof (struct containerInfo));
    memset (srbObjDesc[descInx].collInfo, 0, sizeof (struct containerInfo));
    srbObjDesc[descInx].collInfo->maxDirLen = 0;
    srbObjDesc[descInx].collInfo->offset = 0;
    srbObjDesc[descInx].collInfo->accumOffset = 0;
    srbObjDesc[descInx].collInfo->gotData = 0;
    srbObjDesc[descInx].collInfo->continueIndex = -1;
    srbObjDesc[descInx].collInfo->dirent = NULL;

    return (descInx);
}


#ifdef foo	/* take me out */
static void
modInContainerDesc (int descInx, char *objID, char *collectionName, 
srb_long_t baseOffset, srb_long_t dataSize) {
    /* Set the starting point of this object to the end of the container */
    srbObjDesc[descInx].curContainerSize = srbObjDesc[descInx].objSize;
    srbObjDesc[descInx].baseOffset = baseOffset;
    /* objID is actually containerName */
    if (srbObjDesc[descInx].containerName != NULL)
	free (srbObjDesc[descInx].containerName);
    if (srbObjDesc[descInx].containerColl != NULL)
	free (srbObjDesc[descInx].containerColl);
    srbObjDesc[descInx].containerName = srbObjDesc[descInx].objID;
    srbObjDesc[descInx].containerColl = srbObjDesc[descInx].collection;
    srbObjDesc[descInx].collection = strdup (collectionName);
    srbObjDesc[descInx].objID = strdup (objID);

    /* Set objSize to the input dataSize */
    srbObjDesc[descInx].objSize = dataSize;
}
#endif	/* foo */

static void
fillObjDesc (int descInx, int lowLevelDescInx, int objTypeInx, 
  char *objID, char *collection, char *dataPath, char *resourceName, 
int replNum)
{
    srbObjDesc[descInx].lowLevelDescInx = lowLevelDescInx;
    srbObjDesc[descInx].objTypeInx = objTypeInx;
    srbObjDesc[descInx].objID = strdup (objID);
    srbObjDesc[descInx].dataPath = strdup (dataPath);
    srbObjDesc[descInx].collection = strdup (collection);
    srbObjDesc[descInx].resourceName = strdup (resourceName);
    srbObjDesc[descInx].replNum = replNum;
}

/* svrObjCreate - Entry point for the srbObjCreate SID.
 *
 *
 * Input : int catType - catalog type
 *	   struct varlena *vObjID - The object ID
 *	   struct varlena *vDataTypeName - data type
 *	   struct varlena *vResourceName - resource name
 *	   struct varlena *vCollectionName - collection name
 *	   struct varlena *vPathName - Data path name
 *	   srb_long_t dataSize - data size
 *
 * Output : Returns the Object descriptor - success, negative  - failure
 */

int
svrObjCreate (int catType, struct varlena *vObjID,
struct varlena *vDataTypeName, struct varlena *vResourceName,
struct varlena *vCollectionName, struct varlena *vPathName,
srb_long_t *dataSize)
{
    char *objID;
    char *collectionName;
    char *dataType;
    char *pathName;
    char *logResName;           /* The logical resource name */
    int retVal;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    objID = varToStr (vObjID);
    dataType = varToStr (vDataTypeName);
    logResName = varToStr (vResourceName);
    collectionName = varToStr (vCollectionName);
    pathName = varToStr (vPathName);

    retVal = _svrObjCreate (catType, objID, dataType, logResName,
     collectionName, pathName, *dataSize);

    free (objID);
    free (collectionName);
    free (dataType);
    free (pathName);
    free (logResName);

    return (retVal);
}

int
_svrObjCreate (int catType, char *objID, char *dataType, char *logResName,
     char *collectionName, char *pathName, srb_long_t dataSize)
{
    int retVal, status;
    char *resourceType;
    char *condInput;
    char *userName;
    char *tempStr;
    struct condResult condResult;
    char *collContainer;	/* container associated with the coll */


    tempStr = strdup (objID);
    condInput = malloc (strlen (objID) + 1);
    if (mySplitStr (tempStr, objID, condInput, '&') == NULL) 
	condInput[0] = '\0';

    free (tempStr);

    if (parseCondInput (condInput, CONTAINER, &condResult) >= 0) {
	/* container request */
	retVal = inContainerObjCreate (catType, objID, collectionName,
	condResult.strResult, dataType, dataSize);
    } else if (parseCondInput (condInput,CONT_FD, &condResult) >= 0) {
	/* container request with an already opened container */
	retVal = inContObjCreateWithFd (catType, objID, collectionName, 
	 condResult.intResult, dataType, dataSize);
    } else if ((collContainer = chkContColl (collectionName)) != NULL) {
	/* container-collection */
	retVal = inContainerObjCreate (catType, objID, collectionName,
	collContainer, dataType, dataSize);
	free (collContainer);
    } else {					/* normal request */
	retVal = _objCreate (catType, objID, condInput, dataType, logResName, 
	 collectionName, pathName, dataSize);
    }

    if (retVal >= 0) {
	int myInx;
        if (parseCondInput (condInput, REG_CHKSUM, &condResult) >= 0) {
            srbObjDesc[retVal].chksumFlag = k_FLAG;
            srbObjDesc[retVal].chksum = condResult.intResult;
        } else if (parseCondInput(condInput, VERIFY_CHKSUM, &condResult) >= 0) {
            srbObjDesc[retVal].chksumFlag = K_FLAG;
            srbObjDesc[retVal].chksum = condResult.intResult;
	}
	/* have to initialize other desc too */
	if (srbObjDesc[retVal].chksumFlag != 0) {
            myInx = srbObjDesc[retVal].nextDescInx;
            while (myInx != -1) {
                srbObjDesc[myInx].chksumFlag = srbObjDesc[retVal].chksumFlag;
                srbObjDesc[myInx].chksum = srbObjDesc[retVal].chksum;
                myInx = srbObjDesc[myInx].nextDescInx;
	    }
        }
    }

    free (condInput);
    return (retVal);
}

int
_objCreate (int catType, char *objID, char *condInput, char *dataType, 
char *logResName, char *collectionName, char *pathName, srb_long_t dataSize)
{
    int retVal, status;
    int descInx, exfInx;
    struct mdasResInfo *tmpResInfo;
    int headDescInx = -1;
    int tmpDescInx;
    char *userName;
    char fullPathName[MAX_TOKEN * 2];
    char compPathName[MAX_TOKEN * 2];
    char *myPathName;
    int copyCnt = 0;            /* Keep track of no. of replica */
    int failCnt = 0;            /* Keep track of no. of failure */
    struct mdasResInfo *compResInfo;
    struct condResult condResult;
    int objTypeInx;
    int intReplNum, replNum;
    int lockFd = -1;
    struct resCache *myResCache;

    if ((retVal = parseCondResInfo (collectionName, catType, condInput, 
     logResName, &condResult, &myResCache, dataSize)) < 0) 
	return (retVal);
		
    userName = ClientUser->userName;
    tmpResInfo = myResCache->resInfoHead;

    while (tmpResInfo != NULL) {
	if (copyCnt >= condResult.intResult) 	/* Done */
	    break;	

        compResInfo = NULL;
        if (strcmp (tmpResInfo->phyResType, COMPOUND_RESC) == 0) {
	    compResInfo = tmpResInfo->compoundLink;
	    while (compResInfo != NULL) {
                int resClassInx;
	        /* XXXXX don't do TAPE_RESC 
	        if (strcmp (compResInfo->phyResType, TAPE_RESC) != 0) {  */
		resClassInx = resClassLookup (compResInfo->phyResClass);
                if (resClassEntries[resClassInx].permanent == 0) {
#ifdef LOCK_OBJ
		if (lockFd < 0) 
                    lockFd =_svrObjLock (objID, collectionName,
                      F_SETLKW, F_WRLCK, 0);
#endif
		    exfInx = _objCreateWithResInfo (objID, collectionName, 
		     "", dataSize, compResInfo, fullPathName, &objTypeInx);
		    if (exfInx >= 0)
		        break;
	        }
	        compResInfo = compResInfo->next;
	    }
	} else {
	    exfInx = _objCreateWithResInfo (objID, collectionName, pathName,
	     dataSize, tmpResInfo, fullPathName, &objTypeInx);
	}

        if (exfInx < 0) {
#ifdef LOCK_OBJ  
            if (lockFd >= 0) {
                _svrObjLock (objID, collectionName,
                  F_SETLKW, F_UNLCK, lockFd);
		lockFd = -1;
	    }
#endif

	    retVal = exfInx;
            failCnt ++;
            if (condResult.intResult == ALL)
                break;
            else {
                tmpResInfo = tmpResInfo->next;
                continue;
            }
        }

        /* myPathName is the path that was actually used */
        if (compResInfo != NULL) {
	    /* Create a fake pathName for the compound obj */
	    genCompPath (compPathName, objID);
            myPathName = compPathName;
        } else {
            myPathName = fullPathName;
	}

    	/* Do the registration */

	/* If it is a compound onj, use the phyResName and phyResLoc of the 
	 * compound obj and the fake path name 
	 */
        if (headDescInx == -1) {
		/* set dataSize to 0. dataSize will be registered in
		 * svrObjClose.
		 */
    	    replNum = _svrRegisterDataset (catType, objID, userName, "all",
      	    ClientUser->domainName, dataType, myPathName, 
      	      tmpResInfo->phyResName, collectionName, 0, 
	      objTypeInx, tmpResInfo->phyResLoc);
	} else {
    	    replNum = _svrRegisterReplica (catType, objID, collectionName,
      	     NULL, NULL, tmpResInfo->phyResName, myPathName,
	      1, objTypeInx, tmpResInfo->phyResLoc,
              userName, ClientUser->domainName);
	}
	
        if (replNum < 0) {
#ifdef LOCK_OBJ
            if (lockFd >= 0) {
                _svrObjLock (objID, collectionName,
                  F_SETLKW, F_UNLCK, lockFd);
                lockFd = -1;
            }
#endif
	    retVal = replNum;
	    /* don't print if COLLECTION_NOT_IN_CAT since Srsync does not
	     * check for the existence of the collection */
	    if (retVal != COLLECTION_NOT_IN_CAT && 
	     retVal != DATA_NOT_IN_COLLECTION) {
	        elog (NOTICE, 
	          "_objCreate: Unable to register Object %s, status=%d", 
	          objID, replNum);
	    }

    	    /* Unlink the data object created */

 	    lowLevelClose (objTypeInx, exfInx);
            failCnt ++;
            if (condResult.intResult == ALL)
                break;
            else {
                tmpResInfo = tmpResInfo->next;
                continue;
            }
        }
	intReplNum = -1;
	if (compResInfo != NULL) {
	    /* This is a internal compound resource */
            intReplNum = _svrRegInternalCompObj (
              objID, collectionName, replNum, NO_INT_SEGMENTATION,
              compResInfo->phyResName, fullPathName, 0, 0,
              NEW_INT_REPL_NUM, NO_INT_SEGMENTATION,
              objTypeInx, compResInfo->phyResLoc,
              userName, ClientUser->domainName);

	    if (intReplNum < 0) {
#ifdef LOCK_OBJ
                if (lockFd >= 0) {
                    _svrObjLock (objID, collectionName,
                      F_SETLKW, F_UNLCK, lockFd);
                    lockFd = -1;
                }
#endif
		retVal = intReplNum;
                lowLevelClose (objTypeInx, exfInx);
                failCnt ++;
                if (condResult.intResult == ALL)
                    break;
                else {
                    tmpResInfo = tmpResInfo->next;
                    continue;
                }
	    }
	}

        /* Fill out a srbObjDesc */
 
        descInx = newObjDesc ();
        if (descInx < 0) {
            failCnt ++;
	    retVal = descInx;
            if (condResult.intResult == ALL)
                break;
            else {
                tmpResInfo = tmpResInfo->next;
                continue;
            }
	}

        /* Link it in a singlely linked chain */

        if (headDescInx == -1) {
	    headDescInx = descInx;
        } else {
	    tmpDescInx = headDescInx;
	    while (srbObjDesc[tmpDescInx].nextDescInx != -1) 
	    	tmpDescInx = srbObjDesc[tmpDescInx].nextDescInx;
	    srbObjDesc[tmpDescInx].nextDescInx = descInx;
	}

	/* Use the phyResName of the compound obj instead of the int
	 * compound obj. Use in _svrObjClose for mcat modification.
	 */
	fillObjDesc (descInx, exfInx, objTypeInx, objID, collectionName, 
	  fullPathName, tmpResInfo->phyResName, replNum);
    	srbObjDesc[descInx].objSize = dataSize;
    	srbObjDesc[descInx].intReplNum = intReplNum;
    	srbObjDesc[descInx].intSegNum = NO_INT_SEGMENTATION;
    	srbObjDesc[descInx].nextDescInx = -1;
	if (compResInfo != NULL) 
	    srbObjDesc[descInx].compDataPath = strdup (compPathName);

#ifdef LOCK_OBJ
        if (lockFd >= 0)
            srbObjDesc[descInx].lockFd = lockFd;
#endif
	copyCnt++;
	tmpResInfo = tmpResInfo->next;
    }
    
    /* Check whether the COPIES condition is satisfied */

    if (condResult.intResult == ALL) {
	if (failCnt > 0)
	    status = -1;
	else
	    status = 0;
    } else {
	if (copyCnt < condResult.intResult)
	    status = -1;
	else 
	    status = 0;
    }

    if (status == -1) {		/* failure, have to clean up after ourselves */
	if (headDescInx >= 0) {	/* we created something */

            /* Unregister the obj */

            _svrObjUnlink (objID, collectionName);

	    /* Close the opened files and delete the descriptors */

	    descInx = headDescInx;
    	    while (descInx != -1) {
        	objTypeInx = srbObjDesc[descInx].objTypeInx;
		lowLevelClose (objTypeInx, srbObjDesc[descInx].lowLevelDescInx);
        	tmpDescInx = descInx;
        	descInx = srbObjDesc[descInx].nextDescInx;
        	delObjDesc (tmpDescInx);
    	    }
	    headDescInx = -1;
	    if (retVal >= 0)
		retVal = OBJ_ERR_CREATE_REPL;
	}
    }

    if (headDescInx >= 0) {
        return headDescInx;
    } else { 
	return retVal;
    }
}

int
_objCreateWithResInfo (char *objID, char *collectionName, char *pathName, 
srb_long_t dataSize, struct mdasResInfo *resInfo, char *fullPathName,
int *objTypeInx)
{
    int retVal, status;
    int exfInx;
    char *userName;
    int tinx;

    userName = ClientUser->userName;

    tinx = objTypeLookUp (resInfo->phyResType);
    if (tinx < 0) {
        elog(NOTICE,
         "_objCreateWithResInfo: rescType:%s, resc:%s not supported, status=%d",
          resInfo->phyResType, resInfo->phyResName, tinx);
	return (tinx);
    }

    *objTypeInx = tinx;

    /* Get the fullPathName */

    retVal = getFullPathName (resInfo->logDirPathName,
                              resInfo->phyDirPathName,
                              userName, ClientUser->domainName,
                              pathName, objID,
                              collectionName, fullPathName);

    if (retVal < 0) {
        elog(NOTICE,
         "_objCreateWithResInfo: Unable get resolve pathName: %s, status = %d",
              pathName, retVal);
	return retVal;
    }

    /* Call the create at the next level */

    exfInx = lowLevelCreate (tinx, resInfo->phyResLoc,
     fullPathName, StorVaultMode, srbObjTypeEntry[tinx].chdirFlag, dataSize);

    return (exfInx);
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
    struct resCache *tmpResCache;

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
          "svrObjCreate: Input resource %s is not a registered resource",
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
#ifdef foo	/* already random */
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
		     resInfo->phyResName) == 0) {	/* already on top */
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
		
int 
inContainerObjCreate (int catType, char *objID, char *collectionName,
char *containerName, char *dataType, srb_long_t dataSize)
{
    struct mdasInfoOut *infoOut, *tmpInfoOut;
    int retVal, status;
    int descInx, contInx;
    char outContName[MAX_TOKEN], contCollection[MAX_TOKEN], 
     fullContName[MAX_TOKEN];
    int lockFd;
    int mySize;
    char *mcatHostHint;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Check for TicketUser should have been done by the calling routine */

    retVal = svrParseContainerName (containerName, outContName, contCollection,
     ClientUser->userName, ClientUser->domainName);
    if (retVal < 0) {
	elog (NOTICE,
         "inContainerObjCreate: Error parsing container %s. status = %d",
          containerName, retVal);
        return (retVal);
    }

#ifdef LOCK_OBJ
    lockFd =_svrObjLock (outContName, contCollection, F_SETLKW, F_WRLCK, 0);
    if (lockFd < 0) {
        return (lockFd);
    }
#endif
#ifdef FED_MCAT
    if ((retVal = checkSameMcat (contCollection, collectionName)) < 0) {
        elog (NOTICE,
         "inContainerObjCreate: %s/%s and %s/%s not from same MCAT",
         contCollection, outContName, collectionName, objID);
        return retVal;
    }
#endif	/* FED_MCAT */

    /* Do a mdasGetInfo on the container */

    contInx = resolveContainer (catType, O_RDWR, 1, outContName, 
     contCollection);

    if (contInx < 0) {
	return (contInx);
    } else {
	/* close container desc when an inContainer Obj is closed */
	srbObjDesc[contInx].contInfo->cleanOnClose = 1;
	srbObjDesc[contInx].lockFd = lockFd;
    }

#ifdef WRITE_SYNC
    mySize = _getFileSize (srbObjDesc[contInx].lowLevelDescInx);
    if (mySize != srbObjDesc[contInx].contInfo->curContainerSize) {
        elog(NOTICE,
#if defined(PORTNAME_osx)
         "inContainerObjCreate: %s File size %d not equal to MCAT %qd ",
#else
         "inContainerObjCreate: %s File size %d not equal to MCAT %lld ",
#endif	/* PORTNAME_osx */
         outContName, mySize, srbObjDesc[contInx].contInfo->curContainerSize);
        return (OBJ_ERR_COPY_LEN);
    }
#endif	/* WRITE_SYNC */

    sprintf (fullContName, "%s/%s", contCollection, outContName);

    /* create a ObjDesc for the inContainer object */

    descInx = newObjDesc ();

    if (descInx < 0) {
	svrObjClose (contInx);
	return (descInx); 
    }

    fillObjDescWithInContInfo (descInx, objID, collectionName,
     srbObjDesc[contInx].objSize, 0, contInx, 
      srbObjDesc[contInx].lowLevelDescInx, 0);

#ifdef LOCK_OBJ
    srbObjDesc[contInx].lockFd = lockFd;
#endif

    /* Seek to the baseOffset */

    status = _svrObjSeek (descInx, 0, SEEK_SET);

    if (status < 0) {
	elog(NOTICE, "inContainerObjCreate: _svrObjSeek error, status = %d ",
	 status);
	svrObjClose (descInx);
	return status;
    }

    /* Register the inContainer dataset */

    if (dataSize < 0)
	dataSize = 0;

    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
        /* set dataSize to 0 in the registration */
#ifdef SRB_MDAS
        status = register_incontainer_dataset (catType, objID, collectionName,
	 ClientUser->userName, ClientUser->domainName, fullContName,
	  dataType, 0, srbObjDesc[descInx].baseOffset);
        if (status >= 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        status = remoteRegisterInContDataset (catType, objID, collectionName,
	 ClientUser->userName, ClientUser->domainName, fullContName,
	  dataType, 0, srbObjDesc[descInx].baseOffset, hostTabPtr);
    }

    if (status < 0) {
	elog(NOTICE, 
	"inContainerObjCreate:register_incontainer_dataset error, status = %d ",
	 status);
	svrObjClose (descInx);
	return status;
    }
    
    return descInx;
}

int 
inContObjCreateWithFd (int catType, char *objID, char *collectionName,
int contFd, char *dataType, srb_long_t dataSize)
{
    struct mdasInfoOut *infoOut, *tmpInfoOut;
    int retVal, status;
    int descInx;
    char fullContName[MAX_TOKEN];
    int lockFd;
    int mySize;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Check for TicketUser should have been done by the calling routine */

    /* check whether it is a valid container fd */
    if (srbObjDesc[contFd].lowLevelDescInx == -1 || 
	srbObjDesc[contFd].contInfo == NULL) {
        elog (NOTICE,
         "inContObjCreateWithFd: Input container fd %d not valid", contFd);
        return (OBJ_ERR_OBJ_NOT_CONTAINER);
    }

#ifdef WRITE_SYNC
    mySize = _getFileSize (srbObjDesc[contFd].lowLevelDescInx);
    if (mySize != srbObjDesc[contFd].contInfo->curContainerSize) {
        elog(NOTICE,
#if defined(PORTNAME_osx)
         "inContObjCreateWithFd: %s File size %d not equal to MCAT %qd ",
#else
         "inContObjCreateWithFd: %s File size %d not equal to MCAT %lld ",
#endif	 
         srbObjDesc[contFd].objID, mySize, 
	   srbObjDesc[contFd].contInfo->curContainerSize);
        return (OBJ_ERR_COPY_LEN);
    }
#endif	/* WRITE_SYNC */

    sprintf (fullContName, "%s/%s", srbObjDesc[contFd].collection, 
     srbObjDesc[contFd].objID);

    /* create a ObjDesc for the inContainer object */

    descInx = newObjDesc ();

    if (descInx < 0) {
	svrObjClose (contFd);
	return (descInx); 
    }

    fillObjDescWithInContInfo (descInx, objID, collectionName,
     srbObjDesc[contFd].objSize, 0, contFd, 
      srbObjDesc[contFd].lowLevelDescInx, 0);

    /* Seek to the baseOffset */

    status = _svrObjSeek (descInx, 0, SEEK_SET);

    if (status < 0) {
	elog(NOTICE, "inContainerObjCreate: _svrObjSeek error, status = %d ",
	 status);
	svrObjClose (descInx);
	return status;
    }

    /* Register the inContainer dataset */

    if (dataSize < 0)
	dataSize = 0;
    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
    /* set dataSize to 0 in the registration */
#ifdef SRB_MDAS
        status = register_incontainer_dataset (catType, objID, collectionName,
	 ClientUser->userName, ClientUser->domainName, fullContName,
	  dataType, 0, srbObjDesc[descInx].baseOffset);
        if (status >= 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        status = remoteRegisterInContDataset (catType, objID, collectionName,
	 ClientUser->userName, ClientUser->domainName, fullContName,
	  dataType, 0, srbObjDesc[descInx].baseOffset, hostTabPtr);
    }

    if (status < 0) {
	elog(NOTICE, 
	"inContainerObjCreate:register_incontainer_dataset error, status = %d ",
	 status);
	svrObjClose (descInx);
	return status;
    }
    
    return descInx;
}

int 
resolveContainer (int catType, int openFlag, int createFlag, 
char *containerName, char *contCollection)
{
    int retVal, emptyCnt;
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead, *tmpInfoOut, *prevInfoOut, 
     *permInfoOut, *cacheInfoOut;
    struct mdasInfoOut *emptyContHead;
    char fullContPath[MAX_TOKEN];
    char fullNewContPath[MAX_TOKEN];
    char *containerMaxSize, *phyResClass, *isDirty, *size, *phyResName, 
     *logResName;
    srb_long_t iContainerMaxSize, maxSize;
    int pid;

    infoInp.accessMode = openFlagLookUp (openFlag);
    infoInp.userName = ClientUser->userName;
    infoInp.domainName = ClientUser->domainName;
    infoInp.objID = containerName;
    infoInp.collectionName = contCollection;
    
    if ((retVal = mdasGetInfo (&infoInp, &infoOutHead)) < 0) {
        elog (NOTICE,
         "resolveContainer: mdasGetInfo error for container %s. status = %d",
          containerName, retVal);
        return retVal;
    }

    /* Do some sanity checks. */

    /* infoOutHead->containerName must match $contCollection/$outContName */
    sprintf (fullContPath, "%s/%s", contCollection, containerName);
    if (strcmp (fullContPath, infoOutHead->containerName) != 0) {
	elog (NOTICE, "resolveContainer: %s is not a container", fullContPath);
	freeAllInfoOut (infoOutHead);
	return (OBJ_ERR_OBJ_NOT_CONTAINER);
    }
	
    /* Well, see if TEMPORARY_RES_CL is configured in the resource */

    emptyCnt = getEmptyContainer (catType, fullContPath, infoOutHead,
    &emptyContHead);

    if (emptyCnt < 0) {
	elog(NOTICE, "resolveContainer: getEmptyContainer error, status = %d",
	 emptyCnt);
	freeAllInfoOut (infoOutHead);
	return (emptyCnt);
    }
    
    /* Check if containerMaxSize exceeded for create */

    if (emptyContHead != NULL) {
        iContainerMaxSize = emptyContHead->containerMaxSize;
	logResName = emptyContHead->logResName;
    } else {
        iContainerMaxSize = -1;
	logResName = NULL;
    }
  
    /* Update containerMaxSize and logResName in infoOutHead */

    maxSize = 0;
    tmpInfoOut = infoOutHead;
    while (tmpInfoOut != NULL) {
	tmpInfoOut->containerMaxSize = iContainerMaxSize;
	if (logResName != NULL)
	    tmpInfoOut->logResName = strdup (logResName);
        if (tmpInfoOut->size > maxSize)
            maxSize = tmpInfoOut->size;
        tmpInfoOut = tmpInfoOut->next;
    }

    /* Container full ? */

    if (createFlag > 0 && infoOutHead != NULL && iContainerMaxSize > 0 &&
     maxSize >= iContainerMaxSize) {
        char newContName[MAX_TOKEN];

        sprintf (newContName, "%s.%-d", containerName, time (NULL));
        /* rename the current container */
        retVal =  _svrModifyDataset (catType, containerName, contCollection,
         "", "", newContName, NULL, D_CHANGE_DNAME);

        if (retVal < 0) {
            elog(NOTICE,
             "resolveContainer: Unable to change containerName for %s",
              containerName);
	    freeAllInfoOut (infoOutHead);
	    freeAllInfoOut (emptyContHead);
            return (retVal);
        }

        /* Now make a new container */

        retVal = _svrContainerCreate (catType, fullContPath, 
	 infoOutHead->dataType, infoOutHead->logResName, 
	  infoOutHead->containerMaxSize);

        if (retVal < 0) {
            elog(NOTICE,
             "resolveContainer: Unable to create new container for %s",
              containerName);
	    _svrModifyDataset (catType, newContName, contCollection,
             "", "", containerName, NULL, D_CHANGE_DNAME);
	    freeAllInfoOut (infoOutHead);
	    freeAllInfoOut (emptyContHead);
            return (retVal);
        }
	/*** RAJA: added Feb 12,2003 to make new container inherit 
	     metadata from old container ***/
	retVal = _svrModifyDataset (catType, containerName, contCollection, 
		   "","",
		   contCollection,newContName,
		   D_COPY_META_DATA_FROM_CONTAINER_TO_NEW_CONTAINER);
	if (retVal < 0) {
            elog(NOTICE,
             "resolveContainer: Unable to copy metadata from container %s/%s to new container %s/%s",contCollection, newContName, contCollection,
              containerName);
	    _svrModifyDataset (catType, newContName, contCollection,
             "", "", containerName, NULL, D_CHANGE_DNAME);
	    freeAllInfoOut (infoOutHead);
	    freeAllInfoOut (emptyContHead);
            return (retVal);
        }
	/*** RAJA: added Feb 12,2003 to make new container inherit 
	     metadata from old container ***/
	/* also if the container is associated with a collection,
	 * the association need to be switched
	 */
	sprintf (fullNewContPath, "%s/%s", contCollection, newContName);
	retVal = _svrModifyDataset (catType, containerName, contCollection,
		   "","",
                   fullNewContPath, NULL,
                   D_UPDATE_CONTAINER_FOR_COLLECTION);
	if (retVal < 0) {
	    elog(NOTICE,
	     "resolveContainer: coll-cont update failed, stat = %d",
	     retVal);
            freeAllInfoOut (infoOutHead);
            freeAllInfoOut (emptyContHead);
            return (retVal);
	}
#ifdef AUTO_SYNC_FULL_CONTAINER
	/* fork a child to sync and purge the old container. must use
	 * fork() instead of vfork - don't want to share add space. */

	if ((pid = fork()) == 0) {  /* child */
	    /* closeout opened socket and desc */
	    svrCloseAll ();
#ifdef SRB_MDAS
    	    if ((retVal = open_db2_interaction(MDAS_CATALOG)) != 0) {
	    	elog(NOTICE,
	     	 "resolveContainer: Unable to open_db2_interaction,status = %d",
	       	  retVal);
	    	exit (0);
	    }
#endif
	    retVal = _svrSyncContainer (catType, newContName, 
	     PRIMARY_FLAG | PURGE_FLAG);
#ifdef SRB_MDAS
    	    close_db2_interaction(MDAS_CAT_COMMIT);
#endif
	    exit (0);
        }
#endif	/* AUTO_SYNC_FULL_CONTAINER */

        freeInfoOut (infoOutHead);
	freeAllInfoOut (emptyContHead);

        /* call resolveContainer one more time */
        retVal = resolveContainer (catType, O_RDWR, 1, containerName, 
	 contCollection);

        return (retVal);
    }


    if (emptyCnt <= 0) {
        freeAllInfoOut (emptyContHead);
	emptyContHead = NULL;
    }

    retVal = openContainer (catType, openFlag, containerName, contCollection,
     infoOutHead, emptyContHead);

    /* Don't have to free infoOutHead nor emptyContHead. 
     * Done in openContainer */

    /* freeAllInfoOut (emptyContHead); */

    return (retVal);
}

int
openContainer (int catType, int openFlag, char *containerName, 
char *contCollection, 
struct mdasInfoOut *infoOutHead, struct mdasInfoOut *emptyContHead) 
{
    struct mdasInfoOut *dirtyCacheInfoOut, *dirtyArchInfoOut, 
     *cacheInfoOut, *archInfoOut, *cacheInfoOut1, *archInfoOut1;
    struct mdasInfoOut *tmpInfoOut, *tmp1InfoOut, *nextInfoOut;
    /* empty container */
    struct mdasInfoOut *empCacheInfoOut, *empArchInfoOut, 
     *empCacheInfoOut1, *empArchInfoOut1, 
      *empCacheInfoOut2, *empArchInfoOut2;  
    int resClassInx;
    int retVal;
    int priOnly = 0;	/* primary only flag */
    srb_long_t iContainerMaxSize = 0;
    int markedDirty = 0;

    /* save containerMaxSize */
    if (infoOutHead != NULL)
	iContainerMaxSize = infoOutHead->containerMaxSize;
    else if (emptyContHead != NULL)
	iContainerMaxSize = emptyContHead->containerMaxSize;

    dirtyCacheInfoOut = dirtyArchInfoOut = cacheInfoOut = archInfoOut = 
    empCacheInfoOut = NULL;

    /* sort existing copy by type */

    sortInfoByClass (infoOutHead, &cacheInfoOut, &archInfoOut, 
     &cacheInfoOut1, &archInfoOut1,
      &dirtyCacheInfoOut, &dirtyArchInfoOut);

    combineInfoOut (&cacheInfoOut, &cacheInfoOut1);
    combineInfoOut (&archInfoOut, &archInfoOut1);

    /* See what we got here. If dirtyCacheInfoOut is not empty, just use
     * it and nothing else */

    if (dirtyCacheInfoOut != NULL || dirtyArchInfoOut != NULL) {
	/* There are other out-of-date cache copies, delete them */

	tmpInfoOut = cacheInfoOut;
        while (tmpInfoOut != NULL) {
	    nextInfoOut = tmpInfoOut->next;
    	    retVal = _rmContainerCopy (catType, containerName, contCollection,
     	     tmpInfoOut);
	    queInfoOut (&empCacheInfoOut, tmpInfoOut, 1, 0);  /* que to empty */
	    tmpInfoOut = nextInfoOut;
	}
	/* XXXX Not needed. freeAllInfoOut (cacheInfoOut); */
	cacheInfoOut = NULL;

	/* Also don't attempt to stage it from out-of-date Arch copy */
	freeAllInfoOut (archInfoOut);
	archInfoOut = NULL;
    }

    /* We are here. InfoOut are either all clean or all dirty */
    /* Try to open the cache first */

    if (dirtyCacheInfoOut != NULL) 
	tmpInfoOut = dirtyCacheInfoOut;
    else
    	tmpInfoOut = cacheInfoOut;

    while (tmpInfoOut != NULL) {
    	retVal = _objOpen (containerName, contCollection, openFlag, 
	 tmpInfoOut);

	if (retVal >= 0) {
	    if (tmpInfoOut->isDirty)
		markedDirty = 1;
	    freeAllInfoOut (emptyContHead);
	    emptyContHead = 0;
	    freeAllInfoOut (archInfoOut);
	    freeAllInfoOut (cacheInfoOut);
	    freeAllInfoOut (empCacheInfoOut);
	    combineInfoOut (&dirtyCacheInfoOut, &dirtyArchInfoOut);
            /* do some desc processing for container */
            fillObjDescWithContInfo (retVal, 0, srbObjDesc[retVal].objSize,
             iContainerMaxSize, dirtyCacheInfoOut, markedDirty, 
	      tmpInfoOut->replNum);
	    return (retVal);
	}

	tmpInfoOut = tmpInfoOut->next;
    }
    /* stop if dirtyCacheInfoOut is not empty. Don't want to stage it 
     * because it may not be up to date. */

    if (dirtyCacheInfoOut != NULL) {
	freeAllInfoOut (emptyContHead);
        emptyContHead = 0;
    	freeAllInfoOut (archInfoOut);
    	freeAllInfoOut (cacheInfoOut);
    	freeAllInfoOut (empCacheInfoOut);
	combineInfoOut (&dirtyCacheInfoOut, &dirtyArchInfoOut);
	if (retVal < 0) {
            freeAllInfoOut (dirtyCacheInfoOut);
	}
    	return (retVal);
    }

	
    /* Well, open not successful. Try to stage cache from arch */

    sortInfoByClass (emptyContHead, &empCacheInfoOut, &empArchInfoOut,
     &empCacheInfoOut1, &empArchInfoOut1,
     &empCacheInfoOut2, &empArchInfoOut2);

    /* pick cache resc only */

    combineInfoOut (&empCacheInfoOut, &empCacheInfoOut1);

    freeAllInfoOut (empArchInfoOut1);
    freeAllInfoOut (empArchInfoOut);
    freeAllInfoOut (empCacheInfoOut2);
    freeAllInfoOut (empArchInfoOut2);

    tmpInfoOut = empCacheInfoOut;
    while (tmpInfoOut != NULL) {
	struct mdasInfoOut *newInfoOut, *tmpArchInfoOut;

	if (dirtyArchInfoOut != NULL)
	    tmpArchInfoOut = dirtyArchInfoOut;
	else
	    tmpArchInfoOut = archInfoOut;

	while (tmpArchInfoOut != NULL) {
          retVal = _objReplicate (catType, containerName, contCollection,
           tmpInfoOut->resourceName, "", tmpArchInfoOut, &newInfoOut, NULL);

          if (retVal >= 0) {

	    retVal = _objOpen (containerName, contCollection, O_RDWR, 
	     newInfoOut);
	    freeInfoOut (newInfoOut);
	    if (retVal >= 0) {
		int status;

    	    	/* mark it dirty if the orig copy is dirty */
		if (tmpArchInfoOut->isDirty) {
		    int status;

                    status = _svrModifyDataset (catType, containerName,
                     contCollection, newInfoOut->resourceName,
                      newInfoOut->dataPath, "1", NULL, D_CHANGE_DIRTY);
		    markedDirty = 1;
            	}

		freeAllInfoOut (archInfoOut);
		freeAllInfoOut (cacheInfoOut);
		freeAllInfoOut (empCacheInfoOut);
		combineInfoOut (&dirtyCacheInfoOut, &dirtyArchInfoOut);
		/* do some desc processing for container */
		fillObjDescWithContInfo (retVal, 0, srbObjDesc[retVal].objSize,
		 iContainerMaxSize, dirtyCacheInfoOut, markedDirty,
		  tmpArchInfoOut->replNum);
		return (retVal);
	    } else {
		elog (NOTICE,
                 "openContainer: _objOpen %s error after repl. status = %d",
                   containerName, retVal);
            }
 
	  } else {
	    elog (NOTICE,
             "openContainer: _objReplicate error %s. status = %d",
              containerName, retVal);
	  }
	  tmpArchInfoOut = tmpArchInfoOut->next;
	}
        tmpInfoOut = tmpInfoOut->next;
    }

    /* can't stage. Just open the arch copy */

    if (dirtyArchInfoOut != NULL)
        tmpInfoOut = dirtyArchInfoOut;
    else
        tmpInfoOut = archInfoOut;

    while (tmpInfoOut != NULL) {
        retVal = _objOpen (containerName, contCollection, openFlag,
         tmpInfoOut);

        if (retVal >= 0) {
            if (tmpInfoOut->isDirty)
                markedDirty = 1;
	    freeAllInfoOut (archInfoOut);
	    freeAllInfoOut (cacheInfoOut);
	    freeAllInfoOut (empCacheInfoOut);
	    combineInfoOut (&dirtyCacheInfoOut, &dirtyArchInfoOut);
            /* do some desc processing for container */
            fillObjDescWithContInfo (retVal, 0, srbObjDesc[retVal].objSize,
             iContainerMaxSize, dirtyCacheInfoOut, markedDirty, 
	      tmpInfoOut->replNum);
    	    return (retVal);
	}

        tmpInfoOut = tmpInfoOut->next;
    }

    /* No open */
    if (retVal >= 0)
	retVal = OBJ_ERR_OBJ_NO_CACHE_CONTAINER;

    freeAllInfoOut (archInfoOut);
    freeAllInfoOut (cacheInfoOut);
    freeAllInfoOut (empCacheInfoOut);
    combineInfoOut (&dirtyCacheInfoOut, &dirtyArchInfoOut);
    freeAllInfoOut (dirtyCacheInfoOut);

    return (retVal);
}

int
getEmptyContainer (int catType, char *fullContPath,
 struct mdasInfoOut *infoOutHead, struct mdasInfoOut **emptyContHead)
{
    struct mdasInfoOut *tmpInfoOut, *tmpEmpInfoOut;
    mdasC_sql_result_struct *myresult = NULL;
    char *containerMaxSize, *phyResClass, *phyResName,
     *logResName;
    int i;
    int retVal;
    int emptyCnt = 0;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    tmpInfoOut = tmpEmpInfoOut = *emptyContHead = NULL;

    /* get the container info */

    remoteFlag = getMdasEnabledHost (fullContPath, &hostTabPtr);

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = get_container_info (catType, fullContPath,
         ClientUser->userName, ClientUser->domainName, &myresult, DEF_NUM_ROWS);
#endif
    } else {
        retVal = remoteGetContainerInfo (catType, fullContPath,
         ClientUser->userName, ClientUser->domainName, &myresult, DEF_NUM_ROWS,
	 hostTabPtr);
    }

    if (retVal < 0) {
        elog(NOTICE, "getEmptyContainer: get_container_info error, status = %d",
         retVal);
        freeAllInfoOut (infoOutHead);
        return (retVal);
    }

    containerMaxSize = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_MAX_SIZE], dcs_aname[CONTAINER_MAX_SIZE]);

    phyResClass = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_RSRC_CLASS], dcs_aname[CONTAINER_RSRC_CLASS]);

    phyResName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);

    logResName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[CONTAINER_LOG_RSRC_NAME],
                dcs_aname[CONTAINER_LOG_RSRC_NAME]);

    for (i = 0; i < myresult->row_count; i++) {
        int match;

	match = 0;

        tmpInfoOut = infoOutHead;
        while (tmpInfoOut != NULL) {
            if (strcmp (tmpInfoOut->resourceName,
             &phyResName[i*MAX_DATA_SIZE]) == 0) {
                match = 1;
                break;
            }
            tmpInfoOut = tmpInfoOut->next;
        }
	/* XXXXXXX this is a tmp fix. get_container_info returns the same thing
	 * twice */
#ifdef foo
	if (match == 0) {
	    tmpInfoOut = *emptyContHead;
	    while (tmpInfoOut != NULL) {
            	if (strcmp (tmpInfoOut->resourceName,
                 &phyResName[i*MAX_DATA_SIZE]) == 0) {
                    match = 1;
                    break;
                }
                tmpInfoOut = tmpInfoOut->next;
	    }
        }
#endif

	/* XXXXXXXX end of tmp fix */

        if (match == 0) {           /* no match */
            tmpEmpInfoOut = malloc (sizeof (struct mdasInfoOut));
	    memset (tmpEmpInfoOut, 0, sizeof (struct mdasInfoOut));
            tmpEmpInfoOut->objID = NULL;
            tmpEmpInfoOut->dataPath = NULL;
            tmpEmpInfoOut->resourceLoc = NULL;
            tmpEmpInfoOut->logResName = strdup (logResName);
            tmpEmpInfoOut->phyResClass = NULL;
            tmpEmpInfoOut->containerName = NULL;
            tmpEmpInfoOut->dataType = NULL;
            tmpEmpInfoOut->resourceName = strdup (
             &phyResName[i*MAX_DATA_SIZE]);
	    tmpEmpInfoOut->containerMaxSize = strtoll (containerMaxSize, 0, 0);
	    tmpEmpInfoOut->isDirty = 0;
	    tmpEmpInfoOut->resClassInx = 
	     resClassLookup (&phyResClass [i*MAX_DATA_SIZE]);
	    
            queInfoOut (emptyContHead, tmpEmpInfoOut, 1, 0);
	    emptyCnt++;
        }
    }
    if (emptyCnt == 0) {
        tmpEmpInfoOut = malloc (sizeof (struct mdasInfoOut));
	memset (tmpEmpInfoOut, 0, sizeof (struct mdasInfoOut));
        tmpEmpInfoOut->objID = NULL;
        tmpEmpInfoOut->dataPath = NULL;
        tmpEmpInfoOut->resourceLoc = NULL;
        tmpEmpInfoOut->logResName = strdup (logResName);
        tmpEmpInfoOut->phyResClass = NULL;
        tmpEmpInfoOut->containerName = NULL;
        tmpEmpInfoOut->dataType = NULL;
        tmpEmpInfoOut->resourceName = NULL;
        tmpEmpInfoOut->containerMaxSize = strtoll (containerMaxSize, 0, 0);
        tmpEmpInfoOut->isDirty = 0;

        queInfoOut (emptyContHead, tmpEmpInfoOut, 1, 0);
    }

    freeSqlResult (myresult);
    return (emptyCnt);
}

int 
svrReplContainer (int catType, struct varlena *vContainerName,
struct varlena *vNewResourceName)
{
    char *containerName;
    char *newResourceName;
    int retVal;
    char dataName[MAX_TOKEN], collection[MAX_TOKEN];
    int lockFd;

    containerName = varToStr (vContainerName);
    newResourceName = varToStr (vNewResourceName);

#ifdef LOCK_OBJ
    retVal = svrParseContainerName (containerName, dataName, collection,
     ClientUser->userName, ClientUser->domainName);
    if (retVal < 0) {
        elog (NOTICE,
         "svrObjReplContainer: Error parsing container %s. status = %d",
          containerName, retVal);
        free (containerName);
        free (newResourceName);
        return (retVal);
    }

    /* Do a write lock */
    lockFd =_svrObjLock (dataName, collection, F_SETLKW, F_WRLCK, 0);
    if (lockFd < 0) {
        free (containerName);
    	free (newResourceName);
        return (lockFd);
    }

#endif

    retVal = _svrReplContainer (catType, containerName, newResourceName);

#ifdef LOCK_OBJ
    _svrObjLock (dataName, collection, F_SETLKW, F_UNLCK, lockFd);
#endif

    free (containerName);
    free (newResourceName);

    return retVal;
}

int
_svrReplContainer (int catType, char *containerName, char *newResourceName)
{
    char outContName[MAX_TOKEN], contCollection[MAX_TOKEN];
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead, *tmpInfoOut, *permInfoOut, 
     *emptyInfoOut;
    struct mdasInfoOut *cacheInfoOut, *archInfoOut, *myInfoOut,
     *cacheInfoOut1, *archInfoOut1,
      *dirtyCacheInfoOut, *dirtyArchInfoOut;
    mdasC_sql_result_struct *myresult = NULL;
    int status = 0;
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    retVal = svrParseContainerName (containerName, outContName, contCollection,
     ClientUser->userName, ClientUser->domainName);
    if (retVal < 0) {
        elog (NOTICE,
         "_svrObjReplContainer: Error parsing container %s. status = %d",
          containerName, retVal);
        return (retVal);
    }

    remoteFlag = getMdasEnabledHost (contCollection, &hostTabPtr);

    if (remoteFlag < 0) {
        retVal = remoteFlag;
    } else if (remoteFlag == REMOTE_HOST) {      /* Can talk to mdas */
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        } else {
            retVal = srbReplContainer (hostTabPtr->conn, catType, 
	     containerName, newResourceName);
        }
        return (retVal);
    }

    infoInp.domainName = ClientUser->domainName;
    infoInp.accessMode = "all";
    infoInp.objID = outContName;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = contCollection;

    /* Check with MDAS for the requested data */

    if ((retVal = mdasGetInfo (&infoInp, &infoOutHead)) < 0) {
        return retVal;
    }

    if (!myIsContainer (outContName, contCollection,
     infoOutHead->containerName)) {
        freeAllInfoOut (infoOutHead);
        return (OBJ_ERR_OBJ_NOT_CONTAINER);
    }

    /* match the resource with infoOut */

    myInfoOut = selectInfoOutByResc (newResourceName, &infoOutHead);

    if (myInfoOut == NULL) {
	char fullContPath[MAX_TOKEN];

	sprintf (fullContPath, "%s/%s", contCollection, outContName);

        remoteFlag = getMdasEnabledHost (fullContPath, &hostTabPtr);

        if (remoteFlag < 0) {
	    retVal = remoteFlag;
        } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */

#ifdef SRB_MDAS
            retVal = get_container_info (catType, fullContPath,
             ClientUser->userName, ClientUser->domainName, &myresult, 
	      DEF_NUM_ROWS);
#endif
    	} else {
            retVal = remoteGetContainerInfo (catType, fullContPath,
             ClientUser->userName, ClientUser->domainName, &myresult, 
	      DEF_NUM_ROWS, hostTabPtr);
    	}
	retVal = matchRescWithContResult (newResourceName, myresult);
	freeSqlResult (myresult);
	if (retVal == 0) {	/* no match */
	    struct resCache *myCacheRes;
            struct mdasResInfo *priCacheResInfo, *priArchResInfo,
             *cacheResInfo, *archResInfo;

            if (getResInfo (fullContPath, catType, newResourceName, 
	      ClientUser->domainName, &myCacheRes) < 0) {
                elog (NOTICE,
                  "_svrObjReplContainer: resc %s is not a registered resource",
                  newResourceName);
                return (OBJ_ERR_RES_NOT_REG);
            }

            sortResByClass (myCacheRes->resInfoHead, &priCacheResInfo,
             &priArchResInfo, &cacheResInfo, &archResInfo);

	    /* will allow cache resource even if there is no match */
            if (priCacheResInfo == NULL && cacheResInfo == NULL) {
	        elog(NOTICE,
	         "_svrObjReplContainer: resource %s not in the container resource",
	          newResourceName);
	        freeAllInfoOut (infoOutHead);
	        return (OBJ_ERR_RES_TYPE);
	    }
	}
    } else {
	if (myInfoOut->isDirty > 0) {
	    freeAllInfoOut (infoOutHead);
	    freeAllInfoOut (myInfoOut);
	    return (0);	
	}
    }

    /* sort the infoOutHead */

    sortInfoByClass (infoOutHead, &cacheInfoOut, &archInfoOut,
     &cacheInfoOut1, &archInfoOut1,
     &dirtyCacheInfoOut, &dirtyArchInfoOut);

    combineInfoOut (&dirtyCacheInfoOut, &dirtyArchInfoOut);

    if (dirtyCacheInfoOut == NULL) {
	/* combine into infoOutHead */
	infoOutHead = cacheInfoOut;
	combineInfoOut (&infoOutHead, &cacheInfoOut1);
	combineInfoOut (&infoOutHead, &archInfoOut);
	combineInfoOut (&infoOutHead, &archInfoOut1);

	if (myInfoOut != NULL) {
	    /* nothing dirty and the copy exist */
            freeAllInfoOut (myInfoOut);
            freeAllInfoOut (infoOutHead);
	    return 0;
	}
    } else {	/* it is dirty */
	freeAllInfoOut (cacheInfoOut);
	freeAllInfoOut (cacheInfoOut1);
	freeAllInfoOut (archInfoOut);
	freeAllInfoOut (archInfoOut1);
	infoOutHead = dirtyCacheInfoOut;
    }

    if (infoOutHead == NULL) { 		/* no source */
	freeAllInfoOut (myInfoOut);
	return 0;
    }
	
    if (myInfoOut != NULL) { 
	_rmContainerCopy (catType, outContName, contCollection, myInfoOut);
	freeAllInfoOut (myInfoOut);
	myInfoOut = NULL;
    }

    /* Now replicate the copy */

    tmpInfoOut = infoOutHead;
    while (tmpInfoOut != NULL) {
        struct mdasInfoOut *newInfoOut = NULL;

        retVal = _objReplicate (catType, outContName, contCollection,
         newResourceName, "", tmpInfoOut, &newInfoOut, NULL);
        if (retVal >= 0) {
            /* mark it dirty if the orig copy is dirty */

            if (tmpInfoOut->isDirty) {
                retVal = _svrModifyDataset (catType, outContName,
                 contCollection, newInfoOut->resourceName,
                  newInfoOut->dataPath, "1", NULL, D_CHANGE_DIRTY);
            }
            freeAllInfoOut (newInfoOut);
            break;
	}
	tmpInfoOut = tmpInfoOut->next;
    }

    freeAllInfoOut (infoOutHead);
    return retVal;
}

int 
sortInfoByClass (struct mdasInfoOut *infoOutHead, 
struct mdasInfoOut **oPriCacheInfoOut, 
struct mdasInfoOut **oPriArchInfoOut,
struct mdasInfoOut **oCacheInfoOut, struct mdasInfoOut **oArchInfoOut,
struct mdasInfoOut **oDirtyCacheInfoOut, 
struct mdasInfoOut **oDirtyArchInfoOut)
{
    struct mdasInfoOut *priCacheInfoOut, *priArchInfoOut, *cacheInfoOut,
     *archInfoOut, *dirtyCacheInfoOut, *dirtyArchInfoOut; 
    struct mdasInfoOut *tmpInfoOut, *nextInfoOut;
    int resClassInx;

    priCacheInfoOut = priArchInfoOut = cacheInfoOut =
    archInfoOut = dirtyCacheInfoOut = dirtyArchInfoOut = NULL;

    tmpInfoOut = infoOutHead;
    while (tmpInfoOut != NULL) {
	int topFlag;

        nextInfoOut = tmpInfoOut->next;
        resClassInx = tmpInfoOut->resClassInx;
	if (isResLocLocal (tmpInfoOut->resourceLoc) == 1)
	    topFlag = 1;
	else
	    topFlag = 0;
	if (resClassEntries[resClassInx].permanent == 0 &&
	 tmpInfoOut->isDirty > 0) {
	    queInfoOut (&dirtyCacheInfoOut, tmpInfoOut, 1, topFlag);
        } else if (resClassEntries[resClassInx].permanent == 1 &&
         tmpInfoOut->isDirty > 0) {
            queInfoOut (&dirtyArchInfoOut, tmpInfoOut, 1, topFlag);
        } else if (resClassEntries[resClassInx].permanent == 1 &&
         resClassEntries[resClassInx].primary == 1) {
            queInfoOut (&priArchInfoOut, tmpInfoOut, 1, topFlag);
        } else if (resClassEntries[resClassInx].permanent == 0 &&
         resClassEntries[resClassInx].primary == 1) {
            queInfoOut (&priCacheInfoOut, tmpInfoOut, 1, topFlag);
        } else if (resClassEntries[resClassInx].permanent == 1 &&
         resClassEntries[resClassInx].primary == 0) {
            queInfoOut (&archInfoOut, tmpInfoOut, 1, topFlag);
        } else {
            queInfoOut (&cacheInfoOut, tmpInfoOut, 1, topFlag);
        }
        tmpInfoOut = nextInfoOut;
    }

    *oPriCacheInfoOut = priCacheInfoOut;
    *oPriArchInfoOut = priArchInfoOut;
    *oCacheInfoOut = cacheInfoOut;
    *oArchInfoOut = archInfoOut;
    *oDirtyCacheInfoOut = dirtyCacheInfoOut;
    *oDirtyArchInfoOut = dirtyArchInfoOut;

    return (0);
}

/* Combine 2 link list of mdasGetInfoOut - put topInfoOut on top and
 * botInfoOut at bottom.
 */
int
combineInfoOut (struct mdasInfoOut **topInfoOut,
struct mdasInfoOut **botInfoOut) 
{
    struct mdasInfoOut *tmpInfoOut, *prevInfoOut;

    tmpInfoOut = *topInfoOut;
    if (tmpInfoOut == NULL) {
	*topInfoOut = *botInfoOut;
	*botInfoOut = NULL;
	return (0);
    }

    /* Find the end of topInfoOut */
    while (tmpInfoOut != NULL) {
	prevInfoOut = tmpInfoOut;
	tmpInfoOut = tmpInfoOut->next;
    }

    /* john the 2 link list */
    prevInfoOut->next = *botInfoOut;
    *botInfoOut = NULL;

    return (0);
}

/* Sort and combine the infoOutHead based on class of storage.
 * A slight variation from KESTOCK's fix.
 */

int
sortAndCombInfo (struct mdasInfoOut **infoOutHead)
{
    struct mdasInfoOut *dirtyCacheInfoOut, *dirtyArchInfoOut,
     *cacheInfoOut, *archInfoOut, *cacheInfoOut1, *archInfoOut1;

    sortInfoByClass (*infoOutHead, &cacheInfoOut, &archInfoOut,
     &cacheInfoOut1, &archInfoOut1,
      &dirtyCacheInfoOut, &dirtyArchInfoOut);

    /* combine them in the order of cache first, arch second */

    combineInfoOut (&dirtyCacheInfoOut, &cacheInfoOut);
    combineInfoOut (&dirtyCacheInfoOut, &cacheInfoOut1);
    combineInfoOut (&dirtyCacheInfoOut, &dirtyArchInfoOut);
    combineInfoOut (&dirtyCacheInfoOut, &archInfoOut);
    combineInfoOut (&dirtyCacheInfoOut, &archInfoOut1);

    *infoOutHead = dirtyCacheInfoOut;

    return 0;
}

/* sort the info in infoOutHead for normal object. Through openFlag,
 * determine whether the operation is Read or Write. 
 * Read Opr or Write with O_WR_REPL off:  
 *     Anything dirty - Include only dirty, throw out clean copies.
 *     Nothing dirty - include all copies.
 * Write with O_WR_REPL on - include all copies
 */
 
int
sortAndCombInfoForObj (struct mdasInfoOut **infoOutHead, 
struct mdasInfoOut **dirtyInfo, int openFlag, int replDirtyInfo)
{
    struct mdasInfoOut *dirtyCacheInfoOut, *dirtyArchInfoOut,
     *cacheInfoOut, *archInfoOut, *cacheInfoOut1, *archInfoOut1;

    /* no need to replicate dirtyInfo if it is read only */

    sortInfoByClass (*infoOutHead, &cacheInfoOut, &archInfoOut,
     &cacheInfoOut1, &archInfoOut1,
      &dirtyCacheInfoOut, &dirtyArchInfoOut);

    if (dirtyInfo != NULL)
        *dirtyInfo = NULL;

    if (openFlag & O_WR_REPL) {	   /* overwrite all replica */
	combineInfoOut (&dirtyCacheInfoOut, &dirtyArchInfoOut);
	if (dirtyInfo != NULL)
	    replInfoOut (dirtyInfo, dirtyCacheInfoOut);
        combineInfoOut (&dirtyCacheInfoOut, &cacheInfoOut);
        combineInfoOut (&dirtyCacheInfoOut, &cacheInfoOut1);
        combineInfoOut (&dirtyCacheInfoOut, &archInfoOut);
        combineInfoOut (&dirtyCacheInfoOut, &archInfoOut1);
        *infoOutHead = dirtyCacheInfoOut;
    } else {
	if (dirtyCacheInfoOut != NULL || dirtyArchInfoOut != NULL) {
            combineInfoOut (&dirtyCacheInfoOut, &dirtyArchInfoOut);
	    if (dirtyInfo != NULL && replDirtyInfo > 0)
                replInfoOut (dirtyInfo, dirtyCacheInfoOut);
	    *infoOutHead = dirtyCacheInfoOut;
	    freeAllInfoOut (cacheInfoOut);
	    freeAllInfoOut (cacheInfoOut1);
	    freeAllInfoOut (archInfoOut);
	    freeAllInfoOut (archInfoOut1);
	} else {
            combineInfoOut (&dirtyCacheInfoOut, &dirtyArchInfoOut);
            combineInfoOut (&dirtyCacheInfoOut, &cacheInfoOut);
            combineInfoOut (&dirtyCacheInfoOut, &cacheInfoOut1);
            combineInfoOut (&dirtyCacheInfoOut, &archInfoOut);
            combineInfoOut (&dirtyCacheInfoOut, &archInfoOut1);
            *infoOutHead = dirtyCacheInfoOut;
	}
    }
    return (0);
}

int
sortInfoForSyncD (struct mdasInfoOut **infoOutHead,
struct mdasInfoOut **dirtyInfoOut, 
struct mdasInfoOut **cleanInfoOut)
{
    struct mdasInfoOut *dirtyCacheInfoOut, *dirtyArchInfoOut,
     *cacheInfoOut, *archInfoOut, *cacheInfoOut1, *archInfoOut1;

    sortInfoByClass (*infoOutHead, &cacheInfoOut, &archInfoOut,
     &cacheInfoOut1, &archInfoOut1,
      &dirtyCacheInfoOut, &dirtyArchInfoOut);

    combineInfoOut (&dirtyCacheInfoOut, &dirtyArchInfoOut);
    combineInfoOut (&cacheInfoOut, &archInfoOut);
    combineInfoOut (&cacheInfoOut, &cacheInfoOut1);
    combineInfoOut (&cacheInfoOut, &archInfoOut1);

    *dirtyInfoOut = dirtyCacheInfoOut;
    *cleanInfoOut = cacheInfoOut;

    return 0;
}

int
replInfoOut (struct mdasInfoOut **infoOutHead, 
struct mdasInfoOut *inpInfoOut) 
{
    struct mdasInfoOut *tmpInfoOut, *tmpoInfoOut, *prevInfoOut;

    tmpInfoOut = inpInfoOut;
    *infoOutHead = NULL;
    tmpoInfoOut = NULL;
    while (tmpInfoOut != NULL) {
	tmpoInfoOut = (struct mdasInfoOut *)
            malloc (sizeof (struct mdasInfoOut));
	memset (tmpoInfoOut, 0, sizeof (struct mdasInfoOut));
        if (*infoOutHead == NULL) {
            *infoOutHead = tmpoInfoOut;
        } else  {
            prevInfoOut->next = tmpoInfoOut;
        }
        prevInfoOut = tmpoInfoOut;

        tmpoInfoOut->objTypeInx = tmpInfoOut->objTypeInx;

        tmpoInfoOut->objID = strdup (tmpInfoOut->objID);

        tmpoInfoOut->collection = strdup (tmpInfoOut->collection);

        tmpoInfoOut->dataPath = strdup (tmpInfoOut->dataPath);

        tmpoInfoOut->resourceLoc = strdup (tmpInfoOut->resourceLoc);

        tmpoInfoOut->resourceName = strdup (tmpInfoOut->resourceName);

        tmpoInfoOut->dataType = strdup (tmpInfoOut->dataType);

        tmpoInfoOut->replNum = tmpInfoOut->replNum;

        tmpoInfoOut->segNum = tmpInfoOut->segNum;

        tmpoInfoOut->offset = tmpInfoOut->offset;

        tmpoInfoOut->size = tmpInfoOut->size;

        tmpoInfoOut->phyResClass = strdup (tmpInfoOut->phyResClass);

        tmpoInfoOut->resClassInx = tmpInfoOut->resClassInx;

        tmpoInfoOut->containerName = strdup (tmpInfoOut->containerName);
        tmpoInfoOut->containerMaxSize = tmpInfoOut->containerMaxSize;

        tmpoInfoOut->isDirty = tmpInfoOut->isDirty;
        tmpoInfoOut->chksum = tmpInfoOut->chksum;

	if (tmpInfoOut->logResName != NULL)
            tmpoInfoOut->logResName = strdup (tmpInfoOut->logResName);

        tmpInfoOut = tmpInfoOut->next;
    }

    if (tmpoInfoOut != NULL)
        tmpoInfoOut->next = NULL;

    return (0);
}
    
int
queInfoOut (struct mdasInfoOut **topInfoOut, 
struct mdasInfoOut *infoOut, int singleFlag, int topFlag)
{
    struct mdasInfoOut *tmpInfoOut, *prevInfoOut;

    if (singleFlag > 0)		/* queue only one element */
        infoOut->next = NULL;
    tmpInfoOut = *topInfoOut;
    if (tmpInfoOut == NULL) {
        *topInfoOut = infoOut;
        return (0);
    }

    if (topFlag > 0) {	/* put on top of queue */
	tmpInfoOut = infoOut;
        /* Find the end of topInfoOut */
        while (tmpInfoOut != NULL) {
            prevInfoOut = tmpInfoOut;
            tmpInfoOut = tmpInfoOut->next;
	}
	prevInfoOut->next = *topInfoOut;
	*topInfoOut = infoOut;
    } else {
        /* Find the end of topInfoOut */
        while (tmpInfoOut != NULL) {
            prevInfoOut = tmpInfoOut;
            tmpInfoOut = tmpInfoOut->next;
        }

        /* link to the bottom */
        prevInfoOut->next = infoOut;
    }
    return (0);
}
    
int
sortInfoForRepl (struct mdasInfoOut **infoOut, char *resourceName)
{
    struct mdasInfoOut *matchCacheInfoOut, *matchArchInfoOut, *cacheInfoOut,
     *archInfoOut; 
    struct mdasInfoOut *nextInfoOut, *tmpInfoOut;
    int resClassInx;

    if (*infoOut == NULL)
	return 0;

    matchCacheInfoOut = matchArchInfoOut = cacheInfoOut =
    archInfoOut = NULL;

    tmpInfoOut = *infoOut;
    while (tmpInfoOut != NULL) {
        nextInfoOut = tmpInfoOut->next;
        resClassInx = tmpInfoOut->resClassInx;
        if (resClassEntries[resClassInx].permanent == 0) {
            if (strcmp (tmpInfoOut->resourceName, resourceName) == 0) {
            	queInfoOut (&matchCacheInfoOut, tmpInfoOut, 1, 0);
	    } else {
		queInfoOut (&cacheInfoOut, tmpInfoOut, 1, 0);
	    }
        } else {
            if (strcmp (tmpInfoOut->resourceName, resourceName) == 0) {
            	queInfoOut (&matchArchInfoOut, tmpInfoOut, 1, 0);
	    } else {
		queInfoOut (&archInfoOut, tmpInfoOut, 1, 0);
	    }
        }
        tmpInfoOut = nextInfoOut;
    }

    /* Combine them in the the order of matchCacheInfoOut, cacheInfoOut,
     * matchArchInfoOut and archInfoOut.
     */
    combineInfoOut (&matchCacheInfoOut, &cacheInfoOut);
    combineInfoOut (&matchCacheInfoOut, &matchArchInfoOut);
    combineInfoOut (&matchCacheInfoOut, &archInfoOut);

    *infoOut = matchCacheInfoOut;

    return (0);
}

int
sortResByClass (struct mdasResInfo *resInfoHead,
struct mdasResInfo **oPriCacheResInfo,
struct mdasResInfo **oPriArchResInfo,
struct mdasResInfo **oCacheResInfo, struct mdasResInfo **oArchResInfo)
{
    struct mdasResInfo *priCacheResInfo, *priArchResInfo, *cacheResInfo,
     *archResInfo;
    struct mdasResInfo *tmpResInfo, *nextResInfo;
    int resClassInx;


   priCacheResInfo = priArchResInfo = cacheResInfo =
    archResInfo = NULL;

    tmpResInfo = resInfoHead;
    while (tmpResInfo != NULL) {
	int queFlag;

	if (tmpResInfo->flag == LOCAL_HOST) {
	    queFlag = QUE_TOP;
	} else {
	    queFlag = QUE_BOTTOM;
	}
        resClassInx = resClassLookup (tmpResInfo->phyResClass);
        nextResInfo = tmpResInfo->next;
        if (resClassEntries[resClassInx].permanent == 1 &&
         resClassEntries[resClassInx].primary == 1) {
            queResInfo (&priArchResInfo, tmpResInfo, queFlag);
        } else if (resClassEntries[resClassInx].permanent == 0 &&
         resClassEntries[resClassInx].primary == 1) {
            queResInfo (&priCacheResInfo, tmpResInfo, queFlag);
        } else if (resClassEntries[resClassInx].permanent == 1 &&
         resClassEntries[resClassInx].primary == 0) {
            queResInfo (&archResInfo, tmpResInfo, queFlag);
        } else {
            queResInfo (&cacheResInfo, tmpResInfo, queFlag);
        }
        tmpResInfo = nextResInfo;
    }

    *oPriCacheResInfo = priCacheResInfo;
    *oPriArchResInfo = priArchResInfo;
    *oCacheResInfo = cacheResInfo;
    *oArchResInfo = archResInfo;

    return (0);
}

int
queResInfo (struct mdasResInfo **topResInfo,
struct mdasResInfo *resInfo, int flag)
{
    struct mdasResInfo *tmpResInfo, *prevResInfo;

    resInfo->next = prevResInfo = NULL;
    tmpResInfo = *topResInfo;
    if (tmpResInfo == NULL) {
        *topResInfo = resInfo;
        return (0);
    }

    if (flag == QUE_BOTTOM) {
        /* Find the end of topResInfo */
        while (tmpResInfo != NULL) {
            prevResInfo = tmpResInfo;
            tmpResInfo = tmpResInfo->next;
        }

        /* link to the bottom */
        prevResInfo->next = resInfo;
    } else if (flag == QUE_BY_FREE_SPACE) {
        while (tmpResInfo != NULL) {
	    if (resInfo->freeSpace > tmpResInfo->freeSpace) {
		if (prevResInfo == NULL) {
		    resInfo->next = *topResInfo;
		    *topResInfo = resInfo;
		} else {
		    resInfo->next = prevResInfo->next;
		    prevResInfo->next = resInfo;
		} 
		break;
	    }
	    prevResInfo = tmpResInfo;
            tmpResInfo = tmpResInfo->next;
        }
        /* link to the bottom */
	if (prevResInfo != NULL)
            prevResInfo->next = resInfo;
    } else {	/* queue it to top */
	resInfo->next = tmpResInfo;
	*topResInfo = resInfo;
    }

    return (0);
}

/* Combine 2 link list of mdasResInfo - put topResInfo on top and
 * botResInfo at bottom.
 */
int
combineResInfo (struct mdasResInfo **topResInfo,
struct mdasResInfo **botResInfo)
{
    struct mdasResInfo *tmpResInfo, *prevResInfo;

    tmpResInfo = *topResInfo;
    if (tmpResInfo == NULL) {
        *topResInfo = *botResInfo;
        *botResInfo = NULL;
        return (0);
    }

    /* Find the end of topResInfo */
    while (tmpResInfo != NULL) {
        prevResInfo = tmpResInfo;
        tmpResInfo = tmpResInfo->next;
    }

    /* john the 2 link list */
    prevResInfo->next = *botResInfo;
    *botResInfo = NULL;

    return (0);
}

/* svrContainerCreate - Entry point for the srbContainerCreate SID.
 *
 *
 * Input : int catType - catalog type
 *	   struct varlena *vContainerName - The object ID
 *	   struct varlena *vContainerType - Container data type
 *	   struct varlena *vResourceName - resource name
 *	   srb_long_t *containerSize - container size
 *
 * Output : Returns 0 - success, negative  - failure
 */

int
svrContainerCreate (int catType, struct varlena *vContainerName,
struct varlena *vContainerType, struct varlena *vResourceName,
srb_long_t * containerSize)
{
    int retVal;
    char *resourceType;
    char *logResName, *containerName, *containerType;
    srb_long_t myContainerSize;

    if (TicketUserFlag) {
	elog (NOTICE, "Illegal operation for a ticket user");
	return (ILLEGAL_OPR_TICKET_USER);
    }

    myContainerSize = *containerSize;
    logResName = varToStr (vResourceName);
    containerName = varToStr (vContainerName);
    containerType = varToStr (vContainerType);

    retVal = _svrContainerCreate (catType, containerName, containerType,
     logResName, myContainerSize);

    free (logResName);
    free (containerName);
    free (containerType);

    return (retVal);
}

int
_svrContainerCreate (int catType, char *inpContainerName, char *containerType,
char *logResName, srb_long_t containerSize)
{
    int retVal, status;
    int tinx;
    int exfInx;
    char *resourceType;
    char *userName;
    struct mdasResInfo *priCacheResInfo, *priArchResInfo,
     *cacheResInfo, *archResInfo;
    struct mdasResInfo *tmpResInfo;
    char fullPathName[MAX_TOKEN * 2];
    char compPathName[MAX_TOKEN * 2];
    char *myPathName;

    char collectionName[MAX_TOKEN];
    char fullContainerName[MAX_TOKEN];
    char containerName[MAX_TOKEN];
#ifdef _WIN32
    char *firstPathName;
#else
    char firstPathName[HUGE_STRING];
#endif
    char firstRes[MAX_TOKEN];
    int gotCopy = 0;
    struct mdasResInfo *compResInfo, *myResInfo;
    int intReplNum;
    int remoteFlag;
    struct hostElement  *hostTabPtr;
    struct resCache *myResCache;

    if (containerSize > MAX_CONTAINER_SIZE) {
	return (OBJ_ERR_MAX_SIZE_EXCEEDED);
    } else if (containerSize <= 0) {
	containerSize = DEF_CONTAINER_SIZE;
    }

#ifdef _WIN32
    firstPathName = (char *)calloc(HUGE_STRING,sizeof(char));
#endif

    /* get the mdasResInfo associated with a logResName and domainName */

    if ((status = getResInfo (inpContainerName, catType, logResName, 
     ClientUser->domainName, &myResCache)) < 0) {
        elog (NOTICE,
          "svrContainerCreate: getResInfo for reso %s failed, stat = %d",
          logResName, status);
#ifdef _WIN32
        free(firstPathName); firstPathName=NULL;
#endif
	if (status > CLI_ERR_COMMAND) 
	    return status;
	else
            return (OBJ_ERR_RES_NOT_REG);
    }

    sortResByClass (myResCache->resInfoHead, &priCacheResInfo, 
     &priArchResInfo,
     &cacheResInfo, &archResInfo);

    /* order - pri cache, pri arch, cache, arch */
    combineResInfo (&priCacheResInfo, &priArchResInfo);
    combineResInfo (&priCacheResInfo, &cacheResInfo);
    combineResInfo (&priCacheResInfo, &archResInfo);
    myResCache->resInfoHead = priCacheResInfo;

    userName = ClientUser->userName;

    /* The container collection */
    
    retVal = svrParseContainerName (inpContainerName, containerName, 
     collectionName, ClientUser->userName, ClientUser->domainName);

    if (retVal < 0) {
        elog (NOTICE,
         "svrContainerCreate: Error parsing container %s. status = %d",
          inpContainerName, retVal);
#ifdef _WIN32
        free(firstPathName); firstPathName=NULL;
#endif
        return retVal;
    }

    /* Loop through all permanent resources */
    tmpResInfo = myResCache->resInfoHead;
    while (tmpResInfo != NULL) {
	
	compResInfo = NULL;
        if (strcmp (tmpResInfo->phyResType, COMPOUND_RESC) == 0) {
            compResInfo = tmpResInfo->compoundLink;
            while (compResInfo != NULL) {
		int resClassInx;
                /* XXXXX don't do TAPE_RESC 
                if (strcmp (compResInfo->phyResType, TAPE_RESC) != 0) { */
                resClassInx = resClassLookup (compResInfo->phyResClass);
                /* Don't do permanent */
                if (resClassEntries[resClassInx].permanent == 0) {
                    exfInx = _objCreateWithResInfo (containerName, 
		     collectionName, "", containerSize, compResInfo, 
		     fullPathName, &tinx);
                    if (exfInx >= 0)
                        break;
                }
                compResInfo = compResInfo->next;
            }
        } else {

	    exfInx = _objCreateWithResInfo (containerName, collectionName,
	     "", containerSize, tmpResInfo, fullPathName, &tinx);
	}

        if (exfInx < 0) {	/* error */
	    retVal = exfInx;
            tmpResInfo = tmpResInfo->next;
            continue;
        }

        /* myPathName is the path that was actually used */
        if (compResInfo != NULL) {
            /* Create a fake pathName for the compound obj */
            genCompPath (compPathName, containerName);
            myPathName = compPathName;
        } else {
            myPathName = fullPathName;
        }

        /* Close the object */

        lowLevelClose (tinx, exfInx);
	
        /* Do the registration */


        /* If it is a compound onj, use the phyResName and phyResLoc of the
         * compound obj and the fake path name
         */

        retVal = _svrRegisterDataset (catType, containerName, 
         userName, "all", ClientUser->domainName, containerType, 
	  myPathName, tmpResInfo->phyResName, collectionName, 0,
	  tinx, tmpResInfo->phyResLoc);
      
        if (retVal < 0) {
	    break;
	}

        intReplNum = -1;
        if (compResInfo != NULL) {
            /* This is a internal compound resource */
            intReplNum = _svrRegInternalCompObj (
              containerName, collectionName, retVal, NO_INT_SEGMENTATION,
              compResInfo->phyResName, fullPathName, 0, 0,
              NEW_INT_REPL_NUM, NO_INT_SEGMENTATION,
              tinx, compResInfo->phyResLoc,
              ClientUser->userName, ClientUser->domainName);

            if (intReplNum < 0) {
                break;
            }
        }

	gotCopy ++;
	break;

    }

    if (retVal >= 0 && gotCopy > 0) {		/* no error */
        /* Register the container */

        sprintf (fullContainerName, "%s/%s", collectionName, containerName);

        remoteFlag = getMdasEnabledHost (fullContainerName, &hostTabPtr);

        if (remoteFlag < 0) {
	    retVal = remoteFlag;
        } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
            retVal = register_container_info (catType, fullContainerName,
             userName, ClientUser->domainName, logResName, containerSize);
            if (retVal >= 0)
                commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
        } else {
            retVal = remoteRegisterContainer (catType, fullContainerName,
             userName, ClientUser->domainName, logResName, containerSize,
	     hostTabPtr);
        }

        if (retVal < 0) {       /* failure, have to clean up after ourselves */
            /* Unregister the obj */
            _svrObjUnlink (containerName, collectionName);
        }
    } else if (retVal < 0) {	/* failure, have to clean up after ourselves */
        /* Unregister the obj */
        _svrObjUnlink (containerName, collectionName);
    }
	
#ifdef _WIN32
        free(firstPathName); firstPathName=NULL;
#endif
    return retVal;
}

/* svrContainerOpen - Entry point for the srbContainerOpen SID.
 *
 *
 * Input : int catType - catalog type
 *         struct varlena *vContainerName - The object ID
 *         int openFlag - Unix type open flag
 *
 * Output : Returns 0 - success, negative  - failure
 */

int
svrContainerOpen (int catType, struct varlena *vContainerName, int openFlag)
{
    char *containerName;
    int contInx;

    containerName = varToStr (vContainerName);
    contInx = _svrContainerOpen (catType, containerName, openFlag);
    free (containerName);
    return contInx;
}

int
_svrContainerOpen (int catType, char *containerName, int openFlag)
{
    char outContName[MAX_TOKEN], contCollection[MAX_TOKEN];
    int myOpenFlag;
    int contInx, retVal;
    char *accessMode;
    int lockType, lockFd;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    /* Take out flag such as O_APPEND, O_TRUNC etc for container IO
     */
    if (openFlag & O_WRONLY)
        myOpenFlag = O_WRONLY;
    else if (openFlag & O_RDWR)
        myOpenFlag = O_RDWR;
    else if (openFlag & O_RDONLY)
        myOpenFlag = O_RDONLY;
    else
        myOpenFlag = O_RDONLY;

    accessMode = openFlagLookUp (myOpenFlag);
    if (accessMode == NULL) {
        elog(NOTICE,"svrContainerOpen: openFlag - %x lookup failed",
        openFlag);
        return OBJ_ERR_OPEN_FLAG;
    }

    /* Check for TicketUser should have been done by the calling routine */

    retVal = svrParseContainerName (containerName, outContName,
     contCollection, ClientUser->userName, ClientUser->domainName);
    if (retVal < 0) {
        elog (NOTICE,
         "svrContainerOpen: Error parsing container %s. status = %d",
          containerName, retVal);
    	free (containerName);
        return (retVal);
    }


#ifdef LOCK_OBJ
    /* lock the container */
    if (strcmp (accessMode, "read") == 0)
        lockType = F_RDLCK;
    else
        lockType = F_WRLCK;

    /* Do a write lock through inContainerObjOpen */
    lockFd =_svrObjLock (outContName, contCollection, F_SETLKW, F_WRLCK, 0);
    if (lockFd < 0) {
        return (lockFd);
    }
#endif

    /* Do a mdasGetInfo on the container */

    contInx = resolveContainer (MDAS_CATALOG, myOpenFlag, 1, outContName,
     contCollection);

#ifdef LOCK_OBJ
    if (contInx >= 0) {
	srbObjDesc[contInx].lockFd = lockFd;
	srbObjDesc[contInx].contInfo->cleanOnClose = 1;
    } else {
	_svrObjLock (outContName, contCollection, F_SETLKW, F_UNLCK,
	 lockFd);
    }
#endif
    /* don't close container desc when an inContainer Obj is closed. Has
     * to be closed explicitly */

    if (contInx < 0) 
	return contInx;

    srbObjDesc[contInx].contInfo->cleanOnClose = 0;

    return contInx;
}

/* svrContainerClose - Entry point for the srbContainerClose SID.
 *
 *
 * Input : int descInx - The container descriptor index to close.
 *
 * Output : Returns 0 - success, negative  - failure
 */

int
svrContainerClose (int descInx)
{
    int status;

    if (descInx < 0 || descInx >= MAX_SRBO_DESC ||
    srbObjDesc[descInx].lowLevelDescInx < 0) {
        elog(NOTICE,"svrContainerClose: Invalid desc (%d)", descInx);
        return INP_ERR_DESC;
    }

    if (srbObjDesc[descInx].contInfo == NULL) {
	return (OBJ_ERR_OBJ_NOT_CONTAINER);
    }

    /* tell svrObjClose to close the file */
    srbObjDesc[descInx].contInfo->cleanOnClose = 1;
    status = svrObjClose (descInx);
    return (status);
}

/* svrObjOpen - Entry point for the srbObjOpen SID.
 *
 *
 * Input : struct varlena *vObjID - The object ID
 *         int openFlag - Unix type open flag
 *         struct varlena *vCollectionName - The collection name
 *
 * Output : Returns the Object descriptor - success, negative  - failure
 */

int
svrObjOpen (struct varlena *vObjID, 
int openFlag, struct varlena *vCollectionName)
{
    char *objID, *collectionName;
    int status;

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);

    status = _svrObjOpen (objID, openFlag, collectionName);

    free (objID);
    free (collectionName);

    return (status);
}

int 
_svrObjOpen (char *objID, int openFlag, char *collectionName)
{
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead, *infoOut, *tmpInfoOut;
    int retVal;
    struct condResult condResult;
    char *fullPathName = NULL;
    int  status;

    /* A ticket user is allowed to open with a read only flag */

    if (TicketUserFlag && openFlag != O_RDONLY) {
        elog (NOTICE, "Illegal operation for a ticket user, status = %d",
	  ILLEGAL_OPR_TICKET_USER);
        return (ILLEGAL_OPR_TICKET_USER);
    }

    infoInp.domainName = ClientUser->domainName;

    /* O_CREAT is not allowed. Use svrObjCreate to create an object */

    if (openFlag & O_CREAT) {
	openFlag = openFlag ^ O_CREAT;	/* take out the flag */
	elog(NOTICE,
	  "svrObjOpen: O_CREAT is not allowed. Use svrObjCreate instead");
	return OBJ_ERR_NO_CREATE_IN_OPEN;
    }

    /* change O_WRONLY to O_RDWR because chksum requires reading
     * the data back.
     */
    if (openFlag & O_WRONLY) {
	openFlag = openFlag ^ O_WRONLY;
	openFlag |= O_RDWR;
    }

    infoInp.accessMode = openFlagLookUp (openFlag);

    if (infoInp.accessMode == NULL) {
        elog(NOTICE,"svrObjOpen: openFlag - %x lookup failed", 
	openFlag);
	return OBJ_ERR_OPEN_FLAG;
    }

    infoInp.objID = objID;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = collectionName;

    /* Check with MDAS for the requested data */
    if ((objID[0] == '\0') ||
     (retVal = mdasGetInfo (&infoInp, &infoOutHead)) < 0) {
	char myPath[MAX_OBJ_NAME_LENGTH];
	mdasC_sql_result_struct *myresult;

	if (objID[0] != '\0') {
	    int mylen;
	    
	    mylen = strlen (collectionName);
	    if (collectionName[mylen - 1] == '/') {
		/* ends with a '/'  */
	    	sprintf (myPath, "%s%s", collectionName, objID); 
	    } else {
	    	sprintf (myPath, "%s/%s", collectionName, objID); 
	    }
	} else {
	    strcpy (myPath, collectionName);
	}

	if ((status = svrCollStat (MDAS_CATALOG, myPath, &myresult)) >= 0) {
	    if ((status = checkCollPerm (myresult, ProxyUser, ClientUser,
	     O_RDONLY)) >= 0) {
		retVal = svrGetCollDesc (myPath);
		if (retVal >= 0)
		    retVal |= IS_COLL_FD;
	    } 
	    freeSqlResult (myresult);
	    return retVal;
	} else {
	    return retVal;
	}
    }

    retVal = _svrObjOpenWithInfo (&infoInp, &infoOutHead, openFlag);

    if (retVal >= 0) {
	int myInx;

        if (parseCondInput (objID, REG_CHKSUM, &condResult) >= 0) {
            srbObjDesc[retVal].chksumFlag = k_FLAG;
            srbObjDesc[retVal].chksum = condResult.intResult;
        } else if (parseCondInput(objID, VERIFY_CHKSUM, &condResult) >= 0) {
            srbObjDesc[retVal].chksumFlag = K_FLAG;
            srbObjDesc[retVal].chksum = condResult.intResult;
        }
        /* have to initialize other desc too */
        if (srbObjDesc[retVal].chksumFlag != 0) {
            myInx = srbObjDesc[retVal].nextDescInx;
            while (myInx != -1) {
                srbObjDesc[myInx].chksumFlag = srbObjDesc[retVal].chksumFlag;
                srbObjDesc[myInx].chksum = srbObjDesc[retVal].chksum;
                myInx = srbObjDesc[myInx].nextDescInx;
            }
        }
    }

    /* Free any remaining infoOut in the rest of the list */

    freeAllInfoOut (infoOutHead);

    return (retVal);
}


int

_svrObjOpenWithInfo (struct mdasInfoInp *infoInp, 
struct mdasInfoOut **infoOutHead, int openFlag)
{
    struct mdasInfoOut *dirtyInfo;
    int lockFd = -1;
    int lockType;
    int retVal;


    if ((*infoOutHead)->containerName[0] != '\0') {  /* inContainer object */
#ifdef LOCK_OBJ
	char dataName[MAX_TOKEN], collection[MAX_TOKEN];

	/* lock the container */
	if (strcmp (infoInp->accessMode, "read") == 0)
	    lockType = F_RDLCK;
	else
	    lockType = F_WRLCK;

        retVal = svrParseContainerName ((*infoOutHead)->containerName, dataName, 
	 collection, ClientUser->userName, ClientUser->domainName);
        if (retVal < 0) {
            elog (NOTICE,
             "_svrObjOpenWithInfo: Error parsing container %s. status = %d",
              (*infoOutHead)->containerName, retVal);
            return (retVal);
        }

	/* Do a write lock through inContainerObjOpen */
	lockFd =_svrObjLock (dataName, collection, F_SETLKW, F_WRLCK, 0);
	if (lockFd < 0) {
	    return (lockFd);
	}
#endif
	    
	if (!myIsContainer (infoInp->objID, infoInp->collectionName, 
	 (*infoOutHead)->containerName)) {
            /* not a container, but a inContainer obj */
	    retVal = inContainerObjOpen (infoInp->objID, 
             infoInp->collectionName, openFlag, *infoOutHead);
#ifdef LOCK_OBJ
	    /* free the lock if it is a read. For container, a write 
	     * does not corrupt existing data. */
	    if (retVal >= 0 && lockType == F_WRLCK)
		srbObjDesc[srbObjDesc[retVal].contInx].lockFd = lockFd;
	    else
		_svrObjLock (dataName, collection, F_SETLKW, F_UNLCK,
		 lockFd);
#endif
	} else {
	  /**** RAJA CHANGED -- to allow opening containers as objects
	        June 27,2000 for Container BlastPut  
            elog (NOTICE,
	     "_svrObjOpenWithInfo: Cannot open a container %s/%s directly", 
	      infoInp->collectionName, infoInp->objID);
	    return (OBJ_ERR_OPEN_CONTAINER_NOT_ALLOWED);
	  ****/
	    sortAndCombInfo (infoOutHead);
    	    retVal = _objOpen (infoInp->objID, infoInp->collectionName,
             openFlag, *infoOutHead);
#ifdef LOCK_OBJ
            /* free the lock if it is a read */
            if (retVal >= 0 && lockType == F_WRLCK)
                srbObjDesc[retVal].lockFd = lockFd;
            else
                _svrObjLock (dataName, collection, F_SETLKW, F_UNLCK,
                 lockFd);
#endif

	}
    } else {
	int replDirtyInfo;
	char *tmpPtr;

	if (openFlag & O_RDONLY) {
            replDirtyInfo = 0;
	    tmpPtr = NULL;
	} else if ((tmpPtr = strstr(infoInp->objID, "&")) != NULL) {
	    replDirtyInfo = 0;
	} else {
	    replDirtyInfo = 1;
	}

	sortAndCombInfoForObj (infoOutHead, &dirtyInfo, openFlag,
	 replDirtyInfo);

	/* If objID has a "&" char, need to get dirtyInfo from the
	 * the list with all copies.
	 */

    	retVal = _objOpen (infoInp->objID, infoInp->collectionName,
         openFlag, *infoOutHead);
	if (retVal >= 0 && ~(openFlag & O_RDONLY)) {
	    if (tmpPtr != NULL) {
		int retVal1;
		struct mdasInfoOut *infoOutHead1;

	        *tmpPtr = '\0';	/* take out the '&' char to get all the 
		Info that are dirty */
		retVal1 = mdasGetInfo (infoInp, &infoOutHead1);
		if (retVal1 < 0) { 
		    elog (NOTICE, 
		     "_svrObjOpenWithInfo: mdasGetInfo error, status=%d",
		      retVal1);
		} else {
		    sortAndCombInfoForObj (&infoOutHead1, &dirtyInfo, 
		      openFlag, 1);
		    freeAllInfoOut (infoOutHead1);
		}
		*tmpPtr = '&';	/* put it back */
	    }
	    /* srbObjDesc[retVal].dirtyInfo = dirtyInfo; */
	    queInfoOut (&srbObjDesc[retVal].dirtyInfo, dirtyInfo, 0, 0);
	} else {
	    freeAllInfoOut (dirtyInfo);
	}
    }

    return (retVal);
}

int inContainerObjOpen (char *objID, char *collectionName, int openFlag,
struct mdasInfoOut *infoOutHead)
{
    struct mdasInfoOut *contInfoOut, *tmpInfoOut;
    int retVal, status;
    int descInx, contInx;
    char outContName[MAX_TOKEN], contCollection[MAX_TOKEN];
    int myOpenFlag;
    int mySize;

    /* Take out flag such as O_APPEND, O_TRUNC etc for container IO
     */
    if (openFlag & O_WRONLY)
	myOpenFlag = O_WRONLY;
    else if (openFlag & O_RDWR)
	myOpenFlag = O_RDWR;
    else 
	myOpenFlag = openFlag;

    /* Check for TicketUser should have been done by the calling routine */

    retVal = svrParseContainerName (infoOutHead->containerName, outContName, 
     contCollection, ClientUser->userName, ClientUser->domainName);
    if (retVal < 0) {
        elog (NOTICE,
         "inContainerObjOpen: Error parsing container %s. status = %d",
          infoOutHead->containerName, retVal);
        return (retVal);
    }

    /* Do a mdasGetInfo on the container */

    contInx = resolveContainer (MDAS_CATALOG, myOpenFlag, 0, outContName, 
     contCollection);

    if (contInx < 0)
        return (contInx);

#ifdef WRITE_SYNC
    mySize = _getFileSize (srbObjDesc[contInx].lowLevelDescInx);
    if (mySize != srbObjDesc[contInx].contInfo->curContainerSize) {
        elog(NOTICE,
#if defined(PORTNAME_osx)
         "inContainerObjOpen: %s File size %d not equal to MCAT %qd ",
#else
         "inContainerObjOpen: %s File size %d not equal to MCAT %lld ",
#endif
         outContName, mySize, srbObjDesc[contInx].contInfo->curContainerSize);
        return (OBJ_ERR_COPY_LEN);
    }
#endif	/* WRITE_SYNC */

    /* create a ObjDesc for the inContainer object */

    descInx = newObjDesc ();

    if (descInx < 0) {
        svrObjClose (contInx);
        return (descInx);
    }

    fillObjDescWithInContInfo (descInx, objID, collectionName,
     infoOutHead->offset, infoOutHead->size, contInx, 
      srbObjDesc[contInx].lowLevelDescInx, infoOutHead->chksum);

    /* Seek to the baseOffset */

    status = _svrObjSeek (descInx, 0, SEEK_SET);

    if (status < 0) {
        elog(NOTICE, "inContainerObjOpen: _svrObjSeek error, status = %d ",
         status);
	svrObjClose (descInx);
        return status;
    }

    return descInx;
}

/* _objOpen - Internal ObjOpen.
 * 
 *  int wrReplFlag- Flag to indicate whether replca should be sync 
 */

int _objOpen (char *inpObjID, char *collectionName, int openFlag, 
struct mdasInfoOut *infoOutHead)
{
    struct mdasInfoOut *infoOut;
    char *accessMode;
#ifdef DEMO
    int copyNum = 1;
#endif
    int tinx;
    int exfInx, descInx;
    char comment[MAXEXFPATH];
    int failCnt = 0;
    int headDescInx = -1;
    int tmpDescInx, tmpDescInx1;
    int copyCnt = 0;
    int wrReplFlag;     /* Flag to indicate whether replca should be sync */
    struct mdasInfoOut *cacheInfo = NULL;
    int lockType; 
    int lockFd = -1;
   
    if (openFlag & O_WR_REPL) {
        wrReplFlag = 1;
        /* take out the O_WR_REPL flag because it is not a real unix
         * open flag.
         */
        openFlag = (openFlag & (~O_WR_REPL)) | O_RDWR;
    } else {
        wrReplFlag = 0;
    }

    accessMode = openFlagLookUp (openFlag);

    if (accessMode == NULL) {
        elog(NOTICE,"_objOpen: openFlag - %x lookup failed",
        openFlag);
        return OBJ_ERR_OPEN_FLAG;
    }

    resolveInfoPathName (inpObjID, collectionName, infoOutHead);

    infoOut = infoOutHead;
    while (infoOut != NULL) {

        /* Call the open at the next level, Loop through the infoOutHead 
	 * chain */

    	tinx = infoOut->objTypeInx;

	if (srbObjTypeEntry[tinx].objType == COMPOUND_TYPE) {
	    /* a compound object */
#ifdef LOCK_OBJ
	    /* Don't lock container object. It has its own locking */
	    if (infoOut->containerName[0] == '\0' && lockFd < 0) { 
	        if (openFlag & O_RDONLY) 
                    lockType = F_RDLCK;
                else
                    lockType = F_WRLCK;

                lockFd =_svrObjLock (infoOut->objID, collectionName, 
		  F_SETLKW, lockType, 0);
	    }
#endif
	    exfInx = _svrStageCompObj (infoOut, collectionName, 
	      &cacheInfo, 1);
	    if (exfInx >= 0) {
		exfInx = lowLevelOpen (cacheInfo->objTypeInx, 
		 cacheInfo->dataPath, cacheInfo->resourceLoc, openFlag);
	    }
	} else {
	    exfInx = lowLevelOpen (tinx, infoOut->dataPath, 
	     infoOut->resourceLoc, openFlag);
	}

        if (exfInx < 0) {       /* failure */
#ifdef LOCK_OBJ
            if (lockFd >= 0) {
                _svrObjLock (infoOut->objID, collectionName, 
                  F_SETLKW, F_UNLCK, lockFd);
		lockFd = -1;
	    }
#endif
	    /* Do a audit trail */
	    if (set_audit_trail (GET_AUDIT_TRAIL_SETTING) == AUDIT_TRAIL_ON) {
		sprintf (comment, "SRB: Open error, status = %d\n", exfInx);
	    	_svrObjAudit (MDAS_CATALOG, ClientUser->userName, inpObjID,
		collectionName, infoOut->dataPath, 
		infoOut->resourceName, accessMode, comment, 
		0, ClientUser->domainName);
	    }
	    descInx = exfInx;
            failCnt ++;
            if (wrReplFlag > 0)
                break;
            else {
	        if (wrReplFlag == 0 && copyCnt > 0)
	            break;
	        infoOut = infoOut->next;
		continue;
	    }
        } else {                        /* success */
    	    /* Do a audit trail */
    	    if (set_audit_trail (GET_AUDIT_TRAIL_SETTING) == AUDIT_TRAIL_ON) {
        	sprintf (comment, "SRB: SRB Everywhere !!!\n");
        	_svrObjAudit (MDAS_CATALOG, ClientUser->userName, inpObjID,
        	  collectionName, infoOut->dataPath,
        	  infoOut->resourceName, accessMode, comment,
        	  1, ClientUser->domainName);
            }
        }

        /* Fill out a srbObjDesc */
 
    	descInx = newObjDesc ();
    	if (descInx < 0) {
            failCnt ++;
            if (wrReplFlag > 0)
                break;
            else {
	        if (wrReplFlag == 0 && copyCnt > 0)
	            break;
	        infoOut = infoOut->next;
		continue;
	    }
	}

        /* Link it in a singlely linked chain */

        if (headDescInx == -1) {
            headDescInx = descInx;
        } else {
            tmpDescInx = headDescInx;
            while (srbObjDesc[tmpDescInx].nextDescInx != -1)
                tmpDescInx = srbObjDesc[tmpDescInx].nextDescInx;
            srbObjDesc[tmpDescInx].nextDescInx = descInx;
        }

        if (cacheInfo != NULL) {
    	    /* fillObjDescWithInfo (descInx, exfInx, tinx, collectionName, 
      	      cacheInfo); */
	    /* Use the phyResName of the compound obj instead of the int
             * compound obj. Use in _svrObjClose for mcat modification.
             */
	    fillObjDesc (descInx, exfInx, cacheInfo->objTypeInx, 
	      cacheInfo->objID, collectionName, 
	      cacheInfo->dataPath, infoOut->resourceName, infoOut->replNum);
	    srbObjDesc[descInx].compDataPath = strdup (infoOut->dataPath);
	    srbObjDesc[descInx].objSize = cacheInfo->size;
	    srbObjDesc[descInx].baseOffset = cacheInfo->offset;
	    srbObjDesc[descInx].segNum = cacheInfo->segNum;
	    srbObjDesc[descInx].markedDirty = infoOut->isDirty;
	    srbObjDesc[descInx].chksum = infoOut->chksum;
	    srbObjDesc[descInx].intReplNum = cacheInfo->intReplNum;
            srbObjDesc[descInx].intSegNum = NO_INT_SEGMENTATION;
	    freeAllInfoOut (cacheInfo);
	} else {
    	    fillObjDescWithInfo (descInx, exfInx, tinx, collectionName, 
      	      infoOut);
	}

#ifdef LOCK_OBJ
	if (lockFd >= 0)
            srbObjDesc[descInx].lockFd = lockFd;
#endif

	srbObjDesc[descInx].nextDescInx = -1;
        copyCnt++;

	if (wrReplFlag == 0 && copyCnt > 0)
	    break;
        infoOut = infoOut->next;
    }

    /* If the open failed and wrReplFlag > 0, close any opened desc */

    if (descInx < 0 && wrReplFlag > 0) {
            tmpDescInx = headDescInx;
            while (tmpDescInx != -1) {
                tinx = srbObjDesc[tmpDescInx].objTypeInx;
                lowLevelClose (tinx, srbObjDesc[tmpDescInx].lowLevelDescInx);
		tmpDescInx1 = tmpDescInx;
                tmpDescInx = srbObjDesc[tmpDescInx].nextDescInx;
                delObjDesc (tmpDescInx1);
            }
    }

    if (descInx >= 0)
	return headDescInx;	/* success - return the head */
    else
        return descInx;
}

struct varlena *
svrObjProxyOpr (ProxyOprId operation, 
int inputInt1, int inputInt2, int inputInt3, int inputInt4, 
struct varlena *vInputStr1, struct varlena *vInputStr2,
struct varlena *vInputStr3, struct varlena *vInputStr4,
struct varlena *vInputStr5, struct varlena *vInputStr6,
struct varlena *vInputStr7)
{
    struct varlena *retVal;
    int funcInx;
    char *inputStr1, *inputStr2, *inputStr3, *inputStr4, *inputStr5,
     *inputStr6, *inputStr7;
#ifdef ADDR_64BIT
    srb_long_t myRetLong;
#endif

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
	retVal = genIntRet (ILLEGAL_OPR_TICKET_USER);
	return (retVal);
    }

    for (funcInx = 0; funcInx < nProxyOpr; funcInx++) {
	if (proxyFuncEntries[funcInx].oprId == operation)
	    break;
    }

    if (funcInx >= nProxyOpr) {
	retVal = genIntRet (OBJ_ERR_PROXY_OPR_NUM);
        return (retVal);
    }

    inputStr1 = varToStr (vInputStr1);
    inputStr2 = varToStr (vInputStr2);
    inputStr3 = varToStr (vInputStr3);
    inputStr4 = varToStr (vInputStr4);
    inputStr5 = varToStr (vInputStr5);

#ifdef ADDR_64BIT
    myRetLong = (srb_long_t)
     proxyFuncEntries[funcInx].func (inputInt1, inputInt2, inputInt3,
      inputInt4, inputStr1, inputStr2, inputStr3, inputStr4,
       inputStr5, vInputStr6, vInputStr7);
    retVal = (struct varlena *) myRetLong;
#else
    retVal = (struct varlena *)
     proxyFuncEntries[funcInx].func (inputInt1, inputInt2, inputInt3,
      inputInt4, inputStr1, inputStr2, inputStr3, inputStr4, 
       inputStr5, vInputStr6, vInputStr7);
#endif

    free (inputStr1);
    free (inputStr2);
    free (inputStr3);
    free (inputStr4);
    free (inputStr5);

    return (retVal);
}

/* proxyCopy - Handle proxy copy.
 *
 * Input : int sourceDesc - The source descriptor.
 *
 *         int destDesc - The destination descriptor.
 *
 * Output - return the number of bytes copied.
 */

struct varlena * 
proxyCopy (int inputInt1, int inputInt2, int inputInt3, int inputInt4,
char *inputStr1, char *inputStr2, char *inputStr3, char *inputStr4, 
struct varlena *vInputStr1, struct varlena *vInputStr2, 
struct varlena *vInputStr3)
{
    int nbytes;
    srb_long_t bytesCopied = 0;
    int sourceExfInx, destExfInx;
    int sourceTypeInx, destTypeInx;
#ifdef _WIN32
    char *buf;
#else
    char buf[BUF_SIZE];
#endif
    int sourceDesc, destDesc;
    struct varlena *retval;
    int byteWritten;


    destDesc = inputInt2;
    sourceDesc = inputInt1;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
	retval = genIntRet (ILLEGAL_OPR_TICKET_USER);
        return (retval);
    }

#ifdef _WIN32
    buf = (char *)calloc(BUF_SIZE,sizeof(char));
#endif

    /* Use _svrObjRead and _svrObjWrite which deal with inContainer obj */

    while ((nbytes = _svrObjRead (sourceDesc, buf, BUF_SIZE)) > 0) {
        if ((byteWritten = _svrObjWrite (destDesc, buf, nbytes)) != nbytes) {
            elog (NOTICE,
              "proxyCopy: Bytes written and read don't match, read = %d, written = %d", nbytes, byteWritten);
	    if (byteWritten < 0) {
		retval = genIntRet (byteWritten);
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
		return (retval);
	    } else {
		retval = genIntRet (OBJ_ERR_COPY_LEN);
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
                return (retval);
	    }
        }
        bytesCopied += nbytes;
    }

    if (bytesCopied > 0) {
	if (bytesCopied == srbObjDesc[sourceDesc].objSize) {
	    bytesCopied = 0;
	    if (srbObjDesc[sourceDesc].chksum > 0) {
		srbObjDesc[destDesc].chksum = srbObjDesc[sourceDesc].chksum;
		srbObjDesc[destDesc].chksumFlag = k_FLAG;
	    }
	} else {
	    elog (NOTICE,
              "proxyCopy: Len copies %lld does not match src %lld for %s/%s", 
	      bytesCopied, srbObjDesc[sourceDesc].objSize,
	      srbObjDesc[destDesc].collection, srbObjDesc[destDesc].objID);
	    bytesCopied = OBJ_ERR_COPY_LEN;
	}
    }
#ifdef _WIN32
    free(buf); buf = NULL;
#endif
    retval = genIntRet (bytesCopied);
    return (retval);
}

/* proxyCommand - Handle proxy command operation.
 *
 * 
 */

struct varlena *
proxyCommand (int portalFlag, int connPort, int resolvePath, int inputInt4,
char *command, char *commandArgv, char *proxyAddr, char *clientIP,
char *srbPath, struct varlena *vInputStr1, struct varlena *vInputStr2)
{
    char dataName[MAX_TOKEN];
    char collectionName[MAX_TOKEN];
    char newCommandArgv[MAX_TOKEN];
    char remoteHost[MAX_TOKEN];
    struct hostElement  *hostTabPtr;
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead;
    struct varlena *retval;
    int status;
    int lsock = -1;
    int remoteFlag;
    int pid;

    /* Security check of the command path */
    if (strchr (command, '/') != NULL) {
	elog (NOTICE, "proxyCommand: command name %s is illegal",
	 command);
    	retval = genIntRet (OBJ_ERR_COMMAND_PATH);
    	return (retval);
    }

    /* Get the real unix path and host where data resides */
    /* Only do this if it hasn't been done already on other host,
       resolvePath portion */
    if (resolvePath && strcmp(srbPath, "")) {
        /* Prep for query, query, and sort */
        splitbychar(srbPath, collectionName,dataName,'/');
        infoInp.userName = ClientUser->userName;
        infoInp.domainName = ClientUser->domainName;
        infoInp.objID = dataName;
        infoInp.collectionName = collectionName;
        infoInp.accessMode = openFlagLookUp (O_RDONLY);
        status = mdasGetInfo(&infoInp, &infoOutHead);
        if (status < 0) {
            retval = genIntRet (status);
            return (retval);
        }
        sortAndCombInfo (&infoOutHead);

        /* Add path info to commandArgv */
        strcpy(newCommandArgv, infoOutHead->dataPath);
        strcat(newCommandArgv, " ");
        strcat(newCommandArgv, commandArgv);
        commandArgv = newCommandArgv;

        /* Change the host to the same host where data resides */
        proxyAddr = strcpy(remoteHost, infoOutHead->resourceLoc);

        /* Free memory and make sure not to resolve path again */
        freeAllInfoOut (infoOutHead);
        resolvePath = 0;
    }

    /* Remote proxy operation */
    if (strcmp(proxyAddr, "")) {
#ifdef FED_MCAT
        remoteFlag = chkHostAndZone (proxyAddr, &hostTabPtr);
#else
	remoteFlag = chkHost (proxyAddr, NULL, &hostTabPtr);
#endif
    	if (remoteFlag < 0) {
            elog (NOTICE, "proxyCommand: Invalid host addr %s", proxyAddr);
	    retval = genIntRet (INP_ERR_HOST_ADDR);
             return retval;             
        } else if (remoteFlag == REMOTE_HOST) {	/* remote proxy */
	    status = remoteProxyOpr (hostTabPtr, OPR_COMMAND, 
	     portalFlag, connPort, resolvePath, inputInt4,
	      command, commandArgv, proxyAddr, clientIP,
	       srbPath, NULL, NULL, NULL);
    	    retval = genIntRet (status);
    	    return (retval);
	}
    }


    /* Local proxy operation */
/* Bing Bing: will be supported later. */
#ifndef _WIN32
    if ((pid = FORK()) == 0) {  /* child */
        if (portalFlag > 0) {
	    lsock = portalConnect (connPort, clientIP);
	    if (lsock < 0) {
		elog (NOTICE, 
		 "proxyCommand: portalConnect() failed, status = %d", lsock);
		exit(1);
	    }
	}
        execCommand (command, commandArgv, lsock, portalFlag);
        exit(0);
    } 
#endif

    /* in parent */

    status = 0;
    retval = genIntRet (status);
    return (retval);
}
	     

void
freeInfoOut (struct mdasInfoOut *infoOut)
{
#ifdef TAPE_DRIVE
    tapeFileInfo_t *tmpTapeFileInfo;
#endif

    if (infoOut == NULL)
	return;
    if (infoOut->objID != NULL)
	free (infoOut->objID);

    if (infoOut->collection != NULL)
	free (infoOut->collection);

    if (infoOut->dataPath != NULL)
        free (infoOut->dataPath);

    if (infoOut->resourceLoc != NULL)
        free (infoOut->resourceLoc);

    if (infoOut->resourceName != NULL)
        free (infoOut->resourceName);

    if (infoOut->dataType != NULL)
        free (infoOut->dataType);

    if (infoOut->phyResClass != NULL)
        free (infoOut->phyResClass);

    if (infoOut->containerName != NULL)
        free (infoOut->containerName);

    if (infoOut->logResName != NULL)
        free (infoOut->logResName);

#ifdef TAPE_DRIVE
    tmpTapeFileInfo = infoOut->tapeFileInfo;
    while (tmpTapeFileInfo != NULL) {
	tapeFileInfo_t *thisTapeFileInfo;

	thisTapeFileInfo = tmpTapeFileInfo;
	tmpTapeFileInfo = tmpTapeFileInfo->next;
	free (thisTapeFileInfo);
    }
#endif
    if (infoOut->compoundLink != NULL) {
	freeAllInfoOut (infoOut->compoundLink);
    }

    free (infoOut);
}

void
freeAllInfoOut (struct mdasInfoOut *infoOutHead)
{
    struct mdasInfoOut *tmpInfoOut, *infoOut;

    infoOut = infoOutHead;
    while (infoOut != NULL) {
        tmpInfoOut = infoOut->next;
        freeInfoOut (infoOut);
        infoOut = tmpInfoOut;       /* Try the next one */
    }
}

void
freeResInfo (struct mdasResInfo *resInfo)
{
    struct mdasResInfo *tmpResInfo, *tmpResInfo1;

    tmpResInfo = resInfo;

    while (tmpResInfo != NULL) {
	free (tmpResInfo->phyResType);
	free (tmpResInfo->phyResLoc);
	free (tmpResInfo->phyResName);
	free (tmpResInfo->phyDirPathName);
	free (tmpResInfo->logDirPathName);
	free (tmpResInfo->phyResClass);
	if (tmpResInfo->compoundLink != NULL) {
	    freeResInfo (tmpResInfo->compoundLink);
	}

	tmpResInfo1 = tmpResInfo;
	tmpResInfo = tmpResInfo->next;
	free (tmpResInfo1);
    }
}

/* svrObjClose - Entry point for the srbObjClose SID.
 *
 *
 * Input : int descInx - The object descriptor index to close.
 *
 * Output : Returns 0 - success, negative  - failure
 */

int 
svrObjClose (int descInx)
{
    int status;
    int oldInx;


    if (descInx < 0 || descInx >= MAX_SRBO_DESC || 
    srbObjDesc[descInx].lowLevelDescInx < 0) {
        elog(NOTICE,"svrObjClose: Invalid desc (%d)", descInx);
        return INP_ERR_DESC;
    }

    if (srbObjDesc[descInx].isDir != IS_FILE) {
	svrCollClose (descInx);
	return 0;
    }

    while (descInx != -1) {
	int lowLevelDescInx;
	int myDesc;

        if (srbObjDesc[descInx].contInx >= 0) {
            myDesc = srbObjDesc[descInx].contInx;
        } else {
            myDesc = descInx;
        }

        /* tinx = srbObjDesc[myDesc].objTypeInx; */
        status = _svrObjClose (descInx);
	/* srbObjDesc[myDesc].lowLevelDescInx, tinx); */
#ifdef LOCK_OBJ
	if (srbObjDesc[myDesc].lockFd >= 0 && 
	  (srbObjDesc[myDesc].contInfo == NULL ||
	   srbObjDesc[myDesc].contInfo->cleanOnClose) > 0) {
	    _svrObjLock (srbObjDesc[myDesc].objID, 
	     srbObjDesc[myDesc].collection, F_SETLKW, F_UNLCK, 
	      srbObjDesc[myDesc].lockFd);
	}
#endif	/* LOCK_OBJ */

	oldInx = descInx;
	descInx = srbObjDesc[descInx].nextDescInx;
        delObjDesc (oldInx);
	if (myDesc != oldInx) {		/* Incontainer obj */
	    /* del the container desc ? */
	    if (srbObjDesc[myDesc].contInfo->cleanOnClose > 0)
		delObjDesc (myDesc);
	}
    }

    return status;
}

int
_svrObjClose (int descInx)
{
    srb_long_t size;
    char strSize[MAX_TOKEN];
    char compPath[2 * MAX_TOKEN];
    char *myPath;
    int status;
    struct mdasInfoOut *tmpInfoOut;
    int tmpInx;
    int objTypeInx, lowLevelDescInx;
    int contInx;
    int fileType;
    srb_long_t incontSize = -1;

    fileType = parseObjDesc (descInx, &objTypeInx, &lowLevelDescInx, &contInx);

    if (fileType < 0) {
	return (fileType);
    }

    if (lowLevelDescInx < 0 || objTypeInx < 0) {
        elog(NOTICE,"_svrObjClose: lowLevelDescInx or objTypeInx out of range");
        return INP_ERR_DESC;
    }

    if (lowLevelDescInx == DESC_TAKEN) {
        /* an opened collection */
        return (0);
    }

    /* should,nt have to check if _getWriteSize () has _dbLobjWriteSize */
    if (srbObjTypeEntry[objTypeInx].objType == FILE_TYPE) {
        size = _getWriteSize (objTypeInx, lowLevelDescInx);
    } else {
        size =  srbObjDesc[descInx].objSize;
    }

    if (size <= 0) {
	/* no size change */

        if (fileType == NORMAL_FILE_TYPE ||          /* normal obj */
         fileType == CONTAINER_FILE_TYPE ||   /* a container */
          srbObjDesc[contInx].contInfo->cleanOnClose > 0)
            status = _objClose (objTypeInx, lowLevelDescInx);
	else
	    status = 0;
	return (status);
    }

    /* Get here when the object has been written into,register the new size.  */

    if (fileType == INCONTAINER_FILE_TYPE) {
        /* InContainer obj */
        if (size < srbObjDesc[descInx].baseOffset) {
            elog (NOTICE,
#if defined(PORTNAME_osx)
             "_svrObjClose: Inconsistent container size, baseOffset = %qd, container size = %qd", srbObjDesc[descInx].baseOffset, size);
#else
             "_svrObjClose: Inconsistent container size, baseOffset = %lld, container size = %lld", srbObjDesc[descInx].baseOffset, size);
#endif
            return (OBJ_ERR_CONTAINER_SIZE);
        }

        /* Calculate the size of the inContainer object */
        incontSize = size - srbObjDesc[descInx].baseOffset;

        /* Register the inContainer Object */
#if defined(PORTNAME_osx)
        sprintf (strSize, "%-qd", incontSize);
#else
        sprintf (strSize, "%-lld", incontSize);
#endif
        status = _svrModifyDataset (MDAS_CATALOG,
          srbObjDesc[descInx].objID,
          srbObjDesc[descInx].collection,
          "", "", strSize, NULL, D_CHANGE_SIZE);
        if (status < 0) {
            elog (NOTICE,
             "_svrObjClose: Unable to register size change, status = %d", 
	      status);

            return (status);
        }
    }

    if (contInx >= 0) {    /* Container or inContainer Object */
        tmpInx = contInx;
    } else {
        tmpInx = descInx;
    }

    /* The object has been written into, register the new size.  */

#if defined(PORTNAME_osx)
    sprintf (strSize, "%-qd", size);
#else
    sprintf (strSize, "%-lld", size);
#endif
    if (srbObjDesc[tmpInx].intReplNum >= 0) {
        /* a internal compound object */
        status = _svrModInternalCompObj (
          srbObjDesc[tmpInx].objID,
          srbObjDesc[tmpInx].collection,
          srbObjDesc[tmpInx].replNum,
          NO_INT_SEGMENTATION,
          srbObjDesc[tmpInx].intReplNum,
          srbObjDesc[tmpInx].intSegNum,
	  ClientUser->userName, ClientUser->domainName,
	  strSize, "1",
	  NULL, NULL,
	  D_COMPOUND_MODIFY_SIZE_AND_DIRTY);
        if (status < 0) {
            elog (NOTICE,
         "_svrObjClose: Unable to modify int compound obj %s, status = %d", 
          srbObjDesc[tmpInx].objID, status);
            status = _objClose (objTypeInx, lowLevelDescInx);
            return (status);
        }
	myPath = srbObjDesc[tmpInx].compDataPath;
    } else {
	myPath = srbObjDesc[tmpInx].dataPath;
    }

    status = _svrModifyDataset (MDAS_CATALOG,
     srbObjDesc[tmpInx].objID,
     srbObjDesc[tmpInx].collection,
     srbObjDesc[tmpInx].resourceName,
     myPath,
     strSize, NULL, D_CHANGE_SIZE);

    if (status < 0) {
        elog (NOTICE,
         "_svrObjClose: Unable to register size change, status = %d", 
        status);
        status = _objClose (objTypeInx, lowLevelDescInx);
        return (status);
    }

    /* need to chksum ? */
    if ( srbObjDesc[descInx].chksum > 0) {
	char chksum[CHKSUM_STR_LEN + 1];
	char *chksumPath;

	if (srbObjDesc[descInx].chksumFlag & K_FLAG) {
            if (incontSize > 0) {
                status = _objChksum (objTypeInx, lowLevelDescInx, 0, chksum,
	         srbObjDesc[descInx].baseOffset, incontSize);
	    } else {
                status = _objChksum (objTypeInx, lowLevelDescInx, 0, chksum, 
		 0, 0);
            }
            if (status < 0) {
                elog (NOTICE,
             "_svrObjClose: _objChksum error for obj %s, status = %d",
              srbObjDesc[descInx].objID, status);
                status = _objClose (objTypeInx, lowLevelDescInx);
                return (status);
            }
	    if (srbObjDesc[descInx].chksum != atoi (chksum)) {
		elog (NOTICE, 
		 "_svrObjClose: chksum of %s does not match the given %d",
		 chksum, srbObjDesc[descInx].chksum);
                status = _objClose (objTypeInx, lowLevelDescInx);
		return (OBJ_ERR_BAD_CHKSUM);
	    } 
	} else if (srbObjDesc[descInx].chksumFlag &k_FLAG) {
	    sprintf (chksum, "%d", srbObjDesc[descInx].chksum);
	} else {
	    sprintf (chksum, "0");
	}

	/* register the chksum */
	if (srbObjDesc[descInx].intReplNum >= 0) {
	    chksumPath = srbObjDesc[descInx].compDataPath;
	} else {
	    chksumPath = srbObjDesc[descInx].dataPath;
	}

	status = _svrModifyDataset (MDAS_CATALOG,
          srbObjDesc[descInx].objID,
          srbObjDesc[descInx].collection,
          srbObjDesc[descInx].resourceName,
	  chksumPath,
          chksum,"", D_INSERT_DCHECKSUM);
        if (status < 0) {
            elog (NOTICE,
             "_svrObjClose: Unable to reg chksum for %s/%s, status = %d",
             srbObjDesc[descInx].collection, srbObjDesc[descInx].objID, status);
            return (status);
        }
    }

    status = lowLevelClose (objTypeInx, lowLevelDescInx);

    /* Marking the container dirty. */
    if (srbObjDesc[tmpInx].markedDirty == 0) {

        status = _svrModifyDataset (MDAS_CATALOG,
         srbObjDesc[tmpInx].objID,
         srbObjDesc[tmpInx].collection,
         srbObjDesc[tmpInx].resourceName,
	 myPath,
         "1", NULL, D_CHANGE_DIRTY);
        if (status < 0) {
            elog (NOTICE,
             "_svrObjClose: Unable to reg container to dirty, status = %d",
	     status);
            return (status);
        }
        srbObjDesc[tmpInx].markedDirty = 1;
    }

    /* mark all other dirty copies not dirty */
    tmpInfoOut = srbObjDesc[tmpInx].dirtyInfo;
    while (tmpInfoOut != NULL) {
	int myInx;

	/* XXXX 1/19/05 - take care of container myInx = descInx; */
	myInx = tmpInx;
	while (myInx != -1) {
	    if (srbObjDesc[myInx].replNum < 0 ||
             srbObjDesc[myInx].replNum == tmpInfoOut->replNum) 
		break;
	    myInx = srbObjDesc[myInx].nextDescInx;
	}

	if (myInx == -1) {	/* no match */
            status = _svrModifyDataset (MDAS_CATALOG,
             srbObjDesc[tmpInx].objID,
             srbObjDesc[tmpInx].collection,
             tmpInfoOut->resourceName,
             tmpInfoOut->dataPath,
             "0", NULL, D_CHANGE_DIRTY);
	}

        if (status < 0)
            elog (NOTICE,
             "_svrObjClose: Unable to reg container clean, status = %d", 
	      status);

        tmpInfoOut = tmpInfoOut->next;
    }

    /* free dirtyInfo so that we don't do it
     * again next time.
     */
    if (srbObjDesc[tmpInx].dirtyInfo != NULL) {
        freeAllInfoOut (srbObjDesc[tmpInx].dirtyInfo);
        srbObjDesc[tmpInx].dirtyInfo = NULL;
    }

    return status;
}

int 
_objClose (int objTypeInx, int lowLevelDescInx)
{
int status;

    if (lowLevelDescInx < 0 || objTypeInx < 0) {
        elog(NOTICE,"_objClose: lowLevelDescInx or objTypeInx out of range");
        return INP_ERR_DESC;
    }
#ifdef LOCK_DEBUG
    elog (NOTICE, "_objClose: %d", lowLevelDescInx);
#endif
    switch (srbObjTypeEntry[objTypeInx].objType) {
      case FILE_TYPE:
	status = svrFileClose (lowLevelDescInx);
	break;
      case DB_LOBJ_TYPE:
            status = dbLobjClose (lowLevelDescInx);
            break;
      case DB_TABLE_TYPE:
            status = dbTableClose (lowLevelDescInx);
            break;
      default:                /* shouldn't get here */
            elog(NOTICE, "_objClose: error in srbObjTypeEntry mapping");
            status = OBJ_ERR_OBJ_TYPE;
            break;
    }
    return status;
}

int
_objChksum (int objTypeInx, int lowLevelDescInx, int flags, char *chksum,
srb_long_t myOffset, srb_long_t mySize)
{
    int status;

    if (lowLevelDescInx < 0 || objTypeInx < 0) {
        elog(NOTICE,"_objChksum: lowLevelDescInx or objTypeInx out of range");
        return INP_ERR_DESC;
    }
    switch (srbObjTypeEntry[objTypeInx].objType) {
      case FILE_TYPE:
	status = _svrFileChksum (lowLevelDescInx, flags, chksum, 
	 myOffset, mySize);
	break;
      case DB_LOBJ_TYPE:
            status = 0;
	    sprintf (chksum, "%d", 0);
            break;
      case DB_TABLE_TYPE:
            status = 0;
	    sprintf (chksum, "%d", 0);
            break;
      default:                /* shouldn't get here */
            elog(NOTICE, "_objChksum: error in srbObjTypeEntry mapping");
            status = OBJ_ERR_OBJ_TYPE;
            break;
    }
    return status;
}


int 
svrCollClose (int descInx)
{
    srbObjDesc[descInx].lowLevelDescInx = -1;
    srbObjDesc[descInx].isDir = 0;
    if (srbObjDesc[descInx].collInfo != NULL) {
    	srbObjDesc[descInx].collInfo->maxDirLen = 0;
    	srbObjDesc[descInx].collInfo->offset = 0;
    	srbObjDesc[descInx].collInfo->accumOffset = 0;
    	srbObjDesc[descInx].collInfo->gotData = 0;
    	srbObjDesc[descInx].collInfo->continueIndex = -1;
    	if (srbObjDesc[descInx].collInfo->dirent != NULL) {
	    free (srbObjDesc[descInx].collInfo->dirent);
	    srbObjDesc[descInx].collInfo->dirent = NULL;
	}
    }
    if (srbObjDesc[descInx].collection != NULL) {
	free (srbObjDesc[descInx].collection);
	srbObjDesc[descInx].collection = NULL;
    }

    return 0;
}
    


int 
_objRead(int lowLevelDescInx, int objTypeInx, char *buf, int len) 
{
    int totalread = 0;

    if (lowLevelDescInx < 0 || objTypeInx < 0) {
        elog(NOTICE,"_objRead: lowLevelDescInx or objTypeInx out of range");
        return INP_ERR_DESC;
    }

    switch (srbObjTypeEntry[objTypeInx].objType) {
        case FILE_TYPE:
            totalread = _svrFileRead(lowLevelDescInx, buf, len);
            break;
        case DB_LOBJ_TYPE:
            totalread = _dbLobjRead (lowLevelDescInx, buf, len);
            break;
        case DB_TABLE_TYPE:
            totalread = _dbTableRead (lowLevelDescInx, buf, len);
            break;
        default:                /* shouldn't get here */
	    elog(NOTICE, "_objRead: error in srbObjTypeEntry mapping");
	    totalread = OBJ_ERR_OBJ_TYPE;
            break;
    }
    return totalread;
}

int
_objWrite(int lowLevelDescInx, int objTypeInx, char *buf, int len)
{
    int totalwritten = 0;

    if (lowLevelDescInx < 0 || objTypeInx < 0) {
        elog(NOTICE,"_objWrite: lowLevelDescInx or objTypeInx out of range");
        return INP_ERR_DESC;
    }

    switch (srbObjTypeEntry[objTypeInx].objType) {
        case FILE_TYPE:
            totalwritten = _svrFileWrite(lowLevelDescInx, buf, len);
            break;
        case DB_LOBJ_TYPE:
            totalwritten = _dbLobjWrite (lowLevelDescInx, buf, len);
            break;
        case DB_TABLE_TYPE:
            totalwritten = _dbTableWrite (lowLevelDescInx, buf, len);
            break;
        default:                /* shouldn't get here */
            elog(NOTICE, "_objWrite: error in srbObjTypeEntry mapping");
            totalwritten = OBJ_ERR_OBJ_TYPE;
            break;
    }
    return totalwritten;
}

srb_long_t
_objSeek (int lowLevelDescInx, int objTypeInx, srb_long_t offset, int whence)
{
    srb_long_t retVal;

    if (lowLevelDescInx < 0 || objTypeInx < 0) {
        elog(NOTICE,"_objWrite: lowLevelDescInx or objTypeInx out of range");
        return INP_ERR_DESC;
    }

    switch (srbObjTypeEntry[objTypeInx].objType) {
        case FILE_TYPE:
            retVal = _svrFileSeek(lowLevelDescInx, offset, whence);
            break;
        case DB_LOBJ_TYPE:
            retVal = dbLobjSeek (lowLevelDescInx, offset, whence);
            break;
        case DB_TABLE_TYPE:
            retVal = dbTableSeek (lowLevelDescInx, offset, whence);
            break;
        default:                /* shouldn't get here */
            elog(NOTICE, "_objSeek: error in srbObjTypeEntry mapping");
            retVal = OBJ_ERR_OBJ_TYPE;
            break;
    }
    return retVal;
}

/* svrObjUnlink - Entry point for the srbObjUnlink SID.
 *
 *
 * Input : struct varlena *vObjID - The object ID
 *         struct varlena *vDomainName - The domain name
 *
 * Output : Returns 0 - success, negative  - failure
 */
 
int
svrObjUnlink(struct varlena *vObjID,
struct varlena *vCollectionName)
{
    char *objID, *collectionName;
    int retVal;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);

    retVal = _svrObjUnlink (objID, collectionName);

    free (objID);
    free (collectionName);

    return (retVal);
}

int _svrObjUnlink (char *objID, char *collectionName)
{
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOut, *infoOutHead, *tmpInfoOut;
    int retVal;
    int status;
    char tmpStr[MAX_TOKEN], tmpStr1[MAX_TOKEN];
    int deleteFlag;


    if (strstr (objID, "&ADMIN=") != NULL) {
	if (ClientUser->privUserFlag != LOCAL_PRIV_USER) {
            elog (NOTICE,
              "_svrObjUnlink: not a local admin user but the admin is on");
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
        return retVal;
    }


    /* Call the Unlink at the next level */
 
    if (infoOutHead->containerName[0] != '\0') {  /* inContainer object */
	char fullContName[MAX_TOKEN];

	sprintf (fullContName, "%s/%s", collectionName, infoOutHead->objID);

	if (strcmp (fullContName, infoOutHead->containerName) != 0) {
	    /* Not a container, but a inContainer object */
	    /* Just unregister the object */
            status = _svrModifyDataset (MDAS_CATALOG, objID,
              collectionName, "", "",
              NULL, NULL, deleteFlag);
	    if (status < 0)
	    	elog (NOTICE, 
	     	 "_svrObjUnlink: _svrModifyDataset error. status = %d", status);
      	} else {	/* a container */
#ifdef foo
	    /* XXXXXXXX This is a container. Should fail */
	    elog (NOTICE,
	     "_svrObjUnlink: Trying to Unlink a container");
	    /* status = _rmContainerCopy (MDAS_CATALOG, infoOutHead->objID, 
	     collectionName, infoOutHead); */
	    status = OBJ_ERR_UNLINK_CONTAINER;
#endif
            if (objCntIncontainer (fullContName) > 0) {
                freeAllInfoOut (infoOutHead);
                return CONTAINER_NOT_EMPTY;
            }
            status = _svrRmContainer (MDAS_CATALOG, fullContName, deleteFlag);
      	}
	freeAllInfoOut (infoOutHead);   /* KESTOCK - mem leak fix */
	return status;
    }
    /* RAJA ADDED Dec 3, 2003 for checking and unlinking a linked object ***/
    if (isPureObject(objID, collectionName) == 0) {  /* linked object */
      /* Just unregister the object */
      status = _svrModifyDataset (MDAS_CATALOG, objID,
				  collectionName, "", "",
				  NULL, NULL, deleteFlag);
      if (status < 0)
	elog (NOTICE, 
	      "_svrObjUnlink: _svrModifyDataset error. status = %d", status);
      freeAllInfoOut (infoOutHead);   /* KESTOCK - mem leak fix */
      return status;
    }
    /* RAJA ADDED Dec 3, 2003 for checking and unlinking a linked object ***/
    infoOut = infoOutHead;
    while (infoOut != NULL) {

	status = rmContentOfInfoOut (infoOut, collectionName);


        if (status < 0) {
            elog (NOTICE, 
	     "_svrObjUnlink: rmContentOfInfoOut error. status = %d", status);
            if (retVal == 0)
                retVal = status;
            tmpInfoOut = infoOut->next;
            freeInfoOut (infoOut);
            infoOut = tmpInfoOut;       /* Try the next one */
            continue;   /* don't unregister now since the trash impl */
        }

        /* unregister the object */


    	mySplitStr (objID, tmpStr, tmpStr1, '&');
	/** RAJA June 20, 2003  made it to use objID instead of tmpStr
	    in the call below ****/
        status = _svrModifyDataset (MDAS_CATALOG, objID, 
          infoInp.collectionName, infoOut->resourceName, infoOut->dataPath,
          NULL, NULL, deleteFlag);
	
        if (status < 0) {
	    elog (NOTICE, "_svrObjUnlink: _svrModifyDataset error. status = %d",
	    status);
	    if (retVal == 0)
	        retVal = status;
        }
        tmpInfoOut = infoOut->next;
        freeInfoOut (infoOut);
        infoOut = tmpInfoOut;       /* Try the next one */
    }

    return retVal;
}

int _objUnlink (int objType, int systemType, char *resLoc,
char *fullPathName, int chkDirFlag)
{
    int retVal;

    switch (objType) {
      case FILE_TYPE:
        retVal = _svrFileUnlink (systemType,
            resLoc, fullPathName, chkDirFlag);
        break;

      case DB_LOBJ_TYPE:
        retVal = _dbLobjUnlink (systemType,
          resLoc, fullPathName, SRB_SERVER_USER);
        break;

      case DB_TABLE_TYPE:
        retVal = _dbTableUnlink (systemType,
          resLoc, fullPathName, SRB_SERVER_USER);
        break;

      default:                /* shouldn't get here */
        elog(NOTICE, "_objUnlink: error in srbObjTypeEntry mapping");
        retVal = OBJ_ERR_OBJ_TYPE;
        break;
    }
    return (retVal);
}

int
svrRmContainer (int catType, struct varlena *vContainerName, int deleteFlag) 
{
    char *containerName;
    int retVal;
   

    containerName = varToStr (vContainerName);

    retVal = _svrRmContainer (catType, containerName, deleteFlag);
    free (containerName);

    return retVal;
}

int
_svrRmContainer (int catType, char *containerName, int deleteFlag)
{
    char outContName[MAX_TOKEN], contCollection[MAX_TOKEN],
     fullContName[MAX_TOKEN];
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOut, *infoOutHead, *tmpInfoOut;
    int status = 0;
    int retVal;
    int lockFd;
    int remoteFlag;
    struct hostElement  *hostTabPtr;
    

    retVal = svrParseContainerName (containerName, outContName, contCollection,
     ClientUser->userName, ClientUser->domainName);
    if (retVal < 0) {
        elog (NOTICE,
         "_svrRmContainer: Error parsing container %s. status = %d",
          containerName, retVal);
        return (retVal);
    }

    /* check if the container is empty */

    sprintf (fullContName, "%s/%s", contCollection, outContName);

    if (objCntIncontainer (fullContName) > 0) {
        return CONTAINER_NOT_EMPTY;
    }

#ifdef LOCK_OBJ
    /* Do a write lock */
    lockFd =_svrObjLock (outContName, contCollection, F_SETLKW, F_WRLCK, 0);
    if (lockFd < 0) {
        return (lockFd);
    }
#endif

    infoInp.domainName = ClientUser->domainName;
    if (deleteFlag == D_SU_DELETE_TRASH_ONE) {
	if (ClientUser->privUserFlag != LOCAL_PRIV_USER) {
            elog (NOTICE,
              "_svrRmContainer: %s@%s has no priviledge to empty trash",
	    ClientUser->userName, ClientUser->domainName);
            return (AUTH_ERR_PROXY_NOPRIV);
        }
	infoInp.accessMode = "trash";
    } else {
        infoInp.accessMode = "all";
    }
    infoInp.objID = outContName;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = contCollection;

    /* Check with MDAS for the requested data */

    if ((retVal = mdasGetInfo (&infoInp, &infoOutHead)) < 0) {
#ifdef LOCK_OBJ
    	_svrObjLock (outContName, contCollection, F_SETLKW, F_UNLCK, lockFd);
#endif
        return retVal;
    }

    /* Now remove the container files */

    remoteFlag = getMdasEnabledHost (contCollection, &hostTabPtr);

    if (remoteFlag < 0) {
        return (remoteFlag);
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
        infoOut = infoOutHead;
        while (infoOut != NULL) {

	    status = rmContentOfInfoOut (infoOut, contCollection);

            if (status < 0) {
                retVal = status;
                elog (NOTICE, 
		 "svrRmContainer: rmContentOfInfoOut error. status = %d",
                 status);
		retVal = status;
            }

            tmpInfoOut = infoOut->next;
            freeInfoOut (infoOut);
            infoOut = tmpInfoOut;       /* Try the next one */
        }
    } else {
	freeAllInfoOut (infoOutHead);	/* KESTOCK - mem leak fix */
    }
    status = __svrRmContainer (catType, fullContName, deleteFlag);
#ifdef LOCK_OBJ
        _svrObjLock (outContName, contCollection, F_SETLKW, F_UNLCK, lockFd);
#endif
    if (status < 0) {
        elog (NOTICE, "_svrRmContainer: _svrRmContainer error. status = %d",
        retVal);
        retVal = status;
    }

    return (retVal);
}

int
rmContentOfInfoOut (struct mdasInfoOut *infoOut, char *collectionName)
{
    int tinx, retVal;
    struct mdasInfoOut *cacheInfoOut, *tapeInfoOut, *intInfoOut;

    tinx = infoOut->objTypeInx;
    if (srbObjTypeEntry[tinx].objType == COMPOUND_TYPE) {
        /* a compound object */
        retVal = _svrGetInternalObjInfo (infoOut->objID, collectionName,
          infoOut->replNum, NO_INT_SEGMENTATION,
          &cacheInfoOut, &tapeInfoOut);
        if (retVal < 0) {
            elog (NOTICE,
              "rmContentOfInfoOut: _svrGetIntObjInfo error for %s/%s, stat=%d",
              collectionName, infoOut->objID, retVal);
            return (retVal); 
        }
        intInfoOut = cacheInfoOut;
        while (intInfoOut != NULL) {
            retVal = rmIntNonTapeCompObj (intInfoOut, collectionName,
	     ClientUser->userName, ClientUser->domainName);
            if (retVal < 0) {
                freeInfoOut (cacheInfoOut);
                freeInfoOut (tapeInfoOut);
                return (retVal);
            }
            intInfoOut = intInfoOut->next;
        }
        intInfoOut = tapeInfoOut;
        while (intInfoOut != NULL) {
            retVal = rmAllIntTapeCompObj (intInfoOut, collectionName,
	     ClientUser->userName, ClientUser->domainName);
            if (retVal < 0) {
                freeInfoOut (cacheInfoOut);
                freeInfoOut (tapeInfoOut);
                return (retVal);
            }
            intInfoOut = intInfoOut->next;
        }
        freeInfoOut (cacheInfoOut);
        freeInfoOut (tapeInfoOut);
    } else {
        retVal = _objUnlink (srbObjTypeEntry[tinx].objType,
          srbObjTypeEntry[tinx].systemType,
            infoOut->resourceLoc, infoOut->dataPath, NO_CHKDIR);
    }

    return (retVal);
}


int
__svrRmContainer (int catType, char *fullContName, int deleteFlag)
{
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Do the modify_dataset_info */

    remoteFlag = getMdasEnabledHost (fullContName, &hostTabPtr);

    if (remoteFlag < 0) {
        return (remoteFlag);
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = removeContainer (catType, fullContName,
	 ClientUser->userName, ClientUser->domainName);
        if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }

        /* Do a client call to the remote host */

        retVal = srbRmContainer (hostTabPtr->conn, catType, fullContName,
	 deleteFlag);

	if (retVal < 0) {
	    elog (NOTICE, "__svrRmContainer: srbRmContainer error: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }

    return retVal;
}

int
svrSyncContainer (int catType, struct varlena *vContainerName, int syncFlag)
{
    char *containerName;
    int retVal;
    char dataName[MAX_TOKEN], collection[MAX_TOKEN];
    int status, lockFd;

    containerName = varToStr (vContainerName);

#ifdef LOCK_OBJ
    status = svrParseContainerName (containerName, dataName, collection,
     ClientUser->userName, ClientUser->domainName);
    if (status < 0) {
        elog (NOTICE,
         "_svrSyncContainer: Error parsing container %s. status = %d",
          containerName, retVal);
        free (containerName);
        return (status);
    }

    /* Do a write lock */
    lockFd =_svrObjLock (dataName, collection, F_SETLKW, F_WRLCK, 0);
    if (lockFd < 0) {
        free (containerName);
        return (lockFd);
    }
#endif

    retVal = _svrSyncContainer (catType, containerName, syncFlag);

#ifdef LOCK_OBJ
    _svrObjLock (dataName, collection, F_SETLKW, F_UNLCK, lockFd);
#endif

    free (containerName);
    return (retVal);
}

int
_svrSyncContainer (int catType, char *containerName, int syncFlag) 
{
    char outContName[MAX_TOKEN], contCollection[MAX_TOKEN];
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead, *tmpInfoOut, *permInfoOut, *nextInfoOut;
    struct mdasInfoOut *cacheInfoOut, *archInfoOut, 
     *cacheInfoOut1, *archInfoOut1,
      *dirtyCacheInfoOut, *dirtyArchInfoOut;
    struct mdasInfoOut *empCacheInfoOut, *empArchInfoOut,
     *empCacheInfoOut1, *empArchInfoOut1,
      *empCacheInfoOut2, *empArchInfoOut2, *srcInfoOut;
    struct mdasInfoOut *emptyContHead;

    int status = 0;
    int retVal;
    int tmpInx, permInx;
#ifdef _WIN32
    char *buf;
#else
    char buf[BUF_SIZE];
#endif
    int nbytes, byteWritten;
    char strSize[MAX_TOKEN];
    int allSynced = 1;		/* assume all copied are synced */
    int goodArchCopy = 0;	/* do we have a good arch copy ? */
    int emptyCnt;
    char fullContPath[MAX_TOKEN];
    struct hostElement  *hostTabPtr;
    int remoteFlag;
    

    retVal = svrParseContainerName (containerName, outContName, contCollection,
     ClientUser->userName, ClientUser->domainName);
    if (retVal < 0) {
        elog (NOTICE,
         "svrSyncContainer: Error parsing container %s. status = %d",
          containerName, retVal);
        return (retVal);
    }

    remoteFlag = getMdasEnabledHost (contCollection, &hostTabPtr);

    if (remoteFlag < 0) {
        retVal = remoteFlag;
    } else if (remoteFlag == REMOTE_HOST) {      /* Can talk to mdas */
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        } else {
            retVal = srbSyncContainer (hostTabPtr->conn, catType, containerName,
             syncFlag);
        }
	return (retVal);
    }

#ifdef _WIN32
    buf = (char *)calloc(BUF_SIZE,sizeof(char));
#endif

    infoInp.domainName = ClientUser->domainName;
    infoInp.accessMode = "all";
    infoInp.objID = outContName;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = contCollection;

    /* Check with MDAS for the requested data */

    if ((retVal = mdasGetInfo (&infoInp, &infoOutHead)) < 0) {
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
        return retVal;
    }

    if (!myIsContainer (outContName, contCollection, 
     infoOutHead->containerName)) {
	freeAllInfoOut (infoOutHead);
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
	return (OBJ_ERR_OBJ_NOT_CONTAINER);
    }

    sprintf (fullContPath, "%s/%s", contCollection, outContName); 

    emptyCnt = getEmptyContainer (catType, fullContPath, infoOutHead,
    &emptyContHead);

    if (emptyCnt > 0) {
	sortInfoByClass (emptyContHead, &empCacheInfoOut, &empArchInfoOut,
         &empCacheInfoOut1, &empArchInfoOut1,
          &empCacheInfoOut2, &empArchInfoOut2);
	freeAllInfoOut (empCacheInfoOut);
	freeAllInfoOut (empCacheInfoOut1);
    } else {
	freeAllInfoOut (emptyContHead);	/* KESTOCK - mem leak fix */
	empArchInfoOut = empArchInfoOut1 = NULL;
    }

    /* sort the infoOutHead */

    sortInfoByClass (infoOutHead, &cacheInfoOut, &archInfoOut,
     &cacheInfoOut1, &archInfoOut1,
     &dirtyCacheInfoOut, &dirtyArchInfoOut);

    if (dirtyArchInfoOut != NULL ||	/* have a dirty arch copy, or */
     (dirtyCacheInfoOut == NULL &&  	/* no dirty copy, have a arch copy */
      (archInfoOut != NULL || archInfoOut1 != NULL)))
	goodArchCopy = 1;

    if (syncFlag & PRIMARY_FLAG) {      /* sync to primary arch only */
        if (archInfoOut == NULL && empArchInfoOut == NULL) {      
	    /* no pri arch to sync to */
	    if ((archInfoOut1 != NULL || empArchInfoOut1 != NULL) && 
	     dirtyArchInfoOut == NULL) {
		/* No primary, but have other arch, take the top one from
		 * archInfoOut1 or empArchInfoOut1. */
		if (archInfoOut1 != NULL) { /* pick one from archInfoOut1 */
		    archInfoOut = archInfoOut1;
		    archInfoOut1 = archInfoOut->next;
		    archInfoOut->next = NULL;
		    if (archInfoOut1 != NULL) 
            	        allSynced = 0;
		    freeAllInfoOut (archInfoOut1);
		    archInfoOut1 = NULL;
		    freeAllInfoOut (empArchInfoOut1);
		    empArchInfoOut1 = NULL;
		} else {	/* pick one from empArchInfoOut1 */
		    empArchInfoOut = empArchInfoOut1;
		    empArchInfoOut1 = empArchInfoOut->next;
		    empArchInfoOut->next = NULL;
		    freeAllInfoOut (empArchInfoOut1);
                    empArchInfoOut1 = NULL;
		}
	    } else if (syncFlag & PURGE_FLAG) {	/* Nothing to sync. 
						 * Should we purge ? */
		if ((dirtyArchInfoOut != NULL) ||    /* have a good copy */
		 (archInfoOut1 != NULL && dirtyCacheInfoOut == NULL)) {
		    /* have a good copy or both cache and arch copies
		     * are not dirty. Do the purge */
		    if (archInfoOut1 != NULL) {
            	        freeAllInfoOut (archInfoOut1);
		    	archInfoOut1 = NULL;
            	        freeAllInfoOut (empArchInfoOut1);
		    	empArchInfoOut1 = NULL;
            	    	allSynced = 0;
		    }
            	    freeAllInfoOut (dirtyArchInfoOut);
		    dirtyArchInfoOut = NULL;
		    combineInfoOut (&cacheInfoOut, &cacheInfoOut1);
		    /* Now purge the temporary copy */

		    combineInfoOut (&dirtyCacheInfoOut, &cacheInfoOut);
		    /* don't purge if no good arch copy */
		    if (goodArchCopy > 0) {
		        tmpInfoOut = dirtyCacheInfoOut;
		        while (tmpInfoOut != NULL) {
		            retVal = _rmContainerCopy (catType, outContName, 
			     contCollection, tmpInfoOut);
		            tmpInfoOut = tmpInfoOut->next;
        		}
		    }
		    freeAllInfoOut (dirtyCacheInfoOut);
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
		    return (status);
		} else { 		/* Don't have other copy, no purge */
            	    freeAllInfoOut (archInfoOut1);
            	    freeAllInfoOut (cacheInfoOut);
            	    freeAllInfoOut (cacheInfoOut1);
            	    freeAllInfoOut (dirtyCacheInfoOut);
            	    freeAllInfoOut (dirtyArchInfoOut);
            	    freeAllInfoOut (empArchInfoOut1);
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
            	    return 0;
		}
	    } else {	
		/* no arch to sync to or have a dirty arch already
		 * and not purging */ 
            	freeAllInfoOut (archInfoOut1);
            	freeAllInfoOut (cacheInfoOut);
            	freeAllInfoOut (cacheInfoOut1);
            	freeAllInfoOut (dirtyCacheInfoOut);
            	freeAllInfoOut (dirtyArchInfoOut);
            	freeAllInfoOut (empArchInfoOut1);
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
            	return 0;
	    }
        } else {
	    if (archInfoOut1 != NULL) 	/* have non-pri arch copy */
            	allSynced = 0;
            freeAllInfoOut (archInfoOut1);	/* don't sync to other arch */
	    archInfoOut1 = NULL;
	    freeAllInfoOut (empArchInfoOut1);
            empArchInfoOut1 = NULL;
        }
    } else {	/* sync to all */
        combineInfoOut (&archInfoOut, &archInfoOut1);
        combineInfoOut (&empArchInfoOut, &empArchInfoOut1);
    }
    combineInfoOut (&cacheInfoOut, &cacheInfoOut1);

    retVal = 0;
    if (dirtyCacheInfoOut != NULL || dirtyArchInfoOut != NULL) {
	/* we have a dirty copy */
        /* If there are other out-of-date cache copies, delete them */

        tmpInfoOut = cacheInfoOut;
        while (tmpInfoOut != NULL) {
            nextInfoOut = tmpInfoOut->next;
            _rmContainerCopy (catType, containerName, contCollection,
             tmpInfoOut);
            tmpInfoOut = nextInfoOut;
        }
	freeAllInfoOut (cacheInfoOut);
	cacheInfoOut = NULL;

    	/* File copy - use the dirtyCacheInfoOut as source first */

	status = syncInfoToInfo (dirtyCacheInfoOut, contCollection,
	  archInfoOut, contCollection, 0);

	/* status = 0; */
    	if (dirtyCacheInfoOut == NULL || status < 0) {
	    /* Try dirtyArchInfoOut */
	    status = syncInfoToInfo (dirtyArchInfoOut, contCollection,
              archInfoOut, contCollection, 0);
	}

	if (status == 0)
	    allSynced = 1;
	else
	    allSynced = 0;

        /* register new size for good permanent copy */
        permInfoOut = archInfoOut;
        while (permInfoOut != NULL && permInfoOut->size >= 0) {
            permInfoOut->isDirty = 1;
	    goodArchCopy = 1;
	    permInfoOut = permInfoOut->next;
	}

	/* Replicate to the empty Arch */

	if (dirtyCacheInfoOut != NULL)
	    srcInfoOut = dirtyCacheInfoOut;
	else 
	    srcInfoOut = dirtyArchInfoOut;

        permInfoOut = empArchInfoOut;
        while (permInfoOut != NULL) {
	    struct mdasInfoOut *newInfoOut;

	    newInfoOut = NULL;
	    retVal = _objReplicate (catType, outContName, contCollection,
             permInfoOut->resourceName, "", srcInfoOut, &newInfoOut, NULL);

	    if (retVal < 0) {
		 elog (NOTICE,
                 "_svrSyncContainer: Unable to repl %s/%s to %s, status = %d",
                  contCollection, outContName, permInfoOut->resourceName,
		   retVal);
		status = retVal;
		allSynced = 0;
		permInfoOut->isDirty = -1;	/* failed */
	        permInfoOut->dataPath = NULL;
	    } else {
	    	permInfoOut->isDirty = 1;
		goodArchCopy = 1;
	        /* save dataPath; needed for modifyDatasetInfo */
	        permInfoOut->dataPath = newInfoOut->dataPath;
		newInfoOut->dataPath = NULL;	/* don't free dataPath */
	    }
	    freeAllInfoOut (newInfoOut);
		
	    permInfoOut = permInfoOut->next;
	}
    } else {		/* nothing is dirty */
	int replicatedFlag = 0;

	if (empArchInfoOut != NULL) {
	    struct mdasInfoOut *newInfoOut;

	    if (cacheInfoOut != NULL) 
		srcInfoOut = cacheInfoOut;
	    else 
		srcInfoOut = archInfoOut;

 	    if (srcInfoOut != NULL) {
		replicatedFlag = 1;
        	permInfoOut = empArchInfoOut;
        	while (permInfoOut != NULL) {
            	    struct mdasInfoOut *newInfoOut;

            	    newInfoOut = NULL;
            	    retVal = _objReplicate (catType, outContName, 
		     contCollection, permInfoOut->resourceName, "", 
		      srcInfoOut, &newInfoOut, NULL);
            	    freeAllInfoOut (newInfoOut);

            	    if (retVal < 0) {
                       elog (NOTICE,
                       "_svrSyncContainer: Unable to repl %s to %s,status = %d",
                  	outContName, permInfoOut->resourceName, retVal);
		        status = retVal;
		        allSynced = 0;
			permInfoOut->isDirty = -1;	/* failed */
            	    } else {
                    	permInfoOut->isDirty = 0;  /* source is not dirty */
			goodArchCopy = 1;
                    }

            	    permInfoOut = permInfoOut->next;
		}
            }
	}

	/* Did nothing */

	if (replicatedFlag == 0 && syncFlag & PURGE_FLAG) {
            /* Now purge the temporary copy */

            combineInfoOut (&dirtyCacheInfoOut, &cacheInfoOut);
            /* don't purge if no good arch copy */
            if (goodArchCopy > 0) {
                tmpInfoOut = dirtyCacheInfoOut;
                while (tmpInfoOut != NULL) {
                    retVal = _rmContainerCopy (catType, outContName,
                     contCollection, tmpInfoOut);
                    tmpInfoOut = tmpInfoOut->next;
                }
            }
            freeAllInfoOut (dirtyCacheInfoOut);
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
            return (status);
	} else {
            freeAllInfoOut (dirtyCacheInfoOut);
            freeAllInfoOut (dirtyArchInfoOut);
            freeAllInfoOut (archInfoOut);
            freeAllInfoOut (empArchInfoOut);
            freeAllInfoOut (cacheInfoOut);
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
	    return 0;
	}
    }

    if (retVal >= 0 && status >= 0) {	/* We have no error */

    	/* Mark all dirty cache copies not dirty */
	/* cache will be purged anyway. */
	if ((syncFlag & PURGE_FLAG) == 0 && allSynced > 0) {
	  tmpInfoOut = dirtyCacheInfoOut;
          while (tmpInfoOut != NULL) {
    	    retVal = _svrModifyDataset (MDAS_CATALOG, outContName,
     	     contCollection, tmpInfoOut->resourceName, tmpInfoOut->dataPath, 
      	      "0", NULL, D_CHANGE_DIRTY);

     	    if (retVal < 0) { 
	    	elog (NOTICE, 
             	 "_svrSyncContainer: Reg container is_dirty flag error, status=%d", retVal);
	    	status = retVal;
	    }
	    tmpInfoOut = tmpInfoOut->next;
	  }
    	}

	/* For the arch copies, if allSynced > 0, mark all dirty copies to
	 * 'clean'. Otherwise, mark all synced copied to dirty */
 
	if (allSynced > 0) {
            tmpInfoOut = dirtyArchInfoOut;
            while (tmpInfoOut != NULL) {
            	retVal = _svrModifyDataset (MDAS_CATALOG, outContName,
             	 contCollection, tmpInfoOut->resourceName, tmpInfoOut->dataPath,
              	  "0", NULL, D_CHANGE_DIRTY);

            	if (retVal < 0) {
                    elog (NOTICE,
                     "_svrSyncContainer: Reg container is_dirty flag error, status=%d", retVal);
		    status = retVal;
		}
            	tmpInfoOut = tmpInfoOut->next;
	    }
	    /* Process those that were empty */

	    tmpInfoOut = empArchInfoOut;
	    while (tmpInfoOut != NULL) {
	      if (tmpInfoOut->isDirty > 0) {
                retVal = _svrModifyDataset (MDAS_CATALOG, outContName,
                 contCollection, tmpInfoOut->resourceName, tmpInfoOut->dataPath,
                  "0", NULL, D_CHANGE_DIRTY);

                if (retVal < 0) {
                    elog (NOTICE,
                     "_svrSyncContainer: Reg container is_dirty flag error, status=%d", retVal);
                    status = retVal;
                }
	      }
              tmpInfoOut = tmpInfoOut->next;
            }

        } else {	/* not everything are synced */
            tmpInfoOut = archInfoOut;
            while (tmpInfoOut != NULL) {
		if (tmpInfoOut->isDirty >= 0) {
                    retVal = _svrModifyDataset (MDAS_CATALOG, outContName,
                     contCollection, tmpInfoOut->resourceName, 
		      tmpInfoOut->dataPath, "1", NULL, D_CHANGE_DIRTY);

                    if (retVal < 0) {
                    	elog (NOTICE,
                     	 "_svrSyncContainer: Reg container is_dirty flag error, status=%d", retVal);
		         status = retVal;
                    }
		}
                tmpInfoOut = tmpInfoOut->next;
            }   
	    /* Process those that were empty */

            tmpInfoOut = empArchInfoOut;
            while (tmpInfoOut != NULL) {
              if (tmpInfoOut->isDirty == 0) {
                retVal = _svrModifyDataset (MDAS_CATALOG, outContName,
                 contCollection, tmpInfoOut->resourceName, tmpInfoOut->dataPath,
                  "1", NULL, D_CHANGE_DIRTY);

                if (retVal < 0) {
                    elog (NOTICE,
                     "_svrSyncContainer: Reg container is_dirty flag error, status=%d", retVal);
                    status = retVal;
                }
              }
              tmpInfoOut = tmpInfoOut->next;
            }

	}
    } else {	/* have error */
	/* mark the replica as dirty */
        tmpInfoOut = archInfoOut;
        while (tmpInfoOut != NULL) {
	  if (tmpInfoOut->isDirty > 0) {
            retVal = _svrModifyDataset (MDAS_CATALOG, outContName,
             contCollection, tmpInfoOut->resourceName, tmpInfoOut->dataPath,
              "1", NULL, D_CHANGE_DIRTY);

            if (retVal < 0) {
                elog (NOTICE,
                 "_svrSyncContainer: Reg container is_dirty flag error, status=%d", retVal);
		status = retVal;
	    }
	  }
          tmpInfoOut = tmpInfoOut->next;
        }
    }
    freeAllInfoOut (dirtyArchInfoOut);
    freeAllInfoOut (archInfoOut);
    freeAllInfoOut (empArchInfoOut);

     if ((syncFlag & PURGE_FLAG) == 0) {
	freeAllInfoOut (dirtyCacheInfoOut);
	freeAllInfoOut (cacheInfoOut);
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
	return status;
    }

    /* Now purge the temporary copy */

    combineInfoOut (&dirtyCacheInfoOut, &cacheInfoOut);
    if (goodArchCopy > 0) {	/* don't purge if no good arch copy */
    	tmpInfoOut = dirtyCacheInfoOut;
    	while (tmpInfoOut != NULL) {
    	    retVal = _rmContainerCopy (catType, outContName, contCollection,
     	     tmpInfoOut);
	    tmpInfoOut = tmpInfoOut->next;
	}
    }
    freeAllInfoOut (dirtyCacheInfoOut);
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
    return (status);
}

int
_rmContainerCopy (int catType, char *containerName, char *contCollection,
struct mdasInfoOut *tmpInfoOut)
{
    char fullContName[MAX_TOKEN];
    int retVal;
    int tinx;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Now purge the temporary copy */

    sprintf (fullContName, "%s/%s&COPY=%-d", contCollection, containerName,
     tmpInfoOut->replNum);

    retVal = __svrRmContainer (catType, fullContName, D_DELETE_ONE);

    if (retVal < 0) {
        elog (NOTICE, "_rmContainerCopy: __svrRmContainer error. status = %d",
        retVal);
        return retVal;
    }

   /* Now remove the container files */

    remoteFlag = getMdasEnabledHost (fullContName, &hostTabPtr);

    if (remoteFlag < 0) {
        return (remoteFlag);
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
        tinx = tmpInfoOut->objTypeInx;
        retVal = _objUnlink (srbObjTypeEntry[tinx].objType,
        srbObjTypeEntry[tinx].systemType, tmpInfoOut->resourceLoc,
         tmpInfoOut->dataPath, NO_CHKDIR);
        if (retVal < 0) {
            elog (NOTICE, "svrRmContainer: _objUnlink error. status = %d",
             retVal);
        }
    }

    return (retVal);
}

int 
lowLevelOpen (int objTypeInx, char *dataPath, char *resourceLoc, int oflag)
{
    int exfInx;

    if (objTypeInx < 0) {
        elog(NOTICE,
         "lowLevelOpen: resourceLoc:%s not supported", resourceLoc);
        return (OBJ_ERR_RES_TYPE);
    }


   /* Call open at the next level */

    switch (srbObjTypeEntry[objTypeInx].objType) {
      case FILE_TYPE:
        exfInx = _svrFileOpen (srbObjTypeEntry[objTypeInx].systemType,
          resourceLoc, dataPath, oflag, 0640,
            NO_CHKDIR, GENERIC_CONN);
#ifdef LOCK_DEBUG
	elog(NOTICE, "lowLevelOpen:%d %s", exfInx, dataPath);
#endif
        break;
      case DB_LOBJ_TYPE:
        exfInx = _dbLobjOpen (srbObjTypeEntry[objTypeInx].systemType,
            resourceLoc, dataPath, oflag, 0640,
            SRB_SERVER_USER);
        break;
      case DB_TABLE_TYPE:
        exfInx = _dbTableOpen (srbObjTypeEntry[objTypeInx].systemType,
            resourceLoc, dataPath, oflag, 0640,
            SRB_SERVER_USER);
        break;

      default:              /* shouldn't get here */
        elog(NOTICE, "lowLevelOpen: error in srbObjTypeEntry mapping");
        exfInx = OBJ_ERR_OBJ_TYPE;
        break;
    }

    return (exfInx);
}

struct mdasInfoOut *
getInfoOutByClass (char *classStr, struct mdasInfoOut *infoOutHead)
{
    struct mdasInfoOut *tmpInfoOut;

    tmpInfoOut = infoOutHead;
    while (tmpInfoOut != NULL) {
        if (strcmp (classStr, tmpInfoOut->phyResClass) == 0) {
	    return (tmpInfoOut);
        }
        tmpInfoOut = tmpInfoOut->next;
    }
    return (NULL);
}

/* svrObjRead - Entry point for the srbObjRead SID.
 *
 *
 * Input : int descInx - The object descriptor index to read.
 *	   int len - The length to read.
 *
 * Output : success - Returns the content and bytes read in struct varlena
 *	    failure - return NULL.
 */

struct varlena *
svrObjRead (int descInx, int len)
{
    struct varlena *retval;
    int totalread;
    int status;


    /* add one more to the length because DB2 append a NULL to the end */

    retval = (struct varlena *)malloc(VAROUTHDRSZ + len + 1);

    totalread = _svrObjRead (descInx, VAROUTDATA(retval), len);

    if (totalread >= 0) {
        status = 0;
    } else {
        status = totalread;
        totalread = 0;
    }

    VARSIZE(retval) = totalread + VAROUTHDRSZ;
    VAROUTSTAT(retval) = htonl (status);

    return retval;
}


int _svrObjRead (int descInx, char *buf, int len)
{
    int totalread;
    int tinx;

    if (descInx < 0 || descInx >= MAX_SRBO_DESC ||
    srbObjDesc[descInx].lowLevelDescInx < 0) {
        elog(NOTICE,"svrObjRead: Invalid desc (%d)", descInx);
	return INP_ERR_DESC;
    }

    tinx = srbObjDesc[descInx].objTypeInx;
    switch (srbObjTypeEntry[tinx].objType) {
        case FILE_TYPE:
	    if (srbObjDesc[descInx].contInx >= 0) {
		srb_long_t eofOffset;
		int contInx = srbObjDesc[descInx].contInx;

		eofOffset = 
		 srbObjDesc[descInx].baseOffset + srbObjDesc[descInx].objSize;
		/* Start reading pass EOF ? */
		if (srbObjDesc[contInx].contInfo->curOffset >= eofOffset) {
		    totalread = EOF;
		    break;
		}

		/* Read pass EOF ? */
		if (srbObjDesc[contInx].contInfo->curOffset + len > eofOffset) {
		    /* Adjust len */
		    len = eofOffset - srbObjDesc[contInx].contInfo->curOffset;
		}
            }
		
    	    totalread = _svrFileRead(srbObjDesc[descInx].lowLevelDescInx, 
		buf, len);

            if (srbObjDesc[descInx].contInx >= 0 && totalread > 0) {
		int contInx = srbObjDesc[descInx].contInx;

                /* InContainer obj */
            	srbObjDesc[contInx].contInfo->curOffset += totalread;
            }

	    break;
        case DB_LOBJ_TYPE:
	    totalread = _dbLobjRead (srbObjDesc[descInx].lowLevelDescInx,
                buf, len);
            break;
        case DB_TABLE_TYPE:
	    totalread = _dbTableRead (srbObjDesc[descInx].lowLevelDescInx,
                buf, len);
            break;

        default:                /* shouldn't get here */
            elog(NOTICE, "svrObjRead: error in srbObjTypeEntry mapping");
            totalread = OBJ_ERR_OBJ_TYPE;
            break;
    }

    return totalread;
}

/* svrObjWrite - Entry point for the srbObjWrite SID.
 *
 *
 * Input : int descInx - The object descriptor index to write.
 *	   struct varlena *wbuf - The buffer to write.
 *
 * Output : success - Returns the content and bytes write in struct varlena
 *	    failure - A negative value.
 */

int
svrObjWrite (int descInx, struct varlena *wbuf)
{
    int totalwritten;
    int bytestowrite;

    bytestowrite = VARSIZE(wbuf) - SIZEOF32;

    totalwritten = _svrObjWrite (descInx, VARDATA(wbuf), bytestowrite);

    return (totalwritten);
}

int 
_svrObjWrite (int descInx, char *buf, int bytestowrite)
{
    int totalwritten;
    int chgBaseOffset = 0;
    int tinx;
    int status;
    int contInx;

    if (descInx < 0 || descInx >= MAX_SRBO_DESC ||
    srbObjDesc[descInx].lowLevelDescInx < 0) {
        elog(NOTICE,"svrObjWrite: Invalid desc (%d)", descInx);
        return INP_ERR_DESC;
    }


    while (descInx != -1) {
        tinx = srbObjDesc[descInx].objTypeInx;
        switch (srbObjTypeEntry[tinx].objType) {
            case FILE_TYPE:
	        contInx = srbObjDesc[descInx].contInx;
                /* Writing to an existing inContainer obj ?? */ 
                if (contInx >= 0 && 
                 (srbObjDesc[descInx].baseOffset >=  /* first write ? */
                  srbObjDesc[contInx].contInfo->curOffset || 
                   srbObjDesc[descInx].objSize > 0) && /* An old obj */
                    srbObjDesc[descInx].baseOffset <
                     srbObjDesc[contInx].contInfo->curContainerSize) {
			/* Have to start at the end of the container */
			srbObjDesc[descInx].baseOffset = 
			 srbObjDesc[contInx].contInfo->curContainerSize;
			/* Seek to the baseOffset */
    			status = _svrObjSeek (descInx, 0, SEEK_SET);

    			if (status < 0) {
        		    elog(NOTICE, 
			     "svrObjWrite: _svrObjSeek error, status = %d ", 
			      status);
        		    return status;
    			}
			chgBaseOffset = 1;
			srbObjDesc[descInx].objSize = 0; /* starting at end */
		} else {
			chgBaseOffset = 0;
		}

    	        totalwritten = 
		  _svrFileWrite(srbObjDesc[descInx].lowLevelDescInx, 
			buf, bytestowrite);

		if (srbObjDesc[descInx].contInx >= 0 && 
		 totalwritten > 0) {
        	    /* inContainer object */
		    int contInx = srbObjDesc[descInx].contInx;

        	    srbObjDesc[contInx].contInfo->curOffset += totalwritten;
        	    if (srbObjDesc[contInx].contInfo->curOffset >
        	     srbObjDesc[contInx].contInfo->curContainerSize) {
            	    	srbObjDesc[contInx].contInfo->curContainerSize =
             	         srbObjDesc[contInx].contInfo->curOffset;
        	    }
		    /* Register change in offset */
		    if (chgBaseOffset > 0) {
			char strSize[MAX_TOKEN];

			chgBaseOffset = 0;
                        sprintf (strSize,
#if defined(PORTNAME_osx)
                         "%-qd", srbObjDesc[descInx].baseOffset);
#else
			 "%-lld", srbObjDesc[descInx].baseOffset);
#endif
                        status = _svrModifyDataset (MDAS_CATALOG,
                         srbObjDesc[descInx].objID,
                          srbObjDesc[descInx].collection,
                           "", "", strSize, NULL, D_CHANGE_OFFSET);
                        if (status < 0) {
                         elog (NOTICE,
                         "svrObjWrite: Unable to register new offset, status = %d", status);
                            return (status);
                        }
		    }
    		}

	        break;
            case DB_LOBJ_TYPE:
	        totalwritten = 
		    _dbLobjWrite (srbObjDesc[descInx].lowLevelDescInx,
                    buf, bytestowrite);
                break;
            case DB_TABLE_TYPE:
	        totalwritten = 
		    _dbTableWrite (srbObjDesc[descInx].lowLevelDescInx,
                    buf, bytestowrite);
                break;
            default:                /* shouldn't get here */
                elog(NOTICE, "svrObjWrite: error in srbObjTypeEntry mapping");
                totalwritten = OBJ_ERR_OBJ_TYPE;
                break;
        }
	if (totalwritten < 0)	/* error or eof */
	    break;
	descInx = srbObjDesc[descInx].nextDescInx;
    }
    return totalwritten;
}

/* svrObjSeek - Entry point for the srbObjSeek SID.
 *
 *
 * Input : int descInx - The object descriptor index to seek.
 *         srb_long_t *offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output - Returns 0 upon success.
 *          Returns negative upon failure.
 */

srb_long_t *
svrObjSeek (int descInx, srb_long_t *offset, int whence)
{
    srb_long_t *retVal, myOffset;

    myOffset = *offset;
    retVal = (srb_long_t *) malloc (2*SIZEOF32);

    *retVal = _svrObjSeek (descInx, myOffset, whence);

    return (retVal);
}

/* _svrObjSeek - Internal version of svrObjSeek.
 *
 *
 * Input : int descInx - The object descriptor index to seek.
 *         srb_long_t offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output - Returns 0 upon success.
 *          Returns negative upon failure.
 */

srb_long_t
_svrObjSeek (int descInx, srb_long_t offset, int whence)
{
    int tinx;
    srb_long_t retVal;
    srb_long_t newOffset;

    if (descInx < 0 || descInx >= MAX_SRBO_DESC ||
    srbObjDesc[descInx].lowLevelDescInx < 0) {
        elog(NOTICE,"svrObjSeek: Invalid desc (%d)", descInx);
	retVal = INP_ERR_DESC;
        return retVal;
    }

    while (descInx != -1) {
    	tinx = srbObjDesc[descInx].objTypeInx;

    	/* Special handling for inContainer object seek */
	
	if (srbObjDesc[descInx].contInx >= 0) {
	    int contInx = srbObjDesc[descInx].contInx;

	    if (whence == SEEK_SET) {
	    	offset += srbObjDesc[descInx].baseOffset;
		newOffset = offset;
	    } else if (whence == SEEK_CUR) {
		newOffset = offset + srbObjDesc[contInx].contInfo->curOffset;
	    } else if (whence == SEEK_END) {
		whence = SEEK_SET;
		newOffset = offset + 
		  srbObjDesc[contInx].contInfo->curContainerSize; 
		offset = newOffset;
	    }

	    if (newOffset < srbObjDesc[descInx].baseOffset) {
		elog(NOTICE,
#if defined(PORTNAME_osx)
                 "svrObjSeek: The new position in container %qd is beyond the Off set %qd of the object",
#else
		 "svrObjSeek: The new position in container %lld is below the baseOffset %lld of the object", 
#endif
		  newOffset, srbObjDesc[descInx].baseOffset);
		return (OBJ_ERR_OUT_OF_INCONTAINER_BOUND);
	    }

	    /* objSize > 0 means manipulating an existing object */
            if (srbObjDesc[descInx].objSize > 0 && newOffset >= 
             (srbObjDesc[descInx].baseOffset + srbObjDesc[descInx].objSize)) {
		elog(NOTICE,
#if defined(PORTNAME_osx)
                 "svrObjSeek: The new position in container %qd is beyond the Off set %qd of the object",
#else
                 "svrObjSeek: The new position in container %lld is beyond the Off set %lld of the object",
#endif
                  newOffset, srbObjDesc[descInx].baseOffset + 
		   srbObjDesc[descInx].objSize);
                return (OBJ_ERR_OUT_OF_INCONTAINER_BOUND);
            }
	}
	
        switch (srbObjTypeEntry[tinx].objType) {
            case FILE_TYPE:
                retVal = _svrFileSeek (srbObjDesc[descInx].lowLevelDescInx, 
		  offset, whence);
		break;
            case DB_LOBJ_TYPE:
                retVal = dbLobjSeek(srbObjDesc[descInx].lowLevelDescInx, 
		  (int) offset, whence);
		break;
            case DB_TABLE_TYPE:
                retVal = dbTableSeek(srbObjDesc[descInx].lowLevelDescInx, 
		  (int) offset, whence);
		break;
            default:                /* shouldn't get here */
                elog(NOTICE, "svrObjSeek: error in srbObjTypeEntry mapping");
                retVal = OBJ_ERR_OBJ_TYPE;
                break;
    	}
	if (retVal < 0)		/* error */
	    break;
	/* update the container info */
	if (srbObjDesc[descInx].contInx >= 0) {
	    int contInx = srbObjDesc[descInx].contInx;

	    srbObjDesc[contInx].contInfo->curOffset = newOffset;
	    if (newOffset > srbObjDesc[contInx].contInfo->curContainerSize)
		srbObjDesc[contInx].contInfo->curContainerSize = newOffset;
	}
        descInx = srbObjDesc[descInx].nextDescInx;
    }
    return (retVal);
}

int
svrCollSeek (int fd, int where, int whence, int is64Flag )
{

    int targPos;
    int result;
    char buf[MY_BLKSIZE];

    if (srbObjDesc[fd].collInfo == NULL) {
	elog(NOTICE, "svrCollSeek: opened fd=%d:objID=%s is not a collection",
	 fd, srbObjDesc[fd].objID);
	return (OBJ_ERR_NOT_COLL);
    }

    if (whence == SEEK_SET) {
        targPos = where;
    } else if (whence == SEEK_CUR) {
        targPos = srbObjDesc[fd].collInfo->accumOffset + 
	 srbObjDesc[fd].collInfo->offset + where;
    } else {
        targPos = -1;   /* assume to EOF */
    }

/*
    if (srbObjDesc[fd].gotData > 0 &&
*/
    if (targPos >= srbObjDesc[fd].collInfo->accumOffset && targPos <=
     srbObjDesc[fd].collInfo->accumOffset + 
      srbObjDesc[fd].collInfo->maxDirLen) {
        /* in the same page */
        srbObjDesc[fd].collInfo->offset = targPos - 
	  srbObjDesc[fd].collInfo->accumOffset;
        return (targPos);
    } else if (targPos >= 0) {
        if (targPos < srbObjDesc[fd].collInfo->accumOffset) {  /* start over */
            /* reset some parameter */
            if (srbObjDesc[fd].collInfo->dirent != NULL) {
                free (srbObjDesc[fd].collInfo->dirent);
                srbObjDesc[fd].collInfo->dirent = NULL;
            }
            srbObjDesc[fd].collInfo->maxDirLen = 
	     srbObjDesc[fd].collInfo->offset =
              srbObjDesc[fd].collInfo->accumOffset = 0;
            srbObjDesc[i].collInfo->continueIndex = -1;
            srbObjDesc[i].isDir = IS_DIR_1;
        }
        while ((result = _svrGetdents (fd, buf, MY_BLKSIZE, is64Flag)) > 0) {
            if (targPos <= srbObjDesc[fd].collInfo->accumOffset +
             srbObjDesc[fd].collInfo->maxDirLen) {
                srbObjDesc[fd].collInfo->offset = 
		 targPos - srbObjDesc[fd].collInfo->accumOffset;
                return (targPos);
            }
        }
        return -1;      /* can't get there */
    } else {    /*      SEEK_END */
        targPos = srbObjDesc[fd].collInfo->accumOffset +  
	 srbObjDesc[fd].collInfo->offset;
        /* seek to the end */
         while ((result = _svrGetdents (fd, buf, MY_BLKSIZE, is64Flag)) > 0) {
            targPos += result;
        }
        return (targPos);
    }
}

/* svrObjSync - Entry point for the srbObjSync SID.
 *
 *
 * Input : int descInx - The object descriptor index to sync.
 *
 * Output : Returns 0 - success, negative  - failure
 */

int
svrObjSync (int descInx)
{
    int tinx;
    int retVal;

    if (descInx < 0 || descInx >= MAX_SRBO_DESC ||
    srbObjDesc[descInx].lowLevelDescInx < 0) {
        elog(NOTICE,"svrObjSync: Invalid desc (%d)", descInx);
        return INP_ERR_DESC;
    }

    tinx = srbObjDesc[descInx].objTypeInx;

    switch (srbObjTypeEntry[tinx].objType) {
        case FILE_TYPE:
           retVal = svrFileSync (srbObjDesc[descInx].lowLevelDescInx);
                break;
        case DB_LOBJ_TYPE:
            retVal = 0;
            break;
        case DB_TABLE_TYPE:
            retVal = 0;
            break;
        default:                /* shouldn't get here */
            elog(NOTICE, "svrObjClose: error in srbObjTypeEntry mapping");
            retVal = OBJ_ERR_OBJ_TYPE;
            break;
    }

    return retVal;
}

/*
 * svrObjStat - Stat a data object.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int storSysType - Storage system type.
 *              0 = Unix, 1 = UniTree, 2 = HPSS, 3 = FTP.
 *         struct varlena *hostAddr - The Host address of the storage.
 *         struct varlena *filename - The file Path name to stat.
 *         Relative path => Storage Vault.
 *
 * Output - stat result in varlena format.
 */

struct varlena *
svrObjStat(int catType, struct varlena *vPath, int myType)
{
    struct varlena *retval;
    int status = 0;
    char *path;
#if defined(PORTNAME_c90)
    struct srbStat myStat;
    struct srbStat *retStat;
#else
    struct srbStat *retStat;
#endif

    path = varToStr (vPath);

    retval = (struct varlena *)malloc(VAROUTHDRSZ + sizeof (struct srbStat));
#if defined(PORTNAME_c90)
    retStat = &myStat;
#else
    retStat = (struct srbStat *) (VARDATA(retval) + sizeof(INT32));
#endif
    memset (retStat, 0, sizeof (struct srbStat));

    status = _svrObjStat(catType, path, myType, retStat);

ProcessStatus:

    free (path);

    if (status < 0) {           /* error */
        VARSIZE(retval) = VAROUTHDRSZ;
        VAROUTSTAT(retval) = htonl (status);
    } else {
        htonStat (retStat);
#if defined(PORTNAME_c90)
        memcpy (VAROUTDATA(retval), (char *) retStat, sizeof(struct srbStat));
#endif  /* PORTNAME_c90 */
        VARSIZE(retval) = sizeof (struct srbStat) + VAROUTHDRSZ;
        VAROUTSTAT(retval) = 0;
    }

    return retval;
}

int
_svrObjStat(int catType, char *path, int myType, struct srbStat *retStat)
{
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;


    remoteFlag = getMdasEnabledHost (path, &hostTabPtr);

    if (remoteFlag < 0) {
        return (remoteFlag);
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
    	mdasC_sql_result_struct *myresult;
    	char dataName[MAX_TOKEN], collection[MAX_TOKEN];
    	int myUid, myGid, moMode;


	if ((status = mySplitPath (path, collection, dataName)) < 0)
	    return status;

    	if (myType <= IS_FILE &&
    	 (status = svrDataStat (catType, dataName, collection, 
	 &myresult)) >= 0) {
            /* interprete the info */
            char *sizeList, *timeList, *phyResc, *dirty;
	    int i, dirtyInx = 0;

            sizeList = (char *) getFromResultStruct(
             (mdasC_sql_result_struct *) myresult,
              dcs_tname[SIZE], dcs_aname[SIZE]);
            timeList = (char *) getFromResultStruct(
             (mdasC_sql_result_struct *) myresult,
              dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
            phyResc = (char *) getFromResultStruct(
             (mdasC_sql_result_struct *) myresult,
              dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
            dirty = (char *) getFromResultStruct(
             (mdasC_sql_result_struct *) myresult,
              dcs_tname[IS_DIRTY], dcs_aname[IS_DIRTY]);


	    for (i = 0; i < myresult->row_count; i++) {
		if (atoi (&dirty[i*MAX_DATA_SIZE]) > 0) {
		    dirtyInx = i;
		    break;
		}
	    }
	    sizeList += dirtyInx * MAX_DATA_SIZE;
	    timeList += dirtyInx * MAX_DATA_SIZE;
	    phyResc += dirtyInx * MAX_DATA_SIZE;
	    
            parseDataUserMode (myresult, ProxyUser, ClientUser, 
	     &myUid, &myGid, &moMode);
            retStat->st_uid = myUid;
            retStat->st_gid = myGid;
            retStat->st_mode = moMode;
            retStat->st_mode |= S_IFREG;

            retStat->st_ino = FAKE_INODE + strlen (path);
            retStat->st_atim = retStat->st_mtim = 
	     retStat->st_ctim = (srb_time_t) srbToLocalTime (timeList);
            retStat->st_size = strtoll (sizeList, 0, 0);
            retStat->st_dev = FAKE_DEV;
            retStat->st_blksize = MY_BLKSIZE;
            retStat->st_blocks = retStat->st_size/MY_BLKSIZE + 1;
             retStat->st_nlink = 1;
#ifdef PORTNAME_solaris
            strncpy (retStat->st_fstype, phyResc, FSTYPSZ - 1);
            retStat->st_fstype[FSTYPSZ - 1] = '\n';
#endif  /* PORTNAME_solaris */
            freeSqlResult (myresult);
	} else if (strcmp (path, "/") == 0) {	/* root collection */
            retStat->st_uid = 0;
            retStat->st_gid = 0;
            retStat->st_mode = 0755;

            retStat->st_mode |= S_IFDIR;
            retStat->st_dev = FAKE_DEV;
            retStat->st_size = MY_BLKSIZE;
            retStat->st_blksize = MY_BLKSIZE;
	    /* strlen (path) is needed to get rm -r working. */
            retStat->st_ino = FAKE_INODE + strlen (path);
            retStat->st_blocks = 1;
            retStat->st_nlink = 3;
#ifdef PORTNAME_solaris
            strcpy (retStat->st_fstype, "srb");
#endif
            retStat->st_atim = retStat->st_mtim = 
	     retStat->st_ctim = (srb_time_t) 999999;
            status = 0;
    	} else if (myType != IS_FILE && svrCollStat (catType, path, 
	 &myresult) >= 0) {
            /* a directory */
            parseCollUserMode (myresult, ProxyUser, ClientUser,
	     &myUid, &myGid, &moMode);
            retStat->st_uid = myUid;
            retStat->st_gid = myGid;
            retStat->st_mode = moMode;

            retStat->st_mode |= S_IFDIR;
            retStat->st_dev = FAKE_DEV;
            retStat->st_size = MY_BLKSIZE;
            retStat->st_blksize = MY_BLKSIZE;
	    /* strlen (path) is needed to get rm -r working. */
            retStat->st_ino = FAKE_INODE + strlen (path);
            retStat->st_blocks = 1;
            retStat->st_nlink = 3;
#ifdef PORTNAME_solaris
            strcpy (retStat->st_fstype, "srb");
#endif
            retStat->st_atim = retStat->st_mtim = 
	     retStat->st_ctim = (srb_time_t) 999999;
            status = 0;
            freeSqlResult (myresult);
	} else {
	    if (status == 0)
	        status = MCAT_INQUIRE_ERROR;
        }
#endif
    } else {
	status = remoteObjStat (catType, path, myType, retStat, hostTabPtr);
    }
    return status;
}

struct varlena *
svrObjGetdents (int catType, int fd, int nbyte)
{
    struct varlena *retval;
    int status = 0;
    dirent_t *mydirent;

    mydirent = (dirent_t *) malloc (nbyte);

    fd = fd & (~IS_COLL_FD);	/* strip out the IS_COLL_FD bit */
    status = _svrGetdents (fd, mydirent, nbyte, IS_32_BIT);

ProcessStatus:

    retval = (struct varlena *)malloc(VAROUTHDRSZ + nbyte);
    if (status < 0) {           /* error */
        VARSIZE(retval) = VAROUTHDRSZ;
        VAROUTSTAT(retval) = htonl (status);
    } else {
	if (status > 0) {
	    htonDirent (mydirent, status);
            memcpy (VAROUTDATA(retval), (char *) mydirent, status);
	}
        VARSIZE(retval) = status + VAROUTHDRSZ;
        VAROUTSTAT(retval) = htonl (status);
    }
    free (mydirent);

    return retval;
}

struct varlena *
svrObjGetdents64 (int catType, int fd, int nbyte)
{
    dirent64_t *mydirent;
    struct varlena *retval;
    int status = 0;

    mydirent = (dirent64_t *) malloc (nbyte);

    fd = fd & (~IS_COLL_FD);    /* strip out the IS_COLL_FD bit */
    status = _svrGetdents (fd, mydirent, nbyte, IS_64_BIT);

ProcessStatus:

    retval = (struct varlena *)malloc(VAROUTHDRSZ + nbyte);
    if (status < 0) {           /* error */
        VARSIZE(retval) = VAROUTHDRSZ;
        VAROUTSTAT(retval) = htonl (status);
    } else {
        if (status > 0) {
            htonDirent64 (mydirent, status);
            memcpy (VAROUTDATA(retval), (char *) mydirent, status);
        }
        VARSIZE(retval) = status + VAROUTHDRSZ;
        VAROUTSTAT(retval) = htonl (status);
    }
    free (mydirent);

    return retval;
}

int
svrObjReplicate (int catType, struct varlena *vObjID,
struct varlena *vCollectionName,
struct varlena *vNewResourceName, struct varlena *vNewPathName)
{
    char *objID;
    char *collectionName;
    char *newResourceName;
    char *newPathName;
    int retVal;

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    newPathName = varToStr (vNewPathName);
    newResourceName = varToStr (vNewResourceName);

    retVal = _svrObjReplicate (catType, objID, collectionName, 
     newResourceName, newPathName);

    free (objID);
    free (collectionName);
    free (newResourceName);
    free (newPathName);

    return retVal;
}

int
_svrObjReplicate (int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName)
{
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOut;
    char *userName;
    int newObjInx;
    int retVal = 0; 
    int retVal1 = 0; 
    int status;
    struct mdasResInfo *newResInfo;
#ifdef _WIN32
    char *buf=NULL;
#else
    char buf[BUF_SIZE];
#endif
    int nbytes;
#ifdef _WIN32
    char *newFullPathName=NULL;
#else
    char newFullPathName[HUGE_STRING];
#endif
    char *tmpPathName;
    char *tmpStr, *tmpStr1, *myObjID;
    struct mdasInfoOut *newInfoOut;
    char *tempStr;
    struct condResult condResult;
    char *condInput;


    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

#ifdef _WIN32
    buf = (char *)calloc(BUF_SIZE,sizeof(char));
    newFullPathName = (char *)calloc(HUGE_STRING,sizeof(char));
#endif

    /* Try to open the objID */

    infoInp.domainName = ClientUser->domainName;
    infoInp.objID = objID;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = collectionName;
    infoInp.accessMode = "write";

    /* Check with MDAS for the object to be replicated */
    if ((retVal = mdasGetInfo (&infoInp, &infoOut)) < 0) {
#ifdef _WIN32
        free(buf); buf = NULL;
        free(newFullPathName); newFullPathName = NULL;
#endif
        return retVal;
    }

    /* Cannot replicate inContainer Obj yet */

    if (infoOut->containerName[0] != '\0') {  /* inContainer object */
	if (!myIsContainer (objID, collectionName, infoOut->containerName)) {
	    /* not a container, but a inContainer obj */
	    elog (NOTICE,
	     "_svrObjReplicate: Replication of inContainer obj %s not allowed",
	  objID);
#ifdef _WIN32
        free(buf); buf = NULL;
        free(newFullPathName); newFullPathName = NULL;
#endif
	    return (OBJ_ERR_REPL_INCONT_OBJ_NOT_ALLOWED);
	}
    }

    tempStr = strdup (objID);
    condInput = malloc (strlen (objID) + 1);
    if (mySplitStr (tempStr, objID, condInput, '&') == NULL)
        condInput[0] = '\0';

    if (parseCondInput (condInput, STANDF_STAGE, &condResult) >= 0) {
        retVal = _standfStage (catType, tempStr, collectionName,
         newResourceName, newPathName, infoOut, condResult.intResult);
    } else {
        /* sortInfoForRepl (&infoOut, newResourceName);  */ 
        sortAndCombInfoForObj (&infoOut, NULL, 0, 0);

        retVal = _objReplicate (catType, objID, collectionName, 
         newResourceName, newPathName, infoOut, &newInfoOut, NULL);

        if (retVal >= 0 && infoOut->isDirty > 0) {
 /**RAJA June 20, 2003 changed retval to retVal1 in this block**/
            retVal1 = _svrModifyDataset (MDAS_CATALOG,
             objID, collectionName, newInfoOut->resourceName, 
	     newInfoOut->dataPath, "1", NULL, D_CHANGE_DIRTY);
	    if (retVal1 < 0) 
	        elog (NOTICE,
                 "_svrObjReplicate: _svrModifyDataset of %s error, status = %d",
                  objID, retVal1);
        }
    }

    free (tempStr);
    free (condInput);
    freeInfoOut (newInfoOut);
    freeInfoOut (infoOut);

#ifdef _WIN32
        free(buf); buf = NULL;
        free(newFullPathName); newFullPathName = NULL;
#endif
    return (retVal);
}

int 
_objReplicate (int catType, char *objID, char *collectionName, 
char *newResourceName, char *newPathName, struct mdasInfoOut *origInfoOut, 
struct mdasInfoOut **newInfoOut, struct mdasResInfo *inpResInfo)
{
    struct mdasInfoOut *infoOut, *tmpInfoOut;
    int newObjInx;
    int retVal = 0;
    int status;
    struct mdasResInfo *newResInfo;
    char newFullPathName[MAX_TOKEN * 2];
    char compPathName[MAX_TOKEN * 2];
    char *myPathName;
    char *tmpPathName;
    srb_long_t totalWritten;
    char *tmpStr, *tmpStr1, *myObjID;
    struct mdasResInfo *compResInfo, *myResInfo;
    int intReplNum, replNum;
    int lockFd = -1;
    struct resCache *myResCache;
    struct hostElement  *hostTabPtr;
    int remoteFlag;
    char *portNum, tmpResLoc[MAX_TOKEN];



    if (origInfoOut == NULL)
	return OBJ_ERR_REPL_COND;

    *newInfoOut = NULL;

    infoOut = origInfoOut;

    /* Now try to create the new copy */

    /* If inpResInfo != NULL, use it. Otherwise, call getResInfo() to
     * get it.
     */
    if (inpResInfo == NULL) {
        /* find the resourceType and resourceLoc associated with 
	 * newResourceName and domainName */

        if (getResInfo (collectionName, catType, newResourceName, 
	 ClientUser->domainName, &myResCache) < 0) {
            elog (NOTICE,
              "_objReplicate: %s and %s is not a registered resource",
              newResourceName, ClientUser->domainName);
            return OBJ_ERR_RES_NOT_REG;
        }
#ifdef foo
        if (newResInfo->next != NULL) {
            elog (NOTICE,
              "_objReplicate: Resource is a resource set.Must be a single res");
	    if (inpResInfo == NULL)
	        freeResInfo (newResInfo);
            return OBJ_ERR_RES_NOT_SINGLE;
        }
#endif
	sortResForIngest (myResCache, infoOut->size);	/* new code 5/6/04 */

	newResInfo = myResCache->resInfoHead;
    } else {
        newResInfo = inpResInfo;
    }

    tmpStr = malloc (strlen (objID) + 1);
    tmpStr1 = malloc (strlen (objID) + 1);
    mySplitStr (objID, tmpStr, tmpStr1, '&');
    free (tmpStr1);
    myObjID = tmpStr;

    while (infoOut != NULL) {
	compResInfo = NULL;
        if (strcmp (newResInfo->phyResType, COMPOUND_RESC) == 0) {
            compResInfo = newResInfo->compoundLink;
            while (compResInfo != NULL) {
		int resClassInx;
                /* XXXXX don't do TAPE_RESC 
                if (strcmp (compResInfo->phyResType, TAPE_RESC) != 0) { */ 
		resClassInx = resClassLookup (compResInfo->phyResClass);
                /* Don't do permanent */
#ifdef LOCK_OBJ
                /* Don't lock container object. It has its own locking */
                if (infoOut->containerName[0] == '\0' && lockFd < 0) {
                    lockFd =_svrObjLock (infoOut->objID, collectionName,
                      F_SETLKW, F_WRLCK, 0);
                }
#endif
                if (resClassEntries[resClassInx].permanent == 0) {
		    totalWritten = _svrDataCopyFromInfoToResc ( catType, 
		      myObjID, collectionName,
		      infoOut, compResInfo, newPathName, newFullPathName,
		       &newObjInx);
		    if (totalWritten >= 0)
			break;
		}
		compResInfo = compResInfo->next;
	    }
	} else {
	    compResInfo = NULL;
	    totalWritten = _svrDataCopyFromInfoToResc ( catType, 
	      myObjID, collectionName,
              infoOut, newResInfo, newPathName, newFullPathName,
               &newObjInx);
	}

    	if (totalWritten >= 0) {       /* success */
	    break;
	} else {
#ifdef LOCK_OBJ
            if (lockFd >= 0) {
                _svrObjLock (infoOut->objID, collectionName,
                  F_SETLKW, F_UNLCK, lockFd);
	        lockFd = -1;
	    }
#endif
	}
	infoOut = infoOut->next;
    }

    /* myPathName is the path that was actually used */
    if (compResInfo != NULL) {
        /* Create a fake pathName for the compound obj */
        genCompPath (compPathName, objID);
        myPathName = compPathName;
	myResInfo = compResInfo;
    } else {
        myPathName = newFullPathName;
	myResInfo = newResInfo;
    }


    if (totalWritten < 0) {
#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (origInfoOut->objID, collectionName,
              F_SETLKW, F_UNLCK, lockFd);
#endif
        free (myObjID);
	return totalWritten;
    }
	    
    /* Now register with MDAS */

    replNum = _svrRegisterReplica (catType, myObjID, collectionName,
      infoOut->resourceName, infoOut->dataPath, 
      newResInfo->phyResName, myPathName,
      1, newObjInx, newResInfo->phyResLoc,
      ClientUser->userName, ClientUser->domainName);
    /**RAJA June 20, 2003 **/
    retVal = replNum;
    if (retVal < 0) {		/* error */
#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (origInfoOut->objID, collectionName,
              F_SETLKW, F_UNLCK, lockFd);
#endif
        free (myObjID);
	return retVal;
    }

    if (compResInfo != NULL) {
	intReplNum = _svrRegInternalCompObj (
	  objID, collectionName, replNum, NO_INT_SEGMENTATION, 
	  compResInfo->phyResName, newFullPathName, totalWritten, 0,
	  NEW_INT_REPL_NUM, NO_INT_SEGMENTATION, 
          newObjInx, compResInfo->phyResLoc,
	  ClientUser->userName, ClientUser->domainName); 
	if (intReplNum < 0) {
#ifdef LOCK_OBJ
            if (lockFd >= 0)
                _svrObjLock (origInfoOut->objID, collectionName,
                  F_SETLKW, F_UNLCK, lockFd);
#endif
            free (myObjID);
            return intReplNum;
	}
    } else {
	intReplNum = -1;
    }

    /* XXXXXX have to visit here again to deal with compound obj */
    /* Everythings are OK */
    if (newInfoOut != NULL) {
	/* If it is a comp obj, make the infoOut for the comp obj first */
        tmpInfoOut = malloc (sizeof (struct mdasInfoOut));
	memset (tmpInfoOut, 0, sizeof (struct mdasInfoOut));
        tmpInfoOut->objID = myObjID;
        tmpInfoOut->collection = strdup (collectionName);
	/* use the actual path */
        tmpInfoOut->dataPath = strdup (myPathName);
        tmpInfoOut->resourceLoc = strdup (newResInfo->phyResLoc);
        tmpInfoOut->resourceName = strdup (newResInfo->phyResName);
	if (infoOut->logResName != NULL) {
            tmpInfoOut->logResName = strdup (infoOut->logResName);
	} else {
	    tmpInfoOut->logResName = NULL;
	}
        tmpInfoOut->dataType = strdup (infoOut->dataType);
        tmpInfoOut->objTypeInx = objTypeLookUp (newResInfo->phyResType);
        tmpInfoOut->replNum= replNum;        /* don't know */
        tmpInfoOut->segNum= NO_INT_SEGMENTATION;        /* don't know */
	tmpInfoOut->size = infoOut->size;
	tmpInfoOut->offset = infoOut->offset;
        tmpInfoOut->isDirty = infoOut->isDirty;
        tmpInfoOut->chksum = infoOut->chksum;
        tmpInfoOut->phyResClass = strdup (newResInfo->phyResClass);
        tmpInfoOut->resClassInx = resClassLookup (tmpInfoOut->phyResClass);
        if (infoOut->containerName != NULL) {
            tmpInfoOut->containerName = strdup (infoOut->containerName);
        } else {
            tmpInfoOut->containerName = NULL;
        }
        tmpInfoOut->containerMaxSize = infoOut->containerMaxSize;
        tmpInfoOut->tapeFileInfo = NULL;
        tmpInfoOut->compoundLink = NULL;
	tmpInfoOut->intReplNum = -1;
	tmpInfoOut->intSegNum = NO_INT_SEGMENTATION;
	tmpInfoOut->next = NULL;
        *newInfoOut = tmpInfoOut;
	if (compResInfo != NULL) {
            /* make one for the int compound obj */
            tmpInfoOut = malloc (sizeof (struct mdasInfoOut));
            memset (tmpInfoOut, 0, sizeof (struct mdasInfoOut));
            tmpInfoOut->objID = strdup (myObjID);
            tmpInfoOut->collection = strdup (collectionName);
            tmpInfoOut->dataPath = strdup (newFullPathName);
            tmpInfoOut->resourceLoc = strdup (compResInfo->phyResLoc);
            tmpInfoOut->resourceName = strdup (compResInfo->phyResName);
            if (infoOut->logResName != NULL) {
                tmpInfoOut->logResName = strdup (infoOut->logResName);
            } else {
                tmpInfoOut->logResName = NULL;
            }
            tmpInfoOut->dataType = strdup (infoOut->dataType);
            tmpInfoOut->objTypeInx = newObjInx;
            tmpInfoOut->replNum= replNum;        /* don't know */
            tmpInfoOut->segNum= NO_INT_SEGMENTATION;        /* don't know */
            tmpInfoOut->size = infoOut->size;
            tmpInfoOut->offset = infoOut->offset;
            tmpInfoOut->isDirty = 0;
            tmpInfoOut->chksum = infoOut->chksum;
            tmpInfoOut->phyResClass = strdup (compResInfo->phyResClass);
            tmpInfoOut->resClassInx = resClassLookup (tmpInfoOut->phyResClass);
            if (infoOut->containerName != NULL) {
                tmpInfoOut->containerName = strdup (infoOut->containerName);
            } else {
                tmpInfoOut->containerName = NULL;
            }
            tmpInfoOut->containerMaxSize = infoOut->containerMaxSize;
            tmpInfoOut->tapeFileInfo = NULL;
            tmpInfoOut->compoundLink = NULL;
            tmpInfoOut->intReplNum = intReplNum;
            tmpInfoOut->intSegNum = NO_INT_SEGMENTATION;
            tmpInfoOut->next = NULL;
	    /* link it to the compound obj */
	    (*newInfoOut)->compoundLink = tmpInfoOut;
	} else {
            tmpInfoOut->compoundLink = NULL;
	}
    } else {
	free (myObjID);
    }
#ifdef LOCK_OBJ
    if (lockFd >= 0)
        _svrObjLock (origInfoOut->objID, collectionName,
          F_SETLKW, F_UNLCK, lockFd);
#endif


#ifdef SRB_MDAS
    if (retVal >= 0) {
        retVal = commit_db2_interaction (MDAS_CAT_COMMIT);
        if (retVal < 0)
            elog (NOTICE,
              "_objReplicate: commit_db2_interaction %s error=%d",
                objID, retVal);
    }
#endif
    /**RAJA June 20, 2003  changed retVal to replNum**/
    return replNum;
}

int
svrObjMove (int catType, struct varlena *vObjID,
struct varlena *vCollectionName,
struct varlena *vNewResourceName, struct varlena *vNewPathName)
{
    char *objID;
    char *collectionName;
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOut;
    char *newResourceName;
    char *newPathName;
    char *userName;
    int retVal = 0;
    int status;
    int origObjInx, newObjInx;
    int origExfInx, newExfInx;
    struct mdasResInfo *newResInfo, *myResInfo;
#ifdef _WIN32
    char *buf = NULL;
#else
    char buf[BUF_SIZE];
#endif
    int nbytes;
    char newFullPathName[MAX_TOKEN * 2];
    char compPathName[MAX_TOKEN * 2];
    char *myPathName;
    char *tmpPathName;
    char *tmpStr, *tmpStr1;
    struct mdasResInfo *compResInfo;
    int intReplNum, replNum;
    int lockFd = -1;
    struct resCache *myResCache;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    newPathName = varToStr (vNewPathName);
    newResourceName = varToStr (vNewResourceName);

    /* Try to open the objID */

    infoInp.domainName = ClientUser->domainName;
    infoInp.objID = objID;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = collectionName;
    infoInp.accessMode = "write";

    /* Check with MDAS for the object to be replicated */
    if ((retVal = mdasGetInfo (&infoInp, &infoOut)) < 0) {
        free (collectionName);
        free (objID);
        free (newResourceName);
        free (newPathName);

#ifdef SRB_MDAS
        if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
        return retVal;
    }

    sortAndCombInfo (&infoOut);

    /* Cannot Move inContainer Obj yet */

    if (infoOut->containerName[0] != '\0') {  /* inContainer object */
        elog (NOTICE,
         "svrObjMove: Cannot move an inContainer obj %s ",
          objID);
        return (OBJ_ERR_MOVE_INCONT_OBJ_NOT_ALLOWED);
    }

    if (strncmp (newResourceName, "&CONTAINER=", strlen ("&CONTAINER=")) == 0) {
	status = _svrObjMoveToCont (infoOut, objID, collectionName,
	 newResourceName + strlen ("&CONTAINER="));
 
        free (collectionName);
        free (objID);
        free (newResourceName);
        free (newPathName);
	return (status);
    }

#ifdef _WIN32
    buf = (char *)calloc(BUF_SIZE,sizeof(char));
#endif

    origObjInx = infoOut->objTypeInx;

    if (origObjInx < 0) {
        elog(NOTICE,
         "svrObjMove: resourceName:%s not supported",
          infoOut->resourceName);
        retVal = OBJ_ERR_RES_TYPE;

	freeInfoOut (infoOut);
        free (collectionName);
        free (objID);
        free (newResourceName);
        free (newPathName);

#ifdef SRB_MDAS
        if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
        return retVal;
    }

   /* Call open at the next level */

   if (srbObjTypeEntry[origObjInx].objType == COMPOUND_TYPE) {
        /* a compound object.  */
	struct mdasInfoOut *cacheInfoOut;

#ifdef LOCK_OBJ
        /* Don't lock container object. It has its own locking */
        if (infoOut->containerName[0] == '\0' && lockFd < 0) {
            lockFd =_svrObjLock (infoOut->objID, collectionName,
              F_SETLKW, F_WRLCK, 0);
        }
#endif
        origExfInx = _svrStageCompObj (infoOut, collectionName, &cacheInfoOut,
	 1);
	if (origExfInx >= 0) {
	    origExfInx = lowLevelOpen (cacheInfoOut->objTypeInx, 
	      cacheInfoOut->dataPath, cacheInfoOut->resourceLoc, O_RDONLY);
	      freeInfoOut (cacheInfoOut);
	}
    } else {
	origExfInx = lowLevelOpen (origObjInx, infoOut->dataPath,
     	 infoOut->resourceLoc, O_RDONLY);
    }

    if (origExfInx < 0) {       /* failure */
        elog (NOTICE, "svrObjMove: Unable of open the original copy");
        retVal = origExfInx;
        freeInfoOut (infoOut);
        free (collectionName);
        free (objID);
        free (newResourceName);
        free (newPathName);

#ifdef LOCK_OBJ
	if (lockFd >= 0)
            _svrObjLock (infoOut->objID, collectionName, 
	      F_SETLKW, F_UNLCK, lockFd);
#endif

#ifdef SRB_MDAS
        if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
        return retVal;
    }

    /* Now try to create the new copy */

    /* find the resourceType and resourceLoc associated with newResourceName
     * and domainName */


    if (getResInfo (collectionName, catType, newResourceName, 
     ClientUser->domainName, &myResCache) < 0) {
        elog (NOTICE,
          "svrObjMove: %s and %s is not a registered resource",
          newResourceName, ClientUser->domainName);
        retVal = OBJ_ERR_RES_NOT_REG;
        freeInfoOut (infoOut);
        free (collectionName);
        free (objID);
        free (newResourceName);
        free (newPathName);

#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (infoOut->objID, collectionName, 
              F_SETLKW, F_UNLCK, lockFd);
#endif
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
        return retVal;
    }

    sortResForIngest (myResCache, infoOut->size);  /* new code 5/6/04 */

#ifdef foo
    if (newResInfo->next != NULL) {
        elog (NOTICE,
          "svrObjMove: The newResourceName is a resource set. It must be a single resource");
        retVal = OBJ_ERR_RES_NOT_SINGLE;
        freeInfoOut (infoOut);
        free (collectionName);
        free (objID);
        free (newResourceName);
        free (newPathName);

#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (infoOut->objID, collectionName, 
              F_SETLKW, F_UNLCK, lockFd);
#endif
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
        return retVal;
    }
#endif	/* foo */

    userName = ClientUser->userName;

    tmpStr = malloc (strlen (objID) + 1);
    tmpStr1 = malloc (strlen (objID) + 1);
    mySplitStr (objID, tmpStr, tmpStr1, '&');
    free (tmpStr1);
    free (objID);
    objID = tmpStr;

    compResInfo = NULL;
    if (strcmp (myResCache->resInfoHead->phyResType, COMPOUND_RESC) == 0) {
        compResInfo = myResCache->resInfoHead->compoundLink;
        while (compResInfo != NULL) {
	    int resClassInx;
            /* XXXXX don't do TAPE_RESC 
            if (strcmp (compResInfo->phyResType, TAPE_RESC) != 0) { */ 
	    resClassInx = resClassLookup (compResInfo->phyResClass);
            /* Don't do permanent */
#ifdef LOCK_OBJ
	    if (lockFd < 0)
                lockFd =_svrObjLock (infoOut->objID, collectionName,
                  F_SETLKW, F_WRLCK, 0);
#endif
            if (resClassEntries[resClassInx].permanent == 0) {
	        newExfInx = _objCreateWithResInfo (objID, collectionName,
                newPathName, -1, compResInfo, newFullPathName, &newObjInx);
                if (newExfInx >= 0)
                    break;
            }
            compResInfo = compResInfo->next;
        }
    } else {
        newExfInx = _objCreateWithResInfo (objID, collectionName, 
         newPathName, -1, myResCache->resInfoHead, newFullPathName, &newObjInx);
    }
 
    if (newExfInx < 0) {   /* error */
        elog (NOTICE, "svrObjMove: Unable to create the new copy");
        lowLevelClose (origObjInx, origExfInx);
        retVal = newExfInx;
        freeInfoOut (infoOut);
        free (collectionName);
        free (objID);
        free (newResourceName);
        free (newPathName);

#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (infoOut->objID, collectionName, 
              F_SETLKW, F_UNLCK, lockFd);
#endif

#ifdef SRB_MDAS
        if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
        return retVal;
    }

    /* now read from origExfInx and write to newExfInx */

    while ((nbytes = _objRead(origExfInx, origObjInx, buf, BUF_SIZE)) > 0) {
        int byteWritten;
        if ((byteWritten = _objWrite (newExfInx, newObjInx, buf, nbytes))
          != nbytes) {
            elog (NOTICE, "svrObjMove: Bytes read %d does not match written %d",
             nbytes, byteWritten);
            if (byteWritten < 0)
                retVal = byteWritten;
            else
                retVal = OBJ_ERR_COPY_LEN;
            break;
        }
    }

    lowLevelClose (origObjInx, origExfInx);
    lowLevelClose (newObjInx, newExfInx);

    if (compResInfo != NULL) {
        /* Create a fake pathName for the compound obj */
        genCompPath (compPathName, objID);
        myPathName = compPathName;
	myResInfo = compResInfo;
    } else {
        myPathName = newFullPathName;
	myResInfo = myResCache->resInfoHead;
    }

    if (retVal < 0) {          /* no error */
        /* Unlink the newly created data object */

        _objUnlink (srbObjTypeEntry[newObjInx].objType,
          srbObjTypeEntry[newObjInx].systemType,
            myResCache->resInfoHead->phyResLoc, newFullPathName, NO_CHKDIR);
        freeInfoOut (infoOut);
        free (collectionName);
        free (objID);
        free (newResourceName);
        free (newPathName);
#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (infoOut->objID, collectionName, 
              F_SETLKW, F_UNLCK, lockFd);
#endif
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
        return (retVal);
    }

    /* Now register with MDAS */

    replNum = _svrRegPhyMove (catType, objID, collectionName,
      infoOut->resourceName, infoOut->dataPath,
       myResInfo->phyResName, newFullPathName);

    if (replNum < 0) {		/* error */
	elog (NOTICE, 
	  "svrObjMove: Unable to register move_dataset with MDAS. Status = %d",
	   replNum);
        _objUnlink (srbObjTypeEntry[newObjInx].objType,
         srbObjTypeEntry[newObjInx].systemType,
          myResCache->resInfoHead->phyResLoc, myPathName, NO_CHKDIR);
        freeInfoOut (infoOut);
        free (collectionName);
        free (objID);
        free (newResourceName);
        free (newPathName);
#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (infoOut->objID, collectionName, 
              F_SETLKW, F_UNLCK, lockFd);
#endif
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
	return (replNum);
    }

    intReplNum = -1;
    if (compResInfo != NULL) {
        /* This is a internal compound resource */
        intReplNum = _svrRegInternalCompObj (
          objID, collectionName, replNum, NO_INT_SEGMENTATION,
          compResInfo->phyResName, newFullPathName, infoOut->size, 0,
          NEW_INT_REPL_NUM, NO_INT_SEGMENTATION,
          newObjInx, compResInfo->phyResLoc,
          ClientUser->userName, ClientUser->domainName);

        if (intReplNum < 0) {
            freeInfoOut (infoOut);
            free (collectionName);
            free (objID);
            free (newResourceName);
            free (newPathName);
#ifdef LOCK_OBJ
            if (lockFd >= 0)
                _svrObjLock (infoOut->objID, collectionName, 
                  F_SETLKW, F_UNLCK, lockFd);
#endif
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
	    return (intReplNum);
        }
    }

    /* Everything is fine. Unlink the original data object */

    _objUnlink (srbObjTypeEntry[origObjInx].objType,
      srbObjTypeEntry[origObjInx].systemType,
      infoOut->resourceLoc, infoOut->dataPath, NO_CHKDIR);

#ifdef LOCK_OBJ
    if (lockFd >= 0)
        _svrObjLock (infoOut->objID, collectionName, 
          F_SETLKW, F_UNLCK, lockFd);
#endif


    freeInfoOut (infoOut);

    free (collectionName);
    free (objID);
    free (newResourceName);
    free (newPathName);

#ifdef SRB_MDAS
    if (retVal == 0)
        commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
#ifdef _WIN32
        free(buf); buf = NULL;
#endif
    return retVal;
}

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

static int mdasGetInfo 
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
	infoOut->intSegNum = -1;

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

static int 
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

static char *
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
    struct inputValue *myReplValue;
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
    char *portNum, tmpResLoc[MAX_TOKEN];

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
        tmpResInfo->phyResMaxObjSize = strtoll (&phyResClass[j*MAX_DATA_SIZE],
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
    char *portNum, tmpStr[MAX_TOKEN];

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
                elog (NOTICE, "getResInfo: getCompoundResInfo err, status = %d",
                 status);
            }
	    tmpResInfo->compoundLink = myCacheRes->resInfoHead;
        }
        tmpResInfo->phyDirPathName = strdup (&phyDirPathName[j*MAX_DATA_SIZE]);
        tmpResInfo->logDirPathName = strdup (&logDirPathName[j*MAX_DATA_SIZE]);
        tmpResInfo->phyResClass = strdup (&phyResClass[j*MAX_DATA_SIZE]);
        tmpResInfo->phyResMaxObjSize = strtoll (&phyResClass[j*MAX_DATA_SIZE],
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
    char *pathName, *fullPathName;
    char *userName, *resourceType, *resourceLoc;
    int objInx, retVal;
    int chdirFlag;
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
    char *pathName, *fullPathName;
    char *userName, *domainName, *access;
    char *rmObjResLoc;
    int objInx, retVal;
    int chdirFlag;
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
svrBulkRegister (int catType, struct varlena *vContainer,
struct varlena *packedResult)
{
    char *container;
    mdasC_sql_result_struct *myresult;
    char *domainName, *userName;
    int retVal;
    char contName[MAX_TOKEN];
    char contColl[MAX_TOKEN];
    int noCheckFlag;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    container = varToStr (vContainer);

    retVal = unpackMsg (VAROUTDATA (packedResult), (char **) &myresult,
     sqlResultStruct_PF, NULL);

    if (retVal < 0) {
        elog(NOTICE,
         "svrBulkRegister: unpackMsg() of myresult error, status = %d",
          retVal);
        free (container);
        return (retVal);
    }

    if (ProxyUser->privUserFlag == 0) {
        noCheckFlag = 0;
    } else {
        if (ClientUser->privUserFlag == 1) {
            noCheckFlag = 1;
        } else {
            if (myresult->continuation_index > 0)
                noCheckFlag = 1;
            else
                noCheckFlag = 0;
        }
    }

    retVal = _svrBulkRegister (catType, container, myresult, noCheckFlag);
    free (container);
    freeSqlResult (myresult);

    return retVal;
}

int
_svrBulkRegister (int catType, char *container, 
mdasC_sql_result_struct *myresult, int noCheckFlag)
{
    char *domainName, *userName;
    int retVal;
    char contName[MAX_TOKEN];
    char contColl[MAX_TOKEN];
    int remoteFlag;
    struct hostElement  *hostTabPtr;
    char *collectionName;

    char *inCollection;

    inCollection = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    if (inCollection == NULL) {
        remoteFlag = getMdasEnabledHost (container, &hostTabPtr);
    } else {
        remoteFlag = getMdasEnabledHost (inCollection, &hostTabPtr);
    }

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
      /**** RAJA: Jan 27,2003 added registration of resource datasets ***/
      splitbychar(container, contColl,contName,'/');
      if (isContainerByName(contName,contColl) == MDAS_ZERO) {
	if(noCheckFlag > 0 ||
          (retVal = checkCommonPathPermissionFromResultStruct(catType, 
	    ClientUser->userName, ClientUser->domainName, container, myresult)
	    ) >= 0) {
	  retVal = registerNormalDatasetInBulk (catType, 
	    ClientUser->userName, ClientUser->domainName, container, myresult,
	    noCheckFlag);
	}
      }
      else {
#ifdef FED_MCAT
        collectionName =  (char *) get_from_result_struct(
         (mdasC_sql_result_struct *) myresult,
         dcs_tname[DATA_GRP_NAME],
         dcs_aname[DATA_GRP_NAME]);

        if ((retVal = checkSameMcat (container, collectionName)) < 0) {
	    if (collectionName == NULL)
		collectionName = "NULL";
            elog (NOTICE,
            "svrBulkRegister:container %s and collection %s not from same MCAT",
             container, collectionName);
            return retVal;
        }
#endif  /* FED_MCAT */
	retVal = registerIncontainerDatasetInBulk (catType, 
	 ClientUser->userName, ClientUser->domainName, container, myresult);
      }
      if (retVal >= 0) {
            commit_db2_interaction (MDAS_CAT_COMMIT);
      } else {
          collectionName =  (char *) get_from_result_struct(
           (mdasC_sql_result_struct *) myresult,
           dcs_tname[DATA_GRP_NAME],
           dcs_aname[DATA_GRP_NAME]);

	  if (collectionName != NULL) {
             elog (NOTICE,
            "svrBulkRegister:register error involving collection %s, stat=%d",
             collectionName, retVal);
	  }
	  commit_db2_interaction (MDAS_CAT_ROLLBACK);
      }
#endif
    } else {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }

        /* Do a client call to the remote host */

	/* XXXXX use continuation_index to store the noCheckFlag.
	 * May want to put it as input parameter */
	myresult->continuation_index = noCheckFlag;
        retVal = srbBulkRegister (hostTabPtr->conn, catType, container, 
	 myresult);

        if (retVal < 0) {
            elog (NOTICE, "svrBulkRegister: srbBulkRegister error: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }

    return retVal;
}

int
svrBulkLoad (int catType, struct varlena *vBloadFullPath,
struct varlena *packedResult)
{
    char *bloadFullPath;
    mdasC_sql_result_struct *myresult;
    char *domainName, *userName;
    int retVal;
    char bloadFileName[MAX_TOKEN];
    char bloadCollName[MAX_TOKEN];
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead;
    struct hostElement  *hostTabPtr;
    int remoteFlag;
    char *tmpResLoc, *portNum;
    int openFlag = O_WRONLY;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    bloadFullPath = varToStr (vBloadFullPath);

    if ((retVal = mySplitPath (bloadFullPath, bloadCollName,
     bloadFileName)) < 0) {
        elog(NOTICE,
         "svrBulkLoad: mySplitPath error, status = %d",
          retVal);
        return retVal;
    }

    retVal = unpackMsg (VAROUTDATA (packedResult), (char **) &myresult,
     sqlResultStruct_PF, NULL);

    if (retVal < 0) {
        elog(NOTICE,
         "svrBulkLoad: unpackMsg() of myresult error, status = %d",
          retVal);
        _svrObjUnlink (bloadFileName, bloadCollName);
        return (retVal);
    }

    infoInp.domainName = ClientUser->domainName;
    infoInp.accessMode = openFlagLookUp (openFlag);

    if (infoInp.accessMode == NULL) {
        elog(NOTICE,"svrBulkLoad: openFlag - %x lookup failed",
        O_WRONLY);
	clearSqlResult (myresult);
        _svrObjUnlink (bloadFileName, bloadCollName);
        return OBJ_ERR_OPEN_FLAG;
    }

    infoInp.objID = bloadFileName;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = bloadCollName;
    retVal = mdasGetInfo (&infoInp, &infoOutHead);

    if (retVal < 0) {
	elog(NOTICE,"svrBulkLoad: mdasGetInfo of %s error, status = %d",
	 bloadFullPath, retVal);
        _svrObjUnlink (bloadFileName, bloadCollName);
	clearSqlResult (myresult);
	return retVal;
    }

    /* parseAddr change the content of ResLoc */
    tmpResLoc = strdup (infoOutHead->resourceLoc);
    portNum = parseAddr (tmpResLoc);

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (infoOutHead->resourceLoc, &hostTabPtr);
    portNum = NULL;
#else
    /* parseAddr change the content of ResLoc */
    tmpResLoc = strdup (infoOutHead->resourceLoc);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif

    if (remoteFlag < 0) {
        elog (NOTICE, "svrBulkLoad: Invalid host addr %s", 
	 infoOutHead->resourceLoc);
        _svrObjUnlink (bloadFileName, bloadCollName);
	clearSqlResult (myresult);
        retVal = INP_ERR_HOST_ADDR;
    } else if (remoteFlag == REMOTE_HOST) { /* remote proxy */
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            _svrObjUnlink (bloadFileName, bloadCollName);
            return retVal;
        } else {
	    retVal = srbBulkLoad (hostTabPtr->conn, catType, bloadFullPath,
    	     myresult);
	}
    } else {
	retVal = _svrBulkLoad (catType, myresult, infoOutHead, &infoInp);
	/* only _svrObjUnlink here and not after srbBulkLoad because it 
	 * will be done there.
	 */ 
        _svrObjUnlink (bloadFileName, bloadCollName);
    }

    freeInfoOut (infoOutHead);
    clearSqlResult (myresult);
    return retVal;
}
    
int 
_svrBulkLoad (int catType, mdasC_sql_result_struct *myresult, 
struct mdasInfoOut *infoOutHead, struct mdasInfoInp *infoInp)
{
    char *dataName, *collection, *size, *offset, *pathName, *dataType, *chksum;
    struct mdasResInfo *resInfo, *compResInfo;
    int i, retVal;
    int srcExfInx, destExfInx;
    int srcTypeInx, destTypeInx;
    srb_long_t mySize;
    int nbytes; 
    char *buf = NULL;
    int flagval;
    int isComp;
    char *compPath;
    int srcDesc;
    struct resCache *myResCache;
    
    int numFilesDone = 0;


    /* do it locally */

    /* handcraft some entries in myresult. First, an entry for the physical
     * path */

    myresult->sqlresult[myresult->result_count].tab_name = 
     strdup (dcs_tname[PATH_NAME]);
    myresult->sqlresult[myresult->result_count].att_name = 
     strdup (dcs_aname[PATH_NAME]);

    myresult->sqlresult[myresult->result_count].values = pathName =
     malloc (myresult->row_count * MAX_DATA_SIZE);
    memset (pathName, 0, myresult->row_count * MAX_DATA_SIZE);

    myresult->result_count ++;

    myresult->sqlresult[myresult->result_count].tab_name = 
     strdup (dcs_tname[DATA_TYP_NAME]);
    myresult->sqlresult[myresult->result_count].att_name =
     strdup (dcs_aname[DATA_TYP_NAME]);

    myresult->sqlresult[myresult->result_count].values = dataType =
     malloc (myresult->row_count * MAX_DATA_SIZE);
    memset (dataType, 0, myresult->row_count * MAX_DATA_SIZE);

    myresult->result_count ++;

    dataName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    collection = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    size = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[SIZE], dcs_aname[SIZE]);

    offset = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[OFFSET], dcs_aname[OFFSET]);

    /* client flagval is put in continuation_index for backward compat */
    flagval = myresult->continuation_index;
    if ((flagval & k_FLAG) || (flagval & K_FLAG)) {
        chksum = (char *) get_from_result_struct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
    } else {
	chksum = NULL;
    }

    /* get the resource info */

    if (getResInfo (collection, catType, infoOutHead->resourceName, 
     ClientUser->domainName, &myResCache) < 0) {
        elog (NOTICE,
          "_svrBulkLoad: %s and %s is not a registered resource",
          infoOutHead->resourceName, ClientUser->domainName);
        return OBJ_ERR_RES_NOT_REG;
    }

    resInfo = myResCache->resInfoHead;
    /* open the source file */

    srcTypeInx = destTypeInx = infoOutHead->objTypeInx;

    if (srbObjTypeEntry[srcTypeInx].objType == COMPOUND_TYPE) {
	/* compound obj */
	/* return (SYS_ERR_BLOAD_OF_COMP_RES_NOT_SUPPORTED); */
	isComp = 1;
	srcDesc = _svrObjOpenWithInfo (infoInp, &infoOutHead, O_RDONLY);

        if (srcDesc < 0) {
            elog (NOTICE,
              "_svrBulkLoad: _svrObjOpenWithInfo for %s", infoOutHead->dataPath);
            return srcDesc;
        }
	srcExfInx = srbObjDesc[srcDesc].lowLevelDescInx;
	srcTypeInx = srbObjDesc[srcDesc].objTypeInx;
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

	compPath = malloc (myresult->row_count * MAX_DATA_SIZE);
    } else {
	isComp = 0;
	compPath = NULL;
        srcExfInx = lowLevelOpen (srcTypeInx, infoOutHead->dataPath, 
         infoOutHead->resourceLoc, O_RDONLY);

        if (srcExfInx < 0) {
            elog (NOTICE,
              "_svrBulkLoad: lowLevelOpen for %s", infoOutHead->dataPath);
            return srcExfInx;
        }
    }

    retVal = 0;
    buf = malloc (BUF_SIZE);
    for (i = 0; i < myresult->row_count; i++) {
#ifdef DEBUG_BLOAD
        printf ("%s ", &dataName[i*MAX_DATA_SIZE]);
#endif
	if (isComp == 1) {
	    retVal = genCompPath (&pathName[i*MAX_DATA_SIZE], 
	     &dataName[i*MAX_DATA_SIZE]);
	} else {
            retVal = getFullPathName (resInfo->logDirPathName,
                    resInfo->phyDirPathName,
                    ClientUser->userName, ClientUser->domainName, "", 
		    &dataName[i*MAX_DATA_SIZE], &collection[i*MAX_DATA_SIZE],
		    &pathName[i*MAX_DATA_SIZE]);
	}
        if (retVal < 0) {
            elog(NOTICE,
             "_svrBulkLoad: getFullPathName error for %s/%s",
             &collection[i*MAX_DATA_SIZE], &dataName[i*MAX_DATA_SIZE]);
	    retVal = INP_ERR_FILENAME;
	    break;
        }
	mySize = strtoll (&size[i*MAX_DATA_SIZE], 0, 0);
	if (isComp == 1) {
	    destExfInx = _objCreateWithResInfo (&dataName[i*MAX_DATA_SIZE], 
	      &collection[i*MAX_DATA_SIZE],
              "", mySize, compResInfo, &compPath[i*MAX_DATA_SIZE], 
	      &destTypeInx);
            if (destExfInx < 0) {
                elog (NOTICE, 
		 "_svrBulkLoad: objCreateWithResInfo for %s/%s failed, stat =%d",
                 &collection[i*MAX_DATA_SIZE], 
		 &dataName[i*MAX_DATA_SIZE], destExfInx);
                retVal = destExfInx;
                break;
	    }
	} else {
	    destExfInx = lowLevelCreate (destTypeInx, 
	     infoOutHead->resourceLoc, &pathName[i*MAX_DATA_SIZE],
             StorVaultMode, NO_CHKDIR, -1);

            if (destExfInx < 0) {
                elog (NOTICE, "_svrBulkLoad: Unable to create/open %s",
	         &pathName[i*MAX_DATA_SIZE]);
	        retVal = destExfInx;
	        break;
	    }
        }
        /* open or create is successful */

        numFilesDone ++;

	while (mySize > 0) {
	    int toRead;
	    if (mySize < BUF_SIZE) 
		toRead = mySize;
	    else 
		toRead = BUF_SIZE;
	
            nbytes = _objRead (srcExfInx, srcTypeInx, buf, toRead);
	    /* problem reported by Ma Mei of kek that nbytes may not
	     * equal to toRead if (nbytes == toRead) { */
	    if (nbytes > 0) {
                if (_objWrite (destExfInx, destTypeInx, buf, 
		 nbytes) != nbytes) {
                    elog (NOTICE,
                      "_svrBulkLoad: Bytes written does no match bytes read");
                    retVal = OBJ_ERR_COPY_LEN;
                    break;
		}
            } else {
		elog (NOTICE,
		 "_svrBulkLoad: _objRead error for %s. toRead %d, got %d",
		  &pathName[i*MAX_DATA_SIZE], toRead, nbytes);
		if (nbytes >= 0) 
		    retVal = OBJ_ERR_COPY_LEN;
		else
		    retVal = nbytes;
		break;
	    }
            mySize -= nbytes;
        }
	/* do the chksum verification */
	if (flagval & K_FLAG) {
	    char outChksum[CHKSUM_STR_LEN + 1];
	    _objSeek (destExfInx, destTypeInx, 0, SEEK_SET);
            retVal = _objChksum (destTypeInx, destExfInx, 0, outChksum,
                 0, 0);
	    if (retVal < 0) {
		elog (NOTICE,
		 "_svrBulkLoad: _objChksum error for %s/%s, stat = %s",
		  &collection[i*MAX_DATA_SIZE], &dataName[i*MAX_DATA_SIZE], 
		  retVal);
		return (retVal);
	    } else {
		if (chksum != NULL) {
	            if (atoi (&chksum[i*MAX_DATA_SIZE]) != atoi (outChksum)) {
		        return (OBJ_ERR_BAD_CHKSUM);
		    }
		}
	    }
	}
	_objClose (destTypeInx, destExfInx);
	if (retVal < 0)
	    break;
    }
#ifdef DEBUG_BLOAD
    printf (" pid = %d, %s\n", getpid (), infoOutHead->dataPath);
    fflush (stdout);
#endif
    if (buf != NULL)
	free (buf);
    _objClose (srcTypeInx, srcExfInx);
    if (isComp == 1) {
	delObjDesc (srcDesc);
    }

    if (retVal >= 0) {
	retVal = _svrBulkRegister (catType, infoOutHead->resourceName, 
	 myresult, 1);
	if (retVal < 0) {
            elog (NOTICE, "_svrBulkLoad: _svrBulkRegister error. stat = %d",
	     retVal);
	}
    }

    if (retVal < 0) {
	if (isComp == 0) {
            for (i = 0; i < numFilesDone; i++) {
                _objUnlink (srbObjTypeEntry[destTypeInx].objType,
                 srbObjTypeEntry[destTypeInx].systemType,
                 infoOutHead->resourceLoc, &pathName[i*MAX_DATA_SIZE],
                 NO_CHKDIR);
	    }
	} else {
            for (i = 0; i < numFilesDone; i++) {
                _objUnlink (srbObjTypeEntry[destTypeInx].objType,
                 srbObjTypeEntry[destTypeInx].systemType,
                 compResInfo->phyResLoc, &compPath[i*MAX_DATA_SIZE],
                 NO_CHKDIR);
	    }
        }
    } else if (isComp == 1) {
	/* compound obj */
	for (i = 0; i < numFilesDone; i++) {
	    mySize = strtoll (&size[i*MAX_DATA_SIZE], 0, 0);
	    retVal = _svrRegInternalCompObj (&dataName[i*MAX_DATA_SIZE], 
	      &collection[i*MAX_DATA_SIZE], 0, NO_INT_SEGMENTATION,
              compResInfo->phyResName, &compPath[i*MAX_DATA_SIZE],
	      mySize, 0, 
	      NEW_INT_REPL_NUM, NO_INT_SEGMENTATION,
              destTypeInx, compResInfo->phyResLoc,
              ClientUser->userName, ClientUser->domainName);
	    if (retVal < 0) {
		elog (NOTICE, 
		  "_svrBulkLoad: _svrRegInternalCompObj of %s/&s failed. stat =%d",
		  &collection[i*MAX_DATA_SIZE], &dataName[i*MAX_DATA_SIZE], 
		  retVal);
		break;
	    }
	}
	if (retVal < 0 && i < numFilesDone) {
	    int numIntReg, status;

	    numIntReg = i;
	    for (i = numIntReg; i < numFilesDone; i++) {
               status = _objUnlink (srbObjTypeEntry[destTypeInx].objType,
                 srbObjTypeEntry[destTypeInx].systemType,
                 compResInfo->phyResLoc, &compPath[i*MAX_DATA_SIZE],
                 NO_CHKDIR);
                if (status < 0) {
                    elog (NOTICE,
                     "_svrBulkLoad: _objUnlink error for %s, stat = %d",
                      &compPath[i*MAX_DATA_SIZE],
                      status);
                }
		status = _svrModifyDataset (MDAS_CATALOG, 
		  &dataName[i*MAX_DATA_SIZE],
                  &collection[i*MAX_DATA_SIZE], "", "",
                   NULL, NULL, D_DELETE_ONE);
		if (status < 0) {
		    elog (NOTICE,
		     "_svrBulkLoad: ModifyDataset delete error for %s/%s,stat= %d",
		      &collection[i*MAX_DATA_SIZE], &dataName[i*MAX_DATA_SIZE],
		      status);
		}
	    }
	}
    }
    if (compPath != NULL)
        free (compPath);
	
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
svrCreateCollect (int catType, struct varlena *vParentCollect,
struct varlena *vNewCollect)
{
    char *parentCollect;
    char *newCollect;
    int retVal;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    parentCollect = varToStr (vParentCollect);
    newCollect = varToStr (vNewCollect);

    retVal = _svrCreateCollect (catType, parentCollect, newCollect);

    free (parentCollect);
    free (newCollect);

    return retVal;
}

int 
_svrCreateCollect (int catType, char *parentCollect, char *newCollect)
{
    int retVal;
    char *userName;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Do a make_new_collection */

    remoteFlag = getMdasEnabledHost (parentCollect, &hostTabPtr);

    if (remoteFlag < 0) {
        return (remoteFlag);
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        userName = ClientUser->userName;
	/**** RAJA February 20, 2004 changed to make all collections to be made by path *****
        retVal = make_new_collection (catType, parentCollect,
          newCollect, userName, ClientUser->domainName);
	  **** RAJA February 20, 2004 changed to make all collections to be made by path *****/
        retVal = make_new_collection_path (catType, parentCollect,
          newCollect, userName, ClientUser->domainName);
	if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        retVal = remoteCreateCollect (catType, parentCollect,
          newCollect, hostTabPtr);
    }
    return (retVal);
}

struct varlena *
svrListCollect (int catType, struct varlena *vCollectionName,
struct varlena *vFlag, int rowsWanted)
{
    char *collectionName;
    char *flag;
    struct varlena *retVal;
    int status;
    char *userName;
    mdasC_sql_result_struct  *myresult = NULL;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    collectionName = varToStr (vCollectionName);
    flag = varToStr (vFlag);


    /* Do a get_collections */

    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
        userName = ClientUser->userName;
#ifdef SRB_MDAS
        status = get_collections (catType, collectionName,
          flag, &myresult, rowsWanted);
#endif
    } else {
        status = remoteListCollect (catType, collectionName,
          flag, &myresult, rowsWanted, hostTabPtr);
    }

    free (collectionName);
    free (flag);

    if (status == 0) {
    	retVal = sqlResultToVar (myresult, MULTI_ROW);
    	freeSqlResult (myresult);

    } else {
    	retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    	VARSIZE(retVal) = VAROUTHDRSZ;
    	VAROUTSTAT(retVal) = htonl (status);
	if (myresult != NULL)
	    free (myresult);
    }

    return retVal;
}

int
svrModifyCollect (int catType, struct varlena *vCollectionName,
struct varlena *vDataValue1, struct varlena *vDataValue2,
struct varlena *vDataValue3, int retractionType)
{
    char *collectionName;
    char *dataValue1;
    char *dataValue2;
    char *dataValue3;
    int retVal;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    collectionName = varToStr (vCollectionName);
    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);
    dataValue3 = varToStr (vDataValue3);


    retVal = _svrModifyCollect (catType, collectionName, dataValue1, 
    dataValue2, dataValue3, retractionType);

    free (collectionName);
    free (dataValue1);
    free (dataValue2);
    free (dataValue3);

    return retVal;
}

int
_svrModifyCollect (int catType, char *collectionName, char *dataValue1,
char *dataValue2, char *dataValue3, int retractionType)
{
    int retVal;
    char *userName;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Do a modify_collection_info */

    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        userName = ClientUser->userName;
        retVal = modify_collection_info (catType, userName, collectionName,
          dataValue1, dataValue2, dataValue3, retractionType, 
	    ClientUser->domainName);
	if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        retVal = remoteModifyCollect (catType, collectionName,
          dataValue1, dataValue2, dataValue3, retractionType, hostTabPtr);
    }

    return retVal;
}

int
svrRegisterUserGrp (int catType, struct varlena *vUserGrpName,
struct varlena *vUserGrpPasswd, struct varlena *vUserGrpType,
struct varlena *vUserGrpAddress, struct varlena *vUserGrpPhone,
struct varlena *vUserGrpEmail)
{
    char *userGrpName;
    char *userGrpPasswd;
    char *userGrpType;
    char *userGrpAddress;
    char *userGrpPhone;
    char *userGrpEmail;
    int status;
    char *userName;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    userGrpName = varToStr (vUserGrpName);
    userGrpPasswd = varToStr (vUserGrpPasswd);
    userGrpType = varToStr (vUserGrpType);
    userGrpAddress = varToStr (vUserGrpAddress);
    userGrpPhone = varToStr (vUserGrpPhone);
    userGrpEmail = varToStr (vUserGrpEmail);

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        userName = ClientUser->userName;
        status = register_user_group (catType, userGrpName, userGrpPasswd,
        userGrpType, userGrpAddress, userGrpPhone, 
	userGrpEmail, userName, ClientUser->userAuth, ClientUser->domainName);

	if (status < 0) {
            elog (NOTICE,
              "svrRegisterUserGrp: register_user_group failed status = %d",
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

        status = srbRegisterUserGrp (hostTabPtr->conn, catType, userGrpName,
          userGrpPasswd, userGrpType, userGrpAddress, userGrpPhone,
	  userGrpEmail);

        if (status < 0) {       /* Failure */
            elog (NOTICE, "svrRegisterUserGrp failed: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }

    free (userGrpName);
    free (userGrpPasswd);
    free (userGrpType);
    free (userGrpAddress);
    free (userGrpPhone);
    free (userGrpEmail);

    return (status);
}

int
svrRegisterUser (int catType, struct varlena *vUserName, 
struct varlena *vUserDomain,
struct varlena *vUserPasswd, struct varlena *vUserType,
struct varlena *vUserAddress, struct varlena *vUserPhone,
struct varlena *vUserEmail)
{
    char *userName;
    char *userDomain;
    char *userPasswd;
    char *userType;
    char *userAddress;
    char *userPhone;
    char *userEmail;
    int status;
    char *srbUserName;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    userName = varToStr (vUserName);
    userDomain = varToStr (vUserDomain);
    userPasswd = varToStr (vUserPasswd);
    userType = varToStr (vUserType);
    userAddress = varToStr (vUserAddress);
    userPhone = varToStr (vUserPhone);
    userEmail = varToStr (vUserEmail);

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        srbUserName = ClientUser->userName;
        status = register_user_info (catType, userName, userPasswd, userDomain, 
	userType, userAddress, userPhone,
        userEmail, srbUserName, ClientUser->userAuth, ClientUser->domainName);

        if (status < 0) {
            elog (NOTICE,
              "svrRegisterUser: register_user_info failed status = %d",
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

        status = srbRegisterUser (hostTabPtr->conn, catType, userName,
          userDomain, userPasswd, userType, userAddress, userPhone,
          userEmail);

        if (status < 0) {       /* Failure */
            elog (NOTICE, "svrRegisterUser failed: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }

    free (userName);
    free (userDomain);
    free (userPasswd);
    free (userType);
    free (userAddress);
    free (userPhone);
    free (userEmail);

    return (status);
}

int
svrModifyUser (int catType, struct varlena *vDataValue1,
struct varlena *vDataValue2, int retractionType)
{
    char *dataValue1;
    char *dataValue2;
    int status;
    char *userName;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);

    /* XXXXXX potential problem. Don't know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        userName = ClientUser->userName;
        status = modify_user_info (catType, userName, dataValue1, 
	  dataValue2, retractionType, ClientUser->userAuth, 
	    ClientUser->domainName);

        if (status < 0) {
            elog (NOTICE,
              "svrModifyUser: modify_user_info failed status = %d",
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

        status = srbModifyUser (hostTabPtr->conn, catType, dataValue1,
          dataValue2, retractionType);

        if (status < 0) {       /* Failure */
            elog (NOTICE, "svrModifyUser failed: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }

    free (dataValue1);
    free (dataValue2);

    return (status);
}

int
svrObjAudit (int catType, struct varlena *vUserName, struct varlena *vObjID,
struct varlena *vCollectionName, struct varlena *vDataPath, 
struct varlena *vResourceName, struct varlena *vAccessMode, 
struct varlena *vComment, int success, struct varlena *vDomainName)
{
    char *userName;
    char *objID;
    char *collectionName;
    char *dataPath;
    char *resourceName;
    char *accessMode;
    char *comment;
    char *domainName;
    int status;

    userName = varToStr (vUserName);
    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    dataPath = varToStr (vDataPath);
    resourceName = varToStr (vResourceName);
    accessMode = varToStr (vAccessMode);
    comment = varToStr (vComment);
    domainName = varToStr (vDomainName);

    status = _svrObjAudit (catType, userName, objID, collectionName,  
    dataPath,  resourceName, accessMode,  comment, success,  domainName);

    return status;
}
    
int
_svrObjAudit (int catType, char *userName, char *objID,
char *collectionName,  char *dataPath,  char *resourceName, 
char *accessMode,  char *comment, int success,  char *domainName)
{
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (set_audit_trail (GET_AUDIT_TRAIL_SETTING) != AUDIT_TRAIL_ON)
	return -1;

    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        status = auditDatasetAccess (catType, userName, objID,
        collectionName, dataPath, resourceName, accessMode,
        comment, success, domainName);

        if (status < 0) {
            elog (NOTICE,
              "_svrObjAudit: auditDatasetAccess failed status = %d",
              status);
        }
#endif
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            return status;
        }

        /* Do a client call to the remote host */

        status = srbObjAudit (hostTabPtr->conn, catType, userName,
	objID, collectionName, dataPath, resourceName, accessMode,
        comment, success, domainName);

        if (status < 0) {       /* Failure */
            elog (NOTICE, "_svrObjAudit: srbObjAudit failed: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }
    return (status);
}

/* svrGetPrivUsers - Entry point for the srbGetPrivUsers SID.
 *
 *
 * Input : int catalog - The catalog type. e.g., MDAS_CATALOG
 *	   int rowsWanted - No. or rows to be returned.
 *
 * Output : success - Returns the content in struct varlena
 *          failure - Returns NULL string.
 */

struct varlena *
svrGetPrivUsers (int catalog, int rowsWanted)
{
    struct varlena *retval;
    int status;
    mdasC_sql_result_struct  *myresult = NULL;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
    	status = getSRBUserInfo (catalog, &myresult, rowsWanted);
#endif
    } else {
        status = remoteGetPrivUsers (catalog, &myresult, rowsWanted, 
	 hostTabPtr);
    }

    if (status == 0) {
        retval = sqlResultToVar (myresult, MULTI_ROW);
        freeSqlResult (myresult);
    } else {
	elog (NOTICE, "svrGetPrivUsers: getSRBUserInfo failed. Status = %d",
	  status);
    	retval = (struct varlena *)malloc(VAROUTHDRSZ);
    	VARSIZE(retval) = VAROUTHDRSZ;
    	VAROUTSTAT(retval) = htonl (status);
        if (myresult != NULL)
            free (myresult);
    }

    return retval;
}

/* svrGetMoreRows - Entry point for the srbGetMoreRows SID.
 *
 *
 * Input : int catalog - The catalog type. e.g., MDAS_CATALOG
 *         int contDesc - The continuation descriptor. This is a non negative
 *         integer returned from a srbGetDatasetInfo, srbGetDataDirInfo,
 *         srbListCollect or srbGetPrivUsers call as
 *         myresult->continuation_index.
 *	   int rowsWanted - No. or rows to be returned.
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
 *	   int rowsWanted - No. or rows to be returned.
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

/* svrIssueTicket - Entry point for the srbIssueTicket SID..
 *
 *
 * Input : struct varlena *vObjID - The object ID
 *         struct varlena *vCollectionName - The collection name
 *	   struct varlena *vCollectionFlag - The collect flag if vCollectionName
 *		is non NULL. "R" - the ticket is for all dataset and
 *		sub-collection recursively. "D" - the ticket is for the
 *		datasets directly beneath the colloection.
 *	   struct varlena *vBeginTime - The beginning time when the ticket
 *		becomes effective. A NULL means no time limit.
 *         struct varlena *vEndTime - The ending time of the ticket.
 *	   int accessCnt - The number of time the ticket can be used to
 *		access the dataset.
 *	   struct varlena *vTicketUser - The user/userGroup that will use the
 *		ticket. Multiply users can be specified with the following 
 *		format:
 *			user1@domain1&user2@domain2 ....
 *              If it is NULL, => all users.
 *	   
 * Output : Returns the ticket - success, a negative integer  - failure
 */

struct varlena *
svrIssueTicket (struct varlena *vObjID,
struct varlena *vCollectionName, struct varlena *vCollectionFlag,
struct varlena *vBeginTime, struct varlena *vEndTime,
int accessCnt, struct varlena *vTicketUser)
{
    char *objID;
    char *collectionName;
    char *collectionFlag;
    char *beginTime;
    char *endTime;
    char *ticketUser;
    struct varlena *retVal;
    int status;
    char *ticket;	/* The returned ticket */
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
	status = ILLEGAL_OPR_TICKET_USER;
	retVal = (struct varlena *)malloc(VAROUTHDRSZ);
	VARSIZE(retVal) = VAROUTHDRSZ;
	VAROUTSTAT(retVal) = htonl (status);
	return (retVal);
    }

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    collectionFlag = varToStr (vCollectionFlag);
    beginTime = varToStr (vBeginTime);
    endTime = varToStr (vEndTime);
    ticketUser = varToStr (vTicketUser);


    /* Do a get_collections */

    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        status = issueTicket (objID, collectionName,
          collectionFlag, beginTime, endTime, accessCnt, ticketUser, &ticket, 
	    ClientUser->userName, ClientUser->domainName);
	if (status == 0)
	    commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        status = remoteIssueTicket (objID, collectionName,
          collectionFlag, beginTime, endTime, accessCnt, ticketUser, &ticket,
	  hostTabPtr);
    }

    free (objID);

    free (collectionName);

    free (collectionFlag);

    free (beginTime);

    free (endTime);

    free (ticketUser);

    if (status == 0) {
        retVal = malloc (strlen (ticket) + VAROUTHDRSZ + 1);
	VARSIZE(retVal) = strlen (ticket) + VAROUTHDRSZ + 1;
	strcpy ((char *) VAROUTDATA (retVal), ticket);
        VAROUTSTAT(retVal) = 0;
        free (ticket);
    	return retVal;

    }

errRet:
    retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    VARSIZE(retVal) = VAROUTHDRSZ;
    VAROUTSTAT(retVal) = htonl (status);
    return (retVal);
}

/* svrRemoveTicket - Entry point for the srbRemoveTicket SID.
 *
 *
 * Input : struct varlena *vTicket - The ticket to remove.
 *
 * Output : Returns 0 - success, negative  - failure
 */

int
svrRemoveTicketWithZone (struct varlena *vMcatZone, struct varlena *vTicket)
{
    char *ticket, *mcatZone;
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    ticket = varToStr (vTicket);
    mcatZone = varToStr (vMcatZone);
    
    remoteFlag = getMdasEnabledHost (mcatZone, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        status = removeTicket (ticket,
          ClientUser->userName, ClientUser->domainName);

        if (status == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        status = remoteRemoveTicket (ticket, hostTabPtr);
    }

    free (mcatZone);
    free (ticket);

    return (status);
}

int
svrRemoveTicket (struct varlena *vTicket)
{
    char *ticket;
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    ticket = varToStr (vTicket);
    
    /* XXXXX problem here. Not sure which MCAT */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        status = removeTicket (ticket,
          ClientUser->userName, ClientUser->domainName);

        if (status == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        status = remoteRemoveTicket (ticket, hostTabPtr);
    }

    return (status);
}

/* interPathCheck - Check whether we need to interpret the pathName. We
 * neeed to if we find a '$' in the condition clause (start with a '&').
 *
 * Input - char *objID - The objectID with condition clauses.
 *
 * Output - 0 - no interpretation needed.
 *	    1 - interpretation is needed.
 */

int
interPathCheck (char *objID) 
{
    int i, j;
    int inLen;

    inLen = strlen (objID);
    if (inLen <= 0)
        return 0;

    for (j = 0; j < inLen; j++) {
	if (objID[j] == '&') 
	    return (1);
    }

    return (0);
}

/* svrRegisterContainer - Entry point for the svrRegisterContainer SID.
 *
 *
 * Input : int catType - The catalog type.
 *	   struct varlena *vContainerName - The container name
 *	   struct varlena  *vLogResName - The Logical resource name.
 *         srb_long_t *containerSize - The size of the container. 
 *
 *
 * Output : Returns 0 - success, negative  - failure
 */

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
    struct mdasInfoOut *tmpInfoOut = NULL;
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


/* proxyFunction - Handle proxy function operation.
 *
 * 
 */

struct varlena *
proxyFunction (int inputInt1, int inputInt2, int inputInt3, int inputInt4,
char *inputStr1, char *inputStr2, char *inputStr3, char *inputStr4,
struct varlena *vInputStr1, struct varlena *vInputStr2, 
struct varlena *vInputStr3)
{

  /** inputStr1 =  functionName
      inputStr2 =  hostName
      inputStr4 =  clientAddress
      inputStr3 = stringArgument

  **/

    char *proxyAddr;
    struct hostElement  *hostTabPtr;
    int status;
    struct varlena *retval;
    int lsock = -1;
    int portalFlag, remoteFlag;
    int pid;



    proxyAddr = inputStr3;

    if (proxyAddr != NULL) {
#ifdef FED_MCAT
        remoteFlag = chkHostAndZone (proxyAddr, &hostTabPtr);
#else
	remoteFlag = chkHost (proxyAddr, NULL, &hostTabPtr);
#endif
    	if (remoteFlag < 0) {
            elog (NOTICE, "proxyFunction: Invalid host addr %s", proxyAddr);
	    retval = genIntRet (INP_ERR_HOST_ADDR);
             return retval;             
        } else if (remoteFlag == REMOTE_HOST) {	/* remote proxy */
	    status = remoteProxyOpr (hostTabPtr, OPR_FUNCTION, 
	     inputInt1, inputInt2, inputInt3, inputInt4,
	      inputStr1, inputStr2, inputStr3, inputStr4,
	       NULL, NULL, NULL, NULL);
    	    retval = genIntRet (status);
    	    return (retval);
	}
    }

    /* local proxy opr */

    portalFlag = inputInt1;
    if (portalFlag > 0) {
      lsock = portalConnect (inputInt2, inputStr4);
      if (lsock < 0) {
	elog (NOTICE, 
	      "proxyFunction: portalConnect() failed, status = %d", lsock);
	exit(1);
      }
    }
    status = execFunction (inputStr1, inputStr2, lsock, portalFlag);
    retval = genIntRet (status);
    return (retval);
}

#ifdef LOCK_OBJ

/* svrObjLock - Entry point for the srbObjLock SID.
 *
 *
 * Input : struct varlena *vObjID - The object ID
 *         struct varlena *vCollectionName - The collection name
 *	   lockCmd - F_SETLKW (see fnctl).
 *	   lockType - F_RDLCK, F_WRLCK or F_UNLCK.
 *	   lockFd - The fd of the lock. Valid only for F_UNLCK.
 *
 * Output : Returns the Object descriptor - success, negative  - failure
 */

int
svrObjLock (struct varlena *vObjID, struct varlena *vCollectionName,
int lockCmd, int lockType, int lockFd)
{
    char *objID, *collectionName;
    int retVal;

    if (ProxyUser->privUserFlag <= 0) {
	elog (NOTICE, "svrObjLock: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }

    lockCmd = srbToLocalLockCmd (lockCmd);
    if (lockCmd != F_SETLKW) { 
	elog (NOTICE, "svrObjLock: Illegal Lock cmd: %d", lockCmd);
	return (OBJ_ERR_LOCK_CMD);
    }
    lockType = srbToLocalLockCmd (lockType);

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);

    retVal = _svrObjLock (objID, collectionName, lockCmd, lockType, lockFd);

    free (objID);
    free (collectionName);

    return (retVal);
}

int
_svrObjLock (char *objID, char *collectionName, int lockCmd, int lockType, 
int lockFd)
{
    int status;
    int myLockFd;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
        myLockFd = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
	flock_t myLock;
    	char *lockPath;

    	if ((lockPath = getLockPath (objID, collectionName)) == NULL) {
	    elog (NOTICE, "_svrObjLock: getLockPath error for %s:%s",
	     collectionName, objID);
	    return (INP_ERR_FILENAME);
    	}
    	if (lockType != F_UNLCK) {
	    myLockFd = open (lockPath, O_RDWR | O_CREAT, 0644);
#ifdef LOCK_DEBUG
	    elog(NOTICE, "_svrObjLock:open %d", myLockFd);
#endif
	    if (myLockFd < 0) {
	    	elog(NOTICE, "_svrObjLock: Unable to open lockPath:%s", 
		 lockPath);
		free (lockPath);
	    	return (OBJ_ERR_LOCKFILE_OPEN);
	    }
    	} else {
	    myLockFd = lockFd;
	}

	/* now try to lock the file */
	myLock.l_type = lockType;
	myLock.l_whence = SEEK_SET;
    	myLock.l_start = 0;
    	myLock.l_len = 0;
	status = fcntl (myLockFd, lockCmd, &myLock);
        if (status < 0) {
            elog(NOTICE, "_svrObjLock: Unable to lock lockPath:%s", 
             lockPath);
	    free (lockPath);
            return (OBJ_ERR_LOCKFILE_LOCK);
        }
        if (lockType == F_UNLCK) {
            close (myLockFd);
#ifdef LOCK_DEBUG
	    elog(NOTICE, "_svrObjLock:close %d", myLockFd);
#endif
	}
	free (lockPath);
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            return status;
        }
        /* Do a client call to the remote host */
	myLockFd = srbObjLock (hostTabPtr->conn, objID, collectionName, 
	 lockCmd, lockType, lockFd);
    }
    return (myLockFd);
}

#endif	/* LOCK_OBJ */

/* svrDataCopy - Copy a dataset from souurce to destination.
 * If size == SOURCE_EMPTY, just create the dest dataset but
 * no copying. A size == -1 mean don't know the size and copying
 * will be carried out.
 */
srb_long_t *
svrDataCopy (int destSrcInx, struct varlena *vDestPath, 
struct varlena *vDestResLoc, int srcTypeInx, 
struct varlena *vSrcPath, struct varlena *vSrcResLoc, srb_long_t *size,
int zoneFlag)
{
    char *destPath, *destResLoc;
    char *srcPath, *srcResLoc;
    srb_long_t * retVal;
srb_long_t mySize;

    retVal = (srb_long_t *) malloc (2*SIZEOF32);

    if (ProxyUser->privUserFlag <= 0) {
        elog (NOTICE, "svrDataCopy: User has no privilege for this operation");
	*retVal = AUTH_ERR_PROXY_NOPRIV;
        return (retVal);
    }

    destPath = varToStr (vDestPath);
    destResLoc = varToStr (vDestResLoc);
    srcPath = varToStr (vSrcPath);
    srcResLoc = varToStr (vSrcResLoc);

    mySize = *size;
    *retVal = _svrDataCopy (destSrcInx, destPath, destResLoc, 
     srcTypeInx, srcPath, srcResLoc, mySize, zoneFlag);

    free (destPath);
    free (destResLoc);
    free (srcPath);
    free (srcResLoc);

    return (retVal);
}

/* _svrDataCopy - Copy a dataset from souurce to destination. 
 * If size == SOURCE_EMPTY, just create the dest dataset but 
 * no copying. A size == -1 mean don't know the size and copying 
 * will be carried out.
 */
srb_long_t
_svrDataCopy (int destTypeInx, char *destPath, char *destResLoc,
int srcTypeInx, char *srcPath, char *srcResLoc, srb_long_t size,
int zoneFlag)
{
    srb_long_t retVal = 0;
    int destType, srcType;
    char *portNum;
    struct hostElement  *srcHostTabPtr, *destHostTabPtr, *myHostTabPtr;
    int srcRemoteFlag, destRemoteFlag;
    char *tmpResLoc;

#ifndef FED_MCAT
    zoneFlag = ALL_LOCAL_ZONE;
#endif

    destType = srbObjTypeEntry[destTypeInx].objType;
    srcType = srbObjTypeEntry[srcTypeInx].objType;

#ifdef FED_MCAT
    destRemoteFlag = chkHostAndZone (destResLoc, &destHostTabPtr);
    portNum = NULL;
#else
    /* parseAddr change the content of ResLoc */
    tmpResLoc = strdup (destResLoc);
    portNum = parseAddr (tmpResLoc);
    destRemoteFlag = chkHost (tmpResLoc, portNum, &destHostTabPtr);
    free (tmpResLoc);
#endif
    if (destRemoteFlag < 0) {
        elog (NOTICE, "_svrDataCopy: Invalid dest host addr %s", destResLoc);
        return INP_ERR_HOST_ADDR;               /* failure */
    }

#ifdef FED_MCAT
    srcRemoteFlag = chkHostAndZone (srcResLoc, &srcHostTabPtr);
    portNum = NULL;
#else
    /* parseAddr change the content of ResLoc */
    tmpResLoc = strdup (srcResLoc);
    portNum = parseAddr (tmpResLoc);
    srcRemoteFlag = chkHost (tmpResLoc, portNum, &srcHostTabPtr);
    free (tmpResLoc);
#endif
    if (srcRemoteFlag < 0) {
        elog (NOTICE, "_svrDataCopy: Invalid src host addr %s", srcResLoc);
        return INP_ERR_HOST_ADDR;               /* failure */
    }

#ifdef PARA_OPR
    if (destType == FILE_TYPE && 
     (srcType == FILE_TYPE && srbObjTypeEntry[srcTypeInx].systemType == HPSS_FT)
      && size != SOURCE_EMPTY) {
	/* see if we need to do it remotely */

    	if (srcRemoteFlag == REMOTE_HOST) {
	    if ((zoneFlag == ALL_LOCAL_ZONE || zoneFlag == SRC_LOCAL_ZONE) ||
	     zoneFlag == ALL_SAME_FOREIGN_ZONE) { 
                if ((retVal = remoteConnect (srcHostTabPtr)) < 0) {
                    return retVal;
                }
	        retVal = _srbDataCopy (srcHostTabPtr->conn, destTypeInx, 
		 destPath, destResLoc, srcTypeInx, srcPath, srcResLoc, size, 
		 zoneFlag);
	    } else {
        	retVal = _svrDataCopyS (destTypeInx, destPath, destResLoc,
         	 srcTypeInx, srcPath, srcResLoc, size);
	    }
 	} else {	/* A local call */
            retVal = paraHPSStoFileCopy (destPath, destResLoc, 
	     srbObjTypeEntry[destTypeInx].systemType, srcPath, srcResLoc,
	      srbObjTypeEntry[srcTypeInx].systemType, size);
	}
    } else if ((destType == FILE_TYPE && 
     srbObjTypeEntry[destTypeInx].systemType == HPSS_FT) &&
     srcType == FILE_TYPE && size != SOURCE_EMPTY) {
        /* see if we need to do it remotely */

        if (destRemoteFlag == REMOTE_HOST) {
            if ((zoneFlag == ALL_LOCAL_ZONE || zoneFlag == DEST_LOCAL_ZONE) ||
             zoneFlag == ALL_SAME_FOREIGN_ZONE) {
                if ((retVal = remoteConnect (destHostTabPtr)) < 0) {
                    return retVal;
                }
	        retVal = _srbDataCopy (destHostTabPtr->conn, destTypeInx, 
		 destPath, destResLoc, srcTypeInx, srcPath, srcResLoc, size, 
		 zoneFlag);
            } else {
                retVal = _svrDataCopyS (destTypeInx, destPath, destResLoc,
                 srcTypeInx, srcPath, srcResLoc, size);
            }
        } else {        /* A local call */

            retVal = paraFiletoHPSSCopy (destPath, destResLoc, 
             srbObjTypeEntry[destTypeInx].systemType, srcPath, srcResLoc, 
              srbObjTypeEntry[srcTypeInx].systemType, size);
	}
    } else if (size <= SMALL_FILE_SIZE || 
     (srcRemoteFlag == LOCAL_HOST && destRemoteFlag == LOCAL_HOST)) {
	/* small file or everything are local, just do it locally */
        retVal = _svrDataCopyS (destTypeInx, destPath, destResLoc,
         srcTypeInx, srcPath, srcResLoc, size);
    } else if ((srcRemoteFlag == LOCAL_HOST || destRemoteFlag == LOCAL_HOST) &&
     size <= SizePerThread) {
        retVal = _svrDataCopyS (destTypeInx, destPath, destResLoc,
         srcTypeInx, srcPath, srcResLoc, size);
    } else if (zoneFlag == ALL_DIFF_FOREIGN_ZONE && 
     ClientUser->authSchemeInx != AUTH_ENCRYPT1_INX){
        retVal = _svrDataCopyS (destTypeInx, destPath, destResLoc,
         srcTypeInx, srcPath, srcResLoc, size);
    } else if (srcRemoteFlag == REMOTE_HOST && destRemoteFlag == REMOTE_HOST) {
	/* everything are remote and file size is > SMALL_FILE_SIZE */
	/* do a little differently if GSI_DELEGATE_INX. */
        if (zoneFlag == ALL_LOCAL_ZONE || zoneFlag == SRC_LOCAL_ZONE) {
            myHostTabPtr = srcHostTabPtr;
	} else {
	    myHostTabPtr = destHostTabPtr;
	}
        if ((retVal = remoteConnect (myHostTabPtr)) < 0) {
            return retVal;
        }
        retVal = _srbDataCopy (myHostTabPtr->conn, destTypeInx, destPath,
         destResLoc, srcTypeInx, srcPath, srcResLoc, size, zoneFlag);
    } else if (srcType != FILE_TYPE || destType != FILE_TYPE) {
	/* can only do parallel I/O from FILE_TYPE to FILE_TYPE */
	retVal = _svrDataCopyS (destTypeInx, destPath, destResLoc,
         srcTypeInx, srcPath, srcResLoc, size);
    } else if (srcRemoteFlag == REMOTE_HOST) {	/* one of the loc is remote */
	/* For now, do only the one with src locally */
        retVal = _svrDataCopySrbToLocalP (destTypeInx, destPath, destResLoc, 
	 srcTypeInx, srcPath, srcResLoc, size);
    } else {	
        retVal = _svrDataCopyLocalToSrbP (destTypeInx, destPath, destResLoc,
         srcTypeInx, srcPath, srcResLoc, size);
    }
#else
    if (size <= SMALL_FILE_SIZE || srcRemoteFlag == LOCAL_HOST || 
     destRemoteFlag == LOCAL_HOST ||
      zoneFlag == ALL_DIFF_FOREIGN_ZONE) {
        retVal = _svrDataCopyS (destTypeInx, destPath, destResLoc,
         srcTypeInx, srcPath, srcResLoc, size);
    } else {
        if (zoneFlag == ALL_LOCAL_ZONE || zoneFlag == SRC_LOCAL_ZONE) {
            myHostTabPtr = srcHostTabPtr;
        } else {
            myHostTabPtr = destHostTabPtr;
        }
        if ((retVal = remoteConnect (myHostTabPtr)) < 0) {
            return retVal;
        }
        retVal = _srbDataCopy (myHostTabPtr->conn, destTypeInx, destPath,
         destResLoc, srcTypeInx, srcPath, srcResLoc, size, zoneFlag);
    }
#endif  /* PARA_OPR */
    if (retVal == AUTH_ERR_CLIENT_SVR_DIFF_ZONE) {
	/* cross zone copy. Try serial copy instead */
	retVal = _svrDataCopyS (destTypeInx, destPath, destResLoc,
         srcTypeInx, srcPath, srcResLoc, size);
    }

    return retVal;
}

#ifdef PARA_OPR
srb_long_t
_svrDataCopySrbToLocalP (int destTypeInx, char *destPath, char *destResLoc,
int srcTypeInx, char *srcPath, char *srcResLoc, srb_long_t size)
{
    srb_long_t retval;

    int clPort;
    char *clHostAddr;
    int lsock;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    struct sockaddr_in sin;
    srb_long_t status;
    struct portalInput myInput;
    pthread_t portalManagerThread;
    char sb[MAX_TOKEN];
    struct hostent *hostEnt;
    struct in_addr myIn;
    struct relayHeader myHeader;
    struct hostElement *srcHostTabPtr;
    int srcRemoteFlag;
    char *portNum;
    char *tmpResLoc;
    int retryCnt = 0;

    if ((lsock = portalCreate ()) < 0) {
	elog (NOTICE, 
	  "_svrDataCopySrbToLocalP: portalCreate() failed: errno=%d", errno);
        return lsock;
    }

    listen(lsock, SOMAXCONN);

    length = sizeof (sin);
    if (getsockname (lsock, (struct sockaddr *) &sin, &length)) {
	elog (NOTICE, 
         "_svrDataCopyLocalToSrbP: getsockname() failed: errno=%d", errno);
        return CLI_ERR_SOCKET;
    }
    clPort = ntohs (sin.sin_port);
    clPort = genPortInp (clPort);
    /* clHostAddr = inet_ntoa (sin.sin_addr);   XXXX this give 0 addr */
    clHostAddr = getLocalHostNetAddr ();

    if (clHostAddr == NULL) {
        elog (NOTICE,
         "_svrDataCopySrbToLocalP: getLocalHostNetAddr failed");
        return SYS_ERR_HOST_ADDR;
    }

    ParaCreateFlag = 0;
    myInput.sock = lsock;
    myInput.cookie = getCookieFromPortInp (clPort);
    myInput.locFilePath = destPath;
    myInput.locTypeInx = destTypeInx;
    myInput.locResLoc = destResLoc;
    myInput.operation = GET_OPR;
    myInput.transferHandler = (void *) svrPortalTransfer;
    myInput.size = size;
    myInput.numThreads = 0;
    myInput.threadInx = 0;
    myInput.bytesMoved = 0;
    myInput.status = 0;
    myInput.errorMessage = NULL;
    pthread_mutex_init(&myInput.varMutex, pthread_mutexattr_default);
    pthread_mutex_init(&myInput.condMutex, pthread_mutexattr_default);
    pthread_cond_init (&myInput.cond, pthread_condattr_default);



   status = pthread_create (&portalManagerThread, pthread_attr_default,
                  (void *(*)(void *)) portalManager,
                  (void *) &myInput);

    if (status < 0) {
	elog (NOTICE, 
         "_svrDataCopyLocalToSrbP: pthread_create () failed: errno=%d", errno);
        return SYS_ERR_PTHREAD_CREATE;
    }

#ifdef FED_MCAT
    srcRemoteFlag = chkHostAndZone (srcResLoc, &srcHostTabPtr);
    portNum = NULL;
#else
    /* parseAddr change the content of ResLoc */
    tmpResLoc = strdup (srcResLoc);
    portNum = parseAddr (tmpResLoc);
    srcRemoteFlag = chkHost (tmpResLoc, portNum, &srcHostTabPtr);
    free (tmpResLoc);
#endif
    if (srcRemoteFlag < 0) {
        elog (NOTICE, "_svrDataCopySrbToLocalP: Invalid dest host addr %s", 
	 srcResLoc);
        return INP_ERR_HOST_ADDR;               /* failure */
    }

    if ((retval = remoteConnect (srcHostTabPtr)) < 0) {
        return retval;
    }

    retval = _srbDataGet (srcHostTabPtr->conn, srcTypeInx, srcPath,
     srcResLoc, clHostAddr, clPort, size);

    if (retval < 0) {
        elog (NOTICE,
         "_svrDataCopySrbToLocalP: _srbDataGet of %s failed, stat = %d", 
	 srcPath, retval);
        return retval;
    }

    /* this loop prevent a race condition  for very small file transfer
     * the server may send a the short too quickly
     */
    while (myInput.threadInx <= 0 && retryCnt <= MAX_OPEN_RETRY) {
        srbThreadSleep (0, 100000);
        retryCnt ++;
    }
    for (i = 0; i < myInput.threadInx; i++) {
        pthread_join (myInput.threadId[i], NULL);
    }
    pthread_cancel (portalManagerThread);
    close (lsock);
    (void) pthread_join(portalManagerThread, NULL);
    if (retval >= 0 && retval != myInput.bytesMoved) {
        return (OBJ_ERR_COPY_LEN);
    }

    if (myInput.status < 0 && retval >= 0)
        return myInput.status;
    else
        return retval;

}

srb_long_t
_svrDataCopyLocalToSrbP (int destTypeInx, char *destPath, char *destResLoc,
int srcTypeInx, char *srcPath, char *srcResLoc, srb_long_t size)
{
    srb_long_t retval;

    int clPort;
    char *clHostAddr;
    int lsock;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    struct sockaddr_in sin;
    srb_long_t status;
    struct portalInput myInput;
    pthread_t portalManagerThread;
    char sb[MAX_TOKEN];
    struct hostent *hostEnt;
    struct in_addr myIn;
    struct relayHeader myHeader;
    struct hostElement *destHostTabPtr;
    int destRemoteFlag;
    char *portNum;
    char *tmpResLoc;

    if ((lsock = portalCreate ()) < 0) {
	elog (NOTICE, 
	  "_svrDataCopyLocalToSrbP: portalCreate() failed: errno=%d", errno);
        return lsock;
    }

    listen(lsock, SOMAXCONN);

    length = sizeof (sin);
    if (getsockname (lsock, (struct sockaddr *) &sin, &length)) {
	elog (NOTICE, 
         "_svrDataCopyLocalToSrbP: getsockname() failed: errno=%d", errno);
        return CLI_ERR_SOCKET;
    }
    clPort = ntohs (sin.sin_port);
    clPort = genPortInp (clPort);
    /* clHostAddr = inet_ntoa (sin.sin_addr);   XXXX this give 0 addr */
    clHostAddr = getLocalHostNetAddr ();

    if (clHostAddr == NULL) {
        elog (NOTICE,
         "_svrDataCopyLocalToSrbP: getLocalHostNetAddr failed");
        return SYS_ERR_HOST_ADDR;
    }

    myInput.sock = lsock;
    myInput.cookie = getCookieFromPortInp (clPort);
    myInput.locFilePath = srcPath;
    myInput.locTypeInx = srcTypeInx;
    myInput.locResLoc = srcResLoc;
    myInput.operation = PUT_OPR;
    myInput.transferHandler = (void *) svrPortalTransfer;
    myInput.size = size;
    myInput.numThreads = 0;
    myInput.threadInx = 0;
    myInput.bytesMoved = 0;
    myInput.status = 0;
    myInput.errorMessage = NULL;
    pthread_mutex_init(&myInput.varMutex, pthread_mutexattr_default);
    pthread_mutex_init(&myInput.condMutex, pthread_mutexattr_default);
    pthread_cond_init (&myInput.cond, pthread_condattr_default);



   status = pthread_create (&portalManagerThread, pthread_attr_default,
                  (void *(*)(void *)) portalManager,
                  (void *) &myInput);

    if (status < 0) {
	elog (NOTICE, 
         "_svrDataCopyLocalToSrbP: pthread_create () failed: errno=%d", errno);
        return SYS_ERR_PTHREAD_CREATE;
    }

#ifdef FED_MCAT
    destRemoteFlag = chkHostAndZone (destResLoc, &destHostTabPtr);
    portNum = NULL;
#else
    /* parseAddr change the content of ResLoc */
    tmpResLoc = strdup (destResLoc);
    portNum = parseAddr (tmpResLoc);
    destRemoteFlag = chkHost (tmpResLoc, portNum, &destHostTabPtr);
    free (tmpResLoc);
#endif
    if (destRemoteFlag < 0) {
        elog (NOTICE, "_svrDataCopyLocalToSrbP: Invalid dest host addr %s", 
	 destResLoc);
        return INP_ERR_HOST_ADDR;               /* failure */
    }

    if ((retval = remoteConnect (destHostTabPtr)) < 0) {
        return retval;
    }

    retval = _srbDataPut (destHostTabPtr->conn, destTypeInx, destPath,
     destResLoc, clHostAddr, clPort, size);

    if (retval < 0) {
        elog (NOTICE,
         "_svrDataCopyLocalToSrbP: _srbDataPut of %s failed, stat = %d", 
	 destPath, retval);
        return retval;
    }

    for (i = 0; i < myInput.threadInx; i++) {
	pthread_join (myInput.threadId[i], NULL);
    }
    pthread_cancel (portalManagerThread);
    close (lsock);
    (void) pthread_join(portalManagerThread, NULL);
    if (retval >= 0 && retval != myInput.bytesMoved) {
        return (OBJ_ERR_COPY_LEN);
    }

    if (myInput.status < 0 && retval >= 0)
        return myInput.status;
    else
        return retval;

}

/* svrPortalTransfer - This is almost the same as portalTransfer() for client.
 * We need this for server because the client version cannot call some of
 * the server specific I/O routines.
 */

void *
svrPortalTransfer (struct transferInput *tranInp)
{
   struct relayHeader myHeader;
   char *buf = NULL;
   int bufLen = 0;
   int nbytes, toread;
   char *bufptr;
   struct portalInput *myPortalInput;
   int localFd = -1;
   srb_long_t curOffset = 0;
   int status = 1;


   myPortalInput = (struct portalInput *) tranInp->portInput;
   while (status > 0) {
      /* read the header */

      nbytes = read (tranInp->relaySocketFd, (void *) &myHeader,
        sizeof (myHeader));
      if (nbytes < 0) {
         status = unixErrConv (errno);
         break;
      } else if (nbytes == 0) {
         status = 0;
         break;
      }
      myHeader.operation = ntohl (myHeader.operation);
      srbNtohll ((u_longlong_t *)&myHeader.offset);
      myHeader.length = ntohl (myHeader.length);


      if (myHeader.offset < 0) {
         printf ("portalTranser: requested offset %lld less than zero\n",
          myHeader.offset);
      }

      switch (myHeader.operation) {
       case READ_OPR:
       case GET_OPR:
       if (localFd < 0) {
         pthread_mutex_lock(&myPortalInput->varMutex);
         if (tranInp->threadInx == 0) {
	    ParaCreateFlag = 0;
            localFd = lowLevelCreate (myPortalInput->locTypeInx,
             myPortalInput->locResLoc, myPortalInput->locFilePath,
              StorVaultMode, NO_CHKDIR, -1);
            if (localFd < 0) {
                localFd = lowLevelOpen (myPortalInput->locTypeInx,
                 myPortalInput->locFilePath,
                 myPortalInput->locResLoc, O_WRONLY);
            }
            if (localFd < 0) {
                elog (NOTICE,
                 "svrPortalTransfer: cannot create %s, errno = %d\n",
                   myPortalInput->locFilePath, errno);
                status = unixErrConv (errno);
      		myPortalInput->status = status;
		pthread_mutex_unlock (&myPortalInput->condMutex);
		close (tranInp->relaySocketFd);
   		pthread_exit (NULL);
            }
	    ParaCreateFlag = 1;
          } else {
               int retryCnt = 0;
               while (retryCnt < MAX_OPEN_RETRY * 3) {
		   if (ParaCreateFlag == 0) {
			pthread_mutex_unlock (&myPortalInput->varMutex);
			srbThreadSleep (0, 50000);
                        retryCnt ++;
                        pthread_mutex_lock (&myPortalInput->varMutex);
			continue;
		    } else {
			break;
		    }
		}
		if (retryCnt >= MAX_OPEN_RETRY * 3) {
                    elog (NOTICE,
                     "svrPortalTransfer: Creat timed out for %s thread %d",
                      myPortalInput->locFilePath, tranInp->threadInx);
                    myPortalInput->status = OBJ_ERR_CREATE_TIMEOUT;
                    pthread_mutex_unlock (&myPortalInput->condMutex);
                    close (tranInp->relaySocketFd);
                    pthread_exit (NULL);
		}

                localFd = lowLevelOpen (myPortalInput->locTypeInx,
                 myPortalInput->locFilePath,
                 myPortalInput->locResLoc, O_WRONLY);

                if (localFd < 0) {
                    elog (NOTICE,
                     "svrPortalTransfer: cannot create %s, errno = %d\n",
                       myPortalInput->locFilePath, errno);
                    status = unixErrConv (errno);
                    myPortalInput->status = status;
                    pthread_mutex_unlock (&myPortalInput->condMutex);
                    close (tranInp->relaySocketFd);
                    pthread_exit (NULL);
                }
            }
            pthread_mutex_unlock (&myPortalInput->varMutex);
         }

         if (myHeader.length > bufLen) {
            if (bufLen > 0)
               free (buf);
            buf = (char *) malloc (myHeader.length);
            bufLen = myHeader.length;
         }
         toread = myHeader.length;
         bufptr = buf;
         do {
            nbytes = read (tranInp->relaySocketFd, bufptr, toread);
            if (nbytes > 0) {
               toread -= nbytes;
               bufptr += nbytes;
            }
         } while (toread > 0 && nbytes > 0);

         if (toread > 0) {
	    elog (NOTICE,
             "svrPortalTransfer: %d bytes read at offset %d does not equal to header %d",
              myHeader.length - toread, myHeader.offset, myHeader.length);
            status = unixErrConv (errno);
            break;
         }

         if (curOffset != myHeader.offset) {
             curOffset = _objSeek (localFd, myPortalInput->locTypeInx,
	      myHeader.offset, SEEK_SET);
            if (curOffset != myHeader.offset) {
               elog (NOTICE,
                "svrPortalTransfer: _svrObjSeek of %s to %lld err, errno=%d",
                 myPortalInput->locFilePath, myHeader.offset, errno);
               status = unixErrConv (errno);
               break;
            }
         }

         nbytes = _objWrite (localFd, myPortalInput->locTypeInx,
	  buf, myHeader.length);
         if (nbytes != myHeader.length) {
            elog (NOTICE,
             "svrPortalTransfer: %d bytes written at offset %d does not equal to header %d",
              nbytes, myHeader.offset, myHeader.length);
            status = unixErrConv (errno);
            break;
         }

         curOffset += nbytes;
         pthread_mutex_lock(&myPortalInput->varMutex);
         myPortalInput->bytesMoved += myHeader.length;
         pthread_mutex_unlock(&myPortalInput->varMutex);
         break;
       case WRITE_OPR:
       case PUT_OPR:
         if (localFd < 0) {
            localFd = lowLevelOpen (myPortalInput->locTypeInx,
	     myPortalInput->locFilePath, 
	     myPortalInput->locResLoc, O_RDONLY);
            if (localFd < 0) {
               elog (NOTICE,
                "svrPortalTransfer: cannot open %s, errno = %d\n",
                  myPortalInput->locFilePath, errno);
               status = unixErrConv (errno);
               break;
            }
         }

         if (myHeader.offset >= myPortalInput->size) {
            /* we reached the end */
            status = 0;
            break;
         }

         if (curOffset != myHeader.offset) {
            curOffset = _objSeek (localFd, myPortalInput->locTypeInx,
	     myHeader.offset, SEEK_SET);
            if (curOffset != myHeader.offset) {
               elog (NOTICE,
                "svrPortalTransfer: _svrObjSeek of %s to %lld err, errno=%d",
                 myPortalInput->locFilePath, myHeader.offset, errno);
               status = unixErrConv (errno);
               break;
            }
         }

         if (myHeader.length > bufLen) {
            if (bufLen > 0)
               free (buf);
            buf = (char *) malloc (myHeader.length);
            bufLen = myHeader.length;
         }
         toread = _objRead (localFd, myPortalInput->locTypeInx,
	  buf, myHeader.length);
         if (toread <= 0) {
            if (toread < 0) {
	       elog (NOTICE,
                "svrPortalTransfer: read of %s to %d error, errno = %d",
                  myPortalInput->locFilePath, myHeader.offset, errno);
            }
            status = unixErrConv (errno);
            break;
         }

         curOffset += toread;
         nbytes = write (tranInp->relaySocketFd, buf, toread);
         if (nbytes != toread) {
	    elog (NOTICE,
             "svrPortalTransfer: %d bytes written at offset %d does not equal to header %d",
              nbytes, myHeader.offset, toread);
            status = unixErrConv (errno);
            break;
         }
         pthread_mutex_lock(&myPortalInput->varMutex);
         myPortalInput->bytesMoved += nbytes;
         pthread_mutex_unlock(&myPortalInput->varMutex);
         break;
       case DONE_OPR:
       default:
         /* signal to the main thread if everyone are done */
         status = 0;
         break;
      }
   }
   /* signal to the main thread if everyone are done */
   pthread_mutex_lock (&myPortalInput->condMutex);
   myPortalInput->numThreads --;
   if (status < 0)
      myPortalInput->status = status;
   pthread_mutex_unlock (&myPortalInput->condMutex);
   close (tranInp->relaySocketFd);
   if (localFd >= 0)
      lowLevelClose (myPortalInput->locTypeInx, localFd);
   if (buf != NULL)
      free (buf);

   pthread_exit (NULL);
   /* return; */
}
#endif /* PARA_OPR */

/* _svrDataCopyS - serial version of _svrDataCopy
 *
 */

srb_long_t
_svrDataCopyS (int destTypeInx, char *destPath, char *destResLoc,
int srcTypeInx, char *srcPath, char *srcResLoc, srb_long_t size)
{
    srb_long_t retVal = 0;
    int srcExfInx, destExfInx;
    int nbytes;
    char *buf;
    srb_long_t totalWritten = 0;

    if (size == SOURCE_EMPTY) {
	srcExfInx = SOURCE_EMPTY;
    } else {
	srcExfInx = lowLevelOpen (srcTypeInx, srcPath, srcResLoc, O_RDONLY);
    }

    if (srcExfInx < 0 && srcExfInx != SOURCE_EMPTY) {   /* error */
        elog (NOTICE, "_srbDataCopy: Unable of open the source copy");
        lowLevelClose (srcTypeInx, srcExfInx);
        return srcExfInx;
    }

    destExfInx = lowLevelCreate (destTypeInx, destResLoc, destPath, 
     StorVaultMode, NO_CHKDIR, -1);

    if (destExfInx < 0) {   /* error */
	/* try a open */
	destExfInx = lowLevelOpen (destTypeInx, destPath, destResLoc,
	 O_WRONLY);
	if (destExfInx < 0) {
            elog (NOTICE, "_srbDataCopy: Unable to create/open the dest copy");
            return destExfInx;
	}
    }

    /* now read from srcExfInx and write to destExfInx */

    if (srcExfInx != SOURCE_EMPTY) {
        buf = malloc (BUF_SIZE);
        while ((nbytes = _objRead(srcExfInx, srcTypeInx, buf, BUF_SIZE)) > 0) {
            if (_objWrite (destExfInx, destTypeInx, buf, nbytes) != nbytes) {
                elog (NOTICE,
                  "svrObjReplicate: Bytes written does no match bytes read");
                retVal = OBJ_ERR_COPY_LEN;
                break;
            }
            totalWritten += nbytes;
        }
        free (buf);
    }

    if (srcExfInx != SOURCE_EMPTY)
        lowLevelClose (srcTypeInx, srcExfInx);
    lowLevelClose (destTypeInx, destExfInx);

    if (retVal < 0) {

        /* Unlink the destly created data object */

        _objUnlink (srbObjTypeEntry[destTypeInx].objType,
          srbObjTypeEntry[destTypeInx].systemType,
            destResLoc, destPath, NO_CHKDIR);
        return retVal;
    }

    return (totalWritten);
}

srb_long_t *
svrObjCopy (struct varlena *vSrcObjID, struct varlena *vSrcCollection,
struct varlena *vDestObjID, struct varlena *vDestCollection, 
struct varlena *vDestResource)
{
    char *srcObjID;
    char *srcCollection;
    char *destObjID;
    char *destCollection;
    char *destResource;
    srb_long_t *retVal, myOffset;

    retVal = (srb_long_t *) malloc (2*SIZEOF32);

    if (TicketUserFlag) {
        elog (NOTICE, "svrObjCopy: Illegal operation for a ticket user");
	*retVal = ILLEGAL_OPR_TICKET_USER;
        return (retVal);
    }


    srcObjID = varToStr (vSrcObjID);
    srcCollection = varToStr (vSrcCollection);
    destObjID = varToStr (vDestObjID);
    destCollection = varToStr (vDestCollection);
    destResource = varToStr (vDestResource);

    *retVal = _svrObjCopy (srcObjID, srcCollection, destObjID, destCollection,
     destResource);

    free (srcObjID);
    free (srcCollection);
    free (destObjID);
    free (destCollection);
    free (destResource);

    return (retVal);
}

srb_long_t
_svrObjCopy (char *srcObjID, char *srcCollection, char *destObjID, 
char *destCollection, char *destResource)
{
    int callDatacopy = 1;  /* flag for whether _svrdataCopy should be called */
    char mystr[MAX_TOKEN];
    int srcFd, destFd;
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead;
    char *tempStr;
    char *collContainer = NULL;
    struct mdasResInfo *tmpResInfo;
    struct condResult condResult;
    char fullPathName[MAX_TOKEN * 2];
    char compPathName[MAX_TOKEN * 2];
    char *myPathName;
    int copyCnt = 0;            /* Keep track of no. of replica */
    int failCnt = 0;            /* Keep track of no. of failure */
    srb_long_t status, retVal;
    char *condInput;
    char *userName;
    srb_long_t bytesCopied;
    struct mdasResInfo *compResInfo, *myResInfo;
    int intReplNum, replNum;
    int lockFd = -1;
    struct resCache *myResCache;


    if (srcObjID[0] == '\0' || destObjID[0] == '\0') {
	elog (NOTICE, "svrObjCopy: Zero length Input ObjName");
	return (OBJ_ERR_BAD_PATHNAME);
    }

    /* Open the source obj. Is the source a inContainer obj */

    infoInp.accessMode = openFlagLookUp (O_RDONLY);

    infoInp.objID = srcObjID;
    infoInp.userName = ClientUser->userName;
    infoInp.domainName = ClientUser->domainName;
    infoInp.collectionName = srcCollection;

    if ((retVal = mdasGetInfo (&infoInp, &infoOutHead)) < 0) {
        return (retVal);
    }

/*
    sprintf (mystr, "&%s", condInputEntry[CONTAINER].keyword);
*/
    tempStr = strdup (destObjID);
    condInput = malloc (strlen (tempStr) + 1);
    if (mySplitStr (tempStr, destObjID, condInput, '&') == NULL)
        condInput[0] = '\0';

    if (parseCondInput (condInput, CONTAINER, &condResult) >= 0 || 
     (collContainer = chkContColl (destCollection)) != NULL) {
	/* dest is inContainer objects */
	if (collContainer != NULL)
	    free (collContainer);

	callDatacopy = 0;	/* don't call Datacopy */
	srcFd = _svrObjOpenWithInfo (&infoInp, &infoOutHead, O_RDONLY);

    	/* Free any remaining infoOut in the rest of the list */

    	freeAllInfoOut (infoOutHead);

	if (srcFd >= 0)
	    destFd = _svrObjCreate (MDAS_CATALOG, tempStr, "", destResource, 
	     destCollection, "", -1);
	else
	    destFd = srcFd;
    } else if (infoOutHead->containerName[0] != '\0') {  /* inContainer obj */

	/* don't do container */

	callDatacopy = 0;	/* don't call Datacopy */

        srcFd = _svrObjOpenWithInfo (&infoInp, &infoOutHead, O_RDONLY);

        /* Free any remaining infoOut in the rest of the list */

        freeAllInfoOut (infoOutHead);

        if (srcFd >= 0)
            destFd = _svrObjCreate (MDAS_CATALOG, destObjID, "", 
	     destResource, destCollection, "", -1);
        else
            destFd = srcFd;
    }

    free (tempStr);

    if (callDatacopy == 0) {	/* just do the normal copy */
	if (srcFd >= 0 && destFd >= 0) {
	    bytesCopied = srbToSrbCopy (srcFd, destFd);
            if (srbObjDesc[srcFd].chksum > 0 && bytesCopied > 0) {
                srbObjDesc[destFd].chksum = srbObjDesc[srcFd].chksum;
                srbObjDesc[destFd].chksumFlag = k_FLAG;
            }
	    svrObjClose (srcFd);
	    svrObjClose (destFd);
	    retVal = bytesCopied;
 	} else {
	    if (srcFd >= 0) {
		svrObjClose (srcFd);
	    } else {
		retVal = srcFd;
	    }

	    if (destFd >= 0) {
		svrObjClose (destFd);
	    } else {
		retVal = destFd;
	    }
	}
        free (condInput);
	
	return (retVal);
    }

    /* we are here. Will eventually call svrDataCopy */ 

    if (isDataset (destObjID, destCollection) > 0) {
        free (condInput);
        return (DATA_NOT_UNIQUE_IN_COLLECTION);
    }

    if ((retVal = parseCondResInfo (destCollection, MDAS_CATALOG, condInput, 
     destResource, &condResult, &myResCache, infoOutHead->size)) < 0)
        return (retVal);

    userName = ClientUser->userName;
    tmpResInfo = myResCache->resInfoHead;

    while (tmpResInfo != NULL) {
	int tinx;
	struct mdasInfoOut *srcInfoOut;

        if (copyCnt >= condResult.intResult) {   /* Done */
	    retVal = infoOutHead->size;
            break;
	}

	srcInfoOut = infoOutHead;
	while (srcInfoOut != NULL) {
	    compResInfo = NULL;
            if (strcmp (tmpResInfo->phyResType, COMPOUND_RESC) == 0) {
                compResInfo = tmpResInfo->compoundLink;
                while (compResInfo != NULL) {
		    int resClassInx;
                    /* XXXX don't do TAPE_RESC
                    if (strcmp (compResInfo->phyResType, TAPE_RESC) != 0) { */
		    resClassInx = resClassLookup (compResInfo->phyResClass);
                    /* Don't do permanent */
                    if (resClassEntries[resClassInx].permanent == 0) {
#ifdef LOCK_OBJ
                        /* Don't lock container object.It has its own locking */
                        if (infoOutHead->containerName[0] == '\0' && 
			  lockFd < 0) {
                            lockFd =_svrObjLock (destObjID, destCollection,
                              F_SETLKW, F_WRLCK, 0);
                        }
#endif
                        bytesCopied = _svrDataCopyFromInfoToResc (MDAS_CATALOG,
                          destObjID, destCollection,
                          srcInfoOut, compResInfo, "", fullPathName,
                           &tinx);
                        if (bytesCopied >= 0)
                            break;
                    }
                    compResInfo = compResInfo->next;
                }
            } else {
                bytesCopied = _svrDataCopyFromInfoToResc (MDAS_CATALOG,
                  destObjID, destCollection,
                  srcInfoOut, tmpResInfo, "", fullPathName,
                   &tinx);
            }

	    if (bytesCopied >= 0) {
	        /* copy is good */
		copyCnt ++;
		break;
            } else {
#ifdef LOCK_OBJ
                if (lockFd >= 0) {
                    _svrObjLock (destObjID, destCollection,
                      F_SETLKW, F_UNLCK, lockFd);
		    lockFd = -1;
	        }
#endif
		srcInfoOut = srcInfoOut->next;
		continue;
	    }
	}	/* while loop */

	if (bytesCopied < 0) {	/* error */
#ifdef LOCK_OBJ
            if (lockFd >= 0) {
                _svrObjLock (destObjID, destCollection,
                  F_SETLKW, F_UNLCK, lockFd);
                lockFd = -1;
            }
#endif
	    status = bytesCopied;
	    failCnt ++;
	    tmpResInfo = tmpResInfo->next;
	    continue;
	}

        if (compResInfo != NULL) {
            /* Create a fake pathName for the compound obj */
            genCompPath (compPathName, destObjID);
            myPathName = compPathName;
        } else {
            myPathName = fullPathName;
        }


        /* Do the registration */

        if (copyCnt == 1) {	/* first copy */
            retVal = _svrRegisterDataset (MDAS_CATALOG, destObjID, 
	     userName, "all", ClientUser->domainName, infoOutHead->dataType,
	      myPathName, tmpResInfo->phyResName, destCollection, 
	       bytesCopied, tinx, tmpResInfo->phyResLoc);
        } else if (copyCnt > 1) {
            retVal = _svrRegisterReplica (MDAS_CATALOG, destObjID, 
	     destCollection, NULL, NULL, tmpResInfo->phyResName, myPathName,
	      1, tinx, tmpResInfo->phyResLoc, 
	      userName, ClientUser->domainName);
        } else {
	    /* should not be here */
	    tmpResInfo = tmpResInfo->next;
	    continue;
	}

        if (srcInfoOut->chksum > 0) {
            char chksum[CHKSUM_STR_LEN + 1];
            sprintf (chksum, "%d", srcInfoOut->chksum);
            status = _svrModifyDataset (MDAS_CATALOG, destObjID,
              destCollection, tmpResInfo->phyResName, myPathName,
              chksum,"", D_INSERT_DCHECKSUM);
            if (status < 0) {
                elog (NOTICE,
                 "_svrObjCopy: reg chksum error for %s/%s, status = %d",
                     destCollection, destObjID, status);
            }
	}

        if (retVal < 0) {
#ifdef LOCK_OBJ
            if (lockFd >= 0) {
                _svrObjLock (destObjID, destCollection,
                  F_SETLKW, F_UNLCK, lockFd);
                lockFd = -1;
            }
#endif
	    status = retVal;
	    copyCnt --;
            failCnt ++;
            if (condResult.intResult == ALL)
                break;
	}

        if (compResInfo != NULL) {
            intReplNum = _svrRegInternalCompObj (
              destObjID, destCollection, retVal, NO_INT_SEGMENTATION,
              compResInfo->phyResName, fullPathName, bytesCopied, 0,
              NEW_INT_REPL_NUM, NO_INT_SEGMENTATION,
              tinx, compResInfo->phyResLoc,
              ClientUser->userName, ClientUser->domainName);
            if (intReplNum < 0) {
#ifdef LOCK_OBJ
                if (lockFd >= 0) {
                    _svrObjLock (destObjID, destCollection,
                      F_SETLKW, F_UNLCK, lockFd);
                    lockFd = -1;
                }
#endif
                status = intReplNum;
                copyCnt --;
                failCnt ++;
                if (condResult.intResult == ALL)
                    break;
            }
        }

        tmpResInfo = tmpResInfo->next;
    }	/* while loop */

#ifdef LOCK_OBJ
    if (lockFd >= 0) {
        _svrObjLock (destObjID, destCollection,
          F_SETLKW, F_UNLCK, lockFd);
        lockFd = -1;
    }
#endif

    if (condResult.intResult == ALL) {
	if (failCnt > 0) {
	    if (retVal >= 0) {
	        if (status < 0)
		    retVal = status;
	        else
		    retVal = OBJ_ERR_COPY_CNT;
	    }
	} else {
	    retVal = infoOutHead->size;
	}
    } else if (copyCnt >= condResult.intResult) {
	retVal = infoOutHead->size;
    } else {
        if (retVal >= 0) {
            if (status < 0)
                retVal = status;
            else
                retVal = OBJ_ERR_COPY_CNT;
	}
    }
    free (condInput);
    freeInfoOut (infoOutHead);
    return (retVal);
}

srb_long_t
srbToSrbCopy (int srcFd, int destFd)
{
    char *buf;
    srb_long_t bytesCopied = 0;
    int nbytes, byteWritten;

    /* Use _svrObjRead and _svrObjWrite which deal with
     * inContainer obj */

    buf = malloc (BUF_SIZE);

    while ((nbytes = _svrObjRead (srcFd, buf, BUF_SIZE)) > 0) {
        if ((byteWritten = _svrObjWrite (destFd, buf, nbytes)) != nbytes) {
            elog (NOTICE,
             "srbToSrbCopy: Bytes written %d don't match read %d", 
              nbytes, byteWritten);
            if (byteWritten < 0) {
                bytesCopied = byteWritten;
            } else {
                bytesCopied = OBJ_ERR_COPY_LEN;
            }
            break;
        }
        bytesCopied += nbytes;
    }
    free (buf);
    return (bytesCopied);
}

srb_long_t *
svrObjPut (struct varlena *vDestObjID, struct varlena *vDestCollection, 
struct varlena *vDestResource, struct varlena *vClHostAddr, int clPort, 
struct varlena *vDataType, struct varlena *vDestPath, srb_long_t *size,
int forceFlag)
{
    char *destObjID;
    char *destCollection;
    char *destResource;
    char *clHostAddr;
    char *destPath;
    char *dataType;
    srb_long_t mySize;
    srb_long_t *retVal;

    retVal = (srb_long_t *) malloc (2*SIZEOF32);

    if (TicketUserFlag) {
        elog (NOTICE, "svrObjPut: Illegal operation for a ticket user");
        *retVal = ILLEGAL_OPR_TICKET_USER;
        return (retVal);
    }

    destResource = varToStr (vDestResource);
    dataType = varToStr (vDataType);
    destObjID = varToStr (vDestObjID);
    destCollection = varToStr (vDestCollection);
    destPath = varToStr (vDestPath);
    clHostAddr = varToStr (vClHostAddr);
    mySize = *size;

    *retVal = _svrObjPut (destObjID, destCollection, destResource, clHostAddr,
      clPort, dataType, destPath, mySize, forceFlag, 0, NULL);
     
    free (destObjID);
    free (destCollection);
    free (destResource);
    free (clHostAddr);
    free (dataType);

    return (retVal);
}

struct varlena *
svrObjPutC (struct varlena *vDestObjID, struct varlena *vDestCollection, 
struct varlena *vDestResource, 
struct varlena *vDataType, struct varlena *vDestPath, srb_long_t *size,
int forceFlag, int numThreads, svrComm_t *myComm)
{
    char *destObjID;
    char *destCollection;
    char *destResource;
    char *destPath;
    char *dataType;
    srb_long_t mySize;
    struct varlena * retVal;
    srb_long_t status;
    int sstatus;

    retVal = (struct varlena *)malloc(VAROUTHDRSZ);

    if (TicketUserFlag) {
        elog (NOTICE, "svrObjPutC: Illegal operation for a ticket user");
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (ILLEGAL_OPR_TICKET_USER);
        return (retVal);
    }

    destResource = varToStr (vDestResource);
    dataType = varToStr (vDataType);
    destObjID = varToStr (vDestObjID);
    destCollection = varToStr (vDestCollection);
    destPath = varToStr (vDestPath);
    mySize = *size;

    status = _svrObjPut (destObjID, destCollection, destResource, NULL,
      0, dataType, destPath, mySize, forceFlag | CL_INIT_CONN_FLAG, 
      numThreads, myComm);
     
    free (destObjID);
    free (destCollection);
    free (destResource);
    free (dataType);

    if (status >= 0) {
        sstatus = 0;
    } else {
        sstatus = status;
    }
    VARSIZE(retVal) = VAROUTHDRSZ;
    VAROUTSTAT(retVal) = htonl (sstatus);

    return (retVal);
}

srb_long_t 
_svrObjPut (char *myDestObjID, char *destCollection, char *destResource, 
char *clHostAddr, int clPort, char *dataType, char *destPath, 
srb_long_t mySize, int flag, int numThreads, svrComm_t *myComm)
{
    char *destObjID;
    int callDataget = 1;  /* flag for whether _svrdataCopy should be called */
    char mystr[MAX_TOKEN];
    int srcFd, destFd;
    int retval;
    char *collContainer = NULL;
    struct mdasResInfo *tmpResInfo;
    struct condResult condResult;
    char fullPathName[MAX_TOKEN * 2];
    char compPathName[MAX_TOKEN * 2];
    char *myPathName;
    int copyCnt = 0;            /* Keep track of no. of replica */
    int failCnt = 0;            /* Keep track of no. of failure */
    srb_long_t status;
    char *condInput;
    char *userName;
    struct hostElement  *hostTabPtr;
    srb_long_t retVal;
    struct mdasResInfo *compResInfo, *myResInfo;
    int intReplNum, replNum;
    int fileExist;
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *tmpInfoOut;
    struct mdasInfoOut *infoOutHead = NULL;
    int chksumFlag;
    unsigned long iChksum;
    int lockFd = -1;
    int lsock = -1, lport;
    struct resCache *myResCache;

    if (TicketUserFlag) {
        elog (NOTICE, "_svrObjPut: Illegal operation for a ticket user");
	retVal = ILLEGAL_OPR_TICKET_USER;
        return (retVal);
    }

    destObjID = malloc (strlen (myDestObjID) + 1);
    condInput = malloc (strlen (myDestObjID) + 1);
    if (mySplitStr (myDestObjID, destObjID, condInput, '&') == NULL)
        condInput[0] = '\0';

    if (destObjID[0] == '\0') {
	elog (NOTICE, "svrObjPut: Zero length Input ObjName");
	free (destObjID);
	free (condInput);
	retVal = OBJ_ERR_BAD_PATHNAME;
	return (retVal);
    }

    if (parseCondInput (condInput, REG_CHKSUM, &condResult) >= 0) {
        chksumFlag = k_FLAG;
        iChksum = condResult.intResult;
    } else if (parseCondInput(condInput, VERIFY_CHKSUM, &condResult) >= 0) {
        chksumFlag = K_FLAG;
        iChksum = condResult.intResult;
    } else {
	chksumFlag = 0;
	iChksum = 0;
    }

    infoInp.domainName = ClientUser->domainName;
    infoInp.accessMode = "write";
    infoInp.objID = myDestObjID;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = destCollection;

    retVal = mdasGetInfo (&infoInp, &infoOutHead);

    if (retVal >= 0) {
	/* the file exist */
        if (infoOutHead->next != NULL)
	    fileExist = 2;	/* more than 1 */
	else
	    fileExist = 1;
#ifdef foo
	/* will do just one copy */
	if (forceFlag <= 0 || infoOutHead->next != NULL) {
#endif
	if (flag & F_FLAG == 0) {
            free (destObjID);
            free (condInput);
            return (DATA_NOT_UNIQUE_IN_COLLECTION);
	}
    } else {
	fileExist = 0;
    }

    if (parseCondInput (condInput, CONTAINER, &condResult) >= 0 || 
     (collContainer = chkContColl (destCollection)) != NULL ||
     (infoOutHead != NULL && infoOutHead->containerName[0] != '\0')) {
	char *buf;

	/* inContainer objects */
	if (collContainer != NULL)
	    free (collContainer);

	if (fileExist > 0) {
	    destFd = _svrObjOpenWithInfo (&infoInp, &infoOutHead, O_WRONLY);
	} else {
	    destFd = _svrObjCreate (MDAS_CATALOG, myDestObjID, "", destResource,
	     destCollection, "", -1);
	}

	/* setup srcFd here */

        if (flag & CL_INIT_CONN_FLAG) {

            lsock = setupSrvPortal (myComm, &lport);
            if (lsock < 0) {
                svrObjClose (srcFd);
                return (lsock);
            }
            srcFd = acceptSrvPortal (lsock, lport);
#ifdef _WIN32
            closesocket(lsock);
#else
            close (lsock);
#endif

            if (srcFd < 0) {
                svrObjClose (destFd);
                return (srcFd);
            }
            status = write1stHeaderToPortal (srcFd,
             srbObjDesc[destFd].objSize, 1);
            if (status < 0) {
                return (status);
            }
        } else {
	    srcFd = portalConnect (clPort, clHostAddr);
            if (srcFd < 0) {
                svrObjClose (destFd);
                return (srcFd);
            }
	}

	retVal = portalToSrbCopy (srcFd, destFd, mySize);
	close (srcFd);
	svrObjClose (destFd);

	freeAllInfoOut (infoOutHead);
	free (destObjID);
	free (condInput);
	return (retVal);
    }


#ifdef foo
    if (isDataset (destObjID, destCollection) > 0) {
	free (destObjID);
        free (condInput);
        return (DATA_NOT_UNIQUE_IN_COLLECTION);
    }
#endif

    /* we are here. Will eventually call svrDataPut */ 

    if (fileExist > 1) {	/* have to depend on Obj desc to manage */
	destFd = _svrObjOpenWithInfo (&infoInp, &infoOutHead, O_WRONLY|O_TRUNC);

	if (destFd < 0) {
            return destFd;
        }

	/* setup srcFd here */

        if (flag & CL_INIT_CONN_FLAG) {
            int lsock, lport;

            lsock = setupSrvPortal (myComm, &lport);
            if (lsock < 0) {
                svrObjClose (srcFd);
                return (lsock);
            }
            srcFd = acceptSrvPortal (lsock, lport);
#ifdef _WIN32
            closesocket(lsock);
#else
            close (lsock);
#endif
            if (srcFd < 0) {
                svrObjClose (destFd);
                return (srcFd);
            }
            status = write1stHeaderToPortal (srcFd,
             srbObjDesc[destFd].objSize, 1);
            if (status < 0) {
                svrObjClose (destFd);
                return (status);
            }
        } else {
	    srcFd = portalConnect (clPort, clHostAddr);
            if (srcFd < 0) {
                svrObjClose (destFd);
                return (srcFd);
            }
	}

	retVal = portalToSrbCopy (srcFd, destFd, mySize);
	close (srcFd);
	svrObjClose (destFd);

	freeAllInfoOut (infoOutHead);
	free (destObjID);
	free (condInput);
	return (retVal);
    } else if (fileExist == 1) {
        retVal = rmContentOfInfoOut (infoOutHead, destCollection);
        if (retVal < 0) {
            elog (NOTICE,
             "_svrObjPut: rmContentOfInfoOut error. status = %d", retVal);
            freeAllInfoOut (infoOutHead);
            free (destObjID);
            free (condInput);
            return (retVal);
        }
        if ((retVal = parseCondResInfo (destCollection, MDAS_CATALOG, "",
         infoOutHead->resourceName, &condResult, &myResCache,
	  infoOutHead->size)) < 0) {
            freeAllInfoOut (infoOutHead);
            free (destObjID);
            free (condInput);
            return (retVal);
        }
	destPath = infoOutHead->dataPath;
    } else {
        if ((retVal = parseCondResInfo (destCollection, MDAS_CATALOG, condInput,
         destResource, &condResult, &myResCache, mySize)) < 0) {
            freeAllInfoOut (infoOutHead);
            free (destObjID);
            free (condInput);
            return (retVal);
	}
    }

    userName = ClientUser->userName;
    tmpResInfo = myResCache->resInfoHead;

    while (tmpResInfo != NULL) {
	int tinx;
	struct mdasInfoOut *srcInfoOut;
	srb_long_t byteWritten;

        if (copyCnt >= condResult.intResult) {   /* Done */
	    retVal = mySize;
            break;
	}

	compResInfo = NULL;
        if (strcmp (tmpResInfo->phyResType, COMPOUND_RESC) == 0) {
            compResInfo = tmpResInfo->compoundLink;
            while (compResInfo != NULL) {
		int resClassInx;
                /* XXXX don't do TAPE_RESC
                if (strcmp (compResInfo->phyResType, TAPE_RESC) != 0) { */
		resClassInx = resClassLookup (compResInfo->phyResClass);
                /* Don't do permanent */
                if (resClassEntries[resClassInx].permanent == 0) {
#ifdef LOCK_OBJ
                    if (lockFd < 0) {
                        lockFd =_svrObjLock (destObjID, destCollection,
                          F_SETLKW, F_WRLCK, 0);
                    }
#endif
		    byteWritten = _svrDataPutToResc (MDAS_CATALOG,
	              destObjID, destCollection, clHostAddr, clPort, mySize,
	  	      compResInfo, destPath, fullPathName, &tinx, 
		      flag, numThreads, myComm);
                    if (byteWritten >= 0)
                        break;
                }
                compResInfo = compResInfo->next;
            }
        } else {
	    byteWritten = _svrDataPutToResc (MDAS_CATALOG,
	      destObjID, destCollection, clHostAddr, clPort, mySize,
	      tmpResInfo, destPath, fullPathName, &tinx,
	       flag, numThreads, myComm);
	}

	if (byteWritten >= 0) {
	    /* copy is good */
	    copyCnt ++;
	} else {
#ifdef LOCK_OBJ
            if (lockFd >= 0) {
                _svrObjLock (destObjID, destCollection,
                  F_SETLKW, F_UNLCK, lockFd);
                lockFd = -1;
            }
#endif
	    status = byteWritten;
	    failCnt ++;
	    tmpResInfo = tmpResInfo->next;
	    continue;
	}

        if (compResInfo != NULL) {
            /* Create a fake pathName for the compound obj */
            genCompPath (compPathName, destObjID);
            myPathName = compPathName;
        } else {
            myPathName = fullPathName;
        }

        /* Do the registration */

	if (fileExist > 0) {
	    if (infoOutHead->size != byteWritten) {
		char strSize[32];
#if defined(PORTNAME_osx)
        	sprintf (strSize, "%-qd", byteWritten);
#else
        	sprintf (strSize, "%-lld", byteWritten);
#endif
                retVal = _svrModifyDataset (MDAS_CATALOG,
		 destObjID, destCollection,
          	 "", "", strSize, NULL, D_CHANGE_SIZE);
	    }
        } else if (copyCnt == 1) {	/* first copy */
            retVal = _svrRegisterDataset (MDAS_CATALOG, destObjID, 
	     userName, "all", ClientUser->domainName, dataType,
	      myPathName, tmpResInfo->phyResName, destCollection, 
	       byteWritten, tinx, tmpResInfo->phyResLoc);
        } else if (copyCnt > 1) {
            retVal = _svrRegisterReplica (MDAS_CATALOG, destObjID, 
	     destCollection, NULL, NULL, tmpResInfo->phyResName, myPathName, 
	      1, tinx, tmpResInfo->phyResLoc,
	      userName, ClientUser->domainName);
        } else {
	    /* should not be here */
	    tmpResInfo = tmpResInfo->next;
	    continue;
	}

        if (retVal < 0) {
#ifdef LOCK_OBJ
            if (lockFd >= 0) {
                _svrObjLock (destObjID, destCollection,
                  F_SETLKW, F_UNLCK, lockFd);
                lockFd = -1;
            }
#endif
	    status = retVal;
	    copyCnt --;
            failCnt ++;
            if (condResult.intResult == ALL)
                break;
	}

        if (compResInfo != NULL) {
            intReplNum = _svrRegInternalCompObj (
              destObjID, destCollection, retVal, NO_INT_SEGMENTATION,
              compResInfo->phyResName, fullPathName, byteWritten, 0,
              NEW_INT_REPL_NUM, NO_INT_SEGMENTATION,
              tinx, compResInfo->phyResLoc,
              ClientUser->userName, ClientUser->domainName);
            if (intReplNum < 0) {
#ifdef LOCK_OBJ
                if (lockFd >= 0) {
                    _svrObjLock (destObjID, destCollection,
                      F_SETLKW, F_UNLCK, lockFd);
                    lockFd = -1;
                }
#endif
                status = retVal;
                copyCnt --;
                failCnt ++;
                if (condResult.intResult == ALL)
                    break;
            }
        }
	if (infoOutHead != NULL && infoOutHead->chksum > 0 || chksumFlag) {
	    char chksum[CHKSUM_STR_LEN + 1];

            if (compResInfo != NULL) {
                /* a compound object */

	        retVal = _objChksumAndReg (tinx, 0, chksum, 0, 0,
	         destObjID, destCollection, compResInfo->phyResName, fullPathName,
                 tmpResInfo->phyResLoc, chksumFlag, iChksum, 
		 tmpResInfo->phyResName, myPathName);
	    } else {
	        retVal = _objChksumAndReg (tinx, 0, chksum, 0, 0,
                 destObjID, destCollection, tmpResInfo->phyResName, myPathName,
                 tmpResInfo->phyResLoc, chksumFlag, iChksum, 
		 tmpResInfo->phyResName, myPathName);
	    }
	    if (retVal < 0) {
		status = retVal;
                copyCnt --;
                failCnt ++;
                if (condResult.intResult == ALL)
                    break;
	    }
	}
        tmpResInfo = tmpResInfo->next;
    }	/* while loop */

#ifdef LOCK_OBJ
    if (lockFd >= 0) {
        _svrObjLock (destObjID, destCollection,
          F_SETLKW, F_UNLCK, lockFd);
        lockFd = -1;
    }
#endif

    if (condResult.intResult == ALL) {
	if (failCnt > 0) {
	    if (retVal >= 0) {
	        if (status < 0)
		    retVal = status;
	        else
		    retVal = OBJ_ERR_COPY_CNT;
	    }
	} else {
	    retVal = mySize;
	}
    } else if (copyCnt >= condResult.intResult) {
	retVal = mySize;
    } else {
        if (retVal >= 0) {
            if (status < 0)
                retVal = status;
            else
                retVal = OBJ_ERR_COPY_CNT;
	}
    }
    free (destObjID);
    free (condInput);
    freeAllInfoOut (infoOutHead);
    return (retVal);
}

srb_long_t 
portalToSrbCopy (int srcFd, int destFd, srb_long_t mySize)
{
    char *buf;
    struct relayHeader myHeader;
    int status;
    int nbytes = 0;
    srb_long_t byteWritten = 0;
    srb_long_t bytesCopied = 0;

    buf = malloc (BUF_SIZE);

    while (bytesCopied < mySize) {
        int toread;
        char *bufptr;

        myHeader.operation = htonl (PUT_OPR);
        myHeader.offset = bytesCopied;
        srbHtonll (&myHeader.offset);
        if (bytesCopied + BUF_SIZE > mySize) { 
            toread = mySize - bytesCopied;
            myHeader.length = htonl (toread);
        } else {
            toread = BUF_SIZE;
            myHeader.length = htonl (BUF_SIZE);
        }

        status = write (srcFd, (void *) &myHeader, sizeof (myHeader));

        if (status <= 0) {
            elog (NOTICE,
             "svrObjPut: error writing header to client, status = %d",
              (int) status);
            bytesCopied = SYS_ERR_REMOTE_CONN;
            break;
        }

        myHeader.length = toread;
        bufptr = buf;
        do {
            nbytes = read (srcFd, bufptr, toread);
            if (nbytes > 0) {
                toread -= nbytes;
                bufptr += nbytes;
            }
        } while (toread > 0 && nbytes > 0);

        if (toread == 0) {
            nbytes = myHeader.length;
            if ((byteWritten = _svrObjWrite (destFd, buf, nbytes)) != nbytes) {
                elog (NOTICE,
                 "portalToSrbCopy: Bytes written %d don't match read %d", 
                   nbytes, byteWritten);
                if (byteWritten < 0) {
                    bytesCopied = byteWritten;
                } else {
                    bytesCopied = OBJ_ERR_COPY_LEN;
                }
                break;
            }
            bytesCopied += nbytes;
        } else if (toread < 0) {
            bytesCopied = toread;
        } else {        /* toread > 0 */
            elog (NOTICE,
              "portalToSrbCopy: To read %d bytes, %d bytes read from client",
                myHeader.length, myHeader.length - toread);
            bytesCopied = OBJ_ERR_COPY_LEN;
        }
    }   /* while loop */

    free (buf);
    return (bytesCopied);
}

/* svrDataPut - Copy a dataset from client's portal to destination.
 * If size == SOURCE_EMPTY, just create the dest dataset but
 * no copying. A size == -1 mean don't know the size and copying
 * will be carried out.
 */

srb_long_t *
svrDataPut (int destTypeInx, struct varlena *vDestPath, 
struct varlena *vDestResLoc, struct varlena *vClHostAddr, 
int clPort, srb_long_t *size)
{
    char *destPath;
    char *destResLoc;
    char *clHostAddr;
    srb_long_t mySize;
    srb_long_t *retVal;

    retVal = (srb_long_t *) malloc (2*SIZEOF32);

    if (ProxyUser->privUserFlag <= 0) {
        elog (NOTICE, "svrDataPut: User has no privilege for this operation");
        *retVal = AUTH_ERR_PROXY_NOPRIV;
        return (retVal);
    }

    if (TicketUserFlag) {
        elog (NOTICE, "svrObjPut: Illegal operation for a ticket user");
        *retVal = ILLEGAL_OPR_TICKET_USER;
        return (retVal);
    }

    destPath = varToStr (vDestPath);
    destResLoc = varToStr (vDestResLoc);
    clHostAddr = varToStr (vClHostAddr);
    mySize = *size;

    *retVal = _svrDataPut (destTypeInx, destPath, destResLoc, clHostAddr,
     clPort, mySize, 0, 0, NULL);

    free (destPath);
    free (destResLoc);
    free (clHostAddr);

    return (retVal);    
}

struct varlena *
svrDataPutC (int destTypeInx, struct varlena *vDestPath, 
struct varlena *vDestResLoc, srb_long_t *size, 
int flag, int numThreads, svrComm_t *myComm)
{
    char *destPath;
    char *destResLoc;
    char *clHostAddr;
    srb_long_t mySize;
    struct varlena * retVal;
    srb_long_t status;
    int sstatus;


    retVal = (struct varlena *)malloc(VAROUTHDRSZ);

    if (ProxyUser->privUserFlag <= 0) {
        elog (NOTICE, "svrDataPut: User has no privilege for this operation");
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (AUTH_ERR_PROXY_NOPRIV);
        return (retVal);
    }

    if (TicketUserFlag) {
        elog (NOTICE, "svrObjPut: Illegal operation for a ticket user");
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (ILLEGAL_OPR_TICKET_USER);
        return (retVal);
    }

    destPath = varToStr (vDestPath);
    destResLoc = varToStr (vDestResLoc);
    mySize = *size;

    status = _svrDataPut (destTypeInx, destPath, destResLoc, NULL,
     0, mySize, flag | CL_INIT_CONN_FLAG, numThreads, myComm);

    free (destPath);
    free (destResLoc);
    if (status >= 0) {
        sstatus = 0;
    } else {
        sstatus = status;
    }
    VARSIZE(retVal) = VAROUTHDRSZ;
    VAROUTSTAT(retVal) = htonl (sstatus);

    return (retVal);    
}

/* _svrDataPut - The internal version of svrDataPut.
 * Copy a dataset from client's portal to destination.
 * If size == SOURCE_EMPTY, just create the dest dataset but
 * no copying. A size == -1 mean don't know the size and copying
 * will be carried out.
 */

srb_long_t
_svrDataPut (int destTypeInx, char *destPath, char *destResLoc,
char *clHostAddr, int clPort, srb_long_t size, 
int flag, int numThreads, svrComm_t *myComm)
{
    srb_long_t retVal;
    int srcFd, destExfInx;
    int destType;
    char *portNum;
    struct hostElement  *hostTabPtr;
    int remoteFlag;
    char *tmpResLoc;
    portList_t *myPortList;
    int status;

    if (size < 0)
	return (OBJ_ERR_COPY_LEN);

    destType = srbObjTypeEntry[destTypeInx].objType;

    /* see if we need to do it remotely */

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (destResLoc, &hostTabPtr);
    portNum = NULL;
#else
    /* parseAddr change the content of ResLoc */
    tmpResLoc = strdup (destResLoc);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
    free (tmpResLoc);
#endif

    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", destResLoc);
        return INP_ERR_HOST_ADDR;               /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }
        if (flag & CL_INIT_CONN_FLAG) {
            srbResult *res;
            char mybuf[10*MAX_TOKEN];
            int actualResultLen;

            retVal = _srbDataPutC (hostTabPtr->conn, destTypeInx, destPath,
             destResLoc, size, flag, numThreads, &myPortList);
            if (retVal >= 0) {
                retVal = sendPortList (myComm, myPortList);
		free (myPortList);
                /* read the extra transfer status */
                res = (srbResult *) clReadServerReturn (hostTabPtr->conn,
                 mybuf, &actualResultLen, 0);

                if ((status = clResultStatus(res)) < 0) {
                    retVal = status;
                } else {
                    /* set it to size so that calling routine is happy */
                    retVal = size;
                }
                clClear(res);
            }
        } else {
            retVal = _srbDataPut (hostTabPtr->conn, destTypeInx, destPath,
             destResLoc, clHostAddr, clPort, size);
	}
        return retVal;
    }
    if (srbObjTypeEntry[destTypeInx].chdirFlag > 0) {
        if ((retVal = chkdir (srbObjTypeEntry[destTypeInx].systemType, 
	     destResLoc, destPath, WRT_CHK, FILE_T)) < 0) {
                elog (NOTICE, "_svrDataPut: chkdir error, status = %d", status);
                return retVal;
	}
     }
#ifdef PARA_OPR
    if ((flag & CL_INIT_CONN_FLAG) == 0 && destType == FILE_TYPE &&
     (srbObjTypeEntry[destTypeInx].systemType == HPSS_FT)
      && size != SOURCE_EMPTY && size != 0) {
        retVal = paraHPSSPut (destPath, destResLoc,
         srbObjTypeEntry[destTypeInx].systemType, clHostAddr, clPort, size);
        return retVal;
    } else if ((size <= SizePerThread && numThreads <= 1) || 
     destType != FILE_TYPE || size == 0) {  
	/* too small for double buffer */
        destExfInx = lowLevelCreate (destTypeInx, destResLoc, destPath, 
	 StorVaultMode, NO_CHKDIR, -1);
        if (destExfInx < 0) {   /* error */
            elog (NOTICE, "_srbDataPut: Unable to create the dest copy");
            return destExfInx;
        }

        if (flag & CL_INIT_CONN_FLAG) {
            int lsock, lport;

            lsock = setupSrvPortal (myComm, &lport);
            if (lsock < 0) {
                lowLevelClose (destTypeInx, destExfInx);
                return (lsock);
            }
            srcFd = acceptSrvPortal (lsock, lport);
#ifdef _WIN32
            closesocket(lsock);
#else
            close (lsock);
#endif
            if (srcFd < 0) {
                lowLevelClose (destTypeInx, destExfInx);
                return (srcFd);
            }
            status = write1stHeaderToPortal (srcFd, size, 1);
            if (status < 0) {
                lowLevelClose (destTypeInx, destExfInx);
                return (status);
            }
        } else {
            srcFd = portalConnect (clPort, clHostAddr);
            if (srcFd < 0) {   /* error */
                /* use fprintf intead of elog because multi error msg to client
                 * can cause core dump */
                fprintf (stderr,
                 "_srbDataPut: Unable to connect to port %d at %s\n",
                 clPort & 0xffff, clHostAddr);
                lowLevelClose (destTypeInx, destExfInx);
                return srcFd;
            }
	}

        retVal = portalToSrbCopySB (srcFd, destExfInx, destTypeInx, size);
        close (srcFd);
        lowLevelClose (destTypeInx, destExfInx);
    } else {
        /* do parallel transfer */
        retVal = portalToSrbCopyP (destTypeInx, destPath, destResLoc,
          clHostAddr, clPort, size, flag, numThreads, myComm);
    }
#else

    destExfInx = lowLevelCreate (destTypeInx, destResLoc, destPath, 
     StorVaultMode, NO_CHKDIR, -1);

    if (destExfInx < 0) {   /* error */
        elog (NOTICE, "_srbDataPut: Unable to create the dest copy");
        return destExfInx;
    }

    srcFd = portalConnect (clPort, clHostAddr);

    if (srcFd < 0) {   /* error */
        /* use fprintf intead of elog because multi error msg to client
         * can cause core dump */
        fprintf (stderr, "_srbDataPut: Unable to connect to port %d at %s\n",
         clPort & 0xffff, clHostAddr);
        return srcFd;
    }

    retVal = portalToSrbCopySB (srcFd, destExfInx, destTypeInx, size);

    close (srcFd);
    lowLevelClose (destTypeInx, destExfInx);

#endif  /* PARA_OPR */

    if (retVal < 0) {

        /* Unlink the destly created data object */

        _objUnlink (srbObjTypeEntry[destTypeInx].objType,
          srbObjTypeEntry[destTypeInx].systemType,
            destResLoc, destPath, NO_CHKDIR);
    }

    return (retVal);
}

#ifdef PARA_OPR
srb_long_t
portalToSrbCopyDB (int srcFd, int destExfInx, int destTypeInx, srb_long_t size)
{

    srb_long_t totalWritten = 0;
    int nbytes = 0;
    int byteWritten = 0;
    srb_long_t bytesCopied = 0;
    pthread_t relayThread;
    struct dBuffer dBuf;
    int curInx, compInx;
    pthread_mutex_t condMutex;
    pthread_cond_t  myCondition;

    initDBuffer (&dBuf, &condMutex, &myCondition, NULL);
    dBuf.operation = PUT_OPR;

    dBuf.fileFd = srcFd;

    dBuf.buf[0] = malloc(BUF_SIZE);
    if (!dBuf.buf[0]) {
        close (dBuf.fileFd);
	lowLevelClose (destTypeInx, destExfInx);
        return (SYS_ERR_MALLOC);
    }
    dBuf.buf[1] = malloc(BUF_SIZE);
    if (!dBuf.buf[1]) {
        close (dBuf.fileFd);
	lowLevelClose (destTypeInx, destExfInx);
	free (dBuf.buf[0]);
        return (SYS_ERR_MALLOC);
    }

    pthread_create(&relayThread, pthread_attr_default, 
     (void *(*)(void *)) writeRelay, (void *) &dBuf);

    while (bytesCopied < size && dBuf.status[dBuf.curInx] >= 0 &&
     dBuf.status[dBuf.curInx] != DBUF_ALL_DONE) {

        curInx = dBuf.curInx;
        if (dBuf.length[0] == 0) {
            pthread_mutex_lock (dBuf.condMutex);
	    dBuf.offset[curInx] = 0;
	    if (size > BUF_SIZE)
        	dBuf.length[curInx] = BUF_SIZE;
	    else
		dBuf.length[curInx] = size;

            /* signal the relay thread */
            pthread_cond_signal (dBuf.myCondition);
            pthread_mutex_unlock (dBuf.condMutex);
         }
         pthread_mutex_lock (dBuf.condMutex);

         while (1) {
	    if (dBuf.status[curInx] == DBUF_HAVE_DATA) {
		compInx = curInx;
		dBuf.curInx = curInx = compInx ^ 1;
		break;
	    } else if (dBuf.status[curInx] == DBUF_ALL_DONE) {
		break;
	    } else if (dBuf.status[curInx] < 0) {
		bytesCopied = dBuf.status[curInx];
		break;
	    } else {
		pthread_cond_wait (dBuf.myCondition, dBuf.condMutex);
	    }
	}

	if (dBuf.status[curInx] == DBUF_ALL_DONE ||
	 dBuf.status[curInx] < 0) {
	    pthread_mutex_unlock (dBuf.condMutex);
	    break;
	}

	bytesCopied += dBuf.length[compInx];

        if (bytesCopied + BUF_SIZE > size) {
            dBuf.length[curInx] = size - bytesCopied;
        } else {
            dBuf.length[curInx] = BUF_SIZE;
        }

	dBuf.offset[curInx] = bytesCopied;
	dBuf.status[curInx] = DBUF_NO_DATA;

	/* signal the relay thread */
	pthread_cond_signal (dBuf.myCondition);

	pthread_mutex_unlock (dBuf.condMutex);

        if ((byteWritten = _objWrite ( destExfInx, destTypeInx, 
	 dBuf.buf[compInx], dBuf.length[compInx])) != 
	  dBuf.length[compInx]) {
            elog (NOTICE,
             "svrDataPut: Bytes written %d don't match read %d",
              byteWritten, dBuf.length[compInx]);
            if (byteWritten < 0) {
                bytesCopied = byteWritten;
            } else {
                bytesCopied = OBJ_ERR_COPY_LEN;
            }
            break;
        }
    }	/* while loop */
	
    pthread_mutex_lock (dBuf.condMutex);
    while (1) {
        if (dBuf.status[curInx] == DBUF_HAVE_DATA) {
           break;
        } else if (dBuf.status[0] == DBUF_ALL_DONE) {
           break;
        } else {
           if (dBuf.length[curInx] <= 0)
               break;
           pthread_cond_wait (dBuf.myCondition, dBuf.condMutex);
        }
    }
    dBuf.operation = DONE_OPR;
    dBuf.status[0] = dBuf.status[1] = DBUF_ALL_DONE;
    pthread_cond_signal (dBuf.myCondition);
    pthread_mutex_unlock (dBuf.condMutex);

    /* cancel the relay thread */
    pthread_cancel (relayThread);
    (void) pthread_join(relayThread, NULL);
    sendDoneToPortal (dBuf.fileFd);
    if (dBuf.buf[0])
        (void) free(dBuf.buf[0]);
    if (dBuf.buf[1])
        (void) free(dBuf.buf[1]);

    return (bytesCopied);
}
#endif /* PARA_OPR */

srb_long_t
portalToSrbCopySB (int srcFd, int destExfInx, int destTypeInx, srb_long_t size)
{
    srb_long_t totalWritten = 0;
    int nbytes = 0;
    int byteWritten = 0;
    srb_long_t bytesCopied = 0;
    char *buf;

    buf = malloc (BUF_SIZE);

    while (bytesCopied < size) {
        int toread;
	int bytesRead;

        if (bytesCopied + BUF_SIZE > size) {
	    toread = size - bytesCopied;
        } else {
            toread = BUF_SIZE;
        }

	bytesRead = readFromPortal (srcFd, buf, bytesCopied, toread);

	if (bytesRead == toread) {
            if ((byteWritten = _objWrite (
	     destExfInx, destTypeInx, buf, bytesRead)) != bytesRead) {
                elog (NOTICE,
                 "svrDataPut: Bytes written %d don't match read %d", 
                  byteWritten, bytesRead);
                if (byteWritten < 0) {
                    bytesCopied = byteWritten;
                } else {
                    bytesCopied = OBJ_ERR_COPY_LEN;
                }
                break;
            }
            bytesCopied += byteWritten;
        } else if (bytesRead < 0) {
            bytesCopied = bytesRead;
	    break;
        } else {        /* toread > 0 */
            elog (NOTICE,
             "svrDataPut: To read %d bytes, %d bytes read from client",
               toread, bytesRead);
            bytesCopied = OBJ_ERR_COPY_LEN;
	    break;
	}
    }		/* while loop */
    free (buf);
    sendDoneToPortal (srcFd);
    return (bytesCopied);
}

srb_long_t *
svrObjGet (struct varlena *vSrcObjID, struct varlena *vSrcCollection,
struct varlena *vClHostAddr, int clPort)
{
    char *srcObjID;
    char *srcCollection;
    char *clHostAddr;
    srb_long_t *retVal;

    retVal = (srb_long_t *) malloc (2*SIZEOF32);

    if (TicketUserFlag) {
        elog (NOTICE, "svrObjPut: Illegal operation for a ticket user");
        *retVal = ILLEGAL_OPR_TICKET_USER;
        return (retVal);
    }

    srcObjID = varToStr (vSrcObjID);
    srcCollection = varToStr (vSrcCollection);
    clHostAddr = varToStr (vClHostAddr);

    *retVal = _svrObjGet (srcObjID, srcCollection, clHostAddr, clPort,
     0, 0, NULL);

    free (srcObjID);
    free (srcCollection);
    free (clHostAddr);

    return (retVal);
}

struct varlena *
svrObjGetC (struct varlena *vSrcObjID, struct varlena *vSrcCollection,
int flag, int numThreads, svrComm_t *myComm)
{
    char *srcObjID;
    char *srcCollection;
    char *clHostAddr;
    struct varlena * retVal;
    srb_long_t status;
    int sstatus;

    srcObjID = varToStr (vSrcObjID);
    srcCollection = varToStr (vSrcCollection);

    status = _svrObjGet (srcObjID, srcCollection, NULL, -1, 
     flag | CL_INIT_CONN_FLAG, numThreads, myComm);

    free (srcObjID);
    free (srcCollection);

    if (status >= 0) {
	sstatus = 0;
    } else {
	sstatus = status;
    }
    retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    VARSIZE(retVal) = VAROUTHDRSZ;
    VAROUTSTAT(retVal) = htonl (sstatus);

    return retVal;
}


srb_long_t
_svrObjGet (char *srcObjID, char *srcCollection, char *clHostAddr, int clPort,
int flag, int numThreads, svrComm_t *myComm)
{
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead, *infoOut, *tmpInfoOut;
    srb_long_t retVal;
    char *fullPathName = NULL;
    srb_long_t  status;
    int srcFd, destFd;

    infoInp.domainName = ClientUser->domainName;
    infoInp.accessMode = openFlagLookUp (O_RDONLY);
    infoInp.objID = srcObjID;
    infoInp.collectionName = srcCollection;
    infoInp.userName = ClientUser->userName;
    infoInp.domainName = ClientUser->domainName;

    /* Check with MDAS for the requested data */
    if ((retVal = mdasGetInfo (&infoInp, &infoOutHead)) < 0) {
	return (retVal);
    }

    if (infoOutHead->containerName[0] != '\0') {	
	/* container or in container obj */
	srcFd = _svrObjOpenWithInfo (&infoInp, &infoOutHead, O_RDONLY);
	if (srcFd < 0) {
	    return srcFd;
	}

	if (flag & CL_INIT_CONN_FLAG) {
	    int lsock, lport;

	    lsock = setupSrvPortal (myComm, &lport);
	    if (lsock < 0) {
		svrObjClose (srcFd);
		return (lsock);
	    }
	    destFd = acceptSrvPortal (lsock, lport);
#ifdef _WIN32
            closesocket(lsock);
#else
            close (lsock);
#endif
	    if (destFd < 0) {
		svrObjClose (srcFd);
		return (destFd);
	    }
	    status = write1stHeaderToPortal (destFd,
	     srbObjDesc[srcFd].objSize, 1);
	    if (status < 0) {
		svrObjClose (srcFd);
		return (status); 
	    }
	} else {
    	    destFd = portalConnect (clPort, clHostAddr);
            if (destFd < 0) {
                svrObjClose (srcFd);
                return (destFd);
            }
	}
	retVal = srbToPortalCopy (srcFd, destFd);
#ifdef _WIN32
        closesocket(destFd);
#else
        close (destFd);
#endif
        svrObjClose (srcFd);

	if (retVal >= 0 && retVal != infoOutHead->size)
	    retVal = OBJ_ERR_COPY_LEN;
    } else {

    	/* Will do a _svrObjGet */

    	sortAndCombInfo (&infoOutHead);

    	resolveInfoPathName (srcObjID, srcCollection, infoOutHead);

    	infoOut = infoOutHead;
    	while (infoOut != NULL) {
    	    struct mdasInfoOut *myInfoOut;

	    myInfoOut = resolveCompInfo (infoOut, srcCollection, 1);
	    if (myInfoOut == NULL) {
		retVal = TAPLIB_INTERNAL_ERROR;
		infoOut = infoOut->next;
                continue;
            }

            retVal = _svrDataGet (myInfoOut->objTypeInx, myInfoOut->dataPath, 
             myInfoOut->resourceLoc, clHostAddr, clPort, myInfoOut->size,
	     flag, numThreads, myComm);

	    if (myInfoOut != infoOut)	/* a compound obj */
		freeAllInfoOut (myInfoOut);

            if (retVal >= 0) {
	    	if (retVal != infoOutHead->size) 
            	    retVal = OBJ_ERR_COPY_LEN;
	    	break;
	    }
	    infoOut = infoOut->next;
	}
    }

    freeAllInfoOut (infoOutHead);
    return (retVal);
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
            infoOut->dataPath = malloc (HUGE_STRING);
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

srb_long_t
srbToPortalCopy (int srcFd, int destFd)
{
    char *buf;
    struct relayHeader myHeader;
    int status;
    int nbytes = 0;
    int bytesRead = 0;
    srb_long_t bytesCopied = 0;

    buf = malloc (BUF_SIZE);

    while ((bytesRead = _svrObjRead (srcFd, buf, BUF_SIZE)) > 0) {

        status = writeToPortal (destFd, buf, bytesCopied, bytesRead);

        if (status > 0) {
            bytesCopied += bytesRead;
        } else {
            bytesCopied = status;
            break;
        }
    }   /* while loop */

    sendDoneToPortal (destFd);

    free (buf);
    return (bytesCopied);
}

/* The is the same as srbToPortalCopy except _objRead is used.
 * The is a single buffer version.
 */

srb_long_t
srbToPortalCopySB (int destFd, int srcExfInx, int srcTypeInx)
{
    char *buf;
    struct relayHeader myHeader;
    int status;
    int nbytes = 0;
    int bytesRead = 0;
    srb_long_t bytesCopied = 0;

    buf = malloc (BUF_SIZE);

    while ((bytesRead = _objRead (srcExfInx, srcTypeInx, buf, BUF_SIZE)) > 0) {

        status = writeToPortal (destFd, buf, bytesCopied, bytesRead);

        if (status > 0) {
            bytesCopied += bytesRead;
        } else {
            bytesCopied = status;
            break;
        }
    }   /* while loop */

    sendDoneToPortal (destFd);

    free (buf);
    return (bytesCopied);
}

#ifdef PARA_OPR
/* The is the same as srbToPortalCopy except _objRead is used.
 * The is a double buffer version.
 */

srb_long_t
srbToPortalCopyDB (int destFd, int srcExfInx, int srcTypeInx)
{
    pthread_t relayThread;
    struct dBuffer dBuf;
    int curInx, compInx;
    pthread_mutex_t condMutex;
    pthread_cond_t  myCondition;

    struct relayHeader myHeader;
    int status;
    int nbytes = 0;
    int bytesRead = 0;
    srb_long_t bytesCopied = 0;

    initDBuffer (&dBuf, &condMutex, &myCondition, NULL);
    dBuf.operation = GET_OPR;
    dBuf.fileFd = destFd;

    dBuf.buf[0] = malloc(BUF_SIZE);
    if (!dBuf.buf[0]) {
        return (SYS_ERR_MALLOC);
    }
    dBuf.buf[1] = malloc(BUF_SIZE);
    if (!dBuf.buf[1]) {
        free (dBuf.buf[0]);
        return (SYS_ERR_MALLOC);
    }

    pthread_create(&relayThread, pthread_attr_default,
     (void *(*)(void *)) readRelay, (void *) &dBuf);

    curInx = dBuf.curInx;
    compInx = curInx ^ 1;

    while ((bytesRead = _objRead (srcExfInx, srcTypeInx, 
     dBuf.buf[dBuf.curInx], BUF_SIZE)) > 0) {

	/* relay the data to client */
        pthread_mutex_lock (dBuf.condMutex);
        while (1) {
            if (dBuf.status[compInx] == DBUF_NO_DATA) {  /* The other done ? */
                dBuf.offset[curInx] = bytesCopied;
                dBuf.length[curInx] = bytesRead;
                dBuf.status[curInx] = DBUF_HAVE_DATA;
                /* reset the inx */
                dBuf.curInx = curInx = compInx;
 		compInx = curInx ^ 1;
                break;
            } else if (dBuf.status[compInx] == DBUF_ALL_DONE) {
                break;
            } else if (dBuf.status[compInx] < 0) {
		bytesCopied = dBuf.status[compInx];
                break;
            } else {
                pthread_cond_wait (dBuf.myCondition, dBuf.condMutex);
            }
        }
        pthread_cond_signal (dBuf.myCondition);
        pthread_mutex_unlock (dBuf.condMutex);

	if (dBuf.status[compInx] == DBUF_ALL_DONE || dBuf.status[compInx] < 0)
	    break;
        bytesCopied += bytesRead;

    }   /* while loop */

    curInx = dBuf.curInx;
    compInx = curInx ^ 1;

     pthread_mutex_lock (dBuf.condMutex);
     while (1) {
        if (dBuf.status[compInx] == DBUF_NO_DATA) {
	    dBuf.operation = DONE_OPR;
	    dBuf.status[compInx] = DBUF_ALL_DONE;
            break;
        } else if (dBuf.status[compInx] == DBUF_ALL_DONE) {
            break;
        } else {
            pthread_cond_wait (dBuf.myCondition, dBuf.condMutex);
        }
     }
     pthread_cond_signal (dBuf.myCondition);
     pthread_mutex_unlock (dBuf.condMutex);

     /* cancel the relay thread */
     pthread_cancel (relayThread);
     (void) pthread_join(relayThread, NULL);
     sendDoneToPortal (dBuf.fileFd);
     close (dBuf.fileFd);        /* a portal */
     if (dBuf.buf[0])
         (void) free(dBuf.buf[0]);
     if (dBuf.buf[1])
         (void) free(dBuf.buf[1]);

    return (bytesCopied);
}
#endif /* PARA_OPR */

int
writeToPortal (int destFd, char *buf, srb_long_t offset, int length)
{
    struct relayHeader myHeader;
    int status;

    myHeader.operation = htonl (GET_OPR);
    myHeader.offset = offset;
    srbHtonll (&myHeader.offset);
    myHeader.length = htonl (length);

    status = write (destFd, (void *) &myHeader, sizeof (myHeader));

    if (status <= 0) {
        elog (NOTICE,
         "portalWrite: error writing header to client, fd = %d, errno = %d", 
	  destFd, errno);
        return (SYS_ERR_REMOTE_CONN);
    }
    /* now the data */
    status = write (destFd, buf, length);

    if (status <= 0) {
        elog (NOTICE,
         "portalWrite: error writing data to client, errno = %d", errno);
        return (SYS_ERR_REMOTE_CONN);
    }

    return (status);
}

int
readFromPortal (int srcFd, char *buf, srb_long_t offset, int length)
{
    struct relayHeader myHeader;
    int retVal;
    int nbytes, toread;
    char *bufptr;

    myHeader.operation = htonl (PUT_OPR);
    myHeader.offset = offset;
    srbHtonll (&myHeader.offset);
    myHeader.length = htonl (length);

    retVal = write (srcFd, (void *) &myHeader, sizeof (myHeader));

    if (retVal <= 0) {
        elog (NOTICE,
         "readFromPortal: error writing header to client, status = %d", retVal);
        return (SYS_ERR_REMOTE_CONN);
    }

    toread = length;
    bufptr = buf;
    do {
        nbytes = read (srcFd, bufptr, toread);
        if (nbytes > 0) {
            toread -= nbytes;
            bufptr += nbytes;
        }
    } while (toread > 0 && nbytes > 0);

    return (length - toread);
}

int
sendDoneToPortal (int destFd)
{
    struct relayHeader myHeader;
    int status;

    myHeader.operation = htonl (DONE_OPR);
    myHeader.offset = 0;
    srbHtonll (&myHeader.offset);
    myHeader.length = htonl (0);

    status = write (destFd, (void *) &myHeader, sizeof (myHeader));

    return (status);
}

srb_long_t *
svrDataGet (int srcTypeInx, struct varlena *vSrcPath, 
struct varlena *vSrcResLoc, struct varlena *vClHostAddr, int clPort,
srb_long_t *size)
{
    char *srcPath;
    char *srcResLoc;
    char *clHostAddr;
    srb_long_t *retVal;
    srb_long_t mySize;

    retVal = (srb_long_t *) malloc (2*SIZEOF32);

    if (ProxyUser->privUserFlag <= 0) {
        elog (NOTICE, "svrDataGet: User has no privilege for this operation");
	*retVal = AUTH_ERR_PROXY_NOPRIV;
        return (retVal);
    }

    if (TicketUserFlag) {
        elog (NOTICE, "svrObjPut: Illegal operation for a ticket user");
        *retVal = ILLEGAL_OPR_TICKET_USER;
        return (retVal);
    }

    srcPath = varToStr (vSrcPath);
    srcResLoc = varToStr (vSrcResLoc);
    clHostAddr = varToStr (vClHostAddr);
    mySize = *size;

    *retVal = _svrDataGet (srcTypeInx, srcPath, srcResLoc, clHostAddr,
     clPort, mySize, 0, 0, NULL);

    free (srcPath);
    free (srcResLoc);
    free (clHostAddr);

    return (retVal);    
}

struct varlena *
svrDataGetC (int srcTypeInx, struct varlena *vSrcPath, 
struct varlena *vSrcResLoc, srb_long_t *size, int flag, int numThreads,
svrComm_t *myComm)
{
    char *srcPath;
    char *srcResLoc;
    srb_long_t status;
    int sstatus;
    struct varlena * retVal;

    srb_long_t mySize;

    if (ProxyUser->privUserFlag <= 0) {
        elog (NOTICE, "svrDataGetC: User has no privilege for this operation");
	sstatus = AUTH_ERR_PROXY_NOPRIV;
        retVal = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (sstatus);
        return (retVal);
    }

    srcPath = varToStr (vSrcPath);
    srcResLoc = varToStr (vSrcResLoc);
    mySize = *size;

    status = _svrDataGet (srcTypeInx, srcPath, srcResLoc, NULL,
     0, mySize, flag | CL_INIT_CONN_FLAG, numThreads, myComm);

    free (srcPath);
    free (srcResLoc);

    if (status >= 0) {
	sstatus = 0;
    } else {
        sstatus = status;
    }
    retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    VARSIZE(retVal) = VAROUTHDRSZ;
    VAROUTSTAT(retVal) = htonl (sstatus);

    return (retVal);    
}

srb_long_t
_svrDataGet (int srcTypeInx, char *srcPath, char *srcResLoc,
char *clHostAddr, int clPort, srb_long_t size, int flag, int numThreads, 
svrComm_t *myComm)
{
    srb_long_t retVal;
    int destFd = -1;	/* XXXXX don't close 0 fd */
    int srcExfInx;
    int srcType;
    char *portNum;
    struct hostElement  *hostTabPtr;
    struct relayHeader myHeader;
    int remoteFlag;
    int status;
    srb_long_t totalWritten = 0;
    int nbytes = 0;
    int byteWritten = 0;
    char *tmpResLoc;
    portList_t *myPortList;

    /* see if we need to do it remotely */

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (srcResLoc, &hostTabPtr);
    portNum = NULL;
#else
    /* parseAddr change the content of ResLoc */
    tmpResLoc = strdup (srcResLoc);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
    free (tmpResLoc);
#endif

    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", srcResLoc);
        return INP_ERR_HOST_ADDR;               /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }
	if (flag & CL_INIT_CONN_FLAG) {
	    srbResult *res;
	    char mybuf[10*MAX_TOKEN];
	    int actualResultLen;

            retVal = _srbDataGetC (hostTabPtr->conn, srcTypeInx, srcPath,
             srcResLoc, size, flag, numThreads, &myPortList);
	    if (retVal >= 0) {
	        retVal = sendPortList (myComm, myPortList);
		free (myPortList);
	        /* read the extra transfer status */
                res = (srbResult *) clReadServerReturn (hostTabPtr->conn, 
		 mybuf, &actualResultLen, 0);

                if ((status = clResultStatus(res)) < 0) {
		    retVal = status;
                } else {
		    /* set it to size so that calling routine is happy */
		    retVal = size;
		}
                clClear(res);
	    }
	} else {
            retVal = _srbDataGet (hostTabPtr->conn, srcTypeInx, srcPath,
             srcResLoc, clHostAddr, clPort, size);
	}
        return retVal;
    }

    srcType = srbObjTypeEntry[srcTypeInx].objType;

#ifdef PARA_OPR
    if ((flag & CL_INIT_CONN_FLAG) == 0 && srcType == FILE_TYPE &&
     (srbObjTypeEntry[srcTypeInx].systemType == HPSS_FT)) {
        retVal = paraHPSSGet (srcPath, srcResLoc,
         srbObjTypeEntry[srcTypeInx].systemType, clHostAddr, clPort);
    } else if ((size <= SizePerThread && numThreads <= 1) || 
     srcType != FILE_TYPE || size == 0) {  
	/* too small for double buffer */
        srcExfInx = lowLevelOpen (srcTypeInx, srcPath, srcResLoc, O_RDONLY);

        if (srcExfInx < 0) {   /* error */
            elog (NOTICE, "_svrDataGet: Unable to open the src copy");
            return srcExfInx;
        }
#ifdef foo
	if (flag & CL_INIT_CONN_FLAG && size > 0) {
#endif
	if (flag & CL_INIT_CONN_FLAG) {
            int lsock, lport;

            lsock = setupSrvPortal (myComm, &lport);
            if (lsock < 0) {
                lowLevelClose (srcTypeInx, srcExfInx);
                return (lsock);
            }
            destFd = acceptSrvPortal (lsock, lport);
#ifdef _WIN32
            closesocket(lsock);
#else
            close (lsock);
#endif
            if (destFd < 0) {
                lowLevelClose (srcTypeInx, srcExfInx);
                return (destFd);
            }
            status = write1stHeaderToPortal (destFd, size, 1);
            if (status < 0) {
                lowLevelClose (srcTypeInx, srcExfInx);
                return (status);
            }
	} else {
            destFd = portalConnect (clPort, clHostAddr);
            if (destFd < 0) {   /* error */
                /* use fprintf intead of elog because multi error msg to client
                 * can cause core dump */
                fprintf (stderr, 
	         "_srbDataPut: Unable to connect to port %d at %s\n",
                 clPort & 0xffff, clHostAddr);
                lowLevelClose (srcTypeInx, srcExfInx);
                return destFd;
            }
 	}
        retVal = srbToPortalCopySB (destFd, srcExfInx, srcTypeInx);
        close (destFd);

        lowLevelClose (srcTypeInx, srcExfInx);
    } else {
        /* do parallel transfer */
	retVal = srbToPortalCopyP (srcTypeInx, srcPath, srcResLoc,
	  clHostAddr, clPort, size, flag, numThreads, myComm);
    }

#else

    srcExfInx = lowLevelOpen (srcTypeInx, srcPath, srcResLoc, O_RDONLY);

    if (srcExfInx < 0) {   /* error */
        elog (NOTICE, "_svrDataGet: Unable to open the src copy");
        return srcExfInx;
    }

    destFd = portalConnect (clPort, clHostAddr);

    if (destFd < 0) {   /* error */
        /* use fprintf intead of elog because multi error msg to client
         * can cause core dump */
        fprintf (stderr, "_srbDataPut: Unable to connect to port %d at %s\n",
         clPort & 0xffff, clHostAddr);
        return destFd;
    }

    retVal = srbToPortalCopySB (destFd, srcExfInx, srcTypeInx);
	
    close (destFd);

    lowLevelClose (srcTypeInx, srcExfInx);
#endif  /* PARA_OPR */

    if (retVal >= 0 && retVal != size)
	retVal = OBJ_ERR_COPY_LEN;

    return (retVal);
}

#ifdef PARA_OPR
srb_long_t
portalToSrbCopyP (int destTypeInx, char *destPath, char *destResLoc,
char *clHostAddr, int clPort, srb_long_t size, int flag, int inpNumThreads,
svrComm_t *myComm)
{
    int i, numThreads;
    struct paraPutGetInput myInput[MAX_THREAD];
    pthread_t tid[MAX_THREAD];
    srb_long_t myOffset, retval;
    srb_long_t size0, size1;
    srb_long_t status;
    srb_long_t retVal;
    pthread_mutex_t myLock;
    int lsock = -1;	/* XXXXXX don't close zero fd */
    int srcFd = -1;
    int lport;

    if (flag & CL_INIT_CONN_FLAG == 0) {
        inpNumThreads = 0;
    }
    numThreads = getNumThreadBySize (size, inpNumThreads);

    if (flag & CL_INIT_CONN_FLAG) {
        lsock = setupSrvPortal (myComm, &lport);
        if (lsock < 0) {
            return (lsock);
        }
        /* accept the first connection */
        srcFd = acceptSrvPortal (lsock, lport);
        if (srcFd < 0) {
#ifdef _WIN32
            closesocket(lsock);
#else
            close (lsock);
#endif
            return (srcFd);
        }
        status = write1stHeaderToPortal (srcFd, size, numThreads);
        if (status < 0) {
#ifdef _WIN32
            closesocket(lsock);
#else
            close (lsock);
#endif
            return (status);
        }
    }

    if (numThreads <= 0) {
        size0 = size1 = 0;
    } else {
        size0 = size / numThreads;
        size1 = size - size0 * (numThreads - 1);
    }

    myOffset = 0;

    pthread_mutex_init (&myLock, pthread_mutexattr_default);

    ParaCreateFlag = 0;
    for (i = 0; i < numThreads; i++) {
        if (flag & CL_INIT_CONN_FLAG) {
            if (i > 0) {
                srcFd = acceptSrvPortal (lsock, lport);
                if (srcFd < 0) {
#ifdef _WIN32
                    closesocket(lsock);
#else
                    close (lsock);
#endif
                    return (srcFd);
                }
            }
            myInput[i].portalFd = srcFd;
        }
        myInput[i].flag = flag;
	myInput[i].srbTypeInx = destTypeInx;
	myInput[i].srbPath = destPath;
	myInput[i].srbResLoc = destResLoc;
	myInput[i].clHostAddr = clHostAddr;
	myInput[i].clPort = clPort;
	myInput[i].offset = myOffset;
	myInput[i].threadInx = i;
	myInput[i].myLock = &myLock;
	if (i == numThreads - 1)
	    myInput[i].length = size1;
	else
	    myInput[i].length = size0;
	myOffset += myInput[i].length;

        pthread_create(&tid[i], pthread_attr_default, 
	 (void *(*)(void *)) partialDataPut, (void *) &myInput[i]);
    }

#ifdef _WIN32
    closesocket(lsock);
#else
    close (lsock);
#endif
    retVal = status = 0;
    for ( i = 0; i < numThreads; i++) {
        pthread_join (tid[i], NULL);
	if (myInput[i].status >= 0) {
	    retVal += myInput[i].status;
	} else {
	    status = myInput[i].status;
	}
    }
    if (status < 0)
	return status;
    else
	return retVal;
}

void 
partialDataPut (struct paraPutGetInput *myInput)
{
    int destExfInx, srcFd;
    char *buf;
    int nbytes = 0;
    int retryCnt = 0;
    int byteWritten;
    srb_long_t bytesCopied = 0;
    srb_long_t myOffset;

    pthread_mutex_lock (myInput->myLock);
    if (myInput->threadInx == 0) {
	ParaCreateFlag = 0;
        destExfInx = lowLevelCreate (myInput->srbTypeInx, myInput->srbResLoc,
	 myInput->srbPath, StorVaultMode, NO_CHKDIR, -1);
	if (destExfInx >= 0)
	    ParaCreateFlag = 1;
    } else {
        while (retryCnt < MAX_OPEN_RETRY * 3) {
           if (ParaCreateFlag == 0) {
                pthread_mutex_unlock (myInput->myLock);
                srbThreadSleep (0, 50000);
                retryCnt ++;
	        pthread_mutex_lock (myInput->myLock);
                continue;
            } else {
                break;
            }
	}
	if (retryCnt >= MAX_OPEN_RETRY * 3) {
            elog (NOTICE, "_partialDataPut: Creat timed out for %s thread %d",
             myInput->srbPath, myInput->threadInx);
            myInput->status = OBJ_ERR_CREATE_TIMEOUT;
            pthread_mutex_unlock (myInput->myLock);
            return;
        }

        destExfInx = lowLevelOpen (myInput->srbTypeInx, myInput->srbPath, 
         myInput->srbResLoc, O_WRONLY);
    }
    pthread_mutex_unlock (myInput->myLock);
    if (destExfInx < 0) {   /* error */
        elog (NOTICE, "partialDataPut: Unable to create the dest copy");
	myInput->status = destExfInx;
        return;
    }

    if (myInput->flag & CL_INIT_CONN_FLAG) {
        srcFd = myInput->portalFd;
    } else {
        srcFd = portalConnect (myInput->clPort, myInput->clHostAddr);
    }

    if (srcFd < 0) {   /* error */
        /* use fprintf intead of elog because multi error msg to client
         * can cause core dump */
        fprintf (stderr, 
	 "_partialDataPut: Unable to connect to port %d at %s\n",
         myInput->clPort & 0xffff, myInput->clHostAddr);
	lowLevelClose ( myInput->srbTypeInx, destExfInx);
	myInput->status = srcFd;
        return;
    }

    if (myInput->offset != 0) {
	myInput->status = _objSeek (destExfInx, myInput->srbTypeInx, 
	 myInput->offset, SEEK_SET);
    	if (myInput->status < 0) {
            elog(NOTICE, "_partialDataPut: _objSeek error, status = %d ",
             myInput->status);
	    lowLevelClose (myInput->srbTypeInx, destExfInx);
	    close (srcFd);
            return;
	}
    }

    buf = malloc (BUF_SIZE);

    myOffset = myInput->offset;

    while (bytesCopied < myInput->length) {
        int toread;
        int bytesRead;

        if (bytesCopied + BUF_SIZE > myInput->length) {
            toread = myInput->length - bytesCopied;
        } else {
            toread = BUF_SIZE;
        }

        bytesRead = readFromPortal (srcFd, buf, myOffset, toread);

        if (bytesRead == toread) {
            if ((byteWritten = _objWrite (
             destExfInx, myInput->srbTypeInx, buf, bytesRead)) != bytesRead) {
                elog (NOTICE,
                 "_partialDataPut: Bytes written %d don't match read %d",
                  byteWritten, bytesRead);
                if (byteWritten < 0) {
                    bytesCopied = byteWritten;
                } else {
                    bytesCopied = OBJ_ERR_COPY_LEN;
                }
                break;
            }
            bytesCopied += byteWritten;
	    myOffset += byteWritten;
        } else if (bytesRead < 0) {
            bytesCopied = bytesRead;
            break;
        } else {        /* toread > 0 */
            elog (NOTICE,
             "_partialDataPut: To read %d bytes, %d bytes read from client",
               toread, bytesRead);
            bytesCopied = OBJ_ERR_COPY_LEN;
            break;
        }
    }           /* while loop */
    free (buf);
    sendDoneToPortal (srcFd);
    lowLevelClose (myInput->srbTypeInx, destExfInx);
    close (srcFd);
    myInput->status = bytesCopied;
    return;
}

srb_long_t
srbToPortalCopyP (int srcTypeInx, char *srcPath, char *srcResLoc,
char *clHostAddr, int clPort, srb_long_t size, 
int flag, int inpNumThreads, svrComm_t *myComm)
{
    int i, numThreads;
    struct paraPutGetInput myInput[MAX_THREAD];
    pthread_t tid[MAX_THREAD];
    srb_long_t myOffset, retval;
    srb_long_t size0, size1;
    srb_long_t status;
    srb_long_t retVal;
    int lport;
    int lsock = -1;	/* XXXX be sure notclosing socket 0 */
    int destFd = -1;

    if (flag & CL_INIT_CONN_FLAG == 0) {
	inpNumThreads = 0;
    }
    numThreads = getNumThreadBySize (size, inpNumThreads);

    if (flag & CL_INIT_CONN_FLAG) {
        lsock = setupSrvPortal (myComm, &lport);
        if (lsock < 0) {
            return (lsock);
        }
	/* accept the first connection */
        destFd = acceptSrvPortal (lsock, lport);
        if (destFd < 0) {
#ifdef _WIN32
            closesocket(lsock);
#else
            close (lsock);
#endif
            return (destFd);
        }
        status = write1stHeaderToPortal (destFd, size, numThreads);
        if (status < 0) {
#ifdef _WIN32
            closesocket(lsock);
#else
            close (lsock);
#endif
            return (status);
        }
    }
    if (numThreads <= 0) {
	size0 = size1 = 0;
    } else {
        size0 = size / numThreads;
        size1 = size - size0 * (numThreads - 1);
    }

    myOffset = 0;

    for (i = 0; i < numThreads; i++) {
	if (flag & CL_INIT_CONN_FLAG) {
	    if (i > 0) {
                destFd = acceptSrvPortal (lsock, lport);
                if (destFd < 0) {
#ifdef _WIN32
                    closesocket(lsock);
#else
                    close (lsock);
#endif
                    return (destFd);
	        }
            }
	    myInput[i].portalFd = destFd;
	}
	myInput[i].flag = flag;
        myInput[i].srbTypeInx = srcTypeInx;
        myInput[i].srbPath = srcPath;
        myInput[i].srbResLoc = srcResLoc;
        myInput[i].clHostAddr = clHostAddr;
        myInput[i].clPort = clPort;
        myInput[i].offset = myOffset;
        myInput[i].threadInx = i;
        myInput[i].myLock = NULL;
        if (i == numThreads - 1)
            myInput[i].length = size1;
        else
            myInput[i].length = size0;
        myOffset += myInput[i].length;

        pthread_create(&tid[i], pthread_attr_default,
         (void *(*)(void *)) partialDataGet, (void *) &myInput[i]);
    }

    /* XXXXXX be sure not to close zero fd */
    if (lsock > 0)
#ifdef _WIN32
        closesocket(lsock);
#else
        close (lsock);
#endif

    retVal = status = 0; 
    for ( i = 0; i < numThreads; i++) {
        pthread_join (tid[i], NULL);
        if (myInput[i].status >= 0) {
            retVal += myInput[i].status;
        } else {
            status = myInput[i].status;
        }
    }
    if (status < 0)
        return status;
    else
        return retVal;
}

void
partialDataGet (struct paraPutGetInput *myInput)
{
    int srcExfInx, destFd = -1;
    char *buf;
    srb_long_t status;
    srb_long_t myOffset;
    srb_long_t bytesCopied = 0;


    srcExfInx = lowLevelOpen (myInput->srbTypeInx, myInput->srbPath, 
     myInput->srbResLoc, O_RDONLY);

    if (srcExfInx < 0) {   /* error */
        elog (NOTICE, "partialDataGet: Unable to open the src copy %s",
	 myInput->srbPath);
	myInput->status = srcExfInx;
        return;
    }

    if (myInput->flag & CL_INIT_CONN_FLAG) {
	destFd = myInput->portalFd;
    } else {
        destFd = portalConnect (myInput->clPort, myInput->clHostAddr);
    }

    if (destFd < 0) {   /* error */
        /* use fprintf intead of elog because multi error msg to client
         * can cause core dump */
        fprintf (stderr, 
	 "partialDataGet: Unable to connect to port %d at %s\n",
         myInput->clPort & 0xffff, myInput->clHostAddr);
        lowLevelClose (myInput->srbTypeInx, srcExfInx);
	myInput->status = destFd;
        return;
    }

    if (myInput->offset != 0) {
        myInput->status = _objSeek (srcExfInx, myInput->srbTypeInx,
         myInput->offset, SEEK_SET);
        if (myInput->status < 0) {
            elog(NOTICE, "_partialDataGet: _objSeek error, status = %d ",
             myInput->status);
            lowLevelClose (myInput->srbTypeInx, srcExfInx);
            close (destFd);
            return;
        }
    }

    buf = malloc (BUF_SIZE);

    myOffset = myInput->offset;

    while (bytesCopied < myInput->length) {
        int toread;
        int bytesRead;

        if (bytesCopied + BUF_SIZE > myInput->length) {
            toread = myInput->length - bytesCopied;
        } else {
            toread = BUF_SIZE;
        }

        bytesRead = _objRead (srcExfInx, myInput->srbTypeInx, buf, toread);
	if (bytesRead < 0) {
	    bytesCopied = bytesRead;
	    break;
	}

        status = writeToPortal (destFd, buf, myOffset, bytesRead);

        if (status > 0) {
            bytesCopied += bytesRead;
	    myOffset += bytesRead;
        } else {
            bytesCopied = status;
            break;
        }
    }   /* while loop */

    sendDoneToPortal (destFd);

    free (buf);

    close (destFd);
    lowLevelClose (myInput->srbTypeInx, srcExfInx);
    myInput->status = bytesCopied;
    
    return;
}
#endif /* PARA_OPR */


int
svrSyncData (int catType, struct varlena *vObjID, 
struct varlena *vCollectionName, struct varlena *vResource)
{
    char *objID, *collectionName, *resource;
    int status;

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    resource = varToStr (vResource);

    status = _svrSyncData (catType, objID, collectionName, resource);

    free (objID);
    free (collectionName);
    free (resource);

    return (status);
}

int
_svrSyncData (int catType, char *objID, char *collectionName, char *resource)
{
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead, *infoOut, *tmpInfoOut;
    struct mdasInfoOut *dirtyInfoOut, *cleanInfoOut;
    struct mdasResInfo *newResInfo, *tmpResInfo;
    int retVal;
    int savedRetVal = 0;
    char *fullPathName = NULL;
    struct mdasInfoOut *newInfoOut = NULL;
    char *condInput;
    char *tempStr;
    struct condResult condResult;
    struct resCache *myResCache;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user, status = %d",
          ILLEGAL_OPR_TICKET_USER);
        return (ILLEGAL_OPR_TICKET_USER);
    }

    infoInp.domainName = ClientUser->domainName;

    infoInp.accessMode = "write";

    infoInp.objID = objID;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = collectionName;

    /* Check with MDAS for the requested data */
     if ((retVal = mdasGetInfo (&infoInp, &infoOutHead)) < 0) 
	return retVal;

    if (resource != NULL && strlen (resource) > 0) {
        /* we have an input resource */
        condInput = malloc (strlen (objID) + 1);
        tempStr = strdup (objID);

        if (mySplitStr (tempStr, objID, condInput, '&') == NULL)
            condInput[0] = '\0';

        free (tempStr);


        if ((retVal = parseCondResInfo (collectionName, catType, condInput, 
	 resource, &condResult, &myResCache, infoOutHead->size)) < 0) {
            free (condInput);
            return (retVal);
        }
	newResInfo = myResCache->resInfoHead;

        free (condInput);

	matchRescWithInfo (newResInfo, infoOutHead);
    } else {
	newResInfo = NULL;
    }
    sortInfoForSyncD (&infoOutHead, &dirtyInfoOut, &cleanInfoOut);
    if (dirtyInfoOut == NULL && newResInfo == NULL) {
	/* nothing to do */
	freeAllInfoOut (cleanInfoOut);
	return (0);
    }

    /* update the clean copy */

    retVal = _objUpdate (catType, objID, collectionName,
     dirtyInfoOut, cleanInfoOut);

    if (retVal < 0) {
	freeAllInfoOut (cleanInfoOut);
	freeAllInfoOut (dirtyInfoOut);
	return (retVal);
    }

    /* replicate to newResInfo */

    if (dirtyInfoOut != NULL)
        tmpInfoOut = dirtyInfoOut;
    else
        tmpInfoOut = cleanInfoOut;

    tmpResInfo = newResInfo;

    while (tmpResInfo != NULL) {
        if (tmpResInfo->phyResMaxObjSize == MATCHED_INFO) {
            if (condResult.intResult == ALL) {
                tmpResInfo = tmpResInfo->next;
                continue;
            } else {
                break;
            }
        }

	retVal = _objReplicate (catType, objID, collectionName, 
	 tmpResInfo->phyResName, "", tmpInfoOut, &newInfoOut, tmpResInfo);

        if (retVal >= 0) {
            if (tmpInfoOut->isDirty > 0) {
                retVal = _svrModifyDataset (MDAS_CATALOG,
                 objID, collectionName, tmpResInfo->phyResName,
                  newInfoOut->dataPath, "1", NULL, D_CHANGE_DIRTY);
                if (retVal < 0) {
                    elog (NOTICE,
                     "_svrSyncData: _svrModifyDataset of %s error, status = %d",
                      objID, retVal);
                    break;
                }
            }
            if (condResult.intResult != ALL) {
                /* got a good copy */
                savedRetVal = retVal;
                break;
            }
	} else if (retVal < 0) {
	    savedRetVal = retVal;
	}
	tmpResInfo = tmpResInfo->next;
    }
    freeInfoOut (dirtyInfoOut);
    freeInfoOut (cleanInfoOut);
    freeInfoOut (newInfoOut);

    if (savedRetVal < 0)
	return (savedRetVal);
    else
	return (retVal);
}

int
svrBackupData (int catType, struct varlena *vObjID, 
struct varlena *vCollectionName, struct varlena *vResource, int flag)
{
    char *objID, *collectionName, *resource;
    int status;

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    resource = varToStr (vResource);

    status = _svrBackupData (catType, objID, collectionName, resource, flag);

    free (objID);
    free (collectionName);
    free (resource);

    return (status);
}

int
_svrBackupData (int catType, char *objID, char *collectionName, char *resource,
int flag)
{
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead;
    struct mdasResInfo *newResInfo;
    int retVal;
    char *condInput;
    struct condResult condResult;
    char *tempStr;
    struct resCache *myResCache;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user, status = %d",
          ILLEGAL_OPR_TICKET_USER);
        return (ILLEGAL_OPR_TICKET_USER);
    }

    infoInp.domainName = ClientUser->domainName;

    infoInp.accessMode = "write";

    infoInp.objID = objID;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = collectionName;

    /* Check with MDAS for the requested data */
     if ((retVal = mdasGetInfo (&infoInp, &infoOutHead)) < 0) 
	return retVal;

    if (resource != NULL && strlen (resource) > 0) {
        /* we have an input resource */
        condInput = malloc (strlen (objID) + 1);
        tempStr = strdup (objID);

        if (mySplitStr (tempStr, objID, condInput, '&') == NULL)
            condInput[0] = '\0';

        free (tempStr);


        if ((retVal = parseCondResInfo (collectionName, catType, condInput, 
	 resource, &condResult, &myResCache, infoOutHead->size)) < 0) {
            free (condInput);
            return (retVal);
        }

        free (condInput);
    } else {
	freeAllInfoOut (infoOutHead);
	return (RESOURCE_NOT_IN_CAT);
    }

    retVal = _svrBackupDataWithInfo (catType, objID, collectionName,
     flag, infoOutHead, myResCache->resInfoHead, condResult);

    return (retVal);
}

int
_svrBackupDataWithInfo (int catType, char *objID, char *collectionName,
int flag, struct mdasInfoOut *infoOutHead, struct mdasResInfo *newResInfo,
struct condResult condResult)
{
    struct mdasInfoOut *tmpInfoOut;
    struct mdasInfoOut *dirtyInfoOut, *cleanInfoOut;
    struct mdasResInfo *tmpResInfo;
    int retVal;
    char *fullPathName = NULL;
    struct mdasInfoOut *newInfoOut = NULL;
    char *tempStr;
    int savedRetVal = 0;
    int copyMade = 0;
    int condSatisfied = 0;
    struct mdasResInfo *matchedResInfo = NULL;

    sortInfoForSyncD (&infoOutHead, &dirtyInfoOut, &cleanInfoOut);
    if (dirtyInfoOut == NULL && newResInfo == NULL) {
	/* nothing to do */
	freeAllInfoOut (cleanInfoOut);
	return (0);
    }

    tmpResInfo = newResInfo;

    while (tmpResInfo != NULL) {
	/* a dirty backup copy already ? */
        tmpInfoOut = dirtyInfoOut;
        while (tmpInfoOut != NULL) {
            if (strcmp
             (tmpInfoOut->resourceName, tmpResInfo->phyResName) == 0) {
		retVal = 0;
		matchedResInfo = tmpResInfo;
		condSatisfied = 1;
                break;
            }
            tmpInfoOut = tmpInfoOut->next;
	}
	if (condSatisfied > 0)
	    break;

	/* Is there a clean copy ? */
        tmpInfoOut = cleanInfoOut;
        while (tmpInfoOut != NULL) {
            if (strcmp
             (tmpInfoOut->resourceName, tmpResInfo->phyResName) == 0) {
	        if (dirtyInfoOut != NULL) {	
		    /* only want to update this copy */
		    struct mdasInfoOut *savInfoOut;

		    savInfoOut = tmpInfoOut->next;
		    tmpInfoOut->next = NULL;
    		    /* update the clean copy */

    		    retVal = _objUpdate (catType, objID, collectionName,
     		    dirtyInfoOut, tmpInfoOut);

		    tmpInfoOut->next = savInfoOut;
    		     if (retVal < 0) {
	    		savedRetVal = retVal;
			if (condResult.intResult == ALL) {
        		    freeAllInfoOut (cleanInfoOut);
        		    freeAllInfoOut (dirtyInfoOut);
        		     return (retVal);
			} else {
			    /* try the next res */
            		    tmpInfoOut = tmpInfoOut->next;
                	    continue;
			}
    		    } else {
                        copyMade ++;
			if (condResult.intResult != ALL) 
			    condSatisfied = 1;
			break;
		    }
		} else {
		    /* copy already in sync */
		    retVal = 0;
            	    if (condResult.intResult != ALL) {
			matchedResInfo = tmpResInfo;
			condSatisfied = 1;
            	    }
                    break;
		}
            }
            tmpInfoOut = tmpInfoOut->next;
        }
	if (condSatisfied > 0)
            break;
        tmpResInfo = tmpResInfo->next;
    }

    if (copyMade == 0 && savedRetVal >= 0 && condSatisfied == 0) {
	struct mdasInfoOut *srcInfoOut;

	if (dirtyInfoOut != NULL)
	    srcInfoOut = dirtyInfoOut;
	else
	    srcInfoOut = cleanInfoOut;
	tmpResInfo = newResInfo;
	while (tmpResInfo != NULL) {
	    retVal = _objReplicate (catType, objID, collectionName, 
	     tmpResInfo->phyResName, "", srcInfoOut, &newInfoOut, 
	      tmpResInfo);

            if (retVal >= 0) {
                if (srcInfoOut->isDirty > 0) {
                    retVal = _svrModifyDataset (MDAS_CATALOG,
                     objID, collectionName, tmpResInfo->phyResName,
                      newInfoOut->dataPath, "1", NULL, D_CHANGE_DIRTY);
                    if (retVal < 0) {
                        savedRetVal = retVal;
                        elog (NOTICE,
                         "_svrBackupDataWithInfo: _svrModifyDataset of %s error, status = %d",
                          objID, retVal);
                        break;
                    }
                }
                if (condResult.intResult != ALL) {
                    /* got a good copy */
                    savedRetVal = retVal;
                    break;
                }
	    } else {
	        savedRetVal = retVal;
		if (condResult.intResult == ALL)
		    break;
	    }
	    tmpResInfo = tmpResInfo->next;
	}
    }
    freeInfoOut (dirtyInfoOut);
    freeInfoOut (cleanInfoOut);
    freeInfoOut (newInfoOut);

    if (savedRetVal < 0 && condResult.intResult == ALL)
	return (savedRetVal);
    else
	return (retVal);
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
_objUpdate (int catType, char *objID, char *collection,
struct mdasInfoOut *dirtyInfoOut, struct mdasInfoOut *cleanInfoOut)
{
    struct mdasInfoOut *srcInfoOut, *destInfoOut;
    int retVal;
    int savedRetVal = 0;
    srb_long_t byteWritten = 0;
    char strSize[MAX_TOKEN];

    retVal = syncInfoToInfo (dirtyInfoOut, collection, cleanInfoOut,
      collection, 1);

    return (retVal);
}

int lowLevelCreate (int tinx, char *resourceLoc, 
char *fullPathName, int mode, int chkdirFlag, srb_long_t dataSize)
{
    int exfInx;

    if (tinx < 0) {
        elog(NOTICE,
         "lowLevelCreate: resourceLoc:%s not supported", resourceLoc);
        return (OBJ_ERR_RES_TYPE);
    }

    switch (srbObjTypeEntry[tinx].objType) {
      case FILE_TYPE:
        exfInx = _svrFileCreate (srbObjTypeEntry[tinx].systemType,
          resourceLoc, fullPathName, mode, 
            chkdirFlag, dataSize, GENERIC_CONN);
        break;
      case DB_LOBJ_TYPE:
        exfInx = _dbLobjCreate (srbObjTypeEntry[tinx].systemType,
          resourceLoc, fullPathName, mode, SRB_SERVER_USER); 
        break;
      case DB_TABLE_TYPE:
        exfInx = _dbTableCreate (srbObjTypeEntry[tinx].systemType,
          resourceLoc, fullPathName, mode, SRB_SERVER_USER);
        break;

      default:              /* shouldn't get here */
        elog(NOTICE, 
	"lowLevelCreate: error in srbObjTypeEntry mapping");
        exfInx = OBJ_ERR_OBJ_TYPE;
        break;
    }
    return (exfInx);
}

srb_long_t
_svrDataCopyFromInfoToResc (int catType, char *objID, char *collectionName,
struct mdasInfoOut *srcInfo, 
struct mdasResInfo *newResInfo, char *newPathName, char *newFullPathName, 
int *newObjInx)
{
    srb_long_t totalWritten, size;
    int retVal = 0;
    char *userName;
    int status;
    struct mdasInfoOut *myInfoOut;
    struct mdasInfoOut *cacheInfo = NULL;
    int lockFd = -1;
    int zoneFlag;

    userName = ClientUser->userName;

    if (srcInfo->size <= 0) {
        size = SOURCE_EMPTY;
    } else {
        size = srcInfo->size;
    }

    *newObjInx = objTypeLookUp (newResInfo->phyResType);
    if (*newObjInx < 0) {
	elog (NOTICE,
	 "_svrDataCopyFromInfoToResc: Bad phyResType %s",
	 newResInfo->phyResType);
        return *newObjInx;
    }
    status = getFullPathName (newResInfo->logDirPathName,
                newResInfo->phyDirPathName,
                userName, ClientUser->domainName, newPathName, objID,
                collectionName, newFullPathName);

    if (status < 0) {
        elog(NOTICE,
         "_svrDataCopyFromInfoToResc: resolve pathName: %s", 
	 newPathName);
        return INP_ERR_FILENAME;
    }

    if (srbObjTypeEntry[srcInfo->objTypeInx].objType == COMPOUND_TYPE) {
        /* a compound object */
#ifdef LOCK_OBJ
            /* Don't lock container object. It has its own locking */
            if (srcInfo->containerName[0] == '\0') {
                lockFd =_svrObjLock (srcInfo->objID, collectionName,
                  F_SETLKW, F_RDLCK, 0);
            }
#endif
        retVal = _svrStageCompObj (srcInfo, collectionName, &cacheInfo, 1);
        if (retVal < 0) {
#ifdef LOCK_OBJ
            if (lockFd >= 0)
                _svrObjLock (srcInfo->objID, collectionName,
                  F_SETLKW, F_UNLCK, lockFd);
#endif
	    return retVal;
        }
        myInfoOut = cacheInfo;
    } else {
        myInfoOut = srcInfo;
    }

#ifdef FED_MCAT
    zoneFlag = getZoneFlag (srcInfo->collection, collectionName);
    if (zoneFlag < 0) {
#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (srcInfo->objID, collectionName,
              F_SETLKW, F_UNLCK, lockFd);
#endif
        if (cacheInfo != NULL)
            freeAllInfoOut (cacheInfo);
	return (zoneFlag);
    }
#else	/* FED_MCAT */
    zoneFlag = 0;
#endif	/* FED_MCAT */

    totalWritten = _svrDataCopy (*newObjInx, newFullPathName, 
     newResInfo->phyResLoc, myInfoOut->objTypeInx, 
     myInfoOut->dataPath, myInfoOut->resourceLoc, size, zoneFlag);

    if (totalWritten >= 0) {       /* success */
        if (size != SOURCE_EMPTY && totalWritten != size) {
            int tinx = *newObjInx;
#if defined(PORTNAME_osx)
            elog (NOTICE,
             "_svrDataCopyFromInfoToResc:copied size %d != MCAT %qd",
                totalWritten, size);
#else
            elog (NOTICE,
             "_svrDataCopyFromInfoToResc:copied size %lld != MCAT %lld",
                totalWritten, size);
#endif
            _objUnlink (srbObjTypeEntry[tinx].objType,
             srbObjTypeEntry[tinx].systemType, 
	     newResInfo->phyResLoc, newFullPathName, NO_CHKDIR);
            totalWritten = OBJ_ERR_COPY_LEN;
	}
    }
#ifdef LOCK_OBJ
    if (lockFd >= 0)
        _svrObjLock (srcInfo->objID, collectionName,
          F_SETLKW, F_UNLCK, lockFd);
#endif

    if (cacheInfo != NULL)
        freeAllInfoOut (cacheInfo);

    return (totalWritten);
}

srb_long_t
_svrDataPutToResc (int catType, char *objID, char *collectionName,
char *clHostAddr, int clPort, srb_long_t size,
struct mdasResInfo *newResInfo, char *newPathName, char *newFullPathName,
int *newObjInx, int flag, int numThreads, svrComm_t *myComm)
{
    srb_long_t totalWritten;
    int retVal = 0;
    char *userName;
    int status;


    userName = ClientUser->userName;

    *newObjInx = objTypeLookUp (newResInfo->phyResType);
    if (*newObjInx < 0) {
        elog (NOTICE,
         "_svrDataPutToResc: Bad phyResType %s",
         newResInfo->phyResType);
        return *newObjInx;
    }
    status = getFullPathName (newResInfo->logDirPathName,
                newResInfo->phyDirPathName,
                userName, ClientUser->domainName, newPathName, objID,
                collectionName, newFullPathName);

    if (status < 0) {
        elog(NOTICE,
         "_svrDataPutToResc: resolve pathName: %s",
         newPathName);
        return INP_ERR_FILENAME;
    }

    totalWritten = _svrDataPut (*newObjInx, newFullPathName,
     newResInfo->phyResLoc, clHostAddr, clPort, size, 
     flag, numThreads, myComm);

    if (totalWritten >= 0) {       /* success */
        int tinx = *newObjInx;
        if (size > 0 && totalWritten != size) {
            elog (NOTICE,
             "_svrDataPutToResc: copied size %lld not equal to MCAT %lld",
              totalWritten, size);
            _objUnlink (srbObjTypeEntry[tinx].objType,
             srbObjTypeEntry[tinx].systemType,
             newResInfo->phyResLoc, newFullPathName, NO_CHKDIR);
            totalWritten = OBJ_ERR_COPY_LEN;
        } else if (size > 0 && srbObjTypeEntry[tinx].objType == FILE_TYPE) {
            struct srbStat retStat;
            char *portNum, tmpResLoc[MAX_TOKEN];

	    strcpy (tmpResLoc, newResInfo->phyResLoc);

            portNum = parseAddr (tmpResLoc);

            if ((status = _svrFileStat (srbObjTypeEntry[tinx].systemType,
              tmpResLoc, portNum, newFullPathName, &retStat)) < 0) {
                elog(NOTICE,"_svrDataPutToResc:_svrFileStat failed. stat = %d",
                  status);
                _objUnlink (srbObjTypeEntry[tinx].objType,
                 srbObjTypeEntry[tinx].systemType,
                 newResInfo->phyResLoc, newFullPathName, NO_CHKDIR);
                totalWritten = OBJ_ERR_COPY_LEN;
            } else if (retStat.st_size != size) {
                elog (NOTICE,
                 "_svrDataPutToResc: stat size %lld not equal to MCAT %lld",
                  retStat.st_size, size);
                _objUnlink (srbObjTypeEntry[tinx].objType,
                 srbObjTypeEntry[tinx].systemType,
                 newResInfo->phyResLoc, newFullPathName, NO_CHKDIR);
                totalWritten = OBJ_ERR_COPY_LEN;
            }
        }
    }
    return (totalWritten);
}

int 
syncInfoToInfo (struct mdasInfoOut *inpSrcInfoOut, char *srcCollection,
struct mdasInfoOut *inpDestInfoOut, char *destCollection, int regDirty)
{
    struct mdasInfoOut *srcInfoOut, *destInfoOut;
    srb_long_t byteWritten;
    int retVal;
    int status = 0;
    char strSize[MAX_TOKEN];
    int zoneFlag;

    
    /* use the size to indicate whether a copy has been synced */
    destInfoOut = inpDestInfoOut;
    while (destInfoOut != NULL) {
        destInfoOut->size = -1;
        destInfoOut = destInfoOut->next;
    }

    srcInfoOut = inpSrcInfoOut;
    while (srcInfoOut != NULL) {
        struct mdasInfoOut *mySrcInfoOut;

        mySrcInfoOut = resolveCompInfo (srcInfoOut, srcCollection, 1);
	if (mySrcInfoOut == NULL) {
	    status = TAPLIB_INTERNAL_ERROR;
	    srcInfoOut = srcInfoOut->next;
	    continue;
	}

        /* sync to archInfoOut */
        destInfoOut = inpDestInfoOut;
        while (destInfoOut != NULL && destInfoOut->size < 0) {
	    struct mdasInfoOut *myDestInfoOut;

	    myDestInfoOut = resolveCompInfo (destInfoOut, destCollection, 0);
	    if (myDestInfoOut == NULL) {
		status = TAPLIB_INTERNAL_ERROR;
		destInfoOut = destInfoOut->next;
		continue;
	    }
		
            _objUnlink (srbObjTypeEntry[myDestInfoOut->objTypeInx].objType,
             srbObjTypeEntry[myDestInfoOut->objTypeInx].systemType,
             myDestInfoOut->resourceLoc, myDestInfoOut->dataPath, NO_CHKDIR);
#ifdef FED_MCAT
            zoneFlag = getZoneFlag (mySrcInfoOut->collection, 
	     myDestInfoOut->collection);
#else   /* FED_MCAT */
            zoneFlag = 0;
#endif  /* FED_MCAT */
	    if (zoneFlag >= 0) {
                byteWritten = _svrDataCopy (myDestInfoOut->objTypeInx,
                 myDestInfoOut->dataPath, myDestInfoOut->resourceLoc,
                  mySrcInfoOut->objTypeInx, mySrcInfoOut->dataPath,
                   mySrcInfoOut->resourceLoc, mySrcInfoOut->size, zoneFlag);
	    } else {
		byteWritten = zoneFlag;
	    }
            if (byteWritten >= 0) {
                if (byteWritten != mySrcInfoOut->size) {
                    int tinx = myDestInfoOut->objTypeInx;
#if defined(PORTNAME_osx)
        elog (NOTICE, "syncInfoToInfo:copied size %qd not equal to MCAT %qd",
         byteWritten, mySrcInfoOut->size);
#else
        elog (NOTICE,"syncInfoToInfo:copied size %lld not equal to MCAT %lld",
          byteWritten, mySrcInfoOut->size);
#endif
                    status = destInfoOut->size = OBJ_ERR_COPY_LEN;
                    _objUnlink (srbObjTypeEntry[tinx].objType,
                      srbObjTypeEntry[tinx].systemType,
                       myDestInfoOut->resourceLoc,
                        myDestInfoOut->dataPath, NO_CHKDIR);
                } else {
#if defined(PORTNAME_osx)
                    sprintf (strSize, "%-qd", byteWritten);
#else
                    sprintf (strSize, "%-lld", byteWritten);
#endif
                    if (destInfoOut != myDestInfoOut) {
                        /* A InternalCompObj. Register the new size */
			retVal = _svrModInternalCompObj (
			  myDestInfoOut->objID, destCollection,
			  myDestInfoOut->replNum, myDestInfoOut->segNum,
			  myDestInfoOut->intReplNum, myDestInfoOut->intSegNum,
			  ClientUser->userName, ClientUser->domainName,
			  strSize, "1", NULL, NULL,
			  D_COMPOUND_MODIFY_SIZE_AND_DIRTY);
                        freeInfoOut (myDestInfoOut);
			myDestInfoOut = NULL;
			if (retVal < 0) {
                            elog (NOTICE,
                            "syncInfoToInfo:Reg intComp size %s/%s err,stat=%d",
                              destCollection, destInfoOut->objID, retVal);
                            status = retVal;
                            destInfoOut = destInfoOut->next;
			    continue;
			}
                    } 

                    /* reg the new size and mark it dirty */
                    retVal = _svrModifyDataset (MDAS_CATALOG, 
		      destInfoOut->objID, destCollection, 
		      destInfoOut->resourceName, destInfoOut->dataPath,
                      strSize, NULL, D_CHANGE_SIZE);

                    if (retVal < 0) {
                        elog (NOTICE,
                         "syncInfoToInfo: Reg size for %s/%s error, status=%d",
                           destCollection, destInfoOut->objID, retVal);
                        status = retVal;
                        destInfoOut = destInfoOut->next;
                        continue;
                    } 
		    if (regDirty > 0) {
                        retVal = _svrModifyDataset (MDAS_CATALOG, 
                          destInfoOut->objID, destCollection,
                          destInfoOut->resourceName, destInfoOut->dataPath,
                          "1", NULL, D_CHANGE_DIRTY);
			if (retVal < 0) {
                            elog (NOTICE,
                             "syncInfoToInfo: Reg dirty for %s/%s err,stat=%d",
                               destInfoOut->objID, destCollection, retVal);
                            status = retVal;
                            destInfoOut = destInfoOut->next;
                            continue;
			}
                    }
                    destInfoOut->size = byteWritten;
                }
            } else {
                status = byteWritten;
            }
	    if (myDestInfoOut != NULL && destInfoOut != myDestInfoOut) {
		freeInfoOut (myDestInfoOut);
	    }
            destInfoOut = destInfoOut->next;
        }
        if (mySrcInfoOut != srcInfoOut) {
            freeInfoOut (mySrcInfoOut);
	    mySrcInfoOut = NULL;
	}
        if (status == 0)    /* all is well */
            break;
        else
            srcInfoOut = srcInfoOut->next;
    }

    return status;
}

/* genCompPath - Generate a fake pathName for the compound obj based on the
 * collection and objID of the object.
 */
int
genCompPath (char *outPath, char *objID)
{
    sprintf (outPath, "/%s.%ld.%ld", objID, random(), time(NULL));
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

int 
svrModifyRescInfo (int catType, struct varlena *vRescName, int retractionType,
struct varlena *vDataValue1, struct varlena *vDataValue2,
struct varlena *vDataValue3, struct varlena *vDataValue4)
{
    char *rescName;
    char *dataValue1;
    char *dataValue2;
    char *dataValue3;
    char *dataValue4;
    int retVal;

    if (ProxyUser->privUserFlag <= 0) {
        elog (NOTICE, 
	  "svrModifyRescInfo: User has no privilege for this operation");
        return (AUTH_ERR_PROXY_NOPRIV);
    }

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }
    rescName = varToStr (vRescName);
    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);
    dataValue3 = varToStr (vDataValue3);
    dataValue4 = varToStr (vDataValue4);

    retVal = _svrModifyRescInfo (catType, NULL, rescName, retractionType,
      dataValue1, dataValue2, dataValue3, dataValue4);

    free (rescName);
    free (dataValue1);
    free (dataValue2);
    free (dataValue3);
    free (dataValue4);

    return (retVal);
}

int
_svrModifyRescInfo (int catType, char *mcatName, char *rescName, 
int retractionType,
char *dataValue1, char *dataValue2, char *dataValue3, char *dataValue4)
{
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Do the modify_dataset_info */

    /* XXXXXXX - does not know the mcat name */
    remoteFlag = getMdasEnabledHost (mcatName, &hostTabPtr);

    if (remoteFlag < 0) {
        retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
	char *userName, *domainName;
	char resDeletedStr[MAX_TOKEN];
	int sizeMB;

	sizeMB = atoi (dataValue1);
	if (retractionType == R_INSERT_FREE_SPACE && 
	 sizeMB != RES_RESOURCE_DELETED && sizeMB != RES_RESOURCE_RESTART) {
	    userName = ThisUser->userName; 
	    domainName = ThisUser->domainName;
	} else {
	    userName = ClientUser->userName;
	    domainName = ClientUser->domainName;
	}
        retVal = modify_resource_info (catType, rescName, 
          userName, domainName,
	  retractionType, dataValue1, dataValue2, dataValue3, dataValue4);    
        if (retVal >= 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }

        /* Do a client call to the remote host */

        retVal = srbModifyRescInfo (hostTabPtr->conn, catType, rescName,
          retractionType, dataValue1, dataValue2, dataValue3, dataValue4);

        if (retVal < 0) {       /* Failure */
            elog (NOTICE, "srbModifyRescInfo failed: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }

    return retVal;
}

/*
 * svrRegisterLocation - Register Location
 *
 * Input - struct varlena *vLocName - location name
 *		  struct varlena *vFullAddr - full address
 *      	  struct varlena *vParentLoc - parent location
 *		  struct varlena *vServerUser - server user
 *                struct varlena *vServerUserDomain - server user domain
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
int
svrRegisterLocation(   struct varlena *vLocName, 
 		 struct varlena *vFullAddr, 
		 struct varlena *vParentLoc,
		 struct varlena *vServerUser,
                 struct varlena *vServerUserDomain)
{

    int status;

    char *locName;
    char *fullAddr;
    char *parentLoc;
    char *serverUser;
    char *serverUserDomain;

    locName = varToStr (vLocName);
    fullAddr = varToStr (vFullAddr);
    parentLoc = varToStr (vParentLoc);
    serverUser = varToStr (vServerUser);
    serverUserDomain = varToStr (vServerUserDomain);

    status = _svrRegisterLocation(locName, fullAddr, parentLoc, serverUser,
				  serverUserDomain);

    free (locName);
    free (fullAddr);
    free (parentLoc);
    free (serverUser);
    free (serverUserDomain);

    return status;
}

/*
 * See svrRegisterLocation above for description.
 */
int
_svrRegisterLocation( char *locName, 
 		 char *fullAddr, 
		 char *parentLoc,
		 char *serverUser,
                 char *serverUserDomain)
{
    int status;

    char *userName;
    char *userAuth;
    char *userDomain;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrRegisterLocation: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }
    userName = ClientUser->userName;
    userAuth = "";
    userDomain = ClientUser->domainName;

    /* XXXXXX - potential problem. Does not know mcatHost */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
       status = registerLocationInfo(locName, fullAddr, 
		parentLoc, serverUser, serverUserDomain,
		userName, userDomain,  userAuth);
       if (status == 0) {
	  commit_db2_interaction (MDAS_CAT_COMMIT);
       }
       else {
	  commit_db2_interaction (MDAS_CAT_ROLLBACK);
       }
#else
       status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
      status = remoteRegisterLocation(locName, fullAddr, parentLoc,
		    serverUser, serverUserDomain, hostTabPtr);
    }

    if (status != 0) {
        elog( NOTICE, 
	      "registerLocation: registerLocationInfo Error: %i",status);
    }

    return status;
}


/*
 * svrIngestToken - Ingest Token
 *
 * Input - struct varlena *vTypeName - type name
 *	   struct varlena *vNewValue - new value
 *         struct varlena *vParentValue - parent value
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
svrIngestToken(   struct varlena *vTypeName, 
		       struct varlena *vNewValue, 
		       struct varlena *vParentValue)
{

    int status;

    char *typeName;
    char *newValue;
    char *parentValue;

    typeName = varToStr (vTypeName);
    newValue = varToStr (vNewValue);
    parentValue = varToStr (vParentValue);

    status = _svrIngestToken(typeName, newValue, parentValue);

    free (typeName);
    free (newValue);
    free (parentValue);

    return status;
}

/*
 * See svrIngestToken above for description.
 */
int
_svrIngestToken(char *typeName, 
		char *newValue, 
		char *parentValue)
{
    int status;

    char *userName;
    char *userAuth;
    char *userDomain;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrIngestToken: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }
    userName = ClientUser->userName;
    userAuth = "";
    userDomain = ClientUser->domainName;

    /* XXXXXXX - potential problem. Does not know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
       status = insertTokenInfo(typeName, newValue, parentValue,
				 userName, userDomain,  userAuth);
       if (status == 0) {
	  commit_db2_interaction (MDAS_CAT_COMMIT);
       }
       else {
	  commit_db2_interaction (MDAS_CAT_ROLLBACK);
       }
#else
       status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
       status = remoteInsertTokenInfo(typeName, newValue, parentValue,
	hostTabPtr);
    }

    if (status != 0) {
        elog( NOTICE, 
	      "insertToken: insertTokenInfo Error: %i",status);
    }

    return status;
}

/*
 * svrRegisterResource
 *
 * Input -  struct varlena *vRescName - Resource Name
 *	    struct varlena *vRescType - Resource Type 
 *          struct varlena *vLocation - Resource Location
 *          struct varlena *vPhyPath  - Resource Physical Path
 *          struct varlena *vClass    - Resource Class name
 *         
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
int
svrRegisterResource (struct varlena *vRescName,
				struct varlena *vRescType,
				struct varlena *vLocation,
				struct varlena *vPhyPath,
				struct varlena *vClass,
				int size)
{
    int status;

    char *rescName;
    char *rescType;
    char *location;
    char *phyPath;
    char *class;

    rescName = varToStr (vRescName);
    rescType = varToStr (vRescType);
    location = varToStr (vLocation);
    phyPath = varToStr (vPhyPath);
    class   = varToStr (vClass);

    status = _svrRegisterResource(rescName, rescType, location, 
				  phyPath, class, size);

    free (rescName);
    free (rescType);
    free (phyPath);
    free (class);

    return status;
}

/*
 * See svrRegisterResource above for description.
 */
int
_svrRegisterResource(   char *rescName, char *rescType, char *location, 
			char *phyPath, char *class, int size)
{
    char *userName;
    char *userAuth;
    char *userDomain;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    int status;

    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrRegisterResource: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }
    userName = ClientUser->userName;
    userAuth = "";
    userDomain = ClientUser->domainName;

    /* XXXXXXX potential problem. does not know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
       status = registerResourceInfo(rescName, rescType, location,
	          userName, userDomain,  userAuth, 
                  phyPath, class, size);
       if (status == 0) {
	  commit_db2_interaction (MDAS_CAT_COMMIT);
       }
       else {
	  commit_db2_interaction (MDAS_CAT_ROLLBACK);
       }
#else
       status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
       status = remoteRegisterResourceInfo(rescName, rescType, 
                  location,
	          userName, userDomain,  userAuth, phyPath, class, size,
		  hostTabPtr);
    }


    if (status != 0) {
        elog( NOTICE, 
	      "RegisterResource: registerResourceInfo Error: %i",
	      status);
    }

    return status;   
}


/*
 * svrRegisterLogicalResource
 *
 * Input -  struct varlena *vLogRescName - Logical resource name
 *	    struct varlena *vRescType - Resource Type 
 *          struct varlena *vPhysRescName - Physical resource name
 *          struct varlena *vPhysPath - Physical path
 *         
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
svrRegisterLogicalResource(   struct varlena *vLogRescName, 
		       struct varlena *vRescType, 
		       struct varlena *vPhysRescName,
		       struct varlena *vPhysPath)
{

    int status;

    char *logRescName;
    char *rescType;
    char *physRescName;
    char *physPath;

    logRescName = varToStr (vLogRescName);
    rescType = varToStr (vRescType);
    physRescName = varToStr (vPhysRescName);
    physPath = varToStr (vPhysPath);

    status = _svrRegisterLogicalResource(logRescName,
              rescType, physRescName, physPath);

    free (logRescName);
    free (rescType);
    free (physRescName);
    free (physPath);

    return status;
}

/*
 * See svrRegisterLogicalResource above for description.
 */
int
_svrRegisterLogicalResource(   char *logRescName, 
		       char *rescType, 
		       char *physRescName,
		       char *physPath)
{
    char *userName;
    char *userAuth;
    char *userDomain;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    int status;

    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
       "svrRegisterLogicalResource: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }
    userName = ClientUser->userName;
    userAuth = "";
    userDomain = ClientUser->domainName;

    /* XXXXXXX potential problem. does not know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
         status = registerLogicalResourceInfo(logRescName, rescType, 
			   physRescName,
			   userName, userDomain,  userAuth, physPath);
	 if (status == 0) {
	    commit_db2_interaction (MDAS_CAT_COMMIT);
	 }
	 else {
	    commit_db2_interaction (MDAS_CAT_ROLLBACK);
	 }

#else
	 status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
         status = remoteRegisterLogicalResourceInfo (logRescName, rescType, 
		    physRescName,  physPath, hostTabPtr);
    }

    if (status != 0) {
        elog( NOTICE, 
	      "RegisterLogicalResource: registerLogicalResourceInfo Error: %i",
	      status);
    }

}

/*
 * svrRegisterReplicateResourceInfo
 *
 * Input -  struct varlena *vPhysRescName - Physical resource name
 *	    struct varlena *vRescType - Resource Type 
 *          struct varlena *vOldLogicalRescName - Old logical resource name
 *          struct varlena *vIndefaultPath - Indefault path
 *         
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */
int
svrRegisterReplicateResourceInfo(   struct varlena *vPhysRescName, 
		       struct varlena *vRescType, 
		       struct varlena *vOldLogicalRescName,
		       struct varlena *vIndefaultPath)
{

    int status;

    char *physRescName;
    char *rescType;
    char *oldLogRescName;
    char *indefaultPath;

    physRescName = varToStr (vPhysRescName);
    rescType = varToStr (vRescType);
    oldLogRescName = varToStr (vOldLogicalRescName);
    indefaultPath = varToStr (vIndefaultPath);

    status = _svrRegisterReplicateResourceInfo(physRescName, rescType, 
				      oldLogRescName, indefaultPath);

    free (physRescName);
    free (rescType);
    free (oldLogRescName);
    free (indefaultPath);

    return status;
}

/*
  See svrRegisterReplicateResourceInfo (above) for description.
 */
int
_svrRegisterReplicateResourceInfo(   char *physRescName, 
		       char *rescType, 
		       char *oldLogRescName,
		       char *indefaultPath)
{
    char *userName;
    char *userAuth;
    char *userDomain;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    int status;

    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrRegisterReplicateResourceInfo: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }
    userName = ClientUser->userName;
    userAuth = "";
    userDomain = ClientUser->domainName;

    /* XXXXXXXX potential problem - don't know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
         if (strcmp(indefaultPath, "compound") == 0) {
	   /* oldLogRescName is actually the Compound Resource name */
            status = registerCompoundResourceInfo(oldLogRescName, 
					     physRescName, 
		     userName, userDomain,  userAuth);
	 }
	 else {
            status = registerReplicateResourceInfo(physRescName, rescType, 
					     oldLogRescName,
		     userName, userDomain,  userAuth, indefaultPath);
	 }
	 if (status == 0) {
            commit_db2_interaction (MDAS_CAT_COMMIT);
	 }
	 else {
	    commit_db2_interaction (MDAS_CAT_ROLLBACK);
	 }
#else
        status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
        status = remoteRegisterReplicateResourceInfo (physRescName, rescType, 
		     oldLogRescName,  indefaultPath, hostTabPtr);
    }

    if (status != 0) {
        elog( NOTICE, 
	      "RegisterReplicateResourceInfo: registerReplicateResourceInfo Error: %i",
	      status);
    }
    return status;
}

/*
 * svrDeleteValue - Delete Value
 *
 * Input - int valueType - type of value being deleted
 *	   struct varlena *vDeleteValue - value (name) being deleted
 * Output - Returns 0 if successful.
 *          Returns a negative value upon failure.
 */

int
svrDeleteValue(   int valueType,
		  struct varlena *vDeleteValue)
{

    int status;

    char *deleteValue;

    deleteValue = varToStr (vDeleteValue);

    status = _svrDeleteValue(valueType, deleteValue);

    free (deleteValue);

    return status;
}

/*
 * See svrDeleteValue above for description.
 */
int
_svrDeleteValue(int valueType,
		char *deleteValue)
{
    int status;

    char *userName;
    char *userAuth;
    char *userDomain;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrDeleteValue: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }
    userName = ClientUser->userName;
    userAuth = "";
    userDomain = ClientUser->domainName;

    /* XXXXXX potential problem - don't know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
       status = deleteValueInfo(valueType, deleteValue);
       if (status == 0) {
	  commit_db2_interaction (MDAS_CAT_COMMIT);
       }
       else {
	  commit_db2_interaction (MDAS_CAT_ROLLBACK);
       }
#else
       status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
       status = remoteDeleteValue(valueType, deleteValue, hostTabPtr);
    }

    if (status != 0) {
        elog( NOTICE, 
	      "deleteValue: deleteValueInfo Error: %i",status);
    }

    return status;
}


/* svrSetupSessionPublicKey
 *
 * Input: nothing
 *
 * Output : success - Returns the current public key as struct varlena
 *	    failure - return NULL or negative,
 *                    or error message in struct varlena
 */

struct varlena *
svrSetupSessionPublicKey()
{
    struct varlena *retVal;
    char *cp;
    int len;

    cp = sscGetPublicKeyString();
    if (cp == NULL) return(NULL);

    len = strlen(cp);

    retVal = (struct varlena *)malloc(VAROUTHDRSZ + len + 1);
    VARSIZE(retVal) = len + VAROUTHDRSZ;
    strcpy (VAROUTDATA(retVal), cp);
    VAROUTSTAT(retVal) = htonl (0);

    return retVal;
    
}

    
/*
 * svrSetupSession - set up a session (for encryption) with the
 * MCAT-enabled server.
 * Also see the sscSetupSession library routine.
 *
 * Input - srbConn* conn - From srbConnect ().
 *	   struct varlena *vSessionKey - a string representation of 
 *              the session key from sscSetupSession (encrypted in the 
 *              public key)
 *
 * Output - Returns 0 or positive value upon success.
 *          Returns a negative value upon failure.
 */

int
svrSetupSession(struct varlena *vSessionKey)
{

    int status;

    char *sessionKey;

    sessionKey = varToStr (vSessionKey);

    status = _svrSetupSession(sessionKey);

    memset(sessionKey, 0, strlen(sessionKey));
    free (sessionKey);

    return status;
}

/*
 * See svrSetupSession above for description.
 */
int
_svrSetupSession(char *sessionKey)
{
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* XXXXXX potential problem - don't know mcatName */
    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
       status = sscReceiveSession(sessionKey);
#else
       status = CLI_FUNC_NOT_SUPPORTED;
#endif
    } 
    else {
       status = remoteSetupSession(sessionKey, hostTabPtr);
    }

    elog( NOTICE, 
	      "svrSetupSession: Status: %i",status);

    return status;
}


/* isResLocLocal - check whether the input resource loc is local
 * return 1 if it is local, otherwise zero
 */

int
isResLocLocal (char *resourceLoc)
{
    int remoteFlag;
    char *portNum;
    struct hostElement  *hostTabPtr;
    char tmpResLoc[MAX_TOKEN];

    if (resourceLoc == NULL)
	return (0);
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (resourceLoc, &hostTabPtr);
    portNum = NULL;
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

/* this is a special stage handling routine for standford */
int
_standfStage (int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName, struct mdasInfoOut *origInfoOut,
int regFlag)
{
    struct mdasResInfo *newResInfo;
    struct hostElement  *hostTabPtr;
    int remoteFlag;
    char *portNum, tmpResLoc[MAX_TOKEN];
    int retVal;
    struct resCache *myResCache;

    if (getResInfo (collectionName, catType, newResourceName, 
     ClientUser->domainName, &myResCache) < 0) {
        elog (NOTICE,
          "_standfStage: %s and %s is not a registered resource",
          newResourceName, ClientUser->domainName);
        return OBJ_ERR_RES_NOT_REG;
    }

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (myResCache->resInfoHead->phyResLoc, 
     &hostTabPtr);
    portNum = NULL;
#else
    strcpy (tmpResLoc, myResCache->resInfoHead->phyResLoc);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif

    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", 
	 myResCache->resInfoHead->phyResLoc);
        return INP_ERR_HOST_ADDR;               /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }
        retVal = srbObjReplicate (hostTabPtr->conn, catType, objID,
         /* XXXX should be collectionName, newResInfo->phyResName, newPathName); */
         collectionName, newResourceName, newPathName);
    } else {        /* A local call */
        char commandBuf[HUGE_STRING];
        struct mdasInfoOut *tmpInfoOut;
        int objTypeInx;

        retVal = 0;
        tmpInfoOut = origInfoOut;
        while (tmpInfoOut != NULL) {
            objTypeInx = tmpInfoOut->objTypeInx;
            if (srbObjTypeEntry[objTypeInx].objType != FILE_TYPE ||
             srbObjTypeEntry[objTypeInx].systemType != HPSS_FT) {
                tmpInfoOut = tmpInfoOut->next;
                continue;
            }

            sprintf (commandBuf, "%s/../%s/%s %s %s", (char *) getSrbDataDir(),
             COMMAND_DIR, STANDF_STAGE_CMD, tmpInfoOut->dataPath, newPathName);
            retVal = system (commandBuf);
            if (retVal != 0) {
                elog (NOTICE, "_standfStage: command %s failed, errno = %d",
                 STANDF_STAGE_CMD, errno);
                retVal = unixErrConv (errno);
            } else {
                 struct stat statbuf;

                retVal = stat (newPathName, &statbuf);
                if (retVal < 0) {
                    retVal = SYS_ERR_TRANSFER_FAILED;
                } else if (regFlag == STANDF_REG) {     /* reg the replica */
                    char tempObjID[MAX_TOKEN], condInput[MAX_TOKEN];

                    mySplitStr (objID, tempObjID, condInput, '&');
                    retVal = _svrRegisterReplica (catType, tempObjID,
                     collectionName, tmpInfoOut->resourceName,
                      tmpInfoOut->dataPath,
                      myResCache->resInfoHead->phyResName, newPathName,
		       0, 0, NULL,
                       ClientUser->userName, ClientUser->domainName);
                }
            }
            break;
        }
        if (retVal == 0 && tmpInfoOut == NULL) {
            retVal = HPSS_SRB_EEXIST;
        }
    }

    return retVal;
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

int
getNumThreadBySize (srb_long_t size, int inpNumThreads)
{
    int numThreads;

    if (inpNumThreads > 0) {
	numThreads = (int) (size / MIN_SIZE_PER_THREAD);
	if (numThreads > inpNumThreads) {
	    numThreads = inpNumThreads;
	} 
	if (numThreads > MaxThread) {
	    numThreads = MaxThread;
	} else if (numThreads <= 0 && size > 0) {
	    numThreads = 1;
	} 
    } else {
        numThreads = (int) (size / SizePerThread);

        if (numThreads <= 0) {
            numThreads = 1;
        } else if (numThreads > MaxThread) {
            numThreads = MaxThread;
        } else if (numThreads < MaxThread &&
          size - numThreads * SizePerThread > SizePerThread / 2) {
            numThreads ++;
	}
    }
    return numThreads;
}


int
svrModifyZone (int catType, struct varlena *vZoneName,
struct varlena *vDataValue1, struct varlena *vDataValue2,
struct varlena *vDataValue3, struct varlena *vDataValue4, 
struct varlena *vDataValue5, int actionType)
{
    char *zoneName;
    char *dataValue1;
    char *dataValue2;
    char *dataValue3;
    char *dataValue4;
    char *dataValue5;
    int retVal;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    zoneName = varToStr (vZoneName);
    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);
    dataValue3 = varToStr (vDataValue3);
    dataValue4 = varToStr (vDataValue4);
    dataValue5 = varToStr (vDataValue5);


    retVal = _svrModifyZone (catType, zoneName, dataValue1, 
    dataValue2, dataValue3, dataValue4, dataValue5, actionType);

    free (zoneName);
    free (dataValue1);
    free (dataValue2);
    free (dataValue3);
    free (dataValue4);
    free (dataValue5);

    return retVal;
}

int
_svrModifyZone (int catType, char *zoneName, char *dataValue1,
char *dataValue2, char *dataValue3, char *dataValue4, char *dataValue5, 
int actionType)
{
    int retVal;
    char *userName;
    char *userAuth;
    char *userDomain;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Do a modify_zone_info */
    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrModifyZone: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }
    userName = ClientUser->userName;
    userAuth = "";
    userDomain = ClientUser->domainName;

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = modify_zone_info (catType, userName, userDomain,
	  zoneName, dataValue1, dataValue2, dataValue3,
	  dataValue4, dataValue5, actionType);
	if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        retVal = remoteModifyZone (catType, zoneName,
          dataValue1, dataValue2, dataValue3, dataValue4, dataValue5, 
	  actionType, hostTabPtr);
    }

    return retVal;
}

struct varlena *
svrBulkQueryAnswer (int catType, 
		    struct varlena *vQueryInfo,
		    int rowsWanted)
{
    struct varlena *retval;
    int status;
    char *queryInfo;
    mdasC_sql_result_struct  *myresult = NULL;

    queryInfo = varToStr (vQueryInfo);

    status = _svrBulkQueryAnswer (catType, queryInfo, &myresult, rowsWanted);

    if (status == 0) {
        retval = sqlResultToVar (myresult, MULTI_ROW);
        freeSqlResult (myresult);
    } else {
	elog (NOTICE, "svrBulkQueryAnswer:  failed. Status = %d",
	  status);
        retval = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retval) = VAROUTHDRSZ;
        VAROUTSTAT(retval) = htonl (status);
        if (myresult != NULL)
            free (myresult);
    }

    return retval;
}



int
_svrBulkQueryAnswer (int catType, 
		 char *queryInfo, 
		 mdasC_sql_result_struct **myresult,
		 int rowsWanted)
{
    int status;
    char *mcatHostHint;
    int remoteFlag;
    struct hostElement  *hostTabPtr;
  
  /* RAJA Dec 11, 2003 made to check if some of the queries can be 
       done by user */
    if (checkIfUserAllowedBulkQuery(queryInfo) != 0) {
      if (ClientUser->privUserFlag != 1 && ClientUser->privUserFlag != 2 ) {
        elog (NOTICE, 
          "svrBulkQueryAnswer: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
      }
    }

    remoteFlag = getMdasEnabledHost (NULL,  &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        status = get_bulk_query_answers(catType, queryInfo,
					myresult, rowsWanted,
				  ClientUser->userName,
				  ClientUser->domainName);
#endif
    } else {
        status = remoteBulkQueryAnswer (catType, queryInfo, myresult, 
					rowsWanted, hostTabPtr);
    }
    return (status);

}

int
svrBulkMcatIngest (int catType, struct varlena *vIngestInfo,
                 struct varlena *packedResult)
{
  char *ingestInfo;
  mdasC_sql_result_struct *myresult;
  char *domainName, *userName;
  int retVal, status;
  struct hostElement  *hostTabPtr;
  int remoteFlag;
  char *tmpResLoc, *portNum;
  
  if (TicketUserFlag) {
    elog (NOTICE, "Illegal operation for a ticket user");
    return (ILLEGAL_OPR_TICKET_USER);
  }
  if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrBulkMcatIngest: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
  }
  
  remoteFlag = getMdasEnabledHost (NULL,  &hostTabPtr);
  if (remoteFlag < 0)  {
    elog(NOTICE, "svrBulkMcatIngest: getMdasEnabledHost status = %d",
	 remoteFlag);
    return(remoteFlag);
  }

  ingestInfo = varToStr (vIngestInfo);
  retVal = unpackMsg (VAROUTDATA (packedResult), (char **) &myresult,
		      sqlResultStruct_PF, NULL);
  if (retVal < 0) {
    elog(NOTICE,
	 "svrBulkMcatIngest: unpackMsg() of myresult error, status = %d",
	 retVal);
    return (retVal);
  }

  if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
    status = ingest_bulk_metadata(catType, ingestInfo,
				  myresult,
				  ClientUser->userName,
				  ClientUser->domainName );
#endif
  }
  else {
        if (remoteConnect (hostTabPtr) < 0) {
            status = SYS_ERR_REMOTE_CONN;
        } else {
	    status = srbBulkMcatIngest(hostTabPtr->conn, catType, ingestInfo,
    	     myresult);
	}
  }
  return (status);

}

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
          retVal);
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

int
svrBulkUnload(int catType, int flag, struct varlena *vClHostAddr, int clPort,
struct varlena *vCollection, svrComm_t *myComm)
{
    char *collection;
    char *clHostAddr;
    int status;

    clHostAddr = varToStr (vClHostAddr);
    collection = varToStr (vCollection);

    status = _svrBulkUnload (catType, flag, clHostAddr, clPort, collection, 
     myComm, NULL);

    free (collection);
    free (clHostAddr);

    return (status);
}

struct varlena *
svrBulkUnloadC(int catType, int flag, struct varlena *vCollection, 
svrComm_t *myComm)
{
    char *collection;
    int status;
    struct varlena * retVal;
    portList_t *myPortList = NULL;

    collection = varToStr (vCollection);

    status = _svrBulkUnload (catType, flag | CL_INIT_CONN_FLAG, NULL, 0, 
     collection, myComm, &myPortList);

    free (collection);

    if (status < 0) {
        retVal = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (status);

        return retVal;
    }

   status = packMsg ((char *) myPortList, &retVal, portList_t_PF,
     NULL);

    if (status < 0) {
        return (genIntRet (status));
    }
    VAROUTSTAT(retVal) = htonl (status);

    return (retVal);
}

int
_svrBulkUnload (int catType, int flag, char *clHostAddr, 
int clPort, char *collection, svrComm_t *myComm, portList_t **myPortList)
{
    int status;
    mdasC_sql_result_struct *myresult = NULL;
    int remoteFlag;
    struct hostElement  *hostTabPtr, *tmpHostTabPtr;
    int fileCnt;
    struct bunloadSort bunloadHead, *tmpBunloadSort, *myBunloadSort;
    int continuation_index;
    int myFd = -1;

    remoteFlag = getMdasEnabledHost (collection, &hostTabPtr);

    if (remoteFlag < 0) {
        elog(NOTICE,
         "_svrBulkUnload: getMdasEnabledHost error for %s, status = %d",
          collection, remoteFlag);
	return (remoteFlag);
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        status = get_dataset_info (catType, "NONCONTCOLLINFO",
         ClientUser->userName,
          "read", ClientUser->domainName, collection,
           &myresult, 50*DEF_NUM_ROWS);
        commit_db2_interaction (MDAS_CAT_COMMIT);
        if (status < 0) {
            elog(NOTICE,
             "_svrBulkUnload: Can't get_dataset_info for %s, status = %d",
              collection, status);
            if (myresult != NULL)
                freeSqlResult (myresult);
            return status;
        }
#endif
    } else {
        if (remoteConnect (hostTabPtr) < 0) {
            return SYS_ERR_REMOTE_CONN;
        }

        /* Do a client call. Have to supply a mdasC_sql_result_struct. */

        myresult = (mdasC_sql_result_struct  *)
          malloc (sizeof (mdasC_sql_result_struct));

        if (myresult == NULL) {
            elog(NOTICE,"_svrBulkUnload: malloc error");
            return SYS_ERR_MALLOC;
        }
        memset (myresult, 0, sizeof (mdasC_sql_result_struct));

        if ((status = srbGetDatasetInfo(hostTabPtr->conn, catType,
         "NONCONTCOLLINFO", collection, myresult, 50*DEF_NUM_ROWS, "read")) < 0) {
            elog(NOTICE,
             "_svrBulkUnload: Can't srbGetDatasetInfo %s, status = %d",
              collection, status);
            return status;
        }
    }

    if (myresult == NULL) {
        elog(NOTICE,
         "_svrBulkUnload: get_dataset_info returns NULL result pointer");
        return SYS_ERR_MDAS_CAT_RET;
    }

    if (myresult->result_count <= 0) {
        free (myresult);
        elog(NOTICE,
         "_svrBulkUnload: get_dataset_info returns no result ");
        return SYS_ERR_MDAS_CAT_RET;
    }

    fileCnt = getBulkUnloadHost (myresult, &tmpHostTabPtr, flag);

    if (tmpHostTabPtr != NULL) {	/* pass it off to the host that can best
				 * handle it */
	freeSqlResult (myresult);
        if (remoteConnect (tmpHostTabPtr) < 0) {
            return SYS_ERR_REMOTE_CONN;
        }
	if (flag & CL_INIT_CONN_FLAG) {
            status = _srbBulkUnloadC (tmpHostTabPtr->conn,
	    /* no alarm because the actual transfer is done by other host */
             catType, BL_LOCAL_FLAG, collection, myPortList);
	} else {
            status = _srbBulkUnload (tmpHostTabPtr->conn, 
	     catType, BL_LOCAL_FLAG, clHostAddr, clPort, collection);
	}

	return (status);
    }
	
    
    /* Gets here when we decided to do it here */
    
#ifdef foo
    bunloadHead = (struct bunloadSort *) malloc (sizeof (struct bunloadSort));
#endif
    memset (&bunloadHead, 0, sizeof (struct bunloadSort));

    bunloadHead.myresult = myresult;
    status = sortResultForBunload (&bunloadHead); 

    if (status < 0) {
        if (bunloadHead.myresult != NULL)
            freeSqlResult (bunloadHead.myresult);
        if (bunloadHead.unloadFlag != NULL)
            free (bunloadHead.unloadFlag);
        return (status);
    }

    myFd = setupSockForBunload (clHostAddr, clPort, myComm, flag);
    if (myFd < 0) {
       if (bunloadHead.myresult != NULL)
            freeSqlResult (bunloadHead.myresult);
        if (bunloadHead.unloadFlag != NULL)
            free (bunloadHead.unloadFlag);
        return myFd;
    }


    status = sendBunloadData (&bunloadHead, clHostAddr, clPort, myComm, flag,
     myFd);

   if (bunloadHead.myresult != NULL) {
        freeSqlResult (bunloadHead.myresult);
        bunloadHead.myresult = NULL;
    }

    if (bunloadHead.unloadFlag != NULL) {
        free (bunloadHead.unloadFlag);
        bunloadHead.unloadFlag = NULL;
    }

    if (status < 0) {
        return (DONT_SEND_RETURN);
    }

    continuation_index = myresult->continuation_index;
    while (continuation_index >= 0) {
	myresult = NULL;
	if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
            status = get_more_rows (catType, continuation_index, &myresult, 
	     50*DEF_NUM_ROWS);
#endif
        } else {
            status = remoteGetMoreRows (catType, continuation_index, &myresult, 
	     50*DEF_NUM_ROWS, hostTabPtr);
        }

        if (status < 0 || myresult == NULL || myresult->row_count <= 0) {
	    break;
	}
	continuation_index = myresult->continuation_index;
        memset (&bunloadHead, 0, sizeof (struct bunloadSort));
        bunloadHead.myresult = myresult;
        status = sortResultForBunload (&bunloadHead);
        if (status < 0)
            break;
        status = sendBunloadData (&bunloadHead, clHostAddr, clPort, myComm,
         flag, myFd);

        if (bunloadHead.myresult != NULL) {
            freeSqlResult (bunloadHead.myresult);
            bunloadHead.myresult = NULL;
        }

        if (bunloadHead.unloadFlag != NULL) {
            free (bunloadHead.unloadFlag);
            bunloadHead.unloadFlag = NULL;
        }

        if (status < 0) {
            return (DONT_SEND_RETURN);
        }
    }

    sendDoneToPortal (myFd);

#ifdef _WIN32
    closesocket(myFd);
#else
    close (myFd);
#endif

    return (DONT_SEND_RETURN);

}

int
getBulkUnloadHost (mdasC_sql_result_struct *myresult, 
struct hostElement **myHostTabPtr, int flag)
{
    int i;
    struct hostSort *tmpHost;
    struct hostSort *hostHead = NULL;
    struct resSort *tmpRes;
    char *resourceType, *resourceLoc, *resourceName, *size, *phyResClass,
     *fileName, *collection;
    srb_long_t maxCount;
    struct hostElement  *tmpHostTabPtr;
    int fileCnt = 0;
    char *prevFileName = NULL;
    char *prevCollection = NULL;
    int inx;

    *myHostTabPtr = NULL;
    if (myresult == NULL)
	return 0;

    if ((flag & BL_LOCAL_FLAG) > 0) 
	return (myresult->row_count);

    fileName = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_NAME],
            dcs_aname[DATA_NAME]);
    collection = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_GRP_NAME],
            dcs_aname[DATA_GRP_NAME]);
    resourceType = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[PHY_RSRC_TYP_NAME],
            dcs_aname[PHY_RSRC_TYP_NAME]);
    resourceLoc = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[RSRC_ADDR_NETPREFIX],
          dcs_aname[RSRC_ADDR_NETPREFIX]);
    resourceName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[PHY_RSRC_NAME],
          dcs_aname[PHY_RSRC_NAME]);
    size = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[SIZE], dcs_aname[SIZE]);
    phyResClass = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_CLASS], dcs_aname[RSRC_CLASS]);

    /* loop through just 50 */
    if (myresult->row_count > 50) {
	inx = 50;
    } else {
	inx = myresult->row_count;
    }

    for (i = 0; i < inx; i++) {
#ifdef foo	/* not doing count here */
	if (prevFileName == NULL || strcmp (prevFileName, fileName) != 0 || 
	 strcmp (prevCollection, collection) != 0) {
	    prevFileName = fileName;
	    prevCollection = collection;
	    fileCnt ++;
	}

	if (flag > 0) {		/* do count only */
            resourceType += MAX_DATA_SIZE;
            resourceLoc += MAX_DATA_SIZE;
            resourceName += MAX_DATA_SIZE;
            size += MAX_DATA_SIZE;
            phyResClass += MAX_DATA_SIZE;
            fileName += MAX_DATA_SIZE;
            collection += MAX_DATA_SIZE;
	    continue;
	}
#endif

	tmpHost = hostHead;
	tmpRes = NULL;
	while (tmpHost != NULL) {
	    tmpRes = tmpHost->resPtr;
	    while (tmpRes != NULL) {
		if (strcmp (resourceName, tmpRes->resourceName) == 0) {
		    tmpRes->count += strtoll (size, 0, 0);
		    break;
		}
		tmpRes = tmpRes->next;
	    }
	    if (tmpRes != NULL)
		break;
	    tmpHost = tmpHost->next;
	}

	if (tmpRes == NULL) {
	    /* No match, have to put in a new one */
	    char *portNum, tmpResLoc[MAX_TOKEN];
	    char *tmpLoc;
	    int remoteFlag;

#ifdef FED_MCAT
	    /* Needed because queryBulkUnloadForMove done that already */
	    if (strstr (resourceLoc, "&") == NULL) { 
	        tmpLoc = getAddrWithZone (resourceLoc, collection);
                remoteFlag = chkHostAndZone (tmpLoc, &tmpHostTabPtr);
	        free (tmpLoc);
	    } else {
                remoteFlag = chkHostAndZone (resourceLoc, &tmpHostTabPtr);
	    }
            portNum = NULL;
#else
	    strcpy (tmpResLoc, resourceLoc);
	    portNum = parseAddr (tmpResLoc);
	    remoteFlag = chkHost (tmpResLoc, portNum, &tmpHostTabPtr);
#endif
	    if (remoteFlag < 0) {
                elog (NOTICE, 
		 "getBulkUnloadHost: Invalid host addr %s", resourceLoc);
	    } else {
	        int objTypeInx;
		int resClassInx;

		tmpRes = (struct resSort *) malloc (sizeof (struct resSort));
		memset (tmpRes, 0, sizeof (struct resSort));   
        	objTypeInx = objTypeLookUp (resourceType);
        	if (objTypeInx < 0) {
                    elog (NOTICE, 
		     "getBulkUnloadHost: Invalid host resType %s",resourceType);
		    tmpRes->resClass = 1;
		} else {
		    if (srbObjTypeEntry[objTypeInx].systemType == UNIX_FT) {
			tmpRes->resClass = 0;	/* cache */
		    } else {
			resClassInx = resClassLookup (phyResClass);
			if (resClassInx < 0) 
			    tmpRes->resClass = 1;
			else 
			    tmpRes->resClass = 
			     resClassEntries[resClassInx].permanent;
		    }
		}
		tmpRes->resourceName = resourceName;
		tmpRes->count += strtoll (size, 0, 0);
        	tmpHost = hostHead;
        	while (tmpHost != NULL) {
		    if (tmpHostTabPtr == tmpHost->hostTabPtr)
			break;
		    tmpHost = tmpHost->next;
		}
		if (tmpHost == NULL) {
		    tmpHost = (struct hostSort *) malloc 
		     (sizeof (struct hostSort));
		    memset (tmpHost, 0, sizeof (struct hostSort));
		    tmpHost->hostTabPtr = tmpHostTabPtr;
		    tmpHost->remoteFlag = remoteFlag;
		    tmpHost->next = hostHead;
		    hostHead = tmpHost;
		}
		tmpRes->next = tmpHost->resPtr;
		tmpHost->resPtr = tmpRes;
	    }
        }
	resourceType += MAX_DATA_SIZE;
	resourceLoc += MAX_DATA_SIZE;
	resourceName += MAX_DATA_SIZE;
	size += MAX_DATA_SIZE;
	phyResClass += MAX_DATA_SIZE;
	fileName += MAX_DATA_SIZE;
	collection += MAX_DATA_SIZE;
    }	

#ifdef foo
    if (flag > 0)	/* count only */
	return (fileCnt);
#endif

    /* tally the number */
    tmpHost = hostHead;
    tmpHostTabPtr = NULL;
    maxCount = 0;
    while (tmpHost != NULL) {
	srb_long_t myCount;
	struct hostSort *nextHost;

	myCount = 0;
        tmpRes = tmpHost->resPtr;
        while (tmpRes != NULL) {
	    struct resSort *nextRes;

	    if (tmpRes->resClass > 0)
		myCount += (tmpRes->count / 10);
	    else
		myCount += tmpRes->count;

	    nextRes = tmpRes->next;
	    free (tmpRes);
	    tmpRes = nextRes;
	}
	if (myCount > maxCount) {
	    maxCount = myCount;
	    if (tmpHost->remoteFlag == REMOTE_HOST)
		tmpHostTabPtr = tmpHost->hostTabPtr;
	    else
		tmpHostTabPtr = NULL;
	}
	nextHost = tmpHost->next;
	free (tmpHost);
	tmpHost = nextHost;
    }
    *myHostTabPtr = tmpHostTabPtr;
    return (myresult->row_count);
}

int
sortResultForBunload (struct bunloadSort *tmpBunloadSort) 
{
    int i, j, status;
    int *unloadFlag;
    int startInx, endInx, curInx;
    int numCopies;
    char *fileName, *collection, *phyResType;
    mdasC_sql_result_struct *myresult;
    int fileCnt = 0;

    if (tmpBunloadSort == NULL || tmpBunloadSort->myresult == NULL ||
     tmpBunloadSort->myresult->row_count <= 0) {
        return (MCAT_INQUIRE_ERROR);
    }

    myresult = tmpBunloadSort->myresult;

    fileName = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_NAME],
            dcs_aname[DATA_NAME]);
    collection = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_GRP_NAME],
            dcs_aname[DATA_GRP_NAME]);

    phyResType = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[PHY_RSRC_TYP_NAME],
            dcs_aname[PHY_RSRC_TYP_NAME]);

    unloadFlag = tmpBunloadSort->unloadFlag = (int *) malloc (
     myresult->row_count * sizeof (int));
    memset (unloadFlag, 0, myresult->row_count * sizeof (int));

    for (i = 0; i < myresult->row_count; i++) {
	if (unloadFlag[i] != UN_UNPROC)
	    continue;

	/* find the number of copies */
	numCopies = 1;
	startInx = endInx = i;
	curInx = i + 1;
	unloadFlag[i] = UN_INPROC;
	while (curInx < myresult->row_count) {
	    if (strcmp (&fileName[i*MAX_DATA_SIZE], 
	     &fileName[curInx*MAX_DATA_SIZE]) != 0) {
		break;
	    }

	    if (unloadFlag[curInx] != UN_UNPROC) {
	        curInx ++;
                continue;
	    }

	    if (strcmp (&collection[i*MAX_DATA_SIZE],
             &collection[curInx*MAX_DATA_SIZE]) != 0) {
	        curInx ++;
		continue;
	    }

	    unloadFlag[curInx] = UN_INPROC;
	    numCopies ++;
	    curInx ++;
	}

	if (numCopies == 1 && 
	 strcmp (&phyResType[i*MAX_DATA_SIZE], COMPOUND_RESC) != 0) {
	    unloadFlag[i] = UN_SELECTED;
	    fileCnt ++;
	    continue;
	}

	status = pickCopyToUnload (numCopies, i, curInx - 1, tmpBunloadSort);
	fileCnt ++;
    }

    tmpBunloadSort->fileCnt = fileCnt;
    return (fileCnt);
}

int 
pickCopyToUnload (int numCopies, int startInx, int endInx, 
struct bunloadSort *tmpBunloadSort) 
{
    mdasC_sql_result_struct  *myresult;
    int *unloadFlag;
    int i, status;
    int myInx;
    char *objID, *resourceType, *dataPath, *resourceLoc, *resourceName,
    *dataType, *replNum, *offset, *size, *phyResClass, *containerName,
    *isDirty, *collection;
    char chksum[MAX_TOKEN];
    struct mdasInfoOut *infoOutHead, *infoOut, *prevInfoOut;
    struct mdasInfoOut *cacheInfoOut = NULL;

    if (tmpBunloadSort == NULL || tmpBunloadSort->myresult == NULL ||
     tmpBunloadSort->myresult->row_count <= 0)
	return (SYS_ERR_MDAS_CAT_RET);

    myresult = tmpBunloadSort->myresult;
    unloadFlag = tmpBunloadSort->unloadFlag;


    memset (chksum, 0, sizeof (chksum));	/* no chksum for now */

    collection = (char *) get_from_result_struct(
         (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

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

    if (objID == NULL || resourceType == NULL || dataPath == NULL ||
     resourceLoc == NULL || resourceName == NULL || dataType == NULL ||
      offset == NULL || size == NULL || phyResClass == NULL ||
       containerName == NULL || isDirty == NULL) {
        elog (NOTICE, "objID = %x, resourceType=%x, dataPath=%x, resourceLoc=%x,resourceName=%x, dataType=%x, replNum=%x, offset=%x, size=%x, phyResClass=%x, containerName=%x, isDirty=%x",
         objID, resourceType, dataPath, resourceLoc, resourceName, dataType,
          replNum, offset, size, phyResClass, containerName, isDirty);
        elog(NOTICE,
         "pickCopyToUnload: some NULL result ");
        return SYS_ERR_MDAS_CAT_RET;
    }
    for (i = startInx; i <= endInx; i++) {
	char *tmpLoc;

	if (unloadFlag[i] != UN_INPROC)
	    continue;

        infoOut = (struct mdasInfoOut *)
            malloc (sizeof (struct mdasInfoOut));

        memset (infoOut, 0, sizeof (struct mdasInfoOut));

        if (i == startInx) {
            infoOutHead = infoOut;
        } else  {
            prevInfoOut->next = infoOut;
        }
        prevInfoOut = infoOut;

#ifdef FED_MCAT
        tmpLoc = getAddrWithZone (&resourceLoc[i*MAX_DATA_SIZE],
           &collection[i*MAX_DATA_SIZE]);
#else
        tmpLoc = &resourceLoc[i*MAX_DATA_SIZE];
#endif

        status = fillInfoOut (infoOut, &objID[i*MAX_DATA_SIZE], 
	 &resourceType[i*MAX_DATA_SIZE], &dataPath[i*MAX_DATA_SIZE], 
	 tmpLoc,
	 &resourceName[i*MAX_DATA_SIZE],
         &dataType[i*MAX_DATA_SIZE], &replNum[i*MAX_DATA_SIZE], 
	 &offset[i*MAX_DATA_SIZE], &size[i*MAX_DATA_SIZE], 
	 &phyResClass[i*MAX_DATA_SIZE], &containerName[i*MAX_DATA_SIZE], 
	 &isDirty[i*MAX_DATA_SIZE], chksum, 
	 &collection[i*MAX_DATA_SIZE]);
	/* offset is meaningless here. use it to store the index i */
	infoOut->offset = i;
    }
    sortAndCombInfoForObj (&infoOutHead, NULL, O_RDONLY, 0);
    infoOut = infoOutHead;
    while (infoOut != NULL) {
	if (srbObjTypeEntry[infoOut->objTypeInx].objType != COMPOUND_TYPE) 
	    break;
	infoOut = infoOut->next;
    }
    if (infoOut == NULL) {
      infoOut = infoOutHead;
      while (infoOut != NULL) {	/* we may have to deal with compound obj */

	myInx = infoOut->offset;
	if (srbObjTypeEntry[infoOut->objTypeInx].objType == COMPOUND_TYPE) {
	    status = _svrStageCompObj (infoOut, 
	     &collection[myInx*MAX_DATA_SIZE], &cacheInfoOut, 1);
	    if (status < 0) {
		elog (NOTICE,
		 "pickCopyToUnload: Unable to stage compObj %s/%s, stat=%d",
		  &collection[myInx*MAX_DATA_SIZE], 
		  &objID[myInx*MAX_DATA_SIZE]);
	    } else {
		/* stage is successful have to overwite myresult */
		infoOut = cacheInfoOut;
		infoOut->offset = myInx;

		strcpy (&resourceType[myInx*MAX_DATA_SIZE],
		 srbObjTypeEntry[cacheInfoOut->objTypeInx].resourceType);

        	strcpy (&dataPath[myInx*MAX_DATA_SIZE], 
		 cacheInfoOut->dataPath);

        	strcpy (&resourceLoc[myInx*MAX_DATA_SIZE], 
		 cacheInfoOut->resourceLoc);

        	strcpy (&resourceName[myInx*MAX_DATA_SIZE], 
		 cacheInfoOut->resourceName);

        	strcpy (&phyResClass[myInx*MAX_DATA_SIZE], 
		 cacheInfoOut->phyResClass);
		break;
	    }
	}
	infoOut = infoOut->next;
      }
    }
    
    for (i = startInx; i <= endInx; i++) {
        if (unloadFlag[i] != UN_INPROC)
            continue;
	unloadFlag[i] = UN_REJECT;
    }

    if (infoOut == NULL) {
	freeAllInfoOut (infoOutHead);
	freeAllInfoOut (cacheInfoOut);
	if (status < 0) {
	    return (status);
	} else {
	    elog (NOTICE,
	      "pickCopyToUnload failed for  %s/%s, stat=%d",
		&collection[startInx*MAX_DATA_SIZE], 
		&objID[startInx*MAX_DATA_SIZE]);
	    return (SYS_ERR_MDAS_CAT_RET);
	}
    }

    myInx = infoOut->offset;
    unloadFlag[myInx] = UN_SELECTED;
    freeAllInfoOut (infoOutHead);
    freeAllInfoOut (cacheInfoOut);

    return (myInx);
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
        infoOut->intSegNum = -1;

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
sendBunloadData (struct bunloadSort *bunloadHead, char *clHostAddr, int clPort,
svrComm_t *myComm, int flag, int myFd)
{
    struct bunloadSort *tmpBunloadSort;
    mdasC_sql_result_struct *myresult;
    int status, i;
    char *objID, *resourceType, *dataPath, *resourceLoc, *resourceName,
    *dataType, *replNum, *offset, *size, *phyResClass, *containerName,
    *isDirty, *collection, *chksum;
    char *filePtr, *collPtr, *sizePtr, *offsetPtr, *chksumPtr; 
    struct varlena * packedResult;
    byteStream myBS;
    FunctCall myFunctCall;
    int bufOffset;
    char *buf, *bufptr;

    int curCnt = 0;
    int fileCnt = 0;
    srb_long_t myOffset = 0;

    /* find the number of files first */

    fileCnt = bunloadHead->fileCnt;

    if (fileCnt <= 0) {
        elog(NOTICE,
         "sendBunloadData: Number of files to unload is zero");
        return SYS_ERR_MDAS_CAT_RET;
    }

    myresult = (mdasC_sql_result_struct *) malloc 
     (sizeof (mdasC_sql_result_struct));

    if (myresult == NULL) {
        elog(NOTICE,"sendBunloadData: malloc error");
        return SYS_ERR_MALLOC;
    }

    memset (myresult, 0, sizeof (mdasC_sql_result_struct));

    myresult->result_count = 5;
    myresult->row_count = fileCnt;
    myresult->continuation_index = bunloadHead->myresult->continuation_index;

    for (i = 0; i < myresult->result_count; i++) {
        myresult->sqlresult[i].values = malloc (fileCnt * MAX_DATA_SIZE);
	if (myresult->sqlresult[i].values == NULL) {
            elog(NOTICE,"sendBunloadData: malloc error");
	    freeSqlResult (myresult);
            return SYS_ERR_MALLOC;
        }
	memset (myresult->sqlresult[i].values, 0, fileCnt * MAX_DATA_SIZE);
    }

    filePtr = myresult->sqlresult[0].values;
    collPtr = myresult->sqlresult[1].values;
    sizePtr = myresult->sqlresult[2].values;
    offsetPtr = myresult->sqlresult[3].values;
    chksumPtr = myresult->sqlresult[4].values;

    myresult->sqlresult[0].tab_name = strdup (dcs_tname[DATA_NAME]);
    myresult->sqlresult[0].att_name = strdup (dcs_aname[DATA_NAME]);
    myresult->sqlresult[1].tab_name = strdup (dcs_tname[DATA_GRP_NAME]);
    myresult->sqlresult[1].att_name = strdup (dcs_aname[DATA_GRP_NAME]);
    myresult->sqlresult[2].tab_name = strdup (dcs_tname[SIZE]);
    myresult->sqlresult[2].att_name = strdup (dcs_aname[SIZE]);
    myresult->sqlresult[3].tab_name = strdup (dcs_tname[OFFSET]);
    myresult->sqlresult[3].att_name = strdup (dcs_aname[OFFSET]);
    myresult->sqlresult[4].tab_name = strdup (dcs_tname[DATA_CHECKSUM]);
    myresult->sqlresult[4].att_name = strdup (dcs_aname[DATA_CHECKSUM]);
    
    tmpBunloadSort = bunloadHead;
    while (tmpBunloadSort != NULL) {
	mdasC_sql_result_struct *tmpresult;
	int *unloadFlag;

	tmpresult = tmpBunloadSort->myresult;
	unloadFlag = tmpBunloadSort->unloadFlag;

        collection = (char *) get_from_result_struct(
         (mdasC_sql_result_struct *) tmpresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

        objID = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) tmpresult, dcs_tname[DATA_NAME],
            dcs_aname[DATA_NAME]);
        size = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) tmpresult,
            dcs_tname[SIZE], dcs_aname[SIZE]);
        chksum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) tmpresult,
            dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);

        for (i = 0; i < tmpresult->row_count; i++) {
	    if (unloadFlag[i] != UN_SELECTED)
		continue;
	    strcpy (&filePtr[curCnt*MAX_DATA_SIZE], 
	     &objID[i*MAX_DATA_SIZE]);
	    strcpy (&collPtr[curCnt*MAX_DATA_SIZE],
	     &collection[i*MAX_DATA_SIZE]);
	    strcpy (&sizePtr[curCnt*MAX_DATA_SIZE], 
	     &size[i*MAX_DATA_SIZE]);
#if defined(PORTNAME_osx)
            sprintf (&offsetPtr[curCnt*MAX_DATA_SIZE], "%-qd", myOffset);
#else
            sprintf (&offsetPtr[curCnt*MAX_DATA_SIZE], "%-lld", myOffset);
#endif
	    if (chksum != NULL) {
		strcpy (&chksumPtr[curCnt*MAX_DATA_SIZE],
             &chksum[i*MAX_DATA_SIZE]);
	    }
	    myOffset += strtoll (&size[i*MAX_DATA_SIZE], 0, 0);
	    curCnt++;
           if (curCnt > fileCnt) {
                elog(NOTICE,
                 "sendBunloadData: curCnt %d is larger than fileCnt %d",
		 curCnt, fileCnt);
		freeSqlResult (myresult);
                return SYS_ERR_MDAS_CAT_RET;
            }
	}
        tmpBunloadSort = tmpBunloadSort->next;
    }
    /* Send myresult */

    status = packMsg ((char *) myresult, &packedResult, sqlResultStruct_PF,
     NULL);

    if (status < 0) {
        elog (NOTICE, "sendBunloadData: packMsg error. status = %d", status);
	freeSqlResult (myresult);
        return status;
    }

    myBS.len = VARSIZE (packedResult) - VAROUTHDRSZ;
    myBS.bytes = VAROUTDATA(packedResult);

    /* send the packed myresult */

    status = writeToPortal (myFd, myBS.bytes, 0, myBS.len);

    free (packedResult);
    freeSqlResult (myresult);

    if (status <= 0) {
	/* XXXXX need to have a way of sending error msg back to user */
        fprintf (stderr, 
	 "sendBunloadData: send myresult error. errno = %d\n", errno);
	close (myFd);
        return (DONT_SEND_RETURN);
    }

    /* Now send the files */

    myOffset = 0;
    bufOffset = 0;
    buf = malloc (BL_BUFSIZE);
    bufptr = buf;
    tmpBunloadSort = bunloadHead;
    while (tmpBunloadSort != NULL) {
        mdasC_sql_result_struct *tmpresult;
        int *unloadFlag;

        tmpresult = tmpBunloadSort->myresult;
        unloadFlag = tmpBunloadSort->unloadFlag;

        resourceType = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) tmpresult, dcs_tname[PHY_RSRC_TYP_NAME],
            dcs_aname[PHY_RSRC_TYP_NAME]);
        dataPath = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) tmpresult, dcs_tname[PATH_NAME],
          dcs_aname[PATH_NAME]);
        resourceLoc = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) tmpresult, dcs_tname[RSRC_ADDR_NETPREFIX],
          dcs_aname[RSRC_ADDR_NETPREFIX]);

        size = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) tmpresult,
            dcs_tname[SIZE], dcs_aname[SIZE]);

        collection = (char *) get_from_result_struct(
         (mdasC_sql_result_struct *) tmpresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

        for (i = 0; i < tmpresult->row_count; i++) {
	    int objTypeInx, exfInx, space, readLen, nbytes;
	    srb_long_t toread;
	    char *tmpLoc;

            if (unloadFlag[i] != UN_SELECTED)
                continue;

            objTypeInx = objTypeLookUp (&resourceType[i*MAX_DATA_SIZE]);

#ifdef FED_MCAT
	    tmpLoc = getAddrWithZone (&resourceLoc[i*MAX_DATA_SIZE],
             &collection[i*MAX_DATA_SIZE]);
#else
	    tmpLoc = &resourceLoc[i*MAX_DATA_SIZE];
#endif
            exfInx = lowLevelOpen (objTypeInx, &dataPath[i*MAX_DATA_SIZE], 
	     tmpLoc, O_RDONLY);

#ifdef FED_MCAT
	    free (tmpLoc);
#endif

	    if (exfInx < 0) {
		fprintf (stderr, 
		 "sendBunloadData: unable to open %s at %s, stat = %d\n",
		  &dataPath[i*MAX_DATA_SIZE], &resourceLoc[i*MAX_DATA_SIZE],
		  exfInx);
        	close (myFd);
		free (buf);
        	return (DONT_SEND_RETURN);
	    }

	    toread = strtoll (&size[i*MAX_DATA_SIZE], 0, 0);

    	    while (toread > 0) {
		space = BL_BUFSIZE - bufOffset;

	        if (toread > space && bufOffset > 0) {
		    /* dump the buf */
		    status = writeToPortal (myFd, buf, myOffset, bufOffset);
        	    if (status <= 0) {
        	        fprintf (stderr,
        	         "sendBunloadData: write data error.errno=%d\n", errno);
        	        close (myFd);
			free (buf);
       		        return (DONT_SEND_RETURN);
    		    }

		    myOffset += bufOffset;
		    bufOffset = 0;
		    bufptr = buf;
		    readLen = BL_BUFSIZE;
		} else {
                    if (toread > BL_BUFSIZE)
                        readLen = BL_BUFSIZE;
                    else
                        readLen = toread; 
		    bufptr = buf + bufOffset;  
        	}
		nbytes = _objRead (exfInx, objTypeInx, bufptr, readLen);
        	if (nbytes <= 0) {
		    fprintf (stderr, 
		     "sendBunloadData: read error for %s at %s, toread=%d, read=%d, errno=%d\n",
		      &dataPath[i*MAX_DATA_SIZE], &resourceLoc[i*MAX_DATA_SIZE],
		     toread, nbytes, errno);
                    close (myFd);
		    free (buf);
                    return (DONT_SEND_RETURN);
        	}
        	toread -= nbytes;
        	bufOffset += nbytes;
    	    }
	    lowLevelClose (objTypeInx, exfInx);
	}
	tmpBunloadSort = tmpBunloadSort->next;
    }
    if (bufOffset > 0) {
        status = writeToPortal (myFd, buf, myOffset, bufOffset);
        if (status <= 0) {
            fprintf (stderr,
             "sendBunloadData: write data error.errno=%d\n", errno);
	}
    }
    free (buf);

    return (0);
}

int
_objChksumAndReg (int objTypeInx, int flags, char *chksum,
srb_long_t myOffset, srb_long_t mySize, char *destObjID,
char *destCollection, char *phyResName, char *myPathName, char *phyResLoc,
int chksumFlag, unsigned long iChksum, char *regResName, char *regPathName)
{
    int lowLevelDescInx;
    int retVal;

    if (chksumFlag & K_FLAG) {
        lowLevelDescInx = lowLevelOpen (objTypeInx,
         myPathName, phyResLoc, O_RDONLY);
        if (lowLevelDescInx < 0) {
            elog(NOTICE,
	     "_objChksumAndReg: Unable to open %s for chksum, stat=%d",
             myPathName, lowLevelDescInx);
	    return (lowLevelDescInx);
        }
        retVal = _objChksum (objTypeInx, lowLevelDescInx, flags, chksum, 
         myOffset, mySize);

	/* XXXXX close it here instead of in svrFileSum */
        _objClose (objTypeInx, lowLevelDescInx);
        if (retVal < 0) {
            elog (NOTICE,
             "_objChksumAndReg: _objChksum error for obj %s, status = %d",
              destObjID, retVal);
	    return (retVal);
        }
        if (iChksum != atoi (chksum)) {
            elog (NOTICE,
             "_svrObjClose: chksum of %s does not match the given %d",
             chksum, iChksum);
            return (OBJ_ERR_BAD_CHKSUM);
        }
    } else if (chksumFlag &k_FLAG) {
        sprintf (chksum, "%d", iChksum);
    } else {
        sprintf (chksum, "0");
    }

    /* register the chksum */
    retVal = _svrModifyDataset (MDAS_CATALOG,
      destObjID,
      destCollection,
      regResName,
      regPathName,
      chksum,"", D_INSERT_DCHECKSUM);

    if (retVal < 0) {
        elog (NOTICE,
         "_objChksumAndReg: Unable to reg chksum for %s/%s, status = %d",
         destCollection, destObjID, retVal);
    }

    return (retVal);
}

struct varlena *
svrObjChksum (struct varlena *vObjID, struct varlena *vCollectionName,
int chksumFlag, struct varlena * vInpChksum)
{
    char *objID, *collectionName, *inpChksum;
    int status;
    struct varlena *retval;

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    inpChksum = varToStr (vInpChksum);

    retval = (struct varlena *)malloc(VAROUTHDRSZ + CHKSUM_STR_LEN + 1);

    status = _svrObjChksum (objID, collectionName, chksumFlag, inpChksum,
     VAROUTDATA(retval));

    free (objID);
    free (collectionName);
    free (inpChksum);

    if (status >= 0)
        VARSIZE(retval) = CHKSUM_STR_LEN + VAROUTHDRSZ;
    else
        VARSIZE(retval) = VAROUTHDRSZ;

    VAROUTSTAT(retval) = htonl (status);

    return retval;
}

int
_svrObjChksum (char *objID, char *collectionName, int chksumFlag, 
char *inpChksum, char *chksum)
{
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead, *nextInfoOut, *tmpInfoOut;
    int retVal;
    char *fullPathName = NULL;
    int openFlag;
    int descInx;
    unsigned long iChksum;
    int dirtyFlag = -1;
    int status = 0;
 

    openFlag = O_RDONLY;
    infoInp.domainName = ClientUser->domainName;
    infoInp.accessMode = openFlagLookUp (openFlag);
    infoInp.objID = objID;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = collectionName;

    retVal = mdasGetInfo (&infoInp, &infoOutHead);

    if (retVal < 0) {
	elog (NOTICE,
	 "_svrObjChksum: mdasGetInfo of %s/%s error, stat = %d",
	 collectionName, objID, retVal); 
	return (retVal);
    }

    tmpInfoOut = infoOutHead; 
    while (tmpInfoOut != NULL) {

	if (chksumFlag & l_FLAG) {
	    if (dirtyFlag == -1) {	/* first time */
	        dirtyFlag = tmpInfoOut->isDirty;
		iChksum = tmpInfoOut->chksum;
            } else if (tmpInfoOut->isDirty > 0) {
                if (dirtyFlag == 0) {
                    dirtyFlag = 1;
		    iChksum = tmpInfoOut->chksum;
                } else if (iChksum == 0) {
		    iChksum = tmpInfoOut->chksum;
		}
            }
	    sprintf (chksum, "%d", iChksum);
	    tmpInfoOut = tmpInfoOut->next;
	    retVal = 0;
	    continue;
	}
	/* save next */
	nextInfoOut = tmpInfoOut->next;
	tmpInfoOut->next = NULL;
        descInx = _svrObjOpenWithInfo (&infoInp, &tmpInfoOut, openFlag);
	tmpInfoOut->next = nextInfoOut;	    /* restore next */
        tmpInfoOut = nextInfoOut;
	if (descInx < 0) {
	    status = descInx;
	    continue;
	}


	/* now checksum it */

	if (srbObjDesc[descInx].chksum == 0 || chksumFlag & c_FLAG || 
	  chksumFlag & f_FLAG) {
	    retVal = _chksumOpenedObj (descInx, chksum);


            if (retVal < 0) {
                elog (NOTICE,
             "_svrObjChksum: _chksumOpenedObj error for %s/%s, status = %d",
              srbObjDesc[descInx].collection, srbObjDesc[descInx].objID, 
	       retVal);
                status = retVal;
        	svrObjClose (descInx);
                continue;
            }
	} else {
	    sprintf (chksum, "%d", srbObjDesc[descInx].chksum);
	}

	if (!(chksumFlag & f_FLAG) &&
	  srbObjDesc[descInx].chksum != 0 &&
	  srbObjDesc[descInx].chksum != atoi (chksum)) {
	    elog (NOTICE, 
	      "_svrObjChksum: chksum mismatch: reg = %d, cal = %s\n",
	       srbObjDesc[descInx].chksum, chksum);
	    return (OBJ_ERR_BAD_CHKSUM);
	}
#ifdef foo	/* allow c_FLAG to register too */
	if (!(chksumFlag & c_FLAG) && 
#endif
	if (srbObjDesc[descInx].chksum != atoi (chksum)) {
	    char *chksumPath;

            if (srbObjDesc[descInx].intReplNum >= 0) {
                chksumPath = srbObjDesc[descInx].compDataPath;
            } else {
                chksumPath = srbObjDesc[descInx].dataPath;
            }
            retVal = _svrModifyDataset (MDAS_CATALOG,
              srbObjDesc[descInx].objID,
              srbObjDesc[descInx].collection,
              srbObjDesc[descInx].resourceName,
              chksumPath,
              chksum,"", D_INSERT_DCHECKSUM);
            if (retVal < 0) {
                elog (NOTICE,
                 "_svrObjChksum: Unable to reg chksum for %s/%s, status = %d",
                 srbObjDesc[descInx].collection, srbObjDesc[descInx].objID, 
		 retVal);
                status = retVal;
        	svrObjClose (descInx);
                continue;
	    } else {
		retVal = CHKSUM_REG;
	    }
        }
        svrObjClose (descInx);
    }
    if (status < 0)
	return (status);
    else
	return (retVal);
}
	
int
_chksumOpenedObj (int descInx, char *chksum)
{
    int fileType, objTypeInx, lowLevelDescInx, contInx;
    int status;
    
    fileType = parseObjDesc (descInx, &objTypeInx, &lowLevelDescInx, &contInx);

    if (fileType < 0) {
        return (fileType);
    }
 
    if (lowLevelDescInx < 0 || objTypeInx < 0) {
        elog(NOTICE,"_svrObjClose: lowLevelDescInx or objTypeInx out of range");
        return INP_ERR_DESC;
    }

    if (lowLevelDescInx == DESC_TAKEN) {
        /* an opened collection */
        return (0);
    }


    if (fileType == INCONTAINER_FILE_TYPE) {
        status = _objChksum (objTypeInx, lowLevelDescInx, 0, chksum,
         srbObjDesc[descInx].baseOffset, srbObjDesc[descInx].objSize);
    } else {
        status = _objChksum (objTypeInx, lowLevelDescInx, 0, chksum, 
	  0, srbObjDesc[descInx].objSize);
    }
    return (status);
}

int
lowLevelClose (int objTypeInx, int lowLevelDescInx)
{
    int status;

    if (lowLevelDescInx < 0 || objTypeInx < 0) {
        elog(NOTICE,
	 "lowLevelClose: lowLevelDescInx or objTypeInx out of range");
        return INP_ERR_DESC;
    }

    if (lowLevelDescInx == DESC_TAKEN) {
        /* an opened collection */
        return (0);
    }

#ifdef WRITE_SYNC
    /* sync it if it has been written to */
    if (srbObjTypeEntry[objTypeInx].objType == FILE_TYPE)
        status = _fileWriteSync (lowLevelDescInx);
#endif  /* WRITE_SYNC */
    status = _objClose (objTypeInx, lowLevelDescInx);
    return (status);
}

srb_long_t
_getWriteSize (int objTypeInx, int lowLevelDescInx)
{
    srb_long_t size;

    switch (srbObjTypeEntry[objTypeInx].objType) {
      case FILE_TYPE:
        size = _fileWriteSize (lowLevelDescInx);
        break;
      case DB_LOBJ_TYPE:
#ifdef foo   /* XXXX _dbLobjWriteSize is not working */
        size = _dbLobjWriteSize (lowLevelDescInx);
#endif
        size = 0;
        break;
      case DB_TABLE_TYPE:
      default:                /* shouldn't get here */
        size = 0;
        break;
    }
    return (size);
}
    



int
svrModifyUserNonPriv (int catType, struct varlena *vUserNameDomain,
struct varlena *vDataValue1, struct varlena *vDataValue2,
struct varlena *vDataValue3, struct varlena *vDataValue4, 
struct varlena *vDataValue5, int actionType)
{
    char *userNameDomain;
    char *dataValue1;
    char *dataValue2;
    char *dataValue3;
    char *dataValue4;
    char *dataValue5;
    int retVal;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    userNameDomain = varToStr (vUserNameDomain);
    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);
    dataValue3 = varToStr (vDataValue3);
    dataValue4 = varToStr (vDataValue4);
    dataValue5 = varToStr (vDataValue5);


    retVal = _svrModifyUserNonPriv (catType, userNameDomain, dataValue1, 
    dataValue2, dataValue3, dataValue4, dataValue5, actionType);

    free (userNameDomain);
    free (dataValue1);
    free (dataValue2);
    free (dataValue3);
    free (dataValue4);
    free (dataValue5);

    return retVal;
}

int
_svrModifyUserNonPriv (int catType, char *userNameDomain, char *dataValue1,
char *dataValue2, char *dataValue3, char *dataValue4, char *dataValue5, 
int actionType)
{
    int retVal;
    char *userNameReg;
    char *userAuthReg;
    char *userDomainReg;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    userNameReg = ClientUser->userName;
    userAuthReg = "";
    userDomainReg = ClientUser->domainName;

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = modify_user_info_nonpriv (catType, userNameReg, userDomainReg,
	  userNameDomain, dataValue1, dataValue2, dataValue3,
	  dataValue4, dataValue5, actionType);
	if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        retVal = remoteModifyUserNonPriv (catType, userNameDomain,
          dataValue1, dataValue2, dataValue3, dataValue4, dataValue5, 
	  actionType, hostTabPtr);
    }

    return retVal;
}


int
svrModifyResource (int catType, struct varlena *vResourceName,
struct varlena *vDataValue1, struct varlena *vDataValue2,
struct varlena *vDataValue3, struct varlena *vDataValue4, 
 int actionType)
{
    char *resourceName;
    char *dataValue1;
    char *dataValue2;
    char *dataValue3;
    char *dataValue4;
    int retVal;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    resourceName = varToStr (vResourceName);
    dataValue1 = varToStr (vDataValue1);
    dataValue2 = varToStr (vDataValue2);
    dataValue3 = varToStr (vDataValue3);
    dataValue4 = varToStr (vDataValue4);


    retVal = _svrModifyResource (catType, resourceName, dataValue1, 
    dataValue2, dataValue3, dataValue4,  actionType);

    free (resourceName);
    free (dataValue1);
    free (dataValue2);
    free (dataValue3);
    free (dataValue4);

    return retVal;
}

int
_svrModifyResource (int catType, char *resourceName, char *dataValue1,
char *dataValue2, char *dataValue3, char *dataValue4, int actionType)
{
    int retVal;
    char *userName;
    char *userAuth;
    char *userDomain;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Do a modify_resource_info */
    if (ClientUser->privUserFlag != 1) {
        elog (NOTICE, 
          "svrModifyResource: User has no privilege for this operation");
	return (AUTH_ERR_PROXY_NOPRIV);
    }
    userName = ClientUser->userName;
    userAuth = "";
    userDomain = ClientUser->domainName;

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = modify_resource_info (catType, resourceName, 
				       userName, userDomain,actionType,
				       dataValue1, dataValue2, dataValue3,
				       dataValue4);
	if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        retVal = remoteModifyResource (catType, resourceName,
          dataValue1, dataValue2, dataValue3, dataValue4, 
	  actionType, hostTabPtr);
    }

    return retVal;
}

int 
setupSrvPortal (svrComm_t *myComm, int *mylport)
{
    portList_t thisPortList;
    int lsock, lport;
    struct sockaddr_in sin;
    int myCookie;
    int cookie;
    int status;
    struct varlena * retVal;
    FunctCall myFunctCall;
    int length = sizeof (sin);
    char *laddr = NULL;

    if ((lsock = portalCreate ()) < 0) {
        elog (NOTICE,
         "setupSrvPortal() -- portalCreate() failed: errno=%d",
          errno);
        return lsock;
    }

    length = sizeof (sin);
    if (getsockname (lsock, (struct sockaddr *) &sin, &length)) {
        elog (NOTICE,
         "sendBunloadData() -- getsockname() failed: errno=%d",
          errno);
        return OBJ_ERR_SOCKET;
    }
    lport = ntohs (sin.sin_port);
    lport = genPortInp (lport);
    if (getsockname (myComm->commFd, (struct sockaddr *) &sin, &length)) {
        elog (NOTICE,
         "setupSrvPortal() -- getsockname() of myComm failed: errno=%d",
          errno);
        return OBJ_ERR_SOCKET;
    }

    laddr = inet_ntoa (sin.sin_addr);

    /* a hack. fake a return msg to client. From this point onward, returned
     * value must be DONT_SEND_RETURN */

    thisPortList.numAddr = 1;
    thisPortList.hostAddr = laddr;
    thisPortList.portNum = &lport;

    listen (lsock, SOMAXCONN); 

    status = sendPortList (myComm, &thisPortList);
    if (status < 0) {
        return status;
    }

#ifdef foo
    status = packMsg ((char *) &thisPortList, &retVal, portList_t_PF,
     NULL);

    if (status < 0) {
        return status;
    }

    VAROUTSTAT(retVal) = htonl (status);

    myFunctCall.retlen = -1;

    sendFunctionResult (myComm, &myFunctCall, (char *) retVal);
#endif


    *mylport = lport;
    return (lsock);
}

int
sendPortList (svrComm_t *myComm, portList_t *thisPortList)
{
    struct varlena * retVal;
    FunctCall myFunctCall;
    int status;


    status = packMsg ((char *) thisPortList, &retVal, portList_t_PF,
     NULL);

    if (status < 0) {
	elog (NOTICE,
	 "sendPortList: packMsg of portList_t_PF failed, status = %d",
	 status);
        return status;
    }

    status = NEW_PORTLIST;

    VAROUTSTAT(retVal) = htonl (status);

    myFunctCall.retlen = -1;

    sendFunctionResult (myComm, &myFunctCall, (char *) retVal);

    return (0);
}

int 
acceptSrvPortal (int lsock, int lport)
{
    int myFd;
    int myCookie;
    int cookie, nbytes;

    myFd = accept (lsock, 0, 0);
    if (myFd < 0) {
        elog (NOTICE,
         "acceptSrvPortal() -- accept() failed: errno=%d",
          errno);
        return OBJ_ERR_SOCKET;
    }
    cookie = getCookieFromPortInp (lport);
    if (cookie != 0) {
#ifdef _WIN32
        nbytes = recv(myFd,&myCookie,sizeof(myCookie),0);
#else
        nbytes = read (myFd, &myCookie,sizeof (myCookie));
#endif
        myCookie = ntohl (myCookie);
        if (nbytes != sizeof (myCookie) || myCookie != cookie) {
            elog (NOTICE,
             "sendBunloadData: cookie err, bytes read=%d,cookie=%d,inCookie=%d",
              nbytes, cookie, myCookie);
#ifdef _WIN32
            closesocket(myFd);
#else
            close (myFd);
#endif
            return OBJ_ERR_SOCKET;
        }
    }
    return (myFd);
}

int
write1stHeaderToPortal (int destFd, srb_long_t objSize, int numThreads)
{
    struct firstHeader myHeader;
    int status;

    myHeader.operation = htonl (FIRST_HEADER);
    myHeader.numThreads = htonl (numThreads);
    myHeader.size = objSize;
    srbHtonll (&myHeader.size);

    status = write (destFd, (void *) &myHeader, sizeof (myHeader));

    if (status <= 0) {
        elog (NOTICE,
         "write1stHeaderToPortal: error writing header to client, errno = %d", 
	  errno);
        return (SYS_ERR_REMOTE_CONN);
    }
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
    struct hostElement  *hostTabPtr;
    int remoteFlag;
    char *portNum, tmpResLoc[MAX_TOKEN];
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
        int queFlag;

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
            if (tmpResInfo->flag == LOCAL_HOST) {
                queResInfo (&localAlmostFilledResInfo, tmpResInfo, 
                 QUE_BY_FREE_SPACE);
            } else {
                queResInfo (&almostFilledResInfo, tmpResInfo, 
                 QUE_BY_FREE_SPACE);
            }
	} else {
            if (tmpResInfo->flag == LOCAL_HOST) {
                queResInfo (&localFilledResInfo, tmpResInfo, 
                 QUE_BY_FREE_SPACE);
            } else {
                queResInfo (&filledResInfo, tmpResInfo, 
                 QUE_BY_FREE_SPACE);
            }
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

int
setupSockForBunload (char *clHostAddr, int clPort, svrComm_t *myComm, int flag)
{
    int myFd = -1;
    FunctCall myFunctCall;

    if (flag & CL_INIT_CONN_FLAG) {
        int lsock = -1, lport;

        lsock = setupSrvPortal (myComm, &lport);
        if (lsock < 0) {
            elog (NOTICE,
             "sendBunloadData() -- setupSrvPortal() failed: status=%d",
              lsock);
            return lsock;
        }
       myFd = acceptSrvPortal (lsock, lport);
       if (myFd < 0) {
           return (myFd);
       }
    } else {
        myFd = portalConnect (clPort, clHostAddr);
        if (myFd < 0) {   /* error */
            /* use fprintf intead of elog because multi error msg to client
             * can cause core dump */
            fprintf (stderr,
             "sendBunloadData: Unable to connect to port %d at %s\n",
             clPort & 0xffff, clHostAddr);
            return myFd;
        }

        /* a hack. fake a return msg to client. From this point onward, returned
         * value must be DONT_SEND_RETURN */

        myFunctCall.retlen = SIZEOF32;

        sendFunctionResult (myComm, &myFunctCall, (char *) 0);
    }

    return (myFd);
}

int 
svrModifyForBulkMove(int catType, int operCode, struct varlena *vOperString,
		     struct varlena *inDataInfo)
{
  char *operString;
  mdasC_sql_result_struct *myresult;
  int retVal;
  
  if (TicketUserFlag) {
    elog (NOTICE, "Illegal operation for a ticket user");
    return (ILLEGAL_OPR_TICKET_USER);
  }
  
  operString = varToStr (vOperString);
  retVal = unpackMsg (VAROUTDATA (inDataInfo), (char **) &myresult,
		      sqlResultStruct_PF, NULL);
  if (retVal < 0) {
    elog(NOTICE,
	 "svrModifyForBulkMove:unpackMsg() of myresult error, status = %d",
	 retVal);
    free (operString);
    return (retVal);
  }
  
  retVal = _svrModifyForBulkMove ( catType, operCode, operString, myresult);
  free (operString);
  freeSqlResult (myresult);
  
  return retVal;
  
  
}


int
_svrModifyForBulkMove (int catType, 
		       int operCode,  char *operString,
		       mdasC_sql_result_struct *inDataInfo)
{
  int retVal;
  int remoteFlag;
  struct hostElement  *hostTabPtr;
  
  
  if (operString[0] == '/')
    remoteFlag = getMdasEnabledHost (operString,&hostTabPtr);
  else
    remoteFlag = getMdasEnabledHost (NULL,&hostTabPtr);
  if (remoteFlag < 0) {
    retVal = remoteFlag;
  } 
  else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
    retVal = modify_for_bulk_move (catType,operCode,ClientUser->userName, ClientUser->domainName, 
				   operString, inDataInfo);
    if (retVal == 0) {
      commit_db2_interaction (MDAS_CAT_COMMIT);
    }
    else {
      elog (NOTICE,
	    "svrModifyForBulkMove: register error involving operString %s, stat=%d",
	    operString, retVal);
      commit_db2_interaction (MDAS_CAT_ROLLBACK);
    }
#endif
  } 
  else {
    if ((retVal = remoteConnect (hostTabPtr)) < 0) {
      return retVal;
    }
    
    /* Do a client call to the remote host */
    
    retVal = srbModifyForBulkMove (hostTabPtr->conn, catType,
				   operCode,  operString,inDataInfo);
  }  
  
  return retVal;
  
}

int
svrBulkMove (int catType, struct varlena *vCollectionName, int flag,
struct varlena *vSrcResource, struct varlena *vDestResource,
struct varlena *vContainerName)
{
    char *objID, *collectionName, *srcResource, *destResource, *containerName;
    int status;
    struct mdasResInfo *destResInfo = NULL;
    struct hostElement  *hostTabPtr = NULL;

    collectionName = varToStr (vCollectionName);
    srcResource = varToStr (vSrcResource);
    destResource = varToStr (vDestResource);
    containerName = varToStr (vContainerName);

    if (containerName != NULL && strlen (containerName) > 0) {
        status = _svrBulkMoveToCont (catType, collectionName, flag,
         srcResource, "", containerName);
    } else {
	status = setWhereToStartBulkOpr (catType, flag, destResource, 
	  collectionName, &destResInfo, &hostTabPtr);
	if (status == LOCAL_HOST) {
            status = _svrBulkMove (catType, collectionName, flag,
             srcResource, destResInfo, NULL);
	} else if (status == REMOTE_HOST) {
            if ((status = remoteConnect (hostTabPtr)) >= 0) {
                status = srbBulkMove (hostTabPtr->conn, catType,
                  collectionName, flag | BL_LOCAL_FLAG, srcResource,
                  destResInfo->phyResName, NULL);
	    }
        }
    }

    free (collectionName);
    free (srcResource);
    free (destResource);
    free (containerName);

    return (status);
}

int
svrBulkCopy (int catType, struct varlena *vCollectionName, int flag,
struct varlena *vDestResource, struct varlena *vDestCollection)
{
    char *objID, *collectionName, *destResource, *destCollection;
    int status;
    mdasC_sql_result_struct *myresult = NULL;
    int continuation_index = 1;
    char *container = NULL;
    char *contReplTime = NULL;
    char *contCreateTime = NULL;
    char prevCont[MAX_TOKEN];
    struct mdasResInfo *destResInfo = NULL;
    struct hostElement  *hostTabPtr = NULL;

    collectionName = varToStr (vCollectionName);
    destResource = varToStr (vDestResource);
    destCollection = varToStr (vDestCollection);

    status = setWhereToStartBulkOpr (catType, flag, destResource,
      collectionName, &destResInfo, &hostTabPtr);
    if (status == LOCAL_HOST) {
        status = _svrBulkMove (catType, collectionName, flag | BL_COPY_FLAG,
         NULL, destResInfo, destCollection);
    } else if (status == REMOTE_HOST) {
        if ((status = remoteConnect (hostTabPtr)) >= 0) {
            status = srbBulkCopy (hostTabPtr->conn, catType,
              collectionName, flag | BL_LOCAL_FLAG,
              destResInfo->phyResName, destCollection);
        }
        free (collectionName);
        free (destResource);
        free (destCollection);

        return (status);
    }

    if (status < 0) {
        if (status != MCAT_INQUIRE_ERROR) {
	    free (collectionName);
	    free (destResource);
	    free (destCollection);
	    return (status);
        } else {
            status = 0;  
	}
    }

    status = svrQueryContainerByColl (collectionName, &myresult);

    if (status < 0) {
        if (status == MCAT_INQUIRE_ERROR) {
            status = 0;
	}
        free (collectionName);
        free (destResource);
        free (destCollection);
        return (status);
    }

    prevCont[0] = '\0';
    while (continuation_index >= 0) {
	container = (char *) getFromResultStruct( myresult,
              dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);
        contReplTime = (char *) getFromResultStruct(myresult,
              dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
        contCreateTime = (char *) getFromResultStruct(myresult,
              dcs_tname[DATA_CREATE_TIMESTAMP],
              dcs_aname[DATA_CREATE_TIMESTAMP]);

       for (i = 0; i < myresult->row_count; i++) {
            if (strcmp (container, prevCont) != 0) {
		status = bulkCopyFromCont (catType, collectionName, 
	          destCollection, destResInfo, 
		  container, contReplTime, contCreateTime, flag | BL_COPY_FLAG);
                if (status < 0 && status != MCAT_INQUIRE_ERROR) {
	            freeSqlResult (myresult);
                    return (status);
                }
            }
            strcpy (prevCont,  container);
            container += MAX_DATA_SIZE;
            contReplTime += MAX_DATA_SIZE;
            contCreateTime += MAX_DATA_SIZE;
        }
	continuation_index = myresult->continuation_index;
	freeSqlResult (myresult);

	if (continuation_index >= 0) {
	    status = _svrGetMoreRows (catType, continuation_index, &myresult,
	      DEF_NUM_ROWS);
	    if (status < 0) {
		if (status == MCAT_INQUIRE_ERROR) {
		    status = 0;   
		}
		break;
	    }
	}
    }

    free (collectionName);
    free (destResource);
    free (destCollection);

    return (status);
}

int
_svrBulkMove (int catType, char *collectionName, int flag,
char *srcResource, struct mdasResInfo *destResInfo, char *destCollection)
{
    char *destResource;
    int remoteFlag;
    char *portNum, tmpResLoc[MAX_TOKEN];
    struct hostElement  *hostTabPtr, *tmpHostTabPtr;
    mdasC_sql_result_struct *myresult = NULL;
    int status = 0;

    /* _svrBulkMove should be called at the target resource */

    /* take out BL_LOCAL_FLAG or getBulkUnloadHost won't work */ 
    flag = flag ^ BL_LOCAL_FLAG;

    destResource = destResInfo->phyResName;

    /* got here when we want to do it locally. */

    remoteFlag = getBestSrcForBulkMove (catType, collectionName, flag,
      srcResource, destResource, &myresult, &tmpHostTabPtr);

    if (remoteFlag == REMOTE_HOST) {
	int srcFd = -1; 
	srbConn *conn = NULL;

        if (tmpHostTabPtr->mdasEnabled > 0) {
            /* need to do this or deadlock */
            conn = svrConnect (tmpHostTabPtr->alias->hostName,
             tmpHostTabPtr->zoneHostTab->portNum, NULL, 0, tmpHostTabPtr);
            status = clStatus(conn);
            if (status != CLI_CONNECTION_OK) {
                elog (NOTICE,"Remote connect failed to %s: status = %d", 
		 tmpHostTabPtr->alias->hostName, status);
                if (status < START_OF_ERR_CODE) {
                    /* valid code */
                    return status;
                } else {
                    /* generic code */
                    return (SYS_ERR_REMOTE_CONN);
                }
            }
	    srcFd = srbBulkUnloadForMove (conn, catType, flag,
	      collectionName, srcResource, destResource, &myresult);
        } else {
	    status = remoteConnect (tmpHostTabPtr);
	    if (status < 0) {
                return (status);
	    }
	    srcFd = srbBulkUnloadForMove (tmpHostTabPtr->conn, catType, flag,
	      collectionName, srcResource, destResource, &myresult);
        }
	if (srcFd < 0) {
	    return (srcFd);
	}
	status = unfoldBulkMoveData (srcFd, destResInfo, myresult, flag,
	  collectionName, destCollection);
	if (conn != NULL)
	    clFinish (conn);
    } else {
	if (remoteFlag < 0 || myresult == NULL) {
            status = queryBulkUnloadForMove (catType, flag, collectionName,
              srcResource, destResource, &myresult);
            if (status < 0) {
                return (status);
            }
            if (myresult == NULL || myresult->result_count <= 0) {
                elog(NOTICE,
                 "_svrBulkMove: queryBulkUnloadForMove returns no result ");
                return SYS_ERR_MDAS_CAT_RET;
	    }
        }
	status = localBulkMoveData (catType, collectionName, destResInfo, 
	 myresult, flag, destCollection);
    }

    if (status < 0) {
        elog(NOTICE,
	 "_svrBulkMove: BulkMoveData of %s failed. status = %d",
	  collectionName, status);
	return (status);
    }
	
    return (0);
}

int
svrBulkUnloadForMove(int catType, int flag, struct varlena *vClHostAddr, 
int clPort, struct varlena *vCollection, struct varlena *vSrcResource, 
struct varlena *vTargResource, svrComm_t *myComm)
{
    char *collection;
    char *clHostAddr;
    char *srcResource, *targResource;
    int status;

    clHostAddr = varToStr (vClHostAddr);
    collection = varToStr (vCollection);
    srcResource = varToStr (vSrcResource);
    targResource = varToStr (vTargResource);

    status = _svrBulkUnloadForMove (catType, flag, clHostAddr, clPort, 
     collection, srcResource, targResource, myComm);

    free (collection);
    free (clHostAddr);
    free (srcResource);
    free (targResource);

    return (status);
}

int
_svrBulkUnloadForMove (int catType, int flag, char *clHostAddr,
int clPort, char *collection, char *srcResource, char *targResource,
svrComm_t *myComm)
{
    int status;
    mdasC_sql_result_struct *myresult = NULL, *savedResult = NULL;
    int remoteFlag;
    struct hostElement  *hostTabPtr, *tmpHostTabPtr;
    int fileCnt;
    struct bunloadSort bunloadHead;
    int continuation_index;
    int myFd = -1;

    status = queryBulkUnloadForMove (catType, flag, collection, 
     srcResource, targResource, &myresult);

    if (status < 0) {
	return (status);
    }

    if (myresult == NULL || myresult->result_count <= 0) {
        elog(NOTICE,
         "_svrBulkUnloadForMove: queryBulkUnloadForMove returns no result ");
        return SYS_ERR_MDAS_CAT_RET;
    }

    myFd = setupSockForBunload (clHostAddr, clPort, myComm, flag);
    if (myFd < 0) {
       if (bunloadHead.myresult != NULL)
            freeSqlResult (bunloadHead.myresult);
        if (bunloadHead.unloadFlag != NULL)
            free (bunloadHead.unloadFlag);
        return myFd;
    }

    status = sendBmoveData (myresult, clHostAddr, clPort, myComm, flag,
     myFd);

    if (status < 0) {
        elog(NOTICE,
         "_svrBulkUnloadForMove:sendBmoveData error ,status=%d", status);
        freeSqlResult (myresult);
#ifdef _WIN32
        closesocket(myFd);
#else
        close (myFd);
#endif
        return (DONT_SEND_RETURN);
    }

    continuation_index = myresult->continuation_index;
    savedResult = myresult;
    myresult = NULL;
#ifdef foo
    if (myresult != NULL) {
        freeSqlResult (myresult);
        myresult = NULL;
    }
#endif

    if (continuation_index >= 0) {
        remoteFlag = getMdasEnabledHost (collection, &hostTabPtr);

        if (remoteFlag < 0) {
            elog(NOTICE,
             "_svrBulkUnloadForMove:getMdasEnabledHost error for %s,status=%d",
              collection, remoteFlag);
#ifdef _WIN32
            closesocket(myFd);
#else
            close (myFd);
#endif
            return (DONT_SEND_RETURN);
	}
    }

    while (continuation_index >= 0) {
        myresult = NULL;
        if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
            status = get_more_rows (catType, continuation_index, &myresult,
             BULK_MOVE_NUM_ROWS);
#endif
        } else {
            status = remoteGetMoreRows (catType, continuation_index, &myresult, 
             BULK_MOVE_NUM_ROWS, hostTabPtr);
        }

        if (status < 0 || myresult == NULL || myresult->row_count <= 0) {
            break;
        }
	addZoneToResult (myresult, collection);

        continuation_index = myresult->continuation_index;
        status = sendBmoveData (myresult, clHostAddr, clPort, myComm,
         flag, myFd);

        if (status < 0) {
            elog(NOTICE,
             "_svrBulkUnloadForMove:sendBmoveData error ,status=%d", status);
	    break;
        }

	status = procMovedResult (&savedResult, myFd, flag);
#ifdef foo
        if (myresult != NULL) {
            freeSqlResult (myresult);
            myresult = NULL;
        }
#endif
        if (status < 0) {
	    break;
        }
        savedResult = myresult;
        myresult = NULL;
    }


    if (status < 0) {
	int myStatus;

        myStatus = procMovedResult (&savedResult, myFd, flag);
	if (status == MCAT_INQUIRE_ERROR && myStatus >= 0) {
            sendDoneToPortal (myFd);
	} else {
	    /* not sending sendDoneToPortal will cause error on the other
	     * end */
            printOrphan (savedResult);
            printOrphan (myresult);
	}
        freeSqlResult (savedResult);
        freeSqlResult (myresult);
#ifdef _WIN32
        closesocket(myFd);
#else
        close (myFd);
#endif
        return (DONT_SEND_RETURN);
    }

    status = procMovedResult (&savedResult, myFd, flag);

    sendDoneToPortal (myFd);

#ifdef _WIN32
    closesocket(myFd);
#else
    close (myFd);
#endif

    return (DONT_SEND_RETURN);
}

int
procMovedResult (mdasC_sql_result_struct **savedResult, int remoteFd,
int flag)
{
    int status = 0;

    if (*savedResult != NULL) {
        status = readDelHeader (remoteFd, *savedResult);
        if (status < 0) {
            elog(NOTICE,
             "procMovedResult:readDelHeader error ,status=%d",
              status);
            return (status);
        }
        if ((flag & BL_COPY_FLAG) == 0) {
            status = delMovedSrcData (*savedResult);
            if (status < 0) {
                elog(NOTICE,
                 "procMovedResult:delMovedSrcData error ,status=%d",
                 status);
                return (status);
            }
        }
        freeSqlResult (*savedResult);
        *savedResult = NULL;
    }
    return (status);
}

int
queryBulkUnloadForMove (int catType, int flag, char *collection, 
char *srcResource, char *targResource, mdasC_sql_result_struct **myresult)
{
    int i, status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    char tmpStr[MAX_TOKEN];
    char *mcatName;
    char *phyResLoc;
   
    memset (qval, 0, sizeof (qval));
    memset (selval, 0, sizeof (selval));

    if (ClientUser->privUserFlag != LOCAL_PRIV_USER) {
	/* need to check permission */
	sprintf(qval[DATA_ACCESS_PRIVILEGE]," like '%%''write''%%'");
	sprintf (qval[USER_NAME], " = '%s'", ClientUser->userName);
	sprintf (qval[DOMAIN_DESC], " = '%s'", ClientUser->domainName);
    }
	
    if (flag & BL_COPY_FLAG) {
        selval[DATA_TYP_NAME] = 1;
        selval[REPL_TIMESTAMP] = 1;
        selval[DATA_CREATE_TIMESTAMP] = 1;
    } else {
        if (srcResource != NULL && strlen (srcResource) > 0) {
	    status = setMultiRescQuery (qval[PHY_RSRC_NAME], srcResource);
	    if (status < 0) {
	        elog (NOTICE, 
	         "queryBulkUnloadForMove:setMultiRescQuery of %s err,status=%d",
	          srcResource, status);
	        return (status);
	    }
        } else {
	    if (targResource != NULL && strlen (targResource) > 0) { 
	        sprintf (qval[PHY_RSRC_NAME], " not in ('%s')", targResource);
	    }
	}
    }

    sprintf(qval[CONTAINER_NAME]," is NULL || ='' ");
    sprintf(qval[DATA_GRP_NAME],
      " like '%s' ESCAPE '\\' || like '%s/%%' ESCAPE '\\'",
      collection, collection);

    selval[DATA_NAME] = 1;
    selval[DATA_GRP_NAME] = 1;
    selval[PHY_RSRC_TYP_NAME] = 1;
    selval[DATA_VER_NUM] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[PATH_NAME] = 1;
    selval[RSRC_ADDR_NETPREFIX] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[RSRC_CLASS] = 1;
    selval[SIZE] = 1;
    selval[DATA_CHECKSUM] = 1;

    status = queryDataInfo (collection, selval, (char (*) [MAX_TOKEN]) qval,
     myresult, BULK_MOVE_NUM_ROWS);

    if (status < 0) 
	return status;

    addZoneToResult (*myresult, collection);

    return status;
}

int
addZoneToResult (mdasC_sql_result_struct *myresult, char *mcatHint)
{
    int i;
    int remoteFlag;
    struct hostElement  *hostTabPtr;
    char tmpStr[MAX_TOKEN];
    char *mcatName;
    char *phyResLoc;

    if (myresult == NULL)
	return 0;

    remoteFlag = getMdasEnabledHost (mcatHint, &hostTabPtr);
    if (remoteFlag < 0) {
        elog (NOTICE,
          "queryBulkUnloadForMove: getMdasEnabledHost failed for %s",
          mcatHint);
        return remoteFlag;
    }

    mcatName = hostTabPtr->serverAuthInfo->mcatName;

    phyResLoc = (char *)get_from_result_struct(
                  myresult,
                  dcs_tname[RSRC_ADDR_NETPREFIX],
                    dcs_aname[RSRC_ADDR_NETPREFIX]);

    if (phyResLoc == NULL)
	return 0;

    for (i = 0; i < myresult->row_count; i++) {
        sprintf (tmpStr, "%s&%s", mcatName, phyResLoc);
        strcpy (phyResLoc, tmpStr);
        phyResLoc += MAX_DATA_SIZE;
    }
}

int
setMultiRescQuery (char *qval, char *srcResource)
{
    char *tmpPtr, *startPtr;
    int c;
    char tmpResource[MAX_BMOVE_SRC_RESC][MAX_TOKEN];
    int rescInx = 0, cnt = 0;

    memset (tmpResource, 0, sizeof (tmpResource));

    if (strlen (srcResource) >= MAX_TOKEN) {
	elog (NOTICE, "setMultiRescQuery: Input srcResource %s too long", 
	  srcResource);
	return (INP_ERR_RES_FORMAT);
    }
	
	
    tmpPtr = startPtr = srcResource;
    while ((c = *tmpPtr) != '\0') {
	tmpPtr ++;
        if (c == '|') {
	    if (cnt > 0) {
		strncpy (tmpResource[rescInx], startPtr, cnt);
		rescInx++;
		if (rescInx >= MAX_BMOVE_SRC_RESC) {
		    elog (NOTICE, 
		     "setMultiRescQuery: srcReso %s has more than %d reso",
		      srcResource, MAX_BMOVE_SRC_RESC);
		    return (INP_ERR_RES_FORMAT);
		}
	    }
	    cnt = 0;
	    startPtr = tmpPtr;
	} else {
	    cnt ++;
	}
    }
    if (cnt > 0) {
        strncpy (tmpResource[rescInx], startPtr, cnt);
        rescInx++;
    }
    /* now write the qval */

    sprintf (qval, "in (");

    for (i = 0; i < rescInx; i++) {
	tmpPtr = qval + strlen (qval);
	if (i == rescInx - 1) {		/* last */
	    sprintf (tmpPtr, "'%s')", tmpResource[i]);
	} else {
	    sprintf (tmpPtr, "'%s',", tmpResource[i]);
	}
    }
    if (strlen (qval) >= MAX_TOKEN) {
	elog (NOTICE, "setMultiRescQuery: qval for srcResource %s too long", 
	  srcResource);
	return (INP_ERR_RES_FORMAT);
    }
    return (0);
}

int
sendBmoveData (mdasC_sql_result_struct *myresult, char *clHostAddr, int clPort,
svrComm_t *myComm, int flag, int myFd)
{
    int status, i;
    char *objID, *resourceType, *dataPath, *resourceLoc, *resourceName,
    *dataType, *replNum, *offset, *size, *phyResClass, *containerName,
    *isDirty, *collection, *chksum;
    char *filePtr, *collPtr, *sizePtr, *offsetPtr, *chksumPtr;
    struct varlena * packedResult;
    byteStream myBS;
    FunctCall myFunctCall;
    int bufOffset;
    char *buf, *bufptr;
    int fileCnt = 0;
    srb_long_t myOffset = 0;

    if (myresult == NULL || myresult->row_count <= 0) {
        elog(NOTICE,
         "sendBmoveData: Number of files to unload is zero");
        return SYS_ERR_MDAS_CAT_RET;
    }

    /* add offset to myresult */
    i = myresult->result_count;
    myresult->sqlresult[i].values = 
      malloc (myresult->row_count * MAX_DATA_SIZE);
    if (myresult->sqlresult[i].values == NULL) {
        elog(NOTICE,"sendBmoveData: malloc error");
        return SYS_ERR_MALLOC;
    }
    memset (myresult->sqlresult[i].values, 0, 
     myresult->row_count * MAX_DATA_SIZE);
    myresult->sqlresult[i].tab_name = strdup (dcs_tname[OFFSET]);
    myresult->sqlresult[i].att_name = strdup (dcs_aname[OFFSET]);
    myresult->result_count++;

    offsetPtr = myresult->sqlresult[i].values;
    size = (char *) get_from_result_struct(
    (mdasC_sql_result_struct *) myresult,
        dcs_tname[SIZE], dcs_aname[SIZE]);

    for (i = 0; i < myresult->row_count; i ++) {
#if defined(PORTNAME_osx)
        sprintf (&offsetPtr[i*MAX_DATA_SIZE], "%-qd", myOffset);
#else
        sprintf (&offsetPtr[i*MAX_DATA_SIZE], "%-lld", myOffset);
#endif
	myOffset += strtoll (&size[i*MAX_DATA_SIZE], 0, 0);
    }
    /* Send myresult */

    status = packMsg ((char *) myresult, &packedResult, sqlResultStruct_PF,
     NULL);

    if (status < 0) {
        elog (NOTICE, "sendBmoveData: packMsg error. status = %d", status);
        freeSqlResult (myresult);
        return status;
    }

    myBS.len = VARSIZE (packedResult) - VAROUTHDRSZ;
    myBS.bytes = VAROUTDATA(packedResult);

    /* send the packed myresult */

    status = writeToPortal (myFd, myBS.bytes, 0, myBS.len);

    free (packedResult);

    /* send the data */

    myOffset = 0;
    bufOffset = 0;
    buf = malloc (BL_BUFSIZE);
    bufptr = buf;

    resourceType = (char *)get_from_result_struct(
    (mdasC_sql_result_struct *) myresult, dcs_tname[PHY_RSRC_TYP_NAME],
        dcs_aname[PHY_RSRC_TYP_NAME]);
    dataPath = (char *) get_from_result_struct(
    (mdasC_sql_result_struct *) myresult, dcs_tname[PATH_NAME],
      dcs_aname[PATH_NAME]);
    resourceLoc = (char *) get_from_result_struct(
    (mdasC_sql_result_struct *) myresult, dcs_tname[RSRC_ADDR_NETPREFIX],
      dcs_aname[RSRC_ADDR_NETPREFIX]);

    collection = (char *) get_from_result_struct(
     (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    for (i = 0; i < myresult->row_count; i++) {
        int objTypeInx, exfInx, space, readLen, nbytes;
        srb_long_t toread;

        objTypeInx = objTypeLookUp (&resourceType[i*MAX_DATA_SIZE]);

#ifdef foo
#ifdef FED_MCAT
        tmpLoc = getAddrWithZone (&resourceLoc[i*MAX_DATA_SIZE],
         &collection[i*MAX_DATA_SIZE]);
#else
        tmpLoc = &resourceLoc[i*MAX_DATA_SIZE];
#endif
#endif
        exfInx = lowLevelOpen (objTypeInx, &dataPath[i*MAX_DATA_SIZE],
         &resourceLoc[i*MAX_DATA_SIZE], O_RDONLY);

#ifdef foo
#ifdef FED_MCAT
        free (tmpLoc);
#endif
#endif
        if (exfInx < 0) {
            fprintf (stderr,
             "sendBmoveData: unable to open %s at %s, stat = %d\n",
              &dataPath[i*MAX_DATA_SIZE], &resourceLoc[i*MAX_DATA_SIZE],
              exfInx);
            close (myFd);
            free (buf);
            return (DONT_SEND_RETURN);
        }

        toread = strtoll (&size[i*MAX_DATA_SIZE], 0, 0);

        while (toread > 0) {
            space = BL_BUFSIZE - bufOffset;

            if (toread > space && bufOffset > 0) {
                /* dump the buf */
                status = writeToPortal (myFd, buf, myOffset, bufOffset);
                if (status <= 0) {
                    fprintf (stderr,
                     "sendBmoveData: write data error.errno=%d\n", errno);
                    close (myFd);
                    free (buf);
                    return (DONT_SEND_RETURN);
                }

                myOffset += bufOffset;
                bufOffset = 0;
                bufptr = buf;
                readLen = BL_BUFSIZE;
            } else {
                if (toread > BL_BUFSIZE)
                    readLen = BL_BUFSIZE;
                else
                    readLen = toread;
                bufptr = buf + bufOffset;
            }
            nbytes = _objRead (exfInx, objTypeInx, bufptr, readLen);
            if (nbytes <= 0) {
                fprintf (stderr,
                 "sendBmoveData: read error for %s at %s, toread=%d, read=%d, errno=%d\n",
                  &dataPath[i*MAX_DATA_SIZE], &resourceLoc[i*MAX_DATA_SIZE],
                  toread, nbytes, errno);
                close (myFd);
                free (buf);
                return (DONT_SEND_RETURN);
            }
            toread -= nbytes;
            bufOffset += nbytes;
        }
        lowLevelClose (objTypeInx, exfInx);
    }
    if (bufOffset > 0) {
        status = writeToPortal (myFd, buf, myOffset, bufOffset);
        if (status <= 0) {
            fprintf (stderr,
             "sendBmoveData: write data error.errno=%d\n", errno);
        }
    }
    free (buf);

    return (0);
}

int
unfoldBulkMoveData (int srcFd, struct mdasResInfo *destResInfo, 
mdasC_sql_result_struct *inresult, int flag, 
char *srcCollection, char *destCollection)
{
    int continuation_index = 1;
    srb_long_t offset, size;
    srb_long_t curOffset;
    int local_fd;
    int status;
    char *inCollection, *inObjID, *inSize, *inOffset, *inChksum, *inReplNum,
      *inVerNum;
    char *outCollection, *outObjID, *outReplNum, *outVerNum, *outPath;
    srb_long_t totalSz = 0;
    int toRead;
    int destTypeInx, inx, cnt;
    mdasC_sql_result_struct outresult;
    char *buf;
    char *outDataType, *outSize, *outOffset, *outReplTime, *outCreateTime,
      *outChksum, *inDataType, *inReplTime, *inCreateTime;

    buf = malloc (BL_BUFSIZE);
    if (buf == NULL) {
        fprintf (stderr, "unfoldBulkUnload: Unable to malloc buf\n");
        close (srcFd);
        return (CLI_ERR_MALLOC);
    }

    destTypeInx = objTypeLookUp (destResInfo->phyResType);

    while (continuation_index >= 0) {
        /* hand craft the outresult */
	if (flag & BL_COPY_FLAG) {
            setupResultForCopy (inresult, &outresult, NULL);
            outDataType = (char *) getFromResultStruct(&outresult,
              dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
            outSize = (char *) getFromResultStruct(&outresult,
              dcs_tname[SIZE], dcs_aname[SIZE]);
            outOffset = (char *) getFromResultStruct(&outresult,
              dcs_tname[OFFSET], dcs_aname[OFFSET]);
            outReplTime = (char *) getFromResultStruct(&outresult,
              dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
            outCreateTime = (char *) getFromResultStruct(&outresult,
              dcs_tname[DATA_CREATE_TIMESTAMP], 
	      dcs_aname[DATA_CREATE_TIMESTAMP]);
            outChksum = (char *) getFromResultStruct(&outresult,
              dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
            inDataType = (char *) getFromResultStruct(
             (mdasC_sql_result_struct *) inresult,
                dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
            inReplTime = (char *) getFromResultStruct(
             (mdasC_sql_result_struct *) inresult,
                dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
            inCreateTime = (char *) getFromResultStruct(
             (mdasC_sql_result_struct *) inresult,
                dcs_tname[DATA_CREATE_TIMESTAMP], 
		dcs_aname[DATA_CREATE_TIMESTAMP]);

	} else {
            setupResultForMove (inresult, &outresult, NULL);
	}

        outCollection = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
        outObjID = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
        outVerNum = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);
        outReplNum = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
        outPath = (char *) getFromResultStruct(&outresult,
          dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);


        inCollection = (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

        inObjID = (char *)getFromResultStruct(
        (mdasC_sql_result_struct *) inresult, dcs_tname[DATA_NAME],
            dcs_aname[DATA_NAME]);
        inSize = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[SIZE], dcs_aname[SIZE]);

        inOffset = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[OFFSET], dcs_aname[OFFSET]);

        inChksum = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);

        inReplNum = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);

        inVerNum = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);

        inx = 0;
        curOffset = 0;
	local_fd = localCreateForMove (inCollection, inObjID, inReplNum,
	 inVerNum, destResInfo, destTypeInx, outCollection, outObjID, 
	 outReplNum, outVerNum, outPath, flag, srcCollection, destCollection);  


        if (local_fd < 0) {
            elog(NOTICE,
             "unfoldBulkMoveData: localCreate of %s/%s failed, status = %d",
              inCollection, inObjID, local_fd);
	    close (srcFd);
	    free (buf);
            return local_fd;
        }

	if (flag & BL_COPY_FLAG) {
	    strcpy (outDataType, &inDataType[inx*MAX_DATA_SIZE]);
	    strcpy (outSize, &inSize[inx*MAX_DATA_SIZE]);
	    strcpy (outOffset, &inOffset[inx*MAX_DATA_SIZE]);
	    strcpy (outReplTime, &inReplTime[inx*MAX_DATA_SIZE]);
	    strcpy (outCreateTime, &inCreateTime[inx*MAX_DATA_SIZE]);
	    strcpy (outChksum, &inChksum[inx*MAX_DATA_SIZE]);
	}

        outresult.row_count++;
        offset = strtoll (&inOffset[inx*MAX_DATA_SIZE], 0, 0);
        size = strtoll (&inSize[inx*MAX_DATA_SIZE], 0, 0);


        status = 0;
        while ((cnt =  readDataFromPortal (srcFd, buf, BL_BUFSIZE)) > 0) {
            char *bufptr;

            if (status < 0)
                break;
           bufptr = buf;
            while (cnt > 0) {
                if (cnt >= size) {
                    status = _objWrite (local_fd, destTypeInx, bufptr, size);
                    if (status < size) {
                        fprintf(stderr,
                         "unfoldBulkMoveData: Read %lld bytes, Wrote %d bytes.\n ",
                         size, status);
                        lowLevelClose (destTypeInx, local_fd);
                        status = -1;
                        break;
                    }
                    cnt -= size;
                    curOffset += size;
                    bufptr += size;
                    lowLevelClose (destTypeInx, local_fd);

                    inx ++;
                    if (inx >= inresult->row_count)
                        break;

                    outCollection += MAX_DATA_SIZE;
                    outObjID += MAX_DATA_SIZE;
                    outPath += MAX_DATA_SIZE;
		    if (flag & BL_COPY_FLAG) {
                        outDataType += MAX_DATA_SIZE;
                        outSize += MAX_DATA_SIZE;
                        outOffset += MAX_DATA_SIZE;
                        outReplTime += MAX_DATA_SIZE;
                        outCreateTime += MAX_DATA_SIZE;
                        outChksum  += MAX_DATA_SIZE;
		    } else {
                        outVerNum += MAX_DATA_SIZE;
                        outReplNum += MAX_DATA_SIZE;
		    }


                    local_fd = localCreateForMove (
		      &inCollection[inx*MAX_DATA_SIZE], 
		      &inObjID[inx*MAX_DATA_SIZE], 
		      &inReplNum[inx*MAX_DATA_SIZE], 
		      &inVerNum[inx*MAX_DATA_SIZE], destResInfo, destTypeInx, 
		      outCollection, outObjID, outReplNum, outVerNum, outPath,
		      flag, srcCollection, destCollection);

                    if (local_fd < 0) {
                        status = local_fd;
                        break;
                    }
                    if (flag & BL_COPY_FLAG) {
                        strcpy (outDataType, &inDataType[inx*MAX_DATA_SIZE]);
                        strcpy (outSize, &inSize[inx*MAX_DATA_SIZE]);
                        strcpy (outOffset, &inOffset[inx*MAX_DATA_SIZE]);
                        strcpy (outReplTime, &inReplTime[inx*MAX_DATA_SIZE]);
                        strcpy (outCreateTime,&inCreateTime[inx*MAX_DATA_SIZE]);
                        strcpy (outChksum, &inChksum[inx*MAX_DATA_SIZE]);
        	    }

                    outresult.row_count++;

                    offset = strtoll (&inOffset[inx*MAX_DATA_SIZE], 0, 0);
                    size = strtoll (&inSize[inx*MAX_DATA_SIZE], 0, 0);

                    if (offset != curOffset) {
                        /* something is wrong */
                        elog (NOTICE,
                         "unfoldBulkMoveData: offset mismatch at %s/%s, %lld:%lld\n ",
                         outCollection, outObjID, offset, curOffset);
                        status = -1;
                        break;
                    }
                } else {        /* cnt < size */
                    status = _objWrite (local_fd, destTypeInx, bufptr, cnt);
                    if (status < cnt) {
                        elog (NOTICE,
                         "unfoldBulkMoveData: Read %d bytes,Wrote %d bytes.\n ",
                         cnt, status);
                        status = -1;
                        break;
                    }
                    size -= cnt;
                    curOffset += cnt;
                    cnt = 0;
                }
            }
            if (inx >= inresult->row_count)
                break;
        }
        if (inx < inresult->row_count) {
            elog (NOTICE,
             "unfoldBulkMoveData: Need to unload %d files but done only %d\n ",
             inresult->row_count, inx);
            status = -1;
            break;
        } else {
            status = 0;
        }
	/* do the registration here */
        if (flag & BL_COPY_FLAG) {
            status = _svrBulkRegister (MDAS_CATALOG, 
	      destResInfo->phyResName, &outresult, 1);
	} else {
	    status = _svrModifyForBulkMove (MDAS_CATALOG, BULK_PHY_MOVE, 
	      destResInfo->phyResName, &outresult);
	}
        continuation_index = inresult->continuation_index;
        if (status < 0) {
            elog (NOTICE,
             "unfoldBulkMoveData: _svrModifyForBulkMove error. ststus = %d\n ",
              status);
            break;
        }

	/* tell the source that it is OK to delete the source files */
	status = sendDelToPortal (srcFd, inresult);
        if (status < 0) {
            elog (NOTICE,
             "unfoldBulkMoveData: sendDelToPortal error. ststus = %d\n ",
              status);
	    printOrphan (inresult);
            break;
        }

        freeSqlResult (inresult);
        clearSqlResult (&outresult);
	outresult.row_count = 0;

        if (continuation_index >= 0) {
            status = getResultForBunload (srcFd, &inresult);
            if (status < 0) {
                if (status == MCAT_INQUIRE_ERROR) {
                    status = 0;
                } else {
                    elog (NOTICE,
                     "unfoldBulkMoveData: getResultForBunload error.status=%d",
                      status);
		}
                break;
            }
        } else {
            break;
        }
    }
    if (status < 0) {
	cleanMovedData (destResInfo, &outresult);
    }

    free (buf);
    close (srcFd);
    return (status);
}


int
setupResultForMove (mdasC_sql_result_struct *inresult, 
mdasC_sql_result_struct *outresult, char *container)
{
    int i, fileCnt;

    memset (outresult, 0, sizeof (mdasC_sql_result_struct));

    if (inresult == NULL || inresult->row_count <= 0) 
	return (0);

    outresult->continuation_index = inresult->continuation_index;
    fileCnt = inresult->row_count;

    outresult->sqlresult[0].tab_name = strdup (dcs_tname[DATA_NAME]);
    outresult->sqlresult[0].att_name = strdup (dcs_aname[DATA_NAME]);
    outresult->sqlresult[1].tab_name = strdup (dcs_tname[DATA_GRP_NAME]);
    outresult->sqlresult[1].att_name = strdup (dcs_aname[DATA_GRP_NAME]);
    outresult->sqlresult[2].tab_name = strdup (dcs_tname[DATA_VER_NUM]);
    outresult->sqlresult[2].att_name = strdup (dcs_aname[DATA_VER_NUM]);
    outresult->sqlresult[3].tab_name = strdup (dcs_tname[DATA_REPL_ENUM]);
    outresult->sqlresult[3].att_name = strdup (dcs_aname[DATA_REPL_ENUM]);

    if (container == NULL) {
        outresult->result_count = 5;
        outresult->sqlresult[4].tab_name = strdup (dcs_tname[PATH_NAME]);
        outresult->sqlresult[4].att_name = strdup (dcs_aname[PATH_NAME]);
    } else {
	outresult->result_count = 6;
        outresult->sqlresult[4].tab_name = strdup (dcs_tname[SIZE]);
        outresult->sqlresult[4].att_name = strdup (dcs_aname[SIZE]);
        outresult->sqlresult[5].tab_name = strdup (dcs_tname[OFFSET]);
        outresult->sqlresult[5].att_name = strdup (dcs_aname[OFFSET]);

    }

    for (i = 0; i < outresult->result_count; i++) {
        outresult->sqlresult[i].values = malloc (fileCnt * MAX_DATA_SIZE);
        if (outresult->sqlresult[i].values == NULL) {
            elog(NOTICE,"setupResultForMove: malloc error");
            return SYS_ERR_MALLOC;
        }
        memset (outresult->sqlresult[i].values, 0, fileCnt * MAX_DATA_SIZE);
    }

    return (0);
}

int
setupResultForCopy (mdasC_sql_result_struct *inresult, 
mdasC_sql_result_struct *outresult, char *container)
{
    int i, fileCnt;

    memset (outresult, 0, sizeof (mdasC_sql_result_struct));

    if (inresult == NULL || inresult->row_count <= 0) 
	return (0);

    outresult->continuation_index = inresult->continuation_index;
    fileCnt = inresult->row_count;

    outresult->sqlresult[0].tab_name = strdup (dcs_tname[DATA_NAME]);
    outresult->sqlresult[0].att_name = strdup (dcs_aname[DATA_NAME]);
    outresult->sqlresult[1].tab_name = strdup (dcs_tname[DATA_GRP_NAME]);
    outresult->sqlresult[1].att_name = strdup (dcs_aname[DATA_GRP_NAME]);
    outresult->sqlresult[2].tab_name = strdup (dcs_tname[DATA_TYP_NAME]);
    outresult->sqlresult[2].att_name = strdup (dcs_aname[DATA_TYP_NAME]);

    outresult->sqlresult[3].tab_name = strdup (dcs_tname[PATH_NAME]);
    outresult->sqlresult[3].att_name = strdup (dcs_aname[PATH_NAME]);
    outresult->sqlresult[4].tab_name = strdup (dcs_tname[SIZE]);
    outresult->sqlresult[4].att_name = strdup (dcs_aname[SIZE]);
    outresult->sqlresult[5].tab_name = strdup (dcs_tname[OFFSET]);
    outresult->sqlresult[5].att_name = strdup (dcs_aname[OFFSET]);
    outresult->sqlresult[6].tab_name = strdup (dcs_tname[REPL_TIMESTAMP]);
    outresult->sqlresult[6].att_name = strdup (dcs_aname[REPL_TIMESTAMP]);
    outresult->sqlresult[7].tab_name = 
      strdup (dcs_tname[DATA_CREATE_TIMESTAMP]);
    outresult->sqlresult[7].att_name = 
      strdup (dcs_aname[DATA_CREATE_TIMESTAMP]);
    outresult->sqlresult[8].tab_name = strdup (dcs_tname[DATA_CHECKSUM]);
    outresult->sqlresult[8].att_name = strdup (dcs_aname[DATA_CHECKSUM]);
    outresult->result_count = 9;

    for (i = 0; i < outresult->result_count; i++) {
        outresult->sqlresult[i].values = malloc (fileCnt * MAX_DATA_SIZE);
        if (outresult->sqlresult[i].values == NULL) {
            elog(NOTICE,"setupResultForMove: malloc error");
            return SYS_ERR_MALLOC;
        }
        memset (outresult->sqlresult[i].values, 0, fileCnt * MAX_DATA_SIZE);
    }

    return (0);
}

int
localCreateForMove (char *inCollection, char *inObjID, char *inReplNum,
char *inVerNum, struct mdasResInfo *destResInfo, int destTypeInx, 
char *outCollection, char *outObjID, char *outReplNum, char *outVerNum, 
char *outPath, int flag, char *srcCollection, char *destCollection)
{
    int status;
        
    status = getFullPathName (destResInfo->logDirPathName,
                          destResInfo->phyDirPathName,
                          ClientUser->userName, ClientUser->domainName,
                          "", inObjID, inCollection, outPath);


    if (status < 0) {
        elog(NOTICE,
         "localCreateForMove: Unable get resolve pathName: %s/%s, status = %d",
          inCollection, inObjID, status);
        return status;
    }

    status = lowLevelCreate (destTypeInx,
             destResInfo->phyResLoc, outPath,
             StorVaultMode, NO_CHKDIR, -1);

    if (status < 0) {
	return (status);
    }

    strcpy (outObjID, inObjID);
    if (flag & BL_COPY_FLAG) {
	sprintf (outCollection, "%s%s", 
	  destCollection, inCollection + strlen (srcCollection));
    } else {
        strcpy (outCollection, inCollection);
        strcpy (outReplNum, inReplNum);
        strcpy (outVerNum, inVerNum);
    }

    return (status);
}

int
cleanMovedData (struct mdasResInfo *destResInfo, 
mdasC_sql_result_struct *outresult)
{
    int i, status;
    int destTypeInx;
    char *outPath;

    if (outresult == NULL || outresult->row_count == 0) {
	return 0;
    }

    outPath = (char *) getFromResultStruct(outresult,
      dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);

    destTypeInx = objTypeLookUp (destResInfo->phyResType);

    for (i = 0; i < outresult->row_count; i++) {
        status = _objUnlink (srbObjTypeEntry[destTypeInx].objType,
        srbObjTypeEntry[destTypeInx].systemType, destResInfo->phyResLoc,
         outPath, NO_CHKDIR);
        if (status < 0) {
            elog (NOTICE, "cleanMovedData:_objUnlink error for %s. status = %d",
             outPath, status);
        }
	outPath += MAX_DATA_SIZE;
    }
    return (status);
}

int
getFirstResc (char *srcResource, char *outResName)
{
    char *tmpPtr, *startPtr;
    int c, cnt = 0;

    if (srcResource == NULL || strlen (srcResource) == 0 ||
      outResName == NULL) {
	return -1;
    }

    if (strlen (srcResource) >= MAX_TOKEN) {
        elog (NOTICE, "getFirstResc: Input srcResource %s too long",
          srcResource);
        return (INP_ERR_RES_FORMAT);
    }

    tmpPtr = startPtr = srcResource;
    while ((c = *tmpPtr) != '\0') {
        tmpPtr ++;
        if (c == '|') {
            if (cnt > 0) {
                strncpy (outResName, startPtr, cnt);
		outResName[cnt] = '\0';
		return (0);
	    }
            startPtr = tmpPtr;
	} else {
	    cnt++;
	}
    }
    if (cnt > 0) {
        strncpy (outResName, startPtr, cnt);
	outResName[cnt] = '\0';
	return (0);
    } else {
	return (-1);
    }
}

int
sendDelToPortal (int destFd, mdasC_sql_result_struct *myresult)
{
    struct relayHeader myHeader;
    char *size;
    int status;

    /* use the first file size for confirmation */
   size = (char *) get_from_result_struct(
    (mdasC_sql_result_struct *) myresult,
        dcs_tname[SIZE], dcs_aname[SIZE]);

    myHeader.operation = htonl (DEL_FILES_IN_RES);
    myHeader.offset =  strtoll (size, 0, 0);
    srbHtonll (&myHeader.offset);
    myHeader.length = htonl (myresult->row_count);

    status = write (destFd, (void *) &myHeader, sizeof (myHeader));

    return (status);
}

int printOrphan (mdasC_sql_result_struct *myresult)
{
    int i;
    char *resourceLoc, *pathName;

    if (myresult == NULL) {
	return (0);
    }
    resourceLoc = (char *) get_from_result_struct(
    (mdasC_sql_result_struct *) myresult, dcs_tname[RSRC_ADDR_NETPREFIX],
      dcs_aname[RSRC_ADDR_NETPREFIX]);

    pathName = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) myresult, dcs_tname[PATH_NAME], 
     dcs_aname[PATH_NAME]);

    fprintf (stderr, "possible orphan :\n");
    for (i = 0; i < myresult->row_count; i++) {
	fprintf (stderr, "%s    %s\n", resourceLoc, pathName);
	resourceLoc += MAX_DATA_SIZE;
	pathName += MAX_DATA_SIZE;
    }

    return (0);
}

int
readDelHeader (int myFd, mdasC_sql_result_struct *savedResult)
{
    struct relayHeader myHeader;
    int status;
    char *size;
    srb_long_t lsize;

    status = readHeader (myFd, &myHeader);

    if (status < 0) {
	elog (NOTICE, "readDelHeader: readHeader error, errno = %d", errno);
	return -1;
    }

    if (savedResult == NULL) {
        elog (NOTICE, "readDelHeader: NULL savedResult");
        return -1;
    }

    if (myHeader.operation != DEL_FILES_IN_RES) {
	elog (NOTICE, 
	 "readDelHeader: operation %d not equal to DEL_FILES_IN_RES", 
	 myHeader.operation);
	return -1;
    }
	
    if (myHeader.length != savedResult->row_count) {
        elog (NOTICE, 
         "readDelHeader: header length %d not equal to %d",          
	myHeader.length, savedResult->row_count);
        return -1;
    }
    /* use the first file size for confirmation */
   size = (char *) get_from_result_struct(
    (mdasC_sql_result_struct *) savedResult,
        dcs_tname[SIZE], dcs_aname[SIZE]);

    lsize =  strtoll (size, 0, 0);

    if (myHeader.offset != lsize) {
        elog (NOTICE,
         "readDelHeader: header offset %lld not equal to %lld",
        myHeader.offset, lsize);
        return -1;
    }
    return (0);
}

int
delMovedSrcData (mdasC_sql_result_struct *myresult)
{
    int i, status;
    char *resourceType, *dataPath, *resourceLoc;
    int destTypeInx;

    if (myresult == NULL || myresult->row_count == 0) {
        return 0;
    }

    resourceType = (char *)get_from_result_struct(
    (mdasC_sql_result_struct *) myresult, dcs_tname[PHY_RSRC_TYP_NAME],
        dcs_aname[PHY_RSRC_TYP_NAME]);
    dataPath = (char *) get_from_result_struct(
    (mdasC_sql_result_struct *) myresult, dcs_tname[PATH_NAME],
      dcs_aname[PATH_NAME]);
    resourceLoc = (char *) get_from_result_struct(
    (mdasC_sql_result_struct *) myresult, dcs_tname[RSRC_ADDR_NETPREFIX],
      dcs_aname[RSRC_ADDR_NETPREFIX]);

    for (i = 0; i < myresult->row_count; i++) {
        destTypeInx = objTypeLookUp (resourceType);
        status = _objUnlink (srbObjTypeEntry[destTypeInx].objType,
        srbObjTypeEntry[destTypeInx].systemType, resourceLoc,
         dataPath, NO_CHKDIR);
        if (status < 0) {
            elog (NOTICE, "delMovedSrcData:_objUnlink error for %s.status=%d",
             dataPath, status);
        }
        dataPath += MAX_DATA_SIZE;
        resourceLoc += MAX_DATA_SIZE;
        resourceType += MAX_DATA_SIZE;
    }
    return (status);
}

int
localBulkMoveData (int catType, char *collection, 
struct mdasResInfo *destResInfo, mdasC_sql_result_struct *inresult, int flag,
char *destCollection)
{
    int continuation_index = 1;
    srb_long_t size;
    int status;
    char *inCollection, *inObjID, *inSize, *inOffset, *inChksum, *inReplNum,
      *inVerNum, *inResourceType, *inResourceLoc, *inPath;
    char *outCollection, *outObjID, *outReplNum, *outVerNum, *outPath;
    int destTypeInx, srcTypeInx;
    mdasC_sql_result_struct outresult;
    struct hostElement  *hostTabPtr;
    int remoteFlag;
    char *outDataType, *outSize, *outOffset, *outReplTime, *outCreateTime,
      *outChksum, *inDataType, *inReplTime, *inCreateTime;

    if (inresult == NULL) {
	return (0);
    }

    if (continuation_index >= 0) {
        remoteFlag = getMdasEnabledHost (collection, &hostTabPtr);

        if (remoteFlag < 0) {
            elog(NOTICE,
             "localBulkMoveData:getMdasEnabledHost error for %s,status=%d",
              collection, remoteFlag);
            return (DONT_SEND_RETURN);
        }
    }

    destTypeInx = objTypeLookUp (destResInfo->phyResType);

    while (continuation_index >= 0) {
        /* hand craft the outresult */
        if (flag & BL_COPY_FLAG) {
            setupResultForCopy (inresult, &outresult, NULL);
            outDataType = (char *) getFromResultStruct(&outresult,
              dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
            outSize = (char *) getFromResultStruct(&outresult,
              dcs_tname[SIZE], dcs_aname[SIZE]);
            outOffset = (char *) getFromResultStruct(&outresult,
              dcs_tname[OFFSET], dcs_aname[OFFSET]);
            outReplTime = (char *) getFromResultStruct(&outresult,
              dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
            outCreateTime = (char *) getFromResultStruct(&outresult,
              dcs_tname[DATA_CREATE_TIMESTAMP],
              dcs_aname[DATA_CREATE_TIMESTAMP]);
            outChksum = (char *) getFromResultStruct(&outresult,
              dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
            inDataType = (char *) getFromResultStruct(
             (mdasC_sql_result_struct *) inresult,
                dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
            inReplTime = (char *) getFromResultStruct(
             (mdasC_sql_result_struct *) inresult,
                dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
            inCreateTime = (char *) getFromResultStruct(
             (mdasC_sql_result_struct *) inresult,
                dcs_tname[DATA_CREATE_TIMESTAMP],
                dcs_aname[DATA_CREATE_TIMESTAMP]);
        } else {
            setupResultForMove (inresult, &outresult, NULL);
	}

        outCollection = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
        outObjID = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
        outVerNum = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);
        outReplNum = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
        outPath = (char *) getFromResultStruct(&outresult,
          dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);


        inCollection = (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

        inObjID = (char *)getFromResultStruct(
        (mdasC_sql_result_struct *) inresult, dcs_tname[DATA_NAME],
            dcs_aname[DATA_NAME]);
        inSize = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[SIZE], dcs_aname[SIZE]);

        inOffset = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[OFFSET], dcs_aname[OFFSET]);

        inChksum = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);

        inReplNum = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);

        inVerNum = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);

        inResourceType = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) inresult, dcs_tname[PHY_RSRC_TYP_NAME],
            dcs_aname[PHY_RSRC_TYP_NAME]);
        inPath = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inresult, dcs_tname[PATH_NAME],
          dcs_aname[PATH_NAME]);
        inResourceLoc = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inresult, dcs_tname[RSRC_ADDR_NETPREFIX],
          dcs_aname[RSRC_ADDR_NETPREFIX]);

	for (i = 0; i < inresult->row_count; i++) {
	    srb_long_t bytesCopied;

    	    status = getFullPathName (destResInfo->logDirPathName,
                          destResInfo->phyDirPathName,
                          ClientUser->userName, ClientUser->domainName,
                          "", inObjID, inCollection, outPath);

    	    if (status < 0) {
                elog(NOTICE,
                 "localBulkMoveData: getFullPathName error: %s/%s,status=%d",
                  inCollection, inObjID, status);
                return status;
            }

	    srcTypeInx = objTypeLookUp (inResourceType);
	    size = strtoll (inSize, 0, 0);
	    outresult.row_count++;

            strcpy (outObjID, inObjID);
            if (flag & BL_COPY_FLAG) {
                sprintf (outCollection, "%s%s", 
                  destCollection, inCollection + strlen (collection));
                strcpy (outDataType, inDataType);
                strcpy (outSize, inSize);
                strcpy (outReplTime, inReplTime);
                strcpy (outCreateTime, inCreateTime);
                strcpy (outChksum, inChksum);
            } else {
                strcpy (outCollection, inCollection);
                strcpy (outReplNum, inReplNum);
                strcpy (outVerNum, inVerNum);
	    }

	    bytesCopied = _svrDataCopyS (destTypeInx, outPath, 
	      destResInfo->phyResLoc, srcTypeInx, inPath, inResourceLoc, size);
	    if (bytesCopied != size) {
		if (bytesCopied < 0) {
		    status = bytesCopied;
		} else {
		    status = OBJ_ERR_COPY_LEN;
	 	}
		cleanMovedData (destResInfo, &outresult);
                freeSqlResult (inresult);
                clearSqlResult (&outresult);
		return (status);
	    }
	    outCollection += MAX_DATA_SIZE;
	    outObjID += MAX_DATA_SIZE;
	    outPath += MAX_DATA_SIZE;
	    inCollection += MAX_DATA_SIZE;
	    inObjID += MAX_DATA_SIZE;
	    inPath += MAX_DATA_SIZE;
            inSize += MAX_DATA_SIZE;
	    if (flag & BL_COPY_FLAG) {
                outDataType += MAX_DATA_SIZE;
                outReplTime += MAX_DATA_SIZE;
                outCreateTime += MAX_DATA_SIZE;
                outChksum += MAX_DATA_SIZE;
	        inDataType += MAX_DATA_SIZE;
	        inReplTime += MAX_DATA_SIZE;
	        inCreateTime += MAX_DATA_SIZE;
                inChksum += MAX_DATA_SIZE;
                outSize += MAX_DATA_SIZE;
	    } else {
                outVerNum += MAX_DATA_SIZE;
                outReplNum += MAX_DATA_SIZE;
                inReplNum += MAX_DATA_SIZE;
                inVerNum += MAX_DATA_SIZE;
                inResourceType += MAX_DATA_SIZE;
	        inResourceLoc += MAX_DATA_SIZE;
	    }
	}
        /* do the registration here */
        if (flag & BL_COPY_FLAG) {
            status = _svrBulkRegister (MDAS_CATALOG, destResInfo->phyResName, 
	      &outresult, 1);
        } else {
            status = _svrModifyForBulkMove (MDAS_CATALOG, BULK_PHY_MOVE,
              destResInfo->phyResName, &outresult);
	}
        if (status < 0) {
            elog (NOTICE,
             "localBulkMoveData: getResultForBunload error. ststus = %d\n ",
              status);
            cleanMovedData (destResInfo, &outresult);
            freeSqlResult (inresult);
            clearSqlResult (&outresult);
            return (status);
	}
        continuation_index = inresult->continuation_index;
        clearSqlResult (&outresult);
        if ((flag & BL_COPY_FLAG) == 0) {
            status = delMovedSrcData (inresult);
            if (status < 0) {
                elog(NOTICE,
                 "localBulkMoveData:delMovedSrcData error ,status=%d",
                 status);
	        printOrphan (inresult);
                freeSqlResult (inresult);
                return (status);
	    }
        }
        freeSqlResult (inresult);
        outresult.row_count = 0;

        if (continuation_index >= 0) {
            if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
                status = get_more_rows (catType, continuation_index, 
		  &inresult, BULK_MOVE_NUM_ROWS);
#endif
            } else {
                status = remoteGetMoreRows (catType, continuation_index, 
		  &inresult, BULK_MOVE_NUM_ROWS, hostTabPtr);
            }

            if (status < 0 || inresult == NULL || inresult->row_count <= 0) {
                if (status == MCAT_INQUIRE_ERROR) {
                    status = 0;
                } else {
                    elog (NOTICE,
                     "localBulkMoveData: get_more_rows error. ststus = %d\n ",
                      status);
                }
                return (status);
            }
	    addZoneToResult (inresult, collection);
	}
    }
    return (0);
}

int 
_svrBulkMoveToCont (int catType, char *collectionName, int flag,
char *srcResource, char *destResource, char *containerName)
{
    int remoteFlag;
    char *portNum, tmpResLoc[MAX_TOKEN];
    struct hostElement  *hostTabPtr, *tmpHostTabPtr;
    mdasC_sql_result_struct *myresult = NULL;
    int status = 0;

    if (flag & BL_LOCAL_FLAG) {
	/* do it locally */
         remoteFlag = LOCAL_HOST;
    } else {
        remoteFlag = getBestSrcForBulkMove (catType, collectionName, flag,
          srcResource, destResource, &myresult, &hostTabPtr);
    }

    if (remoteFlag < 0) {
        return remoteFlag;               /* failure */
    } else if (remoteFlag == REMOTE_HOST) {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            return status;
        } else {
            status = srbBulkMove (hostTabPtr->conn, catType, collectionName,
              flag | BL_LOCAL_FLAG, srcResource, destResource,
              containerName);
            return (status);
        }
    }

    /* do this here locally */

    if (myresult == NULL) {
        status = queryBulkUnloadForMove (catType, flag, collectionName,
          srcResource, destResource, &myresult);
        if (status < 0) {
            return (status);
        }
        if (myresult == NULL || myresult->result_count <= 0) {
            elog(NOTICE,
             "_svrBulkMoveToCont: queryBulkUnloadForMove returns no result ");
            return SYS_ERR_MDAS_CAT_RET;
        }
    }
    status = localBulkMoveDataToCont (catType, collectionName, containerName,
     myresult);


    return (status);
}

int
getBestSrcForBulkMove (int catType, char *collectionName, int flag, 
char *srcResource, char *destResource, 
mdasC_sql_result_struct **myresult, struct hostElement **tmpHostTabPtr)
{
    char phyResName[MAX_TOKEN];
    int remoteFlag;
    char *portNum, tmpResLoc[MAX_TOKEN];
    int status;

    if (flag & BL_COPY_FLAG) {
	status = -1;	/* no source resource for copy */
    } else {
        status = getFirstResc (srcResource, phyResName);
    }

    *myresult = NULL;
    *tmpHostTabPtr = NULL;
    if (status < 0) {   /* no srcResource input */
        status = queryBulkUnloadForMove (catType, flag, collectionName,
          srcResource, destResource, myresult);
        if (status < 0) {
            return (status);
        }
        if (*myresult == NULL || (*myresult)->result_count <= 0) {
            elog(NOTICE,
             "getBestSrcForBulkMove:queryBulkUnloadForMove returns no result ");
            return SYS_ERR_MDAS_CAT_RET;
        }
        getBulkUnloadHost(*myresult, tmpHostTabPtr, flag);
        if (*tmpHostTabPtr == NULL) {
            remoteFlag = LOCAL_HOST;
            flag |= BL_QUERY_DONE;
        } else {
            remoteFlag = REMOTE_HOST;
        }
    } else {
	struct resCache *myCacheRes = NULL;
	struct mdasResInfo *srcResInfo = NULL;

        if (getResInfo (collectionName, catType, phyResName,
         ClientUser->domainName, &myCacheRes) < 0) {
            elog (NOTICE,
              "getBestSrcForBulkMove: Input resc %s is not a registered resc",
              phyResName);
            return (OBJ_ERR_RES_NOT_REG);
        }

        sortResForIngest (myCacheRes, -1);

        srcResInfo = myCacheRes->resInfoHead;
#ifdef FED_MCAT
        remoteFlag = chkHostAndZone (srcResInfo->phyResLoc, tmpHostTabPtr);
#else
        /* Parse the hostAddr */
        strcpy (tmpResLoc, srcResInfo->phyResLoc);
        portNum = parseAddr (tmpResLoc);
        remoteFlag = chkHost (tmpResLoc, portNum, tmpHostTabPtr);
#endif
    }

    return (remoteFlag);
}
int 
localBulkMoveDataToCont (int catType, char *collection, 
char *container, mdasC_sql_result_struct *inresult)
{
    int continuation_index = 1;
    srb_long_t size;
    int status;
    char *inCollection, *inObjID, *inSize, *inOffset, *inChksum, *inReplNum,
      *inVerNum, *inResourceType, *inResourceLoc, *inPath;
    char *outCollection, *outObjID, *outReplNum, *outVerNum, 
     *outSize, *outOffset;
    int srcTypeInx, contInx;
    mdasC_sql_result_struct outresult;
    struct hostElement  *hostTabPtr;
    int remoteFlag;
    char *buf;
    int bufOffset;
    srb_long_t contOffset;

    if (inresult == NULL) {
        return (0);
    }

    if (continuation_index >= 0) {
        remoteFlag = getMdasEnabledHost (collection, &hostTabPtr);

        if (remoteFlag < 0) {
            elog(NOTICE,
            "localBulkMoveDataToCont:getMdasEnabledHost error for %s,status=%d",
              collection, remoteFlag);
            return (DONT_SEND_RETURN);
        }
    }

    contInx = _svrContainerOpen (catType, container, O_RDWR);
    if (contInx < 0) {
        elog(NOTICE,
         "localBulkMoveDataToCont:_svrContainerOpen error for %s,status=%d",
          container, contInx);
	return (contInx);
    }
    /* seek to the end of cont */
    if (srbObjDesc[contInx].objSize > 0) {
	srb_long_t lstatus;
        lstatus = _svrObjSeek (contInx, srbObjDesc[contInx].objSize, SEEK_SET);
    
        if (lstatus < 0) {
	    status = lstatus;
            elog (NOTICE, 
	      "localBulkMoveDataToCont: _svrObjSeek error, status = %d ",
              status);
            svrContainerClose (contInx);
            return status;
	}
    }
    contOffset = srbObjDesc[contInx].objSize;

    buf = malloc (BL_BUFSIZE);
    if (buf == NULL) {
        fprintf (stderr, "localBulkMoveDataToCont: Unable to malloc buf\n");
        return (CLI_ERR_MALLOC);
    }

    bufOffset = 0;

    while (continuation_index >= 0) {
        /* hand craft the outresult */
        setupResultForMove (inresult, &outresult, container);

        outCollection = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
        outObjID = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
        outVerNum = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);
        outReplNum = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
        outSize = (char *) getFromResultStruct(&outresult,
          dcs_tname[SIZE], dcs_aname[SIZE]);
        outOffset = (char *) getFromResultStruct(&outresult,
          dcs_tname[OFFSET], dcs_aname[OFFSET]);

        inCollection = (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

        inObjID = (char *)getFromResultStruct(
        (mdasC_sql_result_struct *) inresult, dcs_tname[DATA_NAME],
            dcs_aname[DATA_NAME]);
        inSize = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[SIZE], dcs_aname[SIZE]);

        inOffset = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[OFFSET], dcs_aname[OFFSET]);

        inChksum = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);

        inReplNum = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);

        inVerNum = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);

        inResourceType = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) inresult, dcs_tname[PHY_RSRC_TYP_NAME],
            dcs_aname[PHY_RSRC_TYP_NAME]);
        inPath = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inresult, dcs_tname[PATH_NAME],
          dcs_aname[PATH_NAME]);
        inResourceLoc = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) inresult, dcs_tname[RSRC_ADDR_NETPREFIX],
          dcs_aname[RSRC_ADDR_NETPREFIX]);

        for (i = 0; i < inresult->row_count; i++) {
            srb_long_t bytesCopied;

            srcTypeInx = objTypeLookUp (inResourceType);
            size = strtoll (inSize, 0, 0);

	    bytesCopied = dataCopyToCont (catType, srcTypeInx, inPath, 
	      inResourceLoc, size, contInx, contOffset, buf, &bufOffset);

            if (bytesCopied != size) {
                if (bytesCopied < 0) {
                    status = bytesCopied;
                } else {
                    status = OBJ_ERR_COPY_LEN;
                }
                freeSqlResult (inresult);
                clearSqlResult (&outresult);
		free (buf);
                return (status);
            }

            outresult.row_count++;
	    sprintf (outOffset, "%lld", contOffset);
            strcpy (outSize, inSize);
            strcpy (outCollection, inCollection);
            strcpy (outObjID, inObjID);
            strcpy (outReplNum, inReplNum);
            strcpy (outVerNum, inVerNum);

	    contOffset += size;

            outCollection += MAX_DATA_SIZE;
            outObjID += MAX_DATA_SIZE;
            outVerNum += MAX_DATA_SIZE;
            outReplNum += MAX_DATA_SIZE;
            outSize += MAX_DATA_SIZE;
            outOffset += MAX_DATA_SIZE;
            inCollection += MAX_DATA_SIZE;
            inObjID += MAX_DATA_SIZE;
            inSize += MAX_DATA_SIZE;
            inOffset += MAX_DATA_SIZE;
            inChksum += MAX_DATA_SIZE;
            inReplNum += MAX_DATA_SIZE;
            inVerNum += MAX_DATA_SIZE;
            inResourceType += MAX_DATA_SIZE;
            inPath += MAX_DATA_SIZE;
            inResourceLoc += MAX_DATA_SIZE;
	}
	/* dump the rest of the buffer */
        if (bufOffset > 0) {
            status = _svrObjWrite (contInx, buf, bufOffset);
            if (status != bufOffset) {
                if (status < -1) {
                    elog (NOTICE,
                      "dataCopyToCont:problem writing to cont,stat=nbytes",
                      status);
                } else {
                    elog (NOTICE,
                    "dataCopyToCont:problem writing to cont,%d bytes left",
                    bufOffset - status);
                    status = OBJ_ERR_COPY_LEN;
                }
                return (status);
            }
            bufOffset = 0;
        }

	if (contOffset >= srbObjDesc[contInx].contInfo->containerMaxSize) {
	    status = svrContainerClose (contInx);
	    contInx = -1;
	} else {
            status = _svrRegNewObjSize (catType, srbObjDesc[contInx].objID,
             srbObjDesc[contInx].collection, srbObjDesc[contInx].resourceName,
             srbObjDesc[contInx].dataPath, 
	     contOffset);
            if (status < 0) {
                elog (NOTICE,
                 "localBulkMoveDataToCont:_svrRegNewObjSize error.stat=%d\n ",
                  status);
                freeSqlResult (inresult);
                clearSqlResult (&outresult);
                free (buf);
                return (status);
            }
	    setWriteFlag (contInx, 0);
	    status = procDescDirtyFlag (contInx);
	}

        if (status < 0) {
            elog (NOTICE,
             "localBulkMoveDataToCont:svrContainerClose error.stat=%d\n ",
              status);
            freeSqlResult (inresult);
            clearSqlResult (&outresult);
            free (buf);
            return (status);
        }

        /* do the registration here */
        status = _svrModifyForBulkMove (catType, BULK_PHY_MOVE_INTO_CONTAINER,
          container, &outresult);
        if (status < 0) {
            elog (NOTICE,
             "localBulkMoveDataToCont:_svrModifyForBulkMove error.stat=%d\n ",
              status);
            freeSqlResult (inresult);
            clearSqlResult (&outresult);
	    free (buf);
            return (status);
        }

        if (contInx < 0) {
	    /* get a new container */
            contInx = _svrContainerOpen (catType, container, O_RDWR);
            if (contInx < 0) {
                elog(NOTICE,
                 "localBulkMoveDataToCont:_svrContainerOpen error for %s,status=%d",
                  container, contInx);
                return (contInx);
            }
            /* seek to the end of cont */
	    if (srbObjDesc[contInx].objSize > 0) {
		srb_long_t lstatus;
                lstatus = _svrObjSeek (contInx, srbObjDesc[contInx].objSize, 
		  SEEK_SET);
                if (lstatus < 0) {
		    status =lstatus;
                    elog(NOTICE, 
		      "localBulkMoveDataToCont:_svrObjSeek error,status = %d ",
                      status);
                    svrContainerClose (contInx);
                    return status;
                }
	    }
            contOffset = srbObjDesc[contInx].objSize;
	}

        continuation_index = inresult->continuation_index;
        clearSqlResult (&outresult);
        status = delMovedSrcData (inresult);
        if (status < 0) {
            elog(NOTICE,
             "localBulkMoveDataToCont:delMovedSrcData error ,status=%d",
             status);
            freeSqlResult (inresult);
	    free (buf);
            return (status);
	}
        freeSqlResult (inresult);
        outresult.row_count = 0;

	/* XXXX need to take care of overflow cont here */

        if (continuation_index >= 0) {
            if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
                status = get_more_rows (catType, continuation_index,
                  &inresult, BULK_MOVE_NUM_ROWS);
#endif
            } else {
                status = remoteGetMoreRows (catType, continuation_index,
                  &inresult, BULK_MOVE_NUM_ROWS, hostTabPtr);
            }

            if (status < 0 || inresult == NULL || inresult->row_count <= 0) {
                elog (NOTICE,
                 "localBulkMoveDataToCont: get_more_rows error. ststus = %d\n ",
                  status);
                if (status == MCAT_INQUIRE_ERROR)
                    status = 0;
	        free (buf);
    		svrContainerClose (contInx);
                return (status);
            }
	    addZoneToResult (inresult, collection);
        }
    }
    status = svrContainerClose (contInx);
    free (buf);
    return (0);
}

srb_long_t 
dataCopyToCont (int catType, int srcTypeInx, char *inPath,
char *inResourceLoc, srb_long_t size, int contInx, srb_long_t contOffset, 
char *buf, int *bufOffset)
{
    srb_long_t retVal = 0;
    int srcExfInx;
    int toread, space;
    srb_long_t bytesCopied;
    char *bufptr;
    int readLen, nbytes;
    int status;

    srcExfInx = lowLevelOpen (srcTypeInx, inPath, inResourceLoc, O_RDONLY);

    if (srcExfInx < 0) {   /* error */
        elog (NOTICE, "dataCopyToCont: Unable of open the source copy %s",
	  inPath);
        lowLevelClose (srcTypeInx, srcExfInx);
        return srcExfInx;
    }

    toread = size;
    bufptr = buf + *bufOffset;

    while (toread > 0) {
        int space;

        space = BL_BUFSIZE - *bufOffset;

        if (toread > space) {
            if (*bufOffset > 0) {
                status = _svrObjWrite (contInx, buf, *bufOffset);
                if (status != *bufOffset) {
        	    lowLevelClose (srcTypeInx, srcExfInx);
		    if (status < -1) {
                        elog (NOTICE,
                          "dataCopyToCont:problem writing to cont,stat=nbytes",
                          status);
		        retVal = status;
		    } else {
                        elog (NOTICE,
                        "dataCopyToCont:problem writing to cont,%d bytes left",
              		*bufOffset - status);
                        retVal = OBJ_ERR_COPY_LEN;
		    }
                    return (retVal);
                }
                bufptr = buf;
		*bufOffset = 0;
            }
            readLen = BL_BUFSIZE;
        } else {
            readLen = toread;
        }

	 nbytes = _objRead (srcExfInx, srcTypeInx, bufptr, readLen);
        if (nbytes <= 0) {
            break;
        }
        toread -= nbytes;
        *bufOffset += nbytes;
	bufptr += nbytes;
    }
    lowLevelClose (srcTypeInx, srcExfInx);
    if (toread > 0) {
	if (nbytes < -1) {
            elog (NOTICE,
              "dataCopyToCont: problem reading file %s, status = nbytes",
              inPath, nbytes);
            retVal = nbytes;
            return retVal;
	} else {
            elog (NOTICE,
              "dataCopyToCont: problem reading file %s, %d bytes left\n",
              inPath, toread);
	    retVal = OBJ_ERR_COPY_LEN;
	    return retVal;
	}
    }
    return (size);
}

int
procDescDirtyFlag (int descInx)
{
    int status;
    char *myPath;
    struct mdasInfoOut *tmpInfoOut;
    int tmpInx;

    if (srbObjDesc[descInx].contInx >= 0) {  /* Container or inContainer Obj */
        tmpInx = srbObjDesc[descInx].contInx;
    } else {
        tmpInx = descInx;
    }

    if (srbObjDesc[tmpInx].intReplNum >= 0) {
        myPath = srbObjDesc[tmpInx].compDataPath;
    } else {
        myPath = srbObjDesc[tmpInx].dataPath;
    }
    /* Marking the container dirty. */
    if (srbObjDesc[tmpInx].markedDirty == 0) {
        status = _svrModifyDataset (MDAS_CATALOG,
         srbObjDesc[tmpInx].objID,
         srbObjDesc[tmpInx].collection,
         srbObjDesc[tmpInx].resourceName,
         myPath,
         "1", NULL, D_CHANGE_DIRTY);
        if (status < 0) {
            elog (NOTICE,
             "_svrObjClose: Unable to reg container to dirty, status = %d",
             status);
            return (status);
        }
        srbObjDesc[tmpInx].markedDirty = 1;
    }

    /* mark all other dirty copies not dirty */
    tmpInfoOut = srbObjDesc[tmpInx].dirtyInfo;
    while (tmpInfoOut != NULL) {
        int myInx;
        myInx = descInx;
        while (myInx != -1) {
            if (srbObjDesc[myInx].replNum < 0 ||
             srbObjDesc[myInx].replNum == tmpInfoOut->replNum)
                break;
            myInx = srbObjDesc[myInx].nextDescInx;
        }

        if (myInx == -1) {      /* no match */
            status = _svrModifyDataset (MDAS_CATALOG,
             srbObjDesc[tmpInx].objID,
             srbObjDesc[tmpInx].collection,
             tmpInfoOut->resourceName,
             tmpInfoOut->dataPath,
             "0", NULL, D_CHANGE_DIRTY);
        }

        if (status < 0)
            elog (NOTICE,
             "_svrObjClose: Unable to reg container clean, status = %d",
              status);

        tmpInfoOut = tmpInfoOut->next;
    }

    /* free dirtyInfo so that we don't do it
     * again next time.
     */
    if (srbObjDesc[tmpInx].dirtyInfo != NULL) {
        freeAllInfoOut (srbObjDesc[tmpInx].dirtyInfo);
        srbObjDesc[tmpInx].dirtyInfo = NULL;
    }
    srbObjDesc[tmpInx].dirtyInfo = NULL;

    return (status);
}

int
setWriteFlag (int descInx, int writeFlag)
{
    int objTypeInx, lowLevelDescInx;
    int contInx;
    int fileType;
    srb_long_t incontSize = -1;
    int status = 0;

    fileType = parseObjDesc (descInx, &objTypeInx, &lowLevelDescInx, &contInx);

    if (fileType < 0) {
        return (fileType);
    }

    /* should,nt have to check if _getWriteSize () has _dbLobjWriteSize */
    if (srbObjTypeEntry[objTypeInx].objType == FILE_TYPE) {
	status = _setFileWriteFlag (lowLevelDescInx, writeFlag);
    }
    return (status);
}
 
int 
svrRegPhyMove (int catType, struct varlena *vObjID, 
struct varlena *vCollectionName,
struct varlena *vCurResourceName, struct varlena *vCurDataPath, 
struct varlena *vNewResourceName, struct varlena *vNewFullPathName)
{
    char *objID, *collectionName, *curResourceName, *curDataPath, 
    *newResourceName, *newFullPathName;
    int retVal;
  
    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
      return (ILLEGAL_OPR_TICKET_USER);
    }
  
    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    curResourceName = varToStr (vCurResourceName);
    curDataPath = varToStr (vCurDataPath);
    newResourceName = varToStr (vNewResourceName);
    newFullPathName = varToStr (vNewFullPathName);

    retVal = _svrRegPhyMove (catType, objID, collectionName, curResourceName,
     curDataPath, newResourceName, newFullPathName);

    free (objID);
    free (collectionName);
    free (curResourceName);
    free (curDataPath);
    free (newResourceName);
    free (newFullPathName);
  
    return retVal;
}


int
_svrRegPhyMove (int catType, char *objID,  char *collectionName, 
char *curResourceName, char *curDataPath, char *newResourceName, 
char *newFullPathName)
{
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;
  
  
    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);
    if (remoteFlag < 0) {
        retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = move_dataset (catType, objID, collectionName,
          curResourceName, curDataPath, newResourceName, newFullPathName,
          ClientUser->userName, ClientUser->domainName);
        if (retVal == 0) {
          commit_db2_interaction (MDAS_CAT_COMMIT);
        } else {
            elog (NOTICE,
	      "_svrRegPhyMove: move_dataset error for %s/%s, stat=%d",
	      collectionName, objID, retVal);
        }
#endif
    } else {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }
    
        /* Do a client call to the remote host */
    
        retVal = srbRegPhyMove (hostTabPtr->conn, catType, objID, 
	  collectionName, curResourceName, curDataPath, newResourceName, 
	  newFullPathName);
    }  
    return retVal;
}

int
_svrObjMoveToCont (struct mdasInfoOut *infoOut, char *objID, 
char *collectionName, char *containerName)
{
    mdasC_sql_result_struct *myresult;
    int status;

    /* hand craft the input myresult */

    myresult = malloc (sizeof (mdasC_sql_result_struct));
    myresult->result_count = 12;
    myresult->row_count = 1;

    myresult->sqlresult[0].att_name = strdup (dcs_aname[DATA_NAME]);
    myresult->sqlresult[0].tab_name = strdup (dcs_tname[DATA_NAME]);
    myresult->sqlresult[0].values = strdup (objID);

    myresult->sqlresult[1].att_name = strdup (dcs_aname[DATA_GRP_NAME]);
    myresult->sqlresult[1].tab_name = strdup (dcs_tname[DATA_GRP_NAME]);
    myresult->sqlresult[1].values = strdup (collectionName);

    myresult->sqlresult[2].att_name = strdup (dcs_aname[PHY_RSRC_TYP_NAME]);
    myresult->sqlresult[2].tab_name = strdup (dcs_tname[PHY_RSRC_TYP_NAME]);
    myresult->sqlresult[2].values = 
      strdup (srbObjTypeEntry[infoOut->objTypeInx].resourceType);

    myresult->sqlresult[3].att_name = strdup (dcs_aname[DATA_VER_NUM]);
    myresult->sqlresult[3].tab_name = strdup (dcs_tname[DATA_VER_NUM]);
    myresult->sqlresult[3].values = malloc (MAX_DATA_SIZE);
    memset (myresult->sqlresult[3].values, 0, 1);

    myresult->sqlresult[4].att_name = strdup (dcs_aname[DATA_REPL_ENUM]);
    myresult->sqlresult[4].tab_name = strdup (dcs_tname[DATA_REPL_ENUM]);
    myresult->sqlresult[4].values = malloc (MAX_DATA_SIZE);
    sprintf (myresult->sqlresult[4].values, "%d", infoOut->replNum);

    myresult->sqlresult[5].att_name = strdup (dcs_aname[PATH_NAME]);
    myresult->sqlresult[5].tab_name = strdup (dcs_tname[PATH_NAME]);
    myresult->sqlresult[5].values = strdup (infoOut->dataPath);

    myresult->sqlresult[6].att_name = strdup (dcs_aname[RSRC_ADDR_NETPREFIX]);
    myresult->sqlresult[6].tab_name = strdup (dcs_tname[RSRC_ADDR_NETPREFIX]);
    myresult->sqlresult[6].values = strdup (infoOut->resourceLoc);

    myresult->sqlresult[7].att_name = strdup (dcs_aname[PHY_RSRC_NAME]);
    myresult->sqlresult[7].tab_name = strdup (dcs_tname[PHY_RSRC_NAME]);
    myresult->sqlresult[7].values = strdup (infoOut->resourceName);

    myresult->sqlresult[8].att_name = strdup (dcs_aname[RSRC_CLASS]);
    myresult->sqlresult[8].tab_name = strdup (dcs_tname[RSRC_CLASS]);
    myresult->sqlresult[8].values = strdup (infoOut->phyResClass);

    myresult->sqlresult[9].att_name = strdup (dcs_aname[SIZE]);
    myresult->sqlresult[9].tab_name = strdup (dcs_tname[SIZE]);
    myresult->sqlresult[9].values = malloc (MAX_DATA_SIZE);
    sprintf (myresult->sqlresult[9].values, "%lld", infoOut->size);

    myresult->sqlresult[10].att_name = strdup (dcs_aname[DATA_CHECKSUM]);
    myresult->sqlresult[10].tab_name = strdup (dcs_tname[DATA_CHECKSUM]);
    myresult->sqlresult[10].values = malloc (MAX_DATA_SIZE);
    sprintf (myresult->sqlresult[10].values, "%d", infoOut->chksum);

    myresult->sqlresult[11].att_name = strdup (dcs_aname[CONTAINER_NAME]);
    myresult->sqlresult[11].tab_name = strdup (dcs_tname[CONTAINER_NAME]);
    myresult->sqlresult[11].values = strdup (infoOut->containerName);

    status = localBulkMoveDataToCont (MDAS_CATALOG, collectionName, 
      containerName, myresult);

    return (status);
}

int
svrQueryContainerByColl (char *collection, mdasC_sql_result_struct **myresult)
{
    int status, i;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];

    /* Setup the query */

    memset (qval, 0, sizeof (qval));
    memset (selval, 0, sizeof (selval));

    selval[CONTAINER_NAME] = 1;
    selval[REPL_TIMESTAMP] = 1;
    selval[DATA_CREATE_TIMESTAMP] = 1;

    sprintf (qval[DATA_GRP_NAME],
     " like '%s' ESCAPE '\\' || like '%s/%%' ESCAPE '\\'",
            collection, collection);

    status = queryDataInfo (collection, selval, (char (*) [MAX_TOKEN]) qval, 
      myresult, BULK_MOVE_NUM_ROWS);

    if (status < 0)
        return (status);
    else
        return (0);
}

int
bulkCopyFromCont (int catType, char *srcCollection, 
char *destCollection, struct mdasResInfo *destResInfo, 
char *container, char *contReplTime, char *contCreateTime, int flag)
{
    mdasC_sql_result_struct *inresult = NULL;
    int status;
    mdasC_sql_result_struct outresult;
    int continuation_index = 1;
    int contInx;
    char *inCollection, *inObjID, *inSize, *inChksum, *inPath, *inDataType,
      *inOffset;
    char *outCollection, *outObjID, *outPath;
    char *outDataType, *outSize, *outOffset, *outReplTime, *outCreateTime,
      *outChksum;
    srb_long_t offset, size;
    char *buf, *bufptr;
    srb_long_t tmpOffset, curOffset = 0, diffOffset;
    int local_fd;
    int cnt = 0;
    int destTypeInx;
    int toWrite = 0;
    
    destTypeInx = objTypeLookUp (destResInfo->phyResType);

    status = genIncontListinColl (catType, srcCollection, container, flag,
      &inresult);

    if (status < 0) {
	if (status == MCAT_INQUIRE_ERROR)
	    return (0);
	else
	    return (status);
    }

    contInx = _svrContainerOpen (catType, container, O_RDONLY);
    if (contInx < 0) {
        elog(NOTICE,
         "bulkCopyFromCont:_svrContainerOpen error for %s,status=%d",
          container, contInx);
	freeSqlResult (inresult);
        return (contInx);
    }

    buf = malloc (BL_BUFSIZE);
    bufptr = buf;

    while (continuation_index >= 0) {
        setupResultForCopy (inresult, &outresult, NULL);
        outCollection = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
        outObjID = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
        outPath = (char *) getFromResultStruct(&outresult,
          dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);
        outDataType = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
        outSize = (char *) getFromResultStruct(&outresult,
          dcs_tname[SIZE], dcs_aname[SIZE]);
        outOffset = (char *) getFromResultStruct(&outresult,
          dcs_tname[OFFSET], dcs_aname[OFFSET]);
        outReplTime = (char *) getFromResultStruct(&outresult,
          dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
        outCreateTime = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_CREATE_TIMESTAMP],
          dcs_aname[DATA_CREATE_TIMESTAMP]);
        outChksum = (char *) getFromResultStruct(&outresult,
          dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);

        inDataType = (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
        inCollection = (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
        inObjID = (char *)getFromResultStruct(
        (mdasC_sql_result_struct *) inresult, dcs_tname[DATA_NAME],
            dcs_aname[DATA_NAME]);
        inSize = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[SIZE], dcs_aname[SIZE]);
        inOffset = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[OFFSET], dcs_aname[OFFSET]);
        inChksum = (char *) getFromResultStruct(
        (mdasC_sql_result_struct *) inresult,
            dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);

        for (i = 0; i < inresult->row_count; i++) {
	    offset = strtoll (&inOffset[i*MAX_DATA_SIZE], 0, 0);
	    size = strtoll (&inSize[i*MAX_DATA_SIZE], 0, 0);

            local_fd = localCreateForMove (
                      &inCollection[i*MAX_DATA_SIZE],
                      &inObjID[i*MAX_DATA_SIZE],
		      NULL, NULL, destResInfo, destTypeInx, 
		      &outCollection[i*MAX_DATA_SIZE], 
		      &outObjID[i*MAX_DATA_SIZE],
                      NULL, NULL, 
		      &outPath[i*MAX_DATA_SIZE], 
		      flag, srcCollection, destCollection);

            if (local_fd < 0) {
                elog(NOTICE,
                 "unfoldBulkMoveData: localCreate of %s/%s failed, status = %d",
                  inCollection, inObjID, local_fd);
		status = local_fd;
		break;
            }
	    outresult.row_count++;

	    while (size > 0) {
                diffOffset = offset - curOffset;
                if (diffOffset < 0 || (diffOffset > 0 && diffOffset >= cnt)) {
                    tmpOffset = _svrObjSeek (contInx, offset, SEEK_SET);
		    if (tmpOffset< 0) {
		        status = tmpOffset;
                        elog (NOTICE, 
		          "bulkCopyFromCont:ObjSeek of %s %lld err.status=%d",
                          container, offset, status);
			lowLevelClose (destTypeInx, local_fd);
			break;
                    }
		    bufptr = buf;
                    curOffset = offset;
                    cnt = 0;
                } else {
                    curOffset = offset;
                    bufptr += diffOffset;
                    cnt -= diffOffset;
                }
		if (cnt <= 0) {
		    cnt = _svrObjRead (contInx, buf, BL_BUFSIZE);
		    if (cnt <= 0) {
			elog (NOTICE,
                          "bulkCopyFromCont:_svrObjRead of %s error. remaining size is %lld. status=%d",
                          container, size, cnt);
                        lowLevelClose (destTypeInx, local_fd);
			if (cnt < 0) {
			    status = cnt;
			} else {
			    status = OBJ_ERR_COPY_LEN;
			}
			break;
		    }
		    bufptr = buf;
		}
		if (cnt >= size) {
		    toWrite = size;
		} else {
		    toWrite = cnt;
		}
		status = _objWrite (local_fd, destTypeInx, bufptr, toWrite);
		if (status < toWrite) {
                    elog (NOTICE,
                     "bulkCopyFromCont: %s To write %lld, Wrote %d bytes.",
                     container, toWrite, status);
                    lowLevelClose (destTypeInx, local_fd);
                    status = OBJ_ERR_COPY_LEN;
		    break;
		}
                cnt -= toWrite;
                curOffset += toWrite;
                offset += toWrite;
                bufptr += toWrite;
		size -= toWrite;
	    }	/* while size loop */
            _objClose (destTypeInx, local_fd);
            strcpy (&outDataType[i*MAX_DATA_SIZE], 
	      &inDataType[i*MAX_DATA_SIZE]);
            strcpy (&outSize[i*MAX_DATA_SIZE], &inSize[i*MAX_DATA_SIZE]);
            strcpy (&outOffset[i*MAX_DATA_SIZE], &inOffset[i*MAX_DATA_SIZE]);
            strcpy (&outReplTime[i*MAX_DATA_SIZE], contReplTime);
            strcpy (&outCreateTime[i*MAX_DATA_SIZE], contCreateTime);
            strcpy (&outChksum[i*MAX_DATA_SIZE], &inChksum[i*MAX_DATA_SIZE]);
	    if (status < 0)
		break;
	}	/* for loop */
	if (status < 0) {
	     cleanMovedData (destResInfo, &outresult);
            svrContainerClose (contInx);
            free (buf);
	    return (status);
	}
        status = _svrBulkRegister (MDAS_CATALOG,
          destResInfo->phyResName, &outresult, 1);
        continuation_index = inresult->continuation_index;
        if (status < 0) {
            elog (NOTICE,
             "bulkCopyFromCont: getResultForBunload error. ststus = %d\n ",
              status);
            break;
        }

        freeSqlResult (inresult);
        clearSqlResult (&outresult);
        outresult.row_count = 0;

        if (continuation_index >= 0) {
            status = _svrGetMoreRows (catType, continuation_index, &inresult,
              BULK_MOVE_NUM_ROWS);

            if (status < 0) {
		if (status == MCAT_INQUIRE_ERROR) {
		    status = 0;
		} else {
                    elog (NOTICE,
                     "bulkCopyFromCont: _svrGetMoreRows error. status = %d",
                      status);
		}
                break;
            }
        } else {
            break;
        }
    }
    svrContainerClose (contInx);
    free (buf);
    return (status);
}

int
genIncontListinColl (int catType, char *collection, 
char *container, int flag, mdasC_sql_result_struct **myresult)
{
    int i, status;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];

    memset (qval, 0, sizeof (qval));
    memset (selval, 0, sizeof (selval));

    selval[DATA_NAME] = 1;
    selval[DATA_GRP_NAME] = 1;
    selval[DATA_TYP_NAME] = 1;
    selval[SIZE] = 1;
    selval[OFFSET] = 1;
    selval[DATA_CHECKSUM] = 1;
    sprintf(qval[DATA_ID] ," <> %s%s",
     MDASSCHEMENAME,"MDAS_AD_REPL.container_id");
    sprintf (qval[CONTAINER_NAME], " = '%s'", container);

    sprintf (qval[DATA_GRP_NAME],
     " like '%s' ESCAPE '\\' || like '%s/%%' ESCAPE '\\'",
            collection, collection);

    status = queryDataInfo (collection, selval, (char (*) [MAX_TOKEN]) qval,
      myresult, BULK_MOVE_NUM_ROWS);

    return (status);

}

int setWhereToStartBulkOpr (int catType, int flag,
char *destResource, char *collection, struct mdasResInfo **outDestResInfo,
struct hostElement  **outHostTabPtr)
{
    struct resCache *myCacheRes = NULL;
    struct mdasResInfo *destResInfo = NULL;
    int remoteFlag;
    char *portNum, tmpResLoc[MAX_TOKEN];
    struct hostElement  *hostTabPtr, *tmpHostTabPtr;
    int status = 0;
 
    if (getResInfo (collection, catType, destResource,
     ClientUser->domainName, &myCacheRes) < 0) {
        elog (NOTICE,
          "_svrBulkMove: Input resource %s is not a registered resource",
          destResource);
	*outHostTabPtr = NULL;
	*outDestResInfo = NULL;
        return (OBJ_ERR_RES_NOT_REG);
    }

    sortResForIngest (myCacheRes, -1);

    destResInfo = myCacheRes->resInfoHead;
    while (destResInfo != NULL) {
#ifdef FED_MCAT
        remoteFlag = chkHostAndZone (destResInfo->phyResLoc, &hostTabPtr);
#else
        /* Parse the hostAddr */
        strcpy (tmpResLoc, destResInfo->phyResLoc);
        portNum = parseAddr (tmpResLoc);
        remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif
        if (flag & BL_LOCAL_FLAG) {
             if (remoteFlag == LOCAL_HOST)
                 break;
        } else if (remoteFlag >= 0) {
            break;
        }
        destResInfo = destResInfo->next;
    }

    if (destResInfo == NULL || remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s",
          myCacheRes->resInfoHead->phyResLoc);
	*outDestResInfo = NULL;
	*outHostTabPtr = NULL;
        return INP_ERR_HOST_ADDR;               /* failure */
    }  else if (remoteFlag == REMOTE_HOST) {
	*outDestResInfo = destResInfo;
	*outHostTabPtr = hostTabPtr;
	return (REMOTE_HOST);
    } else {
	*outDestResInfo = destResInfo;
	*outHostTabPtr = hostTabPtr;
	return (LOCAL_HOST);
    }
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

