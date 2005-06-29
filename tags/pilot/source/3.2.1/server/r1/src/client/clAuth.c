/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/*-------------------------------------------------------------------------
 *
 * clAuth.c--
 *     The client authorization routines
 *
 *-------------------------------------------------------------------------
 */

#include "clAuth.h"
#ifdef _WIN32
#include "SrbNtUtil.h"
#include "misc_util.h"
#endif


/*
 * clSendAuth -- send the authentication info to the SRB server
 */
int
clSendAuth(StartupInfo *startup, srbConn *conn)
{
    int status;

    if (strcmp (startup->proxyUserName, TICKET_USER) == 0 ) {
	status = 0;
    } else {
      switch (conn->authSchemeInx) {
      case PASSWD_AUTH_INX:
	/* status = clSendMdasAuth(startup, conn); */
	status = CLI_AUTH_SCHEME_NOT_SUPPORTED;
	break;
      case SEA_AUTH_INX:
      case SEA_ENCRYPT_INX:
	status = CLI_AUTH_SCHEME_NOT_SUPPORTED;
	/* status = clSendSeaAuth (startup, conn); */
	break;
      case GSI_AUTH_INX:
      case GSI_SECURE_COMM_INX:
	status = clSendGsiAuth (startup, conn);
	break;
      case AUTH_ENCRYPT1_INX:
	status = clSendEncryptAuth(startup, conn);
	break;
      case GSI_DELEGATE_INX:
	status = clSendGsiDelegAuth (startup, conn);
	break;
      default:
	status = CLI_AUTH_SCHEME_NOT_SUPPORTED;
	break;
      }
    }
    return(status);
}

int
clSendMdasAuth (StartupInfo *startup, srbConn *conn)
{
    int status;
    Port        *port = conn->port;
    int retBuf;

    /* Send the passwd */

    status = send (port->sock, startup->proxyAuth, 
     sizeof(startup->proxyAuth), 0);

    if (status <= 0) {
        sprintf(conn->errorMessage,
          "clSendMdasAuth() --  couldn't send passwd packet: errno=%d\n%s\n",
            errno,strerror(errno));
	return (CLI_ERR_SEND_PACKET);
    }
    status = clRecvStatus (port->sock, conn);

    return (status);
}

int 
clRecvStatus (int sock, srbConn *conn)
{
    int status;
    int retBuf;
    /* receive status from the server */

#ifdef _WIN32
	status = recv(sock, (char *) &retBuf, 4, 0);
#else
    status = read (sock, (char *) &retBuf, 4);
#endif

    if (status <= 0) {
        sprintf(conn->errorMessage,
          "clRecvStatus() --  couldn't recv status packet: errno=%d\n%s\n",
            errno,strerror(errno));
        return CLI_ERR_RECV;
    }

#if defined(PORTNAME_c90) || defined(PORTNAME_t3e)
    status = sToL (ntohl (VARSIZE (&retBuf)));
#else
    status = ntohl (retBuf);
#endif
    return (status);
}

int 
clSendSeaAuth (StartupInfo *startup, srbConn *conn)
{
#ifdef SEA_AUTH
    char *seaUserStr;
    Port        *port = conn->port;
    int status;

    /* perform SEA Authentication if configured.
     * For now, we're doing MDAS Authentication followed by
     * SEA Auth if configured.
     * Don't do sea authentication if the user is a TICKET_USER.
     */
    seaUserStr = malloc (strlen (startup->proxyUserName) +
      strlen (startup->proxyDomainName) + 2);
    strcpy (seaUserStr, startup->proxyUserName);
    strcat (seaUserStr, "@");
    strcat (seaUserStr, startup->proxyDomainName);
    if (seaAuthClient(port->sock,seaUserStr,startup->proxyAuth)
      != STATUS_OK) {
         (void) sprintf(conn->errorMessage,
         "clSendSeaAuth() --  SEA authentication failed for %s\n", 
   	  seaUserStr);
        free (seaUserStr);
        clRecvStatus (port->sock, conn);
        return AUTH_ERR_SEA_AUTH;
    }
    free (seaUserStr);

    if (conn->authSchemeInx == SEA_ENCRYPT_INX) {
        if (seaBeginEncryptClient(port->sock)<0) {
            (void) sprintf(conn->errorMessage,
              "clSendSeaAuth() --  SEA Begin Encryption failed\n");
            return AUTH_ERR_SEA_ENC;
        }
    }
    status = clRecvStatus (port->sock, conn);
    return (status);
#else
    return AUTH_ERR_SEA_NO_SUPPORT;
#endif
}

int
clSendGsiAuth (StartupInfo *startup, srbConn *conn)
{
#ifdef GSI_AUTH
    Port        *port = conn->port;
    int status;
    char *srbServerDn;

    /* set up the GSI auth */

    if (aid_setup_creds (NULL, GSI_CLIENT_FLAG, GSI_TYPE_FLAG) != 0) {
	return (AUTH_ERR_GSI_SETUP);
    }

    /* Auth with the SRB server */

    if ((srbServerDn = clGetServerDn (conn, conn->srbHost, GSI_AUTH_INX)) 
     == NULL) {
	return (AUTH_ERR_NO_GSI_SERVER_DN);
    }

    if (aid_establish_context_clientside (port->sock, srbServerDn, 
     NO_DELEG_FLAG, GSI_TYPE_FLAG) != 0) {
	return (AUTH_ERR_GSI_AUTH);
    }

    status = clRecvStatus (port->sock, conn);
    return (status);

#else
    return AUTH_ERR_GSI_NO_SUPPORT;
#endif
}

