#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif
 
#include "scommands.h"
#include <pthread.h>

#include <unistd.h>
 
#ifdef PORTNAME_c90
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif

#if defined(PORTNAME_sgi)
#define lseek	lseek64
#define lstat   lstat64
#endif

extern char srbHost[];
extern char inCondition[MAX_OBJ_NAME_LENGTH];


#ifdef DO_PARTIAL_OPR_CODE_HERE
struct fileOpr {
    char *collection;
    char *objName;
    char *localFileName;
    char *resource;
    srb_long_t offset;
    srb_long_t size;
    int rwFlag;		/* read/write flag. READ_OPR or WRITE_OPR */
    int threadInx;
};
    
void *doPartialOpr (void *inFileOpr);
#else
extern void *doPartialOpr (void *inFileOpr);
#endif
int pdataToFileCopy (srbConn *conn, int catType, int flagval, int targType,
char *srcPar, char *srcCh, char *sizeStr, int srcCopyNum,
char *targFile);
int pdataToDirCopy (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, int srcCopyNum, char *targDir);
int pcollToDirCopy (srbConn *conn, int catType, int flagval,
int inColltype, mdasC_sql_result_struct *inCollResult,
char *targDir);
int pcollToDirCopyCont (srbConn *conn, int catType, int flagval, char *srcColl,
char *targDir);
int
pgetMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
srbPathName targNameArray[], int flagval, int copyNum);

/* global input param */
int NumThreads; 
int InpNumThreads = 0;
int OprMode = READ_OPR;
char MyCollection[MAX_TOKEN];
extern pthread_mutex_t Glock;

int usage(char *name) {
        fprintf(stderr,"usage: %s [-vhr] [-n copyNum] [-N NumOfThreads] srbObject|SrbCollection ... localFile|localDir\n", name );
	fprintf(stderr,"-v is a flag for Verbose\n");
	fprintf(stderr,"-r recursive get\n");
	fprintf(stderr,"-n copy number of the source;\n");
	fprintf(stderr,"-N NumOfThreads is an integer;\n");
	fprintf(stderr,"      if the option is not used, the Optimum NumOfThreads will be used\n");

	return(0);
}

