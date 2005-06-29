/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * httpget.c--
 *    test file extraction
 *
 * See README file or execute with -help flag.
 *
 *-------------------------------------------------------------------------
 */
#include "srbUserArgs.h"
#include "srbClient.h"

#define BUFSIZE	4096

int
main(int argc, char **argv)
{
	srbConn *conn;
	const char* progname = "httpget" ;
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
	int syst ;
	char *systname[SRB_SYST_COUNT] = SRB_SYST_NAMES ;
	char *rsrc ;
	char *systv ;
	int status ;

	int c ;
	char *buf ;
	char *localfile ;
	char *externstore ;
	struct srbStat statbuf ;
	long filebytes ;
	int filemode ;
	FILE *fp ;
	int fd;
	long rembytes ;
	int bufbytes ;
	int srbrstat ;
	size_t wstat ;


	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_FILE] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_SYST] = 1 ;
	srbargc[SRB_USER_ARG_RSRC] = 1 ;
	srbargc[SRB_USER_ARG_BSIZE] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "httpget: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	syst = *((int *) srbargv[SRB_USER_ARG_SYST][0]) ;
	rsrc = (char *) srbargv[SRB_USER_ARG_RSRC][0] ;
	systv = systname[syst] ;


	/* get the bufsize */

	bufsize = *((int*) srbargv[SRB_USER_ARG_BSIZE][0]) ;
	buf = (char*) malloc(sizeof(char)*bufsize) ;
	if (buf == (char*) NULL)
	{
		fprintf(stderr,"httpget: buf malloc failed.\n");
		exit(1);
	}


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "httpget : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
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
		fprintf(stderr, "httpget: connected.\n\n") ;

	/* process the stores in "-file" */

	nfiles = srbargc[SRB_USER_ARG_FILE] ;

	for (n=0 ; n<nfiles; n++)
	{
		localfile = srbargv[SRB_USER_ARG_TO][n] ;
		externstore = srbargv[SRB_USER_ARG_FILE][n] ;

		/* open external store */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"httpget: Attempting srbFileOpen with\n\t storSysType = %s (%d)\n\t hostAddr = %s\n\t filename = %s\n\t mode = %d\n", systv, syst, rsrc, externstore, filemode) ;

		fd = srbFileOpen(conn, syst, rsrc, externstore, O_RDONLY, filemode);
		if (fd < 0)
		{
			fprintf(stderr,"httpget: srbFileOpen failed (fd = %d)\n", fd) ;
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}

		/* sync external store */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbfcreate: sync'ing external store ...\n") ;
		srbFileSync (conn, fd);
		if (clStatus(conn) != CLI_CONNECTION_OK)
		{
			fprintf(stderr,"httpget: srbFileSync failed.\n");
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit(1);
		}

		/* read data */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "httpget: reading data ...\n") ;
		filebytes = 0;
		while ((srbrstat = srbFileRead (conn, fd, buf, BUFSIZE)) > 0) 
		    filebytes += srbrstat;
		    
		if (srbrstat < 0) {
		    fprintf(stderr, 
		      "httpget : srbFileRead failed for \"%s\" status = %d\n", 
			externstore, srbrstat) ;
		    fprintf(stderr,"%s",clErrorMessage(conn));
		    fclose(fp) ;
		    srbFileClose (conn, fd);
		    clFinish(conn);
		    exit(1) ;
		}
		fprintf(stderr, "httpget: read %d bytes\n", filebytes) ;

		srbFileClose (conn, fd);
		fprintf(stderr, "httpget: I/O completed successfully.\n\n") ;
	}


	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "httpget: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "httpget: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;


	exit(0);
}
