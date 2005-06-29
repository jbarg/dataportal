/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
/*-------------------------------------------------------------------------
 *
 * srbtabotruncate.c-- truncate table and unregister srbObject
 *
 * See README file or execute with -help flag.
 *
 *-------------------------------------------------------------------------
 */
/** srbtabotruncate -host $HOST -port $PORT -auth $AUTH $VERBOSE -id $TESTTABIDTRUNCATE -coll $PARENTCOLL/$TESTCOLL
**/


#include "srbUserArgs.h"
#include "srbClient.h"


int
main(int argc, char **argv)
{
	srbConn *conn;
	const char* progname = "srbtabotruncate" ;
	int protocol ;
	int srbargc[SRB_USER_ARGS] ;
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC] ;
	int k ;
	int nobjs ;
	int n ;
	char *host ;
	char *port ;
	char *auth ;
	char *coll ;
	char *tmpStr;
	int c ;
	char *externstore ;
	int status;
	int fd;


	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_ID] = 1 ;
	srbargc[SRB_USER_ARG_COLL] = 1 ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbtabotruncate: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	coll = (char *) srbargv[SRB_USER_ARG_COLL][0] ;


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbtabotruncate : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
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
		fprintf(stderr, "srbtabotruncate: connected.\n\n") ;


	/* process the files in "-id" */

	nobjs = srbargc[SRB_USER_ARG_ID] ;

	for (n=0 ; n<nobjs; n++)
	{
		externstore = srbargv[SRB_USER_ARG_ID][n] ;
		/* truncate  external store */
		fd = srbObjOpen(conn, externstore, O_RDWR, coll);
		if (fd < 0)
		{
			fprintf(stderr,"srbtabotruncate: Unable to open object (fd = %d)\n", fd);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
		  fprintf(stderr,"srbtabotruncate: open'ed and truncated object with fd = %d\n", fd);
		srbObjClose (conn, fd);
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbtabotruncate: truncation object closed successfully.\n\n") ;

	}




	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbtabotruncate: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbtabotruncate: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
