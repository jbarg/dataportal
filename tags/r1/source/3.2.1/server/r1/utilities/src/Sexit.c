/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#include "scommands.h"


int
main(int argc, char **argv)
{
   int i;
   
   i = RemoveSrbClientEnvFile();
    if (i < 0)
      {printf("Error Removing Client Environment File:%i\n",i); exit(1);}
   exit(0);
}
