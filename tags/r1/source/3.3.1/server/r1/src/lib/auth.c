/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*
 *
 * auth.c--
 *    Routines for authentication
 *
 *
 */

#include "auth.h"
void Crypt1(char *to,const char *message,const char *password);

int svrCheckAuth (int sock, char *userName, char *domainName, char *mcatZone)
{
    int status;

    if (strcmp (userName, TICKET_USER) == 0) {	/* skip TICKET_USER */
	status = 0;
    } else {
      int authSchemeInx;

      authSchemeInx = svrGetAuthScheme (CLIENT_MODE);
      switch (authSchemeInx) {
      case PASSWD_AUTH_INX:
	status = CLI_AUTH_SCHEME_NOT_SUPPORTED;
	/* status = chkMdasAuth (sock, userName, domainName); */
	break;
      case SEA_AUTH_INX:
	status = CLI_AUTH_SCHEME_NOT_SUPPORTED;
	/* status = chkSeaAuth (sock, userName, domainName); */
	break;
      case SEA_ENCRYPT_INX:
	status = CLI_AUTH_SCHEME_NOT_SUPPORTED;
	/* status = chkSeaAuth (sock, userName, domainName); 
	if (status < 0)
	  break;
	status = enableSeaEncrypt (sock, userName, domainName); */
	break;
      case GSI_AUTH_INX:
      case GSI_SECURE_COMM_INX:
	status = chkGsiAuth (sock, userName, domainName);
	break;
      case AUTH_ENCRYPT1_INX:
	status = chkEncryptAuth (sock, userName, domainName, mcatZone);
	break;
      case GSI_DELEGATE_INX:
	status = chkGsiDelegAuth (sock, userName, domainName);
	break;
      default:
	status = CLI_AUTH_SCHEME_NOT_SUPPORTED;
	break;
      }

      if (status < 0) {
#ifdef FED_MCAT
	fprintf (stderr,
	 "svrCheckAuth: Auth error for %s@%s-%s. status = %d\n", 
	  userName, domainName, mcatZone, status);
#endif
      } else {
        /* check for cross zone authentication */

	 int retBuf;

         if (TicketUserFlag == 0 && 
          (strcmp (ProxyUser->mcatZone, ClientUser->mcatZone) != 0 &&
          strcmp (ProxyUser->mcatZone, 
	  LocalMcat->serverAuthInfo->mcatName) != 0)) {
	    svrSendStatus (sock, CLIENT_AUTH_REQUIRED);
#ifdef _WIN32
            status = recv(sock, (char *) &retBuf, 4, 0);
#else
            status = read (sock, (char *) &retBuf, 4);
#endif
	    if (status <= 0) {
               fprintf (stderr, 
		"svrCheckAuth: couldn't recv status packet: errno=%d, %s\n",
                errno,strerror(errno));
               return CLI_ERR_RECV;
            }

#if defined(PORTNAME_c90) || defined(PORTNAME_t3e)
            authSchemeInx = sToL (ntohl (VARSIZE (&retBuf)));
#else
            authSchemeInx = ntohl (retBuf);
#endif
            switch (authSchemeInx) {

             case GSI_DELEGATE_INX:
               status = chkGsiDelegAuth (sock, userName, domainName);
               break;
             case AUTH_ENCRYPT1_INX:
               status = chkEncryptAuth (sock, ClientUser->userName, 
                 ClientUser->domainName, ClientUser->mcatZone);
               break;
             default:
               fprintf (stderr,
                "svrCheckAuth: cross zone client auth failed. status = %d \n",
                authSchemeInx);
               status = CLIENT_CROSS_ZONE_AUTH_FAILED;
               break;
            }
         }
      }

      /* send status to client */
      svrSendStatus (sock, status);
    }

    return(status);
}

