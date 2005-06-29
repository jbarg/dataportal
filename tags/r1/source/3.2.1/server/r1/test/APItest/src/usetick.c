/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * usetick.c--
 *    test using a ticket to read a dataset. 
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
#define DATATYPE        "ascii text"
#define RESOURCE        "unix-sdsc"
#define COLLECTION      "/srbtest"
#define COLLFLAG        "D"
#define BEGINTIME 	NULL
#define ENDTIME		NULL
#define ACCESSCNT	1
#define ROWS_WANTED     2


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
    int i, nbytes, tmp, in_fd;
    char buf[BUFSIZE];
    struct stat *statbuf;
    int status;
    char *objID;

 
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: %s ticket [objID]\n",
                argv[0]);
        exit(1);
    }

    if (argc == 2)
	objID = NULL;
    else
	objID = argv[2];

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
	
    printf ("--------------------------------------------------------------\n");
    if (objID != NULL)
    	printf ("Openning obj: %s\n", objID);
    else
	printf ("Openning unknown object with ticket\n");

    in_fd = srbObjOpenWithTicket (conn, objID, O_RDONLY, COLLECTION, argv[1]);
    if (in_fd < 0)  {   /* error */
	if (objID != NULL)
            fprintf(stderr, "can't open obj %s, status = %d\n", objID, in_fd);
	else 
	    fprintf(stderr, "can't open obj , status = %d\n", in_fd);
	exit_nicely(conn);
    }

    i = 0;
    if (objID != NULL)
    	printf ("Reading obj : %s\n", objID);
    else
	printf ("Reading obj \n");
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
    clFinish(conn);

    exit(0);
}
