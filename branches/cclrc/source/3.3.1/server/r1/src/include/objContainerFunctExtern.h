/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/* objContainerFunctExtern.h
 * Definition and prototype for objContainerFunct.c.
 */

#ifndef OBJ_CONTAINER_FUNCT_EXTERN_H
#define OBJ_CONTAINER_FUNCT_EXTERN_H

extern int
inContainerObjCreate (int catType, char *objID, char *collectionName,
char *containerName, char *dataType, srb_long_t dataSize);
extern int
inContObjCreateWithFd (int catType, char *objID, char *collectionName,
int contFd, char *dataType, srb_long_t dataSize);
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
extern int
svrReplContainer (int catType, struct varlena *vContainerName,
struct varlena *vNewResourceName);
int
_svrReplContainer (int catType, char *containerName, char *newResourceName);
extern int
svrContainerCreate (int catType, struct varlena *vContainerName,
struct varlena *vContainerType, struct varlena *vResourceName,
srb_long_t *containerSize);
int
_svrContainerCreate (int catType, char *inpContainerName, char *containerType,
char *logResName, srb_long_t containerSize);
extern int
svrContainerOpen (int catType, struct varlena *vContainerName,
int openFlag);
int
_svrContainerOpen (int catType, char *containerName, int openFlag);
extern int
svrContainerClose (int descInx);
int inContainerObjOpen (char *objID, char *collectionName, int openFlag,
struct mdasInfoOut *infoOutHead);
extern int
svrRmContainer (int catType, struct varlena *vContainerName, int deleteFlag);
extern int
_svrRmContainer (int catType, char *containerName, int deleteFlag);
extern int
__svrRmContainer (int catType, char *fullContName, int deleteFlag);
extern int
svrSyncContainer (int catType, struct varlena *vContainerName, int purgeFlag);
extern struct varlena *genIntRet (int status);
int
_svrSyncContainer (int catType, char *containerName, int syncFlag);
int
_rmContainerCopy (int catType, char *containerName, char *contCollection,
struct mdasInfoOut *tmpInfoOut);

#endif	/* OBJ_CONTAINER_FUNCT_EXTERN_H */
