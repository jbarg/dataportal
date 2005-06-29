%{
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

extern int Lineno; 
int Vflag;
FILE *yyin;


/************************************************************/
/* The following constants are used in this program
   and are emitted to the target include file. */
#define SRB_ERR_NAMELEN   50
#define SRB_ERR_SHORTLEN  160
#define SRB_ERR_LONGLEN   512
#define SRB_ERR_ACTLEN    256
/************************************************************/
/* The first error ID assigned.  This value is incremented for
   each additional error. */
#define SRB_ERR_BASE -1000
/************************************************************/

/* The following structure simply holds the entries read from 
    the input file.  Upon parse completion, this table provides
    the information that is used to emit the SRB error include file */

typedef struct 
	{
	caddr_t next;
	int srb_err;
	char err_name[SRB_ERR_NAMELEN];
	char err_entry[SRB_ERR_NAMELEN + SRB_ERR_SHORTLEN +
					SRB_ERR_LONGLEN + SRB_ERR_ACTLEN + 50]; 
	} errent_tbl;

errent_tbl *Tbl_head;
int Errid;

%}

%union
{
	char string[200];
}

%start errs
%token <string> MNENOMIC
%token <string> QUOTESTR
%token <string> ERRID
%token S_LDELIM S_RDELIM S_COMMA 

%%
errs: errs errent {}
	| errent 
	;


errent: entry {}
	| errset {}
	;

errset:  S_LDELIM entry S_RDELIM {}
	| S_LDELIM errs S_RDELIM {}
	;


entry : S_LDELIM MNENOMIC S_COMMA QUOTESTR S_COMMA QUOTESTR S_COMMA QUOTESTR S_RDELIM 
			{ 
			AddErrTbl(Tbl_head, Errid--, $2, $4, $6, $8);  
			if ( Vflag == 1 )
				printf ("%s\n", $2); 
			}	
	| S_LDELIM ERRID S_COMMA MNENOMIC S_COMMA QUOTESTR S_COMMA QUOTESTR S_COMMA QUOTESTR S_RDELIM
			{ 
			AddErrTbl(Tbl_head, atoi($2), $4, $6, $8, $10);  
			if ( Vflag == 1 )
				printf ("%s\n", $4);
			}	
	  ;
	

%%

int yyerror(char *line)
	{
	fprintf (stderr, "Syntax error on line %d\n", Lineno);
	exit(1);
	}


/*--------------------------------------------------------------------------------
errent_tbl *InitErrTbl()

Description:
	InitErrTbl creates an SRB error table and stores the address in the 
	pointer refered to by <errent_tbl>.

Return Values:
	InitErrTbl returns a pointer to an errnet_tbl.  If an error occurs, NULL 
	is returned.
	
--------------------------------------------------------------------------------*/
errent_tbl *InitErrTbl()
	{
	errent_tbl *p;
	
	p = (errent_tbl *)malloc(sizeof(errent_tbl));
	p->srb_err = -1;
	return(p);
	}


/*--------------------------------------------------------------------------------
void AddErrTbl(errent_tbl *head, int srb_err, char *err_name, char *short_msg, 
					char *long_msg,ar *action)

Description:
	AddErrTbl cretes a new error table records and populates it with 
	information contained within the remaining parameters.  The new record 
	is then added to the end of the SRB error table referred to by <head>.
	
Input Paramters:
	<head>	Contains the address of the head of the errtbl chain.
	
	<srb_err>   Contains an integer representing the SRB error id to be 
				assigned to this error.

	<short_msg>	Contains a pointer to a string representing the SRB error
				short message.


	<long_msg>	Contains a pointer to a string representing the SRB error
				long message.


	<action>	Contains a pointer to a string representing the SRB error
				recommended action.


--------------------------------------------------------------------------------*/
void AddErrTbl(errent_tbl *head, int srb_err, char *err_name, char *short_msg, 
					char *long_msg, char *action)
	{
	errent_tbl *new, *p, *q;
	
	if ( head->srb_err == -1 )
		new = head;
	else
		new = InitErrTbl();

	new->srb_err = srb_err;
	strcpy (new->err_name, err_name);
	sprintf (new->err_entry, "\"%s\", %s, %s, %s", err_name, short_msg, long_msg, action); 
	
	for (p = head, q = head; p; q = p, p = (errent_tbl *)p->next);
	if ( head != new )
		q->next = (caddr_t)new;
	}

