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

void usage (char *prog);

int
main(int argc, char **argv)
{
    srbConn *conn;
    char collname[MAX_TOKEN];
    char temp[MAX_TOKEN], temp1[MAX_TOKEN], temp2[MAX_TOKEN];
    int status;
    int i;
    int c;
    int vFlag = 0;
    char svrVersion[MAX_TOKEN];
    char dummy1[MAX_TOKEN], dummy2[MAX_TOKEN],
         cliAPIVersion[MAX_TOKEN], svrAPIVersion[MAX_TOKEN];
   
    while ((c=getopt(argc, argv,"v")) != EOF) {
        switch (c) {
            case 'v':
		vFlag = 1;
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }

    i = firstInitSrbClientEnv();
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

    clFinish(conn);
    exit(0);
}

void usage (char *prog)
{
    fprintf(stderr,"Usage  :%s [-v]\n ", prog);
}

