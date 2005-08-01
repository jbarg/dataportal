/* compoundObj.c - Compound Objects */

#include "compoundObj.h"
#ifdef _WIN32
#include "Unix2Nt.h"
#endif

struct varlena *
svrStageCompObj (struct varlena *vObjID, struct varlena *vCollectionName,
struct varlena *vResourceLoc, struct varlena *vResourceName, 
srb_long_t *offset, srb_long_t *dataSize, int replNum, int segNum, 
int stageFlag)
{
    struct mdasInfoOut *compInfoOut;
    struct mdasInfoOut *outCacheInfoOut = NULL;
    int status;
    struct varlena *packedResult;


    compInfoOut = malloc (sizeof (struct mdasInfoOut));

    memset (compInfoOut, 0, sizeof (struct mdasInfoOut));

    compInfoOut->objID = varToStr (vObjID);
    compInfoOut->resourceLoc = varToStr (vResourceLoc);
    compInfoOut->resourceName = varToStr (vResourceName);
    compInfoOut->size = *dataSize;
    compInfoOut->collection = varToStr (vCollectionName);
    compInfoOut->replNum = replNum;
    compInfoOut->segNum = segNum;

    status = _svrStageCompObj (compInfoOut, compInfoOut->collection, 
     &outCacheInfoOut,
      stageFlag);

    freeInfoOut (compInfoOut);

    if (status < 0) {
	return (genIntRet (status));
    }

    status = packMsg ((char *) outCacheInfoOut, &packedResult, 
      mdasInfoOut_PF, NULL);

    freeInfoOut (outCacheInfoOut);

    if (status < 0) {
        return (genIntRet (status));
    }

    /* VAROUTSTAT(packedResult) = htonl (VARSIZE (packedResult)); */
    VAROUTSTAT(packedResult) = htonl (status);
    return (packedResult);
}

/* _svrStageCompObj () - stage a file from tape. When this function is
 * called, it is assumed that the local host is the host where the 
 * tape drives are located. Also, it is assumed the myTapeFileInfo is
 * sorted by tape number and tapePos. 
 * If stageFlag == 0, if there is no cache copy, just make one up with 
 * zero length without truely staging from tape 
 */


int
_svrStageCompObj (struct mdasInfoOut *compInfoOut, char *collectionName,
struct mdasInfoOut **outCacheInfoOut, int stageFlag)
{
    srbConn *conn = NULL;
    int retVal;
    struct mdasInfoOut *tmpTapeInfo;
#ifdef TAPE_DRIVE
    tapeDev_t tapeDev;
#endif
    int exfInx;
    int intReplNum;
    struct mdasResInfo *compResInfo, *tmpCompResInfo;
    struct mdasInfoOut *cacheInfoOut, *tapeInfoOut;
    char fullPathName[MAX_TOKEN];
    int objTypeInx;
#ifndef FED_MCAT
    char *portNum, *tmpResLoc;
#endif
    struct hostElement  *hostTabPtr;
    int remoteFlag;
    srb_long_t bytesWritten;
    struct resCache *myCacheRes;

    if (outCacheInfoOut == NULL) {
        elog (NOTICE, "_svrStageCompObj: NULL outCacheInfoOut");
        *outCacheInfoOut = NULL;
        return (TAPLIB_INTERNAL_ERROR);
    }

    if (compInfoOut == NULL) {
        elog (NOTICE, "_svrStageCompObj: NULL compInfoOut");
        *outCacheInfoOut = NULL;
        return (TAPLIB_INTERNAL_ERROR);
    }

    *outCacheInfoOut = NULL;

    /* This must be run at the resourceLoc */

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (compInfoOut->resourceLoc, &hostTabPtr);
#else
    tmpResLoc = strdup (compInfoOut->resourceLoc);
    portNum = parseAddr (tmpResLoc);
    remoteFlag = chkHost (tmpResLoc, portNum, &hostTabPtr);
#endif

    if (remoteFlag < 0) {
        elog (NOTICE, "Invalid host addr %s", compInfoOut->resourceLoc);
        return INP_ERR_HOST_ADDR;               /* failure */
    } else if (remoteFlag == REMOTE_HOST && stageFlag > 0) {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }

        retVal = srbStageCompObj (hostTabPtr->conn, 
	  compInfoOut->objID, collectionName, 
          compInfoOut->resourceLoc, compInfoOut->resourceName, 
          compInfoOut->offset, compInfoOut->size, 
          compInfoOut->replNum, NO_INT_SEGMENTATION, stageFlag,
          outCacheInfoOut);
          return retVal;
    }

    /* If it got here, it is a local operation */

    retVal = _svrGetInternalObjInfo (compInfoOut->objID, collectionName,
      compInfoOut->replNum, NO_INT_SEGMENTATION,
      &cacheInfoOut, &tapeInfoOut);

    if (retVal < 0) {
	elog (NOTICE, "_svrStageCompObj: _svrGetInternalObjInfo () error %d  ", 
	 retVal);
	return (retVal);
    }
    
    if (cacheInfoOut == NULL && tapeInfoOut == NULL) {
	elog (NOTICE, 
	  "_svrStageCompObj:compound obj %s/%s has no internal object",
	  collectionName, compInfoOut->objID);
	return (TAPLIB_INTERNAL_ERROR);
    }

	
    if (cacheInfoOut != NULL) {
	/* already on cache. do some sanity check */

	tmpTapeInfo = tapeInfoOut;
	while (tmpTapeInfo != NULL) {
	    if (cacheInfoOut->isDirty > 0) {
		retVal = rmAllIntTapeCompObj (tmpTapeInfo, collectionName,
		  ClientUser->userName, ClientUser->domainName);
		if (retVal < 0) {
                    /* something is very wrong here */
                    freeAllInfoOut (tapeInfoOut);
                    freeAllInfoOut (cacheInfoOut);
                    return TAPLIB_INTERNAL_ERROR;
                }

	    } else if ((cacheInfoOut)->size != tmpTapeInfo->size) {
		elog (NOTICE, 
		  "_svrStageCompObj:cache/tape size mismatch for %s/%s",
		  compInfoOut->objID, collectionName);
		if ((cacheInfoOut)->size <= 0) {
		    /* something is very wrong here */
		    freeAllInfoOut (tapeInfoOut);
		    freeAllInfoOut (cacheInfoOut);
		    return OBJ_ERR_COPY_LEN;
		}
	    }
	    tmpTapeInfo = tmpTapeInfo->next;
	}
	if (cacheInfoOut->isDirty > 0) {
	    /* XXXXXXX set isDirty to not dirty */
	}
	*outCacheInfoOut = cacheInfoOut;
	freeAllInfoOut (tapeInfoOut);
	return 0;
    }
	
    /* Got here when cacheInfoOut == NULL and tapeInfoOut != NULL.
     * Need to stage from tape to cache.
     */

    /* try to create the cache copy */

    retVal = getCompoundResInfo (collectionName, MDAS_CATALOG, 
    compInfoOut->resourceName, &myCacheRes);

    if (retVal < 0) {
	elog (NOTICE, 
	  "_svrStageCompObj:getCompoundResInfo of %s error, status=%d",
	  compInfoOut->resourceName, retVal);
        freeAllInfoOut (tapeInfoOut);
	return retVal;
    }

    compResInfo = myCacheRes->resInfoHead;
    exfInx = -1;

    if (stageFlag <= 0) {
        tmpCompResInfo = compResInfo;
        while (tmpCompResInfo != NULL) {
            int resClassInx;
            /* XXXXX don't do TAPE_RESC 
            if (strcmp (tmpCompResInfo->phyResType, TAPE_RESC) != 0) { */
            resClassInx = resClassLookup (tmpCompResInfo->phyResClass);
            /* Don't do permanent */
            if (resClassEntries[resClassInx].permanent == 0) {
                exfInx = _objCreateWithResInfo (compInfoOut->objID, 
		  collectionName, "", compInfoOut->size, tmpCompResInfo, 
		  fullPathName, &objTypeInx);
                if (exfInx >= 0)
                    break;
            }
            tmpCompResInfo = tmpCompResInfo->next;
        }

        if (exfInx < 0) {
            elog (NOTICE,
              "_svrStageCompObj:Unable to create cache object for %s/%s",
              compInfoOut->objID, collectionName);
            freeAllInfoOut (tapeInfoOut);
            return exfInx;
        }

        _objClose (objTypeInx, exfInx);
        freeAllInfoOut (tapeInfoOut);

	/* register and return */
        intReplNum = _svrRegInternalCompObj (
          compInfoOut->objID, collectionName, compInfoOut->replNum,
          NO_INT_SEGMENTATION, tmpCompResInfo->phyResName, fullPathName,
          0, 0,
          NEW_INT_REPL_NUM, NO_INT_SEGMENTATION,
          objTypeInx, tmpCompResInfo->phyResLoc,
          ClientUser->userName, ClientUser->domainName);
        if (intReplNum < 0) {
            _objUnlink (srbObjTypeEntry[objTypeInx].objType,
              srbObjTypeEntry[objTypeInx].systemType,
              tmpCompResInfo->phyResLoc, fullPathName, NO_CHKDIR);
            return (intReplNum);
        }

        /* success. Have to create a new cacheInfoOut */
        cacheInfoOut = malloc (sizeof (struct mdasInfoOut));
        memset (cacheInfoOut, 0, sizeof (struct mdasInfoOut));
        cacheInfoOut->objID = strdup (compInfoOut->objID);
        cacheInfoOut->collection = strdup (collectionName);
	cacheInfoOut->intSegNum = NO_INT_SEGMENTATION;
        cacheInfoOut->dataPath = strdup (fullPathName);
        cacheInfoOut->resourceLoc = strdup (tmpCompResInfo->phyResLoc);
        cacheInfoOut->resourceName = strdup (tmpCompResInfo->phyResName);
        if (compInfoOut->dataType != NULL)
            cacheInfoOut->dataType = strdup (compInfoOut->dataType);
        else
            cacheInfoOut->dataType = NULL;
        if (compInfoOut->logResName != NULL) {
            cacheInfoOut->logResName = strdup (compInfoOut->logResName);
        } else {
            cacheInfoOut->logResName = NULL;
        }

        cacheInfoOut->objTypeInx = objTypeInx;
        cacheInfoOut->replNum = compInfoOut->replNum;
        cacheInfoOut->segNum = NO_INT_SEGMENTATION;
        cacheInfoOut->intReplNum = intReplNum;
	cacheInfoOut->intSegNum = NO_INT_SEGMENTATION;
        cacheInfoOut->offset = compInfoOut->offset;
        cacheInfoOut->size = 0;
        cacheInfoOut->isDirty = 0;
        cacheInfoOut->phyResClass = strdup (tmpCompResInfo->phyResClass);
        cacheInfoOut->resClassInx =
          resClassLookup (cacheInfoOut->phyResClass);
        if (compInfoOut->containerName != NULL) {
            cacheInfoOut->containerName = strdup (compInfoOut->containerName);
        } else {
            cacheInfoOut->containerName = NULL;
        }
        cacheInfoOut->containerMaxSize = compInfoOut->containerMaxSize;
        cacheInfoOut->tapeFileInfo = NULL;
        cacheInfoOut->compoundLink = NULL;
        cacheInfoOut->next = NULL;

        *outCacheInfoOut = cacheInfoOut;
        return (0);
    }

    /* got here when we need to stage the file */

    tmpCompResInfo = compResInfo;
    while (tmpCompResInfo != NULL) {
        int resClassInx;
        /* XXXXX don't do TAPE_RESC 
        if (strcmp (tmpCompResInfo->phyResType, TAPE_RESC) != 0) { */
        resClassInx = resClassLookup (tmpCompResInfo->phyResClass);
        /* Don't do permanent */
        if (resClassEntries[resClassInx].permanent > 0) {
            tmpCompResInfo = tmpCompResInfo->next;
	    continue;
	}

        if (srbObjTypeEntry [tapeInfoOut->objTypeInx].objType == TAPE_TYPE) {
#ifdef TAPE_DRIVE
            exfInx = _objCreateWithResInfo (compInfoOut->objID, 
              collectionName, "", compInfoOut->size, tmpCompResInfo, 
              fullPathName, &objTypeInx);
            if (exfInx < 0) {
                tmpCompResInfo = tmpCompResInfo->next;
		continue;
	    }

            memset (&tapeDev, 0, sizeof (tapeDev));
	    bytesWritten = _svrStageFromTape (conn, exfInx, objTypeInx, 
	      tapeInfoOut, &tapeDev);
            _objClose (objTypeInx, exfInx);
	    if (bytesWritten != (tapeInfoOut)->size) {
                _objUnlink (srbObjTypeEntry[objTypeInx].objType,
                  srbObjTypeEntry[objTypeInx].systemType,
                  tmpCompResInfo->phyResLoc, fullPathName, NO_CHKDIR);
            }
#else
	    bytesWritten = SYS_ERR_NO_SUPPORT;
#endif
        } else {
	    bytesWritten = _svrDataCopyFromInfoToResc (MDAS_CATALOG,
	      compInfoOut->objID, collectionName, tapeInfoOut, 
	      tmpCompResInfo, "", fullPathName, &objTypeInx);
        }
	if (bytesWritten == (tapeInfoOut)->size)
	    break;
        tmpCompResInfo = tmpCompResInfo->next;
    }

    if (bytesWritten == (tapeInfoOut)->size) {
        struct mdasInfoOut *myCacheInfoOut;

        /* register it */

        intReplNum = _svrRegInternalCompObj (
          tapeInfoOut->objID, collectionName, tapeInfoOut->replNum,
          NO_INT_SEGMENTATION, tmpCompResInfo->phyResName, fullPathName,
          compInfoOut->size, 0,
          NEW_INT_REPL_NUM, NO_INT_SEGMENTATION,
          objTypeInx, tmpCompResInfo->phyResLoc,
          ClientUser->userName, ClientUser->domainName);
        if (intReplNum < 0) {
	    retVal = intReplNum;
            _objUnlink (srbObjTypeEntry[objTypeInx].objType,
              srbObjTypeEntry[objTypeInx].systemType,
              tmpCompResInfo->phyResLoc, fullPathName, NO_CHKDIR);
        } else {
            /* success. Have to create a new cacheInfoOut */
            cacheInfoOut = myCacheInfoOut = 
	    malloc (sizeof (struct mdasInfoOut));
            memset (cacheInfoOut, 0, sizeof (struct mdasInfoOut));
            myCacheInfoOut->objID = strdup (compInfoOut->objID);
            myCacheInfoOut->collection = strdup (collectionName);
            myCacheInfoOut->dataPath = strdup (fullPathName);
            myCacheInfoOut->resourceLoc = strdup (tmpCompResInfo->phyResLoc);
            myCacheInfoOut->resourceName = strdup (tmpCompResInfo->phyResName);
            if (compInfoOut->dataType != NULL)
                myCacheInfoOut->dataType = strdup (compInfoOut->dataType);
            else
                myCacheInfoOut->dataType = NULL;
            if (compInfoOut->logResName != NULL) {
                myCacheInfoOut->logResName = strdup (compInfoOut->logResName);
            } else {
                myCacheInfoOut->logResName = NULL;
            }

            myCacheInfoOut->objTypeInx = objTypeInx;
            myCacheInfoOut->replNum = compInfoOut->replNum;
            myCacheInfoOut->intReplNum = intReplNum;
            myCacheInfoOut->offset = compInfoOut->offset;
            myCacheInfoOut->size = compInfoOut->size;
            myCacheInfoOut->isDirty = 0;
            myCacheInfoOut->phyResClass = strdup (tmpCompResInfo->phyResClass);
            myCacheInfoOut->resClassInx =
              resClassLookup (myCacheInfoOut->phyResClass);
            if (compInfoOut->containerName != NULL) {
                myCacheInfoOut->containerName = 
		  strdup (compInfoOut->containerName);
	    } else {
                myCacheInfoOut->containerName = NULL;
            }
            myCacheInfoOut->containerMaxSize = compInfoOut->containerMaxSize;
            myCacheInfoOut->tapeFileInfo = NULL;
            myCacheInfoOut->compoundLink = NULL;
            myCacheInfoOut->next = NULL;

            *outCacheInfoOut = cacheInfoOut;
            retVal = 0;
	}
    } else if (bytesWritten >= 0) {
	 retVal = OBJ_ERR_COPY_LEN;
            _objUnlink (srbObjTypeEntry[objTypeInx].objType,
              srbObjTypeEntry[objTypeInx].systemType,
              tmpCompResInfo->phyResLoc, fullPathName, NO_CHKDIR);
    } else {
	retVal = bytesWritten;
    }

    if (conn != NULL)
        clFinish (conn);    /* disconnect */

    freeAllInfoOut (tapeInfoOut);

    return retVal;
}

