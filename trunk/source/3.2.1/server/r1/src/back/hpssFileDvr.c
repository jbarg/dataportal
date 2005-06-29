/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/*
 * hpssFileDvr.c-- Routines to handle HPSS storage type calls
 */
#include "hpssFileDvr.h"
 
#ifdef HPSS

void 
hpss_timeout(int sig)
{
   char            string60[60];

   elog(NOTICE, "hpss_timeout(), pid=%d, in hpssFileDrv.c, Timeout %u seconds waiting for %s, last file name used in hpssOpen() \"%s\"",
	  getpid(), uiSecs, cpFunctionName, cpLastFileName);
   /* clFinish(connsave); */
   alarm(0);
   longjmp(jenv, 2);
}

/* _hpssOpen - This is the internal version of open. It is used by both
 * the open and create calls.
 *
 * Input : char *fileName - The path name of the file to be opened.
 *         int fileFlags - The open flag
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the opened file.
 */

int
_hpssOpen(char *fileName, int fileFlags, int fileMode, srb_long_t size)
{
    static int osRanOut = 0;
    int     tmpfd;
    hpss_cos_hints_t  HintsIn;
    hpss_cos_priorities_t HintsPri;
    hpss_cos_hints_t  HintsOut;
    char *tmpCOSId;
    
    /* 
     * preset the HPSS hints structures used to control Class of Service
     */

    memset(&HintsIn,  0, sizeof HintsIn);
    memset(&HintsPri, 0, sizeof HintsPri);
    memset(&HintsOut, 0, sizeof HintsOut);

    if ((HintsIn.COSId = getCosBySize (size)) < 0) 
	return HPSS_SRB_NOCOS;

    HintsPri.COSIdPriority = REQUIRED_PRIORITY; /* must use this priority
                                                        if COSId specified */
 
    tmpfd = hpss_Open(fileName,fileFlags,fileMode
                    ,&HintsIn
                    ,&HintsPri
                    ,&HintsOut);

    return tmpfd;
}

/* hpssOpen - Handles the open call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileFlags - The open flag
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the opened file.
 */

int
hpssOpen(char *fileName, int fileFlags, int fileMode)
{
    int status;
    int retVal;

    uiSecs=MIN_30; /* only wait 30 minutes for HPSS open, otherwise time out with error */

    (void)sprintf(cpFunctionName,"hpssOpen(%s, %d, %d)", fileName, fileFlags, fileMode);

    signal(SIGALRM, hpss_timeout);
    (void)sprintf(cpLastFileName, "%s pid=%d", fileName, getpid());

    if (setjmp(jenv) == 0) {
      alarm(uiSecs);
      status = _hpssOpen(fileName, fileFlags, fileMode, (srb_long_t) -1);
      alarm(0);
    } else {
      /* get there by calling longjmp */
       elog (NOTICE, "timeout because of longjmp trigered by %s. Timeout %u seconds", cpFunctionName, uiSecs);
      status=HPSS_SRB_TIMEOUT_OPEN3;
      strcpy(cpFunctionName, "none_set");
      return (status);
    }
    strcpy(cpFunctionName, "none_set");
    alarm(0);

    if (status < 0) {
        retVal = hpssErrConv (status);
        elog (NOTICE, "HPSS open error. ErrorCode = %d", retVal);
        return (retVal);
    } else
        return (status);
}

/* hpssCreate - Handles the create call.
 *
 * Input : char *fileName - The file path name to be opened
 *         int fileMode - The file mode
 *
 * Output : Returns the file descriptor of the new file.
 */

int
hpssCreate(char *fileName, int fileMode, srb_long_t size)
{
    int status;
    int retVal;

    /* allow user to set the mode of the external file */

    (void) hpss_Umask((mode_t) 0000);

    uiSecs=MIN_5; /* only wait 5 minutes for HPSS create, otherwise time out with error */

    (void)sprintf(cpFunctionName,"hpssCreate(%s, %d, %ld)", fileName, fileMode, size);

    signal(SIGALRM, hpss_timeout);
    
    if (setjmp(jenv) == 0) {
      alarm(uiSecs);
      status = _hpssOpen(fileName, O_RDWR|O_CREAT|O_EXCL, fileMode, size);
      alarm(0);
    } else {
      /* get there by calling longjmp */
       elog (NOTICE, "timeout because of longjmp trigered by %s. Timeout %u seconds", cpFunctionName, uiSecs);
       status=HPSS_SRB_TIMEOUT_CREATE;
       strcpy(cpFunctionName, "none_set");
       return (status);
    }
    strcpy(cpFunctionName, "none_set");
    alarm(0);

    /* (void) hpss_Umask((mode_t) 0077); */
    if (status < 0) {
        retVal = hpssErrConv (status);
        elog (NOTICE, "HPSS create error. ErrorCode = %d", retVal);
        return (retVal);
    } else
        return (status);

}
 
