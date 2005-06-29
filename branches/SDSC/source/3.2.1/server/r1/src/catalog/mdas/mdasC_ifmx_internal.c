/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/****************************************************************************
** Source File Name = mdas_infmx_internal.c
**
**
** PURPOSE: 
**     Contains driver-level library functions to 
**     connect and operate with Informix databases. 
**
** CALLABLE FUNCTIONS:
**     open_env_db2
**     close_env_db2
**     connect_db2
**     disconnect_db2
**     exec_no_res_sql_db2
**     exec_sql_db2
**     done_with_result_db2
**     done_with_default_result_db2
**     bind_default_row_db2
**     bind_n_rows_db2
**     bind_column_db2
**     bind_file_to_cell_db2
**     bind_files_to_column_db2
**     get_row_db2
**     get_rows_db2
**     get_no_of_columns_db2
**     get_column_info_db2
**
** INTERNAL FUNCTIONS USED:
**     logmess_db2
**     print_error_db2
**     check_error_db2
**
** EXTERNAL FUNCTIONS USED: FROM "sqlcli1.h"
**     SQLAllocHandle
**     SQLBindCol
**     SQLBindFileToCol
**     SQLColAttribute
**     SQLConnect
**     SQLDescribeCol
**     SQLDisconnect
**     SQLError
**     SQLExecDirect
**     SQLExtendedFetch
**     SQLFetch
**     SQLFreeHandle
**     SQLNumResultCols
**     SQLSetConnectAttr
**     SQLSetStmtAttr
**     SQLSetEnvAttr
**     SQLEndTran
****************************************************************************/

/****************************************************************************
                             INCLUDES & EXTERNS
****************************************************************************/


/* #include "sqlcli1.h" */
#include <stdlib.h>
#include <infxsql.h>
#include <infxcli.h>
#include <sqlucode.h>
#include "mdasC_ifmx_externs.h"
#include "mdasGlobalsExtern.h"
#include "srbErrno.h" 
#include "elogExtern.h"

/* to get debugging for this file only */
#define DEBUGON
FILE *LOGMESSFILE;
char logmess[HUGH_HUGE_STRING];
unsigned char bind_file_name[MAX_TOKEN];
short int  bind_file_name_length;
unsigned long  int  bind_file_option;
long int  bind_file_status;


/****************************************************************************
                                  FUNCTIONS                                  
****************************************************************************/
/***************************************************************************
  NAME   : logmess_db2
  PURPOSE: Logging message in LOGMESS file handle
****************************************************************************/

void logmess_db2(char *message,
	     SQLHENV henv,
	     SQLHDBC hdbc,
	     SQLHSTMT hstmt,
	     SQLRETURN rc)
{
   time_t *mmmmclk, mmmmcl;
   
   mmmmcl = time(NULL);

#ifdef DEBUGON
   fprintf(LOGMESSFILE,"%24.24s =>%4i  %s\n",ctime(&mmmmcl),rc, message);
#endif /* DEBUGON */ 
}

/***************************************************************************
  NAME   : print_error_db2
  PURPOSE: Prints error messages to ERRMESS file handle.
****************************************************************************/

void print_error_db2(SQLHENV henv,
            SQLHDBC hdbc,
            SQLHSTMT hstmt,
            SQLRETURN frc,  
            int line,       
            char * file)  
{
    SQLCHAR         buffer[SQL_MAX_MESSAGE_LENGTH + 1];
    SQLCHAR         sqlstate[SQL_SQLSTATE_SIZE + 1];
    SQLINTEGER      sqlcode;
    SQLSMALLINT     length,handletype;
    SQLRETURN       retcode;
    SQLHANDLE       handle;
    SQLSMALLINT     errNum = 1;

#ifdef DEBUGON 

    /* Determine which handle is defined and report error for that */
    if ((hdbc == SQL_NULL_HDBC) && (hstmt == SQL_NULL_HSTMT)) {
        handletype = SQL_HANDLE_ENV;
        handle = henv;
    } else if ((henv == SQL_NULL_HENV) && (hstmt == SQL_NULL_HSTMT)) {
        handletype = SQL_HANDLE_DBC;
        handle = hdbc;
    } else if ((henv == SQL_NULL_HENV) && (hdbc == SQL_NULL_HDBC)) {
        handletype = SQL_HANDLE_STMT;
        handle = hstmt;
    } else {
        fprintf(ERRMESSFILE,"Cannot determine where error is henv, hdbc, or hstmt\n");
        return;
    }

    fprintf(ERRMESSFILE,"ERROR: RC= %d Reported from %s, line %d \n",
           frc, file, line);

    /*while (retcode != SQL_NO_DATA) {*/
    while (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) {
        retcode = SQLGetDiagRec(handletype,handle,errNum,sqlstate,
                  &sqlcode, buffer,SQL_MAX_MESSAGE_LENGTH + 1, &length);
        fprintf(ERRMESSFILE,"         SQLSTATE: %s\n", sqlstate);
        fprintf(ERRMESSFILE,"Native Error Code: %ld\n", sqlcode);
        fprintf(ERRMESSFILE,"%s \n", buffer);
        errNum++;
    }
#endif /* DEBUGON */ 


}

