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

For calls that might be the initial SRB call, this library will first
call the initSrbClientEnv and srbConnect routines (if they haven't
been already).  For all calls, this library will then call the
corresponding SRB client routine.

For errors, this library will print an SRB error message, set errno
(converting the SRB error to a somewhat corresponding errno value), and
return a negative value; much like the unix IO routines do.

There is a single .h file defining the routines in this library.  So
the build environment for the client application is simple, they need the
srbUio.h file and the libsrbUio.a library file built for the appropriate OS.

For srbUioOpen, it will emulate the Unix Open call in another way:
if the open fails and the flags argument requests O_CREAT, it will then
call srbObjCreate to attempt to create the object.

Note that the srbUioRename routine renames the srb object (file) in the
SRB logical name space.  If you want to move the file's physical location,
use another API, such as the Sphymove command.

See the srbUio.c file for more information.

Wayne Schroeder
Initial version October, 2003
*/

extern int srbUioOpen(char *name, int flags, int mode );
extern int srbUioClose(int fd);
extern long long srbUioSeek(int fd, long long offset, int whence);
extern int srbUioWrite(int fd, void *buf, size_t nbytes);
extern ssize_t srbUioRead(int fd, void *buf, size_t nbytes);

extern int srbUioStat(char *path, struct stat *buf);
extern int srbUioUnlink(char *path);
extern int srbUioRename(char *oldPath, char *newPath); 

extern int srbUioLock(int fd, long long offset, int size, int mode);

