/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/* objMcatOprFunctExtern.h
 * Definition and prototype for  objMcatOprFunct.c.
 */

#ifndef OBJ_MCAT_OPR_FUNCT_EXTERN_H
#define OBJ_MCAT_OPR_FUNCT_EXTERN_H

/* definition for _svrGenRegisterDataset  
 * regMode - NEW_FIRST_COPY - a new first copy
 *           NEW_REPLICA -  A new replica.
 *           EXISTING_COPY - An existing copy
 */
#define NEW_FIRST_COPY 1
#define NEW_REPLICA 2
#define EXISTING_COPY -1

extern struct varlena * svrGetDatasetInfo (int catType, struct varlena *vObjID,
struct varlena *vCollectionName, int rowsWanted, struct varlena *mode);
struct varlena *sqlResultToVar (mdasC_sql_result_struct *myresult, int rowFlag);
int mdasGetInfo (struct mdasInfoInp *infoInp,
  struct mdasInfoOut **infoOutHead);
int resClassLookup (char *resClassKeyWd);
int domainLookUp (char *domainName);
char *openFlagLookUp (int openFlag);
int parseCondInput (char *condInput, int keywordInx,
struct condResult *condResult);
struct inputValue *matchCondInpValue (struct condInputMap
*condInputMap, char *valueStr);
extern struct varlena *svrGetDataDirInfo (int cat_type, struct varlena *vQval,
struct varlena *vSelVal, int rowsWanted);
extern int myResAttrLookup(char *mcatHostHint, int catType, char *resourceName,
                       char *domainName, char **resourceType,
                       char **resourceLoc);
int
getCompoundResInfo (char *mcatHostHint, int catType, char *cmpdResName,
struct resCache **resInfo);
extern int getResInfo (char *mcatHostHint, int catType, char *logResName,
char *domainName, struct resCache **resCacheInfo);
struct mdasResInfo *getResInfoByClass (struct mdasResInfo *resInfo,
char *phyResClass);
extern int svrRegisterDataset (int catType, struct varlena *vObjID,
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
extern int
_svrRegisterDataset (int catType, char *objID, char *userName,
char *access, char *domainName, char *dataTypeName, char *pathName,
char *resourceName, char *collectionName,
srb_long_t dataSize, int rmObjTypeInx, char *rmOhyResLoc);
extern int
chkDataPerm (int tinx, char *resourceLoc, char *pathName, int oFlag,
int chdirFlag);
extern int
svrUnregisterDataset (struct varlena *vObjID, struct varlena *vCollectionName);
extern int svrRegisterReplica (int catType, struct varlena *vObjID,
struct varlena *vCollectionName,
struct varlena *vOrigResourceName, struct varlena *vOrigPathName,
struct varlena *vNewResourceName, struct varlena *vNewPathName,
struct varlena *vUserName, struct varlena *vDomainName);
extern int
_svrRegisterReplica (int catType, char *objID, char *collectionName,
char *origResourceName, char *origPathName,
char *newResourceName, char *newPathName,
int rmObjFlag, int objTypeInx, char *rmOhyResLoc,
char *userName, char *domainName);
extern int svrModifyDataset (int catType, struct varlena *vObjID,
struct varlena *vCollectionName,
struct varlena *vResourceName, struct varlena *vPathName,
struct varlena *vDataValue1, struct varlena *vDataValue2,
int retractionType);
int _svrModifyDataset (int catType, char *objID, char *collectionName,
char *resourceName, char *pathName, char *dataValue1, char *dataValue2,
int retractionType);
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
struct mdasInfoOut *
selectInfoOutByResc (char *resourceName, struct mdasInfoOut **infoOutHead);
int
matchRescWithContResult (char *resourceName, mdasC_sql_result_struct *myresult);extern char *
chkContColl (char *collectionName);
int
resolveInfoPathName (char *inpObjID, char *collectionName,
struct mdasInfoOut *infoOutHead);
int
matchRescWithInfo (struct mdasResInfo *resInfo,
struct mdasInfoOut *infoOutHead);
int
objCntIncontainer (char *fullContName);
extern int
isResLocLocal (char *resourceLoc);
extern int
checkCommonPathPermissionFromResultStruct(int catType,
                          char *userName,
                          char *domainName,
                          char *rsrcName,
                          mdasC_sql_result_struct *inDataInfo);
extern int
getCommonPathFromSqlResultStruct(char *pathNameSet, int count,
                                 char **commonPath);
int
checkPermInPath(int catType,
                char *userName,
                char *domainName,
                char *rsrcName,
                char *pathName);
int fillInfoOut (struct mdasInfoOut *infoOut, char *objID, char *resourceType,
char *dataPath, char *resourceLoc, char *resourceName,
char *dataType, char *replNum, char *offset, char *size, char *phyResClass,
char *containerName, char *isDirty, char *chksum, char *collection);
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
extern int
parseCondResInfo (char *mcatHostHint, int catType, char *condInput,
char *logResName, struct condResult *condResult,
struct resCache **myCacheRes, srb_long_t dataSize);
extern struct varlena *
svrGetMoreRows (int catalog, int contDesc, int rowsWanted);
extern struct varlena *
svrGenGetMoreRows (int catalog, int contDesc, int rowsWanted);
#ifdef FED_MCAT
extern struct varlena *
svrGetMcatZone (struct varlena *vUserName, struct varlena *vDomainName);
extern int
_svrGetMcatZone (char *userName, char *domainName, char *mcatName);
#endif
extern struct varlena *
svrGenQuery (int cat_type, struct varlena *vMyMcatZone,
struct varlena *vMyGenQuery, int rowsWanted);
int
_svrGenRegisterDataset (int catType, char *destObjID, char *destCollection,
char *userName, char *domainName,
int regMode, struct mdasResInfo *destResInfo, struct mdasResInfo *compResInfo,
char *fullPathName, srb_long_t size, 
char *dataType, int rmObjTypeInx,
char *srcResource, char *srcDataPath, srb_long_t srcSize,
char *compPathName, int *intReplNum, int *lockFd);
int
_svrRegNewObjSize (int catType, char *objID, char *collection, 
char *resourceName, char *pathName, srb_long_t newSize);
int
_svrRegNewIntnernalCompsize (int catType, char *objID, char *collection,
int replNum, int objSegNum, int intReplNum, int intSegNum, 
srb_long_t newSize);
extern int svrModifyExtMetaData (int catType,  struct varlena *vDataName,
                      struct varlena *vCollName, struct varlena *vDataValue1,
                      struct varlena *vDataValue2, struct varlena *vDataValue3,
                      struct varlena *vDataValue4, struct varlena *vDataValue5,
                      int retractionType);
struct varlena *
svrGetUserByDn (int catType, int flag, struct varlena *vUserDn);
struct mdasResInfo *
getCompResInfoForLoad (struct mdasResInfo *resInfo);
#endif	/* OBJ_MCAT_OPR_FUNCT_EXTERN_H */

