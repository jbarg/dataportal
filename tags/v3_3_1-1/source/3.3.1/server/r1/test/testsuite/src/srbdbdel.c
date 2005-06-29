/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbdbdel.c.c--
 *
 * See README file or execute with -help flag.
 *
 *-------------------------------------------------------------------------
 */

#include "srbUserArgs.h"
#include "srbClient.h"


int
main(int argc, char **argv)
{
	srbConn *conn;
	const char* progname = "srbdbdel.c" ;
	int protocol ;
	int srbargc[SRB_USER_ARGS] ;
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC] ;
	int k ;
	int ndirs ;
	int nfiles ;
	int bufsize ;
	int n ;
	char *host ;
	char *port ;
	char *auth ;
	int storSysType ;
	char *resourceLoc ;
	char *table ;
	char *objID ;
	char *nullstr = "" ;
	char *dataPath ;
	int ilen, tlen, plen ;
	int status ;

	int c ;
	char *buf ;
	char *externstore ;
	struct stat statbuf ;
	long filebytes ;
	FILE *fp ;
	int fd;
	long rembytes ;
	int bufbytes ;
	size_t rstat ;
	int srbwstat ;


	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_DBMS] = 1 ;
	srbargc[SRB_USER_ARG_RSRC] = 1 ;
	srbargc[SRB_USER_ARG_ID] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbdbdel.c: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	storSysType = *((int*) srbargv[SRB_USER_ARG_DBMS][0]) ;
	resourceLoc = (char *) srbargv[SRB_USER_ARG_RSRC][0] ;

	table = nullstr ;
	tlen = 0 ;


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbdbdel.c : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
	conn = clConnect (host, port, auth);

	/* check to see that the backend connection was successfully made */
	if (clStatus(conn) != CLI_CONNECTION_OK)
	{
		fprintf(stderr,"Connection to srbMaster failed.\n");
		fprintf(stderr,"%s",clErrorMessage(conn));
		clFinish(conn);
		exit(1);
	}
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbdbdel.c: connected.\n\n") ;


	/* process the files in "-ID" */

	nfiles = srbargc[SRB_USER_ARG_ID] ;

	for (n=0 ; n<nfiles; n++)
	{
		objID = srbargv[SRB_USER_ARG_ID][n] ;
		if (tlen == 0)
			dataPath = objID ;
		else
		{
			ilen = strlen(objID) ;
			dataPath = (char*) malloc(sizeof(char)*(ilen+tlen+2)) ;
			if (dataPath == (char*) NULL)
			{
				fprintf(stderr,"srbdbdel.c: dataPath malloc failed.\n");
				clFinish(conn);
				exit(1);
			}
			strcpy(dataPath,table) ;
			strcat(dataPath,"/") ;
			strcat(dataPath,objID) ;
		}
			

		/* delete external store */
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbdbdel.c: Attempting to call srbDbLobjUnlink with\n\t storSysType = %d\n\t resourceLoc = %s\n\t dataPath = %s\n", storSysType, resourceLoc, dataPath);

		if (srbargc[SRB_USER_ARG_VERB] != 0)

		status = srbDbLobjUnlink(conn, storSysType, resourceLoc, dataPath);
		if (status < 0)
		{
			fprintf(stderr,"srbdbdel.c: Unable to create object (status = %d)\n", status);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbdbdel.c: object unlink'd.\n");

		if (tlen != 0)
			free(dataPath) ;
	}



	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbdbdel.c: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbdbdel.c: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
