/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* Sput.c 
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
extern int NumThreads;
extern char FileGroup[];

extern char ContainerName[MAX_TOKEN]; /* XXXX used for bload.should take out */

extern int
bloadMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
srbPathName *targName, int flagval);
 
char defaultReplicaResource[MAX_TOKEN], defaultReplicaPath[MAX_TOKEN];
char datatype[MAX_TOKEN];
/*
char resourcename[MAX_TOKEN];
char pathname[MAX_TOKEN];
*/
char ResourceName[MAX_TOKEN];
char newpathname[MAX_TOKEN];

void usage (char *prog);
int
putMainStdIn(srbConn *conn, int catType, char *targetName, char *containerName);

int
main(int argc, char **argv)
{
    int c, ii;
    srbPathName nameArray[MAX_TOKEN], targNameArray[MAX_TOKEN];
    char contName[MAX_TOKEN], contCollection[MAX_TOKEN], inpCont[MAX_TOKEN];
    int nArgv;
    srbConn *conn;
    int status;
    int flagval = 0;
    char *userName, *domainName;
    char containerName[MAX_TOKEN];
    char *mcatName = NULL;
    char myMcatName[MAX_TOKEN];
    int copyNum = -1;

    int SFlag = 0;              /* -S option */
    int PFlag = 0;              /* -P option */
    int DFlag = 0;		/* -D option */
    int iFlag = 0;
    int numArg;

    strcpy(inCondition , "");
#ifdef foo
    if (argc < 3) {
	usage (argv[0]);
	exit(1);
    }
#endif

    while ((c=getopt(argc, argv,"N:n:D:S:P:c:R:lfpriavsmMkKVbgG:")) != EOF) {
      switch (c) {
      case 'n':
        copyNum = atoi(optarg);
        break;
      case 'N':
        NumThreads = atoi(optarg);
        break;
      case 'D':
	strcpy (datatype, optarg);
	DFlag = 1;
	break;
      case 'S':
	SFlag = 1;
	strcpy (ResourceName, optarg);
	break;
      case 'P':
	PFlag = 1;
	strcpy (newpathname, optarg);
	break;
      case 'b':
	flagval |= b_FLAG;
	break;
      case 'i':
	iFlag = 1;
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
	flagval |= a_FLAG;      /* overwrite all replica */
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
      case 'c':
	flagval |= c_FLAG;      /* user container */
	strcpy (inpCont, optarg);
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
      case 'k':                   /* register check sum flag */
	flagval |= k_FLAG; 
	break;
      case 'K':                   /* verify and register check sum flag */
	flagval |= K_FLAG; 
	break;
      case 'l':
         flagval |= l_FLAG;
         break;
      case 'g':                   /* verify and register check sum flag */
        flagval |= g_FLAG;
        FileGroup[0] = '\0';
        break;
      case 'G':                   /* verify and register check sum flag */
        flagval |= g_FLAG;
        strcpy (FileGroup, optarg);
        break;
      default:
	usage (argv[0]);
	exit (1);
      }
    }

/*
    if ((flagval & c_FLAG) && (flagval & b_FLAG)) {
	fprintf (stderr, "The -b and -c option cannot be used together.\n");
	fprintf (stderr, "Please use Sbload for bulk loading containers.\n");
	exit (1);
    }
*/

    if (PFlag > 0 && (flagval & b_FLAG)) {
        fprintf (stderr, "The -b and -P option cannot be used together.\n");
        exit (1);
    }

    numArg = argc - optind;
    if (!iFlag) {
	if (numArg <= 0) {	/* must have at least 1 input */
            usage (argv[0]);
            exit (1);
	} else if (numArg == 1) {
            /* Assume "." for target */
    	    targNameArray[0].inpArgv = ".";
	    argc++;
	} else {
	    targNameArray[0].inpArgv = argv[argc-1];
	}
    } else if (iFlag && numArg < 1) {	
	/* must have at least 1 input */
        usage (argv[0]);
        exit (1);
    } else {
        targNameArray[0].inpArgv = argv[argc-1];
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

    if (DFlag == 0)
	strcpy (datatype, "");

    if (SFlag == 0)
	strcpy (ResourceName, "");

    if (PFlag == 0)
	strcpy (newpathname, "");

    /* Set up the source arrays */

    for (i = optind; i < argc - 1; i++) {
#ifdef _WIN32
        nameArray[i - optind].inpArgv = strdup(argv[i]);
        SrbNtPathForwardSlash(nameArray[i - optind].inpArgv);
#else
        nameArray[i - optind].inpArgv = argv[i];
#endif
    }
    nArgv = argc - optind - 1;		/* nArgv is the number of in file */

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sput: Connection to srbMaster failed.\n");
        fprintf(stderr,"Sput: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    /* Convert the target name to SQL like */
    if (convNameArrayToSQL (targNameArray, 1) < 0)
      {clFinish(conn); exit(4);}

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
        } else {
            mcatName = myMcatName;
        }
#else
        mcatName = NULL;
#endif
        parseContainerName (inpCont, contName, contCollection,
         userName, domainName, mcatName);
        sprintf (containerName, "%s/%s", contCollection, contName);
	strcpy (ContainerName, containerName); /*XXXX used in bload.take out */ 
    } else {
	containerName[0] = '\0';
    }

    /* set the target resource */

    if (strlen(ResourceName) == 0) {
        get_user_preference(ResourceName, "DRSRC");
    }

    if ((flagval & b_FLAG) && (getenv ("MCAT_HOST") == NULL)) {
	char *myHost;
	srbConn *myConn;

        myHost = getHostByResource (conn, ResourceName, mcatName);
        if (myHost != NULL) {
            if (strncmp (srbHost, myHost, strlen (srbHost)) != 0) {
                /* different host */
                myConn = srbConnect (myHost, NULL, srbAuth,
                 NULL, NULL, NULL, NULL);
                if (clStatus(myConn) == CLI_CONNECTION_OK) {
                    clFinish (conn);
                    conn = myConn;
                    strcpy (srbHost, myHost);
                }
            }
        }
    }

    if (iFlag) {
      ii = putMainStdIn(conn, MDAS_CATALOG, argv[argc-1], containerName);
    } else if (flagval & b_FLAG) {
      ii = bloadMain (conn, MDAS_CATALOG, nArgv, nameArray, targNameArray,
       flagval);
    } else {
      ii = putMain (conn, MDAS_CATALOG, nArgv, nameArray, targNameArray, 
	flagval, containerName, copyNum);
    }

    clFinish(conn);
    if (ii >= 0 && RStatus >= 0)
      exit(0);
    else 
      exit(1);
}