int main(int argc, char **argv)
{   
    int  i;
    srbPathName targNameArray[1], srcNameArray[MAX_TOKEN];
    int c;
    int copyNum =-1;
    int nArgv;
    srbConn *conn;
    int flagval=0;

/*
  char container[MAX_TOKEN];
  char collectionName[1024];
  char resourceName[1024];
  char localfile[1024];
*/


    while ((c=getopt(argc, argv,"N:n:hvr")) != EOF) {
      switch (c) {
        case 'n':
            copyNum = atoi(optarg);
            break;
	case 'N':
	    InpNumThreads = atoi (optarg);
	    break;
	case 'v':
	    flagval |= v_FLAG;
	    break;
        case 'r':
            flagval |= R_FLAG;
            break;
        case 'h':
         default:
	    usage (argv[0]);
	    exit (1);
      }
    }
    
    /* Set up the source and target arrays */

    targNameArray[0].inpArgv = argv[argc-1];

    for (i = optind; i < argc - 1; i++) {
        srcNameArray[i - optind].inpArgv = argv[i];

    }
    nArgv = argc - optind - 1;          /* nArgv is the number of in file */

   if (nArgv < 1) {   /* must have at least 1 source input */
        usage( argv[0]);
        exit (1);
    }

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}


    /* Initialize the lock */

    pthread_mutex_init (&Glock, NULL);


  conn = srbConnect (NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  if (clStatus(conn) != CLI_CONNECTION_OK) 
  {
        printf("Main thread Connection to SRB server failed.\n");
        printf("%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn);
	exit (1);

  }
  
    /* Convert the source name to SQL like */
    if (convNameArrayToSQL (srcNameArray, nArgv) < 0)
      {clFinish(conn); exit(3);}

    i= pgetMain (conn, MDAS_CATALOG, nArgv, srcNameArray, targNameArray, 
     flagval, copyNum);

}

int
pgetMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
srbPathName targNameArray[], int flagval, int copyNum)
{
    int i;
    int status=(-1), status1=0;
    mdasC_sql_result_struct *dataResult, *collResult;
    int targType;
    int dataCnt = 0;
    int collCnt = 0;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    char *tmpStr;
    char *srcPar, *srcCh, *sizeStr;


    status = chkFileName (&targNameArray[0]);

    targType = targNameArray[0].type;
    dataResult = (mdasC_sql_result_struct *)
      malloc (sizeof (mdasC_sql_result_struct) * nArgv);
    collResult = (mdasC_sql_result_struct *)
      malloc (sizeof (mdasC_sql_result_struct) * nArgv);

    memset (dataResult, 0, sizeof (mdasC_sql_result_struct) * nArgv);
    memset (collResult, 0, sizeof (mdasC_sql_result_struct) * nArgv);

    for (i = 0; i < nArgv; i++) {
        /* set flagval to 0 first for non-recursive check */
        status = expandAndChkName (conn, MDAS_CATALOG, &nameArray[i], 0,
          &dataResult[i], &collResult[i]);
        if (status < 0) {
            fprintf (stderr, "Source %s not found\n", nameArray[i].inpArgv);
            return (CLI_ERR_COLLNAME);
        }

        dataCnt += dataResult[i].row_count;
        collCnt += collResult[i].row_count;
    }

    /* Do some sanity checks */

    if (dataCnt + collCnt == 0) {
        fprintf (stderr, "Source %s not found\n",
          targNameArray[0].inpArgv);
        return (CLI_ERR_COLLNAME);
    }

    if ((dataCnt + collCnt > 1 || collCnt > 0) && targType >= DATANAME_T) {
        fprintf (stderr, "Target %s must be a directory\n",
          targNameArray[0].inpArgv);
        return (CLI_ERR_COLLNAME);
    }

    if (collCnt > 0 && (flagval & R_FLAG) == 0) {
        fprintf (stderr, "Must use the -r flag to copy a collection\n");
        return (CLI_ERR_COLLNAME);
    }

    if (targType == -1) {       /* Target does not exist */
        if (dataCnt + collCnt > 1) {
            fprintf (stderr, "Target directory %s not found\n",
              targNameArray[0].inpArgv);
            return (CLI_ERR_COLLNAME);
        }
    }

    for (i = 0; i < nArgv; i++) {
      do {
        if (dataResult[i].row_count > 0) {      /* source is data */
            if (targType == -1 || targType >= DATANAME_T) {
                /* A normal data-file copy */
		if (dataResult[i].result_count == 0)
		    continue;
		srcPar = (char *) getFromResultStruct(&dataResult[i],
		  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
		srcCh = (char *) getFromResultStruct(&dataResult[i],
		  dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
		sizeStr = (char *) getFromResultStruct(&dataResult[i],
		  dcs_tname[SIZE], dcs_aname[SIZE]);
                status = pdataToFileCopy (conn, catType, flagval, targType,
                  srcPar, srcCh, sizeStr, copyNum, targNameArray[0].outArgv);
            } else if (targType == COLLECTION_T || targType == SPEC_COLL_T) {
                status = pdataToDirCopy (conn, catType, flagval,
                 &dataResult[i], copyNum, targNameArray[0].outArgv);
            }
        }

        if (collResult[i].row_count > 0) {      /* source is coll */
            if (collCnt == 1 && targType == -1) {
                /* the source is a single coll */
                clSplitbychar (targNameArray[0].outArgv, temp, temp1, '/');
                status = mkdir (targNameArray[0].outArgv, 0755);
                if (status < 0) {
                    fprintf(stderr,
                     "Unable to mkdir %s, status = %d\n",
                      targNameArray[0].inpArgv, status);
                    return (CLI_ERR_COLLNAME);
                }
                /* mark it as COLLECTION_T */
                targNameArray[0].type = COLLECTION_T;
                targType = COLLECTION_T;
                tmpStr = (char *) getFromResultStruct (&collResult[i],
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
                status = pcollToDirCopyCont (conn, catType, flagval, tmpStr,
                 targNameArray[0].outArgv);
            } else {
                status = pcollToDirCopy (conn, catType, flagval,
                 nameArray[i].type, &collResult[i], targNameArray[0].outArgv);
            }
        }
        if (dataResult[i].continuation_index >= 0 &&
          dataResult[i].row_count > 0) {
            clearSqlResult (&dataResult[i]);
            status = srbGetMoreRows(conn, catType,
             dataResult[i].continuation_index, &dataResult[i], MAX_GET_RESULTS);            if (status == 0)
                filterDeleted (&dataResult[i]);
        } else
            clearSqlResult (&dataResult[i]);

        if (collResult[i].continuation_index >= 0 &&
          collResult[i].result_count > 0) {
            clearSqlResult (&collResult[i]);
            status1 = srbGetMoreRows(conn, catType,
             collResult[i].continuation_index, &collResult[i], MAX_GET_RESULTS);
            if (status1 == 0)
                filterDeleted (&collResult[i]);
        } else
            clearSqlResult (&collResult[i]);

      } while ((dataResult[i].result_count > 0 && status == 0) ||
      (collResult[i].result_count > 0 && status1 == 0));


    }

    return (0);
}

int
pdataToFileCopy (srbConn *conn, int catType, int flagval, int targType,
char *srcPar, char *srcCh, char *sizeStr, int srcCopyNum,
char *targFile)
{
    int i;
    char temp[MAX_OBJ_NAME_LENGTH];
    srb_long_t datasize;
    srb_long_t  myOffset;
    srb_long_t  size0, size1;
    struct fileOpr myFileOpr[MAX_NUM_THREADS];
    pthread_t tid[MAX_NUM_THREADS];
    int *tstatus[MAX_NUM_THREADS];


    if (sizeStr == NULL)
        datasize = 1;
    else
        datasize = strtoll (sizeStr, 0, 0);

    if (srcCopyNum >=  0) {
        sprintf(temp, "%s&COPY=%i", srcCh, srcCopyNum);
    } else {
        strcpy (temp, srcCh);
    }

    if (strlen(inCondition) > 0) {
        strcat(temp,"&");
        strcat(temp,inCondition);
    }

    if (targType >= DATANAME_T) {  /* overwrite an existing object */
        fprintf (stderr, "Target object %s already exist.\n", targFile);
    } else {
	myOffset = 0;

	if (InpNumThreads > 0) {
            if (InpNumThreads > MAX_NUM_THREADS)
      	        NumThreads = MAX_NUM_THREADS;
            else
                NumThreads = InpNumThreads;
	} else {
	    NumThreads = (int) (datasize / DEF_SIZE_PER_THREAD);
	    if (NumThreads > MAX_NUM_THREADS)
	        NumThreads = MAX_NUM_THREADS;
	    else if (NumThreads <= 0)
	        NumThreads = 1;
	}

	size0 = datasize / NumThreads;
	size1 = datasize - size0 * (NumThreads - 1);

	for (i = 0; i < NumThreads; i++) {
	    myFileOpr[i].collection = srcPar;
	    myFileOpr[i].objName = temp;
	    myFileOpr[i].localFileName = targFile;
	    if (i == NumThreads - 1)
		myFileOpr[i].size = size1;
	    else
	    	myFileOpr[i].size = size0;
	    myFileOpr[i].offset = myOffset;
	    myOffset += myFileOpr[i].size;
	    myFileOpr[i].rwFlag = OprMode;
	    myFileOpr[i].flagval = flagval;
	    myFileOpr[i].threadInx = i;
	    if (flagval & v_FLAG) {
		fprintf (stderr,"pthread_create start for %d ...", i);
		fflush(stderr);
	    }
	    pthread_create(&tid[i], NULL, doPartialOpr, 
	      (void *) &myFileOpr[i]);
	    if (flagval & v_FLAG) {
		fprintf (stderr," done for %d.\n", i);
		fflush(stderr);
	    }
	}

	for ( i = 0; i < NumThreads; i++) {
	    pthread_join (tid[i], (void **) &tstatus[i]);
	    if (flagval & v_FLAG) {
		fprintf (stderr,"pthread_join done for %d. status = %d\n", 
		   i, *tstatus[i]);
		fflush(stderr);
	    }
       	    /* printf ("pthread_join done for thread %d\n", i); */
	}

    }

    return (0);
}

#ifdef DO_PARTIAL_OPR_CODE_HERE	
void *doPartialOpr (void *inFileOpr)
{
    struct fileOpr *myFileOpr;
    srbConn *conn;
    int  i, nbytes,  in_fd, out_fd;
    char *buf;
    int *retval;
    int status;
    srb_long_t sstatus;
    srb_long_t ioSize, sizeRemaining;
    int retryCnt = 0;

    
    myFileOpr = (struct fileOpr *) inFileOpr;

    conn = clConnect (NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to exfMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        clFinish(conn);
        *retval = -1;
        return ((void *) retval);
    }

    retval = (int *) malloc (sizeof (int));
    buf = malloc (BUFSIZE);

    if (myFileOpr->rwFlag == READ_OPR) {
        if (flagval & v_FLAG) {
          fprintf (stderr,"opening local file for %i.\n",
                   myFileOpr->threadInx);
          fflush(stderr);
        }
        out_fd = open (myFileOpr->localFileName,  O_RDWR | O_CREAT, 0644);
        if (out_fd < 0) {
            fprintf(stderr,
             "can't open local file %s\n", myFileOpr->localFileName);
            clFinish(conn);
            exit (1);
        }

        if (flagval & v_FLAG) {
          fprintf (stderr,"opening srbObj for %i.\n",
                   myFileOpr->threadInx);
          fflush(stderr);
        }
        in_fd = srbObjOpen (conn, myFileOpr->objName, O_RDONLY,
         myFileOpr->collection);
        if (in_fd < 0)  {   /* error */
            fprintf(stderr, "can't open srb obj\"%s\"\n", myFileOpr->objName);
            clFinish(conn);
            exit (1);
        }
        if (flagval & v_FLAG) {
          fprintf (stderr,"opened srbObj for %i.\n",
                   myFileOpr->threadInx);
          fflush(stderr);
        }

        if (myFileOpr->offset != 0) {
            sstatus = srbObjSeek (conn, in_fd, myFileOpr->offset, SEEK_SET);
            if (sstatus != myFileOpr->offset) {
                fprintf(stderr, "srbObjSeek loc %lld != input %lld\n",
                 sstatus, myFileOpr->offset);
                clFinish(conn);
                exit (1);
            }
        }


        if (myFileOpr->offset != 0) {
            sstatus = lseek (out_fd, myFileOpr->offset, SEEK_SET);
            if (sstatus != myFileOpr->offset) {
                fprintf(stderr, "llseek loc %lld != input %lld, errno=%d\n",
                 sstatus, myFileOpr->offset, (int) errno);
                clFinish(conn);
                exit (1);
            }
        }

        sizeRemaining = myFileOpr->size;

        while (sizeRemaining > 0) {
            if (myFileOpr->size >= BUFSIZE) {
                ioSize = BUFSIZE;
            } else {
                ioSize = myFileOpr->size;
            }
            nbytes = srbObjRead (conn, in_fd, buf, (int) ioSize);
            if (nbytes <= 0)
                break;

            if (write (out_fd, buf, nbytes) <= 0) {
                fprintf(stderr, "local write errror for %s, errno = %d\n",
                 myFileOpr->objName, errno);
                srbObjClose (conn, in_fd);
                close (out_fd);
                clFinish(conn);
                *retval = -6;
                return ((void *) retval);
            }
            sizeRemaining -= (srb_long_t) nbytes;
            if (flagval & v_FLAG) {
              fprintf (stderr," R%i ",myFileOpr->threadInx);
              fflush(stderr);
            }
        }
        if (flagval & v_FLAG) {
          fprintf (stderr,"\nclosing srbObj for %i.\n",
                   myFileOpr->threadInx);
          fflush(stderr);
        }
        srbObjClose (conn, in_fd);
        if (flagval & v_FLAG) {
          fprintf (stderr,"closed srbObj for %i.\n",
                   myFileOpr->threadInx);
          fflush(stderr);
        }
        close (out_fd);
    } else if (myFileOpr->rwFlag == WRITE_OPR) {
        in_fd = open (myFileOpr->localFileName,  O_RDONLY, 0644);
        if (in_fd < 0) {
            fprintf(stderr, "can't open local file %s\n",
             myFileOpr->localFileName);
            clFinish(conn);
            *retval = -2;
            return ((void *) retval);
        }
        if (myFileOpr->threadInx == 0) {
            pthread_mutex_lock (&Glock);
            if (flagval & v_FLAG) {
              fprintf (stderr,"creating srbObj for %i.\n",
                       myFileOpr->threadInx);
                fflush(stderr);
            }
            out_fd = srbObjCreate (conn, MDAS_CATALOG, myFileOpr->objName,
             NULL, myFileOpr->resource, myFileOpr->collection, NULL, -1);
            pthread_mutex_unlock (&Glock);
            if (out_fd < 0)  {
                fprintf(stderr,
                 "can't create srb obj %s. status = %d\n",
                  myFileOpr->objName, out_fd);
                srbObjUnlink (conn, myFileOpr->objName, myFileOpr->collection);
                clFinish(conn);
                exit (1);
            }
            if (flagval & v_FLAG) {
              fprintf (stderr,"created srbObj for %i.\n",
                       myFileOpr->threadInx);
                fflush(stderr);
            }
        } else {
            while (retryCnt < MAX_P_OPEN_RETRY) {
                pthread_mutex_lock (&Glock);
                pthread_mutex_unlock (&Glock);
                if (flagval & v_FLAG) {
                  fprintf (stderr,"opening srbObj for %i.\n",
                           myFileOpr->threadInx);
                    fflush(stderr);
                }
                out_fd = srbObjOpen (conn, myFileOpr->objName, O_RDWR,
                 myFileOpr->collection);
                if (out_fd >= 0)  {
                    break;
                }
                retryCnt ++;
            }
            if (out_fd < 0)  {   /* error */
                fprintf(stderr,
                 "can't open srb obj\"%s\"\n", myFileOpr->objName);
                srbObjUnlink (conn, myFileOpr->objName, myFileOpr->collection);
                clFinish(conn);
                exit (1);
            }
            if (flagval & v_FLAG) {
              fprintf (stderr,"opened srbObj for %i.\n",
                       myFileOpr->threadInx);
                fflush(stderr);
            }
        }
        if (myFileOpr->offset != 0) {
            sstatus = srbObjSeek (conn, out_fd, myFileOpr->offset, SEEK_SET);
            if (sstatus != myFileOpr->offset) {
                fprintf(stderr, "srbObjSeek loc %lld != input %lld\n",
                 sstatus, myFileOpr->offset);
                srbObjUnlink (conn, myFileOpr->objName, myFileOpr->collection);
                clFinish(conn);
                exit (1);
            }
        }

        if (myFileOpr->offset != 0) {
            sstatus = lseek (in_fd, myFileOpr->offset, SEEK_SET);
            if (sstatus != myFileOpr->offset) {
                fprintf(stderr, "lseek for write loc %lld != input %lld, errno=%lld\n",
                 sstatus, myFileOpr->offset,  errno);
                srbObjUnlink (conn, myFileOpr->objName, myFileOpr->collection);
                clFinish(conn);
                exit (1);
            }
        }

        sizeRemaining = myFileOpr->size;
        while (sizeRemaining > 0) {
            if (myFileOpr->size >= BUFSIZE) {
                ioSize = BUFSIZE;
            } else {
                ioSize = myFileOpr->size;
            }
            nbytes = read (in_fd, buf, ioSize);
            if (nbytes <= 0)
                break;
            if ((status = srbObjWrite (conn, out_fd, buf, nbytes)) <= 0) {
                fprintf(stderr, "srb write errror for %s, errno = %d\n",
                 myFileOpr->objName, status);
                srbObjClose (conn, out_fd);
                close (in_fd);
                srbObjUnlink (conn, myFileOpr->objName, myFileOpr->collection);
                clFinish(conn);
                exit (1);
            }
            sizeRemaining -= (srb_long_t) nbytes;
            if (flagval & v_FLAG) {
              fprintf (stderr," W%i ",myFileOpr->threadInx);
              fflush(stderr);
            }
        }
        close (in_fd);
    }

    free (buf);
    if (flagval & v_FLAG) {
      fprintf (stderr,"start finish processing srb file %s for %i\n", myFileOpr->objName,myFileOpr->threadInx);
      fflush (stderr);
    }
    clFinish(conn);
    if (flagval & v_FLAG) {
      fprintf (stderr,"finished processing srb file %s for %i\n", myFileOpr->objName, myFileOpr->threadInx);
      fflush (stderr);
    }
    *retval = 0;
    return ((void *) retval);
}
#endif	/* DO_PARTIAL_OPR_CODE_HERE */

int
pdataToDirCopy (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, int srcCopyNum, char *targDir)
{
    int i, status;
    char *datatype;
    char temp[MAX_OBJ_NAME_LENGTH];
    char *srcPar, *srcCh, *sizeStr;
    struct stat statbuf;

    if (dataResult->result_count == 0)
        return 0;
    srcPar = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    srcCh = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    sizeStr = (char *) getFromResultStruct(dataResult,
      dcs_tname[SIZE], dcs_aname[SIZE]);

    datatype = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);

    for (i = 0; i < dataResult->row_count;i++) {

        /* Check whether the target file exists */

        sprintf (temp, "%s/%s", targDir, srcCh);
        status = stat (temp, &statbuf);
        if (status >= 0 && (flagval & F_FLAG) == 0) {
            fprintf (stderr, "Local file %s already exist.\n",
              temp);
        } else {        /* it doesn't exist */
	    status = pdataToFileCopy (conn, catType, flagval, -1, 
	     srcPar, srcCh, sizeStr, srcCopyNum, temp);
	    if (status < 0) {
                fprintf (stderr, "Error getting %s/%s.\n", srcPar, srcCh);
		srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    }
        }
        srcPar += MAX_DATA_SIZE;
        srcCh += MAX_DATA_SIZE;
        sizeStr += MAX_DATA_SIZE;
        datatype += MAX_DATA_SIZE;
    }
    return (0);
}

int
pcollToDirCopy (srbConn *conn, int catType, int flagval,
int inColltype, mdasC_sql_result_struct *inCollResult,
char *targDir)
{
    int i, status;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN], tmpTarg[MAX_TOKEN];
    char *origSrc, *curTarg;
    struct stat statbuf;

    if (inCollResult->result_count == 0)
        return 0;
    origSrc = (char *) getFromResultStruct(inCollResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    for (i = 0; i < inCollResult->row_count; i++) {
        if (inColltype != SPEC_COLL_T) {        /* not ".". ".." type */
            /* Make the dir */
            clSplitbychar(origSrc, temp, temp1,'/');
            sprintf (tmpTarg, "%s/%s", targDir, temp1);
            status = stat (tmpTarg, &statbuf);
            if (status < 0) {   /* does not exist */
                status = mkdir(tmpTarg, 0755);
                if (status < 0) {
                    fprintf(stderr,
                     "Unable to mk the target dir %s, status = %d\n",
                      tmpTarg, status);
                    return (CLI_ERR_COLLNAME);
                }
            } else if ((statbuf.st_mode & S_IFDIR) == 0) {  /* Not a dir */
                fprintf(stderr,
                 "collToDir: A local non-directory %s already exists \n",
                  tmpTarg);
                return (CLI_ERR_COLLNAME);
            }
            curTarg = tmpTarg;
        } else {
            curTarg = targDir;
        }
        /* copy the content */
        status = pcollToDirCopyCont (conn, catType, flagval,
          origSrc, curTarg);

        if (status < 0)
             return (CLI_ERR_COLLNAME);
        origSrc += MAX_DATA_SIZE;
    }
    return 0;
}

int
pcollToDirCopyCont (srbConn *conn, int catType, int flagval, char *srcColl, 
char *targDir) 
{
    mdasC_sql_result_struct  myresult;
    int status;

    /* copy the dataset first */

    status = queryDataInColl (conn, catType, srcColl, flagval, MAX_GET_RESULTS,
      &myresult);
    if (status < 0 && status != MCAT_INQUIRE_ERROR) {
        srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        return status;
    }

    status = pdataToDirCopy (conn, catType, flagval,
      &myresult, -1, targDir);
    if (status < 0)
        return status;

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        status = srbGetMoreRows(conn, catType,
          myresult.continuation_index, &myresult, MAX_GET_RESULTS);
        if (myresult.result_count == 0  || status != 0)
            break;

        status = pdataToDirCopy (conn, catType, flagval,
          &myresult, -1, targDir);
        if (status < 0)
            break;
    }
    clearSqlResult (&myresult);

    /* now copy the collection recursively */

    status = querySubCollInColl (conn, catType, srcColl, 0, MAX_GET_RESULTS,
      &myresult);
    if (status < 0 && status != MCAT_INQUIRE_ERROR) {
        srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        return status;
    }

    status = pcollToDirCopy (conn, catType, flagval,
      -1, &myresult, targDir);

    if (status < 0 && status != MCAT_INQUIRE_ERROR)
         return (status);

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        status = srbGetMoreRows(conn, catType,
          myresult.continuation_index, &myresult, MAX_GET_RESULTS);
        if (myresult.result_count == 0  || status != 0)
            break;

        status = pcollToDirCopy (conn, catType, flagval,
          -1, &myresult, targDir);
        if (status < 0)
            break;
    }
    clearSqlResult (&myresult);
    return (0);
}

