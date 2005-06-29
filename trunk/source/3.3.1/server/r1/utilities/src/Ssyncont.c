/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* Ssyncont.c - Create a container 
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
    fprintf(stderr,"Usage: %s [-adhpl] [-z mcatName] container\n", prog);
}

int
syncContMain (srbConn *conn, int catType, int nArgv,
srbPathName nameArray[],int syncFlag);

int
main(int argc, char **argv)
{
    srbConn *conn;
    int i, status;
    int dFlag = 0;	/* delete after sync */
    int pFlag = 0;	/* sync to primary only */
    int flagval = 0;
    int c;
    srbPathName nameArray[MAX_TOKEN];
    int nArgv;
    char *userName, *domainName;
    char *mcatName;
    char myMcatName[MAX_TOKEN];
    
    strcpy(inCondition , "");    

    while ((c=getopt(argc, argv, "adhpz:l")) != EOF) {
        switch (c) {
	    case 'a':
		flagval |= a_FLAG;
		break;
	    case 'd':
		dFlag = PURGE_FLAG;
		break;
            case 'h':
                usage (argv[0]);
                exit (0);
	    case 'p':
		pFlag = PRIMARY_FLAG;
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

    nArgv = 0;
    for (i = optind; i < argc; i++) {
        nameArray[nArgv].inpArgv = argv[i];
	/* Sync the container chain */
	if (flagval & a_FLAG) {
	    nArgv ++;
	    nameArray[nArgv].inpArgv = malloc (MAX_TOKEN);
	    sprintf (nameArray[nArgv].inpArgv, "%s.*", 
	      nameArray[nArgv - 1].inpArgv);
	}
	nArgv ++;
    }

    if (nArgv < 1) {   /* must have at least 1 source input */
        usage( argv[0]);
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
        fprintf(stderr,"Ssyncont: Connection to srbMaster failed.\n");
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
    if (convContNameArrayToSQL (nameArray, nArgv, userName, domainName,
     mcatName) < 0)
        {clFinish(conn); exit(4);}

    status = syncContMain (conn, MDAS_CATALOG, nArgv, nameArray, 
      dFlag|pFlag);
    
    clFinish(conn);
    exit(0);
}

int 
syncContMain (srbConn *conn, int catType, int nArgv, 
srbPathName nameArray[],int syncFlag)
{
    int i, row;
    int status;
    mdasC_sql_result_struct dataResult[MAX_TOKEN];
    int dataCnt = 0;

    for (i = 0; i < nArgv; i++) {
        /* set flagval to 0 first for non-recursive check */
	status = queryData (conn, catType, nameArray[i].outArgv, 0,
	 MAX_GET_RESULTS, &dataResult[i]);
        if (status < 0) {
	    continue;
        }

        dataCnt += dataResult[i].row_count;
    }

    if (dataCnt == 0) {
        fprintf (stderr, "Input container %s not found\n",
          nameArray[0].inpArgv);
        return (CLI_ERR_COLLNAME);
    }

    status = 0;
    for (i = 0; i < nArgv; i++) {
        char *contColl, *contName, *prevContColl, *prevContName;
	char fullContName[MAX_TOKEN];

        contColl = (char *) getFromResultStruct(&dataResult[i],
          dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
        contName = (char *) getFromResultStruct(&dataResult[i],
          dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

 	prevContColl = prevContName = NULL;

	if (status >= 0) {
          for (row = 0; row < dataResult[i].row_count; row++) {
	    /* Could get more that 1 result with the same container */
	    if ((prevContColl == NULL || strcmp (contColl, prevContColl) != 0 ||
	     strcmp (contName, prevContName) != 0) &&
	     isContFamily (contName, nameArray[i].inpArgv)) {
	        sprintf (fullContName, "%s/%s", contColl, contName);
	        status = srbSyncContainer (conn, catType, fullContName, 
		  syncFlag);
	        if (status < 0) {
	            fprintf(stderr,
		     "Ssyncont: Error syncing container '%s', status =  %i\n",
	               fullContName, status);
	            srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
		    break;
	        }
	    }
	    prevContColl = contColl;
	    prevContName = contName;
            contColl += MAX_DATA_SIZE;
            contName += MAX_DATA_SIZE;
	  }
        }
        while (dataResult[i].continuation_index >= 0) {
	    int continuation_index;

	    continuation_index = dataResult[i].continuation_index;
	    clearSqlResult (&dataResult[i]);
            status = srbGetMoreRows(conn, catType,
              continuation_index, &dataResult[i], MAX_GET_RESULTS);
            if (dataResult[i].result_count == 0  || status != 0)
                break;

            contColl = (char *) getFromResultStruct(&dataResult[i],
              dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
            contName = (char *) getFromResultStruct(&dataResult[i],
              dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

            prevContColl = prevContName = NULL;

            for (row = 0; row < dataResult[i].row_count; row++) {
                /* Could get more that 1 result with the same container */
                if ((prevContColl == NULL || 
		 strcmp (contColl, prevContColl) != 0 ||
                 strcmp (contName, prevContName) != 0) &&
                 isContFamily (contName, nameArray[i].inpArgv)) {
                    sprintf (fullContName, "%s/%s", contColl, contName);
                    status = srbSyncContainer (conn, catType, fullContName,
                      syncFlag);
                    if (status < 0) {
                        fprintf(stderr,
                         "Ssyncont: Error syncing container '%s', status =  %i\n",
                           fullContName, status);
                          srb_perror (2, status, "", 
			  SRB_RCMD_ACTION|SRB_LONG_MSG);
                          break;
                    }
                    prevContColl = contColl;
                    prevContName = contName;
                    contColl += MAX_DATA_SIZE;
                    contName += MAX_DATA_SIZE;
		}
            }
        }
    }
    return (status);
}


