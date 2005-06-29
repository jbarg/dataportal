/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * threads.c-- Test some pthread calls
 *
 *-------------------------------------------------------------------------
 */
#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/time.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif
 
#include "srbClient.h"
#include <pthread.h>

/* define for fileOpr.rwFlag */
#define READ_OPR        0
#define WRITE_OPR       1

/* define whether the output file should be written - valid only for READ_OPR */
#define NO_WRITE	0
#define DO_WRITE	1

/* define whether the input file should be read - valid only for WRITE_OPR */
#define NO_READ        0
#define DO_READ        1

#define NUM_RUN		1	/* number of runs to make */
#define BUFSIZE		4000000
#define NUM_THREADS	2
#define OPR_MODE	READ_OPR
#define WRITE_OUT	NO_WRITE

#define HOST_ADDR       NULL
#define COLLECTION      "/home/srb"
#define MAX_NUM_THREADS	10
#define MAX_RETRY	100
#define DEF_FILE_SZ	1000000000	/* 1 gb default file size */

struct fileOpr {
    char *collection;
    char *objName;
    int offset;
    int size;
    int rwFlag;		/* read/write flag. READ_OPR or WRITE_OPR */
    int threadInx;
    struct threadTime *threadtime;
};
    
struct mainTime {
    hrtime_t start;
    hrtime_t stat;
    hrtime_t thrCreate;
    hrtime_t thrJoin;
    hrtime_t end;
};

struct threadTime {
    hrtime_t start;
    hrtime_t connect;
    hrtime_t open;
    hrtime_t seek;
    hrtime_t end;
    float ioRate;	/* I/O rate at bytes/sec */
};


void *getSrb (void *inFileOpr);
void *putSrb (void *inFileOpr);
void *doPartialOpr (void *inFileOpr);

/* global input param */
int NumRun = NUM_RUN; 
int BufSize = BUFSIZE;
int NumThreads = NUM_THREADS; 
int OprMode = READ_OPR;
int WriteOut = NO_WRITE;
int ReadIn = NO_READ;
pthread_mutex_t Glock;

void 
exit_nicely(srbConn* conn)
{
  clFinish(conn);
  exit(1);
}

