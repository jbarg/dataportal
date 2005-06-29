/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 

/*
 * srbServerSfo.c - Sfo routines for the srbServer
 */

#include "srbServerSfo.h"

struct varlena *
svrSfoCreateIndex (int class, int catType, int intUnused2, int intUnused3,
char *proxyAddr, char *inIndexName, char *outIndexName, char *resourceName,
char *strUnused4, struct varlena *bsUnused1, struct varlena *bsUnused2)
{
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (proxyAddr != NULL) {
        remoteFlag = chkHost (proxyAddr, NULL, &hostTabPtr);
        if (remoteFlag < 0) {
            elog (NOTICE, "svrSfoCreateIndex: Invalid host addr %s", proxyAddr);
             return (genIntRet (INP_ERR_HOST_ADDR));
        } else if (remoteFlag == REMOTE_HOST) { /* remote proxy */
            status = remoteProxyOpr (hostTabPtr, SFO_CREATE_INDEX,
             class, catType, intUnused2, intUnused3,
              proxyAddr, inIndexName, outIndexName, resourceName,
               NULL, NULL, NULL, NULL);
            return (genIntRet (status));
        }
    }

    switch (class) {
#ifdef ADR_PROXY
	case DataCutterCl:
	    status = adrSfoCreateIndex (catType, inIndexName, outIndexName, 
	     resourceName);
	    break;
#endif
	default:
	     elog(NOTICE, "svrSfoCreateIndex: sfoClass %d is not supported",
	      class);
	    status = OBJ_ERR_SFO_CLASS;
	    break;
    }

    return (genIntRet (status));
}

struct varlena *
svrSfoDeleteIndex (int class, int catType, int intUnused2, int intUnused3,
char *proxyAddr, char *indexName, char *strUnused2, char *strUnused3,
char *strUnused4, struct varlena *bsUnused1, struct varlena *bsUnused2)
{
    int status;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (proxyAddr != NULL) {
        remoteFlag = chkHost (proxyAddr, NULL, &hostTabPtr);
        if (remoteFlag < 0) {
            elog (NOTICE, "svrSfoDeleteIndex: Invalid host addr %s", proxyAddr);
             return (genIntRet (INP_ERR_HOST_ADDR));
        } else if (remoteFlag == REMOTE_HOST) { /* remote proxy */
            status = remoteProxyOpr (hostTabPtr, SFO_DELETE_INDEX,
             class, catType, intUnused2, intUnused3,
              proxyAddr, indexName, strUnused2, strUnused3,
               NULL, NULL, NULL, NULL);
            return (genIntRet (status));
        }
    }

    switch (class) {
#ifdef ADR_PROXY
	case DataCutterCl:
	    status = adrSfoDeleteIndex (catType, indexName);
	    break;
#endif
	default:
	     elog(NOTICE, "svrSfoDeleteIndex: sfoClass %d is not supported",
	      class);
	    status = OBJ_ERR_SFO_CLASS;
	    break;
    }

    return (genIntRet (status));
}

