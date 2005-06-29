/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
/*
 * srbMaster.c - The main SRB master routines.
 */

#include "srbMaster.h"
#ifdef _WIN32
#include <stdio.h>
#include <process.h>
#include <direct.h>
#include <io.h>
#include "SrbNtUtil.h"
#include "startsock.h"
#include "ntFileDvr.h"
#include "misc_util.h"
#include "SrbNtConst.h"
static void srbWinMain(int aargc,char **aargv);
static int doExec(int portFd);
char *DefaultPort=NULL;
#else
#include <setjmp.h>
#define PIPE_READ_TIMEOUT	2	/* 2 seconds timeout */
jmp_buf Jenv;
#endif
 
/* #define INSTRON 1 */               /* XXXXXXX - for timing only */

#ifndef _WIN32
void
pipeReadTimeout (int sig)
{
    alarm (0);
    longjmp (Jenv, 1);
}
#endif	/* _WIN32 */

int
main(int argc, char *argv[])
{
#ifdef _WIN32
    srbWinMain(argc,argv);
    elog(NOTICE,"srbMasterMain exit.\n");
#else

    int status;
 
    status = srbMasterMain(argc, argv);
    fprintf (stderr, "srbMasterMain exit, status = %d\n", status);
    exit (status);
#endif

/*
#if defined(PORTNAME_solaris) && defined(DB_Oracle)
    _exit (status);
#else 
    exit (status);
#endif
*/
}

/* The srbMaster main routine.
 *
 * Input - int argc
 *	   char *argv[]
 * Output - Should never return 
 */
int
srbMasterMain(int argc, char *argv[])
{
    int opt;
    char        *hostName;
    int         status;
    int         silentflag = 0;
    char        hostbuf[MAXHOSTNAMELEN];
 
    progname = argv[0];

    initThisUserInfo ();	/* Initialize ThisUser */

#ifdef _WIN32
    /* Since we are going to run as a service in NT, we pause the process until
     * all network is available after the machine is rebooted.
     */
    if(!SrbNtRunInConsoleMode())
        Sleep(120000);
#endif
 
#ifndef _WIN32
    UserClass = CLASS_MAS;
    /* for security, no dir or file created can be group or other accessible */
    (void) umask((mode_t) 0077);
#endif
 
    if (!(hostName = getenv("srbHost"))) {
        if (gethostname(hostbuf, MAXHOSTNAMELEN) < 0)
            (void) strcpy(hostbuf, "localhost");
        hostName = hostbuf;
    }
 
    opterr = 0;
    /* Set debug level to minimal logging */
    DebugLvl = 1;
    while ((opt = getopt(argc, argv, "D:d:no:p:S")) != EOF) {
        switch (opt) {
        case 'D':
            /* Set the DataDir */
            DataDir = optarg;
            break;
        case 'd':
            /*
             * Set debugging level for the srbMaster and the srbServer
             */
            /** if ((optind < argc) && *argv[optind] != '-') {
             **   DebugLvl = atoi(argv[optind]);
             **   optind++;
             ** }
             ** else
             **   DebugLvl = 1;
	     **/
	    DebugLvl = atoi(optarg);
            break;
        case 'o':
            /*
             * Other options to pass to the srbServer - for debugging.
             */
            (void) strcat(extraOptions, " ");
            (void) strcat(extraOptions, optarg);
            break;
        case 'p':
            /* Set srbPort */
            masterPortName = (short) atoi(optarg);
            break;
        case 'S':
            /*
             * Start in silent mode (disassociate from controlling tty).
	     * All output goes to SRB_LOGFILE.
             */
            silentflag = 1;
            break;
        default:
            /* usage() never returns */
            usage(progname);
            break;
        }
    }
    if (masterPortName == -1)
        masterPortName = commGetport();
 
    if (!DataDir)
        DataDir = getSrbDataDir();
 
    /*
     * check whether the data directory exists.
     */
    if (pathExist(DataDir)) {
#ifdef _WIN32
    elog(FATAL,"pathExist: Data directory not found \"%s\"\n",DataDir);
#else
	fprintf(stderr, "pathExist: Data directory not found \"%s\"\n",
         DataDir);
#endif
        exit (1);
    }

#ifdef _WIN32
    if(SrbNtFindServerExec(execFile) < 0)
    {
       elog(FATAL,"%s: could not find srbServer to execute...\n",argv[0]);
       exit(1);
    }
#else
    if (!execFile[0] && findServerExec(execFile, argv[0]) < 0) 
    {
        fprintf(stderr, "%s: could not find srbServer to execute...\n",
                argv[0]);
        exit(1);
    }
#endif
 
    /* Open a srbMaster stream "listening" port */
    status = streamServerPort(hostName, masterPortName, &serverSock);
    if (status != STATUS_OK) {
#ifdef _WIN32
        elog(FATAL,"srbMasterMain: cannot create srbServer stream port\n");
#else
        fprintf(stderr, "srbMasterMain: cannot create srbServer stream port\n");
#endif
        exit(1);
    }

#ifdef _WIN32
    srbDaemonize(SRB_LOGFILE);
#else 
    if (silentflag)
        srbDaemonize(SRB_LOGFILE);
#ifdef PORTNAME_osx
    signal(SIGINT, (void *)srbDie);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGHUP, (void *)srbDie);
    signal(SIGTERM, (void *)srbDie);
