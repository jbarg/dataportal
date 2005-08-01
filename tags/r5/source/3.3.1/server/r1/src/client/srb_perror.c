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
#ifdef PORTNAME_solaris
#include <strings.h>
#endif

#ifdef _WIN32
#include <string.h>
#endif

#include "srbErrno.h"

/*------------------------------------------------------------------------------
 void srb_perror(fd, error_id, flags)

  SYNOPSIS
     #include <srbErrno.h>
 
     void srb_perror(fd, error_id, error_mnenomic, flags);
 
  DESCRIPTION
 
     srb_error() emits an SRB error message corresponding to the SRB
     error referred to by <error_id> or <error_mnenomic> (if non-null) .  
	if both <error_id> and <error_mnenomic> are passed, <error_mnenomic>
	will take precedence.
	 The message is emitted to the file
     descriptor referred to by <fd>. If <fd> is invalid, the
     error will be emitted to the stderr (fd 3).
     Optional behavior of srb_perror is controlled by the <flags>
     parameter. <flags> is constructed by ORing the following values:
 
     SRB_LONG_MSG       Emit the SRB error long message.
     SRB_RCMD_ACTION    Emit the SRB recommended action.
 
     The format of the message emitted to <fd> is as follows:
 
     srb error mnenomic: srb short message
     [ srb long message ]
     [ srb recommended action ]
 
  INPUT PARAMETERS
 
     fd        Refers to a file description to which the message should
               be emitted.  If fd is invalid, the message will be emitted
               to stderr.

 
     error_id  Refers to a valid SRB error id from the SRB error message
               table contained within srb_error.h.  If the error id
               is invalid, the error SRB_NO_ERROR will be emitted.
 
     flags     Contains the OR'ed value of the flags described above.


------------------------------------------------------------------------------*/
void srb_perror(int fd, int error_id, char *error_mnenomic, int flags)
{
	struct stat buf;
	char mbuf[SRB_ERR_NAMELEN + SRB_ERR_SHORTLEN +
				SRB_ERR_LONGLEN + SRB_ERR_ACTLEN + 50];
	int i;
	int ifd;
	FILE *fp;
	
	/* Determine if fd is valid -- if not use stderr */
	if (fstat(fd, &buf) != 0)
		ifd = 2;
	else
		ifd = fd;

	/* Convert the fd to a file pointer for fprintf */
	fp = fdopen(ifd, "w");
	if ( fp == NULL )
		{
		fprintf (stderr, "Fatal Error: Can't convert fd %d to file pointer.  Exiting\n", ifd);
		return;
		}

	if ( (error_mnenomic != NULL) && (strcmp("", error_mnenomic) != 0) )
		{
    		/* Find the error ID for the mnenomic. */
    		for ( i = 0; i < Srb_numents; i++ )
			{
			if ( strcmp(srb_errent[i].srb_err_name, error_mnenomic) == 0 )
			break;
			}
 		}
	else
		{
		/* Search error table for error id */
		for ( i = 0; i < Srb_numents; i++ )
			{
			if ( srb_errent[i].srb_err == error_id )
				break;
			}
		}

	if ( i == Srb_numents ) {
	    /* Invalid error id */
	    if ( (error_mnenomic != NULL) && (strcmp("", error_mnenomic) != 0) )
		fprintf (stderr, "srb_perror: Error %s does not correspond to any known SRB error.\n", error_mnenomic);
	    else
		fprintf (stderr, "srb_perror: Error %d does not correspond to any known SRB error.\n", error_id);
	    return;
	}
	
	/* compose the message */
	sprintf (mbuf, "%s: %s\n", srb_errent[i].srb_err_name, srb_errent[i].srb_err_short);

	if ( (flags & SRB_LONG_MSG) != 0 )
		sprintf(&(mbuf[strlen(mbuf)]), "%s\n", srb_errent[i].srb_err_long);

	if ( (flags & SRB_RCMD_ACTION) != 0 )
		sprintf (&(mbuf[strlen(mbuf)]), "%s\n", srb_errent[i].srb_err_action);



	/* Emit the message */
	fprintf (fp, mbuf);

	return;

	}

