/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * packStruct.h
 */

#ifndef PACK_STRUCT_H
#define PACK_STRUCT_H

#include "srb.h"
#ifdef TAPE_DRIVE
#include "srbTapeMiscExtern.h"
#endif
#include "packMsgExtern.h"

/* The following are type definitions for DataCutter types */

/* sfoMBR - Bounding box structure */

typedef struct {
    int dim;            /* bounding box dimension */
    double *minD;        /* minimum in each dimension */
    double *maxD;        /* maximum in each dimansion */
} sfoMBR;

typedef struct {
    sfoMBR	segmentMBR;	/* bounding box for the segment */
    char 	*objID;		/* obj in SRB that contains the segment */
    char	*collectionName; /* collection where obj is stored */
    unsigned int offset;	/* offset of the segment in the object */
    unsigned int size;		/* size of segment */
} segmentInfo;	/* segment meta-data information */

typedef struct {
    int		segmentCount;	/* number of segments returned */
    segmentInfo *segments;	/* segment meta-data information */
    int		continueIndex;	/* continuation flag */
} indexSearchResult;	/* search result structure */

typedef sfoMBR rangeQuery;
    
typedef struct {
    segmentInfo	segInfo;
    char *segment;
} segmentData;

typedef struct {
    int segmentDataCount;	/* # of segments in segmentData array */
    segmentData	*segments;	/* segmentData array */
    int		continueIndex;  /* continuation index */
} filterDataResult; 


struct mdasInfoOut {         /* output struct */
    char *objID;
    char *collection;
    char *dataPath;             /* for file type, this is the file path */
    char *resourceLoc;          /* for file type, this is hostaddr:port */
    char *resourceName;         /* resource name, DB_LOBJ specific */
    char *logResName;           /* Logical resource name, container specific */
    char *dataType;             /* data type - text, gif. etc */
    int objTypeInx;             /* Index into the srbObjTypeEntry[] table */
    int replNum;
    int segNum;
    int intReplNum;             /* >= 0 if it is a internal comp obj */
    int intSegNum;             /* >= 0 if it is a internal comp obj */
    srb_long_t offset;
    srb_long_t size;
    int isDirty;
    int chksum;
    char *phyResClass;
    int resClassInx;
    char *containerName;
    srb_long_t containerMaxSize; /* vaild only if containerName != "" */
#ifdef TAPE_DRIVE
    tapeFileInfo_t *tapeFileInfo;      /* valid only for tape resource */
#else
    void *tapeFileInfo;      /* valid only for tape resource */
#endif
    struct mdasInfoOut *compoundLink;
    struct mdasInfoOut *next;
};


typedef struct {
    int numFile;
    void *fileName;	/* point to an array of char [numFile][MAX_DATA_SIZE] */
} fileList_t;

typedef struct {
    int tapeCartPriCnt;    /* Number of tapeCartPri element in array */
    int *cartType;         /* cartType arranged in order of priority */
} tapeCartPri_t; 

typedef struct {
    int numAddr;
    int *portNum;	/* pointer to an int array portNum[numAddr] */
    char *hostAddr;     /* point to an array of char [numAddr][MAX_DATA_SIZE] */
} portList_t;

#endif 	/* PACK_STRUCT_H */
