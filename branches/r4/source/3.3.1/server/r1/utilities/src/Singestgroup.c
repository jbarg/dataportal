#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif
 
#include "srbClient.h"


extern char *clGetUserName(), *clGetDomainName(), *clGetAuth();

int usage(char *name) {
        fprintf(stderr,"usage: %s   <NewGroupName> <NewGroupPassword> <NewGroupAddress> <NewGroupPhone> <NewGroupEmail>\n", name );
	fprintf(stderr,"Example: Singestgroup newgroup TEST 'address' 'phone' 'e-mail'\n");
	return(0);
}

int main(int argc,
        char* argv[])
{  
  srbConn *conn;
  int i;
  char groupType[MAX_TOKEN];
  if (argc != 6)
  {
      usage(argv[0]);
      exit(0);
  }

  sprintf(groupType,"%s","group");

  conn = srbConnect (NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  if (clStatus(conn) != CLI_CONNECTION_OK) 
  {
        fprintf(stderr,"Connection to exfMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	clFinish(conn);
	exit (1);
  }

  i = srbRegisterUserGrp (conn,0,argv[1],argv[2],groupType,argv[3],argv[4],argv[5]);

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

  clFinish(conn);
  exit(0);
}