void vCrypt1(char *to, const char *message, const char *password);

int
clSendEncryptAuth (StartupInfo *startup, srbConn *conn)
{
  int iStatus=(-1), iReadBytes1=(-1), iWriteBytes2=(-1);
  Port *port = conn->port;
  char *cpDoubleBuf, *cpChallengeMessageBuf;
  char *cpEnd, *cpScrambledMsgClient;
  char ascii_password[512];
  struct stat sStat;
  int iV_flag = 0;
  char sTmpFileName[MAX_TOKEN];
  int sizeRemaining, nbytes;
  
  cpDoubleBuf = malloc(2*CHALLENGE_MESSAGE_SIZE);
  if (cpDoubleBuf == NULL) {
    sprintf(conn->errorMessage,
	    "malloc(%d) failed in clAuth.c:clSendEncryptAuth()",
	    2*CHALLENGE_MESSAGE_SIZE );
    return(AUTH_ERR_MDAS_AUTH);
  }
  
  cpChallengeMessageBuf = cpDoubleBuf;
  cpScrambledMsgClient = cpDoubleBuf + CHALLENGE_MESSAGE_SIZE;
  
  if (startup != NULL) {
    strcpy(ascii_password, startup->proxyAuth);
  } else if (conn != NULL && conn->proxyUser != NULL &&
   conn->proxyUser->userAuth != NULL) {
    strcpy(ascii_password, conn->proxyUser->userAuth);
  } else {
    fprintf (stderr, "clSendEncryptAuth: passwd does not exist\n");
    return (AUTH_ERR_MDAS_AUTH);
  }

  
#ifdef foo	/* redo it below */
#ifdef _WIN32
  iReadBytes1 = recv(port->sock, cpChallengeMessageBuf, CHALLENGE_MESSAGE_SIZE, 0);
#else
  iReadBytes1 = read (port->sock, cpChallengeMessageBuf, CHALLENGE_MESSAGE_SIZE);
#endif  /* WIN32 */ 
#endif

  sizeRemaining = CHALLENGE_MESSAGE_SIZE;
  while (sizeRemaining > 0) {
#ifdef _WIN32
    nbytes = recv(port->sock, cpChallengeMessageBuf, sizeRemaining, 0);
#else
    nbytes = read (port->sock, cpChallengeMessageBuf, sizeRemaining);
#endif  /* WIN32 */
    if (nbytes <= 0)
        break;
    sizeRemaining -= nbytes;
  }
  iReadBytes1 = CHALLENGE_MESSAGE_SIZE - sizeRemaining;

  if (iReadBytes1 != CHALLENGE_MESSAGE_SIZE) {
    sprintf(conn->errorMessage,
	    "clAuth.c:clSendEncryptAuth(), socket read error, errno=%d, bytes read = %d, bytes expected = %d\n%s\n",
	    errno, iReadBytes1, CHALLENGE_MESSAGE_SIZE, strerror(errno));
    printf("%s\n", conn->errorMessage);
    free(cpDoubleBuf);
    return (AUTH_ERR_MDAS_AUTH);
  }
  
  vCrypt1(cpScrambledMsgClient, cpChallengeMessageBuf, ascii_password);
  
#ifdef foo	/* redo below */
#ifdef _WIN32
  iWriteBytes2 = send(port->sock, (const void *) cpScrambledMsgClient, 
				(size_t) CHALLENGE_MESSAGE_SIZE, 0);
#else
  iWriteBytes2 = write(port->sock, (const void *) cpScrambledMsgClient,
		     (size_t) CHALLENGE_MESSAGE_SIZE); 
#endif
#endif
  
  sizeRemaining = CHALLENGE_MESSAGE_SIZE;
  while (sizeRemaining > 0) {
#ifdef _WIN32
  nbytes = send(port->sock, (const void *) cpScrambledMsgClient, 
				(size_t) sizeRemaining, 0);
#else
  nbytes = write(port->sock, (const void *) cpScrambledMsgClient,
		     (size_t) sizeRemaining); 
#endif  /* WIN32 */
    if (nbytes <= 0)
        break;
    sizeRemaining -= nbytes;
  }
  iWriteBytes2 = CHALLENGE_MESSAGE_SIZE - sizeRemaining;

  if (iWriteBytes2 != CHALLENGE_MESSAGE_SIZE) {
    printf("socket write error in auth.c:chkEncryptAuth(), bytes wrote = %d, bytes expected = %d\n%s\n",
	   iWriteBytes2, CHALLENGE_MESSAGE_SIZE, strerror(errno));
    free(cpDoubleBuf);
    return(AUTH_ERR_MDAS_AUTH);
  }
  
  iStatus = clRecvStatus (port->sock, conn); 
  
  free(cpDoubleBuf);
  return (iStatus);
}

int 
initCliSvrAuthLink (int cliSock)
{
    int status, retBuf;

/* Send a msg to client requesting for Client Auth */
#ifdef _WIN32
    status = send(cliSock, (const void *) "A", (size_t) 1, 0);
#else
    status = write(cliSock, (const void *) "A", (size_t) 1);
#endif

    if (status <= 0) {
	fprintf (stderr, 
	 "svrCliSvrEncryptAuth: write init msg to client failed, errno = %d\n", 
	 errno);
	return (AUTH_ERR_MDAS_AUTH);
    }

    /* Read the reply from client */

    status = svrReceiveStatus (cliSock);

    if (status != AUTH_ENCRYPT1_INX) {
       fprintf (stderr,
         "svrCliSvrEncryptAuth: client AuthScheme %d not supported = %d\n",
         status);
        return (AUTH_ERR_MDAS_AUTH);
    }

    return (status);
}

