/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#include "scommands.h"
 


extern char mdasCollectionName[];
 

int
main(int argc, char **argv)
{
  int i;

    if (argc != 1) 
      {fprintf(stderr, "Usage: %s \n",argv[0]);
      exit(1);
      }

    /* XXXX - try the new initSrbEnv. i = initSrbClientEnv(); */
    i = initSrbEnv();
    if (i < 0) {
        if (getenv("mdasCollectionName")) {
            printf("%s\n", getenv("mdasCollectionName"));
        } else {
            printf("Error:%i\n", i); 
            srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    exit (1);
        }
    } else {
        printf("%s\n",mdasCollectionName);
    }
    exit(0);
}
