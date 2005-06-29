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
  fprintf(stderr,"Usage: SmodColl [-dh] [-c container | -T ]  collectionName\n");
}

int
main(int argc, char **argv)
{
  srbConn *conn;
  int dFlag = 0;
  int cFlag = 0;
  int tFlag = 0;
  char collection[MAX_TOKEN], temp[MAX_TOKEN], temp1[MAX_TOKEN];
  char container[MAX_TOKEN];
  char contName[MAX_TOKEN], contCollection[MAX_TOKEN];
   char newValue[MAX_TOKEN];
   char *userName, *domainName;
   int copyNum = -1;
   int mval = 1;
   int i;
   int c;
    char *mcatName;
#ifdef FED_MCAT
    char myMcatName[MAX_TOKEN];
#endif


    while ((c=getopt(argc, argv,"hdc:T")) != EOF) {
        switch (c) {
            case 'h':
		usage ();
                exit (0);
	    case 'd':
		dFlag = 1;
		break;
	    case 'c':
		cFlag = 1;
		strcpy (container, optarg);
		break;
	    case 'T':
		tFlag = 1;
		break;
            default:
                usage ();
                exit (1);
        }
    }

	
    if (argc - optind < 1)
      {usage(); exit(1);}

    i = initSrbClientEnv();
    if (i < 0)
      {printf("SmodColl: Initializatin Error:%i\n",i); exit(1);}
    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
      fprintf(stderr,"Connection to srbMaster failed.\n");
      fprintf(stderr,"%s",clErrorMessage(conn));
      srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      clFinish(conn); exit(3);
    }

    if (conn->clientUser == NULL) {
      userName = conn->proxyUser->userName;
      domainName = conn->proxyUser->domainName;
    } else {
      userName = conn->clientUser->userName;
      domainName = conn->clientUser->domainName;
    }

    sprintf(temp,"%s/alpha",argv[argc-1]);
    splitpath(temp,collection, temp1);

    if (tFlag) {
      i = mvCollToTrash(conn,  collection, userName, domainName);
      if (i < 0)  {
	fprintf(stderr,"SmodColl: Error for collection '%s' when moving to trash : %i\n", 
	      collection,i);
	srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn);
	exit(3);
      }
      clFinish(conn);
      exit(0);
    }
    if (cFlag) { /* container for collections */
#ifdef FED_MCAT
        if (container[0] != '/') {    /* full path */
	    int status;
        char *thisMcat = NULL;

            thisMcat = getmcatNamefromHint (collection);
            if (thisMcat == NULL) {
                status =srbGetMcatZone (conn, userName, domainName,
                 myMcatName);
                if (status >= 0)
                    mcatName = myMcatName;
                else
                    mcatName = NULL;
            } else if (strcmp (thisMcat, "home") == 0 ||
             strcmp (thisMcat, "container") == 0) {
                mcatName = NULL;
            } else {
                    mcatName = thisMcat;
            }
        } else {
            mcatName = NULL;
        }
#else
        mcatName = NULL;
#endif
      parseContainerName (container, contName, contCollection,
       userName, domainName, mcatName);
      sprintf (container, "%s/%s", contCollection, contName);

      if (dFlag) { /* delete */
	i = srbModifyCollect(conn, MDAS_CATALOG,collection,
			     container,"","",
			     C_DELETE_CONTAINER_FOR_COLLECTION);
      }
      else {
	i = srbModifyCollect(conn, MDAS_CATALOG,collection,
			     container,"","",
			     C_INSERT_CONTAINER_FOR_COLLECTION);
      }
    }

    if (i < 0)  {
      fprintf(stderr,"SmodColl: Error for collection '%s': %i\n", 
	      collection,i);
      srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      clFinish(conn); exit(4);
    }
    clFinish(conn);
    exit(0);
}

int
mvCollToTrash(srbConn *conn,  char *collection, 
	      char *userName, char *domainName)
{

  int i;

  i = srbModifyCollect(conn, MDAS_CATALOG,collection,"","","",
		       C_MOVE_COLL_TO_TRASH);
  return(i);

}
