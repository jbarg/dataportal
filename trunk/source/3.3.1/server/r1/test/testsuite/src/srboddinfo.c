/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srboddinfo.c--
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
	const char* progname = "srboddinfo" ;
	int protocol ;
	int srbargc[SRB_USER_ARGS] ;
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC] ;
	int k ;
	int j ;
	int m ;
	char *ptr ;
	int ndirs ;
	int nfiles ;
	int bufsize ;
	int n ;
	char *host ;
	char *port ;
	char *auth ;
	char *objID ;
	char *rsrc ;
	char *coll ;
	char qval[MAX_DCS_NUM][MAX_TOKEN] ;
	int selval[MAX_DCS_NUM] ;
	mdasC_sql_result_struct someresult ;
	mdasC_sql_result_struct myresult[MAX_DCS_NUM] ;
	int status ;
	int results[MAX_DCS_NUM] ; /* # attributes returned per selval */
	char **selattv[MAX_DCS_NUM] ; /* selattv[k] has results[k] strings */
	char **seltabv[MAX_DCS_NUM] ; /* seltabv[k] has results[k] strings */
	int *nvalues[MAX_DCS_NUM] ; /* # values returned per attribute */
	char ***values[MAX_DCS_NUM] ; /* values[k][j] has nvalues[k][j] strs */


	for (k=0; k<SRB_USER_ARGS; k++)
		srbargc[k] = SRB_ARG_ILLEGAL ;
	srbargc[SRB_USER_ARG_HOST] = 1 ;
	srbargc[SRB_USER_ARG_PORT] = 1 ;
	srbargc[SRB_USER_ARG_AUTH] = 1 ;
	srbargc[SRB_USER_ARG_RSRC] = 1 ;
	srbargc[SRB_USER_ARG_ID] = SRB_ARG_REQUIRED ;
	srbargc[SRB_USER_ARG_COLL] = 1 ;
	srbargc[SRB_USER_ARG_VERB] = SRB_ARG_OPTIONAL ;
	srbUserArgs(progname, argc, argv, srbargc, srbargv) ;
	if (srbargc[SRB_USER_ARG_HELP] != 0)
		exit(0) ;
	if (srbargc[SRB_USER_ARG_ERR] != 0)
	{
		fprintf(stderr, "srboddinfo: aborting on srbUserArgs error\n");
		exit(42) ;
	}
	host = (char *) srbargv[SRB_USER_ARG_HOST][0] ;
	port = (char *) srbargv[SRB_USER_ARG_PORT][0] ;
	auth = (char *) srbargv[SRB_USER_ARG_AUTH][0] ;
	rsrc = (char *) srbargv[SRB_USER_ARG_RSRC][0] ;
	coll = (char *) srbargv[SRB_USER_ARG_COLL][0] ;


	/* set up the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboddinfo : trying clConnect with\n\t host = %s\n\t port = %s\n\t auth = %s\n", host, port, auth) ;
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
		fprintf(stderr, "srboddinfo: connected.\n\n") ;


	/* process the files in "-id" */

	nfiles = srbargc[SRB_USER_ARG_ID] ;

	for (n=0 ; n<nfiles; n++)
	{
		objID = srbargv[SRB_USER_ARG_ID][n] ;

		/* get info */
		for (k=0 ; k<MAX_DCS_NUM ; k++)
			qval[k][0] = '\0' ;
		sprintf(qval[DATA_NAME]," = '%s'", objID);
		sprintf(qval[DATA_GRP_NAME]," = '%s'", coll);
		sprintf(qval[RSRC_NAME]," = '%s'", rsrc);
		for (k=0 ; k<MAX_DCS_NUM ; k++)
		{
			for (m=0; m<MAX_DCS_NUM; m++)
			{
				qval[m][0] = '\0' ;
				selval[m] = 0 ;
			}
			selval[k] = 1 ;

			if (srbargc[SRB_USER_ARG_VERB] != 0)
			{
				fprintf(stderr, "srboddinfo: Calling srbGetDataDirInfo with\n") ;
				for (m=0; m < MAX_DCS_NUM; m++)
				{
					if (qval[m][0] != '\0')
						fprintf(stderr, "qval[%d] = %s\n", m, qval[m]) ;
					if (selval[m] != 0)
						fprintf(stderr, "selval[%d] = %d\n", m, selval[m]) ;
				}
			}
			status = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &(myresult[k]), 200);
			if (status < 0)
			{
				if (status != MCAT_INQUIRE_ERROR) {
				    fprintf(stderr,"srboddinfo: srbGetDataDirInfo failed (status = %d)\n", status);
				    fprintf(stderr,"%s",clErrorMessage(conn));
				    clFinish(conn);
				    exit (1);
				} else {
				    fprintf(stderr,"Query has no answer\n");
                                    results[k] = 0;
				    continue;
				}
			}

			/* # attributes returned per selval */
			results[k] = myresult[k].result_count ;
			if (results[k] < 1)
			{
				fprintf(stderr,"srboddinfo: results[%d] (%d) from srbGetDataDirInfo < 1\n", k, results[k]);
				clFinish(conn);
				exit (1);
			}

			/* selattv[k] has results[k] strings */
			selattv[k] = (char**) malloc(results[k]*sizeof(char*)) ;
			if (selattv[k] == (char**) NULL)
			{
				fprintf(stderr,"srboddinfo: malloc failed for selattv[%d]\n", k);
				clFinish(conn);
				exit (1);
			}
			for (j=0; j<results[k] ; j++)
			{
				selattv[k][j] = (char*) malloc(MAX_DATA_SIZE*sizeof(char)) ;
				if (selattv[k][j] == (char*) NULL)
				{
					fprintf(stderr,"srboddinfo: malloc failed for selattv[%d][%d]\n", k, j);
					clFinish(conn);
					exit (1);
				}
				strcpy(selattv[k][j], myresult[k].sqlresult[j].att_name) ;
			}

			/* seltabv[k] has results[k] strings */
			seltabv[k] = (char**) malloc(results[k]*sizeof(char*)) ;
			if (seltabv[k] == (char**) NULL)
			{
				fprintf(stderr,"srboddinfo: malloc failed for seltabv[%d]\n", k);
				clFinish(conn);
				exit (1);
			}
			for (j=0; j<results[k] ; j++)
			{
				seltabv[k][j] = (char*) malloc(MAX_DATA_SIZE*sizeof(char)) ;
				if (seltabv[k][j] == (char*) NULL)
				{
					fprintf(stderr,"srboddinfo: malloc failed for seltabv[%d][%d]\n", k, j);
					clFinish(conn);
					exit (1);
				}
				strcpy(seltabv[k][j], myresult[k].sqlresult[j].tab_name) ;
			}

			/* nvalues[k][] # values returned per attribute */
			nvalues[k] = (int*) malloc(results[k]*sizeof(int)) ;
			if (nvalues[k] == (int*) NULL)
			{
				fprintf(stderr,"srboddinfo: malloc failed for nvalues[%d]\n", k);
				clFinish(conn);
				exit (1);
			}

			/* values[k][] array of values returned per attribute */
			values[k] = (char***) malloc(results[k]*sizeof(char**));
			if (values[k] == (char***) NULL)
			{
				fprintf(stderr,"srboddinfo: malloc failed for values[%d]\n", k);
				clFinish(conn);
				exit (1);
			}


			/* count and get the values */
			for (j=0; j<results[k]; j++)
			{
				nvalues[k][j] = 0 ;
				ptr = (char *) myresult[k].sqlresult[j].values ;
				if (ptr[0] != '\0')
				{
					nvalues[k][j] = myresult[k].row_count;
#ifdef foo
					/* values[k][j] has nvalues[k][j] strs */
					values[k][j] = (char**) malloc(nvalues[k][j]*sizeof(char*)) ;
					if (values[k][j] == (char**) NULL)
					{
						fprintf(stderr,"srboddinfo: malloc failed for values[%d][%d]\n", k, j);
						clFinish(conn);
						exit (1);
					}
					ptr = (char *) myresult[k].sqlresult[j].values ;
					for (m=0; m<nvalues[k][j]; m++)
					{
						values[k][j][m] = (char*) malloc(MAX_DATA_SIZE*sizeof(char)) ;
						if (values[k][j][m] == (char*) NULL)
						{
							fprintf(stderr,"srboddinfo: malloc failed for values[%d][%d][%d]\n", k, j, m);
							clFinish(conn);
							exit (1);
						}
						strcpy(values[k][j][m], ptr) ;
						ptr += MAX_DATA_SIZE ;
					}
#endif /* foo */
				}
			}
			/* MW clear myresult */
			clearSqlResult (&myresult[k]);

			/* added by Raja 23 Jan 2003 for closing oracle open cusor ***/
			if (myresult[k].continuation_index >= 0)
			  srbGetMoreRows(conn, MDAS_CATALOG,myresult[k].continuation_index,
					 &(myresult[k]), 0);
			/* added by Raja 23 Jan 2003 for closing oracle open cusor ***/

		}

		fprintf(stdout,"srbGetDataDirInfo for objID = %s :\n", objID) ;
		fprintf(stdout,"%-24s%-24s%s\n", "att_name", "tab_name", "value(s)");
		fprintf(stdout,"%-24s%-24s%s\n", "----------------", "----------------", "----------------" );

		for (k=0 ; k<MAX_DCS_NUM ; k++)
		{
			for (j=0; j<results[k]; j++)
			{
				fprintf(stdout,"%-24s%-24s\n", selattv[k][j], seltabv[k][j]) ;
#ifdef foo
				for (m=0; m<nvalues[k][j]; m++)
					fprintf(stdout, "%48s%s\n", "", values[k][j][m]);
#endif
			}
		}
		fprintf(stdout,"\n");

	}


	/* close the connection */
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboddinfo: closing connection ...\n") ;
	clFinish(conn);
	if (srbargc[SRB_USER_ARG_VERB] != 0)
		fprintf(stderr, "srboddinfo: finished.\n\n") ;

	srbFreeArgs(srbargc, srbargv) ;

	exit(0);
}
