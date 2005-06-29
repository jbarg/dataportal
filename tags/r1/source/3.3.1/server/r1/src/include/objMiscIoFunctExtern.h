/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/* objMiscIoFunctExtern.h
 * Definition and prototype for srbObjFunct.c.
 */

#ifndef OBJ_MISC_IO_FUNCT_EXTERN_H
#define OBJ_MISC_IO_FUNCT_EXTERN_H

#define MAX_SRBO_DESC   256

 
/* Flag val definition for BulkUnload */

#define BL_LOCAL_FLAG		0x1	/* do it locally */
#define BL_EXIT_WHEN_DONE	0x2	/* exit when done */
#define BL_QUERY_DONE           0x4     /* query already done */
#define BL_COPY_FLAG            0x8     /* bulk copy */

/* definition for parseObjDesc */

#define NORMAL_FILE_TYPE	0
#define INCONTAINER_FILE_TYPE	1
#define CONTAINER_FILE_TYPE	2

#define MULTI_ROW       1       /* the sql result contains multiple rows */
#define SINGLE_ROW      0       /* the sql result contains a single row */

/* The following defines the keyword and format expected of
 *  Replication Condition.
 */

/* keyword index definition */

#define COPIES    0       /* number of copies wanted */
#define CONTAINER 1       /* The container name */
#define REPL_NUM  2       /* The repplica number wanted */
#define CONT_FD   3       /* The fd of an opened container */
#define STANDF_STAGE   4  /* A specical standford stage */
#define REG_CHKSUM     5  /* register the checksum */
#define VERIFY_CHKSUM  6  /* verify the chksum */
#define FILE_GROUP  7  	  /* change file group */


#define INT             0
#define STR             1
#define ALL             1000000
#define RR              1000001
#define RANDOM          1000002
#define INT_INPUT               "integer"
#define STR_INPUT               "string"
#define DefCopies       "one"   /* def value for COPIES of Replica Condition */
#define DefContainer    ""      /* def value for container */
#define DefReplNum      INT_INPUT /* def value for Replica Number */
#define DefChksum       "0"     /* def value for Replica Number */
#define DefContFd       INT_INPUT /* def value for container fd */
#define DefFileGroup    ""      /* def value for container */

/* The deffault values for the mdas registration */

#define DefSchemaName   "null"
#define DefAccess       "all"

#define MATCHED_INFO    -999999 /* used in matchRescWithInfo, phyResMaxObjSize
                                 * is set to this value if the resc matches
                                 * one of the infoOut.
                                 */

/* definition for the unloadFlag */

#define UN_UNPROC       0
#define UN_INPROC       1
#define UN_REJECT       2
#define UN_SELECTED     3

/* definition for queResInfo flag */
#define QUE_BOTTOM      0       /* que it to the bottom */
#define QUE_TOP         1       /* que it to the top */
#define QUE_BY_FREE_SPACE       2       /* que it by free space */

#define DESC_TAKEN      999999

/* used out _objReplicate to indicate the source object is of zero length */

#define SOURCE_EMPTY      -999999

/* define some standford stage values */
#define DefStandfStage  INT_INPUT /* def value for standford stage */
#define STANDF_STAGE_CMD "ooss_Stage"

/* The SRB object descriptor */

struct srbObjDesc {
    char *objID;                /* The object name */
    char *dataPath;             /* for file type, this is the file path.
				 * For comp obj, this is the path for
				 * int comp obj */
    char *compDataPath;         /* for file type, this is file path for 
				 * compound obj */
    char *collection;           /* The collection name */
    char *resourceName;         /* The resource name */
    int lowLevelDescInx;        /* exf_desc or Db_desc index */
    int objTypeInx;             /* Indexed into the srbObjTypeEntry[] table */
    int replNum;
    int segNum;
    srb_long_t baseOffset;      /* The base offset (for inContainer obj) */
    srb_long_t objSize;         /* For in-container obj, if > 0 => existing
                                 * object */
    int lockFd;			/* The file descriptor of the locked file */
    int nextDescInx;            /* The next srbObjDesc index in the chain */
    int contInx;		/* The Index to a container. An inContainer
				 * object if it is >= 0 */
    int isDir;			/* Index for data/coll */
    int markedDirty;            /* Whether dirty has been marked */
    int intReplNum;		/* internal compound repl number. If it is
				 * >= 0, it is a internal compound obj */
    int intSegNum;
    int chksum;
    int chksumFlag;
    struct mdasInfoOut *dirtyInfo;   /* The link list of dirty info
                                         * associated with the file */
    struct containerInfo *contInfo;   /* If non-NULL, it is a container */
    struct collectionInfo *collInfo;  /* If non-NULL, it is an opened coll */
    char fileGroup[ENV_LEN];
};

