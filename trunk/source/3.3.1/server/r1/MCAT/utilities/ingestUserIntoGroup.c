/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "mdasPrototypes.h"
#include "mcatAdmin.h"

extern char *clGetUserName(), *clGetDomainName(), *clGetAuth();

main(int argc,
        char* argv[])
{  
    char data_value_2[MAX_TOKEN];
    int i;
    int retraction_type;
    char *userName, *domainName, *userAuth;
    char clErrorMsg[MAX_TOKEN];
        
    if ( (argc  != 3) ) {
	  printf("Error: Use %s <userName@domainName> <group-name> \n",argv[0]);
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
    printf("opened\n");

    ClientUser = (userInfo *) malloc (sizeof (userInfo));
    ClientUser->privUserFlag = 1;


    i= get_mdas_authorization(userName, userAuth, domainName);
    if (i != 0) {
          printf("Authorization Error:%i\n",i);
          exit(-1);
    }
    printf("authorized\n");

    i = modify_user_info(MDAS_CATALOG, userName, argv[1],argv[2],
			 21,  userAuth,  domainName);
    if (i != 0) printf("ERROR: %i\n",i);
    else  printf("DONE\n");
    close_db2_interaction(MDAS_CAT_COMMIT);
    exit(0);
 
}
  
      
        
     


