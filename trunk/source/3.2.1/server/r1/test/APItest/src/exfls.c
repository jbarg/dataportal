/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * exfls.c--
 *    List an "physical level" file directory.
 *
 *-------------------------------------------------------------------------
 */
#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h> 	/* For C-90 - define O_RDONLY */
#endif

#include "srbClient.h"

#define BUFSIZE		65536
#define srbAuth		NULL
#define UNIX_AM		0	/* UNIX access method */
#define UTREE_AM	1
#define HPSS_AM		2
#define HOST_ADDR	NULL

#define UNIX_HOST       "ghidorah.sdsc.edu"

#define HPSS_HOST       "mda-18.sdsc.edu"

char *rescHost[] = { UNIX_HOST,
                     "",
                     HPSS_HOST
};


extern struct vaultQueElement *srbVaultInfo();

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
    int i, nbytes, tmp;
    char buf[BUFSIZE];
    struct srbStat *statbuf;
    int acc_method, num_of_loop, loop_cnt;
    int dirDesc;
    struct srbDirent *srbDirent;
    char tmpPath[512];

 
    if (argc != 3) {
        fprintf(stderr, 
	    "Usage: %s storTpye(0, 1 or 2)  directory \n",
                argv[0]);
        exit(1);
    }
    acc_method = atoi (argv[1]);
    if (acc_method < UNIX_AM || acc_method > HPSS_AM) {
	fprintf(stderr, "access_method must be 0, 1 or 2\n");
	exit (1);
    }

    /*
     * set up the connection
     */

    conn = clConnect (HOST_ADDR, NULL, srbAuth);

    /* check to see that the backend connection was successfully made */
    if (clStatus(conn) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to exfMaster failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely(conn);
    }
	
    dirDesc = srbOpendir (conn, acc_method, rescHost[acc_method], argv[2]);

    if (dirDesc < 0) {
        fprintf(stderr,"Unable to srbOpendir . \n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        exit (1);
    }
 
    statbuf = (struct srbStat *) malloc (sizeof (struct srbStat));

    while ((srbDirent = srbReaddir (conn, dirDesc)) != NULL) {
	sprintf (tmpPath, "%s/%s", argv[2], srbDirent->d_name);
        i = srbFileStat (conn, acc_method, rescHost[acc_method], 
	 tmpPath, statbuf);
        if (i < 0)  {   /* error */
            fprintf(stderr, "can't stat file\"%s\"\n", srbDirent->d_name);
        } else {
            printf ("mode=%6o %3lld uid=%5d gid=%5d %9lld   %s\n", 
		statbuf->st_mode, statbuf->st_nlink, statbuf->st_uid,
		statbuf->st_gid, statbuf->st_size,
		srbDirent->d_name);
	}
    }
 
    srbClosedir (conn, dirDesc);

    clFinish(conn);
    exit(0);
}