/* hpssClose - Handles the close call.
 *
 * Input : int file - The file descriptor to be closed
 *
 * Output : status of close
 */

int
hpssClose(int file)
{
    int status;
    int retVal;
    
    status = hpss_Close(file);

    if (status < 0) {
        retVal = hpssErrConv (status);
        elog (NOTICE, "HPSS close error. ErrorCode = %d", retVal);
        return (retVal);
    } else
        return (status);

}

/* hpssRead - Handles the read call.
 *
 * Input : int file - The file descriptor to read
 *         char *buffer - The input buffer
 *         int amount - The amount to read
 *
 * Output : Returns to number of bytes read
 */

int
hpssRead(int file, char *buffer, int amount)
{
    int	status;
    int retVal;

    uiSecs=MIN_20; /* only wait 20 minutes for HPSS read, otherwise time out with error */

    (void)sprintf(cpFunctionName,"hpssRead(%d, char *buffer, %d)", file, amount);

    signal(SIGALRM, hpss_timeout);
    
    if (setjmp(jenv) == 0) {
      alarm(uiSecs);
      status = hpss_Read(file, buffer, amount);
      alarm(0);
    } else {
      /* get there by calling longjmp */
       elog (NOTICE, "timeout because of longjmp trigered by %s. Timeout %u seconds", cpFunctionName, uiSecs);
       status=HPSS_SRB_TIMEOUT_READ;
       strcpy(cpFunctionName, "none_set");
       return (status);
    }
    strcpy(cpFunctionName, "none_set");
    alarm(0);

    if (status < 0) {
        retVal = hpssErrConv (status);
        elog (NOTICE, "HPSS read error. ErrorCode = %d", retVal);
        return (retVal);
    } else
        return (status);

}

/* hpssWrite - Handles the write call.
 *
 * Input : int file - The file descriptor to write
 *         char *buffer - The output buffer
 *         int amount - The amount to write
 *
 * Output : Returns to number of bytes written
 */

int
hpssWrite(int file, char *buffer, int amount)
{
    int	status;
    int nwritten;
    int nleft = amount;
    int retVal;

    while (nleft > 0) {
       nwritten = hpss_Write(file, buffer, nleft);
       if (nwritten < 0) {
          status = nwritten;  /* error code */
          retVal = hpssErrConv (status);
          elog (NOTICE, "HPSS write error. ErrorCode = %d", retVal);
          return (retVal);
       }
       nleft -= nwritten;
       buffer += nwritten;
    }
    status = amount;                /* all written successfully */

    if (status < 0) {
        retVal = hpssErrConv (status);
        elog (NOTICE, "HPSS write error. ErrorCode = %d", retVal);
        return (retVal);
    } else
        return (status);
}

/* hpssSeek - Handles the seek call.
 *
 * Input : int file - The file descriptor to seek
 *         int offset - The position of the next operation
 *         int whence - Same definition as in Unix.
 *              SEEK_SET - pointer is set to the value of the Offset parameter.
 *              SEEK_CUR - pointer is set to its current location plus the
 *                      value of the Offset parameter.
 *              SEEK_END - pointer is set to the size of the file plus the
 *                      value of the Offset parameter.
 *
 * Output : Returns the status of seek
 */

srb_long_t
hpssSeek(int file, srb_long_t offset, int whence)
{
    srb_long_t  status, tmp;
    int retVal;
    u_signed64 myOffset;

    /* status = hpss_Lseek(file, offset, whence); */

    /* do the 64 bit seek */

    tmp = offset;

    memcpy (&myOffset, &tmp, 8);

    retVal = hpss_SetFileOffset (file, myOffset, whence, HPSS_SET_OFFSET_FORWARD,
     (u_signed64 *) &status);

    if (retVal < 0) {
        retVal = hpssErrConv (retVal);
        elog (NOTICE, "HPSS seek error. ErrorCode = %d", retVal);
        return (retVal);
    } else {
        return (status);
    }
}

