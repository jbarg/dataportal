/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/****************************************************************************
** SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB **
** SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB **
** SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB **
** SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB SYB **
**
** Source File Name = sybDvrLib.c
**
**
** PURPOSE: 
**     Contains driver-level library functions to 
**     connect and operate with SYBASE databases. 
**
** CALLABLE FUNCTIONS:
**     open_env_sybTab
**     close_env_sybTab
**     connect_sybTab
**     disconnect_sybTab
**     exec_no_res_sql_sybTab
**     exec_sql_sybTab
**     done_with_result_sybTab
**     done_with_default_result_sybTab
**     bind_default_row_sybTab
**     bind_n_rows_sybTab
**     bind_column_sybTab
**     bind_file_to_cell_sybTab
**     bind_files_to_column_sybTab
**     get_row_sybTab
**     get_rows_sybTab
**     get_no_of_columns_sybTab
**     get_column_info_sybTab
**
** INTERNAL FUNCTIONS USED:
**     logmess_sybTab
**     print_error_sybTab
**     check_error_sybTab
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
#include "mdasC_syb_externs.h"
#include "mdasGlobalsExtern.h"
#include "srbErrno.h"
#include "elogExtern.h"
#include <ctpublic.h>
#include "sybDvrLib.h"

/***** DO THIS ONLY FOR Local Timing into db2LogFile
#define LOGTIMINGMCAT 1 
#define INSTRON 1
****/



FILE *PERFFILE;
FILE *LOGMESSFILE;
char logmess[HUGH_HUGE_STRING];
unsigned char bind_file_name[MAX_TOKEN];
short int  bind_file_name_length;
unsigned long  int  bind_file_option;
long int  bind_file_status;
char sybasesidbuffer[SMALL_TOKEN];
void logmess_sybTab(char *message, int rc);



/****************************************************************************
                                  FUNCTIONS                                  
****************************************************************************/

/******************* ERROR HANDLING ROUTINES - CALLBACKS *******************/
/***************************************************************************
  NAME   : serverErrorCallback
  PURPOSE: Client Library call back on error from server
****************************************************************************/
CS_RETCODE CS_PUBLIC
serverErrorCallback(CS_CONTEXT *cp, CS_CONNECTION *chp, CS_SERVERMSG *msgp)
{
#ifdef DEBUGON 
  sprintf(logmess,"Server Error: number(%ld) severity(%ld) state(%ld) line(%ld)\n",
                (long)msgp->msgnumber, (long)msgp->severity,
                (long)msgp->state, (long)msgp->line);
  elog(NOTICE, logmess);
  logmess_sybTab(logmess,2000);
  if (msgp->svrnlen > 0) {
    sprintf(logmess,"Server Error: Server name: %s\n", msgp->svrname);
    elog(NOTICE, logmess);
    logmess_sybTab(logmess,2000);
  }
  if (msgp->proclen > 0) {
    sprintf(logmess,"Server Error: Procedure name: %s\n", msgp->proc);
    elog(NOTICE, logmess);
    logmess_sybTab(logmess,2000);
  }
  sprintf(logmess,"Server Error:Message: %s\n", msgp->text);
  logmess_sybTab(logmess,2000);
#endif /** DEBUGON **/ 
  elog(NOTICE, "Server Error: Message: %s\n", msgp->text);
  return(CS_SUCCEED);
}

/***************************************************************************
  NAME   : clientLibErrorCallback
  PURPOSE: Client Library call back on error from Client Library
****************************************************************************/
CS_RETCODE CS_PUBLIC
clientLibErrorCallback(CS_CONTEXT *cp, CS_CONNECTION *chp, CS_CLIENTMSG *emsgp)
{
#ifdef DEBUGON 
  sprintf(logmess,"Client Library Error: severity(%ld) number(%ld) origin(%ld) layer(%ld)\n",
                (long)CS_SEVERITY(emsgp->severity),
                (long)CS_NUMBER(emsgp->msgnumber),
                (long)CS_ORIGIN(emsgp->msgnumber),
                (long)CS_LAYER(emsgp->msgnumber));
  elog(NOTICE, logmess);
  logmess_sybTab(logmess,2000);
  if (emsgp->osstringlen > 0) {
    sprintf(logmess,"Client Library Error: OS Error No: %ld :%s\n",
	 (long)emsgp->osnumber,emsgp->osstring);
    elog(NOTICE, logmess);
    logmess_sybTab(logmess,2000);
  }
  sprintf(logmess,"Client Library Error: Message: %s\n", emsgp->msgstring);
  logmess_sybTab(logmess,2000);
#endif /** DEBUGON **/ 
  elog(NOTICE, "Client Library Error: Message: %s\n", emsgp->msgstring);
  return(CS_SUCCEED);
}
/***************************************************************************
  NAME   : csLibErrorCallback
  PURPOSE: Client Library call back on error from CS-Library
****************************************************************************/
CS_RETCODE CS_PUBLIC
csLibErrorCallback(CS_CONTEXT *cp,  CS_CLIENTMSG *emsgp)
{
#ifdef DEBUGON 
  sprintf(logmess, "CS Library Error: severity(%ld) number(%ld) origin(%ld) layer(%ld)\n",
                (long)CS_SEVERITY(emsgp->severity),
                (long)CS_NUMBER(emsgp->msgnumber),
                (long)CS_ORIGIN(emsgp->msgnumber),
                (long)CS_LAYER(emsgp->msgnumber));
  elog(NOTICE,logmess);
  logmess_sybTab(logmess,2000);
  if (emsgp->osstringlen > 0) {
    sprintf(logmess,"CS Library Error: OS Error No: %ld :%s\n",
	 (long)emsgp->osnumber,emsgp->osstring);
    elog(NOTICE,  logmess);
    logmess_sybTab(logmess,2000);
    sprintf(logmess,"CS Library Error: Message: %s\n", emsgp->msgstring);
    logmess_sybTab(logmess,2000);
  }
#endif /** DEBUGON **/ 
  elog(NOTICE, "CS Library Error: Message: %s\n", emsgp->msgstring);
  return(CS_SUCCEED);
}

/***************************************************************************
  NAME   : logmess_sybTab
  PURPOSE: Logging message in LOGMESS file handle
****************************************************************************/

