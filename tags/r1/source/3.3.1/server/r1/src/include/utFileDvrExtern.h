/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#ifndef UT_FILE_DVR_EXTERN_H
#define UT_FILE_DVR_EXTERN_H

#ifdef UTREE
#include "srbDirent.h"

extern int utOpen(char *fileName, int fileFlags, int fileMode);
extern int utClose(int file);
extern int utRead(int file, char *buffer, int amount);
extern int utWrite(int file, char *buffer, int amount);
extern long utSeek(int file, long offset, int whence);
extern long utTell(int file);
extern int utSync(int file);
extern int utCreate(char *fileName, int fileMode, int dummySize);
extern int utUnlink(char *filename);
extern int utStat(char *filename, struct stat *statbuf);
extern int utFstat(int fd, struct stat *statbuf);
extern int utMkdir(char *filename, int mode);
extern int utChmod(char *filename, int mode);
extern int utRmdir(char *filename);
extern int utOpendir (char *dirname, void **sysDepDirP);
extern int utClosedir (void *sysDepDir);
extern int utReaddir (void *sysDepDir, struct srbDirent *srbDirent);

#endif /* UTREE */
 
#endif  /* UT_FILE_DVR_EXTERN_H */
