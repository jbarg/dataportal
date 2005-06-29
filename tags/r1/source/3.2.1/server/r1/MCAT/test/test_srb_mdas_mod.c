/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasPrototypes.h"
#include "mcatAdmin.h"

main(int argc,
        char* argv[])
{  
  



        int i;
	char obj_data_name[MAX_TOKEN];
	char obj_path_name[MAX_TOKEN];
	char obj_user_name[MAX_TOKEN];
	char resource_name[MAX_TOKEN];
	char data_value_1[MAX_TOKEN];
	char collection_name[MAX_TOKEN];
	char user_password[MAX_TOKEN];
	char user_domain[MAX_TOKEN];
	char data_value_2[MAX_TOKEN];
	int retraction_type;
        mdasC_sql_result_struct  *myresult;
        
	if ( (argc  != 10) && (argc  != 11) ) {
	  printf("Error: Use test_srb_mdas_mod <modification-type> <dataset-name> <path-name> <resource-name>  <collection-name> <user-name> <password> <domain-name><data-1> [<data-2>] \n");
	  exit(-1);
	}

#ifdef DIAG
   printf("-------------------------------- DIAGNOSTICS ----------------\n");
#endif
        retraction_type = atoi(argv[1]);
	sprintf(obj_data_name,"%s",argv[2]);
        sprintf(obj_path_name,"%s",argv[3]);
        sprintf(resource_name,"%s",argv[4]);
	sprintf(collection_name,"%s",argv[5]);
	sprintf(obj_user_name,"%s",argv[6]);
	sprintf(user_password,"%s",argv[7]);
	sprintf(user_domain,"%s",argv[8]);
        sprintf(data_value_1,"%s",argv[9]); /*user@domain for changing access */
	if (argc == 11)
	  sprintf(data_value_2,"%s",argv[10]);
	else sprintf(data_value_2,"");


        i = open_db2_interaction(MDAS_CATALOG);
        if (i != 0) {
          printf("Opening Error:%i\n",i);
          exit(-1);
        }
        printf("opened\n");
        i= get_mdas_authorization(obj_user_name, user_password, user_domain);
        if (i != 0) {
          printf("Authorization Error:%i\n",i);
          exit(-1);
        }
        printf("authorized\n");
 


       i = modify_dataset_info(MDAS_CATALOG, obj_data_name, 
			       collection_name,
			       obj_path_name,
			       resource_name,
			       data_value_1,  data_value_2,
			       retraction_type, obj_user_name,
			       user_domain);
  if (i != 0) printf("ERROR: %i\n",i);
  else  printf("DONE\n");
       close_db2_interaction(MDAS_CAT_COMMIT);
}
  
        
     


