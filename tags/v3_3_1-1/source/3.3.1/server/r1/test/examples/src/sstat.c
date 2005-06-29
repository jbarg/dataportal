/* sstat.c - get the stat a srb file
 * int
 * srbObjStat(srbConn* conn, int catType, char *path, struct stat *statbuf)
 * int
 * srbObjUnlink (srbConn* conn, char *objID, char *collectionName)
 */

#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h> 
#endif
#include <unistd.h>

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
#define NAME_LEN 256
int
main(int argc, char **argv)
{
    srbConn *conn;
    srbResult *res;
    int retVal; 
    char myPath[NAME_LEN];
    struct stat statbuf;
    int rFlag = 0;
    int c;

    while ((c=getopt(argc, argv,"r")) != EOF) {
        switch (c) {
            case 'r':
		rFlag = 1;
		break;
            default:
		fprintf(stderr, "Usage: %s [-r] srb-file\n",argv[0]);
                exit (1);
        }
    }

    if (optind != argc - 1) {
	fprintf(stderr, "Usage: %s [-r] srb-file\n",argv[0]);
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

    /* stat the object */

    sprintf (myPath, "%s/%s", COLLECTION, argv[optind]);
    retVal = srbObjStat (conn, MDAS_CATALOG, myPath, &statbuf);

    if (retVal < 0) { /* error */
        fprintf(stderr, "can't stat srb file %s, status = %d\n", 
	  myPath, retVal);
        fprintf(stderr,"%s",clErrorMessage(conn));
        clFinish (conn);
	exit (1);
    }

    printf ("Some value of stat for %s/%s: \n", COLLECTION, argv[optind]);
    if ((statbuf.st_mode & S_IFREG) != 0) {     /* A file */
        printf ("It is a file type with size = %lld\n", statbuf.st_size); 
    } else {
	printf ("It is a directory\n");
    }
	
    if (rFlag == 1)
        retVal = srbObjUnlink (conn, argv[optind], COLLECTION);

    clFinish(conn);
    exit (0);
}
