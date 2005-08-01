/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#include "scommands.h"
 

extern char srbAuth[];
extern char srbHost[];
extern char mdasCollectionName[];
extern char inCondition[];

void
usage()
{
fprintf(stderr,"Usage: Srmticket {-F fileName | ticket}\n");
}
int
main(int argc, char **argv)
{
    srbConn *conn;
    int flag = 0;
    char ticketId[MDAS_TICKET_SIZE+2];
    FILE *fd;
    int i;
    char *thisMcat, *mcatName;

    strcpy(inCondition , "");
    if (argc < 2) {usage(); exit(1);}
    if (argv[1][0] == '-')
      {
	if ( argv[1][1] == 'F')
	  flag = 1;
	else
	  {
	    fprintf(stderr,"Srmticket: Unknown Option\n");
	    usage();
	    exit(1);
	  }
      }


    if (argc == 3 && flag == 1)
      { /* file ticket */
	fd = fopen(argv[2], "r");
	if (fd == NULL) 
	 {
	   fprintf(stderr, "Sticket: Unable to open File %s\n",argv[2]);
	   exit(1);
	 }
	fgets(ticketId, MDAS_TICKET_SIZE + 1, fd); 
	fclose(fd);
      }
    else if (argc == 2 && flag == 0)
      {
	strcpy(ticketId, argv[1]);
      }
    else
      {
	fprintf(stderr,"Srmticket: Wrong Usage\n");
	usage();
	exit(1);
      }    

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Sticket Initialization Error:%i\n",i); exit(1);}

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }
#ifdef FED_MCAT
    thisMcat = getmcatNamefromHint (mdasCollectionName);
    if (thisMcat == NULL) {
        mcatName = NULL;
    } else if (strcmp (thisMcat, "home") == 0 ||
     strcmp (thisMcat, "container") == 0) {
            mcatName = NULL;
    } else {
        mcatName = thisMcat;
    }
#endif

#ifdef FED_MCAT
    i = srbRemoveTicketWithZone (conn, mcatName, ticketId);
#else
    i = srbRemoveTicket (conn, ticketId);
#endif

 if ( i != 0) 
    {
      printf("Sticket Error: Unable to Remove Ticket:  %i\n",i);
      srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    }
      fflush(stdout);
      clFinish(conn);
      exit(0);
}


