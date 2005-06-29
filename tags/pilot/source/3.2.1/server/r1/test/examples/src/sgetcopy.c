/* sgetcopy.c - Export a replica from srb to local fs
 * int
 * srbObjOpen(srbConn* conn, char *objID,
 * int oflag, char *collectionName)
 *
 * One or more conditions can be appended to the objID. Each condition must be
 * preceded by the character '&'. Currently, only one condition is supported
 * with this call. 
 * i.e., COPY=n (where n = replica number beginning
 * with 0). e.g. foo&COPY=1 specifies opening replica number 1 of data 
 * object "foo".
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
#define NAME_LEN 256

int
main(int argc, char **argv)
{
    srbConn *conn;
    srbResult *res;
    int  bytesRead, bytesWritten, total, in_fd, out_fd;
    char *buf, objName[NAME_LEN];
    int repl_num;
    

    if (argc != 4) {
	fprintf(stderr, "Usage: %s srb-file repl_num local-file\n",argv[0]);
        exit(1);
    }

    repl_num = atoi (argv[2]);

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

    /* open the srb file = argv[1] */

    sprintf (objName, "%s&COPY=%-d", argv[1], repl_num);

    in_fd = srbObjOpen (conn, objName, O_RDONLY, COLLECTION);
    if (in_fd < 0)  {   /* error */
        fprintf(stderr, "can't open obj\"%s\"\n",
          objName);
        free (buf);
        exit (1);
    }

    /* create a local file with filename = argv[2]. */

    out_fd = open (argv[3], O_CREAT|O_WRONLY, 0640);
    if (out_fd < 0) { /* error */
        fprintf(stderr, "can't create local file %s\n", argv[3]);
        clFinish (conn);
	free (buf);
        exit (1);
    }

    /* Read from the local file and write to the just created srb file */
    total = 0;
    while ((bytesRead = srbObjRead (conn, in_fd, buf, BUFSIZE)) > 0) {
        /* Write to the data object */
        bytesWritten = write (out_fd, buf, bytesRead);
        if (bytesWritten < bytesRead) {
           fprintf(stderr, "Error: Read %d bytes, Wrote %d bytes.\n ",
           bytesRead, bytesWritten);
            clFinish (conn);
	    free (buf);
            exit (1);
        }
	total += bytesWritten;
    }
    srbObjClose (conn, in_fd);
    close (out_fd);
    free (buf);

    printf ("Exported %s/%s to %s with size %d\n", COLLECTION, objName,
      argv[3], total);

    clFinish(conn);
    exit (0);
}
