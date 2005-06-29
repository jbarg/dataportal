/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* distrib.c - Distributed SRB codes.
 */

#include "distrib.h"

#ifdef FED_MCAT
int
chkHostAndZone (char *hostName, struct hostElement  **hostTabPtr)
{
    struct hostElement *tmpHElement;
    struct hostElement *zoneHostTab = NULL;
    struct nameElement *tmpNElement, *localNElement;
    char mcatName[MAX_TOKEN], tmpAddr[MAX_TOKEN],
      addr[MAX_TOKEN], portNum[MAX_TOKEN];
    int remoteFlag;

    /* parse the mcatName and address */
    
    if (hostName == NULL || *hostName == '\0') {
	*hostTabPtr = LocalHostHead;
	return LOCAL_HOST;	/* Assume it is local */
    }

    if (mySplitStr (hostName, mcatName, tmpAddr, '&') == NULL) {
        elog (NOTICE, "chkHostAndZone: no mcatName for %s, using LocaMcat",
         hostName);
        strcpy (mcatName, LocalMcat->serverAuthInfo->mcatName);
        strcpy (tmpAddr, hostName);
	zoneHostTab = LocalMcat;
    } else if (strcmp (mcatName, "defaultMcat") == 0) {
	strcpy (mcatName, LocalMcat->serverAuthInfo->mcatName);
        zoneHostTab = LocalMcat;
    } else {
        remoteFlag = getMdasEnabledHost (mcatName, &zoneHostTab);
	if (remoteFlag < 0) {
            zoneHostTab = LocalMcat;
        }
    }

    mySplitStr (tmpAddr, addr, portNum, ':');

    remoteFlag = chkHost (addr, zoneHostTab->portNum, hostTabPtr);

#ifdef foo
    /* Go through the local host queue first */

    tmpHElement = LocalHostHead;

    while (tmpHElement != NULL) {
	/* Check the port first. Don't check if portNum == NULL */

        if (tmpHElement->zoneHostTab != NULL && 
	 tmpHElement->zoneHostTab->serverAuthInfo != NULL &&
	 tmpHElement->zoneHostTab->serverAuthInfo->mcatName != NULL) {
           if (strcmp (tmpHElement->zoneHostTab->serverAuthInfo->mcatName,
	     mcatName) != 0) {
                tmpHElement = tmpHElement->next;
                continue;
            } else {
		zoneHostTab = tmpHElement->zoneHostTab;
            }
	} else {
	    tmpHElement = tmpHElement->next;
	    continue;
	}

	/* Now check the name */

	tmpNElement = tmpHElement->alias;
	while (tmpNElement != NULL) {
	    if (strcasecmp (tmpNElement->hostName, addr) == 0) { 
	    	*hostTabPtr = tmpHElement;
	    	return LOCAL_HOST;
	    }
	    tmpNElement = tmpNElement->next;
	}
	tmpHElement = tmpHElement->next;
    }

    /* Now go through the remote host queue */

    tmpHElement = RemoteHostHead;

    while (tmpHElement != NULL) {
        /* Check the port first. Don't check if portNum == NULL */
 
       if (tmpHElement->zoneHostTab != NULL &&
         tmpHElement->zoneHostTab->serverAuthInfo != NULL &&
         tmpHElement->zoneHostTab->serverAuthInfo->mcatName != NULL) {
           if (strcmp (tmpHElement->zoneHostTab->serverAuthInfo->mcatName,
             mcatName) != 0) {
                tmpHElement = tmpHElement->next;
                continue;
            } else {
                zoneHostTab = tmpHElement->zoneHostTab;
            }
        } else {
            tmpHElement = tmpHElement->next;
            continue;
        }
 
        /* Now check the name */
 
        tmpNElement = tmpHElement->alias;
        while (tmpNElement != NULL) {
            if (strcasecmp (tmpNElement->hostName, addr) == 0) {
                *hostTabPtr = tmpHElement;
                return REMOTE_HOST;
            }
            tmpNElement = tmpNElement->next;
        }
        tmpHElement = tmpHElement->next;
    }
#endif

    if (remoteFlag < 0 && SvrInitialized == 1 && zoneHostTab != NULL) {	
	/* srbServer has been initialized. Add  the host to the table because
	 * Local MCAT may not be aware of this host. It must be a remote
	 * zone though because we should know locally */
	    char *myHost, *myPort;
	myHost = strdup (addr);
	myPort = strdup (zoneHostTab->portNum);
	tmpHElement = enqueHost (myHost, myPort, &RemoteHostHead);
	tmpHElement->zoneHostTab = zoneHostTab;
	*hostTabPtr = tmpHElement;
        remoteFlag = REMOTE_HOST;
    }

    if (remoteFlag == REMOTE_HOST && 
     atoi ((*hostTabPtr)->portNum) == atoi (LocalHostHead->portNum)) {
	/* a sanity check to make sure we don't get into an infinite
	 * connect loop */
        tmpNElement = (*hostTabPtr)->alias;
        while (tmpNElement != NULL) {
            localNElement = LocalHostHead->alias;
	    while (localNElement != NULL) {
                if (strcasecmp (tmpNElement->hostName, 
		 localNElement->hostName) == 0) {
		    remoteFlag = LOCAL_HOST;
		    tmpNElement = NULL;
		    break;
		}
		localNElement = localNElement->next;
            }
            tmpNElement = tmpNElement->next;
        }
    } 

    if (remoteFlag < 0) {
        *hostTabPtr = NULL;
        return INP_ERR_HOST_ADDR;
    } else {
	return remoteFlag;
    }
}
#endif	/* FED_MCAT */

