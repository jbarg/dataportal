/**************************************************************************
Copyright ^Ó General Atomics, 1995-1999
All Rights Reserved
 
This file contains Original Code and/or Modifications of Original Code as
defined in and that are subject to the General Atomics Storage Resource
Broker Academic and Research Use  License (the 'License'). This license is
for academic and research use only. You may not use, copy, modify or
distribute SRB for commercial purposes, nor incorporate the SRB into
commercial products without the express written permission of General
Atomics nor may you distribute SRB to any third party.
 
You may not use this software except in compliance with the License. Please
obtain a copy of the License at http://www.npaci.edu/DICE/SRB/License and
read it before using this file.
 
The Original Code and all software distributed under the License are
distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
EXPRESS OR IMPLIED, AND GENERAL ATOMICS AND THE SAN DIEGO SUPERCOMPUTER
CENTER HEREBY DISCLAIM ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION,
ANY WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
NON-INFRINGEMENT OF ANY COPYRIGHT, PATENT , TRADEMARK OR OTHER RIGHTS.
GENERAL ATOMICS AND THE SAN DIEGO SUPERCOMPUTER CENTER FURTHER  HAVE NO
OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCMENTS OR
MODIFICATIONS.. Please see the License for the specific language governing
rights and limitations under the License."
 
Please read the terms of this license carefully. By using the SRB software,
you are accepting and agreeing to the terms of this license with General
Atomics. If you are agreeing to this license on behalf of a company, or
other institution, you represent that you are authorized to bind the
company/institution to such a license. If you do not meet this criterion or
you do not agree to any of the terms of this license, you are not
authorized to use the software.
**************************************************************************/
 


#include "scommands.h"


 

extern char srbAuth[];
extern char srbHost[];
extern char mdasCollectionName[];
extern char mdasCollectionHome[];
extern char inCondition[];

srbConn *conn;
int   copyNum = -1;


void usage(char *prog)
{
	   
  fprintf(stderr, 
   "Usage: %s [-r]  [-S resourceName] srbFileName  localFileName\n", prog);
}
int
main(int argc, char **argv)
{
    int i;
    char collname[MAX_TOKEN];
    char dataname[MAX_TOKEN];
    char tmpDataname[MAX_TOKEN];
    char resourcename[MAX_TOKEN];
    srbPathName targName;
    int c;
    srbConn *conn;
    int status;
    int SFlag = 0;              /* -S option */
    int flagval = 0;

    strcpy(inCondition , "");
    while ((c=getopt(argc, argv,"S:r")) != EOF) {
        switch (c) {
            case 'S':
                SFlag = 1;
                strcpy (resourcename, optarg);
                break;
            case 'r':
                flagval |= R_FLAG;
                break;
            default:
                usage (argv[0]);
                exit (1);
        }
    }

    if (argc - optind != 2) {    /* must have 2input */
        usage (argv[0]);
        exit (1);
    }

    if (SFlag == 0)
        strcpy (resourcename, "");

    i = initSrbClientEnv();
    if (i < 0)
      {printf("Ssfstage: Initialization Error:%i\n",i); exit(-1);}

    targName.inpArgv = argv[optind];
    if (parseName (&targName, mdasCollectionName) < 0)
        {exit(4);}

    mySplitpath (targName.outArgv, collname, dataname);

    if (argv[optind + 1][0] != '/') {
	printf ("The local path %s must be a full pathname\n", 
	 argv[optind + 1]);
	exit (1);
    }

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Ssfstage: Connection to srbMaster failed.\n");
        fprintf(stderr,"Ssfstage: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(3);
    }

    if (strlen(resourcename) == 0)
	  get_user_preference(resourcename, "DRSRC");

    if ((flagval & R_FLAG) > 0) {
	sprintf (tmpDataname, "%s&STANDF_STAGE=%i", dataname, STANDF_REG);
    } else {
	sprintf (tmpDataname, "%s&STANDF_STAGE=%i", dataname, STANDF_NOREG);
    }

    status = srbObjReplicate (conn, MDAS_CATALOG, tmpDataname, collname, 
     resourcename, argv[optind + 1]);
    if (status < 0) {
        fprintf (stderr,
         "Ssfstage: srbObjReplicate error status =  %i\n",
          status);
        srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    }

    clFinish(conn);
    exit(0);
}

