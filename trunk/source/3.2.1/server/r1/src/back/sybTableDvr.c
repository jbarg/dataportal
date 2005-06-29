/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



#include "sybTableDvr.h"
/*
#define malloc(x) mymalloc(__FILE__, __LINE__ , x)
#define free(x)   myfree(__FILE__, __LINE__ , x)
*/

int
sybTableOpen(dbTableDesc *exfd, char *resourceLocationName,
                  char *dataPathName, char *uid, char *pwd)
{
/*
 Open an existing table(s) object
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         char *dataPathName - Format : SQL select statement
 *
 */

    tableStateInfo *stateInfo;
    char  tmpChar, tmpStr[STRING_LENMAX];
    char *tmpPtr, *tmpPtr1, *tmpPtr2, *tmpPtr3;
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
        elog (NOTICE, "sybTableOpen: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "sybTableOpen: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }


    if((stateInfo = malloc(sizeof (tableStateInfo))) == NULL) {
      elog (NOTICE, "sybTableOpen: Malloc error");
      return DB_ERR_MALLOC;
    }

    stateInfo->mm_size = 0;
    stateInfo->num_of_cols = 0;
    stateInfo->num_of_rows_read = 0;
    stateInfo->endOfData = 0;
    stateInfo->dataTemplateH = NULL;
    stateInfo->dataTemplateB = NULL;
    stateInfo->dataTemplateT = NULL;
    stateInfo->sql = NULL;
    stateInfo->tableNameList= NULL;
    stateInfo->dataTemplateChoice.choiceCount = 0;
    stateInfo->dataTemplateChoice.roundRobin = 0;
    stateInfo->dataTemplateChoice.choiceCondition = NULL;
    stateInfo->dataTemplateChoice.compiledHeadRE[0] = NULL;
    stateInfo->dataTemplateChoice.compiledTailRE[0] = NULL;
    stateInfo->definedValues.varCount = 0;
    stateInfo->constantValues.varCount = 0;
    stateInfo->storedSQLBuffer = NULL;
    stateInfo->freestoredSQLBufferSize = 0;
    stateInfo->num_of_tables = 0;
    stateInfo->constantValues.varCount = 0;
    stateInfo->definedValues.varCount = 0;
    strcpy(stateInfo->constantValues.preString, SYSTEM_OPEN_TAG);
    strcpy(stateInfo->definedValues.preString, STANDARAD_OPEN_TAG);
    strcpy(stateInfo->sqlType,"");
    strcpy(stateInfo->dataType,"");
    stateInfo->userFuncs.funcCount = 0;
    stateInfo->compSql.sqlCount = 0;
    for (i = 0 ; i < MAX_TABLE_COLS ; i++)
      stateInfo->colName[i] =  NULL;
    if ((i =  open_env_sybTab((long int *)&stateInfo->environ))!= 0)  {
      elog (NOTICE, "sybTableOpen: Error in Opening Environment: %i",i);
      sybTableErrorReport(NULL, stateInfo->environ);
      free(stateInfo); stateInfo = NULL;
      return(DB_TAB_OPEN_ENV_ERROR);
    }
    if ((i =  connect_sybTab( (long int)  stateInfo->environ, 
			   (long int *) &stateInfo->connect, 
			   (unsigned char*) database, 
			   (unsigned char*) uid, 
			   (unsigned char*) pwd))!= 0)  { 

      elog (NOTICE, "sybTableOpen: Error in Connecting to %s: %i",
	    database,i);
      sybTableErrorReport(NULL, stateInfo->environ);
      close_env_sybTab( (long int) stateInfo->environ); 
      free(stateInfo); stateInfo = NULL;
      return(DB_TAB_CONNECT_ERROR);
    }
    if (dataPathName == NULL || strlen(dataPathName) == 0) {
      strcpy(stateInfo->dataType, "SQL");
      exfd->dbSpecificInfo = (char *)stateInfo; 
      addToVarList(&(stateInfo->constantValues), "DT","SQL", "CHAR");
      return(MDAS_SUCCESS);
    }

    
    i = extractOptionsDBDvr(stateInfo, &dataPathName);
    if (i != 0) {
      elog (NOTICE, "sybTableOpen: Error in Extracting Options from Command %s: %i",
	    dataPathName,i);
      sybTableErrorReport(NULL, stateInfo->environ);
      close_env_sybTab( (long int) stateInfo->environ); 
      free(stateInfo); stateInfo = NULL;
      return(i);
    }

    if (dataPathName != NULL && strlen(dataPathName) > 0) {
      if (stateInfo->sql != NULL) 
	free(stateInfo->sql);
      stateInfo->sql = strdup(dataPathName);
      updateVarList(&(stateInfo->constantValues), "QQ",dataPathName, "CHAR");
    }
    else if (stateInfo->sql != NULL) 
      dataPathName = stateInfo->sql;

    if (strcmp(stateInfo->dataType, "SQL")) {
      if (strstr(dataPathName,"insert") == dataPathName ||
	  strstr(dataPathName,"INSERT") == dataPathName) {
	strcpy(stateInfo->sqlType,"INSERT");
	addToVarList(&(stateInfo->constantValues), "QT","INSERT", "CHAR");
	if ((tmpPtr = strstr(dataPathName," into ")) == NULL &&
	    (tmpPtr = strstr(dataPathName," INTO ")) == NULL) {
	  exfd->dbSpecificInfo = (char *)stateInfo; 
	  return(MDAS_SUCCESS);
	}
	tmpPtr += 6;
	while (isspace(*tmpPtr)) tmpPtr++;
	if ((tmpPtr2 = strstr(tmpPtr, " ")) == NULL &&
	    (tmpPtr2 = strstr(tmpPtr, "(")) == NULL) {
	  stateInfo->tableNameList = strdup(tmpPtr);
	  for ( ; *tmpPtr != '\0'; tmpPtr++) 
	    if (*tmpPtr == ',') stateInfo->num_of_tables++;
	  stateInfo->num_of_tables++;
	  addToVarList(&(stateInfo->constantValues), "TL",stateInfo->tableNameList, "CHAR");
	  sprintf(tmpStr,"%i",stateInfo->num_of_tables);
	  addToVarList(&(stateInfo->constantValues), "NT",tmpStr, "CHAR");
	  exfd->dbSpecificInfo = (char *)stateInfo; 
	  i = getColumnInfoForsybTable((long int)stateInfo->environ, 
				    (long int)stateInfo->connect,stateInfo->tableNameList,
				    &(stateInfo->num_of_cols),stateInfo->colName,
				    stateInfo->coltype,stateInfo->displaysize,
				    stateInfo->nullable,stateInfo->precision,
				    stateInfo->scale  );
	  sprintf(tmpStr,"%i",stateInfo->num_of_cols);
	  addToVarList(&(stateInfo->constantValues), "NC",tmpStr, "CHAR");
	  for (i = 0; i < stateInfo->num_of_cols; i++) {
	    stateInfo->lastData[i] = NULL;
	    stateInfo->data[i] = NULL;
	    stateInfo->xmlcolName[i] = NULL;
	    stateInfo->xmlcolNameOpen[i] = NULL;
	    stateInfo->xmlcolNameClose[i] = NULL;
	  }
	  exfd->dbSpecificInfo = (char *)stateInfo; 
	  return(MDAS_SUCCESS);
	}
	tmpChar = *tmpPtr2;
	*tmpPtr2 = '\0';
	stateInfo->tableNameList = strdup(tmpPtr);
	for ( ; *tmpPtr != '\0'; tmpPtr++) 
	  if (*tmpPtr == ',') stateInfo->num_of_tables++;
	stateInfo->num_of_tables++;
	addToVarList(&(stateInfo->constantValues), "TL",stateInfo->tableNameList, "CHAR");
	sprintf(tmpStr,"%i",stateInfo->num_of_tables);
	addToVarList(&(stateInfo->constantValues), "NT",tmpStr, "CHAR");
	*tmpPtr2 = tmpChar;
	if ((tmpPtr3 = strstr(tmpPtr2, "(")) == NULL) {
	  exfd->dbSpecificInfo = (char *)stateInfo; 
	  i = getColumnInfoForsybTable((long int)stateInfo->environ, 
				    (long int)stateInfo->connect,stateInfo->tableNameList,
				    &(stateInfo->num_of_cols),stateInfo->colName,
				    stateInfo->coltype,stateInfo->displaysize,
				    stateInfo->nullable,stateInfo->precision,
				    stateInfo->scale  );
	  sprintf(tmpStr,"%i",stateInfo->num_of_cols);
	  addToVarList(&(stateInfo->constantValues), "NC",tmpStr, "CHAR");
	  for (i = 0; i < stateInfo->num_of_cols; i++) {
	    stateInfo->lastData[i] = NULL;
	    stateInfo->data[i] = NULL;
	    stateInfo->xmlcolName[i] = NULL;
	    stateInfo->xmlcolNameOpen[i] = NULL;
	    stateInfo->xmlcolNameClose[i] = NULL;
	  }
	  return(MDAS_SUCCESS);
	}
	i = 0;
	tmpPtr3++;
	tmpPtr2 = tmpPtr3;
	while ((tmpPtr3 = strstr(tmpPtr2,",")) != NULL) {
	  *tmpPtr3 = '\0';
	  stateInfo->colName[i] = strdup(tmpPtr2);
	  *tmpPtr3 = ',';
	  tmpPtr2 = tmpPtr3 + 1;
	  i++;
	}
	if ((tmpPtr3 = strstr(tmpPtr2, ")")) == NULL) {
	  elog (NOTICE, "sybTableOpen: Insert Statement Format Error: %s",
		dataPathName);
	  disconnect_sybTab( (long int)stateInfo->environ, 
			     (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
	  close_env_sybTab( (long int) stateInfo->environ); 
	  free(stateInfo); stateInfo = NULL;
	  return  INP_ERR_FORMAT;
	}
	*tmpPtr3 = '\0';
	stateInfo->colName[i] = strdup(tmpPtr2);
	stateInfo->num_of_cols = i+1;
	sprintf(tmpStr,"%i",stateInfo->num_of_cols);
	addToVarList(&(stateInfo->constantValues), "NC",tmpStr, "CHAR");
	for (i = 0; i < stateInfo->num_of_cols; i++) {
	  stateInfo->lastData[i] = NULL;
	  stateInfo->data[i] = NULL;
	  stateInfo->xmlcolName[i] = NULL;
	  stateInfo->xmlcolNameOpen[i] = NULL;
	  stateInfo->xmlcolNameClose[i] = NULL;
	  if (getColTypeFromColNamesybTab((long int)stateInfo->environ, 
				(long int)stateInfo->connect,
				stateInfo->coltype[i], stateInfo->colName[i],
				stateInfo->tableNameList) != 0)
	    strcpy(stateInfo->coltype[i],"UNKNOWNTYPE");
	}
	exfd->dbSpecificInfo = (char *)stateInfo; 
	return MDAS_SUCCESS;
      }
      else if (strstr(dataPathName,"delete") == dataPathName ||
	  strstr(dataPathName,"DELETE") == dataPathName) {
	strcpy(stateInfo->sqlType,"DELETE");
	addToVarList(&(stateInfo->constantValues), "QT","DELETE", "CHAR");
	if ((tmpPtr = strstr(dataPathName," from ")) == NULL &&
	    (tmpPtr = strstr(dataPathName," FROM ")) == NULL) {
	  elog (NOTICE, "sybTableOpen: Delete Statement Format Error: %s",
		dataPathName);
	  disconnect_sybTab( (long int)stateInfo->environ, 
			     (long int)stateInfo->connect, MDAS_CAT_ROLLBACK);
	  close_env_sybTab( (long int) stateInfo->environ); 
	  free(stateInfo); stateInfo = NULL;
	  return INP_ERR_FORMAT;
	}
	tmpPtr += 6;
	while (isspace(*tmpPtr)) tmpPtr++;
	if ((tmpPtr2 = strstr(tmpPtr, " ")) == NULL &&
	    (tmpPtr2 = strstr(tmpPtr, "(")) == NULL) {
	  stateInfo->tableNameList = strdup(tmpPtr);
	  for ( ; *tmpPtr != '\0'; tmpPtr++) 
	    if (*tmpPtr == ',') stateInfo->num_of_tables++;
	  stateInfo->num_of_tables++;
	  addToVarList(&(stateInfo->constantValues), "TL",stateInfo->tableNameList, "CHAR");
	  sprintf(tmpStr,"%i",stateInfo->num_of_tables);
	  addToVarList(&(stateInfo->constantValues), "NT",tmpStr, "CHAR");
	  exfd->dbSpecificInfo = (char *)stateInfo; 
	  return(MDAS_SUCCESS);
	}
	*tmpPtr3 = *tmpPtr2;
	*tmpPtr2 = '\0';
	stateInfo->tableNameList = strdup(tmpPtr);
	for ( ; *tmpPtr != '\0'; tmpPtr++) 
	  if (*tmpPtr == ',') stateInfo->num_of_tables++;
	stateInfo->num_of_tables++;
	addToVarList(&(stateInfo->constantValues), "TL",stateInfo->tableNameList, "CHAR");
	sprintf(tmpStr,"%i",stateInfo->num_of_tables);
	addToVarList(&(stateInfo->constantValues), "NT",tmpStr, "CHAR");
	*tmpPtr2 = *tmpPtr3;
	if ((tmpPtr3 = strstr(tmpPtr2, "(")) == NULL) {
	  exfd->dbSpecificInfo = (char *)stateInfo; 
	  return(MDAS_SUCCESS);
	}
	i = 0;
	while ((tmpPtr3 = strstr(tmpPtr2,",")) != NULL) {
	  *tmpPtr3 = '\0';
	  stateInfo->colName[i] = strdup(tmpPtr2);
	  *tmpPtr3 = ',';
	  tmpPtr2 = tmpPtr3 + 1;
	  i++;
	}
	if ((tmpPtr3 = strstr(tmpPtr2, ")")) == NULL) {
	  elog (NOTICE, "sybTableOpen: Insert Statement Format Error: %s",
		dataPathName);
	  disconnect_sybTab( (long int)stateInfo->environ, 
			     (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
	  close_env_sybTab( (long int) stateInfo->environ); 
	  free(stateInfo); stateInfo = NULL;
	  return  INP_ERR_FORMAT;
	}
	*tmpPtr3 = '\0';
	stateInfo->colName[i] = strdup(tmpPtr2);
	stateInfo->num_of_cols = i+1;
	sprintf(tmpStr,"%i",stateInfo->num_of_cols);
	addToVarList(&(stateInfo->constantValues), "NC",tmpStr, "CHAR");
	for (i = 0; i < stateInfo->num_of_cols; i++) {
	  stateInfo->lastData[i] = NULL;
	  stateInfo->data[i] = NULL;
	  stateInfo->xmlcolName[i] = NULL;
	  stateInfo->xmlcolNameOpen[i] = NULL;
	  stateInfo->xmlcolNameClose[i] = NULL;
	  if (getColTypeFromColNamesybTab((long int)stateInfo->environ, 
				(long int)stateInfo->connect,
				stateInfo->coltype[i], stateInfo->colName[i],
				stateInfo->tableNameList) != 0)
	    strcpy(stateInfo->coltype[i],"UNKNOWNTYPE");
	}
	exfd->dbSpecificInfo = (char *)stateInfo; 
	return MDAS_SUCCESS;
      }
      else if (strstr(dataPathName,"update") == dataPathName ||
	  strstr(dataPathName,"UPDATE") == dataPathName) {
	strcpy(stateInfo->sqlType,"UPDATE");
	addToVarList(&(stateInfo->constantValues), "QT","UPDATE", "CHAR");
	exfd->dbSpecificInfo = (char *)stateInfo; 
	return MDAS_SUCCESS;
      }
      else if (strstr(stateInfo->sql,"multiinsert") == stateInfo->sql ||
	  strstr(stateInfo->sql,"MULTIINSERT") == stateInfo->sql) {
	strcpy(stateInfo->sqlType,"MULTIINSERT");
	addToVarList(&(stateInfo->constantValues), "QT","MULTIINSERT", "CHAR");
	i = getMultiInsertInformation(stateInfo, (stateInfo->sql) + 11);
	if (i < 0) {
	  elog (NOTICE, "sybTableOpen:Error: %i in  getMultiInsertInformation");
	  close_env_sybTab( (long int) stateInfo->environ); 
	  free(stateInfo); stateInfo = NULL;
	  return(i);
	}
	if ((stateInfo->storedSQLBuffer = 
	     (char *) malloc(SVR_OBJ_READ_BUFFER_SIZE)) == NULL) {
	  elog (NOTICE, "sybTableOpen:Error: Memory Allocation Error");
	  close_env_sybTab( (long int) stateInfo->environ); 
	  free(stateInfo); stateInfo = NULL;
	  return INP_ERR_FORMAT;
	}
	stateInfo->freestoredSQLBufferSize = SVR_OBJ_READ_BUFFER_SIZE - 1;
	stateInfo->storedSQLBuffer[0]  = '\0';
	stateInfo->num_of_cols = getNumOfColsFromTags(stateInfo);
	for (i = 0; i < stateInfo->num_of_cols; i++) {
	  stateInfo->lastData[i] = NULL;
	  stateInfo->data[i] = NULL;
	  stateInfo->xmlcolNameOpen[i] = NULL;
	  stateInfo->xmlcolNameClose[i] = NULL;
	}
	exfd->dbSpecificInfo = (char *)stateInfo; 
	return MDAS_SUCCESS;
      }
      else if (strstr(dataPathName,"select") != dataPathName &&
	      strstr(dataPathName,"SELECT") != dataPathName) {
        elog (NOTICE, "sybTableOpen:Error: Not a Known Type SQL Statement: %s",
	      dataPathName);
	close_env_sybTab( (long int) stateInfo->environ); 
	free(stateInfo); stateInfo = NULL;
	return INP_ERR_FORMAT;
      }
    }
    
    if (strstr(dataPathName,"select") != dataPathName &&
	strstr(dataPathName,"SELECT") != dataPathName) {
      /* executable SQL statement other than select */
      if ((i=exec_no_res_sql_sybTab(stateInfo->environ,stateInfo->connect,
			(unsigned char *) dataPathName))!= 0) { 
	elog (NOTICE, "sybTableOpen: Error in Executing SQL %s: %i",
	      dataPathName,i);
	sybTableErrorReport(NULL, stateInfo->environ);
	disconnect_sybTab( (long int)stateInfo->environ, 
			   (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
	close_env_sybTab( (long int) stateInfo->environ); 
	free(stateInfo); stateInfo = NULL;
	return(i);
      }
      exfd->dbSpecificInfo = (char *)stateInfo; 
      return(MDAS_SUCCESS);
    }


    if (!strcmp(stateInfo->dataType, "SQL")) 
      strcpy(stateInfo->dataType, "PACK");

    
    strcpy(stateInfo->sqlType,"SELECT");
    addToVarList(&(stateInfo->constantValues), "QT","UPDATE", "CHAR");
    if ((i=exec_sql_sybTab(stateInfo->environ,stateInfo->connect,
			   (long int *) &stateInfo->statement,
			   (unsigned char *)dataPathName))!= 0){ 
      elog (NOTICE, "sybTableOpen: Error in Executing SQL %s: %i",
	    dataPathName,i);
      sybTableErrorReport(NULL, stateInfo->environ);
      disconnect_sybTab( (long int)stateInfo->environ, 
			 (long int)stateInfo->connect, MDAS_CAT_ROLLBACK);
      close_env_sybTab( (long int) stateInfo->environ); 
      free(stateInfo); stateInfo = NULL;
      return(i);
    }
    
    if ((i= get_no_of_columns_sybTab(stateInfo->statement, 
				  &numCols))!= 0) {
      elog (NOTICE, "sybTableOpen: Error in Getting Num of Cols: %i",i);
      sybTableErrorReport(stateInfo->statement, stateInfo->environ);
      disconnect_sybTab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
      close_env_sybTab( (long int) stateInfo->environ); 
      free(stateInfo); stateInfo = NULL;
      return(i);
    }
    if ((i = bind_default_row_sybTab((long int) stateInfo->statement, 
		   stateInfo->data,  
		   stateInfo->data_size, &numCols)) != 0) {
      elog (NOTICE, "sybTableOpen: Error in Binding Row %i: %i",ii,i);
      sybTableErrorReport(stateInfo->statement, stateInfo->environ);
      disconnect_sybTab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
      close_env_sybTab( (long int) stateInfo->environ); 
      free(stateInfo); stateInfo = NULL;
      return(i);
    }
    for (j=0; j < numCols; j++) {
      buf_size = MAX_TOKEN;
      myprecision = 1;
      scale = -1;
      if ((i = get_column_info_sybTab((long int) stateInfo->statement,j+1, 
				   &displaysize,mycolname,
				   &coltype, buf_size, &mycolnamelen,
				   &nullable,
				   &myprecision,&scale)) != 0) {
	elog (NOTICE, 
	      "sybTableOpen: Error in Getting Column Information %i: %i",ii,i);
	sybTableErrorReport(stateInfo->statement, stateInfo->environ);
	done_with_default_result_sybTab((long int)stateInfo->statement, 
				     stateInfo->data,  
				     stateInfo->data_size, 
				     stateInfo->num_of_cols);
	disconnect_sybTab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
	close_env_sybTab( (long int) stateInfo->environ); 
	free(stateInfo); stateInfo = NULL;
	return(i);
      }
      truncate_end_blanks(MAX_TOKEN,mycolname);
      if((stateInfo->colName[j] =
	  (char *)malloc(sizeof(char) * (strlen(mycolname)+1)))  == NULL) {
	elog (NOTICE, "sybTableOpen: Malloc error");
	done_with_default_result_sybTab((long int)stateInfo->statement, 
				     stateInfo->data,  
				     stateInfo->data_size, 
				     stateInfo->num_of_cols);
	for (i = 0 ; i < j; i++) free(stateInfo->colName[j]);
	disconnect_sybTab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
	close_env_sybTab( (long int) stateInfo->environ); 
	free(stateInfo); stateInfo = NULL;
	return DB_ERR_MALLOC;
      }
      strcpy(stateInfo->colName[j], mycolname);
      stateInfo->displaysize[j] = displaysize;
      stateInfo->nullable[j] = nullable;
      stateInfo->precision[j] = myprecision;
      stateInfo->scale[j] = scale;

      getDataTypeFromMap_sybTab(stateInfo->coltype[j],coltype);
      
      jj =  strlen(stateInfo->colName[j]) + 
	    strlen(stateInfo->coltype[j]) +
	    strlen("COLNAME= COLTYPE= MAXSIZE= PRECISION= SCALE = ") +
	    5 * 5 + 3 * 40 + 1;
      if((stateInfo->xmlcolName[j] =
	  (char *)malloc(sizeof(char) * jj ))  == NULL) {
	elog (NOTICE, "sybTableOpen: Malloc error");
	done_with_default_result_sybTab((long int)stateInfo->statement, 
				     stateInfo->data,  
				     stateInfo->data_size, 
				     stateInfo->num_of_cols);
	for (i = 0 ; i < j; i++) {
	  free(stateInfo->colName[j]);
	  free(stateInfo->xmlcolName[j]);
	}
	disconnect_sybTab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
	close_env_sybTab( (long int) stateInfo->environ); 
	free(stateInfo); stateInfo = NULL;
	return DB_ERR_MALLOC;
      }
      sprintf (stateInfo->xmlcolName[j], "COLUMN COLNAME=\"%s\" COLTYPE=\"%s\" MAXSIZE=\"%li\" PRECISION=\"%li\" SCALE=\"%hi\"",
	       stateInfo->colName[j],stateInfo->coltype[j],
	       (long int)stateInfo->displaysize[j],  (long int)stateInfo->precision[j],
	       stateInfo->scale[j]);
    }


    tmpPtr = strdup(dataPathName);
    lower (tmpPtr);
    if ((tmpPtr1 = strstr(tmpPtr, " from ")) != NULL) {
      tmpPtr1 += 6;
      while (isspace(*tmpPtr1)) tmpPtr1++;
      if ((tmpPtr2 = strstr(tmpPtr1, " where ")) != NULL) 
	*tmpPtr2 = '\0';
      stateInfo->tableNameList = strdup(tmpPtr1);
      for ( ; *tmpPtr1 != '\0'; tmpPtr1++) 
	if (*tmpPtr1 == ',') stateInfo->num_of_tables++;
      stateInfo->num_of_tables++;
      addToVarList(&(stateInfo->constantValues), "TL",stateInfo->tableNameList, "CHAR");
      sprintf(tmpStr,"%i",stateInfo->num_of_tables);
      addToVarList(&(stateInfo->constantValues), "NT",tmpStr, "CHAR");
    }
    free(tmpPtr);
    stateInfo->num_of_cols = (int) numCols;
    sprintf(tmpStr,"%i",stateInfo->num_of_cols);
    addToVarList(&(stateInfo->constantValues), "NC",tmpStr, "CHAR");
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      stateInfo->lastData[i] = NULL;
      stateInfo->xmlcolNameOpen[i] = NULL;
      stateInfo->xmlcolNameClose[i] = NULL;
    }
    exfd->dbSpecificInfo = (char *)stateInfo; 
    return(MDAS_SUCCESS);
}



int 
sybTableClose(dbTableDesc  *exfd)
{
  tableStateInfo *stateInfo;
  int i,ii;

  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;

  /* transaction_end_code = 1 ABORT TRANSACTION */
  /* transaction_end_code = 0 COMMIT TRANSACTION */

  if (stateInfo->mm_size > 0)
    free(stateInfo->mm_ptr);
  if (stateInfo->tableNameList != NULL)
    free(stateInfo->tableNameList);

  if (!strcmp(stateInfo->sqlType, "SELECT") &&
      stateInfo->statement != NULL && 
      stateInfo->num_of_cols > 0) {
    if ((i = done_with_default_result_sybTab((long int)stateInfo->statement, 
		       stateInfo->data,  
		       stateInfo->data_size, stateInfo->num_of_cols)) != 0) {
      elog (NOTICE, "sybTableClose: Error in Unbinding and Closing %i",i);
      sybTableErrorReport(NULL, stateInfo->environ);
      return(i);
    }    
  }
  if (!strcmp(stateInfo->sqlType, "MULTIINSERT")) {
    	ii = appendAllUnConsumedSqlIntoAllocatedBuffer(
			       &(stateInfo->compSql),
			       &(stateInfo->storedSQLBuffer),
			       &(stateInfo->freestoredSQLBufferSize));
	if (ii < 0 ) {
	    elog (NOTICE, 
		  "sybTableClose: Error in Constructing SQL To Clean List: %i",ii);
	    return(ii);
	}
	if (ii == 1) {
	  if ((ii = executeCompoundSql(stateInfo, 
				       stateInfo->storedSQLBuffer)) < 0) {
	    return(ii);
	  }
	}
	free(stateInfo->storedSQLBuffer);
  }
  for (i = 0; i < stateInfo->num_of_cols ; i++){
    if (stateInfo->colName[i] != NULL)  free(stateInfo->colName[i]);
    if (stateInfo->xmlcolName[i] != NULL)  free(stateInfo->xmlcolName[i]);
    if (stateInfo->xmlcolNameOpen[i] != NULL) free(stateInfo->xmlcolNameOpen[i]);
    if (stateInfo->xmlcolNameClose[i] != NULL)  free(stateInfo->xmlcolNameClose[i]);
    if (stateInfo->lastData[i] != NULL) free(stateInfo->lastData[i]);
  }
  if (stateInfo->dataTemplateH != NULL) free(stateInfo->dataTemplateH);
  if (stateInfo->dataTemplateB != NULL) free(stateInfo->dataTemplateB);
  if (stateInfo->dataTemplateT != NULL) free(stateInfo->dataTemplateT);
  if (stateInfo->sql != NULL) free(stateInfo->sql);

  if (stateInfo->dataTemplateChoice.choiceCondition != NULL) 
    free(stateInfo->dataTemplateChoice.choiceCondition);
  for (i = 0; i < stateInfo->dataTemplateChoice.choiceCount ; i++) {
    free(stateInfo->dataTemplateChoice.choiceHead[i]);
    free(stateInfo->dataTemplateChoice.choiceBody[i]);
    free(stateInfo->dataTemplateChoice.choiceTail[i]); 
    if (stateInfo->dataTemplateChoice.compiledHeadRE[i] != NULL)
      free(stateInfo->dataTemplateChoice.compiledHeadRE[i]);
    if (stateInfo->dataTemplateChoice.compiledTailRE[i] != NULL)
      free(stateInfo->dataTemplateChoice.compiledTailRE[i]);
  }

  for (i = 0; i < stateInfo->definedValues.varCount; i++) {
    free(stateInfo->definedValues.varName[i]);
    free(stateInfo->definedValues.varValue[i]);
    free(stateInfo->definedValues.varType[i]);
  }
  for (i = 0; i < stateInfo->constantValues.varCount; i++) {
    free(stateInfo->constantValues.varName[i]);
    free(stateInfo->constantValues.varValue[i]);
    free(stateInfo->constantValues.varType[i]);

  }


  /* commit all transactions */

  if ((i = disconnect_sybTab( (long int)stateInfo->environ, 
			   (long int)stateInfo->connect, MDAS_CAT_COMMIT)) != 0) {
    elog (NOTICE, "sybTableClose: Error in Disconnecting:%i",i);
    sybTableErrorReport(NULL, stateInfo->environ);
    return(DB_TAB_DISCONNECT_ERROR);
  }
  if ((i = close_env_sybTab((long int)stateInfo->environ)) != 0) {
    elog (NOTICE, "sybTableClose: Error in Closing Environment:%i",i);
    sybTableErrorReport(NULL, stateInfo->environ);
    return(DB_TAB_CLOSE_ENV_ERROR);
  }
    
  free(stateInfo);
  return(MDAS_SUCCESS);
}/* srbclose */

int 
sybTableRead(dbTableDesc *exfd, char *buffer, int length) 
{
  tableStateInfo *stateInfo;
  int i;

  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  if (stateInfo->endOfData ==  1) {
    if (stateInfo->mm_size > 0) {
      memcpy(buffer, stateInfo->mm_ptr,stateInfo->mm_size);
      free(stateInfo->mm_ptr);
      i = stateInfo->mm_size;
      stateInfo->mm_size = 0;
      return(i);
    }
    else {
      return(0);
    }
  }

  if (!strcmp(stateInfo->dataType, "XMLREL"))
    return(sybTableReadXMLREL(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "XMLTREE"))
    return(sybTableReadXMLTREE(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "HTMLREL"))
    return(sybTableReadHTMLREL(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "HTMLTREE"))
    return(sybTableReadHTMLTREE(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "HTMLNEST"))
    return(sybTableReadHTMLNEST(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "TEMPLATE"))
    return(sybTableReadTEMPLATE(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "PACK"))
    return(sybTableReadPACK(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "SQL"))
    return(sybTableReadPACK(exfd,buffer,length));
  else
    return(sybTableReadPACK(exfd,buffer,length));
}

int 
sybTableReadTEMPLATE(dbTableDesc *exfd, char *buffer, int length)
{

  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj,tt;
  char * mybuf;
  char fillStr[HUGH_HUGE_STRING];

  strcpy(fillStr,"");
  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  mybuf  = buffer;
  ll = length;

  if (stateInfo->mm_size > 0) {
    memcpy(mybuf, stateInfo->mm_ptr,stateInfo->mm_size);
    free(stateInfo->mm_ptr);
    ll -= stateInfo->mm_size;
    mybuf += stateInfo->mm_size;
    stateInfo->mm_size = 0;
  }

  while ((i = get_row_sybTab((long int) stateInfo->statement)) == 0)       
    {
      stateInfo->num_of_rows_read++;
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	truncate_end_blanks(stateInfo->data_size[ii], stateInfo->data[ii]);
      }
      mapRowIntoStyleSheet(stateInfo,fillStr);
      k = strlen(fillStr);
      if (k <= ll) {
	  memcpy(mybuf, fillStr, k);
	  ll -= k;
	  mybuf += k;
	}
      else {     /* k > ll make a copy for later retrieval*/
	  memcpy(mybuf, fillStr,ll);
	  k = k - ll + 1;
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (fillStr + ll),k); 
	  return(length);
      }
    }

  stateInfo->endOfData = 1;
  if (i == MCAT_NO_DATA_FOUND) {
    mapRowIntoStyleSheet(stateInfo,fillStr);
    if (strlen(fillStr) <= ll) {
      strcpy(mybuf,fillStr);
      ll  -= strlen(fillStr);
      return(length - ll);
    }
    else {
      memcpy(mybuf, fillStr,ll);
      k = strlen(fillStr) - ll + 1;
      stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
      stateInfo->mm_size = k;
      mybuf = stateInfo->mm_ptr;
      memcpy(mybuf,(char *) (fillStr + ll),k); 
      return(length);      
    }
  }
  else { 
    if (stateInfo->mm_size > 0) 
      free(stateInfo->mm_ptr);
    return(i);
  }
}

int 
sybTableReadHTMLREL(dbTableDesc *exfd, char *buffer, int length)
{

  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj,tt;
  char * mybuf, *storPtr;
  char tmpStr[MAX_TOKEN], rowStr[MAX_TOKEN];
  char fillStr[HUGE_STRING];

  strcpy(fillStr,"");
  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  mybuf  = buffer;
  ll = length;
  if (stateInfo->num_of_rows_read == 0) {
    /* sending header information */
    strcpy(mybuf,"<HTML>\n<BODY BGCOLOR=#FFFFFF>\n<CENTER>\n<TABLE BORDER CELLSPACING=-5>\n<TR>\n");
    ll  -= strlen(mybuf);
    mybuf += strlen(mybuf);
    for (i = 0 ; i < stateInfo->num_of_cols ; i++) {
      sprintf(mybuf,"<TH>%s</TH>\n",stateInfo->colName[i]);
      mybuf += strlen(stateInfo->colName[i]) + 10;
      ll  -= strlen(stateInfo->colName[i]) + 10;
    }
    strcpy(mybuf,"</TR>\n");
    mybuf += strlen("</TR>\n");
    ll  -= strlen("</TR>\n");
  }
  if (stateInfo->mm_size > 0) {
    memcpy(mybuf, stateInfo->mm_ptr,stateInfo->mm_size);
    free(stateInfo->mm_ptr);
    ll -= stateInfo->mm_size;
    mybuf += stateInfo->mm_size;
    stateInfo->mm_size = 0;
  }
  if ((i = get_row_sybTab((long int) stateInfo->statement)) != 0) {
    stateInfo->endOfData = 1;
    if (i == MCAT_NO_DATA_FOUND){
      strcpy(fillStr,"</TABLE>\n</BODY>\n</HTML>\n");
      if (strlen(fillStr) <= ll) {
	strcpy(mybuf,fillStr);
	ll  -= strlen(fillStr);
	return(length - ll);
      }
      else {
	memcpy(mybuf, fillStr,ll);
	k = strlen(fillStr) - ll + 1;
	stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	stateInfo->mm_size = k;
	mybuf = stateInfo->mm_ptr;
	memcpy(mybuf,(char *) (fillStr + ll),k); 
	return(length);      
      }
    }
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
	if (ii == 0)
	  k += strlen("<TR>\n");
	k += strlen(stateInfo->data[ii]) + strlen("<TD></TD>\n");
	if (ii == stateInfo->num_of_cols - 1) 
	  k +=strlen("</TR>\n");
	tt  = 0;
	storPtr = (char *)malloc(sizeof(char) * k);
	if (ii == 0) {
	  sprintf(storPtr,"<TR>\n");
	  tt += strlen("<TR>\n");
	}
	sprintf(storPtr + tt ,"<TD>%s</TD>\n",stateInfo->data[ii]);
	tt  += strlen(stateInfo->data[ii]) + strlen("<TD></TD>\n");
	if (ii == stateInfo->num_of_cols - 1) {
	  sprintf(storPtr + tt,"</TR>\n");
	  tt += strlen("</TR>\n");
	}
	if (k <= ll) {
	  memcpy(mybuf, storPtr, k);
	  free(storPtr);
	  ll -= k;
	  mybuf += k;
	}
	else {     /* k > ll make a copy for later retrieval*/
	  memcpy(mybuf, storPtr,ll);
	  mybuf += ll;
	  k = k - ll + 1;
	  kk = k;
	  for (jj = ii+1 ; jj < stateInfo->num_of_cols ;jj++)
	    k += strlen(stateInfo->data[jj]) + strlen("<TD></TD>\n");
	  if (ii != stateInfo->num_of_cols - 1) 
	    k += strlen("</TR>\n");
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (storPtr + ll),kk); 
	  mybuf += kk;
	  free(storPtr);
	  for (jj = ii+1 ; jj < stateInfo->num_of_cols ;jj++) {
	    k  = strlen(stateInfo->data[jj]) + strlen("<TD></TD>\n");
	    storPtr = (char *)malloc(sizeof(char) * k);
	    tt =0;
	    sprintf(storPtr + tt, "<TD>%s</TD>\n", stateInfo->data[jj]);
	    tt  += strlen(stateInfo->data[jj]) + 10;
	    memcpy(mybuf,storPtr, k);
	    free(storPtr);
	    mybuf += k;
	  }
	  if (ii != stateInfo->num_of_cols - 1) {
	    strcpy(tmpStr,"</TR>\n");
	    memcpy(mybuf,tmpStr,strlen(tmpStr));
	  }
	  return(length);
	}
       }
      i = get_row_sybTab((long int) stateInfo->statement);
    }
  stateInfo->endOfData = 1;
  if (i == MCAT_NO_DATA_FOUND) {
    strcpy(fillStr,"</TABLE>\n</BODY>\n</HTML>\n");
    if (strlen(fillStr) <= ll) {
      strcpy(mybuf,fillStr);
      ll  -= strlen(fillStr);
      return(length - ll);
    }
    else {
	  memcpy(mybuf, fillStr,ll);
	  k = strlen(fillStr) - ll + 1;
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (fillStr + ll),k); 
	  return(length);      
    }
  }
  else 
    return(i);
}

