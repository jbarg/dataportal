/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* Srmcont - Remove a container */

#include "scommands.h"
int
rmContMain (srbConn *conn, int catType, int nArgv,
srbPathName nameArray[],int flagval);


extern char srbAuth[];
extern char srbHost[];
extern char srbUser[];
extern char mdasCollectionName[];
extern char inCondition[];

void
usage(char *prog)
{
  fprintf(stderr,"Usage: %s [-fa] [-z mcatName] container\n", prog);
  fprintf(stderr," f : forces the container to be emptied before removal\n");
}

int
main(int argc, char **argv)
{
    srbConn *conn;
    char containerName[MAX_TOKEN];
    int i;
    int status;
    int c;
    int flagval = 0;
    srbPathName nameArray[MAX_TOKEN];
    int nArgv;
    char *userName, *domainName;
    char *mcatName;
    char myMcatName[MAX_TOKEN];

    strcpy(inCondition , "");
    status = initSrbClientEnv();
    if (status < 0)
      {printf("Srmcont: Error:%i\n",status); exit(1);}

    while ((c=getopt(argc, argv, "lafhz:")) != EOF) {
        switch (c) {
	    case 'a':
                flagval |= a_FLAG;
                break;
            case 'f':
		flagval |= F_FLAG;
		/*	flagval |= F_FLAG;*/
                break;
            case 'h':
                usage (argv[0]);
                exit (0);
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

    if (optind >= argc) {
        usage (argv[0]);
        exit (1);
    }

    strcpy (containerName, argv[optind]);

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Srmcont: Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
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

    status = rmContMain (conn, MDAS_CATALOG, nArgv, nameArray,
      flagval);

    clFinish(conn);
    exit(0);
}

int
rmContMain (srbConn *conn, int catType, int nArgv,
srbPathName nameArray[],int flagval)
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
                if ((prevContColl == NULL || 
		 strcmp (contColl, prevContColl) != 0 ||
                 strcmp (contName, prevContName) != 0) &&
		 isContFamily (contName, nameArray[i].inpArgv)) {

                    if (flagval & F_FLAG) {

                        status = srbModifyDataset(conn, MDAS_CATALOG, 
			   contName, contColl,
			   "","",
			   "","", D_DELETE_ALL_FROM_CONTAINER);
	                if (status < 0) {
	                    fprintf(stderr,
			     "Srmcont: Error removing files:%s/%s:%i\n", 
		              contColl,contName, status);
	  		    srb_perror (2, status, "", 
			     SRB_RCMD_ACTION|SRB_LONG_MSG);
			    return (status);
		        }
		    }
		    sprintf (fullContName, "%s/%s", contColl, contName);
        	    status = rmContainer (conn, MDAS_CATALOG, fullContName, 
		     flagval);
                    if (status < 0) {
                        fprintf(stderr,
			 "Srmcont: Error removing container '%s': %i\n",
                         fullContName,status);
                        srb_perror (2, status, "", 
		         SRB_RCMD_ACTION|SRB_LONG_MSG);
                    }

		}
                prevContColl = contColl;
                prevContName = contName;
                contColl += MAX_DATA_SIZE;
                contName += MAX_DATA_SIZE;
	    }
	}
	clearSqlResult (&dataResult[i]);
    }
    return (status);
}

