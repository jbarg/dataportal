/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* Slscont.c - Display containers and contents 
 */

#include "scommands.h"

char *
getMcatNameByContainer (srbConn *conn, char *containerName, int flagval,
char *inpMcatName, char *userName, char *domainName); 
int
queryAndPrtContainerMeta (srbConn *conn, char *containerName,
int flagval, char *userName, char *domainName, char *mcatName);
int
queryAndPrtIncontainerMeta (srbConn *conn, char *containerName,
int flagval, char *userName, char *domainName, char *mcatName);

extern char srbAuth[];
extern char srbHost[];
extern char srbUser[];
extern char defaultResource[];
extern char mdasCollectionName[];
extern char inCondition[];

 
void
usage(char *prog)
{
    fprintf(stderr,"Usage: %s [-aAF] [-l] [-z mcatName] [container]\n", prog);
}

int
main(int argc, char **argv)
{
    srbConn *conn;
    int i, row;
    int c;
    mdasC_sql_result_struct  dataResult;
    char *userName, *domainName;
    char fullContName[MAX_TOKEN];
    char *contCollList, *contNameList, *prevContColl, *prevContName;
    int status;
    int flagval = 0;
    char *containerName = NULL;
    char containerFamily[MAX_TOKEN];
    char *mcatName = NULL;
    char myMcatName[MAX_TOKEN];

    strcpy(inCondition , ""); 
    while ((c=getopt(argc, argv, "aAFlz:")) != EOF) {
        switch (c) {
            case 'a':
		flagval |= a_FLAG;
		break;
            case 'A':
                flagval |= A_FLAG;
                break;
            case 'F':
                flagval |= F_FLAG;
                break;
            case 'l':
		flagval |= l_FLAG;
		break;
            case 'z':
                strcpy (myMcatName, optarg);
                flagval |= z_FLAG;
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }

    if (optind > argc-1 && (flagval & F_FLAG)) {
	fprintf (stderr, "Must have at least one input container with -F\n");
	exit (1);
    }

    i = initSrbClientEnv();
    if (i < 0){
	printf("Slscont: Error:%i\n",i); 
	exit(1);
    }

    conn = srbConnect (srbHost, NULL, srbAuth,
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Slscont: Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(3);
    }

    if (conn->clientUser == NULL) {
	userName = conn->proxyUser->userName;
	domainName = conn->proxyUser->domainName;
    } else {
	userName = conn->clientUser->userName;
	domainName = conn->clientUser->domainName;
    }

    /* list container metadata */
    /* Changes made by Roman for Wildcards and Output format */
    /* if (optind >= argc-1 || (flagval & l_FLAG)) { */
    if ((flagval & F_FLAG) == 0) {	/* just list container */
        if (flagval & l_FLAG) {
            printf ("  %-70s  %-15s  %-10s  %-10s  %-7s\n",
             "Container Name", "Resource", "Size", "MaxSize", "Count");
	} else {
            printf ("Containers:\n");
	}

	if (optind >= argc) {
            mcatName = getMcatNameByContainer (conn, NULL, 
	     flagval, myMcatName, userName, domainName);
            status = queryAndPrtContainerMeta (conn, containerName,
             flagval, userName, domainName, mcatName);

            if (status < 0) {
                clFinish(conn);
                exit(4);  /* Raja */
            }
	} else {
	    for (i = optind; i < argc; i++) {
                containerName = argv[i];     /* Roman 9-6-00 begin */

		mcatName = getMcatNameByContainer (conn, containerName, 
		 flagval, myMcatName, userName, domainName);
		
		status = queryAndPrtContainerMeta (conn, containerName,
		 flagval, userName, domainName, mcatName);

                if (status < 0) {
                    clFinish(conn);
                    exit(4);  /* Raja */
                }
		if (flagval & a_FLAG) {
		    int firstTime = 1;
		    int continuation_index;

		    memset (&dataResult, 0, sizeof (dataResult));
		
		    while (dataResult.continuation_index >= 0) {
		        if (firstTime == 1) {
		    	    sprintf (containerFamily, "%s.*", containerName);
		    	    status = queryData (conn, MDAS_CATALOG, 
			     containerFamily, 0, MAX_GET_RESULTS, &dataResult);
			    firstTime = 0;
			} else {
			    continuation_index = dataResult.continuation_index;
            		    clearSqlResult (&dataResult);
            		    status = srbGetMoreRows(conn, MDAS_CATALOG,
              		     continuation_index, &dataResult, MAX_GET_RESULTS);
            		    if (dataResult.result_count == 0  || status != 0)
                		break;
			}
            		contCollList = (char *) getFromResultStruct(&dataResult,
            		  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
            		contNameList = (char *) getFromResultStruct(&dataResult,
             		 dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

            		prevContColl = prevContName = NULL;

          		for (row = 0; row < dataResult.row_count; row++) {
            		    /* Could get more than 1 result with same 
			     * container */
            		    if ((prevContColl == NULL || 
			     strcmp (contCollList, prevContColl) != 0 ||
             		      strcmp (contNameList, prevContName) != 0) &&
             		      isContFamily (contNameList, containerFamily)) {
                		sprintf (fullContName, "%s/%s", contCollList, 
			         contNameList);
                		status = queryAndPrtContainerMeta (conn, 
			         fullContName, flagval, userName, domainName, 
				 mcatName);
                		if (status < 0) {
                    		    srb_perror (2, status, "", 
				     SRB_RCMD_ACTION|SRB_LONG_MSG);
                    		    break;
				}
			    }
            		    prevContColl = contCollList;
            		    prevContName = contNameList;
            		    contCollList += MAX_DATA_SIZE;
            		    contNameList += MAX_DATA_SIZE;
          		}	/* for loop */
            		clearSqlResult (&dataResult);
		    }	/* while loop */
		}	/* a_FLAG */
	    }	/* for loop */
        }
    } else {	/* F_FLAG */
	/* print content of container */
        printf ("Containers:\n");              /* Roman 9-5-00 */
	for (i = optind; i < argc; i++) {
	    containerName = argv[i];

            mcatName = getMcatNameByContainer (conn, containerName, 
	     flagval, myMcatName, userName, domainName);

	    status = queryAndPrtIncontainerMeta (conn, containerName,
	     flagval, userName, domainName, mcatName);

            if (status < 0) {
                clFinish(conn);
                exit(4);  /* Raja */
            }
            if (flagval & a_FLAG) {
                int firstTime = 1;
                int continuation_index;

                memset (&dataResult, 0, sizeof (dataResult));

                while (dataResult.continuation_index >= 0) {
                    if (firstTime == 1) {
                        sprintf (containerFamily, "%s.*", containerName);
                        status = queryData (conn, MDAS_CATALOG,
                         containerFamily, 0, MAX_GET_RESULTS, &dataResult);
                        firstTime = 0;
                    } else {
                        continuation_index = dataResult.continuation_index;
                        clearSqlResult (&dataResult);
                        status = srbGetMoreRows(conn, MDAS_CATALOG,
                         continuation_index, &dataResult, MAX_GET_RESULTS);
                        if (dataResult.result_count == 0  || status != 0)
                            break;
                    }
                    contCollList = (char *) getFromResultStruct(&dataResult,
                      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
                    contNameList = (char *) getFromResultStruct(&dataResult,
                     dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

                    prevContColl = prevContName = NULL;

                    for (row = 0; row < dataResult.row_count; row++) {
                        /* Could get more than 1 result with same
                         * container */
                        if ((prevContColl == NULL ||
                         strcmp (contCollList, prevContColl) != 0 ||
                          strcmp (contNameList, prevContName) != 0) &&
                          isContFamily (contNameList, containerFamily)) {
                            sprintf (fullContName, "%s/%s", contCollList,
                             contNameList);
                            status = queryAndPrtIncontainerMeta (conn,
                             fullContName, flagval, userName, domainName,
                             mcatName);
                            if (status < 0) {
                                srb_perror (2, status, "",
                                 SRB_RCMD_ACTION|SRB_LONG_MSG);
                                break;
                            }
                        }
                        prevContColl = contCollList;
                        prevContName = contNameList;
                        contCollList += MAX_DATA_SIZE;
                        contNameList += MAX_DATA_SIZE;
                    }       /* for loop */
                    clearSqlResult (&dataResult);
                }   /* while loop */
            }       /* a_FLAG */
	}	/* for loop */
    }
    clFinish(conn);
    exit(0);
}

int
prtContainer (mdasC_sql_result_struct *myresult)
{
    int i;
    char *containerName;


    containerName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);

    for (i = 0; i < myresult->row_count; i++) {
	printf ("  %-s\n", containerName);
	containerName += MAX_DATA_SIZE;
    }
    return (0);
}

int
prtContainerLong (mdasC_sql_result_struct *myresult)
{
    int i;
    char *containerName, *maxContainerSize, *size, *resource, *count;
    int iCount;


    containerName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);
    maxContainerSize = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[CONTAINER_MAX_SIZE], dcs_aname[CONTAINER_MAX_SIZE]);
    size = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[CONTAINER_SIZE], dcs_aname[CONTAINER_SIZE]);
    resource = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
    count = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_ID], dcs_aname[DATA_ID]);

    for (i = 0; i < myresult->row_count; i++) {
	iCount = atoi (count) - 1;
	printf ("  %-70s  %-15s  %-10s  %-10s  %-7d\n", 
	 containerName, resource, size, maxContainerSize, iCount);
	containerName += MAX_DATA_SIZE;
	resource += MAX_DATA_SIZE;
	size += MAX_DATA_SIZE;
	maxContainerSize += MAX_DATA_SIZE;
	count += MAX_DATA_SIZE;
    }
    return (0);
}

