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


    srbConn *conn;
    int  nbytes, wbytes, in_fd;
    char buf[BUFSIZE];
    char collname[MAX_TOKEN];
    char dataname[MAX_OBJ_NAME_LENGTH];
    char filename[MAX_TOKEN];

    FILE * infile_fd;
    int  out_fd;
    char datatypename[MAX_TOKEN];
    char resourcename[MAX_TOKEN];
    char pathname[MAX_TOKEN];
    int  datasize;

int
put_app_files(int argc, char argv[][MAX_TOKEN])
{
    int i;
    for (i = 1; i < argc ; i++)
      {
	if ((char) argv[i][0] == '+')
	  {
	    strcpy(filename,(char *)((int) argv[i]+1));
#ifdef _WIN32
        infile_fd = SrbNt_fopen(filename,"r");
#else
	    infile_fd =  fopen(filename,"r");
#endif
	    if (infile_fd == NULL)  
	      {   /* error */
	      fprintf(stderr, "Sappend: can't open local file \"%s\"\n",filename);
	      fprintf(stderr, "Sappend: Exiting \n");
	      srbObjClose (conn, out_fd);
	      clFinish(conn); exit(4);
	      }
	    else
	      {
		while ((nbytes = fread(buf, 1, BUFSIZE,(FILE *)infile_fd ))>0) 
		  {
		    printf("W:%i:%s",nbytes,buf);
		    wbytes = srbObjWrite(conn, out_fd, buf, nbytes);
		    if (wbytes != nbytes) {   /* error */
		      fprintf(stderr, "Sappend: write error in file \"%s\": %i\n", 
			             argv[argc - 1],wbytes);
		      srb_perror (2, wbytes, "", 
			SRB_RCMD_ACTION|SRB_LONG_MSG);
		      fprintf(stderr, "Sappend: Exiting \n");
		      srbObjClose (conn, out_fd);
		      fclose(infile_fd);
		      clFinish(conn); exit(5);
		    }
		  }
		fclose((FILE *)infile_fd);
	      }
	  }
	else
	  {
	    clSplitbychar(argv[i],collname,dataname,'/');
	    in_fd = srbObjOpen (conn, dataname,  O_RDONLY, collname);
	    if (in_fd < 0)  
	      {   /* error */
		fprintf(stderr, 
		 "Sappend: can't open input SRB obj %s, status = %i\n", 
		  argv[i],in_fd);
		srb_perror (2, in_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
		srbObjClose (conn, out_fd);
		clFinish(conn); exit(6);
	      }
	    else
	      {
		while ((nbytes = srbObjRead (conn, in_fd, buf, BUFSIZE)) > 0) 
		  {
		    wbytes = srbObjWrite(conn, out_fd, buf, nbytes);
		    if (wbytes != nbytes) {   /* error */
		      fprintf(stderr, "Sappend: write error in file \"%s\": %i\n", 
			      argv[argc-1],wbytes);
		      srb_perror (2, wbytes, "", 
			SRB_RCMD_ACTION|SRB_LONG_MSG);
		      srbObjClose (conn, out_fd);
		      srbObjClose (conn, in_fd);
		      clFinish(conn); exit(7);
		    }
		  }
 		srbObjClose (conn, in_fd);
	      }
	  }
      }
    srbObjClose (conn, out_fd);
    return 0;
} 

int
append_files(int ival, int sval, char *infile)
{
  if (sval == 0)
    {
      if (ival == 1)
	infile_fd = stdin;
      else 
	{
#ifdef _WIN32
      infile_fd = SrbNt_fopen(infile, "r");
#else
	  infile_fd = fopen(infile, "r");
#endif
	  if (infile_fd == NULL)  
	    {   
	      fprintf(stderr, "Sappend: cannot open local file '%s'\n",
		      infile);
	      fprintf(stderr, "Sappend: exiting\n");
	      return -1;
	    }
	}
      while ((nbytes = fread(buf, 1, BUFSIZE,(FILE *)infile_fd ))>0) 
	{
	  wbytes = srbObjWrite(conn, out_fd, buf, nbytes);
	  if (wbytes != nbytes) {  
	    fprintf(stderr, "Sappend: write error in file  '%s': %i\n",
		      infile,wbytes);
	    fprintf(stderr, "Sappend: Exiting \n");
	    srbObjClose (conn, out_fd);
	    fclose(infile_fd);
	    return -1;
	  }
	}
      srbObjClose (conn, out_fd);
      fclose((FILE *)infile_fd);
    }
  else
    {
      /*** clSplitbychar(infile,collname,dataname,'/');***/
      splitpath(infile,collname,dataname);
      in_fd = srbObjOpen (conn, dataname,  O_RDONLY, collname);
      if (in_fd < 0)  
	{   
	  fprintf(stderr, "Sappend: can't open input SRB obj '%s':%i\n", 
		  infile,in_fd);
	  fprintf(stderr, "Sappend: Exiting \n");
	  srbObjClose (conn, out_fd);
	  return -1;
	}
      else
	{
	  while ((nbytes = srbObjRead (conn, in_fd, buf, BUFSIZE)) > 0) 
	    {
	      wbytes = srbObjWrite(conn, out_fd, buf, nbytes);
	      if (wbytes != nbytes) { 
		fprintf(stderr, "Sappend: write error in file '%s': %i\n", 
			infile,wbytes);
		fprintf(stderr, "Sappend: Exiting \n");
		srbObjClose (conn, out_fd);
		srbObjClose (conn, in_fd);
		return -1;
	      }
	    }
	  srbObjClose (conn, in_fd);
	  srbObjClose (conn, out_fd);
	}
    }
  return(0);
}

void
usage()
{
	fprintf(stderr, "Usage: Sappend localFileName  srbTarget\n");
	fprintf(stderr, "Usage: Sappend -i srbTarget\n");
	fprintf(stderr, "Usage: Sappend -s srbObj srbTarget\n");
}
int
main(int argc, char **argv)
{
  char *targetfile;
  char *infile;
  int ival,mval, sval;
  int i;
  srb_long_t seekStatus;

  ival = 0;
  sval = 0;
  mval = 1;

   if ( argc < 2)
      {
	usage();
	exit(1);
      }
  strcpy(inCondition,"");
   while (argv[mval][0] == '-')
      {if (argv[mval][1] == 's')
	{ sval = 1; mval++;}
       else if(argv[mval][1] == 'i') 
	{ ival = 1; mval++;}
      else
	 {fprintf(stderr, "Unknown Option\n");
	 usage();
	 exit(1);
	 }
      }
   if (mval > 2 )
      {
	usage();
	exit(1);
      }
   if (ival == 0)
     {if (argc < mval + 1) {usage(); exit(0);}
	 infile = argv[mval++];
       }
   if (argc < mval + 1) {usage(); exit(0);}
       targetfile = argv[mval];


    i = initSrbClientEnv();
    if (i < 0)
      {printf("Sappend: Initialization Error:%i\n",i); exit(-1);}

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sappend: Connection to srbMaster failed.\n");
        fprintf(stderr,"Sappend: %s\n",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }
    /*
    clSplitbychar(targetfile,collname,dataname, '/');
    */
    splitpath(targetfile,collname,dataname);

    out_fd = srbObjOpen (conn, dataname,  O_WRONLY, collname);
    if (out_fd < 0)  
      {  
	fprintf(stderr, "Sappend: cannot open output SRB obj Coll='%s'  Data='%s':%i\n", 
		collname,dataname,out_fd);
	fprintf(stderr, "Sappend: Exiting \n");
	clFinish(conn); exit(7);
      }
    seekStatus = srbObjSeek(conn, out_fd , 0,SEEK_END);
    if (seekStatus < 0)
      {
	fprintf(stderr, "Sappend: cannot seek in output SRB obj  '%s':%lld\n", 
		targetfile, seekStatus);
	fprintf(stderr, "Sappend: Exiting \n");
	srbObjClose(conn, out_fd);
	clFinish(conn); exit(8);
      }
    if (append_files(ival, sval, infile) < 0) {
      clFinish(conn); exit(9);
    } else {
      clFinish(conn);
      exit(0);
    }
  exit(0);
}
