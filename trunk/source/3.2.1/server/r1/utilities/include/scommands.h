/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#ifndef SCOMMAND_H
#define SCOMMAND_H

#include <stdio.h>
#ifndef _WIN32
#include <sys/file.h>
#endif
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <fcntl.h>
#endif
#include <sys/types.h>
#if defined(PORTNAME_c90)
#include <sys/jtab.h>
#endif

#ifdef _WIN32
#include "unix2Nt.h"
#endif

/*
#include "srb.h"
#include "clConnectExtern.h"
#include "mdasC_db2_externs.h"
#include "clStubExtern.h"
#include "srb_client_errno.h"
*/
#include "srbClient.h"

extern char *optarg;
extern int optind, opterr, optopt;

#define MAX_ARG_EXP_CNT 300
#define MAX_GET_RESULTS 300
/*
#define MAX_GET_RESULTS 3
*/
#define DEFAULT_FIELD_WIDTH 0
#define DEFAULT_ROW_COUNT 30
#define MAX_P_OPEN_RETRY  50
#define MAX_NUM_THREADS 8
#define BUFSIZE         (4*1024*1024)
/* use the def in portal.h #define SIZE_PER_THR    (200*1024*1024) */

#define DEF_PURGE_TIME  0       /* default purge time in hours */
#define SEC_PER_HR      3600    /* seconds per hour */

/* Rsync definition */
#define RSYNC_ROWS_WANTED 2000

/* dedinition for trashExpired returned value */

#define TRASH_NOT_EXPIRED	0
#define TRASH_EXPIRED		1
#define TRASH_UNKNOWN		-1

/* The SRB path prefix */

#define SRB_PREFIX      	"s%"
#define LOCAL_PREFIX    	"l%"
#define SRB_PREFIX_PATH 	"/s%"
#define LOCAL_PREFIX_PATH 	"/l%"
#define  SRB_ROOT       "/srb/"
#define  SRB_ROOT1       "/srb"
#define LOCAL_CWD       "LOCAL_CWD"	/* env for cwd for local filesystem */
#define SRB_CWD		"SRB_CWD"	/* env for CWd for srb file system */
#define SRB_HOME	"SRB_HOME"	/* env for home for srb file system */
#define LOCAL_HOME	"HOME"		/* env for home for srb file system */
#define CWD_FS_FLAG	"CWD_FS_FLAG"	/* current file system flag, 
					 * SRB or LOCAL */
#define LOCAL_FS	"LOCAL_FS"	/* the CWD is a local file system */
#define SRB_FS		"SRB_FS"	/* the CWD is a SRB file system */
#define ERROR_FS	"ERROR_FS"	/* The input CWD does'nt exist */


/* Definition for the chkColl flag or prtDataLong() and prtDataShort() */
#define NO_CHK_COLL	0	/* don't check collection to print full path */
#define CHK_COLL	1       /* check collection to print full path */

/* The temporary file to hold the bulk load content */
#define TMP_BLOAD_FILE	"TmpBload"

/* definition for isInTrash */

#define NOT_IN_TRASH	0
#define IS_IN_TRASH     1
#define IS_TRASH_ROOT   2

/* The selval flag for the srbGetdataDirInfo call */

typedef struct {
#if defined(PORTNAME_c90)
    unsigned int    	s_unused1:32;	/* unused */
#endif
#if defined(PORTNAME_aix)
    unsigned int        s_funct:8;      /* The select funcction */
    unsigned int        s_prior:8;      /* the sort priority. 0 = no sort.
                                         * 1 = high prior. 255 = low prior */
    unsigned int        s_order:2;      /* sort order. 0 = ascending,
                                         * 1 = descending */
    unsigned int        s_unused:14;    /* unused */
#else
    unsigned short	s_funct:8;	/* The select funcction */
    unsigned short	s_prior:8;	/* the sort priority. 0 = no sort.
		 			 * 1 = high prior. 255 = low prior */
    unsigned short	s_order:2;	/* sort order. 0 = ascending, 
					 * 1 = descending */
    unsigned short	s_unused:14;	/* unused */
#endif
} selVal;

struct srbFileList {
    int numFiles;
    int dupInx1;		/* duplicated */
    int dupInx2;		/* duplicated */
    char *endColl;
    char *endFileName;
    char *listFileName;
    struct srbFileList *next;
};

struct dirStack {
    DIR *dirPtr;
    char dirPath[MAX_TOKEN];
    char baseDir[MAX_TOKEN];
    struct dirStack *next;
    int fileCnt;
};

struct srbFileOut {
    char fileDir[MAX_TOKEN];
    char fileName[MAX_TOKEN];
    srb_long_t size;
    unsigned long chksum;
    char collection[MAX_TOKEN];	/* next few are only relevant to srb files */
    int replNum;
};

