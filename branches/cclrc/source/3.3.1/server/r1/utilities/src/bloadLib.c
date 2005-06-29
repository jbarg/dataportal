/* bloadLib.c 
 */

#if !defined(PORTNAME_osx)
/* XXXX see if this will fix macs buffer problem */
#define DBUF 1
#else
#undef PARA_OPR
#endif


#include "scommands.h"
#ifndef _WIN32
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#endif

#if defined(PORTNAME_linux)
#include <sys/timex.h>
#endif /* PORTNAME_linux */
#include <sys/stat.h>

#ifdef _WIN32
#include "Unix2Nt.h"
#include "nt_extra.h"
#include "SrbNtUtil.h"
#include "misc_util.h"
#endif

#define MAX_ROW_OUT	1000
#define MAX_CONN_CNT	10
#ifdef PARA_OPR
#define MAX_NUM_BL_THR 1024
#define MAX_ACT_REG_THREAD 3
#else
#define MAX_NUM_BL_THR 2
#define MAX_ACT_REG_THREAD 1
#endif
#define MAX_RETRY_SEC 1800
#define THREAD_SLEEP_TIME 2

#define NOT_READY	0
#define HAVE_DATA	1
#define DONE_IO		2
#define ALL_DONE	3

struct bufInfo {
    int bufSize;
    int offset[2];
    char *buf[2];
    int status[2];
    int curInx;
    int myFd;
    srb_long_t contOffset;
    srbConn *conn;
#ifdef PARA_OPR
    pthread_mutex_t condMutex;
    pthread_cond_t cond;
#endif
};

extern int RStatus;

/*
#define DEBUG
*/

int
fileToCollLoad (struct bufInfo *myBufInfo, srb_long_t *curOffset, 
srb_long_t curSize, char *inFileName, char *outCollection, int flagval);
int
dirToCollLoad (struct bufInfo *myBufInfo, srb_long_t *curOffset,
srbPathName *inPathName, char *outCollection, int flagval);
int
dirToCollLoadCont (struct bufInfo *myBufInfo, srb_long_t *curOffset,
char *inPathName, char *outCollection, int flagval);
void
waitBuf (struct bufInfo *myBufInfo); 
void
genBloadFileName ();        /* generate a new bload file */
void *
regAndBload (void *myInput);
int
dumpBuf (struct bufInfo *myBufInfo);
int
procOverFContainer (struct bufInfo *myBufInfo, srb_long_t *curOffset,
int flagval);
int
initResultStruct (mdasC_sql_result_struct *myresult, int flagval);
int
allocThrInx ();
int
srbConnectWithRetry (srbConn **conn, char *myHost, char *myAuth);
int
initContainerBload (srbConn *conn, char *containerName, srb_long_t *curOffset);
int
initNormalBload (srbConn *conn, char *collection);

extern char *optarg;
extern int optind, opterr, optopt;

extern char srbAuth[];
extern char srbHost[];
extern char mdasCollectionName[];
extern char inCondition[];
 
struct regInput {
    mdasC_sql_result_struct  *myresult;
    int myInx;
    char container[MAX_TOKEN];
};

int
regAndBloadS (struct regInput *myInput, srbConn *conn);

char ContainerName[MAX_TOKEN];
char TmpBloadFile[MAX_TOKEN];
char TmpBloadColl[MAX_TOKEN];

#ifdef PARA_OPR
pthread_t Tid[MAX_NUM_BL_THR];
#ifdef DBUF
int TranThrInx = -1;
#endif
#endif
int *Tstatus[MAX_NUM_BL_THR];
int Tactive[MAX_NUM_BL_THR];
int RegThrInx = -1;
int CurThrInx = 0;
int ContainerInx = 0;
srb_long_t MaxContSz = -1;	/* max container sz in MB) */
mdasC_sql_result_struct  *Myresult[MAX_NUM_BL_THR];
struct regInput RegInput[MAX_NUM_BL_THR];

extern char ResourceName[MAX_TOKEN];
extern char datatype[MAX_TOKEN];
extern char newpathname[MAX_TOKEN];

int VFlag = 0;

#ifdef _WIN32
SYSTEMTIME STimestart, STimestop;
#else
struct timeval STimestart, STimestop, STimetd;
#endif

void *regResult (void *myInput);
void transferRoutine (struct bufInfo *myBufInfo);


