/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbObjFunctExtern.h
 * Definition and prototype for srbObjFunct.c.
 */
 
#ifndef SRB_OBJ_FUNCT_EXTERN_H
#define SRB_OBJ_FUNCT_EXTERN_H

#ifdef SRB_MDAS
#include "mdasGlobalsExtern.h"
#endif
 
/* Flag val definition for BulkUnload */

#define BL_LOCAL_FLAG		0x1	/* do it locally */
#define BL_EXIT_WHEN_DONE       0x2     /* exit when done */
#define BL_QUERY_DONE           0x4     /* query already done */
#define BL_COPY_FLAG            0x8     /* bulk copy */

/* definition for parseObjDesc */

#define NORMAL_FILE_TYPE	0
#define INCONTAINER_FILE_TYPE	1
#define CONTAINER_FILE_TYPE	2

#define MAX_BMOVE_SRC_RESC	30	/* max srcResource for bmove
					 * that can be handled */

#define BULK_MOVE_NUM_ROWS	200	/* XXXXXX only for testing */
/* #define BULK_MOVE_NUM_ROWS	5000 */
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

extern void initObjDesc();
extern int svrObjOpen (struct varlena *vDataID,
int openFlag, struct varlena *vCollectionName);
extern svrObjCreate (int catType, struct varlena *vObjID, 
struct varlena *vDataTypeName, struct varlena *vResourceName,
struct varlena *vCollectionName, struct varlena *vPathName,
srb_long_t *dataSize);
extern int svrObjClose (int descInx);
extern int _svrObjClose (int descInx);
extern int
lowLevelClose (int objTypeInx, int lowLevelDescInx);
extern svrObjUnlink(struct varlena *vDataID,
struct varlena *vCollectionName);
extern struct varlena *svrObjRead (int descInx, int len);
extern int svrObjWrite (int descInx, struct varlena *wbuf);
extern srb_long_t *svrObjSeek (int descInx, srb_long_t *offset, int whence);
extern srb_long_t _svrObjSeek (int descInx, srb_long_t offset, int whence);
extern int svrObjSync (int descInx);
extern struct varlena * svrGetDatasetInfo (int catType, struct varlena *vObjID,
struct varlena *vCollectionName, int rowsWanted, struct varlena *mode);
extern struct varlena *svrGetDataDirInfo (int cat_type, struct varlena *vQval,
struct varlena *vSelVal, int rowsWanted);
extern struct varlena *
svrGenQuery (int cat_type, struct varlena *vMyMcatZone,
struct varlena *vMyGenQuery, int rowsWanted);
extern svrRegisterDataset (int catType, struct varlena *vObjID,
struct varlena *vDataTypeName, struct varlena *vResourceName,
struct varlena *vCollectionName, struct varlena *vPathName,
srb_long_t *dataSize);
extern int
svrRegDatasetInternal (int catType, struct varlena *vObjID,
struct varlena *vUserName, struct varlena *vAccess, struct varlena *vDomainName,
struct varlena *vDataTypeName,
struct varlena *vResourceName, struct varlena *vCollectionName,
struct varlena *vPathName, srb_long_t * dataSize,
int rmObjTypeInx, struct varlena *vRmObjResLoc);
extern int svrModifyDataset (int catType, struct varlena *vObjID,
struct varlena *vCollectionName,
struct varlena *vResourceName, struct varlena *vPathName,
struct varlena *vDataValue1, struct varlena *vDataValue2,
int retractionType);
extern int svrObjReplicate (int catType, struct varlena *vObjID,
struct varlena *vCollectionName,
struct varlena *vNewResourceName, struct varlena *vNewPathName);
extern int svrObjMove (int catType, struct varlena *vObjID,
struct varlena *vCollectionName,
struct varlena *vNewResourceName, struct varlena *vNewPathName);
extern struct varlena *svrObjProxyOpr (ProxyOprId operation,
int inputInt1, int inputInt2, int inputInt3, int inputInt4,
struct varlena *vInputStr1, struct varlena *vInputStr2,
struct varlena *vInputStr3, struct varlena *vInputStr4,
struct varlena *vInputStr5, struct varlena *vInputStr6,
struct varlena *vInputStr7);
extern int myResAttrLookup(char *mcatHostHint, int catType, char *resourceName,
                       char *domainName, char **resourceType,
                       char **resourceLoc);
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
extern int svrRegisterUserGrp (int catType, struct varlena *vUserGrpName,
struct varlena *vUserGrpPasswd, 
struct varlena *vUserGrpType,
struct varlena *vUserGrpAddress, struct varlena *vUserGrpPhone,
struct varlena *vUserGrpEmail);
extern int svrRegisterUser (int catType, struct varlena *vUserName,
struct varlena *vUserDomain, struct varlena *vUserPasswd, 
struct varlena *vUserType,
struct varlena *vUserAddress, struct varlena *vUserPhone,
struct varlena *vUserEmail);
extern int svrModifyUser (int catType, struct varlena *vDataValue1,
struct varlena *vDataValue2, int retractionType);
extern int set_audit_trail (int set_value);
extern int svrObjAudit (int catType, struct varlena *vUserName, 
struct varlena *vObjID, struct varlena *vCollectionName, 
struct varlena *vDataPath, struct varlena *vResourceName, 
struct varlena *vAccessMode, struct varlena *vComment, 
int success, struct varlena *vDomainName);
extern int svrRegisterReplica (int catType, struct varlena *vObjID,
struct varlena *vCollectionName,
struct varlena *vOrigResourceName, struct varlena *vOrigPathName,
struct varlena *vNewResourceName, struct varlena *vNewPathName,
struct varlena *vUserName, struct varlena *vDomainName);
extern int
_svrRegisterDataset (int catType, char *objID, char *userName,
char *access, char *domainName, char *dataTypeName, char *pathName,
char *resourceName, char *collectionName,  
srb_long_t dataSize, int rmObjTypeInx, char *rmOhyResLoc);
extern int
_svrRegisterReplica (int catType, char *objID, char *collectionName,
char *origResourceName, char *origPathName,
char *newResourceName, char *newPathName,
int rmObjFlag, int objTypeInx, char *rmOhyResLoc,
char *userName, char *domainName);
extern struct varlena *svrGetPrivUsers (int catalog, int rowsWanted);
extern struct varlena *
svrGetMoreRows (int catalog, int contDesc, int rowsWanted);
extern struct varlena *
svrGenGetMoreRows (int catalog, int contDesc, int rowsWanted);
extern struct varlena *
svrIssueTicket (struct varlena *vObjID,
struct varlena *vCollectionName, struct varlena *vCollectionFlag,
struct varlena *vBeginTime, struct varlena *vEndTime,
int accessCnt, struct varlena *vTicketUser);
extern int
svrRemoveTicket (struct varlena *vTicket);
extern int
svrUnregisterDataset (struct varlena *vObjID, struct varlena *vCollectionName);
extern void closeAllDesc ();
extern int
svrContainerCreate (int catType, struct varlena *vContainerName,
struct varlena *vContainerType, struct varlena *vResourceName,
srb_long_t *containerSize);
extern int
svrContainerOpen (int catType, struct varlena *vContainerName,
int openFlag);
extern int
svrContainerClose (int descInx);
extern int 
svrRegisterContainer (int catType, struct varlena *vContainerName,
 struct varlena  *vLogResName, srb_long_t *containerSize);
