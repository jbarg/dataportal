/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * packDef.h
 */

#ifndef PACK_DEF_H
#define PACK_DEF_H

#include "packMsgExtern.h"


/* The following are type definitions for DataCutter types */

/* The packing format for sfoMBR */
FormatDef sfoMBR_PF = "int dim; double* min[dim]; double* max[dim];";
/* segmentInfo */
FormatDef segmentInfo_PF = "struct sfoMBR_PF; str *objID; str *collectionName; int offset; int size;";
/* indexSearchResult */
FormatDef indexSearchResult_PF = "int segmentCount; struct *segmentInfo_PF[segmentCount]; int continueIndex;";
/* segmentData - have to expand segmentInfo_PF because "size is need */
FormatDef segmentData_PF = "struct sfoMBR_PF; str *objID; str *collectionName; int offset; int size; char *segInfo[size];";
/* filterDataResult */
FormatDef filterDataResult_PF = "int segmentDataCount; struct *segmentData_PF[segmentDataCount]; int continueIndex;";
/* The following are packing format for MCAT results */

/* mdasC_resultinfo of mdasC_db2_externs.h */
FormatDef sqlResultInfo_PF = "str *tab_name[NULL_TERM]; str *att_name[NULL_TERM]; str *values[row_count][MAX_DATA_SIZE];";
/* mdasC_sql_result_struct of mdasC_db2_externs.h */
FormatDef sqlResultStruct_PF = "int result_count; int row_count; struct sqlResultInfo_PF[MAX_SQL_LIMIT]; int continuation_index;";
FormatDef mdasInfoOut_PF = "str *objID[NULL_TERM]; str *collection[NULL_TERM]; str *dataPath[NULL_TERM]; str *resourceLoc[NULL_TERM]; str *resourceName[NULL_TERM]; str *logResName[NULL_TERM]; str *dataType; int objTypeInx; int replNum; int segNum; int intReplNum; int intSegNum; double offset; double size; int isDirty; int chksum; str *phyResClass[NULL_TERM]; int resClassInx; str *containerName[NULL_TERM]; double containerMaxSize; int tapeFileInfo; int compoundLink; int next;"; 

FormatDef fileList_t_PF = "int numFile; str *fileName[numFile][MAX_DATA_SIZE];";
FormatDef portList_t_PF = "int numAddr; int *portNum[numAddr]; str *hostAddr[numAddr][MAX_DATA_SIZE];";
FormatDef tapeCartPri_PF = "int tapeCartPriCnt; int *cartType[tapeCartPriCnt];";
FormatDef genQuery_PF = "int selInxCnt; int qvalCnt; int *selInx[selInxCnt]; int *selVal[selInxCnt]; int *qvalInx[qvalCnt]; str *qval[qvalCnt][MAX_TOKEN];";

FormatDefConst GlobalDef[] = {
        {"MAX_TOKEN", (char *) MAX_TOKEN},
        {"MAX_DATA_SIZE", (char *) MAX_DATA_SIZE},
        {"MAX_SQL_LIMIT", (char *) MAX_SQL_LIMIT},
        {"NULL_TERM", (char *) NULL_TERM}, /* NULL terminated string */
        {"sfoMBR_PF", (char *) &sfoMBR_PF},
        {"segmentInfo_PF", (char *) &segmentInfo_PF},
        {"indexSearchResult_PF", (char *) &indexSearchResult_PF},
        {"segmentData_PF", (char *) &segmentData_PF},
        {"filterDataResult_PF", (char *) &filterDataResult_PF},
        {"sqlResultInfo_PF", (char *) &sqlResultInfo_PF},
        {"sqlResultStruct_PF", (char *) &sqlResultStruct_PF},
        {"mdasInfoOut_PF", (char *) &mdasInfoOut_PF},
        {"fileList_t_PF", (char *) &fileList_t_PF},
        {"tapeCartPri_PF", (char *) &tapeCartPri_PF},
        {"genQuery_PF", (char *) &genQuery_PF},
	{END_DEF_NAME, (char *) END_DEF_VAL},
};

static int nGlobalDef =
    (sizeof(GlobalDef) /sizeof (FormatDefConst));

#endif 	/* PACK_DEF_H */