/***************************************************************************
  NAME   : check_error_db2
  PURPOSE: Checks severity of error and prints  error messages by
           calling to print_error_db2(). Tries to roll back transaction
           when there is a fatal or unknown error.
****************************************************************************/
  
void
check_error_db2(SQLHENV henv,
            SQLHDBC hdbc,
            SQLHSTMT hstmt,
            SQLRETURN frc,
            int line,
            char *file)
{
    SQLRETURN       rc;

    print_error_db2(henv, hdbc, hstmt, frc, line, file);

    switch (frc) {
    case SQL_SUCCESS:
        break;
    case SQL_INVALID_HANDLE:
#ifdef DEBUGON 
        fprintf(ERRMESSFILE,"\nSQL_ERROR: Invalid Handle\n");
#endif /* DEBUGON */ 
        break;
    case SQL_ERROR:
      /*rc = SQLTransact(henv, hdbc, SQL_ROLLBACK);*/
      /* #ifdef DEBUGON
	sprintf(logmess,"NOT SQLTransact:%d", SQL_ROLLBACK);
	logmess_db2(logmess, henv,  hdbc, SQL_NULL_HSTMT, frc);
        if (rc != SQL_SUCCESS)
         fprintf(ERRMESSFILE,"FATAL SQL_ERROR: Transaction Rollback Failed\n");
        else
            fprintf(ERRMESSFILE,"FATAL SQL_ERROR: Transaction Rolled back\n");
	    #endif 
      */
        break;
    case SQL_SUCCESS_WITH_INFO:
        break;
    case SQL_NO_DATA_FOUND:
        break;
    default:
       /*rc = SQLTransact(henv, hdbc, SQL_ROLLBACK);*/
#ifdef DEBUGON
	sprintf(logmess,"NOT SQLTransact:%d", SQL_ROLLBACK);
	logmess_db2(logmess, henv,  hdbc, SQL_NULL_HSTMT, frc);
#endif /* DEBUGON */
#ifdef DEBUGON
	      if (rc != SQL_SUCCESS)
            fprintf(ERRMESSFILE,
		    "FATAL SQL_UNKNOWN_ERROR: Transaction Rollback Failed\n");
        else
            fprintf(ERRMESSFILE,
		    "FATAL SQL_UNKNOWN_ERROR: Transaction Rolled back\n");
#endif /* DEBUGON */
        break;
    }
}          

/***************************************************************************
  NAME   : open_env_db2
  PURPOSE: Opening an execution environment in db2 
****************************************************************************/ 

/*int open_env_db2(SQLHENV *henv)*/
int open_env_db2(long int *henv)
{
   SQLRETURN rc;


   /*rc = SQLAllocEnv(henv); Deprecated*/
   rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, (SQLHENV) henv);
