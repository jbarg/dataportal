/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#ifndef HPSS_FILE_DVR_EXTERN_H
#define HPSS_FILE_DVR_EXTERN_H
 
#ifdef HPSS

#include "srbDirent.h"
#include "srbType.h"

extern int hpssOpen(char *fileName, int fileFlags, int fileMode);
extern int hpssClose(int file);
extern int hpssRead(int file, char *buffer, int amount);
extern int hpssWrite(int file, char *buffer, int amount);
extern srb_long_t hpssSeek(int file, srb_long_t offset, int whence);
extern long hpssTell(int file);
extern int hpssSync(int file);
extern int hpssCreate(char *fileName, int fileMode, srb_long_t size);
extern int hpssUnlink(char *filename);
extern int hpssStat(char *filename, struct stat *statbuf);
extern int hpssFstat(int fd, struct stat *statbuf);
extern int hpssMkdir(char *filename, int mode);
extern int hpssChmod(char *filename, int mode);
extern int hpssRmdir(char *filename);
extern int hpssOpendir (char *dirname, void **sysDepDirP);
extern int hpssClosedir (void *sysDepDir);
extern int hpssReaddir (void *sysDepDir, struct srbDirent *srbDirent);
extern int hpssErrConv (int status);
extern int initHpssCos ();
extern int getCosBySize (srb_long_t size);
extern int hpssMigrate (int fd, int srcLevel, int flags);
extern int hpssStage (int fd, int offset, int len, int srcLevel, int flags);
extern int hpssPurge (int fd, int offset, int len, int srcLevel, int flags);
extern int
_hpssOpen(char *fileName, int fileFlags, int fileMode, srb_long_t size);

#endif /* HPSS */
 
#endif  /* HPSS_FILE_DVR_EXTERN_H */

