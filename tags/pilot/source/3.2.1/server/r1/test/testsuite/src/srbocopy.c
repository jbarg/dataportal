/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbocopy.c--
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
	const char* progname = "srbocopy" ;
	int protocol ;
	int srbargc[SRB_USER_ARGS] ;
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC] ;
	int k ;
	int n ;
	char *host ;
	char *port ;
	char *auth ;
	char *coll ;
	int c ;
	int catType ;
	char *objID ;
	char *origResourceName ;
	char *origPathName ;
	char *newResourceName ;
	char *newPathName ;
	int status;


	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_CATT] = 1 ;
	srbargc[SRB_USER_ARG_ID] = 1 ;
	srbargc[SRB_USER_ARG_COLL] = 1 ;
	srbargc[SRB_USER_ARG_OBJ] = 2 ;
	srbargc[SRB_USER_ARG_RSRC] = 2 ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbocopy: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	coll = (char *) srbargv[SRB_USER_ARG_COLL][0] ;
	catType = *((int*) srbargv[SRB_USER_ARG_CATT][0]) ;
	objID = srbargv[SRB_USER_ARG_ID][0] ;
	origResourceName = (char *) srbargv[SRB_USER_ARG_RSRC][0] ;
	origPathName = (char *) srbargv[SRB_USER_ARG_OBJ][0] ;
	newResourceName = (char *) srbargv[SRB_USER_ARG_RSRC][1] ;
	newPathName = (char *) srbargv[SRB_USER_ARG_OBJ][1] ;


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbocopy : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
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
		fprintf(stderr, "srbocopy: connected.\n\n") ;


	/* copy */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr,"srbocopy: Attempting to call srbObjReplicate with\n\t catType = %d\n\t objID = %s\n\t collectionName = %s\n\t origPathName = %s\n\t origResourceName = %s\n\t newPathName = %s\n\t newResourceName = %s\n", catType, objID, coll, origPathName, origResourceName, newPathName, newResourceName );

	status = srbObjReplicate(conn, catType, objID, coll, newResourceName, newPathName) ;
	if (status < 0)
	{
		fprintf(stderr,"srbocopy: Unable to copy object \"%s\"\n", objID );
		fprintf(stderr,"%s",clErrorMessage(conn));
		clFinish(conn);
		exit (1);
	}
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr,"srbocopy: Copied object \"%s\"\n", objID );


	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbocopy: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbocopy: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
