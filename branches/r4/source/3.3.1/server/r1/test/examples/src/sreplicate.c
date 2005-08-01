/* sreplicate.c - replicate a srb file
int
srbObjReplicate (srbConn* conn, int catType, char *objID, char *collectionName,
char *newResourceName, char *newPathName)
 */

#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h> 
#endif

#include "srbClient.h"

#define HOST       	NULL
#define PORT            NULL
#define SRB_AUTH        NULL	/* For 'PASSWD_AUTH', 'ENCRYPT1' */
#define USER            NULL
#define DOMAIN          NULL
#define AUTH_SCHEME     NULL	/* 'PASSWD_AUTH', 'ENCRYPT1', 'GSI_AUTH' */
#define SERVER_DN       NULL

#define DATATYPE "ascii text"
#define RESOURCE "unix-sdsc"
#define COLLECTION "/srbtest"

#define BUFSIZE 200*4096
int
main(int argc, char **argv)
{
    srbConn *conn;
    srbResult *res;
    int retVal; 

    if (argc != 2) {
	fprintf(stderr, "Usage: %s srb-file\n",argv[0]);
        exit(1);
    }

    conn = srbConnect (HOST, PORT, SRB_AUTH, USER, DOMAIN, AUTH_SCHEME, 
      SERVER_DN);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish (conn);
        exit (1);
    }

    printf ("Connected to srbServer successfully\n");

    /* replicate the object */

    retVal = srbObjReplicate (conn, MDAS_CATALOG, argv[1], COLLECTION, 
      RESOURCE, NULL);
    /* Create a srb file = argv[2] */

    if (retVal < 0) { /* error */
        fprintf(stderr, "can't relicate srb file %s, status = %d\n", 
	  argv[1], retVal);
        fprintf(stderr,"%s",clErrorMessage(conn));
        clFinish (conn);
	exit (1);
    }

    printf ("Replicated %s/%s\n", COLLECTION, argv[1]);

    clFinish(conn);
    exit (0);
}
