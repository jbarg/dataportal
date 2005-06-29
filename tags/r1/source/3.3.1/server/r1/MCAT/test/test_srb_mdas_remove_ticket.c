/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasPrototypes.h"
#include "mcatAdmin.h"



mdasC_sql_result_struct  *myresult;
char *grp_name;
int i, numrows, cont_index;

main(int argc,
        char* argv[])
{  

  time_t mmmmcl;
  char userSet[1000];
  char *ticket;

  if ( argc  != 5) {
    printf("Error: Use test_srb_mdas_remove_ticket <user-name> <passwd>  <domain-name> <ticket>\n");
    exit(-1);
  }
  
  i = open_db2_interaction(MDAS_CATALOG);
  if (i != 0) {
    printf("Opening Error:%i\n",i);
    exit(-1);
  }
  printf("opened\n");
  i= get_mdas_authorization(argv[1], argv[2], argv[3]);
  if (i != 0) {
    printf("Authorization Error:%i\n",i);
    exit(-1);
  }
  printf("authorized\n");


  i = removeTicket(argv[4], argv[1],argv[3]);

  if ( i != 0) 
    {
      printf("Main: Unable to Remove Tickets:  %i\n",i);
      close_db2_interaction(MDAS_CAT_ROLLBACK);
      exit (-1);
    }
      
  printf("Ticket Removed\n");
  close_db2_interaction(MDAS_CAT_COMMIT);
}