int 
chkMdasAuth (int sock, char *userName, char *mdasDomain)
{
    char srbAuth[NAMEDATALEN];
    int status;

    /* read the passwd */

#ifdef _WIN32
    status = recv (sock,srbAuth, sizeof(srbAuth),0);
#else
    status = read (sock, srbAuth, sizeof(srbAuth));
#endif

    if (status <= 0) {
	fprintf(stderr, 
          "chkMdasAuth() --  couldn't read passwd packet: errno=%d\n%s\n",
            errno,strerror(errno));
        return (SYS_ERR_RECV_PACKET);
    }

    status = _svrChkMdasAuth (userName, srbAuth, mdasDomain);

    return (status);

#ifdef foo	/* This section of code replaced by _svrChkMdasAuth */
    if (LocalHostHead->mdasEnabled == 1) {      /* Can talk to mdas */
#ifdef SRB_MDAS
	if ((status = get_mdas_authorization 
	  (userName, srbAuth, mdasDomain)) < 0) {
	    (void) fprintf(stderr,
              "chkMdasAuth: %s@%s MDAS authentication failed, status = %d\n",
		userName, mdasDomain, status);
            return(AUTH_ERR_MDAS_AUTH);
	} else {
	    return(STATUS_OK);
	}
#else
	return(STATUS_OK);
#endif
    } else {		/* Do a remote authorization */
        struct hostElement  *hostTabPtr;
	int remoteFlag;

	remoteFlag = getMdasEnabledHost (NULL, &hostTabPtr);
        if (remoteFlag < 0) {
            (void) fprintf(stderr,
            "chkMdasAuth: No MdasEnabled host\n");
            return(STATUS_ERROR);
        }

        if ((status = remoteConnect (hostTabPtr)) < 0) {
            return(status);
        }

        if ((status = srbChkMdasAuth (hostTabPtr->conn, userName, srbAuth, 
	  mdasDomain)) < 0) {
            elog(NOTICE,
              "connStartup: %s MDAS authentication failed, stat=%d, errno=%d",
		userName, status, errno);
	    if (status == CLI_ERR_BROKEN_PIPE) {
		remoteClose (hostTabPtr);
                if (remoteConnect (hostTabPtr) < 0) {
                    return(STATUS_ERROR);
                }
		/* try again */
		if ((status = srbChkMdasAuth (hostTabPtr->conn, 
		  userName, srbAuth, mdasDomain)) < 0) {
		    return(AUTH_ERR_MDAS_AUTH);
		} else {
		    failureCnt = 0;
		    return(STATUS_OK);
		}
	    } else {
	        if (failureCnt < 0)
		    failureCnt = 1;
	        else 
		    failureCnt++;
	        if (failureCnt > MAX_AUTH_FAILURE) {
	            remoteClose (hostTabPtr);
		    failureCnt = 0;
		}
		return(AUTH_ERR_MDAS_AUTH);
	    }
        } else {
	    failureCnt = 0;
            return(STATUS_OK);
        }
    }
#endif	/* foo */
}

int
chkSeaAuth (int sock, char *userName, char *mdasDomain)
{
#ifdef SEA_AUTH
    char seaNamebuf[NAMEDATALEN];
    int status;
    char        namebuf[NAMEDATALEN];

    strcpy (namebuf, userName);
    strcat (namebuf, "@");
    strcat (namebuf, mdasDomain);

    if (DebugLvl) {
        /* XXXX - remember to take it out */
        /* seaSetShowNet(-1);   uncomment to display sea messages to stdout */
        /*
        fprintf(stderr, "pre-SeaAuth packet->proxyUserName=%s\n",
          packet->proxyUserName);
        fprintf(stderr, "packet options=%s\n",packet->options);
        */
    }
    if (seaAuthServer(sock,seaNamebuf, NAMEDATALEN - 1) != STATUS_OK){
        fprintf(stderr,
          "seaAuthServer error for %s\n", namebuf);
        return(AUTH_ERR_SEA_AUTH);
    }
    if (strcmp (seaNamebuf, namebuf) != 0) {
        fprintf(stderr,
          "seaAuthServer error: Sea User %s does not match Proxy User %s\n",
            seaNamebuf, namebuf);
        return(AUTH_ERR_SEA_AUTH);
    }
    if (DebugLvl >= 2) {
        fprintf(stderr, "post-SeaAuth User=%s\n",
          namebuf);
    }
    return (0);
#else
    return AUTH_ERR_SEA_NO_SUPPORT;
#endif
}