int 
bloadMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[], 
srbPathName *targName, int flagval)
{
    int i;
    int status;
    int dataCnt = 0;
    int collCnt = 0;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    srb_long_t curSize; 
    int myFd;
    srb_long_t curOffset;
    struct bufInfo myBufInfo;
    /* int curInx, ioInx; */

    int *retval;

    srandom ((unsigned int) time(0));
    RStatus = 0;

    for (i = 0; i < nArgv; i++) {
	/* set flagval to 0 first for non-recursive check */
	status = chkFileName (&nameArray[i]);
    	if (status < 0) {
            fprintf (stderr, "Source %s not found\n", nameArray[i].inpArgv);
#ifdef PARA_OPR
#ifdef DBUF
	    pthread_cancel (Tid[TranThrInx]);
#endif
#endif
            return (CLI_ERR_COLLNAME);
    	}

	if (nameArray[i].type >= DATANAME_T)
	    dataCnt ++;
	else
	    collCnt ++;
    }

    /* Do some sanity checks */

    if (dataCnt + collCnt == 0) {
        fprintf (stderr, "Source %s not found\n",
          targName->inpArgv);
        return (CLI_ERR_COLLNAME);
    } else if (dataCnt == 1 && collCnt == 0) {
	/* ingesting just one file. shouldn't be here */
	if (checkCollection (conn, MDAS_CATALOG, targName->outArgv) != 0) {
	    /* not a collection */
	    clSplitbychar (targName->outArgv, temp, temp1, '/');
            status = fileToDataCopy (conn, catType, flagval, -1,
             &nameArray[0], temp, temp1,
             datatype, newpathname, ResourceName, ContainerName, -1);
	} else {
	    /* target a collection */
            status = fileToCollCopy (conn, catType, flagval,
             &nameArray[0], targName->outArgv, datatype, newpathname, 
	     ResourceName, ContainerName);
	}
	return (status);
    }

#ifdef PARA_OPR
    for (i = 0; i < MAX_NUM_BL_THR; i++) {
        Tactive[i] = 0;
        Tstatus[i] = NULL;
    }
#endif
    if (flagval & c_FLAG) {         /* user container */
        myFd = initContainerBload (conn, ContainerName, &curOffset);
    } else {
        myFd = initNormalBload (conn, targName->outArgv);
    }
    if (myFd < 0)  {
        return (myFd);
    }

    myBufInfo.bufSize = BL_BUFSIZE;
    myBufInfo.buf[0] = malloc (BL_BUFSIZE);
#ifdef PARA_OPR
    myBufInfo.buf[1] = malloc (BL_BUFSIZE);
#else
    myBufInfo.buf[1] = NULL;
#endif
    myBufInfo.offset[0] = myBufInfo.offset[1] = 0;
    myBufInfo.status[0] = myBufInfo.status[1] = NOT_READY;
    myBufInfo.curInx = 0;
    myBufInfo.conn = conn;
    myBufInfo.myFd = myFd;

    if (VFlag > 0) {
#ifdef _WIN32
      GetSystemTime(&STimestart);
#else
      (void) gettimeofday(&STimestart, (struct timezone *)0);
#endif
    }

#ifdef PARA_OPR
    pthread_mutex_init(&myBufInfo.condMutex, NULL);
    pthread_cond_init (&myBufInfo.cond, NULL);
#ifdef DBUF
    TranThrInx = allocThrInx ();

    if (TranThrInx < 0) {
      printf (
       "Sbload.c:bloadMain(), Run out of pthreads, TranThrInx=%d\n", 
	TranThrInx);
      return(-3);
    }

    pthread_create(&Tid[TranThrInx], NULL, (void *(*)(void *))transferRoutine,
                  (void *) &myBufInfo);
#endif

    /* Initialize myresult */

    RegThrInx = allocThrInx ();

    if (RegThrInx < 0) {
      printf (
      "Sbload.c:bloadMain(), Run out of pthreads, RegThrInx=%d\n", RegThrInx);
      return(-4);
    }
#else
    RegThrInx = 0;
#endif	/* PARA_OPR */

    Myresult[RegThrInx] = (mdasC_sql_result_struct *) malloc (
     sizeof (mdasC_sql_result_struct));

    initResultStruct (Myresult[RegThrInx], flagval);

    /* Now start copying */

    for (i = 0; i < nArgv; i++) {
	if (nameArray[i].type >= DATANAME_T) {	/* source is data */
	    curSize = nameArray[i].size;
	    status = fileToCollLoad (&myBufInfo, &curOffset, curSize, 
	     nameArray[i].outArgv, targName->outArgv, flagval); 
	} else {	/* source is coll */
            if (nArgv == 1 && checkCollection (myBufInfo.conn, MDAS_CATALOG,
             targName->outArgv)!=0) {
                /* does not exist */
                clSplitbychar (targName->outArgv, temp, temp1, '/');
                status = srbCreateCollect(myBufInfo.conn, catType,
                  temp, temp1);
                if (status < 0) {
                    fprintf(stderr,
                     "Unable to mk the target coll %s, status = %d\n",
                      targName->outArgv, status);
#ifdef PARA_OPR
#ifdef DBUF
                    pthread_cancel (Tid[TranThrInx]);
#endif
#endif
                    return (CLI_ERR_COLLNAME);
                }

    		status = dirToCollLoadCont (&myBufInfo, &curOffset, 
		  nameArray[i].outArgv, targName->outArgv, flagval);
	    } else {
	        status = dirToCollLoad (&myBufInfo, &curOffset, &nameArray[i], 
	         targName->outArgv, flagval);
	    }
	}
    }

    if (Myresult[RegThrInx]->row_count > 0) {
        if ((flagval & k_FLAG) || (flagval & K_FLAG)) 
	    Myresult[RegThrInx]->continuation_index = flagval;
        RegInput[RegThrInx].myresult = Myresult[RegThrInx];
	RegInput[RegThrInx].myInx = RegThrInx;
#ifdef DEBUG
	printf ("RegThrInx = %d\n", RegThrInx);
    	printSqlResult (Myresult[RegThrInx]);
#endif
        if (flagval & c_FLAG) {         /* user container */
            strcpy (RegInput[RegThrInx].container, ContainerName);
#if defined(PARA_OPR) && defined(DBUF)
            dumpBuf (&myBufInfo);
            waitBuf (&myBufInfo);
	    pthread_cancel (Tid[TranThrInx]);
#else
            status = dumpBuf (&myBufInfo);
            if (status < 0) {
                srbObjClose (myBufInfo.conn, myBufInfo.myFd);
                return (status);
            }
#endif
            srbObjClose (myBufInfo.conn, myBufInfo.myFd);

#ifdef PARA_OPR
            pthread_create(&Tid[RegThrInx], NULL, regResult,
              (void *) &RegInput[RegThrInx]);
#else /* PARA_OPR */

            retval = regResult (&RegInput[RegThrInx]);
            if (retval == NULL) {
              fprintf (stderr,
               "bloadMain: regResult return NULL");
              return(-8);
            } else if (*retval < 0) {
              fprintf (stderr,
               "bloadMain: regResult failed. status = %d", *retval);
            }
#endif  /* PARA_OPR */
        } else {        /* normal files */
#if defined(PARA_OPR) && defined(DBUF)
            dumpBuf (&myBufInfo);
            waitBuf (&myBufInfo);        /* wait for I/O to complete */
	    pthread_cancel (Tid[TranThrInx]);

#else
	    status = dumpBuf (&myBufInfo);
	    if (status < 0) {
		srbObjClose (myBufInfo.conn, myBufInfo.myFd);
		return (status);
	    }
#endif
            srbObjClose (myBufInfo.conn, myBufInfo.myFd);
            /* use the container entry to store the the bloadFile */
            sprintf (RegInput[RegThrInx].container, "%s/%s",
             TmpBloadColl, TmpBloadFile);
            /* do a srbBulkLoad */
#ifdef PARA_OPR
            pthread_create(&Tid[RegThrInx], NULL, regAndBload,
                  (void *) &RegInput[RegThrInx]);
#else /* PARA_OPR */
            /* Need to do the bulk load here */
	    status = regAndBloadS (&RegInput[RegThrInx], conn);
	    if (status < 0)
		return (status);
	    else
		status = 0;
#endif
	}
    } else {
	if ((flagval & c_FLAG) == 0) {
            srbObjClose (myBufInfo.conn, myBufInfo.myFd);
            srbObjUnlink (myBufInfo.conn, TmpBloadFile, TmpBloadColl);
	} else {
#if defined(PARA_OPR) && defined(DBUF)
            waitBuf (&myBufInfo);
            pthread_cancel (Tid[TranThrInx]);
#endif
            srbObjClose (myBufInfo.conn, myBufInfo.myFd);
	}
    }

#if defined(PARA_OPR) && defined(DBUF)
    Tactive[TranThrInx] = 0;
#endif
#ifdef PARA_OPR
    status = 0;
    for ( i = 0; i < MAX_NUM_BL_THR; i++) {
	if (Tactive[i] > 0 && Tid[i] != 0) 
            pthread_join (Tid[i], (void **) &Tstatus[i]);
	if (Tstatus[i] != NULL) {
	    if (*Tstatus[i] < 0) {
	        printf ("Thread %d terminated with error. error code = %d\n",
	         i, *Tstatus[i]);
	        status = *Tstatus[i];
	    }
	}
    }
#endif	/* PARA_OPR */

    if (RStatus < 0)
	return RStatus;
    else
	return (0);
}