int 
sybTableReadHTMLNEST(dbTableDesc *exfd, char *buffer, int length)
{

  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj,tt, lastColChanged;
  char * mybuf, *storPtr;
  char tmpStr[MAX_TOKEN], rowStr[MAX_TOKEN];
  char fillStr[HUGE_STRING];

  strcpy(fillStr,"");
  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  mybuf  = buffer;
  ll = length;
  if (stateInfo->num_of_rows_read == 0) {
    /* sending header information */
    strcpy(mybuf,"<HTML>\n<BODY BGCOLOR=#FFFFFF>\n<CENTER>\n<TABLE BORDER CELLSPACING=-5>\n<TR>\n");
    ll  -= strlen(mybuf);
    mybuf += strlen(mybuf);
    for (i = 0 ; i < stateInfo->num_of_cols ; i++) {
      sprintf(mybuf,"<TH>%s</TH>\n",stateInfo->colName[i]);
      mybuf += strlen(stateInfo->colName[i]) + 10;
      ll  -= strlen(stateInfo->colName[i]) + 10;
    }
    strcpy(mybuf,"</TR>\n");
    mybuf += strlen("</TR>\n");
    ll  -= strlen("</TR>\n");
  }
  if (stateInfo->mm_size > 0) {
    memcpy(mybuf, stateInfo->mm_ptr,stateInfo->mm_size);
    free(stateInfo->mm_ptr);
    ll -= stateInfo->mm_size;
    mybuf += stateInfo->mm_size;
    stateInfo->mm_size = 0;
  }
  if ((i = get_row_sybTab((long int) stateInfo->statement)) != 0) {
    stateInfo->endOfData = 1;
    if (i == MCAT_NO_DATA_FOUND){
      strcpy(fillStr,"</TABLE>\n</BODY>\n</HTML>\n");
      if (strlen(fillStr) <= ll) {
	strcpy(mybuf,fillStr);
	ll  -= strlen(fillStr);
	return(length - ll);
      }
      else {
	memcpy(mybuf, fillStr,ll);
	k = strlen(fillStr) - ll + 1;
	stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	stateInfo->mm_size = k;
	mybuf = stateInfo->mm_ptr;
	memcpy(mybuf,(char *) (fillStr + ll),k); 
	return(length);      
      }
    }
    else 
      return(i);
  }
  while (i == 0)       
    {
      stateInfo->num_of_rows_read++;
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	truncate_end_blanks(stateInfo->data_size[ii], stateInfo->data[ii]);
      }
      lastColChanged = 0;
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	if (stateInfo->lastData[ii] == NULL) {
	  stateInfo->lastData[ii] = strdup(stateInfo->data[ii]);
	  lastColChanged =  1;
	}
	else if (lastColChanged) {
	  free(stateInfo->lastData[ii]);
	  stateInfo->lastData[ii] = strdup(stateInfo->data[ii]);
	}
	else if (!strcmp(stateInfo->lastData[ii],stateInfo->data[ii])) {
	  strcpy(stateInfo->data[ii],"");
	}
	else {
	  free(stateInfo->lastData[ii]);
	  stateInfo->lastData[ii] = strdup(stateInfo->data[ii]);
	  lastColChanged =  1;
	}
      }
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	k = 0;
	if (ii == 0)
	  k += strlen("<TR>\n");
	k += strlen(stateInfo->data[ii]) + strlen("<TD></TD>\n");
	if (ii == stateInfo->num_of_cols - 1) 
	  k +=strlen("</TR>\n");
	tt  = 0;
	storPtr = (char *)malloc(sizeof(char) * k);
	if (ii == 0) {
	  sprintf(storPtr,"<TR>\n");
	  tt += strlen("<TR>\n");
	}
	sprintf(storPtr + tt ,"<TD>%s</TD>\n",stateInfo->data[ii]);
	tt  += strlen(stateInfo->data[ii]) + strlen("<TD></TD>\n");
	if (ii == stateInfo->num_of_cols - 1) {
	  sprintf(storPtr + tt,"</TR>\n");
	  tt += strlen("</TR>\n");
	}
	if (k <= ll) {
	  memcpy(mybuf, storPtr, k);
	  free(storPtr);
	  ll -= k;
	  mybuf += k;
	}
	else {     /* k > ll make a copy for later retrieval*/
	  memcpy(mybuf, storPtr,ll);
	  mybuf += ll;
	  k = k - ll + 1;
	  kk = k;
	  for (jj = ii+1 ; jj < stateInfo->num_of_cols ;jj++)
	    k += strlen(stateInfo->data[jj]) + strlen("<TD></TD>\n");
	  if (ii != stateInfo->num_of_cols - 1) 
	    k += strlen("</TR>\n");
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (storPtr + ll),kk); 
	  mybuf += kk;
	  free(storPtr);
	  for (jj = ii+1 ; jj < stateInfo->num_of_cols ;jj++) {
	    k  = strlen(stateInfo->data[jj]) + strlen("<TD></TD>\n");
	    storPtr = (char *)malloc(sizeof(char) * k);
	    tt =0;
	    sprintf(storPtr + tt, "<TD>%s</TD>\n", stateInfo->data[jj]);
	    tt  += strlen(stateInfo->data[jj]) + 10;
	    memcpy(mybuf,storPtr, k);
	    free(storPtr);
	    mybuf += k;
	  }
	  if (ii != stateInfo->num_of_cols - 1) {
	    strcpy(tmpStr,"</TR>\n");
	    memcpy(mybuf,tmpStr,strlen(tmpStr));
	  }
	  return(length);
	}
       }
      i = get_row_sybTab((long int) stateInfo->statement);
    }
  stateInfo->endOfData = 1;
  if (i == MCAT_NO_DATA_FOUND) {
    strcpy(fillStr,"</TABLE>\n</BODY>\n</HTML>\n");
    if (strlen(fillStr) <= ll) {
      strcpy(mybuf,fillStr);
      ll  -= strlen(fillStr);
      return(length - ll);
    }
    else {
	  memcpy(mybuf, fillStr,ll);
	  k = strlen(fillStr) - ll + 1;
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (fillStr + ll),k); 
	  return(length);      
    }
  }
  else 
    return(i);
}

