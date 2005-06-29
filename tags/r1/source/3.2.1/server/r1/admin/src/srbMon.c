
/*-------------------------------------------------------------------------
 *
 * srbMon.c--
 *   Program for monitoring the SRB Master.
 *
 *-------------------------------------------------------------------------
 */

#include "srbMon.h"

/* This is a temp fix for file desc leak */
#define MIN_FD		10
#define MAX_FD		50
#define INC_FD		10

int
main(int argc, char **argv)
{
    int i;
    int status;
    int c;
    int runMode = INF_LOOP;
    int baseFd = MIN_FD;

    signal(SIGALRM, alarmHandler);

    while ((c=getopt(argc, argv,"s")) != EOF) {
        switch (c) {
	    case 's':
		runMode = SINGLE_PASS;
		break;

            default:
                usage (argv[0]);
                exit (1);
        }
    }


    if ((LogFd = logFileOpen (LOG_FILENAME)) < 0)
	exit (1);

    if (runMode == INF_LOOP)
    	daemonize ();

    if ((status = initHost ()) < 0) {	/* Initialize the host table */
	monLog ("initHost failed. status = %d", status);
    	close (LogFd);
	exit (1);
    }

    /* Try to connect to and test each SRB */

    while (1)
    {
    	testSRB (mcatHostHead);
    	testSRB (hostHead);

	if (runMode == SINGLE_PASS)
	    break;

	sleep (SLEEP_TIME);

	for (i = baseFd; i < baseFd + INC_FD; i++) {
	    close (i);
	}
	baseFd += INC_FD;
	if (baseFd >= MAX_FD)
	    baseFd = MIN_FD;
    }

}

void
testSRB (struct srbHostElm *tmpHostElm)
{
    srbConn *conn;
    int status;
    mdasC_sql_result_struct myresult;


    while (tmpHostElm != NULL) {
	monLog ("Testing SRB on host: %s", tmpHostElm->hostName);
	alarm (ALARM_TIME);
	AlarmHostElm = tmpHostElm;
        conn = clConnect (tmpHostElm->hostName, tmpHostElm->portNum, NULL);

        if ((status = clStatus(conn)) != CLI_CONNECTION_OK) {
            monLog ("%s connection failed. status = %d. %s", 
	      tmpHostElm->hostName, status, clErrorMessage(conn));
	    tmpHostElm->failCnt++;
	    if (tmpHostElm->failCnt >= REBOOT_LIM) {
	    	rebootSRB (tmpHostElm, 0);
	        clFinish(conn);
	    	tmpHostElm->failCnt = 0;
	    }
            tmpHostElm = tmpHostElm->next;
	    clFinish(conn);
            continue;
	}
	/* Do a simple query */

	status = srbGetPrivUsers (conn, MDAS_CATALOG, &myresult, 
	  NUM_ROWS_WANTED);
	if (status < 0) {
            monLog ("%s srbGetPrivUsers failed. status = %d. %s",
              tmpHostElm->hostName, status, clErrorMessage(conn));
            tmpHostElm->failCnt++;
            if (tmpHostElm->failCnt >= REBOOT_LIM) {
                rebootSRB (tmpHostElm, 0);
                tmpHostElm->failCnt = 0;
            }
        } else {
	    tmpHostElm->failCnt = 0;
	}

	clFinish(conn);

	alarm (0);
	AlarmHostElm = NULL;

	clearSqlResult (&myresult);

        tmpHostElm = tmpHostElm->next;
    }
}

