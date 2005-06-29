/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "scommands.h"

 

extern char srbAuth[];
extern char srbHost[];
extern char srbUser[];
extern char mdasCollectionName[];
extern char inCondition[];

void
usage()
{
fprintf(stderr,"Usage: Spcommand [-h] [-H hostAddr | -d srbObj] command\n");
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
  int Hflag = 0;
  int Dflag = 0;
  char buf[MAX_TOKEN];
  char *hostAddr = NULL;
  srbPathName filename;
    
    if (argc < 2) {
	usage();
        exit(1);
    }

    while ((c=getopt(argc, argv,"hd:H:")) != EOF) {
        switch (c) {
            case 'h':
                usage ();
                exit (0);
            case 'H':
                hostAddr = strdup (optarg);
                Hflag = 1;
                break;
            case 'd':
                filename.inpArgv = optarg;
                Dflag = 1;
                break;
            default:
                usage ();
                exit (1);
        }
    }

    if (Hflag && Dflag) {
        fprintf(stderr,"Can not use -H and -d options at the same time\n");
        usage();
        exit(1);
    }

    if (argc != optind + 1) {
        usage();
        exit(1);
    }

    strcpy(inCondition , "");
    i = initSrbClientEnv();
    if (i < 0)
      {printf("Spcommand: Error:%i\n",i); exit(1);}


    if (Dflag) {
        if ((status = parseName (&filename, mdasCollectionName) < 0)) {
            exit(3);
        }
    }
    else {
        filename.outArgv = NULL;
    }

    conn = srbConnect (srbHost, NULL, srbAuth,
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Spcommand: Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    /* parse the command for the arguments */
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

    
    
    portal = srbExecCommand (conn, commPtr, argvPtr, hostAddr, filename.outArgv,
     PORTAL_STD_IN_OUT);
    if (portal < 0)
      {
	fprintf(stderr,"Spcommand: Error with srbExecCommand  %i\n", portal);
	srb_perror (2, portal, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(4);
      }

    while ((status = read (portal, buf, sizeof (buf))) > 0) {
	fwrite (buf, status, 1, stdout);
    }

    clFinish(conn);
    exit(0);
}


