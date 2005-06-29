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
extern char mdasDomainName[];
extern char mdasCollectionName[];
extern char inCondition[];

void
usage()
{
  fprintf(stderr,"SgetT [-h] [-u] [-v] [-L n] [-Y n] [ -F fileName | -T ticket | -D dataName | -C collection | -U [-c] userName domainName ]\n");
}

int
main(int argc, char **argv)
{

    srbConn *conn;
    int i;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    int D_case = 0;
    int u_case= 0;
    char ticketId[MDAS_TICKET_SIZE+2];
    char userName[MAX_TOKEN];
    char domainName[MAX_TOKEN];
    char strTmp[MAX_TOKEN];
    char dataName[MAX_TOKEN];
    char collName[MAX_TOKEN];
    FILE *fd;
    int aval = 0;
    int cval = 0;
    int mval = 1;
    int nullcase = 0;
    int L_val = DEFAULT_ROW_COUNT;
    char *thisMcat, *mcatName;

    D_case = DEFAULT_FIELD_WIDTH;
    strcpy(ticketId,"");
    strcpy(userName,"");
    strcpy(dataName,"");
    strcpy(collName,"");
    strcpy(inCondition , "");

    i = initSrbClientEnv();
    if (i < 0)
      {printf("SgetT Initialization Error:%i\n",i);     fflush(stdout); exit(1);}


    for (i = 0; i < MAX_DCS_NUM; i++)
      {
	sprintf(qval[i],"");
	selval[i] = 0;
      }   




    for (; mval < argc ; mval++)
      {
	if (argv[mval][0] == '-')
	  {
	    if (argv[mval][1] == 'D')
	      {if (argc < mval +2) {usage(); exit(1);}
		sprintf(dataName, argv[++mval]);
	      }
	    else if (argv[mval][1] == 'C')
	      {if (argc < mval +2) {usage(); exit(1);}
		sprintf(collName, argv[++mval]);
	      }
	    else if (argv[mval][1] == 'U')
	      {if (argc < mval +2) {usage(); exit(1);}
		if (argv[mval+1][0] == '-')
		  {
		    if (argv[mval+1][1] != 'c')
		      {
			fprintf(stderr,"SgetT Unknown option for -U \n");
			fprintf(stderr,"SgetT [-h] [-u] [-v] [-L n] [-Y n] [ -F fileName | -T ticket | -D dataName | -C collection | -U [-c] userName domainName ]\n");
			exit(1);
		      }
		    cval = 1; mval++;
		  }
		if ( mval != (argc - 3) )
		  {
		    fprintf(stderr,"SgetT Wrong Usage\n: Missing parameters");
		    fprintf(stderr,"SgetT [-h] [-u] [-v] [-L n] [-Y n] [ -F fileName | -T ticket | -D dataName | -C collection | -U [-c] userName domainName ]\n");
		    exit(1);
		  }
		strcpy(userName, argv[++mval]);
		strcpy(domainName, argv[++mval]);
	      }
	    else if (argv[mval][1] == 'T')
	      {if (argc < mval +2) {usage(); exit(1);}
		sprintf(ticketId, argv[++mval]);
	      }
	    else if (argv[mval][1] == 'F')
	      {if (argc < mval +2) {usage(); exit(1);}
		fd = fopen(argv[++mval], "r");
		if (fd == NULL) 
		  {
		    fprintf(stderr, 
			"SgetT: Unable to open Ticket File %s\n",argv[mval]);
		    exit(1);
		  }
		fgets(ticketId, MDAS_TICKET_SIZE + 1, fd); 
		fclose(fd);
	      }
	    else if (argv[mval][1] == 'Y')
	      {if (argc < mval +2) {usage(); exit(1);}
		D_case = atoi(argv[++mval]);
	      }
	    else if (argv[mval][1] == 'L')
	      {if (argc < mval +2) {usage(); exit(1);}
		L_val= atoi(argv[++mval]);
	      }
	    else if (argv[mval][1] == 'u')
	      { 
		u_case = 1; 
	      }
	    else if (argv[mval][1] == 'v')
	      { 
		aval = 1; 
	      }
	    else if (argv[mval][1] == 'h')
	      {
		usage();
		exit(1);
		break;
	      }
	    else 
	      {
		fprintf(stderr,"SgetT Unknown option \n");
		usage();
		exit(1);
	      }
	  }
	else 
	  break;
      }
	    

    
    if (u_case == 0)
      {
	if (strlen(ticketId) > 0)
	  {
	    if (ticketId[0] == 'C')
	      {
		sprintf(qval[TICKET_OWNER_C], " = '%s'", srbUser);
		sprintf(qval[TICKET_OWNER_DOMAIN_C]," = '%s'", mdasDomainName);
		sprintf(qval[TICKET_C], " = '%s'", ticketId);
		selval[DATA_GRP_NAME] = 1;
		selval[TICKET_USER_C] = 1;
		selval[TICKET_USER_DOMAIN_C] = 1;
		if (aval == 1)
		  {
		    selval[DATA_NAME] = 1;
		    selval[SIZE] = 1;
		    selval[DATA_TYP_NAME] = 1;
		    selval[TICKET_BEGIN_TIME_C] = 1;
		    selval[TICKET_END_TIME_C] = 1;
		    selval[TICKET_ACC_COUNT_C] = 1;
		  }
	      }
	    else  if (ticketId[0] == 'D')
	      {
		sprintf(qval[TICKET_OWNER_D], " = '%s'", srbUser);
		sprintf(qval[TICKET_OWNER_DOMAIN_D]," = '%s'", mdasDomainName);
		sprintf(qval[TICKET_D], " = '%s'", ticketId);
		selval[DATA_NAME] = 1;
		selval[DATA_GRP_NAME] = 1;
		selval[TICKET_USER_D] = 1;
		selval[TICKET_USER_DOMAIN_D] = 1;
		if (aval == 1)
		  {
		    selval[SIZE] = 1;
		    selval[DATA_TYP_NAME] = 1;
		    selval[TICKET_BEGIN_TIME_D] = 1;
		    selval[TICKET_END_TIME_D] = 1;
		    selval[TICKET_ACC_COUNT_D] = 1;
		  }
	      }
	    else
	      {fprintf(stderr,"SgetT: Unknown Ticket\n"); exit(1);}
	  }
	else if (strlen(collName) > 0)
	  {
	    sprintf(qval[TICKET_OWNER_C], " = '%s'", srbUser);
	    sprintf(qval[TICKET_OWNER_DOMAIN_C]," = '%s'", mdasDomainName);
	    strcpy(strTmp,collName);
	    make_like_for_mdas(strTmp);
	    if (strcmp(strTmp,collName))
	      {
#ifdef MCAT_VERSION_10
		sprintf(qval[DATA_GRP_NAME]," like '%s %%'",strTmp);
#else
		sprintf(qval[DATA_GRP_NAME]," like '%s'",strTmp);
#endif
		selval[DATA_GRP_NAME] = 1;
	      }
	    else
	      { sprintf(qval[DATA_GRP_NAME]," = '%s'", strTmp);}
	    selval[TICKET_C] = 1;
	    selval[TICKET_USER_C] = 1;
	    selval[TICKET_USER_DOMAIN_C] = 1;
	    if (aval == 1)
	      {
		selval[TICKET_BEGIN_TIME_C] = 1;
		selval[TICKET_END_TIME_C] = 1;
		selval[TICKET_ACC_COUNT_C] = 1;
	      }
		
	  }
	else if (strlen(dataName) > 0)
	  {
	    sprintf(qval[TICKET_OWNER_D], " = '%s'", srbUser);
	    sprintf(qval[TICKET_OWNER_DOMAIN_D]," = '%s'", mdasDomainName);
	    strcpy(strTmp,dataName);
	    make_like_for_mdas(strTmp);
	    if (strcmp(strTmp,dataName))
	      {
#ifdef MCAT_VERSION_10
		sprintf(qval[DATA_NAME]," like '%s %%'",strTmp);
#else
		sprintf(qval[DATA_NAME]," like '%s'",strTmp);
#endif
		selval[DATA_NAME] = 1;
	      }
	    else
	      { sprintf(qval[DATA_NAME]," = '%s'", strTmp);}
	    selval[DATA_GRP_NAME] = 1;
	    selval[TICKET_D] = 1;
	    selval[TICKET_USER_D] = 1;
	    selval[TICKET_USER_DOMAIN_D] = 1;
	    if (aval == 1)
	      {
		selval[SIZE] = 1;
		selval[DATA_TYP_NAME] = 1;
		selval[TICKET_BEGIN_TIME_D] = 1;
		selval[TICKET_END_TIME_D] = 1;
		selval[TICKET_ACC_COUNT_D] = 1;
	      }
	  }
	else if (strlen(userName) > 0)
	  { 
	    if (cval == 1)
	      {
		sprintf(qval[TICKET_OWNER_C], " = '%s'", srbUser);
		sprintf(qval[TICKET_OWNER_DOMAIN_C]," = '%s'", mdasDomainName);
		selval[TICKET_C] = 1;
		sprintf(qval[TICKET_USER_C], " = '%s'",  userName);
		sprintf(qval[TICKET_USER_DOMAIN_C], " = '%s'", domainName);
		selval[DATA_GRP_NAME] = 1;
		if (aval == 1)
		  {
		    selval[DATA_NAME] = 1;
		    selval[SIZE] = 1;
		    selval[DATA_TYP_NAME] = 1;
		    selval[TICKET_BEGIN_TIME_C] = 1;
		    selval[TICKET_END_TIME_C] = 1;
		    selval[TICKET_ACC_COUNT_C] = 1;
		  }
	      }
	    else
	      {
		sprintf(qval[TICKET_OWNER_D], " = '%s'", srbUser);
		sprintf(qval[TICKET_OWNER_DOMAIN_D]," = '%s'", mdasDomainName);
		selval[TICKET_D] = 1;
		sprintf(qval[TICKET_USER_D], " = '%s'",  userName);
		sprintf(qval[TICKET_USER_DOMAIN_D], " = '%s'", domainName);
		selval[DATA_NAME] = 1;
		selval[DATA_GRP_NAME] = 1;
		if (aval == 1)
		  {
		    selval[SIZE] = 1;
		    selval[DATA_TYP_NAME] = 1;
		    selval[TICKET_BEGIN_TIME_D] = 1;
		    selval[TICKET_END_TIME_D] = 1;
		    selval[TICKET_ACC_COUNT_D] = 1;
		  }
	      }
	  }
	else
	  {
	    nullcase = 1;
	    printf("Tickets issued for datasets\n");
	    sprintf(qval[TICKET_OWNER_D], " = '%s'", srbUser);
	    sprintf(qval[TICKET_OWNER_DOMAIN_D]," = '%s'", mdasDomainName);
	    selval[TICKET_D] = 1;
	    selval[TICKET_USER_D] = 1;
	    selval[TICKET_USER_DOMAIN_D] = 1;
	    selval[DATA_NAME] = 1;
	    selval[DATA_GRP_NAME] = 1;
	    if (aval == 1)
	      {
		selval[SIZE] = 1;
		selval[DATA_TYP_NAME] = 1;
		selval[TICKET_BEGIN_TIME_D] = 1;
		selval[TICKET_END_TIME_D] = 1;
		selval[TICKET_ACC_COUNT_D] = 1;
	      }
	  }
      }
    else /* u_case == 1) */
      {
	if (strlen(ticketId) > 0)
	  {
	    if (ticketId[0] == 'C')
	      {
		sprintf(qval[TICKET_USER_C], " = '%s'", srbUser);
		sprintf(qval[TICKET_USER_DOMAIN_C]," = '%s'", mdasDomainName);
		sprintf(qval[TICKET_C], " = '%s'", ticketId);
		selval[DATA_GRP_NAME] = 1;
		if (aval == 1)
		  {
		    selval[TICKET_OWNER_C] = 1;
		    selval[TICKET_OWNER_DOMAIN_C] = 1;
		    selval[DATA_NAME] = 1;
		    selval[SIZE] = 1;
		    selval[DATA_TYP_NAME] = 1;
		    selval[TICKET_BEGIN_TIME_C] = 1;
		    selval[TICKET_END_TIME_C] = 1;
		    selval[TICKET_ACC_COUNT_C] = 1;
		  }
	      }
	    else  if (ticketId[0] == 'D')
	      {
		sprintf(qval[TICKET_USER_D], " = '%s'", srbUser);
		sprintf(qval[TICKET_USER_DOMAIN_D]," = '%s'", mdasDomainName);
		sprintf(qval[TICKET_D], " = '%s'", ticketId);
		selval[DATA_NAME] = 1;
		selval[DATA_GRP_NAME] = 1;
		if (aval == 1)
		  {
		    selval[TICKET_OWNER_D] = 1;
		    selval[TICKET_OWNER_DOMAIN_D] = 1;
		    selval[SIZE] = 1;
		    selval[DATA_TYP_NAME] = 1;
		    selval[TICKET_BEGIN_TIME_D] = 1;
		    selval[TICKET_END_TIME_D] = 1;
		    selval[TICKET_ACC_COUNT_D] = 1;
		  }
	      }
	    else
	      {fprintf(stderr,"SgetT: Unknown Ticket\n"); exit(1);}
	  }
	else if (strlen(collName) > 0)
	  {
	    sprintf(qval[TICKET_USER_C], " = '%s'", srbUser);
	    sprintf(qval[TICKET_USER_DOMAIN_C]," = '%s'", mdasDomainName);
	    strcpy(strTmp,collName);
	    make_like_for_mdas(strTmp);
	    if (strcmp(strTmp,collName))
	      {
#ifdef MCAT_VERSION_10
		sprintf(qval[DATA_GRP_NAME]," like '%s %%'",strTmp);
#else
		sprintf(qval[DATA_GRP_NAME]," like '%s'",strTmp);
#endif
		selval[DATA_GRP_NAME] = 1;
	      }
	    else
	      { sprintf(qval[DATA_GRP_NAME]," = '%s'", strTmp);}
	    selval[TICKET_C] = 1;
	    if (aval == 1)
	      {
		selval[TICKET_OWNER_C] = 1;
		selval[TICKET_OWNER_DOMAIN_C] = 1;
		selval[TICKET_BEGIN_TIME_C] = 1;
		selval[TICKET_END_TIME_C] = 1;
		selval[TICKET_ACC_COUNT_C] = 1;
	      }
		
	  }
	else if (strlen(dataName) > 0)
	  {
	    sprintf(qval[TICKET_USER_D], " = '%s'", srbUser);
	    sprintf(qval[TICKET_USER_DOMAIN_D]," = '%s'", mdasDomainName);
	    strcpy(strTmp,dataName);
	    make_like_for_mdas(strTmp);
	    if (strcmp(strTmp,dataName))
	      {
#ifdef MCAT_VERSION_10
		sprintf(qval[DATA_NAME]," like '%s %%'",strTmp);
#else
		sprintf(qval[DATA_NAME]," like '%s'",strTmp);
#endif
		selval[DATA_NAME] = 1;
	      }
	    else
	      { sprintf(qval[DATA_NAME]," = '%s'", strTmp);}
	    selval[DATA_GRP_NAME] = 1;
	    selval[TICKET_D] = 1;
	    if (aval == 1)
	      {
		selval[TICKET_OWNER_D] = 1;
		selval[TICKET_OWNER_DOMAIN_D] = 1;
		selval[SIZE] = 1;
		selval[DATA_TYP_NAME] = 1;
		selval[TICKET_BEGIN_TIME_D] = 1;
		selval[TICKET_END_TIME_D] = 1;
		selval[TICKET_ACC_COUNT_D] = 1;
	      }
	  }
	else if (strlen(userName) > 0)
	  {
	    if (cval == 1)
	      {
		sprintf(qval[TICKET_USER_C], " = '%s'", srbUser);
		sprintf(qval[TICKET_USER_DOMAIN_C]," = '%s'", mdasDomainName);
		selval[TICKET_C] = 1;
		sprintf(qval[TICKET_OWNER_C], " = '%s'",  userName);
		sprintf(qval[TICKET_OWNER_DOMAIN_C], " = '%s'", domainName);
		selval[DATA_GRP_NAME] = 1;
		if (aval == 1)
		  {
		    selval[DATA_NAME] = 1;
		    selval[SIZE] = 1;
		    selval[DATA_TYP_NAME] = 1;
		    selval[TICKET_BEGIN_TIME_C] = 1;
		    selval[TICKET_END_TIME_C] = 1;
		    selval[TICKET_ACC_COUNT_C] = 1;
		  }
	      }
	    else
	      {
		sprintf(qval[TICKET_USER_D], " = '%s'", srbUser);
		sprintf(qval[TICKET_USER_DOMAIN_D]," = '%s'", mdasDomainName);
		selval[TICKET_D] = 1;
		sprintf(qval[TICKET_OWNER_D], " = '%s'",  userName);
		sprintf(qval[TICKET_OWNER_DOMAIN_D], " = '%s'", domainName);
		selval[DATA_NAME] = 1;
		selval[DATA_GRP_NAME] = 1;
		if (aval == 1)
		  {
		    selval[SIZE] = 1;
		    selval[DATA_TYP_NAME] = 1;
		    selval[TICKET_BEGIN_TIME_D] = 1;
		    selval[TICKET_END_TIME_D] = 1;
		    selval[TICKET_ACC_COUNT_D] = 1;
		  }
	      }
	  }
	else
	  {
	    nullcase = 2;
	    printf("Tickets issued for datasets\n");
	    sprintf(qval[TICKET_USER_D], " = '%s'", srbUser);
	    sprintf(qval[TICKET_USER_DOMAIN_D]," = '%s'", mdasDomainName);
	    selval[TICKET_D] = 1;
	    selval[TICKET_OWNER_D] = 1;
	    selval[TICKET_OWNER_DOMAIN_D] = 1;
	    selval[DATA_NAME] = 1;
	    selval[DATA_GRP_NAME] = 1;
	    if (aval == 1)
	      {
		selval[SIZE] = 1;
		selval[DATA_TYP_NAME] = 1;
		selval[TICKET_BEGIN_TIME_D] = 1;
		selval[TICKET_END_TIME_D] = 1;
		selval[TICKET_ACC_COUNT_D] = 1;
	      }
	  }
      }


    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
         clFinish(conn); exit(3);
    }