int
main(int argc, char **argv)
{
    int  i;
    pthread_t tid[MAX_NUM_THREADS];
    int *tstatus[MAX_NUM_THREADS];
    struct stat statbuf;
    int retval;
    srbConn *conn;
    int size0, size1;
    struct fileOpr myFileOpr[MAX_NUM_THREADS];
    char path[MAX_TOKEN];
    int myOffset;
    struct mainTime maintime, totMaintime;
    struct threadTime totThreadtime;
    int sFlag = 0;
    int c;
    float ftime;
    int runCnt = 0;
    hrtime_t tmpTime;
    int mySize;
    int numInp;
    
    
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-s] [-r NumRun] [-t NumThreads] [-m BufSize] [-o OprMode] [-W WriteOut] [-R ReadIn] srbFile1 srbFile2 ... \n",argv[0]);
        exit(1);
    }

    while ((c=getopt(argc, argv,"r:t:m:o:W:R:s")) != EOF) {
      switch (c) {
	case 's':
	    sFlag = 1;
	    break;
	case 'r':
	    NumRun = atoi (optarg);
	    break;
	case 't':
	    NumThreads = atoi (optarg);
	    break;
	case 'm':
	    BufSize = atoi (optarg);
	    break;
	case 'o':
	    OprMode = atoi (optarg);
	    break;
	case 'W':
	    WriteOut = atoi (optarg);
	    break;
	case 'R':
	    ReadIn = atoi (optarg);
	    break;
      }
    }

    numInp = argc - optind;
    if (numInp < 1) {   /* must have at least 1 input */
        fprintf(stderr, "Usage: %s [-s] [-r NumRun] [-t NumThreads] [-m BufSize] [-o OprMode] [-w WriteOut] srbFile1 srbFile2 ... \n",argv[0]);
        exit (1);
    }

    if (sFlag == 1) {
	NumThreads = 1;
        myFileOpr[0].collection = COLLECTION;
        myFileOpr[0].objName = argv[argc - 1];
        myFileOpr[0].threadtime =
          (struct threadTime *) malloc (sizeof (struct threadTime));
    }
    printf (
     "\n\nStart run: threads=%d, buf=%d, oprMode=%d, WriteOut=%d\n\n",
       NumThreads, BufSize, OprMode, WriteOut);
    fflush (stdout);

    /* initialize the total clock */

    totMaintime.start = totMaintime.stat = totMaintime.thrCreate = 
     totMaintime.thrJoin = totMaintime.end = 0;

    totThreadtime.start = totThreadtime.connect = totThreadtime.open = 
      totThreadtime.seek = totThreadtime.end = 0;

    totThreadtime.ioRate = 0.0;

    /* Initialize the lock */

    pthread_mutex_init (&Glock, NULL);

    while (runCnt < NumRun) {

	runCnt ++;
    	maintime.start = gethrtime ();
        conn = clConnect (HOST_ADDR, NULL, NULL);
        if (clStatus(conn) != CLI_CONNECTION_OK) {
            fprintf(stderr,"Main thread connection to exfMaster failed.\n");
            fprintf(stderr,"%s",clErrorMessage(conn));
            clFinish(conn);
            exit (1);
        }
	/* don't count connect time if single thread */
	if (sFlag == 1 || numInp > 1) 
	    maintime.start = gethrtime ();


    	if (sFlag == 1) {
	    if (OprMode == WRITE_OPR)
	    	putSrb (&myFileOpr[0]);
	    else
	    	getSrb (&myFileOpr[0]);

	    i = 0;
	    totMaintime.start += (gethrtime () - maintime.start);
	    totThreadtime.start += (myFileOpr[i].threadtime->start - 
	     maintime.start);
	    totThreadtime.connect += (myFileOpr[i].threadtime->connect -
	     myFileOpr[i].threadtime->start);
	    totThreadtime.open += (myFileOpr[i].threadtime->open -
               myFileOpr[i].threadtime->connect);
	    totThreadtime.end += (myFileOpr[i].threadtime->end -
             myFileOpr[i].threadtime->open);
	    ftime = ((float) (myFileOpr[i].threadtime->end - 
	     myFileOpr[i].threadtime->open))/ 1000000000.0;
	    totThreadtime.ioRate += ((float) myFileOpr[i].size) / ftime;
	     
            printf ("thread %i start time = %10.2f sec\n", i, ((float)
             (myFileOpr[i].threadtime->start - maintime.start))/1000000000.0);
            printf ("thread %i connect time = %10.2f sec\n", i, ((float)
             (myFileOpr[i].threadtime->connect - 
	       myFileOpr[i].threadtime->start)) / 1000000000.0);
            printf ("thread %i open time = %10.2f sec\n", i, ((float)
             (myFileOpr[i].threadtime->open - 
	       myFileOpr[i].threadtime->connect)) / 1000000000.0);

            printf ("thread %i read/write time = %10.2f sec\n\n", i, ftime);
            printf ("thread %i I/O rate = %10.2f sec\n\n", i, 
	     ((float) myFileOpr[i].size) / ftime);
	    mySize = myFileOpr[i].size;

            if (OprMode == WRITE_OPR)
                srbObjUnlink (conn, myFileOpr[0].objName,
                 myFileOpr[0].collection);
            clFinish(conn);
    	    continue;
    	}

	
    	if (numInp == 1) {
	    /* use threads to transfer this file */
	    myOffset = 0;
	    sprintf (path, "%s/%s",  COLLECTION, argv[optind]);
	    if (OprMode == READ_OPR) {
	        retval = srbObjStat (conn, MDAS_CATALOG, path, &statbuf);
	        if (retval < 0) {
	    	    fprintf(stderr,
		      "Main thread srbObjStat error. retval = %d\n", retval);
            	    clFinish(conn);
            	    exit (1);
	        }
	    } else {
		if (ReadIn == DO_READ) {
	            retval = lstat (argv[optind], &statbuf);
                    if (retval < 0) {
                    	fprintf(stderr,"Main thread lstat error. retval = %d\n",
                    	 retval);
                        clFinish(conn);
                        exit (1);
		    }
                } else {
		    statbuf.st_size = DEF_FILE_SZ;
		}
	    }


	    size0 = statbuf.st_size / NumThreads;
	    size1 = statbuf.st_size - size0 * (NumThreads - 1);

	    maintime.stat = gethrtime ();

	    for (i = 0; i < NumThreads; i++) {
	        myFileOpr[i].collection = COLLECTION;
	        myFileOpr[i].objName = argv[optind];
	        if (i == NumThreads - 1)
		    myFileOpr[i].size = size1;
	        else
	    	    myFileOpr[i].size = size0;
	        myFileOpr[i].offset = myOffset;
	        myOffset += myFileOpr[i].size;
	        myFileOpr[i].rwFlag = OprMode;
	        myFileOpr[i].threadInx = i;
	        myFileOpr[i].threadtime = 
	          (struct threadTime *) malloc (sizeof (struct threadTime));

	        pthread_create(&tid[i], NULL, doPartialOpr, 
		  (void *) &myFileOpr[i]);
	    }

	    maintime.thrCreate = gethrtime ();

    	    for ( i = 0; i < NumThreads; i++) {
                pthread_join (tid[i], (void **) &tstatus[i]);
                printf ("pthread_join done for %d. status = %d\n", 
		  i, *tstatus[i]);
                /* printf ("pthread_join done for thread %d\n", i); */
    	    }

	    maintime.thrJoin = gethrtime ();

            if (OprMode == WRITE_OPR)
                srbObjUnlink (conn, myFileOpr[0].objName,
                 myFileOpr[0].collection);

        } else {
    	    maintime.stat = maintime.start;
    	    for ( i = 0; i < numInp; i++) {
                myFileOpr[i].collection = COLLECTION;
                myFileOpr[i].objName = argv[i + optind];
                myFileOpr[i].threadtime =
                 (struct threadTime *) malloc (sizeof (struct threadTime));

	        if (OprMode == WRITE_OPR)
	            pthread_create(&tid[i], NULL, putSrb, 
		      (void *)&myFileOpr[i]);
		else
	            pthread_create(&tid[i], NULL, getSrb, 
		     (void *)&myFileOpr[i]);
    	    }

	    maintime.thrCreate = gethrtime ();

    	    for ( i = 0; i < numInp; i++) {
                pthread_join (tid[i], (void **) &tstatus[i]);
                printf ("pthread_join done for %d. status = %d\n", 
		 i, *tstatus[i]);
                /* printf ("pthread_join done for thread %d\n", i); */
    	    }

	    maintime.thrJoin = gethrtime ();
        }

        maintime.end = gethrtime ();
        clFinish(conn);

	totMaintime.start += (maintime.end - maintime.start);
	totMaintime.stat += (maintime.stat - maintime.start);
	totMaintime.thrCreate += (maintime.thrCreate - maintime.stat);
	totMaintime.thrJoin += (maintime.thrJoin - maintime.thrCreate);

        /* printf timing */

        printf ("MAIN thread timing:\n");
        printf ("stat time = %10.2f sec\n", 
         ((float)(maintime.stat - maintime.start))/1000000000.0);
        printf ("thread create time = %10.2f sec\n", 
         ((float)(maintime.thrCreate - maintime.stat))/1000000000.0);
        printf ("thread join time = %10.2f sec\n", 
         ((float)(maintime.thrJoin - maintime.thrCreate))/1000000000.0);
        printf ("\nTHREAD thread timing:\n");
	mySize = 0;
        for (i = 0; i < NumThreads; i++) {

            totThreadtime.start += (myFileOpr[i].threadtime->start -
             maintime.start);
            totThreadtime.connect += (myFileOpr[i].threadtime->connect -
             myFileOpr[i].threadtime->start);
            totThreadtime.open += (myFileOpr[i].threadtime->open -
               myFileOpr[i].threadtime->connect);
	    totThreadtime.seek += (myFileOpr[i].threadtime->seek -
              myFileOpr[i].threadtime->open);
            totThreadtime.end += (myFileOpr[i].threadtime->end -
             myFileOpr[i].threadtime->open);
	    ftime = ((float) (myFileOpr[i].threadtime->end - 
	     myFileOpr[i].threadtime->open))/ 1000000000.0;
            totThreadtime.ioRate += ((float) myFileOpr[i].size) / ftime;

    	    printf ("thread %i start time = %10.2f sec\n", i, ((float)
	     (myFileOpr[i].threadtime->start - maintime.stat))/1000000000.0);
    	    printf ("thread %i connect time = %10.2f sec\n", i, ((float)
	     (myFileOpr[i].threadtime->connect - 
	       myFileOpr[i].threadtime->start))/ 1000000000.0);
    	    printf ("thread %i open time = %10.2f sec\n", 
	     i, ((float) (myFileOpr[i].threadtime->open - 
		myFileOpr[i].threadtime->connect))/ 1000000000.0);
    	    printf ("thread %i seek time = %10.2f sec\n", 
	      i, ((float) (myFileOpr[i].threadtime->seek - 
		myFileOpr[i].threadtime->open))/ 1000000000.0);

            printf ("thread %i read/write time = %10.2f sec\n\n", i, ftime);
            printf ("thread %i I/O rate = %10.2f sec\n\n", i, 
	     ((float) myFileOpr[i].size) / ftime);
	    mySize += myFileOpr[i].size;
        }
    }
	
    /* print average timing values */

    printf ("\n\n AVERAGE TIMING\n\n");
    printf ("Fize size = %d\n\n", mySize);
    printf ("MAIN thread timing:\n");
    printf ("wall clock time = %10.2f sec\n",
     ((float) totMaintime.start / (float) NumRun / 1000000000.0)); 
    printf ("stat time = %10.2f sec\n",
     ((float) totMaintime.stat / (float) NumRun /1000000000.0));
    printf ("thread create time = %10.2f sec\n",
     ((float) totMaintime.thrCreate / (float) NumRun /1000000000.0));
    printf ("thread join time = %10.2f sec\n",
     ((float) totMaintime.thrJoin / (float) NumRun /1000000000.0));
    printf ("\nTHREAD thread timing:\n");

    printf ("thread wall clock time = %10.2f sec\n", 
     ((float) totThreadtime.start / (float) NumRun / 
       (float) NumThreads / 1000000000.0));
    printf ("thread connect time = %10.2f sec\n",
     ((float) totThreadtime.connect / (float) NumRun / 
       (float) NumThreads / 1000000000.0));
    printf ("thread open time = %10.2f sec\n",
      ((float) totThreadtime.open / (float) NumRun / 
        (float) NumThreads / 1000000000.0));
    printf ("thread seek time = %10.2f sec\n",
     ((float) totThreadtime.seek / (float) NumRun / 
      (float) NumThreads / 1000000000.0));
    printf ("thread read/write time = %10.2f sec\n\n",
     ((float) totThreadtime.end / (float) NumRun / 
      (float) NumThreads / 1000000000.0));
    printf ("thread I/O rate = %10.2f sec\n\n",
     ((float) totThreadtime.ioRate / (float) NumRun / 
      (float) NumThreads));

    printf ("main thread exiting\n");
    exit (0);
}

	
void *getSrb (void *inFileOpr)
{
    srbConn *conn;
    int  i, nbytes,  in_fd, out_fd;
    char *buf;
    struct fileOpr *myFileOpr;
    int *retval;

    myFileOpr = (struct fileOpr *) inFileOpr;
    myFileOpr->threadtime->start = gethrtime ();

    conn = clConnect (HOST_ADDR, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to exfMaster failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
    	clFinish(conn);
	*retval = -1;
	return ((void *) retval);
    }
	
    myFileOpr->threadtime->connect = gethrtime ();

    retval = (int *) malloc (sizeof (int));
    buf = malloc (BufSize);
    if (WriteOut == DO_WRITE) {
    	out_fd = open (myFileOpr->objName,  O_RDWR | O_CREAT, 0644);
    	if (out_fd < 0) {
	    fprintf(stderr, "can't open local file %s\n", myFileOpr->objName);
    	    clFinish(conn);
	    *retval = -2;
	    return ((void *) retval);
	}
    }

    in_fd = srbObjOpen (conn, myFileOpr->objName, O_RDWR, 
      myFileOpr->collection);
    if (in_fd < 0)  {   /* error */
        fprintf(stderr, "can't open srb obj\"%s\"\n", myFileOpr->objName);
    	clFinish(conn);
	*retval = -3;
	return ((void *) retval);
    }

    myFileOpr->threadtime->open = gethrtime ();
    myFileOpr->threadtime->seek = myFileOpr->threadtime->open;

    myFileOpr->size = 0;
    while ((nbytes = srbObjRead (conn, in_fd, buf, BufSize)) > 0) {
        if (WriteOut == DO_WRITE) {
	  if (write (out_fd, buf, nbytes) <= 0) {
      	    fprintf(stderr, "local write errror for %s, errno = %d\n", 
	      myFileOpr->objName, errno);
    	    srbObjClose (conn, in_fd);
    	    clFinish(conn);
	    *retval = -4;
	    return ((void *) retval);
	  }
	}
	myFileOpr->size += nbytes;
    }

    free (buf);
    srbObjClose (conn, in_fd);
    if (WriteOut == DO_WRITE) 
    	close (out_fd);
    printf ("finish writing local file %s\n", myFileOpr->objName);
    fflush (stdout);
    
    clFinish(conn);

    myFileOpr->threadtime->end = gethrtime ();

    *retval = -12345;
    return ((void *) retval);
}

