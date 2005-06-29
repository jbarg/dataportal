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
** Source File Name = mdas_ora_library.c
**
**
** PURPOSE: 
**     Contains driver-level library functions to 
**     connect and operate with ORACLE databases. 
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
#include "elogExtern.h"

/***** DO THIS ONLY FOR Local Timing into db2LogFile
#define LOGTIMINGMCAT 1 
#define INSTRON 1
****/
/*** TURN OFF IF EVEN MINIMUM DEBUG IS NOT REQUIRED ***/
#define DEBUGMIN 1
/****/

char SLSQUERY1[]="SELECT DISTINCT $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.data_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_TYP.data_typ_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_GRP.data_grp_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.data_size  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.is_dirty  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.data_id   FROM   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_TYP , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_GRP , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_IDGRPUSER1 , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_ACCS , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_ADC_REPL";
char NEWSLSQUERY1[]="SELECT  /*+ ORDERED  INDEX ($$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_ACCS user_id) */ DISTINCT $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.data_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_TYP.data_typ_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_GRP.data_grp_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.data_size  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.is_dirty  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.data_id   FROM  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_GRP ,   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_ADC_REPL , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_ACCS, $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_IDGRPUSER1 ,  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_TYP";

char SLSQUERY2[]="SELECT DISTINCT $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_ADC_REPL.repl_enum  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.data_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_TYP.data_typ_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.modify_timestamp  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_GRP.data_grp_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.data_size  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AR_PHYSICAL.phy_rsrc_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_CD_OWNER_USER.user_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.is_dirty  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_CONTAINER.container_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.data_id   FROM   $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_ADC_REPL , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_TYP , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_GRP , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AR_PHYSICAL , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_CD_OWNER_USER , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_CONTAINER , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_IDGRPUSER1 , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_ACCS";
char NEWSLSQUERY2[]="SELECT /*+ ORDERED  INDEX ($$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_ACCS user_id) */ DISTINCT $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_ADC_REPL.repl_enum  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.data_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_TYP.data_typ_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.modify_timestamp  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_GRP.data_grp_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.data_size  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AR_PHYSICAL.phy_rsrc_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_CD_OWNER_USER.user_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.is_dirty  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_CONTAINER.container_name  , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL.data_id   FROM  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_GRP , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_REPL , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_ADC_REPL , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AD_ACCS, $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_IDGRPUSER1 ,  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_DATA_TYP, $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_CD_OWNER_USER, $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_AR_PHYSICAL , $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$MDAS_TD_CONTAINER";

FILE *PERFFILE;
FILE *LOGMESSFILE;
char logmess[HUGH_HUGE_STRING];
unsigned char bind_file_name[MAX_TOKEN];
short int  bind_file_name_length;
unsigned long  int  bind_file_option;
long int  bind_file_status;
char oraclesidbuffer[SMALL_TOKEN];



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
  NAME   : zeroHda
  PURPOSE: Zeroes the Host Data Area
****************************************************************************/

void zeroHda(hdbc)
{
  char *sptr;
  int i;

  sptr = (char *) hdbc;
  for (i = 0; i < sizeof(Hda_Def) - 1 ; i++)
    sprintf(( char *) sptr + i, "\0");
}


/***************************************************************************
  NAME   : logmess_db2
  PURPOSE: Logging message in LOGMESS file handle
****************************************************************************/

void logmess_db2(char *message, int rc)
{
#ifdef INSTRON
   hrtime_t highResTime;
#endif /* INSTRON */
#ifdef DEBUGON
   time_t *mmmmclk, mmmmcl;

   mmmmcl = time(NULL);
   fprintf(LOGMESSFILE,"ORA: %24.24s =>%4i  %s\n",ctime(&mmmmcl),rc, message);
#endif /* DEBUGON */ 
#ifdef LOGTIMINGMCAT
#ifndef INSTRON
   elogTiming ("MCAT: ",message);
#else
   highResTime = gethrtime();
   if (strstr(message,"SQLExec") == NULL)
     fprintf(PERFFILE,"PER: %lld ns=> %.160s %8i\n",highResTime, message,rc);
   else
     fprintf(PERFFILE,"PER: %lld ns=> %s %8i\n",highResTime, message,rc);
#endif /* INSTRON */
#endif /* LOGTIMINGMCAT */
}

/***************************************************************************
  NAME   : print_error_db2
  PURPOSE: Prints error messages to ERRMESS file handle.
****************************************************************************/

