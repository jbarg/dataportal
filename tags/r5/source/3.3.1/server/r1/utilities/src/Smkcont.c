/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* Smkcont.c - Create a container 
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
    fprintf(stderr,"Usage: %s [-l] [-S resourceName] [-D  dataType] [-s containerSize] [-z mcatName] container\n", prog);
}

int
main(int argc, char **argv)
{
    srbConn *conn;
    char resourceName[MAX_TOKEN], dataType[MAX_TOKEN];
    char myMcatName[MAX_TOKEN];
    char *mcatName;
    char *userName, *domainName;
    srbPathName nameArray[MAX_TOKEN];
    srb_long_t contSize, maxContSize;;
    int i;
    int status;
    int sFlag = 0;
    int SFlag = 0;
    int DFlag = 0;
    int flagval = 0;
    int c;
    
    strcpy(inCondition , "");
    while ((c=getopt(argc, argv, "lS:D:s:z:")) != EOF) {
        switch (c) {
            case 'S':
		strcpy (resourceName, optarg);
		SFlag = 1;
		break;
	    case 's':
		maxContSize = MAX_CONTAINER_SIZE; /* in Mb */
		maxContSize = maxContSize * 1024*1024;
		contSize = strtoll (optarg, 0, 0);
		if (contSize > maxContSize) {
	            fprintf (stderr, 
		     "The input container size exceed max value of %d Mb. Set to max value.\n", MAX_CONTAINER_SIZE);
		contSize = maxContSize;
		}
		sFlag = 1;
		break;
	    case 'D':
		strcpy (dataType, optarg);
		DFlag = 1;
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

    i = initSrbClientEnv();
    if (i < 0){
	printf("Smkcont: Error:%i\n",i); 
	exit(1);
    }

    if (SFlag == 0) {
	if (defaultResource != NULL && defaultResource[0] != '\0') {
	    strcpy (resourceName, defaultResource);
	} else {
	    fprintf (stderr, "A resourceName must be specified using the -S option of in the .MdasEnv file\n");
	    usage (argv[0]);
            exit (1);
        }
    }

    if (DFlag == 0) {
	strcpy (dataType, DefMdasDataTypeName);
    }

    if (sFlag == 0) {
	contSize = DEF_CONTAINER_SIZE;
    }

    if (optind >= argc) {
	usage(argv[0]); 
	exit(1);
    }

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Smkcont: Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    nameArray[0].inpArgv = argv[optind];

    if (conn->clientUser == NULL) {
        userName = conn->proxyUser->userName;
        domainName = conn->proxyUser->domainName;
    } else {
        userName = conn->clientUser->userName;
        domainName = conn->clientUser->domainName;
    }

   /* Convert the source name to SQL like */
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
    if (convContNameArrayToSQL (nameArray, 1, userName, domainName,
     mcatName) < 0) { 
	clFinish(conn); 
	exit(4);
    }

    i = srbContainerCreate (conn, MDAS_CATALOG, nameArray[0].outArgv, dataType, 
     resourceName, contSize);
    if (i < 0) {
	fprintf(stderr,"Smkcont: Error making container '%s', status =  %i\n", 
	 argv[optind], i);
	srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(4);
    }
    clFinish(conn);
    exit(0);
}


