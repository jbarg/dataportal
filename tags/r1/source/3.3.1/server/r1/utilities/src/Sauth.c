/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/* Sauth.c - set up a scrambled authentication (password) file */

#include "scommands.h"
#include <stdio.h>

void usage (char *prog);

int
main(int argc, char **argv)
{
    int i, c;
    int eFlag=0;
    char buf[MAXEXFPATH+10], inbuf[MAXEXFPATH+10];
    int testFd;

    while ((c=getopt(argc, argv,"ehv")) != EOF) {
        switch (c) {
            case 'v':
	      obfSetDebug(1);
	      break;
            case 'e':
	      eFlag=1;
	      break;
            default:
	      usage (argv[0]);
	      exit (1);
        }
    }
    i =  obfSavePw(eFlag, 1, 1, "");
    if (i != 0) {
      printf("Failed with error %d\n", i);
      exit(1);
    }

    i = getMdasAuthFilename(buf);
    if (i == 0) {
      testFd = open(buf,  O_RDONLY, 0);
      if (testFd > 0) {
	close(testFd);
	printf("Remove (no longer needed) %s?:",buf);
	fgets(inbuf, MAX_TOKEN, stdin);
	i = strlen(inbuf);
	if (i > 1 && inbuf[0]=='y') {
	  unlink(buf);
	}
      }
    }
    exit (0);
}


void usage (char *prog)
{
  fprintf(stderr, "Usage: %s [-ehv]\n", prog);
  fprintf(stderr, " -e  echo the password as you enter it (normally there is no echo)\n");
  fprintf(stderr, " -h  this help\n");
  fprintf(stderr, " -v  verbose output\n");
  fprintf(stderr, "Creates a file containing your password, equivalent to the\n");
  fprintf(stderr, ".MdasAuth file, but in a scambled form for improved security.\n");
  fprintf(stderr, "By default this is ~/%s.\n",AUTH_FILENAME_DEFAULT);
  fprintf(stderr, "To change that, define an environment variable, %s,\n",
	  AUTH_FILENAME_ENV_VAR_NAME);
  fprintf(stderr, "be the full path of the file you would like to use.\n");
  fprintf(stderr, "Optionally, also define %s to further randomize the value.\n",
	  AUTH_ADDITIONAL_KEY_ENV_VAR_NAME);
}