#else
    signal(SIGINT, srbDie);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGHUP, srbDie);
    signal(SIGTERM, srbDie);
#endif

#endif
 
#ifdef SRB_MDAS
    if ((status = open_db2_interaction(MDAS_CATALOG)) != 0) {
#ifdef _WIN32
        elog(FATAL,"srbMasterMain: open_db2_interaction Error. Status = %d\n",status);
#else
        fprintf(stderr, 
	  "srbMasterMain: open_db2_interaction Error. Status = %d\n",
            status);
#endif
	exit (1);
    }
#endif


#ifdef foo
#ifndef _WIN32
#ifdef PORTNAME_osx
    signal(SIGCHLD, (void *)reaper);
#else
    signal(SIGCHLD, reaper);
#endif
#endif
#endif

/* log the server timeout */

#ifndef _WIN32
    /* Initialize ServerTimeOut */
    if (getenv ("ServerTimeOut") != NULL) {
	int serverTimeOut;
        serverTimeOut = atoi (getenv ("ServerTimeOut"));
        if (serverTimeOut < MIN_SERVER_TIMEOUT_TIME) {
            fprintf (stderr,
             "ServerTimeOut %d is less than min of %d\n",
             serverTimeOut, MIN_SERVER_TIMEOUT_TIME);
            serverTimeOut = MIN_SERVER_TIMEOUT_TIME;
        }
        fprintf (stderr,
         "ServerTimeOut has been set to %d seconds\n",
         serverTimeOut);
    }
#endif

    status = serverLoop();
 
    exitSrbMaster(status != STATUS_OK);
    return 0; /* not reached */
}

/* serverLoop - this is the main srbMaster Loop.
 * It loops forever.
 *
 * Input - None.
 *
 * Output - None.
 *
 */

