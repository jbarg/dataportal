/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* Sphymove.c 
 */

#include "scommands.h"
#ifndef _WIN32
#include <unistd.h>
#endif
#include <dirent.h>

extern char *optarg;
extern int optind, opterr, optopt;

extern char srbAuth[];
extern char srbHost[];
extern char mdasCollectionName[];
extern char inCondition[];

extern int iGlbRetriesMax;
extern int RStatus;
extern NumThreads;

int
dataToDataMv (srbConn *conn, int catType, mdasC_sql_result_struct *dataResult,
char *destResource, char *newpathname, int copyNum, char *containerName,
int  flagval, char *srcResource);

int
pmvMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
int flagval, char *srcResource, char *destResource, char *newpathname,
char *containerName, int copyNum);

int
moveColl (srbConn *conn, int catType, mdasC_sql_result_struct *collResult,
char *srcResource, char *destResource, char *containerName, int flagval);

char defaultReplicaResource[MAX_TOKEN], defaultReplicaPath[MAX_TOKEN];
char datatype[MAX_TOKEN];

void usage (char *prog);

int
main(int argc, char **argv)
{
    int  newargc;
    char newargv[MAX_ARG_EXP_CNT][MAX_TOKEN];
    char *origObj;
    char *destObj;
    int c;
    char *ch;
    srbPathName nameArray[MAX_TOKEN];
    char contName[MAX_TOKEN], contCollection[MAX_TOKEN], inpCont[MAX_TOKEN];
    int nArgv;
    srbConn *conn;
    int status;
    int flagval = 0;
    char *userName, *domainName;
    char containerName[MAX_TOKEN];
    char srcResource[MAX_TOKEN];
    char destResource[MAX_TOKEN];
    char newpathname[MAX_TOKEN];
    char *mcatName;
    char myMcatName[MAX_TOKEN];
    int copyNum = -1;

    int SFlag = 0;              /* -S option */
    int PFlag = 0;              /* -P option */
    int cFlag = 0;		/* -c option */
    int numArg, option_index = 0;

    containerName[0] = srcResource[0] = destResource[0] =
      newpathname[0] = myMcatName[0] = '\0';

    strcpy(inCondition , "");

    while ((c=getopt(argc, argv,"n:s:S:P:c:kKbrv")) != EOF) {
      switch (c) {
      case 'n':
        copyNum = atoi(optarg);
        break;
      case 'S':
	SFlag = 1;
	strcpy (destResource, optarg);
	break;
      case 'P':
	PFlag = 1;
	strcpy (newpathname, optarg);
	break;
      case 'b':
	flagval |= b_FLAG;
	break;
      case 'r':
	flagval |= R_FLAG;
	break;
      case 's':
	flagval |= s_FLAG;      /* source resource */
	strcpy (srcResource, optarg);
	break;
      case 'c':
	flagval |= c_FLAG;      /* user container */
	strcpy (inpCont, optarg);
	break;
      case 'v':                   /* verbose flag */
	flagval |= v_FLAG;
	break;
      case 'k':                   /* register check sum flag */
	flagval |= k_FLAG; 
	break;
      case 'K':                   /* verify and register check sum flag */
	flagval |= K_FLAG; 
	break;
      default:
	usage (argv[0]);
	exit (1);
      }
    }

    numArg = argc - optind;
    if (numArg <= 0) {	/* must have at least 1 input */
        usage (argv[0]);
        exit (1);
    }

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}
    
    /* Process the input */

    if (SFlag == 0)
	strcpy (destResource, "");

    if (PFlag == 0)
	strcpy (newpathname, "");

    /* Set up the source arrays */

    for (i = optind; i < argc; i++) {
#ifdef _WIN32
        nameArray[i - optind].inpArgv = strdup(argv[i]);
        SrbNtPathForwardSlash(nameArray[i - optind].inpArgv);
#else
        nameArray[i - optind].inpArgv = argv[i];
#endif
    }
    nArgv = argc - optind;		/* nArgv is the number of in file */

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sphymove: Connection to srbMaster failed.\n");
        fprintf(stderr,"Sphymove: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    if (convNameArrayToSQL (nameArray, nArgv) < 0)
        {clFinish(conn); exit(3);}

    if (flagval & c_FLAG) {         /* user container */
    	if (conn->clientUser == NULL) {
            userName = conn->proxyUser->userName;
            domainName = conn->proxyUser->domainName;
    	} else {
            userName = conn->clientUser->userName;
            domainName = conn->clientUser->domainName;
    	}
#ifdef FED_MCAT
        if ((flagval & z_FLAG) == 0) {
            if ((flagval & l_FLAG) == 0) {
                char *thisMcat = NULL;

                thisMcat = getmcatNamefromHint (nameArray[0].outArgv);
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
        parseContainerName (inpCont, contName, contCollection,
         userName, domainName, mcatName);
        sprintf (containerName, "%s/%s", contCollection, contName);
    } else {
	containerName[0] = '\0';
    }

    /* set the target resource */

    if (strlen(destResource) == 0 && (flagval & c_FLAG) == 0) {
        get_user_preference(destResource, "DRSRC");
    }

    status = pmvMain (conn, MDAS_CATALOG, nArgv, nameArray, flagval, 
     srcResource, destResource, newpathname, containerName, copyNum);

    clFinish(conn);
    if (status >= 0 && RStatus >= 0)
      exit(0);
    else 
      exit(1);
}

int 
pmvMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[], 
int flagval, char *srcResource, char *destResource, char *newpathname,
char *containerName, int copyNum)
{
    int i;
    int status=(-1), status1=0;
    mdasC_sql_result_struct *dataResult, *collResult;
    int targType;
    int dataCnt = 0;
    int collCnt = 0;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    char srcPar[MAX_TOKEN], srcCh[MAX_TOKEN];
    char *tmpStr, *tmpStr1;
    int myStatus = 0;

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
	if (collCnt > 0 && (flagval & b_FLAG) == 0 &&
	  (flagval & R_FLAG) == 0) {
	    fprintf (stderr, "Source %s is a collection. use -b or -r option\n",
	    nameArray[i].inpArgv);
            return (CLI_ERR_COLLNAME);
        }
	if (dataCnt > 0 && (flagval & b_FLAG)) {
            fprintf (stderr, 
	     "Source %s is a data file.-b option can only handle collection\n",
             nameArray[i].inpArgv);
            return (CLI_ERR_COLLNAME);
        }
    }

    /* Do some sanity checks */

    if (dataCnt + collCnt == 0) {
        fprintf (stderr, "Input Source %s not found\n",
          nameArray[0].inpArgv);
        return (CLI_ERR_COLLNAME);
    }

    /* Now start moving */

    status = 0;
    for (i = 0; i < nArgv; i++) {
        if (dataResult[i].row_count > 0) {      /* source is data */
            /* A normal data-file move */
            status = dataToDataMv (conn, catType, &dataResult[i], destResource,
	     newpathname, copyNum, containerName, flagval, srcResource);
	    if (status < 0) 
		myStatus = status;
	}
        if (collResult[i].row_count > 0) {      /* source is coll */
            status = moveColl (conn, catType, &collResult[i], 
	     srcResource, destResource, containerName, flagval);
	    if (status < 0) 
		myStatus = status;
	}
    }

    return (myStatus);
}