void *putSrb (void *inFileOpr)
{
    srbConn *conn;
    int  i, nbytes,  in_fd, out_fd;
    char *buf;
    struct fileOpr *myFileOpr;
    int *retval;
    int sizeRemaining;
    int status;

    myFileOpr = (struct fileOpr *) inFileOpr;
    myFileOpr->threadtime->start = gethrtime ();

    conn = clConnect (HOST_ADDR, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to exfMaster failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
    	clFinish(conn);
	*retval = -1;
	return ((void *) retval);
    }
	
    myFileOpr->threadtime->start = gethrtime ();

    retval = (int *) malloc (sizeof (int));
    buf = malloc (BufSize);
    if (ReadIn == DO_READ) {
    	in_fd = open (myFileOpr->objName,  O_RDWR, 0644);
    	if (in_fd < 0) {
	    fprintf(stderr, "can't open local file %s\n", myFileOpr->objName);
    	    clFinish(conn);
	    *retval = -2;
	    return ((void *) retval);
	}
    }

    out_fd = srbObjCreate (conn, MDAS_CATALOG, myFileOpr->objName,
     NULL, NULL, myFileOpr->collection, NULL, -1);

    if (out_fd < 0)  {   /* error */
        fprintf(stderr, "can't open srb obj\"%s\"\n", myFileOpr->objName);
    	clFinish(conn);
	*retval = -3;
	return ((void *) retval);
    }

    myFileOpr->threadtime->open = gethrtime ();
    myFileOpr->threadtime->seek = myFileOpr->threadtime->open;

    myFileOpr->size = 0;
    sizeRemaining = DEF_FILE_SZ;

    while (sizeRemaining > 0) {
	if (ReadIn == DO_READ) {
	    nbytes = read (in_fd, buf, BufSize);
	    if (nbytes <= 0)
		break;
	    sizeRemaining = nbytes;
	} else {
	    nbytes = BufSize;
	    sizeRemaining -= nbytes;
	}


        if ((status = srbObjWrite (conn, out_fd, buf, nbytes)) <= 0) {
            fprintf(stderr, "srb write errror for %s, errno = %d\n",
             myFileOpr->objName, status);
            srbObjClose (conn, out_fd);
            close (in_fd);
            clFinish(conn);
            *retval = -6;
            return ((void *) retval);
        }
	myFileOpr->size += nbytes;
    }

    free (buf);
    srbObjClose (conn, in_fd);
    if (WriteOut == DO_WRITE) 
    	close (out_fd);
    printf ("finish writing local file %s\n", myFileOpr->objName);
    fflush (stdout);
    
    clFinish(conn);

    myFileOpr->threadtime->end = gethrtime ();

    *retval = -12345;
    return ((void *) retval);
}

