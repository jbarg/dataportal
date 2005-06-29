/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/*
 * srbServerMisc.c - Misc routines for the srbServer
 */

#include "srbServerMisc.h"
#include "proxyFunctionHandler.h"

/* initSrbServer - Initialize the SrbServer. Calls a number of initialization
 * routines to initialize data structures in various modules.
 *
 * Input - None
 *
 * Output - None
 */

int
initSrbServer()
{
#ifdef HPSS
#ifdef NO_DCE
    int status;
#endif
#endif
#if defined(HPSS) && defined(NO_DCE)
    char nodceUser[MAX_TOKEN], nodceAuth[MAX_TOKEN];
#endif

    /* seed the random number generator */

#ifdef _WIN32
    srand((unsigned int) getpid());
#else
    srandom((unsigned int) getpid());
#endif
    RandomSeed = 1;	/* srandom has been called */

#ifndef _WIN32
    debugFileOpen();
#endif

    exfinit ();

    initObjDesc();

    initDbLObjDesc();

    initDbTableDesc();

    initVault ();

    initMDriverDesc();



#ifdef HPSS
#ifdef NO_DCE
#if defined(PORTNAME_solaris) || defined(PORTNAME_aix)
    if ((status = readHpssNodceAuth (nodceUser, nodceAuth)) >= 0) {
        hpss_SetAuthType (API_AUTH_TYPE_DCECOMBO);
        if ((status = hpss_SetCombo (nodceUser, nodceAuth)) < 0) {
            fprintf (stderr,
             "initSrbServer: hpss_SetCombo error, errno = %d.\n", errno);
        }
    } else {
        fprintf (stderr, "initSrbServer: readHpssNodceAuth error.\n");
    }
#else	/* PORTNAME_linux */
    if ((status = readHpssNodceAuth (nodceUser, nodceAuth)) >= 0) {
	/* XXXXX API_AUTH_LOGIN_DCE instead of API_AUTH_TYPE_DCECOMBO ? */
        hpss_SetAuthType (API_AUTH_TYPE_DCECOMBO);
        if ((status = hpss_SetCombo (nodceUser, nodceAuth)) < 0) {
            fprintf (stderr,
             "initSrbServer: hpss_SetCombo error, errno = %d.\n", errno);
        }
    } else {
        fprintf (stderr, "initSrbServer: readHpssNodceAuth error.\n");
    }
#endif	/* PORTNAME_solaris */
#else	/* NO_DCE */
    if (getenv ("srbNoDCELogin") == NULL)
        SetLoginContext(NULL, NULL);
#endif  /* NO_DCE */
    initHpssCos ();
#endif	/* HPSS */

    /* set the StorVaultMode */

    if (getenv (STOR_VAULT_MODE_KEYWD) != NULL) {
	StorVaultMode = strtol (getenv (STOR_VAULT_MODE_KEYWD), 0, 0);
    } else {
	StorVaultMode = DEF_STOR_VAULT_MODE;
    }

    /* set up the MaxThread and SizePerThread variable */
    if (getenv (MAX_THREAD_ENV) != NULL) {
	MaxThread = atoi (getenv (MAX_THREAD_ENV));
	if (MaxThread <= 0)
	    MaxThread = DEF_MAX_THREAD;
	else if (MaxThread > MAX_THREAD)
	    MaxThread = MAX_THREAD;
    }
    if (getenv (SIZE_PER_THREAD_ENV) != NULL) {
        SizePerThread = atoi (getenv (SIZE_PER_THREAD_ENV));
        if (SizePerThread <= 0)  
            SizePerThread = DEF_SIZE_PER_THREAD;
	else
	    SizePerThread = SizePerThread * 1048576;
    }  

    /* KESTOCK - parent function already called initSrbHost */
    /** if ((status = initSrbHost()) != 0) {
     **   fprintf(stderr,
     **     "initSrbServer: initSrbHost Error. Status = %d\n",
     **       status);
     **   return status;
     **}
     **/

#ifdef TAPE_DRIVE
    initTapeDevConfig ();
    initTapeLibConfig();
#endif
    return 0;
}

int 
execCommand (char *command, char *commandArgv, int lsock, int portalFlag)
{
    char *av[MAX_TOKEN];
    char *inpPtr, *outPtr, commandBuf[MAX_TOKEN], envBuf[MAX_TOKEN];
    int inx = 1;
    int c;
    int len = 0;
    int openQuote = 0;

    sprintf (commandBuf, "%s/../%s/%s", 
     (char *) getSrbDataDir(), (char *) COMMAND_DIR, command);

    /* parse the commandArgv */

    av[0] = commandBuf;

    if (commandArgv != NULL) {
        if ((c == ' ' && openQuote == 0) || openQuote == 2) {
            openQuote = 0;
            if (len > 0) {      /* end of a argv */
                *inpPtr = '\0'; /* mark the end */
                av[inx] = outPtr;
                /* reset inx and pointer */
                inpPtr++;
                outPtr = inpPtr;
                inx ++;
                len = 0;
            } else {    /* skip over blanks */
                inpPtr++;
                outPtr = inpPtr;
            }
        } else if (c == '\'' || c == '\"') {
            openQuote ++;
            if (openQuote == 1) {
                /* skip the quote */
                inpPtr++;
                outPtr = inpPtr;
            }
        } else if (c == '\'' || c == '\"') {
            openQuote ++;
            if (openQuote == 1) {
                /* skip the quote */
                inpPtr++;
                outPtr = inpPtr;
            }
        } else {
            len ++;
            inpPtr++;
        }
    }

    av[inx] = NULL;	/* terminate with a NULL */ 

    /* close opened file desc opened by the srbServer */

    closeAllDesc ();            /* Close all opened obj descriptor */

    if (portalFlag & PORTAL_STD_IN_OUT) {
	int outFd;

        if (lsock != 0)
	    close (0);		/* close stdin */

        if (lsock != 1)
	    close (1);		/* close stdout */

        if (lsock != 2)
	    close (2);		/* close stderr */

        /* Setup portal as stdin, stdout and stderr ? */

	    dup (lsock);	/* inFd */
	    outFd = dup (lsock);
	    dup (lsock);	/* errFd */
	    close (lsock);
	    lsock = outFd;
    }
	
    /* setup the portal env */

    sprintf (envBuf, "%s=%-d", PORTAL_ENV, lsock);
    putenv (envBuf);
    return(execv(av[0], av));
}
    
func_ptr 
getFunctionCall(char *myFunc)
{

  int i;
  for (i=0;i<nProxyBuiltinFunct;i++) {
    if (!strcmp(functProxyBuiltin[i].funcName, myFunc))
      return(functProxyBuiltin[i].func);
  }
  return(NULL);
}

