/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testunlink.c--
 *    test srbUnregisterDataset() call
 *
 *-------------------------------------------------------------------------
 */
#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>

#include "srbClient.h"

#define srbAuth		NULL
#define HOST_ADDR	NULL


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
    int status;

 
    if (argc < 3) {
        fprintf(stderr, "Usage: %s collection objID1 [objID2 ...]\n",
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
	

    for (i = 2; i < argc; i++) {
    	printf ("Unregistering obj : %s\n", argv[i]);
    	status = srbUnregisterDataset (conn, argv[i], argv[1]);
    	if (status < 0)  {   /* error */
            fprintf(stderr, 
	     "can't unregister obj %s in coll %s, status = %d\n", 
	      argv[i], argv[1], status);
	    fprintf(stderr,"%s",clErrorMessage(conn));
	    exit_nicely(conn);
	}
    }

    clFinish(conn);
    exit(0);
}
