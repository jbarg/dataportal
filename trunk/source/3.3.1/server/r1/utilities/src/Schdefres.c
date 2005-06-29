/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "scommands.h"
extern char defaultResource[];

void usage ()
{
  fprintf(stderr,"Usage: Schdefres  rsrcName\n");
}
int
main(int argc, char **argv)
{
   int i;

   if (argc != 2) {
     usage();
     exit(1);
   }
   i = initSrbEnv();
   if (i < 0) {
     fprintf(stderr,"Error:%i\n",i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     exit (1);
   }
   printf("Old Default Resource: %s\n",defaultResource);
   strcpy(defaultResource,argv[1]);
   rewriteSrbClientEnvFile();
   printf("New Default Resource: %s\n",defaultResource);
   exit(0);
}