int
initHost ()
{
    FILE *file;
    char buf[LINE_SZ];
    char *hostName, *portNum, *srbLogin, *runPath;
    char *conFile;
    int i = 0;
    struct srbHostElm *tmpHostElm;

    /* put together the full pathname for the config file */

    conFile = (char *) malloc((strlen(getDataDir()) +
        strlen(SRB_HOST_FNAME)+2)*sizeof(char));

    strcpy(conFile, getDataDir());
    strcat(conFile, "/");
    strcat(conFile, SRB_HOST_FNAME);

    file = fopen(conFile, "r");

    if (file == NULL) {
        monLog ("initHost: Unable to open file %s, errno = %d", 
	  conFile, errno);
        free (conFile);
        return (-1 * errno);
    }

    free (conFile);

    /* read the file */

    while ((i = getNextStr(file, buf, sizeof(buf))) != EOF) {

        /* Read until end of line then next line */

        if (buf[0] == '#')
        {
            while (i == 0)
                i = getNextStr(file, buf, sizeof(buf));
            continue;
        }

	/* Read until the end of line */
        hostName = (char *) malloc(strlen (buf) + 1);
        strcpy (hostName, buf);
        portNum = parseAddr (hostName);

        /* Read the rest of the line */

	/* the srbLogin */

        i = getNextStr(file, buf, sizeof(buf));
	if (i == '\n') {
	    monLog ("initHost: Input too short for host %s", hostName);
	    return (ERR_HOST_INP_TOO_SH);
	}
        srbLogin = (char *) malloc(strlen (buf) + 1);
        strcpy (srbLogin, buf);

	/* The runPath */

        i = getNextStr(file, buf, sizeof(buf));
        runPath = (char *) malloc(strlen (buf) + 1);
        strcpy (runPath, buf);

        if (i == '\n') {	/* Reached the end of line. */
	    /* Must no be a MDAS_ENABLED host */
	    enqHost (&hostHead, hostName, portNum, srbLogin, runPath);
	    continue;
	} else if (buf[0] == '#') {
            while (i == 0)
                i = getNextStr(file, buf, sizeof(buf));
	    enqHost (&hostHead, hostName, portNum, srbLogin, runPath);
            continue;
        }

	/* The next entry must be MDAS_ENABLED or '#' */

        i = getNextStr(file, buf, sizeof(buf));
	if (strcmp (buf, MDAS_ENABLED_FL) != 0) {
            monLog ("initHost: The forth entry of host %s must be %s.", 
	      hostName, buf);
            return (ERR_HOST_INP_MDAS_EN);
        }

	enqHost (&mcatHostHead, hostName, portNum, srbLogin, runPath);

	/* skip the rest */
        while (i != '\n') 
	    i = getNextStr(file, buf, sizeof(buf));
	continue;
    }
    fclose (file);
    
    return (0);
}

/*
 * enqHost - Enque a hostName entry to the end of the host queue pointed to
 *             by queHead.
 *
 * Input :     char *hostName - The host to be queued. The format of
 *             the hostName string is hostName[:portNumber].
 *             struct srbHostElm *queHead - The queue head of the queue.
 *
 * Output : Returns the point to the (struct srbHostElm) of the host.
 */

int
enqHost (struct srbHostElm **queHead, 
char *hostName, char *portNum, char *srbLogin, char *runPath)
{
    struct srbHostElm *tmpHElement, *tmpHElement1;

    tmpHElement = (struct srbHostElm *) malloc
        (sizeof (struct srbHostElm));
    if (tmpHElement == NULL) {
        monLog ("enqHost: Unable of malloc tmpHElement");
	return (ERR_MALLOC);
    }

    tmpHElement->hostName = hostName;
    tmpHElement->portNum = portNum;
    tmpHElement->srbLogin = srbLogin;
    tmpHElement->runPath = runPath;
    tmpHElement->failCnt = 0;
    tmpHElement->next = NULL;

    if (*queHead == NULL) {
        *queHead = tmpHElement;
        return 0;
    }

    /* append tmpHElement to the end */

    tmpHElement1 = *queHead;
    while (tmpHElement1->next != NULL) {
        tmpHElement1 = tmpHElement1->next;
    }
    tmpHElement1->next = tmpHElement;
    return 0;
}

/* getDataDir - get the "data" directory (where host and log files are
 * kept). It will use the path defined in the srbData env variable first.
 * If it is not defined, use the constant string SRB_DATA_DIR which is
 * defined in the Makefile.
 *
 * Input - None
 *
 * Output - The "data" directory path
 */

char *
getDataDir()
{
    char *p;

    if ((p = (char *) getenv("monDataDir")) != (char *) NULL) {
        return (p);
    }
    return (MON_DATA_DIR);       /* MON_DATA_DIR defined in the Makefile */
}

/*
 * monLog - Error logging function.
 *
 * Input :  char *fmt - The format string
 *
 * Output - None
 */

