/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
#include "scommands.h"
#include <string.h>

extern char mdasCollectionName[];
extern char mdasCollectionHome[];
extern char srbAuth[];
extern char srbHost[];
extern char srbPort[];
extern char mcatZone[];
extern char EnvFilePath[];

extern int SrbShellPort;

#define DefSrbShell "sh"
#define SSH_COMMAND_TIMEOUT 1800
 
int
readAndProcCommand (char *myShell);
int
procSshCommand (char *inbuf, char *myShell);

void alarmSigHandler ();

srbConn *MyConn = NULL;

void usage (char *prog);

int
main(int argc, char **argv)
{
    srbConn *conn;
    char collname[MAX_TOKEN];
    char srbPortEnv[MAX_TOKEN], mdasEnvFile[MAX_TOKEN];
    int status;
    int i;
    int c;
    int flagval = 0;
    int vFlag = 0;
    char svrVersion[MAX_TOKEN];
    char dummy1[MAX_TOKEN], dummy2[MAX_TOKEN],
         cliAPIVersion[MAX_TOKEN], svrAPIVersion[MAX_TOKEN];
    char myCommand[MAX_DATA_SIZE];
    char myShell[MAX_TOKEN];
    char *tmpShell;
   
    while ((c=getopt(argc, argv,"cv")) != EOF) {
        switch (c) {
            case 'v':
		vFlag = 1;
                break;
            case 'c':
		flagval |= c_FLAG;
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }

    /* set the session using the current pid */

    i = firstInitSrbClientEnv();
#ifndef _WIN32
    if (getenv("mdasEnvFile") == NULL) {
        sprintf (mdasEnvFile, "mdasEnvFile=%s/.srb/.MdasEnv.%d", 
	  getenv("HOME"), getpid ());
        putenv (mdasEnvFile);
    }
#endif
    if (i < 0)
      {printf("Error Opening Client Environment File:%i\n",i); exit(1);}

    if (vFlag > 0) {
	dummy1[0] = '\0';
	cliAPIVersion[0] = '\0';

        if (getenv("srbPort") != NULL) {
            printf ("Using Port %s.\n", getenv("srbPort"));
        } else if (srbPort != NULL && strlen (srbPort) > 0) {
            printf ("Using Port %s.\n", srbPort);
        } else {
            printf ("Using default Port %s.\n", DefaultPort);
        }

	splitString (SRB_VERSION, dummy1, cliAPIVersion, '&');
	printf ("Client Release = %s, API version = %s.\n", 
	 dummy1, cliAPIVersion);
    }
    else {
	dummy1[0] = '\0';
	cliAPIVersion[0] = '\0';
	splitString (SRB_VERSION, dummy1, cliAPIVersion, '&');
    }

    i = rewriteSrbClientEnvFile();
    if (i < 0)
      {printf("Error Writing Client Environment File:%i\n",i); exit(1);}
    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error Opening Client Environment File:%i\n",i); exit(1);}
    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn);
	exit(3);
    }
    MyConn = conn;

#ifdef FED_MCAT
    if (mcatZone == NULL || strlen (mcatZone) == 0) {
	status =srbGetMcatZone (conn, conn->proxyUser->userName, 
	 conn->proxyUser->domainName, mcatZone);
	if (status < 0) {
	    fprintf (stderr, "srbGetMcatZone error for %s.%s\n",
	     conn->proxyUser->userName, conn->proxyUser->domainName);
            srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    clFinish (conn);
	    exit (1);
	}
        i = rewriteSrbClientEnvFile();
        if (i < 0) {
	    printf("Error Writing Client Environment File:%i\n",i); 
	    clFinish (conn);
	    exit(1);
	}
    }
#endif

#ifdef FED_MCAT
    if (mcatZone == NULL || strlen (mcatZone) == 0) {
	status =srbGetMcatZone (conn, conn->proxyUser->userName, 
	 conn->proxyUser->domainName, mcatZone);
	if (status < 0) {
	    fprintf (stderr, "srbGetMcatZone error for %s.%s\n",
	     conn->proxyUser->userName, conn->proxyUser->domainName);
	    clFinish (conn);
	    exit (1);
	}
        i = rewriteSrbClientEnvFile();
        if (i < 0) {
	    printf("Error Writing Client Environment File:%i\n",i); 
	    clFinish (conn);
	    exit(1);
	}
    }
#endif

    /* get the server version */

    if (vFlag > 0) {
	i = srbGetSvrVersion (conn, &svrVersion);
	if (i == 0) {
            dummy2[0] = '\0';
            svrAPIVersion[0] = '\0';
            splitString (svrVersion, dummy2, svrAPIVersion, '&');
            printf ("Server Release = %s, API version = %s.\n", 
             dummy2, svrAPIVersion);

	    if (strncmp(svrAPIVersion, cliAPIVersion, 1) != 0) { /* 'A' != 'D' */
	      fprintf(stderr,"ERROR. SRB client and server APIs versions differ.\n")	;
	      clFinish(conn);
	      exit(1);
	    }
	} else {
	    fprintf(stderr,"Unable to srbGetSvrVersion.\n");
	    srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    clFinish(conn);
	    exit(2);
	}
    }
    else {
      i = srbGetSvrVersion (conn, &svrVersion);
      if (i == 0) {
	dummy2[0] = '\0';
	svrAPIVersion[0] = '\0';
	splitString (svrVersion, dummy2, svrAPIVersion, '&');
	if (strncmp(svrAPIVersion, cliAPIVersion, 1) != 0) { /* 'A' != 'D' */
	  fprintf(stderr,"ERROR. SRB client and server APIs versions differ. Server Release = %s, API version = %s, Client Release = %s, API version = %s.\n",
		  dummy2, svrAPIVersion, dummy1, cliAPIVersion);
	  clFinish(conn);
	  exit(4);
	}
      } else {
	fprintf(stderr,"Unable to srbGetSvrVersion.\n");
	srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn);
	exit(5);
      }
    }
    
