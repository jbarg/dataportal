/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/* objMcatAdminFunctExtern.h
 * Definition and prototype for objMcatAdminFunct.c.
 */

#ifndef OBJ_MCAT_ADMIN_FUNCT_EXTERN_H
#define OBJ_MCAT_ADMIN_FUNCT_EXTERN_H

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
int _svrObjAudit (int catType, char *userName, char *objID,
char *collectionName,  char *dataPath,  char *resourceName,
char *accessMode,  char *comment, int success,  char *domainName);
extern struct varlena *svrGetPrivUsers (int catalog, int rowsWanted);
int
svrModifyRescInfo (int catType, struct varlena *vRescName, int retractionType,
struct varlena *vDataValue1, struct varlena *vDataValue2,
struct varlena *vDataValue3, struct varlena *vDataValue4);
int
_svrModifyRescInfo (int catType, char *mcatName, char *rescName,
int retractionType,
char *dataValue1, char *dataValue2, char *dataValue3, char *dataValue4);
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

#endif	/* OBJ_MCAT_ADMIN_FUNCT_EXTERN_H */
