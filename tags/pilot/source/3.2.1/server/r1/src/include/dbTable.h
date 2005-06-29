/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*-------------------------------------------------------------------------
 *
 * dbTable.h - Header file for dbTable.c
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef DB_TABLE_H
#define DB_TABLE_H

#if defined(PORTNAME_osx)
#include <sys/types.h>
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <stdio.h>
#include "c.h"
#include "srb.h"
#include <fcntl.h>      /* for most O_ */
#ifndef O_RDONLY
#include <sys/file.h>   /* for the rest O_ */
#endif /* O_RDONLY */
#ifndef _WIN32
#include <unistd.h>    
#endif

#include "elogExtern.h"
#include "dbTableExtern.h"
#include "srbMiscExtern.h"
#include "exfFunctExtern.h"
#include "dbTableDefines.h" 


/* The the default database user and password */

static char *Db2Passwd = NULL;
static char *Db2User = NULL;
static char *IllusPasswd = NULL;
static char *IllusUser = NULL;
static char *OraclePasswd = NULL;
static char *OracleUser = NULL;
static char *InformixUser = NULL;
static char *InformixPasswd = NULL;
static char *PostgresUser = NULL;
static char *PostgresPasswd = NULL;
static char *SybaseUser = NULL;
static char *SybasePasswd = NULL;
static char *MysqlUser = NULL;
static char *MysqlPasswd = NULL;

/* the keyword for each of the above variables in the config file */

extern userInfo *ProxyUser, *ClientUser, *ThisUser;

dbTableDesc gDbTableDesc[MAX_DB_TABLE_DESC];
static int newDbTableDesc(struct hostElement *hostTabPtr, int remoteFlag,
int dbSystemType, int fd);
static void delDbTableDesc(int i);
char *readTableConfig (char *dbInstance, char *keyWd);
 
extern int TicketUserFlag;

#endif	/* DB_TABLE_H */