#ifdef foo	/* XXXXXXXX moved to clStubExtern.h */
/* The flag definition for Scommand */

#define C_FLAG		0x1		/* collection only or ls acl for Sls */
#define d_FLAG		0x2		/* dataName only */
#define P_FLAG		0x4		/* prompt before changing */
#define R_FLAG		0x8		/* recursive */
#define l_FLAG		0x10		/* The print long flag */
#define f_FLAG		0x20		/* The force flag */
#define F_FLAG		0x20		/* The force flag */
#define a_FLAG		0x40		/* The all replica flag */
#define c_FLAG		0x80		/* The container flag */
#define v_FLAG          0x100           /* The verbose flag for Sput and Sget */
#define m_FLAG          0x200           /* multi-thread Sput and Sget */
#define s_FLAG          0x400           /* single-thread Sput and Sget */
#define retry_FLAG      0x800           /* persistant (retry on error) Sput and Sget */
#define checksum_FLAG  0x1000           /* compute check sum in Sput and Sget */
#define t_FLAG         0x2000           /* Sbunload tar-like output */
#define V_FLAG         0x4000           /* The verbose flag for Sput and Sget plus progress bar */
#define z_FLAG         0x8000           /* The MCAT zone flag */
#define b_FLAG         0x10000           /* bulk load/unload flag */
#define e_FLAG         0x20000           /* exit on error flag */

#define u_FLAG         0x40000000       /* For the rmColl call. If this        
                                         * Flag is on, will unregister instead
                                         * of unlink
                                         */
#define unreg_FLAG	u_FLAG		
#endif	/* foo */

/* The struct for parsing the input dataName/collection */

typedef struct {
    char *inpArgv;		/* The input dataName/collection name */
    char *outArgv;   		/* The output dataName/collection name */	
    srb_long_t size;		/* this should in at 8 byte boundary */
    int type;	/* The type. See def below  */
} srbPathName;

/* Definition for the NameArray type */

#define DATANAME_T	10	/* The input name is of dataName type 
				 * value set is DATANAME_T + row count - 1 */
#define COLLECTION_T	1	/* The input name is of collection type */
#define DATA_COLL_T	2	/* The input name may be data or collection */
#define SPEC_COLL_T	3	/* Special directory (".", "..", "/") */

struct fileOpr {
    char *collection;
    char *objName;
    char *localFileName;
    char *resource;
    srb_long_t offset;
    srb_long_t size;
    int rwFlag;         /* read/write flag. READ_OPR or WRITE_OPR */
    int flagval;
    int threadInx;
};

/* prototype of calls */

