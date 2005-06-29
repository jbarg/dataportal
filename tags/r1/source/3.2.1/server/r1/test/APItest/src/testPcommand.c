/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*-------------------------------------------------------------------------
 *
 * testPcommand.c-- Test the srbExecCommand() call.
 *
 *-------------------------------------------------------------------------
 */

#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#ifdef PORTNAME_solaris
#include <fcntl.h>
#endif

#include "srbClient.h"


void
usage()
{
fprintf(stderr,"Usage: Spcommand [-h] [-H hostAddr] command\n");
}

void exit_nicely(srbConn* conn)
{
  clFinish(conn);
  exit(0);
}

int
main(int argc, char **argv)
{
  srbConn *conn;
  char temp[MAX_TOKEN];
  char *commPtr, *argvPtr, *tmpPtr;
  int len = 0;
  int c, i;
  int portal, status;
  char buf[MAX_TOKEN];
  char *hostAddr = NULL;
    
    if (argc < 2) {
	usage();
        exit(1);
    }

    conn = srbConnect (NULL, NULL, NULL, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Spcommand: Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        exit_nicely(conn);
    }

        while ((c=getopt(argc, argv,"hH:")) != EOF) {
        switch (c) {
            case 'h':
                usage ();
                exit (0);
            case 'H':
                hostAddr = strdup (optarg);
                break;
            default:
                usage ();
                exit (1);
        }
    }

    /* parse the command for the arguments */

    if (argc != optind + 1) {
	usage(); 
	exit(1);
    }
    commPtr = strdup (argv[optind]);
    tmpPtr = commPtr;
    argvPtr = NULL;

    while ((c = *tmpPtr) != '\0') {
        if (c == ' ') {
            if (len > 0) {      /* end of command */
                *tmpPtr = '\0'; /* mark the end */
                argvPtr = tmpPtr + 1;
		break;
            } else {    /* skip over blanks */
                tmpPtr++;
            }
        } else {
            len ++;
            tmpPtr++;
        }
    }

    
    
    portal = srbExecCommand (conn, commPtr, argvPtr, hostAddr, 
     PORTAL_ON);
    if (portal < 0)
      {
	fprintf(stderr,"Error with srbExecCommand  %i\n", portal);
	srb_perror (2, portal, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	exit_nicely(conn);
      }

#ifdef foo
    while ((status = read (portal, buf, sizeof (buf))) > 0) {
        if (status < sizeof (buf))
            buf[status] = '\0';
        printf ("%s", buf);
    }
#endif
    while ((status = read (portal, buf, sizeof (buf))) > 0) {
	fwrite (buf, status, 1, stdout);
    }

    clFinish(conn);
    exit(0);
}


