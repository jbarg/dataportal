/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#include "scommands.h"

 

extern char srbAuth[];
extern char srbHost[];
extern char srbUser[];
extern char mdasCollectionName[];
extern char inCondition[];
 
void
usage()
{
  fprintf(stderr,"Srmdir collection\n");
}

int
main(int argc, char **argv)
{
  srbConn *conn;
  char collName[MAX_TOKEN];
  char temp[MAX_TOKEN], temp2[MAX_TOKEN];
 
  int i;

  strcpy(inCondition , "");
  if (argc < 2) {usage(); exit(1);}
    i = initSrbClientEnv();
    if (i < 0)
      {printf("Srmdir: Error:%i\n",i); exit(1);}


    sprintf(temp,"%s/alpha",argv[1]);
    splitpath(temp, collName, temp2);


    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Srmdir: Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    i = srbModifyCollect(conn, MDAS_CATALOG, collName,
			 "", "", "", D_DELETE_COLL);
    if (i < 0)
      {
	fprintf(stderr,"Srmdir: Error for collection '%s': %i\n", collName,i);
	srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(4);
      }
    clFinish(conn);
    exit(0);
}


