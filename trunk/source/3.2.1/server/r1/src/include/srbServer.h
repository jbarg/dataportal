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
#endif

/* The state of server initialization - used by the exitSrbServer call */
#define INIT_IN_PROGRESS	0
#define INIT_DONE		1

#define SVR_CONN_TIMEOUT	30	/* connection timeout from client */

typedef struct clientConn {
  char  fn_connected;
  Port  fn_port;
  int   fn_fd;  /* the input/output fd */
  char  fn_done; /* set after the client closes its connection */
} clientConn;
 
static Dllist* clientConnList;
 
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
 
/* Ticket user flag */

int TicketUserFlag;	/* 1 = a ticket user; 0 = not a ticket user */
int DbOpened = 0;

extern short  DebugLvl;
extern char OutputFileName[];
extern char *UserName;
extern int     Quiet;
userInfo *ClientUser = NULL;
userInfo *ProxyUser = NULL;
userInfo *ThisUser = NULL;
struct mdasEnvData *ThisMdasEnvData = NULL;
char            *DataDir = (char *) NULL;
struct hostElement *LocalHostHead = NULL;
struct hostElement *RemoteHostHead = NULL;
#ifdef FED_MCAT
struct hostElement *LocalMcat = NULL;
#endif
mdasC_sql_result_struct *SrbUserList = NULL;
int ElogMsgToClient = 1;
#ifdef TAPE_DRIVE
tapeLibConfig_t *TapeLibConfigHead = NULL;
#endif

int SvrInitialized = 0;	/* SvrInitialized = 1 ==> server has been initialized */ 

extern int UserClass;

extern int initMdasEnviron ();

extern int SrbServerMode;

#endif	/* SRB_SERVER_H */
