/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbcat.c-- Cat a Logical Object. 
 *
 *-------------------------------------------------------------------------
 */
#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif
 
#include "srbClient.h"

/*
#define BUFSIZE		100
*/
#define BUFSIZE		99
#define HOST_ADDR       NULL
#define srbAuth		NULL
#define COLLECTION      "/srbtest"




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
    int  nbytes,  in_fd, out_fd;
    char buf[BUFSIZE];

    if (argc != 2) 
      {fprintf(stderr, "Usage: %s objID \n",argv[0]);
        exit(1);
      }
    conn = clConnect (HOST_ADDR, NULL, srbAuth);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to exfMaster failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely(conn);
    }
	
    in_fd = srbObjOpen (conn, argv[1], O_RDWR, COLLECTION);
    if (in_fd < 0)  {   /* error */
      fprintf(stderr, "can't open obj\"%s\"\n", argv[1]);
      exit_nicely(conn);
    }

    while ((nbytes = srbObjRead (conn, in_fd, buf, BUFSIZE)) > 0) {
      printf ("%-*.*s", nbytes,nbytes,buf);
    }

    srbObjClose (conn, in_fd);
    
    clFinish(conn);
    exit(0);
}
