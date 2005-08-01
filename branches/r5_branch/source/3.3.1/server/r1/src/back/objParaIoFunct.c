/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/* objParaIoFunct.c - Routines to handle Parallel I/O type SID.
 */
#include "objParaIoFunct.h"

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
    struct hostElement  *srcHostTabPtr, *destHostTabPtr, *myHostTabPtr;
    int srcRemoteFlag, destRemoteFlag;
#ifndef FED_MCAT
    char *portNum;
    char *tmpResLoc;
#endif
    srb_long_t verifySize;

#ifndef FED_MCAT
    zoneFlag = ALL_LOCAL_ZONE;
#endif

    destType = srbObjTypeEntry[destTypeInx].objType;
    srcType = srbObjTypeEntry[srcTypeInx].objType;

#ifdef FED_MCAT
    destRemoteFlag = chkHostAndZone (destResLoc, &destHostTabPtr);
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
    if (SingleSvrPort > 0) {
        retVal = _svrDataCopyS (destTypeInx, destPath, destResLoc,
         srcTypeInx, srcPath, srcResLoc, size);
#ifdef PARA_OPR
    } else if (destType == FILE_TYPE && 
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
    } else if ((srcType == FILE_TYPE &&
      srbObjTypeEntry[srcTypeInx].systemType == ADS_FT) ||
      (destType == FILE_TYPE &&
      srbObjTypeEntry[destTypeInx].systemType == ADS_FT)) {
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
    } else if (size <= SMALL_FILE_SIZE || srcRemoteFlag == LOCAL_HOST || 
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
    if (retVal >= 0 &&
     srbObjTypeEntry[destTypeInx].objType == FILE_TYPE) {
        verifySize = _getFileSizeByPath (
         srbObjTypeEntry[destTypeInx].systemType, destResLoc, destPath);
        if (verifySize != size && size != SOURCE_EMPTY) {
            elog (NOTICE,
             "_svrDataCopy: target size %lld does not match input size %lld",
            verifySize, size);
            if (srbObjTypeEntry[destTypeInx].systemType != ADS_FT) {
                _objUnlink (srbObjTypeEntry[destTypeInx].objType,
                 srbObjTypeEntry[destTypeInx].systemType,
                 destResLoc, destPath, NO_CHKDIR);
                retVal = OBJ_ERR_COPY_LEN;
            }
            else {
                elog (NOTICE, "_svrDataCopy: ignore size mismatch for ADS");
            }
        }
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
    int lsock = -1;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    struct sockaddr_in sin;
    srb_long_t status;
    struct portalInput myInput;
    pthread_t portalManagerThread;
    struct hostElement *srcHostTabPtr;
    int srcRemoteFlag;
#ifndef FED_MCAT
    char *portNum;
    char *tmpResLoc;
#endif
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
        pthread_cancel (portalManagerThread);
        close (lsock);
        if (retval == MSG_USE_SINGLE_PORT) {
            retval = _svrDataCopyS (destTypeInx, destPath, destResLoc,
             srcTypeInx, srcPath, srcResLoc, size);
            if (retval < 0) {
                elog (NOTICE,
                 "_svrDataCopyLocalToSrbP: _svrDataCopyS of %s failed, sta =%d",
                 destPath, retval);
            }
        } else {
            elog (NOTICE,
             "_svrDataCopySrbToLocalP: _srbDataGet of %s failed, stat = %d", 
	     srcPath, retval);
            return retval;
	}
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
    int lsock = -1;
#ifdef PORTNAME_aix
    size_t length;
#else
    int length;
#endif
    struct sockaddr_in sin;
    srb_long_t status;
    struct portalInput myInput;
    pthread_t portalManagerThread;
    struct hostElement *destHostTabPtr;
    int destRemoteFlag;
#ifndef FED_MCAT
    char *portNum;
    char *tmpResLoc;
#endif

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
        pthread_cancel (portalManagerThread);
        close (lsock);
        return INP_ERR_HOST_ADDR;               /* failure */
    }

    if ((retval = remoteConnect (destHostTabPtr)) < 0) {
        pthread_cancel (portalManagerThread);
        close (lsock);
        return retval;
    }

    retval = _srbDataPut (destHostTabPtr->conn, destTypeInx, destPath,
     destResLoc, clHostAddr, clPort, size);

    if (retval < 0) {
        pthread_cancel (portalManagerThread);
        close (lsock);
	if (retval == MSG_USE_SINGLE_PORT) {
	    retval = _svrDataCopyS (destTypeInx, destPath, destResLoc,
             srcTypeInx, srcPath, srcResLoc, size);
	    if (retval < 0) {
                elog (NOTICE,
                 "_svrDataCopyLocalToSrbP: _svrDataCopyS of %s failed, sta =%d",
                 destPath, retval);
	    }
	} else {
            elog (NOTICE,
             "_svrDataCopyLocalToSrbP: _srbDataPut of %s failed, stat = %d", 
	     destPath, retval);
	}
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
   return NULL;
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
    int srcExfInx = -1, destExfInx = -1;
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
    if (SingleSvrPort > 0) {
	*retVal = MSG_USE_SINGLE_PORT;
	return retVal;
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
    VARSIZE(retVal) = VAROUTHDRSZ;

    if (TicketUserFlag) {
        elog (NOTICE, "svrObjPutC: Illegal operation for a ticket user");
        VAROUTSTAT(retVal) = htonl (ILLEGAL_OPR_TICKET_USER);
        return (retVal);
    }

    if (SingleSvrPort > 0) {
        VAROUTSTAT(retVal) = htonl (MSG_USE_SINGLE_PORT);
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
    VAROUTSTAT(retVal) = htonl (sstatus);

    return (retVal);
}

srb_long_t 
_svrObjPut (char *myDestObjID, char *destCollection, char *destResource, 
char *clHostAddr, int clPort, char *dataType, char *destPath, 
srb_long_t mySize, int flag, int numThreads, svrComm_t *myComm)
{
    char *destObjID;
    int srcFd = -1, destFd = -1;
    char *collContainer = NULL;
    struct mdasResInfo *tmpResInfo;
    struct condResult condResult;
    char fullPathName[MAX_TOKEN * 2];
    char compPathName[MAX_TOKEN * 2];
    int copyCnt = 0;            /* Keep track of no. of replica */
    int failCnt = 0;            /* Keep track of no. of failure */
    srb_long_t status;
    char *condInput;
    srb_long_t retVal;
    struct mdasResInfo *compResInfo = NULL;
    int intReplNum;
    int fileExist;
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead = NULL;
    int chksumFlag;
    int iChksum;
    int lockFd = -1;
    int lsock = -1, lport;
    struct resCache *myResCache;
    srb_long_t tmpSize;
    char fileGroup[ENV_LEN];

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
	/* inContainer objects */
	if (collContainer != NULL)
	    free (collContainer);

	if (fileExist > 0) {
	    destFd = _svrObjOpenWithInfo (&infoInp, &infoOutHead, O_RDWR);
	} else {
	    destFd = _svrObjCreate (MDAS_CATALOG, myDestObjID, "", destResource,
	     destCollection, "", -1);
	}

	retVal = _svrDataPutOneThr (destFd, clHostAddr, clPort, mySize, flag,
	 myComm);

        if (destFd >= 0) {
            if (retVal > 0 && fileExist > 0) {
                setObjDescCond (destFd, condInput);
            }
            status = svrObjClose (destFd);
	    if (status < 0 && retVal > 0) {
		retVal = status;
	    }
        }
#ifdef foo	/* done in _svrDataPutOneThr */	
	/* setup srcFd here */

        if (flag & CL_INIT_CONN_FLAG) {

            lsock = setupSrvPortal (myComm, &lport);
            if (lsock < 0) {
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
	    if (mySize > 0) {
	        tmpSize = mySize;
	    } else {
		tmpSize = srbObjDesc[destFd].objSize;
	    }
            status = write1stHeaderToPortal (srcFd, tmpSize, 1);
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
#endif	/* foo */
    } else if (fileExist > 1) {	/* have to depend on Obj desc to manage */
	destFd = _svrObjOpenWithInfo (&infoInp, &infoOutHead, O_RDWR|O_TRUNC);

        retVal = _svrDataPutOneThr (destFd, clHostAddr, clPort, mySize, flag,
         myComm);
	if (destFd >= 0) {
	    if (retVal > 0) {
		setObjDescCond (destFd, condInput);
	    }
            status = svrObjClose (destFd);
            if (status < 0 && retVal > 0) {
                retVal = status;
            }
	}

#ifdef foo	/* done in _svrDataPutOneThr */
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
#endif	/* foo */
    } else {

        /* we are here. Will eventually call svrDataPut */ 

        setChksumFromCond (condInput, &chksumFlag, &iChksum);
	if (chksumFlag == 0 && infoOutHead != NULL && infoOutHead->chksum > 0) {
	    chksumFlag = k_FLAG;
	}
        setFileGroupFromCond (condInput, fileGroup);
#ifdef foo	/* done in setChksumFromCond */
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
#endif /* foo */

        if (fileExist == 1) {
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
            if ((retVal = parseCondResInfo (destCollection, MDAS_CATALOG, 
	     condInput, destResource, &condResult, &myResCache, mySize)) < 0) {
                freeAllInfoOut (infoOutHead);
                free (destObjID);
                free (condInput);
                return (retVal);
	    }
        }

        tmpResInfo = myResCache->resInfoHead;

        while (tmpResInfo != NULL) {
	    int tinx;
	    srb_long_t byteWritten, registerdSize;
	    int regMode;

            if (copyCnt >= condResult.intResult) {   /* Done */
	        retVal = mySize;
                break;
	    }

            if (fileExist > 0) {
                regMode = EXISTING_COPY;
                registerdSize = infoOutHead->size;
            /* } else if (copyCnt <= 1) { */
            } else if (copyCnt <= 0) { /* mw 3/14/05 */
                regMode = NEW_FIRST_COPY;
                registerdSize = 0;
            } else {
                regMode = NEW_REPLICA;
                registerdSize = byteWritten;
            }

            byteWritten = _svrDataPutToRescAndReg (MDAS_CATALOG,
              destObjID, destCollection, tmpResInfo,
              clHostAddr, clPort, mySize, flag, numThreads, myComm, destPath,
	      dataType, regMode, registerdSize,
	      chksumFlag, iChksum, fileGroup,
              fullPathName, &tinx, &lockFd, &compResInfo);

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
                if (condResult.intResult == ALL)
                    break;
            }
#ifdef foo1	/* done in _svrDataPutToRescAndReg */
	    byteWritten = _svrDataPutToRescOrCompResc (MDAS_CATALOG,
	      destObjID, destCollection, tmpResInfo,
	      clHostAddr, clPort, mySize, flag, numThreads, myComm, destPath,
	      fullPathName, &tinx, &lockFd, &compResInfo); 

#ifdef foo	/* done in _svrDataPutToRescOrCompResc */
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
#endif	/* foo */

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

	    compPathName[0] = '\0';		/* make _svrGenRegisterDataset 
					 * to generate a path */
	    if (fileExist > 0) {
	        regMode = EXISTING_COPY;
	        registerdSize = infoOutHead->size;
	    } else if (copyCnt <= 1) {
	        regMode = NEW_FIRST_COPY;
	        registerdSize = 0;
	    } else {
	        regMode = NEW_REPLICA;
	        registerdSize = byteWritten;
	    }
	    
	    retVal = _svrGenRegisterDataset (MDAS_CATALOG, destObjID, 
	     destCollection, ClientUser->userName, ClientUser->domainName,
	     regMode, tmpResInfo, compResInfo, fullPathName,
	     byteWritten, dataType, tinx, NULL, NULL, registerdSize,
	     compPathName, &intReplNum, &lockFd);

#ifdef foo	/* use _svrGenRegisterDataset instead */	  
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
	         destCollection, NULL, NULL, tmpResInfo->phyResName, 
		 myPathName, 1, tinx, tmpResInfo->phyResLoc,
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
#endif	/* foo */

	    if (retVal < 0) {
                status = retVal;
                copyCnt --;
                failCnt ++;
                if (condResult.intResult == ALL)
                    break;
            }
	
	    if (infoOutHead != NULL && infoOutHead->chksum > 0 || chksumFlag) {
	        char chksum[CHKSUM_STR_LEN + 1];

                if (compResInfo != NULL) {
                    /* a compound object */

	            retVal = _objChksumAndReg (tinx, 0, chksum, 0, 0,
	             destObjID, destCollection, 
		     compResInfo->phyResName, fullPathName, 
		     tmpResInfo->phyResLoc, chksumFlag, iChksum, 
		     tmpResInfo->phyResName, compPathName, -1);
	        } else {
	            retVal = _objChksumAndReg (tinx, 0, chksum, 0, 0,
                     destObjID, destCollection, tmpResInfo->phyResName, 
		     fullPathName,
                     tmpResInfo->phyResLoc, chksumFlag, iChksum, 
		     tmpResInfo->phyResName, fullPathName, -1);
	        }
	        if (retVal < 0) {
		    status = retVal;
                    copyCnt --;
                    failCnt ++;
                    if (condResult.intResult == ALL)
                        break;
	        }
	    }
#endif	/* foo1 */
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
        srbHtonll ((u_longlong_t *) &myHeader.offset);
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

    if (SingleSvrPort > 0) {
        *retVal = MSG_USE_SINGLE_PORT;
        return retVal;
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
    srb_long_t mySize;
    struct varlena * retVal;
    srb_long_t status;
    int sstatus;


    retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    VARSIZE(retVal) = VAROUTHDRSZ;

    if (ProxyUser->privUserFlag <= 0) {
        elog (NOTICE, "svrDataPut: User has no privilege for this operation");
        VAROUTSTAT(retVal) = htonl (AUTH_ERR_PROXY_NOPRIV);
        return (retVal);
    }

    if (TicketUserFlag) {
        elog (NOTICE, "svrObjPut: Illegal operation for a ticket user");
        VAROUTSTAT(retVal) = htonl (ILLEGAL_OPR_TICKET_USER);
        return (retVal);
    }

    if (SingleSvrPort > 0) {
        VAROUTSTAT(retVal) = htonl (MSG_USE_SINGLE_PORT);
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
    int srcFd = -1, destExfInx = -1;
    int destType;
    struct hostElement  *hostTabPtr;
    int remoteFlag;
#ifndef FED_MCAT
    char *tmpResLoc;
    char *portNum;
#endif
    portList_t *myPortList;
    int status;
    srb_long_t verifySize;

    if (size < 0)
	return (OBJ_ERR_COPY_LEN);

    destType = srbObjTypeEntry[destTypeInx].objType;

    /* see if we need to do it remotely */

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (destResLoc, &hostTabPtr);
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
	if (retVal >= 0 &&
	 srbObjTypeEntry[destTypeInx].objType == FILE_TYPE) {
	    verifySize = _getFileSizeByPath (
	     srbObjTypeEntry[destTypeInx].systemType, destResLoc, destPath);
	    if (verifySize != size) {
		elog (NOTICE, 
		 "_svrDataPut: target size %lld does not match input size %lld",
		verifySize, size);
        	_objUnlink (srbObjTypeEntry[destTypeInx].objType,
         	 srbObjTypeEntry[destTypeInx].systemType,
           	 destResLoc, destPath, NO_CHKDIR);
		retVal = OBJ_ERR_COPY_LEN;
	    }
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
     destType != FILE_TYPE || size == 0 ||
     (destType == FILE_TYPE &&
     srbObjTypeEntry[destTypeInx].systemType == ADS_FT)) {
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

    if (retVal >= 0 &&
     srbObjTypeEntry[destTypeInx].objType == FILE_TYPE) {
        verifySize = _getFileSizeByPath (
         srbObjTypeEntry[destTypeInx].systemType, destResLoc, destPath);
        if (verifySize != size) {
            elog (NOTICE, 
             "_svrDataPut: target size %lld does not match input size %lld",
            verifySize, size);
            retVal = OBJ_ERR_COPY_LEN;
        }
    }

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

    if (SingleSvrPort > 0) {
        *retVal = MSG_USE_SINGLE_PORT;
        return retVal;
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
    struct varlena * retVal;
    srb_long_t status;
    int sstatus;

    retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    VARSIZE(retVal) = VAROUTHDRSZ;
    if (SingleSvrPort > 0) {
        VAROUTSTAT(retVal) = htonl (MSG_USE_SINGLE_PORT);
	return (retVal);
    }

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
    VAROUTSTAT(retVal) = htonl (sstatus);

    return retVal;
}


srb_long_t
_svrObjGet (char *srcObjID, char *srcCollection, char *clHostAddr, int clPort,
int flag, int numThreads, svrComm_t *myComm)
{
    struct mdasInfoInp infoInp;
    struct mdasInfoOut *infoOutHead, *infoOut;
    srb_long_t retVal;
    srb_long_t  status;
    int srcFd = -1, destFd = -1;

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

    	sortAndCombInfoForObj (&infoOutHead, NULL, 0, 0);

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

srb_long_t
srbToPortalCopy (int srcFd, int destFd)
{
    char *buf;
    int status;
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
    int status;
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
    srbHtonll ((u_longlong_t *) &myHeader.offset);
    myHeader.length = htonl (length);

    status = write (destFd, (void *) &myHeader, sizeof (myHeader));

    if (status <= 0) {
        elog (NOTICE,
         "portalWrite: error writing header to client, errno = %d", errno);
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
    srbHtonll ((u_longlong_t *) &myHeader.offset);
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

    status = sendStatusToPortal (destFd, DONE_OPR);
}

int
sendStatusToPortal (int destFd, int inStatus)
{
    struct relayHeader myHeader;
    int status;

    myHeader.operation = htonl (inStatus);
    myHeader.offset = 0;
    srbHtonll ((u_longlong_t *)&myHeader.offset);
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

    if (SingleSvrPort > 0) {
        *retVal = MSG_USE_SINGLE_PORT;
        return retVal;
    }

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

    retVal = (struct varlena *)malloc(VAROUTHDRSZ);
    VARSIZE(retVal) = VAROUTHDRSZ;

    if (ProxyUser->privUserFlag <= 0) {
        elog (NOTICE, "svrDataGetC: User has no privilege for this operation");
	sstatus = AUTH_ERR_PROXY_NOPRIV;
        VAROUTSTAT(retVal) = htonl (sstatus);
        return (retVal);
    }

    if (SingleSvrPort > 0) {
        VAROUTSTAT(retVal) = htonl (MSG_USE_SINGLE_PORT);
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
    VAROUTSTAT(retVal) = htonl (sstatus);

    return (retVal);    
}

srb_long_t
_svrDataGet (int srcTypeInx, char *srcPath, char *srcResLoc,
char *clHostAddr, int clPort, srb_long_t size, int flag, int numThreads, 
svrComm_t *myComm)
{
    srb_long_t retVal;
    int destFd = -1, srcExfInx = -1;
    int srcType;
#ifndef FED_MCAT
    char *portNum;
    char *tmpResLoc;
#endif
    struct hostElement  *hostTabPtr;
    int remoteFlag;
    int status;
    portList_t *myPortList;

    /* see if we need to do it remotely */

#ifdef FED_MCAT
    remoteFlag = chkHostAndZone (srcResLoc, &hostTabPtr);
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
     srcType != FILE_TYPE || size == 0 ||
     (srcType == FILE_TYPE &&
     srbObjTypeEntry[srcTypeInx].systemType == ADS_FT)) {

	/* too small for double buffer */
        srcExfInx = lowLevelOpen (srcTypeInx, srcPath, srcResLoc, O_RDONLY);

        if (srcExfInx < 0) {   /* error */
            elog (NOTICE, "_svrDataGet: Unable to open the src copy");
            return srcExfInx;
        }
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
    srb_long_t myOffset;
    srb_long_t size0, size1;
    srb_long_t status;
    srb_long_t retVal;
    pthread_mutex_t myLock;
    int lsock = -1, lport, srcFd = -1;

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
    int destExfInx = -1, srcFd = -1;
    char *buf;
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
    srb_long_t myOffset;
    srb_long_t size0, size1;
    srb_long_t status;
    srb_long_t retVal;
    int lport; 
    int destFd = -1;
    int lsock = -1;
    

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
    int srcExfInx = -1, destFd = -1;
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
setupSrvPortal (svrComm_t *myComm, int *mylport)
{
    portList_t thisPortList;
    int lsock = -1, lport;
    struct sockaddr_in sin;
    int status;
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
    int myFd = -1;
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
    srbHtonll ((u_longlong_t *)&myHeader.size);

    status = write (destFd, (void *) &myHeader, sizeof (myHeader));

    if (status <= 0) {
        elog (NOTICE,
         "write1stHeaderToPortal: error writing header to client, errno = %d", 
	  errno);
        return (SYS_ERR_REMOTE_CONN);
    }
    return (0);
}

srb_long_t 
_svrDataPutToRescOrCompResc (int catType,
char *destObjID, char *destCollection, struct mdasResInfo *destResInfo,
char *clHostAddr, int clPort, srb_long_t mySize, int flag, int numThreads, 
svrComm_t *myComm, char *inpPathName, 
char *outFullPathName, int *objTypeInx, int *lockFd,
struct mdasResInfo **outCompResInfo)
{
    srb_long_t byteWritten;
    struct mdasResInfo *compResInfo = NULL;

        if (strcmp (destResInfo->phyResType, COMPOUND_RESC) == 0) {
            compResInfo = destResInfo->compoundLink;
            while (compResInfo != NULL) {
                int resClassInx;
                resClassInx = resClassLookup (compResInfo->phyResClass);
                /* Don't do permanent */
                if (resClassEntries[resClassInx].permanent == 0) {
#ifdef LOCK_OBJ
                    if (*lockFd < 0) {
                        *lockFd =_svrObjLock (destObjID, destCollection,
                          F_SETLKW, F_WRLCK, 0);
                    }
#endif
		    /* the newPathName should be "" or it get the wrong 
		     * outFullPathName */
                    byteWritten = _svrDataPutToResc (catType,
                      destObjID, destCollection, clHostAddr, clPort, mySize,
                      compResInfo, "", outFullPathName, objTypeInx,
                      flag, numThreads, myComm);
                    if (byteWritten >= 0)
                        break;
                }
                compResInfo = compResInfo->next;
            }
        } else {
            byteWritten = _svrDataPutToResc (catType,
              destObjID, destCollection, clHostAddr, clPort, mySize,
              destResInfo, inpPathName, outFullPathName, objTypeInx,
               flag, numThreads, myComm);
        }

    *outCompResInfo = compResInfo;
    return (byteWritten);
}

srb_long_t
_svrDataPutOneThr (int destFd, char *clHostAddr, int clPort, 
srb_long_t mySize, int flag, svrComm_t *myComm)
{
    int lsock = -1, lport;
    srb_long_t tmpSize;
    srb_long_t retVal;
    int srcFd = -1;
    int status;

    if (destFd < 0) {
        return destFd;
    }

    /* setup srcFd here */

    if (flag & CL_INIT_CONN_FLAG) {

        lsock = setupSrvPortal (myComm, &lport);
        if (lsock < 0) {
            return (lsock);
        }
        srcFd = acceptSrvPortal (lsock, lport);
#ifdef _WIN32
        closesocket(lsock);
#else
        close (lsock);
#endif
        if (srcFd < 0) {
            return (srcFd);
        }
        if (mySize > 0) {
            tmpSize = mySize;
        } else {
            tmpSize = srbObjDesc[destFd].objSize;
        }
        status = write1stHeaderToPortal (srcFd, tmpSize, 1);
        if (status < 0) {
            return (status);
        }
    } else {
        srcFd = portalConnect (clPort, clHostAddr);
        if (srcFd < 0) {
            return (srcFd);
        }
    }

    retVal = portalToSrbCopy (srcFd, destFd, mySize);
    close (srcFd);

    return (retVal);
}

srb_long_t
_svrDataPutToRescAndReg (int catType,
char *destObjID, char *destCollection, struct mdasResInfo *destResInfo,
char *clHostAddr, int clPort, srb_long_t mySize, int flag, int numThreads,
svrComm_t *myComm, char *inpPathName,
char *dataType, int regMode, srb_long_t registerdSize, 
int chksumFlag, int iChksum, char *fileGroup,
char *outFullPathName, int *objTypeInx, int *lockFd,
struct mdasResInfo **outCompResInfo)
{
    srb_long_t byteWritten;
    char compPathName[MAX_TOKEN];
    int intReplNum;
    srb_long_t retVal = 0;

    byteWritten = _svrDataPutToRescOrCompResc (MDAS_CATALOG,
      destObjID, destCollection, destResInfo,
      clHostAddr, clPort, mySize, flag, numThreads, myComm, inpPathName,
      outFullPathName, objTypeInx, lockFd, outCompResInfo);
    if (byteWritten < 0) {
	return (byteWritten);
    }

    compPathName[0] = '\0';             /* make _svrGenRegisterDataset
                                         * to generate a path */

    retVal = _svrGenRegisterDataset (MDAS_CATALOG, destObjID,
     destCollection, ClientUser->userName, ClientUser->domainName,
     regMode, destResInfo, *outCompResInfo, outFullPathName,
     byteWritten, dataType, *objTypeInx, NULL, NULL, registerdSize,
     compPathName, &intReplNum, lockFd);

    if (retVal < 0) {
	return (retVal);
    }

    if (chksumFlag) {
        char chksum[CHKSUM_STR_LEN + 1];

        if (*outCompResInfo != NULL) {
            /* a compound object */

            retVal = _objChksumAndReg (*objTypeInx, 0, chksum, 0, 0,
             destObjID, destCollection,
             outFullPathName, (*outCompResInfo)->phyResLoc, 
	     chksumFlag, iChksum,
             destResInfo->phyResName, compPathName, -1);
        } else {
            retVal = _objChksumAndReg (*objTypeInx, 0, chksum, 0, 0,
             destObjID, destCollection, outFullPathName,
             destResInfo->phyResLoc, chksumFlag, iChksum,
             destResInfo->phyResName, outFullPathName, -1);
        }
    }

    if (retVal < 0) {
        return (retVal);
    }

    if (srbObjTypeEntry[*objTypeInx].objType == FILE_TYPE &&
     fileGroup != NULL && strlen (fileGroup) > 0) {
        if (*outCompResInfo != NULL) {
            /* a compound object */
	    retVal = _svrFileChown (srbObjTypeEntry[*objTypeInx].systemType, 
	    (*outCompResInfo)->phyResLoc, outFullPathName,
	    NULL, fileGroup, 0);
	} else {
	    retVal = _svrFileChown (srbObjTypeEntry[*objTypeInx].systemType, 
	    destResInfo->phyResLoc, outFullPathName,
	    NULL, fileGroup, 0);
	}
    }

    if (retVal < 0) {
        return (retVal);
    } else {
 	return (byteWritten);
    }
}
