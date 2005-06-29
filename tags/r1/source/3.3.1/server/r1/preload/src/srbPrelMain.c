/* #define OPEN_DEBUG */
/* #define FD_DEBUG */
#include "srbPrelMain.h"


#ifdef c_plus_plus
extern "C" 
#endif
int noNativeCallError(char *callStr )
{	
    char buffer[BUFF_SZ];

    sprintf (buffer, "preloadError: No native Syscall for: %s\n",
     callStr);

    syscall(SYS_write, 2, buffer, strlen(buffer));
    syscall(SYS_exit, -1);

    return -1;
}

#ifdef c_plus_plus
extern "C" 
#endif
int myopen( const char* path, int flags, int mode, int is64Flag)
{
    int result;

    va_list preload_arglist;
    int fd;
    char resource[MAX_TOKEN];
    char myOutPath[MAX_TOKEN];

#ifdef foo
    /* XXXXX - a hack - cp somehow set mode = 0 */
    if (mode == 0)
	mode = S_IRUSR | S_IWUSR;
#endif

    va_end(preload_arglist);


    if (isSrbPath ((char *)path, resource, myOutPath) > 0) {
	char collection[MAX_TOKEN];
	char name[MAX_TOKEN];

	if (!prelConn) 
	    prelConn = connToSrb ();
#ifdef OPEN_DEBUG
        fprintf (stderr, "myOutPath=%s\n", myOutPath);
#endif
	if(prelConn) {

#ifdef OPEN_DEBUG
            fprintf (stderr, "parseName output, path=%s,\n",
             myOutPath);
#endif
	    splitpath (myOutPath, collection, name);

#ifdef OPEN_DEBUG
	    fprintf (stderr, "opening coll:%s dataName:%s\n", 
	     collection, name);
#endif
	    if (resource[0] == '\0') {
		strcpy (resource, defaultResource);
	    }

	    if(flags & O_CREAT) {
		if (!(flags & O_EXCL))
		    srbObjUnlink (prelConn, name, collection );
		result = srbObjCreate( prelConn, MDAS_CATALOG, name, 
		 "", resource, collection, 0, 0 );
	    } else {
		result = srbObjOpen( prelConn, name, flags, collection );
		/* may be it is a collection */
#ifdef foo
		if (result < 0) {
		    result = checkCollection (prelConn, MDAS_CATALOG, 
		     myOutPath);
		    if (result == 0) {
		        /* allocation a fake desc */
		        result = getCollDesc();
		    }
		}
#endif
#ifdef foo
		if (result == OBJ_ERR_COLL_OPEN)
		    result = getCollDesc();
#endif
	    }
	    if(result < 0) {
		setErrno (result);
		result = -1;
	    } else {
		result |= SRB_FD_BIT;
	    }
	} else {
	    errno = ENOTCONN;
	    result = -1;
	}
    } else {
#ifdef SYS_open
	if (is64Flag == IS_64_BIT)
#if defined(PORTNAME_linux)
	    /* for linux, just use open */
	    result = syscall(SYS_open, myOutPath, flags, mode);
#else
	    result = syscall(SYS_open64, myOutPath, flags, mode);
#endif
	else
	    result = syscall(SYS_open, myOutPath, flags, mode);
#else
	result = noNativeCallError ("open");
#endif
    }
    if (result >= 0) {
	if (result & IS_COLL_FD) {
	    result = result & (~IS_COLL_FD);
	    srbFdArray[result].isDir = IS_DIR_1;
	}
        strcpy (srbFdArray[result].path, myOutPath);
        srbFdArray[result].fd = result;
    }

    return result;
}

