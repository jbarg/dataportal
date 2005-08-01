/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbaudittrail.c--
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
	struct clHostElement *hostQueHead ;
	const char* progname = "srbaudittrail" ;
	int protocol ;
	int srbargc[SRB_USER_ARGS] ;
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC] ;
	int k ;
	int ndirs ;
	int nfiles ;
	int n ;
	char *host ;
	char *port ;
	char *auth ;
	int trailon ;
	int trailoff ;
	int trailprint ;
	int trailstatus ;
	char *trailstr[2] = {"AUDIT_TRAIL_OFF", "AUDIT_TRAIL_ON"} ;

	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_ON] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_OFF] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbaudittrail: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	trailon = (srbargc[SRB_USER_ARG_ON] != 0) ;
	trailoff = (srbargc[SRB_USER_ARG_OFF] != 0) ;
	trailprint = (srbargc[SRB_USER_ARG_PRINT] != 0) ;

	if (trailon && trailoff)
	{
		fprintf(stderr,"srbaudittrail: both \"-on\" and \"-off\" ?  Make up your mind!\n");
		exit(1);
	}


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbaudittrail : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
	conn = clConnect (host, port, auth);

	/* check to see that the backend connection was successfully made */
	if (clStatus(conn) != CLI_CONNECTION_OK)
	{
		fprintf(stderr,"srbaudittrail: Connection to srbMaster failed.\n");
		fprintf(stderr,"%s",clErrorMessage(conn));
		clFinish(conn);
		exit(1);
	}
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbaudittrail: connected.\n") ;


	/* check the audit trail status */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbaudittrail: checking status.\n") ;
	trailstatus = srbSetAuditTrail(conn, GET_AUDIT_TRAIL_SETTING) ;
	if ((trailstatus != AUDIT_TRAIL_OFF) && (trailstatus != AUDIT_TRAIL_ON))
	{
		fprintf(stderr,"srbaudittrail: ? Unknown srbSetAuditTrail setting (%d)\n", trailstatus);
		exit(1);
	}
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbaudittrail: current status = %s\n", trailstr[trailstatus]) ;

	if (trailon)
	{
		trailstatus = srbSetAuditTrail(conn, AUDIT_TRAIL_ON) ;
		if (trailstatus != AUDIT_TRAIL_ON)
		{
			fprintf(stderr,"srbaudittrail: srbSetAuditTrail failed (trailstatus = %d)\n", trailstatus);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit(1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbaudittrail: srbSetAuditTrail = %s\n", trailstr[trailstatus]) ;
	}

	if (trailoff)
	{
		trailstatus = srbSetAuditTrail(conn, AUDIT_TRAIL_OFF) ;
		if (trailstatus != AUDIT_TRAIL_OFF)
		{
			fprintf(stderr,"srbaudittrail: srbSetAuditTrail failed (trailstatus = %d)\n", trailstatus);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit(1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbaudittrail: srbSetAuditTrail = %s\n", trailstr[trailstatus]) ;
	}


	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbaudittrail: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbaudittrail: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}

