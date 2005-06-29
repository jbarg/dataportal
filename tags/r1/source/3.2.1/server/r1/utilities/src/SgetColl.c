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
    int aFlag = 0;

    int D_case = 0;
    int I_case = 0;
   char collname[MAX_TOKEN];
    char dataname[MAX_TOKEN];

int
getColl_info(int argc, char argv[][MAX_OBJ_NAME_LENGTH], int L_val)
{
  int i,j;
  int first = 0;
  /* for (i= 1; i < argc ; i++) printf("A:%i:%s\n",i,argv[i]);*/
  for (i= 1; i < argc ; i++)
    {
      if (argv[i][strlen(argv[i])-1] ==  '/')
	argv[i][strlen(argv[i])-1] = '\0';
      if (!aFlag) {
	if (make_like_for_mdas(argv[i]) == 1)
#ifdef MCAT_VERSION_10
	  sprintf(qval[COLLECTION_NAME]," like '%s   %%' ESCAPE '\\'",argv[i]);
#else
	  sprintf(qval[COLLECTION_NAME]," like '%s%%' ESCAPE '\\'",argv[i]);
#endif
	else
	  genEqualSQL (qval[COLLECTION_NAME],argv[i]);
      }
      else {
	if (make_like_for_mdas(argv[i]) == 1)
	  sprintf(qval[ACCESS_COLLECTION_NAME],
#ifdef MCAT_VERSION_10
		  " like '%s   %%' ESCAPE '\\'",argv[i]);
#else
		  " like '%s%%' ESCAPE '\\'",argv[i]);
#endif
	else
	  genEqualSQL (qval[ACCESS_COLLECTION_NAME],argv[i]);
      }
   
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
	first = show_addl_results(conn, qval,selval, &myresult, D_case,first, L_val);
      }
    }
  /*printf("%i items displayed\n",first);*/
 return(0);
}

void
usage()
{
  fprintf(stderr,"SgetColl [-ahc] [-I]  [-W|-U username] [-Y n] [-L n] [collectionName]\n");
}
int
main(int argc, char **argv)
{
    int  newargc;
    char newargv[MAX_TOKEN/4][MAX_OBJ_NAME_LENGTH];
    int  darg;
    int L_val = DEFAULT_ROW_COUNT;
    int mval = 1;
    int i,j;
    int X_val =0;
    strcpy(inCondition , "");
    i = initSrbClientEnv();
    if (i < 0)
      {printf("SgetColl Initialization Error:%i\n",i);     fflush(stdout);exit(1);}

    for (i = 0; i < MAX_DCS_NUM; i++)
      {
	sprintf(qval[i],"");
	selval[i] = 0;
      }   

    I_case = 0;
    D_case = DEFAULT_FIELD_WIDTH;
    selval[COLLECTION_NAME] = 1;
    darg = 1;
    myself = -1;    
    if (argc == 1)
      {

      }
    else if (argv[1][0] == '-')
      {
	darg++;
	mval = 2;
	switch (argv[1][1])
	  {
	     case 'a':
	                selval[ACCESS_COLLECTION_NAME] = 1;
	                selval[COLLECTION_NAME] = 0;
			selval[COLLECTION_ACCESS_CONSTRAINT] = 1;
			selval[USER_NAME] = 1;
	                sprintf(qval[USER_NAME], " = '%s'", srbUser);
			aFlag = 1;
			break;
	     case 'c':
	                selval[CONTAINER_FOR_COLLECTION] = 1;
			break;

	     case 'H':	                
			selval[DATA_NAME] = 1;
			sprintf(qval[DATA_HIDE], "= 1");
	                sprintf(qval[DATA_OWNER], "= '%s'", srbUser);
			break;
	     case 'I':	                
	                I_case = 1;
			break;
	     case 'Y':
			D_case = atoi(argv[2]);
			myself = 2;
			darg++; mval++;
			break;
	     case 'W':
	                myself = 0;
			sprintf(qval[USER_NAME], "");
			break;
	     case 'U':
	                if (argc < 3) {usage(); exit(1);}
			sprintf(qval[USER_NAME], " = '%s'", argv[2]);
			myself = 2;
			darg++;  mval++;
			break;
	     case 'L': 
    	                if (argc < 3) {usage(); exit(1);}
			L_val = atoi(argv[2]);
			myself = 2;
			darg++;  mval++;
			break;
	     case 'x': 
	                X_val = 1;
			break;
	     case 'h': 
	                usage();
			exit(0);
			break;
             default :
	                fprintf(stderr,"SgetColl Unknown option \n");
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
	    else if (argv[mval][1] == 'W')
	      {
		myself = 0;
		sprintf(qval[USER_NAME], "");
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
		fprintf(stderr,"SgetColl Unknown option \n");
		usage();
			exit(1);
	      }
	  }
	else 
	  break;
      }
	    


    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    if (X_val == 1) {
      if (argc  == 4)
	i = srbGetDatasetInfo(conn, MDAS_CATALOG,argv[argc-1],argv[argc-2], 
			    &myresult, L_val, "read");
      else
	i = srbGetDatasetInfo(conn, MDAS_CATALOG, "",argv[argc-1], 
			    &myresult, L_val, "read");

      if (i < 0)
	{
	  if (i != CLI_NO_ANSWER) {
	    fprintf(stderr,"SgetColl Error: %i\n", i);
	    srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  } else {
	    fprintf(stderr,"No Answer\n");    fflush(stdout);
	  }
	  clFinish(conn); exit(4);
	}
      show_results_simple(conn,  &myresult, L_val, 0);
      clFinish(conn);
      fflush(stdout);
      exit(0);
    }
    if (argc > 1 && argv[argc -1][0] !=  '-' && argc > (myself+1))
      {
	new_expand_file_names_for_like(conn, mval, argc, argv, &newargc,newargv);
	getColl_info(newargc,newargv, L_val);
	clFinish(conn);    fflush(stdout);
	exit(0);
      }

    if (!I_case) {
      if (!aFlag)
	sprintf(qval[COLLECTION_NAME]," = '%s'",mdasCollectionName);
      else 
	sprintf(qval[ACCESS_COLLECTION_NAME]," = '%s'",mdasCollectionName);
    }

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
	  fprintf(stderr,"SgetColl Error: %i\n", i);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	} else {
	  fprintf(stderr,"No Answer\n");    fflush(stdout);
	}
	clFinish(conn); exit(4);
      }
    show_results(conn, qval, selval, &myresult, D_case, L_val);

    clFinish(conn);
    fflush(stdout);
    exit(0);
}


