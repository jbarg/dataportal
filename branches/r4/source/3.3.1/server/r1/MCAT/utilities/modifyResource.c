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
      printf("Error in Usage\n");
      printf("Usage: modifyResource  <operationType> <ResourceName>  <value1> [<value2> <value3> <value4>]\n");
      printf("OperationType supported are:\n");
      printf("   insertAccess with <value1>=user@domain <value2>=accessPermission eg. 'read','write','all'\n");
      printf("   deleteAccess with <value1>=user@domain\n");
      printf("   changeOwner  with <value1>=user@domain\n");
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

  if (argc < 4) {
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

  if (!strcmp(argv[1],"insertAccess")) {
    if (argc != 5) {    
      usage();
      close_db2_interaction(MDAS_CAT_ROLLBACK);
      exit(-1);
    }
    i =  modify_resource_info(MDAS_CATALOG , argv[2], userName, domainName,
			      R_INSERT_ACCS, argv[3],argv[4],"","");
  }
  else if (!strcmp(argv[1],"deleteAccess")) {
    if (argc != 4) {    
      usage();
      close_db2_interaction(MDAS_CAT_ROLLBACK);
      exit(-1);
    }
    i =  modify_resource_info(MDAS_CATALOG , argv[2], userName, domainName,
			      R_DELETE_ACCS, argv[3],"","","");
  }
  else if (!strcmp(argv[1],"changeOwner")) {
    if (argc != 4) {    
      usage();
      close_db2_interaction(MDAS_CAT_ROLLBACK);
      exit(-1);
    }
    i =  modify_resource_info(MDAS_CATALOG , argv[2], userName, domainName,
			      R_CHANGE_OWNER, argv[3],"","","");
  }
  else {
    usage();
    close_db2_interaction(MDAS_CAT_ROLLBACK);
    exit(-1);
  }
  if (i != 0) {
    printf("ingestResource Register Error: %i\n",i);
    close_db2_interaction(MDAS_CAT_ROLLBACK);
    exit(-1);
  }
  close_db2_interaction(MDAS_CAT_COMMIT);
  exit(0);
}


  
      
        
     


