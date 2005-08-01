/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "scommands.h"

 


extern char mdasCollectionName[];
extern char mdasCollectionHome[];
extern char srbAuth[];
extern char srbHost[];
extern char inCondition[];


int
main(int argc, char **argv)
{
   srbConn *conn;
   srbPathName inpPath;
   char *tmpPath;
   int i, iLen=0;
   int status;

   strcpy(inCondition , "");
   /* XXXX - try the new initSrbEnv. i = initSrbClientEnv(); */
   i = initSrbEnv();
    if (i < 0) {
	printf("Error:%i\n",i);
        srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	exit (1);
    }
 
    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        clFinish(conn); exit(3);
    }

    if (argc == 1)
      {
	strcpy(mdasCollectionName,mdasCollectionHome);
	rewriteSrbClientEnvFile();
	clFinish(conn); exit(0);
      }
    tmpPath = argv[1];

    /* check for "s:" or "/srb" */
    if (strncmp (tmpPath, SRB_PREFIX, strlen (SRB_PREFIX)) == 0) {
        /* skip the prefix */
        tmpPath += strlen (SRB_PREFIX);
    } else if (strncmp (tmpPath, SRB_ROOT, strlen (SRB_ROOT)) == 0) {
        tmpPath = tmpPath + strlen (SRB_ROOT) - 1;
    }

    if (tmpPath[0] == '\0') {
	strcpy(mdasCollectionName,mdasCollectionHome);
	rewriteSrbClientEnvFile();
	clFinish(conn); exit(0);
    }

    inpPath.inpArgv = tmpPath;

    /* parse the path */
    if (parseName (&inpPath, mdasCollectionName) < 0) {
	fprintf (stderr, "bad path name %s\n", argv[1]);
	exit (1);
    }

    strcpy(mdasCollectionName,inpPath.outArgv);

    iLen = strlen(mdasCollectionName);
    if (iLen > 1 && mdasCollectionName[iLen-1] == '/')
      mdasCollectionName[iLen-1] = '\0'; /* remove last character '/' from string like 'Scd /a/b/c/' */

    if ((status = checkCollection(conn, MDAS_CATALOG, mdasCollectionName))
	== 0)
      {
	rewriteSrbClientEnvFile();
        clFinish(conn);
        exit(0);
      }
    else
      {
	fprintf(stderr, "Query of collection %s failed. status = %d\n",
	 mdasCollectionName, status);
	srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn);
        exit(1);
      }
}
