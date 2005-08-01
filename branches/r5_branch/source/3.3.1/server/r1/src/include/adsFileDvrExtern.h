
/* adsFileDvrExtern.h - Definitions and prototypes for adsFileDvr.c
 */

#ifndef ADS_FILE_DVR_EXTERN_H
#define ADS_FILE_DVR_EXTERN_H

#include "srbDirent.h"

extern int adsOpen(char *fileName, int fileFlags, int fileMode);
extern int adsCreate(char *fileName, int fileMode, srb_long_t dummySize);
extern int adsUnlink(char *filename);
extern int adsStat(char *filename, struct stat *statbuf);
extern int adsFstat (int ts, struct stat *statbuf);
extern int adsClose(int ts);
extern int adsRead(int ts, char *buffer, int amount);
extern int adsWrite(int ts, char *buffer, int amount);
extern srb_long_t adsSeek(int ts, srb_long_t offset, int whence);
extern int adsSync(int ts);
extern int adsStat(char *filename, struct stat *statbuf);
extern int adsMkdir(char *filename, int mode);
extern int adsChmod(char *filename, int mode);
extern int adsRmdir(char *filename);
extern int adsOpendir (char *dirname, void **sysDepDirP);
extern int adsClosedir (void *sysDepDir);
extern int adsReaddir (void *sysDepDir, struct srbDirent *srbDirentP);

#endif	/* ADS_FILE_DVR_EXTERN_H */
