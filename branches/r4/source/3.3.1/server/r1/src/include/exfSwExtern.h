/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*-------------------------------------------------------------------------
 *
 * exfSwExtern.h--
 *    external file switch public interface declarations.
 *
 *
 *
 *-------------------------------------------------------------------------
 */
#ifndef EXF_SW_EXTERN_H
#define EXF_SW_EXTERN_H

#include "srbDirent.h"

/* define used by exfmgr */

#define EXF_FAIL	0
#define	EXF_SUCCESS	1

#define	DEFAULT_EXF	0

extern int exfcreate(int which, char *where, char *filename, int fileMode,
srb_long_t size);
extern int exfunlink(int which, char *where, char *filename);
extern int exfopen(int which, char *where, char *filename, int fileFlags, int fileMode);
extern int exfclose(int which, char *where, int file);
extern int exfread(int which, char *where, int file,
		    char *buffer, int length);
extern int exfwrite(int which, char *where, int file,
		     char *buffer, int length);
extern int exfsync(int which, char *where, int file);
extern srb_long_t exfseek(int which, char *where, int file, 
srb_long_t offset, int whence);
extern int exfstat(int which, char *where, char *filename, char *statbuf);
extern int exffstat(int which, char *where, int fd, char *statbuf);
extern int exfmkdir(int which, char *where, char *filename, int mode);
extern int exfchmod(int which, char *where, char *filename, int mode);
extern int exfrmdir(int which, char *where, char *filename);
extern int exfrmdir(int which, char *where, char *filename);
extern int exfopendir(int which, char *where, char *dirname, 
void **sysDepDirP);
extern int exfclosedir(int which, char *where, void *sysDepDir);
extern int exfreaddir(int which, char *where, void *sysDepDir, 
struct srbDirent *srbDirent);
extern srb_long_t exfgetFsFreeSpace (int which, char *where, char *path, 
int flag); 
extern int
exffstage(int which, char *where, char *pathName, int flags);
extern int
exfmigrate(int which, char *where, int fd, int srcLevel, int flags);
extern int
exfpurge(int which, char *where, int fd, int offset, int len,
int srcLevel, int flags);
extern int
exfsetstorattri(int which, char *host, char *user, char *dir);
extern int
exfstage(int which, char *where, int fd, int offset, int len, 
int srcLevel, int flags);
int
exfchown(int which, char *where, char *filename, char *fileUser,
char *fileGroup, int chownFlag);
#endif  /* EXF_SW_EXTERN_H */

