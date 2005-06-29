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

    srbConn *conn;
    int  nbytes,  in_fd, out_fd;
    char buf[BUFSIZE];
    char collname[MAX_TOKEN];
    char dataname[MAX_OBJ_NAME_LENGTH];
    char filename[MAX_TOKEN];
    char ticketId[MDAS_TICKET_SIZE+2];

    FILE * infile_fd, *fd;
    int L_case = 0;
    int copyNum = -1;
    char envstr[MAX_TOKEN];
    char envstr2[MAX_TOKEN];


void

tcat_files(int argc, char argv[][MAX_OBJ_NAME_LENGTH])
{
  char tmpStr[MAX_OBJ_NAME_LENGTH];

    for (i = 1; i < argc ; i++)
      {
	strcpy(collname,"");
	strcpy(tmpStr,"");
	clSplitbychar(argv[i],collname,tmpStr, '/');
	if (copyNum < 0)
	  strcpy(dataname, tmpStr);
	else
	  sprintf(dataname, "%s&COPY=%i",tmpStr,copyNum);
	if (strlen(ticketId) > 0)
	  {
	    strcat(dataname,"&TICKET='");
	    strcat(dataname,ticketId);
	    strcat(dataname,"'");
	  }
	if (strlen(inCondition) > 0)
	  {
	    strcat(dataname,"&");
	    strcat(dataname,inCondition);
	  }
	/****** removed RAJA June 22, 2004 
#ifdef FED_MCAT
	if (strlen (collname) == 0) {
	    initSrbClientEnv();
	    strcpy (collname ,mdasCollectionName);
	}
#endif
	******/
	in_fd = srbObjOpen (conn, dataname,  O_RDONLY, collname);
	if (in_fd < 0)  
	  {   /* error */
	    if (copyNum < 0)
	      fprintf(stderr, "Stcat: can't open SRB obj \"%s:%i\"\n", 
		      argv[i],in_fd);
	    else
	      fprintf(stderr, "Stcat: can't open SRB obj \"%s&COPY=%i:%i\"\n", 
		      argv[i],copyNum ,in_fd);
	  }
	else
	  {
	    while ((nbytes = srbObjRead (conn, in_fd, buf, BUFSIZE)) > 0) 
	      {
		fwrite(buf, 1,nbytes,stdout);
		/*    printf ("%-*.*s", nbytes,nbytes,buf); */
	      }
	    srbObjClose (conn, in_fd);
	  }
      }
}
void
usage()
{
  fprintf(stderr, "Usage: Stcat  [-T ticketFile | -t ticket] [-A condition] [-H hostName]   srbObj  ... \n");
}
int
main(int argc, char **argv)
{
    int  newargc;
#ifdef _WIN32
    char *newargv;   /* need to be very careful since we actually want to build a 2d array. */
#else
    char newargv[MAX_TOKEN][MAX_OBJ_NAME_LENGTH];
#endif

    int mval = 1;
    char hostName[MAX_TOKEN];

#ifdef _WIN32
    newargv = (char *)malloc (MAX_TOKEN * MAX_OBJ_NAME_LENGTH);
#endif

    strcpy(ticketId , "");
    strcpy(inCondition , "");

    if (argc < 2) 
      {usage();
      exit(1);
      }
    

    while (mval < argc && argv[mval][0] == '-') /* Roman 7-25-2000 */
	 {
	   if (argv[mval][1] == 't')
	     {if (argc < mval +2) {usage(); exit(0);}
	      strcpy(ticketId, argv[++mval]); mval++;}
	   else if (argv[mval][1] == 'A')
	     {if (argc < mval +2) {usage(); exit(0);}
	     strcpy(inCondition, argv[++mval]); mval++;}
	   else if (argv[mval][1] == 'H')
	     {if (argc < mval +2) {usage(); exit(0);}
	     strcpy(hostName, argv[++mval]); mval++;}
	   else if (argv[mval][1] == 'T')
	     {
	       if (argc < mval +2) {usage(); exit(0);}
	       fd = fopen(argv[++mval], "r");
	       if (fd == NULL) 
		 {
		   fprintf(stderr, 
			   "Stcat: Unable to open Ticket File %s\n",argv[mval]);
		   exit(0);
		 }
	       fgets(ticketId, MDAS_TICKET_SIZE + 1, fd); 
	       fclose(fd);
	       mval++;
	     }
	   else
	     {fprintf(stderr, "Unknown Option\n");
	      usage();
	     exit(1);
	     }
	 }

    /**** RAJA removed June 22 2004 
    
    if ( mval < argc )                  Roman 7-25-2000 
    {strcpy(hostName,argv[mval++]);}

    else  if (strlen(hostName) == 0)   Roman 7-25-200 
    {usage(); exit(0);}
    *****/
    if (strlen(hostName) == 0)
      {
	i = initSrbClientEnv();
	if (i < 0)
	  {printf("Stcat Initialization Error:%i\n",i); 
	   printf("SRB Host Name not found\n");
	   exit(-1);}
	strcpy(hostName,srbHost);
      }
    else {
      /*** added by RAJA June 22 2004 ***/
      strcpy(envstr,"srbUser=ticketuser");
      putenv(envstr);
      strcpy(envstr2,"mdasDomainName=sdsc" );
      putenv(envstr2);
      strcpy(srbAuth,"CANDO");
      /*** added by RAJA June 22 2004 ***/
    }

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Stcat: Connection to srbMaster failed.\n");
        fprintf(stderr,"Stcat: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	clFinish(conn); exit(3);
    }



    if (mval < argc)
#ifdef _WIN32
      expand_file_names(conn, mval, argc, argv, &newargc, (char (*) [MAX_OBJ_NAME_LENGTH])newargv);
#else
      expand_file_names(conn, mval, argc, argv, &newargc,newargv);
#endif
    else
      {
	 if (strlen(ticketId) > 0)
	   {
	     newargc = 2;
#ifdef _WIN32
         newargv[MAX_OBJ_NAME_LENGTH] = '\0';
#else
         strcpy(newargv[1], "");
#endif
	   }
	 else
	   {
	     fprintf(stderr, "Stcat Wrong Usage\n");
	     usage();
	     clFinish(conn); exit(4);
	   }
      }

    /*printf("ARGC=%i\n",newargc);
      for (i = 1; i < newargc ; i++) printf("%i:%s\n",i,newargv[i]); */

#ifdef _WIN32
    tcat_files(newargc,  (char (*) [MAX_OBJ_NAME_LENGTH])newargv); 
#else
    tcat_files(newargc,  newargv); 
#endif

    clFinish(conn);
    exit(0);
}
