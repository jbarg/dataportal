/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testLobj - test the physical DB Large Object calls.
 *
 *-------------------------------------------------------------------------
 */
#include "srbClient.h"

#define BUFSIZE 1024
#define srbAuth         NULL
#define INFILE		"../../testfiles/test_in"
#define OUTFILE		"../../testfiles/test_out"
#define HOST_ADDR	NULL
#define LOBJ_NAME	"bar999"

char *resourceLoc[] = {
	"mda-18:vaults:db2v2",
	"sage:srb:foo",
	"",
	"",
	"",
	"gabor.sdsc.edu:mcat:foo"
};
/*
	"mda-18:lgndb2sd:db2v2",
*/
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
    int  nbytes,  in_fd, out_fd, funcVal;
    char buf[BUFSIZE];
    int storSysType;
    int tmp;

    if (argc != 2) 
      {fprintf(stderr, "Usage: %s DbType\n",argv[0]);
        exit(1);
    }
    storSysType = atoi (argv[1]);

    conn = clConnect (NULL, NULL, srbAuth);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to SRB failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely(conn);
    }
	
    srbDbLobjUnlink (conn, storSysType, resourceLoc[storSysType],
      LOBJ_NAME);

    out_fd = srbDbLobjCreate (conn, storSysType, resourceLoc[storSysType],
      LOBJ_NAME, 0666);

    printf("out_fd=%d\n", out_fd);

    if (out_fd < 0)  {  
        fprintf(stderr, "can't create obj %s, %s\n", 
	    resourceLoc[storSysType], LOBJ_NAME);
	fprintf(stderr,"%s",clErrorMessage(conn));
      	exit_nicely(conn);
    } 

    /* Open INFILE */

   in_fd = open (INFILE, O_RDONLY, 0666);
   if (in_fd < 0)  {   /* error */
        fprintf(stderr, "can't open file\"%s\"\n",
        INFILE);
        exit_nicely(conn);
    }

    while ((nbytes = read(in_fd, buf, BUFSIZE)) > 0) {
        tmp = srbDbLobjWrite(conn, out_fd, buf, nbytes);
	if (tmp < 0) {
	    fprintf(stderr, "srbDbLobjWrite error: %s", 
	      clErrorMessage(conn));
	    exit_nicely(conn);
	}
	printf ("Bytes Read:%d. Bytes written:%d\n", nbytes, tmp);
    }

    srbDbLobjClose (conn, out_fd);
    close (in_fd);
    
    in_fd = srbDbLobjOpen (conn, storSysType, resourceLoc[storSysType],
      LOBJ_NAME, 0, 0666);

    printf("in_fd=%d\n", in_fd);

    if (in_fd < 0)  {
        fprintf(stderr, "can't open obj %s, %s\n",
            resourceLoc[storSysType], LOBJ_NAME);
        fprintf(stderr,"%s",clErrorMessage(conn));
        exit_nicely(conn);
    }

    if (unlink(OUTFILE) != 0) {
        fprintf(stderr,"Unable to unlink %s\n", OUTFILE);
        fprintf(stderr,"%s",clErrorMessage(conn));
    }

    out_fd = creat (OUTFILE, 0666);
    if (out_fd < 0)  {   /* error */
        fprintf(stderr, "can't create ufile\"%s\"\n",
        OUTFILE);
        exit_nicely(conn);
    }

    while ((nbytes = srbDbLobjRead (conn, in_fd, buf, BUFSIZE)) > 0) {
        tmp = write (out_fd, buf, nbytes);
        printf ("Bytes Read:%d. Bytes written:%d\n", nbytes, tmp);
    }

    srbDbLobjClose (conn, in_fd);
    srbDbLobjUnlink (conn, storSysType, resourceLoc[storSysType],
      LOBJ_NAME);

    clFinish(conn);
    exit(0);
}