void *doPartialOpr (void *inFileOpr)
{
    struct fileOpr *myFileOpr;
    srbConn *conn;
    int  i, nbytes,  in_fd, out_fd;
    char *buf;
    int *retval;
    int status;
    int ioSize, sizeRemaining;
    int retryCnt = 0;

    
    myFileOpr = (struct fileOpr *) inFileOpr;
    myFileOpr->threadtime->start = gethrtime ();

    conn = clConnect (HOST_ADDR, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to exfMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        clFinish(conn);
        *retval = -1;
        return ((void *) retval);
    }

    myFileOpr->threadtime->connect = gethrtime ();

    retval = (int *) malloc (sizeof (int));
    buf = malloc (BufSize);

    if (myFileOpr->rwFlag == READ_OPR) {
	if (WriteOut == DO_WRITE) {
	    out_fd = open (myFileOpr->objName,  O_RDWR | O_CREAT, 0644);
    	    if (out_fd < 0) {
            	fprintf(stderr, 
		  "can't open local file %s\n", myFileOpr->objName);
                clFinish(conn);
                *retval = -2;
                return ((void *) retval);
	    }
    	}

    	in_fd = srbObjOpen (conn, myFileOpr->objName, O_RDWR, 
	 myFileOpr->collection);
    	if (in_fd < 0)  {   /* error */
            fprintf(stderr, "can't open srb obj\"%s\"\n", myFileOpr->objName);
            clFinish(conn);
            *retval = -3;
            return ((void *) retval);
    	}

    	myFileOpr->threadtime->open = gethrtime ();

	if (myFileOpr->offset != 0) {
	    status = srbObjSeek (conn, in_fd, myFileOpr->offset, SEEK_SET);
	    if (status != myFileOpr->offset) {
            	fprintf(stderr, "srbObjSeek loc %d != input %d\n", 
	     	 status, myFileOpr->offset);
            	clFinish(conn);
            	*retval = -4;
            	return ((void *) retval);
            }
	}


        if (myFileOpr->offset != 0) {
	  if (WriteOut == DO_WRITE) {
            status = lseek (out_fd, myFileOpr->offset, SEEK_SET);
            if (status != myFileOpr->offset) {
                fprintf(stderr, "lseek loc %d != input %d\n", 
                 status, myFileOpr->offset);
                clFinish(conn);
                *retval = -5;
                return ((void *) retval);
            }
	  }
        }
    	myFileOpr->threadtime->seek = gethrtime ();

	sizeRemaining = myFileOpr->size;

	while (sizeRemaining > 0) {
	    if (myFileOpr->size >= BufSize) {
	    	ioSize = BufSize;
	    } else {
		ioSize = myFileOpr->size;
	    }
	    nbytes = srbObjRead (conn, in_fd, buf, ioSize);
	    if (nbytes <= 0)
		break;

	    if (WriteOut == DO_WRITE) {
              if (write (out_fd, buf, nbytes) <= 0) {
            	fprintf(stderr, "local write errror for %s, errno = %d\n",
              	 myFileOpr->objName, errno);
            	srbObjClose (conn, in_fd);
		close (out_fd);
            	clFinish(conn);
            	*retval = -6;
            	return ((void *) retval);
	      }
            }
	    sizeRemaining -= nbytes;
    	}
    	srbObjClose (conn, in_fd);
    	close (out_fd);
    } else if (myFileOpr->rwFlag == WRITE_OPR) {
	if (ReadIn == DO_READ) {
            in_fd = open (myFileOpr->objName,  O_RDONLY, 0644);
            if (in_fd < 0) {
            	fprintf(stderr, "can't open local file %s\n", 
		myFileOpr->objName);
            	clFinish(conn);
            	*retval = -2;
            	return ((void *) retval);
	    }
        }

	if (myFileOpr->threadInx == 0) {
	    pthread_mutex_lock (&Glock);
	    out_fd = srbObjCreate (conn, MDAS_CATALOG, myFileOpr->objName,
	     NULL, NULL, myFileOpr->collection, NULL, -1);
	    pthread_mutex_unlock (&Glock);
            if (out_fd < 0)  {   /* error */
            	fprintf(stderr, 
		 "can't create srb obj\"%s\"\n", myFileOpr->objName);
            	clFinish(conn);
            	*retval = out_fd;
            	return ((void *) retval);
	    }
        } else {
	    while (retryCnt < MAX_RETRY) {
	        pthread_mutex_lock (&Glock);
	        pthread_mutex_unlock (&Glock);
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
                clFinish(conn);
                *retval = out_fd;
                return ((void *) retval);
            }
	}
    	myFileOpr->threadtime->open = gethrtime ();

        if (myFileOpr->offset != 0) {
            status = srbObjSeek (conn, out_fd, myFileOpr->offset, SEEK_SET);
            if (status != myFileOpr->offset) {
                fprintf(stderr, "srbObjSeek loc %d != input %d\n",
                 status, myFileOpr->offset);
                clFinish(conn);
                *retval = -4;
                return ((void *) retval);
            }
        }

        if (ReadIn == DO_READ && myFileOpr->offset != 0) {
            status = lseek (in_fd, myFileOpr->offset, SEEK_SET);
            if (status != myFileOpr->offset) {
                fprintf(stderr, "lseek loc %d != input %d\n",
                 status, myFileOpr->offset);
                clFinish(conn);
                *retval = -5;
                return ((void *) retval);
            }
        }

    	myFileOpr->threadtime->seek = gethrtime ();

	sizeRemaining = myFileOpr->size;
        while (sizeRemaining > 0) {
            if (myFileOpr->size >= BufSize) {
                ioSize = BufSize;
            } else {
                ioSize = myFileOpr->size;
            }
	    if (ReadIn == DO_READ) {
            	nbytes = read (in_fd, buf, ioSize);
            	if (nbytes <= 0)
                    break;
	    } else {
		nbytes = BufSize;
	    }

            if ((status = srbObjWrite (conn, out_fd, buf, nbytes)) <= 0) {
                fprintf(stderr, "local write errror for %s, errno = %d\n",
                 myFileOpr->objName, status);
                srbObjClose (conn, out_fd);
                close (in_fd);
                clFinish(conn);
                *retval = -6;
                return ((void *) retval);
            }
            sizeRemaining -= nbytes;
        }
        srbObjClose (conn, out_fd);
	if (ReadIn == DO_READ) 
            close (in_fd);
    }

    free (buf);
    printf ("finish processing srb file %s\n", myFileOpr->objName);
    fflush (stdout);

    clFinish(conn);

    myFileOpr->threadtime->end = gethrtime ();

    *retval = -12345;
    return ((void *) retval);
}