int 
svrCliSvrEncryptAuth (int svrSock, int cliSock)
{
    int iReadBytes1=(-1), iWriteBytes2=(-1);
    char *cpChallengeMessageBuf;
    int status, retBuf;
    int sizeRemaining, nbytes;

    cpChallengeMessageBuf = malloc(2*CHALLENGE_MESSAGE_SIZE);
    if (cpChallengeMessageBuf == NULL) {
        return(AUTH_ERR_MDAS_AUTH);
    }

    /* receive the challenge msg from the server */
#ifdef foo	/* redo it below */
#ifdef _WIN32
    iReadBytes1 = recv(svrSock, cpChallengeMessageBuf, 
     CHALLENGE_MESSAGE_SIZE, 0);
#else
    iReadBytes1 = read (svrSock, cpChallengeMessageBuf, 
     CHALLENGE_MESSAGE_SIZE);
#endif  /* WIN32 */
#endif
    sizeRemaining = CHALLENGE_MESSAGE_SIZE;
    while (sizeRemaining > 0) {
#ifdef _WIN32
        nbytes = recv(svrSock, cpChallengeMessageBuf, sizeRemaining, 0);
#else
       nbytes = read (svrSock, cpChallengeMessageBuf, sizeRemaining);
#endif  /* WIN32 */
        if (nbytes <= 0)
            break;
        sizeRemaining -= nbytes;
    }
    iReadBytes1 = CHALLENGE_MESSAGE_SIZE - sizeRemaining;

    if (iReadBytes1 != CHALLENGE_MESSAGE_SIZE) {
	fprintf (stderr, 
	 "svrCliSvrEncryptAuth: read msg from server failed, errno = %d\n", 
	 errno);
	fprintf (stderr, 
          "svrCliSvrEncryptAuth(), read = %d, bytes expected = %d\n%s\n",
          iReadBytes1, CHALLENGE_MESSAGE_SIZE);
        free(cpChallengeMessageBuf);
        return (AUTH_ERR_MDAS_AUTH);
    }

    /* just forward it to the client */
#ifdef foo	/* redone below */
#ifdef _WIN32
    iWriteBytes2 = send(cliSock, (const void *) cpChallengeMessageBuf,
                                (size_t) CHALLENGE_MESSAGE_SIZE, 0);
#else
    iWriteBytes2 = write(cliSock, (const void *) cpChallengeMessageBuf,
                     (size_t) CHALLENGE_MESSAGE_SIZE);
#endif
#endif
    sizeRemaining = CHALLENGE_MESSAGE_SIZE;
    while (sizeRemaining > 0) {
#ifdef _WIN32
        nbytes = send(cliSock, (const void *) cpChallengeMessageBuf,
                                (size_t) sizeRemaining, 0);
#else
        nbytes = write(cliSock, (const void *) cpChallengeMessageBuf,
                     (size_t) sizeRemaining);
#endif  /* WIN32 */
        if (nbytes <= 0)
            break;
        sizeRemaining -= nbytes;
    }
    iWriteBytes2 = CHALLENGE_MESSAGE_SIZE - sizeRemaining;

    if (iWriteBytes2 != CHALLENGE_MESSAGE_SIZE) {
        fprintf (stderr,
         "svrCliSvrEncryptAuth: write challange msg to failed, errno = %d\n",
         errno);
        fprintf (stderr,
          "svrCliSvrEncryptAuth(), wrote = %d, bytes expected = %d\n%s\n",
          iWriteBytes2, CHALLENGE_MESSAGE_SIZE);
        free(cpChallengeMessageBuf);
        return (AUTH_ERR_MDAS_AUTH);
    }

    /* receive the scrambled msg from client */

#ifdef _WIN32
    iReadBytes1 = recv(cliSock, cpChallengeMessageBuf,
     CHALLENGE_MESSAGE_SIZE, 0);
#else
    iReadBytes1 = read (cliSock, cpChallengeMessageBuf,
     CHALLENGE_MESSAGE_SIZE);
#endif  /* WIN32 */

    if (iReadBytes1 != CHALLENGE_MESSAGE_SIZE) {
        fprintf (stderr,
         "svrCliSvrEncryptAuth:read scrambled msg failed, errno = %d\n",
         errno);
        fprintf (stderr,
          "svrCliSvrEncryptAuth(), read = %d, bytes expected = %d\n%s\n",
          iReadBytes1, CHALLENGE_MESSAGE_SIZE);
        free (cpChallengeMessageBuf);
        return (AUTH_ERR_MDAS_AUTH);
    }

    /* forward the scramled msg to the server */
#ifdef _WIN32
    iWriteBytes2 = send(svrSock, (const void *) cpChallengeMessageBuf,
                                (size_t) CHALLENGE_MESSAGE_SIZE, 0);
#else
    iWriteBytes2 = write(svrSock, (const void *) cpChallengeMessageBuf,
                     (size_t) CHALLENGE_MESSAGE_SIZE);
#endif

    if (iWriteBytes2 != CHALLENGE_MESSAGE_SIZE) {
        fprintf (stderr,
         "svrCliSvrEncryptAuth: write scrambled msg failed, errno = %d\n",
         errno);
        fprintf (stderr,
          "svrCliSvrEncryptAuth(), wrote = %d, bytes expected = %d\n%s\n",
          iWriteBytes2, CHALLENGE_MESSAGE_SIZE);
        free (cpChallengeMessageBuf);
        return (AUTH_ERR_MDAS_AUTH);
    }

    /* receive status from server */
#ifdef _WIN32
    status = recv(svrSock, (char *) &retBuf, 4, 0);
#else
    status = read (svrSock, (char *) &retBuf, 4);
#endif
    if (status <= 0) {
        fprintf (stderr,
        "svrCliSvrEncryptAuth: couldn't recv status packet: errno=%d, %s\n",
           errno,strerror(errno));
         return AUTH_ERR_MDAS_AUTH;
    }

    /* send the status to client */
#if defined(PORTNAME_c90) || defined(PORTNAME_t3e)
    status = sToL (ntohl (VARSIZE (&retBuf)));
#else
    status = ntohl (retBuf);
#endif
    svrSendStatus (cliSock, status);

    return (status);
}