extern int
svrRegisterInContDataset (int catType, struct varlena *vObjID,
struct varlena *vCollectionName, struct varlena *vContainerName,
struct varlena *vDataType, srb_long_t *dataSize, srb_long_t *baseOffset);
extern struct varlena *
svrGetContainerInfo (int catalog, struct varlena *vContainerName,
int rowsWanted);
extern struct varlena *
svrGetResOnChoice (int catType, struct varlena *vLogResName,
struct varlena *vInputFlag);
extern int
svrRmContainer (int catType, struct varlena *vContainerName, int deleteFlag);
extern int
_svrRmContainer (int catType, char *containerName, int deleteFlag);
extern int
__svrRmContainer (int catType, char *fullContName, int deleteFlag);
extern int
svrSyncContainer (int catType, struct varlena *vContainerName, int purgeFlag);
extern struct varlena *genIntRet (int status);
extern int
svrReplContainer (int catType, struct varlena *vContainerName,
struct varlena *vNewResourceName);
extern int
inContainerObjCreate (int catType, char *objID, char *collectionName,
char *containerName, char *dataType, srb_long_t dataSize);
extern int
inContObjCreateWithFd (int catType, char *objID, char *collectionName,
int contFd, char *dataType, srb_long_t dataSize);
extern char *
chkContColl (char *collectionName);
extern int
svrObjLock (struct varlena *vObjID, struct varlena *vCollectionName,
int lockCmd, int lockType, int lockFd);
extern int
_svrObjLock (char *objID, char *collectionName, int lockCmd, int lockType,
int lockFd);
extern int
_svrObjStat(int catType, char *path, int myType, struct srbStat *retStat);
extern struct varlena *
svrObjStat(int catType, struct varlena *vPath, int myType);
extern struct varlena *
svrObjGetdents (int catType, int fd, int nbyte);
extern struct varlena *
svrObjGetdents64 (int catType, int fd, int nbyte);
extern int
svrCollSeek (int fd,  int where, int whence, int is64Flag );
extern srb_long_t *
svrDataCopy (int destObjInx, struct varlena *vDestPath,
struct varlena *vDestResLoc, int srcObjInx,
struct varlena *vSrcPath, struct varlena *vSrcResLoc, srb_long_t *size, 
int zoneFlag);
extern srb_long_t
_svrDataCopy (int destObjInx, char *destPath, char *destResLoc,
int srcObjInx, char *srcPath, char *srcResLoc, srb_long_t size, int zoneFlag);
extern int lowLevelOpen (int objTypeInx, char *dataPath, 
char *resourceLoc, int oflag);
extern srb_long_t *
svrObjCopy (struct varlena *vSrcObjID, struct varlena *vSrcCollection,
struct varlena *vDestObjID, struct varlena *vDestCollection,
struct varlena *vDestResource);
extern srb_long_t
_svrObjCopy (char *srcObjID, char *srcCollection, char *destObjID,
char *destCollection, char *destResource);
extern srb_long_t *
svrObjPut (struct varlena *vDestObjID, struct varlena *vDestCollection,
struct varlena *vDestResource, struct varlena *vClHostAddr, int clPort,
struct varlena *vDataType, struct varlena *vDestPath, srb_long_t *size,
int forceFlag);
extern srb_long_t
_svrObjPut (char *myDestObjID, char *destCollection, char *destResource,
char *clHostAddr, int clPort, char *dataType, char *destPath, 
srb_long_t mySize, int forceFlag, int numThreads, svrComm_t *myComm);
extern srb_long_t
_svrDataPut (int destObjInx, char *destPath, char *destResLoc,
char *clHostAddr, int clPort, srb_long_t size,
int flag, int numThreads, svrComm_t *myComm);
extern int
svrBulkRegister (int catType, struct varlena *vContainer,
struct varlena *packedResult);
extern int
_svrBulkRegister (int catType, char *container,
mdasC_sql_result_struct *myresult, int noCheckFlag);
extern int
svrBulkLoad (int catType, struct varlena *vBloadFullPath,
struct varlena *packedResult);
extern int
_svrBulkLoad (int catType, mdasC_sql_result_struct *myresult,
struct mdasInfoOut *infoOutHead, struct mdasInfoInp *infoInp);
extern srb_long_t *
svrDataPut (int destObjInx, struct varlena *vDestPath,
struct varlena *vDestResLoc, struct varlena *vClHostAddr, 
int clPort, srb_long_t *size);
extern srb_long_t
srbToSrbCopy (int srcFd, int destFd);
extern srb_long_t
portalToSrbCopy (int srcFd, int destFd, srb_long_t mySize);
extern srb_long_t *
svrObjGet (struct varlena *vSrcObjID, struct varlena *vSrcCollection,
struct varlena *vClHostAddr, int clPort);
extern srb_long_t
_svrObjGet (char *srcObjID, char *srcCollection, char *clHostAddr, int clPort,
int flag, int numThreads, svrComm_t *myComm);
extern srb_long_t
srbToPortalCopy (int srcFd, int destFd);
extern int
writeToPortal (int destFd, char *buf, srb_long_t offset, int length);
extern int
sendDoneToPortal (int destFd);
extern srb_long_t
srbToPortalCopySB (int destFd, int srcExfInx, int srcTypeInx);
extern srb_long_t
srbToPortalCopyDB (int destFd, int srcExfInx, int srcTypeInx);
extern srb_long_t *
svrDataGet (int srcTypeInx, struct varlena *vSrcPath,
struct varlena *vSrcResLoc, struct varlena *vClHostAddr, int clPort,
srb_long_t *size);
extern srb_long_t
_svrDataGet (int srcTypeInx, char *srcPath, char *srcResLoc,
char *clHostAddr, int clPort, srb_long_t size, int flag, int numThreads,
svrComm_t *myComm);
extern int
readFromPortal (int srcFd, char *buf, srb_long_t offset, int length);
extern srb_long_t
portalToSrbCopyDB (int srcFd, int destExfInx, int destTypeInx, srb_long_t size);
extern srb_long_t
portalToSrbCopySB (int srcFd, int destExfInx, int destTypeInx, srb_long_t size);
extern srb_long_t
portalToSrbCopyP (int destTypeInx, char *destPath, char *destResLoc,
char *clHostAddr, int clPort, srb_long_t size, int flag, int numThreads,
svrComm_t *myComm);
extern srb_long_t
srbToPortalCopyP (int srcTypeInx, char *srcPath, char *srcResLoc,
char *clHostAddr, int clPort, srb_long_t size,
int flag, int numThreads, svrComm_t *myComm);
extern int 
_objClose (int objTypeInx, int lowLevelDescInx);
extern int
svrSyncData (int catType, struct varlena *vObjID,
struct varlena *vCollectionName, struct varlena *vResource);
extern int
_objCreate (int catType, char *objID, char *condInput, char *dataType,
char *logResName, char *collectionName, char *pathName, srb_long_t dataSize);
extern int lowLevelCreate (int tinx, char *resourceLoc, char *fullPathName, int mode, int chkDirFlag, srb_long_t dataSize);
extern int
chkDataPerm (int tinx, char *resourceLoc, char *pathName, int oFlag, 
int chdirFlag);
extern int
syncInfoToInfo (struct mdasInfoOut *inpSrcInfoOut, char *srcCollection,
struct mdasInfoOut *inpDestInfoOut, char *destCollection, int regDirty);
int 
getCompoundResInfo (char *mcatHostHint, int catType, char *cmpdResName, 
struct resCache **resInfo);
int
genCompPath (char *outPath, char *objID);
int
rmContentOfInfoOut (struct mdasInfoOut *infoOut, char *collectionName);
int
objCntIncontainer (char *fullContName);
int 
svrModifyRescInfo (int catType, struct varlena *vRescName, int retractionType,
struct varlena *vDataValue1, struct varlena *vDataValue2,
struct varlena *vDataValue3, struct varlena *vDataValue4);
int
_svrModifyRescInfo (int catType, char *mcatName, char *rescName, 
int retractionType,
char *dataValue1, char *dataValue2, char *dataValue3, char *dataValue4);
srb_long_t
_svrDataCopyFromInfoToResc (int catType, char *objID, char *collectionName,
struct mdasInfoOut *srcInfo, 
struct mdasResInfo *newResInfo, char *newPathName, char *newFullPathName, 
int *newObjInx);
srb_long_t
_svrDataCopyS (int destTypeInx, char *destPath, char *destResLoc,
int srcTypeInx, char *srcPath, char *srcResLoc, srb_long_t size);
srb_long_t
_svrDataCopyLocalToSrbP (int destTypeInx, char *destPath, char *destResLoc,
int srcTypeInx, char *srcPath, char *srcResLoc, srb_long_t size);
srb_long_t
_svrDataCopySrbToLocalP (int destTypeInx, char *destPath, char *destResLoc,
int srcTypeInx, char *srcPath, char *srcResLoc, srb_long_t size);
void *
svrPortalTransfer (struct transferInput *tranInp);

