/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbMisc.h - Header file for misc.c
 */

#ifndef SRB_MISC_H
#define SRB_MISC_H
 
#ifdef REFACTOR
#include <string.h>
#ifndef _WIN32
#include <sys/param.h>
#endif
#include <sys/types.h>
#if defined(PORTNAME_solaris)
#include <ctype.h>
#endif
#include <sys/stat.h>
#ifndef _WIN32
#include <sys/file.h>
#endif
#include <stdio.h>
#include "srb.h"
#ifndef _WIN32
#include <grp.h>
#include <pwd.h>
#endif
#include <signal.h>
 
#include "c.h"
#include "elogExtern.h"
#include "vault.h"
#include "clAuthExtern.h"
#include "clConnectExtern.h"
#include "srbMiscExtern.h"
#include "distribExtern.h"
#include "clStubExtern.h"
#endif  /* REFACTOR */
#include "serverAll.h"
 
void setSrbUserName();
char* getSrbUserName();
/*  char *UserName; */
char    OutputFileName[MAXEXFPATH + 1];

struct vaultQueElement *vaultQueHead = NULL;
static int      UserId = -1;
int DebugLvl = 1;	/* debug level. default to lowest level */

struct srbObjTypeMap srbObjTypeEntry[] = {
        {"unix",        	FILE_TYPE,      UNIX_FT,	DO_CHKDIR},
        {"unitree",     	FILE_TYPE,      UNITREE_FT,	DO_CHKDIR},
        {"hpss",        	FILE_TYPE,      HPSS_FT,	DO_CHKDIR},
        {"ftp",         	FILE_TYPE,      FTP_FT,		NO_CHKDIR},
        {"http",        	FILE_TYPE,      HTTP_FT,	NO_CHKDIR},
	{"null",     		FILE_TYPE,      NULL_FT,        NO_CHKDIR},
        {"db2 dblobj",  	DB_LOBJ_TYPE,   DB2_LOBJ,	NO_CHKDIR},
        {"illustra dblobj",     DB_LOBJ_TYPE,   ILLUSTRA_LOBJ,	NO_CHKDIR},
        {"objstore dblobj",    	DB_LOBJ_TYPE,   OBJSTORE_LOBJ,	NO_CHKDIR},
        {"oracle dblobj",      	DB_LOBJ_TYPE,   ORA_LOBJ,	NO_CHKDIR},
        {"db2 table",  		DB_TABLE_TYPE,  DB2_TABLE,	NO_CHKDIR},
        {"illustra table",  	DB_TABLE_TYPE,  ILLUSTRA_TABLE,	NO_CHKDIR},
        {"objstore table",    	DB_TABLE_TYPE,  OBJSTORE_TABLE,	NO_CHKDIR},
        {"oracle table",      	DB_TABLE_TYPE,  ORA_TABLE,	NO_CHKDIR},
	{"dpss",                FILE_TYPE,      DPSS_FT,        NO_CHKDIR},
	{"ntfs",                FILE_TYPE,      NTFS_FT,        DO_CHKDIR},
	{"cmstore",             FILE_TYPE,      CMSTORE_FT,     DO_CHKDIR},
	{"tapeCache",           FILE_TYPE,      UNIX_FT,        DO_CHKDIR},
	{TAPE_RESC,             TAPE_TYPE,      TAPE_SYS,     	NO_CHKDIR},
	{COMPOUND_RESC,         COMPOUND_TYPE,  SRB_ARCH,       NO_CHKDIR},
	{"ads_uk",              FILE_TYPE,      ADS_FT,         NO_CHKDIR},
	{"dCache",              FILE_TYPE,      DCACHE_FT,      DO_CHKDIR},
        {"UNIX_NOCHK",        	FILE_TYPE,      UNIX_FT,	NO_CHKDIR},
        {"HPSS_NOCHK",        	FILE_TYPE,      HPSS_FT,	NO_CHKDIR},
	{"informix table",      DB_TABLE_TYPE,  IFMX_TABLE,     NO_CHKDIR},
	{"postgres table",      DB_TABLE_TYPE,  PSG_TABLE,      NO_CHKDIR},
	{"sybase table",        DB_TABLE_TYPE,  SYB_TABLE,      NO_CHKDIR},
	{"mysql table",         DB_TABLE_TYPE,  MYS_TABLE,      NO_CHKDIR},
	{"informix dblobj",    	DB_LOBJ_TYPE,   IFMX_LOBJ,	NO_CHKDIR},
	{"postgres dblobj",    	DB_LOBJ_TYPE,   PSG_LOBJ,	NO_CHKDIR},
	{"sybase dblobj",      	DB_LOBJ_TYPE,   SYB_LOBJ,	NO_CHKDIR},
	{"mysql dblobj",      	DB_LOBJ_TYPE,   MYS_LOBJ,	NO_CHKDIR},
        {"gridFTP",             FILE_TYPE,      GRIDFTP_FT,     CHK_AND_MK_VAULT},
	{"antelope orb misc driver",  MDRIVER_TYPE,   ANT_ORB,  NO_CHKDIR},
        {"datascope misc driver",  MDRIVER_TYPE,   DATASCOPE,  NO_CHKDIR},
        {"dummy",       	999999,         999999,		NO_CHKDIR}
};

/* Number of srbObjTypeEntry */

int nSrbObjType =
    (sizeof(srbObjTypeEntry) / sizeof(struct srbObjTypeMap)) -1;

/* global for chkdirFlag == CHK_AND_MK_VAULT */
int VaultpathStatus = PATH_NOT_IN_VAULT;

#ifdef FED_MCAT
extern struct hostElement *LocalMcat;
#endif

extern userInfo *ClientUser;
extern userInfo *ProxyUser;
extern userInfo *ThisUser;
extern struct mdasEnvData *ThisMdasEnvData;
extern int TicketUserFlag;

extern char *getcwd();
extern int DebugLvl;
extern int mdasEnabled;
extern mdasC_sql_result_struct *SrbUserList;
extern struct hostElement *LocalHostHead;
extern struct hostElement *RemoteHostHead;

#endif  /* SRB_MISC_H */

