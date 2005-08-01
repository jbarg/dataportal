/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * clMisc.h
 * Header file for clMisc.c
 */

#ifndef CL_MISC_H
#define CL_MISC_H

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif /* WIN32 */

#include <errno.h>
#ifdef PORTNAME_solaris
#include <ctype.h>
#include <strings.h>
#endif
#include "srbErrno.h"
#include "srb.h"
#include "clAuthExtern.h"
#include "clMiscExtern.h"

#define CHAR_CUTOFF_LEN         1024

/* definition for COMM_PORTS */

#define DEF_COMM_PORT_NUM_COUNT 200
#define COMM_PORT_NUM_START_KETWORD	"commPortNumStart"
#define COMM_PORT_NUM_COUNT_KETWORD     "commPortNumCount"

int RandomSeed = 0;	/* set whether srandom() has been called yet */

extern int UserClass;

extern int aid_read(int fd, char *buffer, int length);
extern int aid_write(int fd, char *buffer, int length);
#endif	/* CL_MISC_H */