int
initResultStruct (mdasC_sql_result_struct *myresult, int flagval)
{
    int i;

    memset (myresult, 0, sizeof (mdasC_sql_result_struct));
/*
    for (i = 0; i < MAX_SQL_LIMIT; i++) {
        myresult->sqlresult[i].tab_name = NULL;
        myresult->sqlresult[i].att_name = NULL;
        myresult->sqlresult[i].values = NULL;
    }
*/

    if ((flagval & k_FLAG) || (flagval & K_FLAG)) {
        myresult->result_count = 5;
    } else {
        myresult->result_count = 4;
    }
    myresult->row_count = 0;

    for (i = 0; i < myresult->result_count; i++) {
        myresult->sqlresult[i].values = malloc (MAX_ROW_OUT * MAX_DATA_SIZE);
    }
    myresult->sqlresult[0].tab_name = strdup (dcs_tname[DATA_NAME]);
    myresult->sqlresult[0].att_name = strdup (dcs_aname[DATA_NAME]);
    myresult->sqlresult[1].tab_name = strdup (dcs_tname[DATA_GRP_NAME]);
    myresult->sqlresult[1].att_name = strdup (dcs_aname[DATA_GRP_NAME]);
    myresult->sqlresult[2].tab_name = strdup (dcs_tname[SIZE]);
    myresult->sqlresult[2].att_name = strdup (dcs_aname[SIZE]);
    myresult->sqlresult[3].tab_name = strdup (dcs_tname[OFFSET]);
    myresult->sqlresult[3].att_name = strdup (dcs_aname[OFFSET]);
    if ((flagval & k_FLAG) || (flagval & K_FLAG)) {
	myresult->sqlresult[4].tab_name = strdup (dcs_tname[DATA_CHECKSUM]);
        myresult->sqlresult[4].att_name = strdup (dcs_aname[DATA_CHECKSUM]);
    } 

    return (0);
}

