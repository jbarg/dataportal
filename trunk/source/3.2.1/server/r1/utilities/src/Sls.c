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
extern int hasPrinted;
void usage (char *prog);
void prtDataACL (mdasC_sql_result_struct *myresult);

/* to support '-a' flag for GridPortal apps. 
 * The '-a' flag is a combination of '-l' and displaying metada. It displays metadata for 
 * a collection or a dataset with the following form.
 *      att_name:att_value:att_unit
 * , where only the 'att_unit' can be a null string.
 * So the '-a' option may display less result(s) than Smeta.
 */
extern char mdasDomainName[];
extern char srbUser[];
int disp_meta=0;   /* a flag to display user-defined metadata after long print. */
char disp_parcoll[MAX_TOKEN];
srbConn *conn;
static int DisplayUserDefinedMetatData(char *parcoll,char *dataname);
void
verifyFileInData (srbConn *conn, mdasC_sql_result_struct *myresult);
static int VerifyErrCnt = 0;

int lcval =  0;

int
main(int argc, char **argv)
{

    char collName[MAX_TOKEN], temp[MAX_TOKEN], temp2[MAX_TOKEN];
    int i, jj, cnt, argval;
    int flagval = 0;
    srbPathName nameArray[MAX_TOKEN];
    int nArgv;
    int c;
    int L_val = MAX_GET_RESULTS;
    int status;

    strcpy(inCondition , "");
    hasPrinted = 0;

    /* Removed -L option, no longer in use 2-7-01 Roman */
    while ((c=getopt(argc, argv,"aChc:A:rflVp")) != EOF) {
        switch (c) {
            case 'h':
		usage (argv[0]);
                exit (0);
            case 'a':
                flagval |= l_FLAG;
                disp_meta = 1;
                break;
	    case 'A':
		strcpy(inCondition,optarg);
		break;
	    case 'C':
		flagval |= C_FLAG;
		break;
	    case 'c':
		lcval = 1;
		strcpy(inCondition,optarg);
		break;
	    case 'r':
		flagval |= R_FLAG;
		break;
	    case 'V':
		flagval |= V_FLAG;
		break;
	    case 'f':
		flagval |= f_FLAG;
		break;
	    case 'l':
		flagval |= l_FLAG;
		break;
	    case 'p':
		/* use l and v to print physical path */
		flagval |= l_FLAG;
		flagval |= d_FLAG;
		break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }


   i = initSrbClientEnv();
    if (i < 0)
      {printf("Sls Initialization Error:%i\n",i); exit(1);}


    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
      fprintf(stderr,"Connection to srbMaster failed.\n");
      fprintf(stderr,"%s",clErrorMessage(conn));
      srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      clFinish(conn); exit(3);
    }

    if (lcval == 1) {
      strcpy(collName,"");
      nameArray[0].inpArgv = collName;
      nArgv = 1;
    }
    else if (argc == optind) {        /* No input collection or files */
	strcpy(collName,"");
	nameArray[0].inpArgv = collName;
	nArgv = 1;
	flagval |= f_FLAG;	 /* The names are all collection */
    } else {
	for (i = optind; i < argc; i++) {
	    nameArray[i - optind].inpArgv = argv[i];
	}
	nArgv = argc - optind;
    }
	
    /* Convert the name to SQL like */
    if (convNameArrayToSQL (nameArray, nArgv) < 0)
      {clFinish(conn); exit(4);}

    if (lcval == 1) {
      strcpy(nameArray[0].outArgv,"");
      nameArray[0].type = DATANAME_T;
    }
    /* The input may be a mixture of dataset and collection */
    status = queryPrtCollData (conn, MDAS_CATALOG, nArgv, nameArray, flagval, 
      L_val); 
	
    if (flagval & V_FLAG) {
	printf ("Done with file verification\n");
    }
    if (status < 0 || hasPrinted == 0) {
      clFinish(conn); exit(3);
    } else {
    	clFinish(conn);
    	exit(0);
    }
}

