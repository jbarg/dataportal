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

int
bulkIngestFromMetaFile (srbConn *conn,
                         int catType,
                        char *metaFile,
                        mdasC_sql_result_struct *myresult,
                        int numrows);

void
usage()
{
  /***
  fprintf(stderr,"   ACTION_STRING that are supported are:\n");
  fprintf(stderr,"Usage: Spushmeta <ACTION_STRING> <infile>\n");
  fprintf(stderr,"     PUT_USER_TYPE\n");
  fprintf(stderr,"     PUT_USER_DOMAIN\n");
  fprintf(stderr,"     PUT_USER_AUTH_SCHEME\n");
  fprintf(stderr,"     PUT_LOCATION_INFO\n");
  fprintf(stderr,"     PUT_RESOURCE_TYPE\n");
  fprintf(stderr,"     PUT_RESOURCE_CLASS\n");
  fprintf(stderr,"     PUT_RESOURCE_ACCESS_CONSTRAINT\n");
  fprintf(stderr,"     PUT_DATA_TYPE\n");
  fprintf(stderr,"     PUT_DATA_ACCESS_CONSTRAINT\n");
  fprintf(stderr,"     PUT_CHANGED_ZONE_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_USER_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_USER_AUTHMAP\n");
  fprintf(stderr,"     PUT_CHANGED_USER_GROUPS\n");
  fprintf(stderr,"     PUT_CHANGED_USER_UDEF_METADATA\n");
  fprintf(stderr,"     PUT_CHANGED_PHYSICAL_RESOURCE_CORE_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_LOGICAL_RESOURCE_CORE_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_RESOURCE_OTHER_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_RESOURCE_UDEF_METADATA\n");
  fprintf(stderr,"     PUT_CHANGED_RESOURCE_ACCESS\n");
  fprintf(stderr,"     PUT_CHANGED_COLL_CORE_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_DATA_CORE_INFO\n");
  fprintf(stderr,"     PUT_CONTAINER_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_DATA_ACCESS\n");
  fprintf(stderr,"     PUT_CHANGED_COLL_ACCESS\n");
  fprintf(stderr,"     PUT_CHANGED_COLLCONT_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_DATA_UDEFMETA_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_COLL_UDEFMETA_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_DATA_ANNOTATION_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_COLL_ANNOTATION_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_OTHERDATA_INFO\n");
  fprintf(stderr,"     PUT_CHANGED_DATA_GUID_INFO\n");
  fprintf(stderr,"\n");
  ***/
  fprintf(stderr,"Usage: Spushmeta  <inputfile>|-\n");
  fprintf(stderr,"   <inputfile> is a file name; '-' will read from stdin\n");
  fprintf(stderr,"   The input file has contents in the form:\n");
  fprintf(stderr,"COMMANDSTATEMENT\n");
  fprintf(stderr,"ATTRIBUTELIST\n");
  fprintf(stderr,"data (each row in separate line and each value separated by a | \n");
  fprintf(stderr,"data \n");
  fprintf(stderr,"... \n");
  fprintf(stderr,"... \n");
  fprintf(stderr,"data \n");
  fprintf(stderr,"Sample:\n");
  fprintf(stderr,"GET_DATA_TYPE\n");
  fprintf(stderr,"4:DATA_TYP_NAME|324:DATA_TYPE_MIME_STRING|325:DATA_TYPE_EXTENDERS|359:PARENT_DATA_TYPE\n");
  fprintf(stderr,"deleted|||generic\n");
  fprintf(stderr,"gif image|image/gif|.gif|image\n");
  fprintf(stderr,"jpeg image|image/jpeg|.jpeg,.jpg|image\n");
  fprintf(stderr,"pbm image|image/pbm|.pbm|image\n");
  fprintf(stderr,"....\n");




}
int
main(int argc, char **argv)
{
    srbConn *conn;
    int i;
    mdasC_sql_result_struct  myresult;
    int numrows = MAX_QUERY_LIMIT; /* dont change this. is based on packMsg*/
    if (argc != 2) {
      usage();
      exit(1);
    }
    i = initSrbClientEnv();
    if (i < 0) {
      fprintf(stderr,"SgetB Initialization Error:%i\n",i); 
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
    
    i = bulkIngestFromMetaFile(conn, MDAS_CATALOG, argv[1],
			       &myresult, numrows);
    if (i < 0)   {
      fprintf(stderr,"Spushmeta srbBulkMcatIngest Error: %i\n", i);
      srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      fprintf(stderr,"Spushmeta: %s",clErrorMessage(conn));
      clFinish(conn);
      exit(1);
    }
    fflush(stdout);
    clFinish(conn);
    exit(0);
}

int
bulkIngestFromMetaFile (srbConn *conn, 
			 int catType,
			char *metaFile, 
			mdasC_sql_result_struct *myresult, 
			int numrows)
{

 int i,j, k, qq;
 FILE *fd;
 char *buf;
 char *tmpPtr, *tmpPtr2;
 char firstLine[HUGE_STRING];
 int cnt, cntin, cntreg, bufsize;
 int inxInt[MAX_DCS_NUM];
  char ingestInfo[MAX_TOKEN * 2];



   cnt = 0;
   cntin = 0;
   cntreg =  0;
   for (i = 0; i < MAX_SQL_LIMIT; i++) {
     myresult->sqlresult[i].tab_name = NULL;
     myresult->sqlresult[i].att_name = NULL;
     myresult->sqlresult[i].values = NULL;
   }
    myresult->row_count = 0;
    
   if (strcmp(metaFile,"-"))
     fd = fopen(metaFile,"r");
   else
     fd = stdin;

   if (fd == NULL) {
     fprintf(stderr, "can't open metadata file %s, errno = %d\n",
	     metaFile,errno);
     return(-1);
   }


   i = getNextLine(fd,ingestInfo,MAX_TOKEN * 2 - 1);
   if (i == EOF) {
     fprintf(stderr, "Error with Reading File: %s. Note enough lines\n",
	     metaFile);
     return (-1);
   }
   if (strstr(ingestInfo,"GET") == ingestInfo) {
     ingestInfo[0] = 'P';
     ingestInfo[1] = 'U';
   }
   tmpPtr = strstr(ingestInfo,"|");
   if (tmpPtr != NULL)
     *tmpPtr = '\0';


   i = getNextLine(fd,firstLine,HUGE_STRING - 1);
   if (i == EOF) {
     fprintf(stderr, "Error with Reading File: %s. Empty File\n",metaFile);
     return (-1);
   }
   tmpPtr = strtok(firstLine,":");
   while (tmpPtr != NULL) {
     inxInt[cnt] = atoi(tmpPtr);
     cnt++;
     tmpPtr = strtok(NULL,"|");
     if (tmpPtr == NULL)
       break;
     tmpPtr = strtok(NULL,":");
   }
   myresult->result_count = cnt;
   myresult->continuation_index = -1;
   for (k = 0; k < cnt ; k++) {
	  myresult->sqlresult[k].tab_name = strdup(dcs_tname[inxInt[k]]);
	  myresult->sqlresult[k].att_name = strdup(dcs_aname[inxInt[k]]);
	  myresult->sqlresult[k].values  = (char *)  
	    malloc (numrows * MAX_DATA_SIZE);
	  if (myresult->sqlresult[k].values == NULL||
	      myresult->sqlresult[k].tab_name == NULL ||
	      myresult->sqlresult[k].att_name == NULL ) {
	    for (qq = 0  ; qq < k ; qq++) {
	      free(myresult->sqlresult[qq].values);
	      free(myresult->sqlresult[qq].tab_name);
	      free(myresult->sqlresult[qq].att_name);
	    myresult->sqlresult[qq].tab_name = (char *) NULL;
	    myresult->sqlresult[qq].att_name = (char *) NULL;
	    myresult->sqlresult[qq].values  = (char *) NULL; 
	    }
	    return(MEMORY_ALLOCATION_ERROR);
	  }
     
   }
   bufsize = MAX_DATA_SIZE * cnt;
   buf = malloc(bufsize  + 1);
   qq = 1;

   while (qq == 1) {
     k = 0;
     while ((i = getNextLine(fd,buf,bufsize)) != EOF) {
       if (strlen(buf) == 0)
	 continue;
       /***
       tmpPtr = strtok(buf,"|");
       j = 0;
       while (tmpPtr != NULL) {
	 sprintf((char *)
		 (((int) (myresult->sqlresult[j].values))+(k*MAX_DATA_SIZE)),
		 "%s", tmpPtr);
	 j++;
	 tmpPtr = strtok(NULL,"|");
       }
       ***/
       tmpPtr2 = buf;
       tmpPtr = strstr(tmpPtr2,"|");
       j = 0;
       while (tmpPtr != NULL) {
	 *tmpPtr = '\0';
	 sprintf((char *)
		 (((int) (myresult->sqlresult[j].values))+(k*MAX_DATA_SIZE)),
		 "%s", tmpPtr2);
	 j++;
	 tmpPtr2 = tmpPtr+1;
	 tmpPtr = strstr(tmpPtr2,"|");
       }
       sprintf((char *)
	       (((int) (myresult->sqlresult[j].values))+(k*MAX_DATA_SIZE)),
	       "%s", tmpPtr2);
       j++;
       
	 
       if (j > cnt){
	 fprintf(stderr,"More data in a row than required by the header\n");
	 fprintf(stderr,"ROW: %s\n",buf);
       }
       k++;
       if (k == numrows)
	 break;
     }
     if (k < numrows)
       qq = 0;
     myresult->row_count = k;
     /****/
     i = srbBulkMcatIngest(conn, catType, ingestInfo,  myresult);
     if (i < 0) {
       fprintf(stderr,"Error in srbBulkMcatIngest\n");
       return(i);
     } 
     /****/
     /******
     printf("ingestCommand:%s\n",ingestInfo);
     printf("myresult->row_count:%i\n",myresult->row_count);
     printf("myresult->result_count:%i\n",myresult->result_count);
    for (i=0; i < myresult->result_count; i++) {
     rval[i] = myresult->sqlresult[i].values;
     tval[i] = myresult->sqlresult[i].tab_name;
     aval[i] = myresult->sqlresult[i].att_name;
    }
    i = 0;
    while ( i < myresult->row_count) {
      for (j=0; j < myresult->result_count; j++) {
	fprintf(stdout, "%s:%s\n",aval[j], rval[j]);
	rval[j] += MAX_DATA_SIZE;
      }
      if (j > 1)
	 printf("---------------------------------------------------\n"); 
     
      i++;
    }
     **********/
     cntin += i;
     cntreg += k;
     fprintf(stdout,"Stored/Modified %i items out of %i\n",cntin, cntreg);
   }
   return (0);   
}