#ifdef FED_MCAT
    if (vFlag > 0) {
	char serverMcatZone[MAX_TOKEN];

	printf ("Client mcatZone = %s\n", mcatZone);
	/* get the server's MCAT zone */
	status =srbGetMcatZone (conn, NULL, NULL, serverMcatZone);
        if (status < 0) {
            fprintf (stderr, "\nsrbGetMcatZone error for server, stat = %d\n",
	      status);
            clFinish (conn);
            exit (1);
        }
	printf ("Server mcatZone = %s\n", serverMcatZone);
	if (strcmp (serverMcatZone, mcatZone) != 0) {
	    printf (
	     "Client connecting through a foreign zone. Possible problem for cross zone operations.\n");
	}
    }
#endif
    
    if ((tmpShell = getenv("SRB_SHELL")) == NULL) {
	strcpy (myShell, DefSrbShell);
    } else {
	if (strstr (tmpShell, "csh") != NULL) {
	    fprintf (stderr, 
	     "WARNING: SRB_SHELL=%s. Scommand may fail using this shell\n",
	     tmpShell);
	}
        strcpy (myShell, tmpShell);
    }

    sprintf (srbPortEnv, "SRB_SHELL_PORT=%d", conn->sock);
    putenv (srbPortEnv);

    if (flagval & c_FLAG) {
	int numArg;

	numArg = argc - optind;
	memset (myCommand, 0, MAX_TOKEN);
	if (numArg > 0) {
	    for (i = optind; i < argc; i++) {
		strcat (myCommand, argv[i]);
		strcat (myCommand, " ");
	    }
	}
	status = procSshCommand (myCommand, myShell);
    } else {	/* interactive */
        status = readAndProcCommand (myShell);
    }
    SrbShellPort = -1;	/* tell clFinish that it is a real disconn */
    clFinish(conn);
    unlink (EnvFilePath);
    if (status < 0) {
	exit (1);
    } else {
        exit(0);
    }
}

int
readAndProcCommand (char *myShell)
{
    char inbuf[MAX_DATA_SIZE];
    int status;

    signal (SIGALRM, alarmSigHandler);

    while (1) {
        memset (inbuf, 0, MAX_DATA_SIZE);
	printf ("Ssh > ");
	alarm (SSH_COMMAND_TIMEOUT);
	fgets(inbuf, MAX_DATA_SIZE, stdin);
	alarm (0);

	/* replace new line with 0 */
	status = strlen (inbuf);
	if (status > 0) {
	    inbuf[status - 1] = '\0';
	}
        if (strlen (inbuf) == 0) {
            continue;
        } else if (strcmp (inbuf, "quit") == 0 || 
	 strcmp (inbuf, "exit") == 0 || strcmp (inbuf, "Sexit") == 0) {	    
	    return (0);
	} 

	status = procSshCommand (inbuf, myShell);
    }
}

int
procSshCommand (char *inbuf, char *myShell)
{
    int pid;
    char myCommand[MAX_TOKEN], myArg[MAX_DATA_SIZE];
    char *inpPtr, *outPtr;
    char *commPtr, *argvPtr, *tmpPtr;
    int c, status;
    char *av[MAX_TOKEN];
    int inx = 1;
    int len = 0;

    if (strncmp (inbuf, "Sinit ", strlen ("Sinit ")) == 0 ||
      strcmp (inbuf, "Sinit") == 0 ||
      strncmp (inbuf, "Sexit ", strlen ("Sexit ")) == 0 ||
      strcmp (inbuf, "Sexit") == 0) {
	return (0);
    }

    /* parse the inbuf */

    /* if (inbuf[0] == 'S') { */
    if (strcmp (myShell, "NO_SRB_SHELL") == 0) { 
	/* no SRB_SHELL */
    
        commPtr = inbuf;
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

        /* parse the argvPtr */


        if (argvPtr != NULL) {
            inpPtr = argvPtr;
            outPtr = inpPtr;
            while ((c = *inpPtr) != '\0') {
                if (c == ' ') {
                    if (len > 0) {      /* end of a argv */
                        *inpPtr = '\0'; /* mark the end */
                        av[inx] = outPtr;
                        /* reset inx and pointer */
                        inpPtr++;
                        outPtr = inpPtr;
                        inx ++;
                        len = 0;
                    } else {    /* skip over blanks */
                        inpPtr++;
                        outPtr = inpPtr;
                    }
                } else {
                    len ++;
                    inpPtr++;
                }
            }
            if (len > 0) {      /* handle the last argv */
                av[inx] = outPtr;
                inx++;
            }
	}
    } else {    /* normal command */
        /* av[1] = "-fc"; */
	av[1] = "-c";
        inx++;
        av[2] = inbuf;
        inx++;
        commPtr = myShell;
    }

    av[0] = commPtr;

    av[inx] = NULL;

    if ((pid = FORK()) == 0) {  /* child */
	/* close (0); */
	execvp (av[0], av);
    }
    wait (&status);
    
    return (status);
}

void
alarmSigHandler ()
{
    fprintf (stderr, "Ssh has timeout (%d sec) waiting for the next command\n",
     SSH_COMMAND_TIMEOUT);
    clFinish(MyConn);
    unlink (EnvFilePath);
    exit (1);
}

void usage (char *prog)
{
    fprintf(stderr,"Usage  :%s [-v]\n ", prog);
}

