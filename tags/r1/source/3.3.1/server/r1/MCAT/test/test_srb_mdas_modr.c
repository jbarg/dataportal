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
  


        char data_value_2[MAX_TOKEN];
        char data_value_3[MAX_TOKEN];
        char data_value_4[MAX_TOKEN];
        int i;
	int retraction_type;
        
	if ( (argc  != 7) && (argc  != 8) && (argc  != 9)  && (argc  != 10) ) {
	  printf("Error: Use test_srb_mdas_modr <modification-type>  <rsrc-name> <user-name> <pass-word> <domain-name> <data-1> [<data-2>] [<data-3>] [<data-4>]\n");
	  exit(-1);
	}

        i = open_db2_interaction(MDAS_CATALOG);
        if (i != 0) {
          printf("Opening Error:%i\n",i);
          exit(-1);
        }
        printf("opened\n");
        i= get_mdas_authorization( argv[3],  argv[4], argv[5]);
        if (i != 0) {
          printf("Authorization Error:%i\n",i);
          exit(-1);
        }
        printf("authorized\n");

        retraction_type = atoi(argv[1]);
	if (argc >= 8)
	  sprintf(data_value_2,"%s",argv[7]);
	else sprintf(data_value_2,"");
	if (argc >= 9)
	  sprintf(data_value_3,"%s",argv[8]);
	else sprintf(data_value_3,"");
	if (argc == 10)
	  sprintf(data_value_4,"%s",argv[9]);
	else sprintf(data_value_4,"");

       i = modify_resource_info(MDAS_CATALOG, argv[2], argv[3], argv[5], 
				retraction_type, argv[6],
				data_value_2,data_value_3,
				data_value_4);
  if (i != 0) printf("ERROR: %i\n",i);
  else  printf("DONE\n");
    close_db2_interaction(MDAS_CAT_COMMIT);
 
}
  
      
        
     