int
queryPrtCollData (srbConn *conn, int catType, int nArgv,
srbPathName nameArray[], int flagval, int L_val)
{
    mdasC_sql_result_struct  myresult, collResult;
    int i;
    int status, status1;
    char tmpNameStore[MAX_TOKEN * 2];
    for (i = 0; i < nArgv; i++) {

	status1 = 0;
        /* May be it is a dataName */
        strcpy(tmpNameStore,nameArray[i].outArgv);
        if (nameArray[i].type != COLLECTION_T &&
	 nameArray[i].type != SPEC_COLL_T && (flagval & f_FLAG) == 0) {
            status1 = listData (conn, catType, nameArray[i].outArgv, 
	      flagval, L_val);
        }
	if (strlen(tmpNameStore) == 0)
	  continue;

        /* list all collections associated with outArgv[i] */
        status = queryCollColl (conn, catType, tmpNameStore, flagval,
          &collResult);

	if (status < 0) {
	    if (status == MCAT_INQUIRE_ERROR) {
		if ((flagval & f_FLAG) != 0) {
		    fprintf (stderr, "%s: No such collection\n",
		      nameArray[i].inpArgv);
		} else if (status1 < 0) {
		    fprintf (stderr, "%s: No such dataset or collection\n",
		      nameArray[i].inpArgv);
		}
		continue;
	    } else {
		fprintf (stderr, "Error querying %s. Status = %d\n", 
		  nameArray[i].inpArgv, status);
		srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                return status;
	    }
	}
	do {
            listCollContent (conn, catType, &collResult, flagval, L_val);
            if (collResult.continuation_index >= 0) {
            	clearSqlResult (&collResult);
            	status = srbGetMoreRows(conn, catType,
             	  collResult.continuation_index, &collResult, MAX_GET_RESULTS);
		filterDeleted (&collResult);
            } else
            	clearSqlResult (&collResult);
    	} while (collResult.result_count > 0 && status == 0);
    }
    return (0);
}

void
listCollContent (srbConn *conn, int catType,
mdasC_sql_result_struct *collResult, int flagval, int L_val)
{
    char tmpName[MAX_TOKEN];
    char *rval[MAX_DCS_NUM];
    int j;

    /* Loop through the collections and print out the subcoll and
     * dataName in it. */

    rval[DATA_GRP_NAME] = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) collResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    hasPrinted = 1;
    for (j = 0; j < collResult->row_count; j++) {
        /* print the collection */
/*
        if (strstr(rval[DATA_GRP_NAME], "$deleted") != NULL )
            continue;
*/
	if ((flagval & V_FLAG) == 0)
            fprintf(stdout, "%s:\n", rval[DATA_GRP_NAME]);
        /* print the subColl */
        listDataInColl (conn, catType, rval[DATA_GRP_NAME], flagval, L_val);
        listSubColl (conn, catType, rval[DATA_GRP_NAME], flagval, L_val);
        rval[DATA_GRP_NAME] += MAX_DATA_SIZE;
        /* Now print the dataName */
    }
}

/* listData - List the dataName including the effects of special char.
 * put the results in dataResult.
 *
 */

int
listData (srbConn *conn, int catType, char *dataName, int flagval, int L_val)
{
    int i, j;
    int status;
    char *rval[MAX_DCS_NUM];
    mdasC_sql_result_struct dataResult;
    char inbuf[100];
    int printedrowcnt =  0;
    char tmpDataName[MAX_TOKEN];

    status = queryData (conn, catType, dataName, flagval, L_val, &dataResult);

    if (status < 0)
        return status;
    hasPrinted = 1;
    /* print the dataName */

    if (flagval & V_FLAG) {
	verifyFileInData (conn, &dataResult);
    } else if (flagval & l_FLAG) {
        if(disp_meta)
          splitbychar(dataName,disp_parcoll, tmpDataName, '/');
        prtDataLong (&dataResult, CHK_COLL, flagval);
    } else if (flagval & C_FLAG) {
	prtDataACL (&dataResult);
    } else {
        prtDataShort (&dataResult, CHK_COLL);
    }

    printedrowcnt = dataResult.row_count;

    /* loop until no more answer */

    while (dataResult.continuation_index >= 0) {
        clearSqlResult (&dataResult);
        /* getting next set of rows */
        status = srbGetMoreRows(conn, MDAS_CATALOG,
          dataResult.continuation_index, &dataResult, L_val);
        if (dataResult.result_count == 0  || status != 0)
            return (0);
	filterDeleted (&dataResult);

        /* Print the datName */
        if (flagval & V_FLAG) {
            verifyFileInData (conn, &dataResult);
        } else if (flagval & l_FLAG) {
            prtDataLong (&dataResult, CHK_COLL, flagval);
        } else if (flagval & C_FLAG) {
            prtDataACL (&dataResult);
        } else {
            prtDataShort (&dataResult, CHK_COLL);
        }
    }
    clearSqlResult (&dataResult);
    return (0);
}