int 
execFunction(char *command, char *commandArgv, int lsock, int portalFlag)
{
    char *av[MAX_TOKEN];
    char *inpPtr, *outPtr;
    int inx = 1;
    int c;
    int len = 0;
    func_ptr fn;

    av[0] = command;
    inpPtr = strdup (commandArgv);
    outPtr = inpPtr;
    while ((c = *inpPtr) != '\0') {
	if (c == '|') {
	    if (len > 0) {	/* end of a argv */
		*inpPtr = '\0'; /* mark the end */
		av[inx] = outPtr;
		/* reset inx and pointer */
		inpPtr++;
		outPtr = inpPtr;
		inx ++;
		len = 0;
	    } else {	/* skip over blanks */
		inpPtr++;
		outPtr = inpPtr;
	    }
	} else {
	    len ++;
	    inpPtr++;
	}
    }
    if (len > 0) {      /* handle the last argv */  
        av[inx] = outPtr;
	inx++;
    }
next:
    av[inx] = NULL;	/* terminate with a NULL */ 

    /* close opened file desc opened by the srbServer */

    closeAllDesc ();            /* Close all opened obj descriptor */

    if (lsock != 0)
	close (0);		/* close stdin */

    if (lsock != 1)
	close (1);		/* close stdout */

    if (lsock != 2)
	close (2);		/* close stderr */

    /* Setup portal as stdin, stdout and stderr ? */

    if (portalFlag & PORTAL_STD_IN_OUT) {
	int outFd;
	    dup (lsock);		/* inFd */
	    outFd = dup (lsock);
	    dup (lsock);		/* errFd */
	    close (lsock);
	    lsock = outFd;
    }
	
    /* setup the portal env */
    /*
    sprintf (envBuf, "%s=%-d", PORTAL_ENV, lsock);
    putenv (envBuf);
    return(execv(av[0], av));
    **/
    fn =  getFunctionCall(av[0]);
    if (fn != NULL) 
#if defined(PORTNAME_aix)
    i = FUNCTION_CALL_PARAM_ERROR;
#else
    /* i =    (func_ptr *) (fn)((int) inx, (char **) av); */
    i =    (int) (fn)((int) inx, (char **) av);
#endif
    else
      i = FUNCTION_CALL_PARAM_ERROR;
    return(i);
}

int
svrCloseAll ()
{
    struct hostElement  *tmpHElement;

    if (ProxyUser != NULL && ProxyUser->myComm != NULL && 
     ProxyUser->myComm->commFd > 0) {
#ifdef SEA_AUTH
        seaClose(ProxyUser->myComm->commFd);
#else
        close(ProxyUser->myComm->commFd);
#endif
        ProxyUser->myComm->commFd = 0;
    }
    closeAllDesc ();

    /* close all remote client connection */

    tmpHElement = RemoteHostHead;

    while (tmpHElement != NULL) {
	if (tmpHElement->conn != NULL) {
	    if (tmpHElement->conn->Pfout) fclose(tmpHElement->conn->Pfout);
    	    if (tmpHElement->conn->Pfin)  fclose(tmpHElement->conn->Pfin);
    	    if (tmpHElement->conn->Pfdebug) fclose(tmpHElement->conn->Pfdebug);
	    tmpHElement->conn = NULL;
	}
        tmpHElement = tmpHElement->next;
    }

    return (0);
}

int
pingMCAT ()
{
    int status;

    if (LocalHostHead->mdasEnabled != 1)        /* not MCAT enabled */
        return 0;

    /* use initSrbUserList to ping */

    status = initSrbUserList ();
    if (status < 0) {
        /* try to restart the connection first */
#ifdef SRB_MDAS
        close_db2_interaction(MDAS_CAT_COMMIT);
        if (open_db2_interaction(MDAS_CATALOG) == 0) {
            return (SYS_MCAT_CONN_RESTART);
        } else {
            return (SYS_ERR_MCAT_SERVER_DOWN);
        }
#else
	return (SYS_ERR_MCAT_SERVER_DOWN);
#endif
    }

    return (status);
}

int
chkHostBasedAuth(struct sockaddr_in *addr, StartupInfo *sp)
{
    u_long ip_addr;
    static struct conf_line conf[MAX_LINES];
    static int nconf;
    int i;
    char openType[2];
    char buf[MAX_CONF_TOKEN];
    FILE *file;

    if (!HostBasedConFile) {
        HostBasedConFile = (char *) malloc((strlen(DataDir) +
        strlen(HOST_BASE_CONF_FILE)+2)*sizeof(char));
        strcpy(HostBasedConFile, DataDir);
        strcat(HostBasedConFile, "/");
        strcat(HostBasedConFile, HOST_BASE_CONF_FILE);
    }

    /* Open the config file. */

    strcpy (openType, "r");
    file = fopen(HostBasedConFile, openType);
    if (file) {
        nconf = 0;

        /* Get the "name" */
        while ((i = getNextStr(file, buf, sizeof(buf))) != EOF) {
            if (i == '\n') continue;

            /* Comment -- read until end of line then next line */
            if (buf[0] == '#') {
                while (getNextStr(file, buf, sizeof(buf)) == 0) ;
                continue;
            }

            /*
             * Check to make sure this says "all" or that it matches
             * the user name.
             */

            if (strcmp(buf, ALL_NAME) == 0 ||
              (strcmp(buf, sp->proxyUserName) == 0)) {

                /* Get next string, if last on line, ignore */
                if (getNextStr(file, buf, sizeof(buf)) != 0)
                    continue;

                /* Got address */

                conf[nconf].adr = inet_addr(buf);

                /* Get next string (mask) */

                i = getNextStr(file, buf, sizeof(buf));

                if (i != EOF) {
                    /* Add to list, quit if array is full */
                    conf[nconf++].mask = inet_addr(buf);
                    if (nconf == MAX_LINES) break;
                }

                /* If not at linefeed, keep reading until we are */
                while (i == 0)
                    i = getNextStr(file, buf, sizeof(buf));
            }
        }
        fclose(file);
    }
    else {
        (void) fprintf(stderr,
        "chkHostBasedAuth: Cannot open Auth config file %s\n",
          HostBasedConFile);
        return(STATUS_ERROR);
    }


    ip_addr = addr->sin_addr.s_addr;

    /*
     * Mask off the bits and compare the result * with the conf address.
     * A match means that this address is ok.
     */

    for (i = 0; i < nconf; ++i)
        if ((ip_addr & ~conf[i].mask) == conf[i].adr) return(STATUS_OK);

    /* No match */
    return(STATUS_ERROR);
}

/* This is the server version of srbDataStat */

int svrDataStat (int catType, char *dataName, char *collection,
mdasC_sql_result_struct **myresult)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i;
    int status;
#ifdef INSTRON
    hrtime_t beforeTime;
#endif

    if (dataName == NULL || collection == NULL)
        return -1;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[DATA_ID] = 1;
    selval[SIZE] = 1;
    selval[ACCESS_CONSTRAINT] = 1;

    selval[DATA_OWNER] = 1;
    selval[DATA_OWNER_DOMAIN] = 1;
    selval[USER_NAME] = 1;
    selval[DOMAIN_DESC] = 1;
    selval[REPL_TIMESTAMP] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[IS_DIRTY] = 1;

    sprintf(qval[DATA_GRP_NAME]," = '%s'", collection);
    sprintf(qval[DATA_NAME]," = '%s'", dataName);

