/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * srbUserArgs.c
 *
 *-------------------------------------------------------------------------
 */

#include "srbUserArgs.h"

void srbUserArgs(
	const char *token,
	int argc,
	char *argv[],
	int srbargc[SRB_USER_ARGS],
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC]
	)
{
	int k ;
	int n ;
	int j ;
	int m ;
	int jj,kk;
	int found ;
	int optcount ;
	int len ;
	int argnum[SRB_USER_ARGS] ;
	int argindex[SRB_USER_ARGS] ;
	int argset[SRB_USER_ARGS] ;
	int argtype[SRB_USER_ARGS] = SRB_ARG_TYPEV ;
	int cattnum[SRB_CATT_COUNT] = SRB_CATT_NUMS ;
	int dbmsnum[SRB_DBMS_COUNT] = SRB_DBMS_NUMS ;
	int systnum[SRB_SYST_COUNT] = SRB_SYST_NUMS ;
	int opnum[SRB_OP_COUNT] = SRB_OP_NUMS ;
	char *argname[SRB_USER_ARGS] = SRB_USER_ARGV ;
	char *usagestr[SRB_USER_ARGS] = SRB_USAGE_ARGV ;
	char *typename[SRB_ARG_TYPES] = SRB_ARG_TYPENAME ;
	char *cattname[SRB_CATT_COUNT] = SRB_CATT_NAMES ;
	char *dbmsname[SRB_DBMS_COUNT] = SRB_DBMS_NAMES ;
	char *systname[SRB_SYST_COUNT] = SRB_SYST_NAMES ;
	char *opname[SRB_OP_COUNT] = SRB_OP_NAMES ;
	int *argloc ;
	char *p ;
	char *tmpStr;


	/* initialize argloc array */

	argloc = (int*) malloc(sizeof(int)*argc) ;
	if (argloc == (int*) NULL)
	{
		fprintf(stderr, "srbUserArgs : malloc failed for argloc\n") ;
		srbargc[SRB_USER_ARG_ERR] = -1 ;
		return ;
	}

	argloc[0] = -1 ;
	for (k=1 ; k<argc ; k++)
	{
		argloc[k] = -1 ;
		for (n=0 ; n<SRB_USER_ARGS ; n++)
		{
			if (strcmp(argv[k], argname[n]) == 0)
			{
				argloc[k] = n ;
				break ;
			}
		}
	}


	/* check for multiple occurrences of arguments */

	for (k=1 ; k<(argc-1) ; k++)
	{
		if (argloc[k] != -1)
		{
			for (j=k+1 ; j<argc ; j++)
				if (argloc[k] == argloc[j])
				{
					fprintf(stderr, "srbUserArgs : multiple occurrences of arguments (%s) are not permitted\n", argname[argloc[k]]) ;
					fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
					free(argloc) ;
					srbargc[SRB_USER_ARG_ERR] = -1 ;
					return ;
				}
		}
	}


	/* initialize argnum, argset, and argindex arrays */

	for (n=0 ; n<SRB_USER_ARGS ; n++)
	{
		argnum[n] = 0 ;
		argset[n] = 0 ;
		argindex[n] = 0 ;
	}

	for (k=1 ; k<argc ; k++)
	{
		if (argloc[k] != -1)
		{
			n = argloc[k] ;
			argindex[n] = k ;
			j = k + 1 ;
			while (j<argc)
			{
				if (argloc[j] == -1)
				{
					argnum[n] += 1 ;
					j += 1 ;
				}
				else
					break ;
			}
		}
	}


	/* free argloc array */

	free(argloc) ;


	/* check for -verbose flag */

	if (argindex[SRB_USER_ARG_VERB] != 0)
	{
		fprintf(stderr, "%s : Verbose mode on.\n", token) ;
		fprintf(stderr, "Found:\n") ;
		for (n=0 ; n<SRB_USER_ARGS ; n++) if (argindex[n] != 0) fprintf(stderr, "\t%s (p%d, a%d)\n", argname[n], argindex[n], argnum[n]) ;
	}


	/* check for -help flag */

	if (argindex[SRB_USER_ARG_HELP] != 0)
	{
		fprintf(stderr, "Usage.\n") ;
		fprintf(stderr, "  %s\n", argv[0]) ;
		for (n=0 ; n<SRB_USER_ARGS ; n++)
		{
			if (srbargc[n] != SRB_ARG_ILLEGAL)
			{
				fprintf(stderr, "    %-40s%-13s", usagestr[n], typename[argtype[n]]) ;
				if (srbargc[n] == SRB_ARG_REQUIRED)
					fprintf(stderr, "(Required)\n") ;
				else if (srbargc[n] == SRB_ARG_OPTIONAL)
					fprintf(stderr, "(Optional)\n") ;
				else if (srbargc[n] > 0)
					fprintf(stderr, "(Requires %d values)\n", srbargc[n]) ;
				else
				{
					fprintf(stderr, "srbUserArgs : Program error.  Unknown argument type (%d).\n", srbargc[n]) ;
					srbargc[SRB_USER_ARG_ERR] = -1 ;
					return ;
				}
			}
		}
		fprintf(stderr, "Up to %d values permitted for arguments allowing multiple values.\n", SRB_USERARG_MAXC) ;
		if (srbargc[SRB_USER_ARG_CATT] != SRB_ARG_ILLEGAL)
		{
			fprintf(stderr, "Possible \"%s\" values:\n  ", argname[SRB_USER_ARG_CATT]) ;
			for (m=0; m<SRB_CATT_COUNT; m++)
				fprintf(stderr, "  %s", cattname[m]) ;
			fprintf(stderr, "\n") ;
		}
		if (srbargc[SRB_USER_ARG_DBMS] != SRB_ARG_ILLEGAL)
		{
			fprintf(stderr, "Possible \"%s\" values:\n  ", argname[SRB_USER_ARG_DBMS]) ;
			for (m=0; m<SRB_DBMS_COUNT; m++)
				fprintf(stderr, "  %s", dbmsname[m]) ;
			fprintf(stderr, "\n") ;
		}
		if (srbargc[SRB_USER_ARG_OP] != SRB_ARG_ILLEGAL)
		{
			fprintf(stderr, "Possible \"%s\" values:\n  ", argname[SRB_USER_ARG_OP]) ;
			for (m=0; m<SRB_OP_COUNT; m++)
				fprintf(stderr, "  %s", opname[m]) ;
			fprintf(stderr, "\n") ;
		}
		if (srbargc[SRB_USER_ARG_SYST] != SRB_ARG_ILLEGAL)
		{
			fprintf(stderr, "Possible \"%s\" values:\n  ", argname[SRB_USER_ARG_SYST]) ;
			for (m=0; m<SRB_SYST_COUNT; m++)
				fprintf(stderr, "  %s", systname[m]) ;
			fprintf(stderr, "\n") ;
		}
		srbargc[SRB_USER_ARG_HELP] = -1 ;
		return ;
	}


	/* check for illegal arguments */

	for (n=0 ; n<SRB_USER_ARGS ; n++)
	{
		if ((srbargc[n] == SRB_ARG_ILLEGAL) && (argindex[n] != 0))
		{
			fprintf(stderr, "%s : argument \"%s\" not permitted\n", token, argname[n]) ;
			fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
			srbargc[SRB_USER_ARG_ERR] = -1 ;
			return ;
		}
	}


	/* check for missing arguments */

	for (n=0 ; n<SRB_USER_ARGS ; n++)
	{
		if (((srbargc[n] == SRB_ARG_REQUIRED) || (srbargc[n] > 0))
		    &&
		    (argindex[n] == 0))
		{
			fprintf(stderr, "%s : ? argument \"%s\" is required\n", token, argname[n]) ;
			fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
			srbargc[SRB_USER_ARG_ERR] = -1 ;
			return ;
		}
	}


	/* check number of values supplied per argument */

	for (n=0 ; n<SRB_USER_ARGS ; n++)
	{
		if (argindex[n] != 0)
		{
			if (argnum[n] > SRB_USERARG_MAXC)
			{
				fprintf(stderr, "%s : ? number of values for argument \"%s\" (%d) exceeds maximum (%d)\n", token, argname[n], argnum[n], SRB_USERARG_MAXC) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
			if ((argtype[n] == SRB_ARG_TYPE_NULL) && (argnum[n] != 0))
			{
				fprintf(stderr, "%s : ? too many values for argument \"%s\"\n", token, argname[n]) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
			else if ((argtype[n] != SRB_ARG_TYPE_NULL) && (argnum[n] == 0))
			{
				fprintf(stderr, "%s : ? not enough values for argument \"%s\"\n", token, argname[n]) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
			else if ((srbargc[n] == 1) && (srbargc[n] != argnum[n]))
			{
			  /**** added by Raja Sept 30,99 to take care of
			        strings with blanks****/
			}
			else if ((0 < srbargc[n]) && (srbargc[n] != argnum[n]))
			{
				fprintf(stderr, "%s : ? argument \"%s\" requires %d values\n", token, argname[n], srbargc[n] ) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
		}

	}


	/* check '-from' requirements */

	if (argindex[SRB_USER_ARG_FROM] != 0)
	{
		optcount = 0 ;
		if (argindex[SRB_USER_ARG_FILE] != 0) optcount += 1 ;
		if (argindex[SRB_USER_ARG_DIR] != 0) optcount += 1 ;
		if (argindex[SRB_USER_ARG_OBJ] != 0) optcount += 1 ;
		if (argindex[SRB_USER_ARG_TABLE] != 0) optcount += 1 ;
		if (optcount > 1)
		{
			fprintf(stderr, "%s : Erroneous usage. Only one of \"%s\", \"%s\", \"%s\", or \"%s\" may be used with \"%s\".\n", token, argname[SRB_USER_ARG_FILE], argname[SRB_USER_ARG_DIR], argname[SRB_USER_ARG_OBJ], argname[SRB_USER_ARG_TABLE], argname[SRB_USER_ARG_FROM]) ;
			fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
			srbargc[SRB_USER_ARG_ERR] = -1 ;
			return ;
		}
		if (argindex[SRB_USER_ARG_FILE] != 0)
		{
			if (argnum[SRB_USER_ARG_FROM] != argnum[SRB_USER_ARG_FILE])
			{
				fprintf(stderr, "%s : Erroneous usage. The number of values for \"%s\" (%d) must match the number for \"%s\" (%d)\n", token, argname[SRB_USER_ARG_FILE], argnum[SRB_USER_ARG_FILE], argname[SRB_USER_ARG_FROM], argnum[SRB_USER_ARG_FROM] ) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
			for (j=0; j<argnum[SRB_USER_ARG_FILE]; j++)
			{
				k = argindex[SRB_USER_ARG_FILE] + j + 1 ;
				srbargv[SRB_USER_ARG_FILE][j] = (char*) malloc(sizeof(char)*(strlen(argv[k])+1)) ;
				if (srbargv[SRB_USER_ARG_FILE][j] == (char*) NULL)
				{
					fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_FILE][%d]\n", j) ;
					srbargc[SRB_USER_ARG_ERR] = -1 ;
					return ;
				}
				strcpy(srbargv[SRB_USER_ARG_FILE][j], argv[k]) ;
			}
			argset[SRB_USER_ARG_FILE] = argnum[SRB_USER_ARG_FILE] ;
		}
		if (argindex[SRB_USER_ARG_DIR] != 0)
		{
			if ((argnum[SRB_USER_ARG_FROM] != argnum[SRB_USER_ARG_DIR])
			    &&
			    (argnum[SRB_USER_ARG_DIR] != 1))
			{
				fprintf(stderr, "%s : Erroneous usage. The number of values for \"%s\" (%d) must either be 1 or match the number for \"%s\" (%d)\n", token, argname[SRB_USER_ARG_DIR], argnum[SRB_USER_ARG_DIR], argname[SRB_USER_ARG_FROM], argnum[SRB_USER_ARG_FROM] ) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
			if ((argnum[SRB_USER_ARG_FROM] != 1) &&
			         (argnum[SRB_USER_ARG_DIR] == 1))
			{
				k = argindex[SRB_USER_ARG_DIR] + 1 ;
				len = strlen(argv[k]) + 1 ;
				for (j=0; j<argnum[SRB_USER_ARG_FROM]; j++)
				{
					srbargv[SRB_USER_ARG_DIR][j] = (char*) malloc(sizeof(char)*len) ;
					if (srbargv[SRB_USER_ARG_DIR][j] == (char*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_DIR][%d] copy of -dir\n", j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					strcpy(srbargv[SRB_USER_ARG_DIR][j], argv[k]) ;
				}
				argset[SRB_USER_ARG_DIR] = argnum[SRB_USER_ARG_FROM] ;
			}
			else
			{
				for (j=0; j<argnum[SRB_USER_ARG_FROM]; j++)
				{
					k = argindex[SRB_USER_ARG_DIR] + j + 1 ;
					srbargv[SRB_USER_ARG_DIR][j] = (char*) malloc(sizeof(char)*(strlen(argv[k])+1)) ;
					if (srbargv[SRB_USER_ARG_DIR][j] == (char*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_DIR][%d]\n", j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					strcpy(srbargv[SRB_USER_ARG_DIR][j], argv[k]) ;
				}
				argset[SRB_USER_ARG_DIR] = argnum[SRB_USER_ARG_FROM] ;
			}
		}
		if ((srbargc[SRB_USER_ARG_DIR] == SRB_ARG_OPTIONAL)
		    &&
		    (srbargc[SRB_USER_ARG_FILE] == SRB_ARG_OPTIONAL)
		    &&
		    (argindex[SRB_USER_ARG_DIR] == 0)
		    &&
		    (argindex[SRB_USER_ARG_FILE] == 0))
		{
			for (j=0; j<argnum[SRB_USER_ARG_FROM]; j++)
			{
				srbargv[SRB_USER_ARG_DIR][j] = (char*) malloc(sizeof(char)*2) ;
				if (srbargv[SRB_USER_ARG_DIR][j] == (char*) NULL)
				{
					fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_DIR][%d] = \".\"\n", j) ;
					srbargc[SRB_USER_ARG_ERR] = -1 ;
					return ;
				}
				strcpy(srbargv[SRB_USER_ARG_DIR][j], ".") ;
			}
			argset[SRB_USER_ARG_DIR] = argnum[SRB_USER_ARG_FROM] ;
		}
		if ((argset[SRB_USER_ARG_DIR] != 0) && (argset[SRB_USER_ARG_FILE] == 0))
		{
			for (j=0; j<argnum[SRB_USER_ARG_FROM]; j++)
			{
				k = argindex[SRB_USER_ARG_FROM] + j + 1 ;
				p = argv[k] + strlen(argv[k]) - 1 ;
				while ((p > argv[k]) && (p[0] != '/'))
					p -= 1 ;
				if (p[0] == '/')
					p += 1 ;
				len = strlen(p) + strlen(srbargv[SRB_USER_ARG_DIR][j]) + 2 ;
				srbargv[SRB_USER_ARG_FILE][j] = (char*) malloc(sizeof(char)*len) ;
				if (srbargv[SRB_USER_ARG_FILE][j] == (char*) NULL)
				{
					fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_FILE][%d] fusion\n", j) ;
					srbargc[SRB_USER_ARG_ERR] = -1 ;
					return ;
				}
				strcpy(srbargv[SRB_USER_ARG_FILE][j], srbargv[SRB_USER_ARG_DIR][j]) ;
				strcat(srbargv[SRB_USER_ARG_FILE][j], "/") ;
				strcat(srbargv[SRB_USER_ARG_FILE][j], p) ;
			}
			argset[SRB_USER_ARG_FILE] = argnum[SRB_USER_ARG_FROM] ;
		}
		if (argindex[SRB_USER_ARG_OBJ] != 0)
		{
			if (argnum[SRB_USER_ARG_FROM] != argnum[SRB_USER_ARG_OBJ])
			{
				fprintf(stderr, "%s : Erroneous usage. The number of values for \"%s\" (%d) must match the number for \"%s\" (%d)\n", token, argname[SRB_USER_ARG_OBJ], argnum[SRB_USER_ARG_OBJ], argname[SRB_USER_ARG_FROM], argnum[SRB_USER_ARG_FROM] ) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
			if (argnum[SRB_USER_ARG_FROM] != argnum[SRB_USER_ARG_ID])
			{
				fprintf(stderr, "%s : Erroneous usage. The number of values for \"%s\" (%d) must match the number for \"%s\" (%d)\n", token, argname[SRB_USER_ARG_ID], argnum[SRB_USER_ARG_ID], argname[SRB_USER_ARG_FROM], argnum[SRB_USER_ARG_FROM] ) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
			for (j=0; j<argnum[SRB_USER_ARG_OBJ]; j++)
			{
				k = argindex[SRB_USER_ARG_OBJ] + j + 1 ;
				srbargv[SRB_USER_ARG_OBJ][j] = (char*) malloc(sizeof(char)*(strlen(argv[k])+1)) ;
				if (srbargv[SRB_USER_ARG_OBJ][j] == (char*) NULL)
				{
					fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_OBJ][%d]\n", j) ;
					srbargc[SRB_USER_ARG_ERR] = -1 ;
					return ;
				}
				strcpy(srbargv[SRB_USER_ARG_OBJ][j], argv[k]) ;
			}
			argset[SRB_USER_ARG_OBJ] = argnum[SRB_USER_ARG_OBJ] ;
		}
		if ((srbargc[SRB_USER_ARG_OBJ] == SRB_ARG_OPTIONAL)
		    &&
		    (argindex[SRB_USER_ARG_OBJ] == 0))
		{
			for (j=0; j<argnum[SRB_USER_ARG_FROM]; j++)
			{
				k = argindex[SRB_USER_ARG_FROM] + j + 1 ;
				p = argv[k] + strlen(argv[k]) - 1 ;
				while ((p > argv[k]) && (p[0] != '/'))
					p -= 1 ;
				if (p[0] == '/')
					p += 1 ;
				len = strlen(p) + 1 ;
				srbargv[SRB_USER_ARG_OBJ][j] = (char*) malloc(sizeof(char)*len) ;
				if (srbargv[SRB_USER_ARG_OBJ][j] == (char*) NULL)
				{
					fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_OBJ][%d] fusion\n", j) ;
					srbargc[SRB_USER_ARG_ERR] = -1 ;
					return ;
				}
				strcpy(srbargv[SRB_USER_ARG_OBJ][j], p) ;
			}
			argset[SRB_USER_ARG_OBJ] = argnum[SRB_USER_ARG_FROM] ;
		}
		if (argindex[SRB_USER_ARG_TABLE] != 0)
		{
			if (argnum[SRB_USER_ARG_FROM] != argnum[SRB_USER_ARG_TABLE])
			{
				fprintf(stderr, "%s : Erroneous usage. The number of values for \"%s\" (%d) must match the number for \"%s\" (%d)\n", token, argname[SRB_USER_ARG_TABLE], argnum[SRB_USER_ARG_TABLE], argname[SRB_USER_ARG_FROM], argnum[SRB_USER_ARG_FROM] ) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
			for (j=0; j<argnum[SRB_USER_ARG_TABLE]; j++)
			{
				k = argindex[SRB_USER_ARG_TABLE] + j + 1 ;
				srbargv[SRB_USER_ARG_TABLE][j] = (char*) malloc(sizeof(char)*(strlen(argv[k])+1)) ;
				if (srbargv[SRB_USER_ARG_TABLE][j] == (char*) NULL)
				{
					fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_TABLE][%d]\n", j) ;
					srbargc[SRB_USER_ARG_ERR] = -1 ;
					return ;
				}
				strcpy(srbargv[SRB_USER_ARG_TABLE][j], argv[k]) ;
			}
			argset[SRB_USER_ARG_TABLE] = argnum[SRB_USER_ARG_TABLE] ;
		}
		if ((srbargc[SRB_USER_ARG_TABLE] == SRB_ARG_OPTIONAL)
		    &&
		    (argindex[SRB_USER_ARG_TABLE] == 0))
		{
			for (j=0; j<argnum[SRB_USER_ARG_FROM]; j++)
			{
				k = argindex[SRB_USER_ARG_FROM] + j + 1 ;
				p = argv[k] + strlen(argv[k]) - 1 ;
				while ((p > argv[k]) && (p[0] != '/'))
					p -= 1 ;
				if (p[0] == '/')
					p += 1 ;
				len = strlen(p) + 1 ;
				srbargv[SRB_USER_ARG_TABLE][j] = (char*) malloc(sizeof(char)*len) ;
				if (srbargv[SRB_USER_ARG_TABLE][j] == (char*) NULL)
				{
					fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_TABLE][%d] fusion\n", j) ;
					srbargc[SRB_USER_ARG_ERR] = -1 ;
					return ;
				}
				strcpy(srbargv[SRB_USER_ARG_TABLE][j], p) ;
			}
			argset[SRB_USER_ARG_TABLE] = argnum[SRB_USER_ARG_FROM] ;
		}
	}


	/* check '-to' requirements */

	if (srbargc[SRB_USER_ARG_TO] != SRB_ARG_ILLEGAL)
	{
		optcount = 0 ;
		if (argindex[SRB_USER_ARG_FILE] != 0) optcount += 1 ;
		if (argindex[SRB_USER_ARG_DIR] != 0) optcount += 1 ;
		if (argindex[SRB_USER_ARG_OBJ] != 0) optcount += 1 ;
		if (argindex[SRB_USER_ARG_TABLE] != 0) optcount += 1 ;
		if (optcount > 1)
		{
			fprintf(stderr, "%s : Erroneous usage. Only one of \"%s\", \"%s\", \"%s\", or \"%s\" may be used when \"%s\" is valid.\n", token, argname[SRB_USER_ARG_FILE], argname[SRB_USER_ARG_DIR], argname[SRB_USER_ARG_OBJ], argname[SRB_USER_ARG_TABLE], argname[SRB_USER_ARG_TO]) ;
			fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
			srbargc[SRB_USER_ARG_ERR] = -1 ;
			return ;
		}
		if (argindex[SRB_USER_ARG_FILE] != 0)
		{
			if ((argnum[SRB_USER_ARG_TO] != 0)
			    && (argnum[SRB_USER_ARG_TO] != argnum[SRB_USER_ARG_FILE]))
			{
				fprintf(stderr, "%s : Erroneous usage. The number of values for \"%s\" (%d) must match the number for \"%s\" (%d)\n", token, argname[SRB_USER_ARG_FILE], argnum[SRB_USER_ARG_FILE], argname[SRB_USER_ARG_TO], argnum[SRB_USER_ARG_TO] ) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
			if (argnum[SRB_USER_ARG_TO] == 0)
			{
				for (j=0; j<argnum[SRB_USER_ARG_FILE]; j++)
				{
					k = argindex[SRB_USER_ARG_FILE] + j + 1 ;
					p = argv[k] + strlen(argv[k]) - 1 ;
					while ((p > argv[k]) && (p[0] != '/'))
						p -= 1 ;
					if (p[0] == '/')
						p += 1 ;
					len = strlen(p) + 1 ;
					srbargv[SRB_USER_ARG_TO][j] = (char*) malloc(sizeof(char)*len) ;
					if (srbargv[SRB_USER_ARG_TO][j] == (char*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_TO][%d]\n", j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					strcpy(srbargv[SRB_USER_ARG_TO][j], p) ;
				}
				argset[SRB_USER_ARG_TO] = argnum[SRB_USER_ARG_FILE] ;
			}
		}
		if (argindex[SRB_USER_ARG_DIR] != 0)
		{
			if ((argnum[SRB_USER_ARG_TO] != 0)
			    &&
		            (argnum[SRB_USER_ARG_TO] != argnum[SRB_USER_ARG_DIR])
			    &&
			    (argnum[SRB_USER_ARG_TO] != 1))
			{
				fprintf(stderr, "%s : Erroneous usage. The number of values for \"%s\" (%d) must either be 1 or match the number for \"%s\" (%d)\n", token, argname[SRB_USER_ARG_TO], argnum[SRB_USER_ARG_TO], argname[SRB_USER_ARG_DIR], argnum[SRB_USER_ARG_DIR] ) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
			if ((argnum[SRB_USER_ARG_DIR] != 1) &&
			         (argnum[SRB_USER_ARG_TO] == 1))
			{
				k = argindex[SRB_USER_ARG_TO] + 1 ;
				len = strlen(argv[k]) + 1 ;
				for (j=0; j<argnum[SRB_USER_ARG_DIR]; j++)
				{
					srbargv[SRB_USER_ARG_TO][j] = (char*) malloc(sizeof(char)*len) ;
					if (srbargv[SRB_USER_ARG_TO][j] == (char*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_TO][%d] copy of -dir\n", j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					strcpy(srbargv[SRB_USER_ARG_TO][j], argv[k]) ;
				}
				argset[SRB_USER_ARG_TO] = argnum[SRB_USER_ARG_DIR] ;
			}
			else if (argnum[SRB_USER_ARG_TO] == 0)
			{
				for (j=0; j<argnum[SRB_USER_ARG_DIR]; j++)
				{
					k = argindex[SRB_USER_ARG_DIR] + j + 1 ;
					p = argv[k] + strlen(argv[k]) - 2 ;
					while ((p > argv[k]) && (p[0] != '/'))
						p -= 1 ;
					if (p[0] == '/')
						p += 1 ;
					len = strlen(p) + 1 ;
					srbargv[SRB_USER_ARG_TO][j] = (char*) malloc(sizeof(char)*len) ;
					if (srbargv[SRB_USER_ARG_TO][j] == (char*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_TO][%d]\n", j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					strcpy(srbargv[SRB_USER_ARG_TO][j], p) ;
				}
				argset[SRB_USER_ARG_TO] = argnum[SRB_USER_ARG_DIR] ;
			}
		}
		if (argindex[SRB_USER_ARG_OBJ] != 0)
		{
			if ((argnum[SRB_USER_ARG_TO] != 0)
			    && (argnum[SRB_USER_ARG_TO] != argnum[SRB_USER_ARG_OBJ]))
			{
				fprintf(stderr, "%s : Erroneous usage. The number of values for \"%s\" (%d) must match the number for \"%s\" (%d)\n", token, argname[SRB_USER_ARG_OBJ], argnum[SRB_USER_ARG_OBJ], argname[SRB_USER_ARG_TO], argnum[SRB_USER_ARG_TO] ) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
			if (argnum[SRB_USER_ARG_TO] == 0)
			{
				for (j=0; j<argnum[SRB_USER_ARG_OBJ]; j++)
				{
					k = argindex[SRB_USER_ARG_OBJ] + j + 1 ;
					len = strlen(argv[k]) + 1 ;
					srbargv[SRB_USER_ARG_TO][j] = (char*) malloc(sizeof(char)*len) ;
					if (srbargv[SRB_USER_ARG_TO][j] == (char*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_TO][%d]\n", j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					strcpy(srbargv[SRB_USER_ARG_TO][j], argv[k]) ;
				}
				argset[SRB_USER_ARG_TO] = argnum[SRB_USER_ARG_OBJ] ;
			}
		}
		if (argindex[SRB_USER_ARG_TABLE] != 0)
		{
			if ((argnum[SRB_USER_ARG_TO] != 0)
			    && (argnum[SRB_USER_ARG_TO] != argnum[SRB_USER_ARG_TABLE]))
			{
				fprintf(stderr, "%s : Erroneous usage. The number of values for \"%s\" (%d) must match the number for \"%s\" (%d)\n", token, argname[SRB_USER_ARG_TABLE], argnum[SRB_USER_ARG_TABLE], argname[SRB_USER_ARG_TO], argnum[SRB_USER_ARG_TO] ) ;
				fprintf(stderr, "For more help, try\n\t%s -help\n", argv[0]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
			if (argnum[SRB_USER_ARG_TO] == 0)
			{
				for (j=0; j<argnum[SRB_USER_ARG_TABLE]; j++)
				{
					k = argindex[SRB_USER_ARG_TABLE] + j + 1 ;
					len = strlen(argv[k]) + 1 ;
					srbargv[SRB_USER_ARG_TO][j] = (char*) malloc(sizeof(char)*len) ;
					if (srbargv[SRB_USER_ARG_TO][j] == (char*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_TO][%d]\n", j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					strcpy(srbargv[SRB_USER_ARG_TO][j], argv[k]) ;
				}
				argset[SRB_USER_ARG_TO] = argnum[SRB_USER_ARG_TABLE] ;
			}
		}
	}


	/* check '-bufsize' default initialization */

	if (argindex[SRB_USER_ARG_BSIZE] == 0)
	{
		srbargv[SRB_USER_ARG_BSIZE][0] = (int*) malloc(sizeof(int)) ;
		if (srbargv[SRB_USER_ARG_BSIZE][0] == (int*) NULL)
		{
			fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_BSIZE][0]\n") ;
			srbargc[SRB_USER_ARG_ERR] = -1 ;
			return ;
		}
		*((int*) srbargv[SRB_USER_ARG_BSIZE][0]) = SRB_BSIZE_DFLT ;
		argset[SRB_USER_ARG_BSIZE] = 1 ;
	}


	/* special handling for '-cattype' */

	if (argindex[SRB_USER_ARG_CATT] != 0)
	{
		for (j=0; j<argnum[SRB_USER_ARG_CATT]; j++)
		{
			k = argindex[SRB_USER_ARG_CATT] + j + 1 ;
			found = 0 ;
			for (m=0; m<SRB_CATT_COUNT; m++)
			{
				if (strcmp(cattname[m], argv[k]) == 0)
				{
					srbargv[SRB_USER_ARG_CATT][j] = (int*) malloc(sizeof(int)) ;
					if (srbargv[SRB_USER_ARG_CATT][j] == (int*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_CATT][%d]\n", j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					*((int*) srbargv[SRB_USER_ARG_CATT][j]) = cattnum[m] ;
					argset[SRB_USER_ARG_CATT] += 1 ;
					found = 1 ;
					break ;
				}
			}
			if (found == 0)
			{
				fprintf(stderr, "srbUserArgs : Unknown \"%s\" value \"%s\"\n", argname[SRB_USER_ARG_CATT], argv[k]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
		}
	}


	/* special handling for '-dbms' */

	if (argindex[SRB_USER_ARG_DBMS] != 0)
	{
		for (j=0; j<argnum[SRB_USER_ARG_DBMS]; j++)
		{
			k = argindex[SRB_USER_ARG_DBMS] + j + 1 ;
			found = 0 ;
			for (m=0; m<SRB_DBMS_COUNT; m++)
			{
				if (strcmp(dbmsname[m], argv[k]) == 0)
				{
					srbargv[SRB_USER_ARG_DBMS][j] = (int*) malloc(sizeof(int)) ;
					if (srbargv[SRB_USER_ARG_DBMS][j] == (int*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_DBMS][%d]\n", j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					*((int*) srbargv[SRB_USER_ARG_DBMS][j]) = dbmsnum[m] ;
					argset[SRB_USER_ARG_DBMS] += 1 ;
					found = 1 ;
					break ;
				}
			}
			if (found == 0)
			{
				fprintf(stderr, "srbUserArgs : Unknown \"%s\" value \"%s\"\n", argname[SRB_USER_ARG_DBMS], argv[k]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
		}
	}


	/* special handling for '-op' */

	if (argindex[SRB_USER_ARG_OP] != 0)
	{
		for (j=0; j<argnum[SRB_USER_ARG_OP]; j++)
		{
			k = argindex[SRB_USER_ARG_OP] + j + 1 ;
			found = 0 ;
			for (m=0; m<SRB_OP_COUNT; m++)
			{
				if (strcmp(opname[m], argv[k]) == 0)
				{
					srbargv[SRB_USER_ARG_OP][j] = (int*) malloc(sizeof(int)) ;
					if (srbargv[SRB_USER_ARG_OP][j] == (int*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_OP][%d]\n", j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					*((int*) srbargv[SRB_USER_ARG_OP][j]) = opnum[m] ;
					argset[SRB_USER_ARG_OP] += 1 ;
					found = 1 ;
					break ;
				}
			}
			if (found == 0)
			{
				fprintf(stderr, "srbUserArgs : Unknown \"%s\" value \"%s\"\n", argname[SRB_USER_ARG_OP], argv[k]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
		}
	}


	/* special handling for '-syst' */

	if (argindex[SRB_USER_ARG_SYST] != 0)
	{
		for (j=0; j<argnum[SRB_USER_ARG_SYST]; j++)
		{
			k = argindex[SRB_USER_ARG_SYST] + j + 1 ;
			found = 0 ;
			for (m=0; m<SRB_SYST_COUNT; m++)
			{
				if (strcmp(systname[m], argv[k]) == 0)
				{
					srbargv[SRB_USER_ARG_SYST][j] = (int*) malloc(sizeof(int)) ;
					if (srbargv[SRB_USER_ARG_SYST][j] == (int*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for srbargv[SRB_USER_ARG_SYST][%d]\n", j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					*((int*) srbargv[SRB_USER_ARG_SYST][j]) = systnum[m] ;
					argset[SRB_USER_ARG_SYST] += 1 ;
					found = 1 ;
					break ;
				}
			}
			if (found == 0)
			{
				fprintf(stderr, "srbUserArgs : Unknown \"%s\" value \"%s\"\n", argname[SRB_USER_ARG_SYST], argv[k]) ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
		}
	}


	/* check '-auth' default initialization */

	if (argindex[SRB_USER_ARG_AUTH] == 0)
	{
		srbargv[SRB_USER_ARG_AUTH][0] = (char*) NULL ;
		argset[SRB_USER_ARG_AUTH] = -1 ;
	}

	/* finish more or less automatic initialization */

	for (n=0 ; n<SRB_USER_ARGS ; n++)
	{
		if ((argset[n] == 0) && (argindex[n] != 0))
		{
			if (argtype[n] == SRB_ARG_TYPE_STRING)
			{
				for (j=0 ; j<argnum[n] ; j++)
				{
					k = argindex[n] + j + 1 ;
					len = strlen(argv[k]) + 1 ;
					srbargv[n][j] = (char*) malloc(sizeof(char)*len) ;
					if (srbargv[n][j] == (char*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for string srbargv[\"%s\"][%d]\n", argname[n], j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					strcpy(srbargv[n][j], argv[k]) ;
				}
				argset[n] = argnum[n] ;
			}
			else if (argtype[n] == SRB_ARG_TYPE_INT)
			{
				for (j=0 ; j<argnum[n] ; j++)
				{
					k = argindex[n] + j + 1 ;
					srbargv[n][j] = (int*) malloc(sizeof(int)) ;
					if (srbargv[n][j] == (int*) NULL)
					{
						fprintf(stderr, "srbUserArgs : malloc failed for int srbargv[\"%s\"][%d]\n", argname[n], j) ;
						srbargc[SRB_USER_ARG_ERR] = -1 ;
						return ;
					}
					sscanf(argv[k], "%i", ((int*) srbargv[n][j]) ) ;
				}
				argset[n] = argnum[n] ;
			}
			else if (argtype[n] == SRB_ARG_TYPE_NULL)
			{
				argset[n] = -1 ;
			}
			else
			{
				fprintf(stderr, "srbUserArgs : Program error.  Unknown argument type.\n") ;
				srbargc[SRB_USER_ARG_ERR] = -1 ;
				return ;
			}
		}
		/**** added by Raja Sept 30,99 to take care of
		      strings with blanks
		printf("EE:argname[%i] = %s :srbargc[%i] = %i : argnum[%i] = %i\n",
		       n,argname[n],n,srbargc[n],n,argnum[n]);
		****/

		if ((srbargc[n] == 1) && (srbargc[n] != argnum[n]))  {
		  jj = 0;
		  for (kk = 0; kk < argnum[n] ; kk++)
		    jj += strlen(srbargv[n][kk]) + 2;
		  tmpStr = (char *) malloc(jj);
		  strcat(tmpStr, srbargv[n][0]);
		  for (kk = 1; kk <  argnum[n] ; kk++) {
		    strcat(tmpStr, " ");
		    strcat(tmpStr,srbargv[n][kk]);
		  }
		  for (kk = 0; kk < argnum[n] ; kk++)
		    free(srbargv[n][kk]);
		  argset[n] = 1;
		  argnum[n] = 1;
		  srbargv[n][0] = tmpStr;
		}
		srbargc[n] = argset[n] ;	/* FINAL BINDING */
	}

	if (srbargc[SRB_USER_ARG_VERB] != 0)
	{
		fprintf(stderr, "Set:\n") ;
		for (n=0 ; n<SRB_USER_ARGS ; n++)
		{
			if (srbargc[n] != 0)
			{
				if ((srbargc[n] < 0) || (argtype[n] == SRB_ARG_TYPE_NULL))
					fprintf(stderr, "\t%s (void)\n", argname[n]) ;
				else if (argtype[n] == SRB_ARG_TYPE_STRING)
				{
					fprintf(stderr, "\t%s", argname[n]) ;
					for (j=0 ; j<srbargc[n] ; j++)
						fprintf(stderr, " %s", (char*) srbargv[n][j]) ;
					fprintf(stderr, "\n") ;
				}
				else if (argtype[n] == SRB_ARG_TYPE_INT)
				{
					fprintf(stderr, "\t%s", argname[n]) ;
					for (j=0 ; j<srbargc[n] ; j++)
						fprintf(stderr, " %i", *((int*) srbargv[n][j])) ;
					fprintf(stderr, "\n") ;
				}
				else
				{
					fprintf(stderr, "srbUserArgs : Program error.  Unknown argument type set.\n") ;
					srbargc[SRB_USER_ARG_ERR] = -1 ;
					return ;
				}
			}
		}
	}	


}


void srbFreeArgs(
	int srbargc[SRB_USER_ARGS],
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC]
	)
{
	int n ;
	int j ;

	if (srbargc[SRB_USER_ARG_ERR] != 0)
		return ;

	for (n=0 ; n<SRB_USER_ARGS ; n++)
		for (j=0 ; j<srbargc[n] ; j++)
			free(srbargv[n][j]) ;

}

