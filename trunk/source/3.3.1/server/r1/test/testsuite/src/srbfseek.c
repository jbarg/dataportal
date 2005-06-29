/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbfseek.c--
 *    test file extraction
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
	const char* progname = "srbfseek" ;
	int srbargc[SRB_USER_ARGS] ;
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC] ;
	int k ;
	int ndirs ;
	int nfiles ;
	int seek ;
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
	srbargc[SRB_USER_ARG_SEEK] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_BYTES] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_TO] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbfseek: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	syst = *((int *) srbargv[SRB_USER_ARG_SYST][0]) ;
	systv = systname[syst] ;
	rsrc = (char *) srbargv[SRB_USER_ARG_RSRC][0] ;


	/* get the seek and bufsize */

	seek = *((int*) srbargv[SRB_USER_ARG_SEEK][0]) ;
	bufsize = *((int*) srbargv[SRB_USER_ARG_BYTES][0]) ;
	buf = (char*) malloc(sizeof(char)*bufsize) ;
	if (buf == (char*) NULL)
	{
		fprintf(stderr,"srbfseek: buf malloc failed.\n");
		exit(1);
	}


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfseek : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
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
		fprintf(stderr, "srbfseek: connected.\n\n") ;


	/* process the stores in "-file" */

	nfiles = srbargc[SRB_USER_ARG_FILE] ;

	for (n=0 ; n<nfiles; n++)
	{
		localfile = srbargv[SRB_USER_ARG_TO][n] ;
		externstore = srbargv[SRB_USER_ARG_FILE][n] ;

		/* open external store */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbfseek: Attempting srbFileStat with\n\t storSysType = %s (%d)\n\t hostAddr = %s\n\t filename = %s\n", systv, syst, rsrc, externstore) ;

        	status = srbFileStat(conn, syst, rsrc, externstore, &statbuf) ;
        	if (status != 0)
		{
			fprintf(stderr,"srbfseek: srbFileStat failed (status = %d)\n", status) ;
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit(1);
	        }

		filebytes = statbuf.st_size ;
		filemode = statbuf.st_mode ;
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbfseek: \"%s\" is mode %x and %d bytes\n", externstore, filemode, filebytes) ;

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbfseek: Attempting srbFileOpen with\n\t storSysType = %s (%d)\n\t hostAddr = %s\n\t filename = %s\n\t mode = %d\n", systv, syst, rsrc, externstore, filemode) ;

		fd = srbFileOpen(conn, syst, rsrc, externstore, O_RDONLY, filemode);
		if (fd < 0)
		{
			fprintf(stderr,"srbfseek: srbFileOpen failed (fd = %d)\n", fd) ;
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
			fprintf(stderr,"srbfseek: srbFileSync failed.\n");
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit(1);
		}

		/* open local file */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbfseek: opening \"%s\" for write ...\n", localfile) ;
		fp = fopen(localfile, "w") ;
		if (fp == (FILE *) NULL)
		{
			perror("srbfseek") ;
			clFinish(conn);
			exit(1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbfseek: \"%s\" open\n\n", localfile);

		/* seek */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbfseek: seeking to %d ...\n", seek) ;

		srbrstat = srbFileSeek(conn, fd, seek, SEEK_SET) ;
		if (srbrstat < 0)
		{
			fprintf(stderr, "srbfseek : ? srbFileSeek failed for \"%s\" (srbrstat = %d)\n", externstore, srbrstat) ;
			fprintf(stderr,"%s",clErrorMessage(conn));
			fclose(fp) ;
			srbFileClose (conn, fd);
			clFinish(conn);
			exit(1) ;
		}
		fprintf(stderr, "srbfseek : seek succeeded for \"%s\" (srbrstat = %d)\n", externstore, srbrstat) ;


		/* write data */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbfseek: writing data ...\n") ;
		rembytes = bufsize ;
		while (rembytes > 0)
		{
			if (bufsize < rembytes)
				bufbytes = bufsize ;
			else
				bufbytes = (int) rembytes ;

			srbrstat = srbFileRead(conn, fd, buf, bufbytes) ;
			if (srbrstat < 0)
			{
				fprintf(stderr, "srbfseek : ? srbFileRead failed for \"%s\" (srbrstat = %d)\n", externstore, srbrstat) ;
				fprintf(stderr,"%s",clErrorMessage(conn));
				fclose(fp) ;
				srbFileClose (conn, fd);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srbfseek: read %d bytes\n", srbrstat) ;

			wstat = fwrite((void *) buf, bufbytes, 1, fp) ;
			status = ferror(fp) ;
			if ((wstat != 1) || (status != 0))
			{
				fprintf(stderr, "srbfseek : ? fwrite failed for \"%s\" (wstat = %d, ferror = %d)\n", externstore, wstat, status) ;
				fclose(fp) ;
				srbFileClose (conn, fd);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srbfseek: wrote %d bytes\n", wstat*bufbytes) ;

			rembytes -= (long) bufbytes ;
		}

		/* close files */

		fclose(fp) ;
		srbFileClose (conn, fd);
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbfseek: I/O completed successfully.\n\n") ;
	}


	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfseek: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfseek: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;


	exit(0);
}
