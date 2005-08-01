/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testput.c--
 *    Test srbObjPut () call.
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
#define RESOURCE        "hpss-sdsc"
#define COLLECTION      "/srbtest"
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
    int i;
    srb_long_t nbytes;
    mdasC_sql_result_struct *collResult;
    char *fromObj, *toObj, *toResc;
    char *tmpCollection;
    dirent_t buf;
    int status;
    char *resource;

 
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s localPath objID [resource]\n",
                argv[0]);
        exit(1);
    }

    if (argc == 3)
	resource = RESOURCE;
    else 
	resource = argv[3];

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

    nbytes = srbObjPut (conn, argv[2], COLLECTION, resource, 
     "", "", argv[1], -1, 0);

    printf ("bytes written = %lld\n", nbytes);
}