struct varlena *
svrSfoSearchIndex (int class, int maxSegCount, int maxBufSz, int intUnused3,
char *proxyAddr, char *indexName, char *strUnused2, char *strUnused3,
char *strUnused4, struct varlena *packedQuery, struct varlena *bsUnused1) 
{
    int status;
    rangeQuery *query;
    indexSearchResult myresult;
    struct varlena *packedResult, *retval;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (proxyAddr != NULL) {
        remoteFlag = chkHost (proxyAddr, NULL, &hostTabPtr);
        if (remoteFlag < 0) {
            elog (NOTICE, "svrSfoSearchIndex: Invalid host addr %s", proxyAddr);
            retval = genIntRet (INP_ERR_HOST_ADDR);
             return retval;
        } else if (remoteFlag == REMOTE_HOST) { /* remote proxy */
            byteStream myBS;

            myBS.len = VARSIZE (packedQuery) - VARHDRSZ;
            myBS.bytes = VARDATA (packedQuery);

            packedResult = (struct varlena *) malloc (maxBufSz + VAROUTHDRSZ);
            status = remoteProxyOpr (hostTabPtr, SFO_SEARCH_INDEX,
             class, maxSegCount, maxBufSz, intUnused3,
              proxyAddr, indexName, strUnused2, strUnused3, strUnused4,
               &myBS, NULL, (char *) VAROUTDATA (packedResult));
            if (status < 0) {
                free (packedResult);
                retval = genIntRet (status);
                return (retval);
            }

	    if (status > maxBufSz) {
        	elog(NOTICE,
       		 "svrSfoSearchIndex: Result size %d overflows client buf %d",
                  status, maxBufSz);
                free (packedResult);
                return (genIntRet (OBJ_ERR_SFO_BUF_OVERFLOW));
    	    }
            VARSIZE (packedResult) = status;
	    VAROUTSTAT(packedResult) = htonl (VARSIZE (packedResult));
	    return (packedResult);
        }
    }

    status = unpackMsg (VAROUTDATA (packedQuery), (char **) &query, sfoMBR_PF, 
     NULL);
    
    if (status < 0) {
	elog(NOTICE, 
	 "svrSfoSearchIndex: unpackMsg() of query error, status = %d", status);
	return (genIntRet (status));
    }

    switch (class) {
#ifdef ADR_PROXY
        case DataCutterCl:
            status = adrSfoSearchIndex (indexName, (rangeQuery *) query,
	     &myresult, maxSegCount);
            break;
#endif
        default:
             elog(NOTICE, "svrSfoSearchIndex: sfoClass %d is not supported",
              class);
            status = OBJ_ERR_SFO_CLASS;
            break;
    }

    free (query);

    if (status < 0) {
        elog(NOTICE, 
	 "svrSfoSearchIndex: adrSfoSearchIndex () error, status = %d", status);
        return (genIntRet (status));
    }

    status = packMsg ((char *) &myresult, &packedResult, indexSearchResult_PF,
     NULL);

    svrFreeIndexResults (&myresult);
    if (status < 0) {
        elog(NOTICE, 
	 "svrSfoSearchIndex: Error packing indexSearchResult, status = %d", 
	 status);
        return (genIntRet (status));
    }

    if (VARSIZE (packedResult) > maxBufSz) {
	elog(NOTICE,
	 "svrSfoSearchIndex: The result size %d overflows client buf size %d",
	  VARSIZE (packedResult), maxBufSz);
	free (packedResult);
	return (genIntRet (OBJ_ERR_SFO_BUF_OVERFLOW));
    }
    VAROUTSTAT(packedResult) = htonl (VARSIZE (packedResult));

    return (packedResult);
}

struct varlena *
svrSfoGetMoreSearchResult (int class, int maxSegCount, int maxBufSz, 
int continueIndex, 
char *proxyAddr, char *strUnused1, char *strUnused2, char *strUnused3,
char *strUnused4, struct varlena *bsUnused0, struct varlena *bsUnused1)
{
    int status;
    indexSearchResult myresult;
    struct varlena *packedResult, *retval;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (proxyAddr != NULL) {
        remoteFlag = chkHost (proxyAddr, NULL, &hostTabPtr);
        if (remoteFlag < 0) {
            elog (NOTICE, 
             "svrSfoGetMoreSearchResult: Invalid host addr %s", proxyAddr);
            retval = genIntRet (INP_ERR_HOST_ADDR);
             return retval;
        } else if (remoteFlag == REMOTE_HOST) { /* remote proxy */

            packedResult = (struct varlena *) malloc (maxBufSz + VAROUTHDRSZ);
            status = remoteProxyOpr (hostTabPtr, SFO_GET_MORE_SEARCH_RESULT,
             class, maxSegCount, maxBufSz, continueIndex,
              proxyAddr, strUnused1, strUnused2, strUnused3,
               NULL, NULL, NULL, (char *) VAROUTDATA (packedResult));
            if (status < 0) {
                free (packedResult);
                retval = genIntRet (status);
                return (retval);
            }
	    if (status > maxBufSz) {
	        elog(NOTICE,
	         "svrSfoGetMoreSearchResult: Result size %d overflows client buf %d",
	          status, maxBufSz);
	        return (genIntRet (OBJ_ERR_SFO_BUF_OVERFLOW));
	    }
            VARSIZE (packedResult) = status;
	    VAROUTSTAT(packedResult) = htonl (VARSIZE (packedResult));
	    return (packedResult);
        }
    }

    switch (class) {
#ifdef ADR_PROXY
        case DataCutterCl:
    	    status = adrSfoGetMoreSearchResult (continueIndex, &myresult, 
	     maxSegCount);
	    break;
#endif
        default:
            elog(NOTICE, 
	     "adrSfoGetMoreSearchResult: sfoClass %d is not supported", class);
            status = OBJ_ERR_SFO_CLASS;
            break;
    }

    if (status < 0) {
        elog(NOTICE,
         "svrSfoGetMoreSearchResult error, status = %d", status);
        return (genIntRet (status));
    }

    status = packMsg ((char *) &myresult, &packedResult, indexSearchResult_PF,
     NULL);

    svrFreeIndexResults (&myresult);

    if (status < 0) {
        elog(NOTICE,
         "svrSfoGetMoreSearchResult: Error packing indexSearchResult, status = %d",
         status);
        return (genIntRet (status));
    }

    if (VARSIZE (packedResult) > maxBufSz) {
        elog(NOTICE,
         "svrSfoGetMoreSearchResult: The result size %d overflows client buf size %d",
          VARSIZE (packedResult), maxBufSz);
        return (genIntRet (OBJ_ERR_SFO_BUF_OVERFLOW));
    }
    VAROUTSTAT(packedResult) = htonl (VARSIZE (packedResult));
    return (packedResult);
}