#ifdef TAPE_DRIVE
srb_long_t
_svrStageFromTape (srbConn *conn, 
int exfInx, int cacheObjTypeInx, 
struct mdasInfoOut *tapeInfoOut, tapeDev_t *tapeDev)
{
    struct mdasInfoOut *tmpTapeInfo;
    tapeCart_t myTapeCart;
    char *myDevPath;
    tapeFileInfo_t *tmpTapeFileInfo, *prevTapeFileInfo;
    srb_long_t bytesWritten, offset, segOut;
    int bufLen;
    int bytesRead, bytesOut;
    int intReplNum;
    char *buf = NULL;
    int status, retVal;

    /* connect to the tapeLib server */

    if (conn == NULL) {
        conn = svrConnect ((tapeInfoOut)->tapeFileInfo->tapeLibConfig->host,
          (tapeInfoOut)->tapeFileInfo->tapeLibConfig->port, NULL, 1, NULL);


        if ((retVal = clStatus(conn)) != CLI_CONNECTION_OK) {
            elog (NOTICE, "_svrStageCompObj: svrConnect failed. %s",
             myTapeCart.tapeNum);
            return clStatus(conn);
        }
    }

    offset = 0;
    bytesWritten = 0;
    buf = NULL;
    bufLen = 0;


    prevTapeFileInfo = NULL;
    tmpTapeInfo = tapeInfoOut;
    while (tmpTapeInfo != NULL) {

        if (bytesWritten == tmpTapeInfo->size)  /* done */
            break;

        tmpTapeFileInfo = tmpTapeInfo->tapeFileInfo;
        if (offset != 0) {
            _objSeek (exfInx, cacheObjTypeInx, 0, SEEK_SET);
            offset = 0;
        }

        bytesWritten = 0;
        while (tmpTapeFileInfo != NULL) {
            if (prevTapeFileInfo == NULL ||     /* first time */
             strcmp (prevTapeFileInfo->tapeNum,tmpTapeFileInfo->tapeNum) != 0) {
                if ((tapeDev->state & ST_TLIB_MOUNTED) > 0) {
                    /* close and unmount previously mounted tape */
                    swCloseTape (tapeDev->drvInx, tapeDev, 0);
                    status = srbTapelibDismntCart (conn,
                      tapeDev->mountedCart->tapeNum);
                    if (status < 0) {
                        elog (NOTICE,
                          "_svrStageCompObj: Error dismounting %s status = %d",
                          tapeDev->mountedCart->tapeNum, status);
                    }
                    tapeDev->state = tapeDev->state & (~ST_TLIB_MOUNTED);
                }

                /* mount the tape */
                retVal = srbTapelibMntCart (conn, tmpTapeFileInfo->tapeNum, 1,
                 &myDevPath);
                if (retVal < 0) {
                    elog (NOTICE,
                     "_svrStageCompObj: error mounting %s, status = %d",
                      tmpTapeFileInfo->tapeNum, retVal);
                    break;
                }
                /* tapeDev.state = tapeDev.state  | ST_MOUNTED; */

                retVal = initTapeDev (tapeDev, myDevPath,
                  tmpTapeFileInfo->tapeNum);

                free (myDevPath);

                if (retVal < 0) {
                    retVal = TAPLIB_INTERNAL_ERROR;
                    break;
                }
                retVal = srbOpenTapeForRead (tapeDev,
                  &tmpTapeFileInfo->tapePos);
                if (retVal < 0) {
                    elog (NOTICE,
                     "_svrStageCompObj:srbOpenTapeForRead of %s failed,stat=%d",
                      tmpTapeFileInfo->tapeNum, retVal);
                    break;
                }

                bufLen = initTapeBuf (tapeDev->drvTypeInx, bufLen, &buf);
                if (bufLen < 0) {
                    elog (NOTICE,
                     "_svrStageCompObj:initTapeBuf of drvInx %d failed,stat=%d",
                      tapeDev->drvInx, retVal);
                    retVal = TAPE_CONFIG_ERROR;
                    break;
                }
            } else {    /* tape already mounted and opened */
                retVal = swSeekTape (tapeDev->drvInx, tapeDev,
                  &tmpTapeFileInfo->tapePos);
                if (retVal < 0) {
                    elog (NOTICE,
                      "_svrStageCompObj: swSeekTape of %s failed, status=%d\n",
                      retVal);
                    break;
                }
                retVal = readVerifyHdr (tapeDev);
                if (retVal < 0) {
                    elog (NOTICE,
                      "_svrStageCompObj:readVerifyHdr of %s failed, status=%d",
                      tmpTapeFileInfo->tapeNum, retVal);
                    break;
                }
            }
            /* stage the tape segment */

            if (offset != tmpTapeFileInfo->offset) {
                _objSeek (exfInx, cacheObjTypeInx, tmpTapeFileInfo->offset,
                  SEEK_SET);
                offset = tmpTapeFileInfo->offset;
            }

            segOut = 0;
            while ((bytesRead = srbReadTape (tapeDev, buf, bufLen)) > 0) {

                bytesOut = _objWrite (exfInx, cacheObjTypeInx, buf, bytesRead);
                if (bytesOut != bytesRead) {
                    elog (NOTICE,
                      "_svrStageCompObj:bytesOut %d not equal to bytesRead %d\n"
,
                      bytesOut, bytesRead);
                    retVal = OBJ_ERR_COPY_LEN;
                    break;
                }
                segOut += bytesOut;
            }
            if (bytesRead < EOF) {
                retVal = bytesRead;
                break;
            }

            if (retVal < 0)
                break;

            if (segOut != tmpTapeFileInfo->size) {
                elog (NOTICE,
                 "_svrStageCompObj:Inconsistent tape sz for %s intSegNum %d",
                  tapeInfoOut->objID, tmpTapeFileInfo->intSegNum);
                retVal = OBJ_ERR_COPY_LEN;
                break;
            }
            bytesWritten += segOut;
            offset += segOut;
            prevTapeFileInfo = tmpTapeFileInfo;
            tmpTapeFileInfo = tmpTapeFileInfo->next;
        }
        tmpTapeInfo = tmpTapeInfo->next;
    }
    if ((tapeDev->state & ST_TLIB_MOUNTED) > 0) {
        /* close and unmount previously mounted tape */
        swCloseTape (tapeDev->drvInx, tapeDev, 0);
         status = srbTapelibDismntCart (conn,
          tapeDev->mountedCart->tapeNum);
        if (status < 0) {
            elog (NOTICE,
              "_svrStageCompObj: Error dismounting %s status = %d",
              tapeDev->mountedCart->tapeNum, status);
        }
        tapeDev->state = tapeDev->state & (~ST_TLIB_MOUNTED);
    }