static int
serverLoop()
{
    int         serverFd = serverSock;
    fd_set      basemask;
    int         nSockets, nSelected, status;
#ifndef _WIN32
    sigset_t oldsigmask, newsigmask;
#endif
    char *tmp;
    struct passwd * pw;
    Port *myPort;
    int preSpawnCnt;
    int i;
 
    /* Initialize the host table */

    status = initSrbHost ();

    if (status < 0)
	return status;

    prtLocalHost ();	/* Print the local host name */
    prtVaultConf ();	/* Print the Storage vault info. */

    /* fprintf (stderr, "srbMaster version %s is up.\n", SRB_VERSION); */
    elog (NOTICE, "srbMaster version %s is up.", SRB_VERSION);

    nSockets = serverSock + 1;
    FD_ZERO(&basemask);
    FD_SET(serverSock, &basemask);
 
#ifndef _WIN32
    sigprocmask(0,0,&oldsigmask);
    sigemptyset(&newsigmask);
    sigaddset(&newsigmask,SIGCHLD);
#endif

    if ((tmp = getenv (PRE_SPAWN_CNT)) != NULL) {
	preSpawnCnt = atoi (tmp);
    } else {
	preSpawnCnt = DEF_PRE_SPAWN_CNT;
    }

/* for performance concern, we don't need pre-spawn feature in Windows. */
#ifdef _WIN32
    preSpawnCnt = 0;
#endif
	 
    for (i = 0; i < preSpawnCnt; i++) {
	myPort = (Port *) malloc (sizeof(Port));
	myPort->sock = -1;
	myPort->preSpawn = -1;
        while ((status = serverStartup (myPort)) < 0) {
#ifdef _WIN32
            elog(NOTICE,"serverLoop: serverStartup failure, status = %d\n", status);
            /* Sleep(1000*STARTUP_RETRY_TIME); */
#else
            fprintf(stderr,
             "serverLoop: serverStartup failure, status = %d\n", status);
            /* sleep (STARTUP_RETRY_TIME); */
#endif
        }
	enquePort (myPort);
    }

    if (preSpawnCnt <= 0) {	/* need to pre-allocate a Port */
	myPort = (Port *) malloc (sizeof(Port));
        myPort->sock = -1;
        myPort->preSpawn = -1;
    }

#if defined(SRB_SECURE_COMM)
    i = sscEstablishRsaKeypair(1); /* create a new RSA keypair */
#ifdef _WIN32
    if (i > 0) elog(NOTICE,"sscEstablishRsaKeypair took: %d milliseconds\n",i);
    else elog(NOTICE,"sscEstablishRsaKeypair error: %d\n",i);
#else
    if (i>0)fprintf(stderr,"sscEstablishRsaKeypair took: %d milliseconds\n",i);
    else fprintf(stderr,"sscEstablishRsaKeypair error: %d\n",i);
#endif
#endif

    for (;;) {
        /*
         * To avoid race conditions, block SIGCHLD signals while we are
         * handling the request.
         */
#ifdef INSTRON
        int initloopTime = time(0);
#endif



#ifndef _WIN32
        sigprocmask(SIG_BLOCK, &newsigmask, &oldsigmask);
#endif


        while ((nSelected = select(nSockets, &basemask,
                                (fd_set *) NULL,
                                (fd_set *) NULL,
                                (struct timeval *) NULL)) < 0) {

#ifdef _WIN32
            if(WSAGetLastError() == WSAEINTR)
#else
            if (errno == EINTR) 
#endif
            {
#ifdef _WIN32
                elog(WARN,"serverLoop: serverLoop: select failed");
#else
                fprintf(stderr, "serverLoop: select interrupted, \n");
#endif
                continue;
	        }
#ifdef _WIN32
            elog(FATAL,"serverLoop: select failed\n");
#else
            fprintf(stderr, "serverLoop: select failed\n");
#endif
            return(STATUS_ERROR);
        }

#ifdef INSTRON
        elog (NOTICE,
         "serverLoop: start of select loop, select time = %d", 
	  time (0) - initloopTime);
        initloopTime = time (0);
#endif

	/* Spawn a server if preSpawnCnt == 0 */

 	if (preSpawnCnt <= 0 && myPort->preSpawn < 0) { 
	    while ((status = serverStartup (myPort)) < 0) {
#ifdef _WIN32
            elog(FATAL,"serverLoop: serverStartup failure, status = %d\n", status);
            Sleep(1000*STARTUP_RETRY_TIME);
#else
	    	fprintf(stderr, 
		 "serverLoop: serverStartup failure, status = %d\n", status);
	        sleep (STARTUP_RETRY_TIME);
#endif
	    }
	    enquePort (myPort);
	}
	    
#ifdef INSTRON
        elog (NOTICE,
         "serverLoop: after serverStartup");
#endif

	myPort = popPort ();
	if (myPort == NULL) {
#ifdef _WIN32
            elog(FATAL,"serverLoop: No sever in queue\n");
#else
	    fprintf(stderr, "serverLoop: No sever in queue\n");
#endif
	    return(STATUS_ERROR);
        }

        if (DebugLvl > 1) {
#ifdef _WIN32
            elog(NOTICE,"serverLoop: serverLoop: %d sockets pending\n",nSelected);
#else
            fprintf(stderr, "serverLoop: serverLoop: %d sockets pending\n", 
	     nSelected);
#endif
        }

 
        /* New connection pending on our well-known port's socket */

	if (streamConnection(serverFd, myPort) == STATUS_OK) {
            if (DebugLvl > 1)
#ifdef _WIN32
            elog(NOTICE,"serverLoop: connect on %d\n", myPort->sock);
#else
                fprintf(stderr, "serverLoop: connect on %d\n", myPort->sock);
#endif
        } else {
#ifdef _WIN32
            elog(WARN,"serverLoop: streamConnection error status = %d\n",status);
#else
            fprintf(stderr, "serverLoop: streamConnection error status = %d\n",
	     status);
#endif
	    enquePort (myPort);
	    continue;
	}

#ifdef INSTRON
        elog (NOTICE,
         "serverLoop: after streamConnection");
#endif

        if (DebugLvl > 1)
#ifdef _WIN32
            elog(NOTICE,"serverLoop:\t\thandling %d\n", myPort->sock);
#else
            fprintf(stderr, "serverLoop:\t\thandling %d\n", myPort->sock);
#endif
 
        /*
         * Read the incoming packet into its packet buffer.
         * Read the connection id out of the packet so we
         * know who the packet is from.
         */
#ifdef _WIN32
        status = recv(myPort->sock, myPort->buf.data,strlen (STARTUP_HEADER) + 1, 0);
#else
        status = read (myPort->sock, myPort->buf.data, 
	strlen (STARTUP_HEADER) + 1);
#endif

#ifdef INSTRON
        elog (NOTICE,
         "serverLoop: after reading STARTUP_HEADER, status = %d", status);
#endif
	/* Check the msg */
	if ((status == strlen (STARTUP_HEADER) + 1) &&
	 strcmp (myPort->buf.data, STARTUP_HEADER) == 0) {

	    /* Inform the srbServer that it is about to be scheduled */
#ifndef _WIN32
	    close (myPort->pipeFd1);
#endif

    	    /* Send the status to the client */
            svrSendStatus (myPort->sock, myPort->srbPort);
            streamClose(myPort->sock);
	    myPort->preSpawn = -1;	/* mark a new srbServer is needed */
	    myPort->sock = -1;
	    myPort->pipeFd1 = -1;
	    myPort->next = NULL;
	 } else {
#ifdef _WIN32
            elog(NOTICE,"serverLoop: Bad packet:%10s\n",myPort->buf.data);
#else
	    fprintf(stderr, "serverLoop: Bad packet:%10s\n", 
	     myPort->buf.data);
#endif
            svrSendStatus (myPort->sock, STATUS_BAD_PACKET);
            streamClose(myPort->sock);
            myPort->preSpawn = 1;       /* mark no srbServer is needed */
            enquePort (myPort);
        }
#ifdef INSTRON
        elog (NOTICE,
         "serverLoop: after svrSendStatus");
#endif

	/* restore processing SIGCHLD */
#ifndef _WIN32
        sigprocmask(SIG_SETMASK,&oldsigmask,0);
#endif

#if defined(PORTNAME_linux) || defined(PORTNAME_aix) || defined(PORTNAME_c90) || defined(PORTNAME_solaris) || defined(PORTNAME_dgux)
        reaper(0);
#else
        reaper();
#endif
	/* Spawn a new srbServer */
        if (preSpawnCnt > 0 && myPort->preSpawn < 0) {
            while ((status = serverStartup (myPort)) < 0) {
#ifdef _WIN32
            elog(NOTICE,"serverLoop: serverStartup failure, status = %d\n", status);
            Sleep(1000*STARTUP_RETRY_TIME);
#else
                fprintf(stderr,
                 "serverLoop: serverStartup failure, status = %d\n", status);
                sleep (STARTUP_RETRY_TIME);
#endif
            }
            enquePort (myPort);
        }
#ifdef INSTRON
        elog (NOTICE,
         "serverLoop: after 2nd serverStartup, masterLoop time = %d",
	 time (0) - initloopTime);
#endif
    }
}

