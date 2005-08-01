/* Sbregister.c 
 */

#include "scommands.h"
#ifndef _WIN32
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
/* #include <wordexp.h>*/
#include <glob.h>
#endif

#if defined(PORTNAME_linux)
#include <sys/timex.h>
#endif /* PORTNAME_linux */
#include <sys/stat.h>

#define MAX_ROW_OUT	1000
#define MAX_NUM_BL_THR 1024
#define MAX_ACT_REG_THREAD 3

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
    int cont_fd;
    srb_long_t contOffset;
    srbConn *conn;
#ifdef PARA_OPR
    pthread_mutex_t condMutex;
    pthread_cond_t cond;
#endif
};

/*
#define DEBUG
*/

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

char ContainerName[MAX_TOKEN];
char OrigContainerName[MAX_TOKEN];
char ResourceName[MAX_TOKEN];
char InfoFileName[MAX_TOKEN];
#ifdef PARA_OPR
pthread_t Tid[MAX_NUM_BL_THR];
#endif
int *Tstatus[MAX_NUM_BL_THR];
int Tactive[MAX_NUM_BL_THR];
int RegThrInx;
int TranThrInx;
int ActRegThrInx;
int ContainerInx = 0;
srb_long_t MaxContSz = -1;	/* max container sz in MB) */
mdasC_sql_result_struct  *Myresult[MAX_NUM_BL_THR];
struct regInput RegInput[MAX_NUM_BL_THR];
int VFlag = 0;

#ifdef _WIN32
SYSTEMTIME STimestart, STimestop;
#else
struct timeval STimestart, STimestop, STimetd;
#endif

void usage (char *prog);
void *regResult (void *myInput);
void transferRoutine (struct bufInfo *myBufInfo);
int
bregisterMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
srbPathName *targName);
int
bregisterFromInfoFile(srbConn *conn, int catType, srbPathName *targName,
char *infoFileName);
int
allocThrInx ();
int
initResultStruct (mdasC_sql_result_struct *myresult);
int
fileToCollRegister (struct bufInfo *myBufInfo, srb_long_t *curOffset,
                    srb_long_t *curSize,
                    char *inFileName, char *outCollection,
                    char *dataType, char *inDataName);
int
getNumActThr ();
int
dirToCollRegisterCont (struct bufInfo *myBufInfo, srb_long_t *curOffset,
char *inPathName, char *outCollection);
int
dirToCollRegister (struct bufInfo *myBufInfo, srb_long_t *curOffset,
srbPathName *inPathName, char *outCollection);



int
main(int argc, char **argv)
{
    int c, ii;
    srbPathName nameArray[MAX_TOKEN], targName;
    int nArgv;
    srbConn *conn;
    /* wordexp_t pwordexp;*/

    int fFlag = 0;

    strcpy(inCondition , "");
    if (argc < 3) {
	usage (argv[0]);
	exit(1);
    }

    ResourceName[0] = '\0';
    InfoFileName[0] = '\0';
    while ((c=getopt(argc, argv,"S:f:v")) != EOF) {
	switch (c) {
            case 'v':                   /* verbose flag */
                VFlag = 1;
                break;
            case 'S':                   /* verbose flag */
		strcpy (ResourceName, optarg);
                break;
            case 'f':                   /* verbose flag */
		strcpy (InfoFileName, optarg);
                break;
	    default:
		usage (argv[0]);
		exit (1);
	}
    }
    fFlag = strlen(InfoFileName);
    if (fFlag == 0) {
      if (argc - optind < 1) {	/* must have at least 2 input */
        usage (argv[0]);
        exit (1);
      }
    } else {
      if (argc - optind != 0) {	/* must have exactly 1 input */
        usage (argv[0]);
        exit (1);
      }
    }


    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}
    
    /* Process the input */
    

    /* Set up the source and target arrays */

    targName.inpArgv = argv[argc-1];
    if (fFlag == 0) {
      for (i = optind; i < argc - 1; i++) {      
	nameArray[i - optind].inpArgv = argv[i]; 
      }
      nArgv = argc - optind - 1;	/* nArgv is the number of in file */
    }
    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sbregister: Connection to srbMaster failed.\n");
        fprintf(stderr,"Sbregister: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    /* Parse the target name */
    if (parseName (&targName, mdasCollectionName) < 0)
      {clFinish(conn); exit(4);}
    
    for (i = 0; i < MAX_NUM_BL_THR; i++) {
	Tactive[i] = 0;
    }

    if (fFlag == 0) {
      if (ResourceName == NULL || strlen (ResourceName) == 0) {
	get_user_preference(ResourceName, "DRSRC");
      }
      strcpy(ContainerName,ResourceName);
      strcpy(OrigContainerName,ResourceName);
      
      ii = bregisterMain (conn, MDAS_CATALOG, nArgv, nameArray, &targName);

    }
    else {
      ii = bregisterFromInfoFile(conn, MDAS_CATALOG,&targName,InfoFileName);
    }
    clFinish(conn);
    if (ii == 0)
      exit(0);
    else {
      fprintf (stderr, "Sbregister error\n");
      srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      exit(1);
    }
}