extern int svrRegisterLocation (struct varlena *vLocName,
				struct varlena *vFullAddr,
				struct varlena *vParentLoc,
				struct varlena *vServerUser,
				struct varlena *vServerUserDomain);
extern int _svrRegisterLocation (char *locName,
				char *fullAddr,
				char *parentLoc,
				char *serverUser,
				char *serverUserDomain);
extern int svrIngestToken (struct varlena *vTypeName,
			   struct varlena *vNewValue,
			   struct varlena *vParentValue);
extern int _svrIngestToken (char *typeName,
			   char *newValue,
			   char *parentValue);
extern int svrRegisterResource (struct varlena *vResName,
				struct varlena *vResType,
				struct varlena *vLocation,
				struct varlena *vPhyPath,
				struct varlena *vClass,
                                int size);
extern int _svrRegisterResource (char *resName,
				char *resType,
				char *location,
				char *phyPath,
				char *class,
                                int size);
extern int svrRegisterLogicalResource (struct varlena *vRescName,
				       struct varlena *vRescType,
				       struct varlena *vPhysResc,
				       struct varlena *vPhyPath);
extern int _svrRegisterLogicalResource (char *rescName,
				       char *rescType,
				       char *physResc,
				       char *phyPath);
extern int svrRegisterReplicateResourceInfo (
                    struct varlena *vPhysicalResName,
                    struct varlena *vResType,
                    struct varlena *vOldLogicalResType,
                    struct varlena *vInDefaultPath);
