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
 
  char TokenSwitch[SMALL_TOKEN];
  char newValue[SMALL_TOKEN];
  char parentValue[SMALL_TOKEN];
  int i;
  char *userName, *domainName, *userAuth, *tmpPtr;
  char clErrorMsg[MAX_TOKEN];
  char syscallStr[MAX_TOKEN];

  if (argc != 4 && argc != 3)
    {
      printf("Usage: ingestToken  <Type> <NewValue> [<ParentValue>]\n");
      printf("Supported Type(s): ResourceType, DataType, UserType, Domain, Action\n");

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

  strcpy(TokenSwitch,argv[1]);
  strcpy(newValue,argv[2]);
  if (argc == 4)
    strcpy(parentValue,argv[3]);
  else
    strcpy(parentValue,"");

    i = open_db2_interaction(MDAS_CATALOG);
    if (i != 0) {
      printf("Opening Error:%i\n",i);
      exit(-1);
    }
    
    ClientUser = (userInfo *) malloc (sizeof (userInfo));
    ClientUser->privUserFlag = 1;


    i = insertTokenInfo(TokenSwitch,newValue,parentValue,
			userName, domainName, userAuth);
    if (i != 0)
      {
	printf("ingestToken Error: %i\n",i);
	close_db2_interaction(MDAS_CAT_ROLLBACK);
	exit(-1);
      }
    close_db2_interaction(MDAS_CAT_COMMIT);
    if (!strcmp(TokenSwitch,"Domain")) {
      tmpPtr = strstr(argv[0],"Token");
      *tmpPtr ='\0';
      sprintf(syscallStr,"%sUsergroup %s %ld group '' '' ''",argv[0],newValue, getpid());
      i = system(syscallStr);
      if (i != 0) {
	printf("ingestToken Error:Error in Making User Group for Domain:%i\n",i);
	exit(-1);
      }
    }
    exit(0);
}

  
      
        
     