int
bregisterFromInfoFile(srbConn *conn, int catType, srbPathName *targName,
char *infoFileName)
{
      int i;
    int status;
    srb_long_t curSize; 
    int cont_fd = 0;
    srb_long_t curOffset;
    struct bufInfo myBufInfo;
    FILE *infoFd;
    char inBuf[4000];
    char *tmpPtr, *tmpPtr1;
    char *dataType = NULL;
    char *dataName = NULL;
    char *pathName = NULL;
    char *collName = NULL;
    myBufInfo.bufSize = BL_BUFSIZE;
    myBufInfo.offset[0] = myBufInfo.offset[1] = 0;
    myBufInfo.status[0] = myBufInfo.status[1] = NOT_READY;
    myBufInfo.curInx = 0;
    myBufInfo.conn = conn;
    myBufInfo.cont_fd = cont_fd;

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
#endif

    TranThrInx = allocThrInx ();

    /* Initialize myresult */

    RegThrInx = allocThrInx ();

    Myresult[RegThrInx] = (mdasC_sql_result_struct *) malloc (
     sizeof (mdasC_sql_result_struct));

    initResultStruct (Myresult[RegThrInx]);
    
    if (!strcmp(infoFileName,"-"))
      infoFd = stdin;
    else {
      infoFd = fopen(infoFileName,"r");
      if (infoFd == NULL) {
	fprintf(stderr, "Sbregister: can't open local file \"%s\"\n",infoFileName);
	return(INP_ERR_FILENAME);
      }
    }
    if (fgets(ResourceName,MAX_TOKEN - 1, infoFd) == NULL )
      return(EXPRESSION_SYNTAX_ERROR);
    ResourceName[strlen(ResourceName)-1] ='\0';
    strcpy(ContainerName,ResourceName);
    strcpy(OrigContainerName,ResourceName);

    while (fgets(inBuf, HUGE_STRING - 1, infoFd) != NULL) {
      if (strstr(inBuf,"<<END>>") != NULL)
	break;
      if (inBuf[0] == '/' && inBuf[1] == '/')
	continue;
      tmpPtr = inBuf;
      if ((tmpPtr1 = strstr(tmpPtr,"|")) == NULL) {
	fprintf(stderr,"Error in Expression\n");
	fprintf(stderr,"Beginning of String : %s\n",inBuf);
	fprintf(stderr,"Error Part of String: %s\n",tmpPtr);
	return(EXPRESSION_SYNTAX_ERROR);
      }
      *tmpPtr1 = '\0';
      dataName = tmpPtr;
      tmpPtr = tmpPtr1 + 1;
      if ((tmpPtr1 = strstr(tmpPtr,"|")) == NULL) {
	fprintf(stderr,"Error in Expression\n");
	fprintf(stderr,"Beginning of String : %s\n",inBuf);
	fprintf(stderr,"Error Part of String: %s\n",tmpPtr);
	return(EXPRESSION_SYNTAX_ERROR);
      }
      *tmpPtr1 = '\0';
      collName = tmpPtr;
      tmpPtr = tmpPtr1 + 1;
      if ((tmpPtr1 = strstr(tmpPtr,"|")) == NULL) {
	fprintf(stderr,"Error in Expression\n");
	fprintf(stderr,"Beginning of String : %s\n",inBuf);
	fprintf(stderr,"Error Part of String: %s\n",tmpPtr);
	return(EXPRESSION_SYNTAX_ERROR);
      }
      *tmpPtr1 = '\0';
      curSize = atol(tmpPtr);
      tmpPtr = tmpPtr1 + 1;
      if ((tmpPtr1 = strstr(tmpPtr,"|")) == NULL) {
	fprintf(stderr,"Error in Expression\n");
	fprintf(stderr,"Beginning of String : %s\n",inBuf);
	fprintf(stderr,"Error Part of String: %s\n",tmpPtr);
	return(EXPRESSION_SYNTAX_ERROR);
      }
      *tmpPtr1 = '\0';
      dataType = tmpPtr;
      tmpPtr = tmpPtr1 + 1;
      pathName = tmpPtr;
      pathName[strlen(pathName)-1] ='\0';
      status = fileToCollRegister (&myBufInfo, &curOffset, &curSize,
				   pathName, collName, dataType,dataName);
      if (status < 0)
	return (status);
    }
    if (Myresult[RegThrInx]->row_count > 0) {
        RegInput[RegThrInx].myresult = Myresult[RegThrInx];
	RegInput[RegThrInx].myInx = RegThrInx;
        strcpy (RegInput[RegThrInx].container,  ContainerName);
#ifdef DEBUG
    	printSqlResult (Myresult[RegThrInx]);
#endif

#ifdef PARA_OPR
	while (getNumActThr () > MAX_ACT_REG_THREAD) {
	    srbThreadSleep (2, 0);
	}
        status = pthread_create(&Tid[RegThrInx], NULL,
          (void *(*)(void *)) regResult, 
	   (void *) &RegInput[RegThrInx]);
#endif
    }

    status = 0;
#ifdef PARA_OPR
    for ( i = 1; i < MAX_NUM_BL_THR; i++) {
	if (Tactive[i] > 0)
            pthread_join (Tid[i], (void **) &Tstatus[i]);
	if (Tstatus[i] != NULL) {
	    if (*Tstatus[i] < 0) {
	        printf ("Thread %d terminated with error. error code = %d\n",
	         i, *Tstatus[i]);
	        status = *Tstatus[i];
	    }
	    else {
	      if (VFlag > 0) {
		printf ("Thread %d terminated successfully.\n",i);
	      }
	    }
	}
    }
#endif


    return (status);
}

 

