/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* objParaIoFunctExtern.h
 * Definition and prototype for objParaIoFunctExtern.c.
 */
 
#ifndef OBJ_PARA_IO_FUNCT_EXTERN_H
#define OBJ_PARA_IO_FUNCT_EXTERN_H

#ifdef SRB_MDAS
#include "mdasGlobalsExtern.h"
#endif
 
extern srb_long_t *
svrDataCopy (int destObjInx, struct varlena *vDestPath,
struct varlena *vDestResLoc, int srcObjInx,
struct varlena *vSrcPath, struct varlena *vSrcResLoc, srb_long_t *size, 
int zoneFlag);
extern srb_long_t
_svrDataCopy (int destObjInx, char *destPath, char *destResLoc,
int srcObjInx, char *srcPath, char *srcResLoc, srb_long_t size, int zoneFlag);
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
extern srb_long_t *
svrDataPut (int destObjInx, struct varlena *vDestPath,
struct varlena *vDestResLoc, struct varlena *vClHostAddr, 
int clPort, srb_long_t *size);
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
int
sendStatusToPortal (int destFd, int inStatus);
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
extern int svrBulkLoad (int catType, struct varlena *vBloadFullPath,
struct varlena *packedResult);
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
int
sendPortList (svrComm_t *myComm, portList_t *thisPortList);
extern struct varlena *genIntRet (int status);
srb_long_t
_svrDataPutToRescOrCompResc (int catType,
char *destObjID, char *destCollection, struct mdasResInfo *destResInfo,
char *clHostAddr, int clPort, srb_long_t mySize, int flag, int numThreads,
svrComm_t *myComm, char *inpPathName,
char *outFullPathName, int *objTypeInx, int *lockFd,
struct mdasResInfo **outCompResInfo);
srb_long_t
_svrDataPutOneThr (int destFd, char *clHostAddr, int clPort,
srb_long_t mySize, int flag, svrComm_t *myComm);
srb_long_t
_svrDataPutToRescAndReg (int catType,
char *destObjID, char *destCollection, struct mdasResInfo *destResInfo,
char *clHostAddr, int clPort, srb_long_t mySize, int flag, int numThreads,
svrComm_t *myComm, char *inpPathName,
char *dataType, int regMode, srb_long_t registerdSize, 
int chksumFlag, int iChksum, char *fileGroup,
char *outFullPathName, int *objTypeInx, int *lockFd,
struct mdasResInfo **outCompResInfo);
#endif	/* OBJ_PARA_IO_FUNCT_EXTERN_H */
