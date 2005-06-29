/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/* Srmtrash.c - rm files in /trash */


#include "scommands.h"
 
extern char srbAuth[];
extern char srbHost[];
extern char mdasCollectionName[];
extern int RemovingCont;
extern int RStatus;

void usage (char *prog);
int
trashMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
int flagval, int inpPurgeTime);
int
setTrashPath (srbConn *conn, char *userName, char *domainName,
srbPathName nameArray[]);

int
main(int argc, char **argv)
{
    int c;
    int flagval = 0;
    srbPathName nameArray[MAX_TOKEN];
    char inpUser[MAX_TOKEN];
    int inpPurgeTime = DEF_PURGE_TIME;	/* purge time in hours */
    int nArgv;
    srbConn *conn;
    int status;
    char *userName, *domainName;
    int uFlag = 0;

    while ((c=getopt(argc, argv,"AhuU:t:")) != EOF) {
        switch (c) {
            case 'A':
                flagval |= A_FLAG;	/* admin flag */
                break;
            case 'h':
		usage (argv[0]);
		exit (0);
            case 'u':
                flagval |= unreg_FLAG;
                break;
            case 'U':
		uFlag = 1;
		strcpy(inpUser, optarg);
                break;
            case 't':
		inpPurgeTime = atoi (optarg);
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }

    flagval |= t_FLAG;	/* indicate it is trash removal */
    flagval |= R_FLAG;	/* force the recursive flag */

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Strash: Initialization Error:%i\n",i); exit(1);}

    for (i = optind; i < argc; i++) {
        nameArray[i - optind].inpArgv = argv[i];
    }
    nArgv = argc - optind;          /* nArgv is the number of in file */

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Strash: Connection to srbMaster failed.\n");
        fprintf(stderr,"Strash: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); 
	exit(3);
    }

    if (uFlag == 0) {
	if (nArgv <= 0) {
	    /* user not defined */
	    if (flagval & A_FLAG) {	/* admin user purging for other users */
		userName = domainName = NULL;
	    } else {
    	        if (conn->clientUser == NULL) {
        	    userName = conn->proxyUser->userName;
        	    domainName = conn->proxyUser->domainName;
    	        } else {
        	    userName = conn->clientUser->userName;
        	    domainName = conn->clientUser->domainName;
		}
            }
            nArgv = setTrashPath (conn, userName, domainName, nameArray);
            if (nArgv <= 0) {
                clFinish(conn); 
                exit(3);
            }
	}
    } else {	/* parse the user input */
	char thisUser[MAX_TOKEN], thisDomain[MAX_TOKEN];

	memset (thisUser, 0, MAX_TOKEN);
	memset (thisDomain, 0, MAX_TOKEN);
	clSplitbychar (inpUser, thisUser, thisDomain, '@');
	if (strlen (thisUser) == 0 || strlen (thisDomain) == 0) {
	    fprintf (stderr, 
	     "Invalid -u input %s. Must be in the form userName@domainName\n",
	     inpUser);
            clFinish(conn); exit(3);
	}

        if (conn->clientUser == NULL) {
            userName = conn->proxyUser->userName;
            domainName = conn->proxyUser->domainName;
        } else {
            userName = conn->clientUser->userName;
            domainName = conn->clientUser->domainName;
        }

        if ((strcmp (userName, thisUser) != 0 ||
	 strcmp (domainName, thisDomain) != 0) && (flagval & A_FLAG) == 0) {
            fprintf (stderr,
             "The -a option must be used for purging other users file\n");
            clFinish(conn); exit(3);
        }
       if (nArgv <= 0) {
            nArgv = setTrashPath (conn, thisUser, thisDomain, nameArray);
            if (nArgv <= 0) {
                clFinish(conn);
                exit(3);
            }
	}
    }

    /* Convert the source name to SQL like */
    if (convNameArrayToSQL (nameArray, nArgv) < 0)
      {clFinish(conn); exit(4);}

    RStatus = 0;
    trashMain (conn, MDAS_CATALOG, nArgv, nameArray, flagval, inpPurgeTime);

    if (RemovingCont > 0) {
	RStatus = 0;
	/* there is a container in the tree */
	trashMain (conn, MDAS_CATALOG, nArgv, nameArray, flagval, inpPurgeTime);
    }

    clFinish(conn);
    if (RStatus < 0 && RStatus != COLLECTION_NOT_EMPTY) {
        srb_perror (2, RStatus, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	exit (1);
    } else {
        exit(0);
    }
}

int
trashMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
int flagval, int inpPurgeTime)
{
    int i;
    int status, status1;
    mdasC_sql_result_struct targDataResult, targCollResult;
    mdasC_sql_result_struct *dataResult, *collResult;
    int targType;
    int dataCnt = 0;
    int collCnt = 0;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    char srcPar[MAX_TOKEN], srcCh[MAX_TOKEN];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN];
    char *tmpStr, *tmpStr1;


    dataResult = (mdasC_sql_result_struct *)
      malloc (sizeof (mdasC_sql_result_struct) * nArgv);
    collResult = (mdasC_sql_result_struct *)
      malloc (sizeof (mdasC_sql_result_struct) * nArgv);

    memset (dataResult, 0, sizeof (mdasC_sql_result_struct) * nArgv);
    memset (collResult, 0, sizeof (mdasC_sql_result_struct) * nArgv);

    for (i = 0; i < nArgv; i++) {
        /* set flagval to 0 first for non-recursive check */
        status = expandAndChkName (conn, MDAS_CATALOG, &nameArray[i], 0,
          &dataResult[i], &collResult[i]);
        if (status < 0 || dataResult[i].row_count + collResult[i].row_count
	  == 0) {
	    continue;
        }
	do {
	    if (dataResult[i].row_count > 0) {      /* source is data */
		status = rmData (conn, catType, flagval, &dataResult[i], 
		 -1, inpPurgeTime);
	    }

	    if (collResult[i].row_count > 0) {      /* source is coll */
		if ((flagval & R_FLAG) == 0) {
		    fprintf (stderr, "Must use -r to rm collection %s.\n",
		      nameArray[i].inpArgv);
		} else if (nameArray[i].type == SPEC_COLL_T) {
		    fprintf (stderr, "rm of %s not allowed\n",
		      nameArray[i].inpArgv);
		} else {
		    status1 = rmColl (conn, catType, flagval, 
		     &collResult[i], -1, inpPurgeTime);
		}
	    }
            if (dataResult[i].continuation_index >= 0 &&
              dataResult[i].result_count > 0) {
                clearSqlResult (&dataResult[i]);
                status = srbGetMoreRows(conn, catType,
                 dataResult[i].continuation_index, &dataResult[i], 
		  MAX_GET_RESULTS);
                if (status == 0)
                    filterDeleted (&dataResult[i]);
            } else
                clearSqlResult (&dataResult[i]);

            if (collResult[i].continuation_index >= 0 &&
              collResult[i].result_count > 0) {
                clearSqlResult (&collResult[i]);
                status1 = srbGetMoreRows(conn, catType,
                 collResult[i].continuation_index, &collResult[i], 
		  MAX_GET_RESULTS);
                if (status1 == 0)
                    filterDeleted (&collResult[i]);
            } else
                clearSqlResult (&collResult[i]);

        } while ((dataResult[i].result_count > 0 && status == 0) ||
         (collResult[i].result_count > 0 && status1 == 0));
    }
		
    return (0);
}

