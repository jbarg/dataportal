/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbObjFunct.h
 * Header file for srbObjFunct.c.
 */

#ifndef SRB_OBJ_FUNCT_H
#define SRB_OBJ_FUNCT_H

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
#include "clStubExtern.h"
#include "clStubIntern.h"
#include "clAuthExtern.h"
#include "clConnectExtern.h"
#ifdef TAPE_DRIVE
#include "srbTapeMiscExtern.h"
#endif
#ifdef PARA_OPR
#include "hpssParaOprExtern.h"
#endif
 
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
 
#define DESC_TAKEN      999999

/* used out _objReplicate to indicate the source object is of zero length */

#define SOURCE_EMPTY      -999999
 
#define MAX_SRBO_DESC	256

/* Registration retry - at least the Oracle MCAT fails when it is busy */

#define REG_RETRY_CNT_LIMIT  2	/* retry 2 times */	
#define REG_RETRY_TIME	     1  /* the amount of time to sleep */

#define MULTI_ROW	1	/* the sql result contains multiple rows */
#define SINGLE_ROW	0	/* the sql result contains a single row */

#ifdef PORTNAME_linux
#define BUF_SIZE        1048576   	/* 1 Mb */
#else
#if defined(PORTNAME_osx)
#define BUF_SIZE        1048576 / 4     /* 0.25 Mb */
#else
#define BUF_SIZE	2097152 	/* 2 Mb */
#endif
#endif

/* definition for queResInfo flag */
#define QUE_BOTTOM	0	/* que it to the bottom */
#define QUE_TOP		1	/* que it to the top */
#define QUE_BY_FREE_SPACE	2	/* que it by free space */

extern int MaxThread;
extern int SizePerThread;
extern int StorVaultMode;
extern struct mdasResInfo *LocalResInfo;
  
/* The deffault values for the mdas registration */

#define DefSchemaName	"null"
#define DefAccess	"all"


/* The following maps the domainName to a domainInx */

struct srbDomainMap {
    char *domainName;
    int domainInx;
};

#define LEGION_DOMAIN	1	/* The Legion domain */
#define DUMMY_DOMAIN	9999999	/* A dummy domain */

struct srbDomainMap srbDomainEntry[] = {
	{"legion", LEGION_DOMAIN},
	{"dummy",  DUMMY_DOMAIN}
};

/* The follow maps the MDAS access mode to the UNIX flags (open ()) */

struct srbAccessModeMap {
    char* accessMode;
    int flag;
};

struct srbAccessModeMap srbAccessModeEntry[] = {
	{"read",	O_RDONLY},
	{"write",	O_WRONLY},
	{"write",	O_RDWR},
	{"write",	O_WR_REPL},
	{"dummy",	999999}
};

/* The following defines the keyword and format expected of
 *  Replication Condition.
 */

/* keyword index definition */

#define COPIES 	  0       /* number of copies wanted */
#define CONTAINER 1       /* The container name */
#define REPL_NUM  2       /* The repplica number wanted */
#define CONT_FD   3       /* The fd of an opened container */
#define STANDF_STAGE   4  /* A specical standford stage */
#define REG_CHKSUM     5  /* register the checksum */
#define VERIFY_CHKSUM  6  /* verify the chksum */


struct inputValue	{
    int valueInx;
    char *value;
};

struct condInputMap    {
    int keywordInx;             /* The keyword index */
    char * keyword;             /* The keyword */
    char *operator;		/* operator of the condition */
    struct inputValue *inputValue;	/* possible values */
    int nReplValue;		/* number of possible in inputValue */
    char *defValue;		/* the default value */
};

#define INT		0
#define STR		1
#define ALL		1000000
#define RR		1000001
#define RANDOM		1000002
#define INT_INPUT		"integer"
#define STR_INPUT		"string"
#define DefCopies	"one"	/* def value for COPIES of Replica Condition */
#define DefContainer	""      /* def value for container */
#define DefReplNum	INT_INPUT /* def value for Replica Number */
#define DefChksum	"0"     /* def value for Replica Number */
#define DefContFd       INT_INPUT /* def value for container fd */
/* define some standford stage values */
#define DefStandfStage  INT_INPUT /* def value for standford stage */
#define STANDF_STAGE_CMD "ooss_Stage"

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