int
prtInContainer (mdasC_sql_result_struct *myresult, char *containerName)
{

/* 9-12-00 Roman, removed *containerName bug */
/*    char *containerName, *dataName, *collection, *size, *offset;  Roman  */

    int i;
    char *dataName, *collection, *size, *offset;
    char fullName[MAX_TOKEN];

    dataName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    if (dataName == NULL)
       return (0);
    collection = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    size = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[SIZE], dcs_aname[SIZE]);
    offset = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[OFFSET], dcs_aname[OFFSET]);

    for (i = 0; i < myresult->row_count; i++) {
	sprintf (fullName, "%s/%s", collection, dataName);

        if (strcmp (containerName, fullName) != 0)  /* skip container */
            printf ("  %-100s  %-10s  %-10s\n", fullName, offset, size);

	collection += MAX_DATA_SIZE;
	dataName += MAX_DATA_SIZE;
	size += MAX_DATA_SIZE;
	offset += MAX_DATA_SIZE;
    }
    return (0);
}

char *
getMcatNameByContainer (srbConn *conn, char *containerName, int flagval,
char *inpMcatName, char *userName, char *domainName)
{
    char *mcatName;
    int status;

#ifdef FED_MCAT
    if (containerName == NULL || containerName[0] != '/') {    /* full path */
        if ((flagval & z_FLAG) == 0) {
            char *thisMcat = NULL;

            thisMcat = getmcatNamefromHint (mdasCollectionName);
            if (thisMcat == NULL) {
                status =srbGetMcatZone (conn, userName, domainName,
                 inpMcatName);
                if (status >= 0)
                    mcatName = inpMcatName;
                else
                    mcatName = NULL;
            } else if (strcmp (thisMcat, "home") == 0 ||
             strcmp (thisMcat, "container") == 0) {
                mcatName = NULL;
            } else {
                mcatName = thisMcat;
            }
         } else {
            mcatName = inpMcatName;
         }
    } else {
         mcatName = NULL;
    }
#else
    mcatName = NULL;
#endif
    return (mcatName);
}


