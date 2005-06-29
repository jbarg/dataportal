/* adsFileDvr.h - Definitions and prototypes for adsFileDvr.c
 */
 
#ifndef ADS_FILE_DVR_H
#define ADS_FILE_DVR_H

#include <stdio.h>
#include <sys/file.h>
#include <sys/param.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#if defined(PORTNAME_osx)
#include <sys/types.h>
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <fcntl.h>  
#include <sys/file.h> 
#include <dirent.h> 
#include <time.h>

#include "srb.h"
#include "elogExtern.h"
#include "adsFileDvrExtern.h"
#include "tapes.h"
#include "tapesys.h"
#include "recsys.h"
#include "label.h"

#define ADS_SUCCESS         0
#define ADS_FAILURE         -1
#define TAPENAMELEN         6
#define TAPEOWNER_LEN       8
#define MAXCMD              1024
#define MAXCMDOUT           5000
#define BLOCKSIZE           32767           
#define FLF_LEN_POOLNAME    8
#define MAX_FILENAME        1024

#define TAPESIZE          "160GB"
#define TAPEOWNER_DEFAULT "srbtst"
#define VTPSERVER         "vtp.data.rl.ac.uk"
#define POOL_PARENT_DIR   "ADSPOOLS"

extern int DebugLvl;

/* The skeleton type tells WRITESKEL which type of label to write */
enum SKELTYPE  { skelhdr, skeleof, skeleov };

static int _adsOpen(char *tapename, int fileFlags, int fileMode);
static int adsPipeCmd(char *cmdstring, char *cmdout);
static int adsCreateTapename(char *pathname, char *tapename);
static int adsGetTapename(char *pathname, char *tapename);
static int adsWriteTapeLabels(int ts, char *tapename);
static int adsReadTapeLabels(int ts, char *tapename);
static int adsInitAdsStreams(int ts);          
static int adsSetupRecord(struct RECLIST *r);
static char *adsTapeOwner(void);

/* For writing tape labels */
static int  writevol(struct RECLIST *r);
static char get_rform(struct RECLIST *r);
static char get_battr(struct RECLIST *r);
static void upcase(char *s);
static int  put_label(struct RECLIST *r, char *label);
static int  writeskel(struct RECLIST *r, enum SKELTYPE stype);
static void mygetdate(char *dbuf, int *outyear, int *outday);
static int  do_wrend(struct RECLIST *r, enum SKELTYPE stype);
static int  chekvol(struct RECLIST *r);
static void despace(char *str);
static int  get_label(struct RECLIST *r, char *label);

struct AdsStream {
   struct RECLIST *rStream;
   unsigned long fileWriteSize;
};

struct AdsStream adsStreams[MAX_TSTREAM];

#endif	/* ADS_FILE_DVR_H */
