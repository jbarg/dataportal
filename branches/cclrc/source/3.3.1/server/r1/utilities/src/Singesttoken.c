#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif
 
#include "srbClient.h"


extern char *clGetUserName(), *clGetDomainName(), *clGetAuth();

int usage(char *name) {
  fprintf(stderr,"usage: %s    DataType <dataTypeValue> <ParentValue> <mimeTypeString> <dotExtensionString>\n",name);
  fprintf(stderr,"  Please use 'home' as a default for parent value\n");
  fprintf(stderr,"usage: %s    <Type> <NewValue> <ParentValue>\n",name);
  fprintf(stderr,"  Supported Type(s): ResourceType, UserType, Domain\n");
  fprintf(stderr,"  Please use 'home' as a default for parent value\n");
  fprintf(stderr,"usage: %s    <Type> <NewValue>\n",name);
  fprintf(stderr,"  Supported Type(s): ResourceClass, AuthScheme, Action\n");	return(0);
}

int main(int argc,  char* argv[])
{  
  srbConn *conn;
  int i;
  char dataTypeStr[MAX_TOKEN * 3];
 
  if (argc == 1 || !strcmp(argv[1],"-h")) {
    usage(argv[0]);
    exit(1);
  }
  
  conn = srbConnect (NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  if (clStatus(conn) != CLI_CONNECTION_OK)   {
    fprintf(stderr,"Connection to exfMaster failed.\n");
    fprintf(stderr,"%s",clErrorMessage(conn));
    clFinish(conn);
    exit (1);
    
  }
  
  if (!strcmp(argv[1],"ResourceType")  ||
      !strcmp(argv[1],"UserType")  ||
      !strcmp(argv[1],"Domain")){
    if (argc != 4) {usage(argv[0]); exit(1);}
    i = srbIngestToken(conn, argv[1],argv[2],argv[3]);
  }
  else if (!strcmp(argv[1],"ResourceClass")  ||
      !strcmp(argv[1],"AuthScheme")  ||
	   !strcmp(argv[1],"Action")){
    if (argc != 3) {usage(argv[0]); exit(1);}
    i = srbIngestToken(conn, argv[1],argv[2],"");
  }
  else if (!strcmp(argv[1],"DataType")) {
    if (argc != 6) {usage(argv[0]); exit(1);}
    sprintf(dataTypeStr,"%s|%s|%s",argv[2],argv[4],argv[5]);
    i = srbIngestToken(conn, argv[1],dataTypeStr,argv[3]);
  }
  else {
    usage(argv[0]);
    exit(1);
  }
   if (i != 0)  {
    fprintf(stderr,"Singesttoken Registration Error: %i\n",i);
    srb_perror(2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    fprintf(stderr,"Contact SRB Consultants for more help\n");
    clFinish(conn);
    exit(1);
  }
  clFinish(conn);
  exit(0);
}

  
      
        
     