int
chkGsiAuth (int sock, char *userName, char *mdasDomain)
{
#ifdef GSI_AUTH
    char gsiDnBuf[MAX_TOKEN + 1];
    int status;
    char *mcatAuthDn;
    int i;

    /* set up the GSI auth */

    if (aid_setup_creds (NULL, GSI_SERVER_FLAG, GSI_TYPE_FLAG) != 0) {
        return (AUTH_ERR_GSI_SETUP);
    }

    if ((status = aid_establish_context_serverside (sock, gsiDnBuf,
     MAX_TOKEN, GSI_TYPE_FLAG)) != 0) {
	fprintf(stderr, "Gsi Auth error. status = %d\n", status);
        return (AUTH_ERR_GSI_AUTH);
    }

    /* Check the Dn with MCAT */

    status = getAuthDnFromMCAT (userName, mdasDomain, GSI_AUTH_KEYWD,
    &mcatAuthDn);
    
    if (status < 0)
	return (AUTH_ERR_NO_GSI_DN_IN_MCAT);
    
    for (i = 0; i < status; i++) {
    	/* if (strcmp (&mcatAuthDn[i*MAX_DATA_SIZE], gsiDnBuf) == 0) {  */
    	if (compareGSIDn (&mcatAuthDn[i*MAX_DATA_SIZE], gsiDnBuf) > 0) { 
	    free (mcatAuthDn);
	    return (0);
	}
    }
	
    /* no match */


    if (status > 0) {
    	fprintf (stderr, "chkGsiAuth: Dn mismatch, from MCAT-%s, from GSI-%s\n",
	 mcatAuthDn, gsiDnBuf);
	free (mcatAuthDn);
    } else {
	fprintf (stderr, "chkGsiAuth: Dn mismatch, from GSI-%s\n", gsiDnBuf);
    }

    return (AUTH_ERR_GSI_DN_MISMATCH);

#else
    return AUTH_ERR_GSI_NO_SUPPORT;
#endif
}

/* compareGSIDn - compare the GSI Dn using keywords "O=", "OU=", and 
 * "USERID=" (or "UID=")
 * return(1) if match, 0 - no match 
 */
int
compareGSIDn (char *dn1, char *dn2)
{
    if (strcmp (dn1, dn2) == 0)
	return (1);

    if (compareGSIDnByKeywd (dn1, dn2, "O=") <= 0)
	return (0);

    if (compareGSIDnByKeywd (dn1, dn2, "OU=") <= 0)
        return (0);

    if (compareGSIDnByMultiKeywd (dn1, dn2, "USERID=", "UID=") <= 0) 
        return (0);

    return (1);
}

int
compareGSIDnByKeywd (char *dn1, char *dn2, char *keywd)
{
    char *tmpptr1, *tmpptr2;
    char *ptr1, *ptr2;
    int len1, len2;
    int c;

    ptr1 = strstr (dn1, keywd);
    if (ptr1 == NULL)
        return (0);

    tmpptr1 = ptr1;
    len1 = 0;
    while ((c = *tmpptr1) != '\0') {
        if (c == '/')
            break;
        tmpptr1++;
        len1++;
    }

    if (len1 <= 0)
        return (0);

    ptr2 = strstr (dn2, keywd);
    if (ptr2 == NULL)
        return (0);

    tmpptr2 = ptr2;
    len2 = 0;
    while ((c = *tmpptr2) != '\0') {
        if (c == '/')
            break;
        tmpptr2++;
        len2++;
    }

    if (len2 <= 0)
        return (0);

    if (len1 != len2) 
        return (0);

    if (strncmp (ptr1, ptr2, len1) == 0)
	return(1);
    else
        return (0);
}


/* 
   Compare DN keywords where two keywords are synonymous.  In
   particular, a newer GSI (GT3) uses a newer OpenSSL which defines
   an object as UID, whereas NPACI code (and some Globus code) had to
   set this up to be USERID.  The easiest solution is simply to
   equivalence them here, so that if the object id converted to either
   string, it is understood to be the same thing.  See
   <http://bugzilla.globus.org/bugzilla/show_bug.cgi?id=575>.
*/
int
compareGSIDnByMultiKeywd (char *dn1, char *dn2, char *keywd1, char *keywd2)
{
    char *tmpptr1, *tmpptr2;
    char *ptr1, *ptr2;
    int len1, len2;
    int c;

    ptr1 = strstr (dn1, keywd1);
    if (ptr1 == NULL) {
        ptr1 = strstr (dn1, keywd2);
        if (ptr1 == NULL) {
           return (0);
        }
        ptr1 += strlen(keywd2);
    }
    else {
        ptr1 += strlen(keywd1);
    }
    
    tmpptr1 = ptr1;
    len1 = 0;
    while ((c = *tmpptr1) != '\0') {
        if (c == '/')
            break;
        tmpptr1++;
        len1++;
    }

    if (len1 <= 0) {
        return (0);
    }

    ptr2 = strstr (dn2, keywd1);
    if (ptr2 == NULL) {
        ptr2 = strstr (dn2, keywd2);
        if (ptr2 == NULL) {
            return (0);
        }
        ptr2 += strlen(keywd2);
    }
    else {
        ptr2 += strlen(keywd1);
    }

    tmpptr2 = ptr2;
    len2 = 0;
    while ((c = *tmpptr2) != '\0') {
        if (c == '/')
            break;
        tmpptr2++;
        len2++;
    }

    if (len2 <= 0) {
        return (0);
    }

    if (len1 != len2) {
        return (0);
    }

    if (strncmp (ptr1, ptr2, len1) == 0) {
        return(1);
    }
    else {
        return (0);
    }
}

