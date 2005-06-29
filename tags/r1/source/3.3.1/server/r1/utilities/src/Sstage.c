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
extern char mdasCollectionHome[];
extern char inCondition[];
extern int RStatus;

srbConn *conn;

int
stageMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
char *resourcename, int flagval);

int
expandStageData (srbConn *conn, int catType, char *dataName,
char *resourcename, int flagval);

int
stageInColl (srbConn *conn, int catType, char *collection, char *resourcename,
int flagval);

int
stageDataByRes (srbConn *conn, int catType, char *resourcename, int flagval,
mdasC_sql_result_struct *myresult);

void usage(char *prog)
{
	   
  fprintf(stderr, 
   "Usage: %s [-raA]  -S stageResource srbObj|collection  ...\n", prog);
}
int
main(int argc, char **argv)
{
    int i, status;
    char resourcename[MAX_TOKEN];
    int c;
    srbConn *conn;
    int sFlag = 0;              /* -S option */
    int flagval = 0;
    int nArgv;          /* number of inpArgv */
    srbPathName nameArray[MAX_TOKEN];


    if (argc < 2) {
	usage(argv[0]); 
	exit(0);
    }
    strcpy(inCondition , "");
    while ((c=getopt(argc, argv,"S:r")) != EOF) {
        switch (c) {
            case 'S':
                sFlag = 1;
                strcpy (resourcename, optarg);
                break;
            case 'r':
                flagval |= R_FLAG;
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }

    if (argc - optind < 1) {    /* must have at least 1 input */
        usage (argv[0]);
        exit (1);
    }

    if (sFlag == 0) {
	resourcename[0] = '\0';
    }

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Sstage: Initialization Error:%i\n",i); exit(-1);}


    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sstage: Connection to srbMaster failed.\n");
        fprintf(stderr,"Sstage: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    for (i = optind; i < argc; i++) {
        nameArray[i - optind].inpArgv = argv[i];
    }
    nArgv = argc - optind;          /* nArgv is the number of in file */

    /* Convert the source name to SQL like */
    if (convNameArrayToSQL (nameArray, nArgv) < 0)
      {clFinish(conn); exit(4);}

    status = stageMain (conn, MDAS_CATALOG, nArgv, nameArray, resourcename,
     flagval);

    clFinish(conn);
    if (RStatus < 0 || status < 0) {
	exit (1);
    } else {
        exit(0);
    }
}

int
stageMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
char *resourcename, int flagval)
{
    int i;
    int status1 = 0;
    int status = 0;
    int savedStatus = 0;

    for (i = 0; i < nArgv; i++) {
        /* May be it is a dataName */
        if (nameArray[i].type != COLLECTION_T && 
	 nameArray[i].type != SPEC_COLL_T) {
	    status1 = expandStageData (conn, catType, nameArray[i].outArgv,
              resourcename, flagval);
        } else {
	    status1 = MCAT_INQUIRE_ERROR;
	}

        if (flagval & R_FLAG) {
            status = stageInColl (conn, catType, nameArray[i].outArgv,
             resourcename, flagval);
	} else {
	    status = MCAT_INQUIRE_ERROR;
	}

	if (status < 0 && status1 < 0) {
            if (status == MCAT_INQUIRE_ERROR) {
		savedStatus = status1;
		if (status1 == MCAT_INQUIRE_ERROR) {
                    fprintf (stderr, "file/collection %s does not exist, or -r option must be used for collection\n",
                      nameArray[i].outArgv);
		}
	    } else {
		savedStatus = status;
	    } 
	}
    }
    return (savedStatus);
}

/* expandStageData - Sync the dataName including the effects of special 
 * char.  put the results in dataResult.
 *
 */

int
expandStageData (srbConn *conn, int catType, char *dataName, 
char *resourcename, int flagval)
{
    int status;
    mdasC_sql_result_struct dataResult;

  
    status = queryData (conn, catType, dataName, flagval, MAX_GET_RESULTS,
      &dataResult);

    if (status < 0) {
        return status;
    }

    status = stageDataByRes (conn, catType, resourcename, 
	flagval, &dataResult);

    if (status < 0) {
        if (status == MCAT_INQUIRE_ERROR)
            return 0;
        else {
	    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return status;
	}
    }

    /* loop until no more answer */

    while (dataResult.continuation_index >= 0) {
        clearSqlResult (&dataResult);
        status = srbGetMoreRows(conn, MDAS_CATALOG,
          dataResult.continuation_index, &dataResult, MAX_GET_RESULTS);
        if (dataResult.row_count == 0  || status != 0)
            return (0);
        status = stageDataByRes (conn, catType, resourcename, flagval, 
	 &dataResult);
        if (status < 0) {
            if (status == MCAT_INQUIRE_ERROR)
                return 0;
            else {
                srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                return status;
            }
	}
    }
    clearSqlResult (&dataResult);
    return (0);
}

int
stageDataByRes (srbConn *conn, int catType, char *resourcename, int flagval,
mdasC_sql_result_struct *myresult)
{
    int i;
    char *dataList, *collList;
    int status;

    dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    collList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    for (i = 0; i < myresult->row_count; i++) {
	status = srbObjFStage (conn, catType, dataList, collList, 
	 resourcename, 0);

	if (status < 0) {
            fprintf(stderr,
             "stageDataByRes: srbObjFStage of %s/%s. status = %d\n",
	     collList, dataList, status);
             srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    RStatus = status;
	}

        dataList += MAX_DATA_SIZE;
        collList += MAX_DATA_SIZE;
    }
    return (status);
}

int
stageInColl (srbConn *conn, int catType, char *collection, char *resourcename, 
int flagval)
{
    int status, status1;
    mdasC_sql_result_struct myresult;

    status = queryDataInColl (conn, catType, collection, y_FLAG | flagval,
     MAX_GET_RESULTS, &myresult);

    if (status < 0) {
        if (status != MCAT_INQUIRE_ERROR) {
	    RStatus = status;
            srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        }
        return status;
    }

    do {
        int continuation_index;

	status = stageDataByRes (conn, catType, resourcename,
        flagval, &myresult);

	continuation_index = myresult.continuation_index;
	clearSqlResult (&myresult);
	if (continuation_index >= 0) {
	    status1 = srbGetMoreRows(conn, MDAS_CATALOG,
             continuation_index, &myresult, MAX_GET_RESULTS);
	} else {
	    status1 = -1;
	}
    } while (status1 >= 0);

    return (status);
}

