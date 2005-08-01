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
  time_t mmmmcl;
   


/******************** TEST get_loid_info TEST ************************/


	char user_name[MAX_TOKEN];
	char data_name[MAX_TOKEN];
	char access_name[MAX_TOKEN];
	char domain_name[MAX_TOKEN];
	char collection_name[MAX_TOKEN];
	char user_password[MAX_TOKEN];
	char data_pathname[MAX_TOKEN];
	char resource_name[MAX_TOKEN];
	char comments[MAX_TOKEN];
	int success;

	if ( argc  != 11) {
	  printf("Error: Use test_srb_mdas_audit <user-name> <dataset-name> <data-pathname> <resource-name> <collection-name> <access-name> <comments> <success> <passwd> <domain-name> \n");
	  exit(-1);
	}

#ifdef DIAG
   printf("-------------------------------- DIAGNOSTICS ----------------\n");
#endif
        sprintf(user_name,"%s",argv[1]);
	sprintf(data_name,"%s",argv[2]);
	sprintf(data_pathname,"%s",argv[3]);
	sprintf(resource_name,"%s",argv[4]);
	sprintf(collection_name,"%s",argv[5]);
        sprintf(access_name,"%s",argv[6]);
	sprintf(comments,"%s",argv[7]);
	success = atoi(argv[8]);
	sprintf(user_password,"%s",argv[9]);
        sprintf(domain_name,"%s",argv[10]);
	i = open_db2_interaction(MDAS_CATALOG);
	if (i != 0) {
	  printf("Opening Error:%i\n",i);
	  exit(-1);
	}
	printf("opened\n");
	i= get_mdas_authorization(user_name,user_password, domain_name);
	if (i != 0) {
	  printf("Authorization Error:%i\n",i);
	  exit(-1);
	}
	printf("authorized\n");

	i = auditDatasetAccess(MDAS_CATALOG,user_name, data_name,
			       collection_name, data_pathname,
			       resource_name, access_name, comments, 
			       success, domain_name);
     if ( i != 0) 
	{
	  printf("Audit not performed: %i\n",i);
	}
     else
       {
         printf("Audit done\n");
       }
	 close_db2_interaction(MDAS_CAT_COMMIT);

}
  
      
        
     


