/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasC_db2_externs.h"
#include "mdasGlobalsExtern.h"
#include "mdasPrototypes.h"
#include "mcatAdmin.h"


extern char *clGetUserName(), *clGetDomainName(), *clGetAuth();

main(int argc,
        char* argv[])
{  
  int i;
  char dgid[MAX_TOKEN];
  char *userName, *domainName, *userAuth;
  char clErrorMsg[MAX_TOKEN];

  if (argc != 3)
    {
      printf("Usage: ingestUCollection <CollectionName> <ParentCollectionName>\n");
      exit(0);
    }

    userName = clGetUserName(NULL,clErrorMsg);
    if (userName == NULL) {
        fprintf (stderr,
         "Unable to get the registrar's userName. Please configure the .MdasEnv file\n");
        exit(-1);
    }

    domainName = clGetDomainName(NULL,clErrorMsg);
    if (domainName == NULL) {
        fprintf (stderr, 
         "Unable to get the registrar's domainName. Please configure the .MdasEnv file\n");
        exit(-1);
    }

    userAuth = clGetAuth(clErrorMsg);
    if (userAuth == NULL) {
        fprintf (stderr, 
         "Unable to get the registrar's userAuth. Please configure the .MdasEnv file\n");
        exit(-1);
    }

    i = open_db2_interaction(MDAS_CATALOG);
    if (i != 0) {
      printf("Opening Error:%i\n",i);
      exit(-1);
    }
    
    ClientUser = (userInfo *) malloc (sizeof (userInfo));
    ClientUser->privUserFlag = 1;

    if (!strcmp(argv[2],"null"))
      {
	if (argv[1][0] != 'g')
	  {
	    printf("ingestCollection Error:Top level collections should be of the form g<number>\n");
	    printf("                       where <number> is between 1 and 9999.\n");
	    close_db2_interaction(MDAS_CAT_ROLLBACK);
	    exit(-1);
	  }
	strcpy(dgid,(char *)argv[1] + 1);
	i = atoi(dgid);
	if ( i == 0 || i > 9999)
	  {
	    printf("ingestCollection Error:Top level collection cannot be 'g0'\n");
	    close_db2_interaction(MDAS_CAT_ROLLBACK);
	    exit(-1);
	  }
	else if (i < 10)
	  sprintf(dgid,"000%i",i);
	else if (i < 100)
	  sprintf(dgid,"00%i",i);
	else if (i < 1000)
	  sprintf(dgid,"0%i",i);
	else 
	  sprintf(dgid,"%i",i);
	i= get_mdas_authorization(userName, userAuth,  domainName);
	if (i != 0) {
          printf("Authorization Error:%i\n",i);
          exit(-1);
	}

	sprintf(atname[0],"data_grp_id");
	sprintf(atname[1],"data_grp_name");
	sprintf(atname[2],"parent_grp_name");
	sprintf(atval[0],"'%s'",dgid);
	sprintf(atval[1],"'%s'",argv[1]);
	sprintf(atval[2],"'null'");
	i=insertIntoTable(atname,atval,"MDAS_TD_DATA_GRP",3);
	if (i != 0) 
	  {
	    printf("ingestCollection Error: %i\n",
		   MDAS_TD_DATA_GRP_INSERTION_ERROR);
	    printf("check if top-level collection of same name already exists\n");
	    close_db2_interaction(MDAS_CAT_ROLLBACK);
	    exit(-1);
	  }
	sprintf(atname[1],"USER_ID");
	sprintf(atname[2],"ACCESS_ID");
	sprintf(atval[1],"%i",
	        get_user_id_in_domain(userName, domainName));
	sprintf(atval[2],"%i",
		get_itype_value("MDAS_TD_DS_ACCS.access_constraint","all"));
	i=insertIntoTable(atname,atval,"MDAS_AD_GRP_ACCS",3);
	if (i != 0) 
	  {
	    printf("ingestCollection Error: %i\n",
		   MDAS_AD_GRP_ACCS_INSERTION_ERROR);
	    close_db2_interaction(MDAS_CAT_ROLLBACK);
	    exit(-1);
	  }
	close_db2_interaction(MDAS_CAT_COMMIT);
	exit(0);	
      }


    i = make_new_collection(0, argv[2],argv[1],
			     userName,domainName);
    if (i != 0)
      {
	printf("ingestCollection Error: %i\n",i);
	close_db2_interaction(MDAS_CAT_ROLLBACK);
	exit(-1);
      }
    close_db2_interaction(MDAS_CAT_COMMIT);
}

  
      
        
     