struct varlena *
svrSfoApplyFilter (int class, int maxSegCount, int maxBufSz, int filterID,
char *proxyAddr, char *filterArg, char *strUnused2, char *strUnused3,
char *strUnused4, struct varlena *segInput, struct varlena *bsUnused1)
{
    int status;
    indexSearchResult *unpackedSegInput;
    filterDataResult myresult;
    struct varlena *packedResult, *retval;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (proxyAddr != NULL) {
        remoteFlag = chkHost (proxyAddr, NULL, &hostTabPtr);
        if (remoteFlag < 0) {
            elog (NOTICE, "svrSfoApplyFilter: Invalid host addr %s", proxyAddr);
            retval = genIntRet (INP_ERR_HOST_ADDR);
             return retval;
        } else if (remoteFlag == REMOTE_HOST) { /* remote proxy */
	    byteStream myBS;

	    myBS.len = VARSIZE (segInput) - VARHDRSZ;
	    myBS.bytes = VARDATA (segInput);

	    packedResult = (struct varlena *) malloc (maxBufSz + VAROUTHDRSZ);
            status = remoteProxyOpr (hostTabPtr, SFO_APPLY_FILTER,
             class, maxSegCount, maxBufSz, filterID,
              proxyAddr, filterArg, strUnused2, strUnused3, strUnused4,
               &myBS, NULL, (char *) VAROUTDATA (packedResult));
	    if (status < 0) {
		free (packedResult);
            	retval = genIntRet (status);
            	return (retval);
	    }
	    if (status > maxBufSz) {
	        elog(NOTICE,
	         "svrSfoSearchIndex: Result size %d overflows client buf %d",
	          status, maxBufSz);
	        free (packedResult);
	        return (genIntRet (OBJ_ERR_SFO_BUF_OVERFLOW));
	    }
	    VARSIZE (packedResult) = status;
	    VAROUTSTAT(packedResult) = htonl (VARSIZE (packedResult));
	    return (packedResult);
        }
    }

    status = unpackMsg (VAROUTDATA (segInput), (char **) &unpackedSegInput, 
     indexSearchResult_PF, NULL);

    if (status < 0) {
        elog(NOTICE,
         "svrSfoApplyFilter: unpackMsg() of segInput error, status = %d", 
	 status);
        return (genIntRet (status));
    }

    switch (class) {
#ifdef ADR_PROXY
        case DataCutterCl:
            status = adrSfoApplyFilter (filterID, filterArg, 
	     unpackedSegInput->segmentCount, unpackedSegInput->segments, 
	      &myresult, maxSegCount);
            break;
#endif
        default:
             elog(NOTICE, "svrSfoApplyFilter: sfoClass %d is not supported",
              class);
            status = OBJ_ERR_SFO_CLASS;
            break;
    }

    free (unpackedSegInput);
    if (status < 0) {
        elog(NOTICE,
         "svrSfoApplyFilter: adrSfoSearchIndex () error, status = %d", status);
        return (genIntRet (status));
    }

    status = packMsg ((char *) &myresult, &packedResult, filterDataResult_PF, 
     NULL);

    svrFreeFilterResults (&myresult);

    if (status < 0) {
        elog(NOTICE,
         "svrSfoApplyFilter: Error packing filterDataResult, status = %d",
         status);
        return (genIntRet (status));
    }

    if (VARSIZE (packedResult) > maxBufSz) {
        elog(NOTICE,
         "svrSfoSearchIndex: The result size %d overflows client buf size %d",
          VARSIZE (packedResult), maxBufSz);
	free (packedResult);
        return (genIntRet (OBJ_ERR_SFO_BUF_OVERFLOW));
    }
    VAROUTSTAT(packedResult) = htonl (VARSIZE (packedResult));

    return (packedResult);
}