#ifdef c_plus_plus
extern "C" {
#endif
extern void mySplitpath(char *inname, char *collname, char *dataname);
extern int convNameArrayToSQL (srbPathName nameArray[], int nArgv);
extern int convContNameArrayToSQL (srbPathName nameArray[], int nArgv,
char *userName, char *domainName, char *mcatName);
extern int queryPrtColl (srbConn *conn, int catType, int nArgv, 
srbPathName nameArray[], int flagval, int L_val);
extern int queryCollColl (srbConn *conn, int catType, char *collName, 
int flagval, mdasC_sql_result_struct *collResult);
extern void genCollLikeSQL (char *inColl, char *outSQL, int recurFlag);
extern void
genDataLikeSQL (char *inData, char *outSQL);
extern void listSubColl (srbConn *conn, int catType, char *parColl, 
int flagval, int L_val);
extern void prtColl (srbConn *conn, mdasC_sql_result_struct *myresult, 
int flagval);
extern void listDataInColl (srbConn *conn, int catType, char *parColl, 
int flagval, int L_val);
extern void
prtDataShort (mdasC_sql_result_struct *myresult, int chkColl);
extern void
prtDataLong (mdasC_sql_result_struct *myresult, int chkColl, int flagval);
extern int
queryPrtCollData (srbConn *conn, int catType, int nArgv,
srbPathName nameArray[], int flagval, int L_val);
extern void 
listCollContent (srbConn *conn, int catType,
mdasC_sql_result_struct *collResult, int flagval, int L_val);
extern int
queryData (srbConn *conn, int catType, char *dataName, int flagval, int L_val,
mdasC_sql_result_struct *dataResult);
extern void 
chmodDataByRes (srbConn *conn, int catType, int flagval, char *mode, 
char *userdom, mdasC_sql_result_struct *dataResult);
extern int
expandChmodData (srbConn *conn, int catType, char *dataName, int flagval,
char *mode, char *userdom);
extern int
queryAndChmod (srbConn *conn, int catType, int nArgv,
srbPathName nameArray[], int flagval, char *mode, char *userdom);
extern int
queryDataInColl (srbConn *conn, int catType, char *parColl, int flagval, 
int L_val, mdasC_sql_result_struct *myresult);
extern int
querySubCollInColl (srbConn *conn, int catType, char *parColl, int flagval,
int L_val, mdasC_sql_result_struct  *myresult);
extern int
chmodColl (srbConn *conn, int catType, char *collName, int flagval,
char *mode, char *userdom);
extern int
chmodDataInColl (srbConn *conn, int catType, char *collName, int flagval,
char *mode, char *userdom);
extern int
expandAndChkName (srbConn *conn, int catType, srbPathName nameArray[],
int flagval, mdasC_sql_result_struct *dataResult,
mdasC_sql_result_struct *collResult);
extern int 
copyMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
srbPathName targNameArray[], int flagval, char *containerName);
extern int
dataToDataCopy (srbConn *conn, int catType, int flagval, int targType,
mdasC_sql_result_struct *dataResult, int srcCopyNum, char *targColl, 
char *targObj, char *newResource, char *newPath, char *containerName);
int
dataToDataCopy1 (srbConn *conn, int catType, int flagval, int targType,
char *srcCh, char *srcPar, srb_long_t datasize, int srcCopyNum, char *targColl,
char *targObj, char *newResource, char *newPath, char *containerName);
extern int
dataToCollCopy (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, int srcCopyNum, char *targColl, 
char *newResource, char *newPath, char *containerName);
extern int
collToCollCopy (srbConn *conn, int catType, int flagval,
int inColltype, mdasC_sql_result_struct *inCollResult, char *origSrc,
char *newResource, char *containerName);
extern int
collToCollCopyCont (srbConn *conn, int catType, int flagval,
char *srcColl, char *targColl, char *newResource, char *containerName);
extern void filterDeleted (mdasC_sql_result_struct *myresult); 
extern int
chkFileName (srbPathName *fileName);
extern int
fileToDataCopy (srbConn *conn, int catType, int flagval, int targType,
srbPathName *fileName, char *targColl, char *targObj, 
char *datatype, char *newpathname, char*newResource, char *containerName,
int copyNum);
extern int
fileToCollCopy (srbConn *conn, int catType, int flagval,
srbPathName *fileName, char *targColl, 
char *datatype, char *newpathname, char *newResource, char *containerName);
extern int
dirToCollCopyCont (srbConn *conn, int catType, int flagval,
char *srcDir, char *targColl, char *newResource, char *containerName, char *newPathName);
extern int
dirToCollCopy (srbConn *conn, int catType, int flagval,
srbPathName *inPathName, char *origTarg, char *newResource, 
char *containerName, char *newPathName);
extern int
rmColl (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *collResult, int copyNum, int inpPurgeTime);
extern int
rmData (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, int copyNum, int purgeTime);
extern int checkCollection(srbConn *conn, int catType, char *collName);
extern int
getMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
srbPathName targNameArray[], int flagval, int copyNum, char *ticketId);
extern int
putMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
srbPathName targNameArray[], int flagval, char *containerName, int copyNum);
extern int
registerDir (srbConn *conn, int catType,
srbPathName *inPathName, char *origTarg, char *newResource);
extern int 
replicateDataInRes (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *myresult, char *newResource);
extern int
replicateColl (srbConn *conn, int catType, int flagval, char *srcColl,
 char *newResource);