#ifdef INSTRON
    beforeTime =  gethrtime ();
#endif
    status = queryDataInfo (collection, selval, qval, myresult, DEF_NUM_ROWS);

#ifdef INSTRON
    fprintf (stderr, "srbDataStat: query time = %lld\n",
     gethrtime () - beforeTime);
#endif

    return (status);
}


int svrCollStat (int catType, char *collection, 
mdasC_sql_result_struct **myresult)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i;
    int status;
#ifdef INSTRON
    hrtime_t beforeTime;
#endif

    if (collection == NULL || myresult == NULL)
        return -1;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[USER_NAME] = 1;
    selval[DOMAIN_DESC] = 1;
    selval[GROUP_ACCESS_CONSTRAINT] = 1;
    selval[COLL_MODIFY_TIMESTAMP] = 1;

    sprintf(qval[ACCESS_GROUP_NAME]," = '%s'", collection);

#ifdef INSTRON
    beforeTime =  gethrtime ();
#endif
    status = queryDataInfo (collection, selval, qval, myresult, DEF_NUM_ROWS);
#ifdef INSTRON
    fprintf (stderr, "srbCollStat: query time = %lld\n",
     gethrtime () - beforeTime);
#endif

    return (status);
}

int
htonStat (struct srbStat *retStat)
{

    if (retStat == NULL)
        return -1;

    /* convert to net format */
#if defined(PORTNAME_c90)
    retStat->st_size = (srb_size_t) htonl (retStat->st_size);
#else
    srbHtonll ((u_longlong_t *) &retStat->st_size);
#endif
    retStat->st_dev = (srb_dev_t) htonl (retStat->st_dev);
    retStat->st_ino = (srb_ino_t) htonl (retStat->st_ino);
    retStat->st_mode = (srb_mode_t) htonl (retStat->st_mode);
    retStat->st_nlink = (short) ntohs (retStat->st_nlink);
    retStat->st_uid = (srb_uid_t) htonl (retStat->st_uid);
    retStat->st_gid = (srb_gid_t) htonl (retStat->st_gid);
    retStat->st_rdev = (srb_dev_t) htonl (retStat->st_rdev);
    retStat->st_atim = (srb_time_t) htonl (retStat->st_atim);
    retStat->st_mtim = (srb_time_t) htonl (retStat->st_mtim);
    retStat->st_ctim = (srb_time_t) htonl (retStat->st_ctim);
    retStat->st_blksize = (srb_size_t) htonl (retStat->st_blksize);
    retStat->st_blocks = (srb_size_t) htonl (retStat->st_blocks);
    retStat->st_spare1 = (int) htonl (retStat->st_spare1);
    retStat->st_spare2 = (int) htonl (retStat->st_spare2);
    retStat->st_spare3 = (int) htonl (retStat->st_spare3);
    retStat->st_vfstype = (int) htonl (retStat->st_vfstype);
    retStat->st_vfs = (srb_size_t) htonl (retStat->st_vfs);
    retStat->st_type = (srb_size_t) htonl (retStat->st_type);
#ifdef PORTNAME_aix
    retStat->AIX_Reserved1 = (srb_uid_t) htonl (retStat->AIX_Reserved1);
    retStat->AIX_Reserved2 = (srb_gid_t) htonl (retStat->AIX_Reserved2);
    retStat->st_Access = (srb_ushort_t) htonl (retStat->st_Access);
#else
    retStat->Reserved1 = (srb_uid_t) htonl (retStat->Reserved1);
    retStat->Reserved2 = (srb_gid_t) htonl (retStat->Reserved2);
    retStat->st_access = (srb_ushort_t) htonl (retStat->st_access);
#endif
    retStat->st_flag = (srb_ushort_t) ntohs (retStat->st_flag);
    retStat->st_gen = (srb_size_t) htonl (retStat->st_gen);

    return 0;
}

/* int _svrGetdents - The server version of srbGetdents,
 * Output - The number of bytes returned in buf.
 */
