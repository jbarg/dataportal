/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "db2TableDvr.h"

int
db2TableOpen(dbTableDesc *exfd, char *resourceLocationName,
                  char *dataPathName, char *uid, char *pwd)
{
/*
 Open an existing table(s) object
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         char *dataPathName - Format : SQL select statement
 *
 */

    db2TableStateInfo *stateInfo;
    char  tmpStr[STRING_LENMAX];
    char *tmpPtr, *tmpPtr1, *tmpPtr2;
    char instance[STRING_LEN1],hostAddr[STRING_LEN1], database[STRING_LEN1];
    short int numCols;
    int i,ii,j,jj;
    long int             displaysize;
    unsigned char        mycolname[MAX_TOKEN];
    short int            coltype;
    short int            mycolnamelen;
    short int            buf_size;
    short int            nullable;
    unsigned long int    myprecision = 1;
    short int            scale = 1;

    if (mySplitStr(resourceLocationName, hostAddr, tmpStr, ':') == NULL) {
        elog (NOTICE, "db2TableOpen: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "db2TableOpen: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }


    if((stateInfo = malloc(sizeof (db2TableStateInfo))) == NULL) {
      elog (NOTICE, "db2TableOpen: Malloc error");
      return ORA_ERR_MALLOC;
    }

    stateInfo->mm_size = 0;
    stateInfo->num_of_cols = 0;
    stateInfo->endOfData = 0;

    if ((i =  open_env_db2Tab((long int *)&stateInfo->environ))!= 0)  {
      elog (NOTICE, "db2TableOpen: Error in Opening Environment: %i",i);
      db2TableErrorReport(NULL, stateInfo->environ);
      free(stateInfo);
      return(ORA_TAB_OPEN_ENV_ERROR);
    }
    if ((i =  connect_db2Tab( (long int)  stateInfo->environ, 
			   (long int *) &stateInfo->connect, 
			   (unsigned char*) database, 
			   (unsigned char*) uid, 
			   (unsigned char*) pwd))!= 0)  { 

      elog (NOTICE, "db2TableOpen: Error in Connecting to %s: %i",
	    database,i);
      db2TableErrorReport(NULL, stateInfo->environ);
      close_env_db2Tab( (long int) stateInfo->environ); 
      free(stateInfo);
      return(ORA_TAB_CONNECT_ERROR);
    }
    if (dataPathName == NULL || strlen(dataPathName) == 0) {
      exfd->dbSpecificInfo = (char *)stateInfo; 
      return(MDAS_SUCCESS);
    }
    while (*dataPathName == ' ') dataPathName++;
    if(strstr(dataPathName,"select") == NULL &&
       strstr(dataPathName,"SELECT") == NULL) {
        elog (NOTICE, "db2TableOpen: Error: Not a Select Statement : %s",
        dataPathName);
	close_env_db2Tab( (long int) stateInfo->environ); 
	free(stateInfo);
	return INP_ERR_FORMAT;
    }

    if (strncasecmp(dataPathName,"XML", 3) == 0) {
      strcpy(stateInfo->outputType, "XML");
      dataPathName[0] = ' ';
      dataPathName[1] = ' ';
      dataPathName[2] = ' ';
    }
    else {
      strcpy(stateInfo->outputType, "PACK");
    }
    if ((i=exec_sql_db2Tab(stateInfo->environ,stateInfo->connect,
			(long int *) &stateInfo->statement,
			(unsigned char *)dataPathName))!= 0){ 
      elog (NOTICE, "db2TableOpen: Error in Executing SQL %s: %i",
	    dataPathName,i);
      db2TableErrorReport(NULL, stateInfo->environ);
      disconnect_db2Tab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK);
      close_env_db2Tab( (long int) stateInfo->environ); 
      free(stateInfo);
      return(i);
    }
    
    if ((i= get_no_of_columns_db2Tab(stateInfo->statement, 
				  &numCols))!= 0) {
      elog (NOTICE, "db2TableOpen: Error in Getting Num of Cols: %i",i);
      db2TableErrorReport(stateInfo->statement, stateInfo->environ);
      disconnect_db2Tab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
      close_env_db2Tab( (long int) stateInfo->environ); 
      free(stateInfo);
      return(i);
    }
    if ((i = bind_default_row_db2Tab((long int) stateInfo->statement, 
		   stateInfo->data,  
		   stateInfo->data_size, &numCols)) != 0) {
      elog (NOTICE, "db2TableOpen: Error in Binding Row %i: %i",ii,i);
      db2TableErrorReport(stateInfo->statement, stateInfo->environ);
      disconnect_db2Tab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
      close_env_db2Tab( (long int) stateInfo->environ); 
      free(stateInfo);
      return(i);
    }
    for (j=1; j <= numCols; j++) {
      buf_size = MAX_TOKEN;
      myprecision = 1;
      scale = -1;
      if ((i = get_column_info_db2Tab((long int) stateInfo->statement,j, 
				   &displaysize,mycolname,
				   &coltype, buf_size, &mycolnamelen,
				   &nullable,
				   &myprecision,&scale)) != 0) {
	elog (NOTICE, 
	      "db2TableOpen: Error in Getting Column Information %i: %i",ii,i);
	db2TableErrorReport(stateInfo->statement, stateInfo->environ);
	done_with_default_result_db2Tab((long int)stateInfo->statement, 
				     stateInfo->data,  
				     stateInfo->data_size, 
				     stateInfo->num_of_cols);
	disconnect_db2Tab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
	close_env_db2Tab( (long int) stateInfo->environ); 
	free(stateInfo);
	return(i);
      }
      truncate_end_blanks(MAX_TOKEN,mycolname);
      if((stateInfo->colName[j] =
	  (char *)malloc(sizeof(char) * (strlen(mycolname)+1)))  == NULL) {
	elog (NOTICE, "db2TableOpen: Malloc error");
	done_with_default_result_db2Tab((long int)stateInfo->statement, 
				     stateInfo->data,  
				     stateInfo->data_size, 
				     stateInfo->num_of_cols);
	for (i = 1 ; i < j; i++) free(stateInfo->colName[j]);
	disconnect_db2Tab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
	close_env_db2Tab( (long int) stateInfo->environ); 
	free(stateInfo);
	return ORA_ERR_MALLOC;
      }
      strcpy(stateInfo->colName[j], mycolname);
      stateInfo->displaysize[j] = displaysize;
      stateInfo->nullable[j] = nullable;
      stateInfo->precision[j] = myprecision;
      stateInfo->scale[j] = scale;
      for (jj = 0; jj < MAXDATATYPELISTSIZE ; jj++) {
	if (oraDataTypeCode[jj] == coltype) {
	  strcpy(stateInfo->coltype[j], oraDataTypeName[jj]);
	  break;
	}
      }
      if (jj == MAXDATATYPELISTSIZE) 
	strcpy(stateInfo->coltype[j], oraDataTypeName[0]);
      
      jj =  strlen(stateInfo->colName[j]) + 
	    strlen(stateInfo->coltype[j]) +
	    strlen("COLNAME= COLTYPE= MAXSIZE= PRECISION= SCALE = ") +
	    5 * 5 + 3 * 40 + 1;
      if((stateInfo->xmlcolName[j] =
	  (char *)malloc(sizeof(char) * jj ))  == NULL) {
	elog (NOTICE, "db2TableOpen: Malloc error");
	done_with_default_result_db2Tab((long int)stateInfo->statement, 
				     stateInfo->data,  
				     stateInfo->data_size, 
				     stateInfo->num_of_cols);
	for (i = 1 ; i < j; i++) {
	  free(stateInfo->colName[j]);
	  free(stateInfo->xmlcolName[j]);
	}
	disconnect_db2Tab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
	close_env_db2Tab( (long int) stateInfo->environ); 
	free(stateInfo);
	return ORA_ERR_MALLOC;
      }
      sprintf (stateInfo->xmlcolName[j], "COLUMN COLNAME=\"%s\" COLTYPE=\"%s\" MAXSIZE=\"%li\" PRECISION=\"%li\" SCALE=\"%hi\"",
	       stateInfo->colName[j],stateInfo->coltype[j],
	       (long int)stateInfo->displaysize[j],  (long int)stateInfo->precision[j],
	       stateInfo->scale[j]);
    }

    stateInfo->tableNameList = NULL;
    tmpPtr = strdup(dataPathName);
    lower (tmpPtr);
    if ((tmpPtr1 = strstr(tmpPtr, " from ")) != NULL) {
      tmpPtr1 += 6;
      while (*tmpPtr1 == ' ') tmpPtr1++;
      if ((tmpPtr2 = strstr(tmpPtr1, " where ")) != NULL) 
	*tmpPtr2 = '\0';
      stateInfo->tableNameList = strdup(tmpPtr1);
    }
    free(tmpPtr);
    stateInfo->num_of_cols = (int) numCols;
    stateInfo->num_of_rows_read = 0;
    exfd->dbSpecificInfo = (char *)stateInfo; 
    return(MDAS_SUCCESS);
}



