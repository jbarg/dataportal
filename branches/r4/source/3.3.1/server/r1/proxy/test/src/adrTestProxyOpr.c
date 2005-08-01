/* Copyright   1999   The Regents of the University of California
 * All Rights Reserved
 *
 * Permission to use, copy, modify and distribute any part of this Storage
 * Resource Broker (SRB) software package, for educational, research and
 * non-profit purposes, without fee, and without a written agreement is
 * hereby granted, provided that the above copyright notice, this paragraph
 * and the following three paragraphs appear in all copies.
 * Those desiring to incorporate this SRB software into commercial products
 * or use for commercial purposes should contact the Technology Transfer
 * Office, University of California, San Diego, 9500 Gilman Drive, La Jolla,
 * CA 92093-0910, Ph: (619) 534-5815, FAX: (619) 534-7345.
 *
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING
 * LOST PROFITS, ARISING OUT OF THE USE OF THIS SRB SOFTWARE, EVEN IF THE
 * UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * THE SRB SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND THE UNIVERSITY
 * OF CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.  THE UNIVERSITY OF CALIFORNIA MAKES NO
 * REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER IMPLIED OR
 * EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, OR THAT THE USE OF THE
 * SRB SOFTWARE WILL NOT INFRINGE ANY PATENT, TRADEMARK OR OTHER RIGHTS.
 */

/*
 * adrProxyOpr.c - Sfo routines for the srbServer
 */

    double myMin0[] = {1.01, 2.02, 3.03};
    double myMax0[] = {10.1, 20.2, 30.3};
    double myMin1[] = {4.01, 5.02, 6.03};
    double myMax1[] = {40.1, 50.2, 60.3};

#include "adrProxyOpr.h"

int
adrSfoCreateIndex (int catType, char *inIndexName, char *outIndexName, 
char *resourceName)
{
    elog(NOTICE, 
     "adrSfoCreateIndex: inIndex=%s, outIndex=%s, catType=%d, resource=%s", 
      inIndexName, outIndexName, catType, resourceName);
    return (0);
}

int
adrSfoDeleteIndex (int catType, char *indexName)
{
    elog(NOTICE, 
     "adrSfoDeleteIndex: indexName=%s, catType=%d", 
      indexName, catType);
    return (0);
}

int
adrSfoSearchIndex (char *indexName, rangeQuery *query, 
indexSearchResult *myresult, int maxSegCount)
{
    elog(NOTICE, 
     "adrSfoSearchIndex: maxSeg=%d, dim=%d, min0=%f, max0=%f, min=%f, max=%f", 
      maxSegCount, query->dim, query->min[0], query->max[0], 
       query->min[query->dim - 1], query->max[query->dim -1]);
    
    fillSearchResult (myresult);

    return (0);
}

int
adrSfoGetMoreSearchResult (int continueIndex, indexSearchResult *myresult, 
int maxSegCount)
{
    elog(NOTICE,
     "adrSfoGetMoreSearchResult: contInx=%d, maxSeg=%d", 
     continueIndex, maxSegCount);

    fillSearchResult (myresult);

    return 0;
}

int
adrSfoApplyFilter (int filterID, char *filterArg, int numberOfInputSeg,
segmentInfo *inputSeg, filterDataResult *myresult,int maxSegCount)
{
    elog(NOTICE,
     "adrSfoApplyFilter: filterID=%d, filterArg=%s, numberOfInputSeg=%d, objId0=%s, objId=%s", filterID, filterArg, numberOfInputSeg, inputSeg[0].objID, inputSeg[numberOfInputSeg - 1].objID);

    fillFilterResult (myresult);
	
    return 0;
}

int
adrSfoGetMoreFilterResult (int continueIndex, filterDataResult *myresult,
int maxSegCount)
{
    elog(NOTICE,
     "adrSfoGetMoreFilterResult: continueIndex=%d, maxSegCount=%d",
      continueIndex, maxSegCount);
    fillFilterResult (myresult);
    return (0);
}

int fillSearchResult (indexSearchResult *myresult)
{
    myresult->segmentCount = 2;
    myresult->segments = (segmentInfo *) malloc
     (sizeof(segmentInfo) * 2);
    myresult->segments[0].segmentMBR.dim = 3;
    myresult->segments[1].segmentMBR.dim = 3;
    myresult->segments[0].segmentMBR.min = myMin0;
    myresult->segments[0].segmentMBR.max = myMax0;
    myresult->segments[1].segmentMBR.min = myMin1;
    myresult->segments[1].segmentMBR.max = myMax1;
    myresult->segments[0].objID = "foo0";
    myresult->segments[1].objID = "foo1";
    myresult->segments[0].collectionName = "/srbtest/bar0";
    myresult->segments[1].collectionName = "/srbtest/bar1";
    myresult->segments[0].offset = 100;
    myresult->segments[1].offset = 200;
    myresult->segments[0].size = 1000;
    myresult->segments[1].size = 2000;
    myresult->continueIndex = -1;

    return (0);
}

int
fillFilterResult (filterDataResult *myresult)
{
    myresult->segmentDataCount = 2;
    myresult->segments = (segmentData *) malloc
     (sizeof(segmentData) * 2);
    myresult->segments[0].segInfo.segmentMBR.dim = 3;
    myresult->segments[1].segInfo.segmentMBR.dim = 3;
    myresult->segments[0].segInfo.segmentMBR.min = myMin0;
    myresult->segments[0].segInfo.segmentMBR.max = myMax0;
    myresult->segments[1].segInfo.segmentMBR.min = myMin1;
    myresult->segments[1].segInfo.segmentMBR.max = myMax1;
    myresult->segments[0].segInfo.objID = "foo0";
    myresult->segments[1].segInfo.objID = "foo1";
    myresult->segments[0].segInfo.collectionName = "/srbtest/bar0";
    myresult->segments[1].segInfo.collectionName = "/srbtest/bar1";
    myresult->segments[0].segInfo.offset = 100;
    myresult->segments[1].segInfo.offset = 200;
    myresult->segments[0].segInfo.size = 1000;
    myresult->segments[1].segInfo.size = 2000;
    myresult->segments[0].segment = "Data Segment 0";
    myresult->segments[1].segment = "Data Segment 1";
    myresult->continueIndex = -1;

    return (0);
}
