#ifndef SRB_PREL_MISC_H
#define SRB_PREL_MISC_H

#ifdef c_plus_plus
extern "C" {
#endif
       #include "srbClient.h"
#ifdef c_plus_plus
}
#endif

#include <pwd.h>
#include "scommands.h"
#if defined(PORTNAME_solaris)
#include <sys/dirent.h>
#endif
#if defined(PORTNAME_linux)
#include "kernel_stat.h"
#endif

/* not a user on this machine */
#define NOT_LOCAL_USER		999999
#define FAKE_ROOT_USER		777777
#define PUBLIC_USER		"public"
#define MY_BLKSIZE		8192

/* Use fake inode for stat and gendents */
#define FAKE_INODE		10000
#define FAKE_DEV		10

/* srb open file desc array */

struct srbFd {
    int fd;
    int isDir;
    int maxDirLen;
    int offset;
    int accumOffset;	/* The accumulative offset up to the beginning 
			 * of this dirent */
    int continueIndex;
    int gotData;
    void *dirent;
    char path[MAX_TOKEN];
};

struct prelCwd {
    char cwdFsFlag[MAX_TOKEN];		/* LOCAL_FS or SRB_FS */
    char localCwd[MAX_TOKEN];		/* The loacal cwd */
    char srbCwd[MAX_TOKEN];		/* the srbCwd */
};

/* definition for isDir */
#define IS_UNKNOWN	-1	/* don't know if it is file or dir */
#define IS_FILE         0       /* this is a file */
#define IS_DIR_1        1       /* is a collection. new desc */
#define IS_DIR_2        2       /* is a collection. listing data */
#define IS_DIR_3        3       /* is a collection. done listing data */
#define IS_DIR_4        4       /* is a collection. listing collection */

#define MAX_RET_CNT		100 	/* max number of query ret */

/* Used in CwdFlag. define whether the current dir is local or srb */

#define CWD_IS_LOCAL	0
#define CWD_IS_SRB	1

#define SRB_ENV		"SRB_ENV"	/* interprete relative path as srb */


/* Establish an SRB connection to the default server */
srbConn *	connToSrb ();

/* Resolve the SRB path. Format -  /srb/...../...:resource  */
int isSrbPath (char *inPath, char *resource, char *outPath );
int extractRes (char *inPath, char *outPath, char *resource);
int myParseName (srbPathName *srbPathName, char *condition);
int filterSrbPath (char *outPath, char *filterStr);

int srbStat (char *path, struct stat *buf, int myType);
int srbStat64 (char *path, struct stat64 *buf, int myType);
int srbDataStat (char *dataName, char *collection,
mdasC_sql_result_struct *myresult);
int srbCollStat (char *collection, mdasC_sql_result_struct *myresult);
#ifdef LOCAL_STAT
int parseDataUserMode (mdasC_sql_result_struct *myresult, int *outUserId,
int *outGid, int *outMode);
int parseCollUserMode (mdasC_sql_result_struct *myresult, int *outUserId,
int *outGid, int *outMode);
#endif
int srbGetdents (int fd, void *buf, size_t nbyte, int is64Flag);

time_t srbToLocalTime (char *srbTime);
int setFdToStage3 (int fd);
int getDataInColl (char *parColl, int maxRow, char **nameList,
int *continueIndex);
int getCollInColl (char *parColl, int maxRow, char **nameList, 
int *continueIndex);
int srbResToDirent (char *nameList, int rowCnt, int fd, int firstFlag);
int srbResToDirent64 (char *nameList, int rowCnt, int fd, int firstFlag);
int getMoreData (int *continueIndex, int maxCnt, int mcatIndex, 
char **nameList);
int outputDirent (int fd, dirent_t *buf, int bufLen);
int outputDirent64 (int fd, dirent_t *buf, int bufLen);
int stripSrbPath (char *inPath, char *outPath);
int srbCollSeekl (int fd, off64_t where, int whence, int is64Flag );
#if defined(PORTNAME_linux)
int myxstat64_conv (struct kernel_stat *kbuf, struct stat64* buf);
int myxstat_conv (struct kernel_stat *kbuf, struct stat* buf);
#endif

/* Initialize srb env */

int initSrbEnv ();

extern char EnvFilePath[MAX_TOKEN];
extern char defaultResource[MAX_TOKEN];
extern char mdasCollectionName[MAX_TOKEN];
extern char mdasCollectionHome[MAX_TOKEN];
extern int rewriteSrbClientEnvFile ();
extern char *_mygetcwd ();
extern char *mygethome ();
extern char *
splitStringfromEnd(char *st1, char *st2, char *st3, char key);


extern userInfo *ClientUser, *ProxyUser;
#endif