int 
db2TableClose(dbTableDesc  *exfd)
{
  db2TableStateInfo *stateInfo;
  int i,ii;

  stateInfo = (db2TableStateInfo *) exfd->dbSpecificInfo;

  /* transaction_end_code = 1 ABORT TRANSACTION */
  /* transaction_end_code = 0 COMMIT TRANSACTION */

  if (stateInfo->mm_size > 0)
    free(stateInfo->mm_ptr);
  if (stateInfo->tableNameList != NULL)
    free(stateInfo->tableNameList);

  if (stateInfo->statement != NULL && stateInfo->num_of_cols > 0) {
    if ((i = done_with_default_result_db2Tab((long int)stateInfo->statement, 
		       stateInfo->data,  
		       stateInfo->data_size, stateInfo->num_of_cols)) != 0) {
      elog (NOTICE, "db2TableClose: Error in Unbinding and Closing %i",i);
      db2TableErrorReport(NULL, stateInfo->environ);
      return(i);
    }    
  }
  for (i = 1; i <= stateInfo->num_of_cols ; i++){
    free(stateInfo->colName[i]);
    free(stateInfo->xmlcolName[i]);
  }
  /* commit all transactions */

  if ((i = disconnect_db2Tab( (long int)stateInfo->environ, 
			   (long int)stateInfo->connect, MDAS_CAT_COMMIT)) != 0) {
    elog (NOTICE, "db2TableClose: Error in Disconnecting:%i",i);
    db2TableErrorReport(NULL, stateInfo->environ);
    return(ORA_TAB_DISCONNECT_ERROR);
  }
  if ((i = close_env_db2Tab((long int)stateInfo->environ)) != 0) {
    elog (NOTICE, "db2TableClose: Error in Closing Environment:%i",i);
    db2TableErrorReport(NULL, stateInfo->environ);
    return(ORA_TAB_CLOSE_ENV_ERROR);
  }
    
  free(stateInfo);
  return(MDAS_SUCCESS);
}/* srbclose */

