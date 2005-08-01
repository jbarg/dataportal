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
extern char versionString[];
extern int  versionFlag; 
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
    int lcval = 0;


void
cat_files(int argc, char argv[][MAX_OBJ_NAME_LENGTH])
{
  int i;
  char tmpStr[MAX_OBJ_NAME_LENGTH];

    for (i = 1; i < argc ; i++)
      {
	clSplitbychar(argv[i],collname,tmpStr, '/');
	if (copyNum < 0)
	  strcpy(dataname, tmpStr);
	else
	  sprintf(dataname, "%s&COPY=%i",tmpStr,copyNum);
	if (versionFlag > 0) {
          strcat(dataname,"&VNUM='");
	  strcat(dataname,versionString);
	  strcat(dataname,"'");
	}
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
	/*	
	if (lcval == 1) {
	  sprintf(collname,"");
	  sprintf(dataname,"&%s",inCondition);
	}
	*/
	in_fd = srbObjOpen (conn, dataname,  O_RDONLY, collname);
	if (in_fd < 0)  
	  {   /* error */
	    fprintf(stderr, "Scat: can't open SRB obj \"%s/%s:%i\"\n", 
		    collname, dataname ,in_fd);
	    fprintf(stderr,"Scat: %s",clErrorMessage(conn));
	    srb_perror (2, in_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  }
	else
	  {
	    while ((nbytes = srbObjRead (conn, in_fd, buf, BUFSIZE)) > 0) 
	      {
		fwrite(buf, 1,nbytes,stdout);
		/*		printf ("%-*.*s", nbytes,nbytes,buf); */
	      }
	    srbObjClose (conn, in_fd);
	  }
      }
}
void
usage()
{
  fprintf(stderr, "Usage: Scat [-C n] [-T ticketFile | -t ticket] [-V versionString] [-A condition]   srbObj  ... \n");
  fprintf(stderr, "Usage: Scat -c  condition   \n");
}

int
main(int argc, char **argv)
{
    int  newargc;
    int i;
    char newargv[MAX_TOKEN/4][MAX_OBJ_NAME_LENGTH];
    int mval = 1;
    strcpy(ticketId , "");
    strcpy(inCondition , "");

    if (argc < 2) 
      {usage();
      exit(1);
      }
    

       while (mval < argc && argv[mval][0] == '-')
	 {
	   if (argv[mval][1] == 'C')
	     {if (argc < mval +2) {usage(); exit(1);}
	      copyNum = atoi(argv[++mval]); mval++;}
	   else if (argv[mval][1] == 'A')
	     {if (argc < mval +2) {usage(); exit(1);}
	     strcpy(inCondition, argv[++mval]); mval++;}
	   else if (argv[mval][1] == 't')
	     {if (argc < mval +2) {usage(); exit(1);}
	      strcpy(ticketId, argv[++mval]); mval++;}
	   else if (argv[mval][1] == 'c')
	     {if (argc != 3) {usage(); exit(1);}
	      strcpy(inCondition, argv[++mval]); lcval = 1; mval++;}
           else if (argv[mval][1] == 'V')
             {
              strcpy(versionString, argv[++mval]); versionFlag = 1; mval++;}
	   else if (argv[mval][1] == 'T')
	     {
	       if (argc < mval +2) {usage(); exit(1);}
	      fd = fopen(argv[++mval], "r");
	       if (fd == NULL) 
		 {
		   fprintf(stderr, 
			   "Scat: Unable to open Ticket File %s\n",argv[mval]);
		   exit(1);
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

    

    i = initSrbClientEnv();
   if (i < 0)
      {printf("Scat: Initialization Error:%i\n",i); exit(1);}

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Scat: Connection to srbMaster failed.\n");
        fprintf(stderr,"Scat: %s",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    if (mval < argc)
      expand_file_names(conn, mval, argc, argv, &newargc,newargv);
    else if (lcval == 1) {
      strcpy(mdasCollectionName,"");
      strcpy(argv[argc-1],"");
      mval = argc - 1;
      expand_file_names(conn,  mval, argc, argv, &newargc,newargv);
    }
    else
      {
	 if (strlen(ticketId) > 0  )
	   {
	     newargc = 2;strcpy(newargv[1], "");
	   }
	 else
	   {
	     fprintf(stderr, "Scat Wrong Usage\n");
	     usage();
	     clFinish(conn); exit(4);
	   }
      }

    /*printf("ARGC=%i\n",newargc);
      for (i = 1; i < newargc ; i++) printf("%i:%s\n",i,newargv[i]); */

    cat_files(newargc,  newargv); 
    clFinish(conn);
    exit(0);
}
