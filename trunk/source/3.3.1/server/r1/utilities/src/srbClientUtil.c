
/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/*
Version1.1
  Patch 1: 04/01/98 Raja -  new checkCollection  - simplified 
                       new routine called has_like_chars
		       used has_like_chars in several routines in this file
		              where ESCAPE is used. makes queries run faster
			      and uses ESCAPE only when necessary
*/ 
 
#include "scommands.h"
#include <sys/stat.h>
#include <dirent.h>
#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include "Unix2Nt.h"
#include "nt_extra.h"
#include "SrbNtUtil.h"
#include "misc_util.h"
#endif
#include <fcntl.h>

#ifndef _WIN32
#include <grp.h>
#endif

#ifdef _WIN32
#define SRB_NT_SESSION_EXT	"session"
#endif

/* Global user env variable */
char EnvFilePath[MAX_TOKEN] = {'\0', '\0'};
char AuthFilePath[MAX_TOKEN];
char srbAuth[MAX_TOKEN];
char srbHost[MAX_TOKEN];
char srbPort[MAX_TOKEN];
char mcatZone[MAX_TOKEN];
char srbUser[MAX_TOKEN];
char mdasCollectionName[MAX_TOKEN];
char mdasCollectionHome[MAX_TOKEN];
char defaultResource[MAX_TOKEN];
char mdasDomainName[MAX_TOKEN];
char defaultAuthScheme[MAX_TOKEN];
char AuthSchemeBuf[MAX_TOKEN];
int  hasPrinted = 0;
int linkFlag =  0;
int NumThreads =  0;	/* for Sget/Sput with -M and -t options */
int curatorFlag = 0;
int versionFlag = 0;
char versionString[MAX_TOKEN];
char defResrId[MAX_TOKEN][MAX_TOKEN];
char defResrName[MAX_TOKEN][MAX_TOKEN];
char defResrPath[MAX_TOKEN][MAX_TOKEN];
int defResrMax  = 0;
char domainenvbuf[MAX_TOKEN];
char userenvbuf[MAX_TOKEN];
char inCondition[MAX_OBJ_NAME_LENGTH];
char FileGroup[ENV_LEN];
#ifdef PARA_OPR
pthread_mutex_t Glock;
#endif

int RemovingCont = 0;   /* flag to indicate whether we are trying to
                         * remove a container */
int RStatus = 0;        /* status of recurive operation */

void filterDuplicate (mdasC_sql_result_struct *myresult);
void filterDuplicateShort (mdasC_sql_result_struct *myresult);

srb_long_t
srbToLocalFileCopyS (srbConn *conn, char *localPath, char *srbDataName,
char *srbCollection, srb_long_t size, int flagval);
srb_long_t
srbToLocalFileCopyS2 (srbConn *conn, char *localPath, char *srbDataName,
char *srbCollection, srb_long_t size, int flagval);

srb_long_t
localToSrbFileCopyS (srbConn *conn, int catType, char *localPath, 
char *srbDataName, char *srbCollection, srb_long_t datasize, int flagval, 
int destExist, char *datatype, char *newpathname, char *newResource,
int copyNum);
srb_long_t
localToSrbFileCopyS2 (srbConn *conn, int catType, char *localPath, 
char *srbDataName, char *srbCollection, srb_long_t datasize, int flagval, 
int destExist, char *datatype, char *newpathname, char *newResource,
int copyNum);

#ifdef PARA_OPR
srb_long_t
srbToLocalFileCopyP (srbConn *conn, char *localPath, char *srbDataName,
char *srbCollection, srb_long_t size, int flagval);
srb_long_t
srbToLocalFileCopyP2 (srbConn *conn, char *localPath, char *srbDataName,
char *srbCollection, srb_long_t size, int flagval);

srb_long_t
localToSrbFileCopyP (srbConn *conn, int catType, char *localPath, 
char *srbDataName, char *srbCollection, srb_long_t datasize, int flagval, 
int destExist, char *datatype, char *newpathname, char *newResource);
srb_long_t
localToSrbFileCopyP2 (srbConn *conn, int catType, char *localPath, 
char *srbDataName, char *srbCollection, srb_long_t datasize, int flagval, 
int destExist, char *datatype, char *newpathname, char *newResource);
#endif	/* PARA_OPR */

srb_long_t
srbTosrbCopyS (srbConn *conn, int catType, char *srcObjID, char *srcCollection,
char *destObjID, char *destCollection, char *destResource,
char *datatype, char *newPath, srb_long_t datasize, int existFlag, int flagval);

int
getSomeCopyNum(srbConn *conn, int catType, char *collName, char *dataName);

#ifdef PARA_OPR
void *doPartialOpr (void *inFileOpr);
#endif


#define FROM_SRB 0
#define INTO_SRB 1
/* Right-rotate 32-bit integer variable C. */
#define ROTATE_RIGHT(c) if ((c) & 01) (c) = ((c) >>1) + 0x8000; else (c) >>= 1;
#define SLEEP_MULTIPLIER 2
#define SLEEP_MAX 3600
#define SRB_RETRIES_MAX 32000
#define STREQ(a, b) (strcmp ((a), (b)) == 0)
#define RETRY_SLEEP_MAX 24*3600

int iGlbRetriesMax=0;
int prev_stats_len=0;

void vNiceVerboseProgressMessage (int flagval, int iIntoSrb, char *srbName,
			      char *UnixName, ulong ulBytes, ulong ulTotalSize, float fSec, float fBs);
void vNiceVerboseFinalMessage (int flagval, int iIntoSrb, char *srbName,
  char *UnixName, srb_long_t lBytes, float fSec, int iThreads, 
unsigned long ulChecksum);

void vNiceVerboseProgressMessage (int flagval, int iIntoSrb, char *srbName,
				   char *UnixName, ulong ulBytes, ulong ulTotalSize, 
				   float fSec, float fBs)
{
  time_t timval;
  struct tm *tmptr;
  int len;
  char etastr[10];
  unsigned long eta;

  time (&timval);
  tmptr = localtime (&timval);

  if (ulBytes < 0)
    ulBytes = 0;
  if (fSec <= 0.0)
    fSec = 0.00000000001;

  eta =  (unsigned long) ((ulTotalSize - ulBytes)/fBs);
  sprintf (etastr, "%02ld:%02ld:%02ld", eta / 3600,
	   (eta % 3600) / 60, eta % 60);
  
  len = fprintf (stdout,
		 "\r%.0f MB | %.2e B/s | %.0f s | ETA: %8s | %d%%",
		 (float) (ulBytes / (1024.0 * 1024.0)), fBs, fSec, etastr,
		 (int) (100 * (ulBytes * 1.0 / ulTotalSize)));
  if (len < prev_stats_len)
    printf ("%*s", prev_stats_len - len, "");
  
  prev_stats_len = len;

  fflush (stdout);
}

void
vNiceVerboseFinalMessage (int flagval, int iIntoSrb, char *srbName,
			  char *UnixName, srb_long_t lBytes, float fSec,
			  int iThreads, unsigned long ulChecksum)
{
  time_t timval;
  struct tm *tmptr;
  char sTmp1[512], sTmp2[512], sTmp3[512], sTmp4[512];
  char *myPtr;

  if (ulChecksum <= 0) {
    if ((myPtr = strstr (srbName, "_CHKSUM=")) != NULL) {
        ulChecksum = atoi (myPtr + 8);
    }
  }
  time (&timval); 
  tmptr = localtime (&timval);
  sTmp4[0] = '\0';
    
  if (iIntoSrb == INTO_SRB) {
    (void) sprintf (sTmp1, "LOCAL:%s->SRB:%s", UnixName, srbName);
    (void) sprintf (sTmp3, "->SRB:%s", UnixName);
  }
  else {
    (void) sprintf (sTmp1, "SRB:%s->LOCAL:%s", srbName, UnixName);
    (void) sprintf (sTmp3, "->LOCAL:%s", srbName);
  }

  if (lBytes < 0)
    lBytes = 0;
  if (fSec <= 0.0)
    fSec = 0.00000000001;

  (void) sprintf (sTmp2,
	 "\r%s | %5.3f MB | %5.3f MB/s | %.2f s | %4d.%02d.%02d %.02d:%.02d:%.02d",
		  sTmp1, (float) lBytes / 1000000., 
		  (float) lBytes / fSec /1000000.,
		  fSec, 1900 + tmptr->tm_year, tmptr->tm_mon + 1,
		  tmptr->tm_mday, tmptr->tm_hour, tmptr->tm_min,
		  tmptr->tm_sec);

  if (iThreads > 0)
    (void) sprintf (sTmp4, "| %d thr", iThreads);

  if ((flagval & k_FLAG) || (flagval & K_FLAG))
      (void) sprintf (sTmp4, "%s | chksum=%lu", sTmp4, ulChecksum);

  if ((strlen(sTmp1)+strlen(sTmp2)+strlen(sTmp4)) > 132) { /* do not print message longer than 80 chars */
    (void) fprintf (stdout, "\r%s | %s %s\n", sTmp3, sTmp2, sTmp4); /* shorter version */
  }    
  else {
    (void) fprintf (stdout, "\r%s | %s %s\n", sTmp1, sTmp2, sTmp4);  
  }
  fflush (stdout);
}

int iWriteCheckSumIntoSrb (srbConn *conn, unsigned long ulSum, char *srbDataName, char *srbCollection) {
  char collName[MAX_TOKEN];
  char dataName[MAX_TOKEN];
  char newValue[MAX_TOKEN];
  unsigned long ulSum2;

  (void) sprintf(newValue, "%ld", ulSum);
  strncpy(collName, srbCollection, MAX_TOKEN-1);
  strncpy(dataName, srbDataName, MAX_TOKEN-1);

  i = srbModifyDataset(conn, MDAS_CATALOG, 
					dataName, collName,
					"","",
					newValue,"", D_INSERT_DCHECKSUM);

  if (i < 0)
    {
      fprintf(stderr,"iWriteCheckSumIntoSrb: Error in modification:\"%s/%s:%i\"\n", 
	      collName,dataName, i);
      srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      return(-1);
    }
  
  return(0);
}

int iReadChecksumFromSrb (srbConn *conn, char *srbDataName, 
char *srbCollection, unsigned long *ulChecksumFromSrb) 
{
    mdasC_sql_result_struct myresult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM], i;
    int L_val = DEFAULT_ROW_COUNT;
    mdasC_sql_result_struct *dataResult;
    char *chksum, *isDirty;
    int dirtyFlag;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        selval[i] = 0;
        sprintf(qval[i],"");
    }

    genEqualSQL (qval[DATA_NAME], srbDataName);  
    genEqualSQL (qval[DATA_GRP_NAME], srbCollection);
    selval[DATA_CHECKSUM] = 1;
    selval[IS_DIRTY] = 1;

    i = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult, L_val);
    if (i < 0) {
        fprintf(stderr, 
         "can't srbGetDataDirInfo (DATA_CHECKSUM, SRB:%s/%s)\n", 
          srbCollection, srbDataName);
        *ulChecksumFromSrb = 0;
        return(i);
    }
    errno = 0;
    chksum = (char *) getFromResultStruct (
     (mdasC_sql_result_struct *) &myresult, 
     dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
  
    isDirty = (char *) getFromResultStruct (
     (mdasC_sql_result_struct *) &myresult,
     dcs_tname[IS_DIRTY], dcs_aname[IS_DIRTY]);

    *ulChecksumFromSrb = (unsigned long) strtol (chksum, (char **) NULL, 10);
    dirtyFlag = atoi (isDirty);
    for (i = 1; i < myresult.row_count; i++) {
	chksum += MAX_DATA_SIZE;
	isDirty += MAX_DATA_SIZE;

	if (atoi (isDirty) > 0) {
	    if (dirtyFlag == 0) {
		dirtyFlag = 1;
		*ulChecksumFromSrb = 
		  (unsigned long) strtol (chksum, (char **) NULL, 10);
	    } else if (*ulChecksumFromSrb == 0) {
		*ulChecksumFromSrb = 
                  (unsigned long) strtol (chksum, (char **) NULL, 10);
	    }
	}
    }
    return(0);
}

void splitbycharwesc_inclient(char *inname, char *collname, char *dataname,
			      char spchar, char escapechar)
{
  int cnt;
  char cval[HUGE_STRING], dname[HUGE_STRING];
  int i,j;
  cnt = strlen(inname);
  j = 0;
  dname[0] =  '\0';
  cval[0] =  '\0';
  for (i = cnt - 1; i >= 0 ; i--)
    {
      if (inname[i] == spchar && inname[i-1] != escapechar) break;
      if (!(inname[i] == escapechar && inname[i+1] == spchar))
        {
          cval[j] = inname[i];
          j++;
        }
    }
  cval[j] = '\0';
  if (i >= 0)
    {
      inname[i] = '\0';
      strcpy(collname,inname);
      inname[i] = spchar;
    }
  else
    {
      strcpy(collname,"");
    }

  for (i = 0;  j > 0 ; i++, j--)
    dname[i] = cval[j-1];
  dname[i]  = '\0';
  strcpy(dataname,dname);
}



int set_qval_data_cond_inclient(char *in_data_name, char qval[][MAX_TOKEN])
{
  char st1[MAX_OBJ_NAME_LENGTH],st2[MAX_OBJ_NAME_LENGTH],st[MAX_OBJ_NAME_LENGTH]
;
  char cval[MAX_OBJ_NAME_LENGTH];
  char *tt;
  int i, ii, j;
  int attrCntD = 0;
  int attrCntR = 0;
  int attrCntC = 0;
  int attrCntU = 0;
  char attrName[MAX_TOKEN],attrOp[MAX_TOKEN], attrCond[MAX_TOKEN];
  int andFlag = 0;
  char andStr[3];
  char andRep[] = "#$?@;+![](){}/ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  char *tp, *tp1;

  strcpy(st,in_data_name);
  i = 0;
  while (st[0] ==  ' ') i++;
  if (st[i] != '&')
      { /* dataname at beginning */
        strcpy(qval[DATA_NAME], " = '");
        j = 4;
        while (st[i] != '&' && i < strlen(st))
          {
            qval[DATA_NAME][j] = st[i];
            i++; j++;
          }
        qval[DATA_NAME][j] = '\'';
        qval[DATA_NAME][j+1] = '\0';
        strcpy(st, (char *) &st[i]);
      }
  if (strstr(st,"&&")) {
    for (i = 0 ; i < strlen(andRep); i++) {
      sprintf(andStr,"%c%c",andRep[i],andRep[i]);
      if (strstr(st,andStr) == NULL) 
	break;
    }
    if (i != strlen(andRep)) {
      andFlag = 1; 
      tp = st;
      while ((tp1 = strstr(tp,"&&")) != NULL) {
	*tp1 = andStr[0];
	tp1++;
	*tp1 = andStr[0];
	tp = tp1 + 1;
      }
    }
  }
    while (strlen(st) > 0)
        {
          splitbycharwesc_inclient(st, st1, st2, '&' , '\\');
	  
          tt = (char *) st2;
	  make_like_for_mdas(tt);
	  if (andFlag == 1) {
	    tp = tt;
	    while ((tp1 = strstr(tp,andStr)) != NULL) {
	      *tp1 = '&';
	      tp1++;
	      *tp1 = '&';
	      tp = tp1 + 1;
	    }
	  }
          while (  tt[0] == ' ') tt++;
          if( tt[0] == '$')
            {
              if (isdigit(tt[1]) && isdigit(tt[2]))
                {
                  sprintf(cval, "%c%c",tt[1],tt[2]);
                  sprintf(qval[atoi(cval)], (char *) &tt[3]);
                }
            }
          else if(strstr((char *) &tt[0],"PATH") == (char *) &tt[0])
            sprintf(qval[PATH_NAME], "%s", (char *) &tt[4]);
          else if(strstr((char *) &tt[0],"COPY") == (char *) &tt[0])
            sprintf(qval[DATA_REPL_ENUM],  "%s",(char *) &tt[4]);
          else if(strstr((char *) &tt[0],"GUID") == (char *) &tt[0])
            sprintf(qval[GUID],  "%s",(char *) &tt[4]);
          else if(strstr((char *) &tt[0],"DTYPE") == (char *) &tt[0])
            sprintf(qval[DATA_TYP_NAME], "%s", (char *) &tt[5]);
          else if(strstr((char *) &tt[0],"DATA") == (char *) &tt[0])
            sprintf(qval[DATA_NAME], "%s", (char *) &tt[4]);
          else if(strstr((char *) &tt[0],"RESOURCE") == (char *) &tt[0])
            sprintf(qval[RSRC_NAME], "%s", (char *) &tt[8]);
          else if(strstr((char *) &tt[0],"RTYPE") == (char *) &tt[0])
            sprintf(qval[RSRC_TYP_NAME], "%s", (char *) &tt[5]);
          else if(strstr((char *) &tt[0],"COLLECTION") == (char *) &tt[0])
            sprintf(qval[DATA_GRP_NAME], "%s", (char *) &tt[10]);
          else if(strstr((char *) &tt[0],"NETADDR") == (char *) &tt[0])
            sprintf(qval[RSRC_ADDR_NETPREFIX], "%s", (char *) &tt[7]);
          else if(strstr((char *) &tt[0],"DCOMMENTS") == (char *) &tt[0])
            sprintf(qval[DATA_COMMENTS],  "%s",(char *) &tt[9]);
          else if(strstr((char *) &tt[0],"DTIME") == (char *) &tt[0])
            sprintf(qval[REPL_TIMESTAMP],  "%s",(char *) &tt[5]);
          else if(strstr((char *) &tt[0],"USER") == (char *) &tt[0])
            sprintf(qval[DATA_OWNER], "%s", (char *) &tt[4]);
          else if(strstr((char *) &tt[0],"OWNER") == (char *) &tt[0])
            sprintf(qval[DATA_OWNER],  "%s",(char *) &tt[5]);
          else if(strstr((char *) &tt[0],"SIZE") == (char *) &tt[0])
            sprintf(qval[SIZE], "%s", (char *) &tt[4]);
          else if(strstr((char *) &tt[0],"CTIME") == (char *) &tt[0])
            sprintf(qval[DATA_CREATE_TIMESTAMP],  "%s",(char *) &tt[5]);
          else if(strstr((char *) &tt[0],"DIRTY") == (char *) &tt[0])
            sprintf(qval[IS_DIRTY],  "%s",(char *) &tt[5]);
          else if(strstr((char *) &tt[0],"VNUM") == (char *) &tt[0])
            sprintf(qval[DATA_VER_NUM],  "%s",(char *) &tt[4]);
	  else if(strstr((char *) &tt[0],"ATTRCONDD") == (char *) &tt[0]) {
	    splitAttrOpCond((char *) &tt[9],attrName,attrOp, attrCond);
	    if (strstr(attrOp,"like") || strstr(attrOp,"LIKE") ) {
	      make_like_for_mdas(attrCond);
	      strcat (attrCond, "' ESCAPE '\\");
	    }
	    if (attrCntD == 0) {
	      if (make_like_for_mdas(attrName) == 1)
		sprintf(qval[UDSMD0_0]," like  '%s' ESCAPE '\\'",attrName);
	      else
		sprintf(qval[UDSMD0_0]," = '%s'",attrName);
	      sprintf(qval[UDSMD1_0]," %s '%s'", attrOp,attrCond); 
	      attrCntD++;
	    }
	    else if (attrCntD > 0 && attrCntD < 5) {
	      if (make_like_for_mdas(attrName) == 1)
		sprintf(qval[UDSMD0_1 + attrCntD - 1]," like  '%s' ESCAPE '\\'",attrName);
	      else
		sprintf(qval[UDSMD0_1 + attrCntD - 1]," = '%s'",attrName);
	      sprintf(qval[UDSMD1_1 + attrCntD - 1]," %s '%s'", attrOp,attrCond); 
	      attrCntD++;
	    }
	  }
	  else if(strstr((char *) &tt[0],"ATTRCONDC") == (char *) &tt[0]) {
	    splitAttrOpCond((char *) &tt[9],attrName,attrOp, attrCond);
	    if (strstr(attrOp,"like") || strstr(attrOp,"LIKE") ) {
	      make_like_for_mdas(attrCond);
	      strcat (attrCond, "' ESCAPE '\\");
	    }
	    if (attrCntC == 0) {
	      if (make_like_for_mdas(attrName) == 1)
		sprintf(qval[UDSMD_COLL0_0]," like  '%s' ESCAPE '\\'",attrName);
	      else
		sprintf(qval[UDSMD_COLL0_0]," = '%s'",attrName);
	      sprintf(qval[UDSMD_COLL1_0]," %s '%s'", attrOp,attrCond); 
	      attrCntD++;
	    }
	    else if (attrCntC > 0  &&  attrCntC < 5) {
	      if (make_like_for_mdas(attrName) == 1)
		sprintf(qval[UDSMD_COLL0_1 + attrCntD - 1]," like  '%s' ESCAPE '\\'",attrName);
	      else
		sprintf(qval[UDSMD_COLL0_1 + attrCntD - 1]," = '%s'",attrName);
	      sprintf(qval[UDSMD_COLL1_1 + attrCntD - 1]," %s '%s'", attrOp,attrCond); 
	      attrCntD++;
	    }
	  }
	  else if(strstr((char *) &tt[0],"ATTRCONDU") == (char *) &tt[0]) {
	    splitAttrOpCond((char *) &tt[9],attrName,attrOp, attrCond);
	    if (strstr(attrOp,"like") || strstr(attrOp,"LIKE") ) {
	      make_like_for_mdas(attrCond);
	      strcat (attrCond, "' ESCAPE '\\");
	    }
	    if (attrCntU == 0) {
	      if (make_like_for_mdas(attrName) == 1)
		sprintf(qval[UDSMD_USER0_0]," like  '%s' ESCAPE '\\'",attrName);
	      else
		sprintf(qval[UDSMD_USER0_0]," = '%s'",attrName);
	      sprintf(qval[UDSMD_USER1_0]," %s '%s'", attrOp,attrCond); 
	      attrCntD++;
	    }
	    else if (attrCntU > 0  &&  attrCntU < 4) {
	      if (make_like_for_mdas(attrName) == 1)
		sprintf(qval[UDSMD_USER0_1 + attrCntD - 1]," like  '%s' ESCAPE '\\'",attrName);
	      else
		sprintf(qval[UDSMD_USER0_1 + attrCntD - 1]," = '%s'",attrName);
	      sprintf(qval[UDSMD_USER1_1 + attrCntD - 1]," %s '%s'", attrOp,attrCond); 
	      attrCntD++;
	    }
	  }
	  else if(strstr((char *) &tt[0],"ATTRCONDR") == (char *) &tt[0]) {
	    splitAttrOpCond((char *) &tt[9],attrName,attrOp, attrCond);
	    if (strstr(attrOp,"like") || strstr(attrOp,"LIKE") ) {
	      make_like_for_mdas(attrCond);
	      strcat (attrCond, "' ESCAPE '\\");
	    }
	    if (attrCntR == 0) {
	      if (make_like_for_mdas(attrName) == 1)
		sprintf(qval[UDSMD_RSRC0_0]," like  '%s' ESCAPE '\\'",attrName);
	      else
		sprintf(qval[UDSMD_RSRC0_0]," = '%s'",attrName);
	      sprintf(qval[UDSMD_RSRC1_0]," %s '%s'", attrOp,attrCond); 
	      attrCntD++;
	    }
	    else if (attrCntR > 0  &&  attrCntR < 4) {
	      if (make_like_for_mdas(attrName) == 1)
		sprintf(qval[UDSMD_RSRC0_1 + attrCntD - 1]," like  '%s' ESCAPE '\\'",attrName);
	      else
		sprintf(qval[UDSMD_RSRC0_1 + attrCntD - 1]," = '%s'",attrName);
	      sprintf(qval[UDSMD_RSRC1_1 + attrCntD - 1]," %s '%s'", attrOp,attrCond); 
	      attrCntD++;
	    }
	  }
          else if(strstr((char *) &tt[0],"TICKET") == (char *) &tt[0])
            {
              ii = 6;
              while (tt[ii] == ' ' || tt[ii] == '=') ii++;
              if (tt[ii+1] == 'C' || tt[ii+1] == 'c')
                sprintf(qval[TICKET_C],  "%s",(char *) &tt[6]);
              else 
                sprintf(qval[TICKET_D],  "%s",(char *) &tt[6]);
            }
          /* else if(strstr((char *) &tt[0],"") == (char *) &tt[0])
            sprintf(qval[],  "%s",(char *) &tt[]); */
          /* else if (strlen(st1) == 0)
             sprintf(qval[DATA_NAME], " = '%s'", tt); */
/*        else return(ERROR_IN_QUERY_CONDITION); */
          strcpy(st,st1);
        }
 return(0);
}


int
fillQvalWithCondition(char *condition, char qval[][MAX_TOKEN])
{
  char tmpCondition[MAX_OBJ_NAME_LENGTH];

  sprintf(tmpCondition,"&%s",condition);
  return(set_qval_data_cond_inclient(tmpCondition,qval));
}

#ifdef _WIN32
void GetWindowsSessionEnvFile(char *buf)
{
    char tmpstr[MAX_TOKEN];
    GetWindowsClientEnvFile(tmpstr);
    if(strlen(tmpstr) == 0)
      return;

    strcat(tmpstr,".");
    strcat(tmpstr,SRB_NT_SESSION_EXT);

    strcpy(buf,tmpstr);
}
#endif

int firstInitSrbClientEnv()
{
  FILE *fd;
  
  char buf[MAX_TOKEN], vbuf[MAX_TOKEN], gbuf[MAX_TOKEN];
  char inBuf[MAX_TOKEN];
  char *authPw = (char *) NULL;
  int i;


  defResrMax = 0;
  if (getenv("mdasEnvFile")==NULL)
  {
#ifdef _WIN32
      EnvFilePath[0] = '\0';
      GetWindowsClientEnvFile(EnvFilePath);
#else
	  strcpy(buf, getenv("HOME"));
          sprintf(EnvFilePath, "%s/%s",buf,".srb/.MdasEnv");
#endif
  }
  else 
    {
      strcpy(EnvFilePath,getenv("mdasEnvFile"));
    }

#ifdef _WIN32
  fd = SrbNt_fopen(EnvFilePath,"r");
#else
  fd = fopen(EnvFilePath,"r");
#endif

  if (fd == NULL) {
    printf("firstInitSrbClientEnv: Unable to open mdasEnvFile:%s\n",
	   EnvFilePath);
    return (UTIL_ERROR_ENV_FILE_OPEN);
  }
  memset (defaultAuthScheme, 0, sizeof (defaultAuthScheme));  
  strcpy( buf,"");strcpy( vbuf,"");
  while (fgets(inBuf, MAX_TOKEN - 1, fd) != NULL) {
            if (inBuf[0] == '#')        /* comment */
                continue;

	    if (sscanf(inBuf, "%s '%[^']'", buf, vbuf) <= 0) 
                continue;

	    if (!strcmp(buf, "mdasCollectionHome"))  { 
	        strcpy(mdasCollectionName,vbuf);
	        strcpy(mdasCollectionHome,vbuf);
	    } else if (!strcmp(buf, "mdasDomainHome") ||
	     !strcmp(buf, "mdasDomainName")) { 
	      if (getenv("mdasDomainName")) {
	        strcpy(mdasDomainName,getenv("mdasDomainName"));
	      } else {
	        strcpy(mdasDomainName,vbuf);
	      }
	    } else if (!strcmp(buf, "srbUser")) {  
	      if (getenv("srbUser")) 
		  strcpy(srbUser, getenv("srbUser"));
	      else
	          strcpy(srbUser,vbuf);
	    } else if (!strcmp(buf, "srbHost")) {
	      if (getenv("srbHost"))
	          strcpy(srbHost,getenv("srbHost"));
	      else
	          strcpy(srbHost,vbuf);
	    } else if (!strcmp(buf, "srbPort")) { 
	      if (getenv("srbPort"))
	          strcpy(srbPort,getenv("srbPort"));
	      else
	          strcpy(srbPort,vbuf);
            } else if (!strcmp(buf, AUTH_KEYWD)) {
              if (getenv(AUTH_KEYWD))
                  strcpy(defaultAuthScheme, getenv(AUTH_KEYWD));
              else
                  strcpy(defaultAuthScheme, vbuf);
            } else if (!strcmp(buf, "mcatZone")) {
              if (getenv("mcatZone"))
                  strcpy(mcatZone,getenv("mcatZone"));
              else
                  strcpy(mcatZone,vbuf);
	    } else if (!strcmp(buf, "defaultResource")) { 
	      if (getenv("mdasResourceName"))
	          strcpy(defaultResource,getenv("mdasResourceName"));
	      else
	          strcpy(defaultResource,vbuf);
	    }
	    strcpy( buf,"");strcpy( vbuf,"");
    }
  fclose(fd);
  sprintf(domainenvbuf,"mdasDomainName=%s",mdasDomainName);
  putenv(domainenvbuf);
  sprintf(userenvbuf,"srbUser=%s",srbUser);
  putenv(userenvbuf);
  if (strlen (defaultAuthScheme) > 0) {
      sprintf(AuthSchemeBuf,"AUTH_SCHEME=%s", defaultAuthScheme);
      putenv(AuthSchemeBuf);
  }

  if (strcmp (defaultAuthScheme, AUTH_ENCRYPT1_KEYWD) == 0) {
    authPw = readMdasAuth();
    if (authPw != NULL) {
      strncpy(srbAuth, authPw, MAX_TOKEN-1);
    }
  }
  else {
    strcpy (srbAuth, "UNKNOWN");
  }

  /* We currently only allow one s-command sesion in one box. */
#ifdef _WIN32
   EnvFilePath[0] = '\0';
   GetWindowsSessionEnvFile(EnvFilePath);
   fd = SrbNt_fopen(EnvFilePath,"r");
   if(fd > 0) {   /* file exist */
      fclose(fd);
	fprintf(stderr,
	 "NT allows only one session of s-command. Please delete '%s' and try again.\n",EnvFilePath);
	exit(0);
   }
#endif
   return(0);
}

int 
getSessionEnvFile () {

    char *myHome;
    struct stat statbuf;
#ifndef _WIN32
    int parpid;
#endif
#if defined(PORTNAME_c90)
    struct jtab jtabBuf;
#endif


    if (getenv("mdasEnvFile")==NULL) {

#ifdef _WIN32
       EnvFilePath[0] = '\0';
       GetWindowsSessionEnvFile(EnvFilePath);
#else
       parpid = getppid();
       myHome = getenv("HOME");
       if(myHome == NULL )
          return (UTIL_ERROR_ENV_FILE_OPEN);
       sprintf(EnvFilePath, "%s/%s.%i", myHome, ".srb/.MdasEnv",parpid);
#endif /* WIN32 */


#ifdef _WIN32
    if(stat_file(EnvFilePath, &statbuf) == 0)
#else
	if (stat (EnvFilePath, &statbuf) == 0)
#endif
	    return (0);

        /* try the sesssion or job id */
#if defined(PORTNAME_c90)
        if (getjtab (&jtabBuf) > 0) {
            parpid = jtabBuf.j_ppid;
            sprintf(EnvFilePath, "%s/%s.%i", myHome,".srb/.MdasEnv",parpid);
        }
#else   /* PORTNAME_c90 */

#if defined(PORTNAME_osx)
        parpid = getppid ();
#else   /* PORTNAME_osx */

#ifndef _WIN32
        parpid = getsid (parpid);
#endif /* WIN32 */

#endif  /* PORTNAME_osx */

#ifndef _WIN32
        sprintf(EnvFilePath, "%s/%s.%i", myHome, ".srb/.MdasEnv", parpid);
#endif /* WIN32 */

#endif /* PORTNAME_c90 */


#ifdef _WIN32
	if (stat_file (EnvFilePath, &statbuf) == 0)
#else
	if (stat (EnvFilePath, &statbuf) == 0)
#endif
	{
            return (0);
	} else {
	    EnvFilePath[0] = '\0';
            return (UTIL_ERROR_ENV_FILE_OPEN);
        }
    } else {
	sprintf(EnvFilePath, "%s", getenv("mdasEnvFile"));
#ifdef _WIN32
	if (stat_file (EnvFilePath, &statbuf) == 0)
#else
	if (stat (EnvFilePath, &statbuf) != 0) 
#endif
	{
            EnvFilePath[0] = '\0';
            return (UTIL_ERROR_ENV_FILE_OPEN);
	}
    }
    return (0);
}

int initSrbClientEnv()
{
  FILE *fd;
  
  char *myHome;
  char buf[MAX_TOKEN], vbuf[MAX_TOKEN], gbuf[MAX_TOKEN];
  char inBuf[MAX_TOKEN];
  
  int i, envStatus, authStatus;
  char *authPw = (char *) NULL;

  EnvFilePath[0] = '\0';
  AuthFilePath[0] = '\0';
  srbAuth[0] = '\0';
  srbHost[0] = '\0';
  srbPort[0] = '\0';
  mcatZone[0] = '\0';
  srbUser[0] = '\0';
  mdasCollectionName[0] = '\0';
  mdasCollectionHome[0] = '\0';
  defaultResource[0] = '\0';
  mdasDomainName[0] = '\0';

  /* Does the session env file exist ? */

  envStatus = authStatus = 0;
  if (getSessionEnvFile () < 0) {
    initEnvWithEnvVar ();
    envStatus = (-1) * UTIL_ERROR_ENV_FILE_OPEN;
  } else {

#ifdef _WIN32
    fd = fp_fileopen(EnvFilePath,"r");
#else
    fd = fopen (EnvFilePath, "r");
#endif

    if (fd == NULL) {
      initEnvWithEnvVar ();
      envStatus = (-1) * UTIL_ERROR_ENV_FILE_OPEN;
    } else {

      defResrMax = 0;
      strcpy( buf,"");strcpy( vbuf,"");

      while (fgets(inBuf, MAX_TOKEN - 1, fd) != NULL) {
            if (inBuf[0] == '#')        /* comment */
                continue;

            if (sscanf(inBuf, "%s '%[^']'", buf, vbuf) <= 0) 
                continue;

	    if (!strcmp(buf, "mdasCollectionName"))  
	      strcpy(mdasCollectionName,vbuf);
	    else if (!strcmp(buf, "mdasCollectionHome"))  
	      strcpy(mdasCollectionHome,vbuf);
	    else if (!strcmp(buf, "mdasDomainName"))  
	      strcpy(mdasDomainName,vbuf);
	    else if (!strcmp(buf, "srbUser"))  
	      strcpy(srbUser,vbuf);
	    else if (!strcmp(buf, "srbHost"))  
	      strcpy(srbHost,vbuf);
	    else if (!strcmp(buf, "srbPort"))   
	    {
		sprintf (srbPort, vbuf);
              	if (getenv("srbPort") == NULL) {
		    char *myBuf = malloc (MAX_TOKEN);
		    sprintf (myBuf, "srbPort=%s", vbuf);
		    putenv (myBuf);
		}
	    }
            else if (!strcmp(buf, "mcatZone"))
            {
                sprintf (mcatZone, vbuf);
                if (getenv("mcatZone") == NULL) {
		    char *myBuf = malloc (MAX_TOKEN);
                    sprintf (myBuf, "mcatZone=%s", vbuf);
                    putenv (myBuf);
                }
            }
	    else if (!strcmp(buf, "defaultResource"))  
	      strcpy(defaultResource,vbuf);
	      strcpy( buf,"");strcpy( vbuf,"");
      }
      fclose(fd);
      sprintf(domainenvbuf,"mdasDomainName=%s",mdasDomainName);
      putenv(domainenvbuf);
      sprintf(userenvbuf,"srbUser=%s",srbUser);
      putenv(userenvbuf);
    }
  }
   
  authPw = readMdasAuth();
  if (authPw != NULL) {
    strncpy(srbAuth, authPw, MAX_TOKEN-1);
  }
  else {
    authStatus = (-1) * UTIL_ERROR_AUTH_FILE_OPEN;
  }

  if (envStatus != 0) {
    return (envStatus);
  } else if (authStatus != 0) {
    return (authStatus);
  } else {
    return(0);
  }
}

int initEnvWithEnvVar ()
{
    if (getenv("mdasDomainName")) 
      strcpy(mdasDomainName,getenv("mdasDomainName"));

    if (getenv("mdasCollectionName")) 
      strcpy(mdasCollectionName, getenv("mdasCollectionName"));

    if (getenv("srbUser"))
      strcpy(srbUser, getenv("srbUser"));

    if (getenv("srbHost"))
      strcpy(srbHost,getenv("srbHost"));

    if (getenv("srbPort"))
      strcpy(srbPort,getenv("srbPort"));

    if (getenv("mcatZone"))
      strcpy(mcatZone,getenv("mcatZone"));

    if (getenv("mdasResourceName"))
      strcpy(defaultResource,getenv("mdasResourceName"));

    if (getenv("defaultResource"))
      strcpy(defaultResource,getenv("defaultResource"));
}


int rewriteSrbClientEnvFile()
{
  FILE *fd;
#ifndef _WIN32
  int parpid;
#endif
  char buf[MAX_TOKEN];

#ifndef _WIN32
  parpid = getppid();
#endif

  if (getenv("mdasEnvFile")==NULL)
    {
#ifdef _WIN32
      EnvFilePath[0]='\0';
      GetWindowsSessionEnvFile(EnvFilePath);
#else
      strcpy(buf, getenv("HOME"));
      sprintf(EnvFilePath, "%s/%s.%i",buf,".srb/.MdasEnv",parpid);
#endif
  }
  else 
    {
      sprintf(EnvFilePath,"%s",getenv("mdasEnvFile"));
    }

#ifdef _WIN32
  fd = SrbNt_fopen(EnvFilePath,"w");
#else
  fd = fopen(EnvFilePath,"w");
#endif
  if (fd == NULL) {     
    printf("RewriteSrbClientEnv: Unable to open mdasEnvFile for write:%s\n",EnvFilePath);
    return (UTIL_ERROR_ENV_FILE_OPEN);
  }
 
  fprintf(fd, "mdasCollectionName '%s'\n",mdasCollectionName);
  fprintf(fd, "mdasCollectionHome '%s'\n",mdasCollectionHome);
  fprintf(fd, "mdasDomainName '%s'\n",mdasDomainName);
  fprintf(fd, "srbUser '%s'\n",srbUser);
  fprintf(fd, "srbHost '%s'\n",srbHost);
  if (strlen(srbPort) > 0)
    fprintf(fd, "srbPort '%s'\n",srbPort);
  if (strlen(mcatZone) > 0)
    fprintf(fd, "mcatZone '%s'\n",mcatZone);
  if (strlen(defaultResource) > 0)
    fprintf(fd, "defaultResource '%s'\n", defaultResource);

  fclose(fd);
  return(0);
}   

int RemoveSrbClientEnvFile()
{
#ifndef _WIN32
  int parpid;
#endif
  char buf[MAX_TOKEN];

#ifndef _WIN32
  parpid = getppid();
#endif

  if (getenv("mdasEnvFile")==NULL)
    {
#ifdef _WIN32
      EnvFilePath[0] = '\0';
      GetWindowsSessionEnvFile(EnvFilePath);
#else
      strcpy(buf, getenv("HOME"));
      sprintf(EnvFilePath, "%s/%s.%i",buf,".srb/.MdasEnv",parpid);
#endif
    }
  else 
    {
#ifdef _WIN32
      sprintf(EnvFilePath,"%s.%s",getenv("mdasEnvFile"),SRB_NT_SESSION_EXT);
#else
      sprintf(EnvFilePath,"%s.%i",getenv("mdasEnvFile"),parpid);
#endif
    }
  i = unlink(EnvFilePath);
  if (i < 0) fprintf(stderr,"Unlink error for env file:%s:%i\n",EnvFilePath,i);
  return(i);
}



int rewriteSrbClientAuthFile(int printOpt)
{
  FILE *fd;
  char buf[MAX_TOKEN];

  if (getenv("mdasAuthFile")==NULL)
    {
#ifdef _WIN32
      AuthFilePath[0] = '\0';
      GetWindowsClientAuthFile(AuthFilePath);
#else
      strcpy(buf, getenv("HOME"));
	  sprintf(AuthFilePath, "%s/%s",buf,".srb/.MdasAuth");
#endif
    }
  else 
    {
      strcpy(AuthFilePath,getenv("mdasAuthFile"));
    }

#ifdef _WIN32
  fd = SrbNt_fopen(AuthFilePath,"w");
#else
  fd = fopen(AuthFilePath,"w");
#endif

  if (fd == NULL) {
        printf("RewriteSrbClientAuthFile: Unable to open mdasAuthFile for write:%s\n",AuthFilePath);
	return (UTIL_ERROR_AUTH_FILE_OPEN);
  }

  fprintf(fd, "#Mdas-SRB Password\n");
  fprintf(fd, "%s\n",srbAuth);

  fclose(fd);
  if (printOpt) printf("Successfully updated %s\n",AuthFilePath);
  return(0);
}   




void clSplitbychar(char *myinname, char *collname, char *dataname, char spchar)
{
  /* Changed Aug 20,98 for handling SHADOW - condition split before splitby char
     and then appended to dataname */
  int cnt; 
  char cval[MAX_TOKEN], dname[MAX_TOKEN];
  char inname[MAX_TOKEN], condition[MAX_OBJ_NAME_LENGTH];
  int i,j;

  strcpy(condition,"");
  splitString(myinname,inname, condition, '&');

  cnt = strlen(inname);  
  j = 0;
  dname[0] =  '\0';
  cval[0] =  '\0';
  for (i = cnt - 1; i >= 0 ; i--)
    {
      if (inname[i] == spchar && (i == 0 || inname[i-1] != '<' )) break;
      cval[j] = inname[i];
      j++;
    }
  cval[j] = '\0';
  if (i >= 0)
    {
      inname[i] = '\0';
      strcpy(collname,inname);
      inname[i] = spchar;
    }
  else
    {
      strcpy(collname,"");
    }

  for (i = 0;  j > 0 ; i++, j--)
    dname[i] = cval[j-1];
  dname[i]  = '\0';
  if (strlen(condition) > 0)
    sprintf(dataname,"%s&%s", dname, condition);
  else
    sprintf(dataname,"%s", dname);
}

void splitpath(char *myinname, char *collname, char *dataname)
{
  int cnt;
  char cval[MAX_TOKEN], dname[MAX_TOKEN];
  int i,j;
  char temp[MAX_TOKEN], temp1[MAX_TOKEN], temp2[MAX_TOKEN], tempmCN[MAX_TOKEN];
  char inname[MAX_TOKEN], condition[MAX_OBJ_NAME_LENGTH];

  strcpy(condition,"");
  splitString(myinname,inname, condition, '&');


      cnt = strlen(inname);
      j = 0;
      for (i = cnt - 1; i >= 0 ; i--)
        {
          if (inname[i] == '/') break;
          cval[j] = inname[i];
          j++;
        }
      cval[j] = '\0';
      if (i > 0)
        {
          inname[i] = '\0';
          strcpy(collname,inname);
          inname[i] = '/';
        }
      else if (i == 0)
        {
              strcpy(collname,inname);
              strcpy(dataname, "");
        }
      else
        {
          strcpy(collname,"");
        }
      if (strcmp(collname,inname))
        {
          for (i = 0;  j > 0 ; i++, j--)
            dname[i] = cval[j-1];
          dname[i]  = '\0';
          strcpy(dataname,dname);
        }
      if (!strcmp(dataname, ".") && strlen(collname) == 0)
        { strcpy(collname,dataname); strcpy(dataname,"");}
      else if (!strcmp(dataname, "."))
        { strcpy(dataname,"");}

      if(strlen(collname) == 0) strcpy(collname, mdasCollectionName);
      else if (collname[0] == '^' || collname[0] == '~')
        {
          if (strlen(collname) == 1)
            strcpy(temp,mdasCollectionHome);
          else if (collname[1] == '/')
            sprintf(temp,"%s%s",mdasCollectionHome,(char *)(collname + 1));
          else
            {
              clSplitbychar(mdasCollectionHome,temp1, temp,'/');
              sprintf(temp,"%s/%s",temp1,(char *)(collname + 1));
            }
          strcpy(collname,temp);
        }
      else if (collname[0] == '.')
        {
          if (collname[1] == '/')
            {
              sprintf(temp, "%s%s",
                      mdasCollectionName, (char *)(collname + 1));
              strcpy(collname, temp);
            }
          else if (collname[1] == '.' && collname[2] == '/')
            {
              clSplitbychar(mdasCollectionName,temp1, temp,'/');
              strcpy(tempmCN, temp1);
              strcpy(temp2, (char *)(collname + 3));
              while (temp2[0] == '.' && temp2[1] == '.' && temp2[2] == '/')
              {
                clSplitbychar(tempmCN,temp1, temp,'/');
                strcpy(tempmCN, temp1);
                strcpy(temp2, (char *)(temp2 + 3));
              }
              sprintf(temp, "%s/%s",tempmCN, temp2);
              strcpy(collname, temp);
            }
          else if (collname[1] == '.' && strlen(collname) == 2)
            {
              clSplitbychar(mdasCollectionName,temp1, temp,'/');
              strcpy(collname, temp1);
            }
          else if (strlen(collname) == 1)
            {
               strcpy(collname, mdasCollectionName);
            }
          else
            {
              fprintf(stderr, "inconsistent collection: %s\n", collname);
              exit(1);
            }
        }
      else if (collname[0] == '/')
        {
          strcpy(temp,(char *)( collname ));
          strcpy(collname, temp);
        }
      else
        {
          sprintf(temp, "%s/%s", mdasCollectionName,collname);
          strcpy(collname, temp);
        }
      if (strlen(condition) > 0) {
	strcat(dataname,"&");
	strcat(dataname,condition);
      }
}


void mySplitpath (char *inname, char *collname, char *dataname)
{
  int cnt; 
  char cval[MAX_TOKEN], dname[MAX_TOKEN];
  int i,j;



      cnt = strlen(inname);  
      j = 0;
      for (i = cnt - 1; i >= 0 ; i--)
	{
	  if (inname[i] == '/') break;
	  cval[j] = inname[i];
	  j++;
	}
      cval[j] = '\0';
      if (i > 0)
	{
	  inname[i] = '\0';
	  strcpy(collname,inname);
	  inname[i] = '/';
	}
      else if (i == 0)
	{
	      strcpy(collname,inname);
	      strcpy(dataname, "");
	}
      else
	{
	  strcpy(collname,"");
	}
      if (strcmp(collname,inname))
	{
	  for (i = 0;  j > 0 ; i++, j--)
	    dname[i] = cval[j-1];
	  dname[i]  = '\0';
	  strcpy(dataname,dname);
	}
      if (!strcmp(dataname, ".") && strlen(collname) == 0)
	{ strcpy(collname,dataname); strcpy(dataname,"");}
      else if (!strcmp(dataname, "."))
	{ strcpy(dataname,"");}
}

int parseName (srbPathName *mysPathName, char *defCollection)
{
    char temp[MAX_OBJ_NAME_LENGTH], temp1[MAX_OBJ_NAME_LENGTH], 
     temp2[MAX_OBJ_NAME_LENGTH]; 
    char tempmCN[MAX_OBJ_NAME_LENGTH];
    int len = 0, dot2cnt;
    char *cptr1, *cptr2;

    if (mysPathName == NULL || defCollection == NULL) {
	fprintf (stderr, "parseName: input error\n");
	return -1;
    }

    mysPathName->outArgv = malloc (MAX_OBJ_NAME_LENGTH);
    /*
      mysPathName->outArgv = malloc (MAX_TOKEN);
     */

    if (mysPathName->outArgv == NULL) {
	errno = CLI_ERR_MALLOC;
	fprintf(stderr, "Client malloc error.\n");
	return -1;
    }

    len = strlen(mysPathName->inpArgv);

    /* Parse the first 3 char to take care of ".", "..", etc */

    if(len == 0) {
	strcpy(mysPathName->outArgv, mdasCollectionName);
	mysPathName->type = COLLECTION_T;
	return (COLLECTION_T);
    } else if (mysPathName->inpArgv[0] == '^' || 
      mysPathName->inpArgv[0] == '~') {
	if (len == 1) {
	    strcpy(mysPathName->outArgv, mdasCollectionHome);
            mysPathName->type = COLLECTION_T;
            return (COLLECTION_T);
	} else if (mysPathName->inpArgv[1] == '/') {
	    sprintf(mysPathName->outArgv, "%s%s",
	      mdasCollectionHome, (char *)(mysPathName->inpArgv + 1));
	} else {
	    clSplitbychar(mdasCollectionHome,temp1, temp,'/');
	    sprintf(mysPathName->outArgv, "%s/%s",
	     temp1,(char *)(mysPathName->inpArgv + 1));
	}
    } else if (mysPathName->inpArgv[0] == '.' && len == 1) {
        strcpy(mysPathName->outArgv, mdasCollectionName);
        mysPathName->type = SPEC_COLL_T;
        return (SPEC_COLL_T);
    } else if (mysPathName->inpArgv[0] == '.' &&
     mysPathName->inpArgv[1] == '/') { 	/* case for "./" */
	if (len == 2) {
	    strcpy(mysPathName->outArgv, mdasCollectionName);
	    mysPathName->type = SPEC_COLL_T;
            return (SPEC_COLL_T);
	} else {
	    sprintf(mysPathName->outArgv, "%s%s",
	     mdasCollectionName, (char *)(mysPathName->inpArgv + 1));
	}
    } else if (mysPathName->inpArgv[0] == '.' && 
     mysPathName->inpArgv[1] == '.') {	/* case for ".." */
        if (len == 2 || (mysPathName->inpArgv[2] == '/' && len == 3)) {
	    clSplitbychar(mdasCollectionName, mysPathName->outArgv, temp,'/');
            if (mysPathName->outArgv[0] == '\0') {
                mysPathName->outArgv[0] = '/';  /* root dir */
                mysPathName->outArgv[1] = '\0';
            }
            mysPathName->type = SPEC_COLL_T;
            return (SPEC_COLL_T);
        }
	if (mysPathName->inpArgv[2] == '/') {	/* case for "../" */
            clSplitbychar(mdasCollectionName,temp1, temp,'/');
            sprintf(mysPathName->outArgv, "%s%s", temp1, 
              (char *)(mysPathName->inpArgv + 2));
	} else {
	      fprintf(stderr, "inconsistent name: %s\n", mysPathName->inpArgv);
	      return CLI_ERR_COLLNAME;
	}
    } else if (mysPathName->inpArgv[0] == '/') {
	  strcpy(mysPathName->outArgv, mysPathName->inpArgv);
    } else {
	sprintf(mysPathName->outArgv, "%s/%s", 
	  defCollection,mysPathName->inpArgv);
    }

    /* Take care of any "./" and "/" in between */

    len = strlen (mysPathName->outArgv);
    cptr1 = mysPathName->outArgv + len - 2;
    while (cptr1 >= mysPathName->outArgv) {
	if (*cptr1 == '/' && *(cptr1 + 1) == '/') {	/* A "//" pattern */
	    strcpy (cptr1, (cptr1 + 1));
	} else if (*cptr1 == '.' && *(cptr1 + 1) == '/') {
	    if (*(cptr1 - 1) == '/') {	/* we have a "/./" pattern */
		strcpy (cptr1, (cptr1 + 2));	/* just eliminate "./" */
	    }
	}
        cptr1 --;
    }

    /* Take care of any "../" in between */

    len = strlen (mysPathName->outArgv);
    cptr1 = mysPathName->outArgv + len - 4;
    while (cptr1 >= mysPathName->outArgv) {
	if (*cptr1 == '/' && *(cptr1 + 1) == '.' &&
	  *(cptr1 + 2) == '.' && *(cptr1 + 3) == '/') {
	    /* we have a "/../" pattern. Skip to the next "/" */
	    cptr2 = (cptr1 + 4);	/* point to the start of the tail */
	    cptr1 --;	/* skip pass the current "/" */
	    while (cptr1 >= mysPathName->outArgv && *cptr1 != '/') {
	    	cptr1 --;
	    }
	    if (*cptr1 == '/')
	        strcpy (cptr1, cptr2);
	    else
	       return CLI_ERR_COLLNAME;
	}
	cptr1 --;
    }
    /* Now handle the last 3 char to take care of "/.", "/..", etc */

    len = strlen (mysPathName->outArgv);
    cptr1 = mysPathName->outArgv + len - 1;

    if (len >= 2 && *cptr1 == '.' && *(cptr1 - 1) == '/') {
	if (len == 2)
	    *cptr1 = '\0';
	else
	    *(cptr1 - 1) = '\0';
	mysPathName->type = SPEC_COLL_T;
    } else if (len >= 3 && *cptr1 == '.' && *(cptr1 - 1) == '.' &&
      *(cptr1 - 2) == '/') {
	if (len == 3)
	    return CLI_ERR_COLLNAME;

        /* we have a "/../" pattern. Skip to the next "/" */
        cptr1 -= 3;   /* skip pass the "/" */
        while (cptr1 >= mysPathName->outArgv && *cptr1 != '/') {
            cptr1 --;
        }
        if (*cptr1 == '/')
	    *cptr1 = '\0';
        else
           return CLI_ERR_COLLNAME;
	mysPathName->type = SPEC_COLL_T;
    } else {
	mysPathName->type = DATA_COLL_T;
    }

    /*
      Thanks to Syam Gadde (BIRN) for adding this
    */
    cptr1 = mysPathName->outArgv + len - 1;
    if (len > 1 && *cptr1 == '/') {
      *cptr1 = '\0';
    }
	
    return mysPathName->type;
}

int
getSrbObjNameFromCondition(srbConn *conn, int catType,
			   char *inCondition,srbPathName nameArray[], int nArgv,
			   int L_val)
{
  int i;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  mdasC_sql_result_struct dataResult;
  char *dataList, *collList;

  for (i = 0; i < MAX_DCS_NUM; i++) {
    sprintf(qval[i],"");
    selval[i] = 0;
  }
  i = fillQvalWithCondition(inCondition, qval);
  if (i != 0) return(i);
  selval[DATA_NAME] = 1;
  selval[DATA_GRP_NAME] = 1;
  i =  srbGetDataDirInfo(conn, catType, qval, selval, &dataResult, L_val);
  if (i < 0)
    return(i);
  dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) &dataResult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
  collList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) &dataResult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
  for (i = 0; i < dataResult.row_count; i++) { 
    nameArray[i].outArgv = malloc (MAX_OBJ_NAME_LENGTH);
    sprintf(nameArray[i].outArgv,"%s/%s",collList,dataList);
    nameArray[i].type = DATANAME_T;
    collList += MAX_DATA_SIZE;
    dataList += MAX_DATA_SIZE;
  }
  nArgv = i;
  clearSqlResult (&dataResult);
  return(0);
}

int
convNameArrayToSQL (srbPathName nameArray[], int nArgv)
{
    int i;
    int status;

    for (i = 0; i < nArgv; i++) {
	if ((status = parseName (&nameArray[i], mdasCollectionName) < 0))
	    return (status);
	/****        status = make_like_for_mdas(nameArray[i].outArgv);
        if (status < 0) {
            fprintf (stderr,"Collection name error:%s\n",nameArray[i].inpArgv);
            return status;
        }
	removed to avoid double like changing
	****/
    }
    return (0);
}

int
convContNameArrayToSQL (srbPathName nameArray[], int nArgv, char *userName,
char *domainName, char *mcatName)
{
    int i;
    char defContHome[MAX_TOKEN];
    int status;

#ifdef FED_MCAT
    if (mcatName != NULL && strlen (mcatName) > 0) {
        sprintf (defContHome, "/%s%s/%s.%s", mcatName,
         CONTAINER_HOME, userName, domainName);
    } else {
        sprintf (defContHome, "%s/%s.%s", CONTAINER_HOME, userName, domainName);
    }
#else
    sprintf (defContHome, "%s/%s.%s", CONTAINER_HOME, userName, domainName);
#endif
    for (i = 0; i < nArgv; i++) {
        if ((status = parseName (&nameArray[i], defContHome) < 0))
            return (status);
        /**** status = make_like_for_mdas(nameArray[i].outArgv);
        if (status < 0) {
            fprintf (stderr,"Collection name error:%s\n",nameArray[i].inpArgv);
            return status;
        }
	removed to avoid double like changing
	****/
    }
    return (0);
}

/*
int checkCollection(srbConn *conn, int catType, char *collName)
{

  int i,ii;
   mdasC_sql_result_struct Result;

  ii = srbListCollect(conn, catType, collName, "C", &Result, MAX_GET_RESULTS);
  
  return (ii);

 
}
*/
int checkCollection(srbConn *conn, int catType, char *collName)
{

  int i,j,ii;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  mdasC_sql_result_struct  myresult;
  char *rval;
  int hasLike = 0;
    for (i = 0; i < MAX_DCS_NUM; i++)
      {
        sprintf(qval[i],"");
        selval[i] = 0;
      }   

   selval[DATA_GRP_NAME] = 1;
   /*** RAJA: changed Feb 12, 2003 because wild cards dont work ****
    ***if (has_like_chars(collName) == 1)
   ***/
   if ((hasLike = make_like_for_mdas(collName)) == 1)
     genCollLikeSQL (collName, qval[DATA_GRP_NAME], 0);
   /***     sprintf(qval[DATA_GRP_NAME]," like '%s   %%' ESCAPE '\\'", collName); ***/
   else
     genEqualSQL (qval[DATA_GRP_NAME], collName);
   j = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, 
				      &myresult, MAX_GET_RESULTS );
   if (hasLike ) {
     if (myresult.row_count != 1)
       j = NONUNIQUE_METADATA_ERROR;
     else {
       rval = (char *) getFromResultStruct(
		  (mdasC_sql_result_struct *) &myresult, 
		  dcs_tname[DATA_GRP_NAME], 
		  dcs_aname[DATA_GRP_NAME]);
       strcpy(collName,rval);
     }
   }

   clearSqlResult (&myresult);
   if (myresult.continuation_index >= 0)
   	srbGetMoreRows(conn, MDAS_CATALOG,myresult.continuation_index,
     	 &myresult, 0);
  return (j);

 
}

int checkDataset(srbConn *conn, int catType, char *collName, char *objName)
{

  int i,j,ii;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  mdasC_sql_result_struct  myresult;

    for (i = 0; i < MAX_DCS_NUM; i++)
      {
        sprintf(qval[i],"");
        selval[i] = 0;
      }   

   selval[DATA_NAME] = 1;
   if (make_like_for_mdas(collName) == 1)
#ifdef MCAT_VERSION_10
     sprintf(qval[DATA_GRP_NAME]," like '%s   %%' ESCAPE '\\'", collName);
#else
     sprintf(qval[DATA_GRP_NAME]," like '%s%%' ESCAPE '\\'", collName);
#endif
   else
     genEqualSQL (qval[DATA_GRP_NAME], collName);

   if (make_like_for_mdas(objName) == 1)
#ifdef MCAT_VERSION_10
     sprintf(qval[DATA_NAME]," like '%s   %%' ESCAPE '\\'", objName);
#else
     sprintf(qval[DATA_NAME]," like '%s%%' ESCAPE '\\'", objName);
#endif
   else
     genEqualSQL (qval[DATA_NAME], objName);

   j = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, 
				      &myresult, MAX_GET_RESULTS );
   clearSqlResult (&myresult);
   if (myresult.continuation_index >= 0)
        srbGetMoreRows(conn, MDAS_CATALOG,myresult.continuation_index,
         &myresult, 0);

   return (j);
}

void
free_result_struct(int selval[], mdasC_sql_result_struct *myresult)
{

  clearSqlResult (myresult);
  /**** RAJA Feb 10, 2005  changed to above to conform to a good free ***
  int i;
  char *rval;
     for (i=0; i < MAX_DCS_NUM; i++) {
       if(selval[i] > 0)
	 {
	   rval = (char *) getFromResultStruct
	     (myresult,
	      dcs_tname[i], dcs_aname[i]);
	   free (rval);
	 }
     }
     **** RAJA Feb 10, 2005  changed to conform to a good free ***/
}

void
show_results(srbConn *conn, char qval[][MAX_TOKEN], int selval[], 
	     mdasC_sql_result_struct *myresult, int D_case, int L_val)
{
  int i,ii, j, alive, cnt, DELETED, printedrowcnt;
  char *rval[MAX_DCS_NUM];
  char bstr[1000];
  char resstr[HUGE_STRING], tempstr[HUGE_STRING];
  int firsttime = 1;
  int AskVal = 1;
  int hasasked =  1;
  char inbuf[100];


  /*****
 if ( L_val == MAX_GET_RESULTS )
   AskVal = 0;   
   To make results default, show all Roman 11-10-00 */

 printedrowcnt =  0;
 selval[NONDISTINCT] = 0;
  printf("--------------------------- RESULTS ------------------------------\n");
while (firsttime || myresult->continuation_index >= 0)
 {
  strcpy(resstr,"");
  DELETED = 0;
  if (D_case == 0)
   {
     /* Dump the result */
     for (i=0; i < MAX_DCS_NUM; i++) {
       if(selval[i] > 0)
	 rval[i] = (char *) getFromResultStruct
	   ((mdasC_sql_result_struct *) myresult,
	    dcs_tname[i], dcs_aname[i]);
     }
     j = 0;
     alive = 1;
     cnt = 0;
     while ( j < myresult->row_count)
       {
	 for (i=0; i < MAX_DCS_NUM; i++) 
	   {
	     if(selval[i] > 0) 
	       {
		     if (strstr(rval[i],"$deleted") == NULL )
		       {
/* change format slightly per Nelson Zarate's suggestion
			 sprintf(tempstr, "%s :%s\n",dcs_aname[i],rval[i]);
*/
                         if (strcmp (dcs_aname[i], "coll_link") == 0) {
                             sprintf(tempstr,"%s: %s\n","collInheritBit",rval[i]);
                         } else {
                             sprintf(tempstr, "%s: %s\n",dcs_aname[i],rval[i]);
                         }
			 strcat(resstr,tempstr);
			 rval[i] += MAX_DATA_SIZE;
		       }
		     else
		       {
			 DELETED = 1;
			 rval[i] += MAX_DATA_SIZE;
		       }
		     
	       }
	   }
	 if (!DELETED) {printf("%s",resstr); cnt++;printedrowcnt++;
	                printf("-----------------------------------------------------------------\n");}
	 DELETED = 0;
	 j++;
	 strcpy(resstr,"");
       }
     /* printf("%i items displayed\n",j-1);
    if (j == myresult->row_count) printf("Possibly there are more answers\n");
     */

   }
 else  /* D_Case != 0 */
   {
    cnt = 0;
     for (i=0; i < MAX_DCS_NUM; i++) {
       if(selval[i] > 0)
	 {
	   rval[i] = (char *) getFromResultStruct
	     ((mdasC_sql_result_struct *) myresult,
	      dcs_tname[i], dcs_aname[i]);
	   if (D_case > 0 && AskVal > 0 && hasasked) 
	     printf("  %-*s",  D_case, dcs_aname[i]);
	   cnt++;
	 }
     }
     j = 0;
     if (D_case > 0 && AskVal > 0 && hasasked) 
       {printf("\n");
        strcpy(bstr,"");
        for (i=0; i < D_case; i++) strcat(bstr,"-");
        for (i=0; i < cnt   ; i++) printf("%s",bstr);
        printf("\n"); 
       }
     alive = 1;
     strcpy(resstr,"");
     while ( j < myresult->row_count) {
       for (i=0; i < MAX_DCS_NUM; i++) {
	 if(selval[i] > 0) {
	       if (strstr(rval[i],"$deleted") == NULL )
		 {
		   if (D_case > 0)
		     sprintf(tempstr, "  %-*s", D_case, rval[i]);
		   else
		     sprintf(tempstr, " '%s'",rval[i]);
		   strcat(resstr,tempstr);
		   
		   rval[i] += MAX_DATA_SIZE;
		 }
	       else
		 {
		   DELETED = 1;
		   rval[i] += MAX_DATA_SIZE;
		 }
	 }
       }
       if (!DELETED) { printf("%s\n",resstr);printedrowcnt++;} 
       strcpy(resstr,"");
       DELETED = 0; j++;
     }
     /*     printf("%i items displayed\n",j-1);
    if (j == myresult->row_count ) printf("Possibly there are more answers\n");
     */
   }
  clearSqlResult(myresult);

  /* getting next set of rows */
  if (myresult->continuation_index < 0) return;
  /*
  hasasked = 0;
  if (D_case >= 0 && AskVal == 1 && printedrowcnt >= L_val/2) 
    {
      printedrowcnt = 0; hasasked = 1;
      fprintf(stdout,"More (y/n/c/q/number)?");
      fgets(inbuf,100,stdin);
      if (inbuf[strlen(inbuf)-1] == 'n' || inbuf[strlen(inbuf)-1] == 'q')
	{
	  ii = srbGetMoreRows(conn, MDAS_CATALOG,myresult->continuation_index,
			      myresult, 0);
	  return;
	}
      else if (inbuf[strlen(inbuf)-1] == 'c') AskVal = 0;
      else if(atoi(inbuf) > 0) L_val = atoi(inbuf);
    }
  */
      ii = srbGetMoreRows(conn, MDAS_CATALOG,myresult->continuation_index, 
		      myresult, L_val);
  if (myresult == NULL || ii != 0) 
    return;
  
 }
}

int
show_addl_results(srbConn *conn, char qval[][MAX_TOKEN], int selval[], 
	    mdasC_sql_result_struct *myresult, int D_case,int first, int L_val)
{
  int i,ii, j, alive, cnt, DELETED;
  char *rval[MAX_DCS_NUM];
  int printedrowcnt;
  char bstr[1000];
  char resstr[HUGE_STRING], tempstr[HUGE_STRING];
  int firsttime = 1;
  int AskVal = 1;
  int hasasked = 1;
  char inbuf[100];
 
  printedrowcnt = 0;
while (firsttime || myresult->continuation_index >= 0)
 {
  strcpy(resstr,"");
  DELETED = 0;
  if (D_case == 0)
   {
     /* Dump the result */
     for (i=0; i < MAX_DCS_NUM; i++) {
       if(selval[i] > 0)
	 rval[i] = (char *) getFromResultStruct
	   ((mdasC_sql_result_struct *) myresult,
	    dcs_tname[i], dcs_aname[i]);
     }
     j = 0;
     alive = 1;
     cnt = 0;
     while ( j <myresult->row_count)
       {
	 for (i=0; i < MAX_DCS_NUM; i++) 
	   {
	     if(selval[i] > 0) 
	       {
		     if (strstr(rval[i],"$deleted") == NULL )
		       {
/* change format slightly per Nelson Zarate's suggestion
			 sprintf(tempstr, "%s :%s\n",dcs_aname[i],rval[i]);
*/
			 if (strcmp (dcs_aname[i], "coll_link") == 0) {
			     sprintf(tempstr,"%s: %s\n","collInheritBit",rval[i]);
			 } else {
			     sprintf(tempstr, "%s: %s\n",dcs_aname[i],rval[i]);
			 }
			 strcat(resstr,tempstr);
			 rval[i] += MAX_DATA_SIZE;

		       }
		     else
		       {
			 DELETED = 1;
			 rval[i] += MAX_DATA_SIZE;
		       }
	       }
	   }
	 if (!DELETED) {printf("%s",resstr); cnt++; printedrowcnt++;
	 printf("-----------------------------------------------------------------\n");}

	 DELETED = 0;
	 j++;
	 strcpy(resstr,"");
      
       }

   }
 else
   {
    cnt = 0;

     for (i=0; i < MAX_DCS_NUM; i++) {
       if(selval[i] > 0)
	 {
	   rval[i] = (char *) getFromResultStruct
	     ((mdasC_sql_result_struct *) myresult,
	      dcs_tname[i], dcs_aname[i]);
	   if (D_case > 0  && AskVal > 0 && hasasked)
	     if (first == 0) printf("  %-*s",  D_case, dcs_aname[i]);
	   cnt++;
	 }
     }
     j = 0;
     if (first == 0) 
       {
	 if (D_case > 0  && AskVal > 0 && hasasked) 
	   {printf("\n");
	    strcpy(bstr,"");
	    for (i=0; i < D_case; i++) strcat(bstr,"-");
	    for (i=0; i < cnt   ; i++) printf("--%s",bstr);
	    printf("\n");
	   }
       }
     alive = 1;
     while ( j <  myresult->row_count) {
       for (i=0; i < MAX_DCS_NUM; i++) {
	 if(selval[i] > 0) {
	       if (strstr(rval[i],"$deleted") == NULL )
		 {
		   if (D_case > 0)
		     sprintf(tempstr, "  %-*s", D_case, rval[i]);
		   else
		     sprintf(tempstr, " '%s'",rval[i]);
		   strcat(resstr,tempstr);
		   rval[i] += MAX_DATA_SIZE;
		   
		 }
	       else
		 {
		   DELETED = 1;
		   rval[i] += MAX_DATA_SIZE;
		 }
	 }
       }
       if (!DELETED ) { printf("%s\n",resstr);printedrowcnt++;}
       strcpy(resstr,"");
       DELETED = 0; j++;
     }
   }
  clearSqlResult(myresult);

  /* getting next set of rows */
  if (myresult->continuation_index < 0) return( first + j);
/*
  hasasked = 0;
  if (D_case >= 0 && AskVal == 1 && printedrowcnt  > L_val /2 ) 
    {
      printedrowcnt = 0; hasasked = 1;
      fprintf(stdout,"More (y/n/c/q/number)?");
      fgets(inbuf,100,stdin);
      if (inbuf[strlen(inbuf)-1] == 'n' || inbuf[strlen(inbuf)-1] == 'q')
	{
	  ii = srbGetMoreRows(conn, MDAS_CATALOG,myresult->continuation_index,
			      myresult, 0);
	  return( first + j);
	}
      else if (inbuf[strlen(inbuf)-1] == 'c') AskVal = 0;
      else if(atoi(inbuf) > 0) L_val = atoi(inbuf);
    }
*/
  ii = srbGetMoreRows(conn, MDAS_CATALOG,myresult->continuation_index, 
		      myresult, L_val);
  if (myresult == NULL || ii != 0) 
    return( first + j);
 }
 return( first + j);
}

/* make_like_for_mdas - convert special char. 
 *
 * Input - dname - the name to convert.
 *
 * Output - The converted string is put back in dname.
 * 	    Returns values : < 0 ==> error
 *			     > 0 ==> No error. A like char exists.
 *			     = 0 ==> No error.
 */
/**** RAJA: changed Feb 12,2003 to make it more realistic ****
int make_like_for_mdas(char *dname)
{
  char oldname[MAX_OBJ_NAME_LENGTH];
  int i,j, len;
  int like_char = 0;

  strcpy(oldname, dname);
  len = strlen(oldname);
  for (i = 0,j = 0 ; i < len ; i++, j++)
    {
      if (oldname[i] == '&') {
	strcpy(&dname[j], &oldname[i]);
	return(like_char);
      }
      switch (oldname[i])
	{
	   case '*' :
	              dname[j] = '%';
		      like_char = 1;
		      break;
	   case '?' :
	              dname[j] = '_';
		      like_char = 1;
		      break;
	   case '_' :
	   case '\\' :
	   case '%' :
	              dname[j] = '\\';
		      j++;
	              dname[j] = oldname[i];
		      break;
	   default  :
	              dname[j] = oldname[i];
		      break;
	}         
    }

  if (j ==  MAX_TOKEN) {
    fprintf(stderr,"The expanded dataname is too long:%s\n", dname);
    return (CLI_ERR_NAME2LONG);
  }

  dname[j] = '\0';
  return (like_char);
}
******/
int escapeWildCards(char *dname)
{
  char oldname[MAX_OBJ_NAME_LENGTH];
  int i,j, len;
  int like_char = 0;

  strcpy(oldname, dname);
  len = strlen(oldname);
  for (i = 0,j = 0 ; i < len ; i++, j++)
    {
      if (oldname[i] == '&') {
	strcpy(&dname[j], &oldname[i]);
	return(like_char);
      }
      switch (oldname[i])
	{
	   case '_' :
	   case '\\' :
	   case '%' :
		       dname[j] = '\\';
		       j++;
	              dname[j] = oldname[i];
		      break;
	   default  :
	              dname[j] = oldname[i];
		      break;
	}         
    }

  if (j ==  MAX_TOKEN) {
    fprintf(stderr,"The expanded dataname is too long:%s\n", dname);
    return (CLI_ERR_NAME2LONG);
  }

  dname[j] = '\0';
  return (like_char);
}
int make_like_for_mdas(char *dname)
{
  char oldname[MAX_OBJ_NAME_LENGTH];
  int i,j, len;
  int like_char = 0;

  strcpy(oldname, dname);
  len = strlen(oldname);
  if (strstr(oldname,"*") || strstr(oldname,"?"))
    like_char = 1;
  for (i = 0,j = 0 ; i < len ; i++, j++)
    {
      if (oldname[i] == '&') {
	strcpy(&dname[j], &oldname[i]);
	return(like_char);
      }
      switch (oldname[i])
	{
	   case '*' :
	              dname[j] = '%';
		      like_char = 1;
		      break;
	   case '?' :
	              dname[j] = '_';
		      like_char = 1;
		      break;
	   case '_' :
	   case '\\' :
	   case '%' :
	             if (like_char == 1) {
		       dname[j] = '\\';
		       j++;
		     }
	              dname[j] = oldname[i];
		      break;
	   default  :
	              dname[j] = oldname[i];
		      break;
	}         
    }

  if (j ==  MAX_TOKEN) {
    fprintf(stderr,"The expanded dataname is too long:%s\n", dname);
    return (CLI_ERR_NAME2LONG);
  }

  dname[j] = '\0';
  return (like_char);
}

int has_like_chars(char *str)
{
  int i,j, len;

  /** error checking **/
  if ( str == NULL )       /* Roman 9-7-00 */
     return(0);

  len = strlen(str);
  for (i = 0 ; i < len ; i++)
    {
      if (str[i] == '&') {      /* start of conditional input */
        return(0);
      }

      if (str[i] == '%' && (i == 0 || str[i-1] != '\\')) return (1);
      if (str[i] == '_' && (i == 0 || str[i-1] != '\\')) return (1);
    }
  return(0); 
}

/* genEqualSQL - generate a equal SQL qualifier from inStr to outStr.
 * Take out the '\\' in the "\_" or "\%" string.
 */

int
genEqualSQL (char *outStr, char *inStr)
{
    char *outPtr, *inPtr;
    
    if (strlen(inStr) == 0)
      return(0);
    if (outStr == NULL)
      return (-1);
    strcpy (outStr, " = '");
    outPtr = outStr + strlen (outStr);
    inPtr = inStr;

    while (inPtr != NULL && *inPtr != '\0') {
        if (*inPtr == '\\' && (inPtr[1] == '%' || inPtr[1] == '_')) {
            /* skip the '\\' */
            inPtr ++;
        } else {
            *outPtr = *inPtr;
            inPtr ++;
            outPtr ++;
        }
    }

    /* Fill in the ending stuff */

    strcpy (outPtr, "'");

    return (0);
}

int
unmake_like_str (char *outStr, char *inStr)
{
    char *outPtr, *inPtr;

    if (outStr == NULL)
        return (-1);

    outPtr = outStr;
    inPtr = inStr;

    while (*inPtr != '\0') {
        if (*inPtr == '\\' && (inPtr[1] == '%' || inPtr[1] == '_')) {
            /* skip the '\\' */
            inPtr ++;
        } else {
            *outPtr = *inPtr;
            inPtr ++;
            outPtr ++;
        }
    }
    *outPtr = '\0';
    return (0);
}

void
expand_file_names(srbConn *conn, int start, int end,
		  char **argv, int *newargc, 
		  char newargv[][MAX_OBJ_NAME_LENGTH])
{
  char collname[MAX_TOKEN], oldcollname[MAX_TOKEN];
  char dataname[MAX_TOKEN], olddataname[MAX_TOKEN];
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  char tmpData[MAX_TOKEN], condition[MAX_OBJ_NAME_LENGTH];
  int  selval[MAX_DCS_NUM];
  mdasC_sql_result_struct  myresult;
  int i,j, alive;
  int ckd,ckc;
  char *rval[MAX_DCS_NUM];
  int localnewargc;

    for (i = 0; i < MAX_DCS_NUM; i++)
      {
        sprintf(qval[i],"");
        selval[i] = 0;
      }   
    if (strlen(inCondition) > 0) 
      fillQvalWithCondition(inCondition, qval);

    localnewargc = 1;
    for (i = start; i < end ; i++)
      {
            strcpy(condition,"");
	    strcpy(collname,"");
	    strcpy(dataname,"");
            splitString(argv[i], tmpData, condition, '&');
	    splitpath(tmpData,collname,dataname);
	    if (strlen(dataname) == 0) strcpy(dataname,"*");
	    strcpy(oldcollname,collname);
	    strcpy(olddataname,dataname);
	    ckd = make_like_for_mdas(dataname);
	    if (ckd < 0) {
	      fprintf(stderr,"Dataname too long:%s\n",argv[i]);
	      break;
	      /*{clFinish(conn); exit(3);}*/
	    }
	    ckc = make_like_for_mdas(collname);
	    if (ckc < 0) {
	      fprintf(stderr,"Collection name too long:%s\n",argv[i]);
	      break;
	      /*{clFinish(conn); exit(3);}*/
	    }

	    if (!strcmp(dataname, olddataname) && 
		!strcmp(collname, oldcollname))
	      {
		if (strlen(condition) > 0)
		  sprintf(newargv[localnewargc],"%s/%s&%s", collname, dataname,condition);
		else
		  sprintf(newargv[localnewargc],"%s/%s", collname, dataname);
		localnewargc++;
	      }
	    else /* not same as old */
	      {
		selval[DATA_NAME] = 1;
		selval[DATA_GRP_NAME] = 1;
		if (ckd == 1)
#ifdef MCAT_VERSION_10
		  sprintf(qval[DATA_NAME]," like '%s   %%' ESCAPE '\\'",dataname);
#else
		  sprintf(qval[DATA_NAME]," like '%s%%' ESCAPE '\\'",dataname);
#endif
		else
		  genEqualSQL (qval[DATA_NAME], dataname);
		if (ckc == 1)
#ifdef MCAT_VERSION_10
		  sprintf(qval[DATA_GRP_NAME]," like '%s   %%' ESCAPE '\\'",collname);
#else
		  sprintf(qval[DATA_GRP_NAME]," like '%s%%' ESCAPE '\\'",collname);
#endif
		else
		  genEqualSQL (qval[DATA_GRP_NAME], collname);
		j = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, 
				      &myresult, MAX_GET_RESULTS );
		if (j < 0)
		  {
		   sprintf(qval[DATA_NAME],"");
		   if (strlen(dataname) != 0)
		     {


                       if (ckc || ckd)
#ifdef MCAT_VERSION_10
			 sprintf(qval[DATA_GRP_NAME], " like '%s/%s   %%' ESCAPE '\\'",collname,dataname);
#else
		         sprintf(qval[DATA_GRP_NAME]," like '%s/%s%%' ESCAPE '\\'",collname,dataname);
#endif
                       else {
                         sprintf (tmpData, "%s/%s", collname, dataname);
                         genEqualSQL (qval[DATA_GRP_NAME], tmpData);
                       }
		     }
		   /*sprintf(qval[DATA_GRP_NAME],
			     " like '%s/%s  %%' ESCAPE '\\'",collname,dataname);*/
		   else
		     {
		       if (ckc == 1)
#ifdef MCAT_VERSION_10
			 sprintf(qval[DATA_GRP_NAME], " like '%s   %%' ESCAPE '\\'",collname);
#else
		         sprintf(qval[DATA_GRP_NAME], " like '%s%%' ESCAPE '\\'",collname);
#endif
		       else
			 genEqualSQL (qval[DATA_GRP_NAME], collname);
	      }
		     /*  sprintf(qval[DATA_GRP_NAME],
			     " like '%s  %%' ESCAPE '\\'",    collname); */
		   j = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, 
				      &myresult, MAX_GET_RESULTS);
		   if (j < 0)
		     {
		       if (j != CLI_NO_ANSWER)
			 fprintf(stderr,"Error:%i:%s/%s\n", j,
				 oldcollname,olddataname);
		       /*else fprintf(stderr,"No Answer:%s/%s\n",
				    oldcollname,olddataname);*/
		       break;
		     }
	      }
		rval[DATA_NAME] = (char *) getFromResultStruct(
				    (mdasC_sql_result_struct *) &myresult, 
				    dcs_tname[DATA_NAME], 
				       dcs_aname[DATA_NAME]);
		rval[DATA_GRP_NAME] = (char *) getFromResultStruct(
				       (mdasC_sql_result_struct *) &myresult, 
				       dcs_tname[DATA_GRP_NAME], 
				       dcs_aname[DATA_GRP_NAME]);

		j = 0;
		while (j < myresult.row_count)
		    {
		      if (strstr(rval[DATA_NAME],"$deleted") == NULL )
			{
			  if (strlen(condition) > 0)
			    sprintf(newargv[localnewargc],"%s/%s&%s",
				    rval[DATA_GRP_NAME],rval[DATA_NAME],condition);
			  else
			    sprintf(newargv[localnewargc],"%s/%s",
				    rval[DATA_GRP_NAME],rval[DATA_NAME]);
			  localnewargc++;
			}
		      rval[DATA_NAME] += MAX_DATA_SIZE;
		      rval[DATA_GRP_NAME] += MAX_DATA_SIZE;
		      /* if (localnewargc > MAX_ARG_EXP_CNT)*/
		      if (localnewargc >= MAX_TOKEN)
			{
			  fprintf(stderr,
				  "Argument expansion out of bounds:%s:%s\n",
				  argv[i], newargv[localnewargc-1]);
			  clFinish(conn); exit(3);
			}
		      j++;
		    }
	      }
	  
      }
    *newargc = localnewargc;
}
      

void
expand_and_check_file_names(srbConn *conn, int start, int end,
		  char **argv, int *newargc, 
		  char newargv[][MAX_TOKEN])
{
char collname[MAX_TOKEN], oldcollname[MAX_TOKEN];
  char dataname[MAX_TOKEN], olddataname[MAX_TOKEN];
  char tmpData[MAX_TOKEN];
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  mdasC_sql_result_struct  myresult;
  int i,j, alive;
  char *rval[MAX_DCS_NUM];
  char *pval1;
  char *pval2;
  int localnewargc;
  int ckc,ckd;

  for (i = 0; i < MAX_DCS_NUM; i++)
    {
      sprintf(qval[i],"");
      selval[i] = 0;
    }   

            if (strlen(inCondition) > 0) 
	      fillQvalWithCondition(inCondition, qval);
	      localnewargc = 1;
    for (i = start; i < end ; i++)
      {
            strcpy(collname,"");
	    strcpy(dataname,"");
	    splitpath(argv[i],collname,dataname);
	    if (strlen(dataname) == 0) strcpy(dataname,"*");
	    strcpy(oldcollname,collname);
	    strcpy(olddataname,dataname);
	    ckd = make_like_for_mdas(dataname);
	    if (ckd < 0) {
	      fprintf(stderr,"Dataname too long:%s\n",argv[i]);
	      break;
	      /*{clFinish(conn); exit(3);}*/
	    }
	    ckc = make_like_for_mdas(collname);
	    if (ckc < 0) {
	      fprintf(stderr,"Collection name too long:%s\n",argv[i]);
	      break;
	      /*{clFinish(conn); exit(3);}*/
	    }
	      
		selval[DATA_NAME] = 1;
		selval[DATA_GRP_NAME] = 1;
		if (ckd == 1)
#ifdef MCAT_VERSION_10
		  sprintf(qval[DATA_NAME]," like '%s   %%' ESCAPE '\\'",dataname);
#else
		  sprintf(qval[DATA_NAME]," like '%s%%' ESCAPE '\\'",dataname);
#endif
		else
		  genEqualSQL (qval[DATA_NAME], dataname);
		if (ckc == 1)
#ifdef MCAT_VERSION_10
		  sprintf(qval[DATA_GRP_NAME]," like '%s   %%' ESCAPE '\\'",collname);
#else
		  sprintf(qval[DATA_GRP_NAME]," like '%s%%' ESCAPE '\\'",collname);
#endif
		else
		  genEqualSQL (qval[DATA_GRP_NAME], collname);
		/* sprintf(qval[DATA_NAME]," like '%s  %%' ESCAPE '\\'",dataname);
		sprintf(qval[DATA_GRP_NAME]," like '%s  %%' ESCAPE '\\'",
		    collname); */
		j = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, 
				      &myresult, MAX_GET_RESULTS);
		if (j < 0)
		  {
		   sprintf(qval[DATA_NAME],"");
		   if (strlen(dataname) != 0)
		     {
		       if (ckc == 1)
			 sprintf(qval[DATA_GRP_NAME],
#ifdef MCAT_VERSION_10
			     " like '%s/%s   %%' ESCAPE '\\'",collname,dataname);
#else
			     " like '%s/%s%%' ESCAPE '\\'",collname,dataname);
#endif
                       else {
                         sprintf (tmpData, "%s/%s", collname,dataname);
                         genEqualSQL (qval[DATA_GRP_NAME], tmpData);
                       }
		     }
		     /* sprintf(qval[DATA_GRP_NAME],
			     " like '%s/%s  %%' ESCAPE '\\'",
			     collname,dataname);*/
		   else
		     {
		       if (ckc == 1)
			 sprintf(qval[DATA_GRP_NAME],
#ifdef MCAT_VERSION_10
			     " like '%s   %%' ESCAPE '\\'",collname);
#else
			     " like '%s%%' ESCAPE '\\'",collname);
#endif
		       else
			 genEqualSQL (qval[DATA_GRP_NAME], collname);
		     }
		     /* sprintf(qval[DATA_GRP_NAME],
			" like '%s  %%' ESCAPE '\\'", collname); */
		   j = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, 
				      &myresult, MAX_GET_RESULTS);
		   if (j < 0)
		     {
		       if (j != CLI_NO_ANSWER)
			 fprintf(stderr,"Error:%i:%s/%s\n", j,
				 oldcollname,olddataname);
		       /*else fprintf(stderr,"No Answer:%s/%s\n",
				    oldcollname,olddataname);*/
		       break;
		     }
		  }
		rval[DATA_NAME] = (char *) getFromResultStruct(
				    (mdasC_sql_result_struct *) &myresult, 
				    dcs_tname[DATA_NAME], 
				       dcs_aname[DATA_NAME]);
		rval[DATA_GRP_NAME] = (char *) getFromResultStruct(
				       (mdasC_sql_result_struct *) &myresult, 
				       dcs_tname[DATA_GRP_NAME], 
				       dcs_aname[DATA_GRP_NAME]);
		pval1 = rval[DATA_NAME];
		pval2 = rval[DATA_GRP_NAME];
		j = 0;
		alive = 1;
		while ( j < myresult.row_count) {
		      if (strstr(rval[DATA_NAME],"$deleted") == NULL )
			{
			  sprintf(newargv[localnewargc],"%s/%s",
				  rval[DATA_GRP_NAME],rval[DATA_NAME]);
			  localnewargc++;
			}
		      rval[DATA_NAME] += MAX_DATA_SIZE;
		      rval[DATA_GRP_NAME] += MAX_DATA_SIZE;
		      /*      if (localnewargc > MAX_ARG_EXP_CNT) */
		      if (localnewargc >= MAX_TOKEN)
			{
			  fprintf(stderr,
				  "Argument expansion out of bounds:%s:%s\n",
				  argv[i],newargv[localnewargc-1]);
			  clFinish(conn); exit(3);
			}
		  j++;
		}
		free(pval1);
		free(pval2);
	      
      }
    *newargc = localnewargc;
}


void
new_expand_file_names_for_like(srbConn *conn, int start, int end,
		  char **argv, int *newargc, 
		  char newargv[][MAX_OBJ_NAME_LENGTH])
{
  char collname[MAX_TOKEN];
  char dataname[MAX_TOKEN];
  int j;
  int localnewargc;



    localnewargc = 1;
    for (i = start; i < end ; i++)
      {
	    splitpath(argv[i],collname,dataname);
	    sprintf(newargv[localnewargc],"%s/%s", collname, dataname);
	    localnewargc++;
	  
      }
    *newargc = localnewargc;
}
void
expand_file_names_for_like(srbConn *conn, int start, int end,
		  char **argv, int *newargc, 
		  char newargv[][MAX_OBJ_NAME_LENGTH])
{
  char collname[MAX_TOKEN];
  char dataname[MAX_TOKEN];
  int j;
  int localnewargc;



    localnewargc = 1;
    for (i = start; i < end ; i++)
      {
	    splitpath(argv[i],collname,dataname);
	    j = make_like_for_mdas(dataname);
	    if (j < 0) {
	      fprintf(stderr,"Dataname too long:%s\n",argv[i]);
	      break;
	      /*{clFinish(conn); exit(3);}*/
	    }
	    j = make_like_for_mdas(collname);
	    if (j < 0) {
	      fprintf(stderr,"Collection name too long:%s\n",argv[i]);
	      break;
	      /*{clFinish(conn); exit(3);}*/
	    }
	    sprintf(newargv[localnewargc],"%s/%s", collname, dataname);
	    localnewargc++;
	  
      }
    *newargc = localnewargc;
}

int
getDefResr(char *defId, char *defRName, char *defRPath)
{
  int i; 
   for (i = 0 ; i < defResrMax; i++)
    {
      if (!strcmp(defId, defResrId[i]))
	{
	  strcpy(defRName, defResrName[i]);
	  strcpy(defRPath, defResrPath[i]);
	  return (0);
	}
    }
  return (UTIL_ERROR_DEFAULT_RSRC_NOT_FOUND);
}

char *
splitStringfromEnd(char *st1, char *st2, char *st3, char key)
{
    int i,j,k;
    int inLen;
 
    inLen = strlen (st1);
    if (inLen <= 0) {
        st2[0] = '\0';
        st3[0] = '\0';
        return NULL;
    }
    strcpy(st2,st1);
    strcpy(st3,st1);
    for (i = inLen-1; (i >= 0 && st1[i] != key) ; i--){
    }
    if (i >=0)
      st2[i] = '\0';
    else st2[0] = '\0';
 
    if (st1[i] != key)
        return NULL;
 
    strcpy(st3, (char *) &st1[i+1]);
    return &st1[i+1];
}

char *
splitString(char *st1, char *st2, char *st3, char key)
{
    int i,j,k;
    int inLen;
 
    inLen = strlen (st1);
    if (inLen <= 0) {
        st2[0] = '\0';
        st3[0] = '\0';
        return NULL;
    }
 
    for (i = 0; (i < inLen &&  st1[i] != key) ; i++){
         st2[i] = st1[i];
    }
 
    st2[i] = '\0';
 
    if (st1[i] != key)
        return NULL;
 
    strcpy(st3, (char *) &st1[i+1]);
    
    return &st1[i+1];
}

  
int
get_user_preference(char *resource, char *select)
{
  int i; 
   for (i = 0 ; i < defResrMax; i++)
    {
      if (!strcmp(select, defResrId[i]))
	{
	  strcpy(resource, defResrName[i]);
	  return (0);
	}
    }
   strcpy(resource, defaultResource);
  return (0);
}

void
trylocaldatatype(char *datatypename, char *pathName)
{

  char *intype;
  char tmpstr1[MAX_TOKEN];
  char tmpstr2[MAX_TOKEN];
  char tmpstr3[MAX_TOKEN];

  if (splitStringfromEnd(pathName, tmpstr1, tmpstr2, '.') == NULL) { 
    strcpy(datatypename,"generic");
    return;
  }
  if (splitStringfromEnd(tmpstr2,  tmpstr1, tmpstr3, '/') != NULL) {
    strcpy(datatypename,"generic");
    return;
  }

  if (!strcmp(tmpstr2, "c"))
      strcpy(datatypename, "C code");
  else if (!strcmp(tmpstr2, "h"))
      strcpy(datatypename, "C include file");
  else if (!strcmp(tmpstr2, "txt") || !strcmp(tmpstr2, "text"))
      strcpy(datatypename, "ascii text");
  else if (!strcmp(tmpstr2, "z") || !strcmp(tmpstr2, "gz") )
      strcpy(datatypename, "ascii compressed Lempel-Ziv");
  else if (!strcmp(tmpstr2, "tar"))
      strcpy(datatypename, "tar file");
  else if (!strcmp(tmpstr2, "tgz"))
      strcpy(datatypename, "compressed tar file");
  else if (!strcmp(tmpstr2, "jav ") || !strcmp(tmpstr2, "java"))
      strcpy(datatypename, "java code");
  else if (!strcmp(tmpstr2, "gif"))
      strcpy(datatypename, "gif image");
  else if (!strcmp(tmpstr2, "jpeg") || !strcmp(tmpstr2, "jpg"))
      strcpy(datatypename, "jpeg image");
  else if (!strcmp(tmpstr2, "dvi"))
      strcpy(datatypename, "DVI format");
  else if (!strcmp(tmpstr2, "dat") || !strcmp(tmpstr2, "data"))
      strcpy(datatypename, "data");
  else if (!strcmp(tmpstr2, "tex"))
      strcpy(datatypename, "LaTex format");
  else if (!strcmp(tmpstr2, "ps"))
      strcpy(datatypename, "Postscript format");
  else if (!strcmp(tmpstr2, "htm") || !strcmp(tmpstr2, "html"))
      strcpy(datatypename, "html");
  else if (!strcmp(tmpstr2, "tif") || !strcmp(tmpstr2, "tiff"))
      strcpy(datatypename, "tiff image");
  else if (!strcmp(tmpstr2, "au"))
      strcpy(datatypename, "Wave Audio");
  else if (!strcmp(tmpstr2, "sql"))
      strcpy(datatypename, "SQL script");
  else if (!strcmp(tmpstr2, "bin"))
      strcpy(datatypename, "binary");
  else if (!strcmp(tmpstr2, "o"))
      strcpy(datatypename, "link code");
  else if (!strcmp(tmpstr2, "a"))
      strcpy(datatypename, "library code");
  else if (!strcmp(tmpstr2, "doc"))
      strcpy(datatypename, "MSWord Document");
  else if (!strcmp(tmpstr2, "pdf"))
      strcpy(datatypename, "PDF Document");
  else if (!strcmp(tmpstr2, "png"))
      strcpy(datatypename, "PNG-Portable Network Graphics");
  else if (!strcmp(tmpstr2, "bmp"))
      strcpy(datatypename, "BMP -Bit Map");
  else if (!strcmp(tmpstr2, "mp3"))
      strcpy(datatypename, "MP3 - MPEG Audio");
  else sprintf(datatypename,".%s",tmpstr2);
}

int getCollectionList(srbConn *conn, int catType, char *flag, char *collName, 
		      char **collList, int *colCount)
{

  int i,ii;
  mdasC_sql_result_struct myresult;
  char temp[MAX_TOKEN], temp1[MAX_TOKEN];

      sprintf(temp,"%s/alpha",collName);
      splitpath(temp,collName, temp1);

  ii = srbListCollect(conn, catType, collName, flag , &myresult, 
		      MAX_GET_RESULTS);
  if (ii !=0 ) return(ii);

  if (myresult.continuation_index >= 0)
    {
      free(myresult.sqlresult[0].values);
      return (UTIL_ERROR_MAX_RESULT_LIMIT_EXCEEDED);
    }
  *collList = myresult.sqlresult[0].values;
  *colCount = myresult.row_count;
  return (MDAS_SUCCESS);
}
 
/* convPathEnd - Handle name ending with "/" or "/." */
int 
convPathEnd (char *collPath) {
    int len, origLen;

    origLen = len = strlen (collPath);
    if (collPath[len-2] == '/' && collPath[len-1] == '.')
        len --;
    while (len > 1 && collPath[len-1] == '/')
        len --;
    if (len != origLen) {
    	collPath[len] = '\0';
	return SPEC_COLL_T;
    } else {
	return DATA_COLL_T;
    }
}

/* queryCollColl - List all collections generated by collName including the
 * effects of special char and the recursive flag and put the results in
 * collResult.
 *
 */

int
queryCollColl (srbConn *conn, int catType, char *collName, int flagval, 
mdasC_sql_result_struct *collResult)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;
    char *rval[MAX_DCS_NUM];
    int continuation_index;
    int result_count;
    int cursize;
    
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    collLike = make_like_for_mdas (collName); /***MLFM CHECK ****/
    /***** changed RAJA April 9, 2003
    if (collLike > 0 || (flagval & R_FLAG)) {  
        genCollLikeSQL (collName, qval[DATA_GRP_NAME], flagval & R_FLAG);
    } else {
	genEqualSQL (qval[DATA_GRP_NAME], collName);
    }
    ****/

    if ( (flagval & R_FLAG)) {  
      if (collLike > 0) 
	genCollLikeSQL (collName, qval[DATA_GRP_NAME], flagval & R_FLAG);
      else {
	escapeWildCards(collName);
	genCollLikeSQL (collName, qval[DATA_GRP_NAME], flagval & R_FLAG);
      }
    }
    else if (collLike > 0) 
      genCollLikeSQL (collName, qval[DATA_GRP_NAME], 0);
    else
      genEqualSQL (qval[DATA_GRP_NAME], collName);



    selval[DATA_GRP_NAME] = 1;

    status = srbGetDataDirInfo(conn, catType, qval, selval, collResult, 
     MAX_GET_RESULTS);
#ifdef foo	/* code for concatenating results but not needed */
    if (status >= 0) {
	continuation_index = collResult->continuation_index;
	result_count = collResult->result_count;
	cursize = collResult->row_count;
    } else {
	continuation_index = -1;
	result_count = -1;
    }

    while (continuation_index >= 0 && result_count > 0) {
	mdasC_sql_result_struct moreResult;

        status = srbGetMoreRows(conn, catType,
         continuation_index, &moreResult, MAX_GET_RESULTS);
	if (status == 0) {
	    result_count = moreResult.result_count;
	    if (result_count <= 0)
		break;
	    cursize = appendResult (collResult, &moreResult, cursize);
            clearSqlResult (&moreResult);
	    continuation_index = moreResult.continuation_index;
	} else
	    break;
    }
#endif

    return (status);
}

int
attachCondition(mdasC_sql_result_struct *dataResult,char *condition){

  char *srcCh;
  char temp[MAX_OBJ_NAME_LENGTH];
  int i;

  srcCh = (char *) getFromResultStruct(dataResult,
		  dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
  for (i = 0; i < dataResult->row_count;i++) {
    strcat(srcCh,"&");
    strcat(srcCh,condition);
    srcCh += MAX_DATA_SIZE;
  }
  return (0);
}

int
queryData (srbConn *conn, int catType, char *dataName, int flagval, int L_val,
mdasC_sql_result_struct *dataResult)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    char tmpCollName[MAX_TOKEN], tmpDataName[MAX_TOKEN];
    char tmpData[MAX_TOKEN], condition[MAX_OBJ_NAME_LENGTH];
    int i, j;
    int status;
    int likeFlag;

    if ((dataName == NULL || strlen (dataName) == 0) && strlen(inCondition) == 0 ) {
        dataResult->row_count = 0;
        return (MCAT_INQUIRE_ERROR);
    }

    strcpy(condition,"");
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strlen(inCondition) > 0) {
      i = fillQvalWithCondition(inCondition, qval);
      if (i != 0) return(i);
    }

    splitString(dataName, tmpData, condition, '&');
    mySplitpath (tmpData, tmpCollName, tmpDataName);
    if (strlen(qval[DATA_GRP_NAME]) == 0 ) {
      likeFlag = make_like_for_mdas (tmpCollName); /*** MLFM CHECK ****/
      if (likeFlag > 0) {  /* We have a like char */
        genCollLikeSQL (tmpCollName, qval[DATA_GRP_NAME], flagval & R_FLAG);
      } else {
	genEqualSQL (qval[DATA_GRP_NAME], tmpCollName);
      }
    }
    if (strlen(qval[DATA_NAME]) == 0 ) {
      likeFlag = make_like_for_mdas (tmpDataName); /*** MLFM CHECK ****/
      if (likeFlag > 0) {  /* We have a like char */
        genDataLikeSQL (tmpDataName, qval[DATA_NAME]);
      } else {
	genEqualSQL (qval[DATA_NAME], tmpDataName);
      }
    }
    selval[DATA_NAME] = 1;
    selval[DATA_GRP_NAME] = 1;

    if (flagval & Y_FLAG) {
        selval[DATA_CHECKSUM] = 1;
        selval[DATA_REPL_ENUM] = 1; 
        selval[IS_DIRTY] = 1;
    } 

    if (flagval & V_FLAG) {
        selval[PATH_NAME] = 1;
        selval[RSRC_ADDR_NETPREFIX] = 1;
        selval[PHY_RSRC_TYP_NAME] = 1;
        selval[CONTAINER_NAME] = 1;
        selval[OFFSET] = 1;
    } 

    if (flagval & d_FLAG) {	/* list file path */
        selval[PATH_NAME] = 1;
    }

    if (flagval & l_FLAG) {
/*
	sprintf(qval[CONTAINER_REPL_ENUM]," = %s%s",
	 MDASSCHEMENAME,"MDAS_ADC_REPL.repl_enum");
*/
        selval[SIZE] = 1;
        selval[IS_DIRTY] = 1;
        selval[DATA_TYP_NAME] = 1;
        selval[DATA_OWNER] = 1;
        selval[REPL_TIMESTAMP] = 1;
        selval[DATA_REPL_ENUM] = 1; 
/* removed to replace by DATA_REPL_ENUM, Raja Aug 10,2000
	selval[CONTAINER_REPL_ENUM] = 1;
*/
        selval[PHY_RSRC_NAME] = 1;
        selval[CONTAINER_NAME] = 1;
        /* added to eliminate unreal "IC" copy */
        selval[CONTAINER_SIZE] = 1;
        selval[OFFSET] = 1;
        sprintf(qval[ORDERBY], "DATA_NAME");
    } else if (flagval & C_FLAG) {
        selval[DATA_OWNER] = 1;
        selval[ACCESS_CONSTRAINT] = 1;
        selval[USER_NAME] = 1;
        selval[DOMAIN_DESC] = 1;
	selval[ACL_INHERITANCE_BIT] = 1;
    } else if (flagval & B_FLAG) {
        selval[DATA_OWNER] = 1;
        selval[DATA_OWNER_DOMAIN] = 1;
    } else {
	/* XXXXX a temporary implementation to make sure overwring just
	 * one file if in parallel mode */
	if ((flagval & m_FLAG) && (flagval & F_FLAG))
            selval[DATA_REPL_ENUM] = 1; 
        selval[SIZE] = 1;
        selval[IS_DIRTY] = 1;
        selval[DATA_TYP_NAME] = 1;
    }
   if (versionFlag == 1)
     selval[DATA_VER_NUM] = 1;

    status = srbGetDataDirInfo(conn, catType, qval, selval, dataResult,
      L_val);
    if (status == 0) {
	/* take out filterDeleted, but add filterDuplicate 
         * filterDeleted (dataResult);
	 */
        /* XXXXX a temporary implementation to make sure overwring just
         * one file if in parallel mode */

	if (flagval & Y_FLAG) {
	    filterDuplicate (dataResult);
	} else if ((flagval & l_FLAG) == 0 && ((flagval & m_FLAG) == 0 ||
	 (flagval & F_FLAG) == 0))
	    /* filterDuplicate (dataResult); */
	    filterDuplicateShort (dataResult);
	if (strlen(condition) > 0) 
	  {attachCondition(dataResult,condition);}
    }

    return (status);
}

/**** RAJA changed Feb 11 2003 to take care of wild cards **
void 
genCollLikeSQL (char *inColl, char *outSQL, int recurFlag)
{
 
    if (recurFlag) {
	sprintf(outSQL, " like '%s%%' ESCAPE '\\' || like '%s/%%' ESCAPE '\\'", 
	  inColl, inColl);
    } else {
	sprintf(outSQL, " like '%s%%' ESCAPE '\\' && not like '%s/%%' ESCAPE '\\'", inColl, inColl);
    }
}
*** RAJA changed Feb 11 2003 to take care of wild cards ***/
void 
genCollLikeSQL (char *inColl, char *outSQL, int recurFlag)
{

    if (recurFlag) {
#ifdef MCAT_VERSION_10
	sprintf(outSQL, " like '%s  %%' ESCAPE '\\' || like '%s/%%' ESCAPE '\\'", 
	  inColl, inColl);
#else
	sprintf(outSQL, " like '%s' ESCAPE '\\' || like '%s/%%' ESCAPE '\\'", 
	  inColl, inColl);
#endif
    } else {
#ifdef MCAT_VERSION_10
	sprintf(outSQL, " like '%s  %%' ESCAPE '\\' && not like '%s/%%' ESCAPE '\\'", inColl, inColl);
#else
	sprintf(outSQL, " like '%s' ESCAPE '\\' && not like '%s/%%' ESCAPE '\\'", inColl, inColl);
#endif
    }
}

void
genDataLikeSQL (char *inData, char *outSQL)
{

#ifdef MCAT_VERSION_10
    sprintf(outSQL, " like '%s  %%' ESCAPE '\\'", inData);
#else
    sprintf(outSQL, " like '%s%%' ESCAPE '\\'", inData);
#endif
}

/* querySubCollInColl - List the sub-collections in the parent collection */

int
querySubCollInColl (srbConn *conn, int catType, char *parColl, int flagval,
int L_val, mdasC_sql_result_struct  *myresult)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, status;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[DATA_GRP_NAME] = 1;
    if (flagval & C_FLAG) {
        selval[ACL_INHERITANCE_BIT] = 1;
    }
    sprintf(qval[PARENT_COLLECTION_NAME]," = '%s'", parColl);

    status = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval,
      myresult, L_val);

    if (status < 0) {
	if (status != MCAT_INQUIRE_ERROR)	/* no answer */
	    fprintf (stderr, "Error listing collection: %s, status = %d\n",
	  parColl, status);
    }
    return status;
}

/* queryDataInColl - Query the dataName in the parent collection and put
 * the result in myresult.
 */

int
queryDataInColl (srbConn *conn, int catType, char *parColl, int flagval, 
int L_val, mdasC_sql_result_struct *myresult)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i;
    int status;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strlen(inCondition) > 0) {
      i = fillQvalWithCondition(inCondition, qval);
      if (i != 0) return(i);
    }

    selval[DATA_NAME] = 1;
    selval[DATA_GRP_NAME] = 1;

    if (flagval & Y_FLAG) {
        selval[DATA_CHECKSUM] = 1;
        selval[DATA_REPL_ENUM] = 1;
        selval[IS_DIRTY] = 1;
	sprintf(qval[ORDERBY],"DATA_GRP_NAME, DATA_NAME");
    }

    if (flagval & V_FLAG) {
        selval[PATH_NAME] = 1;
        selval[RSRC_ADDR_NETPREFIX] = 1;
        selval[PHY_RSRC_TYP_NAME] = 1;
        selval[CONTAINER_NAME] = 1;
        selval[OFFSET] = 1;
    } 

    if (flagval & d_FLAG) {     /* list file path */
        selval[PATH_NAME] = 1;
    }

    if (flagval & l_FLAG) {
/*
        sprintf(qval[CONTAINER_REPL_ENUM]," = %s%s",
         MDASSCHEMENAME,"MDAS_ADC_REPL.repl_enum");
*/
        selval[SIZE] = 1;
        selval[IS_DIRTY] = 1;
        selval[DATA_TYP_NAME] = 1;
        selval[DATA_OWNER] = 1;
        selval[REPL_TIMESTAMP] = 1;
        selval[DATA_REPL_ENUM] = 1;
	/* added to eliminate unreal "IC" copy */
	selval[CONTAINER_SIZE] = 1;
	selval[OFFSET] = 1;
/* removed to replace by DATA_REPL_ENUM, Raja Aug 10,2000
        selval[CONTAINER_REPL_ENUM] = 1;
*/
        selval[PHY_RSRC_NAME] = 1;
        selval[CONTAINER_NAME] = 1;
	sprintf(qval[ORDERBY], "DATA_NAME");
    } else if (flagval & C_FLAG) {
        selval[DATA_OWNER] = 1;
        selval[ACCESS_CONSTRAINT] = 1;
        selval[USER_NAME] = 1;
        selval[DOMAIN_DESC] = 1;
	selval[ACL_INHERITANCE_BIT] = 1;
    } else if (flagval & B_FLAG) {
        selval[DATA_OWNER] = 1;
        selval[DATA_OWNER_DOMAIN] = 1;
    } else {
        selval[SIZE] = 1;
        selval[IS_DIRTY] = 1;
        selval[DATA_TYP_NAME] = 1;
    }

   /* For data name, use DATA_GRP_NAME rather than 
    * PARENT_COLLECTION_NAME as in the case of listColl. */

    if (flagval & y_FLAG) {  /* We have a like char */
        genCollLikeSQL (parColl, qval[DATA_GRP_NAME], 1);
    } else {
        sprintf(qval[DATA_GRP_NAME]," = '%s'", parColl);
    }
    if (versionFlag == 1)
     selval[DATA_VER_NUM] = 1;


    status = srbGetDataDirInfo(conn, catType, qval, selval,
      myresult, L_val);
    if (status == 0)
	/* take out filterDeleted, but add filterDuplicate
         * filterDeleted (dataResult);
         */
        if (flagval & Y_FLAG) {
            filterDuplicate (myresult);
        } else if ((flagval & l_FLAG) == 0) {
            /* filterDuplicate (myresult); */
            filterDuplicateShort (myresult);
	}
    if (status < 0) {
        if (status != MCAT_INQUIRE_ERROR)       /* no answer */
            fprintf (stderr, "Error listing collection: %s, status = %d\n",
          parColl, status);
    }
    return status;
}

int
expandAndChkName (srbConn *conn, int catType, srbPathName *nameArray, 
int flagval, mdasC_sql_result_struct *dataResult, 
mdasC_sql_result_struct *collResult)
{
    int status;
    int dataFlag;
    int collFlag;
    char tmpNameStore[MAX_TOKEN * 2];
    /* Check whether it is a data set */
    strcpy(tmpNameStore,nameArray->outArgv);
    if (nameArray->type == COLLECTION_T || nameArray->type == SPEC_COLL_T) {
	dataFlag = 0;
	dataResult->row_count = 0;
    } else { 
	status = queryData (conn, catType, nameArray->outArgv, flagval, 
	  MAX_GET_RESULTS, dataResult);
	if (status < 0 || dataResult->result_count == 0) {
	    dataResult->row_count = 0;  
	    dataFlag = 0;
	} else
	    dataFlag = dataResult->row_count;
    }

    if (strlen(tmpNameStore) > 0)
      status = queryCollColl (conn, catType, tmpNameStore, flagval,
          collResult);
    else {
      status = -1;
      collResult->result_count = 0;
    }
    if (status < 0 || collResult->result_count == 0) { 
	collResult->row_count = 0;  
        collFlag = 0;
    } else
        collFlag = 1;

    if (collFlag == 1 && dataFlag >= 1)
	return DATA_COLL_T;
    else if (collFlag == 1)
	return COLLECTION_T;
    else if (dataFlag >= 1) {
        /* XXXXX a temporary implementation to make sure overwring just
         * one file if in parallel mode */
        if ((flagval & m_FLAG) && (flagval & F_FLAG))
	    return (DATANAME_T + dataFlag - 1);
	else 
	    return (DATANAME_T);
    } else
	return -1;
}
    
int 
chkFileName (srbPathName *fileName)
{
    struct stat statbuf;
    int status;
    int len;
    char *cptr1;

    /*
      fileName->outArgv = malloc (MAX_TOKEN);
    */
      
    fileName->outArgv = malloc (MAX_OBJ_NAME_LENGTH);

    if (fileName->outArgv == NULL) {
        fprintf(stderr, "Client malloc error.\n");
        return CLI_ERR_MALLOC;
    }

    strcpy (fileName->outArgv, fileName->inpArgv);

    /* Take care of any "./" and "/" in between */

    len = strlen (fileName->outArgv);
    cptr1 = fileName->outArgv + len - 2;
    while (cptr1 >= fileName->outArgv) {
        if (*cptr1 == '/' && *(cptr1 + 1) == '/') {     /* A "//" pattern */
            strcpy (cptr1, (cptr1 + 1));
        } else if (*cptr1 == '.' && *(cptr1 + 1) == '/') {
            if (*(cptr1 - 1) == '/') {  /* we have a "/./" pattern */
                strcpy (cptr1, (cptr1 + 2));    /* just eliminate "./" */
            }
        }
        cptr1 --;
    }

    /* Now handle the last 3 char to take care of "/","/.", "/..", etc */

    len = strlen (fileName->outArgv);
    cptr1 = fileName->outArgv + len - 1;

    if (len >= 2 && *cptr1 == '/') {
        *cptr1 = '\0';
    } else if (len >= 2 && *cptr1 == '.' && *(cptr1 - 1) == '/') {
        fileName->type = SPEC_COLL_T;
	return (0);
    } else if (len >= 3 && *cptr1 == '.' && *(cptr1 - 1) == '.' &&
      *(cptr1 - 2) == '/') {
        if (len == 3)
            return CLI_ERR_COLLNAME;

        fileName->type = SPEC_COLL_T;
	return (0);
    /* add check for . and .. 4/22/03 MW */
    } else if (strcmp (fileName->outArgv, ".") == 0) {
        fileName->type = SPEC_COLL_T;
	return (0);
    } else if (strcmp (fileName->outArgv, "..") == 0) {
        fileName->type = SPEC_COLL_T;
	return (0);
    /* end of 4/22/03 MW */
    } else {
        fileName->type = DATA_COLL_T;
    }
     
#ifdef _WIN32
	status = stat_file (fileName->inpArgv, &statbuf);
#else
    status = stat (fileName->inpArgv, &statbuf);
#endif 

    if (status != 0) {
	fileName->type = -1;
	return status;
    }

    if ((statbuf.st_mode & S_IFREG) != 0) {	/* A file */
	fileName->type = DATANAME_T;
	fileName->size = statbuf.st_size;
    } else if ((statbuf.st_mode & S_IFDIR) != 0) {	/* A directory */
	fileName->type = COLLECTION_T;
    }
    return (0);
}
    

void
filterDeleted (mdasC_sql_result_struct *myresult)
{
    int i, inInx, outInx;
    char *dataName, *collName, *inStr, *outStr;
    int real_row_count;

    if (myresult->result_count == 0)
        return;

    dataName = (char *) getFromResultStruct (myresult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    collName = (char *) getFromResultStruct (myresult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    if (dataName == NULL && collName == NULL)
	return;

    outInx = 0;
    real_row_count = myresult->row_count;
    for (inInx = 0; inInx < myresult->row_count; inInx ++) { 
	if ((dataName != NULL && 
	strstr(&dataName[inInx*MAX_DATA_SIZE], "$deleted") != NULL) ||
	(collName != NULL &&
	strstr(&collName[inInx*MAX_DATA_SIZE], "$deleted") != NULL)) {
	    real_row_count--;
	} else {
	    if (inInx != outInx) {	/* move it */
	        for (i = 0; i < myresult->result_count; i++){
		    inStr = outStr = (char *) myresult->sqlresult[i].values;
		    inStr += inInx*MAX_DATA_SIZE;
		    outStr += outInx*MAX_DATA_SIZE;
		    strcpy (outStr, inStr);
		}
	    }
	    outInx ++;
	}
    }
    myresult->row_count = real_row_count;
}

void
filterDuplicate (mdasC_sql_result_struct *myresult)
{
    int i, inInx, outInx;
    char *dataName, *collection, *replNum, *dirty, *inStr, *outStr;
    int real_row_count;
    int isDirty;

    if (myresult->result_count == 0)
        return;

    collection = (char *) getFromResultStruct (myresult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    dirty = (char *) getFromResultStruct (myresult,
      dcs_tname[IS_DIRTY], dcs_aname[IS_DIRTY]);

    dataName = (char *) getFromResultStruct (myresult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    replNum = (char *) getFromResultStruct (myresult,
      dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);

    if (dataName == NULL || replNum == NULL)
	return;

    outInx = 0;
    real_row_count = myresult->row_count;

    if (atoi (dirty) > 0)
	isDirty = 1;	/* the base is dirty */
    else
	isDirty = 0;
    real_row_count = myresult->row_count;
    for (inInx = 1; inInx < myresult->row_count; inInx ++) { 
	if (strcmp (&dataName[inInx*MAX_DATA_SIZE], 
	 &dataName[(inInx-1) * MAX_DATA_SIZE]) == 0 &&
	 strcmp (&collection[inInx*MAX_DATA_SIZE],
         &collection[(inInx-1) * MAX_DATA_SIZE]) == 0) {
	    if (atoi (&dirty[inInx * MAX_DATA_SIZE]) > 0 && isDirty != 1) {
		isDirty = 1;
		/* Overwite the non dirty */
                for (i = 0; i < myresult->result_count; i++){
                    inStr = outStr = (char *) myresult->sqlresult[i].values;
                    inStr += inInx*MAX_DATA_SIZE;
                    outStr += outInx*MAX_DATA_SIZE;
                    strcpy (outStr, inStr);
                }
	    }
	    real_row_count--;
	} else {
	    outInx ++;
	    /* if (atoi (dirty) > 0) */
            if (atoi (&dirty[inInx * MAX_DATA_SIZE]) > 0)
                isDirty = 1;    /* the base is dirty */
            else 
                isDirty = 0;

	    if (inInx != outInx) {	/* move it */
	        for (i = 0; i < myresult->result_count; i++){
		    inStr = outStr = (char *) myresult->sqlresult[i].values;
		    inStr += inInx*MAX_DATA_SIZE;
		    outStr += outInx*MAX_DATA_SIZE;
		    strcpy (outStr, inStr);
		}
	    }
	}
    }
    myresult->row_count = real_row_count;
}

int
appendResult (mdasC_sql_result_struct *toresult, 
mdasC_sql_result_struct *fromresult, int cursize)
{
    char *tovalue, *fromvalue;
    int newsize, minNewSize;
    int i;

    if (fromresult == NULL || fromresult->result_count <= 0 ||
      fromresult->row_count <= 0)
        return 0;

    minNewSize = toresult->row_count + fromresult->row_count;
    if (cursize < minNewSize) {
	if (fromresult->row_count >= MAX_GET_RESULTS)
	    newsize = minNewSize + 2 * fromresult->row_count;
	else
	    newsize = minNewSize;
    } else {
	newsize = 0;
    }
   
    for (i = 0; i < fromresult->result_count; i ++) {
	if (newsize > 0) {
	    tovalue = malloc (newsize * MAX_DATA_SIZE);
	    memcpy (tovalue, toresult->sqlresult[i].values, 
	     toresult->row_count * MAX_DATA_SIZE);
	    free (toresult->sqlresult[i].values);
	    toresult->sqlresult[i].values = tovalue;
	} else {
	    tovalue = toresult->sqlresult[i].values;
	}
	tovalue += toresult->row_count * MAX_DATA_SIZE;
	fromvalue = fromresult->sqlresult[i].values;
	memcpy (tovalue, fromvalue, fromresult->row_count * MAX_DATA_SIZE);
    }
    toresult->row_count = minNewSize;
    if (newsize == 0)
	newsize = cursize;

    return (newsize);
}


void
filterDuplicateShort (mdasC_sql_result_struct *myresult)
{
    int i, inInx, outInx;
    char *dataName, *dirty, *inStr, *outStr;
    char *collName;
    char *dirtySz, *myDirtySz;
    int real_row_count;

    if (myresult->result_count == 0)
        return;
    dataName = (char *) getFromResultStruct (myresult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    dirty = (char *) getFromResultStruct (myresult,
      dcs_tname[IS_DIRTY], dcs_aname[IS_DIRTY]);

    collName = (char *) getFromResultStruct (myresult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    if (dataName == NULL || dirty == NULL || collName == NULL)
	return;

    outInx = 1;
    real_row_count = myresult->row_count;
    if (atoi (dirty) > 0)
	myDirtySz = (char *) 1;	/* already dirty */
    else
	myDirtySz = NULL;

    for (inInx = 1; inInx < myresult->row_count; inInx ++) { 
	if (strcmp (&dataName[inInx*MAX_DATA_SIZE], 
	 &dataName[(inInx-1) * MAX_DATA_SIZE]) == 0 && 
	   strcmp (&collName[inInx*MAX_DATA_SIZE],
	 &collName[(inInx-1) * MAX_DATA_SIZE]) == 0	) {
	    if (myDirtySz != NULL && atoi (&dirty[inInx]) > 0) 
		myDirtySz = &dirtySz[inInx];
	    real_row_count--;
	} else {
	    if (inInx != outInx) {	/* move it */
	        for (i = 0; i < myresult->result_count; i++){
		    inStr = outStr = (char *) myresult->sqlresult[i].values;
		    inStr += inInx*MAX_DATA_SIZE;
		    outStr += outInx*MAX_DATA_SIZE;
		    strcpy (outStr, inStr);
		}
                if (myDirtySz > (char *) 1) {
                    strcpy (&dirtySz[outInx - 1], myDirtySz);
                }
    		if (atoi (&dirty[outInx]) > 0)
	            myDirtySz = (char *) 1;
		else
	            myDirtySz = NULL;
	    }
	    outInx ++;
	}
    }
    myresult->row_count = real_row_count;
}

/* rmColl - rm resursively collections given in myresult.
 */

int
rmColl (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *collResult, int copyNum, int inpPurgeTime)
{
    int i, status = 0;
    char inbuf[MAX_TOKEN];
    char *srcPar, *srcCh;
    mdasC_sql_result_struct myresult;
    int trashExpStat;
    int inTrash;

    if (collResult->result_count == 0)
        return 0;

    srcPar = (char *) getFromResultStruct(collResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    for (i = 0; i < collResult->row_count; i++, srcPar += MAX_DATA_SIZE) {
        if ((flagval & P_FLAG) != 0) {
            fprintf(stdout,"rm collection  %s (y/n)?", srcPar);
            fgets(inbuf,100,stdin);
            if (inbuf[0] != 'y' )
                continue;
        }

        if (flagval & t_FLAG) {     /* trash removal */
            trashExpStat = trashExpired (inpPurgeTime, NULL, srcPar);
            if (trashExpStat == TRASH_NOT_EXPIRED)
                continue;
        }

	/* use checkTrashColl : inTrash = isInTrash (srcPar); */
        inTrash = checkTrashColl (srcPar);
	if ((flagval & f_FLAG) == 0 && inTrash == NOT_IN_TRASH) {
	    /* put in the trash instead */

            status = srbModifyCollect (conn, MDAS_CATALOG, 
             srcPar, "", "", "", C_MOVE_COLL_TO_TRASH);
            if (status < 0 && status != MCAT_INQUIRE_ERROR) {
                if (status == NO_ACCS_TO_USER_IN_COLLECTION) {
                    fprintf(stderr,
                     "rmColl:error because of no access to trash collection\n");
		    return status;
		}
                RStatus = status;
                return status;
            }
	    continue;
        }

        status = queryDataInColl (conn, catType, srcPar, flagval,
          MAX_GET_RESULTS, &myresult);
        if (status < 0 && status != MCAT_INQUIRE_ERROR) {
            RStatus = status;
            return status;
        }

        /* rm all dataset in the collection */

        do {
	    int purgeTime;

	    if ((flagval & t_FLAG) && trashExpStat == TRASH_EXPIRED) { 
		purgeTime = 0;
	    } else {
		purgeTime = inpPurgeTime;
	    }
	    
            status = rmData (conn, catType, flagval, &myresult, copyNum, 
	     purgeTime);

            if (myresult.continuation_index >= 0 && myresult.result_count > 0)
{
                clearSqlResult (&myresult);
                status = srbGetMoreRows(conn, catType,
                 myresult.continuation_index, &myresult, MAX_GET_RESULTS);
            } else
                clearSqlResult (&myresult);
        } while (myresult.result_count > 0 && status == 0);

        /* now remove the collection recursively */

        status = querySubCollInColl (conn, catType, srcPar, 0, 
	  MAX_GET_RESULTS, &myresult);

        if (status < 0 && status != MCAT_INQUIRE_ERROR) {
	    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            RStatus = status;
            return status;
	}

        do {
            if (myresult.result_count == 0 || status == MCAT_INQUIRE_ERROR)
                break;

            status = rmColl (conn, catType, flagval, &myresult, copyNum,
	     inpPurgeTime);

            if (myresult.continuation_index >= 0 && myresult.result_count > 0)
{
                clearSqlResult (&myresult);
                status = srbGetMoreRows(conn, catType,
                 myresult.continuation_index, &myresult, MAX_GET_RESULTS);
            } else
                clearSqlResult (&myresult);
        } while (myresult.result_count > 0 && status == 0);

        /* Now the collection is empty. Rm it */

	if (inTrash != IS_TRASH_ROOT || (flagval & F_FLAG)) {
	    /* don't remove trash root */
	    if (flagval & A_FLAG)
                status = srbModifyCollect(conn, catType, srcPar,
                 "", "", "", C_SU_DELETE_TRASH_COLL);
	    else
                status = srbModifyCollect(conn, catType, srcPar,
                 "", "", "", D_DELETE_COLL);

            if (status < 0) {
                if (RStatus == 0)
                    RStatus = status;
	    }
	}
    }
    return status;
}

int
rmData (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, int copyNum, int inpPurgeTime)
{
    int i, status;
    char inbuf[MAX_TOKEN], temp[MAX_TOKEN];
    char *srcPar, *srcCh;

    if (dataResult->result_count == 0)
        return 0;
    srcPar = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    srcCh = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    for (i = 0; i < dataResult->row_count;i++, srcPar += MAX_DATA_SIZE,
      srcCh += MAX_DATA_SIZE) {

        if ((flagval & P_FLAG) != 0) {
            fprintf(stdout,"rm dataset  %s/%s (y/n)?", srcPar , srcCh);
            fgets(inbuf,100,stdin);
            if (inbuf[0] != 'y' )
                continue;
        }
	if ((flagval & unreg_FLAG) != 0) {
	    if (copyNum < 0) {
                if (flagval & A_FLAG) {
                    sprintf(temp, "%s&ADMIN=1",srcCh);
	    	    status = srbUnregisterDataset (conn, temp, srcPar);
                } else {
	    	    status = srbUnregisterDataset (conn, srcCh, srcPar);
                }
	    } else {
		sprintf(temp, "%s&COPY=%i",srcCh, copyNum);	
	    	status = srbUnregisterDataset (conn, temp, srcPar);
	    }
	} else {
            if (copyNum < 0) {
		if ((flagval & f_FLAG) || 
		 checkTrashColl (srcPar) != NOT_IN_TRASH) {
    		    if (flagval & t_FLAG) {     /* trash removal */
        		if (trashExpired (inpPurgeTime, srcCh, srcPar) != 
			 TRASH_EXPIRED)
			    continue;
		    }
		    if (flagval & A_FLAG) {
			sprintf(temp, "%s&ADMIN=1",srcCh);
			status = srbObjUnlink(conn, temp, srcPar);
		    } else {
            	        status = srbObjUnlink(conn, srcCh, srcPar);
		    }
		} else {
		    status = srbModifyDataset (conn, MDAS_CATALOG, srcCh,
		     srcPar, "", "", "", "", D_MOVE_DATA_TO_TRASH);
		}
            } else {
                sprintf(temp, "%s&COPY=%i",srcCh, copyNum);
                status = srbObjUnlink (conn, temp, srcPar);
            }
	}

        if (status == ACTION_NOT_ALLOWED_ON_CONTAINER_OBJECT) {   /* error */
            continue;
        } else if (status == CONTAINER_NOT_EMPTY) {
            RStatus = status;
            RemovingCont = 1;
            continue;
        } else if (status < 0) {
	    if (status == NO_ACCS_TO_USER_IN_COLLECTION) {
		fprintf(stderr,
		 "rmData:error because of no access to trash can collection\n");
		return status;
	    }
	    RStatus = status;
            fprintf(stderr,
             "rmData: error in removing %s/%s. status =  %d\n",
              srcPar, srcCh, status);
              srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        }
    }
    return (0);
}

int
fileToDataCopy (srbConn *conn, int catType, int flagval, int targType,
srbPathName *fileName, char *targColl, char *targObj,
char *datatype, char *newpathname, char *newResource, char *containerName,
int copyNum)
{
    int i;
    srb_long_t datasize;
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN], tmpTargObj[MAX_TOKEN];
    char inbuf[MAX_TOKEN];
    char tmpDataType[MAX_TOKEN];
    int fileExist;
    srb_long_t status;
    unsigned long ichksum;

    datasize = fileName->size;

    if ((flagval & P_FLAG) != 0) {
        fprintf(stdout,"Copy local file %s to %s/%s (y/n)?",
          fileName->inpArgv, targColl, targObj);
        fgets(inbuf,100,stdin);
        if (inbuf[0] != 'y' ) {
            return (0);
	}
    }

    if (targType < DATANAME_T) {       /* create a new dataset */
        setObjCondForNew (targObj, tmpTargObj, flagval, containerName,
         fileName->inpArgv);
#ifdef foo      /* done in targObj */
    	if (flagval & c_FLAG) {         /* user container */
            sprintf (tmpTargObj,
             "%s&CONTAINER=%-s", targObj, containerName);
    	} else if (flagval & a_FLAG) {         /* COPIES=all */
            sprintf (tmpTargObj, "%s&COPIES=all", targObj);
    	} else {
            strcpy (tmpTargObj, targObj);
    	}
#endif
    	if (strlen(datatype) == 0)
            trylocaldatatype(tmpDataType, fileName->inpArgv);
     	else
            strcpy(tmpDataType, datatype);
	fileExist = 0;
    } else {
	fileExist = targType - DATANAME_T + 1;
	strcpy (tmpDataType, datatype);
	/* strcpy (tmpTargObj, targObj); */
        setObjCondForOld (targObj, tmpTargObj, flagval, fileName->inpArgv);
    }
    if (flagval & k_FLAG) {	/* register chksum */
	if (clFileSum (fileName->inpArgv, &ichksum) < 0) {
	    fprintf (stderr,
	     "fileToDataCopy: clFileSum failed for %s\n", fileName->inpArgv);
	    return -1;
	} else if (ichksum != 0) {
	    sprintf (inbuf, "&REG_CHKSUM=%d", ichksum);
	    strcat (tmpTargObj, inbuf);
	}
    } else if (flagval & K_FLAG) {     /* register chksum */
        if (clFileSum (fileName->inpArgv, &ichksum) < 0) {
            fprintf (stderr,
             "fileToDataCopy: clFileSum failed for %s\n", fileName->inpArgv);
            return -1;
        } else if (ichksum != 0) {
            sprintf (inbuf, "&VERIFY_CHKSUM=%d", ichksum);
            strcat (tmpTargObj, inbuf);
        }
    }

    status = localToSrbFileCopy (conn, catType, fileName->inpArgv,
         tmpTargObj, targColl, datasize, flagval, fileExist, tmpDataType,
          newpathname, newResource, copyNum);

    return (status);
}

int 
localToSrbFileCopy (srbConn *conn, int catType, char *localPath,
char *targObj, char *targColl, srb_long_t datasize, int flagval, 
int fileExist, char *dataType, char *newpathname, char *newResource, 
int copyNum)
{
    int status;
    srb_long_t lstatus;
    

    if (fileExist >= 1) {  /* overwrite an existing object */
        if ((flagval & F_FLAG) == 0 && (flagval & a_FLAG) == 0 &&
         copyNum < 0) {
            fprintf (stderr,
             "Target object %s/%s already exist. Use -f or -a to overwrite.\n",
              targColl, targObj);
            RStatus = UTIL_ERROR_TARGET_ALREADY_EXIST;
            return (RStatus);
        }
    }

#ifdef PARA_OPR
    if ((flagval & s_FLAG) || 
     ((flagval & a_FLAG) && fileExist > 0) ||
     /* XXXX (strlen (newpathname) > 0) || */
     ((flagval & m_FLAG) == 0 && getenv ("srbParallel") == NULL)) {
        /* || fileExist > 1 || copyNum >= 0) { */
        lstatus = localToSrbFileCopyS (conn, catType, localPath, 
	 targObj, targColl, datasize, flagval, fileExist, dataType,
	  newpathname, newResource, copyNum);
    } else {
        lstatus = localToSrbFileCopyP (conn, catType, localPath,
         targObj, targColl, datasize, flagval, fileExist, dataType,
          newpathname, newResource);
    }
#else
    lstatus = localToSrbFileCopyS (conn, catType, localPath,
     targObj, targColl, datasize, flagval, fileExist, dataType,
      newpathname, newResource, copyNum);
#endif	/* PARA_OPR */

    if (lstatus >= 0) {
        return (0);
    } else {
	status = lstatus;
        if (status != COLLECTION_NOT_IN_CAT && 
	  status != DATA_NOT_IN_COLLECTION) {    /* rsync depends on this
						  * error to make new coll */
            fprintf (stderr, "localToSrbFileCopy error for %s/%s\n",
              targColl, targObj);
            srb_perror (2, (int) status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	}
        return ((int) status);
    }
}

srb_long_t
localToSrbFileCopyS (srbConn *conn, int catType, char *localPath, 
char *srbDataName, char *srbCollection, srb_long_t datasize, int flagval, 
int destExist, char *datatype, char *newpathname, char *newResource, 
int copyNum)
{ 
  srb_long_t slReturnVal;
  
  if (flagval & retry_FLAG) {
    int iSleep=2, iRetryCount=0; /* keep retrying */
    
    do {
      iRetryCount++;
      
      slReturnVal = localToSrbFileCopyS2 (conn, catType, localPath,
        srbDataName, srbCollection, datasize, flagval,
	 destExist, datatype, newpathname, newResource, copyNum);
      
      if (slReturnVal < 0 && iRetryCount <= iGlbRetriesMax) {
        fprintf(stderr,
          "localToSrbFileCopyS() error (file='%s/%s'), sleeping %d seconds before next retry %d of %d, slReturnVal=%ld\n",
          srbCollection, srbDataName, iSleep, iRetryCount, iGlbRetriesMax, 
	  slReturnVal);
#ifdef _WIN32
        Sleep(iSleep*1000);
#else
        sleep(iSleep);
#endif
        iSleep *= 2;
        if (iSleep > RETRY_SLEEP_MAX) iSleep=RETRY_SLEEP_MAX;
      }
      
      if (slReturnVal < 0 && iRetryCount > iGlbRetriesMax) {
        fprintf(stderr,
	 "localToSrbFileCopyS2() error (file='%s/%s'), too many error (retries=%d), giving up, slReturnVal = %d\n",
          srbCollection, srbDataName, (iRetryCount-1), iSleep, slReturnVal);
        break;
      }
    }
    while (slReturnVal < 0);
  }
  else {
    slReturnVal = localToSrbFileCopyS2 (conn, catType, localPath,
	srbDataName, srbCollection, datasize, flagval,
	destExist, datatype, newpathname, newResource, copyNum);
  }
  return(slReturnVal);
}

srb_long_t
localToSrbFileCopyS2 (srbConn *conn, int catType, char *localPath, 
char *srbDataName, char *srbCollection, srb_long_t datasize, int flagval, 
int destExist, char *datatype, char *newpathname, char *newResource,
int copyNum)
{
    int i, tmp, iBuffCounter=0;
    int in_fd, out_fd;
    srb_long_t status, bytesCopied;
    char *buf;
    float fSec;
#ifdef _WIN32
    SYSTEMTIME startTime, endTime;
#else
    struct timeval diffTime, startTime, endTime, sTimeDiff;
#endif
    unsigned long ulCheckSum = 0;
    ulong ulTotalSize = 1;
    int closeStat;

    buf = malloc (BUFSIZE + 1);
    if (buf == NULL) {
        fprintf (stderr, "localToSrbFileCopyS: Unable to malloc buf\n");
        return (-1);
    }

    if (flagval & v_FLAG)
#ifdef _WIN32
        GetSystemTime(&startTime);
#else
        (void) gettimeofday(&startTime, (struct timezone *)0);
#endif

#ifdef _WIN32
    in_fd = SrbNtFileBinaryOpen(localPath, O_RDONLY);
#else
    in_fd = open (localPath, O_RDONLY, 0);
#endif
    if (in_fd < 0) {
        fprintf(stderr,
         "Unable to open local file '%s', errno = %d.\n",
              localPath, errno);
        free (buf);
        return(in_fd);
    }

    if (flagval & V_FLAG) {
      struct stat statbuf;
      int status;
      /* get local file size for nice progress message */
#ifdef _WIN32
      status = stat_file (localPath, &statbuf);
#else
      status = stat (localPath, &statbuf);
#endif
      if ((statbuf.st_mode & S_IFREG) != 0)
	{                           /* A file */
	  ulTotalSize = (long) statbuf.st_size;
	}
    }
    
    if (destExist == 0) {       /* create a new dataset */
        out_fd = srbObjCreate (conn, catType, srbDataName,
          datatype, newResource, srbCollection, newpathname, datasize);
        if (out_fd < 0) {
	    close (in_fd);
            free (buf);
	    if ((out_fd == COLLECTION_NOT_IN_CAT ||
	     out_fd == DATA_NOT_IN_COLLECTION) && (flagval & Y_FLAG))
		return (out_fd);
            fprintf (stderr,
             "Unable to create object %s/%s, status = %d\n",
              srbCollection, srbDataName, out_fd);
            srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return (out_fd);
        }
    } else if (destExist >= 1) {  /* overwrite an existing object */
#ifdef foo      /* check moved to localToSrbFileCopy */
        if ((flagval & F_FLAG) == 0 && (flagval & a_FLAG) == 0 &&
	 copyNum < 0) {
            fprintf (stderr,
             "Target object %s/%s already exist. Use -f or -a to overwrite.\n",
              srbCollection, srbDataName);
            close (in_fd);
            free (buf);
	    RStatus = UTIL_ERROR_TARGET_ALREADY_EXIST;
            return (0);
        } else {
#endif
            if ((flagval & a_FLAG) != 0) {      /* overwrite all replica */
                out_fd = srbObjOpen (conn, srbDataName, O_WR_REPL|O_TRUNC,
                      srbCollection);
            } else {
                if (copyNum >= 0) {
                    char tmpDataName[MAX_TOKEN];
                    sprintf(tmpDataName, "%s&COPY=%i",srbDataName, copyNum);
                    out_fd = srbObjOpen (conn, tmpDataName, O_RDWR|O_TRUNC,
                     srbCollection);
		} else {
                    out_fd = srbObjOpen (conn, srbDataName, O_RDWR|O_TRUNC,
                     srbCollection);
		}
                if (out_fd < 0) {
                    fprintf (stderr,
                     "Unable to open object %s/%s, status = %d\n",
                      srbCollection, srbDataName, out_fd);
                    srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                    close (in_fd);
            	    free (buf);
                    return (out_fd);
                }
	    }
#ifdef foo
        }
#endif
    }

    bytesCopied = 0;

    while ((i = read(in_fd, buf, BUFSIZE)) > 0) {
#ifdef _WIN32
      SYSTEMTIME sTimeLast;
#else
      struct timeval sTimeLast;
#endif
      int iLastSec = 0;

        tmp = srbObjWrite(conn, out_fd, buf, i);
	iBuffCounter++;
        if (tmp < i) {
            fprintf(stderr, "Error: Read %d bytes, Wrote %d bytes.\n ",
              i, tmp);
            srb_perror (2, tmp, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            free (buf);
	    close (in_fd);
            return (-1);
        }
        bytesCopied += tmp;

	if ((flagval & V_FLAG)) {
	  float fBs;
	  if (iBuffCounter < 2) {
	    
#ifdef _WIN32
	    GetSystemTime(&sTimeLast);
	    fSec = abs((float)startTime.wSecond - 
	     (float)sTimeLast.wSecond) +
	      abs((float)startTime.wMilliseconds - 
	       (float)sTimeLast.wMilliseconds/1000.0);
	    fBs = (float) (bytesCopied / nz (fSec));
	    
	    if(abs(sTimeLast.wSecond - startTime.wSecond) >= 0) {
		vNiceVerboseProgressMessage (flagval, INTO_SRB, srbDataName, 
		 localPath, (ulong) bytesCopied,
		 ulTotalSize, fSec, fBs);
	    }
	    iLastSec = sTimeLast.wSecond;
#else
	    (void) gettimeofday (&sTimeLast, (struct timezone *) 0);
	    (void) tvsub (&sTimeDiff, &sTimeLast, &startTime);
	    fSec = (float) sTimeDiff.tv_sec + ((float) sTimeDiff.tv_usec / 
	     1000000.0);
	    fBs = (float) (bytesCopied / nz (fSec));
	    (void) tvsub (&sTimeDiff, &sTimeLast, &startTime);
	    
	    if (sTimeDiff.tv_usec >= 1) {
	      vNiceVerboseProgressMessage (flagval, INTO_SRB, srbDataName, 
		localPath, (ulong) bytesCopied,
		ulTotalSize, fSec, fBs);
	    }
	    iLastSec = sTimeLast.tv_sec;
#endif
	  }
	  else {
            if (iBuffCounter > 4) iBuffCounter=0;
	  }
	}
    }

    closeStat = srbObjClose (conn, out_fd);

    free (buf);
    close (in_fd);

    if ((flagval & v_FLAG) && bytesCopied > 0) {
#ifdef _WIN32
      GetSystemTime(&endTime);
      fSec = abs((float)startTime.wSecond - (float)endTime.wSecond) +
	abs((float)startTime.wMilliseconds - 
	 (float)endTime.wMilliseconds/1000.0);
#else
      (void) gettimeofday(&endTime, (struct timezone *)0);
      (void)tvsub(&diffTime, &endTime, &startTime);
      fSec = (float)diffTime.tv_sec +
	((float)diffTime.tv_usec / 1000000.0);
#endif
    }
    
    if (datasize > 0 && datasize != bytesCopied) {
      if (bytesCopied >= 0)
	fprintf(stderr,
	  "Error: Wrote %lld bytes, but input file size was %lld bytes.\n",
	  bytesCopied, datasize);
      srb_perror (2, tmp, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    } else {
      if ((flagval & v_FLAG) && bytesCopied > 0) {
	vNiceVerboseFinalMessage (flagval, INTO_SRB, srbDataName, localPath,
				  bytesCopied, fSec, -1, ulCheckSum);
      }
    }

    if (closeStat < 0) {
        srb_perror (2, (int) closeStat, "",
                  SRB_RCMD_ACTION | SRB_LONG_MSG);
        bytesCopied = closeStat;
    }

    return (bytesCopied);
}
/* end of localToSrbFileCopyS2() */


#ifdef PARA_OPR
srb_long_t
localToSrbFileCopyP (srbConn *conn, int catType, char *localPath,
char *srbDataName, char *srbCollection, srb_long_t datasize, int flagval,
int destExist, char *datatype, char *newpathname, char *newResource)
{
  srb_long_t slReturnVal;
  
  if (flagval & retry_FLAG) {
    int iSleep=2, iRetryCount=0; /* keep retrying */
    
    do {
      iRetryCount++;
      
      slReturnVal = localToSrbFileCopyP2 (conn, catType,localPath,
       srbDataName, srbCollection, datasize, flagval,
	 destExist, datatype, newpathname, newResource);

      if (slReturnVal < 0 && iRetryCount <= iGlbRetriesMax) {
        fprintf(stderr,
            "localToSrbFileCopyP() error (file='%s/%s'), sleeping %d seconds before next retry %d of %d, slReturnVal=%ld\n",
            srbCollection, srbDataName, iSleep, iRetryCount, 
	    iGlbRetriesMax, slReturnVal);
#ifdef _WIN32
        Sleep(iSleep*1000);
#else
        sleep(iSleep);
#endif
        iSleep *= 2;
        if (iSleep > RETRY_SLEEP_MAX) iSleep=RETRY_SLEEP_MAX;
      }
      
      if (slReturnVal < 0 && iRetryCount > iGlbRetriesMax) {
        if ((slReturnVal == COLLECTION_NOT_IN_CAT ||
	 slReturnVal == DATA_NOT_IN_COLLECTION) && (flagval & Y_FLAG))
            return (slReturnVal);
        fprintf(stderr,
            "localToSrbFileCopyP22() error (file='%s/%s'), too many error (retries=%d), giving up, slReturnVal = %d\n",
	     srbCollection, srbDataName, (iRetryCount-1), iSleep, slReturnVal);
        break;
      }
    }
    while (slReturnVal < 0);
  }
  else {
    slReturnVal = localToSrbFileCopyP2 (conn, catType,localPath,
	srbDataName, srbCollection, datasize, flagval,
	destExist, datatype, newpathname, newResource);
  }
  return(slReturnVal);
}

srb_long_t localToSrbFileCopyP2 (srbConn *conn, int catType, char *localPath,
char *srbDataName, char *srbCollection, srb_long_t datasize, int flagval,
int destExist, char *datatype, char *newpathname, char *newResource)
{
    srb_long_t bytesCopied;
    float fSec;
#ifdef _WIN32
    SYSTEMTIME startTime, endTime;
#else
    struct timeval diffTime, startTime, endTime;
#endif
    unsigned long ulCheckSum = 0;
    
#ifdef foo
    if (destExist > 1) {
	fprintf (stderr, 
         "Overwriting SRB file with multi-replica not allowed in para mode\n");
        return (-1);
    } else if (destExist == 1) {
	/* srbObjUnlink (conn, srbDataName, srbCollection); */
    }
#endif
    
    if (flagval & v_FLAG)
#ifdef _WIN32
        GetSystemTime(&startTime);
#else
        (void) gettimeofday(&startTime, (struct timezone *)0);
#endif

    if (flagval & M_FLAG || (flagval & m_FLAG) == 0 ||
     strstr (srbDataName, "CONTAINER") != NULL) {
        bytesCopied = srbObjPutC (conn, srbDataName, srbCollection, 
	 newResource, datatype, newpathname, localPath, datasize, 
	 flagval, NumThreads);
	if (bytesCopied == SYS_ERR_NO_SUPPORT ||
	 bytesCopied == CLI_FUNC_NOT_SUPPORTED) {
            bytesCopied = srbObjPut (conn, srbDataName, srbCollection, 
	     newResource, datatype, newpathname, localPath, datasize, 
	     (flagval & F_FLAG));
	}
    } else {
        bytesCopied = srbObjPut (conn, srbDataName, srbCollection, newResource,
         datatype, newpathname, localPath, datasize, (flagval & F_FLAG));
    }

    if (datasize != bytesCopied) {
        if ((bytesCopied == COLLECTION_NOT_IN_CAT ||
	 bytesCopied == DATA_NOT_IN_COLLECTION) && (flagval & Y_FLAG))
            return (bytesCopied);

        if (bytesCopied >= 0)
            fprintf(stderr,
             "Wrote %lld bytes, but input file size was %lld bytes.\n",
               bytesCopied, datasize);
    }

    if ((flagval & v_FLAG) && bytesCopied > 0) {

#ifdef _WIN32
       GetSystemTime(&endTime);
       fSec = abs((float)startTime.wSecond - (float)endTime.wSecond) +
        abs((float)startTime.wMilliseconds - (float)endTime.wMilliseconds/1000.0);
#else
       (void) gettimeofday(&endTime, (struct timezone *)0);
       (void)tvsub(&diffTime, &endTime, &startTime);
       fSec = (float)diffTime.tv_sec +
        ((float)diffTime.tv_usec / 1000000.);
#endif

	vNiceVerboseFinalMessage (flagval, INTO_SRB, srbDataName, localPath,
	  bytesCopied, fSec, conn->numThread, ulCheckSum);
    }


    return (bytesCopied);
}
/* end of localToSrbFileCopyP2 (srbConn *conn, int catType, char *localPath, */

#endif /* PARA_OPR */

int
fileToCollCopy (srbConn *conn, int catType, int flagval,
srbPathName *fileName, char *targColl,
char *datatype, char *newpathname, char *newResource, char *containerName)
{
    int i;
    int in_fd, out_fd;
    srb_long_t datasize;
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN];
    char srcPar[MAX_TOKEN], srcCh[MAX_TOKEN], tmpTargObj[MAX_TOKEN];
    char inbuf[MAX_TOKEN];
    char tmpDataType[MAX_TOKEN];
    int fileExist;
    srb_long_t status;
    unsigned long ichksum;
    int targType;

    if ((flagval & P_FLAG) != 0) {
        fprintf(stdout,
         "Copy  %s to collection %s (y/n)?",
          fileName->inpArgv,  targColl);
        fgets(inbuf,100,stdin);
        if (inbuf[0] != 'y' ) {
            return 0;
	}
    }
    datasize = fileName->size;

    /* take out the P_FLAG */

    flagval = flagval & (~P_FLAG);

    /* Check whether the target dataset exists */

    clSplitbychar(fileName->outArgv, srcPar, srcCh,'/');
    status = checkDataset (conn, catType, targColl, srcCh);

    if (status == 0) {
	/* file exists */
	targType = DATANAME_T;
    } else {
	targType = 0;
    }

    status = fileToDataCopy (conn, catType, flagval, targType, fileName,
     targColl, srcCh, datatype, newpathname, newResource, containerName,
     -1);

#ifdef foo1	/* call fileToDataCopy */
    if (status != 0) {	/* file does not exist */
         setObjCondForNew (srcCh, tmpTargObj, flagval, containerName,
          fileName->inpArgv);
#ifdef foo      /* done in setObjCondForNew */
        if (flagval & c_FLAG) {         /* user container */
            sprintf (tmpTargObj,
                 "%s&CONTAINER=%-s", srcCh, containerName);
        } else if (flagval & a_FLAG) {         /* COPIES=all */
            sprintf (tmpTargObj, "%s&COPIES=all", srcCh);
        } else {
            strcpy (tmpTargObj, srcCh);
        }
#endif

        if (strlen(datatype) == 0)
            trylocaldatatype(tmpDataType, fileName->inpArgv);
        else
            strcpy(tmpDataType, datatype);
        fileExist = 0;
    } else {
        fileExist = 1;
        strcpy (tmpDataType, datatype);
        strcpy (tmpTargObj, srcCh);
    }

    if (flagval & k_FLAG) {     /* register chksum */
        if (clFileSum (fileName->inpArgv, &ichksum) < 0) {
            fprintf (stderr,
             "fileToCollCopy: clFileSum failed for %s\n", fileName->inpArgv);
            return -1;
        } else if (ichksum != 0) {
            sprintf (inbuf, "&REG_CHKSUM=%d", ichksum);
            strcat (tmpTargObj, inbuf);
        }
    } else if (flagval & K_FLAG) {     /* register chksum */
        if (clFileSum (fileName->inpArgv, &ichksum) < 0) {
            fprintf (stderr,
             "fileToCollCopy: clFileSum failed for %s\n", fileName->inpArgv);
            return -1;
        } else if (ichksum != 0) {
            sprintf (inbuf, "&VERIFY_CHKSUM=%d", ichksum);
            strcat (tmpTargObj, inbuf);
        }
    }

#ifdef PARA_OPR
    if ((flagval & s_FLAG) || 
     ((flagval & m_FLAG) == 0 && getenv ("srbParallel") == NULL &&
     (containerName == NULL || strlen (containerName) == 0))) {
/*
      || fileExist) {
*/
        status = localToSrbFileCopyS (conn, catType, fileName->inpArgv,
         tmpTargObj, targColl, datasize, flagval, fileExist, tmpDataType,
          newpathname, newResource, -1);
    } else {
        status = localToSrbFileCopyP (conn, catType, fileName->inpArgv,
         tmpTargObj, targColl, datasize, flagval, fileExist, tmpDataType,
          newpathname, newResource);
    }
#else
    status = localToSrbFileCopyS (conn, catType, fileName->inpArgv,
     tmpTargObj, targColl, datasize, flagval, fileExist, tmpDataType,
      newpathname, newResource, -1);
#endif /* PARA_OPR */
#endif	/* foo1 */

    if (status >= 0)
        return (0);
    else
        return ((int) status);
}

int
dirToCollCopyCont (srbConn *conn, int catType, int flagval,
char *srcDir, char *targColl, char *newResource, char *containerName, 
char *newPathName)
{
    mdasC_sql_result_struct  myresult;
    int i, j;
    int status;
    DIR *dirPtr;
    struct dirent *dp;
    struct stat statbuf;
    char tmpName[MAX_TOKEN];
    srbPathName mysPathName;
    char tmpNewPathName[HUGE_STRING];
    char tmpNewFileName[HUGE_STRING];
    char tmpsrcDir[MAX_TOKEN];

    strcpy(tmpNewPathName,"");
    strcpy(tmpNewFileName,"");
    if (newPathName != NULL && strlen(newPathName) > 0) {
      clSplitbychar(srcDir, tmpName, tmpsrcDir,'/');
      if (!strcmp(newPathName, "."))
	strcpy(tmpNewPathName,tmpsrcDir);
      else
	sprintf(tmpNewPathName, "%s/%s",newPathName,tmpsrcDir);
    }

    dirPtr = opendir (srcDir);
    if (dirPtr == NULL) {
        fprintf (stderr, "The local dir %s does not exist., errno = %d\n", 
	 srcDir, errno);
        return UNIX_ENOENT;
    }

    while ((dp = readdir (dirPtr)) != NULL) {
        if (strcmp (dp->d_name, ".") == 0 || strcmp (dp->d_name, "..") == 0)
            continue;
        sprintf (tmpName, "%s/%s", srcDir, dp->d_name);
#ifdef _WIN32
		status = stat_file(tmpName, &statbuf);
#else
        status = stat (tmpName, &statbuf);
#endif
        if (status != 0) {
            fprintf (stderr, "Unable to stat %s, errno = %d\n", tmpName, errno);
	    closedir (dirPtr);
            return status;
        }
        mysPathName.inpArgv = tmpName;
        mysPathName.outArgv = tmpName;
        if ((statbuf.st_mode & S_IFREG) != 0) {     /* A file */
            mysPathName.type = DATANAME_T;
            mysPathName.size = statbuf.st_size;
	    if (strlen(tmpNewPathName) > 0) 
	      sprintf (tmpNewFileName,"%s/%s", tmpNewPathName,dp->d_name);
            status = fileToCollCopy (conn, catType, flagval,
              &mysPathName, targColl, "", tmpNewFileName,newResource, containerName);
            if (status < 0) {
	    	closedir (dirPtr);
                return status;
	    }
        } else if ((statbuf.st_mode & S_IFDIR) != 0) {      /* A directory */
            mysPathName.type = COLLECTION_T;
            mysPathName.size = 0;

            status = dirToCollCopy (conn, catType, flagval,
              &mysPathName, targColl, newResource, containerName,
				    tmpNewPathName);
            if (status < 0) {
		closedir (dirPtr);
                return status;
	    }
        }
    }
    closedir (dirPtr);
    return (0);
}

int
dirToCollCopy (srbConn *conn, int catType, int flagval,
srbPathName *inPathName, char *origTarg, char *newResource, 
char *containerName, char *newPathName)
{
    int i, status;
    int in_fd, out_fd;
    srb_long_t datasize;
    char *datatype;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN], tmpTarg[MAX_TOKEN];
    char *origSrc, *curTarg;

    if (inPathName->type != SPEC_COLL_T) {      /* not ".". ".." type */
	int inheritCont;
        /* Make the collection */
        clSplitbychar(inPathName->outArgv, temp1, temp,'/');
	if (containerName != NULL && strlen (containerName) > 0) {
	    inheritCont = 0;
	} else {
	    inheritCont = 1;
	}
        status = myCreateCollect(conn, catType, origTarg, temp, inheritCont);
        if (status < 0 && status != DATA_SUBCOLLECTION_NOT_UNIQUE) {
            fprintf(stderr,
             "Unable to mk the target coll %s/%s, status = %d\n",
              origTarg, temp, status);
	    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return (CLI_ERR_COLLNAME);
        }
        sprintf (tmpTarg, "%s/%s", origTarg, temp);
        curTarg = tmpTarg;
    } else {
        curTarg = origTarg;
    }

    /* copy the content */
    status = dirToCollCopyCont (conn, catType, flagval,
      inPathName->outArgv, curTarg, newResource, containerName,
				newPathName);

    if (status < 0)
        return (status);

    return 0;
}

int
dataToDataCopy (srbConn *conn, int catType, int flagval, int targType,
mdasC_sql_result_struct *dataResult, int srcCopyNum, char *targColl, 
char *targObj, char *newResource, char *newPath, char *containerName)
{
    int i, sstatus;
    srb_long_t status;
    int in_fd, out_fd;
    srb_long_t datasize;
    char *datatype;
    char temp[MAX_OBJ_NAME_LENGTH];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN], tmpTargObj[MAX_TOKEN];
    char *srcPar, *srcCh, *tmpStr, *sizeStr;
    char inbuf[MAX_TOKEN];
    int existFlag;
    int doCopyP;
#ifdef _WIN32
    SYSTEMTIME startTime, endTime;
#else
    struct timeval diffTime, startTime, endTime;
#endif
    float fSec;

    if (dataResult->result_count == 0)
        return 0;

    srcPar = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    srcCh = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    sizeStr = (char *) getFromResultStruct(dataResult,
      dcs_tname[SIZE], dcs_aname[SIZE]);
    if (sizeStr == NULL)
        datasize = -1;
    else
        datasize = strtoll (sizeStr, 0, 0);

    datatype = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);

    for (i = 0; i < dataResult->row_count;i++) {

        if ((flagval & P_FLAG) != 0) {
	  if (srcCopyNum < 0) {
	      if (linkFlag)
                fprintf(stdout,"Link  %s/%s to %s/%s (y/n)?",
                  srcPar, srcCh, targColl, targObj);
	      else
                fprintf(stdout,"Copy  %s/%s to %s/%s (y/n)?",
                  srcPar, srcCh, targColl, targObj);
		
	  }
	  else {
	    if (linkFlag)
	      fprintf(stdout,"Link repl_num=%i of %s/%s to %s/%s (y/n)?",
		      srcCopyNum, srcPar, srcCh, targColl, targObj);
	    else
	      fprintf(stdout,"Copy repl_num=%i of %s/%s to %s/%s (y/n)?",
		      srcCopyNum, srcPar, srcCh, targColl, targObj);
	  }
            fgets(inbuf,100,stdin);
            if (inbuf[0] != 'y' ) {
        	srcPar += MAX_DATA_SIZE;
        	srcCh += MAX_DATA_SIZE;
        	sizeStr += MAX_DATA_SIZE;
        	datatype += MAX_DATA_SIZE;
		continue;
	    }
        }
        if (sizeStr == NULL)
            datasize = -1;
        else
            datasize = strtoll (sizeStr, 0, 0);

	sstatus = dataToDataCopy1 (conn, catType, flagval, targType,
	 srcCh, srcPar, datasize, srcCopyNum, targColl, targObj, newResource,
	 newPath, containerName);

	if (sstatus < 0) {
	    return (sstatus);
	}
        srcPar += MAX_DATA_SIZE;
        srcCh += MAX_DATA_SIZE;
        sizeStr += MAX_DATA_SIZE;
        datatype += MAX_DATA_SIZE;
    }
    return (0);
}
int
dataToDataCopy1 (srbConn *conn, int catType, int flagval, int targType,
char *srcCh, char *srcPar, srb_long_t datasize, int srcCopyNum, char *targColl,
char *targObj, char *newResource, char *newPath, char *containerName)
{
    char temp[MAX_OBJ_NAME_LENGTH];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN], tmpTargObj[MAX_TOKEN];
    char *tmpStr;
#ifdef _WIN32
    SYSTEMTIME startTime, endTime;
#else
    struct timeval diffTime, startTime, endTime;
#endif
    char *datatype;
    float fSec;
    int existFlag;
    int doCopyP;
    srb_long_t status;

    if (flagval & v_FLAG)
#ifdef _WIN32
        GetSystemTime(&startTime);
#else
        (void) gettimeofday(&startTime, (struct timezone *)0);
#endif
    if (strcmp (targColl, srcPar) == 0 && strcmp (targObj, srcCh) == 0) {
        if (linkFlag)
            printf ("dataToDataCopy error: trying to link to itself\n");
        else
            printf ("dataToDataCopy error: trying to copy to itself\n");
        return (CLI_ERR_COLLNAME);
    }

    if (srcCopyNum >=  0) {
        sprintf(temp, "%s&COPY=%i",srcCh, srcCopyNum);
    } else {
        strcpy (temp, srcCh);
    }
    if (strlen(inCondition) > 0) {
        strcat(temp,"&");
        strcat(temp,inCondition);
    }

    if (targType == -1) {
        existFlag = 0;
        doCopyP = 1;
    } else {
        existFlag = 1;
        doCopyP = 0;
    }

    if (existFlag == 0) {
        setObjCondForNew (targObj, tmpTargObj, flagval, containerName, NULL);
    } else {
        strcpy (tmpTargObj, targObj);
    }
#ifdef foo      /* done in setObjCondForNew */
    if (flagval & c_FLAG && existFlag == 0) {         /* user container */
        sprintf (tmpTargObj,
         "%s&CONTAINER=%-s", targObj, containerName);
        doCopyP = 0;
    } else if (flagval & a_FLAG && existFlag == 0) {   /* COPIES=all */
        sprintf (tmpTargObj, "%s&COPIES=all", targObj);
    } else {
        strcpy (tmpTargObj, targObj);
    }
#endif

    if (newPath != NULL && strlen (newPath) > 0)
        doCopyP = 0;

    if (linkFlag) {
      status = srbModifyDataset(conn, MDAS_CATALOG,
                                temp, srcPar,"","",
                                tmpTargObj,targColl, D_INSERT_LINK);
    } else {
        if (doCopyP == 0 || (flagval & s_FLAG)) {
            status = srbTosrbCopyS (conn, catType, temp, srcPar, tmpTargObj,
             targColl, newResource, datatype, newPath, datasize, existFlag,
              flagval);
        } else {
            status = srbObjCopy (conn, temp, srcPar, tmpTargObj, targColl,
             newResource);
        }
    }

    if (status < 0) {
        srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        return (status);
    }

    if (flagval & v_FLAG && datasize > 0) {
#ifdef _WIN32
         GetSystemTime(&endTime);
         fSec = abs((float)startTime.wSecond - (float)endTime.wSecond) +
          abs((float)startTime.wMilliseconds -
          (float)endTime.wMilliseconds/ 1000.0);
#else
         (void) gettimeofday(&endTime, (struct timezone *)0);
         (void)tvsub(&diffTime, &endTime, &startTime);
         fSec = (float)diffTime.tv_sec +
            ((float)diffTime.tv_usec / 1000000.);
#endif
          printf
           ("Fn:%s, sz=%5.4f MB, tm=%3.2f sec, rate=%3.2f MB/sec\n",
           srcCh, (float) datasize / 1000000., fSec,
            (float) datasize / 1000000. / fSec);
    }
    return (0);
}


int
dataToCollCopy (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, int srcCopyNum, char *targColl, 
char *newResource, char *newPath, char *containerName)
{
    int i;
    srb_long_t status;
    int in_fd, out_fd;
    srb_long_t datasize;
    char *datatype;
    char tmpSrcObj[MAX_OBJ_NAME_LENGTH];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN], tmpTargObj[MAX_TOKEN];
    char *srcPar, *srcCh, *tmpStr, *sizeStr;
    char *lastSrcPar = NULL;
    char *lastSrcCh = NULL;
    char inbuf[MAX_TOKEN];
    int existFlag;
    int doCopyP;
#ifdef _WIN32
    SYSTEMTIME startTime, endTime;
#else
    struct timeval diffTime, startTime, endTime;
#endif
    float fSec;

    if (dataResult->result_count == 0)
        return 0;
    srcPar = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    srcCh = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    sizeStr = (char *) getFromResultStruct(dataResult,
      dcs_tname[SIZE], dcs_aname[SIZE]);
    if (sizeStr == NULL)
        datasize = -1;
    else
        datasize = strtoll (sizeStr, 0, 0);

    datatype = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);

    for (i = 0; i < dataResult->row_count;i++) {

        if (lastSrcPar != NULL) {       /* are we copying replica ? */
            if (strcmp (lastSrcPar, srcPar) == 0 &&
              strcmp (lastSrcCh, srcCh) == 0) {
	        lastSrcPar = srcPar;
        	lastSrcCh = srcCh;

        	srcPar += MAX_DATA_SIZE;
        	srcCh += MAX_DATA_SIZE;
        	sizeStr += MAX_DATA_SIZE;
        	datatype += MAX_DATA_SIZE;
		continue;
	    }
        }

        if ((flagval & P_FLAG) != 0) {
	  if (srcCopyNum < 0) {
	    if (linkFlag)
	      fprintf(stdout,"Link  %s/%s to collection %s (y/n)?",
		      srcPar , srcCh,  targColl);
	    else
	      fprintf(stdout,"Copy  %s/%s to collection %s (y/n)?",
		      srcPar , srcCh,  targColl);
	  }
	      
	  else {
	    if (linkFlag)
	      fprintf(stdout,
		      "Link  repl_num=%i of %s/%s to collection %s (y/n)?",
		      srcCopyNum , srcPar , srcCh,  targColl);
	    else
	      fprintf(stdout,
		      "Copy  repl_num=%i of %s/%s to collection %s (y/n)?",
		      srcCopyNum , srcPar , srcCh,  targColl);
	  }
            fgets(inbuf,MAX_TOKEN,stdin);
            if (inbuf[0] != 'y' ) {
                lastSrcPar = srcPar;
                lastSrcCh = srcCh;

                srcPar += MAX_DATA_SIZE;
                srcCh += MAX_DATA_SIZE;
                sizeStr += MAX_DATA_SIZE;
                datatype += MAX_DATA_SIZE;
                continue;
            }
        }
        if (flagval & v_FLAG)
#ifdef _WIN32
            GetSystemTime(&startTime);
#else
            (void) gettimeofday(&startTime, (struct timezone *)0);
#endif
       if (strcmp (targColl, srcPar) == 0) {
            printf ("dataToDataCopy error: trying to copy to itself\n");
            return (CLI_ERR_COLLNAME);
        }

        if (sizeStr == NULL)
            datasize = -1;
        else
            datasize = strtoll (sizeStr, 0, 0);

        if (srcCopyNum >=  0) {
            sprintf(tmpSrcObj, "%s&COPY=%i",srcCh, srcCopyNum);
        } else {
            strcpy (tmpSrcObj, srcCh);
        }

	if (strlen(inCondition) > 0)
	  {
	    strcat(tmpSrcObj,"&");
	    strcat(tmpSrcObj,inCondition);
	  }
        /* Check whether the target dataset exists */

        status = checkDataset (conn, catType, targColl, srcCh);

        if (status == 0) {              /* it exists */
            existFlag = 1;
            doCopyP = 0;
        } else {
            existFlag = 0;
            doCopyP = 1;
	}

        if (existFlag > 0 && flagval & a_FLAG) {
            strcpy (tmpTargObj, srcCh);
        } else {
            setObjCondForNew (srcCh, tmpTargObj, flagval, containerName, NULL);
        }
#ifdef foo      /* done in setObjCondForNew */
    	if (flagval & c_FLAG) {         /* user container */
            sprintf (tmpTargObj,
             "%s&CONTAINER=%-s", srcCh, containerName);
        } else if (flagval & a_FLAG && existFlag == 0) {   /* COPIES=all */
            sprintf (tmpTargObj, "%s&COPIES=all", srcCh);
    	} else {
            strcpy (tmpTargObj, srcCh);
        }
#endif
         if (newPath != NULL && strlen (newPath) > 0)
            doCopyP = 0;

	if (linkFlag) {
	  status = srbModifyDataset(conn, MDAS_CATALOG, 
				    tmpSrcObj, srcPar,"","",
				    tmpTargObj,targColl, D_INSERT_LINK);
	}
        else {
	  if (doCopyP == 0) {
            status = srbTosrbCopyS (conn, catType, tmpSrcObj, srcPar, 
	     tmpTargObj, targColl, newResource, datatype, newPath, 
	      datasize, existFlag, flagval);
	  } else {
            status = srbObjCopy (conn, tmpSrcObj, srcPar, tmpTargObj, targColl,
             newResource);
	  }
	}

	if (status < 0) {
	  srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  return ((int) status);
	}
        if (flagval & v_FLAG && datasize > 0) {
#ifdef _WIN32
           GetSystemTime(&endTime);
           fSec = abs((float)startTime.wSecond - (float)endTime.wSecond) +
              abs((float)startTime.wMilliseconds - (float)endTime.wMilliseconds/1000.0);
#else
           (void) gettimeofday(&endTime, (struct timezone *)0);
           (void)tvsub(&diffTime, &endTime, &startTime);
           fSec = (float)diffTime.tv_sec +
            ((float)diffTime.tv_usec / 1000000.);
#endif
            printf
             ("Fn:%s, sz=%5.4f MB, tm=%3.2f sec, rate=%3.2f MB/sec\n",
             srcCh, (float) datasize / 1000000., fSec,
              (float) datasize / 1000000. / fSec);
        }

        lastSrcPar = srcPar;
        lastSrcCh = srcCh;

        srcPar += MAX_DATA_SIZE;
        srcCh += MAX_DATA_SIZE;
        sizeStr += MAX_DATA_SIZE;
        datatype += MAX_DATA_SIZE;
    }
    return (0);
}

int
collToCollCopy (srbConn *conn, int catType, int flagval,
int inColltype, mdasC_sql_result_struct *inCollResult,
char *origTarg, char *newResource, char *containerName)
{
    int i, status;
    int in_fd, out_fd;
    srb_long_t datasize;
    char *datatype;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN], tmpTarg[MAX_TOKEN];
    char *origSrc, *curTarg;

    if (inCollResult->result_count == 0)
        return 0;
    origSrc = (char *) getFromResultStruct(inCollResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    if (strcmp (origSrc, origTarg) == 0) {
	fprintf (stderr, "target and source collections identical\n");
	return (COLLECTION_NAME_ERROR);
    }

    for (i = 0; i < inCollResult->row_count; i++) {
        if (inColltype != SPEC_COLL_T) {        /* not ".". ".." type */
            int inheritCont;

           if (containerName != NULL && strlen (containerName) > 0) {
                inheritCont = 0;
            } else {
                inheritCont = 1;
            }

            /* Make the collection */
            clSplitbychar(origSrc, temp1, temp,'/');
            status = myCreateCollect(conn, catType, origTarg, temp, 
	      inheritCont);
            if (status < 0 && status != DATA_SUBCOLLECTION_NOT_UNIQUE) {
                fprintf(stderr,
                 "Unable to mk the target coll %s/%s, status = %d\n",
                  origTarg, temp, status);
		srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                return ((int) status);
            }
            sprintf (tmpTarg, "%s/%s", origTarg, temp);
            curTarg = tmpTarg;
        } else {
            curTarg = origTarg;
        }
        /* copy the content */
        status = collToCollCopyCont (conn, catType, flagval,
          origSrc, curTarg, newResource, containerName);

        if (status < 0)
             return ((int) status);
        origSrc += MAX_DATA_SIZE;
    }
    return 0;
}


int
collToCollCopyCont (srbConn *conn, int catType, int flagval, char *srcColl, 
char *targColl, char *newResource, char * containerName) {
    mdasC_sql_result_struct  myresult;
    int i, j;
    int status;

    if (flagval & b_FLAG) {
	status = srbBulkCopy (conn, catType, srcColl, 0, newResource, targColl);
	return (status);
    }

    /* copy the dataset first */

    status = queryDataInColl (conn, catType, srcColl, flagval, MAX_GET_RESULTS,
      &myresult);
    if (status < 0 && status != MCAT_INQUIRE_ERROR)
        return status;

    status = dataToCollCopy (conn, catType, flagval,
      &myresult, -1, targColl, newResource, "", containerName);
    if (status < 0)
        return status;

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        status = srbGetMoreRows(conn, catType,
          myresult.continuation_index, &myresult, MAX_GET_RESULTS);
        if (myresult.result_count == 0  || status != 0)
            break;

        status = dataToCollCopy (conn, catType, flagval,
          &myresult, -1, targColl, newResource, "", containerName);
        if (status < 0)
            break;
    }
    clearSqlResult (&myresult);

    /* now copy the collection recursively */

    status = querySubCollInColl (conn, catType, srcColl, 0, MAX_GET_RESULTS,
      &myresult);
    if (status < 0 && status != MCAT_INQUIRE_ERROR)
        return status;

    status = collToCollCopy (conn, catType, flagval,
      -1, &myresult, targColl, newResource, containerName);

    if (status < 0 && status != MCAT_INQUIRE_ERROR)
         return (status);

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        status = srbGetMoreRows(conn, catType,
          myresult.continuation_index, &myresult, MAX_GET_RESULTS);
        if (myresult.result_count == 0  || status != 0)
            break;

        status = collToCollCopy (conn, catType, flagval,
          -1, &myresult, targColl, newResource, containerName);
        if (status < 0)
            break;
    }
    clearSqlResult (&myresult);
    return (0);
}

int
replicateColl (srbConn *conn, int catType, int flagval, char *srcColl, 
 char *newResource) {
    mdasC_sql_result_struct  myresult;
    int i, j;
    int status;
    char *myColl;

    /* replicate the dataset first */

    status = replDataInColl (conn, catType, srcColl, "",
      newResource, flagval, -1);

        if (status < 0 && status != MCAT_INQUIRE_ERROR)
	    return status;

    /* now replicate the collection recursively */

    status = querySubCollInColl (conn, catType, srcColl, 0, MAX_GET_RESULTS,
      &myresult);
    if (status < 0) {
	if (status == MCAT_INQUIRE_ERROR) {
	    return (0);
	} else {
	    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return status;
	}
    }

     myColl = (char *) getFromResultStruct(&myresult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    for (i = 0; i < myresult.row_count; i++) {
    	status = replicateColl (conn, catType, flagval,
	 myColl, newResource);
	myColl += MAX_DATA_SIZE;
	if (status < 0) {
            clearSqlResult (&myresult);
	    if (status == MCAT_INQUIRE_ERROR)
		return 0;
	    else {
	        srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    	return (status);
	    }
	}
    }

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        status = srbGetMoreRows(conn, catType,
          myresult.continuation_index, &myresult, MAX_GET_RESULTS);
        if (myresult.result_count == 0  || status != 0)
            break;

        myColl = (char *) getFromResultStruct(&myresult,
         dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    	for (i = 0; i < myresult.row_count; i++) {
            status = replicateColl (conn, catType, flagval,
             myColl, newResource);
	    if (status < 0 && status != MCAT_INQUIRE_ERROR) {
	        srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            	clearSqlResult (&myresult);
	        return (status);
	    }
            myColl += MAX_DATA_SIZE;
    	}
    }
    clearSqlResult (&myresult);
    return (0);
}

int
dataToFileCopy (srbConn *conn, int catType, int flagval, int targType,
mdasC_sql_result_struct *dataResult, int srcCopyNum,
char *targFile, char *ticketId)
{
    int i, tmp, cnt;
    srb_long_t datasize;
    char *datatype;
    char temp[MAX_OBJ_NAME_LENGTH];
    char temp1[MAX_TOKEN];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN];
    char *srcPar, *srcCh, *tmpStr, *sizeStr;
    char inbuf[MAX_TOKEN];
    srb_long_t status;

    if (dataResult->result_count == 0)
        return 0;

    srcPar = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    srcCh = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    sizeStr = (char *) getFromResultStruct(dataResult,
      dcs_tname[SIZE], dcs_aname[SIZE]);
    if (sizeStr == NULL)
        datasize = -1;
    else
        datasize = strtoll (sizeStr, 0, 0);

    datatype = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);

    for (i = 0; i < dataResult->row_count;i++) {

        if ((flagval & P_FLAG) != 0) {
            if (srcCopyNum < 0)
                fprintf(stdout,"Copy  %s/%s to local file: %s (y/n)?",
                  srcPar, srcCh, targFile);
            else
                fprintf(stdout,"Copy repl_num=%i of %s/%s to local file: %s (y/n)?",
                 srcCopyNum, srcPar, srcCh, targFile);
            fgets(inbuf,MAX_TOKEN,stdin);
            if (inbuf[0] != 'y' ) {
	        srcPar += MAX_DATA_SIZE;
		srcCh += MAX_DATA_SIZE;
		sizeStr += MAX_DATA_SIZE;
		datatype += MAX_DATA_SIZE;
		continue;
	    }
        }

        if (sizeStr == NULL)
            datasize = -1;
        else
            datasize = strtoll (sizeStr, 0, 0);

        if (srcCopyNum >=  0) {
            sprintf(temp, "%s&COPY=%i", srcCh, srcCopyNum);
        } else {
	    strcpy (temp, srcCh);
	}

	if (strlen(ticketId) > 0) {
	    strcat(temp,"&TICKET=");
	    strcat (temp, ticketId);
        }

	if (strlen(inCondition) > 0)
	  {
	    strcat(temp,"&");
	    strcat(temp,inCondition);
	  }

#ifdef PARA_OPR
	if ((flagval & s_FLAG) ||
	 ((flagval & m_FLAG) == 0 && getenv ("srbParallel") == NULL)) {
            status = srbToLocalFileCopyS (conn, targFile, temp, srcPar, 
	     datasize, flagval);
	} else {
            status = srbToLocalFileCopyP (conn, targFile, temp, srcPar,
             datasize, flagval);
	}
#else
        status = srbToLocalFileCopyS (conn, targFile, temp, srcPar,
         datasize, flagval);
#endif	/* PARA_OPR */

        srcPar += MAX_DATA_SIZE;
        srcCh += MAX_DATA_SIZE;
        sizeStr += MAX_DATA_SIZE;
        datatype += MAX_DATA_SIZE;
    }
    if (status >= 0)
        return (0);
    else 
	return ((int) status);
}

int
dataToDirCopy (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, int srcCopyNum, char *targDir,
char *ticketId)
{
    int i, tmp, cnt;
    srb_long_t status;
    srb_long_t datasize;
    char *datatype;
    char temp[MAX_OBJ_NAME_LENGTH];
    char temp1[MAX_TOKEN];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN];
    char *srcPar, *srcCh, *tmpStr, *sizeStr;
    char inbuf[MAX_TOKEN];

    if (dataResult->result_count == 0)
        return 0;
    srcPar = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    srcCh = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    sizeStr = (char *) getFromResultStruct(dataResult,
      dcs_tname[SIZE], dcs_aname[SIZE]);
    if (sizeStr == NULL)
        datasize = -1;
    else
        datasize = strtoll (sizeStr, 0, 0);

    datatype = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);

    for (i = 0; i < dataResult->row_count;i++) {

        if ((flagval & P_FLAG) != 0) {
            if (srcCopyNum < 0)
                fprintf(stdout,"Copy  %s/%s to directory %s (y/n)?",
                  srcPar , srcCh, targDir);
            else
                fprintf(stdout,
                 "Copy  repl_num=%i of %s/%s to collection %s (y/n)?",
                   srcCopyNum , srcPar , srcCh,  targDir);
            fgets(inbuf,MAX_TOKEN,stdin);
            if (inbuf[0] != 'y' ) {
        	srcPar += MAX_DATA_SIZE;
        	srcCh += MAX_DATA_SIZE;
        	sizeStr += MAX_DATA_SIZE;
        	datatype += MAX_DATA_SIZE;
		continue;
	    }
        }

        if (sizeStr == NULL)
            datasize = -1;
        else
            datasize = strtoll (sizeStr, 0, 0);

        if (srcCopyNum >=  0) {
            sprintf(temp, "%s&COPY=%i",srcCh, srcCopyNum);
        } else {
	    strcpy (temp, srcCh);
	}

        if (strlen(ticketId) > 0) {
	    strcat (temp, "&TICKET=");
	    strcat (temp, ticketId);
        }
	if (strlen(inCondition) > 0)
	  {
	    strcat(temp,"&");
	    strcat(temp,inCondition);
	  }

	sprintf (temp1, "%s/%s", targDir, srcCh);

	status = srbToLocalFileCopy1 (conn, temp1, temp, srcPar, datasize,
	 flagval); 
	if (status < 0) {
	    RStatus = status;
	}

        srcPar += MAX_DATA_SIZE;
        srcCh += MAX_DATA_SIZE;
        sizeStr += MAX_DATA_SIZE;
        datatype += MAX_DATA_SIZE;
    }
    if (status >= 0)
        return (0);
    else 
	return ((int) status);
}

srb_long_t
srbToLocalFileCopy1 (srbConn *conn, char *localPath, char *srbDataName,
char *srbCollection, srb_long_t datasize, int flagval)
{
    srb_long_t status;

#ifdef PARA_OPR
        if ((flagval & s_FLAG) ||
         ((flagval & m_FLAG) == 0 && getenv ("srbParallel") == NULL)) {
            status = srbToLocalFileCopyS (conn, localPath, srbDataName, 
	     srbCollection, datasize, flagval);
        } else {
            status = srbToLocalFileCopyP (conn, localPath, srbDataName, 
	     srbCollection, datasize, flagval);
        }
#else
        status = srbToLocalFileCopyS (conn, localPath, srbDataName, 
	 srbCollection, datasize, flagval);
#endif  /* PARA_OPR */
	if (status < 0) {
	    fprintf (stderr, 
	     "srbToLocalFileCopy1: failed for %s/%s, status=%lld\n",
	     srbCollection, srbDataName, status);
	    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	}
        return (status);
}

srb_long_t
srbToLocalFileCopyS (srbConn *conn, char *localPath, char *srbDataName, 
char *srbCollection, srb_long_t size, int flagval)
{  
  srb_long_t slReturnVal;

  if (flagval & retry_FLAG) {
    int iSleep=2, iRetryCount=0; /* keep retrying */

    do {
      iRetryCount++;
      slReturnVal = srbToLocalFileCopyS2 (conn, localPath, srbDataName, 
	  srbCollection, size, flagval);
      
      if (slReturnVal < 0 && iRetryCount <= iGlbRetriesMax) {
	fprintf(stderr,
	  "srbToLocalFileCopyS2() error (file='%s/%s'), sleeping %d seconds before next retry %d of %d, slReturnVal=%ld\n",
	  srbCollection, srbDataName, iSleep, iRetryCount, 
	  iGlbRetriesMax, slReturnVal);
#ifdef _WIN32
        Sleep(iSleep*1000);
#else
	sleep(iSleep);
#endif
	iSleep *= 2;
	if (iSleep > RETRY_SLEEP_MAX) iSleep=RETRY_SLEEP_MAX;
      }
      
      if (slReturnVal < 0 && iRetryCount > iGlbRetriesMax) {
	fprintf(stderr,
	 "srbToLocalFileCopyS2() (file='%s/%s'), too many error (retries=%d), giving up, slReturnVal = %d\n",
	  srbCollection, srbDataName, (iRetryCount-1), iSleep, slReturnVal);
	break;
      } 
    }
    while (slReturnVal < 0);
  }
  else {
    slReturnVal=
     srbToLocalFileCopyS2 (conn, localPath, srbDataName, 
	 srbCollection, size, flagval);
  }
  return(slReturnVal);
}

srb_long_t
srbToLocalFileCopyS2 (srbConn *conn, char *localPath, char *srbDataName, 
char *srbCollection, srb_long_t size, int flagval)
{
    int in_fd, out_fd, status, iBuffCounter=0;
    char *buf;
    struct stat statbuf;
    int tmp, cnt;
    srb_long_t bytesCopied = 0, ulTotalSize = 1;
    float fSec;
#ifdef _WIN32
    SYSTEMTIME startTime, endTime;
#else
    struct timeval diffTime, startTime, endTime, sTimeDiff;
#endif
    unsigned long ulCheckSum = 0, lTotal=1;

    if (flagval & v_FLAG)
#ifdef _WIN32
        GetSystemTime(&startTime);
#else
        (void) gettimeofday(&startTime, (struct timezone *)0);
#endif

    in_fd = srbObjOpen (conn, srbDataName, O_RDONLY, srbCollection);
    if (in_fd < 0) {
        fprintf(stderr,
         "Unable to open object '%s/%s', status = %d.\n",
          srbCollection, srbDataName, in_fd);
        srb_perror (2, in_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        return(in_fd);
    }


    /***  RAJA Apr 6th 2005 - added a check to remove any conditions that are there ***/
    if (strlen(inCondition) > 0) {
      if ((buf = strstr(localPath,inCondition)) != NULL)
	*buf = '\0';
    } 
    /***  RAJA Apr 6th 2005 - added a check to remove any conditions that are there for target name ***/
    /* Check whether the target file exists */
    
#ifdef _WIN32
    status = stat_file (localPath, &statbuf);
#else
    status = stat (localPath, &statbuf);
#endif
    if (status == 0 && (statbuf.st_mode & S_IFREG) != 0) {  /* A file */
        if ((flagval & F_FLAG) == 0) {
            fprintf (stderr, "Local file %s already exist.\n", localPath);
            srbObjClose (conn, in_fd);
	    RStatus = UTIL_ERROR_TARGET_ALREADY_EXIST;
	    return (0);
        }
#ifdef _WIN32
        out_fd = SrbNtFileBinaryOpen(localPath, O_RDWR|O_TRUNC);
#else
        out_fd = open (localPath, O_RDWR|O_TRUNC, 0666);
#endif
        if (out_fd < 0) {
            fprintf (stderr,
             "Unable to open local file %s, status = %d\n",
              localPath, out_fd);
            srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            srbObjClose (conn, in_fd);
            return (out_fd);
        }
    } else {        /* it doesn't exist */
#ifdef _WIN32
        out_fd = SrbNtFileBinaryCreate(localPath);
#else
        out_fd = creat (localPath, 0644);
#endif
        if (out_fd < 0) {
            fprintf (stderr,
             "Unable to create local file %s, status = %d\n",
              localPath, out_fd);
            srbObjClose (conn, in_fd);
            return (out_fd);
        }
    }

    if (flagval & V_FLAG) {
      struct stat statbuf;
      int status, retVal;
      char myPath[MAX_OBJ_NAME_LENGTH];
      /* get SRB file size for nice progress message */
      
      /* stat the object */
      sprintf (myPath, "%s/%s", srbCollection, srbDataName);
      retVal = srbObjStat (conn, MDAS_CATALOG, myPath, &statbuf);
      
      if (retVal < 0) { /* error */
        fprintf(stderr, "can't stat srb file %s/%s, status = %d\n",
		srbCollection, myPath, retVal);
        fprintf(stderr,"%s",clErrorMessage(conn));
      }
      
      if ((statbuf.st_mode & S_IFREG) != 0) {     /* A SRB file */
	ulTotalSize = (long) statbuf.st_size;
      }
    }

    buf = malloc (BUFSIZE + 1);
    if (buf == NULL) {
        fprintf (stderr, "dataToDirCopy: Unable to malloc buf\n");
        return (-1);
    }

    while ((cnt =  srbObjRead (conn, in_fd, buf, BUFSIZE)) > 0) {
#ifdef _WIN32
      SYSTEMTIME sTimeLast;
#else
      struct timeval sTimeLast;
#endif
      int iLastSec = 0;

        tmp = write (out_fd, buf, cnt);
	iBuffCounter++;
        if (tmp < cnt) {
            fprintf(stderr,
              "Error: dataToDirCopy: Read %d bytes, Wrote %d bytes.\n ",
               cnt, tmp);
            free (buf);
            return (-1);
        }
	bytesCopied += tmp;

	if ((flagval & V_FLAG)) { 
	  float fBs;
	  if (iBuffCounter < 2) {
	    	  
#ifdef _WIN32
	    GetSystemTime(&sTimeLast);
	    fSec = abs((float)startTime.wSecond - (float)sTimeLast.wSecond) +
	      abs((float)startTime.wMilliseconds - 
	      (float)sTimeLast.wMilliseconds/1000.0);
	    fBs = (float) (bytesCopied / nz (fSec));
	    
	    if(abs(sTimeLast.wSecond - startTime.wSecond) >= 0)
	      {
		vNiceVerboseProgressMessage (flagval, INTO_SRB, srbDataName, 
		  localPath, (ulong) bytesCopied,
		  ulTotalSize, fSec, fBs);
	      }
	    iLastSec = sTimeLast.wSecond;
#else
	    (void) gettimeofday (&sTimeLast, (struct timezone *) 0);
	    (void) tvsub (&sTimeDiff, &sTimeLast, &startTime);
	    fSec = (float) sTimeDiff.tv_sec + 
	      ((float) sTimeDiff.tv_usec / 1000000.0);
	    fBs = (float) (bytesCopied / nz (fSec));
	    (void) tvsub (&sTimeDiff, &sTimeLast, &startTime);
	    
	    if (sTimeDiff.tv_sec >= 1) 
	      vNiceVerboseProgressMessage (flagval, INTO_SRB, srbDataName, 
		localPath, (ulong) bytesCopied, ulTotalSize, fSec, fBs);
	    iLastSec = sTimeLast.tv_sec;
#endif
	  } else {
            if (iBuffCounter > 4) iBuffCounter=0;
	  }
	}
    }
    srbObjClose (conn, in_fd);

    free (buf);

    close (out_fd);

    if (size > 0 && size != bytesCopied) {
	fprintf (stderr, 
	  "srbToLocalFileCopyS2: expect size=%lld, bytesCopied=%lld\n",
	  size, bytesCopied);
	return (OBJ_ERR_COPY_LEN);
    }

    if ((flagval & k_FLAG) && bytesCopied > 0) {
        if (0 != clFileSum (localPath, &ulCheckSum)) {
	    fprintf (stderr,
	     "\nsrbToLocalFileCopyS(), local file checksum operation failed\n");
	    srb_perror (2, (int) bytesCopied, "",
	    SRB_RCMD_ACTION | SRB_LONG_MSG);
	} else {
            unsigned long ulSrbCheckSum, ulChecksumFromSrb;
            int iErr;
	    char chksum[CHKSUM_STR_LEN + 1];

            /* XXXXX use srbObjChksum because it takes care of replica.
	     * iErr = iReadChecksumFromSrb(conn, srbDataName, srbCollection, 
	     * &ulChecksumFromSrb);
	     */

	    iErr = srbObjChksum (conn, srbDataName, srbCollection, l_FLAG,
	     NULL, chksum);

	    if (iErr < 0) {
	        fprintf (stderr,
		 "can't retrieve checksum value from SRB:%s/%s, stat = %d\n", 
		  srbCollection, srbDataName, iErr); 
	    } else {
		ulChecksumFromSrb = 
		  (unsigned long) strtol (chksum, (char **) NULL, 10);

	        if (ulCheckSum != ulChecksumFromSrb) {
	            if (ulChecksumFromSrb == 0) {
	                fprintf (stderr,
		         "srbToLocalFileCopyS(), %s/%s has no checksum.\n",
			  srbCollection, srbDataName);
	    	    } else {
	      		fprintf (stderr,
		       "\nsrbToLocalFileCopyS(), local file checksum (%ld) differs from SRB stored checksum (%ld)\n", ulCheckSum, ulChecksumFromSrb);
			return (OBJ_ERR_BAD_CHKSUM);
		    }
	        }
	    }
        }
    }
    
    if (cnt < 0 && cnt != EOF && cnt != MCAT_NO_DATA_FOUND) {
        srb_perror (2, cnt, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	bytesCopied = cnt;
    } else {
      if ((flagval & v_FLAG) && bytesCopied > 0) {
	
#ifdef _WIN32
	GetSystemTime(&endTime);
	fSec = abs((float)startTime.wSecond - (float)endTime.wSecond) +
	  abs((float)startTime.wMilliseconds - 
	  (float)endTime.wMilliseconds/1000.0);
#else
	(void) gettimeofday(&endTime, (struct timezone *)0);
	(void)tvsub(&diffTime, &endTime, &startTime);
	fSec = (float)diffTime.tv_sec +
	  ((float)diffTime.tv_usec / 1000000.);
#endif
	
	vNiceVerboseFinalMessage (flagval, FROM_SRB, srbDataName, localPath,
				  bytesCopied, fSec, -1, ulCheckSum);
      }
    }

    return (bytesCopied);
}

#ifdef PARA_OPR
srb_long_t
srbToLocalFileCopyP (srbConn *conn, char *localPath, char *srbDataName,
		     char *srbCollection, srb_long_t datasize, int flagval)
{
  srb_long_t slReturnVal;
  
  if (flagval & retry_FLAG) {
    int iSleep=2, iRetryCount=0; /* keep retrying */
    
    do {
      iRetryCount++;
      
      slReturnVal = srbToLocalFileCopyP2 (conn, localPath, srbDataName,
					  srbCollection, datasize, flagval);
      
      if (slReturnVal < 0 && iRetryCount <= iGlbRetriesMax) {
	fprintf(stderr,
		"srbToLocalFileCopyP2() error (file='%s/%s'), sleeping %d seconds before next retry %d of %d, slReturnVal=%ld\n",
		srbCollection, srbDataName, iSleep, iRetryCount, iGlbRetriesMax, slReturnVal);
#ifdef _WIN32
    Sleep(iSleep*1000);
#else
	sleep(iSleep);
#endif
	iSleep *= 2;
	if (iSleep > RETRY_SLEEP_MAX) iSleep=RETRY_SLEEP_MAX;
      }
 
     if (slReturnVal < 0 && iRetryCount > iGlbRetriesMax) {
	fprintf(stderr,
		"srbToLocalFileCopyP2.2() error (file='%s/%s'), too many error (retries=%d), giving up, slReturnVal = %d\n",
		srbCollection, srbDataName, (iRetryCount-1), iSleep, slReturnVal);
	break;
      }
    }
    while (slReturnVal < 0);
  }
  else {
    slReturnVal = srbToLocalFileCopyP2 (conn, localPath, srbDataName,
					srbCollection, datasize, flagval);
  }
  return(slReturnVal);
}

srb_long_t
srbToLocalFileCopyP2 (srbConn *conn, char *localPath, char *srbDataName,
char *srbCollection, srb_long_t datasize, int flagval)
{
    struct stat statbuf;
    int status;
    srb_long_t bytesCopied;
    float fSec;
    unsigned long ulCheckSum = 0;
#ifdef _WIN32
    SYSTEMTIME startTime, endTime;
#else
    struct timeval diffTime, startTime, endTime;
#endif
#ifdef _WIN32
    status = stat_file (localPath, &statbuf);
#else
    status = stat (localPath, &statbuf);
#endif

    if (status == 0 && (statbuf.st_mode & S_IFREG) != 0) {  /* A file */
        if ((flagval & F_FLAG) == 0) {
            fprintf (stderr, "Local file %s already exist.\n", localPath);
	    RStatus = UTIL_ERROR_TARGET_ALREADY_EXIST;
            return (0);
        }
	truncate (localPath, 0);
    }

    if (flagval & v_FLAG)
#ifdef _WIN32
        GetSystemTime(&startTime);
#else
        (void) gettimeofday(&startTime, (struct timezone *)0);
#endif


    if (flagval & M_FLAG  || (flagval & m_FLAG) == 0) {
        bytesCopied = srbObjGetC (conn, srbDataName, srbCollection, localPath,
         0, NumThreads);
        if (bytesCopied == SYS_ERR_NO_SUPPORT ||
         bytesCopied == CLI_FUNC_NOT_SUPPORTED) {
            bytesCopied = srbObjGet (conn, srbDataName, srbCollection, 
	     localPath);
	}
    } else {
        bytesCopied = srbObjGet (conn, srbDataName, srbCollection, localPath);
    }

    if (datasize > 0 && datasize != bytesCopied) {
        if (bytesCopied >= 0)
            fprintf(stderr,
             "Error: Wrote %lld bytes, but input file size was %lld bytes.\n",
               bytesCopied, datasize);
        srb_perror (2, (int) bytesCopied, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    }

    if ((flagval & v_FLAG) && bytesCopied > 0) {

#ifdef _WIN32
       GetSystemTime(&endTime);
       fSec = abs((float)startTime.wSecond - (float)endTime.wSecond) +
         abs((float)startTime.wMilliseconds - 
	 (float)endTime.wMilliseconds/1000.0);
#else
       (void) gettimeofday(&endTime, (struct timezone *)0);
       (void)tvsub(&diffTime, &endTime, &startTime);
       fSec = (float)diffTime.tv_sec +
        ((float)diffTime.tv_usec / 1000000.);
#endif
    }
    if ((flagval & k_FLAG) && bytesCopied > 0) {
	if (0 != clFileSum (localPath, &ulCheckSum)) {
	     fprintf (stderr,
	      ": srbToLocalFileCopyP(), local file checsum operation failed\n");
	     srb_perror (2, (int) bytesCopied, "",
	       SRB_RCMD_ACTION | SRB_LONG_MSG);
	 } else {
	    unsigned long ulSrbCheckSum, ulChecksumFromSrb;
	    int iErr;
	    char chksum[CHKSUM_STR_LEN + 1];

	    /* XXXXX use srbObjChksum because it takes care of replica.
	     * iErr = iReadChecksumFromSrb(conn, srbDataName, srbCollection, 
	     * &ulChecksumFromSrb);
	     */
            iErr = srbObjChksum (conn, srbDataName, srbCollection, l_FLAG,
             NULL, chksum);

	    if (iErr < 0) {
	      fprintf (stderr,
	       "can't retrieve checksum value from SRB:%s/%s, stat = %d\n", 
		srbCollection, srbDataName, iErr); 
	    } else {
                ulChecksumFromSrb =
                  (unsigned long) strtol (chksum, (char **) NULL, 10);

	      if (ulCheckSum != ulChecksumFromSrb) {
		if (ulChecksumFromSrb == 0) {
		  fprintf (stderr,
                   "srbToLocalFileCopyS(), %s/%s has no checksum.\n",
                    srbCollection, srbDataName);
		} else {
		    fprintf (stderr,
                      "\nsrbToLocalFileCopyS(), local file checksum (%ld) differs from SRB stored checksum (%ld)\n", ulCheckSum, ulChecksumFromSrb);
                        return (OBJ_ERR_BAD_CHKSUM);
		}
	      }
	    }
	}
    }
       
    if ((flagval & v_FLAG) && bytesCopied > 0) {
       vNiceVerboseFinalMessage (flagval, FROM_SRB, srbDataName, localPath,
	 bytesCopied, fSec,  conn->numThread, ulCheckSum);
    }

    return (bytesCopied);
}

/* end of srbToLocalFileCopyP2 (srbConn *conn, char *localPath, char *srbDataName, */
#endif /* PARA_OPR */

int
collToDirCopy (srbConn *conn, int catType, int flagval,
int inColltype, mdasC_sql_result_struct *inCollResult,
char *targDir, char *ticketId)
{
    int i, status;
    int in_fd, out_fd;
    srb_long_t datasize;
    char *datatype;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN], tmpTarg[MAX_TOKEN];
    char *origSrc, *curTarg;
    struct stat statbuf;

    if (inCollResult->result_count == 0)
        return 0;
    origSrc = (char *) getFromResultStruct(inCollResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    for (i = 0; i < inCollResult->row_count; i++) {
        if (inColltype != SPEC_COLL_T) {        /* not ".". ".." type */
            /* Make the dir */
#ifdef _WIN32
            status = stat_file(targDir, &statbuf);
#else
            status = stat (targDir, &statbuf);
#endif
	    /* this check needed for java srbBrowser */
	    if (status < 0) {
#ifdef _WIN32
                status = mk_dir(targDir, 0755);
#else
                status = mkdir(targDir, 0755);
#endif
                if (status < 0) {
                    fprintf(stderr,
                     "Unable to mk the target dir %s, status = %d\n",
                      tmpTarg, status);
                    return (CLI_ERR_COLLNAME);
                }
                curTarg = targDir;
	    } else {
	        clSplitbychar(origSrc, temp, temp1,'/');
	        sprintf (tmpTarg, "%s/%s", targDir, temp1); 

#ifdef _WIN32
		status = stat_file(tmpTarg, &statbuf);
#else
	        status = stat (tmpTarg, &statbuf);
#endif

	        if (status < 0) {	/* does not exist */
#ifdef _WIN32
		    status = mk_dir(tmpTarg, 0755);
#else
            	    status = mkdir(tmpTarg, 0755);
#endif
            	    if (status < 0) {
                        fprintf(stderr,
                         "Unable to mk the target dir %s, status = %d\n",
                          tmpTarg, status);
                        return (CLI_ERR_COLLNAME);
		    }
                } else if ((statbuf.st_mode & S_IFDIR) == 0) {  /* Not a directory */
		    fprintf(stderr,
                     "collToDir: A local non-directory %s already exists \n",
                      tmpTarg);
                    return (UTIL_ERROR_TARGET_ALREADY_EXIST);
	        }
                curTarg = tmpTarg;
	    }
        } else {
            curTarg = targDir;
        }
        /* copy the content */
        status = collToDirCopyCont (conn, catType, flagval,
          origSrc, curTarg, ticketId);

        if (status < 0)
             return ((int) status);
        origSrc += MAX_DATA_SIZE;
    }
    return status;
}

int
collToDirCopyCont (srbConn *conn, int catType, int flagval, char *srcColl, char
 *targDir, char *ticketId) {
    mdasC_sql_result_struct  myresult;
    int i, j;
    int status;

    /* copy the dataset first */

    status = queryDataInColl (conn, catType, srcColl, flagval, MAX_GET_RESULTS,
      &myresult);
    if (status < 0 && status != MCAT_INQUIRE_ERROR) {
	srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        return status;
    }

    status = dataToDirCopy (conn, catType, flagval,
      &myresult, -1, targDir, ticketId);
    if (status < 0)
        return status;

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        status = srbGetMoreRows(conn, catType,
          myresult.continuation_index, &myresult, MAX_GET_RESULTS);
        if (myresult.result_count == 0  || status != 0)
            break;

        status = dataToDirCopy (conn, catType, flagval,
          &myresult, -1, targDir, ticketId);
        if (status < 0)
            break;
    }
    clearSqlResult (&myresult);

    /* now copy the collection recursively */

    status = querySubCollInColl (conn, catType, srcColl, 0, MAX_GET_RESULTS,
      &myresult);
    if (status < 0 && status != MCAT_INQUIRE_ERROR) {
	srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        return status;
    }

    status = collToDirCopy (conn, catType, flagval,
      -1, &myresult, targDir, ticketId);

    if (status < 0 && status != MCAT_INQUIRE_ERROR)
         return (status);

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        status = srbGetMoreRows(conn, catType,
          myresult.continuation_index, &myresult, MAX_GET_RESULTS);
        if (myresult.result_count == 0  || status != 0)
            break;

        status = collToDirCopy (conn, catType, flagval,
          -1, &myresult, targDir, ticketId);
        if (status < 0)
            break;
    }
    clearSqlResult (&myresult);
    return (0);
}

int
registerDirCont (srbConn *conn, int catType, char *srcDir, 
char *targColl, char *newResource)
{
    mdasC_sql_result_struct  myresult;
    int i, j;
    int status;
    DIR *dirPtr;
    struct dirent *dp;
    struct stat statbuf;
    char tmpName[MAX_TOKEN];
    char tmpDataType[MAX_TOKEN];
    srbPathName mysPathName;

    dirPtr = opendir (srcDir);
    if (dirPtr == NULL) {
        fprintf (stderr, "The local dir %s does not exist., errno = %d\n",
         srcDir, errno);
        return UNIX_ENOENT;
    }

    while ((dp = readdir (dirPtr)) != NULL) {
        if (strcmp (dp->d_name, ".") == 0 || strcmp (dp->d_name, "..") == 0)
            continue;
        sprintf (tmpName, "%s/%s", srcDir, dp->d_name);

#ifdef _WIN32
		status = stat_file(tmpName, &statbuf);
#else
        status = stat (tmpName, &statbuf);
#endif

        if (status != 0) {
            fprintf (stderr, "Unable to stat %s, errno = %d\n", tmpName, errno);
            closedir (dirPtr);
            return status;
        }
        if ((statbuf.st_mode & S_IFREG) != 0) {     /* A file */

            trylocaldatatype(tmpDataType, (char *) tmpName);

            status = srbRegisterDataset (conn, catType, dp->d_name,
             tmpDataType, newResource, targColl, tmpName, statbuf.st_size);
            if (status < 0) {
                closedir (dirPtr);
                return status;
            }
        } else if ((statbuf.st_mode & S_IFDIR) != 0) {      /* A directory */
	    mysPathName.inpArgv = tmpName;
	    mysPathName.outArgv = tmpName;
	    mysPathName.type = COLLECTION_T;
            mysPathName.size = 0;
            status = registerDir (conn, catType, 
              &mysPathName, targColl, newResource);
            if (status < 0) {
                closedir (dirPtr);
                return status;
            }
        }
    }
    closedir (dirPtr);
    return (0);
}

int
registerDir (srbConn *conn, int catType, 
srbPathName *inPathName, char *origTarg, char *newResource)
{
    int i, status;
    int in_fd, out_fd;
    srb_long_t datasize;
    char *datatype;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN], tmpTarg[MAX_TOKEN];
    char *origSrc, *curTarg;

    if (inPathName->type != SPEC_COLL_T) {      /* not ".". ".." type */
        /* Make the collection */
        clSplitbychar(inPathName->outArgv, temp1, temp,'/');
        status = srbCreateCollect(conn, catType, origTarg, temp);
        if (status < 0 && status != DATA_SUBCOLLECTION_NOT_UNIQUE) {
            fprintf(stderr,
             "Unable to mk the target coll %s/%s, status = %d\n",
              origTarg, temp, status);
            return (CLI_ERR_COLLNAME);
        }
        sprintf (tmpTarg, "%s/%s", origTarg, temp);
        curTarg = tmpTarg;
    } else {
        curTarg = origTarg;
    }

    /* copy the content */
    status = registerDirCont (conn, catType, inPathName->outArgv, 
     curTarg, newResource);

    if (status < 0)
        return (CLI_ERR_COLLNAME);

    return 0;
}

void
chmodDataByRes (srbConn *conn, int catType, int flagval, char *mode,
char *userdom, mdasC_sql_result_struct *myresult)
{
    int i, j;
    char *dataList, *collList;
    char inbuf[100];
    int pFlag;
    char userName[MAX_TOKEN], domainName[MAX_TOKEN];

    pFlag = (flagval & P_FLAG);
    dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    collList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    for (i = 0; i < myresult->row_count; i++) {
/*
        if (strstr(dataList,"$deleted") == NULL ) {
*/
        if (pFlag > 0) {        /* Prompt user before proceeding */
            fprintf(stdout,"Modify access for dataset  %s/%s (y/n)?",
             collList, dataList);
            fgets(inbuf,MAX_TOKEN,stdin);
            if (inbuf[0] != 'y' ) {
                fprintf(stdout,"Access for dataset  %s/%s not modified.\n",
                  collList, dataList);
                dataList += MAX_DATA_SIZE;
                collList += MAX_DATA_SIZE;
                continue;
            }
        }
	 clSplitbychar (userdom, userName, domainName, '@');
	if (curatorFlag == 0) {
	  if (!strcmp(mode,"owner")) {
            j = srbModifyDataset(conn,  catType, dataList, collList,
               "","", userName, domainName, D_CHANGE_OWNER);
	  }
	  else if (!strcmp(mode,"null")) {
            j = srbModifyDataset(conn,  catType, dataList, collList,
               "","", userdom, "", D_DELETE_ACCS);
	  } else {
            j = srbModifyDataset(conn,  catType, dataList, collList,
                   "","", userdom, mode, D_INSERT_ACCS);
	  }
	}
	else {/* curatorFlag = 1 */
	  if (!strcmp(mode,"owner")) {
            j = srbModifyDataset(conn,  catType, dataList, collList,
               "","", userName, domainName, D_CHANGE_OWNER_BY_CURATOR);
	  }
	  else if (!strcmp(mode,"null")) {
            j = srbModifyDataset(conn,  catType, dataList, collList,
               "","", userdom, "", D_DELETE_ACCS_BY_CURATOR);
	  } else {
            j = srbModifyDataset(conn,  catType, dataList, collList,
                   "","", userdom, mode, D_INSERT_ACCS_BY_CURATOR);
	  }
	  
	}
	if (j != 0) {
	  fprintf(stdout,"Access for dataset  %s/%s not modified for %s. Error = %i\n",
		  collList, dataList, userdom, j);
	}

/*
    }
*/
    dataList += MAX_DATA_SIZE;
    collList += MAX_DATA_SIZE;
    }
}

/* chmodInColl - Chmod the dataName and/or sub-collection in the given
 * parent collection.
 *
 */

int
chmodInColl (srbConn *conn, int catType, char *parColl, int flagval,
char *mode, char *userdom)
{
    mdasC_sql_result_struct  myresult, collResult;
    int i,jj;
    int status;
    char tmpName[MAX_TOKEN];
    char *collList;

    /* list all collections associated with parColl */
    status = queryCollColl (conn, catType, parColl, flagval, &collResult);
    if (status < 0)
        return status;

    /* Loop through the collections and print out the subcoll and
     * dataName in it. */

    do {
        collList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) &collResult,
          dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

        for (j = 0; j < collResult.row_count; j++) {
            /* print the collection */
/*
            if (strstr(collList, "$deleted") != NULL )
                continue;
*/
            if ((flagval & d_FLAG) == 0)
                jj = chmodColl (conn, catType, collList, flagval, mode, userdom);
            /* chmod the dataName in the coll */
            if ((flagval & C_FLAG) == 0 && (flagval & R_FLAG) > 0)
                jj = chmodDataInColl (conn, catType, collList, flagval, mode,
                  userdom);
            collList += MAX_DATA_SIZE;
        }
        if (collResult.continuation_index >= 0) {
            clearSqlResult (&collResult);
            status = srbGetMoreRows(conn, catType,
             collResult.continuation_index, &collResult, MAX_GET_RESULTS);
        } else
            clearSqlResult (&collResult);
    } while (collResult.row_count > 0 && status == 0);

    return (0);
}

int
replDataByRes (srbConn *conn, int catType, char *pathname, char *resourcename,
int flagval, mdasC_sql_result_struct *myresult, int copyNum)
{
    int i,ii;
    char *dataList, *collList;
    char tmpData[MAX_TOKEN];
    char replNumString[MAX_TOKEN];
    char verNumString[MAX_TOKEN];
    char inbuf[100];
    int pFlag;
    int status;
#ifdef _WIN32
    SYSTEMTIME startTime, endTime;
#else
    struct timeval diffTime, startTime, endTime;
#endif
    float fSec;
    srb_long_t datasize;
     char *sizeStr;

    if (myresult->row_count == 0)
	return 0;

    pFlag = (flagval & P_FLAG);
    dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    collList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    if (flagval & v_FLAG)
        sizeStr = (char *) getFromResultStruct(
	  (mdasC_sql_result_struct *) myresult,
          dcs_tname[SIZE], dcs_aname[SIZE]);

    for (i = 0; i < myresult->row_count; i++) {
        if (pFlag > 0) {        /* Prompt user before proceeding */
            fprintf(stdout,"Modify access for dataset  %s/%s (y/n)?",
             collList, dataList);
            fgets(inbuf,100,stdin);
            if (inbuf[0] != 'y' ) {
                fprintf(stdout,"Access for dataset  %s/%s not modified.\n",
                  collList, dataList);
                dataList += MAX_DATA_SIZE;
                collList += MAX_DATA_SIZE;
                continue;
            }
        }
        if (flagval & v_FLAG)
#ifdef _WIN32
            GetSystemTime(&startTime);
#else
            (void) gettimeofday(&startTime, (struct timezone *)0);
#endif

       if (versionFlag == 0 ) {
	 if (copyNum >= 0) {
	   sprintf(tmpData, "%s&COPY=%i",dataList, copyNum);
	 } else {
	   strcpy (tmpData, dataList);
	 }
	 status = srbObjReplicate(conn, MDAS_CATALOG, tmpData, collList,
				  resourcename, pathname);
	 if (status < 0)
	   return (status);
       }
       else { /* there is a version number change **/
	 if (copyNum < 0) {
	   copyNum = getSomeCopyNum(conn,catType,collList,dataList);
	   if (copyNum < 0) 
	     return (copyNum);
         }
	 sprintf(tmpData, "%s&COPY=%i",dataList, copyNum);
         status = srbObjReplicate(conn, MDAS_CATALOG, tmpData, collList,
                                  resourcename, pathname);
         if (status < 0)
           return (status);
	 sprintf(tmpData,"%s&COPY=%i",dataList,status);
	 /*** data_value_1  changes repl_enum and is of format fromreplnum:toreplnum
	      data_value_2  changes version_num  and is of format fromversion_num:toversion_num ***/
	 sprintf(replNumString,"%i:%i",status,copyNum);
	 sprintf(verNumString,"-1:%s",versionString);
	 ii = srbModifyDataset(conn, MDAS_CATALOG, tmpData,collList, "","",
			       replNumString,verNumString, D_CHANGE_REPLNUM_VERNUM);
	 
	 if (ii < 0) {
	   fprintf (stderr,"Version Change Error for %s/%s:%i\n", tmpData, collList,ii);
	   return(ii);
	 }
       }


        if (flagval & v_FLAG) {
	   datasize = strtoll (sizeStr, 0, 0);
#ifdef _WIN32
           GetSystemTime(&endTime);
           fSec = abs((float)startTime.wSecond - (float)endTime.wSecond) +
              abs((float)startTime.wMilliseconds - (float)endTime.wMilliseconds/1000.0);
#else
           (void) gettimeofday(&endTime, (struct timezone *)0);
           (void)tvsub(&diffTime, &endTime, &startTime);
           fSec = (float)diffTime.tv_sec +
            ((float)diffTime.tv_usec / 1000000.);
#endif
 	    if (datasize > 0) 
                printf
                 ("Fn:%s, sz=%5.4f MB, tm=%3.2f sec, rate=%3.2f MB/sec\n",
                 dataList, (float) datasize / 1000000., fSec,
                  (float) datasize / 1000000. / fSec);
	    sizeStr += MAX_DATA_SIZE;
        }

    	dataList += MAX_DATA_SIZE;
    	collList += MAX_DATA_SIZE;
    }
    return 0;
}

/* replInColl - Replicate the dataName and/or sub-collection in the given
 * parent collection.
 *
 */

int
replInColl (srbConn *conn, int catType, char *parColl, char *pathname,
char *resourcename, int flagval, int copyNum)
{
    mdasC_sql_result_struct  myresult, collResult;
    int i;
    int status;
    char tmpName[MAX_TOKEN];
    char *collList;

    /* list all collections associated with parColl */
    status = queryCollColl (conn, catType, parColl, flagval, &collResult);

    if (status < 0) {
        if (status == MCAT_INQUIRE_ERROR)
            return 0;
        else
            return status;
    }

    collList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) &collResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    /* Loop through the collections and print out the subcoll and
     * dataName in it. */

    do {
        for (j = 0; j < collResult.row_count; j++) {
            /* replicate the dataName in the coll */
            if ((flagval & R_FLAG) > 0)
                status = replDataInColl (conn, catType, collList, pathname, 
		 resourcename, flagval, copyNum);
	        if (status < 0) 
		    return status;
            collList += MAX_DATA_SIZE;
        }

        if (collResult.continuation_index >= 0) {
            clearSqlResult (&collResult);
            status = srbGetMoreRows(conn, catType,
             collResult.continuation_index, &collResult, MAX_GET_RESULTS);
        } else
            clearSqlResult (&collResult);
    } while (collResult.row_count > 0 && status == 0);

    return (0);
}

int
replDataInColl (srbConn *conn, int catType, char *parColl, 
char *pathname, char *resourcename, int flagval, int copyNum)
{
    mdasC_sql_result_struct  dataResult;
    int i, j;
    int status;
    char *rval[MAX_DCS_NUM];

    status = queryDataInColl (conn, catType, parColl, flagval, MAX_GET_RESULTS,
      &dataResult);

    if (status < 0 && status != MCAT_INQUIRE_ERROR)
        return status;

    status = replDataByRes (conn, catType, pathname, resourcename, flagval, 
     &dataResult, copyNum);

    if (status < 0)
        return status;

    /* loop until no more answer */

    while (dataResult.continuation_index >= 0) {
        clearSqlResult (&dataResult);
        status = srbGetMoreRows(conn, MDAS_CATALOG,
          dataResult.continuation_index, &dataResult, MAX_GET_RESULTS);

        if (dataResult.row_count == 0  || status != 0) {
            if (status == MCAT_INQUIRE_ERROR)
            	return 0;
            else
            	return status;
	}

    	status = replDataByRes (conn, catType, pathname, resourcename, flagval, 
     	 &dataResult, copyNum);
    	if (status < 0)
            return status;
    }
    clearSqlResult (&dataResult);
    return (0);
}

int
syncDataByRes (srbConn *conn, int catType, char *resourcename,
int flagval, mdasC_sql_result_struct *myresult)
{
    int i;
    char *dataList, *collList;
    int pFlag;
    int status;
    char tmpTargObj[MAX_TOKEN], *targObj;

    if (myresult->row_count == 0)
        return 0;

    pFlag = (flagval & P_FLAG);
    dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    collList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    for (i = 0; i < myresult->row_count; i++) {
        if (flagval & a_FLAG) {         /* COPIES=all */
            sprintf (tmpTargObj, "%s&COPIES=all", dataList);
	    targObj = tmpTargObj;
	} else {
	    targObj = dataList;
	}

        status = srbSyncData (conn, MDAS_CATALOG, targObj, collList,
         resourcename);
        if (status < 0)
            return (status);

        dataList += MAX_DATA_SIZE;
        collList += MAX_DATA_SIZE;
    }
    return 0;
}

int
backupDataByRes (srbConn *conn, int catType, char *resourcename,
int flagval, mdasC_sql_result_struct *myresult, srbConn **myConn)
{
    int i;
    char *dataList, *collList;
    char *owner, *ownerDomain;
    int pFlag;
    int status;
    char tmpTargObj[MAX_TOKEN], *targObj;
    srbConn *thisConn;

    if (myresult->row_count == 0)
        return 0;

    pFlag = (flagval & P_FLAG);

    dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    collList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    owner = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_OWNER], dcs_aname[DATA_OWNER]);

    ownerDomain = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_OWNER_DOMAIN], dcs_aname[DATA_OWNER_DOMAIN]);

    for (i = 0; i < myresult->row_count; i++) {
        if (flagval & a_FLAG) {         /* COPIES=all */
            sprintf (tmpTargObj, "%s&COPIES=all", dataList);
	    targObj = tmpTargObj;
	} else {
	    targObj = dataList;
	}

	if (flagval & A_FLAG) { 	/* admin backup */
	    char *thisUser, *thisDomain;

	    getClientUserFromConn (conn, &thisUser, &thisDomain);

	    if (strcmp (thisUser, owner) == 0 &&
	      strcmp (thisDomain, ownerDomain) == 0) {
		thisConn = conn;
	    } else {
	        status = getClientUserFromConn(*myConn, &thisUser, &thisDomain);
		if (status >= 0 && (strcmp (thisUser, owner) != 0 ||
		  strcmp (thisDomain, ownerDomain) != 0)) {
		    clFinish (*myConn);
		    *myConn = NULL;
		}

		if (*myConn == NULL) {
		    srbSu (owner, ownerDomain);
		    *myConn = srbConnect (srbHost, NULL, srbAuth,
     		      NULL, NULL, NULL, NULL);
    		    if (clStatus(*myConn) != CLI_CONNECTION_OK) {
        		fprintf(stderr,"backupDataByRes:Conn to srb failed.\n");
        		fprintf(stderr,
			  "backupDataByRes: %s",clErrorMessage(*myConn));
        		srb_perror (2, clStatus(*myConn), "", 
			  SRB_RCMD_ACTION|SRB_LONG_MSG);
        		clFinish(*myConn);
        		return (-1);;
    		    }
		}
		thisConn = *myConn;
	    }
	} else {
	    thisConn = conn;
	}
		
        status = srbBackupData (thisConn, MDAS_CATALOG, targObj, collList,
         resourcename, 0);
        if (status < 0)
            return (status);

        dataList += MAX_DATA_SIZE;
        collList += MAX_DATA_SIZE;
        ownerDomain += MAX_DATA_SIZE;
        owner += MAX_DATA_SIZE;
    }
    return 0;
}

/* syncInColl - Sync the dataName and/or sub-collection in the given
 * parent collection.
 *
 */

int
syncInColl (srbConn *conn, int catType, char *parColl, 
char *resourcename, int flagval)
{
    mdasC_sql_result_struct  myresult, collResult;
    int i;
    int status;
    char tmpName[MAX_TOKEN];
    char *collList;

    /* list all collections associated with parColl */
    status = queryCollColl (conn, catType, parColl, flagval, &collResult);

    if (status < 0) {
        if (status == MCAT_INQUIRE_ERROR)
            return 0;
        else
            return status;
    }

    collList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) &collResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    /* Loop through the collections and print out the subcoll and
     * dataName in it. */

    do {
        for (j = 0; j < collResult.row_count; j++) {
            /* Sync the dataName in the coll */
            if ((flagval & R_FLAG) > 0)
                status = syncDataInColl (conn, catType, collList,
                 resourcename, flagval);
		if (status < 0 && status != MCAT_INQUIRE_ERROR)
		    return (status);
            collList += MAX_DATA_SIZE;
        }

        if (collResult.continuation_index >= 0) {
            clearSqlResult (&collResult);
            status = srbGetMoreRows(conn, catType,
             collResult.continuation_index, &collResult, MAX_GET_RESULTS);
        } else
            clearSqlResult (&collResult);
    } while (collResult.row_count > 0 && status == 0);

    return (0);
}

int
syncDataInColl (srbConn *conn, int catType, char *parColl,
char *resourcename, int flagval)
{
    mdasC_sql_result_struct  dataResult;
    int i, j;
    int status;
    char *rval[MAX_DCS_NUM];

    status = queryDataInColl (conn, catType, parColl, flagval, MAX_GET_RESULTS,
      &dataResult);

    if (status < 0)
        return status;

    status = syncDataByRes (conn, catType, resourcename, flagval,
     &dataResult);

    if (status < 0)
        return status;

    /* loop until no more answer */

    while (dataResult.continuation_index >= 0) {
        clearSqlResult (&dataResult);
        status = srbGetMoreRows(conn, MDAS_CATALOG,
          dataResult.continuation_index, &dataResult, MAX_GET_RESULTS);

        if (dataResult.row_count == 0  || status != 0) {
            if (status == MCAT_INQUIRE_ERROR)
                return 0;
            else
                return status;
        }

        status = syncDataByRes (conn, catType, resourcename, flagval,
         &dataResult);
        if (status < 0)
            return status;
    }
    clearSqlResult (&dataResult);
    return (0);
}

/* backupInColl - backup the dataName and/or sub-collection in the given
 * parent collection.
 *
 */

int
backupInColl (srbConn *conn, int catType, char *parColl, 
char *resourcename, int flagval, srbConn **myConn)
{
    mdasC_sql_result_struct  myresult, collResult;
    int i;
    int status;
    char tmpName[MAX_TOKEN];
    char *collList;

    /* list all collections associated with parColl */
    status = queryCollColl (conn, catType, parColl, flagval, &collResult);

    if (status < 0) {
        if (status == MCAT_INQUIRE_ERROR)
            return 0;
        else
            return status;
    }

    collList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) &collResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    /* Loop through the collections and print out the subcoll and
     * dataName in it. */

    do {
        for (j = 0; j < collResult.row_count; j++) {
            /* Sync the dataName in the coll */
            if ((flagval & R_FLAG) > 0)
                status = backupDataInColl (conn, catType, collList,
                 resourcename, flagval, myConn);
		if (status < 0 && status != MCAT_INQUIRE_ERROR)
		    return (status);
            collList += MAX_DATA_SIZE;
        }

        if (collResult.continuation_index >= 0) {
            clearSqlResult (&collResult);
            status = srbGetMoreRows(conn, catType,
             collResult.continuation_index, &collResult, MAX_GET_RESULTS);
        } else
            clearSqlResult (&collResult);
    } while (collResult.row_count > 0 && status == 0);

    return (0);
}

int
backupDataInColl (srbConn *conn, int catType, char *parColl,
char *resourcename, int flagval, srbConn ** myConn)
{
    mdasC_sql_result_struct  dataResult;
    int i, j;
    int status;
    char *rval[MAX_DCS_NUM];

    status = queryDataInColl (conn, catType, parColl, flagval, MAX_GET_RESULTS,
      &dataResult);

    if (status < 0)
        return status;

    status = backupDataByRes (conn, catType, resourcename, flagval,
     &dataResult, myConn);

    if (status < 0)
        return status;

    /* loop until no more answer */

    while (dataResult.continuation_index >= 0) {
        clearSqlResult (&dataResult);
        status = srbGetMoreRows(conn, MDAS_CATALOG,
          dataResult.continuation_index, &dataResult, MAX_GET_RESULTS);

        if (dataResult.row_count == 0  || status != 0) {
            if (status == MCAT_INQUIRE_ERROR)
                return 0;
            else
                return status;
        }

        status = backupDataByRes (conn, catType, resourcename, flagval,
         &dataResult, myConn);
        if (status < 0)
            return status;
    }
    clearSqlResult (&dataResult);
    return (0);
}

int
chmodDataInColl (srbConn *conn, int catType, char *parColl, int flagval,
char *mode, char *userdom)
{
    mdasC_sql_result_struct  dataResult;
    int i, j;
    int status;
    char *rval[MAX_DCS_NUM];

    status = queryDataInColl (conn, catType, parColl, flagval, MAX_GET_RESULTS,
      &dataResult);

    if (status < 0)
        return status;

    chmodDataByRes (conn, catType, flagval, mode, userdom, &dataResult);

    /* loop until no more answer */

    while (dataResult.continuation_index >= 0) {
        clearSqlResult (&dataResult);
        status = srbGetMoreRows(conn, MDAS_CATALOG,
          dataResult.continuation_index, &dataResult, MAX_GET_RESULTS);
        if (dataResult.row_count == 0  || status != 0)
            return (status);
        chmodDataByRes (conn, catType, flagval, mode, userdom, &dataResult);
    }
    clearSqlResult (&dataResult);
    return (0);
}

int
chmodColl (srbConn *conn, int catType, char *collName, int flagval,
char *mode, char *userdom)
{
    int status;
    char inbuf[100];
    char userName[MAX_TOKEN], domainName[MAX_TOKEN];

/*
    if (strstr(collName,"$deleted") != NULL )
        return 0;
*/

    if ((flagval & P_FLAG) > 0) {    /* Prompt user before acting */
        fprintf(stdout,"Modify access for collection  %s (y/n)?", collName);
        fgets(inbuf,100,stdin);
        if (inbuf[0] != 'y' ) {
            fprintf(stdout,"Access for collection  %s not modified.\n",
              collName);
            return (0);
        }
    }
    clSplitbychar (userdom, userName, domainName, '@');
    if (curatorFlag == 0) {
      if (!strcmp(mode,"owner")) {
        status = srbModifyCollect (conn, catType, collName,
          userName, domainName, "", C_CHANGE_COLL_OWNER);
      }
      else if (!strcmp(mode,"null")) {
        status = srbModifyCollect (conn, catType, collName,
          userName, domainName, "", D_DELETE_COLL_ACCS);
      } else {
        status = srbModifyCollect (conn, catType, collName,
	  userName, domainName, mode , D_INSERT_COLL_ACCS);
      }
    }
    else {/* curatorFlag = 1 */
      if (!strcmp(mode,"owner")) {
        status = srbModifyCollect (conn, catType, collName,
          userName, domainName, "", C_CHANGE_COLL_OWNER_BY_CURATOR);
      }
      else if (!strcmp(mode,"null")) {
        status = srbModifyCollect (conn, catType, collName,
          userName, domainName, "", D_DELETE_COLL_ACCS_BY_CURATOR);
      } else {
        status = srbModifyCollect (conn, catType, collName,
	  userName, domainName, mode , D_INSERT_COLL_ACCS_BY_CURATOR);
      }
    }
    if (status < 0) {
        fprintf (stderr, "Unable of Schmod of collection: %s, status = %d\n",
          collName, status);
    }
    return (status);
}

int 
lsContainer (srbConn *conn, char *containerName, char *userName, char *domainName, int flagval, 
mdasC_sql_result_struct *myresult, char *inpMcatName)
{
    int status, i, ckc;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    char contName[MAX_TOKEN], contCollection[MAX_TOKEN],
     fullContName[MAX_TOKEN];
    char *mcatName;

    

    /* Setup the query */

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;  
    }
    if (strlen(inCondition) > 0) {
      i = fillQvalWithCondition(inCondition, qval);
      if (i != 0) return(i);
    }

    if (containerName != NULL) {

        /*** full pathname is handeled here in lsContainer! Roman 2-7-01 ***/ 
#ifdef FED_MCAT
	char myMcatName[MAX_TOKEN];

	if (containerName[0] != '/') {    /* full path */
	    status = srbGetMcatZone (conn, userName, domainName, myMcatName);
	    if (status >= 0)
		mcatName = myMcatName;
	    else
		mcatName = NULL;
	} else {
	    mcatName = NULL;
	}
#else
	mcatName = NULL;
#endif
        parseContainerName (containerName, contName, contCollection,
         userName, domainName, mcatName);
    
        sprintf (fullContName, "%s/%s", contCollection, contName);


        /*** changed to accomodate wild cards Sept 6, 2000  Roman begin ***/
        if ((status = make_like_for_mdas(fullContName)) < 0) {
          fprintf(stderr,"Dataname too long:%s\n",fullContName);
          return(status);
        }
        if (status < 0) {
            fprintf (stderr,"Container name error:%s\n", fullContName);
            return status;
        }
        /* ckc = status; */
	if (status == 1)
	  /*  RAJA feb 10,2004 changed  by removing the blanks 
	   sprintf(qval[CONTAINER_NAME],
	    " like '%s   %%' ESCAPE '\\'", fullContName); 
	  ***/
	  sprintf(qval[CONTAINER_NAME],
            " like '%s' ESCAPE '\\'", fullContName); 
   	else
	    genEqualSQL (qval[CONTAINER_NAME], fullContName);
    }                                                      /* Roman end */
    else {
        /* list all outstanding containers */
      /* RAJA Feb 10 2005 changed to includ empty names and null names  for container 0 *
        sprintf(qval[CONTAINER_NAME], " is not null");
      */
      /* PETER July 15 2005 amended as per RAJA instructions to fix Slscont *
        sprintf(qval[CONTAINER_NAME], "is not null && <> ''");
      */
        sprintf(qval[CONTAINER_NAME], "is not null");
    }
    selval[CONTAINER_NAME] = 1;

    if (flagval & A_FLAG) {  
        selval[ACCESS_CONSTRAINT] = 1;
        sprintf(qval[USER_NAME]," = '%s'", userName);
        sprintf(qval[DOMAIN_DESC]," = '%s'", domainName);
    } else {
        sprintf(qval[DATA_OWNER]," = '%s'", userName);
        sprintf(qval[DATA_OWNER_DOMAIN]," = '%s'", domainName);
    }

    if (flagval & l_FLAG) {
        selval[PHY_RSRC_NAME] = 1;
        selval[CONTAINER_SIZE] = 1;
        selval[CONTAINER_MAX_SIZE] = 1;
        selval[DATA_ID] = 9;
    }

#ifdef FED_MCAT
    status = srbGetDataDirInfoWithZone (conn, MDAS_CATALOG, inpMcatName, 
     qval, selval, myresult, MAX_GET_RESULTS );
#else
    status = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, 
                                      myresult, MAX_GET_RESULTS );
#endif

    return(status);  

}

int
lsInContainer (srbConn *conn, char *userName, char *domainName,
char *containerName, int flagval, mdasC_sql_result_struct *myresult)
{
    int i,status;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    char contName[MAX_TOKEN], contCollection[MAX_TOKEN],
     fullContName[MAX_TOKEN];
    char *mcatName;
#ifdef FED_MCAT
    char myMcatName[MAX_TOKEN];
#endif
    int ckc;
    
    /* Setup the query */
       
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strlen(inCondition) > 0) { 
      i = fillQvalWithCondition(inCondition, qval);
      if (i != 0) return(i);
    }
        
    selval[DATA_NAME] = 1;
    selval[DATA_GRP_NAME] = 1;
    selval[SIZE] = 1;
    selval[OFFSET] = 1;
    sprintf(qval[DATA_ID] ," <> %s%s",
     MDASSCHEMENAME,"MDAS_AD_REPL.container_id");

    /*** full pathname is handeled here in lsInContainer! Roman 2-7-01 ***/    
#ifdef FED_MCAT
    if (containerName[0] != '/') {    /* full path */
        status = srbGetMcatZone (conn, userName, domainName, myMcatName);
        if (status >= 0)
            mcatName = myMcatName;
        else
            mcatName = NULL;
    } else {
        mcatName = NULL;
    }
#else
    mcatName = NULL;
#endif
    parseContainerName (containerName, contName, contCollection,
     userName, domainName, mcatName);
    
    sprintf (fullContName, "%s/%s", contCollection, contName);


    if ((status = make_like_for_mdas(fullContName)) < 0) {
      fprintf(stderr,"Dataname too long:%s\n", fullContName);
      return(status);  
    }
    ckc = status;
    if (status < 0) {
        fprintf (stderr,"Container name error:%s\n", fullContName);
        return status;
    }

    if (ckc == 1)
      /*  RAJA feb 10,2004 changed  by removing the blanks
       sprintf(qval[CONTAINER_NAME],
        " like '%s   %%' ESCAPE '\\'", fullContName);
      ***/
          sprintf(qval[CONTAINER_NAME],
            " like '%s' ESCAPE '\\'", fullContName);

    else
        genEqualSQL (qval[CONTAINER_NAME], fullContName);

    status = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval,
                                      myresult, MAX_GET_RESULTS );
      
    return (status); 

}

int
rmContainer (srbConn *conn, int catType, char *containerName, int flagval)
{
    char *userName, *domainName;
    mdasC_sql_result_struct  myresult;
    int status;

    if (flagval & F_FLAG) {    /* rm all inContainer object first */
        if (conn->clientUser == NULL) {
            userName = conn->proxyUser->userName;
            domainName = conn->proxyUser->domainName;
        } else {
            userName = conn->clientUser->userName;
            domainName = conn->clientUser->domainName;
        }

        status = lsInContainer (conn, userName, domainName, containerName,
         flagval, &myresult);

        if (status < 0) {
            if (status != CLI_NO_ANSWER && status != MCAT_INQUIRE_ERROR) {
                fprintf (stderr,
                 "Srmcont: Error listing container 'status =  %i\n",
                  status);
                srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                return (status);
            }

        }
        status = rmData (conn, MDAS_CATALOG, flagval, &myresult, -1, 0);

        while (myresult.continuation_index >= 0) {
            clearSqlResult (&myresult);
            status = srbGetMoreRows(conn, MDAS_CATALOG,
             myresult.continuation_index, &myresult, MAX_GET_RESULTS);
            if (myresult.result_count == 0  || status != 0)
                break;
            status = rmData (conn, MDAS_CATALOG, flagval, &myresult, -1, 0);
        }
        clearSqlResult (&myresult);
    }
       
    status = srbRmContainer(conn, MDAS_CATALOG, containerName, D_DELETE_ONE);
    return (status);
}

/* int initSrbEnv () - This is the latest initialization that will do
 * Sinit type proc if it has not been done, but will accept the Session
 * env file that already been initialized.
 */

int initSrbEnv ()
{
    int status;
#if defined(PORTNAME_c90)
        struct jtab jtabBuf;
#endif

    if (EnvFilePath[0] != '\0')    /* already been initialized */
        return 0;

    /* Does the session env file exist ? */

    if (getSessionEnvFile () >= 0) {    /* exist */
        /* Not the first time */
        status = initSrbClientEnv ();
        if (status < 0) {
            EnvFilePath[0] = '\0';
            return (status);
        }
    } else {            /* first time */
        /* do the same thing as in Sinit */
        status = firstInitSrbClientEnv ();
        if (status < 0) {
            EnvFilePath[0] = '\0';
            return (status);
        }
        status = rewriteSrbClientEnvFile ();
        if (status < 0) {
            fprintf (stderr,
             "initSrbEnv: RewriteSrbClientEnvFile err, status = %d\n", status);
            EnvFilePath[0] = '\0';
            return (status);
        }
        status = initSrbClientEnv ();
        if (status < 0) {
            fprintf (stderr,
             "initSrbEnv: initSrbClientEnv error, status = %d\n", status);
            EnvFilePath[0] = '\0';
            return (status);
        }
    }
    return (0);
}

int
dataToDataMove (srbConn *conn, int catType, int flagval, int targType,
mdasC_sql_result_struct *dataResult,  char *targColl, char *targObj)
{
    int i,j, status;
    char tempName[MAX_TOKEN];
    char targPar[MAX_TOKEN], targCh[MAX_TOKEN], cval[MAX_TOKEN];
    char *srcPar, *srcCh;
    char inbuf[MAX_TOKEN];

    if (dataResult->result_count == 0)
        return 0;
    srcPar = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    srcCh = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    for (i = 0; i < dataResult->row_count;i++) {
      if ((flagval & P_FLAG) != 0) {
	fprintf(stdout,"Copy  %s/%s to %s/%s (y/n)?",
                  srcPar, srcCh, targColl, targObj);
	fgets(inbuf,MAX_TOKEN,stdin);
	if (inbuf[0] != 'y' ) {
            srcPar += MAX_DATA_SIZE;
            srcCh += MAX_DATA_SIZE;
	    continue;
	}
      }
      if (!strcmp(srcPar,targColl)) { /* only renaming data */
	j =  srbModifyDataset(conn, MDAS_CATALOG,srcCh, srcPar,
			      "","", targObj, "", D_CHANGE_DNAME);
	if (j < 0) {
	  fprintf(stderr, 
		  "dataToDataMove: Error in renaming %s/%s to %s/%s: %i\n",
		  srcPar, srcCh, targColl, targObj, j);
	  srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	}
      }
      else if (!strcmp(srcCh,targObj)) { /* only moving to new collection */
	j = srbModifyDataset(conn, MDAS_CATALOG,srcCh, srcPar,
			     "","", targColl, "", D_CHANGE_GROUP);
	if (j < 0) {
	  fprintf(stderr, 
		  "dataToDataMove: Error in renaming %s/%s to %s/%s: %i\n",
		  srcPar, srcCh, targColl, targObj, j);
	  srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	}
      }
      else { /* changing name of object as well as its collection */
	srandom((unsigned int) getpid());
	sprintf(cval,"%ld",(long) random());
	sprintf(tempName, "%s.%s", targObj,cval);
	j = srbModifyDataset(conn, MDAS_CATALOG,srcCh, srcPar,
			      "","", tempName, "", D_CHANGE_DNAME);
	if (j < 0) {
	  fprintf(stderr, 
		  "dataToDataMove: Error at Stage 1 in renaming %s/%s to %s/%s: %i\n",
		  srcPar, srcCh, targColl, targObj, j);
	  srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            srcPar += MAX_DATA_SIZE;
            srcCh += MAX_DATA_SIZE;
            continue;
	}
	j = srbModifyDataset(conn, MDAS_CATALOG,tempName, srcPar,
			     "","", targColl, "", D_CHANGE_GROUP);
	if (j < 0) {
	  srbModifyDataset(conn, MDAS_CATALOG,tempName, srcPar,
			   "","", srcCh, "", D_CHANGE_DNAME);
	  fprintf(stderr, 
		  "dataToDataMove: Error at Stage 2 in renaming %s/%s to %s/%s: %i\n",
		  srcPar, srcCh, targColl, targObj, j);
	  srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
          srcPar += MAX_DATA_SIZE;
          srcCh += MAX_DATA_SIZE;
          continue;
	}
	j =  srbModifyDataset(conn, MDAS_CATALOG,tempName, targColl,
				      "","", targObj, "", D_CHANGE_DNAME);
	if (j < 0) {
	  srbModifyDataset(conn, MDAS_CATALOG,tempName, targColl,
			   "","", srcPar, "", D_CHANGE_GROUP);
	  srbModifyDataset(conn, MDAS_CATALOG,tempName, srcPar,
			   "","", srcCh, "", D_CHANGE_DNAME);
	  fprintf(stderr, 
		  "dataToDataMove: Error at Stage 3 in renaming %s/%s to %s/%s: %i\n",
		  srcPar, srcCh, targColl, targObj, j);
	  srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	}
      }
      srcPar += MAX_DATA_SIZE;
      srcCh += MAX_DATA_SIZE;
    }
    return (0);
}

int
dataToCollMove (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, char *targColl)
{
    int i,j, status;
    char *srcPar, *srcCh;
    char inbuf[MAX_TOKEN];

    if (dataResult->result_count == 0)
        return 0;
    srcPar = (char *) getFromResultStruct(dataResult,
		   dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    srcCh = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    for (i = 0; i < dataResult->row_count;i++) {
      if ((flagval & P_FLAG) != 0) {
	fprintf(stdout,"Copy  %s/%s to collection %s (y/n)?",
		srcPar , srcCh,  targColl);
	fgets(inbuf,MAX_TOKEN,stdin);
	if (inbuf[0] != 'y' ) {
	    srcPar += MAX_DATA_SIZE;
	    srcCh += MAX_DATA_SIZE;
	    continue;
	}
      }
      j = srbModifyDataset(conn, MDAS_CATALOG,srcCh, srcPar,
			   "","", targColl, "", D_CHANGE_GROUP);
      if (j < 0) {
	fprintf(stderr, 
		"dataToCollMove: Error in renaming %s/%s to %s: %i\n",
		srcPar, srcCh, targColl, j);
	srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      }
      srcPar += MAX_DATA_SIZE;
      srcCh += MAX_DATA_SIZE;
    }
    return (0);
}

int
collToCollMove (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *inCollResult, char *origTarg)
{
    int i,j, status;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN], tmpTarg[MAX_TOKEN];
    char *origSrc, *curTarg;
    char inbuf[MAX_TOKEN];
    char srcParColl[MAX_TOKEN],srcCollName[MAX_TOKEN];
    char newTarg[MAX_TOKEN];
    if (inCollResult->result_count == 0)
        return 0;
    origSrc = (char *) getFromResultStruct(inCollResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    for (i = 0; i < inCollResult->row_count; i++) {
      if ((flagval & P_FLAG) != 0) {
	fprintf(stdout,"Copy  %s to collection %s (y/n)?",
		origSrc,  origTarg);
	fgets(inbuf,MAX_TOKEN,stdin);
	if (inbuf[0] != 'y' ) {
	    origSrc += MAX_DATA_SIZE;
	    continue;
	}
      }
      splitpath(origSrc,srcParColl,srcCollName);
      sprintf(newTarg,"%s/%s",origTarg,srcCollName);
      j = srbModifyCollect(conn, MDAS_CATALOG, origSrc,
		       newTarg, "", "", D_CHANGE_COLL_NAME);
      if (j < 0)  {
	j = srbModifyCollect(conn, MDAS_CATALOG, origSrc,
		       origTarg, "", "", D_CHANGE_COLL_NAME);
	if (j < 0) {
	  fprintf(stderr,
                "collToCollMove: Error when moving collection %s to %s:%i\n", 
		origSrc,origTarg,j);
	  srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	}
      }
      origSrc += MAX_DATA_SIZE;
    }
    return 0;
}

srb_long_t 
srbTosrbCopyS (srbConn *conn, int catType, char *srcObjID, char *srcCollection,
char *destObjID, char *destCollection, char *destResource, 
char *datatype, char *newPath, srb_long_t datasize, int existFlag, int flagval)
{

    int in_fd, out_fd;
    srb_long_t status;

    if (existFlag == 0) {
        out_fd = srbObjCreate (conn, catType, destObjID,
          datatype, destResource, destCollection, newPath, datasize);
        if (out_fd < 0) {
            fprintf (stderr,
             "Unable to create object %s/%s, status = %d\n",
              destCollection, destObjID, out_fd);
            srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            srbObjClose (conn, in_fd);
            return (out_fd);
        }
    } else {
        if ((flagval & F_FLAG) == 0 && (flagval & a_FLAG) == 0) {
                fprintf (stderr, "Target object %s/%s already exist.\n",
                  destCollection, destObjID);
	    RStatus = UTIL_ERROR_TARGET_ALREADY_EXIST;
            return (0);
        }

        if ((flagval & a_FLAG) != 0) {      /* overwrite all replica */
            out_fd = srbObjOpen (conn, destObjID, O_WR_REPL|O_TRUNC,
             destCollection);
        } else {
            out_fd = srbObjOpen (conn, destObjID, O_RDWR|O_TRUNC,
             destCollection);
        }
        if (out_fd < 0) {
            fprintf (stderr,
             "Unable to create object %s/%s, status = %d\n",
              destCollection, destObjID, out_fd);
            srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return (out_fd);
        }
    }

    in_fd = srbObjOpen (conn, srcObjID, O_RDONLY, srcCollection);
    if (in_fd < 0) {
        fprintf(stderr,
         "Unable to open object '%s/%s', status = %d.\n",
          srcCollection, srcObjID, in_fd);
        srb_perror (2, in_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	srbObjClose (conn, out_fd);
        return(in_fd);
    }

    status = srbObjProxyOpr (conn, OPR_COPY, in_fd, out_fd, 0, 0,
     NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    srbObjClose (conn, out_fd);
    srbObjClose (conn, in_fd);

    if (status < 0) {
      srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    }
    return (status);
}

#ifdef PARA_OPR
void *doPartialOpr (void *inFileOpr)
{
    struct fileOpr *myFileOpr;
    srbConn *conn;
    int  i, nbytes,  in_fd, out_fd;
    char *buf;
    int *retval;
    int status;
    srb_long_t sstatus;
    srb_long_t ioSize, sizeRemaining;
    int retryCnt = 0;

   
    myFileOpr = (struct fileOpr *) inFileOpr;

    conn = clConnect (NULL, NULL, NULL);
    if (clStatus(conn) != CLI_CONNECTION_OK) {
        fprintf(stderr,"Connection to SRB server failed.\n");
        fprintf(stderr,"%s",clErrorMessage(conn));
        clFinish(conn);
        exit (1);
    }

    retval = (int *) malloc (sizeof (int));
    buf = malloc (BUFSIZE);

    if (myFileOpr->rwFlag == READ_OPR) {
        if (myFileOpr->flagval & v_FLAG) {
          fprintf (stderr,"opening local file for %i.\n",
                   myFileOpr->threadInx);
          fflush(stderr);
        }
        out_fd = open (myFileOpr->localFileName,  O_RDWR | O_CREAT, 0644);
        if (out_fd < 0) {
            fprintf(stderr,
             "can't open local file %s, errno = %d\n",
               myFileOpr->localFileName, errno);
            clFinish(conn);
            exit (1);
        }

        if (myFileOpr->flagval & v_FLAG) {
          fprintf (stderr,"opening srbObj for %i.\n",
                   myFileOpr->threadInx);
          fflush(stderr);
        }
        in_fd = srbObjOpen (conn, myFileOpr->objName, O_RDONLY,
         myFileOpr->collection);
        if (in_fd < 0)  {   /* error */
            fprintf(stderr, "can't open srb obj %s\n", myFileOpr->objName);
            srb_perror (2, in_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            clFinish(conn);
            exit (1);
        }
        if (myFileOpr->flagval & v_FLAG) {
          fprintf (stderr,"opened srbObj for %i.\n",
                   myFileOpr->threadInx);
          fflush(stderr);
        }

        if (myFileOpr->offset != 0) {
            sstatus = srbObjSeek (conn, in_fd, myFileOpr->offset, SEEK_SET);
            if (sstatus != myFileOpr->offset) {
                fprintf(stderr, "srbObjSeek loc %lld != input %lld\n",
                 sstatus, myFileOpr->offset);
                clFinish(conn);
                srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                exit (1);
            }
        }


        if (myFileOpr->offset != 0) {
            sstatus = lseek (out_fd, myFileOpr->offset, SEEK_SET);
            if (sstatus != myFileOpr->offset) {
                fprintf(stderr, "lseek loc %lld != input %lld, errno=%d\n",
                 sstatus, myFileOpr->offset, (int) errno);
                clFinish(conn);
                exit (1);
            }
        }

        sizeRemaining = myFileOpr->size;

        while (sizeRemaining > 0) {
            if (myFileOpr->size >= BUFSIZE) {
                ioSize = BUFSIZE;
            } else {
                ioSize = myFileOpr->size;
            }
            nbytes = srbObjRead (conn, in_fd, buf, (int) ioSize);
            if (nbytes <= 0)
                break;

            if (write (out_fd, buf, nbytes) <= 0) {
                fprintf(stderr, "local write errror for %s, errno = %d\n",
                 myFileOpr->objName, errno);
                srbObjClose (conn, in_fd);
                close (out_fd);
                clFinish(conn);
                *retval = -6;
                return ((void *) retval);
            }
            sizeRemaining -= (srb_long_t) nbytes;
            if (myFileOpr->flagval & v_FLAG) {
              fprintf (stderr," R%i ",myFileOpr->threadInx);
              fflush(stderr);
            }
        }
        if (myFileOpr->flagval & v_FLAG) {
          fprintf (stderr,"\nclosing srbObj for %i.\n",
                   myFileOpr->threadInx);
          fflush(stderr);
        }
        srbObjClose (conn, in_fd);
        if (myFileOpr->flagval & v_FLAG) {
          fprintf (stderr,"closed srbObj for %i.\n",
                   myFileOpr->threadInx);
          fflush(stderr);
        }
        close (out_fd);
    } else if (myFileOpr->rwFlag == WRITE_OPR) {
        in_fd = open (myFileOpr->localFileName,  O_RDONLY, 0644);
        if (in_fd < 0) {
            fprintf(stderr, "can't open local file %s, errno = %d\n",
             myFileOpr->localFileName, errno);
            clFinish(conn);
            *retval = -2;
            return ((void *) retval);
        }
        if (myFileOpr->threadInx == 0) {
            pthread_mutex_lock (&Glock);
            if (myFileOpr->flagval & v_FLAG) {
              fprintf (stderr,"creating srbObj for %i.\n",
                       myFileOpr->threadInx);
                fflush(stderr);
            }
            out_fd = srbObjCreate (conn, MDAS_CATALOG, myFileOpr->objName,
             NULL, myFileOpr->resource, myFileOpr->collection, NULL, -1);
            pthread_mutex_unlock (&Glock);
            if (out_fd < 0)  {
                fprintf(stderr,
                 "can't create srb obj %s. status = %d\n",
                  myFileOpr->objName, out_fd);
                srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                srbObjUnlink (conn, myFileOpr->objName, myFileOpr->collection);
                clFinish(conn);
                exit (1);
            }
            if (myFileOpr->flagval & v_FLAG) {
              fprintf (stderr,"created srbObj for %i.\n",
                       myFileOpr->threadInx);
                fflush(stderr);
            }
        } else {
            if (myFileOpr->flagval & v_FLAG) {
                fprintf (stderr,"opening srbObj for %i.\n",
                  myFileOpr->threadInx);
                fflush(stderr);
            }

            while (retryCnt < MAX_P_OPEN_RETRY) {
                pthread_mutex_lock (&Glock);
                pthread_mutex_unlock (&Glock);
                out_fd = srbObjOpen (conn, myFileOpr->objName, O_RDWR,
                 myFileOpr->collection);
                if (out_fd >= 0)  {
                    break;
                }
                /* XXXXXX - does not work with threa - usleep (50000); */
                retryCnt ++;
            }
            if (out_fd < 0)  {   /* error */
                fprintf(stderr,
                 "can't open srb obj\"%s\"\n", myFileOpr->objName);
                srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                srbObjUnlink (conn, myFileOpr->objName, myFileOpr->collection);
                clFinish(conn);
                exit (1);
            }
            if (myFileOpr->flagval & v_FLAG) {
              fprintf (stderr,"opened srbObj for %i.\n",
                       myFileOpr->threadInx);
                fflush(stderr);
            }
        }
        if (myFileOpr->offset != 0) {
            sstatus = srbObjSeek (conn, out_fd, myFileOpr->offset, SEEK_SET);
            if (sstatus != myFileOpr->offset) {
                fprintf(stderr, 
		  "srbObjSeek error for %d, loc %lld != input %lld\n",
                   myFileOpr->threadInx, sstatus, myFileOpr->offset);
                srb_perror (2, (int) sstatus, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                srbObjUnlink (conn, myFileOpr->objName, myFileOpr->collection);
                clFinish(conn);
                exit (1);
            }
        }

        if (myFileOpr->offset != 0) {
            sstatus = lseek (in_fd, myFileOpr->offset, SEEK_SET);
            if (sstatus != myFileOpr->offset) {
                fprintf(stderr,
                  "lseek error for %d, loc %lld != input %lld, errno=%d\n",
                    myFileOpr->threadInx, sstatus, myFileOpr->offset,  errno);
                srbObjUnlink (conn, myFileOpr->objName, myFileOpr->collection);
                clFinish(conn);
                exit (1);
            }
        }

        sizeRemaining = myFileOpr->size;
        while (sizeRemaining > 0) {
            if (myFileOpr->size >= BUFSIZE) {
                ioSize = BUFSIZE;
            } else {
                ioSize = myFileOpr->size;
            }
            nbytes = read (in_fd, buf, ioSize);
            if (nbytes <= 0)
                break;
            if ((status = srbObjWrite (conn, out_fd, buf, nbytes)) <= 0) {
                fprintf(stderr, "srb write errror for %s, errno = %d\n",
                 myFileOpr->objName, status);
                srbObjClose (conn, out_fd);
                close (in_fd);
                srbObjUnlink (conn, myFileOpr->objName, myFileOpr->collection);
                clFinish(conn);
                exit (1);
            }
            sizeRemaining -= (srb_long_t) nbytes;
            if (myFileOpr->flagval & v_FLAG) {
              fprintf (stderr," W%i ",myFileOpr->threadInx);
              fflush(stderr);
            }
        }
        srbObjClose (conn, out_fd);
        close (in_fd);
    }

    free (buf);
    if (myFileOpr->flagval & v_FLAG) {
      fprintf (stderr,
	"start finish processing srb file %s for %i\n", 
	 myFileOpr->objName,myFileOpr->threadInx);
      fflush (stderr);
    }
    clFinish(conn);
    if (myFileOpr->flagval & v_FLAG) {
      fprintf (stderr,
	"finished processing srb file %s for %i\n", 
	 myFileOpr->objName, myFileOpr->threadInx);
      fflush (stderr);
    }
    *retval = 0;
    return ((void *) retval);
}
#endif	/* PARA_OPR */

char *
getHostByResource (srbConn *conn, char *resourceName, char *mcatName) 
{
  mdasC_sql_result_struct myresult;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int selval[MAX_DCS_NUM], i, j;
  char *myHost, *portNum, *tmpHost;

  for (i = 0; i < MAX_DCS_NUM; i++) {
    selval[i] = 0;
    sprintf(qval[i],"");
  }

  genEqualSQL (qval[RSRC_NAME], resourceName);
  selval[RSRC_ADDR_NETPREFIX] = 1;

  i = srbGetDataDirInfoWithZone(conn, MDAS_CATALOG, mcatName,
   qval, selval, &myresult, DEFAULT_ROW_COUNT);
  if (i < 0)
    return (NULL);

  tmpHost = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) &myresult,
                  dcs_tname[RSRC_ADDR_NETPREFIX],
                  dcs_aname[RSRC_ADDR_NETPREFIX]);


  if (tmpHost == NULL)
    return NULL;

  srandom(time(0));

  j = (random() >> 4) % myresult.row_count;

  myHost = strdup (&tmpHost[j*MAX_DATA_SIZE]);

  portNum = parseAddr (myHost);

  clearSqlResult (&myresult);


  return (myHost);
}

void
show_results_simple(srbConn *conn, 
		    mdasC_sql_result_struct *myresult, 
		    int L_val, int Fflag)
{

  int i,ii, j;
  char *rval[MAX_DCS_NUM];
  char *tval[MAX_DCS_NUM];
  char *aval[MAX_DCS_NUM];
  char *cval[MAX_DCS_NUM];
  int  ival[MAX_DCS_NUM];
  int done = 0;
  int cnt = 0;
  int firsttime;

  firsttime = 1;
  if (!Fflag) 
   printf("------------------------- RESULTS --------------------------\n"); 


  while (done == 0) {
    for (i=0; i < myresult->result_count; i++) {
      rval[i] = myresult->sqlresult[i].values;
      tval[i] = myresult->sqlresult[i].tab_name;
      aval[i] = myresult->sqlresult[i].att_name;
      if (firsttime) {
	ival[i] = getIndexForTabAttr(tval[i],aval[i], &cval[i]);
	if (ival[i] < 0) {
	  fprintf(stderr,"Wrong TableName, AttributeName Given\n");
	  clFinish(conn);
	  exit(1);
	}
	if (Fflag) {
	  if (i == 0)
	    fprintf(stdout,"%i:%s",ival[i],cval[i]);
	  else
	    fprintf(stdout,"|%i:%s",ival[i],cval[i]);
	}
      }
    }
    if (Fflag && firsttime) {
      fprintf(stdout,"\n");
    }
    firsttime = 0;
    j = 0;
    while ( j < myresult->row_count) {
      for (i=0; i < myresult->result_count; i++) {
	if (Fflag) {
	  if (i > 0)
	    fprintf(stdout, "|%s",rval[i]);
	  else
	    fprintf(stdout, "%s",rval[i]);
	}
	else
	  fprintf(stdout, "%s:%s\n",cval[i], rval[i]);
	rval[i] += MAX_DATA_SIZE;
      }
      if (Fflag)
	fprintf(stdout, "\n");
      cnt++;
      j++;
      if (!Fflag) {
	if (i > 1)
	  printf("---------------------------------------------------\n"); 
      }
      
    }
    if (myresult->continuation_index < 0) {
      if (!Fflag) 
	fprintf(stdout, "Number of Items: %i\n",cnt);
      return;
    }
    ii = srbGetMoreRows(conn, MDAS_CATALOG,myresult->continuation_index, 
			myresult, L_val);
    if (myresult == NULL || ii != 0) {
      fprintf(stderr, "Error in srbGetMoreRows:%i\n",i);
      if (!Fflag) 
	fprintf(stdout, "Number of Items: %i\n",cnt);
      return;
    }
  }
  if (!Fflag) 
    fprintf(stdout, "Number of Items: %i\n",cnt);
  return; 
}
int
getIndexForTabAttr(char *tabName, char *attrName, char **constName)
{
  int i;
  for (i = 0; i < MAX_DCS_NUM; i++) {
    if (strstr(tabName,dcs_tname[i]) && 
	!strcmp(attrName,dcs_aname[i])) {
      *constName = dcs_cname[i];
      return(i);
    }
  }
  return (-1);
}

int
getIndexForConstName(char *constName)
{
  int i;
  for (i = 0; i < MAX_DCS_NUM; i++) {
    if (!strcmp(dcs_cname[i],constName))
      return(i);
  }
  return (-1);
}

void unmakeSafeString(char *source, char* target, int target_size)
{
        register int x,y;

        for(x=0,y=0;source[y] && target_size > x;++x,++y)
        {
                if((target[x] = source[y]) == '=')
                {
                        target[x] = (source[y+1] >= 'A' ? ((source[y+1] & 0xdf) - 'A')+10 : (source[y+1] - '0'));
                        target[x] *= 16;
                        target[x] += (source[y+2] >= 'A' ? ((source[y+2] & 0xdf)-'A')+10 : (source[y+2] - '0'));
                        y+=2;
                }
        }

        target[x] = '\0';
}

/* assumes sizeof char is 1 byte in bitwise operations */
/* no double code attempts to prevent reencoding a coded string. Method is heuristic. */
#define NO_DOUBLE_CODE 1
void makeSafeString(char *source, char* target, int target_size)
{
        register int i, j;
        register char test;
        for(i = 0, j = 0; source[i] && target_size > j; i++, j++)
        {
                switch(source[i])
                {
                        case '=':
#if NO_DOUBLE_CODE
                        test = (source[i+1] >= 'A' ? ((source[i+1] & 0xdf) - 'A')+10 : (source[i+1] - '0'));
                        test *= 16;
                        test += (source[i+2] >= 'A' ? ((source[i+2] & 0xdf)-'A')+10 : (source[i+2] - '0'));
                        switch(test)
                        {
                                case '%':
                                case '=':
                                case '&':
                                case '\'':
                                case '*':
                                case '/':
                                case '?':
                                case '\\':
                                case '^':
                                case '`':
                                case '~':
                                strcpy(target, source);
                                return;
                                default:
                                break;
                        }
#endif
                        case '%':
                        case '&':
                        case '\'':
                        case '*':
                        case '/':
                        case '?':
                        case '\\':
                        case '^':
                        case '`':
                        case '~':
                        target[j] = '=';
                        target[++j] = (source[i] >> 4);
                        target[j] = target[j] > 9 ? target[j] + 55 : target[j] + 48;
                        target[++j] = (source[i] & 0x0F);
                        target[j] = target[j] > 9 ? target[j] + 55 : target[j] + 48;
                        break;
                        default:
                        target[j] = source[i];
                }
        }
        target[j] = '\0';
}

void makeSafeStringFromPath(char *source, char* target, int target_size)
{
        register int i, j;
        register char test;
        for(i = 0, j = 0; source[i] && target_size > j; i++, j++)
        {
                switch(source[i])
                {
                        case '=':
#if NO_DOUBLE_CODE
                        test = (source[i+1] >= 'A' ? ((source[i+1] & 0xdf) - 'A')+10 : (source[i+1] - '0'));
                        test *= 16;
                        test += (source[i+2] >= 'A' ? ((source[i+2] & 0xdf)-'A')+10 : (source[i+2] - '0'));
                        switch(test)
                        {
                                case '=':
                                case '%':
                                case '&':
                                case '\'':
                                case '*':
                                case '?':
                                case '\\':
                                case '^':
                                case '`':
                                case '~':
                                strcpy(target, source);
                                return;
                                default:
                                break;
                        }
#endif
                        case '%':
                        case '&':
                        case '\'':
                        case '*':
                        case '?':
                        case '\\':
                        case '^':
                        case '`':
                        case '~':
                        target[j] = '=';
                        target[++j] = (source[i] >> 4);
                        target[j] = target[j] > 9 ? target[j] + 55 : target[j] + 48;
                        target[++j] = (source[i] & 0x0F);
                        target[j] = target[j] > 9 ? target[j] + 55 : target[j] + 48;
                        break;
                        default:
                        target[j] = source[i];
                }
        }
        target[j] = '\0';
}

/*requires 22+ character buffer*/
char* getUTCTime(char* target)
{
        time_t t;
        struct tm* gmt;

        time(&t);
        gmt = gmtime(&t);
        sprintf(target, "%d-%02d-%02dT%02d:%02d:%02dZ", 1900 + gmt->tm_year,
                gmt->tm_mon + 1, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
        return target;
}

#ifdef	foo
int
isInTrash (char *collection) 
{
    char *tmpPtr;
    int c;

    if (strncmp (collection, "/trash/", 7) == 0) {
	if (strcmp (collection, "/trash/home") == 0 ||
	 strcmp (collection, "/trash/container") == 0) {
	    return (IS_TRASH_ROOT);
	} else {
	    return (IS_IN_TRASH);
	}
    }

    if (strcmp (collection, "/trash") == 0)
        return (IS_TRASH_ROOT);

    tmpPtr = collection + 1;
    while ((c = *tmpPtr) != '\0') {
	if (c == '/')
	    break;
	tmpPtr++;
    }

    if (c == '/') {
        if (strncmp (tmpPtr, "/trash/", 7) == 0) {
            if (strcmp (collection, "/trash/home") == 0 ||
             strcmp (collection, "/trash/container") == 0) {
                return (IS_TRASH_ROOT);
            } else {
                return (IS_IN_TRASH);
            }
	}
        if (strcmp (tmpPtr, "/trash") == 0)
            return (IS_IN_TRASH);
    }
    return (NOT_IN_TRASH);
}
#endif

int
trashExpired (int inpPurgeTime, char *srcCh, char *srcPar)
{
    char *tmpPtr, *tmpPtr1;
    struct tm expTm;
    int year, month, day, hour, minute, second;
    int status;
    time_t expTime, curTime;

    if (inpPurgeTime <= 0)
        return TRASH_EXPIRED;

    memset (&expTm, 0, sizeof (struct tm));

    tmpPtr1 = srcPar;
    while (tmpPtr1 != NULL && (tmpPtr = strstr (tmpPtr1, "::")) != NULL) {
        tmpPtr += 2;    /* skip "::" */
        if (sscanf (tmpPtr, "%d-%02d-%02d-%02d.%02d.%02d", &expTm.tm_year,
         &expTm.tm_mon, &expTm.tm_mday, &expTm.tm_hour, &expTm.tm_min,
         &expTm.tm_sec) == 6)
            break;
        tmpPtr1 = tmpPtr;
    }
    if (tmpPtr == NULL) {       /* date not in collection name */
        /* try dataName */

        if (srcCh == NULL) {
            if (srcPar == NULL)
                fprintf (stderr,
                 "trashExpired: Input has no path\n");
            return TRASH_UNKNOWN;
        }

        tmpPtr1 = srcCh;
        while (tmpPtr1 != NULL && (tmpPtr = strstr (tmpPtr1, "::")) != NULL) {
            tmpPtr += 2;        /* skip "::" */
            if (sscanf (tmpPtr, "%d-%02d-%02d-%02d.%02d.%02d", &expTm.tm_year,
             &expTm.tm_mon, &expTm.tm_mday, &expTm.tm_hour, &expTm.tm_min,
             &expTm.tm_sec) == 6)
                break;
            tmpPtr1 = tmpPtr;
        }
        if (tmpPtr == NULL) {
            return TRASH_UNKNOWN;
        }
    }
    expTm.tm_year -= 1900;
    expTm.tm_mon --;

    expTime = mktime (&expTm);
    curTime = time (NULL);

    if (curTime - expTime < inpPurgeTime * SEC_PER_HR)
        return TRASH_NOT_EXPIRED;
    else
        return TRASH_EXPIRED;
}

int 
dataChksum (srbConn *conn, mdasC_sql_result_struct *dataResult,
int flagval, int srcCopyNum, char *currentColl)
{
    char *srcPar, *srcCh, *tmpStr;
    char temp[MAX_TOKEN];
    int i, status;
    char chksum[CHKSUM_STR_LEN + 1];

    if (dataResult->result_count == 0)
        return 0;

    srcPar = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    srcCh = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);

    for (i = 0; i < dataResult->row_count;i++) {
        if (srcCopyNum >=  0) {
            sprintf(temp, "%s&COPY=%i", srcCh, srcCopyNum);
        } else {
            strcpy (temp, srcCh);
        }
        if (strlen(inCondition) > 0)
          {
            strcat(temp,"&");
            strcat(temp,inCondition);
          }

	status = srbObjChksum (conn, temp, srcPar, flagval, NULL, chksum);

	if (status < 0) {
	    if (flagval & s_FLAG) {
	    fprintf (stderr, 
	      "dataChksum:phySize %s of %s/%s do not match MCAT size, stat=%d\n",

	      chksum, srcPar, srcCh, status);
	    } else {
	    fprintf (stderr, 
	      "dataChksum: srbObjChksum of %s/%s error, stat=%d\n",
	      srcPar, srcCh, status);
	    }
	    RStatus = status;
            srcPar += MAX_DATA_SIZE;
            srcCh += MAX_DATA_SIZE;
	    status = 0;
	    continue;
	} 

	if (flagval & l_FLAG || flagval & v_FLAG)
	    printChksum (srcCh, srcPar, chksum, currentColl, status);

        srcPar += MAX_DATA_SIZE;
        srcCh += MAX_DATA_SIZE;
    }

    return (status);
}

int 
collChksum (srbConn *conn, mdasC_sql_result_struct *inCollResult,
int flagval, int srcCopyNum, char *currentColl)
{
    int i, status;
    char temp[MAX_TOKEN];
    char *origSrc;

    if (inCollResult->result_count == 0)
        return 0;
    origSrc = (char *) getFromResultStruct(inCollResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    for (i = 0; i < inCollResult->row_count; i++) {
        status = collChksumCont (conn, origSrc, flagval, srcCopyNum,
	 currentColl);

        if (status < 0)
             return ((int) status);
        origSrc += MAX_DATA_SIZE;
    }
    return status;
}

int
collChksumCont (srbConn *conn, char *srcColl, int flagval, int srcCopyNum,
char *currentColl)
{
    mdasC_sql_result_struct  myresult;
    int i, j;
    int status;

    /* copy the dataset first */

    status = queryDataInColl (conn, MDAS_CATALOG, srcColl, flagval, 
     MAX_GET_RESULTS, &myresult);

    if (status < 0 && status != MCAT_INQUIRE_ERROR) {
        srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        return status;
    }

    status = dataChksum (conn, &myresult, flagval, srcCopyNum, currentColl);

    if (status < 0)
        return status;

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        status = srbGetMoreRows(conn, MDAS_CATALOG,
          myresult.continuation_index, &myresult, MAX_GET_RESULTS);
        if (myresult.result_count == 0  || status != 0)
            break;

        status = dataChksum (conn, &myresult, flagval, srcCopyNum, currentColl);
        if (status < 0)
            break;
    }
    clearSqlResult (&myresult);

    /* now copy the collection recursively */

    status = querySubCollInColl (conn, MDAS_CATALOG, srcColl, 0, 
      MAX_GET_RESULTS, &myresult);
    if (status < 0 && status != MCAT_INQUIRE_ERROR) {
        srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
        return status;
    }

    status = collChksum (conn, &myresult, flagval, srcCopyNum, currentColl);

    if (status < 0 && status != MCAT_INQUIRE_ERROR)
         return (status);

    while (myresult.continuation_index >= 0) {
        clearSqlResult (&myresult);
        status = srbGetMoreRows(conn, MDAS_CATALOG,
          myresult.continuation_index, &myresult, MAX_GET_RESULTS);
        if (myresult.result_count == 0  || status != 0)
            break;

        status = collChksum (conn, &myresult, flagval, srcCopyNum, currentColl);

        if (status < 0)
            break;
    }
    clearSqlResult (&myresult);
    return (0);
}

void
printChksum (char *srcCh, char *srcPar, char *chksum, char *currentColl, 
int regFlag)
{
    if (strcmp (srcPar, currentColl) != 0) {
	strcpy (currentColl, srcPar);
	printf ("\n%s\n", currentColl);
    }

    if (regFlag == CHKSUM_REG)
        printf ("    %-24.24s    %-8.8s   new_chksum\n", srcCh, chksum);
    else
        printf ("    %-24.24s    %-8.8s\n", srcCh, chksum);
}
      
int
getClientUserFromConn (srbConn *conn, char **userName, char **domainName)
{
    if (conn == NULL) {
	*userName = NULL;
	*domainName = NULL;
	return -1;
    }
    if (conn->clientUser == NULL) {
        *userName = conn->proxyUser->userName;
        *domainName = conn->proxyUser->domainName;
    } else {
        *userName = conn->clientUser->userName;
        *domainName = conn->clientUser->domainName;
    }
    return (0);
}

int
srbSu (char *userName, char *domainName)
{
    char *tmpStr;
;
    if (userName == NULL || domainName == NULL)
	return -1;

    tmpStr = malloc (128);
    sprintf (tmpStr, "%s=%s", CLIENT_USER_KEYWORD, userName);
    putenv (tmpStr);
    tmpStr = malloc (128);
    sprintf (tmpStr, "%s=%s", CLIENT_DOMAIN_KEYWORD, domainName);
    putenv (tmpStr);

    return (0);
}

int
dirToCollRsync (srbConn *conn, int catType, int flagval,
char *srcDir, char *targColl, char *newResource,
char *newPathName, char *tmpInxDir)
{
    mdasC_sql_result_struct  myresult;
    int i, j;
    int status;
    DIR *dirPtr;
    struct dirent *dp;
    struct stat statbuf;
    char localPath[MAX_TOKEN];
    int targetMode;
    struct srbFileList	*srbFileListHead = NULL;
    struct dirStack *dirStackHead;
    struct srbFileOut mySrcFileOut;
    char newTargColl[MAX_TOKEN];
    

    status = queryDataInColl (conn, catType, targColl, y_FLAG | flagval, 
     RSYNC_ROWS_WANTED, &myresult);

    if (status < 0) {
	if (status != MCAT_INQUIRE_ERROR) {
            srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return status;
	}
    }

    if (myresult.continuation_index >= 0) {
	/* metadata are in files */
	targetMode = 1;
        status = genSrbFileList (conn, catType, flagval, targColl, tmpInxDir,
         &srbFileListHead, &myresult);
    } else {
	/* metadata are in mem */
	targetMode = 0;
    }

    dirStackHead = malloc (sizeof (struct dirStack));
    memset (dirStackHead, 0, sizeof (struct dirStack));
    memset (&mySrcFileOut, 0, sizeof (mySrcFileOut));

    dirStackHead->dirPtr = opendir (srcDir);
    if (dirStackHead->dirPtr == NULL) {
        fprintf (stderr, "The local dir %s does not exist., errno = %d\n",
         srcDir, errno);
	clearSrbFileList (srbFileListHead);
        return -1;
    }

    strcpy (dirStackHead->dirPath, ".");
    strcpy (dirStackHead->baseDir, srcDir);

    while ((status = getNextSrcFile (&dirStackHead, &mySrcFileOut,
     flagval)) >= 0) {
	struct srbFileOut myTargFileOut;

	if (status == 0) {
	    struct dirStack *lastStackHead;
            closedir (dirStackHead->dirPtr);
	    /* pop the stack */
	    lastStackHead = dirStackHead;
	    dirStackHead = dirStackHead->next;
	    if (lastStackHead->fileCnt <= 0) {
		/* empty directory */
		char tmpName[MAX_TOKEN], tmpName1[MAX_TOKEN];

                if (strcmp (lastStackHead->dirPath, ".")  == 0) {
                    strcpy (newTargColl, targColl);
                } else {
                    sprintf (newTargColl, "%s/%s", targColl,
                     lastStackHead->dirPath + 2);
                }
        	clSplitbychar (newTargColl, tmpName, tmpName1, '/');
         	myCreateCollect (conn, catType, tmpName, tmpName1, 0);

	    }
	    free (lastStackHead);
	    if (dirStackHead == NULL)
		break;
	    else 
		continue;
	}

	/* get here when status == 1, we got a file */

	if (targetMode == 0) {
	    status = matchFileOutInMem (&mySrcFileOut, &myTargFileOut,
	     &myresult, targColl);
	} else {
	    status = matchFileOutInFileInx (&mySrcFileOut, &myTargFileOut,
             srbFileListHead, targColl);
	}
	
        sprintf (localPath, "%s/%s/%s", dirStackHead->baseDir,
         mySrcFileOut.fileDir, mySrcFileOut.fileName);

	if (status != 0) {

            if (strcmp (dirStackHead->dirPath, ".")  == 0) {
                strcpy (newTargColl, targColl);
            } else {
                sprintf (newTargColl, "%s/%s", targColl, 
		 mySrcFileOut.fileDir + 2);
            }

	    status = fileToDataRsync (conn, catType, flagval, 
	     &mySrcFileOut, NULL, localPath, targColl, 
	      newTargColl, newResource, -1);
	} else {
            status = fileToDataRsync (conn, catType, flagval,
             &mySrcFileOut, &myTargFileOut, localPath, targColl, 
	     myTargFileOut.collection, newResource, -1);
	}

        if (status < 0) {
            clearSrbFileList (srbFileListHead);
            return (status);
        }
    }
    clearSrbFileList (srbFileListHead);
    return (0);
}

int
clearSrbFileList (struct srbFileList *srbFileListHead)
{
    struct srbFileList *mySrbFileList, *prevSrbFileList;

    mySrbFileList = srbFileListHead;
    while (mySrbFileList != NULL) {
	if (mySrbFileList->listFileName != NULL) {
	    unlink (mySrbFileList->listFileName);
	    free (mySrbFileList->listFileName);
	}
	if (mySrbFileList->endColl != NULL) 
	    free (mySrbFileList->endColl);
	if (mySrbFileList->endFileName != NULL) 
	    free (mySrbFileList->endFileName);
	prevSrbFileList = mySrbFileList;
	mySrbFileList = mySrbFileList->next;
	free (prevSrbFileList);
    }
    return (0);
}

/* getNextSrcFile - get the next file pointed to by dirStackHead.
 * return - 0 = no more files, 1 = a file, -1 = error
 */

int
getNextSrcFile (struct dirStack **dirStackHead, 
struct srbFileOut *mySrcFileOut, int flagval)
{
    struct dirStack *mydirHead;
    struct dirent *dp;
    int status;
    struct stat statbuf;
    char filePath[MAX_TOKEN];

    mydirHead = *dirStackHead;

    while ((dp = readdir (mydirHead->dirPtr)) != NULL) {
        if (strcmp (dp->d_name, ".") == 0 || strcmp (dp->d_name, "..") == 0)
            continue;
	else {
	    mydirHead->fileCnt++;
	    break;
	}
    }

    if (dp == NULL)
	return 0;

    sprintf (filePath, "%s/%s/%s", mydirHead->baseDir, mydirHead->dirPath, 
     dp->d_name);
#ifdef _WIN32
    status = stat_file(filePath, &statbuf);
#else
    status = stat (filePath, &statbuf);
#endif
    if (status != 0) {
        fprintf (stderr, "getNextSrcFile: Unable to stat %s, errno = %d\n", 
	  filePath, errno);
        return -1;
    }

    if ((statbuf.st_mode & S_IFREG) != 0) {     /* A file */
	mySrcFileOut->size = statbuf.st_size;
	strcpy (mySrcFileOut->fileDir, mydirHead->dirPath);
	strcpy (mySrcFileOut->fileName, dp->d_name);
        if (flagval & S_FLAG) { /* chk size only */
            mySrcFileOut->chksum = 0;
	    return (1);
        } else {
            status = clFileSum (filePath, &mySrcFileOut->chksum);
            if (status != 0) {
                fprintf (stderr,
                 "getNextSrcFile: clFileSum error for %s, errno = %d\n",
                filePath, errno);
                return (-1);
            } else {
                return (1);
            }
        }
    } else if ((statbuf.st_mode & S_IFDIR) != 0) {      /* A directory */
	struct dirStack *tmpDirStack;

	tmpDirStack = malloc (sizeof (struct dirStack));
        memset (tmpDirStack, 0, sizeof (struct dirStack));
	sprintf (tmpDirStack->dirPath, "%s/%s", mydirHead->dirPath, dp->d_name);
	sprintf (tmpDirStack->baseDir, mydirHead->baseDir);
	tmpDirStack->dirPtr = opendir (filePath);
        if (tmpDirStack->dirPtr == NULL) {
            fprintf (stderr, 
	     "getNextSrcFile: opendir of %s error, errno = %d\n",
             tmpDirStack->dirPath, errno);
            return -1;
        }
	/* push it on top of stack */
	tmpDirStack->next = mydirHead;
	*dirStackHead = tmpDirStack;
	/* recursively drill down the directory */
	status = getNextSrcFile (dirStackHead, mySrcFileOut, flagval);
	return (status);
    } else {
	fprintf (stderr, "getNextSrcFile: Unknow type for %s\n",
         filePath);
	return (-1);
    }
}

int
genSrbFileList (srbConn *conn, int catType, int flagval, char *targColl, 
char *tmpInxDir, struct srbFileList  **srbFileListHead, 
mdasC_sql_result_struct *myresult)
{
    char listFileName[MAX_TOKEN], fileName[MAX_TOKEN], myDir[MAX_TOKEN];
    int fileNum = 0;
    FILE *inxFd;
    struct srbFileList *mySrbFileList;
    int i, status, numFiles; 
    int targCollLen;
    int continuation_index;
    char fileDir[MAX_TOKEN];;

    if (srbFileListHead == NULL) {
	fprintf (stderr, 
	 "genSrbFileList: Internal problem. srbFileListHead == NULL\n");
	return (-1);
    }

    targCollLen = strlen (targColl);
    *srbFileListHead  = NULL;
    if (tmpInxDir != NULL && strlen (tmpInxDir) > 0) {
	strcpy (myDir, tmpInxDir);
    } else {
	myDir[0] = '\0';
    }

    do {
        char *dataName, *collection, *size, *offset, *chksum, *replNum, 
	 *isDirty;
        char *myDataName, *myCollection, *myDirty;

        sprintf (fileName, 
	 "srbList%-d.%-d", fileNum, time(0) + (getpid() << 4));
        if (strlen (myDir) > 0) {
	    sprintf (listFileName, "%s/%s", myDir, fileName);
	    inxFd = fopen (listFileName, "w");
        } else {
	    inxFd = NULL;
        }
    
 	if (inxFd == NULL) {
            /* try the cwd */
	    strcpy (myDir, "."); 
	    sprintf (listFileName, "%s/%s", myDir, fileName);
	    if ((inxFd = fopen (fileName, "w")) == NULL) {
		/* try /tmp */
	        strcpy (myDir, "/tmp"); 
	        sprintf (listFileName, "%s/%s", myDir, fileName);
        	if ((inxFd = fopen (listFileName, "w")) == NULL) {
		    /* try home */
	            strcpy (myDir, "~"); 
	            sprintf (listFileName, "%s/%s", myDir, fileName);
        	    if ((inxFd = fopen (listFileName, "w")) == NULL) {
			fprintf (stderr,
			 "genSrbFileList: fopen error in dir %s, errno = %d\n",
			  fileName, errno);
			return (-1);
		    }
		}
	    }
	}
	if (fileNum > 0) {
            status = srbGetMoreRows(conn, catType,
             continuation_index, myresult, RSYNC_ROWS_WANTED);
    	    if (status < 0) {
                if (status != MCAT_INQUIRE_ERROR)
                    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
		fclose (inxFd);
                return status;
            } 
	    filterDuplicate (myresult);
	}
	fileNum ++;

        dataName = (char *) getFromResultStruct(myresult,
          dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
        collection = (char *) getFromResultStruct(myresult,
          dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
        size = (char *) getFromResultStruct(myresult,
          dcs_tname[SIZE], dcs_aname[SIZE]);
        chksum = (char *) getFromResultStruct(myresult,
          dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
        replNum = (char *) getFromResultStruct(myresult,
          dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
        isDirty = (char *) getFromResultStruct(myresult,
          dcs_tname[IS_DIRTY], dcs_aname[IS_DIRTY]);

	/* save this for later use */
	myDataName = dataName;	
	myDirty = isDirty;
	myCollection = NULL;

	for (i = 0; i < myresult->row_count; i++) {

	    if (strcmp (collection, targColl) == 0) {
		strcpy (fileDir,  ".");	   /* have to put something there */
	    } else {
		sprintf (fileDir, "./%s", collection + targCollLen + 1);
	    }
	    if (i == 0) {	/* save the first fileDir */
	        myCollection = strdup (fileDir);
	    }
            fprintf (inxFd, "%s %s %s %s %s %d %s\n",
             fileDir, collection, dataName, size, replNum, atoi (chksum),
	      isDirty);

	    if (i == myresult->row_count - 1)	/* don't increment the last */
		continue;
            dataName += MAX_DATA_SIZE;
            collection += MAX_DATA_SIZE;
            size += MAX_DATA_SIZE;
            chksum += MAX_DATA_SIZE;
            replNum += MAX_DATA_SIZE;
            isDirty += MAX_DATA_SIZE;
	}
	fclose (inxFd);
	mySrbFileList = malloc (sizeof (struct srbFileList));
	mySrbFileList->numFiles = myresult->row_count;
	mySrbFileList->dupInx1 = -1;	/* no duplicated inx */
	mySrbFileList->dupInx2 = -1;	/* no duplicated inx */
	mySrbFileList->endColl = strdup (fileDir);
	mySrbFileList->endFileName = strdup (dataName);
	mySrbFileList->listFileName = strdup (listFileName);
	mySrbFileList->next = NULL;
	/* queue it */

	if (*srbFileListHead == NULL) {
	    *srbFileListHead = mySrbFileList;
	} else {
	    struct srbFileList *tmpSrbFileList;

	    tmpSrbFileList = *srbFileListHead;

	    while (tmpSrbFileList != NULL) {
		if (tmpSrbFileList->next == NULL) {
		    tmpSrbFileList->next = mySrbFileList;
		    /* check for duplication */
		    if (strcmp (tmpSrbFileList->endColl, myCollection) == 0 &&
		     strcmp (tmpSrbFileList->endFileName, myDataName) == 0) {
			/* we have a duplicate here */
			if (atoi (myDirty) > 0) {
			    /* invalidate the last inx of previous copy */
			    if (tmpSrbFileList->dupInx1 < 0) {
				tmpSrbFileList->dupInx1 = 
				 tmpSrbFileList->numFiles - 1;
			    } else {
                                tmpSrbFileList->dupInx2 = 
                                 tmpSrbFileList->numFiles - 1;
			    }
			} else {
			    /* invalidate the first inx of current copy */
			    mySrbFileList->dupInx1 = 0;
			}
		    }
		    break;
		}
		tmpSrbFileList = tmpSrbFileList->next;
	    }
	}
	if (myCollection != NULL)
	    free (myCollection);

	continuation_index = myresult->continuation_index;
	clearSqlResult (myresult);	
    } while (continuation_index >= 0);

    return (0);
}

int
matchFileOutInMem (struct srbFileOut *inFile, struct srbFileOut *outFile,
mdasC_sql_result_struct *myresult, char *targColl)
{
    char *dataName, *collection, *size, *offset, *chksum, *replNum;
    int targCollLen;
    int status, i;

    dataName = (char *) getFromResultStruct(myresult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    collection = (char *) getFromResultStruct(myresult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    size = (char *) getFromResultStruct(myresult,
      dcs_tname[SIZE], dcs_aname[SIZE]);
    chksum = (char *) getFromResultStruct(myresult,
      dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
    replNum = (char *) getFromResultStruct(myresult,
      dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);

    targCollLen = strlen (targColl);
    for (i = 0; i < myresult->row_count; i++) {
	char fileDir[MAX_TOKEN];

        if (strcmp (collection, targColl) == 0) {
            strcpy (fileDir,  ".");    /* have to put something there */
        } else {
            sprintf (fileDir, "./%s", collection + targCollLen + 1);
        }

	status = strcmp (inFile->fileDir, fileDir);
	if (status < 0) {
	    return (-1);	/* no match */
	} else if (status == 0) {
	    status = strcmp (inFile->fileName, dataName);
	    if (status < 0) {
                return (-1);    /* no match */
	    } else if (status == 0) {
		/* we have a match */
		strcpy (outFile->fileDir, fileDir);
		strcpy (outFile->fileName, dataName);
		strcpy (outFile->collection, collection);
		outFile->replNum = atoi (replNum);
		outFile->chksum = atoi (chksum);
		outFile->size = strtoll (size, 0, 0);
		return (0);
	    }
	}
        dataName += MAX_DATA_SIZE;
        collection += MAX_DATA_SIZE;
        size += MAX_DATA_SIZE;
        chksum += MAX_DATA_SIZE;
        replNum += MAX_DATA_SIZE;
    }
    return (-1);
}

int
createCollectRecur (srbConn *conn, int catType, char *startColl, 
char *collection, int inheritCont)
{
    int status;
    int startLen;
    int pathLen, tmpLen;
    char tmpPath[MAX_TOKEN];

    startLen = strlen (startColl);
    pathLen = strlen (collection);

    strcpy (tmpPath, collection);

    tmpLen = pathLen;

    while (tmpLen > startLen) {
	if (checkCollection (conn, catType, tmpPath) == 0) 
	    break;

        /* Go backward */

        while (tmpLen && tmpPath[tmpLen] != '/')
            tmpLen --;
        tmpPath[tmpLen] = '\0';
    }

    /* Now we go forward and mk the required dir */
    while (tmpLen < pathLen) {
        status = myCreateCollect (conn, catType, tmpPath, 
	 &tmpPath[tmpLen + 1], inheritCont);
        /* Put back the '/' */
        tmpPath[tmpLen] = '/';
        if (status < 0) {
            fprintf (stderr,
	     "createCollectRecur: srbCreateCollect failed for %s, status =%d\n",
              tmpPath);
            return status;
        }
        while (tmpLen && tmpPath[tmpLen] != '\0')
            tmpLen ++;
    }
    return 0;
}

int
createDirRecur (char *startDir, char *directory)
{
    int status;
    int startLen;
    int pathLen, tmpLen;
    char tmpPath[MAX_TOKEN];
    struct stat statbuf;

    startLen = strlen (startDir);
    pathLen = strlen (directory);

    strcpy (tmpPath, directory);

    tmpLen = pathLen;

    while (tmpLen > startLen) {
#ifdef _WIN32
        status = stat_file(tmpPath, &statbuf);
#else
        status = stat (tmpPath, &statbuf);
#endif
	if (status == 0) {
	    if (statbuf.st_mode & S_IFDIR) {
		break;
	    } else {
		fprintf(stderr,
                 "createDirRecur: A local non-directory %s already exists \n",
                  tmpPath);
                return (UTIL_ERROR_TARGET_ALREADY_EXIST);
            }
	}

        /* Go backward */

        while (tmpLen && tmpPath[tmpLen] != '/')
            tmpLen --;
        tmpPath[tmpLen] = '\0';
    }

    /* Now we go forward and mk the required dir */
    while (tmpLen < pathLen) {
        /* Put back the '/' */
        tmpPath[tmpLen] = '/';
#ifdef _WIN32
       status = mk_dir(tmpPath, 0755);
#else
       status = mkdir(tmpPath, 0755);
#endif

        if (status < 0) {
            fprintf (stderr,
	     "createDirRecur: mkdir failed for %s, errno =%d\n",
              tmpPath, errno);
            return -1;
        }
        while (tmpLen && tmpPath[tmpLen] != '\0')
            tmpLen ++;
    }
    return 0;
}

int matchFileOutInFileInx (struct srbFileOut *inFile, 
struct srbFileOut *outFile, struct srbFileList  *srbFileListHead, 
char *targColl)
{
    struct srbFileList *mySrbFileList;
    FILE *inFd;
    int i, status;
    char fileDir[MAX_TOKEN], dataName[MAX_TOKEN], collection[MAX_TOKEN], 
    size[MAX_TOKEN];
    int chksum, replNum, isDirty;
    int targCollLen;

    targCollLen = strlen (targColl);
    mySrbFileList = srbFileListHead;
    
    while (mySrbFileList != NULL) {
	if ((status = strcmp (inFile->fileDir, mySrbFileList->endColl)) > 0) {
	    mySrbFileList = mySrbFileList->next;
	    continue;
	} else if (status == 0) {	/* a match */
	    if ((status = strcmp (inFile->fileName, 
	     mySrbFileList->endFileName)) > 0) {
	        mySrbFileList = mySrbFileList->next;
                continue;
	    } else if (status == 0 && 
	     (mySrbFileList->dupInx1 == mySrbFileList->numFiles -1 ||
	      mySrbFileList->dupInx2 == mySrbFileList->numFiles -1)) {
	        /* check for duplicate */
	        mySrbFileList = mySrbFileList->next;
		continue;
	    }
	}

	inFd = fopen (mySrbFileList->listFileName, "r");
        if (inFd == NULL) {
            fprintf(stderr,
             "matchFileOutInFileInx: open local index file %s err,errno=%d\n",
              mySrbFileList->listFileName, errno);
            return (-1);
        }

        for (i = 0; i < mySrbFileList->numFiles; i++) {
            char fileDir[MAX_TOKEN];

	    status = fscanf (inFd, "%s %s %s %s %d %d %d\n", 
	     fileDir, collection, dataName, size, &replNum, &chksum, &isDirty);
	    if (status != 7) {
                fprintf(stderr,
                 "matchFileOutInFileInx: fscanf of %s. Got %d of 7. ",
                  mySrbFileList->listFileName, status);
                fprintf(stderr,
		 "fileName = %s, collection = %s\n", dataName, collection);
		continue;
	    }
	    if (mySrbFileList->dupInx1 == i || mySrbFileList->dupInx2 == i)
                continue;

            status = strcmp (inFile->fileDir, fileDir);
            if (status < 0) {
		fclose (inFd);
                return (-1);        /* no match */
            } else if (status == 0) {
                status = strcmp (inFile->fileName, dataName);
                if (status < 0) {
		    fclose (inFd);
                    return (-1);    /* no match */
                } else if (status == 0) {
                    /* we have a match */
                    strcpy (outFile->fileDir, fileDir);
                    strcpy (outFile->fileName, dataName);
                    strcpy (outFile->collection, collection);
                    outFile->replNum = replNum;
                    outFile->chksum = chksum;
                    outFile->size = strtoll (size, 0, 0);
		    fclose (inFd);
                    return (0);
		}
            }
        }
	fclose (inFd);
	return (-1);
    }
    return (-1);
}

int
fileToDataRsync (srbConn *conn, int catType, int flagval,
struct srbFileOut *mySrcFileOut, struct srbFileOut *myTargFileOut,
char *localPath, char *origTargColl, char *targColl, char *newResource, 
int copyNum)
{
    char tmpObjName[MAX_TOKEN];
    int status;

    if (!(flagval & S_FLAG) && !(flagval & l_FLAG) && 
     mySrcFileOut->chksum == 0) {
	clFileSum (localPath, &mySrcFileOut->chksum);
    }
    sprintf (tmpObjName, "%s&REG_CHKSUM=%d", mySrcFileOut->fileName,
     mySrcFileOut->chksum);

    if (myTargFileOut == NULL) {	/* create a new dataset */
        /* no match */
        if (flagval & l_FLAG || flagval & v_FLAG) {
            printf ("%s    %lld  %d    N\n",
             localPath,
             mySrcFileOut->size, mySrcFileOut->chksum);
	    if (flagval & l_FLAG)
                return (0);
        }
        /* upload the file */

        status = localToSrbFileCopy (conn, catType, localPath,
         tmpObjName, targColl, mySrcFileOut->size, flagval | Y_FLAG,
         0, "", "", newResource, -1);
        if (status < 0) {
            if (status == COLLECTION_NOT_IN_CAT ||
             status == DATA_NOT_IN_COLLECTION) {
                createCollectRecur (conn, catType, origTargColl, targColl, 1);
                /* try again */
                status = localToSrbFileCopy (conn, catType, localPath,
                 tmpObjName, targColl, mySrcFileOut->size, flagval | Y_FLAG,
                  0, "", "", newResource, -1);
            }
            if (status < 0) {
                fprintf (stderr, "localToSrbFileCopy of %s failed\n",
                 localPath);
                srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                return (status);
            }
        }
    } else {
        /* we have a name match */
        if (myTargFileOut->chksum == 0 &&
          myTargFileOut->size == mySrcFileOut->size &&
           !(flagval & S_FLAG)) {
            /* no chksum */
            char chksum[CHKSUM_STR_LEN + 1];
            status = srbObjChksum (conn, myTargFileOut->fileName,
             myTargFileOut->collection, 0, NULL, chksum);
            if (status < 0) {
                fprintf (stderr, "srbObjChksum of %s/%s failed\n",
                 myTargFileOut->collection, myTargFileOut->fileName);
                srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                return (status);
            }
            myTargFileOut->chksum = atoi (chksum);
        } else if (flagval & S_FLAG) {
            myTargFileOut->chksum = 0;
        }

        if ((myTargFileOut->chksum == mySrcFileOut->chksum 
	 || (flagval & S_FLAG)) &&
         myTargFileOut->size == mySrcFileOut->size) {
            /* a match */
            return (0);
        }
        if (flagval & l_FLAG || flagval & v_FLAG) {
            printf ("%s    %lld  %d    \n",
             localPath,
             mySrcFileOut->size, mySrcFileOut->chksum);
	    if (flagval & l_FLAG)
                return (0);
        }
        /* overwrite existing file */
        status = localToSrbFileCopy (conn, catType, localPath,
         tmpObjName, myTargFileOut->collection,
          mySrcFileOut->size, flagval | F_FLAG | Y_FLAG,
         1, "", "", newResource, -1);
        if (status < 0) {
            fprintf (stderr, "localToSrbFileCopy of %s failed\n",
             localPath);
            srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return (status);
        }
    }
}

int
fileToCollRsync (srbConn *conn, int catType, int flagval,
struct srbFileOut *mySrcFileOut, char *targColl, char *localPath, 
char *resourceName, int copyNum)
{
    mdasC_sql_result_struct dataResult;
    char myPath[MAX_TOKEN];
    char tmpStr1[MAX_TOKEN];
    char tmpStr2[MAX_TOKEN];
    int status;

    clSplitbychar(localPath, tmpStr1, tmpStr2,'/');
    strcpy (mySrcFileOut->fileName, tmpStr2);

    sprintf (myPath, "%s/%s", targColl, mySrcFileOut->fileName);
    status = queryData (conn, catType, myPath, Y_FLAG, MAX_GET_RESULTS, &dataResult);
    if (status < 0) {
        if (status != MCAT_INQUIRE_ERROR) {
            srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return status;
	}
        status = fileToDataRsync (conn, catType, flagval,
          mySrcFileOut, NULL, localPath,
          targColl, targColl, resourceName, -1);
    } else {	/* the file exist */
	struct srbFileOut myTargFileOut;
        char *mySize, *chksum, *collection, *dataName;

        memset (&myTargFileOut, 0, sizeof (myTargFileOut));
        strcpy (myTargFileOut.fileName, tmpStr2);
        collection = (char *) getFromResultStruct (&dataResult,
          dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
        dataName = (char *) getFromResultStruct (&dataResult,
          dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
        mySize = (char *) getFromResultStruct (&dataResult,
          dcs_tname[SIZE], dcs_aname[SIZE]);
        chksum = (char *) getFromResultStruct (&dataResult,
          dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
        strcpy (myTargFileOut.fileName, dataName);
        strcpy (myTargFileOut.collection, collection);
        myTargFileOut.size = strtoll (mySize, 0, 0);
        myTargFileOut.chksum = atoi (chksum);
        status = fileToDataRsync (conn, catType, flagval,
          mySrcFileOut, &myTargFileOut, localPath,
          collection, collection, resourceName, copyNum);
    } 
    return (status);
}

int
dataToDirRsync (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, char *targDir)
{
    char *dataName, *collection, *size, *offset, *chksum, *replNum;
    char targFile[MAX_TOKEN];
    int status;

    if (dataResult->result_count == 0)
        return 0;

    dataName = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    collection = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    size = (char *) getFromResultStruct(dataResult,
      dcs_tname[SIZE], dcs_aname[SIZE]);
    chksum = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
    replNum = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);

    for (i = 0; i < dataResult->row_count; i++) {
	sprintf (targFile, "%s/%s", targDir, dataName);
        status = dataToFileRsync1 (conn, catType, flagval, dataName,
         collection, size, offset, chksum, targFile);
        if (status < 0)
            break;
        dataName += MAX_DATA_SIZE;
        collection += MAX_DATA_SIZE;
        size += MAX_DATA_SIZE;
        chksum += MAX_DATA_SIZE;
        replNum += MAX_DATA_SIZE;
    }
    return (status);
}

int
collToDirRsync (srbConn *conn, int catType, int flagval,
int inColltype, int collCnt, mdasC_sql_result_struct *inCollResult, char *targDir)
{
    int i, j, status;
    int in_fd, out_fd;
    srb_long_t datasize;
    char *datatype;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN], tmpTarg[MAX_TOKEN];
    char *origSrc, *curBaseTarg;
    struct stat statbuf;
    mdasC_sql_result_struct myresult;
    char lastColl[MAX_TOKEN], lastData[MAX_TOKEN];
    int lastDirty = -1;

    if (inCollResult->result_count == 0)
        return 0;
    origSrc = (char *) getFromResultStruct(inCollResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    for (i = 0; i < inCollResult->row_count; i++) {
	char *dataName, *collection, *size, *offset, *chksum, *replNum, 
	 *isDirty;
	int origSrcLen;

	if (collCnt <= 1) {	/* only src coll */
            /* Make the dir */
            curBaseTarg = targDir;
        } else {	/* more than one src */
            clSplitbychar(origSrc, temp, temp1,'/');
            sprintf (tmpTarg, "%s/%s", targDir, temp1);
            curBaseTarg = tmpTarg;
        }

#ifdef _WIN32
        status = stat_file(curBaseTarg, &statbuf);
#else
        status = stat (curBaseTarg, &statbuf);
#endif

        if (status < 0) {   /* does not exist */
#ifdef _WIN32
            status = mk_dir(curBaseTarg, 0755);
#else
            status = mkdir(curBaseTarg, 0755);
#endif
            if (status < 0) {
                fprintf(stderr,
                 "Unable to mk the target dir %s, status = %d\n",
                  curBaseTarg, status);
                return (CLI_ERR_COLLNAME);
            }
        } else if ((statbuf.st_mode & S_IFDIR) == 0) {  /* Not a directory */
            fprintf(stderr,
             "collToDir: A local non-directory %s already exists \n",
              curBaseTarg);
            return (UTIL_ERROR_TARGET_ALREADY_EXIST);
	}

	origSrcLen = strlen (origSrc);

        status = queryDataInColl (conn, catType, origSrc, y_FLAG | flagval,
         RSYNC_ROWS_WANTED, &myresult);

        if (status < 0) {
            if (status != MCAT_INQUIRE_ERROR)
                srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    continue;
        }

	do {
	    int startInx;

            dataName = (char *) getFromResultStruct(&myresult,
              dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
            collection = (char *) getFromResultStruct(&myresult,
              dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
            size = (char *) getFromResultStruct(&myresult,
              dcs_tname[SIZE], dcs_aname[SIZE]);
            chksum = (char *) getFromResultStruct(&myresult,
              dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
            replNum = (char *) getFromResultStruct(&myresult,
              dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
            isDirty = (char *) getFromResultStruct(&myresult,
              dcs_tname[IS_DIRTY], dcs_aname[IS_DIRTY]);

	    if (lastDirty < 0) {
		startInx = 0;
	    } else if (strcmp (lastColl, collection) != 0 ||
	     strcmp (lastData, dataName) != 0) {
		startInx = 0;
	    } else {	/* a match */
		int thisDirty;

		if (lastDirty > 0) {	/* a good copy already */
		    startInx = 1;
		} else {
		    thisDirty = atoi (isDirty);
		    if (thisDirty > 0) {
			startInx = 0;
		    } else {	/* both are not dirty */
			startInx = 1;
		    }
		}
	    }
            for (j = startInx; j < myresult.row_count; j++) {
                int iChksum, localChksum;
	        char myTarg[MAX_TOKEN];
	        srb_long_t datasize;
	        srb_long_t lstatus;

	        if (strlen (collection) == origSrcLen) {
		    sprintf (myTarg, "%s/%s", curBaseTarg, dataName);
	        } else {
		    sprintf (myTarg, "%s/%s", curBaseTarg, 
		     collection + origSrcLen + 1); 
                    status = createDirRecur (curBaseTarg, myTarg);
                    if (status < 0) {
                        fprintf(stderr,
                         "Unable to mk the target dir %s, status = %d\n",
                          myTarg, status);
                        return (CLI_ERR_COLLNAME);
                    }
		    strcat (myTarg, "/");
		    strcat (myTarg, dataName);
	        }

	        status = dataToFileRsync1 (conn, catType, flagval, 
	         dataName, collection, size, offset, chksum, myTarg);

	        if (status < 0)
		    return (status);

                dataName += MAX_DATA_SIZE;
                collection += MAX_DATA_SIZE;
                isDirty += MAX_DATA_SIZE;
                size += MAX_DATA_SIZE;
                chksum += MAX_DATA_SIZE;
                replNum += MAX_DATA_SIZE;
	   }
           if (myresult.continuation_index >= 0 &&
              myresult.result_count > 0) {
		/* save the last dataName, collection and isDirty */
		dataName -= MAX_DATA_SIZE;
                collection -= MAX_DATA_SIZE;
                isDirty -= MAX_DATA_SIZE;
		strcpy (lastColl, collection);
		strcpy (lastData, dataName);
		lastDirty = atoi (isDirty);
                clearSqlResult (&myresult);
                status = srbGetMoreRows(conn, catType,
                 myresult.continuation_index, &myresult, MAX_GET_RESULTS);
                if (status == 0)
                    filterDuplicate (&myresult);
            } else
                clearSqlResult (&myresult);

	} while (myresult.result_count > 0 && status >= 0);
        origSrc += MAX_DATA_SIZE;
    }
    return 0;
}

int
dataToFileRsync (srbConn *conn, int catType, int flagval, int targType,
mdasC_sql_result_struct *dataResult, char *targFile)
{
    char *dataName, *collection, *size, *offset, *chksum, *replNum;
    int status;

    if (dataResult->result_count == 0)
        return 0;

    dataName = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    collection = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    size = (char *) getFromResultStruct(dataResult,
      dcs_tname[SIZE], dcs_aname[SIZE]);
    chksum = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
    replNum = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);

    for (i = 0; i < dataResult->row_count; i++) {
        status = dataToFileRsync1 (conn, catType, flagval, dataName,
         collection, size, offset, chksum, targFile);
	if (status < 0)
	    break;
        dataName += MAX_DATA_SIZE;
        collection += MAX_DATA_SIZE;
        size += MAX_DATA_SIZE;
        chksum += MAX_DATA_SIZE;
        replNum += MAX_DATA_SIZE;
    }
    return (status);
}

int
dataToFileRsync1 (srbConn *conn, int catType, int flagval, 
char *dataName, char *collection, char *size, char *offset, 
char *chksum, char *myTarg)
{
    int iChksum, localChksum;
    srb_long_t datasize;
    srb_long_t lstatus;
    struct stat statbuf;
    int status;

    if (flagval & S_FLAG) {
	iChksum = 0;
    } else {
        iChksum = atoi (chksum);
    }

    if (!(flagval & S_FLAG) && !(flagval & l_FLAG) && iChksum <= 0) {
        status = srbObjChksum (conn, dataName, collection, 0, NULL, chksum);
        if (status < 0) {
            fprintf (stderr, "srbObjChksum of %s/%s failed\n",
             collection, dataName);
            srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            return (status);
        }
        iChksum = atoi (chksum);
    }
    datasize = strtoll (size, 0, 0);

#ifdef _WIN32
    status = stat_file (myTarg, &statbuf);
#else
    status = stat (myTarg, &statbuf);
#endif
    if (status < 0) {   /* does not exist */
        if (flagval & l_FLAG || flagval & v_FLAG) {
            printf ("%s    %lld  %d    N\n",
             myTarg, datasize, iChksum);
        }
        if ((flagval & l_FLAG) == 0) {
            lstatus = srbToLocalFileCopy1 (conn, myTarg,
             dataName, collection, datasize, flagval);
        } else {
            lstatus = 0;
        }
    } else {
        if (datasize == statbuf.st_size && !(flagval & S_FLAG)) {
            if (clFileSum (myTarg, (unsigned long *) &localChksum) < 0) {
                fprintf (stderr,
                 "dataToFileRsync1: clFileSum failed for %s\n", myTarg);
                return -1;
            }
        }  else if (flagval & S_FLAG) {
            localChksum = 0;
        }

        if (datasize != statbuf.st_size || (localChksum != iChksum &&
	 !(flagval & S_FLAG))) {
            if (flagval & l_FLAG || flagval & v_FLAG) {
                printf ("%s    %lld  %d\n",
                 myTarg, datasize, iChksum);
            }
            if ((flagval & l_FLAG) == 0) {
                lstatus = srbToLocalFileCopy1 (conn, myTarg,
                 dataName, collection, datasize, F_FLAG | flagval);
                if (lstatus < 0) {
                    status = lstatus;
                    fprintf (stderr,
                     "srbToLocalFileCopy1 of %s/%s error, status = %d\n",
                     collection, dataName, status);
                    return (status);
		}
            }
	}
    }
    return (0);
}

int
checkTrashColl (char *collection)
{
    char *tmpPtr;

    tmpPtr = collection;

    if (*tmpPtr != '/')
        return -1;

    tmpPtr++;
    if (strcmp ("trash", tmpPtr) == 0) {
	return (IS_TRASH_ROOT);
    } else if (strncmp ("trash/", tmpPtr, 6) == 0) {
        tmpPtr += 6;
    } else {
        /* skip the first entry in the path - myZone */
        while (*tmpPtr != '\0') {
            if (*tmpPtr == '/') {
                tmpPtr++;
                break;
            }
            tmpPtr++;
        }
        if (*tmpPtr == '\0')
            return (NOT_IN_TRASH);

	if (strcmp ("trash", tmpPtr) == 0) {
            return (IS_TRASH_ROOT);
        } else if (strncmp ("trash/", tmpPtr, 6) == 0) {
            tmpPtr += 6;
        } else {
            return (NOT_IN_TRASH);
	}
    }

    /* skip the next two entries: home/user.domain */

    while (*tmpPtr != '\0') {
        if (*tmpPtr == '/') {
            tmpPtr++;
            break;
        }
        tmpPtr++;
    }

    if (*tmpPtr == '\0')
        return (IS_TRASH_ROOT);

    while (*tmpPtr != '\0') {
        if (*tmpPtr == '/') {
            tmpPtr++;
            break;
        }
        tmpPtr++;
    }

    if (*tmpPtr == '\0') {
        return (IS_TRASH_ROOT);
    } else {
        return (IS_IN_TRASH);
    }
}

int
myCreateCollect (srbConn *conn, int catType, char *parcollname, 
char *collname, int inheritCont)
{
    int status;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    mdasC_sql_result_struct  myresult;
    char *contPtr;
    char temp1[MAX_TOKEN];


    status = srbCreateCollect(conn, catType, parcollname, collname);
    if (status < 0) {
        fprintf (stderr,"myCreateCollect: Error for collection %s/%s\n", 
	 parcollname, collname);
        srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	return (status);
    }

    if (inheritCont == 0) {
	return 0;
    }

    for (i = 0; i < MAX_DCS_NUM; i++) {
          sprintf(qval[i],"");
          selval[i] = 0;
    }
    selval[CONTAINER_FOR_COLLECTION] = 1;
    sprintf(qval[COLLECTION_NAME]," = '%s'",parcollname);
    status = srbGetDataDirInfo(conn, MDAS_CATALOG, qval, selval, &myresult,40);
    if (status < 0) {
	if (status == MCAT_INQUIRE_ERROR) {
	    return (0);
	} else {
            fprintf(stderr,"myCreateCollect: Query Error for %s\n", 
	     parcollname);
            srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	    return (status);
	}
    } 
    sprintf (temp1, "%s/%s", parcollname, collname);
    contPtr = (char *) getFromResultStruct
      ((mdasC_sql_result_struct *) &myresult,
       dcs_tname[CONTAINER_FOR_COLLECTION],
       dcs_aname[CONTAINER_FOR_COLLECTION]);
    status = srbModifyCollect(conn, MDAS_CATALOG,temp1,
                             contPtr,"","",
                             C_INSERT_CONTAINER_FOR_COLLECTION);
    if (status < 0)  {
       fprintf(stderr,"myCreateCollect: srbModifyCollect error for %s\n",
         temp1);
        srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	return status;
    }
    return (0);
}

int
dataToDataRsync (srbConn *conn, int catType, int flagval, int targType,
mdasC_sql_result_struct *dataResult, char *targPath,
int copyNum, char *resourceName, char *cTargSize)
{
    char *dataName, *collection, *size, *offset, *chksum, *replNum;
    int status;
    int i;

    if (dataResult->result_count == 0)
        return 0;

    dataName = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    collection = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    size = (char *) getFromResultStruct(dataResult,
      dcs_tname[SIZE], dcs_aname[SIZE]);
    chksum = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
    replNum = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);

    for (i = 0; i < dataResult->row_count; i++) {
        status = dataToDataRsync1 (conn, catType, flagval, dataName,
         collection, size, offset, chksum, targPath, copyNum, resourceName,
         targType, cTargSize, NULL);
        if (status < 0)
            break;
        dataName += MAX_DATA_SIZE;
        collection += MAX_DATA_SIZE;
        size += MAX_DATA_SIZE;
        chksum += MAX_DATA_SIZE;
        replNum += MAX_DATA_SIZE;
    }
    return (status);
}

int
dataToDataRsync1 (srbConn *conn, int catType, int flagval,
char *dataName, char *collection, char *size, char *offset,
char *chksum, char *myTarg, int copyNum, char *resourceName, int targType,
char *cTargSize, char *cTargChksum)
{
    int srcChksum, targChksum;
    srb_long_t datasize, targSize;
    srb_long_t lstatus;
    struct stat statbuf;
    int status;
    char tmpChksum[CHKSUM_STR_LEN + 1];
    char targCh[MAX_TOKEN], targPar[MAX_TOKEN];

    if (cTargSize == NULL) {
        targSize = -1;
    } else {
        targSize = strtoll (cTargSize, 0, 0);
    }
    datasize = strtoll (size, 0, 0);

    splitpath (myTarg, targPar, targCh);
    if (!(flagval & S_FLAG)) {
        if (cTargChksum == NULL) {
            targChksum = -1;
        } else {
            targChksum = atoi (cTargChksum);
            if (targChksum == 0) {
                status = srbObjChksum (conn, targCh, targPar, 0, NULL,
                 tmpChksum);
                if (status < 0) {
                    fprintf (stderr, "srbObjChksum of %s/%s failed\n",
                     collection, dataName);
                    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                    return (status);
                }
                targChksum = atoi (tmpChksum);
            }
        }

        srcChksum = atoi (chksum);
        if (srcChksum <= 0) {
            status = srbObjChksum (conn, dataName, collection, 0, NULL, 
	     tmpChksum);
            if (status < 0) {
                fprintf (stderr, "srbObjChksum of %s/%s failed\n",
                 collection, dataName);
                srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                return (status);
            }
            srcChksum = atoi (tmpChksum);
        }
    }

    if (targType < 0) {   /* does not exist */
        if (flagval & l_FLAG || flagval & v_FLAG) {
            printf ("%s    %lld  %d    N\n",
             myTarg, datasize, srcChksum);
        }
        if ((flagval & l_FLAG) == 0) {
            lstatus = dataToDataCopy1 (conn, catType, flagval, targType,
             dataName, collection, datasize, copyNum, targPar, targCh,
             resourceName, "", "");
        } else {
            lstatus = 0;
        }
    } else if (targType >= DATANAME_T) {        /* target Exists */
	if (!(flagval & S_FLAG)) {
            if (targChksum < 0) {   /* no chksum */
                status = srbObjChksum (conn, targCh, targPar, 0, NULL,
                 tmpChksum);
                if (status < 0) {
                    fprintf (stderr, "srbObjChksum of %s/%s failed\n",
                     targPar, targCh);
                    srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                    return (status);
                }
                targChksum = atoi (tmpChksum);
	    }
        }
        flagval |= F_FLAG;

        if ((!(flagval & S_FLAG) && targChksum != srcChksum) ||
         (targSize >= 0 && targSize != datasize)) {
            if (flagval & l_FLAG || flagval & v_FLAG) {
                printf ("%s    %lld  %d\n",
                 myTarg, datasize, srcChksum);
            }
            if ((flagval & l_FLAG) == 0) {
                lstatus = dataToDataCopy1 (conn, catType, flagval, targType,
                 dataName, collection, datasize, copyNum, targPar, targCh,
                 resourceName, "", "");
                if (lstatus < 0) {
                    status = lstatus;
                    fprintf (stderr,
                     "srbToSrbFileCopy1 of %s/%s error, status = %d\n",
                     collection, dataName, status);
                    return (status);
                }
            }
        }
    }
    return (0);
}

int
dataToCollRsync (srbConn *conn, int catType, int flagval,
mdasC_sql_result_struct *dataResult, char *targColl,
char *resourceName, int copyNum)
{
    char myPath[MAX_TOKEN];
    char tmpStr1[MAX_TOKEN];
    char tmpStr2[MAX_TOKEN];
    int status;
    char *dataName, *collection, *size, *offset, *chksum, *replNum;
    int i;
    mdasC_sql_result_struct targResult;


    if (dataResult->result_count == 0)
        return 0;

    dataName = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    collection = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    size = (char *) getFromResultStruct(dataResult,
      dcs_tname[SIZE], dcs_aname[SIZE]);
    chksum = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
    replNum = (char *) getFromResultStruct(dataResult,
      dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);

    for (i = 0; i < dataResult->row_count; i++) {
        sprintf (myPath, "%s/%s", targColl, dataName);
        status = queryData (conn, catType, myPath, Y_FLAG, MAX_GET_RESULTS,
         &targResult);
        if (status < 0) {
            if (status != MCAT_INQUIRE_ERROR) {
                srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                return status;
            }
            status = dataToDataRsync1 (conn, catType, flagval, dataName,
             collection, size, offset, chksum, myPath, copyNum, resourceName,
             -1, NULL, NULL);
        } else {
            char *targSize, *targChksum;

            targSize = (char *) getFromResultStruct (&targResult,
              dcs_tname[SIZE], dcs_aname[SIZE]);
            targChksum = (char *) getFromResultStruct (&targResult,
              dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);

            status = dataToDataRsync1 (conn, catType, flagval, dataName,
             collection, size, offset, chksum, myPath, copyNum, resourceName,
             DATANAME_T, targSize, targChksum);

            if (status < 0) {
                fprintf (stderr, "dataToDataRsync1 of %s failed, status=%d\n",
                 myPath, status);
                break;
            }
            freeSqlResult (&targResult);
        }
        dataName += MAX_DATA_SIZE;
        collection += MAX_DATA_SIZE;
        size += MAX_DATA_SIZE;
        chksum += MAX_DATA_SIZE;
        replNum += MAX_DATA_SIZE;
    }
    return (status);
}

int
collToCollRsync (srbConn *conn, int catType, int flagval,
int inColltype, int collCnt, mdasC_sql_result_struct *inCollResult,
char *targColl, char *resourceName,char *newPath, char *tmpInxDir, int copyNum)
{
    int i, j, status;
    int in_fd, out_fd;
    srb_long_t datasize;
    char *datatype;
    char temp[MAX_TOKEN], temp1[MAX_TOKEN], tmpTarg[MAX_TOKEN];
    char *origSrc, *curBaseTarg;
    struct stat statbuf;
    mdasC_sql_result_struct myresult, srcResult;
    char lastColl[MAX_TOKEN], lastData[MAX_TOKEN];
    int targetMode;
    struct srbFileOut mySrcFileOut, myTargFileOut;

    struct srbFileList  *srbFileListHead = NULL;
    int lastDirty = -1;

    if (inCollResult->result_count == 0)
        return 0;
    origSrc = (char *) getFromResultStruct(inCollResult,
      dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

    for (i = 0; i < inCollResult->row_count; i++) {
        char *dataName, *collection, *size, *offset, *chksum, *replNum,
         *isDirty;
        int origSrcLen;

        if (collCnt <= 1) {     /* only src coll */
            /* Make the targ coll */
            curBaseTarg = targColl;
        } else {        /* more than one src */
            clSplitbychar(origSrc, temp, temp1,'/');
            sprintf (tmpTarg, "%s/%s", targColl, temp1);
            curBaseTarg = tmpTarg;
            status = createCollectRecur (conn, catType, targColl, curBaseTarg,
             1);
            if (status < 0) {
                fprintf (stderr,
                 "createCollectRecur of %s tailed, status = %d\n",
                  curBaseTarg, status);
                return (status);
            }
        }
        status = queryDataInColl (conn, catType, targColl, y_FLAG | flagval,
         RSYNC_ROWS_WANTED, &myresult);

        if (status < 0) {
            if (status != MCAT_INQUIRE_ERROR) {
                srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
                return status;
            }
        }

        if (myresult.continuation_index >= 0) {
            /* metadata are in files */
            targetMode = 1;
            status = genSrbFileList (conn, catType, flagval, targColl,
             tmpInxDir, &srbFileListHead, &myresult);
        } else {
            /* metadata are in mem */
            targetMode = 0;
        }

        origSrcLen = strlen (origSrc);

        status = queryDataInColl (conn, catType, origSrc, y_FLAG | flagval,
         RSYNC_ROWS_WANTED, &srcResult);

        if (status < 0) {
            if (status != MCAT_INQUIRE_ERROR)
                srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
            continue;
        }

        do {
            int startInx;

            dataName = (char *) getFromResultStruct(&srcResult,
              dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
            collection = (char *) getFromResultStruct(&srcResult,
              dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
            size = (char *) getFromResultStruct(&srcResult,
              dcs_tname[SIZE], dcs_aname[SIZE]);
            chksum = (char *) getFromResultStruct(&srcResult,
              dcs_tname[DATA_CHECKSUM], dcs_aname[DATA_CHECKSUM]);
            replNum = (char *) getFromResultStruct(&srcResult,
              dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
            isDirty = (char *) getFromResultStruct(&srcResult,
              dcs_tname[IS_DIRTY], dcs_aname[IS_DIRTY]);

            if (lastDirty < 0) {
                startInx = 0;
            } else if (strcmp (lastColl, collection) != 0 ||
             strcmp (lastData, dataName) != 0) {
                startInx = 0;
            } else {    /* a match */
                int thisDirty;

                if (lastDirty > 0) {    /* a good copy already */
                    startInx = 1;
                } else {
                    thisDirty = atoi (isDirty);
                    if (thisDirty > 0) {
                        startInx = 0;
                    } else {    /* both are not dirty */
                        startInx = 1;
                    }
                }
            }
            for (j = startInx; j < srcResult.row_count; j++) {
                int iChksum, localChksum;
                char myTarg[MAX_TOKEN];
                srb_long_t datasize;
                srb_long_t lstatus;

                if (strlen (collection) == origSrcLen) {
                    strcpy (mySrcFileOut.fileDir, ".");
                    strcpy (myTarg, curBaseTarg);
                } else {
                    sprintf (myTarg, "%s/%s", curBaseTarg,
                     collection + origSrcLen + 1);
                    status = createCollectRecur (conn, catType, curBaseTarg,
                     myTarg, 1);
                    if (status < 0) {
                        fprintf(stderr,
                         "Unable to mk the target dir %s, status = %d\n",
                          myTarg, status);
                        return (CLI_ERR_COLLNAME);
                    }
                    sprintf (mySrcFileOut.fileDir, "./%s",
                     collection + origSrcLen + 1);
                }
                strcpy (mySrcFileOut.fileName, dataName);

                if (targetMode == 0) {
                    status = matchFileOutInMem (&mySrcFileOut, &myTargFileOut,
                     &myresult, curBaseTarg);
                } else {
                    status = matchFileOutInFileInx (&mySrcFileOut,
                     &myTargFileOut, srbFileListHead, curBaseTarg);
                }

                strcat (myTarg, "/");
                strcat (myTarg, dataName);

                if (status != 0) {
                    status = dataToDataRsync1 (conn, catType, flagval,
                     dataName, collection, size, NULL, chksum, myTarg, copyNum,
                     resourceName, -1, NULL, NULL);
                } else {
                    char targChksum[CHKSUM_STR_LEN + 1],
                     targSize[CHKSUM_STR_LEN + 1];


                    sprintf (targChksum, "%d", myTargFileOut.chksum);
                    sprintf (targSize, "%lld", myTargFileOut.size);
                    status = dataToDataRsync1 (conn, catType, flagval,
                     dataName, collection, size, NULL, chksum, myTarg, copyNum,
                     resourceName, DATANAME_T, targSize, targChksum);
                }

                if (status < 0)
                    return (status);

                dataName += MAX_DATA_SIZE;
                collection += MAX_DATA_SIZE;
                isDirty += MAX_DATA_SIZE;
                size += MAX_DATA_SIZE;
                chksum += MAX_DATA_SIZE;
                replNum += MAX_DATA_SIZE;
           }
           if (srcResult.continuation_index >= 0 &&
              srcResult.result_count > 0) {
                /* save the last dataName, collection and isDirty */
                dataName -= MAX_DATA_SIZE;
                collection -= MAX_DATA_SIZE;
                isDirty -= MAX_DATA_SIZE;
                strcpy (lastColl, collection);
                strcpy (lastData, dataName);
                lastDirty = atoi (isDirty);
                clearSqlResult (&srcResult);
                status = srbGetMoreRows(conn, catType,
                 srcResult.continuation_index, &srcResult, MAX_GET_RESULTS);
                if (status == 0)
                    filterDuplicate (&srcResult);
            } else
                clearSqlResult (&srcResult);

        } while (srcResult.result_count > 0 && status >= 0);
        clearSrbFileList (srbFileListHead);
        origSrc += MAX_DATA_SIZE;
    }
}

int
isContFamily (char *contName, char *inpArgv)
{
    int inpLen;
    char *tmpPtr;
    int contNum;
    int contNumLen;

    if (inpArgv == NULL)
        return (1);

    if (contName == NULL)
        return (0);

    inpLen = strlen (inpArgv);

    /* family query ? */
    if (inpLen <= 2 || inpArgv[inpLen -1] != '*' || 
     inpArgv[inpLen -2] != '.') {
        return (1);
    }

    tmpPtr = contName + inpLen - 1;

    contNumLen = strlen (tmpPtr);

    /* time stamp much be 10 digit long */
    if (contNumLen != 10)
        return (0);

    contNum = atoi (tmpPtr);

    if (contNum < 1000000000)
        return (0);

    return (1);
}

int
setObjCondForNew (char *targObj, char *outTargObj,
int flagval, char *containerName, char *filePath)
{
    char *tmpPtr;
    char outFileGroup[ENV_LEN];

    if (targObj == NULL || outTargObj == NULL) {
        fprintf (stderr, "setObjCondForNew: NULL targObj\n");
        return -1;
    }

    strcpy (outTargObj, targObj);
    tmpPtr = outTargObj + strlen (outTargObj);
    if (flagval & c_FLAG && containerName != NULL) {     /* user container */
        sprintf (tmpPtr,
         "&CONTAINER=%-s", containerName);
        tmpPtr += strlen (tmpPtr);
    }

    if (flagval & g_FLAG) {             /* fileGroup */
        setFileGroup (filePath, outFileGroup, flagval);
        sprintf (tmpPtr,
         "&FILE_GROUP=%-s", outFileGroup);
        tmpPtr += strlen (tmpPtr);
    }

    if (flagval & a_FLAG) {         /* COPIES=all */
        sprintf (tmpPtr, "&COPIES=all");
        tmpPtr += strlen (tmpPtr);
    }
    return (0);
}

int
setObjCondForOld (char *targObj, char *outTargObj,
int flagval, char *filePath)
{
    char *tmpPtr;
    char outFileGroup[ENV_LEN];

    if (targObj == NULL || outTargObj == NULL) {
        fprintf (stderr, "setObjCondForNew: NULL targObj\n");
        return -1;
    }

    strcpy (outTargObj, targObj);
    tmpPtr = outTargObj + strlen (outTargObj);

    if (flagval & g_FLAG) {             /* fileGroup */
        setFileGroup (filePath, outFileGroup, flagval);
        sprintf (tmpPtr,
         "&FILE_GROUP=%-s", outFileGroup);
        tmpPtr += strlen (tmpPtr);
    }

    if (flagval & a_FLAG) {         /* COPIES=all */
        sprintf (tmpPtr, "&COPIES=all");
        tmpPtr += strlen (tmpPtr);
    }
    return (0);
}

int
setFileGroup (char *inpPath, char *outFileGroup, int flagval)
{
/* Windows doesn't handle the user group. */
#ifndef _WIN32
    struct stat statbuf;
    int status;
    struct group *myGroup;

    if (outFileGroup == NULL) {
        fprintf (stderr, "setFileGroup: NULL outFileGroup\n");
        return -1;
    }

    if (!(flagval & g_FLAG)) {
        outFileGroup[0] = '\0';
        return 0;
    }

    if (strlen (FileGroup) > 0) {       /* input file group */
        strcpy (outFileGroup, FileGroup);
        return 0;
    }

    /* get it from inpPath */
    if (inpPath == NULL) {
        fprintf (stderr, "setFileGroup: NULL inpPath\n");
        outFileGroup[0] = '\0';
        return -1;
    }

    status = stat (inpPath, &statbuf);

    if (status != 0) {
        outFileGroup[0] = '\0';
        return -1;
    }

    myGroup = getgrgid (statbuf.st_gid);

    if (myGroup == NULL) {
        /* just use the gid */
        sprintf (outFileGroup, "%-d", statbuf.st_gid);
        return -1;
    }

    strcpy (outFileGroup, myGroup->gr_name);
#endif

    return (0);
}

int
getSomeCopyNum(srbConn *conn, int catType, char *collName, char *dataName)
{
  int i;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  char *copyList = NULL;
  mdasC_sql_result_struct dataResult;

  for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
  }
  selval[DATA_REPL_ENUM] = 1;
  genEqualSQL (qval[DATA_NAME], dataName);
  genEqualSQL (qval[DATA_GRP_NAME], collName);
  i = srbGetDataDirInfo(conn, catType, qval, selval, &dataResult, 3);
  if (i != 0)
    return (DATA_REPL_ENUM_NOT_FOUND);
  copyList = (char *) getFromResultStruct (&dataResult,
					   dcs_tname[DATA_REPL_ENUM],dcs_aname[DATA_REPL_ENUM]);
  if (copyList == NULL)
    return (DATA_REPL_ENUM_NOT_FOUND);
  i = atoi(copyList);
  clearSqlResult(&dataResult);
  return(i);
}

int
getZoneNameList(srbConn *conn, int catType, char zoneList[MAX_GET_RESULTS][MAX_TOKEN], char *zoneName)
{
    int i;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  char *copyList = NULL;
  mdasC_sql_result_struct dataResult;
  for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
  }
  selval[ZONE_NAME] = 1;
  if (strlen(zoneName) > 0) {
    make_like_for_mdas(zoneName);
    sprintf(qval[ZONE_NAME]," like '%s' ", zoneName);
  }
  i = srbGetDataDirInfo(conn, catType, qval, selval, &dataResult, MAX_GET_RESULTS);
  if (i < 0)
    return (ZONE_NAME_NOT_FOUND);
  copyList = (char *) getFromResultStruct (&dataResult,
                                           dcs_tname[ZONE_NAME],dcs_aname[ZONE_NAME]);
  if (copyList == NULL)
    return (ZONE_NAME_NOT_FOUND);
  
  for (i=0; i < dataResult.row_count; i++) {
    strcpy(zoneList[i],copyList);
    copyList += MAX_DATA_SIZE;
  }
  clearSqlResult(&dataResult);
  return(i);
  
  
}


int
queryAndPrintAcrossZones(srbConn *conn, int catType, char *zoneName,
		char qval[][MAX_TOKEN], int *selval, 
		mdasC_sql_result_struct *myresult, int rowsWanted)
{
  int n,printed;
  char zoneList[MAX_GET_RESULTS][MAX_TOKEN];
  int status;

  printed = 0;
  if (zoneName == NULL || strlen(zoneName) == 0) {
      status = srbGetDataDirInfo(conn, catType , qval, selval, myresult,
				 rowsWanted);
      if (status < 0) {
          if (status == MCAT_INQUIRE_ERROR)
            printf("No Answer found for the Query\n");
	  return(status);
      }
      else {
          show_results(conn, qval, selval, myresult, DEFAULT_FIELD_WIDTH,
		       rowsWanted);
      }
      return(0);
  }
  else {
    if (!strcmp(zoneName,"ALL"))
      n = getZoneNameList(conn, catType,zoneList,"");
    else
      n = getZoneNameList(conn, catType,zoneList,zoneName);
      if (n <= 0) 
	return(n);
      for (i = 0; i < n ; i++){
        fprintf(stdout,"Querying Zone=%s\n",zoneList[i]);fflush(stdout);
        status = srbGetDataDirInfoWithZone(conn, catType ,zoneList[i], qval, selval, myresult,
                                 DEFAULT_ROW_COUNT);
	if (status < 0) {
	  if (status == MCAT_INQUIRE_ERROR) 
	    printf("No Answer found for the Query\n");
	  else
	    printf("Error in Querying: %i\n",status);
	}
	else {
	  printed = 1;
	  show_results(conn, qval, selval, myresult, DEFAULT_FIELD_WIDTH,
		       rowsWanted);
	}
      }
  }
  if (printed)
    return(0);
  else
    return(status);
}


int
isAlphaString(char *s)
{
  int i;
  int j = 0;
  for (i = 0 ; i < strlen(s) ; i++) {
    if (isdigit(s[i]))
	j = 1;
    else if (s[i] == 'E' || s[i] == '.' || s[i] == '+' || s[i] == '-' )
      continue;
    else 
      return(1);
    continue;
  }
  if (j == 1)
    return(0);
  else
    return(1);
}