int 
db2TableRead(dbTableDesc *exfd, char *buffer, int length) 
{
  db2TableStateInfo *stateInfo;

  stateInfo = (db2TableStateInfo *) exfd->dbSpecificInfo;
  if (stateInfo->endOfData ==  1) return(MCAT_NO_DATA_FOUND);

  if (!strcmp(stateInfo->outputType, "XML"))
    return(db2TableReadXML(exfd,buffer,length));
  else
    return(db2TableReadPACK(exfd,buffer,length));
}


int 
db2TableReadPACK(dbTableDesc *exfd, char *buffer, int length)
{

  db2TableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj;
  char * mybuf;

  stateInfo = (db2TableStateInfo *) exfd->dbSpecificInfo;
  mybuf  = buffer;
  ll = length;
  if (stateInfo->num_of_rows_read == 0) {
    /* sending header information */
    strcpy(mybuf,"<HEADER>");
    mybuf += strlen("<HEADER>")+1;
    ll  -= strlen("<HEADER>")+1;
    strcpy(mybuf,stateInfo->tableNameList);
    mybuf += strlen(stateInfo->tableNameList) + 1;
    ll  -= strlen(stateInfo->tableNameList) + 1;
     for (i = 1 ; i <= stateInfo->num_of_cols ; i++) {
      strcpy(mybuf,stateInfo->colName[i]);
      mybuf += strlen(stateInfo->colName[i]) + 1;
      ll  -= strlen(stateInfo->colName[i]) + 1;
    }
    strcpy(mybuf,"</HEADER>");
    mybuf += strlen("</HEADER>")+1;
    ll  -= strlen("</HEADER>")+1;
  }
  if (stateInfo->mm_size > 0) {
    memcpy(mybuf, stateInfo->mm_ptr,stateInfo->mm_size);
    free(stateInfo->mm_ptr);
    ll -= stateInfo->mm_size;
    mybuf += stateInfo->mm_size;
    stateInfo->mm_size = 0;
  }
  if ((i = get_row_db2Tab((long int) stateInfo->statement)) != 0) {
    stateInfo->endOfData = 1;
    if (i == MCAT_NO_DATA_FOUND)
      return(length - ll);
    else 
      return(i);
  }
  while (i == 0)       
    {
      stateInfo->num_of_rows_read++;
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	truncate_end_blanks(stateInfo->data_size[ii], stateInfo->data[ii]);
      }
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	if ((k = strlen(stateInfo->data[ii])) < ll) {
	  memcpy(mybuf, stateInfo->data[ii], k+1);
	  ll -= k+1;
	  mybuf += k+1;
	}
	else { /* k >= ll make a copy for later retrieval*/
	  memcpy(mybuf, stateInfo->data[ii],ll);
	  mybuf += ll;
	  k = k - ll + 1;
	  kk = k;
	  for (jj = ii+1 ; jj < stateInfo->num_of_cols ;jj++)
	    k += strlen(stateInfo->data[jj]) + 1;
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (stateInfo->data[ii]) + ll,kk); 
	  mybuf += kk;
	  for (jj = ii+1 ; jj < stateInfo->num_of_cols ;jj++) {
	    k = strlen(stateInfo->data[jj]);
	    memcpy(mybuf,stateInfo->data[jj], k+1);
	    mybuf += k+1;
	  }
	  return(length);
	}
      }
      i = get_row_db2Tab((long int) stateInfo->statement);
    }
  stateInfo->endOfData = 1;
  if (i == MCAT_NO_DATA_FOUND) 
    return(length - ll);
  else 
    return(i);
}


