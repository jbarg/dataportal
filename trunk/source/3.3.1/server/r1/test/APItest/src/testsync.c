/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testsync.c--
 *    test the srbSyncData() call
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
    char buf[BUFSIZE];
    struct stat *statbuf;
    int acc_method, num_of_loop, loop_cnt;
    mdasC_sql_result_struct myresult;
    struct clHostElement *hostQueHead;
    char *resource;
    int status;

 
    if (argc == 3) {
	resource = argv[2];
    } else if (argc == 2) {
	resource = "";
    } else {
        fprintf(stderr, "Usage: %s objID (resource)\n",
         argv[0]);
        exit(1);
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
	

    printf ("Synchronizing obj : %s to resource %s. \n", argv[1], resource);
    status = srbSyncData (conn, MDAS_CATALOG, argv[1],
    COLLECTION, resource);
    if (status < 0)  {   /* error */
        fprintf(stderr, "can't sync obj \"%s\", status = %d\n", argv[1],
	status);
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely(conn);
    }

    clFinish(conn);
    exit(0);
}
