/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
/*-------------------------------------------------------------------------
 *
 * srbopcopy.c-- proxy copy. The "-to" input now specifies the target srcObjId.
 *
 * See README file or execute with -help flag.
 *
 *-------------------------------------------------------------------------
 */

#include "srbUserArgs.h"
#include "srbClient.h"
#define SML_BUF_SZ      16


int
main(int argc, char **argv)
{
	srbConn *conn;
	const char* progname = "srbopcopy" ;
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
	char *srcObjId ;
	char *rsrc ;
	char *coll ;
	mdasC_sql_result_struct myresult ;
	int result_count ;
	char qval[MAX_DCS_NUM][MAX_TOKEN] ;
	int selval[MAX_DCS_NUM] ;
	int status ;
	char in_fd_str[SML_BUF_SZ], out_fd_str[SML_BUF_SZ];

	int c ;
	char *buf ;
	char *destObjId ;
	struct stat statbuf ;
	long filebytes ;
	int in_fd, out_fd;
	long rembytes ;
	int bufbytes ;
	int nbytes ;
        int catType ;
        char *dtyp ;
	char *externstore ;

	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_RSRC] = 1 ;
	srbargc[SRB_USER_ARG_CATT] = 1 ;
        srbargc[SRB_USER_ARG_TYPE] = 1 ;
	srbargc[SRB_USER_ARG_ID] = SRB_ARG_REQUIRED ; 	/* the source obj id */
	srbargc[SRB_USER_ARG_TO] = SRB_ARG_REQUIRED ;	/* the target obj id */
	srbargc[SRB_USER_ARG_OBJ] = SRB_ARG_OPTIONAL ;  /* pathName of new obj */
	srbargc[SRB_USER_ARG_COLL] = 1 ;
	srbargc[SRB_USER_ARG_BSIZE] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbopcopy: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	rsrc = (char *) srbargv[SRB_USER_ARG_RSRC][0] ;
        catType = *((int*) srbargv[SRB_USER_ARG_CATT][0]) ;
        dtyp = (char *) srbargv[SRB_USER_ARG_TYPE][0] ;
	coll = (char *) srbargv[SRB_USER_ARG_COLL][0] ;


	/* get the bufsize */

	bufsize = *((int*) srbargv[SRB_USER_ARG_BSIZE][0]) ;
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr,"srbopcopy: bufsize = %d\n", bufsize);
	buf = (char*) malloc(sizeof(char)*bufsize) ;
	if (buf == (char*) NULL)
	{
		fprintf(stderr,"srbopcopy: buf malloc failed.\n");
		exit(1);
	}


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbopcopy : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
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
		fprintf(stderr, "srbopcopy: connected.\n\n") ;


	/* process the files in "-id" */

	nfiles = srbargc[SRB_USER_ARG_ID] ;

	for (n=0 ; n<nfiles; n++)
	{
		srcObjId = srbargv[SRB_USER_ARG_ID][n] ;
		destObjId = srbargv[SRB_USER_ARG_TO][n] ;
		externstore = srbargv[SRB_USER_ARG_OBJ][n] ;

		/* get external size */
		for (k=0 ; k<MAX_DCS_NUM ; k++)
		{
			qval[k][0] = '\0' ;
			selval[k] = 0 ;
		}
		sprintf(qval[DATA_NAME]," = '%s'", srcObjId);
		sprintf(qval[DATA_GRP_NAME]," = '%s'", coll);
		selval[SIZE] = 1 ;
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbopcopy: Attempting to call srbGetDataDirInfo with\n\t qval[DATA_NAME] = %s\n\t qval[RSRC_NAME] = %s\n\t qval[DATA_GRP_NAME] = %s\n\t selval[SIZE] = %d\n", qval[DATA_NAME], qval[RSRC_NAME], qval[DATA_GRP_NAME], selval[SIZE] );
		status = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult, 200) ;
		if (status < 0)
		{
			fprintf(stderr,"srbopcopy: srbGetDataDirInfo failed (status = %d)\n", status);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		result_count = myresult.result_count ;
		if (result_count != 1)
		{
			fprintf(stderr,
			  "srbopcopy: result_count (%d) from srbGetDataDirInfo != 1\n", status);
			clFinish(conn);
			exit (1);
		}
		filebytes = atoi((char *) myresult.sqlresult[0].values) ;
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbopcopy: object \"%s\" is size %d\n", 
			  srcObjId, filebytes);

                /* Create the target object */

                out_fd = srbObjCreate (conn, catType, destObjId, 
		  dtyp, rsrc, coll, externstore, filebytes );
                if (out_fd < 0)
                {
			fprintf (stderr,
			  "srbopcopy: unable to create obj: %s, status = %d\n", 
			    destObjId, out_fd);
			fprintf(stderr,"%s",clErrorMessage(conn));
                        clFinish(conn);
                        exit(1);
                }
                if (srbargc[SRB_USER_ARG_VERB] != 0)
                        fprintf(stderr, 
			  "srbopcopy: created \"%s\" for write\n", destObjId);

		/* open srcObjId */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,
			  "srbopcopy: Attempting to call srbObjOpen with\n\t");
			fprintf(stderr, "srcObjId = %s\n\t collectionName = %s\n", 
			    srcObjId, coll );

		in_fd = srbObjOpen(conn, srcObjId, O_RDONLY, coll);
		if (in_fd < 0)
		{
			fprintf(stderr,
			  "srbopcopy: Unable to open object %s (fd = %d)\n", 
			    srcObjId, in_fd);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,
			  "srbopcopy: open'ed object with fd = %d\n", in_fd);

		/* get data */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbopcopy: do a proxy copy ...\n") ;

		sprintf (in_fd_str, "%d", in_fd);
    		sprintf (out_fd_str, "%d", out_fd);
		nbytes = srbObjProxyOpr (conn, OPR_COPY, in_fd_str, out_fd_str, 
					 0, 0,
         NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

		if (nbytes < 0) {
			fprintf(stderr, 
			  "srbopcopy: proxy copy error, status = %d\n", 
			    nbytes);
			srbObjClose (conn, in_fd);
			srbObjClose (conn, out_fd);
			clFinish(conn);
			exit (1);
		}

		/* close the opened objects */

		srbObjClose (conn, in_fd);
		srbObjClose (conn, out_fd);

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, 
			  "srbopcopy: I/O completed successfully.\n\n") ;
	}



	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbopcopy: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbopcopy: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