extern int _svrRegisterReplicateResourceInfo (
                    char *physicalResName,
                    char *resType,
                    char *oldLogicalResType,
                    char *inDefaultPath);
extern int svrDeleteValue(int valueType, struct varlena *vDeleteValue);
extern int _svrDeleteValue(int valueType, char *delValue);
extern int getResInfo (char *mcatHostHint, int catType, char *logResName, 
char *domainName, struct resCache **resCacheInfo);
#ifdef FED_MCAT
extern struct varlena *
svrGetMcatZone (struct varlena *vUserName, struct varlena *vDomainName);
extern int
_svrGetMcatZone (char *userName, char *domainName, char *mcatName);
#endif

extern struct varlena *svrSetupSessionPublicKey ();
extern int svrSetupSession(struct varlena *sessionKey);
extern int _svrSetupSession(char *sessionKey);
extern int svrBulkLoad (int catType, struct varlena *vBloadFullPath,
struct varlena *packedResult);
extern int
checkCommonPathPermissionFromResultStruct(int catType,
                          char *userName,
                          char *domainName,
                          char *rsrcName,
                          mdasC_sql_result_struct *inDataInfo);
extern int
getCommonPathFromSqlResultStruct(char *pathNameSet, int count,
                                 char **commonPath);
extern int svrModifyZone (int catType, 
			  struct varlena *vZoneName,
			  struct varlena *vDataValue1, 
			  struct varlena *vDataValue2,
			  struct varlena *vDataValue3, 
			  struct varlena *vDataValue4, 
			  struct varlena *vDataValue5, 
			  int actionType);
