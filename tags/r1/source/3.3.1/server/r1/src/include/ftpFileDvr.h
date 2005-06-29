/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* ftpFileDvr.h - Definitions and prototypes for ftpFileDvr.c
 */
 
#ifndef FTP_FILE_DVR_H
#define FTP_FILE_DVR_H

#include <sys/types.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#include "c.h"
#include "srb.h"
#include "srbDirent.h"
#include "elogExtern.h"
#include "ftpFileDvrExtern.h"
 
#define CMDLEN 500
#define RCV_BUF_SIZE 4000


/*
  Global variables:
 */
char ftpRcvCmdBuf[RCV_BUF_SIZE]; /* buffer for receiving ctrl/stat from FTPD */
int ftpCmdSock=0;  /* the command socket (connected to FTP server)           */
struct sockaddr_in ftpCntlAddr;    /* control socket structure               */
static int ftpDebugFlag=0;

 
static int _ftpOpen(char *fileName, int fileFlags, int fileMode);

#endif	/* FTP_FILE_DVR_H */
