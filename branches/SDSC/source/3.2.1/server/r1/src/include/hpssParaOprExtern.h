#ifndef HPSS_PARA_OPR_EXTERN_H
#define HPSS_PARA_OPR_EXTERN_H
#ifdef PARA_OPR

extern srb_long_t
paraHPSStoFileCopy (char *destPath, char *destResLoc, int destSystemType,
char *srcPath, char *srcResLoc, int srcSystemType, srb_long_t size);
extern srb_long_t
paraFiletoHPSSCopy (char *destPath, char *destResLoc, int destSystemType,
char *srcPath, char *srcResLoc, int srcSystemType, srb_long_t size);
extern srb_long_t
paraHPSSPut (char *destPath, char *destResLoc, int destSystemType,
char *clHostAddr, int clPort, srb_long_t size);
extern srb_long_t
paraHPSSGet (char *srcPath, char *srcResLoc, int systemType,
char *clHostAddr, int clPort);
extern srb_long_t
paraHPSStoFAPCopy (int operation, char *destPath, char *destResLoc,
int destSystemType, char *srcPath, char *srcResLoc, int srcSystemType,
srb_long_t size, char *clHostAddr, int clPort);
extern srb_long_t
paraFAPtoHPSSCopy (int operation, char *destPath, char *destResLoc,
int destSystemType, char *srcPath, char *srcResLoc, int srcSystemType,
srb_long_t size, char *clHostAddr, int clPort);
#endif  /* PARA_OPR */
extern void
writeRelay(struct dBuffer *dBuf);
extern void
readRelay(struct dBuffer *dBuf);
extern int initDBuffer (struct dBuffer *dBuf, pthread_mutex_t *condMutex,
pthread_cond_t  *myCondition, struct moverInput *myMoverInput);
#endif  /* HPSS_PARA_OPR_H */

