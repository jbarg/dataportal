/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/****************************************************************************
** Source File Name = ifmxDvrLib.c
**
** Copyright (c) 1996
**   General Atomics (GA) and San Diego Supercomputer Center (SDSC)
**   San Diego, California, USA
**
** PURPOSE: 
**     Contains driver-level library functions to 
**     connect and operate with DB2 databases. 
**
** CALLABLE FUNCTIONS:
**     open_env_ifmxTab
**     close_env_ifmxTab
**     connect_ifmxTab
**     disconnect_ifmxTab
**     exec_no_res_sql_ifmxTab
**     exec_sql_ifmxTab
**     done_with_result_ifmxTab
**     done_with_default_result_ifmxTab
**     bind_default_row_ifmxTab
**     bind_n_rows_ifmxTab
**     bind_column_ifmxTab
**     bind_file_to_cell_ifmxTab
**     bind_files_to_column_ifmxTab
**     get_row_ifmxTab
**     get_rows_ifmxTab
**     get_no_of_columns_ifmxTab
**     get_column_info_ifmxTab
**
** INTERNAL FUNCTIONS USED:
**     logmess_ifmxTab
**     print_error_ifmxTab
**     check_error_ifmxTab
**
** EXTERNAL FUNCTIONS USED: FROM "sqlcli1.h"
**     SQLAllocConnect
**     SQLAllocEnv
**     SQLAllocStmt
**     SQLBindCol
**     SQLBindFileToCol
**     SQLColAttributes
**     SQLConnect
**     SQLDescribeCol
**     SQLDisconnect
**     SQLError
**     SQLExecDirect
**     SQLExtendedFetch
**     SQLFetch
**     SQLFreeConnect
**     SQLFreeEnv
**     SQLFreeStmt
**     SQLNumResultCols
**     SQLSetConnectOption
**     SQLSetStmtOption
**     SQLTransact
****************************************************************************/

/****************************************************************************
                             INCLUDES & EXTERNS
****************************************************************************/


/* Informix Includes */
#include <stdlib.h>
#include <infxsql.h>
#include <infxcli.h>
#include <sqlucode.h>
/* end Informix Includes */
#include "mdasC_db2_externs.h"
#include "mdasGlobalsExtern.h"
#include "srbErrno.h"
#include "elogExtern.h"
/*#include "db2DvrLib.h"*/
#include "ifmxDvrLib.h"


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
  NAME   : getDataTypeFromMap_ifmxTab
  PURPOSE: Getting Column DataType Name from Data Type Index given by Informix
****************************************************************************/ 
void
getDataTypeFromMap_ifmxTab(char *typeName, short int   typeId)
{
  int jj;
        for (jj = 0; jj < MAXDATATYPELISTSIZE ; jj++) {
          if (ifmxDataTypeCode[jj] == typeId) {
          strcpy(typeName, ifmxDataTypeName[jj]);
          break;
        }
      }
      if (jj == MAXDATATYPELISTSIZE) 
        strcpy(typeName, ifmxDataTypeName[0]);
}
/***************************************************************************
  NAME   : ifmxTableErrorReport
  PURPOSE: Logging error message to elog
****************************************************************************/

void  ifmxTableErrorReport( SQLHSTMT hstmt,SQLHENV henv)
{
    elog(NOTICE, "-- IFMX TABLE SID error--\n");
}

/***************************************************************************
  NAME   : logmess_ifmxTab
  PURPOSE: Logging message in LOGMESS file handle
****************************************************************************/

