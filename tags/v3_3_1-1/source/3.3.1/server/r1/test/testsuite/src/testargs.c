/*-------------------------------------------------------------------------
 *
 * testargs.c
 *
 * See README file or execute with -help flag.
 *
 *-------------------------------------------------------------------------
 */

#include "srbClient.h"
#include "srbUserArgs.h"

int
main(int argc, char **argv)
{
	const char* progname = "testargs" ;
	int protocol ;
	char *host ;
	int srbargc[SRB_USER_ARGS] ;
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC] ;
	int k ;
	int ndirs ;
	int nfiles ;
	int n ;
	char *port ;
	char *auth ;

	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_ERR] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_USER] = 1 ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "testargs: aborting on srbUserArgs error\n");
		exit(42) ;
	}


	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