struct containerInfo {
    srb_long_t curOffset;       /* Currently offset position in container */
    srb_long_t curContainerSize; /* The current sizeofthe container */
    srb_long_t containerMaxSize; /* Object is in-container if > 0 */
    int cleanOnClose;		/* flag to indicate whether this desc should
				 * be closed when a inContainer object closes.
				 */
};

struct collectionInfo {
    int maxDirLen;
    int offset;
    int accumOffset;    /* The accumulative offset up to the beginning
                         * of this dirent */
    int continueIndex;
    int gotData;
    void *dirent;
};

/* struct for input/output for the mdasGetInfo call */

struct mdasInfoInp {         /* input sturct */
    char *objID;
    char *userName;
    char *accessMode;
    char *domainName;
    char *collectionName;
};

#define MBYTES	1048576		/* one Mbytes */
#define ASSUMED_SIZE	(1024 * MBYTES) /* size = -1 ==> assumed 1 g */
#define SIZE_FACTOR	100	/* the multiplication factor for size */

/* structure for cashing the mdasResInfo */

struct resCache {
    char *resName;
    struct hostElement *mcatHostPtr;
    struct mdasResInfo *resInfoHead;
    struct resCache *next;
};

struct inputValue       {
    int valueInx;
    char *value;
};

struct condInputMap    {
    int keywordInx;             /* The keyword index */
    char * keyword;             /* The keyword */
    char *operator;             /* operator of the condition */
    struct inputValue *inputValue;      /* possible values */
    int nReplValue;             /* number of possible in inputValue */
    char *defValue;             /* the default value */
};

struct condResult {     /* The result for for parsing the conditional input */
    int intResult;
    struct inputValue *inputValue;
    char strResult[MAX_TOKEN];
};

/* The proxy operation definition */

/* ptr to func returning int */
typedef int ((*proxyFuncPtr)());

