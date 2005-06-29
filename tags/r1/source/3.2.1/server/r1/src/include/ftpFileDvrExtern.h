/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* ftpFileDvrExtern.h - Definitions and prototypes for ftpFileDvr.c
 */

#ifndef FTP_FILE_DVR_EXTERN_H
#define FTP_FILE_DVR_EXTERN_H

extern int ftpOpen(char *fileName, int fileFlags, int fileMode);
extern int ftpCreate(char *fileName, int fileMode);
extern int ftpUnlink(char *filename);
extern int ftpStat(char *filename, char *statbuf);
extern int ftpClose(int file);
extern int ftpRead(int file, char *buffer, int amount);
extern int ftpWrite(int file, char *buffer, int amount);
extern srb_long_t ftpSeek(int file, long offset, int whence);
extern int ftpSync(int file);
extern int ftpMkdir(char *filename, int mode);
extern int ftpChmod(char *filename, int mode);
extern int ftpRmdir(char *filename);
extern int ftpOpendir (char *dirname, void **sysDepDirP);
extern int ftpClosedir (void *sysDepDir);
extern int ftpReaddir (void *sysDepDir, struct srbDirent *srbDirent);
extern int ftpSetStorAttri(char *userAddr, char *hostAddr, char *dirPath);

#endif	/* FTP_FILE_DVR_EXTERN_H */