#ifdef FED_MCAT
    if (strlen (collName) > 0) {
	thisMcat = getmcatNamefromHint (collName);
    } else {
        thisMcat = getmcatNamefromHint (mdasCollectionName);
        if (thisMcat == NULL) {
            mcatName = NULL;
        } else if (strcmp (thisMcat, "home") == 0 ||
         strcmp (thisMcat, "container") == 0) {
                mcatName = NULL;
        } else {
            mcatName = thisMcat;
        }
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
	  fprintf(stderr,"SgetT Error: %i\n", i);
	} else {
	  fprintf(stderr,"No Answer\n");    fflush(stdout);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	}
	 clFinish(conn); exit(4);
      }
    show_results(conn, qval, selval, &myresult, D_case, L_val);
    if (nullcase > 0)
      {
	for (i = 0; i < MAX_DCS_NUM; i++)
	  {
	    sprintf(qval[i],"");
	    selval[i] = 0;
	  }   
	if (nullcase == 1)
	  {
	    printf("Tickets issued for Collections\n");
	    sprintf(qval[TICKET_OWNER_C], " = '%s'", srbUser);
	    sprintf(qval[TICKET_OWNER_DOMAIN_C]," = '%s'", mdasDomainName);
	    selval[TICKET_C] = 1;
	    selval[TICKET_USER_C] = 1;
	    selval[TICKET_USER_DOMAIN_C] = 1;
	    selval[DATA_GRP_NAME] = 1;
	    if (aval == 1)
	      {
		selval[DATA_NAME] = 1;
		selval[SIZE] = 1;
		selval[DATA_TYP_NAME] = 1;
		selval[TICKET_BEGIN_TIME_C] = 1;
		selval[TICKET_END_TIME_C] = 1;
		selval[TICKET_ACC_COUNT_C] = 1;
	      }
	  }
	else if (nullcase == 2)
	  {
	    printf("Tickets issued for Collections\n");
	    sprintf(qval[TICKET_USER_C], " = '%s'", srbUser);
	    sprintf(qval[TICKET_USER_DOMAIN_C]," = '%s'", mdasDomainName);
	    selval[TICKET_C] = 1;
	    selval[TICKET_OWNER_C] = 1;
	    selval[TICKET_OWNER_DOMAIN_C] = 1;
	    selval[DATA_GRP_NAME] = 1;
	    if (aval == 1)
	      {
		selval[DATA_NAME] = 1;
		selval[SIZE] = 1;
		selval[DATA_TYP_NAME] = 1;
		selval[TICKET_BEGIN_TIME_C] = 1;
		selval[TICKET_END_TIME_C] = 1;
		selval[TICKET_ACC_COUNT_C] = 1;
	      }
	  }
    	i = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval,
			      &myresult, L_val);
	if (i < 0)
	  {
	    if (i != CLI_NO_ANSWER)
	      fprintf(stderr,"SgetT Error: %i\n", i);
	    else fprintf(stderr,"No Answer\n");    fflush(stdout);
	     clFinish(conn); exit(5);
	  }
	show_results(conn, qval, selval, &myresult, D_case, L_val);
      }

    clFinish(conn);
    fflush(stdout);
    exit(0);
}


