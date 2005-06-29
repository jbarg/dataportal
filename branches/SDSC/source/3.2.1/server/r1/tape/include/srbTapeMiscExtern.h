/* srbTapeMiscExtern.h
 */

#ifndef SRB_TAPE_MISC_EXTERN_H
#define SRB_TAPE_MISC_EXTERN_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include "srb.h"
/* #ifdef SRB_SVR_ENV */
#include "elogExtern.h"

/* XXXXXX - take me out */
#ifdef foo
#if defined(PORTNAME_aix)
#define DEF_DEV_PATH "/dev/rmt1"
#else
#define DEF_DEV_PATH "/dev/rmt/0"
#endif
#endif

#define TAPELIB_CONFIG	"tapeLibConfig"		/* The tape library config */
#define TAPE_SEG_SZ	(2147483648) 	/* 2 G segment */
/* #define TAPE_SEG_SZ	(100 * 1024 * 1024) */	/* XXXXX 100 mb for testing */

#if !defined(PORTNAME_aix)
#include <sys/mtio.h>

#define STIOCTOP        MTIOCTOP

#define STOFFL          MTOFFL
#define STREW           MTREW
#define STWEOF          MTWEOF
#define STFSF           MTFSF
#define STRSF           MTBSF
#define STFSR           MTFSR
#define STRSR           MTBSR

#define stop            mtop
#define st_op           mt_op
#define st_count        mt_count
#else
#include <sys/tape.h>
#endif


#define TAPE_OPEN_RETRY		2
#define MAX_FILE_MARK_TO_SEEK   1024

#define	TAPE_DEVNAME_LEN	63
#define	TAPE_NUM_LEN		6
#define	TAPE_TYPE_LEN		16
#define	HOSTNAME_LEN		128
#define	PORT_LEN		8

#define TAPE_DEV_CONFIG         "tapeDevConfig"


/* The mode of the device */

#define READONLY_M	1

typedef struct filePosition {	/* The tape position by end of file mark */
    int partition;		/* the partition number if used */
    int fileSeqNo;		/* The number of EOF mark after whichthis 
				 * this file begins. The tape begin with
				 * a volume lab and then a EOF mark. Real
				 * data starts at recordNo = 1.
				 */
    long long offset;		/* XXXX - should be srb_long_t. 
				 * The relative position from the beginning
				 * of the record. real data start at offset=
				 * sizeof (hdr1Label_t)+sizeof (hdr2Label_t) */
} filePos_t;

#define ABS_POS_SIZE 4

/* Absolute position. Available only to drives such as 3590 */

typedef struct absPosition {
    char address[ABS_POS_SIZE];
} absPos_t;

typedef struct tapePosition {
    absPos_t absPos;
    filePos_t filePos;
} tapePos_t;

/* definition for queCart() call to specify what type of link list,
next or reqLink */
#define SAME_CART_LINK	0
#define NEXT_LINK	1

/* definition for tapeCart_t.retryCnt */

#define MAX_CART_RETRY_CNT	3
typedef struct tapeCart {
    char tapeNum[TAPE_TYPE_LEN];
    int type;           /* see srbTape.h for def, T_3480 or T_3590 */
    int tapeLibInx;	/* under which tape library it is controled by */
    tapePos_t lastPos;
    int state;
    svrComm_t *myComm;	/* The socket to use to reply client's mount req */
    int clientPid;	/* The Pid of the client proccess */
    int priority;
    int retryCnt;
    pthread_t tid;
    pthread_attr_t attr;
    void *sameCartLink;	/* queue of request the ask for the same cart */
    void *next;
} tapeCart_t;

/* This is the output of a getTapeToWrite call */
typedef struct {
    char tapeNum[TAPE_TYPE_LEN];
    srb_long_t eofBytesCnt;
    /* int eofSeqNo; */
    tapePos_t position;
    char mntDevPath[TAPE_DEVNAME_LEN]; /* The device path of the mounted tape */
    int lockFd;
    void *next;
} tapeStat_t;
    
#define TAPE_FULL	-1	/* used in eofSeqNo to indicate the tape is 
				 * full.
				 */
    
