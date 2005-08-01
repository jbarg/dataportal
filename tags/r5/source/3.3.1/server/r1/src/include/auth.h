/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/*-------------------------------------------------------------------------
 *
 * auth.h--
 *    Definitions for auth.c
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef AUTH_H
#define	AUTH_H

#ifdef REFACTOR
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#ifndef _WIN32
#include <sys/param.h>  /* for MAX{HOSTNAME,PATH}LEN, NOFILE */
#include <pwd.h>
#endif /* WIN32 */

#include <ctype.h>                      /* isspace() declaration */

#include "c.h"
#include "srb.h"
#include "commExtern.h"
#include "authExtern.h"
#include "clAuthExtern.h"
#include "elogExtern.h"
#include "authExtern.h"
#endif	/* REFACTOR */

#include "serverAll.h"

#define MAX_AUTH_FAILURE	5

/* This is a server global that hold the auth scheme of the client */
extern struct hostElement *LocalHostHead;
extern struct hostElement *RemoteHostHead;
extern struct hostElement *LocalMcat;
extern int DebugLvl;
extern int TicketUserFlag;
extern userInfo *ProxyUser, *ClientUser, *ThisUser;
extern int ClientCertInx;

extern char *getSrbDataDir();
extern int
compareGSIDn (char *dn1, char *dn2);
extern int
compareGSIDnByKeywd (char *dn1, char *dn2, char *keywd);
extern void
freeSqlResult (mdasC_sql_result_struct *myresult);
int aid_setup_creds(char *service_name, int client_flag, int type_flag);
int aid_establish_context_serverside(int fd, char *clientName, 
int max_len_clientName, int type_flag);
int aid_establish_context_serverside_with_delegate(int fd, char *clientName,
int max_len_clientName, int type_flag);

#endif /* AUTH_H */
