/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * adrProxyOprExtern.h
 */

#ifndef ADR_PROXY_OPR_EXTERN
#define ADR_PROXY_OPR_EXTERN

extern int
adrSfoCreateIndex (int catType, char *inIndexName, char *outIndexName,
char *resourceName);
extern int
adrSfoDeleteIndex (int catType, char *indexName);
extern int
adrSfoSearchIndex (char *indexName, rangeQuery *query,
indexSearchResult *myresult, int maxSegCount);
extern int
adrSfoGetMoreSearchResult (int continueIndex, indexSearchResult *myresult,
int maxSegCount);
extern int
adrSfoApplyFilter (int filterID, char *filterArg, int numberOfInputSeg,
segmentInfo *inputSeg, filterDataResult *myresult,int maxSegCount);
extern int
adrSfoGetMoreFilterResult (int continueIndex, filterDataResult *myresult,
int maxSegCount);

#endif 	/* ADR_PROXY_OPR_EXTERN */