#define DRIVE_ADDR_LEN	24

/* Definition for the tape library type */
#define STK_LIB		0

typedef struct tapeDev {
    char devPath[TAPE_DEVNAME_LEN];     /* the /dev/... path */
    char address[DRIVE_ADDR_LEN];	/* For STK, a four number tuple a:b:c:d
					 * representing acs:lsm:panel:drive */
    int libType;			/* the tape lib type, e.g., STK_LIB */
    int drvTypeInx;			/* drv type inx in the tapeDevGroup 
					 * Same as type in TapeMediaCharTable */
    tapeCart_t *mountedCart;
    tapePos_t position;
    int drvInx;                         /* Tape driver inx in tapeDvrTable */
    int blockSz;
    int state;  /* The current state of the device */
    int mode;   /* The mode of the device. e.g., READONLY_M */
    int fd;
    int problemCnt;	/* A count is the number of times it has problem
			 * mounting or dismounting */
    int seqNo;
    time_t startTime;
    void *next;
} tapeDev_t;

#define MAX_DRV_PROB_CNT	5
/* definition for tapeDev_t.state */
#define ST_CLOSED		0
#define ST_OPENED		1
#define ST_WRITE_MODE		2
#define ST_READ_MODE		4
#define ST_IO_READY		8
#define ST_REWINDED		16
#define ST_UNLOADED		32
#define ST_EOT			64	/* eot */
#define ST_MOUNTED		128	/* The drive is mounted with a tape */
#define ST_TLIB_MOUNTED		256	/* mounted by the tapelib */

/* Definition for tapeDev type */
/* The value is also used to index into the TapeDrvConfig table */
#define T_3480	0
#define T_3590  1
#define NUM_DRV_TYPE	2	/* number of drive types. Should equal to
				         * NumTapeMediaChar */

/* This defines the index into the tapeDvrTable[] table */

#define GENERIC_TAPE_DRV        0       /* the generic tape driver */

/* tape media default characteristic */
typedef struct tapeMediaChar {		
    int type;
    char name[TAPE_TYPE_LEN];
    int blkSize;
    unsigned long long capacity; /* XXXX - capacity in kbytes. For some
				  * reason 20 * 1024 * 1024 * 1024 in the
				  * table below overflows the type */
				/* XXXXXX should use srb_long_t later */
    int dvrInx;			/* The drive index to use */
} tapeMediaChar_t;

typedef struct tapeDrvConfig {
    char devPath[TAPE_DEVNAME_LEN];     /* the /dev/... path */
    char address[DRIVE_ADDR_LEN];       /* For STK, a four number tuple a:b:c:d
                                         * representing acs:lsm:panel:drive */
    int drvInx;                         /* Tape driver inx in tapeDvrTable */
    int cartType;                       /* index into the TapeMediaCharTable */
    void *next;
} tapeDrvConfig_t;

/* tape vol label- ISO/ANSI VOL1. From IBM doc */

typedef struct vol1Label {
        char    labelID[3];             /* "VOL" */
        char    labelNo[1];                /* "1" */
        char    tapeNum[TAPE_NUM_LEN];  /*  6 digit tape number */
	char    accessibility[1];	/* space(0x20 or " ") ==> accessible */
        char    reserved1[13];   
        char    implimentID[13];   	/* implementation ID - future use */
        char    ownerID[14];            /* "SDSC SRB" */
        char    Reserved2[28];   
	char    labelStandardLevel[1];	/* "3" */
} vol1Label_t;

#define FILE_ID_LEN	 17
#define FILE_SEGM_LEN	 6
#define MAX_FILE_SEGM_NO 999999
#define FILE_SEQ_LEN	 8
#define MAX_FILE_SEQ_NO  99999999
#define BLOCK_SZ_LEN	 10
#define MAX_BLOCK_SZ     999999999

/* tape header 1 label- ISO/ANSI hdr1. From IBM doc */

