/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * srbServerSfoExtern.h
 */

#ifndef SRB_SERVER_SFO_EXTERN
#define SRB_SERVER_SFO_EXTERN

#include "packStruct.h"


extern struct varlena *
svrSfoCreateIndex (int class, int catType, int intUnused2, int intUnused3,
char *proxyAddr, char *inIndexName, char *outIndexName, char *resourceName,
char *strUnused4, struct varlena *bsUnused1, 
struct varlena *bsUnused2);
extern struct varlena *
svrSfoDeleteIndex (int class, int catType, int intUnused2, int intUnused3,
char *proxyAddr, char *indexName, char *strUnused2, char *strUnused3,
char *strUnused4, struct varlena *bsUnused1, 
struct varlena *bsUnused2);
extern struct varlena *
svrSfoSearchIndex (int class, int maxSegCount, int maxBufSz, int intUnused3,
char *proxyAddr, char *indexName, char *strUnused2, char *strUnused3,
char *strUnused4,struct varlena *packedQuery, struct varlena *bsUnused1);
extern struct varlena *
svrSfoGetMoreSearchResult (int class, int maxSegCount, int maxBufSz,
int continueIndex,
char *proxyAddr, char *strUnused1, char *strUnused2, char *strUnused3,
char *strUnused4, struct varlena *bsUnused0, struct varlena *bsUnused1); 
extern struct varlena *
svrSfoApplyFilter (int class, int maxSegCount, int maxBufSz, int filterID,
char *proxyAddr, char *filterArg, char *strUnused2, char *strUnused3,
char *strUnused4, struct varlena *segInput, struct varlena *bsUnused1);
extern struct varlena *
svrSfoGetMoreFilterResult (int class, int maxSegCount, int maxBufSz, 
int continueIndex,
char *proxyAddr, char *strUnused1, char *strUnused2, char *strUnused3,
char *strUnused4, struct varlena *bsUnused0, struct varlena *bsUnused1);
extern int
svrFreeIndexResults (indexSearchResult *myresult);
extern int
svrFreeFilterResults(filterDataResult *myresult);
extern int
svrFreeSegmentInfo(segmentInfo *seg_i);

#endif 	/* SRB_SERVER_SFO_EXTERN */
