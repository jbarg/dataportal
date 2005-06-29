/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbftpbrowse.c--
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
	const char* progname = "srbftpbrowse" ;
	int dindex ;
	int srbargc[SRB_USER_ARGS] ;
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC] ;
	int k ;
	int ndirs ;
	int nfiles ;
	int n ;
	char *host ;
	char *port ;
	char *auth ;
	char *systname[SRB_SYST_COUNT] = SRB_SYST_NAMES ;
	char *rsrc ;
	char *systv ;
	int syst ;
	char *dir ;
	char *userAuth = "ftp:srb@sdsc.edu" ;


	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_DIR] = 1 ;
	srbargc[SRB_USER_ARG_RSRC] = 1 ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbftpbrowse: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	rsrc = (char *) srbargv[SRB_USER_ARG_RSRC][0] ;
	syst = 3 ;
	systv = systname[syst] ;
	dir = (char *) srbargv[SRB_USER_ARG_DIR][0] ;


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbftpbrowse : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
	conn = clConnect (host, port, auth);

	/* check to see that the backend connection was successfully made */
	if (clStatus(conn) != CLI_CONNECTION_OK)
	{
		fprintf(stderr,"srbftpbrowse: Connection to srbMaster failed.\n");
		fprintf(stderr,"%s",clErrorMessage(conn));
		clFinish(conn);
		exit(1);
	}
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbftpbrowse: connected.\n\n") ;


	/* set directory info */

	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr,"srbftpbrowse: Attempting srbSetStorAttri with\n\t storSysType = %s (%d)\n\t hostAddr = %s\n\t userAuth = %s\n\t dirPath = %s\n", systv, syst, rsrc, userAuth, dir) ;

	dindex = srbSetStorAttri(conn, syst, rsrc, userAuth, dir);
	if (dindex < 0)
	{
		fprintf(stderr,"srbftpbrowse: srbSetStorAttri failed (dindex = %d)\n", dindex) ;
		fprintf(stderr,"%s",clErrorMessage(conn));
		exit (1);
	}

	/* read directory info */

	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr,"srbftpbrowse: Attempting srbReaddir with dindex = %d\n", dindex) ;


	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbftpbrowse: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbftpbrowse: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;


	exit(0);
}
