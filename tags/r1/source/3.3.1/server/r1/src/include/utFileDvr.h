/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* hpssFileDvr.h */

#ifndef UT_FILE_DVR_H
#define UT_FILE_DVR_H
 
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
#include <dirent.h>
#include <fcntl.h>      /* most O_ */
#ifndef O_RDONLY
#include <sys/file.h>   /* The rest of O_ */
#endif /* O_RDONLY */
#include <unistd.h>     /* for SEEK_ */
 
#include "c.h"
#include "srb.h"
 
#ifdef UTREE
 
#include "utree.h"
#include "elogExtern.h"
#include "utFileDvrExtern.h"

static int _utOpen(char *fileName, int fileFlags, int fileMode);

void snd_illus_clt (char *clt_msg);

#endif	/* UTREE */
#endif  /* UT_FILE_DVR_H */
