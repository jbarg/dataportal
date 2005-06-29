/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
/****************************************************************************
** Source File Name = psgDvrLib.c
**
** PURPOSE: 
**     Contains driver-level library functions to 
**     connect and operate with Postgres databases. 
**
** CALLABLE FUNCTIONS:
**     open_env_psgTab
**     close_env_psgTab
**     connect_psgTab
**     disconnect_psgTab
**     exec_no_res_sql_psgTab
**     exec_sql_psgTab
**     done_with_result_psgTab
**     done_with_default_result_psgTab
**     bind_default_row_psgTab
**     bind_n_rows_psgTab
**     bind_column_psgTab
**     bind_file_to_cell_psgTab
**     bind_files_to_column_psgTab
**     get_row_psgTab
**     get_rows_psgTab
**     get_no_of_columns_psgTab
**     get_column_info_psgTab
**
** INTERNAL FUNCTIONS USED:
**     logmess_psgTab
**     print_error_psgTab
**     check_error_psgTab
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


#include "psqtypes.h"
#include "mdasC_db2_externs.h"
#include "mdasGlobalsExtern.h"
#include "srbErrno.h" 
#include "elogExtern.h"
#include "psgDvrLib.h"


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
  NAME   : logmess_psgTab
  PURPOSE: Logging message in LOGMESS file handle
****************************************************************************/

