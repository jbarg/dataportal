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

void usage(char *incmd)
{
  printf("Usage: %s  <AttributeName> <NewValue> <OldValue>\n", incmd);
  printf("Supported Attributes: PhyResourceName, LogicalResourceName\n");
  printf("                      UserName , DomainOfUser, TypeOfUser\n");
  printf("                      UserGroupName,DomainName, ResourceTypeName\n");
  printf("                      DataTypeName, UserTypeName ActionName\n");
  printf("                      ResourceClassName\n");
  printf("For UserName, DomainOfUser and TypeOfUser the OldValue should be of the form user@domain\n");

}
main(int argc,
        char* argv[])
{  
 
  int i, valueTypeInt;
  char *userName, *domainName, *userAuth, *tmpPtr;
  char clErrorMsg[MAX_TOKEN];
  char syscallStr[MAX_TOKEN];
  char inbuf[MAX_TOKEN];

  if (argc != 4 ) {
    usage(argv[0]);
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
  
  if (!strcmp(argv[1],"PhyResourceName"))
    valueTypeInt = PHYSICAL_RESOURCE_NAME;
  else if (!strcmp(argv[1],"LogicalResourceName"))
    valueTypeInt = LOGICAL_RESOURCE_NAME;
  else if (!strcmp(argv[1],"UserName"))
    valueTypeInt = USER_NAME;
  else if (!strcmp(argv[1],"DomainOfUser"))
    valueTypeInt = USER_DOMAIN;
  else if (!strcmp(argv[1],"TypeOfUser"))
    valueTypeInt = TYPE_OF_USER;
  else if (!strcmp(argv[1],"UserGroupName"))
    valueTypeInt = USER_GROUP_NAME;
  else if (!strcmp(argv[1],"UserTypeName"))
    valueTypeInt = USER_TYPE;
  else if (!strcmp(argv[1]," DomainName"))
    valueTypeInt = DOMAIN_NAME;
  else if (!strcmp(argv[1],"ResourceTypeName"))
    valueTypeInt = RESOURCE_TYPE;
  else if (!strcmp(argv[1],"DataTypeName"))
    valueTypeInt = DATA_TYPE;
  else if (!strcmp(argv[1],"ActionName"))
    valueTypeInt = ACTION_NAME;
  else if (!strcmp(argv[1],"ResourceClassName"))
    valueTypeInt = RESOURCE_CLASS;
  else {
    fprintf (stderr,"Wrong Attribute Name\n");
    usage(argv[0]);
    exit(-1);
  }

  printf("Do you really want to change the %s of %s?(Yes/No):",
	 argv[1],argv[2]);
  fgets(inbuf, MAX_TOKEN, stdin);
  if (strlen(inbuf)>0) inbuf[strlen(inbuf)-1]='\0';
  if (strcmp(inbuf,"Yes"))
    exit (-1);

  
  i = open_db2_interaction(MDAS_CATALOG);
  if (i != 0) {
    printf("Opening Error:%i\n",i);
    exit(-1);
  }

    
    ClientUser = (userInfo *) malloc (sizeof (userInfo));
    ClientUser->privUserFlag = 1;
  i = changeSingleMcatValue(valueTypeInt,argv[2],argv[3],
			    userName, domainName, userAuth);
  if (i != MDAS_SUCCESS)
    {
      fprintf(stderr,"changeValue Error: %i\n",i);
      close_db2_interaction(MDAS_CAT_ROLLBACK);
      exit(-1);
    }
  close_db2_interaction(MDAS_CAT_COMMIT);
  exit(0);
}

  
      
        
     