typedef struct hdr1Label {
        char  labelID[3];               /* "HDR" */
        char  labelNo[1];               /* "1" */
        char  fileName[FILE_ID_LEN];    /* file name */
        char  fileSegmNo[FILE_SEGM_LEN];/* Normally, it is the fileSetID. But
					 * here used to define the file segment 
					 * number. A file can be divided into */
	char  fileSeqNo[FILE_SEQ_LEN];  /* several segments */
        				/* The standard uses 4 bytes for
					 * fileSectionNo and 4 for fileSeqNo.
					 * but 4 bytes for fileSeqNo is too
					 * small. Will use both fields for
					 * fileSeqNo. */
        char  genNo[4];                 /* 0001 */
        char  versNo[2];                /* 00 */
        char  creationDate[6];          /* cyyddd */
        char  expirationDate[6];        /* 900001 - year 2900  */
        char  accessibility[1];         /* space (0x20 or " ") */
        char  blockCount[6];            /* header label - 000000 */
        char  systemCode[13];           /* all 0 */
	char  tapeNum[6];		/* In the standard, the last 7 bytes
					 * is reserved. We are using 6 bytes
					 * to store the tapeNum */	
        char  reserved1[1];             /* 0 */
} hdr1Label_t;


/* tape header 2 label- ISO/ANSI hdr2. From IBM doc */

typedef struct hdr2Label {
        char    labelID[3];             /* HDR */
        char    labelNo[1];             /* 2 */
        char    recordFormat[1];        /* F - fixed length */
        char    blockSz[10];            /* The standard has 5 bytes for
					 * blockLength and 5 for recordLength.
					 * But 5 digit is too small for the
					 * blockLength of some drives. Use
					 * the whole 10 bytes for blockLength */
        char    reservedOp[35];       
        char    bufferOffset[2];        /* 00 */
        char    reserved1[28];         
} hdr2Label_t;

/* some definition for volume label and header labels */

#define TAPE_EXPIR_DATE		"900001"	/* year 2900 */
#define SRB_TAPE_OWNER		"SDSC_SRB"
#define DATA_BEGIN_OFFSET	(sizeof (hdr1Label_t) + sizeof (hdr2Label_t))

typedef struct tapeLibConfig {
    int inx;
    char host[HOSTNAME_LEN];
    char port[PORT_LEN];
    void *next;
} tapeLibConfig_t;

typedef struct tapeFileInfo {
    int intReplNum;
    int intSegNum;
    char tapeNum[TAPE_TYPE_LEN];
    tapePos_t tapePos;
    srb_long_t offset;
    srb_long_t size;
    tapeLibConfig_t *tapeLibConfig;
    void *next;
} tapeFileInfo_t;

/* prototype for srbTapeLib.c */

int setVol1Struct (vol1Label_t *myVol1Lab, char *tapeNum);
int setHdr1Struct (hdr1Label_t *myHdr1Label, char *tapeNum, char *fileID,
int fileSegmNo, int fileSeqNo);
int setHdr2Struct (hdr2Label_t *myHdr2Label, int blockSz);
int setTapeDate (char *myDate);
int srbThreadSleep (int sec, int microSec);
int readVerifyVol1 (tapeDev_t *tapeDev);
int readVerifyHdr (tapeDev_t *tapeDev);
int writeHeader (tapeDev_t *tapeDev, char *fileID);
int srbOpenTapeForRead (tapeDev_t *tapeDev, tapePos_t *destPos);
int srbOpenTapeForWrite (tapeDev_t *tapeDev, tapePos_t *destPos, char *fileID);
int srbReadTape (tapeDev_t *tapeDev, char *buf, int length);
int srbWriteTape (tapeDev_t *tapeDev, char *buf, int length);
int initDrvByDevPath (char *devPath, tapeDev_t *tapeDev);
int initTapeLibConfig ();
tapeLibConfig_t *matchTapeLibByInx (int inx);
int
initTapeBuf (int cartType, int curBufLen, char **buf);
int
initTapeDevConfig ();
int
getCartTypeInxByCartName (char *cartTypeName);
int
getDrvInxByCartName (char *cartTypeName);
char *
getCartNameByCartTypeInx (int cartTypeInx);
int
getBlkSizeByCartTypeInx (int cartType);

#endif	/* SRB_TAPE_MISC_EXTERN_H */