void print_error_db2(Lda_Def *cursor,
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
  NAME   : print_error_elog
  PURPOSE: Prints error messages to ERRMESS file handle.
****************************************************************************/

void print_error_elog(Lda_Def *cursor,
		     int rc, 
		     int line,
		     char *file )
{
    sword n;
    text msg[HUGE_STRING];         /* message buffer to hold error text */
    elog(NOTICE,
	    "ORACLE ERROR: RC= %d, LDARC= %d Reported from %s, line %d \n", 
	    rc, cursor->rc, file, line);

    if (cursor->fc > 0)
      elog(NOTICE,"\n-- ORACLE error when processing OCI function %s \n\n", 
            oci_func_tab[cursor->fc]);
    else
      elog(NOTICE,"\n-- ORACLE error\n");
 
    n = (sword)oerhms((Cda_Def *) cursor, cursor->rc,msg,(sword) sizeof msg);
    elog(NOTICE, "%s\n", msg);
}

/***************************************************************************
  NAME   : check_error_db2
  PURPOSE: Checks severity of error and prints  error messages by
           calling to print_error_db2(). 

****************************************************************************/
  
void
check_error_db2(Lda_Def *cursor,
		int rc,
		int line,
		char *file)
{

    print_error_db2(cursor, rc, line, file);
}          

/***************************************************************************
  NAME   : check_error_elog
  PURPOSE: Checks severity of error and prints  error messages by
           calling to print_error_elog(). 

****************************************************************************/
  
void
check_error_elog(Lda_Def *cursor,
		int rc,
		int line,
		char *file)
{

    print_error_elog(cursor, rc, line, file);
}          

/***************************************************************************
  NAME   : open_env_db2
  PURPOSE: Opening an execution environment in db2 
****************************************************************************/ 


int open_env_db2(long int *inhenv)
{

    Lda_Def *henv;

#ifdef LOGTIMINGMCAT
    logmess_db2("Enter open_env_db2",-2222);
#endif /* LOGTIMINGMCAT */
   sprintf(db2instancebuffer,"ORACLE_HOME=%s",dbhome);
   putenv(db2instancebuffer);

   henv =  malloc(sizeof(Lda_Def));
   bzero(henv, sizeof(Lda_Def));
   *inhenv = (long int) henv;

   if (henv == NULL) 
     {
#ifdef DEBUGON
       logmess_db2("SQLAllocEnv", -1);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave open_env_db2",-1111);
#endif /* LOGTIMINGMCAT */
       return (ENV_MEMORY_ALLOCATION_ERROR);
     }
#ifdef DEBUGON
   logmess_db2("SQLAllocEnv", (int) henv);  
#endif /* DEBUGON */

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave open_env_db2",-2222);
#endif /* LOGTIMINGMCAT */
   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : close_env_db2
  PURPOSE: Closing an open execution environment in db2 
****************************************************************************/
   
int close_env_db2(long int inhenv)
{

    Lda_Def *henv;

#ifdef LOGTIMINGMCAT
    logmess_db2("Enter close_env_db2",-2222);
#endif /* LOGTIMINGMCAT */
    henv = (Lda_Def *) inhenv;
   free(henv);
#ifdef DEBUGON
   logmess_db2("SQLFreeEnv",  (long int) henv);
#endif /* DEBUGON */

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave close_env_db2",-2222);
#endif /* LOGTIMINGMCAT */
   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : connect_db2
  PURPOSE: Opening a connection to db2 server
****************************************************************************/

int connect_db2(long int inhenv,
		long int *inhdbc,
		SQLCHAR *databasename, 
		SQLCHAR *user, 
		SQLCHAR *password)
{
  SQLRETURN rc;
  Lda_Def *henv;
  Hda_Def *myhdbc;

#ifdef LOGTIMINGMCAT
    logmess_db2("Enter connect_db2",-2222);
#endif /* LOGTIMINGMCAT */
  henv = (Lda_Def *) inhenv;

  myhdbc =  malloc(sizeof(Hda_Def));
  *inhdbc = (long int) myhdbc;
  if (*myhdbc == NULL) 
     {
#ifdef DEBUGON
       logmess_db2("SQLAllocConnect", -1);
#endif /* DEBUGON */

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave connect_db2",-1111);
#endif /* LOGTIMINGMCAT */
       return (CONNECT_MEMORY_ALLOCATION_ERROR);
     }
#ifdef DEBUGON
  logmess_db2("SQLAllocConnect", (int) myhdbc);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_db2("Before zeroHda",-1111);
#endif /* LOGTIMINGMCAT */

  zeroHda(myhdbc);

#ifdef LOGTIMINGMCAT
    logmess_db2("Before olog call",-1111);
#endif /* LOGTIMINGMCAT */

  rc = olog(henv, (ub1 *) myhdbc, (text *) user, -1, (text *)password, 
	   -1, (text *) databasename, -1, OCI_LM_DEF);
  if (rc != SQL_SUCCESS) {
    sprintf(oraclesidbuffer,"ORACLE_SID=%s",databasename);
    i = putenv(oraclesidbuffer);
    rc = olog(henv, (ub1 *) myhdbc, (text *) user, -1, (text *)password, 
	      -1, (text *) 0,  -1, OCI_LM_DEF);
  }


#ifdef LOGTIMINGMCAT
    logmess_db2("After olog call",-1111);
#endif /* LOGTIMINGMCAT */
#ifdef DEBUGON
  sprintf(logmess, "SQLConnect: Database = %s, User = %s", databasename, user);  logmess_db2(logmess, rc);
#endif /* DEBUGON */

  if (rc != SQL_SUCCESS)
  {
#ifdef DEBUGMIN
    elog(NOTICE,logmess);
    check_error_elog((Cda_Def *) henv, rc, __LINE__, __FILE__);
#endif /* DEBUGMIN */
#ifdef DEBUGON
    check_error_db2((Cda_Def *) henv, rc, __LINE__, __FILE__);
    logmess_db2("SQLFreeConnect", (int) myhdbc);
#endif /* DEBUGON */

    free(myhdbc);
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave connect_db2",-1111);
#endif /* LOGTIMINGMCAT */
    return(MCAT_CONNECT_ERROR);
  }
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave connect_db2",-2222);
#endif /* LOGTIMINGMCAT */
  return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : disconnect_db2
  PURPOSE: Closing an open connection  in db2 

****************************************************************************/
 
int disconnect_db2(long int inhenv,
		   long int inhdbc,
		   SQLUSMALLINT transstate)
{
  SQLRETURN rc, rrc;
    Lda_Def *henv;
    Hda_Def *hdbc;

    henv = (Lda_Def *) inhenv;
    hdbc = (Hda_Def *) inhdbc;

#ifdef LOGTIMINGMCAT
    logmess_db2("Enter disconnect_db2",-2222);
#endif /* LOGTIMINGMCAT */
   rrc = SQL_SUCCESS ;
   if (transstate == MDAS_CAT_COMMIT)
     rc = ocom(henv);
   else 
     rc = orol(henv);

#ifdef DEBUGON
   sprintf(logmess,"SQLTransact:%d",transstate);
   logmess_db2(logmess, rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2((Cda_Def *)henv, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */
#ifdef DEBUGMIN
       elog(NOTICE,logmess);
       check_error_elog((Cda_Def *)henv, rc,  __LINE__, __FILE__);
#endif /* DEBUGMIN */

       rrc = MCAT_TRANSACT_CLOSE_ERROR;
     } 
   rc = ologof(henv);
#ifdef DEBUGON
   logmess_db2("SQLDisconnect", rc);
#endif /* DEBUGON */

   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2((Cda_Def *) henv, rc, __LINE__, __FILE__);
#endif /* DEBUGON */

       if (rrc == SQL_SUCCESS) rrc = MCAT_DISCONNECT_ERROR;
     }
   free(hdbc);
#ifdef DEBUGON
   logmess_db2("SQLFreeConnect", (long int) hdbc);
#endif /* DEBUGON */

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave disconnect_db2",-2222);
#endif /* LOGTIMINGMCAT */
   return (rrc);
}

/***************************************************************************
  NAME   : exec_no_res_sql_db2
  PURPOSE: Execute a SQL query which has no resulting table, eg. INSERT,
           DELETE, UPDATE or DDL
****************************************************************************/
 
int exec_no_res_sql_db2(long int inhenv, 
			long int inhdbc, 
			SQLCHAR *query)
{
     Cda_Def         *hstmt;
     SQLRETURN       rc;
    Lda_Def *henv;
    Hda_Def *hdbc;
    time_t              tim, time();

    filterQuery(query);
    if (showQuery) {
      time(&tim);
      printf("SQL:%d:%s: %s\n",getpid(),query,ctime(&tim)+4);
      fflush(stdout);
      strcpy(GENERATEDSQL,(char *)query);
    }
#ifdef DEBUGON
    sprintf(logmess, "NORESSQL:%s", query);
    elog(NOTICE,logmess);
#endif
#ifdef LOGTIMINGMCAT
    logmess_db2("Enter exec_no_res_sql_db2",-2222);
#endif /* LOGTIMINGMCAT */
    henv = (Lda_Def *) inhenv;
    hdbc = (Hda_Def *) inhdbc;

     hstmt =  malloc(sizeof(Cda_Def));
     if (hstmt == NULL) 
     {
#ifdef DEBUGON
       logmess_db2("SQLAllocStmt", -1);
#endif /* DEBUGON */

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave exec_no_res_sql_db2",-1111);
#endif /* LOGTIMINGMCAT */
       return (STMT_MEMORY_ALLOCATION_ERROR);
     }
#ifdef DEBUGON
     logmess_db2("SQLAllocStmt", (int) hstmt);
#endif /* DEBUGON */


     rc = oopen(hstmt, henv, (text *) 0, -1, -1, (text *) 0, -1);
#ifdef DEBUGON
     logmess_db2("SQLOpenStmt", (int) hstmt);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
       logmess_db2("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

       oclose(hstmt);
       free(hstmt);
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave exec_no_res_sql_db2",-1111);
#endif /* LOGTIMINGMCAT */
       return(MCAT_CURSOR_OPEN_ERROR);
     }

  rc = ocof(henv);
#ifdef DEBUGON
  logmess_db2("SQLSetConnectOption:SQL_AUTOCOMMIT_OFF",rc);
#endif /* DEBUGON */

  if (rc != SQL_SUCCESS)
   {
#ifdef DEBUGON
        check_error_db2((Cda_Def *)henv, rc, __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
       logmess_db2("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

       oclose(hstmt);
       free(hstmt);
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave exec_no_res_sql_db2",-1111);
#endif /* LOGTIMINGMCAT */
        return(MCAT_AUTOCOMMIT_TURNOFF_ERROR);
    }

#ifdef LOGTIMINGMCAT
    sprintf(logmess, "Before SQLParse:%s", query);
    logmess_db2(logmess,-1111);
#endif /* LOGTIMINGMCAT */

     rc = oparse(hstmt, query, (sb4) -1, DEFER_PARSE, (ub4) VERSION_7);
     sprintf(logmess, "SQLParse:%s", query);
#ifdef DEBUGON
     logmess_db2(logmess,  rc);
#endif /* DEBUGON */


     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_db2(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
	 logmess_db2("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

	 oclose(hstmt);
	 free(hstmt);
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave exec_no_res_sql_db2",-1111);
#endif /* LOGTIMINGMCAT */
	 return(MCAT_PARSE_ERROR);
       }

#ifdef LOGTIMINGMCAT
    logmess_db2("Before SQLExecNoResDirect",-1111);
#endif /* LOGTIMINGMCAT */
     rc = oexec(hstmt);
     sprintf(logmess, "SQLExecNoResDirect:%s", query);
#ifdef DEBUGON
     logmess_db2(logmess,  rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_db2(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */
	
#ifdef DEBUGMIN
	 elog(NOTICE,logmess);
	 check_error_elog(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGMIN */
#ifdef DEBUGON
	 logmess_db2("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

	 oclose(hstmt);
	 free(hstmt);
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave exec_no_res_sql_db2",-1111);
#endif /* LOGTIMINGMCAT */
	 return(MCAT_EXEC_ERROR);
       }
#ifdef DEBUGON
     logmess_db2("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

     oclose(hstmt);
     free(hstmt);
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave exec_no_res_sql_db2",-2222);
#endif /* LOGTIMINGMCAT */
     return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : exec_sql_db2
  PURPOSE: Execute a SQL query which mave have  resulting table
****************************************************************************/

int exec_sql_db2(long int inhenv, 
		 long int inhdbc,
		 long int *inhstmt,
		 SQLCHAR *query)
{
     SQLRETURN       rc;
    Lda_Def *henv;
    Hda_Def *hdbc;
    Cda_Def *hstmt;
    time_t              tim, time();

    filterQuery(query);
    if (showQuery) {
      time(&tim);
      printf("SQL:%d:%s: %s\n",getpid(),query,ctime(&tim)+4);
      fflush(stdout);
      strcpy(GENERATEDSQL,(char *)query);
    }
#ifdef LOGTIMINGMCAT
    logmess_db2("Enter exec_sql_db2",-2222);
#endif /* LOGTIMINGMCAT */
    henv = (Lda_Def *) inhenv;
    hdbc = (Hda_Def *) inhdbc;
     hstmt =  malloc(sizeof(Cda_Def));
     *inhstmt = (long int) hstmt;
     if (hstmt == NULL) 
     {
#ifdef DEBUGON
       logmess_db2("SQLAllocStmt", -1);
#endif /* DEBUGON */

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave exec_sql_db2",-1111);
#endif /* LOGTIMINGMCAT */
       return (STMT_MEMORY_ALLOCATION_ERROR);
     }
#ifdef DEBUGON
     logmess_db2("SQLAllocStmt", (int) hstmt);
#endif /* DEBUGON */

     rc = oopen(hstmt, henv, (text *) 0, -1, -1, (text *) 0, -1);
#ifdef DEBUGON
     logmess_db2("SQLOpenStmt", (int) hstmt);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
       logmess_db2("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

       oclose(hstmt);
       free(hstmt);
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave exec_sql_db2",-1111);
#endif /* LOGTIMINGMCAT */
       return(MCAT_CURSOR_OPEN_ERROR);
     }

  rc = ocof(henv);
#ifdef DEBUGON
  logmess_db2("SQLSetConnectOption:SQL_AUTOCOMMIT_OFF",rc);
#endif /* DEBUGON */

  if (rc != SQL_SUCCESS)
   {
#ifdef DEBUGON
        check_error_db2((Cda_Def *)henv, rc, __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
       logmess_db2("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

       oclose(hstmt); 
       free(hstmt);
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave exec_sql_db2",-1111);
#endif /* LOGTIMINGMCAT */
       return(MCAT_AUTOCOMMIT_TURNOFF_ERROR);
    }

#ifdef LOGTIMINGMCAT
    sprintf(logmess, "Before SQLParse:%s", query);
    logmess_db2(logmess,-1111);
#endif /* LOGTIMINGMCAT */
     rc = oparse(hstmt, query, (sb4) -1, DEFER_PARSE, (ub4) VERSION_7);
     sprintf(logmess, "SQLParse:%s", query);
#ifdef DEBUGON
     logmess_db2(logmess,  rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_db2(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef DEBUGON
	 logmess_db2("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

	 oclose(hstmt);
	 free(hstmt);
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave exec_sql_db2",-1111);
#endif /* LOGTIMINGMCAT */
	 return(MCAT_PARSE_ERROR);
       }


#ifdef LOGTIMINGMCAT
    logmess_db2("Before SQLExecDirect",-1111);
#endif /* LOGTIMINGMCAT */
     rc = oexec(hstmt);
     sprintf(logmess, "SQLExecDirect:%s", query);
#ifdef DEBUGON
     logmess_db2(logmess, rc);
#endif /* DEBUGON */

     if (rc != SQL_SUCCESS)
       {
#ifdef DEBUGON
	 check_error_db2(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */
#ifdef DEBUGMIN
	 elog(NOTICE,logmess);
	 check_error_elog(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGMIN */
#ifdef DEBUGON
	 logmess_db2("SQLFreeStmt", (int) hstmt);
#endif /* DEBUGON */

	 oclose(hstmt);
	 free(hstmt);
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave exec_sql_db2",-1111);
#endif /* LOGTIMINGMCAT */
	 return(MCAT_EXEC_ERROR);
       }

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave exec_sql_db2",-2222);
#endif /* LOGTIMINGMCAT */
     return(SQL_SUCCESS);
}


/***************************************************************************
  NAME   : done_with_result_db2
  PURPOSE: Close a stament environment opened by exec_sql_db2() but which
           used user allocated result structure
****************************************************************************/

int done_with_result_db2(long int inhstmt)
{  
    Cda_Def *hstmt;

#ifdef LOGTIMINGMCAT
    logmess_db2("Enter done_with_result_db2",-2222);
#endif /* LOGTIMINGMCAT */
    hstmt = (Cda_Def *) inhstmt;

#ifdef DEBUGON
   logmess_db2("SQLFreeStmt",  (long int) hstmt);
#endif /* DEBUGON */

   oclose(hstmt);
   free (hstmt);

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave done_with_result_db2",-2222);
#endif /* LOGTIMINGMCAT */
   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : done_with_default_result_db2
  PURPOSE: Close a stament environment opened by exec_sql_db2() which used
           result structure build by bind_default_row_db2()
****************************************************************************/
 
int done_with_default_result_db2(long int inhstmt,
				  SQLCHAR *data[],
				  SQLINTEGER data_size[],
				  SQLSMALLINT num_of_cols)
{  
   SQLRETURN rc;
   int i;
   Cda_Def *hstmt;
   
#ifdef LOGTIMINGMCAT
    logmess_db2("Enter done_with_default_result_db2",-2222);
#endif /* LOGTIMINGMCAT */
    hstmt = (Cda_Def *) inhstmt;
   for (i =  0; i < num_of_cols; i++){
     free(data[i]);
   }
#ifdef DEBUGON
   logmess_db2("SQLFreeStmt",  (long int) hstmt);
#endif /* DEBUGON */

   oclose(hstmt);
   free (hstmt);


#ifdef LOGTIMINGMCAT
    logmess_db2("Leave done_with_default_result_db2",-2222);
#endif /* LOGTIMINGMCAT */
   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : bind_default_row_db2
  PURPOSE: Allocate a default set of result holders to hold one row of 
           result of a SQL statement. 
****************************************************************************/
 
int bind_default_row_db2(long int inhstmt,
		  SQLCHAR *data[],
		  SQLINTEGER data_size[], 
		  SQLSMALLINT *num_of_cols)
{
   SQLRETURN       rc;
   int             i, col;
   Cda_Def *hstmt;
   
#ifdef LOGTIMINGMCAT
    logmess_db2("Enter bind_default_row_db2",-2222);
#endif /* LOGTIMINGMCAT */
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
	    check_error_db2(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave bind_default_row_db2",-1111);
#endif /* LOGTIMINGMCAT */
	    return(MCAT_SELECT_DESCRIBE_ERROR);
	  }
       if (displaysize > (strlen((char *) colname)))
	           collen[col-1] = displaysize + 1;
       else collen[col-1] = strlen((char *) colname) + 1;
       data[col-1] = (SQLCHAR *) malloc((int)collen[col-1]);
       strcpy((char *)data[col-1],"");
       rc = odefin(hstmt, col, (ub1 *) data[col-1],collen[col-1], STRING_TYPE,
		    (sword) -1,  (sb2 *) 0, (text *) 0, -1, -1,
       		    (ub2 *)  &data_size[col-1], (ub2 *) 0);
       if (rc != SQL_SUCCESS)
	 {
#ifdef DEBUGON
	   check_error_db2(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave bind_default_row_db2",-1111);
#endif /* LOGTIMINGMCAT */
	   return(MCAT_SELECT_DEFINE_ERROR);
	 }
      }
    *num_of_cols = col - 1;
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave bind_default_row_db2",-2222);
#endif /* LOGTIMINGMCAT */
   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : get_row_db2
  PURPOSE: Fetch the next (unfetched)  row of result in the 
           row-holder bound by any of the bind_***_db2() calls. To fetch
           multiple row use get_rows_db2()
****************************************************************************/

int get_row_db2(long int inhstmt)
{
   SQLRETURN       rc;
   Cda_Def *hstmt;
   
#ifdef LOGTIMINGMCAT
    logmess_db2("Enter get_row_db2",-2222);
#endif /* LOGTIMINGMCAT */
   hstmt = (Cda_Def *) inhstmt;
   rc =  ofetch(hstmt);
   if (rc != SQL_SUCCESS)
     {
       if (hstmt->rc == NO_DATA_FOUND )
	 {
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave get_row_db2",-1111);
#endif /* LOGTIMINGMCAT */
	   return(MCAT_NO_DATA_FOUND);
	 }
       else if (hstmt->rc == NULL_VALUE_RETURNED )
	 {
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave get_row_db2",-2222);
#endif /* LOGTIMINGMCAT */
	   return(SQL_SUCCESS);
	 }
       else
	 {
#ifdef DEBUGON
	   check_error_db2(hstmt, rc, __LINE__, __FILE__);
#endif /* DEBUGON */
#ifdef DEBUGMIN
	 check_error_elog(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGMIN */
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave get_row_db2",-1111);
#endif /* LOGTIMINGMCAT */
	   return(MCAT_FETCH_ERROR);
	 }
     }
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave get_row_db2",-2222);
#endif /* LOGTIMINGMCAT */
   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : get_no_of_columns_db2
  PURPOSE: Find the number of columns of the result table of an
           exec_sql_db2 statement. 
****************************************************************************/
  
int get_no_of_columns_db2(long int inhstmt,
                        SQLSMALLINT *num_of_cols)
{
  SQLRETURN       rc;
  int             i, col;
  Cda_Def *hstmt;

  /**** Added Aug 24,2000 for speed up */
   *num_of_cols =  1;
   return(MDAS_SUCCESS);
}

int old_get_no_of_columns_db2(long int inhstmt,
                        SQLSMALLINT *num_of_cols)
{
  SQLRETURN       rc;
  int             i, col;
  Cda_Def *hstmt;

#ifdef LOGTIMINGMCAT
    logmess_db2("Enter get_no_of_columns_db2",-2222);
#endif /* LOGTIMINGMCAT */
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
	    check_error_db2(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave get_no_of_columns_db2",-1111);
#endif /* LOGTIMINGMCAT */
	    return(MCAT_NUM_RESULT_ERROR);
	  }
      }
    *num_of_cols = col - 1;
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave get_no_of_columns_db2",-2222);
#endif /* LOGTIMINGMCAT */
    return(SQL_SUCCESS);

}
/***************************************************************************
  NAME   : get_column_info_db2
  PURPOSE: Get assorted column attributes  such as column name, size (as 
           defined in DDL), type, nullability, precision and its scale.
****************************************************************************/
 
int get_column_info_db2(long int inhstmt,
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

#ifdef LOGTIMINGMCAT
    logmess_db2("Enter get_column_info_db2",-2222);
#endif /* LOGTIMINGMCAT */
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
		check_error_db2(hstmt, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave get_column_info_db2",-1111);
#endif /* LOGTIMINGMCAT */
		return(MCAT_SELECT_DESCRIBE_ERROR);
	      }
	  }

#ifdef LOGTIMINGMCAT
    logmess_db2("Leave get_column_info_db2",-2222);
#endif /* LOGTIMINGMCAT */
  return(SQL_SUCCESS);
}



 
close_db2_interaction_internal(long int  henv, 
		  long int  hdbc, int transaction_end_code)

{ 
  MDAS_DB2_SERVER_HNDL *tmp_srvh;
  
#ifdef LOGTIMINGMCAT
    logmess_db2("Enter close_db2_interaction_internal",-2222);
#endif /* LOGTIMINGMCAT */
  if (henv < 0)
    {
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave close_db2_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
	return(MCAT_CLOSE_ENV_ERROR);
    }
  /* transaction_end_code = 1 ABORT TRANSACTION */
  /* transaction_end_code = 0 COMMIT TRANSACTION */
  i =  disconnect_db2( henv, hdbc, transaction_end_code); 
  if (i != 0) {
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave close_db2_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
    return(MCAT_DISCONNECT_ERROR);
  }
  i =  close_env_db2(henv);
  if (i != 0) {
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave close_db2_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
    return(MCAT_CLOSE_ENV_ERROR);
  }
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave close_db2_interaction_internal",-2222);
#endif /* LOGTIMINGMCAT */
#ifdef INSMESSON 
  fflush(INSMESSFILE);
  fclose(INSMESSFILE);
#endif /* INSMESSON */ 
#ifdef DEBUGON 
  fclose(LOGMESSFILE); 
#endif /* DEBUGON */ 


  tmp_srvh = (MDAS_DB2_SERVER_HNDL *) serverhndl[0].conn; 
  free (tmp_srvh);

  return(MDAS_SUCCESS);

}

int open_db2_interaction_internal(long int  *henv, 
		  long int  *hdbc)
{

 MDAS_DB2_SERVER_HNDL *tmp_srvh;
 
  i= initMdasEnviron();
   if (i != 0) 
    return (i);
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
    }
#endif /* INSMESSON */ 
#ifdef LOGTIMINGMCAT
#ifdef INSTRON
    sprintf(logmess,"%s.%ld",DB2LOGFILE, getpid());
    PERFFILE = fopen(logmess,"a");
    if (PERFFILE == NULL)
    {fprintf(stderr,"Cannot open performance file:%s\n",logmess);
    return (INIT_ENVIRON_ERROR);
    }
#endif /* INSTRON */
    logmess_db2("Enter open_db2_interaction_internal",-2222);
#endif /* LOGTIMINGMCAT */
  sprintf(db2instancebuffer,"DB2INSTANCE=%s",DB2INSTANCE);
  i = putenv(db2instancebuffer);
  if (i != 0) 
    {
    return (UNIX_PUTENV_ERROR);
    }

  i =  open_env_db2(henv);
  if (i != 0)
    {
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave open_db2_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */

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
    close_env_db2(*henv); 
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave open_db2_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */

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
      {
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave open_db2_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
    return(MEMORY_ALLOCATION_ERROR);
      }
    strcpy(tmp_srvh->user, user);
    strcpy(tmp_srvh->server,catser);
    tmp_srvh->valid = 1;
    tmp_srvh->henv  = *henv;
    tmp_srvh->hdbc  = *hdbc;
    serverhndl[0].conn = tmp_srvh;
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave open_db2_interaction_internal",-2222);
#endif /* LOGTIMINGMCAT */
  return(MDAS_SUCCESS);
}

int commit_db2_interaction_internal(long int inhenv, 
		  long int  inhdbc, int  transaction_end_code)
{
  SQLRETURN rc;
  Lda_Def *henv;
  Hda_Def *hdbc;
   
#ifdef LOGTIMINGMCAT
    logmess_db2("Enter commit_db2_interaction_internal",-2222);
#endif /* LOGTIMINGMCAT */
  henv = (Lda_Def *) inhenv;
  hdbc = (Hda_Def *) inhdbc;
 
#ifdef DEBUGON
   sprintf(logmess,"Call SQLTransact:%d", transaction_end_code);
   logmess_db2(logmess, rc);
#endif /* DEBUGON */
   if (transaction_end_code == MDAS_CAT_COMMIT)
     rc = ocom(henv);
   else 
     rc = orol(henv);
#ifdef DEBUGON
   sprintf(logmess,"After SQLTransact:%d", transaction_end_code);
   logmess_db2(logmess, rc);
#endif /* DEBUGON */
   if (rc != SQL_SUCCESS)
     {
#ifdef DEBUGON
       check_error_db2((Cda_Def *)henv, rc,  __LINE__, __FILE__);
#endif /* DEBUGON */
#ifdef DEBUGMIN
       elog(NOTICE,logmess);
       check_error_elog((Cda_Def *)henv, rc,  __LINE__, __FILE__);
#endif /* DEBUGMIN */
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave commit_db2_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
       return(MCAT_TRANSACT_CLOSE_ERROR);
     } 
#ifdef LOGTIMINGMCAT
    logmess_db2("Leave commit_db2_interaction_internal",-2222);
#endif /* LOGTIMINGMCAT */
  return(MDAS_SUCCESS);
}




filterQuery(char *query)
{
  char *tmpPtr, *tmpStr1, *tmpStr2;
  char *oldQ;
  
  if ((tmpStr2 = strstr(SLSQUERY1,"$$$$$$$$$$$$$$$$$$$$$")) != NULL) {
    *tmpStr2 = '\0';
    strcat(SLSQUERY1,MDASSCHEMENAME);
    tmpStr1 = tmpStr2 + 30;
    while ((tmpStr2 = strstr(tmpStr1,"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$")) != NULL) {
       *tmpStr2 = '\0';
       strcat(SLSQUERY1, tmpStr1);
       strcat(SLSQUERY1, MDASSCHEMENAME);
       tmpStr1 = tmpStr2 + 30;
    }
    strcat(SLSQUERY1,tmpStr1);  

    tmpStr2 = strstr(NEWSLSQUERY1,"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    *tmpStr2 = '\0';
    strcat(NEWSLSQUERY1,MDASSCHEMENAME);
    tmpStr1 = tmpStr2 + 30;
    while ((tmpStr2 = strstr(tmpStr1,"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$")) != NULL) {
       *tmpStr2 = '\0';
       strcat(NEWSLSQUERY1, tmpStr1);
       strcat(NEWSLSQUERY1, MDASSCHEMENAME);
       tmpStr1 = tmpStr2 + 30;
    }
    strcat(NEWSLSQUERY1,tmpStr1);  


    tmpStr2 = strstr(SLSQUERY2,"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    *tmpStr2 = '\0';
    strcat(SLSQUERY2,MDASSCHEMENAME);
    tmpStr1 = tmpStr2 + 30;
    while ((tmpStr2 = strstr(tmpStr1,"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$")) != NULL) {
       *tmpStr2 = '\0';
       strcat(SLSQUERY2, tmpStr1);
       strcat(SLSQUERY2, MDASSCHEMENAME);
       tmpStr1 = tmpStr2 + 30;
    }
    strcat(SLSQUERY2,tmpStr1);  

    tmpStr2 = strstr(NEWSLSQUERY2,"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
    *tmpStr2 = '\0';
    strcat(NEWSLSQUERY2,MDASSCHEMENAME);
    tmpStr1 = tmpStr2 + 30;
    while ((tmpStr2 = strstr(tmpStr1,"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$")) != NULL) {
       *tmpStr2 = '\0';
       strcat(NEWSLSQUERY2, tmpStr1);
       strcat(NEWSLSQUERY2, MDASSCHEMENAME);
       tmpStr1 = tmpStr2 + 30;
    }
    strcat(NEWSLSQUERY2,tmpStr1);  

  }

  if (strstr(query,SLSQUERY1) != NULL) {
    oldQ = strdup(query);
    tmpPtr = strstr(oldQ," WHERE ");
    if (tmpPtr != NULL) 
      sprintf(query, "%s%s", NEWSLSQUERY1,tmpPtr);
    free(oldQ);
  }
  if (strstr(query,SLSQUERY2) != NULL) {
    oldQ = strdup(query);
    tmpPtr = strstr(oldQ," WHERE ");
    if (tmpPtr != NULL) 
      sprintf(query, "%s%s", NEWSLSQUERY2,tmpPtr);
    free(oldQ);
  }

  return(0);

}
