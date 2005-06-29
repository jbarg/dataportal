/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbmodcoll.c--
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
	const char* progname = "srbmodcoll" ;
	int srbargc[SRB_USER_ARGS] ;
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC] ;
	int k ;
	int ncolls ;
	int n ;
	char *host ;
	char *port ;
	char *auth ;
	int status ;
	int catType ;
	char *collectionName;
	char *nullstr = "" ;
	char *dataValue1 ;
	char *dataValue2 ;
	char *dataValue3 ;
	int retractionType ;
	char *retractv[SRB_OP_COUNT] = SRB_OP_NAMES ;

	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_CATT] = 1 ;
	srbargc[SRB_USER_ARG_COLL] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_OP] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_ATTR1] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_ATTR2] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_ATTR3] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbmodcoll: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	catType = *((int*) srbargv[SRB_USER_ARG_CATT][0]) ;
	retractionType = *((int*) srbargv[SRB_USER_ARG_OP][0]) ;
	if (srbargc[SRB_USER_ARG_ATTR1] == 0)
		dataValue1 = nullstr ;
	else
		dataValue1 = (char *) srbargv[SRB_USER_ARG_ATTR1][0] ;
	if (srbargc[SRB_USER_ARG_ATTR2] == 0)
		dataValue2 = nullstr ;
	else
		dataValue2 = (char *) srbargv[SRB_USER_ARG_ATTR2][0] ;
	if (srbargc[SRB_USER_ARG_ATTR3] == 0)
		dataValue3 = nullstr ;
	else
		dataValue3 = (char *) srbargv[SRB_USER_ARG_ATTR3][0] ;


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbmodcoll : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
	conn = clConnect (host, port, auth);

	/* check to see that the backend connection was successfully made */
	if (clStatus(conn) != CLI_CONNECTION_OK)
	{
		fprintf(stderr,"srbmodcoll: Connection to srbMaster failed.\n");
		fprintf(stderr,"%s",clErrorMessage(conn));
		clFinish(conn);
		exit(1);
	}
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbmodcoll: connected.\n\n") ;


	/* modify collections */

	ncolls = srbargc[SRB_USER_ARG_COLL] ;

	for (n=0 ; n<ncolls ; n++)
	{
		collectionName = (char*) srbargv[SRB_USER_ARG_COLL][n] ;

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbmodcoll: Attempting srbModifyCollect with\n\t catType = %d\n\t collectionName = %s\n\t dataValue1 = %s\n\t dataValue2 = %s\n\t dataValue3 = %s\n\t retractionType = %d (%s)\n", catType, collectionName, dataValue1, dataValue2, dataValue3, retractionType, retractv[retractionType-1]) ;

		status = srbModifyCollect(conn, catType, collectionName, dataValue1, dataValue2, dataValue3, retractionType) ;
		if (status < 0)
		{
			fprintf(stderr,"srbmodcoll: srbModifyCollect failed (status = %d)\n", status) ;
			fprintf(stderr,"%s",clErrorMessage(conn));
			exit(1) ;
	        }
	}


	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbmodcoll: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbmodcoll: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
