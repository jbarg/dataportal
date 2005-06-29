/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasC_db2_externs.h"
#include "elogExtern.h"

extern char MDASSCHEMENAME[];
extern char catser[];
extern char cattype[];
extern char user[];
extern char pass[];
extern char instance[];
extern char exec_type[];
extern char rsrc_id[];
extern char db2instancebuffer[];

extern mdasC_servh  serverhndl[];
extern mdasC_sql_query_struct info[];
extern mdasC_sql_result_struct result[];
int server_handle_count;
int info_struct_count;
int result_struct_count;




void mdasC_init(int argc,
        char* argv[],
        void* comm,
        mdasC_status status
        )
{

     int i;
     mdasC_infoh userinfo, ticketinfo, serverinfo;

     for (i= 0; i < MAX_SERVERS; i++){
	 serverhndl[i].open = 0;
     }
     server_handle_count = 0;
 
     userinfo.info = &user;
     serverinfo.info = &catser;
     ticketinfo.info = &pass;

     i = get_next_free_server_handle();
     sprintf(db2instancebuffer,"DB2INSTANCE=%s",instance);
     putenv(db2instancebuffer);
     
     mdasC_db2_connect(&userinfo,&ticketinfo,&serverinfo,
		       NULL,&serverhndl[i],status);

     serverhndl[i].open = 1;
     strcpy(serverhndl[i].sname,cattype);

}
	   
void mdasC_finalize(void* comm,
        mdasC_status status )
{
     int i;
     for (i=0; i < MAX_SERVERS; i++){
       if (serverhndl[i].open == 1){
	   mdasC_db2_dcon(serverhndl[i],status);
       }
     }
}


mdasC_inquire(mdasC_servh* servh , 
	       mdasC_infoh* info , 
	       mdasC_infoh* result ,
	       mdasC_status status, int numrows)
{
     mdasC_db2_info_inquire(servh ,info ,result ,status, numrows);
}

mdasC_inquire_more(long int hstmt,
		   mdasC_infoh* result ,  
		   mdasC_status status,
		   int numrows
		   )
{
  mdasC_db2_info_inquire_more( hstmt, result, status, numrows);
}


mdasC_exec(mdasC_servh* servh , 
	       mdasC_infoh* data_info , 
	       mdasC_infoh* method_info , 
	       mdasC_infoh* param_info,
	       mdasC_infoh* result ,
	       mdasC_status status)
{
   int i,j;
   char method_name[MAX_INFO_STRUCTS], *meth_name, *meth_path_name, *data_name,
         *data_path_name, full_data_name[MAX_INFO_STRUCTS];
   char tabnameval[MAX_TOKEN];
   char exec_string[HUGE_STRING];
   char new_exec_string[HUGE_STRING];
   int exec_status = 0;

   /* This is to get info about method application (hard coded below)
       i = get_next_free_info();
       j = get_next_free_result();
       set_info_3(&info[i], out_data_type_name,in_data_type_id,
		   exec_type, rsrc_id, user);
       mdasC_inquire(&serverhndl[0], (mdasC_infoh*) &info[i] ,
	       (mdasC_infoh*)  &result[j], status);  
   */
   sprintf(tabnameval,"%s%s", MDASSCHEMENAME,"MDAS_AD_REPL");
   data_name = (char *)get_from_result_struct(
	     (mdasC_sql_result_struct *)data_info, tabnameval,"data_name");
   if (data_name == NULL) 
      {
#ifdef DEBUGON
	elog(NOTICE,"Exec: Missing data name\n");
#endif /* DEBUGON */ 
	set_status_error(status); return;
      }
   sprintf(tabnameval,"%s%s", MDASSCHEMENAME,"MDAS_AM_REPL");
   meth_name = (char *)get_from_result_struct(
	     (mdasC_sql_result_struct *) method_info, tabnameval,"meth_name");
   if (meth_name == NULL) 
      {
#ifdef DEBUGON
	elog(NOTICE,"Exec: Missing  method name\n");
#endif /* DEBUGON */ 
	set_status_error(status);return;
      }
   sprintf(tabnameval,"%s%s", MDASSCHEMENAME,"MDAS_AD_REPL");
   data_path_name = (char *)get_from_result_struct(
	     (mdasC_sql_result_struct *) data_info, tabnameval,"path_name");
   if (data_path_name == NULL) 
      {
#ifdef DEBUGON
	elog(NOTICE,"Exec: Missing  data path name\n");
#endif /* DEBUGON */ 
	set_status_error(status); return;
      }
   if (!strcmp(data_path_name,"")){
     sprintf(full_data_name,"%s",data_name);
   }
   else{
     sprintf(full_data_name,"%s/%s",data_path_name,data_name);
   }
   sprintf(tabnameval,"%s%s", MDASSCHEMENAME,"MDAS_AM_REPL");
   meth_path_name = (char *)get_from_result_struct(
	     (mdasC_sql_result_struct *) method_info, tabnameval,"path_name");
   if (meth_path_name == NULL) 
      {
#ifdef DEBUGON
	elog(NOTICE,"Exec: Missing  method path name\n");
#endif /* DEBUGON */ 
	set_status_error(status); return;
      }
   if (!strcmp(meth_path_name,"")){
     sprintf(method_name,"%s",meth_name);
   }
   else{
     sprintf(method_name,"%s/%s",meth_path_name,meth_name);
   }
   sprintf(exec_string, "%s %s \n", method_name, full_data_name);

#ifdef DEBUGON
   elog(NOTICE,"<P>\n EXEC:%s",exec_string);
#endif /* DEBUGON */ 

   exec_status = system(exec_string);
#ifdef DEBUGON
   elog(NOTICE,"... EXEC(status):%i",exec_status);
#endif /* DEBUGON */ 

}

set_status_error(mdasC_status status)
{
         status[0] = -1 ;
        status[1] = 1 ;
        status[2] = MDAS_FAILURE ;
        status[3] =  MDAS_db2_INFO_INQUIRE ;

}

