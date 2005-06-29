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
extern char mdasDomainName[];

void
usage()
{
  fprintf(stderr,"Usage: Spullmeta [-F] <queryInfo> [<date>] \n");
  fprintf(stderr,"   <queryInfo>  consists of the ACTION_STRING\n");
  fprintf(stderr,"       and data which are relevant after the <date> is displayed\n");
  fprintf(stderr,"   <date> is of the form:  YYYY-MM-DD-HH.MM.SS\n");
  fprintf(stderr,"     it is allowed to give prefix date instead of full date\n");
  fprintf(stderr,"     if <date> is not given all data for the action is returned\n");
  fprintf(stderr,"     <date> is ignored for ACTION_STRING without the word CHANGED,\n"); 
  fprintf(stderr,"     <date> has to be given for ACTION_STRING with the word CHANGED,\n"); 
  fprintf(stderr,"   F option will print the output in format needed for SputB.\n");
  fprintf(stderr,"   ACTION_STRING that are supported are:\n");
  fprintf(stderr,"     GET_USER_TYPE\n");
  fprintf(stderr,"     GET_USER_DOMAIN\n");
  fprintf(stderr,"     GET_USER_AUTH_SCHEME\n");
  fprintf(stderr,"     GET_LOCATION_INFO\n");
  fprintf(stderr,"     GET_RESOURCE_TYPE\n");
  fprintf(stderr,"     GET_RESOURCE_CLASS\n");
  fprintf(stderr,"     GET_RESOURCE_ACCESS_CONSTRAINT\n");
  fprintf(stderr,"     GET_DATA_TYPE\n");
  fprintf(stderr,"     GET_DATA_ACCESS_CONSTRAINT\n");
  fprintf(stderr,"     GET_CHANGED_ZONE_INFO\n");
  fprintf(stderr,"     GET_CHANGED_USER_INFO\n");
  fprintf(stderr,"     GET_CHANGED_USER_AUTHMAP\n");
  fprintf(stderr,"     GET_CHANGED_USER_GROUPS\n");
  fprintf(stderr,"     GET_CHANGED_USER_UDEF_METADATA\n");
  fprintf(stderr,"     GET_CHANGED_PHYSICAL_RESOURCE_CORE_INFO\n");
  fprintf(stderr,"     GET_CHANGED_LOGICAL_RESOURCE_CORE_INFO\n");
  fprintf(stderr,"     GET_CHANGED_RESOURCE_OTHER_INFO\n");
  fprintf(stderr,"     GET_CHANGED_RESOURCE_UDEF_METADATA\n");
  fprintf(stderr,"     GET_CHANGED_RESOURCE_ACCESS\n");
  fprintf(stderr,"     GET_CHANGED_COLL_CORE_INFO\n");
  fprintf(stderr,"     GET_CHANGED_DATA_CORE_INFO\n");
  fprintf(stderr,"     GET_CONTAINER_INFO\n");
  fprintf(stderr,"     GET_CHANGED_DATA_ACCESS\n");
  fprintf(stderr,"     GET_CHANGED_COLL_ACCESS\n");
  fprintf(stderr,"     GET_CHANGED_COLLCONT_INFO\n");
  fprintf(stderr,"     GET_CHANGED_DATA_UDEFMETA_INFO\n");
  fprintf(stderr,"     GET_CHANGED_COLL_UDEFMETA_INFO\n");
  fprintf(stderr,"     GET_CHANGED_DATA_ANNOTATION_INFO\n");
  fprintf(stderr,"     GET_CHANGED_COLL_ANNOTATION_INFO\n");
  fprintf(stderr,"     GET_CHANGED_OTHERDATA_INFO\n");
  fprintf(stderr,"     GET_CHANGED_OTHERDATA_INFO\n");
  fprintf(stderr,"     GET_CHANGED_DATA_GUID_INFO\n");
}

int
main(int argc, char **argv)
{
    srbConn *conn;
    srbResult *res;
    int i;
    int L_val = DEFAULT_ROW_COUNT;
    mdasC_sql_result_struct  myresult;
    char *tmpPtr;
    char queryString[MAX_TOKEN *2];
    int c;
    int Fflag = 0;
    while ((c=getopt(argc, argv,"Fh")) != EOF) {
      switch (c) {
      case 'F':
	Fflag = 1;
	break;
      case 'h':
      default:
	usage (argv[0]);
	exit (1);
      }
    }



    if ((argc - optind) > 2 || (argc - optind) < 1) {
      usage();
      exit(1);
    }
    if (strstr(argv[optind],"CHANGED") && (argc - optind) == 1) {
      fprintf(stderr,"Error: <date> value is needed.\n");
      exit(1);
    }
    i = initSrbClientEnv();
    if (i < 0) {
      fprintf(stderr,"Spullmeta Initialization Error:%i\n",i); 
      exit(1);
    }
    conn = srbConnect (srbHost, NULL, srbAuth, NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to srbMaster failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn);
	exit(1);
    }
    if ((argc - optind) == 2)
      sprintf(queryString,"%s|%s",argv[optind],argv[optind + 1]);
    else
      sprintf(queryString,"%s",argv[optind]);

    i = srbBulkQueryAnswer(conn, MDAS_CATALOG, queryString,  &myresult,L_val);
    if (i < 0)
      {
	if (i != MCAT_INQUIRE_ERROR) {
	  fprintf(stderr,"Spullmeta Error: %i\n", i);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	} 
	else {
	  fprintf(stderr,"No Answer\n");
	}
	clFinish(conn);
	exit(1);
      }

    if (Fflag) 
      fprintf(stdout,"%s\n",queryString);
    show_results_simple(conn,  &myresult, L_val, Fflag);

    fflush(stdout);
    clFinish(conn);
    exit(0);
}