int chkEncryptAuth (int sock, char *userName, char *mdasDomain,
char *mcatZone)
{
  int status=(-1);
  long int liTmp;
  int writebytes, iReadBytes1=0, iRand;
  char *cpTripleBuf, *cpChallengeMessageBuf, *cpScrambledMsgFromClient;    
  char *cpEnd, cVal;
  static int failureCnt;
  struct hostElement  *hostTabPtr;
  int remoteFlag;
  int sizeRemaining, nbytes;

  cpTripleBuf = malloc(3*CHALLENGE_MESSAGE_SIZE);
  if (cpTripleBuf == NULL) {
    elog(NOTICE, "malloc(%d) failed in auth.c:chkEncryptAuth()",
	 3*CHALLENGE_MESSAGE_SIZE );
    return(AUTH_ERR_ENCRYPT_AUTH);
  }
  cpChallengeMessageBuf = cpTripleBuf;
  cpScrambledMsgFromClient = cpTripleBuf+CHALLENGE_MESSAGE_SIZE;
  
  /* (void) srandom(time(0)); */
  for(iRand=0; iRand < CHALLENGE_MESSAGE_SIZE; iRand++){
    cpEnd= (cpChallengeMessageBuf+iRand);
    liTmp = (long int) random();
    cVal = (char)liTmp;
    if(cVal != '\0')
      *cpEnd = cVal;
    else
      *cpEnd = 'g';
  }
  *cpEnd = '\0'; 
  
#ifdef foo	/* redone below */
#ifdef _WIN32
  writebytes = send(sock, (const void *) cpChallengeMessageBuf,
             (size_t) CHALLENGE_MESSAGE_SIZE, 0);
#else
  writebytes = write(sock, (const void *) cpChallengeMessageBuf, 
		     (size_t) CHALLENGE_MESSAGE_SIZE); 
#endif
#endif
  sizeRemaining = CHALLENGE_MESSAGE_SIZE;
  while (sizeRemaining > 0) {
#ifdef _WIN32
  nbytes = send(sock, (const void *) cpChallengeMessageBuf,
                                (size_t) sizeRemaining, 0);
#else
  nbytes = write(sock, (const void *) cpChallengeMessageBuf,
                     (size_t) sizeRemaining);
#endif  /* WIN32 */
    if (nbytes <= 0)
        break;
    sizeRemaining -= nbytes;
  }
  writebytes = CHALLENGE_MESSAGE_SIZE - sizeRemaining;

  if (writebytes != CHALLENGE_MESSAGE_SIZE) {
    elog(NOTICE,
     "chkEncryptAuth(), socket write error. bytes written = %d, expected = %d",
	 writebytes, CHALLENGE_MESSAGE_SIZE);
    free(cpTripleBuf); 
    return(AUTH_ERR_ENCRYPT_AUTH);
  }
#ifdef foo	/* redo it below */
#ifdef _WIN32
  iReadBytes1 = recv (sock, cpScrambledMsgFromClient, CHALLENGE_MESSAGE_SIZE, 0);
#else
  iReadBytes1 = read (sock, cpScrambledMsgFromClient, CHALLENGE_MESSAGE_SIZE);
#endif
#endif

  sizeRemaining = CHALLENGE_MESSAGE_SIZE;
  while (sizeRemaining > 0) {
#ifdef _WIN32
    nbytes = recv(sock, cpScrambledMsgFromClient, sizeRemaining, 0);
#else
    nbytes = read (sock, cpScrambledMsgFromClient, sizeRemaining);
#endif  /* WIN32 */
    if (nbytes <= 0)
        break;
    sizeRemaining -= nbytes;
  }
  iReadBytes1 = CHALLENGE_MESSAGE_SIZE - sizeRemaining;

  if (iReadBytes1 != CHALLENGE_MESSAGE_SIZE) {
    elog(NOTICE,
      "chkEncryptAuth(), socket read error. bytes read = %d, expected = %d", 
	 iReadBytes1, CHALLENGE_MESSAGE_SIZE);
    free(cpTripleBuf); 
    return (AUTH_ERR_ENCRYPT_AUTH);
  }
  cpScrambledMsgFromClient[iReadBytes1] = '\0'; 

  /* remoteFlag = getMdasEnabledHost (getenv ("proxyMcatZone"), &hostTabPtr); */
  remoteFlag = getMdasEnabledHost (mcatZone, &hostTabPtr);
  if (remoteFlag < 0) {
      status = remoteFlag;
  } else if (remoteFlag == LOCAL_HOST) {      /* Can talk to mdas */
    status = _svrChkEncryptAuth (userName, mdasDomain,
                           CHALLENGE_MESSAGE_SIZE, CHALLENGE_MESSAGE_SIZE,
                           cpChallengeMessageBuf,
                           cpScrambledMsgFromClient); 

    free(cpTripleBuf); 
    return (status);
  } else {		/* Do a remote authorization */
    /* have to user ticket user of connect can go back and forth 
     * in a infinite loop */

    if ((status = remoteTiConnect (hostTabPtr)) < 0) {
      elog (NOTICE,"chkEncryptAuth: error in remoteConnect ()");
      return(status);
    }
    if ((status = srbChkEncryptAuth (hostTabPtr->conn, userName, 
			   mdasDomain, 
			   CHALLENGE_MESSAGE_SIZE, CHALLENGE_MESSAGE_SIZE,
			   cpChallengeMessageBuf, 
			   cpScrambledMsgFromClient)) < 0) {
      elog (NOTICE, 
        "chkEncryptAuth: srbChkEncryptAuth error for %s@%s,stat=%d, errno=%d",
	userName, mdasDomain, status, errno);

      if (status == CLI_ERR_BROKEN_PIPE) {
          remoteClose (hostTabPtr);
          if (remoteConnect (hostTabPtr) < 0) {
              return(STATUS_ERROR);
          }

          /* try again */
          if ((status = srbChkEncryptAuth (hostTabPtr->conn,
			   userName,
                           mdasDomain,
                           CHALLENGE_MESSAGE_SIZE, CHALLENGE_MESSAGE_SIZE,
                           cpChallengeMessageBuf,
                           cpScrambledMsgFromClient)) < 0) {
              remoteClose (hostTabPtr);
              return(AUTH_ERR_ENCRYPT_AUTH);
          } else {
              failureCnt = 0;
              remoteClose (hostTabPtr);
              return(STATUS_OK);
          }
      } else {
          if (failureCnt < 0)
              failureCnt = 1;
          else
              failureCnt++;
          if (failureCnt > MAX_AUTH_FAILURE) {
              remoteClose (hostTabPtr);
              failureCnt = 0;
	  }
          remoteClose (hostTabPtr);
          return(AUTH_ERR_ENCRYPT_AUTH);
      }
    } else {
      failureCnt = 0;
      remoteClose (hostTabPtr);
      return(STATUS_OK);
    }
  }
  return(STATUS_OK);
}