#ifdef DEBUGON
   logmess_db2("SQLAllocHandle: SQL_HANDLE_ENV:",(SQLHENV) *henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2((SQLHENV) *henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }

   /* Set ODBC to Version 3 */
   rc = SQLSetEnvAttr ((SQLHENV)*henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0);
   logmess_db2("SQLSetEnvAttr: ",(SQLHENV) *henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2((SQLHENV) *henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }

   return(0);
}

/***************************************************************************
  NAME   : close_env_db2
  PURPOSE: Closing an open execution environment in db2 
****************************************************************************/
   
/*int close_env_db2(SQLHENV henv)*/
int close_env_db2(long int henv)
{
   SQLRETURN rc;

   /*rc = SQLFreeEnv(henv); Deprecated*/
   rc = SQLFreeHandle(SQL_HANDLE_ENV,(SQLHENV) henv);
#ifdef DEBUGON
   logmess_db2("SQLFreeHandle: SQL_HANDLE_ENV: ",(SQLHENV) henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2((SQLHENV)henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   return(0);
}

/***************************************************************************
  NAME   : connect_db2
  PURPOSE: Opening a connection to db2 server
****************************************************************************/

/*int connect_db2( SQLHENV henv,
		  SQLHDBC *hdbc,
		  SQLCHAR *databasename, 
		 SQLCHAR *user, 
		 SQLCHAR *password)*/
int connect_db2( long int henv,
		 long int *hdbc,
		 unsigned char *databasename, 
		 unsigned char *user, 
		 unsigned char *password)
{
   SQLRETURN rc;

   /*rc = SQLAllocConnect(henv, hdbc);  Deprecated*/
   rc = SQLAllocHandle(SQL_HANDLE_DBC,(SQLHENV) henv, (SQLHDBC) hdbc);  
#ifdef DEBUGON
   logmess_db2("SQLAllocHandle: SQL_HANDLE_DBC: ",(SQLHENV) henv,(SQLHDBC) *hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2((SQLHENV) henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   /*rc = SQLSetConnectOption(*hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF); Deprecated*/
   rc = SQLSetConnectAttr((SQLHDBC) *hdbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF, 0); 
#ifdef DEBUGON
   logmess_db2("SQLSetConnectAttr:SQL_ATTR_AUTOCOMMIT_OFF",(SQLHENV) henv,(SQLHDBC) *hdbc, 
	       SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */


   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2((SQLHENV) henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-2);
     }
   rc = SQLConnect((SQLHDBC) *hdbc, databasename, SQL_NTS, user, SQL_NTS, password, 
		   SQL_NTS);

#ifdef DEBUGON
   sprintf(logmess, "SQLConnect: Database = %s, User = %s, %s", databasename, user, password);
   logmess_db2(logmess,(SQLHENV) henv,(SQLHDBC) *hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */


   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2(SQL_NULL_HENV,(SQLHDBC) *hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       
       rc = SQLDisconnect((SQLHDBC) *hdbc);
#ifdef DEBUGON
       logmess_db2("SQLDisconnect",(SQLHENV) henv,(SQLHDBC) *hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */
       /*rc = SQLFreeConnect(*hdbc); Deprecated */
       rc = SQLFreeHandle(SQL_HANDLE_DBC,(SQLHDBC) *hdbc);
#ifdef DEBUGON
       logmess_db2("SQLFreeHandle: SQL_HANDLE_DBC: ",(SQLHENV) henv,(SQLHENV) *hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */
       return(-3);
     }
   return(0);
}

/***************************************************************************
  NAME   : disconnect_db2
  PURPOSE: Closing an open connection  in db2 

****************************************************************************/
 
/*int disconnect_db2(SQLHENV henv,
	    SQLHDBC hdbc, SQLUSMALLINT transstate)*/
int disconnect_db2(long int henv,
	           long int hdbc, unsigned short int transstate)
{
  SQLRETURN rc;
   

   /*rc = SQLTransact(henv, hdbc, transstate); */
   rc = SQLEndTran(SQL_HANDLE_DBC, (SQLHDBC) hdbc, (SQLUSMALLINT) transstate);
#ifdef DEBUGON
   sprintf(logmess,"SQLEndTran:%d",(SQLUSMALLINT) transstate);
   logmess_db2(logmess,(SQLHENV) henv, (SQLHDBC) hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2(SQL_NULL_HENV, (SQLHDBC) hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     } 
   rc = SQLDisconnect((SQLHDBC)hdbc);
#ifdef DEBUGON
   logmess_db2("SQLDisconnect",(SQLHENV) henv,  SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2((SQLHENV) henv, (SQLHDBC)hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-2);
     }
   /*rc = SQLFreeConnect(hdbc); */
   rc = SQLFreeHandle(SQL_HANDLE_DBC,(SQLHDBC)hdbc);
#ifdef DEBUGON
   logmess_db2("SQLFreeHandle: SQL_HANDLE_DBC: ",(SQLHENV) henv,  SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */
   

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2((SQLHENV)henv, (SQLHDBC)hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-3);
     }
   return (0);
}

/***************************************************************************
  NAME   : exec_no_res_sql_db2
  PURPOSE: Execute a SQL query which has no resulting table, eg. INSERT,
           DELETE, UPDATE or DDL
****************************************************************************/
 
/*int exec_no_res_sql_db2(SQLHENV henv, 
		  SQLHDBC hdbc, 
		  SQLCHAR *query)*/
int exec_no_res_sql_db2(long int henv, 
		  long int hdbc, 
		  unsigned char *query)
{
     SQLHSTMT        hstmt;
     SQLRETURN       rc;
     SQLRETURN       rc1;
     SQLINTEGER         result;
     time_t		tim, time();

     filterQuery(query);
     if (showQuery) {
      time(&tim);
      printf("SQL:%d:%s: %s\n",getpid(),query,ctime(&tim)+4);
      fflush(stdout);
      strcpy(GENERATEDSQL,query);
     }

     /*rc = SQLAllocStmt(hdbc, &hstmt);  Deprecated */
     rc = SQLAllocHandle(SQL_HANDLE_STMT,(SQLHDBC) hdbc, &hstmt); 
#ifdef DEBUGON
     logmess_db2("SQLAllocHandle: SQL_HANDLE_STMT:", (SQLHENV)henv, (SQLHDBC) hdbc, hstmt, rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_db2(SQL_NULL_HENV,(SQLHDBC) hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	 return(-1);
       }
     rc = SQLExecDirect(hstmt, query, SQL_NTS);
#ifdef DEBUGON
     logmess_db2("SQLExecDirect",(SQLHENV) henv, (SQLHDBC) hdbc, hstmt, rc);
     logmess_db2(query,(SQLHENV) henv, (SQLHDBC) hdbc, hstmt, rc);
#endif /* DEBUGON */

     switch (rc) {
     case SQL_SUCCESS:
     case SQL_SUCCESS_WITH_INFO:
     case SQL_NO_DATA_FOUND:
       result = 0;
       break;
     case SQL_INVALID_HANDLE:
#ifdef DEBUGON
       check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       result = -2;
       break;
     default:
#ifdef DEBUGON
       check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       result = -3;
       break;
     }

     /*rc = SQLFreeStmt(hstmt, SQL_DROP); Deprecated*/
     rc = SQLFreeHandle(SQL_HANDLE_STMT,hstmt);
#ifdef DEBUGON
     logmess_db2("SQLFreeHandle: SQL_HANDLE_STMT: ",(SQLHENV) henv, (SQLHDBC) hdbc,  SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2((SQLHENV) henv,(SQLHDBC) hdbc,  hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-4);
     }
     return(result);
}

/***************************************************************************
  NAME   : exec_sql_db2
  PURPOSE: Execute a SQL query which mave have  resulting table
****************************************************************************/

/*int exec_sql_db2(SQLHENV henv, 
		  SQLHDBC hdbc, 
		  SQLHSTMT *hstmt,
		  SQLCHAR *query)*/
int exec_sql_db2(long int henv, 
		 long int hdbc, 
		 long int *hstmt,
		 unsigned char *query)
{
     SQLRETURN       rc, rc1;
     SQLINTEGER      result;
     time_t		tim, time();

     /*rc = SQLAllocStmt(hdbc, hstmt);  Deprecated*/
     rc = SQLAllocHandle(SQL_HANDLE_STMT,(SQLHDBC)hdbc,(SQLHSTMT) hstmt); 

     filterQuery(query);
     if (showQuery) {
      time(&tim);
      printf("SQL:%d:%s: %s\n",getpid(),query,ctime(&tim)+4);
      fflush(stdout);
      strcpy(GENERATEDSQL,query);
     }

#ifdef DEBUGON
     logmess_db2("SQLAllocHandle: SQL_HANDLE_STMT:",(SQLHENV) henv, (SQLHDBC) hdbc,(SQLHSTMT) *hstmt, rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_db2(SQL_NULL_HENV,(SQLHDBC) hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	 return(-1);
       }
     rc = SQLExecDirect((SQLHSTMT)*hstmt, query, SQL_NTS);
#ifdef DEBUGON
     logmess_db2("SQLExecDirect",(SQLHENV) henv,(SQLHDBC)  hdbc,(SQLHSTMT) *hstmt, rc);
     logmess_db2(query,(SQLHENV) henv,(SQLHDBC)  hdbc,(SQLHSTMT) hstmt, rc);
#endif /* DEBUGON */

     switch (rc) {
     case SQL_SUCCESS:
     case SQL_SUCCESS_WITH_INFO:
     case SQL_NO_DATA_FOUND:
       result = 0;
       break;
     case SQL_INVALID_HANDLE:
#ifdef DEBUGON
       check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) *hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

       result = -2;
       break;
     default:
#ifdef DEBUGON
       check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) *hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

       result = -3;
       break;
     }
     return(result);
}

/***************************************************************************
  NAME   : done_with_result_db
  PURPOSE: Close a stament environment opened by exec_sql_db2() but which
           used user allocated result structure
****************************************************************************/

/*int done_with_result_db2(SQLHSTMT hstmt)*/
int done_with_result_db2(long int hstmt)
{  
   SQLRETURN rc;

   /*rc = SQLFreeStmt(hstmt, SQL_DROP); Deprecated*/
   rc = SQLFreeHandle(SQL_HANDLE_STMT,(SQLHSTMT)hstmt);
#ifdef DEBUGON
   logmess_db2("SQLFreeHandle: SQL_HANDLE_STMT: ", SQL_NULL_HENV, SQL_NULL_HDBC,SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   return(0);
}

/***************************************************************************
  NAME   : done_with_default_result_db2
  PURPOSE: Close a stament environment opened by exec_sql_db2() which used
           result structure build by bind_default_row_db2()
****************************************************************************/
 
/*int done_with_default_result_db2(SQLHSTMT hstmt,
				  SQLCHAR *data[],
				  SQLINTEGE *data_size,
				  SQLSMALLINT num_of_cols)*/
int done_with_default_result_db2(long int hstmt,
				 unsigned char *data[],
				 long int *data_size,
				 short int num_of_cols)
{  
   SQLRETURN rc;
   int i;

   for (i =  0; i < num_of_cols; i++){
     free(data[i]);
   }
   /*rc = SQLFreeStmt(hstmt, SQL_DROP); Deprecated*/
   rc = SQLFreeHandle(SQL_HANDLE_STMT,(SQLHSTMT) hstmt);
#ifdef DEBUGON
   logmess_db2("SQLFreeHaldne: SQL_HANDLE_STMT: ",SQL_NULL_HENV, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   return(0);
}

/***************************************************************************
  NAME   : bind_default_row_db2
  PURPOSE: Allocate a default set of result holders to hold one row of 
           result of a SQL statement. 
****************************************************************************/
 
/*int bind_default_row_db2(SQLHSTMT hstmt,
		  SQLCHAR *data[],
		  SQLINTEGER data_size[], 
		  SQLSMALLINT *num_of_cols)*/
int bind_default_row_db2(long int hstmt,
		  unsigned char *data[],
		  long int data_size[], 
		  short int *num_of_cols)
{
   SQLRETURN       rc;
   int             i;
   SQLINTEGER      displaysize;
    SQLCHAR         colname[MAX_TOKEN];
    SQLSMALLINT     coltype;
    SQLSMALLINT     colnamelen;
    SQLSMALLINT     nullable;
    SQLUINTEGER      collen[MAX_TOKEN];  /* needed since max size of sdword is int size */
    SQLUINTEGER     precision; 
    SQLSMALLINT     scale;

    SQLCHAR         errmsg[ERRMSG_LEN];
    char newVal[MAX_TOKEN];
    char myerror[MAX_TOKEN];

   rc = SQLNumResultCols((SQLHSTMT)hstmt, num_of_cols);
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }

    for (i = 0; i < *num_of_cols; i++) {
        rc = SQLDescribeCol((SQLHSTMT)hstmt, i + 1, colname, sizeof(colname),
                       &colnamelen, &coltype, &precision, &scale, NULL);
	if (rc != SQL_SUCCESS)
	  {
#ifdef DEBUGON
	    check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	    return(-2);
	  }
        collen[i] = precision; 

        /*rc = SQLColAttributes(hstmt, i + 1, SQL_COLUMN_DISPLAY_SIZE, 
			      NULL, 0,    NULL, &displaysize);   Deprecated */
        rc = SQLColAttribute((SQLHSTMT) hstmt, i + 1, SQL_COLUMN_DISPLAY_SIZE, 
			      NULL, 0,    NULL, &displaysize);
	if (rc != SQL_SUCCESS)
	  {
#ifdef DEBUGON
	    check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	    return(-3);
	  }
       /* since lvarchar size is SDWORD = SQLINTEGER.... dirty, dirty since we have
      to get around the small varchar size in informix, we'll set the max size of lvarchar
      to the max that is allowable in the oracle version :( 2001*/
       if ((displaysize +1) < 0 && displaysize > 0) {
                   collen[i] = 2001;
		   logmess_db2("lvarchar hack",NULL,NULL,NULL,0);
       }
       else if (displaysize > (strlen((char *) colname)))
	           collen[i] = displaysize + 1;
       else
                   collen[i] = strlen((char *) colname) + 1;
       /*collen[i] = max(displaysize, strlen((char *) colname)) + 1;*/
        if (collen[i] == 0) 
		collen[i] = 1;
        data[i] = (SQLCHAR *) malloc((int)collen[i]);
	strcpy((char *)data[i],"");
 
        rc = SQLBindCol((SQLHSTMT) hstmt, i + 1, SQL_C_CHAR, data[i], collen[i],(SQLINTEGER) &data_size[i]);
	if (rc != SQL_SUCCESS)
	  {
#ifdef DEBUGON
	    check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	    return(-4);
	  }
    }
   return(0);
}

/***************************************************************************
  NAME   : get_row_db2
  PURPOSE: Fetch the next (unfetched)  row of result in the 
           row-holder bound by any of the bind_***_db2() calls. To fetch
           multiple row use get_rows_db2()
****************************************************************************/

/*int get_row_db2(SQLHSTMT hstmt)*/
int get_row_db2(long int hstmt)
{
   SQLRETURN       rc;

   rc =  SQLFetch((SQLHSTMT) hstmt);
   if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
     {
       if (rc != SQL_NO_DATA)
	 {
#ifdef DEBUGON
       check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */
	 }
       return(-1);
     }
   return(0);
}

/***************************************************************************
  NAME   : bind_column_db2
  PURPOSE: Allocate an array (or a variable) to hold a column (or cell)
           of results of a  get_rows_db2 statement. 
****************************************************************************/

/*int bind_column_db2(SQLHSTMT hstmt,
		   SQLPOINTER data_column,
		   SQLPOINTER size_column,
		   SQLSMALLINT column_type,
		   SQLINTEGER max_data_size,
		   SQLSMALLINT column_number,
		   SQLINTEGER row_size) */
int bind_column_db2(long int hstmt,
		   void *data_column,
		   void *size_column,
		   short int column_type,
		   long int max_data_size, /* in bytes */
		   short int column_number,
		   long int row_size)
{
   SQLRETURN       rc;

   if (row_size > 1)
     {
       /*rc = SQLSetStmtOption(hstmt, SQL_ROWSET_SIZE, row_size); Deprecated*/
       rc = SQLSetStmtAttr((SQLHSTMT) hstmt, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER) row_size, 0);

       if (rc != SQL_SUCCESS)
	 {
#ifdef DEBUGON
	   check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	   return(-1);
	 }
     }
   if (column_type == 0)
     {
       rc = SQLBindCol((SQLHSTMT) hstmt, column_number, SQL_C_DEFAULT, data_column,
		   max_data_size, size_column);
     }
   else
     {
       rc = SQLBindCol((SQLHSTMT) hstmt, column_number, column_type, data_column,
		   max_data_size, size_column);

     }
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-2);
     }
   return(0);
}

/***************************************************************************
  NAME   : bind_files_to_column_db2
  PURPOSE: Allocate an array of file name holders to hold a LOB column
           of results of a  get_rows_db2 statement. 
****************************************************************************/
 
/*int bind_files_to_column_db2(SQLHSTMT hstmt,
		   void        *file_names,
		   SQLSMALLINT *file_name_cell_size,
		   SQLUINTEGER  *file_options,
		   SQLINTEGER *file_status,
		   SQLINTEGER *result_size_column,
		   SQLUSMALLINT column_number,
		   SQLINTEGER row_size)*/
int bind_files_to_column_db2(long int hstmt,
		   void        *file_names,
		   short int *file_name_cell_size,
		   unsigned long int  *file_options,
		   long int *file_status,
		   long int *result_size_column,
		   unsigned short int column_number,
		   long int row_size)
{
/*    SQLRETURN       rc; */


/*    if (row_size > 1) */
/*      { */
/*        rc = SQLSetStmtOption(hstmt, SQL_ROWSET_SIZE, row_size); */
/*           if (rc != SQL_SUCCESS) */
/* 	    { */
/* #ifdef DEBUGON */
/* 	      check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,  */
/* 		     __LINE__, __FILE__); */
/* #endif /\* DEBUGON *\/ */

/* 	      return(-1); */
/* 	    } */
/*      } */
/*        rc = SQLBindFileToCol(hstmt, column_number, file_names,NULL,  */
/* 		   file_options, 255, result_size_column, file_status); */
/*    if (rc != SQL_SUCCESS) */
/*      { */
/* #ifdef DEBUGON */
/*        	 check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,  */
/* 		     __LINE__, __FILE__); */
/* #endif /\* DEBUGON *\/ */

/*          return(-2); */
/*      } */
    return(0);
}

/***************************************************************************
  NAME   : bind_file_to_cell_db2
  PURPOSE: Allocate a file name holder to hold a LOB 
           in a row of a  get_row_db2 statement. 
****************************************************************************/

/*int bind_file_to_cell_db2(SQLHSTMT hstmt,
		   SQLCHAR  file_name[],
		   SQLSMALLINT *file_name_length,
		   SQLUINTEGER  *file_option,
		   SQLINTEGER *file_status,
		   SQLINTEGER *result_size_column,
		   SQLUSMALLINT column_number)*/
int bind_file_to_cell_db2(long int hstmt,
		   unsigned char  file_name[],
		   short int *file_name_length,
		   unsigned long int  *file_option,
		   long int *file_status,
		   long int *result_size_column,
		   unsigned short int column_number)
{
/*   SQLRETURN       rc; */

/*    rc = SQLBindFileToCol(hstmt, column_number,  */
/* 			 bind_file_name, &bind_file_name_length, */
/* 			 &bind_file_option, 100, */
/* 			 result_size_column, &bind_file_status); */
/*    if (rc != SQL_SUCCESS) */
/*      { */
/* #ifdef DEBUGON */
/*        	 check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,  */
/* 		     __LINE__, __FILE__); */
/* #endif /\* DEBUGON *\/ */

/*          return(-1); */
/*      } */
  return(0);
}

/***************************************************************************
  NAME   : get_rows_db2
  PURPOSE: Fetch the next set of  rows of result in the 
           column-arrays bound by any of the bind_***_db2() calls. To fetch
           a single row use get_row_db2()
****************************************************************************/

/*int get_rows_db2(SQLHSTMT hstmt, SQLUINTEGER *pcrow)*/
int get_rows_db2(long int hstmt, unsigned long int *pcrow)
{
  SQLRETURN       rc;

  rc =  SQLExtendedFetch((SQLHSTMT) hstmt, SQL_FETCH_NEXT,0, (SQLUINTEGER) pcrow, NULL);
   if (rc != SQL_SUCCESS)
     {
         if (rc != SQL_NO_DATA)
#ifdef DEBUGON
       	 check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-1);
     }
  return(0);
}

/***************************************************************************
  NAME   : bind_n_rows_db2
  PURPOSE: Allocate arrays  to hold all column of the result of a 
           get_rows_db2 statement. 
****************************************************************************/

/*int bind_n_rows_db2(SQLHSTMT hstmt,
		  SQLPOINTER  data_column[],
		  SQLPOINTER  size_column[],
		  SQLINTEGER  max_data_size[],  
		  SQLSMALLINT column_type[],
		  SQLSMALLINT num_of_columns,
		  SQLINTEGER  row_size)*/
int bind_n_rows_db2(long int hstmt,
		  void  *data_column[],
		  void  *size_column[],
		  long int  max_data_size[],  /* in bytes */
		  short int column_type[],
		  short int num_of_columns,
		  long int  row_size)
{
  SQLRETURN       rc;
  int             i;
  SQLINTEGER      displaysize;

 if (row_size > 1)
   {
     /*rc = SQLSetStmtOption(hstmt, SQL_ROWSET_SIZE, row_size); Deprecated */
     rc = SQLSetStmtAttr((SQLHSTMT) hstmt, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER) row_size, 0);

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
       	 check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-1);
       }
   }
  for (i = 0; i < num_of_columns; i++){
    if (column_type[i] == 0)
      {
        rc = SQLBindCol((SQLHSTMT)hstmt, i + 1, SQL_C_DEFAULT, data_column[i], 
		    max_data_size[i], size_column[i]);
      }
    else
      {
        rc = SQLBindCol((SQLHSTMT)hstmt, i + 1, column_type[i], data_column[i], 
		    max_data_size[i], size_column[i]);
      }
     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
       	 check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-2);
       }
  }
  return(0);
}

/***************************************************************************
  NAME   : get_no_of_columns_db2
  PURPOSE: Find the number of columns of the result table of an
           exec_sql_db2 statement. 
****************************************************************************/
  
/*int get_no_of_columns_db2(SQLHSTMT hstmt,
			SQLSMALLINT *num_of_cols)*/
int get_no_of_columns_db2(long int hstmt,
			short int *num_of_cols)
{
  SQLRETURN       rc;

  rc = SQLNumResultCols((SQLHSTMT)hstmt, num_of_cols);
  if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
    	 check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-1);
     }
  return(0);
}

