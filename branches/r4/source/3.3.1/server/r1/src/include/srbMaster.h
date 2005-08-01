/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * srbMaster.h - Header file for srbMaster.c
 */
 
#ifndef SRB_MASTER_H
#define SRB_MASTER_H

#ifndef _WIN32
#include <pwd.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/param.h>
#include <netdb.h>     
#endif

#include <signal.h>
#include <string.h>
#include "c.h"
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h> 
#include <sys/stat.h> 

# if defined(PORTNAME_BSD44_derived) || defined(PORTNAME_bsdi) || defined(PORTNAME_osx)
# include <machine/limits.h>
# define MAXINT         INT_MAX
# else
#ifndef _WIN32
# include <values.h>
#endif
# endif /* !PORTNAME_BSD44_derived */

#ifndef _WIN32
#include <sys/wait.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#include <getopt.h>
#endif

 
#if defined(PORTNAME_aix)
#include <sys/select.h>
#endif /* PORTNAME_aix */
 
#include "srb.h"
#include "dllist.h"
#include "commExtern.h"
#include "elogExtern.h"
#include "srbMiscExtern.h"
#include "clAuthExtern.h"
#include "distribExtern.h"
#include "functionHandlerExtern.h"
#include "serverGlobal.h"
 
/*----------------------------------------------------------------
 * definition for the default authentication service
 *----------------------------------------------------------------
 */

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

#define SRB_LOGFILE	"srbLog"

#define STARTUP_RETRY_TIME	5	    /* sleep begore retrying */

/* pre-spawn parameter */
#define DEF_PRE_SPAWN_CNT	0	/* default number of pre-spawn server */
#define MAX_PRE_SPAWN_CNT	3	/* max number of prespawn */
#define PRE_SPAWN_CNT	"PRE_SPAWN_CNT" /* env variable for prespawn */

/* definition for the preSpawn flag */
#define NEED_SPAWN		-1
#define NEED_SPAWN_USING_1_PORT	-2

Port *PortHead = NULL;	/* The head of the Port que */

/*
 * Info for garbage collection.  Whenever a process dies, the srbMaster
 * cleans up after it.
 */
typedef struct bkend {
    int pid;            /* process id of backend */
} Backend;
 
/* list of active backends. */
 
static Dllist*  BackendList;
 
/* list of ports associated with still open connections */
static Dllist*  PortList;
 
static short    masterPortName = -1;
static char     *progname = (char *) NULL;
 
/*
 * Default Values
 */
static char     execFile[MAXPATHLEN] = "";
 
static int      serverSock = INVALID_SOCK;      /* stream socket server */
 
/*
 * Set by the -o option
 */
static char     extraOptions[ARGV_SIZE] = "";
 
/* Defines whether the default authentication (host based) should be used. */

#ifdef MDAS_AUTH
static int MdasAuth = 1;
#else
static int MdasAuth = 0;
#endif

extern char *DataDir;

extern int DebugLvl;
extern char *optarg;
extern int optind, opterr;
extern int UserClass;
extern char *HostBasedConFile;

/* declaration needed for initializing the host table */

extern int initSrbHost();


/*
 * function prototypes
 */
static int connStartup(Port *port);
static int connCreate(int serverFd, Port *port);
#if defined(PORTNAME_linux) || defined(PORTNAME_aix) || defined(PORTNAME_c90) || defined(PORTNAME_solaris) || defined(PORTNAME_dgux)
static void srbDie(int);
#else
static void srbDie(void);
#endif
int reaper();
static void cleanupProc(int pid, int exitstatus);
#ifdef _WIN32
static int doSpawn(StartupInfo *packet, int portFd, int *pid);
#else
static int doExec(int portFd0, int portFd1, int sock);
#endif
static void exitSrbMaster(int status);
static void usage();
static void checkDataDir();
static int serverLoop(void);
static int serverStartup(Port *port);
static void appendOpt(char **argv, int *argcp, char *s);
Port *popPort ();
int enquePort (Port *myPort);

#ifdef foo	/* use serverGlobal.h */
userInfo *ProxyUser = NULL;
userInfo *ClientUser = NULL;
userInfo *ThisUser = NULL;
struct mdasEnvData *ThisMdasEnvData = NULL;
struct hostElement *LocalHostHead = NULL;
struct hostElement *RemoteHostHead = NULL;
#ifdef FED_MCAT
struct hostElement *LocalMcat = NULL;
#endif

char            *DataDir = (char *) NULL;
int ElogMsgToClient = 0;

int SvrInitialized = 0; /* SvrInitialized = 1 ==> server has been initialized */

mdasC_sql_result_struct *SrbUserList = NULL;
int TicketUserFlag;     /* 1 = a ticket user; 0 = not a ticket user */

/* This keep track of the last query that where continuation_index >= 0 */
struct hostElement *LastQueryHost = NULL;
int LastContinuationIndex = -1;

struct mdasResInfo *LocalResInfo = NULL;
#endif	/* foo */

int
srbMasterMain(int argc, char *argv[]);

extern void prtZoneInfo ();
 
static int
serverStartupWithOnePort (Port *port);
#endif	/* SRB_MASTER_H */