int
chkHost (char *hostName, char *portNum, struct hostElement  **hostTabPtr)
{
    struct hostElement *tmpHElement;
    struct nameElement *tmpNElement;

    if (hostName == NULL || *hostName == '\0') {
	*hostTabPtr = LocalHostHead;
	return LOCAL_HOST;	/* Assume it is local */
    }

    /* Go through the local host queue first */

    tmpHElement = LocalHostHead;

    while (tmpHElement != NULL) {
	/* Check the port first. Don't check if portNum == NULL */

	if (portNum != NULL) {
#ifdef foo
            if (tmpHElement->zoneHostTab != NULL) {
                if (strcmp (tmpHElement->zoneHostTab->portNum, portNum) != 0) {
                    tmpHElement = tmpHElement->next;
                    continue;
                }
            } else {
                tmpHElement = tmpHElement->next;
                continue;
            }
#else
	    if (atoi (tmpHElement->portNum) != atoi (portNum)) {
		tmpHElement = tmpHElement->next;
		continue;
	    }
#endif
	}
	/* Now check the name */

	tmpNElement = tmpHElement->alias;
	while (tmpNElement != NULL) {
	    if (strcasecmp (tmpNElement->hostName, hostName) == 0) { 
	    	*hostTabPtr = tmpHElement;
	    	return LOCAL_HOST;
	    }
	    tmpNElement = tmpNElement->next;
	}
	tmpHElement = tmpHElement->next;
    }

    /* Now go through the remote host queue */

    tmpHElement = RemoteHostHead;

    while (tmpHElement != NULL) {
        /* Check the port first. Don't check if portNum == NULL */
 
        if (portNum != NULL) {
#ifdef foo
            if (tmpHElement->zoneHostTab != NULL) {
                if (strcmp (tmpHElement->zoneHostTab->portNum, portNum) != 0) {
                    tmpHElement = tmpHElement->next;
                    continue;
                }
            } else {
                tmpHElement = tmpHElement->next;
                continue;
            }
#else
            if (strcmp (tmpHElement->portNum, portNum) != 0) {
		tmpHElement = tmpHElement->next;
                continue;
	    }
#endif
        }
 
        /* Now check the name */
 
        tmpNElement = tmpHElement->alias;
        while (tmpNElement != NULL) {
            if (strcasecmp (tmpNElement->hostName, hostName) == 0) {
                *hostTabPtr = tmpHElement;
                return REMOTE_HOST;
            }
            tmpNElement = tmpNElement->next;
        }
        tmpHElement = tmpHElement->next;
    }

    *hostTabPtr = NULL;
    return INP_ERR_HOST_ADDR;
}
    
char *
getAddrWithZone (char *addr, char *mcatHostHint)
{
    char *myAddr, *myPort;
    struct hostElement  *hostTabPtr;
    int remoteFlag;

    if (addr == NULL)
	return (NULL);

    myAddr = malloc (strlen (addr) + 100);


    remoteFlag = getMdasEnabledHost (mcatHostHint, &hostTabPtr);

    if (remoteFlag < 0) {
	if (mcatHostHint != NULL) {
	  fprintf (stderr, 
	  "getAddrWithZone: getMdasEnabledHost err: mcatHint %s and addr %s\n",
	    mcatHostHint, addr);
            strcpy  (myAddr, addr);
	}
    } else {
	sprintf (myAddr, "%s&%s", hostTabPtr->serverAuthInfo->mcatName, addr);
    }

    return (myAddr);
}

int
prtLocalHost () {
struct hostElement *tmpHElement;
struct nameElement *tmpNElement;

    tmpHElement = LocalHostHead;

#ifdef _WIN32
    elog(NOTICE,"LocalHostName: ");
#else
    fprintf (stderr, "LocalHostName: ");
#endif

    while (tmpHElement != NULL) {

        /* Now check the name */

        tmpNElement = tmpHElement->alias;
        while (tmpNElement != NULL) {
#ifdef _WIN32
            elog(NOTICE," %s,", tmpNElement->hostName);
#else
            fprintf (stderr, " %s,", tmpNElement->hostName);
#endif
            tmpNElement = tmpNElement->next;
        }
	if (tmpHElement->portNum != NULL)
#ifdef _WIN32
        elog(NOTICE," Port Num: %s.\n", tmpHElement->portNum);
#else
	    fprintf (stderr, " Port Num: %s.\n", tmpHElement->portNum);
#endif
	else
#ifdef _WIN32
        elog(WARN," No Port Number.\n");
#else
	    fprintf (stderr, " No Port Number.\n");
#endif
        tmpHElement = tmpHElement->next;
    }
    return (0);
}

int
remoteFileOpen (int storSysType, struct hostElement  *hostTabPtr, 
char *fileName, int flags, int mode, int chdirFlag)
{
    int fd;
    int status;

    if (hostTabPtr == NULL) {
	elog (NOTICE, "remoteFileOpen: Invalid hostTabPtr");
	return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
	return status;
    }

    /* Do a client call to the remote host */

    fd = _srbFileOpen (hostTabPtr->conn, storSysType, NULL, fileName, flags, 
      mode, chdirFlag);

    if (fd < 0) {	/* Failure */
	elog (NOTICE, "Remote open failed: %s", 
	  clErrorMessage(hostTabPtr->conn));
    }

    return fd;
}

