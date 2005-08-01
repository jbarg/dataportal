/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/****************************************************************************
** ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA **
** ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA **
** ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA **
** ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA ORA **
**
** Source File Name = oraDvrLib.c
**
**
** PURPOSE: 
**     Contains driver-level library functions to 
**     connect and operate with ORACLE databases. 
**
** CALLABLE FUNCTIONS:
**     open_env_oraTab
**     close_env_oraTab
**     connect_oraTab
**     disconnect_oraTab
**     exec_no_res_sql_oraTab
**     exec_sql_oraTab
**     done_with_result_oraTab
**     done_with_default_result_oraTab
**     bind_default_row_oraTab
**     bind_n_rows_oraTab
**     bind_column_oraTab
**     bind_file_to_cell_oraTab
**     bind_files_to_column_oraTab
**     get_row_oraTab
**     get_rows_oraTab
**     get_no_of_columns_oraTab
**     get_column_info_oraTab
**
** INTERNAL FUNCTIONS USED:
**     logmess_oraTab
**     print_error_oraTab
**     check_error_oraTab
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


#include "mdasC_db2_externs.h"
#include "mdasC_ora_externs.h"
#include "mdasGlobalsExtern.h"
#include "srbErrno.h"
#include "oraDvrLib.h"
/*
#define DEBUGON 1
*/


#ifndef ORAMCAT
FILE *LOGMESSFILE;
char logmess[HUGH_HUGE_STRING];
unsigned char bind_file_name[MAX_TOKEN];
short int  bind_file_name_length;
unsigned long  int  bind_file_option;
long int  bind_file_status;
#endif /* ifndef ORAMCAT */

  static char      oracletaisidbuffer[MAX_TOKEN];
   sword           displaysize;
   sb4             maxcolsize;
   sb1             colname[MAX_TOKEN];
   sb2             coltype;
   sword           colnamelen;
   SQLINTEGER      collen[MAX_TOKEN];
   SQLSMALLINT     precision;
   SQLSMALLINT     scale;
   SQLSMALLINT     nullok;


/****************************************************************************
                                  FUNCTIONS                                  
****************************************************************************/
/***************************************************************************
  NAME   : zeroTabHda
  PURPOSE: Zeroes the Host Data Area
****************************************************************************/

void zeroTabHda(Hda_Def *hdbc)
{
  char *sptr;
  int i;

  sptr = (char *) hdbc;
  for (i = 0; i < sizeof(Hda_Def) - 1 ; i++)
    sprintf(( char *) sptr + i, "\0");
}
/***************************************************************************
  NAME   : oraTableErrorReport
  PURPOSE: Logging error message to elog
****************************************************************************/

void  oraTableErrorReport( Cda_Def *cursor, Lda_Def *lda)
{   sword n;
    char msg[512];

    elog(NOTICE, "-- ORACLE TABLE SID error--\n");
    if (cursor == NULL) {
      if (lda->rc != 0) {
	n = oerhms(lda, lda->rc,  (text *)msg, (sword) sizeof msg);
	elog(NOTICE, "%s\n", msg);
      }
    }
    else {
      n = oerhms(lda, cursor->rc,  (text *)msg, (sword) sizeof msg);
      elog(NOTICE, "%s\n", msg);
    }


    if (cursor != NULL && cursor->fc > 0)
        elog(NOTICE, "Processing OCI function %s:\n",
            oci_func_tab[cursor->fc]);
}

/***************************************************************************
  NAME   : logmess_oraTab
  PURPOSE: Logging message in LOGMESS file handle
****************************************************************************/

void logmess_oraTab(char *message, int rc)
{
   time_t *mmmmclk, mmmmcl;

   mmmmcl = time(NULL);
#ifdef DEBUGON
   fprintf(LOGMESSFILE,"ORA: %24.24s =>%4i  %s\n",ctime(&mmmmcl),rc, message);
   fflush(LOGMESSFILE);
#endif /* DEBUGON */ 
}

