/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "scommands.h"


 


extern char mdasCollectionName[];
extern char mdasCollectionHome[];
extern char srbAuth[];
extern char srbHost[];
extern char inCondition[];
extern int  curatorFlag;
extern int  hasPrinted;
void usage (char *prog);
int
main(int argc, char **argv)
{
    int c;
    char mode[MAX_TOKEN];
    int i, status, jj;
    srbPathName nameArray[MAX_TOKEN];
    int nArgv;          /* number of inpArgv */
    int flagval = 0;
    int Dflag = 0;
    int auditFlag = 0;
    char userName[MAX_TOKEN], domainName[MAX_TOKEN], userdom[MAX_TOKEN];
    srbConn *conn;

    curatorFlag = 0;
    hasPrinted = 0;
    strcpy(inCondition , "");
    while ((c=getopt(argc, argv,"hardpcDC")) != EOF) {
        switch (c) {
            case 'h':
                usage (argv[0]);
		exit (1);
                break;
            case 'r':
                flagval |= R_FLAG;
                break;
            case 'd':
                flagval |= d_FLAG;
                break;
            case 'C':
                curatorFlag = 1;
                break;
            case 'D':
                Dflag = 1;
                break;
            case 'p':
                flagval |= P_FLAG;
                break;
            case 'c':
                flagval |= C_FLAG;
                break;
            case 'a':
                auditFlag = 1;
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }

    if (argc < optind + 4) {		/* must have at least 4 inputs */
	fprintf(stderr,"Schmod: Must have at least 4 input values\n");
	usage (argv[0]);
	exit (1);
    }
    
    if ((flagval & C_FLAG) && (flagval & d_FLAG)) {
        fprintf(stderr,"Schmod:  -d and -c options cannot be both on.\n");
        usage (argv[0]);
	 exit (1);
    }

    if (auditFlag) {
      if (!strcmp(argv[optind], "r"))
        strcpy(mode,"read audit");
      else if (!strcmp(argv[optind], "w"))
        strcpy(mode,"write audit");
      else if (!strcmp(argv[optind], "a"))
        strcpy(mode,"all audit");
      else if (!strcmp(argv[optind], "t"))
        strcpy(mode,"annotate audit");
      else if (!strcmp(argv[optind], "c"))
        strcpy(mode,"curate audit");
      else if (!strcmp(argv[optind], "n"))
        strcpy(mode,"null");
      else if (!strcmp(argv[optind], "o"))
        strcpy(mode,"owner");
      else {
        fprintf(stderr,"Schmod:  Unknown permission: %s\n", argv[optind]);
        usage (argv[0]);
        exit (1);
      }
    }
    else {
      if (!strcmp(argv[optind], "r"))
        strcpy(mode,"read");
      else if (!strcmp(argv[optind], "w"))
        strcpy(mode,"write");
      else if (!strcmp(argv[optind], "a"))
        strcpy(mode,"all");
      else if (!strcmp(argv[optind], "t"))
        strcpy(mode,"annotate");
      else if (!strcmp(argv[optind], "c"))
        strcpy(mode,"curate");
      else if (!strcmp(argv[optind], "n"))
        strcpy(mode,"null");
      else if (!strcmp(argv[optind], "o"))
        strcpy(mode,"owner");
      else {
        fprintf(stderr,"Schmod:  Unknown permission: %s\n", argv[optind]);
        usage (argv[0]);
        exit (1);
      }
    }

    strcpy(userName,argv[++optind]);
    strcpy(domainName,argv[++optind]);
    sprintf(userdom,"%s@%s",userName,domainName);
    optind++;

    for (i = optind; i < argc; i++) {
	nameArray[i - optind].inpArgv = argv[i];
    }
    nArgv = argc - optind;

   i = initSrbClientEnv();
    if (i < 0)
      {printf("Sls Initialization Error:%i\n",i); exit(1);}

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
      fprintf(stderr,"Connection to srbMaster failed.\n");
      fprintf(stderr,"%s",clErrorMessage(conn));
      clFinish(conn); exit(3);
    }

    /* Convert the name to SQL like */
    if (convNameArrayToSQL (nameArray, nArgv) < 0)
      {clFinish(conn); exit(4);}

    /* added for speeding up for D and r options ***/
    if (Dflag) {
      if (flagval & R_FLAG) {
	for (i = 0; i < nArgv; i++) {
	  if (curatorFlag == 0) {
	    if (!strcmp(mode,"owner"))
	      status = srbModifyCollect (conn, MDAS_CATALOG, nameArray[i].outArgv,
		       userName, domainName,"", D_CHANGE_INCOLL_OWNER_RECUR);
	    else if (!strcmp(mode,"null"))
	      status = srbModifyCollect (conn, MDAS_CATALOG, nameArray[i].outArgv,
		       userName, domainName, mode , D_DELETE_INCOLL_ACCS_RECUR);
	    else
	      status = srbModifyCollect (conn, MDAS_CATALOG, nameArray[i].outArgv,
		        userName, domainName, mode , D_INSERT_INCOLL_ACCS_RECUR);
	  }
	  else { /* curatorFlag = 1 */
	    if (!strcmp(mode,"owner"))
	      status = srbModifyCollect (conn, MDAS_CATALOG, nameArray[i].outArgv,
		       userName, domainName,"", D_CHANGE_INCOLL_OWNER_RECUR_BY_CURATOR);
	    else if (!strcmp(mode,"null"))
	      status = srbModifyCollect (conn, MDAS_CATALOG, nameArray[i].outArgv,
		       userName, domainName, mode ,D_DELETE_INCOLL_ACCS_RECUR_BY_CURATOR);
	    else
	      status = srbModifyCollect (conn, MDAS_CATALOG, nameArray[i].outArgv,
		        userName, domainName, mode,D_INSERT_INCOLL_ACCS_RECUR_BY_CURATOR);
	  }
	  if (status != 0) {
	    fprintf(stderr,"srbModifyCollect Failed for %s: %i.\n",
		    nameArray[i].outArgv,status);
	    fprintf(stderr,"%s",clErrorMessage(conn));
	    clFinish(conn); 
	    exit(1);
	  }
	}
	clFinish(conn); 
	exit(0);
      }
      else {
	for (i = 0; i < nArgv; i++) {
	  if (curatorFlag == 0) {
	    if (!strcmp(mode,"owner"))
	      status = srbModifyCollect (conn, MDAS_CATALOG, nameArray[i].outArgv,
		  userName, domainName,"", D_CHANGE_INCOLL_OWNER);
	    else if (!strcmp(mode,"null"))
	      status = srbModifyCollect (conn, MDAS_CATALOG, nameArray[i].outArgv,
		   userName, domainName, mode , D_DELETE_INCOLL_ACCS);
	    else
	      status = srbModifyCollect (conn, MDAS_CATALOG, nameArray[i].outArgv,
		   userName, domainName, mode , D_INSERT_INCOLL_ACCS);
	  }
	  else { /* curatorFlag = 1 */
	    if (!strcmp(mode,"owner"))
	      status = srbModifyCollect (conn, MDAS_CATALOG, nameArray[i].outArgv,
		  userName, domainName,"", D_CHANGE_INCOLL_OWNER_BY_CURATOR);
	    else if (!strcmp(mode,"null"))
	      status = srbModifyCollect (conn, MDAS_CATALOG, nameArray[i].outArgv,
		   userName, domainName, mode , D_DELETE_INCOLL_ACCS_BY_CURATOR);
	    else
	      status = srbModifyCollect (conn, MDAS_CATALOG, nameArray[i].outArgv,
		   userName, domainName, mode , D_INSERT_INCOLL_ACCS_BY_CURATOR);
	  }
	  if (status != 0) {
	    fprintf(stderr,"srbModifyCollect Failed for %s: %i.\n",
		    nameArray[i].outArgv,status);
	    fprintf(stderr,"%s",clErrorMessage(conn));
	    clFinish(conn); 
	    exit(1);
	  }
	}
	clFinish(conn); 
	exit(0);
      }
    }
    /* added for speeding up for D and r options ***/


    /* do the chmod */

    jj = queryAndChmod (conn, MDAS_CATALOG, nArgv, nameArray, flagval, mode, userdom);
    if (jj < 0 || hasPrinted ==0)
      exit(1);
    exit(0);
}