int
fileToCollLoad (struct bufInfo *myBufInfo, srb_long_t *curOffset, 
srb_long_t curSize, char *inFileName, char *outCollection, int flagval)
{
    int in_fd;
    int i;
    char *dataPtr, *collPtr, *sizePtr, *offsetPtr;
    char dataName[MAX_TOKEN], tmpName[MAX_TOKEN];
    srb_long_t toread; 
    int readLen;
    char *bufptr;
    int curInx;
    int status;

    int *retval;

    if (flagval & c_FLAG) {         /* user container */
        if (MaxContSz > 0 && *curOffset > 0 &&
         *curOffset + curSize > MaxContSz) { 
	    status = procOverFContainer (myBufInfo, curOffset, flagval);
	    if (status < 0) {
	        srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	        return (status);
	    }
	}
    }
#ifdef _WIN32
    in_fd = SrbNtFileBinaryOpen(inFileName, O_RDONLY);
#else
    in_fd = open (inFileName, O_RDONLY, 0);
#endif
    if (in_fd < 0) {
        fprintf(stderr,
         "Unable to open local file '%s', errno = %d.\n", inFileName, errno);
        return(in_fd);
    }

    curInx = myBufInfo->curInx;
    toread = curSize;
    bufptr = myBufInfo->buf[curInx] + myBufInfo->offset[curInx];

    while (toread > 0) {
	int space;

	space = myBufInfo->bufSize - myBufInfo->offset[curInx];

	if (toread > space) {
	    if (myBufInfo->offset[curInx] > 0) { 
#ifdef PARA_OPR
		dumpBuf (myBufInfo);
#else
                status = dumpBuf (myBufInfo);
                if (status < 0) {
                    srbObjClose (myBufInfo->conn, myBufInfo->myFd);
                    return (status);
                }
#endif
		curInx = myBufInfo->curInx;
		bufptr = myBufInfo->buf[curInx];
	    }
	    readLen = BL_BUFSIZE;
	} else {
	    readLen = toread;
	}
    
    	i = read(in_fd, bufptr, readLen);
	if (i <= 0) {
	    printf ("fileToCollLoad: problem reading file %s,%lld bytes left\n",
	     inFileName, toread);
	    break;
	}
	toread -= i;
	myBufInfo->offset[curInx] += i;
	bufptr += i;   /* changed mw. 12/28/04 */
    }

    close (in_fd);

    dataPtr = (char *) Myresult[RegThrInx]->sqlresult[0].values + 
     MAX_DATA_SIZE * Myresult[RegThrInx]->row_count;
    collPtr = (char *) Myresult[RegThrInx]->sqlresult[1].values + 
     MAX_DATA_SIZE * Myresult[RegThrInx]->row_count;
    sizePtr = (char *) Myresult[RegThrInx]->sqlresult[2].values + 
     MAX_DATA_SIZE * Myresult[RegThrInx]->row_count;
    offsetPtr = (char *) Myresult[RegThrInx]->sqlresult[3].values + 
     MAX_DATA_SIZE * Myresult[RegThrInx]->row_count;

    if ((flagval & k_FLAG) || (flagval & K_FLAG)) {
	char *chksum;
	unsigned long ulCheckSum;

        chksum = (char *) Myresult[RegThrInx]->sqlresult[4].values +
         MAX_DATA_SIZE * Myresult[RegThrInx]->row_count;
	if (clFileSum (inFileName, &ulCheckSum) < 0) {
            fprintf (stderr,
             "fileToCollLoad(), local file checksum failed\n");
	    status = OBJ_ERR_BAD_CHKSUM;
            srb_perror (2, (int) status, "",
                SRB_RCMD_ACTION | SRB_LONG_MSG);
	    return (status);
	}
	sprintf (chksum, "%d", ulCheckSum);
    }

    clSplitbychar (inFileName, tmpName, dataName, '/');
    strcpy (dataPtr, dataName);
    strcpy (collPtr, outCollection);
    sprintf (sizePtr, "%lld", curSize);
    sprintf (offsetPtr, "%lld", *curOffset);
    *curOffset += curSize;

    if (Myresult[RegThrInx]->row_count >= MAX_ROW_OUT - 1) {
    /* problem if the Max container size is larger than DEF_CONTAINER_SIZE */
/*
    if (Myresult[RegThrInx]->row_count >= MAX_ROW_OUT - 1 ||
     ((flagval & c_FLAG) && (*curOffset >= DEF_CONTAINER_SIZE))) {
*/
        if ((flagval & k_FLAG) || (flagval & K_FLAG)) 
            Myresult[RegThrInx]->continuation_index = flagval;

    	Myresult[RegThrInx]->row_count ++;
	RegInput[RegThrInx].myresult = Myresult[RegThrInx];
	RegInput[RegThrInx].myInx = RegThrInx;
#ifdef DEBUG
	printf ("RegThrInx = %d\n", RegThrInx);
    	printSqlResult (Myresult[RegThrInx]);
#endif

#if defined(PARA_OPR) && defined(DBUF)
        dumpBuf (myBufInfo);
        waitBuf (myBufInfo);        /* wait for I/O to complete */
#else
        status = dumpBuf (myBufInfo);
        if (status < 0) {
            srbObjClose (myBufInfo->conn, myBufInfo->myFd);
            return (status);
        }
#endif

	if (flagval & c_FLAG) {         /* user container */
	    strcpy (RegInput[RegThrInx].container, ContainerName);
#ifdef PARA_OPR
	    pthread_create(&Tid[RegThrInx], NULL, regResult,
              (void *) &RegInput[RegThrInx]);
#else /* PARA_OPR */

            retval = regResult (&RegInput[RegThrInx]);
            if (retval == NULL) {
              fprintf (stderr,
               "fileToCollLoad: regResult return NULL");
              return(-8);
            } else if (*retval < 0) {
              fprintf (stderr,
               "fileToCollLoad: regResult failed. status = %d", *retval);
	    }
#endif  /* PARA_OPR */
	} else {	/* normal files */
	    srbObjClose (myBufInfo->conn, myBufInfo->myFd);
	    /* use the container entry to store the the bloadFile */
	    sprintf (RegInput[RegThrInx].container, "%s/%s",
	     TmpBloadColl, TmpBloadFile);
	    /* do a srbBulkLoad */
#ifdef PARA_OPR
	    pthread_create(&Tid[RegThrInx], NULL, regAndBload,
              (void *) &RegInput[RegThrInx]);
#else
	    /* XXXXX call registration routine here */
            status = regAndBloadS (&RegInput[RegThrInx], myBufInfo->conn);
            if (status < 0)
                return (status);
#endif
	    /* start a new bload */
	    genBloadFileName ();        /* generate a new bload file */
	    *curOffset = 0;
	    myBufInfo->myFd = srbObjCreate (myBufInfo->conn, MDAS_CATALOG, 
	     TmpBloadFile, NULL, ResourceName, TmpBloadColl, NULL, -1);
            if (myBufInfo->myFd < 0)  {
                fprintf (stderr,
                  "fileToCollLoad:unable to create of temp bload file %s/%s.\n",
                 TmpBloadColl, TmpBloadFile);
                fprintf (stdout, "%s", clErrorMessage (myBufInfo->conn));
                srb_perror (2, myBufInfo->myFd, "", 
		 SRB_RCMD_ACTION|SRB_LONG_MSG);
		RStatus = myBufInfo->myFd;
                return (myBufInfo->myFd);
            }
	}
	
    	/* Initialize myresult */

#ifdef PARA_OPR
	RegThrInx = allocThrInx ();

	if (RegThrInx < 0) {
	  printf ("Sbload.c:bloadMain(), Run out of maximum allowed pthreads, RegThrInx=%d\n", RegThrInx);
	  return(-5);
	}
#endif

    	Myresult[RegThrInx] = (mdasC_sql_result_struct *) malloc (
         sizeof (mdasC_sql_result_struct));

    	initResultStruct (Myresult[RegThrInx], flagval);
    } else {
    	Myresult[RegThrInx]->row_count ++;
    }

    return (0);
}

void genBloadFileName ()
{
    sprintf (TmpBloadFile, "%s.%-d", TMP_BLOAD_FILE, random());
}

#ifdef PARA_OPR
void
waitBuf (struct bufInfo *myBufInfo) 
{
    int curInx, ioInx;

    pthread_mutex_lock (&myBufInfo->condMutex);
    curInx = myBufInfo->curInx;
    /* XXXX - This seems to be causing problem myBufInfo->status[curInx] = ALL_DONE; */
    if (curInx == 0)
        ioInx = 1;
    else
        ioInx = 0;

    while (myBufInfo->status[ioInx] == HAVE_DATA) {     /* not done with IO */
        pthread_cond_wait (&myBufInfo->cond, &myBufInfo->condMutex);
    }

    while (myBufInfo->status[curInx] == HAVE_DATA) {     /* not done with IO */
        pthread_cond_wait (&myBufInfo->cond, &myBufInfo->condMutex);
    }


    pthread_mutex_unlock (&myBufInfo->condMutex);

    return;
}
#endif	/* PARA_OPR */

int
dirToCollLoad (struct bufInfo *myBufInfo, srb_long_t *curOffset, 
srbPathName *inPathName, char *outCollection, int flagval)
{
    int status;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    char *curTarg;
    char curCollection[MAX_TOKEN];

    if (inPathName->type != SPEC_COLL_T) {      /* not ".". ".." type */
        clSplitbychar(inPathName->outArgv, temp1, temp,'/');
        sprintf (curCollection, "%s/%s", outCollection, temp);
        curTarg = curCollection;
    } else {
        curTarg = outCollection;
    }

    /* copy the content */
    status = dirToCollLoadCont (myBufInfo, curOffset, inPathName->outArgv, 
     curTarg, flagval);  

    if (status < 0)
        return (CLI_ERR_COLLNAME);

    return 0;
}