void
monLog( char *fmt, ... )
{
    va_list ap;
    char                buf[LINE_SZ], line[LINE_SZ];
    register char       *bp, *cp;
    extern      int     errno, sys_nerr;
    time_t              tim, time();
    int         len;

    va_start(ap, fmt);
    time(&tim);
    strcpy(buf, ctime(&tim)+4);
    bp = buf+strlen(buf)-6;
    *bp++ = ':';
    *bp++ = ' ';
    for (cp = fmt; *cp; cp++)
        *bp++ = *cp;
    *bp = '\0';
    vsprintf(line, buf, ap);
    va_end(ap);
    len = strlen(strcat(line, "\n"));

    write(LogFd, line, len);
}

/* logFileOpen - Open the DebugFile and set Err_file to the opened fd.
 *
 * Input - None
 *
 * Output - The opened fd.
 */

int
logFileOpen(char *logFileName)
{
    int logFd;
    char *logFile;

    /* put together the full pathname for the config file */

    logFile = (char *) malloc((strlen(getDataDir()) +
        strlen(LOG_FILENAME)+2)*sizeof(char));

    strcpy(logFile, getDataDir());
    strcat(logFile, "/");
    strcat(logFile, logFileName);

    logFd = open(logFile, O_CREAT|O_TRUNC|O_WRONLY, 0666);
    if (logFd < 0) {
	fprintf (stderr, "logFileOpen: Unable to open %s. errno = %d\n",
	  logFile, errno);
	free (logFile);
        return (-1 * errno);
    } else {
	free (logFile);
	return logFd;
    }
}

void
rebootSRB (struct srbHostElm *tmpHostElm, int killOnly)
{
    char command[LINE_SZ];
    char *logFile;

    /* put together the full pathname for the config file */

    logFile = (char *) malloc((strlen(getDataDir()) +
        strlen(LOG_FILENAME)+2)*sizeof(char));

    strcpy(logFile, getDataDir());
    strcat(logFile, "/");
    strcat(logFile, LOG_FILENAME);

    /* stop SRB master and server */

    sprintf (command, "rsh %s -n -l %s 'cd %s;%s' >> %s", 
      tmpHostElm->hostName,
      tmpHostElm->srbLogin, 
      tmpHostElm->runPath,
      KILL_SCRIPT,
      logFile);


    monLog ("rebootSRB for %s", tmpHostElm->hostName);

    system (command);

    /* start SRB master */

    if (!killOnly) {
        sprintf (command, "rsh %s -n -l %s 'cd %s;%s' >> %s",
          tmpHostElm->hostName,
          tmpHostElm->srbLogin,
          tmpHostElm->runPath,
          RUN_SCRIPT,
          logFile);

        system (command);
    }

    free (logFile);
}

void
daemonize()
{
    if (fork())
	exit (0);

    if (setsid() < 0) {
        fprintf(stderr, "daemonize");
        perror("cannot create a new session.");
        exit(1);
    }

    (void) dup2 (LogFd, 0);
    (void) dup2 (LogFd, 1);
    (void) dup2 (LogFd, 2);
}
#ifdef foo
/* parseAddr - Parse the address with format host:port.
 * Input : char *hostAddr
 *
 * Output : Return the portNum if exists. Otherwise, returns DefaultPort.
 */

char *
parseAddr (char *hostAddr)
{
    char *portNum = NULL;
    char *cptr;

    cptr =  hostAddr;

    if (hostAddr == NULL)
        return NULL;

    while (*cptr != '\0') {
        if (*cptr == ':') {
            *cptr = '\0';
            cptr ++;
            if (*cptr != '\0') {
                portNum = cptr;
            }
            break;
        }
        cptr ++;
    }

    /* Some sanity check of portNum */

    if (portNum != NULL) {
        while (*cptr != '\0') {
            if (*cptr == ':') {
                *cptr = '\0';
                break;
            }
            if (!isdigit (*cptr)) {     /* Not a digit */
                portNum = NULL;
                break;
            }
            cptr ++;
        }
    }

    return (portNum);
}
#endif
int usage (char *prog)
{
    fprintf(stderr, "Usage: %s [-s] \n",prog);
}

static void
#if defined(PORTNAME_linux) || defined(PORTNAME_aix) || defined(PORTNAME_c90) || defined(PORTNAME_solaris)
alarmHandler (int i)
#else
alarmHandler ()
#endif
{
    rebootSRB (AlarmHostElm, 1);
    signal(SIGALRM, alarmHandler);
}
