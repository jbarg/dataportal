#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif
 
#include "srbClient.h"

extern char *clGetUserName(), *clGetDomainName(), *clGetAuth();

int usage(char *name) {
        fprintf(stderr,"usage: %s <operationType> <UserName> <UserDomain> <Value>\n", name );
        fprintf(stderr,"Where <operationType> = addToGroup, addDN, password, delFromGroup, delDN, changeType\n", name );
	fprintf(stderr,"Example: %s addDN mike sdsc '/C=blah/O=foo/user=mike' \n");
	return(0);
}

main(int argc, char* argv[])
{  
  srbConn *conn;
  int i, modAction;
  char userAtDom[MAX_TOKEN], newVal[MAX_TOKEN];

  if (argc != 5) {
      usage(argv[0]);
      exit(0);
  }

  sprintf(userAtDom,"%s@%s",argv[2], argv[3]);

  if (!strcmp(argv[1],"addToGroup")) {
      modAction = U_INSERT_GROUP;
      sprintf(newVal,"%s",argv[4]);
  }
  else if(!strcmp(argv[1],"addDN")) {
      modAction = U_INSERT_AUTH_MAP;
      sprintf(newVal,"GSI_AUTH:%s",argv[4]);
  }
  else if(!strcmp(argv[1],"password")) {
      modAction = SU_CHANGE_PASSWORD;
      sprintf(newVal,"%s",argv[4]);
  }
  else if(!strcmp(argv[1],"delFromGroup")) {
      modAction = U_DELETE_GROUP;
      sprintf(newVal,"%s",argv[4]);
  }
  else if(!strcmp(argv[1],"delDN")) {
      modAction = U_DELETE_AUTH_MAP;
      sprintf(newVal,"%s",argv[4]);
  }
  else if(!strcmp(argv[1],"changeType")) {
      modAction = U_CHANGE_TYPE;
      sprintf(newVal,"%s",argv[4]);
  }
  else {
    usage(argv[0]);
    exit(-1);
  }

  conn = srbConnect (NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  if (clStatus(conn) != CLI_CONNECTION_OK) 
  {
        fprintf(stderr,"Connection to exfMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	clFinish(conn);
	exit (1);

  }

  i = srbModifyUser(conn,0,userAtDom,newVal,modAction);

  if (i == -3207) {
    fprintf(stderr,"You do not have sufficient permission to run this command\n");
    srb_perror(2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
  }
  else if (i != 0) {
    fprintf(stderr,"SmodifyUser Error: %i\n",i);
    srb_perror(2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    fprintf(stderr,"Contact SRB Consultants for more help\n");
    clFinish(conn);
    exit(1);
  }

  clFinish(conn);
  exit(0);
}
