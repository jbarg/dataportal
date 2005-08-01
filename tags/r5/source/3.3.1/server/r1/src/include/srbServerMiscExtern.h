/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* srbServerMiscExtern.h - Definition and prototypes for srbServerMisc.c
 */

#ifndef SRB_SERVER_MISC_EXTERN_H
#define SRB_SERVER_MISC_EXTERN_H

extern int initSrbServer();
extern int execCommand (char *command, char *commandArgv, int lsock, 
int portalFlag);
extern int svrCloseAll ();
extern int initSrbUserList ();
extern int chkSrbUser (char *namebuf);
extern int pingMCAT ();
extern int chkHostBasedAuth(struct sockaddr_in *addr, StartupInfo *sp);
extern int srbCollStat (int catType, char *collection,
mdasC_sql_result_struct **myresult);
extern int svrDataStat (int catType, char *dataName, char *collection,
mdasC_sql_result_struct **myresult);
extern int htonStat (struct srbStat *retStat);
extern int
_svrGetdents (int fd, void *buf, size_t nbyte, int is64Flag);
extern int
svrSetFdToStage3 (int fd);
extern int
svrGetCollInColl (char *parColl, int maxRow, char **nameList,
int *continueIndex);
extern int
svrResToDirent (char *nameList, int rowCnt, int fd, int firstFlag);
extern int
svrGetMoreData (int *continueIndex, int maxCnt, int mcatIndex, char **nameList);
extern int
svrOutputDirent (int fd, dirent_t *buf, int bufLen);
extern int
svrOutputDirent64 (int fd, dirent_t *buf, int bufLen);
extern char *
getLockPath (char *objID, char *collectionName);
extern int
isDataset (char *dataName, char *collection);
extern int
svrParseContainerName (char *inContainerName, char *outContainerName,
char *collectionName, char *userName, char *domainName);
extern int
procStartupMsg (Port *myPort, StartupInfo *sp);
int svrCollStat (int catType, char *collection,
mdasC_sql_result_struct **myresult);
int isPureObject(char *dataName, char *collection);
int execFunction(char *command, char *commandArgv, int lsock, int portalFlag);
int
svrResToDirent64 (char *nameList, int rowCnt, int fd, int firstFlag);
int
svrGetDataInColl (char *parColl, int maxRow, char **nameList,
int *continueIndex);
#endif  /* SRB_SERVER_MISC_EXTERN_H */

