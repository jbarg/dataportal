/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srboregister.c--
 *
 * See README file or execute with -help flag.
 *
 *-------------------------------------------------------------------------
 */
/**
srboregister -host $HOST -port $PORT -auth $AUTH $VERBOSE -rsrc $ORIGRSRC -cattype $CAT  -id $TESTTABID -obj $ORIGTABOBJ -coll $PARENTCOLL/$TESTCOLL -type $TESTTABTYPE
**/
#include "srbUserArgs.h"
#include "srbClient.h"


int
main(int argc, char **argv)
{
	srbConn *conn;
	const char* progname = "srboregister" ;
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
	int catType ;
	char *objID ;
	char *rsrc ;
	char *dtyp ;
	char *coll ;
	int status ;

	int ii;
	int c ;
	char *buf ;
	char *localfile ;
	char *externstore ;
	struct stat statbuf ;
	long filebytes ;
	FILE *fp ;
	int fd;
	long rembytes ;
	int bufbytes ;
	size_t rstat ;
	int srbwstat ;


	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_ID] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_OBJ] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_RSRC] = 1 ;
	srbargc[SRB_USER_ARG_CATT] = 1 ;
	srbargc[SRB_USER_ARG_TYPE] = 1 ;
	srbargc[SRB_USER_ARG_COLL] = 1 ;
	srbargc[SRB_USER_ARG_BSIZE] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srboregister: aborting on srbUserArgs error\n");
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
	buf = (char*) malloc(sizeof(char)*bufsize) ;
	if (buf == (char*) NULL)
	{
		fprintf(stderr,"srboregister: buf malloc failed.\n");
		exit(1);
	}


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboregister : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
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
		fprintf(stderr, "srboregister: connected.\n\n") ;


	/* process the files in "-from" */
	/*
	if ( srbargc[SRB_USER_ARG_TYPE] > 1) {
	  dtyp =  dtypStr;
	  strcat(dtypStr, srbargv[SRB_USER_ARG_TYPE][0]);
	  for (n = 1; n <  srbargc[SRB_USER_ARG_TYPE] ; n++) {
	    strcat(dtyp, " ");
	    strcat(dtyp,srbargv[SRB_USER_ARG_TYPE][n]);
	  }
	}
	*/
	nfiles = srbargc[SRB_USER_ARG_ID] ;

	
	for (n=0 ; n<nfiles; n++)
	{
		objID = srbargv[SRB_USER_ARG_ID][n] ;
		externstore = srbargv[SRB_USER_ARG_OBJ][n] ;
		if (!strcmp(externstore,"''"))
		  strcpy(externstore,"");
		/* register external store */
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboregister: Attempting to call srbObjRegister with\n\t catType = %d\n\t objID = %s\n\t dataTypeName = %s\n\t resourceName = %s\n\t collectionName = %s\n\t pathName = %s\n\t\n", catType, objID, dtyp, rsrc, coll, externstore);

		ii = srbRegisterDataset(conn, catType, objID, dtyp, rsrc, coll, externstore, 0 );
		if (ii < 0)
		{
			fprintf(stderr,"srboregister: Unable to register object (status = %d)\n", ii);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srboregister: registered object with fd = %d\n", fd);

	}



	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboregister: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboregister: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
