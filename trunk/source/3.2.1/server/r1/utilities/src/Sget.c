/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*
 * Sget.c
 */

#include "scommands.h"
#include <dirent.h>

extern char srbAuth[];
extern char srbHost[];
extern char mdasCollectionName[];
extern char inCondition[];
 
extern int iGlbRetriesMax;
extern int RStatus;
extern NumThreads;

int  nbytes, wbytes, in_fd;
int i;
int lcval;
void usage();

int
main(int argc, char **argv)
{

    int  newargc;
    char newargv[MAX_ARG_EXP_CNT][MAX_TOKEN];
    int c;
    int flagval = 0;
    srbPathName targNameArray[MAX_TOKEN], srcNameArray[MAX_TOKEN];
    int nArgv;
    char collname[MAX_TOKEN];
    char dataname[MAX_TOKEN];
    srbConn *conn;
    FILE *fd;
    int status;
    char ticketId[MDAS_TICKET_SIZE+10];
    int copyNum =-1;
    int i, ii;
    int numArg;
    int option_index = 0;

    iGlbRetriesMax=(-1);
    strcpy(ticketId , "");
    strcpy(inCondition , "");
    lcval = 0;
    while ((c=getopt(argc, argv,"N:n:t:T:A:R:pfrvsmMkVbc:")) != EOF) {
      switch (c) {
      case 'n':
	copyNum = atoi(optarg);
	break;
      case 'N':
	NumThreads = atoi(optarg);
	break;
      case 'b':
        flagval |= b_FLAG;
        break;
      case 'T':
	fd = fopen(optarg, "r");
	if (fd == NULL) {
	  fprintf(stderr,
		  "Sget: Unable to open Ticket File %s\n",optarg);
	  exit(1);
	}
	strcpy(ticketId,"'");
	fgets((char *)ticketId+1, MDAS_TICKET_SIZE + 1, fd);
	strcat(ticketId,"'");
	fclose(fd);
	break;
      case 't':
	strcpy(ticketId,"'");
	strcat(ticketId, optarg);
	strcat(ticketId,"'");
	break;
      case 'A':
	strcpy(inCondition, optarg);
	break;
      case 'c':
	strcpy(inCondition, optarg);
	lcval = 1;
	break;
      case 'f':
	flagval |= F_FLAG;
	break;
      case 'p':
	flagval |= P_FLAG;
	break;
      case 'r':
	flagval |= R_FLAG;
	break;
      case 's':
	flagval |= s_FLAG;      /* single thread */
	break;
      case 'm':
	flagval |= m_FLAG;      /* multi thread */
	break;
      case 'M':
	flagval |= M_FLAG;      /* client initiated multi thread */
	flagval |= m_FLAG;      /* multi thread */
	break;
      case 'v':                   /* verbose flag */
	flagval |= v_FLAG;
	break;
      case 'V':                   /* verbose progress bar flag */
	flagval |= V_FLAG;
	flagval |= v_FLAG;        /* force also final report */
	break;
      case 'R':                   /* retry flag */
	flagval |= retry_FLAG;
	iGlbRetriesMax = atoi(optarg);
	if (iGlbRetriesMax == 0 && 0 != strncmp(optarg, "0", 1)) {
	  /* bad number on input */
	  usage (argv[0]);
	  exit (1);
	}
	  if (iGlbRetriesMax < 1) iGlbRetriesMax = 0; 
	  break;
      case 'k':                   /* check sum flag */
	flagval |= k_FLAG; 
	break;
      default:
	usage (argv[0]);
	exit (1);
      }
    }
    
    numArg = argc - optind;
    if (numArg <= 0 && lcval == 0) {      /* must have at least 1 input */
        usage (argv[0]);
        exit (1);
    } else if (numArg == 1  && lcval == 0) {
        /* only one arg, made the second arg '.' */
	targNameArray[0].inpArgv = ".";
        argc++;
    } else if (numArg == 0  && lcval == 1) {
	targNameArray[0].inpArgv = ".";
        argc++;
    } else {
	targNameArray[0].inpArgv = argv[argc-1];
    }

    i = initSrbClientEnv();
    if (i < 0) {
	printf("Sget: Initialization Error:%i\n",i); 
	exit(1);
    }

    /* Set up the target arrays */

    for (i = optind; i < argc - 1; i++) {
        srcNameArray[i - optind].inpArgv = argv[i];

    }

    nArgv = argc - optind - 1;          /* nArgv is the number of in file */
    
   
    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sget: Connection to srbMaster failed.\n");
        fprintf(stderr,"Sget: %s\n",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(3);
    }

    /* Convert the source name to SQL like */
    if (lcval == 1) {
      /*   i =  getSrbObjNameFromCondition(srcNameArray[0].inpArgv, srcNameArray,nArgv);
      if (i < 0)
      {clFinish(conn); exit(3);} */
      strcpy(collname,"");
      srcNameArray[0].inpArgv = collname;
      nArgv = 1;   
    }

    if (convNameArrayToSQL (srcNameArray, nArgv) < 0)
        {clFinish(conn); exit(3);}
    
    if (lcval == 1) {
      strcpy(srcNameArray[0].outArgv,"");
      srcNameArray[0].type = DATANAME_T;
    }
    ii= getMain (conn, MDAS_CATALOG, nArgv, srcNameArray, targNameArray, flagval,
      copyNum, ticketId);
 
    clFinish(conn);
    if (ii >= 0 && RStatus >= 0)
      exit(0);
    else
      exit(1);
}


