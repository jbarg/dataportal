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
extern char mdasCollectionHome[];
extern char inCondition[];
extern char srbUser[];
extern char mdasDomainName[];
extern int versionFlag; 
extern char versionString[];

srbConn *conn;

int
replMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
char *pathname, char *resourcename, int flagval, int copyNum);

int
expandReplData (srbConn *conn, int catType, char *dataName,
char *pathname, char *resourcename, int flagval, int copyNum);

int
ingestReplica(srbConn *conn, int catType,  char *inSrbObject, char *localFile,
              char *pathname, char *resourcename,int copyNum,
              char *datatype, char *versionString);
int
getPathNameAndSize(srbConn *conn, int catType,
		   char *collName, char *dataName, int replNum, char *pathName, int *dataSize);

void usage(char *prog)
{
	   
  fprintf(stderr, 
   "Usage: %s [-n replicaNum] [-prv] [-V versionString]  [-S resourceName] [-P pathName] srbObj|collection  ...\n", prog);
  fprintf(stderr,
   "Usage: %s -I [-n replicaNum] [-V versionString] [-S resourceName] [-P pathName] [-D dataType] localFileName srbObj\n", prog);
}
int
main(int argc, char **argv)
{
    int i;
    char resourcename[MAX_TOKEN];
    char pathname[MAX_TOKEN];
    int c;
    srbConn *conn;
    int SFlag = 0;              /* -S option */
    int PFlag = 0;              /* -P option */
    int flagval = 0;
    int nArgv;          /* number of inpArgv */
    srbPathName nameArray[MAX_TOKEN];
    int   copyNum = -1;
    int IFlag = 0;
    char datatype[MAX_TOKEN];

    if (argc < 2) {
	usage(argv[0]); 
	exit(1);
    }
    strcpy(inCondition , "");
    strcpy(datatype, "");
    strcpy(versionString, "");
    while ((c=getopt(argc, argv,"In:D:S:P:prvhV:")) != EOF) {
        switch (c) {
            case 'n':
                copyNum = atoi(optarg);
                break;
            case 'S':
                SFlag = 1;
                strcpy (resourcename, optarg);
                break;
            case 'I':
                IFlag = 1;
                break;
            case 'D':
	        strcpy (datatype, optarg);
                break;
            case 'P':
                PFlag = 1;
                strcpy (pathname, optarg);
                break;
            case 'V':
                strcpy (versionString, optarg);
		versionFlag = 1;
                break;
            case 'p':
                flagval |= P_FLAG;
                break;
            case 'r':
                flagval |= R_FLAG;
                break;
            case 'v':
                flagval |= v_FLAG;
                break;
  	    case 'h':	
            default:
                usage (argv[0]);
                exit (1);
        }
    }

    if (argc - optind < 1) {    /* must have at least 1 input */
        usage (argv[0]);
        exit (1);
    }

    if (SFlag == 0)
        strcpy (resourcename, "");

    if (PFlag == 0)
        strcpy (pathname, "");

    i = initSrbClientEnv();
    if (i < 0)
      {fprintf(stderr, "Sreplicate: Initialization Error:%i\n",i); exit(-1);}


    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sreplicate: Connection to srbMaster failed.\n");
        fprintf(stderr,"Sreplicate: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    if (strlen(resourcename) == 0)
	  get_user_preference(resourcename, "DRSRC");

    if (IFlag == 1) {
      i = ingestReplica(conn, MDAS_CATALOG,argv[argc-1],argv[argc-2],
			pathname, resourcename,copyNum,datatype,versionString);
      clFinish(conn);
      if (i < 0)
	exit(5);
      exit(0);

    }

    for (i = optind; i < argc; i++) {
        nameArray[i - optind].inpArgv = argv[i];
    }
    nArgv = argc - optind;          /* nArgv is the number of in file */

    /* Convert the source name to SQL like */
    if (convNameArrayToSQL (nameArray, nArgv) < 0)
      {clFinish(conn); exit(4);} 
    
    i = replMain (conn, MDAS_CATALOG, nArgv, nameArray, pathname, resourcename,
     flagval, copyNum);
    clFinish(conn);
    if (i < 0) 
      exit(5);
    exit(0);

}

int
replMain (srbConn *conn, int catType, int nArgv, srbPathName nameArray[],
char *pathname, char *resourcename, int flagval, int copyNum)
{
    int i;
    int status1 = 0;
    int status = 0;
    int status2 = 0; 
    for (i = 0; i < nArgv; i++) {
        /* May be it is a dataName */
        if (nameArray[i].type != COLLECTION_T && 
	 nameArray[i].type != SPEC_COLL_T) {
	    status1 = expandReplData (conn, catType, nameArray[i].outArgv,
              pathname, resourcename, flagval, copyNum);
        }
        if (flagval & R_FLAG)
            status = replInColl (conn, catType, nameArray[i].outArgv,
             pathname, resourcename, flagval, copyNum);
	if (status == 0) {
	    if (status1 < 0)
		status = status1;
	}
        if (status < 0) {
            fprintf (stderr, "Replication error for %s. Status = %d.\n",
              nameArray[i].inpArgv, status);
	    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    status2 = status;
	}
    }
    return (status2);
}

/* expandReplData - Replicate the dataName including the effects of special 
 * char.  put the results in dataResult.
 *
 */

int
expandReplData (srbConn *conn, int catType, char *dataName, 
char *pathname, char *resourcename, int flagval, int copyNum)
{
    int status;
    mdasC_sql_result_struct dataResult;

  
    status = queryData (conn, catType, dataName, flagval, MAX_GET_RESULTS,
      &dataResult);

    if (status < 0) {
	if (status == MCAT_INQUIRE_ERROR)
	    return 0;
	else
            return status;
    }

    status = replDataByRes (conn, catType, pathname, resourcename, 
	flagval, &dataResult, copyNum);

    if (status < 0) {
        if (status == MCAT_INQUIRE_ERROR)
            return 0;
        else {
	    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return status;
	}
    }

    /* loop until no more answer */

    while (dataResult.continuation_index >= 0) {
        clearSqlResult (&dataResult);
        status = srbGetMoreRows(conn, MDAS_CATALOG,
          dataResult.continuation_index, &dataResult, MAX_GET_RESULTS);
        if (dataResult.row_count == 0  || status != 0)
            return (0);
        replDataByRes (conn, catType, pathname, resourcename, flagval, 
	 &dataResult, copyNum);
    }
    clearSqlResult (&dataResult);
    return (0);
}

int
ingestReplica(srbConn *conn, int catType,  char *inSrbObject, char *localFile,
	      char *pathname, char *resourcename,int copyNum, 
	      char *datatype, char *versionString)
{

   char myDataName[MAX_TOKEN];
   char dataName[MAX_TOKEN];
   char collName[MAX_TOKEN];
   int out_fd, dataSize;
   FILE *in_fd;
   char *buf;
   int nbytes,wbytes;
   char replNumString[MAX_TOKEN];
   char verNumString[MAX_TOKEN];

   splitpath(inSrbObject,collName,dataName); 
#ifdef _WIN32
   sprintf(myDataName,"%s##%i",dataName,getpid());
#else
   sprintf(myDataName,"%s##%i",dataName,getppid());
#endif
   out_fd = srbObjCreate (conn,catType,myDataName, datatype,
			  resourcename,collName,pathname,0);
   if (out_fd < 0) {
     fprintf (stderr,"Sreplicate: Error: Unable to create temporary object in SRB:%s/%s \n",collName,myDataName );
     srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     return (out_fd);
   }
   if (!strcmp(localFile, "-"))
     in_fd = stdin;
   else {
     in_fd = fopen(localFile,"r");
     if (in_fd == NULL) {
       fprintf (stderr,"Sreplicate: Error: Unable to open local file errno = %d\n",errno);
       return(-1);
     }
   }
   buf = malloc (BUFSIZE+1);
   if (buf == NULL) {
      fprintf (stderr, "Sreplicate: Unable to malloc buf\n");
      srbObjClose (conn, out_fd);
      fclose(in_fd);
      return (-1);
   }
   while ((nbytes = fread(buf, 1, BUFSIZE,in_fd ))>0) {
     wbytes = srbObjWrite(conn, out_fd, buf, nbytes);
     if (wbytes != nbytes) {  
          fprintf(stderr, "Sreplicate: write error in srbObject %s/%s: %i\n",
                  collName,myDataName,wbytes);
          fprintf(stderr, "Sreplicate: Exiting \n");
          srbObjClose (conn, out_fd);
          fclose(in_fd);
          free(buf);
          return -1;
     }
   }
   free(buf);
   srbObjClose (conn, out_fd);
   fclose(in_fd);
   
/*** register the temporary as a replica to original object****/
   i = getPathNameAndSize(conn, MDAS_CATALOG,collName, myDataName,0, pathname,&dataSize);
   if (i < 0) {
     printf ("Sreplicate:Error when getting path name for object %s/%s. \n",
	     collName, myDataName);
     srbObjUnlink(conn, myDataName,collName);
     return(-1);
   }
   strcat(pathname," ");
   j = srbRegisterReplica (conn, MDAS_CATALOG, dataName,
			   collName, "","",resourcename,
			   pathname,srbUser,mdasDomainName);
   if (j < 0) {
     printf ("Sreplicate:Error when registering replica for object %s/%s. \n",
	     collName, myDataName);
     srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     return(-1);
   }
   /*** unlink the the newly created temporary  file ****/
   i = srbModifyDataset(conn, MDAS_CATALOG, myDataName,collName,"","",
                        "","", D_DELETE_ONE); /**D_CHANGE_DPATH);**/
   if (i < 0) {
     printf ("Sreplicate:Error when unlinking temporary object %s/%s\n",
	     collName, myDataName);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     return(-1);
   }
   
   sprintf(dataName,"%s&COPY=%i",dataName,j);
   pathname[strlen(pathname) -1] = '\0';
   i = srbModifyDataset(conn, MDAS_CATALOG, dataName,collName, "","",
			pathname,"", D_CHANGE_DPATH);
   if (i < 0) {
     printf ("Sreplicate:Error in Datapath Change for %s/%s to %s\n",
             collName,dataName,pathname);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     return(-1);
   }
   
   if (strlen(datatype) > 0) {
     i = srbModifyDataset(conn, MDAS_CATALOG, dataName,collName, "","",
			  datatype,"", D_CHANGE_TYPE);
     if (i < 0) {
       printf ("Sreplicate:Error in Datatype Change for %s/%s to %s\n",
	       collName,dataName,datatype);
       srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       return(-1);
     }
   }
   if (copyNum != -1 || strlen(versionString) > 0) {
     if (copyNum != -1)
       sprintf(replNumString,"%i:%i",j,copyNum);
     else
       sprintf(replNumString,"%i:%i",j,j,j);
     if (strlen(versionString) > 0)
       sprintf(verNumString,"-1:%s",versionString);
     else
       sprintf(verNumString,"-1:-1");
     i = srbModifyDataset(conn, MDAS_CATALOG, dataName,collName, "","",
			  replNumString,verNumString, D_CHANGE_REPLNUM_VERNUM);
     if (i < 0) {
       printf ("Sreplicate:Error in Version Change for %s/%s to %s\n",
               collName,dataName,verNumString);
       srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       return(-1);
     }
   }
   sprintf(pathname,"%i",dataSize);
   i = srbModifyDataset(conn, MDAS_CATALOG, dataName,collName, "","",
			pathname,"", D_CHANGE_SIZE);
   if (i < 0) {
     printf ("Sreplicate:Error in DataSize Change  for %s/%s to %i\n",
             collName,dataName,dataSize);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     return(-1);
   }
   return(0);
}

int
getPathNameAndSize(srbConn *conn, int catType,
		   char *collName, char *dataName, int replNum, char *pathName, int *dataSize)
{
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, status;
    char *nameList;
    char *sizeList;
    for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
    }
   selval[PATH_NAME] = 1;
   selval[SIZE] = 1;
   sprintf(qval[DATA_NAME],"= '%s'",dataName);
   sprintf(qval[DATA_REPL_ENUM],"= %i",replNum);
   sprintf(qval[DATA_GRP_NAME],"= '%s'",collName);

   status = srbGetDataDirInfo(conn, catType , qval, selval, &myResult, 3);
   if (status < 0 || myResult.row_count == 0)
     return -1;

    nameList = (char *) getFromResultStruct(
      &myResult, dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);
    sizeList = (char *) getFromResultStruct(
      &myResult, dcs_tname[SIZE], dcs_aname[SIZE]);
    strcpy(pathName, nameList);
    *dataSize = atoi(sizeList);
    
    free(nameList);
    free(sizeList);
   return 0;
}