/* listSubColl - Print out the sub-collections in the parent collection */

void
listSubColl (srbConn *conn, int catType, char *parColl, int flagval, int L_val)
{
    mdasC_sql_result_struct  myresult;
    int i, j;
    int status;
    char *rval[MAX_DCS_NUM];
    char inbuf[MAX_TOKEN];
    int printedrowcnt =  0;

    status = querySubCollInColl (conn, catType, parColl, flagval, L_val, 
      &myresult);
    if (status < 0)
        return;

    /* Print the sub-collection */
    
    prtColl (conn, &myresult, flagval);

    printedrowcnt = myresult.row_count;

    /* loop until no more answer */

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        /* getting next set of rows */
        status = srbGetMoreRows(conn, MDAS_CATALOG,myresult.continuation_index,
          &myresult, L_val);
        if (myresult.result_count == 0  || status != 0)
            return;
	filterDeleted (&myresult);

        /* Print the sub-collection */
        prtColl (conn, &myresult, flagval);
    }
    clearSqlResult (&myresult);
}

void
prtColl (srbConn *conn, mdasC_sql_result_struct *myresult, int flagval)
{
    int i;
    char *tmpList;
    char disp_coll[MAX_TOKEN];
    int retval;

    tmpList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    hasPrinted = 1;
    for (i = 0; i < myresult->row_count; i++) {
/*
        if (strstr(tmpList,"$deleted") == NULL )
*/
	if (flagval & C_FLAG) {
	    int status;
	    int j;
            char qval[MAX_DCS_NUM][MAX_TOKEN];
            int  selval[MAX_DCS_NUM];
            mdasC_sql_result_struct aclResult;
	    char *accessList, *userList, *domainList; 

            for (j = 0; j < MAX_DCS_NUM; j++) {
                sprintf(qval[j],"");
                selval[j] = 0;
            }

            selval[ACCESS_GROUP_NAME] = 1;
            selval[USER_NAME] = 1;
            selval[DOMAIN_DESC] = 1;
            selval[GROUP_ACCESS_CONSTRAINT] = 1;

            sprintf(qval[ACCESS_GROUP_NAME]," = '%s'", tmpList);

            status = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval,
              &aclResult, MAX_GET_RESULTS );
	    if (status >= 0) {
	        hasPrinted = 1;
                userList = (char *) getFromResultStruct (&aclResult,
                  dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
                domainList = (char *) getFromResultStruct (&aclResult,
                  dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
                accessList = (char *) getFromResultStruct (&aclResult,
                  dcs_tname[GROUP_ACCESS_CONSTRAINT], 
		  dcs_aname[GROUP_ACCESS_CONSTRAINT]);

                for (j = 0; j < aclResult.row_count; j++) {
		    if ((flagval & V_FLAG) == 0)
			if (strlen(tmpList) > 45)
                        fprintf (stdout, "  C-%s  %8.8s  %s@%s\n", 
		          tmpList, accessList, userList, domainList);
			else
			fprintf (stdout, "  C-%-46.46s  %8.8s  %s@%s\n",
                          tmpList, accessList, userList, domainList);
		    userList += MAX_DATA_SIZE;
		    domainList += MAX_DATA_SIZE;
		    accessList += MAX_DATA_SIZE;
		}
                clearSqlResult (&aclResult);
	    }
	} else {
	    if ((flagval & V_FLAG) == 0)
                fprintf (stdout, "  C-%s\n", tmpList);
	}

        /* for GridPortal */
        if(disp_meta)
        {
           strcpy(disp_coll,tmpList);
           retval = DisplayUserDefinedMetatData(disp_coll,NULL);
        }

        tmpList += MAX_DATA_SIZE;
    }
}

/* listDataInColl - Print out the dataName in the parent collection */

void
listDataInColl (srbConn *conn, int catType, char *parColl, int flagval, 
int L_val)
{
    mdasC_sql_result_struct  myresult;
    int i, j;
    int status;
    char *rval[MAX_DCS_NUM];
    char inbuf[MAX_TOKEN];
    int printedrowcnt = 0;

    status = queryDataInColl (conn, catType, parColl, flagval, L_val, 
      &myresult);

    if (status < 0)
        return;
    hasPrinted = 1;
    /* print the dataName */

    if (flagval & V_FLAG) {
        verifyFileInData (conn, &myresult);
    } else if (flagval & l_FLAG) {
        if(disp_meta)
          strcpy(disp_parcoll,parColl);
        prtDataLong (&myresult, NO_CHK_COLL, flagval);
    } else if (flagval & C_FLAG) {
        prtDataACL (&myresult);
    } else {
        prtDataShort (&myresult, NO_CHK_COLL);
    }

    printedrowcnt = myresult.row_count;

    /* loop until no more answer */

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        /* getting next set of rows */
        status = srbGetMoreRows(conn, MDAS_CATALOG,myresult.continuation_index,
          &myresult, L_val);
        if (myresult.result_count == 0  || status != 0)
            return;
	filterDeleted (&myresult);
	hasPrinted = 1;
        /* Print the datName */
        if (flagval & V_FLAG) {
            verifyFileInData (conn, &myresult);
        } else if (flagval & l_FLAG) {
            prtDataLong (&myresult, NO_CHK_COLL, flagval);
        } else if (flagval & C_FLAG) {
            prtDataACL (&myresult);
        } else {
            prtDataShort (&myresult, NO_CHK_COLL);
        }
    }
    clearSqlResult (&myresult);
}

