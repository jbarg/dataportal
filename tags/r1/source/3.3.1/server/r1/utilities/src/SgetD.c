/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*
Version1.1
  Patch 1: 04/01/98 Raja -  added has_like_chars call inorder to reduce
                            use of ESCAPE
*/

#include "scommands.h"

#include "srbSecureComm.h"

extern char srbAuth[];
extern char srbHost[];
extern char srbUser[];
extern char mdasCollectionName[];
extern char inCondition[];

    srbConn *conn;
    int myself;
    
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    int alive;
    char *rval[MAX_DCS_NUM];
    char dname[MAX_TOKEN];
    
    int D_case = 0;
    int I_case = 0;
   char collname[MAX_TOKEN];
    char dataname[MAX_TOKEN];

#if defined(SRB_SECURE_COMM)
void
decryptResults()
{
  int i;
  for (i = 0; i < myresult.result_count ; i++){
    if (strcmp(myresult.sqlresult[i].att_name,"dencrypted")==0) {
      char *cp;
      int j;
      char *cp_value;
      cp_value = (char *)myresult.sqlresult[i].values;
      for (j=0;j<myresult.row_count;j++) {
        cp = sscDecryptString(cp_value);
        strncpy(cp_value, cp, MAX_DATA_SIZE);
        free(cp);
        cp_value += MAX_DATA_SIZE;
      }
    }
  }
}
#endif

int
getD_info(int argc, char argv[][MAX_OBJ_NAME_LENGTH], int L_val)
{
  int i,j;
  int first = 0;
  /* for (i= 1; i < argc ; i++) printf("A:%i:%s\n",i,argv[i]);*/
  for (i= 1; i < argc ; i++)
    {
      clSplitbychar(argv[i],collname,dataname, '/');
      if (make_like_for_mdas(dataname) == 1)
#ifdef MCAT_VERSION_10
	sprintf(qval[DATA_NAME]," like '%s   %%' ESCAPE '\\'",dataname);
#else
	sprintf(qval[DATA_NAME]," like '%s%%' ESCAPE '\\'",dataname);
#endif
      else
	genEqualSQL (qval[DATA_NAME],dataname);
      if (!I_case)
	{
	  if (make_like_for_mdas(collname) == 1)
#ifdef MCAT_VERSION_10
	    sprintf(qval[DATA_GRP_NAME]," like '%s   %%' ESCAPE '\\'",collname);
#else
	    sprintf(qval[DATA_GRP_NAME]," like '%s%%' ESCAPE '\\'",collname);
#endif
	  else
	    genEqualSQL (qval[DATA_GRP_NAME], collname);
	}

      if (strlen(inCondition) > 0) 
	fillQvalWithCondition(inCondition, qval);
      j = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult, L_val);
    if (j < 0)
      {
	/* if (j != CLI_NO_ANSWER)
	  printf("SgetD Error for %s: %i\n",argv[i], j);
	  else printf("No Answer for %s\n",argv[i]); */	
	/** free_result_struct(selval,&myresult); **/
      }
    else
      {
#if defined(SRB_SECURE_COMM)
        decryptResults();
#endif
	first = show_addl_results(conn, qval,selval, &myresult, D_case,first, L_val);
      }
    }
  /*printf("%i items displayed\n",first);*/
 return(0);
}