    if (tapeDev->mountedCart != NULL)
        free (tapeDev->mountedCart);

    if (bytesWritten == (tapeInfoOut)->size) {
        return (bytesWritten);
    } else {    /* error */
        if (retVal < START_OF_ERR_CODE)
            return retVal;
        else
            return TAPLIB_INTERNAL_ERROR;
    }
}

int
initTapeDev (tapeDev_t *tapeDev, char *myDevPath, char *tapeNum)
{
    memset (tapeDev, 0, sizeof (tapeDev_t));

    if (initDrvByDevPath (myDevPath, tapeDev) < 0) {
	elog (NOTICE,
         "initTapeDev: initDrvByDevPath() of %s failed",
         myDevPath);
        return (TAPLIB_INTERNAL_ERROR);
    }

    if (tapeDev->mountedCart == NULL)
	tapeDev->mountedCart = malloc (sizeof (tapeCart_t));

    memset (tapeDev->mountedCart, 0, sizeof (tapeCart_t));

    memcpy (&tapeDev->mountedCart->tapeNum, tapeNum, TAPE_NUM_LEN);
    tapeDev->state = ST_CLOSED;

    return 0;
}
#endif	/* TAPE_DRIVE */

int
svrRegInternalCompObj (struct varlena *vObjName, struct varlena *vObjCollName, 
int   objReplNum, int objSegNum, 
struct varlena *vIntObjRescName, struct varlena *vDataPathName, 
srb_long_t *dataSize, srb_long_t *offset, 
int inpIntReplNum, int intSegNum,
int objTypeInx, struct varlena *vPhyResLoc)
{
    char *objName; 
    char *objCollName;
    char *intObjRescName;
    char *dataPathName;
    char *phyResLoc;
    int retVal;

    objName = varToStr (vObjName);
    objCollName = varToStr (vObjCollName);
    intObjRescName = varToStr (vIntObjRescName);
    dataPathName = varToStr (vDataPathName);
    phyResLoc = varToStr (vPhyResLoc);

    retVal = _svrRegInternalCompObj (objName, objCollName, 
      objReplNum, objSegNum, intObjRescName, dataPathName,
      *dataSize, *offset, inpIntReplNum, intSegNum,
      objTypeInx, phyResLoc,
      ClientUser->userName, ClientUser->domainName);

    free (objName);
    free (objCollName);
    free (intObjRescName);
    free (dataPathName);
    free (phyResLoc);

    return retVal;
}

/* _svrRegInternalCompObj - If objTypeInx >= 0, remove the obj if
 * it failed.
 */

int
_svrRegInternalCompObj (char *objName, char *objCollName, int   objReplNum,
int   objSegNum, char *intObjRescName, char *dataPathName, srb_long_t dataSize,
srb_long_t offset, int inpIntReplNum, int intSegNum, 
int objTypeInx, char *phyResLoc,
char *userName, char *domainName)
{
    int intReplNum;
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    remoteFlag = getMdasEnabledHost (objCollName, &hostTabPtr);

    if (remoteFlag < 0) {
        intReplNum = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        intReplNum = registerInternalCompoundObject (NULL,
         objName, objCollName, objReplNum, objSegNum, intObjRescName,
         dataPathName, dataSize, offset, inpIntReplNum, intSegNum,
          userName, domainName);
	if (intReplNum >= 0)
	    commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }

        intReplNum = srbRegInternalCompObj (hostTabPtr->conn,
         objName, objCollName, objReplNum, objSegNum, intObjRescName,
         dataPathName, dataSize, offset, inpIntReplNum, intSegNum,
	  objTypeInx, phyResLoc);
    }

    if (intReplNum < 0) {
        elog (NOTICE,
          "_svrRegInternalCompObj: Unable to reg intComp Object %s,status=%d", 
	  objName, intReplNum);
	if (objTypeInx >= 0 && phyResLoc != NULL) {
            /* unregister the compound obj */
            retVal = _svrRmCompObj (
              objName,
              objCollName,
              objReplNum,
              objSegNum,
              ClientUser->userName, ClientUser->domainName);

            /* Unlink the data object created */
            _objUnlink (srbObjTypeEntry[objTypeInx].objType,
              srbObjTypeEntry[objTypeInx].systemType,
              phyResLoc, dataPathName, NO_CHKDIR);
	}
    }
    return (intReplNum);
}

int
rmAllIntTapeCompObj (struct mdasInfoOut *tapeInfoOut, char *collectionName,
char *tapeUser, char *tapeDomain)

{
#ifdef TAPE_DRIVE
    tapeFileInfo_t *tmpTapeFileInfo;
#endif
    int retVal;
    
    if (tapeInfoOut == NULL)
	return 0;

    if (srbObjTypeEntry [tapeInfoOut->objTypeInx].objType != TAPE_TYPE) {
	retVal = rmIntNonTapeCompObj (tapeInfoOut, collectionName,
	 tapeUser, tapeDomain);
	return retVal;
    }
    
#ifdef TAPE_DRIVE
    tmpTapeFileInfo = tapeInfoOut->tapeFileInfo;
    while (tmpTapeFileInfo != NULL) {
        retVal = _svrRmInternalCompObj (
          tapeInfoOut->objID,
	  collectionName,
          tapeInfoOut->replNum,
          NO_INT_SEGMENTATION,
          tmpTapeFileInfo->intReplNum,
          tmpTapeFileInfo->intSegNum,
          tapeUser, tapeDomain);
        if (retVal < 0) {
            elog (NOTICE,
             "rmAllIntTapeCompObj: rm tape compound obj %s error, status = %d",
             tapeInfoOut->objID, retVal);
        }
        tmpTapeFileInfo = tmpTapeFileInfo->next;
    }
    return (0);
#else
    return 0;
#endif	/* TAPE_DRIVE */
}

int
rmIntNonTapeCompObj (struct mdasInfoOut *cacheInfoOut, char *collectionName,
char *tapeUser, char *tapeDomain)

{
    int retVal;
    int tinx;

    tinx = cacheInfoOut->objTypeInx;
    retVal = _objUnlink (srbObjTypeEntry[tinx].objType,
      srbObjTypeEntry[tinx].systemType,
      cacheInfoOut->resourceLoc, cacheInfoOut->dataPath, NO_CHKDIR);
    if (retVal < 0) {
        elog (NOTICE,
         "rmIntNonTapeCompObj: _objUnlink of %s error, status = %d",
         cacheInfoOut->dataPath, retVal);
    }

    retVal = _svrRmInternalCompObj (
      cacheInfoOut->objID,
      collectionName,
      cacheInfoOut->replNum,
      cacheInfoOut->segNum,
      cacheInfoOut->intReplNum,
      cacheInfoOut->intSegNum,
      tapeUser, tapeDomain);
    if (retVal < 0) {
        elog (NOTICE,
         "rmIntNonTapeCompObj: rm cache compound obj %s error, status = %d",
         cacheInfoOut->objID, retVal);
    }
    return (0);
}

/* resolveCompInfo - If it is a compound object, stage it to cache and return
 * the mdasInfoOut of the cache copy. Otherwise return the same infoOut. 
 */
struct mdasInfoOut *
resolveCompInfo (struct mdasInfoOut *infoOut, char *collection, int stageFlag)
{
    struct mdasInfoOut *myInfoOut;
    int retVal;

    myInfoOut = NULL;
    if (srbObjTypeEntry[infoOut->objTypeInx].objType == COMPOUND_TYPE) {
        /* a compound object */
        retVal = _svrStageCompObj (infoOut, collection, &myInfoOut,
	  stageFlag);
	if (retVal < 0) {
	    elog (NOTICE, "resolveCompInfo: _svrStageCompObj failed. stst=%d",
	     retVal);
	}
    } else {
        myInfoOut = infoOut;
    }
    return (myInfoOut);
}

int
svrDumpFileList (int tapeLibInx, struct varlena *packedResult, int purgeFlag)
{
    fileList_t *fileList;
    int retVal;

    if (ClientUser->privUserFlag <= 0) {
        elog (NOTICE, 
	  "svrDumpFileList: User has no privilege for this operation");
        return (AUTH_ERR_PROXY_NOPRIV);
    }

    retVal = unpackMsg (VAROUTDATA (packedResult), (char **) &fileList,
     fileList_t_PF, NULL);

    if (retVal < 0) {
        elog(NOTICE,
         "svrDumpFileList: unpackMsg() of myresult error, status = %d",
          retVal);
        return (retVal);
    }

    retVal = _svrDumpFileList (tapeLibInx, fileList, purgeFlag);

    if (fileList->fileName != NULL)
	free (fileList->fileName);

    free (fileList);

    return (retVal);
}