int 
dirToCollLoadCont (struct bufInfo *myBufInfo, srb_long_t *curOffset, 
char *inPathName, char *outCollection, int flagval)
{
    int status;
    DIR *dirPtr;
    struct dirent *dp;
    struct stat statbuf;
    char tmpName[MAX_TOKEN], tmpName1[MAX_TOKEN];
    srbPathName mysPathName;
    int dirContentCnt = 0;

    dirPtr = opendir (inPathName);
    if (dirPtr == NULL) {
        fprintf (stderr, "The local dir %s does not exist., errno = %d\n",
         inPathName, errno);
        return UNIX_ENOENT;
    }

    while ((dp = readdir (dirPtr)) != NULL) {
	dirContentCnt ++;
        if (strcmp (dp->d_name, ".") == 0 || strcmp (dp->d_name, "..") == 0)
            continue;
        sprintf (tmpName, "%s/%s", inPathName, dp->d_name);
        status = stat (tmpName, &statbuf);
        if (status != 0) {
            fprintf (stderr, "Unable to stat %s\n", tmpName);
            if (flagval & e_FLAG) {
                closedir (dirPtr);
                return status;
            } else {
                continue;
            }
        }
        mysPathName.inpArgv = tmpName;
        mysPathName.outArgv = tmpName;
        if ((statbuf.st_mode & S_IFREG) != 0) {     /* A file */
            mysPathName.type = DATANAME_T;
            mysPathName.size = statbuf.st_size;
            status = fileToCollLoad (myBufInfo, curOffset, statbuf.st_size,
	     mysPathName.outArgv, outCollection, flagval);
	     
            if (status < 0) {
                if (flagval & e_FLAG) {
                    closedir (dirPtr);
                    return status;
                } else {
                    continue;
                }
            }
        } else if ((statbuf.st_mode & S_IFDIR) != 0) {      /* A directory */
            mysPathName.type = COLLECTION_T;
            mysPathName.size = 0;

	    /* sprintf (curCollection, "%s/%s", outCollection, dp->d_name); */
            status = dirToCollLoad (myBufInfo, curOffset, &mysPathName, 
	     outCollection, flagval);
            if (status < 0) {
                if (flagval & e_FLAG) {
                    closedir (dirPtr);
                    return status;
                } else {
                    continue;
                }
            }
        }
    }
    closedir (dirPtr);
    if (dirContentCnt <= 2) {
	/* empty directory */
	clSplitbychar (outCollection, tmpName, tmpName1, '/');
	 myCreateCollect (myBufInfo->conn, MDAS_CATALOG, tmpName, tmpName1, 0);
    }

    return (0);
}


void *
regResult (void *myInput)
{
    struct regInput *regInput;
    char *mcatHost, *myHost;
    srbConn *conn;
    int *retval;
#ifdef _WIN32
    SYSTEMTIME sTimestart, sTimestop;
#else
    struct timeval sTimestart, sTimestop, sTimetd;
#endif
    float fSec;
    int row_count;

	
    if (VFlag > 0) {
#ifdef _WIN32
      GetSystemTime(&sTimestart);
#else
      (void) gettimeofday(&sTimestart, (struct timezone *)0);
#endif
    }

    regInput = (struct regInput *) myInput;
    if ((mcatHost = getenv ("MCAT_HOST")) != NULL) {
	myHost = mcatHost;
    } else {
	myHost = srbHost;
    }

    retval = (int *) malloc (sizeof (int));

    if (RStatus < 0) {
	*retval = RStatus;
        return ((void *) retval);
    }
	
    *retval = srbConnectWithRetry (&conn, myHost, srbAuth);

    if (*retval < 0) {
	fprintf(stderr,
          "regResult: Failed to connect after retrying %d times.\n",
	  MAX_CONN_CNT);
	if (RStatus == 0)
	    RStatus = *retval;
	return ((void *) retval);
    }

    *retval = srbBulkRegister (conn, MDAS_CATALOG, regInput->container,
     regInput->myresult, 0);

    row_count = regInput->myresult->row_count;
    freeSqlResult (regInput->myresult);
    clFinish(conn);

    if (VFlag > 0) {
#ifdef _WIN32
      GetSystemTime(&sTimestop);
      fSec = abs((float)sTimestart.wSecond - (float)sTimestop.wSecond) +
          abs((float)sTimestart.wMilliseconds - (float)sTimestop.wMilliseconds/1000.0);
#else
      (void) gettimeofday(&sTimestop, (struct timezone *)0);
      (void)tvsub(&sTimetd, &sTimestop, &sTimestart);
        fSec = (float)sTimetd.tv_sec + ((float)sTimetd.tv_usec / 1000000.);
#endif
	printf ("time to register %d rows = %f\n", row_count, fSec);

    }

    if (*retval < 0) {
        fprintf(stderr,"regResult: srbBulkRegister error, status = %d\n",
	 *retval);
	if (RStatus >= 0)
	    RStatus = *retval;
    }

    Tactive[regInput->myInx] = 0;

    return ((void *) retval);
}

void *
regAndBload (void *myInput)
{
    struct regInput *regInput;
    char *myHost;
    srbConn *conn;
    int *retval;
#ifdef _WIN32
    SYSTEMTIME sTimestart, sTimestop;
#else
    struct timeval sTimestart, sTimestop, sTimetd;
#endif
    float fSec;
    int row_count;

    if (VFlag > 0) {
#ifdef _WIN32
      GetSystemTime(&sTimestart);
#else
      (void) gettimeofday(&sTimestart, (struct timezone *)0);
#endif
    }

    regInput = (struct regInput *) myInput;

    /* XXXXX use srbHost for now. */
    myHost = srbHost;

    retval = (int *) malloc (sizeof (int));

    *retval = srbConnectWithRetry (&conn, myHost, srbAuth);

    if (*retval < 0) {
        fprintf(stderr,
          "regAndBload: Failed to connect after retrying %d times.\n",
          MAX_CONN_CNT);
        if (RStatus == 0)
            RStatus = *retval;
        return ((void *) retval);
    }

    *retval = srbBulkLoad (conn, MDAS_CATALOG, regInput->container,
     regInput->myresult);

    row_count = regInput->myresult->row_count;

    freeSqlResult (regInput->myresult);
    clFinish(conn);

    if (VFlag > 0) {
#ifdef _WIN32
      GetSystemTime(&sTimestop);
      fSec = abs((float)sTimestart.wSecond - (float)sTimestop.wSecond) +
             abs((float)sTimestart.wMilliseconds - 
	     (float)sTimestop.wMilliseconds/1000.0);
#else
      (void) gettimeofday(&sTimestop, (struct timezone *)0);
      (void)tvsub(&sTimetd, &sTimestop, &sTimestart);
        fSec = (float)sTimetd.tv_sec + ((float)sTimetd.tv_usec / 1000000.);
#endif
        printf ("time to bload %d rows = %f\n", row_count, fSec);

    }

    if (*retval < 0) {
        fprintf(stderr,"regAndBload: srbBulkLoad error, status = %d\n",
         *retval);
        if (RStatus >= 0)
            RStatus = *retval;
    }

    Tactive[regInput->myInx] = 0;

    return ((void *) retval);
}