extern int _svrModifyZone (int catType, 
			   char *zoneName, 
			   char *dataValue1, 
			   char *dataValue2, 
			   char *dataValue3, 
			   char *dataValue4, 
			   char *dataValue5, 
			   int actionType);
extern struct varlena * svrBulkQueryAnswer (int catType, 
					    struct varlena *vQueryInfo,
					    int rowsWanted);

extern int _svrBulkQueryAnswer (int catType, 
			    char *queryInfo, 
			    mdasC_sql_result_struct **myresult,
			    int rowsWanted);
extern int svrBulkMcatIngest (int catType, struct varlena *vIngestInfo,
struct varlena *packedResult);
extern int
svrBulkUnload(int catType, int flag, struct varlena *vClHostAddr, int clPort,
struct varlena *vCollection, svrComm_t *myComm);
extern struct varlena *
svrBulkUnloadC(int catType, int flag, 
struct varlena *vCollection, svrComm_t *myComm);
extern int
getBulkUnloadHost (mdasC_sql_result_struct *myresult,
struct hostElement **myHostTabPtr, int flag);
int
svrBackupData (int catType, struct varlena *vObjID,
struct varlena *vCollectionName, struct varlena *vResource, int flag);
int
_objChksum (int objTypeInx, int lowLevelDescInx, int flags, char *chksum,
srb_long_t myOffset, srb_long_t mySize);
struct varlena *
svrObjChksum (struct varlena *vObjID, struct varlena *vCollectionName,
int chksumFlag, struct varlena * vInpChksum);
int
_svrObjChksum (char *objID, char *collectionName, int chksumFlag,
char *inpChksum, char *chksum);
int
_chksumOpenedObj (int descInx, char *chksum);
int
svrRemoveTicketWithZone (struct varlena *vMcatZone, struct varlena *vTicket);
extern int svrModifyUserNonPriv (int catType, 
			  struct varlena *vuserNameDomain,
			  struct varlena *vDataValue1, 
			  struct varlena *vDataValue2,
			  struct varlena *vDataValue3, 
			  struct varlena *vDataValue4, 
			  struct varlena *vDataValue5, 
			  int actionType);
