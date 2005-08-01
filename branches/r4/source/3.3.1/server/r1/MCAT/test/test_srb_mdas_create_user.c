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
    int i, j;
/****************** TEST register_loid_info TEST *********************/

      
        if ( argc  != 11 ) {
          printf("Error: Use test_srb_mdas_create_user  <user-name> <user-password> <user-domain-name> <user-type-name>  <user-address> <user-phone> <user-email> <registrar-name> <registrar-password> <registrar-domain-name>\n");

          exit(-1);
        }

        i = open_db2_interaction(MDAS_CATALOG);
        if (i != 0) {
          printf("Opening Error:%i\n",i);
          exit(-1);
        }
        printf("opened\n");

  printf("USER REGISTRATION STARTED...\n");
      i= register_user_info(MDAS_CATALOG, argv[1],argv[2],argv[3],argv[4],argv[5],argv[6],argv[7],argv[8],argv[9], argv[10]);


      j = close_db2_interaction(MDAS_CAT_COMMIT);
  if (i != 0) 
    {
      printf("ERROR: %i\n",i);
    }
  else 
    {
      printf("DONE\n");
    }

}
  
      
        
     