struct varlena *
svrSfoGetMoreFilterResult (int class, int maxSegCount, int maxBufSz, 
int continueIndex,
char *proxyAddr, char *strUnused1, char *strUnused2, char *strUnused3,
char *strUnused4, struct varlena *bsUnused0, struct varlena *bsUnused1)
{
    int status;
    filterDataResult myresult;
    struct varlena *packedResult, *retval;
    int remoteFlag;
    struct hostElement  *hostTabPtr;

    if (proxyAddr != NULL) {
        remoteFlag = chkHost (proxyAddr, NULL, &hostTabPtr);
        if (remoteFlag < 0) {
            elog (NOTICE, "svrSfoApplyFilter: Invalid host addr %s", proxyAddr);
            retval = genIntRet (INP_ERR_HOST_ADDR);
             return retval;
        } else if (remoteFlag == REMOTE_HOST) { /* remote proxy */
            packedResult = (struct varlena *) malloc (maxBufSz + VAROUTHDRSZ);
            status = remoteProxyOpr (hostTabPtr, SFO_GET_MORE_FILTER_RESULT,
             class, maxSegCount, maxBufSz, continueIndex,
              proxyAddr, strUnused1, strUnused2, strUnused3,
               NULL, NULL, NULL, (char *) VAROUTDATA (packedResult));
            if (status < 0) {
                free (packedResult);
                retval = genIntRet (status);
                return (retval);
            }
	    if (VARSIZE (packedResult) > maxBufSz) {
	        elog(NOTICE,
	         "adrSfoGetMoreFilterResult: Result size %d overflows client buf %d",
	          status, maxBufSz);
	        free (packedResult);
	        return (genIntRet (OBJ_ERR_SFO_BUF_OVERFLOW));
	    }
            VARSIZE (packedResult) = status;
	    VAROUTSTAT(packedResult) = htonl (VARSIZE (packedResult));

	    return (packedResult);
        }
    }

    switch (class) {
#ifdef ADR_PROXY
	case DataCutterCl:
    	    status = adrSfoGetMoreFilterResult (continueIndex, &myresult, 
     	     maxSegCount);
            break;
#endif
        default:
            elog(NOTICE, 
	      "svrSfoGetMoreFilterResult: sfoClass %d is not supported", class);
            status = OBJ_ERR_SFO_CLASS;
            break;
    }


    if (status < 0) {
        elog(NOTICE,
         "svrSfoGetMoreFilterResult: adrSfoGetMoreFilterResult () error, status = %d", status);
        return (genIntRet (status));
    }

    status = packMsg ((char *) &myresult, &packedResult, filterDataResult_PF, 
     NULL);

    svrFreeFilterResults (&myresult);

    if (status < 0) {
        elog(NOTICE,
         "adrSfoGetMoreFilterResult: Error packing filterDataResult, status = %d",
         status);
        return (genIntRet (status));
    }

    if (VARSIZE (packedResult) > maxBufSz) {
        elog(NOTICE,
         "adrSfoGetMoreFilterResult: The result size %d overflows client buf size %d",
          VARSIZE (packedResult), maxBufSz);
        free (packedResult);
        return (genIntRet (OBJ_ERR_SFO_BUF_OVERFLOW));
    }
    VAROUTSTAT(packedResult) = htonl (VARSIZE (packedResult));

    return (packedResult);
}

int
svrFreeIndexResults (indexSearchResult *myresult)
{
    int i,res;

    if (NULL==myresult) return 0;

    if (myresult->segments!=NULL) {
        for (i=0;i<myresult->segmentCount;i++) {
            res = svrFreeSegmentInfo(&(myresult->segments[i]));
            if (res < 0) return res;
        }
        free(myresult->segments);
    }
    return (0);
}

int
svrFreeFilterResults(filterDataResult *myresult)
{
    int i, res;

    if (NULL==myresult) return 0;

    if (myresult->segments!=NULL) {
        for (i=0;i<myresult->segmentDataCount;i++) {
            if (myresult->segments[i].segment!=NULL)
                free(myresult->segments[i].segment);
            res = svrFreeSegmentInfo(&(myresult->segments[i].segInfo));
            if (res < 0) return res;
        }
        free(myresult->segments);
    }

    return 0;
}

int
svrFreeSegmentInfo(segmentInfo *seg_i)
{
    if (seg_i == NULL)
        return 0;

    if (seg_i->segmentMBR.minD != NULL) {
        free(seg_i->segmentMBR.minD);
        seg_i->segmentMBR.minD = NULL;
    }
    if (seg_i->segmentMBR.maxD !=NULL) {
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