int
_svrGetdents (int fd, void *buf, size_t nbyte, int is64Flag)
{
    int status = 0;
    int rowCnt = 0;
    char *nameList;
    int loopAgain;

    if (srbObjDesc[fd].collInfo == NULL) {
        elog(NOTICE, "_svrGetdents: opened fd=%d:objID=%s is not a collection",
         fd, srbObjDesc[fd].objID);
        return (OBJ_ERR_NOT_COLL);
    }

    do {

      loopAgain = 0;
      switch (srbObjDesc[fd].isDir) {
      case IS_DIR_1:
        /* new desc */
        rowCnt = svrGetDataInColl (srbObjDesc[fd].collection, DEF_NUM_ROWS,
         &nameList, &(srbObjDesc[fd].collInfo->continueIndex));
        if (rowCnt <= 0) {
            svrSetFdToStage3 (fd);
            loopAgain = 1;        /* try to list subcollection */
	    break;
        }
        srbObjDesc[fd].collInfo->gotData = 1;
	if (is64Flag == IS_64_BIT)
            status = svrResToDirent64 (nameList, rowCnt, fd, 1);
	else
            status = svrResToDirent (nameList, rowCnt, fd, 1);
        free (nameList);
        if (status <= 0) {
            svrSetFdToStage3 (fd);
            loopAgain = 1;        /* try to list subcollection */
            break;
        }

        if (is64Flag == IS_64_BIT)
            status = svrOutputDirent64 (fd, buf, nbyte);
        else
            status = svrOutputDirent (fd, buf, nbyte);
        if (status <= 0) {
            svrSetFdToStage3 (fd);
            loopAgain = 1;        /* try to list subcollection */
            break;
        }
        srbObjDesc[fd].isDir = IS_DIR_2;
        break;
      case IS_DIR_2:
        /* Continue listing data */

        if (srbObjDesc[fd].collInfo->maxDirLen > 
	 srbObjDesc[fd].collInfo->offset) {
            /* still more data */
            if (is64Flag== IS_64_BIT)
                status = svrOutputDirent64 (fd, buf, nbyte);
            else
                status = svrOutputDirent (fd, buf, nbyte);
            if (status > 0)
                break;
        }

        /* See if we can get more result */

        if (srbObjDesc[fd].collInfo->continueIndex >= 0)
            rowCnt = svrGetMoreData (&srbObjDesc[fd].collInfo->continueIndex, 
	     DEF_NUM_ROWS, DATA_NAME, &nameList);
        if (rowCnt <= 0) {
            svrSetFdToStage3 (fd);
            loopAgain = 1;        /* try to list subcollection */
            break;
        }
	if (is64Flag == IS_64_BIT)
            status = svrResToDirent64 (nameList, rowCnt, fd, 1);
	else
            status = svrResToDirent (nameList, rowCnt, fd, 1);
        free (nameList);
        if (status <= 0) {
            svrSetFdToStage3 (fd);
            loopAgain = 1;        /* try to list subcollection */
            break;
        }

        if (is64Flag == IS_64_BIT)
            status = svrOutputDirent64 (fd, buf, nbyte);
        else
            status = svrOutputDirent (fd, buf, nbyte);
        if (status <= 0) {
            svrSetFdToStage3 (fd);
            loopAgain = 1;        /* try to list subcollection */
            break;
        }
        break;
      case IS_DIR_3:
        /* done listing data. now try to list collection */

        rowCnt = svrGetCollInColl (srbObjDesc[fd].collection, DEF_NUM_ROWS,
         &nameList, &(srbObjDesc[fd].collInfo->continueIndex));

        if (rowCnt < 0)
            rowCnt = 0;
        if (rowCnt == 0 && srbObjDesc[fd].collInfo->gotData == 1)  {
            status = 0;
            break;
        }
        if (srbObjDesc[fd].collInfo->gotData == 1)
	    if (is64Flag == IS_64_BIT)
            	status = svrResToDirent64 (nameList, rowCnt, fd, 0);
	    else
            	status = svrResToDirent (nameList, rowCnt, fd, 0);
        else
	    if (is64Flag == IS_64_BIT)
            	status = svrResToDirent64 (nameList, rowCnt, fd, 1);
	    else
            	status = svrResToDirent (nameList, rowCnt, fd, 1);
        free (nameList);
        if (status <= 0) {
            break;
        }

        if (is64Flag== IS_64_BIT)
            status = svrOutputDirent64 (fd, buf, nbyte);
        else
            status = svrOutputDirent (fd, buf, nbyte);
        if (status <= 0) {
            break;
        }
        srbObjDesc[fd].isDir = IS_DIR_4;
        break;
      case IS_DIR_4:
        /* Continue listing Coll */

        if (srbObjDesc[fd].collInfo->maxDirLen > 
	 srbObjDesc[fd].collInfo->offset) {
            /* still more data */
            if (is64Flag== IS_64_BIT)
                status = svrOutputDirent64 (fd, buf, nbyte);
            else
                status = svrOutputDirent (fd, buf, nbyte);
            if (status > 0)
                break;
        }

        /* See if we can get more result */

        if (srbObjDesc[fd].collInfo->continueIndex >= 0)
            rowCnt = svrGetMoreData (&srbObjDesc[fd].collInfo->continueIndex, 
	     DEF_NUM_ROWS, DATA_GRP_NAME, &nameList);
        if (rowCnt <= 0) {
            break;
        }
	if (is64Flag == IS_64_BIT)
            status = svrResToDirent64 (nameList, rowCnt, fd, 0);
	else
            status = svrResToDirent (nameList, rowCnt, fd, 0);
        free (nameList);
        if (status <= 0) {
            break;
        }

        if (is64Flag== IS_64_BIT)
            status = svrOutputDirent64 (fd, buf, nbyte);
        else
            status = svrOutputDirent (fd, buf, nbyte);
            break;
      default:
        errno = EBADF;
        status = 0;
      }		/* switch */
    } while (loopAgain > 0);
    return status;
}

int
svrSetFdToStage3 (int fd)
{
    if (srbObjDesc[fd].collInfo == NULL) {
        elog(NOTICE, 
	 "svrSetFdToStage3: opened fd=%d:objID=%s is not a collection",
          fd, srbObjDesc[fd].objID);
        return (OBJ_ERR_NOT_COLL);
    }

    srbObjDesc[fd].isDir = IS_DIR_3;
    srbObjDesc[fd].collInfo->continueIndex = -1;
    return (0);
}

int
svrGetDataInColl (char *parColl, int maxRow, char **nameList,
int *continueIndex)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i;
    int status;
    mdasC_sql_result_struct *myresult;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[DATA_NAME] = 1;
    selval[DATA_ID] = 1;
    sprintf(qval[DATA_GRP_NAME]," = '%s'", parColl);

    status = queryDataInfo(parColl, selval, qval, &myresult, maxRow);

    if (status >= 0 && myresult->row_count > 0) {
        *nameList = (char *) getFromResultStruct (myresult,
         dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
        *continueIndex = myresult->continuation_index;
        return (myresult->row_count);
    } else {
        *nameList = NULL;
        *continueIndex = -1;
	return status;
    }
}

int
svrGetCollInColl (char *parColl, int maxRow, char **nameList, 
int *continueIndex)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i;
    int status;
    mdasC_sql_result_struct *myresult;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[DATA_GRP_NAME] = 1;
    sprintf(qval[PARENT_COLLECTION_NAME]," = '%s'", parColl);

    status = queryDataInfo (parColl, selval, qval, &myresult, maxRow);

    if (status >= 0 && myresult->row_count > 0) {
        *nameList = (char *) getFromResultStruct (myresult,
         dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
        *continueIndex = myresult->continuation_index;
        return (myresult->row_count);
    } else {
        *nameList = NULL;
        *continueIndex = -1;
	return status;
    }
}
   
/* svrResToDirent - The server version of srbResToDirent() - 
 * Convert the query result to dirent and return the length
 * of dirent.
 */

