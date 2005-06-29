/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* clStubIntern.h - Header file for clStub.c
 */
 
/*-------------------------------------------------------------------------
 *
 *    Client function interface
 *
 *-------------------------------------------------------------------------
 */
#ifndef CL_STUB_INTERN_H
#define CL_STUB_INTERN_H

#include "exfFunctExtern.h"

extern int _srbFileOpen(srbConn* conn, int storSysType, char *hostAddr, 
char *filename, int flags, int mode, int chdirFlag);
extern int _srbFileCreate(srbConn* conn, int storSysType, char *hostAddr, 
char *filename, int mode, int chdirFlag, srb_long_t size);
extern int _srbFileMkdir(srbConn* conn, int storSysType, char *hostAddr, 
char *filename, int mode, int chdirFlag);
extern int _srbFileChmod(srbConn* conn, int storSysType, char *hostAddr, 
char *filename, int mode, int chdirFlag);
extern int _srbFileUnlink(srbConn* conn, int storSysType, char *hostAddr,
char *filename, int chdirFlag);
extern int _srbFileRmdir(srbConn* conn, int storSysType, char *hostAddr,
char *filename, int chdirFlag);
extern int _srbOpendir(srbConn* conn, int storSysType, char *hostAddr,
char *dirname, int chdirFlag);
/*
 * srbRegisterDataset - Register a SRB dataset
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to create. The objID is obtained
 *              through registration with MDAS.
 *         char *dataTypeName - Data type. e.g. "generic"
 *         char *resourceName - The storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         char *collectionName - The collection name.
 *         char *pathName - The file/DB path of the data.
 *         srb_long_t dataSize - The size of the dataset if known. 0 = unknown.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int
srbRegisterDataset (srbConn* conn, int catType, char *objID,
char *dataTypeName, char *resourceName, char *collectionName,
char *pathName, srb_long_t dataSize);

/*
 * srbObjAudit - Make an audit entry for an object
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *userName - The userName.
 *         char *objID - The objID of the object to audit.
 *         char *collectionName - The name of the collection this objID
 *              belongs.
 *         char *dataPath - The path name of the object.
 *         char *resourceName - the resource name of the object.
 *         char *accessMode - The access mode ("read", "write", "all");
 *         char *comment - comments to be included.
 *         int success - Indication whether the operation was successful.
 *           0 = failure, 1 - success.
 *         char *domainName - The domain name of the user.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

/*
 * srbRegisterContainer - Register a container to MCAT.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *containerName - The name of the container to be registered..
 *         char *resourceName - The storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         srb_long_t containerSize - The size of the container.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int srbRegisterContainer (srbConn *conn, int catType, 
char *containerName, char *resourceName, srb_long_t containerSize);

/*
 * srbRegisterReplica - Register a SRB Replica
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to create. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The collection name.
 *         char *origPathName - The orighnal file/DB path of the data.
 *              (This entry can be NULL).
 *         char *newResourceName - The new storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         char *newPathName - The new file/DB path of the data.
 *         char *userName - The new file/DB path of the data.
 *         struct varlena *domainName - The domain name to which the object
 *         belongs. Valid domains - "sdsc".
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int srbRegisterReplica (srbConn* conn, int catType, char *objID,
char *collectionName, char *origResourceName, char *origPathName,
char *newResourceName, char *newPathName,
char *userName, char *domainName);

extern int srbObjAudit (srbConn* conn, int catType,
char *userName, char *objID,
char *collectionName,  char *dataPath,  char *resourceName,
char *accessMode,  char *comment, int success,  char *domainName);

/*
 * srbRegisterInContDataset - Register a inContainer dataset
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to create. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The collection name.
 *         char *containerName - The collection name.
 *         char *dataTypeName - Data type. e.g. "generic"
 *         srb_long_t dataSize - The size of the dataset if known. 0 = unknown.
 *         srb_long_t baseOffset - The offset of the dataset if known. 
 *				   0 = unknown.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int srbRegisterInContDataset (srbConn *conn, int catType, char *objID,
 char *collectionName, char *containerName, char *dataTypeName,
srb_long_t dataSize, srb_long_t baseOffset);

/*
 * srbGetResOnChoice - Given the Logigical resource name and the inputFlag,
 * return a physical resource name.
 *
 * Input - srbConn* conn - From clConnect ().
 *         int catType - catalog type - 0 - MCAT
 *         char *logResName - The logical resource name.
 *         char *phyResName - The output physical resource.
 *         char *inputFlag - The Input flag, valid inputs are:
 *          "RR" - Round Robin.
 *          "RANDOM" - randomly selecting a physical resource from the
 *                     input logical resource.
 * Output - The Physical resource string.
 *          Returns 0 if successful..
 *          Returns a negative value upon failure.
 * the CALLER must have allocated enough space to hold the result returned
 */

