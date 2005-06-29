/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*
 * Schksum.c
 */

#include "scommands.h"
#include <dirent.h>

extern char srbAuth[];
extern char srbHost[];
extern char mdasCollectionName[];
extern char inCondition[];
 
extern int iGlbRetriesMax;

int  nbytes, wbytes, in_fd;
extern int RStatus;

void usage();

int
chksumMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
int flagval, int copyNum);

int
main(int argc, char **argv)
{
    int c;
    int flagval = 0;
    srbPathName srcNameArray[MAX_TOKEN];
    int nArgv;
    srbConn *conn;
    int copyNum =-1;
    int i, ii;

    strcpy(inCondition , "");

    while ((c=getopt(argc, argv,"vn:frlcsA:")) != EOF) {
      switch (c) {
      case 'n':
	copyNum = atoi(optarg);
	break;
      case 'f':
	flagval |= f_FLAG;
	break;
      case 'l':
	flagval |= l_FLAG;
	break;
      case 'r':
	flagval |= R_FLAG;
	break;
      case 'c':
	flagval |= c_FLAG; 
	break;
      case 'v':
	flagval |= v_FLAG; 
	break;
      case 's':
	flagval |= s_FLAG; 
	break;
      case 'A':
        strcpy(inCondition, optarg);
        break;
      default:
	usage (argv[0]);
	exit (1);
      }
    }
    
    nArgv = argc - optind;

    if (nArgv == 0) {      /* must have at least 1 input */
	srcNameArray[0].inpArgv =  ".";
	nArgv = 1;
    } else {
        for (i = optind; i < argc; i++) {
            srcNameArray[i - optind].inpArgv = argv[i];
        }
    }

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Schksum: Connection to srbMaster failed.\n");
        fprintf(stderr,"Schksum: %s\n",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(3);
    }

    /* Convert the source name to SQL like */
    if (convNameArrayToSQL (srcNameArray, nArgv) < 0)
        {clFinish(conn); exit(3);}

    ii= chksumMain (conn, MDAS_CATALOG, nArgv, srcNameArray, flagval,
      copyNum);
 
    clFinish(conn);
   if (ii == 0 && RStatus == 0)
      exit(0);
    else
      exit(1);
}


int
chksumMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
int flagval, int copyNum)
{
    int i;
    int status=(-1), status1=0;
    mdasC_sql_result_struct *dataResult, *collResult;
    int dataCnt = 0;
    int collCnt = 0;
    char currentColl[MAX_TOKEN];


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
        if (status < 0) {
            fprintf (stderr, "Source %s not found\n", nameArray[i].inpArgv);
            return (CLI_ERR_COLLNAME);
        }

        dataCnt += dataResult[i].row_count;
        collCnt += collResult[i].row_count;
    }

    /* Do some sanity checks */

    if (dataCnt + collCnt == 0) {
        fprintf (stderr, "Source %s not found\n",
          nameArray[0].inpArgv);
        return (CLI_ERR_COLLNAME);
    }

    if (collCnt > 0 && (flagval & R_FLAG) == 0 && (flagval & b_FLAG) == 0) {
        fprintf (stderr, "Must use the -r flag to copy a collection\n");
        return (CLI_ERR_COLLNAME);
    }

    currentColl[0] = '\0';
    status = 0;
    for (i = 0; i < nArgv; i++) {
      do {
        if (dataResult[i].row_count > 0) {      /* source is data */
	    status = dataChksum (conn, &dataResult[i], 
	      flagval, copyNum, currentColl);  
        }

        if (collResult[i].row_count > 0) {      /* source is coll */
            status = collChksum (conn, &collResult[i], 
	     flagval, copyNum, currentColl);
        }
        if (status == MCAT_INQUIRE_ERROR)
            status = 0;          
        else if (status < 0)
            break;

        if (dataResult[i].continuation_index >= 0 &&
          dataResult[i].row_count > 0) {
            clearSqlResult (&dataResult[i]);
            status = srbGetMoreRows(conn, catType,
             dataResult[i].continuation_index, &dataResult[i], MAX_GET_RESULTS);
            if (status == 0)
                filterDeleted (&dataResult[i]);
        } else
            clearSqlResult (&dataResult[i]);

        if (collResult[i].continuation_index >= 0 &&
          collResult[i].result_count > 0) {
            clearSqlResult (&collResult[i]);
            status1 = srbGetMoreRows(conn, catType,
             collResult[i].continuation_index, &collResult[i], MAX_GET_RESULTS);
            if (status1 == 0)
                filterDeleted (&collResult[i]);
        } else
            clearSqlResult (&collResult[i]);

      } while ((dataResult[i].result_count > 0 && status == 0) ||
      (collResult[i].result_count > 0 && status1 == 0));
      if (status == MCAT_INQUIRE_ERROR)
        status = 0;
      else if (status < 0)
	break;
    }

    return (status);
}

void
usage()
{
  fprintf(stderr, "Usage: Schksum [-n n] [-frlcvs] [srbObj|collection ...]  \n");
}

