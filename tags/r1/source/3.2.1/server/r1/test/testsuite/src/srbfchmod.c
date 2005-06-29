/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbfchmod.c--
 *    test file deletion
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
	const char* progname = "srbfchmod" ;
	int protocol ;
	int srbargc[SRB_USER_ARGS] ;
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC] ;
	int k ;
	int ndirs ;
	int nfiles ;
	int n ;
	int mode ;
	char *host ;
	char *port ;
	char *auth ;
	char *externstore ;
	char *systname[SRB_SYST_COUNT] = SRB_SYST_NAMES ;
	char *rsrc ;
	char *systv ;
	int syst ;
	int status ;


	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_FILE] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_SYST] = 1 ;
	srbargc[SRB_USER_ARG_RSRC] = 1 ;
	srbargc[SRB_USER_ARG_MODE] = 1 ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbfchmod: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfchmod : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
	conn = clConnect (host, port, auth);

	/* check to see that the backend connection was successfully made */
	if (clStatus(conn) != CLI_CONNECTION_OK)
	{
		fprintf(stderr,"srbfchmod: Connection to srbMaster failed.\n");
		fprintf(stderr,"%s",clErrorMessage(conn));
		clFinish(conn);
		exit(1);
	}
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfchmod: connected.\n\n") ;

	rsrc = srbargv[SRB_USER_ARG_RSRC][0] ;
	syst = *((int*) srbargv[SRB_USER_ARG_SYST][0]) ;
	systv = systname[syst] ;
	mode = *((int*) srbargv[SRB_USER_ARG_MODE][0]) ;

	nfiles = srbargc[SRB_USER_ARG_FILE] ;
	for (n=0 ; n<nfiles ; n++)
	{
		externstore = (char*) srbargv[SRB_USER_ARG_FILE][n];

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbfchmod: Attempting srbFileChmod with\n\t storSysType = %s (%d)\n\t hostAddr = %s\n\t filename = %s\n\t mode = %d\n", systv, syst, rsrc, externstore, mode) ;

        	status = srbFileChmod(conn, syst, rsrc, externstore, mode) ;
        	if (status != 0)
		{
			fprintf(stderr,"srbfchmod: srbFileChmod failed (status = %d)\n", status) ;
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit(1);
	        }
	}


	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfchmod: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfchmod: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;


	exit(0);
}
