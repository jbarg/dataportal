/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
#include "mcatAdmin.h"

extern int get_collection_id_from_name(int cat_type, char * col_name,
		    char **grp_id, char **grp_name);


main(int argc,
        char* argv[])
{  
  int i;
  time_t mmmmcl;
   


/******************** TEST get_loid_info TEST ************************/
  /*  TEST CASE:   test_srb_mdas_get 001.001 sekar <passwd> all legion  */


	char *grp_id;
	char *grp_name;

	if ( argc  != 5) {
	  printf("Error: Use test_srb_mdas_get_collection_id <group-name> <user-name> <passwd>  <domain-name> \n");
	  exit(-1);
	}

	i = open_db2_interaction(MDAS_CATALOG);
	if (i != 0) {
	  printf("Opening Error:%i\n",i);
	  exit(-1);
	}
	printf("opened\n");
	i= get_mdas_authorization(argv[2], argv[3], argv[4]);
	if (i != 0) {
	  printf("Authorization Error:%i\n",i);
	  exit(-1);
	}
	printf("authorized\n");

	i = get_collection_id_from_name(MDAS_CATALOG, argv[1], &grp_id, &grp_name);
     if ( i != 0) 
       {
	 printf("Main: Missing collection  in database: %i\n",i);
	 exit (-1);
       }


     printf("\n\n ---------------------------- RESULT ------------------- \n");
   i = 0;


   while (strcmp((char *)((int) grp_id + i* MAX_DATA_SIZE), RESWANTED ))
     {
       printf(" %s                  %s\n",  grp_id + i*   MAX_DATA_SIZE,
	      grp_name + i*  MAX_DATA_SIZE);

       i++;

     }

   free(grp_id);
   free(grp_name);
   close_db2_interaction(MDAS_CAT_COMMIT);


}
  
      
        
     


