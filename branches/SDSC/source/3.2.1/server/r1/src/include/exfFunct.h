/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * exfFunct.h - Header file for exfFunct.c
 */
 
#ifndef EXF_FUNCT_H
#define EXF_FUNCT_H

#if defined(PORTNAME_osx)
#include <sys/types.h>
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <stdio.h>
#ifndef _WIN32
#include <sys/file.h>
#endif
#include <sys/stat.h>
#include "c.h"
#include "srb.h"
#include "elogExtern.h"
#include "vault.h"
#include "srbMiscExtern.h"
#include "srbStat.h"
#include "exfFunctExtern.h"
#include "srbDirent.h"
#include "distribExtern.h"
#include "packStruct.h"
#include "clStubExtern.h"
#include "clStubIntern.h"
#include "clMiscExtern.h"
#include "exfSwExtern.h"

#include <fcntl.h>      /* most O_ */
#ifndef O_RDONLY
#include <sys/file.h>   /* The rest of O_ */
#endif /* O_RDONLY */
 
#ifndef _WIN32
#include <unistd.h>     /* for SEEK_ */
#endif

/* exf descriptor */
 
#define MAX_EXF_FDS 256
 
extern char *srbUserDomain;
extern userInfo *ProxyUser, *ClientUser, *ThisUser;
extern struct hostElement *LocalHostHead;
extern struct hostElement *RemoteHostHead;


struct exfDesc {
    char        *hostAddr;     /* The host address where the ext file is */
    char	*portNum;	/* The port number */
    struct hostElement	*hostTabPtr;	/* Pointer to the host table */
    int		remoteFlag;	/* host location, 0 - local, 1 - remote */
    char        *filename;      /* the file pathname */
    int         storSysType;       /* access method */
    int         fd;             /* the file descriptor */
    int 	writeFlag;      /* indicate whether the obj has been written
                                 * into. 0 = no, 1 = yes.
                                 */
    srbConn 	*conn;
    void	*storSysDepend;	/* Storage system dependent stuff */
};
 
struct exfDesc exfDesc[MAX_EXF_FDS];
static int newExfFd(int storSysType, char *hostAddr, char *portNum, 
struct hostElement  *hostTabPtr, char *filename, int fd, int remoteFlag,
srbConn *conn);
static void deleteExfFd(int i);

int mkVaultDir (int storSysType, char *filePath, int pathType);
static int fillSrbStatBuf (struct srbStat *retStat, struct stat *statbuf);
extern int TicketUserFlag;
extern struct vaultQueElement *vaultQueHead;
extern int DebugLvl;
extern int nExfswEntries;

#endif	/* EXF_FUNCT_H */