int 
db2TableReadXML(dbTableDesc *exfd, char *buffer, int length)
{

  db2TableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj,tt;
  char * mybuf, *storPtr;
  char tmpStr[MAX_TOKEN], rowStr[MAX_TOKEN];

  stateInfo = (db2TableStateInfo *) exfd->dbSpecificInfo;
  mybuf  = buffer;
  ll = length;

  if (stateInfo->num_of_rows_read == 0) {
    /* sending header information */
    strcpy(mybuf,"<TABLE_RESULT>");
    mybuf += strlen("<TABLE_RESULT>")+1;
    ll  -= strlen("<TABLE_RESULT>")+1;
    strcpy(mybuf,"<TABLE_NAME>");
    mybuf += strlen("<TABLE_NAME>")+1;
    ll  -= strlen("<TABLE_NAME>")+1;
    strcpy(mybuf,stateInfo->tableNameList);
    mybuf += strlen(stateInfo->tableNameList) + 1;
    ll  -= strlen(stateInfo->tableNameList) + 1;
    strcpy(mybuf,"</TABLE_NAME>");
    mybuf += strlen("</TABLE_NAME>")+1;
    ll  -= strlen("</TABLE_NAME>")+1;
    strcpy(mybuf,"<TABLE_DATA>");
    mybuf += strlen("<TABLE_DATA>")+1;
    ll  -= strlen("<TABLE_DATA>")+1;
  }

  if (stateInfo->mm_size > 0) {
    memcpy(mybuf, stateInfo->mm_ptr,stateInfo->mm_size);
    free(stateInfo->mm_ptr);
    ll -= stateInfo->mm_size;
    mybuf += stateInfo->mm_size;
    stateInfo->mm_size = 0;
  }
  if ((i = get_row_db2Tab((long int) stateInfo->statement)) != 0) {
    stateInfo->endOfData = 1;
    strcpy(mybuf,"</TABLE_DATA>");
    mybuf += strlen("</TABLE_DATA>")+1;
    ll  -= strlen("</TABLE_DATA>")+1;
    strcpy(mybuf,"</TABLE_RESULT>");
    ll  -= strlen("</TABLE_RESULT>")+1;
    if (i == MCAT_NO_DATA_FOUND) 
      return(length - ll);
    else 
      return(i);
  }
  while (i == 0)       
    {
      stateInfo->num_of_rows_read++;
       for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	truncate_end_blanks(stateInfo->data_size[ii], stateInfo->data[ii]);
      }
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	k = 0;
	if (ii == 0) {
	  sprintf(rowStr,"<ROW ROWNUM=\"%i\">",stateInfo->num_of_rows_read);
	  k += strlen(rowStr) + 1;;
	}
	k  += (strlen(stateInfo->xmlcolName[ii+1]) + 3 +
	       strlen("COLUMN") + 4 +
	       strlen(stateInfo->data[ii]) + 1);
	if (ii == stateInfo->num_of_cols - 1) 
	  k +=strlen("</ROW>") + 1;
	tt  = 0;
	storPtr = (char *)malloc(sizeof(char) * k);
	if (ii == 0) {
	  sprintf(storPtr,rowStr);
	  tt += strlen(rowStr) + 1;
	}
	sprintf(storPtr + tt ,"<%s>",stateInfo->xmlcolName[ii+1]);
	tt += strlen(stateInfo->xmlcolName[ii+1]) + 3;
	sprintf(storPtr + tt , stateInfo->data[ii]);
	tt  += strlen(stateInfo->data[ii]) + 1;
	sprintf(storPtr + tt, "</COLUMN>");
	tt  += strlen("</COLUMN>") + 1;
	if (ii == stateInfo->num_of_cols - 1) {
	  sprintf(storPtr + tt,"</ROW>");
	  tt += strlen("</ROW>") + 1;
	}
	if (k <= ll) {
	  memcpy(mybuf, storPtr, k);
	  free(storPtr);
	  ll -= k;
	  mybuf += k;
	}
	else { /* k > ll make a copy for later retrieval*/
	  memcpy(mybuf, storPtr,ll);
	  mybuf += ll;
	  k = k - ll + 1;
	  kk = k;
	  for (jj = ii+1 ; jj < stateInfo->num_of_cols ;jj++)
	    k += (strlen(stateInfo->xmlcolName[jj+1]) + 3 +
		  strlen("COLUMN") + 4 +
		  strlen(stateInfo->data[jj]) + 1);	
	  if (ii != stateInfo->num_of_cols - 1) 
	    k += strlen("</ROW>") + 1;
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (storPtr + ll),kk); 
	  mybuf += kk;
	  free(storPtr);
	  for (jj = ii+1 ; jj < stateInfo->num_of_cols ;jj++) {
	    k  = (strlen(stateInfo->xmlcolName[jj+1]) + 3 +
		  strlen("COLUMN") + 4 +
		  strlen(stateInfo->data[jj]) + 1);
	    storPtr = (char *)malloc(sizeof(char) * k);
	    tt =0;
	    sprintf(storPtr,"<%s>",stateInfo->xmlcolName[jj+1]);
	    tt += strlen(stateInfo->xmlcolName[jj+1]) + 3;
	    sprintf(storPtr + tt, stateInfo->data[jj]);
	    tt  += strlen(stateInfo->data[jj]) + 1;
	    sprintf(storPtr + tt, "</COLUMN>");
	    tt  += strlen("</COLUMN>") + 1;
	    memcpy(mybuf,storPtr, k);
	    free(storPtr);
	    mybuf += k;
	  }
	  if (ii != stateInfo->num_of_cols - 1) {
	    strcpy(tmpStr,"</ROW>");
	    memcpy(mybuf,tmpStr,strlen(tmpStr) + 1);
	  }
	  return(length);
	}
       }
      i = get_row_db2Tab((long int) stateInfo->statement);
    }
  stateInfo->endOfData = 1;
  if (i == MCAT_NO_DATA_FOUND) {
    strcpy(mybuf,"</TABLE_DATA>");
    mybuf += strlen("</TABLE_DATA>")+1;
    ll  -= strlen("</TABLE_DATA>")+1;
    strcpy(mybuf,"</TABLE_RESULT>");
    ll  -= strlen("</TABLE_RESULT>")+1;
    return(length - ll);
  }
  else 
    return(i);
}