#if defined(PARA_OPR) && defined(DBUF)
int
dumpBuf (struct bufInfo *myBufInfo)
{
    int curInx, ioInx;

    curInx = myBufInfo->curInx;

    if (myBufInfo->offset[curInx] <= 0)
	return 0;

    pthread_mutex_lock (&myBufInfo->condMutex);
    myBufInfo->status[curInx] = HAVE_DATA;
    if (curInx == 0)
	ioInx = 1;
    else
	ioInx = 0;

    while (myBufInfo->status[ioInx] == HAVE_DATA) {	/* not done with IO */
	pthread_cond_wait (&myBufInfo->cond, &myBufInfo->condMutex);
    }
    
    myBufInfo->offset[ioInx] = 0;
    myBufInfo->curInx = ioInx;
    pthread_cond_signal (&myBufInfo->cond);
    pthread_mutex_unlock (&myBufInfo->condMutex);

    return 0;
}

void 
transferRoutine (struct bufInfo *myBufInfo)
{
    int status;
#ifdef _WIN32
    SYSTEMTIME sTimestart, sTimestop;
#else
    struct timeval sTimestart, sTimestop, sTimetd;
#endif
    float fSec;
    int curInx, ioInx;


    while (1) {

	pthread_mutex_lock (&myBufInfo->condMutex);

	curInx = myBufInfo->curInx;

    	if (curInx == 0)
            ioInx = 1;
    	else
            ioInx = 0;

	if (myBufInfo->status[ioInx] == ALL_DONE)
	    return;

    	if (myBufInfo->status[ioInx] != HAVE_DATA) {
	    pthread_cond_wait (&myBufInfo->cond, &myBufInfo->condMutex);
	    pthread_mutex_unlock (&myBufInfo->condMutex);
	    continue;
	}
	/* tighten up lock to see if it solve the load problem with 
	 * mac OS 2/15/05 mw */	
	/* pthread_mutex_unlock (&myBufInfo->condMutex); */
	    

    	status = srbObjWrite (myBufInfo->conn, myBufInfo->myFd, 
    	 myBufInfo->buf[ioInx], myBufInfo->offset[ioInx]);

    	if (status != myBufInfo->offset[ioInx]) {
	    if (status < 0 && RStatus >= 0) 
		RStatus = status;
	    printf ("dumpBuf: Try to write %d bytes, %d bytes written\n",
	     myBufInfo->offset[ioInx], status);
    	}
        /* tighten up lock to see if it solve the load problem with
         * mac OS 2/15/05 mw */
	/* pthread_mutex_lock (&myBufInfo->condMutex); */
    	myBufInfo->offset[ioInx] = 0;
	myBufInfo->status[ioInx] = DONE_IO;
	pthread_cond_signal (&myBufInfo->cond);
	if (RStatus < 0)
	    return;
	pthread_mutex_unlock (&myBufInfo->condMutex);
    }
}
#else /* PARA_OPR and DBUF */
int 
dumpBuf (struct bufInfo *myBufInfo)
{
    int status;
#ifdef _WIN32
    SYSTEMTIME sTimestart, sTimestop;
#else
    struct timeval sTimestart, sTimestop, sTimetd;
#endif
    float fSec;
    int ioInx;

    ioInx = 0;

    if (myBufInfo->offset[ioInx] <= 0)
	return (0);

    if (VFlag > 0) {
#ifdef _WIN32
        GetSystemTime(&sTimestart);
#else
        (void) gettimeofday(&sTimestart, (struct timezone *)0);
#endif
    }

    status = srbObjWrite (myBufInfo->conn, myBufInfo->myFd, 
     myBufInfo->buf[ioInx], myBufInfo->offset[ioInx]);

    if (VFlag > 0) {
#ifdef _WIN32
        GetSystemTime(&sTimestop);
        fSec = abs((float)sTimestart.wSecond - (float)sTimestop.wSecond) +
               abs((float)sTimestart.wMilliseconds - (float)sTimestop.wMilliseconds/1000.0);
#else
         (void) gettimeofday(&sTimestop, (struct timezone *)0);
      	(void)tvsub(&sTimetd, &sTimestop, &sTimestart);
        fSec = (float)sTimetd.tv_sec + ((float)sTimetd.tv_usec / 1000000.);
#endif
        printf ("time to transfer %d bytes = %f\n", 
	 myBufInfo->offset[ioInx], fSec);
    }

    if (status != myBufInfo->offset[ioInx]) {
        fprintf (stderr, "dumpBuf: Try to write %d bytes, %d bytes written\n",
	  myBufInfo->offset[ioInx], status);
	if (status >= 0)
	    status = -1;
	RStatus = status;
	return (status);
    }
    myBufInfo->curInx = 0;
    myBufInfo->offset[ioInx] = 0;
    myBufInfo->status[ioInx] = DONE_IO;

    return (0);
}
#endif /* PARA_OPR and DBUF */

#ifdef PARA_OPR
int
getNumActThr ()
{
    int i;
    int numActThr = 0;

    for (i = 0; i < MAX_NUM_BL_THR; i++) {
	if (Tactive[i] > 0)
	    if (Tid[i] == 0) {
		Tactive[i] = 0;
	    } else
	        numActThr ++;
    }
#ifdef DEBUG
    printf ("numActThr = %d\n", numActThr);
#endif
    return (numActThr);
}


