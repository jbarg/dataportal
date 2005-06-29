/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/* srbServer.c - The main code for the srbServer.
 */

/* #define SERVER_DEBUG 1 */
/* #define INSTRON 1 */		/* XXXXXXX - for timing only */

#include "srbServer.h"

int
main(int argc, char *argv[])
{
#ifdef _WIN32

    /* Sleep(60000); */   /* for debug */
    SrbNtCheckExecMode(argc,argv);

#else
    if (!DataDir)
        DataDir = getSrbDataDir();
#endif
    SrbServerMode = 1;		/* indicate it is a server */

    exitSrbServer (srbServerMain(argc, argv));
}
 
/*
 * srbServerMain - The srbServer main loop.
 *
 * Input : int argc 
 *	   char *argv[]
 *
 * Output - None (it never returns)
 */
int
srbServerMain(int argc, char *argv[])
{
    int    flagQ;
    int    flag;
    int    errs = 0;
    int    nSelected; /* number of descriptors selected */
    int    status;
#ifndef _WIN32
    int pipeFd0, pipeFd1;
#endif
    StartupInfo sp;
    int portFd;
#ifdef _WIN32
    time_t curTime;
    HANDLE hWriteToMaster;
    DWORD dwWritten;
    BOOL  fResult = 1;
#endif
 
    extern int    optind;
    extern char   *optarg;
    extern short  DebugLvl;
    Port myPort;
    int lport;
    struct sockaddr_in raddr;		/* the remote address */
    int length;
    svrComm_t *myComm;
    struct timeval connTimeout;
    fd_set      basemask;
    int dummy;
    struct allowedUser *allowedUserHead = NULL;
    int selLoopCnt = 0;
    int lsock = -1;
    int sFlag = 0;
    struct hostElement  *hostTabPtr;


#ifdef INSTRON
    int myPid = getpid();
    time_t curTime = time(NULL);
#endif
 
    /* For dbx debugging */
#ifndef _WIN32
#ifdef SERVER_DEBUG
    if (pathExist ("/tmp/srbdbx120") == 0)
	sleep (20);
#endif
#endif

#ifdef INSTRON
#if defined(PORTNAME_solaris)
    fprintf(stderr,
     "%24.24s srbServerMain: Start of srbServerMain, pid = %d, microSec=%lld\n",
       ctime(&curTime), myPid, gethrtime ());
#endif
#endif

    UserClass = CLASS_SER;

#ifdef _WIN32
    if (startWinsock() != 0)
    {
        elog(FATAL,"srbServerMain: startWinsock() failed\n");
        return(STATUS_ERROR);
    }
#else

    /* Register signal handlers.
     */

    signal(SIGINT, die);
    signal(SIGHUP, die);
    signal(SIGTERM, die);
    signal(SIGUSR1, die);
#endif
 
 
    /* parse command line arguments
     */
    flagQ = 0;
 
    while ((flag = getopt(argc, argv, "d:o:P:p:S:Qe")) != EOF)
        switch (flag) {
        case 'd':
            /* DebugMode = true;  */
            flagQ = 0;
            DebugLvl = (short)atoi(optarg);
            break;
 
        case 'o':
            /* o - send output (stdout and stderr) to a file.
             */
            (void) strncpy(OutputFileName, (char *) optarg, MAXEXFPATH);
            break;
 
        case 'p':
            /*
             *  p - Use the passed file descriptor number as the port
             *    on which to communicate with the user.
             *
             */
#ifdef _WIN32
            hWriteToMaster = (HANDLE)atoi(optarg);
#else
            pipeFd0 = atoi(optarg);
#endif
            break;

        case 'P':
            /*
             *  P - Use the passed file descriptor number as the port
             *    on which to communicate with the user.
             *
             */
#ifndef _WIN32
            pipeFd1 = atoi(optarg);
#endif
            break;
        case 'S':
	    /* use the socket from the master */
            lsock = atoi(optarg);
	    sFlag = 1;
            break;
 
        case 'Q':
            /*  Q - set flagQ mode (reduce debugging output)
             */
            flagQ = 1;
            break;

        default:
            /*  default: bad command line option
             */
            errs++;
    }

       if (errs != 0) {
            fprintf(stderr, 
	     "usage: srbServer {-Q} [-d debug] [-o logfile] -p pipeFd\n");
            exit(2);
       }

#ifdef _WIN32
/* Win32 retrieves the data dir info here. */
    if(NtGetPortNumFromArgs(argc,argv) < 0)
    {
       if(SrbNtRunInConsoleMode())
         fprintf(stderr,
	   "srbServer: cannot get default port for data dir from master");
       else
         NtEmergencyMessage(
	  "\nsrbServer: cannot get default port for data dir from master");
       lport = -1;
       WriteFile(hWriteToMaster, &lport, sizeof(lport), &dwWritten,NULL);
       exit(0);
    }

    if(SrbNtRunInConsoleMode())
    {
       if(ntSetSrbDataDir(argv[0]) < 0)
       {
          fprintf(stderr,"srbServer failed to find data directory.");
          lport = -1;
          WriteFile(hWriteToMaster, &lport, sizeof(lport), &dwWritten,NULL);
          exit(0);
       }
    }
    else
    {
       if(ntSetSrbDataDir(NULL) < 0)
       {
          NtEmergencyMessage("\nsrbServer: failed to find SRB data directory.");
          lport = -1;
          WriteFile(hWriteToMaster, &lport, sizeof(lport), &dwWritten,NULL);
          exit(0);
       }
    }
#endif
    if (sFlag == 0) {
        if ((lsock = portalCreate ()) < 0) {
#ifdef _WIN32
            elog(FATAL,
	     "srbServerMain: portalCreate Error. Status = %d\n", lsock);
#else
	    fprintf(stderr,
              "srbServerMain: portalCreate Error. Status = %d\n", lsock);
	    write (pipeFd0, &lsock, sizeof (lsock));
	    close (pipeFd1);
	    close (pipeFd0);
#endif
            return INIT_IN_PROGRESS;
        }

        /* get the port number */

        if ((lport = getPortNum (lsock)) < 0) {
#ifdef _WIN32
            elog(FATAL,"srbServerMain: getPortNum Error. errno = %d\n", errno);
            WriteFile(hWriteToMaster, &lport, sizeof(lport), &dwWritten,NULL);
#else
            fprintf(stderr,
              "srbServerMain: getPortNum Error. errno = %d\n", errno);
            write (pipeFd0, &lport, sizeof (lport));
	    close (pipeFd1);
	    close (pipeFd0);
#endif
            return INIT_IN_PROGRESS;
        }

        /* listen and accept just one connect */

        listen (lsock, 0);

        /* send the new port number to srbMaster */
#ifdef _WIN32
        fResult = WriteFile(
          hWriteToMaster, &lport, sizeof(lport), &dwWritten,NULL);
        if(fResult == 0) {
            elog(FATAL,
	     "srbServerMain: pipe write failed, errno=%d\n",GetLastError());
            return(INIT_IN_PROGRESS);
        }
#else
         while ((status = write (pipeFd0, &lport, sizeof (lport))) != 
          sizeof (lport)) {
            if (errno == EINTR) {
                continue;
            } else {
                fprintf(stderr,
                 "srbServerMain: pipe write failed, errno=%d\n", errno);
                close (pipeFd0);
                close (pipeFd1);
                return(INIT_IN_PROGRESS);
            }
        }
        close (pipeFd0); 
#endif
    } else {
	listen (lsock, 0);
    }	/* sFlag */

    setAllowedUser (&allowedUserHead);

    /* Connect to MCAT since it may be needed for authentication */
#ifdef SRB_MDAS
#ifdef INSTRON
#if defined(PORTNAME_solaris)
    fprintf(stderr,
     "%24.24s srbServerMain: Before open_db2_inter, pid = %d, microSec=%lld\n",
       ctime(&curTime), myPid, gethrtime ());
#endif
#endif
    if ((status = open_db2_interaction(MDAS_CATALOG)) != 0) {
#ifdef _WIN32
        elog(FATAL,"srbServerMain: open_db2_interaction Error. Status = %d\n",status);
#else
        fprintf(stderr,
          "srbServerMain: open_db2_interaction Error. Status = %d\n",
            status);
#endif
	exitSrbServer(INIT_IN_PROGRESS);
    }
    DbOpened = 1;
#ifdef INSTRON
#if defined(PORTNAME_solaris)
    fprintf(stderr,
     "%24.24s srbServerMain: After open_db2_inter, pid = %d, microSec=%lld\n",
       ctime(&curTime), myPid, gethrtime ());
#endif
#endif

#endif	/* SRB_MDAS */

    /* Initialize ThisUser and ClientUser */

    status = initThisUserInfo ();
    if (status < 0) {
        fprintf (stderr, "srbServerMain: initThisUserInfo error, status = %d\n",
         status);
	exitSrbServer(INIT_IN_PROGRESS);
    }


    /* Initial the host table. initSrbUserList may have to talk to
     * remote hosts.
     */
    if ((status = initSrbHost()) < 0) {
#ifdef _WIN32
        elog(FATAL,"srbServerMain: initSrbHost Error. Status = %d\n",status);
#else
        fprintf(stderr,
          "srbServerMain: initSrbHost Error. Status = %d\n",
            status);
#endif
	exitSrbServer(INIT_IN_PROGRESS);
    }

    status = initSrbUserList ();
    if (status < 0) {
#ifdef _WIN32
        elog(FATAL,
	 "srbServerMain: initSrbUserList Error. Status = %d.\n", status);
#else
        fprintf(stderr,
          "srbServerMain: initSrbUserList Error. Status = %d.\n", status);
#endif
	return (INIT_IN_PROGRESS);
    }

#ifdef _WIN32
    if(SrbNtFindServerExec(srbPathname) < 0)
#else
    if (findServerExec(srbPathname, argv[0]) < 0)
#endif
        elog(FATAL, "%s: could not locate executable, bailing out...",
             argv[0]);
 
    /*
     *  print flags
     */
    if (! flagQ) {
        puts("\t---debug info---");
        printf("\tflagQ =        %c\n", flagQ     ? 't' : 'f');
        puts("\t----------------\n");
    }
 
    /* initialize */
    if (! flagQ) {
        puts("\tinitSrbServer()..");
    }
 
#ifdef INSTRON
#if defined(PORTNAME_solaris)
    fprintf(stderr,
     "%24.24s srbServerMain: Before initSrbServer, pid = %d, microSec=%lld\n",
       ctime(&curTime), myPid, gethrtime ());
#endif
#endif


    if (initSrbServer() < 0)
	 exitSrbServer (INIT_DONE);
 
#ifdef INSTRON
#if defined(PORTNAME_solaris)
    fprintf(stderr,
     "%24.24s srbServerMain: After initSrbServer, pid = %d, microSec=%lld\n",
       ctime(&curTime), myPid, gethrtime ());
#endif
#endif

#ifndef _WIN32
    signal(SIGPIPE, sPipSigHandler);
#endif


    /* The socket will be close when srbMaster scheduled this server */

#ifndef _WIN32
    if (sFlag == 0) {
#ifdef SRB_MDAS
	/* close connection to local MCAT */
        if (LocalHostHead->mdasEnabled != 1) {
            status = getMdasEnabledHost (NULL, &hostTabPtr);
            remoteDisconnect (hostTabPtr);
        }

        if (PIPE_READ_TIMEOUT > 0) {
	    signal (SIGALRM, pipeReadAlarmSigHandler);
            alarm (PIPE_READ_TIMEOUT);
        }
        if (ServerTimeOut > 0 || MCAT_CONN_TIMEOUT > 0) {
            McatTimeoutCount = 0;
        }
#endif	/* SRB_MDAS */
        /* read (pipeFd1, (char *) &dummy, sizeof (dummy)); */
        status = read (pipeFd1, (char *) &dummy, sizeof (dummy));
#ifdef SRB_MDAS
        alarm (0);
        if (DbOpened <= 0 &&
         (status = open_db2_interaction(MDAS_CATALOG)) != 0) {
            fprintf(stderr,
              "srbServerMain: open_db2_interaction Error. Status = %d\n",
                status);
            return (INIT_IN_PROGRESS);
            /* exitSrbServer(INIT_IN_PROGRESS); */
        }
        DbOpened = 1;
#endif	/* SRB_MDAS */
    }
#endif    /* _WIN32 */ 

    FD_ZERO(&basemask);
    FD_SET(lsock, &basemask);

#ifndef _WIN32	/* XXXXX take out the timeout. The select seems to get
		 * into an infinite loop on linux */
    connTimeout.tv_sec = SVR_CONN_TIMEOUT;
    connTimeout.tv_usec = 0;
#endif	/* _WIN32 */

    SvrInitialized = 1;		/* The server has been initialized */
    while ((nSelected = select(lsock + 1, &basemask,
                            (fd_set *) NULL,
                            (fd_set *) NULL,
/* XXXX take out select timeout for now. Cause out of file descriptor error 
            &connTimeout)) <= 0) {
*/
                            NULL)) <= 0) {
	selLoopCnt ++;
        if (errno == EINTR && selLoopCnt < 100) {
            continue;
	}
	if (errno != ENOTTY) {	/* solaris got this error when timed out */
            elog (NOTICE, 
	     "serverLoop: select failed, errno=%d. selLoopCnt = %d. retry\n", 
	     errno, selLoopCnt);
	}
        return(INIT_IN_PROGRESS);
    }

    if (sFlag == 0) {
#ifndef _WIN32
        status = close (pipeFd1);
#endif

        length = sizeof(struct sockaddr_in);
#if defined(PORTNAME_aix)
        portFd = accept (lsock, (struct sockaddr  *) &raddr, 
	 (unsigned long*) &length);
#else
        portFd = accept (lsock, (struct sockaddr  *) &raddr, &length);
#endif


        if (portFd < 0) {
#ifdef _WIN32
            elog(FATAL,
	     "srbServerMain: accept error, errno=%d, %s\n",
	      errno, strerror(errno));
#else
	    fprintf(stderr,
             "srbServerMain: accept error, errno=%d, %s\n",
              errno, strerror(errno));
#endif
            return INIT_IN_PROGRESS;
        }
#ifdef _WIN32
        closesocket(lsock);
#else
        close (lsock);
#endif
    } else {	/* sFlag */
	portFd = lsock;
    }

    if (portFd == 0)    /* XXXX don't know why a particular linux system
                         * returns a 0 value */
        portFd = dup (0);


    myPort.sock = portFd;
    myPort.nBytes = 0;

    /* read the first msg */

     status = packetReceive (&myPort, NON_BLOCKING);
    if (status < 0) {
#ifdef _WIN32
        elog(FATAL,"srbServerMain: packetReceive error, status=%d",status);
#else
        struct sockaddr_in raddr;
#if defined(PORTNAME_aix)
        size_t      laddrlen = sizeof(struct sockaddr);
#else
        int         laddrlen = sizeof(struct sockaddr);
#endif
        if (getpeername(portFd, (struct sockaddr *) &raddr,
          &laddrlen) < 0) {
            fprintf(stderr,
             "srbServerMain: packetReceive error, status=%d\n", status);
	} else {
            fprintf(stderr,
             "srbServerMain: packetReceive error from %s, status=%d\n", 
	     inet_ntoa (raddr.sin_addr), status);
	}
#endif
	svrSendStatus (portFd, status);
        return INIT_IN_PROGRESS;
    }

    status = procStartupMsg (&myPort, &sp);

    if (status < 0) {
#ifdef _WIN32
        elog(FATAL,"srbServerMain: procStartupMsg error, status=%d\n",status);
#else
        struct sockaddr_in raddr;
#if defined(PORTNAME_aix)
        size_t      laddrlen = sizeof(struct sockaddr);
#else
        int         laddrlen = sizeof(struct sockaddr);
#endif
        if (getpeername(portFd, (struct sockaddr *) &raddr,
          &laddrlen) < 0) {
            fprintf(stderr,
             "srbServerMain: procStartupMsg error, status=%d\n", status);
	} else {
            fprintf(stderr,
             "srbServerMain: procStartupMsg error from %s, status=%d\n", 
	     inet_ntoa (raddr.sin_addr), status);
	}
#endif
	svrSendStatus (portFd, status);
        return INIT_IN_PROGRESS;
    }

    status = chkAllowedUser (allowedUserHead, ClientUser->userName,
     ClientUser->domainName);
    if (status <= 0) {
        svrSendStatus (portFd, SYS_IN_MAINTENENCE_MODE);
        return (INIT_IN_PROGRESS);
    }
 
#ifdef foo	/* done in chkAllowedUser */
    /* check whether a user is allowed to connect */

    if (allowedUserHead != NULL) {	/* have limitation */
	struct allowedUser *tmpAllowedUser;

	/* noone is allowed */
	if (allowedUserHead->next == NULL && 
	  allowedUserHead->userName == NULL) {
	    svrSendStatus (portFd, SYS_IN_MAINTENENCE_MODE);
	    return (INIT_IN_PROGRESS);
	}
#ifdef SRB_MDAS
	/* allow ticket user in because it is used for ENCRYPT1 auth */
        if (strcmp (ClientUser->userName, TICKET_USER) != 0) {
#endif
	    tmpAllowedUser = allowedUserHead;
	    while (tmpAllowedUser != NULL) {
		if (tmpAllowedUser->userName != NULL && 
		 strcmp (tmpAllowedUser->userName, ClientUser->userName) == 0 &&
		 tmpAllowedUser->domainName != NULL &&
		 strcmp (tmpAllowedUser->domainName, ClientUser->domainName) 
		 == 0) {
		    /* we have a match */
		    break;
		}
		tmpAllowedUser = tmpAllowedUser->next;
	    }
	    if (tmpAllowedUser == NULL) {
		/* no match */
	        svrSendStatus (portFd, SYS_IN_MAINTENENCE_MODE);
	        return (INIT_IN_PROGRESS);
	    }
	
#ifdef SRB_MDAS
	}
#endif
    }
#endif /* foo */

    /* Do Hostbased auth first */

    if (HostBasedAuth) {
        if ((status = chkHostBasedAuth(&raddr, &sp)) != STATUS_OK) {
#ifdef _WIN32
        elog(FATAL,"srbServerMain: host-based authentication failed.");
#else
            (void) fprintf(stderr,
              "srbServerMain: host-based authentication failed\n");
#endif
	    svrSendStatus (portFd, AUTH_ERR_HOST_BASE_AUTH);
            return(INIT_IN_PROGRESS);
        }
    }

    svrSendStatus (portFd, 0);	/* send status to client */

#ifdef INSTRON
#if defined(PORTNAME_solaris)
    fprintf(stderr,
     "%24.24s srbServerMain: Before svrCheckAuth, pid = %d, microSec=%lld\n",
       ctime(&curTime), myPid, gethrtime ());
#endif
#endif

    if (sFlag == 0) {
        elog (NOTICE, 
	 "Connection from %s by PUser %s@%s Cuser %s@%s, pid = %d",
          inet_ntoa (raddr.sin_addr), 
          sp.proxyUserName, sp.proxyDomainName,
          ClientUser->userName, ClientUser->domainName,
          getpid ());
    } else {
	int laddrlen = sizeof(struct sockaddr);
#if defined(PORTNAME_aix)
	if (getpeername (portFd, (struct sockaddr*) &raddr, 
	 (unsigned long*) &laddrlen) < 0) {
#else
	if (getpeername (portFd, (struct sockaddr*) &raddr, &laddrlen) < 0) {
#endif
            elog (NOTICE, 
	     "getpeername() failed: errno=%d\n%s\n",
              errno, strerror(errno));
	} else {
            elog (NOTICE, 
             "Connection from %s by PUser %s@%s Cuser %s@%s, pid = %d",
              inet_ntoa (raddr.sin_addr),
              sp.proxyUserName, sp.proxyDomainName,
              ClientUser->userName, ClientUser->domainName,
              getpid ());
	}
    }

    myComm = (svrComm_t *) malloc (sizeof (svrComm_t));
    commInit(myComm, portFd);

    /* Check the client Authentication */

    status = svrCheckAuth (portFd, getenv ("proxyUserName"), 
     getenv ("proxyDomainName"), getenv ("proxyMcatZone"));

#ifdef INSTRON
#if defined(PORTNAME_solaris)
    fprintf(stderr,
     "%24.24s srbServerMain: After svrCheckAuth, pid = %d, microSec=%lld\n",
       ctime(&curTime), myPid, gethrtime ());
#endif
#endif

    if (status < 0) {
#ifdef _WIN32
    elog(FATAL,"srbServerMain: svrCheckAuth error. Status = %d\n",status);
#else
	fprintf(stderr, "srbServerMain: svrCheckAuth error. Status = %d\n",
            status);
#endif
	exitSrbServer(INIT_IN_PROGRESS);
    }

    status = recvAndProcCommand (myComm, flagQ);
    exitSrbServer (status);
    return (status);

}

/*
 *  receiveCommand - srbServer command processing loop 
 */

int
recvAndProcCommand (svrComm_t *myComm, int    flagQ)
{
    char   firstchar;
    char   parser_input[MAX_PARSE_BUFFER];
    int status;
    int eofCnt = 0;

    if (getenv ("SingleSvrPort") != NULL) {
	SingleSvrPort = atoi (getenv ("SingleSvrPort"));
    } else {
	SingleSvrPort = 0;
    }

#ifndef _WIN32
    /* Initialize ServerTimeOut */
    if (getenv ("ServerTimeOut") != NULL) {
	ServerTimeOut = atoi (getenv ("ServerTimeOut"));
	if (ServerTimeOut < MIN_SERVER_TIMEOUT_TIME) {
	    fprintf (stderr, 
	     "ServerTimeOut %d is less than min of %d\n", 
	     ServerTimeOut, MIN_SERVER_TIMEOUT_TIME);
	    ServerTimeOut = MIN_SERVER_TIMEOUT_TIME;
	} 
        signal (SIGALRM, alarmSigHandler);
#ifdef SRB_MDAS
	MaxMcatTimeoutForServer = ServerTimeOut / MCAT_CONN_TIMEOUT;
	if (MaxMcatTimeoutForServer <= 0) {
	    MaxMcatTimeoutForServer = 1;
	}
#else
	MaxMcatTimeoutForServer = -1;
#endif
    } else {
	ServerTimeOut = 0;
    }
#endif

    for (;;) {
 
        /*
         * read a command.
         */
        memset(parser_input, 0, MAX_PARSE_BUFFER);
 
#ifndef _WIN32
        if (MaxMcatTimeoutForServer > 0 && MCAT_CONN_TIMEOUT > 0) {
            alarm (MCAT_CONN_TIMEOUT);
        } else if (ServerTimeOut > 0) {
	    alarm (ServerTimeOut);
        }
#endif
        firstchar = readCommand(myComm, parser_input);
#ifndef _WIN32
	if (ServerTimeOut > 0 || MCAT_CONN_TIMEOUT > 0) {
	    McatTimeoutCount = 0;
	    alarm (0);
	}
#endif
#ifdef SRB_MDAS
        if (DbOpened <= 0 && 
	 (status = open_db2_interaction(MDAS_CATALOG)) != 0) {
#ifdef _WIN32
            elog(FATAL,
	     "srbServerMain: open_db2_interaction Error. Status = %d\n",
	     status);
#else
	    fprintf(stderr,
              "srbServerMain: open_db2_interaction Error. Status = %d\n",
                status);
#endif
	    return (INIT_IN_PROGRESS);
            /* exitSrbServer(INIT_IN_PROGRESS); */
        }
        DbOpened = 1;
#endif	/* SRB_MDAS */

        /* process the command */
        switch (firstchar) {
            /*  'F' indicates a function call. Call handleFunctionRequest
             */
        case 'F':
 
            if (! flagQ) {
                time(&tim);
                printf("\tStartTransactionCommand() at %s\n", ctime(&tim));
            }
 
            handleFunctionRequest(myComm);
            break;

            /*  'X' means that the client is closing down the socket
             */
        case 'X':
            commClose(myComm);
	    return (INIT_DONE);
	    /* exitSrbServer (INIT_DONE); */
            break;
 
        default:
	    eofCnt ++;
	    if (eofCnt > 100) {
		return (INIT_DONE);
                /* exitSrbServer (INIT_DONE); */
	    }
	    break;
        }
    }
}

/* Signal handler for several signals
 *
 * Input : None
 *
 * Output : None
 */

void
die()
{
    fprintf (stderr, "pid %d caught a signal and exiting\n", getpid()); 
    exitSrbServer(INIT_DONE);
}

void
sPipSigHandler ()
{
    elog (NOTICE, 
      "Pid = %d. Broken pipe. Sockect connection to Client may be broken",
      getpid ());
    exitSrbServer(INIT_DONE);
}

#ifndef _WIN32
void
#if defined(PORTNAME_linux) || defined(PORTNAME_aix) || defined(PORTNAME_c90) || defined(PORTNAME_solaris) || defined(PORTNAME_dgux) || defined(PORTNAME_linux)
alarmSigHandler (int i)
#else
alarmSigHandler ()
#endif
{
#ifdef SRB_MDAS
    if (MCAT_CONN_TIMEOUT > 0) {
        if (DbOpened > 0) {
	    commit_db2_interaction(MDAS_CAT_COMMIT);
            close_db2_interaction(MDAS_CAT_COMMIT);
            DbOpened = 0;
            elog (NOTICE,
              "Pid = %d disconnect to MCAT due to inaction for %d sec",
              getpid (), MCAT_CONN_TIMEOUT);
        }
	McatTimeoutCount++;
    }
#endif

    if (ServerTimeOut > 0 && (MaxMcatTimeoutForServer < 0 || 
     McatTimeoutCount >= MaxMcatTimeoutForServer)) {
        elog (NOTICE,
          "Pid = %d disconnected due to inaction for %d sec",
          getpid (), ServerTimeOut);
        exitSrbServer(INIT_DONE);
    }
    signal (SIGALRM, alarmSigHandler);
    if (MaxMcatTimeoutForServer > 0 && MCAT_CONN_TIMEOUT > 0) {
        alarm (MCAT_CONN_TIMEOUT);
    } else if (ServerTimeOut > 0) {
        alarm (ServerTimeOut);
    }
}
#endif

/* exitSrbServer - srbServer exit routine.
 *
 * Input : int status - The status of the exit.
 */

void
exitSrbServer(int status)
{
/* Replaced --- exitSrb(status);
*/
    struct hostElement  *tmpHElement;

    if (status != INIT_IN_PROGRESS)
    	closeAllDesc ();            /* Close all opened obj descriptor */
#ifdef HPSS
#ifndef NO_DCE
    if (DebugLvl > 1)
    	elog (NOTICE, "exitSrbServer: Will do a PurgeLoginContext");
    PurgeLoginContext();
#endif
#endif

#ifdef SRB_MDAS
    if (DbOpened > 0)
        close_db2_interaction(MDAS_CAT_COMMIT);
#endif

#ifdef _WIN32
    cleanWinsock();
#endif
    /* disconnect all opened socket to other servers */

    tmpHElement = RemoteHostHead;
    while (tmpHElement != NULL) {
	remoteDisconnect (tmpHElement);
	tmpHElement = tmpHElement->next;
    }

    tmpHElement = LocalHostHead;
    while (tmpHElement != NULL) {
        remoteDisconnect (tmpHElement);
        tmpHElement = tmpHElement->next;
    }

    exit (status);

}

int
setAllowedUser (struct allowedUser **allowedUserHead)
{
    struct allowedUser *tmpAllowedUser;

    char *conFile;
    FILE *file;
    char buf[MAX_LINE_SZ];
    int i;

    *allowedUserHead = NULL;
    conFile = (char *) malloc((strlen(getSrbDataDir()) +
     strlen(MAINTENENCE_CONFIG_FILE)+2)*sizeof(char));

    sprintf (conFile, "%s/%s", getSrbDataDir(), MAINTENENCE_CONFIG_FILE);
    file = fopen(conFile, "r");

    if (file == NULL) {
        if (DebugLvl > 1)
            fprintf(stderr, "Unable to open MAINTENENCE_CONFIG_FILE file %s\n",
	     conFile);
        free (conFile);
        return (0);
    }

    free (conFile);

    while ((i = getNextStr(file, buf, sizeof(buf))) != EOF) {
        char myuser[MAX_TOKEN];
        char mydomain[MAX_TOKEN];

        /* Read until end of line then next line */

        if (buf[0] == '#')
        {
            while (i == 0)
                i = getNextStr(file, buf, sizeof(buf));
            continue;
        }

	if (mySplitStr (buf, myuser, mydomain,'@') != NULL) {
	    tmpAllowedUser = malloc (sizeof (struct allowedUser));
	    memset (tmpAllowedUser, 0, sizeof (struct allowedUser));
	    tmpAllowedUser->userName = strdup (myuser);
	    tmpAllowedUser->domainName = strdup (mydomain);
	    /* queue it */

	    if (*allowedUserHead == NULL) {
		*allowedUserHead = tmpAllowedUser;
	    } else {
		tmpAllowedUser->next = *allowedUserHead;
		*allowedUserHead = tmpAllowedUser;
	    }
	}

        while (i == 0) {        /* read to end of line */
            i = getNextStr(file, buf, sizeof(buf));
            if (mySplitStr (buf, myuser, mydomain,'@') != NULL) {
                tmpAllowedUser = malloc (sizeof (struct allowedUser));
                memset (tmpAllowedUser, 0, sizeof (struct allowedUser));
                tmpAllowedUser->userName = strdup (myuser);
                tmpAllowedUser->domainName = strdup (mydomain);
                /* queue it */

                if (*allowedUserHead == NULL) {
                *allowedUserHead = tmpAllowedUser;
                } else {
                    tmpAllowedUser->next = *allowedUserHead;
                    *allowedUserHead = tmpAllowedUser;
                }
            }
	}
    }
    fclose (file);
    if (*allowedUserHead == NULL) {
	/* make an empty struct --> noone is allowed to use */
	*allowedUserHead = malloc (sizeof (struct allowedUser));
        memset (*allowedUserHead, 0, sizeof (struct allowedUser));
    }
    return (0);
}
int
chkAllowedUser (struct allowedUser *allowedUserHead, char *userName,
char *domainName)
{
    struct allowedUser *tmpAllowedUser;

    if (allowedUserHead == NULL) {
	/* no limit */
	return (1);
    }

    if (userName == NULL || domainName == 0) {
	return (0);
    }

    /* noone is allowed */
    if (allowedUserHead->next == NULL &&
      allowedUserHead->userName == NULL) {
	return (0);
    }

#ifdef SRB_MDAS
    /* allow ticket user in because it is used for ENCRYPT1 auth */
    if (strcmp (userName, TICKET_USER) != 0) {
#endif
        tmpAllowedUser = allowedUserHead;
        while (tmpAllowedUser != NULL) {
            if (tmpAllowedUser->userName != NULL &&
             strcmp (tmpAllowedUser->userName, userName) == 0 &&
             tmpAllowedUser->domainName != NULL &&
             strcmp (tmpAllowedUser->domainName, domainName)
             == 0) {
                /* we have a match */
                break;
            }
            tmpAllowedUser = tmpAllowedUser->next;
        }
        if (tmpAllowedUser == NULL) {
            /* no match */
	    return (0);
        } else {
	    return (1);
	}
#ifdef SRB_MDAS
    }
    return (1);
#endif
}

#ifndef _WIN32
void
#if defined(PORTNAME_linux) || defined(PORTNAME_aix) || defined(PORTNAME_c90) || defined(PORTNAME_solaris) || defined(PORTNAME_dgux) || defined(PORTNAME_linux)
pipeReadAlarmSigHandler (int i)
#else
pipeReadAlarmSigHandler ()
#endif
{
#ifdef SRB_MDAS
    if (PIPE_READ_TIMEOUT > 0) {
        if (DbOpened > 0) {
            close_db2_interaction(MDAS_CAT_COMMIT);
            DbOpened = 0;
        }
    }
#endif
}
#endif	/* _WIN32 */

