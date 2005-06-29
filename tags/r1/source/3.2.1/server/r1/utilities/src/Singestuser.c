#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif
 
#include "srbClient.h"


extern char *clGetUserName(), *clGetDomainName(), *clGetAuth();

int usage(char *name) {
        fprintf(stderr,"usage: %s   <NewUserName> <NewUserPassword> <NewUserDomain> <NewUserType> <NewUserAddress> <NewUserPhone> <NewUserEmail> <NewUserAuthScheme> <NewUserDNString>\n", name );
	fprintf(stderr,"Example: Singestuser test1 TEST1 sdsc staff 'address' 'phone' 'e-mail' '' ''  , if not using GSI_AUTH \n");
	fprintf(stderr,"Needs the following Env Variables for the Registering User:\n");
	fprintf(stderr,"      mdasDomainName\n");
	fprintf(stderr,"      mdasDomainHome\n");
	fprintf(stderr,"      srbUser\n");
	fprintf(stderr,"      srbHost\n");
	fprintf(stderr,"      srbPort\n");
	fprintf(stderr,"      AUTH_SCHEME\n");
	fprintf(stderr,"      SERVER_DN\n");
	return(0);
}

main(int argc,
        char* argv[])
{  
  srbConn *conn;
  int i;
  char userDom[MAX_TOKEN],authDesc[MAX_TOKEN];
  if (argc != 10)
    {
      usage(argv[0]);
      exit(0);
    }

  conn = srbConnect (NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  if (clStatus(conn) != CLI_CONNECTION_OK) 
  {
        fprintf(stderr,"Connection to exfMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	clFinish(conn);
	exit (1);

  }

  i = srbRegisterUser (conn,0,
			 argv[1], argv[3],argv[2],argv[4],argv[5],argv[6],
			     argv[7]);

  if (i == -3207) {
    fprintf(stderr,"You do not have sufficient permission to run this command\n");
    srb_perror(2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
  }
  else if (i != 0)  {
    fprintf(stderr,"SingestUser Registration Error: %i\n",i);
    srb_perror(2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    fprintf(stderr,"Contact SRB Consultants for more help\n");
    clFinish(conn);
    exit(1);
  }

  if ( ! strcmp( argv[8], "GSI_AUTH") ) {
  sprintf(userDom,"%s@%s",argv[1], argv[3]);
  sprintf(authDesc,"%s:%s",argv[8],argv[9]);
  i = srbModifyUser(conn,0,userDom,authDesc,U_INSERT_AUTH_MAP);

  if (i == -3207) {
    fprintf(stderr,"You do not have sufficient permission to run this command\n");
    srb_perror(2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
  }
  else if (i != 0)  {
    fprintf(stderr,"SingestUser DNString Ingestion Error: %i\n",i);
    srb_perror(2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    fprintf(stderr,"Contact SRB Consultants for more help\n");
    clFinish(conn);
    exit(1);
    }
  }
  clFinish(conn);
  exit(0);
}

  
      
        
     


