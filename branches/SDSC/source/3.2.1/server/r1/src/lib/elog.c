/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * elog.c-- Routines for error logger
 *
 */

#include "elog.h"
#ifdef _WIN32
#include "Unix2Nt.h"
#include "SrbNtUtil.h"
static void srbNtWrite_eLog(char *msg,int len);
#endif

/*
 * elog - Error logging function.
 *
 * Input : int lev - The log level
 *	   char *fmt - The format string
 *
 * Output - None
 */

void
elog(int lev, char *fmt, ... )
{
    va_list ap;
    char		buf[ELOG_MAXLEN], line[ELOG_MAXLEN];
    register char	*bp, *cp;
    extern	int	errno;
#ifndef EXF_STANDALONE
    /*    extern	FILE	*Pfout; */
    extern int commFd;
#endif /* !EXF_STANDALONE */
    time_t		tim, time();
    int		len;
    int		i = 0;
    int authSchemeInx;
    
    va_start(ap, fmt);
    
    /* if (lev == DEBUG && Debugfile < 0) {
	return;
    } */
    switch (lev) {
    case DEMOM:
        i = ElogDebugIndentLevel-1;
        if (i < 0) i = 0;
        if (i > 30) i = i%30;
        cp = "";
        break;
    case NOIND:
	i = ElogDebugIndentLevel-1;
	if (i < 0) i = 0;
	if (i > 30) i = i%30;
	cp = "DEBUG:";
	break;
    case DEBUG:
	i = ElogDebugIndentLevel;
	if (i < 0) i = 0;
	if (i > 30) i = i%30;
	cp = "DEBUG:";
	break;
    case NOTICE:
	cp = "NOTICE:";
	break;
    case WARN:
	cp = "WARN:";
	break;
    default:
	sprintf(line, "FATAL %d:", lev);
	cp = line;
    }
#ifdef ELOG_TIMESTAMPS
    time(&tim);
    strcat(strcpy(buf, cp), ctime(&tim)+4);
    bp = buf+strlen(buf)-6;
    *bp++ = ':';
    *bp++ = ' ';
#else
    strcpy(buf,cp);
    bp = buf+strlen(buf);
#endif
    while (i-- >0) *bp++ = ' ';
    for (cp = fmt; *cp; cp++)
	*bp++ = *cp;
    *bp = '\0';
    vsprintf(line, buf, ap);
    va_end(ap);
    len = strlen(strcat(line, "\n"));

#ifdef _WIN32
    srbNtWrite_eLog(line,len);
#else
    if (Debugfile > -1)
	write(Debugfile, line, len);
    else
	fprintf (stderr, "%s", line);
#endif

    if (lev == DEBUG || lev == NOIND)
	return;
    
    /*
     *  If there's an error log file other than the connection to the
     *  client program, write to it first. 
     */
    
    if (Err_file > -1 && Debugfile != Err_file) {
	if (write(Err_file, line, len) < 0) {
	    write(open("/dev/console", O_WRONLY, 0666), line, len);
	    fflush(stdout);
	    fflush(stderr);
	    exit (lev);
	}
#ifdef _WIN32
    FlushFileBuffers(Err_file);
#else
	fsync(Err_file);
#endif
    }
    
#ifdef SRB_SVR_ENV
#ifdef foo
    /* if (getenv ("SHOW_MCAT_QUERY") != NULL) { MW 22/6/04 */
    if (getenv ("SHOW_MCAT_QUERY") == NULL) {
        authSchemeInx = svrGetAuthScheme (CLIENT_MODE);

        /* Send IPC message to the front-end program */
        if (ElogMsgToClient > 0 && ProxyUser != NULL && 
            ProxyUser->myComm != NULL && 
            ProxyUser->myComm->commFd != 0 && lev > DEBUG) {
	      if (lev == NOTICE) 
	          commPutnchar(ProxyUser->myComm, "N", 1, authSchemeInx);
	      else
	          commPutnchar(ProxyUser->myComm, "E", 1, authSchemeInx);
	      commPutstr(ProxyUser->myComm, line, authSchemeInx);
	      commFlush(ProxyUser->myComm, authSchemeInx);
        }
    }
#endif
#endif
    
/*  no exit
    if (lev >= FATAL) {
	fflush(stdout);
	fflush(stderr);
	exitSrb(0);
    }
*/
}

