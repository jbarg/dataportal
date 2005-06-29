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


void usage(char *prog)
{
	   
  fprintf(stderr, 
   "Usage: %s [-n replicaNum] [-prv]  [-S resourceName] [-P pathName] srbObj|collection  ...\n", prog);
}
int
main(int argc, char **argv)
{

    srbResult *res;

    int i,j, pval, mval, lcount;
    char collname[MAX_TOKEN];
    char dataname[MAX_TOKEN];
    char filename[MAX_TOKEN];
    char srbobj[MAX_TOKEN][MAX_TOKEN];
    char resourcename[MAX_TOKEN];
    char inbuf[100];
    char pathname[MAX_TOKEN];
    char tmpStr[MAX_TOKEN];
    int c;
    srbConn *conn;
    int status;
    int nFlag = 0;              /* -c Flag */
    int SFlag = 0;              /* -S option */
    int PFlag = 0;              /* -P option */
    int flagval = 0;
    int nArgv;          /* number of inpArgv */
    srbPathName nameArray[MAX_TOKEN];
    int   copyNum = -1;


    if (argc < 2) {
	usage(argv[0]); 
	exit(1);
    }
    strcpy(inCondition , "");
    while ((c=getopt(argc, argv,"n:S:P:prv")) != EOF) {
        switch (c) {
            case 'n':
                copyNum = atoi(optarg);
                break;
            case 'S':
                SFlag = 1;
                strcpy (resourcename, optarg);
                break;
            case 'P':
                PFlag = 1;
                strcpy (pathname, optarg);
                break;
            case 'p':
                flagval |= P_FLAG;
                break;
            case 'r':
                flagval |= R_FLAG;
                break;
            case 'v':
                flagval |= v_FLAG;
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

    if (SFlag == 0)
        strcpy (resourcename, "");

    if (PFlag == 0)
        strcpy (pathname, "");

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Sreplicate: Initialization Error:%i\n",i); exit(-1);}


    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sreplicate: Connection to srbMaster failed.\n");
        fprintf(stderr,"Sreplicate: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    if (strlen(resourcename) == 0)
	  get_user_preference(resourcename, "DRSRC");

    for (i = optind; i < argc; i++) {
        nameArray[i - optind].inpArgv = argv[i];
    }
    nArgv = argc - optind;          /* nArgv is the number of in file */

    /* Convert the source name to SQL like */
    if (convNameArrayToSQL (nameArray, nArgv) < 0)
      {clFinish(conn); exit(4);} 

    i = replMain (conn, MDAS_CATALOG, nArgv, nameArray, pathname, resourcename,
     flagval, copyNum);
    clFinish(conn);
    if (i < 0) 
      exit(5);
    exit(0);

}

int
replMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
char *pathname, char *resourcename, int flagval, int copyNum)
{
    mdasC_sql_result_struct  myresult, collResult;
    int i;
    int status1 = 0;
    int status = 0;
    int status2 = 0; 
    for (i = 0; i < nArgv; i++) {
        /* May be it is a dataName */
        if (nameArray[i].type != COLLECTION_T && 
	 nameArray[i].type != SPEC_COLL_T) {
	    status1 = expandReplData (conn, catType, nameArray[i].outArgv,
              pathname, resourcename, flagval, copyNum);
        }
        if (flagval & R_FLAG)
            status = replInColl (conn, catType, nameArray[i].outArgv,
             pathname, resourcename, flagval, copyNum);
	if (status == 0) {
	    if (status1 < 0)
		status = status1;
	}
        if (status < 0) {
            fprintf (stderr, "Replication error for %s. Status = %d.\n",
              nameArray[i].inpArgv, status);
	    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    status2 = status;
	}
    }
    return (status2);
}

/* expandReplData - Replicate the dataName including the effects of special 
 * char.  put the results in dataResult.
 *
 */

int
expandReplData (srbConn *conn, int catType, char *dataName, 
char *pathname, char *resourcename, int flagval, int copyNum)
{
    int i, j;
    int status;
    char *rval[MAX_DCS_NUM];
    mdasC_sql_result_struct dataResult;
    int AskVal = 1;
    char inbuf[100];

  
    status = queryData (conn, catType, dataName, flagval, MAX_GET_RESULTS,
      &dataResult);

    if (status < 0) {
	if (status == MCAT_INQUIRE_ERROR)
	    return 0;
	else
            return status;
    }

    status = replDataByRes (conn, catType, pathname, resourcename, 
	flagval, &dataResult, copyNum);

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
        replDataByRes (conn, catType, pathname, resourcename, flagval, 
	 &dataResult, copyNum);
    }
    clearSqlResult (&dataResult);
    return (0);
}
