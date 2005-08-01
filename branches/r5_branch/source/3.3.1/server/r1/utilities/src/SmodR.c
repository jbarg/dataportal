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
  fprintf(stderr,"All these operations can be done only by SRB Administrators\n");
  fprintf(stderr,"Usage: SmodR -a phyResourceName logicalResourceName\n");
  fprintf(stderr,"          add phyResourceName to logicalResourceName\n");
  fprintf(stderr,"          if logical resource is not there then it is created\n");
  fprintf(stderr,"Usage: SmodR -l phyResourceName logicalResourceName\n");
  fprintf(stderr,"          delete phyResourceName from logicalResourceName\n");
  fprintf(stderr,"Usage: SmodR -i user@domain accs resourceName\n");
  fprintf(stderr,"          insert/modifyTo resource access 'accs' for user@domain\n");
  fprintf(stderr,"Usage: SmodR -d user@domain resourceName\n");
  fprintf(stderr,"          delete resource access to  user@domain\n");
  fprintf(stderr,"Usage: SmodR -o user@domain resourceName\n");
  fprintf(stderr,"          change resource ownership to user@domain\n");
  fprintf(stderr,"Usage: SmodR -x maxLatencyInMilliSecs resourceName\n");
  fprintf(stderr,"          change maximum latency of resource  to maxLatencyInMilliSecs\n");
  fprintf(stderr,"Usage: SmodR -n minLatencyInMilliSecs resourceName\n");
  fprintf(stderr,"          change minimum latency of resource  to minLatencyInMilliSecs\n");
  fprintf(stderr,"Usage: SmodR -b bandwidthInMegaBitsPerSec resourceName\n");
  fprintf(stderr,"          change bandwidth of resource to bandwidthInMegaBitsPerSec\n");
  fprintf(stderr,"Usage: SmodR -c maxConcurrency resourceName\n");
  fprintf(stderr,"          change maximum allowed concurrency of resource to maxConcurrency (numeric)\n");
  fprintf(stderr,"Usage: SmodR -s numStripes resourceName\n");
  fprintf(stderr,"          change striping of resource to numStripes (numeric)\n");
  fprintf(stderr,"Usage: SmodR -h numHierarchies resourceName\n");
  fprintf(stderr,"          change number of hierarchies in the resource to numHierarchies (numeric)\n");
  fprintf(stderr,"Usage: SmodR -g capacityInGigaBytes resourceName\n");
  fprintf(stderr,"          change maximum capacity of the resource to capacityInGigaBytes (numeric)\n");
  fprintf(stderr,"Usage: SmodR -z description resourceName\n");
  fprintf(stderr,"          change description of the resource to description (string)\n");
  fprintf(stderr,"Usage: SmodR -u timeStamp currDataSize user@domain resourceName\n");
  fprintf(stderr,"          change current usage for user@domain to currDataSize (numeric in GigaBytes) at the given timeStamp");
  fprintf(stderr,"          if timeStamp is -1 then the timestamp is internally generated\n");
  fprintf(stderr,"          this information is kept historic and is not over written.\n");
  fprintf(stderr,"Usage: SmodR -q quota currentUsage user@domain resourceName\n");
  fprintf(stderr,"          set current quota and usage for user@domain to quota and currentUsage (both numeric in GigaBytes)");
  fprintf(stderr,"          if quota is -1 then the quota is not changed\n");
  fprintf(stderr,"          this information is unique per user-resource pair and gets overwritten when changed.\n");
  fprintf(stderr,"Usage: SmodR -D resourceName\n");
  fprintf(stderr,"          change the resource status to down (-4)\n");
  fprintf(stderr,"Usage: SmodR -U resourceName\n");
  fprintf(stderr,"          change the resource status to up (-1)\n");

}

