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
extern char srbUser[];
extern char srbPort[];
extern char mdasDomainName[];
extern char inCondition[];

    srbConn *conn;
    int  nbytes, wbytes, in_fd;
    char buf[BUFSIZE];
    char collname[MAX_TOKEN];
    char dataname[MAX_TOKEN];
    char targetdataname[MAX_TOKEN];
    char filename[MAX_TOKEN];

    FILE * infile_fd;
    int  out_fd;
    char datatypename[MAX_TOKEN];
    char resourcename[MAX_TOKEN];
    char pathname[MAX_TOKEN];
    srb_long_t  dataSize = 0;
    int stdinval;
    int copyval = 0;

int
register_files(char **localfile, 
	  char *target, 
	  char *datatype, 
	  char *resource, 
	  int   pval,
	  int   lcount)
{
  char tmpstr1[MAX_TOKEN];
  char tmpstr2[MAX_TOKEN];
  char tmpstr3[MAX_TOKEN];
  char tmpstr4[MAX_TOKEN];
  char inbuf[100];
  int ii;

  strcpy(dataname,"");

     sprintf(tmpstr4,"%s/alpha",target);
     splitpath(tmpstr4,tmpstr3,tmpstr2);
     if (checkCollection(conn,  MDAS_CATALOG, tmpstr3) == 0)
       {strcpy(collname,tmpstr3); strcpy(dataname,"");}
     else
       splitpath(target,collname,dataname);

	if (strlen(resource) == 0)
	  get_user_preference(resourcename, "DRSRC");
	else
	  strcpy(resourcename,resource);
    
    for (i = 0; i < lcount ; i++)
      {
	/*  RAJA  NOv 19, 2004 removed. being checked in MCAT
        if (localfile[i][0] != '/') 
          {
            fprintf(stderr,
                    "Sregister: cannot register local obj '%s' (only absolute local path ('/a/b/c/foo') can be registered)\n",
                     localfile[i]);
             return(1);
          }
	***/
	if (lcount == 1)
	  {
	    if (strlen(dataname) == 0)
	      {
		splitStringfromEnd(localfile[i], tmpstr1, tmpstr2, '/');
		sprintf(targetdataname,tmpstr2);
	      }
	    else
	      sprintf(targetdataname,dataname);
	  }
	else
	  {
	    splitStringfromEnd(localfile[i], tmpstr1, tmpstr2, '/');
	    sprintf(targetdataname, "%s%s", dataname,tmpstr2);
	  }
	if (strlen(datatype) == 0) 
	  trylocaldatatype (datatypename, localfile[i]);
	else
	  strcpy(datatypename,datatype);


	if (pval == 1)
	  {
	    fprintf(stdout,"Register  %s as %s/%s on %s (y/n)?",
		    localfile[i], collname, targetdataname, resourcename);
	    fgets(inbuf, 100, stdin);
	    if (inbuf[0] != 'y' )
	      continue;
	  }

	/* printf("Register  %s to %s on %s size %i collection %s type %s\n",
	       localfile[i], targetdataname, resourcename,
	       dataSize, collname, datatypename); 
	       continue;
	*/
	  if (!copyval) {
	    int sizeSet;

	    sizeSet = 0;
	    if (dataSize <= 0) {
	        struct stat statbuf;
	        ii = stat (localfile[i], &statbuf);
		if (ii >= 0) {
		    dataSize = statbuf.st_size;
		    sizeSet = 1;
		}
	    }
	    ii = srbRegisterDataset(conn, MDAS_CATALOG, targetdataname,
				    datatypename, resourcename,
				    collname, localfile[i], dataSize);
            /* have to reset dataSize because it is global :=( */

            if (sizeSet == 1) {
                dataSize = 0;
            }

	  }
	  else {
	    ii = srbRegisterReplica (conn, MDAS_CATALOG, targetdataname,
				     collname, "","",resourcename, 
				     localfile[i],srbUser,mdasDomainName);
	  }
	  if (ii < 0)
	      {
		fprintf(stderr, 
			"Sregister: cannot register SRB obj '%s':%i\n", 
                targetdataname, ii);
		srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	      }
      }
      return (0);
}

void usage()
{
fprintf(stderr, "Usage Error\n");
	fprintf(stderr, "Usage: Sregister [-p] [-D dataType] [-S Size] [-R resourceName]  RegisteringObjectPath ... TargetName \n");
	fprintf(stderr, "Usage: Sregister -c [-p] [-R resourceName]  RegisteringObjectPath  srbObjectName \n");
}

int
main(int argc, char **argv)
{
    char *localfile[MAX_TOKEN];
    char *target;
    char *datatype;
    char *resource;
    int pval, mval, lcount;
    char nulltmpStr[2];
#ifdef _WIN32
    char buff32[2048];
#endif

    strcpy(inCondition , "");

    pval = 0;
    copyval = 0;
    strcpy (nulltmpStr, "");
    target  = nulltmpStr;
    datatype= nulltmpStr;
    resource= nulltmpStr;
    lcount = 0;
    mval = 1;
    stdinval = 0;
    strcpy(pathname,"");

    if (argc < 3) {usage(); exit(1);}
    while (argv[mval][0] == '-')
      {if (argv[mval][1] == 'p')
	{ pval = 1; mval++;}
       else if (argv[mval][1] == 'c')
	{ copyval = 1; mval++;}
       else if(argv[mval][1] == 'D') 
	{ if (argc < mval+2) {usage(); exit(1);}
	  datatype = argv[++mval]; mval++;}
      else if(argv[mval][1] == 'S') 
	{ if (argc < mval+2) {usage(); exit(1);}
	  dataSize = strtoll (argv[++mval], 0, 0); mval++;}
      else if(argv[mval][1] == 'R') 
	{ if (argc < mval+2) {usage(); exit(1);}
	  resource = argv[++mval]; mval++;}
      else
	 {fprintf(stderr, "Unknown Option\n");
	 usage();
	 exit(1);
	 }
      }

    if (argc < mval+2-stdinval) 
      {usage();
      exit(1);
      }

    for (; mval < argc-1; mval++)
      {
#ifdef _WIN32
    sprintf(buff32,"/%s",argv[mval]);
    SrbNtPathForwardSlash(buff32);
    localfile[lcount] = strdup(buff32);
    ++lcount;
#else
	localfile[lcount++] = argv[mval];
#endif
      }
    if (stdinval ==  1) { lcount = 1; localfile[lcount] = argv[argc-1];}

    if (lcount == 0)
      {
	usage();
	exit(1);
      }

    target = argv[argc-1];


    i = initSrbClientEnv();
    if (i < 0)
      {printf("Sregister: Initialization Error:%i\n",i); exit(1);}

    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Sregister: Connection to srbMaster failed.\n");
        fprintf(stderr,"Sregister: %s\n",clErrorMessage(conn));
	srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        clFinish(conn); exit(3);
    }

    i = register_files(localfile, target, datatype, resource, pval, lcount);
    clFinish(conn);
    exit(i);
}
