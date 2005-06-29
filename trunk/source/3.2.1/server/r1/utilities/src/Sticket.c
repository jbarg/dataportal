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
  fprintf(stderr,"Sticket  [-F fileName] [-B beginTime] [-E endTime] [-N AccessCount] { -D dataName | -C collName | -R collName } user@domain ...\n");
  fprintf(stderr,"   use ticketuser@sdsc for issuing generic tickets.\n");
}

int
main(int argc, char **argv)
{
    srbConn *conn;
    int i;
    int mval = 1;
    char begTime[22];
    char endTime[22];
    int  accCnt;
    int  flagVal;
    char fileName[MAX_TOKEN];
    char dataName[MAX_TOKEN];
    char collName[MAX_TOKEN];
    char userSet[MAX_TOKEN];
    char *ticketVal;
    int  newargc, dval;
    char newargv[MAX_TOKEN/4][MAX_OBJ_NAME_LENGTH];
    FILE *fd;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN];

    flagVal = -1;
    strcpy(fileName,"");
    strcpy(dataName,"");
    strcpy(collName,"");
    strcpy(begTime,"");
    strcpy(endTime,"");
    strcpy(userSet,"");
    accCnt = -1;

    strcpy(inCondition , "");
    for (; mval < argc ; mval++)
      {
	if (argv[mval][0] == '-')
	  {
	    if (argv[mval][1] == 'F' )
	      {strcpy(fileName,argv[++mval]);}
	    else if (argv[mval][1] == 'B' )
	      {strcpy(begTime,argv[++mval]);}
	    else if (argv[mval][1] == 'E' )

	      {strcpy(endTime,argv[++mval]);}
	    else if (argv[mval][1] == 'N' )
	      {accCnt = atoi(argv[++mval]);}
	    else if (argv[mval][1] == 'D' )
	      {strcpy(dataName, argv[++mval]); dval = mval; flagVal = 1;}
	    else if (argv[mval][1] == 'C' )
	      {strcpy(collName, argv[++mval]); flagVal = 2;}
	    else if (argv[mval][1] == 'R' )
	      {strcpy(collName, argv[++mval]); flagVal = 3;}
	    else 
	      {
		fprintf(stderr,"Sticket Unknown option \n");
		usage();
		exit(0);
	      }
	  }
	else
	  break;
      }
    if (flagVal < 0)
      {
	fprintf(stderr, "Sticket: wrong usage\n");
	usage();
	exit(0);
      }
    
    if ( mval >= argc)
      {
	sprintf(userSet, "%s@%s", TICKET_USER, TICKET_USER_DOMAIN);
      }
    else
      {
	strcpy(userSet, argv[mval++]);
	for (; mval < argc; mval++)
	  {strcat(userSet,"&"); strcat(userSet,argv[mval]);}
      }


    if (strlen(fileName) > 0)
      {
	fd = fopen(fileName, "w");
	if (fd == NULL) 
	 {
	   fprintf(stderr, "Sticket: Unable to open File %s\n",fileName);
	   exit(0);
	 }
      }

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Sticket Initialization Error:%i\n",i); exit(-1);}

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(3);
    }

    if (flagVal == 1)
      { /*dataset*/
	
	expand_file_names(conn, dval, dval+1, argv, &newargc,newargv);
	clSplitbychar(newargv[1],collName, dataName, '/');

	i = srbIssueTicket(conn, dataName, collName,"",begTime, endTime,
			   accCnt, userSet, &ticketVal);
      }
    else if (flagVal == 2)
      { /*collection*/
	sprintf(temp,"%s/alpha",collName);
	splitpath(temp,collName, temp1);
	i = srbIssueTicket(conn, "", collName, "D", begTime, endTime,
			   accCnt, userSet, &ticketVal);
      }
    else if (flagVal == 3)
      { /*recur collection*/
       	sprintf(temp,"%s/alpha",collName);
	splitpath(temp,collName, temp1);
	i = srbIssueTicket(conn, "", collName, "R", begTime, endTime,
			   accCnt, userSet, &ticketVal);
      }
  if ( i != 0) 
    {
      printf("Sticket: Unable to Issue Tickets:  %i\n",i);
      if (strlen(fileName) > 0) fclose(fd);
      srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      fflush(stdout);
      clFinish(conn);
      exit(0);
    }

    if (strlen(fileName) > 0)
      {
	fprintf(fd, "%s\n",ticketVal);
	fclose(fd);
	printf("Ticket has been generated and stored in %s\n", fileName);
      }
    else
      printf("Ticket=\"%s\"\n",ticketVal);
    
    free(ticketVal);
    fflush(stdout);
    clFinish(conn);
    exit(0);
}


