/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbServer.h - The header file for srbServer.c
 */

#ifndef SRB_SERVER_H
#define SRB_SERVER_H

#ifdef REFACTOR
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <ctype.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#ifndef _WIN32
#include <sys/time.h>
#include <sys/param.h>
#include <netdb.h>   
#include <errno.h>
#include <pwd.h>
#endif
 
#include "c.h"
#if defined(PORTNAME_aix)
#include <sys/select.h>
#endif /* PORTNAME_aix */
 
#include "srb.h"
#include "dllist.h"
#include "commExtern.h"
#include "elogExtern.h"
#include "srbMiscExtern.h"
#include "srbServerExtern.h"
#include "functionHandlerExtern.h"
#include "exfFunctExtern.h"
#include "dbLobjExtern.h"
#include "dbTableExtern.h"
#ifdef TAPE_DRIVE
#include "srbTapeMiscExtern.h"
#endif
#include "srbObjFunctExtern.h"
#include "distribExtern.h"
#include "authExtern.h"
#include "clAuthExtern.h"

#ifdef _WIN32
#include "SrbNtConst.h"
#include <process.h>
#include <io.h>
#include "startsock.h"
#include "getopt.h"
#include "srbServerMiscExtern.h"
#include "SrbNtUtil.h"
#endif
#endif /* REFACTOR */
#include "serverAll.h"
#include "serverGlobal.h"

#define SVR_CONN_TIMEOUT	30	/* connection timeout from client */
#define MCAT_CONN_TIMEOUT	7200	/* connection timeout to MCAT */
#define PIPE_READ_TIMEOUT	1800	/* first pipe read from srbMaster */

typedef struct clientConn {
  char  fn_connected;
  Port  fn_port;
  int   fn_fd;  /* the input/output fd */
  char  fn_done; /* set after the client closes its connection */
} clientConn;
 
static Dllist* clientConnList;
 
#ifdef _WIN32
char *DefaultPort=NULL;
#endif

#ifdef HOST_BASED_AUTH
static int HostBasedAuth = 1;
#else
static int HostBasedAuth = 0;
#endif

#ifdef _WIN32
char            srbPathname[MAXPATHLEN];
#else
char            srbPathname[256];
#endif
time_t          tim;
 
extern int UserClass;
extern short  DebugLvl;
extern char OutputFileName[];

extern int initMdasEnviron ();

void sPipSigHandler ();
#ifndef _WIN32
void
#if defined(PORTNAME_linux) || defined(PORTNAME_aix) || defined(PORTNAME_c90) || defined(PORTNAME_solaris) || defined(PORTNAME_dgux) || defined(PORTNAME_linux)
alarmSigHandler (int i);
#else
alarmSigHandler ();
#endif
#endif	/* _WIN32 */

#ifndef _WIN32
void
#if defined(PORTNAME_linux) || defined(PORTNAME_aix) || defined(PORTNAME_c90) || defined(PORTNAME_solaris) || defined(PORTNAME_dgux) || defined(PORTNAME_linux)
pipeReadAlarmSigHandler (int i);
#else
pipeReadAlarmSigHandler ();
#endif
#endif	/* _WIN32 */

int
chkAllowedUser (struct allowedUser *allowedUserHead, char *userName,
char *domainName);
int
recvAndProcCommand (svrComm_t *myComm, int    flagQ);
extern int SrbServerMode;

#endif	/* SRB_SERVER_H */
