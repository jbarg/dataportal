/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <scommands.h>

extern int Srb_numents;

/*------------------------------------------------------------------------------
    NAME
 
       Serror - Emit an SRB error
 
    SYNOPSIS
 
       Serror { SRB_error_mnenomic | SRB_error_id }
 
    DESCRIPTION
 
       Serror emits the following information about the SRB error
       referred to by one of the arguments:
 
        SRB error id
        SRB error mnenomic
        SRB short message
        SRB long message
        SRB recommended action
 
       Serror requires as input either the SRB_error_mnenomic or
       SRB_error_id.  The SRB_error_mnenomic is usually issued by a
       failing SRB utility.
 
     EXIT STATUS
 
       If the srb error is known, an exit status of 0 is set upon exit.
       It the srb error is unknow, an exit status of 1 is set upon exit.
 
 
------------------------------------------------------------------------------*/

void usage()
	{
	fprintf (stderr, "Serror { SRB_error_mnenomic | SRB_error_id }\n");
	exit(1);
	}

main(argc, argv)
	int argc;
	char **argv;
	{
	int i;

	if (argc > 2 || argc < 2) 
		{
		usage();
		}
	
	printf ("%s\n", argv[0]);
	/* srb error id's must start with a digit */
	if ( isdigit(*argv[1]) || (*argv[1] == '-') )
		srb_perror (1, atoi(argv[1]), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	else
		srb_perror(1, 0, argv[1], SRB_RCMD_ACTION|SRB_LONG_MSG) ;

	exit(0);
	}
