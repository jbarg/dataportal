/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srboget.c--
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
	const char* progname = "srboget" ;
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
	char *tmpChr;
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

for (k =0; k < argc ; k++)
  printf("argv[%d] =%s**\n",k,argv[k]);

	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_RSRC] = 1 ;
	srbargc[SRB_USER_ARG_ID] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_COLL] = 1 ;
	srbargc[SRB_USER_ARG_TO] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_BSIZE] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srboget: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	rsrc = (char *) srbargv[SRB_USER_ARG_RSRC][0] ;
	coll = (char *) srbargv[SRB_USER_ARG_COLL][0] ;


	/* get the bufsize */

	bufsize = *((int*) srbargv[SRB_USER_ARG_BSIZE][0]) ;
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr,"srboget: bufsize = %d\n", bufsize);
	buf = (char*) malloc(sizeof(char)*bufsize) ;
	if (buf == (char*) NULL)
	{
		fprintf(stderr,"srboget: buf malloc failed.\n");
		exit(1);
	}


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboget : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
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
		fprintf(stderr, "srboget: connected.\n\n") ;


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
			perror("srboget") ;
			exit(1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srboget: open'ed \"%s\" for write\n", localfile);

		/* get external size */
		for (k=0 ; k<MAX_DCS_NUM ; k++)
		{
			qval[k][0] = '\0' ;
			selval[k] = 0 ;
		}
		/***
		if ((tmpChr = strstr(objID, "&")) != NULL) {
		  *tmpChr = '\0';
		  sprintf(qval[DATA_NAME]," = '%s'", objID);
		  *tmpChr = '&';
		  while ((tmpChr = strstr(objID, "\\*")) != NULL) {
		    *tmpChr = ' ';
		  }
		  while ((tmpChr = strstr(objID, "\\?")) != NULL) {
		    *tmpChr = ' ';
		  }
		}
		else {
		  sprintf(qval[DATA_NAME]," = '%s'", objID);
		}
		***/
		sprintf(qval[DATA_NAME]," = '%s'", objID);
		sprintf(qval[DATA_GRP_NAME]," = '%s'", coll);
		sprintf(qval[RSRC_NAME]," = '%s'", rsrc);
		selval[SIZE] = 1 ;
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboget: Attempting to call srbGetDataDirInfo with\n\t qval[DATA_NAME] = %s\n\t qval[RSRC_NAME] = %s\n\t qval[DATA_GRP_NAME] = %s\n\t selval[SIZE] = %d\n", qval[DATA_NAME], qval[RSRC_NAME], qval[DATA_GRP_NAME], selval[SIZE] );
		status = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult, 200) ;
		if (status < 0)
		{
			fprintf(stderr,"srboget: srbGetDataDirInfo failed (status = %d)\n", status);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		result_count = myresult.result_count ;
		if (result_count != 1)
		{
			fprintf(stderr,"srboget: result_count (%d) from srbGetDataDirInfo != 1\n", status);
			clFinish(conn);
			exit (1);
		}
		filebytes = atoi((char *) myresult.sqlresult[0].values) ;
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboget: object \"%s\" is size %d\n", objID, filebytes);

		/* get external store */
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboget: Attempting to call srbObjOpen with\n\t objID = %s\n\t collectionName = %s\n", objID, coll );
		fd = srbObjOpen(conn, objID, O_RDONLY, coll);
		if (fd < 0)
		{
			fprintf(stderr,"srboget: Unable to open object (fd = %d)\n", fd);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboget: open'ed object with fd = %d\n", fd);

		/* get data */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srboget: reading data ...\n") ;
		rembytes = filebytes ;
		if (rembytes <= 0)
		  rembytes = 9999999;
		while (rembytes > 0)
		{
			if (bufsize < rembytes)
				bufbytes = bufsize ;
			else
				bufbytes = rembytes ;

			srbrstat = srbObjRead(conn, fd, buf, bufbytes) ;
			if (srbrstat < 0)
			{
				fprintf(stderr, "srboget : ? srbObjRead failed for \"%s\" (srbrstat = %d)\n", objID, srbrstat) ;
				fprintf(stderr,"%s",clErrorMessage(conn));
				fclose(fp) ;
				srbObjClose (conn, fd);
				clFinish(conn);
				exit(1) ;
			}
			if (srbrstat == 0) 
			  break;
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srboget: read %d bytes\n", srbrstat) ;
			bufbytes = srbrstat ;

			wstat = fwrite((void *) buf, bufbytes, 1, fp) ;
			status = ferror(fp) ;
			if ((wstat != 1) || (status != 0))
			{
				fprintf(stderr, "srboget : ? fwrite failed for \"%s\" (wstat = %d, ferror = %d)\n", localfile, wstat, status) ;
				fclose(fp) ;
				srbObjClose (conn, fd);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srboget: wrote %d bytes\n", wstat*bufbytes) ;

			rembytes -= (long) bufbytes ;
		}


		/* close output file */

		fclose(fp) ;

		/* close external store */

		srbObjClose (conn, fd);

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srboget: I/O completed successfully.\n\n") ;
	}



	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboget: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboget: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