int
svrResToDirent (char *nameList, int rowCnt, int fd, int firstFlag)
{
    int i;
    dirent_t *mydirent;
    int mallocLen = 0;
    int direntLen = 0;
    int tmpLen;
    char lastName[MAX_TOKEN], tmpColl[MAX_TOKEN];

    if (srbObjDesc[fd].collInfo == NULL) {
        elog(NOTICE, 
	 "svrResToDirent: opened fd=%d:objID=%s is not a collection",
          fd, srbObjDesc[fd].objID);
        return (OBJ_ERR_NOT_COLL);
    }

    /* do some bookkeeping */

    if (srbObjDesc[fd].collInfo->maxDirLen > srbObjDesc[fd].collInfo->offset) {
        fprintf (stderr,
         "svrResToDirent: Internal error, maxDirLen > offset\n");
        return 0;
    }

    if (nameList == NULL && rowCnt > 0)
        return 0;

    srbObjDesc[fd].collInfo->accumOffset += srbObjDesc[fd].collInfo->maxDirLen;
    srbObjDesc[fd].collInfo->maxDirLen = srbObjDesc[fd].collInfo->offset = 0;
    if (srbObjDesc[fd].collInfo->dirent != NULL) {
        free (srbObjDesc[fd].collInfo->dirent);
        srbObjDesc[fd].collInfo->dirent = NULL;
    }


    /* estimate the no of bytes to alloc */

    for (i = 0; i < rowCnt; i++) {
        mallocLen += (strlen (&nameList[i*MAX_DATA_SIZE]) + 1) /8 * 8 + 8;
    }

    /* first time around, have to add "." and ".." */
    if (firstFlag == 1) {
        /* add 9 bytes for padding */
        mallocLen += (rowCnt + 2)  * (sizeof (dirent_t) + 8);
    } else {
        /* add 9 bytes for padding */
        mallocLen += rowCnt * (sizeof (dirent_t) + 8);
    }

    srbObjDesc[fd].collInfo->dirent = malloc (mallocLen);

    /* pack the dirent */

    mydirent = (dirent_t *) srbObjDesc[fd].collInfo->dirent;

    if (firstFlag == 1) {
        /* hand craft "." and ".." */
        mydirent->d_ino = FAKE_INODE + strlen (srbObjDesc[fd].collection);
        tmpLen = mydirent->d_reclen = sizeof (dirent_t);
        direntLen += tmpLen;
        mydirent->d_off = srbObjDesc[fd].collInfo->accumOffset + direntLen;
        strcpy (mydirent->d_name, ".");
        mydirent = (dirent_t *) ((char *) mydirent + tmpLen);

        mySplitPath (srbObjDesc[fd].collection, tmpColl, lastName);
        mydirent->d_ino = FAKE_INODE + strlen (tmpColl);
        tmpLen = mydirent->d_reclen = sizeof (dirent_t) + 8;
        direntLen += tmpLen;
        mydirent->d_off = srbObjDesc[fd].collInfo->accumOffset + direntLen;
        strcpy (mydirent->d_name, "..");
        mydirent = (dirent_t *) ((char *) mydirent + tmpLen);
    }

    for (i = 0; i < rowCnt; i++) {

        mySplitPath (&nameList[i*MAX_DATA_SIZE], tmpColl, lastName);
        /* this take care of "/XYZ" type path */
        if (lastName[0] == '\0' && tmpColl[0] == '/')
            strcpy (lastName, &tmpColl[1]);
        tmpLen = mydirent->d_reclen = (sizeof (dirent_t) +
          strlen (lastName)) / 8 * 8 + 8;
        direntLen += tmpLen;
        /* fake some no for d_ino and d_off */
        mydirent->d_ino = FAKE_INODE + strlen (&nameList[i*MAX_DATA_SIZE]);
        mydirent->d_off = srbObjDesc[fd].collInfo->accumOffset + direntLen;
        if (direntLen > mallocLen) {
            fprintf (stderr, "svrResToDirent: direntLen > mallocLen\n");
            errno = EINVAL;
            free (srbObjDesc[fd].collInfo->dirent);
            srbObjDesc[fd].collInfo->dirent = NULL;
            return 0;
        }
        strcpy (mydirent->d_name, lastName);

        mydirent = (dirent_t *) ((char *) mydirent + tmpLen);
    }
    srbObjDesc[fd].collInfo->maxDirLen = direntLen;
    srbObjDesc[fd].collInfo->offset = 0;
    return (direntLen);
}

int
svrResToDirent64 (char *nameList, int rowCnt, int fd, int firstFlag)
{
    int i;
    dirent64_t *mydirent;
    int mallocLen = 0;
    int direntLen = 0;
    int tmpLen;
    char lastName[MAX_TOKEN], tmpColl[MAX_TOKEN];

    if (srbObjDesc[fd].collInfo == NULL) {
        elog(NOTICE, 
	 "svrResToDirent64: opened fd=%d:objID=%s is not a collection",
          fd, srbObjDesc[fd].objID);
        return (OBJ_ERR_NOT_COLL);
    }

    /* do some bookkeeping */

    if (srbObjDesc[fd].collInfo->maxDirLen > srbObjDesc[fd].collInfo->offset) {
        fprintf (stderr,
         "svrResToDirent: Internal error, maxDirLen > offset\n");
        return 0;
    }

    if (nameList == NULL && rowCnt > 0)
        return 0;

    srbObjDesc[fd].collInfo->accumOffset += srbObjDesc[fd].collInfo->maxDirLen;
    srbObjDesc[fd].collInfo->maxDirLen = srbObjDesc[fd].collInfo->offset = 0;
    if (srbObjDesc[fd].collInfo->dirent != NULL) {
        free (srbObjDesc[fd].collInfo->dirent);
        srbObjDesc[fd].collInfo->dirent = NULL;
    }


    /* estimate the no of bytes to alloc */

    for (i = 0; i < rowCnt; i++) {
        mallocLen += (strlen (&nameList[i*MAX_DATA_SIZE]) + 1) /8 * 8 + 8;
    }

    /* first time around, have to add "." and ".." */
    if (firstFlag == 1) {
        /* add 9 bytes for padding */
        mallocLen += (rowCnt + 2)  * (sizeof (dirent64_t) + 8);
    } else {
        /* add 9 bytes for padding */
        mallocLen += rowCnt * (sizeof (dirent64_t) + 8);
    }

    srbObjDesc[fd].collInfo->dirent = malloc (mallocLen);

    /* pack the dirent */

    mydirent = (dirent64_t *) srbObjDesc[fd].collInfo->dirent;

    if (firstFlag == 1) {
        /* hand craft "." and ".." */
	mydirent->d_ino = FAKE_INODE + strlen (srbObjDesc[fd].collection);
        tmpLen = mydirent->d_reclen = sizeof (dirent64_t);
        direntLen += tmpLen;
        mydirent->d_off = srbObjDesc[fd].collInfo->accumOffset + direntLen;
        strcpy (mydirent->d_name, ".");
        mydirent = (dirent64_t *) ((char *) mydirent + tmpLen);

        mySplitPath (srbObjDesc[fd].collection, tmpColl, lastName);
        mydirent->d_ino = FAKE_INODE + strlen (tmpColl);
        tmpLen = mydirent->d_reclen = sizeof (dirent64_t) + 8;
        direntLen += tmpLen;
        mydirent->d_off = srbObjDesc[fd].collInfo->accumOffset + direntLen;
        strcpy (mydirent->d_name, "..");
        mydirent = (dirent64_t *) ((char *) mydirent + tmpLen);
    }

    for (i = 0; i < rowCnt; i++) {

        mySplitPath (&nameList[i*MAX_DATA_SIZE], tmpColl, lastName);
        /* this take care of "/XYZ" type path */
        if (lastName[0] == '\0' && tmpColl[0] == '/')
            strcpy (lastName, &tmpColl[1]);
        tmpLen = mydirent->d_reclen = (sizeof (dirent64_t) +
          strlen (lastName)) / 8 * 8 + 8;
        direntLen += tmpLen;
        /* fake some no for d_ino and d_off */
        mydirent->d_ino = FAKE_INODE + strlen (&nameList[i*MAX_DATA_SIZE]);
        mydirent->d_off = srbObjDesc[fd].collInfo->accumOffset + direntLen;
        if (direntLen > mallocLen) {
            fprintf (stderr, "svrResToDirent: direntLen > mallocLen\n");
            errno = EINVAL;
            free (srbObjDesc[fd].collInfo->dirent);
            srbObjDesc[fd].collInfo->dirent = NULL;
            return 0;
        }
        strcpy (mydirent->d_name, lastName);

        mydirent = (dirent64_t *) ((char *) mydirent + tmpLen);
    }
    srbObjDesc[fd].collInfo->maxDirLen = direntLen;
    srbObjDesc[fd].collInfo->offset = 0;
    return (direntLen);
}