struct condInputMap condInputEntry[] = {
        {COPIES,	"COPIES", 	"=",	copiesValue, 5, DefCopies},
        {CONTAINER,	"CONTAINER", 	"=",	containerValue, 1,DefContainer},
        {REPL_NUM,	"COPY", 	"=",	replNumValue, 1, DefReplNum},
        {CONT_FD,	"CONT_FD", 	"=",	contFdValue, 1, DefContFd},
	{STANDF_STAGE,  "STANDF_STAGE", "=",    standfStageValue, 1, DefStandfStage},
        {REG_CHKSUM,	"REG_CHKSUM", 	"=",	chksumValue, 1, DefChksum},
        {VERIFY_CHKSUM,	"VERIFY_CHKSUM", "=",	chksumValue, 1, DefChksum},
};

struct condResult {	/* The result for for parsing the conditional input */
    int intResult;	
    struct inputValue *inputValue;
    char strResult[MAX_TOKEN];
};

#define MATCHED_INFO	-999999 /* used in matchRescWithInfo, phyResMaxObjSize
				 * is set to this value if the resc matches
				 * one of the infoOut.
				 */
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

static int nResClass =
    (sizeof(resClassEntries) / sizeof (resClass));

/* The proxy operation definition */

/* ptr to func returning int */
typedef int ((*proxyFuncPtr)());

typedef struct {
    ProxyOprId	oprId;
    proxyFuncPtr    func;
    char*       funcName;
} proxyFunc;

/* struct for the getBulkUnloadHost routine */

struct resSort {
    char *resourceName;
    int resClass;  /* a special kind. 0 - unix or temp type. 1 - the rest */
    srb_long_t count;
    struct resSort *next;
};

struct hostSort {
    int remoteFlag;
    struct hostElement  *hostTabPtr;
    struct resSort *resPtr;
    struct hostSort *next;
};

struct bunloadSort {
    int fileCnt;
    int *unloadFlag;
    mdasC_sql_result_struct *myresult;
    struct bunloadSort *next;
};

/* definition for the unloadFlag */

#define UN_UNPROC       0
#define UN_INPROC       1
#define UN_REJECT       2
#define UN_SELECTED     3

/* Global for caching resInfo */

struct resCache *ResCacheHead = NULL;
struct resCache *CompResCacheHead = NULL;

struct varlena *
proxyCopy (int inputInt1, int inputInt2, int inputInt3, int inputInt4,
char *inputStr1, char *inputStr2, char *inputStr3, char *inputStr4, 
struct varlena *vInputStr1, struct varlena *vInputStr2, 
struct varlena *vInputStr3);

struct varlena *
proxyCommand (int portalFlag, int connPort, int resolvePath, int inputInt4,
char *command, char *commandArgv, char *proxyAddr, char *clientIP,
char *srbPath, struct varlena *vInputStr1, struct varlena *vInputStr2);

struct varlena *
proxyFunction (int inputInt1, int inputInt2, int inputInt3, int inputInt4,
char *inputStr1, char *inputStr2, char *inputStr3, char *inputStr4, 
struct varlena *vInputStr1, struct varlena *vInputStr2, 
struct varlena *vInputStr3);

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

static int nProxyOpr =
    (sizeof(proxyFuncEntries) / sizeof (proxyFunc));

/* Number of domains defined */

static int nSrbDomain =
    (sizeof(srbDomainEntry) / sizeof(struct srbDomainMap)) - 1;
 
/* Number of srbAccessModeEntry */

static int nSrbAccessMode =
    (sizeof(srbAccessModeEntry) / sizeof(struct srbAccessModeMap)) -1;
 
/* Number of condInputEntry */

static int ncondInput =
    (sizeof(condInputEntry) / sizeof(struct condInputMap));

struct srbObjDesc srbObjDesc[MAX_SRBO_DESC];
int newObjDesc ();
static void delObjDesc (int i);
static void fillObjDesc (int descInx, int lowLevelDescInx, int objTypeInx,
  char *objID, char *collection, char *dataPath, char *resourceName,
int replNum);
static void fillObjDescWithInfo (int descInx, int lowLevelDescInx, 
  int objTypeInx, char *collection, struct mdasInfoOut *infoOut);