int
_svrDumpFileList (int tapeLibInx, fileList_t *fileList, int purgeFlag)
{
#ifdef TAPE_DRIVE
    tapeStat_t tapeStat;
    tapeDev_t tapeDev;
    tapeLibConfig_t *tapeLibConfig = NULL;
#endif
    char *fileName;
    int i, retVal;
    int remoteFlag;
#ifdef TAPE_DRIVE
    int lastFileFlag;
    srbConn *conn = NULL;
#endif
    char *portNum;
    struct hostElement  *hostTabPtr;
    char collection[MAX_TOKEN];
    char compResc[MAX_TOKEN];
    char compRescLoc[MAX_TOKEN];
    struct mdasResInfo *compResInfo, *tmpCompResInfo;
    char tapeUser[MAX_TOKEN], tapeDomain[MAX_TOKEN];
    struct mdasResInfo *tapeResInfo = NULL;
    struct mdasInfoOut *cacheInfoOut = NULL;
    struct mdasInfoOut *tapeInfoOut = NULL;
    int successCnt = 0;
    int lockFd = -1;
    struct resCache *myCacheRes;

    if (fileList == NULL)
	return 0;

#ifdef TAPE_DRIVE
    memset (&tapeDev, 0, sizeof (tapeDev_t));
#endif
	  
    fileName = (char *) fileList->fileName;
    for (i = 0; i < fileList->numFile; i++) {
	retVal = _svrGetCompInfoByFilePath (&fileName[i*MAX_DATA_SIZE], 
	  &cacheInfoOut, &tapeInfoOut, compResc, compRescLoc, collection,
	  tapeUser, tapeDomain);
	if (retVal < 0) {
	    elog(NOTICE,
	      "_svrDumpFileList:_svrGetCompInfo of %s failed,stat=%d",
	      &fileName[i*MAX_DATA_SIZE], retVal);
	    continue;
	}
	portNum = parseAddr (compRescLoc);
        remoteFlag = chkHost (compRescLoc, portNum, &hostTabPtr);
        if (remoteFlag < 0) {
            elog (NOTICE, "Invalid host addr %s of %s", 
	      compRescLoc, &fileName[i*MAX_DATA_SIZE]);
            freeAllInfoOut (cacheInfoOut);
            freeAllInfoOut (tapeInfoOut);
	    continue;
        } else if (remoteFlag == REMOTE_HOST) {
#ifdef TAPE_DRIVE
	    if (conn != NULL) {
	        procCloseTapeDev (&tapeDev, conn);
	        clFinish (conn);    /* disconnect */
	    }
#endif
            freeAllInfoOut (cacheInfoOut);
            freeAllInfoOut (tapeInfoOut);
	    if (successCnt > 0) {
		elog (NOTICE, 
		  "_svrDumpFileList: tape resource loc err for %s",
		  &fileName[i*MAX_DATA_SIZE]);
		return (INP_ERR_HOST_ADDR);
	    }
            if ((retVal = remoteConnect (hostTabPtr)) < 0) {
                return retVal;
            }
	    /* do a remote dump */
            retVal = srbDumpFileList (hostTabPtr->conn,
	      tapeLibInx, fileList, purgeFlag);
	
            return retVal;
        }
	/* Dump the file locally */

#ifdef TAPE_DRIVE
        procTapeInfo (&tapeInfoOut);
#endif

	if (cacheInfoOut == NULL) {
	    /* this should not happen */
	    elog (NOTICE, "_svrDumpFileList: no cache copy  for %s",
	    &fileName[i*MAX_DATA_SIZE]);
            freeAllInfoOut (tapeInfoOut);
            continue;
	}
	
	if (tapeInfoOut != NULL) {	/* already have a tape copy */
	    if (cacheInfoOut->isDirty == 0) {
		/* both cache and tape copies are good */
		/* some sanity check */
		if (cacheInfoOut->size == tapeInfoOut->size) {
		    /* Everything is fine */
                    if (purgeFlag > 0) {
                        retVal = rmIntNonTapeCompObj (cacheInfoOut, collection,
			 tapeUser, tapeDomain);
                        if (retVal < 0) {
                            elog (NOTICE, 
                              "_svrDumpFile:rmIntCacheComp of %s error,stat=%d",
                              &fileName[i*MAX_DATA_SIZE], retVal);
                        }
                    }

		    freeAllInfoOut (cacheInfoOut);
                    freeAllInfoOut (tapeInfoOut);
                    continue;
		} else {
		    /* something is wrong here */
		    elog (NOTICE, 
		      "_svrDumpFileList: %s cache/tape copies size mismatch",
		      &fileName[i*MAX_DATA_SIZE]); 
		    if (cacheInfoOut->size > tapeInfoOut->size) {
			/* assume cache copy is good */
			
                        retVal = rmAllIntTapeCompObj (tapeInfoOut, collection,
			 tapeUser, tapeDomain);
                        if (retVal < 0) {
                            /* something is very wrong here */
                            elog (NOTICE,
                            "_svrDumpFileList:rmAllIntTapeCompObj %s error,stat=%d",
                             &fileName[i*MAX_DATA_SIZE], retVal);
                            freeAllInfoOut (tapeInfoOut);
                            freeAllInfoOut (cacheInfoOut);
			    continue;
		        } else {
			    freeAllInfoOut (tapeInfoOut);
			    tapeInfoOut = NULL;
			}
		    } else {
                        freeAllInfoOut (tapeInfoOut);
                        freeAllInfoOut (cacheInfoOut);
                        continue;
		    }
		}
	    } else {	/* cache is dirty */
	        retVal = rmAllIntTapeCompObj (tapeInfoOut, collection, 
		  tapeUser, tapeDomain);
                if (retVal < 0) {
                    /* something is very wrong here */
		    elog (NOTICE,
		    "_svrDumpFileList:rmAllIntTapeCompObj of %s error,stat=%d",
		     &fileName[i*MAX_DATA_SIZE], retVal);
                    freeAllInfoOut (tapeInfoOut);
                    freeAllInfoOut (cacheInfoOut);
                    continue;
                }
                freeAllInfoOut (tapeInfoOut);
	    }
	}


#ifdef TAPE_DRIVE
	if (i >= fileList->numFile - 1)
	    lastFileFlag = 1;
	else
	    lastFileFlag = 0;
#endif

        retVal = getCompoundResInfo (collection, MDAS_CATALOG, compResc, 
	 &myCacheRes);

        if (retVal < 0) {
            elog (NOTICE, 
	      "_svrDumpFileList:getCompoundResInfo of %s error, status=%d",
              compResc, retVal);
            return retVal;
        }

	compResInfo = myCacheRes->resInfoHead;

        /* find the tape resource */
        tmpCompResInfo = compResInfo;
        while (tmpCompResInfo != NULL) {
            int resClassInx;
            resClassInx = resClassLookup (tmpCompResInfo->phyResClass);
            /* XXXXif (strcmp (tmpCompResInfo->phyResType, TAPE_RESC) == 0) { */
            if (resClassEntries[resClassInx].permanent == 1) {
                tapeResInfo = tmpCompResInfo;
                break;
            }
            tmpCompResInfo = tmpCompResInfo->next;
        }

        if (tapeResInfo == NULL) {
            elog (NOTICE,
              "_svrDumpFileList: compResc %s has no tape resource", compResc);
            return (TAPLIB_NO_TAPE_RESC);
        }

	if (strcmp (tapeResInfo->phyResType, TAPE_RESC) == 0) {
#ifdef TAPE_DRIVE
	    /* Tape resource */

#ifdef LOCK_OBJ
            lockFd =_svrObjLock (cacheInfoOut->objID, collection,
              F_SETLKW, F_RDLCK, 0);
#endif

            /* Have to do the tapeLibConfig here because remote host may
             * not have the tapeLib configured
             */
            if (tapeLibConfig == NULL) {
                tapeLibConfig = matchTapeLibByInx (tapeLibInx);
                if (tapeLibConfig == NULL) {
                    elog (NOTICE,
                      "_svrDumpFileList: No match for input tapeLibInx %d",
                    tapeLibInx);
                    return (TAPLIB_INX_ERROR);
                }
            }

            inFd = lowLevelOpen (cacheInfoOut->objTypeInx, 
	      cacheInfoOut->dataPath, cacheInfoOut->resourceLoc, O_RDONLY);

            if (inFd < 0) {
#ifdef LOCK_OBJ
                if (lockFd >= 0) {
                    _svrObjLock (cacheInfoOut->objID, collection,
                      F_SETLKW, F_UNLCK, lockFd);
		    lockFd = -1;
		}
#endif
                elog (NOTICE,
                  "_svrDumpFileList: Unable to open cache file %s. stat=%d",
                  cacheInfoOut->dataPath, inFd);
                return (inFd);
            }

	    retVal = _svrDumpFileToTape (&conn, inFd, 
	      &fileName[i*MAX_DATA_SIZE], collection,
	      tapeResInfo, cacheInfoOut, tapeLibConfig, 
	      &tapeDev, &tapeStat, lastFileFlag, tapeUser, tapeDomain);

            _objClose (cacheInfoOut->objTypeInx, inFd);
#else
	    retVal = SYS_ERR_NO_SUPPORT;
#endif
	} else {
	    /* Other archival */
	    srb_long_t bytesWritten;
	    int objTypeInx;
	    char fullPathName[MAX_TOKEN];

	    bytesWritten = _svrDataCopyFromInfoToResc (MDAS_CATALOG,
              cacheInfoOut->objID, collection, cacheInfoOut,
              tapeResInfo, "", fullPathName, &objTypeInx);
	    if (bytesWritten == cacheInfoOut->size) {
                retVal = _svrRegInternalCompObj (
                  cacheInfoOut->objID, collection, cacheInfoOut->replNum,
                  cacheInfoOut->segNum, tapeResInfo->phyResName, fullPathName,
                  cacheInfoOut->size, 0,
                  NEW_INT_REPL_NUM, NO_INT_SEGMENTATION,
                  objTypeInx, tapeResInfo->phyResLoc,
                  tapeUser, tapeDomain);
                if (retVal < 0) {
                    _objUnlink (srbObjTypeEntry[objTypeInx].objType,
                      srbObjTypeEntry[objTypeInx].systemType,
                      tapeResInfo->phyResLoc, fullPathName, NO_CHKDIR);
                }
	    } else if (bytesWritten >= 0) {
		retVal = OBJ_ERR_COPY_LEN;
                _objUnlink (srbObjTypeEntry[objTypeInx].objType,
                  srbObjTypeEntry[objTypeInx].systemType,
                  tapeResInfo->phyResLoc, fullPathName, NO_CHKDIR);
	    } else {
		retVal = bytesWritten;
	    }
	}


	if (retVal < 0) {
#ifdef LOCK_OBJ
            if (lockFd >= 0) {
                _svrObjLock (cacheInfoOut->objID, collection,
                  F_SETLKW, F_UNLCK, lockFd);
                lockFd = -1;
            }
#endif
	    elog (NOTICE, "_svrDumpFileList: _svrDumpFile of %s error, stat=%d",
	    &fileName[i*MAX_DATA_SIZE], retVal);
            freeAllInfoOut (cacheInfoOut);
	    continue;
	} else {
	    successCnt ++;
	    /* dump success */
	    if (purgeFlag > 0) {
		retVal = rmIntNonTapeCompObj (cacheInfoOut, collection, 
		 tapeUser, tapeDomain);
		if (retVal < 0) {
		    elog (NOTICE, 
		     "_svrDumpFileList:rmIntNonTapeCompObj of %s error,stat=%d", 
		      &fileName[i*MAX_DATA_SIZE], retVal);
		}
            } else {
		/* mark it not dirty */
                retVal = _svrModInternalCompObj (
                  cacheInfoOut->objID, collection,
                  cacheInfoOut->replNum, cacheInfoOut->segNum,
                  cacheInfoOut->intReplNum, cacheInfoOut->intSegNum,
                  tapeUser, tapeDomain,
                  "0", NULL, NULL, NULL,
                  D_COMPOUND_MODIFY_IS_DIRTY);
                if (retVal < 0) {
                    elog (NOTICE,
                    "_svrDumpFileList:_svrModIntCompObj of %s error,stat=%d",
                      &fileName[i*MAX_DATA_SIZE], retVal);
                }

	    }
#ifdef LOCK_OBJ
            if (lockFd >= 0) {
                _svrObjLock (cacheInfoOut->objID, collection,
                  F_SETLKW, F_UNLCK, lockFd);
                lockFd = -1;
            }
#endif
            freeAllInfoOut (cacheInfoOut);
	}
    }
#ifdef TAPE_DRIVE
    if (conn != NULL) {
        procCloseTapeDev (&tapeDev, conn);
        clFinish (conn);    /* disconnect */
    }
#endif
    return (retVal);
}

