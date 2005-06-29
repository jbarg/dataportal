/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* unixFileDvrExtern.h - Definitions and prototypes for unixFileDvr.c
 */

#ifndef UNIX_FILE_DVR_EXTERN_H
#define UNIX_FILE_DVR_EXTERN_H

#include <sys/types.h>
#if defined(PORTNAME_solaris)
#include <sys/statvfs.h>
#endif
#if defined(PORTNAME_linux)
#include <sys/vfs.h>
#endif
#if defined(PORTNAME_aix) || defined(PORTNAME_sgi)
#include <sys/statfs.h>
#endif
#if defined(PORTNAME_osx)
#include <sys/param.h>
#include <sys/mount.h>
#endif

#include "srbDirent.h"

extern int unixOpen(char *fileName, int fileFlags, int fileMode);
extern int unixCreate(char *fileName, int fileMode, srb_long_t dummySize);
extern int unixUnlink(char *filename);
extern int unixStat(char *filename, struct stat *statbuf);
extern int unixFstat (int fd, struct stat *statbuf);
extern int unixClose(int file);
extern int unixRead(int file, char *buffer, int amount);
extern int unixWrite(int file, char *buffer, int amount);
extern srb_long_t unixSeek(int file, srb_long_t offset, int whence);
extern int unixSync(int file);
extern int unixStat(char *filename, struct stat *statbuf);
extern int unixMkdir(char *filename, int mode);
extern int unixChmod(char *filename, int mode);
extern int unixRmdir(char *filename);
extern int unixOpendir (char *dirname, void **sysDepDirP);
extern int unixClosedir (void *sysDepDir);
extern int unixReaddir (void *sysDepDir, struct srbDirent *srbDirent);
extern srb_long_t unixGetFsFreeSpace(char *path, int flag);

#endif	/* UNIX_FILE_DVR_EXTERN_H */
