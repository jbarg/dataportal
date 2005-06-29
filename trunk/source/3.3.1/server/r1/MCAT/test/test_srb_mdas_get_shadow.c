/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasPrototypes.h"
#include "mdasGlobalsExtern.h"
#include "mcatAdmin.h"


        mdasC_sql_result_struct  *myresult;
	char *res_data_name;
	char *res_data_path_name;
        char *res_rsrc_name;
	char *res_rsrc_typ_name;
	char *res_rsrc_locn_name;
	char *res_data_typ_name;
        char shadow_path[MAX_TOKEN], newPath[MAX_TOKEN];
        char tmpPath[MAX_TOKEN];
	char m_user_name[MAX_TOKEN];
	char data_name[MAX_TOKEN];
	char access_name[MAX_TOKEN];
	char domain_name[MAX_TOKEN];
	char user_pasword[MAX_TOKEN];
	char collection_name[MAX_TOKEN];

main(int argc,
        char* argv[])
{  
  int i, cont_index, numrows;
  time_t mmmmcl;
   char qval[MAX_DCS_NUM][MAX_TOKEN];
int selval[MAX_DCS_NUM];
char *access_list;


        int j, alive;
        char *rval[MAX_DCS_NUM];

         

/******************** TEST get_loid_info TEST ************************/
  /*  TEST CASE:   test_srb_mdas_get 001.001 sekar <passwd> all legion  */


	if ( argc  < 7 ||  argc  > 9 ) {
	  printf("Error: Use test_srb_mdas_get <dataset-name> <user-name> <passwd> <access-name> <domain-name> <colection-name> <rows-required> <shadow-path>\n");
	  exit(-1);
	}
      shadow_path[0] = '\0';
#ifdef DIAG
   printf("-------------------------------- DIAGNOSTICS ----------------\n");
#endif
	sprintf(data_name,"%s",argv[1]);
        sprintf(m_user_name,"%s",argv[2]);
	sprintf(user_pasword,"%s",argv[3]);
        sprintf(access_name,"%s",argv[4]);
        sprintf(domain_name,"%s",argv[5]);
	sprintf(collection_name,"%s",argv[6]);
	if (argc == 7) numrows = 100;
	else numrows =  atoi(argv[7]);
	if (argc == 9) sprintf(shadow_path,"?SHADOW=%s",argv[8]);
	
	/*printf("MAIN:DN=%s,UN=%s,PW=%s,AC=%s,DO=%s,CN=%s\n",data_name,
	       m_user_name,user_pasword,access_name,domain_name,
	       collection_name);*/
	i = open_db2_interaction(MDAS_CATALOG);
	if (i != 0) {
	  printf("Opening Error:%i\n",i);
	  exit(-1);
	}
	printf("opened\n");
	i= get_mdas_authorization(m_user_name,user_pasword, domain_name);
	if (i != 0) {
	  printf("Authorization Error:%i\n",i);
	  exit(-1);
	}
	printf("authorized\n");
	
	i = getDatasetInfo(MDAS_CATALOG, data_name,m_user_name, access_name, 
			   domain_name, collection_name, &myresult, numrows);
     if (myresult == NULL || i != 0) 
       {
	 printf("Main: Missing object data  in database: %i\n",i);
	 free(myresult);
	 close_db2_interaction(MDAS_CAT_COMMIT);
	 exit (-1);
       }
     printf("COLCNT:%i\n",myresult->result_count);
     printf("ROWCNT:%i\n",myresult->row_count);
     printf("STMTHN:%i\n",myresult->continuation_index);
     print_results();
     cont_index = myresult->continuation_index; 
     while (cont_index >= 0)
       {
	 i = get_more_rows(MDAS_CATALOG,cont_index, &myresult, numrows);
	 if (myresult == NULL || i != 0) 
	   {
	     printf("Main: Missing object data  in database: %i\n",i);
	     free(myresult);
		  close_db2_interaction(MDAS_CAT_COMMIT);
		  exit (-1);
	   }
	 printf("COLCNT:%i\n",myresult->result_count);
	 printf("ROWCNT:%i\n",myresult->row_count);
	 printf("STMTHN:%i\n",myresult->continuation_index);
	 print_results();
	 cont_index = myresult->continuation_index; 
       }


     close_db2_interaction(MDAS_CAT_COMMIT);

	


}
  
      
print_results()
{
  char cval[MAX_TOKEN];
  int status;
     printf("\n ---------------------------- RESULT ------------------- \n");

   sprintf(cval,"%sMDAS_AD_REPL",MDASSCHEMENAME);
   res_data_path_name = (char *) get_from_result_struct(
             (mdasC_sql_result_struct *) myresult, cval,"path_name");
   sprintf(cval,"%sMDAS_TD_DATA_TYP",MDASSCHEMENAME);
   res_data_typ_name = (char *)get_from_result_struct(
             (mdasC_sql_result_struct *) myresult, cval,"data_typ_name");
   sprintf(cval,"%sMDAS_AR_PHYSICAL",MDASSCHEMENAME);
   res_rsrc_name = (char *)get_from_result_struct(
             (mdasC_sql_result_struct *) myresult, cval,"phy_rsrc_name");
   sprintf(cval,"%sMDAS_TD_RSRC_2TYP",MDASSCHEMENAME);
   res_rsrc_typ_name = (char *)get_from_result_struct(
             (mdasC_sql_result_struct *) myresult, cval,"rsrc_typ_name");
   sprintf(cval,"%sMDAS_TD_LOCN",MDASSCHEMENAME);
   res_rsrc_locn_name = (char *)get_from_result_struct(
             (mdasC_sql_result_struct *) myresult, cval,"netprefix");



   i = 0;
   while (i < myresult->row_count)
     {

       printf("DATA TYPE NAME: %s\n",res_data_typ_name + i*  MAX_DATA_SIZE);
       printf("DATA PATH NAME: %s\n",res_data_path_name + i*  MAX_DATA_SIZE);
       printf("RESOURCE NAME : %s\n",res_rsrc_name + i*  MAX_DATA_SIZE);
       printf("RESOURCE TYPE : %s\n",res_rsrc_typ_name + i*  MAX_DATA_SIZE);
       printf("RESOURCE LOCN : %s\n",res_rsrc_locn_name + i*  MAX_DATA_SIZE);
       sprintf(tmpPath,"%s/?SPLITPATH",res_data_path_name + i*  MAX_DATA_SIZE);
       printf("TMP PATH NAME : %s\n",tmpPath);
       if (strlen(shadow_path) > 0) {
	 status = getFullPathName (tmpPath, "",
				   m_user_name, domain_name,
				   shadow_path, data_name, collection_name,
				   newPath);
	 if (status != 0) printf("WRONG FULL PATH: %i\n",status);
	 printf("FULL PATH     : %s\n",newPath);
       }
       i++;
       printf(" ------------------------------------------------------------ \n");
     }
   free(res_data_name);
   free(res_data_path_name);
   free(res_rsrc_name);
   free(res_rsrc_typ_name);
   free(res_rsrc_locn_name);
   free(myresult);

}        
     