int
getMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
srbPathName targNameArray[], int flagval, int copyNum, char *ticketId)
{
    int i;
    int status=(-1), status1=0;
    mdasC_sql_result_struct *dataResult, *collResult;
    int targType;
    int dataCnt = 0;
    int collCnt = 0;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    char srcPar[MAX_TOKEN], srcCh[MAX_TOKEN];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN];
    char *tmpStr, *tmpStr1;


    status = chkFileName (&targNameArray[0]);

    targType = targNameArray[0].type;
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
          targNameArray[0].inpArgv);
        return (CLI_ERR_COLLNAME);
    }

    if ((dataCnt + collCnt > 1 || collCnt > 0) && targType >= DATANAME_T) {
        fprintf (stderr, "Target %s must be a directory\n",
          targNameArray[0].inpArgv);
        return (CLI_ERR_COLLNAME);
    }

    if (collCnt > 0 && (flagval & R_FLAG) == 0 && (flagval & b_FLAG) == 0) {
        fprintf (stderr, "Must use the -r flag to copy a collection\n");
        return (CLI_ERR_COLLNAME);
    }

    if (targType == -1) {       /* Target does not exist */
        if (dataCnt + collCnt > 1) {
            fprintf (stderr, "Target directory %s not found\n",
              targNameArray[0].inpArgv);
            return (CLI_ERR_COLLNAME);
        }
    }

    status = 0;
    for (i = 0; i < nArgv; i++) {
      do {
        if (dataResult[i].row_count > 0) {      /* source is data */
            if (targType == -1 || targType >= DATANAME_T) {       
		/* A normal data-file copy */
                status = dataToFileCopy (conn, catType, flagval, targType,
                  &dataResult[i], copyNum, targNameArray[0].outArgv, 
                   ticketId);
            } else if (targType == COLLECTION_T || targType == SPEC_COLL_T) {
                status = dataToDirCopy (conn, catType, flagval,
                 &dataResult[i], copyNum, targNameArray[0].outArgv,
                   ticketId);
            }
        }

        if (collResult[i].row_count > 0) {      /* source is coll */
            if (!(flagval & b_FLAG) && collCnt == 1 && targType == -1) {
            /* if (collCnt == 1 && targType == -1) { */
                /* the source is a single coll */
                clSplitbychar (targNameArray[0].outArgv, temp, temp1, '/');
                status = mkdir (targNameArray[0].outArgv, 0755);
                if (status < 0) {
                    fprintf(stderr,
                     "Unable to mkdir %s, status = %d\n",
                      targNameArray[0].inpArgv, status);
                    return (CLI_ERR_COLLNAME);
                }
                /* mark it as COLLECTION_T */
                targNameArray[0].type = COLLECTION_T;
                targType = COLLECTION_T;
                tmpStr = (char *) getFromResultStruct (&collResult[i],
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
                status = collToDirCopyCont (conn, catType, flagval, tmpStr,
                 targNameArray[0].outArgv, ticketId);
            } else {
		if (flagval & b_FLAG) {
		    char *myColl;
		    srbPathName collNameArray, *contnameArray;
		    int nVal;

    		    myColl = (char *) getFromResultStruct(&collResult[i],
      		    dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

		    collNameArray.inpArgv = strdup (myColl);
		    collNameArray.outArgv = strdup (myColl);

		    nVal = getContainerByColl (conn, &collNameArray, 
                     &contnameArray);
		    if (nVal <= 0) {
			status = nVal;
		    } else {
			status = unloadContMain (conn, catType, nVal, 
			 contnameArray, flagval, targNameArray[0].outArgv, 
			 &collNameArray);
		    }
		} else {
                    status = collToDirCopy (conn, catType, flagval,
                     nameArray[i].type, &collResult[i], 
		     targNameArray[0].outArgv, ticketId);
		}
            }
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
  fprintf(stderr, "Usage: Sget [-n n] [-N numThreads] [-bpfrvsmMkV] [-R retries] [-T ticketFile | -t ticket] [-A condition] srbObj ...  localFile/Dir \n");
  fprintf(stderr, "Usage: Sget -c condition localFile/Dir \n");

}

