/*
**************************************************************************
Copyright Notice
All Rights Reserved

Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************

This is a simple interface library for applications (such as MDS+) to
access SRB functionality at the object level via calls similar to unix
IO calls(open, read, write, close, etc).

There is a single .h file defining the routines in this library.  So
the build environment for the client application is simple, they need the
srbUio.h file and the libsrbUio.a library file built for the appropriate OS.

See the srbUio.h file for more information.

For calls that might be the initial SRB call, this library will first
call the initSrbClientEnv and srbConnect routines (if they haven't
been already).  For all calls, this library will then call the
corresponding SRB client routine.

For errors, this library will print an SRB error message, set errno
(converting the SRB error to a somewhat corresponding errno value), and
return a negative value; much like the unix IO routines do.

For srbUioOpen, it will emulate the Unix Open call in another way:
if the open fails and the flags argument requests O_CREAT, it will then
call srbObjCreate to attempt to create the object.

Note that the srbUioRename routine renames the srb object (file) in the
SRB logical name space.  If you want to move the file's physical location,
use another API, such as the Sphymove command.

Wayne Schroeder
Initial version October, 2003

*/

#include "scommands.h"
#include "srbClient.h"

#define AUTO_DISCONNECT 1  /* If defined, disconnect from the SRB server
     (clean up ~/.srb/.MdasEnv* files) when the last file is closed. */

srbConn *conn=NULL;
int openFileCount=0;

extern char srbAuth[];
extern char srbHost[];
extern char srbUser[];
extern char mdasCollectionName[];
extern char mdasCollectionHome[];
extern char inCondition[];


int
srbUioOpenConnection() 
{
    static int openned=0;  /* state flag, already openned or not */
    int i;
    if (openned==0) {

         /* by calling initSrbEnv instead of initSrbClientEnv(), all
	 functions work fine, whether or not Sinit has been run.
	 (Previously, srbObjCreate would fail with an invalid
	 resource). */
        i = initSrbEnv();
	if (i < 0) {
	    printf("SRB Client Environment Initialization Error:%i\n",i);
	    errno = EIO;
	    return -1;
	}

	conn = srbConnect (srbHost, NULL, srbAuth, 
			   NULL, NULL, NULL, NULL);
	if (clStatus(conn) != CLI_CONNECTION_OK) {
	    fprintf(stderr,"Connection to srbMaster failed: %s\n",
		    clErrorMessage(conn));
	    srb_perror (2, clStatus(conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    clFinish(conn); 
	    errno = EIO;
	    return -2;
	}
	openned=1;
    }
    return 0;
}

int 
srbUioOpen(char *name, int flags, mode_t mode)
{
    int status;
    char dataName[MAX_TOKEN];
    char collName[MAX_TOKEN];

    char newResourceName[MAX_TOKEN];

    char datatype[MAX_TOKEN];
    int myFlags;

    status = srbUioOpenConnection(); /* connect if necessary */
    if (status != 0) {
        return(status);
    }
    splitpath(name, collName, dataName);
    strcpy(datatype,"");

    myFlags=flags;
    myFlags &= ~(O_CREAT);  /* create not allowed */

    status = srbObjOpen(conn, dataName, myFlags, collName);
    if (status < 0) {
        if (status == DATA_NOT_IN_CAT) {
	    if ( (flags&O_CREAT) !=0 ) {
	        get_user_preference(newResourceName, "DRSRC");

    /* If Sinit has not been run, this will fail, altho an open is OK */
		status = srbObjCreate(conn, MDAS_CATALOG, dataName, datatype, 
				      newResourceName, collName, "", 0); 

	    }
	}
    }
    if (status < 0) {
        fprintf(stderr,"srbObjOpen failed: %d %s", status, 
		clErrorMessage(conn));
	errno = EIO;
	if (status == DATA_NOT_IN_CAT)  errno=ENOENT;
	return (-1);
    }

    openFileCount++;

    return(status);
}

int
srbUioDisconnect()
{
  int i;
    if (conn != NULL) {
        i = RemoveSrbClientEnvFile();
    }
}

int
srbUioClose(int fd)
{
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        return(-1);
    }
    srbObjClose(conn, fd);

    openFileCount--;
#ifdef AUTO_DISCONNECT
    if (openFileCount <= 0) {
        srbUioDisconnect();
    }
#endif
    return 0;
}

int
srbUioWrite(int fd, void *buf, size_t nbytes)
{
    int wbytes;
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        return(-1);
    }
    wbytes = srbObjWrite(conn, fd, buf, nbytes);
    if (wbytes < 0) {
        fprintf(stderr,"srbObjWrite error: %d %s", wbytes, 
		clErrorMessage(conn));
    }
    return(wbytes);
}

