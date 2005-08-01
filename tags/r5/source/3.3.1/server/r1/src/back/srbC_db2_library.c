/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include  <unistd.h>
#include "sqlcli1.h"
#include "dbLobj.h"
#include "dbLobjExtern.h"
#include "srbMiscExtern.h"
#include "srbC_db2_externs.h"
/* #include "mdas_errno.h" */

/*
static char logmess[HUGH_HUGE_STRING];

  static int i, n;
  static int             henv;
  static int             hdbc;
  static int             hstmt,hstmt2;
*/
  static int             hstmt2;
  static unsigned char        query[100];
  static char                 queryString[1000]; 




errorDb2Message(int rc, int hstmt, db_state_info *stateInfo, char * message)
{
    SQLCHAR         buffer[SQL_MAX_MESSAGE_LENGTH + 1];
    SQLCHAR         sqlstate[SQL_SQLSTATE_SIZE + 1];
    SQLINTEGER      sqlcode;
    SQLSMALLINT     length;
    int	firstCode = 0;

  elog (NOTICE, "Error: %s:%i",message,rc);
  while (SQLError(stateInfo->henv, stateInfo->hdbc, hstmt, 
		  sqlstate, &sqlcode, buffer,
                    SQL_MAX_MESSAGE_LENGTH + 1, &length) == SQL_SUCCESS) {
        elog (NOTICE,"         SQLSTATE: %s", sqlstate);
        elog (NOTICE,"Native Error Code: %ld", sqlcode);
        elog (NOTICE,"%s", buffer);
	if (firstCode == 0)
	    firstCode = sqlcode;
    }
  SQLFreeStmt(hstmt, SQL_DROP);
    if (firstCode > 0)
	return (-1 * firstCode);
    else
    	return (firstCode);
}

printdbSpecificInfo(dbLobjDesc fd)
{
  db_state_info *stateInfo;

  stateInfo = (db_state_info *) fd.dbSpecificInfo; 

  printf("*******************\n");
  printf("** Type = %i      **\n", fd.dbSystemType);
  printf("** FD   = %i      **\n", stateInfo->fd);
  printf("** HENV = %i      **\n", stateInfo->henv);
  printf("** HDBC = %i      **\n", stateInfo->hdbc);
  printf("** LSIZ = %i      **\n", stateInfo->lobsize);
  printf("** LPOS = %i      **\n", stateInfo->locpos);
  printf("** TNAM = %s      **\n", stateInfo->tableName);
  printf("** OCN  = %s      **\n", stateInfo->outColName);
  printf("** COND = %s      **\n", stateInfo->inCondition);
  printf("*******************\n");

}

void
separateColVal(char *inval, char *columnSet, char *valueSet)
{
  int i,j,k,l,m;
  i = 0; j = 0; k = 0; m = 0;
  l = strlen(inval);
  for (i = 0; i < l ; i++)
    {
      if (inval[i] == '\0') break;
      /*if (inval[i] == ' ' && inval[i-1] != '\\') continue;      */
      else if (inval[i] == '\\' && inval[i-1] != '\\') continue;
      else if (inval[i] == '&' && inval[i-1] != '\\') 
             {valueSet[k] = ','; k++; m = 0;}
      else if (inval[i] == '=' && inval[i-1] != '\\')
             {columnSet[j] = ','; j++; m = 1;}
      else
	if (m == 0) {columnSet[j] = inval[i]; j++;}
        else {valueSet[k] = inval[i]; k++;}
    }
  columnSet[j] = '\0';
  valueSet[k] = ',';  k++;
  valueSet[k] = '\0';
}

convertAnd(char *inval)
{
    char temptab[STRING_LENMAX];
    int i,j,l;
    i = 0; j = 0;
    strcpy(temptab,inval);
    while(temptab[i] != '\0')
      {
	if (temptab[i] == '&')
	  {
	    inval[j] = ' ';
	    inval[++j] = 'A';
	    inval[++j] = 'N';
	    inval[++j] = 'D';
	    inval[++j] = ' ';
	  }
	else
	  {
	    inval[j] = temptab[i];
	  }
	j++; i++;
      }
	    
    
    

}

/*
 Open an existing large object
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         For Illustra, instance is not used.
 *         char *dataPathName - Format : tablename/outCol/inCol=objectID
 *                          or tabname/outCol/inCol1=val1&...&inColn=valn
 *
 */

int srbC_db2_open(dbLobjDesc *exfd, char *resourceLocationName,
		  char *dataPathName, char *uid, char *pwd)