int 
bregisterMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[], 
srbPathName *targName)
{
    int i;
    int status;
    int dataCnt = 0;
    int collCnt = 0;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    srb_long_t curSize; 
    int cont_fd = 0;
    srb_long_t curOffset;
    struct bufInfo myBufInfo;
    char *dataType = NULL;
    char *dataName = NULL;

    myBufInfo.bufSize = BL_BUFSIZE;
    myBufInfo.offset[0] = myBufInfo.offset[1] = 0;
    myBufInfo.status[0] = myBufInfo.status[1] = NOT_READY;
    myBufInfo.curInx = 0;
    myBufInfo.conn = conn;
    myBufInfo.cont_fd = cont_fd;

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
#endif

    TranThrInx = allocThrInx ();

    /* Initialize myresult */

    RegThrInx = allocThrInx ();

    Myresult[RegThrInx] = (mdasC_sql_result_struct *) malloc (
     sizeof (mdasC_sql_result_struct));

    initResultStruct (Myresult[RegThrInx]);

    for (i = 0; i < nArgv; i++) {
	status = chkFileName (&nameArray[i]);
    	if (status < 0) {
            fprintf (stderr, "Source %s not found\n", nameArray[i].inpArgv);
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
    }

    /* Now start copying */

    for (i = 0; i < nArgv; i++) {
	if (nameArray[i].type >= DATANAME_T) {	/* source is data */
	    curSize = nameArray[i].size;
	    status = fileToCollRegister (&myBufInfo, &curOffset, &curSize, 
	     nameArray[i].outArgv, targName->outArgv,dataType,dataName); 
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
                    return (CLI_ERR_COLLNAME);
                }

    		status = dirToCollRegisterCont (&myBufInfo, &curOffset, 
		  nameArray[i].outArgv, targName->outArgv);
	    } else {
	        status = dirToCollRegister (&myBufInfo, &curOffset, &nameArray[i], 
	         targName->outArgv);
	    }
	}
    }

    if (Myresult[RegThrInx]->row_count > 0) {
        RegInput[RegThrInx].myresult = Myresult[RegThrInx];
	RegInput[RegThrInx].myInx = RegThrInx;
        strcpy (RegInput[RegThrInx].container,  ContainerName);
#ifdef DEBUG
    	printSqlResult (Myresult[RegThrInx]);
#endif
#ifdef PARA_OPR
        while (getNumActThr () > MAX_ACT_REG_THREAD) {
            srbThreadSleep (2, 0);
        }
        status = pthread_create(&Tid[RegThrInx], NULL,
          (void *(*)(void *)) regResult, 
	   (void *) &RegInput[RegThrInx]);
#endif
    }

    status = 0;
