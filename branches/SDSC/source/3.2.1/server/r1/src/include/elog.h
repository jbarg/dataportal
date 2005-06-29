/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*-------------------------------------------------------------------------
 *
 * elog.h--
 *    error logging definitions.
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef	ELOG_H
#define ELOG_H

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#ifndef O_RDONLY
#include <sys/file.h>
#endif /* O_RDONLY */
#include <sys/types.h>
#include <stdarg.h>
#include <errno.h>
 
#include "srb.h"
#include "elogExtern.h"
#include "srbMiscExtern.h"
#include <time.h>
 
static int      Debugfile = -1;
static int      Err_file = -1;
static int      ElogDebugIndentLevel = 0;
#ifdef PORTNAME_solaris
  static hrtime_t highResTime;
#endif

extern char     OutputFileName[];
extern userInfo *ProxyUser;
extern userInfo *ThisUser;
extern int ElogMsgToClient;

#define ELOG_MAXLEN 4096


/* elog's to be timestamped */
#define ELOG_TIMESTAMPS

#endif	/* ELOG_H */