extern int _svrModifyUserNonPriv (int catType, 
			   char *userNameDomain, 
			   char *dataValue1, 
			   char *dataValue2, 
			   char *dataValue3, 
			   char *dataValue4, 
			   char *dataValue5, 
			   int actionType);
extern int svrModifyResource (int catType, 
			  struct varlena *vresourceName,
			  struct varlena *vDataValue1, 
			  struct varlena *vDataValue2,
			  struct varlena *vDataValue3, 
			  struct varlena *vDataValue4, 
			  int actionType);
extern int _svrModifyResource (int catType, 
			   char *resourceName, 
			   char *dataValue1, 
			   char *dataValue2, 
			   char *dataValue3, 
			   char *dataValue4, 
			   int actionType);
extern 
int
setupSrvPortal (svrComm_t *myComm, int *mylport);
int
acceptSrvPortal (int lsock, int lport);
int
write1stHeaderToPortal (int destFd, srb_long_t objSize, int numThreads);
struct varlena *
svrObjGetC (struct varlena *vSrcObjID, struct varlena *vSrcCollection,
int flag, int numThreads, svrComm_t *myComm);
struct varlena *
svrDataGetC (int srcTypeInx, struct varlena *vSrcPath,
struct varlena *vSrcResLoc, srb_long_t *size, int flag, int numThreads,
svrComm_t *myComm);
struct varlena *
svrObjPutC (struct varlena *vDestObjID, struct varlena *vDestCollection,
struct varlena *vDestResource,
struct varlena *vDataType, struct varlena *vDestPath, srb_long_t *size,
int forceFlag, int numThreads, svrComm_t *myComm);
struct varlena *
svrDataPutC (int destTypeInx, struct varlena *vDestPath,
struct varlena *vDestResLoc, srb_long_t *size,
int flag, int numThreads, svrComm_t *myComm);
int
svrBulkMove (int catType, struct varlena *vCollectionName, int flag,
struct varlena *vSrcResource, struct varlena *vDestResource,
struct varlena *vContainerName);
int
_svrBulkMove (int catType, char *collectionName, int flag,
char *srcResource, struct mdasResInfo *destResInfo, char *destCollection);
int 
_svrBulkMoveToCont (int catType, char *collectionName, int flag,
char *srcResource, char *destResource, char *containerName);
extern int 
svrModifyForBulkMove(int catType, int operCode, struct varlena *vOperString,
		     struct varlena *inDataInfo);
extern int
_svrModifyForBulkMove(int catType, int operCode, char *operString,
                      mdasC_sql_result_struct *myresult);