#ifdef c_plus_plus
extern "C" 
#endif
int open( const char* path, int flags, ... )
{
    va_list preload_arglist;
    int	mode;
    va_start(preload_arglist,flags);

    mode = va_arg(preload_arglist,int);
    va_end(preload_arglist);
    return myopen( path, flags, mode, IS_32_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
int _open( const char* path, int flags, ... )
{
    va_list preload_arglist;
    int	mode;
    va_start(preload_arglist,flags);

    mode = va_arg(preload_arglist,int);
    va_end(preload_arglist);
    return myopen( path, flags, mode, IS_32_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
int __open( const char* path, int flags, ... )
{
    va_list preload_arglist;
    int	mode;
    va_start(preload_arglist,flags);

    mode = va_arg(preload_arglist,int);
    va_end(preload_arglist);
    return myopen( path, flags, mode, IS_32_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
int ___open( const char* path, int flags, ... )
{
    va_list preload_arglist;
    int	mode;
    va_start(preload_arglist,flags);

    mode = va_arg(preload_arglist,int);
    va_end(preload_arglist);
    return myopen( path, flags, mode, IS_32_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
int open64( const char* path, int flags, ... )
{
    va_list preload_arglist;
    int	mode;
    va_start(preload_arglist,flags);

    mode = va_arg(preload_arglist,int);
    va_end(preload_arglist);
    return myopen( path, flags, mode, IS_64_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
int _open64( const char* path, int flags, ... )
{
    va_list preload_arglist;
    int	mode;
    va_start(preload_arglist,flags);

    mode = va_arg(preload_arglist,int);
    va_end(preload_arglist);
    return myopen( path, flags, mode, IS_64_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
int __open64( const char* path, int flags, ... )
{
    va_list preload_arglist;
    int	mode;
    va_start(preload_arglist,flags);

    mode = va_arg(preload_arglist,int);
    va_end(preload_arglist);
    return myopen( path, flags, mode, IS_64_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
int ___open64( const char* path, int flags, ... )
{
    va_list preload_arglist;
    int	mode;
    va_start(preload_arglist,flags);

    mode = va_arg(preload_arglist,int);
    va_end(preload_arglist);
    return myopen( path, flags, mode, IS_64_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
#if defined(PORTNAME_linux)
int creat ( const char* path, mode_t mode )
#else
int creat ( const char* path, ulong mode )
#endif
{
    return myopen( path, O_CREAT|O_TRUNC|O_WRONLY, mode, IS_32_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
int _creat ( const char* path, ulong mode )
{
    return myopen( path, O_CREAT|O_TRUNC|O_WRONLY, mode, IS_32_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
int __creat ( const char* path, ulong mode )
{
    return myopen( path, O_CREAT|O_TRUNC|O_WRONLY, mode, IS_32_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
#if defined(PORTNAME_linux)
int creat64 ( const char* path, mode_t mode )
#else
int creat64 ( const char* path, ulong mode )
#endif
{
    return myopen( path, O_CREAT|O_TRUNC|O_WRONLY, mode, IS_64_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
int _creat64 ( const char* path, ulong mode )
{
    return myopen( path, O_CREAT|O_TRUNC|O_WRONLY, mode, IS_64_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
int __creat64 ( const char* path, ulong mode )
{
    return myopen( path, O_CREAT|O_TRUNC|O_WRONLY, mode, IS_64_BIT );
}

#ifdef c_plus_plus
extern "C" 
#endif
int myclose( int fd )
{
    int result;

    if (fd < 0 || fd >= SRB_FD_BIT *2) {
	errno = EBADF;
        return -1;
    }

    if(fd & SRB_FD_BIT) {
	int myfd = fd & (~SRB_FD_BIT);
	if (srbFdArray[fd].fd <= 0) {
	    errno = EBADF;
	    return -1;
	}
	if (prelConn)
	    result = srbObjClose( prelConn, myfd);
	else
	    result = 0;
        srbFdArray[fd].fd = 0; 
        srbFdArray[fd].isDir = 0; 
        srbFdArray[fd].maxDirLen = 0;
        srbFdArray[fd].offset = 0;
        srbFdArray[fd].accumOffset = 0;
        srbFdArray[fd].gotData = 0;
        srbFdArray[fd].continueIndex = -1;
        if (srbFdArray[fd].dirent != NULL) {
	    free (srbFdArray[fd].dirent);
            srbFdArray[fd].dirent = NULL;
	}
        srbFdArray[fd].path[0] = '\0';
    } else {
#ifdef SYS_close
	result = syscall(SYS_close,fd);
	srbFdArray[fd].fd = 0;
	 srbFdArray[fd].path[0] = '\0';
#else
	result = noNativeCallError ("close");
#endif
    }

    return result;
}

#ifdef c_plus_plus
extern "C" 
#endif
int close( int fd )
{
    return myclose( fd );
}

#ifdef c_plus_plus
extern "C" 
#endif
int _close( int fd )
{
    return myclose( fd );
}

#ifdef c_plus_plus
extern "C" 
#endif
int __close( int fd )
{
    return myclose( fd );
}

#ifdef c_plus_plus
extern "C" 
#endif
int ___close( int fd )
{
    return myclose( fd );
}

#ifdef c_plus_plus
extern "C" 
#endif
int myunlink (const char *path)
{
    int result;
    char resource[MAX_TOKEN];
    char myOutPath[MAX_TOKEN];

    if (isSrbPath ((char *) path, resource, myOutPath) > 0) {
        char collection[MAX_TOKEN];
        char name[MAX_TOKEN];

        if (!prelConn)
            prelConn = connToSrb ();
        if(prelConn) {
	    splitpath (myOutPath, collection, name);

	    result = srbObjUnlink (prelConn, name, collection );

            if (result < 0) {
            	setErrno (result);
            	result = -1;
            }
        } else {
            errno = ENOTCONN;
            result = -1;
        }
    } else {
#ifdef SYS_unlink
	result = syscall(SYS_unlink, myOutPath);
#else
	result = noNativeCallError ("unlink");
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int unlink( const char* path)
{
    return myunlink( path);
}

#ifdef c_plus_plus
extern "C"
#endif
int _unlink( const char* path)
{
    return myunlink( path);
}

#ifdef c_plus_plus
extern "C" 
#endif
int myrmdir (const char *path)
{
    int result;
    char resource[MAX_TOKEN];
    char myOutPath[MAX_TOKEN];

    if (isSrbPath ((char *) path, resource, myOutPath) > 0) {

        if (!prelConn)
            prelConn = connToSrb ();
        if(prelConn) {

	    result = srbModifyCollect (prelConn, MDAS_CATALOG, 
	     myOutPath, "", "", "", D_DELETE_COLL);

            if (result < 0) {
            	setErrno (result);
            	result = -1;
            }
        } else {
            errno = ENOTCONN;
            result = -1;
        }
    } else {
#ifdef SYS_rmdir
	result = syscall(SYS_rmdir, myOutPath);
#else
	result = noNativeCallError ("rmdir");
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int rmdir( const char* path)
{
    return myrmdir( path);
}

#ifdef c_plus_plus
extern "C"
#endif
int _rmdir( const char* path)
{
    return myrmdir( path);
}

#ifdef c_plus_plus
extern "C" 
#endif
int myrename (const char *oldPath, const char *newPath)
{
    int result;
    char resource[MAX_TOKEN];
    char myOldOutPath[MAX_TOKEN];
    char myNewOutPath[MAX_TOKEN];

    isSrbPath ((char *) newPath, resource, myNewOutPath);
    if (isSrbPath ((char *) oldPath, resource, myOldOutPath) > 0) {


        if (!prelConn)
            prelConn = connToSrb ();
        if(prelConn) {

	    if (checkCollection (prelConn, MDAS_CATALOG, myOldOutPath) == 0) {
		/* a collection */
		result = srbModifyCollect(prelConn, MDAS_CATALOG, myOldOutPath,
                 myNewOutPath, "", "", D_CHANGE_COLL_NAME);
	    } else {
		char myOldName[MAX_TOKEN], myOldColl[MAX_TOKEN];
		char myNewName[MAX_TOKEN], myNewColl[MAX_TOKEN];

		splitpath (myOldOutPath, myOldColl, myOldName);
		splitpath (myNewOutPath, myNewColl, myNewName);

      		if (!strcmp(myNewColl, myOldColl)) { /* only renaming data */
        	    result =  srbModifyDataset(prelConn, MDAS_CATALOG, 
		     myOldName, myOldColl, "","", myNewName, "",D_CHANGE_DNAME);
      		} else if (!strcmp (myOldName,myNewName)) { 
		    /* only moving to new collection */
        	    result = srbModifyDataset (prelConn, MDAS_CATALOG, 
		     myOldName, myOldColl, "","", myNewColl, "",D_CHANGE_GROUP);
      		} else { /* changing name of object as well as its collection */
		    char tempName[MAX_TOKEN], cval[MAX_TOKEN];
		    
        	    srandom((unsigned int) getpid());
        	    sprintf(cval,"%ld",(long) random());
        	    sprintf(tempName, "%s.%s", myNewName, cval);
        	    result = srbModifyDataset(prelConn, MDAS_CATALOG, myOldName,
		      myOldColl, "","", tempName, "", D_CHANGE_DNAME);

		    if (result < 0) {
			setErrno (result);
			return -1;
		    }

        	    result = srbModifyDataset(prelConn, MDAS_CATALOG, tempName, 
		     myOldColl, "","", myNewColl, "", D_CHANGE_GROUP);
        	    if (result < 0) {
          		srbModifyDataset(prelConn, MDAS_CATALOG, tempName, 
			 myOldColl, "","", myOldName, "", D_CHANGE_DNAME);
                        setErrno (result);
                        return -1;
        	    }
        	    result = srbModifyDataset(prelConn, MDAS_CATALOG, tempName, 
		     myNewColl, "","", myNewName, "", D_CHANGE_DNAME);
        	    if (result < 0) {
          		srbModifyDataset(prelConn, MDAS_CATALOG, tempName, 
			 myNewColl, "","", myOldColl, "", D_CHANGE_GROUP);
          		srbModifyDataset(prelConn, MDAS_CATALOG, tempName, 
			 myOldColl, "","", myOldName, "", D_CHANGE_DNAME);
        	    }
      		}
	    }

            if (result < 0) {
            	setErrno (result);
            	result = -1;
            }
        } else {
            errno = ENOTCONN;
            result = -1;
        }
    } else {
#ifdef SYS_rename
	result = syscall(SYS_rename, myOldOutPath, myNewOutPath);
#else
	result = noNativeCallError ("rename");
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int rename (const char* oldPath, const char* newPath)
{
    return myrename (oldPath, newPath);
}

#ifdef c_plus_plus
extern "C"
#endif
int _rename (const char* oldPath, const char* newPath)
{
    return myrename (oldPath, newPath);
}

#ifdef c_plus_plus
extern "C" 
#endif
int mychdir (const char *path)
{
    int result;
    char resource[MAX_TOKEN];
    char myOutPath[MAX_TOKEN];

    if (isSrbPath ((char *) path, resource, myOutPath) > 0) {

        if (!prelConn)
            prelConn = connToSrb ();
        if(prelConn) {
	    if (checkCollection(prelConn, MDAS_CATALOG, myOutPath) == 0) {
		strcpy(mdasCollectionName, myOutPath);
		myPutenv (CWD_FS_FLAG, SRB_FS);
		myPutenv (SRB_CWD, mdasCollectionName);
		result = 0;
	    } else {
		result = -1;
		errno = ENOENT;
	    }

        } else {
            errno = ENOTCONN;
            result = -1;
        }
    } else {
#ifdef SYS_chdir
	result = syscall(SYS_chdir, myOutPath);
	if (result >= 0) {
	    myPutenv (CWD_FS_FLAG, LOCAL_FS);
	    myPutenv (LOCAL_CWD, myOutPath);
	}
#else
	result = noNativeCallError ("chdir");
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int chdir( const char* path)
{
    return mychdir( path);
}

#ifdef c_plus_plus
extern "C"
#endif
int _chdir( const char* path)
{
    return mychdir( path);
}

#ifdef c_plus_plus
extern "C" 
#endif
int myfchdir (int fd)
{
    int result;
    char *mypath;

    if (fd < 0 || fd >= SRB_FD_BIT *2) {
        errno = EBADF;
        return -1;
    }

    if(fd & SRB_FD_BIT) {
        int myfd = fd & (~SRB_FD_BIT);
        if (srbFdArray[fd].fd <= 0) {
            errno = EBADF;
            return -1;
        }

	if (!prelConn) {
            errno = ENOTCONN;
            result = -1;
    	}

	mypath = srbFdArray[fd].path;
	if (checkCollection(prelConn, MDAS_CATALOG, mypath) == 0) {
	    strcpy(mdasCollectionName, mypath);
	    myPutenv (CWD_FS_FLAG, SRB_FS);
	    myPutenv (SRB_CWD, mdasCollectionName);
	    result = 0;
	} else {
	    result = -1;
	    errno = ENOENT;
	}
    } else {
#ifdef SYS_fchdir
	result = syscall(SYS_fchdir, fd);
	if (result >= 0) {
	    myPutenv (CWD_FS_FLAG, LOCAL_FS);
	    myPutenv (LOCAL_CWD, srbFdArray[fd].path);
	}
#else
	result = noNativeCallError ("fchdir");
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int fchdir( int fd)
{
    return myfchdir (fd);
}

#ifdef c_plus_plus
extern "C"
#endif
int _fchdir(int fd)
{
    return myfchdir (fd);
}

#ifdef c_plus_plus
extern "C" 
#endif
char *_mygetcwd ()
{
    char *cwdPtr, *cwdFsFlag;

    if ((cwdFsFlag = getenv (CWD_FS_FLAG)) == NULL) { 
        fprintf (stderr, "CWD_FS_FLAG has not been initialized\n");
        errno = EEXIST;
        return NULL;
    }

    if (strcmp (cwdFsFlag, LOCAL_FS) == 0) {
        cwdPtr = getenv (LOCAL_CWD);
	if (cwdPtr == NULL) {
	    fprintf (stderr, "LOCAL_FS has not been initialized\n");
	    errno = EEXIST;
            return NULL;
	}
    } else if (strcmp (cwdFsFlag, SRB_FS) == 0) {
	int tmpLen;

	cwdPtr = getenv (SRB_CWD);
	if (cwdPtr == NULL) {
	    /* initialize it now */
	    if (EnvFilePath[0] == '\0') {      /* Have not been initialized */
		if (initSrbEnv () < 0) {
		    fprintf (stderr,"_mygetcwd: initSrbEnv error\n");
		    errno = EEXIST;
            	    return NULL;
		}
	    }
	    tmpLen = strlen (mdasCollectionName) + strlen (SRB_CWD) + 8;
	    cwdPtr = malloc (tmpLen);
	    sprintf (cwdPtr, "%s=%s", SRB_CWD, mdasCollectionName);
	    putenv (cwdPtr);
	    cwdPtr = getenv (SRB_CWD);
	}
    } else {
	 fprintf (stderr,"_mygetcwd: Illegal CWD_FS_FLAG: %s\n", cwdFsFlag);
	errno = EEXIST;
        return NULL;
    }
    return cwdPtr;
}

#ifdef c_plus_plus
extern "C" 
#endif
char *mygethome ()
{
    char *homePtr, *cwdFsFlag;

    if ((cwdFsFlag = getenv (CWD_FS_FLAG)) == NULL) { 
        fprintf (stderr, "CWD_FS_FLAG has not been initialized\n");
        errno = EEXIST;
        return NULL;
    }

    if (strcmp (cwdFsFlag, LOCAL_FS) == 0) {
        homePtr = getenv (LOCAL_HOME);
	if (homePtr == NULL) {
	    fprintf (stderr, "mygethome - HOME has not been initialized\n");
	    errno = EEXIST;
            return NULL;
	}
    } else if (strcmp (cwdFsFlag, SRB_FS) == 0) {
	int tmpLen;

	homePtr = getenv (SRB_HOME);
	if (homePtr == NULL) {
	    /* initialize it now */
	    if (EnvFilePath[0] == '\0') {      /* Have not been initialized */
		if (initSrbEnv () < 0) {
		    fprintf (stderr,"mygethome: initSrbEnv error\n");
		    errno = EEXIST;
            	    return NULL;
		}
	    }
	    tmpLen = strlen (mdasCollectionHome) + strlen (SRB_CWD) + 8;
	    homePtr = malloc (tmpLen);
	    sprintf (homePtr, "%s=%s", SRB_HOME, mdasCollectionHome);
	    putenv (homePtr);
	    homePtr = getenv (SRB_HOME);
	}
    } else {
	 fprintf (stderr,"mygethome: Illegal CWD_FS_FLAG: %s\n", cwdFsFlag);
	errno = EEXIST;
        return NULL;
    }
    return homePtr;
}

#ifdef c_plus_plus
extern "C"
#endif
char *mygetcwd (char *buf, uint size)
{

    int result;
    int myLen;
    char *cwdPtr;

    cwdPtr = _mygetcwd ();
    if (cwdPtr == NULL)
	return NULL;

    if (buf == NULL) {
	buf = strdup (cwdPtr);
    } else {
        myLen = strlen (cwdPtr) + 1;
    	if (myLen > size) {
	    errno = ERANGE;
	    buf = NULL;
    	} else {
	    strcpy (buf, cwdPtr);
	}
    }
    return buf;
}

#ifdef c_plus_plus
extern "C"
#endif
char *
getcwd (char *buf, uint size)
{
    return mygetcwd( buf, size);
}

#ifdef c_plus_plus
extern "C"
#endif
char *
_getcwd (char *buf, uint size)
{
    return mygetcwd( buf, size);
}

#ifdef c_plus_plus
extern "C" 
#endif
int mymkdir (const char *path, mode_t mode)
{
    int result;
    char resource[MAX_TOKEN];
    char myOutPath[MAX_TOKEN];

    if (isSrbPath ((char *) path, resource, myOutPath) > 0) {
        char collection[MAX_TOKEN];
        char name[MAX_TOKEN];

        if (!prelConn)
            prelConn = connToSrb ();
        if(prelConn) {
	    splitpath (myOutPath, collection, name);

	    result = srbCreateCollect (prelConn, MDAS_CATALOG,
	     collection, name );

            if (result < 0) {
            	setErrno (result);
            	result = -1;
            }
        } else {
            errno = ENOTCONN;
            result = -1;
        }
    } else {
#ifdef SYS_mkdir
	result = syscall(SYS_mkdir, myOutPath, mode);
#else
	result = noNativeCallError ("mkdir");
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int mkdir( const char* path,  mode_t mode)
{
    return mymkdir( path, mode);
}

#ifdef c_plus_plus
extern "C"
#endif
int _mkdir( const char* path,  mode_t mode)
{
    return mymkdir( path, mode);
}

#ifdef c_plus_plus
extern "C" 
#endif
int myread( int fd, void* data, size_t length )
{
    int result;

    if (fd < 0 || fd >= SRB_FD_BIT *2) {
        errno = EBADF;
        return -1;
    }

    if(fd & SRB_FD_BIT) {
        int myfd = fd & (~SRB_FD_BIT);
        if (srbFdArray[fd].fd <= 0) {
            errno = EBADF;
            return -1;
        }

    	if (!prelConn) {
            errno = ENOTCONN;
            result = -1;
    	}

	result = srbObjRead( prelConn, myfd, 
	 (char*)data, length );
	if (result < 0) {
	    setErrno (result);
	    result = -1;
	}
    } else {

#ifdef SYS_read
	result = syscall(SYS_read,fd, data, length);
#else
	result = noNativeCallError ("read");
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C" 
#endif
int read( int fd, void* data, size_t length )
{
    int status;
    
    status = myread( fd, data, length );
    return (status);
}

#ifdef c_plus_plus
extern "C" 
#endif
int _read( int fd, void* data, size_t length )
{
    return myread( fd, data, length );
}

#ifdef c_plus_plus
extern "C" 
#endif
int __read( int fd, void* data, size_t length )
{
    return myread( fd, data, length );
}

#ifdef c_plus_plus
extern "C" 
#endif
int ___read( int fd, void* data, size_t length )
{
    return myread( fd, data, length );
}

#ifdef c_plus_plus
extern "C" 
#endif
int mywrite( int fd, const void* data, size_t length )
{
    int result;

    if (fd < 0 || fd >= SRB_FD_BIT *2) {
        errno = EBADF;
        return -1;
    }

    if(fd & SRB_FD_BIT) {
        int myfd = fd & (~SRB_FD_BIT);
        if (srbFdArray[fd].fd <= 0) {
            errno = EBADF;
            return -1;
        }

    	if (!prelConn) {
            errno = ENOTCONN;
            result = -1;
    	}

	result = srbObjWrite( prelConn, myfd, 
	 (char*)data, length );
        if(result < 0) {
            setErrno (result);
            result = -1;
        }
    } else {

#ifdef SYS_write
	result = syscall(SYS_write,fd, data, length);
#else
	result = noNativeCallError ("write");
#endif
    }

	return result;
}

#ifdef c_plus_plus
extern "C" 
#endif
int write( int fd, const void* data, size_t length )
{
    return mywrite( fd, data, length );
}

#ifdef c_plus_plus
extern "C" 
#endif
int _write( int fd, const void* data, size_t length )
{
    return mywrite( fd, data, length );
}

#ifdef c_plus_plus
extern "C" 
#endif
int __write( int fd, const void* data, size_t length )
{
    return mywrite( fd, data, length );
}

#ifdef c_plus_plus
extern "C" 
#endif
int ___write( int fd, const void* data, size_t length )
{
    return mywrite( fd, data, length );
}

#ifdef c_plus_plus
extern "C" 
#endif
off_t mylseek( int fd, off_t where, int whence )
{
    off_t result;

    if (fd < 0 || fd >= SRB_FD_BIT *2) {
        errno = EBADF;
        return -1;
    }

    if(fd & SRB_FD_BIT) {
        int myfd = fd & (~SRB_FD_BIT);
        if (srbFdArray[fd].fd <= 0) {
            errno = EBADF;
            return -1;
        }

    	if (!prelConn) {
            errno = ENOTCONN;
           result = -1;
    	} 

        if (srbFdArray[fd].isDir > 0) {         /* a directory */
            result = srbCollSeek (prelConn, myfd, where, whence, IS_32_BIT );
        } else {
            result = srbObjSeek( prelConn, myfd, where, whence );
            if(result < 0) {
                setErrno (result);
                result = -1;
            }
        }
    } else {
#ifdef SYS_lseek
	result = syscall(SYS_lseek,fd, where, whence);
#else
	result = noNativeCallError ("lseek");
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C" 
#endif
off_t lseek( int fd, off_t where, int whence )
{
    return mylseek( fd, where, whence );
}

#ifdef c_plus_plus
extern "C" 
#endif
off_t _lseek( int fd, off_t where, int whence )
{
    return mylseek( fd, where, whence );
}

#ifdef c_plus_plus
extern "C" 
#endif
off_t __lseek( int fd, off_t where, int whence )
{
    return mylseek( fd, where, whence );
}

#ifdef c_plus_plus
extern "C" 
#endif
off_t ___lseek( int fd, off_t where, int whence )
{
    return mylseek( fd, where, whence );
}

#ifdef c_plus_plus
extern "C" 
#endif
off64_t mylseek64( int fd, off64_t where, int whence )
{
    off64_t result;

    if (fd < 0 || fd >= SRB_FD_BIT *2) {
        errno = EBADF;
        return -1;
    }

    if(fd & SRB_FD_BIT) {
        int myfd = fd & (~SRB_FD_BIT);
        if (srbFdArray[fd].fd <= 0) {
            errno = EBADF;
            return -1;
        }

    	if (!prelConn) {
            errno = ENOTCONN;
            result = -1;
    	}

	if (srbFdArray[fd].isDir > 0) { 	/* a directory */
	    result = srbCollSeek (prelConn, myfd, where, whence, IS_64_BIT );
	} else {
	    result = srbObjSeek( prelConn, myfd, where, whence );
            if(result < 0) {
            	setErrno (result);
            	result = -1;
	    }
        }
    } else {
	result = llseek (fd, where, whence);
    }
    return result;
}

#ifdef c_plus_plus
extern "C" 
#endif
off64_t lseek64( int fd, off64_t where, int whence )
{
    return mylseek64( fd, where, whence );
}

#ifdef c_plus_plus
extern "C" 
#endif
off64_t _lseek64( int fd, off64_t where, int whence )
{
    return mylseek64( fd, where, whence );
}

#ifdef c_plus_plus
extern "C" 
#endif
off64_t __lseek64( int fd, off64_t where, int whence )
{
    return mylseek64( fd, where, whence );
}

#ifdef c_plus_plus
extern "C" 
#endif
off64_t ___lseek64( int fd, off64_t where, int whence )
{
    return mylseek64( fd, where, whence );
}

#ifdef c_plus_plus
extern "C" 
#endif
int mystat ( const char* path, void *buf, int is64Flag)
{
    int result;

    char myOutPath[MAX_TOKEN];
    char resource[MAX_TOKEN];

    if (isSrbPath ((char *) path, resource, myOutPath) > 0) {
	char resource[MAX_TOKEN];
	char collection[MAX_TOKEN];
	char name[MAX_TOKEN];

	if (!prelConn) 
	    prelConn = connToSrb ();
	if(prelConn) {
	    if (is64Flag == IS_64_BIT)
	    	result = srbStat64 (myOutPath, (struct stat64 *)buf, 
		 IS_UNKNOWN);
	    else
	    	result = srbStat (myOutPath, (struct stat *)buf, IS_UNKNOWN);
            if (result < 0) {
                setErrno (result);
		result = -1;
	    }
	} else {
	    errno = ENOTCONN;
	    result = -1;
	}
    } else {
	if (is64Flag == IS_64_BIT)
#ifdef SYS_stat        /* XXXXX use SYS_stat for now */
#if defined(PORTNAME_linux)
        {
            struct kernel_stat kbuf;

            /* XXXXX use SYS_stat for now until kernel_stat64 is know */
            result = syscall(SYS_stat, myOutPath, &kbuf);
            myxstat64_conv (&kbuf, (struct stat64 *) buf);
        }
#else   /* PORTNAME_linux */
	    result = syscall(SYS_stat64, myOutPath, buf);
#endif  /* PORTNAME_linux */
#else   /* SYS_stat64 */
	    result = noNativeCallError ("stat64");
#endif	/* SYS_stat64 */
	else
#ifdef SYS_stat
#if defined(PORTNAME_linux)
        {
            struct kernel_stat kbuf;

            result = syscall(SYS_stat, myOutPath, &kbuf);
            myxstat_conv (&kbuf, (struct stat *) buf);
        }
#else	/* PORTNAME_linux */
	    result = syscall(SYS_stat, myOutPath, buf);
#endif  /* PORTNAME_linux */
#else	/* SYS_stat */
	    result = noNativeCallError ("stat");
#endif	/* SYS_stat */
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int stat ( const char* path, struct stat *buf)
{
    int result;

    result = mystat ( path, (void *) buf, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int _stat ( const char* path, struct stat *buf)
{
    int result;

    result = mystat ( path, (void *) buf, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __stat ( const char* path, struct stat *buf)
{
    int result;

    result = mystat ( path, (void *) buf, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int stat64 ( const char* path, struct stat64 *buf)
{
    int result;

    result = mystat ( path, (void *) buf, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int _stat64 ( const char* path, struct stat64 *buf)
{
    int result;

    result = mystat ( path, (void *) buf, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __xstat ( int ver, const char* path, struct stat *buf)
{
    int result;

    result = mystat ( path, (void *) buf, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __xstat64 ( int ver, const char* path, struct stat64 *buf)
{
    int result;

    result = mystat ( path, (void *) buf, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __stat64 ( const char* path, struct stat64 *buf)
{
    int result;

    result = mystat ( path, (void *) buf, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C" 
#endif
int mylstat ( const char* path, void *buf, int is64Flag)
{
    int result;

    char myOutPath[MAX_TOKEN];
    char resource[MAX_TOKEN];

    if (isSrbPath ((char *) path, resource, myOutPath) > 0) {
	char resource[MAX_TOKEN];
	char collection[MAX_TOKEN];
	char name[MAX_TOKEN];

	if (!prelConn) 
	    prelConn = connToSrb ();
	if(prelConn) {
	    if (is64Flag == IS_64_BIT) {
	    	result = srbStat64 (myOutPath, (struct stat64 *)buf, 
		 IS_UNKNOWN);
	    } else {
	    	result = srbStat (myOutPath, (struct stat *)buf, IS_UNKNOWN);
	    }
	    if (result < 0) {
	        setErrno (result);
		result = -1;
	    }

	} else {
	    errno = ENOTCONN;
	    result = -1;
	}
    } else {
	if (is64Flag == IS_64_BIT)
#ifdef SYS_lstat        /* XXXXX use SYS_lstat for now */
#if defined(PORTNAME_linux)
        {
            struct kernel_stat kbuf;

            /* XXXXX use SYS_lstat for now until kernel_stat64 is know */
            result = syscall(SYS_lstat, myOutPath, &kbuf);
            myxstat64_conv (&kbuf, (struct stat64 *) buf);
        }
#else   /* PORTNAME_linux */
            result = syscall(SYS_lstat64, myOutPath, buf);
#endif  /* PORTNAME_linux */
#else   /* SYS_lstat64 */
            result = noNativeCallError ("lstat64");
#endif  /* SYS_lstat64 */
        else
#ifdef SYS_lstat
#if defined(PORTNAME_linux)
        {
            struct kernel_stat kbuf;

            result = syscall(SYS_lstat, myOutPath, &kbuf);
            myxstat_conv (&kbuf, (struct stat *) buf);
        }
#else   /* PORTNAME_linux */
            result = syscall(SYS_lstat, myOutPath, buf);
#endif  /* PORTNAME_linux */
#else   /* SYS_lstat */
            result = noNativeCallError ("lstat");
#endif  /* SYS_lstat */
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int lstat ( const char* path, struct stat *buf)
{
    int result;

    result = mylstat ( path, (void *) buf, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int _lstat ( const char* path, struct stat *buf)
{
    int result;

    result = mylstat ( path, (void *) buf, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __lstat ( const char* path, struct stat *buf)
{
    int result;

    result = mylstat ( path, (void *) buf, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int lstat64 ( const char* path, struct stat64 *buf)
{
    int result;

    result = mylstat ( path, (void *) buf, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int _lstat64 ( const char* path, struct stat64 *buf)
{
    int result;

    result = mylstat ( path, (void *) buf, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __lstat64 ( const char* path, struct stat64 *buf)
{
    int result;

    result = mylstat ( path, (void *) buf, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __lxstat ( int ver, const char* path, struct stat *buf)
{
    int result;

    result = mylstat ( path, (void *) buf, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __lxstat64 ( int ver, const char* path, struct stat64 *buf)
{
    int result;

    result = mylstat ( path, (void *) buf, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C" 
#endif
int myfsync ( int fd)
{
    int result;

    if (fd < 0 || fd >= SRB_FD_BIT *2) {
        errno = EBADF;
        return -1;
    }

    if(fd & SRB_FD_BIT) {
        int myfd = fd & (~SRB_FD_BIT);
        if (srbFdArray[fd].fd <= 0) {
            errno = EBADF;
            return -1;
        }

    	if (!prelConn) {
            errno = ENOTCONN;
            result = -1;
    	}

	result = srbObjSync (prelConn, myfd);

    } else {
#ifdef SYS_fsync
	result = syscall(SYS_fsync, fd);
#else
	result = _fsync (fd);
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int fsync ( int fd)
{
    int result;

    result = myfsync ( fd );
    return (result);
}

#ifdef c_plus_plus
extern "C" 
#endif
int myfstat ( int fd, void *buf, int is64Flag)
{
    int result;

/* printf ("in myfstat, %d\n", fd); */
    if (fd < 0 || fd >= SRB_FD_BIT *2) {
        errno = EBADF;
        return -1;
    }

    if(fd & SRB_FD_BIT) {
        int myfd = fd & (~SRB_FD_BIT);
        if (srbFdArray[fd].fd <= 0) {
            errno = EBADF;
            return -1;
        }

        if (is64Flag == IS_64_BIT)
            result = srbStat64 (srbFdArray[fd].path, (struct stat64 *)buf,
	     srbFdArray[fd].isDir);
        else
	    result = srbStat (srbFdArray[fd].path, (struct stat *)buf, 
	     srbFdArray[fd].isDir);
            if (result < 0) {
                setErrno (result);
		result = -1;
	    }
    } else {
	if (is64Flag == IS_64_BIT) 
#ifdef SYS_fstat	/* XXXXX use SYS_fstat for now */
#if defined(PORTNAME_linux)
        {
            struct kernel_stat kbuf;

	    /* XXXXX use SYS_fstat for now until kernel_stat64 is know */
            result = syscall(SYS_fstat, fd, &kbuf);
            myxstat64_conv (&kbuf, (struct stat64 *) buf);
        }
#else   /* PORTNAME_linux */
	    result = syscall(SYS_fstat64, fd, buf); 
#endif	/* PORTNAME_linux */
#else	/* SYS_fstat64 */
	    result = noNativeCallError ("fstat64");
#endif	/* SYS_fstat64 */
	else
#ifdef SYS_fstat
#if defined(PORTNAME_linux)
	{
	    struct kernel_stat kbuf;

	    result = syscall(SYS_fstat, fd, &kbuf);
	    myxstat_conv (&kbuf, (struct stat *) buf);
	}
#else
	    result = syscall(SYS_fstat, fd, buf);
#endif	/* PORTNAME_linux */
#else	/* SYS_fstat */
	    result = noNativeCallError ("fstat");
#endif	/* SYS_fstat */
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int fstat ( int fd, struct stat *buf)
{
    int result;

    result = myfstat ( fd, (void *) buf, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int _fstat ( int fd, struct stat *buf)
{
    int result;

    result = myfstat ( fd, (void *) buf, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __fstat ( int fd, struct stat *buf)
{
    int result;

    result = myfstat ( fd, (void *) buf, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int fstat64 ( int fd, struct stat64 *buf)
{
    int result;

    result = myfstat ( fd, (void *) buf, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int _fstat64 ( int fd, struct stat64 *buf)
{
    int result;

    result = myfstat ( fd, (void *) buf, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __fstat64 ( int fd, struct stat64 *buf)
{
    int result;

    result = myfstat ( fd, (void *) buf, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __fxstat ( int ver, int fd, struct stat *buf)
{
    int result;

    result = myfstat ( fd, (void *) buf, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __fxstat64 ( int ver, int fd, struct stat64 *buf)
{
    int result;

    result = myfstat ( fd, (void *) buf, IS_64_BIT);
    return (result);
}


#ifdef c_plus_plus
extern "C" 
#endif
int mygetdents ( int fd, void *buf, size_t nbyte, int is64Flag)
{
    int result;

    if (fd < 0 || fd >= SRB_FD_BIT *2) {
        errno = EBADF;
        return -1;
    }

    if(fd & SRB_FD_BIT) {
        int myfd = fd & (~SRB_FD_BIT);
        if (srbFdArray[fd].fd <= 0) {
            errno = EBADF;
            return -1;
        }

       if (!prelConn) {
            errno = ENOTCONN;
            result = -1;
        }

#ifdef LOCAL_FUNCT
	result = srbGetdents (fd, buf, nbyte, is64Flag);
#else
        if (is64Flag == IS_64_BIT) {
	    result = srbObjGetdents64 (prelConn, MDAS_CATALOG, myfd, buf, 
	     nbyte);
#if defined(PORTNAME_linux)
	    if (result > 0)
	        result = srb64ToLinuxDirent64 ((void *) buf, result);
#endif
	} else {
	    result = srbObjGetdents (prelConn, MDAS_CATALOG, myfd, buf, 
	     nbyte);
#if defined(PORTNAME_linux)
	    if (result > 0) 
	        result = srbToLinuxDirent ((void *) buf, result);
#endif
	}
#endif
    } else {
#if defined(PORTNAME_linux)
	/* SYS_getdents struct is the same as the normal dirent_t
	 * with no d_type. have to adjust len,
	 */
	nbyte = nbyte / 2;
        if (nbyte < sizeof(dirent_t))
            nbyte = nbyte * 2;
#endif	/* PORTNAME_linux */

	if (is64Flag == IS_64_BIT) {
#ifdef SYS_getdents64
	    result = syscall(SYS_getdents64, fd, buf, nbyte);
#else	/* SYS_getdents64 */
#if defined(PORTNAME_linux)
	    result = syscall(SYS_getdents, fd, buf, nbyte);
	    if (result > 0)
	    	result = srbToSrbDirent64 (buf, result);
#else	/* PORTNAME_linux */
	    result = noNativeCallError ("getdents64");
#endif	/* PORTNAME_linux */
#endif	/* SYS_getdents64 */
	} else {
#ifdef SYS_getdents
	    result = syscall(SYS_getdents, fd, buf, nbyte);
#else	/* SYS_getdents */
	    result = noNativeCallError ("getdents");
#endif	/* PORTNAME_linux */
	}
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int getdents ( int fd, dirent_t *buf, size_t nbyte)
{
    int result;

    result = mygetdents ( fd, (void *) buf, nbyte, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int _getdents ( int fd, dirent_t *buf, size_t nbyte)
{
    int result;

    result = mygetdents ( fd, (void *) buf, nbyte, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __getdents ( int fd, dirent_t *buf, size_t nbyte)
{
    int result;

    result = mygetdents ( fd, (void *) buf, nbyte, IS_32_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int getdents64 ( int fd, dirent64_t *buf, size_t nbyte)
{
    int result;

    result = mygetdents ( fd, (void *) buf, nbyte, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int _getdents64 ( int fd, dirent64_t *buf, size_t nbyte)
{
    int result;

    result = mygetdents ( fd, (void *) buf, nbyte, IS_64_BIT);
    return (result);
}

#ifdef c_plus_plus
extern "C"
#endif
int __getdents64 ( int fd, dirent64_t *buf, size_t nbyte)
{
    int result;

    result = mygetdents ( fd, (void *) buf, nbyte, IS_64_BIT);
    return (result);
}

#if defined(PORTNAME_linux)
/* XXXXXXXX - need to implement these readdir() calls because they
 * call __getdents which are internal calls.
 */

#ifdef c_plus_plus
extern "C"
#endif
struct dirent *
readdir (DIR *dirp)
{
  dirent_t *mydp;

  struct localDirent *dp;	/* local dirent definition */

  __libc_lock_lock (dirp->lock);

  do
    {
      size_t reclen;

      if (dirp->offset >= dirp->size)
        {
          /* We've emptied out our buffer.  Refill it.  */

          size_t maxread;
          ssize_t bytes;

#ifndef _DIRENT_HAVE_D_RECLEN
          /* Fixed-size struct; must read one at a time (see below).  */
          maxread = sizeof *dp;
#else
          maxread = dirp->allocation;
#endif

          bytes = mygetdents ( dirp->fd, (void *) dirp->data, maxread, 
	   IS_32_BIT);
/*
          bytes = __getdents64 (dirp->fd, dirp->data, maxread);
*/
          if (bytes <= 0)
            {
              dp = NULL;
              break;
            }
	  /* bytes = srbToLinuxDirent ((void *) dirp->data, bytes); */
          dirp->size = (size_t) bytes;

          /* Reset the offset into the buffer.  */
          dirp->offset = 0;
        }

      dp = (struct localDirent*) &dirp->data[dirp->offset];

#ifdef _DIRENT_HAVE_D_RECLEN
      reclen = dp->d_reclen;
#else
      /* The only version of `struct dirent64' that lacks `d_reclen'
         is fixed-size.  */
      assert (sizeof dp->d_name > 1);
      reclen = sizeof *dp;
      /* The name is not terminated if it is the largest possible size.
         Clobber the following byte to ensure proper null termination.  We
         read jst one entry at a time above so we know that byte will not
         be used later.  */
      dp->d_name[sizeof dp->d_name] = '\0';
#endif

      dirp->offset += reclen;

#ifdef _DIRENT_HAVE_D_OFF
      dirp->filepos = dp->d_off;
#else
      dirp->filepos += reclen;
#endif

      /* Skip deleted files.  */
    } while (dp->d_ino == 0);

  __libc_lock_unlock (dirp->lock);

  return (struct dirent *) dp;
}

#ifdef c_plus_plus
extern "C"
#endif
#if defined(PORTNAME_linux)
struct dirent64 *
#else
struct dirent *
#endif
readdir64 (DIR *dirp)
{
  dirent64_t *mydp;	/* this the dirent defined in SRB env */

  struct localDirent64 *dp;	/* local dirent definition */

  __libc_lock_lock (dirp->lock);

  do
    {
      size_t reclen;

      if (dirp->offset >= dirp->size)
        {
          /* We've emptied out our buffer.  Refill it.  */

          size_t maxread;
          ssize_t bytes;

#ifndef _DIRENT_HAVE_D_RECLEN
          /* Fixed-size struct; must read one at a time (see below).  */
          maxread = sizeof *dp;
#else
          maxread = dirp->allocation;
#endif

          bytes = mygetdents ( dirp->fd, (void *) dirp->data, maxread, 
	   IS_64_BIT);
/*
          bytes = __getdents64 (dirp->fd, dirp->data, maxread);
*/
          if (bytes <= 0)
            {
              dp = NULL;
              break;
            }
	  /* bytes = srb64ToLinuxDirent64 ((void *) dirp->data, bytes); */
          dirp->size = (size_t) bytes;

          /* Reset the offset into the buffer.  */
          dirp->offset = 0;
        }

      dp = (struct localDirent64 *) &dirp->data[dirp->offset];

#ifdef _DIRENT_HAVE_D_RECLEN
      reclen = dp->d_reclen;
#else
      /* The only version of `struct dirent64' that lacks `d_reclen'
         is fixed-size.  */
      assert (sizeof dp->d_name > 1);
      reclen = sizeof *dp;
      /* The name is not terminated if it is the largest possible size.
         Clobber the following byte to ensure proper null termination.  We
         read jst one entry at a time above so we know that byte will not
         be used later.  */
      dp->d_name[sizeof dp->d_name] = '\0';
#endif

      dirp->offset += reclen;

#ifdef _DIRENT_HAVE_D_OFF
      dirp->filepos = dp->d_off;
#else
      dirp->filepos += reclen;
#endif

      /* Skip deleted files.  */
    } while (dp->d_ino == 0);

  __libc_lock_unlock (dirp->lock);

#if defined(PORTNAME_linux)
  return (struct dirent64 *) dp;
#else
  return (struct dirent *) dp;
#endif
}

int
srbToLinuxDirent (void * dptr, int nbytes)
{
    dirent_t *mydp;   		/* dirent in SRB env */
    struct localDirent *dp;		/* local dirent definition */
    char *cp, *mycp, *origMycp;
    int myi = 0;
    int i = 0;
    /* 8 bytes diff in size */
    int diffSz = sizeof (dirent64_t) - sizeof (dirent_t);

    origMycp = malloc (nbytes);
    mycp = origMycp;
    memcpy ((void *)mycp, dptr, nbytes);
    memset (dptr, '\0', nbytes);
    cp = (char *) dptr;

    while (myi < nbytes) {
	int reclen;
	dp = (struct localDirent *) cp;
	mydp = (dirent_t *) mycp;
	dp->d_ino = mydp->d_ino;
	dp->d_off = mydp->d_off;
	reclen = mydp->d_reclen;
       if (reclen <= 0)
            break;
	dp->d_reclen = reclen + diffSz;
	/* dp->d_type = 0; */
	strcpy (dp->d_name, mydp->d_name);
	myi += reclen;
        i += (reclen + diffSz);
	cp += (reclen + diffSz);
	mycp += reclen;
    } 
    free (origMycp);
    return (i);
}
    
int
srbToSrbDirent64 (void * dptr, int nbytes)
{
    dirent_t *mydp;   		/* dirent in SRB env */
    dirent64_t *dp;		/* local dirent definition */
    char *cp, *mycp, *origMycp;
    int myi = 0;
    int i = 0;
    /* 8 bytes diff in size */
    int diffSz = sizeof (dirent64_t) - sizeof (dirent_t);

    origMycp = malloc (nbytes);
    mycp = origMycp;
    memcpy ((void *)mycp, dptr, nbytes);
    memset (dptr, '\0', nbytes);
    cp = (char *) dptr;

    while (myi < nbytes) {
	int reclen, localReclen;
	dp = (dirent64_t *) cp;
	mydp = (dirent_t *) mycp;
	dp->d_ino = mydp->d_ino;
	dp->d_off = mydp->d_off;
	reclen = mydp->d_reclen;
	if (reclen <= 0)
	    break;
	dp->d_reclen = reclen + diffSz;
	strcpy (dp->d_name, mydp->d_name);
	myi += reclen;
	i += (reclen + diffSz);
	cp += (reclen + diffSz);
	mycp += reclen;
    } 
    free (origMycp);
    return (i);
}
    
int
srb64ToLinuxDirent64 (void * dptr, int nbytes)
{
    dirent64_t *mydp;  		/* dirent in SRB env */
    struct localDirent64 *dp;		/* local dirent definition */
    char *cp, *mycp, *origMycp;
    int myi = 0;

    origMycp = malloc (nbytes);
    mycp = origMycp;
    memcpy ((void *)mycp, dptr, nbytes);
    memset (dptr, '\0', nbytes);
    cp = (char *) dptr;

    while (myi < nbytes) {
	int reclen;

	dp = (struct localDirent64 *) cp;
	mydp = (dirent64_t *) mycp;
	/* zero out the upper byte. may have the \0 from previous d_name */
	dp->d_ino = mydp->d_ino & 0xffffffffffffff00;
	dp->d_off = mydp->d_off;
	reclen = mydp->d_reclen;
	dp->d_reclen = reclen;
	dp->d_type = 0;
	strcpy (dp->d_name, mydp->d_name);
	myi += reclen;
	cp += reclen;
	mycp += reclen;
    } 
    free (origMycp);
    return (myi);
}
    
/* need this because euidaccess call __access which is not accessible from 
 * outside the library.
 */
#ifdef c_plus_plus
extern "C"
#endif
int
euidaccess ( const char* path, int amode)
{
    int result;

    result = myaccess (path, amode);
    return result;
}

#endif	/* PORTNAME_linux */

#ifdef c_plus_plus
extern "C" 
#endif
int myaccess ( const char* path, int  amode)
{
    int result;

    char myOutPath[MAX_TOKEN];
    char resource[MAX_TOKEN];

    if (isSrbPath ((char *) path, resource, myOutPath) > 0) {
	/* XXXXXXX - Just return 0 for now */
	result = 0;
    } else {
#ifdef SYS_access
	result = syscall(SYS_access, myOutPath, amode);
#else
	result = noNativeCallError ("access");
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C" 
#endif
int access ( const char* path, int  amode)
{
    int result;

    result = myaccess (path, amode);
    return result;
}

#ifdef c_plus_plus
extern "C" 
#endif
int _access ( const char* path, int  amode)
{
    int result;

    result = myaccess (path, amode);
    return result;
}

#ifdef c_plus_plus
extern "C" 
#endif
int __access ( const char* path, int  amode)
{
    int result;

    result = myaccess (path, amode);
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int ioctl ( int fd, int request, ... )
{
    va_list preload_arglist;
    int mode;
    int result;
    int i, myInput[4];

    va_start(preload_arglist,request);

    for (i = 0; i < 4; i++) {
	if (preload_arglist == NULL)
	    break;
	myInput[i] = va_arg (preload_arglist, int);
    }


    if (fd < 0 || fd >= SRB_FD_BIT *2) {
        errno = EBADF;
        return -1;
    }

    if(fd & SRB_FD_BIT) {
        /* XXXXXXX - Just return 0 for now */
	fprintf (stderr, "ioctl: fd=%x, request=%x\n", fd, request);
        result = 0;
    } else {
#ifdef SYS_ioctl
        result = syscall(SYS_ioctl, fd, request, 
	 myInput[0], myInput[1], myInput[2], myInput[3]);
#else
	result = noNativeCallError ("ioctl");
#endif
    }
    return result;

}

#ifdef c_plus_plus
extern "C"
#endif
int fcntl ( int fd, int cmd, ... )
{
    va_list preload_arglist;
    int mode;
    int result;
    int i, myInput[4];

    va_start(preload_arglist,cmd);

    for (i = 0; i < 4; i++) {
	if (preload_arglist == NULL)
	    break;
	myInput[i] = va_arg (preload_arglist, int);
    }


    if (fd < 0 || fd >= SRB_FD_BIT *2) {
        errno = EBADF;
        return -1;
    }

    if(fd & SRB_FD_BIT) {
        /* XXXXXXX - Just return 0 for now */
        result = 0;
    } else {
#ifdef SYS_fcntl
        result = syscall(SYS_fcntl, fd, cmd, 
	 myInput[0], myInput[1], myInput[2], myInput[3]);
#else
	result = noNativeCallError ("fcntl");
#endif
    }
    return result;

}

#ifdef c_plus_plus
extern "C"
#endif
int _fcntl ( int fd, int cmd, ... )
{
    va_list preload_arglist;
    int result;
    int i, myInput[4];

    va_start(preload_arglist,cmd);

    for (i = 0; i < 4; i++) {
        if (preload_arglist == NULL)
            break;
        myInput[i] = va_arg (preload_arglist, int);
    }

    result = fcntl (fd, cmd, myInput[0], myInput[1], myInput[2], myInput[3]);
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int __fcntl ( int fd, int cmd, ... )
{
    va_list preload_arglist;
    int result;
    int i, myInput[4];

    va_start(preload_arglist,cmd);

    for (i = 0; i < 4; i++) {
        if (preload_arglist == NULL)
            break;
        myInput[i] = va_arg (preload_arglist, int);
    }

    result = fcntl (fd, cmd, myInput[0], myInput[1], myInput[2], myInput[3]);
    return result;
}

#if !defined(PORTNAME_linux)
#ifdef c_plus_plus
extern "C" 
#endif
int acl ( const char* path, int  cmd,  int  nentries,   aclent_t *aclbufp)
{
    int result;

    char myOutPath[MAX_TOKEN];
    char resource[MAX_TOKEN];
    struct stat *buf;

    if (isSrbPath ((char *) path, resource, myOutPath) > 0) {
	if (cmd == GETACL) {
	    if (nentries <= 0) 
	    	return 0;
            if (!prelConn)
            	prelConn = connToSrb ();

            if(prelConn) {
		result = srbStat (myOutPath, buf, IS_UNKNOWN);

		if (result < 0) {
		    errno = ENOENT;
		    return -1;
		}
		    
	    	aclbufp[0].a_type = USER_OBJ;
	    	aclbufp[0].a_id = buf->st_uid;
	    	aclbufp[0].a_perm = 0;
	    	if (buf->st_mode & S_IRUSR)
		    aclbufp[0].a_perm |= S_IROTH;
	    	if (buf->st_mode & S_IWUSR)
		    aclbufp[0].a_perm |= S_IWOTH;
	    	if (buf->st_mode & S_IXUSR)
		    aclbufp[0].a_perm |= S_IXOTH;

	    	if (nentries == 1)
	    	    return 1;

	    	aclbufp[1].a_type = GROUP_OBJ;
	    	aclbufp[1].a_id = buf->st_gid;
	    	aclbufp[1].a_perm = 0;
	    	if (buf->st_mode & S_IRGRP)
		    aclbufp[1].a_perm |= S_IROTH;
	    	if (buf->st_mode & S_IWGRP)
		    aclbufp[1].a_perm |= S_IWOTH;
	    	if (buf->st_mode & S_IXGRP)
		    aclbufp[1].a_perm |= S_IXOTH;
	    
	    	if (nentries == 2)
	    	    return 2;

	    	aclbufp[2].a_type = CLASS_OBJ;
	    	aclbufp[2].a_id = -1;
	    	aclbufp[2].a_perm = 0777;

	    	if (nentries == 3)
	    	    return 3;

	    	aclbufp[3].a_type = OTHER_OBJ;
	    	aclbufp[3].a_id = -1;
	    	aclbufp[3].a_perm = 0;
	    	if (buf->st_mode & S_IROTH)
		    aclbufp[3].a_perm |= S_IROTH;
	    	if (buf->st_mode & S_IWOTH)
		    aclbufp[3].a_perm |= S_IWOTH;
	    	if (buf->st_mode & S_IXGRP)
		    aclbufp[3].a_perm |= S_IWOTH;

	    	return 4;
            } else {
                errno = ENOTCONN;
                result = -1;
            }
	} else if (cmd == GETACLCNT) {
	    return 4;
	} else {
	    return nentries;
	}
    } else {
#ifdef SYS_acl
	result = syscall(SYS_acl, myOutPath, cmd, nentries, aclbufp);
#else
	result = noNativeCallError ("acl");
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C" 
#endif
int facl ( int fd, int  cmd,  int  nentries,   aclent_t *aclbufp)
{
    int result;

    if (fd < 0 || fd >= SRB_FD_BIT *2) {
        errno = EBADF;
        return -1;
    }

    if(fd & SRB_FD_BIT) {
        int myfd = fd & (~SRB_FD_BIT);
        if (srbFdArray[fd].fd <= 0) {
            errno = EBADF;
            return -1;
        }

	result = acl (srbFdArray[fd].path, cmd, nentries, aclbufp );
        if (result < 0) {
            setErrno (result);
            result = -1;
        }
    } else {
#ifdef SYS_facl
	result = syscall(SYS_facl, fd, cmd, nentries, aclbufp);
#else
	result = noNativeCallError ("facl");
#endif
    }
    return result;
}
#endif          /* PORTNAME_linux */

#ifdef c_plus_plus
extern "C" 
#endif
void myexit( int status )
{
    if(prelConn) {
	clFinish (prelConn);
	prelConn = 0;
    }
    fflush (stdout);
    fflush (stderr);
#ifdef SYS_exit
    syscall(SYS_exit,status);
#else
    result = noNativeCallError ("exit");
#endif
}

#ifdef c_plus_plus
extern "C" 
#endif
void exit( int status )
{
    myexit( status );
}

#ifdef c_plus_plus
extern "C" 
#endif
void _exit( int status )
{
    myexit( status );
}

#ifdef c_plus_plus
extern "C" 
#endif
void __exit( int status )
{
    myexit( status );
}

#ifdef c_plus_plus
extern "C" 
#endif
void ___exit( int status )
{
    myexit( status );
}

int 
setErrno (int result)
{
    if ((result <= UNIX_ERR_PREFIX - UNIX_ERR_START) && 
     (result >= UNIX_ERR_START - UNIX_ERR_END)) {
	errno = -result + UNIX_ERR_PREFIX;
    } else if ((result <= HPSS_ERR_PREFIX + HPSS_ERR_END) && 
     (result >= HPSS_ERR_PREFIX + HPSS_ERR_START)) {
	errno = -result + HPSS_ERR_PREFIX;
    } else if (result == MCAT_INQUIRE_ERROR || 
     result == COLLECTION_NOT_IN_CAT) {
	errno = ENOENT;
    } else {
        errno = EINVAL;
    }
    return (errno);
}

int
getCollDesc() 
{
    int i;

    /* search for an unused desc from top */
    for (i = SRB_FD_BIT * 2 - 1; i >= SRB_FD_BIT ; i--) {
	if (srbFdArray[i].fd == 0) {
	    srbFdArray[i].fd = i;
	    srbFdArray[i].isDir = IS_DIR_1;
	    srbFdArray[i].maxDirLen = 0;
	    srbFdArray[i].offset = 0;
	    srbFdArray[i].accumOffset = 0;
	    srbFdArray[i].gotData = 0;
	    srbFdArray[i].continueIndex = -1;
	    srbFdArray[i].dirent = NULL;
	    srbFdArray[i].path[0] = '\0'; 
	    return i & ~SRB_FD_BIT;
	}
    }
    errno = EMFILE;
    return -1;
}

int myPutenv (char *keyWd, char *myValue)
{
    int tmpLen;
    char *tmpPtr;

    tmpLen = strlen (keyWd) + strlen (myValue) + 8;
    tmpPtr = malloc (tmpLen);
    if (strcmp (keyWd, SRB_CWD) == 0) 
    	sprintf (tmpPtr,  "%s=%s%s", keyWd, SRB_ROOT1, myValue);
    else
        sprintf (tmpPtr,  "%s=%s", keyWd, myValue);
    putenv (tmpPtr);

    return 0;
}

#ifdef c_plus_plus
extern "C" 
#endif
int myutime (const char *path, const struct utimbuf *buf)
{
    int result;
    char resource[MAX_TOKEN];
    char myOutPath[MAX_TOKEN];

    if (isSrbPath ((char *) path, resource, myOutPath) > 0) {
	return 0;	/* XXXXXX - nothing done for now */
    } else {
#ifdef SYS_utime
	result = syscall(SYS_utime, myOutPath, buf);
#else
	result = noNativeCallError ("utime");
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int utime (const char *path, const struct utimbuf *buf)
{
    return myutime ( path, buf);
}

#ifdef c_plus_plus
extern "C"
#endif
int _utime (const char *path, const struct utimbuf *buf)
{
    return myutime ( path, buf);
}

#ifdef c_plus_plus
extern "C" 
#endif
int mychmod (const char *path, mode_t mode)
{
    int result;
    char resource[MAX_TOKEN];
    char myOutPath[MAX_TOKEN];

    if (isSrbPath ((char *) path, resource, myOutPath) > 0) {
	return 0;	/* XXXXXX - nothing done for now */
    } else {
#ifdef SYS_chmod
	result = syscall(SYS_chmod, myOutPath, mode);
#else
	result = noNativeCallError ("chmod");
#endif
    }
    return result;
}

#ifdef c_plus_plus
extern "C"
#endif
int chmod (const char *path, mode_t mode)
{
    return mychmod ( path, mode);
}

#ifdef c_plus_plus
extern "C"
#endif
int _chmod (const char *path, mode_t mode)
{
    return mychmod ( path, mode);
}