extern int
srbGetResOnChoice(srbConn *conn, int catType, char *logResName,
char *phyResName, char *inputFlag);
extern int _srbObjStat(srbConn* conn, int catType, char *path,
struct srbStat *statbuf, int myType);

/*
 * srbObjLock - Lock a SRB object. It is a priviledged call for
 * now.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The SRB object ID to lock. The objID is obtained
 *              through registration with MDAS.
 *         char *collectionName - The collection name.
 *         lockCmd - F_SETLKW (see fnctl).
 *         lockType - F_RDLCK, F_WRLCK or F_UNLCK.
 *         lockFd - The fd of the lock. Valid only for F_UNLCK.
 *
 * Output : Returns the Object descriptor - success, negative  - failure
 */


extern int
srbObjLock(srbConn* conn, char *objID, char *collectionName,
int lockCmd, int lockType, int lockFd);
extern srb_long_t
_srbDataCopy(srbConn* conn, int destObjInx, char *destPath, char *destResLoc,
int srcObjInx, char *srcPath, char *srcResLoc, srb_long_t size, int zoneFlag);

/*
 * _srbDataPut - Copy a dataset from local space to SRB space. This is
 * only used by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int destObjInx - The object type index.
 *         e.g., objType = srbObjTypeEntry[destObjInx].objType.
 *         char *destPath - The destination path.
 *         char *destResLoc - The destination resource.
 *         char *clHostAddr - The client (source) portal network address.
 *         int clPort - The client portal port number.
 *         srb_long_t size - The size of the file. Must be non-negative.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

extern srb_long_t
_srbDataPut(srbConn* conn, int destObjInx, char *destPath, char *destResLoc,
char *clHostAddr, int clPort, srb_long_t size);

/*
 * _srbDataGet - Get a dataset from SRB space to local space. This is
 * only used by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int srcTypeInx - The object type index.
 *         e.g., objType = srbObjTypeEntry[destObjInx].objType.
 *         char *srcPath - The source path.
 *         char *srcResLoc - The source resource.
 *         char *clHostAddr - The client (source) portal network address.
 *         int clPort - The client portal port number.
 *	   srb_long_t size - The size of the object.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */
extern srb_long_t
_srbDataGet(srbConn* conn, int srcTypeInx, char *srcPath, char *srcResLoc,
char *clHostAddr, int clPort, srb_long_t size);

extern int
srbTapelibMntCart(srbConn* conn, char *tapeNum, int priority,
char **outDevPath);

extern int
srbTapelibDismntCart(srbConn* conn, char *tapeNum);

/*
 * srbDumpFileList - Dump a list of files to tape. This is a priviledged
 * function.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         tapeLibInx - The tape library index to dump the files to.
 *         fileList - list of files to dump. - files to be dumped are
 *           contained in the fileList->fileName[numFile][MAX_DATA_SIZE]
 *         array. Note that the fileName is the physical path name of the
 *         cache copy.
 *         purgeFlag - 1 - purge the cache copy when done dumping.
 *                     0 - no purge.
 *
 *
 * Output - status of the dump.  Returns a negative value upon failure.
 */

