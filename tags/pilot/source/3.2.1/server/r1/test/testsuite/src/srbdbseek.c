/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbdbseek.c--
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
	const char* progname = "srbdbseek" ;
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
	int mode ;
	int offset ;
	int bytes ;
	char *nullstr = "" ;
	char *dataPath ;
	int ilen, tlen, plen ;
	int status ;

	int c ;
	char *buf ;
	char *localfile ;
	struct stat statbuf ;
	long filebytes ;
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
	srbargc[SRB_USER_ARG_RSRC] = 1 ;
	srbargc[SRB_USER_ARG_DBMS] = 1 ;
	srbargc[SRB_USER_ARG_ID] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_MODE] = 1 ;
	srbargc[SRB_USER_ARG_SEEK] = 1 ;
	srbargc[SRB_USER_ARG_BYTES] = 1 ;
	srbargc[SRB_USER_ARG_TO] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_BSIZE] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbdbseek: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	storSysType = *((int*) srbargv[SRB_USER_ARG_DBMS][0]) ;
	resourceLoc = (char *) srbargv[SRB_USER_ARG_RSRC][0] ;
	table = nullstr ;
	tlen = 0 ;
	mode = *((int*) srbargv[SRB_USER_ARG_MODE][0]) ;
	offset = *((int*) srbargv[SRB_USER_ARG_SEEK][0]) ;
	bytes = *((int*) srbargv[SRB_USER_ARG_BYTES][0]) ;


	/* get the bufsize */

	bufsize = *((int*) srbargv[SRB_USER_ARG_BSIZE][0]) ;
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr,"srbdbseek: bufsize = %d\n", bufsize);
	buf = (char*) malloc(sizeof(char)*bufsize) ;
	if (buf == (char*) NULL)
	{
		fprintf(stderr,"srbdbseek: buf malloc failed.\n");
		exit(1);
	}


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbdbseek : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
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
		fprintf(stderr, "srbdbseek: connected.\n\n") ;


	/* process the files in "-id" */

	nfiles = srbargc[SRB_USER_ARG_ID] ;

	for (n=0 ; n<nfiles; n++)
	{
		localfile = srbargv[SRB_USER_ARG_TO][n] ;
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
			

		/* open the file */
		fp = fopen(localfile, "w") ;
		if (fp == (FILE *) NULL)
		{
			perror("srbdbseek") ;
			exit(1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbdbseek: open'ed \"%s\" for write\n", localfile);

		/* get external store */
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbdbseek.c: Attempting to call srbDbLobjOpen with\n\t storSysType = %d\n\t resourceLoc = %s\n\t dataPath = %s\n\t flags = O_RDONLY\n\t mode = %d\n", storSysType, resourceLoc, dataPath, mode );
		fd = srbDbLobjOpen(conn, storSysType, resourceLoc, dataPath, O_RDONLY, mode);
		if (fd < 0)
		{
			fprintf(stderr,"srbdbseek: Unable to open object (fd = %d)\n", fd);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbdbseek: open'ed object with fd = %d\n", fd);

		/* seek */
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbdbseek: Attempting to call srbDbLobjSeek with\n\t offset = %d\n\t whence = SEEK_SET\n", offset );
		status = srbDbLobjSeek(conn, fd, offset, SEEK_SET);
		if (status < 0)
		{
			fprintf(stderr,"srbdbseek: Unable to seek (status = %d)\n", status);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbdbseek: seek o.k.\n");


		/* get data */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbdbseek: reading data ...\n") ;
		rembytes = bytes ;
		while (rembytes > 0)
		{
			if (bufsize < rembytes)
				bufbytes = bufsize ;
			else
				bufbytes = rembytes ;

			srbrstat = srbDbLobjRead(conn, fd, buf, bufbytes) ;
			if (srbrstat < 0)
			{
				fprintf(stderr, "srbdbseek : ? srbDbLobjRead failed for \"%s\" (srbrstat = %d)\n", objID, srbrstat) ;
				fprintf(stderr,"%s",clErrorMessage(conn));
				fclose(fp) ;
				srbDbLobjClose (conn, fd);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srbdbseek: read %d bytes\n", srbrstat) ;
			bufbytes = srbrstat ;

			wstat = fwrite((void *) buf, bufbytes, 1, fp) ;
			status = ferror(fp) ;
			if ((wstat != 1) || (status != 0))
			{
				fprintf(stderr, "srbdbseek : ? fwrite failed for \"%s\" (wstat = %d, ferror = %d)\n", localfile, wstat, status) ;
				fclose(fp) ;
				srbDbLobjClose (conn, fd);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srbdbseek: wrote %d bytes\n", wstat*bufbytes) ;

			rembytes -= (long) bufbytes ;
		}


		/* close output file */

		fclose(fp) ;

		/* close external store */

		srbDbLobjClose (conn, fd);

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbdbseek: I/O completed successfully.\n\n") ;
	}



	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbdbseek: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbdbseek: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