int 
sybTableReadHTMLTREE(dbTableDesc *exfd, char *buffer, int length)
{

  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj,tt, lastColChanged;
  char * mybuf, *storPtr;
  char tmpStr[MAX_TOKEN], rowStr[MAX_TOKEN];
  int showVal[MAX_TABLE_COLS];
  char fillStr[HUGE_STRING];

  strcpy(fillStr,"");
  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  mybuf  = buffer;
  ll = length;
  if (stateInfo->num_of_rows_read == 0) {
    /* sending header information */
    strcpy(mybuf,"<HTML>\n<BODY BGCOLOR=#FFFFFF>\n<CENTER>\n<TABLE BORDER CELLSPACING=-5>\n<TR>\n");
    ll  -= strlen(mybuf);
    mybuf += strlen(mybuf);
    for (i = 0 ; i < stateInfo->num_of_cols ; i++) {
      sprintf(mybuf,"<TH>%s</TH>\n",stateInfo->colName[i]);
      mybuf += strlen(stateInfo->colName[i]) + 10;
      ll  -= strlen(stateInfo->colName[i]) + 10;
    }
    strcpy(mybuf,"</TR>\n");
    mybuf += strlen("</TR>\n");
    ll  -= strlen("</TR>\n");
  }
  if (stateInfo->mm_size > 0) {
    memcpy(mybuf, stateInfo->mm_ptr,stateInfo->mm_size);
    free(stateInfo->mm_ptr);
    ll -= stateInfo->mm_size;
    mybuf += stateInfo->mm_size;
    stateInfo->mm_size = 0;
  }
  if ((i = get_row_sybTab((long int) stateInfo->statement)) != 0) {
    stateInfo->endOfData = 1;
    if (i == MCAT_NO_DATA_FOUND){
      strcpy(fillStr,"</TABLE>\n</BODY>\n</HTML>\n");
      if (strlen(fillStr) <= ll) {
	strcpy(mybuf,fillStr);
	ll  -= strlen(fillStr);
	return(length - ll);
      }
      else {
	memcpy(mybuf, fillStr,ll);
	k = strlen(fillStr) - ll + 1;
	stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	stateInfo->mm_size = k;
	mybuf = stateInfo->mm_ptr;
	memcpy(mybuf,(char *) (fillStr + ll),k); 
	return(length);      
      }
    }
    else 
      return(i);
  }
  while (i == 0)       
    {
      stateInfo->num_of_rows_read++;
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	truncate_end_blanks(stateInfo->data_size[ii], stateInfo->data[ii]);
	showVal[ii] = 0;
      }
      lastColChanged = 0;
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	if (stateInfo->lastData[ii] == NULL) {
	  stateInfo->lastData[ii] = strdup(stateInfo->data[ii]);
	  lastColChanged =  1;
	  showVal[ii] = 1;
	}
	else if (lastColChanged) {
	  free(stateInfo->lastData[ii]);
	  stateInfo->lastData[ii] = strdup(stateInfo->data[ii]);
	  showVal[ii] = 1;
	}
	else if (!strcmp(stateInfo->lastData[ii],stateInfo->data[ii])) {
	  strcpy(stateInfo->data[ii],"");
	}
	else {
	  free(stateInfo->lastData[ii]);
	  stateInfo->lastData[ii] = strdup(stateInfo->data[ii]);
	  lastColChanged =  1;
	  showVal[ii] = 1;
	}
      }
      k = stateInfo->num_of_cols * 
	        (strlen("<TR>\n") + strlen("</TR>\n") +
	             (stateInfo->num_of_cols * strlen("<TD></TD>\n"))) ;
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	k += strlen(stateInfo->data[ii]);
      } 
      storPtr = (char *)malloc(sizeof(char) * k);
      storPtr[0] =  '\0';
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	if (showVal[ii]) {
	  strcat(storPtr,"<TR>\n");
	  for (jj = 0; jj < stateInfo->num_of_cols ; jj++) {
	    if (ii != jj) 
	      strcat(storPtr,"<TD></TD>\n");
	    else {
	      strcat(storPtr,"<TD>");
	      strcat(storPtr,stateInfo->data[jj]);
	      strcat(storPtr,"</TD>\n");
	    }
	  }
	  strcat(storPtr,"</TR>\n");
	}
      }
      k = strlen(storPtr);
      if (k <= ll) {
	memcpy(mybuf, storPtr, k);
	free(storPtr);
	ll -= k;
	mybuf += k;
      }
      else {     /* k > ll make a copy for later retrieval*/
	  memcpy(mybuf, storPtr,ll);
	  k = k - ll + 1;
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (storPtr + ll),k); 
	  free(storPtr);
	  return(length);
      }
      i = get_row_sybTab((long int) stateInfo->statement);
    }
  stateInfo->endOfData = 1;
  if (i == MCAT_NO_DATA_FOUND) {
    strcpy(fillStr,"</TABLE>\n</BODY>\n</HTML>\n");
    if (strlen(fillStr) <= ll) {
      strcpy(mybuf,fillStr);
      ll  -= strlen(fillStr);
      return(length - ll);
    }
    else {
	  memcpy(mybuf, fillStr,ll);
	  k = strlen(fillStr) - ll + 1;
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (fillStr + ll),k); 
	  return(length);      
    }
  }
  else 
    return(i);
}


int 
sybTableReadPACK(dbTableDesc *exfd, char *buffer, int length)
{

  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj;
  char * mybuf;

  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  mybuf  = buffer;
  ll = length;
  if (stateInfo->mm_size > 0) {
    memcpy(mybuf, stateInfo->mm_ptr,stateInfo->mm_size);
    free(stateInfo->mm_ptr);
    ll -= stateInfo->mm_size;
    mybuf += stateInfo->mm_size;
    stateInfo->mm_size = 0;
  }
  if ((i = get_row_sybTab((long int) stateInfo->statement)) != 0) {
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
      i = get_row_sybTab((long int) stateInfo->statement);
    }    
  stateInfo->endOfData = 1;
  if (i == MCAT_NO_DATA_FOUND) 
    return(length - ll);
  else 
    return(i);
}

