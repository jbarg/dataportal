/*-------------------------------------------------------------------------
 *
 * dumptape.c--
 *    test moving an object 
 *-------------------------------------------------------------------------
 */
#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#ifdef PORTNAME_solaris
#include <fcntl.h>
#endif

#include "srbClient.h"

#define SRB_AUTH	NULL
#define HOST_ADDR	NULL

#define NUM_FILES_TO_PROC  20

void
usage (char *prog);

void 
exit_nicely(srbConn* conn)
{
  clFinish(conn);
  exit(1);
}

int
main(int argc, char **argv)
{
    srbConn *conn;
    srbResult *res;
    int i, nbytes, tmp, out_fd;
    mdasC_sql_result_struct myresult;
    fileList_t myFileList;
    char *fileName;
    int status;
    int c;
    char *inputFile = NULL;
    int purgeFlag = 0;
    int numFiles = 0;
    

 
    if (argc < 2) {
	usage (argv[0]);
        exit(1);
    }

    while ((c=getopt(argc, argv,"pf:n:")) != EOF) {
        switch (c) {
            case 'p':
		purgeFlag = 1;
                break;
	    case 'f':
		inputFile = strdup (optarg);
                break;
            case 'n':
                numFiles = atoi (optarg);
                break;

            default:
		usage (argv[0]);
		exit(1);
	 	break;
        }
    }

    /*
     * set up the connection
     */
    conn = clConnect (HOST_ADDR, NULL, SRB_AUTH);

    /* check to see that the backend connection was successfully made */
    if ((status = clStatus(conn)) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to exfMaster failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely(conn);
    }
    
    if (inputFile == NULL) {
        myFileList.fileName = malloc (MAX_DATA_SIZE * (argc - 1));
        fileName = (char *) myFileList.fileName;
        myFileList.numFile = argc - optind;

        for (i = 0; i < myFileList.numFile; i++) {
    	    strcpy (&fileName[i*MAX_DATA_SIZE], argv[i + optind]);
            printf ("Dumping file: %s to tape. \n", &fileName[i*MAX_DATA_SIZE]);
        }
        status = srbDumpFileList (conn, 0, &myFileList, purgeFlag);
        if (status < 0)  {   /* error */
            fprintf(stderr, "dump error, status = %d ", status);
	    fprintf(stderr,"%s",clErrorMessage(conn));
	    free (myFileList.fileName);
	    clFinish(conn);
	    exit (1);
        }
    } else {
        FILE *file;

	if (numFiles <= 0)
	    numFiles = NUM_FILES_TO_PROC;

        myFileList.fileName = malloc (MAX_DATA_SIZE * numFiles);
	fileName = (char *) myFileList.fileName;
	
        file = fopen (inputFile, "r");

        if (file == NULL) {
            fprintf(stderr, "fopen of %s error, errno = %d ", 
	      inputFile, errno);
            free (myFileList.fileName);
            clFinish(conn);
            exit (1);
        }

	i = myFileList.numFile = 0;
	while (fscanf (file, "%s", &fileName[i*MAX_DATA_SIZE]) > 0) {
	    i ++;
	    myFileList.numFile++;
	    if (myFileList.numFile >= numFiles) {
		printf ("dumping %d files, 1st file: %s\n",
		    myFileList.numFile, fileName);
                status = srbDumpFileList (conn, 0, &myFileList, purgeFlag);
        	if (status < 0)  {   /* error */
                    fprintf(stderr, "dump error, status = %d ", status);
                    fprintf(stderr,"%s",clErrorMessage(conn));
                    free (myFileList.fileName);
                    clFinish(conn);
                    exit (1);
                }
		i = myFileList.numFile = 0;
	    }
	}
        fclose (file);
	if (myFileList.numFile > 0) {
            printf ("dumping %d files, 1st file: %s\n",
              myFileList.numFile, fileName);
            status = srbDumpFileList (conn, 0, &myFileList, purgeFlag);
            if (status < 0)  {   /* error */
                fprintf(stderr, "dump error, status = %d ", status);
                fprintf(stderr,"%s",clErrorMessage(conn));
                free (myFileList.fileName);
                clFinish(conn);
                exit (1);
            }
	}
    }

    free (myFileList.fileName);
    clFinish(conn);
    exit(0);
}

void
usage (char *prog)
{
    fprintf(stderr,"Usage  :%s [-p] [-f filelistFile] file1 file2 ...\n",
      prog);
    fprintf (stderr,"file1, file2 are the physical paths of the files to be dumped to tape\n");
    fprintf(stderr,"-p - purge the file after dumping to tape \n");
    fprintf(stderr,"-f filelistFile. Instead of specifying file1, file2 ..., the list files to be dumped can be specified in a file named filelistFile.\n");
}

