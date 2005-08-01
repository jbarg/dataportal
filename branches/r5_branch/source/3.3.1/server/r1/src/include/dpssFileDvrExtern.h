/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
#ifndef DPSS_FILE_DVR_EXTERN_H
#define DPSS_FILE_DVR_EXTERN_H
 
#ifdef DPSS

#include "srbDirent.h"

extern int dpssSrbOpen(char *fileName, int fileFlags, int fileMode);
extern int dpssSrbClose(int file);
extern int dpssSrbRead(int file, char *buffer, int amount);
extern int dpssSrbWrite(int file, char *buffer, int amount);
extern long dpssSrbSeek(int file, long offset, int whence);
extern long dpssSrbTell(int file);
extern int dpssSrbSync(int file);
extern int dpssSrbCreate(char *fileName, int fileMode, int size);
extern int dpssSrbUnlink(char *filename);
extern int dpssSrbStat(char *filename, struct stat *statbuf);
extern int dpssSrbMkdir(char *filename, int mode);
extern int dpssSrbChmod(char *filename, int mode);
extern int dpssSrbRmdir(char *filename);
extern int dpssSrbOpendir (char *dirname, void **sysDepDirP);
extern int dpssSrbClosedir (void *sysDepDir);
extern int dpssSrbReaddir (void *sysDepDir, struct srbDirent *srbDirent);

/**  These are not defined in exfSw.h
extern int dpssSrbErrConv (int status);
extern int initHpssCos ();
extern int getCosBySize (int size);
*/

#endif /* DPSS */
 
#endif  /* DPSS_FILE_DVR_EXTERN_H */

