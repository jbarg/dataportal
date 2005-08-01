/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*
 * Scp.c
 */
#include "scommands.h"
#ifndef _WIN32
#include <unistd.h>
#endif



extern char *optarg;
extern int optind, opterr, optopt;

extern char srbAuth[];
extern char srbHost[];
extern char mdasCollectionName[];
extern char inCondition[];
extern int  linkFlag;
extern int RStatus;

int copyNum = -1;
char defaultReplicaResource[MAX_TOKEN], defaultReplicaPath[MAX_TOKEN];
/*
char resourcename[MAX_TOKEN];
char pathname[MAX_TOKEN];
*/
char newresourcename[MAX_TOKEN];
char newpathname[MAX_TOKEN];

int usage (char *prog);
int
copyMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
srbPathName targNameArray[], int flagval, char *containerName);

int
main(int argc, char **argv)
{
    int c;
    char containerName[MAX_TOKEN];

    int flagval = 0;
    srbPathName nameArray[MAX_TOKEN], targNameArray[MAX_TOKEN];
    char contName[MAX_TOKEN], contCollection[MAX_TOKEN], inpCont[MAX_TOKEN];
    int nArgv;
    srbConn *conn;
    int status;
    char *userName, *domainName;
    int SFlag = 0;              /* -S option */
    int PFlag = 0;              /* -P option */
    char *mcatName;
#ifdef FED_MCAT
    char myMcatName[MAX_TOKEN];
#endif


    strcpy(inCondition , "");
    if (argc < 3) {
	usage (argv[0]);
	exit(1);
    }

    while ((c=getopt(argc, argv,"n:S:P:c:fpravb")) != EOF) {
	switch (c) {
	    case 'n':
		copyNum = atoi(optarg);
		break;
/*
	    case 's':
		sFlag = 1;
		strcpy (resourcename, optarg);
		break;
	    case 'p':
		pFlag = 1;
		strcpy (pathname, optarg);
		break;
*/
            case 's':
                flagval |= s_FLAG;      /* single thread */
                break;
	    case 'S':
		SFlag = 1;
	        strcpy (newresourcename, optarg);
		break;
	    case 'P':
		PFlag = 1;
		strcpy (newpathname, optarg);
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
	    case 'a':
		flagval |= a_FLAG;	/* overwrite all replica */
		break;
            case 'c':
                flagval |= c_FLAG;      /* user container */
                strcpy (inpCont, optarg);
                break;
	    case 'v':
		flagval |= v_FLAG;	/* verbose */
		break;
            case 'b':
                flagval |= b_FLAG;      /* bulk copy */
                break;
	    default:
		usage (argv[0]);
		exit (1);
	}
    }

    if (argc - optind < 2) {	/* must have at least 2 input */
        usage (argv[0]);
        exit (1);
    }


    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}
    
    /* Process the input */

/*
    if (sFlag == 0) 
	strcpy (resourcename, "");

    if (pFlag == 0)
	strcpy (pathname, "");
*/

    if (SFlag == 0)
	strcpy (newresourcename, "");

    if (PFlag == 0)
	strcpy (newpathname, "");

    /* Set up the source and target arrays */

    targNameArray[0].inpArgv = argv[argc-1];

    /* Convert the target name to SQL like */
    if (convNameArrayToSQL (targNameArray, 1) < 0)
      {exit(5);}


    for (i = optind; i < argc - 1; i++) {
        nameArray[i - optind].inpArgv = argv[i];
    }
    nArgv = argc - optind - 1;		/* nArgv is the number of in file */

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Scp: Connection to srbMaster failed.\n");
        fprintf(stderr,"Scp: %s",clErrorMessage(conn));
        clFinish(conn); exit(3);
    }

    if (flagval & c_FLAG) {         /* user container */
        if (conn->clientUser == NULL) {
            userName = conn->proxyUser->userName;
            domainName = conn->proxyUser->domainName;
        } else {
            userName = conn->clientUser->userName;
            domainName = conn->clientUser->domainName;
        }
#ifdef FED_MCAT
        if (inpCont[0] != '/') {    /* full path */
            char *thisMcat = NULL;

            thisMcat = getmcatNamefromHint (targNameArray[0].outArgv);
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
#else
        mcatName = NULL;
#endif
        parseContainerName (inpCont, contName, contCollection,
         userName, domainName, mcatName);
        sprintf (containerName, "%s/%s", contCollection, contName);
    } else {
        containerName[0] = '\0';
    }

    /* Convert the source name to SQL like */
    if (convNameArrayToSQL (nameArray, nArgv) < 0)
      {clFinish(conn); exit(4);}

    /* Convert the target name to SQL like */
    if (convNameArrayToSQL (targNameArray, 1) < 0)
      {clFinish(conn); exit(5);}
    linkFlag = 0;
   i =  copyMain (conn, MDAS_CATALOG, nArgv, nameArray, targNameArray, flagval,
     containerName);

    if (i < 0) {
      fprintf(stderr,"Scp: Error Status = %i\n", i);
      srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);

      clFinish(conn);
    } else if (RStatus < 0) {
      clFinish(conn);
      exit(1);
    } else {
      clFinish(conn);
      exit(0);
    }

}