int 
db2TableWrite(dbTableDesc *exfd, char *sqlInsertString, int length) 
{
/*
 Insert or Update into an existing table object
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         char *sqlInsertString - Format : insert or update statement
 *
 */

  db2TableStateInfo *stateInfo;
  int i;
  char * mybuf, *storPtr;
  char tmpStr[MAX_TOKEN], rowStr[MAX_TOKEN];

  stateInfo = (db2TableStateInfo *) exfd->dbSpecificInfo;

    if ((i=exec_no_res_sql_db2Tab(stateInfo->environ,stateInfo->connect,
			(unsigned char *) sqlInsertString))!= 0){ 
      elog (NOTICE, "db2TableWrite: Error in Executing SQL %s: %i",
	    sqlInsertString,i);
      db2TableErrorReport(NULL, stateInfo->environ);
      disconnect_db2Tab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
      close_env_db2Tab( (long int) stateInfo->environ); 
      free(stateInfo);
      return(i);
    }

    return(MDAS_SUCCESS);
}

int 
db2TableSeek(dbTableDesc *exfd, 
                  int      offset, 
                  int      whence)
{
  return(ORACLE_TABLE_SID_GENERAL_ERROR);
}

int db2TableCreate(dbTableDesc   *exfd, 
                   char       *resourceLocationName, 
                   char       *tableCreateStmt,
                   char       *uid, 
                   char       *pwd)
{
/*
 Create a new table object
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         char *tableCreateStmt - Format : SQL Create Table statement
 *
 */

    db2TableStateInfo *stateInfo;
    char  tmpStr[STRING_LENMAX];
    char *tmpPtr, *tmpPtr1, *tmpPtr2;
    char instance[STRING_LEN1],hostAddr[STRING_LEN1], database[STRING_LEN1];
    short int numCols;
    int i,ii,j,jj;
    long int             displaysize;
    unsigned char        mycolname[MAX_TOKEN];
    short int            coltype;
    short int            mycolnamelen;
    short int            buf_size;
    short int            nullable;
    unsigned long int    myprecision = 1;
    short int            scale = 1;

    if (mySplitStr(resourceLocationName, hostAddr, tmpStr, ':') == NULL) {
        elog (NOTICE, "db2TableCreate: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "db2TableCreate: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }

    while (*tableCreateStmt == ' ') tableCreateStmt++;
    if(strstr(tableCreateStmt,"create") == NULL &&
       strstr(tableCreateStmt,"CREATE") == NULL) {
        elog (NOTICE, "db2TableCreate: Error: Not a Create Statement : %s",
        tableCreateStmt);
        return INP_ERR_RES_FORMAT;
    }

    if((stateInfo = malloc(sizeof (db2TableStateInfo))) == NULL) {
      elog (NOTICE, "db2TableCreate: Malloc error");
      return ORA_ERR_MALLOC;
    }

    stateInfo->mm_size = 0;
    stateInfo->num_of_cols = 0;
    stateInfo->endOfData = 1;

    if ((i =  open_env_db2Tab((long int *)&stateInfo->environ))!= 0)  {
      elog (NOTICE, "db2TableCreate: Error in Creating Environment: %i",i);
      db2TableErrorReport(NULL, stateInfo->environ);
      free(stateInfo);
      return(ORA_TAB_OPEN_ENV_ERROR);
    }
    if ((i =  connect_db2Tab( (long int)  stateInfo->environ, 
			   (long int *) &stateInfo->connect, 
			   (unsigned char*) database, 
			   (unsigned char*) uid, 
			   (unsigned char*) pwd))!= 0)  { 

      elog (NOTICE, "db2TableCreate: Error in Connecting to %s: %i",
	    database,i);
      db2TableErrorReport(NULL, stateInfo->environ);
      close_env_db2Tab( (long int) stateInfo->environ); 
      free(stateInfo);
      return(ORA_TAB_CONNECT_ERROR);
    }

    if ((i=exec_no_res_sql_db2Tab(stateInfo->environ,stateInfo->connect,
			(unsigned char *)tableCreateStmt))!= 0){ 
      elog (NOTICE, "db2TableCreate: Error in Executing SQL %s: %i",
	    tableCreateStmt,i);
      db2TableErrorReport(NULL, stateInfo->environ);
      disconnect_db2Tab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
      close_env_db2Tab( (long int) stateInfo->environ); 
      free(stateInfo);
      return(i);
    }
    
    stateInfo->tableNameList = NULL;
    tmpPtr = strdup(tableCreateStmt);
    lower (tmpPtr);
    if ((tmpPtr1 = strstr(tmpPtr, "table ")) != NULL) {
      tmpPtr1 += 6;
      while (*tmpPtr1 == ' ') tmpPtr1++;
      if ((tmpPtr2 = strstr(tmpPtr1, " ")) != NULL) 
	*tmpPtr2 = '\0';
      stateInfo->tableNameList = strdup(tmpPtr1);
    }
    free(tmpPtr);
    stateInfo->num_of_rows_read = 0;
    exfd->dbSpecificInfo = (char *)stateInfo; 
    return(MDAS_SUCCESS);
}


int db2TableUnlink(char       *resourceLocationName, 
                   char       *tableName,
                   char       *uid, 
                   char       *pwd)
{
/*
 Drop an existing table object
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         char *tablename - Format : name of table
 *
 */

    db2TableStateInfo *stateInfo;
    char  tmpStr[STRING_LENMAX];
    char  queryString[HUGE_STRING];
    char instance[STRING_LEN1],hostAddr[STRING_LEN1], database[STRING_LEN1];
    short int numCols;
    int i,ii,j,jj;
    long int             displaysize;
    unsigned char        mycolname[MAX_TOKEN];
    short int            coltype;
    short int            mycolnamelen;
    short int            buf_size;
    short int            nullable;
    unsigned long int    myprecision = 1;
    short int            scale = 1;

    if (mySplitStr(resourceLocationName, hostAddr, tmpStr, ':') == NULL) {
        elog (NOTICE, "db2TableUnlink: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "db2TableUnlink: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }


    if((stateInfo = malloc(sizeof (db2TableStateInfo))) == NULL) {
      elog (NOTICE, "db2TableUnlink: Malloc error");
      return ORA_ERR_MALLOC;
    }

    stateInfo->mm_size = 0;
    stateInfo->num_of_cols = 0;

    if ((i =  open_env_db2Tab((long int *)&stateInfo->environ))!= 0)  {
      elog (NOTICE, "db2TableUnlink: Error in Unlinking Environment: %i",i);
      db2TableErrorReport(NULL, stateInfo->environ);
      free(stateInfo);
      return(ORA_TAB_OPEN_ENV_ERROR);
    }
    if ((i =  connect_db2Tab( (long int)  stateInfo->environ, 
			   (long int *) &stateInfo->connect, 
			   (unsigned char*) database, 
			   (unsigned char*) uid, 
			   (unsigned char*) pwd))!= 0)  { 

      elog (NOTICE, "db2TableUnlink: Error in Connecting to %s: %i",
	    database,i);
      db2TableErrorReport(NULL, stateInfo->environ);
      close_env_db2Tab( (long int) stateInfo->environ); 
      free(stateInfo);
      return(ORA_TAB_CONNECT_ERROR);
    }
    sprintf(queryString, "drop table %s",tableName);
    if ((i=exec_no_res_sql_db2Tab(stateInfo->environ,stateInfo->connect,
			(unsigned char *)queryString))!= 0){ 
      elog (NOTICE, "db2TableUnlink: Error in Executing SQL %s: %i",
	    queryString,i);
      db2TableErrorReport(NULL, stateInfo->environ);
      disconnect_db2Tab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
      close_env_db2Tab( (long int) stateInfo->environ); 
      free(stateInfo);
      return(i);
    }
    
      disconnect_db2Tab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_COMMIT) ;
      close_env_db2Tab( (long int) stateInfo->environ); 
      free(stateInfo);

    return(MDAS_SUCCESS);
}