/*
 * matchAuthByKeyWd - Given the auth keywd, search the auth table and
 * return the auth index.
 *
 * 
 * Input - char *name - the name of the auth service.
 * Output - The auth index
 *
 */

int
matchAuthByKeyWd(char *name, struct authsvc **myAuthsvc)
{
    int i;
    int srbAuthSvc = -1;
    
    for (i = 0; i < n_authsvcs; ++i) {
	if (!strcmp(name, authsvcs[i].keyWord)) {
	    srbAuthSvc = authsvcs[i].index;
	    *myAuthsvc = &authsvcs[i];
	    break;
	}
    }

    if (srbAuthSvc < 0)
	return (CLI_AUTH_SCHEME_NOT_SUPPORTED);

    return (srbAuthSvc);
}

char *
clAuthInxToKeyWd (int authIndex)
{
    int i;
    char *authKeyWd = NULL;

    for (i = 0; i < n_authsvcs; ++i) {
        if (authsvcs[i].index == authIndex) {
            authKeyWd = authsvcs[i].keyWord;
            break;
        }
    }

    return (authKeyWd);
}

/*
 * clGetUserName -- 
 * Input - None.
 * Output - returns the name the user to be authenticated 
 * if there is an error, return the error message in clErrorMsg
 */
char*
clGetUserName(struct mdasEnvData *myMdasEnvData, char* clErrorMsg)
{
    char *name = (char *) NULL;
    char *tmpName;
#ifdef _WIN32
    TCHAR UserName[MAX_USERNAME_LENGTH];
    DWORD dwSize = MAX_USERNAME_LENGTH;
#else
    struct passwd * pw;
#endif /* WIN32 */
    struct mdasEnvData *tmpMdasEnvData;
    
    tmpName = getenv("srbUser");
    if (tmpName != NULL && tmpName[0] != '\0') {
	name = malloc(strlen(tmpName) + 1);
	(void) strcpy(name, tmpName);
	return (name);
    }

    if (myMdasEnvData == NULL)
	tmpMdasEnvData = readMdasParam ();
    else
	tmpMdasEnvData = myMdasEnvData;

    if (tmpMdasEnvData != NULL && tmpMdasEnvData->user[0] != '\0') {
	name = strdup (tmpMdasEnvData->user);
	if (myMdasEnvData == NULL)
	    free (tmpMdasEnvData);
        return (name);
    }
    if (tmpMdasEnvData != NULL && myMdasEnvData == NULL)
        free (tmpMdasEnvData);

#ifdef _WIN32    
    if (GetUserName(UserName, &dwSize)  && 
    (name = (char *) malloc(dwSize+1))) {
	(void) strcpy(name, UserName);
    }
#else
    pw = getpwuid(getuid());
    if (pw && pw->pw_name && 
    (name = (char *) malloc(strlen(pw->pw_name) + 1))) {
	(void) strcpy(name, pw->pw_name);
    }
#endif /* WIN32 */

    if (name == NULL)
	sprintf(clErrorMsg, 
	  "clGetUserName: Unable to determine the client userName!\n");
	
    return(name);
}

/*
 * clGetDomainName -- 
 * Input - None.
 * Output - returns the domain name of the user
 * if there is an error, return the error message in clErrorMsg
 */
char*
clGetDomainName(struct mdasEnvData *myMdasEnvData, char* clErrorMsg)
{
    char *name = (char *) NULL;
    char *tmpName;
    struct passwd * pw;
    struct mdasEnvData *tmpMdasEnvData;
    
    tmpName = getenv(DOMAIN_KEYWORD);

    if (tmpName != NULL && tmpName[0] != '\0') {
	name = malloc(strlen(tmpName) + 1);
	(void) strcpy(name, tmpName);
	return (name);
    }

    if (myMdasEnvData == NULL)
        tmpMdasEnvData = readMdasParam ();
    else
        tmpMdasEnvData = myMdasEnvData;

    if (tmpMdasEnvData != NULL && tmpMdasEnvData->domain[0] != '\0') {
	name = strdup (tmpMdasEnvData->domain);
    } else {
	sprintf(clErrorMsg, 
	  "FATAL: clConnect: Unable to determine the client domainName!\n");
    }
    if (tmpMdasEnvData != NULL && myMdasEnvData == NULL)
        free (tmpMdasEnvData);
	
    return(name);
}

/*
 * clGetMcatZone -- 
 * Input - None.
 * Output - returns the mcat zone of the user
 * if there is an error, return the error message in clErrorMsg
 */