/***************************************************************************
  NAME   : print_error_oraTab
  PURPOSE: Prints error messages to ERRMESS file handle.
****************************************************************************/

void print_error_oraTab(Lda_Def *cursor,
		     int rc, 
		     int line,
		     char *file )
{
    sword n;
    text msg[HUGE_STRING];         /* message buffer to hold error text */
#ifdef DEBUGON 
    fprintf(ERRMESSFILE,
	    "ORACLE ERROR: RC= %d, LDARC= %d Reported from %s, line %d \n", 
	    rc, cursor->rc, file, line);

    if (cursor->fc > 0)
      fprintf(ERRMESSFILE,"\n-- ORACLE error when processing OCI function %s \n\n", 
            oci_func_tab[cursor->fc]);
    else
      fprintf(ERRMESSFILE,"\n-- ORACLE error\n");
 
    n = (sword)oerhms((Cda_Def *) cursor, cursor->rc,msg,(sword) sizeof msg);
    fprintf(ERRMESSFILE, "%s\n", msg);
    fflush(ERRMESSFILE);
#endif /* DEBUGON */ 

}

/***************************************************************************
  NAME   : check_error_oraTab
  PURPOSE: Checks severity of error and prints  error messages by
           calling to print_error_oraTab(). 

****************************************************************************/
  
void
check_error_oraTab(Lda_Def *cursor,
		int rc,
		int line,
		char *file)
{

    print_error_oraTab(cursor, rc, line, file);
}          
/***************************************************************************
  NAME   : getDataTypeFromMap_oraTab
  PURPOSE: Getting Column DataType Name from Data Type Index given by Oracle
****************************************************************************/ 
void
getDataTypeFromMap_oraTab(char *typeName, short int   typeId)
{
  int jj;
        for (jj = 0; jj < MAXDATATYPELISTSIZE ; jj++) {
          if (oraDataTypeCode[jj] == typeId) {
          strcpy(typeName, oraDataTypeName[jj]);
          break;
        }
      }
      if (jj == MAXDATATYPELISTSIZE) 
        strcpy(typeName, oraDataTypeName[0]);
}
/***************************************************************************
  NAME   : open_env_oraTab
  PURPOSE: Opening an execution environment in oracle 
****************************************************************************/ 