void
elogTiming(char *msg1, char *msg2 )
{

#if defined(PORTNAME_solaris)
  highResTime = gethrtime();
  elog(DEBUG, "TIMING: %lld ns=> %s %s\n",highResTime, msg1,msg2);
#else
  elog(DEBUG, "TIMING: 0  ns=> %s %s\n", msg1,msg2);
#endif /* PORTNAME_solaris */
}

#ifdef SRB_SVR_ENV

/* debugFileOpen - Open the DebugFile and set Err_file to the opened fd.
 *
 * Input - None
 *
 * Output - The opened fd.
 */

int
debugFileOpen()
{
    int fd, istty;
    
    Err_file = Debugfile = -1;
    ElogDebugIndentLevel = 0;
    
    if (OutputFileName[0]) {
	OutputFileName[MAXEXFPATH-1] = '\0';
#ifdef _WIN32
    if ((fd = SrbNtFileOpen(OutputFileName, O_CREAT|O_APPEND|O_WRONLY, 1)) < 0)
#else
	if ((fd = open(OutputFileName, O_CREAT|O_APPEND|O_WRONLY, 0666)) < 0)
#endif
	    elog(FATAL, "debugFileOpen: open of %s: %m", OutputFileName);
	istty = isatty(fd);
	(void) close(fd);
	/* If the file is a tty ,send stdout there 
	 */
	if (istty && !freopen(OutputFileName, "a", stdout))
	    elog(FATAL, "debugFileOpen: %s reopen as stdout: %m",
		 OutputFileName);
	if (!freopen(OutputFileName, "a", stderr))
	    elog(FATAL, "debugFileOpen: %s reopen as stderr: %m",
		 OutputFileName);
	Err_file = Debugfile = fileno(stderr);
	return(Debugfile);
    }
    /* If no filename was specified, send debugging output to stderr.
     */
    fd = fileno(stderr);
#ifndef _WIN32
    if (fcntl(fd, F_GETFD, 0) < 0) {
	sprintf(OutputFileName, "%s/pg.errors.%d",
		getSrbDataDir(), getpid());
	fd = open(OutputFileName, O_CREAT|O_APPEND|O_WRONLY, 0666);
    }
#endif
    if (fd < 0)
	elog(FATAL, "debugFileOpen: could not open debugging file");
    
    Err_file = Debugfile = fd;
    return(Debugfile);
}

#endif	/* SRB_SVR_ENV */

#ifdef _WIN32
static void srbNtWrite_eLog(char *msg,int len)
{
    char log_file[1024];
    int fd;
    int t;

    if(SrbNtRunInConsoleMode())
    {
        t = strlen(msg);
        if(msg[t-1] == '\n')
            fprintf(stderr,"%s",msg);
        else
            fprintf(stderr,"%s\n",msg);
        return;
    }

    sprintf(log_file,"%s/%s",getSrbDataDir(),SRB_NT_LOG_FILE);
    fd = SrbNtFileOpen(log_file,O_CREAT|O_APPEND|O_WRONLY,1);
    if(fd < 0)
        return;

    _write(fd,msg,len);
    _close(fd);
}

void srbNtCleanLogFile()
{
    char log_file[1024];
    int fd;

    sprintf(log_file,"%s/srbLog",getSrbDataDir());

    fd = SrbNtFileOpen(log_file,O_CREAT|_O_TRUNC,1);
    if(fd < 0)
        return;

    _close(fd);
}
#endif
