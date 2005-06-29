/* sput.c - Import a local file into srb
 * int
 * srbObjCreate(srbConn* conn, int catType, char *objID,
 * char *dataTypeName, char *resourceName, char *collectionName,
 * char *pathName, srb_long_t dataSize)
 * int
 * srbObjWrite(srbConn *conn, int desc, char *buf, int len)
 * int
 * srbObjClose (srbConn* conn, int desc)
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
    int  bytesRead, bytesWritten, total, in_fd, out_fd;
    char *buf;

    if (argc != 3) {
	fprintf(stderr, "Usage: %s local-file srb-file\n",argv[0]);
        exit(1);
    }

    buf = malloc (BUFSIZE);

    conn = srbConnect (HOST, PORT, SRB_AUTH, USER, DOMAIN, AUTH_SCHEME, 
      SERVER_DN);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish (conn);
	free (buf);
        exit (1);
    }

    printf ("Connected to srbServer successfully\n");

    /* Create a srb file = argv[2] */

    out_fd = srbObjCreate (conn, MDAS_CATALOG, argv[2],
      DATATYPE, RESOURCE ,COLLECTION, NULL, -1);

    if (out_fd < 0) { /* error */
        fprintf(stderr, "can't create srb file \"%s\", status = %d\n", argv[2], 
        out_fd);
        fprintf(stderr,"%s",clErrorMessage(conn));
        clFinish (conn);
	free (buf);
	exit (1);
    }

    /* Open a local file with filename = argv[1]. */

    in_fd = open (argv[1], O_RDONLY, 0);
    if (in_fd < 0) { /* error */
        fprintf(stderr, "can't open file\"%s\"\n", argv[1]);
        clFinish (conn);
	free (buf);
        exit (1);
    }

    /* Read from the local file and write to the just created srb file */
    total = 0;
    while ((bytesRead = read(in_fd, buf, BUFSIZE)) > 0) {
        /* Write to the data object */
        bytesWritten = srbObjWrite(conn, out_fd, buf, bytesRead);
        if (bytesWritten < bytesRead) {
           fprintf(stderr, "Error: Read %d bytes, Wrote %d bytes.\n ",
           bytesRead, bytesWritten);
            clFinish (conn);
	    free (buf);
            exit (1);
        }
	total += bytesWritten;
    }
    srbObjClose (conn, out_fd);
    close (in_fd);
    free (buf);

    printf ("Created %s/%s with size %d\n", COLLECTION, argv[2], total);

    clFinish(conn);
    exit (0);
}