static void
fillObjDescWithContInfo (int descInx, srb_long_t curOffset,
srb_long_t curContainerSize, srb_long_t containerMaxSize,
struct mdasInfoOut *dirtyInfo, int markedDirty, int replNum);
static void
fillObjDescWithInContInfo (int descInx, char *objID, char *collectionName,
srb_long_t baseOffset, srb_long_t dataSize, int contInx, int lowLevelDescInx,
int chksum);
static int domainLookUp (char *domainName);
static int mdasGetInfo (struct mdasInfoInp *infoInp, 
struct mdasInfoOut **infoOutHead);
static char *openFlagLookUp (int openFlag); 

struct varlena *sqlResultToVar (mdasC_sql_result_struct *myresult, int rowFlag);
 
void freeInfoOut (struct mdasInfoOut *infoOut);
void freeAllInfoOut (struct mdasInfoOut *infoOutHead);
void freeResInfo (struct mdasResInfo *resInfo);
int _objUnlink (int objType, int systemType, char *resLoc,
char *fullPathName, int chkDirFlag);
int _svrObjUnlink (char *objID, char *collectionName);

int my_copy_dataset (int catType, char *objID,  char *collectionName,
char *origResourceName, char *origPathName, char *newResourceName,
char *newPathName, char *userName, char *domainName);

int my_register_dataset_info (int catType, char *objID,  char *userName,
char *access, char *domainName, char *dataTypeName, char *pathName,
char *resourceName, char *collectionName, int dataSize);
int parseCondInput (char *condInput, int keywordInx,
struct condResult *condResult);
struct inputValue *matchCondInpValue (struct condInputMap 
*condInputMap, char *valueStr);
struct mdasResInfo *getResInfoByClass (struct mdasResInfo *resInfo, 
char *phyResClass);
struct mdasInfoOut *getInfoOutByClass (char *classStr, 
struct mdasInfoOut *infoOutHead);
int 
resolveContainer (int catType, int openFlag, int createFlag,
char *containerName, char *contCollection);
int
openContainer (int catType, int openFlag, char *containerName,
char *contCollection,
struct mdasInfoOut *infoOutHead, struct mdasInfoOut *emptyContHead);
int
getEmptyContainer (int catType, char *fullContPath,
 struct mdasInfoOut *infoOutHead, struct mdasInfoOut **emptyContHead);
int
sortInfoByClass (struct mdasInfoOut *infoOutHead,
struct mdasInfoOut **oPriCacheInfoOut,
struct mdasInfoOut **oPriArchInfoOut,
struct mdasInfoOut **oCacheInfoOut, struct mdasInfoOut **oArchInfoOut,
struct mdasInfoOut **oDirtyCacheInfoOut,
struct mdasInfoOut **oDirtyArchInfoOut);
int
sortAndCombInfoForObj (struct mdasInfoOut **infoOutHead,
struct mdasInfoOut **dirtyInfo, int openFlag, int replDirtyInfo);
int
replInfoOut (struct mdasInfoOut **infoOutHead,
struct mdasInfoOut *inpInfoOut);
int
combineInfoOut (struct mdasInfoOut **topInfoOut,
struct mdasInfoOut **botInfoOut);
int
queInfoOut (struct mdasInfoOut **empCacheInfoOut,
struct mdasInfoOut *tmpInfoOut, int singleFlag, int TopFlag);
int
sortInfoForRepl (struct mdasInfoOut **infoOut, char *resourceName);
int sortResByClass (struct mdasResInfo *resInfoHead,
struct mdasResInfo **oPriCacheResInfo,
struct mdasResInfo **oPriArchResInfo,
struct mdasResInfo **oCacheResInfo, struct mdasResInfo **oArchResInfo);
int queResInfo (struct mdasResInfo **topResInfo,
struct mdasResInfo *resInfo, int flag);
int combineResInfo (struct mdasResInfo **topResInfo,
struct mdasResInfo **botResInfo);
struct mdasInfoOut *
selectInfoOutByResc (char *resourceName, struct mdasInfoOut **infoOutHead);
int
matchRescWithContResult (char *resourceName, mdasC_sql_result_struct *myresult);
int
_svrReplContainer (int catType, char *containerName, char *newResourceName);

extern int TicketUserFlag;

