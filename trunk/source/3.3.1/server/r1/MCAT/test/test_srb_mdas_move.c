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
  /*  TEST CASE:   test_srb_mdas_move 001.009 'legion-db-sdsc' 'legion.lc_b_1/001.009' 'legion-unix-sdsc' 'alpha/beta/gamma'*/
      

        if ( argc  != 10 ) {
          printf("Error: Use test_srb_mdas_move  <dataset-name>  <collection-name> <old-resource-name> <old-path-name> <new-resource-name> <new-path-name> <user-name> <password> <domain-name> \n");
	  printf("Example: test_srb_mdas_move 001.009  srbtest 'legion-db-sdsc' 'legion.lc_b_1/001.009' 'legion-unix-sdsc' 'alpha/beta/gamma' sekar <pwd> legion\n");
          exit(-1);
        }
       i = open_db2_interaction(MDAS_CATALOG);
        if (i != 0) {
          printf("Opening Error:%i\n",i);
          exit(-1);
        }
        printf("opened\n");
        i= get_mdas_authorization(argv[7],argv[8],argv[9]);
        if (i != 0) {
          printf("Authorization Error:%i\n",i);
          exit(-1);
        }
        printf("authorized\n");

	i= move_dataset(MDAS_CATALOG, argv[1],argv[2],argv[3],argv[4],argv[5],argv[6], argv[7], argv[9]);
  if (i != 0) printf("ERROR: %i\n",i);
  else  printf("DONE\n");
close_db2_interaction(MDAS_CAT_COMMIT);


}
  
      
        
     