int 
sybTableReadHEAD(dbTableDesc *exfd, char *buffer, int length)
{

  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj;
  char * mybuf;

  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
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
     for (i = 0 ; i < stateInfo->num_of_cols ; i++) {
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
  if ((i = get_row_sybTab((long int) stateInfo->statement)) != 0) {
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
      i = get_row_sybTab((long int) stateInfo->statement);
    }    
  stateInfo->endOfData = 1;
  if (i == MCAT_NO_DATA_FOUND) 
    return(length - ll);
  else 
    return(i);
}

int 
sybTableReadXMLTREE(dbTableDesc *exfd, char *buffer, int length)
{

  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj,tt, lastColChanged, maxColHead;
  char * mybuf, *storPtr;
  char fillStr[HUGE_STRING];
  char tmpStr[MAX_TOKEN], rowStr[MAX_TOKEN];
  int showVal[MAX_TABLE_COLS];

  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  mybuf  = buffer;
  ll = length;
  strcpy(fillStr,"");
  if (stateInfo->num_of_rows_read == 0) {
    /* sending header information */
    sprintf(mybuf,  "<?xml version=\"1.0\"?>\n<TABLE_RESULT>\n<TABLE_NAME>\n%s\n</TABLE_NAME>\n<TABLE_DATA>\n",stateInfo->tableNameList);
    ll  -= strlen(mybuf);
    mybuf += strlen(mybuf);
  }
  if (stateInfo->mm_size > 0) {
    memcpy(mybuf, stateInfo->mm_ptr,stateInfo->mm_size);
    free(stateInfo->mm_ptr);
    ll -= stateInfo->mm_size;
    mybuf += stateInfo->mm_size;
    stateInfo->mm_size = 0;
  }
  if ((i = get_row_sybTab((long int) stateInfo->statement)) != 0) {
    stateInfo->endOfData = 1;
    if (stateInfo->num_of_rows_read > 0) {
      for (jj = stateInfo->num_of_cols-1  ; jj >= 0  ; jj--) {
	strcat(fillStr,"</");
	strcat(fillStr,(char *)replaceBlankByUnderscore(stateInfo->colName[jj],tmpStr));
	strcat(fillStr,">\n");
      }
    }
    strcat(fillStr,"</TABLE_DATA>\n</TABLE_RESULT>\n");
    if (i == MCAT_NO_DATA_FOUND){
      if (strlen(fillStr) <= ll) {
	strcpy(mybuf,fillStr);
	ll  -= strlen(fillStr);
	return(length - ll);
      }
      else {
	memcpy(mybuf, fillStr,ll);
	k = strlen(fillStr) - ll + 1;
	stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	stateInfo->mm_size = k;
	mybuf = stateInfo->mm_ptr;
	memcpy(mybuf,(char *) (fillStr + ll),k); 
	return(length);      
      }
    }
    else 
      return(i);
  }
  maxColHead = 0;
  for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
    if (maxColHead < strlen(stateInfo->colName[ii]))
      maxColHead = strlen(stateInfo->colName[ii]);
  }
  while (i == 0)       
    {
      stateInfo->num_of_rows_read++;
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	truncate_end_blanks(stateInfo->data_size[ii], stateInfo->data[ii]);
	showVal[ii] = 0;
      }
      lastColChanged = 0;
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	if (stateInfo->lastData[ii] == NULL) {
	  stateInfo->lastData[ii] = strdup(stateInfo->data[ii]);
	  lastColChanged =  1;
	  showVal[ii] = 1;
	}
	else if (lastColChanged) {
	  free(stateInfo->lastData[ii]);
	  stateInfo->lastData[ii] = strdup(stateInfo->data[ii]);
	  showVal[ii] = 1;
	}
	else if (!strcmp(stateInfo->lastData[ii],stateInfo->data[ii])) {
	  strcpy(stateInfo->data[ii],"");
	}
	else {
	  free(stateInfo->lastData[ii]);
	  stateInfo->lastData[ii] = strdup(stateInfo->data[ii]);
	  lastColChanged =  1;
	  showVal[ii] = 1;
	}
      }
      k = stateInfo->num_of_cols * 
	        ( (2 * maxColHead)  + 8 + (2 * strlen("<data>\n")) + 1);
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	k += strlen(stateInfo->data[ii]);
      } 
      storPtr = (char *)malloc(sizeof(char) * k);
      storPtr[0] =  '\0';
      for (ii =  0; ii < stateInfo->num_of_cols ; ii++) {
	if (showVal[ii]) {
	  if ((ii > 0 && showVal[ii-1] == 0) ||
	      (ii == 0 && stateInfo->num_of_rows_read > 1)){
	    for (jj = stateInfo->num_of_cols-1  ; jj >= ii  ; jj--) {
	      strcat(storPtr,"</");
	      strcat(storPtr,(char *)replaceBlankByUnderscore(stateInfo->colName[jj],tmpStr));
	      strcat(storPtr,">\n");
	    }
	  }
	  strcat(storPtr,"<");
	  strcat(storPtr,(char *)replaceBlankByUnderscore(stateInfo->colName[ii],tmpStr));
	  strcat(storPtr,">\n<data>");
	  strcat(storPtr,stateInfo->data[ii]);
	  strcat(storPtr,"</data>\n");
	}
      }
      k = strlen(storPtr);
      if (k <= ll) {
	memcpy(mybuf, storPtr, k);
	free(storPtr);
	ll -= k;
	mybuf += k;
      }
      else {     /* k > ll make a copy for later retrieval*/
	  memcpy(mybuf, storPtr,ll);
	  k = k - ll + 1;
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (storPtr + ll),k); 
	  free(storPtr);
	  return(length);
      }
      i = get_row_sybTab((long int) stateInfo->statement);
    }
  stateInfo->endOfData = 1;
  if (i == MCAT_NO_DATA_FOUND) {
    if (stateInfo->num_of_rows_read > 0) {
      for (jj = stateInfo->num_of_cols-1  ; jj >= 0  ; jj--) {
	strcat(fillStr,"</");
	strcat(fillStr,(char *)replaceBlankByUnderscore(stateInfo->colName[jj],tmpStr));
	strcat(fillStr,">\n");
      }
    }
    strcat(fillStr,"</TABLE_DATA>\n</TABLE_RESULT>\n");
    if (strlen(fillStr) <= ll) {
      strcpy(mybuf,fillStr);
      ll  -= strlen(fillStr);
      return(length - ll);
    }
    else {
	  memcpy(mybuf, fillStr,ll);
	  k = strlen(fillStr) - ll + 1;
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (fillStr + ll),k); 
	  return(length);      
    }
  }
  else 
    return(i);
}


int 
sybTableReadXMLREL(dbTableDesc *exfd, char *buffer, int length)
{

  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj,tt;
  char * mybuf, *storPtr;
  char tmpStr[MAX_TOKEN], rowStr[MAX_TOKEN];
  char fillStr[HUGE_STRING];

  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  mybuf  = buffer;
  ll = length;
  strcpy(fillStr,"");
  if (stateInfo->num_of_rows_read == 0) {
    /* sending header information */
    strcpy(mybuf,"<?xml version=\"1.0\"?>\n");
    mybuf += strlen("<?xml version=\"1.0\"?>")+1;
    ll  -= strlen("<?xml version=\"1.0\"?>")+1;
    strcpy(mybuf,"<TABLE_RESULT>\n");
    mybuf += strlen("<TABLE_RESULT>")+1;
    ll  -= strlen("<TABLE_RESULT>")+1;
    strcpy(mybuf,"<TABLE_NAME>\n");
    mybuf += strlen("<TABLE_NAME>")+1;
    ll  -= strlen("<TABLE_NAME>")+1;
    strcpy(mybuf,stateInfo->tableNameList);
    mybuf += strlen(stateInfo->tableNameList);
    ll  -= strlen(stateInfo->tableNameList);
    strcpy(mybuf,"\n</TABLE_NAME>\n");
    mybuf += strlen("</TABLE_NAME>")+2;
    ll  -= strlen("</TABLE_NAME>")+2;
    strcpy(mybuf,"<TABLE_DATA>\n");
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
  if ((i = get_row_sybTab((long int) stateInfo->statement)) != 0) {
    stateInfo->endOfData = 1;
    strcat(fillStr,"</TABLE_DATA>\n</TABLE_RESULT>\n");
    if (i == MCAT_NO_DATA_FOUND){
      if (strlen(fillStr) <= ll) {
	strcpy(mybuf,fillStr);
	ll  -= strlen(fillStr);
	return(length - ll);
      }
      else {
	memcpy(mybuf, fillStr,ll);
	k = strlen(fillStr) - ll + 1;
	stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	stateInfo->mm_size = k;
	mybuf = stateInfo->mm_ptr;
	memcpy(mybuf,(char *) (fillStr + ll),k); 
	return(length);      
      }
    }
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
	  sprintf(rowStr,"<ROW ROWNUM=\"%i\">\n",stateInfo->num_of_rows_read);
	  k += strlen(rowStr) ;
	}
	k  += (strlen(stateInfo->xmlcolName[ii]) + 2 +
	       strlen("COLUMN") + 4 +
	       strlen(stateInfo->data[ii]) ); 
	if (ii == stateInfo->num_of_cols - 1) 
	  k +=strlen("</ROW>") + 1;
	tt  = 0;
	storPtr = (char *)malloc(sizeof(char) * k);
	if (ii == 0) {
	  sprintf(storPtr,rowStr);
	  tt += strlen(rowStr);
	}
	sprintf(storPtr + tt ,"<%s>",stateInfo->xmlcolName[ii]);
	tt += strlen(stateInfo->xmlcolName[ii]) + 2;
	sprintf(storPtr + tt , stateInfo->data[ii]);
	tt  += strlen(stateInfo->data[ii]) ;
	sprintf(storPtr + tt, "</COLUMN>\n");
	tt  += strlen("</COLUMN>") + 1;
	if (ii == stateInfo->num_of_cols - 1) {
	  sprintf(storPtr + tt,"</ROW>\n");
	  tt += strlen("</ROW>") + 1;
	}
	if (k <= ll) {
	  memcpy(mybuf, storPtr, k);
	  free(storPtr);
	  ll -= k;
	  mybuf += k;
	}
	else {     /* k > ll make a copy for later retrieval*/
	  memcpy(mybuf, storPtr,ll);
	  mybuf += ll;
	  k = k - ll + 1;
	  kk = k;
	  for (jj = ii+1 ; jj < stateInfo->num_of_cols ;jj++)
	    k += (strlen(stateInfo->xmlcolName[jj]) + 2 +
		  strlen("COLUMN") + 4 +
		  strlen(stateInfo->data[jj]) );	
	  if (ii != stateInfo->num_of_cols - 1) 
	    k += strlen("</ROW>") + 1;
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (storPtr + ll),kk); 
	  mybuf += kk;
	  free(storPtr);
	  for (jj = ii+1 ; jj < stateInfo->num_of_cols ;jj++) {
	    k  = (strlen(stateInfo->xmlcolName[jj]) + 2 +
		  strlen("COLUMN") + 4 +
		  strlen(stateInfo->data[jj]) );
	    storPtr = (char *)malloc(sizeof(char) * k);
	    tt =0;
	    sprintf(storPtr,"<%s>",stateInfo->xmlcolName[jj]);
	    tt += strlen(stateInfo->xmlcolName[jj]) + 2;
	    sprintf(storPtr + tt, "%s", stateInfo->data[jj]);
	    tt  += strlen(stateInfo->data[jj]) ;
	    sprintf(storPtr + tt, "</COLUMN>\n");
	    tt  += strlen("</COLUMN>") + 1;
	    memcpy(mybuf,storPtr, k);
	    free(storPtr);
	    mybuf += k;
	  }
	  if (ii != stateInfo->num_of_cols - 1) {
	    strcpy(tmpStr,"</ROW>\n");
	    memcpy(mybuf,tmpStr,strlen(tmpStr) + 1);
	  }
	  return(length);
	}
       }
      i = get_row_sybTab((long int) stateInfo->statement);
    }
  stateInfo->endOfData = 1;
  if (i == MCAT_NO_DATA_FOUND) {
    strcpy(fillStr,"</TABLE_DATA>\n</TABLE_RESULT>\n");
    if (strlen(fillStr) <= ll) {
      strcpy(mybuf,fillStr);
      ll  -= strlen(fillStr);
      return(length - ll);
    }
    else {
	  memcpy(mybuf, fillStr,ll);
	  k = strlen(fillStr) - ll + 1;
	  stateInfo->mm_ptr =  (char *)malloc(sizeof(char) * k);
	  stateInfo->mm_size = k;
	  mybuf = stateInfo->mm_ptr;
	  memcpy(mybuf,(char *) (fillStr + ll),k); 
	  return(length);      
    }
  }
  else 
    return(i);
}


int 
sybTableWrite(dbTableDesc *exfd, char *buffer, int length) 
{
/*
 Insert or Update into an existing table object
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         char *sqlInsertString - Format : insert or update statement
 *
 */
  tableStateInfo *stateInfo;
  int i;

  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;

  if (!strcmp(stateInfo->dataType, "XMLREL"))
    return(sybTableWriteXMLREL(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "XMLTREE"))
    return(sybTableWriteXMLTREE(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "HTMLREL"))
    return(sybTableWriteHTMLREL(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "HTMLTREE"))
    return(sybTableWriteHTMLTREE(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "HTMLNEST"))
    return(sybTableWriteHTMLNEST(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "TEMPLATE"))
    return(sybTableWriteTEMPLATE(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "SQL"))
    return(sybTableWriteSQL(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "PACK"))
    return(sybTableWritePACK(exfd,buffer,length));
  else if (!strcmp(stateInfo->dataType, "VALUES"))
    return(sybTableWriteVALUES(exfd,buffer,length));
  else
    return(sybTableWriteSQL(exfd,buffer,length));
}

int 
sybTableWriteSQL(dbTableDesc *exfd, char *sqlInsertString, int length) 
{
  tableStateInfo *stateInfo;
  int i;
  char *mybuf, *mybuf1, *mybuf2;
  char tmpStr[MAX_TOKEN], rowStr[MAX_TOKEN];

  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  mybuf = malloc(length+1);

  
  if (mybuf == NULL)  {
    elog (NOTICE, "sybTableWrite: Memory Allocation Error");
    return(DB_ERR_MALLOC);
    }
  memcpy(mybuf, sqlInsertString, length);
  mybuf[length] = '\0';
  convCtrlCharsToBlanks(mybuf);
  if ((i=executeCompoundSql(stateInfo,
				  (unsigned char *) mybuf)) != 0)
    {
      free(mybuf);
      return(i);
    }
  free(mybuf);
  return(length);
}