void logmess_psgTab(char *message,
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
  NAME   : print_error_psgTab
  PURPOSE: Prints error messages to ERRMESS file handle.
****************************************************************************/

void print_error_psgTab(SQLHENV henv,
            SQLHDBC hdbc,
            SQLHSTMT hstmt,
            SQLRETURN frc,  
            int line,       
            char * file)  
{
    SQLCHAR         buffer[SQL_MAX_MESSAGE_LENGTH + 1];
    SQLCHAR         sqlstate[SQL_SQLSTATE_SIZE + 1];
    SQLINTEGER      sqlcode;
    SQLSMALLINT     length;

#ifdef DEBUGON 

    fprintf(ERRMESSFILE,"ERROR: RC= %d Reported from %s, line %d \n",
           frc, file, line);
    while (SQLError(henv, hdbc, hstmt, sqlstate, &sqlcode, buffer,
                    SQL_MAX_MESSAGE_LENGTH + 1, &length) == SQL_SUCCESS) {
        fprintf(ERRMESSFILE,"         SQLSTATE: %s\n", sqlstate);
        fprintf(ERRMESSFILE,"Native Error Code: %ld\n", sqlcode);
        fprintf(ERRMESSFILE,"%s \n", buffer);
    }
#endif /* DEBUGON */ 


}

/***************************************************************************
  NAME   : check_error_psgTab
  PURPOSE: Checks severity of error and prints  error messages by
           calling to print_error_psgTab(). Tries to roll back transaction
           when there is a fatal or unknown error.
****************************************************************************/
  
void
check_error_psgTab(SQLHENV henv,
            SQLHDBC hdbc,
            SQLHSTMT hstmt,
            SQLRETURN frc,
            int line,
            char *file)
{
    SQLRETURN       rc;

    print_error_psgTab(henv, hdbc, hstmt, frc, line, file);

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
	logmess_psgTab(logmess, henv,  hdbc, SQL_NULL_HSTMT, frc);
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
	logmess_psgTab(logmess, henv,  hdbc, SQL_NULL_HSTMT, frc);
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
  NAME   : open_env_psgTab
  PURPOSE: Opening an execution environment in psgTab 
****************************************************************************/ 

int open_env_psgTab(long int *inhenv)
{
   SQLHENV henv;
   SQLRETURN rc;


   rc = SQLAllocEnv(&henv);

#ifdef DEBUGON
   logmess_psgTab("SQLAllocEnv", henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_psgTab(henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   *inhenv = (long int)henv;
   return(0);
}

/***************************************************************************
  NAME   : close_env_psgTab
  PURPOSE: Closing an open execution environment in psgTab 
****************************************************************************/
   
int close_env_psgTab(long int inhenv)
{
   SQLHENV henv=(SQLHENV)inhenv;
   SQLRETURN rc;

   rc = SQLFreeEnv(henv);
#ifdef DEBUGON
   logmess_psgTab("SQLFreeEnv", henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_psgTab(henv, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   return(0);
}

/***************************************************************************
  NAME   : connect_psgTab
  PURPOSE: Opening a connection to psgTab server
****************************************************************************/

int connect_psgTab( long int inhenv,
		  long int *inhdbc,
		  SQLCHAR *databasename, 
		 SQLCHAR *user, 
		 SQLCHAR *password)
{
   SQLHENV henv=(SQLHENV) inhenv;
   SQLHDBC hdbc;
   SQLRETURN rc;

   rc = SQLAllocConnect(henv, &hdbc);  
   *inhdbc=(long int)hdbc;
#ifdef DEBUGON
   logmess_psgTab("SQLAllocConnect", henv, hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_psgTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   rc = SQLSetConnectOption(hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
#ifdef DEBUGON
   logmess_psgTab("SQLSetConnectOption:SQL_AUTOCOMMIT_OFF", henv, hdbc, 
	       SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */


   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_psgTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-2);
     }
   rc = SQLConnect(hdbc, databasename, SQL_NTS, user, SQL_NTS, password, 
		   SQL_NTS);

#ifdef DEBUGON
   sprintf(logmess, "SQLConnect: Database = %s, User = %s", databasename,user);   logmess_psgTab(logmess, henv, hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */


   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_psgTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       
       rc = SQLDisconnect(hdbc);
#ifdef DEBUGON
       logmess_psgTab("SQLDisconnect", henv, hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */
       rc = SQLFreeConnect(hdbc);
#ifdef DEBUGON
       logmess_psgTab("SQLFreeConnect", henv, hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */
       return(-3);
     }
   return(0);
}

/***************************************************************************
  NAME   : disconnect_psgTab
  PURPOSE: Closing an open connection  in psgTab 

****************************************************************************/
 
int disconnect_psgTab(long int inhenv,
	    long int inhdbc, SQLUSMALLINT transstate)
{
  SQLHENV henv=(SQLHENV)inhenv;
  SQLHDBC hdbc=(SQLHDBC)inhdbc;
  SQLRETURN rc;
   

   rc = SQLTransact(henv, hdbc, transstate);
#ifdef DEBUGON
   sprintf(logmess,"SQLTransact:%d",transstate);
   logmess_psgTab(logmess, henv,  hdbc, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_psgTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     } 
   rc = SQLDisconnect(hdbc);
#ifdef DEBUGON
   logmess_psgTab("SQLDisconnect", henv,  SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_psgTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-2);
     }
   rc = SQLFreeConnect(hdbc);
#ifdef DEBUGON
   logmess_psgTab("SQLFreeConnect", henv,  SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_psgTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-3);
     }
   return (0);
}

/***************************************************************************
  NAME   : exec_no_res_sql_psgTab
  PURPOSE: Execute a SQL query which has no resulting table, eg. INSERT,
           DELETE, UPDATE or DDL
****************************************************************************/
 
int exec_no_res_sql_psgTab(long int inhenv, 
		  long int inhdbc, 
		  SQLCHAR *query)
{
     SQLHENV	     henv=(SQLHENV)inhenv;
     SQLHDBC	     hdbc=(SQLHDBC)inhdbc;
     SQLHSTMT           hstmt;
     SQLRETURN       rc;
     SQLRETURN       rc1;
     SQLINTEGER         result;
    time_t              tim, time();

     filterQuerypsg(query);
     if (showQuery) {
      time(&tim);
      printf("SQL:%d:%s: %s\n",getpid(),query,ctime(&tim)+4);
      fflush(stdout);
      strcpy(GENERATEDSQL,query);
     }


     rc = SQLAllocStmt(hdbc, &hstmt); 
#ifdef DEBUGON
     logmess_psgTab("SQLAllocStmt", henv,  hdbc, hstmt, rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_psgTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	 return(-1);
       }
     rc = SQLExecDirect(hstmt, query, SQL_NTS);
#ifdef DEBUGON
     logmess_psgTab("SQLExecDirect", henv,  hdbc, hstmt, rc);
#endif /* DEBUGON */

     switch (rc) {
     case SQL_SUCCESS:
     case SQL_SUCCESS_WITH_INFO:
     case SQL_NO_DATA_FOUND:
       result = 0;
       break;
     case SQL_INVALID_HANDLE:
#ifdef DEBUGON
       sprintf(logmess,"SQLExecDirect: SQL_INVALID_HANDLE Error when executing query: %s",query);
       logmess_psgTab(logmess,henv,  hdbc, hstmt, rc);
       check_error_psgTab(henv, hdbc, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       result = -2;
       break;
     default:
#ifdef DEBUGON
       sprintf(logmess,"SQLExecDirect: Unknown result Error when executing query: %s",query);
       logmess_psgTab(logmess,henv,  hdbc, hstmt, rc);
       check_error_psgTab(henv, hdbc, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       result = -3;
       break;
     }

     rc = SQLFreeStmt(hstmt, SQL_DROP);
#ifdef DEBUGON
     logmess_psgTab("SQLFreeStmt", henv,  hdbc,  SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_psgTab(henv, hdbc,  hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-4);
     }
     return(result);
}

/***************************************************************************
  NAME   : exec_sql_psgTab
  PURPOSE: Execute a SQL query which mave have  resulting table
****************************************************************************/

int exec_sql_psgTab(long int inhenv, 
		  long int inhdbc, 
		  long int *inhstmt,
		  SQLCHAR *query)
{
     SQLHENV 	     henv=(SQLHENV)inhenv;
     SQLHDBC	     hdbc=(SQLHDBC)inhdbc;
     SQLHSTMT	     hstmt;
     SQLRETURN       rc, rc1;
     SQLINTEGER      result;
    time_t              tim, time();

     filterQuerypsg(query);
     if (showQuery) {
      time(&tim);
      printf("SQL:%d:%s: %s\n",getpid(),query,ctime(&tim)+4);
      fflush(stdout);
      strcpy(GENERATEDSQL,query);
     }


     rc = SQLAllocStmt(hdbc, &hstmt); 
     *inhstmt=(long int)hstmt;
#ifdef DEBUGON
     logmess_psgTab("SQLAllocStmt", henv,  hdbc, hstmt, rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_psgTab(henv, hdbc, SQL_NULL_HSTMT, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	 return(-1);
       }
     rc = SQLExecDirect(hstmt, query, SQL_NTS);
#ifdef DEBUGON
     logmess_psgTab("SQLExecDirect", henv,  hdbc, hstmt, rc);
#endif /* DEBUGON */

     switch (rc) {
     case SQL_SUCCESS:
     case SQL_SUCCESS_WITH_INFO:
     case SQL_NO_DATA_FOUND:
       result = 0;
       break;
     case SQL_INVALID_HANDLE:
#ifdef DEBUGON
       sprintf(logmess,"SQLExecDirect: SQL_INVALID_HANDLE Error when executing query: %s",query);
       logmess_psgTab(logmess,henv,  hdbc, hstmt, rc);
       check_error_psgTab(henv, hdbc, hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

       result = -2;
       break;
     default:
#ifdef DEBUGON
       sprintf(logmess,"SQLExecDirect: Unknown Return: Error when executing query: %s",query);
       logmess_psgTab(logmess,henv,  hdbc, hstmt, rc);
       check_error_psgTab(henv, hdbc, hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

       result = -3;
       break;
     }
     return(result);
}

/***************************************************************************
  NAME   : done_with_result_db
  PURPOSE: Close a stament environment opened by exec_sql_psgTab() but which
           used user allocated result structure
****************************************************************************/

int done_with_result_psgTab(long int inhstmt)
{  
   SQLHSTMT     hstmt=(SQLHSTMT)inhstmt;
   SQLRETURN rc;

   rc = SQLFreeStmt(hstmt, SQL_DROP);
#ifdef DEBUGON
   logmess_psgTab("SQLFreeStmt", SQL_NULL_HENV, SQL_NULL_HDBC,SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   return(0);
}

/***************************************************************************
  NAME   : done_with_default_result_psgTab
  PURPOSE: Close a stament environment opened by exec_sql_psgTab() which used
           result structure build by bind_default_row_psgTab()
****************************************************************************/
 
int done_with_default_result_psgTab(long int inhstmt,
				  SQLCHAR *data[],
				  SQLINTEGER *data_size,
				  SQLSMALLINT num_of_cols)
{  
   SQLHSTMT	hstmt=(SQLHSTMT)inhstmt;
   SQLRETURN rc;
   int i;

   for (i =  0; i < num_of_cols; i++){
     free(data[i]);
   }
   rc = SQLFreeStmt(hstmt, SQL_DROP);
#ifdef DEBUGON
   logmess_psgTab("SQLFreeStmt",SQL_NULL_HENV, SQL_NULL_HDBC, SQL_NULL_HSTMT, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-1);
     }
   return(0);
}

/***************************************************************************
  NAME   : bind_default_row_psgTab
  PURPOSE: Allocate a default set of result holders to hold one row of 
           result of a SQL statement. 
****************************************************************************/
 
int bind_default_row_psgTab(long int inhstmt,
		  SQLCHAR *data[],
		  SQLINTEGER data_size[], 
		  SQLSMALLINT *num_of_cols)
{
   SQLHSTMT	   hstmt=(SQLHSTMT)inhstmt;
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
       check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,
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
	    check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	    return(-2);
	  }
        collen[i] = precision; 

        rc = SQLColAttributes(hstmt, i + 1, SQL_COLUMN_DISPLAY_SIZE, 
			      NULL, 0,    NULL, &displaysize);
	if (rc != SQL_SUCCESS)
	  {
#ifdef DEBUGON
	    check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	    return(-3);
	  }
       if (displaysize > (strlen((char *) colname)))
	           collen[i] = displaysize + 1;
       else collen[i] = strlen((char *) colname) + 1;
       /*collen[i] = max(displaysize, strlen((char *) colname)) + 1;*/
 
        data[i] = (SQLCHAR *) malloc((int)collen[i]);
	strcpy((char *)data[i],"");
        rc = SQLBindCol(hstmt, i + 1, SQL_C_CHAR, data[i], collen[i], &data_size[i]);
	if (rc != SQL_SUCCESS)
	  {
#ifdef DEBUGON
	    check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc,
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	    return(-4);
	  }
    }
   return(0);
}

/***************************************************************************
  NAME   : get_row_psgTab
  PURPOSE: Fetch the next (unfetched)  row of result in the 
           row-holder bound by any of the bind_***_psgTab() calls. To fetch
           multiple row use get_rows_psgTab()
****************************************************************************/

int get_row_psgTab(long int inhstmt)
{
   SQLHSTMT	   hstmt=(SQLHSTMT)inhstmt;
   SQLRETURN       rc;

   rc =  SQLFetch(hstmt);
   if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
     {
       if (rc != SQL_NO_DATA_FOUND)
	 {
#ifdef DEBUGON
       check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */
	 }
       return(-1);
     }
   return(0);
}

/***************************************************************************
  NAME   : bind_column_psgTab
  PURPOSE: Allocate an array (or a variable) to hold a column (or cell)
           of results of a  get_rows_psgTab statement. 
****************************************************************************/

int bind_column_psgTab(long int inhstmt,
		   SQLPOINTER data_column,
		   SQLPOINTER size_column,
		   SQLSMALLINT column_type,
		   SQLINTEGER max_data_size, /* in bytes */
		   SQLSMALLINT column_number,
		   SQLINTEGER row_size)
{
   SQLHSTMT	   hstmt=(SQLHSTMT)inhstmt;
   SQLRETURN       rc;

   if (row_size > 1)
     {
       rc = SQLSetStmtOption(hstmt, SQL_ROWSET_SIZE, row_size);
       if (rc != SQL_SUCCESS)
	 {
#ifdef DEBUGON
	   check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
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
       check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

       return(-2);
     }
   return(0);
}

/***************************************************************************
  NAME   : bind_files_to_column_psgTab
  PURPOSE: Allocate an array of file name holders to hold a LOB column
           of results of a  get_rows_psgTab statement. 
****************************************************************************/
 
int bind_files_to_column_psgTab(long int inhstmt,
		   void        *file_names,
		   SQLSMALLINT *file_name_cell_size,
		   SQLUINTEGER  *file_options,
		   SQLINTEGER *file_status,
		   SQLINTEGER *result_size_column,
		   SQLUSMALLINT column_number,
		   SQLINTEGER row_size)
{
   SQLHSTMT	   hstmt=(SQLHSTMT)inhstmt;
   SQLRETURN       rc;


   if (row_size > 1)
     {
       rc = SQLSetStmtOption(hstmt, SQL_ROWSET_SIZE, row_size);
          if (rc != SQL_SUCCESS)
	    {
#ifdef DEBUGON
	      check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

	      return(-1);
	    }
     }
/*   unimplemented function in Postgres
       rc = SQLBindFileToCol(hstmt, column_number, file_names,NULL, 
		   file_options, 255, result_size_column, file_status);
*/
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       	 check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-2);
     }
   return(0);
}

/***************************************************************************
  NAME   : bind_file_to_cell_psgTab
  PURPOSE: Allocate a file name holder to hold a LOB 
           in a row of a  get_row_psgTab statement. 
****************************************************************************/

int bind_file_to_cell_psgTab(long int inhstmt,
		   SQLCHAR  file_name[],
		   SQLSMALLINT *file_name_length,
		   SQLUINTEGER  *file_option,
		   SQLINTEGER *file_status,
		   SQLINTEGER *result_size_column,
		   SQLUSMALLINT column_number)
{
  SQLHSTMT		  hstmt=(SQLHSTMT)inhstmt;
  SQLRETURN       rc;

/*   unimplemented function in Postgres
   rc = SQLBindFileToCol(hstmt, column_number, 
			 bind_file_name, &bind_file_name_length,
			 &bind_file_option, 100,
			 result_size_column, &bind_file_status);
*/
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       	 check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-1);
     }
  return(0);
}

/***************************************************************************
  NAME   : get_rows_psgTab
  PURPOSE: Fetch the next set of  rows of result in the 
           column-arrays bound by any of the bind_***_psgTab() calls. To fetch
           a single row use get_row_psgTab()
****************************************************************************/

int get_rows_psgTab(long int inhstmt, SQLUINTEGER *pcrow)
{
  SQLHSTMT		  hstmt=(SQLHSTMT)inhstmt;
  SQLRETURN       rc;

  rc =  SQLExtendedFetch(hstmt, SQL_FETCH_NEXT,0, pcrow, NULL);
   if (rc != SQL_SUCCESS)
     {
         if (rc != SQL_NO_DATA_FOUND)
#ifdef DEBUGON
       	 check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-1);
     }
  return(0);
}

/***************************************************************************
  NAME   : bind_n_rows_psgTab
  PURPOSE: Allocate arrays  to hold all column of the result of a 
           get_rows_psgTab statement. 
****************************************************************************/

int bind_n_rows_psgTab(long int inhstmt,
		  SQLPOINTER  data_column[],
		  SQLPOINTER  size_column[],
		  SQLINTEGER  max_data_size[],  /* in bytes */
		  SQLSMALLINT column_type[],
		  SQLSMALLINT num_of_columns,
		  SQLINTEGER  row_size)
{
  SQLHSTMT		  hstmt=(SQLHSTMT)inhstmt;
  SQLRETURN       rc;
  int             i;
  SQLINTEGER      displaysize;

 if (row_size > 1)
   {
     rc = SQLSetStmtOption(hstmt, SQL_ROWSET_SIZE, row_size);
     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
       	 check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
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
       	 check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
		     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-2);
       }
  }
  return(0);
}

/***************************************************************************
  NAME   : get_no_of_columns_psgTab
  PURPOSE: Find the number of columns of the result table of an
           exec_sql_psgTab statement. 
****************************************************************************/
  
int get_no_of_columns_psgTab(long int inhstmt,
			SQLSMALLINT *num_of_cols)
{
  SQLHSTMT		  hstmt=(SQLHSTMT)inhstmt;
  SQLRETURN       rc;

  rc = SQLNumResultCols(hstmt, num_of_cols);
  if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
    	 check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-1);
     }
  return(0);
}

/***************************************************************************
  NAME   : get_column_info_psgTab
  PURPOSE: Get assorted column attributes  such as column name, size (as 
           defined in DDL), type, nullability, precision and its scale.
****************************************************************************/
 
int get_column_info_psgTab(long int inhstmt,
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
  SQLHSTMT		  hstmt=(SQLHSTMT)inhstmt;
  SQLRETURN       rc;

  rc = SQLColAttributes(hstmt, column_number, SQL_COLUMN_DISPLAY_SIZE, 
			NULL, 0, NULL, displaysize);
  if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
    	 check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-1);
     }
  rc = SQLDescribeCol(hstmt, column_number, colname, buf_size,
                       colnamelen, coltype, precision, scale, nullable);
  if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
    	 check_error_psgTab(SQL_NULL_HENV, SQL_NULL_HDBC, hstmt, rc, 
	     __LINE__, __FILE__);
#endif /* DEBUGON */

         return(-2);
     }
  return(0);
}

 
close_psgTab_interaction_internal(long int henv, 
		  long int hdbc, int transaction_end_code)