/* hpssSync - Handles the sync call.
 *
 * Input : int file - The file descriptor to sync
 *
 * Output : Returns the status of sync
 */

int
hpssSync(int file)
{
    int	status;
    
    /*
     *  If the file isn't open, then we don't need to sync it; we
     *  always sync files when we close them.  Also, if we haven't
     *  done any writes that we haven't already synced, we can ignore
     *  the request.
     */
    
    /*
     * no fsync equivalent function for HPSS
     */

    status = INP_ERR_NO_SUPPORT;
    return status;
}

/* hpssUnlink - Handles the unlink call.
 *
 * Input : char *filename - The file path name to unlink
 *
 * Output : Returns the status of unlink
 */

int
hpssUnlink(char *filename)
{
    int status;
    int retVal;
 
    status = hpss_Unlink (filename);
    if (status < 0) {
        retVal = hpssErrConv (status);
        elog (NOTICE, "HPSS unlink error. ErrorCode = %d", retVal);
        return (retVal);
    } else
        return (status);
}

/* hpssStat - Handles the stat call.
 *
 * Input : char *filename - The file path name to stat
 *         struct stat *statbuf - The place to put the stat output
 *
 * Output : Returns the status of stat
 */

int
hpssStat(char *filename, struct stat *statbuf)
{
    int status;
    int retVal;
    struct stat hpssstat;
 
    status = hpss_Stat(filename, &hpssstat);

    if (status < 0) {
        retVal = hpssErrConv (status);
	if (DebugLvl > 1)
            elog (NOTICE, "HPSS stat error. ErrorCode = %d", retVal);
        return (retVal);
    } else {
	convStat32ToStat ((struct stat32 *) &hpssstat, statbuf);
        if (sizeof(statbuf->st_size) == 8) {
            hpss_fileattr_t myAttr;

            status = hpss_FileGetAttributes (filename, &myAttr);
            if (status < 0) {
                retVal = hpssErrConv (status);
                elog (NOTICE,
                 "hpssStat: hpss_FileGetAttributes error. ErrorCode= %d",
                  retVal);
                return (retVal);
            }
#if defined(PORTNAME_linux)
	    hpssToSrbLong (&myAttr.BFSAttr.BfAttribMd.DataLen, 
	     &statbuf->st_size);
#else
            memcpy (&statbuf->st_size, &myAttr.BFSAttr.BfAttribMd.DataLen, 8);
#endif
        }

        return (status);
    }
}

/* hpssFstat - Handles the fstat call.
 *
 * Input : int fd - The file desc name to stat
 *         struct stat *statbuf - The place to put the stat output
 *
 * Output : Returns the status of stat
 */

int
hpssFstat(int fd, struct stat *statbuf)
{
    int status;
    int retVal;
    struct stat hpssstat;

    status = hpss_Fstat(fd, &hpssstat);

    if (status < 0) {
        retVal = hpssErrConv (status);
        if (DebugLvl > 1)
            elog (NOTICE, "HPSS fstat error. ErrorCode = %d", retVal);
        return (retVal);
    } else {
	convStat32ToStat ((struct stat32 *) &hpssstat, statbuf);
        return (status);
    }
}

/* hpssMkdir - Handles the mkdir call.
 *
 * Input : char *filename - The directory path name to mkdir
 *         int mode - The mode of the directory
 *
 * Output : Returns the status of mkdir
 */

int
hpssMkdir(char *filename, int mode)
{
    int status;
    int retVal;
 
    status = hpss_Mkdir(filename, mode);

    if (status < 0) {
        retVal = hpssErrConv (status);
        elog (NOTICE, "HPSS mkdir error. ErrorCode = %d", retVal);
        return (retVal);
    } else
        return (status);
}
 
/* hpssChmod - Handles the chmod call.
 *
 * Input : char *filename - The path name to chmod
 *         int mode - The mode of the directory
 *
 * Output : Returns the status of chmod
 */

int
hpssChmod (char *filename, int mode)
{
    int status;
    int retVal;
 
    status = hpss_Chmod(filename, mode);

    if (status < 0) {
        retVal = hpssErrConv (status);
        elog (NOTICE, "HPSS create error. ErrorCode = %d", retVal);
        return (retVal);
    } else
        return (status);
}

/* hpssRmdir - Handles the rmdir call.
 *
 * Input : char *filename - The path name to rmdir
 *
 * Output : Returns the status of rmdir
 */

