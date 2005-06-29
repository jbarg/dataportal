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
 
int
readFileForMultiLineInsert(char *metaFile, char **inbuf);

void usage()
{
  fprintf(stderr,"Usage: SmodE -i dataName collName tableName insertAttrList insertAttrValueList [insertAttrValueList ...]\n");
  fprintf(stderr,"          insert row(s) into a table; string values need to be quoted\n");
  fprintf(stderr,"Usage: SmodE -d dataName collName tableName conditionAttrList conditionList [conditionList ...]\n");
  fprintf(stderr,"         delete row(s) from a table; string values need to be quoted\n");
  fprintf(stderr,"Usage: SmodE -u dataName collName tableName modifyAttrList modifyAttrValueList conditionAttrList conditionList\n");
  fprintf(stderr,"          modify a row in a table; string values need to be quoted\n");
  fprintf(stderr,"Usage: SmodE -i -f insertFileName dataName collName tableName insertAttrValueList\n");
  fprintf(stderr,"          insert row(s) into a table; string values need to be quoted\n");
  fprintf(stderr,"    Format of file is as follows:\n");
  fprintf(stderr,"     first-line  =attrVal|attrVal|...|attrVal\n");
  fprintf(stderr,"     second-line =attrVal|attrVal|...|attrVal\n");
  fprintf(stderr,"     :\n");
  fprintf(stderr,"     nth-line =attrVal|attrVal|...|attrVal\n");
  fprintf(stderr,"   string values need to be quoted\n");

}

int
main(int argc, char **argv)
{
  srbConn *conn;
  int i, c;
  char tmpStr[HUGE_STRING];
  int IFlag = 0;
  int UFlag = 0;
  int DFlag = 0;
  int FFlag = 0;
  char metaFile[MAX_TOKEN];
  char *inbuf;
  char *valPtr;

    if (argc < 3) {
      usage ();exit (1);
    }

    while ((c=getopt(argc, argv,"f:idu")) != EOF) {
      switch (c) {
      case 'f':
	strcpy(metaFile, optarg);
        FFlag = 1;
        break;
      case 'i':
	if (argc < 7) {
	  usage ();exit (1);
	}
	IFlag = 1;
        break;
      case 'd':
	if (argc < 7) {
	  usage ();exit (1);
	}
	DFlag = 1;
        break;
      case 'u':
	if (argc != 9) {
	  usage ();exit (1);
	}
	UFlag = 1;
        break;
      default:
        usage ();
        exit (1);
      }
    }


    i = initSrbClientEnv();
    if (i < 0)
      {printf("SmodE: Initializatin Error:%i\n",i); exit(1);}
    conn = srbConnect (srbHost, NULL, srbAuth, 
     NULL, NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
      fprintf(stderr,"Connection to srbMaster failed.\n");
      fprintf(stderr,"%s",clErrorMessage(conn));
      srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      clFinish(conn); exit(3);
    }
    

    i = 0;
    if (FFlag == 0) {
      if (IFlag == 1) {
	if (argc == 7)
	  valPtr = argv[6];
	else {
	  strcat(tmpStr,argv[6]);
	  for (i = 7; i < argc ; i++) {
	    strcat(tmpStr,"\n");
	    strcat(tmpStr,argv[i]);
	  }
	  valPtr = tmpStr;
	}
	i = srbModifyExtMetaData (conn, 0,argv[2],argv[3],argv[4],argv[5],valPtr,
				  "","",D_INSERT_INTO_EXTMD_TABLE);
      }
      else if (DFlag == 1) {
	if (argc == 7)
	  valPtr = argv[6];
	else {
	  strcat(tmpStr,argv[6]);
	  for (i = 7; i < argc ; i++) {
	    strcat(tmpStr,"\n");
	    strcat(tmpStr,argv[i]);
	  }
	  valPtr = tmpStr;
	}
	i = srbModifyExtMetaData (conn, 0,argv[2],argv[3],argv[4],argv[5],valPtr,
				  "","",D_DELETE_FROM_EXTMD_TABLE);
      }
      else if (UFlag == 1) {
	i = srbModifyExtMetaData (conn, 0,argv[2],argv[3],argv[4],argv[5],argv[6],
				  argv[7],argv[8],D_MODIFY_IN_EXTMD_TABLE);
      }
    }
    else { /*File based mods */
      if (IFlag == 1) {
	if (argc != 8) {
          clFinish(conn);
          exit (1);
        }
	i = readFileForMultiLineInsert(metaFile, &inbuf);
	if (i < 0) {
	  clFinish(conn);
	  exit (1);
	}
	i = srbModifyExtMetaData (conn, 0, argv[4],argv[5],argv[6],argv[7],inbuf,
                                  "","",D_INSERT_INTO_EXTMD_TABLE);
	free(inbuf);
      }

    }
    if (i < 0) {
      printf("SmodE: Operational Error:%i\n",i); 
      srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      clFinish(conn);
      exit (1);
    }
    else {
      clFinish(conn);
      exit(0);
    }
}


int
readFileForMultiLineInsert(char *metaFile, char **inbuf) 
{

 int i;
 FILE *fd;
 char *buf;
 struct stat statbuf;

#ifdef _WIN32
 if (stat_file(metaFile, &statbuf) != 0)
#else
   if (stat(metaFile, &statbuf) != 0)
#endif
     {
       fprintf(stderr, "unable to stat metadata file %s\n",
              metaFile);
       return (-1);
     }
    fd = fopen(metaFile,"r");
    if (fd == NULL) {
      fprintf(stderr, "can't open metadata file %s, errno = %d\n",
              metaFile,errno);
      return(-1);
    }
    buf = malloc( statbuf.st_size + 20);
    i = fread(buf,1,statbuf.st_size+19, fd);
    fclose(fd);
    if (i <= 0) {
      free(buf);
      fprintf(stderr, "error reading metadata file %s = %i\n", metaFile,i);
      return(-1);
    }

    buf[i] = '\0';
    buf[i+1] = '\0';
    *inbuf = buf;
    return(0);
    
}