void
prtDataShort (mdasC_sql_result_struct *myresult, int chkColl)
{
    int i;
    char *tmpList, *collList;

    tmpList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    if (chkColl == CHK_COLL) {
        collList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    }

    for (i = 0; i < myresult->row_count; i++) {
/*
        if (strstr(tmpList,"$deleted") == NULL ) {
*/
        if (chkColl == CHK_COLL && strcmp (mdasCollectionHome, collList)) {
            fprintf (stdout, "  %s/%s\n", collList, tmpList);
        } else {
            fprintf (stdout, "  %s\n", tmpList);
        }
/*
        }
*/
        tmpList += MAX_DATA_SIZE;
        if (chkColl == CHK_COLL)
            collList += MAX_DATA_SIZE;
    }
}

void
prtDataACL (mdasC_sql_result_struct *myresult)
{
    int i;
    char *dataList;
    char *collList;
    char *ownerList, *accessList, *userList, *domainList;

    dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    ownerList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_OWNER], dcs_aname[DATA_OWNER]);

    accessList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[ACCESS_CONSTRAINT], dcs_aname[ACCESS_CONSTRAINT]);

    userList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[USER_NAME], dcs_aname[USER_NAME]);

    domainList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
    hasPrinted = 1;
    for (i = 0; i < myresult->row_count; i++) {
        fprintf (stdout, 
	  "  %s %12.12s %8.8s  %s@%s\n",
               dataList, ownerList, accessList, userList, domainList);

        ownerList += MAX_DATA_SIZE;
        dataList += MAX_DATA_SIZE;
        accessList += MAX_DATA_SIZE;
        userList += MAX_DATA_SIZE;
        domainList += MAX_DATA_SIZE;
    }
}

void
prtDataLong (mdasC_sql_result_struct *myresult, int chkColl, int flagval)
{
    int i;
    char *dataList, *sizeList, *ownerList, *timeList, *replNumList, *rsrcName;
    char *collList, *containerName, *pathName, *contSize, *offset;
    char tmpContainerName[MAX_TOKEN];
    char *inCont, *dirty;
    char *isDirty;
    /* for GridPortal */
    int retval;
    char tmpDataName[MAX_TOKEN];
    char tmpCollName[MAX_TOKEN];

    dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    sizeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[SIZE], dcs_aname[SIZE]);
    ownerList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_OWNER], dcs_aname[DATA_OWNER]);
    timeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
    replNumList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
