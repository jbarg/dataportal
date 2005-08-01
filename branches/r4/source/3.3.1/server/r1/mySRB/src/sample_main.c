#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#ifdef PORTNAME_c90
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif

/* These 6 constants need to be set for proper working of mySRB */
/* Look at the mySRBCall documentation, below, for explanantions */

#define  LOGDIR              "/projects/diglib/sekar/log2"
#define  RETURNEMAILDOMAIN   "sdsc.edu"
#define  CHECKHOSTADDRESS    "132.249."
#define  CONNECTHOSTADDRESS  "srb.sdsc.edu"
#define  CONNECTPORTNUMBER   "7544"
#define  HTTPTYPE            "secure"    /* empty string if using http */
#define  WEBPATH             "https://srb.npaci.edu/"
#define  EMAILADDRESS        "srb@sdsc.edu"
#define  NEWUSERDOMAIN	     "sdsc"
#define  DEFAULTRESOURCE     "unix-sdsc"
#define  LANGDIR             "/projects/diglib/sekar/lang2"
         
/***********************************************************************
  NAME   : mySRBCall
  PURPOSE: The function call for processing the http-get/put CGI Call
  INPUT  : 
           logDir - Full directory path where log file (mySRBLog) file will
	            reside. this directory will also be used for keeping
		    session information. Make this directory is
		    read-writable by the "account" that will run the 
		    cgi-script. 
	   returnEmailDomain - Used for self-registration: this restricts the
	            users to be inside a email domain. The email domain 
		    (eg. "sdsc.edu") will be used forsending password
	            information back to newly registered users. Hence users
		    outside the domain cannot self-register. They should
		    use other mechanisms with more 'human control' to
		    obtain accounts.
	   checkHostAddress - Used for restricting users to register from
	            only a restricted set of ip-addresses. The partial
		    ip-address string will be sub-string matched in 
		    the incoming REMOTE_ADDR variable for acceptance.
		    for example, use "132.249." to restrict users to
		    be from ip-address matching: 132.249.xxx.xxx
	   connectHostAddress -  The host address where the connection
	            to SRB server will be made. Remember the SRB server
		    need not be on the same host where the cgi-script is 
		    executed.
	   connectPortNumber  - The Port Number that will be used by
	            default for making the SRB connection. Use "5588" for
		    default to connect to SRB Version 1.1.8. Note that
		    this is a string even though the portNumber is an integer.
	   httpType  - The type of http  protocol being used. It can be either
	            https or http. if using https, please use "secure" for this
		    parameter value, otherwise, leave it an empty string "".
	   webPath  - The web directory path name to the one where the 
	            images are stored. 
           emailAddress - The contact emailAddress in case of problems.
	   newuserdomain - the default domain given to a new user
	   defaultResource - default Resource set for file ingestion.
       langdir - Full directory path where language files are stored.
                 Must be readable by the account that runs the cgi.
		    
  OUTPUT : none
  return : none
  NOTE   : See Notes for compilation and installation

 **********************************************************************/
extern int mySRBCall(char *inlogDir,
		     char *returnEmailDomain,
		     char *checkHostAddress,
		     char *connectHostAddress,
		     char *connectPortNumber, 
		     char *inhttptype, 
		     char *webPath, 
		     char *emailAddress,
		     char *newuserdomain,
		     char *defaultResource,
		     char *inlangDir);

int
main(int argc, char **argv){

	mySRBCall(LOGDIR, RETURNEMAILDOMAIN, CHECKHOSTADDRESS, 
		  CONNECTHOSTADDRESS, CONNECTPORTNUMBER, HTTPTYPE,
		  WEBPATH, EMAILADDRESS, NEWUSERDOMAIN, DEFAULTRESOURCE, LANGDIR);
	return 0;
}
