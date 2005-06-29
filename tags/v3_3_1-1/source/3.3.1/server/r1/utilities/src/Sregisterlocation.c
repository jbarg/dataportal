#include <stdio.h>
#ifndef _WIN32
#include <sys/file.h>
#include <sys/stat.h>
#endif
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h>
#endif
 
#include "srbClient.h"


int usage() {
   fprintf(stderr,"usage: Sregisterlocation <NewLocationName> <FullNetworkAddress> <ParentLocation> <ServerUser> <ServerUserDomain>\n");
   return(0);
}

int main(int argc, char* argv[])
{  
   srbConn *conn;
   int i;
   char location[MAX_TOKEN];
   if (argc != 6) {
      usage();
      exit(0);
   }

   conn = srbConnect (NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   if (clStatus(conn) != CLI_CONNECTION_OK) {
      fprintf(stderr,"Connection to srbServer failed.\n");
      fprintf(stderr,"%s",clErrorMessage(conn));
      clFinish(conn);
      exit (1);
   }
   if (strstr(argv[2],":") == NULL)
     sprintf(location,"%s:NULL:NULL",argv[2]);
   else
     strcpy(location,argv[2]);
   i = srbRegisterLocation (conn, argv[1], location,argv[3],argv[4],argv[5]);
   if (i != 0)  {
      fprintf(stderr,"Sregisterlocation Error: %i\n",i);
      srb_perror(2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      clFinish(conn);
      if (i == TOKEN_ALREADY_IN_CAT) {
         exit(2);
      }
      exit(1);
   }

   clFinish(conn);
   exit(0);
}

  
      
        
     


