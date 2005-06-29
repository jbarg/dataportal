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

void
usage()
{
  fprintf(stderr,"Usage: SgetU [-pPhdactgDZ]  [-L number]  [-Y number] [-T userType]  [userName@domainName]\n");
}
int
main(int argc, char **argv)
{
    srbConn *conn;
    srbResult *res;
    int i;
    int L_val = DEFAULT_ROW_COUNT;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    int D_case = DEFAULT_FIELD_WIDTH;
    int L_case = 0;
    char strTmp[MAX_TOKEN], tmpUser[MAX_TOKEN], tmpUserDomain[MAX_TOKEN];
    char *tmpPtr;
    int mval = 1;

    strcpy(inCondition , "");
    i = initSrbClientEnv();
    if (i < 0)
      {printf("SgetU Initialization Error:%i\n",i); exit(1);}


    for (i = 0; i < MAX_DCS_NUM; i++)
      {
	sprintf(qval[i],"");
	selval[i] = 0;
      }   
    selval[USER_NAME] = 1;
    selval[DOMAIN_DESC] = 1;
    selval[USER_ADDRESS] = 1;
    selval[USER_PHONE] = 1;
    selval[USER_EMAIL] = 1;
    if (argc == 1)
      {
      }
    else if (argv[1][0] == '-')
      {
	selval[USER_PHONE] = 0;
	selval[USER_ADDRESS] = 0;
	selval[USER_EMAIL] = 0;
	mval = 2;
	switch (argv[1][1])
	  {
	     case 'p':
	                break;
	     case 'P':
	                selval[USER_ADDRESS] = 1;
	                selval[USER_PHONE] = 1;
	                selval[USER_EMAIL] = 1;
			selval[USER_TYP_NAME] = 1;
			break;
	     case 'a':
	                selval[DATA_NAME] = 1;
			selval[DATA_GRP_NAME] = 1;
	                selval[ACCESS_CONSTRAINT] = 1;
			break;
	     case 'd':
	                selval[DOMAIN_DESC] = 1;
			break;
	     case 't':
	                selval[DATA_NAME] = 1;
	                selval[AUDIT_ACTION_DESC] = 1;
	                selval[AUDIT_TIMESTAMP] = 1;
			selval[AUDIT_COMMENTS] = 1;
			break;
	     case 'g':
	                selval[USER_GROUP_NAME] = 1;
			break;
	     case 'Z':
	                selval[DOMAIN_DESC] = 1;
	                selval[ZONE_NAME] = 1;
			break;
	     case 'c':
	                selval[ACCESS_GROUP_NAME] = 1;
			selval[GROUP_ACCESS_CONSTRAINT] = 1;
			break;	     
	     case 'D':
                        selval[USER_DISTIN_NAME] = 1;
                        selval[USER_AUTH_SCHEME] = 1;
                        break; 
             case 'L':
	                selval[USER_ADDRESS] = 1;
	                selval[USER_PHONE] = 1;
	                selval[USER_EMAIL] = 1;
			if (argc < 3) {usage(); exit(1);}
	                L_val = atoi(argv[2]);
			mval++;
			break;
	     case 'Y':
	                selval[USER_ADDRESS] = 1;
	                selval[USER_PHONE] = 1;
	                selval[USER_EMAIL] = 1;
			if (argc < 3) {usage(); exit(1);}
			D_case = atoi(argv[2]);
			mval++;
			break;
	     case 'T':
	                selval[USER_ADDRESS] = 1;
	                selval[USER_PHONE] = 1;
	                selval[USER_EMAIL] = 1;
			if (argc < 3) {usage(); exit(1);}
			sprintf(qval[USER_TYP_NAME], " = '%s'", argv[2]);
			mval++;
			break;

	     case 'h':
	                usage();
			exit(0);
			break;
             default :
	                fprintf(stderr,"Unknown option \n");
			usage();
			exit(1);
	  }
      }



    for (; mval < argc ; mval++)
      {
	if (argv[mval][0] == '-')
	  {
	    if (argv[mval][1] == 'L' )
	      { if (argc < mval+2) {usage(); exit(1);}
	        L_val = atoi(argv[mval+1]); mval++;}
	    else if (argv[mval][1] == 'Y' )
	      { if (argc < mval+2) {usage(); exit(1);}
	        D_case = atoi(argv[mval+1]); mval++;}
	    else if (argv[mval][1] == 'T' )
	      { if (argc < mval+2) {usage(); exit(1);}
	        sprintf(qval[USER_TYP_NAME], " = '%s'", argv[mval+1]);mval++; }
	    else 
	      {
		fprintf(stderr,"Unknown option \n");
		usage();
		exit(1);
	      }
	  }
	else
	  break;
      }

    if (mval == argc)
      {
	if (strlen(qval[USER_TYP_NAME]) > 0)
	  sprintf(qval[USER_NAME], " not like  '%%$deleted%%'");
	else {
	  sprintf(qval[USER_NAME], " = '%s'",srbUser);
	  sprintf(qval[DOMAIN_DESC], " = '%s'",mdasDomainName);
	}
      }
    else
      {
	if ((tmpPtr = strstr(argv[mval],"@")) == NULL){
	  strcpy(tmpUser,argv[mval]);
	  strcpy(tmpUserDomain,"");
	}
	else {
	  *tmpPtr = '\0';
	  tmpPtr++;
	  strcpy(tmpUser,argv[mval]);
	  strcpy(tmpUserDomain, tmpPtr);
	}
	if(strlen(tmpUser) > 0) {
	  strcpy(strTmp,tmpUser);
	  make_like_for_mdas(strTmp);
	  if (strcmp(strTmp,tmpUser))
#ifdef MCAT_VERSION_10
	    sprintf(qval[USER_NAME]," like '%s %%'",strTmp);
#else
	    sprintf(qval[USER_NAME]," like '%s'",strTmp);
#endif
	  else
	    sprintf(qval[USER_NAME]," = '%s'", strTmp);
	}
	if(strlen(tmpUserDomain) > 0) {
	  strcpy(strTmp,tmpUserDomain);
	  make_like_for_mdas(strTmp);
	  if (strcmp(strTmp,tmpUserDomain))
#ifdef MCAT_VERSION_10
	    sprintf(qval[DOMAIN_DESC]," like '%s %%'",strTmp);
#else
	    sprintf(qval[DOMAIN_DESC]," like '%s'",strTmp);
#endif
	  else
	    sprintf(qval[DOMAIN_DESC]," = '%s'", strTmp);
	}
      }
    if (selval[USER_GROUP_NAME] == 1)
      sprintf(qval[USER_GROUP_NAME], " not like  '%%$deleted%%'");
    if (selval[DATA_NAME] == 1 && argv[1][1] != 't')
      sprintf(qval[DATA_NAME], " not like  '%%$deleted%%'");

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(3);
    }

    i = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult,L_val);
    if (i < 0)
      {
	if (i != CLI_NO_ANSWER) {
	  fprintf(stderr,"SgetU Error: %i\n", i);
	} else {
	  fprintf(stderr,"No Answer\n");
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	}
	clFinish(conn); exit(4);
      }

    show_results(conn, qval, selval, &myresult, D_case, L_val);
    fflush(stdout);
    clFinish(conn);
    exit(0);
}


