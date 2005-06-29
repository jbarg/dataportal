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
      if ( argc  != 4 ) {
          printf("Error: Use test_srb_mdas_auth  user-name> <password> <domain-name>\n");
          exit(-1);
        }



   i = open_db2_interaction(MDAS_CATALOG);
   if (i != 0) {
     printf("Opening Error:%i\n",i);
     exit(-1);
   }
   i= get_mdas_authorization(argv[1],argv[2], argv[3]);
   printf("Authorization=%i\n",i);
   close_db2_interaction(MDAS_CAT_COMMIT);
   
}
  
      
        
     