extern int
srbDumpFileList (srbConn *conn, int tapeLibInx, fileList_t *fileList,
int purgeFlag);

/*
 * srbStageCompObj - Stage a compound Object to cache.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The name Of the compound obj
 *         char *collectionName - The collection
 *         char *resourceLoc - The resc location of the compound obj.
 *         char *resourceName - The resource of the compound obj.
 *         srb_long_t offset - The offset to be staged
 *         srb_long_t size - The size to be staged.
 *         int replNum - The replication number of the compound obj.
 *         int segNum - The segment number of the compound obj.
 *         int stageFlag - if stageFlag <= 0, just create a zero length
 *             file in cache, register and  close it.
 *         struct mdasInfoOut **outCacheInfoOut - The output struct that
 *             contains info for the cache copy.
 *
 * Output - status of the stage.  Returns a negative value upon failure.
 */


extern int
srbStageCompObj (srbConn* conn, char *objID, char *collectionName,
char *resourceLoc, char *resourceName,
srb_long_t offset, srb_long_t size,
int replNum, int segNum, int stageFlag,
struct mdasInfoOut **outCacheInfoOut);

/*
 * srbRegInternalCompObj - register an internal compound obj..
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *objID - The name of the compound obj
 *         char *collectionName - The collection
 *         int objReplNum - The replication number of the compound obj
 *         int objSegNum - The segment number of the compound obj
 *         char *intObjRescName - The resource where the internal compound obj
 *           is located .
 *         char *dataPathName - The physical path of the int comp obj.
 *         srb_long_t dataSize - The size of the int comp obj.
 *         srb_long_t offset - The offset of the int comp obj.
 *         int inpIntReplNum - The replication number of the int compound obj.
 *         int intSegNum - The segment number of the int compound obj.
 *         int objTypeInx - In case of failure, if objTypeInx >=0, the
 *           int comp obj will be removed. It is the object type index
 *           used to do the unlinking.
 *         char *phyResLoc - valid only when objTypeInx >=0. This is the
 *           resouce location used to do the unlinking.
 *
 * Output - status of the registration.  Returns a negative value upon failure.
 */

extern int
srbRegInternalCompObj (srbConn* conn, char *objName, char *objCollName, 
int objReplNum, int objSegNum, char *intObjRescName, char *dataPathName, 
srb_long_t dataSize,
srb_long_t offset, int inpIntReplNum, int intSegNum,
int objTypeInx, char *phyResLoc);
extern int
srbRmIntCompObj (srbConn* conn, char *objName, char *objCollName,
int objReplNum, int objSegNum,
int inpIntReplNum, int intSegNum);
extern int
srbRmCompObj (srbConn* conn, char *objName, char *objCollName,
int objReplNum, int objSegNum);
extern int
srbModInternalCompObj (srbConn* conn, char *objID, char *collectionName,
int   objReplNum, int objSegNum,
int inpIntReplNum, int intSegNum,
char *data_value_1, char *data_value_2,
char *data_value_3, char *data_value_4, int retraction_type);
extern int
srbModifyRescInfo (srbConn* conn, int catType, char *resourceName,
int retractionType,
char *dataValue1, char *dataValue2, char *dataValue3, char *dataValue4);
extern int
srbGetTapeCartPri (srbConn* conn, tapeCartPri_t **outTapeCartPri);
/*
 * srbRegDatasetInternal - Server version of register a SRB dataset. Only
 * a privileged user api.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int catType - catalog type. e,g., MDAS_CATALOG.
 *         char *objID - The SRB object ID to create. The objID is obtained
 *              through registration with MDAS.
 *         char *userName - owner of dataset.
 *         char *access - access mode.
 *         char *domainName - domain of owner
 *         char *dataTypeName - Data type. e.g. "generic"
 *         char *resourceName - The storage resource name.
 *              e.g. "mda18-unix-sdsc"
 *         char *collectionName - The collection name.
 *         char *pathName - The file/DB path of the data.
 *         srb_long_t dataSize - The size of the dataset if known. 0 = unknown.
 *         int rmObjTypeInx - the objTypeInx of the dataset. If this index
 *               is >= 0, unlink the object if the registration failed.
 *         char *rmObjResLoc - If rmObjTypeInx >= 0, the Resource location
 *               of the object.
 *
 *
 * Output - Returns 0 - success.
 *          Returns negative - failure.
 */