/* removed to replace by DATA_REPL_ENUM, Raja Aug 10,2000
        dcs_tname[CONTAINER_REPL_ENUM], dcs_aname[CONTAINER_REPL_ENUM]);
*/
    rsrcName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
    containerName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);
    isDirty = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[IS_DIRTY], dcs_aname[IS_DIRTY]);

    contSize = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[CONTAINER_SIZE], dcs_aname[CONTAINER_SIZE]);

    if (contSize == NULL && containerName != NULL) {
        contSize = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            "MDAS_ADCONT_REPL", dcs_aname[CONTAINER_SIZE]);
    }

    offset = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[OFFSET], dcs_aname[OFFSET]);

    if (flagval & d_FLAG) {     /* list file path */
        pathName = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);
    }

    if (chkColl == CHK_COLL) {
        collList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    }
    else
      collList = NULL;

    hasPrinted = 1;
    for (i = 0; i < myresult->row_count; i++) {
        srb_long_t iSize, iContSize, iOffset;
	int toPrint;
/*
        if (strstr(dataList,"$deleted") == NULL ) {
*/
	toPrint = 1;

        if (*containerName != '\0') {
	    if (contSize == NULL) {	/* could happened because of change of
					 * table . just us a large number */
		contSize = 10000000000000000;
	    } else {
	        iContSize = strtoll (contSize, 0, 0);
	    }
	    iSize = strtoll (sizeList, 0, 0);
	    iOffset = strtoll (offset, 0, 0);
	    if (iSize + iOffset > iContSize) {
		toPrint = 0;
	    } else {
                inCont = "IC";
	    }
        } else {
            inCont = "  ";
        }

	if (toPrint > 0) {
	    if (atoi (isDirty) > 0) 
	        dirty ="%";
	    else
	        dirty = " ";

	    if (flagval & d_FLAG) {
                fprintf (stdout,
                  "  %s  %s  %s  %s\n", dataList, sizeList, rsrcName, pathName);
            } else if (chkColl == CHK_COLL && 
	      strcmp (mdasCollectionHome, collList)) {
                fprintf (stdout,
                  "  %-10.12s %2s %-20.20s %2s %8.12s %16.16s %s %s/%s\n",
                  ownerList, replNumList, rsrcName, inCont, sizeList, timeList,
                  dirty, collList, dataList);
            } else {
                fprintf (stdout, 
	          "  %-10.12s %2s %-20.20s %2s %8.12s %16.16s %s %s\n",
                  ownerList, replNumList, rsrcName, inCont, sizeList, timeList,
                  dirty, dataList);
            }
/*
        }
*/

            /* for GridPortal support */
             if(disp_meta)
             {
                strcpy(tmpDataName,dataList);
	        if (collList != NULL)
	          strcpy(tmpCollName,collList);
	        else
	          strcpy(tmpCollName,disp_parcoll);
	      
                retval = DisplayUserDefinedMetatData(tmpCollName,tmpDataName);
             }
	}

        ownerList += MAX_DATA_SIZE;
        sizeList += MAX_DATA_SIZE;
        replNumList += MAX_DATA_SIZE;
        timeList += MAX_DATA_SIZE;
        dataList += MAX_DATA_SIZE;
        rsrcName += MAX_DATA_SIZE;
	containerName += MAX_DATA_SIZE;
	isDirty += MAX_DATA_SIZE;
        if (chkColl == CHK_COLL)
            collList += MAX_DATA_SIZE;
        if (flagval & d_FLAG) {     /* list file path */
            pathName += MAX_DATA_SIZE;
        }
	contSize += MAX_DATA_SIZE;
	offset += MAX_DATA_SIZE;
    }
}

void
usage (char *prog)
{
    fprintf(stderr,"Usage  :%s [-halC] [-r|-f] [-A condition] [collection(or srbObj) ...]\n",
      prog);
    fprintf(stderr,"Usage  :%s [-halC]  -c condition\n",
      prog);
}


/* Bing Zhu 3-25-2002
 * The function was created for GridPortal team to handle the '-a' option.
 * Bing. Modified on June 25 2002 for displaying metadata for collection(s).
 * Input: if dataname is NULL, the input is a collection.
 */
