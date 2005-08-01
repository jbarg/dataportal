
#ifndef SRB_PRELOAD
#define SRB_PRELOAD

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#if defined(PORTNAME_solaris)
#include <sys/acl.h>
#endif
#if defined(PORTNAME_linux)
#include <features.h>
#include <bits/pthreadtypes.h>
#include "dirstream.h"
#include <dirent.h>
/* confusing on what dirent is */
struct localDirent {
    __ino_t d_ino;
    __off_t d_off;
    unsigned short int d_reclen;
    /* unsigned char d_type; */
    char d_name[256];           /* We must not include limits.h! */
  };

struct localDirent64 {
    __ino64_t d_ino;
    __off64_t d_off;
    unsigned short d_reclen;
    unsigned char d_type;
    char d_name[256];           /* We must not include limits.h! */
  };

#endif


#include "srbPrelMisc.h"

/* IRIX has sys/syscall.h, but it redefines
   SYSVOffset to give incorrect numbers! */

#ifndef sgi
#include <sys/syscall.h>
#else
#include <sys.s>
#endif

/* HPUX does not have dlopen/dlclose! */

#ifndef hpux
#include <dlfcn.h>
#endif

#undef open
#undef creat
#undef close
#undef read
#undef write
#undef lseek
#undef stat
#undef fstat
#undef lstat
#undef getdents
#undef exit


/*
#define FD_ARRAY_SIZE 1024
#define SRB_FD_BIT	 0x10000
*/
#define SRB_FD_BIT	 0x80

#define BUFF_SZ			1024*16
#ifdef __cplusplus
extern "C" {
#endif

int noNativeCallError( char *callStr );

#ifdef __cplusplus
}
#endif

#ifdef c_plus_plus
extern "C" {
#endif
int myopen( const char* path, int flags, int mode, int is64Flag);
int open( const char* path, int flags, ... );
int _open( const char* path, int flags, ... );
int __open( const char* path, int flags, ... );
int ___open( const char* path, int flags, ... );
int open64( const char* path, int flags, ... );
int _open64( const char* path, int flags, ... );
int __open64( const char* path, int flags, ... );
int ___open64( const char* path, int flags, ... );
#if defined(PORTNAME_linux)
int creat ( const char* path, mode_t mode );
#else
int creat ( const char* path, ulong mode );
#endif
int _creat ( const char* path, ulong mode );
int __creat ( const char* path, ulong mode );
#if !defined(PORTNAME_linux)
int creat64 ( const char* path, ulong mode );
#endif
int _creat64 ( const char* path, ulong mode );
int __creat64 ( const char* path, ulong mode );
int myclose( int fd );
int close( int fd );
int _close( int fd );
int __close( int fd );
int ___close( int fd );
int myunlink (const char *path);
int unlink( const char* path);
int _unlink( const char* path);
int myrmdir (const char *path);
int rmdir( const char* path);
int _rmdir( const char* path);
int mychdir (const char *path);
int chdir( const char* path);
int _chdir( const char* path);
int myfchdir (int fd);
int fchdir( int fd);
int _fchdir(int fd);
char *_mygetcwd ();
char *mygethome ();
char *mygetcwd (char *buf, uint size);
char *getcwd (char *buf, uint size);
char *_getcwd (char *buf, uint size);
int mymkdir (const char *path, mode_t mode);
int mkdir( const char* path,  mode_t mode);
int _mkdir( const char* path,  mode_t mode);
int myread( int fd, void* data, size_t length );
int read( int fd, void* data, size_t length );
int _read( int fd, void* data, size_t length );
int __read( int fd, void* data, size_t length );
int ___read( int fd, void* data, size_t length );
int mywrite( int fd, const void* data, size_t length );
int write( int fd, const void* data, size_t length );
int _write( int fd, const void* data, size_t length );
int __write( int fd, const void* data, size_t length );
int ___write( int fd, const void* data, size_t length );
off_t mylseek( int fd, off_t where, int whence );
off_t lseek( int fd, off_t where, int whence );
off_t _lseek( int fd, off_t where, int whence );
off_t __lseek( int fd, off_t where, int whence );
off_t ___lseek( int fd, off_t where, int whence );
off64_t mylseek64( int fd, off64_t where, int whence );
off64_t lseek64( int fd, off64_t where, int whence );
off64_t _lseek64( int fd, off64_t where, int whence );
off64_t __lseek64( int fd, off64_t where, int whence );
off64_t ___lseek64( int fd, off64_t where, int whence );
int mystat ( const char* path, void *buf, int is64Flag);
int stat ( const char* path, struct stat *buf);
int _stat ( const char* path, struct stat *buf);
int __stat ( const char* path, struct stat *buf);
int stat64 ( const char* path, struct stat64 *buf);
int _stat64 ( const char* path, struct stat64 *buf);
int __stat64 ( const char* path, struct stat64 *buf);
int mylstat ( const char* path, void *buf, int is64Flag);
int lstat ( const char* path, struct stat *buf);
int _lstat ( const char* path, struct stat *buf);
int __lstat ( const char* path, struct stat *buf);
int lstat64 ( const char* path, struct stat64 *buf);
int _lstat64 ( const char* path, struct stat64 *buf);
int __lstat64 ( const char* path, struct stat64 *buf);
int myfsync ( int fd);
int fsync ( int fd);
int myfstat ( int fd, void *buf, int is64Flag);
int fstat ( int fd, struct stat *buf);
int _fstat ( int fd, struct stat *buf);
int __fstat ( int fd, struct stat *buf);
int fstat64 ( int fd, struct stat64 *buf);
int _fstat64 ( int fd, struct stat64 *buf);
int __fstat64 ( int fd, struct stat64 *buf);
int mygetdents ( int fd, void *buf, size_t nbyte, int is64Flag);
int getdents ( int fd, dirent_t *buf, size_t nbyte);
int _getdents ( int fd, dirent_t *buf, size_t nbyte);
int getdents64 ( int fd, dirent64_t *buf, size_t nbyte);
int _getdents64 ( int fd, dirent64_t *buf, size_t nbyte);
int access ( const char* path, int  amode);
int ioctl ( int fd, int request, ... );
int fcntl ( int fd, int cmd, ... );
int _fcntl ( int fd, int cmd, ... );
#if !defined(PORTNAME_linux)
int acl ( const char* path, int  cmd,  int  nentries,   aclent_t *aclbufp);
int facl ( int fd, int  cmd,  int  nentries,   aclent_t *aclbufp);
#endif
void myexit( int status );
void exit( int status );
void _exit( int status );
void __exit( int status );
void ___exit( int status );
#ifdef c_plus_plus
}
#endif

int setErrno (int result);
int getCollDesc();
int myPutenv (char *keyWd, char *myValue);

srbConn *prelConn = NULL;

int CwdFlag = CWD_IS_LOCAL;     /* defualt to local dir firect */

/* srb open file desc array */

struct srbFd srbFdArray[SRB_FD_BIT * 2] = { 0 };

#endif