int open_env_oraTab(long int *inhenv)
{

    Lda_Def *henv;

  if (OracleHome != NULL  )  {
    sprintf(OracleHomeStr,"ORACLE_HOME=%s",OracleHome);
    putenv(OracleHomeStr);
  }


   henv =  malloc(sizeof(Lda_Def));
   *inhenv = (long int) henv;

   if (henv == NULL) 
     {
#ifdef DEBUGON
       logmess_oraTab("SQLAllocEnv", -1);
#endif /* DEBUGON */

       return (ENV_MEMORY_ALLOCATION_ERROR);
     }
#ifdef DEBUGON
   logmess_oraTab("SQLAllocEnv", (int) henv);  
#endif /* DEBUGON */

   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : close_env_oraTab
  PURPOSE: Closing an open execution environment in oracle 
****************************************************************************/
   
int close_env_oraTab(long int inhenv)
{

    Lda_Def *henv;

    henv = (Lda_Def *) inhenv;
   free(henv);
#ifdef DEBUGON
   logmess_oraTab("SQLFreeEnv",  (long int) henv);
#endif /* DEBUGON */

   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : connect_oraTab
  PURPOSE: Opening a connection to oracle server
****************************************************************************/

int connect_oraTab(long int inhenv,
		long int *inhdbc,
		SQLCHAR *databasename, 
		SQLCHAR *user, 
		SQLCHAR *password)
{
  SQLRETURN rc;
  Lda_Def *henv;
  Hda_Def *myhdbc;

  henv = (Lda_Def *) inhenv;

  myhdbc =  malloc(sizeof(Hda_Def));
  *inhdbc = (long int) myhdbc;
  if (*myhdbc == NULL) 
     {
#ifdef DEBUGON
       logmess_oraTab("SQLAllocConnect", -1);
#endif /* DEBUGON */

       return (CONNECT_MEMORY_ALLOCATION_ERROR);
     }
#ifdef DEBUGON
  logmess_oraTab("SQLAllocConnect", (int) myhdbc);
#endif /* DEBUGON */
  zeroTabHda(myhdbc);

  rc = olog(henv, (ub1 *) myhdbc, (text *) user, -1, (text *)password, 
	   -1, (text *) databasename, -1, OCI_LM_DEF);
  if (rc != SQL_SUCCESS) {
    sprintf(oracletaisidbuffer,"ORACLE_SID=%s",databasename);
    putenv(oracletaisidbuffer);
    rc = olog(henv, (ub1 *) myhdbc, (text *) user, -1, (text *)password, 
	   -1, (text *) 0, -1, OCI_LM_DEF);
  }

#ifdef DEBUGON
  sprintf(logmess, "SQLConnect: Database = %s, User = %s", databasename, user);  logmess_oraTab(logmess, rc);
#endif /* DEBUGON */

  if (rc != SQL_SUCCESS)
  {

#ifdef DEBUGON
    check_error_oraTab((Cda_Def *) henv, rc, __LINE__, __FILE__);
    logmess_oraTab("SQLFreeConnect", (int) myhdbc);
#endif /* DEBUGON */

    free(myhdbc);
    return(MCAT_CONNECT_ERROR);
  }
  return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : disconnect_oraTab
  PURPOSE: Closing an open connection  in oracle 

****************************************************************************/
 
int disconnect_oraTab(long int inhenv,
		   long int inhdbc,
		   SQLUSMALLINT transstate)
{
  SQLRETURN rc, rrc;
    Lda_Def *henv;
    Hda_Def *hdbc;

    henv = (Lda_Def *) inhenv;
    hdbc = (Hda_Def *) inhdbc;

   rrc = SQL_SUCCESS ;
   if (transstate == MDAS_CAT_COMMIT)
     rc = ocom(henv);
   else 
     rc = orol(henv);

#ifdef DEBUGON
   sprintf(logmess,"SQLTransact:%d",transstate);
   logmess_oraTab(logmess, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_oraTab((Cda_Def *)henv, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

       rrc = MCAT_TRANSACT_CLOSE_ERROR;
     } 
   rc = ologof(henv);
#ifdef DEBUGON
   logmess_oraTab("SQLDisconnect", rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_oraTab((Cda_Def *) henv, rc, __LINE__, __FILE__);
#endif /* DEBUGON */

       if (rrc == SQL_SUCCESS) rrc = MCAT_DISCONNECT_ERROR;
     }
   free(hdbc);
#ifdef DEBUGON
   logmess_oraTab("SQLFreeConnect", (long int) hdbc);
#endif /* DEBUGON */

   return (rrc);
}

/***************************************************************************
  NAME   : exec_no_res_sql_oraTab
  PURPOSE: Execute a SQL query which has no resulting table, eg. INSERT,
           DELETE, UPDATE or DDL
****************************************************************************/
 
int exec_no_res_sql_oraTab(long int inhenv, 
			long int inhdbc, 
			SQLCHAR *query)
{
     Cda_Def         *hstmt;
     SQLRETURN       rc;
    Lda_Def *henv;
    Hda_Def *hdbc;

    henv = (Lda_Def *) inhenv;
    hdbc = (Hda_Def *) inhdbc;

     hstmt =  malloc(sizeof(Cda_Def));
     if (hstmt == NULL) 
     {
#ifdef DEBUGON
       logmess_oraTab("SQLAllocStmt", -1);
#endif /* DEBUGON */

       return (STMT_MEMORY_ALLOCATION_ERROR);
     }
#ifdef DEBUGON
     logmess_oraTab("SQLAllocStmt", (int) hstmt);
#endif /* DEBUGON */


     rc = oopen(hstmt, henv, (text *) 0, -1, -1, (text *) 0, -1);
#ifdef DEBUGON
     logmess_oraTab("SQLOpenStmt", rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_oraTab(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
       logmess_oraTab("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

       oclose(hstmt);
       free(hstmt);
       return(MCAT_CURSOR_OPEN_ERROR);
     }

  rc = ocof(henv);
#ifdef DEBUGON
  logmess_oraTab("SQLSetConnectOption:SQL_AUTOCOMMIT_OFF",rc);
#endif /* DEBUGON */

  if (rc != SQL_SUCCESS)
   {
#ifdef DEBUGON
        check_error_oraTab((Cda_Def *)henv, rc, __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
       logmess_oraTab("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

       oclose(hstmt);
       free(hstmt);
        return(MCAT_AUTOCOMMIT_TURNOFF_ERROR);
    }


     rc = oparse(hstmt, query, (sb4) -1, DEFER_PARSE, (ub4) VERSION_7);
     sprintf(logmess, "SQLParse:%s", query);
#ifdef DEBUGON
     logmess_oraTab(logmess,  rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_oraTab(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
	 logmess_oraTab("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

	 oclose(hstmt);
	 free(hstmt);
	 return(MCAT_PARSE_ERROR);
       }

     rc = oexec(hstmt);
     sprintf(logmess, "SQLExecNoResDirect:%s", query);
#ifdef DEBUGON
     logmess_oraTab(logmess,  rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_oraTab(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
	 logmess_oraTab("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

	 if (hstmt->rc == ORA_UNIQUE_CONSTRAINT_VIOLATION )
	 {
	   oclose(hstmt);
	   free(hstmt);
	   return(DB_UNIQUE_CONSTRAINT_VIOLATION);
	 }
	 oclose(hstmt);
	 free(hstmt);
	 return(MCAT_EXEC_ERROR);
       }
#ifdef DEBUGON
     logmess_oraTab("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

     oclose(hstmt);
     free(hstmt);
     return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : exec_sql_oraTab
  PURPOSE: Execute a SQL query which mave have  resulting table
****************************************************************************/

int exec_sql_oraTab(long int inhenv, 
		 long int inhdbc,
		 long int *inhstmt,
		 SQLCHAR *query)
{
     SQLRETURN       rc;
    Lda_Def *henv;
    Hda_Def *hdbc;
    Cda_Def *hstmt;

    henv = (Lda_Def *) inhenv;
    hdbc = (Hda_Def *) inhdbc;
     hstmt =  malloc(sizeof(Cda_Def));
     *inhstmt = (long int) hstmt;
     if (hstmt == NULL) 
     {
#ifdef DEBUGON
       logmess_oraTab("SQLAllocStmt", -1);
#endif /* DEBUGON */

       return (STMT_MEMORY_ALLOCATION_ERROR);
     }
#ifdef DEBUGON
     logmess_oraTab("SQLAllocStmt", (int) hstmt);
#endif /* DEBUGON */

     rc = oopen(hstmt, henv, (text *) 0, -1, -1, (text *) 0, -1);
#ifdef DEBUGON
     logmess_oraTab("SQLOpenStmt", rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_oraTab(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
       logmess_oraTab("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

       oclose(hstmt);
       free(hstmt);
       return(MCAT_CURSOR_OPEN_ERROR);
     }

  rc = ocof(henv);
#ifdef DEBUGON
  logmess_oraTab("SQLSetConnectOption:SQL_AUTOCOMMIT_OFF",rc);
#endif /* DEBUGON */

  if (rc != SQL_SUCCESS)
   {
#ifdef DEBUGON
        check_error_oraTab((Cda_Def *)henv, rc, __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
       logmess_oraTab("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

       oclose(hstmt); 
       free(hstmt);
       return(MCAT_AUTOCOMMIT_TURNOFF_ERROR);
    }


     rc = oparse(hstmt, query, (sb4) -1, DEFER_PARSE, (ub4) VERSION_7);
     sprintf(logmess, "SQLParse:%s", query);
#ifdef DEBUGON
     logmess_oraTab(logmess,  rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_oraTab(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
	 logmess_oraTab("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

	 oclose(hstmt);
	 free(hstmt);
	 return(MCAT_PARSE_ERROR);
       }


     rc = oexec(hstmt);
     sprintf(logmess, "SQLExecDirect:%s", query);
#ifdef DEBUGON
     logmess_oraTab(logmess, rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_oraTab(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
	 logmess_oraTab("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

	 oclose(hstmt);
	 free(hstmt);
	 return(MCAT_EXEC_ERROR);
       }

     return(SQL_SUCCESS);
}


/***************************************************************************
  NAME   : done_with_result_oraTab
  PURPOSE: Close a stament environment opened by exec_sql_oraTab() but which
           used user allocated result structure
****************************************************************************/

int done_with_result_oraTab(long int inhstmt)
{  
    Cda_Def *hstmt;

    hstmt = (Cda_Def *) inhstmt;

#ifdef DEBUGON
   logmess_oraTab("SQLFreeStmt",  (long int) hstmt);
#endif /* DEBUGON */

   oclose(hstmt);
   free (hstmt);

   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : done_with_default_result_oraTab
  PURPOSE: Close a stament environment opened by exec_sql_oraTab() which used
           result structure build by bind_default_row_oraTab()
****************************************************************************/
 
int done_with_default_result_oraTab(long int inhstmt,
				  SQLCHAR *data[],
				  SQLINTEGER data_size[],
				  SQLSMALLINT num_of_cols)
{  
   SQLRETURN rc;
   int i;
   Cda_Def *hstmt;
   
    hstmt = (Cda_Def *) inhstmt;
   for (i =  0; i < num_of_cols; i++){
     free(data[i]);
   }
#ifdef DEBUGON
   logmess_oraTab("SQLFreeStmt",  (long int) hstmt);
#endif /* DEBUGON */

   oclose(hstmt);
   free (hstmt);


   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : bind_default_row_oraTab
  PURPOSE: Allocate a default set of result holders to hold one row of 
           result of a SQL statement. 
****************************************************************************/
 
int bind_default_row_oraTab(long int inhstmt,
		  SQLCHAR *data[],
		  SQLINTEGER data_size[], 
		  SQLSMALLINT *num_of_cols)
{
   SQLRETURN       rc;
   int             i, col;
   Cda_Def *hstmt;
   
   hstmt = (Cda_Def *) inhstmt;

    for (col = 1; col < MAX_TOKEN ; col++) 
      {
	colnamelen = MAX_TOKEN;
	rc = odescr(hstmt, col, 
		    &maxcolsize,
		    (sb2 *) &coltype, (sb1 *) colname,
		    (sb4 *) &colnamelen, (sb4 *) &displaysize,
	            &precision, &scale,   &nullok);

	if (rc != SQL_SUCCESS)
	  {
	    if (hstmt->rc == VAR_NOT_IN_LIST)
                break;
#ifdef DEBUGON
	    check_error_oraTab(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

	    return(MCAT_SELECT_DESCRIBE_ERROR);
	  }
       if (displaysize > (strlen((char *) colname)))
	           collen[col-1] = displaysize + 1;
       else collen[col-1] = strlen((char *) colname) + 1;
       data[col-1] = (SQLCHAR *) malloc((int)collen[col-1]);
       rc = odefin(hstmt, col, (ub1 *) data[col-1],collen[col-1], STRING_TYPE,
		    (sword) -1,  (sb2 *) 0, (text *) 0, -1, -1,
       		    (ub2 *)  &data_size[col-1], (ub2 *) 0);
       if (rc != SQL_SUCCESS)
	 {
#ifdef DEBUGON
	   check_error_oraTab(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

	   return(MCAT_SELECT_DEFINE_ERROR);
	 }
      }
    *num_of_cols = col - 1;
   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : get_row_oraTab
  PURPOSE: Fetch the next (unfetched)  row of result in the 
           row-holder bound by any of the bind_***_oraTab() calls. To fetch
           multiple row use get_rows_oraTab()
****************************************************************************/

int get_row_oraTab(long int inhstmt)
{
   SQLRETURN       rc;
   Cda_Def *hstmt;
   
   hstmt = (Cda_Def *) inhstmt;
   rc =  ofetch(hstmt);
   if (rc != SQL_SUCCESS)
     {
       if (hstmt->rc == NO_DATA_FOUND )
	 {
	   return(MCAT_NO_DATA_FOUND);
	 }
       else if (hstmt->rc == NULL_VALUE_RETURNED )
	 {
	   return(SQL_SUCCESS);
	 }
       else
	 {
#ifdef DEBUGON
	   check_error_oraTab(hstmt, rc, __LINE__, __FILE__);
#endif /* DEBUGON */

	   return(MCAT_FETCH_ERROR);
	 }
     }
   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : get_no_of_columns_oraTab
  PURPOSE: Find the number of columns of the result table of an
           exec_sql_oraTab statement. 
****************************************************************************/
  
int get_no_of_columns_oraTab(long int inhstmt,
                        SQLSMALLINT *num_of_cols)
{
  SQLRETURN       rc;
  int             i, col;
  Cda_Def *hstmt;
   
  hstmt = (Cda_Def *) inhstmt;

    for (col = 1; col < MAX_TOKEN ; col++) 
      {
	colnamelen = MAX_TOKEN;
	rc = odescr(hstmt, col, 
		    &maxcolsize,
		    (sb2 *) &coltype, (sb1 *) colname,
		    (sb4 *) &colnamelen, (sb4 *) &displaysize,
	            &precision, &scale,   &nullok);

	if (rc != SQL_SUCCESS)
	  {
	    if (hstmt->rc == VAR_NOT_IN_LIST)
                break;
#ifdef DEBUGON
	    check_error_oraTab(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

	    return(MCAT_NUM_RESULT_ERROR);
	  }
      }
    *num_of_cols = col - 1;
    return(SQL_SUCCESS);

}
/***************************************************************************
  NAME   : get_column_info_oraTab
  PURPOSE: Get assorted column attributes  such as column name, size (as 
           defined in DDL), type, nullability, precision and its scale.
****************************************************************************/
 
int get_column_info_oraTab(long int inhstmt,
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
  Cda_Def *hstmt;
  sb4    *mybufptr;
  sb4     mybufsize;
  mybufsize = (sb4) buf_size;
  hstmt = (Cda_Def *) inhstmt;
	rc = odescr(hstmt, column_number,
		    &maxcolsize,
		    (sb2 *) coltype, (sb1 *) colname,
		    (sb4 *) &mybufsize, (sb4 *) displaysize,
	             (sb2 *) precision,  (sb2 *) scale,  
		    (sb2 *) nullable );
	if (rc != SQL_SUCCESS)
	  {
	    if (hstmt->rc != VAR_NOT_IN_LIST)
	      {
#ifdef DEBUGON
		check_error_oraTab(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

		return(MCAT_SELECT_DESCRIBE_ERROR);
	      }
	  }

  return(SQL_SUCCESS);
}

int
getColTypeFromColNameoraTab(long int  henv, long int  hdbc,
		      char *colType, char *colName, char *inTableName)
{
  char *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4;
  char sqlQuery[HUGE_STRING];
  int i,j;
  long int   hstmt;
  long int   data_size[4];
  unsigned char       *data[4];


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
    sprintf(sqlQuery,"select DATA_TYPE from ALL_TAB_COLUMNS where COLUMN_NAME = '%s' and TABLE_NAME = '%s' and OWNER = '%s'",    
	    tmpPtr4, tmpPtr+1,tmpPtr2 );
    *tmpPtr = '.';
  }
  else {
    sprintf(sqlQuery,"select DATA_TYPE from ALL_TAB_COLUMNS where COLUMN_NAME = '%s' and TABLE_NAME = '%s'",    
	    tmpPtr4, tmpPtr2 );
  }
  upper(sqlQuery);
  i = exec_sql_oraTab(henv, hdbc,  &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_oraTab(hstmt, &num_of_cols);
  if (i != 0) return(i);
  i = bind_default_row_oraTab(hstmt, data,  data_size, &num_of_cols);
  if (i != 0) return(i);
  i = get_row_oraTab(hstmt);
  if (i == 0)       {
    truncate_end_blanks(data_size[0], data[0]);
    strcpy(colType, (char *) data[0]);
  }
  else
    {
      i = done_with_default_result_oraTab(hstmt,data,data_size, num_of_cols);
      return(i);
    }
  
  i = done_with_default_result_oraTab(hstmt,data,data_size, num_of_cols);

  return(MDAS_SUCCESS);
}

int
getColumnInfoFororaTable(long int  henv, long int  hdbc,
		      char *inTableName,
		      int *numOfColumns,
		      char *colName[],
		      char coltype[MAX_TABLE_COLS][SMALL_TOKEN],
		      long int displaysize[],
		      short int nullable[],
		      unsigned long int precision[],
		      short int   scale[])
{
  char *tmpPtr, *tmpPtr2, *tmpPtr3, *tmpPtr4;
  char sqlQuery[HUGE_STRING];
  int i,j, jj;
  long int   hstmt;
  long int   data_size[10];
  unsigned char       *data[10];

  tmpPtr2 = inTableName;
  while (isspace(*tmpPtr2)) tmpPtr2++;
  tmpPtr3 = inTableName + strlen(inTableName) -1;
  while (isspace(*tmpPtr3)) tmpPtr3--;
  *(tmpPtr3 +1) = '\0';

  if ((tmpPtr =  strstr(tmpPtr2,".")) != NULL) {
    *tmpPtr = '\0';
    sprintf(sqlQuery,"select distinct COLUMN_NAME, DATA_TYPE, DATA_LENGTH, DATA_PRECISION, \
 DATA_SCALE,  NULLABLE ,  COLUMN_ID from ALL_TAB_COLUMNS \
 where TABLE_NAME = '%s' and OWNER = '%s' order by COLUMN_ID ASC",    
	    tmpPtr+1,tmpPtr2 );
    *tmpPtr = '.';
  }
  else {
    sprintf(sqlQuery,"select distinct COLUMN_NAME, DATA_TYPE, DATA_LENGTH, DATA_PRECISION, \
 DATA_SCALE,  NULLABLE ,  COLUMN_ID from ALL_TAB_COLUMNS \
 where TABLE_NAME = '%s' order by COLUMN_ID ASC",    
	     tmpPtr2 );
  }
  upper(sqlQuery);
  i = exec_sql_oraTab(henv, hdbc,  &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_oraTab(hstmt, &num_of_cols);
  if (i != 0) return(i);
  i = bind_default_row_oraTab(hstmt, data,  data_size, &num_of_cols);
  if (i != 0) return(i);
  i = get_row_oraTab(hstmt);
  if (i == 0)       {
    jj = 0;
    while (i == 0) {
      for (i = 0 ; i < num_of_cols; i++) {
	truncate_end_blanks(data_size[i], data[i]);
      }
      colName[jj] = strdup((char*) data[0]);
      strcpy(coltype[jj], (char*) data[1]);
      displaysize[jj] = atoi((char*) data[2]);
      precision[jj] = atoi((char*) data[3]);
      scale[jj] = atoi((char*) data[4]);
      if (!strcmp((char*) data[5], "Y"))
	nullable[jj] = 1;
      else
	nullable[jj] = 0;
      jj++;
      i = get_row_oraTab(hstmt);
    }
    *numOfColumns = jj;
  }
  else
    {
      i = done_with_default_result_oraTab(hstmt,data,data_size, num_of_cols);
      return(i);
    }
  
  i = done_with_default_result_oraTab(hstmt,data,data_size, num_of_cols);

  return(MDAS_SUCCESS);
}
  

/***** NOT NEEDED 
close_oraTab_interaction_internal(long int  henv, 
		  long int  hdbc, int transaction_end_code)

{ 
  MDAS_DB2_SERVER_HNDL *tmp_srvh;
  
  if (henv < 0)
	return(MCAT_CLOSE_ENV_ERROR);


  i =  disconnect_oraTab( henv, hdbc, transaction_end_code); 
    if (i != 0)  
    return(MCAT_DISCONNECT_ERROR);
  i =  close_env_oraTab(henv);
  if (i != 0)  
    return(MCAT_CLOSE_ENV_ERROR);
#ifdef INSMESSON 
  fclose(INSMESSFILE);
#endif
#ifdef DEBUGON 
  fclose(LOGMESSFILE); 
#endif


  tmp_srvh = (MDAS_DB2_SERVER_HNDL *) serverhndl[0].conn; 
  free (tmp_srvh);

  return(MDAS_SUCCESS);

}


int open_oraTab_interaction_internal(long int  *henv, 
		  long int  *hdbc)
{

 MDAS_DB2_SERVER_HNDL *tmp_srvh;
 
  i= initMdasEnviron();
  if (i != 0) 
    return (i);
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
#endif 

#ifdef INSMESSON
  INSMESSFILE = fopen(MDASINSERTSFILE,"a");
 if (INSMESSFILE == NULL)
    {fprintf(stderr,"Cannot open inserts file:%s\n",MDASINSERTSFILE);
    return (INIT_ENVIRON_ERROR);
    }
#endif

  i =  open_env_oraTab(henv);
  if (i != 0)
    {
#ifdef INSMESSON 
    fclose(INSMESSFILE);
#endif 
#ifdef DEBUGON 
    fclose(LOGMESSFILE); 
#endif
     return(MCAT_OPEN_ENV_ERROR);
    }
  i =  connect_oraTab( *henv, hdbc, (unsigned char*) server, 
            (unsigned char*) user, (unsigned char*) pass); 
  if (i != 0){
    close_env_oraTab(*henv); 
#ifdef INSMESSON 
    fclose(INSMESSFILE);
#endif
#ifdef DEBUGON 
    fclose(LOGMESSFILE); 
#endif
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

int commit_oraTab_interaction_internal(long int inhenv, 
		  long int  inhdbc, int  transaction_end_code)
{
  SQLRETURN rc;
  Lda_Def *henv;
  Hda_Def *hdbc;
   
  henv = (Lda_Def *) inhenv;
  hdbc = (Hda_Def *) inhdbc;
  
   if (transaction_end_code == MDAS_CAT_COMMIT)
     rc = ocom(henv);
   else 
     rc = orol(henv);
   sprintf(logmess,"SQLTransact:%d", transaction_end_code);
#ifdef DEBUGON
   logmess_oraTab(logmess, rc);
#endif

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_oraTab((Cda_Def *)henv, rc,  __LINE__, __FILE__);
#endif

       return(MCAT_TRANSACT_CLOSE_ERROR);
     } 
  return(MDAS_SUCCESS);
}

 NOT NEEDED *****/