static int DisplayUserDefinedMetatData(char *parcoll,char *dataname)
{
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  char *rval[MAX_DCS_NUM];
  int  obj_type;   /* 1 --> for dataset. 0 --> for collection.*/
  int  i,jj;
  int  d0,d1,d2,d3;

  int  to_print;

  mdasC_sql_result_struct tmpMetaResult;


  obj_type = 1;

  for (i = 0; i < MAX_DCS_NUM; i++)
  {
     sprintf(qval[i],"");
     selval[i] = 0;
  }

  sprintf(qval[DATA_GRP_NAME]," = '%s'",parcoll);
  if((dataname != NULL) &&(strlen(dataname) > 0))
  {
     sprintf(qval[DATA_NAME]," = '%s'",dataname);
  }
  else
  {
     obj_type = 0;
  }

  switch(obj_type)
  {
    case 0:
      d0 = UDSMD_COLL0;
      d1 = UDSMD_COLL1;
      d2 = UDSMD_COLL2;
      d3 = UDSMD_COLL3;
      break;
    case 1:
      d0 = UDSMD0;
      d1 = UDSMD1;
      d2 = UDSMD2;
      d3 = UDSMD3;
      sprintf(qval[USER_NAME]," = '%s'",srbUser);
      sprintf(qval[DOMAIN_DESC]," = '%s'",mdasDomainName);
      sprintf(qval[DATA_ACCESS_PRIVILEGE]," like '%%''read''%%'");
      break;
  }
  selval[d0] = 1;
  selval[d1] = 1;
  selval[d2] = 1;
  selval[d3] = 1;

/* Bing: July 22, 2002. The following check is only for datasets. 
 *       Code is commented out and moved into switch block for dataset
 *       case.
 */
/*
  sprintf(qval[USER_NAME]," = '%s'",srbUser);
  sprintf(qval[DOMAIN_DESC]," = '%s'",mdasDomainName);
  sprintf(qval[DATA_ACCESS_PRIVILEGE]," like '%%''read''%%'");
*/

  jj = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval,
                            &tmpMetaResult, MAX_GET_RESULTS);

  if(jj < 0)
  {
     return jj;
  }

  do
  {
     rval[d0] = (char *)getFromResultStruct ((mdasC_sql_result_struct *) &tmpMetaResult,
                                dcs_tname[d0], dcs_aname[d0]);
     rval[d1] = (char *)getFromResultStruct ((mdasC_sql_result_struct *) &tmpMetaResult,
                                dcs_tname[d1], dcs_aname[d1]);
     rval[d2] = (char *)getFromResultStruct ((mdasC_sql_result_struct *) &tmpMetaResult,
                                dcs_tname[d2], dcs_aname[d2]);
     rval[d3] = (char *)getFromResultStruct ((mdasC_sql_result_struct *) &tmpMetaResult,
                                dcs_tname[d3], dcs_aname[d3]);

     for(i=0;i<tmpMetaResult.row_count;i++)
     {
        to_print = 1;
        if((strstr(rval[d0],"$deleted")==NULL)&&(strstr(rval[d1],"$deleted")==NULL)&&(strstr(rval[d2],"$deleted")==NULL))
        {
/*   Bing modified on July 23, 2002 . The both att_name and att_value cannot be null. */
           if((rval[d0]==NULL) || (strlen(rval[d0]) == 0))
             to_print = 0;

           if((rval[d1] == NULL) || (strlen(rval[d1]) == 0))
             to_print = 0;

           if(to_print)
           {
              printf("      %s:%s",rval[d0],rval[d1]);
              if(((rval[d2] != NULL)&&(strlen(rval[d2]) > 0)))
                printf(":%s",rval[d2]);
              if(((rval[d3] != NULL)&&(strlen(rval[d3]) > 0)))
                printf(":%s",rval[d3]);
              printf("\n");
           }

/* 
           if((rval[d0] != NULL)&&(strlen(rval[d0]) > 0))
           {
              printf("      %s:",rval[d0]);
           }
           else
           {
              printf("      :");
           }
           if((rval[d1] != NULL) && (strlen(rval[d1]) > 0))
           {
              printf("%s:",rval[d1]);
           }
           else
           {
              printf(":");
           }
           if(((rval[d2] != NULL)&&(strlen(rval[d2]) > 0)))
           {
              printf("%s",rval[d2]);
           }
           
           printf("\n");
*/
           rval[d0] += MAX_DATA_SIZE;
           rval[d1] += MAX_DATA_SIZE;
           rval[d2] += MAX_DATA_SIZE;
           rval[d3] += MAX_DATA_SIZE;
        }
     }

     free_result_struct(selval, &tmpMetaResult);
     if (tmpMetaResult.continuation_index < 0)
       return 0;

     jj = srbGetMoreRows(conn, MDAS_CATALOG,tmpMetaResult.continuation_index,
                      &tmpMetaResult, MAX_GET_RESULTS);
     if(jj != 0)
       return jj;

  }
  while (tmpMetaResult.continuation_index >= 0);

  return 0;
}

