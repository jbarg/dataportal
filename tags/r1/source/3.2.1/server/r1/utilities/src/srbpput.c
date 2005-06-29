#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif
 
#include "scommands.h"
#include <pthread.h>

#if defined(PORTNAME_sgi)
#define lseek   lseek64
#define lstat   lstat64
#endif

#include <unistd.h>
 
#ifdef PORTNAME_c90
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif

extern char srbHost[];

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
int pfileToDataCopy (srbConn *conn, int catType, int flagval, int targType,
srbPathName *fileName, char *targColl, char *targObj, char *newResource);
int pfileToCollCopy (srbConn *conn, int catType, int flagval,
srbPathName *fileName, char *targColl, char *newResource);
int pdirToCollCopy (srbConn *conn, int catType, int flagval,
srbPathName *inPathName, char *origTarg, char *newResource);
int pdirToCollCopyCont (srbConn *conn, int catType, int flagval,
char *srcDir, char *targColl, char *newResource);

/* global input param */
int NumThreads; 
int InpNumThreads = 0; 
int OprMode = WRITE_OPR;
char MyCollection[MAX_TOKEN];
extern pthread_mutex_t Glock;

char newresourcename[MAX_TOKEN];

int usage(char *name) {
        fprintf(stderr,"usage: %s [-vrh] [-N NumOfThreads] [-S resource] localFile|localDir SrbObject|SrbCollection \n", name );
	fprintf(stderr,"-v is a flag for Verbose\n");
	fprintf(stderr,"-N NumOfThreads is an integer;\n");
	fprintf(stderr,
         "      if the option is not used, The Optimum NumOfThreads will be used\n");
	return(0);
}

