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

usage(){
      printf("Usage: modifyUser  <operationType> <UserName> <UserDomain> <value1> [<value2>]\n");
      printf("OperationType supported are: insertAuthMap\n");
}
main(int argc,
        char* argv[])
{  
  char temp1[MAX_TOKEN];
  char collname[MAX_TOKEN];
  int i;
  char *userName, *domainName, *userAuth;
  char clErrorMsg[MAX_TOKEN];
  char clientUser[MAX_TOKEN];

  if (argc != 5 && argc != 6)    {
    usage();
    exit(-1);
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
  
  if (!strcmp(argv[1],"insertAuthMap")) {
    sprintf(clientUser,"%s@%s",argv[2],argv[3]);
    i =  modify_user_info(0 , userName, clientUser, argv[4],
			  U_INSERT_AUTH_MAP, userAuth, domainName);
    if (i != 0)
      {
	printf("modifyUser insertAuthMap  Error: %i\n",i);
	close_db2_interaction(MDAS_CAT_ROLLBACK);
	exit(-1);
      }
    close_db2_interaction(MDAS_CAT_COMMIT);
    exit(0);
  }
  else if(!strcmp(argv[1],"changePassword")) {
    sprintf(clientUser,"%s@%s",argv[2],argv[3]);
    i =  modify_user_info(0 , userName, clientUser, argv[4],
			  SU_CHANGE_PASSWORD, userAuth, domainName);
    if (i != 0)
      {
	printf("modifyUser changePassword Error: %i\n",i);
	close_db2_interaction(MDAS_CAT_ROLLBACK);
	exit(-1);
      }
    close_db2_interaction(MDAS_CAT_COMMIT);
    exit(0);
  }
  else {
    usage();
    exit(-1);
    }
}


  
      
        
     


