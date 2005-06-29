/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* Sbunload.c - unload a container 
 */

#include "scommands.h"

extern char srbAuth[];
extern char srbHost[];
extern char srbUser[];
extern char defaultResource[];
extern char mdasCollectionName[];
extern char mdasCollectionHome[];
extern char inCondition[];

void
usage(char *prog)
{
    fprintf(stderr,"Usage: %s [-htlk] [-z mcatZone] [-f incontainer_obj]|[-c container] source_collection [local_dir]\n", prog);
}

int
main(int argc, char **argv)
{
    srbConn *conn;
    char *container;
    int i, status;
    int flagval = 0;
    int c;
    srbPathName *nameArray;
    srbPathName collNameArray[1];
    int nArgv;
    char *userName, *domainName;
    char localdir[MAX_TOKEN];
    char *inpFileName;
    char *inpContainer;
    char *srcCollection;
    char *mcatName;
    char myMcatName[MAX_TOKEN];
    
    strcpy(inCondition , "");    

    while ((c=getopt(argc, argv, "f:c:hktz:l")) != EOF) {
        switch (c) {
            case 'h':
                usage (argv[0]);
                exit (0);
	    case 'c':
		flagval |= c_FLAG;
		inpContainer = strdup (optarg);
		break;
	    case 'f':
		flagval |= f_FLAG;
		inpFileName = strdup (optarg);
		break;
	    case 't':
		flagval |= t_FLAG;
		break;
	    case 'k':
		flagval |= k_FLAG;
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

    flagval |= a_FLAG;	/* a_FLAG is always on */
    nArgv = (argc - optind);
    if (nArgv == 2) {
	sprintf (localdir, "%s", argv[optind + 1]);
    } else if (nArgv == 1) {
	sprintf (localdir, "%s", ".");
    } else {
	usage (argv[0]);
        exit (1);
    }

    i = initSrbClientEnv();
    if (i < 0){
	printf("Sbunload: Error:%i\n",i); 
	exit(-1);
    }

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sbunload: Connection to srbMaster failed.\n");
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

    /* first element in the array is the src collection */
    collNameArray[0].inpArgv = argv[optind];

    if (convNameArrayToSQL (collNameArray, 1) < 0)
        {clFinish(conn); exit(3);}

    if (flagval & c_FLAG) {
        /* input is a container */
	nameArray = malloc (MAX_TOKEN * sizeof (srbPathName));
        nameArray[0].inpArgv = inpContainer;
    } else if (flagval & f_FLAG) {
        /* inpot is a SRB incontainer file */
        container = getContainerByObj (conn, inpFileName);
        if (container == NULL) {
            fprintf (stderr, "Input SRB file %s is not in a container\n",
             inpFileName);
            exit (1);
	}
	if (flagval & a_FLAG) {
	    /* This could be one of the child container in the family */
	    setContainerAsRoot (container);
	}
	nameArray = malloc (MAX_TOKEN * sizeof (srbPathName));
	nameArray[0].inpArgv = container;
    } else {
	nArgv = getContainerByColl (conn, collNameArray, &nameArray);
	if (nArgv < 0) {
	    clFinish(conn); 
	    exit(4);
	}
    }
    if (flagval & c_FLAG || flagval & f_FLAG) {
        if (flagval & a_FLAG) {
  	    /* unload the container chain */
            nArgv = 2;
            nameArray[1].inpArgv = malloc (MAX_TOKEN);
            sprintf (nameArray[1].inpArgv, "%s.*",
              nameArray[0].inpArgv);
        } else {
	    nArgv = 1;
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

       /* Convert the source name to SQL like */
        if (nArgv > 0)
            if (convContNameArrayToSQL (nameArray, nArgv, userName, 
	     domainName, mcatName) < 0)
	      {clFinish(conn); exit(4);}
    }

    status = unloadContMain (conn, MDAS_CATALOG, nArgv, nameArray, 
      flagval, localdir, collNameArray);
    
    clFinish(conn);
    exit(0);
}

