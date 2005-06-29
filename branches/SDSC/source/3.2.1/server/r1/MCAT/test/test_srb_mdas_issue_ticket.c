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

  if ( argc  <10) {
    printf("Error: Use test_srb_mdas_issue_ticket <user-name> <passwd>  <domain-name> <data-name> <collection-name> <flag> <begin-time> <end-time> <count> <target-user> ...\n");
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

  strcpy(userSet, argv[10]);
  for (i = 11; i < argc; i++)
    {strcat(userSet,"&"); strcat(userSet,argv[i]);}

  /* printf("4=%s:%s:%s:%s:%s:%i:%s:%s:\n",
     argv[4],argv[5],argv[6],argv[7],argv[8],atoi(argv[9]),argv[10],userSet);
  */

  i = issueTicket(argv[4],argv[5],argv[6],argv[7],argv[8],atoi(argv[9]),
		  userSet, &ticket,
		  argv[1],argv[3]);

  if ( i != 0) 
    {
      printf("Main: Unable to Issue Tickets:  %i\n",i);
      free(ticket);
      close_db2_interaction(MDAS_CAT_ROLLBACK);
      exit (-1);
    }
      
  printf("Ticket: \"%s\"\n", ticket);
  free(ticket);
  close_db2_interaction(MDAS_CAT_COMMIT);
}

