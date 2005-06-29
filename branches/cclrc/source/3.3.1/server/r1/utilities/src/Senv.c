/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#include "scommands.h"

 


extern char mdasCollectionName[];
extern char srbUser[];
extern char srbHost[];
extern char srbPort[];
extern char mcatZone[];
extern char mdasDomainName[];
extern char mdasCollectionHome[];
extern char mdasDomainHome[];
extern char defaultResource[];

int
main(int argc, char **argv)
{
  int i;

    if (argc != 1) 
      {fprintf(stderr, "Usage: %s \n",argv[0]);
      exit(1);
      }

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Error:%i\n",i); exit(1);}
    printf("SRBUSER=%s\n",srbUser);
    printf("SRBHOST=%s\n",srbHost);
    if (getenv("srbPort") != NULL) {
      printf ("SRBPORT=%s\n", getenv("srbPort"));
    } else if (srbPort != NULL && strlen (srbPort) > 0) {
      printf ("SRBPORT=%s\n", srbPort);
    } else {
      printf ("SRBPORT= %s\n", DefaultPort);
    }
    if (strlen(mcatZone) > 0) printf("MCATZONE=%s\n",mcatZone);
    else printf("MCATZONE=default\n");
    printf("MDASCOLLECTIONNAME=%s\n",mdasCollectionName);
    printf("MDASDOMAINNAME=%s\n",mdasDomainName);
    printf("MDASCOLLECTIONHOME=%s\n",mdasCollectionHome);
    printf("defaultResource=%s\n",defaultResource);

    exit(0);
}