int 
queryAndChmod (srbConn *conn, int catType, int nArgv,
srbPathName nameArray[], int flagval, char *mode, char *userdom)
{
    mdasC_sql_result_struct  myresult, collResult;
    int i;
    int status1 = 0;
    int status = 0;
    char tmpNameStore[MAX_TOKEN * 2];
    for (i = 0; i < nArgv; i++) {
        /* May be it is a dataName */
         strcpy(tmpNameStore,nameArray[i].outArgv);
	if (nameArray[i].type != COLLECTION_T && (flagval & C_FLAG) == 0) {
	    status1 = expandChmodData (conn, catType, nameArray[i].outArgv, 
	      flagval, mode, userdom);
	}
	if ((flagval & d_FLAG) == 0 || (flagval & R_FLAG)) 
	    status = chmodInColl (conn, catType, tmpNameStore, 
	     flagval, mode, userdom);

	if (status1 < 0 && status < 0)
	    fprintf (stderr, "%s: No such dataset or collection\n",
              nameArray[i].inpArgv);
	else
	  hasPrinted = 1;
    }
    return (0);
}

/* expandChmodData - Chmod the dataName including the effects of special char.
 * put the results in dataResult.
 *
 */

int
expandChmodData (srbConn *conn, int catType, char *dataName, int flagval, 
char *mode, char *userdom)
{
    int i, j;
    int status;
    char *rval[MAX_DCS_NUM];
    mdasC_sql_result_struct dataResult;
    int AskVal = 1;
    char inbuf[100];

   
    status = queryData (conn, catType, dataName, flagval, MAX_GET_RESULTS, 
      &dataResult);

    if (status < 0)
        return status;

    chmodDataByRes (conn, catType, flagval, mode, userdom, &dataResult);

    /* loop until no more answer */

    while (dataResult.continuation_index >= 0) {
        clearSqlResult (&dataResult);
        status = srbGetMoreRows(conn, MDAS_CATALOG,
          dataResult.continuation_index, &dataResult, MAX_GET_RESULTS);
        if (dataResult.row_count == 0  || status != 0)
            return (0);
	chmodDataByRes (conn, catType, flagval, mode, userdom, &dataResult);
    }
    clearSqlResult (&dataResult);
    return (0);
}

void
 usage (char *prog)
{
    fprintf (stderr, "Usage : %s [-h] [-p] [-a] [-r] [-d|c|D] [-C] w|a|r|n|t|o|c newUserName domainName collection|srbObj ....\n", prog);
    fprintf (stderr, "w - write;r - read;a - all permission;n - null; t - annotate; c - curate (collection); o - change owner\n");
}