/***************************************************************************
  NAME   : get_column_info_db2
  PURPOSE: Get assorted column attributes  such as column name, size (as 
           defined in DDL), type, nullability, precision and its scale.
****************************************************************************/
 
/*int get_column_info_db2(SQLHSTMT hstmt,
			SQLSMALLINT  column_number,
			SQLINTEGER *displaysize,
			SQLCHAR      colname[],
			SQLSMALLINT *coltype,
                        SQLSMALLINT buf_size,
                        SQLSMALLINT *colnamelen,
                        SQLSMALLINT *nullable,
			SQLUINTEGER *precision,
                        SQLSMALLINT *scale
			)*/
int get_column_info_db2(long int hstmt,
			short int  column_number,
			long int *displaysize,
			unsigned char      colname[],
			short int *coltype,
                        short int buf_size,
                        short int *colnamelen,
                        short int *nullable,
			unsigned long int *precision,
                        short int *scale
			)
{
  SQLRETURN       rc;

  /*rc = SQLColAttributes(hstmt, column_number, SQL_COLUMN_DISPLAY_SIZE, 
			NULL, 0, NULL, displaysize);  Deprecated*/
  rc = SQLColAttribute((SQLHSTMT) hstmt, column_number, SQL_COLUMN_DISPLAY_SIZE, 
			NULL, 0, NULL, displaysize);
  if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
    	 check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-1);
     }
  rc = SQLDescribeCol((SQLHSTMT) hstmt, column_number, colname, buf_size,
                       colnamelen, coltype, precision, scale, nullable);
  if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
    	 check_error_db2(SQL_NULL_HENV, SQL_NULL_HDBC,(SQLHSTMT) hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-2);
     }
  return(0);
}

 
close_db2_interaction_internal(long int henv, 
		  long int hdbc, int transaction_end_code)

