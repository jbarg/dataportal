/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbfcreate.c--
 *    test file creation
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
	const char* progname = "srbfcreate" ;
	int protocol ;
	int srbargc[SRB_USER_ARGS] ;
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC] ;
	int k ;
	int ndirs ;
	int nfiles ;
	int bufsize ;
	int n ;
	int mode ;
	char *host ;
	char *port ;
	char *auth ;
	char *systname[SRB_SYST_COUNT] = SRB_SYST_NAMES ;
	char *rsrc ;
	char *systv ;
	int syst ;
	int status ;

	int c ;
	char *buf ;
	char *localfile ;
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
	srbargc[SRB_USER_ARG_FROM] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_FILE] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_DIR] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_SYST] = 1 ;
	srbargc[SRB_USER_ARG_RSRC] = 1 ;
	srbargc[SRB_USER_ARG_MODE] = 1 ;
	srbargc[SRB_USER_ARG_BSIZE] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbfcreate: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;


	/* get the bufsize */

	bufsize = *((int*) srbargv[SRB_USER_ARG_BSIZE][0]) ;
	buf = (char*) malloc(sizeof(char)*bufsize) ;
	if (buf == (char*) NULL)
	{
		fprintf(stderr,"srbfcreate: buf malloc failed.\n");
		exit(1);
	}


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfcreate : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
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
		fprintf(stderr, "srbfcreate: connected.\n\n") ;

	rsrc = srbargv[SRB_USER_ARG_RSRC][0] ;
	syst = *((int*) srbargv[SRB_USER_ARG_SYST][0]) ;
	systv = systname[syst] ;
	mode = *((int*) srbargv[SRB_USER_ARG_MODE][0]) ;

	/* process the files in "-from" */

	nfiles = srbargc[SRB_USER_ARG_FROM] ;

	for (n=0 ; n<nfiles; n++)
	{
		localfile = srbargv[SRB_USER_ARG_FROM][n] ;
		externstore = srbargv[SRB_USER_ARG_FILE][n] ;

		/* check for local file */
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbfcreate: checking for \"%s\" ...\n", localfile) ;
		if ( stat( localfile, &statbuf ) != 0 )
		{
			perror("srbfcreate") ;
			exit(1);
		}
		if ( (statbuf.st_mode & S_IFMT) != S_IFREG )
		{
			fprintf(stderr, "srbfcreate :  ? \"%s\" is not a regular file\n", localfile) ;
			exit(1);
		}

		/* open the file */
		filebytes = statbuf.st_size * sizeof(char) ;
		fp = fopen(localfile, "r") ;
		if (fp == (FILE *) NULL)
		{
			perror("srbfcreate") ;
			exit(1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbfcreate: \"%s\" is %d bytes\n\n", localfile, filebytes);

		/* create external store */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbfcreate: Attempting srbFileCreate with\n\t storSysType = %s (%d)\n\t hostAddr = %s\n\t filename = %s\n\t mode = %d\n", systv, syst, rsrc, externstore, mode) ;

		fd = srbFileCreate(conn, syst, rsrc, externstore, mode, -1) ;
		if (fd < 0)
		{
			fprintf(stderr,"srbfcreate: srbFileCreate failed (fd = %d)\n", fd) ;
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit(1);
	        }

		/* write data */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbfcreate: writing data ...\n") ;
		rembytes = filebytes ;
		while (rembytes > 0)
		{
			if (bufsize < rembytes)
				bufbytes = bufsize ;
			else
				bufbytes = (int) rembytes ;

			rstat = fread((void *) buf, bufbytes, 1, fp) ;
			status = ferror(fp) ;
			if ((rstat != 1) || (status != 0))
			{
				fprintf(stderr, "srbfcreate : ? fread failed for \"%s\" (rstat = %d, ferror(fp) = %d)\n", localfile, rstat, status) ;
				fclose(fp) ;
				srbFileClose (conn, fd);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srbfcreate: read %d bytes\n", rstat*bufbytes) ;

			srbwstat = srbFileWrite(conn, fd, buf, bufbytes) ;
			if (srbwstat < 0)
			{
				fprintf(stderr, "srbfcreate : ? srbFileWrite failed for \"%s\" (srbwstat = %d)\n", localfile, srbwstat) ;
				fprintf(stderr,"%s",clErrorMessage(conn));
				fclose(fp) ;
				srbFileClose (conn, fd);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srbfcreate: wrote %d bytes\n", srbwstat) ;

			rembytes -= (long) bufbytes ;
		}

		/* close input file */

		fclose(fp) ;

		/* close and sync external store */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbfcreate: sync'ing external store ...\n") ;
		srbFileSync (conn, fd);
		if (clStatus(conn) != CLI_CONNECTION_OK)
		{
			fprintf(stderr,"srbfcreate: srbFileSync failed.\n");
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit(1);
		}
		srbFileClose (conn, fd);

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbfcreate: I/O completed successfully.\n\n") ;
	}



	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfcreate: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfcreate: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