void
usage()
{
  fprintf(stderr,"SgetD [-phPrRefagsdDtTc] [-I] [-W | -U userName] [-Y n] [-L n] [-P dataType]  [-A condition]  [-k YYYY-MM-DD-HH.MM] [dataName]\n");
}
int
main(int argc, char **argv)
{
    int  newargc;
    /* char newargv[MAX_TOKEN][MAX_OBJ_NAME_LENGTH]; */
    char *newargv;
    int  darg;
    int L_val = DEFAULT_ROW_COUNT;
    int mval = 1;
    int i;
    int VFlag = 0;

    strcpy(inCondition , "");
    i = initSrbClientEnv();
    if (i < 0)
      {printf("SgetD Initialization Error:%i\n",i);     fflush(stdout);exit(1);}

    newargv = malloc (MAX_TOKEN * MAX_OBJ_NAME_LENGTH);

    for (i = 0; i < MAX_DCS_NUM; i++)
      {
	sprintf(qval[i],"");
	selval[i] = 0;
      }   
    I_case = 0;
    D_case = DEFAULT_FIELD_WIDTH;
    darg = 1;
    selval[DATA_NAME] = 1;
    selval[PATH_NAME] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[DATA_TYP_NAME] = 1;
    selval[DATA_GRP_NAME] = 1;
    selval[OFFSET] = 1;
    selval[SIZE] = 1;
    selval[CONTAINER_NAME] = 1;
    selval[IS_DIRTY] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[DATA_VER_NUM] = 1;
    sprintf(qval[ORDERBY],"DATA_TYP_NAME,  DATA_NAME");

    myself = -1;    
    if (argc == 1)
      {

      }
    else if (argv[1][0] == '-')
      {
	darg++;
	selval[PATH_NAME] = 0;
	selval[PHY_RSRC_NAME] = 0;
	selval[DATA_GRP_NAME] = 0;
	selval[DATA_TYP_NAME] = 0;
	sprintf(qval[ORDERBY],"");
        selval[OFFSET] = 0;
        selval[SIZE] = 0;
	mval = 2;
	switch (argv[1][1])
	  {
	     case 'p':
	                selval[DATA_GRP_NAME] = 1;
			selval[DATA_TYP_NAME] = 1;
			selval[DATA_CREATE_TIMESTAMP] = 1;
	                selval[REPL_TIMESTAMP] = 1;
	                selval[SIZE] = 1;
			break;
	     case 'T':
	                selval[DATA_GRP_NAME] = 1;
			selval[DATA_TYP_NAME] = 1;
	                selval[REPL_TIMESTAMP] = 1;
			sprintf(qval[USERDEFFUNC01]," length(SRBATTR[DATA_NAME]) < 5 ");
			selval[NONDISTINCT] = 1;
	                selval[SIZE] = 1;
			break;
	     case 'e':
	                selval[DATA_ID] = 1;
	                selval[DATA_GRP_NAME] = 1;
			selval[DATA_TYP_NAME] = 1;
	                selval[REPL_TIMESTAMP] = 1;
			selval[OFFSET] = 1;
			selval[SIZE] = 1;
			selval[DATA_HIDE] = 1;
			selval[DATA_CHECKSUM] = 1;
			selval[DATA_LOCK_DESC] = 1;
			selval[DATA_LOCK_EXPIRY] = 1;
			selval[DATA_LOCK_OWNER_NAME] = 1;
			selval[DATA_LOCK_OWNER_DOMAIN] = 1;
			selval[DATA_VER_NUM] = 1;
			break;
	     case 'f':
	                selval[CONTAINER_NAME] = 0;
			selval[IS_DIRTY] = 0;
			selval[DATA_REPL_ENUM] = 0;
			selval[DATA_VER_NUM] = 0;
	                selval[DATA_ID] = 1;
	                selval[DATA_GRP_NAME] = 1;
			selval[DATA_PIN_VAL] = 1;
			selval[DATA_PIN_OWNER_NAME] = 1;
			selval[DATA_PIN_OWNER_DOMAIN] = 1;
			selval[DATA_PIN_EXPIRY] = 1;
			selval[DATA_EXPIRY_DATE] = 1;
			selval[DATA_EXPIRE_DATE_2] = 1;
			selval[DATA_IS_COMPRESSED] = 1;
			selval[DATA_IS_ENCRYPTED] = 1;
			break;
	     case 'g':
	                selval[CONTAINER_NAME] = 0;
			selval[IS_DIRTY] = 0;
			selval[DATA_REPL_ENUM] = 0;
			selval[DATA_VER_NUM] = 0;
	                selval[DATA_GRP_NAME] = 1;
	                selval[GUID] = 1;
	                selval[GUID_FLAG] = 1;
	                selval[GUID_TIME_STAMP] = 1;
			break;
	     case 't':
			selval[CONTAINER_NAME] = 0;
			selval[DATA_REPL_ENUM] = 1;
			selval[DATA_SEG_NUM] = 1;
			selval[INT_REPL_ENUM] = 1;
			selval[INT_SEG_NUM] = 1;
			selval[INT_PATH_NAME] = 1;
			selval[INT_RSRC_NAME] = 1;
			selval[INT_RSRC_TYP_NAME] = 1;
			selval[INT_IS_DIRTY] = 1;
			selval[INT_OFFSET] = 1;
			selval[INT_SIZE] = 1;
			break;
	     case 'P':
	                if (argc < 3) {usage(); exit(1);}
	                selval[PATH_NAME] = 1;
			selval[PHY_RSRC_NAME] = 1;
			selval[DATA_GRP_NAME] = 1;
			selval[DATA_TYP_NAME] = 1;
			sprintf(qval[DATA_TYP_NAME], " = '%s'", argv[2]);
			myself = 2;
			darg++; mval++;
			break;

	     case 'r':
	                selval[PATH_NAME] = 1;
	                selval[PHY_RSRC_NAME] = 1;
			selval[DATA_TYP_NAME] = 1;
			selval[DATA_GRP_NAME] = 1;
			break;
	     case 'R':
	                selval[DATA_ID] = 1;
	                selval[PATH_NAME] = 1;
	                selval[RSRC_NAME] = 1;
			selval[DATA_GRP_NAME] = 1;
			selval[RSRC_ADDR_NETPREFIX] = 1;
			selval[RSRC_TYP_NAME] = 1;
			selval[DATA_TYP_NAME] = 1;
			break;
	     case 'a':
	                selval[ACCESS_CONSTRAINT] = 1;
			selval[USER_NAME] = 1;
			break;
	     case 'k':
	                sprintf(qval[REPL_TIMESTAMP], " >= '%s'", argv[2]);
	                selval[RSRC_ADDR_NETPREFIX] = 1;
	                selval[CONTAINER_NAME] = 0;
			selval[IS_DIRTY] = 0;
			selval[DATA_GRP_NAME] = 1;
			myself = 2;
			darg++; mval++;
			break;
	     case 's':
	                selval[DATA_ID] = 1;
	                selval[ACCESS_CONSTRAINT] = 1;
			selval[USER_NAME] = 1;
			selval[PATH_NAME] = 1;
			selval[PHY_RSRC_NAME] = 1;
			selval[DATA_GRP_NAME] = 1;
			break;
	     case 'd':
	     case 'D':
	                selval[CONTAINER_NAME] = 0;
			selval[IS_DIRTY] = 0;
			selval[DATA_REPL_ENUM] = 0;
			selval[DATA_VER_NUM] = 0;
	                selval[AUDIT_USER] = 1;
			selval[AUDIT_USER_DOMAIN] = 1;
	                selval[NEW_AUDIT_ACTION_DESC] = 1;
	                selval[NEW_AUDIT_TIMESTAMP] = 1;
			selval[NEW_AUDIT_COMMENTS] = 1;
			break;
			/**	     case 'D':
	                selval[DATA_ID] = 1;
	                selval[AUDIT_USER] = 1;
			selval[AUDIT_USER_DOMAIN] = 1;
	                selval[AUDIT_ACTION_DESC] = 1;
	                selval[AUDIT_TIMESTAMP] = 1;
			selval[AUDIT_COMMENTS] = 1;
			selval[PATH_NAME] = 1;
			selval[PHY_RSRC_NAME] = 1;
			selval[DATA_TYP_NAME] = 1;
			selval[DATA_GRP_NAME] = 1;
			break; ***/
	     case 'c':
	                selval[DATA_GRP_NAME] = 1;
			selval[DATA_COMMENTS] = 1;
			break;
	     case 'C':
	                selval[CONTAINER_NAME] = 0;
			selval[IS_DIRTY] = 0;
			selval[DATA_REPL_ENUM] = 0;
			selval[DATA_VER_NUM] = 0;
	                selval[DATA_GRP_NAME] = 1;
			selval[DATA_COMMENTS] = 1;
			break;
	     case 'W':
	                selval[PATH_NAME] = 1;
			selval[DATA_ID] = 1;
			selval[PHY_RSRC_NAME] = 1;
			selval[DATA_GRP_NAME] = 1;
			selval[DATA_TYP_NAME] = 1;
			selval[SIZE] = 1;
	                myself = 0;
			break;
	     case 'I':	 
	       /*               
			selval[PHY_RSRC_NAME] = 1;
			selval[PATH_NAME] = 1;
			selval[DATA_GRP_NAME] = 1;
			selval[DATA_ID] = 1;
			selval[SIZE] = 1;
	       */
	                I_case = 1;
			break;
	     case 'Y':
	                if (argc < 3) {usage(); exit(1);}
			selval[PATH_NAME] = 1;
			selval[PHY_RSRC_NAME] = 1;
			selval[DATA_GRP_NAME] = 1;
			selval[DATA_TYP_NAME] = 1;
			selval[SIZE] = 1;
			D_case = atoi(argv[2]);
			myself = 2;
			darg++; mval++;
			break;
	     case 'A':
	                if (argc < 3) {usage(); exit(1);}
			selval[PATH_NAME] = 1;
			selval[PHY_RSRC_NAME] = 1;
			selval[DATA_GRP_NAME] = 1;
			selval[DATA_TYP_NAME] = 1;
			selval[SIZE] = 1;
			strcpy(inCondition,argv[2]);
			myself = 2;
			darg++; mval++;
			break;
	     case 'U':
	                if (argc < 3) {usage(); exit(1);}
			selval[PATH_NAME] = 1;
			selval[PHY_RSRC_NAME] = 1;
			selval[DATA_GRP_NAME] = 1;
			selval[DATA_TYP_NAME] = 1;
			selval[SIZE] = 1;
			sprintf(qval[USER_NAME], " = '%s'", argv[2]);
			myself = 2;
			darg++;  mval++;
			break;
	     case 'L': 
    	                if (argc < 3) {usage(); exit(1);}
			selval[PATH_NAME] = 1;
			selval[PHY_RSRC_NAME] = 1;
			selval[DATA_GRP_NAME] = 1;
			selval[DATA_TYP_NAME] = 1;
			selval[SIZE] = 1;
			L_val = atoi(argv[2]);
			myself = 2;
			darg++;  mval++;
			break;
	     case 'V': 
        		selval[PATH_NAME] = 1;
        		selval[PHY_RSRC_NAME] = 1;
        		selval[DATA_GRP_NAME] = 1;
        		selval[DATA_TYP_NAME] = 1;
        		sprintf(qval[ORDERBY],"");
        		selval[OFFSET] = 1;
        		selval[SIZE] = 1;
			VFlag = 1;
			break;
	     case 'h': 
	                usage();
			exit(0);
			break;
             default :
	                fprintf(stderr,"SgetD Unknown option \n");
			usage();
			exit(1);
	  }
      }

    for (; mval < argc ; mval++)
      {
	if (argv[mval][0] == '-')
	  {
	    if (argv[mval][1] == 'U')

	      { if (argc < mval +2) {usage(); exit(1);}
		sprintf(qval[USER_NAME], " = '%s'", argv[++mval]);
		myself = 3;
		darg++;
	      }
	    else if (argv[mval][1] == 'P')
	      { if (argc < mval +2) {usage(); exit(1);}
		sprintf(qval[DATA_TYP_NAME], " = '%s'", argv[++mval]);
		myself = 3;
		darg++;
	      }
	     else if (argv[mval][1] == 'k')
	       {
		 sprintf(qval[REPL_TIMESTAMP], " >= '%s'", argv[++mval]);
		 selval[CONTAINER_NAME] = 0;
		 selval[RSRC_ADDR_NETPREFIX] = 1;
		 selval[IS_DIRTY] = 0;
		 selval[DATA_GRP_NAME] = 1;
		 myself = 3;
		 darg++; 
	       }
	    else if (argv[mval][1] == 'W')
	      {
		myself = 0;
		darg++;
	      }
	    else if (argv[mval][1] == 'I')
	      {
		I_case = 1;
		darg++;
	      }
	    else if (argv[mval][1] == 'Y')
	      { if (argc < mval +2) {usage(); exit(1);}
		D_case = atoi(argv[++mval]);
		myself = 3;
		darg++;
	      }
	    else if (argv[mval][1] == 'L')
	      { if (argc < mval +2) {usage(); exit(1);}
		L_val= atoi(argv[++mval]);
		myself = 3;
		darg++;
	      }
	    else if (argv[mval][1] == 'A')
	      { if (argc < mval +2) {usage(); exit(1);}
		strcpy(inCondition,argv[++mval]);
		myself = 3;
		darg++;
	      }
	    else 
	      {
		fprintf(stderr,"SgetD Unknown option \n");
		usage();
			exit(1);
	      }
	  }
	else 
	  break;
      }
	    


    if (myself == -1 && VFlag == 0)
      {
	sprintf(qval[USER_NAME], " = '%s'", srbUser);
      }
    if (myself == 0)
      {
	sprintf(qval[USER_NAME], " not like  '%%$deleted%%'");
      }

    

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(3);
    }

    if (selval[DATA_IS_ENCRYPTED] == 1) {
#if defined(SRB_SECURE_COMM)
      char pubKey[1000];
      int i1, i;
      char *cp;

      i1 = srbSetupSessionPublicKey(conn, pubKey);
      if (i1 < 0 ) {
        fprintf(stderr,"srbSetupSessionPublicKey error %d\n",i);
	clFinish(conn); exit(4);     
      }
      i = sscInitPublicKey(pubKey);
      if (i != SRB_E_NOERROR) {
        fprintf(stderr,"sscInitPublicKey error %d\n",i);
	pubKey[i1]='\0';
	fprintf(stderr,
           "Server public key or message from srbSetupSessionPublicKey: %s\n",
	   pubKey);
	clFinish(conn); exit(5);
      }
      cp = sscSetupSession();
      i = srbSetupSession(conn, cp);
      if (i != SRB_E_NOERROR) {
        fprintf(stderr,"srbSetupSession error %d\n",i);
	clFinish(conn); exit(6); 
      }
      free(cp);
#else
      /* Go ahead and request the field without using secure comm,
         the Server will protect the transfer of the field, returning
         a set string instead of the value */
#endif
    }

    /*printf("AAA:%i:%i:%s,%s\n",myself,argc,argv[argc -1],qval[USER_NAME]);*/
    if (argc > 1 && argv[argc -1][0] !=  '-' && argc > (myself+1))
      {
	/*	expand_file_names_for_like(conn, darg, argc, argv, &newargc,newargv);*/
	new_expand_file_names_for_like(conn, mval, argc, argv, 
	 &newargc, (char (*) [MAX_OBJ_NAME_LENGTH]) newargv);
	getD_info(newargc, (char (*) [MAX_OBJ_NAME_LENGTH]) newargv, L_val);
	clFinish(conn);    fflush(stdout);
	exit(0);
      }

    /*      sprintf(qval[DATA_NAME]," not like '%%$deleted%%'"); */
      if (!I_case)
	sprintf(qval[DATA_GRP_NAME]," = '%s'",mdasCollectionName);
      if (strlen(inCondition) > 0) {
	i = fillQvalWithCondition(inCondition, qval);
	if (i != 0) {
	  fprintf(stderr,"SgetD Error: %i\n", i);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  clFinish(conn); 
	  exit(1);
	}
      }
    i = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult, L_val);
    if (i < 0)
      {
	if (i != CLI_NO_ANSWER) {
	  fprintf(stderr,"SgetD Error: %i\n", i);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	} else {
	  fprintf(stderr,"No Answer\n");    fflush(stdout);
	}
	clFinish(conn); exit(7);
      }

#if defined(SRB_SECURE_COMM)
    decryptResults();
#endif
    show_results(conn, qval, selval, &myresult, D_case, L_val);

    clFinish(conn);
    fflush(stdout);
    exit(0);
}


