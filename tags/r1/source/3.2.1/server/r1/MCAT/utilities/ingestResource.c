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
  char *userName, *domainName, *userAuth;
  char clErrorMsg[MAX_TOKEN];
  char pubName[MAX_TOKEN];

  if (argc != 7)
    {
      printf("Usage: ingestResource  <ResourceName> <ResourceType> <Location> <DefaultPath> <resourceClass> <maxObjSizeInResource>\n");
      printf("Note that public get write access to ingested resource\n"); 
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
    i = registerResourceInfo(argv[1],argv[2],argv[3],
			     userName, domainName, userAuth,
				argv[4],argv[5],atoi(argv[6]));
    if (i != 0)
      {
	printf("ingestResource Error: %i\n",i);
	close_db2_interaction(MDAS_CAT_ROLLBACK);
	exit(-1);
      }
    strcpy(pubName,"public@npaci");
    i =  modify_resource_info(MDAS_CATALOG , argv[1], userName, domainName,
			      R_INSERT_ACCS, pubName,"write","","");
    if (i != 0)
      {
	printf("ingestResource Error  for Public Access: %i\n",i);
	close_db2_interaction(MDAS_CAT_ROLLBACK);
	exit(-1);
      }

    close_db2_interaction(MDAS_CAT_COMMIT);
    return(0);
}

  
      
        
     