int 
allocThrInx ()
{
    int numThreads;
    int i, iTotalErr=0;

    while ((numThreads = getNumActThr ())  > MAX_ACT_REG_THREAD) {
      srbThreadSleep ((int) THREAD_SLEEP_TIME, 0); 
      /* too many running pthreads, wait for some to finish */
      iTotalErr++;
      if (iTotalErr > (int)(MAX_RETRY_SEC / THREAD_SLEEP_TIME)) {
        fprintf (stderr,
         "allocThrInx(), Retry for %d secs. Going ahead with %d threads\n",
                (int)(MAX_RETRY_SEC), numThreads + 1);
        break;       
      }
    } 

    for (i = CurThrInx + 1; i < MAX_NUM_BL_THR; i++) {
	if (Tactive[i] == 0) {
	    Tactive[i] = 1;
	    CurThrInx = i;
	    return (i);
	}
    }

    for (i = 0; i < CurThrInx; i++) {
        if (Tactive[i] == 0) {
            Tactive[i] = 1;
            CurThrInx = i;
            return (i);
        }
    }

/*
    for (i = 0; i < MAX_NUM_BL_THR; i++) {
	if (Tactive[i] == 0) {
	    Tactive[i] = 1;
	    return (i);
	}
    }
*/

    printf ("Runs out of thread\n");

    return (-1);
}
#endif	/* PARA_OPR */

int
procOverFContainer (struct bufInfo *myBufInfo, srb_long_t *curOffset, 
int flagval)
{
    char contColl[MAX_TOKEN], contName[MAX_TOKEN], newContName[MAX_TOKEN];
    int status;
    int i;

#ifdef PARA_OPR
    dumpBuf (myBufInfo);
#else
    int *retval;

    status = dumpBuf (myBufInfo);
    if (status < 0) {
        srbObjClose (myBufInfo->conn, myBufInfo->myFd);
        return (status);
    }
#endif

    /* register whatever in the container */
    if (Myresult[RegThrInx]->row_count > 0) {
        RegInput[RegThrInx].myresult = Myresult[RegThrInx];
	RegInput[RegThrInx].myInx = RegThrInx;
        strcpy (RegInput[RegThrInx].container, ContainerName);
#ifdef DEBUG
        printSqlResult (Myresult[RegThrInx]);
#endif

#ifdef PARA_OPR
        pthread_create(&Tid[RegThrInx], NULL, regResult,
          (void *) &RegInput[RegThrInx]);

        /* Initialize myresult */

        RegThrInx = allocThrInx ();

        if (RegThrInx < 0) {
          fprintf (stderr,
           "procOverFContainer(), Run out of maximum allowed pthreads, RegThrInx=%d\n", RegThrInx);
          return(-7);
        }
#else
	retval = regResult (&RegInput[RegThrInx]);
	if (retval == NULL) {
          fprintf (stderr,
           "procOverFContainer: regResult return NULL");
          return(-8);
        } else if (*retval < 0) {
          fprintf (stderr,
           "procOverFContainer: regResult failed. status = %d", *retval);
	}
#endif

        Myresult[RegThrInx] = (mdasC_sql_result_struct *) malloc (
         sizeof (mdasC_sql_result_struct));

        initResultStruct (Myresult[RegThrInx], flagval);
    }

#ifdef PARA_OPR
    /* wait for everthing done */

    pthread_mutex_lock (&myBufInfo->condMutex);
    while (myBufInfo->status[0] == HAVE_DATA ||
     myBufInfo->status[1] == HAVE_DATA) {     /* not done with IO */
        pthread_cond_wait (&myBufInfo->cond, &myBufInfo->condMutex);
    }

    pthread_mutex_unlock (&myBufInfo->condMutex);

    for ( i = 0; i < MAX_NUM_BL_THR; i++) {
#ifdef DBUF
        if (i != TranThrInx && Tactive[i] > 0)
#else
        if (Tactive[i] > 0)
#endif
            pthread_join (Tid[i], (void **) &Tstatus[i]);
        if (Tstatus[i] != NULL) {
            if (*Tstatus[i] < 0) {
                printf ("Thread %d terminated with error. error code = %d\n",
                 i, *Tstatus[i]);
                status = *Tstatus[i];
            }
        }
    }
#endif

    myBufInfo->offset[0] = myBufInfo->offset[1] = 0;

    srbObjClose (myBufInfo->conn, myBufInfo->myFd);

    clSplitbychar (ContainerName, contColl, contName, '/');

    sprintf (newContName, "%s.%-d", contName, time (NULL));
    /* rename the current container */
    status =  srbModifyDataset (myBufInfo->conn, MDAS_CATALOG,
      contName, contColl,
     "", "", newContName, NULL, D_CHANGE_DNAME);

    if (status < 0) {
	fprintf (stderr, "srbModifyDataset from %s to %s failed\n",
	  contName, newContName);
	return (status);
    }
    status = srbContainerCreate (myBufInfo->conn, MDAS_CATALOG, ContainerName, 
     "", ResourceName, MaxContSz);
    if (status < 0) {
	fprintf (stderr, "srbContainerCreate of %s failed\n",
	 ContainerName);
	return (status);
    }

    /*** open the container as a file ***/
    myBufInfo->myFd = srbObjOpen(myBufInfo->conn, contName, O_RDWR, 
     contColl);
    if (myBufInfo->myFd < 0)  {
        printf ("procOverFContainer: Failure opening SRB container %s. \n",
         ContainerName);
        fprintf (stdout, "%s", clErrorMessage (myBufInfo->conn));
        return (myBufInfo->myFd);
    }
    *curOffset = 0;
    ContainerInx++;

    return (0);
}

