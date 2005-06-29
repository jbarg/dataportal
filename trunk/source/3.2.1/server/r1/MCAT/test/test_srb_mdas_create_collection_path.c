
#include "mdasPrototypes.h"
#include "mcatAdmin.h"


main(int argc,
        char* argv[])
{  
     int i;
       if ( argc  != 6) {
          printf("Error: Use test_srb_mdas_collections <parent-col-name> <new-col-name> <user-name> <passwd> <domain-name> \n");
          exit(-1);
        }

       i = open_db2_interaction(MDAS_CATALOG);
        if (i != 0) {
          printf("Opening Error:%i\n",i);
          exit(-1);
        }
        printf("opened\n");
        i= get_mdas_authorization(argv[3], argv[4], argv[5]);
        if (i != 0) {
          printf("Authorization Error:%i\n",i);
          exit(-1);
        }
        printf("authorized\n");
	
	i = make_new_collection_path(MDAS_CATALOG, argv[1], argv[2], argv[3], argv[5]);
	if (i != 0) printf("Error: %i\n", i);
	printf("done\n ");
	close_db2_interaction(MDAS_CAT_COMMIT);

 

}
  
      
        
     