int
svrGetMoreData (int *continueIndex, int maxCnt, int mcatIndex, char **nameList)
{
    int status;
    mdasC_sql_result_struct *myresult;

    if (*continueIndex < 0) {
        *nameList = NULL;
        return 0;
    }

    status = _svrGetMoreRows (MDAS_CATALOG, *continueIndex, &myresult,
         maxCnt);

    if (status >= 0 && myresult->row_count > 0) {
        *nameList = (char *) getFromResultStruct (myresult,
         dcs_tname[mcatIndex], dcs_aname[mcatIndex]);
        *continueIndex = myresult->continuation_index;
    } else {
        *nameList = NULL;
    }
    status = myresult->row_count;
    free (myresult);
    return (status);
}
   
int
svrOutputDirent (int fd, dirent_t *buf, int bufLen)
{
    dirent_t *direntStart, *mydirent;
    int direntLen = 0;
    int copyLen;


    if (srbObjDesc[fd].collInfo == NULL) {
        elog(NOTICE, 
	 "svrOutputDirent: opened fd=%d:objID=%s is not a collection",
          fd, srbObjDesc[fd].objID);
        return (OBJ_ERR_NOT_COLL);
    }

    direntLen = srbObjDesc[fd].collInfo->maxDirLen - 
     srbObjDesc[fd].collInfo->offset;

    if (direntLen <= 0)
        return 0;

    /* pack the dirent */

    mydirent = (dirent_t *) srbObjDesc[fd].collInfo->dirent;
    mydirent = (dirent_t *) ((char *) mydirent + 
     srbObjDesc[fd].collInfo->offset);
    direntStart = mydirent;

    if (bufLen >= direntLen) {
        copyLen = direntLen;
    } else {
        copyLen = 0;
        while (copyLen + mydirent->d_reclen <= direntLen &&
         copyLen + mydirent->d_reclen <= bufLen) {

            copyLen += mydirent->d_reclen;
            mydirent = (dirent_t *) ((char *) mydirent +
             mydirent->d_reclen);
            if (copyLen == direntLen || copyLen == bufLen)
                break;
        }
    }
    if (copyLen > 0) {
        memcpy ((void *) buf, direntStart, copyLen);
    }

    if (copyLen >= direntLen) {         /* all done */
        srbObjDesc[fd].collInfo->offset = srbObjDesc[fd].collInfo->maxDirLen;
    } else {
        srbObjDesc[fd].collInfo->offset += copyLen;
    }

    return (copyLen);
}

int
svrOutputDirent64 (int fd, dirent_t *buf, int bufLen)
{
    dirent64_t *direntStart, *mydirent;
    int direntLen = 0;
    int copyLen;


    if (srbObjDesc[fd].collInfo == NULL) {
        elog(NOTICE, 
	 "svrOutputDirent64: opened fd=%d:objID=%s is not a collection",
          fd, srbObjDesc[fd].objID);
        return (OBJ_ERR_NOT_COLL);
    }

    direntLen = srbObjDesc[fd].collInfo->maxDirLen - 
     srbObjDesc[fd].collInfo->offset;

    if (direntLen <= 0)
        return 0;

    /* pack the dirent */

    mydirent = (dirent64_t *) srbObjDesc[fd].collInfo->dirent;
    mydirent = (dirent64_t *) ((char *) mydirent + 
     srbObjDesc[fd].collInfo->offset);
    direntStart = mydirent;

    if (bufLen >= direntLen) {
        copyLen = direntLen;
    } else {
        copyLen = 0;
        while (copyLen + mydirent->d_reclen <= direntLen &&
         copyLen + mydirent->d_reclen <= bufLen) {

            copyLen += mydirent->d_reclen;
             mydirent = (dirent64_t *) ((char *) mydirent +
             mydirent->d_reclen);
            if (copyLen == direntLen || copyLen == bufLen)
                break;
        }
    }
    if (copyLen > 0) {
        memcpy ((void *) buf, direntStart, copyLen);
    }

    if (copyLen >= direntLen) {         /* all done */
        srbObjDesc[fd].collInfo->offset = srbObjDesc[fd].collInfo->maxDirLen;
    } else {
        srbObjDesc[fd].collInfo->offset += copyLen;
    }

    return (copyLen);
}

#if defined(HPSS) && defined(NO_DCE)
int
readHpssNodceAuth (char *nodceUser, char *nodceAuth)
{
    char conFile[MAX_TOKEN];
    char buf[MAX_TOKEN];
    FILE *file;

    if (nodceUser == NULL || nodceAuth == NULL)
        return -1;

    nodceUser[0] = '\0';
    nodceAuth[0] = '\0';

    strcpy(conFile, getSrbDataDir());
    strcat(conFile, "/");
    strcat(conFile, HPSS_NODCE_AUTH_FILE);

    file = fopen(conFile, "r");

    if (file == NULL) {
        fprintf(stderr, "Unable to open HPSS_NODCE_AUTH_FILE file %s\n",
         conFile);
        return -1;
    }

    while ((i = getNextLine(file, buf, sizeof(buf))) != EOF) {

        if (nodceUser[0] == '\0') {
            strcpy (nodceUser, buf);
        } else {
            strcpy (nodceAuth, buf);
        }
    }
    fclose (file);

    if (nodceUser[0] != '\0' && nodceAuth[0] != '\0')
        return 0;
    else
        return -1;
}
#endif  /* defined(HPSS) && defined(NO_DCE) */

char *
getLockPath (char *objID, char *collectionName)
{
    char *lockPath;
    char tmpStr[MAX_TOKEN];
    char *tmpPtr;
    int c;

    strcpy (tmpStr, collectionName);

    lockPath = malloc (strlen (objID) + strlen (collectionName) + 
     strlen (getSrbDataDir()) + 50);

    strcpy (tmpStr, collectionName);

    /* convert all '/' in collectionName to '.' */

    tmpPtr = tmpStr;

    while ((c = *tmpPtr) != '\0') {
        if (c == '/') 
	    *tmpPtr = '.';
	tmpPtr++;
    }

    sprintf (lockPath, "%-s/%-s/%-s.%-s", getSrbDataDir(), LOCK_DIR,
     tmpStr, objID);

    while ((c = *tmpPtr) != '\0') {
        if (c == '&') {
            *tmpPtr = '\0';
            break;
        }
        tmpPtr++;
    }

    return (lockPath);
}

/* isDataset check whether the input is a dataset. 
 * retrun - 0 - no, 1 yes
 */

int 
isDataset (char *dataName, char *collection)
{
    mdasC_sql_result_struct  *myresult = NULL;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i;
    int status;

    if (dataName == NULL || collection == NULL)
        return 0;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[SIZE] = 1;

    sprintf(qval[DATA_GRP_NAME]," = '%s'", collection);
    sprintf(qval[DATA_NAME]," = '%s'", dataName);

    status = queryDataInfo (collection, selval, qval, &myresult, DEF_NUM_ROWS);

    if (status >= 0 && myresult->row_count > 0) {
	status = 1;
        freeSqlResult (myresult);
    } else {
	status = 0;
    }
    return (status);
}

