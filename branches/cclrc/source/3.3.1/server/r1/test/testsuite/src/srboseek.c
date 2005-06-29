/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srboseek.c--
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
	const char* progname = "srboseek" ;
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
	char *objID ;
	char *rsrc ;
	char *coll ;
	mdasC_sql_result_struct myresult ;
	int result_count ;
	char qval[MAX_DCS_NUM][MAX_TOKEN] ;
	int selval[MAX_DCS_NUM] ;
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
	int seek ;
	int bytes ;


	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_RSRC] = 1 ;
	srbargc[SRB_USER_ARG_ID] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_COLL] = 1 ;
	srbargc[SRB_USER_ARG_SEEK] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_BYTES] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_TO] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_BSIZE] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srboseek: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	rsrc = (char *) srbargv[SRB_USER_ARG_RSRC][0] ;
	coll = (char *) srbargv[SRB_USER_ARG_COLL][0] ;
	seek = *((int*) srbargv[SRB_USER_ARG_SEEK][0]) ;
	bytes = *((int*) srbargv[SRB_USER_ARG_BYTES][0]) ;


	/* get the bufsize */

	bufsize = *((int*) srbargv[SRB_USER_ARG_BSIZE][0]) ;
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr,"srboseek: bufsize = %d\n", bufsize);
	buf = (char*) malloc(sizeof(char)*bufsize) ;
	if (buf == (char*) NULL)
	{
		fprintf(stderr,"srboseek: buf malloc failed.\n");
		exit(1);
	}


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboseek : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
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
		fprintf(stderr, "srboseek: connected.\n\n") ;


	/* process the files in "-id" */

	nfiles = srbargc[SRB_USER_ARG_ID] ;

	for (n=0 ; n<nfiles; n++)
	{
		objID = srbargv[SRB_USER_ARG_ID][n] ;
		localfile = srbargv[SRB_USER_ARG_TO][n] ;

		/* open the file */
		fp = fopen(localfile, "w") ;
		if (fp == (FILE *) NULL)
		{
			perror("srboseek") ;
			exit(1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srboseek: open'ed \"%s\" for write\n", localfile);

		/* get external size */
		for (k=0 ; k<MAX_DCS_NUM ; k++)
		{
			qval[k][0] = '\0' ;
			selval[k] = 0 ;
		}
		sprintf(qval[DATA_NAME]," = '%s'", objID);
		sprintf(qval[DATA_GRP_NAME]," = '%s'", coll);
		sprintf(qval[RSRC_NAME]," = '%s'", rsrc);
		selval[SIZE] = 1 ;
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboseek: Attempting to call srbGetDataDirInfo with\n\t qval[DATA_NAME] = %s\n\t qval[RSRC_NAME] = %s\n\t qval[DATA_GRP_NAME] = %s\n\t selval[SIZE] = %d\n", qval[DATA_NAME], qval[RSRC_NAME], qval[DATA_GRP_NAME], selval[SIZE] );
		status = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult, 200) ;
		if (status < 0)
		{
			fprintf(stderr,"srboseek: srbGetDataDirInfo failed (status = %d)\n", status);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		result_count = myresult.result_count ;
		if (result_count != 1)
		{
			fprintf(stderr,"srboseek: result_count (%d) from srbGetDataDirInfo != 1\n", status);
			clFinish(conn);
			exit (1);
		}
		filebytes = atoi((char *) myresult.sqlresult[0].values) ;
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboseek: object \"%s\" is size %d\n", objID, filebytes);
		if ((seek+bytes) > filebytes)
		{
			fprintf(stderr,"srboseek: ? sum of requested bytes (%d) and seek offset (%d) for object \"%s\" is greater than object size (%d)\n", bytes, seek, objID, filebytes);
			clFinish(conn);
			exit (1);
		}

		/* open external store */
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboseek: Attempting to call srbObjOpen with\n\t objID = %s\n\t collectionName = %s\n", objID, coll );
		fd = srbObjOpen(conn, objID, O_RDONLY, coll);
		if (fd < 0)
		{
			fprintf(stderr,"srboseek: Unable to open object (fd = %d)\n", fd);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboseek: open'ed object with fd = %d\n", fd);

		/* seek */
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboseek: Attempting to call srbObjSeek with\n\t seek = %d\n\t whence = SEEK_SET\n", seek );
		status = srbObjSeek(conn, fd, seek, SEEK_SET);
		if (status < 0)
		{
			fprintf(stderr,"srboseek: Unable to seek (status = %d)\n", status);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboseek: seek o.k.\n");

		/* get data */
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srboseek: reading data ...\n") ;
		rembytes = bytes ;
		while (rembytes > 0)
		{
			if (bufsize < rembytes)
				bufbytes = bufsize ;
			else
				bufbytes = rembytes ;

			srbrstat = srbObjRead(conn, fd, buf, bufbytes) ;
			if (srbrstat < 0)
			{
				fprintf(stderr, "srboseek : ? srbObjRead failed for \"%s\" (srbrstat = %d)\n", objID, srbrstat) ;
				fprintf(stderr,"%s",clErrorMessage(conn));
				fclose(fp) ;
				srbObjClose (conn, fd);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srboseek: read %d bytes\n", srbrstat) ;
			bufbytes = srbrstat ;

			wstat = fwrite((void *) buf, bufbytes, 1, fp) ;
			status = ferror(fp) ;
			if ((wstat != 1) || (status != 0))
			{
				fprintf(stderr, "srboseek : ? fwrite failed for \"%s\" (wstat = %d, ferror = %d)\n", localfile, wstat, status) ;
				fclose(fp) ;
				srbObjClose (conn, fd);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srboseek: wrote %d bytes\n", wstat*bufbytes) ;

			rembytes -= (long) bufbytes ;
		}


		/* close output file */

		fclose(fp) ;

		/* close external store */

		srbObjClose (conn, fd);

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srboseek: I/O completed successfully.\n\n") ;
	}



	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboseek: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboseek: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
