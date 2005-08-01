/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* objSeqIoFunct.h
 * Header file for srbObjFunct.c.
 */

#ifndef OBJ_SEQ_FUNCT_H
#define OBJ_SEQ_FUNCT_H
#ifdef REFACTOR
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif
#if defined(PORTNAME_osx)
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#endif

#ifndef _WIN32
#include <sys/resource.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#include <string.h>
#include <errno.h>
 
#if defined(PORTNAME_solaris) 
#include <ctype.h>
#endif

#include "c.h"
#include "srb.h"
#include "dllist.h"
#include "commExtern.h"
#include "elogExtern.h"
#include "stubdef.h"
#include "exfSwExtern.h"
#include "srbMiscExtern.h"
#include "srbStat.h"
#include "exfFunctExtern.h"
#include "srbServerSfoExtern.h"
#include "clMiscExtern.h"
#include "clStubExtern.h"
#include "clStubIntern.h"
#include "clAuthExtern.h"
#ifdef TAPE_DRIVE
#include "srbTapeMiscExtern.h"
#endif
#ifdef PARA_OPR
#include "hpssParaOprExtern.h"
#endif
 
#include "MDriverExtern.h"
#include "dbLobjExtern.h"
#include "dbTableExtern.h"
#include "srbServerMiscExtern.h"
#include "packMsgExtern.h"
#include "packDefExtern.h"
#include "packStruct.h"
#include "srbObjFunctExtern.h"
#ifdef SRB_MDAS
#include "mdasC_db2_externs.h"
#endif
#include "srbC_mdas_externs.h"
#include "compoundObjExtern.h"
#include "authExtern.h"
#endif  /* REFACTOR */
#include "serverAll.h"
#include "clExecExtern.h"
#ifdef PARA_OPR
#include "hpssParaOprExtern.h"
#endif
#ifdef _WIN32
#include "Unix2Nt.h"
#endif
 
/* Registration retry - at least the Oracle MCAT fails when it is busy */

#define REG_RETRY_CNT_LIMIT  2	/* retry 2 times */	
#define REG_RETRY_TIME	     1  /* the amount of time to sleep */

extern int MaxThread;
extern int SizePerThread;
extern int StorVaultMode;
extern struct mdasResInfo *LocalResInfo;
  
#define LEGION_DOMAIN	1	/* The Legion domain */
#define DUMMY_DOMAIN	9999999	/* A dummy domain */

struct srbDomainMap srbDomainEntry[] = {
	{"legion", LEGION_DOMAIN},
	{"dummy",  DUMMY_DOMAIN}
};

struct srbAccessModeMap srbAccessModeEntry[] = {
	{"read",	O_RDONLY},
	{"write",	O_WRONLY},
	{"write",	O_RDWR},
	{"write",	O_WR_REPL},
	{"dummy",	999999}
};

struct inputValue copiesValue[] = {
	{INT,	INT_INPUT},		/* an integer */	
	{1,	"one"},		/* an integer */	
	{ALL,	"all"},			/* all resources or replica */
	{RR,	"RR"},			/* Round robin the resources */
	{RANDOM, "RANDOM"}		/* pick a random resource */
};

struct inputValue containerValue[] = {
        {STR,   STR_INPUT}              /* a string input */
};

struct inputValue replNumValue[] = {
        {INT,   INT_INPUT}              /* an integer */
};

struct inputValue contFdValue[] = {
        {INT,   INT_INPUT}              /* an integer */
};

struct inputValue standfStageValue[] = {
        {INT,   INT_INPUT}              /* an integer */
};

struct inputValue chksumValue[] = {
        {INT,   INT_INPUT}              /* an integer */
};
struct inputValue strValue[] = {
        {STR,   STR_INPUT}              /* a string input */
};

struct condInputMap condInputEntry[] = {
        {COPIES,	"COPIES", 	"=",	copiesValue, 5, DefCopies},
        {CONTAINER,	"CONTAINER", 	"=",	containerValue, 1,DefContainer},
        {REPL_NUM,	"COPY", 	"=",	replNumValue, 1, DefReplNum},
        {CONT_FD,	"CONT_FD", 	"=",	contFdValue, 1, DefContFd},
	{STANDF_STAGE,  "STANDF_STAGE", "=",    standfStageValue, 1, DefStandfStage},
        {REG_CHKSUM,	"REG_CHKSUM", 	"=",	chksumValue, 1, DefChksum},
        {VERIFY_CHKSUM,	"VERIFY_CHKSUM", "=",	chksumValue, 1, DefChksum},
        {FILE_GROUP,	"FILE_GROUP", 	"=",	strValue, 1,DefFileGroup},
};

resClass resClassEntries[] = {
    {GENERIC_RES_CL, 		0, 0},
    {ARCHIVAL_RES_CL, 		1, 0},
    {PERMANENT_RES_CL, 		1, 0},
    {CACHE_RES_CL, 		0, 0},
    {VOLATILE_RES_CL,		0, 0},
    {TEMPORARY_RES_CL, 		0, 0},
    {PRI_ARCHIVAL_RES_CL, 	1, 1},
    {PRI_CACHE_RES_CL, 		0, 1},
    {PRI_PERMANENT_RES_CL, 	1, 1},
};

int nResClass =
    (sizeof(resClassEntries) / sizeof (resClass));

/* Global for caching resInfo */

struct resCache *ResCacheHead = NULL;
struct resCache *CompResCacheHead = NULL;

proxyFunc proxyFuncEntries[] = {
    {OPR_COPY, (proxyFuncPtr) proxyCopy, "proxyCopy"},
    {OPR_COMMAND, (proxyFuncPtr) proxyCommand, "proxyCommand"},
    {SFO_CREATE_INDEX,(proxyFuncPtr) svrSfoCreateIndex, "svrSfoCreateIndex"},
    {SFO_DELETE_INDEX,(proxyFuncPtr) svrSfoDeleteIndex, "svrSfoDeleteIndex"},
    {SFO_SEARCH_INDEX,(proxyFuncPtr) svrSfoSearchIndex, "svrSfoSearchIndex"},
    {SFO_GET_MORE_SEARCH_RESULT,(proxyFuncPtr) svrSfoGetMoreSearchResult, "svrSfoGetMoreSearchResult"},
    {SFO_APPLY_FILTER,(proxyFuncPtr) svrSfoApplyFilter, "svrSfoApplyFilter"},
    {SFO_GET_MORE_FILTER_RESULT,(proxyFuncPtr) svrSfoGetMoreFilterResult, "svrSfoGetMoreFilterResult"},
    {OPR_FUNCTION, (proxyFuncPtr) proxyFunction, "proxyFunction"},
};

int nProxyOpr =
    (sizeof(proxyFuncEntries) / sizeof (proxyFunc));

/* Number of domains defined */

int nSrbDomain =
    (sizeof(srbDomainEntry) / sizeof(struct srbDomainMap)) - 1;
 
/* Number of srbAccessModeEntry */

int nSrbAccessMode =
    (sizeof(srbAccessModeEntry) / sizeof(struct srbAccessModeMap)) -1;
 
/* Number of condInputEntry */

int ncondInput =
    (sizeof(condInputEntry) / sizeof(struct condInputMap));

extern struct srbObjDesc srbObjDesc[];
 
int ParaCreateFlag = 0;

extern int TicketUserFlag;

extern struct hostElement *LocalHostHead;
#ifdef FED_MCAT
extern struct hostElement *LocalMcat;
#endif
extern userInfo *ClientUser, *ProxyUser, *ThisUser;

#endif	/* OBJ_SEQ_FUNCT_H */