int
remoteFileOpenUni (int storSysType, struct hostElement  *hostTabPtr, 
char *fileName, int flags, int mode, int chdirFlag, srbConn **conn)
{
    int fd;

    if (hostTabPtr == NULL) {
        elog (NOTICE, "remoteFileCreate: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }

    *conn = svrConnect (hostTabPtr->alias->hostName, hostTabPtr->portNum, NULL,
      0, hostTabPtr);

    if (clStatus(*conn) != CLI_CONNECTION_OK) {
        elog (NOTICE,"Remote connect failed: %s",clErrorMessage(*conn));
        return clStatus(*conn);
    }

    /* Do a client call to the remote host */

    fd = _srbFileOpen (*conn, storSysType, NULL, fileName, flags, 
      mode, chdirFlag);

    if (fd < 0) {	/* Failure */
	elog (NOTICE, "Remote open failed: %s", 
	  clErrorMessage(*conn));
    }

    return fd;
}

int
remoteFileCreate (int storSysType, struct hostElement  *hostTabPtr, 
char *fileName, int mode, int chdirFlag, srb_long_t size)
{
    int fd;
    int status;

    if (hostTabPtr == NULL) {
	elog (NOTICE, "remoteFileCreate: Invalid hostTabPtr");
	return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    fd = _srbFileCreate (hostTabPtr->conn, storSysType, NULL, fileName, 
      mode, chdirFlag, size);

    if (fd < 0) {	/* Failure */
	elog (NOTICE, "Remote create failed: %s", 
	  clErrorMessage(hostTabPtr->conn));
    }

    return fd;
}

int
remoteFileCreateUni (int storSysType, struct hostElement  *hostTabPtr, 
char *fileName, int mode, int chdirFlag, srb_long_t size, srbConn **conn)
{
    int fd;

    if (hostTabPtr == NULL) {
        elog (NOTICE, "remoteFileCreate: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }

    *conn = svrConnect (hostTabPtr->alias->hostName, hostTabPtr->portNum, NULL,
      0, hostTabPtr);

    if (clStatus(*conn) != CLI_CONNECTION_OK) {
        elog (NOTICE,"Remote connect failed: %s",clErrorMessage(*conn));
        return clStatus(*conn);
    }

    /* Do a client call to the remote host */

    fd = _srbFileCreate (*conn, storSysType, NULL, fileName, 
      mode, chdirFlag, size);

    if (fd < 0) {	/* Failure */
	elog (NOTICE, "Remote create failed: %s", 
	  clErrorMessage(*conn));
    }

    return fd;
}

int
remoteFileUnlink (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int chdirFlag)
{
    int status;
 
    if (hostTabPtr == NULL) {
        elog (NOTICE, "remoteFileUnlink: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }
 
    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }
 
    /* Do a client call to the remote host */
 
    status = _srbFileUnlink (hostTabPtr->conn, storSysType, NULL, fileName,
      chdirFlag);
 
    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote Unlink failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }
 
    return status;
}

int
remoteFileStat (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, struct srbStat *retStat)
{
    int status;
 
    if (hostTabPtr == NULL) {
        elog (NOTICE, "remoteFileStat: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }
 
    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }
 
    /* Do a client call to the remote host */
 
    status = srbFileStat (hostTabPtr->conn, storSysType, NULL, fileName,
      retStat);
 
    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote Stat failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }
 
    return status;
}

int
remoteFileFstat (int storSysType, struct hostElement  *hostTabPtr,
int fd_inx, struct srbStat *retStat)
{
    int status;

    if (hostTabPtr == NULL) {
        elog (NOTICE, "remoteFileFstat: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbFileFstat (hostTabPtr->conn, storSysType, NULL, fd_inx,
      retStat);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote Fstat failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return status;
}

int
remoteOpendir (int storSysType, struct hostElement  *hostTabPtr,
char *dirName, int chdirFlag)
{
    int fd;
    int status;
 
    if (hostTabPtr == NULL) {
        elog (NOTICE, "remoteOpendir: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }
 
    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }
 
    /* Do a client call to the remote host */
 
    fd = _srbOpendir (hostTabPtr->conn, storSysType, NULL, dirName,
      chdirFlag);
 
    if (fd < 0) {       /* Failure */
        elog (NOTICE, "Remote Opendir failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }
 
    return fd;
}
 
int
remoteFileMkdir (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int mode, int chdirFlag)
{
    int status;
 
    if (hostTabPtr == NULL) {
        elog (NOTICE, "remoteFileMkdir: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }
 
    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }
 
    /* Do a client call to the remote host */
 
    status = _srbFileMkdir (hostTabPtr->conn, storSysType, NULL, fileName,
      mode, chdirFlag);
 
    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote Mkdir failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }
 
    return status;
}

int
remoteFileRmdir (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int chdirFlag)
{
    int status;
 
    if (hostTabPtr == NULL) {
        elog (NOTICE, "remoteFileRmdir: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }
 
    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }
 
    /* Do a client call to the remote host */
 
    status = _srbFileRmdir (hostTabPtr->conn, storSysType, NULL, fileName,
      chdirFlag);
 
    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote rmdir failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }
 
    return status;
}

int
remoteFileChmod (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int mode, int chdirFlag)
{
    int status;
 
    if (hostTabPtr == NULL) {
        elog (NOTICE, "remoteFileChmod: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }
 
    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }
 
    /* Do a client call to the remote host */
 
    status = _srbFileChmod (hostTabPtr->conn, storSysType, NULL, fileName,
      mode, chdirFlag);
 
    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote chmod failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }
 
    return status;
}

int
remoteDbLobjOpen (int storSysType, struct hostElement  *hostTabPtr, 
char *resourceLoc, char *dataPath, int flags, int mode)
{
    int fd;
    int status;

    if (hostTabPtr == NULL) {
	elog (NOTICE, "remoteDbLobjOpen: Invalid hostTabPtr");
	return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    fd = srbDbLobjOpen (hostTabPtr->conn, storSysType, resourceLoc, 
      dataPath, flags, mode);

    if (fd < 0) {	/* Failure */
	elog (NOTICE, "Remote open failed: %s", 
	  clErrorMessage(hostTabPtr->conn));
    }

    return fd;
}

int
remoteDbLobjCreate (int storSysType, struct hostElement  *hostTabPtr, 
char *resourceLoc, char *dataPath, int mode)
{
    int fd;
    int status;

    if (hostTabPtr == NULL) {
	elog (NOTICE, "remoteDbLobjCreate: Invalid hostTabPtr");
	return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    fd = srbDbLobjCreate (hostTabPtr->conn, storSysType, 
      resourceLoc, dataPath, mode);

    if (fd < 0) {	/* Failure */
	elog (NOTICE, "Remote create failed: %s", 
	  clErrorMessage(hostTabPtr->conn));
    }

    return fd;
}

int
remoteDbLobjUnlink (int storSysType, struct hostElement  *hostTabPtr,
char *resourceLoc, char *dataPath)
{
    int fd;
    int status;

    if (hostTabPtr == NULL) {
        elog (NOTICE, "remoteDbLobjUnlink: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    fd = srbDbLobjUnlink (hostTabPtr->conn, storSysType, resourceLoc,
      dataPath);

    if (fd < 0) {       /* Failure */
        elog (NOTICE, "Remote unlink failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return fd;
}



int
remoteDbTableOpen (int storSysType, struct hostElement  *hostTabPtr, 
char *resourceLoc, char *dataPath, int flags, int mode)
{
    int fd;
    int status;

    if (hostTabPtr == NULL) {
	elog (NOTICE, "remoteDbTableOpen: Invalid hostTabPtr");
	return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    fd = srbDbTableOpen (hostTabPtr->conn, storSysType, resourceLoc, 
      dataPath, flags, mode);

    if (fd < 0) {	/* Failure */
	elog (NOTICE, "Remote open failed: %s", 
	  clErrorMessage(hostTabPtr->conn));
    }

    return fd;
}

int
remoteDbTableCreate (int storSysType, struct hostElement  *hostTabPtr, 
char *resourceLoc, char *dataPath, int mode)
{
    int fd;
    int status;

    if (hostTabPtr == NULL) {
	elog (NOTICE, "remoteDbTableCreate: Invalid hostTabPtr");
	return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    fd = srbDbTableCreate (hostTabPtr->conn, storSysType, 
      resourceLoc, dataPath, mode);

    if (fd < 0) {	/* Failure */
	elog (NOTICE, "Remote create failed: %s", 
	  clErrorMessage(hostTabPtr->conn));
    }

    return fd;
}

int
remoteDbTableUnlink (int storSysType, struct hostElement  *hostTabPtr,
char *resourceLoc, char *dataPath)
{
    int fd;
    int status;

    if (hostTabPtr == NULL) {
        elog (NOTICE, "remoteDbTableUnlink: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    fd = srbDbTableUnlink (hostTabPtr->conn, storSysType, resourceLoc,
      dataPath);

    if (fd < 0) {       /* Failure */
        elog (NOTICE, "Remote unlink failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return fd;
}


/* svrGetHostConfig - Entry point for the srbGetHostConfig SID. Get the
 * SRB host configuration.
 *
 * Input : None
 *
 * Output : Returns a string containing the SRB host configuration
 */
 
struct varlena *
svrGetHostConfig ()
{
    struct hostElement *tmpHElement;
    int resLen = sizeof (INT32);  /* The length required. The first 4 bytes
				   * contains the number hosts to return */
    int numHost = 0;
    char *ptr;          /* a temp char pointer */
    struct varlena *retVal;     /* the return pointer */
#if defined(PORTNAME_c90)
    struct int32Array *iptr;
#else
    int *iptr;
#endif


    /* Go through the local host queue first */

    tmpHElement = LocalHostHead;

    while (tmpHElement != NULL) {
	numHost ++;
	resLen += strlen (tmpHElement->alias->hostName) + 1;
	resLen += 
	  strlen (tmpHElement->portNum) + 3;	/* two more for mdasEnabled */
	
	tmpHElement = tmpHElement->next;
    }

    /* Now go through the remote host queue */

    tmpHElement = RemoteHostHead;

    while (tmpHElement != NULL) {
        numHost ++;
        resLen += strlen (tmpHElement->alias->hostName) + 1;
        resLen += 
          strlen (tmpHElement->portNum) + 3;    /* two more for mdasEnabled */
 
        tmpHElement = tmpHElement->next;
    }

    retVal = (struct varlena *)malloc(VAROUTHDRSZ + resLen);
    VARSIZE(retVal) = resLen + VAROUTHDRSZ;
    VAROUTSTAT(retVal) = 0;	/* status */
#if defined(PORTNAME_c90)
    iptr = (struct int32Array *) VAROUTDATA(retVal);
    iptr->myInt0 = htonl (numHost);
#else
    iptr = (int *) VAROUTDATA(retVal);
    *iptr = htonl (numHost);
#endif	/* PORTNAME_c90 */
    ptr = (char *) VAROUTDATA(retVal) + SIZEOF32;

    tmpHElement = LocalHostHead;
 
    while (tmpHElement != NULL) {
        strcpy (ptr, tmpHElement->alias->hostName);
	ptr += strlen (tmpHElement->alias->hostName);
	*ptr = '\0';
	ptr ++;

        strcpy (ptr, tmpHElement->portNum);
        ptr += strlen (tmpHElement->portNum);
        *ptr = '\0';
        ptr ++;
 
	if (tmpHElement->mdasEnabled == 1) {
	    strcpy (ptr, "1");
	} else {
	    strcpy (ptr, "0");
	}
	ptr ++;
	*ptr = '\0';
        ptr ++;
        tmpHElement = tmpHElement->next;
    }
 
    /* Now go through the remote host queue */
 
    tmpHElement = RemoteHostHead;

    while (tmpHElement != NULL) {
        strcpy (ptr, tmpHElement->alias->hostName);
        ptr += strlen (tmpHElement->alias->hostName);
        *ptr = '\0';
        ptr ++;
 
        strcpy (ptr, tmpHElement->portNum);
        ptr += strlen (tmpHElement->portNum);
        *ptr = '\0';
        ptr ++;
 
        if (tmpHElement->mdasEnabled == 1) {
            strcpy (ptr, "1");
        } else {
            strcpy (ptr, "0");
        }
        ptr ++;
        *ptr = '\0';
        ptr ++;
        tmpHElement = tmpHElement->next;
    }
    return retVal;
}

int
remoteRegisterDataset (int catType, char *objID, char *userName,
char *access, char *domainName, char *dataType, 
char *pathName,   char *resourceName, char *collectionName,  
srb_long_t dataSize, int rmObjTypeInx, char *rmObjResLoc,
struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbRegDatasetInternal (hostTabPtr->conn, catType, objID,
      userName, access, domainName,
      dataType, resourceName, collectionName, pathName, dataSize,
      rmObjTypeInx, rmObjResLoc);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote RegisterDataset failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int
remoteRegisterInContDataset (int catType, char *objID, char *collectionName,
char *userName, char *domainName, char *containerName,
char *dataType, srb_long_t dataSize, srb_long_t baseOffset, 
struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbRegisterInContDataset (hostTabPtr->conn, catType, objID,
      collectionName, containerName, dataType, dataSize, baseOffset);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote RegisterInContDataset failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int
remoteRegisterContainer (int catType, char *containerName, char *userName,
char *domainName, char *logResName, srb_long_t containerSize,
struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbRegisterContainer (hostTabPtr->conn, catType, containerName,
      logResName, containerSize);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote RegisterContainer failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int
remoteModifyDataset (int catType, char *objID, char *collectionName,
char *pathName, 
char *resourceName, char *dataValue1, char *dataValue2, int retractionType,
char *userName, struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbModifyDataset (hostTabPtr->conn, catType, objID,
      collectionName,
	resourceName, pathName, dataValue1, dataValue2, retractionType);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote ModifyDataset failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int
remoteReplDataset (int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName, struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbObjReplicate (hostTabPtr->conn, catType, objID,
      collectionName, newResourceName, newPathName);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteReplDataset failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int
remoteMoveDataset (int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName, struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbObjMove (hostTabPtr->conn, catType, objID,
      collectionName, newResourceName, newPathName);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteMoveDataset failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int
remoteCreateCollect (int catType, char *parentCollect,
char *newCollect, struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbCreateCollect (hostTabPtr->conn, catType,
      parentCollect, newCollect);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteCreateCollect failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int
remoteListCollect (int catType, char *collectionName,
char *flag, mdasC_sql_result_struct **myresult, int rowsWanted,
struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    *myresult = (mdasC_sql_result_struct  *)
      malloc (sizeof (mdasC_sql_result_struct));
    memset (*myresult, 0, sizeof (mdasC_sql_result_struct));
    status = srbListCollect (hostTabPtr->conn, catType,
      collectionName, flag, *myresult, rowsWanted);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteListCollect failed: %s",
          clErrorMessage(hostTabPtr->conn));
	free (*myresult);
	*myresult = NULL;
    }

    return (status);
}

int
remoteModifyCollect (int catType, char *collectionName,
char *dataValue1, char *dataValue2, char *dataValue3, int retractionType,
struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbModifyCollect (hostTabPtr->conn, catType,
      collectionName, dataValue1, dataValue2, dataValue3, retractionType);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteModifyCollect failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int
remoteRegisterReplica (int catType, char *objID, char *collectionName,
          char *origResourceName, char *origPathName, 
	   char *newResourceName, char *newPathName,
            char *userName, char *domainName, struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbRegisterReplica (hostTabPtr->conn, catType, objID,
      collectionName, origResourceName, origPathName, newResourceName, 
	newPathName, userName, domainName);


    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote RegisterReplica failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int
svrChkMdasAuth (struct varlena *vUserName, struct varlena *vSrbAuth,
struct varlena *vMdasDomain)
{
    char *userName;
    char *srbAuth;
    char *mdasDomain;
    int status;

    userName = varToStr (vUserName);
    srbAuth = varToStr (vSrbAuth);
    mdasDomain = varToStr (vMdasDomain);

    status = _svrChkMdasAuth (userName, srbAuth, mdasDomain);

    /* KESTOCK - patch 0004 - mem leak */
    free (userName);
    free (srbAuth);
    free (mdasDomain);

    return (status);
}

int _svrChkMdasAuth (char *userName, char *srbAuth, char *mdasDomain)
{
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        if ((status = get_mdas_authorization (userName, srbAuth, 
	  mdasDomain)) < 0) {
            elog (NOTICE, 
	      "svrChkMdasAuth: user/passwd MDAS authentication failed");
        }
#endif
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            return status;
        }

        /* Do a client call to the remote host */

    	status = srbChkMdasAuth (hostTabPtr->conn, userName, srbAuth, 
	  mdasDomain);

        if (status < 0) {       /* Failure */
            elog (NOTICE, "svrChkMdasAuth failed: %s",
              clErrorMessage(hostTabPtr->conn));
	}
    }


    return (status);
}

int
svrChkEncryptAuth (struct varlena *vUserName, struct varlena *vMdasDomain, 
		   int iLen1, int iLen2, 
		   struct varlena *vBuf1,
		   struct varlena *vBuf2)
{
  char *userName;
  char *srbAuth;
  char *mdasDomain;
  int status; 
  char *cpBuf1, *cpBuf2;
  
  userName = varToStr (vUserName);
  mdasDomain = varToStr (vMdasDomain);
  cpBuf1 = varToStr (vBuf1);
  cpBuf2 = varToStr (vBuf2);
  
  status = _svrChkEncryptAuth (userName, mdasDomain, iLen1, iLen2, 
   cpBuf1, cpBuf2);

  free (userName);
  free (mdasDomain);
  free (cpBuf1);
  free (cpBuf2);

  return (status);
}

int
_svrChkEncryptAuth (char *userName, char *mdasDomain, int iLen1, int iLen2, 
char *cpBuf1, char *cpBuf2)
{
  char ReturnPassword[512];
  char *cpChallengeMessageBuf, *cpScrambledMsgFromClient, 
  *cpScrambledMsgOnServer;
  int status;


#ifdef SRB_MDAS
  cpChallengeMessageBuf = cpBuf1;
  cpScrambledMsgFromClient = cpBuf2;
  
  cpScrambledMsgOnServer = malloc(CHALLENGE_MESSAGE_SIZE);
  if (cpScrambledMsgOnServer == NULL) {
    elog(FATAL,"_svrChkEncryptAuth(), malloc(%d) failed");
    return(AUTH_ERR_ENCRYPT_AUTH);
  }
  status = iGetPasswordFromMcat(userName, &ReturnPassword[0], mdasDomain);
  if (status != MDAS_SUCCESS) {
    bzero(ReturnPassword, 512); /* kill the password ASAP */
    elog (NOTICE, 
     "ERROR _svrChkEncryptAuth(), iGetPasswordFromMcat error: %s@%s), stat=%d",
	  userName, mdasDomain, status);
    free(cpScrambledMsgOnServer);
    return(AUTH_ERR_ENCRYPT_AUTH);
  } else {
    vCrypt1(cpScrambledMsgOnServer, 
	    cpChallengeMessageBuf, ReturnPassword);
    bzero(ReturnPassword, 512);  /* kill the password ASAP */
    if(bcmp(cpScrambledMsgOnServer, cpScrambledMsgFromClient, 
     CHALLENGE_MESSAGE_SIZE) != 0) {
      elog(NOTICE,"svrChkEncryptAuth(): bcmp error for %s@%s",
      userName, mdasDomain);
      fflush(stderr);
      free(cpScrambledMsgOnServer);
      return (AUTH_ERR_ENCRYPT_AUTH);
    }
    free(cpScrambledMsgOnServer);
    return(STATUS_OK);
  }
#else
  status = (AUTH_ERR_ENCRYPT_AUTH);
  return(status);
#endif
}

int
svrChkMdasSysAuth (struct varlena *vUserName, struct varlena *vSrbAuth,
struct varlena *vMdasDomain)
{
    char *userName;
    char *srbAuth;
    char *mdasDomain;
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    userName = varToStr (vUserName);
    srbAuth = varToStr (vSrbAuth);
    mdasDomain = varToStr (vMdasDomain);

    remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);

    if (remoteFlag < 0) {
        status = remoteFlag;
    } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
#ifdef SRB_MDAS
        if ((status = get_mdas_sys_authorization (userName, srbAuth,
          mdasDomain)) < 0) {
            elog (NOTICE,
              "svrChkMdasSysAuth: user/passwd MDAS authentication failed");
            return status;
        } else {
            return(0);
        }
#endif
    } else {
        if ((status = remoteConnect (hostTabPtr)) < 0) {
            return status;
        }

        /* Do a client call to the remote host */

        status = srbChkMdasSysAuth (hostTabPtr->conn, userName, srbAuth,
          mdasDomain);

        if (status < 0) {       /* Failure */
            elog (NOTICE, "srbChkMdasSysAuth failed: %s",
              clErrorMessage(hostTabPtr->conn));
        }
    }

    return (status);
}

int
remoteGetPrivUsers (int catalog, mdasC_sql_result_struct **myresult, 
int rowsWanted, struct hostElement  *hostTabPtr)	
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }
 
    /* Do a client call to the remote host */
 
    *myresult = (mdasC_sql_result_struct  *)
      malloc (sizeof (mdasC_sql_result_struct));

    memset (*myresult, 0, sizeof (mdasC_sql_result_struct));

    status = srbGetPrivUsers (hostTabPtr->conn, catalog, *myresult, rowsWanted);
 
    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote GetPrivUsers failed: %s",
          clErrorMessage(hostTabPtr->conn));
        free (*myresult);
        *myresult = NULL;
    }
 
    return status;
}

int
remoteGetContainerInfo (int catalog, char *containerName,
char *userName, char*domainName, 
mdasC_sql_result_struct **myresult,
int rowsWanted, struct hostElement  *hostTabPtr)	
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }
 
    /* Do a client call to the remote host */
 
    *myresult = (mdasC_sql_result_struct  *)
      malloc (sizeof (mdasC_sql_result_struct));
    memset (*myresult, 0, sizeof (mdasC_sql_result_struct));

    status = srbGetContainerInfo (hostTabPtr->conn, catalog, containerName,
     *myresult, rowsWanted);
 
    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote GetContainerInfo failed: %s",
          clErrorMessage(hostTabPtr->conn));
        free (*myresult);
        *myresult = NULL;
    }
 
    return status;
}

int 
remoteGetMoreRows (int catalog, int contDesc, 
mdasC_sql_result_struct **myresult, int rowsWanted, 
struct hostElement *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }
 
    /* Do a client call to the remote host */
 
    *myresult = (mdasC_sql_result_struct  *)
      malloc (sizeof (mdasC_sql_result_struct));

    memset (*myresult, 0, sizeof (mdasC_sql_result_struct));

    status = srbGetMoreRows (hostTabPtr->conn, catalog, contDesc, 
      *myresult, rowsWanted);
 
    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote GetMoreRows failed: %s",
          clErrorMessage(hostTabPtr->conn));
        free (*myresult);
        *myresult = NULL;
    }
 
    return status;
}

int
remoteIssueTicket (char *objID, char *collectionName,
char *collectionFlag, char *beginTime, char *endTime, int accessCnt, 
char *ticketUser, char **ticket, struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbIssueTicket (hostTabPtr->conn, objID, collectionName,
      collectionFlag, beginTime, endTime, accessCnt, ticketUser, ticket);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteIssueTicket failed: %s",
          clErrorMessage(hostTabPtr->conn));
	*ticket = NULL;
    }

    return (status);
}

int
remoteRemoveTicket (char *ticket, struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbRemoveTicket (hostTabPtr->conn, ticket);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteRemoveTicket failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int prtVaultConf () {
    struct vaultQueElement *tmpElement;
    int len;

    if (vaultQueHead == NULL) {
#ifdef _WIN32
    elog(WARN,"Warning: No storage vault has been configured for the localhost.\n");
#else
	fprintf (stderr, 
	 "Warning: No storage vault has been configured for the localhost.\n");
#endif
	return (0);
    }

#ifdef _WIN32
    elog(NOTICE,"Local storage vault conf:\n");
#else
    fprintf (stderr, "Local storage vault conf:\n");
#endif

    tmpElement = vaultQueHead;

    while (tmpElement != NULL) {
#ifdef _WIN32
    elog(NOTICE,"storSysType: %d, vaultPath: %s\n",
     tmpElement->storSysType, tmpElement->vaultPath);
#else
        fprintf (stderr, "storSysType: %d, vaultPath: %s\n", 
	 tmpElement->storSysType, tmpElement->vaultPath);
#endif
        tmpElement = tmpElement->next;
    }
    return 0;
}

int
remoteGetResOnChoice (int catType, char *logResName,
char *phyResName, char *inputFlag, struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbGetResOnChoice (hostTabPtr->conn, catType, logResName,
      phyResName, inputFlag);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteGetResOnChoice failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int
remoteProxyOpr (struct hostElement  *hostTabPtr, ProxyOprId operation,
int inputInt1, int inputInt2, int inputInt3, int inputInt4,
char *inputStr1, char *inputStr2, char *inputStr3, char *inputStr4,
char *inputStr5, byteStream *inputBStrm1, byteStream *inputBStrm2,
char *outBuf)
{
    int status;

    if (hostTabPtr == NULL) {
	elog (NOTICE, "remoteProxyOpr: Invalid hostTabPtr");
	return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbObjProxyOpr (hostTabPtr->conn, operation,
     inputInt1, inputInt2, inputInt3, inputInt4,
      inputStr1, inputStr2, inputStr3, inputStr4,
       inputStr5, inputBStrm1, inputBStrm2, outBuf);

    return status;
}

int
remoteObjStat (int catType, char *path, int myType, struct srbStat *retStat,
struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */
 
    status = _srbObjStat (hostTabPtr->conn, catType, path, retStat, myType);
 
    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote Stat failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }
 
    return status;
}

int
remoteRegisterLocation(char *locName, char *fullAddr, char *parentLoc,
char *serverUser, char *serverUserDomain, struct hostElement  *hostTabPtr){
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbRegisterLocation (hostTabPtr->conn, locName,
                                fullAddr, parentLoc, serverUser,
                                serverUserDomain);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteRegisterLocation failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int
remoteInsertTokenInfo(char *typeName, char *newValue, char *parentValue,
struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbIngestToken (hostTabPtr->conn, typeName,
                                newValue, parentValue);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteInsertTokenInfo failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}


int
remoteRegisterResourceInfo(char *rescName, char *rescType,
                  char *location,
                  char *userName, char *userDomain,  char *userAuth,
                  char *phyPath, char *class, int size,
		  struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbRegisterResource (hostTabPtr->conn, rescName,
                                    rescType, location, phyPath,
                                    class, size);


    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteRegisterResourceInfo failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);

}

int
remoteRegisterLogicalResourceInfo (char *logRescName, char *rescType,
     char *physRescName,  char *physPath, struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbRegisterLogicalResource (hostTabPtr->conn, logRescName,
                                    rescType, physRescName, physPath);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteRegisterLogicalResourceInfo failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int
remoteRegisterReplicateResourceInfo (char *physRescName, char *rescType,
char *oldLogRescName, char *indefaultPath, struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbRegisterReplicateResourceInfo (hostTabPtr->conn, physRescName,
                                    rescType, oldLogRescName, indefaultPath);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteRegisterReplicateResourceInfo failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int 
remoteDeleteValue( int valueType, char *deleteValue, 
struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbDeleteValue (hostTabPtr->conn, valueType, deleteValue);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteDeleteValue failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

#ifdef foo	/* remoteSetupSessionPublicKey is not used */
int 
remoteSetupSessionPublicKey(char *publicKey)
{
    int status;
    struct hostElement  *hostTabPtr;

    hostTabPtr = getMdasEnabledHost ();
    if (hostTabPtr == NULL) {
        elog (NOTICE,
            "remoteSetupSession: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbSetupSessionPublicKey(hostTabPtr->conn, publicKey);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteSetupSessionPublicKey failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}
#endif

int 
remoteSetupSession(char *sessionKey, struct hostElement  *hostTabPtr)
{
    int status;

    if (hostTabPtr == NULL) {
        elog (NOTICE,
            "remoteSetupSession: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbSetupSession(hostTabPtr->conn, sessionKey);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteSetupSession failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}


int
remoteModifyZone (int catType, char *zoneName,
		  char *dataValue1, char *dataValue2, 
		  char *dataValue3, char *dataValue4,
		  char *dataValue5, int actionType,
		  struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbModifyZone (hostTabPtr->conn, catType,
      zoneName, dataValue1, dataValue2, dataValue3, dataValue4, 
      dataValue5, actionType);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteModifyZone failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

int 
remoteBulkQueryAnswer (int catType, 
				  char *queryInfo,
				  mdasC_sql_result_struct **myresult, 
				  int rowsWanted, 
				  struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }
 
    /* Do a client call to the remote host */
 
    *myresult = (mdasC_sql_result_struct  *)
      malloc (sizeof (mdasC_sql_result_struct));

    memset (*myresult, 0, sizeof (mdasC_sql_result_struct));

    status = srbBulkQueryAnswer (hostTabPtr->conn, catType, queryInfo, 
      *myresult, rowsWanted);
 
    if (status < 0) {       /* Failure */
        elog (NOTICE, "Remote BulkQueryAnswer failed: %s",
          clErrorMessage(hostTabPtr->conn));
        free (*myresult);
        *myresult = NULL;
    }
 
    return status;
}

int
remoteFileChksum (struct hostElement  *hostTabPtr, int fd_inx, 
int flags, unsigned long *ichksum, srb_long_t myOffset, srb_long_t mySize)
{
    int status;

    if (hostTabPtr == NULL) {
	elog (NOTICE, "remoteFileChksum: Invalid hostTabPtr");
	return SYS_ERR_HOST_ADDR;
    }

    if ((status = remoteConnect (hostTabPtr)) < 0) {
	return status;
    }

    /* Do a client call to the remote host */

    status = srbFileChksum (hostTabPtr->conn, fd_inx, 
     flags, ichksum, myOffset, mySize);

    if (status < 0) {	/* Failure */
	elog (NOTICE, "Remote operation failed: %s", 
	  clErrorMessage(hostTabPtr->conn));
    }

    return status;
}




int
remoteModifyUserNonPriv (int catType, char *userNameDomain,
		  char *dataValue1, char *dataValue2, 
		  char *dataValue3, char *dataValue4,
		  char *dataValue5, int actionType,
		  struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbModifyUserNonPriv (hostTabPtr->conn, catType,
      userNameDomain, dataValue1, dataValue2, dataValue3, dataValue4, 
      dataValue5, actionType);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteModifyUserNonPriv failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}



int
remoteModifyResource (int catType, char *resourceName,
		  char *dataValue1, char *dataValue2, 
		  char *dataValue3, char *dataValue4,
		   int actionType,
		  struct hostElement  *hostTabPtr)
{
    int status;

    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }

    /* Do a client call to the remote host */

    status = srbModifyResource (hostTabPtr->conn, catType,
      resourceName, dataValue1, dataValue2, dataValue3, dataValue4, 
      actionType);

    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteModifyResource failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }

    return (status);
}

srb_long_t
remoteGetFsFreeSpace (int storSysType, struct hostElement  *hostTabPtr,
char *path, int flag)
{
    srb_long_t status;
 
    if (hostTabPtr == NULL) {
        elog (NOTICE, "remoteGetFsFreeSpace: Invalid hostTabPtr");
        return SYS_ERR_HOST_ADDR;
    }
 
    if ((status = remoteConnect (hostTabPtr)) < 0) {
        return status;
    }
 
    /* Do a client call to the remote host */
 
    status = srbGetFsFreeSpace (hostTabPtr->conn, storSysType, NULL, 
     path, flag);
 
    if (status < 0) {       /* Failure */
        elog (NOTICE, "remoteGetFsFreeSpace failed: %s",
          clErrorMessage(hostTabPtr->conn));
    }
 
    return status;
}