{ 
  MDAS_PSGTAB_SERVER_HNDL *tmp_srvh;
  

  /* transaction_end_code = 1 ABORT TRANSACTION */
  /* transaction_end_code = 0 COMMIT TRANSACTION */
  i =  disconnect_psgTab( henv, hdbc, transaction_end_code); 
    if (i != 0)  
    return(MCAT_DISCONNECT_ERROR);
  i =  close_env_psgTab(henv);
  if (i != 0)  
    return(MCAT_CLOSE_ENV_ERROR);
#ifdef INSMESSON 
  fclose(INSMESSFILE);
#endif /* INSMESSON */ 
#ifdef DEBUGON 
  fclose(LOGMESSFILE); 
#endif /* DEBUGON */ 


  tmp_srvh = (MDAS_PSGTAB_SERVER_HNDL *) serverhndl[0].conn; 
  free (tmp_srvh);

  return(MDAS_SUCCESS);

}

int open_psgTab_interaction_internal(long int *henv, 
		  long int *hdbc)
{
 MDAS_PSGTAB_SERVER_HNDL *tmp_srvh;
 
  i= initMdasEnviron();
  if (i != 0) 
    return (INIT_ENVIRON_ERROR);
  sprintf(psgTabinstancebuffer,"DB2INSTANCE=%s",DB2INSTANCE);
  i = putenv(psgTabinstancebuffer);
  if (i != 0) 
    {
      return (UNIX_PUTENV_ERROR);
    }
#ifdef DEBUGON 
  LOGMESSFILE = fopen(PSGTABLOGFILE,"a");
  if (LOGMESSFILE == NULL)
    {
      fprintf(stderr,"Cannot open log file:%s\n",DB2LOGFILE);
      return (INIT_ENVIRON_ERROR);
    }
#endif /* DEBUGON */ 
#ifdef INSMESSON
  INSMESSFILE = fopen(MDASINSERTSFILE,"a");
 if (INSMESSFILE == NULL)
    {
      fprintf(stderr,"Cannot open inserts file:%s\n",MDASINSERTSFILE);
      return (INIT_ENVIRON_ERROR);
    }
#endif /* INSMESSON */ 
      
 i =  open_env_psgTab(henv);
 if (i != 0){
#ifdef INSMESSON 
   fclose(INSMESSFILE);
#endif /* INSMESSON */ 
#ifdef DEBUGON 
    fclose(LOGMESSFILE); 
#endif /* DEBUGON */ 
    return(MCAT_OPEN_ENV_ERROR);
 }
  i =  connect_psgTab( *henv, hdbc, (unsigned char*) server, 
            (unsigned char*) user, (unsigned char*) pass); 
  if (i != 0){
    elog(NOTICE,"Failure to connnect to database, error code=%d\n",i);
    if (i != -3) {
      /* for case -3, a disconnect has already been done and another
	 can cause a segfault. */
      disconnect_psgTab( *henv, *hdbc, 1); 
    }
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

int commit_psgTab_interaction_internal(long int inhenv, 
		  long int inhdbc, int  transaction_end_code)
{
  SQLHENV   henv=(SQLHENV)inhenv;
  SQLHDBC   hdbc=(SQLHDBC)inhdbc;
  i = SQLTransact(henv, hdbc, transaction_end_code);
  
  return(i);
}

filterQuerypsg(char *query)
{
  char *tmpPtr, *tmpPtr1, *tmpPtr2;

  if ((tmpPtr = strstr(query,"offset")) != NULL) {
     tmpPtr1 =  query;
     GENERATEDSQL[0] = '\0';

     while (tmpPtr != NULL) {
       *tmpPtr = '\0';
       if (checkIfConstant(tmpPtr1) == 0) {
	 strcat(GENERATEDSQL, tmpPtr1);
	 *tmpPtr = 'o';
	 strcat(GENERATEDSQL,"\"offset\"");
	 tmpPtr1 = tmpPtr + 6;
	 tmpPtr = strstr(tmpPtr1, "offset");
       }
       else {
	 *tmpPtr = 'o';
	 tmpPtr = strstr(tmpPtr+1, "offset");
       }
     }
     strcat(GENERATEDSQL, tmpPtr1);
     strcpy(query,GENERATEDSQL);
  }
  if ((tmpPtr = strstr(query," ESCAPE ")) != NULL) {
     tmpPtr1 =  query;
     GENERATEDSQL[0] = '\0';
     
     while (tmpPtr != NULL) {
       tmpPtr = tmpPtr + 8;
       while (*tmpPtr == ' ') tmpPtr++;
       if (*tmpPtr == '\'' && *(tmpPtr+1) == '\\' && *(tmpPtr+2) == '\'') {
	 *(tmpPtr+2) = '\0';
	 strcat(GENERATEDSQL, tmpPtr1);
	 *(tmpPtr+2) =  '\'';
	 strcat(GENERATEDSQL,"\\'");
	 tmpPtr1 = tmpPtr + 3;
	 tmpPtr = strstr(tmpPtr1," ESCAPE ");
       }
       else {
         *tmpPtr--;
	 *tmpPtr = '\0';
	 strcat(GENERATEDSQL, tmpPtr1);
	 *tmpPtr = ' ';
	 tmpPtr1 = tmpPtr;
	 tmpPtr = strstr(tmpPtr1," ESCAPE ");
       }
     }
     strcat(GENERATEDSQL, tmpPtr1);
     strcpy(query,GENERATEDSQL);
     tmpPtr1 =  query;
     GENERATEDSQL[0] = '\0';
     tmpPtr = strchr(tmpPtr1, '\\');
     while ( tmpPtr != NULL) {
       if (*(tmpPtr+1) == '_' || *(tmpPtr+1) == '%' ) {
	 *tmpPtr = '\0';
	 strcat(GENERATEDSQL, tmpPtr1);
	 *tmpPtr =  '\\';
	 strcat(GENERATEDSQL,"\\\\");
	 tmpPtr1 = tmpPtr + 1;
	 tmpPtr = strchr(tmpPtr1, '\\');
       }
       else {
	 *tmpPtr = '\0';
	 strcat(GENERATEDSQL, tmpPtr1);
	 *tmpPtr =  '\\';
	 strcat(GENERATEDSQL,"\\");
	 tmpPtr1 = tmpPtr + 1;
	 tmpPtr = strchr(tmpPtr1, '\\');
       }
       
     }
     strcat(GENERATEDSQL, tmpPtr1);
     strcpy(query,GENERATEDSQL);

  }
  if ((tmpPtr = strstr(query,"/*+")) != NULL &&
      (tmpPtr2 = strstr(query,"*/")) != NULL &&
      tmpPtr < tmpPtr2) {
    *tmpPtr  = '\0';
    strcat(query," ");
    strcat(query, (char *) (tmpPtr2 + 2));
  }

     return;
}

int
checkIfConstant(char *str)
{
  int i,j,k;
  i = 0;
  j = 0;
  k = 0;
  while (str[k]) {
    if (str[k] == '\'') i++;
    if (str[k] == '"') j++;
    k++;
  }
  if ( (i % 2 != 0) || (j  % 2 != 0)) 
    return 1;
  else
    return 0;
}


/*********************  added  to the original mdasC_psg_intenal.c 
     after changing db2->psg and filterQuery to filetrQuerypsg   *****************/


/***************************************************************************
  NAME   : getDataTypeFromMap_psgTab
  PURPOSE: Getting Column DataType Name from Data Type Index given by Informix
****************************************************************************/ 
void
getDataTypeFromMap_psgTab(char *typeName, short int   typeId)
{
  int jj;
        for (jj = 0; jj < MAXDATATYPELISTSIZE ; jj++) {
          if (psgDataTypeCode[jj] == typeId) {
          strcpy(typeName, psgDataTypeName[jj]);
          break;
        }
      }
      if (jj == MAXDATATYPELISTSIZE) 
        strcpy(typeName, psgDataTypeName[0]);
}


int
getColTypeFromColNamepsgTab(long int  henv, long int  hdbc,
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
  i = exec_sql_psgTab( (SQLHENV) henv, (SQLHDBC) hdbc, &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_psgTab( hstmt, &num_of_cols);
  if (i != 0) return(i);
  i = bind_default_row_psgTab( hstmt, data, data_size, &num_of_cols);
  if (i != 0) return(i);
  i = get_row_psgTab(hstmt);
  if (i == 0)       {
    truncate_end_blanks(data_size[0], data[0]);
    truncate_end_blanks(data_size[1], data[1]);
    coltypenum = atoi(data[0]);
    if (coltypenum >255) 
	coltypenum -= 256;
    getDataTypeFromMap_psgTab(colType, coltypenum);
	/*strcpy(colType, (char *) data[0]);*/
  }
  else
    {
      i = done_with_default_result_psgTab(hstmt,data,data_size, num_of_cols);
      return(i);
    }
  
  i = done_with_default_result_psgTab(hstmt,data,data_size, num_of_cols);

  return(MDAS_SUCCESS);
}

int
getColumnInfoForpsgTable(long int  henv, long int  hdbc,
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

  i = exec_sql_psgTab( (SQLHENV) henv, (SQLHENV) hdbc,  &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_psgTab(hstmt, &num_of_cols);
  if (i != 0) return(i);

  *numOfColumns = num_of_cols;
  for (i=1; i < num_of_cols; i++) {
	  
    j = get_column_info_psgTab(hstmt, i+1, (SQLINTEGER) &displaysize[i], colName[i], &tmpcoltype, SMALL_TOKEN , &tmpint2, &nullable[i], (SQLUINTEGER) &precision[i], &scale[i]);

    getDataTypeFromMap_psgTab(colType[i],tmpcoltype);
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
  i = exec_sql_psgTab(henv, hdbc,  &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_psgTab(hstmt, &num_of_cols);
  if (i != 0) return(i);
  i = bind_default_row_psgTab(hstmt, data,  data_size, &num_of_cols);
  if (i != 0) return(i);
  i = get_row_psgTab(hstmt);
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
      i = get_row_psgTab(hstmt);
    }
    *numOfColumns = jj;
  }
  else
    {
      i = done_with_default_result_psgTab(hstmt,data,data_size, num_of_cols);
      return(i);
    }
  
  i = done_with_default_result_psgTab(hstmt,data,data_size, num_of_cols);
*/
  return(MDAS_SUCCESS);
}
  

