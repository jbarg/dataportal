/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* clStub.h - Header file for clStub.c
 */

/*-------------------------------------------------------------------------
 *
 *    Client function interface
 *
 *-------------------------------------------------------------------------
 */
#ifndef CL_STUB_H
#define CL_STUB_H

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif /* WIN32 */

#include <sys/stat.h>
#ifdef PORTNAME_solaris
#include <ctype.h>
#endif
#include "srb.h"
#include "stubdef.h"
#include "mdasC_db2_externs.h"
#include "vault.h"
#include "clExecExtern.h"
#include "clStubExtern.h"
#include "srbMiscExtern.h"
#include "clMiscExtern.h"
#include "packMsgExtern.h"
#include "packDefExtern.h"
#include "packStruct.h"
#include "clStubIntern.h"
 
#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

#define DATASET_INFO_COL_CNT	12	/* number of columns returned from
					 * a get_dataset_info call */

/* definition for the getSqlResultLen() call */
#define MAX_SQL_RESULT_LEN	(10*1024*1024)	/* 10 Mb */

/* The client call output struct */
typedef struct {
    int resultLen;
    char *retval;
} ClOut;

#ifdef c_plus_plus
extern "C" {
#endif

int
clCall (srbConn *conn, int funcid, srb_long_t *inpArgv, ClOut *clOut);

#ifdef c_plus_plus
}
#endif


#endif 	/* CL_STUB_H */