srb_long_t
srbUioSeek(int fd, srb_long_t offset, int whence)
{
    srb_long_t status;
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        return(-1);
    }
    status = srbObjSeek(conn, fd, offset, whence);
    if (status != 0) {
        errno=EIO;
    }
    return status;
}

int
srbUioLock(int fd, srb_long_t offset, int size, int mode)
{
    int status;
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        return(-1);
    }
    status = srbObjLockRange(conn, fd, offset, size, mode);
    if (status != 0) {
        errno=EIO;
    }
    return status;
}

ssize_t
srbUioRead(int fd, void *buf, size_t nbytes)
{
    int rbytes;
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        return(-1);
    }
    rbytes = srbObjRead(conn, fd, buf, nbytes);
    if (rbytes < 0) {
        fprintf(stderr,"srbObjRead error: %d %s", rbytes, 
		clErrorMessage(conn));
    }
    return(rbytes);
}


int
srbUioUnlink(char *path)
{
    int status;
    char collName[MAX_TOKEN];
    char fileName[MAX_TOKEN];

    status = srbUioOpenConnection(); /* connect if necessary */
    if (status != 0) {
        return(status);
    }

    splitpath(path, collName, fileName);  /* split path and, if necessary,
					     prepend mdasCollectionName */
    status = srbObjUnlink(conn, fileName, collName);
    if (status != 0) {
        fprintf(stderr,"srbObjUnlink failed: %d %s", status, 
		clErrorMessage(conn));
	errno = EIO;
	if (status == DATA_NOT_IN_CAT)  errno=ENOENT;
	if (status == MCAT_INQUIRE_ERROR)  errno=ENOENT;
	return (-1);
    }
    return (status);
}

int
srbUioStat(char *path, struct stat *buf)
{
    int status;
    char myPath[MAX_TOKEN*2];

    status = srbUioOpenConnection(); /* connect if necessary */
    if (status != 0) {
        return(status);
    }

    if (*path=='/') {
      strncpy(myPath, path, MAX_TOKEN);
    }
    else {
      strncpy(myPath, mdasCollectionName, MAX_TOKEN);
      strcat(myPath, "/");
      strncat(myPath, path, MAX_TOKEN-1);
    }
    status = srbObjStat (conn, MDAS_CATALOG, myPath, buf);
    if (status != 0) {
        fprintf(stderr,"srbObjStat failed: %d %s", status, 
		clErrorMessage(conn));
	errno = EIO;
	if (status == DATA_NOT_IN_CAT)  errno=ENOENT;
	if (status == MCAT_INQUIRE_ERROR)  errno=ENOENT;
	return (-1);
    }
    return (status);
}

int
srbUioRename(char *oldPath, char *newPath) 
{
    srbPathName nameArray[MAX_TOKEN], targNameArray[MAX_TOKEN];
    int status;
    int flagval;
    int nArgv;

    if (strlen(oldPath)==0 || strlen(newPath)==0) {
        fprintf(stderr, 
	      "srbUioRename: Input argument error, null string\n");
	errno = EINVAL;
	return -1;
    }

    status = srbUioOpenConnection(); /* connect if necessary */
    if (status != 0) {
        return(status);
    }

    targNameArray[0].inpArgv = newPath;

    nameArray[0].inpArgv = oldPath;
    nArgv = 1;

    /* Convert the source name to SQL like */
    if (convNameArrayToSQL (nameArray, nArgv) < 0) {
        fprintf(stderr, 
	      "srbUioRename: convNameArrayToSQL error\n");
	errno = EINVAL;
	return -1;
    }

    /* Convert the target name to SQL like */
    if (convNameArrayToSQL (targNameArray, 1) < 0) {
        fprintf(stderr, 
	      "srbUioRename: convNameArrayToSQL error\n");
	errno = EINVAL;
	return -1;
    }

    flagval = 0;

    status = mvMain(conn, MDAS_CATALOG, nArgv, nameArray, targNameArray,
		    flagval);
    if (status != 0) {
	  errno = EIO;
	  if (status == CLI_ERR_COLLNAME)  errno=ENOENT;
	  return -1;
    }
    return 0;
}