char*
clGetMcatZone(struct mdasEnvData *myMdasEnvData, char* clErrorMsg)
{
    char *name = (char *) NULL;
    char *tmpName;
    struct passwd * pw;
    struct mdasEnvData *tmpMdasEnvData;
    
    tmpName = getenv(MCAT_ZONE_KEYWORD);

    if (tmpName != NULL && tmpName[0] != '\0') {
	name = malloc(strlen(tmpName) + 1);
	(void) strcpy(name, tmpName);
	return (name);
    }

    if (myMdasEnvData == NULL)
        tmpMdasEnvData = readMdasParam ();
    else
        tmpMdasEnvData = myMdasEnvData;

    if (tmpMdasEnvData != NULL && tmpMdasEnvData->mcatZone[0] != '\0') {
	name = strdup (tmpMdasEnvData->mcatZone);
    }
    if (tmpMdasEnvData != NULL && myMdasEnvData == NULL)
        free (tmpMdasEnvData);
	
    return(name);
}

/*
 * clGetAuth -- 
 * Input - None.
 * Output - returns the passwd of the user
 * if there is an error, return the error message in clErrorMsg
 */
char*
clGetAuth (char* clErrorMsg)
{
    char *name = (char *) NULL;
    char *tmpName;
#ifdef _WIN32
    TCHAR UserName[MAX_USERNAME_LENGTH];
    DWORD dwSize = MAX_USERNAME_LENGTH;
#else
    struct passwd * pw;
#endif /* WIN32 */
    
    tmpName = getenv("srbAuth");

    if (tmpName != NULL) {
	name = malloc(strlen(tmpName) + 1);
	(void) strcpy(name, tmpName);
	return (name);
    }

    if ((name = readMdasAuth ()) == NULL) {
#ifdef _WIN32
        if (GetUserName(UserName, &dwSize)  && 
        (name = (char *) malloc(dwSize+1))) {
	    (void) strcpy(name, UserName);
        }    
#else
        pw = getpwuid(getuid());
        if (pw && pw->pw_name &&
        (name = (char *) malloc(strlen(pw->pw_name) + 1))) {
            (void) strcpy(name, pw->pw_name);
        }
#endif /* WIN32 */
    }

    return(name);
}

int initUserInfo (srbConn *conn, struct mdasEnvData *myMdasEnvData,
char* srbAuth, char *userName, char *domainName, char* clErrorMsg)
{
    int status;
    char *tmp, *tmp1;

    conn->proxyUser = (userInfo *) malloc (sizeof (userInfo));
    memset (conn->proxyUser, 0, sizeof (userInfo));

    if (userName != NULL && strcmp (userName, TICKET_USER) == 0) {
        conn->clientUser = NULL;            /* same as conn->proxyUser */
    } else if ((tmp = getenv(CLIENT_USER_KEYWORD)) && 
     (tmp1 = getenv(CLIENT_DOMAIN_KEYWORD))) {
	conn->clientUser = (userInfo *) malloc (sizeof (userInfo));
	conn->clientUser->userName = strdup (tmp);
	conn->clientUser->domainName = strdup (tmp1);
	conn->clientUser->mcatZone = NULL;
	conn->clientUser->userAuth = NULL;
	
    } else { 
        conn->clientUser = NULL;            /* same as conn->proxyUser */
    }

    /* Fill in the userInfo. For real client, conn->proxyUser and 
     * conn->clientUser  */

    if (!userName || userName[0] == '\0') {
    	conn->proxyUser->userName = clGetUserName(myMdasEnvData, clErrorMsg);
    } else
        conn->proxyUser->userName = strdup(userName);


    if (conn->proxyUser->userName == NULL) {
	free (conn->proxyUser);
	conn->proxyUser = NULL;
        return (CLI_ERR_INVAILD_USER);
    }

    if (strcmp (conn->proxyUser->userName, TICKET_USER) == 0) {
	conn->proxyUser->domainName = strdup (TICKET_USER_DOMAIN);
    } else {
    	if (!domainName || domainName[0] == '\0') {
            conn->proxyUser->domainName = clGetDomainName (myMdasEnvData, 
	     clErrorMsg);
    	} else
            conn->proxyUser->domainName = strdup(domainName);

   	if (conn->proxyUser->domainName == NULL) {
	    free (conn->proxyUser);
	    conn->proxyUser = NULL;
            return (CLI_ERR_INVAILD_DOMAIN);
    	}
	conn->proxyUser->mcatZone = clGetMcatZone (myMdasEnvData,
             clErrorMsg);
    }

    if (!srbAuth || srbAuth[0] == '\0') {
        conn->proxyUser->userAuth = clGetAuth (clErrorMsg);
    } else {
        conn->proxyUser->userAuth = strdup(srbAuth);
    }

    return (0);
}

char *
readMdasAuth ()
{
    FILE *file;
    char buf[MAXEXFPATH];
    char *retVal;
    int i;

#ifdef _SRB_WIN_SERVICE
	sprintf(buf,"%s/%s",getSrbDataDir(),CL_MDAS_AUTH_FILENAME);
#else

    if (getenv("mdasAuthFile") !=NULL) {
      retVal = getenv("mdasAuthFile");
      strcpy (buf, retVal);
    } else {
#ifdef _WIN32
        retVal =  get_home_env();
#else
        retVal =  getenv ("HOME");
#endif  /* WIN32 */

        if (retVal == NULL)
            return NULL;

        strcpy (buf, retVal);
        strcat(buf, "/");
        strcat(buf, CL_MDAS_AUTH_FILE);
    }
#endif  /* _SRB_WIN_SERVICE */


#ifdef _WIN32
	file = fp_fileopen(buf, "r");
#else
    file = fopen(buf, "r");
#endif

    buf[0] = '\0';

    if (file) {

    	/* get the string */

    	while ((i = getNextStr(file, buf, sizeof(buf))) != EOF) {

            /* Comment -- read until end of line then next line */
            if (buf[0] == '#') {
                while (getNextStr(file, buf, sizeof(buf)) == 0) ;
                continue;
            }
	    if (strlen (buf) > 0) {
	    	retVal = malloc (strlen (buf) + 1);
		strcpy (retVal, buf);
		fclose (file);
		return (retVal);
	    }
	}
        fclose (file);
    }
    return (NULL);
}

