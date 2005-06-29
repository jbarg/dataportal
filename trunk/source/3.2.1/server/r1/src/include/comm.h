/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*-------------------------------------------------------------------------
 *
 * comm.h--
 *    Definitions for comm.c
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef COMM_H
#define COMM_H

#include <signal.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <unistd.h>             /* for ttyname() */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#endif

#include <errno.h>
#include <fcntl.h>
 
#ifdef PORTNAME_linux
#ifndef SOMAXCONN
#define SOMAXCONN 5             /* from Linux listen(2) man page */
#endif /* SOMAXCONN */
#endif /* PORTNAME_linux */
 
#include "c.h"
#include "srb.h"
#include "vault.h"
#include "elogExtern.h"
#include "distribExtern.h"
#ifdef TAPE_DRIVE
#include "srbTapeMiscExtern.h"
#endif
#include "srbObjFunctExtern.h"
#include "commExtern.h"
#include "clAuthExtern.h"

 
#define CHAR_CUTOFF_LEN         1024
 
int MaxThread = DEF_MAX_THREAD;
int SizePerThread = DEF_SIZE_PER_THREAD;

extern struct hostElement *LocalHostHead;
extern struct hostElement *RemoteHostHead;
#ifdef FED_MCAT
extern struct hostElement *LocalMcat;
#endif

 
/* ----------------
 *      declarations
 * ----------------
 */
FILE *Pfdebug;    /* debugging */
 
int commTracep = 0;               /* 1 to print out debugging messages */
FILE *debug_port = (FILE *) NULL;

/* This keep track of the last query that where continuation_index >= 0 */
struct hostElement *LastQueryHost = NULL;
int LastContinuationIndex = -1;

struct mdasResInfo *LocalResInfo = NULL;

/* struct serverAuthInfo *McatServerAuthInfo = NULL; */

extern int DebugLvl;
extern int UserClass;
/* extern void cPipSigHandler (); done in clConnectExtern.h */
extern userInfo *ProxyUser;
extern userInfo *ClientUser;
extern userInfo *ThisUser;
extern struct mdasEnvData *ThisMdasEnvData;
extern struct serverAuthInfo *DefServerAuthInfo;
extern struct serverAuthInfo *ServerAuthInfo;
extern int SrbServerClientSock;

/* extern struct mdasEnvData *MdasEnvData; */
int
fillMcatHostInfo (char *mcatName, char *mcatHost, char *mcatAuthScheme,
char *mcatDn, char *myPortNum, int cnt);

#endif	/* COMM_H */
