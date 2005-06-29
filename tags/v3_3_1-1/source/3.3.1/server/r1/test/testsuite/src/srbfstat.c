/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbfstat.c--
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
	struct srbStat statbuf ;
	const char* progname = "srbfstat" ;
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
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbfstat: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	rsrc = srbargv[SRB_USER_ARG_RSRC][0] ;
	syst = *((int*) srbargv[SRB_USER_ARG_SYST][0]) ;
	systv = systname[syst] ;


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfstat : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
	conn = clConnect (host, port, auth);

	/* check to see that the backend connection was successfully made */
	if (clStatus(conn) != CLI_CONNECTION_OK)
	{
		fprintf(stderr,"srbfstat: Connection to srbMaster failed.\n");
		fprintf(stderr,"%s",clErrorMessage(conn));
		clFinish(conn);
		exit(1);
	}
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfstat: connected.\n\n") ;


	nfiles = srbargc[SRB_USER_ARG_FILE] ;
	for (n=0 ; n<nfiles ; n++)
	{
		externstore = (char*) srbargv[SRB_USER_ARG_FILE][n];

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbfstat: Attempting srbFileStat with\n\t storSysType = %s (%d)\n\t hostAddr = %s\n\t filename = %s\n", systv, syst, rsrc, externstore) ;

        	status = srbFileStat(conn, syst, rsrc, externstore, &statbuf) ;
        	if (status != 0)
		{
			fprintf(stderr,"srbfstat: srbFileStat failed (status = %d)\n", status) ;
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit(1);
	        }


		fprintf(stdout,"Stat of \"%s\" on \"%s\" using \"%s\"\n", externstore, (char*) srbargv[SRB_USER_ARG_RSRC][0], systname[*((int*) srbargv[SRB_USER_ARG_SYST][0])] );
		fprintf(stdout, "st_dev      = %x \n", statbuf.st_dev);
		fprintf(stdout, "st_ino      = %x \n", statbuf.st_ino);
		fprintf(stdout, "st_mode     = %x \n", statbuf.st_mode);
		fprintf(stdout, "st_nlink    = %d \n", statbuf.st_nlink);
		fprintf(stdout, "st_uid      = %d \n", statbuf.st_uid);
		fprintf(stdout, "st_gid      = %d \n", statbuf.st_gid);
		fprintf(stdout, "st_rdev     = %x \n", statbuf.st_rdev);
		fprintf(stdout, "st_size     = %d \n", statbuf.st_size);
		fprintf(stdout, "st_atim     = %d \n", statbuf.st_atim);
		fprintf(stdout, "st_spare1   = %d \n", statbuf.st_spare1);
		fprintf(stdout, "st_mtim     = %d \n", statbuf.st_mtim);
		fprintf(stdout, "st_spare2   = %d \n", statbuf.st_spare2);
		fprintf(stdout, "st_ctim     = %d \n", statbuf.st_ctim);
		fprintf(stdout, "st_spare3   = %d \n", statbuf.st_spare3);
		fprintf(stdout, "st_blksize  = %d \n", statbuf.st_blksize);
		fprintf(stdout, "st_blocks   = %d \n", statbuf.st_blocks);
		fprintf(stdout, "st_vfstype  = %d \n", statbuf.st_vfstype);
		fprintf(stdout, "st_vfs      = %x \n", statbuf.st_vfs);
		fprintf(stdout, "st_type     = %x \n", statbuf.st_type);
		fprintf(stdout, "st_flag     = %x \n", statbuf.st_flag);
		fprintf(stdout, "st_gen      = %x \n", statbuf.st_gen);
		fprintf(stdout, "\n");
	}


	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfstat: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbfstat: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;


	exit(0);
}
