/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/* objBulkFunctExtern.h
 * Definition and prototype for objBulkFunct.c.
 */

#ifndef OBJ_BULK_FUNCT_EXTERN_H
#define OBJ_BULK_FUNCT_EXTERN_H

#define BUNLOAD_NUM_ROWS 5000	/* for BBSRC */

#define BULK_MOVE_NUM_ROWS      200

#define MAX_BMOVE_SRC_RESC      30      /* max srcResource for bmove
                                         * that can be handled */

extern int
svrBulkLoad (int catType, struct varlena *vBloadFullPath,
struct varlena *packedResult);
extern int
_svrBulkLoad (int catType, mdasC_sql_result_struct *myresult,
struct mdasInfoOut *infoOutHead, struct mdasInfoInp *infoInp);
extern int
svrBulkUnload(int catType, int flag, struct varlena *vClHostAddr, int clPort,
struct varlena *vCollection, svrComm_t *myComm);
extern int
svrBulkUnload(int catType, int flag, struct varlena *vClHostAddr, int clPort,
struct varlena *vCollection, svrComm_t *myComm);
extern struct varlena *
svrBulkUnloadC(int catType, int flag,
struct varlena *vCollection, svrComm_t *myComm);
extern int
_svrBulkUnload (int catType, int flag, char *clHostAddr,
int clPort, char *collection, svrComm_t *myComm, portList_t **myPortList);
int
sendBunloadData (struct bunloadSort *bunloadHead, char *clHostAddr, int clPort,
svrComm_t *myComm, int flag, int myFd);
extern int
svrBulkRegister (int catType, struct varlena *vContainer,
struct varlena *packedResult, int noCheckFlag);
extern int
_svrBulkRegister (int catType, char *container,
mdasC_sql_result_struct *myresult, int noCheckFlag);

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
getBulkUnloadHost (mdasC_sql_result_struct *myresult,
struct hostElement **myHostTabPtr, int flag);
int
sortResultForBunload (struct bunloadSort *tmpBunloadSort);
int
pickCopyToUnload (int numCopies, int startInx, int endInx,
struct bunloadSort *tmpBunloadSort);
void
closeUnloadSock (int sock, int flag);
srb_long_t
portalToPortalRelay (int srcFd, int destFd);
int
sendStatusToClient (int status, int retlen, svrComm_t *myComm);
int
setupSockForBunload (char *clHostAddr, int clPort, svrComm_t *myComm, int flag);
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
srb_long_t
dataCopyToCont (int catType, int srcTypeInx, char *inPath,
char *inResourceLoc, srb_long_t size, int contInx, srb_long_t contOffset,
char *buf, int *bufOffset);
int
procDescDirtyFlag (int descInx);
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
int
svrObjMoveColl (int catType, struct varlena *vCollectionName, int flag,
struct varlena *vSrcResource, struct varlena *vDestResource,
struct varlena *vContainerName);
int
_svrObjMoveColl (int catType, char *collectionName, int flag, 
char *srcResource, char *destResource, char *containerName);
int
stageAndFileResult (struct mdasInfoOut *infoOut,
char *resourceType, char *dataPath, char *resourceLoc, char *resourceName,
char *phyResClass);
int procBMoveResForComp (mdasC_sql_result_struct *myresult);
#endif	/* OBJ_BULK_FUNCT_EXTERN_H */
