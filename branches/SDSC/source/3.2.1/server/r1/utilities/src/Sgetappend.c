/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

#include "srbClient.h"
#include "srb_client_errno.h"


extern char srbAuth[];
extern char srbHost[];
extern char mdasCollectionName[];
extern char inCondition[];


    char ticketId[MDAS_TICKET_SIZE+2];
    srbConn *conn;
    int  nbytes, wbytes, in_fd;
    char buf[BUFSIZE];
    int i;
    char collname[MAX_TOKEN];
    char dataname[MAX_TOKEN];
    char targetfile[MAX_TOKEN];
    FILE *out_fd, *fd;
    int copyNum =-1;


void
getappend_files(char srbobj[][MAX_OBJ_NAME_LENGTH],
	  char *target, 
	  int   pval,
	  int   lcount)
{
  char inbuf[100];
  char tmpStr[MAX_TOKEN];

  for (i = 1; i < lcount ; i++)
    {

      if (lcount == 2)
	{
	  if (pval == 1)
	    {
	      if (copyNum < 0)
		fprintf(stdout,"Append  %s to %s (y/n)?",
			srbobj[i],target);
	      else
		fprintf(stdout,"Append  %s&COPY=%i to %s (y/n)?",
			srbobj[i],copyNum , target);
	    gets(inbuf);
	    if (inbuf[strlen(inbuf)-1] != 'y' )
	      continue;
	    }
	  out_fd =  fopen(target,"a");
	  if (out_fd != NULL)
	    {

	      clSplitbychar(srbobj[i],collname,dataname,'/');
	      if (copyNum >=  0)
		{ 
		  sprintf(tmpStr, "&COPY=%i",copyNum);
		  strcat(dataname,tmpStr);
		}
	      
	      if (strlen(ticketId) > 0)
		{
		  strcat(dataname,"&TICKET='");
		  strcat(dataname,ticketId);
		  strcat(dataname,"'");
		}

	    in_fd = srbObjOpen (conn, dataname,  O_RDONLY, collname);
	    if (in_fd < 0)  
	      {  
		if (copyNum < 0)
		  fprintf(stderr, 
			  "Sgetappend: cannot open SRB obj '%s':%i\n", 
			srbobj[i],in_fd);
		else
		  fprintf(stderr, 
			  "Sgetappend: cannot open SRB obj '%s&COPY=%i':%i\n", 
			srbobj[i],copyNum,in_fd);
		srb_perror (2, in_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
		fclose(out_fd);
		return;
	      }
	    else
	      {
		while ((nbytes = srbObjRead (conn, in_fd, buf, BUFSIZE)) > 0) 
		  {
		    wbytes = fwrite(buf,1,nbytes,out_fd);
		    if (wbytes != nbytes) { 
		      fprintf(stderr, "Sgetappend: write error in file '%s'\n", 
			      srbobj[i]);
		      fclose(out_fd);
		      srbObjClose (conn, in_fd);
		      return;
		    }
		  }
		if (nbytes < 0 && nbytes != EOF)
		    srb_perror (2, nbytes, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
 		srbObjClose (conn, in_fd);
		fclose(out_fd);
		return;
	      }
	    }
	  else
	    {
	      fprintf(stderr, "Sgetappend: Error Unable to open target file %s\n",
		      target);
	      return;
	    }
	}
      clSplitbychar(srbobj[i],collname,dataname,'/');

      sprintf(targetfile, "%s/%s",target,dataname);
	  if (pval == 1)
	    {
	      if (copyNum < 0)
		fprintf(stdout,"Copy  %s to %s(y/n)?",
			srbobj[i],targetfile);
	      else
		fprintf(stdout,"Copy  %s&COPY=%i to %s (y/n)?",
			srbobj[i],copyNum , targetfile);
	    gets(inbuf);
	    if (inbuf[strlen(inbuf)-1] != 'y' )
	      continue;
	    }
      out_fd =  fopen(targetfile,"w");
      if (out_fd == NULL)  
	{   
	  fprintf(stderr, "Sgetappend: cannot open output target file '%s'\n", 
		  targetfile);
	}
      else
	{
	  if (copyNum >=  0)
	    { 
	      sprintf(tmpStr, "&COPY=%i",copyNum);
	      strcat(dataname,tmpStr);
	    }
	  in_fd = srbObjOpen (conn, dataname,  O_RDONLY, collname);
	  if (in_fd < 0)  
	    {  
	      if (copyNum < 0)
		fprintf(stderr, "Sgetappend: cannot open SRB obj '%s':%i\n", 
			srbobj[i],in_fd);
	      else
		fprintf(stderr, "Sgetappend: cannot open SRB obj '%s&COPY=%i':%i\n", 
			srbobj[i],copyNum,in_fd);
	      srb_perror (2, in_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	      fclose(out_fd);
	    }
	  else
	    {
	      while ((nbytes = srbObjRead (conn, in_fd, buf, BUFSIZE)) > 0) 
		{
		  wbytes = fwrite(buf,1,nbytes,out_fd);
		  if (wbytes != nbytes) { 
		    fprintf(stderr, "Sgetappend: write error for file '%s'\n", 
			    srbobj[i]);
		    srbObjClose (conn, in_fd);
		  }
		}
		if (nbytes < 0 && nbytes != EOF)
		    srb_perror (2, nbytes, "", 
		     SRB_RCMD_ACTION|SRB_LONG_MSG);
	      srbObjClose (conn, in_fd);
	      fclose(out_fd);
	    }
	}
    }
}

void
usage()
{
  fprintf(stderr, "Usage: Sgetappend [-C n] [-p] [-T ticketFile | -t ticket]   srbObj  ...  localFile \n");
}

int
main(int argc, char **argv)
{

    char srbobj[MAX_TOKEN][MAX_OBJ_NAME_LENGTH];
    char *target;
    int pval, mval, tflag, lcount;

    strcpy(ticketId , "");
    strcpy(inCondition , "");
    if (argc < 3) 
      {usage();
      exit(1);
      }
    pval = 0;
    target  = NULL;
    lcount = 0;
    mval = 1;
    while (mval < argc && argv[mval][0] == '-')
      {
	if (argv[mval][1] == 'p')
	  { pval = 1; mval++;}
	else if  (argv[mval][1] == 'C')
	  {if (argc < mval +2) {usage(); exit(1);}
	   copyNum = atoi(argv[++mval]); mval++; }
	else if  (argv[mval][1] == 'A')
	  {if (argc < mval +2) {usage(); exit(1);}
	   strcpy(inCondition,argv[++mval]); mval++; }
	else if (argv[mval][1] == 't')
	  {if (argc < mval +2) {usage(); exit(1);}
	  strcpy(ticketId, argv[++mval]); mval++;}
	else if (argv[mval][1] == 'T')
	  {
	    if (argc < mval +2) {usage(); exit(1);}
	      fd = fopen(argv[++mval], "r");
	      if (fd == NULL) 
		{
		   fprintf(stderr, 
			   "Sgetappend: Unable to open Ticket File %s\n",argv[mval]);
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
    target = argv[argc-1];
    i = initSrbClientEnv();
    if (i < 0)
      {printf("Sgetappend: Initialization Error:%i\n",i); exit(1);}

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sgetappend: Connection to srbMaster failed.\n");
        fprintf(stderr,"Sgetappend: %s\n",clErrorMessage(conn));
	fprintf(stderr,"Sgetappend: Exiting \n");
        clFinish(conn); exit(3);
    }

    if (mval < argc-1)
      expand_file_names(conn, mval, argc-1, argv, &lcount,srbobj);
    else
      {
	 if (strlen(ticketId) > 0)
	   {
	     lcount = 2;strcpy(srbobj[1], "");
	   }
	 else
	   {
	     fprintf(stderr, "Sgetappend Wrong Usage\n");
	     usage();
	     clFinish(conn); exit(4);
	   }
      }
    /* expand_file_names(conn, mval, argc-1, argv, &lcount,srbobj);*/
    if (lcount > 1)
      getappend_files(srbobj, target, pval,lcount);
    else
      fprintf(stderr,"Sgetappend: srbObj does not exist\n");
    clFinish(conn);
    exit(0);
}

