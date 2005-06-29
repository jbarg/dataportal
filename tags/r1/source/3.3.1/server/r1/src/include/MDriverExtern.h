
 


/* MDriverExtern.h - Definitions and prototype for MDriver.c
 */

#ifndef M_DRIVER_EXTERN_H
#define M_DRIVER_EXTERN_H


typedef struct {
    struct hostElement  *hostTabPtr;    /* Pointer to the host table */
    int         remoteFlag;     /* host location, 0 - local, 1 - remote */
    int         MDriverType;    /* type of misc driver */
    int         fd;                /* the file descriptor or lob locator*/
    char*        driverSpecificInfo;   /* pointerto DriverSpecific State Info */
}MDriverDesc ;

/*
 * prototypes for functions in MDriver.c.
 */
extern void initMDriverDesc();
static int newMDriverDesc (struct hostElement *hostTabPtr, int remoteFlag,  int MDriverType, char * driverSpecificInfo, int fd);
static void delMDriverDesc (int i);
/*
extern int MDriverOpen(int MDriverType, struct mdasGetInfoOut *dataInfo, struct varlena *dataPath, int openFlag, int mode);
extern int _MDriverOpen(int MDriverType, struct mdasGetInfoOut *dataInfo, char *dataPath, int openFlag, int mode, int driverUserFlag);
extern int MDriverCreate(int MDriverType, struct mdasResInfo *resourceInfo, struct varlena *dataPath, int mode);
extern int _MDriverCreate(int MDriverType, struct mdasResInfo *resourceInfo, char *dataPath, int mode, int driverUserFlag);
*/
extern int MDriverOpen(int MDriverType,struct varlena  *rsrcLoc, struct varlena *dataPath, int openFlag, int mode);
extern int _MDriverOpen(int MDriverType, char *rsrcLoc, char *dataPath, int openFlag, int mode, int driverUserFlag);
extern int MDriverCreate(int MDriverType, struct varlena *rsrcLoc, struct varlena *dataPath, int mode);
extern int _MDriverCreate(int MDriverType, char *rsrcLoc, char *dataPath, int mode, int driverUserFlag);

extern int MDriverClose (int descInx);
extern struct varlena *MDriverRead(int fd, int len);
extern int _MDriverRead (int descInx, char *buf, int len);
extern int MDriverWrite (int descInx, struct varlena *buf);
extern int _MDriverWrite (int descInx, char *buf, int len);
extern int MDriverSeek (int descInx, long offset, int whence);
extern int MDriverUnlink(int MDriverType, struct varlena  *resourceLoc, struct varlena *dataPath);
extern int _MDriverUnlink(int MDriverType, char *resourceLoc, char *dataPath,int driverUserFlag);
extern int MDriverSync (int descIn);
extern struct varlena *MDriverProc (int descInx,int outLen,
			struct varlena *vprocName,
				    struct varlena *vinBuf);
extern int _MDriverProc (int descInx,char *procName,
			 char *inBuf, int inLen, 
			 char *outBuf, int outLen );

#endif	/* M_DRIVER_EXTERN_H */