#ifdef TAPE_DRIVE
int
_svrDumpFileToTape (srbConn **conn, int inFd, char *fileName, char *collection, 
struct mdasResInfo *tapeResInfo, struct mdasInfoOut *cacheInfoOut, 
tapeLibConfig_t *tapeLibConfig, 
tapeDev_t *tapeDev, tapeStat_t *tapeStat, int lastFileFlag,
char *tapeUser, char *tapeDomain)
{
    int retVal;
    tapePos_t destPos;
    srb_long_t byteWritten;
    int byteRead;
    char pathName[MAX_TOKEN];
    srb_long_t totalByteWritten = 0;
    srb_long_t seqByteWritten;
    int bufLen = 0;
    char *buf = NULL;
    int intReplNum = NEW_INT_REPL_NUM;
    int fullTapeFlag = 0;
    int retryCnt = 0;

    if (*conn == NULL) {
	*conn = svrConnect (tapeLibConfig->host, tapeLibConfig->port, 
	  NULL, 1, NULL);
        if ((retVal = clStatus(*conn)) != CLI_CONNECTION_OK) {
            elog (NOTICE, "_svrDumpFile: svrConnect to %s failed: %s",
             tapeLibConfig->host, clErrorMessage(*conn));
	    retVal = clStatus (*conn);
	    freeSrbConn (*conn);
	    *conn = NULL;
            return (retVal);
        }
    }

    while (cacheInfoOut->size > totalByteWritten) {

        if ((tapeDev->state & ST_TLIB_MOUNTED) == 0) { 
	    /* no tape mounted */
            memset (tapeStat, 0, sizeof (tapeStat_t));
	    /* XXXXXX _svrGetTapeToWrite need to be changed */
            retVal = _svrGetTapeToWrite (*conn, tapeLibConfig->inx, tapeStat);

            if (retVal < 0) {
                elog (NOTICE, 
		  "_svrDumpFile: srbGetTapeTowrite failed: %s, stat=%d",
                 clErrorMessage(*conn), retVal);
		return retVal;
	    }

	    /* tapeDev->state = tapeDev->state  | ST_MOUNTED; */

            retVal = initTapeDev (tapeDev, tapeStat->mntDevPath,
              tapeStat->tapeNum);

            if (retVal < 0) {
                retVal = TAPLIB_INTERNAL_ERROR;
	        return (retVal);
            }

            bufLen = initTapeBuf (tapeDev->drvTypeInx, bufLen, &buf);
            if (bufLen < 0) {
                elog (NOTICE,
                  "_svrDumpFile:initTapeBuf of drvInx %d failed,stat=%d\n",
                     tapeDev->drvInx, retVal);
                return bufLen;
            }

	    /* set the postion of the tape to eot */
	    /* tapeDev->position.filePos.fileSeqNo = tapeStat->eofSeqNo; */
            retVal = srbOpenTapeForWrite (tapeDev, &tapeStat->position,
	     cacheInfoOut->objID);
            if (retVal < 0) {
                elog (NOTICE,
                  "_svrDumpFile:srbOpenTapeForWrite of %s failed,stat=%d",
                  tapeStat->tapeNum, retVal);
		/* possible the failure is caused by full tape */
                if (retryCnt <= 0) {
                    elog (NOTICE,
                      "_svrDumpFile: end of tape reached for tape %s",
                      tapeStat->tapeNum);
                    /* Inform the catalog that the tape is full */
                    retVal = regTapeStat (tapeStat, 1, 1);
                    if (retVal < 0) {
                        if (buf != NULL)
                            free (buf);
                        return (retVal);
                    }

                    /* dismount the full tape */
                    procCloseTapeDev (tapeDev, *conn);
                    retryCnt ++;
                    continue;
                } else {
		    if (buf != NULL)
                        free (buf);
	            return (retVal);
		}
            }

        } else {
	    tapePos_t destPos;

            retVal = writeHeader (tapeDev, cacheInfoOut->objID);
            if (retVal < 0) {
		/* Sometime writeHeader got unknown error. Retry one time */
                elog (NOTICE,
	          "_svrDumpFile:writeHeader failed, errno=%d", errno);
		/* save the tape position */
		destPos = tapeDev->position;
		swCloseTape (tapeDev->drvInx, tapeDev, 0);
		retVal = srbOpenTapeForWrite (tapeDev, &destPos, 
		  cacheInfoOut->objID);
                if (retVal < 0) {
                    elog (NOTICE,
                      "_svrDumpFile:srbOpenTapeForWrite of %s failed,stat=%d",
                      tapeStat->tapeNum, retVal);

		    /* possible the failure is caused by full tape */
		    if (retryCnt <= 0) {
                        elog (NOTICE,
                          "_svrDumpFile: end of tape reached for tape %s",
                          tapeStat->tapeNum);
                        /* Inform the catalog that the tape is full */
                        retVal = regTapeStat (tapeStat, 1, 1);
                        if (retVal < 0) {
                            if (buf != NULL)
                                free (buf);
                            return (retVal);
                        }

                        /* dismount the full tape */
                        procCloseTapeDev (tapeDev, *conn);
			retryCnt ++;
			continue;
		    } else {
                        if (buf != NULL)
                            free (buf);
                        return (retVal);
		    }
		}
            }
	    retryCnt = 0;	/* success - reset retryCnt */
	    if (buf == NULL) {
                bufLen = initTapeBuf (tapeDev->drvTypeInx, bufLen, &buf);
                if (bufLen < 0) {
                    elog (NOTICE,
                      "_svrDumpFile:initTapeBuf of drvInx %d failed,stat=%d\n",
                         tapeDev->drvInx, retVal);
                    return bufLen;
		}
            }

        }

	seqByteWritten = 0;
        while ((byteRead = _objRead (inFd, cacheInfoOut->objTypeInx, 
	  buf, bufLen)) > 0) {
            byteWritten = srbWriteTape (tapeDev, buf, byteRead);
            if (byteWritten != byteRead) {
		int myErrno;

		myErrno = errno;
#if defined(PORTNAME_solaris)
		/* with solaris, eot does not produce any error (errno ==0),
		 * Just the byteWritten != byteRead
		 */
		if (byteWritten >= 0 && retryCnt <= 0) {
#else
                /* XXXXXXX ENXIO is the end of tape errno for AIX */
                if (myErrno == ENXIO && retryCnt <= 0) {
#endif
		    retryCnt ++;
                    elog (NOTICE,
                      "_svrDumpFile: end of tape reached for tape %s",
		      tapeStat->tapeNum);
		    /* Inform the catalog that the tape is full */
		    retVal = regTapeStat (tapeStat, 1, 1);
		    if (retVal < 0) {
			if (buf != NULL)
			    free (buf);
			return (retVal);
		    }

		    /* go back to the last good segment */
		    _objSeek (inFd, cacheInfoOut->objTypeInx, 
		      totalByteWritten, SEEK_SET);

		    seqByteWritten = 0;
		      
		    /* dismount the full tape */
		    procCloseTapeDev (tapeDev, *conn);

		    fullTapeFlag = 1;
		    break;
	        } else {
                    elog (NOTICE,
                    "_svrDumpFile:tape %s-byteWritten %d != read %d,errno=%d",
                     tapeStat->tapeNum, byteWritten, byteRead, errno);

                    /* dismount the full tape */
                    procCloseTapeDev (tapeDev, *conn);

                    if (buf != NULL)
                        free (buf);
		    if (byteWritten >= 0) 
			return OBJ_ERR_COPY_LEN;
		    else 
		        return (byteWritten);
		}
	    }

	    retryCnt = 0;	/* success - reset retryCnt */
	    seqByteWritten += byteWritten;
	    if (seqByteWritten >= TAPE_SEG_SZ)
		break;
        }

        if (fullTapeFlag > 0) {
            fullTapeFlag = 0;
            continue;
        }

	if (totalByteWritten + seqByteWritten == cacheInfoOut->size ||
	  seqByteWritten >= TAPE_SEG_SZ) {
            /* done writing data, write a EOF mark */
            retVal = swWriteEOF (tapeDev->drvInx, tapeDev);

            if (retVal < 0) {
                printf ("_svrDumpFiles: WriteEOF failed, errno=%d\n", errno);
                if (buf != NULL)
                    free (buf);
                return retVal;
            }
	} else {
	    elog (NOTICE, 
#if defined(PORTNAME_osx)
	      "_svrDumpFiles: tape write size error, want %qd, got %qd",
#else
	      "_svrDumpFiles: tape write size error, want %lld, got %lld",
#endif
	      cacheInfoOut->size, totalByteWritten + seqByteWritten);
            if (buf != NULL)
                free (buf);
	    return OBJ_ERR_COPY_LEN;
	}


	/* have to back off 1 on fileSeqNo because the file starts
	 * at the file mark one before the end */
	retVal = genTapePathName (pathName, tapeStat->tapeNum, 
	  tapeDev->position.filePos.fileSeqNo - 1);

	if (retVal < 0) {
            if (buf != NULL)
                free (buf);
	    return (retVal);
	}
	
	intReplNum = _svrRegInternalCompObj (cacheInfoOut->objID, collection, 
	  cacheInfoOut->replNum, cacheInfoOut->segNum, 
	  tapeResInfo->phyResName, pathName, 
	  seqByteWritten, totalByteWritten, intReplNum, NEW_INT_SEG_NUM,
          -1, NULL,
	  tapeUser, tapeDomain);
	  
        if (intReplNum < 0) {
            if (buf != NULL)
                free (buf);
            return (intReplNum);
        }

	totalByteWritten += seqByteWritten;
	tapeStat->eofBytesCnt += seqByteWritten;
	tapeStat->position.filePos.fileSeqNo = 
	  tapeDev->position.filePos.fileSeqNo;

	/* update the tape stat */

	if (totalByteWritten >= cacheInfoOut->size && lastFileFlag > 0)
	    /* done, release the tape too */
            retVal = regTapeStat (tapeStat, 1, 0);
	else
            retVal = regTapeStat (tapeStat, 0, 0);

        if (retVal < 0) {
            if (buf != NULL)
                free (buf);
            return (retVal);
        }
	
    }	/* While loop */



    if (buf != NULL)
        free (buf);

    return (0);
}

int
procCloseTapeDev (tapeDev_t *tapeDev, srbConn *conn)
{
    int retVal;

    if (tapeDev->state & ST_TLIB_MOUNTED != 0 && 
      tapeDev->mountedCart != NULL) 
        swCloseTape (tapeDev->drvInx, tapeDev, 0);

    tapeDev->state = 0;

    if (conn == NULL) {
	/* no connection */
        if (tapeDev->mountedCart != NULL) {
            free (tapeDev->mountedCart);
	    tapeDev->mountedCart = NULL;
	}
	return 0;
    }

    if (tapeDev != NULL && tapeDev && tapeDev->mountedCart != NULL) {
        retVal = srbTapelibDismntCart (conn, tapeDev->mountedCart->tapeNum);
    }

    if (tapeDev->mountedCart != NULL) {
        free (tapeDev->mountedCart);
	tapeDev->mountedCart = NULL;
    }

    return 0;
}

int
genTapePathName (char *pathName, char *tapeNum, int fileSeqNo)
{
    if (pathName == NULL || tapeNum == NULL) {
	elog (NOTICE, "genTapePathName: NULL input pointer");
	return (SYS_ERR_NULL_INPUT);
    }

    sprintf (pathName, "%s|%d", tapeNum, fileSeqNo);

    return (0);
}
    
int
parseTapePathName (char *pathName, char *tapeNum, int *fileSeqNo)
{
    char myTapeNum[MAX_TOKEN];
    char myFileSeqNo[MAX_TOKEN];

    if (pathName == NULL) {
	elog (NOTICE, "parseTapePathName: NULL pathName input");
	return (OBJ_ERR_BAD_PATHNAME);
    }

    memset (myTapeNum, 0, MAX_TOKEN);
    memset (myFileSeqNo, 0, MAX_TOKEN);

    if (strlen (pathName) > MAX_TOKEN - 1) {
        elog (NOTICE, "parseTapePathName: pathName %s too long", pathName);
        return (OBJ_ERR_BAD_PATHNAME);
    }

    mySplitStr (pathName, myTapeNum, myFileSeqNo, '|');
    if (strlen (myTapeNum) == 0 || (strlen (myTapeNum) > TAPE_TYPE_LEN ||
      strlen (myFileSeqNo) == 0)) {
        elog (NOTICE, "parseTapePathName: problem with pathName %s", pathName);
        return (OBJ_ERR_BAD_PATHNAME);
    }

    strcpy (tapeNum, myTapeNum);
    *fileSeqNo = atoi (myFileSeqNo);

    return (0);
}

int
_svrGetTapeToWrite (srbConn *conn, int inpTapeLibInx, tapeStat_t *tapeStat)
{
    int lockFd;
    int retVal;
    char *myDevPath;
    int i, k;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  *myresult = NULL;
    char *tapeNum;
    char *owner;
    char *tapeType;
    char *tapeLibInx;
    char *fullFlag;
    char *curFileSeqNum;
    char *curAbsPos;
    char *bytesWritten;
    tapeCartPri_t *outTapeCartPri;

    retVal = srbGetTapeCartPri (conn, &outTapeCartPri);
    if (retVal < 0) {
	elog (NOTICE, "_svrGetTapeToWrite: srbGetTapeCartPri error, stat=%d",
	  retVal);
	return retVal;
    }

    for (k = 0; k < NUM_DRV_TYPE; k++) {

        for (i = 0; i < MAX_DCS_NUM; i++) {
            selval[i] = 0;
            sprintf(qval[i],"");
        }

        selval[TAPE_NUMBER] = 1;
        selval[TAPE_OWNER] = 1;
        selval[TAPE_TYPE_VAL] = 1;
        selval[TAPE_LIBINX] = 1;
        selval[TAPE_FULLFLAG] = 1;
        selval[TAPE_CURRENT_FILE_SEQNUM] = 1;
        selval[TAPE_CURRENT_ABS_POSITION] = 1;
        selval[TAPE_BYTES_WRITTEN] = 1;

        /* get tape that are not full */
        sprintf(qval[TAPE_FULLFLAG], " = '0'");
        sprintf(qval[TAPE_TYPE_VAL], " = '%s'", 
	  getCartNameByCartTypeInx (outTapeCartPri->cartType[k]));

        retVal = queryDataInfo (NULL, selval, (char (*) [MAX_TOKEN]) qval, 
	 &myresult, NUM_ROWS_FOR_TAPE_QUERY);
        if (retVal < 0) {
	    continue;
        }

        tapeNum = (char *)getFromResultStruct(
              (mdasC_sql_result_struct *) myresult,
              dcs_tname[TAPE_NUMBER],
              dcs_aname[TAPE_NUMBER]);
        tapeLibInx = (char *)getFromResultStruct(
              (mdasC_sql_result_struct *) myresult,
              dcs_tname[TAPE_LIBINX],
              dcs_aname[TAPE_LIBINX]);
        curFileSeqNum = (char *)getFromResultStruct(
              (mdasC_sql_result_struct *) myresult,
              dcs_tname[TAPE_CURRENT_FILE_SEQNUM],
              dcs_aname[TAPE_CURRENT_FILE_SEQNUM]);
        bytesWritten = (char *)getFromResultStruct(
              (mdasC_sql_result_struct *) myresult,
              dcs_tname[TAPE_BYTES_WRITTEN],
              dcs_aname[TAPE_BYTES_WRITTEN]);

        for (i = 0; i < myresult->row_count; i++) {
    	    int myTapeLibInx;

	    if (inpTapeLibInx != atoi (&tapeLibInx[i*MAX_DATA_SIZE])) 
	        continue;

            tapeStat->lockFd = _svrObjLock (&tapeNum[i*MAX_DATA_SIZE], 
	      "tape", F_SETLK, F_WRLCK, 0);
            if (tapeStat->lockFd >= 0) {        /* success */
                /* Mount the tape */
                retVal = srbTapelibMntCart (conn,
                &tapeNum[i*MAX_DATA_SIZE], 1, &myDevPath);
                if (retVal < 0) {
                    elog (NOTICE,
                      "_svrGetTapeToWrite: Unable to mount tape %s, stat=%d",
                      &tapeNum[i*MAX_DATA_SIZE], retVal);
                    _svrObjLock (tapeStat->tapeNum, "tape", F_SETLK, F_UNLCK,
                    tapeStat->lockFd);
                    freeSqlResult (myresult);
		    free (outTapeCartPri->cartType);
		    free (outTapeCartPri);
                    return (retVal);
                }

                strcpy (tapeStat->tapeNum, &tapeNum[i*MAX_DATA_SIZE]);
                tapeStat->eofBytesCnt = 
	          strtoll (&bytesWritten[i*MAX_DATA_SIZE], 0, 0);
                tapeStat->position.filePos.fileSeqNo =
                  atoi (&curFileSeqNum[i*MAX_DATA_SIZE]);
                strcpy (tapeStat->mntDevPath, myDevPath);

                freeSqlResult (myresult);
		free (outTapeCartPri->cartType);
		free (outTapeCartPri);
                free (myDevPath);

	        return (0);
            }
        }
        freeSqlResult (myresult);
    }
    free (outTapeCartPri->cartType);
    free (outTapeCartPri);

    return (TAPE_OUT_OF_BLANK_TAPE);
}
	    
int 
freeAllTapeStat (tapeStat_t *topTapeStat)
{
    tapeStat_t *tmpTapeStat, *nextTapeStat;

    tmpTapeStat = topTapeStat;
	    
    while (tmpTapeStat != NULL) {
	nextTapeStat = tmpTapeStat->next;
	free (tmpTapeStat);
	tmpTapeStat = nextTapeStat;
    }
    return 0;
}

int
regTapeStat (tapeStat_t *tapeStat, int relTapeFlag, 
int fullFlag)
{
    int retVal;
    char *curAbsPos = "";
    char curFileSeqNum[MAX_TOKEN];
    char bytesWritten[MAX_TOKEN];
    char *cFullFlag;

    sprintf (curFileSeqNum, "%d", tapeStat->position.filePos.fileSeqNo);

#if defined(PORTNAME_osx)
    sprintf (bytesWritten, "%qd", tapeStat->eofBytesCnt);
#else
    sprintf (bytesWritten, "%lld", tapeStat->eofBytesCnt);
#endif
    if (fullFlag == 0) {
	cFullFlag = "0";
    } else {
	cFullFlag = "1";
    }
    retVal = _svrModifyRescInfo (MDAS_CATALOG, NULL,
      tapeStat->tapeNum, T_UPDATE_TAPE_INFO,
      cFullFlag,
      curFileSeqNum,
      curAbsPos,
      bytesWritten);

    if (relTapeFlag > 0) {
	_svrObjLock (tapeStat->tapeNum, "tape", F_SETLK, F_UNLCK, 
	tapeStat->lockFd);
    }

    return (retVal);
}
#endif	/* TAPE_DRIVE */

/* _svrGetCompInfoByFilePath - This is almost the same as 
 * getInternalReplicaInfoGivenPathOfOne.
 */
int
_svrGetCompInfoByFilePath (char *fileName, 
struct mdasInfoOut **cacheInfoOut, struct mdasInfoOut **tapeInfoOut, 
char *compResc, char *compRescLoc, char *myCollection, 
char *tapeUser, char *tapeDomain)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int i , selval[MAX_DCS_NUM];
    int retVal;
    char *objID, *cReplNum, *collection, *phyRescName, *resourceLoc,
    *cSegNum, *cIntReplNum, *cIntSegNum, *intPathName, *intRescName,
    *intRescLoc, *intRescType, *intIsDirty, *intRescClass, *cOffset,
    *cIntSize;
    char *userName, *domainName;
    mdasC_sql_result_struct *myresult = NULL;

    *cacheInfoOut = NULL;
    *tapeInfoOut = NULL;

    for (i = 0; i < MAX_DCS_NUM; i++)  {
      sprintf(qval[i],"");
      selval[i] = 0;
    }


    sprintf(qval[INT_PATH_NAME_FOR_REPL]," = '%s'",fileName);

    selval[DATA_CONT_NAME] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[COLLECTION_CONT_NAME] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[RSRC_ADDR_NETPREFIX] = 1;
    selval[DATA_SEG_NUM] = 1;
    selval[INT_REPL_ENUM] = 1;
    selval[INT_SEG_NUM] = 1;
    selval[INT_PATH_NAME] = 1;
    selval[INT_RSRC_NAME] = 1;
    selval[INT_RSRC_ADDR_NETPREFIX] = 1;
    selval[INT_RSRC_TYP_NAME] = 1;
    selval[INT_IS_DIRTY] = 1;
    selval[INT_RSRC_CLASS] = 1;
    selval[INT_OFFSET] = 1;
    selval[INT_SIZE] = 1;
    selval[USER_NAME] = 1;
    selval[DOMAIN_DESC] = 1;

  retVal = queryDataInfo (myCollection, selval, (char (*) [MAX_TOKEN]) qval, 
   &myresult, DEF_NUM_ROWS);

    if (retVal < 0)
	return retVal;
   
    userName = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[USER_NAME],
            dcs_aname[USER_NAME]);
    domainName = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DOMAIN_DESC],
            dcs_aname[DOMAIN_DESC]);

    strcpy (tapeUser, userName);
    strcpy (tapeDomain, domainName);

    objID = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_CONT_NAME],
            dcs_aname[DATA_CONT_NAME]);

    cReplNum = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_REPL_ENUM],
            dcs_aname[DATA_REPL_ENUM]);

    collection = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[COLLECTION_CONT_NAME],
            dcs_aname[COLLECTION_CONT_NAME]);

    phyRescName = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[PHY_RSRC_NAME],
            dcs_aname[PHY_RSRC_NAME]);

    resourceLoc = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[RSRC_ADDR_NETPREFIX],
            dcs_aname[RSRC_ADDR_NETPREFIX]);

    cSegNum = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_SEG_NUM],
            dcs_aname[DATA_SEG_NUM]);

    cIntReplNum = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_REPL_ENUM],
            dcs_aname[INT_REPL_ENUM]);

    cIntSegNum = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_SEG_NUM],
            dcs_aname[INT_SEG_NUM]);

    intPathName = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_PATH_NAME],
            dcs_aname[INT_PATH_NAME]);

    intRescName = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_RSRC_NAME],
            dcs_aname[INT_RSRC_NAME]);

    intRescLoc = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,dcs_tname[INT_RSRC_ADDR_NETPREFIX],
            dcs_aname[INT_RSRC_ADDR_NETPREFIX]);

    intRescType = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_RSRC_TYP_NAME],
            dcs_aname[INT_RSRC_TYP_NAME]);

    intIsDirty = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_IS_DIRTY],
            dcs_aname[INT_IS_DIRTY]);

    intRescClass = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_RSRC_CLASS],
            dcs_aname[INT_RSRC_CLASS]);

    cOffset = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_OFFSET],
            dcs_aname[INT_OFFSET]);

    cIntSize = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_SIZE],
            dcs_aname[INT_SIZE]);

    if (myresult->row_count <= 0)
	return (MCAT_INQUIRE_ERROR);

    strcpy (compResc, phyRescName);
    strcpy (compRescLoc, resourceLoc);
    strcpy (myCollection, collection);
    
    for (i = 0; i < myresult->row_count; i++) {
	int objTypeInx;
	struct mdasInfoOut *infoOut;

        objTypeInx = objTypeLookUp (&intRescType [i*MAX_DATA_SIZE]);
        if (objTypeInx < 0) {
            freeSqlResult (myresult);
            freeInfoOut (*cacheInfoOut);
            freeInfoOut (*tapeInfoOut);
            return (OBJ_ERR_RES_TYPE);
        }

        infoOut = (struct mdasInfoOut *)
            malloc (sizeof (struct mdasInfoOut));

        memset (infoOut, 0, sizeof (struct mdasInfoOut));

	infoOut->objTypeInx = objTypeInx;
	infoOut->objID = strdup (&objID[i*MAX_DATA_SIZE]);
	infoOut->collection = strdup (&collection[i*MAX_DATA_SIZE]);
	infoOut->replNum = atoi (&cReplNum[i*MAX_DATA_SIZE]);
	infoOut->segNum = atoi (&cSegNum[i*MAX_DATA_SIZE]);
	infoOut->intReplNum = atoi (&cIntReplNum[i*MAX_DATA_SIZE]);
	infoOut->intSegNum = atoi (&cIntSegNum[i*MAX_DATA_SIZE]);
	infoOut->dataPath = strdup (&intPathName[i*MAX_DATA_SIZE]);
	infoOut->resourceName = strdup (&intRescName[i*MAX_DATA_SIZE]);
	infoOut->resourceLoc = strdup (&intRescLoc[i*MAX_DATA_SIZE]);
	infoOut->isDirty = atoi (&intIsDirty[i*MAX_DATA_SIZE]);
	infoOut->phyResClass = strdup (&intRescClass[i*MAX_DATA_SIZE]);
        infoOut->resClassInx = resClassLookup (infoOut->phyResClass);
        infoOut->size = strtoll (&cIntSize[i*MAX_DATA_SIZE], 0, 0);
        infoOut->offset = strtoll (&cOffset[i*MAX_DATA_SIZE], 0, 0);

        /* XXXX if (strcmp (&intRescType[i*MAX_DATA_SIZE], TAPE_RESC) == 0) { */
	if (resClassEntries[infoOut->resClassInx].permanent == 1) {
            /* a tape resource */
            queInfoOut (tapeInfoOut, infoOut, 1, 0);
        } else {
            queInfoOut (cacheInfoOut, infoOut, 1, 0);
        }

    }

    freeSqlResult (myresult);

    return (0);
}

