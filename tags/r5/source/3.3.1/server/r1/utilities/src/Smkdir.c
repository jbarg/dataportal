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
usage(char *prog)
{
fprintf(stderr,"Usage: %s [-N] [-c container] collection\n",prog);
fprintf(stderr,"Usage: %s -p [-N] collection\n",prog);
}

int
main(int argc, char **argv)
{
  srbConn *conn;
  char parcollname[MAX_TOKEN], collname[MAX_TOKEN];
  char *contPtr;
  char container[MAX_TOKEN];
  char contName[MAX_TOKEN], contCollection[MAX_TOKEN];
  char temp[MAX_TOKEN], temp1[MAX_TOKEN], temp2[MAX_TOKEN];
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  mdasC_sql_result_struct  myresult;
  int N_val = 0;
  int i, c;
  int cFlag = 0;
  int pFlag = 0;
  char *mcatName;
#ifdef FED_MCAT
  char myMcatName[MAX_TOKEN];
#endif


    strcpy(inCondition , "");
    while ((c=getopt(argc, argv,"Nhpc:")) != EOF) {
        switch (c) {
            case 'h':
		usage (argv[0]);
                exit (0);
	    case 'N':
		N_val = 1;
		break;
            case 'p':
	        pFlag = 1;
		break;
            case 'c':
                cFlag = 1;
                strcpy (container, optarg);
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }
    if (argc == optind) {        /* No input collection  */
      usage (argv[0]);
      exit(1);
    }

    i = initSrbClientEnv();
    if (i < 0) {
      printf("Smkdir: Initialization Error:%i\n",i); 
      exit(1);
    }

    sprintf(temp,"%s/alpha",argv[argc-1]);
    splitpath(temp, temp1, temp2);
    clSplitbychar(temp1, parcollname, collname,'/');
    if (strlen(parcollname) == 0)
      strcpy(parcollname,"/");

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Smkdir: Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    if (pFlag) {
      if (N_val)
	N_val = 0;
      N_val = 1;
      i = createCollectRecur (conn, MDAS_CATALOG,"/", temp1,N_val);
      if (i < 0) {
        fprintf(stderr,"Smkdir: Error for collection '%s': %i\n", temp1,i);
        srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(4);
      }
      clFinish(conn);
      exit(0);
    }

    i = srbCreateCollect(conn, MDAS_CATALOG, parcollname, collname);
    if (i < 0)
      {
	fprintf(stderr,"Smkdir: Error for collection '%s': %i\n", temp1,i);
	srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(4);	
      }

    if (cFlag) { /* container for collections */
      char *userName, *domainName;

      if (conn->clientUser == NULL) {
          userName = conn->proxyUser->userName;
          domainName = conn->proxyUser->domainName;
      } else {
          userName = conn->clientUser->userName;
          domainName = conn->clientUser->domainName;
     }
#ifdef FED_MCAT
     if (container[0] != '/') {    /* full path */
	int status;
        char *thisMcat = NULL;

        thisMcat = getmcatNamefromHint (temp1);
        if (thisMcat == NULL) {
            status =srbGetMcatZone (conn, userName, domainName,
             myMcatName);
            if (status >= 0)
                mcatName = myMcatName;
            else
                mcatName = NULL;
        } else if (strcmp (thisMcat, "home") == 0 ||
         strcmp (thisMcat, "container") == 0) {
            mcatName = NULL;
        } else {
            mcatName = thisMcat;
        }
     } else {
         mcatName = NULL;
     }
#else
     mcatName = NULL;
#endif

      parseContainerName (container, contName, contCollection,
       userName, domainName, mcatName);
      sprintf (container, "%s/%s", contCollection, contName);
      sprintf (temp, "%s/%s", parcollname, collname);

      i = srbModifyCollect(conn, MDAS_CATALOG, temp,
                           container,"","",
                           C_INSERT_CONTAINER_FOR_COLLECTION);

      if (i < 0)  {
        fprintf(stderr,"SmodColl: Error for collection '%s': %i\n",
              temp, i);
        srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      }
      clFinish(conn); exit(5);     
    }

    /*** June 5 - added container to new collection ***/
    if (!N_val) {
      for (i = 0; i < MAX_DCS_NUM; i++)
	{
	  sprintf(qval[i],"");
	  selval[i] = 0;
	}   
      selval[CONTAINER_FOR_COLLECTION] = 1;
      sprintf(qval[COLLECTION_NAME]," = '%s'",parcollname);
      i = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult,40);
      if (i < 0) {
	if (i != MCAT_INQUIRE_ERROR) {
	  fprintf(stderr,"Smkdir: Parent Container Query Error: %i\n", i);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  clFinish(conn); exit(6);	
	} 
	else {
	  clFinish(conn);
	  exit(0);
	}
      }
      else {
	contPtr = (char *) getFromResultStruct
	  ((mdasC_sql_result_struct *) &myresult,
	   dcs_tname[CONTAINER_FOR_COLLECTION], 
	   dcs_aname[CONTAINER_FOR_COLLECTION]);
	i = srbModifyCollect(conn, MDAS_CATALOG,temp1,
			     contPtr,"","",
			     C_INSERT_CONTAINER_FOR_COLLECTION);
	if (i < 0)  {
	  fprintf(stderr,"Smkdir: Error when associating container for collection '%s': %i\n", 
		  collname,i);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  clFinish(conn); exit(7);	
	}
      }
    }
    clFinish(conn);
    exit(0);
}