int 
copyMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[], 
srbPathName targNameArray[], int flagval, char *containerName)
{
    int i;
    int status, status1;
    mdasC_sql_result_struct targDataResult, targCollResult;
    mdasC_sql_result_struct *dataResult, *collResult;
    int targType;
    int dataCnt = 0;
    int collCnt = 0;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN];
    char *tmpStr, *tmpStr1;
    char tmpinCondition[MAX_OBJ_NAME_LENGTH];
    
    strcpy(tmpinCondition,inCondition);
    strcpy(inCondition,"");
    targType = expandAndChkName (conn, MDAS_CATALOG, &targNameArray[0], 0, 
      &targDataResult, &targCollResult);
    strcpy(inCondition,tmpinCondition);
    unmake_like_str (targNameArray[0].outArgv, targNameArray[0].outArgv);
    if (targDataResult.row_count + targCollResult.row_count > 1) {
	fprintf (stderr, "The target spcification is not unique\n");
	return (CLI_ERR_COLLNAME);
    }
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
	unmake_like_str (nameArray[i].outArgv, nameArray[i].outArgv);
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
	fprintf (stderr, "Target %s must be a collection\n", 
	  targNameArray[0].inpArgv);
	return (CLI_ERR_COLLNAME);
    }

    if (collCnt > 0 && (flagval & R_FLAG) == 0 && (flagval & b_FLAG) == 0) {
        fprintf (stderr, "Must use the -r or -b flags to copy a collection\n");
	return (CLI_ERR_COLLNAME);
    }
    if (targType == -1) {	/* Target does not exist */
	if (dataCnt + collCnt > 1) {
            fprintf (stderr, "Target collection %s not found\n",
              targNameArray[0].inpArgv);
            return (CLI_ERR_COLLNAME);
	}
    }

    /* set the target resource */

    if (strlen(newresourcename) == 0) {
        get_user_preference(newresourcename, "DRSRC");
	/* sprintf(newpathname, ""); */
    }

    /* Now start copying */

    for (i = 0; i < nArgv; i++) {
      do {
	if (dataResult[i].row_count > 0) {	/* source is data */
	    if (targType == -1) {	/* A normal new data-data copy */
    		clSplitbychar (targNameArray[0].outArgv, targPar, targCh, '/');
	    	status = dataToDataCopy (conn, catType, flagval, targType, 
		  &dataResult[i], copyNum, targPar, targCh, newresourcename,
		   newpathname, containerName);
	    } else if (targType >= DATANAME_T) {
		tmpStr = (char *) getFromResultStruct (&targDataResult,
      		  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
		tmpStr1 = (char *) getFromResultStruct (&targDataResult,
      		  dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
                status = dataToDataCopy (conn, catType, flagval, targType,
                  &dataResult[i], copyNum, tmpStr, tmpStr1, newresourcename,
		   newpathname, containerName);
	    } else if (targType == COLLECTION_T || targType == SPEC_COLL_T) {
		tmpStr = (char *) getFromResultStruct (&targCollResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
		status = dataToCollCopy (conn, catType, flagval, 
		 &dataResult[i], copyNum, tmpStr, newresourcename, 
		  newpathname, containerName);
	    }
	} 

	if (collResult[i].row_count > 0) {	/* source is coll */
            if (collCnt == 1 && targType == -1) {   
		int inheritCont;

                if (containerName != NULL && strlen (containerName) > 0) {
                    inheritCont = 0;
                } else {
                    inheritCont = 1;
                }

		/* the source is a single coll */
            	clSplitbychar (targNameArray[0].outArgv, temp, temp1, '/');
            	status = myCreateCollect(conn, catType, temp, temp1, 
		 inheritCont);
                if (status < 0) {
                    fprintf(stderr,
		     "Unable to mk the target coll %s, status = %d\n",
                      targNameArray[0].inpArgv, status);
                    return (CLI_ERR_COLLNAME);
            	}
                /* mark it as COLLECTION_T */
                targNameArray[0].type = COLLECTION_T;
                targType = COLLECTION_T;
                tmpStr = (char *) getFromResultStruct (&collResult[i],
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
		status = collToCollCopyCont (conn, catType, flagval, tmpStr, 
		 targNameArray[0].outArgv, newresourcename, containerName);
	    } else {
		tmpStr = (char *) getFromResultStruct (&targCollResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
		status = collToCollCopy (conn, catType, flagval, 
		  nameArray[i].type, &collResult[i], tmpStr, newresourcename,
		   containerName);
	    } 
	}

	if (status < 0 && status != MCAT_INQUIRE_ERROR) {
	    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    break;
	}
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
    }

    if (status < 0 && status != MCAT_INQUIRE_ERROR) {
	return status;
    } else {
        return (0);
    }
}


int usage (char *prog)
{
  fprintf(stderr,
          "Usage: %s [-n replicaNum] [-c container] [-S destResourceName] [-P destPathname] [-fpsravb] origObj destObj\n",prog);
  return(0);
}