extern char *
splitString(char *st1, char *st2, char *st3, char key);
extern int getSessionEnvFile ();
extern int initSrbClientEnv();
extern int firstInitSrbClientEnv();
extern int rewriteSrbClientEnvFile();
extern int parseName (srbPathName *mysPathName, char *defCollection);
extern int replDataInColl (srbConn *conn, int catType, char *parColl,
char *pathname, char *resourcename, int flagval, int copyNum);
extern int replDataByRes (srbConn *conn, int catType, char *pathname, char *resourcename,
int flagval, mdasC_sql_result_struct *myresult, int copyNum);
extern int
replInColl (srbConn *conn, int catType, char *parColl, char *pathname,
char *resourcename, int flagval, int copyNum);
extern int
syncDataByRes (srbConn *conn, int catType, char *resourcename,
int flagval, mdasC_sql_result_struct *myresult);
extern int
syncInColl (srbConn *conn, int catType, char *parColl,
char *resourcename, int flagval);
extern int
syncDataInColl (srbConn *conn, int catType, char *parColl,
char *resourcename, int flagval);
int
expandSyncData (srbConn *conn, int catType, char *dataName,
char *resourcename, int flagval);
int
syncdMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
char *resourcename, int flagval);
char *
getHostByResource (srbConn *conn, char *resourceName);
char *
getContainerByObj (srbConn *conn, char *inContainerObj);
int
unloadContMain (srbConn *conn, int catType, int nArgv,
srbPathName nameArray[],int flagval, char *localdir,
srbPathName collNameArray[]);
int
getContainerByColl (srbConn *conn, srbPathName collNameArray[],
srbPathName **nameArray);
int
trashExpired (int inpPurgeTime, char *srcCh, char *srcPar);
int
dataChksum (srbConn *conn, mdasC_sql_result_struct *dataResult,
int flagval, int srcCopyNum, char *currentColl);
void
printChksum (char *srcCh, char *srcPar, char *chksum, char *currentColl, 
int regFlag);
int
collChksum (srbConn *conn, mdasC_sql_result_struct *inCollResult,
int flagval, int srcCopyNum, char *currentColl);
int
collChksumCont (srbConn *conn, char *srcColl, int flagval, int srcCopyNum,
char *currentColl);
int
getClientUserFromConn (srbConn *conn, char **userName, char **domainName);
int
srbSu (char *userName, char *domainName);
int
backupDataByRes (srbConn *conn, int catType, char *resourcename,
int flagval, mdasC_sql_result_struct *myresult, srbConn **myConn);
int
backupInColl (srbConn *conn, int catType, char *parColl,
char *resourcename, int flagval, srbConn **myConn);
int
backupDataInColl (srbConn *conn, int catType, char *parColl,
char *resourcename, int flagval, srbConn ** myConn);
int
getNextSrcFile (struct dirStack **dirStackHead,
struct srbFileOut *mySrcFileOut, int flagval);
int
dirToCollRsync (srbConn *conn, int catType, int flagval,
char *srcDir, char *targColl, char *newResource, 
char *newPathName, char *tmpInxDir);
int
genSrbFileList (srbConn *conn, int catType, int flagval, char *targColl,
char *tmpInxDir, struct srbFileList  **srbFileListHead,
mdasC_sql_result_struct *myresult);
int
matchFileOutInMem (struct srbFileOut *inFile, struct srbFileOut *outFile,
mdasC_sql_result_struct *myresult, char *targColl);
int
localToSrbFileCopy (srbConn *conn, int catType, char *localPath,
char *targObj, char *targColl, srb_long_t datasize, int flagval, 
int fileExist, char *dataType, char *newpathname, char *newResource,
int copyNum);
int
createCollectRecur (srbConn *conn, int catType, char *startColl,
char *collection, int inheritCont);
int
createDirRecur (char *startDir, char *directory);
int
fileToDataRsync (srbConn *conn, int catType, int flagval,
struct srbFileOut *mySrcFileOut, struct srbFileOut *myTargFileOut,
char *localPath, char *origTargColl, char *targColl, char *newResource,
int copyNum);
int
fileToCollRsync (srbConn *conn, int catType, int flagval,
struct srbFileOut *mySrcFileOut, char *targColl, char *localPath,
char *resourceName, int copyNum);
int
dataToDirRsync (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, char *targDir);
int
collToDirRsync (srbConn *conn, int catType, int flagval,
int inColltype, int collCnt, mdasC_sql_result_struct *inCollResult, 
char *targDir);
srb_long_t
srbToLocalFileCopy1 (srbConn *conn, char *localPath, char *srbDataName,
char *srbCollection, srb_long_t datasize, int flagval);
int
dataToFileRsync1 (srbConn *conn, int catType, int flagval,
char *dataName, char *collection, char *size, char *offset,
char *chksum, char *myTarg);
int
dataToFileRsync (srbConn *conn, int catType, int flagval, int targType,
mdasC_sql_result_struct *dataResult, char *targFile);
int
dataToDirRsync (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, char *targDir);
int
checkTrashColl (char *collection);
int
myCreateCollect (srbConn *conn, int catType, char *parcollname,
char *collname, int inheritCont);
int
dataToDataRsync (srbConn *conn, int catType, int flagval, int targType,
mdasC_sql_result_struct *dataResult, char *targPath,
int copyNum, char *resourceName);
int
dataToDataRsync1 (srbConn *conn, int catType, int flagval,
char *dataName, char *collection, char *size, char *offset,
char *chksum, char *myTarg, int copyNum, char *resourceName, int targType,
char *cTargSize, char *cTargChksum);

int
collToCollRsync (srbConn *conn, int catType, int flagval,
int inColltype, int collCnt, mdasC_sql_result_struct *inCollResult,
char *targColl, char *resourceName,char *newPath, char *tmpInxDir, int copyNum);
int
dataToCollRsync (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, char *targColl,
char *resourceName, int copyNum);
#ifdef c_plus_plus
}
#endif
#endif	/* SCOMMAND_H */
