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
extern char srbUser[];
extern char mdasCollectionName[];
extern char inCondition[];
extern char mdasDomainName[];

    srbConn *conn;
    int i, myself;
    
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    int j, alive;
    char *rval[MAX_DCS_NUM];
    char dname[MAX_TOKEN];
    
    int D_case = 0;
    int I_case = 0;
   char collname[MAX_TOKEN];
    char dataname[MAX_TOKEN];
    char envstr[MAX_TOKEN];
    char envstr2[MAX_TOKEN];



void
usage()
{
fprintf(stderr,"Stls [-v] [-L number]  [-Y number] [-A condition] [-H hostname] { -F ticketFile | ticket}\n");
}
int
main(int argc, char **argv)
{
    int  newargc;
    char newargv[MAX_TOKEN][MAX_TOKEN];
    int  darg;
    int L_val = DEFAULT_ROW_COUNT;
    char hostName[MAX_TOKEN];
    char ticketId[MAX_TOKEN];
    int mval;
    int aval =0;
    FILE *fd;
    char *thisMcat, *mcatName;

    I_case = 0;
    D_case = DEFAULT_FIELD_WIDTH;
    darg = 1;
    mval = 1; 
    strcpy(ticketId,"");
    strcpy(hostName,"");
    strcpy(inCondition,"");
    strcpy(srbUser,"");
    strcpy(mdasDomainName,"");
     for (; mval < argc ; mval++)
      {
	if (argv[mval][0] == '-')
	  {
	    if (argv[mval][1] == 'L' )
	      {if (argc < mval +2) {usage(); exit(0);}
	        L_val = atoi(argv[mval+1]); mval++;}
	    else if (argv[mval][1] == 'A')
	      {if (argc < mval +2) {usage(); exit(0);}
	      strcpy(inCondition, argv[mval+1]); mval++;}
	    else if (argv[mval][1] == 'Y' )
	      {if (argc < mval +2) {usage(); exit(0);}
	        D_case = atoi(argv[mval+1]); mval++;}
	    else if (argv[mval][1] == 'H' )
	      {if (argc < mval +2) {usage(); exit(0);}
	        strcpy(hostName,argv[mval+1]); mval++;
	      }
	     else if (argv[mval][1] == 'v' )
	      {aval = 1;}
	    else if (argv[mval][1] == 'F' )
	      {
		if (argc < mval +2) {usage(); exit(0);}
	        fd = fopen(argv[mval+1], "r");
		if (fd == NULL) 
		  {
		    fprintf(stderr, "Sticket: Unable to open File %s\n",argv[mval+1]);
		    exit(0);
		  }
		fgets(ticketId, MDAS_TICKET_SIZE + 1, fd); 
		fclose(fd);
		mval++;
	      }
	    else 
	      {
		fprintf(stderr,"Stls: Unknown option \n");
		usage();
		exit(0);
	      }
	  }
	else
	  break;
      }
     if (strlen(ticketId) ==  0)
       {
	 if (mval > argc - 1)
	   {
	     fprintf(stderr,"Stls: Wrong Usage \n");
	     usage();
	     exit(0);
	   }
	 strcpy(ticketId, argv[mval++]);
       }

    for (i = 0; i < MAX_DCS_NUM; i++)
      {
	sprintf(qval[i],"");
	selval[i] = 0;
      }   
    if (strlen(inCondition) > 0) {
      i = fillQvalWithCondition(inCondition, qval);
      if (i != 0) {
	fprintf(stderr,"Stls: Data Condition Error: %i \n",i);
	srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	exit(-1);
      }
    }
    if (strlen(hostName) == 0)
      {
	i = initSrbClientEnv();
	if (i < 0)
	  {printf("Stls Initialization Error:%i\n",i); 
	   printf("SRB Host Name not found\n");
	   exit(-1);}
	strcpy(hostName,srbHost);
      }
    else {
      /*** added by RAJA June 22 2004 ***/
      strcpy(envstr,"srbUser=ticketuser");
      putenv(envstr);
      strcpy(envstr2,"mdasDomainName=sdsc" );
      putenv(envstr2);
      strcpy(srbAuth,"CANDO");
      /*** added by RAJA June 22 2004 ***/
    }
    if (ticketId[0] == 'C')
      {
	sprintf(qval[TICKET_C], " = '%s'", ticketId);
	/*
	sprintf(qval[TICKET_USER_C], " = '%s'",  TICKET_USER);
	sprintf(qval[TICKET_USER_DOMAIN_C], " = '%s'", TICKET_USER_DOMAIN);
	*/
	sprintf(qval[TICKET_USER_C], " in ( '%s', '%s')",  srbUser, TICKET_USER);
	sprintf(qval[TICKET_USER_DOMAIN_C], " in ( '%s', '%s')",TICKET_USER_DOMAIN,mdasDomainName);
	selval[DATA_NAME] = 1;
	selval[DATA_GRP_NAME] = 1;
	if (aval == 1)
	  {
	    selval[SIZE] = 1;
	    selval[DATA_TYP_NAME] = 1;
	    selval[TICKET_BEGIN_TIME_C] = 1;
	    selval[TICKET_END_TIME_C] = 1;
	    selval[TICKET_ACC_COUNT_C] = 1;
	    selval[TICKET_OWNER_C] = 1;
	    selval[TICKET_OWNER_DOMAIN_C] = 1;
	  }
      }
    else if (ticketId[0] == 'D')
      {
	sprintf(qval[TICKET_D], " = '%s'", ticketId);
	/*
	sprintf(qval[TICKET_USER_D], " = '%s'",  TICKET_USER);
	sprintf(qval[TICKET_USER_DOMAIN_D], " = '%s'", TICKET_USER_DOMAIN);
	*/
	sprintf(qval[TICKET_USER_D], " in ( '%s', '%s')",  srbUser, TICKET_USER);
	sprintf(qval[TICKET_USER_DOMAIN_D], " in ( '%s', '%s')",TICKET_USER_DOMAIN,mdasDomainName);

	selval[DATA_NAME] = 1;
	selval[DATA_GRP_NAME] = 1;
	if (aval == 1)
	  {
	    selval[SIZE] = 1;
	    selval[DATA_TYP_NAME] = 1;
	    selval[TICKET_BEGIN_TIME_D] = 1;
	    selval[TICKET_END_TIME_D] = 1;
	    selval[TICKET_ACC_COUNT_D] = 1;
	    selval[TICKET_OWNER_D] = 1;
	    selval[TICKET_OWNER_DOMAIN_D] = 1;
	  }
      }
    else
      {fprintf(stderr,"Stls: Unknown Ticket\n"); exit(0);}


    conn = srbConnect (hostName, NULL, srbAuth, 
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
    i = srbGetDataDirInfoWithZone(conn, MDAS_CATALOG, mcatName, qval, selval, 
     &myresult, L_val);
#else
    i = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult, L_val);
#endif
    if (i < 0)
      {
	if (i != CLI_NO_ANSWER) {
	  fprintf(stderr,"SgetU Error: %i\n", i);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	} else fprintf(stderr,"No Answer\n");    fflush(stdout);
	 clFinish(conn); exit(4);
      }
    show_results(conn, qval, selval, &myresult, D_case, L_val);

    clFinish(conn);
    fflush(stdout);
    exit(0);
}


