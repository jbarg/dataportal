/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/* objSeqIoFunctExtern.h
 * Definition and prototype for objSeqIoFunct.c.
 */

#ifndef OBJ_SEQ_IO_FUNCT_EXTERN_H
#define OBJ_SEQ_IO_FUNCT_EXTERN_H
extern int svrObjCreate (int catType, struct varlena *vObjID,
struct varlena *vDataTypeName, struct varlena *vResourceName,
struct varlena *vCollectionName, struct varlena *vPathName,
srb_long_t *dataSize);
extern int
_svrObjCreate (int catType, char *objID, char *dataType, char *logResName,
     char *collectionName, char *pathName, srb_long_t dataSize);
extern int
_objCreate (int catType, char *objID, char *condInput, char *dataType,
char *logResName, char *collectionName, char *pathName, srb_long_t dataSize);
extern int lowLevelCreate (int tinx, char *resourceLoc, char *fullPathName, int
mode, int chkDirFlag, srb_long_t dataSize);
int
_objCreateWithResInfo (char *objID, char *collectionName, char *pathName,
srb_long_t dataSize, struct mdasResInfo *resInfo, char *fullPathName,
int *objTypeInx);
extern int svrObjOpen (struct varlena *vDataID,
int openFlag, struct varlena *vCollectionName);
int
_svrObjOpen (char *objID, int openFlag, char *collectionName);
int inContainerObjOpen (char *objID, char *collectionName, int openFlag,
struct mdasInfoOut *infoOutHead);
extern int
_svrObjOpenWithInfo (struct mdasInfoInp *infoInp,
struct mdasInfoOut **infoOutHead, int openFlag);
int _objOpen (char *inpObjID, char *collectionName, int openFlag,
struct mdasInfoOut *infoOutHead);
extern int svrObjClose (int descInx);
extern int _svrObjClose (int descInx);
extern int
_objClose (int objTypeInx, int lowLevelDescInx);
int
_objChksum (int objTypeInx, int lowLevelDescInx, int flags, char *chksum,
srb_long_t myOffset, srb_long_t mySize);
int
_objRead(int lowLevelDescInx, int objTypeInx, char *buf, int len);
int
_objWrite(int lowLevelDescInx, int objTypeInx, char *buf, int len);
srb_long_t
_objSeek (int lowLevelDescInx, int objTypeInx, srb_long_t offset, int whence);
extern int svrObjUnlink(struct varlena *vDataID,
struct varlena *vCollectionName);
int _svrObjUnlink (char *objID, char *collectionName);
int _objUnlink (int objType, int systemType, char *resLoc,
char *fullPathName, int chkDirFlag);
extern int lowLevelOpen (int objTypeInx, char *dataPath,
char *resourceLoc, int oflag);
extern struct varlena *svrObjRead (int descInx, int len);
int _svrObjRead (int descInx, char *buf, int len);
extern int svrObjWrite (int descInx, struct varlena *wbuf);
int
_svrObjWrite (int descInx, char *buf, int bytestowrite);
extern srb_long_t *svrObjSeek (int descInx, srb_long_t *offset, int whence);
extern srb_long_t _svrObjSeek (int descInx, srb_long_t offset, int whence);
int
svrCollSeek (int fd, int where, int whence, int is64Flag );
extern int svrObjSync (int descInx);
extern struct varlena *
svrObjStat(int catType, struct varlena *vPath, int myType);
extern int
_svrObjStat(int catType, char *path, int myType, struct srbStat *retStat);
extern int svrObjReplicate (int catType, struct varlena *vObjID,
struct varlena *vCollectionName,
struct varlena *vNewResourceName, struct varlena *vNewPathName);
int
_svrObjReplicate (int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName);
int
_objReplicate (int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName, struct mdasInfoOut *origInfoOut,
struct mdasInfoOut **newInfoOut, struct mdasResInfo *inpResInfo);
extern int svrObjMove (int catType, struct varlena *vObjID,
struct varlena *vCollectionName,
struct varlena *vSrcResource,
struct varlena *vNewResourceName, struct varlena *vNewPathName,
struct varlena *vContainerName);
extern srb_long_t *
svrObjCopy (struct varlena *vSrcObjID, struct varlena *vSrcCollection,
struct varlena *vDestObjID, struct varlena *vDestCollection,
struct varlena *vDestResource);
extern srb_long_t
_svrObjCopy (char *srcObjID, char *srcCollection, char *destObjID,
char *destCollection, char *destResource);
extern srb_long_t
srbToSrbCopy (int srcFd, int destFd);
extern int
svrSyncData (int catType, struct varlena *vObjID,
struct varlena *vCollectionName, struct varlena *vResource);
int
_svrSyncData (int catType, char *objID, char *collectionName, char *resource);
int
svrBackupData (int catType, struct varlena *vObjID,
struct varlena *vCollectionName, struct varlena *vResource, int flag);
int
_svrBackupData (int catType, char *objID, char *collectionName, char *resource,
int flag);
int
_svrBackupDataWithInfo (int catType, char *objID, char *collectionName,
int flag, struct mdasInfoOut *infoOutHead, struct mdasResInfo *newResInfo,
struct condResult condResult);
int
_objUpdate (int catType, char *objID, char *collection,
struct mdasInfoOut *dirtyInfoOut, struct mdasInfoOut *cleanInfoOut);
int
syncInfoToInfo (struct mdasInfoOut *inpSrcInfoOut, char *srcCollection,
struct mdasInfoOut *inpDestInfoOut, char *destCollection, int regDirty);
struct varlena *
svrObjChksum (struct varlena *vObjID, struct varlena *vCollectionName,
int chksumFlag, struct varlena * vInpChksum);
int
_svrObjChksum (char *objID, char *collectionName, int chksumFlag,
char *inpChksum, char *chksum);
int
_chksumOpenedObj (int descInx, char *chksum);
extern int
lowLevelClose (int objTypeInx, int lowLevelDescInx);
int
_objChksumAndReg (int objTypeInx, int flags, char *chksum,
srb_long_t myOffset, srb_long_t mySize, char *destObjID,
char *destCollection, char *myPathName, char *phyResLoc,
int chksumFlag, unsigned long iChksum, char *regResName, char *regPathName,
int inpLowLevelDescInx);
int
_objCreateWithResOrCompInfo (char *objID, char *collectionName,
char *inpPathName, srb_long_t dataSize, struct mdasResInfo *resInfo,
char *outFullPathName, int *objTypeInx, int *lockFd,
struct mdasResInfo **outCompResInfo);
srb_long_t 
_svrDataCopyFromInfoToRescOrCompResc (int catType,
char *destObjID, char *destCollection, struct mdasResInfo *destResInfo,
struct mdasInfoOut *srcInfoOut, char *inpPathName,
char *outFullPathName, int *objTypeInx, int *lockFd,
struct mdasResInfo **outCompResInfo);
int
genLowLevelOpen (struct mdasInfoOut *inpInfoOut, char *collectionName,
int openFlag, int *lockFd, int stageFlag, struct mdasInfoOut **outCacheInfoOut);
int
_svrObjMove (int catType, char *objID, char *collectionName,
char *srcResource, char *newResourceName, char *newPathName,
char *container);
int
_svrObjChkPhySize (char *objID, char *collectionName, int chksumFlag,
char *outSize);
#endif	/* OBJ_SEQ_IO_FUNCT_EXTERN_H */