int
hpssRmdir (char *filename)
{
    int status;
    int retVal;
 
    status = hpss_Rmdir(filename);
    if (status < 0) {
        retVal = hpssErrConv (status);
        elog (NOTICE, "HPSS rmdir error. ErrorCode = %d", retVal);
        return (retVal);
    } else
        return (status);
}

/* hpssOpendir - Handles the opendir call.
 *
 * Input : char *dirname - The path name to Opendir
 *         **sysDepDirP - The address to store the opened *descriptor.
 *
 * Output : Returns the status of Opendir - 0 - OK, -1 - error.
 */
 
int
hpssOpendir (char *dirname, void **sysDepDirP)
{
    int status;
    DIR *dirPtr;
    int *intPtr;
    int retVal;
 
   
    status = hpss_Opendir (dirname);

    if (status < 0) {
        retVal = hpssErrConv (status);
        elog (NOTICE, "HPSS opendir error. ErrorCode = %d", retVal);
        return (retVal);
    }

    intPtr = (int *) malloc (sizeof (int));
    *intPtr = status;
    *sysDepDirP = (void *) intPtr;
    return (0);
}

/* hpssClosedir - Handles the closedir call.
 *
 * Input : *sysDepDir - The *DIR to close.
 *
 * Output : Returns the status of closedir
 */
 
int
hpssClosedir (void *sysDepDir)
{
    int status;
    int *dirdes;
    int retVal;

    dirdes = (int *) sysDepDir;
    status = hpss_Closedir (*dirdes);
    free ((char *) sysDepDir);

    if (status < 0) {
        retVal = hpssErrConv (status);
        elog (NOTICE, "HPSS create error. ErrorCode = %d", retVal);
        return (retVal);
    } else
        return (status);
}

/* hpssReaddir - Handles the readdir call.
 *
 * Input : *sysDepDir - The *descriptor to read.
 *         struct srbDirent *srbDirent - Address of the output srbDirent.
 *
 * Output : Returns the status of readdir
 */
 
