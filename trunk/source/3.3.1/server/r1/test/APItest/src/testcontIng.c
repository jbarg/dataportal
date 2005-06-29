/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testcontIng.c--
 *    test direct ingestion of object into container.
 *
 *-------------------------------------------------------------------------
 */

#include "srbClient.h"

#define BUFSIZE		4096
#define srbAuth		NULL
#define UNIX_AM		0	/* UNIX access method */
#define UTREE_AM	1
#define HPSS_AM		2
#define HOST_ADDR	NULL
#define DATATYPE        "generic"
#define RESOURCE        "unix-sdsc"
#define LOG_RESC        "test-log-resc"
#define COLLECTION      "/home/srb"
#define CONTAINER_SZ	100000
#define CONTAINER_NM	"foocont2"	/* this container must already exist */
#define INFILE0		"../src/testcoll.c"
#define INFILE1		"../src/testexf.c"
#define OUTFILE0		"out1.c"
#define OUTFILE1		"out2.c"
#define ROWS_WANTED	2

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
    int i, nbytes, tmp, in_fd, out_fd, total;
    char buf[BUFSIZE];
    struct stat *statbuf;
    int acc_method;
    mdasC_sql_result_struct *myresult;
    struct clHostElement *hostQueHead;
    int status;
    mdasC_sql_result_struct *collResult;
    char *infile[2];
    char *outfile[2];
    int contFd;
    char tmpStr[200];
    
    infile[0] = INFILE0;
    infile[1] = INFILE1;
    outfile[0] = OUTFILE0;
    outfile[1] = OUTFILE1;

    /*
     * set up the connection
     */
    conn = clConnect (HOST_ADDR, NULL, srbAuth);

    /* check to see that the backend connection was successfully made */
    if ((status = clStatus(conn)) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to srbMaster failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely(conn);
    }
	
    /* open the container */

    contFd = srbContainerOpen (conn, MDAS_CATALOG, CONTAINER_NM, O_WRONLY);

    if (contFd < 0)  {   /* error */
        fprintf(stderr, "can't open container \"%s\", status = %d\n",
         CONTAINER_NM, contFd);
        exit_nicely(conn);
    }

    for (i = 0; i < 2; i++) {
	sprintf (tmpStr, "%s&CONT_FD=%d", outfile[i], contFd);
	out_fd = srbObjCreate (conn, MDAS_CATALOG, tmpStr, "", "",
	 COLLECTION, "", -1);
	if (out_fd < 0) {
	    fprintf(stderr, "can't create object %s, status = %d\n", 
	     tmpStr, out_fd);
	}

    	in_fd = open (infile[i], O_RDONLY, 0);
    	if (in_fd < 0)  {   /* error */
            fprintf(stderr, "can't open file\"%s\"\n",
             infile[i]);
            exit_nicely(conn);
    	}

    	printf ("writing obj : %s\n", outfile[i]);
        while ((nbytes = read(in_fd, buf, BUFSIZE)) > 0) {
            total += nbytes;
            tmp = srbObjWrite(conn, out_fd, buf, nbytes);
            if (tmp < nbytes) {
            	fprintf(stderr, "Error: Read %d bytes, Wrote %d bytes.\n ",
                nbytes, tmp);
            	exit_nicely(conn);
            }
    	}

        printf ("\n\nBytes written: %d\n", total);
	srbObjClose (conn, out_fd);
	close (in_fd);
    }
    srbContainerClose (conn, contFd);


#ifdef foo
    printf ("Creating container : %s\n", argv[1]);
    status = srbContainerCreate (conn, MDAS_CATALOG, argv[1], DATATYPE, 
     LOG_RESC, CONTAINER_SZ);
    if (status < 0)  {   /* error */
        fprintf(stderr, "can't create container \"%s\", status = %d\n", 
	 argv[1], status);
	exit_nicely(conn);
    }

    	i = 0;
	printf ("Reading obj : %s\n", argv[1]);
    	while ((nbytes = srbObjRead (conn, in_fd, buf, BUFSIZE)) > 0) {
	    i += nbytes;
	    printf ("Content of the object: \n%s\n", buf);
    	}

    	if (nbytes < 0) {
            fprintf(stderr, "Error reading, status = %d\n", nbytes);
            fprintf(stderr,"%s",clErrorMessage(conn));
    	}
	
	printf ("\n\nBytes read: %d\n", i);
        printf ("----------------------------------------------------------\n");
    	srbObjClose (conn, in_fd);
#endif
    clFinish(conn);
    exit(0);
}
