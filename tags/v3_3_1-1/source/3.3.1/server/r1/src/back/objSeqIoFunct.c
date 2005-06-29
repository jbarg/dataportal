/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/* objSeqIoFunct - Routines to handle object level sequential I/O.
 */
#include "objSeqIoFunct.h"

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
    int retVal;
    char *condInput;
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
	setObjDescCond (retVal, condInput);
#ifdef foo	/* done in setObjDescCond */
	int myInx = -1;
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
#endif
    }

    free (condInput);
    return (retVal);
}

int
_objCreate (int catType, char *objID, char *condInput, char *dataType, 
char *logResName, char *collectionName, char *pathName, srb_long_t dataSize)
{
    int retVal, status;
    int descInx = -1, exfInx = -1;
    struct mdasResInfo *tmpResInfo;
    int headDescInx = -1;
    int tmpDescInx = -1;
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
	int regMode;

	if (copyCnt >= condResult.intResult) 	/* Done */
	    break;	

	exfInx = _objCreateWithResOrCompInfo (objID, collectionName,
	 pathName, dataSize, tmpResInfo, 
	 fullPathName, &objTypeInx, &lockFd, &compResInfo);

#ifdef foo	/* done using _objCreateWithResOrCompInfo */
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
#endif	/* foo */

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

       compPathName[0] = '\0';         /* make _svrGenRegisterDataset
                                         * to generate a path */
        if (headDescInx == -1) {
            regMode = NEW_FIRST_COPY;
        } else {
            regMode = NEW_REPLICA;
        }

	replNum = _svrGenRegisterDataset (catType, objID, collectionName,
	 userName, ClientUser->domainName, regMode, tmpResInfo, compResInfo,
	 fullPathName, 0, dataType, objTypeInx, 
	 NULL, NULL, 0, compPathName, &intReplNum, &lockFd);

#ifdef foo	/* done in _svrGenRegisterDataset */
        /* myPathName is the path that was actually used */
        if (compResInfo != NULL) {
	    /* Create a fake pathName for the compound obj */
	    genCompPath (compPathName, objID);
            myPathName = compPathName;
        } else {
            myPathName = fullPathName;
	}

    	/* Do the registration */

	/* If it is a compound obj, use the phyResName and phyResLoc of the 
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
#endif	/* foo */

        if (replNum < 0) {
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

        /* Fill out a srbObjDesc */
 
	descInx = getAndQueNewObjDesc (&headDescInx);
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

#ifdef foo	/* done in getAndQueNewObjDesc */
        descInx = newObjDesc ();
        /* Link it in a singlely linked chain */

        if (headDescInx == -1) {
	    headDescInx = descInx;
        } else {
	    tmpDescInx = headDescInx;
	    while (srbObjDesc[tmpDescInx].nextDescInx != -1) 
	    	tmpDescInx = srbObjDesc[tmpDescInx].nextDescInx;
	    srbObjDesc[tmpDescInx].nextDescInx = descInx;
	}
#endif	/* foo */

	/* Use the phyResName of the compound obj instead of the int
	 * compound obj. Use in _svrObjClose for mcat modification.
	 */
	fillObjDesc (descInx, exfInx, objTypeInx, objID, collectionName, 
	  fullPathName, tmpResInfo->phyResName, replNum, dataSize);
/*
    	srbObjDesc[descInx].objSize = dataSize;
*/
    	srbObjDesc[descInx].nextDescInx = -1;
	if (compResInfo != NULL) { 
	    srbObjDesc[descInx].compDataPath = strdup (compPathName);
            srbObjDesc[descInx].intReplNum = intReplNum;
            srbObjDesc[descInx].intSegNum = NO_INT_SEGMENTATION;
	}
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

	    lowLevelCloseAllObjDesc (&headDescInx);
#ifdef foo	/* done in lowLevelCloseAllObjDesc */
	    descInx = headDescInx;
    	    while (descInx != -1) {
        	objTypeInx = srbObjDesc[descInx].objTypeInx;
		lowLevelClose (objTypeInx, srbObjDesc[descInx].lowLevelDescInx);
        	tmpDescInx = descInx;
        	descInx = srbObjDesc[descInx].nextDescInx;
        	delObjDesc (tmpDescInx);
    	    }
	    headDescInx = -1;
#endif
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
    int retVal;
    int exfInx = -1;
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
    struct mdasInfoOut *infoOutHead;
    int retVal;
    struct condResult condResult;

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

	if (svrCollStat (MDAS_CATALOG, myPath, &myresult) >= 0) {
	    if (checkCollPerm (myresult, ProxyUser, ClientUser,
	     O_RDONLY) >= 0) {
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
	int myInx = -1;

        setObjDescCond (retVal, objID);
#ifdef foo      /* done in setObjDescCond */
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
#endif
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
    int exfInx = -1, descInx = -1;
    char comment[MAXEXFPATH];
    int failCnt = 0;
    int headDescInx = -1;
    int tmpDescInx = -1, tmpDescInx1 = -1;
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

	/* XXXXX a temp debug msg */
	if (strcmp (collectionName, infoOut->collection) != 0) {
	    elog (NOTICE, 
	     "_objOpen: obj path mismatch: input %s/%s, infOut %s/%s",
	     collectionName, inpObjID, infoOut->collection, infoOut->objID);
	}

        /* Call the open at the next level, Loop through the infoOutHead 
	 * chain */

        exfInx = genLowLevelOpen (infoOut, collectionName, openFlag,
         &lockFd, 1, &cacheInfo);
#ifdef foo	/* done in genLowLevelOpen */
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
#endif	/* foo */
        if (exfInx < 0) {       /* failure */
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
 
	descInx = getAndQueNewObjDesc (&headDescInx);

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

#ifdef foo      /* done in getAndQueNewObjDesc */
    	descInx = newObjDesc ();
        /* Link it in a singlely linked chain */

        if (headDescInx == -1) {
            headDescInx = descInx;
        } else {
            tmpDescInx = headDescInx;
            while (srbObjDesc[tmpDescInx].nextDescInx != -1)
                tmpDescInx = srbObjDesc[tmpDescInx].nextDescInx;
            srbObjDesc[tmpDescInx].nextDescInx = descInx;
        }
#endif	/* foo */

        if (cacheInfo != NULL) {
	    /* Use the phyResName of the compound obj instead of the int
             * compound obj. Use in _svrObjClose for mcat modification.
             */
	    fillObjDesc (descInx, exfInx, cacheInfo->objTypeInx, 
	      cacheInfo->objID, collectionName, 
	      cacheInfo->dataPath, infoOut->resourceName, infoOut->replNum,
	      cacheInfo->size);
/*
	    srbObjDesc[descInx].objSize = cacheInfo->size;
*/
	    srbObjDesc[descInx].markedDirty = infoOut->isDirty;
	    srbObjDesc[descInx].chksum = infoOut->chksum;
	    srbObjDesc[descInx].baseOffset = cacheInfo->offset;
	    srbObjDesc[descInx].compDataPath = strdup (infoOut->dataPath);
	    srbObjDesc[descInx].segNum = cacheInfo->segNum;
	    srbObjDesc[descInx].intReplNum = cacheInfo->intReplNum;
            srbObjDesc[descInx].intSegNum = NO_INT_SEGMENTATION;
	    freeAllInfoOut (cacheInfo);
	    cacheInfo = NULL;
	} else {
    	    fillObjDescWithInfo (descInx, exfInx, infoOut->objTypeInx, 
	     collectionName, infoOut);
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
	    lowLevelCloseAllObjDesc (&headDescInx);
#ifdef foo
            tmpDescInx = headDescInx;
            while (tmpDescInx != -1) {
                tinx = srbObjDesc[tmpDescInx].objTypeInx;
                lowLevelClose (tinx, srbObjDesc[tmpDescInx].lowLevelDescInx);
		tmpDescInx1 = tmpDescInx;
                tmpDescInx = srbObjDesc[tmpDescInx].nextDescInx;
                delObjDesc (tmpDescInx1);
            }
#endif
    }

    if (descInx >= 0)
	return headDescInx;	/* success - return the head */
    else
        return descInx;
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
    int oldInx = -1;


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
	int myDesc;

        if (srbObjDesc[descInx].contInx >= 0) {
            myDesc = srbObjDesc[descInx].contInx;
        } else {
            myDesc = descInx;
        }

        status = _svrObjClose (descInx);
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
    char *myPath;
    int status;
    struct mdasInfoOut *tmpInfoOut;
    int tmpInx = -1;
    int objTypeInx, lowLevelDescInx = -1;
    int contInx = -1;
    int fileType;
    srb_long_t incontSize = -1;
    int savedStatus = 0;

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

    if (fileType == NORMAL_FILE_TYPE &&
     strlen (srbObjDesc[descInx].fileGroup) > 0 && 
     srbObjTypeEntry[objTypeInx].objType == FILE_TYPE) {
	savedStatus = _svrFileChownByInx (lowLevelDescInx, NULL, 
	srbObjDesc[descInx].fileGroup, 0);
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

        status = _svrRegNewObjSize(MDAS_CATALOG, srbObjDesc[descInx].objID,
          srbObjDesc[descInx].collection, "", "", incontSize);

#ifdef foo      /* done in _svrRegNewObjsize */

#if defined(PORTNAME_osx)
        sprintf (strSize, "%-qd", incontSize);
#else
        sprintf (strSize, "%-lld", incontSize);
#endif
        status = _svrModifyDataset (MDAS_CATALOG,
          srbObjDesc[descInx].objID,
          srbObjDesc[descInx].collection,
          "", "", strSize, NULL, D_CHANGE_SIZE);
#endif	/* foo */
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

#ifdef foo	/* done in _svrRegNewIntnernalCompsize */
#if defined(PORTNAME_osx)
    sprintf (strSize, "%-qd", size);
#else
    sprintf (strSize, "%-lld", size);
#endif
#endif	/* foo */
    if (srbObjDesc[tmpInx].intReplNum >= 0) {
        /* a internal compound object */
	status = _svrRegNewIntnernalCompsize (MDAS_CATALOG,
          srbObjDesc[tmpInx].objID, srbObjDesc[tmpInx].collection,
          srbObjDesc[tmpInx].replNum, NO_INT_SEGMENTATION,
          srbObjDesc[tmpInx].intReplNum, srbObjDesc[tmpInx].intSegNum, size);
#ifdef foo      /* done in _svrRegNewIntnernalCompsize */
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
#endif	/* foo */
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

    status = _svrRegNewObjSize (MDAS_CATALOG, srbObjDesc[tmpInx].objID,
     srbObjDesc[tmpInx].collection, srbObjDesc[tmpInx].resourceName,
     myPath, size);
#ifdef foo      /* done in _svrRegNewObjsize */
    status = _svrModifyDataset (MDAS_CATALOG,
     srbObjDesc[tmpInx].objID,
     srbObjDesc[tmpInx].collection,
     srbObjDesc[tmpInx].resourceName,
     myPath,
     strSize, NULL, D_CHANGE_SIZE);
#endif	/* foo */
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
	srb_long_t chksumSize, chksumOffset;

	if (incontSize > 0) {
	    chksumSize = incontSize;
	    chksumOffset = srbObjDesc[descInx].baseOffset;
	} else {
	    chksumSize = chksumOffset = 0;
	}   

        if (srbObjDesc[descInx].intReplNum >= 0) {
            chksumPath = srbObjDesc[descInx].compDataPath;
        } else {
            chksumPath = srbObjDesc[descInx].dataPath;
        }

	status = _objChksumAndReg (objTypeInx, 0, chksum, 
	 chksumOffset, chksumSize, 
	 srbObjDesc[descInx].objID, srbObjDesc[descInx].collection,
	 NULL, NULL,
	 srbObjDesc[descInx].chksumFlag, srbObjDesc[descInx].chksum,
	 srbObjDesc[descInx].resourceName, chksumPath, lowLevelDescInx);

#ifdef foo	/* done in _objChksumAndReg */
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
#endif	/* foo */
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

    if (status >= 0 && savedStatus < 0) {
	return savedStatus;
    } else {
        return status;
    }
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
      case MDRIVER_TYPE:
            status = MDriverClose (lowLevelDescInx);
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
      case MDRIVER_TYPE:
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
        case MDRIVER_TYPE:
            totalread = _MDriverRead (lowLevelDescInx, buf, len);
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
        case MDRIVER_TYPE:
            totalwritten = _MDriverWrite (lowLevelDescInx, buf, len);
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
        case MDRIVER_TYPE:
            retVal = MDriverSeek (lowLevelDescInx, offset, whence);
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


	if (status == UNIX_ENOENT || status == HPSS_SRB_ENOENT) {
            elog (NOTICE,
             "_svrObjUnlink: dataPath %s does not exist",
	     infoOut->dataPath);
        } else if (status < 0) {
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
      case MDRIVER_TYPE:
        retVal = _MDriverUnlink (systemType,
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
lowLevelOpen (int objTypeInx, char *dataPath, char *resourceLoc, int oflag)
{
    int exfInx = -1;

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
      case MDRIVER_TYPE:
	exfInx = _MDriverOpen (srbObjTypeEntry[objTypeInx].systemType,
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
        case MDRIVER_TYPE:
	    totalread = _MDriverRead (srbObjDesc[descInx].lowLevelDescInx,
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
    int contInx = -1;
    srb_long_t lstatus;

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
    			lstatus = _svrObjSeek (descInx, 0, SEEK_SET);

    			if (lstatus < 0) {
        		    elog(NOTICE, 
			     "svrObjWrite: _svrObjSeek error, status = %lld ", 
			      lstatus);
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
            case MDRIVER_TYPE:
	        totalwritten = 
		    _MDriverWrite (srbObjDesc[descInx].lowLevelDescInx,
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
            case MDRIVER_TYPE:
                retVal = MDriverSeek(srbObjDesc[descInx].lowLevelDescInx, 
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
	case MDRIVER_TYPE:
           retVal = MDriverSync (srbObjDesc[descInx].lowLevelDescInx);
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
            char *sizeList, *timeList;

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
            timeList = (char *) getFromResultStruct(
             (mdasC_sql_result_struct *) myresult,
              dcs_tname[COLL_MODIFY_TIMESTAMP], 
	      dcs_aname[COLL_MODIFY_TIMESTAMP]);

            retStat->st_atim = retStat->st_mtim = 
	     retStat->st_ctim = (srb_time_t) srbToLocalTime (timeList);
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
    int retVal1 = 0; 
    int retVal;
    struct mdasInfoOut *newInfoOut;
    char *tempStr;
    struct condResult condResult;
    char *condInput;


    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }


    /* Try to open the objID */

    infoInp.domainName = ClientUser->domainName;
    infoInp.objID = objID;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = collectionName;
    infoInp.accessMode = "write";

    /* Check with MDAS for the object to be replicated */
    if ((retVal = mdasGetInfo (&infoInp, &infoOut)) < 0) {
        return retVal;
    }

    /* Cannot replicate inContainer Obj yet */

    if (infoOut->containerName[0] != '\0') {  /* inContainer object */
	if (!myIsContainer (objID, collectionName, infoOut->containerName)) {
	    /* not a container, but a inContainer obj */
	    elog (NOTICE,
	     "_svrObjReplicate: Replication of inContainer obj %s not allowed",
	  objID);
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

    return (retVal);
}

int 
_objReplicate (int catType, char *objID, char *collectionName, 
char *newResourceName, char *newPathName, struct mdasInfoOut *origInfoOut, 
struct mdasInfoOut **newInfoOut, struct mdasResInfo *inpResInfo)
{
    struct mdasInfoOut *infoOut, *tmpInfoOut;
    int newObjInx = -1;
    int retVal = 0;
    struct mdasResInfo *newResInfo;
    char newFullPathName[MAX_TOKEN * 2];
    char compPathName[MAX_TOKEN * 2];
    char *myPathName;
    srb_long_t totalWritten;
    char *tmpStr, *tmpStr1, *myObjID;
    struct mdasResInfo *compResInfo;
    int intReplNum, replNum;
    int lockFd = -1;
    struct resCache *myResCache;
    char strSize[MAX_TOKEN];


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
	totalWritten = _svrDataCopyFromInfoToRescOrCompResc (catType,
	  myObjID, collectionName, newResInfo, infoOut, newPathName,
	  newFullPathName, &newObjInx, &lockFd, &compResInfo);
#ifdef foo	/* done in _svrDataCopyFromInfoToRescOrCompResc */
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
#endif	/* foo */

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

    compPathName[0] = '\0';         /* make _svrGenRegisterDataset
                                     * to generate a path */
    replNum = _svrGenRegisterDataset (catType, myObjID, collectionName,
     ClientUser->userName, ClientUser->domainName, NEW_REPLICA,
     newResInfo, compResInfo, newFullPathName, totalWritten, "", newObjInx, 
     infoOut->resourceName, infoOut->dataPath, infoOut->size,
     compPathName, &intReplNum, &lockFd);
 
#ifdef foo	/* use _svrGenRegisterDataset instead */
    /* myPathName is the path that was actually used */
    if (compResInfo != NULL) {
        /* Create a fake pathName for the compound obj */
        genCompPath (compPathName, objID);
        myPathName = compPathName;
    } else {
        myPathName = newFullPathName;
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
    /* RAJA ADDED Oct 1, 2004 TO REGISTER NEW SIZE WHEN IT IS DIFFERENT.
       GOOD FOR COPYING FROM SOURCES WITH 0 SIZE such as databases */

    if (totalWritten > 0 && infoOut != NULL && infoOut->size != totalWritten) {
        sprintf (strSize, "%lld", totalWritten);

        _svrModifyDataset (catType, myObjID, collectionName, 
	     newResInfo->phyResName, myPathName,
             strSize, NULL, D_CHANGE_SIZE);
    }

    /* RAJA ADDED Oct 1, 2004 TO REGISTER NEW SIZE WHEN IT IS DIFFERENT.
       GOOD FOR COPYING FROM SOURCES WITH 0 SIZE such as databases */

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
#endif	/* foo */

    retVal = replNum;
    if (retVal < 0) {           /* error */
        free (myObjID);
        return retVal;
    }

    if (newInfoOut != NULL) {
	/* If it is a comp obj, make the infoOut for the comp obj first */
        tmpInfoOut = malloc (sizeof (struct mdasInfoOut));
	memset (tmpInfoOut, 0, sizeof (struct mdasInfoOut));
        tmpInfoOut->objID = myObjID;
        tmpInfoOut->collection = strdup (collectionName);
	/* use the actual path */
	if (strlen (compPathName) > 0) {
            myPathName = compPathName;
	} else {
            myPathName = newFullPathName;
	}
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
struct varlena *vSrcResource,
struct varlena *vNewResourceName, struct varlena *vNewPathName,
struct varlena *vContainerName)
{
    char *objID;
    char *collectionName;
    char *newResourceName;
    char *newPathName;
    char *srcResource;
    char *container;
    int status;

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user");
        return (ILLEGAL_OPR_TICKET_USER);
    }

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);
    newPathName = varToStr (vNewPathName);
    newResourceName = varToStr (vNewResourceName);
    srcResource = varToStr (vSrcResource);
    container = varToStr (vContainerName);

    if (container != NULL && strlen (container) > 0) {
	if (newResourceName != NULL) {
	    newResourceName[0] = '\0';  
	}
    }

    status = _svrObjMove (catType, objID, collectionName, srcResource,
     newResourceName, newPathName, container);

    free (objID);
    free (collectionName);
    free (newPathName);
    free (newResourceName);
    free (srcResource);
    free (container);
    
    return (status);
}

int
_svrObjMove (int catType, char *objID, char *collectionName,
char *srcResource, char *newResourceName, char *newPathName, char *container)
{
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOut;
    int retVal = 0;
    int origObjInx = -1, newObjInx = -1;
    int origExfInx = -1, newExfInx = -1;
#ifdef foo
#ifdef _WIN32
    char *buf = NULL;
#else
    char buf[BUF_SIZE];
#endif
#endif
    int nbytes;
    char newFullPathName[MAX_TOKEN * 2];
    char compPathName[MAX_TOKEN * 2];
    char *myPathName;
    char tmpStr[MAX_TOKEN], tmpStr1[MAX_TOKEN];
    struct mdasResInfo *compResInfo;
    int lockFd = -1;
    struct resCache *myResCache;
    int intReplNum, replNum;
    struct mdasInfoOut *cacheInfoOut;
    int status;
    srb_long_t byteWritten, size;

    /* Try to open the objID */

    infoInp.domainName = ClientUser->domainName;
    infoInp.objID = objID;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = collectionName;
    infoInp.accessMode = "write";

    /* Check with MDAS for the object to be replicated */
    if ((retVal = mdasGetInfo (&infoInp, &infoOut)) < 0) {

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

    if (container != NULL && strlen (container) > 0) {
        status = _svrObjMoveToCont (infoOut, objID, collectionName,
         container);

        return (status);
    }

#ifdef foo	/* done in _svrDataCopyFromInfoToRescOrCompResc */
    /* Call open at the next level */

    origExfInx = genLowLevelOpen (infoOut, collectionName, O_RDONLY,
      &lockFd, 1, &cacheInfoOut);

    origObjInx = infoOut->objTypeInx;
    if (origObjInx < 0) {
        elog(NOTICE,
         "svrObjMove: resourceName:%s not supported",
          infoOut->resourceName);
        retVal = OBJ_ERR_RES_TYPE;

        freeInfoOut (infoOut);

#ifdef SRB_MDAS
        if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
        return retVal;
    }

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
	}
    } else {
	origExfInx = lowLevelOpen (origObjInx, infoOut->dataPath,
     	 infoOut->resourceLoc, O_RDONLY);
    }
   if (origExfInx < 0) {       /* failure */
#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (infoOut->objID, collectionName,
              F_SETLKW, F_UNLCK, lockFd);
#endif
    }

    if (origExfInx < 0) {       /* failure */
        elog (NOTICE, "svrObjMove: Unable of open the original copy");
        retVal = origExfInx;
        freeInfoOut (infoOut);

#ifdef SRB_MDAS
        if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
        return retVal;
    } else {
	if (cacheInfoOut != NULL) {
	    origObjInx = cacheInfoOut->objTypeInx;
	    freeInfoOut (cacheInfoOut);
	    cacheInfoOut = NULL;
	} else {
	    origObjInx = infoOut->objTypeInx;
	}

    }
#endif	/* foo */

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

#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (infoOut->objID, collectionName, 
              F_SETLKW, F_UNLCK, lockFd);
#endif
        return retVal;
    }

    sortResForIngest (myResCache, infoOut->size);  /* new code 5/6/04 */

    mySplitStr (objID, tmpStr, tmpStr1, '&');
    objID = tmpStr;

    byteWritten = _svrDataCopyFromInfoToRescOrCompResc (catType, 
      objID, collectionName, myResCache->resInfoHead, infoOut, newPathName,
      newFullPathName, &newObjInx, &lockFd, &compResInfo);
#ifdef foo 	/* done in _objCreateWithResOrCompInfo */
    newExfInx = _objCreateWithResOrCompInfo (objID, collectionName,
     newPathName, -1, myResCache->resInfoHead, newFullPathName, 
     &newObjInx, &lockFd, &compResInfo);

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
#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (infoOut->objID, collectionName, 
              F_SETLKW, F_UNLCK, lockFd);
#endif

#ifdef SRB_MDAS
        if (retVal == 0)
            commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
        return retVal;
    }

#ifdef _WIN32
    buf = (char *)calloc(BUF_SIZE,sizeof(char));
#endif

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

#ifdef _WIN32
    free(buf); buf = NULL;
#endif

    lowLevelClose (origObjInx, origExfInx);
    lowLevelClose (newObjInx, newExfInx);
#endif	/* foo */

    if (byteWritten < 0) {          /* no error */
        /* Unlink the newly created data object */
#ifdef foo
        _objUnlink (srbObjTypeEntry[newObjInx].objType,
          srbObjTypeEntry[newObjInx].systemType,
            myResCache->resInfoHead->phyResLoc, newFullPathName, NO_CHKDIR);
#endif
        freeInfoOut (infoOut);
#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (infoOut->objID, collectionName, 
              F_SETLKW, F_UNLCK, lockFd);
#endif
        retVal = byteWritten;
        return (retVal);
    }

    if (byteWritten != infoOut->size) {
        elog (NOTICE, "svrObjMove: Bytes moved %lld does not match size %lld",
         byteWritten);
	return (OBJ_ERR_COPY_LEN);
    }

    if (compResInfo != NULL) {
        /* Create a fake pathName for the compound obj */
        genCompPath (compPathName, objID);
        myPathName = compPathName;
    } else {
        myPathName = newFullPathName;
    }

    /* Now register with MDAS */

    replNum = _svrRegPhyMove (catType, objID, collectionName,
      infoOut->resourceName, infoOut->dataPath,
       myResCache->resInfoHead->phyResName, myPathName);


    if (replNum < 0) {		/* error */
	elog (NOTICE, 
	  "svrObjMove: Unable to register move_dataset with MDAS. Status = %d",
	   replNum);
        _objUnlink (srbObjTypeEntry[newObjInx].objType,
         srbObjTypeEntry[newObjInx].systemType,
          myResCache->resInfoHead->phyResLoc, newFullPathName, NO_CHKDIR);
        freeInfoOut (infoOut);
#ifdef LOCK_OBJ
        if (lockFd >= 0)
            _svrObjLock (infoOut->objID, collectionName, 
              F_SETLKW, F_UNLCK, lockFd);
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
#ifdef LOCK_OBJ
            if (lockFd >= 0)
                _svrObjLock (infoOut->objID, collectionName, 
                  F_SETLKW, F_UNLCK, lockFd);
#endif
	    return (intReplNum);
        }
    }

    /* Everything is fine. Unlink the original data object */

    rmContentOfInfoOut (infoOut, infoOut->collection);
#ifdef foo
    _objUnlink (srbObjTypeEntry[origObjInx].objType,
      srbObjTypeEntry[origObjInx].systemType,
      infoOut->resourceLoc, infoOut->dataPath, NO_CHKDIR);
#endif

#ifdef LOCK_OBJ
    if (lockFd >= 0)
        _svrObjLock (infoOut->objID, collectionName, 
          F_SETLKW, F_UNLCK, lockFd);
#endif

    freeInfoOut (infoOut);

#ifdef SRB_MDAS
    if (retVal == 0)
        commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    return retVal;
}

int
svrObjMoveColl (int catType, struct varlena *vCollectionName, int flag,
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
        if (destResource != NULL) {
            destResource[0] = '\0';
        }
    }

    flag = 0;
    status = _svrObjMoveColl (catType, collectionName, flag, srcResource,
      destResource, containerName);

    free (collectionName);
    free (srcResource);
    free (destResource);
    free (containerName);

    return (status);
}

int
_svrObjMoveColl (int catType, char *collectionName, int flag, 
char *srcResource, char *destResource, char *containerName)
{
    int status, i;
    mdasC_sql_result_struct *myresult = NULL;
    int continuation_index = 1;
    char *inCollection, *inObjID;


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

    while (continuation_index >= 0) {
        inCollection = (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

        inObjID = (char *)getFromResultStruct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_NAME],
            dcs_aname[DATA_NAME]);

        for (i = 0; i < myresult->row_count; i++) {

            status = _svrObjMove (catType, inObjID, inCollection,
              srcResource, destResource, "", containerName);
	    if (status < 0) {
                freeSqlResult (myresult);
                return (status);
            }

	    inCollection += MAX_DATA_SIZE;
	    inObjID += MAX_DATA_SIZE;
	}
        continuation_index = myresult->continuation_index;
        freeSqlResult (myresult);

        if (continuation_index >= 0) {
            status = _svrGetMoreRows (catType, continuation_index, &myresult,
              BULK_MOVE_NUM_ROWS);

            if (status < 0) {
                if (status == MCAT_INQUIRE_ERROR) {
                    status = 0;
                } else {
                    elog (NOTICE,
                     "_svrObjMoveColl: _svrGetMoreRows error. status = %d",
                      status);
                }
                break;
            }
        } else {
            break;
        }
    }	/* while loop */
    return (status);
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
    srb_long_t *retVal;

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
    struct mdasResInfo *compResInfo;
    int intReplNum;
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

    /* sortAndCombInfo added to make sure dirty copu go first 11/15/04 */
    sortAndCombInfoForObj (&infoOutHead, NULL, 0, 0);

    while (tmpResInfo != NULL) {
	int tinx;
	int regMode;
	srb_long_t registerdSize;
	struct mdasInfoOut *srcInfoOut;

        if (copyCnt >= condResult.intResult) {   /* Done */
	    retVal = infoOutHead->size;
            break;
	}

	srcInfoOut = infoOutHead;
	while (srcInfoOut != NULL) {
            bytesCopied = _svrDataCopyFromInfoToRescOrCompResc (MDAS_CATALOG,
              destObjID, destCollection, tmpResInfo, srcInfoOut, "",
              fullPathName, &tinx, &lockFd, &compResInfo);
#ifdef foo	/* done in _svrDataCopyFromInfoToRescOrCompResc */
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
#endif	/* foo */

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

        /* Do the registration */

        compPathName[0] = '\0';         /* make _svrGenRegisterDataset
                                         * to generate a path */
        if (copyCnt <= 1) {
            regMode = NEW_FIRST_COPY;
            registerdSize = 0;
        } else {
            regMode = NEW_REPLICA;
            registerdSize = bytesCopied;
        }

        retVal = _svrGenRegisterDataset (MDAS_CATALOG, destObjID,
         destCollection, ClientUser->userName, ClientUser->domainName,
         regMode, tmpResInfo, compResInfo, fullPathName,
         bytesCopied, infoOutHead->dataType, tinx, NULL, NULL, registerdSize,
         compPathName, &intReplNum, &lockFd);

        if (compResInfo != NULL) {
            myPathName = compPathName;
        } else {
            myPathName = fullPathName;
        }

#ifdef foo      /* use _svrGenRegisterDataset instead */
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
#endif	/* foo */

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
    struct mdasInfoOut *infoOutHead, *tmpInfoOut;
    struct mdasInfoOut *dirtyInfoOut, *cleanInfoOut;
    struct mdasResInfo *newResInfo, *tmpResInfo;
    int retVal;
    int savedRetVal = 0;
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
    struct mdasInfoOut *newInfoOut = NULL;
    int savedRetVal = 0;
    int copyMade = 0;
    int condSatisfied = 0;

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

int lowLevelCreate (int tinx, char *resourceLoc, 
char *fullPathName, int mode, int chkdirFlag, srb_long_t dataSize)
{
    int exfInx = -1;

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
      case MDRIVER_TYPE:
	exfInx = _MDriverCreate (srbObjTypeEntry[tinx].systemType,
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

    retVal = getInfoOutForOpr (srcInfo, srcInfo->collection, O_RDONLY,
      &lockFd, 1, &myInfoOut, &cacheInfo);

    if (retVal < 0) {
	return (retVal);
    }

#ifdef foo	/* done in getInfoOutForOpr */
    if (srbObjTypeEntry[srcInfo->objTypeInx].objType == COMPOUND_TYPE) {
        /* a compound object */
#ifdef LOCK_OBJ
            /* Don't lock container object. It has its own locking */
            if (srcInfo->containerName[0] == '\0') {
                lockFd =_svrObjLock (srcInfo->objID, collectionName,
                  F_SETLKW, F_RDLCK, 0);
            }
#endif
        retVal = _svrStageCompObj (srcInfo, srcInfo->collection, &cacheInfo, 1);
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
#endif	/* foo */

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
_objUpdate (int catType, char *objID, char *collection,
struct mdasInfoOut *dirtyInfoOut, struct mdasInfoOut *cleanInfoOut)
{
    int retVal;

    retVal = syncInfoToInfo (dirtyInfoOut, collection, cleanInfoOut,
      collection, 1);

    return (retVal);
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
#ifdef foo	/* done in _svrRegNewObjSize */
#if defined(PORTNAME_osx)
                    sprintf (strSize, "%-qd", byteWritten);
#else
                    sprintf (strSize, "%-lld", byteWritten);
#endif
#endif	/* foo */
                    if (destInfoOut != myDestInfoOut) {
                        /* A InternalCompObj. Register the new size */
			retVal = _svrRegNewIntnernalCompsize (MDAS_CATALOG,
                          myDestInfoOut->objID, destCollection,
                          myDestInfoOut->replNum, myDestInfoOut->segNum,
                          myDestInfoOut->intReplNum, myDestInfoOut->intSegNum,
                          byteWritten);
#ifdef foo	/* done in _svrModInternalCompObj */
			retVal = _svrModInternalCompObj (
			  myDestInfoOut->objID, destCollection,
			  myDestInfoOut->replNum, myDestInfoOut->segNum,
			  myDestInfoOut->intReplNum, myDestInfoOut->intSegNum,
			  ClientUser->userName, ClientUser->domainName,
			  byteWritten, "1", NULL, NULL,
			  D_COMPOUND_MODIFY_SIZE_AND_DIRTY);
#endif	/* foo */
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

                    retVal = _svrRegNewObjSize (MDAS_CATALOG,
                      destInfoOut->objID, destCollection,
                      destInfoOut->resourceName, destInfoOut->dataPath,
                      byteWritten);
#ifdef foo	/* done in _svrRegNewObjSize */
                    retVal = _svrModifyDataset (MDAS_CATALOG, 
		      destInfoOut->objID, destCollection, 
		      destInfoOut->resourceName, destInfoOut->dataPath,
                      strSize, NULL, D_CHANGE_SIZE);
#endif	/* foo */

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

/* _objChksumAndReg -
 * myPathName and phyResLoc are only needed if the file is
 * not already opened, i.e., inpLowLevelDescInx < 0.
 * For compound resource, myPathName and phyResLoc are the values
 * for the actual internal component and regResName and regPathName
 * are the values for the top level compound object.
 * for noraml objects, myPathName and phyResLoc should be the same as 
 * regResName and regPathName.
 */

int
_objChksumAndReg (int objTypeInx, int flags, char *chksum,
srb_long_t myOffset, srb_long_t mySize, char *destObjID, char *destCollection, 
char *myPathName, char *phyResLoc,
int chksumFlag, unsigned long iChksum, char *regResName, char *regPathName,
int inpLowLevelDescInx)
{
    int lowLevelDescInx = -1;
    int retVal;

    if (chksumFlag & K_FLAG) {
        if (inpLowLevelDescInx < 0) {
	    lowLevelDescInx = lowLevelOpen (objTypeInx,
             myPathName, phyResLoc, O_RDONLY);
           if (lowLevelDescInx < 0) {
                elog(NOTICE,
	         "_objChksumAndReg: Unable to open %s for chksum, stat=%d",
                 myPathName, lowLevelDescInx);
	        return (lowLevelDescInx);
	    }
	} else {
	    lowLevelDescInx = inpLowLevelDescInx;
        }

        retVal = _objChksum (objTypeInx, lowLevelDescInx, flags, chksum, 
         myOffset, mySize);

	/* XXXXX close it here instead of in svrFileSum */
	if (inpLowLevelDescInx < 0) {
            _objClose (objTypeInx, lowLevelDescInx);
	}
        if (retVal < 0) {
            elog (NOTICE,
             "_objChksumAndReg: _objChksum error for obj %s, status = %d",
              destObjID, retVal);
	    return (OBJ_ERR_BAD_CHKSUM);
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

    if (chksumFlag & s_FLAG) {
	/* check the size of the physical file */
	status = _svrObjChkPhySize (objID, collectionName, chksumFlag,
         VAROUTDATA(retval));
    } else {
        status = _svrObjChksum (objID, collectionName, chksumFlag, inpChksum,
         VAROUTDATA(retval));
    }

    free (objID);
    free (collectionName);
    free (inpChksum);

    if (status >= 0 || (chksumFlag & s_FLAG))
        VARSIZE(retval) = CHKSUM_STR_LEN + VAROUTHDRSZ;
    else
        VARSIZE(retval) = VAROUTHDRSZ;

    VAROUTSTAT(retval) = htonl (status);

    return retval;
}

int
_svrObjChkPhySize (char *objID, char *collectionName, int chksumFlag,
char *outSize)
{
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead, *myInfoOut, *tmpInfoOut; 
    struct mdasInfoOut *cacheInfoOut = NULL, *tapeInfoOut = NULL;
    int retVal;
    unsigned long iChksum;
    int status = 0;
    srb_long_t verifySize = 0;

    infoInp.domainName = ClientUser->domainName;
    infoInp.accessMode = openFlagLookUp (O_RDONLY);
    infoInp.objID = objID;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = collectionName;

    retVal = mdasGetInfo (&infoInp, &infoOutHead);
    if (retVal < 0) {
        elog (NOTICE,
         "_svrObjChkPhySize: mdasGetInfo of %s/%s error, stat = %d",
         collectionName, objID, retVal);
        return (retVal);
    }

    tmpInfoOut = infoOutHead;
    while (tmpInfoOut != NULL) {
	int destTypeInx;

	if (tmpInfoOut->containerName[0] != '\0') {
	    /* skip container */
	    retVal = 0;
	    outSize[0] = '\0';
	    tmpInfoOut = tmpInfoOut->next;
	    continue;
	}
	    
	destTypeInx = tmpInfoOut->objTypeInx;

        if (srbObjTypeEntry[destTypeInx].objType != FILE_TYPE) {
            retVal = 0;
            outSize[0] = '\0';
            tmpInfoOut = tmpInfoOut->next;
            continue;
        }

	if (tmpInfoOut->compoundLink) {
            retVal = _svrGetInternalObjInfo (tmpInfoOut->objID, 
	      tmpInfoOut->collection, tmpInfoOut->replNum, NO_INT_SEGMENTATION,
      	      &cacheInfoOut, &tapeInfoOut);
	    combineInfoOut (&cacheInfoOut, &tapeInfoOut);
	    tapeInfoOut = NULL;
	    if (cacheInfoOut == NULL) {
                retVal = 0;
                outSize[0] = '\0';
                tmpInfoOut = tmpInfoOut->next;
                continue;
	    }
	    sortAndCombInfoForObj (&cacheInfoOut, NULL, 0, 0);
	    myInfoOut = cacheInfoOut;
            destTypeInx = myInfoOut->objTypeInx;
	} else {
	    myInfoOut = tmpInfoOut;
	}


        verifySize = _getFileSizeByPath (
             srbObjTypeEntry[destTypeInx].systemType, myInfoOut->resourceLoc, 
	      myInfoOut->dataPath);
        if (verifySize != myInfoOut->size) {
            elog (NOTICE,
             "_svrObjChkPhySize:phySize %lld do not match MCAT %lld for %s/%s",
            verifySize, myInfoOut->size, collectionName, objID);
            retVal = OBJ_ERR_COPY_LEN;
	    break;
        } else {
	    retVal = 0;
	}
	if (cacheInfoOut != NULL) {
	    freeAllInfoOut (cacheInfoOut);
	    cacheInfoOut = NULL;
	}
	tmpInfoOut = tmpInfoOut->next;
    }
    sprintf (outSize, "%lld", verifySize);
    freeAllInfoOut (infoOutHead);
    
    return (retVal);
}

int
_svrObjChksum (char *objID, char *collectionName, int chksumFlag, 
char *inpChksum, char *chksum)
{
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead, *nextInfoOut, *tmpInfoOut;
    int retVal;
    int openFlag;
    int descInx = -1;
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
    	    freeAllInfoOut (infoOutHead);
	    return (OBJ_ERR_BAD_CHKSUM);
	}
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
    freeAllInfoOut (infoOutHead);

    if (status < 0)
	return (status);
    else
	return (retVal);
}
	
int
_chksumOpenedObj (int descInx, char *chksum)
{
    int fileType, objTypeInx = -1, lowLevelDescInx = -1, contInx = -1;
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

int
_objCreateWithResOrCompInfo (char *objID, char *collectionName,
char *inpPathName, srb_long_t dataSize, struct mdasResInfo *resInfo,
char *outFullPathName, int *objTypeInx, int *lockFd, 
struct mdasResInfo **outCompResInfo)
{
    int exfInx = -1;
    struct mdasResInfo *compResInfo = NULL;

    if (strcmp (resInfo->phyResType, COMPOUND_RESC) == 0) {
        compResInfo = resInfo->compoundLink;
        while (compResInfo != NULL) {
            int resClassInx;
            resClassInx = resClassLookup (compResInfo->phyResClass);
           if (resClassEntries[resClassInx].permanent == 0) {
#ifdef LOCK_OBJ
                if (*lockFd < 0)
                    *lockFd =_svrObjLock (objID, collectionName,
                      F_SETLKW, F_WRLCK, 0);
#endif
                exfInx = _objCreateWithResInfo (objID, collectionName,
                 "", dataSize, compResInfo, outFullPathName, objTypeInx);
                if (exfInx >= 0)
                    break;
            }
            compResInfo = compResInfo->next;
        }
    } else {
        exfInx = _objCreateWithResInfo (objID, collectionName, inpPathName,
         dataSize, resInfo, outFullPathName, objTypeInx);
    }
    *outCompResInfo = compResInfo;
    return (exfInx);
}

srb_long_t 
_svrDataCopyFromInfoToRescOrCompResc (int catType,
char *destObjID, char *destCollection, struct mdasResInfo *destResInfo,
struct mdasInfoOut *srcInfoOut, char *inpPathName, 
char *outFullPathName, int *objTypeInx, int *lockFd,
struct mdasResInfo **outCompResInfo)
{
    srb_long_t bytesCopied;
    struct mdasResInfo *compResInfo = NULL;

    if (strcmp (destResInfo->phyResType, COMPOUND_RESC) == 0) {
        compResInfo = destResInfo->compoundLink;
        while (compResInfo != NULL) {
            int resClassInx;
            resClassInx = resClassLookup (compResInfo->phyResClass);
            /* Don't do permanent */
            if (resClassEntries[resClassInx].permanent == 0) {
#ifdef LOCK_OBJ
                /* Don't lock container object.It has its own locking */
                if (srcInfoOut->containerName[0] == '\0' &&
                  *lockFd < 0) {
                    *lockFd =_svrObjLock (destObjID, destCollection,
                      F_SETLKW, F_WRLCK, 0);
                }
#endif
                bytesCopied = _svrDataCopyFromInfoToResc (catType,
                  destObjID, destCollection,
                  srcInfoOut, compResInfo, "", outFullPathName, objTypeInx);
                if (bytesCopied >= 0)
                    break;
            }
            compResInfo = compResInfo->next;
        }
    } else {
        bytesCopied = _svrDataCopyFromInfoToResc (catType,
          destObjID, destCollection,
          srcInfoOut, destResInfo, "", outFullPathName, objTypeInx);
    }
    *outCompResInfo = compResInfo;
    return (bytesCopied);
}

int
genLowLevelOpen (struct mdasInfoOut *inpInfoOut, char *collectionName,
int openFlag, int *lockFd, int stageFlag, struct mdasInfoOut **outCacheInfoOut)
{
    int status;
    struct mdasInfoOut *myInfoOut;

    status = getInfoOutForOpr (inpInfoOut, collectionName, openFlag, 
      lockFd, stageFlag, &myInfoOut, outCacheInfoOut);

    if (status < 0) {
	return (status);
    }

    status = lowLevelOpen (myInfoOut->objTypeInx, myInfoOut->dataPath,
     myInfoOut->resourceLoc, openFlag);

    if (status < 0 && *outCacheInfoOut != NULL) {
#ifdef LOCK_OBJ
        if (*lockFd >= 0) {
            _svrObjLock (inpInfoOut->objID, collectionName,
              F_SETLKW, F_UNLCK, *lockFd);
            *lockFd = -1;
        }
#endif
	freeAllInfoOut (*outCacheInfoOut);
	*outCacheInfoOut = NULL;
    }
    return (status);
}
