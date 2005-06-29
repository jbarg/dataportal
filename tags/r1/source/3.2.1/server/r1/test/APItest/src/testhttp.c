/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/* testhttp.c - Test the Extended File API for the HTTP.
 */

#include "srbClient.h"

#define BUFSIZE         512
#define srbAuth         NULL
#define UNIX_AM         0       /* UNIX access method */
#define UTREE_AM        1
#define HPSS_AM         2
#define HOST_ADDR       NULL
#define DATATYPE        "ascii text"
#define RESOURCE        "unix-sdsc"
#define COLLECTION      "/srbtest"


void
exit_nicely(srbConn* conn)
{
  clFinish(conn);
  exit(1);
}

main(int argc, char **argv)
{
    srbConn *conn;
    int i, nbytes, fd;
    int status;

    char buf[BUFSIZE];

    if ( argc  != 2) {
    	fprintf(stderr, "Usage: %s url_pathname\n", argv[0]);
    	printf
          ("e.g.: %s 'http://www.sdsc.edu:80/~sekar/mdas/patentsystemf.html'\n",
	    argv[0]);
          exit(-1);
    }


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

    printf ("Opening HTTP file : %s\n", argv[1]);

    fd = srbFileOpen (conn, HTTP_FT, "", argv[1], O_RDONLY, 0666);
    if (fd < 0)  {   /* error */
        fprintf(stderr, "can't open file %s, status=%d\n",
        argv[1], fd);
        exit_nicely(conn);
    }

    i = 0;
    printf ("Reading %s\n", argv[1]);
    while ((nbytes = srbFileRead (conn, fd, buf, BUFSIZE)) > 0) {
        i += nbytes;
        printf ("Content of data block :  \n%s\n", buf);
    }

    if (nbytes < 0) {
        fprintf(stderr, "Error reading, status = %d\n", nbytes);
        fprintf(stderr,"%s",clErrorMessage(conn));
    }

    printf ("\n\nBytes read: %d\n", i);
    printf ("----------------------------------------------------------\n");
    srbObjClose (conn, fd);
    clFinish(conn);
    exit(0);
}