int
setTrashPath (srbConn *conn, char *userName, char *domainName, 
srbPathName nameArray[])
{
    char *trashPath0, *trashPath1;
    char myMcatName[MAX_TOKEN];
    int status;
    int narg;
    char *thisMcat = NULL;
    char *thisUser = NULL, *thisDomain = NULL;

    trashPath0 = malloc (MAX_TOKEN);

    if (userName == NULL) {
	sprintf (trashPath0, "/trash/*");
    } else {
	sprintf (trashPath0, "/trash/home/%s.%s", userName, domainName);
    }
    nameArray[0].inpArgv = trashPath0;
#ifdef FED_MCAT
   thisMcat = getmcatNamefromHint (mdasCollectionName);
    if (thisMcat != NULL && (strcmp (thisMcat, "home") == 0 ||
     strcmp (thisMcat, "container") == 0 ||
     strcmp (thisMcat, "trash") == 0)) {
	free (thisMcat);
        thisMcat = NULL;
    }

    if (thisMcat == NULL) {
	if (userName == NULL || domainName == NULL) {
	    /* admin user */
            if (conn->clientUser == NULL) {
                thisUser = conn->proxyUser->userName;
                thisDomain = conn->proxyUser->domainName;
            } else {
                thisUser = conn->clientUser->userName;
                thisDomain = conn->clientUser->domainName;
            }
	} else {
	    thisUser = userName;
	    thisDomain = domainName;
	}

        status = srbGetMcatZone (conn, thisUser, thisDomain, myMcatName);
        if (status < 0) {
            fprintf (stderr,
             "setTrashPath: srbGetMcatZone error. status = %d\n", status);
            return (status);
        }
        thisMcat = myMcatName;
    }

    trashPath1 = malloc (MAX_TOKEN);
    if (userName == NULL) {
        sprintf (trashPath1, "/%s/trash/*", thisMcat);
    } else {
        sprintf (trashPath1, "/%s/trash/home/%s.%s", thisMcat, 
	 userName, domainName);
    }
    nameArray[1].inpArgv = trashPath1;

    narg = 2;
#else
    narg = 1;
#endif
    return narg;
}

void usage (char *prog)
{
  fprintf(stderr, "Usage: %s [-Ahu]  [-U user@domain]  [-t expiration_time_in_hours] [srbObj|collection  ... \n", prog);
}

