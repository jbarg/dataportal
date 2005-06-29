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


void
usage()
{
  fprintf(stderr,"Stoken [-L n] [-Y n] typeName [value]\n");
  fprintf(stderr," Types can be one of: Location, ResourceType, DataType, UserType, Domain, Action, AccessConstraint, Zone, ResourceClass\n");
  fprintf(stderr," value if given will be used to restrict the answer to that type-name. It can contain wild-cards. If using wild-card quote the string.\n");
}
int
main(int argc, char **argv)
{

    srbConn *conn;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    int i;
    int L_val = DEFAULT_ROW_COUNT;
    int mval = 1;
    int D_case = DEFAULT_FIELD_WIDTH;

    strcpy(inCondition , "");
    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}


    for (i = 0; i < MAX_DCS_NUM; i++)
      {
	sprintf(qval[i],"");
	selval[i] = 0;
      }   

    for (; mval < argc ; mval++)
      {
	if (argv[mval][0] == '-')
	  {
	    if (argv[mval][1] == 'L' )
	      {if (argc < mval +2) {usage(); exit(1);}
	        L_val = atoi(argv[mval+1]); mval++;}
	    else if (argv[mval][1] == 'Y' )
	      {if (argc < mval +2) {usage(); exit(1);}
	        D_case = atoi(argv[mval+1]); mval++;}
	    else if (argv[mval][1] == 'h' )
	      {
		
		usage();
		exit(0);
	      }
	    else
	      {
		fprintf(stderr,"Stoken Unknown option \n");
		usage();
		exit(1);
	      }
	  }
	else
	  break;
      }
    if (argc < mval + 1) {usage(); exit(1);}
	        
    if  (!strcmp(argv[mval],"ResourceType")) {
      if (argc > mval+1) {
	sprintf(qval[DATA_NAME],argv[mval+1]);
	if (make_like_for_mdas(qval[DATA_NAME]) == 1)
	  sprintf(qval[RSRC_TYP_NAME], " like '%s'",
		  qval[DATA_NAME]);
	else
	  genEqualSQL (qval[RSRC_TYP_NAME],argv[mval+1]);
	sprintf(qval[DATA_NAME], "");
      }
      selval[RSRC_TYP_NAME] = 1;
    }
    else if  (!strcmp(argv[mval],"ResourceClass")) {
      if (argc > mval+1) {
	sprintf(qval[DATA_NAME],argv[mval+1]);
	if (make_like_for_mdas(qval[DATA_NAME]) == 1)
	  sprintf(qval[RSRC_CLASS], " like '%s'",
		  qval[DATA_NAME]);
	else
	  genEqualSQL (qval[RSRC_CLASS],argv[mval+1]);
	sprintf(qval[DATA_NAME], "");
      }
      selval[RSRC_CLASS] = 1;
    }
    else if (!strcmp(argv[mval],"DataType")) {
      if (argc > mval+1) {
	sprintf(qval[DATA_NAME],argv[mval+1]);
	if (make_like_for_mdas(qval[DATA_NAME]) == 1)
	  sprintf(qval[DATA_TYP_NAME], " like '%s'",
		  qval[DATA_NAME]);
	else
	  genEqualSQL (qval[DATA_TYP_NAME],argv[mval+1]);
	sprintf(qval[DATA_NAME], "");
      }
      selval[DATA_TYP_NAME] = 1;
      selval[DATA_TYPE_MIME_STRING] = 1;
      selval[DATA_TYPE_EXTENDERS] = 1;
    }
    else if (!strcmp(argv[mval],"UserType")) {
      if (argc > mval+1) {
	sprintf(qval[DATA_NAME],argv[mval+1]);
	if (make_like_for_mdas(qval[DATA_NAME]) == 1)
	  sprintf(qval[USER_TYP_NAME], " like '%s'",
		  qval[DATA_NAME]);
	else
	  genEqualSQL (qval[USER_TYP_NAME],argv[mval+1]);
	sprintf(qval[DATA_NAME], "");
      }
      selval[USER_TYP_NAME] = 1;
    }
    else if (!strcmp(argv[mval],"Domain")) {
      if (argc > mval+1) {
	sprintf(qval[DATA_NAME],argv[mval+1]);
	if (make_like_for_mdas(qval[DATA_NAME]) == 1)
	  sprintf(qval[DOMAIN_DESC], " like '%s'",
		  qval[DATA_NAME]);
	else
	  genEqualSQL (qval[DOMAIN_DESC],argv[mval+1]);
	sprintf(qval[DATA_NAME], "");
      }
      selval[DOMAIN_DESC] = 1;
    }
    else if (!strcmp(argv[mval],"Location")) {
      if (argc > mval+1) {
	sprintf(qval[DATA_NAME],argv[mval+1]);
	if (make_like_for_mdas(qval[DATA_NAME]) == 1)
	  sprintf(qval[LOCATION_NAME], " like '%s'",
		  qval[DATA_NAME]);
	else
	  genEqualSQL (qval[LOCATION_NAME],argv[mval+1]);
	sprintf(qval[DATA_NAME], "");
      }
      selval[LOCATION_NAME] = 1;
      selval[RSRC_ADDR_NETPREFIX] = 1;
    }
    else if (!strcmp(argv[mval],"Action")) {
      if (argc > mval+1) {
	sprintf(qval[DATA_NAME],argv[mval+1]);
	if (make_like_for_mdas(qval[DATA_NAME]) == 1)
	  sprintf(qval[AUDIT_ACTION_DESC], " like '%s'",
		  qval[DATA_NAME]);
	else
	  genEqualSQL (qval[AUDIT_ACTION_DESC],argv[mval+1]);
	sprintf(qval[DATA_NAME], "");
      }
      selval[AUDIT_ACTION_DESC] = 1;
    }
    else if (!strcmp(argv[mval],"AccessConstraint")) {
      if (argc > mval+1) {
	sprintf(qval[DATA_NAME],argv[mval+1]);
	if (make_like_for_mdas(qval[DATA_NAME]) == 1)
	  sprintf(qval[ACCESS_CONSTRAINT], " like '%s'",
		  qval[DATA_NAME]);
	else
	  genEqualSQL (qval[ACCESS_CONSTRAINT],argv[mval+1]);
	sprintf(qval[DATA_NAME], "");
      }
      selval[ACCESS_CONSTRAINT] = 1;
    }
    else if (!strcmp(argv[mval],"Zone")) {
      if (argc > mval+1) {
	sprintf(qval[DATA_NAME],argv[mval+1]);
	if (make_like_for_mdas(qval[DATA_NAME]) == 1)
	  sprintf(qval[ZONE_NAME], " like '%s'",
		  qval[DATA_NAME]);
	else
	  genEqualSQL (qval[ZONE_NAME],argv[mval+1]);
	sprintf(qval[DATA_NAME], "");
      }

      selval[ZONE_NAME] = 1;
      selval[ZONE_LOCALITY] = 1;
      selval[ZONE_LOCN_DESC] = 1;
      selval[ZONE_NETPREFIX] = 1;
      selval[ZONE_PORT_NUM] = 1;
      selval[ZONE_ADMIN_AUTH_SCHEME_NAME] = 1;
      selval[ZONE_ADMIN_DISTIN_NAME] = 1;
      selval[ZONE_STATUS] = 1;
      selval[ZONE_CREATE_DATE] = 1;
      selval[ZONE_MODIFY_DATE] = 1;
      selval[ZONE_COMMENTS] = 1;
      selval[ZONE_CONTACT] = 1;
      selval[ZONE_ADMIN_NAME] = 1;
      selval[ZONE_ADMIN_DOMAIN_NAME] = 1;
    }
    else {
      fprintf(stderr,"Stoken Unknown option \n");
      usage();
      exit(1);
    }

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }
    
    i = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult, L_val);
    if (i < 0)
      {
	if (i != CLI_NO_ANSWER) {
	  fprintf(stderr,"Stoken Error: %i\n", i);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	} else fprintf(stderr,"No Answer\n");
	clFinish(conn); exit(4);
      }
    show_results(conn, qval, selval, &myresult, D_case, L_val);
    clFinish(conn);
    exit(0);
}


