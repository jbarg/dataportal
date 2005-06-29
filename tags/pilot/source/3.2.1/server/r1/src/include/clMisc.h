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
#include "srbErrno.h"
#include "srb.h"
#include "clAuthExtern.h"
#include "clMiscExtern.h"

#define CHAR_CUTOFF_LEN         1024

int RandomSeed = 0;	/* set whether srandom() has been called yet */

#endif	/* CL_MISC_H */
