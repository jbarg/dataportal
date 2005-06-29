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
extern char mdasCollectionName[];
extern char inCondition[];
extern int RemovingCont;
extern int RStatus;

void usage (char *prog);

int
main(int argc, char **argv)
{
    int c;
    int flagval = 0;
    int copyNum =-1;
    srbPathName nameArray[MAX_TOKEN], targNameArray[MAX_TOKEN];
    int nArgv;
    srbConn *conn;
    int status;

    strcpy(inCondition , "");
    if (argc < 2) {
	usage(argv[0]); 
	exit(1);
    }

    while ((c=getopt(argc, argv,"aA:n:pruf")) != EOF) {
        switch (c) {
            case 'n':
                copyNum = atoi(optarg);
                break;
            case 'a':
                flagval |= A_FLAG;
                break;
            case 'f':
                flagval |= f_FLAG;
                break;
            case 'p':
                flagval |= P_FLAG;
                break;
	    case 'A':
		strcpy(inCondition, optarg);
		break;
            case 'r':
                flagval |= R_FLAG;
                break;
            case 'u':
                flagval |= u_FLAG;
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }

    if (argc - optind < 1) {    /* must have at least 2 input */
        usage (argv[0]);
        exit (1);
    }

#ifdef NO_TRASH_SYSTEM
    flagval |= f_FLAG;
#endif

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Srm: Initialization Error:%i\n",i); exit(1);}

    for (i = optind; i < argc; i++) {
        nameArray[i - optind].inpArgv = argv[i];
    }
    nArgv = argc - optind;          /* nArgv is the number of in file */

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Srm: Connection to srbMaster failed.\n");
        fprintf(stderr,"Srm: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    /* Convert the source name to SQL like */
    if (convNameArrayToSQL (nameArray, nArgv) < 0)
      {clFinish(conn); exit(4);}

    RStatus = 0;
    rmMain (conn, MDAS_CATALOG, nArgv, nameArray, flagval, copyNum);

    if (RemovingCont > 0) {
	RStatus = 0;
	/* there is a container in the tree */
	rmMain (conn, MDAS_CATALOG, nArgv, nameArray, flagval, copyNum);
    }

    clFinish(conn);
    if (RStatus < 0) {
        srb_perror (2, RStatus, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	exit (1);
    } else {
        exit(0);
    }
}

int
rmMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
int flagval, int copyNum)
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
            fprintf (stderr, "Input %s not found\n", nameArray[i].inpArgv);
            return (CLI_ERR_COLLNAME);
        }
	do {
	    if (dataResult[i].row_count > 0) {      /* source is data */
		status = rmData (conn, catType, flagval, &dataResult[i], 
		 copyNum, 0);
		if (status == NO_ACCS_TO_USER_IN_COLLECTION) {
		    flagval |= f_FLAG;
		    status = rmData (conn, catType, flagval, &dataResult[i],
                     copyNum, 0);
		    if (status >= 0) {
			fprintf (stderr,
		         "Force removal was successful.Use Srm -f next time\n");
		    }
		}
	    }

	    if (collResult[i].row_count > 0) {      /* source is coll */
		if ((flagval & R_FLAG) == 0) {
		    fprintf (stderr, "Must use -r to rm collection %s.\n",
		      nameArray[i].inpArgv);
		} else if (nameArray[i].type == SPEC_COLL_T) {
		    fprintf (stderr, "rm of %s not allowed\n",
		      nameArray[i].inpArgv);
		} else {
		    status1 = rmColl (conn, catType, flagval, &collResult[i],
		     copyNum, 0);
                if (status1 == NO_ACCS_TO_USER_IN_COLLECTION) {
                    flagval |= f_FLAG;
                        status1 = rmColl (conn, catType, flagval, 
			 &collResult[i], copyNum, 0);
                    if (status1 >= 0) {
                        fprintf (stderr,
			 "Force removal was successful.Use Srm -f next time\n");
                    }
                }

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

void usage (char *prog)
{
  fprintf(stderr, "Usage: %s [-puf] [-n replicaNum]  [-A condition] srbObj  ... \n", prog);
  fprintf(stderr, "Usage: %s [-puf] [-n replicaNum]  [-A condition] -r srbObj|collection  ... \n", prog);
}