/* _svrGetInternalObjInfo - This is very similar to getInternalReplicaInfo
 */

int _svrGetInternalObjInfo (char *objName, char *objCollName,
int   objReplNum, int   objSegNum,
struct mdasInfoOut **cacheInfoOut, struct mdasInfoOut **tapeInfoOut)
{

    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int i , selval[MAX_DCS_NUM];
    int retVal;
    char *objID, *cReplNum, *cSegNum, 
    *cIntReplNum, *cIntSegNum, *intPathName, *intRescName,
    *intRescLoc, *intRescType, *intIsDirty, *intRescClass, *cOffset,
    *cIntSize;
    mdasC_sql_result_struct *myresult = NULL;

    *cacheInfoOut = NULL;
    *tapeInfoOut = NULL;

    for (i = 0; i < MAX_DCS_NUM; i++)  {
      sprintf(qval[i],"");
      selval[i] = 0;
    }

    sprintf(qval[DATA_NAME]," = '%s'",objName);
    sprintf(qval[DATA_GRP_NAME]," = '%s'",objCollName);
    sprintf(qval[DATA_REPL_ENUM]," = %i",objReplNum);
    sprintf(qval[DATA_SEG_NUM]," = %i",objSegNum);

    selval[DATA_NAME] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[DATA_SEG_NUM] = 1;
    selval[INT_REPL_ENUM] = 1;
    selval[INT_SEG_NUM] = 1;
    selval[INT_PATH_NAME] = 1;
    selval[INT_RSRC_NAME] = 1;
    selval[INT_RSRC_ADDR_NETPREFIX] = 1;
    selval[INT_RSRC_TYP_NAME] = 1;
    selval[INT_IS_DIRTY] = 1;
    selval[INT_RSRC_CLASS] = 1;
    selval[INT_OFFSET] = 1;
    selval[INT_SIZE] = 1;