struct mdasEnvData *
readMdasParam ()
{
    FILE *file;
    char inBuf[MAXEXFPATH];
    char buf[MAXEXFPATH];
    char buf1[MAXEXFPATH];
    char *retVal;
    int i;
    struct mdasEnvData *myMdasEnvData;

    myMdasEnvData = malloc (sizeof (struct mdasEnvData));
    memset (myMdasEnvData, 0, sizeof (struct mdasEnvData));

#ifdef _SRB_WIN_SERVICE
	sprintf(buf,"%s/%s",getSrbDataDir(),CL_MDAS_ENV_FILENAME);
#else
#ifdef _WIN32
    retVal =  get_home_env();
#else
    retVal =  getenv ("HOME");
#endif
    if (getenv("mdasEnvFile") !=NULL) {
      retVal = getenv("mdasEnvFile");
      strcpy (buf, retVal);
    }
    else {
      if (retVal == NULL) {
        return myMdasEnvData;
      }
      strcpy (buf, retVal);
      strcat(buf, "/");
      strcat(buf, CL_MDAS_ENV_FILE);
    }
#endif    /* _SRB_WIN_SERVICE */

#ifdef _WIN32
    file = fp_fileopen(buf, "r");
#else
    file = fopen(buf, "r");
#endif

    buf[0] = '\0';
    buf1[0] = '\0';

    if (file) {

    	/* get the string */

    	while (fgets(inBuf, MAXEXFPATH - 1, file) != NULL) {

	    if (inBuf[0] == '#')	/* comment */
		continue;

#ifdef _WIN32
		buf[0] ='\0';
		buf1[0] = '\0';
#endif
    	    if (sscanf(inBuf, "%s '%[^']'", buf, buf1) <= 0) 
		continue;

	    if (!strcmp(buf, USER_KEYWORD)) {
	    	if (strlen (buf1) > 0) {
		    strcpy (myMdasEnvData->user, buf1);
		}
	    } else if (!strcmp(buf, DOMAIN_KEYWORD)) {
	    	if (strlen (buf1) > 0) {
		    strcpy (myMdasEnvData->domain, buf1);
		}
	    } else if (!strcmp(buf, DOMAIN_HOME_KEYWORD)) {
		/* mdasDomainName and mdasDomainHome are the same */
	    	if (strlen (buf1) > 0) {
		    strcpy (myMdasEnvData->domain, buf1);
		}
	    } else if (!strcmp(buf, DEF_COLL_KEYWORD)) {
	    	if (strlen (buf1) > 0) {
		    strcpy (myMdasEnvData->defColl, buf1);
		}
	    } else if (!strcmp(buf, DEF_RESC_KEYWORD)) {
	    	if (strlen (buf1) > 0) {
		    strcpy (myMdasEnvData->defRes, buf1);
		}
	    } else if (!strcmp(buf, HOST_KEYWORD)) {
	    	if (strlen (buf1) > 0) {
		    strcpy (myMdasEnvData->host, buf1);
		}
	    } else if (!strcmp(buf, PORT_KEYWORD)) {
	    	if (strlen (buf1) > 0) {
		    strcpy (myMdasEnvData->port, buf1);
		}
            } else if (!strcmp(buf, MCAT_ZONE_KEYWORD)) {
                if (strlen (buf1) > 0) {
                    strcpy (myMdasEnvData->mcatZone, buf1);
                }
	    } else if (!strcmp(buf, AUTH_KEYWD)) {
	    	if (strlen (buf1) > 0) {
		    strcpy (myMdasEnvData->authScheme, buf1);
		}
	    } else if (!strcmp(buf, AUTH_SERVER_KEYWD)) {
	    	if (strlen (buf1) > 0) {
		    strcpy (myMdasEnvData->authServer, buf1);
		}
	    } else if (!strcmp(buf, AUTH_SERVER_DN_KEYWD)) {
	    	if (strlen (buf1) > 0) {
		    strcpy (myMdasEnvData->authServerDn, buf1);
		}
	    } 
	}
        myMdasEnvData->retVal = 0;
	fclose (file);
    } else {
	free (myMdasEnvData);
	myMdasEnvData = NULL;
    }
    return myMdasEnvData;
}

/* clSetAuthScheme - Set the default auth scheme (DefServerAuthInfo) and
 * conn->authSchemeInx for this connection */

