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
/****************** TEST register_loid_info TEST *********************/
/*  TEST CASE:   test_srb_mdas_create 001.009 sekar all legion 'legion-object' 'legion-db' 'legion.lc_b_1/001.009' 'legion-db-sdsc' 'legion class-class' 100*/
      
        if ( argc  != 11 ) {
          printf("Error: Use test_srb_mdas_create  <dataset-name> <user-name>  <access-name> <domain-name> <data-type-name>  <path-name> <resource-name> <data-group-name> <size>  <password>\n");
	  printf("Example: test_srb_mdas_create 001.009 sekar all legion 'legion-object' 'legion-db' 'legion.lc_b_1/001.009' 'legion-db-sdsc' 'legion class-class' 100 <password>\n");
          exit(-1);
        }

        i = open_db2_interaction(MDAS_CATALOG);
        if (i != 0) {
          printf("Opening Error:%i\n",i);
          exit(-1);
        }
        printf("opened\n");
        i= get_mdas_authorization(argv[2],argv[10], argv[4]);
        if (i != 0) {
          printf("Authorization Error:%i\n",i);
          exit(-1);
        }
        printf("authorized\n");

  printf("REGISTRATION STARTED...\n");
      i= register_dataset_info(MDAS_CATALOG, argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8], atoi(argv[9]));
  if (i != 0) 
    {
      printf("ERROR: %i\n",i);
      close_db2_interaction(1);
    }
  else 
    {
      printf("DONE\n");
      close_db2_interaction(MDAS_CAT_COMMIT);
    }

}
  
      
        
     


