/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* unixFileDvr.h - Definitions and prototypes for unixFileDvr.c
 */
 
#ifndef UNIX_FILE_DVR_H
#define UNIX_FILE_DVR_H

#include <stdio.h>
#include <sys/file.h>
#include <sys/param.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#if defined(PORTNAME_osx)
#include <sys/types.h>
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <fcntl.h>  
#include <sys/file.h> 
#include <dirent.h> 
#include <unistd.h>     /* for SEEK_ */

#include"c.h"
#include"srb.h"
#include "elogExtern.h"
#include "clMiscExtern.h"
#include "unixFileDvrExtern.h"
 
 
static int _unixOpen(char *fileName, int fileFlags, int fileMode);
extern int DebugLvl;

#endif	/* UNIX_FILE_DVR_H */