int 
clSetAuthScheme (srbConn *conn, struct mdasEnvData *myMdasEnvData,
char *authScheme)
{
    char *tmpAuthKeyWd = NULL;
    char *tmpAuthDn;
    char *tmpHostAddr;
    struct authsvc *myAuthsvc;
    int status;

    /* If no input authScheme and DefServerAuthInfo already been 
     * set during the last connect, just use DefServerAuthInfo 
     */

    if (DefServerAuthInfo != NULL && authScheme == NULL) {
	/* DefServerAuthInfo has been set and no authScheme */
	conn->authSchemeInx = DefServerAuthInfo->authsvc->index;
	/* free(tmpAuthKeyWd); */
	return DefServerAuthInfo->authsvc->index;
    }

    /* set the auth scheme of this connection */

    if (authScheme != NULL) {	/* using the input value first */
	/* use the input value */
	tmpAuthKeyWd = authScheme;
    } else if ((tmpAuthKeyWd = getenv (AUTH_KEYWD)) != NULL) {
	/* use the env value */
    } else if (myMdasEnvData != NULL && 
	myMdasEnvData->authScheme[0] != '\0') {
	/* use the value in .MdasEnv file */
        tmpAuthKeyWd = myMdasEnvData->authScheme;
    } else {
	/* use the default value */
        tmpAuthKeyWd = DEFAULT_CLIENT_AUTHSVC;
    }
    conn->authSchemeInx = matchAuthByKeyWd (tmpAuthKeyWd, &myAuthsvc);
    /* This line is added by Charlie C */
    status = conn->authSchemeInx;

    /* setup the DefServerAuthInfo - ServerAuthInfo of last resort */
    if (DefServerAuthInfo == NULL) {
	/* don't use the host in conn->srbHost for the Default. Use the
	 * srbHost, AuthDn pair either through env or the .MdasEnv file.
	 */ 
    	if ((tmpAuthDn = getenv (AUTH_SERVER_DN_KEYWD)) == NULL) {
	    if (myMdasEnvData != NULL) 
		tmpAuthDn = myMdasEnvData->authServerDn;
            else        
                tmpAuthDn = "";
    	}

    	if ((tmpHostAddr = getenv ("srbHost")) == NULL) {
	    if (myMdasEnvData != NULL && myMdasEnvData->host[0] != '\0') 
		tmpHostAddr = myMdasEnvData->host;
	    else
            	tmpHostAddr = conn->srbHost;
    	}


    	DefServerAuthInfo = (struct serverAuthInfo *) malloc 
         (sizeof (struct serverAuthInfo));
    	status = fillServerAuthInfo (tmpHostAddr, tmpAuthKeyWd,
         tmpAuthDn, DefServerAuthInfo);
    	if (status < 0) {
	    free (DefServerAuthInfo);
	    DefServerAuthInfo = NULL;
    	}
    }

    return (status);
}

int
fillServerAuthInfo (char *hostAddr, char *authKeyWd, char *serverDn, 
struct serverAuthInfo *serverAuthInfo)
{
    int status;
    struct authsvc *myAuthsvc;

#ifndef GSI_AUTH
    if (authKeyWd != NULL && (strcmp (authKeyWd, GSI_DELEGATE_KEYWD) == 0 ||
     strcmp (authKeyWd, GSI_AUTH_KEYWD) == 0 ||
     strcmp (authKeyWd, GSI_SECURE_COMM_KEYWD) == 0)) {
	fprintf (stderr, 
	 "fillServerAuthInfo - GSI not built into SRB for authScheme %s \n",
	  authKeyWd);
	authKeyWd = DEFAULT_CLIENT_AUTHSVC;
    }
#endif 
    if (authKeyWd == NULL)
	authKeyWd = DEFAULT_CLIENT_AUTHSVC;

    status = matchAuthByKeyWd (authKeyWd, &myAuthsvc);
    if (status < 0) {
	fprintf (stderr,
	 "fillServerAuthInfo: authKeyWd %s not supported, use default %s\n", 
	 authKeyWd, DEFAULT_CLIENT_AUTHSVC);
	authKeyWd = DEFAULT_CLIENT_AUTHSVC;
    }

    serverAuthInfo->authsvc = myAuthsvc;
    serverAuthInfo->hostAddr = strdup (hostAddr);
    serverAuthInfo->serverDn = strdup (serverDn);
    serverAuthInfo->next = NULL;

    return status;
}

int
clSetAuthOption (srbConn *conn, int authSchemeInx)
{
    conn->authSchemeInx = authSchemeInx;
    conn->srbOptions = malloc (AUTH_OPT_LEN);

    sprintf (conn->srbOptions, "%-d", authSchemeInx);

    return (0);
}

char *
clGetServerDn (srbConn *conn, char *hostAddr, int authInx)
{
    struct serverAuthInfo *tmpServerAuthInfo;
    int status;

    if (DefServerAuthInfo == NULL)	/* DefServerAuthInfo must be init */
	return NULL;

    if (authInx == DefServerAuthInfo->authsvc->index &&
     strcasecmp (hostAddr, DefServerAuthInfo->hostAddr) == 0)
	return (DefServerAuthInfo->serverDn);

    if (ServerAuthInfo == NULL) {
	status = getMcatServerAuthInfo (conn);
    }

    if (status < 0)
	return (NULL);

    tmpServerAuthInfo = ServerAuthInfo;

    while (tmpServerAuthInfo != NULL) {
    	if (authInx == tmpServerAuthInfo->authsvc->index &&
     	 strcasecmp (hostAddr, tmpServerAuthInfo->hostAddr) == 0)
            return (tmpServerAuthInfo->serverDn);

	tmpServerAuthInfo = tmpServerAuthInfo->next;
    }
    return (NULL);
}

