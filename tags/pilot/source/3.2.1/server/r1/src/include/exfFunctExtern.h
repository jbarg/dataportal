/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* exfFunctExtern.h - Definitions and prototype for exfFunct.c
 */

#ifndef EXF_FUNCT_EXTERN_H
#define EXF_FUNCT_EXTERN_H

#include "srbStat.h"

/* definition for pathType which are used by chdir () and mkVaultDir () */

#define FILE_T  0               /* The path is a file */
#define DIR_T   1               /* The path is a directory */

extern void exfinit();
extern char *getPathByType (int storSysType);
extern void initVault();
extern char *getFilePath (int storSysType, char *filePath1);
extern char *matchVault (int storSysType, char *filePath);

extern int svrFileCreate(int storSysType, struct varlena *hostAddr,
struct varlena *filename, int mode, int chdirFlag, srb_long_t *size);
extern int _svrFileCreate (int storSysType, char *hostAddr, char *fileName,
int mode, int chdirFlag, srb_long_t size, int remoteConnFlag);
extern int svrFileOpen(int storSysType, struct varlena *hostAddr,
struct varlena *filename, int flags, int mode, int chdirFlag);
extern int _svrFileOpen(int storSysType, char *hostAddr, char *fileName,
int flags, int mode, int chdirFlag, int remoteConnFlag);
extern int svrFileClose(int fd_inx);
extern int svrFileUnlink(int storSysType, struct varlena *hostAddr, 
struct varlena *filename, int chdirFlag);
extern int _svrFileUnlink(int storSysType, char *hostAddr, 
char *filename, int chdirFlag);
extern struct varlena *svrFileRead(int fd, int len);
extern int svrFileWrite(int fd, struct varlena *wbuf);
extern int _svrFileWrite(int fd_inx, char *buf, int len);
extern srb_long_t *svrFileSeek(int fd_inx, srb_long_t *offset, int whence);
extern srb_long_t _svrFileSeek(int fd_inx, srb_long_t offset, int whence);
extern int svrFileSync(int fd_inx);
extern struct varlena * svrFileStat(int storSysType, struct varlena *hostAddr, 
struct varlena *filename);
extern struct varlena *
svrFileFstat(int storSysType, struct varlena *hostAddr, int fd);
extern int svrFileMkdir(int storSysType, struct varlena *hostAddr,
struct varlena *filename, int mode, int chdirFlag);
extern int _svrFileMkdir(int storSysType, char *hostAddr, char *filename,
int mode, int chdirFlag);
extern int svrFileChmod(int storSysType, struct varlena *hostAddr,
struct varlena *filename, int mode, int chdirFlag);
extern int _svrFileChmod (int storSysType, char *hostAddr, char *filename,
int mode, int chdirFlag);
extern int svrFileRmdir(int storSysType, struct varlena *hostAddr,
struct varlena *filename, int chdirFlag);
extern int _svrFileRmdir(int storSysType, char *hostAddr, 
char *filename, int chdirFlag);

extern int svrOpendir(int storSysType, struct varlena *hostAddr, 
struct varlena *dirname, int chdirFlag);
extern int _svrOpendir(int storSysType, char *hostAddr, 
char *dirname, int chdirFlag);
extern int svrClosedir(int fd_inx);
extern struct varlena *svrReaddir(int fd_inx);

extern int svrSetStorAttri(int storSysType, struct varlena *hostAddr,
struct varlena *userAuth, struct varlena *dirPath);

extern struct varlena *svrVaultInfo ();
extern struct varlena *svrGetVersion ();
extern int
_svrFileStat(int storSysType, char *hostAddrBuf, char *portNum, char *filename,
struct srbStat *retStat);
extern int 
_svrFileFstat(int storSysType, char *hostAddrBuf, char *portNum,
int fd, struct srbStat *retStat);
extern srb_long_t _fileWriteSize (int fd_inx);
extern int
svrFileMigrate(int fd, int srcLevel, int flags);
extern int
svrFileStage(int fd, int offset, int len, int srcLevel, int flags);
extern int
svrFilePurge(int fd, int offset, int len, int srcLevel, int flags);
extern int
chkdir (int storSysType, char *hostAddrBuf, char *fileNameBuf,
int chkFlag, int pathType);
struct varlena *
svrFileChksum (int fd_inx, int flag, srb_long_t *offset, srb_long_t *size);
int
_svrFileChksum (int fd_inx, int flag, char *chksum, 
srb_long_t myOffset, srb_long_t mySize);
int
svrFileSum (int fd_inx, unsigned long *ichksum, 
srb_long_t myOffset, srb_long_t mySize);
srb_long_t *
svrGetFsFreeSpace (int storSysType, struct varlena *hostAddr,
struct varlena  *vPath, int flag);
srb_long_t
_svrGetFsFreeSpace (int storSysType, char *hostAddr, char *path, int flag);
int
_setFileWriteFlag (int fd_inx, int writeFlag);

#endif	/* EXF_FUNCT_EXTERN_H */