extern struct hostElement *LocalHostHead;
#ifdef FED_MCAT
extern struct hostElement *LocalMcat;
#endif
extern userInfo *ClientUser, *ProxyUser, *ThisUser;
extern getresOnChoice();
extern int
_svrObjOpenWithInfo (struct mdasInfoInp *infoInp,
struct mdasInfoOut **infoOutHead, int openFlag);
extern int
parseCondResInfo (char *mcatHostHint, int catType, char *condInput, 
char *logResName, struct condResult *condResult, 
struct resCache **myCacheRes, srb_long_t dataSize);
extern int
_svrObjCreate (int catType, char *objID, char *dataType, char *logResName,
     char *collectionName, char *pathName, srb_long_t dataSize);
srb_long_t
_objSeek (int lowLevelDescInx, int objTypeInx, srb_long_t offset, int whence);
int
_objWrite(int lowLevelDescInx, int objTypeInx, char *buf, int len);
int
_objRead(int lowLevelDescInx, int objTypeInx, char *buf, int len);
int
_objReplicate (int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName, struct mdasInfoOut *origInfoOut,
struct mdasInfoOut **newInfoOut, struct mdasResInfo *inpResInfo);
int
sortInfoForSyncD (struct mdasInfoOut **infoOutHead,
struct mdasInfoOut **dirtyInfoOut, 
struct mdasInfoOut **cleanInfoOut);
int 
matchRescWithInfo (struct mdasResInfo *resInfo, 
struct mdasInfoOut *infoOutHead);
int
_objCreateWithResInfo (char *objID, char *collectionName, char *pathName,
srb_long_t dataSize, struct mdasResInfo *resInfo, char *fullPathName,
int *objTypeInx);
srb_long_t
_svrDataPutToResc (int catType, char *objID, char *collectionName,
char *clHostAddr, int clPort, srb_long_t size,
struct mdasResInfo *newResInfo, char *newPathName, char *newFullPathName,
int *newObjInx, int flag, int numThreads, svrComm_t *myComm);
#ifdef PARA_OPR
extern void
partialDataPut (struct paraPutGetInput *myInput);
extern void
partialDataGet (struct paraPutGetInput *myInput);
#endif /* PARA_OPR */
extern int
isResLocLocal (char *resourceLoc);
int
_standfStage (int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName, struct mdasInfoOut *origInfoOut,
int regFlag);
int
getNumThreadBySize (srb_long_t size, int inpNumThreads);
int
pickCopyToUnload (int numCopies, int startInx, int endInx,
struct bunloadSort *tmpBunloadSort);
int fillInfoOut (struct mdasInfoOut *infoOut, char *objID, char *resourceType,
char *dataPath, char *resourceLoc, char *resourceName,
char *dataType, char *replNum, char *offset, char *size, char *phyResClass,
char *containerName, char *isDirty, char *chksum, char *collection);
int
sendBunloadData (struct bunloadSort *bunloadHead, char *clHostAddr, int clPort,
svrComm_t *myComm, int flag, int myFd);
extern int
_svrBulkUnload (int catType, int flag, char *clHostAddr,
int clPort, char *collection, svrComm_t *myComm, portList_t **myPortList);
int
_objChksumAndReg (int objTypeInx, int flags, char *chksum,
srb_long_t myOffset, srb_long_t mySize, char *destObjID,
char *destCollection, char *phyResName, char *myPathName, char *phyResLoc,
int chksumFlag, unsigned long iChksum, char *regResName, char *regPathName);
int
parseObjDesc (int descInx, int *objTypeInx, int *lowLevelDescInx,
int *contInx);
srb_long_t
_getWriteSize (int objTypeInx, int lowLevelDescInx);
int
sendPortList (svrComm_t *myComm, portList_t *thisPortList);
int
sortResForIngest (struct resCache *myCacheRes, srb_long_t dataSize);
int
sortResBySpace (struct mdasResInfo *resInfoHead,
struct mdasResInfo **ovacantResInfo, struct mdasResInfo **ofilledResInfo,
srb_long_t dataSize);
struct mdasResInfo *
matchLocalResInfo (char *resName);
srb_long_t
getAndRegResSpace (struct mdasResInfo *myResInfo, time_t curTime);
int
setupSockForBunload (char *clHostAddr, int clPort, svrComm_t *myComm, int flag);

#endif	/* SRB_OBJ_FUNCT_H */