int
getMcatServerAuthInfo (srbConn *conn)
{
   int i;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    int status;
    srbConn *defConn;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    selval[SERVER_NETPREFIX] = 1;
    selval[USER_AUTH_SCHEME] = 1;
    selval[USER_DISTIN_NAME] = 1;

    /* connect to the default server */
    defConn = clConnect (DefServerAuthInfo->hostAddr, conn->srbPort, 
     conn->proxyUser->userAuth);

    if (clStatus(defConn) != CLI_CONNECTION_OK) {
	conn->status = defConn->status;
	strcpy (conn->errorMessage, defConn->errorMessage);
	clFinish (defConn);
	return (conn->status);
    }

    if ((status = srbGetDataDirInfo(defConn, MDAS_CATALOG,
     qval, selval, &myresult, DEF_NUM_ROWS) < 0)) {
        return status;
    }

    if (myresult.row_count <= 0)
	return (MCAT_INQUIRE_ERROR);

    status = procMcatServerAuthInfo (&myresult);
    clFinish (defConn);
    return status;
}

int
procMcatServerAuthInfo (mdasC_sql_result_struct  *myresult)
{
   int i;
    int status;
    char *serverLoc, *authScheme, *dnName;
    char *tmpServerLoc, *tmpAuthScheme, *tmpDnName;
    struct serverAuthInfo *tmpInfo, *lastInfo = NULL;

    serverLoc = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[SERVER_NETPREFIX], 
		   dcs_aname[SERVER_NETPREFIX]);

    authScheme = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[USER_AUTH_SCHEME], dcs_aname[USER_AUTH_SCHEME]);

    dnName = (char *)getFromResultStruct(
                (mdasC_sql_result_struct *) myresult,
                  dcs_tname[USER_DISTIN_NAME], dcs_aname[USER_DISTIN_NAME]);

    for (i = 0; i < myresult->row_count; i++) {
	tmpInfo = (struct serverAuthInfo *) malloc
    	 (sizeof (struct serverAuthInfo));
	parseAddr (&serverLoc[i*MAX_DATA_SIZE]);
	tmpServerLoc = &serverLoc[i*MAX_DATA_SIZE];
	tmpAuthScheme = &authScheme[i*MAX_DATA_SIZE];
	tmpDnName = &dnName[i*MAX_DATA_SIZE];
	status = fillServerAuthInfo (tmpServerLoc, tmpAuthScheme,
    	 tmpDnName, tmpInfo);
	tmpInfo->next = NULL;

    	if (status < 0) {
            free (tmpInfo);
	    break;
	}

	/* Queue it */

	if (ServerAuthInfo == NULL) {
	    ServerAuthInfo = tmpInfo;
	} else {
	    lastInfo->next = tmpInfo;
	}
	lastInfo = tmpInfo;
    }

    free (serverLoc);
    free (authScheme);
    free (dnName);
	
    return (0);
}

char * getFromResultStruct(mdasC_sql_result_struct *result,
                              char *tab_name, char *att_name)
{
  int i;
  for (i = 0; i <  result->result_count ; i++){
    /* XXXXX - strstr is used because the schema name has been appended to
     * the beginning of sqlresult[i].tab_name.
     */
    if((strstr(result->sqlresult[i].tab_name,tab_name) ||
	strstr(tab_name, result->sqlresult[i].tab_name)) &&
       !strcmp(result->sqlresult[i].att_name,att_name))
      {return ((char *) result->sqlresult[i].values);}
  }
  return (NULL);
}

char *getAttributeColumn(mdasC_sql_result_struct *result, 
			int attrIndex)
{
   return(getFromResultStruct(result,
			      dcs_tname[attrIndex], 
			      dcs_aname[attrIndex]));
} 

int
clSendGsiDelegAuth (StartupInfo *startup, srbConn *conn)
{
#ifdef GSI_AUTH
    Port        *port = conn->port;
    int status;
    char srbServerDn[MAX_TOKEN];

    /* set up the GSI auth */

    if (aid_setup_creds (NULL, GSI_CLIENT_FLAG, GSI_TYPE_FLAG) != 0) {
	return (AUTH_ERR_GSI_SETUP);
    }

#ifdef foo
    /* Auth with the SRB server */

    if ((srbServerDn = clGetServerDn (conn, conn->srbHost, GSI_AUTH_INX)) 
     == NULL) {
	return (AUTH_ERR_NO_GSI_SERVER_DN);
    }
#endif

    if (aid_establish_context_clientside_without_server_dn (port->sock, 
     srbServerDn, MAX_TOKEN,
     DO_DELEG_FLAG, GSI_TYPE_FLAG) != 0) {
	return (AUTH_ERR_GSI_AUTH);
    }

    status = clRecvStatus (port->sock, conn);
    return (status);

#else
    return AUTH_ERR_GSI_NO_SUPPORT;
#endif
}

int
svrSendGsiDelegAuth (int delegateCertInx, srbConn *conn)
{
#ifdef GSI_AUTH
    Port        *port = conn->port;
    int status;
    char srbServerDn[MAX_TOKEN];

    /* set up the GSI auth */

    if (aid_setup_creds (NULL, GSI_CLIENT_FLAG, GSI_TYPE_FLAG) != 0) {
        return (AUTH_ERR_GSI_SETUP);
    }

    if (aid_establish_context_using_client_delegate (port->sock,
     NULL, srbServerDn, MAX_TOKEN, delegateCertInx,
     DO_DELEG_FLAG) != 0) {
        return (AUTH_ERR_GSI_AUTH);
    }

    status = clRecvStatus (port->sock, conn);
    return (status);

#else
    return AUTH_ERR_GSI_NO_SUPPORT;
#endif
}