int
queryAndPrtContainerMeta (srbConn *conn, char *containerName,
int flagval, char *userName, char *domainName, char *mcatName)
{
    char contName[MAX_TOKEN], contCollection[MAX_TOKEN], 
     fullContName[MAX_TOKEN];
    int status;
    mdasC_sql_result_struct  myresult;
    char *myContainerName;

    memset (&myresult, 0, sizeof (myresult));

    if (containerName == NULL) {
	myContainerName = NULL;
    } else {
        parseContainerName (containerName, contName, contCollection,
         userName, domainName, mcatName);
        sprintf (fullContName, "%s/%s", contCollection, contName);
	myContainerName = fullContName;
    }
    status = lsContainer (conn, myContainerName, userName, domainName,
     flagval, &myresult, mcatName);         /* Roman 9-6-00 end   */

    if (status < 0) {
        if (status != CLI_NO_ANSWER && status != MCAT_INQUIRE_ERROR) {
            fprintf (stderr,
             "Slscont: Error listing container 'status =  %i\n", status);
              srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    return (status);
        }
	return (0);
    } else if (myresult.result_count == 0) {
	return (0);
    }

    if (flagval & l_FLAG) {
        prtContainerLong (&myresult);
    } else {
        prtContainer (&myresult);
    }

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        status = srbGetMoreRows(conn, MDAS_CATALOG,
           myresult.continuation_index, &myresult, MAX_GET_RESULTS);
        if (myresult.result_count == 0  || status != 0)
            break;
        if (flagval & l_FLAG) {
            prtContainerLong (&myresult);
         } else {
            prtContainer (&myresult);
         }
     }
     clearSqlResult (&myresult);
    return (0);
}

int
queryAndPrtIncontainerMeta (srbConn *conn, char *containerName,
int flagval, char *userName, char *domainName, char *mcatName)
{
    char contName[MAX_TOKEN], contCollection[MAX_TOKEN],
     fullContName[MAX_TOKEN];
    int status;
    mdasC_sql_result_struct  myresult;

    parseContainerName (containerName, contName, contCollection,
     userName, domainName, mcatName);

    sprintf (fullContName, "%s/%s", contCollection, contName);

    status = lsInContainer (conn, userName, domainName, fullContName,
     flagval, &myresult);

    if (status < 0) {
        if (status != CLI_NO_ANSWER && status != MCAT_INQUIRE_ERROR) {
            fprintf (stderr,
             "Slscont: Error listing container 'status =  %i\n", status);
            srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    return (status);
        }
        return (0);          /* Roman 9-5-00 */
    } else if (myresult.result_count == 0) {
        return (0);
    }

    printf ("%s\n", fullContName);     /* Roman 9-5-00 */
    printf ("  %-100s  %-10s  %-10s\n",
     "InContainer Objects", "Offset", "Size");   /* Roman 9-5-00 */

    prtInContainer (&myresult, fullContName);

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        status = srbGetMoreRows(conn, MDAS_CATALOG,
         myresult.continuation_index, &myresult, MAX_GET_RESULTS);
        if (myresult.result_count == 0  || status != 0)
            break;
         prtInContainer (&myresult, fullContName);
    }
    clearSqlResult (&myresult);
    return (status);
}

