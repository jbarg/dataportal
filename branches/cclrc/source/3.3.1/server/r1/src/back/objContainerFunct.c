/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/* objContainerFunct.c - Routines to handle Container Object type SID.
 */
#include "objContainerFunct.h"

/* #define AUTO_SYNC_FULL_CONTAINER 1 */	/* XXXXXXX take me out */
		
int 
inContainerObjCreate (int catType, char *objID, char *collectionName,
char *containerName, char *dataType, srb_long_t dataSize)
{
    int retVal, status;
    int descInx, contInx;
    char outContName[MAX_TOKEN], contCollection[MAX_TOKEN], 
     fullContName[MAX_TOKEN];
    int lockFd = -1;
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
    int status;
    int descInx;
    char fullContName[MAX_TOKEN];
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
    struct mdasInfoOut *infoOutHead, *tmpInfoOut; 
#ifdef AUTO_SYNC_FULL_CONTAINER
    struct mdasInfoOut *maxInfoOut;
#endif
    struct mdasInfoOut *emptyContHead;
    char fullContPath[MAX_TOKEN];
    char fullNewContPath[MAX_TOKEN];
    char *logResName;
    srb_long_t iContainerMaxSize, maxSize;

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
#ifdef AUTO_SYNC_FULL_CONTAINER
	    maxInfoOut = tmpInfoOut;
#endif
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
	    /* try one more time */
	    srbThreadSleep (1, 0);
            sprintf (newContName, "%s.%-d", containerName, time (NULL));
            /* rename the current container */
            retVal =  _svrModifyDataset (catType, containerName, contCollection,
             "", "", newContName, NULL, D_CHANGE_DNAME);
	}

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
	    struct hostElement  *hostTabPtr;
            int remoteFlag;
	    char *hostAddr;
#ifndef FED_MCAT
            char *portNum, tmpResLoc[MAX_TOKEN];
#endif

	    if (maxInfoOut == NULL) {
		hostAddr = NULL;
	    } else {
		hostAddr = maxInfoOut->resourceLoc;
	    }
#ifdef FED_MCAT
            remoteFlag = chkHostAndZone (hostAddr, &hostTabPtr);
#else

            /* Parse the hostAddr */
	    strcpy (tmpResLoc, hostAddr);
	    portNum = parseAddr (tmpResLoc);
    	    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif
            /* closeout opened socket and desc */
            /* svrCloseAll (); */

	    hostTabPtr->conn = NULL;
            if ((retVal = remoteConnect (hostTabPtr)) < 0) {
                return retVal;
            } else {
                retVal = srbSyncContainer (hostTabPtr->conn, catType, 
		fullNewContPath,
                 PRIMARY_FLAG | PURGE_FLAG);
		if (retVal < 0) {
		    elog (NOTICE, 
		     "AUTO_SYNC: srbSyncContainer of %s failed, status = %d",
		    fullNewContPath, retVal);
		    exit (1);
		}
	    }