int
svrParseContainerName (char *inContainerName, char *outContainerName,
char *collectionName, char *userName, char *domainName)
{
    char tmpStr[MAX_TOKEN];

    if (inContainerName[0] == '/') {    /* full path */
        if (splitStrByLastChar (inContainerName, collectionName,
         outContainerName, '/') == NULL || inContainerName[0] != '/') {
            outContainerName[0] = collectionName[0] = 0;
            return (OBJ_ERR_BAD_CONTAINER_NAME);
        } else {
            return (0);
        }
    }

    if (splitStrByLastChar (inContainerName, tmpStr, outContainerName, '/') ==
     NULL) {            /* simple form */
#ifdef FED_MCAT
        char mcatName[MAX_TOKEN];
        int status;

        status = _svrGetMcatZone (userName, domainName, mcatName);
        if (status == 0) {
            strcpy (outContainerName, inContainerName);
            sprintf (collectionName, "/%s%s/%s.%s", mcatName, CONTAINER_HOME,
             userName, domainName);
        } else {
            strcpy (outContainerName, inContainerName);
            sprintf (collectionName, "%s/%s.%s", CONTAINER_HOME,
             userName, domainName);
        }
#else
        strcpy (outContainerName, inContainerName);
        sprintf (collectionName, "%s/%s.%s", CONTAINER_HOME,
         userName, domainName);
#endif
    } else {
        sprintf (collectionName, "%s/%s", CONTAINER_HOME, tmpStr);
    }

    return (0);
}