void logmess_sybTab(char *message, int rc)
{
#ifdef INSTRON
   hrtime_t highResTime;
#endif /* INSTRON */
#ifdef DEBUGON
   time_t *mmmmclk, mmmmcl;

   mmmmcl = time(NULL);
   fprintf(LOGMESSFILE,"SYB: %24.24s =>%4i  %s\n",ctime(&mmmmcl),rc, message);
   fflush(LOGMESSFILE);
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
  NAME   : open_env_sybTab
  PURPOSE: Opening an execution environment in sybTab 
****************************************************************************/ 


int open_env_sybTab(long int *inhenv)
{

    CS_CONTEXT         *context;  /* Context structure     */
    CS_RETCODE          ret;

#ifdef LOGTIMINGMCAT
    logmess_sybTab("Enter open_env_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
#ifdef DEBUGON
    logmess_sybTab("Enter open_env_sybTab",1000);
#endif /* DEBUGON */
#ifdef DEBUGON
      logmess_sybTab("cs_ctx_alloc", 1000);
#endif /* DEBUGON */
    context = (CS_CONTEXT *)NULL;
    ret = cs_ctx_alloc(EX_CTLIB_VERSION, &context);
    if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("open_env_sybTab: Error with cs_ctx_alloc",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
      logmess_sybTab("Leave open_env_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      return (SYB_CTX_MEMORY_ALLOCATION_ERROR);
    }
#ifdef DEBUGON
      logmess_sybTab("ct_init", 1000);
#endif /* DEBUGON */
    ret = ct_init(context, EX_CTLIB_VERSION);
    if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("open_env_sybTab: Error with ct_init",ret);
#endif /* DEBUGON */
      close_env_sybTab((long int) context);
#ifdef LOGTIMINGMCAT
      logmess_sybTab("Leave open_env_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      return (SYB_CT_INIT_ERROR);
    }
#ifdef DEBUGON
      logmess_sybTab("cs_config", 1000);
#endif /* DEBUGON */
    ret = cs_config(context, CS_SET, CS_MESSAGE_CB,
		    (CS_VOID *)csLibErrorCallback,
		    CS_UNUSED, NULL);
    if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("open_env_sybTab: Error with cs_config",ret);
#endif /* DEBUGON */
      close_env_sybTab((long int) context);
#ifdef LOGTIMINGMCAT
      logmess_sybTab("Leave open_env_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      return (SYB_ERROR_CALLBACK_INIT_ERROR);
    }
#ifdef DEBUGON
      logmess_sybTab("ct_callback", 1000);
#endif /* DEBUGON */
    ret = ct_callback(context, NULL, CS_SET, CS_CLIENTMSG_CB,
                          (CS_VOID *)clientLibErrorCallback);
    if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("open_env_sybTab: Error with ct_callback",ret);
#endif /* DEBUGON */
      close_env_sybTab((long int) context);
#ifdef LOGTIMINGMCAT
      logmess_sybTab("Leave open_env_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      return (SYB_ERROR_CALLBACK_INIT_ERROR);
    }
#ifdef DEBUGON
      logmess_sybTab("ct_callback", 1000);
#endif /* DEBUGON */
    ret = ct_callback(context, NULL, CS_SET, CS_SERVERMSG_CB,
                          (CS_VOID *)serverErrorCallback);
    if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("open_env_sybTab: Error with ct_callback",ret);
#endif /* DEBUGON */
      close_env_sybTab((long int) context);
#ifdef LOGTIMINGMCAT
      logmess_sybTab("Leave open_env_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      return (SYB_ERROR_CALLBACK_INIT_ERROR);
    }

    *inhenv = (long int) context;
    sybQuInfo = NULL;                /* initialize the pointer */
#ifdef DEBUGON
    logmess_sybTab("Leave open_env_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave open_env_sybTab",-2222);
#endif /* LOGTIMINGMCAT */

    return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : close_env_sybTab
  PURPOSE: Closing an open execution environment in sybTab 
****************************************************************************/
   
int close_env_sybTab(long int inhenv)
{

    CS_CONTEXT         *context;  /* Context structure     */

#ifdef DEBUGON
    logmess_sybTab("Enter close_env_sybTab",1000);
#endif /* DEBUGON */
    context = (CS_CONTEXT *) inhenv;
    if (context == NULL) 
      return(SQL_SUCCESS);
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Enter close_env_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
    context = (CS_CONTEXT *) inhenv;
#ifdef DEBUGON
    logmess_sybTab("ct_exit and cs_ctx_drop",1000);
#endif /* DEBUGON */
    ct_exit(context, CS_FORCE_EXIT);
    cs_ctx_drop(context);
    
#ifdef DEBUGON
    logmess_sybTab("Leave close_env_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave close_env_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
    return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : connect_sybTab
  PURPOSE: Opening a connection to sybTab server
****************************************************************************/

int connect_sybTab(long int inhenv,
		long int *inhdbc,
		SQLCHAR *databasename, 
		SQLCHAR *user, 
		SQLCHAR *password)
{
  SQLRETURN rc;

  CS_CONTEXT         *context;  /* Context structure     */
  CS_CONNECTION      *connection;
  CS_RETCODE          ret;
  CS_BOOL             trueParam;
  int i;
  trueParam = CS_TRUE;

  context = (CS_CONTEXT *) inhenv;

#ifdef LOGTIMINGMCAT
    logmess_sybTab("Enter connect_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
#ifdef DEBUGON
  logmess_sybTab("Enter connect_sybTab",-2222);
  sprintf(logmess, "connect_sybTab: Database = %s, User = %s", databasename, user);  
  logmess_sybTab(logmess, 1000);
#endif /* DEBUGON */

#ifdef DEBUGON
  logmess_sybTab("ct_con_alloc",1000);
#endif /* DEBUGON */
  ret = ct_con_alloc(context, &connection);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("connect_sybTab: Error with ct_con_alloc",ret);
#endif /* DEBUGON */
    close_env_sybTab((long int) context);
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave connect_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      return (SYB_CONNECT_MEMORY_ALLOCATION_ERROR);
  }

  *inhdbc = (long int) connection;
#ifdef DEBUGON
  logmess_sybTab("ct_con_props",1000);
#endif /* DEBUGON */
  ret = ct_con_props(connection, CS_SET, CS_USERNAME,
                           (CS_CHAR *) user, CS_NULLTERM, NULL);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("connect_sybTab: Error with ct_con_props",ret);
#endif /* DEBUGON */
    close_env_sybTab((long int) context);
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave connect_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      return (SYB_CONNECT_MEMORY_ALLOCATION_ERROR);
  }
#ifdef DEBUGON
  logmess_sybTab("ct_con_props",1000);
#endif /* DEBUGON */
        ret = ct_con_props(connection, CS_SET, CS_PASSWORD,
                           (CS_CHAR *) password, CS_NULLTERM, NULL);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("connect_sybTab: Error with ct_con_props",ret);
#endif /* DEBUGON */
    close_env_sybTab((long int) context);
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave connect_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      return (SYB_CONNECT_MEMORY_ALLOCATION_ERROR);
  }
#ifdef DEBUGON
  logmess_sybTab("ct_connect",1000);
#endif /* DEBUGON */
        ret = ct_connect(connection, (CS_CHAR *) databasename,CS_NULLTERM);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("connect_sybTab: Error with ct_connect",ret);
#endif /* DEBUGON */
    close_env_sybTab((long int) context);
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave connect_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      return (SYB_CONNECT_MEMORY_ALLOCATION_ERROR);
  }
  
  /* set properties and transaction levels */

#ifdef DEBUGON
  logmess_sybTab("ct_options",1000);
#endif /* DEBUGON */
  ret = ct_options(connection, CS_SET,CS_OPT_STR_RTRUNC, 
		   &trueParam, CS_UNUSED,NULL);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("connect_sybTab: Error with ct_options",ret);
#endif /* DEBUGON */
    close_env_sybTab((long int) context);
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave connect_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    return (SYB_CONNECT_MEMORY_ALLOCATION_ERROR);
  }
  
  /*** explicitly start a transaction ***/
#ifdef DEBUGON
  logmess_sybTab("executing begin transaction",1000);
#endif /* DEBUGON */
  i = exec_no_res_sql_sybTab((long int) context, (long int) connection,
			  (SQLCHAR *) "begin transaction" );
  if (i < 0) {
#ifdef DEBUGON
  logmess_sybTab("connect_sybTab: Error with executing begin transaction",i);
#endif /* DEBUGON */
  disconnect_sybTab((long int) context, (long int) connection,MDAS_CAT_ROLLBACK);
  close_env_sybTab((long int) context);
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave connect_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    return (i);
  }

#ifdef DEBUGON
    logmess_sybTab("Leave connect_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave connect_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
  return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : disconnect_sybTab
  PURPOSE: Closing an open connection  in sybTab 

****************************************************************************/
 
int disconnect_sybTab(long int inhenv,
		   long int inhdbc,
		   SQLUSMALLINT transstate)
{
  CS_CONTEXT         *context;  /* Context structure     */
  CS_CONNECTION      *connection;
  CS_RETCODE          ret;
  int i;


    context = (CS_CONTEXT *) inhenv;
    connection = (CS_CONNECTION *) inhdbc;

#ifdef DEBUGON
    logmess_sybTab("Enter disconnect_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Enter disconnect_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
#ifdef DEBUGON
    logmess_sybTab("exec_no_res_sql_sybTab",1000);
#endif /* DEBUGON */

    if (transstate == MDAS_CAT_ROLLBACK)
      i = exec_no_res_sql_sybTab((long int) context, (long int) connection, (SQLCHAR *) "rollback transaction" );
    else
      i = exec_no_res_sql_sybTab((long int) context, (long int) connection, (SQLCHAR *) "commit transaction" );
  if (i < 0) {
#ifdef DEBUGON
    sprintf(logmess,"disconnect_sybTab: Error with executing disconnect_sybTab: %i",transstate);
    logmess_sybTab(logmess,i);
#endif /* DEBUGON */
  }
#ifdef DEBUGON
    logmess_sybTab("ct_exit and cs_ctx_drop",1000);
#endif /* DEBUGON */
    ct_close(connection, CS_FORCE_CLOSE);
    ct_con_drop(connection);
#ifdef DEBUGON
    logmess_sybTab("Leave disconnect_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave disconnect_sybTab",-2222);
#endif /* LOGTIMINGMCAT */

  freeAllSybQuInfoPtrsybTab();
  return (i);
}

/***************************************************************************
  NAME   : exec_no_res_sql_sybTab
  PURPOSE: Execute a SQL query which has no resulting table, eg. INSERT,
           DELETE, UPDATE or DDL
****************************************************************************/
 
int exec_no_res_sql_sybTab(long int inhenv, 
			long int inhdbc, 
			SQLCHAR *query)
{

  CS_CONTEXT         *context;  /* Context structure     */
  CS_CONNECTION      *connection;
  CS_COMMAND         *cmd;/* Command structure.    */
  CS_RETCODE          ret;
  CS_INT          res_type;
  int i;
    time_t              tim, time();

     filterQuerysyb(query);
     if (showQuery) {
      time(&tim);
      printf("SQL:%d:%s: %s\n",getpid(),query,ctime(&tim)+4);
      fflush(stdout);
      strcpy(GENERATEDSQL,query)
     }

    context = (CS_CONTEXT *) inhenv;
    connection = (CS_CONNECTION *) inhdbc;

#ifdef LOGTIMINGMCAT
    logmess_sybTab("Enter exec_no_res_sql_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
#ifdef DEBUGON
    sprintf(logmess,"Enter exec_no_res_sql_sybTab with query:%s\n",query);
    logmess_sybTab(logmess,1000);
#endif  /* DEBUGON */

#ifdef DEBUGON
    logmess_sybTab("ct_cmd_alloc", 1000);
#endif /* DEBUGON */
    ret = ct_cmd_alloc(connection, &cmd);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("exec_no_res_sql_sybTab: Error with ct_cmd_alloc",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave exec_no_res_sql_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    return (SYB_COMMAND_MEMORY_ALLOCATION_ERROR);
  }


#ifdef DEBUGON
  logmess_sybTab("ct_command", 1000);
#endif /* DEBUGON */
  ret = ct_command(cmd, CS_LANG_CMD,
		     (char *)  query,CS_NULLTERM, CS_UNUSED);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("exec_no_res_sql_sybTab: Error with ct_commmand",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave exec_no_res_sql_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    ct_cancel(NULL,cmd,CS_CANCEL_ALL);ct_cmd_drop(cmd);
    return (SYB_INITIATE_COMMAND_ERROR);
  }

#ifdef DEBUGON
  logmess_sybTab("ct_send", 1000);
#endif /* DEBUGON */
  ret = ct_send(cmd);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("exec_no_res_sql_sybTab: Error with ct_send",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave exec_no_res_sql_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    ct_cancel(NULL,cmd,CS_CANCEL_ALL);ct_cmd_drop(cmd);
    return (SYB_SEND_COMMAND_ERROR);
  }

#ifdef DEBUGON
  logmess_sybTab("ct_results", 1000);
#endif /* DEBUGON */

  while ((ret = ct_results(cmd, &res_type))== CS_SUCCEED) {
    if (res_type != CS_CMD_SUCCEED && res_type != CS_CMD_DONE &&
	res_type != CS_ROW_RESULT) {
#ifdef DEBUGON    
      sprintf(logmess,"exec_no_res_sql_sybTab: Error with ct_results Result-Type=%i",(int) res_type);
    logmess_sybTab(logmess,res_type);
#endif 
#ifdef LOGTIMINGMCAT
      logmess_sybTab("Leave exec_no_res_sql_sybTab",-1111);
#endif 
      ct_cancel(NULL,cmd,CS_CANCEL_ALL);ct_cmd_drop(cmd);
      return (SYB_COMMAND_RESULT_TYPE_ERROR);
    }
    
  }
  /****
  ret = ct_results(cmd, &res_type);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("exec_no_res_sql_sybTab: Error with ct_results",ret);
#endif 
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave exec_no_res_sql_sybTab",-1111);
#endif 
    ct_cancel(NULL,cmd,CS_CANCEL_ALL);ct_cmd_drop(cmd);
    return (SYB_COMMAND_RESULTS_ERROR);
  }
  if (res_type != CS_CMD_SUCCEED) {
#ifdef DEBUGON
    sprintf(logmess,"exec_no_res_sql_sybTab: Error with ct_results Result-Type=%i",(int) res_type);
    logmess_sybTab(logmess,res_type);  
#endif 
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave exec_no_res_sql_sybTab",-1111);
#endif 
    ct_cancel(NULL,cmd,CS_CANCEL_ALL);ct_cmd_drop(cmd);
    return (SYB_COMMAND_RESULT_TYPE_ERROR);
  }

****/

#ifdef DEBUGON
  logmess_sybTab("ct_cmd_drop", 1000);
#endif /* DEBUGON */
  ct_cancel(NULL,cmd,CS_CANCEL_ALL);ret = ct_cmd_drop(cmd);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("exec_no_res_sql_sybTab: Error with ct_cmd_drop",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave exec_no_res_sql_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    return (SYB_SEND_COMMAND_ERROR);
  }

#ifdef DEBUGON
    logmess_sybTab("Leave exec_no_res_sql_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave exec_no_res_sql_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
     return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : exec_sql_sybTab
  PURPOSE: Execute a SQL query which mave have  resulting table
****************************************************************************/

int exec_sql_sybTab(long int inhenv, 
		 long int inhdbc,
		 long int *inhstmt,
		 SQLCHAR *query)
{

  CS_CONTEXT         *context;  /* Context structure     */
  CS_CONNECTION      *connection;
  CS_COMMAND         *cmd;/* Command structure.    */
  CS_RETCODE          ret;
  CS_INT          res_type;
  int i;
    time_t              tim, time();

     filterQuerysyb(query);
     if (showQuery) {
      time(&tim);
      printf("SQL:%d:%s: %s\n",getpid(),query,ctime(&tim)+4);
      fflush(stdout);
      strcpy(GENERATEDSQL,query)
     }

  context = (CS_CONTEXT *) inhenv;
  connection = (CS_CONNECTION *) inhdbc;

#ifdef LOGTIMINGMCAT
    logmess_sybTab("Enter exec_sql_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
#ifdef DEBUGON
    sprintf(logmess,"Enter exec_sql_sybTab with query:%s\n",query);
    logmess_sybTab(logmess,1000);
#endif  /* DEBUGON */

#ifdef DEBUGON
    logmess_sybTab("ct_cmd_alloc", 1000);
#endif /* DEBUGON */
    ret = ct_cmd_alloc(connection, &cmd);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("exec_sql_sybTab: Error with ct_cmd_alloc",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave exec_sql_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    return (SYB_COMMAND_MEMORY_ALLOCATION_ERROR);
  }

  *inhstmt = (long int) cmd;

#ifdef DEBUGON
  logmess_sybTab("ct_command", 1000);
#endif /* DEBUGON */
  ret = ct_command(cmd, CS_LANG_CMD,
		     (char *)query,CS_NULLTERM, CS_UNUSED);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("exec_sql_sybTab: Error with ct_commmand",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave exec_sql_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    ct_cancel(NULL,cmd,CS_CANCEL_ALL);ct_cmd_drop(cmd);
    return (SYB_INITIATE_COMMAND_ERROR);
  }

#ifdef DEBUGON
  logmess_sybTab("ct_send", 1000);
#endif /* DEBUGON */
  ret = ct_send(cmd);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("exec_sql_sybTab: Error with ct_send",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave exec_sql_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    ct_cancel(NULL,cmd,CS_CANCEL_ALL);ct_cmd_drop(cmd);
    return (SYB_SEND_COMMAND_ERROR);
  }

#ifdef DEBUGON
  logmess_sybTab("ct_results", 1000);
#endif /* DEBUGON */
  ret = ct_results(cmd, &res_type);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("exec_sql_sybTab: Error with ct_results",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave exec_sql_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    ct_cancel(NULL,cmd,CS_CANCEL_ALL);ct_cmd_drop(cmd);
    return (SYB_COMMAND_RESULTS_ERROR);
  }
  if (res_type != CS_CMD_SUCCEED && res_type != CS_CMD_DONE &&
	res_type != CS_ROW_RESULT && res_type != CS_COMPUTE_RESULT ) {
#ifdef DEBUGON
    sprintf(logmess,"exec_sql_sybTab: Error with ct_results Result-Type=%d",(int) res_type);
    logmess_sybTab(logmess,res_type);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave exec_sql_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    ct_cancel(NULL,cmd,CS_CANCEL_ALL);ct_cmd_drop(cmd);
    return (SYB_COMMAND_RESULT_TYPE_ERROR);
  }
  i = allocSybQuInfoPtrsybTab((long int) cmd);
#ifdef DEBUGON
  logmess_sybTab("Leave exec_sql_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
  logmess_sybTab("Leave exec_sql_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
  if (i < 0)
    return(i);
  return(SQL_SUCCESS);
}


/***************************************************************************
  NAME   : done_with_result_sybTab
  PURPOSE: Close a stament environment opened by exec_sql_sybTab() but which
           used user allocated result structure
****************************************************************************/

int done_with_result_sybTab(long int inhstmt)
{  
  CS_COMMAND         *cmd;/* Command structure.    */
  CS_RETCODE          ret;
  int i;

  cmd = (CS_COMMAND *) inhstmt;

#ifdef DEBUGON
   logmess_sybTab("Enter done_with_result_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Enter done_with_result_sybTab",-2222);
#endif /* LOGTIMINGMCAT */

#ifdef DEBUGON
  logmess_sybTab("ct_cmd_drop", 1000);
#endif /* DEBUGON */
  ct_cancel(NULL,cmd,CS_CANCEL_ALL);ret = ct_cmd_drop(cmd);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("done_with_result_sybTab: Error with ct_cmd_drop",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave done_with_result_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    return (SYB_SEND_COMMAND_ERROR);
  }

#ifdef DEBUGON
    logmess_sybTab("Leave done_with_result_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave done_with_result_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : done_with_default_result_sybTab
  PURPOSE: Close a stament environment opened by exec_sql_sybTab() which used
           result structure build by bind_default_row_sybTab()
****************************************************************************/
 
int done_with_default_result_sybTab(long int inhstmt,
				  SQLCHAR *data[],
				  SQLINTEGER data_size[],
				  SQLSMALLINT num_of_cols)
{  

  CS_COMMAND         *cmd;/* Command structure.    */
  CS_RETCODE          ret;
  int i;


  cmd = (CS_COMMAND *) inhstmt;

#ifdef DEBUGON
  logmess_sybTab("Enter done_with_result_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
  logmess_sybTab("Enter done_with_result_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
  for (i =  0; i < num_of_cols; i++){
    free(data[i]);
  }

#ifdef DEBUGON
  logmess_sybTab("ct_cmd_drop", 1000);
#endif /* DEBUGON */
  ct_cancel(NULL,cmd,CS_CANCEL_ALL);ret = ct_cmd_drop(cmd);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("done_with_default_result_sybTab: Error with ct_cmd_drop",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave done_with_default_result_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    return (SYB_SEND_COMMAND_ERROR);
  }
  
  freeSybQuInfoPtrsybTab(inhstmt);

#ifdef DEBUGON
    logmess_sybTab("Leave done_with_default_result_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave done_with_default_result_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : bind_default_row_sybTab
  PURPOSE: Allocate a default set of result holders to hold one row of 
           result of a SQL statement. 
****************************************************************************/
 
int bind_default_row_sybTab(long int inhstmt,
		  SQLCHAR *data[],
		  SQLINTEGER data_size[], 
		  SQLSMALLINT *num_of_cols)
{
  CS_COMMAND         *cmd;/* Command structure.    */
  CS_RETCODE          ret;
  CS_INT              innumcols;
  CS_DATAFMT          datafmt;
  int i;
  CS_INT              col,j;

  /**** note: num_of_cols coming in may be wrong ***/
  cmd = (CS_COMMAND *) inhstmt;

#ifdef DEBUGON
  logmess_sybTab("Enter bind_default_row__sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
  logmess_sybTab("Enter bind_default_row_sybTab",-2222);
#endif /* LOGTIMINGMCAT */

#ifdef DEBUGON
  logmess_sybTab("ct_res_info", 1000);
#endif /* DEBUGON */
  ret = ct_res_info(cmd, CS_NUMDATA, &innumcols, CS_UNUSED, NULL);
  if (ret != CS_SUCCEED) {
#ifdef DEBUGON
  logmess_sybTab("bind_default_row_sybTab: Error with ct_res_info",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave done_with_default_result_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    return (SYB_RES_INFO_ERROR);
  }
  if (innumcols <= 0){
#ifdef DEBUGON
  logmess_sybTab("bind_default_row_sybTab: Error got zero or less columns",innumcols);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave done_with_default_result_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    return (SYB_NUM_RESULT_ERROR);
  }
  *num_of_cols = innumcols;

  for (col = 1; col <= innumcols ; col++)     {
#ifdef DEBUGON
    logmess_sybTab("ct_describe", 1000);
#endif /* DEBUGON */
    ret = ct_describe(cmd, col, &datafmt);
    if (ret != CS_SUCCEED) {
#ifdef DEBUGON
      logmess_sybTab("bind_default_row_sybTab: Error with ct_describe",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
      logmess_sybTab("Leave done_with_default_result_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      for (j = 0; j < col-1 ; j++) free(data[j]);
      return (SYB_SELECT_DESCRIBE_ERROR);
    }
    datafmt.datatype  = CS_CHAR_TYPE;
    datafmt.format    = CS_FMT_NULLTERM;
    datafmt.maxlength = MAX(datafmt.maxlength + 1,MAX_TOKEN);
    data[col-1] = (SQLCHAR *) malloc((int)datafmt.maxlength);
    if (data[col-1] == NULL) {
#ifdef DEBUGON
      logmess_sybTab("bind_default_row_sybTab: Data Memory Allocation Error",-9999);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
      logmess_sybTab("Leave done_with_default_result_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      for (j = 0; j < col-1 ; j++) free(data[j]);
      return (SYB_DATA_MEMORY_ALLOCATION_ERROR);
    }
    strcpy((char *)data[col-1],"");
#ifdef DEBUGON
    logmess_sybTab("ct_bind", 1000);
#endif /* DEBUGON */
    ret = ct_bind(cmd, col, &datafmt, data[col-1],
		      (CS_INT *) &data_size[col-1] , NULL);
    if (ret != CS_SUCCEED) {
#ifdef DEBUGON
      logmess_sybTab("bind_default_row_sybTab: Error with ct_bind",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
      logmess_sybTab("Leave done_with_default_result_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      for (j = 0; j < col ; j++) free(data[j]);
      return (SYB_SELECT_DESCRIBE_ERROR);
    }
  }

  ret =  modSybQuInfoPtrsybTab(inhstmt, (void *) data, (void *) data_size);
  if (ret < 0) {
#ifdef LOGTIMINGMCAT
      logmess_sybTab("Leave done_with_default_result_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      for (j = 0; j < col ; j++) free(data[j]);
      return (ret);
  }
#ifdef DEBUGON
  logmess_sybTab("Leave bind_default_row_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
  logmess_sybTab("Leave bind_default_row_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
  return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : get_row_sybTab
  PURPOSE: Fetch the next (unfetched)  row of result in the 
           row-holder bound by any of the bind_***_sybTab() calls. To fetch
           multiple row use get_rows_sybTab()
****************************************************************************/

int get_row_sybTab(long int inhstmt)
{
  CS_COMMAND         *cmd; /* Command structure.    */
  CS_RETCODE          ret;
  CS_INT                  rows_read;
  SQLCHAR *data[];
  SQLINTEGER data_size[];
  CS_INT          res_type;
  SQLSMALLINT  num_of_cols;

  cmd = (CS_COMMAND *) inhstmt;

#ifdef DEBUGON
  logmess_sybTab("Enter get_row_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Enter get_row_sybTab",-2222);
#endif /* LOGTIMINGMCAT */


#ifdef DEBUGON
  logmess_sybTab("ct_fetch", 1000);
#endif /* DEBUGON */


  ret = ct_fetch(cmd, CS_UNUSED, CS_UNUSED, CS_UNUSED,
                        &rows_read);
  if (ret != CS_SUCCEED  && ret != CS_ROW_FAIL ) {
    if (ret == CS_END_DATA) { /*** we got  CS_END_DATA ***/
#ifdef DEBUGON
  logmess_sybTab("get_row_sybTab: End Data for ct_fetch",ret);
#endif /* DEBUGON */

AA: 
#ifdef DEBUGON
  logmess_sybTab("ct_results", 1000);
#endif /* DEBUGON */
  ret = ct_results(cmd, &res_type);
  if (ret != CS_SUCCEED) {
    if (ret != CS_END_RESULTS) {
#ifdef DEBUGON
  logmess_sybTab("get_row_sybTab: Error with ct_results",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave get_row_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    ct_cancel(NULL,cmd,CS_CANCEL_ALL);ct_cmd_drop(cmd);
    return (SYB_COMMAND_RESULTS_ERROR);
    }
    else {
#ifdef DEBUGON
  logmess_sybTab("get_row_sybTab: Finished with ct_results",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave get_row_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    return(MCAT_NO_DATA_FOUND);
    }
  }
  if (res_type != CS_CMD_SUCCEED && res_type != CS_CMD_DONE &&
	res_type != CS_ROW_RESULT && res_type != CS_COMPUTE_RESULT ) {
#ifdef DEBUGON
    sprintf(logmess,"get_row_sybTab: Error with ct_results Result-Type=%d",(int) res_type);
    logmess_sybTab(logmess,res_type);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave get_row_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    ct_cancel(NULL,cmd,CS_CANCEL_ALL);ct_cmd_drop(cmd);
    return (SYB_COMMAND_RESULT_TYPE_ERROR);
  }
  if (res_type == CS_CMD_SUCCEED || res_type == CS_CMD_DONE) {
    goto AA;   
  }

   i = getSybQuInfoPtrsybTab(inhstmt, &data, &data_size);
   if (i < 0) 
     return(i);
  i = bind_default_row_sybTab(inhstmt,data,data_size, &num_of_cols);
#ifdef DEBUGON
  logmess_sybTab("Calling Recursively get_row_sybTab", 1000);
#endif /* DEBUGON */
   i = get_row_sybTab(inhstmt);
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave get_row_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
  return(i);
    } /*** we got  CS_END_DATA ***/
    else {
#ifdef DEBUGON
  logmess_sybTab("get_row_sybTab: Error with ct_fetch",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave done_with_default_result_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
    return (MCAT_FETCH_ERROR);
    }
  }
  
#ifdef DEBUGON
  logmess_sybTab("Leave get_row_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave get_row_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
   return(SQL_SUCCESS);
}

/***************************************************************************
  NAME   : get_no_of_columns_sybTab
  PURPOSE: Find the number of columns of the result table of an
           exec_sql_sybTab statement. 
****************************************************************************/
  
int get_no_of_columns_sybTab(long int inhstmt,
                        SQLSMALLINT *num_of_cols)
{

  /**** Added Aug 24,2000 for speed up */
   *num_of_cols =  1;
   return(MDAS_SUCCESS);

}
/***************************************************************************
  NAME   : get_column_info_sybTab
  PURPOSE: Get assorted column attributes  such as column name, size (as 
           defined in DDL), type, nullability, precision and its scale.
****************************************************************************/
 
int get_column_info_sybTab(long int inhstmt,
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

  CS_COMMAND         *cmd;/* Command structure.    */
  CS_RETCODE          ret;
  CS_INT              innumcols;
  CS_DATAFMT          datafmt;
  int i;
  CS_INT              col,j;

  cmd = (CS_COMMAND *) inhstmt;



#ifdef DEBUGON
  logmess_sybTab("Enter get_column_info_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Enter get_column_info_sybTab",-2222);
#endif /* LOGTIMINGMCAT */

#ifdef DEBUGON
    logmess_sybTab("ct_describe", 1000);
#endif /* DEBUGON */
    ret = ct_describe(cmd, column_number, &datafmt);
    if (ret != CS_SUCCEED) {
#ifdef DEBUGON
      logmess_sybTab("get_column_info_sybTab: Error with ct_describe",ret);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
      logmess_sybTab("Leave get_column_info_sybTab",-1111);
#endif /* LOGTIMINGMCAT */
      return (SYB_SELECT_DESCRIBE_ERROR);
    }
    *coltype = (SQLSMALLINT) datafmt.datatype;
    *colnamelen = (SQLSMALLINT) datafmt.namelen;
    if (datafmt.namelen >= 0) 
      strcpy((char *)colname, (char *) datafmt.name);
    else
      strcpy((char *)colname,"");
    *displaysize = (SQLINTEGER) datafmt.maxlength;
    *precision = (SQLUINTEGER) datafmt.precision;
    *scale = (SQLSMALLINT) datafmt.scale;
    if (datafmt.status & CS_CANBENULL)
      *nullable =  (SQLSMALLINT) 1;
    else
      *nullable =  (SQLSMALLINT) 0;

#ifdef DEBUGON
  logmess_sybTab("Leave get_column_info_sybTab",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave get_column_info_sybTab",-2222);
#endif /* LOGTIMINGMCAT */
  return(SQL_SUCCESS);
}



 
close_sybTab_interaction_internal(long int  henv, 
		  long int  hdbc, int transaction_end_code)

{ 
  MDAS_DB2_SERVER_HNDL *tmp_srvh;
  
#ifdef DEBUGON
  logmess_sybTab("Enter close_sybTab_interaction_internal",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Enter close_sybTab_interaction_internal",-2222);
#endif /* LOGTIMINGMCAT */
  if (henv < 0)
    {
#ifdef DEBUGON
      logmess_sybTab("close_sybTab_interaction_internal: Error with henv value",henv);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave close_sybTab_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
	return(MCAT_CLOSE_ENV_ERROR);
    }
  /* transaction_end_code = 1 ABORT TRANSACTION */
  /* transaction_end_code = 0 COMMIT TRANSACTION */
#ifdef DEBUGON
      logmess_sybTab("disconnect_sybTab",1000);
#endif /* DEBUGON */
  i =  disconnect_sybTab( henv, hdbc, transaction_end_code); 
  if (i != 0) {
#ifdef DEBUGON
      logmess_sybTab("close_sybTab_interaction_internal: Error with disconnect_sybTab",i);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave close_sybTab_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
    return(MCAT_DISCONNECT_ERROR);
  }
#ifdef DEBUGON
      logmess_sybTab("close_env_sybTab",1000);
#endif /* DEBUGON */
  i =  close_env_sybTab(henv);
  if (i != 0) {
#ifdef DEBUGON
      logmess_sybTab("close_sybTab_interaction_internal: Error with close_env_sybTab",i);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave close_sybTab_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
    return(MCAT_CLOSE_ENV_ERROR);
  }
#ifdef DEBUGON
  logmess_sybTab("Leave close_sybTab_interaction_internal",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave close_sybTab_interaction_internal",-2222);
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

int open_sybTab_interaction_internal(long int  *henv, 
		  long int  *hdbc)
{

 MDAS_DB2_SERVER_HNDL *tmp_srvh;


  i= initMdasEnviron();
  if (i != 0) {
#ifdef DEBUGON
      logmess_sybTab("open_sybTab_interaction_internal: Error with initMdasEnviron",i);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave open_sybTab_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
    return (i);
  }
#ifdef DEBUGON 
  LOGMESSFILE = fopen(SYBTABLOGFILE,"a");
  if (LOGMESSFILE == NULL)
    {fprintf(stderr,"Cannot open log file:%s\n",SYBTABLOGFILE);
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
    sprintf(logmess,"%s.%ld",SYBTABLOGFILE, getpid());
    PERFFILE = fopen(logmess,"a");
    if (PERFFILE == NULL)
    {fprintf(stderr,"Cannot open performance file:%s\n",logmess);
    return (INIT_ENVIRON_ERROR);
    }
#endif /* INSTRON */
#endif /* LOGTIMINGMCAT */
#ifdef DEBUGON
  logmess_sybTab("Enter open_sybTab_interaction_internal",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Enter open_sybTab_interaction_internal",-2222);
#endif /* LOGTIMINGMCAT */

#ifdef DEBUGON
      logmess_sybTab("open_env_sybTab",1000);
#endif /* DEBUGON */

  i =  open_env_sybTab(henv);
  if (i != 0)
    {
#ifdef DEBUGON
      logmess_sybTab("open_sybTab_interaction_internal: Error with open_env_sybTab",i);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave open_sybTab_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
#ifdef INSMESSON 
    fclose(INSMESSFILE);
#endif /* INSMESSON */ 
#ifdef DEBUGON 
    fclose(LOGMESSFILE); 
#endif /* DEBUGON */ 
     return(MCAT_OPEN_ENV_ERROR);
    }
#ifdef DEBUGON
      logmess_sybTab("connect_sybTab",1000);
#endif /* DEBUGON */
  i =  connect_sybTab( *henv, hdbc, (unsigned char*) server, 
            (unsigned char*) user, (unsigned char*) pass); 
  if (i != 0){
#ifdef DEBUGON
      logmess_sybTab("open_sybTab_interaction_internal: Error with connect_sybTab",i);
#endif /* DEBUGON */
    close_env_sybTab(*henv); 
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave open_sybTab_interaction_internal",-1111);
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
    logmess_sybTab("Leave open_sybTab_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
    return(MEMORY_ALLOCATION_ERROR);
      }
    strcpy(tmp_srvh->user, user);
    strcpy(tmp_srvh->server,catser);
    tmp_srvh->valid = 1;
    tmp_srvh->henv  = *henv;
    tmp_srvh->hdbc  = *hdbc;
    serverhndl[0].conn = tmp_srvh;
#ifdef DEBUGON
      logmess_sybTab("Leave open_sybTab_interaction_internal",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave open_sybTab_interaction_internal",-2222);
#endif /* LOGTIMINGMCAT */

  return(MDAS_SUCCESS);
}

int commit_sybTab_interaction_internal(long int inhenv, 
		  long int  inhdbc, int  transaction_end_code)
{
  CS_CONTEXT         *context;  /* Context structure     */
  CS_CONNECTION      *connection;
  CS_RETCODE          ret;
  int i;

  context = (CS_CONTEXT *) inhenv;
  connection = (CS_CONNECTION *) inhdbc;

#ifdef DEBUGON
  logmess_sybTab("Enter commit_sybTab_interaction_internal",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
  logmess_sybTab("Enter commit_sybTab_interaction_internal",-2222);
#endif /* LOGTIMINGMCAT */
 
#ifdef DEBUGON
    logmess_sybTab("Before SQLTransact",transaction_end_code);
#endif /* DEBUGON */
    if (transaction_end_code == MDAS_CAT_ROLLBACK)
      i = exec_no_res_sql_sybTab((long int) context, (long int) connection, (SQLCHAR *) "rollback transaction" );
    else
      i = exec_no_res_sql_sybTab((long int) context, (long int) connection, (SQLCHAR *) "commit transaction" );
  if (i < 0) {
#ifdef DEBUGON
    sprintf(logmess,"commit_sybTab_interaction_internal: Error with executing SQLTransact: %i",transaction_end_code);
    logmess_sybTab(logmess,i);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave commit_sybTab_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
    return(MCAT_TRANSACT_CLOSE_ERROR);
  }
#ifdef DEBUGON
   logmess_sybTab("After Successful SQLTransact",transaction_end_code);
#endif /* DEBUGON */

#ifdef DEBUGON
  logmess_sybTab("commit_sybTab_interaction_internal: executing begin transaction",1000);
#endif /* DEBUGON */
  i = exec_no_res_sql_sybTab((long int) context, (long int) connection,
			  (SQLCHAR *) "begin transaction" );
  if (i < 0) {
#ifdef DEBUGON
  logmess_sybTab("commit_sybTab_interaction_internal: Error with executing begin transaction",i);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave commit_sybTab_interaction_internal",-1111);
#endif /* LOGTIMINGMCAT */
    return (i);
  }

#ifdef DEBUGON
   logmess_sybTab("Leave commit_sybTab_interaction_internal",1000);
#endif /* DEBUGON */
#ifdef LOGTIMINGMCAT
    logmess_sybTab("Leave commit_sybTab_interaction_internal",-2222);
#endif /* LOGTIMINGMCAT */
  return(MDAS_SUCCESS);
}


int 
freeAllSybQuInfoPtrsybTab() 
{
  sybase_internal_info_ptr tmpSybQuInfoPtr1, tmpSybQuInfoPtr2;
  tmpSybQuInfoPtr1 = sybQuInfo;
#ifdef DEBUGON
   logmess_sybTab("freeAllSybQuInfoPtrsybTab",1000);
#endif /* DEBUGON */

  while (tmpSybQuInfoPtr1 != NULL) {
    tmpSybQuInfoPtr2 = tmpSybQuInfoPtr1->next;
    free(tmpSybQuInfoPtr1);
    tmpSybQuInfoPtr1 = tmpSybQuInfoPtr2;
  }
  return 0;
}

int 
freeSybQuInfoPtrsybTab(long int inhstmt)
{

  sybase_internal_info_ptr tmpSybQuInfoPtr1;
  sybase_internal_info_ptr *tmpSybQuInfoPtr2;
#ifdef DEBUGON
   sprintf(logmess,"freeSybQuInfoPtrsybTab: %ld",inhstmt);
   logmess_sybTab(logmess,3333);
#endif /* DEBUGON */
  tmpSybQuInfoPtr2 = &sybQuInfo;
  tmpSybQuInfoPtr1 = sybQuInfo;
  while (tmpSybQuInfoPtr1 != NULL) {
    if (tmpSybQuInfoPtr1->hstmt ==  inhstmt) {
      *tmpSybQuInfoPtr2 = tmpSybQuInfoPtr1->next;
      free(tmpSybQuInfoPtr1);
      return 0;
    }
    tmpSybQuInfoPtr2 = &tmpSybQuInfoPtr1;
    tmpSybQuInfoPtr1 = tmpSybQuInfoPtr1->next;
  }
  elog(NOTICE, "freeSybQuInfoPtrsybTab: Error in Freeing Memory: Query Pointer Not in List");
  return -1;
  /**

  sybase_internal_info_ptr tmpSybQuInfoPtr1, tmpSybQuInfoPtr2;
#ifdef DEBUGON
   sprintf(logmess,"freeSybQuInfoPtrsybTab: %ld",inhstmt);
   logmess_sybTab(logmess,3333);
#endif 
  tmpSybQuInfoPtr2 = sybQuInfo;
  tmpSybQuInfoPtr1 = sybQuInfo;
  while (tmpSybQuInfoPtr1 != NULL) {
    if (tmpSybQuInfoPtr1->hstmt ==  inhstmt) {
      tmpSybQuInfoPtr2 = tmpSybQuInfoPtr1->next;
      free(tmpSybQuInfoPtr1);
      return 0;
    }
    tmpSybQuInfoPtr2 = tmpSybQuInfoPtr1;
    tmpSybQuInfoPtr1 = tmpSybQuInfoPtr1->next;
  }
  elog(NOTICE, "freeSybQuInfoPtrsybTab: Error in Freeing Memory: Query Pointer Not in List");
  return -1;
**/

}

int 
allocSybQuInfoPtrsybTab(long int inhstmt)
{
  sybase_internal_info_ptr tmpSybQuInfoPtr1, tmpSybQuInfoPtr2;

#ifdef DEBUGON
   sprintf(logmess,"allocSybQuInfoPtrsybTab: %ld",inhstmt);
   logmess_sybTab(logmess,3333);
#endif /* DEBUGON */
  tmpSybQuInfoPtr1 = malloc (sizeof(sybase_internal_info));
  if (tmpSybQuInfoPtr1 == NULL) {
    elog(NOTICE, "allocSybQuInfoPtrsybTab: Error in Allocating Memory");
    return (MEMORY_ALLOCATION_ERROR);
  }
  tmpSybQuInfoPtr1->next = sybQuInfo;
  tmpSybQuInfoPtr1->hstmt = inhstmt;
  sybQuInfo = tmpSybQuInfoPtr1;
#ifdef DEBUGON
   sprintf(logmess,"allocSybQuInfoPtrsybTab: sybQuInfo = %ld, inhstmt= %ld,  tmpSybQuInfoPtr1->hstmt = %ld", 
	   (long int) sybQuInfo, inhstmt, tmpSybQuInfoPtr1->hstmt);
   logmess_sybTab(logmess,3333);
#endif /* DEBUGON */
  return (0);   
}

int 
modSybQuInfoPtrsybTab(long int inhstmt, void *data,
		  void *data_size)
{
  sybase_internal_info_ptr tmpSybQuInfoPtr1;

#ifdef DEBUGON
   sprintf(logmess,"modSybQuInfoPtrsybTab:  sybQuInfo = %ld, inhstmt= %ld ",(long int) sybQuInfo, inhstmt);
   logmess_sybTab(logmess,3333);
#endif /* DEBUGON */
  tmpSybQuInfoPtr1 = sybQuInfo;
  while (tmpSybQuInfoPtr1 != NULL) {
#ifdef DEBUGON
sprintf(logmess,"tmpSybQuInfoPtr1->hstmt = %ld inhstmt = %ld",
	 tmpSybQuInfoPtr1->hstmt, inhstmt);
#endif /* DEBUGON */
    if (tmpSybQuInfoPtr1->hstmt ==  inhstmt) {
      tmpSybQuInfoPtr1->data = data;
      tmpSybQuInfoPtr1->data_size = data_size;
#ifdef DEBUGON
   sprintf(logmess,"modSybQuInfoPtrsybTab: data=%ld", (long int) data);
   logmess_sybTab(logmess,3333);
#endif /* DEBUGON */
      return 0;
    }
    tmpSybQuInfoPtr1 = tmpSybQuInfoPtr1->next;
  }
#ifdef DEBUGON
   sprintf(logmess,"Error for modSybQuInfoPtrsybTab: stmt=%ld",inhstmt);
   logmess_sybTab(logmess,3333);
#endif /* DEBUGON */
  elog(NOTICE, "modSybQuInfoPtrsybTab: Error in Modifying QueryList for %ld", 
       inhstmt);
  return (MODIFY_QUERY_LIST_ERROR);
}

int
getSybQuInfoPtrsybTab(long int inhstmt, void **data,
		  void **data_size)
{
  sybase_internal_info_ptr tmpSybQuInfoPtr1;

#ifdef DEBUGON
   logmess_sybTab("getSybQuInfoPtrsybTab: %ld",inhstmt);
#endif /* DEBUGON */
  tmpSybQuInfoPtr1 = sybQuInfo;
  while (tmpSybQuInfoPtr1 != NULL) {
    if (tmpSybQuInfoPtr1->hstmt ==  inhstmt) {
      *data = tmpSybQuInfoPtr1->data;
      *data_size = tmpSybQuInfoPtr1->data_size;
#ifdef DEBUGON
   sprintf(logmess,"getSybQuInfoPtrsybTab: data=%ld", (long int) *data);
   logmess_sybTab(logmess,3333);
#endif /* DEBUGON */

      return 0;
    }
    tmpSybQuInfoPtr1 = tmpSybQuInfoPtr1->next;
  }
#ifdef DEBUGON
   sprintf(logmess,"Error for getSybQuInfoPtrsybTab: stmt=%ld",inhstmt);
   logmess_sybTab(logmess,3333);
#endif /* DEBUGON */
  elog(NOTICE, "getSybQuInfoPtrsybTab: Error in Gettinging QueryList for %ld", 
       inhstmt);
  return (GET_QUERY_LIST_ERROR);
}


filterQuerysyb(char *query)
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



/*********************  added  to the original mdasC_syb_intenal.c 
     after changing db2->syb 
     and filterQuery to filetrQuerysyb 

     also changed  SybQuInfoPtr( to SybQuInfoPtrsybTab(
           and     SybQuInfoPtr: to SybQuInfoPtrsybTab:
  *****************/

/***************************************************************************
  NAME   : getDataTypeFromMap_sybTab
  PURPOSE: Getting Column DataType Name from Data Type Index given by Informix
****************************************************************************/ 
void
getDataTypeFromMap_sybTab(char *typeName, short int   typeId)
{
  int jj;
        for (jj = 0; jj < MAXDATATYPELISTSIZE ; jj++) {
          if (sybDataTypeCode[jj] == typeId) {
          strcpy(typeName, sybDataTypeName[jj]);
          break;
        }
      }
      if (jj == MAXDATATYPELISTSIZE) 
        strcpy(typeName, sybDataTypeName[0]);
}


int
getColTypeFromColNamesybTab(long int  henv, long int  hdbc,
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
  i = exec_sql_sybTab( (SQLHENV) henv, (SQLHDBC) hdbc, &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_sybTab( hstmt, &num_of_cols);
  if (i != 0) return(i);
  i = bind_default_row_sybTab( hstmt, data, data_size, &num_of_cols);
  if (i != 0) return(i);
  i = get_row_sybTab(hstmt);
  if (i == 0)       {
    truncate_end_blanks(data_size[0], data[0]);
    truncate_end_blanks(data_size[1], data[1]);
    coltypenum = atoi(data[0]);
    if (coltypenum >255) 
	coltypenum -= 256;
    getDataTypeFromMap_sybTab(colType, coltypenum);
	/*strcpy(colType, (char *) data[0]);*/
  }
  else
    {
      i = done_with_default_result_sybTab(hstmt,data,data_size, num_of_cols);
      return(i);
    }
  
  i = done_with_default_result_sybTab(hstmt,data,data_size, num_of_cols);

  return(MDAS_SUCCESS);
}

int
getColumnInfoForsybTable(long int  henv, long int  hdbc,
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

  i = exec_sql_sybTab( (SQLHENV) henv, (SQLHENV) hdbc,  &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_sybTab(hstmt, &num_of_cols);
  if (i != 0) return(i);

  *numOfColumns = num_of_cols;
  for (i=1; i < num_of_cols; i++) {
	  
    j = get_column_info_sybTab(hstmt, i+1, (SQLINTEGER) &displaysize[i], colName[i], &tmpcoltype, SMALL_TOKEN , &tmpint2, &nullable[i], (SQLUINTEGER) &precision[i], &scale[i]);

    getDataTypeFromMap_sybTab(colType[i],tmpcoltype);
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
  i = exec_sql_sybTab(henv, hdbc,  &hstmt, (unsigned char *) sqlQuery);
  if (i != 0) return(i);
  i = get_no_of_columns_sybTab(hstmt, &num_of_cols);
  if (i != 0) return(i);
  i = bind_default_row_sybTab(hstmt, data,  data_size, &num_of_cols);
  if (i != 0) return(i);
  i = get_row_sybTab(hstmt);
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
      i = get_row_sybTab(hstmt);
    }
    *numOfColumns = jj;
  }
  else
    {
      i = done_with_default_result_sybTab(hstmt,data,data_size, num_of_cols);
      return(i);
    }
  
  i = done_with_default_result_sybTab(hstmt,data,data_size, num_of_cols);
*/
  return(MDAS_SUCCESS);
}
  