int
main(int argc, char **argv)
{
  srbConn *conn;
  int i;
    char tmpStr[MAX_TOKEN], tmpStr1[MAX_TOKEN];


    if (argc < 3) {
      usage ();exit (1);
    }
    i = initSrbClientEnv();
    if (i < 0)
      {printf("SmodR: Initializatin Error:%i\n",i); exit(1);}
    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
      fprintf(stderr,"Connection to srbMaster failed.\n");
      fprintf(stderr,"%s",clErrorMessage(conn));
      srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      clFinish(conn); exit(3);
    }
    


    if (!strcmp(argv[1],"-i")) {
      if (argc != 5) {
	usage ();clFinish(conn);exit (1);
      }
      i = srbModifyResource (conn, 0, argv[argc -1],argv[2],argv[3],
			     "","",R_INSERT_ACCS);
    }
    else if (!strcmp(argv[1],"-d")) {
      if (argc != 4) {
	usage ();clFinish(conn);exit (1);
      }
      i = srbModifyResource (conn, 0, argv[argc -1],argv[2],"",
			     "","",R_DELETE_ACCS);
    }
    else if (!strcmp(argv[1],"-o")) {
      if (argc != 4) {
	usage ();clFinish(conn);exit (1);
      }
      i = srbModifyResource (conn, 0, argv[argc -1],argv[2],"",
			     "","",R_CHANGE_OWNER);
    }
    else if (!strcmp(argv[1],"-x")) {
      if (argc != 4) {
	usage ();clFinish(conn);exit (1);
      }
      i = srbModifyResource (conn, 0, argv[argc -1],argv[2],"",
			     "","",R_ADJUST_LATENCY_MAX_IN_MILLISEC);
    }
    else if (!strcmp(argv[1],"-n")) {
      if (argc != 4) {
	usage ();clFinish(conn);exit (1);
      }
      i = srbModifyResource (conn, 0, argv[argc -1],argv[2],"",
			     "","",R_ADJUST_LATENCY_MIN_IN_MILLISEC);
    }
    else if (!strcmp(argv[1],"-b")) {
      if (argc != 4) {
	usage ();clFinish(conn);exit (1);
      }
      i = srbModifyResource (conn, 0, argv[argc -1],argv[2],"",
			     "","",R_ADJUST_BANDWIDTH_IN_MBITSPS);
    }
    else if (!strcmp(argv[1],"-c")) {
      if (argc != 4) {
	usage ();clFinish(conn);exit (1);
      }
      i = srbModifyResource (conn, 0, argv[argc -1],argv[2],"",
			     "","",R_ADJUST_MAXIMUM_CONCURRENCY);
    }
    else if (!strcmp(argv[1],"-s")) {
      if (argc != 4) {
	usage ();clFinish(conn);exit (1);
      }
      i = srbModifyResource (conn, 0, argv[argc -1],argv[2],"",
			     "","",R_ADJUST_NUM_OF_STRIPES);
    }
    else if (!strcmp(argv[1],"-h")) {
      if (argc != 4) {
	usage ();clFinish(conn);exit (1);
      }
      i = srbModifyResource (conn, 0, argv[argc -1],argv[2],"",
			     "","",R_ADJUST_NUM_OF_HIERARCHIES);
    }
    else if (!strcmp(argv[1],"-g")) {
      if (argc != 4) {
	usage ();clFinish(conn);exit (1);
      }
      i = srbModifyResource (conn, 0, argv[argc -1],argv[2],"",
			     "","",R_ADJUST_CAPACITY_IN_GIGABYTES);
    }
    else if (!strcmp(argv[1],"-u")) {
      if (argc != 6) {
	usage ();clFinish(conn);exit (1);
      }
      i = srbModifyResource (conn, 0, argv[argc -1],argv[2],argv[3],
			     argv[4],"",R_INSERT_RSRC_USAGE_TOTAL_BY_DATE);
    }
    else if (!strcmp(argv[1],"-q")) {
      if (argc != 6) {
	usage ();clFinish(conn);exit (1);
      }
      i = srbModifyResource (conn, 0, argv[argc -1],argv[2],argv[3],
			     argv[4],"",R_INSERT_RSRC_USAGE_AND_QUOTA);
    }
    else if (!strcmp(argv[1],"-l")) {
      if (argc != 4) {
	usage ();clFinish(conn);exit (1);
      }
	i = srbModifyResource (conn, 0, argv[argc -1],argv[2],"",
			       "","",R_DELETE_PHY_RSRC_FROM_LOG_RSRC);
    }
    else if (!strcmp(argv[1],"-a")) {
      if (argc != 4) {
	usage ();clFinish(conn);exit (1);
      }
	i = srbModifyResource (conn, 0, argv[argc -1],argv[2],"",
			       "","",R_INSERT_PHY_RSRC_INTO_LOG_RSRC);
    }
    else if (!strcmp(argv[1],"-U")) {
        if (argc != 3) {
            usage ();clFinish(conn);exit (1);
        }
        sprintf (tmpStr, "%d", time (0));
        sprintf (tmpStr1, "%d", RES_RESOURCE_RESTART);
        i = srbModifyRescInfo (conn, MDAS_CATALOG,
         argv[2], R_INSERT_FREE_SPACE, tmpStr1, tmpStr,
         NULL, NULL);

    }
    else if (!strcmp(argv[1],"-D")) {
        if (argc != 3) {
            usage ();clFinish(conn);exit (1);
        }
        sprintf (tmpStr, "%d", time (0));
        sprintf (tmpStr1, "%d", RES_RESOURCE_DELETED);
        i = srbModifyRescInfo (conn, MDAS_CATALOG,
         argv[2], R_INSERT_FREE_SPACE, tmpStr1, tmpStr,
         NULL, NULL);
    }
    if (i < 0) {
      printf("SmodR: Operational Error:%i\n",i); 
      srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      clFinish(conn);
      exit (1);
    }
    else {
      clFinish(conn);
      exit(0);
    }
}