void
verifyFileInData (srbConn *conn, mdasC_sql_result_struct *myresult)
{
    int i, status;
    char *tmpList, *dataList, *collList, *fileList, 
     *resTypeList, *resLocList, *sizeList, *offsetList, *containerList;
    struct srbStat myStat;

    dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    collList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    fileList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);

    resLocList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[RSRC_ADDR_NETPREFIX], dcs_aname[RSRC_ADDR_NETPREFIX]);

    resTypeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[PHY_RSRC_TYP_NAME], dcs_aname[PHY_RSRC_TYP_NAME]);

    sizeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[SIZE], dcs_aname[SIZE]);

    offsetList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[OFFSET], dcs_aname[OFFSET]);

    containerList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);
    hasPrinted = 1;
    for (i = 0; i < myresult->row_count; i++) {
	int myType;
	srb_long_t mySize, myOffset;

	/* find the type */
	if (strstr (resTypeList, "unix") != NULL) {
	    myType = UNIX_FT;
#ifdef foo
	} else if (strstr (resTypeList, "hpss") != NULL) {
            fprintf (stderr, "not checking  hpss data %s/%s\n",
             collList, dataList);
            /* myType = HPSS_FT; */
            myType = -1;
	} else if (strstr (resTypeList, "UNITREE_FT") != NULL) {
	    myType = UNITREE_FT;
	} else {
	    myType = -1;
	    fprintf (stderr, "verifyFileInData: %s/%s - unknown type %s \n",
	     collList, dataList, resTypeList);
	}
#endif
	} else {
	    myType = -1;
	    if (VerifyErrCnt <= 0) {
		fprintf (stderr, 
		 "verifyFileInData: Currently checking only files in resource type UNIX. At least one file is in non-UNIX type resource - %s\n",
		 resTypeList);
		VerifyErrCnt++;
	    }
	}

	if (myType >= 0) {
	    status = srbFileStat (conn, myType, resLocList, fileList, &myStat);
	    if (status < 0) {
                fprintf (stderr, "problem with stat of %s/%s, stat = %d\n",
                 collList, dataList, status);
		fprintf (stderr, "filePath = %s\n", fileList);
	    } else {
		int contLen;
		char myPath[MAX_TOKEN];
		
	        mySize = strtoll (sizeList, 0, 0);
		
		if ((contLen = strlen (containerList)) > 0) {	/* container */
		    sprintf (myPath, "%s/%s", collList, dataList);
		}

		if (contLen > 0 && strcmp (myPath, containerList) == 0) {
	            myOffset = strtoll (offsetList, 0, 0);
		    mySize += myOffset;
                    if (mySize > myStat.st_size) {
                        fprintf (stderr,
                         "problem with inCont file %s/%s size mismatch mcat=%lld, file=%lld\n",
                         collList, dataList, mySize, myStat.st_size);
                        fprintf (stderr, "filePath = %s\n", fileList);
                    }
		} else {
	            if (mySize != myStat.st_size) {
                        fprintf (stderr, 
		         "problem with %s/%s size mismatch mcat=%lld, file=%lld\n",
                         collList, dataList, mySize, myStat.st_size);
                        fprintf (stderr, "filePath = %s\n", fileList);
		    }
		}
	    }
	}

        dataList += MAX_DATA_SIZE;
        collList += MAX_DATA_SIZE;
        fileList += MAX_DATA_SIZE;
        resLocList += MAX_DATA_SIZE;
        sizeList += MAX_DATA_SIZE;
        resTypeList += MAX_DATA_SIZE;
        offsetList += MAX_DATA_SIZE;
        containerList += MAX_DATA_SIZE;
    }
}

