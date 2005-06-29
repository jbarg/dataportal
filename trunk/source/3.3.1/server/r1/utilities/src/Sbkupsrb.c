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

srbConn *conn;

int
backupMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
char *resourcename, int flagval);

int
expandBackupData (srbConn *conn, int catType, char *dataName,
char *resourcename, int flagval, srbConn **myConn);

void usage(char *prog)
{
	   
  fprintf(stderr, 
   "Usage: %s [-raA]  -S backupResource srbObj|collection  ...\n", prog);
}
int
main(int argc, char **argv)
{
    int i;
    char resourcename[MAX_TOKEN];
    int c;
    srbConn *conn;
    int sFlag = 0;              /* -S option */
    int flagval = 0;
    int nArgv;          /* number of inpArgv */
    srbPathName nameArray[MAX_TOKEN];
    int status;


    if (argc < 2) {
	usage(argv[0]); 
	exit(1);
    }
    strcpy(inCondition , "");
    while ((c=getopt(argc, argv,"S:aAr")) != EOF) {
        switch (c) {
            case 'S':
                sFlag = 1;
                strcpy (resourcename, optarg);
                break;
            case 'a':
                flagval |= a_FLAG;
                break;
            case 'A':
                flagval |= A_FLAG;
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
	fprintf (stderr, "Must specify a backupResource\n");
        usage (argv[0]);
        exit (1);
    }

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Sbkupsrb: Initialization Error:%i\n",i); exit(-1);}


    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sbkupsrb: Connection to srbMaster failed.\n");
        fprintf(stderr,"Sbkupsrb: %s",clErrorMessage(conn));
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

    status = backupMain (conn, MDAS_CATALOG, nArgv, nameArray, resourcename,
     flagval);

    clFinish(conn);
    if (status >= 0)
        exit(0);
    else
        exit(1);
}

int
backupMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
char *resourcename, int flagval)
{
    int i;
    int status1 = 0;
    int status = 0;
    int saveStatus = 0;
    srbConn *myConn = NULL;

    flagval |= B_FLAG;		/* B_FLAG now stand for backup */

    for (i = 0; i < nArgv; i++) {
        /* May be it is a dataName */
        if (nameArray[i].type != COLLECTION_T && 
	 nameArray[i].type != SPEC_COLL_T) {
	    status1 = expandBackupData (conn, catType, nameArray[i].outArgv,
              resourcename, flagval, &myConn);
        }
        if (flagval & R_FLAG)
            status = backupInColl (conn, catType, nameArray[i].outArgv,
             resourcename, flagval, &myConn);
	if (status == 0) {
	    if (status1 < 0 && saveStatus == 0)
		saveStatus = status1;
	}
        if (status < 0) {
	    if (status < 0 && saveStatus == 0)
                saveStatus = status;
            fprintf (stderr, "Sync data error for %s. Status = %d.\n",
              nameArray[i].inpArgv, status);
	    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	}
    }
    return (saveStatus);
}

/* expandBackupData - Sync the dataName including the effects of special 
 * char.  put the results in dataResult.
 *
 */

int
expandBackupData (srbConn *conn, int catType, char *dataName, 
char *resourcename, int flagval, srbConn **myConn)
{
    int status;
    mdasC_sql_result_struct dataResult;

  
    status = queryData (conn, catType, dataName, flagval, MAX_GET_RESULTS,
      &dataResult);

    if (status < 0) {
	if (status == MCAT_INQUIRE_ERROR)
	    return 0;
	else
            return status;
    }

    status = backupDataByRes (conn, catType, resourcename, 
	flagval, &dataResult, myConn);

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
        status = backupDataByRes (conn, catType, resourcename, flagval, 
	 &dataResult, myConn);
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
