/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* 
 * clExec.h
 * Header file for clExec.c
 */

#ifndef CL_EXEC_H
#define CL_EXEC_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "srb.h"
#include <signal.h>
#include "clExecExtern.h"
#include "clMiscExtern.h"
#include "clAuthExtern.h"
#include "dllist.h"

static srbResult* makeEmptysrbResult(srbConn *conn, int status);
extern int SrbServerClientSock;
extern int SrbServerMode;
extern int ClientCertInx;
#endif	/* CL_EXEC_H */
