/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srboticket.c--
 *
 * See README file or execute with -help flag.
 *
 *-------------------------------------------------------------------------
 */

#include "srbUserArgs.h"
#include "srbClient.h"

#define COLLFLAG        "D"
#define BEGINTIME       NULL
#define ENDTIME         NULL
#define ACCESSCNT       -1

int
main(int argc, char **argv)
{
	srbConn *conn, *conn1;
	const char* progname = "srboticket" ;
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
	char *ticket;


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
		fprintf(stderr, "srboticket: aborting on srbUserArgs error\n");
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
		fprintf(stderr,"srboticket: bufsize = %d\n", bufsize);
	buf = (char*) malloc(sizeof(char)*bufsize) ;
	if (buf == (char*) NULL)
	{
		fprintf(stderr,"srboticket: buf malloc failed.\n");
		exit(1);
	}


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboticket : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
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
		fprintf(stderr, "srboticket: connected.\n\n") ;


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
			perror("srboticket") ;
			exit(1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srboticket: open'ed \"%s\" for write\n", localfile);

                if ((status = srbIssueTicket (conn, objID, coll, COLLFLAG,
                  BEGINTIME, ENDTIME, ACCESSCNT, "", &ticket)) < 0) {
                    fprintf(stderr, "can't srbIssueTicket obj\"%s\", %d\n",
                      argv[1], status);
                    clFinish(conn);
                    exit (1);
                }

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
			fprintf(stderr,"srboticket: Attempting to call srbGetDataDirInfo with\n\t qval[DATA_NAME] = %s\n\t qval[RSRC_NAME] = %s\n\t qval[DATA_GRP_NAME] = %s\n\t selval[SIZE] = %d\n", qval[DATA_NAME], qval[RSRC_NAME], qval[DATA_GRP_NAME], selval[SIZE] );
		status = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult, 200) ;
		if (status < 0)
		{
			fprintf(stderr,"srboticket: srbGetDataDirInfo failed (status = %d)\n", status);
			fprintf(stderr,"%s",clErrorMessage(conn));
			srbRemoveTicket (conn, ticket);
			clFinish(conn);
			exit (1);
		}
		result_count = myresult.result_count ;
		if (result_count != 1)
		{
			fprintf(stderr,"srboticket: result_count (%d) from srbGetDataDirInfo != 1\n", status);
			srbRemoveTicket (conn, ticket);
			clFinish(conn);
			exit (1);
		}
		filebytes = atoi((char *) myresult.sqlresult[0].values) ;
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboticket: object \"%s\" is size %d\n", objID, filebytes);

		/* get external store */
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboticket: Attempting to call srbObjOpen with\n\t objID = %s\n\t collectionName = %s\n", objID, coll );

		/* Login as TICKET_USER */

		conn1 = tiUserConnect (host, port);
        	if (clStatus(conn1) != CLI_CONNECTION_OK)
        	{
                    fprintf(stderr,"Connection to srbMaster failed.\n");
                    fprintf(stderr,"%s",clErrorMessage(conn1));
		    srbRemoveTicket (conn, ticket);
                    clFinish(conn);
                    exit(1);
        	}


		sleep (5);	/* For some reason, Oracle DBMS reqires
				 * some time for a commit transaction to stick.
				 */

		fd = srbObjOpenWithTicket (conn1, objID, O_RDONLY, coll,
		ticket);
		if (fd < 0)
		{
			fprintf(stderr,"srboticket: Unable to open object (fd = %d)\n", fd);
			fprintf(stderr,"%s",clErrorMessage(conn1));
			srbRemoveTicket (conn, ticket);
			clFinish(conn);
			exit (1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboticket: open'ed object with fd = %d\n", fd);

		/* get data */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srboticket: reading data ...\n") ;
		rembytes = filebytes ;
		while (rembytes > 0)
		{
			if (bufsize < rembytes)
				bufbytes = bufsize ;
			else
				bufbytes = rembytes ;

			srbrstat = srbObjRead(conn1, fd, buf, bufbytes) ;
			if (srbrstat < 0)
			{
				fprintf(stderr, "srboticket : ? srbObjRead failed for \"%s\" (srbrstat = %d)\n", objID, srbrstat) ;
				fprintf(stderr,"%s",clErrorMessage(conn));
				fclose(fp) ;
				srbObjClose (conn, fd);
				srbRemoveTicket (conn, ticket);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srboticket: read %d bytes\n", srbrstat) ;
			bufbytes = srbrstat ;

			wstat = fwrite((void *) buf, bufbytes, 1, fp) ;
			status = ferror(fp) ;
			if ((wstat != 1) || (status != 0))
			{
				fprintf(stderr, "srboticket : ? fwrite failed for \"%s\" (wstat = %d, ferror = %d)\n", localfile, wstat, status) ;
				fclose(fp) ;
				srbObjClose (conn, fd);
				srbRemoveTicket (conn, ticket);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srboticket: wrote %d bytes\n", wstat*bufbytes) ;

			rembytes -= (long) bufbytes ;
		}


		/* close output file */

		fclose(fp) ;

		/* close external store */

		srbObjClose (conn1, fd);

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srboticket: I/O completed successfully.\n\n") ;
	}



	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboticket: closing connection ...\n") ;
	srbRemoveTicket (conn, ticket);
	clFinish(conn);
	clFinish(conn1);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboticket: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