int 
sybTableWriteVALUES(dbTableDesc *exfd, char *sqlInsertString, int length) 
{
 tableStateInfo *stateInfo;
 int i, ii,j,k;
 char *mybuf, *mybuf1, *mybuf2, *tmpmmPtr;
 char myChar;
 char tmpStr[MAX_TOKEN], rowStr[MAX_TOKEN];
 char *sqlString;
 char *collapsePtr;
 char *endOfBuffer;
 int mmSize;

  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  tmpmmPtr = stateInfo->mm_ptr;
  mmSize  = stateInfo->mm_size;
  collapsePtr = NULL;
  
  if (stateInfo->mm_size > 0 ) {
    collapsePtr = (char *) malloc(mmSize + length);
    if (collapsePtr == NULL)  {
      elog (NOTICE, "sybTableWrite: Memory Allocation Error");
      return(DB_ERR_MALLOC);
    }
    memcpy(collapsePtr,stateInfo->mm_ptr, mmSize-1);
    memcpy(collapsePtr+ mmSize - 1, sqlInsertString,length);
    *(collapsePtr + mmSize - 1 + length) = '\0';
    mybuf = collapsePtr;
    free(stateInfo->mm_ptr);
    stateInfo->mm_size = 0;
    endOfBuffer = collapsePtr + mmSize-2 + length;
  }
  else {
    mybuf = malloc(length + 1);
    if (mybuf == NULL)  {
      elog (NOTICE, "sybTableWrite: Memory Allocation Error");
      return(DB_ERR_MALLOC);
    }
    memcpy(mybuf, sqlInsertString, length);
    mybuf[length] = '\0';
    endOfBuffer = mybuf + length - 1;
  }
  convCtrlCharsToBlanks(mybuf);
  mybuf1 = mybuf;
  trimsides(mybuf1);

 if (!strcmp(stateInfo->sqlType,"MULTIINSERT")) {
  while (*mybuf1 != '\0') {
    for (i =  0; i < stateInfo->num_of_cols ;i++) {
      if ((mybuf2 = (char *) getEndOfValue(mybuf1)) == NULL) {
	tmpmmPtr = malloc(strlen(mybuf1) + 1);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = strlen(mybuf1) + 1;
	memcpy(tmpmmPtr, mybuf1, strlen(mybuf1) + 1);
	break;
      }
      *mybuf2 = '\0';
      stateInfo->data[i] = mybuf1;
      mybuf1 = mybuf2 + 1;
    }
    for (i = 0 ; i < stateInfo->compSql.sqlCount ; i++) {
      for (j = 0 ;  j < stateInfo->num_of_cols ; j++) {
	for (k = 0; k < MAX_SQL_TAGS_COUNT ; k++) {
	  if (stateInfo->compSql.tagusage[i][k] == TAG_BAD)
	    break;
	  if (atoi(stateInfo->compSql.tags[i][k]) == j) {
	    stateInfo->compSql.tagusage[i][k] = TAG_FULL;
	    stateInfo->compSql.dataValue[i][k] = stateInfo->data[j];
	  }
	}
      }
    }


    if ((ii = appendAllUnConsumedSqlIntoAllocatedBuffer(
					  &(stateInfo->compSql),
			       &(stateInfo->storedSQLBuffer),
			       &(stateInfo->freestoredSQLBufferSize))) < 0) {
      free(mybuf);
      return(ii);
    }
    if ((ii = executeCompoundSql(stateInfo, 
				 stateInfo->storedSQLBuffer)) < 0) {
      free(mybuf);
      return(ii);
    }
    stateInfo->freestoredSQLBufferSize += 
      strlen(stateInfo->storedSQLBuffer); 
    stateInfo->storedSQLBuffer[0]  = '\0';
  }
  free(mybuf);
  return(length);
 }
 else {
  while (*mybuf1 != '\0') {
    if ((mybuf2 = (char *) getEndOfStatement(mybuf1)) == NULL) {
      tmpmmPtr = malloc(strlen(mybuf1) + 1);
      stateInfo->mm_ptr = tmpmmPtr;
      stateInfo->mm_size = strlen(mybuf1) + 1;
      memcpy(tmpmmPtr, mybuf1, strlen(mybuf1) + 1);
      break;
    }
    myChar = *mybuf2;
    *mybuf2 = '\0';
    sqlString =  (char *) malloc(strlen(stateInfo->sql) +
				 strlen(mybuf1) + 20);
    if (*mybuf1 != '(') 
      sprintf(sqlString, "%s VALUES ( %s )", stateInfo->sql,mybuf1);
    else
      sprintf(sqlString, "%s VALUES %s ",stateInfo->sql,mybuf1);
      
    if ((i=exec_no_res_sql_sybTab(stateInfo->environ,stateInfo->connect,
				  (unsigned char *) sqlString))!= 0){ 
      elog (NOTICE, "sybTableWrite: Error in Executing SQL %s: %i",
	    sqlString,i);
      free(mybuf);
      free(sqlString);
      return(i);
    }
    free(sqlString);
    if (myChar == '\0')
      break;
    mybuf1 = mybuf2 + 1;
    while (isspace(*mybuf1)) mybuf1++;
  }
  free(mybuf);
  return(length);
 }
}

int 
sybTableWritePACK(dbTableDesc *exfd, char *buffer, int length) 
{
  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj;
  int cntData, mmSize, bufSize;
  char *sqlString;
  char *mybuf, *tmpPtr, *tmpPtr1, *tmpPtr2, *tmpmmPtr, *localbuffer;
  char *collapsePtr;


  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  cntData = 0;
  localbuffer = NULL;

  tmpmmPtr = stateInfo->mm_ptr;
  mmSize  = stateInfo->mm_size;
  mybuf = buffer;
  bufSize = length;

  if (mmSize > 0) {
    while ((mmSize > 0) &&
	   (tmpPtr = memchr(tmpmmPtr,'\0',mmSize)) != NULL){
      stateInfo->data[cntData] = tmpmmPtr;
      stateInfo->data_size[cntData++] = tmpPtr - tmpmmPtr + 1;
      mmSize -= tmpPtr - tmpmmPtr + 1;
      if (mmSize > 0) 
	tmpmmPtr = tmpPtr+1;
      else {
	free(stateInfo->mm_ptr);
	stateInfo->mm_size = 0;
      }
    }
    if (mmSize > 0) {
      if ((tmpPtr = memchr(mybuf,'\0',bufSize)) != NULL ) {
	localbuffer = (char *) malloc(mmSize + tmpPtr - mybuf + 1);
	memcpy(localbuffer,tmpmmPtr,mmSize);
	memcpy(localbuffer+mmSize, mybuf,tmpPtr - mybuf + 1);
	free(stateInfo->mm_ptr);
	stateInfo->mm_size = 0;
	stateInfo->data[cntData] = localbuffer;
	stateInfo->data_size[cntData++] = tmpPtr - mybuf + 1;
	bufSize -= tmpPtr - mybuf + 1;
	if (bufSize > 0) mybuf = tmpPtr + 1;
      }
      else {
	tmpPtr = (char *) malloc(stateInfo->mm_size + length);
	memcpy(tmpPtr,stateInfo->mm_ptr, stateInfo->mm_size);
	memcpy(tmpPtr+stateInfo->mm_size,buffer,length);
	free(stateInfo->mm_ptr);
	stateInfo->mm_ptr = tmpPtr;
	stateInfo->mm_size += length;
	return(length);
      }
    }
  }
  while (1) {
    for (; cntData < stateInfo->num_of_cols ; cntData++) {
      if ((tmpPtr = memchr(mybuf,'\0',bufSize)) == NULL ) {
	/* end of buffer reached before numOfCols */
	mmSize = 0;
	for (i = 0; i < cntData; i++) 
	  mmSize += stateInfo->data_size[i];
	mmSize += bufSize;
	tmpmmPtr = malloc(mmSize);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = mmSize;
	for (i = 0; i < cntData; i++) {
	  memcpy(tmpmmPtr,stateInfo->data[i],stateInfo->data_size[i]);
	  tmpmmPtr += stateInfo->data_size[i];
	}
	memcpy(tmpmmPtr,mybuf,bufSize);
	if (localbuffer != NULL) free(localbuffer);
	localbuffer = NULL;
	return(length);
      }
      stateInfo->data[cntData] = mybuf;
      stateInfo->data_size[cntData] = tmpPtr - mybuf + 1;
      bufSize -= tmpPtr - mybuf + 1;
      if (bufSize > 0) mybuf = tmpPtr + 1;
    }

    cntData = 0;
    mmSize = 0;
   if (!strcmp(stateInfo->sqlType,"MULTIINSERT")) {
    for (i = 0 ; i < stateInfo->compSql.sqlCount ; i++) {
      for (j = 0 ;  j < stateInfo->num_of_cols ; j++) {
	for (k = 0; k < MAX_SQL_TAGS_COUNT ; k++) {
	  if (stateInfo->compSql.tagusage[i][k] == TAG_BAD)
	    break;
	  if (atoi(stateInfo->compSql.tags[i][k]) == j) {
	    stateInfo->compSql.tagusage[i][k] = TAG_FULL;
	    stateInfo->compSql.dataValue[i][k] = stateInfo->data[j];
	  }
	}
      }
    }


    if ((ii = appendAllUnConsumedSqlIntoAllocatedBuffer(
					  &(stateInfo->compSql),
			       &(stateInfo->storedSQLBuffer),
			       &(stateInfo->freestoredSQLBufferSize))) < 0) {
      return(ii);
    }
    if ((ii = executeCompoundSql(stateInfo, 
				 stateInfo->storedSQLBuffer)) < 0) {
      return(ii);
    }
    stateInfo->freestoredSQLBufferSize += 
      strlen(stateInfo->storedSQLBuffer); 
    stateInfo->storedSQLBuffer[0]  = '\0';
    if (bufSize == 0) 
      break;
   }
   else {
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (strstr(stateInfo->data[i],"'") == NULL)
	mmSize += stateInfo->data_size[i] + 3;
      else 
	mmSize += (2 * stateInfo->data_size[i]) + 3;
    }
    sqlString =  (char *) malloc(strlen(stateInfo->sql) +
				 mmSize + 20);
    sprintf(sqlString, "%s VALUES ( ", stateInfo->sql);
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (i != 0) strcat(sqlString,",");
      tmpPtr = stateInfo->data[i];
      while (isspace(*tmpPtr)) tmpPtr++;
      if (strstr(stateInfo->coltype[i],"CHAR") != NULL &&
	  *tmpPtr != '\'') {
	strcat(sqlString,"'");
	if (strstr(tmpPtr,"'") == NULL)
	  strcat(sqlString,tmpPtr);
	else {
	  tmpPtr2 = sqlString + strlen(sqlString);
	  while (*tmpPtr != '\0') {
	    if (*tmpPtr == '\'') *tmpPtr2++ = '\'';
	    *tmpPtr2++ = *tmpPtr++;
	  }
	  *tmpPtr2 = '\0';
	}
	strcat(sqlString,"'");
      }
      else {
	strcat(sqlString,stateInfo->data[i]);
      }
    }
    strcat(sqlString,")");
    if (localbuffer != NULL) free(localbuffer);
    localbuffer = NULL;
    if ((i=exec_no_res_sql_sybTab(stateInfo->environ,stateInfo->connect,
				  (unsigned char *) sqlString))!= 0){ 
      elog (NOTICE, "sybTableWrite: Error in Executing SQL %s: %i",
	    mybuf,i);
      free(sqlString);
      return(i);
    }
    free(sqlString);
    if (bufSize == 0) 
      break;
   }
  }
  return(length);
}


int 
sybTableWriteHTMLREL(dbTableDesc *exfd, char *buffer, int length) 
{
  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj;
  int cntData, mmSize, bufSize;
  char *sqlString;
  char *mybuf, *tmpPtr, *tmpPtr1, *tmpPtr2, *tmpmmPtr, *tmpmmPtr2, *localbuffer;
  char *collapsePtr;
  char *endOfBuffer;

  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  cntData = 0;
  localbuffer = NULL;
  tmpmmPtr = stateInfo->mm_ptr;
  mmSize  = stateInfo->mm_size;
  mybuf = buffer;
  bufSize = length;
  collapsePtr = NULL;
  endOfBuffer = buffer+length -1;

  if (stateInfo->mm_size > 0 ) {
    collapsePtr = (char *) malloc(mmSize + bufSize);
    memcpy(collapsePtr,stateInfo->mm_ptr, stateInfo->mm_size-1);
    memcpy(collapsePtr+stateInfo->mm_size-1,buffer,length+1);
    mybuf = collapsePtr;
    free(stateInfo->mm_ptr);
    stateInfo->mm_size = 0;
    endOfBuffer = collapsePtr + mmSize-2 + length;
  }

  while (1) {
    for (; cntData < stateInfo->num_of_cols ; cntData++) {

      if (((tmpPtr = strstr(mybuf,"<TD>")) == NULL) ||
	  (tmpPtr+3 > endOfBuffer)) {
	/* end of buffer reached before numOfCols */
	mmSize = 0;
	for (i = 0; i < cntData; i++) 
	  mmSize += stateInfo->data_size[i] -1 + strlen("<TD></TD>");
	mmSize += endOfBuffer - mybuf + 1 + 1;
	tmpmmPtr = malloc(mmSize);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = mmSize;
	*tmpmmPtr =  '\0';
	for (i = 0; i < cntData; i++) {
	  strcat(tmpmmPtr,"<TD>");
	  strcat(tmpmmPtr,stateInfo->data[i]);
	  strcat(tmpmmPtr,"</TD>");
	}
	*(tmpmmPtr + strlen(tmpmmPtr) + (endOfBuffer - mybuf + 1) ) =  '\0';
	memcpy((tmpmmPtr + strlen(tmpmmPtr)), 
	       mybuf, endOfBuffer - mybuf + 1);
	if (localbuffer != NULL) free(localbuffer);
	localbuffer = NULL;
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(length);
      }
      mybuf = tmpPtr + 4;
      if (((tmpPtr = strstr(mybuf,"</TD>")) != NULL) &&
	  (tmpPtr+4 <= endOfBuffer)){
	*tmpPtr = '\0';
	stateInfo->data[cntData] = mybuf;
	stateInfo->data_size[cntData] = tmpPtr - mybuf + 1;
	/****	*tmpPtr = '<'; ****/
	mybuf = tmpPtr + 1;
      }
      else {
	/* end of buffer reached before end of data */
	mmSize = 0;
	for (i = 0; i < cntData; i++) 
	  mmSize += stateInfo->data_size[i] -1 + strlen("<TD></TD>");
	mmSize += endOfBuffer - mybuf + 1 + strlen("<TD>") + 1;
	tmpmmPtr = malloc(mmSize);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = mmSize;
	*tmpmmPtr =  '\0';
	for (i = 0; i < cntData; i++) {
	  strcat(tmpmmPtr,"<TD>");
	  strcat(tmpmmPtr,stateInfo->data[i]);
	  strcat(tmpmmPtr,"</TD>");
	}
	strcat(tmpmmPtr,"<TD>");
	*(tmpmmPtr + strlen(tmpmmPtr) + (endOfBuffer - mybuf + 1) ) =  '\0';
	memcpy((tmpmmPtr + strlen(tmpmmPtr)), 
	       mybuf, endOfBuffer - mybuf + 1);
	if (localbuffer != NULL) free(localbuffer);
	localbuffer = NULL;
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(length);
      }
    }

    cntData = 0;
    mmSize = 0;


   if (!strcmp(stateInfo->sqlType,"MULTIINSERT")) {
    for (i = 0 ; i < stateInfo->compSql.sqlCount ; i++) {
      for (j = 0 ;  j < stateInfo->num_of_cols ; j++) {
	for (k = 0; k < MAX_SQL_TAGS_COUNT ; k++) {
	  if (stateInfo->compSql.tagusage[i][k] == TAG_BAD)
	    break;
	  if (atoi(stateInfo->compSql.tags[i][k]) == j) {
	    stateInfo->compSql.tagusage[i][k] = TAG_FULL;
	    stateInfo->compSql.dataValue[i][k] = stateInfo->data[j];
	  }
	}
      }
    }

    if ((ii = appendAllUnConsumedSqlIntoAllocatedBuffer(
					  &(stateInfo->compSql),
			       &(stateInfo->storedSQLBuffer),
			       &(stateInfo->freestoredSQLBufferSize))) < 0) {
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      return(ii);
    }
    if ((ii = executeCompoundSql(stateInfo, 
				 stateInfo->storedSQLBuffer)) < 0) {
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      return(ii);
    }
    stateInfo->freestoredSQLBufferSize += 
      strlen(stateInfo->storedSQLBuffer); 
    stateInfo->storedSQLBuffer[0]  = '\0';
    if (bufSize == 0) 
      break;
   }
   else {
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (strstr(stateInfo->data[i],"'") == NULL)
	mmSize += stateInfo->data_size[i] + 3;
      else 
	mmSize += (2 * stateInfo->data_size[i]) + 3;
    }
    sqlString =  (char *) malloc(strlen(stateInfo->sql) +
				 mmSize + 20);
    sprintf(sqlString, "%s VALUES ( ", stateInfo->sql);
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (i != 0) strcat(sqlString,",");
      tmpPtr = stateInfo->data[i];
      while (isspace(*tmpPtr)) tmpPtr++;
      if (strstr(stateInfo->coltype[i],"CHAR") != NULL &&
	  *tmpPtr != '\'') {
	strcat(sqlString,"'");
	if (strstr(tmpPtr,"'") == NULL)
	  strcat(sqlString,tmpPtr);
	else {
	  tmpPtr2 = sqlString + strlen(sqlString);
	  while (*tmpPtr != '\0') {
	    if (*tmpPtr == '\'') *tmpPtr2++ = '\'';
	    *tmpPtr2++ = *tmpPtr++;
	  }
	  *tmpPtr2 = '\0';
	}
	strcat(sqlString,"'");
      }
      else {
	strcat(sqlString,stateInfo->data[i]);
      }
    }
    strcat(sqlString,")");
    if (localbuffer != NULL) free(localbuffer);
    localbuffer = NULL;
    if ((i=exec_no_res_sql_sybTab(stateInfo->environ,stateInfo->connect,
				  (unsigned char *) sqlString))!= 0){ 
      elog (NOTICE, "sybTableWrite: Error in Executing SQL %s: %i",
	    mybuf,i);
      free(sqlString);
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      return(i);
    }
    free(sqlString);
    if (bufSize == 0) 
      break;
   }
  }
  if (collapsePtr != NULL) free(collapsePtr);
  collapsePtr = NULL;
  return(length);

}