typedef struct {
    ProxyOprId  oprId;
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

/* The following maps the domainName to a domainInx */

struct srbDomainMap {
    char *domainName;
    int domainInx;
};

/* The follow maps the MDAS access mode to the UNIX flags (open ()) */

struct srbAccessModeMap {
    char* accessMode;
    int flag;
};

int set_audit_trail (int set_value);
extern void initObjDesc();
int
newObjDesc ();
extern void closeAllDesc ();
void 
delObjDesc (int i);
int
parseObjDesc (int descInx, int *objTypeInx, int *lowLevelDescInx,
int *contInx);
void
fillObjDescWithInfo (int descInx, int lowLevelDescInx, int objTypeInx,
char *collection, struct mdasInfoOut *infoOut);
void
fillObjDescWithContInfo (int descInx, srb_long_t curOffset,
srb_long_t curContainerSize, srb_long_t containerMaxSize,
struct mdasInfoOut *dirtyInfo, int markedDirty, int replNum);
void 
fillObjDescWithInContInfo (int descInx, char *objID, char *collectionName,
srb_long_t baseOffset, srb_long_t dataSize, int contInx, int lowLevelDescInx,
int chksum);
int
svrGetCollDesc (char *myPath);
void
fillObjDesc (int descInx, int lowLevelDescInx, int objTypeInx,
char *objID, char *collection, char *dataPath, char *resourceName,
int replNum, srb_long_t objSize);
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
combineInfoOut (struct mdasInfoOut **topInfoOut,
struct mdasInfoOut **botInfoOut);
int
sortAndCombInfo (struct mdasInfoOut **infoOutHead);
int
sortInfoForSyncD (struct mdasInfoOut **infoOutHead,
struct mdasInfoOut **dirtyInfoOut,
struct mdasInfoOut **cleanInfoOut);
int
replInfoOut (struct mdasInfoOut **infoOutHead,
struct mdasInfoOut *inpInfoOut);
struct mdasInfoOut *getInfoOutByClass (char *classStr,
struct mdasInfoOut *infoOutHead);
int
queInfoOut (struct mdasInfoOut **empCacheInfoOut,
struct mdasInfoOut *tmpInfoOut, int singleFlag, int TopFlag);
int
sortInfoForRepl (struct mdasInfoOut **infoOut, char *resourceName);
int sortResByClass (struct mdasResInfo *resInfoHead,
struct mdasResInfo **oPriCacheResInfo,
struct mdasResInfo **oPriArchResInfo,
struct mdasResInfo **oCacheResInfo, struct mdasResInfo **oArchResInfo);
int
sortResByClass (struct mdasResInfo *resInfoHead,
struct mdasResInfo **oPriCacheResInfo,
struct mdasResInfo **oPriArchResInfo,
struct mdasResInfo **oCacheResInfo, struct mdasResInfo **oArchResInfo);
int queResInfo (struct mdasResInfo **topResInfo,
struct mdasResInfo *resInfo, int flag);
int combineResInfo (struct mdasResInfo **topResInfo,
struct mdasResInfo **botResInfo);
extern struct varlena *svrObjProxyOpr (ProxyOprId operation,
int inputInt1, int inputInt2, int inputInt3, int inputInt4,
struct varlena *vInputStr1, struct varlena *vInputStr2,
struct varlena *vInputStr3, struct varlena *vInputStr4,
struct varlena *vInputStr5, struct varlena *vInputStr6,
struct varlena *vInputStr7);
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
void freeInfoOut (struct mdasInfoOut *infoOut);
void freeAllInfoOut (struct mdasInfoOut *infoOutHead);
void freeResInfo (struct mdasResInfo *resInfo);
int
svrObjLockRange (int fd, srb_long_t *offset, int size, int mode);
int
_svrObjLockRange (int fd, srb_long_t offset, int size, int mode);
extern struct varlena *
svrObjGetdents (int catType, int fd, int nbyte);
extern struct varlena *
svrObjGetdents64 (int catType, int fd, int nbyte);
extern int
svrCollSeek (int fd,  int where, int whence, int is64Flag );
extern int svrCreateCollect (int catType, struct varlena *vParentCollect,
struct varlena *vNewCollect);
extern int _svrCreateCollect (int catType, char *parentCollect,
char *newCollect);
extern struct varlena *svrListCollect (int catType,
struct varlena *vCollectionName, struct varlena *vFlag, int rowsWanted);
extern int svrModifyCollect (int catType, struct varlena *vCollectionName,
struct varlena *vDataValue1, struct varlena *vDataValue2,
struct varlena *vDataValue3, int retractionType);
extern int _svrModifyCollect (int catType, char *collectionName,
char *dataValue1, char *dataValue2, char *dataValue3, int retractionType);
extern struct varlena *
svrIssueTicket (struct varlena *vObjID,
struct varlena *vCollectionName, struct varlena *vCollectionFlag,
struct varlena *vBeginTime, struct varlena *vEndTime,
int accessCnt, struct varlena *vTicketUser);
extern int
svrRemoveTicket (struct varlena *vTicket);
int
svrRemoveTicketWithZone (struct varlena *vMcatZone, struct varlena *vTicket);
int
interPathCheck (char *objID);
extern int
svrObjLock (struct varlena *vObjID, struct varlena *vCollectionName,
int lockCmd, int lockType, int lockFd);
extern int
_svrObjLock (char *objID, char *collectionName, int lockCmd, int lockType,
int lockFd);
int
genCompPath (char *outPath, char *objID);
extern struct varlena *svrSetupSessionPublicKey ();
extern int svrSetupSession(struct varlena *sessionKey);
extern int _svrSetupSession(char *sessionKey);
int
_standfStage (int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName, struct mdasInfoOut *origInfoOut,
int regFlag);
int
getNumThreadBySize (srb_long_t size, int inpNumThreads);
srb_long_t
_getWriteSize (int objTypeInx, int lowLevelDescInx);
int
_svrObjFStage (int catType, char *objID, char *collectionName,
char *resource, int flag);
int
svrObjFStage (int catType, struct varlena *vObjID, struct varlena *vCollection,
struct varlena *vResource, int flag);
extern struct varlena *svrObjProc (int descInx, int outLen,
                                   struct varlena *vprocName,
                                   struct varlena *vinBuf);
extern int _svrObjProc(int descInx, char *procName, char *inBuf, int inLen,
           char *outBuf, int outLen );
int
svrCollClose (int descInx);
int
rmContentOfInfoOut (struct mdasInfoOut *infoOut, char *collectionName);
int
getInfoOutForOpr (struct mdasInfoOut *inpInfoOut, char *collectionName,
int openFlag, int *lockFd, int stageFlag, struct mdasInfoOut **outInfoOut,
struct mdasInfoOut **outCacheInfoOut);
int
getAndQueNewObjDesc (int *headDescInx);
void
lowLevelCloseAllObjDesc (int *headDescInx);
void
setObjDescCond (int descInx, char *condInput);
void 
setChksumFromCond (char *condInput, int *chksumFlag, int *iChksum);
void
setFileGroupFromCond (char *condInput, char *outFileGroup);
#endif	/* OBJ_MISC_IO_FUNCT_EXTERN_H */

