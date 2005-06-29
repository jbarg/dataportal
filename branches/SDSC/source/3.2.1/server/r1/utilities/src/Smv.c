/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#include "scommands.h"
#ifndef _WIN32
#include <unistd.h>
#endif




extern char srbAuth[];
extern char srbHost[];
extern char mdasCollectionName[];
extern char inCondition[];

srbConn *conn;

char collname[MAX_TOKEN];
char dataname[MAX_TOKEN];
char newcollection[MAX_TOKEN];

void
usage()
{
fprintf(stderr, "Usage 1: Smv srbObj targetObj\n");
fprintf(stderr, "Usage 2: Smv collection newcollection\n");
fprintf(stderr, "Usage 3: Smv  [-p] srbObj ... collectionName\n");
}

int
coll_mv_files(int argc, char argv[][MAX_OBJ_NAME_LENGTH], int pval)
{
  int i,j,k;
  char temp[MAX_TOKEN], temp1[MAX_TOKEN];
  char inbuf[100];
  sprintf(temp,"%s/alpha",newcollection);
  splitpath(temp,newcollection, temp1);
  i = checkCollection(conn, MDAS_CATALOG, newcollection);
  if (i == 0)
   {
    for (i = 1; i < argc ; i++)
      {

	if (pval == 1)
	{
	    fprintf(stdout,"Move  %s  to collection %s (y/n)?",
		    argv[i],newcollection);
	    fgets(inbuf, 100, stdin);
	    if (inbuf[0] != 'y' )
              continue;

	}
	clSplitbychar(argv[i],collname,dataname, '/');
	j = srbModifyDataset(conn, MDAS_CATALOG,dataname, collname,
			     "","",
			     newcollection,"", D_CHANGE_GROUP);
	if (j < 0)  
	  {   /* error */
	    fprintf(stderr, "Smv: error for object \"%s\"\n", 
		    argv[i]);
	    srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    return(j);
	  }
      }
   }
  else
    {
        fprintf(stderr, "Query of collection %s failed. status = %d\n",
         newcollection);
	srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	return (i);
    }
  return (0);
}

int
rename_collection(char *inname, char* newname)
{
  int i,j,k;
  char temp[MAX_TOKEN], collName[MAX_TOKEN], newName[MAX_TOKEN];
  char temp2[MAX_TOKEN];
  sprintf(temp,"%s/alpha",inname);
  splitpath(temp,collName, temp2);
  sprintf(temp,"%s/alpha",newname);
  splitpath(temp,newName, temp2);
  i = srbModifyCollect(conn, MDAS_CATALOG, collName,
		       newName, "", "", D_CHANGE_COLL_NAME);
  if (i < 0)
    {
      fprintf(stderr, 
	      "Srename: Error in renaming collection: \"%s:%i\"\n", 
	      collName,i);
      srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      return -1;
    }
  return 0;
}

int
rename_object(char *inname, char* newname)
{
  int i,j,k;
  char cval[MAX_TOKEN];
  char temp[MAX_TOKEN], collName[MAX_TOKEN], dataName[MAX_TOKEN];
  char tempName[MAX_TOKEN], newcollName[MAX_TOKEN], newdataName[MAX_TOKEN];

  srandom((unsigned int) getpid());
  sprintf(cval,"%ld",(long) random());

  splitpath(inname,collName,dataName);
  splitpath(newname,newcollName,newdataName);
  sprintf(tempName, "%s.%s", newdataName,cval);
  j =  srbModifyDataset(conn, MDAS_CATALOG,dataName, collName,
				      "","", tempName, "", D_CHANGE_DNAME);
  if (j < 0)
    {
      fprintf(stderr, 
	      "Srename: Error in renaming dataset:\"%s/%s:%i:1\"\n",
	      collName,dataName, j);
      srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      return -1;
    }
  j = srbModifyDataset(conn, MDAS_CATALOG,tempName, collName,
				      "","", newcollName, "", D_CHANGE_GROUP);
  if (j < 0)
    {
      srbModifyDataset(conn, MDAS_CATALOG,tempName, collName,
				      "","", dataName, "", D_CHANGE_DNAME);
      fprintf(stderr, 
	      "Srename: Error in renaming dataset:\"%s/%s:%i:2\"\n",
	      collName,dataName, j);
      srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      return -1;

    }
  j =  srbModifyDataset(conn, MDAS_CATALOG,tempName, newcollName,
				      "","", newdataName, "", D_CHANGE_DNAME);
  if (j < 0)
    {
      srbModifyDataset(conn, MDAS_CATALOG,tempName, newcollName,
		       "","", collName, "", D_CHANGE_DNAME);
      srbModifyDataset(conn, MDAS_CATALOG,tempName, collName,
				      "","", dataName, "", D_CHANGE_DNAME);
      fprintf(stderr, 
	      "Srename: Error in renaming dataset:\"%s/%s:%i:3\"\n",
	      collName,dataName, j);
      srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      return -1;

    }
  return 0;
}