  retVal = queryDataInfo (objCollName, selval, (char (*) [MAX_TOKEN]) qval, 
   &myresult, DEF_NUM_ROWS);

    if (retVal < 0)
        return retVal;

    objID = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_NAME],
            dcs_aname[DATA_NAME]);

    cReplNum = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_REPL_ENUM],
            dcs_aname[DATA_REPL_ENUM]);

    cSegNum = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_SEG_NUM],
            dcs_aname[DATA_SEG_NUM]);

    cIntReplNum = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_REPL_ENUM],
            dcs_aname[INT_REPL_ENUM]);

    cIntSegNum = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_SEG_NUM],
            dcs_aname[INT_SEG_NUM]);

    intPathName = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_PATH_NAME],
            dcs_aname[INT_PATH_NAME]);

    intRescName = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_RSRC_NAME],
            dcs_aname[INT_RSRC_NAME]);

    intRescLoc = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,dcs_tname[INT_RSRC_ADDR_NETPREFIX],
            dcs_aname[INT_RSRC_ADDR_NETPREFIX]);

    intRescType = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_RSRC_TYP_NAME],
            dcs_aname[INT_RSRC_TYP_NAME]);

    intIsDirty = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_IS_DIRTY],
            dcs_aname[INT_IS_DIRTY]);

    intRescClass = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_RSRC_CLASS],
            dcs_aname[INT_RSRC_CLASS]);

    cOffset = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_OFFSET],
            dcs_aname[INT_OFFSET]);

    cIntSize = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[INT_SIZE],
            dcs_aname[INT_SIZE]);

    if (myresult->row_count <= 0)
        return (MCAT_INQUIRE_ERROR);

    for (i = 0; i < myresult->row_count; i++) {
        int objTypeInx;
        struct mdasInfoOut *infoOut;

        objTypeInx = objTypeLookUp (&intRescType [i*MAX_DATA_SIZE]);
        if (objTypeInx < 0) {
            freeSqlResult (myresult);
            freeInfoOut (*cacheInfoOut);
            freeInfoOut (*tapeInfoOut);
            freeSqlResult (myresult);
            return (OBJ_ERR_RES_TYPE);
        }

        infoOut = (struct mdasInfoOut *)
            malloc (sizeof (struct mdasInfoOut));

        memset (infoOut, 0, sizeof (struct mdasInfoOut));

	infoOut->objTypeInx = objTypeInx;
        infoOut->objID = strdup (&objID[i*MAX_DATA_SIZE]);
        infoOut->collection = strdup (objCollName);
        infoOut->replNum = atoi (&cReplNum[i*MAX_DATA_SIZE]);
        infoOut->segNum = atoi (&cSegNum[i*MAX_DATA_SIZE]);
        infoOut->intReplNum = atoi (&cIntReplNum[i*MAX_DATA_SIZE]);
        infoOut->intSegNum = atoi (&cIntSegNum[i*MAX_DATA_SIZE]);
        infoOut->dataPath = strdup (&intPathName[i*MAX_DATA_SIZE]);
        infoOut->resourceName = strdup (&intRescName[i*MAX_DATA_SIZE]);
#ifdef FED_MCAT
        infoOut->resourceLoc = getAddrWithZone (&intRescLoc[i*MAX_DATA_SIZE],
         objCollName);
#else
        infoOut->resourceLoc =  strdup (&intRescLoc[i*MAX_DATA_SIZE]);
#endif
        /* infoOut->resourceLoc = strdup (&intRescLoc[i*MAX_DATA_SIZE]); */
        infoOut->isDirty = atoi (&intIsDirty[i*MAX_DATA_SIZE]);
        infoOut->phyResClass = strdup (&intRescClass[i*MAX_DATA_SIZE]);
        infoOut->resClassInx = resClassLookup (infoOut->phyResClass);
        infoOut->size = strtoll (&cIntSize[i*MAX_DATA_SIZE], 0, 0);
        infoOut->offset = strtoll (&cOffset[i*MAX_DATA_SIZE], 0, 0);

        /* XXXX if (strcmp (&intRescType[i*MAX_DATA_SIZE], TAPE_RESC) == 0) { */
	if (resClassEntries[infoOut->resClassInx].permanent == 1) {
            /* a tape resource */
            queInfoOut (tapeInfoOut, infoOut, 1, 0);
        } else {
            queInfoOut (cacheInfoOut, infoOut, 1, 0);
        }

    }
    /* process the tapeInfoOut */

#ifdef TAPE_DRIVE
    procTapeInfo (tapeInfoOut);
#endif

    freeSqlResult (myresult);

    return (0);
}

#ifdef TAPE_DRIVE
/* procTapeInfo - Process the input link list of tapeInfoOut. Sort them by 
 * intReplNum and link the intSegNum in the tapeFileInfo link
 */

