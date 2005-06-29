/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
*************************************************************************/

#ifdef SRB_MDAS

#include "mdasGlobalsExtern.h"
#include "elogExtern.h"
#include "mdasPrototypes.h"
#include "srbMiscExtern.h"
#if defined(PORTNAME_solaris)
#include <ctype.h>
#endif


#ifdef MYSMCAT
#include "mdasC_mys_externs.h"
#endif

#ifdef PSQMCAT
#define nextDataId "nextval('DataId')"
#define currDataId "currval('DataId')"
#endif  /* PSQMCAT */
#ifdef ORAMCAT
#define nextDataId "DataId.NextVal"
#define currDataId "DataId.CurrVal"
#endif /* ORAMCAT */
char *forwardPath (char *pathName, int count);

#ifdef ORAMCAT
int commit_db2_interaction_internal(long int inhenv,
                  long int  inhdbc, int  transaction_end_code);
#endif
#ifdef DB2MCAT
int commit_db2_interaction_internal(SQLHENV henv,
                  SQLHDBC hdbc, int  transaction_end_code);
#endif
#ifdef SYBMCAT
int commit_db2_interaction_internal(long int inhenv,
                  long int  inhdbc, int  transaction_end_code);
#endif
#ifdef PSQMCAT
int commit_db2_interaction_internal(long int inhenv,
                  long int inhdbc, int  transaction_end_code);
#endif

#ifdef IFMXMCAT
int commit_db2_interaction_internal(SQLHENV henv,
                  SQLHDBC hdbc, int  transaction_end_code);
#endif

#ifdef MYSMCAT
int commit_db2_interaction_internal(SQLHENV henv, SQLHDBC hdbc, int  transaction_end_code);
#endif

#define MCAT 1

int BulkLoadFlag = 0;

int ExecSqlDb2(long int   henv, long int hdbc, char *str)
{
  int res;

  res = exec_no_res_sql_db2(henv,hdbc, (unsigned char *) str);

  return (res);
}

int
remove_quotes(char *msg, char *ermsg)
{
  int i = 0;
  int j = 0;
  int k;
  i = 0;
  j = 0;
  if (msg == NULL)
    {
      ermsg[0]  = '\0';
    }

  k = strlen(msg);
  for (i=0; ((msg[i] != '\0') && (i < k)) ; i++)
    {
      if ((msg[i] != '\'') && (msg[i] != '\"'))
            {
              ermsg[j] = msg[i];
              j++;
            }
    }
  ermsg[j] = '\0' ;
}

void error_exit(char *msg)
{
  char newmsg[HUGE_STRING], newmsg2[HUGE_STRING], t[MAX_TOKEN], s[MAX_TOKEN];
  char ermsg[HUGE_STRING];
  int intval, intval2, intval3, res;
  int old_failure;

  commit_db2_interaction(1);

    failure = METADATA_ACCESS_ERROR;

}






void get_ctype_attr_value(char cval[], char cname[], char name[], char val[])
{
    char *tmpPtr;

    for (k=0;k < MAX_TOKEN && name[k] != '.' ;k++){
      tabname[k] = name[k];
    }
    tabname[k] = '\0';
    sprintf(sqlq, "select %s%s from %s%s where %s%s = %s", 
	    MDASSCHEMENAME, cname, MDASSCHEMENAME, tabname, MDASSCHEMENAME, 
	    name, val);
    if ((tmpPtr = (char *) getTabledSingleData(sqlq)) != NULL) {
      strcpy(cval,tmpPtr);
      return;
    }
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) {failure = METADATA_ACCESS_ERROR;return;}
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) {failure = METADATA_ACCESS_ERROR;return;}
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) {failure = METADATA_ACCESS_ERROR;return;}
    i = get_row_db2(hstmt);
    if (i == 0)       {
        truncate_end_blanks(data_size[0], data[0]);
        strcpy(cval, (char *) data[0]);
	addToTabledSingleData(sqlq, data[0]);
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
      }
    else
      {
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry:  Data missing: %s='%s':: %s",name, val,sqlq);
		failure = METADATA_ACCESS_ERROR;

      }

   return;
}

int
auditDataset(int dataId)
{
  return(MDAS_SUCCESS);
}

int 
performAuditDataAccess(char *actName, int userId, int dataId, 
		       char *collName,char* comments)
{
  if (auditLevel == 0)  return(MDAS_SUCCESS);
  if (auditLevel >= 2 || auditDataset(dataId) == MDAS_SUCCESS ) {
    if (auditLevel == 3 || audit_flag == 1) {
      if (strstr(actName,"write") != NULL)
	performAudit(D_WRITE,userId,dataId,"","");
      else
	performAudit(D_READ,userId,dataId,"","");
    }
    else {
      if (strstr(actName,"write") != NULL)
	performAudit(D_WRITE,ANNON_USER,dataId,"","");
      else
	performAudit(D_READ,ANNON_USER,dataId,"","");
    }
  }
  return(MDAS_SUCCESS);
}

int
performAudit(int actId, int userId, int dataId, char *collName,char* comments)
{
 char tS[100];
 int i;
 char query_string[2000];
  get_time(tS);
 
 if (auditLevel == 0)  return(MDAS_SUCCESS);
#if defined(ORAMCAT) || defined(PSQMCAT)
  if (dataId == DATAID_CURRVAL) 
    sprintf(query_string, "insert into %sMDAS_AUDIT  (aud_timestamp,actionId,userId,dataId,collName,aud_comments ) values ('%s',%i,%i,%s,'%s','%s')",
	  MDASSCHEMENAME,tS,actId, userId, currDataId, collName, comments);
  else 
    sprintf(query_string, "insert into %sMDAS_AUDIT (aud_timestamp,actionId,userId,dataId,collName,aud_comments ) values ('%s',%i,%i,%i,'%s','%s')",
	    MDASSCHEMENAME,tS,actId, userId, dataId, collName, comments);
#else
  sprintf(query_string, "insert into %sMDAS_AUDIT (aud_timestamp,actionId,userId,dataId,collName,aud_comments ) values ('%s',%i,%i,%i,'%s','%s')",
	  MDASSCHEMENAME,tS, actId, userId, dataId, collName, comments);

#endif /* ORAMCAT */

  i = ExecSqlDb2(henv, hdbc,  query_string);
  if (i != 0)  
    elog(NOTICE,"Failure in insert Audit Information:%s\n",query_string);
  
  return(MDAS_SUCCESS);

}



int get_itype_attr_value( char cname[], char name[], char val[])
{
    char *tmpPtr;
 
    for (k=0;k < MAX_TOKEN && name[k] != '.' ;k++){
      tabname[k] = name[k];
    }
    tabname[k] = '\0';
    sprintf(sqlq, "select %s%s from %s%s where %s%s = '%s'", 
	    MDASSCHEMENAME, cname, 
	    MDASSCHEMENAME, tabname, MDASSCHEMENAME,name, val);
    if ((tmpPtr = (char *) getTabledSingleData(sqlq)) != NULL) {
      j = atoi( (char *) tmpPtr);
      return(j);
    }
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: Data missing: %s='%s':: %s",name, val,sqlq);
	failure = METADATA_ACCESS_ERROR;
	return (METADATA_ACCESS_ERROR);
      }
    else 
      {
	truncate_end_blanks(data_size[0], data[0]);
	j = atoi( (char *) data[0]);
	addToTabledSingleData(sqlq, data[0]);
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	return(j);
      }
}

int
get_5ctype_list_from_query(char cval[][MAX_TOKEN], char cval2[][MAX_TOKEN], 
			   char cval3[][MAX_TOKEN],char cval4[][MAX_TOKEN],
			   char cval5[][MAX_TOKEN],
			   int *colcount,  char *sqlq)
{
  int ii,i;

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (i);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (i);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (i);
    i = get_row_db2(hstmt);
    if (i != 0)
      {
	ii = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,
             "MDAS registry:  FAiled Query: %s",sqlq);
	failure = METADATA_ACCESS_ERROR;
	return(i);
      }

    ii = 0;
    while (i == 0 && ii < MAX_CTYPE_LIST_COUNT)       
      {
        truncate_end_blanks(data_size[0], data[0]);
        strcpy(cval[ii], (char *) data[0]);
        truncate_end_blanks(data_size[1], data[1]);
        strcpy(cval2[ii], (char *) data[1]);
        truncate_end_blanks(data_size[2], data[2]);
        strcpy(cval3[ii], (char *) data[2]);
        truncate_end_blanks(data_size[3], data[3]);
        strcpy(cval4[ii], (char *) data[3]);
        truncate_end_blanks(data_size[4], data[4]);
        strcpy(cval5[ii], (char *) data[4]);
	ii++;
	strcpy((char *)data[0],"");
	strcpy((char *)data[1],"");
	strcpy((char *)data[2],"");
	strcpy((char *)data[3],"");
	strcpy((char *)data[4],"");
	i = get_row_db2(hstmt);
      }
    *colcount = ii;
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
}

int
get_4ctype_list_from_query(char cval[][MAX_TOKEN], char cval2[][MAX_TOKEN], 
			   char cval3[][MAX_TOKEN],char cval4[][MAX_TOKEN], 
			   int *colcount,  char *sqlq)
{
  int ii,i;

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (i);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (i);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (i);
    i = get_row_db2(hstmt);
    if (i != 0)
      {
	ii = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,
             "MDAS registry:  FAiled Query: %s",sqlq);
	failure = METADATA_ACCESS_ERROR;
	return(i);
      }

    ii = 0;
    while (i == 0 && ii < MAX_CTYPE_LIST_COUNT)       
      {
        truncate_end_blanks(data_size[0], data[0]);
        strcpy(cval[ii], (char *) data[0]);
        truncate_end_blanks(data_size[1], data[1]);
        strcpy(cval2[ii], (char *) data[1]);
        truncate_end_blanks(data_size[2], data[2]);
        strcpy(cval3[ii], (char *) data[2]);
        truncate_end_blanks(data_size[3], data[3]);
        strcpy(cval4[ii], (char *) data[3]);
	ii++;
	strcpy((char *)data[0],"");
	strcpy((char *)data[1],"");
	strcpy((char *)data[2],"");
	strcpy((char *)data[3],"");
	i = get_row_db2(hstmt);
      }
    *colcount = ii;
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
}

int
get_3ctype_list_from_query(char cval[][MAX_TOKEN], char cval2[][MAX_TOKEN], 
			   char cval3[][MAX_TOKEN], 
			   int *colcount,  char *sqlq)
{
  int ii,i;

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (i);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (i);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (i);
    i = get_row_db2(hstmt);
    if (i != 0)
      {
	ii = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,
             "MDAS registry:  FAiled Query: %s",sqlq);
	failure = METADATA_ACCESS_ERROR;
	return(i);
      }

    ii = 0;
    while (i == 0 && ii < MAX_CTYPE_LIST_COUNT)       
      {
        truncate_end_blanks(data_size[0], data[0]);
        strcpy(cval[ii], (char *) data[0]);
        truncate_end_blanks(data_size[1], data[1]);
        strcpy(cval2[ii], (char *) data[1]);
        truncate_end_blanks(data_size[2], data[2]);
        strcpy(cval3[ii], (char *) data[2]);
	ii++;
	strcpy((char *)data[0],"");
	strcpy((char *)data[1],"");
	strcpy((char *)data[2],"");
	i = get_row_db2(hstmt);
      }
    *colcount = ii;
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
}

int
get_2ctype_list_from_query(char cval[][MAX_TOKEN], char cval2[][MAX_TOKEN], 
			   int *colcount,  char *sqlq)
{
  int ii;
  int i;

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (i);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (i);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (i);
    i = get_row_db2(hstmt);
    if (i != 0)
      {
	ii = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,
             "MDAS registry:  FAiled Query: %s",sqlq);
	failure = METADATA_ACCESS_ERROR;
	return(i);
      }
    ii = 0;
    while (i == 0 && ii < MAX_CTYPE_LIST_COUNT)       
      {
        truncate_end_blanks(data_size[0], data[0]);
        strcpy(cval[ii], (char *) data[0]);
        truncate_end_blanks(data_size[1], data[1]);
        strcpy(cval2[ii], (char *) data[1]);
	ii++;
	strcpy((char *)data[0],"");
	strcpy((char *)data[1],"");
	i = get_row_db2(hstmt);
      }
    *colcount = ii;
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
}



int
get_ctype_list_from_query(char cval[][MAX_TOKEN], int *colcount,
	              char *sqlq)
{
  int ii;

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0)
      {
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,
             "MDAS registry:  FAiled Query: %s",sqlq);
	failure = METADATA_ACCESS_ERROR;
	return(-1);
      }
    ii = 0;
    while (i == 0 && ii < MAX_CTYPE_LIST_COUNT)       
      {
        truncate_end_blanks(data_size[0], data[0]);
        strcpy(cval[ii], (char *) data[0]);
	ii++;
	strcpy((char *)data[0],"");
	i = get_row_db2(hstmt);
      }
    *colcount = ii;
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
 return(MDAS_SUCCESS);
}

int
get_2itype_list_from_query(int cval[], int cval2[], int *colcount,
	              char *sqlq)
{
  int ii,i;

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (i);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (i);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (i);
    i = get_row_db2(hstmt);
    if (i != 0)
      {
	ii = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,
             "MDAS registry:  FAiled Query: %s",sqlq);
	failure = METADATA_ACCESS_ERROR;
	return(i);
      }
    ii = 0;
    while (i == 0 && ii < MAX_ITYPE_LIST_COUNT)       
      {
	truncate_end_blanks(data_size[0], data[0]);
	cval[ii] = atoi( (char *) data[0]);
        truncate_end_blanks(data_size[1], data[1]);
	cval2[ii] = atoi( (char *) data[1]);
	ii++;
	strcpy((char *)data[0],"");
	strcpy((char *)data[1],"");
	i = get_row_db2(hstmt);
      }
    *colcount = ii;
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
}


int
get_itype_list_from_query(int cval[], int *colcount,
	              char *sqlq)
{
  int ii;

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0)
      {
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,
             "MDAS registry:  FAiled Query: %s",sqlq);
	failure = METADATA_ACCESS_ERROR;
	return(-1);
      }
    ii = 0;
    while (i == 0 && ii < MAX_ITYPE_LIST_COUNT)       
      {
	truncate_end_blanks(data_size[0], data[0]);
	cval[ii] = atoi( (char *) data[0]);
	ii++;
	strcpy((char *)data[0],"");
	i = get_row_db2(hstmt);
      }
    *colcount = ii;
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
}


int
get_row_from_query(char cval[][MAX_TOKEN], int *colcount,
	              char *sqlq)
{
  int ii,i;

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (i);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (i);
    if (num_of_cols > *colcount)  return (COLUMN_COUNT_MISMATCH);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (i);
    i = get_row_db2(hstmt);
    if (i != 0)
      {
	ii = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,
             "MDAS registry:  Failed Query: %s",sqlq);
	failure = METADATA_ACCESS_ERROR;
	return(i);
      }
    ii = 0;
     if (num_of_cols > USABLE_TABLE_COLS)
       num_of_cols = USABLE_TABLE_COLS; /* added Jan 22,2003/overflow */
    for (ii = 0; ii < num_of_cols ; ii++)
      {
        truncate_end_blanks(data_size[ii], data[ii]);
        strcpy(cval[ii], (char *) data[ii]);
      }
    *colcount = num_of_cols;
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
}



int get_ctype_value(char cval[], char name[], char val[])
{
    char *tmpPtr;

    for (k=0;k < MAX_TOKEN && name[k] != '.' ;k++){
      tabname[k] = name[k];
    }
    tabname[k] = '\0';
    sprintf(sqlq, "select * from %s%s where %s%s = '%s'", 
	    MDASSCHEMENAME, tabname, MDASSCHEMENAME, name, val);
    if ((tmpPtr = (char *) getTabledSingleData(sqlq)) != NULL) {
      strcpy(cval,tmpPtr);
      return 0;
    }
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) {failure = METADATA_ACCESS_ERROR;return 0;}
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) {failure = METADATA_ACCESS_ERROR;return 0;}
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) {failure = METADATA_ACCESS_ERROR;return 0;}
    i = get_row_db2(hstmt);
    if (i == 0)       {
        truncate_end_blanks(data_size[0], data[0]);
        strcpy(cval, (char *) data[0]);
	addToTabledSingleData(sqlq, data[0]);
      }
    else
      {
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry:  Data missing: %s='%s':: %s",name, val,sqlq);
		failure = METADATA_ACCESS_ERROR;
	return 0;
      }
        
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return 0;
}

int
get_ctype_value_from_query(char *cval, char *sqlq)
{
    char *tmpPtr;

    if ((tmpPtr = (char *) getTabledSingleData(sqlq)) != NULL) {
      strcpy(cval,tmpPtr);
      return(0);
    }
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) {failure = METADATA_ACCESS_ERROR;return(i);}
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) {failure = METADATA_ACCESS_ERROR;return(i);}
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) {failure = METADATA_ACCESS_ERROR;return(i);}
    i = get_row_db2(hstmt);
    if (i == 0)       {
        truncate_end_blanks(data_size[0], data[0]);
        strcpy(cval, (char *) data[0]);
	addToTabledSingleData(sqlq, data[0]);
      }
    else
      {
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	failure = METADATA_ACCESS_ERROR;
        return(METADATA_ACCESS_ERROR);
      }
        
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return (0);
}


int
get_itype_value_from_query(char *sqlq)
{
  int ii,i, ival ;
  char *tmpPtr;
    if ((tmpPtr = (char *) getTabledSingleData(sqlq)) != NULL) {
      ival = atoi( (char *) tmpPtr);
      return(ival);
    }
  
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (i);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (i);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (i);
    i = get_row_db2(hstmt);
    if (i != 0)
      {
	ii = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,
             "MDAS registry:  Failed Query: %s",sqlq);
	failure = METADATA_ACCESS_ERROR;
	return(i);
      }
    truncate_end_blanks(data_size[0], data[0]);
    ival = atoi( (char *) data[0]);
    addToTabledSingleData(sqlq, data[0]);
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(ival);
}

int get_itype_value(char name[], char val[])
{
    char *tmpPtr;

    for (k=0;k < MAX_TOKEN && name[k] != '.' ;k++){
      tabname[k] = name[k];
    }
    tabname[k] = '\0';
    sprintf(sqlq, "select * from %s%s where %s%s = '%s'", 
	    MDASSCHEMENAME, tabname, MDASSCHEMENAME,name, val);
    if ((tmpPtr = (char *) getTabledSingleData(sqlq)) != NULL) {
      i = atoi( (char *) tmpPtr);
      return(i);
    }

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: Data missing: %s='%s':: %s",name, val,sqlq);
	failure = METADATA_ACCESS_ERROR;
	return (METADATA_ACCESS_ERROR);
      }
    else 
      {
	truncate_end_blanks(data_size[0], data[0]);
	j = atoi( (char *) data[0]);
	addToTabledSingleData(sqlq, data[0]);
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	return(j);
      }
}

int
inContainerObjectById(int dataId) 
{

  sprintf(sqlq, "select data_id from %sMDAS_AD_REPL where data_id = %i and container_id <> %i",
	  MDASSCHEMENAME, dataId , dataId );

  tabledIsOkay = 1;
  if(check_exists(sqlq) != 0) 
	  {
	    tabledIsOkay = 0;
	    return (NOT_IN_CONTAINER_OBJECT);
	  }
  tabledIsOkay = 0;
  return(MDAS_SUCCESS);
}

int
inContainerObjectByName(char *dataName, char *collName) 
{


   sprintf(sqlq, "select t1.data_id from %sMDAS_AD_REPL t1 , %sMDAS_TD_DATA_GRP t2 where t1.data_name = '%s'  AND t1.data_id <> t1.container_id AND t2.data_grp_name = '%s' AND t1.data_grp_id = t2.data_grp_id ", 
	  MDASSCHEMENAME, MDASSCHEMENAME, dataName,collName);
   tabledIsOkay = 1;
  if(check_exists(sqlq) == 0) 
	  {
	    tabledIsOkay = 0;
	    return (MDAS_ONE);
	  }
  tabledIsOkay = 0;
  return(MDAS_ZERO);
}

int
getLocalZone(char *zoneId)
{
  int i;
  failure = 0;
  sprintf(sqlq, "select zone_id from %s%s where local_zone_flag = 1",
	  MDASSCHEMENAME, "MDAS_TD_ZONE");
  tabledIsOkay = 1;
  get_ctype_value_from_query(zoneId,sqlq);
  tabledIsOkay = 0;
  if (failure != 0)
    return(LOCAL_ZONE_NOT_FOUND);
  return(0);
}
int
getDataIdContIdByName(char *dataName, char *collName , int *dataId, int *contId ) 
{
   
  int i,dnum[MAX_ITYPE_LIST_COUNT], cnum[MAX_ITYPE_LIST_COUNT];
  int rowcount;
#ifdef DB2MCAT
   sprintf(sqlq, "select distinct t1.data_id, container_td_id from %sMDAS_AD_REPL t1 , %sMDAS_TD_DATA_GRP t2 where ", 
	  MDASSCHEMENAME, MDASSCHEMENAME);
#else
   sprintf(sqlq, "select distinct t1.data_id, container_id from %sMDAS_AD_REPL t1 , %sMDAS_TD_DATA_GRP t2 where ", 
	  MDASSCHEMENAME, MDASSCHEMENAME);   
#endif
   if (strlen(dataName) > 0) 
     sprintf(&sqlq[strlen(sqlq)]," t1.data_name = '%s'  AND ",dataName);
   if (strlen(collName) > 0)
     sprintf(&sqlq[strlen(sqlq)]," t2.data_grp_name = '%s' AND ",collName);
   sprintf(&sqlq[strlen(sqlq)],"t1.data_grp_id = t2.data_grp_id ");

  i = get_2itype_list_from_query(dnum,cnum,&rowcount,sqlq);
  if (i != 0 || rowcount == 0) {
    return (DATA_NOT_IN_CAT);
  }
  *dataId = dnum[0];
  *contId = cnum[0];
  return(MDAS_SUCCESS);
}

int 
isContainer(int dataId) 
{
  sprintf(sqlq, "select container_id from %sMDAS_TD_CONTAINER where container_id = %i",
	  MDASSCHEMENAME, dataId );
  tabledIsOkay = 1;
  if(check_exists(sqlq) == 0) 
	  {
	    tabledIsOkay = 0;
	    return (MDAS_ONE);
	  }
  tabledIsOkay = 0;
  return(MDAS_ZERO);
}

int 
isContainerByName(char *containerName, char *collectionName) 
{
  sprintf(sqlq, "select container_id from %sMDAS_TD_CONTAINER where container_name = '%s/%s'",
	  MDASSCHEMENAME, collectionName,containerName );
  tabledIsOkay = 1;
  if(check_exists(sqlq) == 0) 
	  {
	    tabledIsOkay = 0;
	    return (MDAS_ONE);
	  }
  tabledIsOkay = 0;
  return(MDAS_ZERO);
}

int getInContainerDataIdWithCollectionAndAccess(char *obj_data_name,
				char *obj_user_name,  char *collection_name,
				char *domain_name,
				int *obj_data_id,  int *replication_num, 
				int *obj_user_id, char *forAccess)
{
  int i,ii,j,k;
  /*** June 15, 2001  added to make it look and use for user group access 
  sprintf(sqlq, "select t9.data_id, t9.repl_enum, t3.user_id from %sMDAS_AD_REPL t9, %sMDAS_ADC_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t9.data_name = '%s' AND t9.is_deleted = 0 AND     t6.data_grp_name = '%s' and  t6.data_grp_id = t9.data_grp_id AND t9.container_id = t1.data_id AND  t3.user_name = '%s' AND ((t1.data_owner = t3.user_id) OR  (t3.user_id = t4.user_id AND  t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and  t4.access_id >= t5.access_id AND t5.access_constraint like '%s%%'))   AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id  AND t8.user_id = t3.user_id ",  */
 sprintf(sqlq, "select  t9.data_id, t9.repl_enum, t3.user_id from %sMDAS_AD_REPL t9, %sMDAS_ADC_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t2, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t9.data_name = '%s' AND t9.is_deleted = 0 AND     t6.data_grp_name = '%s' and  t6.data_grp_id = t9.data_grp_id AND t9.container_id = t1.data_id AND  t3.user_name = '%s' AND t3.user_id = t2.user_id AND t2.group_user_id = t4.user_id AND  t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and  t4.access_id >= t5.access_id AND t5.access_privilege like '%%%s%%'  AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id  AND t8.user_id = t3.user_id ",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name, 
	  collection_name, obj_user_name, forAccess, domain_name);
	

#ifdef DEBUGON 
  printf("GICDIWC:%s\n",sqlq);
#endif /* DEBUGON */ 

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (i);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (i);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (i);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        ii = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: Get_data_id: Data missin:%s:%s/%s",
		collection_name,obj_user_name, obj_data_name);
	sprintf(sqlq, "select distinct t1.data_id from  %sMDAS_AD_REPL t1 where t1.data_name = '%s' and t1.is_deleted = 0", 
		MDASSCHEMENAME,obj_data_name);
	
	if (check_exists(sqlq) != 0) 
	  {
	    failure = DATA_NOT_IN_CAT;
	    return (DATA_NOT_IN_CAT);
	  }
	
	/* data in given collection */
	
	sprintf(sqlq, "select distinct  t1.data_id from  %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where  t1.data_name = '%s' and t1.is_deleted = 0 and   t1.data_grp_id = t2.data_grp_id and t2.data_grp_name = '%s'" ,
		MDASSCHEMENAME,MDASSCHEMENAME,
		obj_data_name, collection_name);
	if (check_exists(sqlq) != 0) 
	  {
	    failure = DATA_NOT_IN_COLLECTION;
	    return (DATA_NOT_IN_COLLECTION);
	  }
	
	/* is user in given domain */
	sprintf(sqlq, "select distinct  t1.user_id from  %sMDAS_CD_USER t1, %sMDAS_AU_DOMN t2, %sMDAS_TD_DOMN t3 where  t1.user_name = '%s' and   t1.user_id = t2.user_id and t2.domain_id = t3.domain_id  and t3.domain_desc = '%s'" ,
		MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
		obj_user_name, domain_name);
	tabledIsOkay = 1;
	if (check_exists(sqlq) != 0) 
	  {
	    tabledIsOkay = 0;
	    failure = USER_NOT_IN_DOMN;
	    return (USER_NOT_IN_DOMN);
	  }
	tabledIsOkay = 0;
	failure = NO_DATA_ACCS_TO_USER_IN_DOMN;
	return (NO_DATA_ACCS_TO_USER_IN_DOMN);
      }
    else 
      {
    truncate_end_blanks(data_size[0], data[0]);
    truncate_end_blanks(data_size[1], data[1]);
    truncate_end_blanks(data_size[2], data[2]);
    *obj_data_id = atoi( (char *) data[0]);
    *replication_num = atoi( (char *) data[1]);
    *obj_user_id = atoi( (char *) data[2]);
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
      }

}
int get_incontainer_data_id_with_collection(char *obj_data_name,
				char *obj_user_name,  char *collection_name,
				char *domain_name,
				int *obj_data_id,  int *replication_num, 
				int *obj_user_id)
{
  int i,ii,j,k;
  /*** June 15, 2001   added to make it look and use for user group access 
       sprintf(sqlq, "select t9.data_id, t9.repl_enum, t3.user_id from %sMDAS_AD_REPL t9, %sMDAS_ADC_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t9.data_name = '%s' AND t9.is_deleted = 0 AND     t6.data_grp_name = '%s' and  t6.data_grp_id = t9.data_grp_id AND t9.container_id = t1.data_id AND  t3.user_name = '%s' AND ((t1.data_owner = t3.user_id) OR  (t3.user_id = t4.user_id AND  t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and  t4.access_id = t5.access_id AND t5.access_constraint like 'all%%'))   AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id  AND t8.user_id = t3.user_id ", ***/
       sprintf(sqlq, "select   t9.data_id, t9.repl_enum, t3.user_id from %sMDAS_AD_REPL t9, %sMDAS_ADC_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t2, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t9.data_name = '%s' AND t9.is_deleted = 0 AND     t6.data_grp_name = '%s' and  t6.data_grp_id = t9.data_grp_id AND t9.container_id = t1.data_id AND  t3.user_name = '%s' AND t3.user_id = t2.user_id and t2.group_user_id = t4.user_id AND  t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and  t4.access_id = t5.access_id AND t5.access_privilege like '%%all%%'  AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id  AND t8.user_id = t3.user_id ",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name, 
	  collection_name, obj_user_name, domain_name);
	

#ifdef DEBUGON 
  printf("GICDIWC:%s\n",sqlq);
#endif /* DEBUGON */ 

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (i);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (i);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (i);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        ii = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: Get_data_id: Data missin:%s:%s/%s",
		collection_name,obj_user_name, obj_data_name);
	sprintf(sqlq, "select distinct t1.data_id from  %sMDAS_AD_REPL t1 where t1.data_name = '%s' and t1.is_deleted = 0", 
		MDASSCHEMENAME,obj_data_name);
	
	if (check_exists(sqlq) != 0) 
	  {
	    failure = DATA_NOT_IN_CAT;
	    return (DATA_NOT_IN_CAT);
	  }
	
	/* data in given collection */
	
	sprintf(sqlq, "select distinct  t1.data_id from  %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where  t1.data_name = '%s' and t1.is_deleted = 0 and   t1.data_grp_id = t2.data_grp_id and t2.data_grp_name = '%s'" ,
		MDASSCHEMENAME,MDASSCHEMENAME,
		obj_data_name, collection_name);
	if (check_exists(sqlq) != 0) 
	  {
	    failure = DATA_NOT_IN_COLLECTION;
	    return (DATA_NOT_IN_COLLECTION);
	  }
	
	/* is user in given domain */
	sprintf(sqlq, "select distinct  t1.user_id from  %sMDAS_CD_USER t1, %sMDAS_AU_DOMN t2, %sMDAS_TD_DOMN t3 where  t1.user_name = '%s' and   t1.user_id = t2.user_id and t2.domain_id = t3.domain_id  and t3.domain_desc = '%s'" ,
		MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
		obj_user_name, domain_name);
	tabledIsOkay = 1;
	if (check_exists(sqlq) != 0) 
	  {
	    tabledIsOkay = 0;
	    failure = USER_NOT_IN_DOMN;
	    return (USER_NOT_IN_DOMN);
	  }
	tabledIsOkay = 0;
	failure = NO_DATA_ACCS_TO_USER_IN_DOMN;
	return (NO_DATA_ACCS_TO_USER_IN_DOMN);
      }
    else 
      {
    truncate_end_blanks(data_size[0], data[0]);
    truncate_end_blanks(data_size[1], data[1]);
    truncate_end_blanks(data_size[2], data[2]);
    *obj_data_id = atoi( (char *) data[0]);
    *replication_num = atoi( (char *) data[1]);
    *obj_user_id = atoi( (char *) data[2]);
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
      }

}

int get_rsrc_id_with_accs(char *rsrc_name, int given_repl_enum,
			  char *obj_user_name,char *user_domain_name, 
			  char *forAccs, int *rsrc_id, int *obj_user_id, int *out_repl_enum)
{
    mdasC_sql_result_struct* result1 = NULL;
    char qval[MAX_DCS_NUM][MAX_TOKEN], tmpStr[MAX_TOKEN * 2];
    int i , selval[MAX_DCS_NUM];



  if (given_repl_enum < 0) {
    sprintf(sqlq, "select   t1.rsrc_id, t1.repl_enum, t3.user_id from %sMDAS_AR_REPL t1 , %sMDAS_AU_GROUP t3, %sMDAS_AR_ACCS t4,  %sMDAS_TD_RSRC_ACCS t5, %sMDAS_CD_USER t6,  %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.rsrc_name = '%s' and t6.user_name = '%s' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t6.user_id and t6.user_id = t3.user_id AND t3.group_user_id = t4.user_id AND t1.rsrc_id = t4.rsrc_id AND t1.repl_enum=t4.repl_enum and  t4.access_id >= t5.access_id AND t5.access_privilege like '%%%s%%'", 
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, 
	  rsrc_name,  obj_user_name, user_domain_name, forAccs);
  }
  else {
    sprintf(sqlq, "select   t1.rsrc_id, t1.repl_enum, t3.user_id from %sMDAS_AR_REPL t1 , %sMDAS_AU_GROUP t3, %sMDAS_AR_ACCS t4,  %sMDAS_TD_RSRC_ACCS t5, %sMDAS_CD_USER t6,  %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.rsrc_name = '%s' and t1.repl_enum = %i and t6.user_name = '%s' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t6.user_id and t6.user_id = t3.user_id AND t3.group_user_id = t4.user_id AND t1.rsrc_id = t4.rsrc_id AND t1.repl_enum=t4.repl_enum and  t4.access_id >= t5.access_id AND t5.access_privilege like '%%%s%%'", 
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, 
	  rsrc_name, given_repl_enum, obj_user_name, user_domain_name, forAccs);
  }
  i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
  if (i != 0) return (MDAS_FAILURE);
  i = get_no_of_columns_db2(hstmt, &num_of_cols);
  if (i != 0) return (MDAS_FAILURE);
  i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
  if (i != 0) return (MDAS_FAILURE);
  i = get_row_db2(hstmt);
  if (i != 0) {
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(NO_MODIFY_RSRC_ACCS_TO_USER);
  }
  else {
    truncate_end_blanks(data_size[0], data[0]);
    truncate_end_blanks(data_size[1], data[1]);
    truncate_end_blanks(data_size[2], data[2]);
    *rsrc_id = atoi( (char *) data[0]);
    *out_repl_enum = atoi( (char *) data[1]);
    *obj_user_id = atoi( (char *) data[2]);
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
  }

}

int get_rsrc_id(char *rsrc_name, int given_repl_enum,
		int *rsrc_id, int *out_repl_enum)
{
    mdasC_sql_result_struct* result1 = NULL;
    char qval[MAX_DCS_NUM][MAX_TOKEN], tmpStr[MAX_TOKEN * 2];
    int i , selval[MAX_DCS_NUM];


  if (given_repl_enum < 0) {
    sprintf(sqlq, "select   t1.rsrc_id, t1.repl_enum from %sMDAS_AR_REPL t1 where t1.rsrc_name = '%s'", 
	  MDASSCHEMENAME, rsrc_name);
  }
  else {
    sprintf(sqlq, "select   t1.rsrc_id, t1.repl_enum from %sMDAS_AR_REPL t1 where t1.rsrc_name = '%s' and t1.repl_enum = %i)", 
	  MDASSCHEMENAME, rsrc_name, given_repl_enum);
  }
  i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
  if (i != 0) return (MDAS_FAILURE);
  i = get_no_of_columns_db2(hstmt, &num_of_cols);
  if (i != 0) return (MDAS_FAILURE);
  i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
  if (i != 0) return (MDAS_FAILURE);
  i = get_row_db2(hstmt);
  if (i != 0) {
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(NO_MODIFY_RSRC_ACCS_TO_USER);
  }
  else {
    truncate_end_blanks(data_size[0], data[0]);
    truncate_end_blanks(data_size[1], data[1]);
    *rsrc_id = atoi( (char *) data[0]);
    *out_repl_enum = atoi( (char *) data[1]);
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
  }

}



int getDataIdWithCollectionAndAccess(char *obj_data_name,
				char *obj_path_name, char *resource_name,
				char *obj_user_name,  char *collection_name,
				char *domain_name,
				int *obj_data_id,  int *replication_num, 
				int *obj_user_id, char *forAccess, char *versionString)
{

 
  if ( resource_name != NULL && strcmp( resource_name,""))
    {
      if ( collection_name != NULL && strcmp( collection_name,""))
	{
	  /*** June 15, 2001   added to make it look and use for user group access 
	 sprintf(sqlq, "select t1.data_id, t1.repl_enum, t3.user_id, t1.version_num from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND ((t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_constraint like '%s%%') OR t1.data_owner = t3.user_id) AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", ****/
	  sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id, t1.version_num from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id = t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_privilege like '%%%s%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ",
	 MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	 MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	 MDASSCHEMENAME,
	 obj_data_name,  obj_path_name, 
	  collection_name, resource_name, obj_user_name, 
	  forAccess, domain_name);
	}
      else
	{
     /*** June 15, 2001  added to make it look and use for user group access 
	 sprintf(sqlq, "select t1.data_id, t1.repl_enum, t3.user_id, t1.version_num from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND ((t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_constraint like '%s%%') OR t1.data_owner = t3.user_id) AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ",  ****/
	 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id, t1.version_num from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id = t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_privilege like '%%%s%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", 
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name,  obj_path_name, 
	  resource_name, obj_user_name, forAccess, domain_name);
	}
    }
else
  {
      if ( collection_name != NULL && strcmp( collection_name,""))
	{
     /*** June 15, 2001   added to make it look and use for user group access 
	 sprintf(sqlq, "select t1.data_id, t1.repl_enum, t3.user_id, t1.version_num from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s'  AND t1.is_deleted = 0 AND     t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND   t3.user_name = '%s' AND ((t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_constraint like '%s%%') OR t1.data_owner = t3.user_id) AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", ****/
	 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id, t1.version_num from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s'  AND t1.is_deleted = 0 AND     t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND   t3.user_name = '%s' AND t3.user_id = t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_privilege like '%%%s%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name, 
	  collection_name, obj_user_name, forAccess, domain_name);
	}
      else
	{
     /*** June 15, 2001  added to make it look and use for user group access 
	 sprintf(sqlq, "select t1.data_id, t1.repl_enum, t3.user_id, t1.version_num from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.is_deleted = 0 AND   t3.user_name = '%s' AND ((t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_constraint like '%s%%') OR t1.data_owner = t3.user_id) AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ",  ****/
 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id, t1.version_num from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.is_deleted = 0 AND   t3.user_name = '%s' AND t3.user_id = t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_privilege like '%%%s%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", 
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name, 
	  obj_user_name, forAccess, domain_name);
	}

  }
#ifdef DEBUGON 
  printf("GDIWC:%s\n",sqlq);
#endif /* DEBUGON */ 

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: Get_data_id: Data missin:%s:%s:%s/%s",
		obj_user_name, resource_name, obj_path_name,obj_data_name);
	sprintf(sqlq, "select distinct t1.data_id from  %sMDAS_AD_REPL t1 where t1.data_name = '%s' and t1.is_deleted = 0", 
	    MDASSCHEMENAME,obj_data_name);

	if (check_exists(sqlq) != 0) 
	  {
	    failure = DATA_NOT_IN_CAT;
	    return (DATA_NOT_IN_CAT);
	  }
    if (resource_name != NULL && strcmp( resource_name,""))
      {
	sprintf(sqlq, "select t1.rsrc_name from %sMDAS_AR_REPL t1 where t1.rsrc_name = '%s'",
		 MDASSCHEMENAME, resource_name);
	tabledIsOkay = 1;
	if (check_exists(sqlq) != 0) 
	  {
	    tabledIsOkay = 0;
	    failure = RESOURCE_NOT_IN_CAT;
	    return (RESOURCE_NOT_IN_CAT);
	  }
	tabledIsOkay = 0;
	sprintf(sqlq, "select t1.data_ID from %sMDAS_AD_REPL t1  , %sMDAS_CD_RSRC t2   where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0  AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s'",
		 MDASSCHEMENAME, MDASSCHEMENAME, 
		obj_data_name, obj_path_name, resource_name);
	if (check_exists(sqlq) != 0) 
	  {
	    failure = DATA_NOT_IN_RESOURCE_OR_PATH;
	    return (DATA_NOT_IN_RESOURCE_OR_PATH);
	  }
      }
    /* data in given collection */
  if ( collection_name != NULL && strcmp( collection_name,""))
  {
    sprintf(sqlq, "select distinct  t1.data_id from  %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where  t1.data_name = '%s' and t1.is_deleted = 0 and   t1.data_grp_id = t2.data_grp_id and t2.data_grp_name = '%s'" ,
	    MDASSCHEMENAME,MDASSCHEMENAME,
	    obj_data_name, collection_name);
    if (check_exists(sqlq) != 0) 
      {
	failure = DATA_NOT_IN_COLLECTION;
	  return (DATA_NOT_IN_COLLECTION);
      }
  }

    /* is user in given domain */
    sprintf(sqlq, "select distinct  t1.user_id from  %sMDAS_CD_USER t1, %sMDAS_AU_DOMN t2, %sMDAS_TD_DOMN t3 where  t1.user_name = '%s' and   t1.user_id = t2.user_id and t2.domain_id = t3.domain_id  and t3.domain_desc = '%s'" ,
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    obj_user_name, domain_name);
    tabledIsOkay = 1;
    if (check_exists(sqlq) != 0) 
      {
	tabledIsOkay = 0;
	failure = USER_NOT_IN_DOMN;
	return (USER_NOT_IN_DOMN);
      }
    tabledIsOkay = 0;
    /* does user have given access to data */
  
    sprintf(sqlq, "select distinct  t3.access_id from  %sMDAS_AD_REPL t4, %sMDAS_CD_USER t1 , %sMDAS_AU_GROUP t2, %sMDAS_AD_ACCS t3, %sMDAS_TD_DS_ACCS t5 where  t1.user_name = '%s' and t1.user_id = t2.user_id and t3.user_id = t2.group_user_id and t5.access_privilege like  '%%%s%%'  and t4.data_name = '%s' and t4.is_deleted = 0 and t4.data_id = t3.data_id and t3.access_id >= t5.access_id ",
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    MDASSCHEMENAME,
	    obj_user_name,  forAccess, obj_data_name);
    if (check_exists(sqlq) != 0)	
      {
	failure = NO_MODIFY_DATA_ACCS_TO_USER;
	return (NO_MODIFY_DATA_ACCS_TO_USER);
      }
    failure = NO_DATA_ACCS_TO_USER_IN_DOMN;
    return (NO_DATA_ACCS_TO_USER_IN_DOMN);
      }
    else 
      {
    truncate_end_blanks(data_size[0], data[0]);
    truncate_end_blanks(data_size[1], data[1]);
    truncate_end_blanks(data_size[2], data[2]);
    truncate_end_blanks(data_size[3], data[3]);
    *obj_data_id = atoi( (char *) data[0]);
    *replication_num = atoi( (char *) data[1]);
    *obj_user_id = atoi( (char *) data[2]);
    strcpy(versionString,  (char *) data[3]);
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
      }
}
int
getCollIdFromCollPathForUserIdAccs(char *srcCollId, char *collPath, 
				   int user_id, char *access)
{
  int i,j,k;
	  sprintf(sqlq, "select   t1.data_grp_id from %sMDAS_TD_DATA_GRP t1, %sMDAS_AU_GROUP t2, %sMDAS_AD_GRP_ACCS t4,  %sMDAS_TD_DS_ACCS t5  where t1.data_grp_name = '%s'  AND t2.user_id =  %i and   t4.user_id  =  t2.group_user_id AND t5.access_privilege like '%%%s%%' AND t4.data_grp_id = t1.data_grp_id AND t4.access_id = t5.access_id",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, 
	  MDASSCHEMENAME,  collPath, user_id,  access);
#ifdef DEBUGON 
  printf("getCollIdFromCollPathForUserIdAccs:%s\n",sqlq);
#endif /* DEBUGON */ 

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (i);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (i);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (i);
    i = get_row_db2(hstmt);
    if (i == 0) {
      truncate_end_blanks(data_size[0], data[0]);
      strcpy(srcCollId,(char *) data[0]);
    }
    done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(i);

}


int
getObjIdFromCollPathForUserIdAccs(char *collPath, int user_id, char *access)
{
  int i,j,k;
  char dataName[MAX_TOKEN];
  char collName[MAX_TOKEN];

  splitbychar(collPath,collName,dataName, '/');
     /*** June 15, 2001    added to make it look and use for user group access 
	  sprintf(sqlq, "select t1.data_id from %sMDAS_AD_REPL t1, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 where t1.data_name = '%s'  AND t1.is_deleted = 0 AND     t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND   ((t1.data_owner = %i) OR (t4.user_id  = %i AND t5.access_constraint like '%s%%' AND t4.data_id = t1.data_id AND t4.repl_enum = t1.repl_enum and t4.access_id = t5.access_id))", ***/
	  sprintf(sqlq, "select   t1.data_id from %sMDAS_AD_REPL t1, %sMDAS_AU_GROUP t2, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 where t1.data_name = '%s'  AND t1.is_deleted = 0 AND     t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND t2.user_id =  %i and  t4.user_id  =  t2.group_user_id AND t5.access_privilege like '%%%s%%' AND t4.data_id = t1.container_id AND t4.repl_enum = t1.repl_enum and t4.access_id = t5.access_id",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, dataName, collName, user_id,  access);
#ifdef DEBUGON 
  printf("getObjIdFromCollPathForUserIdAccs:%s\n",sqlq);
#endif /* DEBUGON */ 

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (i);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (i);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (i);
    i = get_row_db2(hstmt);
    if (i == 0) {
      truncate_end_blanks(data_size[0], data[0]);
      i = atoi( (char *) data[0]);
    }
    done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(i);
}


int
get_data_id_and_version_with_collection(char *obj_data_name,
                                char *obj_path_name, char *resource_name,
                                char *obj_user_name,  char *collection_name,
                                char *domain_name,
                                int *obj_data_id,  int *replication_num,
                                int *obj_user_id, char *versionString)
{

  if ( resource_name != NULL && strcmp( resource_name,""))
    {
     if ( collection_name != NULL && strcmp( collection_name,""))
	{

	  sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id, t1.version_num from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id=t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_privilege like '%%all%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME,
	  obj_data_name,  obj_path_name, 
	  collection_name, resource_name, obj_user_name, domain_name);
	}
      else
	{
	 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id, t1.version_num from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id=t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_privilege like '%%all%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", 
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name,  obj_path_name, 
	  resource_name, obj_user_name, domain_name);
	}
    }
else
  {
      if ( collection_name != NULL && strcmp( collection_name,""))
	{
	 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id, t1.version_num from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s'  AND t1.is_deleted = 0 AND     t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND   t3.user_name = '%s' AND  t3.user_id=t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_privilege like '%%all%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name, 
	  collection_name, obj_user_name, domain_name);
	}
      else
	{
	 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id, t1.version_num from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.is_deleted = 0 AND   t3.user_name = '%s' AND  t3.user_id=t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_privilege like '%%all%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", 
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name, 
	  obj_user_name, domain_name);
	}

  }
#ifdef DEBUGON 
  printf("GDIWC:%s\n",sqlq);
#endif /* DEBUGON */ 

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: Get_data_id: Data missin:%s:%s:%s/%s",
		obj_user_name, resource_name, obj_path_name,obj_data_name);
	sprintf(sqlq, "select distinct t1.data_id from  %sMDAS_AD_REPL t1 where t1.data_name = '%s' and t1.is_deleted = 0", 
	    MDASSCHEMENAME,obj_data_name);

	if (check_exists(sqlq) != 0) 
	  {
	    failure = DATA_NOT_IN_CAT;
	    return (DATA_NOT_IN_CAT);
	  }
    if (resource_name != NULL && strcmp( resource_name,""))
      {
	sprintf(sqlq, "select t1.rsrc_name from %sMDAS_AR_REPL t1 where t1.rsrc_name = '%s'",
		 MDASSCHEMENAME, resource_name);
	tabledIsOkay = 1;
	if (check_exists(sqlq) != 0) 
	  {
	    tabledIsOkay = 0;
	    failure = RESOURCE_NOT_IN_CAT;
	    return (RESOURCE_NOT_IN_CAT);
	  }
	tabledIsOkay = 0;
	sprintf(sqlq, "select t1.data_id from %sMDAS_AD_REPL t1  , %sMDAS_CD_RSRC t2   where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0  AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s'",
		 MDASSCHEMENAME, MDASSCHEMENAME, 
		obj_data_name, obj_path_name, resource_name);
	if (check_exists(sqlq) != 0) 
	  {
	    failure = DATA_NOT_IN_RESOURCE_OR_PATH;
	    return (DATA_NOT_IN_RESOURCE_OR_PATH);
	  }
      }
    /* data in given collection */
  if ( collection_name != NULL && strcmp( collection_name,""))
  {
    sprintf(sqlq, "select distinct  t1.data_id from  %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where  t1.data_name = '%s' and t1.is_deleted = 0 and   t1.data_grp_id = t2.data_grp_id and t2.data_grp_name = '%s'" ,
	    MDASSCHEMENAME,MDASSCHEMENAME,
	    obj_data_name, collection_name);
    if (check_exists(sqlq) != 0) 
      {
	failure = DATA_NOT_IN_COLLECTION;
	  return (DATA_NOT_IN_COLLECTION);
      }
  }

    /* is user in given domain */
    sprintf(sqlq, "select distinct  t1.user_id from  %sMDAS_CD_USER t1, %sMDAS_AU_DOMN t2, %sMDAS_TD_DOMN t3 where  t1.user_name = '%s' and   t1.user_id = t2.user_id and t2.domain_id = t3.domain_id  and t3.domain_desc = '%s'" ,
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    obj_user_name, domain_name);
    tabledIsOkay = 1;
    if (check_exists(sqlq) != 0) 
      {
	tabledIsOkay = 0;
	failure = USER_NOT_IN_DOMN;
	return (USER_NOT_IN_DOMN);
      }
    tabledIsOkay = 0;
    /* does user have given access to data */
  
    sprintf(sqlq, "select distinct  t3.access_id from  %sMDAS_AD_REPL t4, %sMDAS_CD_USER t1 , %sMDAS_AU_GROUP t2, %sMDAS_AD_ACCS t3, %sMDAS_TD_DS_ACCS t5 where  t1.user_name = '%s' and t1.user_id = t2.user_id and t3.user_id = t2.group_user_id and t5.access_privilege like  '%%all%%'  and t4.data_name = '%s' and t4.is_deleted = 0 and t4.data_id = t3.data_id and t3.access_id >= t5.access_id ",
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    MDASSCHEMENAME,
	    obj_user_name,  obj_data_name);
    if (check_exists(sqlq) != 0)	
      {
	failure = NO_MODIFY_DATA_ACCS_TO_USER;
	return (NO_MODIFY_DATA_ACCS_TO_USER);
      }
    failure = NO_DATA_ACCS_TO_USER_IN_DOMN;
    return (NO_DATA_ACCS_TO_USER_IN_DOMN);
      }
    else 
      {
    truncate_end_blanks(data_size[0], data[0]);
    truncate_end_blanks(data_size[1], data[1]);
    truncate_end_blanks(data_size[2], data[2]);
    truncate_end_blanks(data_size[3], data[3]);
    *obj_data_id = atoi( (char *) data[0]);
    *replication_num = atoi( (char *) data[1]);
    *obj_user_id = atoi( (char *) data[2]);
    strcpy(versionString,  (char *) data[3]);
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
      }


}
int get_data_id_with_collection(char *obj_data_name,
				char *obj_path_name, char *resource_name,
				char *obj_user_name,  char *collection_name,
				char *domain_name,
				int *obj_data_id,  int *replication_num, 
				int *obj_user_id)
{

 
  if ( resource_name != NULL && strcmp( resource_name,""))
    {
     if ( collection_name != NULL && strcmp( collection_name,""))
	{
     /*** June 15, 2001  added to make it look and use for user group access 
	  sprintf(sqlq, "select t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND ((t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_constraint like 'all%%') OR t1.data_owner = t3.user_id) AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", ***/
	  sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id=t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_privilege like '%%all%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME,
	  obj_data_name,  obj_path_name, 
	  collection_name, resource_name, obj_user_name, domain_name);
	}
      else
	{
     /*** June 15, 2001  added to make it look and use for user group access 
	  sprintf(sqlq, "select t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND ((t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_constraint like 'all%%') OR t1.data_owner = t3.user_id) AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", ***/
	 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id=t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_privilege like '%%all%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", 
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name,  obj_path_name, 
	  resource_name, obj_user_name, domain_name);
	}
    }
else
  {
      if ( collection_name != NULL && strcmp( collection_name,""))
	{
	/*** June 15, 2001   added to make it look and use for user group access 
	 sprintf(sqlq, "select t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s'  AND t1.is_deleted = 0 AND     t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND   t3.user_name = '%s' AND ((t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_constraint like 'all%%') OR t1.data_owner = t3.user_id) AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", ***/
	 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s'  AND t1.is_deleted = 0 AND     t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND   t3.user_name = '%s' AND  t3.user_id=t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_privilege like '%%all%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name, 
	  collection_name, obj_user_name, domain_name);
	}
      else
	{
     /*** June 15, 2001  added to make it look and use for user group access 
	 sprintf(sqlq, "select t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.is_deleted = 0 AND   t3.user_name = '%s' AND ((t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_constraint like 'all%%') OR t1.data_owner = t3.user_id) AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", ***/
	 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.is_deleted = 0 AND   t3.user_name = '%s' AND  t3.user_id=t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_privilege like '%%all%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", 
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name, 
	  obj_user_name, domain_name);
	}

  }
#ifdef DEBUGON 
  printf("GDIWC:%s\n",sqlq);
#endif /* DEBUGON */ 

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: Get_data_id: Data missin:%s:%s:%s/%s",
		obj_user_name, resource_name, obj_path_name,obj_data_name);
	sprintf(sqlq, "select distinct t1.data_id from  %sMDAS_AD_REPL t1 where t1.data_name = '%s' and t1.is_deleted = 0", 
	    MDASSCHEMENAME,obj_data_name);

	if (check_exists(sqlq) != 0) 
	  {
	    failure = DATA_NOT_IN_CAT;
	    return (DATA_NOT_IN_CAT);
	  }
    if (resource_name != NULL && strcmp( resource_name,""))
      {
	sprintf(sqlq, "select t1.rsrc_name from %sMDAS_AR_REPL t1 where t1.rsrc_name = '%s'",
		 MDASSCHEMENAME, resource_name);
	tabledIsOkay = 1;
	if (check_exists(sqlq) != 0) 
	  {
	    tabledIsOkay = 0;
	    failure = RESOURCE_NOT_IN_CAT;
	    return (RESOURCE_NOT_IN_CAT);
	  }
	tabledIsOkay = 0;
	sprintf(sqlq, "select t1.data_id from %sMDAS_AD_REPL t1  , %sMDAS_CD_RSRC t2   where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0  AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s'",
		 MDASSCHEMENAME, MDASSCHEMENAME, 
		obj_data_name, obj_path_name, resource_name);
	if (check_exists(sqlq) != 0) 
	  {
	    failure = DATA_NOT_IN_RESOURCE_OR_PATH;
	    return (DATA_NOT_IN_RESOURCE_OR_PATH);
	  }
      }
    /* data in given collection */
  if ( collection_name != NULL && strcmp( collection_name,""))
  {
    sprintf(sqlq, "select distinct  t1.data_id from  %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where  t1.data_name = '%s' and t1.is_deleted = 0 and   t1.data_grp_id = t2.data_grp_id and t2.data_grp_name = '%s'" ,
	    MDASSCHEMENAME,MDASSCHEMENAME,
	    obj_data_name, collection_name);
    if (check_exists(sqlq) != 0) 
      {
	failure = DATA_NOT_IN_COLLECTION;
	  return (DATA_NOT_IN_COLLECTION);
      }
  }

    /* is user in given domain */
    sprintf(sqlq, "select distinct  t1.user_id from  %sMDAS_CD_USER t1, %sMDAS_AU_DOMN t2, %sMDAS_TD_DOMN t3 where  t1.user_name = '%s' and   t1.user_id = t2.user_id and t2.domain_id = t3.domain_id  and t3.domain_desc = '%s'" ,
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    obj_user_name, domain_name);
    tabledIsOkay = 1;
    if (check_exists(sqlq) != 0) 
      {
	tabledIsOkay = 0;
	failure = USER_NOT_IN_DOMN;
	return (USER_NOT_IN_DOMN);
      }
    tabledIsOkay = 0;
    /* does user have given access to data */
  
    sprintf(sqlq, "select distinct  t3.access_id from  %sMDAS_AD_REPL t4, %sMDAS_CD_USER t1 , %sMDAS_AU_GROUP t2, %sMDAS_AD_ACCS t3, %sMDAS_TD_DS_ACCS t5 where  t1.user_name = '%s' and t1.user_id = t2.user_id and t3.user_id = t2.group_user_id and t5.access_privilege like  '%%all%%'  and t4.data_name = '%s' and t4.is_deleted = 0 and t4.data_id = t3.data_id and t3.access_id >= t5.access_id ",
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    MDASSCHEMENAME,
	    obj_user_name,  obj_data_name);
    if (check_exists(sqlq) != 0)	
      {
	failure = NO_MODIFY_DATA_ACCS_TO_USER;
	return (NO_MODIFY_DATA_ACCS_TO_USER);
      }
    failure = NO_DATA_ACCS_TO_USER_IN_DOMN;
    return (NO_DATA_ACCS_TO_USER_IN_DOMN);
      }
    else 
      {
    truncate_end_blanks(data_size[0], data[0]);
    truncate_end_blanks(data_size[1], data[1]);
    truncate_end_blanks(data_size[2], data[2]);
    *obj_data_id = atoi( (char *) data[0]);
    *replication_num = atoi( (char *) data[1]);
    *obj_user_id = atoi( (char *) data[2]);
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
      }
}

int get_data_id_with_collection_withanyaccess(char *obj_data_name,
				char *obj_path_name, char *resource_name,
				char *obj_user_name,  char *collection_name,
				char *domain_name,
				int *obj_data_id,  int *replication_num, 
				int *obj_user_id)
{

 
  if ( resource_name != NULL && strcmp( resource_name,""))
    {
     if ( collection_name != NULL && strcmp( collection_name,""))
	{
     /*** June 15, 2001  added to make it look and use for user group access 
	 sprintf(sqlq, "select t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_constraint like '%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", ***/
	 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id = t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_constraint like '%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", 
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, 
	  obj_data_name,  obj_path_name, 
	  collection_name, resource_name, obj_user_name, domain_name);
	}
      else
	{
     /*** June 15, 2001  added to make it look and use for user group access
	 sprintf(sqlq, "select t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_constraint like '%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", ***/
	 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id = t9.user_id and t9.group_user_id  = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_constraint like '%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name,  obj_path_name, 
	  resource_name, obj_user_name, domain_name);
	}
    }
else
  {
      if ( collection_name != NULL && strcmp( collection_name,""))
	{
     /*** June 15, 2001   added to make it look and use for user group access
	 sprintf(sqlq, "select t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s'  AND t1.is_deleted = 0 AND     t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND   t3.user_name = '%s' AND t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_constraint like '%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", ***/
	 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s'  AND t1.is_deleted = 0 AND     t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND   t3.user_name = '%s' AND t3.user_id = t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_constraint like '%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name, 
	  collection_name, obj_user_name, domain_name);
	}
      else
	{
     /*** June 15, 2001  added to make it look and use for user group access
	 sprintf(sqlq, "select t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.is_deleted = 0 AND   t3.user_name = '%s' AND t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_constraint like '%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", ***/
	 sprintf(sqlq, "select   t1.data_id, t1.repl_enum, t3.user_id from %sMDAS_AD_REPL t1 ,  %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8  where t1.data_name = '%s' and t1.is_deleted = 0 AND   t3.user_name = '%s' AND t3.user_id = t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id = t5.access_id AND t5.access_constraint like '%%' AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id = t3.user_id ", 
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name, 
	  obj_user_name, domain_name);
	}

  }
#ifdef DEBUGON 
  printf("GDIWCWNA:%s\n",sqlq);
#endif /* DEBUGON */ 

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: Get_data_id: Data missin:%s:%s:%s/%s",
		obj_user_name, resource_name, obj_path_name,obj_data_name);
	sprintf(sqlq, "select distinct t1.data_id from  %sMDAS_AD_REPL t1 where t1.data_name = '%s' and t1.is_deleted = 0", 
	    MDASSCHEMENAME,obj_data_name);

	if (check_exists(sqlq) != 0) 
	  {
	    failure = DATA_NOT_IN_CAT;
	    return (DATA_NOT_IN_CAT);
	  }
	sprintf(sqlq, "select t1.rsrc_name from %sMDAS_AR_REPL t1 where t1.rsrc_name = '%s'",
		 MDASSCHEMENAME, resource_name);
	tabledIsOkay = 1;
	if (check_exists(sqlq) != 0) 
	  {
	    tabledIsOkay = 0;
	    failure = RESOURCE_NOT_IN_CAT;
	    return (RESOURCE_NOT_IN_CAT);
	  }
	tabledIsOkay = 0;
	sprintf(sqlq, "select t1.data_id from %sMDAS_AD_REPL t1  , %sMDAS_CD_RSRC t2   where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0  AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s'",
		 MDASSCHEMENAME, MDASSCHEMENAME, 
		obj_data_name, obj_path_name, resource_name);
	if (check_exists(sqlq) != 0) 
	  {
	    failure = DATA_NOT_IN_RESOURCE_OR_PATH;
	    return (DATA_NOT_IN_RESOURCE_OR_PATH);
	  }

    /* data in given collection */
    sprintf(sqlq, "select distinct  t1.data_id from  %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where  t1.data_name = '%s' and t1.is_deleted = 0 and   t1.data_grp_id = t2.data_grp_id and t2.data_grp_name = '%s'" ,
	    MDASSCHEMENAME,MDASSCHEMENAME,
	    obj_data_name, collection_name);
    if (check_exists(sqlq) != 0) 
      {
	failure = DATA_NOT_IN_COLLECTION;
	  return (DATA_NOT_IN_COLLECTION);
      }


    /* is user in given domain */
    sprintf(sqlq, "select distinct  t1.user_id from  %sMDAS_CD_USER t1, %sMDAS_AU_DOMN t2, %sMDAS_TD_DOMN t3 where  t1.user_name = '%s' and   t1.user_id = t2.user_id and t2.domain_id = t3.domain_id  and t3.domain_desc = '%s'" ,
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    obj_user_name, domain_name);
    tabledIsOkay = 1;
    if (check_exists(sqlq) != 0) 
      {
	tabledIsOkay = 0;
	failure = USER_NOT_IN_DOMN;
	return (USER_NOT_IN_DOMN);
      }
    tabledIsOkay = 0;

    /* does user haa any access to data */
  
    sprintf(sqlq, "select distinct  t3.access_id from %sMDAS_AD_REPL t4, %sMDAS_CD_USER t1 , %sMDAS_AU_GROUP t2, %sMDAS_AD_ACCS t3,  %sMDAS_TD_DS_ACCS t5 where  t1.user_name = '%s' and t1.user_id = t2.user_id and t3.user_id = t2.group_user_id and t5.access_constraint like  '%%' and t4.data_name = '%s' and t4.is_deleted = 0 and t4.data_id = t3.data_id and t3.access_id >= t5.access_id ",
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    MDASSCHEMENAME,
	    obj_user_name,  obj_data_name);
    if (check_exists(sqlq) != 0)	
      {
	failure = NO_MODIFY_DATA_ACCS_TO_USER;
	return (NO_MODIFY_DATA_ACCS_TO_USER);
      }
    failure = NO_DATA_ACCS_TO_USER_IN_DOMN;
    return (NO_DATA_ACCS_TO_USER_IN_DOMN);
      }
    else 
      {
    truncate_end_blanks(data_size[0], data[0]);
    truncate_end_blanks(data_size[1], data[1]);
    truncate_end_blanks(data_size[2], data[2]);
    *obj_data_id = atoi( (char *) data[0]);
    *replication_num = atoi( (char *) data[1]);
    *obj_user_id = atoi( (char *) data[2]);
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
      }
}

int
getDataInfo(int dataId, char atval[][MAX_DATA_SIZE])
{
  char coloutnames[1000];

   strcpy(coloutnames, " t1.data_id,  t1.repl_enum,  t1.data_name,  t1.data_typ_id,  t1.path_name,  t1.rsrc_id,  t1.repl_timestamp,  t1.data_grp_id,  t1.data_size,  t1.is_deleted, t1.data_owner, t1.is_dirty, t1.offset, t1.container_id,t1.container_td_id, t1.d_comment,t1.datalock,t1.lockowner,t1.version_num,t1.auditflag, t1.seg_num, t1.dlockexpiry, t1.dchecksum, t1.dhide,t1.modify_timestamp, t1.last_accs_time");


   sprintf(sqlq, "select %s from %sMDAS_AD_REPL t1 where t1.data_id = %i",
	   coloutnames,MDASSCHEMENAME,dataId);
   i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
   if (i != 0) return (MDAS_FAILURE);
   i = get_no_of_columns_db2(hstmt, &num_of_cols);
   if (i != 0) return (MDAS_FAILURE);
   i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
   if (i != 0) return (MDAS_FAILURE);
   i = get_row_db2(hstmt);
   if (i != 0) 
     {
       i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
       failure = NO_DATA_REPLICA_IN_CAT;
       return (NO_DATA_REPLICA_IN_CAT);
     }
   else 
     {
       for (i=0; i < num_of_cols; i++){
	 truncate_end_blanks(data_size[i], data[i]);
	 strcpy(atval[i],  (char *) data[i]);
       }
       i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
       return(MDAS_SUCCESS);
     }
}

int
get_data_repl_info_with_data_id_repl_num(int data_id, 
					 int rnum, 
					 char * verNum, 
					 char atval[][MAX_DATA_SIZE])
{
  char coloutnames[1000];

  strcpy(coloutnames, " t1.data_id,  t1.repl_enum,  t1.data_name,  t1.data_typ_id,  t1.path_name,  t1.rsrc_id,  t1.modify_timestamp,  t1.data_grp_id,  t1.data_size,  t1.is_deleted, t1.data_owner, t1.is_dirty, t1.offset, t1.container_id,t1.datalock,t1.lockowner,t1.version_num,t1.container_td_id, t1.d_comment,t1.modify_timestamp, t1.last_accs_time, t1.auditflag, t1.seg_num, t1.dlockexpiry, t1.dchecksum, t1.dhide");

  sprintf(sqlq, "select %s  from %sMDAS_AD_REPL t1 where t1.data_id = %i and t1.repl_enum = %i and t1.version_num = '%s'",
	  coloutnames,MDASSCHEMENAME,data_id,rnum,verNum);

  i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
  if (i != 0) return (MDAS_FAILURE);
  i = get_no_of_columns_db2(hstmt, &num_of_cols);
  if (i != 0) return (MDAS_FAILURE);
  i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
  if (i != 0) return (MDAS_FAILURE);
  i = get_row_db2(hstmt);
  if (i != 0)
    {
      i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
      failure = NO_DATA_REPLICA_IN_CAT;
      return (NO_DATA_REPLICA_IN_CAT);
    }
  else
    {
      for (i=0; i < num_of_cols; i++){
	truncate_end_blanks(data_size[i], data[i]);
	strcpy(atval[i],  (char *) data[i]);
      }
      i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
      return(MDAS_SUCCESS);
    }
}  
  


int
get_data_repl_info(char *obj_data_name, char *obj_path_name, 
		   char *resource_name, 
		   char *collection_name, char *obj_user_name, 
		   char * domain_name,
		   char atval[][MAX_DATA_SIZE])
{
  char coloutnames[1000];

  strcpy(coloutnames, " t1.data_id,  t1.repl_enum,  t1.data_name,  t1.data_typ_id,  t1.path_name,  t1.rsrc_id,  t1.modify_timestamp,  t1.data_grp_id,  t1.data_size,  t1.is_deleted, t1.data_owner, t1.is_dirty, t1.offset, t1.container_id,t1.datalock,t1.lockowner,t1.version_num,t1.container_td_id, t1.d_comment,t1.modify_timestamp, t1.last_accs_time, t1.auditflag, t1.seg_num, t1.dlockexpiry, t1.dchecksum, t1.dhide");



      if ( collection_name != NULL && strcmp( collection_name,""))
	{
/* changed to accomodate use of dataset & collection without
***   original resource or path being given 
	      sprintf(sqlq, "select t1.*  from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_constraint like '%%read%%'  AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id =  t3.user_id",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME,MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name,  obj_path_name, 
	  collection_name, resource_name, obj_user_name, domain_name);
*/
	  if (resource_name != NULL && strcmp( resource_name,"") &&
	      obj_path_name != NULL && strcmp( obj_path_name ,""))
	    {
     /*** June 15, 2001  added to make it look and use for user group access
	      sprintf(sqlq, "select  %s  from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_constraint like '%%read%%'  AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id =  t3.user_id",***/
	      sprintf(sqlq, "select  %s  from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id = t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_privilege like '%%read%%'  AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id =  t3.user_id",
		      coloutnames,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME,MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME,
	  obj_data_name,  obj_path_name, 
	  collection_name, resource_name, obj_user_name, domain_name);
	    }
	  else
	    {
     /*** June 15, 2001  added to make it look and use for user group access
	      sprintf(sqlq, "select  %s  from %sMDAS_AD_REPL t1 , %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s' and  t1.is_deleted = 0 AND t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND t3.user_name = '%s' AND t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_constraint like '%%read%%'  AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id =  t3.user_id",***/
	      sprintf(sqlq, "select  %s  from %sMDAS_AD_REPL t1 , %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5  , %sMDAS_TD_DATA_GRP t6 , %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8 where t1.data_name = '%s' and  t1.is_deleted = 0 AND t6.data_grp_name = '%s' and t6.data_grp_id = t1.data_grp_id AND t3.user_name = '%s' AND t3.user_id = t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_privilege like '%%read%%'  AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id =  t3.user_id",
		      coloutnames,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME, MDASSCHEMENAME,MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name,  
	  collection_name, obj_user_name, domain_name);
	    }
	}
      else
	{
     /*** June 15, 2001  added to make it look and use for user group access
	  sprintf(sqlq, "select %s from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8   where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_constraint like '%%read%%'  AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id =  t3.user_id", ***/
	  sprintf(sqlq, "select %s from %sMDAS_AD_REPL t1 , %sMDAS_CD_RSRC t2, %sMDAS_CD_USER t3, %sMDAS_AU_GROUP t9, %sMDAS_AD_ACCS t4,  %sMDAS_TD_DS_ACCS t5, %sMDAS_TD_DOMN t7 , %sMDAS_AU_DOMN t8   where t1.data_name = '%s' and t1.path_name = '%s' AND t1.is_deleted = 0 AND  t1.rsrc_id = t2.rsrc_id AND t2.rsrc_name = '%s' AND t3.user_name = '%s' AND t3.user_id = t9.user_id and t9.group_user_id = t4.user_id AND t1.data_id = t4.data_id AND t1.repl_enum = t4.repl_enum and t4.access_id >= t5.access_id AND t5.access_privilege like '%%read%%'  AND t7.domain_desc = '%s' AND t7.domain_id = t8.domain_id AND t8.user_id =  t3.user_id",
		 coloutnames,
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME, MDASSCHEMENAME,
	  obj_data_name,  obj_path_name, 
	  resource_name, obj_user_name, domain_name);
	}

#ifdef DEBUGON 
      if (DEBUGON) printf("GDRI:%s\n",sqlq);
#endif /* DEBUGON */ 

    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: Get_data_repl_info: %s:%s/%s", resource_name, obj_path_name,obj_data_name);
	failure = NO_DATA_REPLICA_IN_CAT;
	return (NO_DATA_REPLICA_IN_CAT);
      }
    else 
      {
	for (i=0; i < num_of_cols; i++){
	  truncate_end_blanks(data_size[i], data[i]);
	  strcpy(atval[i],  (char *) data[i]);
	}
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(MDAS_SUCCESS);
      }
}

int get_max_repl_num(int obj_data_id)
{

    int r_num;

  sprintf(sqlq,"select max(repl_enum) from %sMDAS_AD_REPL where data_id = %i",
	  MDASSCHEMENAME,  obj_data_id);
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);

    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: Get_max_repl_num: Data missin:%s",sqlq);
	failure = DATA_NOT_IN_CAT;
	return (DATA_NOT_IN_CAT);
      }
    else 
      {
    truncate_end_blanks(data_size[0], data[0]);
    r_num = 1 + atoi( (char *) data[0]);
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(r_num);
      }

}


int get_max_version_num(char *verNum,  int obj_data_id, int repl_enum)
{

    int r_num;
    if (repl_enum < 0)
      sprintf(sqlq,"select max(version_num) from %sMDAS_AD_REPL where data_id = %i",
	  MDASSCHEMENAME,  obj_data_id);
    else
      sprintf(sqlq,"select max(version_num) from %sMDAS_AD_REPL where data_id = %i and repl_enum = %i",
	  MDASSCHEMENAME,  obj_data_id, repl_enum);
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: Get_max_repl_num: Data missin:%s",sqlq);
	failure = DATA_NOT_IN_CAT;
	return (DATA_NOT_IN_CAT);
      }
    else 
      {
	truncate_end_blanks(data_size[0], data[0]);
	/* r_num = 1 + atoi( (char *) data[0]); */
	if (atoi( (char *) data[0]) > 0 )
	  sprintf(verNum,"%i",1 + atoi( (char *) data[0])); 
	else if (strlen((char *)data[0]) == 1 && data[0][0] == '0')
	  strcpy(verNum,"1");
	else /* this  isa string */
	  sprintf(verNum,"%s.%s",data[0],timeval);
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	return(0);
      }

}

/****
int get_next_counter_value(char name[])
{
 
    sprintf(sqlq, "select cvalue from %sMDAS_COUNTER where cname = '%s'", 
            MDASSCHEMENAME, name);
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: Data missing: %s",name);
	failure = METADATA_COUNTER_ERROR;
	return (METADATA_COUNTER_ERROR);
      }
    else 
      {
	truncate_end_blanks(data_size[0], data[0]);
	j = atoi((char *) data[0]);
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	sprintf(sqlq,"update %sMDAS_COUNTER set cvalue = %i where cname ='%s'",
		MDASSCHEMENAME, j+1, name);
	i = exec_no_res_sql_db2(henv, hdbc, (unsigned char *)sqlq);
	if (i != 0) 
	  {
	    error_exit("MDAS registry: COUNTER setting error");
	    return(METADATA_COUNTER_ERROR);
	  }
	return(j);
      }
}
****/

int 
roundRobinLogRsrc(int rsrcNum, int phyRsrcNum[],int count)
{
  int i,rowcount;
  char cval[USABLE_TABLE_COLS][MAX_TOKEN];

    sprintf(sqlq, "lock table %sMDAS_AR_LRSRC_RRBN in exclusive mode", 
            MDASSCHEMENAME);
    i = exec_no_res_sql_db2(henv, hdbc, (unsigned char *) sqlq);
    if (i != 0) {        
      sprintf(errmess,"roundRobinLogRsrc: locking error for %i",rsrcNum);
      error_exit(errmess);
      failure = TABLE_LOCK_ERROR;
      return (TABLE_LOCK_ERROR);
    }
    sprintf(sqlq, "select phy_rsrc_id from %sMDAS_AR_LRSRC_RRBN where rsrc_id = %i",
	    MDASSCHEMENAME, rsrcNum);
rowcount = 1;
    i = get_row_from_query(cval,&rowcount,sqlq);
    if (i < 0 || rowcount == 0) {
      sprintf(sqlq, "insert into %sMDAS_AR_LRSRC_RRBN values (%i,%i)",
	      MDASSCHEMENAME,rsrcNum,phyRsrcNum[0]);
      i = exec_no_res_sql_db2(henv, hdbc, (unsigned char *)sqlq);
      if (i != 0) 
	{
	  error_exit("MDAS registry: roundRobinLogRsrc new insertion error");
	  strcpy(GENERATEDSQL,sqlq);
	  failure = MCAT_EXEC_ERROR;
	  return(i);
      }

    }    
    j = atoi((char *) cval[0]);
    for (i = 0; i < count ; i++) {
      if (phyRsrcNum[i] == j) break;
    }
    if (i == count) j = phyRsrcNum[0];
    else if (i == (count-1)) j = phyRsrcNum[0];
    else j = phyRsrcNum[i+1];
    sprintf(sqlq,"update %sMDAS_AR_LRSRC_RRBN set phy_rsrc_id = %i where rsrc_id = %i",
	    MDASSCHEMENAME, j,  rsrcNum);
    i = exec_no_res_sql_db2(henv, hdbc, (unsigned char *)sqlq);
    if (i != 0) 
      {
	strcpy(GENERATEDSQL,sqlq);
	error_exit("MDAS registry: roundRobinLogRsrc setting error");
	failure = MCAT_EXEC_ERROR;
	return(i);
      }
    commit_db2_interaction(0);
    return(j);
}

int get_next_counter_value(char name[])
{

    sprintf(sqlq, "lock table %sMDAS_COUNTER in exclusive mode", 
            MDASSCHEMENAME);
    i = exec_no_res_sql_db2(henv, hdbc, (unsigned char *) sqlq);
    if (i != 0) {        
      sprintf(errmess,"MDAS registry: locking error for %s",name);
      error_exit(errmess);
      failure = METADATA_COUNTER_ERROR;
      return (METADATA_COUNTER_ERROR);
    }

    sprintf(sqlq, "select cvalue from %sMDAS_COUNTER where cname = '%s'", 
            MDASSCHEMENAME, name);
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) {        
      sprintf(errmess,"MDAS registry: MDAS_COUNTER access error1: %s",name);
      error_exit(errmess);
      failure = METADATA_COUNTER_ERROR;
      return (METADATA_COUNTER_ERROR);
    }
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0){        
      sprintf(errmess,"MDAS registry: MDAS_COUNTER access error2: %s",name);
      error_exit(errmess);
      failure = METADATA_COUNTER_ERROR;
      return (METADATA_COUNTER_ERROR);
    }
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0)  {        
      sprintf(errmess,"MDAS registry: MDAS_COUNTER access error3: %s",name);
      error_exit(errmess);
      failure = METADATA_COUNTER_ERROR;
      return (METADATA_COUNTER_ERROR);
    }
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
        sprintf(errmess,"MDAS registry: MDAS_COUNTER access error4: %s",name);
        error_exit(errmess);
	failure = METADATA_COUNTER_ERROR;
	return (METADATA_COUNTER_ERROR);
      }
    else 
      {
	truncate_end_blanks(data_size[0], data[0]);
	j = atoi((char *) data[0]);
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	sprintf(sqlq,"update %sMDAS_COUNTER set cvalue = %i where cname ='%s'",
		MDASSCHEMENAME, j+1, name);
	i = exec_no_res_sql_db2(henv, hdbc, (unsigned char *)sqlq);
	if (i != 0) 
	  {
	    error_exit("MDAS registry: COUNTER setting error");
	    failure = METADATA_COUNTER_ERROR;
	    return(METADATA_COUNTER_ERROR);
	  }
	commit_db2_interaction(0);
	return(j);
      }
}

int insertIntoTable(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount)
{
  if (strstr(tabname,MDASSCHEMENAME) == tabname)
    sprintf(sqlq, "insert into %s (", tabname);
  else
    sprintf(sqlq, "insert into %s%s (",MDASSCHEMENAME, tabname);
  sprintf(sqlq, "%s  %s", sqlq, atname[0]);
  for (i = 1; i < atcount; i++)
    {
       sprintf(sqlq, "%s , %s", sqlq, atname[i]);
    }
  sprintf(sqlq, "%s ) values (", sqlq);
  sprintf(sqlq, "%s %s", sqlq, atval[0]);
  for (i = 1; i < atcount; i++)
    {
       sprintf(sqlq, "%s , %s", sqlq, atval[i]);
    }
  sprintf(sqlq, "%s )", sqlq); 

  i = ExecSqlDb2(henv, hdbc,  sqlq);
  if (i != 0)  
    {
      sprintf(errmess,"Data Insertion Error: %s",sqlq);
      error_exit(errmess);
	failure = METADATA_INSERTION_ERROR;
      return(METADATA_INSERTION_ERROR);
    }
  return(MDAS_SUCCESS);
}


int insertIntoTableDontCare(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount)
{

  if (strstr(tabname,MDASSCHEMENAME) == tabname)
    sprintf(sqlq, "insert into %s (", tabname);
  else
    sprintf(sqlq, "insert into %s%s (",MDASSCHEMENAME, tabname);
  sprintf(sqlq, "%s  %s", sqlq, atname[0]);
  for (i = 1; i < atcount; i++)
    {
       sprintf(sqlq, "%s , %s", sqlq, atname[i]);
    }
  sprintf(sqlq, "%s ) values (", sqlq);
  sprintf(sqlq, "%s %s", sqlq, atval[0]);
  for (i = 1; i < atcount; i++)
    {
       sprintf(sqlq, "%s , %s", sqlq, atval[i]);
    }
  sprintf(sqlq, "%s )", sqlq);
  i = ExecSqlDb2(henv, hdbc,  sqlq);
  if (i != 0)  
    {

      return(METADATA_INSERTION_ERROR);
    }
  return(MDAS_SUCCESS);
}

int
deleteFromTableWithAllDataAccessCheck(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount, int accsId, int userId)
{
  /* assumes that the data_id attribute is given first */ 
  if (atcount == 0)
     return(MDAS_FAILURE);  
#ifdef PSQMCAT
  if (strstr(tabname,MDASSCHEMENAME) == tabname)
    sprintf(sqlq, "delete from %s  WHERE %s = %s ",
	    tabname,atname[0], atval[0]);
  else
    sprintf(sqlq, "delete from %s%s   WHERE %s = %s ",
	    MDASSCHEMENAME, tabname,atname[0], atval[0]);
  for (i = 1; i < atcount; i++) {
      sprintf(sqlq, "%s AND %s = %s", sqlq, atname[i], atval[i]);
  }
  sprintf(sqlq, "%s and exists (select B.data_id, B.repl_enum from %sMDAS_AD_ACCS B where B.data_id = %s and B.repl_enum= repl_enum and  B.user_id = %i and B.access_id >= %i",
	  sqlq, MDASSCHEMENAME,atname[0], userId, accsId);
#else
  if (strstr(tabname,MDASSCHEMENAME) == tabname)
    sprintf(sqlq, "delete from %s A  WHERE A.%s = %s ",
	    tabname,atname[0], atval[0]);
  else
    sprintf(sqlq, "delete from %s%s A  WHERE A.%s = %s ",
	    MDASSCHEMENAME, tabname,atname[0], atval[0]);
  for (i = 1; i < atcount; i++) {
      sprintf(sqlq, "%s AND %s = %s", sqlq, atname[i], atval[i]);
  }
  sprintf(sqlq, "%s and exists (select B.data_id, B.repl_enum from %sMDAS_AD_ACCS B where B.data_id = A.%s and B.repl_enum= A.repl_enum and  B.user_id = %i and B.access_id >= %i",
	  sqlq, MDASSCHEMENAME,atname[0], userId, accsId);
#endif
  i = ExecSqlDb2(henv, hdbc, sqlq);
  if (i != 0)  
    {
      sprintf(errmess,"Data Deletion  Error: %s",sqlq);
	failure = METADATA_DELETION_ERROR;
      return(METADATA_DELETION_ERROR);
    }
  return(MDAS_SUCCESS);

}
int deleteFromTableWithCondition(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount)
{
  if (atcount == 0)
     return(MDAS_FAILURE);
  if (strstr(tabname,MDASSCHEMENAME) == tabname)
    sprintf(sqlq, "delete from %s ",tabname);
  else
    sprintf(sqlq, "delete from %s%s ",MDASSCHEMENAME, tabname);
  for (i = 0; i < atcount; i++)
    {
      if (i > 0)
	{
	  sprintf(sqlq, "%s AND %s  %s", sqlq, atname[i], atval[i]);
	}
      else
	{
	  sprintf(sqlq, "%s  WHERE %s  %s", sqlq, atname[i], atval[i]);
	}
	  
    }

  i = ExecSqlDb2(henv, hdbc, sqlq);
  if (i != 0)  
    {
      sprintf(errmess,"Data Deletion  Error: %s",sqlq);
	failure = METADATA_DELETION_ERROR;
      return(METADATA_DELETION_ERROR);
    }
  return(MDAS_SUCCESS);
}

int deleteFromTable(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount)
{
  if (atcount == 0)
     return(MDAS_FAILURE);
  if (strstr(tabname,MDASSCHEMENAME) == tabname)
    sprintf(sqlq, "delete from %s ",tabname);
  else
    sprintf(sqlq, "delete from %s%s ",MDASSCHEMENAME, tabname);
  for (i = 0; i < atcount; i++)
    {
      if (i > 0)
	{
	  sprintf(sqlq, "%s AND %s = %s", sqlq, atname[i], atval[i]);
	}
      else
	{
	  sprintf(sqlq, "%s  WHERE %s = %s", sqlq, atname[i], atval[i]);
	}
	  
    }

  i = ExecSqlDb2(henv, hdbc, sqlq);
  if (i != 0)  
    {
      sprintf(errmess,"Data Deletion  Error: %s",sqlq);
	failure = METADATA_DELETION_ERROR;
      return(METADATA_DELETION_ERROR);
    }
  return(MDAS_SUCCESS);
}

int changeAttrValue(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount)
{
  if (strstr(tabname,MDASSCHEMENAME) == tabname)
    sprintf(sqlq, "update %s set %s = %s ",tabname,atname[0], atval[0]);
  else
    sprintf(sqlq, "update %s%s set %s = %s ",MDASSCHEMENAME, tabname,
	    atname[0], atval[0]);
  for (i = 1; i < atcount; i++)
    {
      if (i > 1)
	{
	  sprintf(sqlq, "%s AND %s = %s", sqlq, atname[i], atval[i]);
	}
      else
	{
	  sprintf(sqlq, "%s  WHERE %s = %s", sqlq, atname[i], atval[i]);
	}
	  
    }
  i = ExecSqlDb2(henv, hdbc, sqlq);
  if (i != 0)  
    {
      sprintf(errmess,"Data Update Error: %s",sqlq);
	failure = METADATA_UPDATE_ERROR;
      return(METADATA_UPDATE_ERROR);
    }
  return(MDAS_SUCCESS);
}

int changeAttrValueSetWithCondition(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount, int setCount)
{

  if (strstr(tabname,MDASSCHEMENAME) == tabname)
    sprintf(sqlq, "update %s set ", tabname);
  else
    sprintf(sqlq, "update %s%s set ", MDASSCHEMENAME, tabname);
  for (i = 0 ; i < setCount; i++) {
    if (i > 0) strcat(sqlq,",");
    sprintf(sqlq, "%s %s = %s", sqlq, atname[i],atval[i]);
  }
  for (i = setCount; i < atcount; i++)
    {
      if (i > setCount)
	  sprintf(sqlq, "%s AND %s  %s", sqlq, atname[i], atval[i]);
      else
	  sprintf(sqlq, "%s  WHERE %s  %s", sqlq, atname[i], atval[i]);
    }
  i = ExecSqlDb2(henv, hdbc, sqlq);
  if (i != 0)  
    {
      sprintf(errmess,"Data Update Error: %s",sqlq);
	failure = METADATA_UPDATE_ERROR;
      return(METADATA_UPDATE_ERROR);
    }
  return(MDAS_SUCCESS);
}

int changeAttrValueSet(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount, int setCount)
{

  if (strstr(tabname,MDASSCHEMENAME) == tabname)
    sprintf(sqlq, "update %s set ", tabname);
  else
    sprintf(sqlq, "update %s%s set ", MDASSCHEMENAME, tabname);
  for (i = 0 ; i < setCount; i++) {
    if (i > 0) strcat(sqlq,",");
    sprintf(sqlq, "%s %s = %s", sqlq, atname[i],atval[i]);
  }
  for (i = setCount; i < atcount; i++)
    {
      if (i > setCount)
	  sprintf(sqlq, "%s AND %s = %s", sqlq, atname[i], atval[i]);
      else
	  sprintf(sqlq, "%s  WHERE %s = %s", sqlq, atname[i], atval[i]);
    }
  i = ExecSqlDb2(henv, hdbc, sqlq);
  i  = 0;
  if (i != 0)  
    {
      sprintf(errmess,"Data Update Error: %s",sqlq);
	failure = METADATA_UPDATE_ERROR;
      return(METADATA_UPDATE_ERROR);
    }
  return(MDAS_SUCCESS);
}

int appendAttrValue(char atname[][MAX_TOKEN],char atval[][MAX_DATA_SIZE], char *tabname, int atcount)
{
  if (strstr(tabname,MDASSCHEMENAME) == tabname)
    sprintf(sqlq, "update %s set %s = %s || %s ", tabname,
	        atname[0], atname[0], atval[0]);
  else
    sprintf(sqlq, "update %s%s set %s = %s || %s ",
	         MDASSCHEMENAME, tabname,
	        atname[0], atname[0], atval[0]);
  for (i = 1; i < atcount; i++)
    {
      if (i > 1)
	{
	  sprintf(sqlq, "%s AND %s = %s", sqlq, atname[i], atval[i]);
	}
      else
	{
	  sprintf(sqlq, "%s  WHERE %s = %s", sqlq, atname[i], atval[i]);
	}
	  
    }
  i = ExecSqlDb2(henv, hdbc, sqlq);
  if (i != 0)  
    {
      sprintf(errmess,"Data Update Error: %s",sqlq);
	failure = METADATA_UPDATE_ERROR;
      return(METADATA_UPDATE_ERROR);
    }
  return(MDAS_SUCCESS);
}



int initMdasEnviron()
{

     i = setConfigInfo();
     if ( i != 0 )  return (i);
     strcpy(catser,MDASDBNAME);
     strcpy(cattype,MDASDBTYPE);
     strcpy(dbhome,DBHOME);
     strcpy(user,DB2USER);
     strcpy(pass,DB2PASSWORD);
     strcpy(instance,DB2INSTANCE);
     strcpy(exec_type,MDASCATEXECTYPE);
     strcpy(rsrc_id,MDASCATRSRCIC);
     strcpy(server,MDASDBNAME);
     strcpy(uid,DB2USER);
     strcpy(pwd,DB2PASSWORD);
     cur_pds_num =  MAX_DCS_NUM;

     TABLED_SINGLE_DATA_COUNTER = 0;
     TABLED_EXISTS_DATA_COUNTER = 0;
     TABLED_GETDATASETINFO_COUNTER = 0;

     audit_flag = 0;
     for (i = 0; i < MAX_QUERY_LIMIT; i++)
	  {
	    queryCont[i].id = 0;
	  }
     return(MDAS_SUCCESS);

     
}



int register_dataset_info(int cat_type, char *my_obj_data_name, char *obj_user, 
			char *obj_access_name, char *obj_domain_name, 
			char *obj_type_name,   
			char *my_obj_path_name,   char *resource_name,
			char *my_obj_group_name,  srb_long_t obj_size)
{ 


  int  obj_action_id ,i;
  int udon = 0;
  char obj_group_id[MAX_TOKEN];
  char *tmpStrPtr,*tmpStrPtr2,*tmpStrPtr3;
  char* udsmd[MAX_ADREPL_STRING_METADATA]; /*userDefinedStringMetadata */
  char* udimd[MAX_ADREPL_INTEGER_METADATA];
  char tmporaryStr[MAX_TOKEN];
  char tmporaryStr2[3 * MAX_TOKEN];
  char obj_data_name[MAX_TOKEN + 20];
  char obj_group_name[MAX_TOKEN + 20];
  char obj_path_name[MAX_TOKEN + 20];

  if (my_obj_data_name == NULL || strlen(my_obj_data_name) == 0) {
    return(DATA_NAME_NOT_FOUND);
  }
    
  make_quote_to_two_quote(my_obj_data_name,obj_data_name);
  make_quote_to_two_quote(my_obj_group_name,obj_group_name);
  make_quote_to_two_quote(my_obj_path_name,obj_path_name);


  if (strstr(obj_data_name,"&UDSMD") != NULL ||
      strstr(obj_data_name,"&UDIMD") != NULL) {
    udon = 1;
    for (i = 0;  i < MAX_ADREPL_STRING_METADATA ; i++)
      udsmd[i] = NULL;
    for (i = 0;  i < MAX_ADREPL_INTEGER_METADATA ; i++)
      udimd[i] = NULL;
  }
   strcpy(reg_registrar_name,obj_user);
   strcpy(reg_obj_name, obj_data_name);
   strcpy(reg_obj_code,"MDAS_CD_DATA");
   strcpy(reg_action_name,"create dataset");
    failure = 0;

  ival[1] = 0;

   ival[4] = get_user_id_in_domain(obj_user, obj_domain_name);
     if (ival[4] < 0) { return (USER_NOT_IN_DOMN);}

  tabledIsOkay = 1;
  ival[5]  = get_itype_value("MDAS_TD_DS_ACCS.access_constraint",
			    obj_access_name);
  tabledIsOkay = 0;
  if (failure != 0) { return (ACCESS_TYPE_NOT_IN_CAT);}
  ival[12] = -1;
  ival[13] = 0;

  
  /*** augmented for checking user access to resource June 8,2000
  ival[18] = get_itype_value("MDAS_CD_RSRC.rsrc_name", resource_name); 
  if (failure != 0) { return (RESOURCE_NOT_IN_CAT);}
  ***/
  ival[18] = getResourceIdWithAccessToUserId("write", resource_name , ival[4]);
  if (ival[18] < 0)  { return (ival[18]);}
  tabledIsOkay = 1;

  sprintf(query_string,"select phy_default_path from %sMDAS_AR_PHYSICAL where phy_rsrc_id = %i",
	  MDASSCHEMENAME, ival[18]);
  get_ctype_value_from_query(cval[2],query_string);
  if (failure != 0)
    return(PHYSICAL_RESOURCE_NOT_IN_CAT);
  if (cval[2][0] == '/') {
    if (obj_path_name[0] != '/')
      return (OBJ_ERR_BAD_PATHNAME);
  }
  get_ctype_value(cval[2], "MDAS_TD_DATA_TYP.data_typ_name", obj_type_name);
  tabledIsOkay = 0;
  if (failure != 0) 
    { 
/*
      if (obj_type_name[0] == '.') 
	{ failure = 0; 
	  get_ctype_value(cval[2], "MDAS_TD_DATA_TYP.data_typ_name", 
			  "generic");}
      else 
	{return (DATA_TYPE_NOT_IN_CAT); }
*/
      failure = 0; 
      tabledIsOkay = 1;
      get_ctype_value(cval[2], "MDAS_TD_DATA_TYP.data_typ_name", 
			  "generic");
      tabledIsOkay = 0;

    }

  get_time(cval[5]);
  if (failure != 0) { return (GET_TIME_ERROR);}
  tabledIsOkay = 1;
  get_ctype_value(cval[13], "MDAS_TD_DATA_GRP.data_grp_name",obj_group_name);
  tabledIsOkay = 0;
  if (failure != 0) { return (COLLECTION_NOT_IN_CAT);} 
  reg_registrar_id = ival[4];
  reg_obj_repl_enum = 0;
  strcpy(reg_timeval,cval[5]);
   
  if ((tmpStrPtr = strchr(obj_data_name,'&')) != NULL) {
    *tmpStrPtr = '\0';
    tmpStrPtr++;
    while (strlen(tmpStrPtr) > 0) {
      if ((tmpStrPtr2 = strchr(tmpStrPtr,'&')) != NULL) {
          *tmpStrPtr2 = '\0';
	  tmpStrPtr2++;
      }
      else
	tmpStrPtr2 = tmpStrPtr + strlen(tmpStrPtr);
      if(!strcmp(tmpStrPtr,"UDSMD")) {
	tmpStrPtr +=5;
	if((tmpStrPtr3 = strchr(tmpStrPtr,'=')) != NULL) {
	  tmpStrPtr3 = '\0';
	  tmpStrPtr3++;
	  i = atoi(tmpStrPtr);
	  if  (i < 0 || i >= MAX_ADREPL_STRING_METADATA)
	    return (METADATA_FORM_ERROR);
	  udsmd[i] = tmpStrPtr3;
	}
	else 
	  return (METADATA_FORM_ERROR);
      }
      else if(!strcmp(tmpStrPtr,"UDIMD")) {
	tmpStrPtr +=5;
	if((tmpStrPtr3 = strchr(tmpStrPtr,'=')) != NULL) {
	  tmpStrPtr3 = '\0';
	  tmpStrPtr3++;
	  i = atoi(tmpStrPtr);
	  if  (i < 0 || i >= MAX_ADREPL_INTEGER_METADATA)
	    return (METADATA_FORM_ERROR);
	  udimd[i] = tmpStrPtr3;
	}
	else 
	  return (METADATA_FORM_ERROR);
      }
      else
	return (METADATA_FORM_ERROR);
      tmpStrPtr = tmpStrPtr2;
    }
  }
  res = check_unique_dname_in_group(obj_data_name,  cval[13]);
  if (res != 0)   {
    sprintf(query_string,"data registry API: name not unique in collection:%s",
	    obj_group_name);
    return (DATA_NOT_UNIQUE_IN_COLLECTION);
  }

  
  res = check_unique_dname_in_respath(obj_data_name, obj_path_name,ival[18]);
  if (res != 0)   {
    sprintf(query_string,"data registry API: name not unique in path:%s in %s",
	    obj_path_name, resource_name);
    return (DATA_NOT_UNIQUE_IN_PATH);
  }

   if(strstr(obj_type_name,"shadow object") != NULL) {
     if (obj_path_name[0] == '/')
       strcat(obj_path_name,"/?SHADOW");
     else
       strcat(obj_path_name,"?SHADOW");
#ifndef MYSRB
     res = is_system_userid(ival[4]);
#endif
     if (res != 0) return(res);
   }
   

    res = check_permit_in_group("write",  cval[13], ival[4]);
    if (res !=0)  { return (res);}
  ival[0] = 0;
  reg_obj_id =  ival[0];

#if defined(ORAMCAT) || defined(PSQMCAT)
  sprintf(query_string, 
	  "insert into %s%s (data_id,repl_enum,data_name,data_typ_id,path_name,rsrc_id,repl_timestamp,data_grp_id,data_size,is_deleted,data_owner,is_dirty,offset,container_id,modify_timestamp,last_accs_time,datalock,lockowner,version_num,container_td_id,d_comment) values (%s,0,'%s','%s','%s',%i,'%s','%s',%lld,0,%i,0,0,%s,'%s','%s',0,0,'0',0,'')",
	  MDASSCHEMENAME, 
	  "MDAS_AD_REPL", nextDataId,  obj_data_name,
	  cval[2],
	  obj_path_name, ival[18],
	  cval[5], cval[13],
	  obj_size, ival[4], currDataId, cval[5], cval[5]);
  ival[0] = DATAID_CURRVAL;
#else
  ival[0] = get_next_counter_value("DATA_ID");
  if (failure != 0) {
    return (DATA_ID_COUNTER_ERROR);
  }
  reg_obj_id =  ival[0];
  sprintf(query_string, 
	  "insert into %s%s (data_id,repl_enum,data_name,data_typ_id,path_name,rsrc_id,repl_timestamp,data_grp_id,data_size,is_deleted,data_owner,is_dirty,offset,container_id,modify_timestamp,last_accs_time,datalock,lockowner,version_num,container_td_id,d_comment) values (%i,0,'%s','%s','%s',%i,'%s','%s',%lld,0,%i,0,0,%i,'%s','%s',0,0,'0',0,'')",
	  MDASSCHEMENAME, 
	  "MDAS_AD_REPL",  ival[0], obj_data_name,
	  cval[2],
	  obj_path_name, ival[18],
	  cval[5], cval[13],
	  obj_size, ival[4],ival[0],  cval[5], cval[5]);
#endif /* ORAMCAT */


  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0) 
    {error_exit("data registry API: REPL data insertion error"); 
    return(MDAS_AD_REPL_INSERTION_ERROR);}
  sprintf(tmporaryStr2,"%s/%s",obj_group_name,obj_data_name);
  performAudit(D_CREATE,ival[4],ival[0],"",tmporaryStr2);


  if (udon) {
#ifdef FED_MCAT
    sprintf(query_string, 
        "insert into %s%s ( data_id, metadatanum,userdef_creat_date,userdef_modif_date ",
	  MDASSCHEMENAME, "MDAS_AD_MDATA");
#else
    sprintf(query_string, 
        "insert into %s%s ( data_id, metadatanum",
	  MDASSCHEMENAME, "MDAS_AD_MDATA");
#endif
    for (i = 0;  i < MAX_ADREPL_STRING_METADATA ; i++) {
      if (udsmd[i] != NULL) {
	sprintf(tmporaryStr,",userdef_metastr%i",i);
	strcat(query_string,tmporaryStr);
      }
    }
    for (i = 0;  i < MAX_ADREPL_INTEGER_METADATA ; i++) {
      if (udimd[i] != NULL) {
	sprintf(tmporaryStr,",userdef_metaint%i",i);
	strcat(query_string,tmporaryStr);
      }
    }
#if defined(ORAMCAT) || defined(PSQMCAT)
#ifdef FED_MCAT
    sprintf(query_string2,
	    ") values (currDataId, 0, '%s','%s'", cval[5], cval[5]);
#else
    sprintf(query_string2,
	    ") values (currDataId, 0");
#endif
#else
#ifdef FED_MCAT
    sprintf(query_string2,
	    ") values (%i, 0, '%s','%s'", ival[0], cval[5], cval[5]);
#else
    sprintf(query_string2,
	    ") values (%i, 0", ival[0]);
#endif
#endif /* ORAMCAT */
    for (i = 0;  i < MAX_ADREPL_STRING_METADATA ; i++) {
      if (udsmd[i] != NULL) {
	sprintf(tmporaryStr,",'%s'",udsmd[i]);
	strcat(query_string2,tmporaryStr);
      }
    }
    for (i = 0;  i < MAX_ADREPL_INTEGER_METADATA ; i++) {
      if (udimd[i] != NULL){
	sprintf(tmporaryStr,",%i",atoi(udsmd[i]));
	strcat(query_string2,tmporaryStr);
      }
    }
    strcat(query_string,query_string2);
    strcat(query_string,")");
    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0) 
      { error_exit("data registry API: meta-daata insertion error"); 
      return(MDAS_AD_MDATA_INSERTION_ERROR);}
  }


#if defined(ORAMCAT) || defined(PSQMCAT)
  sprintf(query_string, 
	  "insert into %s%s (data_id,repl_enum,user_id,access_id) values (%s,0,%i,%i)",MDASSCHEMENAME, 
	  "MDAS_AD_ACCS", currDataId, ival[4],ival[5]);
#else
  sprintf(query_string, 
	  "insert into %s%s (data_id,repl_enum,user_id,access_id) values (%i,0,%i,%i)",MDASSCHEMENAME, 
	  "MDAS_AD_ACCS", ival[0], ival[4],ival[5]);
#endif /* ORAMCAT */

  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0) 
    { error_exit("data registry API: ACCS data insertion error"); 
    return(MDAS_AD_ACCS_INSERTION_ERROR);}

  res = insertGuidBySRB(ival[0]);
  if (res != 0) 
    return(res);
  res = inheritACLWithSticky( cval[13],ival[0],0, currDataId,ival[4],"single-data-register");
  if (res != 0)
    return(res);
#if defined(ORAMCAT) || defined(PSQMCAT)
  sprintf(query_string,
          "insert into %s%s (data_id,metadatanum) values (%s,-1)",MDASSCHEMENAME,"MDAS_AD_MDATA", currDataId);
#else
  sprintf(query_string,
          "insert into %s%s (data_id,metadatanum) values (%i,-1)",MDASSCHEMENAME,"MDAS_AD_MDATA",ival[0]);
#endif /* ORAMCAT */
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)
    return(MDAS_AD_MDATA_INSERTION_ERROR);
 return(MDAS_SUCCESS);

}

int modify_extensible_metadata(int cat_type, char *data_name,
			char *collection_name,
			char *in_data_value_1,char *data_value_2,
			char *data_value_3,char *data_value_4, char *data_value_5,
			int retraction_type, char *user_name,
			char * domain_name)
{
  int i, user_id,ii,jj,kk,ll;
  char data_grp_id[MAX_TOKEN] ="";
  char *strPtr, *strPtr2, *strPtr3, *strPtr4;
  char *tmpPtr, *tmpPtr2;
  char *data_value_1;
  int data_id = 0;
  int tid = -1;
  int uid = -1;
  int cid = -1;
  int did = -1;
  int rid = -1;
  int gid = -1;
  int cnid = -1;
  int atDataId;
  char atDataName[MAX_TOKEN], atCollName[MAX_TOKEN], atCollId[MAX_TOKEN];
  /*
  if (retraction_type == RELOAD_EXT_TAB_ATTR_INDEX) {
    i = reload_ext_tab_attr_index(cat_type, in_data_value_1);
    return(i);
  }
  */
  failure = 0;
  user_id = get_user_id_in_domain(user_name, domain_name); 
  if (user_id < 0) 
    return (user_id);  
  data_value_1 = in_data_value_1;
  while (isspace(*data_value_1)) data_value_1++;
  if (strstr(data_value_1,"MDAS_") != NULL) {
    if (is_system_userid(user_id) != 0 || strcmp(data_value_1,"MDAS_TD_SCHM"))
      return(ACTION_NOT_ALLOWED);
  }
  else {
    /* checking for access to table: done by looking for access in /home/ext_tables/table_name */
    i = check_permit_in_dataname("write","/home/ext_schema/table_names",data_value_1,user_id);
    if (i !=0)
      return (i);
  }  
  if (data_name != NULL && strlen(data_name) > 0) { /* its data related metadata */
    data_id =   getDataId(collection_name,  data_name);
    if (data_id < 0) 
      return(data_id);
    i = check_permit_in_dataid("write",data_id,user_id);
    if (i !=0) 
      return (i);
  }
  else if (collection_name != NULL && strlen(collection_name) > 0) {/* its collection related metadata */
    get_ctype_value(data_grp_id, "MDAS_TD_DATA_GRP.data_grp_name", collection_name);
    if (failure != 0) 
      return (COLLECTION_NOT_IN_CAT);
    i = check_permit_in_group("write",data_grp_id,user_id);
    if (i !=0) 
      return (i);
  }
  
  
  

  
  if (data_value_3 != NULL && data_value_4 != NULL ) 
      sprintf(query_string2,"%s:%s:%s:%s",data_name,collection_name,
	      data_value_1,data_value_2,data_value_3,data_value_4);
  else if (data_value_3 != NULL)
    sprintf(query_string2,"%s:%s:%s:%s",data_name,collection_name,
	    data_value_1,data_value_2,data_value_3,"");
  else
    sprintf(query_string2,"%s:%s:%s:%s",data_name,collection_name,
	    data_value_1,data_value_2,"","");
  get_time(cval[5]);
  performAudit(retraction_type,user_id,data_id,data_grp_id,query_string2);


  switch (retraction_type)
    {
    case D_DELETE_FROM_EXTMD_TABLE:
                         /* data_1 contains table name
			    data_2 contains one or more attribute names in that table 
			    separated by | and
			    data_3 contains the comparison-values corresponding to
			    those attributes.  Separate rows/statements by new_line. 
			    Attribute names data_id,user_id,  and collection_id
			    are keywords and will be substituted. Use 0 as place-holder
			 */
                         BulkLoadFlag = 1;
                         jj = getAttrValFromBulkLoadMetadataString(data_value_2,"|", atval,0, &strPtr2);
                         BulkLoadFlag = 0;
                         if (jj < 0)
                           return(jj);
                         strPtr = data_value_3;
                         strPtr2 = NULL;
                         for (i = 0; i < jj; i++) {
			   strcpy(atname[i],atval[i]);
                           if (!strcmp(atname[i],"data_id") || !strcmp(atname[i],"DATA_ID"))
                             did = i;
                           else if (!strcmp(atname[i],"user_id")|| !strcmp(atname[i],"USER_ID"))
                             uid = i;
                           else if (!strcmp(atname[i],"collection_id") || !strcmp(atname[i],"COLLECTION_ID"))
                             cid = i;
			   else if (!strcmp(atname[i],"rsrc_id")|| !strcmp(atname[i],"RSRC_ID"))
                             rid = i;
                           else if (!strcmp(atname[i],"container_id") || !strcmp(atname[i],"CONTAINER_ID"))
                             cnid = i;
                           else if (!strcmp(atname[i],"guid") || !strcmp(atname[i],"GUID"))
                             gid = i;

			 }
			 while (strPtr != NULL) {
                         BulkLoadFlag = 1;
                           ii =   getAttrValFromBulkLoadMetadataString(strPtr,"|", atval, 0,&strPtr2);
                         BulkLoadFlag = 0;
                           if (ii == UNIX_EINVAL )
                             break;
                           else if (ii < 0)
                             return(ii);
                           if (ii != jj)
                             return(COLUMN_COUNT_MISMATCH);
			   if (did >= 0) {
			     if (strlen(atval[did]) == 0 || !strcmp(atval[did],"0"))
			       sprintf(atval[did]," = %i",data_id);
			     else {
			       tmpPtr = atval[did];
			       while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
				 tmpPtr++;
			       if (atoi(tmpPtr) <= 0) {
				 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
				 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
				   tmpPtr2--;
				 tmpPtr2++;
				 *tmpPtr2 = '\0';
				 splitbychar(tmpPtr,atCollName,atDataName,'/');
				 atDataId = getDataId(atCollName,atDataName);
				 if (atDataId < 0)
				   return(atDataId);
				 i = check_permit_in_dataid("write",atDataId,user_id);
				 if (i !=0)
				   return (i);
				 sprintf(atval[did]," = %i",atDataId);
			       }
			       else {
				 i = check_permit_in_dataid("write",atoi(tmpPtr),user_id);
				 if (i !=0)
                                   return (i);
			       }
			     }
			   }
                           else if (uid >= 0) {
			     if (strlen(atval[uid]) == 0 || !strcmp(atval[uid],"0"))
			       sprintf(atval[uid]," = %i",user_id);
			     else {
			       tmpPtr = atval[uid];
                               while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                                 tmpPtr++;
                               if (atoi(tmpPtr) <= 0) {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
				 tmpPtr2++;
                                 *tmpPtr2 = '\0';
				 splitbychar(tmpPtr,atCollName,atDataName,'@');
				 atDataId = get_user_id_in_domain(atCollName,atDataName);
				 if (atDataId < 0)
				   return(atDataId);
				 sprintf(atval[uid]," = %i",atDataId);
			       }
			     }
			   }
                           else if (cid >= 0) {
			     if (strlen(atval[cid]) == 0 || !strcmp(atval[cid],"0"))
			       sprintf(atval[cid]," = '%s'",data_grp_id);
			     else {
			       tmpPtr = atval[cid];
                               while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                                 tmpPtr++;
                               if (atoi(tmpPtr) <= 0) {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
				 tmpPtr2++;
                                 *tmpPtr2 = '\0';
				 failure = 0;
				 get_ctype_value(atCollId, "MDAS_TD_DATA_GRP.data_grp_name",tmpPtr);
				 if (failure != 0)
				   return (COLLECTION_NOT_IN_CAT);
				 atDataId = check_permit_in_group("write",atCollId,user_id);
				 if (atDataId !=0)
				   return (atDataId);
				 sprintf(atval[cid]," = '%s'",atCollId);
			       }
			       else {
				 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
                                 tmpPtr2++;
                                 *tmpPtr2 = '\0';
				 atDataId = check_permit_in_group("write",tmpPtr,user_id);
				 if (atDataId !=0)
                                   return (atDataId);
				 *tmpPtr2 = '\'';
			       }
			     }
			   }
			   else if (rid >= 0) {
			     tmpPtr = atval[rid];
			     while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
			       tmpPtr++;
			     if (atoi(tmpPtr) <= 0) {
			       tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
			       while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
				 tmpPtr2--;
			       tmpPtr2++;
			       *tmpPtr2 = '\0';
			       failure = 0;
			       atDataId = getResourceIdWithAccessToUserId("read",tmpPtr,user_id);
			       if (atDataId < 0)
				 return (atDataId);
			       sprintf(atval[rid]," = %i ",atDataId);
			     }
			   }
			   else  if (cnid >= 0) {
                             if (strlen(atval[cnid]) == 0 || !strcmp(atval[cnid],"0"))
                               sprintf(atval[cnid]," = %i",data_id);
                             else {
                               tmpPtr = atval[cnid];
                               while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                                 tmpPtr++;
                               if (atoi(tmpPtr) <= 0) {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
				 tmpPtr2++;
                                 *tmpPtr2 = '\0';
                                 splitbychar(tmpPtr,atCollName,atDataName,'/');
                                 atDataId = getDataId(atCollName,atDataName);
                                 if (atDataId < 0)
                                   return(atDataId);
				 if (isContainer(atDataId) == MDAS_ONE ) {
				   i = check_permit_in_dataid("write",atDataId,user_id);
				   if (i !=0)
				     return (i);
				   sprintf(atval[cnid]," = %i",atDataId);
				 }
				 else
				   return(CONTAINER_NOT_IN_CAT);
			       }
			       else {
				 if (isContainer(atoi(tmpPtr)) == MDAS_ONE ) {
                                   i = check_permit_in_dataid("write",atoi(tmpPtr),user_id);
                                   if (i !=0)
                                     return (i);
				 }
				 else
				   return(CONTAINER_NOT_IN_CAT);
			       }
			     }
			   }
 
                           i = deleteFromTableWithCondition(atname, atval,data_value_1,ii);
                           if (i != 0) return(METADATA_INSERTION_ERROR);
                           strPtr = strPtr2;
                           strPtr2 = NULL;
                         }
			 break; 
    case D_INSERT_INTO_EXTMD_TABLE:
                         /* data_1 contains table name
			    data_2 contains one or more attribute in that table and
			    data_3 contains the values corresponding to
			    those attributes. Separate rows by new_line. New rows are inserted.
			     Attribute names data_id,user_id,time_val, and collection_id
                            are keywords and will be substituted.  Use 0 as place-holder
			 */
                         BulkLoadFlag = 1;
                         jj = getAttrValFromBulkLoadMetadataString(data_value_2,"|", atval,0, &strPtr2);
                         BulkLoadFlag = 0;
			 if (jj < 0)
			   return(jj);
			 strPtr = data_value_3;
			 strPtr2 = NULL;
			 for (i = 0; i < jj; i++) {
			   strcpy(atname[i],atval[i]);
			   if (!strcmp(atname[i],"data_id") || !strcmp(atname[i],"DATA_ID")) 
			     did = i;
			   else if (!strcmp(atname[i],"user_id")|| !strcmp(atname[i],"USER_ID"))
			     uid = i;
			   else if (!strcmp(atname[i],"time_val")|| !strcmp(atname[i],"TIME_VAL"))
			     tid = i;
			   else if (!strcmp(atname[i],"collection_id")|| !strcmp(atname[i],"COLLECTION_ID"))
			     cid = i;
			   else if (!strcmp(atname[i],"rsrc_id")|| !strcmp(atname[i],"RSRC_ID"))
                             rid = i;
                           else if (!strcmp(atname[i],"container_id") || !strcmp(atname[i],"CONTAINER_ID"))
                             cnid = i;
                           else if (!strcmp(atname[i],"guid") || !strcmp(atname[i],"GUID"))
                             gid = i;
			 }

			 while (strPtr != NULL) {
                         BulkLoadFlag = 1;
			   ii =   getAttrValFromBulkLoadMetadataString(strPtr,"|", atval, 0,&strPtr2);
                         BulkLoadFlag = 0;
			   if (ii == UNIX_EINVAL )
			     break;
			   else if (ii < 0)
			     return(ii);
			   if (ii != jj)
			     return(COLUMN_COUNT_MISMATCH);
			   if (tid >= 0) {
			     if (strlen(atval[tid]) == 0 || !strcmp(atval[tid],"0"))
			       sprintf(atval[tid],"'%s'",cval[5]);
			   }
			   else if (did >= 0) {
			     if (strlen(atval[did]) == 0 || !strcmp(atval[did],"0"))
			       sprintf(atval[did],"%i",data_id);
			     else {
			       tmpPtr = atval[did];
                               while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                                 tmpPtr++;
                               if (atoi(tmpPtr) <= 0) {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
				 tmpPtr2++;
                                 *tmpPtr2 = '\0';
				 splitbychar(tmpPtr,atCollName,atDataName,'/');
				 atDataId = getDataId(atCollName,atDataName);
				 if (atDataId < 0)
				   return(atDataId);
				 i = check_permit_in_dataid("write",atDataId,user_id);
				 if (i !=0)
				   return (i);
				 sprintf(atval[did],"%i",atDataId);
			       }
			       else {
				 i = check_permit_in_dataid("write",atoi(tmpPtr),user_id);
				 if (i !=0)
                                   return (i);
			       }
			     }
			   }
			   else if (uid >= 0) {
			     if (strlen(atval[uid]) == 0 || !strcmp(atval[uid],"0"))
			       sprintf(atval[uid],"%i",user_id);
			     else {
			       tmpPtr = atval[uid];
			       while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                                 tmpPtr++;
                               if (atoi(tmpPtr) <= 0) {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
                                 tmpPtr2++;
                                 *tmpPtr2 = '\0';
                                 splitbychar(tmpPtr,atCollName,atDataName,'@');
				 atDataId = get_user_id_in_domain(atCollName,atDataName);
				 if (atDataId < 0)
				   return(atDataId);
				 sprintf(atval[uid],"%i",atDataId);
			       }
			     }
			   }
			   else if (cid >= 0) {
			     if (strlen(atval[cid]) == 0|| !strcmp(atval[cid],"0"))
			       sprintf(atval[cid],"'%s'",data_grp_id);
			     else {
			       tmpPtr = atval[cid];
                               while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                                 tmpPtr++;
                               if (atoi(tmpPtr) <= 0) {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
                                 tmpPtr2++;
                                 *tmpPtr2 = '\0';
				 
				 failure = 0;
				 get_ctype_value(atCollId, "MDAS_TD_DATA_GRP.data_grp_name", atval[cid]);
				 if (failure != 0)
				   return (COLLECTION_NOT_IN_CAT);
				 i = check_permit_in_group("write",atCollId,user_id);
				 if (i !=0)
				   return (i);
				 sprintf(atval[cid],"'%s'",atCollId);
			       }
			       else {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
                                 tmpPtr2++;
                                 *tmpPtr2 = '\0';
                                 atDataId = check_permit_in_group("write",tmpPtr,user_id);
                                 if (atDataId !=0)
                                   return (atDataId);
                                 *tmpPtr2 = '\'';
                               }
			     }
			   }
			   else if (rid >= 0) {
			     tmpPtr = atval[rid];
                             while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                               tmpPtr++;
                             if (atoi(tmpPtr) <= 0) {
                               tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                               while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                 tmpPtr2--;
                               *tmpPtr2 = '\0';
                               failure = 0;
                               atDataId = getResourceIdWithAccessToUserId("read",tmpPtr,user_id);
                               if (atDataId < 0)
                                 return (atDataId);
                               sprintf(atval[rid],"%i",atDataId);
                             }
			   }
                           else  if (cnid >= 0) {
                             if (strlen(atval[cnid]) == 0 || !strcmp(atval[cnid],"0"))
                               sprintf(atval[cnid],"%i",data_id);
                             else {
                               tmpPtr = atval[cnid];
                               while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                                 tmpPtr++;
                               if (atoi(tmpPtr) <= 0) {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
                                 tmpPtr2++;
                                 *tmpPtr2 = '\0';
                                 splitbychar(tmpPtr,atCollName,atDataName,'/');
                                 atDataId = getDataId(atCollName,atDataName);
                                 if (atDataId < 0)
                                   return(atDataId);
                                 if (isContainer(atDataId) == MDAS_ONE ) {
                                   i = check_permit_in_dataid("write",atDataId,user_id);
                                   if (i !=0)
                                     return (i);
                                   sprintf(atval[cnid],"%i",atDataId);
                                 }
                                 else
                                   return(CONTAINER_NOT_IN_CAT);
                               }
                               else {
                                 if (isContainer(atoi(tmpPtr)) == MDAS_ONE ) {
                                   i = check_permit_in_dataid("write",atoi(tmpPtr),user_id);
                                   if (i !=0)
                                     return (i);
                                 }
                                 else
                                   return(CONTAINER_NOT_IN_CAT);
                               }
                             }
			   }			   
			   i = insertIntoTable(atname, atval,data_value_1,ii);
			   if (i != 0) return(METADATA_INSERTION_ERROR);
			   strPtr = strPtr2;
			   strPtr2 = NULL;
			 }
			 break; 
    case D_MODIFY_IN_EXTMD_TABLE:
                         /* data_1 contains table name
			    data_2 contains one or more attribute in that table and
			    data_3 contains the values corresponding to
			    those attributes.these are new values to be inserted 
			    data_4 contains one or more attribute in that table and
			    data_5 contains the comparison-values corresponding to
			    those attributes. These are used to chosse the rows.
			    Separate rows by new_line. New rows are inserted.
			     Attribute names data_id,user_id,time_val, and collection_id
                            are keywords and will be substituted.  Use 0 as place-holder
			 */
                         BulkLoadFlag = 1;
                         jj = getAttrValFromBulkLoadMetadataString(data_value_2,"|", atval,0, &strPtr2);
                         BulkLoadFlag = 0;
			 if (jj < 0)
			   return(jj);
                         BulkLoadFlag = 1;
                         kk = getAttrValFromBulkLoadMetadataString(data_value_4,"|", atval,jj, &strPtr2);
                         BulkLoadFlag = 0;
			 if (kk < 0)
			   return(kk);
			 for (i = 0; i < kk; i++) {
			   strcpy(atname[i],atval[i]);
			   if (!strcmp(atname[i],"data_id"))
			     did = i;
			   else if (!strcmp(atname[i],"user_id"))
			     uid = i;
			   else if (!strcmp(atname[i],"time_val"))
			     tid = i;
			   else if (!strcmp(atname[i],"collection_id"))
			     cid = i;
                           else if (!strcmp(atname[i],"rsrc_id")|| !strcmp(atname[i],"RSRC_ID"))
                             rid = i;
                           else if (!strcmp(atname[i],"container_id") || !strcmp(atname[i],"CONTAINER_ID"))
                             cnid = i;
                           else if (!strcmp(atname[i],"guid") || !strcmp(atname[i],"GUID"))
                             gid = i;
			 }
			 strPtr = data_value_3;
			 strPtr2 = NULL;
			 strPtr3 = data_value_5;
			 strPtr4 = NULL;

			 while (strPtr != NULL) {
			   BulkLoadFlag = 1;
			   ii =   getAttrValFromBulkLoadMetadataString(strPtr,"|", atval, 0,&strPtr2);
			   BulkLoadFlag = 0;
			   if (ii == UNIX_EINVAL )
			     break;
			   else if (ii < 0)
			     return(ii);
			   if (ii != jj)
			     return(COLUMN_COUNT_MISMATCH);
			   BulkLoadFlag = 1;
			   ll =   getAttrValFromBulkLoadMetadataString(strPtr3,"|", atval, ii,&strPtr4);
			   BulkLoadFlag = 0;
			   if (ll == UNIX_EINVAL )
                             break;
                           else if (ll < 0)
                             return(ll);
                           if (kk != ll)
                             return(COLUMN_COUNT_MISMATCH);
			   if (tid >= 0) {
			     if (strlen(atval[tid]) == 0 || !strcmp(atval[tid],"0")) {
			       if (tid < jj)
				 sprintf(atval[tid],"'%s'",cval[5]);
			       else 
				 sprintf(atval[tid]," = '%s'",cval[5]);
			     }
			   }
			   else if (did >= 0) {
			     if (strlen(atval[did]) == 0 || !strcmp(atval[did],"0")) {
			       if (did < jj)
				 sprintf(atval[did],"%i",data_id);
			       else
				 sprintf(atval[did]," = %i",data_id);
			     }
			     else {
			       tmpPtr = atval[did];
                               while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                                 tmpPtr++;
                               if (atoi(tmpPtr) <= 0) {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
                                 tmpPtr2++;
                                 *tmpPtr2 = '\0';
                                 splitbychar(tmpPtr,atCollName,atDataName,'/');
                                 atDataId = getDataId(atCollName,atDataName);
                                 if (atDataId < 0)
                                   return(atDataId);
                                 i = check_permit_in_dataid("write",atDataId,user_id);
                                 if (i !=0)
                                   return (i);
				 if (did < jj)
				   sprintf(atval[did],"%i",atDataId);
				 else
				   sprintf(atval[did]," = %i",atDataId);
			       }
			       else {
                                 i = check_permit_in_dataid("write",atoi(tmpPtr),user_id);
                                 if (i !=0)
                                   return (i);
                               }
			     }
			   }
			   else if (uid >= 0) {
			     if (strlen(atval[uid]) == 0 || !strcmp(atval[uid],"0")) {
			       if (uid < jj)
				 sprintf(atval[uid],"%i",user_id);
			       else
				 sprintf(atval[uid]," = %i",user_id);
			     }
			     else {
			       tmpPtr = atval[uid];
                               while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                                 tmpPtr++;
                               if (atoi(tmpPtr) <= 0) {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
                                 tmpPtr2++;
                                 *tmpPtr2 = '\0';
                                 splitbychar(tmpPtr,atCollName,atDataName,'@');
                                 atDataId = get_user_id_in_domain(atCollName,atDataName);
                                 if (atDataId < 0)
                                   return(atDataId);
				 if (uid < jj)
				   sprintf(atval[uid],"%i",atDataId);
				 else
				   sprintf(atval[uid]," = %i",atDataId);
			       }
			     }
			   }
			   else if (cid >= 0) {
			     if (strlen(atval[cid]) == 0|| !strcmp(atval[cid],"0")) {
			       if (cid < jj)
				 sprintf(atval[cid],"'%s'",data_grp_id);
			       else
				 sprintf(atval[cid]," = '%s'",data_grp_id);
			     }
			     else {
			                                     tmpPtr = atval[cid];
                               while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                                 tmpPtr++;
                               if (atoi(tmpPtr) <= 0) {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
                                 tmpPtr2++;
                                 *tmpPtr2 = '\0';

                                 failure = 0;
                                 get_ctype_value(atCollId, "MDAS_TD_DATA_GRP.data_grp_name", atval[cid]);
                                 if (failure != 0)
                                   return (COLLECTION_NOT_IN_CAT);
                                 i = check_permit_in_group("write",atCollId,user_id);
                                 if (i !=0)
                                   return (i);
				 if (cid < jj)
				   sprintf(atval[cid],"'%s'",atCollId);
				 else
				   sprintf(atval[cid]," = '%s'",atCollId);
                               }
                               else {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
                                 tmpPtr2++;
                                 *tmpPtr2 = '\0';
                                 atDataId = check_permit_in_group("write",tmpPtr,user_id);
				 if (atDataId !=0)
                                   return (atDataId);
                                 *tmpPtr2 = '\'';
                               }
			     }
			   }
			   else if (rid >= 0) {
			     tmpPtr = atval[rid];
                             while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                               tmpPtr++;
                             if (atoi(tmpPtr) <= 0) {
                               tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                               while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                 tmpPtr2--;
                               *tmpPtr2 = '\0';
                               failure = 0;
                               atDataId = getResourceIdWithAccessToUserId("read",tmpPtr,user_id);
                               if (atDataId < 0)
                                 return (atDataId);
			       if (rid < jj)
				 sprintf(atval[rid],"%i",atDataId);
			       else
				 sprintf(atval[rid]," = %i",atDataId);
			     }
			   }
                           else  if (cnid >= 0) {
                             if (strlen(atval[cnid]) == 0 || !strcmp(atval[cnid],"0")) {
			       if (cnid < jj)
                                 sprintf(atval[cnid],"%i",data_id);
			       else
				 sprintf(atval[cnid]," = %i",data_id);
			     }
                             else {
                               tmpPtr = atval[cnid];
                               while ( *tmpPtr == ' ' || *tmpPtr == '=' || *tmpPtr == '\'')
                                 tmpPtr++;
                               if (atoi(tmpPtr) <= 0) {
                                 tmpPtr2 = tmpPtr + strlen(tmpPtr) -1;
                                 while ( *tmpPtr2 == ' ' || *tmpPtr2 == '\'')
                                   tmpPtr2--;
                                 tmpPtr2++;
                                 *tmpPtr2 = '\0';
                                 splitbychar(tmpPtr,atCollName,atDataName,'/');
                                 atDataId = getDataId(atCollName,atDataName);
                                 if (atDataId < 0)
                                   return(atDataId);
                                 if (isContainer(atDataId) == MDAS_ONE ) {
                                   i = check_permit_in_dataid("write",atDataId,user_id);
                                   if (i !=0)
                                     return (i);
				   if (cnid < jj)
				     sprintf(atval[cnid],"%i",atDataId);
				   else
				     sprintf(atval[cnid]," = %i",atDataId);
                                 }
                                 else
                                   return(CONTAINER_NOT_IN_CAT);
                               }
                               else {
                                 if (isContainer(atoi(tmpPtr)) == MDAS_ONE ) {
                                   i = check_permit_in_dataid("write",atoi(tmpPtr),user_id);
                                   if (i !=0)
                                     return (i);
                                 }
                                 else
                                   return(CONTAINER_NOT_IN_CAT);
                               }
                             }
			   }
			   i = changeAttrValueSetWithCondition(atname, atval,data_value_1,kk,ii);
			   if (i != 0) return(METADATA_INSERTION_ERROR);
			   strPtr = strPtr2;
			   strPtr2 = NULL;
			   strPtr3 = strPtr4;
			   strPtr4 = NULL;
			 }
			 break; 
    default:
                         return (MDAS_CATALOG_MODIFY_TYPE_ERROR);
			 break;
    }

  return(MDAS_SUCCESS);
}
int modify_dataset_info(int cat_type, char *my_obj_data_name,
			char *my_collection_name,
			char *my_obj_path_name,
			 char *resource_name, 
			 char *in_data_value_1, char *data_value_2,
			 int retraction_type, char *obj_user_name,
			char * domain_name)
{ 

  int    i, obj_data_id, obj_user_id, obj_action_id;
  int    replication_num, copy_repl_num;
  char obj_group_name[MAX_TOKEN];
  int    all_replicas;
  char datval[MAX_TOKEN], colval[MAX_TOKEN],datval2[MAX_TOKEN];
  char obj_path_name[MAX_TOKEN + 20];
  char obj_data_name[MAX_TOKEN + 20];
  char collection_name[MAX_TOKEN + 20];
  char chuser[MAX_TOKEN];
  char chdomain[MAX_TOKEN];
  char mdata_attrnum[MAX_TOKEN], mdata_num[MAX_TOKEN];
  char new_data_name[MAX_DATA_SIZE * 2];
  char data_value_1[MAX_DATA_SIZE * 2];
  int mdata_maxnum;
  char tmpsqlq[MAX_TOKEN];
  int    repllist[MAX_TOKEN];
  int colcount,jj;
  int given_repl_enum;
  char *tmpStr, *tmpPtr, *andPtr;
  int inContainerObject = 0;
  int isContainerFlag = 0;
  int newObjIntId;
  char udmdName[MAX_TOKEN];
  char srcCollPath[2 * MAX_TOKEN];
  char srcCollId[MAX_TOKEN];
  char given_version_num[MAX_TOKEN];
  char didStr[MAX_TOKEN];
  char localZoneName[MAX_TOKEN];
  char *localMcatName;
  int oldReplNum = -1;
  int newReplNum = -1;
  char oldVerNum[MAX_TOKEN];
  char newVerNum[MAX_TOKEN];
  char versionString[MAX_TOKEN];
#if MYSMCAT
  char mys_buf[MAX_TOKEN];
#endif
   failure = 0;
   all_replicas = 0;
   given_repl_enum = -1;
   strcpy(given_version_num,"'0'");

   make_quote_to_two_quote(my_obj_path_name,obj_path_name);
   make_quote_to_two_quote(my_obj_data_name,obj_data_name);
   make_quote_to_two_quote(my_collection_name,collection_name);

    if ( retraction_type == D_BULK_INSERT_UDEF_META_DATA_FOR_MANY_DATA) {
      i =  bulkInsertUdefMetadataForManyData(obj_user_name,domain_name,
					    in_data_value_1, data_value_2,"");
      return (i);
    } 
    if ((in_data_value_1 != NULL && 
     strlen(in_data_value_1)  > MAX_DATA_SIZE * 2) ||
     (data_value_2 != NULL && strlen(data_value_2) > MAX_DATA_SIZE * 2)) {
        return (NAME2LONG_ERROR);
    }
    if (data_value_2 != NULL && strlen(data_value_2) > 0)
      strcpy(new_data_name,data_value_2);
    else
      strcpy(new_data_name,obj_data_name);
    if (in_data_value_1 != NULL && strlen(in_data_value_1) > 0)
      strcpy(data_value_1,in_data_value_1);
    else
      strcpy(data_value_1,"");


   if ((tmpStr = strstr(obj_data_name,"&COPY=")) != NULL)  {
     *tmpStr ='\0';
     andPtr = NULL;
     if ((andPtr = strstr(tmpStr+1,"&")) != NULL)
       andPtr = '\0';
     given_repl_enum= atoi((char *) &tmpStr[6]);
     *tmpStr ='&';
     if (andPtr != NULL)
       *andPtr ='&';
   }
   if ((tmpStr = strstr(obj_data_name,"&DVERSION=")) != NULL) {
     *tmpStr ='\0';
     andPtr = NULL;
     if ((andPtr = strstr(tmpStr+1,"&")) != NULL)
       andPtr = '\0';
     strcpy(given_version_num,(char *) &tmpStr[10]);
     *tmpStr ='&';
     if (andPtr != NULL)
       *andPtr ='&';
   }
   if ((tmpStr = strstr(obj_data_name,"&")) != NULL) 
     *tmpStr ='\0';


   obj_data_id =   getDataId(collection_name,  obj_data_name);
   if (obj_data_id < 0) return(obj_data_id);

#ifdef COMPOUNDNOTDONE
   if (isCompoundObjectById(obj_data_id) == MDAS_ONE) {
     if (retraction_type == D_DELETE_ONE) 
       return(removeCompoundObject(NULL, obj_data_name,collection_name,
				   given_repl_enum, -1, 
				   obj_user_name, domain_name));
   }
#endif /* COMPOUNDNOTDONE */

  if (inContainerObjectById(obj_data_id) == MDAS_SUCCESS)
    inContainerObject = 1;

  if (inContainerObject == 1) {
    if (retraction_type == D_INSERT_ACCS ||
	retraction_type == D_DELETE_ACCS ||
	retraction_type == D_INSERT_AUDIT ||
	retraction_type == D_CHANGE_REPLNUM_VERNUM )
      return(ACTION_NOT_ALLOWED_ON_IN_CONTAINER_OBJECT);
  }

  if (isContainer(obj_data_id) == MDAS_ONE ) {
/* XXXXX allow D_MOVE_DATA_TO_TRASH. MW  
     if (retraction_type == D_DELETE_ONE || retraction_type == D_SU_DELETE_TRASH_ONE || retraction_type == D_MOVE_DATA_TO_TRASH)
*/
     if (retraction_type == D_DELETE_ONE || retraction_type == D_SU_DELETE_TRASH_ONE)
       return(ACTION_NOT_ALLOWED_ON_CONTAINER_OBJECT);
     isContainerFlag = 1;
  }

   strcpy(reg_registrar_name,obj_user_name);
   strcpy(reg_obj_name, obj_data_name);
   strcpy(reg_obj_code,"MDAS_CD_DATA");
   strcpy(reg_action_name,"modify dataset");
   if ((resource_name == NULL || !strcmp(resource_name, "")) 
       && given_repl_enum == -1)
     all_replicas = 1;

   if (retraction_type > SYS_ACTION_TYPE_MIN && retraction_type < SYS_ACTION_TYPE_MAX) {
       replication_num = 0;
       obj_user_id = get_user_id_in_domain(obj_user_name, domain_name);
       if (is_system_userid(obj_user_id) != 0)
	 return (SYS_USER_AUTHORIZATION_ERROR);
       obj_data_id = getDataId(collection_name,obj_data_name);
       if (obj_data_id < 0)
	 return (obj_data_id);
       if (all_replicas == 0) {
	 if (strlen(obj_path_name) > 0 && strlen(resource_name) > 0) {
	   sprintf(sqlq, "select A.repl_enum from %sMDAS_AD_REPL A, %sMDAS_AR_REPL B where A.data_id = %i and A.path_name ='%s' and A.rsrc_id = B.rsrc_id and B.rsrc_name = '%s'", MDASSCHEMENAME,MDASSCHEMENAME,  obj_data_id,obj_path_name, 
		   resource_name );
	   tabledIsOkay = 1;
	   if ((replication_num = get_itype_value_from_query(sqlq)) < -1) {
	     tabledIsOkay = 0;
	     return (MDAS_AD_REPL_ACCESS_ERROR);
	   }
	   tabledIsOkay = 0;
	 }
       }
       i = 0;
       failure= 0;
   } else if (retraction_type > CURATOR_ACTION_TYPE_MIN && retraction_type < CURATOR_ACTION_TYPE_MAX) {
     replication_num = 0;
     obj_user_id = get_user_id_in_domain(obj_user_name, domain_name);
     if (is_curator_userid(obj_user_id,collection_name ) != 0)
	 return (USER_NOT_CURATOR_ERROR);
     obj_data_id = getDataId(collection_name,obj_data_name);
       if (all_replicas == 0) {
	 if (strlen(obj_path_name) > 0 && strlen(resource_name) > 0) {
	   sprintf(sqlq, "select A.repl_enum from %sMDAS_AD_REPL A, %sMDAS_AR_REPL B where A.data_id = %i and A.path_name ='%s' and A.rsrc_id = B.rsrc_id and B.rsrc_name = '%s'", MDASSCHEMENAME,MDASSCHEMENAME,  obj_data_id,obj_path_name, 
		   resource_name );
	   tabledIsOkay = 1;
	   if ((replication_num = get_itype_value_from_query(sqlq)) < -1) {
	     tabledIsOkay = 0;
	     return (MDAS_AD_REPL_ACCESS_ERROR);
	   }
	   tabledIsOkay = 0;
	 }
       }
       i = 0;
       failure= 0;
       retraction_type = retraction_type - CURATOR_ACTION_TYPE_MIN;
   }
   else if (retraction_type == D_DELETE_ANNOTATIONS ||
       retraction_type == D_INSERT_ANNOTATIONS ||
       retraction_type == D_COPY_ANNOTATIONS_FROM_DATA_TO_DATA ||
       retraction_type == D_UPDATE_ANNOTATIONS) {
     i =  getDataIdWithCollectionAndAccess(obj_data_name, obj_path_name, 
		   resource_name,
		   obj_user_name, collection_name,  domain_name,
		   &obj_data_id, &replication_num, &obj_user_id,
		   "annotate",versionString);
   }
   else  if (retraction_type == D_CHANGE_OWNER) {
     i = 0;
     obj_user_id = get_user_id_in_domain(obj_user_name, domain_name);
     if (obj_user_id < 0)
       return (obj_user_id);
     obj_data_id = getDataIdWithOwnerId( collection_name, 
					 obj_data_name, obj_user_id);
     if (obj_data_id < 0)
       return(obj_data_id);
   }
   else if ( retraction_type == D_INSERT_LINK ) {
     if (inContainerObject == 0)
       i =getDataIdWithCollectionAndAccess(obj_data_name, obj_path_name, 
		   resource_name,
		   obj_user_name, collection_name,  domain_name,
		   &obj_data_id, &replication_num, &obj_user_id, "read",versionString);
     else
       i =getInContainerDataIdWithCollectionAndAccess(obj_data_name, 
		   obj_user_name, collection_name,  domain_name,
		   &obj_data_id, &replication_num, &obj_user_id, "read");
   }
   else {
     if (inContainerObject == 0) 
       i =get_data_id_and_version_with_collection(obj_data_name, obj_path_name, 
		   resource_name,
		   obj_user_name, collection_name,  domain_name,
		   &obj_data_id, &replication_num, &obj_user_id,versionString);
     else
       i =get_incontainer_data_id_with_collection(obj_data_name, 
		   obj_user_name, collection_name,  domain_name,
		   &obj_data_id, &replication_num, &obj_user_id);
     if (i < 0 /* && retraction_type == D_INSERT_ACCS */ ) {
       i = 0;
       obj_user_id = get_user_id_in_domain(obj_user_name, domain_name);
       if (obj_user_id < 0)
	 return (obj_user_id);
       obj_data_id = getDataIdWithOwnerId( collection_name, 
					   obj_data_name, obj_user_id);
       if (obj_data_id < 0)
	 return(obj_data_id);
     }
   }

   if (i < 0 )   {  
     return(i);
   }
   get_time(timeval);
   failure = 0;
   if (all_replicas)  replication_num = 0;
   if (given_repl_enum >= 0) {
     replication_num =given_repl_enum;
     all_replicas = 0;
   }
   reg_registrar_id = obj_user_id;
   reg_obj_id =  obj_data_id;
   reg_obj_repl_enum = replication_num;
   strcpy(reg_timeval,timeval);
   /* RAJA ADDED JAN 27 TO USE VERSION NUMBER  - RAJA: commented out until further check JAN 27
        justfor safety no reason.....
       if (!strcmp(given_version_num,"'0'")  && all_replicas == 0)
	 sprintf(given_version_num,"'%s'",versionString);
   / *  RAJA ADDED JAN 27 TO USE VERSION NUMBER */

   sprintf(didStr,"%i",obj_data_id);
   if (data_value_1 != NULL && data_value_2 != NULL)
      sprintf(query_string2,"%s:%s:%100.100s:%1.100s",obj_data_name,collection_name,
	   data_value_1,data_value_2);
   else if (data_value_1 != NULL)
      sprintf(query_string2,"%s:%s:%100.100s:%s",obj_data_name,collection_name,
           data_value_1,"");
   else
      sprintf(query_string2,"%s:%s:%s:%s",obj_data_name,collection_name,
           "","");
   performAudit(retraction_type,obj_user_id,obj_data_id,"",query_string2);

  switch (retraction_type)
    {
      /*    case D_INSERT_FILE_METADATA:
                         i = insertFileMetadata(obj_data_name,collection_name,
						 data_value_1);
			 if (i != 0)  {
			   elog(NOTICE,"Failure in extractTemplateMetadata for %s/%s using %s\n",collection_name,obj_data_name, data_value_1);
			   return(i);
			 }

			 break; 
      ***/
    case D_BULK_INSERT_UDEF_META_DATA_FOR_DATA:
                        i =  bulkInsertUdefMetadataForData(obj_data_id,
				       data_value_1, data_value_2,"");
			if (i != 0) return (i);
			break; 
    case D_SU_DELETE_TRASH_ONE:
                        if (strstr(collection_name,DEF_TRASH_COLL) != collection_name) {
			  failure = 0;
			  sprintf(sqlq, "select zone_id from %s%s where local_zone_flag = 1",
				  MDASSCHEMENAME, "MDAS_TD_ZONE");
			  tabledIsOkay = 1;
			  get_ctype_value_from_query(localZoneName,sqlq);
			  tabledIsOkay = 0;
			  if (failure != 0) 
			    return(ACTION_NOT_ALLOWED);
			  sprintf(sqlq, "/%s%s",localZoneName,DEF_TRASH_COLL);
			  if (strstr(collection_name,sqlq) !=collection_name)
			    return(ACTION_NOT_ALLOWED);
			}
			failure = 0;
    case D_DELETE_ONE:
                         strcpy(reg_action_name,"drop dataset");
			 /** added to provide deletion timestamp date **/
			 sprintf(atname[0],"modify_timestamp");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"'%s'",timeval);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
			   j++;
			 }
                         i = changeAttrValue(atname, atval,"MDAS_AD_REPL", j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);
			 /** added to provide deletion timestamp date **/
			 sprintf(atname[0],"data_id");
                         sprintf(atname[1],"repl_enum");
                         sprintf(atval[0],"%i",obj_data_id);
                         sprintf(atval[1],"%i",replication_num);
			 if (all_replicas) j = 1; else j = 2;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
                           j++;
			 }
			 if (all_replicas){
			   i = deleteFromTable(atname, atval,"MDAS_AD_ACCS",j);
			   failure = 0;
			   i =deleteFromTable(atname,atval,"MDAS_AD_AUDIT",1);
			   i=deleteFromTable(atname,atval,"MDAS_AD_ANNOTATION",1);
			   i =deleteFromTable(atname,atval,"MDAS_AD_MDATA",1);
			   i=deleteFromTable(atname,atval,"MDAS_AU_TCKT_DATA",1);
#ifndef LEANMCAT
#ifdef FED_MCAT
			   i=deleteFromTable(atname, atval,"MDAS_AD_GUID",1);
#endif /* FED_MCAT */
			   i=deleteFromTable(atname,atval,"MDAS_AD_CLASS",1);
			   sprintf(atname[0],"INDEXED_DATA_ID");
			   i=deleteFromTable(atname,atval,"MDAS_AD_DATA_INDEX",1);
			   sprintf(atname[0],"DATA_ID_OF_INDEX");
			   i=deleteFromTable(atname,atval,"MDAS_AD_DATA_INDEX",1);
			   sprintf(atname[0]," METHFOR_DATA_ID");
			   i=deleteFromTable(atname,atval,"MDAS_AD_DATA_METH",1);
			   sprintf(atname[0]," DATA_ID_OF_METH");	 
			   i=deleteFromTable(atname,atval,"MDAS_AD_DATA_METH",1);
			   sprintf(atname[0],"DATA_ID_OF_INDEX");
			   i=deleteFromTable(atname,atval,"MDAS_AD_COLL_INDEX",1);
			   sprintf(atname[0],"DATA_ID_OF_METH");
			   i=deleteFromTable(atname,atval,"MDAS_AD_COLL_METH",1);
#endif /* LEANMCAT */
			   /* RAJA ADDED DEC 3,2003 for removing links to this object */
			   sprintf(atname[0],"container_id");
			   i = deleteFromTable(atname, atval,"MDAS_AD_REPL",1);
			   /* RAJA ADDED DEC 3,2003 for removing links to this object */
			   sprintf(atname[0],"data_id");
			 }
                         i = deleteFromTable(atname, atval,"MDAS_AD_REPL",j);
			 if (i != 0) 
			   return (MDAS_AD_REPL_DELETION_ERROR);
			 if (all_replicas == 0 && 
			     (!strcmp(given_version_num,"'0'")||
			      !strcmp(given_version_num,"0") )) {
#if MYSMCAT
			   sprintf(sqlq, "select max(version_num) from %sMDAS_AD_REPL where data_id = %i and repl_enum = %i", MDASSCHEMENAME, obj_data_id, replication_num);
			   i = get_ctype_value_from_query(mys_buf, sqlq);
			   if(i == 0){
			   	sprintf(sqlq, "update %sMDAS_AD_REPL set version_num = '0' where data_id = %i and repl_enum = %i and version_num in ('%s')", MDASSCHEMENAME, obj_data_id, replication_num, mys_buf);
				i = ExecSqlDb2(henv, hdbc, sqlq);

			   }
#else
			   sprintf(sqlq, "update %sMDAS_AD_REPL set version_num = '0' where data_id = %i and repl_enum = %i and version_num in (select max(version_num) from %sMDAS_AD_REPL where data_id = %i and repl_enum = %i)",MDASSCHEMENAME,obj_data_id, replication_num,MDASSCHEMENAME,obj_data_id,replication_num);
			   i = ExecSqlDb2(henv, hdbc, sqlq);
#endif
			 }

			 if (!all_replicas) {
			   sprintf(sqlq, "select t1.data_id from %sMDAS_AD_REPL t1    where t1.data_id = %i", MDASSCHEMENAME, obj_data_id);
			  if (check_exists(sqlq) != 0) {
			   i = deleteFromTable(atname, atval,"MDAS_AD_ACCS",1);
			   failure = 0;
#ifndef LEANMCAT
#ifdef FED_MCAT
			   i=deleteFromTable(atname, atval,"MDAS_AD_GUID",1);
#endif /* FED_MCAT */
			   i=deleteFromTable(atname,atval,"MDAS_AD_ANNOTATION",1);
			   i =deleteFromTable(atname,atval,"MDAS_AD_MDATA",1);
			   i=deleteFromTable(atname,atval,"MDAS_AU_TCKT_DATA",1);
			   i=deleteFromTable(atname,atval,"MDAS_AD_CLASS",1);
			   sprintf(atname[0],"INDEXED_DATA_ID");
			   i=deleteFromTable(atname,atval,"MDAS_AD_DATA_INDEX",1);
			   sprintf(atname[0],"DATA_ID_OF_INDEX");
			   i=deleteFromTable(atname,atval,"MDAS_AD_DATA_INDEX",1);
			   sprintf(atname[0]," METHFOR_DATA_ID");
			   i=deleteFromTable(atname,atval,"MDAS_AD_DATA_METH",1);
			   sprintf(atname[0]," DATA_ID_OF_METH");	 
			   i=deleteFromTable(atname,atval,"MDAS_AD_DATA_METH",1);
			   sprintf(atname[0],"DATA_ID_OF_INDEX");
			   i=deleteFromTable(atname,atval,"MDAS_AD_COLL_INDEX",1);
			   sprintf(atname[0],"DATA_ID_OF_METH");
			   i=deleteFromTable(atname,atval,"MDAS_AD_COLL_METH",1);
			   sprintf(atname[0],"data_id");
#endif /* LEANMCAT */
			   /* RAJA ADDED DEC 3,2003 for removing links to this object */
			   sprintf(atname[0],"container_id");
			   i = deleteFromTable(atname, atval,"MDAS_AD_REPL",1);
			   /* RAJA ADDED DEC 3,2003 for removing links to this object */
			  }
			 }
			 /*
			 sprintf(atname[0],"data_name");
                         sprintf(atname[1],"data_id");
			 sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"'%s$deleted'",obj_data_name);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
			 i = changeAttrValue(atname, atval,"MDAS_AD_REPL",j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);
			 sprintf(atname[0],"is_deleted");
			 sprintf(atval[0],"1");
			 i = changeAttrValue(atname, atval,"MDAS_AD_REPL",j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);
			 sprintf(atname[0],"path_name");
			 sprintf(atval[0],"'%s$deleted'",obj_path_name);
			 i = changeAttrValue(atname, atval,"MDAS_AD_REPL",j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);
			 removeTabledQueries(obj_data_name);
			 removeTabledQueries(didStr);

			 */
                         break;

    case D_DELETE_ALL_FROM_CONTAINER:
                         strcpy(reg_action_name,"drop dataset");
			 if (isContainerFlag != 1) 
			   return(ACTION_NOT_ALLOWED);
			 sprintf(tmpsqlq," where data_id <> %i and data_id in (select t2.data_id from MDAS_AD_REPL t2 where t2.container_id = %i )",
				 obj_data_id, obj_data_id);
			 /*****
			 sprintf(sqlq,"delete from %s%s %s",
				 MDASSCHEMENAME,"MDAS_AD_COMMENTS",tmpsqlq);
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0)  {
			   elog(NOTICE,"Failure to delete using %s\n",sqlq);
			   return(MDAS_AD_COMMENTS_DELETION_ERROR);
			 }
			 
				 deprecated Jan 31, 2003 */
			 sprintf(sqlq,"delete from %s%s %s",
				 MDASSCHEMENAME,"MDAS_AD_AUDIT",tmpsqlq);
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0)  {
			   elog(NOTICE,"Failure to delete using %s\n",sqlq);
			   return(MDAS_AD_AUDIT_DELETION_ERROR);
			 }
			 sprintf(sqlq,"delete from %s%s %s",
				 MDASSCHEMENAME,"MDAS_AD_ANNOTATION",tmpsqlq);
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0)  {
			   elog(NOTICE,"Failure to delete using %s\n",sqlq);
			   return(MDAS_AD_ANNOTATION_DELETION_ERROR);
			 }
			 sprintf(sqlq,"delete from %s%s %s",
				 MDASSCHEMENAME,"MDAS_AD_MDATA",tmpsqlq);
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0)  {
			   elog(NOTICE,"Failure to delete using %s\n",sqlq);
			   return(MDAS_AD_MDATA_DELETION_ERROR);
			 }
			 sprintf(sqlq,"delete from %s%s %s",
				 MDASSCHEMENAME,"MDAS_AU_TCKT_DATA",tmpsqlq);
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0)  {
			   elog(NOTICE,"Failure to delete using %s\n",sqlq);
			   return(MDAS_AU_TCKT_DATA_DELETION_ERROR);
			 }
#ifdef FED_MCAT
			 sprintf(sqlq,"delete from %s%s %s",
				 MDASSCHEMENAME,"MDAS_AD_GUID",tmpsqlq);
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0)  {
			   elog(NOTICE,"Failure to delete using %s\n",sqlq);
			   return(MDAS_AD_GUID_DELETION_ERROR);
			 }
#endif /* FED_MCAT */
			 /** added to provide deletion timestamp date **/
			 
			 sprintf(sqlq,"update %s%s set modify_timestamp = '%s' where container_id = %i and data_id <> %i",
				 MDASSCHEMENAME,"MDAS_AD_REPL",timeval, 
				 obj_data_id, obj_data_id);
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0)  {
			   elog(NOTICE,"Failure to update using %s\n",sqlq);
			   return(MDAS_AD_REPL_UPDATE_ERROR);
			 }
			 /** added to provide deletion timestamp date **/
			 sprintf(sqlq,"delete from %s%s where container_id=%i and data_id <> %i",
				 MDASSCHEMENAME,"MDAS_AD_REPL", 
				 obj_data_id, obj_data_id);
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0)  {
			   elog(NOTICE,"Failure to delete using %s\n",sqlq);
			   return(MDAS_AD_REPL_DELETION_ERROR);
			 }
                         break;
    case D_CHANGE_OFFSET:                
                         strcpy(reg_action_name,"change offset");
			 if (strtoll(data_value_1,0,0) < 0) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(atname[0],"offset");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"%s",data_value_1);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
                           j++;
			 }
                         i = changeAttrValue(atname, atval,"MDAS_AD_REPL", j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break;

    case D_CHANGE_DIRTY:                
                         strcpy(reg_action_name,"change dirty");
			 if (atoi(data_value_1) < 0) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(atname[0],"is_dirty");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"%i",atoi(data_value_1));
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
                           j++;
			 }
                         i = changeAttrValue(atname, atval,"MDAS_AD_REPL", j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break;

    case D_CHANGE_REPLNUM_VERNUM:             
			 oldReplNum = -1;
			 newReplNum = -1;
			 strcpy(oldVerNum,"-1");
			 strcpy(newVerNum,"-1");
 elog(NOTICE,"d1=%s,d2=%s.\n",data_value_1,data_value_2);
                         if ((tmpPtr = strstr(data_value_1,":")) != NULL) {

			   *tmpPtr = '\0';
			   oldReplNum =  atoi(data_value_1);
			   newReplNum =  atoi(tmpPtr+1);
			 }
                         if ((tmpPtr = strstr(data_value_2,":")) != NULL) {
			   *tmpPtr = '\0';
			   strcpy(oldVerNum, (char *)data_value_2);
			   strcpy(newVerNum, (char *)tmpPtr+1);
			 }			 
                         strcpy(reg_action_name,"change replenum");
			 if (!all_replicas && newReplNum == -1) {
			   intval = get_max_repl_num(obj_data_id);
			   if (intval < 0) 
			     return (intval);
			 }
			 else 
			   intval = newReplNum;
			 if (!strcmp(newVerNum,"-1")) {
			   if (all_replicas)
			     intval2 = get_max_version_num(newVerNum,obj_data_id,-1);
			   else
			     intval2 = get_max_version_num(newVerNum,obj_data_id,intval);
			   if (intval2 < 0) 
			     return (intval2);
			 }
elog(NOTICE,"v=%s,r=%i,or=%i,ov=%s.\n",newVerNum,intval,oldReplNum,oldVerNum);
			 sprintf(atname[0],"version_num");
			 sprintf(atname[1],"repl_enum");
			 sprintf(atname[2],"data_id");
			 sprintf(atname[3],"repl_enum");
			 sprintf(atname[4],"version_num");
			 sprintf(atval[0],"'%s'",newVerNum);
			 sprintf(atval[1],"%i",intval);
			 sprintf(atval[2],"%i",obj_data_id);
			 sprintf(atval[3],"%i",oldReplNum);
			 sprintf(atval[4],"'%s'",oldVerNum);
			 if (!strcmp(oldVerNum,"-1")) 
			   i = changeAttrValueSet(atname, atval,
						"MDAS_AD_REPL", 4, 2);
			 else
			   i = changeAttrValueSet(atname, atval,
						"MDAS_AD_REPL", 5,2);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break;

    case D_INSERT_LOCK:                
                         strcpy(reg_action_name,"change lock");
			 intval = getAccessId(data_value_1);
			 if (intval < 0) 
			   return (intval);
			 if (all_replicas)
			   sprintf(sqlq,"select lockowner from %sMDAS_AD_REPL where data_id = %i", 
				   MDASSCHEMENAME, obj_data_id);
			 else
			   sprintf(sqlq,"select lockowner from %sMDAS_AD_REPL where data_id = %i and repl_enum = %i", 
				   MDASSCHEMENAME,obj_data_id,replication_num);
			 tabledIsOkay = 1;
			 if ((i = get_itype_value_from_query(sqlq)) < 0) {
			   tabledIsOkay = 0;
			   return (MDAS_AD_REPL_ACCESS_ERROR);
			 }
			 tabledIsOkay = 0;
			 if (i  > 0 && i != obj_user_id)
			   return(LOCK_OWNERSHIP_ERROR);
			 sprintf(atname[0],"datalock");
			 sprintf(atname[1],"lockowner");
			 sprintf(atname[2],"dlockexpiry");
                         sprintf(atname[3],"data_id");
                         sprintf(atname[4],"repl_enum");
			 sprintf(atval[0],"%i",intval);
			 sprintf(atval[1],"%i",obj_user_id);
			 sprintf(atval[2],"'%s'",data_value_2);
			 sprintf(atval[3],"%i",obj_data_id);
			 sprintf(atval[4],"%i",replication_num);
			 if (all_replicas) j = 4; else j = 5;
			 i = changeAttrValueSet(atname, atval,"MDAS_AD_REPL", 
						j, 3);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break;
#ifdef FED_MCAT
    case D_DELETE_GUID:
			   sprintf(atname[0],"data_id");
			   sprintf(atval[0],"%i",obj_data_id);
			   i = deleteFromTable(atname, atval,"MDAS_AD_GUID",1);
			   if (i != 0) return (MDAS_AD_GUID_DELETION_ERROR);
			   break;
    case D_INSERT_GUID:
                         sprintf(sqlq,"select guid_flag from %sMDAS_AD_GUID where data_id = %i ", 
				 MDASSCHEMENAME,obj_data_id);
			 tabledIsOkay = 1;
			 if ((i = get_itype_value_from_query(sqlq)) >=  0) {
			   /* guid_flag == 0 means it is made by SRB and hence can be
			      overwritten */
			   /* guid_flag >= 1 means it is made by external generatort
			      and hence cannot be overwritten only if data_value_2 is
			      greater than the i*/
			   tabledIsOkay = 0;
			   if (data_value_2  != NULL && strlen(data_value_2) > 0)
			     j =  atoi(data_value_2);
			   else
			     j = 1;
			   if (i >= j && strcmp(data_value_1,"STORE_INTERNAL_GUID") )
			     return(OLD_EXTERNAL_GUID_EXISTS);
			   sprintf(atname[0],"guid");
			   sprintf(atname[1],"guid_flag");
			   sprintf(atname[2],"guid_time_stamp");
			   sprintf(atname[3],"data_id");
			   if (!strcmp(data_value_1,"STORE_INTERNAL_GUID")) {
			     i = getGuidBySRB(atval[1]);
			     if (i < 0)
			       return(i);
			     sprintf(atval[0],"'%s'",atval[1]);
			     sprintf(atval[1],"%i", 0);
			   }
			   else {
			     sprintf(atval[0],"'%s'",data_value_1);
			     sprintf(atval[1],"%i", j);
			   }
			   sprintf(atval[2],"'%s'",timeval);
			   sprintf(atval[3],"%i",obj_data_id);
			   i = changeAttrValueSet(atname, atval,"MDAS_AD_GUID", 4,3);
			   if (i != 0) return (MDAS_AD_GUID_UPDATE_ERROR);
			 }
			 else {
			   /* no guid exists so insert a new one */
			   sprintf(atname[0],"guid");
			   sprintf(atname[1],"guid_flag");
			   sprintf(atname[2],"guid_time_stamp");
			   sprintf(atname[3],"data_id");
			   if (!strcmp(data_value_1,"STORE_INTERNAL_GUID")) {
			     i = getGuidBySRB(atval[1]);
			     if (i < 0)
			       return(i);
			     sprintf(atval[0],"'%s'",atval[1]);
			     sprintf(atval[1],"%i", 0);
			   }
			   else {
			     sprintf(atval[0],"'%s'",data_value_1);
			     sprintf(atval[1],"%i", 1);
			   }
			   sprintf(atval[2],"'%s'",timeval);
			   sprintf(atval[3],"%i",obj_data_id);
			   i = insertIntoTable(atname, atval,"MDAS_AD_GUID", 4);
			   if (i != 0) return (MDAS_AD_GUID_INSERTION_ERROR);
			 }
			 break;
#endif /* FED_MCAT */			 
    case D_INSERT_DCHECKSUM:                
                         strcpy(reg_action_name,"change checksum");
			 if (all_replicas) j = 2; else j = 3;
			 sprintf(atname[0],"dchecksum");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"'%s'",data_value_1);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
                           j++;
			 }
                         i = changeAttrValue(atname, atval,"MDAS_AD_REPL", j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break;
    case D_INSERT_DHIDE:                
                         strcpy(reg_action_name,"change dhide");
			 if (all_replicas) j = 2; else j = 3;
			 sprintf(atname[0],"dhide");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"%i",atoi(data_value_1));
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
                         i = changeAttrValue(atname, atval,"MDAS_AD_REPL", j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break;
    case D_INSERT_AUDITFLAG:                
                         strcpy(reg_action_name,"change dhide");
			 if (all_replicas) j = 2; else j = 3;
			 sprintf(atname[0],"auditflag");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"%i",atoi(data_value_1));
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
                         i = changeAttrValue(atname, atval,"MDAS_AD_REPL", j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break;

    case D_UPDATE_PIN:
                         if (all_replicas == 0) {
			   sprintf(sqlq,"select * from %sMDAS_AD_MISC1 where  data_id = %i and repl_enum = %i ", MDASSCHEMENAME, 
				   obj_data_id,replication_num);
			 }
			 else {
			   sprintf(sqlq,"select * from %sMDAS_AD_MISC1 where  data_id = %i ", MDASSCHEMENAME, obj_data_id);
			 }
			 if (check_exists(sqlq) != 0) {
			   strcpy(reg_action_name,"insert datamisc");
			   sprintf(atname[0],"data_id");
			   sprintf(atname[1],"dpinowner");
			   sprintf(atname[2],"dpin");
			   sprintf(atname[3],"dpinexpiry");
			   sprintf(atname[4],"repl_enum");
			   
			   sprintf(atval[0],"%i",obj_data_id);
			   sprintf(atval[1],"%i",obj_user_id);
			   sprintf(atval[2],"%i",atoi(data_value_1));
			   sprintf(atval[3],"'%s'",data_value_2);
			   sprintf(atval[4],"%i",replication_num);
			   if (all_replicas) j = 4; else j = 5;
			   if (strlen(given_version_num) > 0 && 
			       all_replicas == 0){
			     sprintf(atname[j],"version_num");
			     sprintf(atval[j],"%s",given_version_num);
			     j++;
			   }
			   if (all_replicas == 0) {
			     i = insertIntoTable(atname, atval,
						 "MDAS_AD_MISC1",j);
			     if (i!=0) return(MDAS_AD_MISC1_INSERTION_ERROR);
			   }	
			   else {
			     sprintf(sqlq,
				     "select repl_enum from %sMDAS_AD_REPL where data_id = %i ",
				     MDASSCHEMENAME, obj_data_id);
			     i = get_itype_list_from_query(repllist, 
							   &colcount,sqlq);
			     if (i == 0 ) {
			       sprintf(atname[j],"repl_enum");
			       for(jj = 0; jj < colcount; jj++) {
				 sprintf(atval[j],"%i", repllist[jj]);
				 i = insertIntoTable(atname, atval,
						     "MDAS_AD_MISC1", j+1);
				 if(i!=0) return(MDAS_AD_MISC1_INSERTION_ERROR);
				 replication_num = repllist[jj];
			       }
			     }
			   }
			 }
			 else {
			   strcpy(reg_action_name,"update datamisc1");
			   if (all_replicas == 0) {
			     sprintf(sqlq,"select   * from %sMDAS_AD_MISC1 where  data_id = %i and repl_enum = %i and (dpinowner =  %i or dpin = 0)", MDASSCHEMENAME, 
				     obj_data_id,replication_num,obj_user_id);
			   }
			   else {
			     sprintf(sqlq,"select   * from %sMDAS_AD_MISC1 where  data_id = %i  and (dpinowner =  %i or dpin = 0)", MDASSCHEMENAME, obj_data_id,obj_user_id);
			   }
			   if (strlen(given_version_num) > 0 && 
			       all_replicas == 0){
			     strcat(sqlq," and version_num = ");
			     strcat(sqlq,given_version_num);
			   }
			   if (check_exists(sqlq) != 0) 
			     return(DATA_PIN_OWNERSHIP_ERROR);
			   sprintf(atname[0],"dpinowner");
			   sprintf(atname[1],"dpin");
			   sprintf(atname[2],"dpinexpiry");
			   sprintf(atname[3],"data_id");
			   sprintf(atname[4],"repl_enum");
			   if (atoi(data_value_1) == 0) { /* remove pin */
			     sprintf(atval[0],"0");
			     sprintf(atval[1],"0");
			     sprintf(atval[2],"''");
			   }
			   else {
			     sprintf(atval[0],"%i",obj_user_id);
			     sprintf(atval[1],"%i",atoi(data_value_1));
			     sprintf(atval[2],"'%s'",data_value_2);
			   }
			   sprintf(atval[3],"%i",obj_data_id);
			   sprintf(atval[4],"%i",replication_num);	
			   
			   if (all_replicas == 0) 
			     i = changeAttrValueSet(atname, atval,
						  "MDAS_AD_MISC1",  5, 3);
			   else
			     i = changeAttrValueSet(atname,atval,"MDAS_AD_MISC1",4,3);
			   if (i != 0) return (MDAS_AD_MISC1_UPDATE_ERROR);
			 }
	 
			 

			 break;
    case D_UPDATE_DEXPIRE_DATE:
                         if (all_replicas == 0) {
			   sprintf(sqlq,"select * from %sMDAS_AD_MISC1 where  data_id = %i and repl_enum = %i ", MDASSCHEMENAME, 
				   obj_data_id,replication_num);
			 }
			 else {
			   sprintf(sqlq,"select * from %sMDAS_AD_MISC1 where  data_id = %i ", MDASSCHEMENAME, obj_data_id);
			 }
			 if (check_exists(sqlq) != 0) {
			   strcpy(reg_action_name,"insert datamisc");
			   sprintf(atname[0],"data_id");
			   sprintf(atname[1],"dexpire_date");
			   sprintf(atname[2],"repl_enum");
			   
			   sprintf(atval[0],"%i",obj_data_id);
			   sprintf(atval[1],"'%s'",data_value_1);
			   sprintf(atval[2],"%i",replication_num);
			   if (all_replicas == 0) {
			     i = insertIntoTable(atname, atval,
						 "MDAS_AD_MISC1",3);
			     if (i!=0) return(MDAS_AD_MISC1_INSERTION_ERROR);
			   }	
			   else {
			     sprintf(sqlq,
				     "select repl_enum from %sMDAS_AD_REPL where data_id = %i ",
				     MDASSCHEMENAME, obj_data_id);
			     i = get_itype_list_from_query(repllist, 
							   &colcount,sqlq);
			     if (i == 0 ) {
			       for(jj = 0; jj < colcount; jj++) {
				 sprintf(atval[2],"%i", repllist[jj]);
				 i = insertIntoTable(atname, atval,
						     "MDAS_AD_MISC1", 3);
				 if(i!=0) return(MDAS_AD_MISC1_INSERTION_ERROR);
				 replication_num = repllist[jj];
			       }
			     }
			   }
			 }
			 else {
			   strcpy(reg_action_name,"update datamisc1");
			   sprintf(atname[1],"data_id");
			   sprintf(atname[0],"dexpire_date");
			   sprintf(atname[2],"repl_enum");			   
			   sprintf(atval[1],"%i",obj_data_id);
			   sprintf(atval[0],"'%s'",data_value_1);
			   sprintf(atval[2],"%i",replication_num);
			   if (all_replicas == 0) 
			     i = changeAttrValue(atname, atval,
						  "MDAS_AD_MISC1",  3);
			   else
			     i = changeAttrValue(atname,atval,"MDAS_AD_MISC1",2);
			   if (i != 0) return (MDAS_AD_MISC1_UPDATE_ERROR);
			 }
			 break;
    case D_UPDATE_DEXPIRE_DATE_2:
                         if (all_replicas == 0) {
			   sprintf(sqlq,"select * from %sMDAS_AD_MISC1 where  data_id = %i and repl_enum = %i ", MDASSCHEMENAME, 
				   obj_data_id,replication_num);
			 }
			 else {
			   sprintf(sqlq,"select * from %sMDAS_AD_MISC1 where  data_id = %i ", MDASSCHEMENAME, obj_data_id);
			 }
			 if (check_exists(sqlq) != 0) {
			   strcpy(reg_action_name,"insert datamisc");
			   sprintf(atname[0],"data_id");
			   sprintf(atname[1],"dexpire_date_2");
			   sprintf(atname[2],"repl_enum");
			   
			   sprintf(atval[0],"%i",obj_data_id);
			   sprintf(atval[1],"'%s'",data_value_1);
			   sprintf(atval[2],"%i",replication_num);
			   if (all_replicas == 0) {
			     i = insertIntoTable(atname, atval,
						 "MDAS_AD_MISC1",3);
			     if (i!=0) return(MDAS_AD_MISC1_INSERTION_ERROR);
			   }	
			   else {
			     sprintf(sqlq,
				     "select repl_enum from %sMDAS_AD_REPL where data_id = %i ",
				     MDASSCHEMENAME, obj_data_id);
			     i = get_itype_list_from_query(repllist, 
							   &colcount,sqlq);
			     if (i == 0 ) {
			       for(jj = 0; jj < colcount; jj++) {
				 sprintf(atval[2],"%i", repllist[jj]);
				 i = insertIntoTable(atname, atval,
						     "MDAS_AD_MISC1", 3);
				 if(i!=0) return(MDAS_AD_MISC1_INSERTION_ERROR);
				 replication_num = repllist[jj];
			       }
			     }
			   }
			 }
			 else {
			   strcpy(reg_action_name,"update datamisc1");
			   sprintf(atname[1],"data_id");
			   sprintf(atname[0],"dexpire_date_2");
			   sprintf(atname[2],"repl_enum");			   
			   sprintf(atval[1],"%i",obj_data_id);
			   sprintf(atval[0],"'%s'",data_value_1);
			   sprintf(atval[2],"%i",replication_num);
			   if (all_replicas == 0) 
			     i = changeAttrValue(atname, atval,
						  "MDAS_AD_MISC1",  3);
			   else
			     i = changeAttrValue(atname,atval,"MDAS_AD_MISC1",2);
			   if (i != 0) return (MDAS_AD_MISC1_UPDATE_ERROR);
			 }
			 break;
    case D_UPDATE_DCOMPRESSED:
                         if (all_replicas == 0) {
			   sprintf(sqlq,"select * from %sMDAS_AD_MISC1 where  data_id = %i and repl_enum = %i ", MDASSCHEMENAME, 
				   obj_data_id,replication_num);
			 }
			 else {
			   sprintf(sqlq,"select * from %sMDAS_AD_MISC1 where  data_id = %i ", MDASSCHEMENAME, obj_data_id);
			 }
			 if (check_exists(sqlq) != 0) {
			   strcpy(reg_action_name,"insert datamisc");
			   sprintf(atname[1],"data_id");
			   sprintf(atname[0],"dcompressed");
			   sprintf(atname[2],"repl_enum");
			   
			   sprintf(atval[1],"%i",obj_data_id);
			   sprintf(atval[0],"'%s'",data_value_1);
			   sprintf(atval[2],"%i",replication_num);
			   if (all_replicas) j = 2; else j = 3;
			   if (strlen(given_version_num) > 0 && 
			       all_replicas == 0){
			     sprintf(atname[j],"version_num");
			     sprintf(atval[j],"%s",given_version_num);
			     j++;
			   }
			   if (all_replicas == 0) {
			     i = insertIntoTable(atname, atval,
						 "MDAS_AD_MISC1",j);
			     if (i!=0) return(MDAS_AD_MISC1_INSERTION_ERROR);
			   }	
			   else {
			     sprintf(sqlq,
				     "select repl_enum from %sMDAS_AD_REPL where data_id = %i ",
				     MDASSCHEMENAME, obj_data_id);
			     i = get_itype_list_from_query(repllist, 
							   &colcount,sqlq);
			     if (i == 0 ) {
			       sprintf(atname[j],"repl_enum");
			       for(jj = 0; jj < colcount; jj++) {
				 sprintf(atval[j],"%i", repllist[jj]);
				 i = insertIntoTable(atname, atval,
						     "MDAS_AD_MISC1", j+1);
				 if(i!=0) return(MDAS_AD_MISC1_INSERTION_ERROR);
				 replication_num = repllist[jj];
			       }
			     }
			   }
			 }
			 else {
			   strcpy(reg_action_name,"update datamisc1");
			   sprintf(atname[1],"data_id");
			   sprintf(atname[0],"dcompressed");
			   sprintf(atname[2],"repl_enum");	
			   sprintf(atval[1],"%i",obj_data_id);
			   sprintf(atval[0],"'%s'",data_value_1);
			   sprintf(atval[2],"%i",replication_num);
			   if (all_replicas) j = 2; else j = 3;
			   if (strlen(given_version_num) > 0 && 
			       all_replicas == 0){
			     sprintf(atname[j],"version_num");
			     sprintf(atval[j],"%s",given_version_num);
			     j++;
			   }
			   i = changeAttrValue(atname,atval,"MDAS_AD_MISC1",j);
			   if (i != 0) return (MDAS_AD_MISC1_UPDATE_ERROR);
			 }
			 break;
    case D_UPDATE_DENCRYPTED:
                         if (all_replicas == 0) {
			   sprintf(sqlq,"select * from %sMDAS_AD_MISC1 where  data_id = %i and repl_enum = %i ", MDASSCHEMENAME, 
				   obj_data_id,replication_num);
			 }
			 else {
			   sprintf(sqlq,"select * from %sMDAS_AD_MISC1 where  data_id = %i ", MDASSCHEMENAME, obj_data_id);
			 }
			 if (check_exists(sqlq) != 0) {
			   strcpy(reg_action_name,"insert datamisc");
			   sprintf(atname[0],"data_id");
			   sprintf(atname[1],"dencrypted");
			   sprintf(atname[2],"repl_enum");
			   
			   sprintf(atval[0],"%i",obj_data_id);
			   sprintf(atval[1],"'%s'",data_value_1);
			   sprintf(atval[2],"%i",replication_num);
			   if (all_replicas) j = 2; else j = 3;
			   if (strlen(given_version_num) > 0 && 
			       all_replicas == 0){
			     sprintf(atname[j],"version_num");
			     sprintf(atval[j],"%s",given_version_num);
			     j++;
			   }
			   if (all_replicas == 0) {
			     i = insertIntoTable(atname, atval,
						 "MDAS_AD_MISC1",j);
			     if (i!=0) return(MDAS_AD_MISC1_INSERTION_ERROR);
			   }	
			   else {
			     sprintf(sqlq,
				     "select repl_enum from %sMDAS_AD_REPL where data_id = %i ",
				     MDASSCHEMENAME, obj_data_id);
			     i = get_itype_list_from_query(repllist, 
							   &colcount,sqlq);
			     if (i == 0 ) {
			       sprintf(atname[j],"repl_enum");
			       for(jj = 0; jj < colcount; jj++) {
				 sprintf(atval[j],"%i", repllist[jj]);
				 i = insertIntoTable(atname, atval,
						     "MDAS_AD_MISC1", j+1);
				 if(i!=0) return(MDAS_AD_MISC1_INSERTION_ERROR);
				 replication_num = repllist[jj];
			       }
			     }
			   }
			 }
			 else {
			   strcpy(reg_action_name,"update datamisc1");
			   sprintf(atname[1],"data_id");
			   sprintf(atname[0],"dencrypted");
			   sprintf(atname[2],"repl_enum");	
			   sprintf(atval[1],"%i",obj_data_id);
			   sprintf(atval[0],"'%s'",data_value_1);
			   sprintf(atval[2],"%i",replication_num);
			   if (all_replicas) j = 2; else j = 3;
			   if (strlen(given_version_num) > 0 && 
			       all_replicas == 0){
			     sprintf(atname[j],"version_num");
			     sprintf(atval[j],"%s",given_version_num);
			     j++;
			   }
			   i = changeAttrValue(atname, atval,
						  "MDAS_AD_MISC1",  j);
			   if (i != 0) return (MDAS_AD_MISC1_UPDATE_ERROR);
			 }
			 break;
			 
    case D_CHANGE_TYPE:         
                         strcpy(reg_action_name,"change type");
			 tabledIsOkay = 1;
		         get_ctype_value(chval,
					 "MDAS_TD_DATA_TYP.data_typ_name",
					 data_value_1);
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (DATA_TYPE_NOT_IN_CAT);
			 }
			 sprintf(atname[0],"data_typ_id");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"'%s'",chval);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
			   j++;
			 }
                         i = changeAttrValue(atname, atval,"MDAS_AD_REPL", j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break;
    case D_INSERT_LINK:
                         strcpy(reg_action_name,"insert link");
			 i = makeDataLink(data_value_2,data_value_1,
					  obj_data_id,obj_user_id);
			 if (i !=0)  { return (i);}

			 break;
    case D_MOVE_DATA_TO_TRASH:
                         sprintf(new_data_name,"%s::%s",obj_data_name,timeval);
#ifdef FED_MCAT
			 /* use data_value_1 as tmp */
			 sprintf (data_value_1, "%s.%s",   
			  obj_user_name, domain_name); 
			 
			 localMcatName = (char *) getLocalMcatName();
			 if (strstr (collection_name, data_value_1) == NULL) {
			    /* not user normal home path */ 
                            if (strstr(collection_name, localMcatName)
                                != (char *) (collection_name +1)) {
			       /* move forward 3 '/' */
			       tmpPtr = forwardPath (collection_name, 3);
			       if (tmpPtr != NULL) {
				  sprintf(data_value_1, "%s/home/%s.%s/%s",
				   DEF_TRASH_COLL, obj_user_name, domain_name,
				   tmpPtr);
			       } else {
                                  sprintf(data_value_1,"%s/home/%s.%s",
                                   DEF_TRASH_COLL, obj_user_name, domain_name);
			       }
                            } else {
                               /* move forward 4 '/' */
                               tmpPtr = forwardPath (collection_name, 4);
                               if (tmpPtr != NULL) {
                                  sprintf(data_value_1, "/%s%s/home/%s.%s/%s",
                                   localMcatName, DEF_TRASH_COLL, 
				   obj_user_name, domain_name, tmpPtr);
                               } else {
                                  tmpPtr = strstr ((char *)(collection_name +1),
                                            "/");
                                  if (tmpPtr == NULL)
                                      return(ACTION_NOT_ALLOWED);
                                  sprintf(data_value_1,"/%s%s/home/%s.%s", 
				    localMcatName, DEF_TRASH_COLL,
				    obj_user_name, domain_name);
			       }
                            }
			 } else {
			    if (strstr(collection_name, localMcatName)
			     != (char *) (collection_name +1)) {
			       sprintf(data_value_1,"%s%s",
			           DEF_TRASH_COLL,collection_name);
			    } else {
			       tmpPtr = strstr ((char *) (collection_name +1),
					    "/");
			       if (tmpPtr == NULL)
			         return(ACTION_NOT_ALLOWED);
			       sprintf(data_value_1,"/%s%s%s", localMcatName,
				   DEF_TRASH_COLL,tmpPtr);
			    }
			 }
#else
			 sprintf(data_value_1,"%s%s",
			           DEF_TRASH_COLL,collection_name);
#endif
    case D_MOVE_DATA_TO_NEWCOLL:
                         splitbychar(data_value_1,colval,datval2,'/');
                         i = make_new_collection_path(cat_type, 
				  colval,datval2,
				  obj_user_name,domain_name);
			 if (i != 0)
			   return(i);
    case D_CHANGE_GROUP:
                         strcpy(reg_action_name,"change group");
			 tabledIsOkay = 1;
			 failure =  0;
			 get_ctype_value(chval,
					 "MDAS_TD_DATA_GRP.data_grp_name",
					 data_value_1);
			 if (failure != 0) {
			   return (COLLECTION_NOT_IN_CAT);
			 }
			 i = check_permit_in_group("write",chval,obj_user_id);
			 if (i !=0)  { return (i);}
			 tabledIsOkay = 0;
			 failure =  0;
			 res = check_unique_dname_in_group(
							   new_data_name, 
							   chval);
			 if (res != 0)   {
			   sprintf(query_string,"data modify API: name not unique in collection:%s",
				   data_value_1);
			   error_exit(query_string);
			   return (DATA_NOT_UNIQUE_IN_COLLECTION);
			 }
			 sprintf(atname[0],"data_grp_id");
			 sprintf(atname[1],"data_name");
                         sprintf(atname[2],"modify_timestamp");
                         sprintf(atname[3],"data_id");
			 sprintf(atval[0],"'%s'",chval);
			 sprintf(atval[1],"'%s'",new_data_name);
			 sprintf(atval[2],"'%s'",timeval);
			 sprintf(atval[3],"%i",obj_data_id);
                         i = changeAttrValueSet(atname, atval,"MDAS_AD_REPL", 4, 3);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);
			 if (isContainerFlag == 1) {
			   sprintf(atname[0],"container_name");
			   sprintf(atname[1],"container_id");
			   sprintf(atval[0],"'%s/%s'",data_value_1,new_data_name);
			   sprintf(atval[1],"%i",obj_data_id);
			   i = changeAttrValue(atname, atval,
					       "MDAS_TD_CONTAINER", 2);
			   if (i != 0) return (MDAS_TD_CONTAINER_UPDATE_ERROR);
			 }
			 i = inheritACLWithSticky( chval, obj_data_id,-1, "",obj_user_id,"multi-replica-move");
			 if (i != 0) return (i);
			 removeTabledQueries(obj_data_name);
			 removeTabledQueries(didStr);
			 removeTabledQueries(data_value_1);

			 break;

    case D_CHANGE_DNAME:
                         strcpy(reg_action_name,"change data name");
			 if (strlen(data_value_1) == 0)
			   return (DATA_NAME_NOT_FOUND);
			 tabledIsOkay = 1;
			 get_ctype_value(chval,
					 "MDAS_TD_DATA_GRP.data_grp_name",
					 collection_name);
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (COLLECTION_NOT_IN_CAT);
			 }
			 res = check_unique_dname_in_group(
							   data_value_1,
							   chval);
			     if (res != 0)   {
			       sprintf(query_string,"data modify API: name not unique in collection:%s",
				       data_value_1);
			       return (DATA_NOT_UNIQUE_IN_COLLECTION);
			     }
			 sprintf(atname[0],"data_name");
                         sprintf(atname[1],"data_id");
			 sprintf(atval[0],"'%s'",data_value_1);
			 sprintf(atval[1],"%i",obj_data_id);
                         i = changeAttrValue(atname, atval,"MDAS_AD_REPL", 2);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);
/**** removed for speed
                         i = changeAttrValue(atname, atval,"MDAS_CD_DATA", 2);
			 if (i != 0) return (MDAS_CD_DATA_UPDATE_ERROR);
****/
			 if (isContainerFlag == 1) {
			   sprintf(atname[0],"container_name");
			   sprintf(atname[1],"container_id");
			   sprintf(atval[0],"'%s/%s'",collection_name,
				   data_value_1);
			   sprintf(atval[1],"%i",obj_data_id);
			   i = changeAttrValue(atname, atval,
					       "MDAS_TD_CONTAINER", 2);
			   if (i != 0) return (MDAS_TD_CONTAINER_UPDATE_ERROR);
			 }
			 removeTabledQueries(obj_data_name);
			 removeTabledQueries(didStr);
			 removeTabledQueries(data_value_1);

			 break;



    case D_INSERT_ACCS:
    case D_SU_INSERT_ACCS:
                         strcpy(reg_action_name,"insert access");
			 splitbychar(data_value_1, chuser, chdomain,'@');
			 intval = get_user_id_in_domain(
					     chuser, chdomain);
			 /* intval = get_user_id_in_domain(
			    data_value_1, domain_name);*/
			 if (intval < 0) { return (USER_NOT_IN_DOMN);}
			 tabledIsOkay = 1;
                         intval2 = get_itype_value(
					 "MDAS_TD_DS_ACCS.access_constraint",
					 data_value_2);
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (ACCESS_TYPE_NOT_IN_CAT);
			 }
                         sprintf(atname[0],"data_id");
                         sprintf(atname[1],"user_id");
			 sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"%i",intval);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
                         i = deleteFromTable(atname, atval,"MDAS_AD_ACCS", j);
			 if (i != 0) return (MDAS_AD_ACCS_DELETION_ERROR);

                         sprintf(atname[2],"access_id");
                         sprintf(atname[3],"repl_enum");
			 sprintf(atval[2],"%i",intval2);
			 sprintf(atval[3],"%i",replication_num);
			 /*
                         i = insertIntoTable(atname, atval,"MDAS_AD_ACCS", 4);
			 if (i != 0) return (MDAS_AD_ACCS_INSERTION_ERROR);
			 copy_repl_num =replication_num;
			 if (all_replicas)
			   {
			     while (i == 0)
			       {
				 copy_repl_num++;
				 sprintf(sqlq, "select t1.data_id from %sMDAS_AD_REPL t1    where t1.data_id = %i and t1.repl_enum = %i ",
					 MDASSCHEMENAME, 
					 obj_data_id,copy_repl_num);
				 if (check_exists(sqlq) != 0)  break;
				 sprintf(atval[3],"%i", copy_repl_num);
				 i = insertIntoTableDontCare(atname, atval,
						     "MDAS_AD_ACCS", 4);
			       }
			   }
			 */
			 if (all_replicas == 0) {
			   i = insertIntoTable(atname, atval,"MDAS_AD_ACCS",4);
			   if (i != 0) return (MDAS_AD_ACCS_INSERTION_ERROR);
			 }
			 else 
			   {
			     sprintf(sqlq,
				     "select repl_enum from %sMDAS_AD_REPL where data_id = %i ",
				     MDASSCHEMENAME, obj_data_id);
			     i = get_itype_list_from_query(repllist, 
							   &colcount,sqlq);
			     if (i == 0 ) {
			       for(jj = 0; jj < colcount; jj++) {
				 sprintf(atval[3],"%i", repllist[jj]);
				 i = insertIntoTable(atname, atval,
						     "MDAS_AD_ACCS", 4);
				 if (i != 0) return (MDAS_AD_ACCS_INSERTION_ERROR);
				 replication_num = repllist[jj];
			       }
			     }
			   }

 			 break;

    case D_DELETE_ACCS:
    case D_SU_DELETE_ACCS:
                         strcpy(reg_action_name,"delete access");
			 splitbychar(data_value_1, chuser, chdomain,'@');
			 intval = get_user_id_in_domain(
					     chuser, chdomain);
			 /* intval = get_user_id_in_domain(
			    data_value_1, domain_name); */
			 if (intval < 0) { return (USER_NOT_IN_DOMN);}
                         /* intval2 = get_itype_value(
					 "MDAS_TD_DS_ACCS.access_constraint",
					 data_value_2);
			 if (failure != 0) {
			   return (ACCESS_TYPE_NOT_IN_CAT);
			 }
			 */
                         sprintf(atname[0],"data_id");
                         /*sprintf(atname[1],"access_id");*/
                         sprintf(atname[1],"user_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"%i",obj_data_id);
			 /*sprintf(atval[1],"%i",intval2);*/
			 sprintf(atval[1],"%i",intval);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
                         i = deleteFromTable(atname, atval,"MDAS_AD_ACCS", j);
			 if (i != 0) return (MDAS_AD_ACCS_DELETION_ERROR);
			 if (all_replicas) {
			   sprintf(sqlq,
				   "select repl_enum from %sMDAS_AD_REPL where data_id = %i ",
				   MDASSCHEMENAME, obj_data_id);
			   i = get_itype_list_from_query(repllist, 
							 &colcount,sqlq);
			     if (i == 0 ) {
			       replication_num = repllist[colcount-1];
			     }
			     else return (MDAS_AD_ACCS_DELETION_ERROR);
			 }
			 removeTabledQueries(obj_data_name);
			 removeTabledQueries(didStr);

			 break;

    case D_DELETE_ATTR:
                         strcpy(reg_action_name,"delete comments");

                         sprintf(atname[0],"data_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 			       
                         i=deleteFromTable(atname,atval,
					   dcs_tname[atoi(data_value_2)],1);
			 if (i != 0) return (MDAS_AD_COMMENTS_DELETION_ERROR);

			 break;
    case D_INSERT_ATTR:
			 strcpy(reg_action_name,"insert comments");
			 sprintf(sqlq,"select * from %s where data_id = %i",
				  dcs_tname[atoi(data_value_2)],
				 obj_data_id);
			 if (check_exists(sqlq) != 0) {
			   sprintf(atname[0],"data_id");
			   sprintf(atname[1],dcs_aname[atoi(data_value_2)]);
			   sprintf(atval[0],"%i",obj_data_id);
			   sprintf(atval[1],"%s",data_value_1);
			   i = insertIntoTable(atname, atval,
					     dcs_tname[atoi(data_value_2)], 2);
			   if (i != 0) {
			     sprintf(atval[1],"'%s'",data_value_1);
			     i = insertIntoTable(atname, atval,
					     dcs_tname[atoi(data_value_2)],2);
			     if (i != 0) 
			       return (MDAS_AD_COMMENTS_INSERTION_ERROR);
			   }
			 }
			 else {
			   sprintf(atname[0],dcs_aname[atoi(data_value_2)]);
			   sprintf(atname[1],"data_id");
			   sprintf(atval[0],"%s",data_value_1);
			   sprintf(atval[1],"%i",obj_data_id);
			   i = changeAttrValue(atname, atval,
					      dcs_tname[atoi(data_value_2)],2);
			   if (i != 0) {
			     sprintf(atval[0],"'%s'",data_value_1);
			     i = changeAttrValue(atname, atval,
					      dcs_tname[atoi(data_value_2)],2);
			     if (i != 0)
			       return (MDAS_AD_COMMENTS_APPEND_ERROR);
			   }
			 }

			 break;
    case D_MODIFY_ATTR:
			 strcpy(reg_action_name,"update comments");
			 sprintf(atname[0],dcs_aname[atoi(data_value_2)]);
			 sprintf(atname[1],"data_id");
			 sprintf(atval[0],"%s",data_value_1);
			 sprintf(atval[1],"%i",obj_data_id);
			 i = changeAttrValue(atname, atval,
					     dcs_tname[atoi(data_value_2)],2);
			 if (i != 0) {
			   sprintf(atval[0],"'%s'",data_value_1);
			   i = changeAttrValue(atname, atval,
					     dcs_tname[atoi(data_value_2)],2);
			   if (i != 0)
			     return (MDAS_AD_COMMENTS_APPEND_ERROR);
			 }

			 break;
    case D_DELETE_MULTI_ATTR:
                         strcpy(reg_action_name,"delete comments");
                         strcpy(mdata_attrnum,"");
			 strcpy(mdata_num,"");
                         splitbychar(data_value_2, 
				     mdata_attrnum, mdata_num,'@');
			 /*row@table*/
			 if (strlen(mdata_attrnum) == 0 || 
			     atoi(mdata_attrnum) < 0 ) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(atname[0],"metadatanum");
			 sprintf(atval[0],"%i",atoi(mdata_attrnum));
                         sprintf(atname[1],"data_id");
			 sprintf(atval[1],"%i",obj_data_id);
                         i=deleteFromTable(atname,atval,
					   dcs_tname[atoi(mdata_num)],2);
			 if (i != 0) return (MDAS_AD_COMMENTS_DELETION_ERROR);

			 break;
    case D_INSERT_MULTI_ATTR:  
                         strcpy(reg_action_name,"insert comments");
      		         sprintf(sqlq,"select * from %s where data_id = %i",
				 dcs_tname[atoi(data_value_2)],
				 obj_data_id);
			 if (check_exists(sqlq) == 0) {
			   sprintf(sqlq,"select max(metadatanum) from  %s where data_id =  %i ",  dcs_tname[atoi(data_value_2)],obj_data_id);
			   mdata_maxnum = get_itype_value_from_query(sqlq) ;
			   if (mdata_maxnum < 0) {
			     if (mdata_maxnum != -1) 
			       return (mdata_maxnum);
			     sprintf(sqlq,"delete from %s where data_id =  %i  and metadatanum = -1",
				       dcs_tname[atoi(data_value_2)],obj_data_id);
			     ExecSqlDb2(henv, hdbc, sqlq);
			   }
			   mdata_maxnum++;
			 }
			 else {
			   mdata_maxnum =  1;
			 }
			 sprintf(atname[0],"data_id");
			 sprintf(atname[1],dcs_aname[atoi(data_value_2)]);
			 sprintf(atname[2],"metadatanum");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"%s",data_value_1);
			 sprintf(atval[2],"%i",mdata_maxnum);
			 i = insertIntoTable(atname, atval,
					     dcs_tname[atoi(data_value_2)], 3);
			 if (i != 0) {
			   sprintf(atval[1],"'%s'",data_value_1);
			   i = insertIntoTable(atname, atval,
					     dcs_tname[atoi(data_value_2)],3);
			   if (i != 0) 
			     return (MDAS_AD_COMMENTS_INSERTION_ERROR);
			 }

			 return(mdata_maxnum);
			 break; 

    case D_MODIFY_MULTI_ATTR:
                         strcpy(reg_action_name,"update comments");
                         strcpy(mdata_attrnum,"");
			 strcpy(mdata_num,"");
                         splitbychar(data_value_2, 
				     mdata_attrnum, mdata_num,'@');
			 /*row@table*/
			 if (strlen(mdata_attrnum) == 0 || 
			     strlen(mdata_num) == 0 ||
			     atoi(mdata_attrnum) <= 0 || 
			     atoi(mdata_num) <= 0 ) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(atname[0],dcs_aname[atoi(mdata_num)]);
			 sprintf(atname[1],"data_id");
			 sprintf(atname[2],"metadatanum");
			 sprintf(atval[0],"%s",data_value_1);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",atoi(mdata_attrnum));
                         i = changeAttrValue(atname, atval,
					     dcs_tname[atoi(mdata_num)], 3);
			 
			 if (i != 0) {
			   sprintf(atval[0],"'%s'",data_value_1);
			   i = changeAttrValue(atname, atval,
					     dcs_tname[atoi(mdata_num)], 3);
			   if (i != 0) return (MDAS_AD_COMMENTS_UPDATE_ERROR);
			 }

			 break; 
    case D_DELETE_COMMENTS:
                         strcpy(reg_action_name,"delete comments");

	 /*** removed Aug 28,2001 since MDAS_AD_COMMENTS has been deprecated
                         sprintf(atname[0],"data_id");
                         sprintf(atname[1],"user_id");
                         sprintf(atname[2],"com_date");
                         sprintf(atname[3],"repl_enum");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"%i",obj_user_id);
			 sprintf(atval[2],"'%s'",data_value_1);
			 sprintf(atval[3],"%i",replication_num);
			 if (data_value_1 != NULL && strcmp( data_value_1,""))
			   {if (all_replicas) j = 3; else j = 4;}
			 else
			   {if (all_replicas) j = 2; 
			      else 
				{
				  sprintf(atname[2],"repl_enum");
				  sprintf(atval[2],"%i",replication_num);
				  j = 3;
				}
			   }
			 
                         i=deleteFromTable(atname,atval,"MDAS_AD_COMMENTS",j);
			 if (i != 0) return (MDAS_AD_COMMENTS_DELETION_ERROR);
	 ***/

			 sprintf(atname[0],"d_comment");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"''");
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas)  j = 2;
			 else   j = 3;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
			   j++;
			 }
			 i=changeAttrValue(atname,atval,"MDAS_AD_REPL",j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break;
    case D_INSERT_COMMENTS:
                         strcpy(reg_action_name,"insert comments");
	 /*** removed Aug 28,2001 since MDAS_AD_COMMENTS has been deprecated
                         sprintf(atname[0],"data_id");
                         sprintf(atname[1],"repl_enum");
                         sprintf(atname[2],"comments");
                         sprintf(atname[3],"com_date");
                         sprintf(atname[4],"user_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"%i",replication_num);
			 sprintf(atval[2],"'%s'",data_value_1);
			 sprintf(atval[3],"'%s'",timeval);
			 sprintf(atval[4],"%i",obj_user_id);

			 if (all_replicas == 0) {
			   i = insertIntoTable(atname, atval,"MDAS_AD_COMMENTS",5);
			   if (i != 0) return (MDAS_AD_COMMENTS_INSERTION_ERROR);
			 }			   
			 else
			   {
			     sprintf(sqlq,
				     "select repl_enum from %sMDAS_AD_REPL where data_id = %i ",
				     MDASSCHEMENAME, obj_data_id);
			     i = get_itype_list_from_query(repllist, 
							   &colcount,sqlq);
			     if (i == 0 ) {
			       for(jj = 0; jj < colcount; jj++) {
				 sprintf(atval[1],"%i", repllist[jj]);
				 i = insertIntoTable(atname, atval,
						     "MDAS_AD_COMMENTS", 5);
				 if (i != 0) return (MDAS_AD_COMMENTS_INSERTION_ERROR);
				 replication_num = repllist[jj];

			       }
			     }
			   }
	 ***/

			 sprintf(atname[0],"d_comment");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"'%s'",data_value_1);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas)  j = 2;
			 else   j = 3;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
			   j++;
			 }
			 i=changeAttrValue(atname,atval,"MDAS_AD_REPL",j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break;
    case D_UPDATE_COMMENTS:
	 /*** removed Aug 28,2001 since MDAS_AD_COMMENTS has been deprecated
                         if (all_replicas == 0) {
			   sprintf(sqlq,"select * from %sMDAS_AD_COMMENTS where  data_id = %i and repl_enum = %i ", MDASSCHEMENAME, 
				   obj_data_id,replication_num);
			 }
			 else {
			   sprintf(sqlq,"select * from %sMDAS_AD_COMMENTS where  data_id = %i ", MDASSCHEMENAME, obj_data_id);
			 }
			 if (check_exists(sqlq) != 0) {
			   strcpy(reg_action_name,"insert comments");
			   sprintf(atname[0],"data_id");
			   sprintf(atname[1],"user_id");
			   sprintf(atname[2],"comments");
			   sprintf(atname[3],"com_date");
			   sprintf(atname[4],"repl_enum");
			   
			   sprintf(atval[0],"%i",obj_data_id);
			   sprintf(atval[1],"%i",obj_user_id);
			   sprintf(atval[2],"'%s'",data_value_1);
			   sprintf(atval[3],"'%s'",timeval);
			   sprintf(atval[4],"%i",replication_num);
			   if (all_replicas == 0) {
			     i = insertIntoTable(atname, atval,
						 "MDAS_AD_COMMENTS",5);
			     if (i!=0) return(MDAS_AD_COMMENTS_INSERTION_ERROR);
			   }	
			   else {
			     sprintf(sqlq,
				     "select repl_enum from %sMDAS_AD_REPL where data_id = %i ",
				     MDASSCHEMENAME, obj_data_id);
			     i = get_itype_list_from_query(repllist, 
							   &colcount,sqlq);
			     if (i == 0 ) {
			       for(jj = 0; jj < colcount; jj++) {
				 sprintf(atval[4],"%i", repllist[jj]);
				 i = insertIntoTable(atname, atval,
						     "MDAS_AD_COMMENTS", 5);
				 if(i!=0) return(MDAS_AD_COMMENTS_INSERTION_ERROR);
				 replication_num = repllist[jj];
			       }
			     }
			   }
			 }
			 else {
			   strcpy(reg_action_name,"update comments");
			   sprintf(atname[0],"comments");
			   sprintf(atname[1],"data_id");
			   sprintf(atname[2],"repl_enum");
			   sprintf(atval[0],"'%s'",data_value_1);
			   sprintf(atval[1],"%i",obj_data_id);
			   sprintf(atval[2],"%i",replication_num);
			   
			   if (all_replicas == 0) 
			     i =changeAttrValue(atname,atval,"MDAS_AD_COMMENTS",3);
			   else
			     i =changeAttrValue(atname,atval,"MDAS_AD_COMMENTS",2);
			   if (i != 0) return (MDAS_AD_COMMENTS_UPDATE_ERROR);
			 }
	 ***/
                         strcpy(reg_action_name,"insert comments");
			 sprintf(atname[0],"d_comment");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"'%s'",data_value_1);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas)  j = 2;
			 else   j = 3;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
			   j++;
			 }
			 i=changeAttrValue(atname,atval,"MDAS_AD_REPL",j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);
			 

			 break;
    case D_APPEND_COMMENTS:
                         strcpy(reg_action_name,"insert comments");
			 sprintf(atname[0],"d_comment");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"'%s'",data_value_1);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas)  j = 2;
			 else   j = 3;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
			   j++;
			 }
			 i=appendAttrValue(atname,atval,"MDAS_AD_REPL",j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break;


    case D_INSERT_AUDIT:

			 break;
    case D_DELETE_ANNOTATIONS:
                         strcpy(reg_action_name,"delete annotations");

                         sprintf(atname[0],"data_id");
                         sprintf(atname[1],"user_id");
                         sprintf(atname[2],"anno_date");
                         sprintf(atname[3],"repl_enum");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"%i",obj_user_id);
			 sprintf(atval[2],"'%s'",data_value_1);
			 sprintf(atval[3],"%i",replication_num);
			 if (data_value_1 != NULL && strcmp( data_value_1,""))
			   {if (all_replicas) j = 3; else j = 4;}
			 else
			   {if (all_replicas) j = 2; 
			      else 
				{
				  sprintf(atname[2],"repl_enum");
				  sprintf(atval[2],"%i",replication_num);
				  j = 3;
				}
			   }
			 
                         i=deleteFromTable(atname,atval,"MDAS_AD_ANNOTATION",j);
			 if (i != 0) return(MDAS_AD_ANNOTATION_DELETION_ERROR);
			 break;
    case D_INSERT_ANNOTATIONS:
                         strcpy(reg_action_name,"insert annotations");

                         sprintf(atname[0],"data_id");
                         sprintf(atname[1],"repl_enum");
                         sprintf(atname[2],"annotations");
                         sprintf(atname[3],"anno_date");
                         sprintf(atname[4],"user_id");
			 sprintf(atname[5],"anno_position");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"%i",replication_num);
			 sprintf(atval[2],"'%s'",data_value_1);
			 sprintf(atval[3],"'%s'",timeval);
			 sprintf(atval[4],"%i",obj_user_id);
			 sprintf(atval[5],"'%s'",data_value_2);
			 if (all_replicas == 0) {
			   i = insertIntoTable(atname, atval,"MDAS_AD_ANNOTATION",6);
			   if (i != 0) return (MDAS_AD_ANNOTATION_INSERTION_ERROR);
			 }			   
			 else
			   {
			     sprintf(sqlq,
				     "select repl_enum from %sMDAS_AD_REPL where data_id = %i ",
				     MDASSCHEMENAME, obj_data_id);
			     i = get_itype_list_from_query(repllist, 
							   &colcount,sqlq);
			     if (i == 0 ) {
			       for(jj = 0; jj < colcount; jj++) {
				 sprintf(atval[1],"%i", repllist[jj]);
				 i = insertIntoTable(atname, atval,
						     "MDAS_AD_ANNOTATION", 6);
				 if (i != 0) return (MDAS_AD_ANNOTATION_INSERTION_ERROR);
				 replication_num = repllist[jj];

			       }
			     }
			   }

			 break;
    case D_UPDATE_ANNOTATIONS:
                         strcpy(reg_action_name,"update annotations");
			 /*****/
			 tmpPtr = strstr(data_value_1,"@@@");
			 if (tmpPtr != NULL) *tmpPtr='\0';
			 /*****/
                         sprintf(atname[0],"annotations");
                         sprintf(atname[1],"user_id");
                         sprintf(atname[2],"data_id");
			 sprintf(atname[3],"anno_date");
                         sprintf(atname[4],"repl_enum");
			 sprintf(atval[0],"'%s'",data_value_1);
			 sprintf(atval[1],"%i",obj_user_id);
			 sprintf(atval[2],"%i",obj_data_id);
			 sprintf(atval[3],"'%s'",data_value_2);
			 sprintf(atval[4],"%i",replication_num);
			 if (strlen(data_value_2) > 0) {
			   if (all_replicas == 0) 
			     i = changeAttrValue(atname, atval,"MDAS_AD_ANNOTATION",5);
			   else
			     i = changeAttrValue(atname, atval,"MDAS_AD_ANNOTATION",4);
			 }
			 else {
			   sprintf(atname[3],"repl_enum");
			   sprintf(atval[3],"%i",replication_num);
			   if (all_replicas == 0) 
			     i = changeAttrValue(atname, atval,"MDAS_AD_ANNOTATION",4);
			   else
			     i = changeAttrValue(atname, atval,"MDAS_AD_ANNOTATION",3);
			 }
			 if (i != 0) return (MDAS_AD_ANNOTATION_INSERTION_ERROR);

			 /****/
			 if (tmpPtr != NULL) {
			   tmpPtr +=3;
			   sprintf(atname[0],"anno_position");
			   sprintf(atval[0],"'%s'",tmpPtr);
			   if (all_replicas == 0) 
			     i = changeAttrValue(atname, atval,"MDAS_AD_ANNOTATION",5);
			   else
			     i = changeAttrValue(atname, atval,"MDAS_AD_ANNOTATION",4);
			   if (i != 0) return (MDAS_AD_ANNOTATION_INSERTION_ERROR);
			   
			 }
			 /*****/

			 break;
    case D_CHANGE_OWNER:
                         strcpy(reg_action_name,"change owner");
			 intval = get_user_id_in_domain(
					     data_value_1, data_value_2);
			 if (intval < 0) { return (USER_NOT_IN_DOMN);}

			 sprintf(atname[0],"data_owner");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"%i",intval);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 /*	 
			 if (all_replicas) j = 2; else j = 3; 
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
			   j++;
			 }
			 */
			 j =  2; /* change owner of all replicas RAJA March 16, 2004 */
                         i = changeAttrValue(atname, atval,"MDAS_AD_REPL", j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 removeTabledQueries(obj_data_name);
			 removeTabledQueries(didStr);

			 break;
    case D_CHANGE_SIZE:                
                         strcpy(reg_action_name,"change size");
			 if (strtoll(data_value_1,0,0) < 0) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(atname[0],"data_size");
			 sprintf(atname[1],"modify_timestamp");
			 sprintf(atname[2],"last_accs_time");
                         sprintf(atname[3],"data_id");
                         sprintf(atname[4],"repl_enum");
			 sprintf(atval[0],"%s",data_value_1);
			 sprintf(atval[1],"'%s'",timeval);
			 sprintf(atval[2],"'%s'",timeval);
			 sprintf(atval[3],"%i",obj_data_id);
			 sprintf(atval[4],"%i",replication_num);
			 if (all_replicas) j = 4; else j = 5;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
			   j++;
			 }
                         i = changeAttrValueSet(atname, atval,"MDAS_AD_REPL", j, 3);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break; 
    case D_CHANGE_DPATH:                
                         strcpy(reg_action_name,"change size");
			 sprintf(atname[0],"path_name");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"'%s'",data_value_1);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
			   j++;
			 }
                         i = changeAttrValue(atname, atval,"MDAS_AD_REPL", j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break; 
    case D_CHANGE_MODIFY_TIMESTAMP:
                         strcpy(reg_action_name,"change modify timestamp");
			 sprintf(atname[0],"modify_timestamp");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
                        if (strlen(data_value_1) == 0)
                          sprintf(atval[0],"'%s'",timeval);
                        else
                          sprintf(atval[0],"'%s'",data_value_1);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
			   j++;
			 }
                         i = changeAttrValue(atname, atval,"MDAS_AD_REPL", j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);


			 break;
    case D_CHANGE_LAST_ACCS_TIME:
                         strcpy(reg_action_name,"change modify timestamp");
			 sprintf(atname[0],"last_accs_time");
                         sprintf(atname[1],"data_id");
                         sprintf(atname[2],"repl_enum");
			 sprintf(atval[0],"'%s'",timeval);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",replication_num);
			 if (all_replicas) j = 2; else j = 3;
			 if (strlen(given_version_num) > 0 && 
			     all_replicas == 0){
			   sprintf(atname[j],"version_num");
			   sprintf(atval[j],"%s",given_version_num);
			   j++;
			 }
                         i = changeAttrValue(atname, atval,"MDAS_AD_REPL", j);
			 if (i != 0) return (MDAS_AD_REPL_UPDATE_ERROR);

			 break;
    case D_DELETE_CLASS:
                         strcpy(reg_action_name,"delete class");

                         sprintf(atname[0],"data_id");
                         sprintf(atname[1],"class_type");
                         sprintf(atname[2],"class_name");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"'%s'",data_value_1);
			 sprintf(atval[2],"'%s'",data_value_2);
                         i=deleteFromTable(atname,atval,"MDAS_AD_CLASS",3);
			 if (i != 0) return (MDAS_AD_CLASS_DELETION_ERROR);

			 break;
    case D_INSERT_CLASS:
                         strcpy(reg_action_name,"insert class");

                         sprintf(atname[0],"data_id");
                         sprintf(atname[1],"class_type");
                         sprintf(atname[2],"class_name");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"'%s'",data_value_1);
			 sprintf(atval[2],"'%s'",data_value_2);
                         i=insertIntoTable(atname,atval,"MDAS_AD_CLASS",3);
			 if (i != 0) return (MDAS_AD_CLASS_INSERTION_ERROR);

			 break;
    case D_DELETE_INDEX_FOR_DATA:
                         strcpy(reg_action_name,"delete index for data");
			 newObjIntId = getObjIdFromCollPathForUserIdAccs(
					 data_value_1, obj_user_id, "all");
			 if (newObjIntId < 0) return(newObjIntId);
                         sprintf(atname[0],"data_id_of_index");
                         sprintf(atname[1],"indexed_data_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"%i",newObjIntId);
                         i=deleteFromTable(atname,atval,"MDAS_AD_DATA_INDEX",2);
			 if (i != 0) return(MDAS_AD_DATA_INDEX_DELETION_ERROR);

			 break;
    case D_INSERT_INDEX_FOR_DATA:
                         strcpy(reg_action_name,"insert index for data");
			 newObjIntId = getObjIdFromCollPathForUserIdAccs(
					 data_value_1, obj_user_id, "all");
			 if (newObjIntId < 0) return(newObjIntId);
                         sprintf(atname[0],"data_id_of_index");
                         sprintf(atname[1],"indexed_data_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"%i",newObjIntId);
                         i=insertIntoTable(atname,atval,"MDAS_AD_DATA_INDEX",2);
			 if (i !=0) return(MDAS_AD_DATA_INDEX_INSERTION_ERROR);

			 break;
    case D_DELETE_INDEX_FOR_DATATYPE:
                         strcpy(reg_action_name,"delete index for data type");
			 tabledIsOkay = 1;
		         get_ctype_value(chval,"MDAS_TD_DATA_TYP.data_typ_name", data_value_1);
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (DATA_TYPE_NOT_IN_CAT);
			 }
                         sprintf(atname[0],"data_id_of_index");
                         sprintf(atname[1],"indexed_datatyp_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"'%s'",chval);
                         i=deleteFromTable(atname,atval,"MDAS_AD_DTYP_INDEX",2);
			 if (i != 0) return(MDAS_AD_DTYP_INDEX_DELETION_ERROR);

			 break;
    case D_INSERT_INDEX_FOR_DATATYPE:
                         strcpy(reg_action_name,"insert index for data type");
			 tabledIsOkay = 1;
		         get_ctype_value(chval,"MDAS_TD_DATA_TYP.data_typ_name", data_value_1);
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (DATA_TYPE_NOT_IN_CAT);
			 }
                         sprintf(atname[0],"data_id_of_index");
                         sprintf(atname[1],"indexed_datatyp_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"'%s'",chval);
                         i=insertIntoTable(atname,atval,"MDAS_AD_DTYP_INDEX",2);
			 if (i != 0) return(MDAS_AD_DTYP_INDEX_INSERTION_ERROR);
			 break;
    case D_DELETE_INDEX_FOR_COLLECTION:
                         strcpy(reg_action_name,"delete index for collection");
			 tabledIsOkay = 1;
			 get_ctype_value(chval,
					 "MDAS_TD_DATA_GRP.data_grp_name",
					 data_value_1);
			 tabledIsOkay = 0;
			 if (failure != 0) return (COLLECTION_NOT_IN_CAT);
			 i = check_permit_in_group("all%", chval,obj_user_id);
			 if (i !=0)  { return (i);}
                         sprintf(atname[0],"data_id_of_index");
                         sprintf(atname[1],"indexed_datacoll_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"'%s'",chval);
                         i=deleteFromTable(atname,atval,"MDAS_AD_COLL_INDEX",2);
			 if (i != 0) return(MDAS_AD_COLL_INDEX_DELETION_ERROR);

			 break;
    case D_INSERT_INDEX_FOR_COLLECTION:
                         strcpy(reg_action_name,"insert index for collection");
			 tabledIsOkay = 1;
			 get_ctype_value(chval,
					 "MDAS_TD_DATA_GRP.data_grp_name",
					 data_value_1);
			 tabledIsOkay = 0;
			 if (failure != 0) return (COLLECTION_NOT_IN_CAT);
			 i = check_permit_in_group("all%", chval,obj_user_id);
			 if (i !=0)  { return (i);}
                         sprintf(atname[0],"data_id_of_index");
                         sprintf(atname[1],"indexed_datacoll_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"'%s'",chval);
                         i=insertIntoTable(atname,atval,"MDAS_AD_COLL_INDEX",2);
			 if (i != 0) return(MDAS_AD_COLL_INDEX_INSERTION_ERROR);

			 break;
    case D_DELETE_METHOD_FOR_DATA:
                         strcpy(reg_action_name,"delete method for data");
			 newObjIntId = getObjIdFromCollPathForUserIdAccs(
					 data_value_1, obj_user_id, "all");
			 if (newObjIntId < 0) return(newObjIntId);
                         sprintf(atname[0],"data_id_of_meth");
                         sprintf(atname[1],"methfor_data_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"%i",newObjIntId);
                         i=deleteFromTable(atname,atval,"MDAS_AD_DATA_METH",2);
			 if (i != 0) return(MDAS_AD_DATA_METH_DELETION_ERROR);

			 break;
    case D_INSERT_METHOD_FOR_DATA:
                         strcpy(reg_action_name,"insert method for data");
			 newObjIntId = getObjIdFromCollPathForUserIdAccs(
					 data_value_1, obj_user_id, "all");
			 if (newObjIntId < 0) return(newObjIntId);
                         sprintf(atname[0],"data_id_of_meth");
                         sprintf(atname[1],"methfor_data_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"%i",newObjIntId);
                         i=insertIntoTable(atname,atval,"MDAS_AD_DATA_METH",2);
			 if (i !=0) return(MDAS_AD_DATA_METH_INSERTION_ERROR);

			 break;
    case D_DELETE_METHOD_FOR_DATATYPE:
                         strcpy(reg_action_name,"delete method for data type");
			 tabledIsOkay = 1;
		         get_ctype_value(chval,"MDAS_TD_DATA_TYP.data_typ_name", data_value_1);
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (DATA_TYPE_NOT_IN_CAT);
			 }
                         sprintf(atname[0],"data_id_of_meth");
                         sprintf(atname[1],"methfor_datatyp_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"'%s'",chval);
                         i=deleteFromTable(atname,atval,"MDAS_AD_DTYP_METH",2);
			 if (i != 0) return(MDAS_AD_DTYP_METH_DELETION_ERROR);

			 break;
    case D_INSERT_METHOD_FOR_DATATYPE:
                         strcpy(reg_action_name,"insert method for data type");
			 tabledIsOkay = 1;
		         get_ctype_value(chval,"MDAS_TD_DATA_TYP.data_typ_name", data_value_1);
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (DATA_TYPE_NOT_IN_CAT);
			 }
                         sprintf(atname[0],"data_id_of_meth");
                         sprintf(atname[1],"methfor_datatyp_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"'%s'",chval);
                         i=insertIntoTable(atname,atval,"MDAS_AD_DTYP_METH",2);
			 if (i != 0) return(MDAS_AD_DTYP_METH_INSERTION_ERROR);

			 break;
    case D_DELETE_METHOD_FOR_COLLECTION:
                         strcpy(reg_action_name,"delete method for collection");
			 tabledIsOkay = 1;
			 get_ctype_value(chval,
					 "MDAS_TD_DATA_GRP.data_grp_name",
					 data_value_1);
			 tabledIsOkay = 0;
			 if (failure != 0) return (COLLECTION_NOT_IN_CAT);
			 i = check_permit_in_group("all%", chval,obj_user_id);
			 if (i !=0)  { return (i);}
                         sprintf(atname[0],"data_id_of_meth");
                         sprintf(atname[1],"methfor_datacoll_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"'%s'",chval);
                         i=deleteFromTable(atname,atval,"MDAS_AD_COLL_METH",2);
			 if (i != 0) return(MDAS_AD_COLL_METH_DELETION_ERROR);

			 break;
    case D_INSERT_METHOD_FOR_COLLECTION:
                         strcpy(reg_action_name,"insert method for collection");
			 tabledIsOkay = 1;
			 get_ctype_value(chval,
					 "MDAS_TD_DATA_GRP.data_grp_name",
					 data_value_1);
			 tabledIsOkay = 0;
			 if (failure != 0) return (COLLECTION_NOT_IN_CAT);
			 i = check_permit_in_group("all%", chval,obj_user_id);
			 if (i !=0)  { return (i);}
                         sprintf(atname[0],"data_id_of_meth");
                         sprintf(atname[1],"methfor_datacoll_id");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"'%s'",chval);
                         i=insertIntoTable(atname,atval,"MDAS_AD_COLL_METH",2);
			 if (i != 0) return(MDAS_AD_COLL_METH_INSERTION_ERROR);

			 break;
    case D_DELETE_STRUCT_METADATA:
                         strcpy(reg_action_name,"delete structured meta data");
			 tmpStr = strstr(data_value_1,"&");
			 *tmpStr =  '\0';
			 tmpStr++;
                         sprintf(atname[0],"strblngsto_data_id");
			 sprintf(atname[1],"structure_type");
			 sprintf(atname[2],"structure_comments");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"'%s'",data_value_1);
			 sprintf(atval[2],"'%s'",tmpStr);
                         i=deleteFromTable(atname,atval,"MDAS_AD_STRCT_BLOB",3);
			 if (i != 0) return(MDAS_AD_STRCT_BLOB_DELETION_ERROR);

			 break;
    case D_INSERT_EXTERNAL_STRUCT_METADATA:
                         strcpy(reg_action_name,"insert external structured meta data");
			 newObjIntId = getObjIdFromCollPathForUserIdAccs(
					 data_value_2, obj_user_id, "all");
			 if (newObjIntId < 0) return(newObjIntId);
			 tmpStr = strstr(data_value_1,"&");
			 *tmpStr =  '\0';
			 tmpStr++;
                         sprintf(atname[0],"strblngsto_data_id");
                         sprintf(atname[1],"strt_isin_data_id");
			 sprintf(atname[2],"structure_type");
			 sprintf(atname[3],"structure_comments");
			 sprintf(atname[4],"internal_structure");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"%i",newObjIntId);
			 sprintf(atval[2],"'%s'",data_value_1);
			 sprintf(atval[3],"'%s'",tmpStr);
			 sprintf(atval[3],"'%s'",tmpStr);
			 sprintf(atval[4],"''");
                         i=insertIntoTable(atname,atval,"MDAS_AD_STRCT_BLOB",5);
			 if (i != 0) return(MDAS_AD_STRCT_BLOB_INSERTION_ERROR);
			 break;
    case D_INSERT_INTERNAL_STRUCT_METADATA:
                         strcpy(reg_action_name,"insert internal structured meta data");
			 tmpStr = strstr(data_value_1,"&");
			 *tmpStr =  '\0';
			 tmpStr++;
                         sprintf(atname[0],"strblngsto_data_id");
                         sprintf(atname[1],"strt_isin_data_id");
			 sprintf(atname[2],"structure_type");
			 sprintf(atname[3],"structure_comments");
			 sprintf(atname[4],"internal_structure");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"-1");
			 sprintf(atval[2],"'%s'",data_value_1);
			 sprintf(atval[3],"'%s'",tmpStr);
			 sprintf(atval[4],"'%s'",data_value_2);
                         i=insertIntoTable(atname,atval,"MDAS_AD_STRCT_BLOB",5);
			 if (i != 0) return(MDAS_AD_STRCT_BLOB_INSERTION_ERROR);

			 break;


    case D_CHANGE_USER_DEFINED_STRING_META_DATA:  
                         strcpy(mdata_attrnum,"");
			 strcpy(mdata_num,"");
                         splitbychar(data_value_1, mdata_attrnum, mdata_num,'@');
                         strcpy(reg_action_name,"change user defined string meta data");			 
			 if (strlen(mdata_attrnum) == 0 || 
			     strlen(mdata_num) == 0 ||
			     atoi(mdata_attrnum) < 0 || 
			     atoi(mdata_num) < 0 ) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(udmdName,"userdef_metastr%i",
				 atoi(mdata_attrnum));
#ifdef FED_MCAT
			 sprintf(atname[0],"%s",udmdName);
			 sprintf(atname[1],"userdef_modif_date");
                         sprintf(atname[2],"data_id");
			 sprintf(atname[3],"metadatanum");
			 sprintf(atval[0],"'%s'",data_value_2);
			 sprintf(atval[1],"'%s'",timeval);
			 sprintf(atval[2],"%i",obj_data_id);
			 sprintf(atval[3],"%i",atoi(mdata_num));
                         i = changeAttrValueSet(atname, atval,"MDAS_AD_MDATA", 4,2);
#else
			 sprintf(atname[0],"%s",udmdName);
                         sprintf(atname[1],"data_id");
			 sprintf(atname[2],"metadatanum");
			 sprintf(atval[0],"'%s'",data_value_2);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",atoi(mdata_num));
                         i = changeAttrValue(atname, atval,"MDAS_AD_MDATA", 3);
#endif
			 if (i != 0) return (MDAS_AD_MDATA_UPDATE_ERROR);

			 break; 
    case D_INSERT_USER_DEFINED_STRING_META_DATA:  
                         strcpy(reg_action_name,"insert user defined string meta data");
      		         sprintf(sqlq,"select data_id from %sMDAS_AD_MDATA where  data_id = %i ", MDASSCHEMENAME, obj_data_id);
			 if (check_exists(sqlq) == 0) {
			   sprintf(sqlq,"select max(metadatanum) from  %sMDAS_AD_MDATA where data_id =  %i ", MDASSCHEMENAME, obj_data_id);
			   mdata_maxnum = get_itype_value_from_query(sqlq) ;
			   if (mdata_maxnum < 0) {
			     if (mdata_maxnum != -1) 
			       return (mdata_maxnum);
			     sprintf(sqlq,"delete from %sMDAS_AD_MDATA where data_id =  %i  and metadatanum = -1",
				     MDASSCHEMENAME,obj_data_id);
			     ExecSqlDb2(henv, hdbc, sqlq);
			   }
			   mdata_maxnum++;
			 }
			 else {
			   mdata_maxnum =  0;
			 }
			 if (atoi(data_value_1) < 0) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(udmdName,"userdef_metastr%i",
				 atoi(data_value_1));
			 sprintf(atname[0],"%s",udmdName);
                         sprintf(atname[1],"data_id");
			 sprintf(atname[2],"metadatanum");
			 sprintf(atname[3],"userdef_modif_date");
			 sprintf(atname[4],"userdef_creat_date");
			 sprintf(atval[0],"'%s'",data_value_2);
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",mdata_maxnum);
			 sprintf(atval[3],"'%s'",timeval);
			 sprintf(atval[4],"'%s'",timeval);

#ifdef FED_MCAT
                         i = insertIntoTable(atname, atval,"MDAS_AD_MDATA", 5);
#else
                         i = insertIntoTable(atname, atval,"MDAS_AD_MDATA", 3);
#endif
			 if (i != 0) return (MDAS_AD_MDATA_INSERTION_ERROR);

 			 return(mdata_maxnum);
			 break; 
    case D_DELETE_USER_DEFINED_STRING_META_DATA:  
                         strcpy(reg_action_name,"delete user defined string meta data");
			 if (atoi(data_value_1) < -1) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
                         sprintf(atname[0],"data_id");
			 sprintf(atname[1],"metadatanum");
			 sprintf(atval[0],"%i",obj_data_id);
			 if (atoi(data_value_1) == -1) {
			   i = deleteFromTable(atname,atval,"MDAS_AD_MDATA",1);
			 }
			 else {
			   sprintf(atval[1],"%i",atoi(data_value_1));
			   i = deleteFromTable(atname,atval,"MDAS_AD_MDATA",2);
			 }
			 if (i != 0) return (MDAS_AD_MDATA_DELETION_ERROR);

			 break; 

    case D_CHANGE_USER_DEFINED_INTEGER_META_DATA:  
                         strcpy(mdata_attrnum,"");
			 strcpy(mdata_num,"");
                         splitbychar(data_value_1, mdata_attrnum, mdata_num,'@');
                         strcpy(reg_action_name,"change user defined integer meta data");			 
			 if (strlen(mdata_attrnum) == 0 || 
			     strlen(mdata_num) == 0 ||
			     atoi(mdata_attrnum) < 0 || 
			     atoi(mdata_num) < 0 ) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(udmdName,"userdef_metaint%i",
				 atoi(mdata_attrnum));
#ifdef FED_MCAT
			 sprintf(atname[0],"%s",udmdName);
			 sprintf(atname[1],"userdef_modif_date");
                         sprintf(atname[2],"data_id");
			 sprintf(atname[3],"metadatanum");
			 sprintf(atval[0],"%i",atoi(data_value_2));
			 sprintf(atval[1],"'%s'",timeval);
			 sprintf(atval[2],"%i",obj_data_id);
			 sprintf(atval[3],"%i",atoi(mdata_num));
                         i = changeAttrValueSet(atname, atval,"MDAS_AD_MDATA", 4,2);
#else
			 sprintf(atname[0],"%s",udmdName);
                         sprintf(atname[1],"data_id");
			 sprintf(atname[2],"metadatanum");
			 sprintf(atval[0],"%i",atoi(data_value_2));
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",atoi(mdata_num));
                         i = changeAttrValue(atname, atval,"MDAS_AD_MDATA", 3);
#endif
			 if (i != 0) return (MDAS_AD_MDATA_UPDATE_ERROR);


			 break; 
    case D_INSERT_USER_DEFINED_INTEGER_META_DATA:  
                         strcpy(reg_action_name,"insert user defined integer meta data");
      		         sprintf(sqlq,"select data_id from %sMDAS_AD_MDATA where  data_id = %i ", MDASSCHEMENAME, obj_data_id);
			 if (check_exists(sqlq) == 0) {
			   sprintf(sqlq,"select max(metadatanum) from  %sMDAS_AD_MDATA where data_id =  %i ", MDASSCHEMENAME, obj_data_id);
			   mdata_maxnum = get_itype_value_from_query(sqlq) ;
			   if (mdata_maxnum < 0) {
                             if (mdata_maxnum != -1)
			       return (mdata_maxnum);
			     sprintf(sqlq,"delete from %sMDAS_AD_MDATA where data_id =  %i  and metadatanum = -1",
                                     MDASSCHEMENAME,obj_data_id);
			     ExecSqlDb2(henv, hdbc, sqlq);
                           }
			   mdata_maxnum++;
			 }
			 else {
			   mdata_maxnum =  0;
			 }
			 if (atoi(data_value_1) < 0) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(udmdName,"userdef_metaint%i",
				 atoi(data_value_1));
			 sprintf(atname[0],"%s",udmdName);
                         sprintf(atname[1],"data_id");
			 sprintf(atname[2],"metadatanum");
			 sprintf(atname[3],"userdef_modif_date");
			 sprintf(atname[4],"userdef_creat_date");
			 sprintf(atval[0],"%i",atoi(data_value_2));
			 sprintf(atval[1],"%i",obj_data_id);
			 sprintf(atval[2],"%i",mdata_maxnum);
			 sprintf(atval[3],"'%s'",timeval);
			 sprintf(atval[4],"'%s'",timeval);

#ifdef FED_MCAT
                         i = insertIntoTable(atname, atval,"MDAS_AD_MDATA", 5);
#else
                         i = insertIntoTable(atname, atval,"MDAS_AD_MDATA", 3);
#endif
			 if (i != 0) return (MDAS_AD_MDATA_INSERTION_ERROR);
			 return(mdata_maxnum);
			 break; 
    case D_DELETE_USER_DEFINED_INTEGER_META_DATA:  
                         strcpy(reg_action_name,"delete user defined integer meta data");
			 if (atoi(data_value_1) < 0) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
                         sprintf(atname[0],"data_id");
			 sprintf(atname[1],"metadatanum");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"%i",atoi(data_value_1));

                         i = deleteFromTable(atname, atval,"MDAS_AD_MDATA", 2);
			 if (i != 0) return (MDAS_AD_MDATA_DELETION_ERROR);

			 break; 
    case D_COPY_ANNOTATIONS_FROM_DATA_TO_DATA:
                         strcpy(reg_action_name,"insert annotations");
			 sprintf(srcCollPath,"%s/%s",data_value_1,data_value_2);
			 newObjIntId = getObjIdFromCollPathForUserIdAccs(
					     srcCollPath, obj_user_id, "read");
			 if (newObjIntId < 0) return(newObjIntId);
			 timeval[11]='\0';
			 sprintf(sqlq, "insert into %sMDAS_AD_ANNOTATION \
(data_id, repl_enum,annotations,anno_date,user_id,anno_position ) \
select %i, %i, annotations,'%s'||substr(anno_date,12,9),%i,anno_position \
from %sMDAS_AD_ANNOTATION where data_id = %i",
				 MDASSCHEMENAME, obj_data_id,replication_num,
				 timeval, obj_user_id,
				 MDASSCHEMENAME,newObjIntId );
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0) 		      
			   return(MDAS_AD_ANNOTATION_INSERTION_ERROR);

			 break; 
    case D_UPDATE_CONTAINER_FOR_COLLECTION:
                     /* dv1= changed or extended (overflowed) containername*/
			   strcpy(reg_action_name,"update container for collection");
			   sprintf(sqlq,"select container_id from %sMDAS_TD_CONTAINER where  container_name = '%s' ", MDASSCHEMENAME, data_value_1);
			   tabledIsOkay = 1;
			   intval = get_itype_value_from_query(sqlq) ;
			   tabledIsOkay = 0;
			   if (intval < 0) 
			     return (intval);
			   sprintf(sqlq,"update %sMDAS_AD_COLLCONT set container_id =%i where container_id = %i", MDASSCHEMENAME, obj_data_id, intval );
			   i = ExecSqlDb2(henv, hdbc, sqlq);
			   if (i != 0) return (MDAS_AD_COLLCONT_UPDATE_ERROR);

			 break;
    case D_COPY_META_DATA_FROM_CONTAINER_TO_NEW_CONTAINER:
                         strcpy(reg_action_name,"insert access");
			 sprintf(srcCollPath,"%s/%s",data_value_1,data_value_2);
			 newObjIntId = getObjIdFromCollPathForUserIdAccs(
					     srcCollPath, obj_user_id, "all");
			 if (newObjIntId < 0) return(newObjIntId);
			 intval = get_max_repl_num(obj_data_id);
			 if (intval < 0)  return (intval);
			 sprintf(sqlq, "insert into %sMDAS_AD_ACCS (data_id, repl_enum, user_id,access_id) select %i, 0,  user_id, access_id from %sMDAS_AD_ACCS where data_id = %i and repl_enum = %i and user_id <> %i",
				 MDASSCHEMENAME,obj_data_id,MDASSCHEMENAME,
				 newObjIntId, intval, obj_user_id);
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0) 		      
			   return(MDAS_AD_ACCS_INSERTION_ERROR);
			 break; 
    case D_COPY_META_DATA_FROM_DATA_TO_DATA:
                         strcpy(reg_action_name,"insert user defined string meta data");
      		         sprintf(sqlq,"select data_id from %sMDAS_AD_MDATA where  data_id = %i ", MDASSCHEMENAME, obj_data_id);
			 if (check_exists(sqlq) == 0) {
			   sprintf(sqlq,"select max(metadatanum) from  %sMDAS_AD_MDATA where data_id =  %i ", MDASSCHEMENAME, obj_data_id);
			   mdata_maxnum = get_itype_value_from_query(sqlq) ;
			   if (mdata_maxnum < 0) {
                             if (mdata_maxnum != -1)
			       return (mdata_maxnum);
			     sprintf(sqlq,"delete from %sMDAS_AD_MDATA where data_id =  %i  and metadatanum = -1",
				     MDASSCHEMENAME, obj_data_id);
			     ExecSqlDb2(henv, hdbc, sqlq);
                           }
			   mdata_maxnum++;
			 }
			 else {
			   mdata_maxnum =  0;
			 }
			 sprintf(srcCollPath,"%s/%s",data_value_1,data_value_2);
			 newObjIntId = getObjIdFromCollPathForUserIdAccs(
					     srcCollPath, obj_user_id, "read");
			 if (newObjIntId < 0) return(newObjIntId);
#ifdef FED_MCAT
			 sprintf(sqlq, "insert into %sMDAS_AD_MDATA \
(data_id, metadatanum,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1,userdef_creat_date,userdef_modif_date )  \
select %i, metadatanum + %i,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1,userdef_creat_date,userdef_modif_date from %sMDAS_AD_MDATA where data_id = %i",
				   MDASSCHEMENAME, obj_data_id, mdata_maxnum,
				   MDASSCHEMENAME,newObjIntId );
#else
			 sprintf(sqlq, "insert into %sMDAS_AD_MDATA \
(data_id, metadatanum,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1)  \
select %i, metadatanum + %i,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1 from %sMDAS_AD_MDATA where data_id = %i",
				   MDASSCHEMENAME, obj_data_id, mdata_maxnum,
				   MDASSCHEMENAME,newObjIntId );
#endif
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0) 		      
			   return(MDAS_AD_MDATA_INSERTION_ERROR);
			 break; 
    case D_COPY_META_DATA_FROM_COLL_TO_DATA:
                         strcpy(reg_action_name,"insert user defined string meta data");
      		         sprintf(sqlq,"select data_id from %sMDAS_AD_MDATA where  data_id = %i ", MDASSCHEMENAME, obj_data_id);
			 if (check_exists(sqlq) == 0) {
			   sprintf(sqlq,"select max(metadatanum) from  %sMDAS_AD_MDATA where data_id =  %i ", MDASSCHEMENAME, obj_data_id);
			   mdata_maxnum = get_itype_value_from_query(sqlq) ;
			   if (mdata_maxnum < 0) {
                             if (mdata_maxnum != -1)
			       return (mdata_maxnum);
			     sprintf(sqlq,"delete from %sMDAS_AD_MDATA where data_id =  %i  and metadatanum = -1",
                                     MDASSCHEMENAME,obj_data_id);
                             ExecSqlDb2(henv, hdbc, sqlq);
                           }
			   mdata_maxnum++;
			 }
			 else {
			   mdata_maxnum =  0;
			 }

			 i = getCollIdFromCollPathForUserIdAccs(
				   srcCollId, data_value_1, obj_user_id, "read");
			 if (i < 0) return(i);
#ifdef FED_MCAT
			 sprintf(sqlq, "insert into %sMDAS_AD_MDATA \
(data_id, metadatanum,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1,userdef_creat_date,userdef_modif_date )  \
select %i, metadatanum + %i,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1,userdef_creat_date,userdef_modif_date from %sMDAS_AD_COLLMDATA where data_grp_id = '%s'",
				   MDASSCHEMENAME, obj_data_id, mdata_maxnum,
				   MDASSCHEMENAME,srcCollId );
#else
			 sprintf(sqlq, "insert into %sMDAS_AD_MDATA \
(data_id, metadatanum,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1 )  \
select %i, metadatanum + %i,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1 from %sMDAS_AD_COLLMDATA where data_grp_id = '%s'",
				   MDASSCHEMENAME, obj_data_id, mdata_maxnum,
				   MDASSCHEMENAME,srcCollId );
#endif
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0) 		      
			   return(MDAS_AD_MDATA_INSERTION_ERROR);
			 break; 
    case D_DELETE_USER_DEFINED_ATTR_VAL_META_DATA:
                         strcpy(reg_action_name,"delete user defined string meta data");
                         sprintf(atname[0],"data_id");
			 sprintf(atname[1],"userdef_metastr0");
			 sprintf(atname[2],"userdef_metastr1");
			 sprintf(atval[0],"%i",obj_data_id);
			 sprintf(atval[1],"'%s'",data_value_1);
			 sprintf(atval[2],"'%s'",data_value_2);
			 i = deleteFromTable(atname,atval,"MDAS_AD_MDATA",3);
			 if (i != 0) return (MDAS_AD_MDATA_DELETION_ERROR);
			 break; 
    default:
                         return (MDAS_CATALOG_MODIFY_TYPE_ERROR);
			 break;
    }
  removeTabledQueries(obj_data_name);
  removeTabledQueries(didStr);
  return(MDAS_SUCCESS);
}	

int	
check_owner_id_of_collection(char *data_group_id, int obj_user_id)
{
   sprintf(sqlq,"select coll_owner from %sMDAS_TD_DATA_GRP where  data_grp_id = '%s' AND coll_owner = %i ", MDASSCHEMENAME, data_group_id,obj_user_id);
   return(check_exists(sqlq));

}
int modify_collection_info(int cat_type, char *obj_user_name,
			   char *group_name, 
			 char *in_data_value_1, char *data_value_2,
			   char *data_value_3,
			 int retraction_type, char *user_domain_name)

{ 

  int   jj, obj_user_id, obj_action_id, obj_domain_id;
  char data_group_id[MAX_TOKEN];
  char datval[MAX_TOKEN], colval[MAX_TOKEN],datval2[MAX_TOKEN];
  char data_value_1[MAX_TOKEN * 2];
  char mdata_attrnum[MAX_TOKEN], mdata_num[MAX_TOKEN];
  int mdata_maxnum, oldCollNameLen;
  char udmdName[MAX_TOKEN];
  int newObjIntId;
  char srcCollPath[2 * MAX_TOKEN];
  char srcCollId[MAX_TOKEN];
  char *tmpPtr;
  char localZoneName[MAX_TOKEN];
  char *localMcatName;

    failure = 0;
    if (in_data_value_1 != NULL && strlen(in_data_value_1) > 0)
      strcpy(data_value_1,in_data_value_1);
    else
      strcpy(data_value_1,"");
   strcpy(reg_registrar_name,obj_user_name);
   strcpy(reg_obj_name, group_name);
   strcpy(reg_obj_code,"MDAS_CD_USER");
   strcpy(reg_action_name,"modify collection");

   obj_user_id = get_user_id_in_domain(obj_user_name, user_domain_name); 
   if (obj_user_id < 0) { return (USER_NOT_IN_DOMN); }
   reg_registrar_id = obj_user_id;

  get_ctype_value(data_group_id, "MDAS_TD_DATA_GRP.data_grp_name", group_name);
   if (failure != 0) 
     { return (COLLECTION_NOT_IN_CAT);}

   if (retraction_type > SYS_ACTION_TYPE_MIN && retraction_type < SYS_ACTION_TYPE_MAX) {
     if (is_system_userid(obj_user_id) != 0)
       return (SYS_USER_AUTHORIZATION_ERROR);
     i  = 0;
     failure = 0;
   }
   else if (isUserDomainAdminFlag == 1  && 
	    retraction_type == D_INSERT_COLL_ACCS && 
	    strstr(group_name,user_domain_name) ) {
     i  = 0; 
     failure = 0;
   }
   else if (retraction_type > CURATOR_ACTION_TYPE_MIN && retraction_type < CURATOR_ACTION_TYPE_MAX) {
     if (is_curator_userid(obj_user_id,group_name ) != 0)
	 return (USER_NOT_CURATOR_ERROR);
     i =  0;
     failure = 0;
     retraction_type = retraction_type - CURATOR_ACTION_TYPE_MIN;
   }
   else if (retraction_type == C_DELETE_ANNOTATIONS ||
       retraction_type == C_INSERT_ANNOTATIONS ||
       retraction_type == C_COPY_ANNOTATIONS_FROM_COLL_TO_COLL ||
       retraction_type == C_UPDATE_ANNOTATIONS) {
     i =  check_permit_in_group("annotate%", data_group_id, obj_user_id);
   }
   else if (retraction_type == C_CHANGE_COLL_OWNER) {
     i = check_owner_id_of_collection(data_group_id, obj_user_id);
   }
   else   {
     i = check_permit_in_group("all%", data_group_id, obj_user_id);
     if (i != 0 && retraction_type == D_INSERT_COLL_ACCS)
       i = check_owner_id_of_collection(data_group_id, obj_user_id);
   }
   if (i != 0 ) 
     return (i);


  failure = 0;
   reg_obj_repl_enum = 0;
   get_time(timeval);
   strcpy(reg_timeval,timeval );
   reg_obj_id = 0;
   sprintf(query_string2,"%s:%s:%s:%s",group_name,data_value_1,data_value_2,data_value_3);
   performAudit(retraction_type,obj_user_id,0,data_group_id,query_string2);

   switch (retraction_type)
    {
    case C_BULK_INSERT_UDEF_META_DATA_FOR_COLL:
                        i =  bulkInsertUdefMetadataForColl(data_group_id,
				       data_value_1, data_value_2,"");
			if (i != 0) return (i);
			break; 
      
    case D_CHANGE_INCOLL_OWNER:
                         /* dv1=user_name,dv2=domain_name,dv3=accesspermit*/
                         strcpy(reg_action_name,"insert collection access");
			 intval = get_user_id_in_domain(
					     data_value_1, data_value_2);

			 if (intval < 0) { return (USER_NOT_IN_DOMN);}
			 i =insertMultiOwner(obj_user_id, data_group_id,intval,"D");
			 if (i != 0) return (MDAS_AD_GRP_ACCS_INSERTION_ERROR);
			 break;
    case D_INSERT_INCOLL_ACCS:
                         /* dv1=user_name,dv2=domain_name,dv3=accesspermit*/
                         strcpy(reg_action_name,"insert collection access");
			 intval = get_user_id_in_domain(
					     data_value_1, data_value_2);

			 if (intval < 0) { return (USER_NOT_IN_DOMN);}
			 tabledIsOkay = 1;
                         intval2 = get_itype_value(
					 "MDAS_TD_DS_ACCS.access_constraint",
					 data_value_3);
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (ACCESS_TYPE_NOT_IN_CAT);
			 }
			 i =insertMultiAccs(obj_user_id, data_group_id,intval,intval2,"D");
			 if (i != 0) return (MDAS_AD_GRP_ACCS_INSERTION_ERROR);
			 break;
    case D_CHANGE_INCOLL_OWNER_RECUR:
                         /* dv1=user_name,dv2=domain_name,dv3=accesspermit*/
                         strcpy(reg_action_name,"insert collection access");
			 intval = get_user_id_in_domain(
					     data_value_1, data_value_2);

			 if (intval < 0) { return (USER_NOT_IN_DOMN);}
			 i =insertMultiOwner(obj_user_id, data_group_id,intval,"R");
			 if (i != 0) return (MDAS_AD_GRP_ACCS_INSERTION_ERROR);
			 break;
    case D_INSERT_INCOLL_ACCS_RECUR:
                         /* dv1=user_name,dv2=domain_name,dv3=accesspermit*/
                         strcpy(reg_action_name,"insert collection access");
			 intval = get_user_id_in_domain(
					     data_value_1, data_value_2);

			 if (intval < 0) { return (USER_NOT_IN_DOMN);}
			 tabledIsOkay = 1;
                         intval2 = get_itype_value(
					 "MDAS_TD_DS_ACCS.access_constraint",
					 data_value_3);
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (ACCESS_TYPE_NOT_IN_CAT);
			 }
			 i =insertMultiAccs(obj_user_id, data_group_id,intval,intval2,"R");
			 if (i != 0) return (MDAS_AD_GRP_ACCS_INSERTION_ERROR);
			 break;
    case D_DELETE_INCOLL_ACCS:
                         /* dv1=user_name,dv2=domain_name,dv3=accesspermit*/
                         strcpy(reg_action_name,"insert collection access");
			 intval = get_user_id_in_domain(
					     data_value_1, data_value_2);

			 if (intval < 0) { return (USER_NOT_IN_DOMN);}
			 i =deleteMultiAccs(obj_user_id, data_group_id,intval,intval2,"D");
			 if (i != 0) return (MDAS_AD_GRP_ACCS_INSERTION_ERROR);
			 break;
    case D_DELETE_INCOLL_ACCS_RECUR:
                         /* dv1=user_name,dv2=domain_name,dv3=accesspermit*/
                         strcpy(reg_action_name,"insert collection access");
			 intval = get_user_id_in_domain(
					     data_value_1, data_value_2);
			 if (intval < 0) { return (USER_NOT_IN_DOMN);}
			 i =deleteMultiAccs(obj_user_id, data_group_id,intval,intval2,"R");
			 if (i != 0) return (MDAS_AD_GRP_ACCS_INSERTION_ERROR);
			 break;
    case D_INSERT_COLL_ACCS:
    case C_SU_INSERT_COLL_ACCS:
                        /* dv1=user_name,dv2=domain_name,dv3=accesspermit*/
                         strcpy(reg_action_name,"insert collection access");
			 intval = get_user_id_in_domain(
					     data_value_1, data_value_2);

			 if (intval < 0) { return (USER_NOT_IN_DOMN);}
			 tabledIsOkay = 1;
                         intval2 = get_itype_value(
					 "MDAS_TD_DS_ACCS.access_constraint",
					 data_value_3);
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (ACCESS_TYPE_NOT_IN_CAT);
			 }
                         sprintf(atname[0],"data_grp_id");
                         sprintf(atname[1],"user_id");
			 sprintf(atval[0],"'%s'",data_group_id);
			 sprintf(atval[1],"%i",intval);
                         i=deleteFromTable(atname, atval,"MDAS_AD_GRP_ACCS",2);
                         sprintf(atname[2],"access_id");
			 sprintf(atval[2],"%i",intval2);
                         i=insertIntoTable(atname, atval,"MDAS_AD_GRP_ACCS",3);
			 if (i != 0) return (MDAS_AD_GRP_ACCS_INSERTION_ERROR);
			 break;
    case D_DELETE_COLL_ACCS:
    case C_SU_DELETE_COLL_ACCS:
                         /* dv1=user_name,dv2=domain_name,dv3=accesspermit*/
                         strcpy(reg_action_name,"delete collection access");
			 intval = get_user_id_in_domain(
					     data_value_1, data_value_2);
			 if (intval < 0) { return (USER_NOT_IN_DOMN);}
                         /*intval2 = get_itype_value(
					 "MDAS_TD_DS_ACCS.access_constraint",
					 data_value_3);
			 if (failure != 0) {
			   return (ACCESS_TYPE_NOT_IN_CAT);
			 }*/
                         sprintf(atname[0],"data_grp_id");
                         sprintf(atname[1],"user_id");
                         /*sprintf(atname[2],"access_id");*/
			 sprintf(atval[0],"'%s'",data_group_id);
			 sprintf(atval[1],"%i",intval);
			 /*sprintf(atval[2],"%i",intval2);
                         i=deleteFromTable(atname, atval,"MDAS_AD_GRP_ACCS",3);
			 */
			 i=deleteFromTable(atname, atval,"MDAS_AD_GRP_ACCS",2);
			 if (i != 0) return (MDAS_AD_GRP_ACCS_DELETION_ERROR);
			 removeTabledQueries(group_name);
			 removeTabledQueries(data_group_id);

			 break;
    case D_UPDATE_COLL_COMMENTS:
                         strcpy(reg_action_name,"update comments");
                         sprintf(atname[0],"coll_comments");
                         sprintf(atname[1],"data_grp_id");
			 sprintf(atval[0],"'%s'",data_value_1);
			 sprintf(atval[1],"'%s'",data_group_id);
			 i = changeAttrValue(atname, atval,"MDAS_TD_DATA_GRP",2);
			 if (i != 0) 
			   return (MDAS_TD_DATA_GRP_UPDATE_ERROR);
			 break;
    case C_MOVE_COLL_TO_TRASH:
#ifdef FED_MCAT
                         /* use data_value_1 as tmp */
                         sprintf (data_value_1, "%s.%s",
                          obj_user_name, user_domain_name);

			 localMcatName = (char *) getLocalMcatName();
                         if (strstr (group_name, data_value_1) == NULL) {
                            /* not user normal home path */
                            if (strstr(group_name, localMcatName)
                                != (char *) (group_name +1)) {
                               /* move forward 3 '/' */
                               tmpPtr = forwardPath (group_name, 3);
                               if (tmpPtr != NULL) {
                                  sprintf(data_value_1, "%s/home/%s.%s/%s::%s",
                                   DEF_TRASH_COLL, obj_user_name, 
				   user_domain_name, tmpPtr, timeval);
                               } else {
                                  sprintf(data_value_1,"%s/home/%s.%s%s::%s",
                                   DEF_TRASH_COLL, obj_user_name, 
				   user_domain_name, group_name, timeval);
                               }
                            } else {
                               /* move forward 4 '/' */
                               tmpPtr = forwardPath (group_name, 4);
                               if (tmpPtr != NULL) {
                                  sprintf(data_value_1,"/%s%s/home/%s.%s/%s::%s",
                                   localMcatName, DEF_TRASH_COLL,
                                   obj_user_name, user_domain_name, tmpPtr, 
				   timeval);
                               } else {
                                  tmpPtr = strstr ((char *)(group_name +1),
                                            "/");
                                  if (tmpPtr == NULL)
                                      return(ACTION_NOT_ALLOWED);
                                  sprintf(data_value_1,"/%s%s/home/%s.%s%s::%s",
                                    localMcatName, DEF_TRASH_COLL,
                                    obj_user_name, user_domain_name, 
				    group_name, timeval);
                               }
                            }
		         } else {
			    if (strstr(group_name, localMcatName)
			        != (char *) (group_name +1)) {
			      sprintf(data_value_1,"%s%s::%s",
				   DEF_TRASH_COLL,group_name,timeval);
			    } else {
			       tmpPtr = strstr ((char *) (group_name +1),
					    "/");
			       if (tmpPtr == NULL)
			          return(ACTION_NOT_ALLOWED);
			       sprintf(data_value_1,"/%s%s%s::%s",localMcatName,
				   DEF_TRASH_COLL,tmpPtr,timeval);
			     }
		         }
#else
			 sprintf(data_value_1,"%s%s::%s",
				 DEF_TRASH_COLL,group_name,timeval);
#endif
    case C_MOVE_COLL_TO_NEWCOLL:
			 splitbychar(data_value_1, datval,datval2,'/');
			 splitbychar(datval, colval,datval2,'/');
			 i = make_new_collection_path(cat_type, 
				  colval,datval2,
				  obj_user_name,user_domain_name);
			 if (i != 0)
			   return(i);
    case D_CHANGE_COLL_NAME:
    case C_SU_CHANGE_COLL_NAME:
                         strcpy(reg_action_name,"change collection name");
                         /* check if the move is being done underneath itself */
			 sprintf(colval,"%s/",group_name);
			 if (strstr(data_value_1, colval) == data_value_1)
			   return(ACTION_NOT_ALLOWED);
			 sprintf(sqlq, 
			   "select  data_grp_id from %sMDAS_TD_DATA_GRP where data_grp_name = '%s'",
				 MDASSCHEMENAME, data_value_1);
			 if (check_exists(sqlq) == 0) 
			   {
			     failure = COLLECTION_ALREADY_IN_CAT;
			     return (COLLECTION_ALREADY_IN_CAT);
			   }
			 failure = 0;
			 strcpy(datval,data_value_1);
			 splitbychar(datval, colval,datval2,'/');
			 if (strcmp(colval,""))
			 {
			   get_ctype_value(chval, 
				  "MDAS_TD_DATA_GRP.data_grp_name", colval);
			   if (failure != 0) 
			     { return (COLLECTION_NOT_IN_CAT);}
			   i = check_permit_in_group("all%", 
						     chval, obj_user_id);
			   if (i !=0)  { return (i);}
			 }
			 else {
			   /***
			   return (COLLECTION_NAME_ERROR);
			   ****/
			   strcpy(colval,"/");
			   failure = 0;
			   get_ctype_value(chval, 
				  "MDAS_TD_DATA_GRP.data_grp_name",  colval);
			   if (failure != 0) 
			     { return (COLLECTION_NOT_IN_CAT);}
			   i = check_permit_in_group("all%", 
						     chval, obj_user_id);
			   if (i !=0)  { return (i);}
				   
			 }
			 sprintf(atname[0],"data_grp_name");
                         sprintf(atname[1],"data_grp_id");
                         sprintf(atval[0],"'%s'",data_value_1);
			 sprintf(atval[1],"'%s'",data_group_id);
                         i = changeAttrValue(atname, atval,
					     "MDAS_TD_DATA_GRP", 2);
			 if (i != 0) return (MDAS_TD_DATA_GRP_UPDATE_ERROR);
			 sprintf(atname[0],"parent_grp_name");
                         sprintf(atname[1],"data_grp_id");
                         sprintf(atval[0],"'%s'",colval);
			 sprintf(atval[1],"'%s'",data_group_id);
                         i = changeAttrValue(atname, atval,
					     "MDAS_TD_DATA_GRP", 2);
			 if (i != 0) return (MDAS_TD_DATA_GRP_UPDATE_ERROR);
			 removeTabledQueries(group_name);
			 removeTabledQueries(data_value_1);
			 removeTabledQueries(data_group_id);
			 /* added new for changing descendents also. Nov 8, 2000*/
			 /* update collection name */
			 oldCollNameLen = strlen(group_name) + 1;
			 if (oldCollNameLen > (strlen(data_value_1) + 1))
			   jj = DEFAULT_ATTR_SIZE - oldCollNameLen;
			 else
			   jj = DEFAULT_ATTR_SIZE - (strlen(data_value_1) + 1);
			 strcpy(srcCollPath, group_name);
			 mcatEscapeWildCards(srcCollPath);
#ifdef FED_MCAT
			 sprintf(sqlq,"update %sMDAS_TD_DATA_GRP set data_grp_name = '%s'||substr(data_grp_name,%i,%i) , coll_mod_timestamp = '%s' where data_grp_name like '%s/%%'  ESCAPE '\\'",
			     MDASSCHEMENAME,data_value_1, oldCollNameLen, 
			     jj, timeval, srcCollPath);
#else
			 sprintf(sqlq,"update %sMDAS_TD_DATA_GRP set data_grp_name = '%s'||substr(data_grp_name,%i,%i)  where data_grp_name like '%s/%%'  ESCAPE '\\'",
			     MDASSCHEMENAME,data_value_1, oldCollNameLen, 
			     jj, srcCollPath);
#endif
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0)  {
			   elog(NOTICE,"Failure to update collection name: %s\n",sqlq);
			     return(MDAS_TD_DATA_GRP_UPDATE_ERROR);
			 }
			 /* update parent coll name */
			 sprintf(sqlq,"update %sMDAS_TD_DATA_GRP set parent_grp_name = '%s'||substr(parent_grp_name,%i,%i) where parent_grp_name = '%s' or parent_grp_name like '%s/%%' ESCAPE '\\'",
			     MDASSCHEMENAME,data_value_1, oldCollNameLen, 
			     jj, group_name,
			     srcCollPath);			     
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0)  {
			   elog(NOTICE,"Failure to update collection name: %s\n",sqlq);
			     return(MDAS_TD_DATA_GRP_UPDATE_ERROR);
			 }
			 /* update any container which is in that collection */
			 sprintf(sqlq,"update %sMDAS_TD_CONTAINER set container_name = '%s'||substr(container_name,%i,%i)  where container_name like '%s/%%'  ESCAPE '\\'",
			     MDASSCHEMENAME,data_value_1, oldCollNameLen, 
			     jj, srcCollPath);
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0)  {
			   elog(NOTICE,"Failure to update container name when moving collection SQL: %s\n",sqlq);
			     return(MDAS_TD_CONTAINER_UPDATE_ERROR);
			 }
			 
			 /****		
update MDAS_TD_DATA_GRP set data_grp_name = '/home/alpha/beta'||substr(data_grp_name,24,226) where data_grp_name like '/home/sekar.sdsc/mytest/%';
                           123456789012345678901234567890
update MDAS_TD_DATA_GRP set (parent_grp_name = '/home/alpha/beta'||substr(parent_grp_name,24,226) where  parent_grp_name = '/home/sekar.sdsc/mytest' or parent_grp_name like '/home/sekar.sdsc/mytest/%' ;	
			 ****/
		 /* added new for changing descendents also. Nov 8, 2000 */
			 i = inheritACLWithSticky( chval,0,0,data_value_1,obj_user_id,"recur-collection-move");
			 if (i != 0) return(i);
			 break;
		
      /*
    case C_INSERT_LINK:
                         strcpy(reg_action_name,"insert link");
			 i = makeCollLink(data_value_2,data_value_1,
					  data_group_id,obj_user_id);
			 if (i !=0)  { return (i);}
			 break;
      */
    case C_SU_DELETE_TRASH_COLL:
                         if (strstr(group_name,DEF_TRASH_COLL) != group_name) {
			 failure = 0;
			 sprintf(sqlq, "select zone_id from %s%s where local_zone_flag = 1",
				 MDASSCHEMENAME, "MDAS_TD_ZONE");
			 tabledIsOkay = 1;
			 get_ctype_value_from_query(localZoneName,sqlq);
			 tabledIsOkay = 0;
			 if (failure != 0) 
			   return(ACTION_NOT_ALLOWED);
			 sprintf(sqlq, "/%s%s",localZoneName,DEF_TRASH_COLL);
			 if (strstr(group_name,sqlq) !=group_name)
			   return(ACTION_NOT_ALLOWED);
			 }
			 failure = 0;
    case D_DELETE_COLL:
                         strcpy(reg_action_name,"delete collection");
			 i = 20;
			 /****** RAJA MCAT COLL_LINK PROBLEM ***  DEC2002
			 sprintf(sqlq, "select  data_grp_id from %sMDAS_TD_DATA_GRP where data_grp_name = '%s' AND coll_link > 0",
				   MDASSCHEMENAME, group_name);
			 i = check_exists(sqlq);
			 ********************/
			 if (i != 0) {
			   strcpy(srcCollPath, group_name);
			   mcatEscapeWildCards(srcCollPath);
			   sprintf(sqlq, 
				   "select  data_grp_id from %sMDAS_TD_DATA_GRP where data_grp_name like '%s/%%'  ESCAPE '\\' AND data_grp_name not like '%s/%%$deleted%%'  ESCAPE '\\'",
				   MDASSCHEMENAME, srcCollPath,srcCollPath);
			   
			   /*printf("AA:%i:%s\n",check_exists(sqlq),sqlq);*/
			   if (check_exists(sqlq) == 0) 
			     {
			       failure = COLLECTION_NOT_EMPTY;
			       return (COLLECTION_NOT_EMPTY);
			     }
			   /*printf("AA:%i:%s\n",check_exists(sqlq),sqlq);*/
			   sprintf(sqlq, 
				   "select  data_id from %sMDAS_AD_REPL where data_grp_id = '%s' AND is_deleted <> 1 ",
				   MDASSCHEMENAME, data_group_id);
			   if (check_exists(sqlq) == 0) 
			     {
			       failure = COLLECTION_NOT_EMPTY;
			       return (COLLECTION_NOT_EMPTY);
			     }
			   /**** June 5 2000 - changed to really delete 
				 sprintf(atname[0],"data_grp_name");
				 sprintf(atname[1],"data_grp_id");
				 sprintf(atval[0],"'%s$deleted'",group_name);
				 sprintf(atval[1],"'%s'",data_group_id);
				 i = changeAttrValue(atname, atval,
				 "MDAS_TD_DATA_GRP", 2);
				 if (i != 0) return (MDAS_TD_DATA_GRP_UPDATE_ERROR);
			   ****/
			   sprintf(atname[0],"data_grp_id");
			   sprintf(atval[0],"'%s'",data_group_id);
			   i=deleteFromTable(atname,atval,"MDAS_AD_COLLMDATA",1);
			   i=deleteFromTable(atname,atval,"MDAS_AD_GRP_ACCS",1);
			   i=deleteFromTable(atname,atval,"MDAS_AD_COLLCONT",1);
#ifndef LEANMCAT
			   sprintf(atname[0],"indexed_datcoll_id");
			   i=deleteFromTable(atname,atval,"MDAS_AD_COLL_INDEX",1);
			   sprintf(atname[0],"methfor_datcoll_id");
			   i=deleteFromTable(atname,atval,"MDAS_AD_COLL_METH",1);
			   sprintf(atname[0],"data_grp_id");
#endif /* LEANMCAT */
			   i=deleteFromTable(atname,atval,"MDAS_TD_DATA_GRP",1);
			   removeTabledQueries(group_name);
			   removeTabledQueries(data_group_id);
			 }
			 else { /* it is a link */
			   sprintf(atname[0],"data_grp_name");
			   sprintf(atval[0],"'%s'",group_name);
			   i=deleteFromTable(atname,atval,"MDAS_TD_DATA_GRP",1);
			   removeTabledQueries(group_name);
			 }
			 
			 break;
    case C_INSERT_CONTAINER_FOR_COLLECTION:
                         /* dv1=containername*/
                         strcpy(reg_action_name,"insert container for collection");										                        sprintf(sqlq,"select container_id from %sMDAS_TD_CONTAINER where  container_name = '%s' ", MDASSCHEMENAME, data_value_1);
			   tabledIsOkay = 1;
			   intval = get_itype_value_from_query(sqlq) ;
			   tabledIsOkay = 0;
			   if (intval < 0) 
			     return (intval);
                         sprintf(atname[0],"data_grp_id");
                         sprintf(atname[1],"container_id");
			 sprintf(atname[2],"data_typ_id");
			 sprintf(atval[0],"'%s'",data_group_id);
			 sprintf(atval[1],"%i",intval);
			 sprintf(atval[2],"'0000'");
			 i=insertIntoTable(atname, atval,"MDAS_AD_COLLCONT",3);
			 if (i != 0) return (MDAS_AD_COLLCONT_INSERTION_ERROR);

			 break;
    case C_DELETE_CONTAINER_FOR_COLLECTION:
                         /* dv1=containername*/
                         strcpy(reg_action_name,"delete container for collection");										                        sprintf(sqlq,"select container_id from %sMDAS_TD_CONTAINER where  container_name = '%s' ", MDASSCHEMENAME, data_value_1);
			   tabledIsOkay = 1;
			   intval = get_itype_value_from_query(sqlq) ;
			   tabledIsOkay = 0;
			   if (intval < 0) 
			     return (intval);
                         sprintf(atname[0],"data_grp_id");
                         sprintf(atname[1],"container_id");
			 sprintf(atval[0],"'%s'",data_group_id);
			 sprintf(atval[1],"%i",intval);
			 i=deleteFromTable(atname, atval,"MDAS_AD_COLLCONT",2);
			 if (i != 0) return (MDAS_AD_COLLCONT_DELETION_ERROR);
			 removeTabledQueries(group_name);
			 removeTabledQueries(data_group_id);
			 removeTabledQueries(data_value_1);
			 break;
    case C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA:  
                         strcpy(mdata_attrnum,"");
			 strcpy(mdata_num,"");
                         splitbychar(data_value_1, mdata_attrnum, mdata_num,'@');
                         strcpy(reg_action_name,"change user defined string meta data for collection");			 
			 if (strlen(mdata_attrnum) == 0 || 
			     strlen(mdata_num) == 0 ||
			     atoi(mdata_attrnum) < 0 || 
			     atoi(mdata_num) < 0 ) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(udmdName,"userdef_metastr%i",
				 atoi(mdata_attrnum));
#ifdef FED_MCAT
			 sprintf(atname[0],"%s",udmdName);
			 sprintf(atname[1],"userdef_modif_date");
                         sprintf(atname[2],"data_grp_id");
			 sprintf(atname[3],"metadatanum");
			 sprintf(atval[0],"'%s'",data_value_2);
			 sprintf(atval[1],"'%s'",timeval);
			 sprintf(atval[2],"'%s'",data_group_id);
			 sprintf(atval[3],"%i",atoi(mdata_num));
                         i=changeAttrValueSet(atname,atval,"MDAS_AD_COLLMDATA",4,2);
#else
			 sprintf(atname[0],"%s",udmdName);
                         sprintf(atname[1],"data_grp_id");
			 sprintf(atname[2],"metadatanum");
			 sprintf(atval[0],"'%s'",data_value_2);
			 sprintf(atval[1],"'%s'",data_group_id);
			 sprintf(atval[2],"%i",atoi(mdata_num));
                         i=changeAttrValue(atname,atval,"MDAS_AD_COLLMDATA",3);
#endif
			 if (i != 0) return (MDAS_AD_COLLMDATA_UPDATE_ERROR);

			 break; 
    case C_INSERT_USER_DEFINED_COLL_STRING_META_DATA:  
                         strcpy(reg_action_name,"insert user defined string meta data for collection");
      		         sprintf(sqlq,"select data_grp_id from %sMDAS_AD_COLLMDATA where  data_grp_id = '%s' ", MDASSCHEMENAME, data_group_id);
			 if (check_exists(sqlq) == 0) {
			   sprintf(sqlq,"select max(metadatanum) from  %sMDAS_AD_COLLMDATA where data_grp_id =  '%s' ", MDASSCHEMENAME, data_group_id);
			   mdata_maxnum = get_itype_value_from_query(sqlq) ;
			   if (mdata_maxnum < 0) {
                             if (mdata_maxnum != -1)
			       return (mdata_maxnum);
			     sprintf(sqlq,"delete from %sMDAS_AD_COLLMDATA where data_grp_id =  '%s' and metadatanum = -1",
				     MDASSCHEMENAME, data_group_id);
			     ExecSqlDb2(henv, hdbc, sqlq);
                           }
			   mdata_maxnum++;
			 }
			 else {
			   mdata_maxnum =  0;
			 }
			 if (atoi(data_value_1) < 0) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(udmdName,"userdef_metastr%i",
				 atoi(data_value_1));
			 sprintf(atname[0],"%s",udmdName);
                         sprintf(atname[1],"data_grp_id");
			 sprintf(atname[2],"metadatanum");
			 sprintf(atname[3],"userdef_modif_date");
			 sprintf(atname[4],"userdef_creat_date");
			 sprintf(atval[0],"'%s'",data_value_2);
			 sprintf(atval[1],"'%s'",data_group_id);
			 sprintf(atval[2],"%i",mdata_maxnum);
			 sprintf(atval[3],"'%s'",timeval);
			 sprintf(atval[4],"'%s'",timeval);

#ifdef FED_MCAT
                         i=insertIntoTable(atname,atval,"MDAS_AD_COLLMDATA",5);
#else
                         i=insertIntoTable(atname,atval,"MDAS_AD_COLLMDATA",3);
#endif
			 if (i != 0) return (MDAS_AD_COLLMDATA_INSERTION_ERROR);

			 return(mdata_maxnum);
			 break; 
    case C_DELETE_USER_DEFINED_COLL_STRING_META_DATA:  
                         strcpy(reg_action_name,"delete user defined string meta data for collection");
			 if (atoi(data_value_1) < -1) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
                         sprintf(atname[0],"data_grp_id");
			 sprintf(atname[1],"metadatanum");
			 sprintf(atval[0],"'%s'",data_group_id);
			 if (atoi(data_value_1) == -1) {
			   i = deleteFromTable(atname,atval,"MDAS_AD_COLLMDATA", 1);
			 }
			 else {
			   sprintf(atval[1],"%i",atoi(data_value_1));
			   i = deleteFromTable(atname, atval,"MDAS_AD_COLLMDATA", 2);
			 }
			 if (i != 0) return (MDAS_AD_COLLMDATA_DELETION_ERROR);

			 break; 

    case C_CHANGE_USER_DEFINED_COLL_INTEGER_META_DATA:  
                         strcpy(mdata_attrnum,"");
			 strcpy(mdata_num,"");
                         splitbychar(data_value_1, mdata_attrnum, mdata_num,'@');
                         strcpy(reg_action_name,"change user defined integer meta data for collection");			 
			 if (strlen(mdata_attrnum) == 0 || 
			     strlen(mdata_num) == 0 ||
			     atoi(mdata_attrnum) < 0 || 
			     atoi(mdata_num) < 0 ) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(udmdName,"userdef_metaint%i",
				 atoi(mdata_attrnum));
#ifdef FED_MCAT
			 sprintf(atname[0],"%s",udmdName);
			 sprintf(atname[1],"userdef_modif_date");
                         sprintf(atname[2],"data_grp_id");
			 sprintf(atname[3],"metadatanum");
			 sprintf(atval[0],"%i",atoi(data_value_2));
			 sprintf(atval[1],"'%s'",timeval);
			 sprintf(atval[2],"'%s'",data_group_id);
			 sprintf(atval[3],"%i",atoi(mdata_num));
                         i = changeAttrValueSet(atname, atval,"MDAS_AD_COLLMDATA", 4,2);
#else
			 sprintf(atname[0],"%s",udmdName);
                         sprintf(atname[1],"data_grp_id");
			 sprintf(atname[2],"metadatanum");
			 sprintf(atval[0],"%i",atoi(data_value_2));
			 sprintf(atval[1],"'%s'",data_group_id);
			 sprintf(atval[2],"%i",atoi(mdata_num));
                         i = changeAttrValue(atname, atval,"MDAS_AD_COLLMDATA", 3);
#endif
			 if (i != 0) return (MDAS_AD_COLLMDATA_UPDATE_ERROR);

			 break; 
    case C_INSERT_USER_DEFINED_COLL_INTEGER_META_DATA:  
                         strcpy(reg_action_name,"insert user defined integer meta data for collection");
      		         sprintf(sqlq,"select data_grp_id from %sMDAS_AD_COLLMDATA where  data_grp_id = '%s' ", MDASSCHEMENAME, data_group_id);
			 if (check_exists(sqlq) == 0) {
			   sprintf(sqlq,"select max(metadatanum) from  %sMDAS_AD_COLLMDATA where data_grp_id =  '%s' ", MDASSCHEMENAME, data_group_id);
			   mdata_maxnum = get_itype_value_from_query(sqlq) ;
			   if (mdata_maxnum < 0) {
                             if (mdata_maxnum != -1)
			       return (mdata_maxnum);
			     sprintf(sqlq,"delete from %sMDAS_AD_COLLMDATA where data_grp_id =  '%s' and metadatanum = -1",
                                     MDASSCHEMENAME, data_group_id);
                             ExecSqlDb2(henv, hdbc, sqlq);
                           }
			   mdata_maxnum++;
			 }
			 else {
			   mdata_maxnum =  0;
			 }
			 if (atoi(data_value_1) < 0) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(udmdName,"userdef_metaint%i",
				 atoi(data_value_1));
			 sprintf(atname[0],"%s",udmdName);
                         sprintf(atname[1],"data_grp_id");
			 sprintf(atname[2],"metadatanum");
			 sprintf(atname[3],"userdef_modif_date");
			 sprintf(atname[4],"userdef_creat_date");
			 sprintf(atval[0],"%i",atoi(data_value_2));
			 sprintf(atval[1],"'%s'",data_group_id);
			 sprintf(atval[2],"%i",mdata_maxnum);
			 sprintf(atval[3],"'%s'",timeval);
			 sprintf(atval[4],"'%s'",timeval);

#ifdef FED_MCAT
                         i=insertIntoTable(atname,atval,"MDAS_AD_COLLMDATA",5);
#else
                         i=insertIntoTable(atname,atval,"MDAS_AD_COLLMDATA",3);
#endif
			 if (i != 0) return (MDAS_AD_COLLMDATA_INSERTION_ERROR);

			 return(mdata_maxnum);
			 break; 
    case C_DELETE_USER_DEFINED_COLL_INTEGER_META_DATA:  
                         strcpy(reg_action_name,"delete user defined integer meta data for collection");
			 if (atoi(data_value_1) < 0) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
                         sprintf(atname[0],"data_grp_id");
			 sprintf(atname[1],"metadatanum");
			 sprintf(atval[0],"'%s'",data_group_id);
			 sprintf(atval[1],"%i",atoi(data_value_1));

                         i = deleteFromTable(atname, atval,"MDAS_AD_COLLMDATA", 2);
			 if (i != 0) return (MDAS_AD_COLLMDATA_DELETION_ERROR);

			 break; 
    case C_DELETE_ATTR:
                         strcpy(reg_action_name,"delete comments");

                         sprintf(atname[0],"data_grp_id");
			 sprintf(atval[0],"'%s'",data_group_id);

			 
                         i=deleteFromTable(atname,atval,
					   dcs_tname[atoi(data_value_2)],1);
			 if (i != 0) return (MDAS_AD_COLLMDATA_DELETION_ERROR);
			 break;
    case C_INSERT_ATTR:
			 strcpy(reg_action_name,"insert comments");
			 sprintf(sqlq,"select * from %s where data_grp_id = '%s'",
				  dcs_tname[atoi(data_value_2)],
				 data_group_id);
			 if (check_exists(sqlq) != 0) {
                         sprintf(atname[0],"data_grp_id");
			   sprintf(atname[1],dcs_aname[atoi(data_value_2)]);
			 sprintf(atval[0],"'%s'",data_group_id);
			   sprintf(atval[1],"%s",data_value_1);
			   i = insertIntoTable(atname, atval,
					     dcs_tname[atoi(data_value_2)], 2);
			   if (i != 0) {
			     sprintf(atval[1],"'%s'",data_value_1);
			     i = insertIntoTable(atname, atval,
					     dcs_tname[atoi(data_value_2)],2);
			     if (i != 0) 
			       return (MDAS_AD_COLLMDATA_INSERTION_ERROR);
			   }
			 }
			 else {
			   sprintf(atname[0],dcs_aname[atoi(data_value_2)]);
			   sprintf(atname[1],"data_grp_id");
			   sprintf(atval[0],"%s",data_value_1);
			   sprintf(atval[1],"'%s'",data_group_id);
			   i = changeAttrValue(atname, atval,
					      dcs_tname[atoi(data_value_2)],2);
			   if (i != 0) {
			     sprintf(atval[0],"'%s'",data_value_1);
			     i = changeAttrValue(atname, atval,
					      dcs_tname[atoi(data_value_2)],2);
			     if (i != 0)
			       return (MDAS_AD_COLLMDATA_UPDATE_ERROR);
			   }
			 }

			 break;
    case C_MODIFY_ATTR:
			 strcpy(reg_action_name,"update comments");
			 sprintf(atname[0],dcs_aname[atoi(data_value_2)]);
			 sprintf(atname[1],"data_grp_id");
			 sprintf(atval[0],"%s",data_value_1);
			 sprintf(atval[1],"'%s'",data_group_id);
			 i = changeAttrValue(atname, atval,
					     dcs_tname[atoi(data_value_2)],2);
			 if (i != 0) {
			   sprintf(atval[0],"'%s'",data_value_1);
			   i = changeAttrValue(atname, atval,
					     dcs_tname[atoi(data_value_2)],2);
			   if (i != 0)
			     return (MDAS_AD_COLLMDATA_UPDATE_ERROR);
			 }
			 break;
    case C_DELETE_MULTI_ATTR:
                         strcpy(reg_action_name,"delete comments");
                         strcpy(mdata_attrnum,"");
			 strcpy(mdata_num,"");
                         splitbychar(data_value_2, 
				     mdata_attrnum, mdata_num,'@');
			 /*row@table*/
			 if (strlen(mdata_attrnum) == 0 || 
			     atoi(mdata_attrnum) < 0 ) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(atname[0],"metadatanum");
			 sprintf(atval[0],"%i",atoi(mdata_attrnum));
                         sprintf(atname[1],"data_grp_id");
			 sprintf(atval[1],"'%s'",data_group_id);
                         i=deleteFromTable(atname,atval,
					   dcs_tname[atoi(mdata_num)],2);
			 if (i != 0) return (MDAS_AD_COLLMDATA_DELETION_ERROR);

			 break;
    case C_INSERT_MULTI_ATTR:  
                         strcpy(reg_action_name,"insert comments");
      		         sprintf(sqlq,"select * from %s where data_grp_id = '%s'",
				 dcs_tname[atoi(data_value_2)],
				 data_group_id);
			 if (check_exists(sqlq) == 0) {
			   sprintf(sqlq,"select max(metadatanum) from  %s where data_grp_id =  '%s' ",  dcs_tname[atoi(data_value_2)],data_group_id);
			   mdata_maxnum = get_itype_value_from_query(sqlq) ;
			   if (mdata_maxnum < 0) {
			     if (mdata_maxnum != -1)
			       return (mdata_maxnum);
			     sprintf(sqlq,"delete from %s where data_grp_id =  '%s'  and metadatanum = -1",
				     dcs_tname[atoi(data_value_2)],data_group_id);
			     ExecSqlDb2(henv, hdbc, sqlq);
                           }
			   mdata_maxnum++;
			 }
			 else {
			   mdata_maxnum =  1;
			 }
			 sprintf(atname[0],"data_grp_id");
			 sprintf(atname[1],dcs_aname[atoi(data_value_2)]);
			 sprintf(atname[2],"metadatanum");
			 sprintf(atval[0],"'%s'",data_group_id);
			 sprintf(atval[1],"%s",data_value_1);
			 sprintf(atval[2],"%i",mdata_maxnum);
			 i = insertIntoTable(atname, atval,
					     dcs_tname[atoi(data_value_2)], 3);
			 if (i != 0) {
			   sprintf(atval[1],"'%s'",data_value_1);
			   i = insertIntoTable(atname, atval,
					     dcs_tname[atoi(data_value_2)],3);
			   if (i != 0) 
			     return (MDAS_AD_COLLMDATA_INSERTION_ERROR);
			 }
			 return(mdata_maxnum);
			 break; 

    case C_MODIFY_MULTI_ATTR:
                         strcpy(reg_action_name,"update comments");
                         strcpy(mdata_attrnum,"");
			 strcpy(mdata_num,"");
                         splitbychar(data_value_2, 
				     mdata_attrnum, mdata_num,'@');
			 /*row@table*/
			 if (strlen(mdata_attrnum) == 0 || 
			     strlen(mdata_num) == 0 ||
			     atoi(mdata_attrnum) <= 0 || 
			     atoi(mdata_num) <= 0 ) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(atname[0],dcs_aname[atoi(mdata_num)]);
                         sprintf(atname[1],"data_grp_id");
			 sprintf(atname[2],"metadatanum");
			 sprintf(atval[0],"%s",data_value_1);
			 sprintf(atval[1],"'%s'",data_group_id);
			 sprintf(atval[2],"%i",atoi(mdata_attrnum));
                         i = changeAttrValue(atname, atval,
					     dcs_tname[atoi(mdata_num)], 3);
			 
			 if (i != 0) {
			   sprintf(atval[0],"'%s'",data_value_1);
			   i = changeAttrValue(atname, atval,
					     dcs_tname[atoi(mdata_num)], 3);
			   if (i != 0) return (MDAS_AD_COLLMDATA_UPDATE_ERROR);
			 }

			 break; 
    case C_COPY_META_DATA_FROM_DATA_TO_COLL:
                         strcpy(reg_action_name,"insert user defined string meta data");
      		         sprintf(sqlq,"select data_grp_id from %sMDAS_AD_COLLMDATA where  data_grp_id = '%s' ", MDASSCHEMENAME, data_group_id);
			 if (check_exists(sqlq) == 0) {
			   sprintf(sqlq,"select max(metadatanum) from  %sMDAS_AD_COLLMDATA where data_grp_id =  '%s' ", MDASSCHEMENAME, data_group_id);
			   mdata_maxnum = get_itype_value_from_query(sqlq) ;
			   if (mdata_maxnum < 0) {
                             if (mdata_maxnum != -1)
			       return (mdata_maxnum);
			     sprintf(sqlq,"delete from %sMDAS_AD_COLLMDATA where data_grp_id =  '%s' and metadatanum = -1",
                                     MDASSCHEMENAME, data_group_id);
                             ExecSqlDb2(henv, hdbc, sqlq);
                           }
			   mdata_maxnum++;
			 }
			 else {
			   mdata_maxnum =  0;
			 }
			 sprintf(srcCollPath,"%s/%s",data_value_1,data_value_2);
			 newObjIntId = getObjIdFromCollPathForUserIdAccs(
					     srcCollPath, obj_user_id, "read");
			 if (newObjIntId < 0) return(newObjIntId);
#ifdef FED_MCAT
			 sprintf(sqlq, "insert into %sMDAS_AD_COLLMDATA \
(data_grp_id, metadatanum,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1,userdef_creat_date,userdef_modif_date )  \
select '%s', metadatanum + %i,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1,userdef_creat_date,userdef_modif_date from %sMDAS_AD_MDATA where data_id = %i",
				   MDASSCHEMENAME, data_group_id, mdata_maxnum,
				   MDASSCHEMENAME,newObjIntId );
#else
			 sprintf(sqlq, "insert into %sMDAS_AD_COLLMDATA \
(data_grp_id, metadatanum,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1 )  \
select '%s', metadatanum + %i,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1 from %sMDAS_AD_MDATA where data_id = %i",
				   MDASSCHEMENAME, data_group_id, mdata_maxnum,
				   MDASSCHEMENAME,newObjIntId );
#endif
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0) 		      
			   return(MDAS_AD_COLLMDATA_INSERTION_ERROR);
			 break; 
    case C_COPY_META_DATA_FROM_COLL_TO_COLL:
                         strcpy(reg_action_name,"insert user defined string meta data");
      		         sprintf(sqlq,"select data_grp_id from %sMDAS_AD_COLLMDATA where  data_grp_id = '%s' ", MDASSCHEMENAME, data_group_id);
			 if (check_exists(sqlq) == 0) {
			   sprintf(sqlq,"select max(metadatanum) from  %sMDAS_AD_COLLMDATA where data_grp_id =  '%s' ", MDASSCHEMENAME, data_group_id);
			   mdata_maxnum = get_itype_value_from_query(sqlq) ;
			   if (mdata_maxnum < 0) {
                             if (mdata_maxnum != -1)
			       return (mdata_maxnum);
			     sprintf(sqlq,"delete from %sMDAS_AD_COLLMDATA where data_grp_id =  '%s' and metadatanum = -1",
                                     MDASSCHEMENAME, data_group_id);
                             ExecSqlDb2(henv, hdbc, sqlq);
                           }
			   mdata_maxnum++;
			 }
			 else {
			   mdata_maxnum =  0;
			 }
			 i = getCollIdFromCollPathForUserIdAccs(
				   srcCollId, data_value_1, obj_user_id, "read");
			 if (i < 0) return(i);
#ifdef FED_MCAT
		 sprintf(sqlq, "insert into %sMDAS_AD_COLLMDATA \
(data_grp_id, metadatanum,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1,userdef_creat_date,userdef_modif_date  )  \
select '%s', metadatanum + %i,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1,userdef_creat_date,userdef_modif_date  from %sMDAS_AD_COLLMDATA where data_grp_id = '%s'",
				   MDASSCHEMENAME, data_group_id, mdata_maxnum,
				   MDASSCHEMENAME,srcCollId );
#else
		 sprintf(sqlq, "insert into %sMDAS_AD_COLLMDATA \
(data_grp_id, metadatanum,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1 )  \
select '%s', metadatanum + %i,userdef_metastr0,userdef_metastr1,userdef_metastr2,\
userdef_metastr3,userdef_metastr4,userdef_metastr5,userdef_metastr6,\
userdef_metastr7,userdef_metastr8,userdef_metastr9, \
userdef_metaint0,userdef_metaint1 from %sMDAS_AD_COLLMDATA where data_grp_id = '%s'",
				   MDASSCHEMENAME, data_group_id, mdata_maxnum,
				   MDASSCHEMENAME,srcCollId );
#endif
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0) 		      
			   return(MDAS_AD_COLLMDATA_INSERTION_ERROR);

			 break; 
    case C_DELETE_ANNOTATIONS:
                         strcpy(reg_action_name,"delete annotations");
                         sprintf(atname[0],"data_grp_id");
                         sprintf(atname[1],"user_id");
                         sprintf(atname[2],"anno_date");
			 sprintf(atval[0],"'%s'",data_group_id);
			 sprintf(atval[1],"%i",obj_user_id);
			 sprintf(atval[2],"'%s'",data_value_1);
                         i=deleteFromTable(atname,atval,"MDAS_AC_ANNOTATION",3);
			 if (i != 0) return(MDAS_AC_ANNOTATION_DELETION_ERROR);
			 break;
    case C_INSERT_ANNOTATIONS:
                         strcpy(reg_action_name,"insert annotations");
                         sprintf(atname[0],"data_grp_id");
                         sprintf(atname[1],"annotations");
                         sprintf(atname[2],"anno_date");
                         sprintf(atname[3],"user_id");
			 sprintf(atname[4],"anno_type");
			 sprintf(atval[0],"'%s'",data_group_id);
			 sprintf(atval[1],"'%s'",data_value_1);
			 sprintf(atval[2],"'%s'",timeval);
			 sprintf(atval[3],"%i",obj_user_id);
			 sprintf(atval[4],"'%s'",data_value_2);
			 i = insertIntoTable(atname, atval,"MDAS_AC_ANNOTATION",5);
			 if (i != 0) return(MDAS_AC_ANNOTATION_INSERTION_ERROR);
			 break;
    case C_UPDATE_ANNOTATIONS:
                         strcpy(reg_action_name,"update annotations");
			 tmpPtr = strstr(data_value_1,"@@@");
			 if (tmpPtr != NULL) *tmpPtr='\0';
                         sprintf(atname[0],"annotations");
                         sprintf(atname[1],"user_id");
                         sprintf(atname[2],"data_grp_id");
			 sprintf(atname[3],"anno_date");
			 sprintf(atval[0],"'%s'",data_value_1);
			 sprintf(atval[1],"%i",obj_user_id);
			 sprintf(atval[2],"'%s'",data_group_id);
			 sprintf(atval[3],"'%s'",data_value_2);
			 i = changeAttrValue(atname, atval,"MDAS_AC_ANNOTATION",4);
			 if (i != 0) return (MDAS_AD_ANNOTATION_UPDATE_ERROR);
			 if (tmpPtr != NULL) {
			   tmpPtr +=3;
			   sprintf(atname[0],"anno_type");
			   sprintf(atval[0],"'%s'",tmpPtr);
			   i = changeAttrValue(atname, atval,"MDAS_AC_ANNOTATION",4);
			   if (i != 0) return (MDAS_AC_ANNOTATION_UPDATE_ERROR);
			 }


			 break;
#ifdef FED_MCAT
    case C_CHANGE_MODIFY_TIMESTAMP:
                         strcpy(reg_action_name,"modify coll timestamp");
                         sprintf(atname[0],"coll_mod_timestamp");
                         sprintf(atname[1],"data_grp_id");
			 sprintf(atval[0],"'%s'",data_value_1);
			 sprintf(atval[1],"'%s'",data_group_id);
			 i = changeAttrValue(atname, atval,"MDAS_TD_DATA_GRP",2);
			 if (i != 0) 
			   return (MDAS_TD_DATA_GRP_UPDATE_ERROR);
			 break;
#endif
    case C_CHANGE_ACL_INHERITANCE_BIT:         
                         sprintf(atname[0],"coll_link");
			 sprintf(atname[1],"data_grp_id");
                         sprintf(atval[0],"%i",atoi(data_value_1));
			 sprintf(atval[1],"'%s'",data_group_id);
                         i = changeAttrValue(atname, atval,"MDAS_TD_DATA_GRP",2);
                         if (i != 0)
                           return (MDAS_TD_DATA_GRP_UPDATE_ERROR);
                         break;
    case C_CHANGE_ACL_INHERITANCE_BIT_RECUR:
                         sprintf(sqlq,"update %sMDAS_TD_DATA_GRP set coll_link = %i where  data_grp_id like '%s%%' and  coll_owner = %i",
				 MDASSCHEMENAME,atoi(data_value_1),data_group_id,obj_user_id);
			 i = ExecSqlDb2(henv, hdbc, sqlq);
			 if (i != 0)
			   return(MDAS_TD_DATA_GRP_UPDATE_ERROR);
			 break;
    case C_CHANGE_COLL_OWNER:
                         strcpy(reg_action_name,"change coll owner");
			 intval = get_user_id_in_domain(
					     data_value_1, data_value_2);
			 if (intval < 0) { return (USER_NOT_IN_DOMN);}
			 sprintf(atname[0],"coll_owner");
                         sprintf(atname[1],"data_grp_id");
			 sprintf(atval[0],"%i",intval);
			 sprintf(atval[1],"'%s'",data_group_id);
			 i = changeAttrValue(atname, atval,"MDAS_TD_DATA_GRP",2);
			 if (i != 0) 
			   return (MDAS_TD_DATA_GRP_UPDATE_ERROR);
			 break;
    case C_DELETE_USER_DEFINED_ATTR_VAL_META_DATA:
                         strcpy(reg_action_name,"delete user defined string meta data");
                         sprintf(atname[0],"data_grp_id");
			 sprintf(atname[1],"userdef_metastr0");
			 sprintf(atname[2],"userdef_metastr1");
			 sprintf(atval[0],"'%s'",data_group_id);
			 sprintf(atval[1],"'%s'",data_value_1);
			 sprintf(atval[2],"'%s'",data_value_2);
			 i = deleteFromTable(atname,atval,"MDAS_AD_COLLMDATA",3);
			 if (i != 0) return (MDAS_AD_COLLMDATA_DELETION_ERROR);
			 break; 
      default:
                         return (MDAS_CATALOG_MODIFY_TYPE_ERROR);
			 break;


    }
   removeTabledQueries(group_name);
   removeTabledQueries(data_group_id);

  return(MDAS_SUCCESS);

}		 



int modify_user_info(int cat_type, char *obj_user_name,
			 char *data_value_1, char *data_value_2,
			 int retraction_type, char *user_password,
		     char *user_domain_name)
{ 

  int    replication_num, i;
  int    obj_user_id, obj_action_id;
  char  *newUserDomain, *newUserName, *distinName;
  char tmpStr[MAX_TOKEN];
  char tmpStr2[MAX_TOKEN]; 
  char *tmpPtr;

  char mdata_attrnum[MAX_TOKEN], mdata_num[MAX_TOKEN];
  int mdata_maxnum;
  char udmdName[MAX_TOKEN];
  char data_value_encoded[MAX_TOKEN];


  newUserName = obj_user_name;
  /* printf("%s:%s:%s:%s:%s:%i\n", obj_user_name,user_password,user_domain_name,data_value_1,data_value_2,retraction_type);*/
   strcpy(reg_obj_code,"MDAS_CD_USER");
   strcpy(reg_registrar_name,obj_user_name);
   strcpy(reg_obj_name, data_value_1);
   strcpy(reg_action_name,"modify user");
    failure = 0;

/***** June 2.2000 changed so that password is not
      checked since it is already being done at a higher level
      
   if (retraction_type == U_CHANGE_PASSWORD || 
       retraction_type == U_DELETE_AUTH_MAP || 
       retraction_type == U_INSERT_AUTH_MAP || 
       retraction_type == U_CHANGE_VERKEY || 
       retraction_type == U_CHANGE_ENCKEY )
     {

       i = web_mdas_sys_authorization(obj_user_name, 
					  user_password, user_domain_name);
       if (i == 0) {
	 strcpy(reg_registrar_name,obj_user_name);
	 strcpy(reg_obj_name, data_value_1);
	 reg_registrar_id = get_user_id_in_domain(obj_user_name, 
						  user_domain_name);
	 if (reg_registrar_id < 0 )
	   {  
	     return (USER_NOT_IN_DOMN);
	   }
	 if (strlen(data_value_2) == 0) {
	   obj_user_id = reg_registrar_id;
	 }
	 else {
	   newUserDomain = strstr(data_value_1,"@");
	   *newUserDomain = '\0';
	   newUserDomain++;       
	   obj_user_id = get_user_id_in_domain(data_value_1, newUserDomain);
	   if (obj_user_id < 0) { return (USER_NOT_IN_DOMN);}
	 }
       }
       else {
	 i = web_mdas_authorization(obj_user_name, user_password, 
				      user_domain_name);
	 if ( i != 0)    
	   {
	     sprintf(query_string,
		     "user modify API: Failed Password System:%s,%s,%s\n",
		     user_password,obj_user_name, user_domain_name);
	     return(USER_AUTHORIZATION_ERROR);
	   }
	 strcpy(reg_registrar_name,obj_user_name);
	 strcpy(reg_obj_name,obj_user_name);
       
	 obj_user_id = get_user_id_in_domain(obj_user_name, user_domain_name);
	 if (obj_user_id < 0) { return (USER_NOT_IN_DOMN);}
	 
	 reg_registrar_id = obj_user_id;
	 reg_obj_id =  obj_user_id;
       }
     }
   else
     {
       
       i = web_mdas_sys_authorization(obj_user_name, user_password, user_domain_name);
       if ( i != 0)    
	 {
	   sprintf(query_string,"user modify API: Failed Password System:%s,%s,%s\n",
		   user_password,obj_user_name, user_domain_name);
	   return(SYS_USER_AUTHORIZATION_ERROR);
	 }
       strcpy(reg_registrar_name,obj_user_name);
       strcpy(reg_obj_name, data_value_1);
       reg_registrar_id = get_user_id_in_domain(obj_user_name, 
						user_domain_name);
       if (reg_registrar_id < 0 )



	 {  
	   return (USER_NOT_IN_DOMN);
	 }
       newUserDomain = strstr(data_value_1,"@");
       *newUserDomain = '\0';
       newUserName = data_value_1;
       newUserDomain++;       
       obj_user_id = get_user_id_in_domain(data_value_1, newUserDomain);
       if (obj_user_id < 0) { return (USER_NOT_IN_DOMN);}
     }
*******/

    if (retraction_type == U_CHANGE_PASSWORD || 
	retraction_type == U_BULK_INSERT_UDEF_META_DATA_FOR_USER) { 
      obj_user_id = get_user_id_in_domain(obj_user_name,user_domain_name);
      if (obj_user_id < 0) { return (USER_NOT_IN_DOMN);}
      reg_registrar_id = obj_user_id;
    }
    else {
      newUserDomain = strstr(data_value_1,"@");
      if (newUserDomain == NULL) return(USER_NOT_IN_DOMN);
      *newUserDomain = '\0';
      newUserDomain++;       
      intval = -1;
      if (retraction_type == U_INSERT_GROUP ||
	  retraction_type == U_DELETE_GROUP ||
	  retraction_type == U_ADD_GROUP_OWNER ||
	  retraction_type == U_REMOVE_GROUP_OWNER ) {
#ifdef FED_MCAT
	tmpPtr =NULL;
	if ((tmpPtr = strstr(data_value_2,"|")) == NULL)
	  intval = get_user_id_in_domain(data_value_2, "groups");
	else {
	  *tmpPtr = '\0';
	  sprintf(tmpStr,"groups|%s",tmpPtr + 1);
	  intval = get_user_id_in_domain(data_value_2, tmpStr);
	}
#else
	intval = get_user_id_in_domain(data_value_2, "groups");
#endif
	if (intval < 0) { return (USER_GROUP_NOT_IN_CAT);}
      }
      if (isUserDomainAdminFlag == 1  && intval > 0  )
	i = 0;
      else {
	i = isUserSysAdmin(obj_user_name,user_domain_name ); 
	if (i != 0 && strcmp(data_value_2,"sysadmin") &&
	    !strcmp( newUserDomain,user_domain_name) && intval < 0) { 
	  i = isUserDomainAdmin(obj_user_name, user_domain_name ); 
	}
	if ( i != 0 && intval >= 0) {
	  sprintf(sqlq,"select user_id from %s%s where user_id = %i and user_address like '%%<groupowner=%s@%s>%%'", 
		  MDASSCHEMENAME,"MDAS_AU_INFO",intval,obj_user_name,user_domain_name); 
	  if (check_exists(sqlq) != 0) {
	    sprintf(query_string,"user modify API: Failed for %i :%s,%s\n", 
		    retraction_type, obj_user_name,user_domain_name); 
	    return(SYS_USER_AUTHORIZATION_ERROR); 
	  } 
	}
	else if (i != 0) {
	  sprintf(query_string,"user modify API: Failed for %i :%s,%s\n",
                    retraction_type, obj_user_name,user_domain_name);
            return(SYS_USER_AUTHORIZATION_ERROR);
	}
      }
      i = 0;
      failure = 0;
      reg_registrar_id = get_user_id_in_domain(obj_user_name,user_domain_name);
      if (reg_registrar_id < 0 )  {   return (USER_NOT_IN_DOMN);  }
      obj_user_id = get_user_id_in_domain(data_value_1, newUserDomain);
      if (obj_user_id < 0) { return (USER_NOT_IN_DOMN);}
    }
    
    reg_obj_id =  obj_user_id;
    
    
    get_time(timeval);
    if (failure != 0 )
      return (GET_TIME_ERROR);

   reg_obj_repl_enum = 0;
   strcpy(reg_timeval,timeval);

   switch (retraction_type)
    {
    case U_DELETE:
                         strcpy(reg_action_name,"drop user");
			 tabledIsOkay = 1;
                         get_ctype_value(chval,"MDAS_TD_USER_TYP.user_typ_name",
					 "deleted");
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (USER_TYPE_NOT_IN_CAT);
			 }				 
			 sprintf(atname[0],"user_name");
                         sprintf(atname[1],"user_id");
			 sprintf(atval[0],"'%s$deleted'",newUserName);
			 sprintf(atval[1],"%i",obj_user_id);
			 i = changeAttrValue(atname, atval,"MDAS_CD_USER",2);
			 if (i != 0) return (MDAS_CD_USER_UPDATE_ERROR);
                         sprintf(atname[0],"user_typ_id");
			 sprintf(atval[0],"'%s'",chval);
			 i = changeAttrValue(atname, atval,"MDAS_CD_USER",2);
			 if (i != 0) return (MDAS_CD_USER_UPDATE_ERROR);
			 removeTabledQueries(newUserName);
			 sprintf(tmpStr,"%i",obj_user_id);
			 removeTabledQueries(tmpStr);

                         break;
    case U_DELETE_DOMN:
                         strcpy(reg_action_name,"delete domain");
			 tabledIsOkay = 1;
                         get_ctype_value(chval, "MDAS_TD_DOMN.domain_desc", 
					 data_value_2);
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (MDAS_TD_DOMN_ACCESS_ERROR);
			 }
                         sprintf(atname[0],"user_id");
			 sprintf(atname[1],"domain_id");
			 sprintf(atval[0],"%i",obj_user_id);
			 sprintf(atval[1],"'%s'",chval);
                         i = deleteFromTable(atname, atval,"MDAS_AU_DOMN",2);
			 if (i != 0) return (MDAS_AU_DOMN_DELETION_ERROR);
			 removeTabledQueries(newUserName);
			 sprintf(tmpStr,"%i",obj_user_id);
			 removeTabledQueries(tmpStr);

			 break;
    case U_INSERT_DOMN:
                         strcpy(reg_action_name,"insert domain");

			 i = check_unique_name_in_domain("USER", data_value_1,
					     data_value_2);
			 if (i != 0)   {
			   sprintf(query_string,
                             "user modify API: Non-unique User Name:%s\n",
				   data_value_1);
			   return(USER_NOT_UNIQUE_IN_DOMN);
			 }
			 tabledIsOkay = 1;
                         get_ctype_value(chval, "MDAS_TD_DOMN.domain_desc", 
					 data_value_2);
			 tabledIsOkay = 0;			 
			 if (failure != 0) {
			   return (MDAS_TD_DOMN_ACCESS_ERROR);
			 }
			 sprintf(sqlq,"select * from  %sMDAS_AU_DOMN where user_id = %i and domain_id = '%s'", MDASSCHEMENAME,obj_user_id,chval);
			 if (check_exists(sqlq) == 0) 
			   return(USER_IN_DOMAIN_ALREADY_IN_CAT);
                         sprintf(atname[0],"user_id");
			 sprintf(atname[1],"domain_id");
			 sprintf(atval[0],"%i",obj_user_id);
			 sprintf(atval[1],"'%s'",chval);
                         i = insertIntoTable(atname, atval,"MDAS_AU_DOMN",2);
			 if (i != 0) return (MDAS_AU_DOMN_INSERTION_ERROR);

			 break;
     case U_DELETE_GROUP:
                         strcpy(reg_action_name,"delete group");
                         sprintf(atname[0],"user_id");
			 sprintf(atname[1],"group_user_id");
			 sprintf(atval[0],"%i",obj_user_id);
			 sprintf(atval[1],"%i",intval);
                         i = deleteFromTable(atname, atval,"MDAS_AU_GROUP",2);
			 if (i != 0) return (MDAS_AU_GROUP_DELETION_ERROR);

			 break;
    case U_INSERT_GROUP:
                         strcpy(reg_action_name,"insert group");
			 if (intval < 0) { return (USER_GROUP_NOT_IN_CAT);}
			 sprintf(sqlq,"select * from  %sMDAS_AU_GROUP where user_id = %i and group_user_id = %i", MDASSCHEMENAME,obj_user_id,intval);
			 if (check_exists(sqlq) == 0) 
			   return(USER_IN_GROUP_ALREADY_IN_CAT);
                         sprintf(atname[0],"user_id");
			 sprintf(atname[1],"group_user_id");
			 sprintf(atval[0],"%i",obj_user_id);
			 sprintf(atval[1],"%i",intval);
                         i = insertIntoTable(atname, atval,"MDAS_AU_GROUP",2);
			 if (i != 0) return (MDAS_AU_GROUP_INSERTION_ERROR);

			 break;
    case U_UPDATE_ADDRESS:
                         sprintf(atname[0],"user_address");
                         sprintf(atname[1],"user_id");
                         sprintf(atval[0],"'%s'",data_value_2);
                         sprintf(atval[1],"%i",obj_user_id);
                         i = changeAttrValue(atname, atval,"MDAS_AU_INFO", 2);
                         if (i != 0) return (MDAS_AU_INFO_UPDATE_ERROR);
                         break; 
    case U_UPDATE_EMAIL:
                         sprintf(atname[0],"user_email");
                         sprintf(atname[1],"user_id");
                         sprintf(atval[0],"'%s'",data_value_2);
                         sprintf(atval[1],"%i",obj_user_id);
                         i = changeAttrValue(atname, atval,"MDAS_AU_INFO", 2);
                         if (i != 0) return (MDAS_AU_INFO_UPDATE_ERROR);
                         break;
    case U_UPDATE_PHONE:
                         sprintf(atname[0],"user_phone");
                         sprintf(atname[1],"user_id");
                         sprintf(atval[0],"'%s'",data_value_2);
                         sprintf(atval[1],"%i",obj_user_id);
                         i = changeAttrValue(atname, atval,"MDAS_AU_INFO", 2);
                         if (i != 0) return (MDAS_AU_INFO_UPDATE_ERROR);
                         break;

    case U_ADD_GROUP_OWNER:
                         sprintf(sqlq,
				 "select user_address from %sMDAS_AU_INFO where user_id = %i",
				 MDASSCHEMENAME,intval); 
			 failure = 0;
			 get_ctype_value_from_query(tmpStr,sqlq);
			 if (failure != 0)  
			   return(ACTION_NOT_ALLOWED); 
			 sprintf(atname[0],"user_address");
			 sprintf(atname[1],"user_id"); 
			 sprintf(atval[0],"'%s<groupowner=%s@%s>'",
				 tmpStr,data_value_1, newUserDomain);
			 sprintf(atval[1],"%i",intval); 
			 i = changeAttrValue(atname, atval,"MDAS_AU_INFO", 2); 
                         if (i != 0) return (MDAS_AU_INFO_UPDATE_ERROR);
			 break;
    case U_REMOVE_GROUP_OWNER: 
                         sprintf(sqlq, 
				 "select user_address from %sMDAS_AU_INFO where user_id = %i", 
				 MDASSCHEMENAME,intval);  
			 failure = 0; 
			 get_ctype_value_from_query(tmpStr,sqlq); 
			 if (failure != 0)   
			   return(ACTION_NOT_ALLOWED);  
			 sprintf(tmpStr2,"<groupowner=%s@%s>",data_value_1, newUserDomain); 
			 elog(NOTICE,"PPPPP:tmpStr=%s,tmpStr2=%s\n",tmpStr,tmpStr2);
			 if ((tmpPtr = strstr(tmpStr,tmpStr2)) == NULL)
			   return(USER_NAME_NOT_FOUND);
			 *tmpPtr = '\0';
			 tmpPtr = tmpPtr + strlen(tmpStr2);
			 strcat(tmpStr,tmpPtr);
			 sprintf(atname[0],"user_address"); 
                         sprintf(atname[1],"user_id");  
                         sprintf(atval[0],"'%s'",tmpStr);
                         sprintf(atval[1],"%i",intval);  
                         i = changeAttrValue(atname, atval,"MDAS_AU_INFO", 2);  
                         if (i != 0) return (MDAS_AU_INFO_UPDATE_ERROR); 
                         break; 
    case U_CHANGE_TYPE:         
                         strcpy(reg_action_name,"change type");
			 tabledIsOkay = 1;
		         get_ctype_value(chval,"MDAS_TD_USER_TYP.user_typ_name",
					 data_value_2);
			 tabledIsOkay = 0;
			 if (failure != 0) {
			   return (USER_TYPE_NOT_IN_CAT);
			 }
			 sprintf(atname[0],"user_typ_id");
                         sprintf(atname[1],"user_id");
			 sprintf(atval[0],"'%s'",chval);
			 sprintf(atval[1],"%i",obj_user_id);
                         i = changeAttrValue(atname, atval,"MDAS_CD_USER", 2);
			 if (i != 0) return (MDAS_CD_USER_UPDATE_ERROR);

			 break;

    case U_CHANGE_PASSWORD:
                         strcpy(reg_action_name,"change password");
			 sprintf(atname[0],"priv_key");
			 sprintf(atname[1],"user_id");
			 obf2Encode(data_value_1, data_value_encoded, 
				    MAX_TOKEN, OBF2_EXTRA_KEY);
			 sprintf(atval[0],"'%s'",data_value_encoded);
			 sprintf(atval[1],"%i",obj_user_id);
			 i = changeAttrValue(atname, atval,"MDAS_AU_AUTH_KEY",
					     2);
			  if (i != 0) return (MDAS_AU_AUTH_KEY_UPDATE_ERROR);

			 break;
    case SU_CHANGE_PASSWORD:
                         strcpy(reg_action_name,"change password");
                         sprintf(atname[0],"priv_key");
                         sprintf(atname[1],"user_id");
			 obf2Encode(data_value_2, data_value_encoded, 
				    MAX_TOKEN, OBF2_EXTRA_KEY);
                         sprintf(atval[0],"'%s'",data_value_encoded);
                         sprintf(atval[1],"%i",obj_user_id);
                         i = changeAttrValue(atname, atval,"MDAS_AU_AUTH_KEY",
                                             2);
                          if (i != 0) return (MDAS_AU_AUTH_KEY_UPDATE_ERROR);
 
                         break;
    case U_DELETE_AUTH_MAP:
                         strcpy(reg_action_name,"delete authorization mapping");
			 distinName = strstr(data_value_2,":");
			 *distinName = '\0';
			 distinName++;       
			 intval = get_itype_value(
				   "MDAS_TD_AUTH_SCHM.auth_scheme", 
					 data_value_2);
			 if (failure != 0) {
			   return (MDAS_TD_AUTH_SCHM_ACCESS_ERROR);
			 }

                         sprintf(atname[0],"user_id");
			 sprintf(atname[1],"distin_name");
			 sprintf(atname[2],"auth_scheme_id");
			 sprintf(atval[0],"%i",obj_user_id);
			 sprintf(atval[1],"'%s'",distinName);
			 sprintf(atval[2],"%i",intval);
                         i =deleteFromTable(atname,atval,"MDAS_AU_AUTH_MAP",3);
			 if (i != 0) return (MDAS_AU_AUTH_MAP_DELETION_ERROR);

			 break;
    case U_INSERT_AUTH_MAP:
                         strcpy(reg_action_name,"insert authorization mapping");
			 distinName = strstr(data_value_2,":");
			 *distinName = '\0';
			 distinName++;       
			 intval = get_itype_value(
				   "MDAS_TD_AUTH_SCHM.auth_scheme", 
					 data_value_2);
			 if (failure != 0) {
			   return (MDAS_TD_AUTH_SCHM_ACCESS_ERROR);
			 }
			 sprintf(sqlq,"select * from  %sMDAS_AU_AUTH_MAP where user_id = %i and distin_name =  '%s' and auth_scheme_id = %i", MDASSCHEMENAME,obj_user_id,distinName,intval);
			 if (check_exists(sqlq) == 0) 
			   return(USER_AUTH_SCHEME_ALREADY_IN_CAT);

                         sprintf(atname[0],"user_id");
			 sprintf(atname[1],"distin_name");
			 sprintf(atname[2],"auth_scheme_id");
			 sprintf(atval[0],"%i",obj_user_id);
			 sprintf(atval[1],"'%s'",distinName);
			 sprintf(atval[2],"%i",intval);
                         i =insertIntoTable(atname,atval,"MDAS_AU_AUTH_MAP",3);
			 if (i != 0) return (MDAS_AU_AUTH_MAP_INSERTION_ERROR);

			 break;
    case U_BULK_INSERT_UDEF_META_DATA_FOR_USER:
                        i =  bulkInsertUdefMetadataForUser(obj_user_id,
				       data_value_1, data_value_2,"");
			if (i != 0) return (i);
			break; 
      default:
                         return (MDAS_CATALOG_MODIFY_TYPE_ERROR);
			 break;


    }
#ifdef FED_MCAT
   sprintf(atname[0],"user_modify_date");
   sprintf(atname[1],"user_id");
   sprintf(atval[0],"'%s'",timeval);
   sprintf(atval[1],"%i",obj_user_id);
   i = changeAttrValue(atname, atval,"MDAS_CD_USER", 2);
   if (i != 0) return (MDAS_CD_USER_UPDATE_ERROR);
#endif
   removeTabledQueries(newUserName);
   sprintf(tmpStr,"%i",obj_user_id);
   removeTabledQueries(tmpStr);
   removeTabledQueries(data_value_1);
  return(MDAS_SUCCESS);

}	

int
modify_tape_info(int cat_type, char *tapeNum,
		     char *obj_user_name,char *user_domain_name,
		     int retraction_type,
		     char *data_value_1, char *data_value_2, 
		     char *data_value_3, char *data_value_4)
{
  int i;
  switch (retraction_type) {
  case T_INSERT_TAPE_INFO:
    /* initially  fullFlag = 0; currentFileSeqNum  = 1;
       currentAbsPosition = 0 ; bytesWritten  = 0 */
    sprintf(sqlq,"insert into %sMDAS_AR_TAPE_INFO values ('%s','%s','%s',%s,0,1,'0',0)", MDASSCHEMENAME,data_value_1,data_value_2, data_value_3,data_value_4);
    i = ExecSqlDb2(henv, hdbc, sqlq);
    if (i != 0) 		      
      return(MDAS_AR_TAPE_INFO_INSERTION_ERROR);
    break;
  case T_UPDATE_TAPE_INFO:
    i = 0;
    if (data_value_1 != NULL && strlen(data_value_1) != 0) {
      sprintf(atname[i],"fullFlag");
      sprintf(atval[i],"%s",data_value_1);
      i++;
    }
    if (data_value_2 != NULL && strlen(data_value_2) !=  0) {
      sprintf(atname[i],"currentFileSeqNum");
      sprintf(atval[i],"%s",data_value_2);
      i++;
    }
    if (data_value_3 != NULL && strlen(data_value_3) != 0) {
      sprintf(atname[i],"currentAbsPosition");
      sprintf(atval[i],"'%s'",data_value_3);
      i++;
    }
    if (data_value_4 != NULL && strlen(data_value_4) != 0) {
      sprintf(atname[i],"bytesWritten");
      sprintf(atval[i],"%s",data_value_4);
      i++;
    }
    sprintf(atname[i],"tapeNumber ");
    sprintf(atval[i],"'%s'",tapeNum);
    j = changeAttrValueSet(atname, atval,"MDAS_AR_TAPE_INFO", i+1, i);
			 if (j != 0) return (MDAS_AR_TAPE_INFO_UPDATE_ERROR);

    break;
  case T_UPDATE_TAPE_INFO_2:
    i = 0;
    if (data_value_1 != NULL && strlen(data_value_1) != 0) {
      sprintf(atname[i],"tapenumber");
      sprintf(atval[i],"'%s'",data_value_1);
      i++;
    }
    if (data_value_2 != NULL && strlen(data_value_2) != 0) {
      sprintf(atname[i],"tapeowner");
      sprintf(atval[i],"'%s'",data_value_2);
      i++;
    }
    if (data_value_3 != NULL && strlen(data_value_3) != 0) {
      sprintf(atname[i],"tapetype");
      sprintf(atval[i],"'%s'",data_value_3);
      i++;
    }
    if (data_value_4 != NULL && strlen(data_value_4) != 0) {
      sprintf(atname[i],"tapelibinx");
      sprintf(atval[i],"%s",data_value_4);
      i++;
    }
    sprintf(atname[i],"tapeNumber ");
    sprintf(atval[i],"'%s'",tapeNum);
    j = changeAttrValueSet(atname, atval,"MDAS_AR_TAPE_INFO", i+1, i);
			 if (j != 0) return (MDAS_AR_TAPE_INFO_UPDATE_ERROR);

    break;
  case T_DELETE_TAPE_INFO:
    sprintf(sqlq,"delete from  %sMDAS_AR_TAPE_INFO where tapeNumber = '%s'",
	    MDASSCHEMENAME,tapeNum);
    i = ExecSqlDb2(henv, hdbc, sqlq);
    if (i != 0) 		      
      return(MDAS_AR_TAPE_INFO_DELETION_ERROR);
    break;
  default:
       return (MDAS_CATALOG_MODIFY_TYPE_ERROR);
    break;
  }
  return(0);

}


int
modify_user_info_nonpriv(int   cat_type, 
		 char *regUserName,
		 char *regUserDomain,
                 char *user_name_domain,
                 char *data_value_1, 
                 char *data_value_2,
                 char *data_value_3,
                 char *data_value_4,
                 char *data_value_5,
                 int   retraction_type)
{

  int    replication_num;
  int    obj_user_id, obj_action_id;
  char  *newUserDomain, *newUserName, *distinName;
  char tmpStr[MAX_TOKEN];
  char *tmpPtr,*tmpPtr1;
  char obj_user_name[MAX_TOKEN], user_domain_name[MAX_TOKEN];

  char mdata_attrnum[MAX_TOKEN], mdata_num[MAX_TOKEN];
  int mdata_maxnum;
  char udmdName[MAX_TOKEN];
  char data_value_encoded[MAX_TOKEN];

  newUserName = obj_user_name;
  /* printf("%s:%s:%s:%s:%s:%i\n",obj_user_name,user_password,user_domain_name,data_value_1,data_value_2,retraction_type);*/
   strcpy(reg_obj_code,"MDAS_CD_USER");
   strcpy(reg_registrar_name,obj_user_name);
   strcpy(reg_obj_name, data_value_1);
   strcpy(reg_action_name,"modify user");
    failure = 0;
    splitbychar(user_name_domain,obj_user_name,user_domain_name,'@');
    obj_user_id = get_user_id_in_domain(obj_user_name,user_domain_name);
    if (obj_user_id < 0) { return (USER_NOT_IN_DOMN);}
    reg_registrar_id = obj_user_id;
    get_time(timeval);
    if (failure != 0 )
      return (GET_TIME_ERROR);
   reg_obj_repl_enum = 0;
   strcpy(reg_timeval,timeval);
   switch (retraction_type)
    {
    case U_CHANGE_PASSWORD:
                         strcpy(reg_action_name,"change password");
			 sprintf(atname[0],"priv_key");
			 sprintf(atname[1],"user_id");
			 obf2Encode(data_value_1, data_value_encoded, 
				    MAX_TOKEN, OBF2_EXTRA_KEY);
			 sprintf(atval[0],"'%s'",data_value_encoded);
			 sprintf(atval[1],"%i",obj_user_id);
			 i = changeAttrValue(atname, atval,"MDAS_AU_AUTH_KEY",
					     2);
			  if (i != 0) return (MDAS_AU_AUTH_KEY_UPDATE_ERROR);

			 break;
    case U_CHANGE_INFO: 
                         strcpy(reg_action_name,"change info");
			 sprintf(atname[0],"user_address");
			 sprintf(atname[1],"user_phone"); 
			 sprintf(atname[2],"user_email");  
                         sprintf(atname[3],"user_id");
			 sprintf(atval[0],"%s",data_value_1);
			 sprintf(atval[1],"%s",data_value_2); 
                         sprintf(atval[2],"%s",data_value_3);  
                         sprintf(atval[3],"%i",obj_user_id); 
			 i = changeAttrValueSet(atname, atval,"MDAS_AU_INFO", 4,1);
			 if (i != 0) return (MDAS_AU_INFO_UPDATE_ERROR);
			 break;
				 
    case U_BULK_INSERT_UDEF_META_DATA_FOR_USER:
                        i =  bulkInsertUdefMetadataForUser(obj_user_id,
				       data_value_1, data_value_2,"");
			if (i != 0) return (i);
			break; 
    case U_CHANGE_USER_DEFINED_STRING_META_DATA:
                         strcpy(mdata_attrnum,"");
			 strcpy(mdata_num,"");
                         splitbychar(data_value_1, mdata_attrnum, mdata_num,'@');
                         strcpy(reg_action_name,"change user defined string meta data");
			 if (strlen(mdata_attrnum) == 0 || 
			     strlen(mdata_num) == 0 ||
			     atoi(mdata_attrnum) < 0 || 
			     atoi(mdata_num) < 0 ) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(udmdName,"userdef_metastr%i",
				 atoi(mdata_attrnum));
			 sprintf(atname[0],"%s",udmdName);
			 sprintf(atname[1],"userdef_modif_date");
                         sprintf(atname[2],"user_id");
			 sprintf(atname[3],"metadatanum");
			 sprintf(atval[0],"'%s'",data_value_2);
			 sprintf(atval[1],"'%s'",timeval);
			 sprintf(atval[2],"%i",obj_user_id);
			 sprintf(atval[3],"%i",atoi(mdata_num));
                         i = changeAttrValueSet(atname, atval,"MDAS_AU_MDATA", 4,2);
			 if (i != 0) return (MDAS_AU_MDATA_UPDATE_ERROR);
      break;
    case U_INSERT_USER_DEFINED_STRING_META_DATA:  
                         strcpy(reg_action_name,"insert user defined string meta data");
      		         sprintf(sqlq,"select user_id from %sMDAS_AU_MDATA where  user_id = %i ", MDASSCHEMENAME, obj_user_id);
			 if (check_exists(sqlq) == 0) {
			   sprintf(sqlq,"select max(metadatanum) from  %sMDAS_AU_MDATA where user_id =  %i ", MDASSCHEMENAME, obj_user_id);
			   mdata_maxnum = get_itype_value_from_query(sqlq) ;
			   if (mdata_maxnum < 0) {
                             if (mdata_maxnum != -1)
			       return (mdata_maxnum);
			     sprintf(sqlq,"delete from %sMDAS_AU_MDATA where user_id =  %i  and metadatanum = -1",
				     MDASSCHEMENAME, obj_user_id);
			     ExecSqlDb2(henv, hdbc, sqlq);
                           }
			   mdata_maxnum++;
			 }
			 else {
			   mdata_maxnum =  0;
			 }
			 if (atoi(data_value_1) < 0) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(udmdName,"userdef_metastr%i",
				 atoi(data_value_1));
			 sprintf(atname[0],"%s",udmdName);
                         sprintf(atname[1],"user_id");
			 sprintf(atname[2],"metadatanum");
			 sprintf(atname[3],"userdef_modif_date");
			 sprintf(atname[4],"userdef_creat_date");
			 sprintf(atval[0],"'%s'",data_value_2);
			 sprintf(atval[1],"%i",obj_user_id);
			 sprintf(atval[2],"%i",mdata_maxnum);
			 sprintf(atval[3],"'%s'",timeval);
			 sprintf(atval[4],"'%s'",timeval);

                         i = insertIntoTable(atname, atval,"MDAS_AU_MDATA", 5);
			 if (i != 0) return (MDAS_AU_MDATA_INSERTION_ERROR);

 			 return(mdata_maxnum);
			 break; 
    case U_DELETE_USER_DEFINED_STRING_META_DATA:  
                         strcpy(reg_action_name,"delete user defined string meta data");
			 if (atoi(data_value_1) < -1) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
                         sprintf(atname[0],"user_id");
			 sprintf(atname[1],"metadatanum");
			 sprintf(atval[0],"%i",obj_user_id);
			 if (atoi(data_value_1) == -1) {
			   i = deleteFromTable(atname,atval,"MDAS_AU_MDATA",1);
			 }
			 else {
			   sprintf(atval[1],"%i",atoi(data_value_1));
			   i = deleteFromTable(atname,atval,"MDAS_AU_MDATA",2);
			 }
			 if (i != 0) return (MDAS_AU_MDATA_DELETION_ERROR);

			 break; 
    case U_DELETE_USER_DEFINED_ATTR_VAL_META_DATA:
                         strcpy(reg_action_name,"delete user defined string meta data");
                         sprintf(atname[0],"user_id");
			 sprintf(atname[1],"userdef_metastr0");
			 sprintf(atname[2],"userdef_metastr1");
			 sprintf(atval[0],"%i",obj_user_id);
			 sprintf(atval[1],"'%s'",data_value_1);
			 sprintf(atval[2],"'%s'",data_value_2);
			 i = deleteFromTable(atname,atval,"MDAS_AU_MDATA",3);
			 if (i != 0) return (MDAS_AU_MDATA_DELETION_ERROR);
			 break; 
      default:
                         return (MDAS_CATALOG_MODIFY_TYPE_ERROR);
			 break;


    }
#ifdef FED_MCAT
   sprintf(atname[0],"user_modify_date");
   sprintf(atname[1],"user_id");
   sprintf(atval[0],"'%s'",timeval);
   sprintf(atval[1],"%i",obj_user_id);
   i = changeAttrValue(atname, atval,"MDAS_CD_USER", 2);
   if (i != 0) return (MDAS_CD_USER_UPDATE_ERROR);
#endif
   removeTabledQueries(newUserName);
   sprintf(tmpStr,"%i",obj_user_id);
   removeTabledQueries(tmpStr);
   removeTabledQueries(data_value_1);
  return(MDAS_SUCCESS);

}


int
modify_resource_info(int cat_type, char *rsrc_name,
		     char *obj_user_name,char *user_domain_name,
		     int retraction_type,
		     char *data_value_1, char *data_value_2, 
		     char *data_value_3, char *data_value_4)
{
  int    i, rsrc_id, obj_user_id, obj_action_id;
  int    replication_num, copy_repl_num;
  int    all_replicas;
  char chuser[MAX_TOKEN];
  char chdomain[MAX_TOKEN];
  char mdata_attrnum[MAX_TOKEN], mdata_num[MAX_TOKEN];
  int mdata_maxnum;
  int    repllist[MAX_TOKEN];
  int colcount,jj;
  int given_repl_enum, out_repl_enum;
  char *tmpStr;
  char udmdName[MAX_TOKEN];
  char didStr[MAX_TOKEN], action_term[MAX_TOKEN];

   if (retraction_type ==  T_INSERT_TAPE_INFO ||
       retraction_type ==  T_UPDATE_TAPE_INFO ||
       retraction_type ==  T_UPDATE_TAPE_INFO_2 ||
       retraction_type ==  T_DELETE_TAPE_INFO )
     return(modify_tape_info(cat_type,rsrc_name,obj_user_name,
			     user_domain_name,
			     retraction_type, data_value_1, data_value_2, 
			     data_value_3, data_value_4));
   failure = 0;
   all_replicas = 0;
   replication_num = 0;
   given_repl_enum = -1;
   if ((tmpStr = strstr(rsrc_name,"&COPY=")) != NULL)  {
     *tmpStr ='\0';
     given_repl_enum= atoi((char *) &tmpStr[6]);
     replication_num =given_repl_enum;
   }
   else {
     all_replicas = 1;
   }
   i = get_rsrc_id_with_accs(rsrc_name,given_repl_enum,
			     obj_user_name,user_domain_name,"all", 
			     &rsrc_id, &obj_user_id,&out_repl_enum);
   if (i < 0) {
     obj_user_id = get_user_id_in_domain(obj_user_name,user_domain_name);
      if (obj_user_id < 0 )  {   return (USER_NOT_IN_DOMN);  }
      i = is_system_userid(obj_user_id);
      if (i != 0) 
	return(i);
      i = get_rsrc_id(rsrc_name,given_repl_enum, &rsrc_id, &out_repl_enum);
      if (i < 0) 
	return(i);
   }
   if (given_repl_enum < 0)
     replication_num = out_repl_enum;
     
   strcpy(reg_registrar_name,obj_user_name);
   strcpy(reg_obj_name, rsrc_name);
   strcpy(reg_obj_code,"MDAS_CD_RSRC");
   strcpy(reg_action_name,"modify resource");
   get_time(timeval);

   reg_registrar_id = obj_user_id;
   reg_obj_id =  rsrc_id;
   reg_obj_repl_enum = replication_num;
   strcpy(reg_timeval,timeval);
   sprintf(didStr,"%i",rsrc_id);

  switch (retraction_type)
    {
    case R_INSERT_ACCS:
       strcpy(reg_action_name,"insert access");
       splitbychar(data_value_1, chuser, chdomain,'@');
       intval = get_user_id_in_domain(chuser, chdomain);
       if (intval < 0)  return (USER_NOT_IN_DOMN);
       tabledIsOkay = 1;
       intval2 = get_itype_value("MDAS_TD_RSRC_ACCS.access_constraint", data_value_2);
       tabledIsOkay = 0;
       if (failure != 0) return (ACCESS_TYPE_NOT_IN_CAT);  
       sprintf(atname[0],"rsrc_id");
       sprintf(atname[1],"user_id");
       sprintf(atname[2],"repl_enum");
       sprintf(atval[0],"%i",rsrc_id);
       sprintf(atval[1],"%i",intval);
       sprintf(atval[2],"%i",replication_num);
       if (all_replicas) j = 2; else j = 3;
       i = deleteFromTable(atname, atval,"MDAS_AR_ACCS", j);
       if (i != 0) return (MDAS_AR_ACCS_DELETION_ERROR);
       
       sprintf(atname[2],"access_id");
       sprintf(atname[3],"repl_enum");
       sprintf(atval[2],"%i",intval2);
       sprintf(atval[3],"%i",replication_num);
       if (all_replicas == 0) {
	 i = insertIntoTable(atname, atval,"MDAS_AR_ACCS",4);
	 if (i != 0) return (MDAS_AR_ACCS_INSERTION_ERROR);
       }
       else {
	 sprintf(sqlq,
		 "select repl_enum from %sMDAS_AR_REPL where rsrc_id = %i ",
		 MDASSCHEMENAME, rsrc_id);
	 i = get_itype_list_from_query(repllist, 
				       &colcount,sqlq);
	 if (i == 0 ) {
	   for(jj = 0; jj < colcount; jj++) {
	     sprintf(atval[3],"%i", repllist[jj]);
	     i = insertIntoTable(atname, atval, "MDAS_AR_ACCS", 4);
	     if (i != 0) return (MDAS_AR_ACCS_INSERTION_ERROR);
	     replication_num = repllist[jj];
	   }
	 }
	 else {
	   return (MDAS_AR_ACCS_INSERTION_ERROR);
	 }  
       }
       strcpy(action_term,"insert access");
       break;
    case R_DELETE_ACCS:
       strcpy(reg_action_name,"delete access");
       splitbychar(data_value_1, chuser, chdomain,'@');
       intval = get_user_id_in_domain(chuser, chdomain);
       if (intval < 0) { return (USER_NOT_IN_DOMN);}
       sprintf(atname[0],"rsrc_id");
       sprintf(atname[1],"user_id");
       sprintf(atname[2],"repl_enum");
       sprintf(atval[0],"%i",rsrc_id);
       sprintf(atval[1],"%i",intval);
       sprintf(atval[2],"%i",replication_num);
       if (all_replicas) j = 2; else j = 3;
       i = deleteFromTable(atname, atval,"MDAS_AR_ACCS", j);
       if (i != 0) return (MDAS_AR_ACCS_DELETION_ERROR);
       break;
    case R_CHANGE_OWNER:
      splitbychar(data_value_1, chuser, chdomain,'@');
      intval = get_user_id_in_domain(chuser, chdomain);
      if (intval < 0) { return (USER_NOT_IN_DOMN);}
      sprintf(atname[0],"owner");
      sprintf(atname[1],"rsrc_id");
      sprintf(atval[0],"%i",intval);
      sprintf(atval[1],"%i",rsrc_id);
      i = changeAttrValue(atname, atval,"MDAS_AR_REPL", 2);
      if (i != 0) return (MDAS_AR_REPL_UPDATE_ERROR);
      sprintf(atname[0],"phy_owner");
      sprintf(atname[1],"phy_rsrc_id");
      i = changeAttrValue(atname, atval,"MDAS_AR_PHYSICAL", 2);
      if (i != 0) return (MDAS_AR_PHYSICAL_UPDATE_ERROR);
       break;      
    case R_BULK_INSERT_UDEF_META_DATA_FOR_RSRC:
      i =  bulkInsertUdefMetadataForResource(rsrc_id,
					     data_value_1, data_value_2,"");
      if (i != 0) return (i);
      break; 
    case R_ADJUST_LATENCY_MAX_IN_MILLISEC:
      sprintf(sqlq,"select rsrc_id from %sMDAS_AR_INFO where rsrc_id = %i\n",MDASSCHEMENAME,rsrc_id);
	sprintf(atname[0],"mlsec_latency_max");
	sprintf(atname[1],"rsrc_id");
	sprintf(atval[0],"%i",atoi(data_value_1));
	sprintf(atval[1],"%i",rsrc_id);
      if (check_exists(sqlq) == 0) {
	i = changeAttrValue(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_UPDATE_ERROR);
      }
      else {
	i = insertIntoTable(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_INSERTION_ERROR);
      }
      break;
    case R_ADJUST_LATENCY_MIN_IN_MILLISEC:
      sprintf(sqlq,"select rsrc_id from %sMDAS_AR_INFO where rsrc_id = %i\n",MDASSCHEMENAME,rsrc_id);
	sprintf(atname[0],"mlsec_latency_min");
	sprintf(atname[1],"rsrc_id");
	sprintf(atval[0],"%i",atoi(data_value_1));
	sprintf(atval[1],"%i",rsrc_id);
      if (check_exists(sqlq) == 0) {
	i = changeAttrValue(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_UPDATE_ERROR);
      }
      else {
	i = insertIntoTable(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_INSERTION_ERROR);
      }
      break;
    case R_ADJUST_BANDWIDTH_IN_MBITSPS:
      sprintf(sqlq,"select rsrc_id from %sMDAS_AR_INFO where rsrc_id = %i\n",MDASSCHEMENAME,rsrc_id);
	sprintf(atname[0],"mbps_bandwidth");
	sprintf(atname[1],"rsrc_id");
	sprintf(atval[0],"%i",atoi(data_value_1));
	sprintf(atval[1],"%i",rsrc_id);
      if (check_exists(sqlq) == 0) {
	i = changeAttrValue(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_UPDATE_ERROR);
      }
      else {
	i = insertIntoTable(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_INSERTION_ERROR);
      }
      break;
    case R_ADJUST_MAXIMUM_CONCURRENCY:
      sprintf(sqlq,"select rsrc_id from %sMDAS_AR_INFO where rsrc_id = %i\n",MDASSCHEMENAME,rsrc_id);
	sprintf(atname[0],"concurrency_max");
	sprintf(atname[1],"rsrc_id");
	sprintf(atval[0],"%i",atoi(data_value_1));
	sprintf(atval[1],"%i",rsrc_id);
      if (check_exists(sqlq) == 0) {
	i = changeAttrValue(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_UPDATE_ERROR);
      }
      else {
	i = insertIntoTable(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_INSERTION_ERROR);
      }
      break;
    case R_ADJUST_NUM_OF_STRIPES:
      sprintf(sqlq,"select rsrc_id from %sMDAS_AR_INFO where rsrc_id = %i\n",MDASSCHEMENAME,rsrc_id);
	sprintf(atname[0],"num_of_stripes");
	sprintf(atname[1],"rsrc_id");
	sprintf(atval[0],"%i",atoi(data_value_1));
	sprintf(atval[1],"%i",rsrc_id);
      if (check_exists(sqlq) == 0) {
	i = changeAttrValue(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_UPDATE_ERROR);
      }
      else {
	i = insertIntoTable(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_INSERTION_ERROR);
      }
      break;
    case R_ADJUST_NUM_OF_HIERARCHIES:
      sprintf(sqlq,"select rsrc_id from %sMDAS_AR_INFO where rsrc_id = %i\n",MDASSCHEMENAME,rsrc_id);
	sprintf(atname[0],"num_of_hierarchies");
	sprintf(atname[1],"rsrc_id");
	sprintf(atval[0],"%i",atoi(data_value_1));
	sprintf(atval[1],"%i",rsrc_id);
      if (check_exists(sqlq) == 0) {
	i = changeAttrValue(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_UPDATE_ERROR);
      }
      else {
	i = insertIntoTable(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_INSERTION_ERROR);
      }
      break;
    case R_ADJUST_RSRC_COMMENT:
      sprintf(sqlq,"select rsrc_id from %sMDAS_AR_INFO where rsrc_id = %i\n",MDASSCHEMENAME,rsrc_id);
        sprintf(atname[0],"rsrc_comments");
        sprintf(atname[1],"rsrc_id");
        sprintf(atval[0],"'%s'",data_value_1);
        sprintf(atval[1],"%i",rsrc_id);
      if (check_exists(sqlq) == 0) {
        i = changeAttrValue(atname, atval,"MDAS_AR_REPL", 2);
        if (i != 0) return (MDAS_AR_INFO_UPDATE_ERROR);
      }
      else {
        i = insertIntoTable(atname, atval,"MDAS_AR_REPL", 2);
        if (i != 0) return (MDAS_AR_INFO_INSERTION_ERROR);
      }
      break;
    case R_ADJUST_CAPACITY_IN_GIGABYTES:
      sprintf(sqlq,"select rsrc_id from %sMDAS_AR_INFO where rsrc_id = %i\n",MDASSCHEMENAME,rsrc_id);
	sprintf(atname[0],"megab_capacity");
	sprintf(atname[1],"rsrc_id");
	sprintf(atval[0],"%i",atoi(data_value_1));
	sprintf(atval[1],"%i",rsrc_id);
      if (check_exists(sqlq) == 0) {
	i = changeAttrValue(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_UPDATE_ERROR);
      }
      else {
	i = insertIntoTable(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_INSERTION_ERROR);
      }
      break;
    case R_ADJUST_DESCRIPTION:
      sprintf(sqlq,"select rsrc_id from %sMDAS_AR_INFO where rsrc_id = %i",MDASSCHEMENAME,rsrc_id);
	sprintf(atname[0],"description");
	sprintf(atname[1],"rsrc_id");
	sprintf(atval[0],"'%s'",data_value_1);
	sprintf(atval[1],"%i",rsrc_id);
      if (check_exists(sqlq) == 0) {
	i = changeAttrValue(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_UPDATE_ERROR);
      }
      else {
	i = insertIntoTable(atname, atval,"MDAS_AR_INFO", 2);
	if (i != 0) return (MDAS_AR_INFO_INSERTION_ERROR);
      }
      break;
    case R_INSERT_RSRC_USAGE_TOTAL_BY_DATE:
      splitbychar(data_value_3, chuser, chdomain,'@');
      intval = get_user_id_in_domain(chuser, chdomain);
      if (intval < 0) { return (USER_NOT_IN_DOMN);}
      sprintf(atname[0],"usage_timestamp");
      sprintf(atname[1],"sum_data_size");
      sprintf(atname[2],"data_owner");
      sprintf(atname[3],"rsrc_id");
      if (!strcmp(data_value_1,"-1"))
	sprintf(atval[0],"'%s'",timeval);
      else
	sprintf(atval[0],"'%s'",data_value_1);
      sprintf(atval[1],"%i",atoi(data_value_2));
      sprintf(atval[2],"%i",intval);
      sprintf(atval[3],"%i",rsrc_id);
      i = insertIntoTable(atname, atval,"MDAS_AR_DAILY_TOT", 4);
      if (i != 0) return (MDAS_AR_DAILY_TOT_INSERTION_ERROR);
      break;
    case R_INSERT_RSRC_USAGE_AND_QUOTA:
      splitbychar(data_value_3, chuser, chdomain,'@');
      intval = get_user_id_in_domain(chuser, chdomain);
      if (intval < 0) { return (USER_NOT_IN_DOMN);}
      sprintf(sqlq,"select rsrc_id from %sMDAS_AR_USE_QUOTA where rsrc_id = %i and data_owner = %i",
	      MDASSCHEMENAME,rsrc_id,intval);
      if (atoi(data_value_1) == -1) {
	sprintf(atname[0],"sum_data_size");
	sprintf(atname[1],"data_owner");
	sprintf(atname[2],"rsrc_id");
	sprintf(atval[0],"%i",atoi(data_value_2));
	sprintf(atval[1],"%i",intval);
	sprintf(atval[2],"%i",rsrc_id);
	j  = 3;
      }
      else {
	sprintf(atname[0],"max_quota");
	sprintf(atname[1],"sum_data_size");
	sprintf(atname[2],"data_owner");
	sprintf(atname[3],"rsrc_id");
	sprintf(atval[0],"%i",atoi(data_value_1));
	sprintf(atval[1],"%i",atoi(data_value_2));
	sprintf(atval[2],"%i",intval);
	sprintf(atval[3],"%i",rsrc_id);
	j  = 4;
      }
      if (check_exists(sqlq) == 0) {
	i = changeAttrValueSet(atname, atval,"MDAS_AR_USE_QUOTA", j, j-2);
	if (i != 0) return (MDAS_AR_USE_QUOTA_UPDATE_ERROR);
      }
      else {
	i = insertIntoTable(atname, atval,"MDAS_AR_USE_QUOTA", j);
	if (i != 0) return (MDAS_AR_USE_QUOTA_INSERTION_ERROR);
      }
      break;
    case R_CHANGE_USER_DEFINED_STRING_META_DATA:
                         strcpy(mdata_attrnum,"");
			 strcpy(mdata_num,"");
                         splitbychar(data_value_1, mdata_attrnum, mdata_num,'@');
                         strcpy(reg_action_name,"change user defined string meta data");			 
			 if (strlen(mdata_attrnum) == 0 || 
			     strlen(mdata_num) == 0 ||
			     atoi(mdata_attrnum) < 0 || 
			     atoi(mdata_num) < 0 ) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(udmdName,"userdef_metastr%i",
				 atoi(mdata_attrnum));
			 sprintf(atname[0],"%s",udmdName);
			 sprintf(atname[1],"userdef_modif_date");
                         sprintf(atname[2],"rsrc_id");
			 sprintf(atname[3],"metadatanum");
			 sprintf(atval[0],"'%s'",data_value_2);
			 sprintf(atval[1],"'%s'",timeval);
			 sprintf(atval[2],"%i",rsrc_id);
			 sprintf(atval[3],"%i",atoi(mdata_num));
                         i = changeAttrValueSet(atname, atval,"MDAS_AR_MDATA", 4,2);
			 if (i != 0) return (MDAS_AR_MDATA_UPDATE_ERROR);
      break;
    case R_INSERT_USER_DEFINED_STRING_META_DATA:  
                         strcpy(reg_action_name,"insert user defined string meta data");
      		         sprintf(sqlq,"select rsrc_id from %sMDAS_AR_MDATA where  rsrc_id = %i ", MDASSCHEMENAME, rsrc_id);
			 if (check_exists(sqlq) == 0) {
			   sprintf(sqlq,"select max(metadatanum) from  %sMDAS_AR_MDATA where rsrc_id =  %i ", MDASSCHEMENAME, rsrc_id);
			   mdata_maxnum = get_itype_value_from_query(sqlq) ;
			   if (mdata_maxnum < 0) {
                             if (mdata_maxnum != -1)
			       return (mdata_maxnum);
			     sprintf(sqlq,"delete from %sMDAS_AR_MDATA where rsrc_id =  %i  and metadatanum = -1",
                                     MDASSCHEMENAME, rsrc_id);
			     ExecSqlDb2(henv, hdbc, sqlq);
                           }
			   mdata_maxnum++;
			 }
			 else {
			   mdata_maxnum =  0;
			 }
			 if (atoi(data_value_1) < 0) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
			 sprintf(udmdName,"userdef_metastr%i",
				 atoi(data_value_1));
			 sprintf(atname[0],"%s",udmdName);
                         sprintf(atname[1],"rsrc_id");
			 sprintf(atname[2],"metadatanum");
			 sprintf(atname[3],"userdef_modif_date");
			 sprintf(atname[4],"userdef_creat_date");
			 sprintf(atval[0],"'%s'",data_value_2);
			 sprintf(atval[1],"%i",rsrc_id);
			 sprintf(atval[2],"%i",mdata_maxnum);
			 sprintf(atval[3],"'%s'",timeval);
			 sprintf(atval[4],"'%s'",timeval);

                         i = insertIntoTable(atname, atval,"MDAS_AR_MDATA", 5);
			 if (i != 0) return (MDAS_AR_MDATA_INSERTION_ERROR);

 			 return(mdata_maxnum);
			 break; 
    case R_DELETE_USER_DEFINED_STRING_META_DATA:  
                         strcpy(reg_action_name,"delete user defined string meta data");
			 if (atoi(data_value_1) < -1) {
			   return (DATA_VALUE_TYPE_ERROR);
			 }      
                         sprintf(atname[0],"rsrc_id");
			 sprintf(atname[1],"metadatanum");
			 sprintf(atval[0],"%i",rsrc_id);
			 if (atoi(data_value_1) == -1) {
			   i = deleteFromTable(atname,atval,"MDAS_AR_MDATA",1);
			 }
			 else {
			   sprintf(atval[1],"%i",atoi(data_value_1));
			   i = deleteFromTable(atname,atval,"MDAS_AR_MDATA",2);
			 }
			 if (i != 0) return (MDAS_AR_MDATA_DELETION_ERROR);

			 break; 
    case R_DELETE_PHY_RSRC_FROM_LOG_RSRC:  
      failure = 0;
      strcpy(reg_action_name,"delete physical resource from logical resource");
      j = get_itype_value("MDAS_AR_PHYSICAL.phy_rsrc_name", 
				data_value_1);
      if (failure != 0 || j < 0) 
	return (RESOURCE_NOT_IN_CAT);
      sprintf(atname[0],"rsrc_id");
      sprintf(atname[1],"phy_rsrc_id");
      sprintf(atval[0],"%i",rsrc_id);
      sprintf(atval[1],"%i",j);
      i = deleteFromTable(atname,atval,"MDAS_AR_REPL",2);
      if (i != 0) 
	return (MDAS_AR_REPL_DELETION_ERROR);
      
      break; 
    case R_INSERT_PHY_RSRC_INTO_LOG_RSRC:
      failure = 0;
      i  =  get_itype_value("MDAS_AR_REPL.rsrc_name", rsrc_name);
      if (failure != 0) { 
	i =  registerLogicalResourceInfo(rsrc_name,"logical",data_value_1,
					 obj_user_name, user_domain_name, "", "");
      }
      else {
	i  = registerReplicateResourceInfo(data_value_1, "logical", rsrc_name,
					 obj_user_name, user_domain_name, "", "");
	return (i);
      }
      break;
    case R_DELETE_USER_DEFINED_ATTR_VAL_META_DATA:
                         strcpy(reg_action_name,"delete user defined string meta data");
                         sprintf(atname[0],"rsrc_id");
			 sprintf(atname[1],"userdef_metastr0");
			 sprintf(atname[2],"userdef_metastr1");
			 sprintf(atval[0],"%i",rsrc_id);
			 sprintf(atval[1],"'%s'",data_value_1);
			 sprintf(atval[2],"'%s'",data_value_2);
			 i = deleteFromTable(atname,atval,"MDAS_AR_MDATA",3);
			 if (i != 0) return (MDAS_AR_MDATA_DELETION_ERROR);
			 break; 
    case R_INSERT_FREE_SPACE:
      sprintf(atname[0],"free_space");
      sprintf(atname[1],"fs_timestamp");
      sprintf(atname[2],"phy_rsrc_id");
      if (data_value_2 == NULL  || strlen(data_value_2) == 0)
	sprintf(atval[1],"'%s",timeval);
      else
	sprintf(atval[1],"'%s'",data_value_2);
      sprintf(atval[0],"%i",atoi(data_value_1));
      sprintf(atval[2],"%i",rsrc_id);
      i = changeAttrValueSet(atname, atval,"MDAS_AR_PHYSICAL", 3,2);
      if (i != 0) return (MDAS_AR_PHYSICAL_UPDATE_ERROR);
      break;
    default:
       return (MDAS_CATALOG_MODIFY_TYPE_ERROR);
       break;
    }
#ifdef FED_MCAT
   sprintf(atname[0],"rsrc_modify_date");
   sprintf(atname[1],"rsrc_id");
   sprintf(atname[2],"repl_enum");
   sprintf(atval[0],"'%s'",timeval);
   sprintf(atval[1],"%i",rsrc_id);
   sprintf(atval[2],"%i",replication_num);
   if (all_replicas) j = 2; else j = 3;
   i = changeAttrValue(atname, atval,"MDAS_AR_REPL", j);
   if (i != 0) return (MDAS_AR_REPL_UPDATE_ERROR);
#endif
   removeTabledQueries(rsrc_name);
   removeTabledQueries(didStr);


   return(0);
}	 

int move_dataset(int cat_type, char *my_obj_data_name, 
		 char *my_collection_name, char *old_resource_name,
		 char *old_obj_path_name, char *new_obj_resource_name,
		 char *new_obj_path_name, char *obj_user_name,
		 char *domain_name)
{ 
 char   my_new_obj_path_name[MAX_TOKEN + 20];
 char   my_old_obj_path_name[MAX_TOKEN + 20];
 char obj_data_name[MAX_TOKEN + 20];
 char collection_name[MAX_TOKEN + 20];
 char versionString[MAX_TOKEN + 20];
 char tmpStr[MAX_TOKEN];
  int    obj_data_id, obj_user_id, obj_action_id;
  int    replication_num;
    failure = 0;
   strcpy(reg_registrar_name,obj_user_name);
   strcpy(reg_obj_name, obj_data_name);
   strcpy(reg_obj_code,"MDAS_CD_DATA");
   strcpy(reg_action_name,"move");

   
   make_quote_to_two_quote(old_obj_path_name,my_old_obj_path_name);
   make_quote_to_two_quote(new_obj_path_name,my_new_obj_path_name);
   make_quote_to_two_quote(my_obj_data_name,obj_data_name);
   make_quote_to_two_quote(my_collection_name,collection_name);


    i =get_data_id_and_version_with_collection(obj_data_name, my_old_obj_path_name, 
				   old_resource_name, 
		   obj_user_name, collection_name,  domain_name, 
		   &obj_data_id, &replication_num, &obj_user_id, versionString);
  if (i < 0)
    {  
      return (i);
    }

  if (inContainerObjectById(obj_data_id) == MDAS_SUCCESS)
    return(ACTN_NOT_ALLOWED_ON_IN_CONTAINER_OR_LINKED_OBJECT);

  get_time(timeval);

   reg_registrar_id = obj_user_id;
   reg_obj_id =  obj_data_id;
   reg_obj_repl_enum = replication_num;
   strcpy(reg_timeval,timeval);
   
  
  tabledIsOkay = 1;
  intval = get_itype_value("MDAS_CD_RSRC.rsrc_name",
			   new_obj_resource_name  );
  tabledIsOkay = 0;
  if (intval < 0) {
    return (MDAS_CD_RSRC_ACCESS_ERROR);
  }

  res = check_unique_dname_in_respath(obj_data_name, my_new_obj_path_name,intval);
  if (res != 0)   {
    sprintf(query_string,"data movement API: name not unique in path:%s/%s in %s",
	    my_new_obj_path_name, obj_data_name, new_obj_resource_name);
    return (DATA_NOT_UNIQUE_IN_PATH);
  }



  sprintf(atname[0],"path_name");
  sprintf(atname[1],"data_id");
  sprintf(atname[2],"repl_enum");
  sprintf(atname[3],"version_num");
  sprintf(atval[0],"'%s'",my_new_obj_path_name);
  sprintf(atval[1],"%i",obj_data_id);
  sprintf(atval[2],"%i",replication_num);
  sprintf(atval[3],"'%s'",versionString);
  i = changeAttrValue(atname, atval,"MDAS_AD_REPL", 4);
  if (i !=0) return (MDAS_AD_REPL_UPDATE_ERROR);
  sprintf(atname[0],"rsrc_id");
  sprintf(atname[1],"data_id");
  sprintf(atname[2],"repl_enum");
  sprintf(atval[0],"%i",intval);
  sprintf(atval[1],"%i",obj_data_id);
  sprintf(atval[2],"%i",replication_num);
  i = changeAttrValue(atname, atval,"MDAS_AD_REPL", 4);
  if (i !=0) return (MDAS_AD_REPL_UPDATE_ERROR);

  performAudit(D_MOVE,obj_user_id,obj_data_id,"","");


  removeTabledQueries(obj_data_name);
  sprintf(tmpStr,"%i",obj_data_id);
  removeTabledQueries(tmpStr);

  return(MDAS_SUCCESS);


}

int copy_dataset(int cat_type, char *obj_data_name, 
		 char *collection_name, char *old_resource_name,
		 char *old_obj_path_name, char *new_obj_resource_name,
		 char *new_obj_path_name, char *obj_user_name, 
		 char *domain_name)
{
  int i;
  i = copy_dataset_with_repl_enum(cat_type,obj_data_name,collection_name,
				  old_resource_name,old_obj_path_name,
				  new_obj_resource_name,new_obj_path_name,
				  obj_user_name,domain_name,-1);
  return(i);
}

int copy_dataset_with_repl_enum(int cat_type, char *my_obj_data_name, 
		 char *my_collection_name, char *old_resource_name,
		 char *old_obj_path_name, char *new_obj_resource_name,
		 char *new_obj_path_name, char *obj_user_name, 
		 char *domain_name, int given_repl_enum)
{ 

  int    obj_data_id, obj_user_id,  obj_action_id;
  int    replication_num;
  char   my_new_obj_path_name[MAX_TOKEN + 20];
  char   my_old_obj_path_name[MAX_TOKEN + 20];
  char obj_data_name[MAX_TOKEN + 20];
  char collection_name[MAX_TOKEN + 20];

  char tmpStr[MAX_TOKEN];

   strcpy(reg_registrar_name,obj_user_name);
   strcpy(reg_obj_name, obj_data_name);
   strcpy(reg_obj_code,"MDAS_CD_DATA");
   strcpy(reg_action_name,"copy");

    failure = 0;

     make_quote_to_two_quote(old_obj_path_name,my_old_obj_path_name);
     make_quote_to_two_quote(new_obj_path_name,my_new_obj_path_name);
     make_quote_to_two_quote(my_obj_data_name,obj_data_name);
     make_quote_to_two_quote(my_collection_name,collection_name);


    /** Raja Aug 12, 2002 - changed to ignore obj path name ****/
     /* XXXXX - revert back 01/19/05 -- messed up replication of
      * container with a good and bad copy. 
      my_old_obj_path_name[0] = '\0';  */
   i =get_data_repl_info(obj_data_name,my_old_obj_path_name, old_resource_name,
			   collection_name, obj_user_name, domain_name,
			   atval);
     if (i < 0)  {  return (i);   }

  if (inContainerObjectById(atoi(atval[0])) == MDAS_SUCCESS)
    return(ACTN_NOT_ALLOWED_ON_IN_CONTAINER_OR_LINKED_OBJECT);

     /* NOT NEEDED strcpy(atval[8], atval[11]); */
  if (given_repl_enum == -1) {
    replication_num = get_max_repl_num(atoi(atval[0]));
    if ( replication_num < 0)
      {  
	return (NO_DATA_REPLICA_IN_CAT);
      }
  }
  else {
    replication_num = given_repl_enum;
  }
    
   intval3 = get_user_id_in_domain(obj_user_name, domain_name);
     if (intval3 < 0) { return (USER_NOT_IN_DOMN);}

   get_time(timeval);
   strcpy(reg_timeval,timeval);
   reg_registrar_id =  intval3;
   reg_obj_id =  atoi(atval[0]);
   reg_obj_repl_enum = replication_num;

  tabledIsOkay = 1;
  intval = get_itype_value("MDAS_CD_RSRC.rsrc_name",
			   new_obj_resource_name  );
  tabledIsOkay = 0;
  if (intval < 0) {

    return (MDAS_CD_RSRC_ACCESS_ERROR);
  }


  res = check_unique_dname_in_respath(obj_data_name, my_new_obj_path_name,intval);
  if (res != 0)   {
    sprintf(query_string,"data copy API: name not unique in path:%s/%s in %s",
	    my_new_obj_path_name, obj_data_name, new_obj_resource_name);
    return (DATA_NOT_UNIQUE_IN_PATH);
  }


  sprintf(atname[0],"data_id");
  sprintf(atname[1],"repl_enum");
  sprintf(atname[2],"data_name");
  sprintf(atname[3],"data_typ_id");
  sprintf(atname[4],"path_name");
  sprintf(atname[5],"rsrc_id");
  sprintf(atname[6],"repl_timestamp");
  sprintf(atname[7],"data_grp_id");
  sprintf(atname[8],"data_size");
  sprintf(atname[9],"is_deleted");
  sprintf(atname[10],"data_owner");
  sprintf(atname[11],"is_dirty");
  sprintf(atname[12],"offset");
  sprintf(atname[13],"container_id");
  sprintf(atname[14],"datalock");
  sprintf(atname[15],"lockowner");
  sprintf(atname[16],"version_num");
  sprintf(atname[17],"container_td_id");
  sprintf(atname[18],"d_comment");
  sprintf(atname[19],"modify_timestamp");
  sprintf(atname[20],"last_accs_time");
  sprintf(atname[21],"auditflag");
  sprintf(atname[22],"seg_num");
  sprintf(atname[23],"dlockexpiry");
  sprintf(atname[24],"dchecksum");
  sprintf(atname[25],"dhide");

  sprintf(atval[1],"%i",replication_num);
  sprintf(atval[4],"'%s'",my_new_obj_path_name);
  sprintf(atval[5],"%i",intval);
  sprintf(atval[6],"'%s'",timeval);
  sprintf(atval[19],"'%s'",timeval);
  sprintf(atval[20],"'%s'",timeval);
  /*  sprintf(atval[10],"%i",intval3); removed apr24,01 for keeping owner original */
  sprintf(atval[39],"'%s'",atval[2]);
  strcpy(atval[2],atval[39]);
  sprintf(atval[39],"'%s'",atval[3]);
  strcpy(atval[3],atval[39]);
  sprintf(atval[39],"'%s'",atval[7]);
  strcpy(atval[7],atval[39]);
  sprintf(atval[39],"'%s'",atval[16]);
  strcpy(atval[16],atval[39]);
  sprintf(atval[39],"'%s'",atval[18]);
  strcpy(atval[18],atval[39]);
  sprintf(atval[39],"'%s'",atval[23]);
  strcpy(atval[23],atval[39]);
  sprintf(atval[39],"'%s'",atval[24]);
  strcpy(atval[24],atval[39]);


  i = insertIntoTable(atname,atval,"MDAS_AD_REPL",26);
  if (i !=0) return (MDAS_AD_REPL_INSERTION_ERROR);
  /********** RAJA: Changed Feb 12, 2003 to inherit access permission for the replicated dataset also 
  tabledIsOkay = 1;
  intval2 = get_itype_value(
			    "MDAS_TD_DS_ACCS.access_constraint",
			    "all");
  tabledIsOkay = 0;
  if (failure != 0) {
    return (MDAS_TD_DS_ACCS_ACCESS_ERROR);
  }
  sprintf(atname[0],"data_id");
  sprintf(atname[1],"repl_enum");
  sprintf(atname[2],"user_id");
  sprintf(atname[3],"access_id");
  sprintf(atval[2],"%i",intval3);
  sprintf(atval[3],"%i",intval2);
  i = insertIntoTable(atname, atval,"MDAS_AD_ACCS", 4);
  if (i !=0) return (MDAS_AD_ACCS_INSERTION_ERROR);
  if (atoi(atval[10]) != intval3) {
    sprintf(atval[2],atval[10]);
    i = insertIntoTable(atname, atval,"MDAS_AD_ACCS", 4);  
    if (i !=0) return (MDAS_AD_ACCS_INSERTION_ERROR);   
  }
  *********** RAJA: Changed Feb 12, 2003 to inherit access permission for the replicated dataset also **********/

  sprintf(sqlq, "insert into %sMDAS_AD_ACCS (data_id, repl_enum, user_id,access_id) select %s, %i,  user_id, access_id from %sMDAS_AD_ACCS where data_id = %s and repl_enum = %i ",
	  MDASSCHEMENAME,atval[0],replication_num,  MDASSCHEMENAME,
	  atval[0],replication_num-1);
  i = ExecSqlDb2(henv, hdbc, sqlq);
  if (i != 0) 		      
    return(MDAS_AD_ACCS_INSERTION_ERROR);
  

  performAudit(D_REPLICATE,intval3,atoi(atval[0]),"",query_string);


  removeTabledQueries(obj_data_name);
  sprintf(tmpStr,"%i",obj_data_id);
  removeTabledQueries(tmpStr);
  /**** changed Aug 5, 2002 to return replication_num instead of
  return(MDAS_SUCCESS);
  ****/
  return(replication_num);

}




int
read_ext_tab_attr_index(int cat_type, char *schemaValues )
{
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int i , j,  selval[MAX_DCS_NUM];
  mdasC_sql_result_struct *myresult;
  char *aval, *tval, *cval, *sval;

  for (i = 0; i < MAX_DCS_NUM; i++)
    {
           sprintf(qval[i],"");
           selval[i] = 0;
    }
  selval[EXTENSIBLE_SCHEMA_NAME] = 1;
  selval[EXTENSIBLE_TABLE_NAME] = 1;
  selval[EXTENSIBLE_ATTR_NAME] = 1;
  selval[IX_COLL_NAME_FOR_DATASET] = 1;
  selval[EXTENSIBLE_ATTR_OUTSIDE_NAME] = 1;
  if (schemaValues != NULL && strlen(schemaValues) > 0)
    sprintf(qval[EXTENSIBLE_SCHEMA_NAME],schemaValues);
  failure = 0;
  i = get_data_dir_info(cat_type, qval, selval, &myresult,
                        dcs_tname, dcs_aname, MAX_EXT_TABLE_INDEX - MIN_EXT_TABLE_INDEX);
  if ( i < 0 || myresult == NULL )
    return(i);
  sval = (char *) get_from_result_struct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_SCHEMA_NAME],dcs_aname[EXTENSIBLE_SCHEMA_NAME]);
  tval = (char *) get_from_result_struct(
                  (mdasC_sql_result_struct *) myresult,
		  dcs_tname[EXTENSIBLE_TABLE_NAME],dcs_aname[EXTENSIBLE_TABLE_NAME]);
  aval = (char *) get_from_result_struct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_NAME],dcs_aname[EXTENSIBLE_ATTR_NAME]);
  cval = (char *) get_from_result_struct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_OUTSIDE_NAME],dcs_aname[EXTENSIBLE_ATTR_OUTSIDE_NAME]);
  j = MIN_EXT_TABLE_INDEX;
  for (i = 0; i <myresult->row_count ; i++) {
    if (strcmp(tval,"UDSMD")) {
      if (strlen(sval) > 0)
	sprintf(dcs_cname[j],"%s.%s",sval,cval);
      else
	sprintf(dcs_cname[j],"%s",cval);
      strcpy(dcs_tname[j],tval);
      strcpy(dcs_aname[j],aval);
      j++;
    }
    sval += MAX_DATA_SIZE;
    tval += MAX_DATA_SIZE;
    aval += MAX_DATA_SIZE;
    cval += MAX_DATA_SIZE;
  }
  sval = (char *) get_from_result_struct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_SCHEMA_NAME],dcs_aname[EXTENSIBLE_SCHEMA_NAME]);
  tval = (char *) get_from_result_struct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_TABLE_NAME],dcs_aname[EXTENSIBLE_TABLE_NAME]);
  aval = (char *) get_from_result_struct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_NAME],dcs_aname[EXTENSIBLE_ATTR_NAME]);
  cval = (char *) get_from_result_struct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_OUTSIDE_NAME],dcs_aname[EXTENSIBLE_ATTR_OUTSIDE_NAME]);
  free (tval);
  free (aval);
  free (cval);
  free (sval);
  free(myresult);
  return(0);

}

int
reload_ext_tab_attr_index(int cat_type, char *schemaValues )
{
  int i;
  for (i = MIN_EXT_TABLE_INDEX ; i <  MAX_EXT_TABLE_INDEX; i++) {
    strcpy(dcs_cname[i],"");
    strcpy(dcs_tname[i],"");
    strcpy(dcs_aname[i],"");
  }
  return(read_ext_tab_attr_index(cat_type,schemaValues ));
}
 


int setConfigInfo()
{
  FILE *fd;
  char mdas_config_file[MAX_TOKEN];
  char buf[MAX_TOKEN], vbuf[MAX_TOKEN];
  int i;
  char metadata_fkrel_file[MAX_TOKEN];
  char   tempdcs[MAX_TOKEN];

  if (getenv("GENERATE_GUID") != NULL)
    strncpy(genGuidFlag,getenv("GENERATE_GUID"),MAX_TOKEN -1);
  else
    strcpy(genGuidFlag,"NO_AUTO_GUID");
  genGuidFlag[MAX_TOKEN-1] = '\0';

  if (getenv("AUDIT_LEVEL") != NULL)
    auditLevel = atoi(getenv("AUDIT_LEVEL"));
  else
    auditLevel = 0;

  if (getenv("SHOW_MCAT_QUERY") != NULL)
    showQuery  = 1;
  else
    showQuery = 0;
  if (getenv("ALLOW_NON_CHECKING_REGISTERY_OF_DATA") != NULL)
    ALLOW_NON_CHECKING_REGISTERY_OF_DATA  = 1;
  else
    ALLOW_NON_CHECKING_REGISTERY_OF_DATA  = 0;

  if (getenv("MDAS_CONFIG_FILE") != NULL)
    {
      strcpy(mdas_config_file,getenv("MDAS_CONFIG_FILE"));
    }
  else if (getenv("MDAS_DATA_DIRECTORY") != NULL)
    {
      strcpy(mdas_config_file,getenv("MDAS_DATA_DIRECTORY"));
      sprintf(mdas_config_file,"%s/%s",mdas_config_file, 
	MDAS_CONFIG_FILE);
    }
  else
    {
      strcpy(mdas_config_file, getSrbDataDir());
      strcat(mdas_config_file, "/");
      strcat(mdas_config_file, MDAS_CONFIG_FILE);

    }
 
  fd = fopen(mdas_config_file,"r");
  if (fd == NULL) return (MDAS_CONFIG_FILE_ERROR);

   while ((i = getNextStr(fd, buf, sizeof(buf))) != EOF) {
	    if (buf[0] == '#') {
	      while (getNextStr(fd, buf, sizeof(buf)) == 0) ;
	      continue;
            }
	     i = getNextStr(fd, vbuf, sizeof(vbuf));
	     if (!strcmp(buf, "MDASDBTYPE"))  strcpy(MDASDBTYPE,vbuf);
	     else if (!strcmp(buf, "MDASSCHEMENAME")) {
	       if (strcmp(vbuf,"''"))
		 strcpy(MDASSCHEMENAME,vbuf);
	       else
		 strcpy(MDASSCHEMENAME,"");
	     }
	     else if (!strcmp(buf, "DB2PASSWORD")) {
	       if (strcmp(vbuf,"''"))
		 strcpy(DB2PASSWORD,vbuf);
	       else
		 strcpy(DB2PASSWORD,"");
	     }
	     else if (!strcmp(buf, "MDASDBNAME"))  
	       strcpy(MDASDBNAME,vbuf);
	     else if (!strcmp(buf, "MDASINSERTSFILE"))  
	       strcpy(MDASINSERTSFILE,vbuf);
	     else if (!strcmp(buf, "MDASCATEXECTYPE"))  
	       strcpy(MDASCATEXECTYPE,vbuf);
	     else if (!strcmp(buf, "MDASCATRSRCIC"))  
	       strcpy(MDASCATRSRCIC,vbuf);
	     else if (!strcmp(buf, "DB2INSTANCE"))  
	       strcpy(DB2INSTANCE,vbuf);
	     else if (!strcmp(buf, "DB2USER"))  
	       strcpy(DB2USER,vbuf);
	     else if (!strcmp(buf, "DB2LOGFILE"))  
	       strcpy(DB2LOGFILE,vbuf);
	     else if (!strcmp(buf, "DBHOME"))
	       strcpy(DBHOME,vbuf);
   }

   /* Initialize the metadatafkrelbuffer */

   strcpy(metadatafkrelbuffer, getSrbDataDir());
   strcat(metadatafkrelbuffer, "/");
   strcat(metadatafkrelbuffer, METADATA_FKREL_FILE);

   fclose(fd);

     if ((strstr(dcs_tname[0],MDASSCHEMENAME) != &dcs_tname[0][0]) ||
	  (strstr(dcs_tname[1],MDASSCHEMENAME) != &dcs_tname[1][0])
	  ) {
        for (i = 0; i <MAX_DCS_NUM; i++) 
            {
               strcpy(tempdcs,dcs_tname[i]);
               sprintf( dcs_tname[i] , "%s%s", MDASSCHEMENAME, tempdcs);
             }
      }
   return(MDAS_SUCCESS);
}

void
makelikeforvarchar(char *str)
{
  /* remove any " %" at end for like */

  char *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4;

  tmpPtr2 = str;
  while (isspace(*tmpPtr2)) tmpPtr2++;
  if (strncasecmp(tmpPtr2, "like ", 5)) {
    if (strncasecmp(tmpPtr2, "not ", 4)) {
      return;
    }
    else {
      tmpPtr2 += 4;
      while (isspace(*tmpPtr2)) tmpPtr2++;
      if (strncasecmp(tmpPtr2, "like ", 5)) 
	return;
    }
  }
  tmpPtr3 = str + strlen(str) -1;
  while (isspace(*tmpPtr3))tmpPtr3--;
  if ((tmpPtr = strstr(tmpPtr2, "ESCAPE")) != NULL) {
    /* skip all 'ESCAPE' */
    tmpPtr++;
    while ((tmpPtr4  = strstr(tmpPtr, "ESCAPE")) != NULL) tmpPtr = tmpPtr4 +1;
    tmpPtr--;
    tmpPtr--;
    while (isspace(*tmpPtr)) tmpPtr--;
    if (*tmpPtr == '\'')  tmpPtr3 = tmpPtr ;
  }
  if (*tmpPtr3 == '\'') tmpPtr3--;
  if (*tmpPtr3 != '%') 
    return;
  while (*tmpPtr3 == '%') tmpPtr3--;
  if (*tmpPtr3 != ' ') 
    return;
  while (isspace(*tmpPtr3)) tmpPtr3--;
  tmpPtr3++;
  *tmpPtr3 = '\'';
  tmpPtr3++;
  while (*tmpPtr3 != '\'') {
    *tmpPtr3 = ' ';
    tmpPtr3++;
  }
  *tmpPtr3 = ' ';
  return;
}

void
makelikeforchar(char **str)
{
  /* add if needed ' %' at end for like */
  char *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4;

  tmpPtr2 = *str;
  while (isspace(*tmpPtr2)) tmpPtr2++;
  if (strncasecmp(tmpPtr2, "like ", 5)) {
    if (strncasecmp(tmpPtr2, "not ", 4)) {
      return;
    }
    else {
      tmpPtr2 += 4;
      while (isspace(*tmpPtr2)) tmpPtr2++;
      if (strncasecmp(tmpPtr2, "like ", 5)) 
	return;
    }
  }
  tmpPtr3 =  *str + strlen(*str) -1;
  while (isspace(*tmpPtr3))tmpPtr3--;
  if ((tmpPtr = strstr(tmpPtr2, "ESCAPE")) != NULL) {
    /* skip all 'ESCAPE' */
    tmpPtr++;
    while ((tmpPtr4  = strstr(tmpPtr, "ESCAPE")) != NULL) tmpPtr = tmpPtr4 +1;
    tmpPtr--;
    tmpPtr--;
    while (isspace(*tmpPtr)) tmpPtr--;
    if (*tmpPtr == '\'')  tmpPtr3 = tmpPtr ;
  }

  if (*tmpPtr3 == '\'') tmpPtr3--;
  if (*tmpPtr3 == '%') 
    return;
  /** possible memory leak - we dont know if 
      space for *str was allocated or static  **/
  tmpPtr = (char *) malloc( strlen(*str) + 7) ;
  tmpPtr3++;
  *tmpPtr3 = '\0';
  sprintf(tmpPtr, "%s %%'", *str);
  tmpPtr3++;
  strcat(tmpPtr, tmpPtr3);
  *str = tmpPtr;
  return;
}

int set_info_srb_call(mdasC_sql_query_struct *queryinfo,
	      mdasC_sql_result_struct *resultinfo, 
	      char qval[][MAX_TOKEN],
	      int selval[], 
	      char tname[][MAX_TOKEN],
	      char aname[][MAX_TOKEN], int numrows)
{
  int i, j, k, l, qq;
  int groupByFlag = 0;
  char *tmpqval;


  int qvalOverFlow = 0;	/* XXXXX this is a temp hack that fix the case where
			 * the length of qval overflow MAX_TOKEN */

  j = 0;
  k = 0;
  l = 0;
  strcpy(groupByList, "GROUP BY ");

  for (i = 0; i < cur_pds_num ; i++) 
    {
      if(strcmp(qval[i],"") && qvalOverFlow == 0)
	{
	      queryinfo->sqlwhere[j].tab_name = (char *) &tname[i];
	      queryinfo->sqlwhere[j].att_name = (char *) &aname[i];

#ifdef MCAT_VERSION_10
	      /* MCAT Version 1.0 used char() instead of varchar() for storing
		 information in the database. */
	      tmpqval = (char *) &qval[i]; 
	      makelikeforchar(&tmpqval);
	      queryinfo->sqlwhere[j].att_value = tmpqval;
#else
	      makelikeforvarchar(qval[i]);
	      queryinfo->sqlwhere[j].att_value = (char *) &qval[i];
#endif
	      /* XXXXX a hack to take care of overflow */
	      if (strlen ((char *) &qval[i]) > MAX_TOKEN)
		qvalOverFlow = 1;
	      else
		qvalOverFlow = 0;

	      j++;
	} else {
	  qvalOverFlow = 0;
	}
      if (selval[i] > 0)
	{
	  queryinfo->sqlselect[k].tab_name  = (char *) &tname[i];
	  queryinfo->sqlselect[k].att_name   = (char *) &aname[i];
	 switch(selval[i])
	   {
	      case 0: break;
              case 1: sprintf(queryinfo->sqlselect[k].aggr_val,
			      "%s.%s ",tname[i],aname[i] );
	              if (strlen(groupByList) > 10) strcat(groupByList, ", ");
	              strcat(groupByList,queryinfo->sqlselect[k].aggr_val);
		      break;

              case 2: sprintf(queryinfo->sqlselect[k].aggr_val,
			      "count(%s.%s) ",tname[i],aname[i] );
	              groupByFlag = 1;
		      break;

              case 3: sprintf(queryinfo->sqlselect[k].aggr_val,
			      "max(%s.%s) ",tname[i],aname[i] );
	              groupByFlag = 1;
		      break;

              case 4: sprintf(queryinfo->sqlselect[k].aggr_val,
			      "min(%s.%s) ",tname[i],aname[i] );
	              groupByFlag = 1;
		      break;

              case 5: sprintf(queryinfo->sqlselect[k].aggr_val,
			      "avg(%s.%s) ",tname[i],aname[i] );
	              groupByFlag = 1;
		      break;

              case 6: sprintf(queryinfo->sqlselect[k].aggr_val,
			      "sum(%s.%s) ",tname[i],aname[i] );
	              groupByFlag = 1;
		      break;
              case 7: sprintf(queryinfo->sqlselect[k].aggr_val,
			      "variance(%s.%s) ",tname[i],aname[i] );
	              groupByFlag = 1;
		      break;
              case 8: sprintf(queryinfo->sqlselect[k].aggr_val,
			      "stddev(%s.%s) ",tname[i],aname[i] );
	              groupByFlag = 1;
		      break;
              case 9: sprintf(queryinfo->sqlselect[k].aggr_val,
                              "count(distinct %s.%s) ",tname[i],aname[i] );
                      groupByFlag = 1;
                      break;
              case 10: sprintf(queryinfo->sqlselect[k].aggr_val,
                              "avg(%s.%s) ",tname[i],aname[i] );
                      groupByFlag = 1;
                      break;
 
              case 11: sprintf(queryinfo->sqlselect[k].aggr_val,
                              "sum(%s.%s) ",tname[i],aname[i] );
                      groupByFlag = 1;
                      break;
              case 12: sprintf(queryinfo->sqlselect[k].aggr_val,
                              "variance(%s.%s) ",tname[i],aname[i] );
                      groupByFlag = 1;
                      break;
              case 13: sprintf(queryinfo->sqlselect[k].aggr_val,
                              "stddev(%s.%s) ",tname[i],aname[i] );
                      groupByFlag = 1;
                      break;
              case 14: sprintf(queryinfo->sqlselect[k].aggr_val,
                              "substr(%s.%s,1,%i) ",tname[i],aname[i],
			       MAX_DATA_SIZE -1 );
                      break;
 
 
  	      default: break;

	   }
	 /***  RAJA Feb 10,2004 changed to strdup to be in allign with pack and unpack routine 
	  resultinfo->sqlresult[k].tab_name = (char *) &tname[i];
	  resultinfo->sqlresult[k].att_name = (char *) &aname[i];
          RAJA Feb 10,2004 changed to strdup to be in allign with pack and unpack routine ***/
	 resultinfo->sqlresult[k].tab_name =  strdup(tname[i]);
	 resultinfo->sqlresult[k].att_name =  strdup(aname[i]);
	  resultinfo->sqlresult[k].values  = (char *)  
	    malloc (numrows * MAX_DATA_SIZE * sizeof(char));
	  if (resultinfo->sqlresult[k].values == NULL) {
	    for (qq = 0  ; qq < k ; qq++) {
	      free(resultinfo->sqlresult[qq].values);
	      /***  added RAJA feb 10,2004  to accomodate the strdup above ***/
	      free(resultinfo->sqlresult[qq].tab_name);
	      free(resultinfo->sqlresult[qq].att_name);
	      /***  added RAJA Feb 10,2004  to accomodate the strdup above ***/
	      resultinfo->sqlresult[qq].tab_name = (char *) NULL;
	      resultinfo->sqlresult[qq].att_name = (char *) NULL;
	      resultinfo->sqlresult[qq].values  = (char *) NULL; 
	    }
	    return(MEMORY_ALLOCATION_ERROR);
	  }
	  k++;
	}

    }
  if (groupByFlag == 0)
     strcpy(groupByList,"");
  if (!strcmp(groupByList,"GROUP BY "))
    strcpy(groupByList,"");

  queryinfo->condition_count = j;
  queryinfo->select_count = k;
  resultinfo->result_count = k;
  return (MDAS_SUCCESS);
}





int auditDatasetAccess(int cat_type, char *obj_user_name,
		       char *my_obj_data_name, char *my_collection_name,
		       char *my_obj_path_name,
			 char *resource_name, 
			 char *obj_access_name, char *comments, int success,
		       char *domain_name)
{ 
    

  int    obj_data_id, obj_user_id, obj_action_id, obj_access_id;
  int    replication_num;
  char   access_name[MAX_TOKEN];
  char obj_path_name[MAX_TOKEN + 20];
  char obj_data_name[MAX_TOKEN + 20];
  char collection_name[MAX_TOKEN + 20];


   strcpy(reg_registrar_name,obj_user_name);
   strcpy(reg_obj_name, obj_data_name);
   strcpy(reg_obj_code,"MDAS_CD_DATA");
   make_quote_to_two_quote(my_obj_path_name,obj_path_name);
   make_quote_to_two_quote(my_obj_data_name,obj_data_name);
   make_quote_to_two_quote(my_collection_name,collection_name);

    i =get_data_id_with_collection_withanyaccess(obj_data_name, obj_path_name, 
				   resource_name, 
		   obj_user_name, collection_name, domain_name,
		   &obj_data_id, &replication_num, &obj_user_id);
    if (i < 0 ) {  return (i);    }

    if (success)
      performAudit(D_AUDIT_SUCCESS,obj_user_id,obj_data_id,"",comments);
    else
      performAudit(D_AUDIT_FAILURE,obj_user_id,obj_data_id,"",comments);



    
  return(MDAS_SUCCESS);
}	




int register_user_info(int cat_type, char *user_name, 
		       char *user_password, char *in_user_domain_name, 
		       char *user_type_name, 
		       char* user_address, char* user_phone, char* user_email,
		        char *registrar_name, char *registrar_password,
		       char *registrar_domain_name)

{ 
  int  obj_action_id;
  char temp1[MAX_TOKEN * 2], newuserNameDomain[MAX_TOKEN * 2];
  char temp2[MAX_TOKEN * 2];
  char collname[MAX_TOKEN];
  char user_domain_name[MAX_TOKEN * 2];
  int i;
  char *tmpPtr;
  char zoneName[MAX_TOKEN];
  char localZoneName[MAX_TOKEN];
  char distinInfo[MAX_TOKEN];
  char data_value_encoded[MAX_TOKEN];

   strcpy(reg_registrar_name,registrar_name);
   strcpy(reg_obj_name, user_name);
   strcpy(reg_obj_code,"MDAS_CD_USER");
   strcpy(reg_action_name,"create new user");
   strcpy(user_domain_name, in_user_domain_name);

   isUserDomainAdminFlag = 0;
   failure = 0;
   i = isUserSysAdmin(registrar_name,registrar_domain_name );
   /*   i = web_mdas_sys_authorization(registrar_name, registrar_password,
	registrar_domain_name );  */
   if (i != 0 && strcmp(user_type_name,"sysadmin") && 
       !strcmp( in_user_domain_name,registrar_domain_name)) {
     i = isUserDomainAdmin(registrar_name, registrar_domain_name );
     isUserDomainAdminFlag = 1;
   }
   if ( i != 0)    
     {
       return(SYS_USER_AUTHORIZATION_ERROR);
     }


  reg_registrar_id = get_user_id_in_domain(registrar_name, 
					   registrar_domain_name);
  if (reg_registrar_id < 0) { return (USER_NOT_IN_DOMN);}


  ival[1] = 0;
  ival[2] = 0;
  ival[10] = -1;
  ival[11] = -1;
  ival[12] = -1;
  ival[13] = -1;
  ival[14] = -1;
  ival[15] = -1;
  ival[16] = -1;
  tabledIsOkay = 1;
  get_ctype_value(cval[3], "MDAS_TD_USER_TYP.user_typ_name", user_type_name);
  tabledIsOkay = 0;
  if (failure != 0) { return (MDAS_TD_USER_TYP_ACCESS_ERROR);}

  strcpy(cval[7],"");
  strcpy(cval[8],"");


#ifdef FED_MCAT
  failure = 0;
  sprintf(query_string, "select zone_id from %s%s where local_zone_flag = 1",
	  MDASSCHEMENAME, "MDAS_TD_ZONE");
  get_ctype_value_from_query(localZoneName,query_string );
  if (failure != 0) 
    return( LOCAL_ZONE_NOT_FOUND);
  if ((tmpPtr = strstr(user_domain_name,"|")) == NULL) {
    strcpy(zoneName,localZoneName);
  }
  else {
    *tmpPtr = '\0';
    strcpy(zoneName, tmpPtr + 1);
  }
#else
  strcpy(zoneName,"");
#endif
  failure = 0;
  get_time(cval[5]);
  if (failure != 0) { return (GET_TIME_ERROR);}
  reg_obj_repl_enum = 0;
  strcpy(reg_timeval,cval[5]);
  
  i = check_unique_name_in_domain("USER",user_name,user_domain_name);
  if (i != 0)   {
    sprintf(query_string,"user registry API: Non-unique User Name:%s\n",
	    user_name);
    return(USER_NOT_UNIQUE_IN_DOMN);
  }
  tabledIsOkay = 1;
  get_ctype_value(cval[6], "MDAS_TD_DOMN.domain_desc", user_domain_name);
  tabledIsOkay = 0;
  if (failure != 0) { return (MDAS_TD_DOMN_ACCESS_ERROR);}
  
  ival[0] = get_next_counter_value("USER_ID");
  if (failure != 0) {
    return (USER_ID_COUNTER_ERROR);
  }
  reg_obj_id = ival[0];


#ifdef FED_MCAT
  sprintf(query_string, 
         "insert into %s%s (user_id,user_name,user_typ_id,zone_id,user_creation_date,user_modify_date) values (%i,'%s','%s','%s','%s','%s')",
	  MDASSCHEMENAME, "MDAS_CD_USER", 
	  ival[0], user_name, cval[3], zoneName,cval[5],cval[5] );
#else
  sprintf(query_string, 
         "insert into %s%s (user_id,user_name,user_typ_id) values (%i,'%s','%s')",
	  MDASSCHEMENAME, "MDAS_CD_USER", 
	  ival[0], user_name, cval[3] );
#endif
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)  
    {error_exit("user registry API: CD user insertion error");
     return(MDAS_CD_USER_INSERTION_ERROR);}
  obf2Encode(user_password, data_value_encoded, 
	     MAX_TOKEN, OBF2_EXTRA_KEY);
  sprintf(query_string, "insert into %s%s (user_id,priv_key) values (%i,'%s')",
	  MDASSCHEMENAME, "MDAS_AU_AUTH_KEY", ival[0],data_value_encoded);
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)  
  {error_exit("user registry API: password insertion error");
   return(MDAS_AU_AUTH_KEY_INSERTION_ERROR);}

  sprintf(query_string, "insert into %s%s (user_id,domain_id)values (%i,'%s')",
	  MDASSCHEMENAME, "MDAS_AU_DOMN", ival[0], cval[6]);
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0) 
    { error_exit("user registry API: DOMN  insertion error");
    return(MDAS_AU_DOMN_INSERTION_ERROR);}

  /* every user added as themselves as a group*/
  sprintf(query_string, "insert into %s%s (user_id,group_user_id) values (%i,%i)",
	  MDASSCHEMENAME, "MDAS_AU_GROUP",ival[0], ival[0]);
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0) 
    {error_exit("user registry API: INTERNAL GROUP data insertion error");
    return(MDAS_AU_GROUP_INSERTION_ERROR);}

  /* every user added into public group*/
  tabledIsOkay = 1;
  ival[17]  = get_itype_value("MDAS_CD_USER.user_name","public");
  tabledIsOkay = 0;
  sprintf(query_string, "insert into %s%s (user_id,group_user_id) values (%i,%i)",
	  MDASSCHEMENAME, "MDAS_AU_GROUP",ival[0], ival[17]);
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0) 
    {error_exit("user registry API: PUBLIC GROUP data insertion error");
    return(MDAS_AU_GROUP_INSERTION_ERROR);}
  
  sprintf(query_string,"insert into %s%s (user_id,user_address,user_phone,user_email) values (%i,'%s','%s','%s')",
	  MDASSCHEMENAME, "MDAS_AU_INFO", ival[0], 
	  user_address, user_phone, user_email); 
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0) 
    {error_exit("user registry API: INFO data insertion error");
    return(MDAS_AU_INFO_INSERTION_ERROR);}
    /* make the user's home collection */
#ifdef  FED_MCAT
    sprintf(temp2,"/%s%s",localZoneName,DEF_PARENT_COLL);
    sprintf(collname, "%s.%s",user_name,user_domain_name);
    i = make_new_collection(0, temp2, collname, registrar_name,
                            registrar_domain_name);
    if (i != 0 && i != DATA_SUBCOLLECTION_NOT_UNIQUE )
      {
         printf("ingestUser MakeCollection Error for %s/%s: %i\n",
                temp2, collname,i);
        commit_db2_interaction(MDAS_CAT_ROLLBACK);
        return(i);
      }
    sprintf(temp1, "%s/%s", temp2, collname);
    i = modify_collection_info(0, registrar_name, temp1,
                               user_name,user_domain_name, "all",
                               D_INSERT_COLL_ACCS, registrar_domain_name);
    if (i != 0)
      {
         printf("ingestUser ModifyCollection Error for %s: %i\n",temp1,i);
        commit_db2_interaction(MDAS_CAT_ROLLBACK);
        return(i);
      }
#endif
    sprintf(temp2,"%s",DEF_PARENT_COLL);
    sprintf(collname, "%s.%s",user_name,user_domain_name);
    i = make_new_collection(0, temp2, collname, registrar_name,
			    registrar_domain_name);
    if (i != 0 && i != DATA_SUBCOLLECTION_NOT_UNIQUE )
      {
	 printf("ingestUser MakeCollection Error for %s/%s: %i\n",
		temp2, collname,i);
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
    sprintf(temp1, "%s/%s", temp2, collname);
    i = modify_collection_info(0, registrar_name, temp1,
			       user_name,user_domain_name, "all",
			       D_INSERT_COLL_ACCS, registrar_domain_name);
    if (i != 0)
      {
	 printf("ingestUser ModifyCollection Error for %s: %i\n",temp1,i); 
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
   
    /* make the user's container collection */
#ifdef  FED_MCAT
    sprintf(temp2,"/%s%s",localZoneName,DEF_CONTAINER_COLL);
    sprintf(collname, "%s.%s",user_name,user_domain_name);
    i = make_new_collection(0, temp2, collname, registrar_name,
                            registrar_domain_name);
    if (i != 0 && i != DATA_SUBCOLLECTION_NOT_UNIQUE)
      {
         printf("ingestUser MakeCollection Error for %s/%s: %i\n",
                temp2, collname,i);
        commit_db2_interaction(MDAS_CAT_ROLLBACK);
        return(i);
      }
    sprintf(temp1, "%s/%s", temp2, collname);
    i = modify_collection_info(0, registrar_name, temp1,
                               user_name,user_domain_name, "all",
                               D_INSERT_COLL_ACCS, registrar_domain_name);
    if (i != 0)
      {
         printf("ingestUser ModifyCollection Error for %s: %i\n",temp1,i);
        commit_db2_interaction(MDAS_CAT_ROLLBACK);
        return(i);
      }
#endif
    sprintf(temp2,"%s",DEF_CONTAINER_COLL);
    sprintf(collname, "%s.%s",user_name,user_domain_name);
    i = make_new_collection(0, temp2, collname, registrar_name,
			    registrar_domain_name);
    if (i != 0 && i != DATA_SUBCOLLECTION_NOT_UNIQUE)
      {
         printf("ingestUser MakeCollection Error for %s/%s: %i\n",
		temp2, collname,i); 
        commit_db2_interaction(MDAS_CAT_ROLLBACK);
        return(i);
      }
    sprintf(temp1, "%s/%s", temp2, collname);
    i = modify_collection_info(0, registrar_name, temp1,
                               user_name,user_domain_name, "all",
                               D_INSERT_COLL_ACCS, registrar_domain_name);
    if (i != 0)
      {
         printf("ingestUser ModifyCollection Error for %s: %i\n",temp1,i); 
        commit_db2_interaction(MDAS_CAT_ROLLBACK);
        return(i);
      }
   
    /* making admin user info */

    if (!strcmp(user_type_name,"sysadmin")) {
#ifdef  FED_MCAT
      sprintf(temp2,"/%s%s",localZoneName,DEF_PARENT_COLL);
      i = modify_collection_info(0, registrar_name, temp2,
                               user_name,user_domain_name, "all",
                               D_INSERT_COLL_ACCS, registrar_domain_name);

      if (i != 0) {
        printf("ingestUser ModifyCollection Error for %s: %i\n",temp2,i);
        commit_db2_interaction(MDAS_CAT_ROLLBACK);
        return(i);
      }
#endif
      sprintf(temp2,"%s",DEF_PARENT_COLL);
      i = modify_collection_info(0, registrar_name, temp2,
                               user_name,user_domain_name, "all",
                               D_INSERT_COLL_ACCS, registrar_domain_name);
      
      if (i != 0)   {
	printf("ingestUser ModifyCollection Error for %s: %i\n",temp2,i); 
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
#ifdef  FED_MCAT
    sprintf(temp2,"/%s%s",localZoneName,DEF_CONTAINER_COLL);
      i = modify_collection_info(0, registrar_name, temp2,
                               user_name,user_domain_name, "all",
                               D_INSERT_COLL_ACCS, registrar_domain_name);

      if (i != 0) {
        printf("ingestUser ModifyCollection Error for %s: %i\n",temp2,i);
        commit_db2_interaction(MDAS_CAT_ROLLBACK);
        return(i);
      }
#endif
    sprintf(temp2,"%s",DEF_CONTAINER_COLL);
      i = modify_collection_info(0, registrar_name, temp2,
                               user_name,user_domain_name, "all",
                               D_INSERT_COLL_ACCS, registrar_domain_name);
      
      if (i != 0)   {
	printf("ingestUser ModifyCollection Error for %s: %i\n",temp2,i); 
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      i = modify_collection_info(0, registrar_name, "/",
                               user_name,user_domain_name, "all",
                               D_INSERT_COLL_ACCS, registrar_domain_name);
      
      if (i != 0)   {
	printf("ingestUser ModifyCollection Error for %s: %i\n","/",i); 
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
      sprintf(newuserNameDomain,"%s@%s",user_name,user_domain_name);
      i = modify_user_info (0,registrar_name, newuserNameDomain,
			    "system",
			    U_INSERT_GROUP,
			    "",registrar_domain_name);
      if (i != 0)   {
	printf("ingestUser modify_user_info error for %s: %i\n",
	       newuserNameDomain,i);
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
    
#ifdef  FED_MCAT
      sprintf(temp2,"/%s",localZoneName);
      i = modify_collection_info(0, registrar_name, temp2,
                               user_name,user_domain_name, "all",
                               D_INSERT_COLL_ACCS, registrar_domain_name);
      
      if (i != 0)   {
	printf("ingestUser ModifyCollection Error for %s: %i\n",temp2,i); 
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
#endif

    }

    sprintf(newuserNameDomain,"%s@%s",user_name,user_domain_name);
    if (strcmp(user_domain_name,"groups")) {
      i = modify_user_info (0,registrar_name, newuserNameDomain,
			    user_domain_name,
			    U_INSERT_GROUP,
			    "",registrar_domain_name);
      if (i != 0)   {
	printf("ingestUser modify_user_info error for groups: %i\n",i);
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
    }
    sprintf(newuserNameDomain,"%s@%s",user_name,user_domain_name);
    /*    sprintf(distinInfo,"generic:/C=UNKNOWN");*/
     sprintf(distinInfo,"ENCRYPT1:NULL",
	     zoneName,user_name,user_address, user_email); 
      
    i = modify_user_info (0,registrar_name, newuserNameDomain,
			    distinInfo,
			    U_INSERT_AUTH_MAP,
			    "",registrar_domain_name);
    if (i != 0)   {
      printf("ingestUser modify_user_info error for distinInfo=%s:%i\n",
	     distinInfo,i);
      commit_db2_interaction(MDAS_CAT_ROLLBACK);
      return(i);
    }
#ifdef FED_MCAT
    if (strcmp(zoneName,localZoneName)) {
      /**** Mike didn't want this: make the user's home collection in their zone 
    
    sprintf(temp2,"/%s%s",zoneName,DEF_PARENT_COLL);
    sprintf(collname, "%s.%s",user_name,user_domain_name);
    i = make_new_collection(0, temp2, collname, registrar_name,
			    registrar_domain_name);
    if (i != 0 && i != DATA_SUBCOLLECTION_NOT_UNIQUE)
      {
	printf("ingestUser MakeCollection Error for %s/%s: %i\n",
	       temp2, collname,i); 
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
    sprintf(temp1, "%s/%s", temp2, collname);
    i = modify_collection_info(0, registrar_name, temp1,
			       user_name,user_domain_name, "all",
			       D_INSERT_COLL_ACCS, registrar_domain_name);
    if (i != 0)
      {
	 printf("ingestUser ModifyCollection Error for %s: %i\n",temp1,i); 
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	return(i);
      }
    Mike didn't want this:*******/
      /**** Mike didn't want this: make the user's container collection in their zone
    sprintf(temp2,"/%s%s",zoneName,DEF_CONTAINER_COLL);
    sprintf(collname, "%s.%s",user_name,user_domain_name);
    i = make_new_collection(0, temp2, collname, registrar_name,
			    registrar_domain_name);
    if (i != 0&& i != DATA_SUBCOLLECTION_NOT_UNIQUE )
      {
	printf("ingestUser MakeCollection Error for %s/%s: %i\n",
	       temp2, collname,i); 
        commit_db2_interaction(MDAS_CAT_ROLLBACK);
        return(i);
      }
    sprintf(temp1, "%s/%s", temp2, collname);
    i = modify_collection_info(0, registrar_name, temp1,
                               user_name,user_domain_name, "all",
                               D_INSERT_COLL_ACCS, registrar_domain_name);
    if (i != 0)
      {
	 printf("ingestUser ModifyCollection Error for %s: %i\n",temp1,i); 
        commit_db2_interaction(MDAS_CAT_ROLLBACK);
        return(i);
      }
          Mike didn't want this:*******/

    }
#endif
    sprintf(query_string,
	    "insert into %s%s (user_id,metadatanum) values (%i,-1)",MDASSCHEMENAME,"MDAS_AU_MDATA",ival[0]);
    res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)
    return(MDAS_AU_MDATA_INSERTION_ERROR);
  return(MDAS_SUCCESS);
  
}


int register_user_group(int cat_type, char *group_name, 
			char *group_password,
			char *group_type_name, 
			char* group_address, char* group_phone, 
			char* group_email,
		        char *registrar_name, char * registrar_password,
			char *registrar_domain_name)
{ 
  int i;
    failure = 0;
  i = register_user_info(cat_type, group_name, group_password, 
			 "groups", group_type_name, 
			 group_address, group_phone, group_email,
			 registrar_name, registrar_password, 
			 registrar_domain_name);
  if (i != 0)  
    return(i);
  get_time(timeval);
       if (failure != 0 )
	 {  
	   return (GET_TIME_ERROR);
	 }

   reg_obj_repl_enum = 0;
   strcpy(reg_timeval,timeval);

  strcpy(reg_obj_code,"MDAS_CD_USER");
  strcpy(reg_registrar_name,registrar_name);
  strcpy(reg_obj_name, group_name);
  strcpy(reg_action_name,"insert group");

  
  intval = get_user_id_in_domain(group_name, "groups");
   if (intval < 0) { return (USER_NOT_IN_DOMN); }

   
   return(i);
}


int get_user_id_in_domain(char *user_name, char *in_user_domain) 
{
  int user_id; 
  char *tmpPtr = NULL;
  char user_domain[MAX_TOKEN * 2];
  strcpy(user_domain,in_user_domain);

  tmpPtr = strstr(user_domain,"|");
  if (tmpPtr != NULL)
      *tmpPtr = '\0';
  if (isInstallSRBUser(user_name) == 0) {
    tmpPtr = NULL;
  }
  /**** SRB 3.0 not checking domain ****/
   tmpPtr = NULL;
  /**** SRB 3.0 not checking domain ****/

  if (tmpPtr == NULL) {
    sprintf(sqlq, "select t2.user_id  from %sMDAS_CD_USER t1, %sMDAS_AU_DOMN t2, %sMDAS_TD_DOMN t3 where t1.user_name = '%s' AND t3.domain_desc = '%s' AND t1.user_id = t2.user_id AND t3.domain_id = t2.domain_id",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  user_name,   user_domain);
  }
  else {
    *tmpPtr = '\0';
    sprintf(sqlq, "select t2.user_id  from %sMDAS_CD_USER t1, %sMDAS_AU_DOMN t2, %sMDAS_TD_DOMN t3 where t1.user_name = '%s' AND t3.domain_desc = '%s' AND t1.zone_id = '%s' AND t1.user_id = t2.user_id AND t3.domain_id = t2.domain_id",
	  MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,
	  user_name,   user_domain, tmpPtr+1);
    *tmpPtr = '|';
  }
  tabledIsOkay = 1;
  user_id = get_itype_value_from_query(sqlq);
  tabledIsOkay = 0;
  if (user_id >= 0) 
    return (user_id);
  else {
    failure = USER_NOT_IN_DOMN;
    return(USER_NOT_IN_DOMN);
  }
  /**** OPT
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	failure = USER_NOT_IN_DOMN;
	return (USER_NOT_IN_DOMN);
      }
    else 
      {
    truncate_end_blanks(data_size[0], data[0]);
    user_id = atoi( (char *) data[0]);
    i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
    return(user_id);
      }
  ****/
}


int getResourceIdWithAccessToUserId(char *accessPermit, 
				    char *resourceName , 
				    int user_id)
{
   char access_name[MAX_TOKEN];  
   int intval;

   if (strstr(accessPermit,"read") != NULL ||
       strstr(accessPermit,"execute") != NULL ) 
     strcpy( access_name,"read");
   else if (strstr(accessPermit,"write") != NULL) 
     strcpy(access_name,"write");
   else strcpy(access_name,accessPermit);
   

       sprintf(sqlq,"select t0.phy_rsrc_id from %sMDAS_AR_PHYSICAL t0, %sMDAS_AR_ACCS t1,  %sMDAS_TD_RSRC_ACCS t2,  %sMDAS_AU_GROUP t3  where  t0.phy_rsrc_name = '%s' AND t0.phy_rsrc_id = t1.rsrc_id AND   t3.user_id = %i AND t3.group_user_id = t1.user_id AND  t2.access_privilege like '%%%s%%' and t1.access_id >= t2.access_id",
	     MDASSCHEMENAME, MDASSCHEMENAME, MDASSCHEMENAME,MDASSCHEMENAME,
	     resourceName, user_id, access_name);
       tabledIsOkay = 1;
       intval = get_itype_value_from_query(sqlq) ;
       tabledIsOkay = 0;
       if (intval < 0) {
	 failure = NO_ACCS_TO_USER_IN_PHYSICAL_RESOURCE;
	 return (NO_ACCS_TO_USER_IN_PHYSICAL_RESOURCE);
       }
       return(intval);
}

int check_permit_in_dataid_by_access_id(int data_id, int user_id, int access_id)
{
  int i;

  sprintf(sqlq, "select distinct t1.data_id from %sMDAS_AD_REPL t4, %sMDAS_AD_ACCS t1, %sMDAS_AU_GROUP t3 where  t4.data_id = %i  AND t4.container_id = t1.data_id  AND t3.user_id = %i AND t3.group_user_id = t1.user_id AND t1.access_id >= %i",
             MDASSCHEMENAME, MDASSCHEMENAME,  data_id, user_id, access_id);
  if (check_exists(sqlq) != 0)  {
#ifdef DEBUGON
    elog(NOTICE, "Failurein check_permit_in_dataid : %s\n",sqlq);
#endif /* DEBUGON */
    failure = NO_DATA_ACCS_TO_USER;
    return (NO_DATA_ACCS_TO_USER);
  }
  return(MDAS_SUCCESS);
}

int
check_permit_in_dataname(char *obj_access_name, char *collName, char *dataName, int user_id)
{
  char access_name[MAX_TOKEN];
   int i;

     i = is_system_userid(user_id);
     if (i == 0)
       return(MDAS_SUCCESS);

     if (strstr(obj_access_name,"read") != NULL ||
	 strstr(obj_access_name,"execute") != NULL ) strcpy( access_name,"read");
     else if (strstr(obj_access_name,"write") != NULL)
       strcpy(access_name,"write");
     else strcpy(access_name,obj_access_name);

     sprintf(sqlq, "select distinct t2.access_constraint from %sMDAS_TD_DATA_GRP t4, %sMDAS_AD_REPL t5, %sMDAS_AD_ACCS t1, %sMDAS_TD_DS_ACCS t2, %sMDAS_AU_GROUP t3 where  t4.data_grp_name = '%s' AND t5.data_name = '%s' AND t4.data_grp_id = t5.data_grp_id AND t5.container_id = t1.data_id AND t3.user_id = %i AND t3.group_user_id = t1.user_id AND  t2.access_privilege like '%%%s%%' and t1.access_id >= t2.access_id",
	     MDASSCHEMENAME, MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	     collName, dataName, user_id, access_name);

	if (check_exists(sqlq) != 0) 
	  {
	    failure = NO_DATA_ACCS_TO_USER;
	    return (NO_DATA_ACCS_TO_USER);
	  }
	return(MDAS_SUCCESS);

     
}

int check_permit_in_dataid(char *obj_access_name, int data_id, int user_id)
{
   char access_name[MAX_TOKEN];  
   int i;

     i = is_system_userid(user_id);
     if (i == 0)
       return(MDAS_SUCCESS);
     if (strstr(obj_access_name,"read") != NULL ||
       strstr(obj_access_name,"execute") != NULL ) strcpy( access_name,"read");
   else if (strstr(obj_access_name,"write") != NULL) 
     strcpy(access_name,"write");
   else strcpy(access_name,obj_access_name);

     sprintf(sqlq, "select distinct t2.access_constraint from %sMDAS_AD_REPL t4, %sMDAS_AD_ACCS t1, %sMDAS_TD_DS_ACCS t2, %sMDAS_AU_GROUP t3 where  t4.data_id = %i  AND t4.container_id = t1.data_id AND t3.user_id = %i AND t3.group_user_id = t1.user_id AND  t2.access_privilege like '%%%s%%' and t1.access_id >= t2.access_id",
	     MDASSCHEMENAME, MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	     data_id, user_id, access_name);

	if (check_exists(sqlq) != 0) 
	  {
#ifdef DEBUGON
	    elog(NOTICE, "Failurein check_permit_in_dataid : %s\n",sqlq);
#endif /* DEBUGON */
	    failure = NO_DATA_ACCS_TO_USER;
	    return (NO_DATA_ACCS_TO_USER);
	  }
	return(MDAS_SUCCESS);

}

int check_permit_in_group(char *obj_access_name, char *group_id,
			  int user_id)
{
   char access_name[MAX_TOKEN];  
   int i;

     i = is_system_userid(user_id);
     if (i == 0)
       return(MDAS_SUCCESS);
     if (strstr(obj_access_name,"read") != NULL ||
       strstr(obj_access_name,"execute") != NULL ) strcpy( access_name,"read");
   else if (strstr(obj_access_name,"write") != NULL) 
     strcpy(access_name,"write");
   else strcpy(access_name,obj_access_name);

     /****     
     sprintf(sqlq, "select distinct t2.access_constraint from %sMDAS_AD_GRP_ACCS t1, %sMDAS_TD_DS_ACCS t2 where  t1.data_grp_id = '%s'  AND t1.user_id = %i AND  t2.access_constraint like '%%%s%%' and t1.access_id >= t2.access_id",
	     MDASSCHEMENAME, MDASSCHEMENAME,
	     group_id, user_id, access_name);
     ****/
     sprintf(sqlq, "select distinct t2.access_constraint from %sMDAS_AD_GRP_ACCS t1, %sMDAS_TD_DS_ACCS t2, %sMDAS_AU_GROUP t3 where  t1.data_grp_id = '%s'  AND t3.user_id = %i AND t3.group_user_id = t1.user_id AND  t2.access_privilege like '%%%s%%' and t1.access_id >= t2.access_id",
	     MDASSCHEMENAME, MDASSCHEMENAME,MDASSCHEMENAME,
	     group_id, user_id, access_name);

     /*printf("CPG1:%i:%s\n",check_exists(sqlq),sqlq);*/
	if (check_exists(sqlq) != 0) 
	  {
#ifdef DEBUGON
	    elog(NOTICE, "Failure to access collection: %s\n",sqlq);
#endif /* DEBUGON */
	    failure = NO_ACCS_TO_USER_IN_COLLECTION;
	    return (NO_ACCS_TO_USER_IN_COLLECTION);
	  }
	return(MDAS_SUCCESS);

}

int  get_next_in_hierarchy(char *parent_col_id, char *new_col_id,
			 char *tab_name, char* attr_name)
{
  char c1,c2, c3 , c4;

  /*** Added July 30, 2001 to avoid hierarchy contention ***/
    sprintf(sqlq, "lock table %s%s in exclusive mode", 
            MDASSCHEMENAME, tab_name);
    i = exec_no_res_sql_db2(henv, hdbc, (unsigned char *) sqlq);
    if (i != 0) {        
      sprintf(errmess,"get_next_in_hierarchy: locking error for %s. Error: %i",
	      tab_name, i);
      error_exit(errmess);
      failure = TABLE_LOCK_ERROR;
      commit_db2_interaction(MDAS_CAT_ROLLBACK);
      return (TABLE_LOCK_ERROR);
    }
  /*** Added July 30, 2001 to avoid hierarchy contention ***/

    sprintf(sqlq, "select t1.%s from %s%s t1 where t1.%s like '%s.____%%'  AND t1.%s not like '%s.____.%%' order by 1 DESC  ",
	  attr_name, MDASSCHEMENAME, tab_name, attr_name, 
	  parent_col_id,  attr_name, parent_col_id);
    /*printf("GNH1:%s\n",sqlq);*/
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	failure = MDAS_HIERARCHY_ERROR;
	/*** Added July 30, 2001 to avoid hierarchy contention ***/
	commit_db2_interaction(MDAS_CAT_ROLLBACK);
	/*** Added July 30, 2001 to avoid hierarchy contention ***/
	return (MDAS_HIERARCHY_ERROR);
      }
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	sprintf(new_col_id,"%s.0001", parent_col_id);
      }
    else       {
	truncate_end_blanks(data_size[0], data[0]);
	strcpy(new_col_id, (char *) data[0]);
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	
	c1 = new_col_id[strlen(new_col_id) - 1];
	c2 = new_col_id[strlen(new_col_id) - 2];
	c3 = new_col_id[strlen(new_col_id) - 3];
	c4 = new_col_id[strlen(new_col_id) - 4];
	
	if (c1 >= '0' && c1 < '9')
	  c1 = c1 + 1;
	else if (c1 == '9')
	  c1 = 'a';
	else if (c1 >= 'a' && c1 < 'z')
	  c1 = c1 + 1;
	else if (c1 == 'z')
	  /*	  c1 = 'A';
	else if (c1 >= 'A' && c1 < 'Z')
	  c1 = c1 + 1;
	else if (c1 == 'Z') */

	  {
	    c1 = '0';
	    if (c2 >= '0' && c2 < '9')
	      c2 = c2 + 1;
	    else if (c2 == '9')
	      c2 = 'a';
	    else if (c2 >= 'a' && c2 < 'z')
	      c2 = c2 + 1;
	    else if (c2 == 'z')
	      /* c2 = 'A';
	    else if (c2 >= 'A' && c2 < 'Z')
	      c2 = c2 + 1;
	    else if (c2 == 'Z') */
	      {
		c2 = '0';
		if (c3 >= '0' && c3 < '9')
		  c3 = c3 + 1;
		else if (c3 == '9')
		  c3 = 'a';
		else if (c3 >= 'a' && c3 < 'z')
		  c3 = c3 + 1;
		else if (c3 == 'z')
		  /* c3 = 'A';
		else if (c3 >= 'A' && c3 < 'Z')
		  c3 = c3 + 1;
		else if (c3 == 'Z')  */
		  {
		    c3 = '0';
		    if (c4 >= '0' && c4 < '9')
		      c4 = c4 + 1;
		    else if (c4 == '9')
		      c4 = 'a';
		    else if (c4 >= 'a' && c4 < 'z')
		      c4 = c4 + 1;
		    else if (c4 == 'z')
		      /* c4 = 'A';
		    else if (c4 >= 'A' && c4 < 'Z')
		      c4 = c4 + 1;
		      else if (c4 == 'Z')  */ {
		      /*** Added July 30, 2001 to avoid hierarchy contention ***/
		      commit_db2_interaction(MDAS_CAT_ROLLBACK);
		      /*** Added July 30, 2001 to avoid hierarchy contention ***/
		      return (MDAS_HIERARCHY_OVERFLOW_ERROR);
		    }
		  }
	      }
	  }
      

    new_col_id[strlen(new_col_id) - 1] =  c1;
    new_col_id[strlen(new_col_id) - 2] =  c2;
    new_col_id[strlen(new_col_id) - 3] =  c3;
    new_col_id[strlen(new_col_id) - 4] =  c4;
    }
    return(MDAS_SUCCESS);
}




int make_new_collection_path(int cat_type, char *parent_col_name, 
			char *col_name_new,
		    char *user_name, char *user_domain)
{

  char tmpCollName[MAX_TOKEN];
  char remCollName[MAX_TOKEN];
  int i;
  int j = 10;
  int k, l;

  if (strlen(col_name_new) == 0)
    return(COLLECTION_NOT_IN_CAT);
  k = (int)  getpid();
  sprintf(sqlq, "select data_grp_id from %s%s where data_grp_name = '%s'",
	  MDASSCHEMENAME,"MDAS_TD_DATA_GRP",parent_col_name);
  if (check_exists(sqlq) != 0) {
    splitbychar(parent_col_name, tmpCollName, remCollName,'/');
    i = make_new_collection_path(cat_type,tmpCollName, remCollName,
			     user_name, user_domain);
    if (i != 0)
      return(i);
  }
  j = 10;
  while (j  > 0) {
    i = make_new_collection(cat_type, parent_col_name, 
			    col_name_new, user_name, user_domain);
    if (i >= 0) 
      return (i);
    if (i == DATA_SUBCOLLECTION_NOT_UNIQUE)
      return(0);
    j--;
    for (l  =  0; l < k ; l++)
       getpid();
  }
    return(i);
}

int make_new_collection(int cat_type, char *parent_col_name, 
			char *col_name_new,
		    char *user_name, char *user_domain)
{
  int user_id;
   strcpy(reg_registrar_name,user_name);
   user_id = get_user_id_in_domain(user_name, user_domain);
   if (user_id < 0) { return (USER_NOT_IN_DOMN); }
   return( make_new_collection_by_id(cat_type,parent_col_name, 
			     col_name_new,user_id));
}
   

int make_new_collection_by_id(int cat_type, char *parent_col_name, 
			char *col_name_new_in, int user_id)
{
   char parent_col_id[MAX_TOKEN];
   char new_col_id[MAX_TOKEN];
   int domain_id ;
   char new_col_name[MAX_TOKEN];
   char col_name_new[MAX_TOKEN];

    if (col_name_new_in[0] == '/')
      strcpy(col_name_new, (char *)(col_name_new_in + 1));
    else
      strcpy(col_name_new,col_name_new_in);

    failure = 0;
    if (strcmp(parent_col_name,"/"))
      sprintf(new_col_name,"%s/%s",parent_col_name,col_name_new);
    else
      sprintf(new_col_name,"/%s",col_name_new);

   strcpy(reg_obj_name, new_col_name);
   strcpy(reg_obj_code,"MDAS_CD_USER");
   strcpy(reg_action_name,"create collection");
   reg_registrar_id = user_id;
   reg_obj_repl_enum = 0;
   get_time(reg_timeval);
   reg_obj_id = 0;


   tabledIsOkay = 1;
   get_ctype_value(parent_col_id, "MDAS_TD_DATA_GRP.data_grp_name", 
		    parent_col_name);
   tabledIsOkay = 0;
    if (failure != 0) { return (COLLECTION_NOT_IN_CAT);}

    /*printf("PARCOLNM=%s, PARCOLID=%s\n",parent_col_name,parent_col_id);*/

    i = check_permit_in_group("write", parent_col_id, 
			      user_id);
    if (i !=0 && isUserDomainAdminFlag != 1)  { return (i);}
    failure = 0;
  sprintf(sqlq, "select t1.data_grp_id from %sMDAS_TD_DATA_GRP t1 where t1.data_grp_id like '%s.____%%'  AND t1.data_grp_id not like '%s.____.%%' AND t1.data_grp_name = '%s'",
	  MDASSCHEMENAME, 
	  parent_col_id,  parent_col_id, new_col_name);
  /*printf("MNC1:%i:%s\n",check_exists(sqlq),sqlq);*/
	if (check_exists(sqlq) == 0) 
	  {
	    failure = DATA_SUBCOLLECTION_NOT_UNIQUE;
	    return (DATA_SUBCOLLECTION_NOT_UNIQUE);
	  }
	
   i = get_next_in_hierarchy(parent_col_id,new_col_id,
			 "MDAS_TD_DATA_GRP", "data_grp_id");
   if (i != 0) {
     return (i);
   }
   /*printf("MNC2:%i:%s",i, new_col_id);*/

#ifdef FED_MCAT
     sprintf(sqlq, "insert into %s%s (DATA_GRP_ID, DATA_GRP_NAME,PARENT_GRP_NAME,COLL_OWNER,COLL_CR_TIMESTAMP,COLL_COMMENTS,COLL_MOD_TIMESTAMP) values ('%s','%s', '%s', %i,'%s','','%s')",
	  MDASSCHEMENAME, "MDAS_TD_DATA_GRP", new_col_id, new_col_name, 
	     parent_col_name, user_id,reg_timeval,reg_timeval);
#else
     sprintf(sqlq, "insert into %s%s (DATA_GRP_ID, DATA_GRP_NAME,PARENT_GRP_NAME,COLL_OWNER,COLL_CR_TIMESTAMP,COLL_COMMENTS) values ('%s','%s', '%s', %i,'%s','')",
	  MDASSCHEMENAME, "MDAS_TD_DATA_GRP", new_col_id, new_col_name, 
	     parent_col_name, user_id,reg_timeval);
#endif
  res = ExecSqlDb2(henv, hdbc, sqlq);
  if (res != 0) 
    { sprintf(query_string,
	      "user registry API: DATA COLLECTION insertion error:%s", sqlq);
    /**error_exit(query_string);**/
     /*** Added July 30, 2001 to avoid hierarchy contention ***/
     commit_db2_interaction(MDAS_CAT_ROLLBACK);
     /*** Added July 30, 2001 to avoid hierarchy contention ***/
    return(MDAS_TD_DATA_GRP_INSERTION_ERROR);
    }
  /*** Added July 30, 2001 to avoid hierarchy contention ***/
  /*** relocated to below ****/
  /*** Added July 30, 2001 to avoid hierarchy contention ***/
  failure = 0;
  tabledIsOkay = 1;
  intval  = get_itype_value("MDAS_TD_DS_ACCS.access_constraint","all");
  tabledIsOkay = 0;
  if (failure != 0) {return (MDAS_TD_DS_ACCS_ACCESS_ERROR);}
			    
     sprintf(sqlq, "insert into %s%s values ('%s',%i,%i)",
	  MDASSCHEMENAME, "MDAS_AD_GRP_ACCS", new_col_id, user_id,intval);
  res = ExecSqlDb2(henv, hdbc, sqlq);
  if (res != 0) 
    {error_exit("user registry API: DATA COLLECTION insertion error");
    return(MDAS_AD_GRP_ACCS_INSERTION_ERROR);}
  sprintf(sqlq,"%s/%s",parent_col_name,col_name_new);
  res = inheritACLWithSticky(parent_col_id,0,0,new_col_id,user_id,"single-collection-register");
  if (res != 0)
    return(res);
  performAudit(C_CREATE,user_id,0,new_col_id,"");
  sprintf(query_string,
          "insert into %s%s (data_grp_id,metadatanum) values ('%s',-1)",MDASSCHEMENAME,"MDAS_AD_COLLMDATA", new_col_id);
  res = ExecSqlDb2(henv, hdbc, query_string);
  if (res != 0)
    return(MDAS_AD_COLLMDATA_INSERTION_ERROR);
  commit_db2_interaction(MDAS_CAT_COMMIT);
  return (MDAS_SUCCESS);
}


int get_collections(int cat_type, char * col_name, char *flag,
		    mdasC_sql_result_struct  **myresult, int numrows)
{
  char fstr1[MAX_TOKEN], fstr2[MAX_TOKEN];
  char col_id[MAX_TOKEN];
  char *grp_name;
  mdasC_sql_result_struct *newres;
  char like_col_name[MAX_TOKEN * 2];
  int cnt; 
    failure = 0;
   strcpy(reg_obj_code,"NOOBJ");

   strcpy(like_col_name, col_name);
   mcatEscapeWildCards(like_col_name);
#ifdef MCAT_VERSION_10
  if (strstr(flag, "R") == NULL)
    { 
      sprintf(sqlq, 
	      "(select data_grp_name as alpha, data_grp_id as beta, 'A' from %sMDAS_TD_DATA_GRP  where ((data_grp_name like '%s %%' OR data_grp_name like '%s/%%') AND data_grp_name not like  '%s/%%/%%' AND data_grp_name not like '%%$deleted%%') union select '  '|| t1.data_name as alpha,  t2.data_grp_id as beta, 'B' from %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where ( t1.data_grp_id=t2.data_grp_id and t2.data_grp_name  like '%s %%'  AND t2.data_grp_name not like  '%s/%%'  AND   t1.data_name not like '%%$deleted%%') )  order by 2 ASC, 3 ASC",  
	      MDASSCHEMENAME, like_col_name, like_col_name, like_col_name,
	      MDASSCHEMENAME, MDASSCHEMENAME,  like_col_name,  like_col_name);
    }
  else 
    { 
      sprintf(sqlq, 
	      "(select data_grp_name as alpha, data_grp_id as beta, 'A'  from %sMDAS_TD_DATA_GRP  where ((data_grp_name like '%s %%' OR data_grp_name like '%s/%%') AND data_grp_name not like '%%$deleted%%')  union select  '  '|| t1.data_name as alpha,  t2.data_grp_id as beta, 'B'  from %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where t1.data_grp_id=t2.data_grp_id and ((t2.data_grp_name  like '%s %%' OR t2.data_grp_name like '%s/%%')  AND t2.data_grp_name not like '%%$deleted%%' AND   t1.data_name not like '%%$deleted%%'))  order by 2 ASC, 3 ASC",  
	      MDASSCHEMENAME,  like_col_name,  like_col_name, 
	      MDASSCHEMENAME, MDASSCHEMENAME,  like_col_name, like_col_name);
    }
#else
  if (strstr(flag, "R") == NULL)
    { 
      sprintf(sqlq, 
	      "(select data_grp_name as alpha, data_grp_id as beta, 'A' from %sMDAS_TD_DATA_GRP  where ((data_grp_name like '%s' OR data_grp_name like '%s/%%') AND data_grp_name not like  '%s/%%/%%' AND data_grp_name not like '%%$deleted%%') union select '  '|| t1.data_name as alpha,  t2.data_grp_id as beta, 'B' from %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where ( t1.data_grp_id=t2.data_grp_id and t2.data_grp_name  like '%s'  AND t2.data_grp_name not like  '%s/%%'  AND   t1.data_name not like '%%$deleted%%') )  order by 1 ASC, 3 ASC",  
	      MDASSCHEMENAME, like_col_name, like_col_name, like_col_name,
	      MDASSCHEMENAME, MDASSCHEMENAME,  like_col_name,  like_col_name);
    }
  else 
    { 
      sprintf(sqlq, 
	      "(select data_grp_name as alpha, data_grp_id as beta, 'A'  from %sMDAS_TD_DATA_GRP  where ((data_grp_name like '%s' OR data_grp_name like '%s/%%') AND data_grp_name not like '%%$deleted%%')  union select  '  '|| t1.data_name as alpha,  t2.data_grp_id as beta, 'B'  from %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where t1.data_grp_id=t2.data_grp_id and ((t2.data_grp_name  like '%s' OR t2.data_grp_name like '%s/%%')  AND t2.data_grp_name not like '%%$deleted%%' AND   t1.data_name not like '%%$deleted%%'))  order by 1 ASC, 3 ASC",  
	      MDASSCHEMENAME,  like_col_name,  like_col_name, 
	      MDASSCHEMENAME, MDASSCHEMENAME,  like_col_name, like_col_name);
    }
  /*** beta changed to alpha in above  Nov 17,2000 ****/
#endif
  /*printf("GC:%s\n",sqlq); */
    i = exec_sql_db2(henv, hdbc,  &hstmt, (unsigned char *) sqlq);
    if (i != 0) return (MDAS_FAILURE);
    i = get_no_of_columns_db2(hstmt, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = bind_default_row_db2(hstmt, data,  data_size, &num_of_cols);
    if (i != 0) return (MDAS_FAILURE);
    i = get_row_db2(hstmt);
    if (i != 0) 
      {
        i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	failure =  COLLECTION_NOT_IN_CAT;
	/*printf("GC:%s\n",sqlq);*/
	return (COLLECTION_NOT_IN_CAT);
      }
    k = 0;
    cnt = 0;
    grp_name=  (char *) malloc(numrows * MAX_DATA_SIZE * sizeof(char));

    while (i == 0 && cnt < (numrows-1) )
      {
	truncate_end_blanks(data_size[0], data[0]);
 	sprintf((char *)(((int) (grp_name)) + (k * MAX_DATA_SIZE)),
                      "%s", data[0]);
	strcpy((char *)data[0],"");
        i = get_row_db2(hstmt);
        k++;
        cnt++;
      }
    if (i == 0)
      {
	truncate_end_blanks(data_size[0], data[0]);
 	sprintf((char *)(((int) (grp_name)) + (k * MAX_DATA_SIZE)),
                      "%s", data[0]);
	k++;
	cnt++;
      }
    /*
    sprintf((char *)(((int) (grp_name))+ (k * MAX_DATA_SIZE)),"%s",RESWANTED);
    */
    newres = (mdasC_sql_result_struct *) 
      malloc(sizeof(mdasC_sql_result_struct));
 	if (newres == NULL) return(MEMORY_ALLOCATION_ERROR);
    newres->sqlresult[0].values = grp_name;
    newres->sqlresult[0].tab_name =dcs_tname[15];
    newres->sqlresult[0].att_name =dcs_aname[15] ;
    
    if (cnt >= numrows)
      {
	for (i =  0; i < num_of_cols; i++){
	  free(data[i]);
	}
	for (i = 0; i < MAX_QUERY_LIMIT; i++)
	  {
	    if (queryCont[i].id == 0) break;
	  }
	queryCont[i].id = 1;
	queryCont[i].hstmt =  hstmt;
	queryCont[i].colcount = 1;
	queryCont[i].sqlselect[0].tab_name =newres->sqlresult[0].tab_name;
	queryCont[i].sqlselect[0].att_name =newres->sqlresult[0].att_name;
	newres->continuation_index = i;
      }
    else
      {
	i = done_with_default_result_db2(hstmt,data,data_size, num_of_cols);
	newres->continuation_index = -1;
      }
    newres->result_count = 1;
    newres->row_count = cnt;
    rmGlbFromResult (newres);
    *myresult = newres;

    return(MDAS_SUCCESS);
}



int
close_db2_interaction(int transaction_end_code)

{ 
  return(close_db2_interaction_internal(henv, hdbc,
	       transaction_end_code));
}

int open_db2_interaction(int cat_type)
{
  int i;
  failure = 0; 
  if (cat_type != MDAS_CATALOG) 
    return (MDAS_CATALOG_TYPE_ERROR);
  /**  srandom((unsigned int) getpid()); ***/
  i = open_db2_interaction_internal(&henv, &hdbc);
  if (i == MDAS_SUCCESS)
    read_ext_tab_attr_index(cat_type,"");
  return(i);
}

int commit_db2_interaction(int  transaction_end_code)
{
  return(commit_db2_interaction_internal(
		   henv, hdbc,transaction_end_code));

}

int
insertMultiOwner(int registrarId, char *data_group_id, int userId, char *acType)
{
  int i,j,a, res;

  if (!strcmp(acType,"D")) {
    sprintf(sqlq,"update %sMDAS_AD_REPL set data_owner = %i where data_grp_id = '%s' and  data_owner = %i ",
	    MDASSCHEMENAME,  userId,  data_group_id, registrarId);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_ACCS_UPDATE_ERROR);

    sprintf(sqlq,"update %sMDAS_TD_DATA_GRP set coll_owner = %i where  data_grp_id = '%s' and  coll_owner = %i",
	    MDASSCHEMENAME, userId,  data_group_id,  registrarId);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_GRP_ACCS_UPDATE_ERROR);
  }
  else if (!strcmp(acType,"R")) {
    sprintf(sqlq,"update %sMDAS_AD_REPL set data_owner = %i where data_grp_id like '%s%%'  and  data_owner = %i ",
	    MDASSCHEMENAME,  userId,  data_group_id, registrarId);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_ACCS_UPDATE_ERROR);
    sprintf(sqlq,"update %sMDAS_TD_DATA_GRP set coll_owner = %i where  data_grp_id like '%s%%' and  coll_owner = %i",
	    MDASSCHEMENAME, userId,  data_group_id,  registrarId);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_GRP_ACCS_UPDATE_ERROR);
  }
  else {
    return(ACTION_NOT_ALLOWED);
  }
  return(MDAS_SUCCESS);
}

int
insertMultiAccs(int registrarId, char *data_group_id, int userId, int accessId, char *acType)
{
  int i,j,a, res;

#if MYSMCAT
  char mys_buf[MAX_TOKEN];
#endif

  a = getAccessId("all audit");

  if (!strcmp(acType,"D")) {

#if MYSMCAT
    sprintf(sqlq,"select A.data_id from %sMDAS_AD_REPL A, %sMDAS_AD_ACCS B where A.data_grp_id = '%s' and  A.data_id = B.data_id and A.repl_enum = B.repl_enum and B.user_id = %i and B.access_id >= %i", MDASSCHEMENAME,MDASSCHEMENAME, data_group_id,  registrarId, a );
    if(0 != get_ctype_value_from_query(mys_buf, sqlq))
	return(MDAS_AD_ACCS_UPDATE_ERROR);
    sprintf(sqlq,"update %sMDAS_AD_ACCS Z set Z.access_id = %i where  Z.user_id = %i and Z.data_id in ('%s')", MDASSCHEMENAME, accessId, userId, mys_buf);
#else
    sprintf(sqlq,"update %sMDAS_AD_ACCS set access_id = %i\
 where  user_id = %i and data_id in (select A.data_id \
 from %sMDAS_AD_REPL A, %sMDAS_AD_ACCS B \
 where A.data_grp_id = '%s' and  A.data_id = B.data_id and \
 A.repl_enum = B.repl_enum and B.user_id = %i and B.access_id >= %i )",
	    MDASSCHEMENAME, accessId, userId, MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a );
    /*
    sprintf(sqlq,"update %sMDAS_AD_ACCS Z set Z.access_id = %i\
 where  Z.user_id = %i and Z.data_id in (select A.data_id \
 from %sMDAS_AD_REPL A, %sMDAS_AD_ACCS B \
 where A.data_grp_id = '%s' and  A.data_id = B.data_id and \
 A.repl_enum = B.repl_enum and B.user_id = %i and B.access_id >= %i )",
	    MDASSCHEMENAME, accessId, userId, MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a );
    */
#endif
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_ACCS_UPDATE_ERROR);

    sprintf(sqlq,"insert into %sMDAS_AD_ACCS \
 select A.data_id, A.repl_enum, %i,%i from %sMDAS_AD_REPL A, %sMDAS_AD_ACCS B \
 where A.data_grp_id = '%s' and  A.data_id = B.data_id and \
 A.repl_enum = B.repl_enum and B.user_id = %i and B.access_id >= %i and \
 not exists (select C.user_id from %sMDAS_AD_ACCS C \
 where A.data_id = C.data_id and A.repl_enum = C.repl_enum and \
 C.user_id = %i ) ",
	    MDASSCHEMENAME, userId, accessId, MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a,MDASSCHEMENAME, userId);

    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_ACCS_INSERTION_ERROR);
#if MYSMCAT
    sprintf(sqlq,"select A.data_grp_id from %sMDAS_TD_DATA_GRP A, %sMDAS_TD_DATA_GRP C, %sMDAS_AD_GRP_ACCS B where C.data_grp_id = '%s' and  C.data_grp_name = A.parent_grp_name and A.data_grp_id = B.data_grp_id and  B.user_id = %i and B.access_id >= %i", MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME, data_group_id,  registrarId, a );
    if(0 != get_ctype_value_from_query(mys_buf, sqlq))
      return(MDAS_AD_GRP_ACCS_UPDATE_ERROR);
    sprintf(sqlq,"update %sMDAS_AD_GRP_ACCS Z set Z.access_id = %i where  Z.user_id = %i and Z.data_grp_id in ('%s')", MDASSCHEMENAME, accessId, userId, mys_buf);
#else
    sprintf(sqlq,"update %sMDAS_AD_GRP_ACCS set access_id = %i\
 where  user_id = %i and data_grp_id in (select A.data_grp_id \
 from %sMDAS_TD_DATA_GRP A, %sMDAS_TD_DATA_GRP C, %sMDAS_AD_GRP_ACCS B \
 where C.data_grp_id = '%s' and  C.data_grp_name = A.parent_grp_name and \
 A.data_grp_id = B.data_grp_id and  B.user_id = %i and B.access_id >= %i )",
	    MDASSCHEMENAME, accessId, userId, 
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a );
    /*
    sprintf(sqlq,"update %sMDAS_AD_GRP_ACCS Z set Z.access_id = %i\
 where  Z.user_id = %i and Z.data_grp_id in (select A.data_grp_id \
 from %sMDAS_TD_DATA_GRP A, %sMDAS_TD_DATA_GRP C, %sMDAS_AD_GRP_ACCS B \
 where C.data_grp_id = '%s' and  C.data_grp_name = A.parent_grp_name and \
 A.data_grp_id = B.data_grp_id and  B.user_id = %i and B.access_id >= %i )",
	    MDASSCHEMENAME, accessId, userId, 
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a );
    */
#endif
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_GRP_ACCS_UPDATE_ERROR);

    sprintf(sqlq,"insert into %sMDAS_AD_GRP_ACCS \
 select A.data_grp_id, %i,%i from %sMDAS_TD_DATA_GRP A, %sMDAS_TD_DATA_GRP D, \
 %sMDAS_AD_GRP_ACCS B where D.data_grp_id = '%s' and \
 D.data_grp_name = A.parent_grp_name and A.data_grp_id = B.data_grp_id and \
 B.user_id = %i and B.access_id >= %i and \
 not exists (select C.user_id from %sMDAS_AD_GRP_ACCS C \
 where A.data_grp_id = C.data_grp_id and C.user_id = %i ) ",
	    MDASSCHEMENAME, userId, accessId, 
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a,MDASSCHEMENAME, userId);
   
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_GRP_ACCS_INSERTION_ERROR);

  }
  else if (!strcmp(acType,"R")) {
#if MYSMCAT
    sprintf(sqlq,"select A.data_id from %sMDAS_AD_REPL A, %sMDAS_AD_ACCS B where A.data_grp_id like '%s%%' and  A.data_id = B.data_id and A.repl_enum = B.repl_enum and B.user_id = %i and B.access_id >= %i", MDASSCHEMENAME,MDASSCHEMENAME, data_group_id,  registrarId, a);
    if(0 != get_ctype_value_from_query(mys_buf, sqlq))
      return(MDAS_AD_ACCS_UPDATE_ERROR);
    sprintf(sqlq,"update %sMDAS_AD_ACCS Z set Z.access_id = %i where  Z.user_id = %i and Z.data_id in ('%s')", MDASSCHEMENAME, accessId, userId, mys_buf);
#else
    sprintf(sqlq,"update %sMDAS_AD_ACCS set access_id = %i\
 where  user_id = %i and data_id in (select A.data_id \
 from %sMDAS_AD_REPL A, %sMDAS_AD_ACCS B \
 where A.data_grp_id like '%s%%' and  A.data_id = B.data_id and \
 A.repl_enum = B.repl_enum and B.user_id = %i and B.access_id >= %i )",
	    MDASSCHEMENAME, accessId, userId, MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a );
    /*
    sprintf(sqlq,"update %sMDAS_AD_ACCS Z set Z.access_id = %i\
 where  Z.user_id = %i and Z.data_id in (select A.data_id \
 from %sMDAS_AD_REPL A, %sMDAS_AD_ACCS B \
 where A.data_grp_id like '%s%%' and  A.data_id = B.data_id and \
 A.repl_enum = B.repl_enum and B.user_id = %i and B.access_id >= %i )",
	    MDASSCHEMENAME, accessId, userId, MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a );
    */
#endif
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_ACCS_UPDATE_ERROR);

    sprintf(sqlq,"insert into %sMDAS_AD_ACCS \
 select A.data_id, A.repl_enum, %i,%i from %sMDAS_AD_REPL A, %sMDAS_AD_ACCS B \
 where A.data_grp_id like '%s%%' and  A.data_id = B.data_id and \
 A.repl_enum = B.repl_enum and B.user_id = %i and B.access_id >= %i and \
 not exists (select C.user_id from %sMDAS_AD_ACCS C \
 where A.data_id = C.data_id and A.repl_enum = C.repl_enum and \
 C.user_id = %i ) ",
	    MDASSCHEMENAME, userId, accessId, MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a,MDASSCHEMENAME, userId);

    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_ACCS_INSERTION_ERROR);

#if MYSMCAT
    sprintf(sqlq,"select A.data_grp_id from %sMDAS_TD_DATA_GRP A, %sMDAS_TD_DATA_GRP C, %sMDAS_AD_GRP_ACCS B where C.data_grp_id like '%s%%' and  C.data_grp_name = A.parent_grp_name and A.data_grp_id = B.data_grp_id and  B.user_id = %i and B.access_id >= %i", MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME, data_group_id,  registrarId, a );
    if(0 != get_ctype_value_from_query(mys_buf, sqlq))
      return(MDAS_AD_GRP_ACCS_UPDATE_ERROR);
    sprintf(sqlq,"update %sMDAS_AD_GRP_ACCS Z set Z.access_id = %i where  Z.user_id = %i and Z.data_grp_id in ('%s')", MDASSCHEMENAME, accessId, userId, mys_buf); 
#else
    sprintf(sqlq,"update %sMDAS_AD_GRP_ACCS  set access_id = %i\
 where  user_id = %i and data_grp_id in (select A.data_grp_id \
 from %sMDAS_TD_DATA_GRP A, %sMDAS_TD_DATA_GRP C, %sMDAS_AD_GRP_ACCS B \
 where C.data_grp_id like '%s%%' and  C.data_grp_name = A.parent_grp_name and \
 A.data_grp_id = B.data_grp_id and  B.user_id = %i and B.access_id >= %i )",
	    MDASSCHEMENAME, accessId, userId, 
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a );
    /*
    sprintf(sqlq,"update %sMDAS_AD_GRP_ACCS Z set Z.access_id = %i\
 where  Z.user_id = %i and Z.data_grp_id in (select A.data_grp_id \
 from %sMDAS_TD_DATA_GRP A, %sMDAS_TD_DATA_GRP C, %sMDAS_AD_GRP_ACCS B \
 where C.data_grp_id like '%s%%' and  C.data_grp_name = A.parent_grp_name and \
 A.data_grp_id = B.data_grp_id and  B.user_id = %i and B.access_id >= %i )",
	    MDASSCHEMENAME, accessId, userId, 
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a );
    */
#endif
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_GRP_ACCS_UPDATE_ERROR);


    sprintf(sqlq,"insert into %sMDAS_AD_GRP_ACCS \
 select A.data_grp_id, %i,%i from %sMDAS_TD_DATA_GRP A, %sMDAS_TD_DATA_GRP D, \
 %sMDAS_AD_GRP_ACCS B where D.data_grp_id like '%s%%' and \
 D.data_grp_name = A.parent_grp_name and A.data_grp_id = B.data_grp_id and \
 B.user_id = %i and B.access_id >= %i and \
 not exists (select C.user_id from %sMDAS_AD_GRP_ACCS C \
 where A.data_grp_id = C.data_grp_id and C.user_id = %i ) ",
	    MDASSCHEMENAME, userId, accessId, 
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a,MDASSCHEMENAME, userId);
   
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_GRP_ACCS_INSERTION_ERROR);

    /*** added to take care of the given collection also ***/
    sprintf(sqlq,"select * from %sMDAS_AD_GRP_ACCS where data_grp_id = '%s' and user_id = %i", MDASSCHEMENAME, data_group_id,userId);
    if (check_exists(sqlq) == 0) {
      sprintf(sqlq,"update %sMDAS_AD_GRP_ACCS set access_id = %i where data_grp_id = '%s' and user_id = %i", MDASSCHEMENAME, accessId, data_group_id,userId);
    }
    else {
      sprintf(sqlq,"insert into %sMDAS_AD_GRP_ACCS values ('%s',%i,%i) ",
	     MDASSCHEMENAME,data_group_id, userId, accessId);
    }
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_GRP_ACCS_INSERTION_ERROR);

    /*** added to take care of the given collection also ***/
  }
  else {
    return(ACTION_NOT_ALLOWED);
  }
  return(MDAS_SUCCESS);
}


int
deleteMultiAccs(int registrarId, char *data_group_id, int userId, int accessId, char *acType)
{
  int i,j,a, res;

  a = getAccessId("all audit");

  if (!strcmp(acType,"D")) {
    sprintf(sqlq,"delete from %sMDAS_AD_ACCS \
 where  user_id = %i and data_id in (select A.data_id \
 from %sMDAS_AD_REPL A, %sMDAS_AD_ACCS B \
 where A.data_grp_id = '%s' and  A.data_id = B.data_id and \
 A.repl_enum = B.repl_enum and B.user_id = %i and B.access_id >= %i )",
	    MDASSCHEMENAME,  userId, MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a );

    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_ACCS_UPDATE_ERROR);

    sprintf(sqlq,"delete from %sMDAS_AD_GRP_ACCS \
 where  user_id = %i and data_grp_id in (select A.data_grp_id \
 from %sMDAS_TD_DATA_GRP A, %sMDAS_TD_DATA_GRP C, %sMDAS_AD_GRP_ACCS B \
 where C.data_grp_id = '%s' and  C.data_grp_name = A.parent_grp_name and \
 A.data_grp_id = B.data_grp_id and  B.user_id = %i and B.access_id >= %i )",
	    MDASSCHEMENAME,  userId, 
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a );

    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_GRP_ACCS_UPDATE_ERROR);

  }
  else if (!strcmp(acType,"R")) {
    sprintf(sqlq,"delete from %sMDAS_AD_ACCS  \
 where  user_id = %i and data_id in (select A.data_id \
 from %sMDAS_AD_REPL A, %sMDAS_AD_ACCS B \
 where A.data_grp_id like '%s%%' and  A.data_id = B.data_id and \
 A.repl_enum = B.repl_enum and B.user_id = %i and B.access_id >= %i )",
	    MDASSCHEMENAME, userId, MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a );

    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_ACCS_UPDATE_ERROR);

    sprintf(sqlq,"delete from %sMDAS_AD_GRP_ACCS  \
 where  user_id = %i and data_grp_id in (select A.data_grp_id \
 from %sMDAS_TD_DATA_GRP A, %sMDAS_TD_DATA_GRP C, %sMDAS_AD_GRP_ACCS B \
 where C.data_grp_id like '%s%%' and  C.data_grp_name = A.parent_grp_name and \
 A.data_grp_id = B.data_grp_id and  B.user_id = %i and B.access_id >= %i )",
	    MDASSCHEMENAME,  userId, 
	    MDASSCHEMENAME,MDASSCHEMENAME,MDASSCHEMENAME,
	    data_group_id,  registrarId, a );

    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_GRP_ACCS_UPDATE_ERROR);

    /*** added to take care of the given collection also ***/
    sprintf(sqlq,"delete from  %sMDAS_AD_GRP_ACCS where data_grp_id = '%s' and user_id = %i", MDASSCHEMENAME,  data_group_id,userId);
    res = ExecSqlDb2(henv, hdbc, sqlq);
    if (res != 0) 
      return(MDAS_AD_GRP_ACCS_INSERTION_ERROR);

    /*** added to take care of the given collection also ***/

  }
  else {
    return(ACTION_NOT_ALLOWED);
  }
  return(MDAS_SUCCESS);
}

int mcatEscapeWildCards(char *dname)
{
  char oldname[MAX_OBJ_NAME_LENGTH];
  int i,j, len;
  int like_char = 0;

  strcpy(oldname, dname);
  len = strlen(oldname);
  for (i = 0,j = 0 ; i < len ; i++, j++)
    {
      switch (oldname[i])
	{
	   case '_' :
	   case '\\' :
	   case '%' :
		       dname[j] = '\\';
		       j++;
	              dname[j] = oldname[i];
		      break;
	   default  :
	              dname[j] = oldname[i];
		      break;
	}         
    }
  dname[j] = '\0';
  return (like_char);
}

int
bulkInsertUdefMetadataForColl(char *data_group_id,
			      char *metadataString,
			      char *separator, char *modDate)
{
  int i,ii, mdata_maxnum;
  char *strPtr, *strPtr2;

  sprintf(sqlq,"select data_grp_id from %sMDAS_AD_COLLMDATA where  data_grp_id = '%s' ", 
	  MDASSCHEMENAME, data_group_id);
  if (check_exists(sqlq) == 0) {
    sprintf(sqlq,
	    "select max(metadatanum) from %sMDAS_AD_COLLMDATA where data_grp_id = '%s' ", 
	    MDASSCHEMENAME,  data_group_id);
    mdata_maxnum = get_itype_value_from_query(sqlq) ;
    if (mdata_maxnum < 0) {
      if (mdata_maxnum != -1)
	return (mdata_maxnum);
      sprintf(sqlq,"delete from %sMDAS_AD_COLLMDATA where data_grp_id =  '%s' and metadatanum = -1",
	      MDASSCHEMENAME, data_group_id);
      ExecSqlDb2(henv, hdbc, sqlq);
    }
    mdata_maxnum++;
  }
  else {
    mdata_maxnum =  0;
  }
  strPtr = metadataString;
  strPtr2 = NULL;

  get_time(cval[5]);
  sprintf(atname[0],"data_grp_id");
  sprintf(atname[1],"metadatanum");
  sprintf(atname[2],"userdef_modif_date");
  sprintf(atname[3],"userdef_creat_date");
  sprintf(atname[4],"userdef_metastr0");
  sprintf(atname[5],"userdef_metastr1");
  sprintf(atname[6],"userdef_metastr2");
  sprintf(atname[7],"userdef_metastr3");
  sprintf(atname[8],"userdef_metastr4");
  sprintf(atname[9],"userdef_metastr5");
  sprintf(atname[10],"userdef_metastr6");
  sprintf(atname[11],"userdef_metastr7");
  sprintf(atname[12],"userdef_metastr8");
  sprintf(atname[13],"userdef_metastr9");
  sprintf(atname[14],"userdef_metaint0");
  sprintf(atname[15],"userdef_metaint1");
  sprintf(atval[0],"'%s'",data_group_id);
  if (modDate != NULL && strlen(modDate) > 0)
    sprintf(atval[2],"'%s'",modDate);
  else
    sprintf(atval[2],"'%s'",cval[5]);
  sprintf(atval[3],"'%s'",cval[5]);



  while (strPtr != NULL) {
    sprintf(atval[1],"%i",mdata_maxnum);
    ii =   getAttrValFromBulkLoadMetadataString(strPtr, separator,
						atval, 4, &strPtr2);
    if (ii == UNIX_EINVAL )
      return (0);
    else if (ii < 0)
      return(ii);
#ifdef FED_MCAT
    i = insertIntoTable(atname, atval,"MDAS_AD_COLLMDATA", ii );
#else
    strcpy(atname[2],atname[ii-2]);
    strcpy(atname[3],atname[ii-1]);
    strcpy(atval[2],atval[ii-2]);
    strcpy(atval[3],atval[ii-1]);
    i = insertIntoTable(atname, atval,"MDAS_AD_COLLMDATA", ii-2 );
#endif
    if (i != 0) return (MDAS_AD_COLLMDATA_INSERTION_ERROR);
    mdata_maxnum++;
    strPtr = strPtr2;
    strPtr2 = NULL;
  }
  return(0);

}

int
bulkInsertUdefMetadataForResource(int rsrc_id,
			      char *metadataString,
			      char *separator, char *modDate)
{
  int i,ii, mdata_maxnum;
  char *strPtr, *strPtr2;

  sprintf(sqlq,"select rsrc_id from %sMDAS_AR_MDATA where  rsrc_id = %i ", 
	  MDASSCHEMENAME, rsrc_id);
  if (check_exists(sqlq) == 0) {
    sprintf(sqlq,
	    "select max(metadatanum) from %sMDAS_AR_MDATA where rsrc_id = %i ",
	    MDASSCHEMENAME, rsrc_id);
    mdata_maxnum = get_itype_value_from_query(sqlq) ;
    if (mdata_maxnum < 0) {
      if (mdata_maxnum != -1)
	return (mdata_maxnum);
      sprintf(sqlq,"delete from %sMDAS_AR_MDATA where rsrc_id = %i  and metadatanum = -1",
	      MDASSCHEMENAME, rsrc_id);
      ExecSqlDb2(henv, hdbc, sqlq);
    }
    mdata_maxnum++;
  }
  else {
    mdata_maxnum =  0;
  }
  strPtr = metadataString;
  strPtr2 = NULL;
  get_time(cval[5]);
  sprintf(atname[0],"rsrc_id");
  sprintf(atname[1],"metadatanum");
  sprintf(atname[2],"userdef_modif_date");
  sprintf(atname[3],"userdef_creat_date");
  sprintf(atname[4],"userdef_metastr0");
  sprintf(atname[5],"userdef_metastr1");
  sprintf(atname[6],"userdef_metastr2");
  sprintf(atname[7],"userdef_metastr3");
  sprintf(atname[8],"userdef_metastr4");
  sprintf(atname[9],"userdef_metastr5");
  sprintf(atname[10],"userdef_metastr6");
  sprintf(atname[11],"userdef_metastr7");
  sprintf(atname[12],"userdef_metastr8");
  sprintf(atname[13],"userdef_metastr9");
  sprintf(atname[14],"userdef_metaint0");
  sprintf(atname[15],"userdef_metaint1");
  sprintf(atval[0],"%i",rsrc_id);
  if (modDate != NULL && strlen(modDate) > 0)
    sprintf(atval[2],"'%s'",modDate);
  else
    sprintf(atval[2],"'%s'",cval[5]);
  sprintf(atval[3],"'%s'",cval[5]);


  while (strPtr != NULL) {
    sprintf(atval[1],"%i",mdata_maxnum);
    ii =   getAttrValFromBulkLoadMetadataString(strPtr, separator,
						atval, 4, &strPtr2);
    if (ii == UNIX_EINVAL )
      return (0);
    else if (ii < 0)
      return(ii);
#ifdef FED_MCAT
    i = insertIntoTable(atname, atval,"MDAS_AR_MDATA", ii );
#else
    strcpy(atname[2],atname[ii-2]);
    strcpy(atname[3],atname[ii-1]);
    strcpy(atval[2],atval[ii-2]);
    strcpy(atval[3],atval[ii-1]);
  i = insertIntoTable(atname, atval,"MDAS_AR_MDATA", ii-2 );
#endif
    if (i != 0) return (MDAS_AR_MDATA_INSERTION_ERROR);
    mdata_maxnum++;
    strPtr = strPtr2;
    strPtr2 = NULL;
  }
  return(0);

}
 

int
bulkInsertUdefMetadataForUser(int obj_user_id,
			      char *metadataString,
			      char *separator, char *modDate)
{
  int i,ii, mdata_maxnum;
  char *strPtr, *strPtr2;

  sprintf(sqlq,"select user_id from %sMDAS_AU_MDATA where  user_id = %i ", 
	  MDASSCHEMENAME, obj_user_id);
  if (check_exists(sqlq) == 0) {
    sprintf(sqlq,
	    "select max(metadatanum) from %sMDAS_AU_MDATA where user_id = %i ",
	    MDASSCHEMENAME, obj_user_id);
    mdata_maxnum = get_itype_value_from_query(sqlq) ;
    if (mdata_maxnum < 0) {
      if (mdata_maxnum != -1)
	return (mdata_maxnum);
      sprintf(sqlq,"delete from %sMDAS_AU_MDATA where  user_id = %i  and metadatanum = -1",
	      MDASSCHEMENAME,  obj_user_id);
      ExecSqlDb2(henv, hdbc, sqlq);
    }
    mdata_maxnum++;
  }
  else {
    mdata_maxnum =  0;
  }
  strPtr = metadataString;
  strPtr2 = NULL;

  get_time(cval[5]);
  sprintf(atname[0],"user_id");
  sprintf(atname[1],"metadatanum");
  sprintf(atname[2],"userdef_modif_date");
  sprintf(atname[3],"userdef_creat_date");
  sprintf(atname[4],"userdef_metastr0");
  sprintf(atname[5],"userdef_metastr1");
  sprintf(atname[6],"userdef_metastr2");
  sprintf(atname[7],"userdef_metastr3");
  sprintf(atname[8],"userdef_metastr4");
  sprintf(atname[9],"userdef_metastr5");
  sprintf(atname[10],"userdef_metastr6");
  sprintf(atname[11],"userdef_metastr7");
  sprintf(atname[12],"userdef_metastr8");
  sprintf(atname[13],"userdef_metastr9");
  sprintf(atname[14],"userdef_metaint0");
  sprintf(atname[15],"userdef_metaint1");
  sprintf(atval[0],"%i",obj_user_id);
  if (modDate != NULL && strlen(modDate) > 0)
    sprintf(atval[2],"'%s'",modDate);
  else
    sprintf(atval[2],"'%s'",cval[5]);
  sprintf(atval[3],"'%s'",cval[5]);


  while (strPtr != NULL) {
    sprintf(atval[1],"%i",mdata_maxnum);
    ii =   getAttrValFromBulkLoadMetadataString(strPtr, separator,
						atval, 4, &strPtr2);
    if (ii == UNIX_EINVAL )
      return (0);
    else if (ii < 0)
      return(ii);
#ifdef FED_MCAT
    i = insertIntoTable(atname, atval,"MDAS_AU_MDATA", ii );
#else
    strcpy(atname[2],atname[ii-2]);
    strcpy(atname[3],atname[ii-1]);
    strcpy(atval[2],atval[ii-2]);
    strcpy(atval[3],atval[ii-1]);
    i = insertIntoTable(atname, atval,"MDAS_AU_MDATA", ii-2 );
#endif
    if (i != 0) return (MDAS_AU_MDATA_INSERTION_ERROR);
    mdata_maxnum++;
    strPtr = strPtr2;
    strPtr2 = NULL;
  }
  return(0);

}
 

int
bulkInsertUdefMetadataForDataName(char *dataName, char * collName,
				  char *metadataString,
				  char *separator, char *modDate) {
  int i,j;

  i = getDataId(collName,dataName);
  if (i < 0)
    return(i);
  j = bulkInsertUdefMetadataForData(i, metadataString,separator,modDate);
  return(j);
}

int
bulkInsertUdefMetadataForData(int obj_data_id,
			      char *metadataString,
			      char *separator, char *modDate)
{
  int i,ii, mdata_maxnum;
  char *strPtr, *strPtr2;

  sprintf(sqlq,"select data_id from %sMDAS_AD_MDATA where  data_id = %i ", 
	  MDASSCHEMENAME, obj_data_id);
  if (check_exists(sqlq) == 0) {
    sprintf(sqlq,
	    "select max(metadatanum) from %sMDAS_AD_MDATA where data_id = %i ",
	    MDASSCHEMENAME, obj_data_id);
    mdata_maxnum = get_itype_value_from_query(sqlq) ;
    if (mdata_maxnum < 0) {
      if (mdata_maxnum != -1)
	return (mdata_maxnum);
      sprintf(sqlq,"delete from %sMDAS_AD_MDATA where data_id = %i  and metadatanum = -1",
	      MDASSCHEMENAME, obj_data_id);
      ExecSqlDb2(henv, hdbc, sqlq);
    }
    mdata_maxnum++;
  }
  else {
    mdata_maxnum =  0;
  }
  strPtr = metadataString;
  strPtr2 = NULL;

  get_time(cval[5]);
  sprintf(atname[0],"data_id");
  sprintf(atname[1],"metadatanum");
  sprintf(atname[2],"userdef_modif_date");
  sprintf(atname[3],"userdef_creat_date");
  sprintf(atname[4],"userdef_metastr0");
  sprintf(atname[5],"userdef_metastr1");
  sprintf(atname[6],"userdef_metastr2");
  sprintf(atname[7],"userdef_metastr3");
  sprintf(atname[8],"userdef_metastr4");
  sprintf(atname[9],"userdef_metastr5");
  sprintf(atname[10],"userdef_metastr6");
  sprintf(atname[11],"userdef_metastr7");
  sprintf(atname[12],"userdef_metastr8");
  sprintf(atname[13],"userdef_metastr9");
  sprintf(atname[14],"userdef_metaint0");
  sprintf(atname[15],"userdef_metaint1");
  sprintf(atval[0],"%i",obj_data_id);
  if (modDate != NULL && strlen(modDate) > 0)
    sprintf(atval[2],"'%s'",modDate);
  else
    sprintf(atval[2],"'%s'",cval[5]);
  sprintf(atval[3],"'%s'",cval[5]);

  while (strPtr != NULL) {
    sprintf(atval[1],"%i",mdata_maxnum);
    ii =   getAttrValFromBulkLoadMetadataString(strPtr, separator,
						atval, 4, &strPtr2);
    if (ii == UNIX_EINVAL )
      return (0);
    else if (ii < 0)
      return(ii);
#ifdef FED_MCAT
    i = insertIntoTable(atname, atval,"MDAS_AD_MDATA", ii );
#else
    strcpy(atname[2],atname[ii-2]);
    strcpy(atname[3],atname[ii-1]);
    strcpy(atval[2],atval[ii-2]);
    strcpy(atval[3],atval[ii-1]);
    i = insertIntoTable(atname, atval,"MDAS_AD_MDATA", ii-2 );
#endif
    if (i != 0) return (MDAS_AD_MDATA_INSERTION_ERROR);
    mdata_maxnum++;
    strPtr = strPtr2;
    strPtr2 = NULL;
  }
  return(0);

}

int
bulkInsertUdefMetadataForManyData(char *userName, char *domainName,
			      char *metadataString,
			      char *separator, char *modDate)
{
  int i,ii, mdata_maxnum;
  char *strPtr, *strPtr2;
  char tmporaryStr[2 * MAX_TOKEN];
  char oldCollName[MAX_TOKEN];
  char oldDataName[MAX_TOKEN];
  int oldMaxNum;
  int firstMaxNum = -1;
  char *tmpPtr, *tmpPtr2;
  int j;
  int userId, obj_data_id;
  int res;
  strPtr = metadataString;
  strPtr2 = NULL;
  oldCollName[0] ='\0';
  oldDataName[0] ='\0';


  res = 0;
  if (modDate == NULL || strlen(modDate) == 0)
    get_time(cval[5]);
  else
    strcpy(cval[5],modDate);

    ii =   getAttrValFromBulkLoadMetadataString(strPtr, separator,
						atval, 0, &strPtr2);
    if (ii == UNIX_EINVAL )
      return (0);
    else if (ii < 0)
      return(ii);
    if ((tmpPtr = strstr(atval[0],"'SETMINMETADATANUM=")) != NULL) {
      tmpPtr += strlen("'SETMINMETADATANUM=");
      if (strstr(tmpPtr,"GIVENPERDATA")) {
	firstMaxNum = -2;
	j  = 3;
      }
      else if (strstr(tmpPtr,"GETFROMMCAT")) {
	firstMaxNum = -1;
	j = 2;
      }
      else {
	if ((tmpPtr2 = strstr(tmpPtr,"'")) != NULL)
	  *tmpPtr2 = '\0';
	firstMaxNum = atoi(tmpPtr);
	if (tmpPtr2 != NULL)
	  *tmpPtr2 = '\'';
	j = 2;
      }
    strPtr = strPtr2;
    strPtr2 = NULL;
    }
    else {
      strPtr = metadataString;
      strPtr2 = NULL;
      j = 2;
    }

    userId =  get_user_id_in_domain(userName, domainName);

  /* format of input: collName|dataname|metadatanum|metadata0|metadata...  if  GIVENPERDATA*/
  /* format of input: collName|dataname|metadata0|metadata...   */
  
  while (strPtr != NULL) {
    ii =   getAttrValFromBulkLoadMetadataString(strPtr, separator,
						atval, 0, &strPtr2);
    if (ii == UNIX_EINVAL )
      return (0);
    else if (ii < 0)
      return(ii);

    if (strcmp(oldCollName, atval[0]) || strcmp(oldDataName, atval[1])) {
      obj_data_id = getDataIdWithOwnerId(atval[0],atval[1],userId);
      if (obj_data_id < 0)
	return (obj_data_id);
      strcpy(oldCollName, atval[0]);
      strcpy(oldDataName, atval[1]);
      if (firstMaxNum == -1) {
	sprintf(sqlq,"select data_id from %sMDAS_AD_MDATA where  data_id = %i ", 
		MDASSCHEMENAME, obj_data_id);
	if (check_exists(sqlq) == 0) {
	  sprintf(sqlq,
	    "select max(metadatanum) from %sMDAS_AD_MDATA where data_id = %i ",
		  MDASSCHEMENAME, obj_data_id);
	  mdata_maxnum = get_itype_value_from_query(sqlq) ;
	  if (mdata_maxnum < 0) {
	    if (mdata_maxnum != -1)
	      return (mdata_maxnum);
	    sprintf(sqlq,"delete from %sMDAS_AD_MDATA where data_id = %i  and metadatanum = -1",
		    MDASSCHEMENAME,obj_data_id);
	    ExecSqlDb2(henv, hdbc, sqlq);
	  }
	  mdata_maxnum++;
	}
	else {
	  mdata_maxnum =  0;
	}

      }
      else if (firstMaxNum == -2) {
	if ((tmpPtr2 = strstr((char *)atval[2]+1,"'")) != NULL)
	  *tmpPtr2 = '\0';
	mdata_maxnum = atoi((char *)atval[2]+1);
	if (tmpPtr2 != NULL)
	  *tmpPtr2 = '\'';
      }
      else 
	mdata_maxnum = firstMaxNum;
    }
    else {
      if (firstMaxNum == -2) {
	if ((tmpPtr2 = strstr((char *)atval[2]+1,"'")) != NULL)
	  *tmpPtr2 = '\0';
	mdata_maxnum = atoi((char *)atval[2]+1);
	if (tmpPtr2 != NULL)
	  *tmpPtr2 = '\'';
      }
      else 
	mdata_maxnum++;
    }

    
#ifdef FED_MCAT
    sprintf(query_string, "insert into %s%s ( data_id, metadatanum,userdef_creat_date,userdef_modif_date ",
	    MDASSCHEMENAME, "MDAS_AD_MDATA");
#else
    sprintf(query_string, "insert into %s%s ( data_id, metadatanum ",
	    MDASSCHEMENAME, "MDAS_AD_MDATA");
#endif
    for (i = 0;  i < ii - j && i < MAX_ADREPL_STRING_METADATA ; i++) {
	sprintf(tmporaryStr,",userdef_metastr%i",i);
	strcat(query_string,tmporaryStr);
    }
    for (i = 0 ; i <(ii-MAX_ADREPL_STRING_METADATA -j)&& i < MAX_ADREPL_INTEGER_METADATA ; i++){
      sprintf(tmporaryStr,",userdef_metaint%i",i);
      strcat(query_string,tmporaryStr);
    }
#ifdef FED_MCAT
    sprintf(tmporaryStr,
	    ") values (%i, %i, '%s','%s'",  obj_data_id ,mdata_maxnum,
	    cval[5], cval[5]);
#else
    sprintf(tmporaryStr,
	    ") values (%i, %i",  obj_data_id ,mdata_maxnum);
#endif
    strcat(query_string,tmporaryStr);
    for (i = 0;  i < ii - j && i < MAX_ADREPL_STRING_METADATA ; i++) {
	sprintf(tmporaryStr,",%s",atval[i+j]);
	strcat(query_string,tmporaryStr);
    }
    for (i = 0 ; i <(ii-MAX_ADREPL_STRING_METADATA -j)&& i < MAX_ADREPL_INTEGER_METADATA ; i++){
	sprintf(tmporaryStr,",%i",atoi(atval[i+j+MAX_ADREPL_STRING_METADATA]));
	strcat(query_string,tmporaryStr);
    }
    strcat(query_string,")");

    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0) 
      { error_exit("data registry API: meta-daata insertion error"); 
      return(MDAS_AD_MDATA_INSERTION_ERROR);}
    strPtr = strPtr2;
    strPtr2 = NULL;
  }
  return(0);

}

int
getAttrValFromBulkLoadMetadataString(char *beginPtr, 
				     char *separator,
				     char atval[][MAX_DATA_SIZE],
				     int atvalStartIndex,
				     char **endPtr)
{

 char *tmpPtr, *tmpPtr2;
 char c,d;
 int csep;
 int ii;
   
   tmpPtr = beginPtr;
   tmpPtr2 = tmpPtr;
   c = separator[0];
   d = separator[0];
   ii = atvalStartIndex;
   csep = strlen(separator);
   if (csep == 0)
     return(METADATA_FORM_ERROR);
   if (*tmpPtr == '\0')
     return(UNIX_EINVAL);
   while (1 == 1) {
     if (*tmpPtr == d) {
       if (csep > 1 && strstr(tmpPtr,separator) != tmpPtr){
	 tmpPtr++;
       }
       else {
	 *tmpPtr = '\0';
	 if (ii > (atvalStartIndex + 9) || BulkLoadFlag == 1)
	   sprintf(atval[ii],"%s",tmpPtr2);
	 else
	   sprintf(atval[ii],"'%s'",tmpPtr2);
	 ii++;
	 *tmpPtr = d;
	 tmpPtr =  tmpPtr + csep;
	 tmpPtr2 = tmpPtr;
       }
     }
     else if (*tmpPtr == '\n' || iscntrl(*tmpPtr) || *tmpPtr == '\0' ){
       c = *tmpPtr;
       *tmpPtr = '\0';
       if (ii > (atvalStartIndex + 9) || BulkLoadFlag == 1)
	 sprintf(atval[ii],"%s",tmpPtr2);
       else
	 sprintf(atval[ii],"'%s'",tmpPtr2);
       *tmpPtr =  c;
       ii++;
       while (*tmpPtr != '\0' && (*tmpPtr == '\n' ||iscntrl(*tmpPtr) ))
	 tmpPtr++;
       *endPtr = tmpPtr;
       return(ii);
     }
     else {
       tmpPtr++;
     }
   }
}

int
getDataIdWithOwnerId(char *collName, char *dataName, int ownerId)
{
  int i;

  if (collName[0] =='\'')
    sprintf(sqlq,"select data_id from %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where t1.data_name = %s and t1.data_owner = %i  and t2.data_grp_name =%s and t1.data_grp_id = t2.data_grp_id", MDASSCHEMENAME, MDASSCHEMENAME, dataName, ownerId, collName);
  else
    sprintf(sqlq,"select data_id from %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where t1.data_name = '%s' and t1.data_owner = %i  and t2.data_grp_name ='%s' and t1.data_grp_id = t2.data_grp_id", MDASSCHEMENAME, MDASSCHEMENAME, dataName, ownerId, collName);
   i = get_itype_value_from_query(sqlq) ;
   if (i < 0) {
     if (collName[0] =='\'')
       sprintf(sqlq,"select data_id from %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where t1.data_name = %s and t2.data_grp_name =%s and t1.data_grp_id = t2.data_grp_id", MDASSCHEMENAME, MDASSCHEMENAME, dataName, collName);
     else
       sprintf(sqlq,"select data_id from %sMDAS_AD_REPL t1, %sMDAS_TD_DATA_GRP t2 where t1.data_name = '%s' and t2.data_grp_name ='%s' and t1.data_grp_id = t2.data_grp_id", MDASSCHEMENAME, MDASSCHEMENAME, dataName, collName);
     i = get_itype_value_from_query(sqlq) ;
     if (i < 0)
       return (DATA_NOT_IN_CAT);
     else
       return (USER_IS_NOT_OWNER_ERROR);
   }
   return(i);
}

int
isInstallSRBUser(char *userName) 
{
  for (i = 0; i < SMALL_TOKEN; i++) {
    if (!strcmp(userName, installSRBUser[i]))
      return(0);
    if (!strcmp("", installSRBUser[i]))
      return(-1);
  }
}

int
insertGuidBySRB(int data_id)
{
  char guidStr[MAX_TOKEN];
  int i;
  char tS[100];
  char zoneId[MAX_TOKEN];

  get_time(tS);
#ifdef  FED_MCAT


  if (!strcmp(genGuidFlag,"NO_AUTO_GUID") )
    return(MDAS_SUCCESS);
  else if (!strcmp(genGuidFlag,"STORE_AUTO_GUID") ) {
    i = getGuidBySRB(guidStr);
    if (i < 0)
      return(i);
  }
  else if (!strcmp(genGuidFlag,"STORE_ZONE_DATA_ID") ) {
    i = getLocalZone(zoneId);
    if (i < 0)
      return(i);
    if (data_id > 0) {
      sprintf(query_string,
            "insert into %s%s (data_id,guid,guid_flag,guid_time_stamp) values (%i,'%s:%i',0,'%s')",
            MDASSCHEMENAME, "MDAS_AD_GUID", data_id, zoneId,data_id,tS);
    }
    else {
#if defined(ORAMCAT) || defined(PSQMCAT)
      sprintf(query_string,
	      "insert into %s%s (data_id,guid,guid_flag,guid_time_stamp) values (%s,'%s:'||%s,0,'%s')",
	      MDASSCHEMENAME, "MDAS_AD_GUID", currDataId, zoneId,currDataId,tS);
#else
      sprintf(query_string,
	      "insert into %s%s (data_id,guid,guid_flag,guid_time_stamp) values (%i,'%s:%i',0,'%s')",
	      MDASSCHEMENAME, "MDAS_AD_GUID", data_id, zoneId,data_id,tS);
#endif /* ORAMCAT */
    }
    i = ExecSqlDb2(henv, hdbc, query_string);
    if (i != 0) {
      elog(NOTICE, "insertGuidBySRB: Failing SQL with %i:%s",i,query_string);
      return(MDAS_AD_GUID_INSERTION_ERROR);
    }
    return(MDAS_SUCCESS);
  }
  else
    strcpy(guidStr,genGuidFlag);


  if (data_id > 0) {
    sprintf(query_string, 
	    "insert into %s%s (data_id,guid,guid_flag,guid_time_stamp) values (%i,'%s',0,'%s')",
	    MDASSCHEMENAME, "MDAS_AD_GUID", data_id, guidStr,tS);

  }
  else {
#if defined(ORAMCAT) || defined(PSQMCAT)
    sprintf(query_string, 
	    "insert into %s%s (data_id,guid,guid_flag,guid_time_stamp) values (%s,'%s',0,'%s')",
	    MDASSCHEMENAME, "MDAS_AD_GUID", currDataId, guidStr,tS);
#else
    sprintf(query_string, 
	    "insert into %s%s (data_id,guid,guid_flag,guid_time_stamp) values (%i,'%s',0,'%s')",
	    MDASSCHEMENAME, "MDAS_AD_GUID", data_id, guidStr,tS);
#endif /* ORAMCAT */
  }
  i = ExecSqlDb2(henv, hdbc, query_string);
  if (i != 0) 
    return(MDAS_AD_GUID_INSERTION_ERROR);
#endif /**  FED_MCAT **/

  return(MDAS_SUCCESS);

}
#endif

char *
forwardPath (char *pathName, int count)
{
    char *tmpPtr;
    int myCount = 0;

    if (pathName == NULL || count <= 0)
	return NULL;

    tmpPtr = pathName;

    while (*tmpPtr != '\0') {
	if (*tmpPtr == '/') {
	    myCount ++;
	    if (myCount >= count) 
		break;
	}
	tmpPtr++;
    }

    if (myCount >= count) {
	return (++tmpPtr);
    } else {
	return NULL;
    }
}
	



int
inheritACLWithSticky( char *parentCollId,
		      int dataId,
		      int replnum,
		      char *collId,
		      int userId,
		      char *flagStr)
{
  int stickyValue,i,res;
  /* stickyValue = 0 - no inheritance 
     stickyValue = 1 - inherit ACL from parent collection , if data or collection
                       inherit stickyValue from parent collection, if collection
     XXX stickyValue = 2 - do as for stickyValue = 1 and also
                       inherit user-defined metadata from parent collection
  */
  sprintf(query_string,"select coll_link from %sMDAS_TD_DATA_GRP where data_grp_id = '%s'",
	  MDASSCHEMENAME,  parentCollId);
  stickyValue = get_itype_value_from_query(query_string);
  if (stickyValue == 0)
    return(MDAS_SUCCESS);

  if (!strcmp(flagStr,"single-data-register" )) {
#if defined(ORAMCAT) || defined(PSQMCAT)
    sprintf(query_string,
	    "insert into %s%s (data_id,repl_enum,user_id,access_id) select %s,%i,user_id,access_id from %s%s where data_grp_id = '%s' and user_id <> %i",
	    MDASSCHEMENAME, "MDAS_AD_ACCS", collId, replnum, 
	    MDASSCHEMENAME, "MDAS_AD_GRP_ACCS", parentCollId, userId);
#else
    sprintf(query_string,
	    "insert into %s%s (data_id,repl_enum,user_id,access_id) select %i,%i,user_id,access_id from %s%s where data_grp_id = '%s' and user_id not in (select user_id from %s%s where data_id = %i and repl_enum = %i)",
	    MDASSCHEMENAME,"MDAS_AD_ACCS", dataId, replnum, 
	    MDASSCHEMENAME, "MDAS_AD_GRP_ACCS", parentCollId,
	    MDASSCHEMENAME,"MDAS_AD_ACCS", dataId, replnum);
#endif /* ORAMCAT */
    
    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0) {
      elog(NOTICE, "Error inheritACLWithSticky: %s\n",query_string);
      return(MDAS_AD_ACCS_INSERTION_ERROR);
    }
    if (stickyValue == 2) {
    } 
  }
  else if (!strcmp(flagStr,"single-collection-register" )) {
    sprintf(query_string,
            "insert into %s%s (data_grp_id,user_id,access_id) select '%s',user_id,access_id from %s%s where data_grp_id = '%s'  and user_id not in (select user_id from %s%s where data_grp_id = '%s')",
            MDASSCHEMENAME, "MDAS_AD_GRP_ACCS", collId, 
	    MDASSCHEMENAME, "MDAS_AD_GRP_ACCS", parentCollId,
	    MDASSCHEMENAME, "MDAS_AD_GRP_ACCS", collId);
    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0) {
      elog(NOTICE, "Error inheritACLWithSticky: %s\n",query_string);
      return(MDAS_AD_GRP_ACCS_INSERTION_ERROR);
    }
    sprintf(query_string,
	    "update %s%s set coll_link = %i where data_grp_id = '%s'",MDASSCHEMENAME, "MDAS_TD_DATA_GRP",stickyValue, 
	    collId);
    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0) {
      elog(NOTICE, "Error inheritACLWithSticky: %s\n",query_string);
      return(MDAS_AD_GRP_ACCS_INSERTION_ERROR);
    }
  }
  else if (!strcmp(flagStr,"multi-replica-move")) {
    sprintf(query_string,
            "insert into %s%s (data_id,repl_enum,user_id,access_id) select B.data_id, B.repl_enum ,A.user_id, A.access_id from %s%s A, %s%s B  where A.data_grp_id = '%s' and B.data_id = %i and A.user_id not in (select C.user_id from %s%s C where C.data_id = B.data_id  and C.repl_enum = B.repl_enum)",
            MDASSCHEMENAME,"MDAS_AD_ACCS", 
            MDASSCHEMENAME, "MDAS_AD_GRP_ACCS", MDASSCHEMENAME,"MDAS_AD_REPL", parentCollId, dataId,
            MDASSCHEMENAME,"MDAS_AD_ACCS");
    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0) {
      elog(NOTICE, "Error inheritACLWithSticky: %s\n",query_string);
      return(MDAS_AD_GRP_ACCS_INSERTION_ERROR);
    }

  }
  else if (!strcmp(flagStr,"recur-collection-move")) {
    /* first the files */
    sprintf(query_string,
            "insert into %s%s (data_id,repl_enum,user_id,access_id) select B.data_id, B.repl_enum ,A.user_id, A.access_id from %s%s A, %s%s B, %s%s D  where A.data_grp_id = '%s' and D.data_grp_name like '%s%%' and D.data_grp_id = B.data_grp_id and A.user_id not in (select C.user_id from %s%s C where C.data_id = B.data_id  and C.repl_enum = B.repl_enum)",
            MDASSCHEMENAME,"MDAS_AD_ACCS", 
            MDASSCHEMENAME, "MDAS_AD_GRP_ACCS", MDASSCHEMENAME,"MDAS_AD_REPL", MDASSCHEMENAME,"MDAS_TD_DATA_GRP",
	    parentCollId, collId,
            MDASSCHEMENAME,"MDAS_AD_ACCS");
    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0) {
      elog(NOTICE, "Error inheritACLWithSticky: %s\n",query_string);
      return(MDAS_AD_GRP_ACCS_INSERTION_ERROR);
    }
    /* next the sub collections */
    sprintf(query_string,
            "insert into %s%s (data_grp_id,user_id,access_id) select B.data_grp_id, A.user_id, A.access_id from %s%s A, %s%s B,  %s%s D   where A.data_grp_id = '%s' and D.data_grp_name like '%s%%' and D.data_grp_id = B.data_grp_id and A.user_id not in (select C.user_id from %s%s C where C.data_grp_id = B.data_grp_id)",
	    MDASSCHEMENAME,"MDAS_AD_GRP_ACCS",
	    MDASSCHEMENAME, "MDAS_AD_GRP_ACCS", MDASSCHEMENAME,"MDAS_TD_DATA_GRP", MDASSCHEMENAME,"MDAS_TD_DATA_GRP",
	    parentCollId, collId,
	    MDASSCHEMENAME,"MDAS_AD_GRP_ACCS");
    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0) {
      elog(NOTICE, "Error inheritACLWithSticky: %s\n",query_string);
      return(MDAS_AD_GRP_ACCS_INSERTION_ERROR);
    }

    /* third the sticky bits */
    sprintf(query_string,
            "update %s%s set coll_link = %i where data_grp_name like  '%s%%'",MDASSCHEMENAME, "MDAS_TD_DATA_GRP",
	    stickyValue, collId);
    res = ExecSqlDb2(henv, hdbc, query_string);
    if (res != 0) {
      elog(NOTICE, "Error inheritACLWithSticky: %s\n",query_string);
      return(MDAS_AD_GRP_ACCS_INSERTION_ERROR);
    }

  }
  return(MDAS_SUCCESS);
}