int
sybTableWriteHTMLTREE(dbTableDesc *exfd, char *buffer, int length) 
{
  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj;
  int cntData, mmSize, bufSize;
  char *sqlString;
  char *mybuf, *tmpPtr, *tmpPtr1, *tmpPtr2, *tmpmmPtr, *tmpmmPtr2, *localbuffer;
  int emptyVal;
  char *collapsePtr;
  char *endOfBuffer;

  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  cntData = 0;
  localbuffer = NULL;
  tmpmmPtr = stateInfo->mm_ptr;
  mmSize  = stateInfo->mm_size;
  mybuf = buffer;
  bufSize = length;
  collapsePtr = NULL;
  endOfBuffer = buffer+length -1;

  if (stateInfo->mm_size > 0 ) {
    collapsePtr = (char *) malloc(mmSize + bufSize);
    memcpy(collapsePtr,stateInfo->mm_ptr, stateInfo->mm_size-1);
    memcpy(collapsePtr+stateInfo->mm_size-1,buffer,length+1);
    mybuf = collapsePtr;
    free(stateInfo->mm_ptr);
    stateInfo->mm_size = 0;
    endOfBuffer = collapsePtr + mmSize-2 + length;
  }


  while (1) {
    for (; cntData < stateInfo->num_of_cols ; cntData++) {

      if (((tmpPtr = strstr(mybuf,"<TD>")) == NULL) ||
	  (tmpPtr+3 > endOfBuffer)) {
	/* end of buffer reached before numOfCols */
	mmSize = 0;
	for (i = 0; i < cntData; i++) 
	  mmSize += stateInfo->data_size[i] -1 + strlen("<TD></TD>");
	mmSize += endOfBuffer - mybuf + 1 + 1;
	tmpmmPtr = malloc(mmSize);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = mmSize;
	*tmpmmPtr =  '\0';
	for (i = 0; i < cntData; i++) {
	  strcat(tmpmmPtr,"<TD>");
	  strcat(tmpmmPtr,stateInfo->data[i]);
	  strcat(tmpmmPtr,"</TD>");
	}
	*(tmpmmPtr + strlen(tmpmmPtr) + (endOfBuffer - mybuf + 1) ) =  '\0';
	memcpy((tmpmmPtr + strlen(tmpmmPtr)), 
	       mybuf, endOfBuffer - mybuf + 1);
	if (localbuffer != NULL) free(localbuffer);
	localbuffer = NULL;
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(length);
      }
      mybuf = tmpPtr + 4;
      if (((tmpPtr = strstr(mybuf,"</TD>")) != NULL) &&
	  (tmpPtr+4 <= endOfBuffer)) {
	*tmpPtr = '\0';
	stateInfo->data[cntData] = mybuf;
	stateInfo->data_size[cntData] = tmpPtr - mybuf + 1;
	/****   *tmpPtr = '<'; ****/
	mybuf = tmpPtr + 1;
      }
      else {
	/* end of buffer reached before end of data */
	mmSize = 0;
	for (i = 0; i < cntData; i++) 
	  mmSize += stateInfo->data_size[i] -1 + strlen("<TD></TD>");
	mmSize += endOfBuffer - mybuf + 1 + strlen("<TD>") + 1;
	tmpmmPtr = malloc(mmSize);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = mmSize;
	*tmpmmPtr =  '\0';
	for (i = 0; i < cntData; i++) {
	  strcat(tmpmmPtr,"<TD>");
	  strcat(tmpmmPtr,stateInfo->data[i]);
	  strcat(tmpmmPtr,"</TD>");
	}
	strcat(tmpmmPtr,"<TD>");
	*(tmpmmPtr + strlen(tmpmmPtr) + (endOfBuffer - mybuf + 1) ) =  '\0';
	memcpy((tmpmmPtr + strlen(tmpmmPtr)), 
	       mybuf, endOfBuffer - mybuf + 1);
	if (localbuffer != NULL) free(localbuffer);
	localbuffer = NULL;
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(length);
      }
    }
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (strlen(stateInfo->data[i]) != 0) {
	if (stateInfo->lastData[i] != NULL) 
	  free(stateInfo->lastData[i]);
	stateInfo->lastData[i] = strdup(stateInfo->data[i]);
      }
    }

    cntData = 0;
    mmSize = 0;
    if (localbuffer != NULL) free(localbuffer);
    localbuffer = NULL;
    emptyVal = 0;
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (stateInfo->lastData[i] == NULL) {
	emptyVal = 1;
	break;
      }
    }
    if (emptyVal == 1) 
      continue; /** do the while(1) since not all data is collected**/

    for (i = 0; i < stateInfo->num_of_cols; i++) {
      stateInfo->data[i] = stateInfo->lastData[i];
      stateInfo->data_size[i] =  strlen(stateInfo->data[i]);
    }


   if (!strcmp(stateInfo->sqlType,"MULTIINSERT")) {
    for (i = 0 ; i < stateInfo->compSql.sqlCount ; i++) {
      for (j = 0 ;  j < stateInfo->num_of_cols ; j++) {
	for (k = 0; k < MAX_SQL_TAGS_COUNT ; k++) {
	  if (stateInfo->compSql.tagusage[i][k] == TAG_BAD)
	    break;
	  if (atoi(stateInfo->compSql.tags[i][k]) == j) {
	    stateInfo->compSql.tagusage[i][k] = TAG_FULL;
	    stateInfo->compSql.dataValue[i][k] = stateInfo->data[j];
	  }
	}
      }
    }

    if ((ii = appendAllUnConsumedSqlIntoAllocatedBuffer(
					  &(stateInfo->compSql),
			       &(stateInfo->storedSQLBuffer),
			       &(stateInfo->freestoredSQLBufferSize))) < 0) {
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      return(ii);
    }
    if ((ii = executeCompoundSql(stateInfo, 
				 stateInfo->storedSQLBuffer)) < 0) {
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      return(ii);
    }
    stateInfo->freestoredSQLBufferSize += 
      strlen(stateInfo->storedSQLBuffer); 
    stateInfo->storedSQLBuffer[0]  = '\0';
    if (bufSize == 0) 
      break;
   }
   else {

    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (strstr(stateInfo->data[i],"'") == NULL)
	mmSize += stateInfo->data_size[i] + 3;
      else 
	mmSize += (2 * stateInfo->data_size[i]) + 3;
    }
    sqlString =  (char *) malloc(strlen(stateInfo->sql) +
				 mmSize + 20);
    sprintf(sqlString, "%s VALUES ( ", stateInfo->sql);
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (i != 0) strcat(sqlString,",");
      tmpPtr = stateInfo->data[i];
      while (isspace(*tmpPtr)) tmpPtr++;
      if (strstr(stateInfo->coltype[i],"CHAR") != NULL &&
	  *tmpPtr != '\'') {
	strcat(sqlString,"'");
	if (strstr(tmpPtr,"'") == NULL)
	  strcat(sqlString,tmpPtr);
	else {
	  tmpPtr2 = sqlString + strlen(sqlString);
	  while (*tmpPtr != '\0') {
	    if (*tmpPtr == '\'') *tmpPtr2++ = '\'';
	    *tmpPtr2++ = *tmpPtr++;
	  }
	  *tmpPtr2 = '\0';
	}
	strcat(sqlString,"'");
      }
      else {
	strcat(sqlString,stateInfo->data[i]);
      }
    }
    strcat(sqlString,")");
    if (localbuffer != NULL) free(localbuffer);
    localbuffer = NULL;
    if (stateInfo->lastData[stateInfo->num_of_cols-1] != NULL) 
      free(stateInfo->lastData[stateInfo->num_of_cols-1]);
    stateInfo->lastData[stateInfo->num_of_cols-1] =  NULL;

    if ((i=exec_no_res_sql_sybTab(stateInfo->environ,stateInfo->connect,
				  (unsigned char *) sqlString))!= 0){ 
      elog (NOTICE, "sybTableWrite: Error in Executing SQL %s: %i",
	    mybuf,i);
      free(sqlString);
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      return(i);
    }
    free(sqlString);
    if (bufSize == 0) 
      break;
   }
  }
  if (collapsePtr != NULL) free(collapsePtr);
  collapsePtr = NULL;
  return(length);
}

int 
sybTableWriteHTMLNEST(dbTableDesc *exfd, char *buffer, int length) 
{
  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj;
  int cntData, mmSize, bufSize, nofilluptohere;
  char *sqlString;
  char *mybuf, *tmpPtr, *tmpPtr1, *tmpPtr2, *tmpmmPtr, *tmpmmPtr2, *localbuffer;
  int emptyVal;
  char *collapsePtr;
  char *endOfBuffer;


  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  cntData = 0;
  nofilluptohere = 1;
  localbuffer = NULL;
  tmpmmPtr = stateInfo->mm_ptr;
  mmSize  = stateInfo->mm_size;
  mybuf = buffer;
  bufSize = length;
  collapsePtr = NULL;
  endOfBuffer = buffer+length -1;

  if (stateInfo->mm_size > 0 ) {
    collapsePtr = (char *) malloc(mmSize + bufSize);
    memcpy(collapsePtr,stateInfo->mm_ptr, stateInfo->mm_size-1);
    memcpy(collapsePtr+stateInfo->mm_size-1,buffer,length+1);
    mybuf = collapsePtr;
    free(stateInfo->mm_ptr);
    stateInfo->mm_size = 0;
    endOfBuffer = collapsePtr + mmSize-2 + length;
  }


  while (1) {
    for (; cntData < stateInfo->num_of_cols ; cntData++) {

      if (((tmpPtr = strstr(mybuf,"<TD>")) == NULL) ||
	  (tmpPtr+3 > endOfBuffer)) {
	/* end of buffer reached before numOfCols */
	mmSize = 0;
	for (i = 0; i < cntData; i++) 
	  mmSize += stateInfo->data_size[i] -1 + strlen("<TD></TD>");
	mmSize += endOfBuffer - mybuf + 1  + 1;
	tmpmmPtr = malloc(mmSize);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = mmSize;
	*tmpmmPtr =  '\0';
	for (i = 0; i < cntData; i++) {
	  strcat(tmpmmPtr,"<TD>");
	  strcat(tmpmmPtr,stateInfo->data[i]);
	  strcat(tmpmmPtr,"</TD>");
	}
	*(tmpmmPtr + strlen(tmpmmPtr) + (endOfBuffer - mybuf + 1) ) =  '\0';
	memcpy((tmpmmPtr + strlen(tmpmmPtr)), 
	       mybuf, endOfBuffer - mybuf + 1);
	if (localbuffer != NULL) free(localbuffer);
	localbuffer = NULL;
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(length);
      }
      mybuf = tmpPtr + 4;
      if (((tmpPtr = strstr(mybuf,"</TD>")) != NULL) &&
	  (tmpPtr+4 <= endOfBuffer)) {
	*tmpPtr = '\0';
	stateInfo->data[cntData] = mybuf;
	stateInfo->data_size[cntData] = tmpPtr - mybuf + 1;
	/****   *tmpPtr = '<'; ****/
	mybuf = tmpPtr + 1;
      }
      else {
	/* end of buffer reached before end of data */
	mmSize = 0;
	for (i = 0; i < cntData; i++) 
	  mmSize += stateInfo->data_size[i] -1 + strlen("<TD></TD>");
	mmSize += endOfBuffer - mybuf + 1 + strlen("<TD>") + 1;
	tmpmmPtr = malloc(mmSize);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = mmSize;
	*tmpmmPtr =  '\0';
	for (i = 0; i < cntData; i++) {
	  strcat(tmpmmPtr,"<TD>");
	  strcat(tmpmmPtr,stateInfo->data[i]);
	  strcat(tmpmmPtr,"</TD>");
	}
	strcat(tmpmmPtr,"<TD>");
	*(tmpmmPtr + strlen(tmpmmPtr) + (endOfBuffer - mybuf + 1) ) =  '\0';
	memcpy((tmpmmPtr + strlen(tmpmmPtr)), 
	       mybuf, endOfBuffer - mybuf + 1);
	if (localbuffer != NULL) free(localbuffer);
	localbuffer = NULL;
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(length);
      }
    }

    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (strlen(stateInfo->data[i]) == 0 && nofilluptohere) {
	stateInfo->data[i] = stateInfo->lastData[i];
	stateInfo->data_size[i] =  strlen(stateInfo->lastData[i]) + 1;
      }
      else {
	if (stateInfo->lastData[i] != NULL) 
	  free(stateInfo->lastData[i]);
	stateInfo->lastData[i] = strdup(stateInfo->data[i]);
	nofilluptohere = 0;
      }
    }

    cntData = 0;
    nofilluptohere = 1;
    mmSize = 0;
    if (localbuffer != NULL) free(localbuffer);
    localbuffer = NULL;

   if (!strcmp(stateInfo->sqlType,"MULTIINSERT")) {
    for (i = 0 ; i < stateInfo->compSql.sqlCount ; i++) {
      for (j = 0 ;  j < stateInfo->num_of_cols ; j++) {
	for (k = 0; k < MAX_SQL_TAGS_COUNT ; k++) {
	  if (stateInfo->compSql.tagusage[i][k] == TAG_BAD)
	    break;
	  if (atoi(stateInfo->compSql.tags[i][k]) == j) {
	    stateInfo->compSql.tagusage[i][k] = TAG_FULL;
	    stateInfo->compSql.dataValue[i][k] = stateInfo->data[j];
	  }
	}
      }
    }
    if ((ii = appendAllUnConsumedSqlIntoAllocatedBuffer(
					  &(stateInfo->compSql),
			       &(stateInfo->storedSQLBuffer),
			       &(stateInfo->freestoredSQLBufferSize))) < 0) {
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      return(ii);
    }
    if ((ii = executeCompoundSql(stateInfo, 
				 stateInfo->storedSQLBuffer)) < 0) {
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      return(ii);
    }
    stateInfo->freestoredSQLBufferSize += 
      strlen(stateInfo->storedSQLBuffer); 
    stateInfo->storedSQLBuffer[0]  = '\0';
    if (bufSize == 0) 
      break;
   }
   else {
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (strstr(stateInfo->data[i],"'") == NULL)
	mmSize += stateInfo->data_size[i] + 3;
      else 
	mmSize += (2 * stateInfo->data_size[i]) + 3;
    }
    sqlString =  (char *) malloc(strlen(stateInfo->sql) +
				 mmSize + 20);
    sprintf(sqlString, "%s VALUES ( ", stateInfo->sql);
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (i != 0) strcat(sqlString,",");
      tmpPtr = stateInfo->data[i];
      while (isspace(*tmpPtr)) tmpPtr++;
      if (strstr(stateInfo->coltype[i],"CHAR") != NULL &&
	  *tmpPtr != '\'') {
	strcat(sqlString,"'");
	if (strstr(tmpPtr,"'") == NULL)
	  strcat(sqlString,tmpPtr);
	else {
	  tmpPtr2 = sqlString + strlen(sqlString);
	  while (*tmpPtr != '\0') {
	    if (*tmpPtr == '\'') *tmpPtr2++ = '\'';
	    *tmpPtr2++ = *tmpPtr++;
	  }
	  *tmpPtr2 = '\0';
	}
	strcat(sqlString,"'");
      }
      else {
	strcat(sqlString,stateInfo->data[i]);
      }
    }
    strcat(sqlString,")");
    if (localbuffer != NULL) free(localbuffer);
    localbuffer = NULL;
    
    if ((i=exec_no_res_sql_sybTab(stateInfo->environ,stateInfo->connect,
				  (unsigned char *) sqlString))!= 0){ 
      elog (NOTICE, "sybTableWrite: Error in Executing SQL %s: %i",
	    mybuf,i);
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      free(sqlString);
      return(i);
    }
    free(sqlString);
    if (bufSize == 0) 
      break;
   }
  }
  if (collapsePtr != NULL) free(collapsePtr);
  collapsePtr = NULL;
  return(length);

}



