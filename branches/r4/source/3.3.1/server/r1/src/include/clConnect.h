/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* clConnect.h
 * Header file for clConnect.c
 */

#ifndef CL_CONNECT_H
#define CL_CONNECT_H

#include <stdlib.h>
#include <sys/types.h>

#ifdef _WIN32
  #undef _UNICODE
  #define MAX_USERNAME_LENGTH 128
  #include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif /* WIN32 */

#include <fcntl.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>

#ifndef _WIN32
#include <netdb.h>
#endif

#include <errno.h>
#include <signal.h>
#include "srb.h"
#include "dllist.h"
#include "clAuthExtern.h"
#include "clConnectExtern.h"

#define NOTIFYLIST_INITIAL_SIZE 10
#define NOTIFYLIST_GROWBY 10

/* user class definition */

int UserClass = CLASS_CLI;

extern struct serverAuthInfo *DefServerAuthInfo;
extern struct serverAuthInfo *ServerAuthInfo;

int SrbShellPort = -1;
static int SrbShellLockFd = -1;

static int connectSvr(srbConn *conn);
void freeSrbConn(srbConn *conn);
static void closeSrbConn(srbConn *conn);
int
clFileLock (char *filePath, int lockCmd, int lockType, int lockFd);

/* extern struct mdasEnvData *MdasEnvData; */

#endif	/* CL_CONNECT_H */