int 
dumpBufS (struct bufInfo *myBufInfo)
{
    int status;
#ifdef _WIN32
    SYSTEMTIME sTimestart, sTimestop;
#else
    struct timeval sTimestart, sTimestop, sTimetd;
#endif
    float fSec;
    int ioInx;

    ioInx = 0;

    if (myBufInfo->offset[ioInx] <= 0)
	return (0);

    if (VFlag > 0) {
#ifdef _WIN32
        GetSystemTime(&sTimestart);
#else
        (void) gettimeofday(&sTimestart, (struct timezone *)0);
#endif
    }

    status = srbObjWrite (myBufInfo->conn, myBufInfo->myFd, 
     myBufInfo->buf[ioInx], myBufInfo->offset[ioInx]);

    if (VFlag > 0) {
#ifdef _WIN32
        GetSystemTime(&sTimestop);
        fSec = abs((float)sTimestart.wSecond - (float)sTimestop.wSecond) +
               abs((float)sTimestart.wMilliseconds - (float)sTimestop.wMilliseconds/1000.0);
#else
         (void) gettimeofday(&sTimestop, (struct timezone *)0);
      	(void)tvsub(&sTimetd, &sTimestop, &sTimestart);
        fSec = (float)sTimetd.tv_sec + ((float)sTimetd.tv_usec / 1000000.);
#endif
        printf ("time to transfer %d bytes = %f\n", 
	 myBufInfo->offset[ioInx], fSec);
    }

    if (status != myBufInfo->offset[ioInx]) {
        fprintf (stderr, "dumpBuf: Try to write %d bytes, %d bytes written\n",
	  myBufInfo->offset[ioInx], status);
	if (status < 0)
	    return status;
	else
	    return -1;
	    
    }
    myBufInfo->curInx = 0;
    myBufInfo->offset[ioInx] = 0;
    myBufInfo->status[ioInx] = DONE_IO;

    return (0);
}

#ifndef	PARA_OPR
int
regAndBloadS (struct regInput *myInput, srbConn *conn)
{
    struct regInput *regInput;
    char *mcatHost, *myHost;
    int status;
#ifdef _WIN32
    SYSTEMTIME sTimestart, sTimestop;
#else
    struct timeval sTimestart, sTimestop, sTimetd;
#endif
    float fSec;
    int row_count;

    if (VFlag > 0) {
#ifdef _WIN32
      GetSystemTime(&sTimestart);
#else
      (void) gettimeofday(&sTimestart, (struct timezone *)0);
#endif
    }

    regInput = (struct regInput *) myInput;

    /* XXXXX use srbHost for now. */
    myHost = srbHost;

    status = srbBulkLoad (conn, MDAS_CATALOG, regInput->container,
     regInput->myresult);

    row_count = regInput->myresult->row_count;

    freeSqlResult (regInput->myresult);

    if (VFlag > 0) {
#ifdef _WIN32
      GetSystemTime(&sTimestop);
      fSec = abs((float)sTimestart.wSecond - (float)sTimestop.wSecond) +
             abs((float)sTimestart.wMilliseconds - (float)sTimestop.wMilliseconds/1000.0);
#else
      (void) gettimeofday(&sTimestop, (struct timezone *)0);
      (void)tvsub(&sTimetd, &sTimestop, &sTimestart);
        fSec = (float)sTimetd.tv_sec + ((float)sTimetd.tv_usec / 1000000.);
#endif
        printf ("time to bload %d rows = %f\n", row_count, fSec);

    }

    if (status < 0) {
        fprintf(stderr,"regAndBloadS: srbBulkLoad error, status = %d\n",
         status);
    }
    Tactive[regInput->myInx] = 0;

    return (status);
}
#endif	/* PARA_OPR */

int
srbConnectWithRetry (srbConn **conn, char *myHost, char *myAuth)
{
    int status;
    int connectCnt = 0;

    while (connectCnt < MAX_CONN_CNT) {
        if (connectCnt > 0)
            srbThreadSleep ((int) THREAD_SLEEP_TIME, 0);
        *conn = srbConnect (myHost, NULL, myAuth,
         NULL, NULL, NULL, NULL);
        if (clStatus(*conn) != CLI_CONNECTION_OK) {
            fprintf(stderr,
             "srbConnectWithRetry: Connection to srbMaster failed. Retrying\n");
            fprintf(stderr,"srbConnectWithRetry: %s",clErrorMessage(*conn));
            srb_perror (2, clStatus(*conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            if (clStatus(*conn) >= 0)
                status = -1;
            else
                status = clStatus(*conn);
            connectCnt ++;
        } else {
            status = 0;
            break;
        }
    }
    if (status < 0)
	*conn = NULL;

    return status;
}

int 
initContainerBload (srbConn *conn, char *containerName, srb_long_t *curOffset)
{
    char contColl[MAX_TOKEN], contName[MAX_TOKEN];
    int myFd;
    int status;
    char *containerMaxSize;
    mdasC_sql_result_struct myresult;

    clSplitbychar (containerName, contColl, contName, '/');

    /*** open the container as a file ***/
    myFd = srbObjOpen(conn, contName, O_RDWR, contColl);
    if (myFd < 0)  {
        fprintf (stderr, 
	  "initContainerBload: Failure opening SRB container %s. \n",
         containerName);
        fprintf (stdout, "%s", clErrorMessage (conn));
        return (myFd);
    }
    *curOffset = srbObjSeek(conn, myFd, 0, SEEK_END);
    if (*curOffset < 0) {
        fprintf (stderr,
         "initContainerBload: Failure seeking in SRB container %s :%lld \n",
          containerName, *curOffset);
        fprintf (stdout, "%s", clErrorMessage (conn));
        srbObjClose (conn, myFd);
	return (-1);
    }
    /* set MaxContSz */
    status = srbGetContainerInfo (conn, MDAS_CATALOG,
     ContainerName, &myresult, MAX_GET_RESULTS);

    if (status < 0) {
        fprintf (stderr, "srbGetContainerInfo failed for %s\n",
          ContainerName);
        srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        srbObjClose (conn, myFd);
    }

    containerMaxSize = (char *) getFromResultStruct(
     (mdasC_sql_result_struct *) &myresult,
        dcs_tname[CONTAINER_MAX_SIZE], dcs_aname[CONTAINER_MAX_SIZE]);
    MaxContSz = strtoll (containerMaxSize, 0, 0);
    clearSqlResult (&myresult);

    return myFd;
}

int 
initNormalBload (srbConn *conn, char *collection)
{
    int myFd;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];

    if (checkCollection (conn, MDAS_CATALOG, collection) != 0) {
        clSplitbychar (collection, temp, temp1, '/');
        strcpy (TmpBloadColl, temp);
    } else {
        strcpy (TmpBloadColl, collection);
    }
    genBloadFileName ();        /* generate a new bload file */
    myFd = srbObjCreate (conn, MDAS_CATALOG, TmpBloadFile, NULL,
     ResourceName, TmpBloadColl, NULL, -1);
    if (myFd < 0)  {
        fprintf (stderr,
          "initNormalBload: unable to create of temp bload file %s/%s.\n",
         TmpBloadColl, TmpBloadFile);
        fprintf (stdout, "%s", clErrorMessage (conn));
        srb_perror (2, myFd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        return (myFd);
    }
    return myFd;
}