int
putMainStdIn(srbConn *conn, int catType, char *targetName, char *containerName)
{
    int wbytes, nbytes, out_fd;
    char collName[MAX_TOKEN];
    char dataName[MAX_OBJ_NAME_LENGTH];
    FILE *infile_fd;
    char *buf;


    splitpath(targetName,collName,dataName);
    infile_fd = stdin;
    if (strlen(containerName) > 0 ) {         /* user container */
      strcat(dataName,"&CONTAINER=");
      strcat(dataName,containerName);
    }

    out_fd = srbObjCreate (conn, catType, dataName,
	  datatype, ResourceName, collName, newpathname, 0);
    if (out_fd < 0) {
      fprintf (stderr,
	       "Sput: Unable to create object %s/%s, status = %d\n",
	       collName,dataName, out_fd);
      srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      fclose(infile_fd);
      return (out_fd);
    }
    buf = malloc (BUFSIZE + 1);
    if (buf == NULL) {
      fprintf (stderr, "Sput: Unable to malloc buf\n");
      srbObjClose (conn, out_fd);
      fclose(infile_fd);
      return (-1);
    }

    while ((nbytes = fread(buf, 1, BUFSIZE,(FILE *)infile_fd ))>0) 
      {
	wbytes = srbObjWrite(conn, out_fd, buf, nbytes);
	if (wbytes != nbytes) {  
	  fprintf(stderr, "Sput: write error in srbObject %s/%s: %i\n",
		  collName,dataName,wbytes);
	  fprintf(stderr, "Sput: Sput: Exiting \n");
	  srbObjClose (conn, out_fd);
	  fclose(infile_fd);
          free(buf);
	  return -1;
	}
      }
    free(buf);
    srbObjClose (conn, out_fd);
    fclose(infile_fd);
    
    return(0);
}
int 
putMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[], 
srbPathName targNameArray[], int flagval, char *containerName, int copyNum)
{
    int i;
    int status;
    mdasC_sql_result_struct targDataResult, targCollResult;
    mdasC_sql_result_struct *dataResult, *collResult;
    int targType;
    int dataCnt = 0;
    int collCnt = 0;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN];
    char *tmpStr, *tmpStr1;

    /* use 0 flagval for expandAndChkName. don't want recursive */
    targType = expandAndChkName (conn, MDAS_CATALOG, &targNameArray[0], 0, 
      &targDataResult, &targCollResult);
    unmake_like_str (targNameArray[0].outArgv, targNameArray[0].outArgv);
    if (targDataResult.row_count + targCollResult.row_count > 1) {
	if ((flagval & m_FLAG) && (flagval & F_FLAG)) {
	    if (targCollResult.row_count > 0 && targDataResult.row_count > 0) {
        	fprintf (stderr, "The target specification is not unique\n");
        	return (CLI_ERR_COLLNAME);
	    }
	} else {
	    fprintf (stderr, "The target specification is not unique\n");
	    return (CLI_ERR_COLLNAME);
	}
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

    if ((dataCnt + collCnt > 1 || collCnt > 0) && targType >= DATANAME_T) {
	fprintf (stderr, "Target %s must be a collection\n", 
	  targNameArray[0].inpArgv);
	return (CLI_ERR_COLLNAME);
    }

    if (collCnt > 0 && (flagval & R_FLAG) == 0) {
        fprintf (stderr, "Must use the -r flag to copy a collection\n");
	return (CLI_ERR_COLLNAME);
    }

    if ((dataCnt > 1 || (dataCnt > 0 && collCnt > 0)) && 
     strlen (newpathname) > 0) {
        fprintf (stderr, "-P option not allowed with multiple sources\n");
        return (CLI_ERR_COLLNAME);
    }

    if (targType == -1) {	/* Target does not exist */
	if (dataCnt + collCnt > 1) {
            fprintf (stderr, "Target collection %s not found\n",
              targNameArray[0].inpArgv);
            return (CLI_ERR_COLLNAME);
	}
    }

    /* Now start copying */

    status = 0;
    for (i = 0; i < nArgv; i++) {
	if (nameArray[i].type >= DATANAME_T) {	/* source is data */
	    if (targType == -1) {	/* A normal new data-data copy */
    		clSplitbychar (targNameArray[0].outArgv, targPar, targCh, '/');
                /* added April 8, 2003 RAJA to take care of wildcards in targ names ***/    
		strcpy(targNameArray[0].outArgv, targPar);
                expandAndChkName (conn, MDAS_CATALOG, &targNameArray[0], 0,
		  &targDataResult, &targCollResult);
	       if (targCollResult.row_count != 1) {
		 fprintf (stderr, "The target specification %s is not unique\n", targNameArray[0].outArgv);
		 return (CLI_ERR_COLLNAME);
	       }
	       
                tmpStr = (char *) getFromResultStruct (&targCollResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
	    	status = fileToDataCopy (conn, catType, flagval, targType, 
		  &nameArray[i], tmpStr, targCh,
		   datatype, newpathname, ResourceName, containerName, copyNum);
	    } else if (targType >= DATANAME_T) {
		tmpStr = (char *) getFromResultStruct (&targDataResult,
      		  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
		tmpStr1 = (char *) getFromResultStruct (&targDataResult,
      		  dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
                status = fileToDataCopy (conn, catType, flagval, targType,
                 &nameArray[i], tmpStr, tmpStr1,
		  datatype, newpathname, ResourceName, containerName, copyNum);
	    } else if (targType == COLLECTION_T || targType == SPEC_COLL_T) {
		tmpStr = (char *) getFromResultStruct (&targCollResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
		status = fileToCollCopy (conn, catType, flagval, 
		 &nameArray[i], tmpStr, datatype,newpathname, ResourceName,
		  containerName);
	    }
	} else {	/* source is coll */
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
		status = dirToCollCopyCont (conn, catType, flagval, 
		 nameArray[i].inpArgv, targNameArray[0].outArgv, 
		  ResourceName, containerName,newpathname);
	    } else {
		tmpStr = (char *) getFromResultStruct (&targCollResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
		status = dirToCollCopy (conn, catType, flagval, 
		  &nameArray[i], tmpStr, ResourceName, containerName,
					newpathname);
	    } 
	}
        if (status == MCAT_INQUIRE_ERROR)
            status = 0;          
        else if (status < 0)
            break;
    }

    return (status);
}

void
usage (char *prog)
{
        fprintf(stderr,
          "Usage: %s [-n replNum} [-N numThreads] [-D  dataType] [-c container] [-S destResourceName] [-P destPathname]  [-R retries] [-fbpravsmMkKV] localFile|localDir destObj\n",prog);
	fprintf(stderr,
          "Usage: %s -i [-D  dataType] [-c container] [-S destResourceName] [-P destPathname] [-a] targetObj\n",prog);
}