{ 
  MDAS_DB2_SERVER_HNDL *tmp_srvh;
  

  /* transaction_end_code = 1 ABORT TRANSACTION */
  /* transaction_end_code = 0 COMMIT TRANSACTION */
  i =  disconnect_db2( henv, hdbc, transaction_end_code); 
    if (i != 0)  
    return(MCAT_DISCONNECT_ERROR);
  i =  close_env_db2(henv);
  if (i != 0)  
    return(MCAT_CLOSE_ENV_ERROR);
#ifdef INSMESSON 
  fclose(INSMESSFILE);
#endif /* INSMESSON */ 
#ifdef DEBUGON 
  fclose(LOGMESSFILE); 
#endif /* DEBUGON */ 


  tmp_srvh = (MDAS_DB2_SERVER_HNDL *) serverhndl[0].conn; 
  free (tmp_srvh);

  return(MDAS_SUCCESS);

}

/*int open_db2_interaction_internal(SQLHENV *henv, 
		  SQLHDBC *hdbc)*/
int open_db2_interaction_internal(long int *henv, 
		  long int *hdbc)
{

 MDAS_DB2_SERVER_HNDL *tmp_srvh;
 
  i= initMdasEnviron();
  if (i != 0) 
    return (INIT_ENVIRON_ERROR);
  sprintf(db2instancebuffer,"DB2INSTANCE=%s",DB2INSTANCE);
  i = putenv(db2instancebuffer);
  if (i != 0) 
    {
    return (UNIX_PUTENV_ERROR);
    }
#ifdef DEBUGON 
  LOGMESSFILE = fopen(DB2LOGFILE,"a");
  if (LOGMESSFILE == NULL)
    {fprintf(stderr,"Cannot open log file:%s\n",DB2LOGFILE);
    return (INIT_ENVIRON_ERROR);
    }
#endif /* DEBUGON */ 
#ifdef INSMESSON
  INSMESSFILE = fopen(MDASINSERTSFILE,"a");
 if (INSMESSFILE == NULL)
    {fprintf(stderr,"Cannot open inserts file:%s\n",MDASINSERTSFILE);
    return (INIT_ENVIRON_ERROR);
#endif /* INSMESSON */ 

  i =  open_env_db2(henv);
  if (i != 0){
#ifdef INSMESSON 
    fclose(INSMESSFILE);
#endif /* INSMESSON */ 
#ifdef DEBUGON 
    fclose(LOGMESSFILE); 
#endif /* DEBUGON */ 
    return(MCAT_OPEN_ENV_ERROR);
  }
  i =  connect_db2( *henv, hdbc, (unsigned char*) server, 
            (unsigned char*) user, (unsigned char*) pass); 
  if (i != 0){
    disconnect_db2( *henv, *hdbc, 1); 
#ifdef INSMESSON 
    fclose(INSMESSFILE);
#endif /* INSMESSON */ 
#ifdef DEBUGON 
    fclose(LOGMESSFILE); 
#endif /* DEBUGON */ 
    return(MCAT_CONNECT_ERROR);
  }

  
    serverhndl[0].open = 1;
    strcpy(serverhndl[0].sname,cattype);
    tmp_srvh = (MDAS_DB2_SERVER_HNDL *) malloc(sizeof(MDAS_DB2_SERVER_HNDL));
    if (tmp_srvh == NULL) 
      {return(MEMORY_ALLOCATION_ERROR);}
    strcpy(tmp_srvh->user, user);
    strcpy(tmp_srvh->server,catser);
    tmp_srvh->valid = 1;
    tmp_srvh->henv  = *henv;
    tmp_srvh->hdbc  = *hdbc;
    serverhndl[0].conn = tmp_srvh;


  return(MDAS_SUCCESS);
}

int commit_db2_interaction_internal(SQLHENV henv, 
		  SQLHDBC hdbc, int  transaction_end_code)
{

  /*i = SQLTransact(henv, hdbc, transaction_end_code); Deprecated*/
  i = SQLEndTran(SQL_HANDLE_DBC, hdbc, transaction_end_code);

  return(i);
}

filterQuery(char *query)
{
  char *tmpPtr, *tmpPtr1, *tmpPtr2;

  if ((tmpPtr = strstr(query,"/*+")) != NULL &&
      (tmpPtr2 = strstr(query,"*/")) != NULL &&
      tmpPtr < tmpPtr2) {
    *tmpPtr  = '\0';
    strcat(query," ");
    strcat(query, (char *) (tmpPtr2 + 2));
  }
}
