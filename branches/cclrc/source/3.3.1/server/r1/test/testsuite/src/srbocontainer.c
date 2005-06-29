/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbocontainer.c--
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
	const char* progname = "srbocontainer" ;
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
	char *container ;
	char *myContainer;
	int status ;

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
	char tmpStr[100];


	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_FROM] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_ID] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_OBJ] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_RSRC] = 1 ;
	srbargc[SRB_USER_ARG_CATT] = 1 ;
	srbargc[SRB_USER_ARG_TYPE] = 1 ;
	srbargc[SRB_USER_ARG_COLL] = 1 ;
	srbargc[SRB_USER_ARG_BSIZE] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbargc[SRB_USER_ARG_CONTAINER] = SRB_ARG_REQUIRED ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srbocontainer: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	rsrc = (char *) srbargv[SRB_USER_ARG_RSRC][0] ;
	catType = *((int*) srbargv[SRB_USER_ARG_CATT][0]) ;
	dtyp = (char *) srbargv[SRB_USER_ARG_TYPE][0] ;
	coll = (char *) srbargv[SRB_USER_ARG_COLL][0] ;
	container = (char *) srbargv[SRB_USER_ARG_CONTAINER][0] ;

	myContainer = malloc (512);
	if (getenv ("mcatZone") != NULL) {
	    sprintf (myContainer, "/%s/container/%s.%s/%s", 
	     getenv ("mcatZone"), getenv("srbUser"), getenv ("srbDomain"), 
	     container);
	} else {
            sprintf (myContainer, "/container/%s.%s/%s",
             getenv("srbUser"), getenv ("srbDomain"),
	     container);
	}
	container = myContainer;


	/* get the bufsize */

	bufsize = *((int*) srbargv[SRB_USER_ARG_BSIZE][0]) ;
	buf = (char*) malloc(sizeof(char)*bufsize) ;
	if (buf == (char*) NULL)
	{
		fprintf(stderr,"srbocontainer: buf malloc failed.\n");
		exit(1);
	}


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbocontainer : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
	conn = clConnect (host, port, auth);

	/* check to see that the backend connection was successfully made */
	if (clStatus(conn) != CLI_CONNECTION_OK)
	{
		fprintf(stderr,"Connection to srbMaster failed.\n");
		fprintf(stderr,"%s",clErrorMessage(conn));
		clFinish(conn);
		exit(1);
	}
	if (srbargc[SRB_USER_ARG_VERB] != 0) {
		fprintf(stderr, "srbocontainer: connected.\n\n") ;
		fprintf(stderr, "srbocontainer: trying to create container\n");
	}

	/* remove the object and the container first */

	objID = srbargv[SRB_USER_ARG_ID][0];
	srbObjUnlink (conn, objID, coll);

	srbRmContainer (conn, catType, container, D_DELETE_ONE);

	/* Create the container */

	status = srbContainerCreate (conn, catType, container, dtyp, rsrc,
	 10000000);

        if (status < 0) {
            fprintf(stderr,"srbocreate: Unable to create container %s, status = %d\n",
	     container, status);
            fprintf(stderr,"%s",clErrorMessage(conn));
            clFinish(conn);
            exit (1);
        }

	
	/* process the files in "-from" */

	nfiles = srbargc[SRB_USER_ARG_FROM] ;

	for (n=0 ; n<nfiles; n++)
	{
		localfile = srbargv[SRB_USER_ARG_FROM][n] ;
		objID = srbargv[SRB_USER_ARG_ID][n] ;
		externstore = srbargv[SRB_USER_ARG_OBJ][n] ;

		/* check for local file */
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbocontainer: checking for \"%s\" ...\n", localfile) ;
		if ( stat( localfile, &statbuf ) != 0 )
		{
        		srbObjUnlink (conn, objID, coll);
        		srbRmContainer (conn, catType, container, D_DELETE_ONE);
			perror("srbocontainer") ;
			exit(1);
		}
		if ( (statbuf.st_mode & S_IFMT) != S_IFREG )
		{
        		srbObjUnlink (conn, objID, coll);
        		srbRmContainer (conn, catType, container, D_DELETE_ONE);
			fprintf(stderr, "srbocontainer :  ? \"%s\" is not a regular file\n", localfile) ;
			exit(1);
		}

		/* open the file */
		filebytes = statbuf.st_size * sizeof(char) ;
		fp = fopen(localfile, "r") ;
		if (fp == (FILE *) NULL)
		{
			perror("srbocontainer") ;
        		srbObjUnlink (conn, objID, coll);
        		srbRmContainer (conn, catType, container, D_DELETE_ONE);
			exit(1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbocontainer: \"%s\" is %d bytes\n\n", localfile, filebytes);

		/* create external store */
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbocontainer: Attempting to call srbObjCreate with\n\t catType = %d\n\t objID = %s\n\t dataTypeName = %s\n\t resourceName = %s\n\t collectionName = %s\n\t pathName = %s\n\t dataSize = %d\n", catType, objID, dtyp, rsrc, coll, externstore, filebytes );

		sprintf (tmpStr, "%s&CONTAINER=%s", 
		 objID, container);
		fprintf (stderr, "ObjName=%s\n", tmpStr);
		fd = srbObjCreate(conn, catType, tmpStr, dtyp, rsrc, coll, externstore, filebytes );
		if (fd < 0)
		{
        		srbObjUnlink (conn, objID, coll);
        		srbRmContainer (conn, catType, container, D_DELETE_ONE);
			fprintf(stderr,"srbocontainer: Unable to create object (fd = %d)\n", fd);
			fprintf(stderr,"%s",clErrorMessage(conn));
			clFinish(conn);
			exit (1);
		}
		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr,"srbocontainer: created object with fd = %d\n", fd);

		/* write data */

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbocontainer: writing data ...\n") ;
		rembytes = filebytes ;
		while (rembytes > 0)
		{
			if (bufsize < rembytes)
				bufbytes = bufsize ;
			else
				bufbytes = (int) rembytes ;

			rstat = fread((void *) buf, bufbytes, 1, fp) ;
			status = ferror(fp) ;
			if ((rstat != 1) || (status != 0))
			{
				fprintf(stderr, "srbocontainer : ? fread failed for \"%s\" (rstat = %d, ferror(fp) = %d)\n", localfile, rstat, status) ;
				fclose(fp) ;
				srbObjClose (conn, fd);
        			srbObjUnlink (conn, objID, coll);
        			srbRmContainer (conn, catType, container,
				 D_DELETE_ONE);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srbocontainer: read %d bytes\n", rstat*bufbytes) ;

			srbwstat = srbObjWrite(conn, fd, buf, bufbytes) ;
			if (srbwstat < 0)
			{
				fprintf(stderr, "srbocontainer : ? srbObjWrite failed for \"%s\" (srbwstat = %d)\n", localfile, srbwstat) ;
				fprintf(stderr,"%s",clErrorMessage(conn));
				fclose(fp) ;
				srbObjClose (conn, fd);
        			srbObjUnlink (conn, objID, coll);
        			srbRmContainer (conn, catType, container,
				 D_DELETE_ONE);
				clFinish(conn);
				exit(1) ;
			}
			if (srbargc[SRB_USER_ARG_VERB] != 0)
				fprintf(stderr, "srbocontainer: wrote %d bytes\n", srbwstat) ;

			rembytes -= (long) bufbytes ;
		}

		/* close input file */

		fclose(fp) ;

		/* close external store */

		srbObjClose (conn, fd);

		if (srbargc[SRB_USER_ARG_VERB] != 0)
			fprintf(stderr, "srbocontainer: I/O completed successfully.\n\n") ;
	}


	status = srbSyncContainer (conn, catType, container, PRIMARY_FLAG);
	if (status < 0) {
            srbObjUnlink (conn, objID, coll);
            srbRmContainer (conn, catType, container, D_DELETE_ONE);
            clFinish(conn);
            exit(1) ;
        }

	status = srbSyncContainer (conn, catType, container, PURGE_FLAG);
	if (status < 0) {
            srbObjUnlink (conn, objID, coll);
            srbRmContainer (conn, catType, container, D_DELETE_ONE);
            clFinish(conn);
            exit(1) ;
        }

       	status = srbObjUnlink (conn, objID, coll);
	if (status < 0) {
            srbRmContainer (conn, catType, container, D_DELETE_ONE);
            clFinish(conn);
            exit(1) ;
        }

       	status = srbRmContainer (conn, catType, container, D_DELETE_ONE);
	if (status < 0) {
            clFinish(conn);
            exit(1) ;
        }

	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbocontainer: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srbocontainer: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
