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
  fprintf(stderr,"SgetR [-lchdDpP] [-z mcatName] [-L n] [-Y n] [-T resourcType] [resourceName] \n");
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
    char strTmp[MAX_TOKEN];
    char *mcatName;
    int D_case = DEFAULT_FIELD_WIDTH;
    int cmpdRsrc =  0;
    int c;
    int zFlag = 0;

    strcpy(inCondition , "");
    i = initSrbClientEnv();
    if (i < 0)
      {printf("SgetR Initialization Error:%i\n",i); exit(1);}


    for (i = 0; i < MAX_DCS_NUM; i++)
      {
	sprintf(qval[i],"");
	selval[i] = 0;
      }   
    selval[RSRC_NAME] = 1;
    selval[RSRC_ADDR_NETPREFIX] = 1;
    selval[RSRC_TYP_NAME] = 1;
    selval[RSRC_DEFAULT_PATH] = 1;
    selval[PHY_RSRC_DEFAULT_PATH] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[PHY_RSRC_TYP_NAME] = 1;
    selval[RSRC_CLASS] = 1;
    selval[RSRC_OWNER_ZONE] = 1; 
    selval[RSRC_OWNER_NAME] = 1; 
    selval[RSRC_OWNER_DOMAIN] = 1; 
    while ((c=getopt(argc, argv,"lchdUDpPL:Y:T:H:z:")) != EOF) {
        switch (c) {
	     case 'p':
			selval[RSRC_NAME] = 0;
			selval[RSRC_TYP_NAME] = 0;
			selval[RSRC_DEFAULT_PATH] = 0;
			selval[RSRC_REPL_ENUM] = 0;
			break;
	     case 'P':
			selval[RSRC_NAME] = 0;
			selval[RSRC_TYP_NAME] = 0;
			selval[RSRC_DEFAULT_PATH] = 0;
			selval[RSRC_REPL_ENUM] = 0;    
			selval[RSRC_ADDR_NETPREFIX] = 0;
			selval[PHY_RSRC_DEFAULT_PATH] = 0;
			selval[PHY_RSRC_TYP_NAME] = 0;
			break;
	     case 'D':
			selval[DATA_NAME]  = 1;
	                selval[PATH_NAME] = 1;
			sprintf(qval[DATA_NAME], 
				" not like  '%%$deleted%%'");
			break;
	     case 'd':
	                selval[DATA_NAME]  = 1;
	                selval[PATH_NAME] = 1;
			selval[RSRC_ADDR_NETPREFIX] = 0;
			selval[RSRC_TYP_NAME] = 0;
			selval[RSRC_DEFAULT_PATH] = 0;
			selval[PHY_RSRC_DEFAULT_PATH] = 0;
			selval[PHY_RSRC_NAME] = 0;
			selval[PHY_RSRC_TYP_NAME] = 0;
			sprintf(qval[DATA_NAME], 
				" not like  '%%$deleted%%'");
			break;
	     case 'c':

			selval[RSRC_NAME] = 0;
			selval[RSRC_TYP_NAME] = 0;
			selval[RSRC_DEFAULT_PATH] = 0;
			selval[RSRC_REPL_ENUM] = 0;    
			selval[RSRC_ADDR_NETPREFIX] = 0;
			selval[PHY_RSRC_DEFAULT_PATH] = 0;
			selval[PHY_RSRC_TYP_NAME] = 0;

	                selval[PHY_RSRC_TYP_NAME] = 1;
			selval[RSRC_ADDR_NETPREFIX] = 1;
			selval[PHY_RSRC_NAME] = 1;
			selval[PHY_RSRC_DEFAULT_PATH] = 1;
			selval[RSRC_DEFAULT_PATH] = 1;
			selval[RSRC_CLASS] = 1;
			selval[MAX_OBJ_SIZE] = 1;
			selval[COMPOUND_RSRC_NAME] = 1;
			cmpdRsrc =  1;
			break;
	     case 'l':
			selval[RESOURCE_MAX_LATENCY]  = 1;
			selval[RESOURCE_MIN_LATENCY]  = 1;
			selval[RESOURCE_BANDWIDTH]  = 1;
			selval[RESOURCE_MAX_CONCURRENCY]  = 1;
			selval[RESOURCE_NUM_OF_HIERARCHIES]  = 1;
			selval[RESOURCE_NUM_OF_STRIPES]  = 1;
			selval[RESOURCE_CAPACITY]  = 1;
			selval[RESOURCE_DESCRIPTION]  = 1;
    			selval[FREE_SPACE]  = 1;
			selval[FS_TIMESTAMP]  = 1;
	                break;
	     case 'U':
		       selval[RSRC_ADDR_NETPREFIX] = 0;
		       selval[RSRC_TYP_NAME] = 0;
		       selval[RSRC_DEFAULT_PATH] = 0;
		       selval[PHY_RSRC_DEFAULT_PATH] = 0;
		       selval[PHY_RSRC_NAME] = 0;
		       selval[PHY_RSRC_TYP_NAME] = 0;
		       selval[RSRC_CLASS] = 0;
		       selval[RSRC_OWNER_ZONE] = 0; 
		       selval[RSRC_OWNER_NAME] = 0; 
		       selval[RSRC_OWNER_DOMAIN] = 0; 
		       selval[RSRC_ACCESS_CONSTRAINT] = 1;
		       selval[RSRC_ACCS_GRPUSER_NAME] = 1;
		       selval[RSRC_ACCS_GRPUSER_DOMAIN] = 1;
		       selval[RSRC_ACCS_GRPUSER_ZONE] = 1;
		       break;
	     case 'L':  
			L_val = atoi (optarg);
			break;
	     case 'Y':   
			D_case = atoi(optarg);
			break;
	     case 'T':
			sprintf(qval[RSRC_TYP_NAME], " = '%s'", optarg);
			break;
	     case 'H':
			sprintf (strTmp, "%s*", optarg);
			make_like_for_mdas (strTmp);
			genCollLikeSQL (strTmp, qval[RSRC_ADDR_NETPREFIX], 0);
			break;
             case 'z':
#ifdef FED_MCAT
			mcatName = strdup (optarg);
			zFlag = 1;
#endif
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

#ifdef FED_MCAT
      if (zFlag == 0) {
            char *thisMcat = NULL;

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

    if (optind == argc)
      {
	sprintf(qval[RSRC_NAME], " not like  '%%$deleted%%'");
      }
    else if (optind+1 ==  argc)
      {
	strcpy(strTmp,argv[optind]);
	make_like_for_mdas(strTmp);
	if (cmpdRsrc == 0) {
	  if (strcmp(strTmp,argv[optind]))
#ifdef MCAT_VERSION_10
	    sprintf(qval[RSRC_NAME]," like '%s %%'",strTmp);
#else
	  sprintf(qval[RSRC_NAME]," like '%s'",strTmp);
#endif
	  else
	    sprintf(qval[RSRC_NAME]," = '%s'", strTmp);
	}
	else {

	  if (strcmp(strTmp,argv[optind]))
#ifdef MCAT_VERSION_10
	    sprintf(qval[COMPOUND_RSRC_NAME]," like '%s %%'",strTmp);
#else
	  sprintf(qval[COMPOUND_RSRC_NAME]," like '%s%%'",strTmp);
#endif
	  else
	    sprintf(qval[COMPOUND_RSRC_NAME]," = '%s'", strTmp);
	}

      }
    else 
      {
	fprintf(stderr,"SgetR Usage Error \n");
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

#ifdef FED_MCAT
    i = srbGetDataDirInfoWithZone(conn, MDAS_CATALOG, mcatName, qval, selval, 
      &myresult,L_val);
#else
    i = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult,L_val);
#endif
    if (i < 0)
      {
	if (i != CLI_NO_ANSWER || i != MCAT_INQUIRE_ERROR) {
	  fprintf(stderr,"SgetR Error: %i\n", i);
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


