/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/* objBulkFunct.c - Routines to handle Bulk operation type SID.
 */
#include "objBulkFunct.h"

int
svrBulkLoad (int catType, struct varlena *vBloadFullPath,
struct varlena *packedResult)
{
    char *bloadFullPath;
    mdasC_sql_result_struct *myresult;
    int retVal;
    char bloadFileName[MAX_TOKEN];
    char bloadCollName[MAX_TOKEN];
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead;
    struct hostElement  *hostTabPtr;
    int remoteFlag;
#ifndef FED_MCAT
    char *portNum;
    char *tmpResLoc;
#endif
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
#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (infoOutHead->resourceLoc, &hostTabPtr);
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
    char *dataName, *collection, *size, *pathName, *dataType, *chksum;
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

	compResInfo = getCompResInfoForLoad (resInfo);

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
		break;
		/* return (retVal); */
	    } else {
		if (chksum != NULL) {
	            if (atoi (&chksum[i*MAX_DATA_SIZE]) != atoi (outChksum)) {
			retVal = OBJ_ERR_BAD_CHKSUM;
			break;
		        /* return (OBJ_ERR_BAD_CHKSUM); */
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
           &myresult, BUNLOAD_NUM_ROWS);
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
         "NONCONTCOLLINFO", collection, myresult, BUNLOAD_NUM_ROWS, "read")) < 0) {
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
             catType, BL_LOCAL_FLAG | BL_EXIT_WHEN_DONE, 
	     collection, myPortList);
	    if (status == MSG_USE_SINGLE_PORT) {
		sendStatusToClient (MSG_USE_SINGLE_PORT, -1, myComm);
		portalToPortalRelay (tmpHostTabPtr->conn->sock, myComm->commFd);
		remoteClose (tmpHostTabPtr);
		status = DONT_SEND_RETURN;
	    }
	} else {
            status = _srbBulkUnload (tmpHostTabPtr->conn, 
	     catType, BL_LOCAL_FLAG, clHostAddr, clPort, collection);
	}

	return (status);
    }
	
    
    /* Gets here when we decided to do it here */
    
#ifdef foo
    bunloadHead = (struct bunloadSort *) malloc (sizeof (struct bunloadSort));
#endif	/* foo */
    memset (&bunloadHead, 0, sizeof (struct bunloadSort));

    continuation_index = myresult->continuation_index;
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
        if (flag & BL_EXIT_WHEN_DONE && SingleSvrPort > 0) {
            close (myComm->commFd);
            return (EXIT_WHEN_DONE);
        } else {
            return (DONT_SEND_RETURN);
	}
    }

    while (continuation_index >= 0) {
	myresult = NULL;
	if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
            status = get_more_rows (catType, continuation_index, &myresult, 
	     BUNLOAD_NUM_ROWS);
#endif
        } else {
            status = remoteGetMoreRows (catType, continuation_index, &myresult, 
	     BUNLOAD_NUM_ROWS, hostTabPtr);
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
            if (flag & BL_EXIT_WHEN_DONE && SingleSvrPort > 0) {
                close (myComm->commFd);
                return (EXIT_WHEN_DONE);
            } else {
                return (DONT_SEND_RETURN);
            }
        }

    }

    sendDoneToPortal (myFd);

    closeUnloadSock (myFd, flag);

    if (flag & BL_EXIT_WHEN_DONE && SingleSvrPort > 0) {
        close (myComm->commFd);
        return (EXIT_WHEN_DONE);
    } else
        return (DONT_SEND_RETURN);

}