int 
sybTableWriteXMLREL(dbTableDesc *exfd, char *buffer, int length)
{
  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj;
  int cntData, mmSize, bufSize;
  char *sqlString;
  char *mybuf, *tmpPtr, *tmpPtr1, *tmpPtr2, *tmpPtr3, *tmpmmPtr, *tmpmmPtr2, *localbuffer;
  char *collapsePtr;
  char *endOfBuffer;


  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  cntData = 0;
  localbuffer = NULL;
  tmpmmPtr = stateInfo->mm_ptr;
  mmSize  = stateInfo->mm_size;
  mybuf = buffer;
  bufSize = length;
  collapsePtr = NULL;
  endOfBuffer = buffer+length -1;


  if (stateInfo->mm_size > 0 ) {
    collapsePtr = (char *) malloc(mmSize + bufSize);
    memcpy(collapsePtr,stateInfo->mm_ptr, stateInfo->mm_size-1);
    memcpy(collapsePtr+stateInfo->mm_size-1,buffer,length+1);
    mybuf = collapsePtr;
    free(stateInfo->mm_ptr);
    stateInfo->mm_size = 0;
    endOfBuffer = collapsePtr + mmSize-2 + length;
  }

  while (1) {
    for (; cntData < stateInfo->num_of_cols ; cntData++) {

      if (((tmpPtr3 = strstr(mybuf,"<COLUMN")) == NULL) ||
	  ((tmpPtr  = strstr(tmpPtr3,">")) == NULL) ||
	  (tmpPtr > endOfBuffer)) {
	/* end of buffer reached before numOfCols */
	mmSize = 0;
	for (i = 0; i < cntData; i++) 
	  mmSize += stateInfo->data_size[i] -1 + strlen("<COLUMN></COLUMN>");
	mmSize += endOfBuffer - mybuf + 1  + 1;
	tmpmmPtr = malloc(mmSize);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = mmSize;
	*tmpmmPtr =  '\0';
	for (i = 0; i < cntData; i++) {
	  strcat(tmpmmPtr,"<COLUMN>");
	  strcat(tmpmmPtr,stateInfo->data[i]);
	  strcat(tmpmmPtr,"</COLUMN>");
	}
	*(tmpmmPtr + strlen(tmpmmPtr) + (endOfBuffer - mybuf + 1) ) =  '\0';
	memcpy((tmpmmPtr + strlen(tmpmmPtr)), 
	       mybuf, endOfBuffer - mybuf + 1);
	if (localbuffer != NULL) free(localbuffer);
	localbuffer = NULL;
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(length);
      }
      
      mybuf = tmpPtr + 1;
      if (((tmpPtr = strstr(mybuf,"</COLUMN>")) != NULL) &&
	  (tmpPtr+8 <= endOfBuffer)) {
	*tmpPtr = '\0';
	stateInfo->data[cntData] = mybuf;
	stateInfo->data_size[cntData] = tmpPtr - mybuf + 1;
	/****	*tmpPtr = '<'; ****/
	mybuf = tmpPtr + 1;
      }
      else {
	/* end of buffer reached before end of data */
	mmSize = 0;
	for (i = 0; i < cntData; i++) 
	  mmSize += stateInfo->data_size[i] -1 + strlen("<COLUMN></COLUMN>");
	mmSize += endOfBuffer - mybuf + 1  + strlen("<COLUMN>") + 1;
	tmpmmPtr = malloc(mmSize);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = mmSize;
	*tmpmmPtr =  '\0';
	for (i = 0; i < cntData; i++) {
	  strcat(tmpmmPtr,"<COLUMN>");
	  strcat(tmpmmPtr,stateInfo->data[i]);
	  strcat(tmpmmPtr,"</COLUMN>");
	}
	strcat(tmpmmPtr,"<COLUMN>");
	*(tmpmmPtr + strlen(tmpmmPtr) + (endOfBuffer - mybuf + 1) ) =  '\0';
	memcpy((tmpmmPtr + strlen(tmpmmPtr)), 
	       mybuf, endOfBuffer - mybuf + 1);
	if (localbuffer != NULL) free(localbuffer);
	localbuffer = NULL;
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(length);
      }
    }

    cntData = 0;
    mmSize = 0;

   if (!strcmp(stateInfo->sqlType,"MULTIINSERT")) {
    for (i = 0 ; i < stateInfo->compSql.sqlCount ; i++) {
      for (j = 0 ;  j < stateInfo->num_of_cols ; j++) {
	for (k = 0; k < MAX_SQL_TAGS_COUNT ; k++) {
	  if (stateInfo->compSql.tagusage[i][k] == TAG_BAD)
	    break;
	  if (atoi(stateInfo->compSql.tags[i][k]) == j) {
	    stateInfo->compSql.tagusage[i][k] = TAG_FULL;
	    stateInfo->compSql.dataValue[i][k] = stateInfo->data[j];
	  }
	}
      }
    }

    if ((ii = appendAllUnConsumedSqlIntoAllocatedBuffer(
					  &(stateInfo->compSql),
			       &(stateInfo->storedSQLBuffer),
			       &(stateInfo->freestoredSQLBufferSize))) < 0) {
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      return(ii);
    }
    if ((ii = executeCompoundSql(stateInfo, 
				 stateInfo->storedSQLBuffer)) < 0) {
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      return(ii);
    }
    stateInfo->freestoredSQLBufferSize += 
      strlen(stateInfo->storedSQLBuffer); 
    stateInfo->storedSQLBuffer[0]  = '\0';
    if (bufSize == 0) 
      break;
   }
   else {
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (strstr(stateInfo->data[i],"'") == NULL)
	mmSize += stateInfo->data_size[i] + 3;
      else 
	mmSize += (2 * stateInfo->data_size[i]) + 3;
    }
    sqlString =  (char *) malloc(strlen(stateInfo->sql) +
				 mmSize + 20);
    sprintf(sqlString, "%s VALUES ( ", stateInfo->sql);
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (i != 0) strcat(sqlString,",");
      tmpPtr = stateInfo->data[i];
      while (isspace(*tmpPtr)) tmpPtr++;
      if (strstr(stateInfo->coltype[i],"CHAR") != NULL &&
	  *tmpPtr != '\'') {
	strcat(sqlString,"'");
	if (strstr(tmpPtr,"'") == NULL)
	  strcat(sqlString,tmpPtr);
	else {
	  tmpPtr2 = sqlString + strlen(sqlString);
	  while (*tmpPtr != '\0') {
	    if (*tmpPtr == '\'') *tmpPtr2++ = '\'';
	    *tmpPtr2++ = *tmpPtr++;
	  }
	  *tmpPtr2 = '\0';
	}
	strcat(sqlString,"'");
      }
      else {
	strcat(sqlString,stateInfo->data[i]);
      }
    }
    strcat(sqlString,")");
    if (localbuffer != NULL) free(localbuffer);
    localbuffer = NULL;
    if ((i=exec_no_res_sql_sybTab(stateInfo->environ,stateInfo->connect,
				  (unsigned char *) sqlString))!= 0){ 
      elog (NOTICE, "sybTableWrite: Error in Executing SQL %s: %i",
	    mybuf,i);
      free(sqlString);
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;

      return(i);
    }
    free(sqlString);
    if (bufSize == 0) 
      break;
   }
  }
  if (collapsePtr != NULL) free(collapsePtr);
  collapsePtr = NULL;
  return(length);
}

int
getNextDataFromXMLTREE(char **buf, char **dataTagOpen, char **dataTagClose, 
		       char **dataValue, int length)
{
  char *mybuf, *tmpPtr, *tmpPtr1, *tmpPtr2;
  int i,j, k;

  mybuf = *buf;

  if ((tmpPtr = strstr(mybuf,"<data>")) == NULL)
    return (-1);
  if (tmpPtr+5 > (mybuf + length - 1)) 
    return (-1);
  if ((tmpPtr1 = strstr(tmpPtr,"</data>")) == NULL)
    return (-1);
  if (tmpPtr1+6 > (mybuf + length - 1)) 
    return (-1);
  for ( tmpPtr2 =  tmpPtr - 1  ; tmpPtr2 >= mybuf ; tmpPtr2--) 
    if (*tmpPtr2 == '<') break;
  if (tmpPtr2 < mybuf)
    return (DATA_TAG_ERROR);

  *tmpPtr1 = '\0';
  *(tmpPtr + 5) = '\0';
  *(tmpPtr1 + 6) = '\0';
  *dataValue = tmpPtr + 6;
  *buf = tmpPtr1 + 7;
  *dataTagOpen = tmpPtr2+1;
  *dataTagClose = tmpPtr1 + 1;
  return (MDAS_SUCCESS);
}

int 
sybTableWriteXMLTREE(dbTableDesc *exfd, char *inbuffer, int length)
{
  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj;
  int cntData, mmSize, bufSize;
  char *sqlString;
  char *mybuf, *tmpPtr, *tmpPtr1, *tmpPtr2, *tmpmmPtr, *tmpmmPtr2, *localbuffer;
  char *collapsePtr, *buffer,  *dataTagOpen, *dataTagClose, *dataValue;
  char emptyStr[2];
  int newValInsert;


  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  cntData = 0;
  tmpmmPtr = stateInfo->mm_ptr;
  mmSize  = stateInfo->mm_size;
  mybuf = inbuffer;
  buffer = inbuffer;
  bufSize = length;
  collapsePtr = NULL;
  strcpy(emptyStr,"");

  if (stateInfo->mm_size > 0 ) {
    collapsePtr = (char *) malloc(mmSize + bufSize);
    memcpy(collapsePtr,stateInfo->mm_ptr, mmSize - 1);
    memcpy(collapsePtr+stateInfo->mm_size-1,inbuffer,bufSize);
    *(collapsePtr + mmSize + bufSize -1 ) = '\0';
    mybuf = collapsePtr;
    free(stateInfo->mm_ptr);
    stateInfo->mm_size = 0;
    buffer = collapsePtr;
    bufSize = (mmSize - 1  + length);
    
  }

  while (1) {
    for (cntData = 0 ; cntData < stateInfo->num_of_cols ; cntData++) {
      
      i = getNextDataFromXMLTREE(&mybuf,&dataTagOpen,&dataTagClose,&dataValue,
				 bufSize - (mybuf -  buffer));
      
      if (i == DATA_TAG_ERROR) {
	elog (NOTICE, "sybTableWrite: Error in Data Tag for Buffer: %s", mybuf);
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(DATA_TAG_ERROR);
      }
      if (i != 0 ) {
	mmSize = 0;
	for (i = 0; i < cntData; i++) 
	  mmSize += stateInfo->data_size[i] -1 + 
	    strlen(stateInfo->xmlcolNameOpen[i]) + 
	    strlen(stateInfo->xmlcolNameClose[i]) + 
	    strlen("<><>");

	mmSize += bufSize - (mybuf - buffer)  + 1;
	tmpmmPtr = malloc(mmSize);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = mmSize;
	*tmpmmPtr =  '\0';
	for (i = 0; i < cntData; i++) {
	  strcat(tmpmmPtr,"<");
	  strcat(tmpmmPtr,stateInfo->xmlcolNameOpen[i]);
	  strcat(tmpmmPtr,">");
	  strcat(tmpmmPtr,stateInfo->data[i]);
	  strcat(tmpmmPtr,"<");
	  strcat(tmpmmPtr,stateInfo->xmlcolNameClose[i]);
	  strcat(tmpmmPtr,">");
	}
	memcpy(tmpmmPtr + strlen(tmpmmPtr), mybuf, buffer + bufSize - mybuf );
	*(tmpmmPtr+mmSize - 1 ) =  '\0';
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(length);
      }
      for (i  = 0;  i < stateInfo->num_of_cols; i++) {
	if (stateInfo->xmlcolNameOpen[i] == NULL)
	  break;
	if (!strcmp(dataTagOpen,stateInfo->xmlcolNameOpen[i])) 
	  break;
      }
      if (i == stateInfo->num_of_cols) {
	elog (NOTICE, "sybTableWrite: Error in Data Tag: %s", dataTagOpen);
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(DATA_TAG_ERROR);
      }
      else if (stateInfo->xmlcolNameOpen[i] == NULL){
	stateInfo->xmlcolNameOpen[i] =  strdup(dataTagOpen);
	stateInfo->xmlcolNameClose[i] =  strdup(dataTagClose);
      }
      stateInfo->data[i] = dataValue;
      stateInfo->data_size[i] = strlen(dataValue) + 1;
      for (j = cntData ; j < i ; j++) {
	stateInfo->data[j] =  emptyStr;
	stateInfo->data_size[j] = 0;
      }
      cntData = i;
    }
    newValInsert = 0;
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (strlen(stateInfo->data[i]) == 0 && newValInsert == 0) {
	stateInfo->data[i] = stateInfo->lastData[i];
	stateInfo->data_size[i] = strlen(stateInfo->data[i]) + 1;
      }
      else {
	if (stateInfo->lastData[i] != NULL) 
	  free(stateInfo->lastData[i]);
	newValInsert = 1;
	stateInfo->lastData[i] = strdup(stateInfo->data[i]);
      }
    }

    cntData = 0;
    mmSize = 0;
   if (!strcmp(stateInfo->sqlType,"MULTIINSERT")) {
    for (i = 0 ; i < stateInfo->compSql.sqlCount ; i++) {
      for (j = 0 ;  j < stateInfo->num_of_cols ; j++) {
	for (k = 0; k < MAX_SQL_TAGS_COUNT ; k++) {
	  if (stateInfo->compSql.tagusage[i][k] == TAG_BAD)
	    break;
	  if (atoi(stateInfo->compSql.tags[i][k]) == j) {
	    stateInfo->compSql.tagusage[i][k] = TAG_FULL;
	    stateInfo->compSql.dataValue[i][k] = stateInfo->data[j];
	  }
	}
      }
    }

    if ((ii = appendAllUnConsumedSqlIntoAllocatedBuffer(
					  &(stateInfo->compSql),
			       &(stateInfo->storedSQLBuffer),
			       &(stateInfo->freestoredSQLBufferSize))) < 0) {
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      return(ii);
    }
    if ((ii = executeCompoundSql(stateInfo, 
				 stateInfo->storedSQLBuffer)) < 0) {
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      return(ii);
    }
    stateInfo->freestoredSQLBufferSize += 
      strlen(stateInfo->storedSQLBuffer); 
    stateInfo->storedSQLBuffer[0]  = '\0';
    if (bufSize == 0) 
      break;
   }
   else {
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (strstr(stateInfo->data[i],"'") == NULL)
	mmSize += stateInfo->data_size[i] + 3;
      else 
	mmSize += (2 * stateInfo->data_size[i]) + 3;
    }
    sqlString =  (char *) malloc(strlen(stateInfo->sql) +
				 mmSize + 20);
    sprintf(sqlString, "%s VALUES ( ", stateInfo->sql);
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (i != 0) strcat(sqlString,",");
      tmpPtr = stateInfo->data[i];
      while (isspace(*tmpPtr)) tmpPtr++;
      if (strstr(stateInfo->coltype[i],"CHAR") != NULL &&
	  *tmpPtr != '\'') {
	strcat(sqlString,"'");
	if (strstr(tmpPtr,"'") == NULL)
	  strcat(sqlString,tmpPtr);
	else {
	  tmpPtr2 = sqlString + strlen(sqlString);
	  while (*tmpPtr != '\0') {
	    if (*tmpPtr == '\'') *tmpPtr2++ = '\'';
	    *tmpPtr2++ = *tmpPtr++;
	  }
	  *tmpPtr2 = '\0';
	}
	strcat(sqlString,"'");
      }
      else {
	strcat(sqlString,stateInfo->data[i]);
      }
    }
    strcat(sqlString,")");
    
    if ((i=exec_no_res_sql_sybTab(stateInfo->environ,stateInfo->connect,
				  (unsigned char *) sqlString))!= 0){ 
      elog (NOTICE, "sybTableWrite: Error in Executing SQL %s: %i",
	    mybuf,i);
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      free(sqlString);
      return(i);
    }
    free(sqlString);
    if (bufSize == 0) 
      break;
   }
  }
  if (collapsePtr != NULL) free(collapsePtr);
  collapsePtr = NULL;
  return(length);
}



