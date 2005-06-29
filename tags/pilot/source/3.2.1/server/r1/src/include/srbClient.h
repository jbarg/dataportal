/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbClient.h - The include file required for the srb client library -
 * libSrbClient.a.
 */

#ifndef SRB_CLIENT_H
#define SRB_CLIENT_H

#include <stdio.h>
#include <errno.h>
#ifndef _WIN32
#include <sys/file.h>
#endif
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris) || defined(PORTNAME_t3e)
#include <fcntl.h>
#endif
#ifndef _WIN32
#include <unistd.h>
#endif

/* #include "srb_error.h" */
#include "srb.h"
#include "vault.h"
#include "clConnectExtern.h"
#include "mdasC_db2_externs.h"
#include "functionHandlerExtern.h"
/* #include "functionHandler.h" */
#include "clStubExtern.h"
/* #include "mdasGlobals.h" */
#include "mdasGlobalsExtern.h"
#include "srbStat.h"
#include "packMsgExtern.h"
#include "packDefExtern.h"
#include "packStruct.h"
#include "clSfoExtern.h"
#include "clAuthExtern.h"

#ifdef _WIN32
#include "unix2nt.h"
#endif

#endif	/* SRB_CLIENT_H */