int
procStartupMsg (Port *myPort, StartupInfo *sp)
{
    PacketBuf *p;
    char *tmp;
    int status;
    static char envEntry[NUM_ENV_ENTRY][2 * ARGV_SIZE];
    char        namebuf[NAMEDATALEN];
    int privUserFlag;   /* proxy user is a privileged user ? */
    int clPrivUserFlag;   /* client user is a privileged user ? */
    int authSchemeInx;


    p = &myPort->buf;

    memset (sp, 0, sizeof (StartupInfo));
#ifdef foo
    sp->proxyUserName[0]='\0';
    sp->proxyDomainName[0]='\0';
    sp->proxyAuth[0]='\0';
    sp->clientUserName[0]='\0';
    sp->clientDomainName[0]='\0';
#ifdef FED_MCAT
    sp->proxyMcatZone[0]='\0';
    sp->clientMcatZone[0]='\0';
#endif
    sp->clientAuth[0]='\0';
    sp->options[0]='\0';
    sp->execFile[0]='\0';
    sp->version[0]='\0';
#endif

    tmp= p->data;
    strncpy(sp->proxyUserName,tmp,sizeof(sp->proxyUserName));
    tmp += sizeof(sp->proxyUserName);
    strncpy(sp->proxyDomainName,tmp, sizeof(sp->proxyDomainName));
    tmp += sizeof(sp->proxyDomainName);

    /* move setting privUserFlag to here because  proxyMcatZone depends on it */
    strcpy (namebuf, sp->proxyUserName);
    strcat (namebuf, "@");
    strcat (namebuf, sp->proxyDomainName);

    privUserFlag = chkSrbUser (namebuf);

#ifdef FED_MCAT
    /* trust LOCAL_PRIV_USER */
    if (privUserFlag == LOCAL_PRIV_USER)
        strncpy(sp->proxyMcatZone,tmp, sizeof(sp->proxyMcatZone));
    tmp += sizeof(sp->proxyMcatZone);
#endif
    strncpy(sp->clientUserName,tmp, sizeof(sp->clientUserName));
    tmp += sizeof(sp->clientUserName);
    strncpy(sp->clientDomainName,tmp, sizeof(sp->clientDomainName));
    tmp += sizeof(sp->clientDomainName);

#ifdef FED_MCAT
    if (privUserFlag == LOCAL_PRIV_USER)
        strncpy(sp->clientMcatZone,tmp, sizeof(sp->clientMcatZone));
    tmp += sizeof(sp->clientMcatZone);
    strncpy(sp->version,tmp, sizeof(sp->version));
    tmp += sizeof(sp->version);
    strncpy(sp->options,tmp, sizeof(sp->options));
    tmp += sizeof(sp->options);
    strncpy(sp->execFile,tmp, sizeof(sp->execFile));
    tmp += sizeof(sp->execFile);
#else
    strncpy(sp->options,tmp, sizeof(sp->options));
    tmp += sizeof(sp->options);
    strncpy(sp->execFile,tmp, sizeof(sp->execFile));
    tmp += sizeof(sp->execFile);
    strncpy(sp->version,tmp, sizeof(sp->version));
    tmp += sizeof(sp->version);
#endif
	
    authSchemeInx = atoi (sp->options);

    /* move up ClientUser here because remoteConnect needs it */

    if (ClientUser == NULL) {
        ClientUser = (userInfo *) malloc (sizeof (userInfo));
        memset (ClientUser, 0, sizeof (userInfo));
    }

    ClientUser->userName = strdup (sp->clientUserName);

    if (strcmp (ClientUser->userName, TICKET_USER) == 0) {
        TicketUserFlag = 1;     /* A ticket user */
    } else {
        TicketUserFlag = 0;     /* Not a ticket user */
    }

    ClientUser->userId = getUserId (ClientUser->userName);

    ClientUser->domainName = strdup (sp->clientDomainName);

    ClientUser->userAuth = '\0';

    ClientUser->mcatZone = '\0';

#ifdef FED_MCAT
    if (strlen (sp->proxyMcatZone) == 0) { 
        status = _svrGetMcatZone (sp->proxyUserName, sp->proxyDomainName,
         sp->proxyMcatZone);
        if (status < 0) {
            fprintf (stderr, 
	     "_svrGetMcatZone error for %s@%s, stat=%d, pid=%d\n",
             sp->proxyUserName, sp->proxyDomainName, status, getpid ());
	    if (status == MCAT_INQUIRE_ERROR)
		return (USER_NAME_NOT_FOUND);
	    else
                return (status);
        }
    }
    if (strlen (sp->clientMcatZone) == 0) {
        if (strcmp (sp->proxyUserName, sp->clientUserName) == 0 &&
         strcmp (sp->proxyDomainName, sp->clientDomainName) == 0) {
            strncpy(sp->clientMcatZone, sp->proxyMcatZone,
             sizeof(sp->clientMcatZone));
        } else {
            status = _svrGetMcatZone (sp->clientUserName, sp->clientDomainName,
             sp->clientMcatZone);
            if (status < 0) {
                fprintf (stderr, 
		 "_svrGetMcatZone error for %s@%s, stat=%d, pid=%d\n",
                 sp->clientUserName, sp->clientDomainName, status, getpid ());
                if (status == MCAT_INQUIRE_ERROR)
                    return (USER_NAME_NOT_FOUND);
                else
                    return (status);
            }
        }
    }
#ifdef foo	/* done in chkSrbUser */
    if (privUserFlag > 0) {
        if (strcmp (sp->proxyMcatZone,
         LocalMcat->serverAuthInfo->mcatName) != 0)
            privUserFlag = FOREIGN_PRIV_USER;
    }
#endif
#endif
    /* ProxyUser not the same as the ClientUser. The ProxyUser must
     * must be a privileged user */

    if ((strcmp (sp->proxyUserName, sp->clientUserName) != 0) ||
      (strcmp (sp->proxyDomainName, sp->clientDomainName) != 0)) {
        if (privUserFlag <= 0) {
            /* Not a privileged user */
            fprintf(stderr, "Proxy user: %s is not a privileged user\n",
              namebuf);
            return (AUTH_ERR_PROXY_NOPRIV);
        }
#ifdef foo
	/* check if the client and proxy user are from the same zone and
         * if the proxy user is in the local zone */
	if (strcmp (sp->proxyMcatZone, sp->clientMcatZone) != 0 &&
	 strcmp (sp->proxyMcatZone, LocalMcat->serverAuthInfo->mcatName) != 0 &&
	  authSchemeInx != GSI_DELEGATE_INX && TicketUserFlag == 0) {
	    elog (NOTICE, 
	     "clent %s@%s is from %s while proxyUser %s@%s is from %s",
	     sp->clientUserName, sp->clientDomainName, sp->clientMcatZone,
	     sp->proxyUserName, sp->proxyDomainName, sp->proxyMcatZone);
	    return AUTH_ERR_CLIENT_SVR_DIFF_ZONE;
	}
#endif
        strcpy (namebuf, sp->clientUserName);
        strcat (namebuf, "@");
        strcat (namebuf, sp->clientDomainName);
        clPrivUserFlag = chkSrbUser (namebuf);
#ifdef FED_MCAT
        if (clPrivUserFlag > 0) { 
            if (strcmp (sp->clientMcatZone, 
	      LocalMcat->serverAuthInfo->mcatName) != 0)
                clPrivUserFlag = FOREIGN_PRIV_USER;
	}
#endif
    } else {
	clPrivUserFlag = privUserFlag;
    }

    /* set some env variable */

    for (i = 0; i < NUM_ENV_ENTRY; i++) {
        memset(envEntry[i], 2*ARGV_SIZE,0);
    }
    /*
     * Set up the necessary environment variables for the srbServer
     */

    sprintf(envEntry[0], "proxyPrivFlag=%d", privUserFlag);
    putenv(envEntry[0]);
    sprintf(envEntry[1], "clientUserName=%s", sp->clientUserName);
    putenv(envEntry[1]);
    sprintf(envEntry[2], "clientDomainName=%s", sp->clientDomainName);
    putenv(envEntry[2]);

    sprintf(envEntry[3], "clientPrivFlag=%d", clPrivUserFlag);
    putenv(envEntry[3]);

    sprintf(envEntry[4], "proxyUserName=%s", sp->proxyUserName);
    putenv(envEntry[4]);
    sprintf(envEntry[5], "proxyDomainName=%s", sp->proxyDomainName);
    putenv(envEntry[5]);
    sprintf(envEntry[6], "srbVersion=%s", sp->version);
    putenv(envEntry[6]);
#ifdef FED_MCAT
    sprintf(envEntry[7], "proxyMcatZone=%s", sp->proxyMcatZone);
    putenv(envEntry[7]);
    sprintf(envEntry[8], "clientMcatZone=%s", sp->clientMcatZone);
    putenv(envEntry[8]);
#endif

    /* set up ProxyUser and ClientUser that used to be done in 
     * initProxyUserInfo */

    if (ProxyUser == NULL) {
        ProxyUser = (userInfo *) malloc (sizeof (userInfo));
        memset (ProxyUser, 0, sizeof (userInfo));
#ifdef foo
	if (authSchemeInx == GSI_DELEGATE_INX) {
	    /* For GSI_DELEGATE, ProxyUser and ClientUser are the same */
	    ProxyUser->userName = strdup (ClientUser->userName);
	    ProxyUser->domainName = strdup (ClientUser->domainName);
	    ProxyUser->userId = ClientUser->userId;
	} else {
            ProxyUser->userName = clGetUserName(NULL, errorMsg);

            if (ProxyUser->userName == NULL) {
                return (CLI_ERR_INVAILD_USER);
            }

            ProxyUser->userId = getUserId (ProxyUser->userName);

            ProxyUser->domainName = clGetDomainName (NULL, errorMsg);

            if (ProxyUser->domainName == NULL) {
                return (CLI_ERR_INVAILD_DOMAIN);
            }

            ProxyUser->userAuth = clGetAuth (errorMsg);
	}
#endif	/* foo */
        ProxyUser->userName = sp->proxyUserName;
        ProxyUser->userId = getUserId (ProxyUser->userName);
        ProxyUser->domainName = sp->proxyDomainName;
        /* ProxyUser->userAuth = clGetAuth (errorMsg); */
    } else {
        /* already done it at least once */
        if (ProxyUser->mcatZone != NULL)
            free (ProxyUser->mcatZone);
    }
      
#ifdef FED_MCAT
    ProxyUser->mcatZone = strdup (sp->proxyMcatZone);
#ifdef foo
    ProxyUser->mcatZone = clGetMcatZone (NULL, errorMsg);
    if (ProxyUser->mcatZone == NULL && LocalMcat != NULL)
        ProxyUser->mcatZone = strdup(LocalMcat->serverAuthInfo->mcatName);
#endif
    ClientUser->mcatZone = strdup (sp->clientMcatZone);
#endif

    ProxyUser->privUserFlag = atoi (getenv ("proxyPrivFlag"));

    ProxyUser->authSchemeInx = ClientUser->authSchemeInx = authSchemeInx;

    ClientUser->privUserFlag = atoi (getenv (CLIENT_PRIV_KEYWORD));

    return (0);
}


/* isPureObject check whether the input is a non-linked and non-container dataset. 
 * retrun - 0=no, 1=yes, negative for error
 */

int 
isPureObject(char *dataName, char *collection)
{
    mdasC_sql_result_struct  *myresult = NULL;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i;
    int status;
    char *dIdStr, *cIdStr;

    if (dataName == NULL || collection == NULL)
        return 0;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[DATA_ID] = 1;
    selval[CONTAINER_ID] = 1;

    sprintf(qval[DATA_GRP_NAME]," = '%s'", collection);
    sprintf(qval[DATA_NAME]," = '%s'", dataName);

    status = queryDataInfo (collection, selval, qval, &myresult, DEF_NUM_ROWS);

    if (status >= 0 && myresult->row_count > 0) {
      dIdStr = (char *) getFromResultStruct (myresult,
         dcs_tname[DATA_ID], dcs_aname[DATA_ID]);
      cIdStr = (char *) getFromResultStruct (myresult,
         dcs_tname[CONTAINER_ID], dcs_aname[CONTAINER_ID]);
      if (!strcmp( dIdStr,cIdStr))
	status = 1;
      else
	status = 0;
      freeSqlResult (myresult);
    } 
    return (status);
}
