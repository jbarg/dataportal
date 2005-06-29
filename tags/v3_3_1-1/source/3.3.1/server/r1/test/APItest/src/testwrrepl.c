/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testwrrepl.c--
 *    test writing replica copies 
 *
 *-------------------------------------------------------------------------
 */
#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#ifdef PORTNAME_solaris
#include <fcntl.h>
#endif

#include "srbClient.h"

#define BUFSIZE		1028
#define srbAuth		NULL
#define UNIX_AM		0	/* UNIX access method */
#define UTREE_AM	1
#define HPSS_AM		2
#define HOST_ADDR	NULL

#define DATATYPE	"ascii text"
#define RESOURCE	"unix-sdsc"
#define COLLECTION	"/srbtest"

char *inFileName = "../../testfiles/test_in";
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
    int i, nbytes, tmp, in_fd, out_fd;
    char buf[BUFSIZE];
    struct stat *statbuf;
    int acc_method, num_of_loop, loop_cnt;
    mdasC_sql_result_struct myresult;
    struct clHostElement *hostQueHead;
    int status;
    char *resourceName;  

 
    if (argc != 2) {
        fprintf(stderr, "Usage: %s objID\n",
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
	

    printf ("opening obj : %s\n", argv[1]);
    out_fd = srbObjOpen (conn, argv[1], O_WR_REPL, COLLECTION);
    if (out_fd < 0)  {   /* error */
        fprintf(stderr, "can't open obj \"%s\", status = %d\n", argv[1],
	out_fd);
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely(conn);
    }

    in_fd = open (inFileName, O_RDONLY, 0);
    if (in_fd < 0)  {   /* error */
        fprintf(stderr, "can't open file\"%s\"\n",
        inFileName);
        exit_nicely(conn);
    }

    i = 0;
    printf ("writing obj : %s\n", argv[1]);
    while ((nbytes = read(in_fd, buf, BUFSIZE)) > 0) {
        i += nbytes;
        tmp = srbObjWrite(conn, out_fd, buf, nbytes);
        if (tmp < nbytes) {
            fprintf(stderr, "Error: Read %d bytes, Wrote %d bytes.\n ",
            nbytes, tmp);
            exit_nicely(conn);
        }
    }

    printf ("\n\nBytes written: %d\n", i);
    srbObjClose (conn, out_fd);
    close (in_fd);

    clFinish(conn);
    exit(0);
}