{
    int             henv;
    int             hdbc;
    db_state_info *stateInfo;

    char             instance[STRING_LEN1],hostAddr[STRING_LEN1];
    char database[STRING_LEN1], tmpStr[STRING_LENMAX];



    char s1[100];

    SQLRETURN            i;
    SQLINTEGER           fd;
    SQLINTEGER           lobsize, locsize;
 
    /* dataPathName must begin with a '/' */

    if (*dataPathName != '/')
        return DBLOBJ_PATH_NAME_ERROR;

    dataPathName ++;            /* skip the '/' */

    /*First allocate an area for state information */
    if (exfd->dbSystemType != DB2_LOBJ)
      {	elog (NOTICE, "srbC_db2_open: Bad DB2_LOBJ Type: %i",
	      exfd->dbSystemType);
      return DBLOBJ_TYPE_ERROR;}

    stateInfo = malloc(sizeof (db_state_info));


    /*    mySplitStr(dataPathName, stateInfo->tableName, 
	      stateInfo->objectName,'/');
    */

    if (mySplitStr(dataPathName, stateInfo->tableName,tmpStr,'/') == NULL) 
      {
	elog (NOTICE, "srbC_db2_open: Illegal dataPathName format: %s",
	      dataPathName);
        free(stateInfo);return(DBLOBJ_PATH_NAME_ERROR);
      }
    if (mySplitStr(tmpStr, stateInfo->outColName,
		    stateInfo->inCondition,'/') == NULL) 
      {
	elog (NOTICE, "srbC_db2_open: Illegal dataPathName format: %s",
	      dataPathName);
        free(stateInfo);return(DBLOBJ_PATH_NAME_ERROR);
      }
    convertAnd(stateInfo->inCondition);

    if (mySplitStr(resourceLocationName, hostAddr, tmpStr, ':') == NULL) {
        elog (NOTICE, "srbC_db2_open: Illegal resourceLocationName format: %s",
        resourceLocationName);
        free(stateInfo);return DBLOBJ_RESOURCE_NAME_ERROR;
    }

    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "srbC_db2_open: Illegal resourceLocationName format: %s",
        resourceLocationName);
        free(stateInfo);return DBLOBJ_RESOURCE_NAME_ERROR;
    }

  sprintf(s1,"DB2INSTANCE=%s",instance);
  i = putenv(s1);
  if (i != SQL_SUCCESS) 
    {elog (NOTICE, "srbC_db2_open: Instance Error");
    free(stateInfo);return DBLOBJ_PUTENV_ERROR;}
  i = SQLAllocEnv((long *) &henv);
  if (i != SQL_SUCCESS) 
    {elog (NOTICE, "srbC_db2_open: AllocEnv Error");
    free(stateInfo);return DBLOBJ_ALLOC_ENV_ERROR;}
  i = SQLAllocConnect(henv, (long *) &hdbc);
  if (i != SQL_SUCCESS)  
    {elog (NOTICE, "srbC_db2_open: AllocConnect Error");
    free(stateInfo);SQLFreeEnv(henv); return DBLOBJ_ALLOC_CONNECT_ERROR;}
  i = SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
  if (i != SQL_SUCCESS)  
    {elog (NOTICE, "srbC_db2_open: SetConOption Error");
    free(stateInfo);SQLFreeEnv(henv); return DBLOBJ_OPTION_SETTING_ERROR;}
  i = SQLConnect(hdbc, (unsigned char*) database, SQL_NTS, 
	       (unsigned char*) uid, SQL_NTS, (unsigned char*) pwd,  SQL_NTS); 
  if (i != SQL_SUCCESS)  
    {elog (NOTICE, "srbC_db2_open:  Connect Error");
    free(stateInfo);SQLFreeConnect(hdbc);SQLFreeEnv(henv); return DBLOBJ_CONNECT_ERROR;}

  stateInfo->henv  = henv;
  stateInfo->hdbc  = hdbc;

  sprintf(queryString, "select %s from %s where %s",
	stateInfo->outColName, stateInfo->tableName, 
	   stateInfo->inCondition);
  i = SQLAllocStmt(hdbc, (long *) &hstmt); 
  if (i != SQL_SUCCESS) 
    {elog (NOTICE, "srbC_db2_open:  AllocStmt Error");
    free(stateInfo);SQLDisconnect(hdbc);SQLFreeConnect(hdbc);SQLFreeEnv(henv); return DBLOBJ_ALLOC_STMT_ERROR ;}
  i = SQLExecDirect(hstmt, (unsigned char *) queryString, SQL_NTS);
  if (i != SQL_SUCCESS) 
    {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_open:SQLExecDirect");free(stateInfo);SQLDisconnect(hdbc);SQLFreeConnect(hdbc);SQLFreeEnv(henv); return i;}
  i = SQLBindCol(hstmt, 1, SQL_C_BLOB_LOCATOR, &fd,0, &locsize);
  if (i != SQL_SUCCESS) 
    {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_open:SQLBindCol"); free(stateInfo);SQLDisconnect(hdbc);SQLFreeConnect(hdbc);SQLFreeEnv(henv); return i;}
  i = SQLFetch(hstmt);
  if (i != SQL_SUCCESS && i != SQL_SUCCESS_WITH_INFO)
     {
       if (i != SQL_NO_DATA)
	 {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_open:SQLFetch"); free(stateInfo);SQLDisconnect(hdbc);SQLFreeConnect(hdbc);SQLFreeEnv(henv); return i; }
     }
  stateInfo->fd = fd;
  i = SQLAllocStmt(hdbc, (long *) &hstmt2); 
  if (i != SQL_SUCCESS) { i =errorDb2Message(i,hstmt2,stateInfo,"srbC_db2_open:SQLAllocStmt"); free(stateInfo);SQLDisconnect(hdbc);SQLFreeConnect(hdbc);SQLFreeEnv(henv); return i;}
  i = SQLGetLength(hstmt2, SQL_C_BLOB_LOCATOR, fd, &lobsize, 0);
  if (i != SQL_SUCCESS) { i =errorDb2Message(i,hstmt2,stateInfo,"srbC_db2_open:SQLGetLength"); free(stateInfo);SQLFreeStmt(hstmt, SQL_DROP);SQLDisconnect(hdbc);SQLFreeConnect(hdbc);SQLFreeEnv(henv); return i;}
  stateInfo->lobsize = lobsize;
  stateInfo->locpos  = 1;
  i = SQLFreeStmt(hstmt, SQL_DROP);
  if (i != SQL_SUCCESS) { i =errorDb2Message(i,hstmt2,stateInfo,"srbC_db2_open:SQLFreeStmt"); free(stateInfo);SQLFreeStmt(hstmt2, SQL_DROP);SQLDisconnect(hdbc);SQLFreeConnect(hdbc);SQLFreeEnv(henv);return i;}
  i = SQLFreeStmt(hstmt2, SQL_DROP);
  if (i != SQL_SUCCESS) 
    {elog (NOTICE, "srbC_db2_open:  FreeStmt Error");
    free(stateInfo);SQLDisconnect(hdbc);SQLFreeConnect(hdbc);
    SQLFreeEnv(henv);return (DBLOBJ_FREE_STMT_ERROR);}
  exfd->dbSpecificInfo 
       = (char *)stateInfo;     /* return ptr to state information
                                   for this connection */
  return 0;
}

int srbC_db2_close(dbLobjDesc  *exfd)
{
  db_state_info *stateInfo;

  stateInfo = (db_state_info *) exfd->dbSpecificInfo;

    if (exfd->dbSystemType != DB2_LOBJ)
      {	elog (NOTICE, "srbC_db2_close: Bad DB2_LOBJ Type: %i",
	      exfd->dbSystemType);return DBLOBJ_TYPE_ERROR;}
  i = SQLAllocStmt(stateInfo->hdbc, (long *) &hstmt); 
  if (i != SQL_SUCCESS) 
    {elog (NOTICE, "srbC_db2_close: AllocStmt Error");      
    SQLDisconnect(stateInfo->hdbc);SQLFreeConnect(stateInfo->hdbc);SQLFreeEnv(stateInfo->henv);free(stateInfo);return (DBLOBJ_FREE_STMT_ERROR);}
  i = SQLSetParam(hstmt, 1, SQL_C_BLOB_LOCATOR, SQL_BLOB_LOCATOR, 0,
                 0, &(stateInfo->fd), NULL);
  if (i != SQL_SUCCESS) {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_close:SQLSetParam"); SQLDisconnect(stateInfo->hdbc);SQLFreeConnect(stateInfo->hdbc);SQLFreeEnv(stateInfo->henv);free(stateInfo);return i;}
  sprintf(queryString, "FREE LOCATOR ?");
  i = SQLExecDirect(hstmt,  (unsigned char *) queryString, SQL_NTS);
  if (i != SQL_SUCCESS) {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_close:SQLExecDirect");SQLDisconnect(stateInfo->hdbc);SQLFreeConnect(stateInfo->hdbc);SQLFreeEnv(stateInfo->henv);free(stateInfo);return i;}
  i = SQLTransact(stateInfo->henv,stateInfo-> hdbc, SQL_COMMIT);
  if (i != SQL_SUCCESS) {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_close:SQLTransact"); SQLDisconnect(stateInfo->hdbc);SQLFreeConnect(stateInfo->hdbc);SQLFreeEnv(stateInfo->henv);free(stateInfo);return i;}
  i = SQLFreeStmt(hstmt, SQL_DROP);
  if (i != SQL_SUCCESS) 
    {elog (NOTICE, "srbC_db2_close: FreeStmt Error"); 
    SQLDisconnect(stateInfo->hdbc);SQLFreeConnect(stateInfo->hdbc);
    SQLFreeEnv(stateInfo->henv);free(stateInfo);
    return (DBLOBJ_FREE_STMT_ERROR);}
  i = SQLDisconnect(stateInfo->hdbc);
  if (i != SQL_SUCCESS)  
    {elog (NOTICE, "srbC_db2_close: Disconnect Error");
    SQLFreeConnect(stateInfo->hdbc);SQLFreeEnv(stateInfo->henv);
    free(stateInfo);return  DBLOBJ_DISCONNECT_ERROR;}
  i = SQLFreeConnect(stateInfo->hdbc);
  if (i != SQL_SUCCESS) 
    {elog (NOTICE, "srbC_db2_close: FreeConnect Error");
    SQLFreeEnv(stateInfo->henv);free(stateInfo);
    return  DBLOBJ_FREE_CONNECT_ERROR;}
  i = SQLFreeEnv(stateInfo->henv);
  if (i != SQL_SUCCESS) 
    {elog (NOTICE, "srbC_db2_close:  FreeEnv Error");
    free(stateInfo); return ( DBLOBJ_FREE_ENV_ERROR);}
  
  free(stateInfo);
  return 0;
}

int srbC_db2_read(dbLobjDesc *exfd, char *buffer, int length)
{
      int             hstmt;
      int             size;
      int             bloc;
      db_state_info *stateInfo;

      
  stateInfo = (db_state_info *) exfd->dbSpecificInfo; 
    if (exfd->dbSystemType != DB2_LOBJ)
      {	elog (NOTICE, "srbC_db2_read: Bad DB2_LOBJ Type: %i",
	      exfd->dbSystemType);return  DBLOBJ_TYPE_ERROR;}
  bloc = stateInfo->locpos;
  if (length < 1 || stateInfo->locpos > stateInfo->lobsize)
    return 0;
  if (length > (stateInfo->lobsize - stateInfo->locpos + 1)) 
    length = stateInfo->lobsize - stateInfo->locpos + 1;
  i = SQLAllocStmt(stateInfo->hdbc, (long *) &hstmt); 
  if (i != SQL_SUCCESS) 
    {elog (NOTICE, "srbC_db2_read: AllocStmt Error");
    return DBLOBJ_ALLOC_STMT_ERROR;}
  i = SQLGetSubString(hstmt, SQL_C_BLOB_LOCATOR, stateInfo->fd,
		      stateInfo->locpos, length, SQL_C_BINARY, buffer, 
		      length + 1, (long *) &size,0);
  if (i != SQL_SUCCESS && i != SQL_SUCCESS_WITH_INFO)
     {
       if (i != SQL_NO_DATA)
	 {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_read:SQLGetSubString"); return i;}
     }
  stateInfo->locpos = stateInfo->locpos + size;
  i = SQLFreeStmt(hstmt, SQL_DROP);
  if (i != SQL_SUCCESS) {elog (NOTICE, "srbC_db2_read: FreeStmt Error");
      return DBLOBJ_FREE_STMT_ERROR;}
  return (stateInfo->locpos - bloc);
}

int srbC_db2_seek(dbLobjDesc *exfd, int offset, int whence)
{
  db_state_info *stateInfo;

    if (exfd->dbSystemType != DB2_LOBJ)
      {elog (NOTICE, "srbC_db2_seek: Bad DB2_LOBJ Type: %i",
	      exfd->dbSystemType);	return  DBLOBJ_TYPE_ERROR;}
  stateInfo = (db_state_info *) exfd->dbSpecificInfo; 
 switch (whence)
   {
     case SEEK_SET:
		   /* The 1 is added because the internal representation
		    * starts with byte 1 when the UNIX implemnetation 
		    * starts with byte 0.
		    */

                   stateInfo->locpos = offset + 1;
		   break;
     case SEEK_CUR:
                   stateInfo->locpos = stateInfo->locpos + offset;
		   break;
     case SEEK_END:
                   stateInfo->locpos = stateInfo->lobsize + offset;
		   break;
   }
     if (stateInfo->locpos < 1) stateInfo->locpos = 1;
     if (stateInfo->locpos > (stateInfo->lobsize + 1))  
       stateInfo->locpos = stateInfo->lobsize + 1;

    /* Substact 1 because the internal representation starts with byte 1. */
     return (stateInfo->locpos - 1);
}

int srbC_db2_write(dbLobjDesc *exfd, char *buffer, int length)
{

  db_state_info *stateInfo;

      int             hstmt;
      int             size;
      int             bloc;
      SQLCHAR            stmtbuf[STRING_LEN1];
      char           *temp_buffer;
      SQLINTEGER           fd;
    SQLINTEGER           lobsize, locsize;
      
    stateInfo = (db_state_info *) exfd->dbSpecificInfo; 
    if (exfd->dbSystemType != DB2_LOBJ)
      {	elog (NOTICE, "srbC_db2_write: Bad DB2_LOBJ Type: %i",
	      exfd->dbSystemType);return  DBLOBJ_TYPE_ERROR;}
    bloc = stateInfo->locpos;
    i = SQLAllocStmt(stateInfo->hdbc, (long *) &hstmt); 
    if (i != SQL_SUCCESS) 
      {elog (NOTICE, "srbC_db2_write: AllocStmt Error");
      return DBLOBJ_ALLOC_STMT_ERROR;}
    if (stateInfo->locpos > 1)
      {
	temp_buffer = malloc(bloc);
	i = SQLGetSubString(hstmt, SQL_C_BLOB_LOCATOR, stateInfo->fd,
			    1, bloc - 1, SQL_C_BINARY, temp_buffer, 
			    bloc , (long *) &size,0);
	if (i != SQL_SUCCESS && i != SQL_SUCCESS_WITH_INFO)
	  {
	    if (i != SQL_NO_DATA) { i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLGetSubString");free(temp_buffer); return (i);}
	  }
	if (size != bloc - 1) {SQLFreeStmt(hstmt, SQL_DROP);free(temp_buffer); return DBLOBJ_SIZE_ERROR;}
	
	sprintf((char *) stmtbuf,
		"UPDATE %s  SET %s = CAST( ? AS BLOB(100M)) || CAST( ? AS BLOB(100M))  WHERE %s",
		stateInfo->tableName,   stateInfo->outColName,
		stateInfo->inCondition);
	i = SQLPrepare(hstmt, stmtbuf, SQL_NTS);
	if (i != SQL_SUCCESS) {free(temp_buffer); return (errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLPrepare"));}
	bloc = bloc -1;
	i = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BLOB,
			     bloc, 0, temp_buffer, bloc, (long *) &bloc);
	if (i != SQL_SUCCESS) {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLBindParameter"); free(temp_buffer); return i;}
	i = SQLBindParameter(hstmt, 2, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BLOB,
			     length, 0, buffer, length, (long *) &length);
	if (i != SQL_SUCCESS) {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLBindParameter"); free(temp_buffer); return i;}
	i = SQLExecute(hstmt);
	free(temp_buffer); 
	if (i != SQL_SUCCESS) {return (errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLExecute"));}
      }
    else
      {
	sprintf((char *) stmtbuf,
		"UPDATE %s  SET %s = ?  WHERE %s",
		stateInfo->tableName,   stateInfo->outColName,
		stateInfo->inCondition);
	i = SQLPrepare(hstmt, stmtbuf, SQL_NTS);
	if (i != SQL_SUCCESS) { return(errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLPrepare")); }
	i = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BLOB,
			     length, 0, buffer, length,  (long *) &length);
	if (i != SQL_SUCCESS) { return(errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLBindParameter")); }
	i = SQLExecute(hstmt);
	if (i != SQL_SUCCESS) {return(errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLExecute")); }
	bloc =  bloc -1; /*add 97/8/21 sn cfrst block wrt had size less by 1*/
      }
    i = SQLSetParam(hstmt, 1, SQL_C_BLOB_LOCATOR, SQL_BLOB_LOCATOR, 0,
		    0, &(stateInfo->fd), NULL);
    if (i != SQL_SUCCESS) {return(errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLSetParameter")); }
    sprintf(queryString, "FREE LOCATOR ?");
    i = SQLExecDirect(hstmt,  (unsigned char *) queryString, SQL_NTS);
    if (i != SQL_SUCCESS) {return(errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLExecDirect")); }
    
    sprintf(queryString, "select %s from %s where %s",
	    stateInfo->outColName, stateInfo->tableName,
	    stateInfo->inCondition);
    i = SQLExecDirect(hstmt, (unsigned char *) queryString, SQL_NTS);
    if (i != SQL_SUCCESS) {return(errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLExecDirect")); }
    i = SQLBindCol(hstmt, 1, SQL_C_BLOB_LOCATOR, &fd,0, &locsize);
    if (i != SQL_SUCCESS) {return(errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLBindCol")); }
    i = SQLFetch(hstmt);
    if (i != SQL_SUCCESS && i != SQL_SUCCESS_WITH_INFO)
      {
	if (i != SQL_NO_DATA){return(errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLFetch")); }
      }
    
    i = SQLAllocStmt(stateInfo->hdbc, (long *) &hstmt2); 
    if (i != SQL_SUCCESS) { return(errorDb2Message(i,hstmt,stateInfo,"srbC_db2_write:SQLAllocStmt")); }
    i = SQLGetLength(hstmt2, SQL_C_BLOB_LOCATOR, fd, &lobsize, 0);
    if (i != SQL_SUCCESS) { i = errorDb2Message(i,hstmt2,stateInfo,"srbC_db2_write:SQLGetLength"); SQLFreeStmt(hstmt, SQL_DROP);return i;}
    stateInfo->fd = fd;
    stateInfo->lobsize = lobsize;
    stateInfo->locpos  = lobsize + 1;
    i = SQLFreeStmt(hstmt2, SQL_DROP);
    if (i != SQL_SUCCESS) 
      {elog (NOTICE, "srbC_db2_write:  FreeStmt Error");
      return DBLOBJ_FREE_STMT_ERROR;}
    i = SQLFreeStmt(hstmt, SQL_DROP);
    if (i != SQL_SUCCESS)
      {elog (NOTICE, "srbC_db2_write:  FreeStmt Error");
      return DBLOBJ_FREE_STMT_ERROR;}    
    return (stateInfo->locpos - bloc -1);
}



/* srbC_db2_create - Create a DB2 large object.
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         For Illustra, instance is not used.
 *         char *dataPathName - Format : tablename/outCol/inCondition
 */

int srbC_db2_create(dbLobjDesc *exfd, char *resourceLocationName, 
                  char *dataPathName, char *uid, char *pwd)

{
  db_state_info *stateInfo;

    int             henv;
    int             hdbc;

    char             instance[STRING_LEN1],hostAddr[STRING_LEN1];
    char database[STRING_LEN1], tmpStr[STRING_LENMAX];
    char columnSet[STRING_LENMAX], valueSet[STRING_LENMAX];

    char s1[100];

    SQLRETURN            i;
    SQLINTEGER           fd;
    SQLINTEGER           lobsize, locsize;
    SQLINTEGER           length = 1;
    SQLCHAR             buffer[] = " ";
    SQLCHAR             stmtbuf[STRING_LEN1];

    /* dataPathName must begin with a '/' */

    if (*dataPathName != '/')
	return DBLOBJ_PATH_NAME_ERROR;

    dataPathName ++;		/* skip the '/' */

    /* First allocate an area for state information */
    stateInfo = malloc(sizeof (db_state_info));

    if (exfd->dbSystemType != DB2_LOBJ)
      {	elog (NOTICE, "srbC_db2_create: Bad DB2_LOBJ Type: %i",
	      exfd->dbSystemType);return  DBLOBJ_TYPE_ERROR;}


    /*if (mySplitStr(dataPathName, stateInfo->tableName, 
      stateInfo->objectName,    '/') == NULL) {
        elog (NOTICE, 
	  "srbC_db2_create: Error in dataPathName : %s, must be in the form tableName/objectName",
        dataPathName);
	free(stateInfo);
	return -1;
    }
    */

    if (mySplitStr(dataPathName, stateInfo->tableName,tmpStr,'/') == NULL) 
      {
	elog (NOTICE, "srbC_db2_create: Illegal dataPathName format: %s",
	      dataPathName);
        free(stateInfo);return(DBLOBJ_PATH_NAME_ERROR);
      }
    if (mySplitStr(tmpStr, stateInfo->outColName,
		    stateInfo->inCondition,'/') == NULL) 
      {
	elog (NOTICE, "srbC_db2_create: Illegal dataPathName format: %s",
	      dataPathName);
        free(stateInfo);return(DBLOBJ_PATH_NAME_ERROR);
      }

    separateColVal(stateInfo->inCondition,  columnSet, valueSet);
    convertAnd(stateInfo->inCondition);



    if (mySplitStr(resourceLocationName, hostAddr, tmpStr, ':') == NULL) {
        elog (NOTICE, "srbC_db2_create: Error in resourceLocationName : %s, must be in the form hostAddr:database:instance",
        resourceLocationName);
	free(stateInfo);
        return DBLOBJ_RESOURCE_NAME_ERROR;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "srbC_db2_create: Error in resourceLocationName : %s",
        resourceLocationName);
	free(stateInfo);
        return DBLOBJ_RESOURCE_NAME_ERROR;
    }

  sprintf(s1,"DB2INSTANCE=%s",instance);
  i = putenv(s1);
  if (i != SQL_SUCCESS) {free(stateInfo);return DBLOBJ_PUTENV_ERROR;}
  i = SQLAllocEnv((long *) &henv);
  if (i != SQL_SUCCESS) {free(stateInfo);return DBLOBJ_ALLOC_ENV_ERROR;}
  i = SQLAllocConnect(henv, (long *) &hdbc);
  if (i != SQL_SUCCESS) {SQLFreeEnv(henv);free(stateInfo);return DBLOBJ_ALLOC_CONNECT_ERROR;}
  i = SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
  if (i != SQL_SUCCESS) 
    {SQLFreeConnect(hdbc);SQLFreeEnv(henv);free(stateInfo);return DBLOBJ_OPTION_SETTING_ERROR;}
  i = SQLConnect(hdbc, (unsigned char*) database, SQL_NTS, 
	       (unsigned char*) uid, SQL_NTS, (unsigned char*) pwd,  SQL_NTS); 
  if (i != SQL_SUCCESS) 
    {SQLFreeConnect(hdbc);SQLFreeEnv(henv);free(stateInfo);return DBLOBJ_CONNECT_ERROR;}

  stateInfo->henv  = henv;
  stateInfo->hdbc  = hdbc;

  i = SQLAllocStmt(hdbc, (long *) &hstmt); 
  if (i != SQL_SUCCESS) {SQLDisconnect(hdbc);SQLFreeConnect(hdbc);SQLFreeEnv(henv);free(stateInfo);return  DBLOBJ_ALLOC_STMT_ERROR ;}

    sprintf((char *) stmtbuf,
          "insert into %s ( %s %s) values (%s  ?)",
	  stateInfo->tableName, columnSet, stateInfo->outColName, valueSet);
    i = SQLPrepare(hstmt, stmtbuf, SQL_NTS);
    if (i != SQL_SUCCESS) 
      { i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_create:SQLPrepare"); SQLFreeStmt(hstmt, SQL_DROP);SQLFreeConnect(hdbc);SQLFreeEnv(henv);free(stateInfo);return i;}
    i = SQLBindParameter(hstmt, 1, SQL_PARAM_INPUT, SQL_C_BINARY, SQL_BLOB,
			 length, 0, buffer, length,  (long *) &length);
    if (i != SQL_SUCCESS) { i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_create:SQLBindParameter");SQLFreeConnect(hdbc);SQLFreeEnv(henv);free(stateInfo);return i;}
    i = SQLExecute(hstmt);
    if (i != SQL_SUCCESS) {i=errorDb2Message(i,hstmt,stateInfo,"srbC_db2_open:SQLExecute");SQLFreeConnect(hdbc);SQLFreeEnv(henv);free(stateInfo);return(i);}

  sprintf(queryString, "select %s from %s where %s ",
	  stateInfo->outColName, stateInfo->tableName, 
	  stateInfo->inCondition);

  i = SQLExecDirect(hstmt, (unsigned char *) queryString, SQL_NTS);
  if (i != SQL_SUCCESS) {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_open:SQLExecDirect");SQLDisconnect(hdbc);SQLFreeConnect(hdbc);SQLFreeEnv(henv);free(stateInfo);return i;}
  i = SQLBindCol(hstmt, 1, SQL_C_BLOB_LOCATOR, &fd,0, &locsize);
  if (i != SQL_SUCCESS) {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_open:SQLBindCol"); SQLDisconnect(hdbc);SQLFreeConnect(hdbc);SQLFreeEnv(henv);free(stateInfo);return i;}
  i = SQLFetch(hstmt);
  if (i != SQL_SUCCESS && i != SQL_SUCCESS_WITH_INFO)
     {
       if (i != SQL_NO_DATA){i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_open:SQLFetch"); SQLDisconnect(hdbc);SQLFreeConnect(hdbc);SQLFreeEnv(henv);free(stateInfo);return i; }
     }
  stateInfo->fd = fd;
  stateInfo->lobsize = 0;
  stateInfo->locpos  = 1;
  i = SQLFreeStmt(hstmt, SQL_DROP);
  if (i != SQL_SUCCESS) { SQLDisconnect(hdbc);SQLFreeConnect(hdbc);SQLFreeEnv(henv);free(stateInfo);return (DBLOBJ_FREE_STMT_ERROR);}
  exfd->dbSpecificInfo 
       = (char *)stateInfo;     /* return ptr to state information
                                   for this connection */
  return 0;
}



/* srbC_db2_unlink - Delete a DB2 large object.
 *
 * Input : char *resourceLocationName - format : host:database:instance.
 *         For Illustra, instance is not used.
 *         char *dataPathName - Format : tablename/objectID
 */

int srbC_db2_unlink(char *resourceLocationName, 
                  char *dataPathName, char *uid, char *pwd)

{
    int             henv;
    int             hdbc;
    db_state_info *stateInfo;

    char             instance[STRING_LEN1],hostAddr[STRING_LEN1];
        char database[STRING_LEN1], tmpStr[STRING_LENMAX];

    char s1[100];

    SQLRETURN            i;
    SQLINTEGER           fd;
    SQLINTEGER           lobsize, locsize;
    SQLINTEGER           length = 1;
    SQLCHAR             buffer[] = " ";
    SQLCHAR             stmtbuf[STRING_LEN1];

    /* dataPathName must begin with a '/' */

    if (*dataPathName != '/')
        return DBLOBJ_PATH_NAME_ERROR;

    dataPathName ++;            /* skip the '/' */

    /* First allocate an area for state information */
    stateInfo = malloc(sizeof (db_state_info));
    if (stateInfo == NULL)   {	return DBLOBJ_STATE_ALLOC_ERROR;}

    /*if (mySplitStr(dataPathName, stateInfo->tableName, stateInfo->objectName,
      '/') == NULL) {
        elog (NOTICE, "srbC_db2_unlink: Error in dataPathName : %s, must be in the form tableName/objectName",
        dataPathName);
	free(stateInfo);
        return DBLOBJ_PATH_NAME_ERROR;
    }
    */
   if (mySplitStr(dataPathName, stateInfo->tableName,tmpStr,'/') == NULL) 
      {
	elog (NOTICE, "srbC_db2_unlink: Illegal dataPathName format: %s",
	      dataPathName);
        free(stateInfo);return(DBLOBJ_PATH_NAME_ERROR);
      }
    if (mySplitStr(tmpStr, stateInfo->outColName,
		    stateInfo->inCondition,'/') == NULL) 
      {
	elog (NOTICE, "srbC_db2_unlink: Illegal dataPathName format: %s",
	      dataPathName);
        free(stateInfo);return(DBLOBJ_PATH_NAME_ERROR);
      }
    convertAnd(stateInfo->inCondition);

    if (mySplitStr(resourceLocationName, hostAddr, tmpStr, ':') == NULL) {
        elog (NOTICE, "srbC_db2_unlink: Error in resourceLocationName : %s, must be in the form hostAddr:database:instance",
        resourceLocationName);
	free(stateInfo);
        return DBLOBJ_RESOURCE_NAME_ERROR;
    }
    if (mySplitStr(tmpStr, database, instance, ':') == NULL) {
        elog (NOTICE, "db2unlink: Error in resourceLocationName : %s",
        resourceLocationName);
	free(stateInfo);
        return DBLOBJ_RESOURCE_NAME_ERROR;
    }

  sprintf(s1,"DB2INSTANCE=%s",instance);
  i = putenv(s1);
  if (i != SQL_SUCCESS) {elog (NOTICE, "srbC_db2_unlink: Instance Error");
                           free(stateInfo);return DBLOBJ_PUTENV_ERROR;}
  i = SQLAllocEnv((long *) &henv);
  if (i != SQL_SUCCESS) {elog (NOTICE, "srbC_db2_unlink: AllocEnv Error");
                           free(stateInfo); return DBLOBJ_ALLOC_ENV_ERROR;}
  i = SQLAllocConnect(henv, (long *) &hdbc);
  if (i != SQL_SUCCESS) {elog (NOTICE,"srbC_db2_unlink: AllocConnect Error");
                           free(stateInfo);SQLFreeEnv(henv);return DBLOBJ_ALLOC_CONNECT_ERROR;}
  i = SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
  if (i != SQL_SUCCESS) {elog (NOTICE,"srbC_db2_unlink: SetConOption Error");
                           free(stateInfo);
			   SQLFreeConnect(hdbc); SQLFreeEnv(henv);
			   return DBLOBJ_OPTION_SETTING_ERROR;}
  i = SQLConnect(hdbc, (unsigned char*) database, SQL_NTS, 
	       (unsigned char*) uid, SQL_NTS, (unsigned char*) pwd,  SQL_NTS); 
  if (i != SQL_SUCCESS) {elog (NOTICE, "srbC_db2_unlink: Connect Error");
                          free(stateInfo); SQLFreeConnect(hdbc); SQLFreeEnv(henv);return  DBLOBJ_CONNECT_ERROR;}

  stateInfo->henv  = henv;
  stateInfo->hdbc  = hdbc;


 
  i = SQLAllocStmt(hdbc, (long *) &hstmt); 
  if (i != SQL_SUCCESS) {elog (NOTICE, "srbC_db2_unlink: AllocStmt Error");
                           free(stateInfo);
			   SQLDisconnect(hdbc);
			   SQLFreeConnect(hdbc); SQLFreeEnv(henv);
			   return DBLOBJ_ALLOC_STMT_ERROR;}

  sprintf(queryString, "delete from %s where %s",
	  stateInfo->tableName, 
		 stateInfo->inCondition);

  i = SQLExecDirect(hstmt, (unsigned char *) queryString, SQL_NTS);
  if (i != SQL_SUCCESS) {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_unlink:SQLExecDirect");
                           free(stateInfo); 
			   SQLDisconnect(hdbc);
			   SQLFreeConnect(hdbc); SQLFreeEnv(henv);
			   SQLFreeStmt(hstmt, SQL_DROP);
			   return i;}
  i = SQLTransact(stateInfo->henv,stateInfo-> hdbc, SQL_COMMIT);
  if (i != SQL_SUCCESS) {i =errorDb2Message(i,hstmt,stateInfo,"srbC_db2_unlink:SQLTransact");
                           free(stateInfo); 
			   SQLDisconnect(hdbc);
			   SQLFreeConnect(hdbc); SQLFreeEnv(henv);
			   SQLFreeStmt(hstmt, SQL_DROP);
			   return i;}
  i = SQLFreeStmt(hstmt, SQL_DROP);
  if (i != SQL_SUCCESS) {elog (NOTICE, "srbC_db2_unlink: FreeStmt Error");
                            free(stateInfo); 
			    SQLDisconnect(hdbc);
			    SQLFreeConnect(hdbc); SQLFreeEnv(henv);
			    return DBLOBJ_FREE_STMT_ERROR;}
  i = SQLDisconnect(stateInfo->hdbc);
  if (i != SQL_SUCCESS) {elog (NOTICE, "srbC_db2_unlink: Disconnect Error");
                           free(stateInfo);
			   SQLFreeConnect(hdbc); SQLFreeEnv(henv);
			   return  DBLOBJ_DISCONNECT_ERROR;}
  i = SQLFreeConnect(stateInfo->hdbc);
  if (i != SQL_SUCCESS) {elog (NOTICE, "srbC_db2_unlink: FreeConnect Error");
                           free(stateInfo);
			   SQLFreeEnv(henv);
			   return  DBLOBJ_FREE_CONNECT_ERROR;}
  i = SQLFreeEnv(stateInfo->henv);
  if (i != SQL_SUCCESS) {elog (NOTICE, "srbC_db2_unlink: FreeEnv Error");
                            free(stateInfo);
			    return (DBLOBJ_FREE_ENV_ERROR);}
  free(stateInfo);
  return 0;
}