int
enableSeaEncrypt (int sock, char *userName, char *mdasDomain)
{
#ifdef SEA_AUTH
    char        seaNamebuf[NAMEDATALEN];

        strcpy (seaNamebuf, userName);
        strcat (seaNamebuf, "@");
        strcat (seaNamebuf, mdasDomain);

    /* XXXX - take it out */
    /* seaSetShowNet(-1);  uncomment for sea messages to stdout */
    if (seaBeginEncryptServer(sock, seaNamebuf) != STATUS_OK) {
        return(AUTH_ERR_SEA_ENC);
    }
    fprintf(stderr,"SEA Encryption Enabled\n");
    return (0);
#else
    return AUTH_ERR_SEA_NO_SUPPORT;
#endif
}

int
getAuthDnFromMCAT (char *userName, char *domainName, char *authScheme,
char **mcatDnOutput)
{
    int i;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  *myresult = NULL;
    int status;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[USER_DISTIN_NAME] = 1;
    sprintf(qval[USER_NAME]," =  '%s'",userName);
    sprintf(qval[DOMAIN_DESC]," =  '%s'",domainName);
    sprintf(qval[USER_AUTH_SCHEME]," =  '%s'",authScheme);

    /* status = queryDataInfo (getenv ("proxyMcatZone"), selval, qval, */ 
    /* get it from local MCAT */
    status = queryDataInfo (NULL, selval, qval, 
     &myresult, DEF_NUM_ROWS);

    if (status == MCAT_INQUIRE_ERROR) {
	status = queryDataInfo (getenv ("proxyMcatZone"), selval, qval,
	 &myresult, DEF_NUM_ROWS);
    }

    if (status < 0) {
        elog (NOTICE, "getAuthDnFromMCAT: queryDataInfo error. status = %d", 
	 status);
	return status;
    }

    *mcatDnOutput = (char *)getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[USER_DISTIN_NAME], 
		   dcs_aname[USER_DISTIN_NAME]);
    if (*mcatDnOutput == NULL)
	status = 0;
    else
        status = myresult->row_count;
    free (myresult);
    return(status);
}

