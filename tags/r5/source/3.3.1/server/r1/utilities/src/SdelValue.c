#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif
 
#include "srbClient.h"

extern char *clGetUserName(), *clGetDomainName(), *clGetAuth();

int usage(char *name) {
        fprintf(stderr,"usage: %s <operationType> <DeleteValue>\n", name );
        fprintf(stderr,"Where <operationType> = User, PhyResource, Location\n");
        fprintf(stderr,"For <operationType> = User, the <DeleteValue> should be of the form user@domain\n");
	return(0);
}

int main(int argc, char* argv[])
{  
  srbConn *conn;
  int i, delAction;
  char delVal[MAX_TOKEN];
  char inbuf[MAX_TOKEN];

  if (argc != 3) {
      usage(argv[0]);
      exit(0);
  }

  sprintf(delVal,"%s",argv[2]);

  if (!strcmp(argv[1],"User"))
      delAction = DELETE_TYPE_USER;
  else if (!strcmp(argv[1],"PhyResource"))
      delAction = DELETE_TYPE_RESOURCE;
  else if (!strcmp(argv[1],"Location"))
      delAction = DELETE_TYPE_LOCATION;
  else {
    usage(argv[0]);
    exit(-1);
  }

  printf("Do you really want to remove the %s of %s?(Yes/No):",
         argv[1],argv[2]);
  fgets(inbuf, MAX_TOKEN, stdin);
  if (strlen(inbuf)>0) inbuf[strlen(inbuf)-1]='\0';
  if (strcmp(inbuf,"Yes") && strcmp(inbuf,"Y") && strcmp(inbuf,"y")) {
    printf("Delete aborted\n");
    exit (-1);
  }

  conn = srbConnect (NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  if (clStatus(conn) != CLI_CONNECTION_OK) 
  {
        fprintf(stderr,"Connection to exfMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	clFinish(conn);
	exit (1);

  }

  i = srbDeleteValue(conn,delAction,delVal);

  if (i == -3207) {
    fprintf(stderr,"You do not have sufficient permission to run this command\n");
    srb_perror(2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
  }
  else if (i != 0) {
    fprintf(stderr,"SdelValue Error: %i\n",i);
    srb_perror(2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    fprintf(stderr,"Contact SRB Consultants for more help\n");
    clFinish(conn);
    exit(1);
  }

  clFinish(conn);
  exit(0);
}