#ifdef foo
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
#endif	/* foo */
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
    struct mdasInfoOut *tmpInfoOut, *nextInfoOut;
    /* empty container */
    struct mdasInfoOut *empCacheInfoOut, *empArchInfoOut, 
     *empCacheInfoOut1, *empArchInfoOut1, 
      *empCacheInfoOut2, *empArchInfoOut2;  
    int retVal;
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
                /* mark it dirty if the orig copy is dirty */
                if (tmpArchInfoOut->isDirty) {
                    _svrModifyDataset (catType, containerName,
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
    int lockFd = -1;

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
    struct mdasInfoOut *infoOutHead, *tmpInfoOut;
    struct mdasInfoOut *cacheInfoOut, *archInfoOut, *myInfoOut,
     *cacheInfoOut1, *archInfoOut1,
      *dirtyCacheInfoOut, *dirtyArchInfoOut;
    mdasC_sql_result_struct *myresult = NULL;
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
        if (retVal == 0) {      /* no match */
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
    } else {    /* it is dirty */
        freeAllInfoOut (cacheInfoOut);
        freeAllInfoOut (cacheInfoOut1);
        freeAllInfoOut (archInfoOut);
        freeAllInfoOut (archInfoOut1);
        infoOutHead = dirtyCacheInfoOut;
    }

    if (infoOutHead == NULL) {          /* no source */
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
    int gotCopy = 0;
    struct mdasResInfo *compResInfo;
    int intReplNum;
    int remoteFlag;
    struct hostElement  *hostTabPtr;
    struct resCache *myResCache;

    if (containerSize > MAX_CONTAINER_SIZE) {
	return (OBJ_ERR_MAX_SIZE_EXCEEDED);
    } else if (containerSize <= 0) {
	containerSize = DEF_CONTAINER_SIZE;
    }

    /* get the mdasResInfo associated with a logResName and domainName */

    if ((status = getResInfo (inpContainerName, catType, logResName, 
     ClientUser->domainName, &myResCache)) < 0) {
        elog (NOTICE,
          "svrContainerCreate: getResInfo for reso %s failed, stat = %d",
          logResName, status);
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
        return retVal;
    }

    /* Loop through all permanent resources */
    tmpResInfo = myResCache->resInfoHead;
    while (tmpResInfo != NULL) {
	int fakeLockFd = 100;	/* no lock */
	
        exfInx = _objCreateWithResOrCompInfo (containerName, collectionName,
         "", containerSize, tmpResInfo,
         fullPathName, &tinx, &fakeLockFd, &compResInfo);

#ifdef foo      /* done using _objCreateWithResOrCompInfo */

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
#endif	/* foo */

        if (exfInx < 0) {	/* error */
	    retVal = exfInx;
            tmpResInfo = tmpResInfo->next;
            continue;
        }

        /* Close the object */

        lowLevelClose (tinx, exfInx);

        /* Do the registration */

       compPathName[0] = '\0';         /* make _svrGenRegisterDataset
                                         * to generate a path */

        retVal = _svrGenRegisterDataset (catType, 
	 containerName, collectionName,
          userName, ClientUser->domainName, NEW_FIRST_COPY, 
	 tmpResInfo, compResInfo,
         fullPathName, 0, containerType, tinx,
         NULL, NULL, 0, compPathName, &intReplNum, &fakeLockFd);

#ifdef foo	/* done in _svrGenRegisterDataset */
        /* myPathName is the path that was actually used */
        if (compResInfo != NULL) {
            /* Create a fake pathName for the compound obj */
            genCompPath (compPathName, containerName);
            myPathName = compPathName;
        } else {
            myPathName = fullPathName;
        }

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
#endif	/* foo */

        if (retVal < 0) {
            break;
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
svrContainerOpen (int catType, struct varlena *vContainerName,
int openFlag)
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
    int lockFd = -1;

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
        return (retVal);
    }


#ifdef LOCK_OBJ
    /* lock the container */

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

int inContainerObjOpen (char *objID, char *collectionName, int openFlag,
struct mdasInfoOut *infoOutHead)
{
    int retVal, status;
    int descInx, contInx;
    char outContName[MAX_TOKEN], contCollection[MAX_TOKEN];
    int myOpenFlag;

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
    int lockFd = -1;
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
    int status, lockFd = -1;

    containerName = varToStr (vContainerName);

#ifdef LOCK_OBJ
    status = svrParseContainerName (containerName, dataName, collection,
     ClientUser->userName, ClientUser->domainName);
    if (status < 0) {
        elog (NOTICE,
         "_svrSyncContainer: Error parsing container %s. status = %d",
          containerName, status);
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
		    return (status);
		} else { 		/* Don't have other copy, no purge */
            	    freeAllInfoOut (archInfoOut1);
            	    freeAllInfoOut (cacheInfoOut);
            	    freeAllInfoOut (cacheInfoOut1);
            	    freeAllInfoOut (dirtyCacheInfoOut);
            	    freeAllInfoOut (dirtyArchInfoOut);
            	    freeAllInfoOut (empArchInfoOut1);
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
            return (status);
	} else {
            freeAllInfoOut (dirtyCacheInfoOut);
            freeAllInfoOut (dirtyArchInfoOut);
            freeAllInfoOut (archInfoOut);
            freeAllInfoOut (empArchInfoOut);
            freeAllInfoOut (cacheInfoOut);
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
