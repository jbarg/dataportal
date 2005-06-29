/* ntFileDvrExtern.h */
#ifndef __ntFileDvrExtern_h__
#define __ntFileDvrExtern_h__

#if defined(__cplusplus)
extern "C"
{
#endif

int  ntOpen(char *fileName, int fileFlags, int fileMode);
int  ntCreate(char *fileName, int fileMode, int dummySize);
int  ntClose(int file);
int  ntRead(int file, char *buffer, int amount);
int  ntWrite(int file, char *buffer, int amount);
long ntSeek(int file, long offset, int whence);
int  ntSync(int file);
int  ntUnlink(char *filename);
int  ntStat(char *filename, struct stat *statbuf);
int  ntMkdir(char *filename, int mode);
int ntFstat(int fd, struct stat *statbuf);  
int  ntChmod (char *filename, int mode);
int  ntRmdir (char *filename);
int  ntOpendir (char *dirname, void **sysDepDirP);
int  ntClosedir (void *sysDepDir);
int  ntReaddir (void *sysDepDir, struct srbDirent *srbDirent);

#if defined(__cplusplus)
}
#endif


#endif