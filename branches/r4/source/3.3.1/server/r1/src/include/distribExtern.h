/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* distribExtern.h - The header file for distrib.c
 */

#ifndef DISTRIB_EXTERN_H
#define DISTRIB_EXTERN_H

#ifdef PORTNAME_solaris
#include <strings.h>
#endif

#include "clConnectExtern.h"
#include "srbStat.h"
#include "commExtern.h"

#define LOCAL_HOST	0	/* A local host */
#define REMOTE_HOST	1	/* A remote host */

extern int remoteFileOpen (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int flags, int mode, int chdirFlag, char *hostAddrBuf);
extern int
remoteFileOpenUni (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int flags, int mode, int chdirFlag, srbConn **conn);
extern int remoteFileCreate (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int mode, int chdirFlag, srb_long_t size, char *hostAddr);
extern int
remoteFileCreateUni (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int mode, int chdirFlag, srb_long_t size, srbConn **conn);
extern int remoteOpendir (int storSysType, struct hostElement  *hostTabPtr,
char *dirName, int chdirFlag);
extern int remoteFileRmdir (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int chdirFlag, char *hostAddr);
extern int remoteFileChmod (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int mode, int chdirFlag);
extern int remoteFileUnlink (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int chdirFlag, char *hostAddrBuf);
extern int remoteFileStat (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, struct srbStat *retStat, char *hostAddrBuf);
extern int remoteFileFstat (int storSysType, struct hostElement  *hostTabPtr,
int fd_inx, struct srbStat *retStat);
extern int
remoteFileMkdir (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int mode, int chdirFlag, char *hostAddr);
extern int remoteDbLobjCreate (int storSysType, struct hostElement *hostTabPtr,
char *resourceLoc, char *dataPath, int mode);
extern int remoteDbLobjOpen (int storSysType, struct hostElement  *hostTabPtr,
char *resourceLoc, char *dataPath, int flags, int mode);
extern int remoteDbLobjUnlink (int storSysType, struct hostElement  *hostTabPtr,
char *resourceLoc, char *dataPath);
extern int remoteDbTableCreate (int storSysType, struct hostElement *hostTabPtr,
char *resourceLoc, char *dataPath, int mode);
extern int remoteDbTableOpen (int storSysType, struct hostElement  *hostTabPtr,
char *resourceLoc, char *dataPath, int flags, int mode);
extern int remoteDbTableUnlink (int storSysType, struct hostElement *hostTabPtr,
char *resourceLoc, char *dataPath);
/* extern struct hostElement  *getMdasEnabledHost(); */
extern struct varlena *svrGetHostConfig ();
extern int remoteRegisterDataset (int catType, char *objID, char *userName,
char *access, char *domainName, char *dataType, 
char *pathName,   char *resourceName, char *collectionName,  
srb_long_t dataSize, int rmObjTypeInx, char *rmObjResLoc,
struct hostElement  *hostTabPtr);
extern int remoteRegisterContainer (int catType, char *containerName, 
char *userName, char *domainName, char *logResName, srb_long_t containerSize,
struct hostElement  *hostTabPtr);
extern int remoteRegisterInContDataset (int catType, char *objID, 
char *collectionName, char *userName, char *domainName, char *containerName,
char *dataType, srb_long_t dataSize, srb_long_t baseOffset,
struct hostElement  *hostTabPtr);
extern int remoteModifyDataset (int catType, char *objID, 
char *collectionName, char *pathName,
char *resourceName, char *dataValue1, char *dataValue2, int retractionType,
char *userName, struct hostElement  *hostTabPtr);
extern int remoteRegisterRepl (int catType, char *objID, char *collectionName,
char *origResName, char *origPathName,
char *newResName, char *newPathName, char *userName, char *domainName);
extern int remoteReplDataset (int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName, struct hostElement  *hostTabPtr);
extern int remoteMoveDataset (int catType, char *objID, char *collectionName,
char *srcResource, char *newResourceName, char *newPathName, 
char *container, struct hostElement  *hostTabPtr);
extern int remoteModifyCollect (int catType, char *collectionName,
char *dataValue1, char *dataValue2, char *dataValue3, int retractionType,
struct hostElement  *hostTabPtr);
extern int remoteListCollect (int catType, char *collectionName,
char *flag, mdasC_sql_result_struct **myresult, int rowsWanted, 
struct hostElement  *hostTabPtr);
extern int remoteCreateCollect (int catType, char *parentCollect, 
char *newCollect, struct hostElement  *hostTabPtr);
extern int remoteRegisterReplica (int catType, char *objID, 
char *collectionName, char *origResourceName, char *origPathName,
char *newResourceName, char *newPathName,
char *userName, char *domainName, struct hostElement  *hostTabPtr);
extern int svrChkMdasAuth (struct varlena *vUserName, struct varlena *vSrbAuth,
struct varlena *vMdasDomain);
extern int 
_svrChkMdasAuth (char *userName, char *srbAuth, char *mdasDomain);
extern int svrChkEncryptAuth (struct varlena *vUserName, 
struct varlena *vMdasDomain,
int iLen1, int iLen2,
struct varlena *cpBuf1, struct varlena *cpBuf2);
extern int svrChkMdasSysAuth (struct varlena *vUserName, 
struct varlena *vSrbAuth, struct varlena *vMdasDomain);
extern int remoteGetPrivUsers (int catalog, mdasC_sql_result_struct **myresult,
int rowsWanted, struct hostElement  *hostTabPtr);
extern int remoteGetMoreRows (int catalog, int contDesc,
mdasC_sql_result_struct **myresult, int rowsWanted, 
struct hostElement  *hostTabPtr);
extern int
remoteIssueTicket (char *objID, char *collectionName,
char *collectionFlag, char *beginTime, char *endTime, int accessCnt, 
char *ticketUser, char **ticket, struct hostElement  *hostTabPtr);
extern int
remoteRemoveTicket (char *ticket, struct hostElement  *hostTabPtr);
extern int
remoteProxyOpr (struct hostElement  *hostTabPtr, ProxyOprId operation,
int inputInt1, int inputInt2, int inputInt3, int inputInt4,
char *inputStr1, char *inputStr2, char *inputStr3, char *inputStr4,
char *inputStr5, byteStream *inputBStrm1, byteStream *inputBStrm2,
char *outBuf);
extern int
remoteGetContainerInfo (int catalog, char *containerName,
char *userName, char*domainName,
mdasC_sql_result_struct **myresult,
int rowsWanted, struct hostElement  *hostTabPtr);
extern int
chkHost (char *hostName, char *portNum, struct hostElement  **hostTabPtr);
extern int
remoteRegisterLocation(char *locName, char *fullAddr, char *parentLoc,
char *serverUser, char *serverUserDomain, struct hostElement  *hostTabPtr);
extern int
remoteInsertTokenInfo(char *typeName, char *newValue, char *parentValue,
struct hostElement  *hostTabPtr);
extern int
remoteRegisterResourceInfo(char *rescName, char *rescType,
char *location, char *userName, char *userDomain,  char *userAuth,
char *phyPath, char *class, int size, struct hostElement  *hostTabPtr);
extern int
remoteRegisterLogicalResourceInfo (char *logRescName, char *rescType,
char *physRescName,  char *physPath, struct hostElement  *hostTabPtr);
extern int
remoteRegisterReplicateResourceInfo(char *physRescName, char *rescType,
char *oldLogRescName, char *indefaultPath, struct hostElement  *hostTabPtr);
extern int
remoteDeleteValue(int valueType, char *deleteValue, 
struct hostElement  *hostTabPtr);
extern int
remoteGetResOnChoice (int catType, char *logResName,
char *phyResName, char *inputFlag, struct hostElement  *hostTabPtr);
extern int
remoteObjStat (int catType, char *path, int myType, struct srbStat *retStat,
struct hostElement  *hostTabPtr);
/* XXXXXX need to add struct hostElement  *hostTabPtr to these 2 tables */
extern int
remoteSetupSessionPublicKey(char *publicKey);
extern int
remoteSetupSession(char *sessionKey, struct hostElement  *hostTabPtr);
extern int remoteModifyZone (int catType, char *zoneName,
       char *dataValue1, char *dataValue2, char *dataValue3, char *dataValue4,
       char *dataValue5, int actionType, struct hostElement  *hostTabPtr);