int
sendBunloadData (struct bunloadSort *bunloadHead, char *clHostAddr, int clPort,
svrComm_t *myComm, int flag, int myFd)
{
    struct bunloadSort *tmpBunloadSort;
    mdasC_sql_result_struct *myresult;
    int status, i;
    char *objID, *resourceType, *dataPath, *resourceLoc, *size,
    *collection, *chksum;
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
	sendStatusToPortal (myFd, SYS_ERR_MDAS_CAT_RET);
        return SYS_ERR_MDAS_CAT_RET;
    }

    myresult = (mdasC_sql_result_struct *) malloc 
     (sizeof (mdasC_sql_result_struct));

    if (myresult == NULL) {
        elog(NOTICE,"sendBunloadData: malloc error");
	sendStatusToPortal (myFd, SYS_ERR_MALLOC);
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
	    sendStatusToPortal (myFd, SYS_ERR_MALLOC);
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
	closeUnloadSock (myFd, flag);
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
        	closeUnloadSock (myFd, flag);
		free (buf);
		if (flag & BL_EXIT_WHEN_DONE && SingleSvrPort > 0) {
		    close (myComm->commFd);
		    return (EXIT_WHEN_DONE);
		} else
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
        	        closeUnloadSock (myFd, flag);
			free (buf);
                	if (flag & BL_EXIT_WHEN_DONE && SingleSvrPort > 0) {
                            close (myComm->commFd);
                    	    return (EXIT_WHEN_DONE);
                	} else
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
		     "sendBunloadData: read error for %s at %s, toread=%lld, read=%d, errno=%d\n",
		      &dataPath[i*MAX_DATA_SIZE], &resourceLoc[i*MAX_DATA_SIZE],
		     toread, nbytes, errno);
                    closeUnloadSock (myFd, flag);
		    free (buf);
                    if (flag & BL_EXIT_WHEN_DONE && SingleSvrPort > 0) {
                        close (myComm->commFd);
                        return (EXIT_WHEN_DONE);
                    } else
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
svrBulkRegister (int catType, struct varlena *vContainer,
struct varlena *packedResult, int noCheckFlag)
{
    char *container;
    mdasC_sql_result_struct *myresult;
    int retVal;

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
    } else if (ClientUser->privUserFlag == 1) {
        noCheckFlag = 1;
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
    int retVal;
    int remoteFlag;
    struct hostElement  *hostTabPtr;
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
      char *collectionName;
      char contColl[MAX_TOKEN];
      char contName[MAX_TOKEN];

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
         myresult, noCheckFlag);

        if (retVal < 0) {
            elog (NOTICE, "svrBulkRegister: srbBulkRegister error: %s",
              clErrorMessage(hostTabPtr->conn));
        }
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
  int retVal, status;
  struct hostElement  *hostTabPtr;
  int remoteFlag;

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
#ifdef foo      /* not doing count here */
        if (prevFileName == NULL || strcmp (prevFileName, fileName) != 0 ||
         strcmp (prevCollection, collection) != 0) {
            prevFileName = fileName;
            prevCollection = collection;
            fileCnt ++;
        }

        if (flag > 0) {         /* do count only */
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
#ifndef FED_MCAT
            char *portNum, tmpResLoc[MAX_TOKEN];
#endif
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
                        tmpRes->resClass = 0;   /* cache */
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
    if (flag > 0)       /* count only */
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
    int i;
    int *unloadFlag;
    int curInx;
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

        pickCopyToUnload (numCopies, i, curInx - 1, tmpBunloadSort);
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

    if (tmpBunloadSort == NULL || tmpBunloadSort->myresult == NULL ||
     tmpBunloadSort->myresult->row_count <= 0)
        return (SYS_ERR_MDAS_CAT_RET);

    myresult = tmpBunloadSort->myresult;
    unloadFlag = tmpBunloadSort->unloadFlag;


    memset (chksum, 0, sizeof (chksum));        /* no chksum for now */

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
        elog (NOTICE, "objID = %x, resourceType=%x, dataPath=%x, resourceLoc=%x, resourceName=%x, dataType=%x, replNum=%x, offset=%x, size=%x, phyResClass=%x, containerName=%x, isDirty=%x",
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
#ifdef FED_MCAT
        free (tmpLoc);
#endif

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
      while (infoOut != NULL) { /* we may have to deal with compound obj */

        myInx = infoOut->offset;
        if (srbObjTypeEntry[infoOut->objTypeInx].objType == COMPOUND_TYPE) {
	    status = stageAndFileResult (infoOut, 
	      &resourceType[myInx*MAX_DATA_SIZE], 
	      &dataPath[myInx*MAX_DATA_SIZE], 
	      &resourceLoc[myInx*MAX_DATA_SIZE], 
	      &resourceName[myInx*MAX_DATA_SIZE],
	      &phyResClass[myInx*MAX_DATA_SIZE]);
	    if (status >= 0) 
		break; 

#ifdef foo 	/* done in stageAndFileResult */
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
#endif	/* foo */
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

    return (myInx);
}

int
stageAndFileResult (struct mdasInfoOut *infoOut, 
char *resourceType, char *dataPath, char *resourceLoc, char *resourceName,
char *phyResClass)
{
    struct mdasInfoOut *cacheInfoOut = NULL;
    int status;

    status = _svrStageCompObj (infoOut, infoOut->collection, &cacheInfoOut, 1);
    if (status < 0) {
        elog (NOTICE,
         "stageAndFileResult: Unable to stage compObj %s/%s, stat=%d",
          infoOut->collection, infoOut->objID);
    } else {
        /* stage is successful have to overwite myresult */

        strcpy (resourceType,
        srbObjTypeEntry[cacheInfoOut->objTypeInx].resourceType);

        strcpy (dataPath, cacheInfoOut->dataPath);

        strcpy (resourceLoc, cacheInfoOut->resourceLoc);

        strcpy (resourceName, cacheInfoOut->resourceName);

        strcpy (phyResClass, cacheInfoOut->phyResClass);

	freeAllInfoOut (cacheInfoOut);
    }
    return (status);
}

void
closeUnloadSock (int sock, int flag)
{
    /* don't close if using SingleSvrPort */
    if ((flag & CL_INIT_CONN_FLAG) && SingleSvrPort > 0)
	return;

#ifdef _WIN32
    closesocket(sock);
#else
    close (sock);
#endif
}

srb_long_t
portalToPortalRelay (int srcFd, int destFd)
{
    char *buf;
    int status;
    int bytesRead = 0;
    srb_long_t bytesCopied = 0;
    struct relayHeader myHeader;
    char *tmpPtr;
    int toRead;

    tmpPtr = (char *) &myHeader;
    toRead = sizeof (myHeader);
    while (toRead > 0) {
        status = read (srcFd, tmpPtr, toRead);
	toRead -= status;
	tmpPtr += status;
    }
    if (toRead != 0) {
	elog (NOTICE, 
	 "portalToPortalRelay: Read relayHeader problem, toRead=%d, length=%d",
	 toRead, myHeader.length);
	return (OBJ_ERR_COPY_LEN);
    }

    status = write (destFd, &myHeader, sizeof (myHeader));

    buf = malloc (BUF_SIZE);

    while ((bytesRead = read (srcFd, buf, BUF_SIZE)) > 0) {
        status = write (destFd, buf, bytesRead);
        if (status == bytesRead) {
            bytesCopied += bytesRead;
        } else {
	    elog (NOTICE, "portalToPortalRelay: To write %d, %d written, errno=%d",
	     bytesRead, status, errno);
            bytesCopied = OBJ_ERR_COPY_LEN;
            break;
        }
    }   /* while loop */

    free (buf);
    return (bytesCopied);
}

int
sendStatusToClient (int status, int retlen, svrComm_t *myComm)
{
    struct varlena * retVal;
    FunctCall myFunctCall;

    if (retlen == -1) {
        retVal = (struct varlena *)malloc(VAROUTHDRSZ);
        VARSIZE(retVal) = VAROUTHDRSZ;
        VAROUTSTAT(retVal) = htonl (MSG_USE_SINGLE_PORT);
        myFunctCall.retlen = -1;
        sendFunctionResult (myComm, &myFunctCall, (char *) retVal);
	return (0);
    } else {
	return (-1);
    }
}

int
setupSockForBunload (char *clHostAddr, int clPort, svrComm_t *myComm, int flag)
{
    int myFd = -1;
    FunctCall myFunctCall;

    if (flag & CL_INIT_CONN_FLAG) {
        int lsock = -1, lport;

        if (SingleSvrPort <= 0) {
            lsock = setupSrvPortal (myComm, &lport);
            if (lsock < 0) {
                elog (NOTICE,
                 "setupSockForBunload() -- setupSrvPortal() failed: status=%d",
                  lsock);
                return lsock;
            }
           myFd = acceptSrvPortal (lsock, lport);
           if (myFd < 0) {
               return (myFd);
           }
        } else {        /* SingleSvrPort */
            sendStatusToClient (MSG_USE_SINGLE_PORT, -1, myComm);
            myFd = myComm->commFd;
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
    flag = flag & (~BL_LOCAL_FLAG);

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

int procBMoveResForComp (mdasC_sql_result_struct *myresult)
{
    char *collection, *objID, *resourceType, *dataPath, *resourceLoc,
    *resourceName, *replNum, *size, *phyResClass, *chksum, *intReplNum;
    char dummy[MAX_TOKEN];
    int status, i;
    struct mdasInfoOut *infoOut = NULL;
 
    memset (dummy, 0, sizeof (dummy));

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
    replNum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
    size = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[SIZE], dcs_aname[SIZE]);
    phyResClass = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_CLASS], dcs_aname[RSRC_CLASS]);
    chksum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);

    /* use INT_REPL_ENUM to indicate whether it is a compund obj */
    i = myresult->result_count;
    myresult->sqlresult[i].values = intReplNum =
      malloc (myresult->row_count * MAX_DATA_SIZE);
    if (myresult->sqlresult[i].values == NULL) {
        elog(NOTICE,"procBMoveResForComp: malloc error");
	free (infoOut);
        return SYS_ERR_MALLOC;
    }
    memset (myresult->sqlresult[i].values, 0,
     myresult->row_count * MAX_DATA_SIZE);
    myresult->sqlresult[i].tab_name = strdup (dcs_tname[INT_REPL_ENUM]);
    myresult->sqlresult[i].att_name = strdup (dcs_aname[INT_REPL_ENUM]);
    myresult->result_count++;

    for (i = 0; i < myresult->row_count; i++) {
	char *tmpLoc;

        if (strcmp (&resourceType[i*MAX_DATA_SIZE], COMPOUND_RESC) == 0) {

#ifdef FED_MCAT
            tmpLoc = getAddrWithZone (&resourceLoc[i*MAX_DATA_SIZE],
               &collection[i*MAX_DATA_SIZE]);
#else
            tmpLoc = &resourceLoc[i*MAX_DATA_SIZE];
#endif
            infoOut = malloc (sizeof (struct mdasInfoOut));
            memset (infoOut, 0, sizeof (struct mdasInfoOut));

            status = fillInfoOut (infoOut, &objID[i*MAX_DATA_SIZE],
             &resourceType[i*MAX_DATA_SIZE], &dataPath[i*MAX_DATA_SIZE],
             tmpLoc,
             &resourceName[i*MAX_DATA_SIZE],
             dummy, &replNum[i*MAX_DATA_SIZE],
             dummy, &size[i*MAX_DATA_SIZE],
             &phyResClass[i*MAX_DATA_SIZE], dummy,
             dummy, &chksum[i*MAX_DATA_SIZE],
             &collection[i*MAX_DATA_SIZE]);

            status = stageAndFileResult (infoOut,
              &resourceType[i*MAX_DATA_SIZE],
              &dataPath[i*MAX_DATA_SIZE],
              &resourceLoc[i*MAX_DATA_SIZE],
              &resourceName[i*MAX_DATA_SIZE],
              &phyResClass[i*MAX_DATA_SIZE]);

            freeInfoOut (infoOut);
            if (status < 0) { 
        	elog(NOTICE,
		  "procBMoveResForComp: stageAndFileResult error, stat = %d",
		  status);
        	return status;
	    }
	    sprintf (&intReplNum[i*MAX_DATA_SIZE], "0");
#ifdef FED_MCAT
            free (tmpLoc);
#endif
	} else {
            sprintf (&intReplNum[i*MAX_DATA_SIZE], "-1");
	}
    }
    return (0);
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


    collection = (char *) get_from_result_struct(
     (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    status = procBMoveResForComp (myresult);
    if (status < 0) {
        elog(NOTICE,
         "localBulkMoveData:procBMoveResForComp error for %s,status=%d",
          collection, status);
        return (status);
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
    char *collection, *objID, *resourceType, *dataPath, *resourceLoc,
    *resourceName, *replNum, *size, *phyResClass, *chksum, *intReplNum;
    char dummy[MAX_TOKEN];
    int destTypeInx;
   struct mdasInfoOut *infoOut = NULL;

    memset (dummy, 0, sizeof (dummy));

    if (myresult == NULL || myresult->row_count == 0) {
        return 0;
    }

    collection = (char *) get_from_result_struct(
         (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    objID = (char *)get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[DATA_NAME],
            dcs_aname[DATA_NAME]);
    resourceName = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult, dcs_tname[PHY_RSRC_NAME],
          dcs_aname[PHY_RSRC_NAME]);
    replNum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
    size = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[SIZE], dcs_aname[SIZE]);
    phyResClass = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[RSRC_CLASS], dcs_aname[RSRC_CLASS]);
    chksum = (char *) get_from_result_struct(
        (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);

    resourceType = (char *)get_from_result_struct(
    (mdasC_sql_result_struct *) myresult, dcs_tname[PHY_RSRC_TYP_NAME],
        dcs_aname[PHY_RSRC_TYP_NAME]);
    dataPath = (char *) get_from_result_struct(
    (mdasC_sql_result_struct *) myresult, dcs_tname[PATH_NAME],
      dcs_aname[PATH_NAME]);
    resourceLoc = (char *) get_from_result_struct(
    (mdasC_sql_result_struct *) myresult, dcs_tname[RSRC_ADDR_NETPREFIX],
      dcs_aname[RSRC_ADDR_NETPREFIX]);

    /* this is a fake intReplNum set by procBMoveResForComp to indicate
     * whether the file is a compound obj */
    intReplNum = (char *) get_from_result_struct(
    (mdasC_sql_result_struct *) myresult, dcs_tname[INT_REPL_ENUM],
      dcs_aname[INT_REPL_ENUM]);

    for (i = 0; i < myresult->row_count; i++) {
	int intIntReplNum;

	if (intReplNum != NULL) {
	    intIntReplNum = atoi (intReplNum);
	} else {
	    intIntReplNum = -1;
	}

	if (intIntReplNum >= 0) {
            infoOut = malloc (sizeof (struct mdasInfoOut));
            memset (infoOut, 0, sizeof (struct mdasInfoOut));
            status = fillInfoOut (infoOut, objID,
             COMPOUND_RESC, dataPath,
             resourceLoc,
             resourceName,
             dummy, replNum,
             dummy, size,
             phyResClass, dummy,
             dummy, chksum,
             collection);
	    status = rmContentOfInfoOut (infoOut, infoOut->collection);
	    freeInfoOut (infoOut);
	} else {
            destTypeInx = objTypeLookUp (resourceType);
            status = _objUnlink (srbObjTypeEntry[destTypeInx].objType,
            srbObjTypeEntry[destTypeInx].systemType, resourceLoc,
             dataPath, NO_CHKDIR);
	}
        if (status < 0) {
            elog (NOTICE, 
	     "delMovedSrcData:_objUnlink error for %s.status=%d",
             dataPath, status);
	}

        collection += MAX_DATA_SIZE;
        objID += MAX_DATA_SIZE;
        resourceType += MAX_DATA_SIZE;
        dataPath += MAX_DATA_SIZE;
        resourceLoc += MAX_DATA_SIZE;
        resourceName += MAX_DATA_SIZE;
        replNum += MAX_DATA_SIZE;
        size += MAX_DATA_SIZE;
        phyResClass += MAX_DATA_SIZE;
        chksum += MAX_DATA_SIZE;
	if (intReplNum != NULL)
            intReplNum += MAX_DATA_SIZE;
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
            return (remoteFlag);
        }
    }

    destTypeInx = objTypeLookUp (destResInfo->phyResType);

    while (continuation_index >= 0) {
	status = procBMoveResForComp (inresult);
       if (status < 0) {
            elog(NOTICE,
             "localBulkMoveData:procBMoveResForComp error for %s,status=%d",
              collection, status);
            return (status);
        }

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
        status = procBMoveResForComp (inresult);
       if (status < 0) {
            elog(NOTICE,
             "localBulkMoveDataToCont:procBMoveResForComp error for %s,status=%d",
              collection, status);
            return (status);
        }

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
    srb_long_t toread;
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
