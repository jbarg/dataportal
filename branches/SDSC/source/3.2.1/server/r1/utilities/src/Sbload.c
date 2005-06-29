/* Sbload.c 
 */

#include "scommands.h"
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>

#if defined(PORTNAME_linux)
#include <sys/timex.h>
#endif /* PORTNAME_linux */
#include <sys/stat.h>

/*
#define DEBUG
*/

char OrigContainerName[MAX_TOKEN];
char ResourceName[MAX_TOKEN];
char datatype[MAX_TOKEN];
char newpathname[MAX_TOKEN];


extern char *optarg;
extern int optind, opterr, optopt;

extern char srbAuth[];
extern char srbHost[];
extern char mdasCollectionName[];
extern char inCondition[];
 
extern int VFlag;
extern srb_long_t MaxContSz;	/* max container sz in MB) */
extern char ContainerName[MAX_TOKEN];

extern int
bloadMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
srbPathName *targName, int flagval);

void usage (char *prog);

int
main(int argc, char **argv)
{
    int  newargc;
    char newargv[MAX_ARG_EXP_CNT][MAX_TOKEN];
    char *origObj;
    char *destObj;
    int c, ii;
    char *ch;
    char *myHost;

    int flagval = 0;
    srbPathName *nameArray, targName;
    char contName[MAX_TOKEN], contCollection[MAX_TOKEN], inpCont[MAX_TOKEN];
    int nArgv;
    srbConn *conn, *myConn;
    int status;
    int nFlag = 0;              /* -c Flag */
    char *userName, *domainName;
    int numThreads = 0;
    mdasC_sql_result_struct myresult;
    char *containerMaxSize, *logResName;
    char *mcatName;
    char myMcatName[MAX_TOKEN];


    int cFlag = 0;		/* -c option */
    int tFlag = 0;		/* -t option */


    strcpy(inCondition , "");
    if (argc < 3) {
	usage (argv[0]);
	exit(1);
    }

    ResourceName[0] = '\0';
    datatype[0] = '\0';
    newpathname[0] = '\0';
    
    while ((c=getopt(argc, argv,"keS:s:c:vl")) != EOF) {
	switch (c) {
            case 'c':
                flagval |= c_FLAG;      /* user container */
		strcpy (inpCont, optarg);
                break;
            case 'e':
                flagval |= e_FLAG;      /* exit on error */
                break;
            case 'v':                   /* verbose flag */
                VFlag = 1;
                break;
            case 'k':                   /* chksum flag */
		flagval |= k_FLAG;
                break;
            case 's':                   /* verbose flag */
                MaxContSz = strtoll (optarg, 0, 0);
                break;
            case 'S':                   /* verbose flag */
		strcpy (ResourceName, optarg);
                break;
            case 'l':
                flagval |= l_FLAG;
                break;
	    default:
		usage (argv[0]);
		exit (1);
	}
    }

    if (argc - optind < 1) {	/* must have at least 2 input */
        usage (argv[0]);
        exit (1);
    }


    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}
    
    /* Process the input */


    nArgv = argc - optind - 1;		/* nArgv is the number of in file */

    if (nArgv <= 0) {
        usage (argv[0]);
        exit (1);
    }

    /* Set up the source and target arrays */

    targName.inpArgv = argv[argc-1];

    nameArray = (srbPathName *) malloc (nArgv * sizeof (srbPathName));
    for (i = optind; i < argc - 1; i++) {
        nameArray[i - optind].inpArgv = argv[i];
    }

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sbload: Connection to srbMaster failed.\n");
        fprintf(stderr,"Sbload: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(3);
    }

    /* Parse the target name */
    if (parseName (&targName, mdasCollectionName) < 0)
        {clFinish(conn); exit(4);}

    if (flagval & c_FLAG) {         /* user container */
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

                thisMcat = getmcatNamefromHint (targName.outArgv);
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
        parseContainerName (inpCont, contName, contCollection,
         userName, domainName, mcatName);
        sprintf (ContainerName, "%s/%s", contCollection, contName);
	strcpy (OrigContainerName, ContainerName);

        status = srbGetContainerInfo (conn, MDAS_CATALOG,
         ContainerName, &myresult, MAX_GET_RESULTS);

        if (status < 0) {
            fprintf (stderr, "srbGetContainerInfo failed for %s\n",
              ContainerName);
	    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    clFinish(conn); exit(5);
        }

        if (MaxContSz > 0) {
            MaxContSz = MaxContSz * 1024 * 1024;
        } else {
            containerMaxSize = (char *) getFromResultStruct(
            (mdasC_sql_result_struct *) &myresult,
                dcs_tname[CONTAINER_MAX_SIZE], dcs_aname[CONTAINER_MAX_SIZE]);
	    MaxContSz = strtoll (containerMaxSize, 0, 0);
        }

        if (ResourceName == NULL || strlen (ResourceName) == 0) {
            logResName =  (char *) getFromResultStruct(
             (mdasC_sql_result_struct *) &myresult,
             dcs_tname[CONTAINER_LOG_RSRC_NAME],
             dcs_aname[CONTAINER_LOG_RSRC_NAME]);

            strncpy (ResourceName, logResName, MAX_TOKEN -1);
	}
    } else {	/* not using container */
	if (ResourceName == NULL || strlen (ResourceName) == 0) {
	    get_user_preference(ResourceName, "DRSRC");
	}
    }

    if ((flagval & c_FLAG) == 0 || 
     ((flagval & c_FLAG) && (getenv ("MCAT_HOST") == NULL))) {
	myHost = getHostByResource (conn, ResourceName);
	if (myHost != NULL) {
	    if (strncmp (srbHost, myHost, strlen (srbHost)) != 0) {
		/* different host */
		myConn = srbConnect (myHost, NULL, srbAuth,
                 NULL, NULL, NULL, NULL);
    		if (clStatus(myConn) == CLI_CONNECTION_OK) {
		    clFinish (conn);
		    conn = myConn;
		    strcpy (srbHost, myHost);
		}
	    }
	}
    }

    /* seed the random number */
    srandom (getpid());
    ii = bloadMain (conn, MDAS_CATALOG, nArgv, nameArray, &targName, 
     flagval);

    clFinish (conn);

    if (ii == 0)
      exit(0);
    else 
      exit(1);
}

void
usage (char *prog)
{
        fprintf(stderr,
          "Usage: %s [-c container] [-S resourceName] [-s maxContSzInMbytes] -lk localFile/localDirectory ... targetCollection\n",prog);
}