int
hpssReaddir (void *sysDepDir, struct srbDirent *srbDirent)
{
    struct dirent dp;
    int status;
    int *dirdes;
    int retVal;
 
    dirdes = (int *) sysDepDir;
    status = hpss_Readdir (*dirdes, (hpss_dirent_t *) &dp);
 
#if defined(PORTNAME_aix)
    if (status < 0 || dp.d_namlen <= 0) { /* Have to check d_namlen because 
					   * status is always 0 (a bug) 
					   * even when the end is reached
					   */
#elif defined(PORTNAME_solaris) || defined(PORTNAME_linux)
    if (status < 0) {
#endif
        retVal = hpssErrConv (status);
        elog (NOTICE, "HPSS readdir error. HPSS errorCode = %d", retVal);
        return (retVal);
    }

#if defined(PORTNAME_aix)
    srbDirent->d_ino = htonl (dp.d_ino);
    srbDirent->d_offset = htonl (dp.d_offset);
    srbDirent->d_reclen = htons (dp.d_reclen);
    srbDirent->d_namlen = htons (dp.d_namlen);
    strcpy (srbDirent->d_name, dp.d_name);
/* ---- Added by K.Ishikawa 2003-07-02 ---- */
#elif defined(PORTNAME_linux)
    srbDirent->d_ino = htonl (dp.d_ino);
    srbDirent->d_reclen = htons (dp.d_reclen);
    strcpy (srbDirent->d_name, dp.d_name);
#elif defined(PORTNAME_solaris)
    srbDirent->d_ino = htonl (dp.d_ino);
    srbDirent->d_offset = htonl (dp.d_off);
    srbDirent->d_reclen = htons (dp.d_reclen);
    srbDirent->d_namlen = 0;            /* no equiv entry for solaris */
    strcpy (srbDirent->d_name, dp.d_name);
#endif

    return (0);
}
 
int hpssErrConv (int status)
{

    if (status < HPSS_ERR_START && status > HPSS_ERR_END) {
	elog (NOTICE, "hpssErrConv: Unknown HPSS error %d", status);
	return (HPSS_UNKNOWN_ERR);
    } 

    return (HPSS_ERR_PREFIX + status);
}

#ifndef NO_DCE
/*
 * PurgeLoginContext - purge a login context and stop refresh thread
 *
 * Input - None
 *
 * Output : The status of the purge. 0 = success, != 0 => error.
 */

signed32 PurgeLoginContext(void)
{
    error_status_t          e_status;
    int                     i_status;
    pthread_addr_t          p_status;


    /*
     * Stop the refresh thread, then wait for it to finish before deleting
     * the security context.  Ignore the thread's status value.  Detach
     * thread just in case server doesn't exit for some reason.
     *
     */

    i_status = pthread_cancel(refreshThread);
    if (i_status)
        return errno;
    i_status = pthread_join(refreshThread, &p_status);
    if (i_status)
        return errno;

    i_status = pthread_detach(&refreshThread);
    if (i_status)
        return errno;

    sec_login_purge_context(&loginContext, &e_status);
    if (e_status != error_status_ok)
        return e_status;
    return HPSS_E_NOERROR;
}
/*
 * SetLoginContext - establish a DCE login context for a server
 * This routine establishes a DCE login context for a server.  The routine
 * gets the server's password from a named keytab.
 *
 * Input : char *in_PrincipalName - The user's DCE login name
 *         char *in_KeytabName - The path name of the keytab file
 *
 * Output : Returns the status of the call. 0 = success, != 0 => error.
 */

signed32 SetLoginContext(
char            *in_PrincipalName,
char            *in_KeytabName)
{
    static                          called = 0;     /* 1=>rtn was called */
    error_status_t                  status;
    error_status_t                  status2;        /* throw-away status */
    sec_login_auth_src_t            authSrc;
    void                            *keydata=0;     /* Server's key */
    boolean32                       resetPwd;       /* throw-away data */
    char *tmp_PrincipalName, *tmp_KeytabName;

    /* Don't let the routine be called more than once */
    if (called)
        return HPSS_EINVAL;
    called = 1;


    if (in_PrincipalName == NULL) {
        tmp_PrincipalName = getenv ("srbPrincipalName");
        if (tmp_PrincipalName == NULL)
            tmp_PrincipalName = df_PrincipalName;
    } else {
        tmp_PrincipalName = in_PrincipalName;
    }

    if (in_KeytabName == NULL) {
        tmp_KeytabName = getenv ("srbKeytabName");
        if (tmp_KeytabName == NULL)
            tmp_KeytabName = df_KeytabName;
    } else {
        tmp_KeytabName = df_KeytabName;
    }

    KeytabName = malloc(strlen(tmp_KeytabName) + 1);
    if (KeytabName == NULL) {
        elog(FATAL, "SetLoginContext: Palloc PrincipalName failed!");
        return errno;
    }
    strcpy(KeytabName, tmp_KeytabName);

    PrincipalName = malloc(strlen(tmp_PrincipalName) + 1);
    if (PrincipalName == NULL) {
        elog(FATAL, "SetLoginContext: Palloc PrincipalName failed!");
        return errno;
    }
    strcpy(PrincipalName, tmp_PrincipalName);

    /* Create an initial login context for the process */

    status = createLoginContext();
    if (status != HPSS_E_NOERROR) {
        elog(FATAL, "SetLoginContext: createLoginContext failed!");
        return status;
    }

    /* Start a thread to refresh the credentials */

    if (pthread_create(&refreshThread, pthread_attr_default,
      (pthread_startroutine_t) refreshLoginContext, (pthread_addr_t) NULL)) {
        status = errno;
        sec_login_purge_context(&loginContext, &status2);
        elog(FATAL, "SetLoginContext: pthread_create failed!");
        return status;
    }
    return HPSS_E_NOERROR;
}

/*
 * createLoginContext - create a new login context
 *
 * Input : None
 *
 * Output : The status of the call. 0 = success, != 0 => error.
 */

signed32 createLoginContext(void)
{
    error_status_t                  status;
    error_status_t                  status2;        /* throw-away status */
    sec_login_auth_src_t            authSrc;
    void                            *keydata=0;     /* Server's key */
    boolean32                       resetPwd;       /* throw-away data */

    /* Create an initial login context for the process */

    /* Set up the DCE login context */
    if (!sec_login_setup_identity((unsigned_char_p_t) PrincipalName,
        sec_login_no_flags, &loginContext, &status2)) {
        elog(FATAL, "createLoginContext:Sec_login_setup_identity failed!");
        return HPSS_EAUTH;
    }

    /* Get key version number 0 (the latest key) */
    sec_key_mgmt_get_key(rpc_c_authn_dce_secret, KeytabName,
        (unsigned_char_t *) PrincipalName, 0, &keydata, &status);
    if (status != error_status_ok) {
        elog(FATAL, "createLoginContext: Sec_key_mgmt_get_key failed!");
        return status;
    }

    if (!sec_login_validate_identity(loginContext, keydata,
      &resetPwd, &authSrc, &status2)) {
        sec_key_mgmt_free_key(keydata, &status2);
        sec_login_purge_context(&loginContext, &status2);
        elog(NOTICE,
          "createLoginContext: sec_login_validate_identity failed!");
        return HPSS_EAUTH;
    }

    sec_key_mgmt_free_key(keydata, &status2);

    if (authSrc != sec_login_auth_src_network) {
        /* For now at least, require network credentials */
        sec_login_purge_context(&loginContext, &status2);
        elog(FATAL, "createLoginContext: No network credentials!");
        return HPSS_EAUTH;
    }

    sec_login_set_context(loginContext, &status);
    if (status != error_status_ok) {
        sec_login_purge_context(&loginContext, &status2);
        elog(FATAL, "createLoginContext: sec_login_set_context failed!");
        return status;
    }
    return HPSS_E_NOERROR;
}



/*
 * refreshLoginContext - refresh a login context
 *
 * Input : None
 *
 * Output : None
 */

static void refreshLoginContext(
pthread_addr_t                  unusedArg)
{
    sec_login_auth_src_t    authSrc;        /* ignored */
    signed32                delayTime;      /* How long to delay */
    signed32                expireTime;     /* When context will expire */
    static int              failCount = 0;  /* # of refresh failures */
    void                    *keydata=0;     /* Server's key */
    signed32                margin;         /* Time it takes to renew */
    struct timeval          now;
    boolean32               resetPwd;       /* ignored */
    error_status_t          status;
    error_status_t          status2;        /* Throw-away error code */
    struct timespec         timer;          /* How long to delay */
    struct timezone         tz;             /* Time zone (unused) */
    signed32                validTime;      /* How long before expire */

#define TEN_MINUTES 600
#define TEN_SECONDS 10
#define MAX_FAILS 5

    while (TRUE) {
        gettimeofday(&now, &tz);
        sec_login_get_expiration(loginContext, &expireTime, &status);
        if (status != error_status_ok && status !=
          sec_login_s_not_certified) {
            elog(NOTICE,
              "refreshLoginContext :sec_login_get_expiration failed, status=%d",
                status);
            delayTime = TEN_MINUTES;
        } else {
            validTime = expireTime - now.tv_sec;
            margin = TEN_MINUTES;
            if (margin > validTime/2)
                    margin = validTime/2;
            delayTime = validTime - margin;
        }
        if (delayTime < TEN_SECONDS)    /* Don't let loop go wild! */
            delayTime = TEN_SECONDS;

        timer.tv_sec = delayTime;
        timer.tv_nsec = 0;
        if (HPSS_Logging)
            fprintf(stderr, "   Delay time is %ld\n", delayTime);
        pthread_delay_np(&timer);


        /* Check to see if we have had too many recent failures.
         * If so, log the fact, and then try to get a new credential.
         * If that fails, continue the loop and hope for the best.
         */

        if (failCount >= MAX_FAILS) {
            status = HPSS_EAUTH;
            elog(NOTICE,
                "refreshLoginContext: Refresh failed too many times!");
            status = createLoginContext();
            if (status != HPSS_E_NOERROR) {
                elog(NOTICE,
                  "refreshLoginContext: refreshLoginContext failed!");
            } else
               failCount = 0;
            continue;               /* Resume the refresh loop */
        }


        /* We haven't failed (too often), so try to refresh (again) */

        failCount++;                    /* Assume that refresh will fail */

        /* Note: DCE doc for sec_login_refresh_identify is wrong! */
        if (!sec_login_refresh_identity(loginContext, &status)) {
            HPSS_ERROR(status,
              "refreshLoginContext/sec_login_refresh_identity",
                NOMAP, HPSS_RESUME);
            continue;
        }
        sec_key_mgmt_get_key(rpc_c_authn_dce_secret, KeytabName,
          (unsigned_char_t *) PrincipalName, 0, &keydata, &status);
        if (status != error_status_ok) {
            elog(NOTICE, "refreshLoginContext: sec_key_mgmt_get_key failed!");
            continue;
        }
        if (!sec_login_validate_identity(loginContext,
          keydata, &resetPwd, &authSrc, &status)) {
            elog(NOTICE,
              "refreshLoginContext: sec_login_validate_identity failed!");
            continue;
        }
        sec_key_mgmt_free_key(keydata, &status2);
        failCount = 0;          /* Reset counter => refresh for a while */
    }
}
#else
#ifndef SEA_AUTH
/* a dummy needed by the no_dce lib */
int
seaAuthClient (int x, char *y, char *z)
{}
#endif
#endif  /* NO_DCE */

int
initHpssCos ()
{
   FILE *file;
    char buf[LINE_LEN];
    char *conFile;
    int i;
    struct hpssCosTable *tmpTable;

    hpssCosHead = NULL;

    /* put together the full pathname for the config file */

    conFile = (char *) malloc((strlen(getSrbDataDir()) +
        strlen(HPSS_COS_CONFIG)+2)*sizeof(char));

    strcpy(conFile, getSrbDataDir());
    strcat(conFile, "/");
    strcat(conFile, HPSS_COS_CONFIG);

    file = fopen(conFile, "r");

    if (file == NULL) {
	HpssDefCos = DEFAULT_COS;
	fprintf(stderr, 
	 "Cannot open HPSS config file: %s. Will use default class %d.\n",
	  HPSS_COS_CONFIG, HpssDefCos);
	free (conFile);
	return (-1);
    }

    free (conFile);
    HpssDefCos = -1;

    while ((i = getNextStr(file, buf, sizeof(buf))) != EOF) {

        /* Read until end of line then next line */

        if (buf[0] == '#') {
            while (i == 0)
                i = getNextStr(file, buf, sizeof(buf));
            continue;
        }

	if (i == '\n') {
	    fprintf(stderr, 
	     "initHpssCos: %s must have at least 2 integers per line. Got %s\n",
	     HPSS_COS_CONFIG, buf);
	    return -1;
	}

	tmpTable = (struct hpssCosTable *) malloc (sizeof(struct hpssCosTable));

	if (tmpTable == NULL) {
	    fprintf(stderr, "initHpssCos: malloc error. Errno = %d\n", errno);
	    return (-1);
	}

	tmpTable->cos = atoi (buf);

	/* read the next integer */

	i = getNextStr(file, buf, sizeof(buf));

        if (buf[0] == '#') {
	    fprintf(stderr, 
	     "initHpssCos: %s must have at least 2 integers per line. \n",
	      HPSS_COS_CONFIG);
            while (i == 0)
                i = getNextStr(file, buf, sizeof(buf));
            continue;
        }

	tmpTable->maxKByteSz = atoi (buf);
	
	enqueCos (tmpTable);

	if (i == '\n') 
	    continue;

        /* Read the rest of the line */

	i = getNextStr(file, buf, sizeof(buf));

	if (strcmp (buf, DEF_COS_KEY) == 0)
	    HpssDefCos = tmpTable->cos;

	/* discard the rest */

        while (i == 0) 
            i = getNextStr(file, buf, sizeof(buf));
    }
    if (HpssDefCos == -1)
	HpssDefCos = DEFAULT_COS;
	
    return (0);
}

int
enqueCos (struct hpssCosTable *cosTable)
{
    struct hpssCosTable *lastTable = NULL;
    struct hpssCosTable *tmpTable;

    if (hpssCosHead == NULL) {
	hpssCosHead = cosTable;
	hpssCosHead->next = NULL;
	return 0;
    }

    tmpTable = hpssCosHead;

    /* Insert in accending order */

    while (tmpTable != NULL) {
	if (tmpTable->maxKByteSz > cosTable->maxKByteSz)
	    break;

	lastTable = tmpTable;
	tmpTable = lastTable->next;
    }

    if (lastTable == NULL) {
	cosTable->next = hpssCosHead;
	hpssCosHead = cosTable;
    } else {
	cosTable->next = lastTable->next;
	lastTable->next = cosTable;
    }
    
    return (0);
}

int
getCosBySize (srb_long_t size)
{
    struct hpssCosTable *tmpTable;

    if (hpssCosHead == NULL || size < 0)
	return HpssDefCos;

    tmpTable = hpssCosHead;

    /* Insert in accending order */

    while (tmpTable != NULL) {
	if ((size / 1000) < tmpTable->maxKByteSz)
	    return (tmpTable->cos);
	tmpTable = tmpTable->next;
    }
    return HPSS_SRB_NOCOS;
}

int 
hpssMigrate (int fd, int srcLevel, int flags)
{
    u_signed64 bytesMigrated;
    int status;
    int retVal;

    status = hpss_Migrate (fd, srcLevel, flags, &bytesMigrated);

    if (status < 0) {
	retVal = hpssErrConv (status);
	return (retVal);
    }

    return (cast32m (bytesMigrated));
}

int
hpssStage (int fd, int offset, int len, int srcLevel, int flags)
{
    int status;
    int retVal;
    u_signed64 offset64, len64;

    offset64 = cast64m (offset);
    len64 = cast64m (len);

    uiSecs=MIN_30; /* only wait 30 minutes for HPSS stage, otherwise time out with error */
 
    (void)sprintf(cpFunctionName,"hpssStage(%d, %d, %d, %d, %d)", fd, offset, len, srcLevel, flags);

    signal(SIGALRM, hpss_timeout);
    
    if (setjmp(jenv) == 0) {
      alarm(uiSecs);
      status = hpss_Stage (fd, offset64, len64, srcLevel, flags); 
      alarm(0);
    } else {
      /* get there by calling longjmp */
       elog (NOTICE, "timeout because of longjmp trigered by %s. Timeout %u seconds", cpFunctionName, uiSecs);
       status=HPSS_SRB_TIMEOUT_STAGE;
       strcpy(cpFunctionName, "none_set");
       return (status);
    }
    strcpy(cpFunctionName, "none_set");
    alarm(0);

    if (status < 0) {
        retVal = hpssErrConv (status);
        return (retVal);
    }

    return (status);
}

int
hpssPurge (int fd, int offset, int len, int srcLevel, int flags)
{
    u_signed64 bytesPurged;
    int status;
    int retVal;
    u_signed64 offset64, len64;

    offset64 = cast64m (offset);
    len64 = cast64m (len);

    status = hpss_Purge (fd, offset64, len64, srcLevel, flags, &bytesPurged);

    if (status < 0) {
        retVal = hpssErrConv (status);
        return (retVal);
    }

    return (cast32m (bytesPurged));
}

int
convStat32ToStat (struct stat32 *mystat32, struct stat *mystat)
{
    if (mystat32 == NULL || mystat == NULL)
        return (-1);

    mystat->st_dev = (mystat32->st_dev);
    mystat->st_ino = (mystat32->st_ino);
    mystat->st_mode = (mystat32->st_mode);
    mystat->st_nlink = (mystat32->st_nlink);
    mystat->st_uid = (mystat32->st_uid);
    mystat->st_gid = (mystat32->st_gid);
    mystat->st_rdev = (mystat32->st_rdev);
    mystat->st_size = (mystat32->st_size);
    mystat->st_atime = (mystat32->st_atime);
    mystat->st_mtime = (mystat32->st_mtime);
    mystat->st_ctime = (mystat32->st_ctime);
    mystat->st_blksize = (mystat32->st_blksize);
    mystat->st_blocks = (mystat32->st_blocks);
#if defined(PORTNAME_aix) || defined(PORTNAME_alpha) || defined(PORTNAME_sunos) || defined(PORTNAME_c90)
    mystat->st_spare1 = (mystat32->st_spare1);
    mystat->st_spare2 = (mystat32->st_spare2);
    mystat->st_spare3 = (mystat32->st_spare3);
#endif  /* PORTNAME_aix */
#ifdef PORTNAME_aix
    mystat->st_vfstype = (mystat32->st_vfstype);
    mystat->st_vfs = (mystat32->st_vfs);
    mystat->st_type = (mystat32->st_type);
    mystat->st_access = (mystat32->st_access);
    mystat->st_flag = (mystat32->st_flag);
#endif  /* PORTNAME_aix */
#if defined(PORTNAME_aix) || defined(PORTNAME_alpha)
    mystat->st_gen = (mystat32->st_gen);
#endif  /* defined(PORTNAME_aix) || defined(PORTNAME_alpha) */
    return (0);
}

int 
hpssToSrbLong (u_signed64 *hpss64, srb_long_t *srbLong)
{
    u_signed64 *myLong;

    if (hpss64 == NULL || srbLong == NULL) 
	return -1;

    if (ntohl(1) == 1) {
	memcpy (srbLong, hpss64, sizeof (srb_long_t));
        return 0;
    }

     /* just switch the position of the high, low 32 bit int */

    myLong = (u_signed64 *) srbLong;
    myLong->high = hpss64->low;
    myLong->low = hpss64->high;

    return 0;
}
    
#endif /* HPSS */