extern int
srbRegDatasetInternal (srbConn* conn, int catType, char *objID,
char *userName, char *access, char *domainName,
char *dataTypeName, char *resourceName, char *collectionName,
char *pathName, srb_long_t dataSize,
int rmObjTypeInx, char *rmObjResLoc);

/*
 * srbChkEncryptAuth - Authenticate a userName/passwd.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         char *userName   and
 *         char *mdasDomain - The MDAS Domain.
 *         int iLen1 - The length of the Challenge/Scrambled buffer
 *         int sessionFlag - create a session ? (for fed MCAT only)
 *             0 - don't create a session, 1 - create a session.
 *         char *cpChallengeMessageBuf - The challenge msg buf
 *         char *cpScrambledMsgFromClient - The Scrambled response
 *
 *
 * Output - Returns 0 (success for sessionFlag = 0), or a positive session
 *          number (sessionFlag = 1).
 *          Returns negative - failure.
 */

int
srbChkEncryptAuth (srbConn* conn, char *userName, char *mdasDomain,
                   int iLen1, int sessionFlag,
                   char *cpChallengeMessageBuf,
                   char *cpScrambledMsgFromClient);

int
qvalToGenQuery (char qval[][MAX_TOKEN], int *selval, genQuery_t *myGenQuery);
int
clearGenQuery (genQuery_t *myGenQuery);

/*
 * _srbBulkUnload - Internal version of Bulk unload of non-container files
 * used by server-server call.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int flag - flag value for future use.
 *         char *clHostAddr - The host address of client portal.
 *         int clPort - The port number of client portal.
 *         char *collection - The collection to unload
 *
 * Output - In addition to myresult, returns 0 or positive value
 *          representing the socket fd from which the data will come.
 *
 */

extern int
_srbBulkUnload (srbConn *conn, int catType, int flag,
char *clHostAddr, int clPort, char *collection);
extern int
_srbBulkUnloadC (srbConn *conn, int catType, int flag,
char *collection, portList_t **myPortList);
/*
 * _srbDataGetC - The client driver version of _srbDataGet. Get a dataset
 * from SRB space to local space. This is only used by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int srcTypeInx - The object type index.
 *         e.g., objType = srbObjTypeEntry[destObjInx].objType.
 *         char *srcPath - The source path.
 *         char *srcResLoc - The source resource.
 *         srb_long_t size - The size of the object.
 *         int flag - must have CL_INIT_CONN_FLAG on
 *         int numThreads - input number of threads
 *         portList_t **myPortList - the output portList.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
_srbDataGetC(srbConn* conn, int srcTypeInx, char *srcPath, char *srcResLoc,
srb_long_t size, int flag, int numThreads, portList_t **myPortList);
/*
 * _srbDataGetC - The client driven version of _srbDataGet. Get a dataset
 * from SRB space to local space. This is only used by the SRB server.
 *
 * Input - srbConn* conn - From srbConnect ().
 *         int srcTypeInx - The object type index.
 *         e.g., objType = srbObjTypeEntry[destObjInx].objType.
 *         char *srcPath - The source path.
 *         char *srcResLoc - The source resource.
 *         srb_long_t size - The size of the object.
 *         int flag - must have CL_INIT_CONN_FLAG on
 *         int numThreads - input number of threads
 *         portList_t **myPortList - the output portList.
 *
 * Output - Returns the number of bytes copied.
 *          Returns a negative value upon failure.
 */

srb_long_t
_srbDataGetC(srbConn* conn, int srcTypeInx, char *srcPath, char *srcResLoc,
srb_long_t size, int flag, int numThreads, portList_t **myPortList);
#endif	/* CL_STUB_INTERN_H */