int 
sybTableWriteTEMPLATE(dbTableDesc *exfd, char *inbuffer, int length)
{

  tableStateInfo *stateInfo;
  int i,j,k,kk,ll, ii, jj, tagIndx;
  int cntData, mmSize, bufSize, unusedBufSize;
  char *sqlString;
  char *mybuf, *tmpPtr, *tmpPtr1, *tmpPtr2, *tmpmmPtr, *tmpmmPtr2, *localbuffer;
  char *collapsePtr, *buffer,  *dataValue;
  char emptyStr[2];
  char dataTag[MAX_TAG_SIZE];
  char *unusedBuffer;
  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  cntData = 0;
  tmpmmPtr = stateInfo->mm_ptr;
  mmSize  = stateInfo->mm_size;
  mybuf = inbuffer;
  buffer = inbuffer;
  unusedBuffer = inbuffer;
  unusedBufSize = length;
  bufSize = length;
  collapsePtr = NULL;
  strcpy(emptyStr,"");

  

  if (stateInfo->mm_size > 0 ) {
    collapsePtr = (char *) malloc(mmSize + bufSize + 1);
    memcpy(collapsePtr,stateInfo->mm_ptr, mmSize );
    memcpy(collapsePtr+stateInfo->mm_size,inbuffer,bufSize);
    *(collapsePtr + mmSize + bufSize  ) = '\0';
    mybuf = collapsePtr;
    free(stateInfo->mm_ptr);
    stateInfo->mm_size = 0;
    buffer = collapsePtr;
    bufSize = (mmSize   + length);
    unusedBuffer =  collapsePtr;
    unusedBufSize = bufSize ;
    
  }

 if (!strcmp(stateInfo->sqlType,"MULTIINSERT")) {
  while (1) {
      
      j = getNextDataFromTEMPLATE(stateInfo, &mybuf, dataTag, &dataValue,
				  bufSize - (mybuf -  buffer));
      
      if (j != RE_MATCH_NOT_FOUND && j != MDAS_SUCCESS ) {
	elog (NOTICE, "sybTableWrite: Error in Processing Choice Rule: %i", j);
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(j);
      }
      if (j == RE_MATCH_NOT_FOUND ) {
	tmpmmPtr = malloc(unusedBufSize + 1);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = unusedBufSize;
	memcpy(tmpmmPtr , unusedBuffer, unusedBufSize);
	*(tmpmmPtr + unusedBufSize  ) =  '\0';
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(length);
      }
      for (tagIndx  = 0;  tagIndx < stateInfo->num_of_cols; tagIndx++) {
	if (!strcmp(dataTag,stateInfo->xmlcolName[tagIndx])) break;
      }
      if (tagIndx == stateInfo->num_of_cols) {
	stateInfo->xmlcolName[tagIndx] =  strdup(dataTag);
	stateInfo->num_of_cols++;
      }
      if (tagIndx == 0) {
	ii = appendAllUnConsumedSqlIntoAllocatedBuffer(
			       &(stateInfo->compSql),
			       &(stateInfo->storedSQLBuffer),
			       &(stateInfo->freestoredSQLBufferSize));
	if (ii < 0 ) {
	    elog (NOTICE, 
		  "sybTableWrite: Error in Constructing SQL  in List: %i",ii);
	    if (collapsePtr != NULL) free(collapsePtr);
	    collapsePtr = NULL;
	    return(ii);
	}
	if (ii == 1) {
	  if ((ii = executeCompoundSql(stateInfo, 
				       stateInfo->storedSQLBuffer)) < 0) {
	    if (collapsePtr != NULL) free(collapsePtr);
	    collapsePtr = NULL;
	    return(ii);
	  }
	  stateInfo->freestoredSQLBufferSize += 
	    strlen(stateInfo->storedSQLBuffer); 
	  stateInfo->storedSQLBuffer[0]  = '\0';
	}
      }
      for (i = 0 ; i < stateInfo->compSql.sqlCount ; i++) {
	for (j = 0 ;  j < MAX_SQL_TAGS_COUNT ; j++) {
	  if (stateInfo->compSql.tagusage[i][j] == TAG_BAD)
	    break;
	  if (!strcmp(stateInfo->compSql.tags[i][j], dataTag)) {
	    if (stateInfo->compSql.tagusage[i][j] == TAG_FULL) {
	      /* make insert  statement  */

	      if ((ii = appendSqlFromSqlListIntoAllocatedBuffer(
			       &(stateInfo->compSql), i, 
			       &(stateInfo->storedSQLBuffer),
			       &(stateInfo->freestoredSQLBufferSize)))
		  < 0) {
		elog (NOTICE, 
		   "sybTableWrite: Error in Constructing SQL '%i' in List: %i",
		      i ,ii);
		if (collapsePtr != NULL) free(collapsePtr);
		collapsePtr = NULL;
		return(ii);
	      }
	      for (k = 0; k < MAX_SQL_TAGS_COUNT ; k++) {
		if (stateInfo->compSql.tagusage[i][k] == TAG_BAD)
		  break;
		stateInfo->compSql.tagusage[i][k] = TAG_CONSUMED;
	      } 
	    }
	    stateInfo->compSql.tagusage[i][j] = TAG_FULL;
	    stateInfo->compSql.dataValue[i][j] = dataValue;
	    break;
	  }
	}
      }
      if (stateInfo->data[tagIndx] != NULL) 
	free (stateInfo->data[tagIndx]);
      stateInfo->data[tagIndx] = dataValue;
      if (bufSize == 0) 
	break;
      unusedBuffer = mybuf;
      unusedBufSize = bufSize - (mybuf -  buffer);
      
  }
  if (collapsePtr != NULL) free(collapsePtr);
  collapsePtr = NULL;
  return(length);
 }
 else {
  while (1) {
    for (cntData = 0 ; cntData < stateInfo->num_of_cols ; cntData++) {
      
      j = getNextDataFromTEMPLATE(stateInfo, &mybuf, dataTag, &dataValue,
				  bufSize - (mybuf -  buffer));
      
      if (j != RE_MATCH_NOT_FOUND && j != MDAS_SUCCESS ) {
	elog (NOTICE, "sybTableWrite: Error in Processing Choice Rule: %i", j);
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	for (i = 0; i < cntData; i++) {
	  if (stateInfo->data[i] != emptyStr)
	    free(stateInfo->data[i]);
	}
	return(j);
      }
      if (j == RE_MATCH_NOT_FOUND ) {
	tmpmmPtr = malloc(unusedBufSize + 1);
	stateInfo->mm_ptr = tmpmmPtr;
	stateInfo->mm_size = unusedBufSize;
	memcpy(tmpmmPtr , unusedBuffer, unusedBufSize);
	*(tmpmmPtr + unusedBufSize  ) =  '\0';
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	return(length);
      }
      for (i  = 0;  i < stateInfo->num_of_cols; i++) {
	if (stateInfo->xmlcolName[i] == NULL)
	  break;
	if (!strcmp(dataTag,stateInfo->xmlcolName[i])) 
	  break;
      }
      if (i == stateInfo->num_of_cols) {
	elog (NOTICE, "sybTableWrite: Error in Data Tag: %s", dataTag);
	if (collapsePtr != NULL) free(collapsePtr);
	collapsePtr = NULL;
	for (i = 0; i <= cntData; i++) {
	  if (stateInfo->data[i] != emptyStr)
	    free(stateInfo->data[i]);
	}
	return(DATA_TAG_ERROR);
      }
      else if (stateInfo->xmlcolName[i] == NULL){
	stateInfo->xmlcolName[i] =  strdup(dataTag);

      }
      stateInfo->data[i] = dataValue;
      stateInfo->data_size[i] = strlen(dataValue);
      for (j = cntData ; j < i ; j++) {
	stateInfo->data[j] =  emptyStr;
	stateInfo->data_size[j] = 0;
      }
      cntData = i;
    }
    
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (strlen(stateInfo->data[i]) == 0) {
	stateInfo->data[i] = stateInfo->lastData[i];
      }
      else {
	if (stateInfo->lastData[i] != NULL) 
	  free(stateInfo->lastData[i]);
	stateInfo->lastData[i] = stateInfo->data[i];
      }
    }
    unusedBuffer = mybuf;
    unusedBufSize = bufSize - (mybuf -  buffer);

    cntData = 0;
    mmSize = 0;

    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (strstr(stateInfo->data[i],"'") == NULL)
	mmSize += stateInfo->data_size[i] + 3;
      else 
	mmSize += (2 * stateInfo->data_size[i]) + 3;
    }
    sqlString =  (char *) malloc(strlen(stateInfo->sql) +
				 mmSize + 20);
    sprintf(sqlString, "%s VALUES ( ", stateInfo->sql);
    for (i = 0; i < stateInfo->num_of_cols; i++) {
      if (i != 0) strcat(sqlString,",");
      tmpPtr = stateInfo->data[i];
      while (isspace(*tmpPtr)) tmpPtr++;
      if (strstr(stateInfo->coltype[i],"CHAR") != NULL &&
	  *tmpPtr != '\'') {
	strcat(sqlString,"'");
	if (strstr(tmpPtr,"'") == NULL)
	  strcat(sqlString,tmpPtr);
	else {
	  tmpPtr2 = sqlString + strlen(sqlString);
	  while (*tmpPtr != '\0') {
	    if (*tmpPtr == '\'') *tmpPtr2++ = '\'';
	    *tmpPtr2++ = *tmpPtr++;
	  }
	  *tmpPtr2 = '\0';
	}
	strcat(sqlString,"'");
      }
      else {
	strcat(sqlString,stateInfo->data[i]);
      }
    }
    strcat(sqlString,")");
    
    if ((i=exec_no_res_sql_sybTab(stateInfo->environ,stateInfo->connect,
				  (unsigned char *) sqlString))!= 0){ 
      elog (NOTICE, "sybTableWrite: Error in Executing SQL %s: %i",
	    sqlString,i);
      if (collapsePtr != NULL) free(collapsePtr);
      collapsePtr = NULL;
      free(sqlString);
      return(i);
    }
    free(sqlString);
    if (bufSize == 0) 
      break;
  }
  if (collapsePtr != NULL) free(collapsePtr);
  collapsePtr = NULL;
  return(length);
 }

}

int 
sybTableSeek(dbTableDesc *exfd, 
                  int      offset, 
                  int      whence)
{
  tableStateInfo *stateInfo;
  
  stateInfo = (tableStateInfo *) exfd->dbSpecificInfo;
  if (!strcmp(stateInfo->sqlType,"SELECT"))
    return(TABLE_SID_GENERAL_ERROR);
  else
    return(MDAS_SUCCESS);
}

int sybTableCreate(dbTableDesc   *exfd, 
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

    tableStateInfo *stateInfo;
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
        elog (NOTICE, "sybTableCreate: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "sybTableCreate: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }

    while (isspace(*tableCreateStmt)) tableCreateStmt++;
    if(strstr(tableCreateStmt,"create") == NULL &&
       strstr(tableCreateStmt,"CREATE") == NULL) {
        elog (NOTICE, "sybTableCreate: Error: Not a Create Statement : %s",
        tableCreateStmt);
        return INP_ERR_RES_FORMAT;
    }

    if((stateInfo = malloc(sizeof (tableStateInfo))) == NULL) {
      elog (NOTICE, "sybTableCreate: Malloc error");
      return DB_ERR_MALLOC;
    }

    stateInfo->mm_size = 0;
    stateInfo->num_of_cols = 0;
    stateInfo->endOfData = 1;

    if ((i =  open_env_sybTab((long int *)&stateInfo->environ))!= 0)  {
      elog (NOTICE, "sybTableCreate: Error in Creating Environment: %i",i);
      sybTableErrorReport(NULL, stateInfo->environ);
      free(stateInfo); stateInfo = NULL;
      return(DB_TAB_OPEN_ENV_ERROR);
    }
    if ((i =  connect_sybTab( (long int)  stateInfo->environ, 
			   (long int *) &stateInfo->connect, 
			   (unsigned char*) database, 
			   (unsigned char*) uid, 
			   (unsigned char*) pwd))!= 0)  { 

      elog (NOTICE, "sybTableCreate: Error in Connecting to %s: %i",
	    database,i);
      sybTableErrorReport(NULL, stateInfo->environ);
      close_env_sybTab( (long int) stateInfo->environ); 
      free(stateInfo); stateInfo = NULL;
      return(DB_TAB_CONNECT_ERROR);
    }

    if ((i=exec_no_res_sql_sybTab(stateInfo->environ,stateInfo->connect,
			(unsigned char *)tableCreateStmt))!= 0){ 
      elog (NOTICE, "sybTableCreate: Error in Executing SQL %s: %i",
	    tableCreateStmt,i);
      sybTableErrorReport(NULL, stateInfo->environ);
      disconnect_sybTab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
      close_env_sybTab( (long int) stateInfo->environ); 
      free(stateInfo); stateInfo = NULL;
      return(i);
    }
    
    stateInfo->tableNameList = NULL;
    tmpPtr = strdup(tableCreateStmt);
    lower (tmpPtr);
    if ((tmpPtr1 = strstr(tmpPtr, "table ")) != NULL) {
      tmpPtr1 += 6;
      while (isspace(*tmpPtr1)) tmpPtr1++;
      if ((tmpPtr2 = strstr(tmpPtr1, " ")) != NULL) 
	*tmpPtr2 = '\0';
      stateInfo->tableNameList = strdup(tmpPtr1);
      for ( ; *tmpPtr1 != '\0'; tmpPtr1++) 
	if (*tmpPtr1 == ',') stateInfo->num_of_tables++;
      stateInfo->num_of_tables++;

    }
    free(tmpPtr);
    stateInfo->num_of_rows_read = 0;
    exfd->dbSpecificInfo = (char *)stateInfo; 
    return(MDAS_SUCCESS);
}


int sybTableUnlink(char       *resourceLocationName, 
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

    tableStateInfo *stateInfo;
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
        elog (NOTICE, "sybTableUnlink: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "sybTableUnlink: Error in resourceLocationName : %s",
        resourceLocationName);
        return INP_ERR_RES_FORMAT;
    }


    if((stateInfo = malloc(sizeof (tableStateInfo))) == NULL) {
      elog (NOTICE, "sybTableUnlink: Malloc error");
      return DB_ERR_MALLOC;
    }

    stateInfo->mm_size = 0;
    stateInfo->num_of_cols = 0;

    if ((i =  open_env_sybTab((long int *)&stateInfo->environ))!= 0)  {
      elog (NOTICE, "sybTableUnlink: Error in Unlinking Environment: %i",i);
      sybTableErrorReport(NULL, stateInfo->environ);
      free(stateInfo); stateInfo = NULL;
      return(DB_TAB_OPEN_ENV_ERROR);
    }
    if ((i =  connect_sybTab( (long int)  stateInfo->environ, 
			   (long int *) &stateInfo->connect, 
			   (unsigned char*) database, 
			   (unsigned char*) uid, 
			   (unsigned char*) pwd))!= 0)  { 

      elog (NOTICE, "sybTableUnlink: Error in Connecting to %s: %i",
	    database,i);
      sybTableErrorReport(NULL, stateInfo->environ);
      close_env_sybTab( (long int) stateInfo->environ); 
      free(stateInfo); stateInfo = NULL;
      return(DB_TAB_CONNECT_ERROR);
    }
    sprintf(queryString, "drop table %s",tableName);
    if ((i=exec_no_res_sql_sybTab(stateInfo->environ,stateInfo->connect,
			(unsigned char *)queryString))!= 0){ 
      elog (NOTICE, "sybTableUnlink: Error in Executing SQL %s: %i",
	    queryString,i);
      sybTableErrorReport(NULL, stateInfo->environ);
      disconnect_sybTab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_ROLLBACK) ;
      close_env_sybTab( (long int) stateInfo->environ); 
      free(stateInfo); stateInfo = NULL;
      return(i);
    }
    
      disconnect_sybTab( (long int)stateInfo->environ, 
		      (long int)stateInfo->connect, MDAS_CAT_COMMIT) ;
      close_env_sybTab( (long int) stateInfo->environ); 
      free(stateInfo); stateInfo = NULL;

    return(MDAS_SUCCESS);
}
int
executeCompoundSql(tableStateInfo *stateInfo, char *sqlbuf)
{
  char *mybuf1, *mybuf2;
  char myChar;
  int environ, connect;
  
  environ = stateInfo->environ;
  connect = stateInfo->connect;
  mybuf1 = sqlbuf;
  trimsides(mybuf1);
  if (reverseExecution(stateInfo)) {
    while (*mybuf1 != '\0') {
      if ((mybuf2 =  (char *) getEndOfStatement(mybuf1)) == NULL) 
	break;
      myChar = *mybuf2;
      *mybuf2 = '\0';
      if ((i=exec_no_res_sql_sybTab(environ, connect,
				    (unsigned char *) mybuf1))!= 0){ 
	if ( i != DB_UNIQUE_CONSTRAINT_VIOLATION) {
	  elog (NOTICE, "sybTableWrite: Error in Executing SQL %s: %i",
		mybuf1,i);
	  return(i);
	}
      }
      if (myChar == '\0')
	break;
      mybuf1 = mybuf2 + 1;
      while (isspace(*mybuf1)) mybuf1++;
    }
  }
  else {
    while (*mybuf1 != '\0') {
      if ((mybuf2 =  (char *) getEndOfStatement(mybuf1)) == NULL) 
	break;
      myChar = *mybuf2;
      *mybuf2 = '\0';
      if ((i=exec_no_res_sql_sybTab(environ, connect,
				    (unsigned char *) mybuf1))!= 0){ 
	if ( i != DB_UNIQUE_CONSTRAINT_VIOLATION) {
	  elog (NOTICE, "sybTableWrite: Error in Executing SQL %s: %i",
		mybuf1,i);
	  return(i);
	}
      }
      if (myChar == '\0')
	break;
      mybuf1 = mybuf2 + 1;
      while (isspace(*mybuf1)) mybuf1++;
    }
    
  }
  return(MDAS_SUCCESS);
}



