/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*-------------------------------------------------------------------------
 *
 * dbLobj.h - Header file for dbLobj.c
 *
 *
 *-------------------------------------------------------------------------
 */

#ifndef DB_LOBJ_H
#define DB_LOBJ_H

#ifdef REFACTOR
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
#include "dbLobjExtern.h"
#include "srbC_db2_externs.h"
#include "srbMiscExtern.h"
#include "exfFunctExtern.h"
#include "srbC_ora_externs.h"
#include "clStubExtern.h"
#endif  /* REFACTOR */
#include "serverAll.h"
 
#define MAX_DB_LOBJ_DESC 256

#define LOBJ_CONFIG	"LobjConfig"	/* the lobj config file */

/* The the default database user and password */

static char *Db2Passwd = NULL;
static char *Db2User = NULL;
static char *IllusPasswd = NULL;
static char *IllusUser = NULL;
static char *OraclePasswd = NULL;
static char *OracleUser = NULL;

/* the keyword for each of the above variables in the config file */
#define DB2_PASSWD_KEYWD	"Db2Passwd"
#define DB2_USER_KEYWD		"Db2User"
#define ILLUS_PASSWD_KEYWD	"IllusPasswd"
#define ILLUS_USER_KEYWD	"IllusUser"
#define ORACLE_PASSWD_KEYWD	"OraclePasswd"
#define ORACLE_USER_KEYWD	"OracleUser"
#define DB2_HOME_KEYWD         "Db2Home"
#define ILLUS_HOME_KEYWD       "IllusHome"
#define ORACLE_HOME_KEYWD      "OracleHome"
#define INFORMIX_HOME_KEYWD     "InformixHome"
#define SYBASE_HOME_KEYWD       "SybaseHome"
#define POSTGRES_HOME_KEYWD     "PostgresHome"
#define MYSQL_HOME_KEYWD        "MysqlHome"

extern userInfo *ProxyUser, *ClientUser, *ThisUser;

dbLobjDesc gDbLobjDesc[MAX_DB_LOBJ_DESC];
static int newDbLObjDesc(struct hostElement *hostTabPtr, int remoteFlag,
int dbSystemType, int fd);
static void delDbLObjDesc(int i);
char *readLobjConfig (char *keyWd);
 
extern int TicketUserFlag;

#endif	/* DB_LOBJ_H */