int
svrBulkUnloadForMove(int catType, int flag, struct varlena *vClHostAddr,
int clPort, struct varlena *vCollection, struct varlena *vSrcResource,
struct varlena *vTargResource, svrComm_t *myComm);
int
_svrBulkUnloadForMove (int catType, int flag, char *clHostAddr,
int clPort, char *collection, char *srcResource, char *targResource,
svrComm_t *myComm);
int
queryBulkUnloadForMove (int catType, int flag, char *collection,
char *srcResource, char *targResource, mdasC_sql_result_struct **myresult);
int
setMultiRescQuery (char *qval, char *srcResource);
int
sendBmoveData (mdasC_sql_result_struct *myresult, char *clHostAddr, int clPort,
svrComm_t *myComm, int flag, int myFd);
int
unfoldBulkMoveData (int srcFd, struct mdasResInfo *destResInfo,
mdasC_sql_result_struct *inresult, int flag,
char *srcCollection, char *destCollection);
int
setupResultForMove (mdasC_sql_result_struct *inresult,
mdasC_sql_result_struct *outresult, char *container);
int
setupResultForCopy (mdasC_sql_result_struct *inresult,
mdasC_sql_result_struct *outresult, char *container);
int
localCreateForMove (char *inCollection, char *inObjID, char *inReplNum,
char *inVerNum, struct mdasResInfo *destResInfo, int destTypeInx,
char *outCollection, char *outObjID, char *outReplNum, char *outVerNum,
char *outPath, int flag, char *srcCollection, char *destCollection);
int
cleanMovedData (struct mdasResInfo *destResInfo,
mdasC_sql_result_struct *outresult);
int
getFirstResc (char *srcResource, char *outResName);
int
sendDelToPortal (int destFd, mdasC_sql_result_struct *myresult);
int printOrphan (mdasC_sql_result_struct *myresult);
int
readDelHeader (int myFd, mdasC_sql_result_struct *savedResult);
int
delMovedSrcData (mdasC_sql_result_struct *myresult);
int
getBestSrcForBulkMove (int catType, char *collectionName, int flag,
char *srcResource, char *destResource,
mdasC_sql_result_struct **myresult, struct hostElement **tmpHostTabPtr);
int 
localBulkMoveDataToCont (int catType, char *collectionName, 
char *containerName, mdasC_sql_result_struct *myresult);
int
_svrContainerOpen (int catType, char *containerName, int openFlag);
srb_long_t 
dataCopyToCont (int catType, int srcTypeInx, char *inPath,
char *inResourceLoc, srb_long_t size, int contInx, srb_long_t contOffset, 
char *buf, int *bufOffset);
int
procDescDirtyFlag (int descInx);
int
_svrRegNewObjSize (int catType, char *objID, char *collection,
char *resourceName, char *pathName, srb_long_t newSize);
int
setWriteFlag (int descInx, int writeFlag);
int
svrRegPhyMove (int catType, struct varlena *vObjID,
struct varlena *vCollectionName,
struct varlena *vCurResourceName, struct varlena *vCurDataPath,
struct varlena *vNewResourceName, struct varlena *vNewFullPathName);
int
_svrRegPhyMove (int catType, char *objID,  char *collectionName,
char *curResourceName, char *curDataPath, char *newResourceName,
char *newFullPathName);
int
localBulkMoveData (int catType, char *collection,
struct mdasResInfo *destResInfo, mdasC_sql_result_struct *inresult, int flag,
char *destCollection);
int
addZoneToResult (mdasC_sql_result_struct *myresult, char *mcatHint);
int
svrBulkCopy (int catType, struct varlena *vCollectionName, int flag,
struct varlena *vDestResource, struct varlena *vDestCollection);
int
svrQueryContainerByColl (char *collection, mdasC_sql_result_struct **myresult);
int
bulkCopyFromCont (int catType, char *collectionName,
char *destCollection, struct mdasResInfo *destResInfo, char *container,
char *contReplTime, char *contCreateTime, int flag);
int
genIncontListinColl (int catType, char *collection,
char *container, int flag, mdasC_sql_result_struct **myresult);
int setWhereToStartBulkOpr (int catType, int flag,
char *destResource, char *collection, struct mdasResInfo **outDestResInfo,
struct hostElement  **outHostTabPtr);
int
procMovedResult (mdasC_sql_result_struct **savedResult, int remoteFd,
int flag);
#endif	/* SRB_OBJ_FUNCT_EXTERN_H */
