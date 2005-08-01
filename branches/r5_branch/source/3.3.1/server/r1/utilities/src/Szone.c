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
extern char mdasCollectionName[];
extern char inCondition[];

void
usage()
{
  fprintf(stderr," Szone -h\n");
  fprintf(stderr," Szone -l zone  <locnDesc> <portNumber> <adminUserName@Domain> <contactInformation> <comment>\n");
  fprintf(stderr," Szone -r zone  <locnDesc> <portNumber> <adminUserName@Domain> <contactInformation> <comment>\n");
  fprintf(stderr," Szone -M zone  <locnDesc> <portNumber> <adminUserName@Domain> <contactInformation> <comment>\n");
  fprintf(stderr," Szone -U zone  <userName> <DomainName>\n");
  fprintf(stderr," Szone -C oldZoneName  <newZoneName>\n");
  fprintf(stderr," Szone -L zone <newLocalFlagValueNumber>\n");
  fprintf(stderr," Szone -S zone <newStatusNumber>\n");
  fprintf(stderr," -h option is used to print this help message\n");
  fprintf(stderr," -l option is used to insert a local zone\n");
  fprintf(stderr," -r option is used to insert a remote zone\n");
  fprintf(stderr," -M option is used to modify information about an existing zone. If you want to change a particular information, put the new value at the appropriate argument position. If an item is not being changed just use a quoted empty string ('') in that position. \n");
  fprintf(stderr," -U option is used to change the zone value for a SRB user\n");
  fprintf(stderr," -C option is used to change the name of a zone. It changes the name for all associated users also.\n");
  fprintf(stderr," -L option is used to change whether a zone is local or remote. Use 1 to make it local and 0 to make it remote. Only one zone can be local for each MCAT\n");
  fprintf(stderr," -S option is used to change the status of the zone. All zones are normally active (= 1). \n");
}


int
main(int argc, char **argv)
{
    srbConn *conn;
    int i;
    char empStr[3] = "";

    if (argc == 1){
      fprintf(stderr,"Unknown option \n");
      usage();
      exit(1);
    }

    i = initSrbClientEnv();
    if (i < 0)  {
      printf("Szone Initialization Error:%i\n",i); 
      exit(1);
    }


    conn = srbConnect (srbHost, NULL, srbAuth, 
		       NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
      fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    if (!strcmp(argv[1] ,"-l")) {
      if (argc != 8) {
	fprintf(stderr,"Incorrect Number of Arguments\n");
	usage();
	exit(1);
      }
      i  = srbModifyZone (conn, 0, argv[2],
			  argv[3],argv[4],argv[5],argv[6],argv[7],
			  Z_INSERT_NEW_LOCAL_ZONE);
    }
    else if (!strcmp(argv[1] ,"-r")) {
      if (argc != 8) {
	fprintf(stderr,"Incorrect Number of Arguments\n");
	usage();
	exit(1);
      }
      i  = srbModifyZone (conn, 0, argv[2],
			  argv[3],argv[4],argv[5],argv[6],argv[7],
			  Z_INSERT_NEW_ALIEN_ZONE);
    }
    else if (!strcmp(argv[1] ,"-M")) {
      if (argc != 8) {
	fprintf(stderr,"Incorrect Number of Arguments\n");
	usage();
	exit(1);
      }
      i  = srbModifyZone (conn, 0, argv[2],
			  argv[3],argv[4],argv[5],argv[6],argv[7],
			  Z_MODIFY_ZONE_INFO);
    }
    else if (!strcmp(argv[1] ,"-U")) {
      if (argc != 5) {
	fprintf(stderr,"Incorrect Number of Arguments\n");
	usage();
	exit(1);
      }
      i  = srbModifyZone (conn, 0, argv[2],
			  argv[3],argv[4],empStr,empStr,empStr,
			  Z_MODIFY_ZONE_FOR_USER);
    }
    else if (!strcmp(argv[1] ,"-C")) {
      if (argc != 4) {
	fprintf(stderr,"Incorrect Number of Arguments\n");
	usage();
	exit(1);
      }
      i  = srbModifyZone (conn, 0, argv[2],
			  argv[3],empStr,empStr,empStr,empStr,
			  Z_CHANGE_ZONE_NAME);
    }
    else if (!strcmp(argv[1] ,"-L")) {
      if (argc != 4) {
	fprintf(stderr,"Incorrect Number of Arguments\n");
	usage();
	exit(1);
      }
      i  = srbModifyZone (conn, 0, argv[2],
			  argv[3],empStr,empStr,empStr,empStr,
			  Z_MODIFY_ZONE_LOCAL_FLAG);
    }
    else if (!strcmp(argv[1] ,"-S")) {
      if (argc != 4) {
	fprintf(stderr,"Incorrect Number of Arguments\n");
	usage();
	exit(1);
      }
      i  = srbModifyZone (conn, 0, argv[2],
			  argv[3],empStr,empStr,empStr,empStr,
			  Z_MODIFY_ZONE_STATUS);
    }
    else if (!strcmp(argv[1] ,"-h")) {
      usage();
      exit(1);
    }
    else {
      fprintf(stderr,"Unknown option \n");
      usage();
      exit(1);
    }
    clFinish(conn);
    if (i < 0) {
      fprintf(stderr,"Szone: Error in Performing Action: %i\n",i);
      srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      exit(1);
    }

    exit(0);
}


