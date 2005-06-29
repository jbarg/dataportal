/* slock.c - Test the SrbObjLockRange call.
   based on sget.c.
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
    srb_long_t offset;
    int stat;
    int  bytesRead, bytesWritten, total, in_fd, out_fd;
    char *buf;

    if (argc < 3) {
	fprintf(stderr, "Usage: %s srb-file local-file [offset] [read]\n",
		argv[0]);
        exit(1);
    }

    offset = 0;
    if (argc >= 4) {
      int i;
      i = atoi(argv[3]);
      offset = i;
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

    /* open the srb file = argv[1] */

    in_fd = srbObjOpen (conn, argv[1], O_RDWR, "");
    if (in_fd < 0)  {   /* error */
        fprintf(stderr, "can't open obj\"%s\"\n",
          argv[1]);
        free (buf);
        exit (1);
    }

    stat=0;
    if (argc < 5) {
      printf("Calling srbObjLockRange with mode=2 (set Write lock)\n");
      stat = srbObjLockRange(conn, in_fd, offset, 100, 2);
      printf ("Returned from srbObjLockRange, stat=%d\n",stat);
    }
    else {
      printf("Calling srbObjLockRange with mode=1 (set Read lock)\n");
      stat = srbObjLockRange(conn, in_fd, offset, 100, 2);
      printf ("Returned from srbObjLockRange, stat=%d\n",stat);
    }

    //    stat = srbObjSeek(conn, in_fd, offset, SEEK_SET);
    //    printf ("stat=%d\n",stat);

    printf("sleeping 30 secs\n");
    sleep(30);
    printf("done with sleep\n");

    /* create a local file with filename = argv[2]. */

    out_fd = open (argv[2], O_CREAT|O_WRONLY, 0640);
    if (out_fd < 0) { /* error */
        fprintf(stderr, "can't create local file %s\n", argv[2]);
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

    printf ("Exported %s/%s to %s with size %d\n", COLLECTION, argv[1],
      argv[2], total);

    clFinish(conn);
    exit (0);
}
