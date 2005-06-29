/* compoundObjExtern.h */

#ifndef COMPOUND_OBJ_EXTERN_H
#define COMPOUND_OBJ_EXTERN_H

#include "packStruct.h"
/* #include "packDef.h" */
#include "packDefExtern.h"

#define TAPE_LOCK_SLEEP_TIME	30	/* 30 sec */
int
svrRegInternalCompObj (struct varlena *vObjName, struct varlena *vObjCollName, 
int   objReplNum, int objSegNum, 
struct varlena *vIntObjRescName, struct varlena *vDataPathName, 
srb_long_t *dataSize, srb_long_t *offset, 
int inpIntReplNum, int intSegNum,
int objTypeInx, struct varlena *vPhyResLoc);
int
_svrRegInternalCompObj (char *objName, char *objCollName, int   objReplNum,
int   objSegNum, char *intObjRescName, char *dataPathName, srb_long_t dataSize,
srb_long_t offset, int intReplNum, int intSegNum,
int objTypeInx, char *phyResLoc,
char *userName, char *domainName);
int
_svrStageCompObj (struct mdasInfoOut *compInfoOut, char *collectionName,
struct mdasInfoOut **cacheInfoOut, int stageFlag);
int
rmAllIntTapeCompObj (struct mdasInfoOut *tapeInfoOut, char *collectionName,
char *tapeUser, char *tapeDomain);
int
rmIntNonTapeCompObj (struct mdasInfoOut *cacheInfoOut, char *collectionName,
char *tapeUser, char *tapeDomain);
struct varlena *
svrStageCompObj (struct varlena *vObjID, struct varlena *vCollectionName,
struct varlena *vResourceLoc, struct varlena *vResourceName,
srb_long_t *offset, srb_long_t *dataSize, int replNum, int segNum, 
int stageFlag);
struct mdasInfoOut *
resolveCompInfo (struct mdasInfoOut *infoOut, char *collection, int stageFlag);
int
_svrDumpFileList (int tapeLibInx, fileList_t *fileList, int purgeFlag);
#ifdef TAPE_DRIVE
int
_svrDumpFileToTape (srbConn **conn, int inFd, char *fileName, char *collection, 
struct mdasResInfo *tapeResInfo,
struct mdasInfoOut *cacheInfoOut, tapeLibConfig_t *tapeLibConfig,
tapeDev_t *tapeDev, tapeStat_t *tapeStat, int lastFileFlag,
char *tapeUser, char *tapeDomain);
int
procCloseTapeDev (tapeDev_t *tapeDev, srbConn *conn);
int
genTapePathName (char *pathName, char *tapeNum, int fileSeqNo);
int
_svrGetTapeToWrite (srbConn *conn, int tapeLibInx, tapeStat_t *tapeStat);
int 
freeAllTapeStat (tapeStat_t *topTapeStat);
int
regTapeStat (tapeStat_t *tapeStat, int relTapeFlag,
int fullFlag);
#endif	/* TAPE_DRIVE */
int
svrDumpFileList (int tapeLibInx, struct varlena *packedResult, int purgeFlag);
int
_svrGetCompInfoByFilePath (char *fileName,
struct mdasInfoOut **cacheInfoOut, struct mdasInfoOut **tapeInfoOut,
char *compResc, char *compRescLoc, char *myCollection,
char *tapeUser, char *tapeDomain);
int _svrGetInternalObjInfo (char *objName, char *objCollName,
int   objReplNum, int   objSegNum,
struct mdasInfoOut **cacheInfoOut, struct mdasInfoOut **tapeInfoOut);
int
procTapeInfo (struct mdasInfoOut **tapeInfoOut);
struct mdasInfoOut *
matchIntReplNum (struct mdasInfoOut *infoOutHead, 
struct mdasInfoOut *myInfoOut);
#ifdef TAPE_DRIVE
int
queTapeFileInfo (struct mdasInfoOut *myInfoOut, tapeFileInfo_t *myTapeFileInfo);
int
queTapeInfo (struct mdasInfoOut **tapeInfoHead, struct mdasInfoOut *myInfoOut);
int
genTapeFileInfo (struct mdasInfoOut *myInfoOut, 
tapeFileInfo_t **outTapeFileInfo);
int
parseTapePathName (char *pathName, char *tapeNum, int *fileSeqNo);
#endif	/* TAPE_DRIVE */
int
svrRmInternalCompObj (struct varlena *vObjID, struct varlena *vCollectionName,
int replNum, int segNum, int intReplNum, int intSegNum);
int
_svrRmInternalCompObj (char *objID, char *collectionName,
int replNum, int segNum, int intReplNum, int intSegNum,
char *userName, char *domainName);
int
svrRmCompObj (struct varlena *vObjID, struct varlena *vCollectionName,
int replNum, int segNum);
int
_svrRmCompObj (char *objID, char *collectionName,
int replNum, int segNum,
char *userName, char *domainName);
int
svrModInternalCompObj (struct varlena *vObjName, struct varlena *vObjCollName,
int   objReplNum, int objSegNum,
int inpIntReplNum, int intSegNum,
struct varlena *vData_value_1, struct varlena *vData_value_2,
struct varlena *vData_value_3, struct varlena *vData_value_4,
int retraction_type);
int
_svrModInternalCompObj (char *objName, char *objCollName,
int   objReplNum, int   objSegNum,
int   intReplNum, int   intSegNum,
char *userName, char *domainName,
char *data_value_1, char *data_value_2,
char *data_value_3, char *data_value_4, int retraction_type);
#ifdef TAPE_DRIVE
srb_long_t
_svrStageFromTape (srbConn *conn,
int exfInx, int cacheObjTypeInx,
struct mdasInfoOut *tapeInfoOut, tapeDev_t *tapeDev);
#endif	/* TAPE_DRIVE */

#endif	/* COMPOUND_OBJ_EXTERN_H */