extern int remoteBulkQueryAnswer (int catType, 
				  char *queryInfo,
				  mdasC_sql_result_struct **myresult, 
				  int rowsWanted, 
				  struct hostElement  *hostTabPtr);
extern int
_svrChkEncryptAuth (char *userName, char *mdasDomain, int iLen1, int iLen2,
char *cpBuf1, char *cpBuf2);

extern int
remoteFileChksum (struct hostElement  *hostTabPtr, int fd_inx,
int flags, unsigned long *ichksum,
srb_long_t myOffset, srb_long_t mySize);

extern char *
getAddrWithZone (char *addr, char *mcatHostHint);

extern int
chkHostAndZone (char *hostName, struct hostElement  **hostTabPtr);

extern int remoteModifyUserNonPriv (int catType, char *userNameDomain,
       char *dataValue1, char *dataValue2, char *dataValue3, char *dataValue4,
       char *dataValue5, int actionType, struct hostElement  *hostTabPtr);

extern int remoteModifyResource (int catType, char *resourceName,
       char *dataValue1, char *dataValue2, char *dataValue3, char *dataValue4,
       int actionType, struct hostElement  *hostTabPtr);

extern int 
remoteObjProc(int descInx, char *procName, char *inBuf, int inLen,
	       char *outBuf, int outLen, struct hostElement  *hostTabPtr );
srb_long_t
remoteGetFsFreeSpace (int storSysType, struct hostElement  *hostTabPtr,
char *path, int flag);

int
remoteFileFStage (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, int flags);
int
prtLocalHost ();
int prtVaultConf ();
int 
remoteGetPrivUsers (int catalog, mdasC_sql_result_struct **myresult,
int rowsWanted, struct hostElement  *hostTabPtr);

int
remoteMDriverOpen (int storSysType, struct hostElement  *hostTabPtr, 
char *rsrcLoc, char *fileName, int flags, int mode);

int
remoteMDriverCreate (int storSysType, struct hostElement  *hostTabPtr, char *rsrcLoc,
char *fileName, int mode);

int
remoteMDriverUnlink (int storSysType, struct hostElement  *hostTabPtr,
		     char *resourceLoc,  char *dataPath);


/* Definition used by a number function for specify whether a new connect
 * should be a unique connection or the generic one attached to the
 * struct hostElement.
 */
#define GENERIC_CONN	0
#define UNIQUE_CONN	1

int
remoteFileChown (int storSysType, struct hostElement  *hostTabPtr,
char *fileName, char *fileUser, char *fileGroup, int chownFlag);
#endif	/* DISTRIB_EXTERN_H */