#ifdef PARA_OPR
    for ( i = 1; i < MAX_NUM_BL_THR; i++) {
	if (Tactive[i] > 0)
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


    return (status);
}

void
usage (char *prog)
{
	fprintf(stderr,
          "Usage: %s  [-S resourceName]  localFile/localDirectory ... targetCollection\n",prog);
	fprintf(stderr,
          "Usage: %s  -f infoFileName|-\n",prog);
}

int
initResultStruct (mdasC_sql_result_struct *myresult)
{
    int i;

    for (i = 0; i < MAX_SQL_LIMIT; i++) {
        myresult->sqlresult[i].tab_name = NULL;
        myresult->sqlresult[i].att_name = NULL;
        myresult->sqlresult[i].values = NULL;
    }

    myresult->result_count = 5;
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
    myresult->sqlresult[3].tab_name = strdup (dcs_tname[DATA_TYP_NAME]);
    myresult->sqlresult[3].att_name = strdup (dcs_aname[DATA_TYP_NAME]);
    myresult->sqlresult[4].tab_name = strdup (dcs_tname[PATH_NAME]);
    myresult->sqlresult[4].att_name = strdup (dcs_aname[PATH_NAME]);
    return (0);
}

int
fileToCollRegister (struct bufInfo *myBufInfo, srb_long_t *curOffset, 
		    srb_long_t *curSize, 
		    char *inFileName, char *outCollection, 
		    char *dataType, char *inDataName)
{
    char *dataPtr, *collPtr, *sizePtr, *typePtr, *pathPtr;
    char dataName[MAX_TOKEN], tmpName[MAX_TOKEN];

    dataPtr = (char *) Myresult[RegThrInx]->sqlresult[0].values + 
     MAX_DATA_SIZE * Myresult[RegThrInx]->row_count;
    collPtr = (char *) Myresult[RegThrInx]->sqlresult[1].values + 
     MAX_DATA_SIZE * Myresult[RegThrInx]->row_count;
    sizePtr = (char *) Myresult[RegThrInx]->sqlresult[2].values + 
     MAX_DATA_SIZE * Myresult[RegThrInx]->row_count;
    typePtr = (char *) Myresult[RegThrInx]->sqlresult[3].values + 
     MAX_DATA_SIZE * Myresult[RegThrInx]->row_count;
    pathPtr = (char *) Myresult[RegThrInx]->sqlresult[4].values + 
     MAX_DATA_SIZE * Myresult[RegThrInx]->row_count;

    if (inDataName == NULL || strlen(inDataName) == 0) {
      clSplitbychar (inFileName, tmpName, dataName, '/');
      strcpy (dataPtr, dataName);
    }
    else 
       strcpy (dataPtr, inDataName);

    sprintf (sizePtr,"%lld",*curSize);
    strcpy (collPtr, outCollection);
    if (dataType == NULL || strlen(dataType) == 0)
      trylocaldatatype(typePtr, inFileName);
    else
      strcpy (typePtr, dataType);

    sprintf (pathPtr, "%s",inFileName);


    if (Myresult[RegThrInx]->row_count >= MAX_ROW_OUT - 1) {
    	Myresult[RegThrInx]->row_count ++;
	RegInput[RegThrInx].myresult = Myresult[RegThrInx];
	RegInput[RegThrInx].myInx = RegThrInx;
	strcpy (RegInput[RegThrInx].container, ContainerName);
#ifdef DEBUG
    	printSqlResult (Myresult[RegThrInx]);
#endif

	ActRegThrInx = RegThrInx;
#ifdef PARA_OPR
        while (getNumActThr () > MAX_ACT_REG_THREAD) {
            srbThreadSleep (2, 0);
        }
	pthread_create(&Tid[RegThrInx], NULL, regResult,
          (void *) &RegInput[RegThrInx]);
#endif
	
    	/* Initialize myresult */

	RegThrInx = allocThrInx ();

    	Myresult[RegThrInx] = (mdasC_sql_result_struct *) malloc (
         sizeof (mdasC_sql_result_struct));

    	initResultStruct (Myresult[RegThrInx]);
    } else {
    	Myresult[RegThrInx]->row_count ++;
    }

    return (0);
}

int
dirToCollRegister (struct bufInfo *myBufInfo, srb_long_t *curOffset, 
srbPathName *inPathName, char *outCollection)
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
    status = dirToCollRegisterCont (myBufInfo, curOffset, inPathName->outArgv, 
     curTarg);  

    if (status < 0)
        return (CLI_ERR_COLLNAME);

    return 0;
}