int
main(int argc, char **argv)
{
    int  newargc;
    int pval = 0;
    /* char newargv[MAX_TOKEN][MAX_OBJ_NAME_LENGTH]; */
    char tmpStr1[MAX_TOKEN], tmpStr2[MAX_TOKEN];
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    char collName1[MAX_TOKEN],collName2[MAX_TOKEN];
    int i,j,k;
    int c;
    int flagval = 0;
    srbPathName nameArray[MAX_TOKEN], targNameArray[MAX_TOKEN];
    int nArgv;

    strcpy(inCondition , "");
    while ((c=getopt(argc, argv,"p")) != EOF) {
	switch (c) {
	    case 'p':
		flagval |= P_FLAG;
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
      {printf("Smv Initialization Error:%i\n",i); exit(1);}
    
    targNameArray[0].inpArgv = argv[argc-1];

    for (i = optind; i < argc - 1; i++) {
        nameArray[i - optind].inpArgv = argv[i];
    }
    nArgv = argc - optind - 1;		/* nArgv is the number of in file */

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Smv: Connection to srbMaster failed.\n");
        fprintf(stderr,"Smv: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }
    /* Convert the source name to SQL like */
    if (convNameArrayToSQL (nameArray, nArgv) < 0)
      {clFinish(conn); exit(4);}

    /* Convert the target name to SQL like */
    if (convNameArrayToSQL (targNameArray, 1) < 0)
      {clFinish(conn); exit(5);}

    mvMain (conn, MDAS_CATALOG, nArgv, nameArray, targNameArray, flagval);
    clFinish(conn);
    exit(0);

}

int 
mvMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[], 
srbPathName targNameArray[], int flagval)
{
    int i;
    int status, status1;
    mdasC_sql_result_struct targDataResult, targCollResult;
    mdasC_sql_result_struct *dataResult, *collResult;
    int targType;
    int dataCnt = 0;
    int collCnt = 0;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];
    char srcPar[MAX_TOKEN], srcCh[MAX_TOKEN];
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

    if (targType == -1) {	/* Target does not exist */
	if (dataCnt + collCnt > 1) {
            fprintf (stderr, "Target collection %s not found\n",
              targNameArray[0].inpArgv);
            return (CLI_ERR_COLLNAME);
	}
    }
    /* Now start moving */

    for (i = 0; i < nArgv; i++) {
      do {
	if (dataResult[i].row_count > 0) {	/* source is data */
	    if (targType == -1) {	/* A normal new data-data move */
    		clSplitbychar (targNameArray[0].outArgv, targPar, targCh, '/');
	    	status = dataToDataMove (conn, catType, flagval, targType, 
		  &dataResult[i],targPar, targCh);
	    } else if (targType >= DATANAME_T) {
		tmpStr = (char *) getFromResultStruct (&targDataResult,
      		  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
		tmpStr1 = (char *) getFromResultStruct (&targDataResult,
      		  dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
                status = dataToDataMove (conn, catType, flagval, targType,
                  &dataResult[i], tmpStr, tmpStr1);
	    } else if (targType == COLLECTION_T || targType == SPEC_COLL_T) {
		tmpStr = (char *) getFromResultStruct (&targCollResult,
                  dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
		status = dataToCollMove (conn, catType, flagval, 
		 &dataResult[i],  tmpStr);
	    }
	} 

	if (collResult[i].row_count > 0) {	/* source is coll */
	  if (collCnt == 1 && targType == -1) {   
	    tmpStr = (char *) targNameArray[0].outArgv;
	  }
	  else {
	    tmpStr = (char *) getFromResultStruct (&targCollResult,
		     dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
	  }
	  status = collToCollMove (conn, catType, flagval, 
				   &collResult[i], tmpStr);
	}
	if (dataResult[i].continuation_index >= 0 && 
	  dataResult[i].row_count > 0) {
	    clearSqlResult (&dataResult[i]);
	    status = srbGetMoreRows(conn, catType, 
	     dataResult[i].continuation_index, &dataResult[i],MAX_GET_RESULTS);
	    if (status == 0)
	    	filterDeleted (&dataResult[i]);
	} else 
	    clearSqlResult (&dataResult[i]);

	if (collResult[i].continuation_index >= 0 &&
	  collResult[i].result_count > 0) {
	    clearSqlResult (&collResult[i]);
	    status1 = srbGetMoreRows(conn, catType, 
	     collResult[i].continuation_index, &collResult[i],MAX_GET_RESULTS);
	    if (status1 == 0)
	    	filterDeleted (&collResult[i]);
	} else
	    clearSqlResult (&collResult[i]);

      } while ((dataResult[i].result_count > 0 && status == 0) ||
      (collResult[i].result_count > 0 && status1 == 0));
    }

    return (0);
}

