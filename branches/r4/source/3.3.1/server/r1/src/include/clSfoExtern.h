/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * clSfoExtern.h
 */

#ifndef CLSFO_EXTERN_H
#define CLSFO_EXTERN_H

#include "srb.h"
#include "clConnectExtern.h"
#include "mdasC_db2_externs.h"
#include "clStubExtern.h"
#include "packMsgExtern.h"
#include "packDefExtern.h"
#include "packStruct.h"
#include "packMsgExtern.h"

/* The SFO class definition */

typedef enum {
    DataCutterCl
} sfoClass;

extern int
sfoCreateIndex (srbConn *conn, sfoClass myClass, int catType,
char *hostName, char *inIndexName, char *outIndexName, char *resourceName);
extern int
sfoDeleteIndex (srbConn *conn, sfoClass myClass, int catType,
char *hostName, char *indexName);
extern int
sfoSearchIndex (srbConn *conn, sfoClass myClass, char *hostName, 
char *indexName,
void *query, indexSearchResult **myresult, int maxSegCount);
extern int
sfoGetMoreSearchResult (srbConn *conn, sfoClass myClass, char *hostName,
int continueIndex, indexSearchResult **myresult, int maxSegCount);
extern int
sfoApplyFilter (srbConn *conn, sfoClass myClass, char *hostName,
int filterID, char *filterArg, int numberOfInputSeg, segmentInfo *inputSeg,
filterDataResult **myresult, int maxSegCount);
extern int
sfoGetMoreFilterResult (srbConn *conn, sfoClass myClass, char *hostName, 
int continueIndex,
filterDataResult **myresult, int maxSegCount);
extern int
sfoFreeIndexResults (indexSearchResult *myresult);
extern int
sfoFreeFilterResults(filterDataResult *myresult);
extern int
sfoFreeSegmentInfo(segmentInfo *seg_i);
#endif 	/* CLSFO_EXTERN_H */
