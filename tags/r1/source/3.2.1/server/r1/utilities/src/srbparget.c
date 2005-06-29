#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif
 
#include "scommands.h"
#if defined(PORTNAME_sgi)
#define lseek	lseek64
#define lstat   lstat64
#endif

extern char srbHost[];
extern char inCondition[MAX_OBJ_NAME_LENGTH];
extern char mdasCollectionName[];
void *doPartialOpr (void *inFileOpr);

/***
int usage(char *name) {
        fprintf(stderr,"usage: %s [-vh] [-n copyNum] [-m numofstreams] -H remoteSRBHost -S seek -L length  -U user@domain -A authFileName srbObject localFile\n", name );
	fprintf(stderr,"-v is a flag for Verbose\n");
	fprintf(stderr,"-h this help message\n");
	fprintf(stderr,"-r recursive get\n");
	fprintf(stderr,"-n copy number of the source;\n");
	fprintf(stderr,"-R remote SRB Host to connect\n");
	fprintf(stderr,"-S seek in the remote file; zero by default\n");
	fprintf(stderr,"-L length of partial file to get; full size by default\n");
	fprintf(stderr,"-U user name and her domain; by default taken from environment variable or MdasEnv file  \n");
	fprintf(stderr,"-A file containing the authorization information when using ENCRYPT1 password option; by default taken from MDASAUth file\n");
	return(0);
}
***/
int usage(char *name) {
        fprintf(stderr,"usage: %s [-vh] [-n copyNum] [-m numofstreams] -H remoteSRBHost -S seek -L length srbObject localFile\n", name );
	fprintf(stderr,"-v is a flag for Verbose\n");
	fprintf(stderr,"-h this help message\n");
	fprintf(stderr,"-r recursive get\n");
	fprintf(stderr,"-n copy number of the source;\n");
	fprintf(stderr,"-m number of parallel streams (threads) to use in this segment of data transfer\n");
	fprintf(stderr,"-H remote SRB Host to connect\n");
	fprintf(stderr,"-S seek in the remote file; zero by default\n");
	fprintf(stderr,"-L length of partial file to get; full size by default\n");
	return(0);
}

int main(int argc, char **argv)
{   
    int  i;
    int copyNum = -1;
    int numOfStreams = -1;
    int flagval=0;
    char c;

    srbPathName srbObjName;
    char localFile[2*MAX_TOKEN];
    char hostName[MAX_TOKEN];
    srb_long_t seeksize;
    srb_long_t lengthsize;
    int lstatus;
    char temp[2*MAX_TOKEN];
    char objCollName[2*MAX_TOKEN];
    char objDataName[2*MAX_TOKEN];

    strcpy(mdasCollectionName,"");
    strcpy(inCondition,"");
    seeksize =  0;
    lengthsize = -1;
    strcpy(hostName,srbHost);

    while ((c=getopt(argc, argv,"m:n:H:S:L:hv")) != EOF) {
      switch (c) {
        case 'n':
            copyNum = strtoll (optarg, 0, 0);
            break;
        case 'm':
            numOfStreams = strtoll (optarg, 0, 0);
	    if (numOfStreams > MAX_NUM_THREADS) {
	      printf("Number of Threads Exceed Supported Maximum Number %i\n",
		     MAX_NUM_THREADS);
	      exit(-1);
	    }
            break;
	case 'H':
	    strcpy(hostName, optarg);
	    break;
	case 'v':
	    flagval |= v_FLAG;
	    break;
        case 'S':
            seeksize = atoi(optarg);
            break;
        case 'L':
            lengthsize = atoi(optarg);
            break;
        case 'h':
         default:
	    usage (argv[0]);
	    exit (1);
      }
    }
    if ((argc - optind) != 2 || lengthsize == -1) {
      printf("Error in Usage. Needs more arguments\n");
      usage (argv[0]);
      exit (1);
    }
      
    srbObjName.inpArgv = argv[argc-2];
    strcpy(localFile,argv[argc-1]);

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}

  if (parseName (&srbObjName, mdasCollectionName) < 0) {
        fprintf (stderr, "Error: bad srbObjectName %s\n", argv[argc-2]);
        exit (1);
    }

    splitpath(srbObjName.outArgv,objCollName,objDataName);

    if (copyNum >=  0) {
        sprintf(temp, "%s&COPY=%i", objDataName, copyNum);
    } else {
        strcpy (temp, objDataName);
    }
    if (strlen(inCondition) > 0) {
      strcat(temp,"&");
      strcat(temp,inCondition);
    }

    if (numOfStreams == -1)
      i = getDataFromSrbToFileSingleStream(hostName,temp,objCollName,
			     localFile,seeksize,lengthsize,flagval);
    else 
      i = getDataFromSrbToFileMultiStreams(hostName,temp,objCollName,
			      localFile,seeksize,lengthsize,flagval,
					   numOfStreams );
    if (i < 0) {
      printf("Error in File Transfer:%i\n",i );
      exit(-1);
    }
    exit(0);

}