void logmess_ifmxTab(char *message,
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
  NAME   : print_error_ifmxTab
  PURPOSE: Prints error messages to ERRMESS file handle.
****************************************************************************/

void print_error_ifmxTab(SQLHENV henv,
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

    while (retcode != SQL_NO_DATA) {
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
  NAME   : check_error_ifmxTab
  PURPOSE: Checks severity of error and prints  error messages by
           calling to print_error_ifmxTab(). Tries to roll back transaction
           when there is a fatal or unknown error.
****************************************************************************/
  
void
check_error_ifmxTab(SQLHENV henv,
            SQLHDBC hdbc,
            SQLHSTMT hstmt,
            SQLRETURN frc,
            int line,
            char *file)
{
    SQLRETURN       rc;

    print_error_ifmxTab(henv, hdbc, hstmt, frc, line, file);

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
	logmess_ifmxTab(logmess, henv,  hdbc, SQL_NULL_HSTMT, frc);
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
	logmess_ifmxTab(logmess, henv,  hdbc, SQL_NULL_HSTMT, frc);
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
  NAME   : open_env_ifmxTab
  PURPOSE: Opening an execution environment in ifmx 
****************************************************************************/ 

int open_env_ifmxTab(SQLHENV *henv)
{
   SQLRETURN rc;


   /*rc = SQLAllocEnv(henv); Deprecated*/
   rc = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE,  henv);
#ifdef DEBUGON
   logmess_ifmxTab("SQLAllocHandle: SQL_HANDLE_ENV:", *henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab( *henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }

   /* Set ODBC to Version 3 */
   rc = SQLSetEnvAttr (*henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, 0);
   logmess_ifmxTab("SQLSetEnvAttr: ", *henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab( *henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }

   return(0);
	
}

/***************************************************************************
  NAME   : close_env_ifmxTab
  PURPOSE: Closing an open execution environment in Informix
****************************************************************************/
   
int close_env_ifmxTab(SQLHENV henv)
{
   SQLRETURN rc;

   /* rc = SQLFreeEnv(henv); deprecated */
   rc = SQLFreeHandle(SQL_HANDLE_ENV, henv);
#ifdef DEBUGON
   logmess_ifmxTab("SQLFreeEnv", henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab(henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   return(0);
}

/***************************************************************************
  NAME   : connect_ifmxTab
  PURPOSE: Opening a connection to Informix server
****************************************************************************/

int connect_ifmxTab( SQLHENV henv,
		  SQLHDBC *hdbc,
		  SQLCHAR *databasename, 
		 SQLCHAR *user, 
		 SQLCHAR *password)
{
   SQLRETURN rc;

   rc = SQLAllocHandle(SQL_HANDLE_DBC, henv, hdbc);
#ifdef DEBUGON
   logmess_ifmxTab("SQLAllocConnect", henv, *hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab(henv, *hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   /*rc = SQLSetConnectOption(*hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);*/
   rc = SQLSetConnectAttr(*hdbc, SQL_ATTR_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF, 0);
#ifdef DEBUGON
   logmess_ifmxTab("SQLSetConnectOption:SQL_AUTOCOMMIT_OFF", henv, *hdbc, 
	       SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */


   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab(henv, *hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-2);
     }
   rc = SQLConnect(*hdbc, databasename, SQL_NTS, user, SQL_NTS, password, 
		   SQL_NTS);

#ifdef DEBUGON
   sprintf(logmess, "SQLConnect: Database = %s, User = %s", databasename,user);   logmess_ifmxTab(logmess, henv, *hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */


   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab(henv, *hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       
       rc = SQLDisconnect(*hdbc);
#ifdef DEBUGON
       logmess_ifmxTab("SQLDisconnect", henv, *hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */
       /*rc = SQLFreeConnect(*hdbc);*/
       rc = SQLFreeHandle(SQL_HANDLE_DBC, *hdbc);
#ifdef DEBUGON
       logmess_ifmxTab("SQLFreeConnect", henv, *hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */
       return(-3);
     }
   return(0);
}

/***************************************************************************
  NAME   : disconnect_ifmxTab
  PURPOSE: Closing an open connection  in Informix 

****************************************************************************/
 
int disconnect_ifmxTab(SQLHENV henv,
	    SQLHDBC hdbc, SQLUSMALLINT transstate)
{
  SQLRETURN rc;
   

   /*rc = SQLTransact(henv, hdbc, transstate);*/
   rc = SQLEndTran(SQL_HANDLE_DBC, hdbc, transstate);
#ifdef DEBUGON
   sprintf(logmess,"SQLTransact:%d",transstate);
   logmess_ifmxTab(logmess, henv,  hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     } 
   rc = SQLDisconnect(hdbc);
#ifdef DEBUGON
   logmess_ifmxTab("SQLDisconnect", henv,  SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-2);
     }
   /*rc = SQLFreeConnect(hdbc);*/
   rc = SQLFreeHandle(SQL_HANDLE_DBC,hdbc);
#ifdef DEBUGON
   logmess_ifmxTab("SQLFreeHandle", henv,  SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-3);
     }
   return (0);
}

/***************************************************************************
  NAME   : exec_no_res_sql_ifmxTab
  PURPOSE: Execute a SQL query which has no resulting table, eg. INSERT,
           DELETE, UPDATE or DDL
****************************************************************************/
 
int exec_no_res_sql_ifmxTab(SQLHENV henv, 
		  SQLHDBC hdbc, 
		  SQLCHAR *query)
{
     SQLHSTMT        hstmt;
     SQLRETURN       rc;
     SQLRETURN       rc1;
     SQLINTEGER         result;

     /*rc = SQLAllocStmt(hdbc, &hstmt); */
     rc = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
#ifdef DEBUGON
     logmess_ifmxTab("SQLAllocStmt", henv,  hdbc, hstmt, rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_ifmxTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	 return(-1);
       }
     rc = SQLExecDirect(hstmt, query, SQL_NTS);
#ifdef DEBUGON
     logmess_ifmxTab("SQLExecDirect", henv,  hdbc, hstmt, rc);
#endif /* DEBUGON */

     switch (rc) {
     case SQL_SUCCESS:
     case SQL_SUCCESS_WITH_INFO:
     case SQL_NO_DATA_FOUND:
       result = 0;
       break;
     case SQL_INVALID_HANDLE:
#ifdef DEBUGON
       check_error_ifmxTab(henv, hdbc, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       result = -2;
       break;
     default:
#ifdef DEBUGON
       check_error_ifmxTab(henv, hdbc, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       result = -3;
       break;
     }

     /*rc = SQLFreeStmt(hstmt, SQL_DROP);*/
     rc = SQLFreeHandle(SQL_HANDLE_STMT,hstmt);
#ifdef DEBUGON
     logmess_ifmxTab("SQLFreeStmt", henv,  hdbc,  SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab(henv, hdbc,  hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-4);
     }
     return(result);
}

/***************************************************************************
  NAME   : exec_sql_ifmxTab
  PURPOSE: Execute a SQL query which mave have  resulting table
****************************************************************************/

int exec_sql_ifmxTab(SQLHENV henv, 
		  SQLHDBC hdbc, 
		  SQLHSTMT *hstmt,
		  SQLCHAR *query)
{
     SQLRETURN       rc, rc1;
     SQLINTEGER      result;
     /*rc = SQLAllocStmt(hdbc, hstmt); */
     rc = SQLAllocHandle(SQL_HANDLE_STMT,(SQLHDBC)hdbc,(SQLHSTMT) hstmt);
#ifdef DEBUGON
     logmess_ifmxTab("SQLAllocStmt", henv,  hdbc, *hstmt, rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_ifmxTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	 return(-1);
       }
     rc = SQLExecDirect(*hstmt, query, SQL_NTS);
#ifdef DEBUGON
     logmess_ifmxTab("SQLExecDirect", henv,  hdbc, *hstmt, rc);
#endif /* DEBUGON */

     switch (rc) {
     case SQL_SUCCESS:
     case SQL_SUCCESS_WITH_INFO:
     case SQL_NO_DATA_FOUND:
       result = 0;
       break;
     case SQL_INVALID_HANDLE:
#ifdef DEBUGON
       check_error_ifmxTab(henv, hdbc, *hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

       result = -2;
       break;
     default:
#ifdef DEBUGON
       check_error_ifmxTab(henv, hdbc, *hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

       result = -3;
       break;
     }
     return(result);
}

/***************************************************************************
  NAME   : done_with_result_ifmxTab
  PURPOSE: Close a stament environment opened by exec_sql_ifmxTab() but which
           used user allocated result structure
****************************************************************************/

int done_with_result_ifmxTab(SQLHSTMT hstmt)
{  
   SQLRETURN rc;

   /*rc = SQLFreeStmt(hstmt, SQL_DROP);*/
   rc = SQLFreeHandle(SQL_HANDLE_STMT,(SQLHSTMT)hstmt);
#ifdef DEBUGON
   logmess_ifmxTab("SQLFreeStmt", SQL_NULL_HENV, SQL_NULL_HDBC,SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   return(0);
}

/***************************************************************************
  NAME   : done_with_default_result_ifmxTab
  PURPOSE: Close a stament environment opened by exec_sql_ifmxTab() which used
           result structure build by bind_default_row_ifmxTab()
****************************************************************************/
 
int done_with_default_result_ifmxTab(SQLHSTMT hstmt,
				  SQLCHAR *data[],
				  SQLINTEGER *data_size,
				  SQLSMALLINT num_of_cols)
{  
   SQLRETURN rc;
   int i;

   for (i =  0; i < num_of_cols; i++){
     free(data[i]);
   }
   /*rc = SQLFreeStmt(hstmt, SQL_DROP);*/
   rc = SQLFreeHandle(SQL_HANDLE_STMT,(SQLHSTMT) hstmt);
#ifdef DEBUGON
   logmess_ifmxTab("SQLFreeStmt",SQL_NULL_HENV, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   return(0);
}

/***************************************************************************
  NAME   : bind_default_row_ifmxTab
  PURPOSE: Allocate a default set of result holders to hold one row of 
           result of a SQL statement. 
****************************************************************************/
 
int bind_default_row_ifmxTab(SQLHSTMT hstmt,
		  SQLCHAR *data[],
		  SQLINTEGER data_size[], 
		  SQLSMALLINT *num_of_cols)
{
   SQLRETURN       rc;
   int             i;
   SQLINTEGER      displaysize;
    SQLCHAR         colname[MAX_TOKEN];
    SQLSMALLINT     coltype;
    SQLSMALLINT     colnamelen;
    SQLSMALLINT     nullable;
    SQLINTEGER      collen[MAX_TOKEN];
    SQLUINTEGER     precision;
    SQLSMALLINT     scale;

    SQLCHAR         errmsg[ERRMSG_LEN];

   rc = SQLNumResultCols(hstmt, num_of_cols);
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
 
    for (i = 0; i < *num_of_cols; i++) {
        rc = SQLDescribeCol(hstmt, i + 1, colname, sizeof(colname),
                       &colnamelen, &coltype, &precision, &scale, NULL);
	if (rc != SQL_SUCCESS)
	  {
#ifdef DEBUGON
	    check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	    return(-2);
	  }
        collen[i] = precision; 

        /*rc = SQLColAttributes(hstmt, i + 1, SQL_COLUMN_DISPLAY_SIZE, 
			      NULL, 0,    NULL, &displaysize);*/
	rc = SQLColAttribute((SQLHSTMT) hstmt, i + 1, SQL_COLUMN_DISPLAY_SIZE, 
			      NULL, 0,    NULL, &displaysize);
	if (rc != SQL_SUCCESS)
	  {
#ifdef DEBUGON
	    check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	    return(-3);
	  }
       /* since lvarchar size is SDWORD = SQLINTEGER.... dirty, dirty since we have
      to get around the small varchar size in informix, we'll set the max size of lvarchar
      to the max that is allowable in the oracle version :( 2001*/
       if ((displaysize +1) < 0 && displaysize > 0) {
                   collen[i] = 2001;
		   logmess_ifmxTab("lvarchar hack",NULL,NULL,NULL,0);
       }
       else if (displaysize > (strlen((char *) colname)))
	           collen[i] = displaysize + 1;
       else collen[i] = strlen((char *) colname) + 1;
       /*collen[i] = max(displaysize, strlen((char *) colname)) + 1;*/
       if (collen[i] == 0) 
		collen[i] = 1;
 
        data[i] = (SQLCHAR *) malloc((int)collen[i]);
	strcpy(data[i],"");
 
        rc = SQLBindCol(hstmt, i + 1, SQL_C_CHAR, data[i], collen[i], &data_size[i]);
	if (rc != SQL_SUCCESS)
	  {
#ifdef DEBUGON
	    check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	    return(-4);
	  }
    }
   return(0);
}

/***************************************************************************
  NAME   : get_row_ifmxTab
  PURPOSE: Fetch the next (unfetched)  row of result in the 
           row-holder bound by any of the bind_***_ifmxTab() calls. To fetch
           multiple row use get_rows_ifmxTab()
****************************************************************************/

int get_row_ifmxTab(SQLHSTMT hstmt)
{
   SQLRETURN       rc;

   rc =  SQLFetch(hstmt);
   if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
     {
       if (rc != SQL_NO_DATA)
	 {
#ifdef DEBUGON
       check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */
       return(MCAT_FETCH_ERROR);
	 }
       return(MCAT_NO_DATA_FOUND);
     }
   return(0);
}

/***************************************************************************
  NAME   : bind_column_ifmxTab
  PURPOSE: Allocate an array (or a variable) to hold a column (or cell)
           of results of a  get_rows_ifmxTab statement. 
****************************************************************************/

int bind_column_ifmxTab(SQLHSTMT hstmt,
		   SQLPOINTER data_column,
		   SQLPOINTER size_column,
		   SQLSMALLINT column_type,
		   SQLINTEGER max_data_size, /* in bytes */
		   SQLSMALLINT column_number,
		   SQLINTEGER row_size)
{
   SQLRETURN       rc;

   if (row_size > 1)
     {
       /*rc = SQLSetStmtOption(hstmt, SQL_ROWSET_SIZE, row_size);*/
       rc = SQLSetStmtAttr((SQLHSTMT) hstmt, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER) row_size, 0);

       if (rc != SQL_SUCCESS)
	 {
#ifdef DEBUGON
	   check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	   return(-1);
	 }
     }
   if (column_type == 0)
     {
       rc = SQLBindCol(hstmt, column_number, SQL_C_DEFAULT, data_column,
		   max_data_size, size_column);
     }
   else
     {
       rc = SQLBindCol(hstmt, column_number, column_type, data_column,
		   max_data_size, size_column);

     }
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-2);
     }
   return(0);
}

/***************************************************************************
  NAME   : bind_files_to_column_ifmxTab
  PURPOSE: Allocate an array of file name holders to hold a LOB column
           of results of a  get_rows_ifmxTab statement. 
	   There doesn't appear to be anyplace this is used...... --toaster
****************************************************************************/
 
int bind_files_to_column_ifmxTab(SQLHSTMT hstmt,
		   void        *file_names,
		   SQLSMALLINT *file_name_cell_size,
		   SQLUINTEGER  *file_options,
		   SQLINTEGER *file_status,
		   SQLINTEGER *result_size_column,
		   SQLUSMALLINT column_number,
		   SQLINTEGER row_size)
{
/*   SQLRETURN       rc;


   if (row_size > 1)
     {
       rc = SQLSetStmtOption(hstmt, SQL_ROWSET_SIZE, row_size);
          if (rc != SQL_SUCCESS)
	    {
#ifdef DEBUGON
	      check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif *//* DEBUGON *//*

	      return(-1);
	    }
     }
       rc = SQLBindFileToCol(hstmt, column_number, file_names,NULL, 
		   file_options, 255, result_size_column, file_status);
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       	 check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif *//* DEBUGON *//*

         return(-2);
     }*/
   return(0);
}

/***************************************************************************
  NAME   : bind_file_to_cell_ifmxTab
  PURPOSE: Allocate a file name holder to hold a LOB 
           in a row of a  get_row_ifmxTab statement. 
	   again, not used anywhere --toaster
****************************************************************************/

int bind_file_to_cell_ifmxTab(SQLHSTMT hstmt,
		   SQLCHAR  file_name[],
		   SQLSMALLINT *file_name_length,
		   SQLUINTEGER  *file_option,
		   SQLINTEGER *file_status,
		   SQLINTEGER *result_size_column,
		   SQLUSMALLINT column_number)
{
/*  SQLRETURN       rc;

   rc = SQLBindFileToCol(hstmt, column_number, 
			 bind_file_name, &bind_file_name_length,
			 &bind_file_option, 100,
			 result_size_column, &bind_file_status);
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       	 check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif *//* DEBUGON *//*

         return(-1);
     }*/
  return(0);
}

/***************************************************************************
  NAME   : get_rows_ifmxTab
  PURPOSE: Fetch the next set of  rows of result in the 
           column-arrays bound by any of the bind_***_ifmxTab() calls. To fetch
           a single row use get_row_ifmxTab()
****************************************************************************/

int get_rows_ifmxTab(SQLHSTMT hstmt, SQLUINTEGER *pcrow)
{
  SQLRETURN       rc;

  rc =  SQLExtendedFetch(hstmt, SQL_FETCH_NEXT,0, pcrow, NULL);
   if (rc != SQL_SUCCESS)
     {
         if (rc != SQL_NO_DATA)
#ifdef DEBUGON
       	 check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-1);
     }
  return(0);
}

/***************************************************************************
  NAME   : bind_n_rows_ifmxTab
  PURPOSE: Allocate arrays  to hold all column of the result of a 
           get_rows_ifmxTab statement. 
****************************************************************************/

int bind_n_rows_ifmxTab(SQLHSTMT hstmt,
		  SQLPOINTER  data_column[],
		  SQLPOINTER  size_column[],
		  SQLINTEGER  max_data_size[],  /* in bytes */
		  SQLSMALLINT column_type[],
		  SQLSMALLINT num_of_columns,
		  SQLINTEGER  row_size)
{
  SQLRETURN       rc;
  int             i;
  SQLINTEGER      displaysize;

 if (row_size > 1)
   {
     /*rc = SQLSetStmtOption(hstmt, SQL_ROWSET_SIZE, row_size);*/
     rc = SQLSetStmtAttr( hstmt, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER) row_size, 0);

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
       	 check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-1);
       }
   }
  for (i = 0; i < num_of_columns; i++){
    if (column_type[i] == 0)
      {
        rc = SQLBindCol(hstmt, i + 1, SQL_C_DEFAULT, data_column[i], 
		    max_data_size[i], size_column[i]);
      }
    else
      {
        rc = SQLBindCol(hstmt, i + 1, column_type[i], data_column[i], 
		    max_data_size[i], size_column[i]);
      }
     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
       	 check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-2);
       }
  }
  return(0);
}

/***************************************************************************
  NAME   : get_no_of_columns_ifmxTab
  PURPOSE: Find the number of columns of the result table of an
           exec_sql_ifmxTab statement. 
****************************************************************************/
  
int get_no_of_columns_ifmxTab(SQLHSTMT hstmt,
			SQLSMALLINT *num_of_cols)
{
  SQLRETURN       rc;

  rc = SQLNumResultCols(hstmt, num_of_cols);
  if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
    	 check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-1);
     }
  return(0);
}

/***************************************************************************
  NAME   : get_column_info_ifmxTab
  PURPOSE: Get assorted column attributes  such as column name, size (as 
           defined in DDL), type, nullability, precision and its scale.
****************************************************************************/
 
int get_column_info_ifmxTab(SQLHSTMT hstmt,
			SQLSMALLINT  column_number,
			SQLINTEGER *displaysize,
			SQLCHAR      colname[],
			SQLSMALLINT *coltype,
                        SQLSMALLINT buf_size,
                        SQLSMALLINT *colnamelen,
                        SQLSMALLINT *nullable,
			SQLUINTEGER *precision,
                        SQLSMALLINT *scale
			)
{
  SQLRETURN       rc;

  /*rc = SQLColAttributes(hstmt, column_number, SQL_COLUMN_DISPLAY_SIZE, 
			NULL, 0, NULL, displaysize);*/
  rc = SQLColAttribute(hstmt, column_number, SQL_COLUMN_DISPLAY_SIZE, 
			NULL, 0, NULL, displaysize);
  if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
    	 check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-1);
     }
  rc = SQLDescribeCol(hstmt, column_number, colname, buf_size,
                       colnamelen, coltype, precision, scale, nullable);
  if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
    	 check_error_ifmxTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-2);
     }
  return(0);
}




int
getColTypeFromColNameifmxTab(long int  henv, long int  hdbc,
		      char *colType, char *colName, char *inTableName)
{
  char *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4;
  char sqlQuery[HUGE_STRING];
  int i,j;
  SQLHSTMT hstmt;
  SQLINTEGER   data_size[4];
  unsigned char       *data[4];
  short int coltypenum;


  tmpPtr2 = inTableName;
  while (isspace(*tmpPtr2)) tmpPtr2++;
  tmpPtr3 = inTableName + strlen(inTableName) -1;
  while (isspace(*tmpPtr3)) tmpPtr3--;
  *(tmpPtr3 +1) = '\0';
  tmpPtr4 = colName ;
  while (isspace(*tmpPtr4)) tmpPtr4++;
  tmpPtr3 = colName + strlen(colName) -1;
  while (isspace(*tmpPtr3)) tmpPtr3--;
  *(tmpPtr3 +1) = '\0';

  if ((tmpPtr =  strstr(tmpPtr2,".")) != NULL) {
    *tmpPtr = '\0';
    sprintf(sqlQuery,"select coltype from systables,syscolumns where COLNAME = '%s' and TABNAME = '%s' and OWNER = '%s' and A.tabid = B.tabid",    
	    tmpPtr4, tmpPtr+1,tmpPtr2 );
    *tmpPtr = '.';
  }
  else {
    sprintf(sqlQuery,"select coltype from systables A,syscolumns B where COLNAME = '%s' and TABNAME = '%s' and A.tabid = B.tabid",    
	    tmpPtr4, tmpPtr2 );
  }
  upper(sqlQuery);
  i = exec_sql_ifmxTab( (SQLHENV) henv, (SQLHDBC) hdbc, &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_ifmxTab( hstmt, &num_of_cols);
  if (i != 0) return(i);
  i = bind_default_row_ifmxTab( hstmt, data, data_size, &num_of_cols);
  if (i != 0) return(i);
  i = get_row_ifmxTab(hstmt);
  if (i == 0)       {
    truncate_end_blanks(data_size[0], data[0]);
    truncate_end_blanks(data_size[1], data[1]);
    coltypenum = atoi(data[0]);
    if (coltypenum >255) 
	coltypenum -= 256;
    getDataTypeFromMap_ifmxTab(colType, coltypenum);
	/*strcpy(colType, (char *) data[0]);*/
  }
  else
    {
      i = done_with_default_result_ifmxTab(hstmt,data,data_size, num_of_cols);
      return(i);
    }
  
  i = done_with_default_result_ifmxTab(hstmt,data,data_size, num_of_cols);

  return(MDAS_SUCCESS);
}

int
getColumnInfoForifmxTable(long int  henv, long int  hdbc,
		      char *inTableName,
		      int *numOfColumns,
		      char *colName[],
		      char colType[MAX_TABLE_COLS][SMALL_TOKEN],
		      long int displaysize[],
		      short int nullable[],
		      unsigned long int precision[],
		      short int   scale[])
{
  char *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4;
  char sqlQuery[HUGE_STRING];
  short int tmpcoltype,tmpint1, tmpint2;
  int i,j, jj;
  SQLHSTMT   hstmt;
  SQLINTEGER   data_size[10];
  unsigned char       *data[10];

  sprintf(sqlQuery,"select first 1 * from %s",inTableName);
  upper(sqlQuery);

  i = exec_sql_ifmxTab( (SQLHENV) henv, (SQLHENV) hdbc,  &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_ifmxTab(hstmt, &num_of_cols);
  if (i != 0) return(i);

  *numOfColumns = num_of_cols;
  for (i=1; i < num_of_cols; i++) {
	  
    j = get_column_info_ifmxTab(hstmt, i+1, (SQLINTEGER) &displaysize[i], colName[i], &tmpcoltype, SMALL_TOKEN , &tmpint2, &nullable[i], (SQLUINTEGER) &precision[i], &scale[i]);

    getDataTypeFromMap_ifmxTab(colType[i],tmpcoltype);
  }
/*

  tmpPtr2 = inTableName;
  while (isspace(*tmpPtr2)) tmpPtr2++;
  tmpPtr3 = inTableName + strlen(inTableName) -1;
  while (isspace(*tmpPtr3)) tmpPtr3--;
  *(tmpPtr3 +1) = '\0';

  if ((tmpPtr =  strstr(tmpPtr2,".")) != NULL) {
    *tmpPtr = '\0';
    sprintf(sqlQuery,"select distinct COLNAME, COLTYPE, COLLENGTH, TYPESCHEMA, \
 SCALE, NULLS, COLNO  from SYSCAT.COLUMNS where TABNAME = '%s' \
 and TABSCHEMA = '%s' order by COLNO ASC",
	    tmpPtr+1,tmpPtr2 );
    *tmpPtr = '.';
  }
  else {
    sprintf(sqlQuery,"select distinct COLNAME, TYPENAME, LENGTH, TYPESCHEMA, \
 SCALE, NULLS, COLNO  from SYSCAT.COLUMNS where TABNAME = '%s' \
 order by COLNO ASC", 
	     tmpPtr2 );
  }
  upper(sqlQuery);
  i = exec_sql_ifmxTab(henv, hdbc,  &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_ifmxTab(hstmt, &num_of_cols);
  if (i != 0) return(i);
  i = bind_default_row_ifmxTab(hstmt, data,  data_size, &num_of_cols);
  if (i != 0) return(i);
  i = get_row_ifmxTab(hstmt);
  if (i == 0)       {
    jj = 0;
    while (i == 0) {
      for (i = 0 ; i < num_of_cols; i++) {
	truncate_end_blanks(data_size[i], data[i]);
      }
      colName[jj] = strdup(data[0]);
      if (!strcmp(data[1],"SYSIBM"))
	strcpy(colType[jj], (char *) data[1]);
      else 
	sprintf(colType[jj], "%s.%s", data[3], data[1]);
      displaysize[jj] = atoi(data[2]);
      *//** precision[jj] = atoi(data[3]); **/
/*      precision[jj] = 0; 
      scale[jj] = atoi(data[4]);
      if (!strcmp(data[5], "Y"))
	nullable[jj] = 1;
      else
	nullable[jj] = 0;
      jj++;
      i = get_row_ifmxTab(hstmt);
    }
    *numOfColumns = jj;
  }
  else
    {
      i = done_with_default_result_ifmxTab(hstmt,data,data_size, num_of_cols);
      return(i);
    }
  
  i = done_with_default_result_ifmxTab(hstmt,data,data_size, num_of_cols);
*/
  return(MDAS_SUCCESS);
}
  

