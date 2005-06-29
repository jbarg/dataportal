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

#include "srbC_mdas_externs.h"
#include "mcatAdmin.h"

extern char *clGetUserName(), *clGetDomainName(), *clGetAuth();

main(int argc,
        char* argv[])
{  
  char temp1[MAX_TOKEN];
  char collname[MAX_TOKEN];
  int i;
  char *userName, *domainName, *userAuth;
  char clErrorMsg[MAX_TOKEN];
  char newuserNameDomain[MAX_TOKEN];
  if (argc != 8)
    {
      printf("Usage: ingestUser  <UserName> <UserPassword> <UserDomain> <UserType> <UserAddress> <UserPhone> <UserEmail>\n");
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

    i = register_user_info(0, argv[1],argv[2],argv[3],
			     argv[4],argv[5],argv[6],
			     argv[7],
			     userName, userAuth, domainName);
    if (i != 0)
      {
	printf("ingestUser Register Error: %i\n",i);
	close_db2_interaction(MDAS_CAT_ROLLBACK);
	exit(-1);
      }
 /**********
    sprintf(collname, "%s.%s",argv[1],argv[3]);
    i = make_new_collection(0, DEF_PARENT_COLL, collname, userName,domainName);
    if (i != 0)
      {
	printf("ingestUser: MakeCollection Error: %i\n",i);
	close_db2_interaction(MDAS_CAT_ROLLBACK);
	exit(-1);
      }
    sprintf(temp1, "%s/%s", DEF_PARENT_COLL, collname);
    i = modify_collection_info(0, userName, temp1,
			       argv[1], argv[3], "all",
			       D_INSERT_COLL_ACCS, domainName);
    if (i != 0)
      {
	printf("ingestUser: ModifyCollection Error: %i\n",i);
	close_db2_interaction(MDAS_CAT_ROLLBACK);
	exit(-1);
      }
   
    sprintf(collname, "%s.%s",argv[1],argv[3]);
    i = make_new_collection(0, DEF_CONTAINER_COLL, collname, userName,
     domainName);
    if (i != 0)
      {
        printf("ingestUser: MakeCollection Error: %i\n",i);
        close_db2_interaction(MDAS_CAT_ROLLBACK);
        exit(-1);
      }
    sprintf(temp1, "%s/%s", DEF_CONTAINER_COLL, collname);
    i = modify_collection_info(0, userName, temp1,
                               argv[1], argv[3], "all",
                               D_INSERT_COLL_ACCS, domainName);
    if (i != 0)
      {
        printf("ingestUser: ModifyCollection Error: %i\n",i);
        close_db2_interaction(MDAS_CAT_ROLLBACK);
        exit(-1);
      }
    if (!strcmp(argv[4],"sysadmin")) {
    i = modify_collection_info(0, userName, DEF_PARENT_COLL,
                               argv[1], argv[3], "all",
                               D_INSERT_COLL_ACCS, domainName);
      
    if (i != 0)
      {
        printf("ingestUser: ModifyCollection Error for %s: %i\n", DEF_PARENT_COLL,i);
        close_db2_interaction(MDAS_CAT_ROLLBACK);
        exit(-1);
      }
    }

    sprintf(newuserNameDomain,"%s@%s",argv[1],argv[3]);
    i = modify_user_info (0,userName, newuserNameDomain,argv[3],U_INSERT_GROUP,
			  "",domainName);
    if (i != 0)
      {
        printf("ingestUser: Insert User Into Doamin Group Error for: %i\n",i);
        close_db2_interaction(MDAS_CAT_ROLLBACK);
        exit(-1);
      }
*******/
    close_db2_interaction(MDAS_CAT_COMMIT);
    return(0);
}

  
      
        
     