int
getDataFromSrbToFileSingleStream(char *hostName, char *objDataName, 
				 char *objCollName, char *localFile,
				 srb_long_t  seeksize,srb_long_t lengthsize, 
				 int flagval)
{

    srbConn *conn;
    int lstatus;
    int out_fd,in_fd;
    int ioSize, nbytes;
    srb_long_t  sstatus, sizeRemaining;
    char buf[BUFSIZE];


    conn = srbConnect (hostName, NULL, NULL, NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) 
      {
        printf("Error: Connection to SRB server  Failed for Host at %s.\n",hostName);
        printf("%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn);
	exit (1);
	
      }

    if (flagval & v_FLAG) {
      fprintf (stderr,"Opening localFile %s ...",localFile);
      fflush(stderr);
    }
    
    out_fd = open (localFile,  O_RDWR | O_CREAT, 0644);
    if (out_fd < 0) {
      fprintf(stderr,
	      "Error: Opening local file %s\n", localFile);
      clFinish(conn);
      exit (1);
    }
    if (flagval & v_FLAG) {
      fprintf (stderr,"done.\n");
      fflush(stderr);
    }
    if (flagval & v_FLAG) {
      fprintf (stderr,"Opening srbObject %s/%s ...", objCollName,objDataName);
      fflush(stderr);
    }
    in_fd = srbObjOpen (conn, objDataName, O_RDONLY,
			objCollName);
    if (in_fd < 0)  {   /* error */
      fprintf(stderr, "can't open srb obj\"%s/%s\"\n", 
	      objCollName,objDataName);
      clFinish(conn);
      exit (1);
    }
    if (flagval & v_FLAG) {
      fprintf (stderr,"done.\n");
      fflush(stderr);
    }

    if (seeksize != 0) {
      sstatus = srbObjSeek (conn, in_fd, seeksize, SEEK_SET);
      if (sstatus != seeksize) {
	fprintf(stderr, "srbObjSeek loc %lld != input %lld\n",
		sstatus, seeksize);
	close(out_fd);
	srbObjClose(conn, in_fd);
	clFinish(conn);
	exit (1);
      }
      sstatus = lseek (out_fd, seeksize, SEEK_SET);
      if (sstatus != seeksize) {
	fprintf(stderr, "llseek loc %lld != input %lld, errno=%d\n",
		sstatus, seeksize, (int) errno);
	srbObjClose(conn,in_fd);
	close(out_fd);
	clFinish(conn);
	exit (1);
      }
    }
      if (lengthsize >= 0)
	sizeRemaining = lengthsize;
      else
	sizeRemaining = -1;

    while (sizeRemaining == -1 || sizeRemaining > 0) {
      if ( sizeRemaining  >= BUFSIZE) {
	ioSize = BUFSIZE;
      } else {
	ioSize = sizeRemaining;
      }
      nbytes = srbObjRead (conn, in_fd, buf, (int) ioSize);
      if (nbytes <= 0)
	break;
      if (write (out_fd, buf, nbytes) <= 0) {
	fprintf(stderr, "local write errror for %s, errno = %d\n",
		localFile, errno);
	srbObjClose (conn, in_fd);
	close (out_fd);
	clFinish(conn);
	exit(-1);
      }
      sizeRemaining -= (srb_long_t) nbytes;
      if (flagval & v_FLAG) {
	fprintf (stderr," Read %i\n ",lengthsize - sizeRemaining);
	fflush(stderr);
      }
    }
    if (flagval & v_FLAG) {
      fprintf (stderr,"\nClosing srbObject ...");
      fflush(stderr);
    }
    srbObjClose (conn, in_fd);
    if (flagval & v_FLAG) {
      fprintf (stderr,"done\n");
      fflush(stderr);
    }
    if (flagval & v_FLAG) {
      fprintf (stderr,"\nClosing local file ...");
      fflush(stderr);
    }
    close (out_fd);
    if (flagval & v_FLAG) {
      fprintf (stderr,"done\n");
      fflush(stderr);
    }
    clFinish(conn);
    if (flagval & v_FLAG) {
      fprintf (stderr,"Data Transfer Finished.\n");
      fflush(stderr);
    }
    return(0);
}

    
int
getDataFromSrbToFileMultiStreams(char *hostName, char *objDataName, 
				char *objCollName, char *localFile,
				srb_long_t inOffset, srb_long_t datasize, 
				int flagval, int NumThreads)
{
  
    int  i;
    srb_long_t  size0, size1;
    struct fileOpr myFileOpr[MAX_NUM_THREADS];
    pthread_t tid[MAX_NUM_THREADS];
    int *tstatus[MAX_NUM_THREADS];
    srb_long_t myOffset;
    
    size0 = datasize / NumThreads;
    size1 = datasize - size0 * (NumThreads - 1);
    myOffset = inOffset;
      if (flagval & v_FLAG) {
	fprintf (stderr,"Number of Threads = %d Using size %lld for each thread\n Starting at Offset %lld\n",NumThreads, size0,myOffset);
	fflush(stderr);
      }
    for (i = 0; i < NumThreads; i++) {
      myFileOpr[i].collection = objCollName;
      myFileOpr[i].objName = objDataName;
      myFileOpr[i].localFileName = localFile;
      if (i == NumThreads - 1)
	myFileOpr[i].size = size1;
      else
	myFileOpr[i].size = size0;
      myFileOpr[i].offset = myOffset;
      myOffset += myFileOpr[i].size;
      myFileOpr[i].rwFlag =  READ_OPR;
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
    
  return(0);
}