/*--------------------------------------------------------------------------------
void EmitErrTbl(errent_tbl *head, int fd)

Description:
	EmitErrTbl emits the SRB error table include file to the file descriptor referred
	to by <fd>.  The sections emitted are #defines for the SRB error symbols, then 
	the actual error table.

Input Parameters:
	<head>	Contains the address of the head of the errtbl chain.

	<fd>	Contains the file descritor to which the SRB error include is to 
			emitted to.
	
--------------------------------------------------------------------------------*/
void EmitErrTbl(errent_tbl *head, int fd)
	{
	errent_tbl *p;
	FILE *fp;
	unsigned long serno;   
	time_t stime;
	int count = 0;

		
	fp = fdopen(fd, "w");
	if ( fp == NULL )
		{
		perror("Fatal Error: Can't convert fd %d to file pointer.  Exiting\n");
		exit(1);
		}


	/* Emit header */
	fprintf (fp, "/* This file was automatically generated.  Do not edit */\n\n");
	fprintf (fp, "#ifndef SRB_ERRNO_H\n");
	fprintf (fp, "#define SRB_ERRNO_H\n");
        fprintf (fp, "#ifdef c_plus_plus\n");
        fprintf (fp, "extern \"C\"\n");
        fprintf (fp, "{\n");
        fprintf (fp, "#endif\n");
	fprintf (fp, "#define UNIX_ERR_START          (1)\n");
	fprintf (fp, "#define UNIX_ERR_END            (99)\n");
	fprintf (fp, "#define UNIX_ERR_PREFIX         (-1300)\n");
	fprintf (fp, "#define HPSS_ERR_START          (-87)\n");
	fprintf (fp, "#define HPSS_ERR_END            (-1)\n");
	fprintf (fp, "#define HPSS_ERR_PREFIX         (-1400)\n");
	fprintf (fp, "#define SRB_ERR_NAMELEN %d\n", SRB_ERR_NAMELEN);
	fprintf (fp, "#define SRB_ERR_SHORTLEN %d\n", SRB_ERR_SHORTLEN);
	fprintf (fp, "#define SRB_ERR_LONGLEN %d\n", SRB_ERR_LONGLEN);
	fprintf (fp, "#define SRB_ERR_ACTLEN %d\n", SRB_ERR_ACTLEN);

	fprintf (fp, "\n/* srb_perror() flags */\n");
	fprintf (fp, "#define SRB_LONG_MSG 1\n");
	fprintf (fp, "#define SRB_RCMD_ACTION 2\n");

        fprintf (fp, "struct srb_errtbl\n       {\n");
        fprintf (fp, "  int srb_err;\n");
        fprintf (fp, "  char srb_err_name[SRB_ERR_NAMELEN];\n");
        fprintf (fp, "  char srb_err_short[SRB_ERR_SHORTLEN];\n");
        fprintf (fp, "  char srb_err_long[SRB_ERR_LONGLEN];\n");
        fprintf (fp, "  char srb_err_action[SRB_ERR_ACTLEN];\n");
        fprintf (fp, "  };\n");



	fprintf (fp, "\n/* DEFINES SECTION */\n");
	/* Emit #define section */
	for (p = head; p; p = (errent_tbl *)(p -> next))
		{
		fprintf(fp, "#define %s %d\n", p->err_name, p->srb_err);
		count++;
		}
	
	
	/* Emit error table */ 
	fprintf (fp, "#if defined(SRB_ERR_TBL)\n");
	fprintf (fp, "extern int Srb_errtbl_serialno;\n");
	fprintf (fp, "extern int Srb_numents;\n");
	fprintf (fp, "extern struct srb_errtbl srb_errent[123];\n");
	fprintf (fp, "	 \n");

	fprintf (fp, "\n/* ERROR TABLE DECLARATIONS */\n");
	
	fprintf (fp, "#else\n");
	time(&stime);
	serno = (unsigned long)stime;
	fprintf (fp, "int Srb_errtbl_serialno = %lu;\n", stime);
	fprintf (fp, "int Srb_numents = %d;\n\n", count);
 	fprintf (fp, "struct srb_errtbl srb_errent[] =\n");	
	fprintf (fp, "	{\n");	
	for (p = head; p; p = (errent_tbl *)(p -> next))
		fprintf (fp, "		%d, %s,\n", p->srb_err, p->err_entry);
	fprintf (fp, "	};\n");

	fprintf (fp, "#endif /* SRB_ERR_TBL */\n");
        fprintf (fp, "#ifdef c_plus_plus\n");
        fprintf (fp, "};\n");
        fprintf (fp, "#endif\n");
	fprintf (fp, "#endif /* SRB_ERRNO_H */\n");

	}

usage()
	{
	fprintf(stderr,  "gen_srberror [-v] { SRB error source file }\n");
	exit(1);
	}

main(argc, argv)
	int argc;
	char **argv;
	{
	int c;
	extern char *optarg;
	extern int optind;


	/* Parse args */
	if ( argc > 3 )
		usage();

	Vflag = 0; 
	while ((c = getopt(argc, argv, "v")) != EOF)
		switch(c) 
			{
			case 'v':
				Vflag = 1;
				break;
			case '?':
				usage();
			}
		
	if ( argv[optind] != NULL )
		{
		/* Use input file */
		yyin = fopen(argv[optind], "r");
		if ( yyin ==  NULL )
			{
			fprintf (stderr, "Can't open input file %s\n", argv[1]);
			perror("gen_srberror");
			exit(1);
			}
		}
	
	/* Print program version number */ 
	if ( Vflag == 1 )
		printf ("srb_generror 1.0\n===================================\n");

	Errid = SRB_ERR_BASE;
	Lineno = 1;
	Tbl_head = InitErrTbl();
	yyparse();
	EmitErrTbl(Tbl_head, 1);

	exit(0);

	}