int
dataToDataMv (srbConn *conn, int catType, mdasC_sql_result_struct *dataResult, 
char *destResource, char *newpathname, int copyNum, char *containerName,
int flagval, char *srcResource)
{
    char tmpStr[MAX_TOKEN];
    char *srcPar, *srcCh;
    int status;
    int myStatus = 0;

    srcPar = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    srcCh = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    for (i = 0; i < dataResult->row_count;i++) {
        if (copyNum >=  0) {
            sprintf(tmpStr, "&COPY=%i",copyNum);
            strcat(srcCh, tmpStr);
        }
        if (flagval & c_FLAG) {         /* user container */
            /* sprintf (tmpStr, "&CONTAINER=%-s", containerName); */
	    /* a hack. . use the destResource field for container */
	    status = srbObjMove(conn, catType, srcCh, srcPar,
                        NULL, destResource, newpathname, containerName);
        } else {
            status = srbObjMove(conn, catType, srcCh, srcPar,
                        NULL, destResource, newpathname, NULL);
	}
        if (status < 0) {   /* error */
            fprintf(stderr, "dataToDataMv: srbObjMove error for %s/%s :%i\n",
              srcPar, srcCh, status);
            srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    RStatus = myStatus = status;
        }
        srcPar += MAX_DATA_SIZE;
        srcCh += MAX_DATA_SIZE;
    }
    return (myStatus);
}

int
moveColl (srbConn *conn, int catType, mdasC_sql_result_struct *collResult, 
char *srcResource, char *destResource, char *containerName, int flagval)
{
    char tmpStr[MAX_TOKEN];
    char *myColl;
    int status;
    int myStatus = 0;
/*
mdasC_sql_result_struct *nonContResult;
char *targetDir="test/srcx";
int srcFd;
*/

    myColl = (char *) getFromResultStruct(collResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    for (i = 0; i < collResult->row_count;i++) {
	if (flagval & b_FLAG) {
            status = srbBulkMove (conn, catType, myColl, 0,
              srcResource, destResource, containerName);
	} else {
            status = srbObjMoveColl (conn, catType, myColl, 0,
              srcResource, destResource, containerName);
	}
        if (status < 0) {   /* error */
	    if (status == MCAT_INQUIRE_ERROR) {
		status = 0;
		/* not a real error */
	    } else if (status == DATA_NOT_IN_CAT && containerName != NULL &&
	      strlen (containerName) > 0) {
		fprintf (stderr,
		 "moveColl: srbBulkMove failed. Container %s may not exist\n",
		  containerName);
	        RStatus = myStatus = status;
	    } else {
                fprintf(stderr,"moveColl: srbBulkMove error for %s :%i\n",
                  myColl, status);
                srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	        RStatus = myStatus = status;
	    }
        }
        myColl += MAX_DATA_SIZE;
    }
    return (myStatus);
}

void
usage (char *prog)
{
        fprintf(stderr,
          "Usage: %s [-n replNum} [-c container] [-S targetResource] [-P destPathname]  srbFile ...\n",prog);
        fprintf(stderr,
          "Usage: %s -b|-r [-c container] [-s sourceResource] [-S targetResource] srbCollection ...\n",prog);
}