int 
getUserAuthSchemeCnt (char *userName, char *domainName)
{
    mdasC_sql_result_struct *outresult = NULL;
    int status, authCount;

    status = getAuthSchemeFromMCAT (userName, domainName, &outresult);

    if (status < 0 || outresult == NULL) {
	return (0);
    } else {
	authCount = outresult->row_count;
	freeSqlResult (outresult);
	return (authCount);
    }
}
	
int
getAuthSchemeFromMCAT (char *userName, char *domainName, 
mdasC_sql_result_struct  **outresult)
{
    int i;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  *myresult = NULL;
    int status;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[USER_DISTIN_NAME] = 1;
    selval[USER_AUTH_SCHEME] = 1;
    sprintf(qval[USER_NAME]," =  '%s'",userName);
    sprintf(qval[DOMAIN_DESC]," =  '%s'",domainName);

    /* get it from local MCAT */
    status = queryDataInfo (NULL, selval, qval, 
     &myresult, DEF_NUM_ROWS);

    if (status == MCAT_INQUIRE_ERROR) {
	status = queryDataInfo (getenv ("proxyMcatZone"), selval, qval,
	 &myresult, DEF_NUM_ROWS);
    }

    if (status < 0) {
	*outresult = NULL;
    } else {
        *outresult = myresult;
    }
    return(status);
}

int
chkGsiDelegAuth (int sock, char *userName, char *mdasDomain)
{
#ifdef GSI_AUTH
    char gsiDnBuf[MAX_TOKEN + 1];
    int status;
    char *mcatAuthDn;
    int i;
    char *clUserName, *clDomainName;

    clUserName = ClientUser->userName;
    clDomainName = ClientUser->domainName;

    /* set up the GSI auth */

    if (aid_setup_creds (NULL, GSI_SERVER_FLAG, GSI_TYPE_FLAG) != 0) {
        return (AUTH_ERR_GSI_SETUP);
    }

    if ((status = aid_establish_context_serverside_with_delegate (sock, 
     gsiDnBuf, MAX_TOKEN, GSI_TYPE_FLAG)) != 0) {
	fprintf(stderr, "Gsi Auth error. status = %d\n", status);
        return (AUTH_ERR_GSI_AUTH);
    }

    /* Check the Dn with MCAT */

    status = getAuthDnFromMCAT (clUserName, clDomainName, GSI_AUTH_KEYWD,
    &mcatAuthDn);
    
    if (status < 0)
	return (AUTH_ERR_NO_GSI_DN_IN_MCAT);
    
    /* save the delegateCertInx */

    if (ProxyUser != NULL && ProxyUser->myComm != NULL) {
	ProxyUser->myComm->delegateCertInx = sock;
	ClientCertInx = sock;
    } 

    for (i = 0; i < status; i++) {
    	/* if (strcmp (&mcatAuthDn[i*MAX_DATA_SIZE], gsiDnBuf) == 0) {  */
    	if (compareGSIDn (&mcatAuthDn[i*MAX_DATA_SIZE], gsiDnBuf) > 0) { 
	    free (mcatAuthDn);
	    return (0);
	}
    }
	
    /* no match */


    if (status > 0) {
    	fprintf (stderr, "chkGsiAuth: Dn mismatch, from MCAT-%s, from GSI-%s\n",
	 mcatAuthDn, gsiDnBuf);
	free (mcatAuthDn);
    } else {
	fprintf (stderr, "chkGsiAuth: Dn mismatch, from GSI-%s\n", gsiDnBuf);
    }

    return (AUTH_ERR_GSI_DN_MISMATCH);

#else
    return AUTH_ERR_GSI_NO_SUPPORT;
#endif
}

