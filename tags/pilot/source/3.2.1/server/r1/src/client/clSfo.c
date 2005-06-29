/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


#include "clSfo.h"

int
sfoCreateIndex (srbConn *conn, sfoClass myClass, int catType,
char *hostName, char *inIndexName, char *outIndexName, char *resourceName)
{
    int status;
    char *myResource;
    
    if (resourceName != NULL) {
        myResource = resourceName;
    } else {
        if (!(myResource = getenv("mdasResourceName"))) {
            myResource = DefMdasResourceName;
        }
    }

    status = srbObjProxyOpr (conn, SFO_CREATE_INDEX, myClass, catType, 0, 0, 
     hostName, inIndexName, outIndexName, resourceName,
      NULL, NULL, NULL, NULL);

    return status;
}

int
sfoDeleteIndex (srbConn *conn, sfoClass myClass, int catType,
char *hostName, char *indexName)
{
    int status;

    status = srbObjProxyOpr (conn, SFO_DELETE_INDEX, myClass, catType, 0, 0, 
     hostName, indexName, NULL, NULL,
      NULL, NULL, NULL, NULL);

    return status;
}

int 
sfoSearchIndex (srbConn *conn, sfoClass myClass, char *hostName, char *indexName,
void *query, indexSearchResult **myresult, int maxSegCount)
{
    int status;
    struct varlena *packedQuery;
    char *outBuf;
    byteStream queryBS;

    status = packMsg ((char *) query, &packedQuery, sfoMBR_PF, NULL);
    
    if (status < 0) 
        return (status);

    queryBS.len = VARSIZE (packedQuery) - VARHDRSZ;
    queryBS.bytes = VARDATA(packedQuery);
    
    outBuf = malloc (RESULT_BUF_SZ);
    status = srbObjProxyOpr (conn, SFO_SEARCH_INDEX, 
     myClass, maxSegCount, RESULT_BUF_SZ, 0, 
      hostName, indexName, NULL, NULL, NULL,
       &queryBS, NULL, outBuf);
    
    free (packedQuery);

    if (status < 0) {
	free (outBuf);
	return status;
    }

    status = unpackMsg (outBuf, (char **) myresult, indexSearchResult_PF,
     NULL);

    free (outBuf);

    return (status);
}
    
int
sfoGetMoreSearchResult (srbConn *conn, sfoClass myClass, char *hostName,
int continueIndex, indexSearchResult **myresult, int maxSegCount)
{
    char *outBuf;
    int status;

    outBuf = malloc (RESULT_BUF_SZ);
    status = srbObjProxyOpr (conn, SFO_GET_MORE_SEARCH_RESULT,
     myClass, maxSegCount, RESULT_BUF_SZ, continueIndex,
      hostName, NULL, NULL, NULL,
       NULL, NULL, NULL, outBuf);

    if (status < 0) {
        free (outBuf);
        return status;
    }

    status = unpackMsg (outBuf, (char **) myresult, indexSearchResult_PF,
     NULL);

    free (outBuf);

    return (status);
}

int
sfoApplyFilter (srbConn *conn, sfoClass myClass, char *hostName,
int filterID, char *filterArg, int numberOfInputSeg, segmentInfo *inputSeg,
filterDataResult **myresult, int maxSegCount)
{
    int status;
    struct varlena *packedSeg;
    char *outBuf;
    byteStream segBS;
    indexSearchResult mySegInput;

    mySegInput.segmentCount = numberOfInputSeg;
    mySegInput.segments = inputSeg;
    mySegInput.continueIndex = -1;	/* not used */

    status = packMsg ((char *) &mySegInput, &packedSeg, indexSearchResult_PF, 
     NULL);

    if (status < 0)
        return (status);

    segBS.len = VARSIZE (packedSeg) - VARHDRSZ;
    segBS.bytes = VARDATA(packedSeg);

    outBuf = malloc (RESULT_BUF_SZ);
    status = srbObjProxyOpr (conn, SFO_APPLY_FILTER,
     myClass, maxSegCount, RESULT_BUF_SZ, filterID,
      hostName, filterArg, NULL, NULL, NULL,
       &segBS, NULL, outBuf);

    free (packedSeg);

    if (status < 0) {
        free (outBuf);
        return status;
    }

    status = unpackMsg (outBuf, (char **) myresult, filterDataResult_PF,
     NULL);

    free (outBuf);

    return (status);
}

int
sfoGetMoreFilterResult (srbConn *conn, sfoClass myClass, char *hostName, 
int continueIndex,
filterDataResult **myresult, int maxSegCount)
{
    char *outBuf;
    int status;

    outBuf = malloc (RESULT_BUF_SZ);
    status = srbObjProxyOpr (conn, SFO_GET_MORE_FILTER_RESULT,
     myClass, maxSegCount, RESULT_BUF_SZ, continueIndex,
      hostName, NULL, NULL, NULL,
       NULL, NULL, NULL, outBuf);

    if (status < 0) {
        free (outBuf);
        return status;
    }

    status = unpackMsg (outBuf, (char **) myresult, filterDataResult_PF,
     NULL);

    free (outBuf);

    return (status);
}

int 
sfoFreeIndexResults (indexSearchResult *myresult)
{
    int i,res;

    if (NULL==myresult) return 0;

    if (myresult->segments!=NULL) {
    	for (i=0;i<myresult->segmentCount;i++) {
      	    res = sfoFreeSegmentInfo(&(myresult->segments[i]));
      	    if (res < 0) return res;
    	}
    	free(myresult->segments);
    }
    free (myresult);
    return (0);
}

int 
sfoFreeFilterResults(filterDataResult *myresult)
{
    int i, res;

    if (NULL==myresult) return 0;

    if (myresult->segments!=NULL) {
    	for (i=0;i<myresult->segmentDataCount;i++) {
      	    if (myresult->segments[i].segment!=NULL)
        	free(myresult->segments[i].segment);
      	    res = sfoFreeSegmentInfo(&(myresult->segments[i].segInfo));
      	    if (res < 0) return res;
    	}
    	free(myresult->segments);
    }
    free (myresult);

    return 0;
}

int 
sfoFreeSegmentInfo(segmentInfo *seg_i)
{
    if (seg_i == NULL)
	return 0;

    if (seg_i->segmentMBR.minD != NULL) {
    	free(seg_i->segmentMBR.minD);
    	seg_i->segmentMBR.minD = NULL;
    }
    if (seg_i->segmentMBR.maxD!=NULL) {
    	free(seg_i->segmentMBR.maxD);
    	seg_i->segmentMBR.maxD = NULL;
    }
    if (seg_i->objID!=NULL) {
    	free(seg_i->objID);
    	seg_i->objID = NULL;
    }
    if (seg_i->collectionName!=NULL) {
    	free(seg_i->collectionName);
    	seg_i->collectionName = NULL;
    }
    return 0;
}