/* reaper - SIGCHLD signal handling routine.
 */

static void
#if defined(PORTNAME_linux) || defined(PORTNAME_aix) || defined(PORTNAME_c90) || defined(PORTNAME_solaris) || defined(PORTNAME_dgux)
reaper(int i)
#else
reaper()
#endif
{
#ifndef _WIN32
    int status;         /* srbServer exit status */
    int pid;            /* process id of dead srbServer */
 
    if (DebugLvl > 1)
        fprintf(stderr, "reaper: Reaping dead processes\n");
    while((pid = waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0)
        cleanupProc(pid, status);
#ifdef PORTNAME_osx
    signal(SIGCHLD, (void *)reaper);
#else
    signal(SIGCHLD, reaper);
#endif
#endif
}

/*
 * cleanupProc -- Cleanup after terminated srbServer.
 *
 * Input - int pid - The pid of the srbServer.
 *	   int exitstatus - the exit status of the child - srbServer.
 *
 * Output - None.
 */
static void
cleanupProc(int pid,
            int exitstatus)   
{
    int         sig;
   
    if (DebugLvl) {
        /* fprintf(stderr, "cleanupProc: pid %d exited with status %d\n",
                pid, exitstatus); */
	/* having elog in a signal handler is not good because it
	 * call ctime() which in solaris calls getenv() which will
	 * lock a mutex. If getenv() was interrupted, a deadlock
	 * could occur.
	 */
        elog (NOTICE, "cleanupProc: pid %d exited with status %d",
                pid, exitstatus);
    }
    return;
}

/*
 * srbDie -- signal handler for SIGINT, SIGHUP and SIGTERM.
 */

static void
#if defined(PORTNAME_linux) || defined(PORTNAME_aix) || defined(PORTNAME_c90) || defined(PORTNAME_solaris) || defined(PORTNAME_dgux) || defined(PORTNAME_linux)
srbDie(int i)
#else
srbDie()
#endif
{
/*
#if defined(PORTNAME_solaris) && defined(DB_Oracle)
    _exit (0);
#else 
    exit (0);
#endif
*/
    fprintf (stderr, "srbMaster caught signal %d\n", i);
    exit (0);
}

/* usage - srbMaster usage description
 *
 * Input - None
 * Output - none
 */

static void
usage(char *progname)
{
    fprintf(stderr, "usage: %s [options..]\n", progname);
    fprintf(stderr, 
	"\t-a authsys\tdo/do not permit use of an authentication system\n");
    fprintf(stderr, "\t-d [1|2|3]\tset debugging level\n");
    fprintf(stderr, "\t-D datadir\tset data directory\n");
    fprintf(stderr, "\t-o option\tpass 'option' to each srbServer servers\n");
    fprintf(stderr, "\t-p port\t\tspecify port for srbMaster to listen on\n");
    fprintf(stderr, "\t-S\t\tsilent mode (disassociate from tty)\n");
/*
#if defined(PORTNAME_solaris) && defined(DB_Oracle)
    _exit (1);
#else
    exit(1);
#endif
    exit(1);
*/
}

/*
 * connCreate -- create a connection data structure and a new connection
 * with the client.
 *
 * Input - int serverFd - The srbMaster socket Fd.
 *	   int *newFdP - The place to put the new socket Fd.
 *
 * Output - Place new Fd in newFdP.
 * Returns the status of the connection.
 */
static int
connCreate(int serverFd, Port *port)
{
    int         status;
 
 
    if ((status = streamConnection(serverFd, port)) != STATUS_OK) {
        streamClose(port->sock);
    }
    port->nBytes = 0;
 
    return (status);
}

 
/*
 * serverStartup -- Start the srbServer process
 *
 * Input : StartupInfo *packet - client's startup packet. 
 *	   Port *port - The Port associated with the connection.
 *	   int *pidPtr - The location to put the srbServer pid.
 *	    
 * Output : Returns STATUS_ERROR if the fork/exec failed, STATUS_OK
 *      otherwise.
 *
 */
static int
serverStartup(Port *port)
{
    int         pid, i;
    char *cp;
    int status;
    time_t curTime;
#ifndef _WIN32
    int pipeFd0[2];
    int pipeFd1[2];
#endif
#ifdef _WIN32
    HANDLE hChildWrite, hParentRead;
    SECURITY_ATTRIBUTES sec;
    DWORD dwRead;
    DWORD errno;
#endif
 
    if (port->preSpawn >= 0)
	return 0;

#ifdef INSTRON
#if defined(PORTNAME_solaris)
    curTime = time(NULL);
    fprintf(stderr,
     "%24.24s serverStartup: Master Starting server, microSec=%lld\n",
       ctime(&curTime), gethrtime ());
#endif
#endif

    /* set some env */

    if (!getenv("srbData")) {
	char *envStr;
	
	envStr = malloc (strlen (DataDir) + 10);
        sprintf(envStr, "srbData=%s", DataDir);
        putenv(envStr);
    }


#ifdef _WIN32
    pid = 0;     /* doSpwan() would return real PID in WINDOWS. */

    sec.nLength = sizeof(SECURITY_ATTRIBUTES);
    sec.lpSecurityDescriptor = NULL;
    sec.bInheritHandle = TRUE;

    CreatePipe(&hParentRead,&hChildWrite, &sec,0);

    if (doExec((int)hChildWrite) < 0)
    {
        errno = GetLastError();
        elog(WARN, "child[%d]: serverStartup: spawnv failed (error %d)\n",pid, errno);
        return(STATUS_ERROR);
    }

#else
    /* create a pipe to talk to srbServer */

    if (pipe (pipeFd0) < 0) {
	fprintf(stderr, "serverStartup: error creating pipe, errno=%d\n",
	 errno);
	return (STATUS_ERROR);
    }

    if (pipe (pipeFd1) < 0) {
        fprintf(stderr, "serverStartup: error creating pipe, errno=%d\n",
         errno);
        return (STATUS_ERROR);
    }

    if ((pid = FORK()) == 0) {  /* child */
	close (pipeFd0[0]);
	close (pipeFd1[1]);
        if (doExec(pipeFd0[1], pipeFd1[0]))
            fprintf(stderr, "child[%d]: serverStartup: execv failed, errno=%d\n",
             pid, errno);
        _exit(1);
    }
 
    /* in parent */

    close (pipeFd0[1]);
    close (pipeFd1[0]);

    port->pipeFd1 = pipeFd1[1];

    if (pid < 0) {
        fprintf(stderr, "serverStartup: fork failed\n");
    	close (pipeFd0[0]);	
    	close (pipeFd1[1]);	
        return(STATUS_ERROR);
    }
#endif
 
#ifdef INSTRON
#if defined(PORTNAME_solaris)
    curTime = time(NULL);
    fprintf(stderr,
     "%24.24s serverStartup: Master Started server pid=%d, microSec=%lld\n",
       ctime(&curTime), pid, gethrtime ());
#endif
#endif

    port->preSpawn = 1;		/* spawning success */

#ifdef _WIN32
    status = ReadFile(hParentRead, &port->srbPort, sizeof(port->srbPort), &dwRead, NULL);
    if((status == -1) || (dwRead != sizeof(port->srbPort)))
    {
        elog(FATAL,"srbMaster: failed to get info from child process.\n");
        CloseHandle(hParentRead);
        return STATUS_ERROR;
    }
    CloseHandle(hParentRead);
    CloseHandle(hChildWrite);

#else
    signal(SIGALRM, pipeReadTimeout);
    if (setjmp(Jenv) == 0) {
      alarm (PIPE_READ_TIMEOUT);
      while ((status = read (pipeFd0[0], &port->srbPort, 
       sizeof(port->srbPort))) != sizeof(port->srbPort)) {
	if (errno == EINTR) {
	    continue;
	} else {
    	    fprintf(stderr, 
	     "serverStartup: pipe read failed, errno=%d pid=%d\n", errno, pid);
	    kill (pid, SIGTERM);
    	    close (pipeFd0[0]);
    	    close (pipeFd1[1]);
	    alarm (0);
	    return(STATUS_ERROR);
	}
      }
      alarm (0);
      close (pipeFd0[0]);
    } else {
	fprintf(stderr,
	 "serverStartup: pipe read timeout for pid=%d\n", pid);
	kill (pid, SIGTERM);
        close (pipeFd0[0]);
        close (pipeFd1[1]);
        return(STATUS_ERROR);
    }
#endif

    return(STATUS_OK);
}

/*
 * doExec -- Set up the argument list and perform an execv system call
 *
 * Input : StartupInfo *packet - client's startup packet.
 *         int portFd - The Port Fd associated with the connection.
 * Output :
 *      Shouldn't return at all.
 *      If execv() fails, return status.
 */
#ifdef _WIN32
static int doExec(int portFd0)
#else
static int
doExec(int portFd0, int portFd1)
#endif
{
#ifdef _WIN32
    char        execbuf[1024];
#else
    char        execbuf[MAXPATHLEN];
#endif
    char        portbuf0[ARGV_SIZE];
    char        portbuf1[ARGV_SIZE];
    char        debugbuf[ARGV_SIZE];
    char        *av[ARGV_SIZE];
#ifdef _WIN32
    char        consolebuf[100];
#endif

    int ac = 0;
    int i;
 
#ifdef _WIN32
    strcpy(execbuf,execFile);
#else
    if (strlen (execFile) >= MAXPATHLEN) {
      fprintf(stderr, "doExec: execFile %s too long\n", execFile);
      return (-1);
    }
    strcpy(execbuf, execFile);
#endif
    av[ac++] = execbuf;
 
    /*
     *  Pass the debugging level (decremented by one) to the srbServer. 
     *  Level one debugging stays in srbMaster.Levels two and higher
     *  are passed to the srbServer.
     */
 
    if (DebugLvl > 1) {
        (void) sprintf(debugbuf, "-d%d", DebugLvl - 1);
        av[ac++] = debugbuf;
    }
    else
        av[ac++] = "-Q";
 
    /* Tell the srbServer the descriptor of the  socket */
    (void) sprintf(portbuf0, "-p%d", portFd0);
    av[ac++] = portbuf0;

#ifndef _WIN32
    (void) sprintf(portbuf1, "-P%d", portFd1);
    av[ac++] = portbuf1;
#endif

#ifdef _WIN32
    if(SrbNtRunInConsoleMode())
    {
        strcpy(consolebuf,"console");
        av[ac++] = consolebuf;
    }
#endif


    av[ac] = (char *) NULL;
 
    if (DebugLvl > 1) {
#ifdef _WIN32
        elog(NOTICE,"child[%d]: execv(",getpid());
#else
        fprintf(stderr, "child[%d]: execv(",
                getpid());
#endif
        for (i = 0; i < ac; ++i)
#ifdef _WIN32
            elog(NOTICE,"%s,)\n ", av[i]);
#else
            fprintf(stderr, "%s, ", av[i]);
        fprintf(stderr, ")\n");
#endif
    }
 
#ifdef _WIN32
    return(_spawnv(_P_NOWAIT, av[0], av));
#else
    return(execv(av[0], av));
#endif
}

/*
 * exitSrbMaster -- Cleanup and exit SrbMaster.
 *
 * Input : status - The exit status.
 * Output - None.
 */
static void
exitSrbMaster(int status)
{
    if (serverSock != INVALID_SOCK)
#ifdef _WIN32
        closesocket(serverSock);
#else
        close(serverSock);
#endif
    /* exitSrb(status);  is replace by the following lines */
#ifdef SRB_MDAS
    close_db2_interaction(MDAS_CAT_COMMIT);
#endif
    exit (status);
}

/*
 * appendOpt -- Append argument in a string to an argv array
 *
 * Input : char **argv - Pointer to the *argv array.
 * 	   int *argcp - the index into the *argv array.
 *	   char *s - The string containing the additional arg.
 *	   
 * Outout : Updated *argv and argcp.
 */

static void
appendOpt(char **argv, int *argcp, char *s)
{
    int i = *argcp;
 
    while (s && *s) {
        while (isspace(*s))
            ++s;
        if (*s)
            argv[i++] = s;
        while (*s && !isspace(*s))
            ++s;
        if (isspace(*s))
            *s++ = '\0';
    }
    *argcp = i;
}

/* enquePort - queue myPort to the end of the queue headed by PortHead.
 */

int enquePort (Port *myPort)
{
    Port *tmpPort;

    myPort->next = NULL;
    if ((tmpPort = PortHead) == NULL) {
	PortHead = myPort;
 	return 0;
    }

    while (tmpPort->next != NULL) {
	if (tmpPort->next == myPort) {
	    fprintf (stderr, "enquePort; trying to link element in queue\n");
	    return (-1);
	}
        tmpPort = tmpPort->next;
    }
    tmpPort->next = myPort;

    return 0;
}

Port *
popPort ()
{
    Port *tmpPort;

    if ((tmpPort = PortHead) == NULL) {
	return (NULL);
    }

    PortHead = tmpPort->next;
    tmpPort->next = NULL;

    return (tmpPort);
}


#ifdef _WIN32
extern void srbNtCleanLogFile();   /* in elog.c */
static void srbWinMain(int aargc,char **aargv)
{
	int status;

	SrbNtCheckExecMode(aargc,aargv);
	
	if(ntSetSrbDataDir(aargv[0]) < 0)
	{
        fprintf(stderr,"SRB could not find Data directory. Check the followings and re-start the SRB server.\n");

        fprintf(stderr,"1. You need to use the full path name to start 'srbMaster.exe'. e.g. D:\\SRB Server\\bin\\srbMaster.exe console. Or\n");
        fprintf(stderr,"2. The directory '.\\..\\data' must exist and has at least two files, 'MdasEnv' and 'MdasAuth'.\n");

		exit(0);
	}

    ntSetSrbDefaultPort();

	srbNtCleanLogFile();

	if(!SrbNtRunInConsoleMode())  
	{	/* run as a NT service.  */
		if(SrbNtRunService(aargc,aargv) < 0)   /* In NT,  SrbNtRunService will call srbMasterMain() */
		{
			elog(FATAL,"SRB failed to initialize the SRB service.");
			exit(0);
		}
	}
	else
	{	/* else we run in console mode, this is just for debugging and testing. */
		status = srbMasterMain(aargc, aargv);
	}
}
#endif


