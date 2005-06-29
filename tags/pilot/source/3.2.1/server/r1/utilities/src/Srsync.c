/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*
 * Srsync.c
 */

#include "scommands.h"
#include <dirent.h>

#define LOCAL_TO_SRB    0
#define SRB_TO_LOCAL    1
#define SRB_TO_SRB      2

extern char srbAuth[];
extern char srbHost[];
extern char mdasCollectionName[];
extern char inCondition[];
 
extern int RStatus;

int
rsyncLocalToSrbMain (srbConn *conn, int catType, int nArgv,
srbPathName nameArray[], srbPathName targNameArray[], int flagval,
int copyNum, char *tmpDir, char *resourceName);

#ifdef foo
int
mydataToDataCopy1 (srbConn *conn, int catType, int flagval, int targType,
char *srcCh, char *srcPar, srb_long_t datasize, int srcCopyNum, char *targColl,
char *targObj, char *newResource, char *newPath, char *containerName);
#endif

int
rsyncSrbToSrbMain (srbConn *conn, int catType, int nArgv,
srbPathName nameArray[], srbPathName targNameArray[], int flagval,
int copyNum, char *tmpDir, char *resourceName);

extern int    linkFlag;


void usage();

int
main(int argc, char **argv)
{

    int  newargc;
    char newargv[MAX_ARG_EXP_CNT][MAX_TOKEN];
    int c;
    int flagval = 0;
    srbPathName srcNameArray[MAX_TOKEN], targNameArray[MAX_TOKEN];
    int nArgv;
    char collname[MAX_TOKEN];
    char dataname[MAX_TOKEN];
    char contName[MAX_TOKEN], contCollection[MAX_TOKEN], inpCont[MAX_TOKEN];
    char tmpStr1[MAX_TOKEN];
    char *userName, *domainName;
    char tmpDir[MAX_TOKEN];
    char resourceName[MAX_TOKEN];
    srbConn *conn;
    FILE *fd;
    int status;
    int copyNum =-1;
    int i, ii;
    int numArg;
    int option_index = 0;
    int myMode = -1;

    int SFlag = 0;              /* -S option */

    tmpDir[0] = '\0';

    while ((c=getopt(argc, argv,"samMlt:rS:v")) != EOF) {
      switch (c) {
      case 'f':
	flagval |= f_FLAG;
	break;
      case 'l':
	flagval |= l_FLAG;
	break;
      case 'r':
	flagval |= R_FLAG;
	break;
      case 'a':
	flagval |= a_FLAG; 
        break;
      case 'm':
	flagval |= m_FLAG; 
        break;
      case 's':
	flagval |= S_FLAG; 
        break;
      case 'M':
        flagval |= M_FLAG;      /* client initiated multi thread */
        flagval |= m_FLAG;      /* multi thread */
        break;
      case 'v':
	flagval |= v_FLAG; 
        break;
      case 't':
        strcpy (tmpDir, optarg);
        break;
      case 'S':
        SFlag = 1;
        strcpy (resourceName, optarg);
        break;
      default:
	usage (argv[0]);
	exit (1);
      }
    }
    
    nArgv = argc - optind;

    if (nArgv < 2) {      /* must have at least 1 input */
        usage (argv[0]);
        exit (1);
    } else {
	if (strcmp (argv[argc-1], "s:") == 0) {
	    targNameArray[0].inpArgv = ".";
	    myMode = LOCAL_TO_SRB;
	} else if (strncmp (argv[argc-1], "s:", 2) == 0) {
	    targNameArray[0].inpArgv = argv[argc-1] + 2;
	    myMode = LOCAL_TO_SRB;
	} else {
            targNameArray[0].inpArgv = argv[argc-1];
	    myMode = SRB_TO_LOCAL;
	}
    }

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}

    if (SFlag == 0)
        strcpy (resourceName, "");

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Srsync: Connection to srbMaster failed.\n");
        fprintf(stderr,"Srsync: %s\n",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(3);
    }

    /* set the target resource */

    if (strlen(resourceName) == 0) {
        get_user_preference(resourceName, "DRSRC");
    }

    for (i = optind; i < argc - 1; i++) {
        if (strcmp (argv[i], "s:") == 0) {
	    if (myMode == LOCAL_TO_SRB) {
		if (i == optind) {
		    myMode = SRB_TO_SRB;
		} else {
		    fprintf (stderr, 
		     "Input error, Inconsistent input src\n");
		    exit (1);
		}
	    }
	    srcNameArray[i - optind].inpArgv = ".";
        } else if (strncmp (argv[i], "s:", 2) == 0) {
            if (myMode == LOCAL_TO_SRB) {
                if (i == optind) {
                    myMode = SRB_TO_SRB;
                } else {
                    fprintf (stderr,
                     "Input error, Inconsistent input src\n");
                    exit (1);
                }
            }
#ifdef _WIN32
            srcNameArray[i - optind].inpArgv = strdup(argv[i] + 2);
            SrbNtPathForwardSlash(srcNameArray[i - optind].inpArgv);
#else
            srcNameArray[i - optind].inpArgv = argv[i] + 2;
#endif
        } else {
	    if (myMode != LOCAL_TO_SRB) {
                fprintf (stderr,
                 "Input error, Both source and target are local\n");
                exit (1);
            }
#ifdef _WIN32
            srcNameArray[i - optind].inpArgv = strdup(argv[i]);
            SrbNtPathForwardSlash(nameArray[i - optind].inpArgv);
#else
            srcNameArray[i - optind].inpArgv = argv[i];
#endif
        }
    }
    nArgv = argc - optind - 1;          /* nArgv is the number of in file */

    flagval = flagval | Y_FLAG;

    if (myMode == LOCAL_TO_SRB) {
        /* Convert the target name to SQL like */
        if (convNameArrayToSQL (targNameArray, 1) < 0) {
	    clFinish(conn); 
	    exit(4);
	}
        ii= rsyncLocalToSrbMain (conn, MDAS_CATALOG, nArgv, 
	  srcNameArray, targNameArray, flagval, -1, tmpDir,
	  resourceName);
    } else if (myMode == SRB_TO_LOCAL) {
        /* Convert the source name to SQL like */
        if (convNameArrayToSQL (srcNameArray, nArgv) < 0) {
	    clFinish(conn); 
	    exit(3);
	}
	ii=  rsyncSrbToLocalMain (conn, MDAS_CATALOG, nArgv, 
	 srcNameArray, targNameArray, flagval);
    } else {	/* SRB_TO_SRB */
        /* Convert the source name to SQL like */
        if (convNameArrayToSQL (srcNameArray, nArgv) < 0) {
            clFinish(conn); 
            exit(3);
        }
        if (convNameArrayToSQL (targNameArray, 1) < 0) {
            clFinish(conn); 
            exit(4);
        }
        ii= rsyncSrbToSrbMain (conn, MDAS_CATALOG, nArgv,
          srcNameArray, targNameArray, flagval, -1, tmpDir,
          resourceName);
    }

    clFinish(conn);
    if (ii == 0 && RStatus == 0)
      exit(0);
    else
      exit(1);
}


