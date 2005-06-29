/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * clMiscExtern.h
 */

#ifndef CL_MISC_EXTERN_H
#define CL_MISC_EXTERN_H

#ifndef _WIN32
#include <pwd.h>
#endif /* WIN32 */
#include  <sys/stat.h>
#include <dirent.h>
#include "srbType.h"
#include "srbStat.h"
#include "mdasGlobalsExtern.h"
#include "portal.h"

#define COMM_BUF_SIZE 512

#ifdef c_plus_plus
extern "C" {
#endif

typedef struct {
    int inpInx1;
    int inpInx2;
    int outInx;
    unsigned char inbuf[COMM_BUF_SIZE];
    char outbuf[COMM_BUF_SIZE];
} comBuf;

/* strct for srbGenQuery */
typedef struct {
    int selInxCnt;
    int qvalCnt;
    int *selInx;
    int *selVal;
    int *qvalInx;
    void *qval;         /* point to an array of char [qvalCnt][MAX_TOKEN] */
} genQuery_t;

extern int bufgetc(int fd, comBuf *commBuf, int authSchemeInx);
extern int bufread(int fd, char *buf, int len, comBuf *commBuf, 
int authSchemeInx);
extern int bufungetc(char c, int fd, comBuf *commBuf);
extern int bufputc(char c, int fd, comBuf *cmmBuf, int authSchemeInx);
extern int bufputs(char *s, int fd, comBuf *comBuf, int authSchemeInx);
extern int bufflush(int fd, comBuf *commBuf, int authSchemeInx);
extern int clGets(char* s, int maxlen, int fd, comBuf *commBuf,
int authSchemeInx);
extern int clGetnchar(char* s, int maxlen, int fd, comBuf *commBuf, 
int authSchemeInx);
extern int clPutnchar(char* s, int maxlen, int fd, comBuf *commBuf, 
int authSchemeInx);
extern int clPuts(char* s, int fd, comBuf *commBuf, int authSchemeInx);
extern int clGetc(int fd, comBuf *comBuf, int authSchemeInx);
extern int clGetInt(int* result, int bytes, int fd, comBuf *commBuf, 
int authSchemeInx);
extern int clGetLong(srb_long_t* result, int bytes, int fd, comBuf *commBuf, 
int authSchemeInx);
extern int
clPutLong(srb_long_t *i, int bytes, int fd, comBuf *commBuf, 
int authSchemeInx);
#if defined(PORTNAME_c90)
extern int
crGetInt(int* result, int bytes, int fd, comBuf *commBuf, int authSchemeInx);
#endif
extern int clPutInt(int n, int bytes, int fd, comBuf *commBuf, 
int authSchemeInx);
extern int clFlush(int fd, comBuf *commBuf, int authSchemeInx);
extern int getNextStr(FILE *Pfin, char *buf, int bufsz);
extern int clUnixErrConv (int status);
extern char *splitStrByLastChar(char *st1, char *st2, char *st3, char key);
extern int
srbRead (int fd, char *inbuf, int len, int authSchemeInx);
extern int
srbWrite (int fd, char *outbuf,int len, int authSchemeInx);
extern char *parseAddr (char *hostAddr);
extern int
portalCreate ();
extern int portalConnect (int port, char *hostName);
extern int
parseCollUserMode (mdasC_sql_result_struct *myresult, userInfo *proxyUser,
userInfo *clientUser, int *outUserId, int *outGid, int *outMode);
extern int
parseDataUserMode (mdasC_sql_result_struct *myresult, userInfo *proxyUser,
userInfo *clientUser, int *outUserId, int *outGid, int *outMode);
extern int
checkCollPerm (mdasC_sql_result_struct *myresult, userInfo *proxyUser,
userInfo *clientUser, int mode);
extern time_t srbToLocalTime (char *srbTime);
extern int
srbToLocalStat (struct srbStat *mySrbStat, struct stat *statbuf);
extern int
srbToLocalStat64 (struct srbStat *mySrbStat, struct stat64 *statbuf);
extern int
htonDirent (dirent_t *mydirent, int len);
extern int
htonDirent64 (dirent64_t *mydirent, int len);
extern int
ntohDirent (dirent_t *mydirent, int len);
extern int
ntohDirent64 (dirent64_t *mydirent, int len);
#if !defined(PORTNAME_c90)
#ifndef htonll
extern int ntohll (u_longlong_t *mylonglong);
extern int ntohll (u_longlong_t *mylonglong);
#endif
#endif  /* PORTNAME_c90 */
extern int srbToLocalLockCmd (int srbCmd);
extern int localToSrbLockCmd (int localCmd);
extern int unixErrConv (int status);
extern int
getrandomArray (int arraySz, int **randomAray);
extern srb_long_t addrToSRBLong (char *myAddr);
int
genPortInp (int clPort);
int
getPortFromPortInp (int portInp);
int
getCookieFromPortInp (int portInp);
void *
portalTransfer (struct transferInput *tranInp);
int
setMcatNameInQval (char* mcatName, char qval[][MAX_TOKEN]);
int
parseContainerName (char *inContainerName, char *outContainerName,
char *collectionName, char *userName, char *domainName, char *mcatName);
extern char *
getmcatNamefromHint (char *mcatNameHint);
int
readDataFromPortal (int srb_fd, char *buf, int maxLen);
#ifdef c_plus_plus
}
#endif

#endif 	/* CL_MISC_EXTERN_H */
