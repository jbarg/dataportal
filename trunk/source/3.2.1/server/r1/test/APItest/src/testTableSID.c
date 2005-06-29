/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testTable - test the physical DB table calls.
 *
 *-------------------------------------------------------------------------
 */
#include "srbClient.h"

#define BUFSIZE 1016
#define srbAuth         NULL
#define INFILE		"../../testfiles/test_in"
#define OUTFILE		"../../testfiles/test_out"
#define HOST_ADDR	NULL
#define SELECT_STMT     "select * from sekar.mdas_cd_user"
char *resourceLoc[] = {
	"hpss17.sdsc.edu:mcat:db2v5",
	"sage:srb:foo",
	"",
	"",
	"",
	"ghidorah.sdsc.edu:dev7:foo"
};
/*
	"ghidorah.sdsc.edu:dev7:foo"
	mda-18:vaults:db2v2",
	"mda-18:lgndb2sd:db2v2",
*/
void 
exit_nicely(srbConn* conn)
{
  clFinish(conn);
  exit(1);
}

int
main(int argc, char **argv)
{
    srbConn *conn;
    int  nbytes,  in_fd, out_fd, funcVal;
    char buf[BUFSIZE];
    int storSysType;
    int tmp,ii;
    char myc;
    if (argc != 2) 
      {fprintf(stderr, "Usage: %s DbType\n",argv[0]);
        exit(1);
    }
    storSysType = atoi (argv[1]);

    conn = clConnect (NULL, NULL, srbAuth);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to SRB failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely(conn);
    }
	

    while ( 1 == 1) {
      printf("Choose [o/r/w/c/CREAtE/uNLINK/eXIT]:");
      gets(buf);
      myc = buf[strlen(buf) -1];
      switch (myc) {
      case 'o' :
	printf("Submit Select Statement(<CR> for write):");
	gets(buf);
	out_fd = srbDbTableOpen (conn, storSysType, resourceLoc[storSysType],
				 buf, 0, 0666);
	
	printf("out_fd=%d\n", out_fd);
	
	if (out_fd < 0)  {  
	  fprintf(stderr, "srbDbTableOpen error for %s, %s\n", 
		  resourceLoc[storSysType], buf);
	  fprintf(stderr,"%s",clErrorMessage(conn));
	  exit_nicely(conn);
	} 
	break;
      case 'r' :
	while ((nbytes = srbDbTableRead(conn, out_fd, buf, BUFSIZE)) > 0) {
	  if (nbytes < 0){
	    fprintf(stderr, "srbDbTableRead error: %s", 
		    clErrorMessage(conn));
	    srbDbTableClose (conn, out_fd);
	    exit_nicely(conn);
	  }
	  for(ii = 0; ii < nbytes ; ii++)
	    if (buf[ii] == '\0') 
	      printf("\n");
	    else printf("%c",buf[ii]);
	  fflush(stdout);
	}
	break;
      case 'w' :
	printf("Submit Insert/Update Statement:");
	gets(buf);
	nbytes = srbDbTableWrite(conn, out_fd, buf, BUFSIZE);
	if (nbytes < 0) {  
	  fprintf(stderr, "srbDbTableWrite error: %s", 
		  clErrorMessage(conn));
	  srbDbTableClose (conn, out_fd);
	  exit_nicely(conn);
	}
	break;
      case 'c' :
	srbDbTableClose (conn, out_fd);
	break;
      case 't' :
	printf("Submit Create Statement:");
	gets(buf);
	out_fd = srbDbTableCreate(conn, storSysType, resourceLoc[storSysType],
				  buf,  0666);
	printf("out_fd=%d\n", out_fd);
	if (out_fd < 0)  {  
	  fprintf(stderr, "srbDbTableCreate error for %s, %s\n", 
		  resourceLoc[storSysType], buf);
	  fprintf(stderr,"%s",clErrorMessage(conn));
	  exit_nicely(conn);
	} 
	break;
      case 'u':
	printf("Submit Table Name:");
	gets(buf);
	out_fd = srbDbTableUnlink(conn, storSysType, resourceLoc[storSysType],
				  buf);
				  
	break;
      case 'e' :
	exit_nicely(conn);
	break;
      default :
	break;
	
      }
    }
}