int
rsyncLocalToSrbMain (srbConn *conn, int catType, int nArgv, 
srbPathName nameArray[], srbPathName targNameArray[], int flagval, 
int copyNum, char *tmpDir, char *resourceName)
{
    int i;
    int status=(-1), status1=0;
    mdasC_sql_result_struct *dataResult, *collResult;
    mdasC_sql_result_struct targDataResult, targCollResult;
    int targType;
    int dataCnt = 0;
    int collCnt = 0;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    char srcPar[MAX_TOKEN], srcCh[MAX_TOKEN];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN];
    char *tmpStr, *tmpStr1;
    char currentColl[MAX_TOKEN];
    struct srbFileOut mySrcFileOut, myTargFileOut;
    int numCommonFile = 0;

    srandom ((unsigned int) (time(0) + (getpid() << 4)));

    /* set flagval to 0 first for non-recursive check */
    targType = expandAndChkName (conn, MDAS_CATALOG, &targNameArray[0], Y_FLAG,
      &targDataResult, &targCollResult);

    unmake_like_str (targNameArray[0].outArgv, targNameArray[0].outArgv);
    if (targDataResult.row_count + targCollResult.row_count > 1) {
        fprintf (stderr, "The target specification is not unique\n");
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
        status = chkFileName (&nameArray[i]);
        if (status < 0) {
            fprintf (stderr, "Source %s not found\n", nameArray[i].inpArgv);
            return (CLI_ERR_COLLNAME);
        }

        if (nameArray[i].type >= DATANAME_T)
            dataCnt ++;
        else
            collCnt ++;
    }


    /* Do some sanity checks */

    if (dataCnt + collCnt == 0) {
        fprintf (stderr, "Source %s not found\n",
          targNameArray[0].inpArgv);
        return (CLI_ERR_COLLNAME);
    }

    if (collCnt > 0 && (flagval & R_FLAG) == 0) {
        fprintf (stderr, "Must use the -r flag to rsync a directory\n");
        return (CLI_ERR_COLLNAME);
    }

    if ((dataCnt + collCnt > 1 || collCnt > 0) && targType >= DATANAME_T) {
        fprintf (stderr, "Target %s must be a collection\n",
          targNameArray[0].inpArgv);
        return (CLI_ERR_COLLNAME);
    }

    if (targType == -1 && collCnt >= 1) {       /* Target does not exist */
        clSplitbychar(targNameArray[0].outArgv, temp1, temp,'/');
        status = myCreateCollect(conn, catType, temp1, temp, 1);
        if (status < 0 && status != DATA_SUBCOLLECTION_NOT_UNIQUE) {
            fprintf(stderr,
             "Unable to mk the target coll %s/%s, status = %d\n",
              temp1, temp, status);
            srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return (CLI_ERR_COLLNAME);
        }
	targType = COLLECTION_T;
    }

    status = 0;
    for (i = 0; i < nArgv; i++) {
        if (nameArray[i].type >= DATANAME_T) {  /* source is data */
	    memset (&mySrcFileOut, 0, sizeof (mySrcFileOut));
	    mySrcFileOut.size = nameArray[i].size;
            status = clFileSum (nameArray[i].inpArgv, &mySrcFileOut.chksum);
            if (status != 0) {
                fprintf (stderr,
                 "rsyncLocalToSrbMain: clFileSum error for %s, errno = %d\n",
                nameArray[i].inpArgv, errno);
                return (-1);
	    }

            if (targType == -1) {       /* A normal new data-data copy */
                clSplitbychar (targNameArray[0].outArgv, targPar, targCh, '/');
                /* added April 8, 2003 RAJA to take care of wildcards in targ 
		 * names ***/
                strcpy(targNameArray[0].outArgv, targPar);
                expandAndChkName (conn, MDAS_CATALOG, &targNameArray[0], 0,
                  &targDataResult, &targCollResult);
               if (targCollResult.row_count != 1) {
                 fprintf (stderr, "The target spcification is not unique\n");
                 return (CLI_ERR_COLLNAME);
               }

		strcpy (mySrcFileOut.fileName, targCh);
                tmpStr = (char *) getFromResultStruct (&targCollResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
                status = fileToDataRsync (conn, catType, flagval, 
                  &mySrcFileOut, NULL, nameArray[i].inpArgv,
		  tmpStr, tmpStr, resourceName, copyNum);
            } else if (targType >= DATANAME_T) {
		char *mySize, *chksum;
		memset (&myTargFileOut, 0, sizeof (myTargFileOut));
                tmpStr = (char *) getFromResultStruct (&targDataResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
                tmpStr1 = (char *) getFromResultStruct (&targDataResult,
                  dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
                mySize = (char *) getFromResultStruct (&targDataResult,
                  dcs_tname[SIZE], dcs_aname[SIZE]);
                chksum = (char *) getFromResultStruct (&targDataResult,
                  dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
		strcpy (myTargFileOut.fileName, tmpStr1);
		strcpy (mySrcFileOut.fileName, tmpStr1);
		strcpy (myTargFileOut.collection, tmpStr);
		myTargFileOut.size = strtoll (mySize, 0, 0);
		myTargFileOut.chksum = atoi (chksum);
                status = fileToDataRsync (conn, catType, flagval, 
                  &mySrcFileOut, &myTargFileOut, nameArray[i].inpArgv,
                  tmpStr, tmpStr, resourceName, copyNum);
            } else if (targType == COLLECTION_T || targType == SPEC_COLL_T) {
                tmpStr = (char *) getFromResultStruct (&targCollResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
		if (tmpStr == NULL) {
                    tmpStr = targNameArray[0].outArgv;
                }
                status = fileToCollRsync (conn, catType, flagval,
                 &mySrcFileOut, tmpStr, nameArray[i].inpArgv, resourceName, 
		  copyNum);
            }
        } else {        /* source is coll */
	    char tmpTarg[MAX_TOKEN];
	
	    if (targType == -1) {
		clSplitbychar(targNameArray[0].outArgv, temp, temp1,'/');
                status = myCreateCollect(conn, catType, temp, temp1, 1);
                if (status < 0 && status != DATA_SUBCOLLECTION_NOT_UNIQUE) {
                    fprintf(stderr,
                     "Unable to mk the target coll %s/%s, status = %d\n",
                      temp, temp1, status);
                    srb_perror (2, status, "",SRB_RCMD_ACTION|SRB_LONG_MSG);
                    return (CLI_ERR_COLLNAME);
                }
		tmpStr = targNameArray[0].outArgv;
	    } else {
                tmpStr = (char *) getFromResultStruct (&targCollResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
	    }
	    if (collCnt > 1) {	/* more than one src coll */
                clSplitbychar(nameArray[i].outArgv, temp, temp1,'/');
                sprintf (tmpTarg, "%s/%s", tmpStr, temp1);
		if (checkCollection (conn, MDAS_CATALOG, tmpTarg) != 0) {
        	    status = myCreateCollect(conn, catType, tmpStr, temp1, 1);
        	    if (status < 0 && status != DATA_SUBCOLLECTION_NOT_UNIQUE) {
            		fprintf(stderr,
             		 "Unable to mk the target coll %s/%s, status = %d\n",
              		  tmpStr, temp1, status);
            		srb_perror (2, status, "",SRB_RCMD_ACTION|SRB_LONG_MSG);
            		return (CLI_ERR_COLLNAME);
        	    }
		}
		tmpStr = tmpTarg;
	    } else if (tmpStr == NULL) {
		tmpStr = targNameArray[0].outArgv;
	    }
            status = dirToCollRsync (conn, catType, flagval,
              nameArray[i].outArgv, tmpStr, resourceName,"", tmpDir);
        }
        if (status == MCAT_INQUIRE_ERROR)
            status = 0;
        else if (status < 0)
            break;
    }

    return (status);
}

int
rsyncSrbToLocalMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
srbPathName targNameArray[], int flagval)
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

    for (i = 0; i < nArgv; i++) {
        /* set flagval to 0 first for non-recursive check */
        status = expandAndChkName (conn, MDAS_CATALOG, &nameArray[i], Y_FLAG,
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
	if (collCnt > 0) {
	    status = mkdir (targNameArray[0].outArgv, 0755);
	    if (status < 0) {
		fprintf (stderr, "Unable to mkdir %s. errno = %d\n",
                 targNameArray[0].inpArgv, errno);
                return (CLI_ERR_COLLNAME);
	    } else {
	        targType = COLLECTION_T;
	    }
	}
    }

    status = 0;
    for (i = 0; i < nArgv; i++) {
      do {
        if (dataResult[i].row_count > 0) {      /* source is data */
            if (targType == -1 || targType >= DATANAME_T) {
                /* A normal data-file copy */
                status = dataToFileRsync (conn, catType, flagval, targType,
                  &dataResult[i], targNameArray[0].outArgv);
            } else if (targType == COLLECTION_T || targType == SPEC_COLL_T) {
                status = dataToDirRsync (conn, catType, flagval,
                 &dataResult[i], targNameArray[0].outArgv);
            }
        }

        if (collResult[i].row_count > 0) {      /* source is coll */
            status = collToDirRsync (conn, catType, flagval,
             nameArray[i].type, collCnt, &collResult[i],
             targNameArray[0].outArgv);
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

int
rsyncSrbToSrbMain (srbConn *conn, int catType, int nArgv,
srbPathName nameArray[], srbPathName targNameArray[], int flagval,
int copyNum, char *tmpDir, char *resourceName)
{
    int i;
    int status=(-1), status1=0;
    mdasC_sql_result_struct *dataResult, *collResult;
    mdasC_sql_result_struct targDataResult, targCollResult;
    int targType;
    int dataCnt = 0;
    int collCnt = 0;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    char srcPar[MAX_TOKEN], srcCh[MAX_TOKEN];
    char *tmpStr, *tmpStr1;
    char currentColl[MAX_TOKEN];
    struct srbFileOut mySrcFileOut, myTargFileOut;
    int numCommonFile = 0;

    srandom ((unsigned int) (time(0) + (getpid() << 4)));

    /* set flagval to 0 first for non-recursive check */
    targType = expandAndChkName (conn, MDAS_CATALOG, &targNameArray[0], Y_FLAG,
      &targDataResult, &targCollResult);

    unmake_like_str (targNameArray[0].outArgv, targNameArray[0].outArgv);
    if (targDataResult.row_count + targCollResult.row_count > 1) {
        fprintf (stderr, "The target specification is not unique\n");
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
        status = expandAndChkName (conn, MDAS_CATALOG, &nameArray[i], Y_FLAG,
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

    if (targType == -1 && collCnt >= 1) {       /* Target does not exist */
        clSplitbychar(targNameArray[0].outArgv, temp1, temp,'/');
        status = myCreateCollect(conn, catType, temp1, temp, 1);
        if (status < 0 && status != DATA_SUBCOLLECTION_NOT_UNIQUE) {
            fprintf(stderr,
             "Unable to mk the target coll %s/%s, status = %d\n",
              temp1, temp, status);
            srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return (CLI_ERR_COLLNAME);
        }
        targType = COLLECTION_T;
    }

    status = 0;
    for (i = 0; i < nArgv; i++) {
        do {
            if (dataResult[i].row_count > 0) {      /* source is data */
                if (targType == -1 || targType >= DATANAME_T) {
                    /* A normal data-file copy */
                    status = dataToDataRsync (conn, catType, flagval, targType,
                      &dataResult[i], targNameArray[0].outArgv, copyNum, 
		       resourceName);
                } else if (targType == COLLECTION_T || 
		 targType == SPEC_COLL_T) {
                    status = dataToCollRsync (conn, catType, flagval,
                     &dataResult[i], targNameArray[0].outArgv,
		      resourceName, copyNum);
		}
            }
            if (collResult[i].row_count > 0) {      /* source is coll */
                if (targType == -1) {
                    clSplitbychar(targNameArray[0].outArgv, temp, temp1,'/');
                    status = myCreateCollect(conn, catType, temp, temp1, 1);
                    if (status < 0 && status != DATA_SUBCOLLECTION_NOT_UNIQUE) {
                        fprintf(stderr,
                         "Unable to mk the target coll %s/%s, status = %d\n",
                          temp, temp1, status);
                        srb_perror (2, status, "",SRB_RCMD_ACTION|SRB_LONG_MSG);
                        return (CLI_ERR_COLLNAME);
                    }
		}

                status = collToCollRsync (conn, catType, flagval,
                 nameArray[i].type, collCnt, &collResult[i],
                 targNameArray[0].outArgv, resourceName,"", tmpDir, copyNum);
            }
#ifdef foo
            status = dirToCollRsync (conn, catType, flagval,
              nameArray[i].outArgv, tmpStr, resourceName,"", tmpDir);
#endif
            if (status == MCAT_INQUIRE_ERROR)
                status = 0;
            else if (status < 0)
                break;

            if (dataResult[i].continuation_index >= 0 &&
              dataResult[i].row_count > 0) {
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
  fprintf(stderr, "Usage: Srsync [-amMlrvs] [-S resource] [-t tmpInxDir] sourceFile/sourceDirectory [....] targetFile/targetCollection\n");
}

#ifdef foo
int
mydataToDataCopy1 (srbConn *conn, int catType, int flagval, int targType,
char *srcCh, char *srcPar, srb_long_t datasize, int srcCopyNum, char *targColl,
char *targObj, char *newResource, char *newPath, char *containerName)
{
    char temp[MAX_OBJ_NAME_LENGTH];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN], tmpTargObj[MAX_TOKEN];
    char *tmpStr;
#ifdef _WIN32
    SYSTEMTIME startTime, endTime;
#else
    struct timeval diffTime, startTime, endTime;
#endif
    char *datatype;
    float fSec;
    int existFlag;
    int doCopyP;
    srb_long_t status;

    if (flagval & v_FLAG)
#ifdef _WIN32
        GetSystemTime(&startTime);
#else
        (void) gettimeofday(&startTime, (struct timezone *)0);
#endif
    if (strcmp (targColl, srcPar) == 0 && strcmp (targObj, srcCh) == 0) {
        if (linkFlag)
            printf ("dataToDataCopy error: trying to link to itself\n");
        else
            printf ("dataToDataCopy error: trying to copy to itself\n");
        return (CLI_ERR_COLLNAME);
    }

#ifdef foo
    if (sizeStr == NULL)
        datasize = 1;
    else
        datasize = strtoll (sizeStr, 0, 0);
#endif

    if (srcCopyNum >=  0) {
        sprintf(temp, "%s&COPY=%i",srcCh, srcCopyNum);
    } else {
        strcpy (temp, srcCh);
    }
    if (strlen(inCondition) > 0) {
        strcat(temp,"&");
        strcat(temp,inCondition);
    }

    if (targType == -1) {
        existFlag = 0;
        doCopyP = 1;
    } else {
        existFlag = 1;
        doCopyP = 0;
    }

    if (flagval & c_FLAG && existFlag == 0) {         /* user container */
        sprintf (tmpTargObj,
         "%s&CONTAINER=%-s", targObj, containerName);
        doCopyP = 0;
    } else if (flagval & a_FLAG && existFlag == 0) {   /* COPIES=all */
        sprintf (tmpTargObj, "%s&COPIES=all", targObj);
    } else {
        strcpy (tmpTargObj, targObj);
    }

    if (newPath != NULL && strlen (newPath) > 0)
        doCopyP = 0;

    if (linkFlag) {
      status = srbModifyDataset(conn, MDAS_CATALOG,
                                temp, srcPar,"","",
                                tmpTargObj,targColl, D_INSERT_LINK);
    } else {
        if (doCopyP == 0 || (flagval & s_FLAG)) {
            status = srbTosrbCopyS (conn, catType, temp, srcPar, tmpTargObj,
             targColl, newResource, datatype, newPath, datasize, existFlag,
              flagval);
        } else {
            status = srbObjCopy (conn, temp, srcPar, tmpTargObj, targColl,
             newResource);
        }
    }

    if (status < 0) {
        srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        return (status);
    }

    if (flagval & v_FLAG && datasize > 0) {
#ifdef _WIN32
         GetSystemTime(&endTime);
         fSec = abs((float)startTime.wSecond - (float)endTime.wSecond) +
          abs((float)startTime.wMilliseconds - 
          (float)endTime.wMilliseconds/ 1000.0);
#else
         (void) gettimeofday(&endTime, (struct timezone *)0);
         (void)tvsub(&diffTime, &endTime, &startTime);
         fSec = (float)diffTime.tv_sec +
            ((float)diffTime.tv_usec / 1000000.);
#endif
          printf
           ("Fn:%s, sz=%5.4f MB, tm=%3.2f sec, rate=%3.2f MB/sec\n",
           srcCh, (float) datasize / 1000000., fSec,
            (float) datasize / 1000000. / fSec);
    }
    return (0);
}
#endif	/* foo */