int main(int argc, char **argv)
{   
    int  i;
    char path[MAX_TOKEN];
    srbPathName nameArray[MAX_TOKEN], targNameArray[1];

    int c;
    int runCnt = 0;
    int nArgv;
    srbConn *conn;
    int idx;
    int flagval=0;
    int SFlag = 0;              /* -S option */

    while ((c=getopt(argc, argv,"S:N:hvr")) != EOF) {
      switch (c) {
	case 'N':
	    InpNumThreads = atoi (optarg);
	    break;
	case 'v':
	    flagval |= v_FLAG;   
	    break;
        case 'S':
            SFlag = 1;
            strcpy (newresourcename, optarg);
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
        nameArray[i - optind].inpArgv = argv[i];
    }
    nArgv = argc - optind - 1;          /* nArgv is the number of in file */

    if (nArgv < 1) {   /* must have at least 1 source input */
        usage( argv[0]);
        exit (1);
    }

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}

    /* Convert the target name to SQL like */
    if (convNameArrayToSQL (targNameArray, 1) < 0)
      {clFinish(conn); exit(3);}

    conn = srbConnect (NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        printf("Main thread Connection to SRB server failed.\n");
        printf("%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn);
	exit (1);

    }

    /* Initialize the lock */

    pthread_mutex_init (&Glock, NULL);

    i = pputMain (conn, MDAS_CATALOG, nArgv, nameArray, targNameArray, flagval);

    clFinish(conn);
    exit(0);
}

int
pputMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
srbPathName targNameArray[], int flagval)
{
    int i;
    int status, status1;
    mdasC_sql_result_struct targDataResult, targCollResult;
    mdasC_sql_result_struct *dataResult, *collResult;
    int targType;
    int dataCnt = 0;
    int collCnt = 0;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    char srcPar[MAX_TOKEN], srcCh[MAX_TOKEN];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN];
    char *tmpStr, *tmpStr1;

    targType = expandAndChkName (conn, MDAS_CATALOG, &targNameArray[0], 0,
      &targDataResult, &targCollResult);
    unmake_like_str (targNameArray[0].outArgv, targNameArray[0].outArgv);
    if (targDataResult.row_count + targCollResult.row_count > 1) {
        fprintf (stderr, "The target spcification is not unique\n");
        return (CLI_ERR_COLLNAME);
    }
    dataResult = (mdasC_sql_result_struct *)
      malloc (sizeof (mdasC_sql_result_struct) * nArgv);
    collResult = (mdasC_sql_result_struct *)
      malloc (sizeof (mdasC_sql_result_struct) * nArgv);

    memset (dataResult, 0, sizeof (mdasC_sql_result_struct) * nArgv);
    memset (collResult, 0, sizeof (mdasC_sql_result_struct) * nArgv);

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
          targNameArray[0].inpArgv);
        return (CLI_ERR_COLLNAME);
    }

    if ((dataCnt + collCnt > 1 || collCnt > 0) && targType >= DATANAME_T) {
        fprintf (stderr, "Target %s must be a collection\n",
          targNameArray[0].inpArgv);
        return (CLI_ERR_COLLNAME);
    }

    if (collCnt > 0 && (flagval & R_FLAG) == 0) {
        fprintf (stderr, "Must use the -r flag to copy a collection\n");
        return (CLI_ERR_COLLNAME);
    }
    if (targType == -1) {       /* Target does not exist */
        if (dataCnt + collCnt > 1) {
            fprintf (stderr, "Target collection %s not found\n",
              targNameArray[0].inpArgv);
            return (CLI_ERR_COLLNAME);
        }
    }

    /* set the target resource */

    if (strlen(newresourcename) == 0) {
        get_user_preference(newresourcename, "DRSRC");
    }

    /* Now start copying */

    for (i = 0; i < nArgv; i++) {
        if (nameArray[i].type >= DATANAME_T) {  /* source is data */
            if (targType == -1) {       /* A normal new data-data copy */
                clSplitbychar (targNameArray[0].outArgv, targPar, targCh, '/');
                status = pfileToDataCopy (conn, catType, flagval, targType,
                  &nameArray[i], targPar, targCh, newresourcename);
            } else if (targType >= DATANAME_T) {
                tmpStr = (char *) getFromResultStruct (&targDataResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
                tmpStr1 = (char *) getFromResultStruct (&targDataResult,
                  dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
                status = pfileToDataCopy (conn, catType, flagval, targType,
                 &nameArray[i], tmpStr, tmpStr1,
                  newresourcename);
            } else if (targType == COLLECTION_T || targType == SPEC_COLL_T) {
                tmpStr = (char *) getFromResultStruct (&targCollResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
                status = pfileToCollCopy (conn, catType, flagval,
                 &nameArray[i], tmpStr, newresourcename);
            }
        } else {        /* source is coll */
            if (collCnt == 1 && targType == -1) {
                /* the source is a single coll */
                clSplitbychar (targNameArray[0].outArgv, temp, temp1, '/');
                status = srbCreateCollect(conn, catType, temp, temp1);
                if (status < 0) {
                    fprintf(stderr,
                     "Unable to mk the target coll %s, status = %d\n",
                      targNameArray[0].inpArgv, status);
                    return (CLI_ERR_COLLNAME);
                }
                /* mark it as COLLECTION_T */
                targNameArray[0].type = COLLECTION_T;
                targType = COLLECTION_T;
                status = pdirToCollCopyCont (conn, catType, flagval,
                 nameArray[i].inpArgv, targNameArray[0].outArgv,
                  newresourcename);
            } else {
                tmpStr = (char *) getFromResultStruct (&targCollResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
                status = pdirToCollCopy (conn, catType, flagval,
                  &nameArray[i], tmpStr, newresourcename);
            }
        }
    }

    return (0);
}

int
pfileToDataCopy (srbConn *conn, int catType, int flagval, int targType,
srbPathName *fileName, char *targColl, char *targObj, char *newResource)
{
    int i, status, tmp;
    srb_long_t datasize;
    char inbuf[MAX_TOKEN];
    char tmpDataType[MAX_TOKEN];
    pthread_t tid[MAX_NUM_THREADS];
    int *tstatus[MAX_NUM_THREADS];
#if defined(PORTNAME_sgi)
    struct stat64 statbuf;
#else
    struct stat statbuf;
#endif
    int retval;
    srb_long_t  size0, size1;
    struct fileOpr myFileOpr[MAX_NUM_THREADS];
    srb_long_t  myOffset;

    if (targType >= DATANAME_T) {  /* overwrite an existing object */
        fprintf (stderr,
         "Target object %s/%s already exist.\n",
          targColl, targObj);
        return (CLI_ERR_COLLNAME);
    }

    myOffset = 0;

    if (InpNumThreads > 0) {
	if (InpNumThreads > MAX_NUM_THREADS)
	    NumThreads = MAX_NUM_THREADS;
	else 
	    NumThreads = InpNumThreads;
    } else {
	NumThreads = (int) (fileName->size / DEF_SIZE_PER_THREAD);
	if (NumThreads > MAX_NUM_THREADS) 
	    NumThreads = MAX_NUM_THREADS;
	else if (NumThreads <= 0)
	    NumThreads = 1;
    }

    size0 = fileName->size / NumThreads;
    size1 = fileName->size - size0 * (NumThreads - 1);

    for (i = 0; i < NumThreads; i++) {
        myFileOpr[i].collection = targColl;
	myFileOpr[i].resource = newResource;
	myFileOpr[i].objName = targObj;
	myFileOpr[i].localFileName = fileName->inpArgv;
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
        fprintf(stderr,"Connection to SRB server failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        clFinish(conn);
	exit (1);
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
	     "can't open local file %s, errno = %d\n", 
	       myFileOpr->localFileName, errno);
            clFinish(conn);
	    exit (1);
	}

        if (flagval & v_FLAG) {
	  fprintf (stderr,"opening srbObj for %i.\n", 
		   myFileOpr->threadInx);
	  fflush(stderr);
	}
    	in_fd = srbObjOpen (conn, myFileOpr->objName, O_RDWR, 
	 myFileOpr->collection);
    	if (in_fd < 0)  {   /* error */
            fprintf(stderr, "can't open srb obj %s\n", myFileOpr->objName);
	    srb_perror (2, in_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
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
		srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
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
            fprintf(stderr, "can't open local file %s, errno = %d\n", 
	     myFileOpr->localFileName, errno);
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
		srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
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
	    if (flagval & v_FLAG) {
                fprintf (stderr,"opening srbObj for %i.\n",
                  myFileOpr->threadInx);
                fflush(stderr);
            }

	    while (retryCnt < MAX_P_OPEN_RETRY) {
	        pthread_mutex_lock (&Glock);
	        pthread_mutex_unlock (&Glock);
		out_fd = srbObjOpen (conn, myFileOpr->objName, O_RDWR,
         	 myFileOpr->collection);
            	if (out_fd >= 0)  { 
		    break;
		}
		usleep (50000);
		retryCnt ++;
	    }
	    if (out_fd < 0)  {   /* error */
                fprintf(stderr,
                 "can't open srb obj\"%s\"\n", myFileOpr->objName);
		srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
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
		srb_perror (2, (int) sstatus, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
		srbObjUnlink (conn, myFileOpr->objName, myFileOpr->collection);
                clFinish(conn);
		exit (1);
            }
        }

        if (myFileOpr->offset != 0) {
            sstatus = lseek (in_fd, myFileOpr->offset, SEEK_SET);
            if (sstatus != myFileOpr->offset) {
                fprintf(stderr, 
		  "lseek for write loc %lld != input %lld, errno=%lld\n",
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
        srbObjClose (conn, out_fd);
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
pfileToCollCopy (srbConn *conn, int catType, int flagval,
srbPathName *fileName, char *targColl, char *newResource)
{
    int i, status, tmp;
    srb_long_t datasize;
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN];
    char srcPar[MAX_TOKEN], srcCh[MAX_TOKEN], tmpTargObj[MAX_TOKEN];
    char inbuf[MAX_TOKEN];
    char tmpDataType[MAX_TOKEN];

    /* Check whether the target dataset exists */

    clSplitbychar(fileName->outArgv, srcPar, srcCh,'/');
    status = checkDataset (conn, catType, targColl, srcCh);

    if (status == 0) {          /* it exists */
        fprintf (stderr, "Target object %s/%s already exist.\n",
         targColl, srcCh);
        return (CLI_ERR_COLLNAME);
    }

    status = pfileToDataCopy (conn, catType, flagval, -1,
     fileName, targColl, srcCh, newResource);

    return (status);
}

int
pdirToCollCopy (srbConn *conn, int catType, int flagval,
srbPathName *inPathName, char *origTarg, char *newResource)
{
    int i, status;
    int in_fd, out_fd;
    srb_long_t datasize;
    char *datatype;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN], tmpTarg[MAX_TOKEN];
    char *origSrc, *curTarg;

    if (inPathName->type != SPEC_COLL_T) {      /* not ".". ".." type */
        /* Make the collection */
        clSplitbychar(inPathName->outArgv, temp1, temp,'/');
        status = srbCreateCollect(conn, catType, origTarg, temp);
        if (status < 0 && status != DATA_SUBCOLLECTION_NOT_UNIQUE) {
            fprintf(stderr,
             "Unable to mk the target coll %s/%s, status = %d\n",
              origTarg, temp, status);
            srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return (CLI_ERR_COLLNAME);
        }
        sprintf (tmpTarg, "%s/%s", origTarg, temp);
        curTarg = tmpTarg;
    } else {
        curTarg = origTarg;
    }

    /* copy the content */
    status = pdirToCollCopyCont (conn, catType, flagval,
      inPathName->outArgv, curTarg, newResource);

    if (status < 0)
        return (CLI_ERR_COLLNAME);

    return 0;
}

int
pdirToCollCopyCont (srbConn *conn, int catType, int flagval,
char *srcDir, char *targColl, char *newResource)
{
    mdasC_sql_result_struct  myresult;
    int i, j;
    int status;
    DIR *dirPtr;
    struct dirent *dp;
    struct stat statbuf;
    char tmpName[MAX_TOKEN];
    srbPathName mysPathName;
    char tmpNewFileName[HUGE_STRING];
    char tmpsrcDir[MAX_TOKEN];

    strcpy(tmpNewFileName,"");

    dirPtr = opendir (srcDir);
    if (dirPtr == NULL) {
        fprintf (stderr, "The local dir %s does not exist., errno = %d\n",
         srcDir, errno);
        return UNIX_ENOENT;
    }

    while ((dp = readdir (dirPtr)) != NULL) {
        if (strcmp (dp->d_name, ".") == 0 || strcmp (dp->d_name, "..") == 0)
            continue;
        sprintf (tmpName, "%s/%s", srcDir, dp->d_name);
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
            mysPathName.size = statbuf.st_size;
            status = pfileToCollCopy (conn, catType, flagval,
              &mysPathName, targColl, newResource);
            if (status < 0) {
                closedir (dirPtr);
                return status;
            }
        } else if ((statbuf.st_mode & S_IFDIR) != 0) {      /* A directory */
            mysPathName.type = COLLECTION_T;
            mysPathName.size = 0;

            status = pdirToCollCopy (conn, catType, flagval,
              &mysPathName, targColl, newResource);

            if (status < 0) {
                closedir (dirPtr);
                return status;
            }
        }
    }
    closedir (dirPtr);
    return (0);
}