int
procTapeInfo (struct mdasInfoOut **tapeInfoOut)
{
    char tapeNum[MAX_TOKEN]; 
    struct mdasInfoOut *tmpInfoOut, *nextInfoOut, *newTapeInfoOut;
    
    tmpInfoOut = *tapeInfoOut;

    if (tmpInfoOut == NULL)
	return (0);

    if (srbObjTypeEntry [tmpInfoOut->objTypeInx].objType != TAPE_TYPE) 
	return (0);


    newTapeInfoOut = nextInfoOut = NULL;


    while (tmpInfoOut != NULL) {
	struct mdasInfoOut *matchInfoOut;
	tapeFileInfo_t *tapeFileInfo;

	nextInfoOut = tmpInfoOut->next;
	tmpInfoOut->next = NULL;
	if ((matchInfoOut = matchIntReplNum (newTapeInfoOut, tmpInfoOut)) == 
	  NULL) {
	    /* A new intReplNum */
	    tmpInfoOut->tapeFileInfo = NULL;
	    tapeFileInfo = NULL;
	    genTapeFileInfo (tmpInfoOut, &tapeFileInfo);
	    queTapeFileInfo (tmpInfoOut, tapeFileInfo);
	    queTapeInfo (&newTapeInfoOut, tmpInfoOut);
	} else {
	    genTapeFileInfo (tmpInfoOut, &tapeFileInfo);
	    freeInfoOut (tmpInfoOut);
	    queTapeFileInfo (matchInfoOut, tapeFileInfo);
	    matchInfoOut->size += tapeFileInfo->size;
	}
        tmpInfoOut = nextInfoOut;
    }
    *tapeInfoOut = newTapeInfoOut;
    return (0);
}
#endif	/* TAPE_DRIVE */

struct mdasInfoOut *
matchIntReplNum (struct mdasInfoOut *infoOutHead, struct mdasInfoOut *myInfoOut)
{
    struct mdasInfoOut *tmpInfoOut;

    tmpInfoOut = infoOutHead;

    while (tmpInfoOut != NULL) {
	if (strcmp (tmpInfoOut->objID, myInfoOut->objID) == 0 &&
	  tmpInfoOut->replNum == myInfoOut->replNum &&
	  tmpInfoOut->intReplNum == tmpInfoOut->intReplNum) {
	    /* a match */
	    return tmpInfoOut;
	}

	tmpInfoOut = tmpInfoOut->next;
    }

    /* no match */

    return NULL;
}

#ifdef TAPE_DRIVE
int
queTapeFileInfo (struct mdasInfoOut *myInfoOut, tapeFileInfo_t *myTapeFileInfo)
{
    tapeFileInfo_t *tmpTapeFileInfo, *prevTapeFileInfo;

    if (myTapeFileInfo == NULL)
	return 0;

    tmpTapeFileInfo = myInfoOut->tapeFileInfo;
    prevTapeFileInfo = NULL;
    while (tmpTapeFileInfo != NULL) {
	/* sort it by increasing intSegNum */
	if (myTapeFileInfo->intSegNum < tmpTapeFileInfo->intSegNum) {
	    if (prevTapeFileInfo == NULL) {
		/* At the top */
		myInfoOut->tapeFileInfo = myTapeFileInfo;
	    } else {
		prevTapeFileInfo->next = myTapeFileInfo;
	    }
	    myTapeFileInfo->next = tmpTapeFileInfo;
	    return (0);
	}

	prevTapeFileInfo = tmpTapeFileInfo;
	tmpTapeFileInfo = tmpTapeFileInfo->next;
    }

    /* got to the end */

    if (prevTapeFileInfo == NULL) {
        /* At the top */
        myInfoOut->tapeFileInfo = myTapeFileInfo;
    } else {
        prevTapeFileInfo->next = myTapeFileInfo;
    }
    myTapeFileInfo->next = NULL;
    return (0);
}
#endif	/* TAPE_DRIVE */

int
queTapeInfo (struct mdasInfoOut **tapeInfoHead, struct mdasInfoOut *myInfoOut)
{
    struct mdasInfoOut *tmpInfoOut, *prevInfoOut;

    tmpInfoOut = *tapeInfoHead;
    prevInfoOut = NULL;
    while (tmpInfoOut != NULL) {
        /* sort it by increasing intReplNum */
        if (myInfoOut->intReplNum < tmpInfoOut->intReplNum) {
            if (prevInfoOut == NULL) {
                /* At the top */
                *tapeInfoHead = myInfoOut;
            } else {
                prevInfoOut->next = myInfoOut;
            }
            myInfoOut->next = tmpInfoOut;
            return (0);
        }

        prevInfoOut = tmpInfoOut;
        tmpInfoOut = tmpInfoOut->next;
    }

    /* got to the end */

    if (prevInfoOut == NULL) {
        /* At the top */
        *tapeInfoHead = myInfoOut;
    } else {
        prevInfoOut->next = myInfoOut;
    }
    myInfoOut->next = NULL;
    return (0);
}

#ifdef TAPE_DRIVE
int
genTapeFileInfo (struct mdasInfoOut *myInfoOut, 
tapeFileInfo_t **outTapeFileInfo)
{
    int retVal;
    tapeFileInfo_t *myTapeFileInfo;
    int tapeLibInx;

    myTapeFileInfo = malloc (sizeof (tapeFileInfo_t));
    memset (myTapeFileInfo, 0, sizeof (tapeFileInfo_t));

    retVal = parseTapePathName (myInfoOut->dataPath, myTapeFileInfo->tapeNum, 
      &myTapeFileInfo->tapePos.filePos.fileSeqNo);
    
    if (retVal < 0) {
	free (myTapeFileInfo);
        *outTapeFileInfo = NULL;
	return (retVal);
    }

    myTapeFileInfo->intReplNum = myInfoOut->intReplNum;
    myTapeFileInfo->intSegNum = myInfoOut->intSegNum;
    myTapeFileInfo->offset = myInfoOut->offset;
    myTapeFileInfo->size = myInfoOut->size;

    /* XXXXXX - assume tapeLibInx == 0 for now */

    tapeLibInx = 0;
    myTapeFileInfo->tapeLibConfig = matchTapeLibByInx (tapeLibInx);

    myTapeFileInfo->next = NULL;
    *outTapeFileInfo = myTapeFileInfo;
    return (0);
}

#endif	/* TAPE_DRIVE */

int
svrRmInternalCompObj (struct varlena *vObjID, struct varlena *vCollectionName,
int replNum, int segNum, int intReplNum, int intSegNum)
{
    char *objID;
    char *collectionName;
    int retVal;

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);

    retVal = _svrRmInternalCompObj (objID, collectionName, 
      replNum, segNum, intReplNum, intSegNum,
      ClientUser->userName, ClientUser->domainName);

    free (objID);
    free (collectionName);

    return retVal;
}

int
_svrRmInternalCompObj (char *objID, char *collectionName,
int replNum, int segNum, int intReplNum, int intSegNum,
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
        retVal = removeInternalCompoundObject (NULL,
         objID, collectionName, replNum, segNum, intReplNum, intSegNum,
          userName, domainName);

	if (retVal >= 0)
	    commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }

        retVal = srbRmIntCompObj (hostTabPtr->conn,
         objID, collectionName, replNum, segNum, intReplNum, intSegNum);
    }
    if (retVal < 0) {
	elog (NOTICE,
	  "_svrRmInternalCompObj: RmIntCompObj failed for %s/%s, stat=%d",
	  collectionName, objID, retVal);
    }

    return retVal;
}

int
svrRmCompObj (struct varlena *vObjID, struct varlena *vCollectionName,
int replNum, int segNum)
{
    char *objID;
    char *collectionName;
    int retVal;

    objID = varToStr (vObjID);
    collectionName = varToStr (vCollectionName);

    retVal = _svrRmCompObj (objID, collectionName,
      replNum, segNum, 
      ClientUser->userName, ClientUser->domainName);

    free (objID);
    free (collectionName);

    return retVal;
}

int
_svrRmCompObj (char *objID, char *collectionName,
int replNum, int segNum,
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
        retVal = removeCompoundObject (NULL,
         objID, collectionName, replNum, segNum,
          userName, domainName);
	if (retVal >= 0)
	    commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }

        retVal = srbRmCompObj (hostTabPtr->conn,
         objID, collectionName, replNum, segNum);
    }
    if (retVal < 0) {
        elog (NOTICE,
          "srbRmCompObj: RmCompObj failed for %s/%s, stat=%d",
          collectionName, objID, retVal);
    }

    return retVal;
}

int
svrModInternalCompObj (struct varlena *vObjName, struct varlena *vObjCollName,
int   objReplNum, int objSegNum,
int inpIntReplNum, int intSegNum,
struct varlena *vData_value_1, struct varlena *vData_value_2,
struct varlena *vData_value_3, struct varlena *vData_value_4,
int retraction_type)
{
    char *objName;
    char *objCollName;
    char *data_value_1; 
    char *data_value_2;
    char *data_value_3; 
    char *data_value_4;
    int retVal;

    objName = varToStr (vObjName);
    objCollName = varToStr (vObjCollName);
    data_value_1 = varToStr (vData_value_1);
    data_value_2 = varToStr (vData_value_2);
    data_value_3 = varToStr (vData_value_3);
    data_value_4 = varToStr (vData_value_4);

    retVal = _svrModInternalCompObj (objName, objCollName,
      objReplNum, objSegNum,
      inpIntReplNum, intSegNum,
      ClientUser->userName, ClientUser->domainName,
      data_value_1, data_value_2, data_value_3,
      data_value_4, retraction_type);

    free (objName);
    free (objCollName);
    free (data_value_1);
    free (data_value_2);
    free (data_value_3);
    free (data_value_4);

    return retVal;
}

int
_svrModInternalCompObj (char *objName, char *objCollName, 
int   objReplNum, int   objSegNum,
int   intReplNum, int   intSegNum,
char *userName, char *domainName,
char *data_value_1, char *data_value_2,
char *data_value_3, char *data_value_4, int retraction_type)
{

    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    remoteFlag = getMdasEnabledHost (objCollName, &hostTabPtr);

    if (remoteFlag < 0) {
        retVal = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        retVal = modifyInternalCompoundObject (NULL,
         objName, objCollName, objReplNum, objSegNum,
	 intReplNum, intSegNum, 
	  userName, domainName,
	  data_value_1, data_value_2, 
	  data_value_3, data_value_4, retraction_type);
	if (retVal >= 0)
	    commit_db2_interaction (MDAS_CAT_COMMIT);
#endif
    } else {
        if ((retVal = remoteConnect (hostTabPtr)) < 0) {
            return retVal;
        }

        retVal = srbModInternalCompObj (hostTabPtr->conn,
         objName, objCollName, objReplNum, objSegNum,
         intReplNum, intSegNum, 
          data_value_1, data_value_2, 
          data_value_3, data_value_4, retraction_type);
    }
    if (retVal < 0) {
        elog (NOTICE,
          "_svrModInternalCompObj: ModInternalCompObj failed for %s/%s,stat=%d",
          objCollName, objName, retVal);
    }

    return retVal;
}

