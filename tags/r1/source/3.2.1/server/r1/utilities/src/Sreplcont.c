/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/* Sreplcont.c - Replicate a container 
 */

#include "scommands.h"

 

extern char srbAuth[];
extern char srbHost[];
extern char srbUser[];
extern char defaultResource[];
extern char mdasCollectionName[];
extern char inCondition[];

void
usage(char *prog)
{
    fprintf(stderr,"Usage: %s [-l] [-z mcatName] -S resource container\n", prog);
}

int
main(int argc, char **argv)
{
    srbConn *conn;
    int contSize;
    int i;
    int SFlag = 0;	/* resource flag */
    char resourcename[MAX_TOKEN];
    int c;
    char *userName, *domainName;
    char *mcatName;
    char myMcatName[MAX_TOKEN];
    char contName[MAX_TOKEN], contCollection[MAX_TOKEN], 
     containerName[MAX_TOKEN];
    int flagval = 0;
    int status;
    
    strcpy(inCondition , "");
    while ((c=getopt(argc, argv, "S:z:l")) != EOF) {
        switch (c) {
	    case 'S':
		SFlag = 1;
		strcpy (resourcename, optarg);
		break;
            case 'l':
                flagval |= l_FLAG;
                break;
            case 'z':
                strcpy (myMcatName, optarg);
                flagval |= z_FLAG;
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }

    if (SFlag == 0) { 
	usage (argv[0]);
	exit (1);
    }

    if (optind + 1 != argc) {
	usage (argv[0]);
	exit (1);
    }

    i = initSrbClientEnv();
    if (i < 0){
	printf("Ssyncont: Error:%i\n",i); 
	exit(-1);
    }

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sreplcont: Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(3);
    }

    if (conn->clientUser == NULL) {
        userName = conn->proxyUser->userName;
        domainName = conn->proxyUser->domainName;
    } else {
        userName = conn->clientUser->userName;
        domainName = conn->clientUser->domainName;
    }
#ifdef FED_MCAT
    if ((flagval & z_FLAG) == 0) {
        if ((flagval & l_FLAG) == 0) {
            char *thisMcat = NULL;

            thisMcat = getmcatNamefromHint (mdasCollectionName);
            if (thisMcat == NULL) {
                status =srbGetMcatZone (conn, userName, domainName,
                 myMcatName);
                if (status >= 0)
                    mcatName = myMcatName;
                else
                    mcatName = NULL;
            } else if (strcmp (thisMcat, "home") == 0 ||
             strcmp (thisMcat, "container") == 0) {
                mcatName = NULL;
            } else {
                mcatName = thisMcat;
            }
        } else {
            mcatName = NULL;
        }
    } else {
        mcatName = myMcatName;
    }
#else
    mcatName = NULL;
#endif
    parseContainerName (argv[optind], contName, contCollection,
     userName, domainName, mcatName);
    sprintf (containerName, "%s/%s", contCollection, contName);


    i = srbReplContainer (conn, MDAS_CATALOG, containerName, resourcename);
    if (i < 0) {
	fprintf(stderr,
	 "Sreplcont: Error replicating container %s, status =  %i\n", 
	  argv[optind], i);
	srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(4);
    }
    clFinish(conn);
    exit(0);
}


