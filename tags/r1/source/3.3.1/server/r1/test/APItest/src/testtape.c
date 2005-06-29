/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testtape.c--
 *    test the srbDumpFileList() call
 *-------------------------------------------------------------------------
 */
#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#ifdef PORTNAME_solaris
#include <fcntl.h>
#endif

#include "srbClient.h"

#define BUFSIZE		4096
#define srbAuth		NULL
#define UNIX_AM		0	/* UNIX access method */
#define UTREE_AM	1
#define HPSS_AM		2
#define HOST_ADDR	NULL

#define DATATYPE	"ascii text"
#define RESOURCE	"unix-sdsc"
#define COLLECTION	"/srbtest"

#define NUM_FILES_TO_PROC  20
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
        fprintf(stderr, "Usage: %s fileName ...\n",
         argv[0]);
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
	 	break;
        }
    }

    /*
     * set up the connection
     */
    conn = clConnect (HOST_ADDR, NULL, srbAuth);

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
