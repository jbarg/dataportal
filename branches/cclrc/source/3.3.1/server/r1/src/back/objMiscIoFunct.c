/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/* objMiscIoFunct - Misc Routines to handle Object type SID.
 */
#include "objMiscIoFunct.h"

/* #define AUTO_SYNC_FULL_CONTAINER 1 */	/* XXXXXXX take me out */

/* XXXX this section may be replaced by code from Mdas */

int AuditSetting = AUDIT_TRAIL_OFF;

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
            srbObjDesc[i].segNum = NO_INT_SEGMENTATION;
	    srbObjDesc[i].objSize = -1;
	    srbObjDesc[i].contInx = -1;
#ifdef LOCK_OBJ
	    srbObjDesc[i].lockFd = -1;
#endif
	    srbObjDesc[i].isDir = IS_FILE;
	    srbObjDesc[i].intReplNum = -1;
	    srbObjDesc[i].intSegNum = NO_INT_SEGMENTATION;
	    srbObjDesc[i].fileGroup[0] = '\0';
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

    for (descInx = 0; descInx < MAX_SRBO_DESC; descInx++) {
        if (srbObjDesc[descInx].lowLevelDescInx >= 0) {
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

void 
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
void
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

void
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

void 
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

int
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


void
fillObjDesc (int descInx, int lowLevelDescInx, int objTypeInx, 
char *objID, char *collection, char *dataPath, char *resourceName, 
int replNum, srb_long_t objSize)
{
    srbObjDesc[descInx].lowLevelDescInx = lowLevelDescInx;
    srbObjDesc[descInx].objTypeInx = objTypeInx;
    srbObjDesc[descInx].objID = strdup (objID);
    srbObjDesc[descInx].dataPath = strdup (dataPath);
    srbObjDesc[descInx].collection = strdup (collection);
    srbObjDesc[descInx].resourceName = strdup (resourceName);
    srbObjDesc[descInx].replNum = replNum;
    srbObjDesc[descInx].objSize = objSize;
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
    char *inputStr1, *inputStr2, *inputStr3, *inputStr4, *inputStr5;
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
    if (FORK() == 0) {  /* child */
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

/*
 * svrObjLockRange - set or check a lock on a range within an Object
 *         This uses fcntl lock operations so the physical file must be
 *         on a Unix/Linux file system.
 *
 * Input - int fd_inx - The file descriptor to lock (from srbFileOpen or
 *         srbFileCreate).
 *         srb_long_t offset - The position of the operation
 *         int size - The length of the lock range.
 *         int mode - The operation mode (MDSPlus convention) :
 *            0 Unlock
 *            1 ReadLock
 *            2 WriteLock
 *
 * Output - Returns 0 upon success.
 *          Returns -1 upon failure.
*/
int
svrObjLockRange(int descInx, srb_long_t *offset, int size, int mode)
{
    int retVal;

    srb_long_t myOffset;

    myOffset = *offset;

    retVal = _svrObjLockRange (descInx, myOffset, size, mode);
    return (retVal);
}

/*
 Internal version of srvObjLockRange
 */
int
_svrObjLockRange(int descInx, srb_long_t offset, int size, int mode)
{
    int tinx;
    int retVal;
    if (descInx < 0 || descInx >= MAX_SRBO_DESC ||
    srbObjDesc[descInx].lowLevelDescInx < 0) {
        elog(NOTICE,"svrObjLockRange: Invalid desc (%d)", descInx);
        retVal = INP_ERR_DESC;
        return retVal;
    }
    tinx = srbObjDesc[descInx].objTypeInx;
    if (srbObjTypeEntry[tinx].objType == FILE_TYPE) {
        retVal = _svrFileLockRange (srbObjDesc[descInx].lowLevelDescInx, 
                                    offset, size, mode);
    }
    else {
        elog(NOTICE, "svrObjLockRange: attempting to LockRange non-File");
        retVal = OBJ_ERR_OBJ_TYPE;
    }
    return (retVal);
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
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Do a make_new_collection */

    remoteFlag = getMdasEnabledHost (parentCollect, &hostTabPtr);

    if (remoteFlag < 0) {
        return (remoteFlag);
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        char *userName;
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
#ifdef SRB_MDAS
        char *userName;
        userName = ClientUser->userName;
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
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    /* Do a modify_collection_info */

    remoteFlag = getMdasEnabledHost (collectionName, &hostTabPtr);

    if (remoteFlag < 0) {
	retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        char *userName;
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
    int j;
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

/* genCompPath - Generate a fake pathName for the compound obj based on the
 * collection and objID of the object.
 */
int
genCompPath (char *outPath, char *objID)
{
    sprintf (outPath, "/%s.%ld.%ld", objID, random(), time(NULL));
    return (0);
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


/* this is a special stage handling routine for standford */
int
_standfStage (int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName, struct mdasInfoOut *origInfoOut,
int regFlag)
{
    struct hostElement  *hostTabPtr;
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum, tmpResLoc[MAX_TOKEN];
#endif
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

srb_long_t
_getWriteSize (int objTypeInx, int lowLevelDescInx)
{
    srb_long_t size;

    switch (srbObjTypeEntry[objTypeInx].objType) {
      case FILE_TYPE:
        size = _fileWriteSize (lowLevelDescInx);
        break;
      case DB_LOBJ_TYPE:
      case DB_TABLE_TYPE:
      case MDRIVER_TYPE:
      default:                /* shouldn't get here */
        size = 0;
        break;
    }
    return (size);
}

int
svrObjFStage (int catType, struct varlena *vObjID, struct varlena *vCollection, 
struct varlena *vResource, int flag)
{
    char *objID;
    char *collection;
    char *resource;
    int status;

    objID = varToStr (vObjID);
    collection = varToStr (vCollection);
    resource = varToStr (vResource);

    status = _svrObjFStage (catType, objID, collection, resource, flag);

    free (objID);
    free (collection);
    free (resource);

    return (status);
}

int 
_svrObjFStage (int catType, char *objID, char *collectionName, 
char *resource, int flag)
{
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead, *tmpInfoOut;
    int retVal;
    int objTypeInx;

    /* A ticket user is allowed to open with a read only flag */

    if (TicketUserFlag) {
        elog (NOTICE, "Illegal operation for a ticket user, status = %d",
	  ILLEGAL_OPR_TICKET_USER);
        return (ILLEGAL_OPR_TICKET_USER);
    }

    infoInp.domainName = ClientUser->domainName;

    infoInp.accessMode = openFlagLookUp (O_RDONLY);

    infoInp.objID = objID;
    infoInp.userName = ClientUser->userName;
    infoInp.collectionName = collectionName;

    /* Check with MDAS for the requested data */
    if ((retVal = mdasGetInfo (&infoInp, &infoOutHead)) < 0) {
	return retVal;
    }

    sortAndCombInfoForObj (&infoOutHead, NULL, O_RDONLY, 0);

    tmpInfoOut = infoOutHead;

    while (tmpInfoOut != NULL) {
	if (resource != NULL && strlen (resource) > 0) {
	    if (strcmp (tmpInfoOut->resourceName, resource) == 0) {
		objTypeInx = tmpInfoOut->objTypeInx;
		break;
	    }
	} else {
	    objTypeInx = tmpInfoOut->objTypeInx;
	    break;
	}
	tmpInfoOut = tmpInfoOut->next;
    }
    if (tmpInfoOut != NULL && 
     srbObjTypeEntry[objTypeInx].objType == FILE_TYPE) {
	retVal = _svrFileFStage (srbObjTypeEntry[objTypeInx].systemType,
	 tmpInfoOut->resourceLoc, tmpInfoOut->dataPath, flag);
    } else {
	retVal = INP_ERR_NO_SUPPORT;
    }

    freeAllInfoOut (infoOutHead);

    return (retVal);
}



struct varlena *svrObjProc(int descInx, int outLen,
	    struct varlena *vprocName,
	    struct varlena *vinBuf)
{
    struct varlena *retval;
    int totalread;
    int status;
    int bytestowrite;
    char *procName;

    retval = (struct varlena *)malloc(VAROUTHDRSZ +  outLen + 1);
    bytestowrite = VARSIZE(vinBuf)  - SIZEOF32;   
    procName = varToStr (vprocName);
    totalread = _svrObjProc (descInx, procName, 
			     VARDATA(vinBuf), bytestowrite,
			     VAROUTDATA(retval), outLen);
         
    if (totalread >= 0) {
        status = 0;
    } else {
        status = totalread;
        totalread = 0;
    }
    VARSIZE(retval) = totalread + VAROUTHDRSZ;
    VAROUTSTAT(retval) = htonl (status);

    return(retval);
}

int
_svrObjProc(int descInx, char *procName, char *inBuf, int inLen,
	    char *outBuf, int outLen )
{
   
    int totalread;
    int tinx;

    if (descInx < 0 || descInx >= MAX_SRBO_DESC ||
	srbObjDesc[descInx].lowLevelDescInx < 0) {
        elog(NOTICE,"_svrObjProc: Invalid desc (%d)", descInx);
        return INP_ERR_DESC;
    }
    
    tinx = srbObjDesc[descInx].objTypeInx;
    switch (srbObjTypeEntry[tinx].objType) {
	case FILE_TYPE:
	case DB_LOBJ_TYPE:
	case DB_TABLE_TYPE:
	    totalread = FUNCTION_NOT_SUPPORTED;
	    break;
	case MDRIVER_TYPE:
	    totalread = _MDriverProc (srbObjDesc[descInx].lowLevelDescInx,
				      procName,
				      inBuf, inLen,
				      outBuf, outLen );
	    break;
	default:          /* shouldn't get here */
	    elog(NOTICE, "_svrObjProc: error in srbObjTypeEntry mapping");
            totalread  = OBJ_ERR_OBJ_TYPE;
	    break;
	}
	
    return totalread ;
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

/* getInfoOutForOpr - The inpInfoOut can be normal or compound type. 
 * If it is compound type, stage it. Return the infoPut that is ready 
 * for operation 
 */

int
getInfoOutForOpr (struct mdasInfoOut *inpInfoOut, char *collectionName,
int openFlag, int *lockFd, int stageFlag, struct mdasInfoOut **outInfoOut,
struct mdasInfoOut **outCacheInfoOut)
{
    int tinx, lockType, status;
    struct mdasInfoOut *cacheInfo;

    tinx = inpInfoOut->objTypeInx;
    *outInfoOut = NULL;
    *outCacheInfoOut = NULL;

    if (srbObjTypeEntry[tinx].objType == COMPOUND_TYPE) {
            /* a compound object */
#ifdef LOCK_OBJ
        /* Don't lock container object. It has its own locking */
        if (inpInfoOut->containerName[0] == '\0' && *lockFd < 0) {
            if (openFlag & O_RDONLY)
                lockType = F_RDLCK;
            else
                lockType = F_WRLCK;

            *lockFd =_svrObjLock (inpInfoOut->objID, collectionName,
              F_SETLKW, lockType, 0);
        }
#endif
        status = _svrStageCompObj (inpInfoOut, collectionName,
          &cacheInfo, 1);

        if (status < 0) {       /* failure */
#ifdef LOCK_OBJ
            if (*lockFd >= 0) {
                _svrObjLock (inpInfoOut->objID, collectionName,
                  F_SETLKW, F_UNLCK, *lockFd);
                *lockFd = -1;
            }
#endif
	    return (status);
	}

	*outInfoOut = cacheInfo;
	*outCacheInfoOut = cacheInfo;
    } else {
	*outInfoOut = inpInfoOut;
	*outCacheInfoOut = NULL;
    }

    return (0);
}

int 
getAndQueNewObjDesc (int *headDescInx) 
{
    int tmpDescInx, descInx;

    descInx = newObjDesc ();
    if (descInx < 0) {
        return descInx;
    }

    /* Link it in a singlely linked chain */

    if (*headDescInx == -1) {
        *headDescInx = descInx;
    } else {
        tmpDescInx = *headDescInx;
        while (srbObjDesc[tmpDescInx].nextDescInx != -1)
            tmpDescInx = srbObjDesc[tmpDescInx].nextDescInx;
        srbObjDesc[tmpDescInx].nextDescInx = descInx;
    }
    srbObjDesc[descInx].nextDescInx = -1;

    return descInx;
}

void
lowLevelCloseAllObjDesc (int *headDescInx)
{
    int objTypeInx, tmpDescInx;
    int descInx = *headDescInx;

    while (descInx != -1) {
        objTypeInx = srbObjDesc[descInx].objTypeInx;
        lowLevelClose (objTypeInx, srbObjDesc[descInx].lowLevelDescInx);
        tmpDescInx = descInx;
        descInx = srbObjDesc[descInx].nextDescInx;
        delObjDesc (tmpDescInx);
    }
    *headDescInx = -1;
}
void
setObjDescCond (int descInx, char *condInput)
{
    int myInx;

    if (condInput == NULL || descInx < 0 || descInx >= MAX_SRBO_DESC) {
	return;
    }
 
    setChksumFromCond (condInput, &srbObjDesc[descInx].chksumFlag,
     &srbObjDesc[descInx].chksum);
#ifdef foo 	/* done in setChksumFromCond */
    if (parseCondInput (condInput, REG_CHKSUM, &condResult) >= 0) {
        srbObjDesc[descInx].chksumFlag = k_FLAG;
        srbObjDesc[descInx].chksum = condResult.intResult;
    } else if (parseCondInput(condInput, VERIFY_CHKSUM, &condResult) >= 0) {
        srbObjDesc[descInx].chksumFlag = K_FLAG;
        srbObjDesc[descInx].chksum = condResult.intResult;
    }
#endif
    /* have to initialize other desc too */
    if (srbObjDesc[descInx].chksumFlag != 0) {
        myInx = srbObjDesc[descInx].nextDescInx;
        while (myInx != -1) {
            srbObjDesc[myInx].chksumFlag = srbObjDesc[descInx].chksumFlag;
            srbObjDesc[myInx].chksum = srbObjDesc[descInx].chksum;
            myInx = srbObjDesc[myInx].nextDescInx;
        }
    }
    setFileGroupFromCond (condInput, srbObjDesc[descInx].fileGroup);    
    if (strlen (srbObjDesc[descInx].fileGroup) > 0) {
        myInx = srbObjDesc[descInx].nextDescInx;
        while (myInx != -1) {
            strcpy (srbObjDesc[myInx].fileGroup, srbObjDesc[descInx].fileGroup);
            myInx = srbObjDesc[myInx].nextDescInx;
        }
    }
}

void
setChksumFromCond (char *condInput, int *chksumFlag, int *iChksum)
{
    struct condResult condResult;

    if (condInput == NULL) {
        *chksumFlag = 0;
        *iChksum = 0;
	return;
    }

    if (parseCondInput (condInput, REG_CHKSUM, &condResult) >= 0) {
        *chksumFlag = k_FLAG;
        *iChksum = condResult.intResult;
    } else if (parseCondInput(condInput, VERIFY_CHKSUM, &condResult) >= 0) {
        *chksumFlag = K_FLAG;
        *iChksum = condResult.intResult;
    } else {
        *chksumFlag = 0;
        *iChksum = 0;
    }
}

void
setFileGroupFromCond (char *condInput, char *outFileGroup)
{
    struct condResult condResult;

    if (outFileGroup == NULL)
	return;

    if (condInput == NULL) {
	*outFileGroup = '\0';
        return;
    }

    if (parseCondInput (condInput, FILE_GROUP, &condResult) >= 0) {
	if (strlen (condResult.strResult) >= ENV_LEN) {
	    elog (NOTICE, 
	     "setFileGroupFromCond: fileGroup: %s from %s@%s longer than %d",
	     condResult.strResult, 
	     ClientUser->userName, ClientUser->domainName, ENV_LEN);
            *outFileGroup = '\0';
	} else {
	    strcpy (outFileGroup, condResult.strResult);
	}
    } else {
        *outFileGroup = '\0';
    }
}