int 
dirToCollRegisterCont (struct bufInfo *myBufInfo, srb_long_t *curOffset, 
char *inPathName, char *outCollection)
{
    int status;
    DIR *dirPtr;
    struct dirent *dp;
    struct stat statbuf;
    char tmpName[MAX_TOKEN];
    srbPathName mysPathName;
    char *dataType = NULL;
    char *dataName = NULL;

    dirPtr = opendir (inPathName);
    if (dirPtr == NULL) {
        fprintf (stderr, "The local dir %s does not exist., errno = %d\n",
         inPathName, errno);
        return UNIX_ENOENT;
    }

    while ((dp = readdir (dirPtr)) != NULL) {
        if (strcmp (dp->d_name, ".") == 0 || strcmp (dp->d_name, "..") == 0)
            continue;
        sprintf (tmpName, "%s/%s", inPathName, dp->d_name);
        status = stat (tmpName, &statbuf);
        if (status != 0) {
            fprintf (stderr, "Unable to stat %s\n", tmpName);
            closedir (dirPtr);
            return status;
        }
        mysPathName.inpArgv = tmpName;
        mysPathName.outArgv = tmpName;
        if ((statbuf.st_mode & S_IFREG) != 0) {     /* A file */
            mysPathName.type = DATANAME_T;
            mysPathName.size = (srb_long_t) statbuf.st_size;
            status = fileToCollRegister (myBufInfo, curOffset, &(mysPathName.size),
	     mysPathName.outArgv, outCollection, dataType, dataName);
	     
            if (status < 0) {
                closedir (dirPtr);
                return status;
            }
        } else if ((statbuf.st_mode & S_IFDIR) != 0) {      /* A directory */
            mysPathName.type = COLLECTION_T;
            mysPathName.size = 0;

	    /* sprintf (curCollection, "%s/%s", outCollection, dp->d_name); */
            status = dirToCollRegister (myBufInfo, curOffset, &mysPathName, 
	     outCollection);
            if (status < 0) {
                closedir (dirPtr);
                return status;
            }
        }
    }
    closedir (dirPtr);
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

    conn = srbConnect (myHost, NULL, srbAuth,
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"regResult: Connection to srbMaster failed.\n");
        fprintf(stderr,"regResult: %s",clErrorMessage(conn));
        srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish (conn);
	*retval = -1;
	return ((void *) retval);
    }
    
    *retval = srbBulkRegister (conn, MDAS_CATALOG, regInput->container,
     regInput->myresult, 0);

    row_count = regInput->myresult->row_count;
    clFinish(conn);
    if (VFlag > 0) {
#ifdef _WIN32
      GetSystemTime(&sTimestop);
      fSec = abs((float)sTimestart.wSecond - (float)sTimestop.wSecond) +
          abs((float)sTimestart.wMilliseconds - (float)sTimestop.wMilliseconds/1000.0);
#else
      (void) gettimeofday(&sTimestop, (struct timezone *)0);
      (void) gettimeofday(&sTimestop, (struct timezone *)0);
      (void)tvsub(&sTimetd, &sTimestop, &sTimestart);
        fSec = (float)sTimetd.tv_sec + ((float)sTimetd.tv_usec / 1000000.);
#endif
	printf ("time to register %d rows = %f\n", row_count, fSec);
	if (*retval >= 0 ) {
	  printf ("     start: %s\n     end  : %s\n",
		  (char *) regInput->myresult->sqlresult[4].values,
		  (char *) regInput->myresult->sqlresult[4].values + 
		  MAX_DATA_SIZE * (row_count - 1));
	  fflush(stdout);
	}

    }
    freeSqlResult (regInput->myresult);

    if (*retval < 0) {
        fprintf(stderr,"regResult: srbBulkRegister error, status = %d\n",
	 *retval);
    }

    Tactive[regInput->myInx] = 0;

    return ((void *) retval);
}


int 
allocThrInx ()
{
    int i;

    for (i = 0; i < MAX_NUM_BL_THR; i++) {
	if (Tactive[i] == 0) {
	    Tactive[i] = 1;
	    return (i);
	}
    }

    printf ("Runs out of thread\n");

    return (-1);
}

int
getNumActThr ()
{
    int i;
    int numActThr = 0;

    for (i = 0; i < MAX_NUM_BL_THR; i++) {
	if (Tactive[i] > 0)
	    numActThr ++;
    }
    return (numActThr);
}

