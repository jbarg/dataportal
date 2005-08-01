#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "scommands.h"


/*  For readdir(3)  */
#include <sys/types.h>
#include <dirent.h>
/********************/


#ifdef PORTNAME_c90
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif

#define LOGIT 1

#define LF 10
#define CR 13
#define LANG_STRING_NBR 300

/*   CAN CAUSE PROBLEMS */
#define DATACOMMENTS 1
/*   CAN CAUSE PROBLEMS */
#define VERSION_NUM         "V3.3"
#define MYSRBBUFSIZE        2000000
#define SRBUSER             "sekar"
#define SRBUSERDOMAIN       "sdsc"
#define SRBAUTH             "CANDO"
#define QSIZE               MYSRBBUFSIZE
/**
#define DATAFORMNUMOFARGS 6
#define COLLFORMNUMOFARGS 5
**/
#define DATAFORMNUMOFARGS 7
#define COLLFORMNUMOFARGS 6

#define MAX_NUM_COMP_COUNT 5

#define MAX_RESULT_NUM 100
#define MAX_ENTRIES 1000
#define MAX_U_ENTRIES 15

/* datascope (ROADNet) constants --sifang */
#define dbSCHEMA_TABLES  -358
#define dbSCHEMA_FIELDS  -351

char *aaa[] = {
"ExtSchm<BR>Query",
"Extensible Schema Query",
"Select Schemas From",
"Submit",
"Resource Name",
"Data Comments",
"Data Size",
"Modify Timestamp",
"Resource Type",
"Resource Location",
"Create Timestamp",
"User Name",
"User Access Privilege",
"User Domain",
"Update<BR>ExtSchm",
"Select Table From:<BR>Schema->Table",
"How many Attributes: <BR>do you plan to query?",
"How many Rows do you plan to query?",
"Will your query relate:<BR> to data or collections?",
"Yes (with user-def metadata)",
"Yes (but no user-def metadata)",
"No",
"How many Rows do you plan to ingest?",
"Extensible Schema Ingestion",
"Insert Extensible Metadata",
"Starting Meta Data Ingest ",
"Please single-quote string values<BR>All values should be given in  a row",
"Update ExtMetadata",     /* 27 */
"Modify",
"Select Function: Ingest",
"Modify<BR>CheckBox",
"Delete<BR>CheckBox",
"Modifying Metadata",
"Modifying Annotation",
"Delete",
"Extensible Schema Modify",
"Extensible Schema Delete",
"Reset",
"Modify ExtMetadata",
"Delete ExtMetadata",
"Deleting Metadata", /* 40 */
"Extensible Schema Update"
};
char * escape_url(char *in);
char * simple_escape_url(char *in);
char * substituteString(char *datastr, char *instr, char *outstr);

typedef struct {
   char name[MAX_TOKEN];
   int size;
   char *val;
   char *fstr;
} entry;
 
FILE *fId;
FILE *filesfd = NULL;

unsigned int nID = 1;   /* node id for zones, resources and users in zone info */
                /* also used as an initial value in printAccsCollectionsAsTree
                   to avoid node ID collisions */


/***** for testing purposes, to be removed eventually *****/
FILE *testfile, *testfile2;
int qq;
char mybuf[20];
/**********************************************************/


/********** for bad-length pb on RedHat 7 ************/
char *solvepb;
/*****************************************************/

typedef struct {
    entry entries[MAX_ENTRIES];
    char tmpoStr[HUGE_STRING];
    char cookieStr[HUGE_STRING];
    char pidStr[MAX_TOKEN];
    char frameStr[MAX_TOKEN];
    char remoteHost[MAX_TOKEN];
    char srbUser[MAX_TOKEN];
    char userDomain[MAX_TOKEN];
    char currentLanguage[MAX_TOKEN];
    char startcol[MAX_TOKEN];
    char startzone[MAX_TOKEN];    
    char errorMsg[MAX_TOKEN];
    int showAllMetaFlag;
    int oddFlag;
    unsigned int rowCntr;
    char srbPort[MAX_TOKEN];
    char imageName[MAX_TOKEN];
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    char cfile[300];
    int m;  /* m is the number of the last entry which usually stores session ID # */
    int alive;
    char selectData[20*HUGE_STRING];
//    char *selectData;  /* initial memory allocation is made in performQuery() */
//    unsigned int sizeOfSelectData; /*** used for dynamic memory allocation ***/
    srbConn *conn;
    int  nbytes;
    int  in_fd;
    int out_fd;
    char buf[MYSRBBUFSIZE];
    char user[MAX_TOKEN];
    char passwd[MAX_TOKEN];
    char domain[MAX_TOKEN];
    char hostname[MAX_TOKEN];
    char envstr[MAX_TOKEN];
    char envstr2[MAX_TOKEN];
    char envstr3[MAX_TOKEN];
    char envstr4[MAX_TOKEN];
    char tmpenvstr1[MAX_TOKEN];
    char tmpenvstr2[MAX_TOKEN];
    char tmpenvstr3[MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int heading;
    int first;
    int firsts;
    int firstd;
    int isTicket;
    int slinkFlag;
    int queryFunction;
    int globalCounter;
    int  likeCollFlag;
    char tmpCollName[1000];
    char tmpDataName[1000];
    char urlcollList[3*MAX_TOKEN];
    char urldataList[3*MAX_TOKEN];
    char urlpathNameList[3*MAX_TOKEN];
    char urldataTypeList[3*MAX_TOKEN];
    char urlrsrcName[3*MAX_TOKEN];
    char nonurlcollList[3*MAX_TOKEN];
    char nonurldataList[3*MAX_TOKEN];
    int linkCtr;
    char *tmpPtr;
    char *linkPtr[MAX_ENTRIES];
    int showMDVCntr;
    char *showMDVTitle[200];
    int  showMDVId[200];
    char *collOwner;
    int showMetaType;
    char sqlDataName[3*MAX_TOKEN];
    char returnEmailDomain[MAX_TOKEN];
    char checkHostAddress[MAX_TOKEN];
    char connectHostAddress[MAX_TOKEN];  
    char SRBPORT[MAX_TOKEN];
    char httptype[MAX_TOKEN]; 
    char emailAddress[MAX_TOKEN]; 
    char newUserDomain[MAX_TOKEN]; 
    char defaultResource[MAX_TOKEN];
    int op;
    char *currentMcatZone;
    char *oldMcatZone;
    char userHomeZone[MAX_TOKEN];
    char userConnectionZone[MAX_TOKEN];
    char *udsmdArray[MAX_UDSMD_NUMBER];
} paramIn;

typedef paramIn *inStruct;

typedef struct {
    int status;
    int pageType;
    
    inStruct Sentry;
/******* temporary *******/
    char *entr0;
    FILE *filest;

/****** the following will point to another struct that will depend on
         what type of page is to be dispayed 
          
    void *specOutput;  
    
*************************************************************/ 

} paramOut;

typedef paramOut *outStruct;

char *mySrbStrings[LANG_STRING_NBR];

char mode[MAX_TOKEN];

/*************************************/

char tmpoStr[HUGE_STRING];




char srbUser[MAX_TOKEN];


char errorMsg[MAX_TOKEN];
int showAllMetaFlag = 0;
int oddFlag;


char imageName[MAX_TOKEN];
/*************************************/

char *metaAttrNames[] = {"Data Name",
"Data Replica",
"Collection",
"Data Type",
"Data Size",
"Data Comments",
"Creation Date",
"Last Modify Date",
"Owner (Name@Dom)",
"Resource Name",
"Resource Class",
"Resource Type",
"Rsrc Max Latency",
"Rsrc Bandwidth",
"Rsrc NumOfHierarchy",
"Rsrc Capacity",
"Container Name",
"User (Name@Dom)",
"Annotation",
"Annotator (Name@Dom)",
"Anno Position",
"Anno TimeStamp",
"null"};
char *dicomStr[][3] = { /* name , id , type */
"Institution Name","235","text",
"Manufacturer","236","text",
"Station Name","237","text",
"Patients Sex","238","text",
"Patients Name","239","text",
"PatientID","240","text",
"Patients BirthTime","241","text",
"Patients Age","250","number",
"PatientsSize","251","number",
"PatientsWeight","252","number",
"Occupation","253","text",
"Ethnic Group","242","text",
"AccessionNumber","247","number",
"ProcedureDescription","248","text",
"Study Date","243","text",
"Study Time","244","text",
"StudyID","245","text",
"Study Description","246","text",
"StudyInstanceUID","249","text",
"Series Number","254","number",
"SeriesInstanceUID","255","text",
"Series Date","258","text",
"Series Time","259","text",
"Series Description","260","text",
"Modality","256","text",
"Image Number","257","number",
"Protocol Name","261","text",
"Presentation Label","262","text",
"Admitting Diagnoses Description","263","text",
"SCHEMAEND","",""
};

char *dubcoreStr[][4] = { /*name,value,units,comments*/
"DC.Identifier","","","",
"DC.Title","","","",
"DC.Title","","Alternate","Use this for alternate title",
"DC.Creator","","","Use Value Type to describe Creator, eg. Artist,Sculptor,Writer,etc",
"DC.Creator","","","",
"DC.Creator","","","",
"DC.Contributor","","","",
"DC.Contributor","","","",
"DC.Subject","","","Use Value Type to describe Subject Classifications, eg. LC,ACM",
"DC.Subject","","","",
"DC.Subject","","","",
"DC.Description","","","",
"DC.Date","","","Use Value Type to describe the date, eg. available from, created, added to collection, etc",
"DC.Type",",Collection,Dataset,Event,Image,Interactive Resource,Service,Software,Sound,Text","DCMI Type","from DCMI working group",
"DC.Type","","","Use for Non-DCMI Type",
"DC.Format","","","",
"DC.Format","","","",
"DC.Publisher","","","",
"DC.Source","","","",
"DC.Language","","","",
"DC.Coverage","","","Use Value Type to describe the Coverage, eg. place, period, date range, etc",
"DC.Coverage","","","",
"DC.Rights","","","",
"DC.Relation","","","Use Value Type to describe the relationship to the related article, eg. photograph, xml-metadata, etc<BR>Also use 'URL:','SRB:' to show the location",
"DC.Relation","","","",
"DC.Relation","","","",
"DC.NULL","","",""};


char METAATTRLIST[]=  "<SELECT SIZE=1 NAME=a1>\
<OPTION SELECTED> \
<OPTION> Data Name\
<OPTION> Data Replica\
<OPTION> Collection\
<OPTION> Data Type\
<OPTION> Data Size\
<OPTION> Data Comments\
<OPTION> Creation Date\
<OPTION> Last Modify Date\
<OPTION> Owner (Name@Dom)\
<OPTION> Resource Name\
<OPTION> Resource class\
<OPTION> Resource Type\
<OPTION> Rsrc Max Latency\
<OPTION> Rsrc Bandwidth\
<OPTION> Rsrc NumOfHierarchy\
<OPTION> Rsrc Capacity\
<OPTION> Container Name\
<OPTION> User (Name@Dom)\
<OPTION> Annotation\
<OPTION> Annotator (Name@Dom)\
<OPTION> Anno Position\
<OPTION> Anno TimeStamp\
</SELECT>";
char METACOMPLIST[] = "<SELECT SIZE=1 NAME=c1><OPTION SELECTED> = <OPTION> <> <OPTION> > <OPTION> num > <OPTION> < <OPTION> num < <OPTION> >= <OPTION> num >= <OPTION> <= <OPTION> num <= <OPTION> between <OPTION> num between <OPTION> not between <OPTION> num not between <OPTION> like <OPTION> not like <OPTION> sounds like <OPTION> sounds not like <OPTION> in <OPTION> not in <OPTION> contains <OPTION> not contains </SELECT>";

/***********/
  char qval[MAX_DCS_NUM][MAX_TOKEN], cfile[300];
  int i, j, alive;
  char selectData[HUGE_STRING];

    int  nbytes,  in_fd, out_fd;
    char buf[MYSRBBUFSIZE];
    char user[MAX_TOKEN];
    char passwd[MAX_TOKEN];
    char domain[MAX_TOKEN];
    char hostname[MAX_TOKEN];
    char envstr[MAX_TOKEN], envstr2[MAX_TOKEN], envstr3[MAX_TOKEN], envstr4[MAX_TOKEN];
    char tmpenvstr1[MAX_TOKEN], tmpenvstr2[MAX_TOKEN], tmpenvstr3[MAX_TOKEN];
  int selval[MAX_DCS_NUM];
  int heading = 0;
  int first = 1;
  int firsts = 1;
  int firstd = 1;
  int isTicket = 0;
  int slinkFlag = 0;
  int queryFunction = 0;

  int  likeCollFlag = 0;
  char tmpCollName[1000];
  char tmpDataName[1000];
  char urlcollList[3*MAX_TOKEN];
  char urldataList[3*MAX_TOKEN];
  char urlpathNameList[3*MAX_TOKEN];
  char urldataTypeList[3*MAX_TOKEN];
  char urlrsrcName[3*MAX_TOKEN];
  char nonurlcollList[3*MAX_TOKEN];
  char nonurldataList[3*MAX_TOKEN];
  char emptyStr[] = "";
  int linkCtr = 0;
  char *tmpPtr;
  char *linkPtr[MAX_ENTRIES];
  int showMDVCntr =  0;
  char *showMDVTitle[200];
  int  showMDVId[200];
  char *collOwner;
  int showMetaType;
  char sqlDataName[3*MAX_TOKEN];
  char logDirec[3*MAX_TOKEN];
  char langDir[3*MAX_TOKEN];
  char returnEmailDomain[MAX_TOKEN];
  char checkHostAddress[MAX_TOKEN];
  char connectHostAddress[MAX_TOKEN];  
  char SRBPORT[MAX_TOKEN];
  char httptype[MAX_TOKEN]; 
  char webPath[MAX_TOKEN]; 
  char emailAddress[MAX_TOKEN]; 
  char newUserDomain[MAX_TOKEN]; 
  char defaultResource[MAX_TOKEN];
  
  /**************************************************************************/
  
hScr()
{
  /***
  printf("<noscript>\n");
  printf("<meta http-equiv=\"refresh\" content=\"2\">\n");
  printf("</noscript>\n");
  printf("<script language=\"JavaScript\">\n");
  printf("<!--\n");
  printf("var sURL = unescape(window.location.pathname);\n");
  printf("function doLoad()\n");
  printf("{\n");
  printf("    setTimeout( \"refresh()\", 2*1000 );\n");
  printf("}\n");
  printf("\n");
  printf("function refresh()\n");
  printf("{\n");
  printf("    window.location.href = sURL;\n");
  printf("}\n");
  printf("//-->\n");
  printf("</script>\n");
  printf("<script language=\"JavaScript1.1\">\n");
  printf("<!--\n");
  printf("function refresh()\n");
  printf("{\n");
  printf("    window.location.replace( sURL );\n");
  printf("}\n");
  printf("//-->\n");
  printf("</script>\n");
  printf("<script language=\"JavaScript1.2\">\n");
  printf("<!--\n");
  printf("function refresh()\n");
  printf("{\n");
  printf("    window.location.reload( true );\n");
  printf("}\n");
  printf("//-->\n");
  printf("</script>\n");
  ****/
}

bRMap (int nBacks, int refFlag) 
{
  int i, j;
  j = 0;
  /***
  printf ("\n<img src=\"%s/docimg.gif\" onclick=\"", webPath);
  for (i = 0; i < nBacks; i++) {
    if (j==1) printf(";");
    printf ("history.back()");
    j = 1;
  }
  if (refFlag) {
    if (j==1) printf(";");
    printf ("refresh()");
  }
  printf ("\">");
  ***/
  printf ("\n<form><input type=\"button\" value=\"Back\"  onclick=\"");
  if (nBacks > 0) {
    printf ("window.history.go(-%i)",nBacks);
    j = 1;
  }
  /***
  if (refFlag) {
    if (j==1) printf(";");
    printf ("javascript:location.reload()");
    }***/
  printf ("\"></form>");
  
}

getBoundary(char **stquery, char *boundary)
{
  char *tmp1;
  
  tmp1 = *stquery;
  while (*tmp1 != '\n')
    tmp1++;
  tmp1--;
  *tmp1 = '\0';
  strcpy(boundary,*stquery);
  *tmp1 = '\n';
}

char*
findBoundary(char *inStr, char *bound, int length)
{
  int i,j;
  void *tmpPtr, *tmpPtr1;
  j = strlen(bound);
  tmpPtr = (void *) inStr;
  while (length > 0) {
    /**    printf("  AA:%i:%c\n",length,bound[0]); fflush(stdout);**/
    tmpPtr1 = memchr( tmpPtr, bound[0], length);
    /**printf("  BB:%i:%c %i:%c\n", (char *) tmpPtr, (int) *((char *) tmpPtr), (char *) tmpPtr1,(int) *((char *) tmpPtr1)); **/
    if (tmpPtr1 == NULL) 
      return NULL;
    if (memcmp(tmpPtr1, (void *) bound, j) == 0) 
      return tmpPtr1;
    length = length -  (int) ((char *)tmpPtr1 - (char *) tmpPtr) + 1;
    tmpPtr = (void *) ((char *) tmpPtr1 + 1);
  }
  return NULL;
    
}
int
getmultipartword( entry *iEntry, char **stquery, char *boundary, int length)
{
  char *inS, *tmp1,*tmp2, *endStr;
  int i,j,k;

  if (strlen(*stquery) < (2 * strlen(boundary)))
    return(1);
  
  if ((tmp1 = strstr((char *) *stquery,boundary)) == NULL) {
    webErrorExit("No Beginning Boundary Found In Field:",0);
  }
  if ((endStr = findBoundary((char *) (tmp1 + 1), boundary,length)) == NULL) {
    webErrorExit("No Ending Boundary Found In Field:",0);
  }
  *(endStr -2) = '\0';   /* why are we doing this ?? */

  if ((tmp2 =  strstr(tmp1,"name=\"")) == NULL){
    webErrorExit("No Name Found In Field",0);
  }
  tmp2 += 6;
  tmp1 =  strchr(tmp2,'\"');
  *tmp1 = '\0';
  tmp1++;
  strcpy(iEntry->name, tmp2);
  if ((int) (strstr(tmp1, "filename") != NULL &&
	     strstr(tmp1, "filename") - tmp1) < 5) {
    /* this is a file - skip two lines*/
    while (*tmp1 != '\n')
      tmp1++;
    tmp1++;
    while (*tmp1 != '\n')
      tmp1++;
    tmp1++;
    /** looks like this is needed **/
    if (strstr(iEntry->name,"filename") != NULL) {
      tmp1++;
      tmp1++;  
    }
  /** looks like this is needed **/      
    iEntry->size = (int)(endStr - tmp1  );
    iEntry->fstr = NULL;
    iEntry->val = (char *) malloc(iEntry->size);
    memcpy(iEntry->val, tmp1, iEntry->size);
    iEntry->size -= 2;
  }
  else {
  /** looks like this is needed **
  tmp1++;
  tmp1++;  
  ** looks like this is needed **/      
    iEntry->val = (char *) malloc(strlen(tmp1) + 3);
    iEntry->fstr = NULL;
    strcpy(iEntry->val,tmp1);
    iEntry->size = strlen(iEntry->val);
  }
  *stquery = endStr;

  return 0;
}
 int
  mybgets(char inbuf[], char buffer[], int j)
  {
    char ch;
    int i = 0;
    ch = inbuf[j];
    j++;
    if (ch == '\0')
      {
	return -1;
      }
    while (ch != '\n')
      {
	buffer[i] = ch;
	++i;
	ch = inbuf[j];
	j++;
    if (ch == '\0')
      {
	return -1;
      }
     }
     buffer[i] = '\0';
     return j;
 }


 char 
 myfgets(FILE *infile, char buffer[])
 {
   char ch;
   int i = 0;
   ch = getc(infile);
   if (ch == EOF)
     {
       return ch;
     }
   while (ch != '\n')
     {
       buffer[i] = ch;
       ++i;
       ch = getc(infile);
     }
     buffer[i] = '\0';

     return '1';
 }


		       



 void getword(char *word, char *line, char stop) {
   int x = 0,y;

     for(x=0;((line[x]) && (line[x] != stop));x++)
	 word[x] = line[x];

     word[x] = '\0';
     if(line[x]) ++x;
     y=0;

     while(line[y++] = line[x++]);
 }

 char *makeword(char *line, char stop) {
     int x = 0,y;
     char *word = (char *) malloc(sizeof(char) * (strlen(line) + 1));

     for(x=0;((line[x]) && (line[x] != stop));x++)
	 word[x] = line[x];

     word[x] = '\0';
     if(line[x]) ++x;
     y=0;

     while(line[y++] = line[x++]);
     return word;
 }

 char *fmakeword(FILE *f, char stop, int *cl) {
     int wsize;
     char *word;
     int ll;

     wsize = 102400;
     ll=0;
     word = (char *) malloc(sizeof(char) * (wsize + 1));

     while(1) {
	 word[ll] = (char)fgetc(f); printf("%c",word[ll]);
	 if(ll==wsize) {
	     word[ll+1] = '\0';
	     wsize+=102400;
	     word = (char *)realloc(word,sizeof(char)*(wsize+1));
	 }
	 --(*cl);
	 if((word[ll] == stop) || (feof(f)) || (!(*cl))) {
	     if(word[ll] != stop) ll++;
	     word[ll] = '\0';
	     return word;
	 }
	 ++ll;
     }
 }


 int rind(char *s, char c) {
     register int x;
     for(x=strlen(s) - 1;x != -1; x--)
	 if(s[x] == c) return x;
     return -1;
 }

 int getline(char *s, int n, FILE *f) {
     register int i=0;

     while(1) {
	 s[i] = (char)fgetc(f);

	 if(s[i] == CR)
	     s[i] = fgetc(f);

	 if((s[i] == 0x4) || (s[i] == LF) || (i == (n-1))) {
	     s[i] = '\0';
	     return (feof(f) ? 1 : 0);
	 }
	 ++i;
     }
 }

 void send_fd(FILE *f, FILE *fd)
 {
     int num_chars=0;
     char c;

     while (1) {
	 c = fgetc(f);
	 if(feof(f))
	     return;
	 fputc(c,fd);
     }
 }

webErrorExit(char *msg, int status)
{
  if (!heading) {    
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html>");
    printf("<body background=\"%s/srb3.jpg\">", webPath);
  }
  printf("<B><FONT COLOR=#DC143C>Error: %s: %i</B>\n",msg, status);
  printf("</body></html>");
  exit(-1);
}

int
srbGetDataDirInfoShunt(inStruct Sentries, srbConn* conn, int catType, 
		       char qval[][MAX_TOKEN],int *selval, 
		       mdasC_sql_result_struct *myresult, int rowsWanted)
{
  int i; 

  /***  if (Sentries->currentMcatZone != NULL)
    fprintf(stdout,"CurrentMcatZone:%s<BR>\n",Sentries->currentMcatZone);
  else
    fprintf(stdout,"CurrentMcatZone:NULL<BR>\n");
  ***/

  i = srbGetDataDirInfoWithZone(conn,catType,Sentries->currentMcatZone,
				qval,selval,myresult,rowsWanted);
	
  return(i);
}



int showMeta(inStruct Sentries)
{
   char annoDataName[1000],annoCollName[300];

   collOwner = Sentries->entries[7].val;
   if (strlen( Sentries->entries[1].val) == 0 )   {
     getCollectionMeta(Sentries, 0);
   }
   else {
     strcpy(annoCollName,Sentries->entries[2].val);
     strcpy(annoDataName,Sentries->entries[1].val);
     i = getDatasetMeta(Sentries, 0);
     strcpy(Sentries->entries[2].val,annoCollName);
     strcpy(Sentries->entries[1].val,annoDataName);
     getDatasetAnno(Sentries, annoCollName,annoDataName,0);
   }

}


int showMetaNew(inStruct Sentries)
{
   char annoDataName[1000], annoCollName[300];


   if (strlen( Sentries->entries[1].val) == 0 )   {
     clSplitbychar (Sentries->entries[2].val, Sentries->tmpCollName, Sentries->tmpDataName, '/');
     if (Sentries->showMetaType == 0)
       printSrbTop("C", mySrbStrings[16], Sentries->tmpCollName, Sentries->tmpDataName, Sentries->entries[7].val);
     else if (Sentries->showMetaType == 1)
       printSrbTop("C","View Metadata Describing Contents", Sentries->tmpCollName, Sentries->tmpDataName, Sentries->entries[7].val);
     else if (Sentries->showMetaType == 2)
       printSrbTop("C","View Metadata Describing Collection", Sentries->tmpCollName, Sentries->tmpDataName, Sentries->entries[7].val);
     strcpy(annoCollName, Sentries->entries[2].val);
     strcpy(annoDataName, Sentries->entries[1].val);
     getCollectionMetaNew(Sentries);
     strcpy(Sentries->entries[2].val,annoCollName);
     strcpy(Sentries->entries[1].val,annoDataName);
     if (Sentries->showMetaType == 0) {
	  getCollectionAnno(Sentries, annoCollName,annoDataName,0);
	 }
   }
   else {
     if (Sentries->showMetaType == 0)
       printSrbTop("D", mySrbStrings[16], Sentries->entries[2].val, Sentries->entries[1].val, Sentries->entries[7].val);
     else
       printSrbTop("D","View Metadata Describing Entity", Sentries->tmpCollName, Sentries->tmpDataName, Sentries->entries[7].val);
     strcpy(annoCollName, Sentries->entries[2].val);
     strcpy(annoDataName, Sentries->entries[1].val);
     i = getDatasetMetaNew(Sentries);
     strcpy(Sentries->entries[2].val,annoCollName);
     strcpy(Sentries->entries[1].val,annoDataName);
     if (Sentries->showMetaType == 0) 
       getDatasetAnno(Sentries, annoCollName,annoDataName,0);
   }
   /*   printSrbBot(NULL,"Done","View Hidden (System) Metadata","Add Commentary");*/
   printSrbBot(NULL,NULL,NULL,NULL);

}


int 
modifyMetaForm(inStruct Sentries)
{

   if (strlen( Sentries->entries[1].val) == 0 )   {
     getCollectionMeta(Sentries, 1);
   }
   else {
     getDatasetMeta(Sentries, 1);
   }
}


int
makeDCMetaPage(inStruct Sentries)
{
     if (Sentries->entries[1].val && strcmp(Sentries->entries[1].val, "")) {
        getDatasetDC(Sentries, 0); 
     }
     else {
         getCollectionDC(Sentries, 0);    
     }
}


int
makeDeleteMetaForm(inStruct Sentries)
{
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE></HEAD><body background=\"%s/srb3.jpg\"><CENTER>\n", "MYSRB V9.0", webPath);
  printf("<TABLE><TR><TD><B><FONT COLOR=#0000FF>%s %s/%s</FONT></B></TD></TR>\n", mySrbStrings[227],
	 Sentries->entries[2].val,Sentries->entries[1].val);
  printf("<TR><TD align=center><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"deletemeta\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"seek\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
  printf("<input type=hidden name=\"owner\" value=\"%s\">",Sentries->entries[7].val);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[228]);
   printf("<B><FONT COLOR=#0000FF>%s</FONT></B></TD></TR></TABLE></body></html>\n", mySrbStrings[229]);
  return 0;
}

int
makeRemoveDataCollForm(inStruct Sentries)
{
   
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE></HEAD><body background=\"%s/srb3.jpg\"><CENTER>\n", "MYSRB V9.0", webPath);
  printf("<TABLE><TR><TD><B><FONT COLOR=#0000FF>%s %s/%s</FONT></B></TD></TR>\n", mySrbStrings[244], Sentries->entries[2].val, Sentries->entries[1].val);
  printf("<TR><TD align=center><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"removeDataColl\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"seek\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
  printf("<input type=hidden name=\"owner\" value=\"%s\">",Sentries->entries[7].val);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[228]);
  printf("<B><FONT COLOR=#0000FF>%s</FONT></B></TD></TR></TABLE></body></html>\n", mySrbStrings[229]);

  return 0;
}

int
makeSlinksForm(inStruct Sentries)
{
    char parColl[MAX_TOKEN], collName[MAX_TOKEN];

  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);

  printf("<TABLE><TR><TD><B><FONT COLOR=#0000FF>Soft Link</FONT></B></TD></TR>\n");
  printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n",mySrbStrings[26], Sentries->entries[2].val);
  if (strlen(Sentries->entries[1].val) > 0) 
    printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n", mySrbStrings[191],
	   Sentries->entries[1].val);
  printf("<TR><TD><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"slinks\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"rsrc\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
  
  getAccsCollectionsAsSelect(Sentries, "all",NULL);
  printf("<STRONG><FONT COLOR=#0000FF>%s</FONT></STRONG>%s\n", mySrbStrings[235], Sentries->selectData); /* 7 */
  printf("<BR><STRONG><FONT COLOR=#0000FF>Target Name In Collection:</FONT></STRONG>\n");
  clSplitbychar (Sentries->entries[2].val, parColl, collName, '/');
  if (strlen(Sentries->entries[1].val) > 0) 
    printf("<input type=text name=newdname value=\"%s\">\n", Sentries->entries[1].val);
  else
    printf("<input type=text name=newdname value=\"%s\">\n", collName);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"Link\"></FORM></TD></TR></TABLE>\n");
  return 0;
}

int
makeSlinkCollForm(inStruct Sentries)
{
    char parColl[MAX_TOKEN], collName[MAX_TOKEN];

  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);

  printf("<TABLE><TR><TD><B><FONT COLOR=#0000FF>Soft Link</FONT></B></TD></TR>\n");
  printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n",mySrbStrings[26], Sentries->entries[2].val);
  if (strlen(Sentries->entries[1].val) > 0) 
    printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n", mySrbStrings[191],
	   Sentries->entries[1].val);
  printf("<TR><TD><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"slinkColl\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"rsrc\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
  
  getAccsCollectionsAsSelect(Sentries, "all",NULL);
  printf("<STRONG><FONT COLOR=#0000FF>%s</FONT></STRONG>%s\n", mySrbStrings[235], Sentries->selectData); /* 7 */
  printf("<BR><STRONG><FONT COLOR=#0000FF>Target Name In Collection:</FONT></STRONG>\n");
  clSplitbychar (Sentries->entries[2].val, parColl, collName, '/');
  if (strlen(Sentries->entries[1].val) > 0) 
    printf("<input type=text name=newdname value=\"%s\">\n", Sentries->entries[1].val);
  else
    printf("<input type=text name=newdname value=\"%s\">\n", collName);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"Link\"></FORM></TD></TR></TABLE>\n");
  return 0;
}

int
makeRenamedsForm(inStruct Sentries)
{
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE></HEAD><body background=\"%s/srb3.jpg\">\n", "MYSRB V9.0", webPath);
  printf("<TABLE><TR><TD><B><FONT COLOR=#0000FF>%s</FONT></B></TD></TR>\n", mySrbStrings[231]);
  printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n",mySrbStrings[26], Sentries->entries[2].val);
  if (strlen(Sentries->entries[1].val) > 0)
  printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n", mySrbStrings[191], Sentries->entries[1].val);
  printf("<TR><TD><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"renameds\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<INPUT NAME=\"newname\", VALUE= \"\", SIZE=80,12>\n");
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM></TD></TR></TABLE></body></html>\n", mySrbStrings[211]);
  return 0;
}

int
makeMovedsForm(inStruct Sentries)
{
    char contSelect[MAX_TOKEN];

  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);

  printf("<TABLE><TR><TD><B><FONT COLOR=#0000FF>%s</FONT></B></TD></TR>\n", mySrbStrings[241]);
  printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n",mySrbStrings[26],
	 Sentries->entries[2].val);
  if (strlen(Sentries->entries[1].val) > 0) {
    printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n", mySrbStrings[191], Sentries->entries[1].val);
    printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n",mySrbStrings[72],
	   Sentries->entries[3].val);
  }
  printf("<TR><TD><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"moveds\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"rsrc\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
  getAccsCollectionsAsSelect(Sentries, "all",NULL);                  /** coll 7 **/
  printf("<STRONG><FONT COLOR=#0000FF>%s</FONT></STRONG>%s\n", mySrbStrings[235],Sentries->selectData); /* 7 */
  if (strlen(Sentries->entries[1].val) > 0) {
    printf("<BR><STRONG><FONT COLOR=#0000FF>%s</FONT></STRONG>\n", mySrbStrings[236]);
    printf("<input type=text name=newdname value=\"%s\">\n", Sentries->entries[1].val);
    getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData,Sentries->entries[3].val,"#sdsc-fs#sdsc-mda18-fs#");
    printf("<BR><STRONG><FONT COLOR=#0000FF>%s</STRONG></FONT>%s \n", mySrbStrings[237],Sentries->selectData);
    sprintf(contSelect, " like '%%/%s.%s/%%'",Sentries->srbUser,Sentries->userDomain);
    getValuesAsSelectWithCond(Sentries, CONTAINER_NAME, "","",contSelect);
    printf("<BR><STRONG><FONT COLOR=#0000FF>%s</STRONG></FONT>%s %s\n", mySrbStrings[238],Sentries->selectData, mySrbStrings[239]);
  }
  else {
    printf("<input type=hidden name=newdname value=\"\">\n");  /* 8 */
    printf("<input type=hidden name=newrsrc value=\"\">\n");   /* 9 */
    printf("<input type=hidden name=newcont value=\"\">\n");   /* 10 */
  }
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM></TD></TR></TABLE>\n", mySrbStrings[241]);
  return 0;
}


int
copymetaform(inStruct Sentries)
{
  char *newColl;
  
  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);
  if (strlen(Sentries->entries[1].val) > 0) {
    if ( Sentries->m > DATAFORMNUMOFARGS)
      newColl = Sentries->entries[Sentries->m-1].val;
    else
      newColl = Sentries->entries[2].val;
  }
  else {
    if ( Sentries->m > COLLFORMNUMOFARGS)
      newColl = Sentries->entries[Sentries->m-1].val;
    else
      newColl = Sentries->entries[2].val;
    
  }
  if (strlen(Sentries->entries[1].val) > 0) 
    printf("<CENTER><B><FONT COLOR=#0000FF>%s</FONT><FONT COLOR=#FF0000>%s/%s</FONT></B></CENTER>\n", mySrbStrings[216], Sentries->entries[2].val,Sentries->entries[1].val);
  else
    printf("<CENTER><B><FONT COLOR=#0000FF>Copy Metadata from Collection:</FONT><FONT COLOR=#FF0000>%s</FONT></B></CENTER>\n",Sentries->entries[2].val);
  i = getDataNamesinCollection(Sentries, newColl); 
  if (i < 0) {
    printf("<STRONG><FONT COLOR=#FF0000>No Data Objects  Found in Collection:</FONT><FONT COLOR=#0000FF> %s</FONT>\n",newColl); 
    return(0);
  }
  printf("<TABLE><TR><TD>\n");
  printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"copymeta\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"rsrc\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
  printf("<input type=hidden name=\"owner\" value=\"%s\">",Sentries->entries[7].val);
  printf("<input type=hidden name=\"newcoll\", value=\"%s\">",newColl); /*  8 */
  printf("<STRONG><FONT COLOR=#FF0000>%s</FONT></STRONG> %s</TD>\n", mySrbStrings[217], Sentries->selectData); /* 9 */
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);fflush(stdout);
  printf("<TR><TD ALIGN=CENTER><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM></TD></TR>\n", mySrbStrings[221]);fflush(stdout);
  if ( (strlen(Sentries->entries[1].val) > 0 && Sentries->m == DATAFORMNUMOFARGS) ||(Sentries->m == COLLFORMNUMOFARGS) ) {
    printf("<TR><TD> <HR SIZE=3 NOSHADE><STRONG>%s</STRONG> </TD></TR>\n", mySrbStrings[218]);
    printf("<TR><TD><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
    printf("<input type=hidden name=\"function\" value=\"copymeta\">");
    printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
    printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
    printf("<input type=hidden name=\"rsrc\" value=\"%s\">",Sentries->entries[3].val);
    printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
    printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
    printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
    printf("<input type=hidden name=\"owner\" value=\"%s\">",Sentries->entries[7].val);
    getAccsCollectionsAsSelect(Sentries, "all",NULL);                 /*   8 **/
    printf("<STRONG><FONT COLOR=#FF0000>%s</FONT></STRONG> %s</TD>\n", mySrbStrings[219], Sentries->selectData);
    printf("<input type=hidden name=newdname value=\"\">\n");  /* 9 */
    printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
    printf("<TR><TD  ALIGN=CENTER><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM></TD></TR>\n", mySrbStrings[222]);fflush(stdout);
    printf("<TR><TD> <HR SIZE=3 NOSHADE> <STRONG>%s</STRONG> </TD></TR>\n", mySrbStrings[218]);
    printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
    printf("<input type=hidden name=\"function\" value=\"copymetaform\">");
    printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
    printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
    printf("<input type=hidden name=\"rsrc\" value=\"%s\">",Sentries->entries[3].val);
    printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
    printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
    printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
    printf("<input type=hidden name=\"owner\" value=\"%s\">",Sentries->entries[7].val);
    printf("<TR><TD><STRONG><FONT COLOR=#FF0000>%s</FONT></STRONG> %s</TD>\n", mySrbStrings[220],
	   Sentries->selectData);fflush(stdout);
    printf("<input type=hidden name=newdname value=\"\">\n");  /* 9 */
    printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
    printf("<TR><TD  ALIGN=CENTER><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM></TD></TR>\n", mySrbStrings[223]);
  }

  printf("</TABLE>\n");
  return 0;
}

int
makeCopydsForm(inStruct Sentries)
{
  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);

  printf("<TABLE><TR><TD><B><FONT COLOR=#0000FF>%s</FONT></B></TD></TR>\n", mySrbStrings[107]);
  printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n",mySrbStrings[26], Sentries->entries[2].val);
  if (strlen(Sentries->entries[1].val) > 0) {
    printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n",mySrbStrings[191],Sentries->entries[1].val);
    printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n",mySrbStrings[72],
	   Sentries->entries[3].val);
  }

  printf("<TR><TD><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"copyds\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"rsrc\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
  getAccsCollectionsAsSelect(Sentries, "all",NULL);   /*   7 **/
  printf("<STRONG><FONT COLOR=#0000FF>%s</FONT></STRONG>%s\n", mySrbStrings[235], Sentries->selectData);
 
  if (strlen(Sentries->entries[1].val) > 0) {
    printf("<BR><STRONG><FONT COLOR=#0000FF>%s</FONT></STRONG>\n", mySrbStrings[236]);
    printf("<input type=text name=newdname value=\"%s\">\n", Sentries->entries[1].val);
    getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData, Sentries->entries[3].val,"#sdsc-fs#sdsc-mda18-fs#");
    printf("<BR><STRONG><FONT COLOR=#0000FF>%s</STRONG></FONT>%s\n", mySrbStrings[237], Sentries->selectData);
    getValuesAsSelect(Sentries, CONTAINER_NAME, Sentries->selectData,"","");
    printf("<BR><STRONG><FONT COLOR=#0000FF>%s</STRONG></FONT>%s %s\n", mySrbStrings[238], Sentries->selectData, mySrbStrings[239]);
  }
  else {
    printf("<input type=hidden name=newdname value=\"\">\n");  /* 8 */
    printf("<input type=hidden name=newrsrc value=\"\">\n");   /* 9 */
    printf("<input type=hidden name=newcont value=\"\">\n");   /* 10 */
  }
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM></TD></TR></TABLE>\n", mySrbStrings[240]);
  return 0;
}

int 
makeCommInsertForm(inStruct Sentries)
{

  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);
  
  if (isDataInSRB(Sentries) !=  1) {
    printf("Data Object %s in %s Not Found\n",Sentries->entries[1].val,Sentries->entries[2].val);
    return 0;
  }
  printf("<TABLE><TR><TD><B><FONT COLOR=#0000FF>%s</FONT></B></TD></TR>\n", mySrbStrings[205]);
  printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n",mySrbStrings[26],
	 Sentries->entries[2].val);
  if (strlen(Sentries->entries[1].val) > 0)
    printf("<TR><TD><FONT COLOR=#FF0000>%s</FONT> %s</TD></TR>\n",mySrbStrings[191], Sentries->entries[1].val);
  printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"insertDataComments\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<TR><TD><INPUT NAME=\"comments\", VALUE= \"\", SIZE=80,12></TD></TR>\n");
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM></TABLE>\n", mySrbStrings[204]);
  return 0;
}


int
makeDCMetaInsertForm(inStruct Sentries)
{

  char tmpStr[MAX_TOKEN * 2];
  int  isColl = 0;
  char *tmpPtr,*tmpPtr1;

  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);
  if ((tmpPtr= strstr(Sentries->entries[1].val,"@SHADOW=")) != NULL)
      *tmpPtr = '\0';

  if (strlen( Sentries->entries[1].val) == 0 )   {
    /*** collection only ***/
    if (isCollInSRB(Sentries) !=  1) {
      printf("Collection %s Not Found\n",Sentries->entries[2].val);
      bRMap(1,0);
      return 0;
    }
    printf("<B><FONT COLOR=#0000FF>Enter Dublin Core Metadata for Collection: </FONT>%s</B>\n",Sentries->entries[2].val);
    printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
    printf("<input type=hidden name=\"function\" value=\"insertCollMetaData\">");
    printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
    strcpy(tmpStr,"Metadata for Data");
  }
  else {
    if (isDataInSRB(Sentries) !=  1) {
      printf("Data Object %s in %s Not Found\n",Sentries->entries[1].val,Sentries->entries[2].val);
      bRMap(1,0);
      return 0;
    }
    printf("<B><FONT COLOR=#0000FF>%s</FONT> %s <FONT COLOR=#0000FF>%s</FONT> %s</B>\n", mySrbStrings[202],
	   Sentries->entries[1].val, mySrbStrings[198],Sentries->entries[2].val);
    printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
    printf("<input type=hidden name=\"function\" value=\"insertDataMetaData\">");
    printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
    printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
    if (atoi(Sentries->entries[6].val) < 0) {
      if ((tmpPtr = strstr(Sentries->entries[5].val,"/?SHADOW")) != NULL) {
	tmpPtr += 8;
	if (atoi(Sentries->entries[6].val) == -1)
	  printf("<input type=hidden name=\"slinkval\" value=\"F:%s\">",tmpPtr);
	else
	  printf("<input type=hidden name=\"slinkval\" value=\"D:%s\">",tmpPtr);

      }
      else
	printf("<input type=hidden name=\"slinkval\" value=\"\">");
    }
    else
      printf("<input type=hidden name=\"slinkval\" value=\"\">");
    strcpy(tmpStr,"");
  }

  

    printf("<table><tr><th align=center>%s</th><th></th><th align=center>%s</th><th align=center>Value Type</th></tr>\n", mySrbStrings[77], mySrbStrings[78]);

    for (i = 0; ; i++) {
      if (!strcmp(dubcoreStr[i][0],"DC.NULL"))
	break;
      printf("<tr><td><FONT COLOR=#FF0000>%s: </FONT>\
<INPUT NAME=\"newmdname%i\" TYPE=hidden value=\"%s\"></td><td><b>=</b></td>",
	     dubcoreStr[i][0],i,dubcoreStr[i][0]);
      strcpy(tmpStr,dubcoreStr[i][1]);
      if((tmpPtr = strstr(tmpStr,",")) != NULL) {
	*tmpPtr = '\0';
	printf ("<td><SELECT SIZE=1 NAME=newmdval%i><OPTION SELECTED>%s",
		i,tmpStr);
	tmpPtr1 =  tmpPtr + 1;
	while(( tmpPtr = strstr(tmpPtr1,",")) != NULL) {
	  *tmpPtr = '\0';
	  printf ("<OPTION>%s",tmpPtr1);
	  tmpPtr1 =  tmpPtr + 1;
	}
	printf("<OPTION>%s</SELECT>\n",tmpPtr1);
      }
      else {
	printf("<td><INPUT NAME=\"newmdval%i\" VALUE= \"%s\" SIZE=20,12></td>",
	       i,tmpStr );
      }
      strcpy(tmpStr,dubcoreStr[i][2]);
      if((tmpPtr = strstr(tmpStr,",")) != NULL) {
	*tmpPtr = '\0';
	printf ("<td><SELECT SIZE=1 NAME=newvalunit%i><OPTION SELECTED>%s",
		i,tmpStr);
	tmpPtr1 =  tmpPtr + 1;
	while(( tmpPtr = strstr(tmpPtr1,",")) != NULL) {
	  *tmpPtr = '\0';
	  printf ("<OPTION>%s",tmpPtr1);
	  tmpPtr1 =  tmpPtr + 1;
	}
	printf("<OPTION>%s</SELECT>\n",tmpPtr1);
      }
      else {
	printf("<td><INPUT NAME=\"newmvalunit%i\" VALUE= \"%s\" SIZE=20,12></td>",
	       i,tmpStr );
      }
      printf("<INPUT type=hidden  NAME=\"newdefval%i\", VALUE= \"\", SIZE=16,12>\
<INPUT type=hidden  NAME=\"newcomm%i\", VALUE= \"\", SIZE=16,12><td>%s</td></tr>\n",
	    i,i,dubcoreStr[i][3]);
    }
    
    printf("</table>\n");
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[201]);
  return 0;
}
/**
int
metaExtractForm()
{
  int i;

  printf("<B><FONT COLOR=#0000FF>Extract Metadata Info for Data Object:</FONT> %s <FONT COLOR=#0000FF>in Collection:</FONT> %s</B>\n",
	   entries[1].val,entries[2].val); fflush(stdout);
  if (strstr(entries[1].val,"&"))
     escape_url(entries[1].val);
  if (strstr(entries[2].val,"&"))
     escape_url(entries[2].val);
  unescape_url(entries[5].val);
  if (strstr(entries[5].val,":") != NULL)
    i=getExtractStyleForm(selectData,"html");
  else
    i=getExtractStyleForm(selectData,entries[5].val);
  if (i < 0) {
    printf("No Extraction Method Found for the Data Type of the chosen  SRB Object\n");
    return(-1);
  }
  printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"metaextract\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",entries[2].val);
  printf("<BR><STRONG><FONT COLOR=#0000FF>Extraction Metod to Use: </FONT></STRONG>%s\n",selectData); 
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"Extract\"></FORM>\n");
  return 0;
}
**/

int
metaExtractForm(inStruct Sentries)
{
  int i;

  printf("<B><FONT COLOR=#0000FF>%s</FONT> %s <FONT COLOR=#0000FF>%s</FONT> %s</B>\n", mySrbStrings[192], Sentries->entries[1].val, mySrbStrings[198], Sentries->entries[2].val); 
  fflush(stdout);
  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);
  unescape_url(Sentries->entries[5].val);
  if (strstr(Sentries->entries[5].val,":") != NULL)
    i=getExtractStyleForm(Sentries, Sentries->selectData,"html");
  else
    i=getExtractStyleForm(Sentries, Sentries->selectData,Sentries->entries[5].val);
  if (i < 0) {
    printf("%s\n", mySrbStrings[199]);
    return(-1);
  }
  printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"metaextractform1\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<BR><STRONG><FONT COLOR=#0000FF>Extraction Metod to Use: </FONT></STRONG>%s\n", Sentries->selectData); 
  getAccsCollectionsAsSelect(Sentries, "all","NULL");
  printf("<BR><STRONG><FONT COLOR=#0000FF>Do you need Metadata Extracted From Another Dataset<BR> If so choose a collection name for identifying the dataset: </FONT></STRONG>%s\n", Sentries->selectData); 
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"Extract\"></FORM>\n");
  return 0;
}

int
metaExtractForm1(inStruct Sentries)
{
  int i;

  printf("<B><FONT COLOR=#0000FF>%</FONT> %s <FONT COLOR=#0000FF>%s</FONT> %s</B>\n", mySrbStrings[192], Sentries->entries[1].val, mySrbStrings[198], Sentries->entries[2].val); 
  fflush(stdout);
  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);
  if (strstr(Sentries->entries[3].val,"&"))
     escape_url(Sentries->entries[3].val);
  printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"metaextract\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"extmethod\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"metacoll\" value=\"%s\">",Sentries->entries[4].val);
  getDataNamesinCollection(Sentries, Sentries->entries[4].val); 
  printf("<BR><STRONG><FONT COLOR=#0000FF>Select dataset holding Metadata: </FONT></STRONG>%s\n",selectData); 
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"Extract\"></FORM>\n");
  return 0;
}

int
makeSQLDataFormPage(inStruct Sentries){

    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<HTML><HEAD> <TITLE> %s </TITLE> ", "MYSRB V9.0");
    printf("<body background=\"%s/srb3.jpg\">", webPath);
    printf("<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\">\n");
    
    if (Sentries->isTicket)
    printf("<input type=hidden name=\"function\" value=\"ticketsqldata\">");    
    else
    printf("<input type=hidden name=\"function\" value=\"sqldata\">");
    
    printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
    printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
    printf("<input type=hidden name=\"seek\" value=\"%s\">",Sentries->entries[3].val);
    printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
    printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
    printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
    printf("<input type=hidden name=\"owner\" value=\"%s\">",Sentries->entries[7].val);
    printf("<B><FONT COLOR=#0000FF>Query Value: </FONT></B><INPUT NAME=\"shadow\", VALUE= \"\", SIZE=\"40,12\"> %s\n", mySrbStrings[239]);
    printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->entries[8].val);
    printf("<BR><INPUT TYPE=\"submit\" VALUE=\"Submit\"></FORM>\n");
    printf("</body></html>");
    return 0;
}

int
makeExecDataFormPage(inStruct Sentries){
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
   printf ("<HTML><HEAD> <TITLE> %s </TITLE> ", "MYSRB V9.0");
   printf("<body background=\"%s/srb3.jpg\">", webPath);

    printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
    printf("<input type=hidden name=\"function\" value=\"execdata\">");
    printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
    printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
    printf("<input type=hidden name=\"seek\" value=\"%s\">",Sentries->entries[3].val);
    printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
    printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
    printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
    printf("<input type=hidden name=\"owner\" value=\"%s\">",Sentries->entries[7].val);
    printf("<B><FONT COLOR=#0000FF>%s </FONT></B><INPUT NAME=\"shadow\", VALUE= \"\", SIZE=40,12> %s\n", mySrbStrings[263], mySrbStrings[239]);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->entries[8].val);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"Submit\"></FORM>\n");
  printf("</body></html>");
  return 0;
}

int 
makeMetaInsertForm(inStruct Sentries)
{

  char tmpStr[MAX_TOKEN];
  int  isColl = 0;

  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);
  
  if (strlen( Sentries->entries[1].val) == 0 )   {
    /*** collection only ***/
    if (isCollInSRB(Sentries) !=  1) {
      printf("Collection %s Not Found\n",Sentries->entries[2].val);
      bRMap(1,0);
      return 0;
    }
    printf("<B><FONT COLOR=#0000FF>Enter Metadata for Collection: </FONT>%s</B>\n",Sentries->entries[2].val);
    printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
    printf("<input type=hidden name=\"function\" value=\"insertCollMetaData\">");
    printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
    strcpy(tmpStr,"Metadata for Data");
    isColl  =1;
  }
  else {
    if ((tmpPtr= strstr(Sentries->entries[1].val,"@SHADOW=")) != NULL)
      *tmpPtr = '\0';
    if (isDataInSRB(Sentries) !=  1) {
      printf("Data Object %s in %s Not Found\n",Sentries->entries[1].val,Sentries->entries[2].val);
      bRMap(1,0);
      return 0;
    }
    printf("<B><FONT COLOR=#0000FF>%s</FONT> %s <FONT COLOR=#0000FF>%s</FONT> %s</B>\n", mySrbStrings[200],
	   Sentries->entries[1].val, mySrbStrings[198], Sentries->entries[2].val);
    printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
    printf("<input type=hidden name=\"function\" value=\"insertDataMetaData\">");
    printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
    printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
    if (atoi(Sentries->entries[6].val) < 0) {
      if ((tmpPtr = strstr(Sentries->entries[5].val,"/?SHADOW")) != NULL) {
	tmpPtr += 8;
	if (atoi(Sentries->entries[6].val) == -1)
	  printf("<input type=hidden name=\"slinkval\" value=\"F:%s\">",tmpPtr);
	else
	  printf("<input type=hidden name=\"slinkval\" value=\"D:%s\">",tmpPtr);

      }
      else
	printf("<input type=hidden name=\"slinkval\" value=\"\">");
    }
    else
      printf("<input type=hidden name=\"slinkval\" value=\"\">");
    strcpy(tmpStr,"");
  }
  if (isColl) {
    printf("<table><tr><th align=center>%s</th><th></th><th align=center>%s</th><th align=center>%s</th><th align=center>%s</th><th align=center>%s</th></tr>\n", mySrbStrings[77], mySrbStrings[78], mySrbStrings[62], mySrbStrings[64], mySrbStrings[63]);
    printf("<tr><td><INPUT NAME=\"newmdname1\", VALUE= \"\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval1\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit1\", VALUE= \"\", SIZE=10,10></td><td><INPUT  NAME=\"newdefval1\", VALUE= \"\", SIZE=16,12></td><td><INPUT   NAME=\"newcomm1\", VALUE= \"\", SIZE=16,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname2\", VALUE= \"\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval2\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit2\", VALUE= \"\", SIZE=10,10></td><td><INPUT   NAME=\"newdefval2\", VALUE= \"\", SIZE=16,12></td><td><INPUT  NAME=\"newcomm2\", VALUE= \"\", SIZE=16,12></td>\n\
<tr><td><INPUT NAME=\"newmdname3\", VALUE= \"%s\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval3\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit3\", VALUE= \"\", SIZE=10,10></td><td><INPUT   NAME=\"newdefval3\", VALUE= \"\", SIZE=16,12></td><td><INPUT   NAME=\"newcomm3\", VALUE= \"\", SIZE=16,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname4\", VALUE= \"%s\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval4\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit4\", VALUE= \"\", SIZE=10,10></td><td><INPUT   NAME=\"newdefval4\", VALUE= \"\", SIZE=16,12></td><td><INPUT   NAME=\"newcomm4\", VALUE= \"\", SIZE=16,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname5\", VALUE= \"%s\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval5\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit5\", VALUE= \"\", SIZE=10,10></td><td><INPUT   NAME=\"newdefval5\", VALUE= \"\", SIZE=16,12></td><td><INPUT   NAME=\"newcomm5\", VALUE= \"\", SIZE=16,12></td></tr>\n\
</table>\n",tmpStr,tmpStr,tmpStr);
    printf("%s\n", mySrbStrings[170]);
  }
  else {
    printf("<table><tr><th align=center>%s</th><th></th><th align=center>%s</th><th align=center>%s</th></tr>\n", mySrbStrings[77], mySrbStrings[78], mySrbStrings[62]);
    printf("<tr><td><INPUT NAME=\"newmdname1\", VALUE= \"\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval1\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit1\", VALUE= \"\", SIZE=10,10></td><td><INPUT type=hidden  NAME=\"newdefval1\", VALUE= \"\", SIZE=16,12></td><td><INPUT type=hidden  NAME=\"newcomm1\", VALUE= \"\", SIZE=16,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname2\", VALUE= \"\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval2\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit2\", VALUE= \"\", SIZE=10,10></td><td><INPUT type=hidden  NAME=\"newdefval2\", VALUE= \"\", SIZE=16,12></td><td><INPUT type=hidden  NAME=\"newcomm2\", VALUE= \"\", SIZE=16,12></td>\n\
<tr><td><INPUT NAME=\"newmdname3\", VALUE= \"%s\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval3\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit3\", VALUE= \"\", SIZE=10,10></td><td><INPUT type=hidden  NAME=\"newdefval3\", VALUE= \"\", SIZE=16,12></td><td><INPUT type=hidden  NAME=\"newcomm3\", VALUE= \"\", SIZE=16,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname4\", VALUE= \"%s\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval4\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit4\", VALUE= \"\", SIZE=10,10></td><td><INPUT type=hidden  NAME=\"newdefval4\", VALUE= \"\", SIZE=16,12></td><td><INPUT type=hidden  NAME=\"newcomm4\", VALUE= \"\", SIZE=16,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname5\", VALUE= \"%s\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval5\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit5\", VALUE= \"\", SIZE=10,10></td><td><INPUT type=hidden  NAME=\"newdefval5\", VALUE= \"\", SIZE=16,12></td><td><INPUT type=hidden  NAME=\"newcomm5\", VALUE= \"\", SIZE=16,12></td></tr>\n\
</table>\n",tmpStr,tmpStr,tmpStr);
  }

  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[201]);
  return 0;
}



int
makeOtherInfoMenuPage(inStruct Sentries)
{
    int i;
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf ("<HTML><HEAD> <TITLE> %s </TITLE></HEAD><body background=\"%s/srb3.jpg\"><CENTER>\n", mySrbStrings[182], webPath);
    printf ("<TABLE><TR><TD><OL>\n");
    printf("<LI><A href=\"mysrb331.cgi?function=basicresourceinfo&data=%s&coll=%s&session=%s\" TARGET=\"rtbtbot\">%s</A>\n",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val, mySrbStrings[178]);
    printf("<LI><A href=\"mysrb331.cgi?function=basicuserinfo&data=%s&coll=%s&session=%s\" TARGET=\"rtbtbot\">%s</A>\n",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val, mySrbStrings[179]);
    printf("<LI><A href=\"mysrb331.cgi?function=basicalluserinfo&data=%s&coll=%s&session=%s\" TARGET=\"rtbtbot\">%s</A>\n",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val, mySrbStrings[180]);
    printf("<LI><A href=\"mysrb331.cgi?function=usercontainerinfo&data=%s&coll=%s&session=%s\" TARGET=\"rtbtbot\">%s</A>\n",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val, mySrbStrings[181]);
    
    printf ("</OL></TD></TR></TABLE>\n");
    printf ("</body></html>\n");

}
int
makeOtherInfoPage(inStruct Sentries)
{
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD><FRAMESET rows=\"40%,60%\" BORDER=no>", mySrbStrings[182]);
    printf("<FRAME SRC=\"mysrb331.cgi?function=otherinfomenu&data=%s&coll=%s&session=%s\" name=\"rtbttop\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val);
    printf("<FRAME SRC=\"%s/srbempty.html\" name=\"rtbtbot\">", webPath);
    printf("</FRAMESET> </body></html>");

}

int
makeDCInsertPage(inStruct Sentries)
{
   int i;

   if (strstr(Sentries->entries[1].val,"&"))
	escape_url(Sentries->entries[1].val);
printf("Content-type: text/html%c%c",10,10);fflush(stdout);

   printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"sub%s\"> </HEAD><FRAMESET rows=\"40%,60%\" BORDER=no>", "MYSRB V9.0", Sentries->pidStr);
   printf("<FRAME SRC=\"mysrb331.cgi?function=dcmeta&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\" name=\"rtbttop\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[Sentries->m].val);
   printf("<FRAME SRC=\"mysrb331.cgi?function=dcmetainsertionform&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\" name=\"rtbtbot\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[Sentries->m].val);
   printf("</FRAMESET> </body></html>");


}

int
makeMetaInsertPage(inStruct Sentries)
{
   int i;

   if (strstr(Sentries->entries[1].val,"&"))
	escape_url(Sentries->entries[1].val);
   printf("Content-type: text/html%c%c",10,10);fflush(stdout);

   printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD><FRAMESET rows=\"40%,60%\" BORDER=no>", "MYSRB V9.0");
   printf("<FRAME SRC=\"mysrb331.cgi?function=meta&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\" name=\"rtbttop\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[Sentries->m].val);
   printf("<FRAME SRC=\"mysrb331.cgi?function=metainsertionform&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\" name=\"rtbtbot\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[Sentries->m].val);
   printf("</FRAMESET> </body></html>");
}

int
makeContainerForm(inStruct Sentries)
{
  int i;
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"sub%s\"> </HEAD>", mySrbStrings[173], Sentries->entries[1].val);
  printf("<body background=\"%s/srb3.jpg\">", webPath);
  printf ("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"  >\n");
  printf("<input type=hidden name=\"function\" value=\"makecontainer\">");
    printf("<TABLE>\n");
    printf("<TR><TD align=RIGHT><STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[174]);
    printf("</TD><TD><INPUT  NAME=\"contcoll\" VALUE= \"/container/%s.%s\" SIZE=40,12 >", Sentries->srbUser, Sentries->userDomain );
    printf("</TD></TR><TR><TD align=RIGHT>\n");
    printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT> \n", mySrbStrings[168]);
    printf("</TD><TD><INPUT  NAME=\"contname\"  VALUE= \"\"  SIZE=40,12>");
    printf("</TD></TR><TR><TD align=RIGHT>\n");
    printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT> \n", mySrbStrings[175]);
    printf("</TD><TD><INPUT  NAME=\"contsize\"  VALUE= \"%i\"  SIZE=40,12>",DEF_CONTAINER_SIZE);
    printf("</TD></TR><TR><TD align=RIGHT>\n");
    printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT> \n", mySrbStrings[176]);
    getValuesAsSelect(Sentries, DATA_TYP_NAME, Sentries->selectData,"generic","");
    printf("</TD><TD>%s\n", Sentries->selectData);
    printf("</TD></TR><TR><TD align=RIGHT>\n");    
    printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT> \n",mySrbStrings[72]);
    getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData,"myContRsrc","#sdsc-fs#sdsc-mda18-fs#");
    printf("</TD><TD>%s\n", Sentries->selectData);
    printf("</TD></TR>\n");
    printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
    printf("<TR><TD></TD><TD align=LEFT><INPUT TYPE=\"submit\" VALUE=\"%s\">\n</FORM></TD></TR>\n",mySrbStrings[177]);
    printf("</TABLE></body></html>");
    
    return;


}
int 
makeSchemaQueryPage(inStruct Sentries)
{
   int i=0;
  char urlcollection[MAX_TOKEN];

  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD><body background=\"%s/srb3.jpg\"><H1>DICOM Schema Query</H1>", "MYSRB V9.0", webPath);
 printf ("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"  >\n");
  printf("<input type=hidden name=\"function\" value=\"schemaqueryvalues\">");
  printf("<table><tr><th align=right>%s</th><th align=right>%s</th><th></th><th align = left>%s</th></tr>\n", mySrbStrings[79], mySrbStrings[77], mySrbStrings[78]);
  printf("<tr></tr>\n");fflush(stdout);
  while (strcmp(dicomStr[i][0],"SCHEMAEND")) { 
    printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showSMDV%i\"></td><td  align=right><input type=hidden name=\"%s\" value=\"%s\">%s</td><td>%s</td><td><INPUT NAME=\"%s\", VALUE= \"\", SIZE=30,12></td></tr>\n",i,dicomStr[i][2],dicomStr[i][1],dicomStr[i][0],METACOMPLIST,dicomStr[i][0]);fflush(stdout);
    i++;
  }
  printf("</table>\n");
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\"><INPUT TYPE=\"reset\" VALUE=\"%s\"></FORM>\n", mySrbStrings[81], mySrbStrings[80]);

  printf("</body></html>");
  return;
}

int
makeIngestExtSchemaListing(inStruct Sentries)
{
  int i = 0;
  char urlcollection[MAX_TOKEN];
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE>", mySrbStrings[22]);
  printf ("<BASE TARGET = \"sub%s\"> </HEAD>", Sentries->pidStr);
  
  if (!strcmp(mode,"light")) {
            printf("<body bgcolor=\"#FFFFF7\">");
  }
  else {
            printf("<body background=\"%s/srb3.jpg\">", webPath);
  }
  printf ("<H2>%s</H2>",aaa[41]);
  fflush(stdout);
  getExtSchemaNameTableNameAsSelect(Sentries, Sentries->selectData,NULL);
  printf ("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"  >\n");
  printf("<input type=hidden name=\"function\" value=\"extschmtableattrlisting\">");
  strcpy(urlcollection,Sentries->entries[1].val);
  escape_url(urlcollection);
  printf("<input type=hidden name=\"data\" value=\"%s\">",urlcollection);
  strcpy(urlcollection,Sentries->entries[2].val);
  escape_url(urlcollection);
  printf("<input type=hidden name=\"collection\" value=\"%s\">",urlcollection);
  printf("<STRONG><FONT COLOR=#FF0000>%s:</FONT></STRONG><BR>\n", aaa[15]);
  printf("%s\n",Sentries->selectData);
  printf("<BR><FONT COLOR=#FF0000><STRONG>%s<input type=text name=\"numofrows\" size=2 value=\"5\">\n",
	 aaa[22]);
  printf("<BR>%s: <INPUT TYPE=\"radio\" name=\"selection\" value =\"ingest\" checked>",aaa[29]);
  printf("&nbsp;&nbsp;  %s: <INPUT TYPE=\"radio\" name=\"selection\" value =\"modify\" >",aaa[28]);
  printf("&nbsp;&nbsp;  %s: <INPUT TYPE=\"radio\" name=\"selection\" value =\"delete\" ></STRONG></FONT><BR>",aaa[34]);
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\"><BR>\n",aaa[3]);
  printf("<INPUT TYPE=\"reset\" VALUE=\"%s\"></FORM>\n", mySrbStrings[80]);
  printf("</body></html>");
  return;
}
int
makeExtSchemaListing(inStruct Sentries)
{
  int i = 0;
  char urlcollection[MAX_TOKEN];
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE>", mySrbStrings[22]);
  printf ("<BASE TARGET = \"sub%s\"> </HEAD>", Sentries->pidStr);
  
  if (!strcmp(mode,"light")) {
            printf("<body bgcolor=\"#FFFFF7\">");
  }
  else {
            printf("<body background=\"%s/srb3.jpg\">", webPath);
  }
  printf ("<H2>%s</H2>",aaa[1]);
  fflush(stdout);
  getExtSchemaNamesAsCheckBox(Sentries, Sentries->selectData,"");
  printf ("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"  >\n");
  printf("<input type=hidden name=\"function\" value=\"extschmattrlisting\">");
  strcpy(urlcollection,Sentries->entries[1].val);
  escape_url(urlcollection);
  printf("<input type=hidden name=\"collection\" value=\"%s\">",urlcollection);
  printf("<STRONG><FONT COLOR=#FF0000>%s:</FONT></STRONG><BR>\n", aaa[2]);
  printf("%s\n",Sentries->selectData);
  printf("<TABLE><TR><TD VALIGN=TOP><STRONG><FONT COLOR=#FF0000>%s<BR> </TD><TD NOWRAP><FONT COLOR=#0000FF> %s:</FONT><input type=radio name=\"isansdata\" value=\"browseyes\" checked> <BR><FONT COLOR=#0000FF> %s:</FONT><input type=radio name=\"isansdata\" value=\"yes\"> <BR><FONT COLOR=#0000FF> %s:</FONT><input type=radio name=\"isansdata\" value=\"no\"></TD></TR>\n",aaa[18],aaa[19],aaa[20],aaa[21]);
  printf("<TR><TD VALIGN=TOP><FONT COLOR=#FF0000><STRONG>%s</STRONG></FONT></TD> <TD><input type=text name=\"numofattr\" size=2 value=\"5\"></TD></TR></TABLE>\n",aaa[16]);
  printf("<input type=hidden name=\"session\" value=\"%s\"><br>", Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\"><INPUT TYPE=\"reset\" VALUE=\"%s\"></FORM>\n", aaa[3], mySrbStrings[80]);
  printf("</body></html>");
  return;
}

int 
makeDeleteExtSchemaTableAttributeListing(inStruct Sentries)
{
  int i = 0;
  char urlcollection[MAX_TOKEN];
  char *schemaList;
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE>", mySrbStrings[22]);
  printf ("<BASE TARGET = \"sub%s\"> </HEAD>", Sentries->pidStr);

  printf("<body background=\"%s/srb3.jpg\">", webPath);
  printf ("<H2>%s</H2>",aaa[36]);
  fflush(stdout);
  printf ("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"  >\n");
  printf("<input type=hidden name=\"function\" value=\"deleteExtSchmMetaData\">");
  strcpy(urlcollection,Sentries->entries[1].val);
  escape_url(urlcollection);
  printf("<B><FONT COLOR=#0000FF>%s:</FONT><input type=text name=\"data\" value=\"%s\">",mySrbStrings[200],urlcollection);
  strcpy(urlcollection,Sentries->entries[2].val);
  escape_url(urlcollection);
  printf("<FONT COLOR=#0000FF>%s</FONT><input type=text name=\"collection\" value=\"%s\">",
	 mySrbStrings[198],urlcollection);
  strcpy(urlcollection,Sentries->entries[3].val);
  escape_url(urlcollection);
  printf("<input type=hidden name=\"schematable\" value=\"%s\">",urlcollection);
  getExtSchemaTableAttrAsTabledInsert(Sentries, Sentries->selectData,Sentries->entries[3].val,
				      atoi(Sentries->entries[4].val), 2); /* 2 = delete */
  printf("%s\n",Sentries->selectData);
  printf("<input type=hidden name=\"session\" value=\"%s\"><br>", Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", aaa[39]);
  printf("</body></html>");
  return;
}

int
makeModifyExtSchemaTableAttributeListing(inStruct Sentries)
{
  int i = 0;
  char urlcollection[MAX_TOKEN];
  char *schemaList;
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE>", mySrbStrings[22]);
  printf ("<BASE TARGET = \"sub%s\"> </HEAD>", Sentries->pidStr);

  printf("<body background=\"%s/srb3.jpg\">", webPath);
  printf ("<H2>%s</H2>",aaa[35]);
  fflush(stdout);
  printf ("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"  >\n");
  printf("<input type=hidden name=\"function\" value=\"modifyExtSchmMetaData\">");
  strcpy(urlcollection,Sentries->entries[1].val);
  escape_url(urlcollection);
  printf("<B><FONT COLOR=#0000FF>%s:</FONT><input type=text name=\"data\" value=\"%s\">",mySrbStrings[200],urlcollection);
  strcpy(urlcollection,Sentries->entries[2].val);
  escape_url(urlcollection);
  printf("<FONT COLOR=#0000FF>%s</FONT><input type=text name=\"collection\" value=\"%s\">",
	 mySrbStrings[198],urlcollection);
  strcpy(urlcollection,Sentries->entries[3].val);
  escape_url(urlcollection);
  printf("<input type=hidden name=\"schematable\" value=\"%s\">",urlcollection);
  getExtSchemaTableAttrAsTabledInsert(Sentries, Sentries->selectData,Sentries->entries[3].val,
				      atoi(Sentries->entries[4].val), 1); /* 1 = modify */
  printf("%s\n",Sentries->selectData);
  printf("<input type=hidden name=\"session\" value=\"%s\"><br>", Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\"><INPUT TYPE=\"reset\" VALUE=\"%s\"></FORM>\n", aaa[38], aaa[37]);
  printf("</body></html>");
  return;
}

int
makeExtSchemaTableAttributeListing(inStruct Sentries)
{
  int i = 0;
  char urlcollection[MAX_TOKEN];
  char *schemaList;
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE>", mySrbStrings[22]);
  printf ("<BASE TARGET = \"sub%s\"> </HEAD>", Sentries->pidStr);

  printf("<body background=\"%s/srb3.jpg\">", webPath);
  printf ("<H2>%s</H2>",aaa[23]);
  fflush(stdout);
  printf ("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"  >\n");
  printf("<input type=hidden name=\"function\" value=\"ingestExtSchmMetaData\">");
  strcpy(urlcollection,Sentries->entries[1].val);
  escape_url(urlcollection);
  printf("<B><FONT COLOR=#0000FF>%s:</FONT><input type=text name=\"data\" value=\"%s\">",mySrbStrings[200],urlcollection);
  strcpy(urlcollection,Sentries->entries[2].val);
  escape_url(urlcollection);
  printf("<FONT COLOR=#0000FF>%s</FONT><input type=text name=\"collection\" value=\"%s\">",
	 mySrbStrings[198],urlcollection);
  strcpy(urlcollection,Sentries->entries[3].val);
  escape_url(urlcollection);
  printf("<input type=hidden name=\"schematable\" value=\"%s\">",urlcollection);
  getExtSchemaTableAttrAsTabledInsert(Sentries, Sentries->selectData,Sentries->entries[3].val,
				      atoi(Sentries->entries[4].val),0 ); /* 0 = ingest */
  printf("%s\n",Sentries->selectData);
  printf("<input type=hidden name=\"session\" value=\"%s\"><br>", Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\"><INPUT TYPE=\"reset\" VALUE=\"%s\"></FORM>\n", aaa[24], mySrbStrings[80]);
  printf("</body></html>");
  return;
}

int
makeExtSchemaAttributeListing(inStruct Sentries)
{
  int i = 0;
  char urlcollection[MAX_TOKEN];
  char *schemaList;
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE>", mySrbStrings[22]);
  printf ("<BASE TARGET = \"sub%s\"> </HEAD>", Sentries->pidStr);

  if (!strcmp(mode,"light")) {
            printf("<body bgcolor=\"#FFFFF7\">");
  }
  else {
            printf("<body background=\"%s/srb3.jpg\">", webPath);
  }
  printf ("<H2>%s</H2>",aaa[1]);
  fflush(stdout);
  schemaList = malloc(MAX_TOKEN * (Sentries->m - 1));
  sprintf(schemaList," '%s'", Sentries->entries[2].val);
  for (i = 3 ; i < Sentries->m - 1; i++) {
    strcat(schemaList,", '");
    strcat(schemaList,Sentries->entries[i].val);
    strcat(schemaList,"'");
  }
  getExtSchemaAttrListAsSelect(Sentries, Sentries->selectData,schemaList);
  printf ("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"  >\n");
  printf("<input type=hidden name=\"function\" value=\"extschemaattrvalues\">");
  printf("<input type=hidden name=\"schemalist\" value=\"%s\">",schemaList);
  printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[185]);
  printf("<table><tr><th align=right>%s</th><th align=center>%s</th><th></th><th align = center>%s</th></tr>\n", mySrbStrin\
gs[79], mySrbStrings[77], mySrbStrings[78]);
  printf("<tr></tr>\n");
  for (i = 0  ; i < atoi(Sentries->entries[Sentries->m].val) ; i++) {
    printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showESA%i\"></td><td>%s</td><td>%s</td><td><INPUT NAME=\"esaval%i\", VALUE= \"\", SIZE=20,12></td></tr>\n",
          Sentries->globalCounter++, Sentries->selectData,METACOMPLIST,i);
  }
  printf("</table><br>\n");
  if (!strcmp(Sentries->entries[Sentries->m - 1].val,"browseyes")) {
    addBrowseQuery(Sentries); 
  }
  else if (!strcmp(Sentries->entries[Sentries->m - 1].val,"no")) {
    printf("<input type=hidden name=\"isansdata\" value=\"no\"><br>");
  }
  printf("<input type=hidden name=\"session\" value=\"%s\"><br>", Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\"><INPUT TYPE=\"reset\" VALUE=\"%s\"></FORM>\n", mySrbStrings[81], mySrbStrings[80]);
  printf("</body></html>");
  return;
}

int
addBrowseQuery(inStruct Sentries)
{
  int i = 0;
  char urlcollection[MAX_TOKEN];



    getUDSMD0AsSelect(Sentries, Sentries->selectData,"", Sentries->entries[1].val);

  /**** need an empty field to allow browse query even if there is no Metadata ***/
  if (!strlen(Sentries->selectData)) {
   sprintf(Sentries->selectData, "<SELECT SIZE=1 NAME=c1><OPTION></SELECT>");
  }

  /*
  printf ("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"  >\n");
  printf("<input type=hidden name=\"function\" value=\"browsequeryvalues\">");
  */
  printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[185]);
  printf("<table><tr><th align=right>%s</th><th align=center>%s</th><th></th><th align=left >%s</th></tr>\n", mySrbStrings[79], mySrbStrings[77], mySrbStrings[78]);
  printf("<tr></tr>\n");
  for (i = 0  ; i < 5 ; i++) {
    printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showMDV%i\"></td><td>%s</td><td>%s</td><td><INPUT NAME=\"newmdval%i\", VALUE= \"\", SIZE=20,12></td></tr>\n",
	  Sentries->globalCounter++, Sentries->selectData,METACOMPLIST,i);
  }

printf("<tr><th align=right>%s</th><th align=center>%s</th><th></th><th align=left>%s</th></tr>\n", mySrbStrings[79], mySrbStrings[77], mySrbStrings[78]);
  printf("<tr></tr>\n");


  printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showMDV%i\"></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td>%s</td>",Sentries->globalCounter++,DATA_ANNOTATION, mySrbStrings[66],METACOMPLIST);
  printf("<td><INPUT NAME=\"Annotation\", VALUE=\"\", SIZE=20,12></td></tr>\n");
    getUserAsSelect(Sentries, "");
    printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showMDV%i\"></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n", Sentries->globalCounter++,DATA_ANNOTATION_USERNAME, mySrbStrings[82], Sentries->selectData);
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",DATA_OWNER, mySrbStrings[48], Sentries->selectData);
    printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showMDV%i\"></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",Sentries->globalCounter++,USER_NAME, aaa[11], Sentries->selectData);
    getValuesAsSelect(Sentries, ACCESS_CONSTRAINT, Sentries->selectData,"","");
    printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showMDV%i\"></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td>",Sentries->globalCounter++,
	   DATA_ACCESS_PRIVILEGE,aaa[12]);
    printf("<td align=center><input type=hidden name=\"c1\" value=\"contains\"><B>=</B></td><td>%s</td></tr>\n",
	   Sentries->selectData);
    
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td>%s</td><td><INPUT NAME=\"dataName\", VALUE= \"\", SIZE=20,12></td></tr>\n",DATA_NAME, mySrbStrings[46], METACOMPLIST);
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td>%s</td><td><INPUT NAME=\"collName\", VALUE= \"\", SIZE=20,12></td></tr>\n",COLLECTION_NAME, mySrbStrings[69], METACOMPLIST);
    getValuesAsSelect(Sentries, DATA_TYP_NAME, Sentries->selectData,"","");
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",DATA_TYP_NAME, mySrbStrings[52],Sentries->selectData);
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td>%s</td><td><INPUT NAME=\"dataComments\", VALUE= \"\", SIZE=20,12></td></tr>\n",DATA_COMMENTS, aaa[5],METACOMPLIST);
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td>%s</td><td><INPUT NAME=\"dataComments\", VALUE= \"\", SIZE=20,12></td></tr>\n",GUID, aaa[24],METACOMPLIST);
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td>%s</td><td><INPUT NAME=\"dataSize\", VALUE= \"\", SIZE=20,12></td></tr>\n",SIZE, aaa[6],METACOMPLIST);fflush(stdout);


    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td>",REPL_TIMESTAMP, aaa[7]);
    printf("<td>%s</td>",METACOMPLIST); fflush(stdout);
    printf("<td><INPUT NAME=\"modifyTimeStamp\",VALUE= \"\",SIZE=20,12>(YYYY-MM-DD-HH.MM.SS)</td></tr>\n");
    printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showMDV%i\"></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td>",DATA_CREATE_TIMESTAMP, Sentries->globalCounter++,
	   aaa[10]);
    printf("<td>%s</td>",METACOMPLIST);
    printf("<td><INPUT NAME=\"dataCreateTimeStamp\", VALUE= \"\", SIZE=20,12>(YYYY-MM-DD-HH.MM.SS)</td></tr>\n");
    getValuesAsSelect(Sentries, PHY_RSRC_NAME, Sentries->selectData,"","");
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td>",PHY_RSRC_NAME, aaa[4]);
    printf("<td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",Sentries->selectData);


    getValuesAsSelect(Sentries, RSRC_TYP_NAME, Sentries->selectData,"","");

    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",RSRC_TYP_NAME, aaa[8],Sentries->selectData);
    getValuesAsSelect(Sentries, LOCATION_NAME, Sentries->selectData,"","");
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",LOCATION_NAME, aaa[9],Sentries->selectData);

    

  printf("</table><br>\n");
  
/********* for collection metadata ******/
  printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[183]);
  printf("<table><tr><th align=right>%s</th><th align=center>%s</th><th></th><th align = center>%s</th></tr>\n", mySrbStrings[79], mySrbStrings[77], mySrbStrings[78]);
  printf("<tr></tr>\n");
  getUDSMD_COLL0AsSelect(Sentries, Sentries->selectData,"", Sentries->entries[1].val);

  /**** again, an empty field to allow browse query even if there is no Metadata ***/
  if (!strlen(Sentries->selectData)) {
   sprintf(Sentries->selectData,"<SELECT SIZE=1 NAME=c1><OPTION></SELECT>");
  }

  printf("<tr></tr>\n");
  for (i = 0  ; i < 5 ; i++) {
    printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showMDV%i\"></td><td>%s</td><td>%s</td><td><INPUT NAME=\"newmdval%i\", VALUE= \"\", SIZE=20,12></td></tr>\n",
	  Sentries->globalCounter++, Sentries->selectData,METACOMPLIST,i);
  }
/****************************************/ 
  
  printf("</table><br>\n");

   getAccsCollectionsAsSelect(Sentries, NULL, Sentries->entries[1].val);
   printf("<STRONG><FONT COLOR=#FF0000>%s</FONT></STRONG>%s<br>\n", mySrbStrings[184],Sentries->selectData);
  
  printf("<br><INPUT TYPE=CHECKBOX NAME=\"collOnly\">&nbsp;&nbsp;<b>Show Collections Only</b><br>");
}

int 
makeBrowseQueryPage(inStruct Sentries)
{
  int i = 0;
  char urlcollection[MAX_TOKEN];

  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE>", mySrbStrings[22]); 
  printf ("<BASE TARGET = \"sub%s\"> </HEAD>", Sentries->pidStr);
  
        if (!strcmp(mode,"light")) {
            printf("<body bgcolor=\"#FFFFF7\">");
        }
        else {
            printf("<body background=\"%s/srb3.jpg\">", webPath);
        }
  
  printf ("<H2>%s %s</H2>", mySrbStrings[76], Sentries->entries[1].val);
  fflush(stdout);

    getUDSMD0AsSelect(Sentries, Sentries->selectData,"", Sentries->entries[1].val);

  /**** need an empty field to allow browse query even if there is no Metadata ***/
  if (!strlen(Sentries->selectData)) {
   sprintf(Sentries->selectData, "<SELECT SIZE=1 NAME=c1><OPTION></SELECT>");
  }


  printf ("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"  >\n");
  printf("<input type=hidden name=\"function\" value=\"browsequeryvalues\">");

  printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[185]);
  printf("<table><tr><th align=right>%s</th><th align=center>%s</th><th></th><th align = center>%s</th></tr>\n", mySrbStrings[79], mySrbStrings[77], mySrbStrings[78]);
  printf("<tr></tr>\n");
  for (i = 0  ; i < 5 ; i++) {
    printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showMDV%i\"></td><td>%s</td><td>%s</td><td><INPUT NAME=\"newmdval%i\", VALUE= \"\", SIZE=20,12></td></tr>\n",
	  Sentries->globalCounter++, Sentries->selectData,METACOMPLIST,i);
  }
printf("<table><tr><th align=right>%s</th><th align=center>%s</th><th></th><th align = center>%s</th></tr>\n", mySrbStrin\
gs[79], mySrbStrings[77], mySrbStrings[78]);
  printf("<tr></tr>\n");
  if (!strcmp(Sentries->entries[2].val,"more")) {
    printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showMDV%i\"></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td>%s</td><td><INPUT NAME=\"Annotation\", VALUE= \"\", SIZE=20,12></td></tr>\n", Sentries->globalCounter++,DATA_ANNOTATION, mySrbStrings[66],METACOMPLIST);
    getUserAsSelect(Sentries, "");
    printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showMDV%i\"></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n", Sentries->globalCounter++,DATA_ANNOTATION_USERNAME, mySrbStrings[82], Sentries->selectData);
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",DATA_OWNER, mySrbStrings[48], Sentries->selectData);
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td>%s</td><td><INPUT NAME=\"dataName\", VALUE= \"\", SIZE=20,12></td></tr>\n",DATA_NAME, mySrbStrings[46], METACOMPLIST);
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td>%s</td><td><INPUT NAME=\"collName\", VALUE= \"\", SIZE=20,12></td></tr>\n",COLLECTION_NAME, mySrbStrings[69], METACOMPLIST);
    getValuesAsSelect(Sentries, DATA_TYP_NAME, Sentries->selectData,"","");
    printf("<tr><td></td><td  align=right><input type=hidden name=\"c1\" value=\"%i\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",DATA_TYP_NAME, mySrbStrings[52],Sentries->selectData);

  }

  printf("</table><br>\n");
  
/********* for collection metadata ******/
  printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[183]);
  printf("<table><tr><th align=right>%s</th><th align=center>%s</th><th></th><th align = center>%s</th></tr>\n", mySrbStrings[79], mySrbStrings[77], mySrbStrings[78]);
  printf("<tr></tr>\n");
  getUDSMD_COLL0AsSelect(Sentries, Sentries->selectData,"", Sentries->entries[1].val);

  /**** again, an empty field to allow browse query even if there is no Metadata ***/
  if (!strlen(Sentries->selectData)) {
   sprintf(Sentries->selectData,"<SELECT SIZE=1 NAME=c1><OPTION></SELECT>");
  }

  printf("<tr></tr>\n");
  for (i = 0  ; i < 5 ; i++) {
    printf("<tr><td><INPUT TYPE=CHECKBOX NAME=\"showMDV%i\"></td><td>%s</td><td>%s</td><td><INPUT NAME=\"newmdval%i\", VALUE= \"\", SIZE=20,12></td></tr>\n",
	  Sentries->globalCounter++, Sentries->selectData,METACOMPLIST,i);
  }
/****************************************/ 
  
  printf("</table><br>\n");

   getAccsCollectionsAsSelect(Sentries, NULL, Sentries->entries[1].val);
   printf("<STRONG><FONT COLOR=#FF0000>%s</FONT></STRONG>%s<br>\n", mySrbStrings[184],Sentries->selectData);
  
  printf("<br><INPUT TYPE=CHECKBOX NAME=\"collOnly\">&nbsp;&nbsp;<b>Show Collections Only</b><br>");
  
  printf("<input type=hidden name=\"session\" value=\"%s\"><br>", Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\"><INPUT TYPE=\"reset\" VALUE=\"%s\"></FORM>\n", mySrbStrings[81], mySrbStrings[80]);
  if (!strcmp(Sentries->entries[2].val,"first")) {
    strcpy(urlcollection,Sentries->entries[1].val);
    escape_url(urlcollection);
    printf("<A HREF=\"mysrb331.cgi?function=browsequery&collection=%s&type=more&session=%s\" >%s</A>\n",urlcollection, Sentries->pidStr, mySrbStrings[83]);
  }
  printf("</body></html>");
  return;
}

/* 
 * Write a page for displaying query options when obj is a datascope obj.
 *
 * Datascope specific fucntion --sifang
 */
int
makeDatascopeQueryPage(inStruct Sentries)
{
  int i,fd;

  char datascopeTables[HUGE_STRING];
  char datascopeFields[HUGE_STRING * 4];

  printf("Content-type: text/html%c%c",10,10);fflush(stdout);

  printf ("<HTML><HEAD> <TITLE> MYSRB - A TransSystem Data Explorer </TITLE> <BASE TARGET = \"%ld\"> </HEAD><body background=\"%s/srb3.jpg\"><H1>Datascope Query</H1>", getpid(),webPath);
  printf ("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"  >\n");
  printf("<input type=hidden name=\"function\" value=\"queryDatascope\">");fflush(stdout);
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"seek\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
  printf("<input type=hidden name=\"owner\" value=\"%s\">",Sentries->entries[7].val);


  fd = connectDatascope(Sentries);
  i = getValuesAsSelectFromDatascope(Sentries,fd,datascopeTables,HUGE_STRING-1, dbSCHEMA_TABLES);
  i = getValuesAsSelectFromDatascope(Sentries,fd,datascopeFields,(HUGE_STRING*4)-1, dbSCHEMA_FIELDS);
  srbObjClose (Sentries->conn,fd);
  for (i = 0 ; i < 5 ; i ++) {
    printf("Perform ");
    printf("<SELECT SIZE=1 NAME=ds1><OPTION SELECTED><OPTION>dbopen<OPTION>dbjoin<OPTION>dbgroup<OPTION>dbleftjoin<OPTION>dbnojoin<OPTION>dbselect<OPTION>dbseparate<OPTION>dbsever<OPTION>dbsort<OPTION>dbsubset<OPTION>dbtheta<OPTION>dbungroup<OPTION>verbose</SELECT>");
    /*printf("%s",dbSCHEMA_TABLES); */
    /*  printf("On Table: <input name=\"dse1%i\" value=\"\" SIZE=20,12>",i);*/
    printf("On Table: %s",datascopeTables);
    printf("With Expression: <input name=\"dse2%i\" value=\"\" SIZE=20,12><BR>",i);
  }
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->entries[8].val);
  printf("<INPUT TYPE=\"submit\" VALUE=\"Query Datascope\"><INPUT TYPE=\"reset\" VALUE=\"Clear\"></FORM>\n");
  printf("<B>Possible Field Names are:</B> %s",datascopeFields);
  printf("<BR> Use these in expressions when necessary.");
  printf("</body></html>");
  return;

}

int
makeQueryMetaPage(inStruct Sentries)
{
  int i;
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);

  printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD><body background=\"%s/srb3.jpg\"><H1>Query By MetaData</H1>", mySrbStrings[22], webPath);
  printf ("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"  >\n");
  printf("<input type=hidden name=\"function\" value=\"queryMetaValues\">");
  printf("<table><tr><th align=right>%s</th><th></th><th align = left>%s</th></tr>\n", mySrbStrings[77], mySrbStrings[78]);
  printf("<tr></tr>\n");
  i  = 0 ;
  while (strcmp(metaAttrNames[i],"null")) {
    /**if (!strcmp(metaAttrNames[i],"Collection")) {
      getValuesAsSelect(COLLECTION_NAME,selectData,"","");
      printf("<tr><td  align=right><input type=hidden name=\"c1\" value=\"%s\">%s</td><td><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",metaAttrNames[i],metaAttrNames[i],selectData);
    }
    else **/
    if (!strcmp(metaAttrNames[i],"Data Type")) {
      getValuesAsSelect(Sentries, DATA_TYP_NAME, Sentries->selectData,"","");
      printf("<tr><td  align=right><input type=hidden name=\"c1\" value=\"%s\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",metaAttrNames[i], metaAttrNames[i],selectData);
    }
    else if (!strcmp(metaAttrNames[i],"Resource Name")) {
      getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData,"","#sdsc-fs#sdsc-mda18-fs#");
      printf("<tr><td  align=right><input type=hidden name=\"c1\" value=\"%s\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",metaAttrNames[i],metaAttrNames[i],selectData);
    }
    else if (!strcmp(metaAttrNames[i],"Resource Class")) {
      getValuesAsSelect(Sentries, RSRC_CLASS, Sentries->selectData,"","");
      printf("<tr><td  align=right><input type=hidden name=\"c1\" value=\"%s\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",metaAttrNames[i],metaAttrNames[i],selectData);
    }
    else if (!strcmp(metaAttrNames[i],"Resource Type")) {
      getValuesAsSelect(Sentries, RSRC_TYP_NAME, Sentries->selectData,"","");
      printf("<tr><td  align=right><input type=hidden name=\"c1\" value=\"%s\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",metaAttrNames[i],metaAttrNames[i],selectData);
    }
    else if (!strcmp(metaAttrNames[i],"Container Name")) {
      getValuesAsSelect(Sentries, CONTAINER_NAME,Sentries->selectData,"","");
      printf("<tr><td  align=right><input type=hidden name=\"c1\" value=\"%s\">%s</td><td align=center><input type=hidden name=\"c1\" value=\"=\"><B>=</B></td><td>%s</td></tr>\n",metaAttrNames[i],metaAttrNames[i],selectData);
    }
    else {
      printf("<tr><td  align=right><input type=hidden name=\"c1\" value=\"%s\">%s</td><td>%s</td><td><INPUT NAME=\"%s\", VALUE= \"\", SIZE=20,12></td></tr>\n",metaAttrNames[i], metaAttrNames[i],METACOMPLIST,metaAttrNames[i]);
    }
    i++;
  }
  /*
  for (  i = 0; i < 6 ; i++) {
    printf("<tr><td  align=center>%s</td><td>%s</td><td><INPUT NAME=\"newmdname%i\", VALUE= \"\", SIZE=20,12></td>", METAATTRLIST,METACOMPLIST,i);
  }
  */
  for (i = 0  ; i < 1 ; i++) {
  printf("<tr><td><INPUT NAME=\"newmdname%i\", VALUE= \"\", SIZE=20,12></td><td>%s</td><td><INPUT NAME=\"newmdval%i\", VALUE= \"\", SIZE=20,12></td></tr>\n",
	 i,METACOMPLIST,i);
  }
  printf("</table><BR>\n");
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\"><INPUT TYPE=\"reset\" VALUE=\"%s\"></FORM>\n", mySrbStrings[81], mySrbStrings[80]);
  printf("</body></html>");
  return;
}

int
makeSlinkFullPage(inStruct Sentries)
{
   int i;
   for (i=1; i < 8; i++) 
	escape_url(Sentries->entries[i].val);
   printf("Content-type: text/html%c%c",10,10);fflush(stdout);
   printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\">", "MYSRB V9.0");
   printf ("<meta http-equiv=\"no-cache\">");
   printf ("</HEAD><FRAMESET rows=\"25%,75%\" BORDER=no >");
   printf("<FRAME SRC=\"mysrb331.cgi?function=slinkmeta&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
   printf("<FRAME SRC=\"mysrb331.cgi?function=slinkdata&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
   printf("</FRAMESET> </body></html>");
}

int
makeURLFullPage(inStruct Sentries)
{
   int i;
   for (i=1; i < 8; i++) 
	escape_url(Sentries->entries[i].val);
   printf("Content-type: text/html%c%c",10,10);fflush(stdout);
   printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\">", "MYSRB V9.0");
   printf ("<meta http-equiv=\"no-cache\">");
   printf ("</HEAD><FRAMESET rows=\"25%,75%\" BORDER=no >");
   printf("<FRAME SRC=\"mysrb331.cgi?function=meta&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
   printf("<FRAME SRC=\"%s\">",Sentries->entries[5].val);
   printf("</FRAMESET> </body></html>");
}

int
makeTicketURLFullPage(inStruct Sentries)
{
   int i;
   for (i=1; i < 8; i++) 
	escape_url(Sentries->entries[i].val);
   printf("Content-type: text/html%c%c",10,10);fflush(stdout);
   printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\">", "MYSRB V9.0");
   printf ("<meta http-equiv=\"no-cache\">");
   printf ("</HEAD><FRAMESET rows=\"25%,75%\" BORDER=no >");
   printf("<FRAME SRC=\"mysrb331.cgi?function=ticketmeta&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
   printf("<FRAME SRC=\"%s\">",Sentries->entries[5].val);
   printf("</FRAMESET> </body></html>");
}

makeSQLFullPage(inStruct Sentries)
{
   int i;
   for (i=1; i < 8; i++) 
	escape_url(Sentries->entries[i].val);
   printf("Content-type: text/html%c%c",10,10);fflush(stdout);
   printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\">", "MYSRB V9.0");
   printf ("<meta http-equiv=\"no-cache\">");
   printf ("</HEAD><FRAMESET rows=\"25%,75%\" BORDER=no >");
   printf("<FRAME SRC=\"mysrb331.cgi?function=meta&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
   printf("<FRAME SRC=\"mysrb331.cgi?function=sqldataform&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
   printf("</FRAMESET> </body></html>");
}

makeExecFullPage(inStruct Sentries)
{
   int i;
   for (i=1; i < 8; i++) 
	escape_url(Sentries->entries[i].val);
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
   printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\">", "MYSRB V9.0");
   printf ("<meta http-equiv=\"no-cache\">");
   printf ("</HEAD><FRAMESET rows=\"25%,75%\" BORDER=no >");
   printf("<FRAME SRC=\"mysrb331.cgi?function=meta&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">", Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
   printf("<FRAME SRC=\"mysrb331.cgi?function=execdataform&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">", Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
   printf("</FRAMESET> </body></html>");
}

/*
 * datascope specific function --sifang 
 */
makeDatascopeFullPage(inStruct Sentries)
{
   int i;
   for (i=1; i < 8; i++) 
	  escape_url(Sentries->entries[i].val);
   printf("Content-type: text/html%c%c",10,10);fflush(stdout);
   printf ("<HTML><HEAD> <TITLE> MYSRB - A TransSystem Data Explorer </TITLE> <BASE TARGET = \"_top\">"); 

    
   fflush(stdout);
   printf ("<meta http-equiv=\"no-cache\">");
   printf ("</HEAD><FRAMESET rows=\"25%,75%\" BORDER=no >");
   printf("<FRAME SRC=\"mysrb331.cgi?function=meta&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
   printf("<FRAME SRC=\"mysrb331.cgi?function=datascopedataform&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
   printf("</FRAMESET> </body></html>");fflush(stdout);
}

int
makeFullPage(inStruct Sentries)
{
   int i;
   for (i=1; i < 9; i++) 
   escape_url(Sentries->entries[i].val);

    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\">", "MYSRB V9.0");
    printf("<meta http-equiv=\"no-cache\">");
    printf("</HEAD>");
    printf("<FRAMESET rows=\"25%,75%\" BORDER=no >");
    printf("<FRAME SRC=\"mysrb331.cgi?function=meta&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val, Sentries->entries[2].val, Sentries->entries[3].val, Sentries->entries[4].val, Sentries->entries[5].val, Sentries->entries[6].val, Sentries->entries[7].val, Sentries->entries[8].val);
    printf("<FRAME NAME=\"bottom%s\" SRC=\"mysrb331.cgi?function=data&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">", Sentries->entries[8].val, Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val, Sentries->entries[8].val);
    printf("</FRAMESET></html>");
}


int
makeFullPageWithTree(inStruct Sentries)
{
   int i;
   for (i=1; i < 9; i++) 
   escape_url(Sentries->entries[i].val);

    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\">", "MYSRB V9.0");
    printf("<meta http-equiv=\"no-cache\">");
    printf("</HEAD>");
    printf("<FRAMESET cols=\"25%,75%\" BORDER=no >");
    printf("<FRAME SRC=\"mysrb331.cgi?function=maketree&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val, Sentries->entries[2].val, Sentries->entries[3].val, Sentries->entries[4].val, Sentries->entries[5].val, Sentries->entries[6].val, Sentries->entries[7].val, Sentries->entries[8].val);
    printf("<FRAME NAME=\"sub%s\" SRC=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">", Sentries->entries[8].val, Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
    printf("</FRAMESET> </body></html>");
}


int
makeTicketSlinkFullPage(inStruct Sentries)
{
   int i;

   for (i=1; i < 8; i++) 
	escape_url(Sentries->entries[i].val);
   
printf("Content-type: text/html%c%c",10,10);fflush(stdout);
   printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD><FRAMESET rows=\"25%,75%\" BORDER=no>", "MYSRB V9.0");
   printf("<FRAME SRC=\"mysrb331.cgi?function=ticketslinkmeta&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
   printf("<FRAME SRC=\"mysrb331.cgi?function=ticketslinkdata&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);

   printf("</FRAMESET> </body></html>");

}

int
makeTicketFullPage(inStruct Sentries)
{
   int i;

   for (i=1; i < 8; i++) 
	escape_url(Sentries->entries[i].val);
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD><FRAMESET rows=\"25%,75%\" BORDER=no>", "MYSRB V9.0");
    printf("<FRAME SRC=\"mysrb331.cgi?function=ticketmeta&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
    printf("<FRAME SRC=\"mysrb331.cgi?function=ticketdata&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);

    printf("</FRAMESET> </body></html>");

}

int
makeTicketSQLFullPage(inStruct Sentries)
{
   int i;

   for (i=1; i < 8; i++) 
	escape_url(Sentries->entries[i].val);
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD><FRAMESET rows=\"25%,75%\" BORDER=no>", "MYSRB V9.0");
    printf("<FRAME SRC=\"mysrb331.cgi?function=ticketmeta&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);
    printf("<FRAME SRC=\"mysrb331.cgi?function=ticketsqldataform&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[8].val);

    printf("</FRAMESET> </body></html>");

}

/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/

/* prepares input and output struct */

int prepare(inStruct Sentries, outStruct Sresult, char *inlogDir, char *inreturnEmailDomain, char *incheckHostAddress, char *inconnectHostAddress, char *inSRBPORT, char *inhttptype, char *inwebPath, char *inemailAddress, char* innewUserDomain, char *inDefaultResource, char *inlangDir)
{
    strcpy(logDirec, inlogDir);
    strcpy(Sentries->returnEmailDomain,inreturnEmailDomain);
    strcpy(Sentries->checkHostAddress,incheckHostAddress);
    strcpy(Sentries->connectHostAddress,inconnectHostAddress);
    strcpy(Sentries->SRBPORT, inSRBPORT);
    strcpy(Sentries->httptype,inhttptype);
    strcpy(webPath, inwebPath);
    strcpy(Sentries->emailAddress, inemailAddress);
    strcpy(Sentries->newUserDomain, innewUserDomain);
    strcpy(Sentries->defaultResource,inDefaultResource);
    strcpy(langDir, inlangDir);
    Sresult->Sentry=Sentries;

    Sentries->showAllMetaFlag=0;
    Sentries->heading=0;
    Sentries->first=1;
    Sentries->firsts=1;
    Sentries->firstd=1;
    Sentries->isTicket=0;
    Sentries->slinkFlag=0;
    Sentries->queryFunction=0;
    Sentries->globalCounter=0;
    Sentries->likeCollFlag=0;
    Sentries->linkCtr=0;
    Sentries->showMDVCntr=0;
    Sentries->m = 1;
    Sentries->oldMcatZone = NULL;
    Sentries->currentMcatZone= NULL;
    Sentries->op = 0;
    
}



int logIt(int m, char *logDirec, char *cookieStr, entry entries[])
{
  int i;
  FILE *fId;
  char timeval1[3*MAX_TOKEN];
     
  sprintf(timeval1, "%s/mySRBLog",logDirec);
  fId = fopen (timeval1, "a");
     if (fId == NULL) 
       return (-errno);
     get_time(timeval1);
     fprintf(fId,"%s: %s %s %s  ", VERSION_NUM, timeval1,getenv("REMOTE_ADDR"), cookieStr);
     for (i = 0; i <= m ; i++) {
       if ((!strcmp(entries[0].val,"put")      && i==m-1) ||
	   (!strcmp(entries[0].val,"editdata")      && i==m-1) ||
	   (!strcmp(entries[0].val,"appenddata")      && i==m-1) ||
	   (!strcmp(entries[0].val,"sentry"  ) && i==3 )  ||
	   (!strcmp(entries[0].val,"chpasswd") && i > 2 ) ) {
       }
       else {
	 fprintf(fId,"  %s",entries[i].val);
       }
     }
     fprintf(fId,"\n--------------------------------------------------\n");
     fclose(fId);
     return(0);
}


/************** mySRBCall is the central function ********************/

int mySRBCall(char *inlogDir, char *inreturnEmailDomain, char *incheckHostAddress, char *inconnectHostAddress, char *inSRBPORT, char *inhttptype, char *inwebPath, char *inemailAddress, char* innewUserDomain, char *inDefaultResource, char *inlangDir)
{
    inStruct Sentries=(paramIn*)malloc(sizeof(paramIn));
    outStruct Sresult=(paramOut*)malloc(sizeof(paramOut));

    prepare(Sentries,Sresult,inlogDir,inreturnEmailDomain,incheckHostAddress,inconnectHostAddress,inSRBPORT,inhttptype,inwebPath,inemailAddress,innewUserDomain,inDefaultResource,inlangDir);
    getEntries(Sentries);
    performQuery(Sentries, Sresult);
    displayResult(Sresult);
    
    exit(0);
}


/* reads values entered in html form and stores them in the inStruct previously defined */

int getEntries(inStruct Sentries) {
char dataName[MAX_TOKEN * 2];
char collName[MAX_TOKEN * 2];
int dataseek, copyNum;
int datasize;
register int x;
char *stquery, *tmpq, *tmpStr, *tmpStr1, *tmpPtr;
char reqMethod[100];
int msgLength;
char contentType[100];
char boundary[MAX_TOKEN];
  


    putenv("HOME=/");

    dataseek = 0;
    datasize = 0;
    copyNum = -1;

   if (getenv("CONTENT_TYPE") != NULL)
     strcpy(contentType,getenv("CONTENT_TYPE"));
   else 
     strcpy(contentType,"");
   if (getenv("REQUEST_METHOD") != NULL)
     strcpy(reqMethod,getenv("REQUEST_METHOD"));
   else 
     strcpy(reqMethod,"");
   if (getenv("HTTP_COOKIE") != NULL) 
     strcpy(Sentries->cookieStr,getenv("HTTP_COOKIE"));
     
   else 
     strcpy(Sentries->cookieStr,"");
   if (strstr(Sentries->cookieStr,"*") != NULL ||
      strstr(Sentries->cookieStr,"..") != NULL || 
       strstr(Sentries->cookieStr,"?") != NULL ||
       strstr(Sentries->cookieStr,"/") != NULL ||
       strstr(Sentries->cookieStr,"\\") != NULL ) { 
        
        Sentries->op = -1;
        return (1);
        

   }


   if (!strcmp(reqMethod,"POST") || !strcmp(reqMethod,"post")) {
     msgLength = atoi(getenv("CONTENT_LENGTH")) + 10;
     stquery =  malloc(msgLength);
     if (fread(stquery, 1, msgLength, stdin) != (msgLength-10)) {
		webErrorExit("short fread",0);
	}
     stquery[msgLength] = '\0';
   }
   else { 
     stquery =  malloc(QSIZE);
        if (getenv("QUERY_STRING") != NULL)
	  strcpy(stquery,getenv("QUERY_STRING"));
	else 
	  strcpy(stquery,"");
   }

   if (strstr(contentType,"multipart/form-data") != NULL) {

     i = msgLength - 10;
     getBoundary(&stquery, boundary);
     /***     printf("Boundary:**%s**<BR>\n",boundary);fflush(stdout); ***/
     for(x=0;  *stquery != '\0';x++) {
       if (x == MAX_ENTRIES) webErrorExit("MaxEntries Exceeded",x);
       Sentries->m=x; 
       /***     printf("GettingX:%i....\n",x);fflush(stdout); ***/
       tmpPtr = stquery;
       if (getmultipartword(&Sentries->entries[x],&stquery, boundary, i) != 0)
	 break;
       i -= stquery - tmpPtr;
       /***     printf("%i:%s=%s<BR>\n",entries[x].size,entries[x].name,entries[x].val);fflush(stdout);***/
     }
     Sentries->m--;
   }
   else {
    
    /**  the following is to take care of the 
     home col. name bad length pb Linux on RedHat7  *******/
    solvepb=malloc(10);
    free(solvepb);
    /******************************************************/
    
     for(x=0;  stquery[0] != '\0';x++) {
       if (x == MAX_ENTRIES) webErrorExit("MaxEntries Exceeded",x);
       Sentries->m=x; 
       Sentries->entries[x].val =  malloc(HUGE_STRING);
       getword(Sentries->entries[x].val,stquery,'&');
       plustospace(Sentries->entries[x].val);
       unescape_url(Sentries->entries[x].val);
       sprintf(Sentries->entries[x].name, (char *) makeword(Sentries->entries[x].val,'='));
     }
   }


   if ( strcmp(Sentries->currentLanguage,"") ) {
   readLangFile(langDir, Sentries->currentLanguage);
       }
   return 0;   
     
}



/*****   this function processes the query and if needed interacts with SRB   *************/

int performQuery(inStruct Sentries, outStruct Sresult) {
    int i, x;
    char *tmpStr, *tmpStr1, *stquery, *tmpq, *tmpPtr;
    char dataName[MAX_TOKEN * 2];
    char collName[MAX_TOKEN * 2];
    int dataseek, copyNum;
    int datasize;
    char reqMethod[100];
    int msgLength;
    char contentType[100];
    char boundary[MAX_TOKEN];


    removeFinalBlanksFromEntries(Sentries);
    
//    Sentries->selectData = (char *)malloc(10*HUGE_STRING);
//    Sentries->sizeOfSelectData = 10*HUGE_STRING;
   
    if (Sentries->op==-1) {
     Sentries->entries[0].val[0] = 'F';
     Sentries->entries[0].val[1] = 'L';
     logIt(Sentries->m, logDirec, Sentries->cookieStr, Sentries->entries);
     printf("Content-type: text/html%c%c",10,10);fflush(stdout);
     printf("<html>");
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf("<br>Error: No Session Information Available. Please login\n");
     for (i = 0; i <= Sentries->m; i++)
         printf("WWWWW %i:%s=%s<BR>",i,Sentries->entries[i].name,Sentries->entries[i].val);
     printf("PPPPP: Cookie String: %s<BR>\n",Sentries->cookieStr);
     fflush(stdout);
     printf("</body></html>");
     return (1);
     }
    #ifdef LOGIT
     i = logIt(Sentries->m, logDirec, Sentries->cookieStr, Sentries->entries);
     if (i < 0) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("Status Error = %i\n", i);
      printf("</body></html>");
      exit(0);      
     }
    #endif /* LOGIT */


     if (!strcmp(Sentries->entries[0].val, "listentries")) { 
       /*     if (!strcmp(Sentries->entries[0].val, "extschemaattrvalues") ||
	 !strcmp(Sentries->entries[0].val, "browsequeryvalues")) {
            if (!strcmp(Sentries->entries[0].val, "extschemaattrvalues")) {*/
       printf("Content-type: text/html%c%c",10,10);fflush(stdout);
       printf("<html>");
       printf("<body background=\"%s/srb3.jpg\">", webPath);
       for (i = 0; i <= Sentries->m; i++) 
	 printf("%i:%s=%s<BR>",i,Sentries->entries[i].name,Sentries->entries[i].val);
       printf("</body>");
       printf("</html>");
       exit(0);
     }
     if (!strcmp(Sentries->entries[0].val, "nothing")) {  /* for tests */
      webErrorExit("nothing to be done here", 0);
      exit(0);
     }
   
     if (!strcmp(Sentries->entries[0].val,"sentry")) {
      strcpy(Sentries->startzone, Sentries->entries[5].val);
      strcpy(Sentries->startcol, Sentries->entries[10].val);
      strcpy(Sentries->currentLanguage, Sentries->entries[8].val);
      getMode(Sentries->entries[7].val);
      readLangFile(langDir, Sentries->currentLanguage);
      loginUser(Sentries);
      exit(0);
     }

    if (!strcmp(Sentries->entries[0].val,"showticket")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      fprintf(stdout, "<script language=\"JavaScript\">\n\
        function mySubmit(formID, arg){\n\
        if(arg.value != \"doNothing\")\n\
            formID.submit();\n\
        }\n\
        </script>\n");
      printf("</head>");        
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      Sentries->isTicket = 1;
      strcpy(Sentries->currentLanguage, Sentries->entries[2].val);   
      strcpy(Sentries->pidStr, Sentries->entries[Sentries->m].val);
      makeTicketConnection(Sentries);
      getTicketInfo(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"ticketmeta")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      Sentries->isTicket = 1;
      printf("<html>");
      printf("<title>Show Meta for %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);      
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      strcpy(Sentries->pidStr, Sentries->entries[Sentries->m].val);
      makeTicketConnection(Sentries);
      showMeta(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"ticketslinkmeta")) {
      printf("Content-type: text/html%c%c",10,10);
      fflush(stdout);
      Sentries->isTicket = 1;
      Sentries->slinkFlag = 1;
      printf("<html>");
      printf("<title>Show Meta for %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);      
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      fflush(stdout);
      strcpy(Sentries->pidStr,Sentries->entries[Sentries->m].val);
      makeTicketConnection(Sentries);
      showMeta(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"ticketslinkdata")) {
      strcpy(Sentries->pidStr, Sentries->entries[Sentries->m].val);
      Sentries->isTicket = 1;
      Sentries->slinkFlag = 1;
      makeTicketConnection(Sentries);
      if (strlen( Sentries->entries[1].val) == 0) {
	printf("Content-type: text/html%c%c",10,10);fflush(stdout);
        printf("<html>");
	printf("<title>Collection Info for %s</title> <base target = \"_top\">",Sentries->entries[2].val);
        printf("<body background=\"%s/srb3.jpg\">", webPath);
	/**
	   getTicketCollectionInfo(Sentries->entries[2].val,Sentries->entries[1].val);
	***/

	getCollectionInfo(Sentries);

	clFinish(Sentries->conn);
	printf("</body></html>");
	exit(0);
      }
      else {
	tmpPtr = strstr(Sentries->entries[1].val,"@SHADOW");
	if (tmpPtr != NULL) *tmpPtr='&';
	sprintf(dataName,"%s&TICKET='%s'",Sentries->entries[1].val,Sentries->entries[Sentries->m].val);
	Sentries->entries[1].val = dataName;
	sendSlinkData(Sentries);
	exit(0);
      }
    }

    if (!strcmp(Sentries->entries[0].val,"ticketsqldata")) {
      strcpy(Sentries->entries[0].val,"ticketdata");
      sprintf(Sentries->sqlDataName,"%s&SHADOW= %s",Sentries->entries[1].val, Sentries->entries[8].val); /* space before entries[8].val is required */
      Sentries->entries[1].val = Sentries->sqlDataName;
    }

    if (!strcmp(Sentries->entries[0].val,"ticketdata")) {
      strcpy(Sentries->pidStr,Sentries->entries[Sentries->m].val);
      Sentries->isTicket = 1;
      makeTicketConnection(Sentries);
      if (strlen( Sentries->entries[1].val) == 0) {
	printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	printf("<html><head>");
	printf("<title>Collection Info for %s</title> <base target = \"_top\">",Sentries->entries[2].val);
	fprintf(stdout, "<script language=\"JavaScript\">\n\
function mySubmit(formID, arg){\n\
if(arg.value != \"doNothing\")\n\
formID.submit();\n\
}\n\
</script>\n");
	printf("</head>");        
        printf("<body background=\"%s/srb3.jpg\">", webPath);
	/**
	   getTicketCollectionInfo(Sentries->entries[2].val,Sentries->entries[1].val);
	***/
	getCollectionInfo(Sentries);
	clFinish(Sentries->conn);
	printf("</body></html>");
	exit(0);
      }
      else {
	sprintf(dataName,"%s&TICKET='%s'",Sentries->entries[1].val,Sentries->entries[Sentries->m].val);
	Sentries->entries[1].val = dataName;
	sendData(Sentries);
	exit(0);
      }
    }
    

    if (!strcmp(Sentries->entries[0].val,"ticketdownloadfile")){
      strcpy(Sentries->pidStr,Sentries->entries[Sentries->m].val);
      Sentries->isTicket = 1;
      makeTicketConnection(Sentries);
      sprintf(dataName,"%s&TICKET='%s'",Sentries->entries[1].val,Sentries->entries[Sentries->m].val);
	Sentries->entries[1].val = dataName;
	sendDownloadData(Sentries);
	exit(0);
    }

     if (!strcmp(Sentries->entries[0].val,"chpasswd")) {
      changePassword(Sentries);
      exit(0);
     }
     if (!strcmp(Sentries->entries[0].val,"registeruser")) {
      registerUser();
      exit(0);
     }
     
    if (!strcmp(Sentries->entries[0].val,"full")) {
      makeFullPage(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"treefull")) {
      makeFullPageWithTree(Sentries);
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"slinkfull")) {
      makeSlinkFullPage(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"urlfull")) {
      makeURLFullPage(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"sqlfull")) {
      makeSQLFullPage(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"execfull")) {
      makeExecFullPage(Sentries);
      exit(0);
    }

    /* datascope specific --sifang */
    if (!strcmp(Sentries->entries[0].val,"datascopefull")) {
      makeDatascopeFullPage(Sentries);
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"ticketslinkfull")) {
      makeTicketSlinkFullPage(Sentries);
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"ticketfull")) {
      makeTicketFullPage(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"ticketsqlfull")) {
      makeTicketSQLFullPage(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"ticketurlfull")) {
      makeTicketURLFullPage(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"ticketsqldataform")) {
      strcpy(Sentries->pidStr,Sentries->entries[Sentries->m].val);
      Sentries->isTicket = 1;
      makeTicketConnection(Sentries);  
      makeSQLDataFormPage(Sentries);
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"metainsert")) {
      makeMetaInsertPage(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"dcinsert")) {
      makeDCInsertPage(Sentries);
      exit(0);
    } 

    /** 
	if (!strcmp(entries[0].val,"annoinsert")) {
	makeAnnoInsertPage(m); 
	exit(0);
	} 
    **/

    sprintf(Sentries->pidStr,"mySessionId%s=",Sentries->entries[Sentries->m].val);   
   
   if ((tmpStr = strstr(Sentries->cookieStr,Sentries->pidStr)) != NULL) {
     tmpStr1 = tmpStr + strlen(Sentries->pidStr) - 1;
     *tmpStr1 = '\0';
     strcpy(Sentries->cookieStr,tmpStr + strlen(Sentries->pidStr));
     strcpy(Sentries->pidStr,Sentries->entries[Sentries->m].val);
     tmpStr = strstr(Sentries->cookieStr,";");
     if (tmpStr != NULL)
       *tmpStr = '\0';
       Sentries->m--;
   }
   else {
    
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<br>Error: No Session Information Available. Please login\n");
      printf("<br>WWWWWWWW:Cookie:%s:pid:%s::mstr:%s<BR>\n",Sentries->cookieStr,Sentries->pidStr,Sentries->entries[Sentries->m].val); 
      fflush(stdout);
      printf("</body></html>");
      exit(1);
   }

   
    if (!strcmp(Sentries->entries[0].val,"unmount")) {
      makeUnMount(Sentries);
    }
   
    makeConnection(Sentries);
     
    /* added to make zone name attached to collection name */
    
    if (Sentries->currentMcatZone != NULL) {
      if (strstr(Sentries->entries[2].val,"/home")
	    ==Sentries->entries[2].val ||
	  strstr(Sentries->entries[2].val,"/container")
	    ==Sentries->entries[2].val ||
	  strstr(Sentries->entries[2].val,"/srbtest")
	    ==Sentries->entries[2].val	 ) {
	if (strcmp(Sentries->currentMcatZone, Sentries->userConnectionZone)) {
	  sprintf(tmpoStr,"/%s%s",Sentries->currentMcatZone,
		  Sentries->entries[2].val);
	  free(Sentries->entries[2].val);
	  Sentries->entries[2].val =  strdup(tmpoStr);
	}
      }
    }

    /* added to make zone name attached to collection name */    

    if (!strcmp(Sentries->entries[0].val,"sqldataform")) {
      makeSQLDataFormPage(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"maketree")) {
      makeTreePageNew(Sentries);
      exit(0);
    }  
    
    if (!strcmp(Sentries->entries[0].val,"makeZoneFrame")) {
      makeZoneFrame(Sentries);
      exit(0);
    }    

    if (!strcmp(Sentries->entries[0].val,"makecollframe")) {
      makeCollFrame(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"execdataform")) {
      makeExecDataFormPage(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"removeDataCollForm")) {
      makeRemoveDataCollForm(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"renamedsform")) {
      makeRenamedsForm(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"deleteannoform")) {
      makeDeleteAnnoForm(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"deletemetaform")) {
      makeDeleteMetaForm(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"otherinfo")) {
      makeOtherInfoPage(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"otherinfomenu")) {
      makeOtherInfoMenuPage(Sentries);
      exit(0);
    }    

    if (!strcmp(Sentries->entries[0].val,"schemaquery")) {
      makeSchemaQueryPage(Sentries);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"basicresourceinfo")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Show Basic Resource Info</title>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      showBasicResourceInfo(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"basicuserinfo")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Show Basic User Info</title>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      showBasicUserInfo(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"usercontainerinfo")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Show Basic User Info</title>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      showUserContainerInfo(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"basicalluserinfo")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Show All User Info</title>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      showBasicAllUserInfo(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"metaextractform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf ("<HTML><HEAD> ");
      printf("<TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD> ", "MYSRB V9.0");
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      metaExtractForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"metaextractform1")) {
      if (!strcmp(Sentries->entries[4].val,"NULL")) {
	strcpy(Sentries->entries[0].val,"metaextract");
      }
      else {
        printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	printf ("<HTML><HEAD> ");
	printf("<TITLE> MYSRB - A TransSystem Data Explorer </TITLE> <BASE TARGET = \"_top\"> </HEAD> ");
	printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
	metaExtractForm1(Sentries);
	clFinish(Sentries->conn);
	printf("</body></html>");
	exit(0);
      }
    }

    if (!strcmp(Sentries->entries[0].val,"metaextract")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Extract Metadata</title>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      metaExtract(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"annoinsert")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s</title>", mySrbStrings[203]);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      annoinsertNew(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"showmeta")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Show Meta for %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      showMeta(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"showmetaNew")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s %s/%s</title> <base target = \"_top\">", mySrbStrings[196], Sentries->entries[2].val, Sentries->entries[1].val);

        if (!strcmp(mode,"light")) {
            printf("<body bgcolor=\"#FFFFF7\">");
        }
        else {
            printf("<body background=\"%s/srb3.jpg\">", webPath);
        }

      Sentries->showMetaType = 0;
      showMetaNew(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"showmetaE")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Show Meta for %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val, Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      Sentries->showMetaType = 2;
      showMetaNew(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"showmetaC")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Show Meta for %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      showMetaType = 1;
      showMetaNew(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"removeDataColl")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Deleting %s/%s</title> <base target = \"sub%s\">",Sentries->entries[2].val, Sentries->entries[1].val, Sentries->pidStr);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      removeDataColl(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"deletemeta")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Deleting Metadata for %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val, Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      deleteMeta(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"deleteanno")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Deleting Annotations for %s/%s</title> <base target = \"_top\">", Sentries->entries[2].val, Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      deleteAnno(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"createCollectionForm")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>%s %s</title> <base target = \"sub%s\"></head>", mySrbStrings[171], Sentries->entries[1].val, Sentries->entries[3].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      createCollectionForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"comminsert")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s %s</title> <base target = \"_top\">", mySrbStrings[206], Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      makeCommInsertForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"copydsform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>%s</title> <base target =  \"sub%s\"></head>", mySrbStrings[107], Sentries->pidStr);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      makeCopydsForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"copymetaform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s</title> <base target = \"sub%s\">", mySrbStrings[97], Sentries->pidStr);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      copymetaform(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"copyannoform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s</title> <base target = \"_top\">", mySrbStrings[224]);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      copyannoform(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"movedsform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s</title> <base target = \"_top\">", mySrbStrings[241]);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      makeMovedsForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"slinksform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s</title> <base target = \"sub%s\">", mySrbStrings[241], Sentries->pidStr);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      makeSlinksForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"slinkCollform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s</title> <base target = \"sub%s\">", mySrbStrings[241], Sentries->pidStr);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      makeSlinkCollForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"querymeta")) {
      makeQueryMetaPage(Sentries);
      clFinish(Sentries->conn);
      exit(0);
    }
    
    /* datascope specific --sifang */
    if (!strcmp(Sentries->entries[0].val,"datascopedataform")) {
      makeDatascopeQueryPage(Sentries);
      clFinish(Sentries->conn);
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"browsequery")) {
      makeBrowseQueryPage(Sentries);
      clFinish(Sentries->conn);
      exit(0);
    }
    if (!strcmp(Sentries->entries[0].val,"extschmnamelisting")) {
      makeExtSchemaListing(Sentries);
      clFinish(Sentries->conn);
      exit(0);
    }
    if (!strcmp(Sentries->entries[0].val,"ingestextschmnamelisting")) {
      makeIngestExtSchemaListing(Sentries);
      clFinish(Sentries->conn);
      exit(0);
    }
    if (!strcmp(Sentries->entries[0].val,"extschmattrlisting")) {
      makeExtSchemaAttributeListing(Sentries);
      clFinish(Sentries->conn);
      exit(0);
    }
    if (!strcmp(Sentries->entries[0].val,"extschmtableattrlisting")) {
      if (!strcmp(Sentries->entries[Sentries->m].val,"ingest"))
	makeExtSchemaTableAttributeListing(Sentries);
      else if (!strcmp(Sentries->entries[Sentries->m].val,"modify"))
	makeModifyExtSchemaTableAttributeListing(Sentries);
      else
	makeDeleteExtSchemaTableAttributeListing(Sentries);
      clFinish(Sentries->conn);
      exit(0);
    }
    



    
    if (!strcmp(Sentries->entries[0].val,"makecontainerform")) {
      makeContainerForm(Sentries);
      clFinish(Sentries->conn);
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"makecontainer")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Container Creation</title> <base target = \"_top\">");
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      makeContainer(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"getcollmetaattr")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<HTML><HEAD> <TITLE> %s %s</TITLE> <BASE TARGET = \"sub%s\"></HEAD>", mySrbStrings[112], Sentries->entries[1].val, Sentries->entries[3].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      getCollMetaAttr(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"createtextfileform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>%s %s</title> <base target = \"sub%s\"></head>", mySrbStrings[113], Sentries->entries[1].val, Sentries->entries[3].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      createTextFileForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"regfile")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Dataset  Registration in %s</title> <base target = \"_top\">", Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      regFile(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"regURL")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>URL  Registration  for %s</title> <base target = \"_top\">",Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      regURL(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"regexec")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Command Registration for %s</title> <base target = \"_top\">",Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      regexec(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"regSQL")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>SQL Registration for %s</title> <base target = \"_top\"></head>",Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      regSQL(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"regORB")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Dataset Ingestion for %s</title> <base target = \"_top\">",Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      regORB(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"regdir")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>Dataset Ingestion for %s</title> <base target = \"_top\"></head>", Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      regDir(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"regfileform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>%s %s</title> <base target = \"sub%s\"></head>", mySrbStrings[149], Sentries->entries[1].val, Sentries->entries[3].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      regFileForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"regurlform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>%s %s</title> <base target = \"sub%s\"></head>", mySrbStrings[154],Sentries->entries[1].val, Sentries->entries[3].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      regURLForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"regexecform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>%s %s</title> <base target = \"sub%s\"></head>", mySrbStrings[167],Sentries->entries[1].val, Sentries->entries[3].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      regExecForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"regsqlform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>%s %s</title> <base target = \"sub%s\"></head>", mySrbStrings[155], Sentries->entries[1].val, Sentries->entries[3].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      regSQLForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"regorbform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>%s %s</title> <base target = \"sub%s\"></head>", mySrbStrings[156], Sentries->entries[1].val, Sentries->entries[3].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      regORBForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"regdirform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>%s %s</title> <base target = \"sub%s\"></head>", mySrbStrings[152],Sentries->entries[1].val, Sentries->entries[3].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      regDirForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"reingestform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Dataset Ingestion for %s</title> <base target = \"_top\">",Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      makeReIngestForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"ingestreplicaform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Dataset Ingestion for %s</title> <base target = \"_top\">",Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      ingestReplicaForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"ingestnewversionform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Dataset Version Ingestion Form for %s</title> <base target = \"_top\">", Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      ingestNewVersionForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"reingest")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Dataset ReIngestion for %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val, Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      reIngestFileIntoSRB(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"ingestreplica")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Dataset ReIngestion for %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      ingestReplicaFileIntoSRB(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"ingestnewversion")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>New Version Ingestion for %s/%s</title> <base target = \"_top\">", Sentries->entries[2].val, Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      ingestNewVersionFileIntoSRB(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"changetypeform")) {
      makeChangeTypeForm(Sentries, Sentries->m);
      clFinish(Sentries->conn);
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"changetype")){
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      hScr();
      printf("<title>Change Type for %s/%s</title> <base target =\"_top\">", Sentries->entries[2].val, Sentries->entries[1].val);
      printf("</head>\n<body background=\"%s/srb3.jpg\">", webPath);
      changetype(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"changeURLform")) {
      makeChangeURLForm(Sentries, Sentries->m);
      clFinish(Sentries->conn);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"changeURL")){
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      hScr();
      printf("<title>Change URL for %s/%s</title> <base target =\"_top\">", Sentries->entries[2].val, Sentries->entries[1].val);
      printf("</head>\n<body background=\"%s/srb3.jpg\">", webPath);
      changeURL(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"changeSQLform")) {
      makeChangeSQLForm(Sentries, Sentries->m);
      clFinish(Sentries->conn);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"changeSQL")){
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      hScr();
      printf("<title>Change SQL Query for %s/%s</title> <base target =\"_top\">", Sentries->entries[2].val, Sentries->entries[1].val);
      printf("</head>\n<body background=\"%s/srb3.jpg\">", webPath);
      changeSQL(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"editdata")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Dataset ReIngestion for %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      editData(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"appenddata")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Append to %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      appendData(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"showallmeta")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Collection Info for %s</title> <base target = \"_top\">",Sentries->entries[2].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      showAllMetaFlag = 1;
      getCollectionInfo(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"put")) {
      ingestFileIntoSRB(Sentries);
      clFinish(Sentries->conn);
      exit(0);
    }    

    if (!strcmp(Sentries->entries[0].val,"makecoll")) {
      makeCollectionInSRB(Sentries);
      clFinish(Sentries->conn);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"queryMetaValues")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf ("<HTML><HEAD> ");
      printf("<TITLE> MYSRB - A TransSystem Data Explorer </TITLE> <BASE TARGET = \"_top\"> </HEAD> ");
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      queryMetaValues(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }
    
    /* datascope specific --sifang */
    if (!strcmp(Sentries->entries[0].val,"queryDatascope")) {
      queryDatascope(Sentries, Sentries->m);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"schemaqueryvalues")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf ("<HTML><HEAD> ");
      printf("<TITLE> MYSRB - A TransSystem Data Explorer </TITLE> <BASE TARGET = \"_top\"> </HEAD> ");
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      schemaQueryValues(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"extschemaattrvalues")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf("<HTML><HEAD> ");
      printf("<TITLE> %s </TITLE> <BASE TARGET = \"_top\">\n", "MYSRB V9.0");

    fprintf(stdout, "<script language=\"JavaScript\">\n\
    function mySubmit(formID, arg){\n\
    if(arg.value != \"doNothing\")\n\
        formID.submit();\n\
    }\n\
    </script>\n");

    fprintf(stdout,"</HEAD> ");
      
        if (!strcmp(mode,"light")) {
            printf("<body bgcolor=\"#FFFFF7\">");
        }
        else {
            printf("<body background=\"%s/srb3.jpg\">", webPath);
        }

      MakeEntriesForQuery(Sentries);
      printf("<CENTER>");
      if (!strcmp(Sentries->entries[Sentries->m].name,"isansdata"))
	extSchemaOnlyQueryValues(Sentries);
      else {
	extSchemaQueryValues(Sentries);
      }
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"browsequeryvalues")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf("<HTML><HEAD> ");
      printf("<TITLE> %s </TITLE> <BASE TARGET = \"_top\">\n", "MYSRB V9.0");

    fprintf(stdout, "<script language=\"JavaScript\">\n\
    function mySubmit(formID, arg){\n\
    if(arg.value != \"doNothing\")\n\
        formID.submit();\n\
    }\n\
    </script>\n");

    fprintf(stdout,"</HEAD> ");
      
        if (!strcmp(mode,"light")) {
            printf("<body bgcolor=\"#FFFFF7\">");
        }
        else {
            printf("<body background=\"%s/srb3.jpg\">", webPath);
        }

      MakeEntriesForQuery(Sentries);
    
      printf("<CENTER>");
      browseQueryValues(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"metainsertionform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf ("<HTML><HEAD> ");
      printf("<TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD> ", "MYSRB V9.0");
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      makeMetaInsertForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"dcmetainsertionform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf ("<HTML><HEAD> ");
      printf("<TITLE> %s </TITLE> <BASE TARGET = \"sub%s\"> </HEAD> ", "MYSRB V9.0", Sentries->pidStr);
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      makeDCMetaInsertForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"replicationform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf ("<HTML><HEAD> ");
      printf("<TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD> ", "MYSRB V9.0");
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      replicationForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"createsnapshotform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf ("<HTML><HEAD> ");
      printf("<TITLE> %s </TITLE> <BASE TARGET = \"sub%s\"> </HEAD> ", "MYSRB V9.0", Sentries->pidStr);
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      createSnapshotForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"aclform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf ("<HTML><HEAD> ");
      printf("<TITLE> %s </TITLE> <BASE TARGET = \"sub%s\"> </HEAD> ", "MYSRB V9.0", Sentries->pidStr);
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      aclForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"giveticketform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf ("<HTML><HEAD> ");
      printf("<TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD> ", "MYSRB V9.0");
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      giveTicketForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"giveticket")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf ("<HTML><HEAD> ");
      printf("<TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD> ", "MYSRB V9.0");
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      giveTicket(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"addtoticketform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf ("<HTML><HEAD> ");
      printf("<TITLE> MYSRB - A TransSystem Data Explorer </TITLE> <BASE TARGET = \"_top\"> </HEAD> ");
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      addToTicketForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"replicate")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf ("<HTML><HEAD> ");
      printf("<TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD> ", "MYSRB V9.0");
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      replicateFileIntoSRB(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"createsnapshot")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf ("<HTML><HEAD> ");
      printf("<TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD> ", "MYSRB V9.0");
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      snapshotFileIntoSRB(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"annoinsertionform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf ("<HTML><HEAD> ");
      printf("<TITLE> MYSRB - A TransSystem Data Explorer </TITLE> <BASE TARGET = \"_top\"> </HEAD> ");
      printf("<body background=\"%s/srb3.jpg\"><CENTER>", webPath);
      makeAnnoInsertForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"insertDataAnnotation")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Annotation Ingestion  for %s/%s</title> <base target = \"_top\">", Sentries->entries[2].val, Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      insertDataAnnotation(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"insertCollAnnotation")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Annotation Ingestion  for %%s</title> <base target = \"_top\">",
	     Sentries->entries[2].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      insertCollAnnotation(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"insertDataComments")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>Comment Ingestion  for %s/%s</title> <base target = \"_top\">",
	     Sentries->entries[2].val, Sentries->entries[1].val);
      hScr();
      printf("</head>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      if (strlen(Sentries->entries[1].val) > 0)
	insertDataComments(Sentries);
      else
	insertCollComments(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"authorize")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Authorization Ingestion  for %s/%s</title> <base target = \"_top\">",
	     Sentries->entries[2].val,Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      insertDataACL(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"insertDataMetaData")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s %s</title> <base target = \"_top\">", mySrbStrings[171], Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      insertDataMetaData(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"ingestExtSchmMetaData")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s %s</title> <base target = \"_top\">", mySrbStrings[171], Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      insertExtSchmMetaData(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }
    if (!strcmp(Sentries->entries[0].val,"modifyExtSchmMetaData")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s %s</title> <base target = \"_top\">", mySrbStrings[207], Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      modifyExtSchmMetaData(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"deleteExtSchmMetaData")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s %s</title> <base target = \"_top\">", mySrbStrings[207], Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);fflush(stdout);
      deleteExtSchmMetaData(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }


    if (!strcmp(Sentries->entries[0].val,"insertCollMetaData")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Sub-Collection Creation for %s</title> <base target = \"_top\">",Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      insertCollMetaData(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"modifymetaform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s %s/%s</title> <base target = \"_top\">", mySrbStrings[207], Sentries->entries[2].val, Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      modifyMetaForm(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"modifyannoform")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>%s %s/%s</title> <base target = \"_top\">", mySrbStrings[213], Sentries->entries[2].val, Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      /* modifyAnnoForm(m);*/
      modifyAnnoFormNew(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"modifyanno")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      if (strlen(Sentries->entries[1].val) > 0) {
    	printf("<title>%s %s/%s</title> <base target = \"_top\">", mySrbStrings[213], Sentries->entries[2].val, Sentries->entries[1].val);
    	printf("<body background=\"%s/srb3.jpg\">", webPath);
    	modifyAnnoData(Sentries);
      }
      else {
    	printf("<title>%s %s</title> <base target = \"_top\">", mySrbStrings[213], Sentries->entries[2].val);
    	printf("<body background=\"%s/srb3.jpg\">", webPath);
    	modifyAnnoColl(Sentries);
      }
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"modifymeta")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      if (strlen(Sentries->entries[1].val) > 0) {
	printf("<title>%s %s/%s</title> <base target = \"_top\">", mySrbStrings[207], Sentries->entries[2].val, Sentries->entries[1].val);
	printf("<body background=\"%s/srb3.jpg\">", webPath);
	modifyDataMeta(Sentries);
      }
      else {
	printf("<title>%s %s</title> <base target = \"_top\">", mySrbStrings[207], Sentries->entries[2].val);
	printf("<body background=\"%s/srb3.jpg\">", webPath);
	modifyCollMeta(Sentries);
      }
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"copyanno")){
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Copy Annottaions from %s/%s to %s/%s </title> <base target = \"_top\">",
	     Sentries->entries[2].val, Sentries->entries[1].val, Sentries->entries[Sentries->m-1].val, Sentries->entries[Sentries->m].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<FONT COLOR=#0000FF>Copy Annottaions from</FONT> <FONT COLOR=#FF0000>%s/%s</FONT>  <FONT COLOR=#0000FF>to </FONT><FONT COLOR=#FF0000>%s/%s </FONT><BR>\n",
	     Sentries->entries[2].val, Sentries->entries[1].val, Sentries->entries[Sentries->m-1].val, Sentries->entries[Sentries->m].val);
      copyanno(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"copymeta")){
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Copy Metadata from %s/%s to %s/%s </title> <base target = \"sub%s\">",
	     Sentries->entries[2].val, Sentries->entries[1].val, Sentries->entries[Sentries->m-1].val, Sentries->entries[Sentries->m].val, Sentries->pidStr);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<FONT COLOR=#0000FF>Copy Metadata from</FONT> <FONT COLOR=#FF0000>%s/%s</FONT>  <FONT COLOR=#0000FF>to </FONT><FONT COLOR=#FF0000>%s/%s </FONT><BR>\n",
	     Sentries->entries[2].val, Sentries->entries[1].val, Sentries->entries[Sentries->m-1].val, Sentries->entries[Sentries->m].val);
      copymeta(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"copyds")){
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Copy %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      if (strlen(Sentries->entries[1].val) > 0) 
	    copydsData(Sentries);
      else
	    copydsColl(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"moveds")){
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Move %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      if (strlen(Sentries->entries[1].val) > 0) 
	    movedsData(Sentries);
      else
	    movedsColl(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"renameds")){
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      hScr();
      printf("<title>Rename %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
      printf("</head>\n<body background=\"%s/srb3.jpg\">", webPath);
      renameds(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"slinks")){
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Soft Link  %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      slinks(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"slinkColl")){
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><head>");
      printf("<title>Soft Link  %s/%s</title> <base target = \"sub%s\"></head>\n",Sentries->entries[8].val,Sentries->entries[7].val, Sentries->pidStr);
      printf("<body background=\"%s/srb3.jpg\">\n", webPath);
      slinkColl(Sentries, Sentries->entries[8].val, Sentries->entries[7].val, Sentries->entries[2].val);
      bRMap(2,1);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }
   
    if (!strcmp(Sentries->entries[0].val,"showoptions")) {

      return 0;
    }  

    if (!strcmp(Sentries->entries[0].val,"choptions")) {
      strcpy(Sentries->startcol, Sentries->entries[5].val);
      saveOptions(Sentries);
      return 0;
    }      

    if (!strcmp(Sentries->entries[0].val,"downloadfile")){
      sendDownloadData(Sentries);
    }

    if (!strcmp(Sentries->entries[0].val,"slinkdownloadfile")){
      sendDownloadData(Sentries);
    }

    if (!strcmp(Sentries->entries[0].val,"checkout")){
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Check out  %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      checkout();
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }
    if (!strcmp(Sentries->entries[0].val,"offline")){
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Lock File  %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      offline();
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"dbcopy")) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<title>Collection Info for %s</title> <base target = \"_top\">",Sentries->entries[2].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      makeDbCopy(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"slinkmeta")) {
        printf("Content-type: text/html%c%c",10,10);fflush(stdout);
        printf("<html>");
	    printf("<title>Metadata Info for %s:%s</title> <base target = \"_top\">",Sentries->entries[2].val,
	    Sentries->entries[1].val);
        printf("<body background=\"%s/srb3.jpg\">", webPath);
	  slinkFlag = 1;
      showMeta(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }
    
    if (!strcmp(Sentries->entries[0].val,"meta")) {
        printf("Content-type: text/html%c%c",10,10);fflush(stdout);
        printf("<html>");
	    printf("<title>Metadata Info for %s:%s</title> <base target = \"_top\">",Sentries->entries[2].val, Sentries->entries[1].val);

        if (!strcmp(mode,"light")) {
            printf("<body bgcolor=\"#FFFFF7\">");
        }
        else {
            printf("<body background=\"%s/srb3.jpg\">", webPath);
        }
        
      	/*    showMeta(); */
	Sentries->showMetaType = 0;
	
	showMetaNew(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"dcmeta")) {
        printf("Content-type: text/html%c%c",10,10);fflush(stdout);
        printf("<html>");
	    printf("<title>Metadata Info for %s:%s</title> <base target = \"_top\">", Sentries->entries[2].val, Sentries->entries[1].val);
        printf("<body background=\"%s/srb3.jpg\">", webPath);
      makeDCMetaPage(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"showanno")) {
        printf("Content-type: text/html%c%c",10,10);fflush(stdout);
        printf("<html>");
	printf("<title>Annotation Info for %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,
	       Sentries->entries[1].val);
        printf("<body background=\"%s/srb3.jpg\">", webPath);
      showAnno(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"anno")) {
        printf("Content-type: text/html%c%c",10,10);fflush(stdout);
        printf("<html>");
	    printf("<title>Annotation Info for %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,
	       Sentries->entries[1].val);
        printf("<body background=\"%s/srb3.jpg\">", webPath);
      makeAnnoPage(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

   if(!strcmp(Sentries->entries[0].val,"editform")) {
     printf("Content-type: text/html%c%c",10,10);fflush(stdout);
     printf("<html>");
     printf("<title>Editing %s/%s</title> <base target = \"_top\">",
	    Sentries->entries[2].val,Sentries->entries[1].val);
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     sendEditForm(Sentries);
     clFinish(Sentries->conn);
     printf("</body></html>");
     exit(0);
   }

   if(!strcmp(Sentries->entries[0].val,"appendform")) {
     printf("Content-type: text/html%c%c",10,10);fflush(stdout);
     printf("<html>");
     printf("<title>Editing %s/%s</title> <base target = \"_top\">",
	    Sentries->entries[2].val,Sentries->entries[1].val);
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     sendAppendForm(Sentries);
     clFinish(Sentries->conn);
     printf("</body></html>");
     exit(0);
   }

   if (!strcmp(Sentries->entries[0].val,"slinkdata")) {
     Sentries->slinkFlag = 1;
     if (strcmp(Sentries->entries[6].val,"-1")) {
       printf("Content-type: text/html%c%c",10,10);fflush(stdout);
       printf("<html><head>");
       printf ("<META HTTP-EQUIV=\"expires\" content=\"Wed, 26 Feb 1997 08:21:57 GMT\">");
       printf("<title>Collection Info for %s</title></head>\n", Sentries->entries[2].val);
       printf("<base target = \"_top\">");
       printf("<body background=\"%s/srb3.jpg\">", webPath); 
       getSlinkCollectionInfo(Sentries);
       printf("</body></html>");
     }
      else {
	tmpPtr = strstr(Sentries->entries[1].val,"@SHADOW");
	
	if (tmpPtr != NULL) 
	    *tmpPtr='&';
	sendSlinkData(Sentries);
      }   
      clFinish(Sentries->conn);
      exit(0);
    }

    if (!strcmp(Sentries->entries[0].val,"execdata")) {
      printf("Content-type: text/html%c%c",10,10);
      printf("<html><head>");
      printf("<META HTTP-EQUIV=\"expires\" content=\"Wed, 26 Feb 1997 08:21:57 GMT\">");
      printf("<title>Command Execution: %s</title></head>\n",Sentries->entries[5].val);
      printf("<base target = \"_top\">", Sentries->entries[2].val);
      printf("<body background=\"%s/srb3.jpg\">", webPath); fflush(stdout);
      execData(Sentries);
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(0);
    }

   if (strlen( Sentries->entries[1].val) == 0 || strstr(Sentries->entries[1].val,"*") || strstr(Sentries->entries[1].val,"?") ||strstr(Sentries->entries[2].val,"*") || strstr(Sentries->entries[2].val,"?") )
      {
        printf("Content-type: text/html%c%c",10,10);fflush(stdout);
        printf("<html><head>");
        if (!strcmp(mode,"light")) {
                printf("<link href=\"%s/original.css\" rel=\"stylesheet\" type=\"text/css\">", webPath);
        }
    	printf("<META HTTP-EQUIV=\"expires\" content=\"Wed, 26 Feb 1997 08:21:57 GMT\">");
    	printf("<title>Collection Info for %s</title>\n", Sentries->entries[2].val);
    	printf("<base target = \"sub%s\">\n", Sentries->entries[8].val);

        fprintf(stdout, "<script language=\"JavaScript\">\n\
        function mySubmit(formID, arg){\n\
        if(arg.value != \"doNothing\")\n\
            formID.submit();\n\
        }\n\
        </script>\n");

    	printf("</head>");    	
    	
        if (!strcmp(mode,"light")) {
            printf("<body bgcolor=\"#FFFFF7\">");
            getCollectionInfo_simple(Sentries);
        }
        else {
            printf("<body background=\"%s/srb3.jpg\">", webPath);
            getCollectionInfo(Sentries);
        }
        
        clFinish(Sentries->conn);
        printf("</body></html>");
        exit(0);
      }

    if (!strcmp(Sentries->entries[0].val,"sqldata")) {
      strcpy(Sentries->entries[0].val,"data");
      sprintf(Sentries->sqlDataName,"%s&SHADOW= %s",Sentries->entries[1].val, Sentries->entries[8].val); /* space before entries[8].val is required */
      Sentries->entries[1].val = Sentries->sqlDataName;
    }
 
    if(!strcmp(Sentries->entries[0].val,"hrefdata")) {
     splitpath(Sentries->entries[1].val, collName, dataName);
     strcpy(Sentries->entries[1].val, dataName);
     strcpy(Sentries->entries[2].val, collName);
     Sentries->m = 2;
    }

    if(!strcmp(Sentries->entries[0].val,"data")) {
      sendData(Sentries);
    }   
    

    Sresult->pageType = -1; /*** Function Unavailable ***/
    Sresult->entr0 = strdup(Sentries->entries[0].val);
    clFinish(Sentries->conn);  
      
/********** temporary ************
    exit(0);
/*********************************/

    return 0;
}

int displayResult(outStruct Sresult) {

    if (!strcmp(Sresult->Sentry->entries[0].val,"showoptions")) {
    
    showOptionsMenu(Sresult);
    
    return 0;
  

    }




    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html>");
    printf("<body background=\"%s/srb3.jpg\">", webPath);

    if (Sresult->pageType == -1) {

        printf("<B>Function Unavailable: %s</B>\n", Sresult->entr0);
        printf("</body></html>");    
    }
    
    return 0;    
}

int saveOptions(inStruct Sentries) {
    char sessionId[MAX_TOKEN];
    char buf[MAX_U_ENTRIES][MAX_TOKEN];
    int i;

    sprintf(sessionId,"%s/%s",logDirec,Sentries->cookieStr);
    
    fId = fopen(sessionId,"r");
    
    if (fId == NULL) {
        printf("Content-type: text/html%c%c",10,10);fflush(stdout);
        printf("<html>");
        printf("<body background=\"%s/srb3.jpg\">", webPath);
        printf("<br>Error: No Session Information\n");
        printf("</body></html>");
        fflush(stdout);
        exit(1);
    }

    for (i=0; i < MAX_U_ENTRIES; i++) {
         fgets(buf[i], MAX_TOKEN, fId);
    }

    fclose(fId);

 /*** pretty heavy to close and re-open the file, but better for portability ***/
    fId = fopen(sessionId,"w");

    for (i=0; i<10; i++) {
        fprintf(fId, "%s", buf[i]);
    }

    fprintf(fId, "%s\n", Sentries->entries[3].val);
    fprintf(fId, "%s\n", Sentries->entries[4].val);

    for (i=12; i < MAX_U_ENTRIES; i++) {
        fprintf(fId, "%s", buf[i]);
    }

    fclose(fId);    

 /** the lang file has to be read again in case language has been changed **/
    strcpy(Sentries->currentLanguage, Sentries->entries[3].val);
    getMode(Sentries->entries[4].val);
    readLangFile(langDir, Sentries->currentLanguage);

    
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html><HEAD>\n<TITLE>%s</TITLE> <BASE TARGET = \"_top\"></HEAD>\n", mySrbStrings[22]);
    printf("<body background=\"%s/srb3.jpg\">", webPath);
       
    /** user must see if mode has changed **/
        if (!strcmp(mode,"light")) {
    printf("<CENTER><B><FONT COLOR=#0000FF>%s (%s)</FONT></B><P>\n", mySrbStrings[10], mySrbStrings[24]);
        }
        else {
    printf("<CENTER><B><FONT COLOR=#0000FF>%s</FONT></B><P>\n", mySrbStrings[10]);
        }

     if (!Sentries->startcol || !strcmp(Sentries->startcol,"")) {
        sprintf(Sentries->startcol, "/home/%s.%s", Sentries->entries[1].val, Sentries->entries[2].val);
        }
    
    printf("<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" >\n\
<input type=hidden name=\"function\" value=\"treefull\">\n\
<input type=hidden name=\"dataname\" value=\"\">\n\
&nbsp; &nbsp; <STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n\
<INPUT NAME=\"collection\", VALUE= \"%s\" , SIZE=30,12>\n<BR>\
&nbsp; &nbsp; <FONT COLOR=#0000FF>%s</FONT><BR> \n\
<INPUT type=hidden NAME=\"seek\", VALUE= \"0\">\n",
	 Sentries->pidStr, mySrbStrings[11], Sentries->startcol, mySrbStrings[12]);
  printf("<input type=hidden name=\"copynum\" value=\"0\">");
  printf("<INPUT NAME=\"datatype\" type=hidden VALUE=\"\">\n");
  printf("<INPUT NAME=\"vernum\" type=hidden VALUE=\"1\">\n");
  printf("<INPUT NAME=\"owner\" type=hidden VALUE=\"%s@%s\">\n", Sentries->entries[1].val, Sentries->entries[2].val);
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[13]);


  /*printf("<P><CENTER><TABLE WIDTH=50%%><TR><TD BGCOLOR=\"#DDEFFF\" align=center >&nbsp;<P> */
    printf("<P><CENTER><TABLE WIDTH=50%%><TR><TD align=center >&nbsp;<P> \
<A HREF=\"%s/%s\" target=\"_self\"><IMG SRC=\"%s/%s\" ALT=\"Help\"><BR>%s</A><P>&nbsp;</TD>\n", webPath, mySrbStrings[56], webPath, "help.gif", mySrbStrings[14]);

 printf("<TD  align=center >&nbsp;<P> \
<A HREF=\"mysrb331.cgi?function=showoptions&username=%s&domainname=%s&startcol=%s&session=%s\" target=\"_self\" ><IMG SRC=\"%s/%s\" ALT=\"Exit MySRB\"><BR>%s</A><P>&nbsp;</TD>\n", Sentries->entries[1].val, Sentries->entries[2].val, Sentries->startcol, Sentries->pidStr, webPath, "opmenu.gif", mySrbStrings[17]);

    /*    printf("<TD BGCOLOR=\"#DDEFFF\" align=center >&nbsp;<P> */
    printf("<TD  align=center >&nbsp;<P> \
<A HREF=\"mysrb331.cgi?function=unmount&session=%s\" target=\"_self\" ><IMG SRC=\"%s/%s\" ALT=\"Exit MySRB\"><BR>%s</A><P>&nbsp;</TD></TABLE></CENTER>\n", Sentries->pidStr, webPath, "exitmysrb.gif", mySrbStrings[15]);
  printf("</BODY>\n</HTML>\n");
    exit(0);
}


int showOptionsMenu(outStruct Sresult) {
    
    
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><HEAD>\n<TITLE>%s</TITLE> <BASE TARGET = \"_top\"></HEAD>\n", mySrbStrings[22]);
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<B><CENTER><FONT COLOR=#0000FF>%s</FONT></B><P>\n\
<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"_self\" >\n\
<input type=hidden name=\"function\" value=\"choptions\">\n\
<input type=hidden name=\"username\" value=\"%s\">\n\
<input type=hidden name=\"domainname\" value=\"%s\">", mySrbStrings[18],Sresult->Sentry->entries[1].val, Sresult->Sentry->entries[2].val);


    printf("<TABLE>");

    printf("<tr>\n\
<td align=RIGHT><strong><font color=#FF0000>%s:</font></strong></td>\n\
<td><SELECT name=lang>", mySrbStrings[19]);


 /* getLangfiles() prints options for select menu */
    getLangFiles(Sresult->Sentry);


    printf("</SELECT></td></tr>");

    printf("<tr><td align=RIGHT><strong><font color=#FF0000>%s:</font></strong></td>\n\
<td><SELECT name=mode>", mySrbStrings[20]);

/* this is temporay, the modes will not be hard written in the code */
    if (!strcmp(mode, "advanced"))
        printf("<OPTION SELECTED");
    else
        printf("<OPTION");

    printf (" VALUE=\"advanced\">%s", mySrbStrings[23]);
    
    if (!strcmp(mode, "light"))
        printf("<OPTION SELECTED");
    else
        printf("<OPTION");    
        
    printf(" VALUE=\"light\">%s</SELECT></td>\n\
</tr>", mySrbStrings[24]);

    printf("<input type=hidden name=\"startcol\" value=\"%s\">\n", Sresult->Sentry->entries[3].val);
 
    printf("<input type=hidden name=\"session\" value=\"%s\">\n\
<tr><td align=center colspan=2>\n\
<INPUT TYPE=\"submit\" VALUE=\"  OK  \"></FORM></TD></TR></TABLE>", Sresult->Sentry->pidStr);    
      
      
    printf("<P><CENTER><TABLE WIDTH=50%%><TR><TD align=center ><P>\n\
    <A HREF=\"%s/%s\" target=\"_self\"><IMG SRC=\"%s/%s\" ALT=\"Help\"><BR>%s</A><P></TD>\n", webPath, mySrbStrings[56], webPath, "help.gif", mySrbStrings[14]);

    printf("<TD  align=center ><P>\n\
    <A HREF=\"mysrb331.cgi?function=unmount&session=%s\" target=\"_self\" ><IMG SRC=\"%s/%s\" ALT=\"Exit MySRB\"><BR>%s</A><P></TD></TABLE>\n", Sresult->Sentry->pidStr, webPath, "exitmysrb.gif", mySrbStrings[15]);


      
        
      printf("</CENTER></body></html>"); 
       
      return 0;
}


int getLangFiles(inStruct Sentries) {
    struct dirent *dp;
    DIR *dirp = opendir(langDir);
    char *tmpPtr;
    
    while ((dp = readdir(dirp)) != NULL) {
        if (tmpPtr=strstr(dp->d_name, "mySrbLang_")) {
            tmpPtr+=strlen("mySrbLang_");
            if (!strcmp(tmpPtr,Sentries->currentLanguage)) {
                printf("<OPTION SELECTED VALUE=\"%s\">%s", tmpPtr, tmpPtr);
            }
            else {
                printf("<OPTION VALUE=\"%s\">%s", tmpPtr, tmpPtr);
            }
        }
    }
    
    closedir(dirp);

    
//  When modularisation complete add meter q in main loop and 
//	if (q==0) {
//	    webErrorExit("No language file found",0);
//	}
	
  
    return 0;    
    
    
}


/************   end of new functions for layer based architecture    ********/



sendSlinkData(inStruct Sentries) {
   sendData(Sentries);  

}



sendDownloadData(inStruct Sentries) {
    char objType[HUGE_STRING];
    char collName[HUGE_STRING];
    char dataName[HUGE_STRING];
    int dataseek, copyNum;
    int datasize;
    char contentType[100];

    dataseek = 0;
    datasize = 0;
    copyNum = -1;
    heading = 1;
    strcpy(collName,"");
    strcpy(dataName,Sentries->entries[1].val);
 
    if (Sentries->m >= 2) 
      strcpy(collName,Sentries->entries[2].val);
    if (Sentries->m >= 6) 
      sprintf(dataName,"%s&COPY=%s&DVERSION='%s'",Sentries->entries[1].val,Sentries->entries[4].val,Sentries->entries[6].val);
    else if (Sentries->m >= 4) 
      sprintf(dataName,"%s&COPY=%s",Sentries->entries[1].val,Sentries->entries[4].val);

    in_fd = srbObjOpen (Sentries->conn, dataName,  O_RDONLY, collName);
    if (in_fd < 0)  {   /* error */
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<H1> %s/%s </H1><HR>",collName,dataName);fflush(stdout);
      printf("can't open obj \"%s\" in collection \"%s\": <A href=\"srberror2.cgi?%i\">Error: %i</A> \n",
	     dataName, collName, in_fd, in_fd);
      clFinish(Sentries->conn);
      bRMap(1,0);
      printf("</body></html>");
      exit(1);
    }

    printf("Content-type: application/srb.download%c%c",10,10);fflush(stdout);
    while ((nbytes = srbObjRead (Sentries->conn, in_fd, buf, MYSRBBUFSIZE)) > 0) 
      write(1, buf, nbytes);
    srbObjClose (Sentries->conn, in_fd);
    clFinish(Sentries->conn);
    exit(0);
}

sendAppendForm(inStruct Sentries)
{

  printf("<center><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"appenddata\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"seek\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
  printf("<B><FONT COLOR=#0000FF>Please Place Append Text in TextArea</FONT></B><BR>\n");
  printf("<textarea rows=30 cols=90 name=\"dataval\"> </textarea>\n");
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM><BR>\n", mySrbStrings[133]);
  printf("<B><FONT COLOR=#0000FF>Use Back Button if you do not want to append now</FONT></B></center></body></html>\n");

}

sendEditForm(inStruct Sentries)
{
    char objType[HUGE_STRING];
    char collName[HUGE_STRING];
    char dataName[HUGE_STRING];
    int dataseek, copyNum;
    int datasize;
   char contentType[100];

  dataseek = 0;
  datasize = 0;
  copyNum = -1;
  heading = 1;
  strcpy(collName,"");
  strcpy(dataName,Sentries->entries[1].val);
    if (Sentries->m >= 2) 
      strcpy(collName,Sentries->entries[2].val);
    if (strlen(Sentries->entries[4].val) > 0){
      strcat(dataName,"&COPY=");
      strcat(dataName,Sentries->entries[4].val);
    }

    if (strlen(Sentries->entries[6].val) > 0){
      strcat(dataName,"&DVERSION='");
      strcat(dataName,Sentries->entries[6].val);
      strcat(dataName,"'");
    }


    in_fd = srbObjOpen (Sentries->conn, dataName,  O_RDONLY, collName);
    if (in_fd < 0)  {   /* error */
      printf("Error: Cannot  open Object: \"%s\" in collection \"%s\": <A href=\"srberror2.cgi?%i\">Error: %i</A> \n",
	     dataName, collName,in_fd,in_fd);
      clFinish(Sentries->conn);
      bRMap(1,0);
      printf("</body></html>");
      exit(1);
    }
    printf("<center><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"editdata\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"seek\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
  printf("<B><FONT COLOR=#0000FF>Please Edit the Contents of the Textbox</FONT></B><BR>\n");
  printf("<textarea rows=30 cols=90 name=\"dataval\">\n");
  while ((nbytes = srbObjRead (Sentries->conn, in_fd, buf, MYSRBBUFSIZE-2)) > 0) {
    buf[nbytes] ='\0';
    substituteString(buf,"</textarea>" , "</areatext>");
    printf("%s",buf);
  }  
  printf("</textarea>\n");
  srbObjClose (Sentries->conn, in_fd);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM><BR>\n", mySrbStrings[89]);
  printf("<B><FONT COLOR=#0000FF>Use Back Button if you do not want to edit now</FONT></B></center></body></html>\n");
}

/* datascope specific --sifang */
int
queryDatascope(inStruct Sentries, int m)
{
  int i,j;

  tmpoStr[0] = '\0';
  j  = 8;
  for (i = 0; i < 5 ; i++) {
    if (strlen(Sentries->entries[8 + (i * 3)].val) <= 0)
      continue;

    sprintf(&tmpoStr[strlen(tmpoStr)], "%s %s %s;;", 
	    Sentries->entries[8 +(i*3)].val,
	    Sentries->entries[9 +(i*3)].val,
	    Sentries->entries[10 +(i*3)].val);
  }
  if (strlen(tmpoStr) > 0) {
    tmpoStr[strlen(tmpoStr)-1] = '\0';
    tmpoStr[strlen(tmpoStr)-1] = '\0';
    strcat(Sentries->entries[1].val,"&SHADOW=<DSPROCESS>");
    strcat(Sentries->entries[1].val,tmpoStr);
    strcat(Sentries->entries[1].val,"</DSPROCESS>");
  }
  strcat(Sentries->entries[8].val,Sentries->entries[m].val);
  sendData(Sentries);
  return(0);
}

/* datascope specific --sifang */
int
getValuesAsSelectFromDatascope(inStruct Sentries, int fd, char *buf, int bufSize,  int queryType)
{
  int i;
  char *tmpStr, *tmpPtr, *tmpPtr1;

    sprintf(tmpoStr, "dbquery|%d",queryType);
    buf[0] = '\0';
    i = srbObjProc(Sentries->conn, fd , tmpoStr, "",0,buf, bufSize);
    if (i < 0) {
      sprintf(buf,"<SELECT SIZE=1 NAME=des%d><OPTION SELECTED></SELECT>",queryType);
      return(i);
    }
    tmpStr = strdup(buf);
    tmpPtr = tmpStr;
    /**
    i = 4;
    while ((tmpPtr1 = strchr(tmpPtr,'|')) != NULL) {
      tmpPtr = tmpPtr1 + 1;
      i--;
      if (i == 0)
	break;
    }
    ***/
    sprintf(buf,"<SELECT SIZE=1 NAME=des%i><OPTION SELECTED>",queryType);
    while ((tmpPtr1 = strchr(tmpPtr,'|')) != NULL) {
      *tmpPtr1 = '\0';
      strcat(buf,"<OPTION>");
      strcat(buf,tmpPtr);
      tmpPtr = tmpPtr1 + 1;
    }
    strcat(buf,"</SELECT>");
    free(tmpStr);
    return(0);
}

/* datascope specific --sifang */
int 
connectDatascope(inStruct Sentries) 
{
    char objType[HUGE_STRING];
    char collName[HUGE_STRING];
    char dataName[HUGE_STRING];
    int dataseek, copyNum;
    int datasize;
    char contentType[100];
    int in_fd;
    copyNum = -1;

  strcpy(collName,Sentries->entries[2].val);
  strcpy(selectData,Sentries->entries[1].val);
  if (strlen(Sentries->entries[4].val) > 0){
    strcat(selectData,"&COPY=");
    strcat(selectData,Sentries->entries[4].val);
  }
  if (strlen(Sentries->entries[6].val) > 0){
    strcat(selectData,"&DVERSION='");
    strcat(selectData,Sentries->entries[6].val);
    strcat(selectData,"'");
  }

  in_fd = srbObjOpen (Sentries->conn, selectData,  O_RDONLY, collName);
  if (in_fd < 0)  {   /* error */
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html>");
    printf("<body background=\"%s/srb2.jpg\">", webPath);
    printf("<H1> %s </H1><HR>",Sentries->entries[1].val);fflush(stdout);
    printf("Cannot Open Object \"%s\" in collection \"%s\": <A href=\"srberror.cgi?%i\">Error: %i</A> \n",
	   selectData, Sentries->entries[2].val,in_fd,in_fd);
    clFinish(Sentries->conn);
    bRMap(1,0);
    printf("</body></html>");
    exit(1);
  }
  return(in_fd);
}

sendData(inStruct Sentries) {
    char objType[HUGE_STRING];
    char collName[HUGE_STRING];
    char dataName[HUGE_STRING];
    int dataseek, copyNum;
    int datasize;
    char contentType[100];

    dataseek = 0;
    datasize = 0;
    copyNum = -1;
    heading = 1;

    if (Sentries->m >= 2) strcpy(collName, Sentries->entries[2].val);
    else  strcpy(collName,"");
    strcpy(Sentries->selectData, Sentries->entries[1].val);
    if (strlen(Sentries->entries[4].val) > 0) {
      strcat(Sentries->selectData,"&COPY=");
      strcat(Sentries->selectData, Sentries->entries[4].val);
    }

    if (strlen(Sentries->entries[6].val) > 0 && !Sentries->slinkFlag){
      strcat(Sentries->selectData, "&DVERSION='");
      strcat(Sentries->selectData, Sentries->entries[6].val);
      strcat(Sentries->selectData, "'");
    }

    /**
    if (m >= 4) {
      sprintf( selectData,"%s&COPY=%s&DVERSION='%s'",entries[1].val,entries[4].val,entries[6].val);
    **/
      /*      strcat(entries[1].val,"&COPY=");
	      strcat(entries[1].val,entries[4].val);*/
    /* }   */

    Sentries->in_fd = srbObjOpen(Sentries->conn, Sentries->selectData,  O_RDONLY, collName);

    if (Sentries->in_fd < 0)  {   /* error */
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<H1> %s </H1><HR>", Sentries->entries[1].val);fflush(stdout);
      printf("Cannot Open Object \"%s\" in collection \"%s\": <A href=\"srberror.cgi?%i\">Error: %i</A> \n",
	     Sentries->selectData, Sentries->entries[2].val, Sentries->in_fd, Sentries->in_fd);
      clFinish(Sentries->conn);
      bRMap(1,0);
      printf("</body></html>");
      exit(1);
    }
    /*****
    if (m >= 3) {
      dataseek = atoi(entries[3].val);
      if (dataseek > 0) {
	i = srbObjSeek(Sentries->conn, in_fd, dataseek, SEEK_CUR);
	if (i != dataseek) { 
	  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf("<html>");
	  printf("<body background=\"%s/srb3.jpg\">", webPath);
	  printf("<H1> %s </H1><HR>",entries[4].val);fflush(stdout);
	  printf("can't seek in obj \"%s\" in collection \"%s\": <A href=\"srberror2.cgi?%i\">Error: %i</A>\n",
		 selectData, entries[2].val,i,i);
	  srbObjClose (Sentries->conn, in_fd);
	  clFinish(Sentries->conn);
	  bRMap(1,0);
	  printf("</body></html>");
	  exit(1);
	}
      }
    }
    if (m >= 5) {
      datasize = atoi(entries[5].val);
    }
    *****/
    
    getContentType(Sentries, collName, contentType);


    if (!strcmp(contentType,"jpeg image")|| strstr(Sentries->entries[1].val,".jpg")!= NULL || strstr(Sentries->entries[1].val,".jpeg")!= NULL )
	printf("Content-type:image/jpeg %c%c",10,10);
    else if (!strcmp(contentType,"gif image")|| strstr(Sentries->entries[1].val,".gif")!= NULL)
	printf("Content-type:image/gif %c%c",10,10);
    else if (!strcmp(contentType,"tiff image")|| strstr(Sentries->entries[1].val,".tif")!= NULL || strstr(Sentries->entries[1].val,".tiff")!= NULL )
	printf("Content-type:image/tiff %c%c",10,10);
    else if (!strcmp(contentType,"Power Point Slide")|| strstr(Sentries->entries[1].val,".ppt")!= NULL )
	printf("Content-type:application/vnd.ms-powerpoint %c%c",10,10);
    else if (!strcmp(contentType,"FITS image")|| strstr(Sentries->entries[1].val,".fit")!= NULL || strstr(Sentries->entries[1].val,".fits")!= NULL )
	printf("Content-type:application/x-fits %c%c",10,10);
    else if (!strcmp(contentType,"DICOM image")|| strstr(Sentries->entries[1].val,".IMA")!= NULL )
	printf("Content-type:application/dicom %c%c",10,10);
    else if (!strcmp(contentType,"Quicktime Movie")|| strstr(Sentries->entries[1].val,".mov")!= NULL )
	printf("Content-type:video/quicktime %c%c",10,10);
    else if (!strcmp(contentType,"MPEG Movie")|| strstr(Sentries->entries[1].val,".mpg")|| strstr(Sentries->entries[1].val,".mpeg"))
	printf("Content-type:video/mpeg %c%c",10,10);
    else if (!strcmp(contentType,"MSWord Document")|| strstr(Sentries->entries[1].val,".doc")!= NULL || strstr(Sentries->entries[1].val,".rtf")!= NULL)
	printf("Content-type:application/msword %c%c",10,10);
    else if (!strcmp(contentType,"Excel Spread Sheet")|| strstr(Sentries->entries[1].val,".xls")!= NULL )
	printf("Content-type:application/x-msexcel %c%c",10,10);
    else if (!strcmp(contentType,"ppm image")|| strstr(Sentries->entries[1].val,".ppm")!= NULL )
	printf("Content-type:image/x-portable-pixmap %c%c",10,10);
    else if (!strcmp(contentType,"html")|| strstr(Sentries->entries[1].val,".htm")!= NULL )
	printf("Content-type:text/html %c%c",10,10);
    else if (!strcmp(contentType,"xml")|| strstr(Sentries->entries[1].val,".xml")!= NULL )
	printf("Content-type:text/xml %c%c",10,10);
    else if (!strcmp(contentType,"realAudio")|| strstr(Sentries->entries[1].val,".ra")!= NULL )
	printf("Content-type:audio/x-pn-realaudio  %c%c",10,10);
    else if (!strcmp(contentType,"realVideo")|| strstr(Sentries->entries[1].val,".rv")!= NULL )
	printf("Content-type:audio/x-pn-realaudio  %c%c",10,10);
    else if  (strstr(contentType,"PDF") != NULL|| strstr(Sentries->entries[1].val,".pdf")!= NULL ) 
      printf("Content-type:application/pdf %c%c",10,10);
    else if  (strstr(contentType,"powerpoint") != NULL || strstr(Sentries->entries[1].val,".ppt")!= NULL ) 
      printf("Content-type:application/vnd.ms-powerpoint %c%c",10,10);
    else if  (strstr(contentType,"AVI") != NULL|| strstr(Sentries->entries[1].val,".avi")) 
      printf("Content-type:video/msvideo %c%c",10,10);
    else if  (strstr(contentType,"Postscript") != NULL || strstr(Sentries->entries[1].val,".ps")!= NULL ) {
      printf("Content-type:application/postscript %c%c",10,10);
    }
    else if  (strstr(contentType,"database") != NULL || strstr(contentType,"orb data") != NULL) {
      if ((strstr(Sentries->entries[1].val,"XML") != NULL) ||
	  (strstr(Sentries->entries[1].val,".xml") != NULL))
	printf("Content-type:text/xml %c%c",10,10);
      else if ((strstr(Sentries->entries[1].val,"HTML") != NULL) ||
	       (strstr(Sentries->entries[1].val,".html") != NULL))
	printf("Content-type:text/html %c%c",10,10);
      else {
	printf("Content-type: text/html %c%c",10,10);
	printf("<html><body background=\"%s/srb3.jpg\"><PRE>", webPath);
      }
    }
    else {
      printf("Content-type: text/html %c%c",10,10);
      printf("<html><body background=\"%s/srb3.jpg\"><PRE>", webPath);

    }
 
    fflush(stdout);
    
   /* datascope specific --sifang */
   /* RAJA ADDED for datascopeimages stuff */
   if ( !strcmp(Sentries->entries[0].val,"queryDatascope")) {
      printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"><input type=hidden name=\"function\" value=\"sqldata\"><input type=hidden name=\"data\" value=\"%s\" ><input type=hidden name=\"coll\" value=\"%s\" ><input type=hidden name=\"seek\" value=\"0\"><input type=hidden name=\"copynum\" value=\"\"><input type=hidden name=\"datatype\" value=\"\"><input type=hidden name=\"vernum\" value=\"\"><input type=hidden name=\"owner\" value=\"\">",
        dataName,collName); fflush(stdout);
   }
   /* RAJA ADDED for datascopeimages stuff */

   if (datasize == 0) {
     while ((Sentries->nbytes = srbObjRead (Sentries->conn, Sentries->in_fd, Sentries->buf, MYSRBBUFSIZE)) > 0) {
       write(1, Sentries->buf, Sentries->nbytes);
     }
   }
   else {
     while (datasize > 0) {
       if (datasize > MYSRBBUFSIZE)
	 Sentries->nbytes = srbObjRead (Sentries->conn, Sentries->in_fd, Sentries->buf, MYSRBBUFSIZE);
       else
	 Sentries->nbytes = srbObjRead (Sentries->conn, Sentries->in_fd, Sentries->buf, datasize);
       if (Sentries->nbytes > 0)
	 write(1, Sentries->buf, Sentries->nbytes);
       else
	 break;
       datasize = datasize - Sentries->nbytes;
     }
   }
    srbObjClose (Sentries->conn, Sentries->in_fd);
    clFinish(Sentries->conn);
    
    /* datascope specific --sifang */
    /* RAJA ADDED for datascopeimages stuff */
    if ( !strcmp(Sentries->entries[0].val,"queryDatascope")) {
	    printf("<input type=hidden name=\"session\" value=\"%s\"><BR><INPUT TYPE=\"submit\" VALUE=\"Show Selection\"></FORM>",Sentries->pidStr); 
	    fflush(stdout);
    }
    /* RAJA ADDED for datascopeimages stuff */
    exit(0);

}

getContentType(inStruct Sentries, char *collName,  char *contentType)
{
    mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;
    char *tmpPtr;
    char *dataName = Sentries->entries[1].val;
    
    strcpy(contentType, "");
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if ((tmpPtr = strstr(dataName,"&")) == NULL) {
      sprintf(qval[DATA_NAME], " =  '%s'",dataName);
    }
    else {
      while(isspace(*dataName )) dataName++;
      if (*dataName == '&')
	set_qval_data_cond(dataName,qval);
      else {
	*tmpPtr =  '\0';
	sprintf(qval[DATA_NAME], " =  '%s'",dataName);
	*tmpPtr =  '&';
	set_qval_data_cond(tmpPtr,qval);
      }
    }
    if (strlen(collName) > 0)
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collName);
    selval[DATA_TYP_NAME] = 1;
     status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       5);
    if (status < 0)
      {
	if (status != MCAT_INQUIRE_ERROR) {
	  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	  printf("<html>");
	  printf("<body background=\"%s/srb3.jpg\">", webPath);
	  printf("<H1> %s </H1><HR>", Sentries->entries[4].val);fflush(stdout);
	  printf("Type Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status, status);
	  bRMap(1,0);
	  printf("</body></html>");
	}
	return;
      }
    
    tmpPtr = (char *) getFromResultStruct(
		    (mdasC_sql_result_struct *) &collResult,
		    dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
    strcpy(contentType,tmpPtr);
    free(tmpPtr);
    return(0);

}

int
makeReIngestForm(inStruct Sentries)
{  

  fprintf(stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  enctype=\"multipart/form-data\"  >\n<input type=hidden name=\"function\" value=\"reingest\">\n");
  fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s </FONT><FONT COLOR=#0000FF>%s</FONT></STRONG>", mySrbStrings[68], Sentries->entries[1].val);
  fprintf(stdout,"<INPUT type = hidden NAME=\"dataname\", VALUE= \"%s\">\n",Sentries->entries[1].val);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s: </FONT><FONT COLOR=#0000FF>%s</FONT></STRONG>", mySrbStrings[69],Sentries->entries[2].val);
  fprintf(stdout,"<INPUT type = hidden NAME=\"collection\", VALUE= \"%s\">\n",Sentries->entries[2].val);
  fprintf(stdout,"<INPUT type = hidden NAME=\"copynum\", VALUE= \"%s\">\n",Sentries->entries[4].val);
  fprintf(stdout,"<INPUT type = hidden NAME=\"vernum\", VALUE= \"%s\">\n",Sentries->entries[6].val);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"file\" name=\"filename\"  size=30 />", mySrbStrings[190]);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  fprintf(stdout,"<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[75]);
}

int
ingestReplicaForm(inStruct Sentries)
{  

  fprintf(stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  enctype=\"multipart/form-data\"  >\n<input type=hidden name=\"function\" value=\"ingestreplica\">\n");
  fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s </FONT><FONT COLOR=#0000FF>%s</FONT></STRONG>", mySrbStrings[68],Sentries->entries[1].val);
  fprintf(stdout,"<INPUT type = hidden NAME=\"dataname\", VALUE= \"%s\">\n",Sentries->entries[1].val);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s: </FONT><FONT COLOR=#0000FF>%s</FONT></STRONG>", mySrbStrings[69],Sentries->entries[2].val);
  fprintf(stdout,"<INPUT type = hidden NAME=\"collection\", VALUE= \"%s\">\n",Sentries->entries[2].val);
  fprintf(stdout,"<INPUT type = hidden NAME=\"copynum\", VALUE= \"%s\">\n",Sentries->entries[4].val);
  fprintf(stdout,"<INPUT type = hidden NAME=\"vernum\", VALUE= \"%s\">\n",Sentries->entries[6].val);

  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[72]);
  getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData, Sentries->defaultResource,"#sdsc-fs#sdsc-mda18-fs#");
  fprintf(stdout,"%s\n", Sentries->selectData);fflush(stdout);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT>\n", mySrbStrings[52]);fflush(stdout);
  getValuesAsSelect(Sentries, DATA_TYP_NAME, Sentries->selectData,(char *)NULL,"");
  fprintf(stdout,"%s\n", Sentries->selectData);fflush(stdout);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"file\" name=\"filename\"  size=30 />", mySrbStrings[190]);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  fprintf(stdout,"<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[103]);
}


int
ingestNewVersionForm(inStruct Sentries)
{  

  fprintf(stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  enctype=\"multipart/form-data\"  >\n<input type=hidden name=\"function\" value=\"ingestnewversion\">\n");
  fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s </FONT><FONT COLOR=#0000FF>%s</FONT></STRONG>", mySrbStrings[68], Sentries->entries[1].val);
  fprintf(stdout,"<INPUT type = hidden NAME=\"dataname\", VALUE= \"%s\">\n", Sentries->entries[1].val);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s: </FONT><FONT COLOR=#0000FF>%s</FONT></STRONG>", mySrbStrings[69], Sentries->entries[2].val);
  fprintf(stdout,"<INPUT type = hidden NAME=\"collection\", VALUE= \"%s\">\n", Sentries->entries[2].val);
  fprintf(stdout,"<INPUT type = hidden NAME=\"copynum\", VALUE= \"%s\">\n", Sentries->entries[4].val);
  fprintf(stdout,"<INPUT type = hidden NAME=\"vernum\", VALUE= \"%s\">\n", Sentries->entries[6].val);

  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[72]);
  getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData, Sentries->defaultResource,"#sdsc-fs#sdsc-mda18-fs#");
  fprintf(stdout,"%s\n", Sentries->selectData);
  fflush(stdout);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT>\n", mySrbStrings[52]);
  fflush(stdout);
  getValuesAsSelect(Sentries, DATA_TYP_NAME, Sentries->selectData,(char *)NULL,"");
  fprintf(stdout,"%s\n", Sentries->selectData);
  fflush(stdout);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"file\" name=\"filename\"  size=30 />", mySrbStrings[190]);
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  fprintf(stdout,"<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[105]);
}


int regFileForm(inStruct Sentries)
{
  mdasC_sql_result_struct collResult;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  char contSelect[MAX_TOKEN];
  int i, j;
  int status;
  int collLike;
  char *collection;

  if (strlen(Sentries->entries[2].val) == 0)
    collection = Sentries->entries[1].val;
  else
    collection = Sentries->entries[2].val;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    sprintf(qval[UDSMD_COLL0], " = 'COLLMETAATTR'");
    selval[METADATA_NUM_COLL] = 1;
    selval[UDSMD_COLL1] = 1;
    selval[UDSMD_COLL2] = 1;
    selval[UDSMD_COLL3] = 1;
    selval[UDSMD_COLL4] = 1;

  fprintf(stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  enctype=\"multipart/form-data\"  >\n<input type=hidden name=\"function\" value=\"regfile\">\n");
  fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s </STRONG></FONT>\
<INPUT NAME=\"dataname\", VALUE= \"\" , SIZE=20,12>\
<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT> \
      <INPUT NAME=\"collection\", VALUE= \"%s\" , SIZE=20,12>\
<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[68], mySrbStrings[69],collection,mySrbStrings[72]);
  getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData, Sentries->defaultResource,"#sdsc-fs#sdsc-mda18-fs#");
  fprintf(stdout,"%s\n", Sentries->selectData);fflush(stdout);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT>\n", mySrbStrings[52]);fflush(stdout);
  getValuesAsSelect(Sentries, DATA_TYP_NAME, Sentries->selectData,(char *)NULL,"");
  fprintf(stdout,"%s\n", Sentries->selectData);fflush(stdout);

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
      if (status != MCAT_INQUIRE_ERROR) {
	printf("Metadat Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	return(status);
      }
    }
    else {
      filterDeleted (&collResult);
      first = 1;
      prtCollMetaAttr(&collResult);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtCollMetaAttr(&collResult);
      }
      clearSqlResult (&collResult);
    }
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #1:</STRONG> </FONT> <INPUT NAME=\"omd1n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd1v\", VALUE= \"\" , SIZE=20,12> <INPUT NAME=\"dunit1\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #2:</STRONG> </FONT> <INPUT NAME=\"omd2n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd2v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit2\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #3:</STRONG> </FONT> <INPUT NAME=\"omd3n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd3v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit3\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);

  fprintf(stdout,"<HR><BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"text\" name=\"fname\"  value=\"\" size=60 />", mySrbStrings[147]);
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  fprintf(stdout,"<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[148]);

}
int regURLForm(inStruct Sentries)
{
  mdasC_sql_result_struct collResult;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  char contSelect[MAX_TOKEN];
  int i, j;
  int status;
  int collLike;
  char *collection;

  if (strlen(Sentries->entries[2].val) == 0)
    collection = Sentries->entries[1].val;
  else
    collection = Sentries->entries[2].val;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      Sentries->likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    sprintf(qval[UDSMD_COLL0], " = 'COLLMETAATTR'");
    selval[METADATA_NUM_COLL] = 1;
    selval[UDSMD_COLL1] = 1;
    selval[UDSMD_COLL2] = 1;
    selval[UDSMD_COLL3] = 1;
    selval[UDSMD_COLL4] = 1;
    /*
  fprintf(stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  enctype=\"multipart/form-data\"  >\n<input type=hidden name=\"function\" value=\"regURL\">\n");
    */
    fprintf(stdout,"<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n<input type=hidden name=\"function\" value=\"regURL\">\n");
  fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s </STRONG></FONT>\
<INPUT NAME=\"dataname\", VALUE= \"\" , SIZE=20,12>\
<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT> \
      <INPUT NAME=\"collection\", VALUE= \"%s\" , SIZE=20,12>", mySrbStrings[68], mySrbStrings[69],collection);
  fprintf(stdout,"<INPUT NAME=\"resource\", VALUE= \"http-server\" , type=\"hidden\">\n");
  fprintf(stdout,"<INPUT NAME=\"datatype\", VALUE= \"URL\" , type=\"hidden\">\n");
  status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);
  if (status < 0)     {
    if (status != MCAT_INQUIRE_ERROR) {
      printf("Metadat Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
      return(status);
    }
  }
  else {
    filterDeleted (&collResult);
    Sentries->first = 1;
    prtCollMetaAttr(&collResult);
    while (collResult.continuation_index >= 0) {
      clearSqlResult (&collResult);
      status = srbGetMoreRows(Sentries->conn, 0,
			      collResult.continuation_index,
			      &collResult, MAX_RESULT_NUM);
      if (collResult.result_count == 0  || status != 0)
	break;
      filterDeleted (&collResult);
      prtCollMetaAttr(&collResult);
    }
    clearSqlResult (&collResult);
  }
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #1:</STRONG> </FONT> <INPUT NAME=\"omd1n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd1v\", VALUE= \"\" , SIZE=20,12> <INPUT NAME=\"dunit1\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #2:</STRONG> </FONT> <INPUT NAME=\"omd2n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd2v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit2\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #3:</STRONG> </FONT> <INPUT NAME=\"omd3n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd3v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit3\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  
  fprintf(stdout,"<HR><BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"text\" name=\"fname\"  value=\"\" size=60 />", mySrbStrings[150]);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  fprintf(stdout,"<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[148]);

}


int regExecForm(inStruct Sentries)
{
  mdasC_sql_result_struct collResult;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  char contSelect[MAX_TOKEN];
  int i, j;
  int status;
  int collLike;
  char *collection;

  if (strlen(Sentries->entries[2].val) == 0)
    collection = Sentries->entries[1].val;
  else
    collection = Sentries->entries[2].val;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      Sentries->likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    sprintf(qval[UDSMD_COLL0], " = 'COLLMETAATTR'");
    selval[METADATA_NUM_COLL] = 1;
    selval[UDSMD_COLL1] = 1;
    selval[UDSMD_COLL2] = 1;
    selval[UDSMD_COLL3] = 1;
    selval[UDSMD_COLL4] = 1;
    /*
  fprintf(stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  enctype=\"multipart/form-data\"  >\n<input type=hidden name=\"function\" value=\"regexec\">\n");
    */
    fprintf(stdout,"<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n<input type=hidden name=\"function\" value=\"regexec\">\n");
  fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s </STRONG></FONT>\
<INPUT NAME=\"dataname\", VALUE= \"\" , SIZE=20,12>\
<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT> \
      <INPUT NAME=\"collection\", VALUE= \"%s\" , SIZE=20,12>", mySrbStrings[68], mySrbStrings[69],collection);
  /***
  fprintf(stdout,"<INPUT NAME=\"resource\", VALUE= \"http-server\" , type=\"hidden\">\n");
  ***/
fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[72]);
  getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData,Sentries->defaultResource,"#sdsc-fs#sdsc-mda18-fs#");
  fprintf(stdout,"%s\n",Sentries->selectData);fflush(stdout);

  fprintf(stdout,"<INPUT NAME=\"datatype\", VALUE= \"Executable\" , type=\"hidden\">\n");
  status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);
  if (status < 0)     {
    if (status != MCAT_INQUIRE_ERROR) {
      printf("Metadat Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
      return(status);
    }
  }
  else {
    filterDeleted (&collResult);
    Sentries->first = 1;
    prtCollMetaAttr(&collResult);
    while (collResult.continuation_index >= 0) {
      clearSqlResult (&collResult);
      status = srbGetMoreRows(Sentries->conn, 0,
			      collResult.continuation_index,
			      &collResult, MAX_RESULT_NUM);
      if (collResult.result_count == 0  || status != 0)
	break;
      filterDeleted (&collResult);
      prtCollMetaAttr(&collResult);
    }
    clearSqlResult (&collResult);
  }
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #1:</STRONG> </FONT> <INPUT NAME=\"omd1n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd1v\", VALUE= \"\" , SIZE=20,12> <INPUT NAME=\"dunit1\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #2:</STRONG> </FONT> <INPUT NAME=\"omd2n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd2v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit2\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #3:</STRONG> </FONT> <INPUT NAME=\"omd3n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd3v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit3\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  
  fprintf(stdout,"<HR><BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"text\" name=\"fname\"  value=\"\" size=60 />", mySrbStrings[165]);
   fprintf(stdout,"<BR><FONT COLOR=#0000FF>%s</FONT>\n", mySrbStrings[166]);
  fprintf(stdout,"<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  fprintf(stdout,"<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[148]);

}

int regORBForm(inStruct Sentries)
{
  mdasC_sql_result_struct collResult;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  char contSelect[MAX_TOKEN];
  int i, j;
  int status;
  int collLike;
  char *collection;

  if (strlen(Sentries->entries[2].val) == 0)
    collection = Sentries->entries[1].val;
  else
    collection = Sentries->entries[2].val;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      Sentries->likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    sprintf(qval[UDSMD_COLL0], " = 'COLLMETAATTR'");
    selval[METADATA_NUM_COLL] = 1;
    selval[UDSMD_COLL1] = 1;
    selval[UDSMD_COLL2] = 1;
    selval[UDSMD_COLL3] = 1;
    selval[UDSMD_COLL4] = 1;

    fprintf(stdout,"<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n<input type=hidden name=\"function\" value=\"regORB\">\n");
  fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s </STRONG></FONT>\
<INPUT NAME=\"dataname\", VALUE= \"\" , SIZE=20,12>\
<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT> \
      <INPUT NAME=\"collection\", VALUE= \"%s\" , SIZE=20,12>", mySrbStrings[68], mySrbStrings[69],collection);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n",mySrbStrings[72]);
  getRsrcOfTypeAsSelect(Sentries, Sentries->selectData," like '%orb misc driver%'");
  fprintf(stdout,"%s\n",Sentries->selectData);fflush(stdout);
  fprintf(stdout,"<INPUT NAME=\"datatype\", VALUE= \"orb data\" , type=\"hidden\">\n");
  fprintf(stdout,"<HR><BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"text\" name=\"orbname\"  value=\"idablade.ucsd.edu\" size=30 />\n", mySrbStrings[157]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"text\" name=\"sensorname\"  value=\"\" size=30 />\n", mySrbStrings[158]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT>", mySrbStrings[159]);
 fprintf(stdout,"<SELECT SIZE=1 NAME=orbwhich>\
<OPTION SELECTED VALUE=\"ORBOLDEST\">Oldest Packet\
<OPTION VALUE=\"ORBCURRENT\">Current Packet\
<OPTION VALUE=\"ORBNEWEST\">Newest Packet\
<OPTION VALUE=\"ORBNEXT\">Next Packet after Current\
<OPTION VALUE=\"ORBPREV\">Previous Packet before Current\
<OPTION VALUE=\"ORBNEXT_WAIT\">Next Packet after Current with Wait\
</SELECT>\n");
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"text\" name=\"orbtimeout\"  value=\"10\" size=6 />\n", mySrbStrings[160]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"text\" name=\"orbnumofpkts\"  value=\"20\" size=6 />\n", mySrbStrings[161]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT>", mySrbStrings[162]);
 fprintf(stdout,"<SELECT SIZE=1 NAME=presentation>\
<OPTION SELECTED VALUE=\"BASIC\"> Basic\
<OPTION VALUE=\"GPSHTML\">GPS in html\
<OPTION VALUE=\"GPSXML\">GPS in XML\
<OPTION VALUE=\"WAVJAV1\">Waveform using Java Applet Version 1\
<OPTION VALUE=\"WAVJAV2\">Waveform using Java Applet Version 2\
</SELECT>\n");
   fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"text\" name=\"presentationparams\"  value=\"\" size=20 /><HR>\n", mySrbStrings[163]);

  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"text\" name=\"orbnumofpkts\"  value=\"20\" size=6 /><HR>\n", mySrbStrings[164]);

  status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);
  if (status < 0)     {
    if (status != MCAT_INQUIRE_ERROR) {
      printf("Metadat Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
      return(status);
    }
  }
  else {
    filterDeleted (&collResult);
    Sentries->first = 1;
    prtCollMetaAttr(&collResult);
    while (collResult.continuation_index >= 0) {
      clearSqlResult (&collResult);
      status = srbGetMoreRows(Sentries->conn, 0,
			      collResult.continuation_index,
			      &collResult, MAX_RESULT_NUM);
      if (collResult.result_count == 0  || status != 0)
	break;
      filterDeleted (&collResult);
      prtCollMetaAttr(&collResult);
    }
    clearSqlResult (&collResult);
  }
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #1:</STRONG> </FONT> <INPUT NAME=\"omd1n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd1v\", VALUE= \"\" , SIZE=20,12> <INPUT NAME=\"dunit1\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #2:</STRONG> </FONT> <INPUT NAME=\"omd2n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd2v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit2\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #3:</STRONG> </FONT> <INPUT NAME=\"omd3n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd3v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit3\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  fprintf(stdout,"<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[148]);

}

int regSQLForm(inStruct Sentries)
{
  mdasC_sql_result_struct collResult;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  char contSelect[MAX_TOKEN];
  int i, j;
  int status;
  int collLike;
  char *collection;
  
 
  if (strlen(Sentries->entries[2].val) == 0)
    collection = Sentries->entries[1].val;
  else
    collection = Sentries->entries[2].val;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
   
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      Sentries->likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    sprintf(qval[UDSMD_COLL0], " = 'COLLMETAATTR'");
    selval[METADATA_NUM_COLL] = 1;
    selval[UDSMD_COLL1] = 1;
    selval[UDSMD_COLL2] = 1;
    selval[UDSMD_COLL3] = 1;
    selval[UDSMD_COLL4] = 1;
    /*
  fprintf(stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  enctype=\"multipart/form-data\"  >\n<input type=hidden name=\"function\" value=\"regSQL\">\n");
    */
    
    
    fprintf(stdout,"<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n<input type=hidden name=\"function\" value=\"regSQL\">\n");
  fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s </STRONG></FONT>\
<INPUT NAME=\"dataname\", VALUE= \"\" , SIZE=20,12>\
<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT> \
      <INPUT NAME=\"collection\", VALUE= \"%s\" , SIZE=20,12>", mySrbStrings[68], mySrbStrings[69],collection);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n",mySrbStrings[72]);
  getRsrcOfTypeAsSelect(Sentries, Sentries->selectData," like '%table database%'");
  fprintf(stdout,"%s\n",Sentries->selectData);fflush(stdout);
  fprintf(stdout,"<INPUT NAME=\"datatype\", VALUE= \"database shadow object\" , type=\"hidden\">\n");
  status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);
  if (status < 0)     {
    if (status != MCAT_INQUIRE_ERROR) {
      printf("Metadat Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
      return(status);
    }
  }
  else {
    filterDeleted (&collResult);
    Sentries->first = 1;
    prtCollMetaAttr(&collResult);
    while (collResult.continuation_index >= 0) {
      clearSqlResult (&collResult);
      status = srbGetMoreRows(Sentries->conn, 0,
			      collResult.continuation_index,
			      &collResult, MAX_RESULT_NUM);
      if (collResult.result_count == 0  || status != 0)
	break;
      filterDeleted (&collResult);
      prtCollMetaAttr(&collResult);
    }
    clearSqlResult (&collResult);
  }

  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #1:</STRONG> </FONT> <INPUT NAME=\"omd1n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd1v\", VALUE= \"\" , SIZE=20,12> <INPUT NAME=\"dunit1\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #2:</STRONG> </FONT> <INPUT NAME=\"omd2n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd2v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit2\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #3:</STRONG> </FONT> <INPUT NAME=\"omd3n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd3v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit3\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  
  fprintf(stdout,"<HR><BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"text\" name=\"fname\"  value=\"\" size=60>", mySrbStrings[153]);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  fprintf(stdout,"<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[148]);

}


int regDirForm(inStruct Sentries)
{
  mdasC_sql_result_struct collResult;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  char contSelect[MAX_TOKEN];
  int i, j;
  int status;
  int collLike;
  char *collection;

  if (strlen(Sentries->entries[2].val) == 0)
    collection = Sentries->entries[1].val;
  else
    collection = Sentries->entries[2].val;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    sprintf(qval[UDSMD_COLL0], " = 'COLLMETAATTR'");
    selval[METADATA_NUM_COLL] = 1;
    selval[UDSMD_COLL1] = 1;
    selval[UDSMD_COLL2] = 1;
    selval[UDSMD_COLL3] = 1;
    selval[UDSMD_COLL4] = 1;

    fprintf(stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  enctype=\"multipart/form-data\"  >\n<input type=hidden name=\"function\" value=\"regdir\">\n");
    fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s </STRONG></FONT>", mySrbStrings[68]);
    fprintf(stdout,"<INPUT NAME=\"dataname\", VALUE= \"\" , SIZE=20,12>\
<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT> \
      <INPUT NAME=\"collection\", VALUE= \"%s\" , SIZE=20,12>\
<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[69],collection,mySrbStrings[72]);
  getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData, Sentries->defaultResource,"#sdsc-fs#sdsc-mda18-fs#");
  fprintf(stdout,"%s\n",Sentries->selectData);fflush(stdout);
    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
      if (status != MCAT_INQUIRE_ERROR) {
	printf("Metadat Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	return(status);
      }
    }
    else {
      filterDeleted (&collResult);
      Sentries->first = 1;
      prtCollMetaAttr(&collResult);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtCollMetaAttr(&collResult);
      }
      clearSqlResult (&collResult);
    }
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #1:</STRONG> </FONT> <INPUT NAME=\"omd1n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd1v\", VALUE= \"\" , SIZE=20,12> <INPUT NAME=\"dunit1\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #2:</STRONG> </FONT> <INPUT NAME=\"omd2n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd2v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit2\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #3:</STRONG> </FONT> <INPUT NAME=\"omd3n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd3v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit3\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);

  fprintf(stdout,"<HR><BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"text\" name=\"fname\"  value=\"\" size=60 />", mySrbStrings[151]);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  fprintf(stdout,"<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[148]);

}

int
createTextFileForm(inStruct Sentries)
{  
  mdasC_sql_result_struct collResult;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  char contSelect[MAX_TOKEN];
  int i, j;
  int status;
  int collLike;
  char *collection;

  if (strlen(Sentries->entries[2].val) == 0)
    collection = Sentries->entries[1].val;
  else
    collection = Sentries->entries[2].val;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    sprintf(qval[UDSMD_COLL0], " = 'COLLMETAATTR'");
    selval[METADATA_NUM_COLL] = 1;
    selval[UDSMD_COLL1] = 1;
    selval[UDSMD_COLL2] = 1;
    selval[UDSMD_COLL3] = 1;
    selval[UDSMD_COLL4] = 1;

  fprintf(stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\" >\n<input type=hidden name=\"function\" value=\"put\">\n");
  fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s </STRONG></FONT>\
<INPUT NAME=\"dataname\", VALUE= \"\" , SIZE=20,12>\
<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT> \
      <INPUT NAME=\"collection\", VALUE= \"%s\" , SIZE=20,12>\
<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[68], mySrbStrings[69],collection,mySrbStrings[72]);
  getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData, Sentries->defaultResource,"#sdsc-fs#sdsc-mda18-fs#");
  fprintf(stdout,"%s\n",Sentries->selectData);fflush(stdout);
  fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[71]);
  
  sprintf(contSelect, " like '%%/%s.%s/%%'", Sentries->srbUser, Sentries->userDomain);
  getValuesAsSelectWithCond(Sentries, CONTAINER_NAME, "","",contSelect);
  printf("%s\n", Sentries->selectData);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#0000FF> %s</STRONG></FONT>\n", mySrbStrings[70]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT>\n", mySrbStrings[52]);fflush(stdout);
  getValuesAsSelect(Sentries, DATA_TYP_NAME, Sentries->selectData,"text","");
  fprintf(stdout,"%s\n",Sentries->selectData);fflush(stdout);

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
      if (status != MCAT_INQUIRE_ERROR) {
	printf("Metadat Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	return(status);
      }
    }
    else {
      filterDeleted (&collResult);
      first = 1;
      prtCollMetaAttr(&collResult);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtCollMetaAttr(&collResult);
      }
      clearSqlResult (&collResult);
    }
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #1:</STRONG> </FONT> <INPUT NAME=\"omd1n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd1v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit1\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #2:</STRONG> </FONT> <INPUT NAME=\"omd2n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd2v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit2\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #3:</STRONG> </FONT> <INPUT NAME=\"omd3n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd3v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit3\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);

  fprintf(stdout,"<HR><CENTER><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT></BR>", mySrbStrings[145]);
  printf("<textarea rows=10 cols=90 name=\"dataval\"></textarea>\n");
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  fprintf(stdout,"<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></center></FORM>\n", mySrbStrings[146]);
}

int
deleteMeta(inStruct Sentries)
{
  int ii;
  char tmpStr[MAX_TOKEN * 2];
  if (strlen(Sentries->entries[1].val) == 0) {
    printf("Removing Metadata for Collection %s ...", Sentries->entries[2].val);
    ii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, Sentries->entries[2].val,
	      "-1", "", "", C_DELETE_USER_DEFINED_COLL_STRING_META_DATA);
  }
  else {
    printf("Removing Metadata for File %s/%s ...", 
	   Sentries->entries[2].val,Sentries->entries[1].val);
    ii = srbModifyDataset(Sentries->conn,  MDAS_CATALOG, Sentries->entries[1].val, Sentries->entries[2].val,
               "","", "-1", "", D_DELETE_USER_DEFINED_STRING_META_DATA);
    
  }
  if (ii < 0) {
    printf("<BR>Error removing metadata : <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",ii,ii);
    srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    bRMap(2,0);
    return;
  }

  printf("Done");
  bRMap(2,1);
  return;
}


int
removeDataColl(inStruct Sentries)
{
  int ii;
  char tmpStr[MAX_TOKEN * 2];
  if (strlen(Sentries->entries[1].val) == 0) {
    printf("Removing Collection %s ...", Sentries->entries[2].val);
    ii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, Sentries->entries[2].val,
			   "", "", "", D_DELETE_COLL);
    if (ii < 0) {
      printf("<BR>Error removing collection %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", Sentries->entries[2].val,ii,ii);
      srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      bRMap(2,0);
      return;
    }
  }
  else {
    if (strlen(Sentries->entries[4].val) > 0 && strlen(Sentries->entries[6].val) > 0) {
      sprintf(tmpStr, "%s&COPY=%s&DVERSION='%s'",Sentries->entries[1].val, Sentries->entries[4].val, Sentries->entries[6].val);
      printf("Removing DataObject %s/%s ...",Sentries->entries[2].val,tmpStr);

       ii = srbObjUnlink(Sentries->conn, tmpStr, Sentries->entries[2].val);
    }
    else if (strlen(Sentries->entries[4].val) > 0) {
      sprintf(tmpStr, "%s&COPY=%s",Sentries->entries[1].val, Sentries->entries[4].val);
      printf("Removing DataObject %s/%s ...",Sentries->entries[2].val,tmpStr);
       ii = srbObjUnlink(Sentries->conn, tmpStr, Sentries->entries[2].val);
    }
    else {
      printf("Removing DataObject %s/%s ...",Sentries->entries[2].val,Sentries->entries[1].val);
      ii = srbObjUnlink(Sentries->conn, Sentries->entries[1].val, Sentries->entries[2].val);
    }
    if (ii < 0) {
      printf("<BR>Error removing dataObject %s/%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", 
	     Sentries->entries[2].val,Sentries->entries[1].val,ii,ii);
      srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      bRMap(2,0);
      return;
    }
  }
  printf("Done");
  bRMap(2,1);
  return;
}

int giveTicketForm(inStruct Sentries)
{

  printf("<CENTER> <B><FONT COLOR=#0000FF>SRB Tickets</FONT></B>\n");
  printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\" target=\"%s\"  >\n", Sentries->frameStr);
  printf("<input type=hidden name=\"function\" value=\"giveticket\">\n<BR>\n");
  printf("<TABLE><TR><TD>");
  if (strlen(Sentries->entries[1].val) > 0) {
    printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT>\n", mySrbStrings[68]);
  }
    printf("<INPUT type=hidden NAME=\"dataname\", VALUE= \"%s\">%s\n",Sentries->entries[1].val,Sentries->entries[1].val);
  printf("</TD></TR><TR><TD>\n");
  printf("<STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT> \n", mySrbStrings[69]);
  printf("<INPUT type=hidden NAME=\"collection\", VALUE= \"%s\">%s\n",Sentries->entries[2].val,Sentries->entries[2].val);
  printf("</TD></TR>\n");
  printf("<TR><TD><STRONG><FONT COLOR=#FF0000>User Name:</STRONG></FONT>\n");
  getUserAsSelect(Sentries, "ticketuser@sdsc");
  printf("%s\n",Sentries->selectData);
  printf("</TD></TR>\n");
  /*  printf("<TR><TD><STRONG><FONT COLOR=#FF0000>Access Constraint:</STRONG></FONT>\n");
  getValuesAsSelect(ACCESS_CONSTRAINT,selectData,"read","");
  printf("%s\n",selectData);
  printf("</TD></TR>\n");
  **/
  printf("<TR><TD><STRONG><FONT COLOR=#FF0000>Begin Time:</STRONG></FONT>\n");
  printf("<INPUT TYPE=TEXT NAME=begtime VALUE=\"\"> (Empty or San Diego Time in format YYYY-MM-DD-HH.MM.SS)\n");
  printf("</TD></TR><TR><TD><STRONG><FONT COLOR=#FF0000>End Time:</STRONG></FONT>\n") ;
  printf("<INPUT TYPE=TEXT NAME=endtime VALUE=\"\">\n");
  printf("</TD></TR><TR><TD><STRONG><FONT COLOR=#FF0000>Number of Access:</STRONG></FONT>\n");
  printf("<INPUT TYPE=TEXT NAME=accscnt VALUE=\"100\">\n");
  printf("</TABLE>\n");
  if (strlen(Sentries->entries[1].val) > 0)
    printf("<input type=hidden name=\"recursive\" value=\"\">");
  else {
    printf("Recursive: Yes: <input type=radio name=\"recursive\" value=\"R\" checked>");
    printf(" No: <input type=radio name=\"recursive\" value=\"\">");
  }
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"Issue Ticket\">\n</FORM>\n");
  return;


}

int addToTicketForm(inStruct Sentries)
{
  construction();
}

int aclForm(inStruct Sentries)
{

  printf("<CENTER> <B><FONT COLOR=#0000FF>SRB Authorization</FONT></B>\n");
  printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"   >\n");
  printf("<input type=hidden name=\"function\" value=\"authorize\">\n<BR>\n");
  printf("<TABLE><TR><TD>");
  if (strlen(Sentries->entries[1].val) > 0) {
    printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT>\n", mySrbStrings[68]);
  }
    printf("<INPUT type=hidden NAME=\"dataname\", VALUE= \"%s\">%s\n",Sentries->entries[1].val,Sentries->entries[1].val);
  printf("</TD></TR><TR><TD>\n");
  printf("<STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT> \n", mySrbStrings[69]);
  printf("<INPUT type=hidden NAME=\"collection\", VALUE= \"%s\">%s\n",Sentries->entries[2].val,Sentries->entries[2].val);
  printf("</TD></TR><TR><TD>\n");
  prtCurACL(Sentries);
  printf("</TD></TR>\n"); fflush(stdout);
  printf("<TR><TD><STRONG><FONT COLOR=#FF0000>User Name:</STRONG></FONT>\n");
  getUserAsSelect(Sentries, "public@npaci");
  printf("%s\n", Sentries->selectData);
  printf("</TD></TR>\n"); fflush (stdout);
  printf("<TR><TD><STRONG><FONT COLOR=#FF0000>Access Constraint:</STRONG></FONT>\n");
  getValuesAsSelect(Sentries, ACCESS_CONSTRAINT, Sentries->selectData,"read","");
  printf("%s\n", Sentries->selectData);
  if (strlen(Sentries->entries[1].val) == 0) {
    printf("<TR><TD><STRONG><FONT COLOR=#FF0000>Access Control Type:</STRONG></FONT>\n");
    printf("<SELECT SIZE=1 NAME=acltype>\
<OPTION SELECTED VALUE=\"0\"> this Collection Only\
<OPTION VALUE=\"1\">All Files and SubCollections in this Collection\
<OPTION VALUE=\"2\">this Collection  and All Files and SubCollections in this Collection\
<OPTION VALUE=\"3\">Recursively All Files and SubCollections in this Collection\
<OPTION VALUE=\"4\">this Collection  and Recursively All Files and SubCollections in this Collection\
</SELECT>\n");
  }
  printf("</TD></TR></TABLE>\n");
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"Authorize\">\n</FORM>\n");
  return;

}

int
createSnapshotForm(inStruct Sentries)
{

  printf("<CENTER> <B><FONT COLOR=#0000FF>SRB SnapShot</FONT></B>\n");
  printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"   >\n");
  printf("<input type=hidden name=\"function\" value=\"createsnapshot\">\n<BR>\n");
  printf("<TABLE><TR><TD>");
  printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT>\n", mySrbStrings[68]);
  if (Sentries->m > 1)
    printf("<INPUT type=hidden NAME=\"dataname\", VALUE= \"%s\">%s\n",Sentries->entries[1].val,Sentries->entries[1].val);
  else
    printf("<INPUT NAME=\"dataname\", VALUE= \"\" , SIZE=20,12>\n");
  printf("</TD><TR><TD>\n");
  printf("<STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT> \n", mySrbStrings[69]);
  if (Sentries->m > 1)
    printf("<INPUT type=hidden NAME=\"collection\", VALUE= \"%s\">%s\n",Sentries->entries[2].val,Sentries->entries[2].val);
  else
    printf("<INPUT NAME=\"collection\", VALUE=\"/home/sekar.sdsc/metatest\",SIZE=20,12>\n");
  printf("</TD><TR><TD>\n");
  printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n",mySrbStrings[72]);
  getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData, Sentries->defaultResource,"#sdsc-fs#sdsc-mda18-fs#");
  printf("%s\n", Sentries->selectData);
  printf("</TD><TR><TD>\n");
  printf("<STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT>\n", mySrbStrings[52]);
  getValuesAsSelect(Sentries, DATA_TYP_NAME, Sentries->selectData, mySrbStrings[232],"");
  printf("%s\n", Sentries->selectData);
  printf("</TD></TR></TABLE>\n");
  printf("<INPUT type=hidden  NAME=\"copynum\", VALUE= \"%s\" >\n",Sentries->entries[4].val);    
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\">\n</FORM>\n", mySrbStrings[106]);
  return;

}

int
replicationForm(inStruct Sentries)
{

  printf("<CENTER> <B><FONT COLOR=#0000FF>%s</FONT></B>\n", mySrbStrings[234]);
  printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"   >\n");
  printf("<input type=hidden name=\"function\" value=\"replicate\">\n<BR>\n");
  printf("<TABLE><TR><TD>");
  printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT>\n", mySrbStrings[68]);
  if (Sentries->m > 1)
    printf("<INPUT type=hidden NAME=\"dataname\", VALUE= \"%s\">%s\n",Sentries->entries[1].val,Sentries->entries[1].val);
  else
    printf("<INPUT NAME=\"dataname\", VALUE= \"\" , SIZE=20,12>\n");
  printf("</TD><TR><TD>\n");
  printf("<STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT> \n", mySrbStrings[69]);
  if (Sentries->m > 1)
    printf("<INPUT type=hidden NAME=\"collection\", VALUE= \"%s\">%s\n",Sentries->entries[2].val,Sentries->entries[2].val);
  else
    printf("<INPUT NAME=\"collection\", VALUE=\"/home/sekar.sdsc/metatest\",SIZE=20,12>\n");
  printf("</TD><TR><TD>\n");
  printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n",mySrbStrings[72]);
  getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData, Sentries->defaultResource,"#sdsc-fs#sdsc-mda18-fs#");
  printf("%s\n", Sentries->selectData);
  printf("</TD><TR><TD>\n");
  printf("<STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT>\n", mySrbStrings[52]);
  getValuesAsSelect(Sentries, DATA_TYP_NAME, Sentries->selectData, mySrbStrings[232],"");
  printf("%s\n", Sentries->selectData);
  printf("</TD></TR></TABLE>\n");
  printf("<INPUT type=hidden  NAME=\"copynum\", VALUE= \"%s\" >\n",Sentries->entries[4].val);    
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\">\n</FORM>\n", mySrbStrings[233]);
  return;

}


int
fileIngestionForm(inStruct Sentries)
{

  printf("<CENTER> <B><FONT COLOR=#0000FF>SRB DataSet Access</FONT></B>\n");
  printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"   >\n");
  printf("<input type=hidden name=\"function\" value=\"getcollmetaattr\">\n<BR>\n");
  printf("<STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT>\n", mySrbStrings[69]);
  getValuesAsSelect(Sentries, COLLECTION_NAME, Sentries->selectData,"/home/sekar.sdsc/metatest/test","");
  printf("%s\n",selectData);
  printf("<BR><B>%s</B><BR>\n<input name=\"altcoll\" value=\"\" , size=40,5>\n", mySrbStrings[218]);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR>\n<INPUT TYPE=\"submit\" VALUE=\"Send\">\n</FORM>\n");
  return;
}

int makeContainer(inStruct Sentries) 
{
  char contName[MAX_TOKEN * 2];
  int i,ii;

  if (strlen(Sentries->entries[2].val) == 0 ) {
     printf ("Error: Container Name is Empty. Please use back button and resubmit.\n");
     bRMap(1,0);
     return;
   }
  if (strlen(Sentries->entries[1].val) == 0 ) {
     printf ("Error: Container Collection Name is Empty. Please use back button and resubmit.\n");
     bRMap(1,0);
     return;
  }
  printf("<BR>Starting Container Creation ... "); fflush(stdout);
  sprintf(contName,"%s/%s",Sentries->entries[1].val,Sentries->entries[2].val);
  i = srbContainerCreate (Sentries->conn, MDAS_CATALOG, contName,Sentries->entries[4].val, 
     Sentries->entries[5].val, strtoll (Sentries->entries[3].val, 0, 0));
  if (i < 0) {
    printf("Container Creation Error for  container '%s' in resource '%s' with size '%lld' nd dataType '%s'  , status =  %i\n", 
	   contName,Sentries->entries[5].val,strtoll (Sentries->entries[3].val, 0, 0),
	   Sentries->entries[4].val, i);
    srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
  }
  printf("Done\n<BR>"); 
  bRMap(2,1);
  return;
}


int
giveTicket(inStruct Sentries)
{
  char *trgColl, *trgObj, *userDom, *accsName, *recFlag;
  char *begTime, *endTime;
  int i,ii, accCnt;
  char tickVal[MAX_TOKEN],  *ticketVal;

   printf("<BR>Starting Ticket Issue ... "); fflush(stdout);

   trgObj = Sentries->entries[1].val;
   trgColl = Sentries->entries[2].val;
   userDom = Sentries->entries[3].val;
   begTime = Sentries->entries[4].val;
   endTime = Sentries->entries[5].val;
   accCnt = atoi(Sentries->entries[6].val);
   recFlag = Sentries->entries[7].val;
   ticketVal = tickVal;
 
   ii = srbIssueTicket(Sentries->conn, trgObj, trgColl,recFlag,begTime, endTime,
                           accCnt, userDom, &ticketVal);
   if (ii < 0) {
     printf ("Ticket Ingestion Error for %s/%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,trgObj,ii,ii);
     srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); fflush(stdout);
   printf("<B>Ticket: </B> <FONT COLOR=#0000FF>%s</FONT>\n",ticketVal);
   bRMap(2,0);
   printf("</body></html>");
   return;

}


int snapshotFileIntoSRB (inStruct Sentries)
{
   char *trgColl, *trgObj, *rsrcName, *dataType, *oldReplNum, *srcVerNum;
   char dataObj[MAX_TOKEN * 2];
   int  i,ii,iii,out_fd, status, dataSize;
   char newReplNumTxt[MAX_TOKEN];
     
   trgObj = Sentries->entries[1].val;
   trgColl = Sentries->entries[2].val;
   rsrcName = Sentries->entries[3].val;
   dataType = Sentries->entries[4].val;
   oldReplNum = Sentries->entries[5].val;
   srcVerNum = Sentries->entries[6].val;
   printf("<BR>Snapshot Operation Beginning ... "); fflush(stdout);

   sprintf(dataObj,"%s&COPY=%i&DVERSION='%i'",trgObj,atoi(oldReplNum),atoi(srcVerNum));

   i = srbObjReplicate(Sentries->conn, MDAS_CATALOG, dataObj,trgColl,rsrcName,"");
   if (i < 0) {
     printf ("Error in replicating object %s/%s in %s: <A href=\"srberror3.cgi?%i\">Error: %i</A> \n",
              trgColl, dataObj, rsrcName ,i,i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }


   if (strcmp(dataType, mySrbStrings[232])) {
     sprintf(dataObj,"%s&COPY=%i",trgObj,i);
     iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, dataObj,trgColl, "","",
			   dataType,"", D_CHANGE_TYPE);
     if (iii < 0) {
       printf ("Datatype Change Error for %s/%s to %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       trgColl,dataObj,dataType,iii,iii);
       srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
   }


   sprintf(dataObj,"%s&COPY=%i",trgObj,i);
   sprintf(newReplNumTxt,"%i:%s",i,oldReplNum);
   iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, dataObj,trgColl, "","",
			  newReplNumTxt,"-1:-1", D_CHANGE_REPLNUM_VERNUM);
   if (iii < 0) {
     printf ("Version Change Error for %s/%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,dataObj,iii,iii);
     srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); 
   bRMap(2,1);
   printf("</body></html>");fflush(stdout);
   return;
}




int replicateFileIntoSRB (inStruct Sentries)
{
   char *trgColl, *trgObj, *rsrcName, *dataType, *oldReplNum;
   char dataObj[MAX_TOKEN * 2];
   int  i,ii,iii,out_fd, status, dataSize;

   trgObj = Sentries->entries[1].val;
   trgColl = Sentries->entries[2].val;
   rsrcName = Sentries->entries[3].val;
   dataType = Sentries->entries[4].val;
   oldReplNum = Sentries->entries[5].val;

   printf("<BR>Replica Operation Beginning ... "); fflush(stdout);
   sprintf(dataObj,"%s&COPY=%i",trgObj,atoi(oldReplNum));
   i = srbObjReplicate(Sentries->conn, MDAS_CATALOG, dataObj,trgColl,rsrcName,"");
   if (i < 0) {
     printf ("Error in replicating object %s/%s in %s: <A href=\"srberror3.cgi?%i\">Error: %i</A> \n",
              trgColl, dataObj, rsrcName ,i,i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   if (strcmp(dataType, mySrbStrings[232])) {
     sprintf(dataObj,"%s&COPY=%i",trgObj,i);
     iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, dataObj,trgColl, "","",
			   dataType,"", D_CHANGE_TYPE);
     if (iii < 0) {
       printf ("Datatype Change Error for %s/%s to %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       trgColl,dataObj,dataType,iii,iii);
       srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
   }
   printf("Done\n<BR>"); 
   bRMap(2,1);
   printf("</body></html>");fflush(stdout);
   return;
}

int
appendData(inStruct Sentries)
{
   char *trgColl;
   char trgObj[MAX_TOKEN * 2];
   int  i,out_fd, status,dataSize;

   sprintf(trgObj,"%s&COPY=%s&DVERSION='%s'",Sentries->entries[1].val,Sentries->entries[4].val,Sentries->entries[6].val);
   trgColl = Sentries->entries[2].val;
   dataSize = strlen(Sentries->entries[Sentries->m].val);
   printf("<BR>ReIngest Operation Beginning ... "); fflush(stdout);
   out_fd = srbObjOpen (Sentries->conn, trgObj, O_WRONLY, trgColl);
   if (out_fd < 0) {
     printf ("Unable to open object %s/%s,  <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, out_fd, out_fd);

     srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   i = srbObjSeek(Sentries->conn, out_fd, 0, SEEK_END);
   if (i < 0) {
     printf ("Unable to seek in object %s/%s,  <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, out_fd,out_fd);
     srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     srbObjClose(Sentries->conn,out_fd);
     return;
   }

   substituteString(Sentries->entries[Sentries->m].val, "</areatext>","</textarea>");
   i = srbObjWrite(Sentries->conn, out_fd, Sentries->entries[Sentries->m].val,dataSize);
   if (i != dataSize) {
     printf ("Write Error when create object %s/%s, wrote only %d of %d\n",
              trgColl, trgObj, i, dataSize);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   i = srbObjClose(Sentries->conn,out_fd);
   if (i < 0) {
     printf ("Error when closing object %s/%s. <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, i,i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); 
   bRMap(2,1);
   return;
}



int
regDir(inStruct Sentries)
{
   char *trgColl, *rsrcName, *dataType;
   int  i,ii,iii,out_fd, status;
   char trgObj[MAX_TOKEN * 3];
   char newVal[MAX_TOKEN];
   char iName[MAX_TOKEN];
   char iVal[MAX_TOKEN];
   char iUnit[MAX_TOKEN];
   srb_long_t  dataSize;

   strcpy(trgObj, Sentries->entries[1].val);
   trgColl = Sentries->entries[2].val;
   rsrcName = Sentries->entries[3].val;
   dataSize = 0;
 
   if (strlen(trgObj) == 0) {
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf ("Error: Target Data Name Empty. Please use back button and resubmit.\n");
     bRMap(1,0);
     return;
   }
   printf("<title>Registering Directory %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
   printf("<body background=\"%s/srb3.jpg\">", webPath);
   printf("<BR>Register Operation Beginning for %s  as %s/%s  ... ",
	 Sentries->entries[Sentries->m].val, trgColl, trgObj ); 
   fflush(stdout);


   i = srbRegisterDataset(Sentries->conn, MDAS_CATALOG, trgObj,
			  "directory shadow object",rsrcName,trgColl,
                                     Sentries->entries[Sentries->m].val, dataSize);

   /****
            ii = srbRegisterReplica (Sentries->conn, MDAS_CATALOG, targetdataname,
                                     collname, "","",resourcename, 
                                     localfile[i],srbUser,mdasDomainName);
   ****/
   if (i < 0) {
     printf ("<BR>Error when registering Object: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", i,i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     return;
   }
   printf("Done\n<BR>"); fflush(stdout);
   printf("<BR>%s ... ",aaa[25]); fflush(stdout);
   strcpy(trgObj, Sentries->entries[1].val);

     for (i = 4; i < Sentries->m ; i =  i+3) {
       if (strlen(Sentries->entries[i+1].val) == 0)
	 continue;
       strcpy(iName, Sentries->entries[i].val);
       strcpy(iVal, Sentries->entries[i+1].val);     
     strcpy(iUnit,Sentries->entries[i+2].val);
     iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   "0",iName,
			   D_INSERT_USER_DEFINED_STRING_META_DATA);
     if (iii < 0) {
       printf ("Metadata Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName,iii,iii);
       srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     sprintf(newVal,"1@%i",iii);

     ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   newVal,iVal,
			   D_CHANGE_USER_DEFINED_STRING_META_DATA);
     if (ii < 0) {
       printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName, iVal,ii,ii);
       srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     if (strlen(iUnit) > 0) {
       sprintf(newVal,"2@%i",iii);
       ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			     newVal,iUnit,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iUnit,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }


   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);

   return;
}

int
regFile(inStruct Sentries)
{
   char *trgColl,  *rsrcName, *dataType;
   int  i,ii,iii,out_fd, status;
   char trgObj[MAX_TOKEN * 3];
   char newVal[MAX_TOKEN];
   char iName[MAX_TOKEN];
   char iVal[MAX_TOKEN];
   char iUnit[MAX_TOKEN];
   srb_long_t  dataSize;

   strcpy(trgObj, Sentries->entries[1].val);
   trgColl = Sentries->entries[2].val;
   rsrcName = Sentries->entries[3].val;

   dataType = Sentries->entries[4].val;
   dataSize = -1;
 
   if (strlen(trgObj) == 0) {
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf ("Error: Target Data Name Empty. Please use back button and resubmit.\n");
     bRMap(1,0);
     printf("</body></html>");
     return;
   }
   printf("<title>Registering data file %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
   printf("<body background=\"%s/srb3.jpg\">", webPath);
   printf("<BR>Register Operation Beginning for %s  as %s/%s  ... ",
	 Sentries->entries[Sentries->m].val, trgColl, trgObj ); 
   fflush(stdout);


   i = srbRegisterDataset(Sentries->conn, MDAS_CATALOG, trgObj,dataType,rsrcName,trgColl,
                                     Sentries->entries[Sentries->m].val, dataSize);

   /****
            ii = srbRegisterReplica (Sentries->conn, MDAS_CATALOG, targetdataname,
                                     collname, "","",resourcename, 
                                     localfile[i],srbUser,mdasDomainName);
   ****/
   if (i < 0) {
     printf ("<BR>Error when registering Object: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", i,i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); fflush(stdout);
   printf("<BR>%s ... ",aaa[25]); fflush(stdout);
   strcpy(trgObj, Sentries->entries[1].val);

     for (i = 5; i < Sentries->m ; i =  i+3) {
       if (strlen(Sentries->entries[i+1].val) == 0)
	 continue;
       strcpy(iName, Sentries->entries[i].val);
       strcpy(iVal, Sentries->entries[i+1].val);     
     strcpy(iUnit,Sentries->entries[i+2].val);
     iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   "0",iName,
			   D_INSERT_USER_DEFINED_STRING_META_DATA);
     if (iii < 0) {
       printf ("Metadata Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName,iii,iii);
       srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     sprintf(newVal,"1@%i",iii);

     ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   newVal,iVal,
			   D_CHANGE_USER_DEFINED_STRING_META_DATA);
     if (ii < 0) {
       printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName, iVal,ii,ii);
       srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     if (strlen(iUnit) > 0) {
       sprintf(newVal,"2@%i",iii);
       ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			     newVal,iUnit,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iUnit,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }


   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);

   return;
}

int
regURL(inStruct Sentries)
{
   char *trgColl, *rsrcName, *dataType;
   int  i,ii,iii,out_fd, status;
   char trgObj[MAX_TOKEN * 3];
   char newVal[MAX_TOKEN];
   char iName[MAX_TOKEN];
   char iVal[MAX_TOKEN];
   char iUnit[MAX_TOKEN];
   srb_long_t dataSize = 0;

   strcpy(trgObj, Sentries->entries[1].val);
   trgColl = Sentries->entries[2].val;
   rsrcName = Sentries->entries[3].val;
   dataType = Sentries->entries[4].val;

    if (strlen(trgObj) == 0) {
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf ("Error: Target Data Name Empty. Please use back button and resubmit.\n");
     bRMap(1,0);
     printf("</body></html>");
     return;
   }
   printf("<title>Registering data file %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
   printf("<body background=\"%s/srb3.jpg\">", webPath);
   printf("<BR>Register Operation Beginning for %s  as %s/%s  ... ",
	 Sentries->entries[Sentries->m].val, trgColl, trgObj ); 
   fflush(stdout);

   i = srbRegisterDataset(Sentries->conn, MDAS_CATALOG, trgObj,dataType,rsrcName,trgColl,
                                     Sentries->entries[Sentries->m].val, dataSize);
                                     
   if (i < 0) {
     printf ("<BR>Error when registering Object: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", i,i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); fflush(stdout);
   printf("<BR>%s ... ",aaa[25]); fflush(stdout);
   strcpy(trgObj, Sentries->entries[1].val);

     for (i = 5; i < Sentries->m ; i =  i+3) {
       if (strlen(Sentries->entries[i+1].val) == 0)
	 continue;
       strcpy(iName, Sentries->entries[i].val);
       strcpy(iVal, Sentries->entries[i+1].val);     
     strcpy(iUnit,Sentries->entries[i+2].val);
     iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   "0",iName,
			   D_INSERT_USER_DEFINED_STRING_META_DATA);
     if (iii < 0) {
       printf ("Metadata Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName,iii,iii);
       srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     sprintf(newVal,"1@%i",iii);

     ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   newVal,iVal,
			   D_CHANGE_USER_DEFINED_STRING_META_DATA);
     if (ii < 0) {
       printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName, iVal,ii,ii);
       srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     if (strlen(iUnit) > 0) {
       sprintf(newVal,"2@%i",iii);
       ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			     newVal,iUnit,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iUnit,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }


   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);

   return;
}


int
regexec(inStruct Sentries)
{
   char *trgColl,  *rsrcName, *dataType;
   int  i,ii,iii,out_fd, status;
   char trgObj[MAX_TOKEN * 3];
   char newVal[MAX_TOKEN];
   char iName[MAX_TOKEN];
   char iVal[MAX_TOKEN];
   char iUnit[MAX_TOKEN];
   srb_long_t  dataSize;
   
   
   strcpy(trgObj, Sentries->entries[1].val);
   trgColl = Sentries->entries[2].val;
   rsrcName = Sentries->entries[3].val;

   dataType = Sentries->entries[4].val;
   dataSize = 0;
 
   if (strlen(trgObj) == 0) {
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf ("Error: Target Data Name Empty. Please use back button and resubmit.\n");
     bRMap(1,0);
     printf("</body></html>");
     return;
   }
   if (strlen(Sentries->entries[Sentries->m].val) == 0) {
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf ("Error: Command Name Empty. Please use back button and resubmit.\n");
     bRMap(1,0);
     printf("</body></html>");
     return;
   }
   printf("<title>Registering data file %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
   printf("<body background=\"%s/srb3.jpg\">", webPath);
   printf("<BR>Register Operation Beginning for %s  as %s/%s  ... ",
	 Sentries->entries[Sentries->m].val, trgColl, trgObj ); 
   fflush(stdout);


   i = srbRegisterDataset(Sentries->conn, MDAS_CATALOG, trgObj,dataType,rsrcName,trgColl,
                                     Sentries->entries[Sentries->m].val, dataSize);
   if (i < 0) {
     printf ("<BR>Error when registering Object: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", i,i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); fflush(stdout);
   printf("<BR>%s ... ",aaa[25]); fflush(stdout);
   strcpy(trgObj, Sentries->entries[1].val);

     for (i = 5; i < Sentries->m ; i =  i+3) {
       if (strlen(Sentries->entries[i+1].val) == 0)
	 continue;
       strcpy(iName, Sentries->entries[i].val);
       strcpy(iVal, Sentries->entries[i+1].val);     
     strcpy(iUnit,Sentries->entries[i+2].val);
     iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   "0",iName,
			   D_INSERT_USER_DEFINED_STRING_META_DATA);
     if (iii < 0) {
       printf ("Metadata Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName,iii,iii);
       srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     sprintf(newVal,"1@%i",iii);

     ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   newVal,iVal,
			   D_CHANGE_USER_DEFINED_STRING_META_DATA);
     if (ii < 0) {
       printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName, iVal,ii,ii);
       srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     if (strlen(iUnit) > 0) {
       sprintf(newVal,"2@%i",iii);
       ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			     newVal,iUnit,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iUnit,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }


   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);

   return;
}

int
regORB(inStruct Sentries)
{
   char *trgColl,  *rsrcName, *dataType;
   int  i,ii,iii,out_fd, status;
   char trgObj[MAX_TOKEN * 3];
   char newVal[MAX_TOKEN];
   char iName[MAX_TOKEN];
   char iVal[MAX_TOKEN];
   char iUnit[MAX_TOKEN];
   srb_long_t  dataSize;

   strcpy(trgObj, Sentries->entries[1].val);
   trgColl = Sentries->entries[2].val;
   rsrcName = Sentries->entries[3].val;

   dataType = Sentries->entries[4].val;
   dataSize = -1;
 
   if (strlen(trgObj) == 0) {
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf ("Error: Target Data Name Empty. Please use back button and resubmit.\n");
     bRMap(1,0);
     printf("</body></html>");
     return;
   }
   printf("<title>Registering data file %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
   printf("<body background=\"%s/srb3.jpg\">", webPath);
   printf("<BR>Register Operation Beginning for %s  as %s/%s  ... ",
	 Sentries->entries[Sentries->m].val, trgColl, trgObj ); 
   fflush(stdout);
   if (strlen(Sentries->entries[5].val) == 0 || strlen(Sentries->entries[6].val) == 0) {
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf ("Error: ORB Name or Sensor Name is empty. Please use back button to fill and resubmit.\n");
     bRMap(1,0);
     printf("</body></html>");
     return;
   }
   /*** idablade.ucsd.edu<ORBSELECT>SMER_GORGE_CAMPBELL/EXP/ASC</ORBSELECT><ORBWHICH>-13</ORBWHICH><ORBTIMEOUT>10</ORBTIMEOUT><ORBNUMOFPKTS>60</ORBNUMOFPKTS><ORBPRESENTATION>WAVJAV1|| 06|| ||</ORBPRESENTATION><ORBNUMBULKREADS>30</ORBNUMBULKREADS>****/

   sprintf(Sentries->urlpathNameList,"%s<ORBSELECT>%s</ORBSELECT><ORBWHICH>%s</ORBWHICH><ORBTIMEOUT>%i</ORBTIMEOUT><ORBNUMOFPKTS>%i</ORBNUMOFPKTS><ORBPRESENTATION>%s%s</ORBPRESENTATION><ORBNUMBULKREADS>%i</ORBNUMBULKREADS>",
	      Sentries->entries[5].val, Sentries->entries[6].val, Sentries->entries[7].val,
	      atoi(Sentries->entries[8].val), atoi(Sentries->entries[9].val),
	      Sentries->entries[10].val, Sentries->entries[11].val, atoi(Sentries->entries[12].val));

   i = srbRegisterDataset(Sentries->conn, MDAS_CATALOG, trgObj,dataType,rsrcName,trgColl,
                                     Sentries->urlpathNameList, dataSize);
   if (i < 0) {
     printf ("<BR>Error when registering Object: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", i,i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); fflush(stdout);
   printf("<BR>%s ... ",aaa[25]); fflush(stdout);
   strcpy(trgObj, Sentries->entries[1].val);

     for (i = 13; i <= Sentries->m ; i =  i+3) {
       if (strlen(Sentries->entries[i+1].val) == 0)
	 continue;
       strcpy(iName, Sentries->entries[i].val);
       strcpy(iVal, Sentries->entries[i+1].val);     
     strcpy(iUnit,Sentries->entries[i+2].val);
     iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   "0",iName,
			   D_INSERT_USER_DEFINED_STRING_META_DATA);
     if (iii < 0) {
       printf ("Metadata Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName,iii,iii);
       srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     sprintf(newVal,"1@%i",iii);

     ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   newVal,iVal,
			   D_CHANGE_USER_DEFINED_STRING_META_DATA);
     if (ii < 0) {
       printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName, iVal,ii,ii);
       srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     if (strlen(iUnit) > 0) {
       sprintf(newVal,"2@%i",iii);
       ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			     newVal,iUnit,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iUnit,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }


   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);

   return;
}


int
regSQL(inStruct Sentries)
{
   char *trgColl,  *rsrcName, *dataType;
   int  i,ii,iii,out_fd, status;
   srb_long_t dataSize;
   char trgObj[MAX_TOKEN * 3];
   char newVal[MAX_TOKEN];
   char iName[MAX_TOKEN];
   char iVal[MAX_TOKEN];
   char iUnit[MAX_TOKEN];

   strcpy(trgObj, Sentries->entries[1].val);
   trgColl = Sentries->entries[2].val;
   rsrcName = Sentries->entries[3].val;

   dataType = Sentries->entries[4].val;
   dataSize = -1;
 
   if (strlen(trgObj) == 0) {
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf ("Error: Target Data Name Empty. Please use back button and resubmit.\n");
     bRMap(1,0);
     printf("</body></html>");
     return;
   }
   printf("<title>Registering data file %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
   printf("<body background=\"%s/srb3.jpg\">", webPath);
   printf("<BR>Register Operation Beginning for %s  as %s/%s  ... ",
	 Sentries->entries[Sentries->m].val, trgColl, trgObj ); 
   fflush(stdout);


   i = srbRegisterDataset(Sentries->conn, MDAS_CATALOG, trgObj,dataType,rsrcName,trgColl,
                                     Sentries->entries[Sentries->m].val, dataSize);
   if (i < 0) {
     printf ("<BR>Error when registering Object: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", i,i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); fflush(stdout);
   printf("<BR>%s ... ",aaa[25]); fflush(stdout);
   strcpy(trgObj, Sentries->entries[1].val);

     for (i = 5; i < Sentries->m ; i =  i+3) {
       if (strlen(Sentries->entries[i+1].val) == 0)
	 continue;
       strcpy(iName, Sentries->entries[i].val);
       strcpy(iVal, Sentries->entries[i+1].val);     
     strcpy(iUnit,Sentries->entries[i+2].val);
     iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   "0",iName,
			   D_INSERT_USER_DEFINED_STRING_META_DATA);
     if (iii < 0) {
       printf ("Metadata Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName,iii,iii);
       srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     sprintf(newVal,"1@%i",iii);

     ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   newVal,iVal,
			   D_CHANGE_USER_DEFINED_STRING_META_DATA);
     if (ii < 0) {
       printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName, iVal,ii,ii);
       srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     if (strlen(iUnit) > 0) {
       sprintf(newVal,"2@%i",iii);
       ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			     newVal,iUnit,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iUnit,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }


   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);

   return;
}


int
ingestNewVersionFileIntoSRB(inStruct Sentries)
{
   char *trgColl,  *rsrcName, *dataType, *oldReplNum;
   int  i, ii, iii, out_fd, status;
   srb_long_t dataSize;
   char trgObj[MAX_TOKEN * 3];
   char newVal[MAX_TOKEN * 3], dObjName[MAX_TOKEN * 3];
   char newReplNumTxt[MAX_TOKEN];  

   strcpy(trgObj, Sentries->entries[1].val);
   trgColl = Sentries->entries[2].val;
   rsrcName = Sentries->entries[5].val;
   dataType = Sentries->entries[6].val;
   dataSize = Sentries->entries[7].size;
   if (dataSize == 0)
     dataSize = strlen(Sentries->entries[7].val);
   oldReplNum = Sentries->entries[3].val;

   if (strlen(trgObj) == 0) {
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf ("Error: Target Data Name Empty. Please use back button and resubmit.\n");
     bRMap(1,0);
     printf("</body></html>");
     return;
   }
   printf("<title>Ingesting data file %s/%s</title> <base target = \"_top\">", Sentries->entries[2].val, Sentries->entries[1].val);
   printf("<body background=\"%s/srb3.jpg\">", webPath);
   printf("<BR>Ingest Operation Beginning for %s/%s of Size %d ... ",
	 trgColl, trgObj,dataSize ); 
   fflush(stdout);
   strcat(trgObj,"##");
   strcat(trgObj, Sentries->pidStr);
   out_fd = srbObjCreate (Sentries->conn, 0, 
	trgObj,dataType,rsrcName,trgColl,"",dataSize);
   if (out_fd < 0) {
     printf ("Unable to create object %s/%s,  <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, out_fd,out_fd);
     srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   i = srbObjWrite(Sentries->conn, out_fd, Sentries->entries[Sentries->m].val,dataSize);
   if (i != dataSize) {
     printf ("Write Error when create object %s/%s, wrote only %d of %d\n",
              trgColl, trgObj, i, dataSize);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   i = srbObjClose(Sentries->conn, out_fd);
   if (i < 0) {
     printf ("Error when closing object %s/%s. <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, i,i);
     bRMap(2,0);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     printf("</body></html>");
     return;
   }
   /*** register the temporary as a replica to original object****/
   i = getPathName(Sentries, trgColl, trgObj,0, newVal);
   if (i < 0) {
     printf ("Error when getting path name for object %s/%s. <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, i,i);
     bRMap(2,0);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     srbObjUnlink(Sentries->conn, trgObj,trgColl);

     printf("</body></html>");
     return;
   }
   strcat(newVal," ");
   
   j = srbRegisterReplica (Sentries->conn, 0, Sentries->entries[1].val,
                                     trgColl, "","",rsrcName,
                                     newVal, Sentries->srbUser, Sentries->userDomain);
   if (j < 0) {
     printf ("Error when registering replica for object %s/%s. <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              Sentries->entries[1].val, trgObj, j,j);
     bRMap(2,0);
     srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     srbObjUnlink(Sentries->conn, trgObj,trgColl);
     printf("</body></html>");
     return;
   }
   /*** unlink the the newly created temporary  file ****/
   i = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl,"","",
			"wewewerr","", D_DELETE_ONE); /**D_CHANGE_DPATH);**/
   if (i < 0) {
     printf ("Error when unlinking temporary object %s/%s. <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, i,i);
     bRMap(2,0);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     printf("</body></html>");
     return;
     } 
   
   sprintf(dObjName,"%s&COPY=%i", Sentries->entries[1].val,j);
   newVal[strlen(newVal) -1] = '\0';
   i = srbModifyDataset(Sentries->conn, MDAS_CATALOG, dObjName,trgColl, "","",
			   newVal,"", D_CHANGE_DPATH);
   if (i < 0) {
     printf ("DataPath Change Error for %s/%s to %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,dObjName,newVal,i,i);
     srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }   


   i = srbModifyDataset(Sentries->conn, MDAS_CATALOG, dObjName,trgColl, "","",
			   dataType,"", D_CHANGE_TYPE);
   if (i < 0) {
     printf ("Datatype Change Error for %s/%s to %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,dObjName,dataType,i,i);
     srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }   
   sprintf(newVal,"%i",dataSize);
   i = srbModifyDataset(Sentries->conn, MDAS_CATALOG, dObjName,trgColl, "","",
			  newVal,"", D_CHANGE_SIZE);
   if (i < 0) {
     printf ("Datatype Change Error for %s/%s to %i: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,dObjName,dataSize,i,i);
     srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }   

   
   sprintf(dObjName,"%s&COPY=%i", Sentries->entries[1].val, oldReplNum);
   sprintf(newReplNumTxt,"%s:%s",oldReplNum,oldReplNum);
   iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, dObjName,trgColl, "","",
			  newReplNumTxt,"-1:-1", D_CHANGE_REPLNUM_VERNUM);
   if (iii < 0) {
     printf ("Version Change Error for %s/%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,dObjName,iii,iii);
     srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   sprintf(dObjName,"%s&COPY=%i", Sentries->entries[1].val, j);
   sprintf(newReplNumTxt,"%i:%s",j,oldReplNum);
   iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, dObjName,trgColl, "","",
			  newReplNumTxt,"0:0", D_CHANGE_REPLNUM_VERNUM);
   if (iii < 0) {
     printf ("Version Change Error for %s/%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,dObjName,iii,iii);
     srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }

   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);
   printf("</body></html>");
   return;
}


int
ingestReplicaFileIntoSRB(inStruct Sentries)
{
   char *trgColl,  *rsrcName, *dataType;
   int  i, ii, iii, out_fd, status, dataSize;
   char trgObj[MAX_TOKEN * 3];
   char newVal[MAX_TOKEN * 3], dObjName[MAX_TOKEN * 3];

   strcpy(trgObj, Sentries->entries[1].val);
   trgColl = Sentries->entries[2].val;
   rsrcName = Sentries->entries[5].val;
   dataType = Sentries->entries[6].val;
   dataSize = Sentries->entries[7].size;
   if (dataSize == 0)
     dataSize = strlen(Sentries->entries[7].val);


   if (strlen(trgObj) == 0) {
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf ("Error: Target Data Name Empty. Please use back button and resubmit.\n");
     bRMap(1,0);
     printf("</body></html>");
     return;
   }
   printf("<title>Ingesting data file %s/%s</title> <base target = \"_top\">",Sentries->entries[2].val,Sentries->entries[1].val);
   printf("<body background=\"%s/srb3.jpg\">", webPath);
   printf("<BR>Ingest Operation Beginning for %s/%s of Size %d ... ",
	 trgColl, trgObj,dataSize ); 
   fflush(stdout);
   strcat(trgObj,"##");
   strcat(trgObj,Sentries->pidStr);
   out_fd = srbObjCreate (Sentries->conn, 0, 
	trgObj,dataType,rsrcName,trgColl,"",dataSize);
   if (out_fd < 0) {
     printf ("Unable to create object %s/%s,  <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, out_fd,out_fd);
     srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   i = srbObjWrite(Sentries->conn, out_fd, Sentries->entries[Sentries->m].val,dataSize);
   if (i != dataSize) {
     printf ("Write Error when create object %s/%s, wrote only %d of %d\n",
              trgColl, trgObj, i, dataSize);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   i = srbObjClose(Sentries->conn,out_fd);
   if (i < 0) {
     printf ("Error when closing object %s/%s. <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, i,i);
     bRMap(2,0);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     printf("</body></html>");
     return;
   }
   /*** register the temporary as a replica to original object****/
   i = getPathName(Sentries, trgColl, trgObj,0, newVal);
   if (i < 0) {
     printf ("Error when getting path name for object %s/%s. <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, i,i);
     bRMap(2,0);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     srbObjUnlink(Sentries->conn, trgObj,trgColl);

     printf("</body></html>");
     return;
   }
   strcat(newVal," ");
   j = srbRegisterReplica (Sentries->conn, 0, Sentries->entries[1].val,
                                     trgColl, "","",rsrcName,
                                     newVal, Sentries->srbUser, Sentries->userDomain);
   if (j < 0) {
     printf ("Error when registering replica for object %s/%s. <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              Sentries->entries[1].val, trgObj, j,j);
     bRMap(2,0);
     srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     srbObjUnlink(Sentries->conn, trgObj,trgColl);
     printf("</body></html>");
     return;
   }
   /*** unlink the the newly created temporary  file ****/
   i = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl,"","",
			"wewewerr","", D_DELETE_ONE); /**D_CHANGE_DPATH);**/
   if (i < 0) {
     printf ("Error when unlinking temporary object %s/%s. <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, i,i);
     bRMap(2,0);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     printf("</body></html>");
     return;
     } 
   
   sprintf(dObjName,"%s&COPY=%i",Sentries->entries[1].val,j);
   newVal[strlen(newVal) -1] = '\0';
   i = srbModifyDataset(Sentries->conn, MDAS_CATALOG, dObjName,trgColl, "","",
			   newVal,"", D_CHANGE_DPATH);
   if (i < 0) {
     printf ("DataPath Change Error for %s/%s to %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,dObjName,newVal,i,i);
     srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }   


   i = srbModifyDataset(Sentries->conn, MDAS_CATALOG, dObjName,trgColl, "","",
			   dataType,"", D_CHANGE_TYPE);
   if (i < 0) {
     printf ("Datatype Change Error for %s/%s to %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,dObjName,dataType,i,i);
     srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }   
   sprintf(newVal,"%i",dataSize);
   i = srbModifyDataset(Sentries->conn, MDAS_CATALOG, dObjName,trgColl, "","",
			  newVal,"", D_CHANGE_SIZE);
   if (i < 0) {
     printf ("Datatype Change Error for %s/%s to %i: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,dObjName,dataSize,i,i);
     srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }   
   

   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);
   printf("</body></html>");
   return;
}



int
makeDbCopy(inStruct Sentries)
{
   char *srcColl, *srcObj;
   char *trgColl, *trgObj;
   int  in_fd, out_fd, status;
   char temp[MAX_OBJ_NAME_LENGTH];
   char temp1[MAX_TOKEN];

   srcColl = Sentries->entries[1].val;
   srcObj = Sentries->entries[2].val;
   trgColl = Sentries->entries[3].val;
   trgObj = Sentries->entries[4].val;
   printf("<BR>Copy Operation Beginning ... "); fflush(stdout);

   in_fd = srbObjOpen (Sentries->conn, srcObj,  O_RDONLY,srcColl);
   if (in_fd < 0) {
     printf("Cannot open source object \"%s\" in collection \"%s\": <A href=\"srberror3.cgi?%i\">Error: %i</A> \n",
	     srcObj,srcColl, in_fd,in_fd);
     bRMap(2,0);
     return(in_fd);
   }
   out_fd = srbObjOpen (Sentries->conn, trgObj,  O_WRONLY, trgColl);
   if (out_fd < 0)  { 
     srbObjClose(Sentries->conn,in_fd);
     printf("Cannot open target object \"%s\" in collection \"%s\": <A href=\"srberror3.cgi?%i\">Error: %i</A> \n",
	    trgObj,trgColl, out_fd,out_fd);
     bRMap(2,0);
     return(out_fd);
   }
   sprintf (temp, "%i", in_fd);
   sprintf (temp1, "%i", out_fd);
   /***
   status = srbObjProxyOpr (Sentries->conn, OPR_COPY, temp, temp1, 0, 0,
         NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
   ***/
   srbObjClose (Sentries->conn, out_fd);
   srbObjClose (Sentries->conn, in_fd);
   if (status < 0) {
     printf ("Error in Copy: <A href=\"srberror3.cgi?%i\">Error: %i</A><P>\n",status,status );
   }
   printf(" Successful");fflush(stdout);
   bRMap(2,1);
   return(status);
}





/***** OLD
int getDatasetDC(char *collection, char *dataName)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM], orderby[100];
    int i, j;
    int status;
    int collLike;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      likeCollFlag = 1;
      selval[DATA_GRP_NAME] = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    if (strstr(dataName,"^")) {
       make_like_for_mdasmysrb(dataName);
       client_set_qval_data_cond(dataName,qval);
       selval[DATA_NAME] = 1;
    }
    else {
      if (strstr(dataName,"*") || strstr(dataName,"?")) {
        make_like_for_mdasmysrb(dataName);
        sprintf(qval[DATA_NAME], " like '%s'  ESCAPE '\\' ",dataName);
	selval[DATA_NAME] = 1;
      }
      else {
        sprintf(qval[DATA_NAME], " =  '%s'",dataName);
      }
   }

    selval[DC_CONTRIBUTOR_TYPE] = 1;    orderby[0]=DC_TITLE;
    selval[DC_SUBJECT_CLASS] = 1;       orderby[1]=DC_CONTRIBUTOR_NAME;
    selval[DC_DESCRIPTION_TYPE] = 1;    orderby[2]=DC_CONTRIBUTOR_TYPE;
    selval[DC_TYPE] = 1;                orderby[4]=DC_SUBJECT_NAME;
    selval[DC_SOURCE_TYPE] = 1;         orderby[3]=DC_SUBJECT_CLASS;;
    selval[DC_LANGUAGE] = 1;            orderby[5]=DC_LANGUAGE;
    selval[DC_RELATION_TYPE] = 1;       orderby[6]=DC_TYPE;
    selval[DC_COVERAGE_TYPE] = 1;       orderby[8]=DC_DESCRIPTION;
    selval[DC_RIGHTS_TYPE] = 1;         orderby[7]=DC_DESCRIPTION_TYPE;
    selval[DC_TITLE] = 1;               orderby[9]=DC_SOURCE;
    selval[DC_CONTRIBUTOR_NAME] = 1;    orderby[10]=DC_SOURCE_TYPE;
    selval[DC_SUBJECT_NAME] = 1;        orderby[11]=DC_COVERAGE;
    selval[DC_DESCRIPTION] = 1;         orderby[12]=DC_COVERAGE_TYPE;
    selval[DC_PUBLISHER] = 1;           orderby[13]=DC_RIGHTS;
    selval[DC_SOURCE] = 1;              orderby[14]=DC_RIGHTS_TYPE;
    selval[DC_RELATED_DATA_DESCR] = 1;  orderby[15]=DC_PUBLISHER;
    selval[DC_RELATED_DATA] = 1;        orderby[16]=DC_RELATED_DATA;
    selval[DC_RELATED_COLL] = 1;        orderby[17]=DC_RELATED_COLL;
    selval[DC_COVERAGE] = 1;            orderby[18]=DC_RELATED_DATA_DESCR;
    selval[DC_RIGHTS] = 1;              orderby[19]=DC_RELATION_TYPE;

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadat Access <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", status,status);fflush(stdout);
	return(status);
    }
    else {
      filterDeleted (&collResult);
      prtTableHeader("dublin core");
      first = 1;
      prtMetaQueryResult(&collResult, orderby);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtMetaQueryResult(&collResult, orderby);
      }
      clearSqlResult (&collResult);
    }
    return(0);
}
*****/



int
schemaQueryValues(inStruct Sentries, int m)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j,jj;
    int status,showMDVFlag;
    int collLike;
    char st1[MAX_OBJ_NAME_LENGTH],st2[MAX_OBJ_NAME_LENGTH];
    int attrId;

    showMDVCntr = 0;
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    for (i = 1; i  < m  ; i++) {
      /* printf("%i:%i:%s=%s:%s=%s:%s=%s<br>\n",m,i,entries[i].name,entries[i].val,entries[i+1].name,entries[i+1].val,entries[i+2].name,entries[i+2].val); fflush(stdout);*/
      if (strstr(Sentries->entries[i].name,"showSMDV") == Sentries->entries[i].name) {
	i++;
	showMDVTitle[showMDVCntr] = Sentries->entries[i+2].name;
	showMDVId[showMDVCntr] = atoi(Sentries->entries[i].val);
	showMDVCntr++;
	selval[atoi(Sentries->entries[i].val)] = 1;
      }
      if (strlen(Sentries->entries[i+2].val) == 0) {
	i = i+2;
	continue;
      }
      if (!strcmp(Sentries->entries[i].name,"number")) {
	sprintf(qval[atoi(Sentries->entries[i].val)]," %s %s", 
		Sentries->entries[i+1].val,Sentries->entries[i+2].val);
      }
      else {
	if (!strcmp(Sentries->entries[i+1].val,"like") || 
	    !strcmp(Sentries->entries[i+1].val,"not like")) 
	  make_like_for_mdasmysrb(Sentries->entries[i+2].val);
	sprintf(qval[atoi(Sentries->entries[i].val)]," %s '%s'", 
		Sentries->entries[i+1].val,Sentries->entries[i+2].val);
      }
      i = i+2;
    }

    selval[DATA_GRP_NAME] = 1;
    selval[DATA_NAME] = 1;
    selval[SIZE] = 1;
    selval[DATA_TYP_NAME] = 1;
    selval[DATA_OWNER] = 1;
    selval[DATA_OWNER_DOMAIN] = 1;
    selval[REPL_TIMESTAMP] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[DATA_VER_NUM] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[CONTAINER_NAME] = 1;
#ifdef DATACOMMENTS
    selval[DATA_COMMENTS] = 14;
#endif
    selval[PATH_NAME] = 1;

    /****
    for (i = 0; i < MAX_DCS_NUM; i++) {
        if (selval[i] > 0)
	  printf("%i:   SValue:%i<BR>\n",i,selval[i]);
        if (strcmp(qval[i],""))
	  printf("%i:   QValue:%s<BR>\n",i,qval[i]);
    }
	return(0);
    ***/
    queryFunction = 1;
    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);
    /*printf("Status:%i\n",status); fflush(stdout);*/
    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	else
	  printf("No Answer found for the Query\n");
	bRMap(1,0);
	return;
    }
    else {
      filterDeleted (&collResult);
      first = 1; Sentries->rowCntr = 1; printf("<TABLE CELLSPACING=0 CELLPADDING=0 RULES=ALL>\n");
      prtDataLongWeb (Sentries, &collResult, CHK_COLL);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtDataLongWeb (Sentries, &collResult, CHK_COLL);
      }
    }
    if (filesfd != NULL && filesfd != stdout) {
      fflush(filesfd);
      fseek(filesfd, 0 , SEEK_SET);
      while ((j = fread (buf,1,60000,filesfd)) > 0) {
	buf[j] = '\0';
	printf("%s",buf);
      }
    }
    fprintf(stdout,"</TABLE>\n");
    clearSqlResult (&collResult);

    bRMap(1,0);
    return;
}


int
extSchemaOnlyQueryValues(inStruct Sentries)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int i, j,jj, collMDRank;
    int status,showMDVFlag;
    int collLike;
    char st1[MAX_OBJ_NAME_LENGTH],st2[MAX_OBJ_NAME_LENGTH];
    int attrId;
    int collOnly = 0;
    int numCompCount = 0;


    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    Sentries->showMDVCntr = 0;
    showMDVFlag = 0;
    readExtendedTableAttributeIndex(Sentries, Sentries->entries[1].val,Sentries->udsmdArray);
    for (i = 2 ; i <= Sentries->m - 1 ; i++) {
            showMDVFlag = 0;
      if (strstr(Sentries->entries[i].name,"showESA") == Sentries->entries[i].name) {
	i++;
	if (strlen(Sentries->entries[i].val) == 0) {
	  i = i+2;
	  continue;
	}
	Sentries->showMDVTitle[Sentries->showMDVCntr] = Sentries->entries[i].val;
	Sentries->showMDVCntr++;
	showMDVFlag = 1 ;
      }
      if (strlen(Sentries->entries[i].val) == 0) {
	i = i+2;
	continue;
      }
      if (!strcmp(Sentries->entries[i+1].val,"like") || !strcmp(Sentries->entries[i+1].val,"not like")) 
            make_like_for_mdasmysrb(Sentries->entries[i+2].val);
      j = getIndexForConstName(Sentries->entries[i].val);

      if ( j < 0) {
	clFinish(Sentries->conn);
	sprintf(st1,"Unknown Schema Attribute:%s",Sentries->entries[i].val);
	webErrorExit(st1, j);
      }
      if (strlen(Sentries->entries[i+2].val) > 0) {
	if (isAlphaString(Sentries->entries[i+2].val))
	  sprintf(qval[j]," %s '%s'",Sentries->entries[i+1].val,Sentries->entries[i+2].val);
	else
	  sprintf(qval[j]," %s %s",Sentries->entries[i+1].val,Sentries->entries[i+2].val);
      }
      if (showMDVFlag == 1) {
          selval[j] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = j;
      }
      i = i+2; 
    }

    Sentries->queryFunction = 1;
    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);
    /*printf("Status:%i\n",status); fflush(stdout);*/
    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	else
	  printf("No Answer found for the Query\n");
	bRMap(1,0);
	return;
    }
    filterDeleted (&collResult);
    Sentries->first = 1; Sentries->rowCntr = 1; printf("<TABLE CELLSPACING=0 CELLPADDING=0 RULES=ALL>\n");
    prtNoDataLongWeb (Sentries, &collResult);
    
    
    fflush(stdout);
    
    while (collResult.continuation_index >= 0) {
      clearSqlResult (&collResult);
      
      status = srbGetMoreRows(Sentries->conn, 0,
			      collResult.continuation_index,
			      &collResult, MAX_RESULT_NUM);
      if (collResult.result_count == 0  || status != 0)
	break;
      filterDeleted (&collResult);
      prtNoDataLongWeb (Sentries, &collResult);
      fflush(stdout);
    }
    if (filesfd != NULL && filesfd != stdout) {
      fflush(filesfd);
      fseek(filesfd, 0 , SEEK_SET);
      while ((j = fread (buf,1,60000,filesfd)) > 0) {
	buf[j] = '\0';
	printf("%s",buf);
      }
    }
    fprintf(stdout,"</TABLE>\n");
    clearSqlResult (&collResult);

    bRMap(1,0);
    return;

}

int
extSchemaQueryValues(inStruct Sentries)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int i, j,jj, collMDRank;
    int status,showMDVFlag;
    int collLike;
    char st1[MAX_OBJ_NAME_LENGTH],st2[MAX_OBJ_NAME_LENGTH];
    int attrId;
    int collOnly = 0;
    int numCompCount = 0;


    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    Sentries->showMDVCntr = 0;
    showMDVFlag = 0;
    readExtendedTableAttributeIndex(Sentries, Sentries->entries[1].val,Sentries->udsmdArray);
    for (i = 2 ; i <= Sentries->m ; i++) {
      if (strstr(Sentries->entries[i].name,"showESA") != Sentries->entries[i].name && 
	  strstr(Sentries->entries[i].name,"esa")  != Sentries->entries[i].name )
	break;
            showMDVFlag = 0;
      if (strstr(Sentries->entries[i].name,"showESA") == Sentries->entries[i].name) {
	i++;
	if (strlen(Sentries->entries[i].val) == 0) {
	  i = i+2;
	  continue;
	}
	Sentries->showMDVTitle[Sentries->showMDVCntr] = Sentries->entries[i].val;
	Sentries->showMDVCntr++;
	showMDVFlag = 1 ;
      }
      if (strlen(Sentries->entries[i].val) == 0) {
	i = i+2;
	continue;
      }
      if (!strcmp(Sentries->entries[i+1].val,"like") || !strcmp(Sentries->entries[i+1].val,"not like")) 
            make_like_for_mdasmysrb(Sentries->entries[i+2].val);

      j = getIndexForConstName(Sentries->entries[i].val);
      if ( j < 0) {
	clFinish(Sentries->conn);
	webErrorExit("Unknown Schema Attribute", j);
      }
      if (strlen(Sentries->entries[i+2].val) > 0) {
	if (isAlphaString(Sentries->entries[i+2].val))
	  sprintf(qval[j]," %s '%s'",Sentries->entries[i+1].val,Sentries->entries[i+2].val);
	else
	  sprintf(qval[j]," %s %s",Sentries->entries[i+1].val,Sentries->entries[i+2].val);
      }
      if (showMDVFlag == 1) {
          selval[j] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = j;
      }
      i = i+2; 
    }


/** if 'show collections only' checkbox is checked it generates an extra entry that
    will mess up with the rest of the count, so we get rid of it **/
    if (!strcmp(Sentries->entries[Sentries->m].val, "on")) {
        /** means we want to show collection names only **/
	    collOnly=1;
	    strcpy(Sentries->entries[Sentries->m].val, Sentries->entries[Sentries->m+1].val);
	    Sentries->m--;
    }


/**** if the 'more options' menu has been openned, "entries" numbers for collection metadata are
	6 ranks further. if 'more options' has not been openned then 30 < m < 42 , otherwise m > 48.
	this values might change if function is modified    *************************************/


    	collMDRank = 16;
    /*    for (jj =  0, i = 1; i  < Sentries->m  ; jj++,i++) { changed for ESA ****/
    for (jj =  0; i  < Sentries->m  ; jj++,i++) { 
      /*      printf("%i:%i:%s->%s:%s:%s<br>\n",jj,i,Sentries->entries[i].name,Sentries->entries[i].val,Sentries->entries[i+1].val,Sentries->entries[i+2].val);	*/
      showMDVFlag = 0;
      if (strstr(Sentries->entries[i].name,"showMDV") == Sentries->entries[i].name) {
	i++;
	if (strlen(Sentries->entries[i].val) == 0) {
	  i = i+2;
	  continue;
	}
	Sentries->showMDVTitle[Sentries->showMDVCntr] = Sentries->entries[i].val;
	Sentries->showMDVCntr++;
	showMDVFlag = 1 ;
      }
      if (strlen(Sentries->entries[i].val) == 0) {
	i = i+2;
	continue;
      }
      if (!strcmp(Sentries->entries[i+1].val,"like") || !strcmp(Sentries->entries[i+1].val,"not like")) 
            make_like_for_mdasmysrb(Sentries->entries[i+2].val);


	
      if (jj == 0) {
	sprintf(qval[UDSMD0]," = '%s'",Sentries->entries[i].val);
	/***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD1]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/	  
	if (showMDVFlag == 1) {
	  selval[UDSMD1] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD1;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD1]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD1]," ESCAPE '\\' ");
	}     
	selval[UDSMD5] = 1;
      }
      else if (jj == 1) {
	sprintf(qval[UDSMD0_1]," = '%s'",Sentries->entries[i].val);
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD1_1]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/ 
	if (showMDVFlag == 1) {
	  selval[UDSMD1_1] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD1_1;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD1_1]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD1_1]," ESCAPE '\\' ");
	}
	selval[UDSMD5] = 1;
      }
      else if (jj == 2) {
	sprintf(qval[UDSMD0_2]," = '%s'",Sentries->entries[i].val); 
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD1_2]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/
	if (showMDVFlag == 1) {
	  selval[UDSMD1_2] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD1_2;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD1_2]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD1_2]," ESCAPE '\\' ");
	}
	selval[UDSMD5] = 1;
      }
      else if (jj == 3) {
	sprintf(qval[UDSMD0_3]," = '%s'",Sentries->entries[i].val); 
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD1_3]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/
	if (showMDVFlag == 1) {
	  selval[UDSMD1_3] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD1_3;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD1_3]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD1_3]," ESCAPE '\\' ");
	}
	selval[UDSMD5] = 1;
      }
      else if (jj == 4) {
	sprintf(qval[UDSMD0_4]," = '%s'",Sentries->entries[i].val); 
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD1_4]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/	
	if (showMDVFlag == 1) {
	  selval[UDSMD1_4] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD1_4;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD1_4]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD1_4]," ESCAPE '\\' ");
	}
	selval[UDSMD5] = 1;
      }

/****** added for collection metadata **************/
/****** see above for explanations about collMDRank ***********/
      else if (jj == 5 + collMDRank ) {
	sprintf(qval[UDSMD_COLL0]," = '%s'",Sentries->entries[i].val);
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD_COLL1]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/
	if (showMDVFlag == 1) {
	  selval[UDSMD_COLL1] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD_COLL1;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD_COLL1]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD_COLL1]," ESCAPE '\\' ");
	}
      }
      else if (jj == 6 + collMDRank ) {
	sprintf(qval[UDSMD_COLL0_1]," = '%s'",Sentries->entries[i].val);
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD_COLL1_1]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/	 
	if (showMDVFlag == 1) {
	  selval[UDSMD_COLL1_1] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD_COLL1_1;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD_COLL1_1]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val);
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD_COLL1_1]," ESCAPE '\\' ");
	}
      }
      else if (jj == 7 + collMDRank ) {
	sprintf(qval[UDSMD_COLL0_2]," = '%s'",Sentries->entries[i].val);
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD_COLL1_2]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/
	if (showMDVFlag == 1) {
	  selval[UDSMD_COLL1_2] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD_COLL1_2;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD_COLL1_2]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD_COLL1_2]," ESCAPE '\\' ");
	}
      }
      else if (jj == 8 + collMDRank ) {
	sprintf(qval[UDSMD_COLL0_3]," = '%s'",Sentries->entries[i].val);
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD_COLL1_3]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/	
	if (showMDVFlag == 1) {
	  selval[UDSMD_COLL1_3] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD_COLL1_3;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD_COLL1_3]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD_COLL1_3]," ESCAPE '\\' ");
	}
      }
      else if (jj == 9 + collMDRank ) {
	sprintf(qval[UDSMD_COLL0_4]," = '%s'",Sentries->entries[i].val);
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD_COLL1_4]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/	
	if (showMDVFlag == 1) {
	  selval[UDSMD_COLL1_4] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD_COLL1_4;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD_COLL1_4]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD_COLL1_4]," ESCAPE '\\' ");
	}
      }
/********* end of collection metadata **************/        
      
      
      else {
	j = atoi(Sentries->entries[i].val);
      	if (showMDVFlag == 1) {
	  selval[j] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr - 1] = j;
	  if (j == DATA_ANNOTATION_USERNAME) {
	    Sentries->showMDVTitle[Sentries->showMDVCntr - 1] = metaAttrNames[19];
	    selval[DATA_ANNOTATION_USERDOMAIN] = 1;
	    Sentries->showMDVTitle[Sentries->showMDVCntr] = metaAttrNames[18];
	    Sentries->showMDVId[Sentries->showMDVCntr] = DATA_ANNOTATION_USERDOMAIN;
	    Sentries->showMDVCntr++;
	  }
	  if (j == USER_NAME) {
            Sentries->showMDVTitle[Sentries->showMDVCntr - 1] = aaa[11];
	    selval[DOMAIN_DESC] = 1;
	    Sentries->showMDVTitle[Sentries->showMDVCntr] = aaa[13];
	    Sentries->showMDVId[Sentries->showMDVCntr] = DOMAIN_DESC;
	    Sentries->showMDVCntr++;
	  } 
	  else if (j == DATA_ANNOTATION)
	    Sentries->showMDVTitle[Sentries->showMDVCntr - 1] = metaAttrNames[18];
	  else if (j == DATA_CREATE_TIMESTAMP)
	    Sentries->showMDVTitle[Sentries->showMDVCntr - 1] = aaa[7];
	  else
	    Sentries->showMDVCntr--;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  if (j == DATA_OWNER) {
	    splitbycharwesc(Sentries->entries[i+2].val, st1, st2, '@' , '\\');
	    sprintf(qval[DATA_OWNER], " %s '%s'", Sentries->entries[i+1].val, st1);
	    sprintf(qval[DATA_OWNER_DOMAIN], " %s '%s'", Sentries->entries[i+1].val, st2);
	    if (strstr(Sentries->entries[i+1].val,"like") != NULL) {
	      strcat(qval[DATA_OWNER]," ESCAPE '\\' ");
	      strcat(qval[DATA_OWNER_DOMAIN]," ESCAPE '\\' ");
	    }
	  }
	  else if (j == DATA_ANNOTATION_USERNAME) {
	    splitbycharwesc(Sentries->entries[i+2].val, st1, st2, '@' , '\\');
	    sprintf(qval[DATA_ANNOTATION_USERNAME], " %s '%s'", Sentries->entries[i+1].val, st1);
	    sprintf(qval[DATA_ANNOTATION_USERDOMAIN], " %s '%s'", Sentries->entries[i+1].val, st2);
	    if (strstr(Sentries->entries[i+1].val,"like") != NULL) {
	      strcat(qval[DATA_ANNOTATION_USERNAME]," ESCAPE '\\' ");
	      strcat(qval[DATA_ANNOTATION_USERDOMAIN]," ESCAPE '\\' ");
	    }
	  }
	  else if (j == USER_NAME) {
	    splitbycharwesc(Sentries->entries[i+2].val, st1, st2, '@' , '\\');
            sprintf(qval[USER_NAME], " %s '%s'", Sentries->entries[i+1].val, st1);
            sprintf(qval[DOMAIN_DESC], " %s '%s'", Sentries->entries[i+1].val, st2);
            if (strstr(Sentries->entries[i+1].val,"like") != NULL) {
              strcat(qval[USER_NAME]," ESCAPE '\\' ");
              strcat(qval[DOMAIN_DESC]," ESCAPE '\\' ");
            }
          }
	  else if (j < MAX_DCS_NUM ){
	    sprintf(qval[j]," %s '%s'", Sentries->entries[i+1].val, Sentries->entries[i+2].val); 
	    if (strstr(Sentries->entries[i+1].val,"like") != NULL) 
	      strcat(qval[j]," ESCAPE '\\' ");
	  }
	}
      }
      i = i+2; 
    }
    


    

    if (collOnly == 1) {
        selval[DATA_GRP_NAME] = 1;
        selval[COLL_OWNER_NAME] = 1;
        selval[COLL_OWNER_DOMAIN] = 1;
        selval[COLL_CREATE_TIMESTAMP] = 1;
        selval[COLL_COMMENTS] = 1;
    }
    else {
        selval[DATA_GRP_NAME] = 1;
        selval[DATA_NAME] = 1;
        selval[SIZE] = 1;
        selval[DATA_TYP_NAME] = 1;
        selval[DATA_OWNER] = 1;
        selval[DATA_OWNER_DOMAIN] = 1;
        selval[REPL_TIMESTAMP] = 1;
        selval[DATA_REPL_ENUM] = 1;
        selval[DATA_VER_NUM] = 1;
        selval[PHY_RSRC_NAME] = 1;
        selval[CONTAINER_NAME] = 1;
#ifdef DATACOMMENTS
        selval[DATA_COMMENTS] = 14;
#endif
        selval[PATH_NAME] = 1;
    }

    /****
    for (i = 0; i < MAX_DCS_NUM; i++) {
        if (selval[i] > 0)
	  printf("%i:   SValue:%i<BR>\n",i,selval[i]);
        if (strcmp(qval[i],""))
	  printf("%i:   QValue:%s<BR>\n",i,qval[i]);
    }
	return(0);
    ***/
    Sentries->queryFunction = 1;

    /********  search from selected collection recursively  ***********/
    sprintf(qval[DATA_GRP_NAME], " like '%s%%'  ESCAPE '\\' ", Sentries->entries[Sentries->m].val);

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);
    /*printf("Status:%i\n",status); fflush(stdout);*/
    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	else
	  printf("No Answer found for the Query\n");
	bRMap(1,0);
	return;
    }
    else {
      filterDeleted (&collResult);
      Sentries->first = 1; Sentries->rowCntr = 1; printf("<TABLE CELLSPACING=0 CELLPADDING=0 RULES=ALL>\n");

    if (collOnly == 1) {
	    prtCollWeb_query(Sentries, &collResult);   
    }
    else {
        prtDataLongWeb (Sentries, &collResult, CHK_COLL);
    }
    
    fflush(stdout);
    
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	
    if (collOnly == 1) {
	    prtCollWeb_query(Sentries, &collResult);   
    }
    else {
        prtDataLongWeb (Sentries, &collResult, CHK_COLL);
    }
    
    fflush(stdout);
	
      }
    }
    if (filesfd != NULL && filesfd != stdout) {
      fflush(filesfd);
      fseek(filesfd, 0 , SEEK_SET);
      while ((j = fread (buf,1,60000,filesfd)) > 0) {
	buf[j] = '\0';
	printf("%s",buf);
      }
    }
    fprintf(stdout,"</TABLE>\n");
    clearSqlResult (&collResult);

    bRMap(1,0);
    return;
    

    /************************8 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4
        selval[DATA_GRP_NAME] = 1;
        selval[DATA_NAME] = 1;
        selval[SIZE] = 1;
        selval[DATA_TYP_NAME] = 1;
        selval[DATA_OWNER] = 1;
        selval[DATA_OWNER_DOMAIN] = 1;
        selval[REPL_TIMESTAMP] = 1;
        selval[DATA_REPL_ENUM] = 1;
        selval[DATA_VER_NUM] = 1;
        selval[PHY_RSRC_NAME] = 1;
        selval[CONTAINER_NAME] = 1;
#ifdef DATACOMMENTS
        selval[DATA_COMMENTS] = 14;
#endif
        selval[PATH_NAME] = 1;
    Sentries->queryFunction = 1;

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);
    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	else
	  printf("No Answer found for the Query\n");
	bRMap(1,0);
	return;
    }
    else {
      filterDeleted (&collResult);
      Sentries->first = 1; Sentries->rowCntr = 1; printf("<TABLE CELLSPACING=0 CELLPADDING=0 RULES=ALL>\n");

    if (collOnly == 1) {
	    prtCollWeb_query(Sentries, &collResult);   
    }
    else {
        prtDataLongWeb (Sentries, &collResult, CHK_COLL);
    }
    
    fflush(stdout);
    
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	
    if (collOnly == 1) {
	    prtCollWeb_query(Sentries, &collResult);   
    }
    else {
        prtDataLongWeb (Sentries, &collResult, CHK_COLL);
    }
    
    fflush(stdout);
	
      }
    }
    if (filesfd != NULL && filesfd != stdout) {
      fflush(filesfd);
      fseek(filesfd, 0 , SEEK_SET);
      while ((j = fread (buf,1,60000,filesfd)) > 0) {
	buf[j] = '\0';
	printf("%s",buf);
      }
    }
    fprintf(stdout,"</TABLE>\n");
    clearSqlResult (&collResult);

    bRMap(1,0);
    return;
    ************************8 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ *****/ 
}

int
browseQueryValues(inStruct Sentries)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int i, j,jj, collMDRank;
    int status,showMDVFlag;
    int collLike;
    char st1[MAX_OBJ_NAME_LENGTH],st2[MAX_OBJ_NAME_LENGTH];
    int attrId;
    int collOnly = 0;
    int numCompCount = 0;


    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    Sentries->showMDVCntr = 0;
    showMDVFlag = 0;

/** if 'show collections only' checkbox is checked it generates an extra entry that
    will mess up with the rest of the count, so we get rid of it **/
    if (!strcmp(Sentries->entries[Sentries->m].val, "on")) {
        /** means we want to show collection names only **/
	    collOnly=1;
	    strcpy(Sentries->entries[Sentries->m].val, Sentries->entries[Sentries->m+1].val);
	    Sentries->m--;
    }


/**** if the 'more options' menu has been openned, "entries" numbers for collection metadata are
	6 ranks further. if 'more options' has not been openned then 30 < m < 42 , otherwise m > 48.
	this values might change if function is modified    *************************************/

    if (Sentries->m > 48) {
    	collMDRank = 6;
    }
    else {
    	collMDRank = 0;
    }

    for (jj =  0, i = 1; i  < Sentries->m  ; jj++,i++) {
      /*printf("%i:%i:%s:%s:%s<br>\n",m,i,entries[i].val,entries[i+1].val,entries[i+2].val);*/
      showMDVFlag = 0;
      if (strstr(Sentries->entries[i].name,"showMDV") == Sentries->entries[i].name) {
	i++;
	if (strlen(Sentries->entries[i].val) == 0) {
	  i = i+2;
	  continue;
	}
	Sentries->showMDVTitle[Sentries->showMDVCntr] = Sentries->entries[i].val;
	Sentries->showMDVCntr++;
	showMDVFlag = 1 ;
      }
      if (strlen(Sentries->entries[i].val) == 0) {
	i = i+2;
	continue;
      }
      if (!strcmp(Sentries->entries[i+1].val,"like") || !strcmp(Sentries->entries[i+1].val,"not like")) 
            make_like_for_mdasmysrb(Sentries->entries[i+2].val);
	
	
      if (jj == 0) {
	sprintf(qval[UDSMD0]," = '%s'",Sentries->entries[i].val);
	/***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD1]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }

	  } 
    /***********************************/	  
	if (showMDVFlag == 1) {
	  selval[UDSMD1] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD1;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD1]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD1]," ESCAPE '\\' ");
	}     
	selval[UDSMD5] = 1;
      }
      else if (jj == 1) {
	sprintf(qval[UDSMD0_1]," = '%s'",Sentries->entries[i].val);
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD1_1]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/ 
	if (showMDVFlag == 1) {
	  selval[UDSMD1_1] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD1_1;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD1_1]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD1_1]," ESCAPE '\\' ");
	}
	selval[UDSMD5] = 1;
      }
      else if (jj == 2) {
	sprintf(qval[UDSMD0_2]," = '%s'",Sentries->entries[i].val); 
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD1_2]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/
	if (showMDVFlag == 1) {
	  selval[UDSMD1_2] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD1_2;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD1_2]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD1_2]," ESCAPE '\\' ");
	}
    selval[UDSMD5] = 1;
      }
      else if (jj == 3) {
	sprintf(qval[UDSMD0_3]," = '%s'",Sentries->entries[i].val); 
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD1_3]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/
	if (showMDVFlag == 1) {
	  selval[UDSMD1_3] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD1_3;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD1_3]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD1_3]," ESCAPE '\\' ");
	}
    selval[UDSMD5] = 1;
      }
      else if (jj == 4) {
	sprintf(qval[UDSMD0_4]," = '%s'",Sentries->entries[i].val); 
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD1_4]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/	
	if (showMDVFlag == 1) {
	  selval[UDSMD1_4] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD1_4;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD1_4]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD1_4]," ESCAPE '\\' ");
	}
    selval[UDSMD5] = 1;
      }

/****** added for collection metadata **************/
/****** see above for explanations about collMDRank ***********/
      else if (jj == 5 + collMDRank ) {
	sprintf(qval[UDSMD_COLL0]," = '%s'",Sentries->entries[i].val);
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD_COLL1]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/
	if (showMDVFlag == 1) {
	  selval[UDSMD_COLL1] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD_COLL1;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD_COLL1]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD_COLL1]," ESCAPE '\\' ");
	}
      }
      else if (jj == 6 + collMDRank ) {
	sprintf(qval[UDSMD_COLL0_1]," = '%s'",Sentries->entries[i].val);
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD_COLL1_1]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/	 
	if (showMDVFlag == 1) {
	  selval[UDSMD_COLL1_1] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD_COLL1_1;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD_COLL1_1]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val);
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD_COLL1_1]," ESCAPE '\\' ");
	}
      }
      else if (jj == 7 + collMDRank ) {
	sprintf(qval[UDSMD_COLL0_2]," = '%s'",Sentries->entries[i].val);
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD_COLL1_2]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/
	if (showMDVFlag == 1) {
	  selval[UDSMD_COLL1_2] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD_COLL1_2;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD_COLL1_2]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD_COLL1_2]," ESCAPE '\\' ");
	}
      }
      else if (jj == 8 + collMDRank ) {
	sprintf(qval[UDSMD_COLL0_3]," = '%s'",Sentries->entries[i].val);
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD_COLL1_3]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/	
	if (showMDVFlag == 1) {
	  selval[UDSMD_COLL1_3] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD_COLL1_3;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD_COLL1_3]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD_COLL1_3]," ESCAPE '\\' ");
	}
      }
      else if (jj == 9 + collMDRank ) {
	sprintf(qval[UDSMD_COLL0_4]," = '%s'",Sentries->entries[i].val);
    /***** for numeric comparison ******/
	  if (strstr(Sentries->entries[i+1].val, "num") ) {
	        Sentries->entries[i+1].val+=4;
	        if (numCompCount < MAX_NUM_COMP_COUNT) {
    	        sprintf(qval[USERDEFFUNC01+numCompCount], "TO_NUMBER(SRBATTR[UDSMD_COLL1_4]) %s %s ", Sentries->entries[i+1].val, Sentries->entries[i+2].val);
    	        Sentries->entries[i+2].val[0] = '\0';
    	        numCompCount++;
    	    }
	  } 
    /***********************************/	
	if (showMDVFlag == 1) {
	  selval[UDSMD_COLL1_4] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr-1] = UDSMD_COLL1_4;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  sprintf(qval[UDSMD_COLL1_4]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	  if (strstr(Sentries->entries[i+1].val,"like") != NULL)
	    strcat(qval[UDSMD_COLL1_4]," ESCAPE '\\' ");
	}
      }
/********* end of collection metadata **************/        
      
      
      else {
	j = atoi(Sentries->entries[i].val);
      	if (showMDVFlag == 1) {
	  selval[j] = 1;
	  Sentries->showMDVId[Sentries->showMDVCntr - 1] = j;
	  if (j == DATA_ANNOTATION_USERNAME) {
	    Sentries->showMDVTitle[Sentries->showMDVCntr - 1] = metaAttrNames[19];
	    selval[DATA_ANNOTATION_USERDOMAIN] = 1;
	    Sentries->showMDVTitle[Sentries->showMDVCntr] = metaAttrNames[18];
	    Sentries->showMDVId[Sentries->showMDVCntr] = DATA_ANNOTATION_USERDOMAIN;
	    Sentries->showMDVCntr++;
	  }
	  else if (j == DATA_ANNOTATION)
	    Sentries->showMDVTitle[Sentries->showMDVCntr - 1] = metaAttrNames[18];
	  else
	    Sentries->showMDVCntr--;
	}
	if (strlen(Sentries->entries[i+2].val) > 0) {
	  if (j == DATA_OWNER) {
	    splitbycharwesc(Sentries->entries[i+2].val, st1, st2, '@' , '\\');
	    sprintf(qval[DATA_OWNER], " %s '%s'", Sentries->entries[i+1].val, st1);
	    sprintf(qval[DATA_OWNER_DOMAIN], " %s '%s'", Sentries->entries[i+1].val, st2);
	    if (showMDVFlag == 1) 
	      selval[DATA_OWNER_DOMAIN] = 1;
	    if (strstr(Sentries->entries[i+1].val,"like") != NULL) {
	      strcat(qval[DATA_OWNER]," ESCAPE '\\' ");
	      strcat(qval[DATA_OWNER_DOMAIN]," ESCAPE '\\' ");
	    }
	  }
	  else if (j == DATA_ANNOTATION_USERNAME) {
	    splitbycharwesc(Sentries->entries[i+2].val, st1, st2, '@' , '\\');
	    sprintf(qval[DATA_ANNOTATION_USERNAME], " %s '%s'", Sentries->entries[i+1].val, st1);
	    sprintf(qval[DATA_ANNOTATION_USERDOMAIN], " %s '%s'", Sentries->entries[i+1].val, st2);
	    if (strstr(Sentries->entries[i+1].val,"like") != NULL) {
	      strcat(qval[DATA_ANNOTATION_USERNAME]," ESCAPE '\\' ");
	      strcat(qval[DATA_ANNOTATION_USERDOMAIN]," ESCAPE '\\' ");
	    }
	  }
	  else if (j < MAX_DCS_NUM ){
	    sprintf(qval[j]," %s '%s'", Sentries->entries[i+1].val, Sentries->entries[i+2].val); 
	    if (strstr(Sentries->entries[i+1].val,"like") != NULL) 
	      strcat(qval[j]," ESCAPE '\\' ");
	  }
	}
      }
      i = i+2; 
    }
    


    
      /** added for more
    if (m > 1) {
      for ( ; i < m; i = i+3) {
	if (strlen(entries[i+2].val) == 0)
	  continue;
	j = atoi(entries[i].val);
	if (j == DATA_OWNER) {
	  if (!strcmp(entries[i+1].val,"like") || 
	      !strcmp(entries[i+1].val,"not like")) {
	    make_like_for_mdasmysrb(entries[i+2].val);
	    splitbycharwesc(entries[i+2].val, st1, st2, '@' , '\\');
	    sprintf(qval[DATA_OWNER], " %s '%s'  ESCAPE '\\' ",
		    entries[i+1].val, st1);
	    sprintf(qval[DATA_OWNER_DOMAIN], " %s '%s'  ESCAPE '\\' ",
		    entries[i+1].val, st2);
	  }
	  else {
	    splitbycharwesc(entries[i+2].val, st1, st2, '@' , '\\');
	    sprintf(qval[DATA_OWNER], " %s '%s'",
		    entries[i+1].val, st1);
	    sprintf(qval[DATA_OWNER_DOMAIN], " %s '%s'",
		    entries[i+1].val, st2);
	  }
	}
	else if (j == DATA_ANNOTATION_USERNAME) {
	  if (!strcmp(entries[i+1].val,"like") || 
	      !strcmp(entries[i+1].val,"not like")) {
	    make_like_for_mdasmysrb(entries[i+2].val);
	    splitbycharwesc(entries[i+2].val, st1, st2, '@' , '\\');
	    sprintf(qval[DATA_ANNOTATION_USERNAME], " %s '%s'  ESCAPE '\\' ",
		    entries[i+1].val, st1);
	    sprintf(qval[DATA_ANNOTATION_USERDOMAIN], " %s '%s'  ESCAPE '\\' ",
		    entries[i+1].val, st2);
	  }
	  else {
	    splitbycharwesc(entries[i+2].val, st1, st2, '@' , '\\');
	    sprintf(qval[DATA_ANNOTATION_USERNAME], " %s '%s'",
		    entries[i+1].val, st1);
	    sprintf(qval[DATA_ANNOTATION_USERDOMAIN], " %s '%s'",
		    entries[i+1].val, st2);
	  }
	}
	else if (j < MAX_DCS_NUM ){
	  if (!strcmp(entries[i+1].val,"like") || 
	      !strcmp(entries[i+1].val,"not like")) {
	    make_like_for_mdasmysrb(entries[i+2].val);
	    sprintf(qval[j]," %s '%s'  ESCAPE '\\' ",
		    entries[i+1].val, entries[i+2].val);
	  }
	  else
	    sprintf(qval[j]," %s '%s'", 
		    entries[i+1].val, entries[i+2].val); 
	}
      }
    }
    * added for more **/


    if (collOnly == 1) {
        selval[DATA_GRP_NAME] = 1;
        selval[COLL_OWNER_NAME] = 1;
        selval[COLL_OWNER_DOMAIN] = 1;
        selval[COLL_CREATE_TIMESTAMP] = 1;
        selval[COLL_COMMENTS] = 1;
    }
    else {
        selval[DATA_GRP_NAME] = 1;
        selval[DATA_NAME] = 1;
        selval[SIZE] = 1;
        selval[DATA_TYP_NAME] = 1;
        selval[DATA_OWNER] = 1;
        selval[DATA_OWNER_DOMAIN] = 1;
        selval[REPL_TIMESTAMP] = 1;
        selval[DATA_REPL_ENUM] = 1;
        selval[DATA_VER_NUM] = 1;
        selval[PHY_RSRC_NAME] = 1;
        selval[CONTAINER_NAME] = 1;
#ifdef DATACOMMENTS
        selval[DATA_COMMENTS] = 14;
#endif
        selval[PATH_NAME] = 1;
    }

    /****
    for (i = 0; i < MAX_DCS_NUM; i++) {
        if (selval[i] > 0)
	  printf("%i:   SValue:%i<BR>\n",i,selval[i]);
        if (strcmp(qval[i],""))
	  printf("%i:   QValue:%s<BR>\n",i,qval[i]);
    }
	return(0);
    ***/
    Sentries->queryFunction = 1;

    /********  search from selected collection recursively  ***********/
    sprintf(qval[DATA_GRP_NAME], " like '%s%%'  ESCAPE '\\' ", Sentries->entries[Sentries->m].val);

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);
    /*printf("Status:%i\n",status); fflush(stdout);*/
    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	else
	  printf("No Answer found for the Query\n");
	bRMap(1,0);
	return;
    }
    else {
      filterDeleted (&collResult);
      Sentries->first = 1; Sentries->rowCntr = 1; printf("<TABLE CELLSPACING=0 CELLPADDING=0 RULES=ALL>\n");

    if (collOnly == 1) {
	    prtCollWeb_query(Sentries, &collResult);   
    }
    else {
        prtDataLongWeb (Sentries, &collResult, CHK_COLL);
    }
    
    fflush(stdout);
    
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	
    if (collOnly == 1) {
	    prtCollWeb_query(Sentries, &collResult);   
    }
    else {
        prtDataLongWeb (Sentries, &collResult, CHK_COLL);
    }
    
    fflush(stdout);
	
      }
    }
    if (filesfd != NULL && filesfd != stdout) {
      fflush(filesfd);
      fseek(filesfd, 0 , SEEK_SET);
      while ((j = fread (buf,1,60000,filesfd)) > 0) {
	buf[j] = '\0';
	printf("%s",buf);
      }
    }
    fprintf(stdout,"</TABLE>\n");
    clearSqlResult (&collResult);

    bRMap(1,0);
    return;
}

int
queryMetaValues(inStruct Sentries, int m)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;
    char st1[MAX_OBJ_NAME_LENGTH],st2[MAX_OBJ_NAME_LENGTH];
    int attrId;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    for (i =  1; i < m ; i =  i+3) {
      /*printf("%i:%i:%s:%s:%s<br>\n",m,i,entries[i].val,entries[i+1].val,entries[i+2].val);*/
      if (!strcmp(Sentries->entries[i].val,"Data Name")) 
	attrId =DATA_NAME;
      else if (!strcmp(Sentries->entries[i].val,"Data Replica")) 
	attrId =DATA_REPL_ENUM;
      else if (!strcmp(Sentries->entries[i].val,"Data Version")) 
	attrId =DATA_VER_NUM;
      else if (!strcmp(Sentries->entries[i].val,"Collection")) 
	attrId =COLLECTION_NAME;
      else if (!strcmp(Sentries->entries[i].val,"Data Type")) 
	attrId =DATA_TYP_NAME;
      else if (!strcmp(Sentries->entries[i].val,"Data Size")) 
	attrId =SIZE;
      else if (!strcmp(Sentries->entries[i].val,"Data Comments")) 
	attrId =DATA_COMMENTS;
      else if (!strcmp(Sentries->entries[i].val,"Creation Date")) 
	attrId =DATA_CREATE_TIMESTAMP;
      else if (!strcmp(Sentries->entries[i].val,"Last Modify Date")) 
	attrId =REPL_TIMESTAMP;
      else if (!strcmp(Sentries->entries[i].val,"Owner (Name@Dom)")) 
	attrId =DATA_OWNER;
      else if (!strcmp(Sentries->entries[i].val,"Resource Name")) 
	attrId =RSRC_NAME;
      else if (!strcmp(Sentries->entries[i].val,"Resource Class")) 
	attrId =RSRC_CLASS;
      else if (!strcmp(Sentries->entries[i].val,"Resource Type")) 
	attrId =RSRC_TYP_NAME;
      else if (!strcmp(Sentries->entries[i].val,"Rsrc Max Latency")) 
	attrId =RESOURCE_MAX_LATENCY;
      else if (!strcmp(Sentries->entries[i].val,"Rsrc Bandwidth")) 
	attrId =RESOURCE_BANDWIDTH;
      else if (!strcmp(Sentries->entries[i].val,"Rsrc NumOfHierarchy")) 
	attrId =RESOURCE_NUM_OF_HIERARCHIES;
      else if (!strcmp(Sentries->entries[i].val,"Rsrc Capacity")) 
	attrId =RESOURCE_CAPACITY;
      else if (!strcmp(Sentries->entries[i].val,"Container Name")) 
	attrId =CONTAINER_NAME;
      else if (!strcmp(Sentries->entries[i].val,"User (Name@Dom)")) 
	attrId =USER_NAME;
      else if (!strcmp(Sentries->entries[i].val,"Annotation")) 
	attrId =DATA_ANNOTATION;
      else if (!strcmp(Sentries->entries[i].val,"Annotator (Name@Dom)")) 
	attrId =DATA_ANNOTATION_USERNAME;
      else if (!strcmp(Sentries->entries[i].val,"Anno Position")) 
	attrId =DATA_ANNOTATION_POSITION;
      else if (!strcmp(Sentries->entries[i].val,"Anno TimeStamp")) 
	attrId =DATA_ANNOTATION_TIMESTAMP;
      else 
	attrId = UDSMD0;

      
      if (attrId != UDSMD0) {
      if (strlen(Sentries->entries[i+2].val) == 0)
	continue;
      }
      if (attrId == UDSMD0) {
	if (strstr(Sentries->entries[i].val,"*") || strstr(Sentries->entries[i].val,"?")) {
	  make_like_for_mdasmysrb(Sentries->entries[i].val);
	  sprintf(qval[UDSMD0]," like '%s'  ESCAPE '\\' ",Sentries->entries[i].val);
	}
	else {
	  if (strlen(Sentries->entries[i].val) > 0)
	    sprintf(qval[UDSMD0]," = '%s'",Sentries->entries[i].val); 
	}
	if (!strcmp(Sentries->entries[i+1].val,"like") || 
	    !strcmp(Sentries->entries[i+1].val,"not like")) {
	  make_like_for_mdasmysrb(Sentries->entries[i+2].val);
	  sprintf(qval[UDSMD1]," %s '%s'  ESCAPE '\\' ",
		  Sentries->entries[i+1].val, Sentries->entries[i+2].val);
	}
	else {
	  if (strlen(Sentries->entries[i+2].val) > 0)
	    sprintf(qval[UDSMD1]," %s '%s'", 
		    Sentries->entries[i+1].val,Sentries->entries[i+2].val); 
	}
      }
      else if (attrId == DATA_OWNER) {
	if (!strcmp(Sentries->entries[i+1].val,"like") || 
	    !strcmp(Sentries->entries[i+1].val,"not like")) {
	  make_like_for_mdasmysrb(Sentries->entries[i+2].val);
	  splitbycharwesc(Sentries->entries[i+2].val, st1, st2, '@' , '\\');
	  sprintf(qval[DATA_OWNER], " %s '%s'  ESCAPE '\\' ",
		  Sentries->entries[i+1].val, st1);
	  sprintf(qval[DATA_OWNER_DOMAIN], " %s '%s'  ESCAPE '\\' ",
		  Sentries->entries[i+1].val, st2);
	}
	else {
	  splitbycharwesc(Sentries->entries[i+2].val, st1, st2, '@' , '\\');
	  sprintf(qval[DATA_OWNER], " %s '%s'",
		  Sentries->entries[i+1].val, st1);
	  sprintf(qval[DATA_OWNER_DOMAIN], " %s '%s'",
		  Sentries->entries[i+1].val, st2);
	}
      }
      else if (attrId == USER_NAME) {
	if (!strcmp(Sentries->entries[i+1].val,"like") || 
	    !strcmp(Sentries->entries[i+1].val,"not like")) {
	  make_like_for_mdasmysrb(Sentries->entries[i+2].val);
	  splitbycharwesc(Sentries->entries[i+2].val, st1, st2, '@' , '\\');
	  sprintf(qval[USER_NAME], " %s '%s'  ESCAPE '\\' ",
		  Sentries->entries[i+1].val, st1);
	  sprintf(qval[DOMAIN_DESC], " %s '%s'  ESCAPE '\\' ",
		  Sentries->entries[i+1].val, st2);
	}
	else {
	  splitbycharwesc(Sentries->entries[i+2].val, st1, st2, '@' , '\\');
	  sprintf(qval[USER_NAME], " %s '%s'",
		  Sentries->entries[i+1].val, st1);
	  sprintf(qval[DOMAIN_DESC], " %s '%s'",
		  Sentries->entries[i+1].val, st2);
	}
      }
      else if (attrId == DATA_ANNOTATION_USERNAME) {
	if (!strcmp(Sentries->entries[i+1].val,"like") || 
	    !strcmp(Sentries->entries[i+1].val,"not like")) {
	  make_like_for_mdasmysrb(Sentries->entries[i+2].val);
	  splitbycharwesc(Sentries->entries[i+2].val, st1, st2, '@' , '\\');
	  sprintf(qval[DATA_ANNOTATION_USERNAME], " %s '%s'  ESCAPE '\\' ",
		  Sentries->entries[i+1].val, st1);
	  sprintf(qval[DATA_ANNOTATION_USERDOMAIN], " %s '%s'  ESCAPE '\\' ",
		  Sentries->entries[i+1].val, st2);
	}
	else {
	  splitbycharwesc(Sentries->entries[i+2].val, st1, st2, '@' , '\\');
	  sprintf(qval[DATA_ANNOTATION_USERNAME], " %s '%s'",
		  Sentries->entries[i+1].val, st1);
	  sprintf(qval[DATA_ANNOTATION_USERDOMAIN], " %s '%s'",
		  Sentries->entries[i+1].val, st2);
	}
      }
      else {
	if (!strcmp(Sentries->entries[i+1].val,"like") || 
	    !strcmp(Sentries->entries[i+1].val,"not like")) {
	  make_like_for_mdasmysrb(Sentries->entries[i+2].val);
	  sprintf(qval[attrId]," %s '%s'  ESCAPE '\\' ",
		  Sentries->entries[i+1].val, Sentries->entries[i+2].val);
	}
	else
	  sprintf(qval[attrId]," %s '%s'", 
		   Sentries->entries[i+1].val, Sentries->entries[i+2].val); 
      }
    }
    selval[DATA_GRP_NAME] = 1;
    selval[DATA_NAME] = 1;
    selval[SIZE] = 1;
    selval[DATA_TYP_NAME] = 1;
    selval[DATA_OWNER] = 1;
    selval[DATA_OWNER_DOMAIN] = 1;
    selval[REPL_TIMESTAMP] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[DATA_VER_NUM] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[CONTAINER_NAME] = 1;
#ifdef DATACOMMENTS
    selval[DATA_COMMENTS] = 14;
#endif
    selval[PATH_NAME] = 1;
    selval[UDSMD5] = 1;
    /*    for (i = 0; i < MAX_DCS_NUM; i++) {
        if (strcmp(qval[i],""))
	  printf("%i:   Value:%s<BR>\n",i,qval[i]);
	  }*/
    queryFunction = 1;
    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);
    /*printf("Status:%i\n",status); fflush(stdout);*/
    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	else
	  printf("No Answer found for the Query\n");
	bRMap(1,0);
	return;
    }
    else {
      filterDeleted (&collResult);
      first = 1; Sentries->rowCntr = 1; printf("<TABLE CELLSPACING=0 CELLPADDING=0 RULES=ALL>\n");
      prtDataLongWeb (Sentries, &collResult, CHK_COLL);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtDataLongWeb (Sentries, &collResult, CHK_COLL);
      }
    }
    if (filesfd != NULL && filesfd != stdout) {
      fflush(filesfd);
      fseek(filesfd, 0 , SEEK_SET);
      while ((j = fread (buf,1,60000,filesfd)) > 0) {
	buf[j] = '\0';
	printf("%s",buf);
      }
    }
    fprintf(stdout,"</TABLE>\n");
    clearSqlResult (&collResult);

    bRMap(1,0);
    return;
}

int showUserContainerInfo(inStruct Sentries)
{
    mdasC_sql_result_struct queryResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM], orderby[100];
    int i, j, status;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    selval[CONTAINER_NAME] = 1;               orderby[0]=CONTAINER_NAME;
    selval[CONTAINER_MAX_SIZE] = 1;           orderby[1]=CONTAINER_MAX_SIZE;
    selval[CONTAINER_LOG_RSRC_NAME] = 1;   orderby[2]=CONTAINER_LOG_RSRC_NAME;
    selval[CONTAINER_RSRC_CLASS] = 1;      orderby[3]= CONTAINER_RSRC_CLASS;
    selval[CONTAINER_SIZE] = 1;               orderby[4]=CONTAINER_SIZE;
    selval[CONTAINER_RSRC_NAME] = 1;          orderby[5]=CONTAINER_RSRC_NAME;

    sprintf(qval[CONTAINER_NAME], " like '/container/%s.%s/%%' ", Sentries->srbUser, Sentries->userDomain);

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &queryResult,
			       MAX_RESULT_NUM);
    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadat Access<A href=\"srberror2.cgi?%i\">Error: %i</A> \n", status,status);
	else
	  printf("No Answer Found\n");
	return(status);
    }
    else {
      filterDeleted (&queryResult);
      prtTableHeader("container");
      prtMetaQueryResult(&queryResult, orderby);
      while (queryResult.continuation_index >= 0) {
	clearSqlResult (&queryResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				queryResult.continuation_index,
				&queryResult, MAX_RESULT_NUM);
	if (queryResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&queryResult);
	prtMetaQueryResult(&queryResult, orderby);
      }
      clearSqlResult (&queryResult);
      printf("</TABLE>\n");
    }
    return(0);

}

int 
showBasicResourceInfo(inStruct Sentries)
{
    mdasC_sql_result_struct queryResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM], orderby[100];
    int i, j, status;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    selval[RSRC_NAME] = 1;                    orderby[0]=RSRC_NAME;
    selval[PHY_RSRC_NAME] = 1;                orderby[1]=PHY_RSRC_NAME;
    selval[RSRC_ADDR_NETPREFIX] = 1;          orderby[2]=RSRC_ADDR_NETPREFIX;
    selval[PHY_RSRC_DEFAULT_PATH] = 1;        orderby[3]=PHY_RSRC_DEFAULT_PATH;
    selval[PHY_RSRC_TYP_NAME] = 1;            orderby[4]=PHY_RSRC_TYP_NAME;
    selval[RSRC_CLASS] = 1;                   orderby[5]=RSRC_CLASS;
    /**selval[RESOURCE_MAX_LATENCY]  = 1;     orderby[6]=RESOURCE_MAX_LATENCY;
    selval[RESOURCE_MIN_LATENCY]  = 1;        orderby[7]=RESOURCE_MIN_LATENCY;
    selval[RESOURCE_BANDWIDTH]  = 1;          orderby[8]=RESOURCE_BANDWIDTH;
    selval[RESOURCE_MAX_CONCURRENCY]  = 1;    orderby[9]=RESOURCE_MAX_CONCURRENCY;
    selval[RESOURCE_NUM_OF_HIERARCHIES]  = 1; orderby[10]=RESOURCE_NUM_OF_HIERARCHIES;
    selval[RESOURCE_NUM_OF_STRIPES]  = 1;     orderby[11]=RESOURCE_NUM_OF_STRIPES;
    selval[RESOURCE_CAPACITY]  = 1;           orderby[12]=RESOURCE_CAPACITY;
    selval[RESOURCE_DESCRIPTION]  = 1;        orderby[13]=RESOURCE_DESCRIPTION;
    **/
    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &queryResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadata Access<A href=\"srberror2.cgi?%i\">Error: %i</A> \n", status,status);
	else
	  printf("No Answer Found\n");
	return(status);
    }
    else {
      filterDeleted (&queryResult);
      prtTableHeader("basic resource");
      prtMetaQueryResult(&queryResult, orderby);
      while (queryResult.continuation_index >= 0) {
	clearSqlResult (&queryResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				queryResult.continuation_index,
				&queryResult, MAX_RESULT_NUM);
	if (queryResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&queryResult);
	prtMetaQueryResult(&queryResult, orderby);
      }
      clearSqlResult (&queryResult);
      printf("</TABLE>\n");
    }
    return(0);

}

int 
showBasicUserInfo(inStruct Sentries)
{
    mdasC_sql_result_struct queryResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, status,orderby[100];

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    selval[USER_NAME] = 1;           orderby[0]=USER_NAME;
    selval[DOMAIN_DESC] = 1;         orderby[1]=DOMAIN_DESC;
    selval[USER_ADDRESS] = 1;        orderby[2]=USER_ADDRESS;
    selval[USER_PHONE] = 1;          orderby[3]=USER_PHONE;
    selval[USER_EMAIL] = 1;          orderby[4]=USER_EMAIL;
    selval[USER_TYP_NAME] = 1;       orderby[5]=USER_TYP_NAME;
    sprintf(qval[USER_NAME], " = '%s' ",Sentries->srbUser);
    sprintf(qval[DOMAIN_DESC], " = '%s' ",Sentries->userDomain);

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &queryResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadat Access<A href=\"srberror2.cgi?%i\">Error: %i</A> \n", status,status);
	else
	  printf("No Answer Found\n");
	return(status);
    }
    else {
      filterDeleted (&queryResult);
      prtTableHeader("basic user");
      prtMetaQueryResult(&queryResult, orderby);
      while (queryResult.continuation_index >= 0) {
	clearSqlResult (&queryResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				queryResult.continuation_index,
				&queryResult, MAX_RESULT_NUM);
	if (queryResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&queryResult);
	prtMetaQueryResult(&queryResult, orderby);
      }
      clearSqlResult (&queryResult);
      printf("</TABLE>\n");
    }
    return(0);

}

int 
showBasicAllUserInfo(inStruct Sentries)
{
    mdasC_sql_result_struct queryResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM],orderby[100];
    int i, j, status;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    selval[USER_NAME] = 1;           orderby[0]=USER_NAME;
    selval[DOMAIN_DESC] = 1;         orderby[1]=DOMAIN_DESC;
    selval[USER_ADDRESS] = 1;        orderby[2]=USER_ADDRESS;
    selval[USER_PHONE] = 1;          orderby[3]=USER_PHONE;
    selval[USER_EMAIL] = 1;          orderby[4]=USER_EMAIL;
    selval[USER_TYP_NAME] = 1;       orderby[5]=USER_TYP_NAME;

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &queryResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadat Access<A href=\"srberror2.cgi?%i\">Error: %i</A> \n", status,status);
	else
	  printf("No Answer Found\n");
	return(status);
    }
    else {
      filterDeleted (&queryResult);
      prtTableHeader("basic user");
      prtMetaQueryResult(&queryResult, orderby);
      while (queryResult.continuation_index >= 0) {
	clearSqlResult (&queryResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				queryResult.continuation_index,
				&queryResult, MAX_RESULT_NUM);
	if (queryResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&queryResult);
	prtMetaQueryResult(&queryResult, orderby);
      }
      clearSqlResult (&queryResult);
      printf("</TABLE>\n");
    }
    return(0);

}


int
getTicketInfo(inStruct Sentries)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    char userName[MAX_TOKEN], domnName[MAX_TOKEN];
    int i, j;
    int status;
    char *ticketId;
    char * ticketCollName[MAX_TOKEN];

    ticketId = Sentries->entries[Sentries->m].val;
    clSplitbychar (Sentries->entries[1].val, userName, domnName, '@');
    printCommonTicketButtons(Sentries);fflush(stdout);
    fprintf(stdout,"<TABLE CELLSPACING=0 CELLPADDING=0>\n");      
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (ticketId[0] == 'D') {
      sprintf(qval[TICKET_D], " = '%s'", ticketId);
      /*
      sprintf(qval[TICKET_USER_D], " = '%s'",  userName);
      sprintf(qval[TICKET_USER_DOMAIN_D], " = '%s'", domnName);
      */
      sprintf(qval[TICKET_USER_D], " in ( '%s', '%s')",  srbUser, TICKET_USER);
      sprintf(qval[TICKET_USER_DOMAIN_D], " in ( '%s', '%s')", TICKET_USER_DOMAIN, "");  
      
      selval[DATA_GRP_NAME] = 1;
      selval[DATA_NAME] = 1;
      selval[SIZE] = 1;
      selval[DATA_TYP_NAME] = 1;
      selval[DATA_OWNER] = 1;
      selval[DATA_OWNER_DOMAIN] = 1;
      selval[REPL_TIMESTAMP] = 1;
      selval[DATA_VER_NUM] = 1;
      selval[DATA_REPL_ENUM] = 1;
      selval[PHY_RSRC_NAME] = 1;
      selval[CONTAINER_NAME] = 1;
#ifdef DATACOMMENTS
      selval[DATA_COMMENTS] = 14;
#endif
      selval[PATH_NAME] = 1;
   
       status = srbGetDataDirInfoShunt(Sentries, Sentries->conn, 0, qval, selval, &collResult, MAX_RESULT_NUM);

      if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Error in Listing Ticket Datasets: <A href=\"srberror2.cgi?%i\">Error: %i</A> \n", status, status);
	else
	  printf("No Ticket Information Found\n");
	bRMap(1,0);
	fflush(stdout);
      }
      else {
	filterDeleted (&collResult);
	Sentries->first = 1; Sentries->rowCntr = 1;
	printf("<TABLE CELLSPACING=0 CELLPADDING=0 RULES=ALL>\n");
	fflush(stdout);
	/*	prtTicketDataLongWeb (&collResult, CHK_COLL); */

	prtDataLongWeb (Sentries, &collResult, CHK_COLL);

	fflush(stdout);
	while (collResult.continuation_index >= 0) {
	  clearSqlResult (&collResult);
	  status = srbGetMoreRows(Sentries->conn, 0,
				  collResult.continuation_index,
				  &collResult, MAX_RESULT_NUM);
	  if (collResult.result_count == 0  || status != 0)
	    break;
	  filterDeleted (&collResult);
	  /*prtTicketDataLongWeb (&collResult, CHK_COLL);*/
	  prtDataLongWeb (Sentries, &collResult, CHK_COLL);
	}
	clearSqlResult (&collResult);
      }
    }
    else  {    
      if (isTicketRecursive(Sentries, ticketId, userName, domnName, ticketCollName)) {

	/**** RTRTRT mar7,05 raja 
        sprintf(qval[TICKET_C], " = '%s'", ticketId);
	sprintf(qval[TICKET_USER_C], " = '%s'",  userName);
	sprintf(qval[TICKET_USER_DOMAIN_C], " = '%s'", domnName);

	selval[C_TICKET_COLL_NAME] = 1;
	selval[COLL_OWNER_NAME] = 1;
	selval[C_TICKET_COLL_CREATE_TIMESTAMP] = 1;
	selval[C_TICKET_COLL_COMMENTS] = 1;
	selval[COLL_OWNER_DOMAIN] = 1;
	selval[C_TICKET_COLL_NAME] = 1;
	****/
	/*	selval[COLLECTION_LINK_NUM ] = 1; */
	status = querySubCollInfo(Sentries, Sentries->conn, 0, ticketCollName, MAX_RESULT_NUM, &collResult);
	/*	RTRTRT mar7,05 raja status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult, MAX_RESULT_NUM);*/
	if (status < 0) {
	  if (status != MCAT_INQUIRE_ERROR)       /* no answer */
	    printf( "Error listing collections for ticket: %s, status = %d\n",
		    ticketId, status);
	  else
	    printf("N0 Ticket Information Found\n");
	  bRMap(1,0);
	}

	Sentries->rowCntr = 1; Sentries->first = 1; 
      /*      prtTicketCollWeb (&collResult);      fflush(stdout); */
	prtCollWeb (Sentries,&collResult);   fflush(stdout);
	while (collResult.continuation_index >= 0) {
	  clearSqlResult (&collResult);
	  status = srbGetMoreRows(Sentries->conn, 0,collResult.continuation_index,
				  &collResult, MAX_RESULT_NUM);
	  if (collResult.result_count == 0  || status != 0)
	    return;
	  filterDeleted (&collResult);
	  
	  /* Print the sub-collection */
	  /*      prtTicketCollWeb (&collResult);      fflush(stdout); */
	  prtCollWeb (Sentries, &collResult);   fflush(stdout);
	  
	}
	clearSqlResult (&collResult);
      }
      /* RTRTRT      else { */
	sprintf(qval[TICKET_C], " = '%s'", ticketId);
	sprintf(qval[TICKET_USER_C], " = '%s'",  userName);
	sprintf(qval[TICKET_USER_DOMAIN_C], " = '%s'", domnName);
	selval[DATA_GRP_NAME] = 1;
	selval[DATA_NAME] = 1;
	selval[SIZE] = 1;
	selval[DATA_TYP_NAME] = 1;
	selval[DATA_OWNER] = 1;
	selval[DATA_OWNER_DOMAIN] = 1;
	selval[REPL_TIMESTAMP] = 1;
	selval[DATA_VER_NUM] = 1;
	selval[DATA_REPL_ENUM] = 1;
	selval[PHY_RSRC_NAME] = 1;
	selval[CONTAINER_NAME] = 1;
#ifdef DATACOMMENTS
	selval[DATA_COMMENTS] = 14;
#endif
	selval[PATH_NAME] = 1;
	
	status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,MAX_RESULT_NUM);
	if (status < 0)     {
	  if (status != MCAT_INQUIRE_ERROR)
	    printf("Error in Listing Ticket Datasets: <A href=\"srberror2.cgi?%i\">Error: %i</A> \n", status, status);
	  else
	    printf("No Ticket Information Found\n");
	  bRMap(1,0);
	  fflush(stdout);
	}
	else {
	  filterDeleted (&collResult);
	  Sentries->first = 1; Sentries->rowCntr = 1;printf("<TABLE CELLSPACING=0 CELLPADDING=0 RULES=ALL>\n");
	  /*	prtTicketDataLongWeb (&collResult, CHK_COLL); */
	  prtDataLongWeb (Sentries, &collResult, CHK_COLL);
	  fflush(stdout);
	  while (collResult.continuation_index >= 0) {
	    clearSqlResult (&collResult);
	    status = srbGetMoreRows(Sentries->conn, 0,
				    collResult.continuation_index,
				    &collResult, MAX_RESULT_NUM);
	    if (collResult.result_count == 0  || status != 0)
	      break;
	    filterDeleted (&collResult);
	    /*prtTicketDataLongWeb (&collResult, CHK_COLL);*/
	    prtDataLongWeb (Sentries, &collResult, CHK_COLL);
	  }
	  clearSqlResult (&collResult);
	}
      }      
    /* RTRTRT    } */
      if (filesfd != NULL && filesfd != stdout) {
	fflush(filesfd);
	fseek(filesfd, 0 , SEEK_SET);
	while ((j = fread (buf,1,60000,filesfd)) > 0) {
	  buf[j] = '\0';
	  printf("%s",buf);
	}
      }
    fprintf(stdout,"</TABLE><P>\n");
    printCommonTicketButtons(Sentries);   
}


int
getTicketCollectionInfo(inStruct Sentries, char *collection, char *dataName)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;
    char urlcollection[MAX_TOKEN];
    char parurlcollection[MAX_TOKEN];
    char tmpStr[MAX_TOKEN];
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    if (strlen(dataName) > 0) {
      if (strstr(dataName,"^")) {
         make_like_for_mdasmysrb(dataName);
         client_set_qval_data_cond(dataName,qval);
      }
      else {
         make_like_for_mdasmysrb(dataName);
         sprintf(qval[DATA_NAME], " like '%s'  ESCAPE '\\' ",dataName);
      }
    }
    selval[DATA_GRP_NAME] = 1;
     selval[DATA_NAME] = 1;
    selval[SIZE] = 1;
    selval[DATA_TYP_NAME] = 1;
    selval[DATA_OWNER] = 1;
    selval[DATA_OWNER_DOMAIN] = 1;
    selval[REPL_TIMESTAMP] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[DATA_VER_NUM] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[CONTAINER_NAME] = 1;
#ifdef DATACOMMENTS
    selval[DATA_COMMENTS] = 14;
#endif
    selval[PATH_NAME] = 1;

    strcpy(urlcollection,collection);
    escape_url(urlcollection);
    clSplitbychar (urlcollection, parurlcollection, tmpStr, '/');
    
    printCommonTicketButtons(Sentries /*urlcollection, parurlcollection*/);

     printf("<FONT COLOR=#FF0000>%s</FONT>\n",collection);
    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);

      fprintf(stdout,"<TABLE CELLSPACING=0 CELLPADDING=0>\n");      fflush(stdout);
    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Collection Access<A href=\"srberror2.cgi?%i\">Error: %i</A> \n", status, status);
    }
    else {
      filterDeleted (&collResult);
      first = 1;printf("<TABLE CELLSPACING=0 CELLPADDING=0 RULES=ALL>\n");
      if (likeCollFlag) {
	/*	prtTicketDataLongWeb (&collResult, CHK_COLL); */
	prtDataLongWeb (Sentries, &collResult, CHK_COLL);
      }
      else {
	/*	prtTicketDataLongWeb (&collResult, CHK_COLL); */
	prtDataLongWeb (Sentries, &collResult, CHK_COLL);
      }
      fflush(stdout);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	if (likeCollFlag) {
	  /*	prtTicketDataLongWeb (&collResult, CHK_COLL); */
	  prtDataLongWeb (Sentries, &collResult, CHK_COLL);
	}
	else {
	  /*	prtTicketDataLongWeb (&collResult, CHK_COLL); */
	  prtDataLongWeb (Sentries, &collResult, CHK_COLL);
	}
      }

      clearSqlResult (&collResult);
    }
    if (filesfd != NULL && filesfd != stdout) {
      fflush(filesfd);
      fseek(filesfd, 0 , SEEK_SET);
      while ((j = fread (buf,1,60000,filesfd)) > 0) {
	buf[j] = '\0';
	printf("%s",buf);
      }
    }

    first = 1; Sentries->rowCntr = 1;
    status = querySubCollInfo(Sentries, Sentries->conn, 0, collection, MAX_RESULT_NUM, &collResult);
    if (status ==  0) {
      /*      prtTicketCollWeb (&collResult);      fflush(stdout); */
      prtCollWeb (Sentries, &collResult);   fflush(stdout);
      while (collResult.continuation_index >= 0) {
        clearSqlResult (&collResult);
        status = srbGetMoreRows(Sentries->conn, 0,collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
        if (collResult.result_count == 0  || status != 0)
	  return;
	filterDeleted (&collResult);
	
        /* Print the sub-collection */
	/*    prtTicketCollWeb (&collResult);      fflush(stdout); */
	prtCollWeb (Sentries, &collResult);   fflush(stdout);
	
      }
    }
    fprintf(stdout,"</TABLE><P>\n");
    printCommonTicketButtons(Sentries/*urlcollection, parurlcollection*/);

}

int
getSlinkCollectionInfo(inStruct Sentries)
{
    mdasC_sql_result_struct dataResult, collResult;
    char parData[MAX_TOKEN],parPath[MAX_TOKEN * 4];
    char netPrefix[MAX_TOKEN];
    char defPath[MAX_TOKEN * 4];
    char srbDataName[MAX_TOKEN * 4];
    char cmdStr[10000], inStr[11000], dirBuffer[10000];
    char *ownerName, *dataSize, *creatDate, *dataName;
    char *inName, *inColl, *inPath, *inResource, *copyNum;
    char *tmpPtr, *cmdPtr, *cmdPtr2;
    int i, j, k;


    inName = Sentries->entries[1].val;
    inColl = Sentries->entries[2].val;
    inResource = Sentries->entries[3].val;
    inPath = Sentries->entries[5].val;
    copyNum = Sentries->entries[4].val;
    escape_url(inColl);
    sprintf(defPath," -l %s", Sentries->entries[5].val);

 if ((tmpPtr= strstr(defPath,"/?SHADOW")) != NULL) {
   if (*(tmpPtr + 8) == '\0') {
     *tmpPtr ='\0';
     parData[0] ='\0';
     parPath[0] ='\0';
   }
   else {
     strcpy(tmpPtr+ 1,tmpPtr + 8);
     strcpy(parData,inName);
     strcpy(parPath,inPath);
     tmpPtr = strstr(parPath,"/?SHADOW");
     tmpPtr += 8;
     for (i = (strlen(tmpPtr) -1); i > 0; i--) {
       if ((char ) *(tmpPtr + i) == '/')
	 break;
     }
     tmpPtr[i] = '\0';
   }
   i = getNetprefixForResource(Sentries, inResource, netPrefix);
      if (i < 0) {
    	printf("Host Name Error for %s <A href=\"srberror2.cgi?%i\">Error: %i</A> \n",inResource, i,i);
    	return;
      }

      strcpy(cmdStr,"ls");

      i = srbExecCommand (Sentries->conn, cmdStr, defPath, netPrefix, "",
                            PORTAL_STD_IN_OUT);

      if (i < 0) {
    	printf("Link Access<A href=\"srberror2.cgi?%i\">Error: %i</A> \n", i,i);
    	return;
      }
        
      /**
      printf("<PRE>\n LS DATA\n");
      while ((j = read (i, cmdStr, 6000)) > 0) {
	cmdStr[j] = '\0';
	printf("%s",cmdStr);
      }
      printf("</PRE>\n");
     return;
      **/

      /*  drwxr-sr-x   2 sekar    sys200       512 Aug 30 12:53 ASCI2000	   */
      /*  0123456789012345678901234567890123456789012345678901234567890123456789 */
      if (Sentries->isTicket)
	    printShadowTicketButtons(Sentries, parData, parPath);
      else
	    printShadowButtons(Sentries, parData, parPath);

      fprintf(stdout,"<FONT COLOR=#0000FF>Shadow Directory: <FONT COLOR=#FF0000>%s</FONT> attached at   %s/%s</FONT>\n",(char *) (defPath + 3),inColl,inName);
      fprintf(stdout,"<TABLE CELLSPACING=0 CELLPADDING=0 RULES=ALL>\n");
      first = 1;  firstd = 1; Sentries->rowCntr = 0; oddFlag = 0;
      dirBuffer[0] ='\0';
      cmdStr[0] ='0';
  while ((j = read (i, inStr, sizeof(inStr) - 1)) > 0) {
	inStr[j] = '\0';
	strcat(cmdStr,inStr);
	cmdPtr = cmdStr;
    while(strlen(cmdPtr) > 0) {
      Sentries->rowCntr++;
      if ((cmdPtr2 = strstr(cmdPtr,"\n")) == NULL) {
    	strcpy(cmdStr,cmdPtr);
    	cmdPtr = (char *)(cmdStr + strlen(cmdStr));
    	continue;
      }
      *cmdPtr2 = '\0';
      if (*cmdPtr != '-')  {
    	if (*cmdPtr == 'd') {
    	  strcat(dirBuffer,cmdPtr);
    	  strcat(dirBuffer,"\n");
    	}
    	cmdPtr = cmdPtr2 + 1;
    	continue;
      }

    k = 0;
    
    /* get owner name */
    k += nextToken(cmdPtr);
    k += nextToken(&cmdPtr[k]);
    ownerName = &cmdPtr[k];
    k += nextSeparator(&cmdPtr[k]);
    cmdPtr[k]='\0';
    
    /* get data size */
    k += nextToken(&cmdPtr[k]);
    k += nextToken(&cmdPtr[k]);
    dataSize = &cmdPtr[k];
    k += nextSeparator(&cmdPtr[k]);
    cmdPtr[k]='\0';
    
    /* get creation time */
    k += nextToken(&cmdPtr[k]);
    creatDate = &cmdPtr[k];
    k += nextToken(&cmdPtr[k]);
    k += nextToken(&cmdPtr[k]);
    k += nextSeparator(&cmdPtr[k]);
    cmdPtr[k]='\0';
    
    /* get data name */            
    k += nextToken(&cmdPtr[k]);       
    dataName = &cmdPtr[k];    

	/*printf("##%s::%s:%s:%s:%s@@<BR>\n",cmdPtr,ownerName,dataSize,creatDate,dataName);fflush(stdout);*/
	  
	  tmpPtr = strstr(inPath,"/?SHADOW");
	  tmpPtr += 8;
	  if (strlen(tmpPtr) == 0) {
	    sprintf(srbDataName, "%s@SHADOW=%s",inName,dataName);
	    sprintf(defPath,"%s%s",inPath,dataName);
	  }
	  else {
	    sprintf(srbDataName, "%s@SHADOW=%s/%s",inName,tmpPtr,dataName);
	    sprintf(defPath,"%s/%s",inPath,dataName);
	  }
	  escape_url(srbDataName);
	  escape_url(defPath);
	  /*printf("**%s::%s:%s:%s:%s!!<BR>\n",cmdPtr,ownerName,dataSize,creatDate,srbDataName);fflush(stdout);*/
	  if (first) {
	    fprintf (stdout,"\n<TR BGCOLOR=\"#DDEFFF\"><TH>Function</TH><TH>Data Name</TH><TH>Creation Time</TH><TH>Owner</TH>\
<TH>Size</TH><TH>Resource</TH></TR>\n");
	    first = 0;
	  }
	  oddFlag = (oddFlag + 1) % 2;
	  if (oddFlag) {
	    fprintf (stdout,"<TR bgcolor=\"#CCBB88\">");
	  }
	  else {
	    fprintf (stdout,"<TR bgcolor=\"#CCCCCC\">");
	  }

	  if (Sentries->isTicket) {
	    fprintf (stdout,"<FORM name =\"selform%i\" METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" ><TD NOWRAP VALIGN=top><FONT face=\"verdana\" size=\"2\"> \n", Sentries->rowCntr, Sentries->frameStr);
	    fprintf(stdout,"<SELECT SIZE=1 NAME=function onChange=\"selform%i.submit()\">\
<OPTION SELECTED VALUE=\"ticketslinkfull\">Get Data\
<OPTION VALUE=\"showmetaNew\">%s\
<OPTION VALUE=\"showmetaE\">%s\
<OPTION VALUE=\"showanno\">Show Annotations\
</SELECT>\n", Sentries->rowCntr, mySrbStrings[85], mySrbStrings[86]);
	  }
	  else {
	    fprintf (stdout,"<FORM name =\"selform%i\" METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" ><TD NOWRAP VALIGN=top><FONT face=\"verdana\" size=\"2\"> \n", Sentries->rowCntr, Sentries->frameStr);
	    fprintf(stdout,"<SELECT SIZE=1 NAME=function onChange=\"selform%i.submit()\">\
<OPTION SELECTED VALUE=\"slinkfull\">Get Data\
<OPTION VALUE=\"slinkdownloadfile\">Download Data\
<OPTION VALUE=\"showmetaNew\">Show All Metadata\
<OPTION VALUE=\"showmetaE\">Show Entity Metadata\
<OPTION VALUE=\"showanno\">Show Annotations\
<OPTION VALUE=\"metainsert\">Insert MyMetadata\
<OPTION VALUE=\"dcinsert\">Insert DublinCore\
<OPTION VALUE=\"modifymetaform\">Modify Metadata\
<OPTION VALUE=\"slinksform\">SoftLink File\
</SELECT>\n", Sentries->rowCntr);fflush(stdout);
	  }
	  fprintf(stdout,"<INPUT NAME=\"dataname\" type=hidden VALUE=\"%s\">\n", srbDataName);
	  fprintf(stdout,"<INPUT NAME=\"collection\" type=hidden VALUE=\"%s\">\n", inColl);
	  fprintf(stdout,"<INPUT NAME=\"rsrcname\" type=hidden VALUE=\"%s\">\n", inResource);
	  fprintf(stdout,"<INPUT NAME=\"copynum\" type=hidden VALUE=\"%s\">\n", copyNum);
	  fprintf(stdout,"<INPUT NAME=\"datatype\" type=hidden VALUE=\"%s\">\n", defPath);
	  fprintf(stdout,"<INPUT NAME=\"vernum\" type=hidden VALUE=\"-1\">\n");
	  /*  entries[5].val);*/
	  fprintf(stdout,"<INPUT NAME=\"owner\" type=hidden VALUE=\"%s\">\n",ownerName);
	  fprintf(stdout,"<INPUT NAME=\"session\" type=hidden VALUE=\"%s\">\n",Sentries->pidStr);
	  fprintf(stdout,"</FONT></TD></FORM>\n");fflush(stdout);
	if (Sentries->isTicket) {
	  fprintf(stdout,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=ticketslinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=-1&owner=%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/docimg.gif\" HEIGHT=16 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=ticketslinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&owner=%s&vernum=-1&session=%s\"  target=\"%s\">%s </A>",srbDataName,inColl,inResource,copyNum, defPath,ownerName,Sentries->pidStr, Sentries->frameStr, webPath,
		  srbDataName,inColl,inResource, copyNum, defPath,ownerName,Sentries->pidStr, Sentries->frameStr,dataName);
	}
	else {
	  fprintf(stdout,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=slinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=-1&owner=%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/docimg.gif\" HEIGHT=16 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=slinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=-1&owner=%s&session=%s\"  target=\"%s\">%s </A>",srbDataName,inColl,inResource, copyNum, defPath,ownerName,Sentries->pidStr, Sentries->frameStr, webPath,
		  srbDataName,inColl,inResource,copyNum,  defPath,ownerName,Sentries->pidStr, Sentries->frameStr,dataName);fflush(stdout);
	}
	fprintf(stdout,"</TD><TD  nowrap align=center> %s </TD><TD nowrap align=center> %s </TD><TD> %s </TD><TD> %s </TD></TR>\n",creatDate,ownerName,dataSize,inResource);fflush(stdout);
	cmdPtr = cmdPtr2 + 1; 
    }
  }
  /*** directories ***/
  cmdPtr = dirBuffer;
    while(strlen(cmdPtr) > 0) {
      Sentries->rowCntr++;
      if ((cmdPtr2 = strstr(cmdPtr,"\n")) != NULL)
	*cmdPtr2 = '\0';

    k = 0;
    
    /* get owner name */
    k += nextToken(cmdPtr);
    k += nextToken(&cmdPtr[k]);
    ownerName = &cmdPtr[k];
    k += nextSeparator(&cmdPtr[k]);
    cmdPtr[k]='\0';
    
    /* get data size */
    k += nextToken(&cmdPtr[k]);
    k += nextToken(&cmdPtr[k]);
    dataSize = &cmdPtr[k];
    k += nextSeparator(&cmdPtr[k]);
    cmdPtr[k]='\0';
    
    /* get creation time */
    k += nextToken(&cmdPtr[k]);
    creatDate = &cmdPtr[k];
    k += nextToken(&cmdPtr[k]);
    k += nextToken(&cmdPtr[k]);
    k += nextSeparator(&cmdPtr[k]);
    cmdPtr[k]='\0';
    
    /* get data name */            
    k += nextToken(&cmdPtr[k]);       
    dataName = &cmdPtr[k];  	

	/*printf("##%s::%s:%s:%s:%s@@<BR>\n",cmdPtr,ownerName,dataSize,creatDate,dataName);fflush(stdout);*/
	  
	  tmpPtr = strstr(inPath,"/?SHADOW");
	  tmpPtr += 8;
	  if (strlen(tmpPtr) == 0) 
	    sprintf(defPath,"%s%s",inPath,dataName);
	  else 
	    sprintf(defPath,"%s/%s",inPath,dataName);
	  escape_url(inName);
	  escape_url(defPath);
	  /*printf("**%s::%s:%s:%s:%s!!<BR>\n",cmdPtr,ownerName,dataSize,creatDate,srbDataName);fflush(stdout);*/
	  if (firstd) {
	    fprintf (stdout,"<TR BGCOLOR=\"#DDEFFF\"><TH>Function</TH><TH>Sub Directory</TH><TH>Creation Time</TH><TH>Owner</TH>\
<TH>Size</TH><TH>Resource</TH></TR>\n");
	    firstd = 0;
	  }
	  oddFlag = (oddFlag + 1) % 2;
	  if (oddFlag) {
	    fprintf (stdout,"<TR bgcolor=\"#CCBB88\">");
	  }
	  else {
	    fprintf (stdout,"<TR bgcolor=\"#CCCCCC\">");
	  }

	  if (Sentries->isTicket) {
	    fprintf (stdout,"<FORM name =\"selform%i\" METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" ><TD NOWRAP VALIGN=top><FONT face=\"verdana\" size=\"2\"> \n", Sentries->rowCntr,Sentries->frameStr);
	    fprintf(stdout,"<SELECT SIZE=1 NAME=function onChange=\"selform%i.submit()\">\
<OPTION SELECTED VALUE=\"ticketslinkfull\">Get Data\
<OPTION VALUE=\"showmetaNew\">%s\
<OPTION VALUE=\"showmetaE\">Show Entity Metadata\
<OPTION VALUE=\"showanno\">Show Annotations\
</SELECT>\n", Sentries->rowCntr);
	  }
	  else {
	    fprintf (stdout,"<FORM name =\"selform%i\" METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" ><TD NOWRAP VALIGN=top><FONT face=\"verdana\" size=\"2\"> \n", Sentries->rowCntr,Sentries->frameStr);
	    fprintf(stdout,"<SELECT SIZE=1 NAME=function onChange=\"selform%i.submit()\">\
<OPTION SELECTED VALUE=\"slinkfull\">Open Collection\
<OPTION VALUE=\"showmetaNew\">Show All Metadata\
<OPTION VALUE=\"showmetaE\">Show Entity Metadata\
<OPTION VALUE=\"showmetaC\">Show Content Metadata\
<OPTION VALUE=\"showanno\">Show Annotations\
<OPTION VALUE=\"metainsert\">Insert MyMetadata\
<OPTION VALUE=\"modifymetaform\">Modify Metadata\
<OPTION VALUE=\"slinksform\">SoftLink File\
</SELECT>\n", Sentries->rowCntr);fflush(stdout);
	  }
	  fprintf(stdout,"<INPUT NAME=\"dataname\" type=hidden VALUE=\"%s\">\n",
		  inName);
	  fprintf(stdout,"<INPUT NAME=\"collection\" type=hidden VALUE=\"%s\">\n",
		  inColl);
	  fprintf(stdout,"<INPUT NAME=\"rsrcname\" type=hidden VALUE=\"%s\">\n",inResource);
	  fprintf(stdout,"<INPUT NAME=\"copynum\" type=hidden VALUE=\"%s\">\n",copyNum);
	  fprintf(stdout,"<INPUT NAME=\"datatype\" type=hidden VALUE=\"%s\">\n",defPath);
	  fprintf(stdout,"<INPUT NAME=\"vernum\" type=hidden VALUE=\"-2\">\n");
	  /*  entries[5].val);*/
	  fprintf(stdout,"<INPUT NAME=\"owner\" type=hidden VALUE=\"%s\">\n",ownerName);
	  fprintf(stdout,"<INPUT NAME=\"session\" type=hidden VALUE=\"%s\">\n",Sentries->pidStr);
	  fprintf(stdout,"</FONT></TD></FORM>\n");fflush(stdout);
	if (Sentries->isTicket) {
	  fprintf(stdout,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=ticketslinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=-2&owner=%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/collimg.gif\" HEIGHT=16 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=ticketslinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=-2&owner=%s&session=%s\"  target=\"%s\">%s </A>",inName,inColl,inResource, copyNum,defPath,ownerName,Sentries->pidStr, Sentries->frameStr, webPath,
		  inName,inColl,inResource,  copyNum,defPath,ownerName,Sentries->pidStr, Sentries->frameStr,dataName);
	}
	else {
	  fprintf(stdout,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=slinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=-2&owner=%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/collimg.gif\" HEIGHT=16 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=slinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=-2&owner=%s&session=%s\"  target=\"%s\">%s </A>",inName,inColl,inResource, copyNum, defPath,ownerName,Sentries->pidStr, Sentries->frameStr, webPath, 
		  inName,inColl,inResource,copyNum,  defPath,ownerName,Sentries->pidStr, Sentries->frameStr,dataName);fflush(stdout);
	}
	fprintf(stdout,"</TD><TD  nowrap align=center> %s </TD><TD nowrap align=center> %s </TD><TD> %s </TD><TD> %s </TD></TR>\n",creatDate,ownerName,dataSize,inResource);fflush(stdout);
	  /* fprintf(stdout,"<TR><TD COLSPAN=4>%s</TD></TR>\n",cmdPtr);fflush(stdout);*/
      cmdPtr = cmdPtr2 + 1; 
    }
 }
 else {
   printf(" Collection Path Not Properly Defined\n");
 }

 fprintf(stdout,"</TABLE><P>\n");
 if (Sentries->isTicket)
   printShadowTicketButtons(Sentries, parData, parPath);
 else
   printShadowButtons(Sentries, parData, parPath);
 
}

int prtNoDataLongWeb(inStruct Sentries, mdasC_sql_result_struct *myresult)
{
    int i, jj;
    char *dataList, *sizeList, *ownerList, *timeList, *replNumList, *rsrcName, *ownerDomnList;
    char *collList, *containerName, *dataComments, *dataTypeList, *verNumList, *pathNameList;
    char *udsmd5List;
    char *udsmdList[20];
    char tmpContainerName[MAX_TOKEN], printdName[4*MAX_TOKEN];
    char *inCont, *tmpPtr;
    char tmpCollName[MAX_TOKEN], tmpDataName[MAX_TOKEN], funcType[MAX_TOKEN];
    int ShadowOddFlag = 0;  /* oddFlag for shadow objects must be treated separately */
    
    
    Sentries->oddFlag = 0;
    jj = 0;

    for (jj =  0; jj < Sentries->showMDVCntr; jj++)  {
      udsmdList[jj] = (char *) getFromResultStruct(
	  (mdasC_sql_result_struct *) myresult,
	   dcs_tname[Sentries->showMDVId[jj]], dcs_aname[Sentries->showMDVId[jj]]);
      if (udsmdList[jj] == NULL)
	printf("jj=%i is NULL!! showMDVId[jj]= %i !!\n",jj, Sentries->showMDVId[jj]);
    }



    for (i = 0; i < myresult->row_count; i++) {
      if (Sentries->first) {
	fprintf (stdout,"\n<TR BGCOLOR=\"#DDEFFF\">");
	fprintf(stdout, "<TH></TH>\n");
	for (jj = 0; jj < Sentries->showMDVCntr; jj++)
	  fprintf (stdout,"<TH>%s</TH>\n",Sentries->showMDVTitle[jj]);
	Sentries->first = 0;
      }
      fflush(stdout);
      Sentries->oddFlag = (Sentries->oddFlag + 1) % 2;
      if (Sentries->oddFlag) {
	fprintf (stdout,"<TR bgcolor=\"#88BBFF\">");
      }
      else {
	fprintf (stdout,"<TR bgcolor=\"#DDEEFF\">");
      }

      fprintf(stdout, "<TD>%d</TD>", Sentries->rowCntr);
      
      for (jj = 0; jj < Sentries->showMDVCntr; jj++) {
	if (Sentries->showMDVId[jj] == DATA_ANNOTATION_USERNAME) {
	  fprintf (stdout,"<TD>%s@%s</TD>",udsmdList[jj],udsmdList[jj+1]);
	  udsmdList[jj] += MAX_DATA_SIZE; 	
	  udsmdList[jj+1] += MAX_DATA_SIZE; 	
	  jj++;
	}
	else {
	  fprintf (stdout,"<TD>%s</TD>",udsmdList[jj]);
	  udsmdList[jj] += MAX_DATA_SIZE; 	
	}
      }
      fprintf (stdout,"<TR>");
      Sentries->rowCntr++;
    }
    
}

int prtDataLongWeb(inStruct Sentries, mdasC_sql_result_struct *myresult, int chkColl)
{
    int i, jj;
    char *dataList, *sizeList, *ownerList, *timeList, *replNumList, *rsrcName, *ownerDomnList;
    char *collList, *containerName, *dataComments, *dataTypeList, *verNumList, *pathNameList;
    char *udsmd5List;
    char *udsmdList[20];
    char tmpContainerName[MAX_TOKEN], printdName[4*MAX_TOKEN];
    char *inCont, *tmpPtr;
    char tmpCollName[MAX_TOKEN], tmpDataName[MAX_TOKEN], funcType[MAX_TOKEN];
    int ShadowOddFlag = 0;  /* oddFlag for shadow objects must be treated separately */
    
    
    dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    dataTypeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
    sizeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[SIZE], dcs_aname[SIZE]);
    ownerList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_OWNER], dcs_aname[DATA_OWNER]);
    ownerDomnList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_OWNER_DOMAIN], dcs_aname[DATA_OWNER_DOMAIN]);

    timeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
    replNumList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);

    verNumList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);
    rsrcName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
    containerName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);
    pathNameList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);
#ifdef DATACOMMENTS
    dataComments = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_COMMENTS], dcs_aname[DATA_COMMENTS]);
#endif
    if (queryFunction)
      udsmd5List =  (char *) getFromResultStruct(
            (mdasC_sql_result_struct *) myresult,
	    dcs_tname[UDSMD5], dcs_aname[UDSMD5]);
    collList = (char *) getFromResultStruct(
	    (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    Sentries->oddFlag = 0;
    jj = 0;

    for (jj =  0; jj < Sentries->showMDVCntr; jj++)  {
      udsmdList[jj] = (char *) getFromResultStruct(
	  (mdasC_sql_result_struct *) myresult,
	   dcs_tname[Sentries->showMDVId[jj]], dcs_aname[Sentries->showMDVId[jj]]);
      if (udsmdList[jj] == NULL)
	printf("jj=%i is NULL!! showMDVId[jj]= %i !!\n",jj, Sentries->showMDVId[jj]);
    }



    for (i = 0; i < myresult->row_count; i++) {
      strcpy(Sentries->urlcollList,collList);
      strcpy(Sentries->urldataList,dataList);
      strcpy(Sentries->nonurlcollList,collList);
      strcpy(Sentries->nonurldataList,dataList);
      strcpy(Sentries->urlpathNameList,pathNameList);
      strcpy(Sentries->urldataTypeList,dataTypeList);
      strcpy(Sentries->urlrsrcName,rsrcName);
        if (atoi(sizeList) == -1)
	  strcpy(sizeList,"UNK");
      escape_url(Sentries->urlcollList);
      escape_url(Sentries->urldataList);
      escape_url(Sentries->urlpathNameList);
      escape_url(Sentries->urldataTypeList);
      escape_url(Sentries->urlrsrcName);
      if (chkColl == CHK_COLL ) 
	sprintf(printdName,"%s/%s",collList, dataList);
      else
	strcpy(printdName,dataList);


    if (strstr(dataTypeList,"directory shadow object")  != NULL) {
    if (Sentries->firsts) {
        filesfd = tmpfile();
        if (filesfd == NULL) 
            filesfd = stdout;
        else {
            fprintf (filesfd,"\n<TR BGCOLOR=\"#DDEFFF\">");
            fprintf (filesfd, "<TH></TH>\n");     /** for object count column **/
            fprintf (filesfd,"<TH>Function</TH><TH>Shadow ObjName</TH><TH>Creation Time</TH><TH>Owner</TH><TH> &nbsp; </TH><TH> &nbsp; </TH><TH> &nbsp; </TH><TH> &nbsp; </TH><TH>Resource</TH><TH> &nbsp; </TH></TR>\n");
        }
        Sentries->firsts = 0;
    }
    ShadowOddFlag = (ShadowOddFlag + 1) % 2;
    if (ShadowOddFlag) {
        /* fprintf (filesfd,"<TR bgcolor=\"#DDCC99\">");*/
        fprintf (filesfd,"<TR bgcolor=\"#88bbff\">");
    }
    else {
        /*fprintf (filesfd,"<TR bgcolor=\"#CCCCCC\">");*/
        fprintf (filesfd,"<TR bgcolor=\"#DDeeff\">");
    }
    strcpy(Sentries->imageName,"shadowobj");
    if (Sentries->queryFunction) {
        if (strlen(udsmd5List) > 0) {
            strcat(printdName,"->");
            strcat(printdName,&(udsmd5List[2]));
            if (udsmd5List[0] == 'F'){
                strcat(Sentries->urldataList,"@SHADOW=");
                strcat(Sentries->urldataList,&(udsmd5List[2]));
                strcat(Sentries->nonurldataList,"@SHADOW=");
                strcat(Sentries->nonurldataList,&(udsmd5List[2]));
                escape_url(Sentries->urldataList);
                strcpy(verNumList,"-1");
                strcpy(Sentries->imageName,"docimg");
            }
            else if (udsmd5List[0] == 'D'){
                strcat(pathNameList,&(udsmd5List[2]));
                escape_url(pathNameList);
                strcpy(verNumList,"-2");
                strcpy(Sentries->imageName,"collimg");
            }
        }
    }
    
    /*** object count  ***/
    fprintf(filesfd, "<TD>%d</TD>", Sentries->rowCntr);
    /****/
	    
	if (Sentries->isTicket) {
	  fprintf (filesfd,"<FORM name =\"selform%i\" METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" ><TD NOWRAP VALIGN=top><FONT face=\"verdana\" size=\"2\"> \n", Sentries->rowCntr,Sentries->frameStr);
	  fprintf(filesfd,"<SELECT SIZE=1 NAME=function onChange=\"mySubmit(selform%i, this)\">", Sentries->rowCntr);
	  fprintf(filesfd,"<OPTION SELECTED VALUE=\"doNothing\">%s", mySrbStrings[268]);
fprintf(filesfd,"<OPTION VALUE=\"ticketslinkfull\">%s\
<OPTION VALUE=\"showmetaNew\">%s\
<OPTION VALUE=\"showmetaE\">%s\
<OPTION VALUE=\"showanno\">%s\
</SELECT>\n", mySrbStrings[137], mySrbStrings[85], mySrbStrings[86], mySrbStrings[87]);
    fflush(stdout);
	}
	else {
	  fprintf(filesfd,"<FORM name =\"selform%i\" METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" ><TD NOWRAP VALIGN=top><FONT face=\"verdana\" size=\"2\"> \n", Sentries->rowCntr, Sentries->frameStr);
	  fprintf(filesfd,"<SELECT SIZE=1 NAME=function onChange=\"mySubmit(selform%i, this)\">", Sentries->rowCntr);
	  fprintf(filesfd,"<OPTION SELECTED VALUE=\"doNothing\">%s", mySrbStrings[268]);
	  fprintf(filesfd,"<OPTION VALUE=\"slinkfull\">%s", mySrbStrings[137]);
	  fprintf(filesfd,"<OPTION VALUE=\"showmetaNew\">%s", mySrbStrings[85]);
	  fprintf(filesfd,"<OPTION VALUE=\"showmetaE\">%s", mySrbStrings[86]);
	  fprintf(filesfd,"<OPTION VALUE=\"showanno\">%s", mySrbStrings[87]);
	  fprintf(filesfd,"<OPTION VALUE=\"metainsert\">%s", mySrbStrings[91]);
	  fprintf(filesfd,"<OPTION VALUE=\"metainsert\">%s", mySrbStrings[91]);
	  fprintf(filesfd,"<OPTION VALUE=\"ingestextschmnamelisting\">%s",aaa[27]);
	  fprintf(filesfd,"<OPTION VALUE=\"modifymetaform\">%s", mySrbStrings[95]);
	  fprintf(filesfd,"<OPTION VALUE=\"comminsert\">%s", mySrbStrings[94]);
	  fprintf(filesfd,"<OPTION VALUE=\"copydsform\">%s", mySrbStrings[138]);
	  fprintf(filesfd,"<OPTION VALUE=\"movedsform\">%s", mySrbStrings[139]);
	  fprintf(filesfd,"<OPTION VALUE=\"slinksform\">%s", mySrbStrings[140]);
	  fprintf(filesfd,"<OPTION VALUE=\"renamedsform\">%s", mySrbStrings[141]);
	  fprintf(filesfd,"<OPTION VALUE=\"aclform\">%s", mySrbStrings[110]);
	  fprintf(filesfd,"<OPTION VALUE=\"giveticketform\">%s", mySrbStrings[111]);
	  fprintf(filesfd,"<OPTION VALUE=\"removeDataCollForm\">%s", mySrbStrings[142]);
	  fprintf(filesfd,"</SELECT>\n");
	}
	/*	
	fprintf(filesfd,"<INPUT NAME=\"dataname\" type=hidden VALUE=\"%s\">\n",
		urldataList);
	fprintf(filesfd,"<INPUT NAME=\"collection\" type=hidden VALUE=\"%s\">\n",
		urlcollList);
	*/
	fprintf(filesfd,"<INPUT NAME=\"dataname\" type=hidden VALUE=\"%s\">\n",
		Sentries->nonurldataList);
	fprintf(filesfd,"<INPUT NAME=\"collection\" type=hidden VALUE=\"%s\">\n",
		Sentries->nonurlcollList);

	fprintf(filesfd,"<INPUT NAME=\"rsrcname\" type=hidden VALUE=\"%s\">\n", Sentries->urlrsrcName);
	fprintf(filesfd,"<INPUT NAME=\"copynum\" type=hidden VALUE=\"%s\">\n",replNumList);
	fprintf(filesfd,"<INPUT NAME=\"datatype\" type=hidden VALUE=\"%s\">\n",pathNameList);
	fprintf(filesfd,"<INPUT NAME=\"vernum\" type=hidden VALUE=\"%s\">\n",verNumList);
	fprintf(filesfd,"<INPUT NAME=\"owner\" type=hidden VALUE=\"%s@%s\">\n",ownerList,ownerDomnList);
	fprintf(filesfd,"<INPUT NAME=\"session\" type=hidden VALUE=\"%s\">\n",Sentries->pidStr);


	fprintf(filesfd,"</FONT></TD></FORM>\n");
	if (Sentries->isTicket) {
	  fprintf(filesfd,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=ticketslinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/%s.gif\" HEIGHT=18 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=ticketslinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\">%s </A>",Sentries->urldataList,Sentries->urlcollList,Sentries->urlrsrcName, replNumList,pathNameList,verNumList,ownerList,ownerDomnList,Sentries->pidStr, Sentries->frameStr, webPath, Sentries->imageName,
		  Sentries->urldataList,Sentries->urlcollList,Sentries->urlrsrcName, replNumList,pathNameList,verNumList,ownerList,ownerDomnList,Sentries->pidStr, Sentries->frameStr,printdName);
	}
	else {
	  fprintf(filesfd,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=slinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/%s.gif\" HEIGHT=18 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=slinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\">%s </A>",Sentries->urldataList,Sentries->urlcollList,Sentries->urlrsrcName,replNumList, pathNameList,verNumList,ownerList,ownerDomnList,Sentries->pidStr, Sentries->frameStr, webPath, Sentries->imageName,
		  Sentries->urldataList,Sentries->urlcollList,Sentries->urlrsrcName, replNumList, pathNameList,verNumList,ownerList,ownerDomnList,Sentries->pidStr, Sentries->frameStr,printdName);
	}
	
#ifdef DATACOMMENTS
	if (strlen(dataComments) > 0) 
	  fprintf (filesfd,"&nbsp;&nbsp;&nbsp;<IMG SRC=\"%s/commentsbut.gif\" ALT=\"%s\" width=18 height=18 border=0></A>\n", webPath, dataComments);
#endif
	
	
	fprintf(filesfd,"</TD><TD  nowrap align=center> %s </TD><TD nowrap align=center> %s@%s </TD><TD align=center> %s </TD><TD> &nbsp; </TD><TD> &nbsp; </TD><TD> &nbsp; </TD><TD align=center> %s </TD><TD> &nbsp; </TD></TR>\n",timeList,ownerList,ownerDomnList,replNumList,rsrcName);
	/**
	if (showAllMetaFlag == 1) {
	  fprintf (filesfd,"<TR>");
	  fprintf (filesfd,"<TD  valign=top align=right><B>Metadata:</B></TD><TD COLSPAN=8>\n");
	  strcpy(tmpCollName,entries[2].val);
	  strcpy(tmpDataName,entries[1].val);
	  strcpy(entries[2].val,collList);
	  strcpy(entries[1].val,dataList);
	  showMeta();
	  strcpy(entries[2].val,tmpCollName);
	  strcpy(entries[1].val,tmpDataName);
	  fprintf (filesfd,"</TD></TR><TR><TD COLSPAN=9><HR NOSHADE></TD><TR>\n");fflush(stdout);
	}
	**/
      }
      else  { /* not a  directory shadow object */



	if (Sentries->first) {
	  fprintf (stdout,"\n<TR BGCOLOR=\"#DDEFFF\">");
	  
	  /** for object count column **/
	  fprintf(stdout, "<TH></TH>\n");
	  
	  for (jj = 0; jj < Sentries->showMDVCntr; jj++)
	    fprintf (stdout,"<TH>%s</TH>\n",Sentries->showMDVTitle[jj]);
	    
    
	  fprintf (stdout,"<TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH>\
<TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH>\
</TR>\n", mySrbStrings[45], mySrbStrings[46], mySrbStrings[47], mySrbStrings[48],
    mySrbStrings[49], mySrbStrings[50], mySrbStrings[51], mySrbStrings[52],
    mySrbStrings[53], mySrbStrings[54]);
    
	  Sentries->first = 0;
	}
	fflush(stdout);

	Sentries->oddFlag = (Sentries->oddFlag + 1) % 2;
	if (Sentries->oddFlag) {
	  fprintf (stdout,"<TR bgcolor=\"#88BBFF\">");
	}
	else {
	  fprintf (stdout,"<TR bgcolor=\"#DDEEFF\">");
	}

	if (*containerName != '\0') {
	    inCont = "Yes";
	} else {
	    inCont = "No";
	}
	
	/*** object count  ***/
    fprintf(stdout, "<TD>%d</TD>", Sentries->rowCntr);
    /****/
    
	for (jj = 0; jj < Sentries->showMDVCntr; jj++) {
	  if (Sentries->showMDVId[jj] == DATA_ANNOTATION_USERNAME) {
	    fprintf (stdout,"<TD>%s@%s</TD>",udsmdList[jj],udsmdList[jj+1]);
	    udsmdList[jj] += MAX_DATA_SIZE; 	
	    udsmdList[jj+1] += MAX_DATA_SIZE; 	
	    jj++;
	  }
	  else {
	    fprintf (stdout,"<TD>%s</TD>",udsmdList[jj]);
	    udsmdList[jj] += MAX_DATA_SIZE; 	
	  }
	}
	

	fprintf(stdout, "<TD NOWRAP VALIGN=top><FONT face=\"verdana\" size=\"2\">\n");

	fprintf (stdout,"<FORM name =\"selform%i\" METHOD=\"POST\" ACTION=\"mysrb331.cgi\" target=\"%s\" >\n", Sentries->rowCntr,Sentries->frameStr);
	if (Sentries->isTicket) {
	  fprintf (stdout,"<SELECT SIZE=1 NAME=function onChange=\"mySubmit(selform%i, this)\">", Sentries->rowCntr);
	  fprintf(stdout,"<OPTION SELECTED VALUE=\"doNothing\">%s", mySrbStrings[268]);
fprintf(stdout,"<OPTION VALUE=\"ticketfull\">%s\
<OPTION VALUE=\"ticketmeta\">%s\
<OPTION VALUE=\"ticketdownloadfile\">%s\
</SELECT>\n", mySrbStrings[84], mySrbStrings[85], mySrbStrings[88]);
	}
	else {
	  if (!strcmp(dataTypeList,"URL")) {
	    fprintf(stdout,"<SELECT SIZE=1 NAME=function onChange=\"mySubmit(selform%i, this)\">", Sentries->rowCntr);
	    fprintf(stdout,"<OPTION SELECTED VALUE=\"doNothing\">%s", mySrbStrings[268]);
        fprintf(stdout,"<OPTION VALUE=\"urlfull\">%s", mySrbStrings[130]);
        fprintf(stdout,"<OPTION VALUE=\"showmetaNew\">%s", mySrbStrings[85]);
	    fprintf(stdout,"<OPTION VALUE=\"showmetaE\">%s", mySrbStrings[86]);
	    fprintf(stdout,"<OPTION VALUE=\"showanno\">%s", mySrbStrings[87]);
	    fprintf(stdout,"<OPTION VALUE=\"metaextractform\">%s", mySrbStrings[90]);
	    fprintf(stdout,"<OPTION VALUE=\"metainsert\">%s", mySrbStrings[91]);
	    fprintf(stdout,"<OPTION VALUE=\"ingestextschmnamelisting\">%s",aaa[27]);
	    fprintf(stdout,"<OPTION VALUE=\"dcinsert\">%s", mySrbStrings[92]);
	    fprintf(stdout,"<OPTION VALUE=\"annoinsert\">%s", mySrbStrings[93]);
	    fprintf(stdout,"<OPTION VALUE=\"comminsert\">%s", mySrbStrings[94]);
	    fprintf(stdout,"<OPTION VALUE=\"modifymetaform\">%s", mySrbStrings[95]);
	    fprintf(stdout,"<OPTION VALUE=\"modifyannoform\">%s", mySrbStrings[96]);
	    fprintf(stdout,"<OPTION VALUE=\"changeURLform\">%s", mySrbStrings[265]);
	    fprintf(stdout,"<OPTION VALUE=\"copymetaform\">%s", mySrbStrings[97]);
	    fprintf(stdout,"<OPTION VALUE=\"copyannoform\">%s", mySrbStrings[98]);
	    fprintf(stdout,"<OPTION VALUE=\"deletemetaform\">%s", mySrbStrings[99]);
	    fprintf(stdout,"<OPTION VALUE=\"deleteannoform\">%s", mySrbStrings[100]);
	    fprintf(stdout,"<OPTION VALUE=\"renamedsform\">%s", mySrbStrings[131]);
	    fprintf(stdout,"<OPTION VALUE=\"replicationform\">%s", mySrbStrings[102]);
	    fprintf(stdout,"<OPTION VALUE=\"ingestreplicaform\">%s", mySrbStrings[103]);
	    fprintf(stdout,"<OPTION VALUE=\"registerreplicaform\">%s", mySrbStrings[104]);
	    fprintf(stdout,"<OPTION VALUE=\"ingestnewversionform\">%s", mySrbStrings[105]);
	    fprintf(stdout,"<OPTION VALUE=\"createsnapshotform\">%s", mySrbStrings[106]);
	    fprintf(stdout,"<OPTION VALUE=\"copydsform\">%s", mySrbStrings[118]);
	    fprintf(stdout,"<OPTION VALUE=\"movedsform\">%s", mySrbStrings[119]);
	    fprintf(stdout,"<OPTION VALUE=\"slinksform\">%s", mySrbStrings[120]);
	    fprintf(stdout,"<OPTION VALUE=\"aclform\">%s", mySrbStrings[110]);
	    fprintf(stdout,"<OPTION VALUE=\"giveticketform\">%s", mySrbStrings[111]);
	    fprintf(stdout,"<OPTION VALUE=\"removeDataCollForm\">%s", mySrbStrings[121]);
        fprintf(stdout,"</SELECT>\n");
	  }
	  else if (!strcmp(dataTypeList,"Executable")) {
	    
	    fprintf(stdout,"<SELECT SIZE=1 NAME=function onChange=\"mySubmit(selform%i, this)\">", Sentries->rowCntr);
	    fprintf(stdout,"<OPTION SELECTED VALUE=\"doNothing\">%s", mySrbStrings[268]);
	    fprintf(stdout,"<OPTION VALUE=\"execfull\">%s", mySrbStrings[136]);
	    fprintf(stdout,"<OPTION VALUE=\"showmetaNew\">%s", mySrbStrings[85]);
	    fprintf(stdout,"<OPTION VALUE=\"showmetaE\">%s", mySrbStrings[86]);
	    fprintf(stdout,"<OPTION VALUE=\"showanno\">%s", mySrbStrings[87]);
	    fprintf(stdout,"<OPTION VALUE=\"metaextractform\">%s", mySrbStrings[90]);
	    fprintf(stdout,"<OPTION VALUE=\"metainsert\">%s", mySrbStrings[91]);
	    fprintf(stdout,"<OPTION VALUE=\"ingestextschmnamelisting\">%s",aaa[27]);
	    fprintf(stdout,"<OPTION VALUE=\"dcinsert\">%s", mySrbStrings[92]);
	    fprintf(stdout,"<OPTION VALUE=\"annoinsert\">%s", mySrbStrings[93]);
	    fprintf(stdout,"<OPTION VALUE=\"comminsert\">%s", mySrbStrings[94]);
	    fprintf(stdout,"<OPTION VALUE=\"modifymetaform\">%s", mySrbStrings[95]);
	    fprintf(stdout,"<OPTION VALUE=\"modifyannoform\">%s", mySrbStrings[96]);
	    fprintf(stdout,"<OPTION VALUE=\"copymetaform\">%s", mySrbStrings[97]);
	    fprintf(stdout,"<OPTION VALUE=\"copyannoform\">%s", mySrbStrings[98]);
	    fprintf(stdout,"<OPTION VALUE=\"deletemetaform\">%s", mySrbStrings[99]);
	    fprintf(stdout,"<OPTION VALUE=\"deleteannoform\">%s", mySrbStrings[100]);
	    fprintf(stdout,"<OPTION VALUE=\"renamedsform\">%s", mySrbStrings[135]);
	    fprintf(stdout,"<OPTION VALUE=\"replicationform\">%s", mySrbStrings[102]);
	    fprintf(stdout,"<OPTION VALUE=\"ingestreplicaform\">%s", mySrbStrings[103]);
	    fprintf(stdout,"<OPTION VALUE=\"registerreplicaform\">%s", mySrbStrings[104]);
	    fprintf(stdout,"<OPTION VALUE=\"ingestnewversionform\">%s", mySrbStrings[105]);
	    fprintf(stdout,"<OPTION VALUE=\"createsnapshotform\">%s", mySrbStrings[106]);
	    fprintf(stdout,"<OPTION VALUE=\"copydsform\">%s", mySrbStrings[122]);
	    fprintf(stdout,"<OPTION VALUE=\"slinksform\">%s", mySrbStrings[123]);
	    fprintf(stdout,"<OPTION VALUE=\"aclform\">%s", mySrbStrings[110]);
	    fprintf(stdout,"<OPTION VALUE=\"giveticketform\">%s", mySrbStrings[111]);
	    fprintf(stdout,"<OPTION VALUE=\"removeDataCollForm\">%s", mySrbStrings[124]);
	    fprintf(stdout,"</SELECT>\n");

	  }
	  else if (!strcmp(dataTypeList,"database shadow object")) {

	    fprintf(stdout,"<SELECT SIZE=1 NAME=function onChange=\"mySubmit(selform%i, this)\">", Sentries->rowCntr);
	    fprintf(stdout,"<OPTION SELECTED VALUE=\"doNothing\">%s", mySrbStrings[268]);
	    fprintf(stdout,"<OPTION VALUE=\"sqlfull\">%s", mySrbStrings[125]);
	    fprintf(stdout,"<OPTION VALUE=\"showmetaNew\">%s", mySrbStrings[85]);
	    fprintf(stdout,"<OPTION VALUE=\"showmetaE\">%s", mySrbStrings[86]);
	    fprintf(stdout,"<OPTION VALUE=\"showanno\">%s", mySrbStrings[87]);
	    fprintf(stdout,"<OPTION VALUE=\"metaextractform\">%s", mySrbStrings[90]);
	    fprintf(stdout,"<OPTION VALUE=\"metainsert\">%s", mySrbStrings[91]);
	    fprintf(stdout,"<OPTION VALUE=\"ingestextschmnamelisting\">%s",aaa[27]);
	    fprintf(stdout,"<OPTION VALUE=\"dcinsert\">%s", mySrbStrings[92]);
	    fprintf(stdout,"<OPTION VALUE=\"annoinsert\">%s", mySrbStrings[93]);
	    fprintf(stdout,"<OPTION VALUE=\"comminsert\">%s", mySrbStrings[94]);
	    fprintf(stdout,"<OPTION VALUE=\"modifymetaform\">%s", mySrbStrings[95]);
	    fprintf(stdout,"<OPTION VALUE=\"modifyannoform\">%s", mySrbStrings[96]);
	    fprintf(stdout,"<OPTION VALUE=\"changeSQLform\">%s", mySrbStrings[266]);
	    fprintf(stdout,"<OPTION VALUE=\"copymetaform\">%s", mySrbStrings[97]);
	    fprintf(stdout,"<OPTION VALUE=\"copyannoform\">%s", mySrbStrings[98]);
	    fprintf(stdout,"<OPTION VALUE=\"deletemetaform\">%s", mySrbStrings[99]);
	    fprintf(stdout,"<OPTION VALUE=\"deleteannoform\">%s", mySrbStrings[100]);
	    fprintf(stdout,"<OPTION VALUE=\"renamedsform\">%s", mySrbStrings[134]);
	    fprintf(stdout,"<OPTION VALUE=\"replicationform\">%s", mySrbStrings[102]);
	    fprintf(stdout,"<OPTION VALUE=\"ingestreplicaform\">%s", mySrbStrings[103]);
	    fprintf(stdout,"<OPTION VALUE=\"registerreplicaform\">%s", mySrbStrings[104]);
	    fprintf(stdout,"<OPTION VALUE=\"ingestnewversionform\">%s", mySrbStrings[105]);
	    fprintf(stdout,"<OPTION VALUE=\"createsnapshotform\">%s", mySrbStrings[106]);
	    fprintf(stdout,"<OPTION VALUE=\"copydsform\">%s", mySrbStrings[126]);
	    fprintf(stdout,"<OPTION VALUE=\"movedsform\">%s", mySrbStrings[127]);
	    fprintf(stdout,"<OPTION VALUE=\"slinksform\">%s", mySrbStrings[128]);
	    fprintf(stdout,"<OPTION VALUE=\"aclform\">%s", mySrbStrings[110]);
	    fprintf(stdout,"<OPTION VALUE=\"giveticketform\">%s", mySrbStrings[111]);
	    fprintf(stdout,"<OPTION VALUE=\"removeDataCollForm\">%s", mySrbStrings[129]);
	    fprintf(stdout,"</SELECT>\n");

	  }
	  
	  /* datascope specific ????? --sifang */
	  /* ATTENTION !!! */
	  /* i am not sure if this part is needed, as i am merging roadnet mysrb to newest mysrb  :( */
	  /* looks like it's obslete tho, by comparing to the original alpha1.c
	  /* if there is problem, try uncomment this part, don't foget to adapte mySrbStrings... */
	  
	  else if (!strcmp(dataTypeList,"orb data")) {
	    fprintf(stdout,"<SELECT SIZE=1 NAME=function onChange=\"mySubmit(selform%i, this)\">", Sentries->rowCntr);
	    fprintf(stdout,"<OPTION SELECTED VALUE=\"doNothing\">%s", mySrbStrings[268]);  
	    fprintf(stdout,
	    " <OPTION SELECTED VALUE=\"full\">Access ORB\
        <OPTION VALUE=\"showmetaNew\">Show All Metadata\
        <OPTION VALUE=\"showmetaE\">Show Entity Metadata\
        <OPTION VALUE=\"showanno\">Show Annotations\
        <OPTION VALUE=\"metaextractform\">Extract Metadata\
        <OPTION VALUE=\"metainsert\">Insert MyMetadata\
	<OPTION VALUE=\"ingestextschmnamelisting\">%s\
        <OPTION VALUE=\"dcinsert\">Insert DublinCore\
        <OPTION VALUE=\"annoinsert\">Insert Annotation\
        <OPTION VALUE=\"comminsert\">Update Comment\
        <OPTION VALUE=\"modifymetaform\">Modify Metadata\
        <OPTION VALUE=\"modifyannoform\">Modify Annotation\
        <OPTION VALUE=\"copymetaform\">Copy Metadata\
        <OPTION VALUE=\"copyannoform\">Copy Annotation\
        <OPTION VALUE=\"deletemetaform\">Delete All Metadata\
        <OPTION VALUE=\"deleteannoform\">Delete All Annotation\
        <OPTION VALUE=\"changeaccesspathform\">Change Access Command\
        <OPTION VALUE=\"renamedsform\">ReName Object\
        <OPTION VALUE=\"replicationform\">Replicate\
        <OPTION VALUE=\"ingestreplicaform\">Ingest Replica\
        <OPTION VALUE=\"registerreplicaform\">Register Replica\
        <OPTION VALUE=\"ingestnewversionform\">Ingest New Version\
        <OPTION VALUE=\"createsnapshotform\">Create Snapshot\
        <OPTION VALUE=\"copydsform\">Copy\
        <OPTION VALUE=\"movedsform\">Move\
        <OPTION VALUE=\"slinksform\">SoftLink\
        <OPTION VALUE=\"aclform\">Control Access\
        <OPTION VALUE=\"giveticketform\">Issue Ticket Access\
        <OPTION VALUE=\"removeDataCollForm\">delete ORB Command\
        </SELECT>\n",aaa[27]);
	  }
	  
	  else {
	    
	    fprintf (stdout,"<SELECT SIZE=1 NAME=function onChange=\"mySubmit(selform%i, this)\">\n", Sentries->rowCntr);
	    fprintf(stdout,"<OPTION SELECTED VALUE=\"doNothing\">%s", mySrbStrings[268]);
	    fprintf (stdout,"<OPTION VALUE=\"full\">%s", mySrbStrings[84]);

	    if (strstr(dataTypeList,"text") != NULL ||
		strstr(dataTypeList,"html") != NULL ||
		strstr(dataTypeList," code") != NULL||
		strstr(dataTypeList,"xml") != NULL  ||
		strstr(dataTypeList,"script") != NULL ||
		strstr(dataList,".txt") != NULL) {
	      fprintf (stdout,"<OPTION VALUE=\"editform\">%s", mySrbStrings[132]);
	      fprintf (stdout,"<OPTION VALUE=\"appendform\">%s", mySrbStrings[133]);
	    }
	    fprintf (stdout,"<OPTION VALUE=\"reingestform\">%s", mySrbStrings[89]);
	    fprintf (stdout,"<OPTION VALUE=\"showmetaNew\">%s", mySrbStrings[85]);
	    fprintf (stdout,"<OPTION VALUE=\"showmetaE\">%s", mySrbStrings[86]);
	    fprintf (stdout,"<OPTION VALUE=\"showanno\">%s", mySrbStrings[87]);
	    fprintf (stdout,"<OPTION VALUE=\"metaextractform\">%s", mySrbStrings[90]);
	    fprintf (stdout,"<OPTION VALUE=\"metainsert\">%s", mySrbStrings[91]);
	    fprintf(stdout,"<OPTION VALUE=\"ingestextschmnamelisting\">%s",aaa[27]);
	    fprintf (stdout,"<OPTION VALUE=\"dcinsert\">%s", mySrbStrings[92]);
	    fprintf (stdout,"<OPTION VALUE=\"annoinsert\">%s", mySrbStrings[93]);
	    fprintf (stdout,"<OPTION VALUE=\"comminsert\">%s", mySrbStrings[94]);
	    fprintf (stdout,"<OPTION VALUE=\"modifymetaform\">%s", mySrbStrings[95]);
	    fprintf (stdout,"<OPTION VALUE=\"modifyannoform\">%s", mySrbStrings[96]);
	    fprintf (stdout,"<OPTION VALUE=\"changetypeform\">%s", mySrbStrings[267]);
	    fprintf (stdout,"<OPTION VALUE=\"copymetaform\">%s", mySrbStrings[97]);
	    fprintf (stdout,"<OPTION VALUE=\"copyannoform\">%s", mySrbStrings[98]);
	    fprintf (stdout,"<OPTION VALUE=\"deletemetaform\">%s", mySrbStrings[99]);
	    fprintf (stdout,"<OPTION VALUE=\"deleteannoform\">%s", mySrbStrings[100]);
	    fprintf (stdout,"<OPTION VALUE=\"renamedsform\">%s", mySrbStrings[101]);
	    fprintf (stdout,"<OPTION VALUE=\"replicationform\">%s", mySrbStrings[102]);
	    fprintf (stdout,"<OPTION VALUE=\"ingestreplicaform\">%s", mySrbStrings[103]);
	    fprintf (stdout,"<OPTION VALUE=\"registerreplicaform\">%s", mySrbStrings[104]);
	    fprintf (stdout,"<OPTION VALUE=\"ingestnewversionform\">%s", mySrbStrings[105]);
	    fprintf (stdout,"<OPTION VALUE=\"createsnapshotform\">%s", mySrbStrings[106]);
	    fprintf (stdout,"<OPTION VALUE=\"copydsform\">%s", mySrbStrings[107]);
	    fprintf (stdout,"<OPTION VALUE=\"movedsform\">%s", mySrbStrings[108]);
	    fprintf (stdout,"<OPTION VALUE=\"slinksform\">%s", mySrbStrings[109]);
	    fprintf (stdout,"<OPTION VALUE=\"aclform\">%s", mySrbStrings[110]);
	    fprintf (stdout,"<OPTION VALUE=\"giveticketform\">%s", mySrbStrings[111]);
	    fprintf (stdout,"<OPTION VALUE=\"downloadfile\">%s", mySrbStrings[88]);
	    fprintf (stdout,"<OPTION VALUE=\"rlock\">%s", mySrbStrings[114]);
	    fprintf (stdout,"<OPTION VALUE=\"wlock\">%s", mySrbStrings[115]);
	    fprintf (stdout,"<OPTION VALUE=\"unlock\">%s", mySrbStrings[116]);
	    fprintf (stdout,"<OPTION VALUE=\"removeDataCollForm\">%s", mySrbStrings[117]);
	    fprintf (stdout,"</SELECT>\n");

	  }
	}
	/*
	fprintf(stdout,"<INPUT NAME=\"dataname\" type=hidden VALUE=\"%s\">\n",
		Sentries->urldataList);fflush(stdout);
	fprintf(stdout,"<INPUT NAME=\"collection\" type=hidden VALUE=\"%s\">\n",
		Sentries->urlcollList);
	*/
	fprintf(stdout,"<INPUT NAME=\"dataname\" type=hidden VALUE=\"%s\">\n",
		Sentries->nonurldataList);
	fprintf(stdout,"<INPUT NAME=\"collection\" type=hidden VALUE=\"%s\">\n",
		Sentries->nonurlcollList);

	fprintf(stdout,"<INPUT NAME=\"rsrcname\" type=hidden VALUE=\"%s\">\n",Sentries->urlrsrcName);
	fprintf(stdout,"<INPUT NAME=\"copynum\" type=hidden VALUE=\"%s\">\n",replNumList);
	if (!strcmp(dataTypeList,"URL") ||
	    !strcmp(dataTypeList,"database shadow object") ||
	    !strcmp(dataTypeList,"Executable") ||
	    !strcmp(dataTypeList,"datascope data") ||
	    !strcmp(dataTypeList,"orb data")) 
	  fprintf(stdout,"<INPUT NAME=\"pathname\" type=hidden VALUE=\"%s\">\n",Sentries->urlpathNameList);
	else if (!strcmp(dataTypeList,"Executable")) 
	  fprintf(stdout,"<INPUT NAME=\"pathname\" type=hidden VALUE=\"%s\">\n",Sentries->urlpathNameList);
	else
	  fprintf(stdout,"<INPUT NAME=\"datatype\" type=hidden VALUE=\"%s\">\n",Sentries->urldataTypeList);
	  
	fprintf(stdout,"<INPUT NAME=\"vernum\" type=hidden VALUE=\"%s\">\n",verNumList);

	fprintf(stdout,"<INPUT NAME=\"owner\" type=hidden VALUE=\"%s@%s\">\n",ownerList,ownerDomnList);

	fprintf(stdout,"<INPUT NAME=\"session\" type=hidden VALUE=\"%s\">\n",
	Sentries->pidStr);
	fprintf(stdout,"</FONT></TD></FORM>\n"); 
	fflush(stdout);

	fprintf (stdout,"<TD VALIGN=middle nowrap>");
	
	if (Sentries->isTicket) 
	  strcpy(funcType, "ticket");	
	else 
	  strcpy(funcType, "");
	
	/* datascope specific --sifang */
	/* WARNNING: i don't know if this would work... */
	/* if it doesn't check line 7902 of raja's roadnet mysrb */
	  
	if (!strcmp(dataTypeList,"URL"))
	  strcat(funcType,"urlfull");
	else if (!strcmp(dataTypeList,"database shadow object"))
	  strcat(funcType,"sqlfull");
	else if (!strcmp(dataTypeList,"Executable"))
	  strcat(funcType,"execfull");
	else if  (!strcmp(dataTypeList,"datascope data"))
	  strcat(funcType, "datascopefull");	
	else
	 strcat(funcType,"full");
	
    /* avoid unwanted final \n */
    snprintf(buf, strlen(mySrbStrings[88]), "%s\0", mySrbStrings[88]);
    
    
    fprintf (stdout,"<A HREF=\"mysrb331.cgi?function=%s&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\" ><IMG SRC=\"%s/docimg.gif\" ALIGN=middle HEIGHT=18 BORDER=0></A> \n", funcType, Sentries->urldataList, Sentries->urlcollList, Sentries->urlrsrcName, replNumList,Sentries->urlpathNameList,verNumList,ownerList,ownerDomnList, Sentries->pidStr, Sentries->frameStr, webPath); 

    /* new download link */		 
    if (!strcmp(funcType, "full")) {
	    fprintf (stdout,"<A HREF=\"mysrb331.cgi/%s?function=downloadfile&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\" ><IMG SRC=\"%s/downloadDS.gif\" ALIGN=middle HEIGHT=18 BORDER=0 ALT=\"%s\" ></A> \n", printdName, Sentries->urldataList, Sentries->urlcollList, Sentries->urlrsrcName, replNumList,Sentries->urlpathNameList,verNumList,ownerList,ownerDomnList, Sentries->pidStr, Sentries->frameStr, webPath, buf);     
    }    
    /*********************/
    
	fprintf (stdout,"<A HREF=\"mysrb331.cgi?function=%s&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\" >%s </A>", funcType, Sentries->urldataList, Sentries->urlcollList, Sentries->urlrsrcName, replNumList,Sentries->urlpathNameList,verNumList,ownerList,ownerDomnList, Sentries->pidStr, Sentries->frameStr,
		 printdName );

#ifdef DATACOMMENTS
	if (strlen(dataComments) > 0) 
	fprintf (stdout,"&nbsp;&nbsp;&nbsp;<IMG SRC=\"%s/commentsbut.gif\" ALT=\"%s\" ALIGN=middle width=18 height=18 border=0></A>\n",webPath, dataComments);
#endif

	fprintf (stdout,"</TD><TD nowrap> %s </TD>\
<TD nowrap align=center> %s@%s </TD><TD align=center> %s </TD><TD align=center> %s </TD><TD align=center> %s</TD><TD nowrap align=center> %s </TD><TD nowrap align=center> %s </TD><TD align=center> %s </TD></TR>\n",
		 timeList,ownerList,ownerDomnList , replNumList,verNumList, sizeList,dataTypeList,rsrcName, inCont);


	if (Sentries->showAllMetaFlag == 1) {
	  fprintf (stdout,"<TR>");
	  fprintf (stdout,"<TD  valign=top align=right><B>Metadata:</B></TD><TD COLSPAN=8>\n");
	  strcpy(tmpCollName, Sentries->entries[2].val);
	  strcpy(tmpDataName, Sentries->entries[1].val);
	  strcpy(Sentries->entries[2].val,collList);
	  strcpy(Sentries->entries[1].val,dataList);
	  showMeta(Sentries);
	  strcpy(Sentries->entries[2].val,tmpCollName);
	  strcpy(Sentries->entries[1].val,tmpDataName);
	  fprintf (stdout,"</TD></TR><TR><TD COLSPAN=9><HR NOSHADE></TD><TR>\n");fflush(stdout);
	}
      } /* else for directory shadow object */
        ownerList += MAX_DATA_SIZE;
        ownerDomnList += MAX_DATA_SIZE;
        sizeList += MAX_DATA_SIZE;
        replNumList += MAX_DATA_SIZE;
        verNumList += MAX_DATA_SIZE;
        timeList += MAX_DATA_SIZE;
        dataList += MAX_DATA_SIZE;
        dataTypeList += MAX_DATA_SIZE;
        rsrcName += MAX_DATA_SIZE;
    	containerName += MAX_DATA_SIZE;
#ifdef DATACOMMENTS
	dataComments += MAX_DATA_SIZE;
#endif
	if (Sentries->queryFunction)
	  udsmd5List += MAX_DATA_SIZE;
	collList += MAX_DATA_SIZE;
	pathNameList += MAX_DATA_SIZE;
	Sentries->rowCntr++;
    }
}


int prtDataLongWeb_simple(inStruct Sentries, mdasC_sql_result_struct *myresult, int chkColl)
{
    int i, jj;
    char *dataList, *sizeList, *ownerList, *timeList, *replNumList, *rsrcName, *ownerDomnList;
    char *collList, *containerName, *dataComments, *dataTypeList, *verNumList, *pathNameList;
    char *udsmd5List;
    char *udsmdList[20];
    char tmpContainerName[MAX_TOKEN], printdName[ 4 * MAX_TOKEN];
    char *inCont, *tmpPtr;
    char tmpCollName[MAX_TOKEN], tmpDataName[MAX_TOKEN], funcType[MAX_TOKEN];
    
    
    dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    dataTypeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
    sizeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[SIZE], dcs_aname[SIZE]);
    ownerList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_OWNER], dcs_aname[DATA_OWNER]);
    ownerDomnList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_OWNER_DOMAIN], dcs_aname[DATA_OWNER_DOMAIN]);

    timeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
    replNumList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);

    verNumList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);
    rsrcName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
    containerName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);
    pathNameList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);
#ifdef DATACOMMENTS
    dataComments = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_COMMENTS], dcs_aname[DATA_COMMENTS]);
#endif
    if (queryFunction)
      udsmd5List =  (char *) getFromResultStruct(
            (mdasC_sql_result_struct *) myresult,
	    dcs_tname[UDSMD5], dcs_aname[UDSMD5]);
    collList = (char *) getFromResultStruct(
	    (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    Sentries->oddFlag = 0;
    jj = 0;

    for (jj =  0; jj < Sentries->showMDVCntr; jj++)  {
      udsmdList[jj] = (char *) getFromResultStruct(
	  (mdasC_sql_result_struct *) myresult,
	   dcs_tname[Sentries->showMDVId[jj]], dcs_aname[Sentries->showMDVId[jj]]);
      if (udsmdList[jj] == NULL)
	printf("jj=%i is NULL!! showMDVId[jj]= %i !!\n",jj, Sentries->showMDVId[jj]);
    }



    for (i = 0; i < myresult->row_count; i++) {
      strcpy(Sentries->urlcollList,collList);
      strcpy(Sentries->urldataList,dataList);
      strcpy(Sentries->nonurlcollList,collList);
      strcpy(Sentries->nonurldataList,dataList);
      strcpy(Sentries->urlpathNameList,pathNameList);
      strcpy(Sentries->urldataTypeList,dataTypeList);
      strcpy(Sentries->urlrsrcName,rsrcName);
        if (atoi(sizeList) == -1)
	  strcpy(sizeList,"UNK");
      escape_url(Sentries->urlcollList);
      escape_url(Sentries->urldataList);
      escape_url(Sentries->urlpathNameList);
      escape_url(Sentries->urldataTypeList);
      escape_url(Sentries->urlrsrcName);
      if (chkColl == CHK_COLL ) 
	sprintf(printdName,"%s/%s",collList, dataList);
      else
	strcpy(printdName,dataList);


      if (strstr(dataTypeList,"directory shadow object")  != NULL) {
	if (Sentries->firsts) {
	  filesfd = tmpfile();
	  if (filesfd == NULL) 
	    filesfd = stdout;
	  else {
	    fprintf (filesfd,"\n<TR BGCOLOR=\"#DDEFFF\">");
	    fprintf (filesfd,"<TH>Function</TH><TH>Shadow ObjName</TH><TH>Creation Time</TH><TH>Owner</TH><TH> &nbsp; </TH><TH> &nbsp; </TH><TH> &nbsp; </TH><TH> &nbsp; </TH><TH>Resource</TH><TH> &nbsp; </TH></TR>\n");
	  }
	  Sentries->firsts = 0;
	}
	Sentries->oddFlag = (Sentries->oddFlag + 1) % 2;
	if (Sentries->oddFlag) {
	  /* fprintf (filesfd,"<TR bgcolor=\"#DDCC99\">");*/
	  fprintf (filesfd,"<TR bgcolor=\"#FFFFFF\">");
	}
	else {
	  /*fprintf (filesfd,"<TR bgcolor=\"#CCCCCC\">");*/
	  fprintf (filesfd,"<TR bgcolor=\"#DDeeff\">");
	}
	strcpy(Sentries->imageName,"shadowobj");
	if (Sentries->queryFunction) {
	  if (strlen(udsmd5List) > 0) {
	    strcat(printdName,"->");
	    strcat(printdName,&(udsmd5List[2]));
	    if (udsmd5List[0] == 'F'){
	      strcat(Sentries->urldataList,"@SHADOW=");
	      strcat(Sentries->urldataList,&(udsmd5List[2]));
	      strcat(Sentries->nonurldataList,"@SHADOW=");
	      strcat(Sentries->nonurldataList,&(udsmd5List[2]));
	      escape_url(Sentries->urldataList);
	      strcpy(verNumList,"-1");
	      strcpy(Sentries->imageName,"docimg");
	    }
	    else if (udsmd5List[0] == 'D'){
	      strcat(pathNameList,&(udsmd5List[2]));
	      escape_url(pathNameList);
	      strcpy(verNumList,"-2");
	      strcpy(Sentries->imageName,"collimg");
	    }
	  }
	}


	    


	if (Sentries->isTicket) {
	  fprintf(filesfd,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=ticketslinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/%s.gif\" HEIGHT=18 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=ticketslinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\">%s </A>",Sentries->urldataList,Sentries->urlcollList,Sentries->urlrsrcName, replNumList,pathNameList,verNumList,ownerList,ownerDomnList,Sentries->pidStr, Sentries->frameStr, webPath, imageName,
		  Sentries->urldataList,Sentries->urlcollList,Sentries->urlrsrcName, replNumList,pathNameList,verNumList,ownerList,ownerDomnList,Sentries->pidStr, Sentries->frameStr,printdName);
	}
	else {
	  fprintf(filesfd,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=showmetaNew&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/%s.gif\" HEIGHT=18 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=slinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\">%s </A>",Sentries->urldataList,Sentries->urlcollList,Sentries->urlrsrcName,replNumList, pathNameList,verNumList,ownerList,ownerDomnList,Sentries->pidStr, Sentries->frameStr, webPath, imageName,
		  Sentries->urldataList,Sentries->urlcollList,Sentries->urlrsrcName, replNumList, pathNameList,verNumList,ownerList,ownerDomnList,Sentries->pidStr, Sentries->frameStr,printdName);
	}
	
#ifdef DATACOMMENTS
	if (strlen(dataComments) > 0) 
	  fprintf (filesfd,"&nbsp;&nbsp;&nbsp;<IMG SRC=\"%s/commentsbut.gif\" ALT=\"%s\" width=18 height=18 border=0></A>\n", webPath, dataComments);
#endif
	
	
	fprintf(filesfd,"</TD><TD  nowrap align=center> %s </TD><TD nowrap align=center> %s@%s </TD><TD align=center> %s </TD><TD> &nbsp; </TD><TD> &nbsp; </TD><TD> &nbsp; </TD><TD align=center> %s </TD><TD> &nbsp; </TD></TR>\n",timeList,ownerList,ownerDomnList,replNumList,rsrcName);
	/**
	if (showAllMetaFlag == 1) {
	  fprintf (filesfd,"<TR>");
	  fprintf (filesfd,"<TD  valign=top align=right><B>Metadata:</B></TD><TD COLSPAN=8>\n");
	  strcpy(tmpCollName,entries[2].val);
	  strcpy(tmpDataName,entries[1].val);
	  strcpy(entries[2].val,collList);
	  strcpy(entries[1].val,dataList);
	  showMeta();
	  strcpy(entries[2].val,tmpCollName);
	  strcpy(entries[1].val,tmpDataName);
	  fprintf (filesfd,"</TD></TR><TR><TD COLSPAN=9><HR NOSHADE></TD><TR>\n");fflush(stdout);
	}
	**/
      }
      else  { /* not a  directory shadow object */



	if (Sentries->first) {
	  fprintf (stdout,"\n<TR BGCOLOR=\"#DDEFFF\">");
	  for (jj = 0; jj < Sentries->showMDVCntr; jj++)
	    fprintf (stdout,"<TH>%s</TH>\n",Sentries->showMDVTitle[jj]);
	    
    
	  fprintf (stdout,"<TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH>\
</TR>\n",  mySrbStrings[46], mySrbStrings[47], mySrbStrings[48],
     mySrbStrings[51], mySrbStrings[52]);

    
	  Sentries->first = 0;
	}

	Sentries->oddFlag = (Sentries->oddFlag + 1) % 2;
	if (Sentries->oddFlag) {
	  fprintf (stdout,"<TR bgcolor=\"#FFFFFF\">");
	}
	else {
	  fprintf (stdout,"<TR bgcolor=\"#DDEEFF\">");
	}

	if (*containerName != '\0') {
	    inCont = "Yes";
	} else {
	    inCont = "No";
	}

	for (jj = 0; jj < Sentries->showMDVCntr; jj++) {
	  if (Sentries->showMDVId[jj] == DATA_ANNOTATION_USERNAME) {
	    fprintf (stdout,"<TD>%s@%s</TD>",udsmdList[jj],udsmdList[jj+1]);
	    udsmdList[jj] += MAX_DATA_SIZE; 	
	    udsmdList[jj+1] += MAX_DATA_SIZE; 	
	    jj++;
	  }
	  else {
	    fprintf (stdout,"<TD>%s</TD>",udsmdList[jj]);
	    udsmdList[jj] += MAX_DATA_SIZE; 	
	  }
	}

	fprintf (stdout,"<TD VALIGN=middle nowrap>");
//	if (!strcmp(dataTypeList,"URL"))
//	  strcpy(funcType,"urlfull");
//	else if (!strcmp(dataTypeList,"database shadow object"))
//	  strcpy(funcType,"sqlfull");
//	else if (!strcmp(dataTypeList,"Executable"))
//	  strcpy(funcType,"execfull");
//	else if (isTicket) 
//	  strcpy(funcType,"ticketfull");
//	else
//	  strcpy(funcType,"full");
	strcpy(funcType,"showmetaNew");

	fprintf (stdout,"<A HREF=\"mysrb331.cgi?function=%s&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\" ><IMG SRC=\"%s/docimg.gif\" HEIGHT=18 BORDER=0></A> \n", funcType, Sentries->urldataList, Sentries->urlcollList, Sentries->urlrsrcName, replNumList,Sentries->urlpathNameList,verNumList,ownerList,ownerDomnList, Sentries->pidStr, Sentries->frameStr, webPath); 
    strcpy(funcType,"full");
	fprintf (stdout,"<A HREF=\"mysrb331.cgi?function=%s&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\" >%s </A>", funcType, Sentries->urldataList, Sentries->urlcollList, Sentries->urlrsrcName, replNumList,Sentries->urlpathNameList,verNumList,ownerList,ownerDomnList, Sentries->pidStr, Sentries->frameStr,
		 printdName );


#ifdef DATACOMMENTS
	if (strlen(dataComments) > 0) 
	fprintf (stdout,"&nbsp;&nbsp;&nbsp;<IMG SRC=\"%s/commentsbut.gif\" ALT=\"%s\" width=18 height=18 border=0></A>\n",webPath, dataComments);
#endif

	
    if (!strcmp(mode,"light")) {
        
	fprintf (stdout,"</TD><TD nowrap> %s </TD>\
<TD nowrap align=center> %s@%s </TD><TD align=center> %s </TD><TD nowrap align=center> %s </TD></TR>\n",
		 timeList,ownerList,ownerDomnList , sizeList, dataTypeList);
    }
    
    else {
    
	fprintf (stdout,"</TD><TD nowrap> %s </TD>\
<TD nowrap align=center> %s@%s </TD><TD align=center> %s </TD><TD align=center> %s </TD><TD align=right>&nbsp;&nbsp;&nbsp;%s</TD><TD nowrap align=center> %s </TD><TD nowrap align=center> %s </TD><TD align=center> %s </TD></TR>\n",
		 timeList,ownerList,ownerDomnList , replNumList,verNumList, sizeList,dataTypeList,rsrcName, inCont);
    }


	if (Sentries->showAllMetaFlag == 1) {
	  fprintf (stdout,"<TR>");
	  fprintf (stdout,"<TD  valign=top align=right><B>Metadata:</B></TD><TD COLSPAN=8>\n");
	  strcpy(tmpCollName, Sentries->entries[2].val);
	  strcpy(tmpDataName, Sentries->entries[1].val);
	  strcpy(Sentries->entries[2].val,collList);
	  strcpy(Sentries->entries[1].val,dataList);
	  showMeta(Sentries);
	  strcpy(Sentries->entries[2].val,tmpCollName);
	  strcpy(Sentries->entries[1].val,tmpDataName);
	  fprintf (stdout,"</TD></TR><TR><TD COLSPAN=9><HR NOSHADE></TD><TR>\n");fflush(stdout);
	}
      } /* else for directory shadow object */
        ownerList += MAX_DATA_SIZE;
        ownerDomnList += MAX_DATA_SIZE;
        sizeList += MAX_DATA_SIZE;
        replNumList += MAX_DATA_SIZE;
        verNumList += MAX_DATA_SIZE;
        timeList += MAX_DATA_SIZE;
        dataList += MAX_DATA_SIZE;
        dataTypeList += MAX_DATA_SIZE;
        rsrcName += MAX_DATA_SIZE;
    	containerName += MAX_DATA_SIZE;
#ifdef DATACOMMENTS
	dataComments += MAX_DATA_SIZE;
#endif
	if (Sentries->queryFunction)
	  udsmd5List += MAX_DATA_SIZE;
	collList += MAX_DATA_SIZE;
	pathNameList += MAX_DATA_SIZE;
	Sentries->rowCntr++;
    }
}



int
prtTicketDataLongWeb (inStruct Sentries, mdasC_sql_result_struct *myresult, int chkColl)
{
    int i;
    char *dataList, *sizeList, *ownerList, *ownerDomnList, *timeList, *replNumList, *rsrcName;
    char *collList, *containerName, *dataComments, *dataTypeList, *verNumList, *pathNameList;
    char tmpContainerName[MAX_TOKEN],printdName[ 2 * MAX_TOKEN];
    char *inCont;
    char tmpCollName[MAX_TOKEN], tmpDataName[MAX_TOKEN];

    dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
    dataTypeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
    sizeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[SIZE], dcs_aname[SIZE]);
    ownerList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_OWNER], dcs_aname[DATA_OWNER]);
    ownerDomnList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_OWNER_DOMAIN], dcs_aname[DATA_OWNER_DOMAIN]);
    timeList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[REPL_TIMESTAMP], dcs_aname[REPL_TIMESTAMP]);
    replNumList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_REPL_ENUM], dcs_aname[DATA_REPL_ENUM]);
    verNumList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_VER_NUM], dcs_aname[DATA_VER_NUM]);
    rsrcName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[PHY_RSRC_NAME], dcs_aname[PHY_RSRC_NAME]);
    containerName = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[CONTAINER_NAME], dcs_aname[CONTAINER_NAME]);
    pathNameList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);
#ifdef DATACOMMENTS
    dataComments = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_COMMENTS], dcs_aname[DATA_COMMENTS]);
#endif
    collList = (char *) getFromResultStruct(
	    (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    oddFlag = 0;
printf("BB\n");fflush(stdout);
	
    for (i = 0; i < myresult->row_count; i++) {
      strcpy(urlcollList,collList);
      strcpy(urldataList,dataList);
      strcpy(nonurlcollList,collList);
      strcpy(nonurldataList,dataList);
      escape_url(urlcollList);
      escape_url(urldataList);
	if (*containerName != '\0') {
	    inCont = "Yes";
	} else {
	    inCont = "No";
	}
	if (first) {
	    fprintf (stdout,"\n<TR BGCOLOR=\"#DDEFFF\"><TH>Function</TH><TH>Data Name</TH>\
<TH>Creation Time</TH><TH>Owner</TH><TH>Replica<BR>Number</TH><TH>Version<BR>Number</TH><TH>Size</TH><TH>%s</TH><TH>Resource</TH><TH>In Container</TH>\
</TR>\n", mySrbStrings[52]);
	    first = 0;
	}
	if (chkColl == CHK_COLL ) 
	  sprintf(printdName,"%s/%s",collList, dataList);
	else
	  strcpy(printdName,dataList);
	oddFlag = (oddFlag + 1) % 2;
	if (oddFlag) {
	  fprintf (stdout,"<TR bgcolor=\"#88BBFF\">");
	}
	else {
	  fprintf (stdout,"<TR bgcolor=\"#DDEEFF\">");
	}
        fprintf (stdout,"<TD nowrap><A HREF=\"mysrb331.cgi?function=ticketdownloadfile&dataname=%s&collection=%s&seek=0&copynum=%s&vernum=%s&owner=%s@%s&session=%s\" ><IMG SRC=\"%s/blue-ball.gif\" ALT=\"%s\"></A>\n",urldataList, urlcollList,replNumList,verNumList,ownerList ,ownerDomnList,Sentries->pidStr, webPath, mySrbStrings[88]);
        fprintf (stdout,"<A HREF=\"mysrb331.cgi?function=ticketmeta&dataname=%s&collection=%s&seek=0&copynum=%s&vernum=%s&owner=%s@%s&session=%s\" ><IMG SRC=\"%s/white-ball.gif\" ALT=\"Show Metadata\"></A>\n",urldataList, urlcollList,replNumList,verNumList,ownerList ,ownerDomnList,Sentries->pidStr, webPath);


	    fprintf (stdout,"<TD VALIGN=middle nowrap>");
        fprintf (stdout,"<A HREF=\"mysrb331.cgi?function=ticketfull&dataname=%s&collection=%s&seek=0&copynum=%s&datatype=&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\" ><IMG SRC=\"%s/docimg.gif\" HEIGHT=18 BORDER=0></A> \n", urldataList, urlcollList, replNumList,verNumList,ownerList ,ownerDomnList, Sentries->pidStr, Sentries->frameStr, webPath);
	fprintf (stdout,"<A HREF=\"mysrb331.cgi?function=ticketfull&dataname=%s&collection=%s&seek=0&copynum=%s&datatype=&vernum=%s&owner=%s@%s&session=%s\"  target=\"%s\" >%s </A>", urldataList, urlcollList, replNumList,verNumList,ownerList ,ownerDomnList, Sentries->pidStr, Sentries->frameStr, printdName );
#ifdef DATACOMMENTS
	if (strlen(dataComments) > 0) 
	fprintf (stdout,"&nbsp;&nbsp;&nbsp;<IMG SRC=\"%s/commentsbut.gif\" ALT=\"%s\" width=18 height=18 border=0></A>\n", webPath, dataComments);
#endif



	fprintf (stdout,"</TD><TD nowrap> %s </TD>\
<TD nowrap align=center> %s@%s </TD><TD align=center> %s </TD><TD align=center> %s </TD><TD align=right>&nbsp;&nbsp;&nbsp;%s</TD><TD nowrap align=center> %s </TD><TD nowrap align=center> %s </TD><TD align=center> %s </TD></TR>\n",
		 timeList, ownerList ,ownerDomnList, replNumList, verNumList,sizeList,dataTypeList,rsrcName, inCont);

	if (showAllMetaFlag == 1) {
	  fprintf (stdout,"<TR>");
	  fprintf (stdout,"<TD  valign=top align=right><B>Metadata:</B></TD><TD COLSPAN=8>\n");
	  strcpy(tmpCollName,Sentries->entries[2].val);
	  strcpy(tmpDataName,Sentries->entries[1].val);
	  strcpy(Sentries->entries[2].val,collList);
	  strcpy(Sentries->entries[1].val,dataList);
	  showMeta(Sentries);
	  strcpy(Sentries->entries[2].val,tmpCollName);
	  strcpy(Sentries->entries[1].val,tmpDataName);
	  fprintf (stdout,"</TD></TR><TR><TD COLSPAN=9><HR NOSHADE></TD><TR>\n");fflush(stdout);
	}

        ownerList += MAX_DATA_SIZE;
        ownerDomnList += MAX_DATA_SIZE;
        sizeList += MAX_DATA_SIZE;
        replNumList += MAX_DATA_SIZE;
	verNumList += MAX_DATA_SIZE;
        timeList += MAX_DATA_SIZE;
        dataList += MAX_DATA_SIZE;
        dataTypeList += MAX_DATA_SIZE;
        rsrcName += MAX_DATA_SIZE;
	containerName += MAX_DATA_SIZE;
#ifdef DATACOMMENTS
	dataComments += MAX_DATA_SIZE;
#endif
	collList += MAX_DATA_SIZE;
	pathNameList += MAX_DATA_SIZE;
    }
}



int make_like_for_mdasmysrb(char *dname)
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

int client_set_qval_data_cond(char *in_data_name, char qval[][MAX_TOKEN])
{
  char st1[MAX_OBJ_NAME_LENGTH], st2[MAX_OBJ_NAME_LENGTH], st[MAX_OBJ_NAME_LENGTH];
  char cval[MAX_OBJ_NAME_LENGTH];
  char *tmpDomnPtr;
  char *tt;
  int i, ii, j;

  
  strcpy(st,in_data_name);
  i = 0;
  while (st[0] ==  ' ') i++;
  if (st[i] != '^')
      { /* dataname at beginning */
        strcpy(qval[DATA_NAME], " like '");
        j = 7;
        while (st[i] != '^' && i < strlen(st))
          {
            qval[DATA_NAME][j] = st[i];
            i++; j++;
          }
        qval[DATA_NAME][j] = '\'';
        qval[DATA_NAME][j+1] = '\0';
        strcpy(st, (char *) &st[i]);
      }
 while (strlen(st) > 0)
        {
          splitbycharwesc(st, st1, st2, '^' , '\\');
          tt = (char *) st2;
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
          else if(strstr((char *) &tt[0],"DVERSION") == (char *) &tt[0])
            sprintf(qval[DATA_VER_NUM],  "%s",(char *) &tt[4]);
          else if(strstr((char *) &tt[0],"DTYPE") == (char *) &tt[0])
            sprintf(qval[DATA_TYP_NAME], "%s", (char *) &tt[5]);
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
          else if(strstr((char *) &tt[0],"ANNOTATION") == (char *) &tt[0])
            sprintf(qval[DATA_ANNOTATION],  "%s",(char *) &tt[10]);
          else if(strstr((char *) &tt[0],"ANNOTATOR") == (char *) &tt[0]){
            tmpDomnPtr = strstr((char *) &tt[0],"@");
            if (tmpDomnPtr !=NULL) {
                *tmpDomnPtr = '\0';
                tmpDomnPtr++;
            }
            sprintf(qval[DATA_ANNOTATION_USERNAME],  "%s'",(char *) &tt[9]);
            sprintf(qval[DATA_ANNOTATION_USERDOMAIN]," = '%s", tmpDomnPtr);
          }
          else if(strstr((char *) &tt[0],"DATAMETANAME") == (char *) &tt[0])
            sprintf(qval[UDSMD0],  "%s",(char *) &tt[12]);
          else if(strstr((char *) &tt[0],"DATAMETAVAL") == (char *) &tt[0])
            sprintf(qval[UDSMD1],  "%s",(char *) &tt[11]);
          else if(strstr((char *) &tt[0],"COLLMETANAME") == (char *) &tt[0])
            sprintf(qval[UDSMD_COLL0],  "%s",(char *) &tt[12]);
          else if(strstr((char *) &tt[0],"COLLMETAVAL") == (char *) &tt[0])
            sprintf(qval[UDSMD_COLL1],  "%s",(char *) &tt[11]);
          else if(strstr((char *) &tt[0],"TICKET") == (char *) &tt[0])
            {
              ii = 6;
              while (tt[ii] == ' ' || tt[ii] == '=') ii++;
              if (tt[ii+1] == 'C' || tt[ii+1] == 'c')
                sprintf(qval[TICKET_C],  "%s",(char *) &tt[6]);
              else 
                sprintf(qval[TICKET_D],  "%s",(char *) &tt[6]);
            }
          else if(strstr((char *) &tt[0],"DATA") == (char *) &tt[0])
            sprintf(qval[DATA_NAME], "%s", (char *) &tt[4]);
          /* else if(strstr((char *) &tt[0],"") == (char *) &tt[0])
            sprintf(qval[],  "%s",(char *) &tt[]); */
          /* else if (strlen(st1) == 0)
             sprintf(qval[DATA_NAME], " = '%s'", tt); */
/*        else return(ERROR_IN_QUERY_CONDITION); */
          strcpy(st,st1);
        }
 return(0);
}

char*
substituteString(char *datastr, char *instr, char *outstr)
{
  char *tmpS, *myStr, *tmpS2;
  int i,j,k,l;
  
  myStr = datastr;
  if ((tmpS2 = strstr(myStr,instr)) != NULL) {
    l =  strlen(datastr);
    if (strlen(instr) < strlen(outstr)) 
      tmpS = (char *) malloc(sizeof(char) * l * 2);
    else 
      tmpS = (char *) malloc((sizeof(char) * l )  + 2);
    tmpS[0] ='\0';
    *tmpS2 = '\0';
    strcat(tmpS,myStr);
    strcat(tmpS,outstr);
    *tmpS2 = instr[0];
    myStr = tmpS2 + strlen(instr);
    while ((tmpS2 = strstr(myStr,instr)) != NULL) {
      *tmpS2 = '\0';
      strcat(tmpS,myStr);
      strcat(tmpS,outstr);
      *tmpS2 = instr[0];
      myStr = tmpS2 + strlen(instr);
    }
    strcpy(datastr,tmpS);
    free(tmpS);
  }
  return(datastr);
}

char *
simple_escape_url(char *instr)
{
  char *tmpS;
  int i,j,k,l;

  l =  strlen(instr);
  tmpS = (char *) malloc(sizeof(char) * l * 2);
  j= 0;
  for (i = 0; i < l; i++){
    if (instr[i] == '\"' 
        ) { 
      tmpS[j++] = '%';
      sprintf(&tmpS[j],"%X", instr[i]);
      j += 2;
    }
    else {
      tmpS[j++] = instr[i];
    }
  }
  tmpS[j] ='\0';
  strcpy(instr, tmpS);
  return(tmpS);
}

char *
escape_url(char *instr)
{
  char *tmpS;
  int i,j,k,l;

  l =  strlen(instr);
  tmpS = (char *) malloc(sizeof(char) * l * 2);
  j= 0;
  for (i = 0; i < l; i++){
    if (instr[i] == ' ' ||
        instr[i] == '<' ||
        instr[i] == '#' ||
        instr[i] == '>' ||
        instr[i] == '_' ||
        instr[i] == '+' ||
        instr[i] == '%' ||
        instr[i] == '&' ||
        instr[i] == '@' ||
        instr[i] == '\?' ||
        instr[i] == '\'' ||
        instr[i] == '\"' 
        ) { 
      tmpS[j++] = '%';
      sprintf(&tmpS[j],"%X", instr[i]);
      j += 2;
    }
    else {
      tmpS[j++] = instr[i];
    }
  }
  tmpS[j] ='\0';
  strcpy(instr, tmpS);
  return(tmpS);
}

int
getValuesAsSelectWithCond(inStruct Sentries, int index,  char *defaultItem, char *stopList, char *selectCond)
{
  mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, first;
    int status;
    char *tmpList, *origTmpList;
    char inItem[MAX_TOKEN];
    char *data = Sentries->selectData;
    
   strcpy(data,"");
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[index] = 1;
   strcpy(qval[index],selectCond);
   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      5*MAX_RESULT_NUM);
   if (status < 0 || myResult.row_count == 0) {
     if (defaultItem != NULL) 
       sprintf(data,"<SELECT SIZE=1 NAME=c1><OPTION SELECTED> %s</SELECT>",
	       defaultItem);
     else
       sprintf(data,"<SELECT SIZE=1 NAME=c1><OPTION SELECTED></SELECT>");
     return -1;
   }
    tmpList = (char *) getFromResultStruct(
      &myResult,
      dcs_tname[index], dcs_aname[index]);
    origTmpList = tmpList;
    first = 1;
    sprintf(data,"<SELECT SIZE=1 NAME=c1>");
    if (defaultItem != NULL) {
      strcat(data,"<OPTION SELECTED> ");
      strcat(data,defaultItem);
      first = 0;
    }
    for ( i = 0; i < myResult.row_count; i++) {
      sprintf(inItem,"#%s#",tmpList);
      if (strstr(stopList,inItem) == NULL) {
	if (first )
	  { strcat(data,"<OPTION SELECTED>");first = 0;}
	else
	  strcat(data,"<OPTION> ");
	strcat(data,tmpList);
      }
      tmpList += MAX_DATA_SIZE;
   }
   strcat(data,"</SELECT>");
   free(origTmpList);
   return 0;
}

getExtSchemaAttrListAsSelect(inStruct Sentries, char *data, char *schemaCondition)
{
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, first;
    int status;
    char *tmpList, *origTmpList;
    char *tmpList2, *origTmpList2;


    for (i = 0; i < MAX_DCS_NUM; i++) {
      sprintf(qval[i],"");
      selval[i] = 0;
    }
    selval[EXTENSIBLE_SCHEMA_NAME] = 1;
    selval[EXTENSIBLE_ATTR_OUTSIDE_NAME] = 1;
    sprintf(qval[EXTENSIBLE_SCHEMA_NAME],"in ( %s ) ", schemaCondition);
    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
				     MAX_EXT_TABLE_INDEX - MIN_EXT_TABLE_INDEX);
    if (status < 0 || myResult.row_count == 0) {
      return -1;
    }
    first = 1;
    sprintf(data,"<SELECT SIZE=1 NAME=esa1>");
    strcat(data,"<OPTION SELECTED>  ");
    tmpList = (char *) getFromResultStruct( &myResult,
	   dcs_tname[EXTENSIBLE_SCHEMA_NAME], dcs_aname[EXTENSIBLE_SCHEMA_NAME]);
    tmpList2 = (char *) getFromResultStruct( &myResult,
	   dcs_tname[EXTENSIBLE_ATTR_OUTSIDE_NAME], dcs_aname[EXTENSIBLE_ATTR_OUTSIDE_NAME]);
    origTmpList = tmpList;
    origTmpList2 = tmpList2;
    
    for ( i = 0; i < myResult.row_count; i++) {
      if (strlen(tmpList2) > 0) {
	strcat(data,"<OPTION> ");
	strcat(data,tmpList);
	strcat(data,".");
	strcat(data,tmpList2);
      }
      tmpList += MAX_DATA_SIZE;
      tmpList2 += MAX_DATA_SIZE;
    }
    strcat(data,"</SELECT>");
   free(origTmpList);
   free(origTmpList2);
   return 0;
}
      


getExtSchemaNameTableNameAsSelect(inStruct Sentries, char *data, char *defaultItem)
{
  mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, first;
    int status;
    char *tmpList, *origTmpList;
    char *tmpList2, *origTmpList2;

    
   strcpy(data,"");
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[EXTENSIBLE_SCHEMA_NAME]=1;
   selval[EXTENSIBLE_TABLE_NAME] = 1;

   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      MAX_RESULT_NUM);
   if (status < 0 || myResult.row_count == 0) {
     return -1;
   }
   first = 1;
   sprintf(data,"<SELECT SIZE=1 NAME=c1>");    
   if (defaultItem != NULL) {
       strcat(data,"<OPTION SELECTED> ");
       strcat(data, defaultItem);
       first = 0;
   }
   tmpList = (char *) getFromResultStruct( &myResult,
					   dcs_tname[EXTENSIBLE_SCHEMA_NAME], dcs_aname[EXTENSIBLE_SCHEMA_NAME]);
    origTmpList = tmpList;
    tmpList2 = (char *) getFromResultStruct( &myResult,
                                           dcs_tname[EXTENSIBLE_TABLE_NAME], dcs_aname[EXTENSIBLE_TABLE_NAME]);
    origTmpList2 = tmpList2;

    for ( i = 0; i < myResult.row_count; i++) {
      if (first )
	{ strcat(data,"<OPTION SELECTED>");first = 0;}
      else
	strcat(data,"<OPTION> ");
      strcat(data,tmpList);
      strcat(data,"->");
      strcat(data,tmpList2);
      tmpList += MAX_DATA_SIZE;
      tmpList2 += MAX_DATA_SIZE;

    }
   strcat(data,"</SELECT>");
   free(origTmpList);
   free(origTmpList2);
   return 0;
}

getExtSchemaNamesAsCheckBox(inStruct Sentries, char *data, char *defaultItem)
{
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, first;
    int status;
    char *tmpList, *origTmpList;


   strcpy(data,"");
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[EXTENSIBLE_SCHEMA_NAME] = 1;
   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
				   MAX_RESULT_NUM);
   if (status < 0 || myResult.row_count == 0) {
     return -1;
   }
   tmpList = (char *) getFromResultStruct( &myResult,
					   dcs_tname[EXTENSIBLE_SCHEMA_NAME], dcs_aname[EXTENSIBLE_SCHEMA_NAME]);
   origTmpList = tmpList;
   for ( i = 0; i < myResult.row_count; i++) {
     strcat(data,"<input type=\"checkbox\" name=\"schname\"  value=\"");
     strcat(data,tmpList);
     strcat(data,"\"> ");
     strcat(data,tmpList);
     strcat(data,"<BR>");
     tmpList += MAX_DATA_SIZE;
   }
   free(origTmpList);
   return 0;
}

getUDSMD0AsSelect(inStruct Sentries, char *data, char *defaultItem, char *collName)
{
  mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, first;
    int status;
    char *tmpList, *origTmpList;

    
   strcpy(data,"");
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[UDSMD0] = 1;
   sprintf(qval[COLLECTION_NAME]," like '%s%%' ",collName);

   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      MAX_RESULT_NUM);
   if (status < 0 || myResult.row_count == 0) {
     return -1;
   }
   first = 1;
   sprintf(data,"<SELECT SIZE=1 NAME=c1>");    
   if (defaultItem != NULL) {
       strcat(data,"<OPTION SELECTED> ");
       strcat(data, defaultItem);
       first = 0;
   }
   tmpList = (char *) getFromResultStruct(
      &myResult,
      dcs_tname[UDSMD0], dcs_aname[UDSMD0]);
    origTmpList = tmpList;
    for ( i = 0; i < myResult.row_count; i++) {
      if (first )
	{ strcat(data,"<OPTION SELECTED>");first = 0;}
      else
	strcat(data,"<OPTION> ");
      strcat(data,tmpList);
      tmpList += MAX_DATA_SIZE;
    }
   strcat(data,"</SELECT>");
   free(origTmpList);
   return 0;
}

getUDSMD_COLL0AsSelect(inStruct Sentries, char *data, char *defaultItem, char *collName)
{
  mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, first;
    int status;
    char *tmpList, *origTmpList;

    
   strcpy(data,"");
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[UDSMD_COLL0] = 1;
   sprintf(qval[COLLECTION_NAME]," like '%s%%' ",collName);

   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      MAX_RESULT_NUM);
   if (status < 0 || myResult.row_count == 0) {
     return -1;
   }
   first = 1;
   sprintf(data,"<SELECT SIZE=1 NAME=c1>");    
   if (defaultItem != NULL) {
       strcat(data,"<OPTION SELECTED> ");
       strcat(data, defaultItem);
       first = 0;
   }
   tmpList = (char *) getFromResultStruct(
      &myResult,
      dcs_tname[UDSMD_COLL0], dcs_aname[UDSMD_COLL0]);
    origTmpList = tmpList;
    for ( i = 0; i < myResult.row_count; i++) {
      if (first )
	{ strcat(data,"<OPTION SELECTED>");first = 0;}
      else
	strcat(data,"<OPTION> ");
      strcat(data,tmpList);
      tmpList += MAX_DATA_SIZE;
    }
   strcat(data,"</SELECT>");
   free(origTmpList);
   return 0;
}

getRsrcOfTypeAsSelect(inStruct Sentries, char *data, char *type)
{
  mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, first;
    int status;
    char *tmpList, *origTmpList;

    
   strcpy(data,"");
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[RSRC_NAME] = 1;
   sprintf(qval[PHY_RSRC_TYP_NAME],"  %s ",type);
   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      MAX_RESULT_NUM);
   if (status < 0 || myResult.row_count == 0) {
     return -1;
   }
    tmpList = (char *) getFromResultStruct(
      &myResult,
      dcs_tname[RSRC_NAME], dcs_aname[RSRC_NAME]);
    origTmpList = tmpList;
    first = 1;
    sprintf(data,"<SELECT SIZE=1 NAME=c1>");
    for ( i = 0; i < myResult.row_count; i++) {
      if (first )
	{ strcat(data,"<OPTION SELECTED>");first = 0;}
      else
	strcat(data,"<OPTION> ");
      strcat(data,tmpList);
      tmpList += MAX_DATA_SIZE;
    }
   strcat(data,"</SELECT>");
   free(origTmpList);
   return 0;
}

getExtractStyleForm(inStruct Sentries, char *data, char *dataType)
{
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, first;
    int status;
    char *dataList, *origDataList;
    char *collList, *origCollList;

   first = 1;
   strcpy(data,"");
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[COLLECTION_NAME] = 1;
   selval[DATA_NAME] = 1;
   sprintf(qval[UDSMD0]," = 'Meta Data Extraction Method for Data Type'");
   sprintf(qval[UDSMD1]," = '%s'",dataType);
   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      MAX_RESULT_NUM);
   if (status < 0 || myResult.row_count == 0) {
     return -1;
   }
   dataList = (char *) getFromResultStruct(
       &myResult,
       dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
   collList = (char *) getFromResultStruct(
       &myResult,
       dcs_tname[COLLECTION_NAME], dcs_aname[COLLECTION_NAME]);
   origDataList = dataList;
   origCollList = collList;
   sprintf(data,"<SELECT SIZE=1 NAME=metex1>");
   for ( i = 0; i < myResult.row_count; i++) {
     if (first )
       { strcat(data,"<OPTION SELECTED>");first = 0;}
     else
       strcat(data,"<OPTION> ");
     strcat(data,collList);
     strcat(data,"/");
     strcat(data,dataList);
     dataList += MAX_DATA_SIZE;
     collList += MAX_DATA_SIZE;
   }
   strcat(data,"</SELECT>");
   free(origDataList);
   free(origCollList);
   return 0;
}


getAccsCollectionsAsSelect(inStruct Sentries, char *accs, char *defaultItem)
{
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, first;
    int status;
    char *tmpList, *origTmpList;
    char *data = Sentries->selectData;
    
   strcpy(data,"");
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[ACCESS_COLLECTION_NAME] = 1;
   if (accs != NULL && strlen(accs) > 0) {
   	sprintf(qval[COLLECTION_ACCESS_CONSTRAINT]," = '%s' ",accs);
  	sprintf(qval[USER_NAME], " = '%s'", Sentries->srbUser);
   	sprintf(qval[DOMAIN_DESC], " = '%s'", Sentries->userDomain);
   }
   else {
   	sprintf(qval[COLLECTION_ACCESS_CONSTRAINT], "like '%%'");
  	sprintf(qval[USER_NAME], " = '%s'", Sentries->srbUser);
   	sprintf(qval[DOMAIN_DESC], " = '%s'", Sentries->userDomain);

   }
   
   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult, 5*MAX_RESULT_NUM);

   if (status < 0 || myResult.row_count == 0) {
    if (defaultItem) {
            sprintf(data,"<SELECT SIZE=1 NAME=colaccs1><OPTION SELECTED>%s", defaultItem);
        }
        else {
            sprintf(data,"<SELECT SIZE=1 NAME=colaccs1>");    
        }

     strcat(data,"</SELECT>");
     return -1;
   }

   tmpList = (char *) getFromResultStruct(
       &myResult,
       dcs_tname[ACCESS_COLLECTION_NAME], dcs_aname[ACCESS_COLLECTION_NAME]);
   origTmpList = tmpList;

   if (defaultItem != NULL) {
      sprintf(data,"<SELECT SIZE=1 NAME=c1><OPTION SELECTED> %s ",defaultItem);
      i =0;
    }
    else {
      sprintf(data,"<SELECT SIZE=1 NAME=c1><OPTION SELECTED> %s",tmpList);
      tmpList += MAX_DATA_SIZE;
      i = 1;
    }
   
   for (; i < myResult.row_count; i++) {

      if (!strstr(tmpList,"$deleted") && strstr(tmpList,"/trash/")!=tmpList )  {  
          strcat(data,"<OPTION> ");
          strcat(data,tmpList);
      }
      
      tmpList += MAX_DATA_SIZE;
   }

      /* changed here to add more rows */
   while (myResult.continuation_index >= 0) {
        clearSqlResult (&myResult);       

        if (status < 0 || myResult.row_count == 0) {
             strcat(data,"</SELECT>");
             return -1;
        }
        
        tmpList = (char *) getFromResultStruct(
           &myResult,
           dcs_tname[ACCESS_COLLECTION_NAME], dcs_aname[ACCESS_COLLECTION_NAME]);
        origTmpList = tmpList;
       
        for (i =0; i < myResult.row_count; i++) {
        
          if (!strstr(tmpList,"$deleted") && strstr(tmpList,"/trash/")!=tmpList )  {  
              strcat(data,"<OPTION> ");
              strcat(data,tmpList);
          }
          
          tmpList += MAX_DATA_SIZE;
        }
    }
   
   strcat(data,"</SELECT>");
   free(origTmpList);
   return 0;
}


int
getValuesAsSelect(inStruct Sentries, int index, char *data, char *defaultItem, char *stopList)
{
  mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, first;
    int status;
    char *tmpList, *origTmpList;
    char inItem[MAX_TOKEN];
    
   strcpy(data,"");
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[index] = 1;
   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      5*MAX_RESULT_NUM);
   if (status < 0 || myResult.row_count == 0)  {
     if (defaultItem != NULL) 
       sprintf(data,"<SELECT SIZE=1 NAME=c1><OPTION SELECTED> %s</SELECT>",
	       defaultItem);
     else
       sprintf(data,"<SELECT SIZE=1 NAME=c1><OPTION SELECTED></SELECT>");
     return -1;
   }
   
    tmpList = (char *) getFromResultStruct(
      &myResult,
      dcs_tname[index], dcs_aname[index]);
    origTmpList = tmpList;
    first = 1;
    sprintf(data,"<SELECT SIZE=1 NAME=c1>");
    if (defaultItem != NULL) {
      strcat(data,"<OPTION SELECTED> ");
      strcat(data,defaultItem);
      first = 0;
    }
    for ( i = 0; i < myResult.row_count; i++) {
      sprintf(inItem,"#%s#",tmpList);
      if (strstr(stopList,inItem) == NULL) {
	if (first)
	  { strcat(data,"<OPTION SELECTED>");first = 0;}
	else
	  strcat(data,"<OPTION> ");
	strcat(data,tmpList);
      }
      tmpList += MAX_DATA_SIZE;
   }
   strcat(data,"</SELECT>");
   free(origTmpList);
   return 0;
}


int 
getPathName(inStruct Sentries, char *collName, char *dataName, int replNum, char *pathName)
{
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, status;
    char *nameList;

   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[PATH_NAME] = 1;
   sprintf(qval[DATA_NAME],"= '%s'",dataName);
   sprintf(qval[DATA_REPL_ENUM],"= %i",replNum);
   sprintf(qval[DATA_GRP_NAME],"= '%s'",collName);

   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      MAX_RESULT_NUM);
   if (status < 0 || myResult.row_count == 0) 
     return -1;
   
    nameList = (char *) getFromResultStruct(
      &myResult,
      dcs_tname[PATH_NAME], dcs_aname[PATH_NAME]);
    strcpy(pathName, nameList);
   free(nameList);
   return 0;
}

int
getUserAsSelect(inStruct Sentries, char *defaultItem)
{
  mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    char *nameList, *domList, *origNameList, *origDomList;
    char *data = Sentries->selectData;
    
   sprintf(data,"");
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[USER_NAME] = 1;
   selval[DOMAIN_DESC] = 1;
   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      MAX_RESULT_NUM);
   if (status < 0 || myResult.row_count == 0) 
     return -1;
   
    nameList = (char *) getFromResultStruct(
      &myResult,
      dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
    domList = (char *) getFromResultStruct(
      &myResult,
      dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
    origNameList = nameList;
    origDomList = domList;
    
    if (defaultItem != NULL) {
      sprintf(data,"<SELECT SIZE=1 NAME=c1><OPTION SELECTED> %s ",defaultItem);
      i =0;
    }
    else {
      sprintf(data,"<SELECT SIZE=1 NAME=c1><OPTION SELECTED> %s@%s ",nameList,domList);
      nameList += MAX_DATA_SIZE;
      domList += MAX_DATA_SIZE;
      i = 1;
    }
   for (; i < myResult.row_count; i++) {
      strcat(data,"<OPTION>");
      strcat(data,nameList);
      strcat(data,"@");
      strcat(data,domList);
      nameList += MAX_DATA_SIZE;
      domList += MAX_DATA_SIZE;
   }

      /* changed here to add more rows */
   while (myResult.continuation_index >= 0) {
     clearSqlResult (&myResult);
     status = srbGetMoreRows(Sentries->conn, 0, myResult.continuation_index,
			     &myResult, MAX_RESULT_NUM);
     if (myResult.result_count == 0  || status != 0)
       return;

     filterDeleted (&myResult);
     nameList = (char *) getFromResultStruct(
			       &myResult,
			       dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
     domList = (char *) getFromResultStruct(
      		       &myResult,
		       dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
     origNameList = nameList;
     origDomList = domList;
 
     for (i=0; i < myResult.row_count; i++) {

    /**********************************/          
        /*** must make sure there is enough memory allocated for selectData ***
        if ((strlen(data) + MAX_DATA_SIZE) >= Sentries->sizeOfSelectData ) {
             j = expandDataString(data, Sentries->sizeOfSelectData, 10*HUGE_STRING); 
             if (j==0) {
                    Sentries->sizeOfSelectData += 10*HUGE_STRING;
             }
        }        
    /**********************************/
       
        strcat(data,"<OPTION>");
        strcat(data,nameList);
        strcat(data,"@");
        strcat(data,domList);
     
        nameList += MAX_DATA_SIZE;
        domList += MAX_DATA_SIZE;
	
     }
   }   
   strcat(data,"</SELECT>");
   free(origNameList);
   free(origDomList);
   return 0;
}

prtCurACL(inStruct Sentries)
{
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i;
    int status;
    char *nameList,*domList,*accsList,*origNameList,*origDomList,*origAccsList;
    char *dataName = Sentries->entries[1].val;
    char *collName = Sentries->entries[2].val;
    
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   if (strlen(dataName) > 0) {
     selval[USER_NAME] = 1;
     selval[DOMAIN_DESC] = 1;
     selval[ACCESS_CONSTRAINT] = 1;
     sprintf(qval[DATA_NAME], " = '%s'",dataName);
     sprintf(qval[DATA_GRP_NAME], " = '%s'",collName);
     status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      MAX_RESULT_NUM);
     if (status < 0 || myResult.row_count == 0) 
       return -1;
     nameList = (char *) getFromResultStruct(
		 &myResult,
		 dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
     domList = (char *) getFromResultStruct(
		 &myResult,
		 dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
     accsList = (char *) getFromResultStruct(
		 &myResult,
		 dcs_tname[ACCESS_CONSTRAINT], dcs_aname[ACCESS_CONSTRAINT]);
   }
   else {
     selval[USER_NAME] = 1;
     selval[DOMAIN_DESC] = 1;
     selval[GROUP_ACCESS_CONSTRAINT] = 1;
     sprintf(qval[ACCESS_COLLECTION_NAME], " = '%s'",collName);
     status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      MAX_RESULT_NUM);
     if (status < 0 || myResult.row_count == 0) 
       return -1;
     nameList = (char *) getFromResultStruct(
		 &myResult,
		 dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
     domList = (char *) getFromResultStruct(
		 &myResult,
		 dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
     accsList = (char *) getFromResultStruct(
		 &myResult,
		 dcs_tname[GROUP_ACCESS_CONSTRAINT], dcs_aname[GROUP_ACCESS_CONSTRAINT]);
   }
   origNameList = nameList;
   origDomList = domList;
   origAccsList = accsList;
   fprintf(stdout, "<TABLE ALIGN=CENTER BORDER=2 CELLPADDING=2><TR><TH>User Name</TH><TH>Access</TH></TR>\n");
   
   for (i = 0; i < myResult.row_count; i++) {
     fprintf(stdout, "<TR><TD>%s@%s</TD><TD>%s</TD></TR>\n",nameList,domList,accsList);
     nameList += MAX_DATA_SIZE;
     domList += MAX_DATA_SIZE;
     accsList += MAX_DATA_SIZE;
   }
   
   while (myResult.continuation_index >= 0) {
        clearSqlResult (&myResult);
        status = srbGetMoreRows(Sentries->conn, 0, myResult.continuation_index,
        	     &myResult, MAX_RESULT_NUM);
  			     
        if (myResult.result_count == 0  || status != 0) {
            break;
        }
        
        if (strlen(dataName) > 0) {      
            nameList = (char *) getFromResultStruct(
             &myResult,
             dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
            domList = (char *) getFromResultStruct(
             &myResult,
             dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
            accsList = (char *) getFromResultStruct(
             &myResult,
             dcs_tname[ACCESS_CONSTRAINT], dcs_aname[ACCESS_CONSTRAINT]);   
        }
        else {
            nameList = (char *) getFromResultStruct(
            	 &myResult,
            	 dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
            domList = (char *) getFromResultStruct(
            	 &myResult,
            	 dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
            accsList = (char *) getFromResultStruct(
            	 &myResult,
            	 dcs_tname[GROUP_ACCESS_CONSTRAINT], dcs_aname[GROUP_ACCESS_CONSTRAINT]);
        }

        for (i = 0; i < myResult.row_count; i++) {
             fprintf(stdout, "<TR><TD>%s@%s</TD><TD>%s</TD></TR>\n",nameList,domList,accsList); fflush(stdout);
             nameList += MAX_DATA_SIZE;
             domList += MAX_DATA_SIZE;
             accsList += MAX_DATA_SIZE;
        }
 
   }
   fprintf(stdout, "</TABLE>\n"); fflush(stdout);
   /*** seems that with clearSqlResult() and this something gets freed twice ****
   free(origNameList);
   free(origDomList);
   free(origAccsList);
   /****************************************************************************/
   return 0;

}

int
writeRegLog(inStruct Sentries)
{
  char sessionId[MAX_TOKEN];
  char   timeval1[MAX_TOKEN];
  get_time(timeval1);
  sprintf(sessionId,"%s/serverlog/%s.%s", logDirec,timeval1, Sentries->entries[1].val);
  fId = fopen(sessionId,"w");
  if (fId == NULL) {
    sprintf(errorMsg,"Unable to Open Session");
    return(-errno);
  }
  i = fprintf(fId,"Date=%s\nName=%s\nAddress=%s\nEmail=%s\nPhone=%s\nHost=%s\n",timeval1,
	      Sentries->entries[1].val, Sentries->entries[2].val , 
	      Sentries->entries[3].val, Sentries->entries[4].val,
	      Sentries->remoteHost);
  if (i < 0) {
    sprintf(errorMsg,"Write Error in Log");
    return(-errno);
  }
  fclose( fId );
  return(0);
}


int
reputConnInfo(inStruct Sentries) {
  int i,j,k;
  char timeval1[MAX_TOKEN];
  char sessionId[MAX_TOKEN];
  char u[20][MAX_TOKEN];
  sprintf(sessionId,"%s/%s",logDirec, Sentries->cookieStr);
  fId = fopen(sessionId,"r");
  if (fId == NULL) {
    sprintf(Sentries->errorMsg,"Unable to Get Session Info. Please login.");
    return(-errno);
  }
  /*charliehack - up from 9*/
  for (i = 0; i < MAX_U_ENTRIES; i++) {
    fgets(u[i],MAX_TOKEN,fId);
    u[i][strlen(u[i])-1] = '\0';
  }
  
  strcpy(u[12], Sentries->currentMcatZone);
    
 /*** pretty heavy to close and re-open the file, but better for portability ***/  
  fclose(fId);
  fId = fopen(sessionId,"w");  
    
  for (i = 0; i < MAX_U_ENTRIES; i++) {
    j = fprintf(fId,"%s\n",u[i]);
    if (j <= 0) {
      sprintf(errorMsg,"Unable to Create Session");
      fclose( fId );
      return(j);
    }
  }
  if (i <= 0) {
    sprintf(errorMsg,"Unable to Create Session");
    fclose( fId );
    return(i);
  }
  fclose(fId);
  return(0);

}

int putConnInfo (inStruct Sentries) 
{
  char timeval1[MAX_TOKEN];
  char sessionId[MAX_TOKEN];
  char *tmpPtr;
  int i;  

  get_time(timeval1);
sprintf(sessionId,"%s/%s.%ld",logDirec,timeval1,getpid());
  fId = fopen(sessionId,"w");
  sprintf(sessionId,"%s.%ld",timeval1,getpid());
  if (fId == NULL) {
    sprintf(errorMsg,"Unable to Open Session");
    return(-errno);
  }
  if (Sentries->currentMcatZone == NULL)
     tmpPtr = strdup("NULL");
  else
     tmpPtr = Sentries->currentMcatZone;
 
  i = fprintf(fId,"%s\n%s\n%s\n%s\n%s\n%ld\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",sessionId,timeval1,
	      Sentries->entries[1].val, Sentries->entries[2].val , Sentries->entries[3].val, 
	      getpid(), Sentries->remoteHost, Sentries->entries[9].val, Sentries->entries[4].val, Sentries->entries[6].val, Sentries->entries[8].val, Sentries->entries[7].val, tmpPtr, Sentries->userHomeZone,Sentries->userConnectionZone);
  if (i <= 0) {
    sprintf(errorMsg,"Unable to Create Session");
    return(i);
  }
  fclose( fId );
  strcpy(Sentries->cookieStr, sessionId);
  sprintf(Sentries->pidStr,"%ld",getpid());
  sprintf(Sentries->frameStr,"_top");
  return(0);
}


int readLangFile(char *langDir, char *currentLanguage) {
/* opens language file and sets values to mySrbStrings*/

  char langFile[MAX_TOKEN];
  char buf[HUGE_STRING];
  int cptr=0;
 
  if (!strlen(currentLanguage)) 
    sprintf(langFile,"%s/mySrbLang_English",langDir);
  else
    sprintf(langFile,"%s/mySrbLang_%s",langDir, currentLanguage);  
  
  fId = fopen(langFile,"r");
  if (fId==NULL) {
    sprintf(errorMsg,"Unable to open language file");
    return(-errno);
  }

  while(getc(fId)!=EOF) {
  fseek(fId, -1, SEEK_CUR);
  fgets(buf,HUGE_STRING,fId);
  mySrbStrings[cptr++]=strdup(buf);
  }
  fclose(fId);

  return(0);
}



int
loginUser(inStruct Sentries)
{
	int i;
	
	if(getenv("REMOTE_ADDR") == NULL)
	{
		printf("Content-type: text/html%c%c",10,10);fflush(stdout);
		printf("<html>");
		printf("<body background=\"%s/srb3.jpg\">", webPath);
		printf("<br>Error getting Remote Host. Disconnecting\n");
		fflush(stdout);
		printf("</body></html>");
		exit(1);    
	}
	strcpy(Sentries->remoteHost,getenv("REMOTE_ADDR"));
	if(strlen(Sentries->remoteHost) == 0)
	{
		printf("Content-type: text/html%c%c",10,10);fflush(stdout);
		printf("<html>");
		printf("<body background=\"%s/srb3.jpg\">", webPath);
		printf("<br>Error with Remote Host. Disconnecting\n");
		fflush(stdout);
		printf("</body></html>");
		exit(1);    
	}
/*
	printf("Content-type:	text/plain\n\n");
	for(i = 0; i < 4; i++)
	{
		printf("%d: %s\n", i, entries[i].val);
	}
*/
	if(	0 == strcmp("demouser", Sentries->entries[1].val) &&
		0 == strcmp("npaci", Sentries->entries[2].val) &&
		0 == strcmp("DEFAULT", Sentries->entries[4].val) &&
		0 == strcmp("srb.sdsc.edu", Sentries->entries[6].val))
	{
		sprintf(Sentries->entries[3].val,"%s", "SC99D");
	}


     sprintf(Sentries->envstr,"srbUser=%s", Sentries->entries[1].val);
     putenv(Sentries->envstr);
     sprintf(Sentries->envstr2,"mdasDomainName=%s", Sentries->entries[2].val );
     putenv(Sentries->envstr2);
     if (strlen(Sentries->entries[4].val) > 0 && strcmp(Sentries->entries[4].val,"DEFAULT"))
	{
        sprintf(Sentries->envstr3,"srbPort=%s", Sentries->entries[4].val);
        putenv(Sentries->envstr3);      
       strcpy(Sentries->srbPort,Sentries->entries[4].val );
     }
     else if (Sentries->SRBPORT != NULL && strcmp(Sentries->SRBPORT ,"NULL")){
        sprintf(Sentries->envstr3,"srbPort=%s", Sentries->SRBPORT );
        putenv(Sentries->envstr3);
        strcpy(Sentries->srbPort, Sentries->SRBPORT );
     }
     else 
       strcpy(Sentries->srbPort,"");
       
	/*charliechack
	Expect hostname to be entries[6], if no entries[6] then take default,
	which has already been stored in connectHostAddress
	*/

	if(Sentries->entries[6].val != NULL)
		sprintf(Sentries->connectHostAddress,"%s", Sentries->entries[6].val);

	/*
	printf("Content-type:	text/plain\n\n->%s<-\n", connectHostAddress);
	endcharliechack
	*/

	sprintf(Sentries->envstr4,"srbHost=%s", Sentries->connectHostAddress);
	putenv(Sentries->envstr4);


	/***    strcat(entries[3].val,"ER");***/
	if(strstr(Sentries->remoteHost,"141.248.") != NULL) {
	printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	printf("<html>");
	printf("<br>Connection to exfMaster failed.\n");
	printf("<body background=\"%s/srb3.jpg\">", webPath);
	printf("<br>Error in Connecting to SRB: <A href=\"srberror2.cgi?-3005\">Error: -3005</A>\n");
	fflush(stdout);
	printf("</body></html>");
	exit(1);
	}



#ifdef USE118LEAN
    Sentries->conn = clConnect (Sentries->connectHostAddress, NULL, Sentries->entries[3].val);
#else
	/*printf("Content-type: text/plain\n\nhost=%s\n%s\n%s\n%s\n", connectHostAddress, entries[3].val, entries[1].val, entries[2].val);*/
    Sentries->conn = srbConnect (Sentries->connectHostAddress, NULL, Sentries->entries[3].val, Sentries->entries[1].val, Sentries->entries[2].val, NULL, NULL);
	/*printf("status: %d\n", clStatus(Sentries->conn));*/

#endif
  if (clStatus(Sentries->conn) != CLI_CONNECTION_OK) {
    sleep(1);
#ifdef USE118LEAN
    Sentries->conn = clConnect (Sentries->connectHostAddress, NULL, Sentries->entries[3].val);
#else
    Sentries->conn = srbConnect (Sentries->connectHostAddress, NULL, Sentries->entries[3].val, 
     Sentries->entries[1].val, Sentries->entries[2].val, NULL, NULL);
#endif
    if (clStatus(Sentries->conn) != CLI_CONNECTION_OK) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      if (clStatus(Sentries->conn) == -1003) {
	printf("<br><FONT COLOR=#FF0000>Authentication Error.</FONT> Please check the password and resubmit.\n");
      }
      else if (clStatus(Sentries->conn) == -1109) {
	printf("<br><FONT COLOR=#FF0000>MySRB Server Error.</FONT> The MySRB Server seems to be unavailable. Please contact  <A HREF=\"mailto:%s\">%s</A> for further information.\n", emailAddress, emailAddress);
      }
      else {
	printf("<br><FONT COLOR=#FF0000>Connection to MySRB System failed.</FONT>\n");
	printf("<br>Error in Connecting to SRB: <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", clStatus(Sentries->conn),clStatus(Sentries->conn));
	printf("<br>Some Parameters used Connecting to SRB: Server=%s, Port=%s, User=%s \n",Sentries->connectHostAddress, Sentries->srbPort,  Sentries->entries[1].val);
	srb_perror (2, clStatus(Sentries->conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	printf("%s",clErrorMessage(Sentries->conn));      
      }
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(1);
    }
  }
  getUserHomeZone(Sentries);
  getUserConnectionZone(Sentries);
  clFinish(Sentries->conn);

  i = putConnInfo (Sentries);
   
   if (i < 0) {
     printf("Content-type: text/html%c%c",10,10);fflush(stdout);
     printf("<html>");
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf("<br>Error: %s<BR>\n<A href=\"srberror2.cgi?%i\">Error: %i</A>\n",Sentries->errorMsg,i,i);
     fflush(stdout);
     printf("</body></html>");
     exit(1);
   }
     
	if(!strcmp(Sentries->httptype,"secure"))
		printf("Set-Cookie:mySessionId%s=%s; secure\n",Sentries->pidStr,Sentries->cookieStr);
	else
		printf("Set-Cookie:mySessionId%s=%s\n",Sentries->pidStr,Sentries->cookieStr);
		


     if (!Sentries->startcol || !strcmp(Sentries->startcol,"")) {
	    if (Sentries->startzone && strcmp(Sentries->startzone,"")) {
            sprintf(Sentries->startcol, "/%s/home/%s.%s", Sentries->startzone, Sentries->entries[1].val, Sentries->entries[2].val);
	    }
	    else {
            sprintf(Sentries->startcol, "/home/%s.%s", Sentries->entries[1].val, Sentries->entries[2].val);	        
	    }
     }
 
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html><HEAD>\n<TITLE>%s</TITLE> <BASE TARGET = \"_top\"></HEAD>\n", mySrbStrings[22]);
      printf("<body background=\"%s/srb3.jpg\"><CENTER><B><FONT COLOR=#0000FF>%s</FONT></B><P>\n\
<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" >\n\
<input type=hidden name=\"function\" value=\"treefull\">\n\
<input type=hidden name=\"dataname\" value=\"\">\n\
&nbsp; &nbsp; <STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n\
<INPUT NAME=\"collection\", VALUE= \"%s\" , SIZE=30,12>\n<BR>\
&nbsp; &nbsp; <FONT COLOR=#0000FF>%s</FONT><BR> \n\
<INPUT type=hidden NAME=\"seek\", VALUE= \"0\">\n",
	 webPath, mySrbStrings[10], Sentries->pidStr, mySrbStrings[11], Sentries->startcol, mySrbStrings[12]);
  printf("<input type=hidden name=\"copynum\" value=\"0\">");
  printf("<INPUT NAME=\"datatype\" type=hidden VALUE=\"\">\n");
  printf("<INPUT NAME=\"vernum\" type=hidden VALUE=\"1\">\n");
  printf("<INPUT NAME=\"owner\" type=hidden VALUE=\"%s@%s\">\n", Sentries->entries[1].val, Sentries->entries[2].val);
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  printf("<INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[13]);


  /*printf("<P><CENTER><TABLE WIDTH=50%%><TR><TD BGCOLOR=\"#DDEFFF\" align=center >&nbsp;<P> */
    printf("<P><CENTER><TABLE WIDTH=50%%><TR><TD align=center >&nbsp;<P> \
<A HREF=\"%s/%s\" target=\"_self\"><IMG SRC=\"%s/%s\" ALT=\"Help\"><BR>%s</A><P>&nbsp;</TD>\n", webPath, mySrbStrings[56], webPath, "help.gif", mySrbStrings[14]);

 printf("<TD  align=center >&nbsp;<P> \
<A HREF=\"mysrb331.cgi?function=showoptions&username=%s&domainname=%s&startcol=%s&session=%s\" target=\"_self\" ><IMG SRC=\"%s/%s\" ALT=\"Exit MySRB\"><BR>%s</A><P>&nbsp;</TD>\n", Sentries->entries[1].val, Sentries->entries[2].val, Sentries->startcol, Sentries->pidStr, webPath, "opmenu.gif", mySrbStrings[17]);

    /*    printf("<TD BGCOLOR=\"#DDEFFF\" align=center >&nbsp;<P> */
    printf("<TD  align=center >&nbsp;<P> \
<A HREF=\"mysrb331.cgi?function=unmount&session=%s\" target=\"_self\" ><IMG SRC=\"%s/%s\" ALT=\"Exit MySRB\"><BR>%s</A><P>&nbsp;</TD></TABLE></CENTER>\n", Sentries->pidStr, webPath, "exitmysrb.gif", mySrbStrings[15]);

  printf("</BODY>\n</HTML>\n");

}

int 
makeUnMount(inStruct Sentries)
{
  int i;
  char sessionId[MAX_TOKEN];

    
  sprintf(sessionId,"%s/%s",logDirec,Sentries->cookieStr);
  fId = fopen(sessionId,"r");
  
  if (fId == NULL) {
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf ("<html><HEAD><TITLE> %s </TITLE> </HEAD>", "MYSRB V9.0");
    printf("<body background=\"%s/srb3.jpg\">", webPath);
    printf("<br>Error in Session Information\n");
    printf("<br>DisConnection Aborted\n");
    printf("</body></html>");
    fflush(stdout);
    exit(1);
  }
  fclose(fId);
  
   i =  unlink(sessionId);
   if(!strcmp(Sentries->httptype,"secure"))
		printf("Set-Cookie:mySessionId%s=; expires Thu, 01-Jan-2001 00:00:00 GMT; secure\n", Sentries->pidStr);
	else
		printf("Set-Cookie:mySessionId%s=; expires Thu, 01-Jan-2001 00:00:00 GMT\n", Sentries->pidStr);
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf ("<html><HEAD> <TITLE> %s </TITLE> </HEAD>",  "zoneSRB 3.0.1");
    if (!strcmp(mode,"light")) {
        printf("<body bgcolor=\"#FFFFF7\">");
    }
    else {
        printf("<body background=\"%s/srb3.jpg\">", webPath);
    } 
fflush(stdout);
   if (i < 0) 
     printf("<br>Error when DisConnecting from SRB Unlink Error:%i",i);
   else
     printf("<br>Exit myZoneSRB\n");
    fflush(stdout);printf("</body></html>\n");
    fflush(stdout);
    exit(1);
}


int
getConnInfo(char u[][MAX_TOKEN], inStruct Sentries) {
  int i,j,k;
  char timeval1[MAX_TOKEN];
  char sessionId[MAX_TOKEN];
  
  get_time(timeval1);
  sprintf(sessionId,"%s/%s",logDirec, Sentries->cookieStr);
  fId = fopen(sessionId,"r");
  if (fId == NULL) {
    sprintf(Sentries->errorMsg,"Unable to Get Session Info. Please login.");
    return(-errno);
  }
  /*charliehack - up from 9*/
  for (i = 0; i < MAX_U_ENTRIES; i++) {
    fgets(u[i],MAX_TOKEN,fId);
    u[i][strlen(u[i])-1] = '\0';
  }
  fclose(fId);
  if (strcmp(Sentries->cookieStr,u[0])) {
    unlink(sessionId);
    sprintf(Sentries->errorMsg,"Bad Session Id");
    return(INPUT_PROPERTY_ERROR);
  }
  for (i = 0 ; i < 11; i++)
  {
    if (timeval1[i] != u[1][i])
	{
      unlink(sessionId);
      sprintf(Sentries->errorMsg,"Session Timed Out. Please Login Again.");
      return(INPUT_PROPERTY_ERROR);
    }
  }
  j = (atoi((char *) &u[1][i]) * 60) + atoi((char *) &u[1][i+3]);
  k = (atoi((char *) &timeval1[i]) * 60) + atoi((char *) &timeval1[i+3]);
  /***  if ((k - j) > 60) {***/
  if ((k - j) > atoi(u[7])) {
    unlink(sessionId);
    sprintf(Sentries->errorMsg,"Session Timed Out. Please Login Again.");
    return(INPUT_PROPERTY_ERROR);
  }
  if (strcmp(u[6], Sentries->remoteHost)) {
    unlink(sessionId);
    sprintf(Sentries->errorMsg,"Error with Remote Host Name. Disconnecting.");
    return(INPUT_PROPERTY_ERROR);
  }
  
  /** strcpy(frameStr,u[5]); **/
//  strcpy(Sentries->frameStr,"_top");
    sprintf(Sentries->frameStr,"sub%s", Sentries->pidStr);  /* now changed as "_top" is the tree frame */
  
  return(0);

}

int
makeConnection(inStruct Sentries)
{
    char u[20][MAX_TOKEN];
	char addressStr[200];
	char* tmpptr;
    
    if (getenv("REMOTE_ADDR") == NULL) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<br>Error getting Remote Host. Disconnecting\n");
      fflush(stdout);
      printf("</body></html>");
      exit(1);    
    }
    strcpy(Sentries->remoteHost,getenv("REMOTE_ADDR"));
    if (strlen(Sentries->remoteHost) == 0) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<br>Error with Remote Host. Disconnecting\n");
      fflush(stdout);
      printf("</body></html>");
      exit(1);    
    }

    i = getConnInfo(u, Sentries);

    if (i < 0) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<br>Error: %s<BR>\n<A href=\"srberror2.cgi?%i\">Error: %i</A>",Sentries->errorMsg,i, i);
      fflush(stdout);
      printf("</body></html>");
      exit(1);
    }

    strcpy(Sentries->srbUser,u[2]);
    strcpy(Sentries->userDomain, u[3]);
    strcpy(Sentries->currentLanguage, u[10]);
    readLangFile(langDir, Sentries->currentLanguage);
    strcpy(Sentries->userHomeZone,u[13]);
    strcpy(Sentries->userConnectionZone,u[14]);
    if (!strcmp(u[12],"NULL"))
	Sentries->oldMcatZone = NULL;
    else
        Sentries->oldMcatZone = strdup(u[12]);
        
        
    if (Sentries->entries[2].val) { /*** NULL when makecontainerform is called ***/
       if (Sentries->entries[2].val[0] == '/') {
         if (strstr(Sentries->entries[2].val,"/home")==Sentries->entries[2].val ||
    	 strstr(Sentries->entries[2].val,"/container")==Sentries->entries[2].val ||
    	 strstr(Sentries->entries[2].val,"/srbtest")==Sentries->entries[2].val
    	 ) {
           Sentries->currentMcatZone =  Sentries->oldMcatZone;
         }
         else {
           Sentries->currentMcatZone = strdup(Sentries->entries[2].val + 1);
           if ((tmpPtr = strstr(Sentries->currentMcatZone,"/")) != NULL)
    	 *tmpPtr = '\0';
         }
       }
       else
         Sentries->currentMcatZone =  Sentries->oldMcatZone;
    }
   
    getMode(u[11]);
    sprintf(Sentries->envstr,"srbUser=%s", u[2]);
    putenv(Sentries->envstr);
    sprintf(Sentries->envstr2,"mdasDomainName=%s", u[3] );
    putenv(Sentries->envstr2);
    if (strlen(u[8]) == 0 || !strcmp(u[8],"DEFAULT"))
	{
      if (strlen(Sentries->SRBPORT) && strcmp(Sentries->SRBPORT ,"NULL"))
   /* if (SRBPORT != NULL && strcmp(SRBPORT ,"NULL"))  */
	  {
		sprintf(Sentries->envstr3,"srbPort=%s", Sentries->SRBPORT );
		putenv(Sentries->envstr3);
      }
    }
    else
	{
		/*u[8] is port
		  u[9] is host*/

		strcpy(Sentries->connectHostAddress, u[9]);
 
		strcpy(addressStr, u[8]);
		if((tmpptr = strstr(addressStr, ":")) == NULL)
		{
			sprintf(Sentries->envstr3,"srbPort=%s", u[8]);
			putenv(Sentries->envstr3);      
		}else
		{
			*tmpptr = '\0';
			strcpy(Sentries->connectHostAddress, addressStr);
			tmpPtr++;
			sprintf(Sentries->envstr3, "srbPort=%s", tmpptr);
			putenv(Sentries->envstr3);
		}


    }
    if (strlen(u[9]) > 0 && strcmp(u[9],"DEFAULT"))
      strcpy(Sentries->connectHostAddress, u[9]);
    sprintf(Sentries->envstr4,"srbHost=%s", Sentries->connectHostAddress);
    putenv(Sentries->envstr4);


#ifdef USE118LEAN

    Sentries->conn = clConnect (Sentries->connectHostAddress, NULL, u[4]);
#else
    Sentries->conn = srbConnect (Sentries->connectHostAddress, NULL, u[4],
     u[2], u[3], NULL, NULL);
#endif

    if (clStatus(Sentries->conn) != CLI_CONNECTION_OK) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<br>Connection to exfMaster failed.\n");
      printf("<br>Error in Connecting to SRB: <A href=\"srberror2.cgi?%i\">Error: %i</A>\n",clStatus(Sentries->conn),clStatus(Sentries->conn));
      fflush(stdout);
      srb_perror (2, clStatus(Sentries->conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      printf("%s",clErrorMessage(Sentries->conn));
      clFinish(Sentries->conn);
      printf("</body></html>");
      exit(1);
    }
    return(0);
}

int makeTicketConnection(inStruct Sentries)
{
  char u[20][MAX_TOKEN];
  int i,j;
  char sessionId[MAX_TOKEN];
  char *tmpPtr, *tmpPtr2;
  char hp[MAX_TOKEN];
  char *tkHostAddr;
  char *tkPort;

  readLangFile(langDir, ""); 
  /****RTRTRT:  readLangFile(langDir, Sentries->currentLanguage); *****/
  sprintf(sessionId,"%s/serverlog/ticketuserid",logDirec);
  fId = fopen(sessionId,"r");
  if (fId == NULL) {
    printf("FATAL ERROR: Unable to Get TicketUserInfo. Please contact <A HREF=\"mailto:%s\">SRB</A>", emailAddress);
    return 1;
  }
  for (i = 0; i < 3; i++) {
    fgets(u[i],MAX_TOKEN,fId);
    u[i][strlen(u[i])-1] = '\0';
  }
  fclose(fId);

    sprintf(envstr,"srbUser=%s", u[0]);
    putenv(envstr);
    sprintf(envstr2,"mdasDomainName=%s", u[1] );
    putenv(envstr2);
    strcpy(hp,Sentries->entries[Sentries->m].val);

    if ((tkHostAddr = strstr(Sentries->entries[Sentries->m].val,"@")) != NULL) {
      *tkHostAddr = '\0';
      tkHostAddr++;
      if ((tkPort =  strstr(tkHostAddr,":")) == NULL) 
	tkPort = Sentries->SRBPORT;
      else {
	*tkPort = '\0';
	tkPort++;
      }
      if (!strcmp( tkHostAddr,"DEFAULT"))
	tkHostAddr = Sentries->connectHostAddress;
      if (!strcmp( tkPort,"DEFAULT"))
	tkPort = Sentries->SRBPORT;
      sprintf(envstr3,"srbPort=%s", tkPort );
      putenv(envstr3);
      sprintf(envstr4,"srbHost=%s", tkHostAddr);
      putenv(envstr4);
    }
    else if (Sentries->SRBPORT != NULL && strcmp(Sentries->SRBPORT ,"NULL")){
      sprintf(envstr3,"srbPort=%s", Sentries->SRBPORT );
      putenv(envstr3);
      sprintf(envstr4,"srbHost=%s", Sentries->connectHostAddress);
      putenv(envstr4);
      tkHostAddr = Sentries->connectHostAddress;
    }
    
    
#ifdef USE118LEAN
    Sentries->conn = clConnect (tkHostAddr, NULL, u[2]);
#else
    Sentries->conn = srbConnect (tkHostAddr, NULL, u[2], u[0], u[1], NULL, NULL);
#endif


  if (clStatus(Sentries->conn) != CLI_CONNECTION_OK) {
    printf("<br>Connection to exfMaster failed.\n");
    printf("<br>Error in Connecting to SRB: <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", clStatus(Sentries->conn),clStatus(Sentries->conn));
    fflush(stdout);
    srb_perror (2, clStatus(Sentries->conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    printf("%s",clErrorMessage(Sentries->conn));
    clFinish(Sentries->conn);
    return 1;
  }
  return(0);
  
}
int
changePassword(inStruct Sentries)
{
  int i,j;

    if (strcmp(Sentries->entries[4].val,Sentries->entries[5].val))
      {
	printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	printf("<html>");
	printf("<body background=\"%s/srb3.jpg\">", webPath);
	printf("<br>Connection to exfMaster failed.\n");
        printf("ERROR: Password typing error.<BR>Please use Back command and retype new password.\n");
	printf("</body></html>");
	exit(1);
      }

    sprintf(envstr,"srbUser=%s", Sentries->entries[1].val);
    putenv(envstr);
    sprintf(envstr2,"mdasDomainName=%s", Sentries->entries[2].val );
    putenv(envstr2);

    if (strlen(Sentries->entries[6].val)) {
           sprintf(envstr3,"srbPort=%s", Sentries->entries[6].val);
    	    putenv(envstr3);        
    }
    else {
        if (Sentries->SRBPORT != NULL && strcmp(Sentries->SRBPORT ,"NULL")){
           sprintf(envstr3,"srbPort=%s", Sentries->SRBPORT );
    	    putenv(envstr3);
        }
    }
    
    if (strlen(Sentries->entries[7].val)) {    
        strcpy(Sentries->connectHostAddress, Sentries->entries[7].val);
        sprintf(envstr4,"srbHost=%s", Sentries->entries[7].val);
        putenv(envstr4);
    }
    else {
        sprintf(envstr4,"srbHost=%s", Sentries->connectHostAddress);
        putenv(envstr4);
    }


#ifdef USE118LEAN
    Sentries->conn = clConnect (Sentries->connectHostAddress, NULL, Sentries->entries[3].val); 
#else
    Sentries->conn = srbConnect (Sentries->connectHostAddress, NULL, Sentries->entries[3].val, 
     Sentries->entries[1].val, Sentries->entries[2].val, NULL, NULL);
#endif

  if (clStatus(Sentries->conn) != CLI_CONNECTION_OK) {
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html>");
    printf("<body background=\"%s/srb3.jpg\">", webPath);
    printf("<br>Error in Connecting to SRB: <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", clStatus(Sentries->conn), clStatus(Sentries->conn));
    fflush(stdout);
    srb_perror (2, clStatus(Sentries->conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    printf("%s",clErrorMessage(Sentries->conn));
    clFinish(Sentries->conn);
    printf("</body></html>");
    exit(1);
  }

  i = srbModifyUser(Sentries->conn, MDAS_CATALOG, Sentries->entries[4].val,
                      "", U_CHANGE_PASSWORD);

  if (i != 0)  {
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html>");
    printf("<body background=\"%s/srb3.jpg\">", webPath);
    printf("<br>Error: Password Change Failed: <A href=\"srberror2.cgi?%i\">Error: %i</A>\n",i,i);
    printf("</body></html>");
    clFinish(Sentries->conn);
    exit(1);
  }
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html>");
    printf("<body background=\"%s/srb3.jpg\">", webPath);
    printf("Password change Successful");
    printf("</body></html>");    
    clFinish(Sentries->conn);
}


int
registerUser(inStruct Sentries)
{
  int i,j;
  char sessionId[MAX_TOKEN];
  char u[8][MAX_TOKEN];
  char *tmpPtr;
  char uPass[MAX_TOKEN], msgBody[MAX_TOKEN];
  char uName[MAX_TOKEN], uEmail[MAX_TOKEN];
  FILE *fpMailer;

	char buf[MAX_TOKEN];

  srandom((unsigned int) getpid());
  sprintf(uPass,"%ld",(long) random());

    /***
    tmpPtr  = strstr ( entries[3].val,"@");
	sprintf(buf, "@%s", "blah");
    if (tmpPtr == NULL || strcmp (tmpPtr,buf)) {   
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("Registration Error: Your email address should be user@%s.<BR>Please use Back command and resubmit.\n", "blah");
      printf("</body></html>");
      exit(1);
    }
    *tmpPtr = '\0';
    strcpy(uName,entries[3].val);
    *tmpPtr = '@';
    if (strchr(uName, ' ') != NULL) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("Registration Error: Your Email Address contains blanks.<BR>Please use Back command and resubmit.\n");
      printf("</body></html>");
      exit(1);
    }
    ***/
    strcpy(uName,Sentries->entries[3].val);
sprintf(uEmail,"%s@%s",uName,returnEmailDomain);
    if (getenv("REMOTE_ADDR") == NULL) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<br>FATAL WEB SERVER ERROR: Error getting Remote Host Information. Disconnecting\n");
      fflush(stdout);
      printf("</body></html>");
      exit(1);    
    }
    strcpy(Sentries->remoteHost,getenv("REMOTE_ADDR"));
    if (strlen(Sentries->remoteHost) == 0) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<br>FATAL WEB SERVER ERROR: Error getting Remote Host Information. Disconnecting\n");
      fflush(stdout);
      printf("</body></html>");
      exit(1);    
    }
  if (strstr(Sentries->remoteHost,checkHostAddress) != Sentries->remoteHost) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body background=\"%s/srb3.jpg\">", webPath);
      printf("<br>Registration Error: Only users with valid email accounts at %s can register in this way. Double-check the spelling of your entry.\n", returnEmailDomain);
      fflush(stdout);
      printf("</body></html>");
      exit(1);    

  }

  sprintf(sessionId,"%s/serverlog/serverid",logDirec);
  fId = fopen(sessionId,"r");
  if (fId == NULL) {
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html>");
    printf("FATAL ERROR: Unable to Register. Please try later or contact <A HREF=\"mailto:%s\">SRB</A>", emailAddress);
    printf("</body></html>");
    exit(1);
  }
  for (i = 0; i < 3; i++) {
    fgets(u[i],MAX_TOKEN,fId);
    u[i][strlen(u[i])-1] = '\0';
  }
  fclose(fId);

    sprintf(envstr,"srbUser=%s", u[0]);
    putenv(envstr);
    sprintf(envstr2,"mdasDomainName=%s", u[1] );
    putenv(envstr2);
    if (Sentries->SRBPORT != NULL && strcmp(Sentries->SRBPORT ,"NULL")){
       sprintf(envstr3,"srbPort=%s", Sentries->SRBPORT );
	    putenv(envstr3);
    }
    sprintf(envstr4,"srbHost=%s", connectHostAddress);
    putenv(envstr4);

#ifdef USE118LEAN
    Sentries->conn = clConnect (connectHostAddress, NULL, u[2]);
#else
    Sentries->conn = srbConnect (connectHostAddress, NULL, u[2],
     u[0], u[1], NULL, NULL);
#endif

  if (clStatus(Sentries->conn) != CLI_CONNECTION_OK) {
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html>");
    printf("<br>Connection to exfMaster failed.\n");
    printf("<body background=\"%s/srb3.jpg\">", webPath);
    printf("<br>Error in Connecting to SRB: <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", clStatus(Sentries->conn),clStatus(Sentries->conn));
    fflush(stdout);
    srb_perror (2, clStatus(Sentries->conn), "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    printf("%s",clErrorMessage(Sentries->conn));
    clFinish(Sentries->conn);
    printf("</body></html>");
    exit(1);
  }
  sprintf(msgBody,"%s, %s",Sentries->entries[1].val,Sentries->entries[2].val);
 
  i = srbRegisterUser (Sentries->conn,0,
			 uName,  newUserDomain,uPass,
			 "staff", msgBody,
			 Sentries->entries[4].val,uEmail);
  if (i != 0)  {
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html>");
    printf("<body background=\"%s/srb3.jpg\">", webPath);
    printf("<br>Error: Registration  Failed: <A href=\"srberror2.cgi?%i\">Error: %i</A>\n",i,i);
    printf("</body></html>");
    clFinish(Sentries->conn);
    exit(1);
  }
  writeRegLog(Sentries);
  if ((fpMailer = popen("/usr/lib/sendmail -t","w")) == NULL) {
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html>");
    printf("<body background=\"%s/srb3.jpg\">", webPath);
    printf("<br>Error: Registration Failed: Email Failure \n");
    printf("</body></html>");
    clFinish(Sentries->conn);
    exit(1);
  }
  sprintf(msgBody,"To: %s\nFrom: %s\nSubject: MySRB Registration\nYour User Name is %s\nYour SRB Domain is %s.\nYour SRB Password is %s\n\n", emailAddress, uEmail , uName, newUserDomain, uPass);
  i =fputs(msgBody, fpMailer);
  if (i != strlen(msgBody))      {
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html>");
    printf("<body background=\"%s/srb3.jpg\">", webPath);
    printf("<br>Error: Registration  Failed: Email Sending Error\n");
    printf("</body></html>");
    clFinish(Sentries->conn);
    exit(1);
  }
  pclose(fpMailer);

  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf("<html>");
  printf("<body background=\"%s/srb3.jpg\">", webPath);
  printf("<br>Success: User Registered.\n");
  printf("<br> An email has been sent with your password.<br> Please change it as soon as possible.\n");
  fflush(stdout);
  printf("</body></html>");
  clFinish(Sentries->conn);
  return (0);
}

int
prtMetaQueryResult (mdasC_sql_result_struct *myresult, int orderby[])
{
  int i,j;
  char *meta[100];

  for (i = 0; i < myresult->result_count; i++) {
    meta[i] = (char *) getFromResultStruct(
		 myresult,
         dcs_tname[orderby[i]], dcs_aname[orderby[i]]);
  }
  for (i = 0; i < myresult->row_count; i++) {
    printf("<TR>");
    for (j = 0 ; j <  myresult->result_count; j++) {
      printf("<TD>%s</TD>",meta[j]);
      meta[j] += MAX_DATA_SIZE;
    }
     printf("</TR>\n");fflush(stdout);
  }
}
 
prtTableHeader(char * qType)
{

  printf("<TABLE><TR>\n");
  if (!strcmp(qType,"basic resource")) {
    printf("<TH align=left>Logical Resource Name</TH>");
    printf("<TH align=left>Physical Resource Name</TH>");
    printf("<TH align=left>Resource Address</TH>");
    printf("<TH align=left>Default Path in Resource</TH>");
    printf("<TH align=left>Resource Type</TH>");
    printf("<TH align=left>Resource Class</TH>");
    /**
    printf("<TH align=left>Resource Latency (Max)</TH>");
    printf("<TH align=left>Resource Latency (Min)</TH>");
    printf("<TH align=left>Resource Bandwidth</TH>");
    printf("<TH align=left>Resource Concurrency (Max)</TH>");
    printf("<TH align=left>Number of Hierarchies in Resource</TH>");
    printf("<TH align=left>Number of Stripes in Resource</TH>");
    printf("<TH align=left>Resource Capacity</TH>");
    printf("<TH align=left>Resource Description</TH>");
    **/
  }
  else if (!strcmp(qType,"basic user")) {
    printf("<TH align=left>User Name</TH>");
    printf("<TH align=left>User Domain</TH>");
    printf("<TH align=left>Address</TH>");
    printf("<TH align=left>Phone</TH>");
    printf("<TH align=left>Email</TH>");
    printf("<TH align=left>Type</TH>");
  }
  else if (!strcmp(qType,"container")) {
    printf("<TH align=left>Container Name</TH>");
    printf("<TH align=left>Container Max Size</TH>");
    printf("<TH align=left>Logical Resource Name</TH>");
    printf("<TH align=left>Container Resource Class</TH>");
    printf("<TH align=left>Container Current Size</TH>");
    printf("<TH align=left>Physical Resource Name</TH>");
  }
  else if (!strcmp(qType,"dublin core")) {
    printf("<TH align=left>Title</TH>");
    printf("<TH align=left>Contributor Name</TH>");
    printf("<TH align=left>Contributor Type</TH>");
    printf("<TH align=left>Subject Classification</TH>");
    printf("<TH align=left>Subject</TH>");
    printf("<TH align=left>Language</TH>");
    printf("<TH align=left>Type</TH>");
    printf("<TH align=left>Description Type</TH>");
    printf("<TH align=left>Description</TH>");
    printf("<TH align=left>Source</TH>");
    printf("<TH align=left>Source Type</TH>");
    printf("<TH align=left>Coverage</TH>");
    printf("<TH align=left>Coverage Type</TH>");
    printf("<TH align=left>Righta</TH>");
    printf("<TH align=left>RightaType</TH>");
    printf("<TH align=left>Publisher</TH>");
    printf("<TH align=left>Related Data Name</TH>");
    printf("<TH align=left>Related Collection</TH>");
    printf("<TH align=left>Raelted Data Description</TH>");
    printf("<TH align=left>Relationship Type</TH>");
  }
  printf("</TR>\n");fflush(stdout);
}

int
printCommonTicketButtons(inStruct Sentries)
{
    printf("<A HREF=\"%s/%s\" ><IMG SRC=\"%s/%s\" ALT=\"Help\"></A>\n", webPath, mySrbStrings[56], webPath, "help.gif");
fflush(stdout);
    printf("<A HREF=\"mysrb331.cgi?function=unmount&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Exit MySRB\"></A>\n",
	   Sentries->pidStr, webPath, "exitmysrb.gif");
fflush(stdout);
    printf("<P>\n");

fflush(stdout);

}


printCommonButtons(inStruct Sentries, char *urlcollection, char *parurlcollection)
{

  /****** light mode *********/    
  if (!strcmp(mode,"light")) {

/*** In the 2d and 3d tables replace 100% by 480 as '%' can cause pbs ***/

  printf("<table align=center width=480 border=0 cellpadding=1 cellspacing=0 bgcolor=\"#006699\">");
  printf("	<tr><td><table width=480 cellpadding=0 cellspacing=0 border=0 bgcolor=\"#FFFFFF\">");
  printf("		<tr><td><table width=480 border=0  cellpadding=3 cellspacing=0><tr>");
  printf("<td align=center onmouseover=\"MouseOver(this)\" onmouseout=\"MouseOut(this)\"><a href=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=&session=%s\"  target=\"%s\">%s</a></td>", "",parurlcollection, Sentries->pidStr, Sentries->frameStr, mySrbStrings[186]);
  printf("<td align=center onmouseover=\"MouseOver(this)\" onmouseout=\"MouseOut(this)\"><a href=\"mysrb331.cgi?function=browsequery&collection=%s&type=first&session=%s\">%s</a></td>",urlcollection, Sentries->pidStr, mySrbStrings[187]);
  printf("<td align=center onmouseover=\"MouseOver(this)\" onmouseout=\"MouseOut(this)\"><a href=\"%s/%s\">%s</a></td>", webPath, mySrbStrings[56], mySrbStrings[188]);
  printf("<td align=center onmouseover=\"MouseOver(this)\" onmouseout=\"MouseOut(this)\"><a href=\"mysrb331.cgi?function=unmount&session=%s\">%s</a></td>", Sentries->pidStr, mySrbStrings[189]);
  printf("</tr></table></td></tr>");
  printf("	</table></td></tr>");
  printf("</table></td>");
    
//  printf("<TABLE BGCOLOR=\"#DDEFFF\" bordercolor=\"#DDEFFF\" ><TR>");
//  printf("<TD align=center BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=&session=%s\"  target=\"%s\"><IMG SRC=\"%s/%s\" ALT=\"Move Up\"><BR>Move Up</A></TD>\n", "",parurlcollection, Sentries->pidStr, Sentries->frameStr, webPath,"uparrow.gif");
//
//   printf("<TD align=center BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=browsequery&collection=%s&type=first&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Browse Query\"><BR>Browse Query</A></TD>\n",urlcollection,
//	   Sentries->pidStr, webPath, "querymd.gif");
//
//    printf("<TD align=center BGCOLOR=\"#DDEFFF\"><A HREF=\"%s/%s\" ><IMG SRC=\"%s/%s\" ALT=\"Help\"><BR>Online Help</A></TD>\n", webPath, mySrbStrings[56], webPath, "help.gif");
//    printf("<TD align=center BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=unmount&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Exit MySRB\"><BR>Exit MySRB</A></TD>\n",
//	   Sentries->pidStr, webPath,"exitmysrb.gif");
//	   
//	    printf("</TR></TABLE><P>\n");   
    
  }

  /********** end of light mode **********/  
  else {

  printf("<CENTER><TABLE BGCOLOR=\"#DDEFFF\" bordercolor=\"#DDEFFF\" ><TR valign=\"top\" >");
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=&session=%s\"  target=\"%s\"><IMG SRC=\"%s/%s\" ALT=\"Move Up\"><BR>%s</A></TD>\n", "",parurlcollection, Sentries->pidStr, Sentries->frameStr, webPath,"uparrow.gif", mySrbStrings[30]);
  /*  printf("<TD BGCOLOR=\"#DDEFFF\">&nbsp;&nbsp;&nbsp;</TD>\n");*/

  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=getcollmetaattr&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Ingest New File\"><BR>%s</A></TD>\n", urlcollection,"", Sentries->pidStr, webPath,"ingestFile.gif", mySrbStrings[31]);
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=createtextfileform&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Create New File\"><BR>%s</A></TD>\n", urlcollection,"", Sentries->pidStr, webPath,"ingestFile.gif", mySrbStrings[32]);
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=regfileform&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Register New File\"><BR>%s</A></TD>\n",
	 urlcollection,"", Sentries->pidStr, webPath, "ingestFile.gif", mySrbStrings[33]);
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=regdirform&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Register Directory\"><BR>%s</A></TD>\n",
	 urlcollection,"", Sentries->pidStr, webPath, "newsubcollection.gif", mySrbStrings[34]);
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=regurlform&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Register URL\"><BR>%s</A></TD>\n",
	 urlcollection,"", Sentries->pidStr, webPath, "ingestFile.gif", mySrbStrings[35]);
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=regsqlform&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Register SQL\"><BR>%s</A></TD>\n", urlcollection,"", Sentries->pidStr, webPath, "ingestFile.gif", mySrbStrings[36]);
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=regorbform&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Register ORB File\"><BR>%s</A></TD>\n", urlcollection,"", Sentries->pidStr, webPath, "ingestFile.gif", mySrbStrings[37]);
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=regexecform&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Register Command\"><BR>%s</A></TD>\n",
	 urlcollection,"", Sentries->pidStr, webPath, "ingestFile.gif", mySrbStrings[38]);
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=createCollectionForm&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Create New SubCollection\"><BR>%s</A></TD>\n",
	 urlcollection,"", Sentries->pidStr, webPath, "newsubcollection.gif", mySrbStrings[39]);
   printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=makecontainerform&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Create New Container\"><BR>%s</A></TD>\n",
	   Sentries->pidStr,webPath, "mkcontainer.gif", mySrbStrings[40]);
   /*printf("<TD BGCOLOR=\"#DDEFFF\">&nbsp;&nbsp;&nbsp;</TD>\n");*/
/*
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=showallmeta&dataname=%s&collection=%s&altcoll=%s&session=%s\"  target=\"_self\" ><IMG SRC=\"%s/%s\" ALT=\"Show Metadata\"><BR>Show<BR>Meta</A></TD>\n",
	 "",urlcollection,"",Sentries->pidStr, webPath, "showmeta.gif");

      printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=schemaquery&dataname=dicom&collection=/home/testuser.sdsc/schemas&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Query DICOM\"><BR>Query<BR>DICOM</A></TD>\n","",
	   pidStr, webPath, "querymd.gif");
  */
    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=browsequery&collection=%s&type=first&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Browse Query\"><BR>%s</A></TD>\n",urlcollection,
	   Sentries->pidStr, webPath, "querymd.gif", mySrbStrings[41]);
    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=extschmnamelisting&collection=%s&type=first&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Extensible Schema  Query\"><BR>%s</A></TD>\n",urlcollection,
           Sentries->pidStr, webPath, "querymd.gif", aaa[0]);
    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=ingestextschmnamelisting&data=&collection=%s&session=%s\"><IMG SRC=\"%s/%s\" ALT=\"Insert Into Extensible Schema\"><BR>%s</A></TD>\n",urlcollection,
	   Sentries->pidStr, webPath, "ingestFile.gif",aaa[14]);

    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=otherinfo&data=%s&coll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Other Info\"><BR>%s</A></TD>\n",
	   Sentries->entries[2].val,Sentries->entries[3].val,Sentries->pidStr, webPath, "otherinfo.gif", mySrbStrings[42]);
    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"%s/%s\" ><IMG SRC=\"%s/%s\" ALT=\"Help\"><BR>%s</A></TD>\n", webPath, mySrbStrings[56], webPath, "help.gif", mySrbStrings[43]);
    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=unmount&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Exit MySRB\"><BR>%s</A></TD>\n",
	   Sentries->pidStr, webPath,"exitmysrb.gif", mySrbStrings[44]);
    printf("</TR></TABLE></CENTER><P>\n");
    }

}


printShadowTicketButtons(inStruct Sentries, char *parData, char *parPath)
{
}


printShadowButtons(inStruct Sentries, char *parData, char *parPath)
{
  printf("<TABLE BGCOLOR=\"#DDEFFF\" bordercolor=\"#DDEFFF\" ><TR>");
  if (parData[0] =='\0')
    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=1&session=%s\"  target=\"%s\"><IMG SRC=\"%s/%s\" ALT=\"Move Up\"><BR>%s</A></TD>\n",
	 parData,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,parPath, Sentries->pidStr, Sentries->frameStr, webPath, "uparrow.gif", mySrbStrings[30]);
  else
    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=slinkfull&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=1&session=%s\"  target=\"%s\"><IMG SRC=\"%s/%s\" ALT=\"Move Up\"><BR>%s</A></TD>\n",
	 parData,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,parPath, Sentries->pidStr, Sentries->frameStr, webPath, "uparrow.gif", mySrbStrings[30]);
  /**
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=getcollmetaattr&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Ingest New File\"><BR>Ingest<BR>File</A></TD>\n",
	 urlcollection,"",Sentries->pidStr, webPath, "ingestFile.gif");
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=regfileform&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Register New File\"><BR>Register<BR>File</A></TD>\n",
	 urlcollection,"",Sentries->pidStr, webPath, "ingestFile.gif");
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=regdirform&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Register New File\"><BR>Register<BR>Directory</A></TD>\n",
	 urlcollection,"",Sentries->pidStr, webPath, "newsubcollection.gif");
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=createCollectionForm&collection=%s&altcoll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Create New SubCollection\"><BR>Make<BR>Collection</A></TD>\n",
	 urlcollection,"",Sentries->pidStr, webPath, "newsubcollection.gif");
   printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=makecontainerform&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Create New Container\"><BR>Make<BR>Container</A></TD>\n",
	   pidStr, webPath, "mkcontainer.gif");
  printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=showallmeta&dataname=%s&collection=%s&altcoll=%s&session=%s\"  target=\"_self\" ><IMG SRC=\"%s/%s\" ALT=\"Show Metadata\"><BR>Show<BR>Meta</A></TD>\n",
	 "",urlcollection,"",Sentries->pidStr, webPath, "showmeta.gif");
  **/
  /*
    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=querymeta&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Query MetaData\"><BR>Query<BR>Meta</A></TD>\n",
	   pidStr, webPath, "querymd.gif");
  **/
    /**
    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=browsequery&collection=%s&type=first&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Browse Query\"><BR>Browse<BR>Query</A></TD>\n",urlcollection,
	   pidStr, webPath, "querymd.gif");
    **/
    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=otherinfo&data=%s&coll=%s&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Other Info\"><BR>Other<BR>Info</A></TD>\n",
	   Sentries->entries[2].val,Sentries->entries[3].val, Sentries->pidStr, webPath, "otherinfo.gif");
    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"%s/%s\" ><IMG SRC=\"%s/%s\" ALT=\"Help\"><BR>Online<BR>Help</A></TD>\n", webPath, mySrbStrings[56], webPath, "help.gif");
    printf("<TD BGCOLOR=\"#DDEFFF\"><A HREF=\"mysrb331.cgi?function=unmount&session=%s\" ><IMG SRC=\"%s/%s\" ALT=\"Exit MySRB\"><BR>Exit<BR>MySRB</A></TD>\n", Sentries->pidStr, webPath, "exitmysrb.gif");
    printf("</TR></TABLE><P>\n");
}



int
metaExtract(inStruct Sentries)
{
  int i,j,status;
  char argStr[HUGE_STRING];
  j = 0;
  /**
     j = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
     entries[1].val, entries[2].val, "","",
     "/home/testuser.sdsc/mytest/htmlExtractTemplate.sty","",D_EXTRACT_TEMPLATE_METADATA);
     if (j != 0)
     printf("<br>Error: In Extracting Metadata  for %s/%s; <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",entries[2].val,entries[1].val, j,j);
     else 
     printf("Done");
  **/
  printf("Extracting Metadata ..."); fflush(stdout);
  if (!strcmp(Sentries->entries[4].val,"NULL")) 
    sprintf(argStr,"%s|%s/%s", Sentries->entries[3].val, Sentries->entries[2].val,Sentries->entries[1].val);
  else 
    sprintf(argStr,"%s|%s/%s|%s/%s",Sentries->entries[3].val, Sentries->entries[2].val,Sentries->entries[1].val,
	    Sentries->entries[4].val,Sentries->entries[5].val);

  status = srbExecFunction (Sentries->conn, "extractMetadata", argStr, NULL,
                            PORTAL_STD_IN_OUT);
  if (status < 0) {
    printf("Error in srbExecFunction: %i\n",status);
    
    while ((status = read (status, buf, sizeof (buf))) > 0) {
        if (status < sizeof (buf))
            buf[status] = '\0';
        printf ("%s", buf);
    }
    return(-1);
  }
  printf("done");
  bRMap(2,0);
  return(0);  

}



int
copymeta(inStruct Sentries)
{
  int i,j;
  char *srcColl, *srcObj, *trgColl, *trgObj;

  printf("Copying Metadata ...\n");fflush(stdout);
  srcColl = Sentries->entries[2].val;
  srcObj = Sentries->entries[1].val;
  trgColl = Sentries->entries[Sentries->m-1].val;
  trgObj = Sentries->entries[Sentries->m].val;
  
  if (strlen(srcObj) == 0) { /* from collection */
    if (strlen(trgObj) == 0) { /* from collection to collection */
      i = srbModifyCollect(Sentries->conn, MDAS_CATALOG, 
			   trgColl,
			   srcColl,srcObj,"",
			     C_COPY_META_DATA_FROM_COLL_TO_COLL);
    }
    else { /* from collection to object */
      i = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
			   trgObj, trgColl,
			   "","", srcColl,srcObj,
			     D_COPY_META_DATA_FROM_COLL_TO_DATA);
    }
  }
  else { /* from object */
    if (strlen(trgObj) == 0) { /* from object to collection */
      i = srbModifyCollect(Sentries->conn, MDAS_CATALOG, 
			   trgColl,
			   srcColl,srcObj,"",
			     C_COPY_META_DATA_FROM_DATA_TO_COLL);
    }
    else { /* from object to object */
      i = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
			   trgObj, trgColl,
			   "","", srcColl,srcObj,
			     D_COPY_META_DATA_FROM_DATA_TO_DATA);
    }
  }
    if (i != 0) 
      printf("<br>Error: In Copying Metadata; <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", i,i);
    else 
      printf("Done");
    bRMap(2,0);
    return(0);  
}



int copydsData(inStruct Sentries)
{
    int i, status;
    int in_fd, out_fd;
    char temp[MAX_OBJ_NAME_LENGTH];
    char tmpTargObj[2 * MAX_TOKEN];

    printf("Copying Data ..."); fflush(stdout);

    sprintf(temp, "%s&COPY=%s&DVERSION='%s'",Sentries->entries[1].val,Sentries->entries[4].val,Sentries->entries[6].val);
    in_fd = srbObjOpen (Sentries->conn, temp, O_RDONLY, Sentries->entries[2].val);
    if (in_fd < 0) {
      printf("Error: Unable to open object '%s/%s', status = %d.\n",
              Sentries->entries[2].val, temp, in_fd);
      srb_perror (2, in_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      bRMap(2,0);
      return(in_fd);
    }
    if (strlen(Sentries->entries[10].val) > 0) 
      sprintf (tmpTargObj, "%s&CONTAINER=%-s", Sentries->entries[8].val, Sentries->entries[10].val);
    else
      strcpy (tmpTargObj,Sentries->entries[8].val);
      
      /**   store data type in entries[5].val   **/
      getContentType(Sentries, Sentries->entries[2].val, Sentries->entries[5].val);      
    out_fd = srbObjCreate (Sentries->conn, MDAS_CATALOG,tmpTargObj,
              Sentries->entries[5].val,Sentries->entries[9].val,  Sentries->entries[7].val, "", -1);
    if (out_fd < 0) {
      printf ("Error: Unable to create new object %s/%s, status = %d\n",
	      Sentries->entries[7].val, tmpTargObj, out_fd);
      srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      srbObjClose (Sentries->conn, in_fd);
      bRMap(2,0);
      return (out_fd);
    }
    
    status = srbObjProxyOpr (Sentries->conn, OPR_COPY, in_fd, out_fd, 0, 0,
			     NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    srbObjClose (Sentries->conn, out_fd);
    srbObjClose (Sentries->conn, in_fd);
    if (status < 0) {
      srbObjUnlink(Sentries->conn, Sentries->entries[8].val,Sentries->entries[7].val);
      printf ("Error: Error in Copying from %s/%s to %s/%s: %d\n",
	      Sentries->entries[2].val, temp, Sentries->entries[7].val, Sentries->entries[8], status);
	  srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  bRMap(2,0);
 	  return (status);
    }
    printf(" Done");
    bRMap(2,1);
    return(0);
}

int copydsColl()
{
    construction(); 
}

int movedsData(inStruct Sentries)
{
    int i,j, status;
    char tempName[2 * MAX_TOKEN];
    char temp[MAX_OBJ_NAME_LENGTH];
    int in_fd, out_fd;
    char tmpTargObj[2 * MAX_TOKEN];
    char cval[MAX_TOKEN];

    j = 0;
    printf("Moving Data ..."); fflush(stdout);

    if ((strlen(Sentries->entries[9].val) == 0 && strlen(Sentries->entries[10].val) == 0) ||
	(!strcmp(Sentries->entries[9].val,Sentries->entries[3].val) && strlen(Sentries->entries[10].val) == 0)
	) { /* Logical  Move */
      if (!strcmp(Sentries->entries[2].val,Sentries->entries[7].val)) { /* only renaming data */
	if (strcmp(Sentries->entries[1].val,Sentries->entries[8].val))
	  j =  srbModifyDataset(Sentries->conn, MDAS_CATALOG,Sentries->entries[1].val, Sentries->entries[2].val,
				"","", Sentries->entries[8].val, "", D_CHANGE_DNAME);
      }
      else if (!strcmp(Sentries->entries[1].val,Sentries->entries[8].val)) { /* only moving to new collection */
	j = srbModifyDataset(Sentries->conn, MDAS_CATALOG,Sentries->entries[1].val, Sentries->entries[2].val,
			     "","", Sentries->entries[7].val, "", D_CHANGE_GROUP);
      }
      else { /* changing name of object as well as its collection */
	srandom((unsigned int) getpid());
	sprintf(cval,"%ld",(long) random());
	sprintf(tempName, "%s.%s.%s", cval,Sentries->entries[8].val,cval);
	j = srbModifyDataset(Sentries->conn, MDAS_CATALOG,Sentries->entries[1].val, Sentries->entries[2].val,
			     "","", tempName, "", D_CHANGE_DNAME);
	if (j < 0) goto next;
	j = srbModifyDataset(Sentries->conn, MDAS_CATALOG,tempName, Sentries->entries[2].val,
			     "","", Sentries->entries[7].val, "", D_CHANGE_GROUP);
	if (j < 0) {
	  srbModifyDataset(Sentries->conn, MDAS_CATALOG,tempName, Sentries->entries[2].val,
			   "","", Sentries->entries[1].val, "", D_CHANGE_DNAME);
	  goto next;
	}
	j =  srbModifyDataset(Sentries->conn, MDAS_CATALOG,tempName, Sentries->entries[7].val,
			      "","", Sentries->entries[8].val, "", D_CHANGE_DNAME);
	if (j < 0) {
	  srbModifyDataset(Sentries->conn, MDAS_CATALOG,tempName, Sentries->entries[7].val,
			   "","", Sentries->entries[2].val, "", D_CHANGE_GROUP);
	  srbModifyDataset(Sentries->conn, MDAS_CATALOG,tempName, Sentries->entries[2].val,
			   "","", Sentries->entries[1].val, "", D_CHANGE_DNAME);
	  goto next;
	}
      }
      
next:
      if (j < 0) {
	printf ("Error: Error in Moving from %s/%s to %s/%s: %d\n",
		Sentries->entries[2].val, Sentries->entries[1].val, Sentries->entries[7].val,Sentries->entries[8].val,j);
	srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	bRMap(2,0);
 	return (j);
      }
      printf(" Done");
      bRMap(2,1);
      return(0);
    }
    else { /* Physical  Move */
      if (strlen(Sentries->entries[10].val) == 0 && !strcmp(Sentries->entries[1].val,Sentries->entries[8].val) &&
	  !strcmp(Sentries->entries[2].val,Sentries->entries[7].val)
	  ) { /* just move to another resource */
	sprintf(temp, "%s&COPY=%s&DVERSION='%s'",Sentries->entries[1].val,Sentries->entries[4].val,Sentries->entries[6].val);
	j = srbObjMove(Sentries->conn, MDAS_CATALOG, temp, Sentries->entries[2].val,"",
			 Sentries->entries[9].val , "","");
	if (j < 0)  {  
	  printf ("Error: Error in Moving from %s/%s to %s/%s: %d\n",
		Sentries->entries[2].val, Sentries->entries[1].val, Sentries->entries[7].val,Sentries->entries[8].val,j);
	  srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  bRMap(2,0);
 	  return(j);
	}
	bRMap(2,1);
	printf(" Done");
	return(j);
      }
      sprintf(temp, "%s&COPY=%s&DVERSION='%s'",Sentries->entries[1].val,Sentries->entries[4].val,Sentries->entries[6].val);
      in_fd = srbObjOpen (Sentries->conn, temp, O_RDONLY, Sentries->entries[2].val);
      if (in_fd < 0) {
	printf("Error: Unable to open object '%s/%s', status = %d.\n",
	       Sentries->entries[2].val, temp, in_fd);
	srb_perror (2, in_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	bRMap(2,0);
	return(in_fd);
      }
      srandom((unsigned int) getpid());
      sprintf(cval,"%ld",(long) random());
      if (strlen(Sentries->entries[10].val) > 0) 
	sprintf (tmpTargObj, "%s.%s.%s&CONTAINER=%-s", cval,Sentries->entries[8].val,cval, Sentries->entries[10].val);
      else
	sprintf (tmpTargObj, "%s.%s.%s", cval,Sentries->entries[8].val,cval);
      out_fd = srbObjCreate (Sentries->conn, MDAS_CATALOG,tmpTargObj,
			     Sentries->entries[5].val,Sentries->entries[9].val,  Sentries->entries[7].val, "", -1);
      if (out_fd < 0) {
	printf ("Error: Unable to create new object %s/%s, status = %d\n",
		Sentries->entries[7].val, tmpTargObj, out_fd);
	srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	srbObjClose (Sentries->conn, in_fd);
	bRMap(2,0);
	return (out_fd);
      }
      status = srbObjProxyOpr (Sentries->conn, OPR_COPY, in_fd, out_fd, 0, 0,
			       NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
      srbObjClose (Sentries->conn, out_fd);
      srbObjClose (Sentries->conn, in_fd);
      if (status < 0) {
	srbObjUnlink(Sentries->conn, tmpTargObj,Sentries->entries[7].val);
	printf ("Error: Error in Copying from %s/%s to %s/%s: %d\n",
		Sentries->entries[2].val, temp,Sentries->entries[5].val, Sentries->entries[7].val,tmpTargObj, status);
	srb_perror (2, status, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	bRMap(2,0);
	return (status);
      }
      /** rename it back ***/
      sprintf (tmpTargObj, "%s.%s.%s", cval,Sentries->entries[8].val,cval);
      sprintf(tempName, "%s&COPY=%s",Sentries->entries[1].val, Sentries->entries[4].val);
      if (strcmp(Sentries->entries[1].val,Sentries->entries[7].val)) {
	j = srbModifyDataset(Sentries->conn, MDAS_CATALOG,tmpTargObj, Sentries->entries[7].val,
			     "","", Sentries->entries[8].val, "", D_CHANGE_DNAME);
	if (j < 0) {
	  srbObjUnlink(Sentries->conn, tmpTargObj,Sentries->entries[7].val);
	  printf ("Error: Error in Moving from %s/%s to %s/%s: %d\n",
		  Sentries->entries[2].val, Sentries->entries[1].val, Sentries->entries[7].val,Sentries->entries[8].val,j);
	  srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  bRMap(2,0);
	  return (j);
	}
	/** now delete the old dataset **/
	i = srbObjUnlink(Sentries->conn, tempName, Sentries->entries[2].val);
	if (i < 0) {
	  printf ("Error: Error in Removing from %s/%s: %d\n",
		  Sentries->entries[2].val, tempName, i);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  bRMap(2,0);
	  return (i);
	}
      }
      else {
	/** delete first **/
	i = srbObjUnlink(Sentries->conn, tempName, Sentries->entries[2].val);
	if (i < 0) {
	  printf ("Error: Error in Removing from %s/%s: %d\n",
		  Sentries->entries[2].val, tempName, i);
	  srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  srbObjUnlink(Sentries->conn, tmpTargObj,Sentries->entries[7].val);
	  bRMap(2,0);
	  return (i);
	}
	/** then move **/
	j = srbModifyDataset(Sentries->conn, MDAS_CATALOG,tmpTargObj, Sentries->entries[7].val,
			     "","", Sentries->entries[8].val, "", D_CHANGE_DNAME);
	if (j < 0) {
	  printf ("Error: Error in Moving from %s/%s to %s/%s: %d\n",
		  Sentries->entries[2].val, Sentries->entries[1].val, Sentries->entries[7].val,Sentries->entries[8].val,j);
	  printf("File has been moved to %s/%s\n",Sentries->entries[7].val,tmpTargObj);
	  srb_perror (2, j, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	  bRMap(2,0);
	  return (j);
	}
	
      }
      bRMap(2,1);
      printf(" Done");
      return(0);
    }
}

int renameds(inStruct Sentries)
{
  int j;
  char tmpStr[MAX_TOKEN];
  
  
   printf("Renaming ...\n");fflush(stdout);
   if (strlen(Sentries->entries[1].val) > 0) {
     j = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
			  Sentries->entries[1].val, Sentries->entries[2].val,
			  "","", Sentries->entries[3].val,"",
			  D_CHANGE_DNAME);
     if (j != 0) 
       printf("<br>Error: In Renaming %s/%s to %s; <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",Sentries->entries[2].val,Sentries->entries[1].val,Sentries->entries[3].val, j,j);
     else 
      printf("Done");
   }
   else {
     splitbychar(Sentries->entries[2].val,tmpoStr,tmpStr,'/');
     strcat(tmpoStr,"/");
     strcat(tmpoStr, Sentries->entries[3].val);
     j = srbModifyCollect(Sentries->conn, MDAS_CATALOG,Sentries->entries[2].val,
			  tmpoStr,"",  "",
			  D_CHANGE_COLL_NAME);
     if (j != 0) 
       printf("<br>Error: In Renaming %s to %s; <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",Sentries->entries[2].val,tmpoStr, j,j);
     else 
      printf("Done");

   }
   bRMap(2,1);
   return 0;
}

int slinks(inStruct Sentries)
{
   printf("Linking %s/%s to  %s/%s ...\n",Sentries->entries[2].val,Sentries->entries[1].val,Sentries->entries[7].val,Sentries->entries[8].val);fflush(stdout);
   if (strlen(Sentries->entries[1].val) > 0) {
     j = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
			  Sentries->entries[1].val, Sentries->entries[2].val,
			  "","", Sentries->entries[8].val, Sentries->entries[7].val,
			  D_INSERT_LINK);
   }
   else {
     j = srbModifyCollect(Sentries->conn, MDAS_CATALOG,Sentries->entries[2].val,
			  Sentries->entries[8].val, Sentries->entries[7].val,"",
			  C_INSERT_LINK);
   }
     if (j != 0) 
       printf("<br>Error: In Linking %s/%s to %s/%s; <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",Sentries->entries[2].val,Sentries->entries[1].val,Sentries->entries[7].val,Sentries->entries[8].val, j,j);
     else 
      printf("Done");

   bRMap(2,1);
   return 0;
}


int slinkColl(inStruct Sentries, char *trgColl, char *prtColl, char *srcColl) 
{   
    mdasC_sql_result_struct collResult;
    char destColl[MAX_TOKEN];
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int i, status;
    
/* create new collection in target parent collection */
    if (strlen(trgColl) == 0 ) {
        printf ("Error: Empty Collection Name.\n");
        return (-1);
    }
    
    printf("<BR>Linking %s to %s/%s... ", srcColl, prtColl, trgColl); fflush(stdout);
    
    status = srbCreateCollect(Sentries->conn, MDAS_CATALOG, prtColl, trgColl);
    
    if (status < 0)  {
        printf("<BR>Error: Unable to create collection '%s': <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", trgColl, status, status);
        return(status);
    }
    
    printf("Done<BR>"); fflush(stdout);

    sprintf(destColl, "%s/%s", prtColl, trgColl);

/* list all datasets in source collection and link them into destination collection */
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }

    sprintf(qval[DATA_GRP_NAME], " =  '%s'", srcColl);
    
    selval[DATA_GRP_NAME] = 1;
    selval[DATA_NAME] = 1;
   
    status = srbGetDataDirInfoShunt(Sentries, Sentries->conn, MDAS_CATALOG, qval, selval, &collResult, MAX_RESULT_NUM);

    if (status < 0)     {
        if (status != MCAT_INQUIRE_ERROR)
            printf("Unable to open %s. Collection Access <A href=\"srberror2.cgi?%i\">Error: %i</A> \n", srcColl, status, status);
    }
    else {
        filterDeleted (&collResult);
        slinkDatasetsInColl(Sentries, &collResult, destColl, srcColl);   
        
        while (collResult.continuation_index >= 0) {
            clearSqlResult (&collResult);
            status = srbGetMoreRows(Sentries->conn, 0, collResult.continuation_index, &collResult, MAX_RESULT_NUM);
            
            if (collResult.result_count == 0  || status != 0)
                return (status);
                
            filterDeleted (&collResult);
            slinkDatasetsInColl(Sentries, &collResult, destColl, srcColl);   
        }
        clearSqlResult (&collResult);
    }        
/* now list all subcollections and recursively call slinkColl() for each of them */    
    status = querySubCollInfo(Sentries, Sentries->conn, 0, srcColl, MAX_RESULT_NUM, &collResult);

    if (status < 0)     {
        if (status != MCAT_INQUIRE_ERROR)
            printf("Unable to open %s. Collection Access<A href=\"srberror2.cgi?%i\">Error: %i</A> \n", srcColl, status, status);
    }
    else {    
        slinkSubcollsInColl(Sentries, &collResult, destColl, srcColl);
        
        while (collResult.continuation_index >= 0) {
            clearSqlResult (&collResult);
            status = srbGetMoreRows(Sentries->conn, 0, collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
				
            if (collResult.result_count == 0  || status != 0)
	            return (status);
	            
	        filterDeleted (&collResult);
	        slinkSubcollsInColl(Sentries, &collResult, destColl, srcColl);         
        }
        clearSqlResult (&collResult);        
    }
    return (status);
}


int slinkDatasetsInColl(inStruct Sentries, mdasC_sql_result_struct *myresult, char *destColl, char *srcColl) 
{
    char *dataList;
    int status;
    
    dataList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
        
    for (i = 0; i < myresult->row_count; i++) {
        printf("Linking %s/%s to  %s/%s ... ", srcColl, dataList, destColl, dataList);
        fflush(stdout);

        status = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
        		  dataList, srcColl, "", "", dataList, destColl,
        		  D_INSERT_LINK);

        if (status != 0) 
            printf("<br>Error: In Linking %s/%s to %s/%s; <A href=\"srberror3.cgi?%i\">Error: %i</A><BR>", srcColl, dataList, destColl, dataList, status, status);
        else 
            printf("Done<BR>");
            
        fflush(stdout);            
        dataList += MAX_DATA_SIZE;
	}
    return (status);
}


int slinkSubcollsInColl(inStruct Sentries, mdasC_sql_result_struct *myresult, char *destColl, char *srcColl) 
{
    char *collList;
    char collName[2*MAX_TOKEN];
    char parcollName[MAX_TOKEN];
    int i, status;
    
    collList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
         
    for (i = 0; i < myresult->row_count; i++) {
        clSplitbychar (collList, parcollName, collName, '/');
        status = slinkColl(Sentries, collName, destColl, collList);
        collList += MAX_DATA_SIZE;
	}
    return (status);
}


int checkout()
{
  construction();
}

int offline()
{
  construction();
}





int accsByTicket()
{
  construction();
}

construction()
{
  printf("<B>Function Currently UnAvailable</B>\n");
}




int
isTicketRecursive (inStruct Sentries, char *ticketId, char *userName, char *domnName, char *ticketCollName)
{
    mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    char *tCN;
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    sprintf(qval[TICKET_C], " = '%s'", ticketId);
    sprintf(qval[TICKET_USER_C], " = '%s'",  userName);
    sprintf(qval[TICKET_USER_DOMAIN_C], " = '%s'", domnName);
    sprintf(qval[C_TICKET_RECURSIVE], " = 1 ");
    /*RTRTRT mar7,05 raja    selval[C_TICKET_RECURSIVE] = 1; */
    selval[C_TICKET_COLL_NAME] = 1;

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult, MAX_RESULT_NUM);
    if (status < 0 ) {
      clearSqlResult (&collResult);
      return (0);
    }
    else {
      tCN = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) &collResult,
        dcs_tname[C_TICKET_COLL_NAME], dcs_aname[C_TICKET_COLL_NAME]);
      if (tCN == NULL) {
	clearSqlResult (&collResult);
	return (0);
      }
      strcpy(ticketCollName,tCN);
      clearSqlResult (&collResult);
      return(1);
    }
}

int
getNetprefixForResource(inStruct Sentries, char *inResource, char *netPrefix)
{
    mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    char userName[MAX_TOKEN], domnName[MAX_TOKEN];
    int i, j;
    int status;
    char *npList, *tmpPtr;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    sprintf(qval[RSRC_NAME], " = '%s'", inResource);
    selval[RSRC_ADDR_NETPREFIX] = 1;
    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult, 
			       10);
    if (status < 0)
      return(status);
    if (collResult.row_count == 0)
      return (CLI_NO_ANSWER);
    npList = (char *) getFromResultStruct(
		 &collResult, dcs_tname[RSRC_ADDR_NETPREFIX], 
		 dcs_aname[RSRC_ADDR_NETPREFIX]);
    if (npList == NULL)
      return (CLI_NO_ANSWER);
    if ((tmpPtr =  strstr(npList,":")) != NULL)
      *tmpPtr = '\0';
    strcpy(netPrefix,npList);
    clearSqlResult (&collResult);
    return 0;
}

printSrbBot(char *message, char *button1, char *button2, char *button3) {
  
  printf("</td></tr></table>\n");
   /*********************** BUTTON AREA **************************/
  if (button1 != NULL) {
    fprintf(stdout,"<p>\n<center>\n<form METHOD=POST ACTION=\"\">\n");
    fprintf(stdout,"<input TYPE=button VALUE=\"%s\">\n",button1);
  }
  if (button2 != NULL) 
    fprintf(stdout,"<input TYPE=button VALUE=\"%s\">\n",button2);
  if (button3 != NULL) 
    fprintf(stdout,"<input TYPE=button VALUE=\"%s\">\n",button3);
  if (button1 != NULL) 
    fprintf(stdout,"</form></center><p>\n");
}

printSrbTop(char *objType, char *message, char *parName, char *childName, char *owner)
{
  /*********************** SCREEN TITLE *************************/
  /*fprintf (stdout,"<IMG SRC=\"%s/srbtop.gif\" width=100%%>\n", webPath);fflush(stdout); removed to make the image goaway*/
  
  
  /******************* FILE IDENTIFICATION **********************/

  /*
    fprintf(stdout,"<table border=0 cellspacing=0 cellpadding=0><tr><td  width=10%% valign=top><IMG SRC=\"%s/srbside.gif\"></td><td valign=top>\n", webPath); xhanged to make the image goaway*/

  fprintf(stdout,"<table border=0 cellspacing=0 cellpadding=0><tr><td  width=10%% valign=top></td><td valign=top>\n"); 
    fprintf (stdout,"<center><font color=000088 face='arial,sans-serif'>\n\
  <h2> %s </h2></font></center>\
<font face='arial,sans-serif'>\
<table border=0 cellspacing=0 cellpadding=0>",message);fflush(stdout);
    if (!strcmp(objType,"C")) 
      fprintf (stdout,"<tr><td>%s </td><td><b>  %s </td></tr>\n",mySrbStrings[26],
	       childName);
    else if (!strcmp(objType,"D")) 
      fprintf (stdout,"<tr><td>%s </td><td><b>  %s </td></tr>\n", mySrbStrings[191], childName);
    fprintf (stdout,"<tr><td>%s&nbsp;&nbsp;</td><td><b> %s </td></tr>\n", mySrbStrings[25],parName);
    fprintf(stdout,"<tr><td>%s </td><td><b>  %s</td></tr></table>\n <hr><p>", mySrbStrings[27], owner);fflush(stdout);

}

int
getRealCollName(inStruct Sentries, char *realCollName, char *inCollName)
{
    mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
      char *npList;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    sprintf(qval[DATA_GRP_NAME], " = '%s'", inCollName);
    selval[REAL_COLLECTION_NAME] = 1;
    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult, 
			       10);
    if (status < 0)
      return(status);
    if (collResult.row_count == 0)
      return (CLI_NO_ANSWER);
    npList = (char *) getFromResultStruct(
		 &collResult, dcs_tname[REAL_COLLECTION_NAME], 
		 dcs_aname[REAL_COLLECTION_NAME]);
    if (npList == NULL)
      return (CLI_NO_ANSWER);
    strcpy(realCollName,npList);
    clearSqlResult (&collResult);
    return 0;

}

int 
annoinsertNew(inStruct Sentries)
{
   if (strlen( Sentries->entries[1].val) > 0 )   {
     if (isDataInSRB(Sentries) !=  1) {
       printf("Data Object %s in %s Not Found\n",Sentries->entries[1].val,Sentries->entries[2].val);
       bRMap(1,0);
       return 0;
     }
   }
   if (strlen( Sentries->entries[1].val) == 0 )   {
     clSplitbychar (Sentries->entries[2].val,tmpCollName, tmpDataName, '/');
     printSrbTop("C", mySrbStrings[203],tmpCollName, tmpDataName, Sentries->entries[7].val);fflush(stdout);
     getCollectionAnno(Sentries, Sentries->entries[2].val,Sentries->entries[1].val, 0);
   }
   else {
     printSrbTop("D", mySrbStrings[203],
		 Sentries->entries[2].val,Sentries->entries[1].val,Sentries->entries[7].val);fflush(stdout);
     getDatasetAnno(Sentries, Sentries->entries[2].val,Sentries->entries[1].val, 0);
   }
   printf("<hr><p>\n");
   
  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);
  if ((tmpPtr= strstr(Sentries->entries[1].val,"@SHADOW=")) != NULL)
    *tmpPtr = '\0';


   if (strlen( Sentries->entries[1].val) > 0 )   {
     printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
     printf("<input type=hidden name=\"function\" value=\"insertDataAnnotation\">");
   }
   else {
     printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
     printf("<input type=hidden name=\"function\" value=\"insertCollAnnotation\">");
   }
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  if (atoi(Sentries->entries[6].val) < 0) {
    if ((tmpPtr = strstr(Sentries->entries[5].val,"/?SHADOW")) != NULL) {
      tmpPtr += 8;
      if (atoi(Sentries->entries[6].val) == -1)
	printf("<input type=hidden name=\"slinkval\" value=\"F:%s\">",tmpPtr);
      else
	printf("<input type=hidden name=\"slinkval\" value=\"D:%s\">",tmpPtr);
    }
    else
      printf("<input type=hidden name=\"slinkval\" value=\"\">");
  }
  else
    printf("<input type=hidden name=\"slinkval\" value=\"\">");

  if (strlen( Sentries->entries[1].val) > 0 )  
    printf("<table width=100% border=0 cellspacing=4 cellpadding=0><tr><td width=5%>&nbsp;</td><td align=right><b> %s </b></td><td><INPUT NAME=\"newmdname1\", VALUE= \"Comments\", SIZE=16,12></td></tr>\n", mySrbStrings[215]);
    else 
      printf("<table width=100% border=0 cellspacing=4 cellpadding=0><tr><td width=5%>&nbsp;</td><td align=right><b> Type </b></td><td><INPUT NAME=\"newmdname1\", VALUE= \"Comments\", SIZE=16,12></td></tr>\n");
      printf("<tr><td width=5%>&nbsp;</td><td align=right><b> %s </b></td><td><INPUT NAME=\"newmdval1\", VALUE= \"\", SIZE=40,12></td></tr></table>\n", mySrbStrings[66]);
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  printf("<BR><CENTER><INPUT TYPE=\"submit\" VALUE=\"%s\"></CENTER></FORM>\n", mySrbStrings[93]);

  printSrbBot(NULL,NULL,NULL,NULL);
  return 0;
}

int 
makeAnnoInsertForm(inStruct Sentries)
{

  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);
  if ((tmpPtr= strstr(Sentries->entries[1].val,"@SHADOW=")) != NULL)
    *tmpPtr = '\0';

   if (strlen( Sentries->entries[1].val) > 0 )   {
     if (isDataInSRB(Sentries) !=  1) {
       printf("Data Object %s in %s Not Found\n",Sentries->entries[1].val,Sentries->entries[2].val);
       bRMap(1,0);
       return 0;
     }
     printf("<B><FONT COLOR=#0000FF>Enter Annotation for Data Object: %s in Collection %s</FONT></B>\n",
	 Sentries->entries[1].val,Sentries->entries[2].val);
     printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
     printf("<input type=hidden name=\"function\" value=\"insertDataAnnotation\">");
    printf("<table><tr><th align=center>%s</th><th></th><th align=center>%s</th></tr>\n", mySrbStrings[215], mySrbStrings[66]);
   }
   else {
     printf("<B><FONT COLOR=#0000FF>Enter Annotation for Collection %s</FONT></B>\n",
	 Sentries->entries[2].val);
     printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
     printf("<input type=hidden name=\"function\" value=\"insertCollAnnotation\">");
    printf("<table><tr><th align=center>Type</th><th></th><th align=center>%s</th></tr>\n", mySrbStrings[66]);
   }
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  if (atoi(Sentries->entries[6].val) < 0) {
    if ((tmpPtr = strstr(Sentries->entries[5].val,"/?SHADOW")) != NULL) {
      tmpPtr += 8;
      if (atoi(Sentries->entries[6].val) == -1)
	printf("<input type=hidden name=\"slinkval\" value=\"F:%s\">",tmpPtr);
      else
	printf("<input type=hidden name=\"slinkval\" value=\"D:%s\">",tmpPtr);
    }
    else
      printf("<input type=hidden name=\"slinkval\" value=\"\">");
  }
  else
    printf("<input type=hidden name=\"slinkval\" value=\"\">");

  printf("<tr><td><INPUT NAME=\"newmdname1\", VALUE= \"Comments\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval1\", VALUE= \"\", SIZE=40,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname2\", VALUE= \"Comments\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval2\", VALUE= \"\", SIZE=40,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname3\", VALUE= \"Comments\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval3\", VALUE= \"\", SIZE=40,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname4\", VALUE= \"Comments\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval4\", VALUE= \"\", SIZE=40,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname5\", VALUE= \"Comments\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval5\", VALUE= \"\", SIZE=40,12></td></tr>\n\
</table>\n");
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[93]);
  return 0;
}


int
copyanno(inStruct Sentries)
{
  int i,j;
  char *srcColl, *srcObj, *trgColl, *trgObj;

  printf("Copying Annotations ...\n");fflush(stdout);
  srcColl = Sentries->entries[2].val;
  srcObj = Sentries->entries[1].val;
  trgColl = Sentries->entries[Sentries->m-1].val;
  trgObj = Sentries->entries[Sentries->m].val;
  i = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
		       trgObj, trgColl,
		       "","", srcColl,srcObj,
		       D_COPY_ANNOTATIONS_FROM_DATA_TO_DATA);
  if (i != 0) 
    printf("<br>Error: In Copying Metadata; <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", i,i);
  else 
    printf("Done");
  bRMap(2,0);
  return(0);  
}

int copyannoform(inStruct Sentries)
{
  char *newColl;
  
  if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
  if (strstr(Sentries->entries[2].val,"&"))
     escape_url(Sentries->entries[2].val);
  if ( Sentries->m > DATAFORMNUMOFARGS)
    newColl = Sentries->entries[Sentries->m-1].val;
  else
    newColl = Sentries->entries[2].val;

  printf("<CENTER><B><FONT COLOR=#0000FF>%s</FONT><FONT COLOR=#FF0000>%s/%s</FONT></B></CENTER>\n", mySrbStrings[226],Sentries->entries[2].val,Sentries->entries[1].val);

  i = getDataNamesinCollection(Sentries, newColl); 
  if (i < 0) {
    printf("<STRONG><FONT COLOR=#FF0000>No Data Objects  Found in Collection:</FONT><FONT COLOR=#0000FF> %s</FONT>\n",newColl); 
    return(0);
  }
  printf("<TABLE><TR><TD>\n");
  printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"copyanno\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"rsrc\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
  printf("<input type=hidden name=\"owner\" value=\"%s\">",Sentries->entries[7].val);
  printf("<input type=hidden name=\"newcoll\", value=\"%s\">",newColl); /* 8 */
  printf("<STRONG><FONT COLOR=#FF0000>%s</FONT></STRONG> %s</TD>\n", mySrbStrings[217], Sentries->selectData); /* 9 */
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);fflush(stdout);
  printf("<TR><TD ALIGN=CENTER><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM></TD></TR>\n", mySrbStrings[225]);fflush(stdout);
  
  if ( Sentries->m == DATAFORMNUMOFARGS ) {
    printf("<TR><TD> <HR SIZE=3 NOSHADE> <STRONG>%s</STRONG> </TD></TR>\n", mySrbStrings[218]);
    printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
    printf("<input type=hidden name=\"function\" value=\"copyannoform\">");
    printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
    printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
    printf("<input type=hidden name=\"rsrc\" value=\"%s\">",Sentries->entries[3].val);
    printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
    printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
    printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
    printf("<input type=hidden name=\"owner\" value=\"%s\">",Sentries->entries[7].val);
    getAccsCollectionsAsSelect(Sentries, "all",NULL);                 /*   8 **/
    printf("<TR><TD><STRONG><FONT COLOR=#FF0000>%s</FONT></STRONG> %s</TD>\n", mySrbStrings[220],
	   Sentries->selectData);fflush(stdout);
    printf("<input type=hidden name=newdname value=\"\">\n");  /* 9 */
    printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
    printf("<TR><TD  ALIGN=CENTER><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM></TD></TR>\n", mySrbStrings[223]);
  }
  printf("</TABLE>\n");
  return 0;
}
int
deleteAnno(inStruct Sentries)
{
  int ii;
  char tmpStr[MAX_TOKEN * 2];

  printf("Removing Annotations for File %s/%s ...", 
	   Sentries->entries[2].val,Sentries->entries[1].val);
  ii = srbModifyDataset(Sentries->conn,  MDAS_CATALOG, Sentries->entries[1].val, Sentries->entries[2].val,
               "","", "", "", D_DELETE_ANNOTATIONS);
  if (ii < 0) {
    printf("<BR>Error removing annotations: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",ii,ii);
    srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
    bRMap(2,0);
    return;
  }

  printf("Done");
  bRMap(2,1);
  return;
}

int
makeAnnoInsertPage(inStruct Sentries, int m)
{
   int i;

   if (strstr(Sentries->entries[1].val,"&"))
	escape_url(Sentries->entries[1].val);
printf("Content-type: text/html%c%c",10,10);fflush(stdout);

   printf ("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\"> </HEAD><FRAMESET rows=\"40%,60%\" BORDER=no>", "MYSRB V9.0");
   printf("<FRAME SRC=\"mysrb331.cgi?function=anno&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\" name=\"rtbttop\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[m].val);
   printf("<FRAME SRC=\"mysrb331.cgi?function=annoinsertionform&dataname=%s&collection=%s&rsrcname=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\" name=\"rtbtbot\">",Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val,Sentries->entries[m].val);
   printf("</FRAMESET> </body></html>");
}


int
makeAnnoPage(inStruct Sentries)
{

   if (strlen( Sentries->entries[1].val) == 0 )   {
     getCollectionAnno(Sentries, Sentries->entries[2].val,Sentries->entries[1].val, 0);
   }
   else {
     getDatasetAnno(Sentries, Sentries->entries[2].val,Sentries->entries[1].val, 0);
   }

}
int
showAnno(inStruct Sentries)
{

   if (strlen( Sentries->entries[1].val) == 0 )   {
     clSplitbychar (Sentries->entries[2].val,tmpCollName, tmpDataName, '/');
     printSrbTop("C", mySrbStrings[197],tmpCollName, tmpDataName, Sentries->entries[7].val);
     getCollectionAnno(Sentries, Sentries->entries[2].val,Sentries->entries[1].val, 0);
   }
   else {
     printSrbTop("D", mySrbStrings[197],
		 Sentries->entries[2].val,Sentries->entries[1].val,Sentries->entries[7].val);
     getDatasetAnno(Sentries, Sentries->entries[2].val,Sentries->entries[1].val, 0);
   }
   printSrbBot(NULL,NULL,NULL,NULL);
}

int
makeDeleteAnnoForm(inStruct Sentries)
{
  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> %s </TITLE></HEAD><body background=\"%s/srb3.jpg\"><CENTER>\n", "MYSRB V9.0", webPath);
  printf("<TABLE><TR><TD><B><FONT COLOR=#0000FF>%s %s/%s</FONT></B></TD></TR>\n", mySrbStrings[230],
	 Sentries->entries[2].val,Sentries->entries[1].val);
  printf("<TR><TD align=center><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"deleteanno\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"seek\" value=\"%s\">",Sentries->entries[3].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"datatype\" value=\"%s\">",Sentries->entries[5].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">",Sentries->entries[6].val);
  printf("<input type=hidden name=\"owner\" value=\"%s\">",Sentries->entries[7].val);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->entries[Sentries->m].val);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[228]);
   printf("<B><FONT COLOR=#0000FF>%s</FONT></B></TD></TR></TABLE></body></html>\n", mySrbStrings[229]);
  return 0;
}

int
modifyAnnoData(inStruct Sentries, int m)
{
  int i,j;

  printf("%s...\n",aaa[33]);

  for (i = 1; i <= m ; i++ ) {
    if (strstr(Sentries->entries[i].name,"anncb") != Sentries->entries[i].name)
      continue;
    if (strlen(Sentries->entries[i+4].val) > 0) {
      sprintf(tmpoStr,"%s@@@%s",Sentries->entries[i+4].val,Sentries->entries[i+3].val);
      j = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
			   Sentries->entries[i-2].val, Sentries->entries[i-1].val,
			   "","",
			   tmpoStr,Sentries->entries[i+5].val,
			     D_UPDATE_ANNOTATIONS);
    }
    else {
            j = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
			   Sentries->entries[i-2].val, Sentries->entries[i-1].val,
			   "","",
			   Sentries->entries[i+5].val,"",
			     D_DELETE_ANNOTATIONS);
    }
    if (j != 0)  {
    printf("<br>Error: In Updating Annotations for time-stamp: %s<A href=\"srberror3.cgi?%i\">Error: %i</A>\n",Sentries->entries[i+5].val, j,j);
    bRMap(2,0);
    return(0);
    }
    i +=4;
  }
  bRMap(2,0);
  printf("Done");
  
}

int 
modifyAnnoFormNew(inStruct Sentries)
{

   if (strlen( Sentries->entries[1].val) == 0 )   {
     clSplitbychar (Sentries->entries[2].val,tmpCollName, tmpDataName, '/');
     printSrbTop("C",mySrbStrings[212],tmpCollName, tmpDataName, Sentries->entries[7].val);fflush(stdout);
     getCollectionAnno(Sentries, Sentries->entries[2].val,Sentries->entries[1].val,1);
   }
   else {
     printSrbTop("D",mySrbStrings[212],
		 Sentries->entries[2].val,Sentries->entries[1].val,Sentries->entries[7].val);fflush(stdout);
     getDatasetAnno(Sentries, Sentries->entries[2].val,Sentries->entries[1].val,1);
   }
   printSrbBot(NULL,NULL,NULL,NULL);
}

/***
modifyAnnoForm(int m)
{

   if (strlen( Sentries->entries[1].val) == 0 )   {
     getCollectionAnno(entries[2].val,entries[1].val,1);
   }
   else {
     getDatasetAnno(entries[2].val,entries[1].val,1);
   }
}
**/


int
prtAnnoData (inStruct Sentries, mdasC_sql_result_struct *myresult, int updateFlag)
{
  int i,j;
  char *collList, *dataList;
  char *anno,*anno_time,*anno_user,*anno_domn,*anno_pos;

  if (first) {
    first = 0;
    Sentries->globalCounter = 1;
    strcpy(tmpDataName,"");
    strcpy(tmpCollName,"");

    if ( updateFlag == 1) {
      fprintf (stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" >\n\
<input type=hidden name=\"function\" value=\"modifyanno\">\n",Sentries->frameStr);
      fprintf (stdout,"<TABLE><TR><TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH></TR>\n", mySrbStrings[211], mySrbStrings[82], mySrbStrings[215], mySrbStrings[66]);
    }
  }
  anno =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[DATA_ANNOTATION], dcs_aname[DATA_ANNOTATION]);
  anno_user =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[DATA_ANNOTATION_USERNAME], dcs_aname[DATA_ANNOTATION_USERNAME]);
  anno_domn =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[DATA_ANNOTATION_USERDOMAIN], dcs_aname[DATA_ANNOTATION_USERDOMAIN]);
  anno_time =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[DATA_ANNOTATION_TIMESTAMP], dcs_aname[DATA_ANNOTATION_TIMESTAMP]);
  anno_pos =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[DATA_ANNOTATION_POSITION], dcs_aname[DATA_ANNOTATION_POSITION]);
  dataList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
  collList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

  if (updateFlag == 0) {  
    fprintf (stdout,"<p><font color=000088><b>%s <i> %s </i></b></font>\n\
<table width=100%% border=0 cellspacing=4 cellpadding=0>\n\
<tr><td width=5%%>&nbsp;</td>\
<td><i><font color=000088><b>%s</b> &nbsp;&nbsp;\
 %s</td></tr>\n", mySrbStrings[59], dataList, mySrbStrings[194], mySrbStrings[193]);
  }
  for (i = 0; i < myresult->row_count; i++) {
    if (strcmp(dataList,tmpDataName) || strcmp(collList,tmpCollName)) {
      /*      fprintf (stdout,"<P><FONT COLOR=#0000FF>%s/%s</FONT><P>\n",collList,dataList);*/
      strcpy(tmpDataName,dataList);
      strcpy(tmpCollName,collList);
    }
    if (updateFlag == 0) {
      /**
      fprintf(stdout,"<BR><FONT COLOR=#FF0000>Annotator:</FONT> %s@%s on %s\n",
	      anno_user,anno_domn,anno_time);
      fprintf(stdout,"<BR><FONT COLOR=#FF0000>Position:</FONT> %s\n",anno_pos);
      fprintf (stdout,"<BR><FONT COLOR=#FF0000>Annotation:</FONT> %s\n",anno);
      fprintf (stdout,"<HR>");
      **/
      fprintf (stdout,"<tr><td>&nbsp;</td><td><b>%s:</b> &nbsp;&nbsp;  %s  &lt; %s@%s , %s &gt;</td></tr>\n",  anno_pos,anno,anno_user,anno_domn,anno_time);
    }
    else { /**  updateFlag == 1  */
      fprintf(stdout,"<input type=hidden name=\"data\" value=\"%s\">\n",dataList);
      fprintf(stdout,"<input type=hidden name=\"coll\" value=\"%s\">\n",collList);
      fprintf(stdout,"<TR><TD><INPUT TYPE=CHECKBOX NAME=\"anncb%i\">\
<INPUT TYPE=HIDDEN NAME=\"annuser%i\" VALUE=\"%s\" >\
<INPUT TYPE=HIDDEN NAME=\"anndomn%i\" VALUE=\"%s\" ><TD> %s@%s </TD>\
<TD><INPUT TYPE=TEXT NAME=\"annpos%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=TEXT NAME=\"annanno%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=TEXT NAME=\"anntime%i\" VALUE=\"%s\" ></TD></TR>\n",
	      Sentries->globalCounter,Sentries->globalCounter,anno_user,Sentries->globalCounter,anno_domn,
	      anno_user,anno_domn,
	      Sentries->globalCounter,anno_pos,Sentries->globalCounter,anno,Sentries->globalCounter,anno_time);
    }
    Sentries->globalCounter++;
    anno += MAX_DATA_SIZE;
    anno_user += MAX_DATA_SIZE;
    anno_domn += MAX_DATA_SIZE;
    anno_time += MAX_DATA_SIZE;
    anno_pos += MAX_DATA_SIZE;
    dataList += MAX_DATA_SIZE;
    collList += MAX_DATA_SIZE;

  }
   fprintf(stdout,"</table>\n");
}

int
collectionCreationForm(inStruct Sentries)
{

  printf("<CENTER> <B><FONT COLOR=#0000FF>SRB Collection Creation</FONT></B>\n");
  printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\" >\n");
  printf("<input type=hidden name=\"function\" value=\"createCollectionForm\">\n<BR>\n");
  printf("<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[25]);
  getValuesAsSelect(Sentries, COLLECTION_NAME, Sentries->selectData, "/home/sekar.sdsc/metatest/test", "");
  printf("%s\n", Sentries->selectData);
  printf("<input type=hidden name=\"session\" value=\"%s\">",Sentries->pidStr);
  printf("<BR>\n<INPUT TYPE=\"submit\" VALUE=\"Send\">\n</FORM>\n");
  return;
}

int
createCollectionForm(inStruct Sentries)
{
  mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    char contSelect[MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    char *collection, *meta0, *meta1, *meta2, *meta3, *meta4;

   if (strstr(Sentries->entries[1].val,"&"))
     escape_url(Sentries->entries[1].val);
   collection = Sentries->entries[1].val;
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
   selval[METADATA_NUM_COLL] = 1;
   selval[UDSMD_COLL0] = 1;
   selval[UDSMD_COLL1] = 1;
   selval[UDSMD_COLL2] = 1;
   selval[UDSMD_COLL3] = 1;
   selval[UDSMD_COLL4] = 1;
   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      MAX_RESULT_NUM);


   if (status < 0 &&  status != MCAT_INQUIRE_ERROR) {
     printf("Metadat Access Error for Parent Collection: <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status, status);
     bRMap(1,0);
     printf("</body></html>");
     return;
   }
   printf("<FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\"     >\n\
<input type=hidden name=\"function\" value=\"makecoll\">\n\
<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT> \
<INPUT NAME=\"parcoll\", VALUE= \"%s\" , SIZE=20,12><BR>\n\
<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT> \
<INPUT NAME=\"newcoll\", VALUE= \"\" , SIZE=20,12> <BR> \
<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT> \n", mySrbStrings[25],Sentries->entries[1].val, mySrbStrings[169], mySrbStrings[168]);
   /*<INPUT NAME=\"collcont\", VALUE= \"inherit from parent\" , SIZE=20,12> <BR>\n\ */

    sprintf(contSelect, " like '%%/%s.%s/%%'", Sentries->srbUser, Sentries->userDomain);
    getValuesAsSelectWithCond(Sentries, CONTAINER_NAME, "inherit from parent","",contSelect);
    printf("%s\n", Sentries->selectData);

   printf("<table>\
<tr><th align=center>%s</th><th></th><th align=center>%s</th><th align=center>%s</th><th align=center>%s</th><th align=center>%s</th></tr>\n", mySrbStrings[77], mySrbStrings[78], mySrbStrings[62], mySrbStrings[64], mySrbStrings[63]);
   filterDeleted (&myResult);
   Sentries->first = 1;
   meta0 =  (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) &myResult,
         dcs_tname[UDSMD_COLL0], dcs_aname[UDSMD_COLL0]);
   meta1 =  (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) &myResult,
	 dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL1]);
   meta2 =  (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) &myResult,
	 dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL2]);
   meta3 =  (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) &myResult,
	 dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL3]);
   meta4 =  (char *) getFromResultStruct(
         (mdasC_sql_result_struct *) &myResult,
	 dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL4]);
   for (i = 0; i < myResult.row_count; i++) {
    if (!strcmp(meta0,"COLLMETAATTR"))
      printf ("<tr><td><INPUT NAME=\"prtmdname%i\", VALUE= \"Metadata for Data\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"prtmdval%i\", VALUE= \"%s\", SIZE=16,12></td><td><INPUT NAME=\"prtvalunit%i\", VALUE= \"%s\", SIZE=10,10></td><td><INPUT NAME=\"prtdefval%i\", VALUE= \"%s\", SIZE=16,12></td><td><INPUT NAME=\"prtcomm%i\", VALUE= \"%s\", SIZE=16,12></td></tr>\n",i,i,meta1,i,meta2,i,meta3,i,meta4);
    else
      printf ("<tr><td><INPUT NAME=\"prtmdname%i\", VALUE= \"%s\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"prtmdval%i\", VALUE= \"%s\", SIZE=16,12></td><td><INPUT NAME=\"prtvalunit%i\", VALUE= \"%s\", SIZE=10,10></td><td><INPUT NAME=\"prtdefval%i\", VALUE= \"%s\", SIZE=16,12></td><td><INPUT NAME=\"prtcomm%i\", VALUE= \"%s\", SIZE=16,12></td></tr>\n",i,meta0,i,meta1,i,meta2,i,meta3,i,meta4);
    meta0 += MAX_DATA_SIZE;
    meta1 += MAX_DATA_SIZE;
    meta2 += MAX_DATA_SIZE;
    meta3 += MAX_DATA_SIZE;
    meta4 += MAX_DATA_SIZE;
  }
printf("<tr><td><INPUT NAME=\"newmdname1\", VALUE= \"\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval1\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit1\", VALUE= \"\", SIZE=10,10></td><td><INPUT NAME=\"newdefval1\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newcomm1\", VALUE= \"\", SIZE=16,12></td></tr>\n\
        <tr><td><INPUT NAME=\"newmdname2\", VALUE= \"\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval2\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit2\", VALUE= \"\", SIZE=10,10></td><td><INPUT NAME=\"newdefval2\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newcomm2\", VALUE= \"\", SIZE=16,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname3\", VALUE= \"Metadata for Data\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval3\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit3\", VALUE= \"\", SIZE=10,10></td><td><INPUT NAME=\"newdefval3\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newcomm3\", VALUE= \"\", SIZE=16,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname4\", VALUE= \"Metadata for Data\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval4\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit4\", VALUE= \"\", SIZE=10,10></td><td><INPUT NAME=\"newdefval4\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newcomm4\", VALUE= \"\", SIZE=16,12></td></tr>\n\
<tr><td><INPUT NAME=\"newmdname5\", VALUE= \"Metadata for Data\", SIZE=16,12></td><td><b>=</b></td><td><INPUT NAME=\"newmdval5\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newvalunit5\", VALUE= \"\", SIZE=10,10></td><td><INPUT NAME=\"newdefval5\", VALUE= \"\", SIZE=16,12></td><td><INPUT NAME=\"newcomm5\", VALUE= \"\", SIZE=16,12></td></tr>\n\
</table>\n\
%s\n", mySrbStrings[170]);
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM></BODY></HTML>\n", mySrbStrings[172]);
 return;
}


int
getCollectionAnno(inStruct Sentries, char *collection, char *dataName, int updateFlag)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;

fflush(stdout);
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    selval[DATA_GRP_NAME] = 1;
      selval[COLL_ANNOTATION] = 14;
      selval[COLL_ANNOTATION_TIMESTAMP] = 1;
      selval[COLL_ANNOTATION_USERNAME] = 1;
      selval[COLL_ANNOTATION_USERDOMAIN] = 1;
      selval[COLL_ANNOTATION_TYPE] = 1;


    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
                               MAX_RESULT_NUM);

    if (status < 0)     {
        if (status != MCAT_INQUIRE_ERROR)
          printf("Annotation Access <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", status, status);
	else
	  printf("<HR>%s\n", mySrbStrings[29]);
    }
    else {
      filterDeleted (&collResult);

      prtAnnoColl(Sentries, &collResult, updateFlag);
      while (collResult.continuation_index >= 0) {
        clearSqlResult (&collResult);
        status = srbGetMoreRows(Sentries->conn, 0,
                                collResult.continuation_index,
                                &collResult, MAX_RESULT_NUM);
        if (collResult.result_count == 0  || status != 0)
          break;
        filterDeleted (&collResult);
        prtAnnoColl(Sentries, &collResult, updateFlag);
      }
      clearSqlResult (&collResult);
    }
    /***
    if (updateFlag == 1) {
      fprintf(stdout,"</TABLE><input type=hidden name=\"session\" value=\"%s\">\
<FONT COLOR=#0000FF>Please check the Change boxes of lines that have been changed before submitting<BR>To delete an annotation delete the corresponding 'Annotation' string<BR>\n</FONT>\
<INPUT TYPE=SUBMIT></FORM>\n",Sentries->pidStr);
    }
    ***/
     return(0);
}

int
getCollectionInfo(inStruct Sentries)
{
    char *collection = Sentries->entries[2].val;
    char *dataName = Sentries->entries[1].val;
    mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int i, j;
    unsigned int k;
    int DSIndex, inDSIndex;
    int collIndex, inCollIndex;
    int status;
    int collLike;
    char urlcollection[MAX_TOKEN];
    char parurlcollection[MAX_TOKEN];
    char tmpStr[MAX_TOKEN];
    char *tmpPtr;
    
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ", collection);
      Sentries->likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'", collection);
    }
    if (strlen(dataName) > 0) {
      if (strstr(dataName,"^")) {
         make_like_for_mdasmysrb(dataName);
         client_set_qval_data_cond(dataName,qval);
      }
      else {
         make_like_for_mdasmysrb(dataName);
         sprintf(qval[DATA_NAME], " like '%s'  ESCAPE '\\' ",dataName);
      }
    }
    
    /**** To determine what DS and collection range to print ****/
    /**** get info as "xxx-yyy" with xxx starting position (integer) for datasets 
          and yyy for collections ****/          
     if (tmpPtr = strstr(Sentries->entries[5].val, "-")) {
        inCollIndex = atoi(tmpPtr+1);
        tmpPtr[0] = '\0';
        inDSIndex = atoi(Sentries->entries[5].val);
    }
    else {
        inCollIndex = 0;
        inDSIndex = 0;    
    }
    /***********************/
    
    selval[DATA_GRP_NAME] = 1;
    selval[DATA_NAME] = 1;
    selval[SIZE] = 1;
    selval[DATA_TYP_NAME] = 1;
    selval[DATA_OWNER] = 1;
    selval[DATA_OWNER_DOMAIN] = 1;
    selval[REPL_TIMESTAMP] = 1;
    selval[DATA_VER_NUM] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[CONTAINER_NAME] = 1;
#ifdef DATACOMMENTS
    selval[DATA_COMMENTS] = 14;
#endif
    selval[PATH_NAME] = 1;
    strcpy(urlcollection,collection);
    escape_url(urlcollection);
    clSplitbychar (urlcollection, parurlcollection, tmpStr, '/');
    if (Sentries->isTicket)
      printCommonTicketButtons(Sentries /*, urlcollection, parurlcollection*/);
    else
      printCommonButtons(Sentries, urlcollection, parurlcollection);

    printf("<FONT FACE='arial,sans-serif' SIZE=4 COLOR=#000000>%s</FONT>\n",collection);

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult, MAX_RESULT_NUM);

      fprintf(stdout,"<TABLE CELLSPACING=0 CELLPADDING=0>\n");      fflush(stdout);
      
    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Collection Access<A href=\"srberror2.cgi?%i\">Error: %i</A> \n", status, status);
    }
    else {
      DSIndex = 0;
      
      /** print results only if we want the first 1-100 **/
      if (DSIndex == inDSIndex) {
          filterDeleted (&collResult);
          Sentries->first = 1; Sentries->rowCntr = 1;
          /*printf("<TABLE CELLSPACING=0 CELLPADDING=0 RULES=ALL>\n");*/
          if (Sentries->likeCollFlag) {
    
    		    prtDataLongWeb(Sentries, &collResult, CHK_COLL);   
          }
          
          else {
    
    		    prtDataLongWeb(Sentries, &collResult, NO_CHK_COLL);   
          }
          fflush(stdout);
      }
      
      if (collResult.continuation_index >= 0) { 

          while (collResult.continuation_index >= 0) {

            	clearSqlResult (&collResult);
            	status = srbGetMoreRows(Sentries->conn, 0, collResult.continuation_index,
            				&collResult, MAX_RESULT_NUM);
            	if (collResult.result_count == 0  || status != 0)
            	    break;
            	    
            	DSIndex++;
            	
            	/** is it the range we want to print ? **/
            	if (DSIndex == inDSIndex) {
                	filterDeleted (&collResult);
                	Sentries->first = 1; Sentries->rowCntr = 1+DSIndex*MAX_RESULT_NUM;
                	if (Sentries->likeCollFlag) {
                      prtDataLongWeb(Sentries, &collResult, CHK_COLL);   
                	}
                	else {
                      prtDataLongWeb(Sentries, &collResult, NO_CHK_COLL);   
                	}
            	}
          }
          
          /** table row for more dataset results **/
          fprintf(stdout, "<TR VALIGN=\"BASELINE\">\n");
          fprintf(stdout, "<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\" target=\"bottom%s\" ><TD colspan=5><FONT face=\"arial\">Show Datasets:</FONT>", Sentries->pidStr);
	  if (Sentries->isTicket)
	    fprintf(stdout, "<input type=\"hidden\" name=\"function\" value=\"ticketdata\">");
	  else
	    fprintf(stdout, "<input type=\"hidden\" name=\"function\" value=\"data\">");    
          fprintf(stdout, "<input type=\"hidden\" name=\"dataname\" value=\"\">");
          fprintf(stdout, "<input type=\"hidden\" name=\"collection\" value=\"%s\">", collection);
          fprintf(stdout, "<input type=\"hidden\" name=\"seek\" value=\"0\">");
          fprintf(stdout, "<input type=\"hidden\" name=\"copynum\" value=\"0\">");
          fprintf(stdout, "<SELECT SIZE=1 NAME=\"range\">");
                    
          for (k=0; k <= DSIndex; k++) {
                if (k == inDSIndex+1) {
                    fprintf(stdout, "<OPTION SELECTED VALUE=\"%d-%d\">%d - %d", k, inCollIndex, 1+k*MAX_RESULT_NUM, (k+1)*MAX_RESULT_NUM);
                }
                else {
                    fprintf(stdout, "<OPTION VALUE=\"%d-%d\">%d - %d", k, inCollIndex, 1+k*MAX_RESULT_NUM, (k+1)*MAX_RESULT_NUM);
                }
          }
          
          fprintf(stdout, "</SELECT>");          
          fprintf(stdout, "<input type=\"hidden\" name=\"vernum\" value=\"1\">");
          fprintf(stdout, "<input type=\"hidden\" name=\"owner\" value=\"%s\">", Sentries->entries[7].val);
          fprintf(stdout, "<input type=\"hidden\" name=\"session\" value=\"%s\">", Sentries->pidStr);
          fprintf(stdout,"<input type=\"submit\" value=\"Go\">");      
          fprintf(stdout, "</TD></FORM></TR>\n");
          /*****************/          

      }
      
      clearSqlResult (&collResult);
    }

    if (filesfd != NULL && filesfd != stdout) {
      fflush(filesfd);
      fseek(filesfd, 0, SEEK_SET);
      while ((j = fread (buf,1,60000,filesfd)) > 0) {
	buf[j] = '\0';
	printf("%s",buf);
      }
    }
  
    Sentries->first = 1; Sentries->rowCntr = 1;
    status = querySubCollInfo(Sentries, Sentries->conn, 0, collection, MAX_RESULT_NUM, &collResult);
    if (status ==  0) {
            collIndex = 0;
		    
		    /** print results only if we want the first 1-100 **/
		    if (collIndex == inCollIndex) {
    		    prtCollWeb(Sentries, &collResult);          
                fflush(stdout);
		    }
		    
		    if (collResult.continuation_index >= 0) {
		        
                while (collResult.continuation_index >= 0) {
                    clearSqlResult (&collResult);
                    status = srbGetMoreRows(Sentries->conn, 0, collResult.continuation_index,
                    		&collResult, MAX_RESULT_NUM);
                    if (collResult.result_count == 0  || status != 0)
                        return;
                    
                    collIndex++;
                    
                    /** is it the collection range we want to print ? **/
                    if (collIndex == inCollIndex) {
                        filterDeleted (&collResult);
                        Sentries->rowCntr = 1+collIndex*MAX_RESULT_NUM;
                        prtCollWeb(Sentries, &collResult);         
                        fflush(stdout);
                    }
                }
                
                /** table row for more collection results **/
                fprintf(stdout, "<TR VALIGN=\"BASELINE\">\n");
                fprintf(stdout, "<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\" target=\"bottom%s\" ><TD colspan=5><FONT face=\"arial\">Show Collections:&nbsp;</FONT>", Sentries->pidStr);
		if (Sentries->isTicket)
		  fprintf(stdout, "<input type=\"hidden\" name=\"function\" value=\"ticketdata\">");
		else
		  fprintf(stdout, "<input type=\"hidden\" name=\"function\" value=\"data\">");    
                fprintf(stdout, "<input type=\"hidden\" name=\"dataname\" value=\"\">");
                fprintf(stdout, "<input type=\"hidden\" name=\"collection\" value=\"%s\">", collection);
                fprintf(stdout, "<input type=\"hidden\" name=\"seek\" value=\"0\">");
                fprintf(stdout, "<input type=\"hidden\" name=\"copynum\" value=\"0\">");
                fprintf(stdout, "<SELECT SIZE=1 NAME=\"range\">");
                        
                for (k=0; k <= collIndex; k++) {
                    if (k == inCollIndex+1) {
                        fprintf(stdout, "<OPTION SELECTED VALUE=\"%d-%d\">%d - %d", inDSIndex, k, 1+k*MAX_RESULT_NUM, (k+1)*MAX_RESULT_NUM);
                    }
                    else {
                        fprintf(stdout, "<OPTION VALUE=\"%d-%d\">%d - %d", inDSIndex, k, 1+k*MAX_RESULT_NUM, (k+1)*MAX_RESULT_NUM);
                    }
                }
                
                fprintf(stdout, "</SELECT>");                
                fprintf(stdout, "<input type=\"hidden\" name=\"vernum\" value=\"1\">");
                fprintf(stdout, "<input type=\"hidden\" name=\"owner\" value=\"%s\">", Sentries->entries[7].val);
                fprintf(stdout, "<input type=\"hidden\" name=\"session\" value=\"%s\">", Sentries->pidStr);
                fprintf(stdout,"&nbsp;<input type=\"submit\" value=\"Go\">");      
                fprintf(stdout, "</TD></FORM></TR>\n");
                /*****************/ 
            
            }            
            
    }
    fprintf(stdout,"</TABLE><P>\n");
    if (Sentries->isTicket)
      printCommonTicketButtons(Sentries /*, urlcollection, parurlcollection*/);
    else
      printCommonButtons(Sentries, urlcollection, parurlcollection);

}


int
getCollectionInfo_simple(inStruct Sentries)
{
    char *collection = Sentries->entries[2].val;
    char *dataName = Sentries->entries[1].val;
    mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;
    char urlcollection[MAX_TOKEN];
    char parurlcollection[MAX_TOKEN];
    char tmpStr[MAX_TOKEN];
    
    
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ", collection);
      Sentries->likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'", collection);
    }
    if (strlen(dataName) > 0) {
      if (strstr(dataName,"^")) {
         make_like_for_mdasmysrb(dataName);
         client_set_qval_data_cond(dataName,qval);
      }
      else {
         make_like_for_mdasmysrb(dataName);
         sprintf(qval[DATA_NAME], " like '%s'  ESCAPE '\\' ",dataName);
      }
    }
    selval[DATA_GRP_NAME] = 1;
    selval[DATA_NAME] = 1;
    selval[SIZE] = 1;
    selval[DATA_TYP_NAME] = 1;
    selval[DATA_OWNER] = 1;
    selval[DATA_OWNER_DOMAIN] = 1;
    selval[REPL_TIMESTAMP] = 1;
    selval[DATA_VER_NUM] = 1;
    selval[DATA_REPL_ENUM] = 1;
    selval[PHY_RSRC_NAME] = 1;
    selval[CONTAINER_NAME] = 1;
#ifdef DATACOMMENTS
    selval[DATA_COMMENTS] = 14;
#endif
    selval[PATH_NAME] = 1;
    strcpy(urlcollection,collection);
    escape_url(urlcollection);
    clSplitbychar (urlcollection, parurlcollection, tmpStr, '/');
    
  printf("<script language=\"javascript\">\n\
<!--\n\
function MouseOver(elem)\n\
{	elem.style.backgroundColor = \"#FABFBF\";\n\
	//elem.style.backgroundColor = \"#DCE8FF\";\n\
}\n\
function MouseOut(elem)\n\
{	elem.style.backgroundColor = \"#FFFFFF\";\n\
}\n\
//-->\n\
</script>");     

      
    if (Sentries->isTicket)
      printCommonTicketButtons(Sentries /*, urlcollection, parurlcollection*/);
    else
      printCommonButtons(Sentries, urlcollection, parurlcollection);

    printf("<BR>");

    printf("<FONT SIZE=4 COLOR=#000000>%s</FONT><BR><BR>\n", collection);  
      
    printf("<br>");

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult, MAX_RESULT_NUM);
     
      fprintf(stdout, "<table align=center border=0 cellpadding=1 cellspacing=0 bgcolor=\"#006699\">");

      fprintf(stdout,"<TR><TD><TABLE CELLSPACING=0 CELLPADDING=2>\n");      fflush(stdout);
    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Collection Access<A href=\"srberror2.cgi?%i\">Error: %i</A> \n", status, status);
    }
    else {
      filterDeleted (&collResult);
      Sentries->first = 1; Sentries->rowCntr = 1;
      /*printf("<TABLE CELLSPACING=0 CELLPADDING=0 RULES=ALL>\n");*/
      if (Sentries->likeCollFlag) {

	        prtDataLongWeb_simple(Sentries, &collResult, CHK_COLL);
  
      }
      
      else {

	        prtDataLongWeb_simple(Sentries, &collResult, NO_CHK_COLL);
 
      }
      
      fflush(stdout);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0, collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	if (Sentries->likeCollFlag) {

	        prtDataLongWeb_simple(Sentries, &collResult, CHK_COLL);
  
	}
	else {

	        prtDataLongWeb_simple(Sentries, &collResult, NO_CHK_COLL);
  
	}
      }

      clearSqlResult (&collResult);
    }

    if (filesfd != NULL && filesfd != stdout) {
      fflush(filesfd);
      fseek(filesfd, 0 , SEEK_SET);
      while ((j = fread (buf,1,60000,filesfd)) > 0) {
	buf[j] = '\0';
	printf("%s",buf);
      }
    }

    Sentries->first = 1; Sentries->rowCntr = 1;
    status = querySubCollInfo(Sentries, Sentries->conn, 0, collection, MAX_RESULT_NUM, &collResult);
    if (status ==  0) {

	        prtCollWeb_simple(Sentries, &collResult);
     
      fflush(stdout);
      while (collResult.continuation_index >= 0) {
        clearSqlResult (&collResult);
        status = srbGetMoreRows(Sentries->conn, 0, collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
        if (collResult.result_count == 0  || status != 0)
	  return;
	filterDeleted (&collResult);
	
        /* Print the sub-collection */
	        prtCollWeb_simple(Sentries, &collResult);
        

     fflush(stdout);
	
      }
    }
    fprintf(stdout,"</TABLE></TD></TR></TABLE><P>\n");
    if (Sentries->isTicket)
      printCommonTicketButtons(Sentries /*, urlcollection, parurlcollection*/);
    else
      printCommonButtons(Sentries, urlcollection, parurlcollection);

}


int
getCollectionMeta(inStruct Sentries,  int updateFlag)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;
    char *collection = Sentries->entries[2].val;


    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      Sentries->likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    selval[DATA_GRP_NAME] = 1;
    selval[METADATA_NUM_COLL] = 1;
    selval[UDSMD_COLL0] = 1;
    selval[UDSMD_COLL1] = 1;
    selval[UDSMD_COLL2] = 1;
    selval[UDSMD_COLL3] = 1;
    selval[UDSMD_COLL4] = 1;


    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadat Access<A href=\"srberror2.cgi?%i\">Error: %i</A> \n", status,status);
	else
	  printf("%s\n", mySrbStrings[28]);
	return(status);
    }
    else {
      filterDeleted (&collResult);
      Sentries->first = 1;
      prtMetaColl(Sentries, &collResult,updateFlag);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtMetaColl(Sentries, &collResult,updateFlag);
      }
      clearSqlResult (&collResult);
    }
    if (updateFlag == 1) {
      fprintf(stdout,"</TABLE><input type=hidden name=\"session\" value=\"%s\">\
<FONT COLOR=#0000FF>%s<BR>%s<BR>\n</FONT>\
<INPUT TYPE=SUBMIT VALUE=\"%s\"></FORM>\n",Sentries->pidStr, mySrbStrings[208], mySrbStrings[210], mySrbStrings[209]);
    }
    return(0);
}


int
getCollectionMetaNew(inStruct Sentries)
{
    char *collection = Sentries->entries[2].val;
    mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;


    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      Sentries->likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    selval[DATA_GRP_NAME] = 1;
    selval[METADATA_NUM_COLL] = 1;
    selval[UDSMD_COLL0] = 1;
    selval[UDSMD_COLL1] = 1;
    selval[UDSMD_COLL2] = 1;
    selval[UDSMD_COLL3] = 1;
    selval[UDSMD_COLL4] = 1;



    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);
  
    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadat Access<A href=\"srberror2.cgi?%i\">Error: %i</A> \n", status,status);
	else
	  printf("%s\n", mySrbStrings[28]);
	return(status);
    }
    else {
      filterDeleted (&collResult);
      Sentries->first = 1;
      prtMetaCollNew(Sentries, &collResult,collection);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtMetaCollNew(Sentries, &collResult,collection);
      }
      clearSqlResult (&collResult);
    }
    return(0);
}

int
getCollMetaAttr(inStruct Sentries)
{  
  mdasC_sql_result_struct collResult;
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int  selval[MAX_DCS_NUM];
  char contSelect[MAX_TOKEN];
  int i, j;
  int status;
  int collLike;
  char *collection;

  if (strlen(Sentries->entries[2].val) == 0)
    collection = Sentries->entries[1].val;
  else
    collection = Sentries->entries[2].val;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      Sentries->likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    sprintf(qval[UDSMD_COLL0], " = 'COLLMETAATTR'");
    selval[METADATA_NUM_COLL] = 1;
    selval[UDSMD_COLL1] = 1;
    selval[UDSMD_COLL2] = 1;
    selval[UDSMD_COLL3] = 1;
    selval[UDSMD_COLL4] = 1;

  fprintf(stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  enctype=\"multipart/form-data\"  >\n<input type=hidden name=\"function\" value=\"put\">\n");
  fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s </STRONG></FONT>\
<INPUT NAME=\"dataname\", VALUE= \"\" , SIZE=20,12>\
<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT> \
      <INPUT NAME=\"collection\", VALUE= \"%s\" , SIZE=20,12>\
<BR><STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[68], mySrbStrings[69],collection,mySrbStrings[72]);
  getValuesAsSelect(Sentries, RSRC_NAME, Sentries->selectData, Sentries->defaultResource,"#sdsc-fs#sdsc-mda18-fs#");
  fprintf(stdout,"%s\n", Sentries->selectData); fflush(stdout);
  fprintf(stdout,"<STRONG><FONT COLOR=#FF0000>%s</STRONG></FONT>\n", mySrbStrings[71]);
  
  sprintf(contSelect, " like '%%/%s.%s/%%'", Sentries->srbUser, Sentries->userDomain);
  getValuesAsSelectWithCond(Sentries, CONTAINER_NAME, "","",contSelect);
  printf("%s\n",Sentries->selectData);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#0000FF> %s</STRONG></FONT>\n", mySrbStrings[70]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s:</STRONG></FONT>\n", mySrbStrings[52]);fflush(stdout);
  getValuesAsSelect(Sentries, DATA_TYP_NAME, Sentries->selectData,(char *)NULL,"");
  fprintf(stdout,"%s\n", Sentries->selectData);fflush(stdout);

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
      if (status != MCAT_INQUIRE_ERROR) {
	printf("Metadat Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	return(status);
      }
    }
    else {
      filterDeleted (&collResult);
      Sentries->first = 1;
      prtCollMetaAttr(&collResult);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtCollMetaAttr(&collResult);
      }
      clearSqlResult (&collResult);
    }
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #1:</STRONG> </FONT> <INPUT NAME=\"omd1n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd1v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit1\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #2:</STRONG> </FONT> <INPUT NAME=\"omd2n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd2v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit2\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);
  fprintf(stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s #3:</STRONG> </FONT> <INPUT NAME=\"omd3n\", VALUE= \"\" , SIZE=20,12> = <INPUT NAME=\"omd3v\", VALUE= \"\" , SIZE=20,12><INPUT NAME=\"dunit3\", VALUE= \"\" , SIZE=8,8> <FONT COLOR=#0000FF>(%s)</FONT>\n", mySrbStrings[73], mySrbStrings[62]);

  fprintf(stdout,"<HR><BR><STRONG><FONT COLOR=#FF0000>%s</STRONG> </FONT><input type=\"file\" name=\"filename\" size=60 />", mySrbStrings[74]);
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  fprintf(stdout,"<BR><INPUT TYPE=\"submit\" VALUE=\"%s\"></FORM>\n", mySrbStrings[75]);
}

int
insertCollAnnotation(inStruct Sentries)
{
  char *trgColl, *trgObj, *trgSlinkVal;
  int i,ii;
  char newVal[MAX_TOKEN];
  char iName[3 * MAX_TOKEN];
  char iVal[MAX_TOKEN+20];
   printf("<BR>Starting Annotation Ingest ... "); fflush(stdout);

   trgObj = Sentries->entries[1].val;
   trgColl = Sentries->entries[2].val;
   for (i = 4; i < Sentries->m ; i=i+2) {
     if (strlen(Sentries->entries[i].val) == 0 || strlen(Sentries->entries[i+1].val) == 0 )
       continue;
     strcpy(iName,Sentries->entries[i].val);
     iName[MAX_TOKEN-1] = '\0';
     strcpy(iVal,Sentries->entries[i+1].val);
     iVal[MAX_TOKEN-1] = '\0';
     ii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, trgColl, 
			     iVal,iName,"",
			     C_INSERT_ANNOTATIONS);
     if (ii < 0) {
       printf ("Annotation Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName,ii,ii);
       srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     sleep(1);
   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);
   printf("</body></html>");
   return;

}

int insertCollComments(inStruct Sentries)
{
  char *trgColl, *trgObj, *comm;
  int i,ii;
  char newVal[MAX_TOKEN];
  char iName[MAX_TOKEN];
  char iVal[MAX_TOKEN];
   printf("<BR>Starting Comment Ingest ... "); fflush(stdout);

   trgColl = Sentries->entries[2].val;
   comm = Sentries->entries[3].val;
   ii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, trgColl,
			   comm, "", "", D_UPDATE_COLL_COMMENTS);

   if (ii < 0) {
     printf ("Comment Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,ii,ii);
     srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);
   printf("</body></html>");
   return;

}


int
insertCollMetaData(inStruct Sentries)
{
   char *trgColl;
   int i,ii, iii;
   char newVal[MAX_TOKEN];
   char iName[MAX_TOKEN];
   char iVal[MAX_TOKEN];
   char iUnit[MAX_TOKEN];
   char iDef[MAX_TOKEN];
   char iCom[MAX_TOKEN];
   printf("<BR>%s ... ",aaa[25]); fflush(stdout);

   trgColl = Sentries->entries[1].val;

   for (i = 2; i < Sentries->m ; i=i+5) {
     if (strlen(Sentries->entries[i].val) == 0 || strlen(Sentries->entries[i+1].val) == 0 )
       continue;
     strcpy(iName,Sentries->entries[i].val);
     strcpy(iVal,Sentries->entries[i+1].val);
     strcpy(iUnit,Sentries->entries[i+2].val);
     strcpy(iDef,Sentries->entries[i+3].val);
     strcpy(iCom,Sentries->entries[i+4].val);
     if (!strcmp(iName,"Metadata for Data")) 
       strcpy(iName,"COLLMETAATTR");

       
     iii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, trgColl,
			   "0",iName,"",
			   C_INSERT_USER_DEFINED_COLL_STRING_META_DATA);
     if (iii < 0) {
       printf ("Metadata Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName,iii,iii);
       srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     sprintf(newVal,"1@%i",iii);
     ii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, trgColl, 
			   newVal,iVal,"",
			   C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
     if (ii < 0) {
       printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName, iVal,ii,ii);
       srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     if (strlen(iUnit) > 0) {
       sprintf(newVal,"2@%i",iii);
       ii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, trgColl, 
			     newVal,iUnit,"",
			     C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iUnit,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }
     if (strlen(iDef) > 0) {
       sprintf(newVal,"3@%i",iii);
       ii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, trgColl, 
			     newVal,iDef,"",
			     C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iDef,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }
     if (strlen(iCom) > 0) {
       sprintf(newVal,"4@%i",iii);
       ii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, trgColl, 
			     newVal,iCom,"",
			     C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iCom,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }
   }
   printf("Done\n<BR>"); 
   bRMap(2,1);
   printf("</body></html>");
   return;

}


int
isCollInSRB(inStruct Sentries)
{
    char *coll = Sentries->entries[2].val;
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;

   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   sprintf(qval[DATA_GRP_NAME], " =  '%s'",coll);
   selval[DATA_GRP_NAME] = 1;

   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      MAX_RESULT_NUM);
   if (status < 0 ) {
     if (status != MCAT_INQUIRE_ERROR) 
       printf("MCAT <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status, status);
     return 0;
   }
   if (myResult.row_count > 0)
     return 1;
   return 0;
}


int
makeCollectionInSRB(inStruct Sentries)
{
   char *trgColl, *prtColl, *collCont;
   char actColl[MAX_TOKEN];
   int i,ii, iii;
   char newVal[MAX_TOKEN];
   char iName[MAX_TOKEN];
   char iVal[MAX_TOKEN];
   char iUnit[MAX_TOKEN];
   char iDef[MAX_TOKEN];
   char iCom[MAX_TOKEN];
   char qval[MAX_DCS_NUM][MAX_TOKEN];
   int  selval[MAX_DCS_NUM];
   mdasC_sql_result_struct  myresult;

   if (strlen(Sentries->entries[2].val) == 0 ) {
     printf("Content-type: text/html%c%c",10,10);fflush(stdout);
     printf("<html>");
     printf("<title>Collection Info for %s</title> <base target = \"_top\">",Sentries->entries[2].val);
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf ("Error: Collection Name is Empty. Please use back button and resubmit.\n");
     bRMap(2,0);
     printf("</body></html>");
     return;
   }

   prtColl = Sentries->entries[1].val;
   trgColl = Sentries->entries[2].val;
   collCont = Sentries->entries[3].val;
   sprintf(actColl,"%s/%s",prtColl,trgColl);
   printf("Content-type: text/html%c%c",10,10);fflush(stdout);
   printf("<html>");
   printf("<title>Collection Info for %s</title> <base target = \"_top\">",Sentries->entries[2].val);
   printf("<body background=\"%s/srb3.jpg\">", webPath);
   printf("<BR>Collection Creation Beginning ... "); fflush(stdout);

   i = srbCreateCollect(Sentries->conn, MDAS_CATALOG, prtColl,trgColl);
   if (i < 0)  {
     printf("<BR>Error: Unable to create collection '%s': <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", trgColl,i,i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done<BR>\n");
   printf("Beginning to Add Container ...");
   if (strlen(collCont) > 0) {
     if (!strcasecmp(collCont,"inherit from parent")) {
       for (i = 0; i < MAX_DCS_NUM; i++)
	 {
	   sprintf(qval[i],"");
	   selval[i] = 0;
	 }   
       selval[CONTAINER_FOR_COLLECTION] = 1;
       sprintf(qval[COLLECTION_NAME]," = '%s'",prtColl);
       i = srbGetDataDirInfoShunt(Sentries,Sentries->conn, MDAS_CATALOG, qval, selval, &myresult,40);
       if (i < 0) {
	 if (i != MCAT_INQUIRE_ERROR) {
	   printf("<BR>Error: Parent Container Query Error: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", i,i);
	   srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	   bRMap(2,0);
	   printf("</body></html>");
	   return;
	 } 
	 else {
	   printf("<BR>Note: Parent has No Container. No container set for collection\n");
	 }
       }
       else {
	 collCont = (char *) getFromResultStruct
	   ((mdasC_sql_result_struct *) &myresult,
	    dcs_tname[CONTAINER_FOR_COLLECTION], 
	    dcs_aname[CONTAINER_FOR_COLLECTION]);
	 i = srbModifyCollect(Sentries->conn, MDAS_CATALOG,actColl, collCont,"","",
			      C_INSERT_CONTAINER_FOR_COLLECTION);
	 if (i < 0)  {
	   printf("<BR>Error: when associating container %s with %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", 
		  collCont, actColl,i,i);
	   srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	   bRMap(2,0);
	   printf("</body></html>");
	   return;
	 }
       }
     }
     else {
	 i = srbModifyCollect(Sentries->conn, MDAS_CATALOG,actColl, collCont,"","",
			    C_INSERT_CONTAINER_FOR_COLLECTION);
	 if (i < 0)  {
	   printf("<BR>Error: when associating container %s with %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", 
		  collCont,actColl,i,i);
	   srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	   bRMap(2,0);
	   printf("</body></html>");
	   return;
	 }
     }
   }
   printf("Done<BR>\n");
   printf("<BR>%s ... ",aaa[25]); fflush(stdout);
   for (i = 4; i < Sentries->m ; i=i+5) {
     if (strlen(Sentries->entries[i].val) == 0 || strlen(Sentries->entries[i+1].val) == 0 )
       continue;
     strcpy(iName,Sentries->entries[i].val);
     strcpy(iVal,Sentries->entries[i+1].val);
     strcpy(iUnit,Sentries->entries[i+2].val);
     strcpy(iDef,Sentries->entries[i+3].val);
     strcpy(iCom,Sentries->entries[i+4].val);
     if (!strcmp(iName,"Metadata for Data")) 
       strcpy(iName,"COLLMETAATTR");

       
     iii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, actColl,
			   "0",iName,"",
			   C_INSERT_USER_DEFINED_COLL_STRING_META_DATA);
     if (iii < 0) {
       printf ("Metadata Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName,iii,iii);
       srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     sprintf(newVal,"1@%i",iii);
     ii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, actColl, 
			   newVal,iVal,"",
			   C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
     if (ii < 0) {
       printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName, iVal,ii,ii);
       srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     if (strlen(iUnit) > 0) {
       sprintf(newVal,"2@%i",iii);
       ii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, actColl, 
			     newVal,iUnit,"",
			     C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iUnit,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }
     if (strlen(iDef) > 0) {
       sprintf(newVal,"3@%i",iii);
       ii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, actColl, 
			     newVal,iDef,"",
			     C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iDef,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }
     if (strlen(iCom) > 0) {
       sprintf(newVal,"4@%i",iii);
       ii = srbModifyCollect(Sentries->conn, MDAS_CATALOG, actColl, 
			     newVal,iCom,"",
			     C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iCom,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }

   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);
   printf("</body></html>");
   return;

}


int
modifyAnnoColl(inStruct Sentries)
{
  int i,j;

  printf("%s...\n",aaa[33]);

  for (i = 1; i <= Sentries->m ; i++ ) {
    if (strstr(Sentries->entries[i].name,"anncb") != Sentries->entries[i].name)
      continue;
    if (strlen(Sentries->entries[i+4].val) > 0) {
      sprintf(tmpoStr,"%s@@@%s",Sentries->entries[i+4].val,Sentries->entries[i+3].val);
      j = srbModifyCollect(Sentries->conn, MDAS_CATALOG, Sentries->entries[i-1].val,
			     tmpoStr,Sentries->entries[i+5].val,"",
			     C_UPDATE_ANNOTATIONS);
    }
    else {
            j = srbModifyCollect(Sentries->conn, MDAS_CATALOG, Sentries->entries[i-1].val,
			   Sentries->entries[i+5].val,"","",
			     D_DELETE_ANNOTATIONS);
    }
    if (j != 0)  {
    printf("<br>Error: In Updating Annotations for time-stamp: %s<A href=\"srberror3.cgi?%i\">Error: %i</A>\n",Sentries->entries[i+5].val, j,j);
    bRMap(2,0);
    return(0);
    }
    i +=4;
  }
  bRMap(2,0);
  printf("Done");
  
}


int
modifyCollMeta(inStruct Sentries)
{
  int i,j;
  j=0;
  printf("%s...\n",aaa[32]);fflush(stdout);

  for (i = 1; i <= Sentries->m ; i++ ) {
    if (strstr(Sentries->entries[i].name,"metacb") != Sentries->entries[i].name)
      continue;
    if (strlen(Sentries->entries[i+2].val) > 0) {
      sprintf(tmpoStr,"0@%s",Sentries->entries[i+1].val);
      j = srbModifyCollect(Sentries->conn, MDAS_CATALOG, 
			   Sentries->entries[i-1].val,
			   tmpoStr,Sentries->entries[i+2].val,"",
			     C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
      if (j != 0) break;
      sprintf(tmpoStr,"1@%s",Sentries->entries[i+1].val);
      j = srbModifyCollect(Sentries->conn, MDAS_CATALOG, 
			  Sentries->entries[i-1].val,
			   tmpoStr,Sentries->entries[i+3].val,"",
			     C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
      if (j != 0) break;
      sprintf(tmpoStr,"2@%s",Sentries->entries[i+1].val);
      j = srbModifyCollect(Sentries->conn, MDAS_CATALOG, 
			   Sentries->entries[i-1].val,
			   tmpoStr,Sentries->entries[i+4].val,"",
			     C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
      if (j != 0) break;
      sprintf(tmpoStr,"3@%s",Sentries->entries[i+1].val);
      j = srbModifyCollect(Sentries->conn, MDAS_CATALOG, 
			  Sentries->entries[i-1].val,
			   tmpoStr,Sentries->entries[i+5].val,"",
			     C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
      if (j != 0) break;
      sprintf(tmpoStr,"4@%s",Sentries->entries[i+1].val);
      j = srbModifyCollect(Sentries->conn, MDAS_CATALOG, 
			   Sentries->entries[i-1].val,
			   tmpoStr,Sentries->entries[i+6].val,"",
			     C_CHANGE_USER_DEFINED_COLL_STRING_META_DATA);
      if (j != 0) break;
      
    }
    else {
            j = srbModifyCollect(Sentries->conn, MDAS_CATALOG, 
			   Sentries->entries[i-1].val,
			   Sentries->entries[i+1].val,"","",
			     C_DELETE_USER_DEFINED_COLL_STRING_META_DATA);
	    if (j != 0) break;
    }
    i +=4;
  }
    if (j != 0) 
      printf("<br>Error: In Updating Metadat %s=%s; <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",Sentries->entries[i+2].val,Sentries->entries[i+3].val, j,j);
    else 
      printf("Done");
    bRMap(2,0);
    return(0);  
}

int movedsColl()
{
  construction();
}



int
prtAnnoColl (inStruct Sentries, mdasC_sql_result_struct *myresult, int updateFlag)
{
  int i,j;
  char *collList, *dataList;
  char *anno,*anno_time,*anno_user,*anno_domn,*anno_pos;



  anno =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[COLL_ANNOTATION], dcs_aname[COLL_ANNOTATION]);
  anno_user =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[COLL_ANNOTATION_USERNAME], dcs_aname[COLL_ANNOTATION_USERNAME]);
  anno_domn =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[COLL_ANNOTATION_USERDOMAIN], dcs_aname[COLL_ANNOTATION_USERDOMAIN]);
  anno_time =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[COLL_ANNOTATION_TIMESTAMP], dcs_aname[COLL_ANNOTATION_TIMESTAMP]);
  anno_pos =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[COLL_ANNOTATION_TYPE], dcs_aname[COLL_ANNOTATION_TYPE]);
  collList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
  strcpy(tmpCollName,collList);

  clSplitbychar (collList, tmpCollName, tmpDataName, '/');

  /***/
  if (first) {
    first = 0;
    Sentries->globalCounter = 1;
    if ( updateFlag == 1) {
      fprintf (stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" >\n\
<input type=hidden name=\"function\" value=\"modifyanno\">\n", Sentries->frameStr);
      fprintf (stdout,"<TABLE><TR><TH>%s</TH><TH>%s</TH><TH>Type</TH><TH>%s</TH></TR>\n", mySrbStrings[211], mySrbStrings[82], mySrbStrings[66]);
    }

  }
  /***   THIS WILL BE BNEEDED LATER 
  if (first) {
    first = 0;
    globalCounter = 1;
    if ( updateFlag == 1) {
      fprintf (stdout,"<TABLE><TR><TD><FONT COLOR=blue>Remove</font></TD><TD><FONT COLOR=red>Update</font></TD><TD><B>Type</B><BR>Name/Date</TD><TD><B>Annotation</B></TD></TR>\n");
    }
  }
  
  this WILL BE BNEEDED LATER *****/

  if (updateFlag == 0) {  
    fprintf (stdout,"<p><font color=000088><b>%s <i> %s </i></b></font>\n\
<table width=100%% border=0 cellspacing=4 cellpadding=0>\n\
<tr><td width=5%%>&nbsp;</td>\
<td><i><font color=000088><b>%s:</b> &nbsp;&nbsp;\
 %s &lt; %s &gt;</td></tr>\n", mySrbStrings[59], tmpDataName, mySrbStrings[65], mySrbStrings[66], mySrbStrings[67]);
  }
   for (i = 0; i < myresult->row_count; i++) {
    if (updateFlag == 0) {
      fprintf (stdout,"<tr><td>&nbsp;</td><td><b>%s:</b> &nbsp;&nbsp;  %s  &lt; %s@%s , %s &gt;</td></tr>\n",  anno_pos,anno,anno_user,anno_domn,anno_time);
    }
    else { /**  updateFlag == 1  */
      /***/
      fprintf(stdout,"<input type=hidden name=\"data\" value=\"\">\n");
      fprintf(stdout,"<input type=hidden name=\"coll\" value=\"%s\">\n",collList);
      fprintf(stdout,"<TR><TD><INPUT TYPE=CHECKBOX NAME=\"anncb%i\">\
<INPUT TYPE=HIDDEN NAME=\"annuser%i\" VALUE=\"%s\" >\
<INPUT TYPE=HIDDEN NAME=\"anndomn%i\" VALUE=\"%s\" ><TD> %s@%s </TD>\
<TD><INPUT TYPE=TEXT NAME=\"anntyp%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=TEXT NAME=\"annanno%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=TEXT NAME=\"anntime%i\" VALUE=\"%s\" ></TD></TR>\n",
	      Sentries->globalCounter, Sentries->globalCounter,anno_user, Sentries->globalCounter,anno_domn,
	      anno_user,anno_domn,
	      Sentries->globalCounter,anno_pos, Sentries->globalCounter,anno, Sentries->globalCounter,anno_time);
      /**/
      /***
      fprintf (stdout,"<TR><FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" >\n",frameStr);
      fprintf (stdout,"<input type=hidden name=\"function\" value=\"modifyanno\">\n");
      fprintf (stdout,"<TD valign=top align=center><BR><BUTTON NAME=\"action\"  type=\"SUBMIT\" value=\"update\"><img src=\"%s/blue-ball.gif\" alt=\"update\"></BUTTON></TD>\n", webPath);
      fprintf (stdout,"<TD valign=top align=center><BR><BUTTON NAME=\"action\" type=\"SUBMIT\" value=\"delete\"><img src=\"%s/red-ball.gif\" alt=\"delete\"></BUTTON></TD>\n", webPath);
      fprintf (stdout,"<TD valign=top><INPUT TYPE=TEXT NAME=\"anntyp\" VALUE=\"%s\" ><BR>%s@%s<BR>%s</TD>\n",anno_pos,anno_user,anno_domn,anno_time);
      fprintf (stdout,"<TD><TEXTAREA ROWS=5 COLS=50 NAME=\"annanno\">%s</TEXTAREA></TD></FORM><TR>\n",anno);
      ****/
	  /***   THIS WILL BE BNEEDED LATER 

      fprintf (stdout,"<TR><TD valign=top align=center><BR><FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" >\n",frameStr);
      fprintf (stdout,"<input type=hidden name=\"function\" value=\"modifyanno\">\n");
      fprintf(stdout,"<input type=hidden name=\"data\" value=\"%s\">\n",dataList);
      fprintf(stdout,"<input type=hidden name=\"coll\" value=\"%s\">\n",collList);
      fprintf(stdout,"<INPUT TYPE=hidden NAME=\"anncb%i\">\n",globalCounter);
      fprintf(stdout,"<INPUT TYPE=HIDDEN NAME=\"annuser%i\" VALUE=\"%s\" >\
<INPUT TYPE=HIDDEN NAME=\"anndomn%i\" VALUE=\"%s\" >\
<INPUT TYPE=HIDDEN NAME=\"annpos%i\" VALUE=\"%s\" >\
<INPUT TYPE=HIDDEN NAME=\"annanno%i\" VALUE=\"\" >\
<INPUT TYPE=HIDDEN NAME=\"anntime%i\" VALUE=\"%s\" >\
<INPUT TYPE=
</FORM></TD>\n",
	      anno_user,globalCounter,anno_domn,
	      globalCounter,anno_pos,globalCounter,globalCounter,anno_time);
      fprintf (stdout,"<TD><FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" >\n",frameStr);
      fprintf (stdout,"<input type=hidden name=\"function\" value=\"modifyanno\">\n");
      fprintf(stdout,"<input type=hidden name=\"data\" value=\"%s\">\n",dataList);
      fprintf(stdout,"<input type=hidden name=\"coll\" value=\"%s\">\n",collList);
      fprintf(stdout,"<INPUT TYPE=hidden NAME=\"anncb%i\">\n",globalCounter);
      fprintf(stdout,"<INPUT TYPE=HIDDEN NAME=\"annuser%i\" VALUE=\"%s\" >\
<INPUT TYPE=HIDDEN NAME=\"anndomn%i\" VALUE=\"%s\" >\
<TD><INPUT TYPE=HIDDEN NAME=\"annpos%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=HIDDEN NAME=\"annanno%i\" VALUE=\"\" ></TD>\
<TD><INPUT TYPE=HIDDEN NAME=\"anntime%i\" VALUE=\"%s\" ></FORM></TD>\n",
	      anno_user,globalCounter,anno_domn,
	      globalCounter,anno_pos,globalCounter,globalCounter,anno_time);
	 this WILL BE BNEEDED LATER   ****/


    }
    Sentries->globalCounter++;
    anno += MAX_DATA_SIZE;
    anno_user += MAX_DATA_SIZE;
    anno_domn += MAX_DATA_SIZE;
    anno_time += MAX_DATA_SIZE;
    anno_pos += MAX_DATA_SIZE;
    dataList += MAX_DATA_SIZE;
    collList += MAX_DATA_SIZE;

  }
   fprintf(stdout,"</table>\n");
}


int
prtCollMetaAttr (mdasC_sql_result_struct *myresult)
{
  int i,j;
  char *meta1,*meta2,*meta3,*meta4, *tmpPtr, *tmpPtr1;
  meta1 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL1]);
  meta2 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL2]);
  meta3 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL3]);
  meta4 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL4]);
  for (i = 0; i < myresult->row_count; i++) {
    if (strlen(meta3) > 0) {
      if ((tmpPtr = strstr(meta3,",")) != NULL) {
	*tmpPtr = '\0';
	fprintf (stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</FONT>:</STRONG><INPUT TYPE=hidden NAME=\"ATRNAME\", VALUE= \"%s\"><SELECT SIZE=1 NAME=%s><OPTION SELECTED> %s", meta1,meta1,meta1, meta3 );
	tmpPtr1 =  tmpPtr + 1;
	while(( tmpPtr = strstr(tmpPtr1,",")) != NULL) {
	  *tmpPtr = '\0';
	  fprintf (stdout,"<OPTION>%s",tmpPtr1);
	  tmpPtr1 =  tmpPtr + 1;
	}
	fprintf (stdout,"<OPTION>%s</SELECT> <FONT COLOR=#0000FF>%s</FONT>\n",tmpPtr1, meta2);
	if (strlen(meta4) > 0) fprintf (stdout,"  /* %s */\n",meta4);
	fprintf(stdout,"<input type=hidden name=\"dunits\" value=\"%s\">",meta2);
      }
      else {
      fprintf (stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</FONT>:</STRONG> <INPUT TYPE=hidden NAME=\"ATRNAME\", VALUE= \"%s\"><INPUT NAME=\"%s\", VALUE= \"%s\" , SIZE=20,12> <FONT COLOR=#0000FF>%s</FONT>\n",meta1,meta1,meta1, meta3, meta2);
      if (strlen(meta4) > 0) fprintf (stdout,"  /* %s */\n",meta4);
      fprintf(stdout,"<input type=hidden name=\"dunits\" value=\"%s\">",meta2);
      }
    }
    else {
      fprintf (stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</FONT>:</STRONG> <INPUT TYPE=hidden NAME=\"ATRNAME\", VALUE= \"%s\"><INPUT NAME=\"%s\", VALUE= \"\" , SIZE=20,12> <FONT COLOR=#0000FF>%s</FONT>\n",meta1,meta1,meta1, meta2);
      if (strlen(meta4) > 0) fprintf (stdout,"  /* %s */\n",meta4);
      fprintf(stdout,"<input type=hidden name=\"dunits\" value=\"%s\">",meta2);

    }
    meta1 += MAX_DATA_SIZE;
    meta2 += MAX_DATA_SIZE;
    meta3 += MAX_DATA_SIZE;
    meta4 += MAX_DATA_SIZE;
  }

}

int
prtCollMetaAttrOLD (mdasC_sql_result_struct *myresult)
{
  int i,j;
  char *meta1,*meta2,*meta3,*meta4, *tmpPtr, *tmpPtr1;
  meta1 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL1]);
  meta2 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL2]);
  meta3 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL3]);
  meta4 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL4]);
  for (i = 0; i < myresult->row_count; i++) {
    if (strlen(meta3) > 0) {
      if ((tmpPtr = strstr(meta3,",")) != NULL) {
	*tmpPtr = '\0';
	fprintf (stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</FONT>:</STRONG><SELECT SIZE=1 NAME=%s><OPTION SELECTED> %s", meta1,meta1, meta3 );
	tmpPtr1 =  tmpPtr + 1;
	while(( tmpPtr = strstr(tmpPtr1,",")) != NULL) {
	  *tmpPtr = '\0';
	  fprintf (stdout,"<OPTION>%s",tmpPtr1);
	  tmpPtr1 =  tmpPtr + 1;
	}
	fprintf (stdout,"<OPTION>%s</SELECT> <FONT COLOR=#0000FF>%s</FONT>\n",tmpPtr1, meta2);
	if (strlen(meta4) > 0) fprintf (stdout,"  /* %s */\n",meta4);
	fprintf(stdout,"<input type=hidden name=\"dunits\" value=\"%s\">",meta2);
      }
      else {
      fprintf (stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</FONT>:</STRONG> <INPUT NAME=\"%s\", VALUE= \"%s\" , SIZE=20,12> <FONT COLOR=#0000FF>%s</FONT>\n",meta1,meta1, meta3, meta2);
      if (strlen(meta4) > 0) fprintf (stdout,"  /* %s */\n",meta4);
      fprintf(stdout,"<input type=hidden name=\"dunits\" value=\"%s\">",meta2);
      }
    }
    else {
      fprintf (stdout,"<BR><STRONG><FONT COLOR=#FF0000>%s</FONT>:</STRONG> <INPUT NAME=\"%s\", VALUE= \"\" , SIZE=20,12> <FONT COLOR=#0000FF>%s</FONT>\n",meta1,meta1, meta2);
      if (strlen(meta4) > 0) fprintf (stdout,"  /* %s */\n",meta4);
      fprintf(stdout,"<input type=hidden name=\"dunits\" value=\"%s\">",meta2);

    }
    meta1 += MAX_DATA_SIZE;
    meta2 += MAX_DATA_SIZE;
    meta3 += MAX_DATA_SIZE;
    meta4 += MAX_DATA_SIZE;
  }

}

int
prtMetaCollNew(inStruct Sentries, mdasC_sql_result_struct *myresult, char* inCollName)
{
  int i, j;
  char *meta0, *meta1, *meta2, *meta3, *meta4, *metanum,*collList;
  char *tmeta0, *tmeta1, *tmeta2, *tmeta3, *tmeta4, *tmetanum,*tcollList;

  meta0 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[UDSMD_COLL0], dcs_aname[UDSMD_COLL0]);
  meta1 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL1]);
  meta2 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL2], dcs_aname[UDSMD_COLL2]);
  meta3 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL3], dcs_aname[UDSMD_COLL3]);
  meta4 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL4], dcs_aname[UDSMD_COLL4]);
  metanum =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[METADATA_NUM_COLL], dcs_aname[METADATA_NUM_COLL]);
  collList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    tmeta0 = meta0;
    tmeta1 = meta1;
    tmeta2 = meta2;
    tmeta3 = meta3;
    tmeta4 = meta4;
    tmetanum = metanum;
    tcollList = collList;

  /****************** MAIN CONTENTS OF SCREEN *********************/
    if (Sentries->showMetaType == 0 || Sentries->showMetaType == 2) {
      fprintf (stdout,"<font color=000088><b>%s \
<i> %s </i></b></font>\n\
<table width=100%% border=0 cellspacing=4 cellpadding=0>\n\
<tr><td width=5%%>&nbsp;</td>\
<td><i><font color=000088>%s:&nbsp;&nbsp;\
<b> %s</b> <font color=red>%s</font> &lt; %s &gt;</td></tr>\n", mySrbStrings[57], Sentries->tmpDataName, mySrbStrings[60], mySrbStrings[61], mySrbStrings[62], mySrbStrings[63]);fflush(stdout);
      for (i = 0; i < myresult->row_count; i++) {
	if (strcmp(meta0,"COLLMETAATTR")) {
	  fprintf (stdout,"<tr><td>&nbsp;</td><td>%s :&nbsp;&nbsp;<b>%s</b> <font color=red>%s</font>\n",meta0,meta1,meta2);
	  if (strlen(meta4) > 0)
	    fprintf (stdout," &lt; %s &gt;</td></tr>\n",meta4);
	}
	meta0 += MAX_DATA_SIZE;
	meta1 += MAX_DATA_SIZE;
	meta2 += MAX_DATA_SIZE;
	meta3 += MAX_DATA_SIZE;
	meta4 += MAX_DATA_SIZE;
	metanum += MAX_DATA_SIZE;
	collList += MAX_DATA_SIZE;
      }
      fprintf (stdout,"</table>\n");fflush(stdout);
    }
    meta0 = tmeta0;
    meta1 = tmeta1;
    meta2 = tmeta2;
    meta3 = tmeta3;
    meta4 = tmeta4;
    metanum = tmetanum;
    collList = tcollList;
    if (Sentries->showMetaType == 0 || Sentries->showMetaType == 1) {
      fprintf (stdout,"<p><font color=000088><b>%s <i> %s </i></b></font>\n\
<table width=100%% border=0 cellspacing=4 cellpadding=0>\n\
<tr><td width=5%%>&nbsp;</td>\
<td><i><font color=000088>%s: &nbsp;&nbsp;\
%s (%s) &lt; %s &gt;</td></tr>\n", mySrbStrings[58], Sentries->tmpDataName, mySrbStrings[60], mySrbStrings[62], mySrbStrings[64], mySrbStrings[63]);
      for (i = 0; i < myresult->row_count; i++) {
	if (!strcmp(meta0,"COLLMETAATTR")) {
	  fprintf (stdout,"<tr><td>&nbsp;</td><td>%s :&nbsp;&nbsp;  %s\n",meta1,meta2);
	  if (strlen(meta3) > 0)
	    fprintf (stdout,"   (%s)\n", meta3);
	  else
	    fprintf (stdout,"   (no default values)\n");
	  if (strlen(meta4) > 0)
	    fprintf (stdout," &lt; %s  &gt;</td></tr>\n",meta4);
	}
	meta0 += MAX_DATA_SIZE;
	meta1 += MAX_DATA_SIZE;
	meta2 += MAX_DATA_SIZE;
	meta3 += MAX_DATA_SIZE;
	meta4 += MAX_DATA_SIZE;
	metanum += MAX_DATA_SIZE;
	collList += MAX_DATA_SIZE;
      }
      fprintf (stdout,"</table>\n");
    }

}

int
prtMetaColl (inStruct Sentries, mdasC_sql_result_struct *myresult, int updateFlag)
{
  int i,j;
  char *meta0, *meta1, *meta2, *meta3, *meta4, *metanum,*collList;

  if (first) {
    first = 0;
    Sentries->globalCounter = 1;
    strcpy(tmpCollName,"");
    if ( updateFlag == 1) {
      fprintf (stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" >\n\
<input type=hidden name=\"function\" value=\"modifymeta\">\n", Sentries->frameStr);
      fprintf (stdout,"<TABLE><TR><TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH></TR>\n", mySrbStrings[211], mySrbStrings[77], mySrbStrings[78], mySrbStrings[62], mySrbStrings[64], mySrbStrings[63]);
      fflush(stdout);
    }
  }
  meta0 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[UDSMD_COLL0], dcs_aname[UDSMD_COLL0]);
  meta1 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL1], dcs_aname[UDSMD_COLL1]);
  meta2 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL2], dcs_aname[UDSMD_COLL2]);
  meta3 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL3], dcs_aname[UDSMD_COLL3]);
  meta4 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD_COLL4], dcs_aname[UDSMD_COLL4]);
  metanum =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[METADATA_NUM_COLL], dcs_aname[METADATA_NUM_COLL]);
  collList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
  for (i = 0; i < myresult->row_count; i++) {
    if (strcmp(collList,tmpCollName)) {
      if (!showAllMetaFlag) {
	fprintf (stdout,"<BR><FONT COLOR=#0000FF>%s</FONT>\n",collList);
      }
      strcpy(tmpCollName,collList);
    }
    if (updateFlag == 0) {
      if (strcmp(meta0,"COLLMETAATTR"))
	fprintf (stdout,"<BR><FONT COLOR=#FF0000>%s</FONT>: %s <FONT COLOR=#0000FF>%s</FONT>\n",meta0,meta1,meta2);
      else
	fprintf (stdout,"<BR><FONT COLOR=#FF0000>Collection Meta Attribute</FONT>: %s <FONT COLOR=#0000FF>%s</FONT>\n",meta1,meta2);
      if (strlen(meta3) > 0)
	fprintf(stdout," <B>DefValue:</B><FONT COLOR=#FF0000>%s</FONT>\n",meta3);
      if (strlen(meta4) > 0)
	fprintf(stdout," <B>%s</B><FONT COLOR=#FF0000>%s</FONT>\n", mySrbStrings[195],meta4);
    }
    else {
      fprintf(stdout,"<input type=hidden name=\"data\" value=\"\">\n");fflush(stdout);
      fprintf(stdout,"<input type=hidden name=\"coll\" value=\"%s\">\n",collList);fflush(stdout);
      fprintf(stdout,"<TR><TD><INPUT TYPE=CHECKBOX NAME=\"metacb%i\">\
<input type=hidden name=\"metanum%i\" VALUE=\"%s\" >\
<TD><INPUT TYPE=TEXT NAME=\"metaname%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=TEXT NAME=\"metaval%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=TEXT NAME=\"metaunit%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=TEXT NAME=\"metadef%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=TEXT NAME=\"metacomm%i\" VALUE=\"%s\" ></TD></TR>\n",
	      Sentries->globalCounter,
	      Sentries->globalCounter, metanum, Sentries->globalCounter,meta0,Sentries->globalCounter,meta1,
	      Sentries->globalCounter, meta2,  Sentries->globalCounter,meta3,Sentries->globalCounter,meta4);fflush(stdout);
    }
    Sentries->globalCounter++;
    meta0 += MAX_DATA_SIZE;
    meta1 += MAX_DATA_SIZE;
    meta2 += MAX_DATA_SIZE;
    meta3 += MAX_DATA_SIZE;
    meta4 += MAX_DATA_SIZE;
    metanum += MAX_DATA_SIZE;
    collList += MAX_DATA_SIZE;
  }
}


int
prtCollWeb (inStruct Sentries, mdasC_sql_result_struct *myresult)
{
    int i,iii;
    char *tmpList;
    char *ownerList, *domainList;
    char *collComments;
    char *timeList;
    char *collLinkNumList;
    char collName[2 * MAX_TOKEN];
    char parcollName[MAX_TOKEN];
    


    tmpList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    ownerList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_OWNER_NAME], dcs_aname[COLL_OWNER_NAME]);
    domainList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_OWNER_DOMAIN], dcs_aname[COLL_OWNER_DOMAIN]);
    collComments= (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_COMMENTS], dcs_aname[COLL_COMMENTS]);
    timeList= (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_CREATE_TIMESTAMP], dcs_aname[COLL_CREATE_TIMESTAMP]);
/*
    collLinkNumList	= (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLLECTION_LINK_NUM], dcs_aname[COLLECTION_LINK_NUM]);
*/
    Sentries->oddFlag = 0;
    

    if (Sentries->first) {
      fprintf (stdout,"\n<TR BGCOLOR=\"#DDEFFF\">\n");
      fprintf (stdout, "<TH></TH>"); /*** for collection count column ***/
      fprintf (stdout, "<TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH></TR>\n", mySrbStrings[45], mySrbStrings[55], mySrbStrings[47], mySrbStrings[48]);
	    Sentries->first = 0;
    }
    for (i = 0; i < myresult->row_count; i++) {
      clSplitbychar (tmpList, parcollName, collName, '/');
/*
      if (atoi(collLinkNumList) == 0) {
      	strcpy(urlcollList,tmpList);
	strcpy(nonurlcollList,tmpList);
      }
      else {
	if(getRealCollName(urlcollList,tmpList) == 0) {
	  strcpy(nonurlcollList,urlcollList);
	  sprintf(collName,"%s->%s",collName,urlcollList);
	}
	else {
      	strcpy(urlcollList,tmpList);
	strcpy(nonurlcollList,tmpList);
	}
      }
*/
/*big note coll link next two lines to be removed if you're doing colllink*/
	strcpy(Sentries->urlcollList,tmpList);
	strcpy(Sentries->nonurlcollList,tmpList);
      escape_url(Sentries->urlcollList);
      Sentries->oddFlag = (Sentries->oddFlag + 1) % 2;
      if (Sentries->oddFlag) {
	fprintf (stdout,"<TR bgcolor=\"#88BBFF\">");
      }
      else {
	fprintf (stdout,"<TR bgcolor=\"#DDEEFF\">");
      }
      
    /*** object count  ***/
    fprintf(stdout, "<TD>%d</TD>", Sentries->rowCntr);
    /****/
      
      if (Sentries->isTicket) {
	  fprintf (stdout,"<FORM name =\"collselform%i\" METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" ><TD NOWRAP VALIGN=top><FONT face=\"verdana\" size=\"2\"> \n", Sentries->rowCntr, Sentries->frameStr);
	fprintf (stdout,"<SELECT SIZE=1 NAME=function onChange=\"collselform%i.submit()\">\
<OPTION SELECTED VALUE=\"ticketfull\">%s\
<OPTION VALUE=\"ticketmeta\">%s\
</SELECT>\n", Sentries->rowCntr, mySrbStrings[143], mySrbStrings[85]);
	}
	else { 

	    fprintf(stdout,"<FORM name =\"collselform%i\" METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" ><TD NOWRAP VALIGN=top><FONT face=\"verdana\" size=\"2\"> \n", Sentries->rowCntr, Sentries->frameStr);
    	fprintf(stdout,"<SELECT SIZE=1 NAME=function onChange=\"mySubmit(collselform%i, this)\">", Sentries->rowCntr);
	    fprintf(stdout,"<OPTION SELECTED VALUE=\"doNothing\">%s", mySrbStrings[269]);
    	fprintf(stdout,"<OPTION VALUE=\"full\">%s", mySrbStrings[143]);
	    fprintf(stdout,"<OPTION VALUE=\"showmetaNew\">%s", mySrbStrings[85]);
	    fprintf(stdout,"<OPTION VALUE=\"showmetaE\">%s", mySrbStrings[86]);
	    fprintf(stdout,"<OPTION VALUE=\"showmetaC\">%s", mySrbStrings[144]);
	    fprintf(stdout,"<OPTION VALUE=\"showanno\">%s", mySrbStrings[87]);
	    fprintf(stdout,"<OPTION VALUE=\"comminsert\">%s", mySrbStrings[94]);
	    fprintf(stdout,"<OPTION VALUE=\"metainsert\">%s", mySrbStrings[91]);
	    fprintf(stdout,"<OPTION VALUE=\"ingestextschmnamelisting\">%s",aaa[27]);
	    fprintf(stdout,"<OPTION VALUE=\"dcinsert\">%s", mySrbStrings[92]);
	    fprintf(stdout,"<OPTION VALUE=\"modifymetaform\">%s", mySrbStrings[95]);
	    fprintf(stdout,"<OPTION VALUE=\"copymetaform\">%s", mySrbStrings[97]);
	    fprintf(stdout,"<OPTION VALUE=\"deletemetaform\">%s", mySrbStrings[99]);
	    fprintf(stdout,"<OPTION VALUE=\"annoinsert\">%s", mySrbStrings[93]);
	    fprintf(stdout,"<OPTION VALUE=\"deleteannoform\">%s", mySrbStrings[100]);
	    fprintf(stdout,"<OPTION VALUE=\"modifyannoform\">%s", mySrbStrings[96]);
	    fprintf(stdout,"<OPTION VALUE=\"copyannoform\">%s", mySrbStrings[98]);
	    fprintf(stdout,"<OPTION VALUE=\"copydsform\">%s", mySrbStrings[138]);
	    fprintf(stdout,"<OPTION VALUE=\"movedsform\">%s", mySrbStrings[139]);
	    fprintf(stdout,"<OPTION VALUE=\"slinkCollform\">%s", mySrbStrings[140]);
	    fprintf(stdout,"<OPTION VALUE=\"renamedsform\">%s", mySrbStrings[141]);
	    fprintf(stdout,"<OPTION VALUE=\"aclform\">%s", mySrbStrings[110]);
	    fprintf(stdout,"<OPTION VALUE=\"giveticketform\">%s", mySrbStrings[111]);
	    fprintf(stdout,"<OPTION VALUE=\"removeDataCollForm\">%s", mySrbStrings[142]);
	    fprintf(stdout,"</SELECT>\n");

	}
	fprintf(stdout,"<INPUT NAME=\"dataname\" type=hidden VALUE=\"\">\n");
       fprintf(stdout,"<INPUT NAME=\"collection\" type=hidden VALUE=\"%s\">\n",
		Sentries->nonurlcollList);
	fprintf(stdout,"<INPUT NAME=\"seek\" type=hidden VALUE=\"0\">\n");
	fprintf(stdout,"<INPUT NAME=\"copynum\" type=hidden VALUE=\"0\">\n");
	fprintf(stdout,"<INPUT NAME=\"datatype\" type=hidden VALUE=\"\">\n");
	fprintf(stdout,"<INPUT NAME=\"vernum\" type=hidden VALUE=\"0\">\n");
	fprintf(stdout,"<INPUT NAME=\"owner\" type=hidden VALUE=\"%s@%s\">\n",ownerList,domainList);

	fprintf(stdout,"<INPUT NAME=\"session\" type=hidden VALUE=\"%s\">\n",
		Sentries->pidStr);
	fprintf(stdout,"</FONT></TD></FORM>\n");
	if (Sentries->isTicket) {
	  fprintf(stdout,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=ticketfull&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/collimg.gif\" ALIGN=middle HEIGHT=16 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=ticketfull&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\">%s </A>","",Sentries->urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr, webPath,
		 "", Sentries->urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr,collName);
	}
	else {
	  fprintf(stdout,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/collimg.gif\" ALIGN=middle HEIGHT=16 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\">%s </A>","",Sentries->urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr, webPath,
		 "", Sentries->urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr, collName);
	}

#ifdef DATACOMMENTS
	if (strlen(collComments) > 0) 
	  fprintf (stdout,"&nbsp;&nbsp;&nbsp;<IMG SRC=\"%s/commentsbut.gif\" ALT=\"%s\" ALIGN=middle width=18 height=18 border=0></A>\n", webPath,collComments);
#endif


	fprintf(stdout,"</TD><TD  nowrap align=center> %s </TD><TD nowrap align=center> %s@%s </TD></TR>\n",timeList,ownerList,domainList);
	if (Sentries->showAllMetaFlag == 1) {
	  fprintf (stdout,"<TR>");
	  fprintf (stdout,"<TD valign=top align=right><B>Metadata:</B></TD><TD COLSPAN=8>\n");
	  strcpy(Sentries->tmpCollName, Sentries->entries[2].val);
	  strcpy(Sentries->tmpDataName, Sentries->entries[1].val);
	  strcpy(Sentries->entries[2].val, Sentries->nonurlcollList);
	  strcpy(Sentries->entries[1].val,"");
	  showMeta(Sentries);
	  strcpy(Sentries->entries[2].val, Sentries->tmpCollName);
	  strcpy(Sentries->entries[1].val, Sentries->tmpDataName);
	  fprintf (stdout,"<HR NOSHADE></TD></TR>\n");
	}
        tmpList += MAX_DATA_SIZE;
        ownerList += MAX_DATA_SIZE;
        domainList += MAX_DATA_SIZE;
        timeList += MAX_DATA_SIZE;
	collComments += MAX_DATA_SIZE;
/*	collLinkNumList	+= MAX_DATA_SIZE; */
	Sentries->rowCntr++;
    }
}



int
prtCollWeb_simple(inStruct Sentries, mdasC_sql_result_struct *myresult)
{
    int i,iii;
    char *tmpList;
    char *ownerList, *domainList;
    char *collComments;
    char *timeList;
    char *collLinkNumList;
    char collName[2 * MAX_TOKEN];
    char parcollName[MAX_TOKEN];
    

    tmpList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    ownerList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_OWNER_NAME], dcs_aname[COLL_OWNER_NAME]);
    domainList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_OWNER_DOMAIN], dcs_aname[COLL_OWNER_DOMAIN]);
    collComments= (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_COMMENTS], dcs_aname[COLL_COMMENTS]);
    timeList= (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_CREATE_TIMESTAMP], dcs_aname[COLL_CREATE_TIMESTAMP]);
/*
    collLinkNumList	= (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLLECTION_LINK_NUM], dcs_aname[COLLECTION_LINK_NUM]);
*/
    Sentries->oddFlag = 0;
    

    if (Sentries->first) {
      fprintf (stdout,"\n<TR BGCOLOR=\"#FFFFFF\"><TH>%s</TH><TH>%s</TH><TH>%s</TH><TH></TH><TH></TH></TR>\n", mySrbStrings[55], mySrbStrings[47], mySrbStrings[48]);
	    Sentries->first = 0;
    }
    for (i = 0; i < myresult->row_count; i++) {
      clSplitbychar (tmpList, parcollName, collName, '/');
/*
      if (atoi(collLinkNumList) == 0) {
      	strcpy(urlcollList,tmpList);
	strcpy(nonurlcollList,tmpList);
      }
      else {
	if(getRealCollName(urlcollList,tmpList) == 0) {
	  strcpy(nonurlcollList,urlcollList);
	  sprintf(collName,"%s->%s",collName,urlcollList);
	}
	else {
      	strcpy(urlcollList,tmpList);
	strcpy(nonurlcollList,tmpList);
	}
      }
*/
/*big note coll link next two lines to be removed if you're doing colllink*/
	strcpy(Sentries->urlcollList,tmpList);
	strcpy(Sentries->nonurlcollList,tmpList);
      escape_url(Sentries->urlcollList);
      Sentries->oddFlag = (Sentries->oddFlag + 1) % 2;
      if (Sentries->oddFlag) {
	fprintf (stdout,"<TR bgcolor=\"#DDEEFF\">");
      }
      else {
	fprintf (stdout,"<TR bgcolor=\"#FFFFFF\">");
      }
      if (Sentries->isTicket) {
	  fprintf (stdout,"<FORM name =\"collselform%i\" METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" ><TD NOWRAP VALIGN=top><FONT face=\"verdana\" size=\"2\"> \n", Sentries->rowCntr, Sentries->frameStr);
	fprintf (stdout,"<SELECT SIZE=1 NAME=function onChange=\"collselform%i.submit()\">\
<OPTION SELECTED VALUE=\"ticketfull\">%s\
<OPTION VALUE=\"ticketmeta\">%s\
</SELECT>\n", Sentries->rowCntr, mySrbStrings[143], mySrbStrings[85]);
	}




	if (Sentries->isTicket) {
	  fprintf(stdout,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=ticketfull&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/collimg.gif\" HEIGHT=16 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=ticketfull&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\">%s </A>","",Sentries->urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr, webPath,
		 "", Sentries->urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr,collName);
	}
	else {
	  fprintf(stdout,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=showmetaNew&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/collimg.gif\" HEIGHT=16 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\">%s </A>","",Sentries->urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr, webPath,
		 "", Sentries->urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr, collName);
	}

#ifdef DATACOMMENTS
	if (strlen(collComments) > 0) 
	  fprintf (stdout,"&nbsp;&nbsp;&nbsp;<IMG SRC=\"%s/commentsbut.gif\" ALT=\"%s\" width=18 height=18 border=0></A>\n", webPath,collComments);
#endif


	fprintf(stdout,"</TD><TD  nowrap align=center> %s </TD><TD nowrap align=center> %s@%s </TD><TD></TD><TD></TD></TR>\n",timeList,ownerList,domainList);
	if (Sentries->showAllMetaFlag == 1) {
	  fprintf (stdout,"<TR>");
	  fprintf (stdout,"<TD valign=top align=right><B>Metadata:</B></TD><TD COLSPAN=8>\n");
	  strcpy(Sentries->tmpCollName, Sentries->entries[2].val);
	  strcpy(Sentries->tmpDataName, Sentries->entries[1].val);
	  strcpy(Sentries->entries[2].val, Sentries->nonurlcollList);
	  strcpy(Sentries->entries[1].val,"");
	  showMeta(Sentries);
	  strcpy(Sentries->entries[2].val, Sentries->tmpCollName);
	  strcpy(Sentries->entries[1].val, Sentries->tmpDataName);
	  fprintf (stdout,"<HR NOSHADE></TD></TR>\n");
	}
        tmpList += MAX_DATA_SIZE;
        ownerList += MAX_DATA_SIZE;
        domainList += MAX_DATA_SIZE;
        timeList += MAX_DATA_SIZE;
	collComments += MAX_DATA_SIZE;
/*	collLinkNumList	+= MAX_DATA_SIZE; */
	Sentries->rowCntr++;
    }
}



int
prtCollWeb_query (inStruct Sentries, mdasC_sql_result_struct *myresult)
{
    int i,iii, jj;
    char *tmpList;
    char *ownerList, *domainList;
    char *collComments;
    char *timeList;
    char *collLinkNumList;
    char *udsmdList[20];
    char collName[2 * MAX_TOKEN];
    char parcollName[MAX_TOKEN];
    

    tmpList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    ownerList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_OWNER_NAME], dcs_aname[COLL_OWNER_NAME]);
    domainList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_OWNER_DOMAIN], dcs_aname[COLL_OWNER_DOMAIN]);
    collComments= (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_COMMENTS], dcs_aname[COLL_COMMENTS]);
    timeList= (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_CREATE_TIMESTAMP], dcs_aname[COLL_CREATE_TIMESTAMP]);
/*
    collLinkNumList	= (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLLECTION_LINK_NUM], dcs_aname[COLLECTION_LINK_NUM]);
*/

    /** get values of metadata attribute for possible display **/
    for (jj =  0; jj < Sentries->showMDVCntr; jj++)  {
      udsmdList[jj] = (char *) getFromResultStruct(
	  (mdasC_sql_result_struct *) myresult,
	   dcs_tname[Sentries->showMDVId[jj]], dcs_aname[Sentries->showMDVId[jj]]);
      if (udsmdList[jj] == NULL)
	printf("jj=%i is NULL!! showMDVId[jj]= %i !!\n",jj, Sentries->showMDVId[jj]);
    }

    Sentries->oddFlag = 0;
    

    if (Sentries->first) {
      fprintf (stdout,"\n<TR BGCOLOR=\"#DDEFFF\">\n");
      
      /*** header for metadata names whose values will be shown ***/      
      for (jj = 0; jj < Sentries->showMDVCntr; jj++) {
	    fprintf (stdout,"<TH>%s</TH>\n",Sentries->showMDVTitle[jj]);
	  } 
        
      fprintf (stdout,"<TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH></TR>\n", mySrbStrings[45], mySrbStrings[69], mySrbStrings[47], mySrbStrings[48]);
	    Sentries->first = 0;
    }
    for (i = 0; i < myresult->row_count; i++) {
      clSplitbychar (tmpList, parcollName, collName, '/');
/*
      if (atoi(collLinkNumList) == 0) {
      	strcpy(urlcollList,tmpList);
	strcpy(nonurlcollList,tmpList);
      }
      else {
	if(getRealCollName(urlcollList,tmpList) == 0) {
	  strcpy(nonurlcollList,urlcollList);
	  sprintf(collName,"%s->%s",collName,urlcollList);
	}
	else {
      	strcpy(urlcollList,tmpList);
	strcpy(nonurlcollList,tmpList);
	}
      }
*/
/*big note coll link next two lines to be removed if you're doing colllink*/
	strcpy(Sentries->urlcollList,tmpList);
	strcpy(Sentries->nonurlcollList,tmpList);
      escape_url(Sentries->urlcollList);
      Sentries->oddFlag = (Sentries->oddFlag + 1) % 2;
      if (Sentries->oddFlag) {
	fprintf (stdout,"<TR bgcolor=\"#88BBFF\">");
      }
      else {
	fprintf (stdout,"<TR bgcolor=\"#DDEEFF\">");
      }
      
     /*** to print value of metadata attribute if 'show' was checked ***/ 
	for (jj = 0; jj < Sentries->showMDVCntr; jj++) {
	  if (Sentries->showMDVId[jj] == DATA_ANNOTATION_USERNAME) {
	    fprintf (stdout,"<TD>%s@%s</TD>",udsmdList[jj],udsmdList[jj+1]);
	    udsmdList[jj] += MAX_DATA_SIZE; 	
	    udsmdList[jj+1] += MAX_DATA_SIZE; 	
	    jj++;
	  }
	  else {
	    fprintf (stdout,"<TD>%s</TD>",udsmdList[jj]);
	    udsmdList[jj] += MAX_DATA_SIZE; 	
	  }
	}
	/************************************************/
      
      if (Sentries->isTicket) {
	  fprintf (stdout,"<FORM name =\"collselform%i\" METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" ><TD NOWRAP VALIGN=top><FONT face=\"verdana\" size=\"2\"> \n", Sentries->rowCntr, Sentries->frameStr);
	fprintf (stdout,"<SELECT SIZE=1 NAME=function onChange=\"collselform%i.submit()\">\
<OPTION SELECTED VALUE=\"ticketfull\">%s\
<OPTION VALUE=\"ticketmeta\">%s\
</SELECT>\n", Sentries->rowCntr, mySrbStrings[143], mySrbStrings[85]);
	}
	else { 

	    fprintf(stdout,"<FORM name =\"collselform%i\" METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" ><TD NOWRAP VALIGN=top><FONT face=\"verdana\" size=\"2\"> \n", Sentries->rowCntr, Sentries->frameStr);
    	fprintf(stdout,"<SELECT SIZE=1 NAME=function onChange=\"mySubmit(collselform%i, this)\">", Sentries->rowCntr);
	    fprintf(stdout,"<OPTION SELECTED VALUE=\"doNothing\">%s", "&nbsp;&nbsp;-- Choose Action --");
    	fprintf(stdout,"<OPTION VALUE=\"full\">%s", mySrbStrings[143]);
	    fprintf(stdout,"<OPTION VALUE=\"showmetaNew\">%s", mySrbStrings[85]);
	    fprintf(stdout,"<OPTION VALUE=\"showmetaE\">%s", mySrbStrings[86]);
	    fprintf(stdout,"<OPTION VALUE=\"showmetaC\">%s", mySrbStrings[144]);
	    fprintf(stdout,"<OPTION VALUE=\"showanno\">%s", mySrbStrings[87]);
	    fprintf(stdout,"<OPTION VALUE=\"comminsert\">%s", mySrbStrings[94]);
	    fprintf(stdout,"<OPTION VALUE=\"metainsert\">%s", mySrbStrings[91]);
	    fprintf(stdout,"<OPTION VALUE=\"modifymetaform\">%s", mySrbStrings[95]);
	    fprintf(stdout,"<OPTION VALUE=\"copymetaform\">%s", mySrbStrings[97]);
	    fprintf(stdout,"<OPTION VALUE=\"deletemetaform\">%s", mySrbStrings[99]);
	    fprintf(stdout,"<OPTION VALUE=\"annoinsert\">%s", mySrbStrings[93]);
	    fprintf(stdout,"<OPTION VALUE=\"deleteannoform\">%s", mySrbStrings[100]);
	    fprintf(stdout,"<OPTION VALUE=\"modifyannoform\">%s", mySrbStrings[96]);
	    fprintf(stdout,"<OPTION VALUE=\"copyannoform\">%s", mySrbStrings[98]);
	    fprintf(stdout,"<OPTION VALUE=\"copydsform\">%s", mySrbStrings[138]);
	    fprintf(stdout,"<OPTION VALUE=\"movedsform\">%s", mySrbStrings[139]);
	    fprintf(stdout,"<OPTION VALUE=\"slinksform\">%s", mySrbStrings[140]);
	    fprintf(stdout,"<OPTION VALUE=\"renamedsform\">%s", mySrbStrings[141]);
	    fprintf(stdout,"<OPTION VALUE=\"aclform\">%s", mySrbStrings[110]);
	    fprintf(stdout,"<OPTION VALUE=\"giveticketform\">%s", mySrbStrings[111]);
	    fprintf(stdout,"<OPTION VALUE=\"removeDataCollForm\">%s", mySrbStrings[142]);
	    fprintf(stdout,"</SELECT>\n");

	}
	fprintf(stdout,"<INPUT NAME=\"dataname\" type=hidden VALUE=\"\">\n");
       fprintf(stdout,"<INPUT NAME=\"collection\" type=hidden VALUE=\"%s\">\n",
		Sentries->nonurlcollList);
	fprintf(stdout,"<INPUT NAME=\"seek\" type=hidden VALUE=\"0\">\n");
	fprintf(stdout,"<INPUT NAME=\"copynum\" type=hidden VALUE=\"0\">\n");
	fprintf(stdout,"<INPUT NAME=\"datatype\" type=hidden VALUE=\"\">\n");
	fprintf(stdout,"<INPUT NAME=\"vernum\" type=hidden VALUE=\"0\">\n");
	fprintf(stdout,"<INPUT NAME=\"owner\" type=hidden VALUE=\"%s@%s\">\n",ownerList,domainList);

	fprintf(stdout,"<INPUT NAME=\"session\" type=hidden VALUE=\"%s\">\n",
		Sentries->pidStr);
	fprintf(stdout,"</FONT></TD></FORM>\n");
	if (Sentries->isTicket) {
	  fprintf(stdout,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=ticketfull&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/collimg.gif\" ALIGN=middle HEIGHT=16 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=ticketfull&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\">%s </A>","",Sentries->urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr, webPath,
		 "", Sentries->urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr, Sentries->nonurlcollList);
	}
	else {
	  fprintf(stdout,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/collimg.gif\" ALIGN=middle HEIGHT=16 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\">%s </A>","",Sentries->urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr, webPath,
		 "", Sentries->urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr, Sentries->nonurlcollList);
	}

#ifdef DATACOMMENTS
	if (strlen(collComments) > 0) 
	  fprintf (stdout,"&nbsp;&nbsp;&nbsp;<IMG SRC=\"%s/commentsbut.gif\" ALT=\"%s\" ALIGN=middle width=18 height=18 border=0></A>\n", webPath,collComments);
#endif


	fprintf(stdout,"</TD><TD  nowrap align=center> %s </TD><TD nowrap align=center> %s@%s </TD></TR>\n",timeList,ownerList,domainList);
	if (Sentries->showAllMetaFlag == 1) {
	  fprintf (stdout,"<TR>");
	  fprintf (stdout,"<TD valign=top align=right><B>Metadata:</B></TD><TD COLSPAN=8>\n");
	  strcpy(Sentries->tmpCollName, Sentries->entries[2].val);
	  strcpy(Sentries->tmpDataName, Sentries->entries[1].val);
	  strcpy(Sentries->entries[2].val, Sentries->nonurlcollList);
	  strcpy(Sentries->entries[1].val,"");
	  showMeta(Sentries);
	  strcpy(Sentries->entries[2].val, Sentries->tmpCollName);
	  strcpy(Sentries->entries[1].val, Sentries->tmpDataName);
	  fprintf (stdout,"<HR NOSHADE></TD></TR>\n");
	}
        tmpList += MAX_DATA_SIZE;
        ownerList += MAX_DATA_SIZE;
        domainList += MAX_DATA_SIZE;
        timeList += MAX_DATA_SIZE;
	collComments += MAX_DATA_SIZE;
/*	collLinkNumList	+= MAX_DATA_SIZE; */
	Sentries->rowCntr++;
    }
}



int
prtTicketCollWeb (inStruct Sentries, mdasC_sql_result_struct *myresult)
{
    int i;
    char *tmpList;
    char *ownerList;
    char *domainList;
    char *collComments;
    char *timeList;

    tmpList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[C_TICKET_COLL_NAME], dcs_aname[C_TICKET_COLL_NAME]);
    ownerList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_OWNER_NAME], dcs_aname[COLL_OWNER_NAME]);
    domainList = (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[COLL_OWNER_DOMAIN], dcs_aname[COLL_OWNER_DOMAIN]);
    collComments= (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[C_TICKET_COLL_COMMENTS], dcs_aname[C_TICKET_COLL_COMMENTS]);
    timeList= (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[C_TICKET_COLL_CREATE_TIMESTAMP], dcs_aname[C_TICKET_COLL_CREATE_TIMESTAMP]);
    oddFlag = 0;
    

    if (first) {
      fprintf (stdout,"\n<TR BGCOLOR=\"#DDEFFF\"><TH>Function</TH><TH>SubCollection</TH><TH>Creation Time</TH><TH>Owner</TH></TR>\n");
	    first = 0;
    }
    for (i = 0; i < myresult->row_count; i++) {
      	strcpy(urlcollList,tmpList);
	strcpy(nonurlcollList,tmpList);
	escape_url(urlcollList);
	oddFlag = (oddFlag + 1) % 2;
	if (oddFlag) {
	  fprintf (stdout,"<TR bgcolor=\"#88BBFF\">");
	}
	else {
	  fprintf (stdout,"<TR bgcolor=\"#DDEEFF\">");
	}
        fprintf (stdout,"<TD><A HREF=\"mysrb331.cgi?function=ticketmeta&dataname=%s&collection=%s&owner=%s@%s&session=%s\" ><IMG SRC=\"%s/white-ball.gif\" ALT=\"Show Metadata\"></A></TD>\n","", urlcollList,ownerList,domainList,Sentries->pidStr, webPath);

	fprintf(stdout,"<TD NOWRAP><A HREF=\"mysrb331.cgi?function=ticketfull&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\"><IMG SRC=\"%s/collimg.gif\" HEIGHT=16 BORDER=0></A>\n<A HREF=\"mysrb331.cgi?function=ticketfull&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"%s\">%s </A>","",urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr, webPath,
		 "",urlcollList,ownerList,domainList, Sentries->pidStr, Sentries->frameStr,tmpList);
#ifdef DATACOMMENTS
	if (strlen(collComments) > 0) 
	  fprintf (stdout,"&nbsp;&nbsp;&nbsp;<IMG SRC=\"%s/commentsbut.gif\" ALT=\"%s\" width=18 height=18 border=0></A>\n", webPath,collComments);
#endif


	fprintf(stdout,"</TD><TD nowrap align=center> %s </TD><TD nowrap align=center> %s@%s </TD></TR>\n",timeList,ownerList,domainList);
	if (showAllMetaFlag == 1) {
	  /**	  if (oddFlag) 
	    fprintf (stdout,"<TR>");
	  else
	  fprintf (stdout,"<TR bgcolor=\"#DDEEFF\">");**/
	  fprintf (stdout,"<TR>");
	  fprintf (stdout,"<TD valign=top align=right><B>Metadata:</B></TD><TD COLSPAN=8>\n");
	  strcpy(tmpCollName,Sentries->entries[2].val);
	  strcpy(tmpDataName,Sentries->entries[1].val);
	  strcpy(Sentries->entries[2].val,tmpList);
	  strcpy(Sentries->entries[1].val,"");
	  showMeta(Sentries);
	  strcpy(Sentries->entries[2].val,tmpCollName);
	  strcpy(Sentries->entries[1].val,tmpDataName);
	  fprintf (stdout,"<HR NOSHADE></TD></TR>\n");
	}
        tmpList += MAX_DATA_SIZE;
        ownerList += MAX_DATA_SIZE;
        domainList += MAX_DATA_SIZE;
        timeList += MAX_DATA_SIZE;
	collComments += MAX_DATA_SIZE;
	
    }
}


querySubCollInfo(inStruct Sentries, srbConn *conn, int catType, char *parColl, int L_val, 
mdasC_sql_result_struct  *myresult)
{
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, status;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    selval[DATA_GRP_NAME] = 1;
    selval[COLL_OWNER_NAME] = 1;
    selval[COLL_OWNER_DOMAIN] = 1;
    selval[COLL_CREATE_TIMESTAMP] = 1;
    selval[COLL_COMMENTS] = 1;
    sprintf(qval[PARENT_COLLECTION_NAME]," = '%s'", parColl);
/*    selval[COLLECTION_LINK_NUM ] = 1;
*/
    status = srbGetDataDirInfoShunt(Sentries,conn, MDAS_CATALOG, qval, selval, myresult, L_val);

    if (status == 0) {
        filterDeleted (myresult);
    }

    if (status < 0) {
        if (status != MCAT_INQUIRE_ERROR)       /* no answer */
            fprintf (stderr, "Error listing collection: %s, status = %d\n",
          parColl, status);
    }
    return (status);
}

int
editData(inStruct Sentries)
{
   char *trgColl;
   char trgObj[MAX_TOKEN * 2];
   int  i,out_fd, status,dataSize;

   sprintf(trgObj,"%s&COPY=%s&DVERSION='%s'",Sentries->entries[1].val,Sentries->entries[4].val,Sentries->entries[6].val);
   trgColl = Sentries->entries[2].val;
   dataSize = strlen(Sentries->entries[Sentries->m].val);
   printf("<BR>ReIngest Operation Beginning ... "); fflush(stdout);

   out_fd = srbObjOpen (Sentries->conn, trgObj, O_WRONLY|O_TRUNC, trgColl);
   if (out_fd < 0) {
     printf ("Unable to open object %s/%s,  <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, out_fd,out_fd);

     srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   substituteString(Sentries->entries[Sentries->m].val, "</areatext>","</textarea>");
   i = srbObjWrite(Sentries->conn, out_fd, Sentries->entries[Sentries->m].val,dataSize);
   if (i != dataSize) {
     printf ("Write Error when create object %s/%s, wrote only %d of %d\n",
              trgColl, trgObj, i, dataSize);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   i = srbObjClose(Sentries->conn,out_fd);
   if (i < 0) {
     printf ("Error when closing object %s/%s. <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, i,i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); 
   bRMap(2,1);
   return;
}

int
reIngestFileIntoSRB(inStruct Sentries)
{
   char *trgColl;
   char trgObj[MAX_TOKEN * 2];
   int  i,out_fd, status,dataSize;

   sprintf(trgObj,"%s&COPY=%s&DVERSION='%s'",Sentries->entries[1].val,Sentries->entries[3].val,Sentries->entries[4].val);
   trgColl = Sentries->entries[2].val;
   dataSize = Sentries->entries[Sentries->m].size;

   printf("<BR>Ingest Operation Beginning ... "); fflush(stdout);

   out_fd = srbObjOpen (Sentries->conn, trgObj, O_WRONLY|O_TRUNC, trgColl);
   if (out_fd < 0) {
     printf ("Unable to open object %s/%s,  <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, out_fd,out_fd);

     srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }

   i = srbObjWrite(Sentries->conn, out_fd, Sentries->entries[Sentries->m].val,dataSize);
   if (i != dataSize) {
     printf ("Write Error when create object %s/%s, wrote only %d of %d\n",
              trgColl, trgObj, i, dataSize);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }

   i = srbObjClose(Sentries->conn,out_fd);
   if (i < 0) {
     printf ("Error when closing object %s/%s. <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, i,i);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); 
   bRMap(2,1);
   return;
}

int execData(inStruct Sentries)
{

    char netPrefix[MAX_TOKEN];
    char defPath[MAX_TOKEN * 4];
    char cmdStr[1000];
    char dataStr[MAX_TOKEN];
    int i,j;

    if ((tmpPtr= strstr(Sentries->entries[5].val," ")) != NULL) {
      *tmpPtr = '\0'; tmpPtr++;
      strcpy(cmdStr,Sentries->entries[5].val);
      sprintf(defPath," %s %s",tmpPtr, Sentries->entries[8].val);
    }
    else {
     strcpy(cmdStr,Sentries->entries[5].val);
     sprintf(defPath," %s", Sentries->entries[8].val);
    }

    i = getNetprefixForResource( Sentries, Sentries->entries[3].val, netPrefix);

    if (i < 0) {
      printf("Host Name Error for %s <A href=\"srberror2.cgi?%i\">Error: %i</A> \n",Sentries->entries[3].val, i,i);
      return(i);
    }

    i = srbExecCommand (Sentries->conn, cmdStr, defPath, netPrefix, "", 
			PORTAL_STD_IN_OUT);
   
    if (i < 0) {
      printf("Error in Execution <A href=\"srberror2.cgi?%i\">Error: %i</A> \n", i,i);
      return(i);
    }
    printf ("<PRE>");
    /*   while ((j = read (i, dataStr, (int) (sizeof(dataStr) - 20))) > 0) { */
    /*
    dataStr = malloc(1000000);
    if (dataStr == NULL) {
      printf("Error in Allocating Memory\n");
      return(0);
    }
    while ((j = read (i, dataStr, 1000000 - 5) > 0)) {
        if (j < sizeof(dataStr))
            buf[j] = '\0';
        printf ("%s", dataStr);
    }  */
    
    while ((j = read (i, dataStr, sizeof(dataStr))) > 0) { /* ok to use sizeof() here since dataStr has been declared as char[xxx] and not as a pointer */
        fwrite (dataStr, j, 1, stdout);
    }

    printf ("</PRE>");
    return(0);
}


int
getDataNamesinCollection(inStruct Sentries, char *collName)
{
  mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j, first;
    int status;
    char *tmpList, *origTmpList;
    char *data = Sentries->selectData;
    
    
   strcpy(data,"");
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[DATA_NAME] = 1;
   sprintf(qval[COLLECTION_NAME]," = '%s' ",collName);
   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult,
			      MAX_RESULT_NUM);
   first = 1;
   sprintf(data,"<SELECT SIZE=1 NAME=dname1>");
   if (status < 0 || myResult.row_count == 0) {
     sprintf(data,"");
     return -1;
   }
   tmpList = (char *) getFromResultStruct(
       &myResult,
       dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
   if (tmpList == NULL)
     printf("8");fflush(stdout);    
   origTmpList = tmpList;

   for ( i = 0; i < myResult.row_count; i++) {

     if (first) {
       strcat(data,"<OPTION SELECTED> ");
       first = 0;
     }
     else
       strcat(data,"<OPTION> ");
      strcat(data,tmpList);
      tmpList += MAX_DATA_SIZE;
   }
   strcat(data,"</SELECT>");
   free(origTmpList);
   return 0;
}

int
getDatasetAnno(inStruct Sentries, char *collection, char *dataName, int updateFlag)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;


    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      Sentries->likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    if (strstr(dataName,"^")) {
       make_like_for_mdasmysrb(dataName);
       client_set_qval_data_cond(dataName,qval);
    }
    else {
    if (strstr(dataName,"*") || strstr(dataName,"?")) {
      make_like_for_mdasmysrb(dataName);
      sprintf(qval[DATA_NAME], " like '%s'  ESCAPE '\\' ",dataName);
    }
    else {
      sprintf(qval[DATA_NAME], " =  '%s'",dataName);
    }
   }
    selval[DATA_GRP_NAME] = 1;
    selval[DATA_NAME] = 1;
      selval[DATA_ANNOTATION] = 14;
      selval[DATA_ANNOTATION_TIMESTAMP] = 1;
      selval[DATA_ANNOTATION_USERNAME] = 1;
      selval[DATA_ANNOTATION_USERDOMAIN] = 1;
      selval[DATA_ANNOTATION_POSITION] = 1;


    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
                               MAX_RESULT_NUM);

    if (status < 0)     {
        if (status != MCAT_INQUIRE_ERROR)
          printf("Annotation Access <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", status, status);
	else
	  printf("<HR>%s\n", mySrbStrings[29]);
    }
    else {
      filterDeleted (&collResult);
      first = 1;
      prtAnnoData(Sentries, &collResult, updateFlag);
      while (collResult.continuation_index >= 0) {
        clearSqlResult (&collResult);
        status = srbGetMoreRows(Sentries->conn, 0,
                                collResult.continuation_index,
                                &collResult, MAX_RESULT_NUM);
        if (collResult.result_count == 0  || status != 0)
          break;
        filterDeleted (&collResult);
        prtAnnoData(Sentries, &collResult, updateFlag);
      }
      clearSqlResult (&collResult);
    }
    if (updateFlag == 1) {
      fprintf(stdout,"</TABLE><input type=hidden name=\"session\" value=\"%s\">\
<FONT COLOR=#0000FF>%s<BR>%s<BR>\n</FONT>\
<INPUT TYPE=SUBMIT VALUE=\"%s\"></FORM>\n", Sentries->pidStr, mySrbStrings[208], mySrbStrings[214], mySrbStrings[209]);
    }
     return(0);
}


int 
getDatasetDC(inStruct Sentries, int updateFlag)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;
    char *collection = Sentries->entries[2].val;
    char *dataName = Sentries->entries[1].val;


    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    if (strstr(dataName,"^")) {
       make_like_for_mdasmysrb(dataName);
       client_set_qval_data_cond(dataName,qval);
    }
    else {
      if (strstr(dataName,"*") || strstr(dataName,"?")) {
        make_like_for_mdasmysrb(dataName);
        sprintf(qval[DATA_NAME], " like '%s'  ESCAPE '\\' ",dataName);
      }
      else {
        sprintf(qval[DATA_NAME], " =  '%s'",dataName);
      }
   }
    sprintf(qval[UDSMD0], " like 'DC.%%' ");
    selval[DATA_GRP_NAME] = 1;
    selval[DATA_NAME] = 1;
    selval[METADATA_NUM] = 1;
    selval[UDSMD0] = 1;
    selval[UDSMD1] = 1;
    selval[UDSMD2] = 1;
    

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadat Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	else 
	  printf("No Dublin Core Metadata Found\n");
	return(status);
    }
    else {
      filterDeleted (&collResult);
      first = 1;
      prtMetaData(Sentries, &collResult,updateFlag);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtMetaData(Sentries, &collResult,updateFlag);
      }
      clearSqlResult (&collResult);
    }
    return(0);
}


int 
getCollectionDC(inStruct Sentries, int updateFlag)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;
    char *collection = Sentries->entries[2].val;


    Sentries->showMetaType = 2;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }

    sprintf(qval[UDSMD_COLL0], " like 'DC.%%' ");
    selval[METADATA_NUM_COLL] = 1;    
    selval[DATA_GRP_NAME] = 1;
    selval[UDSMD_COLL0] = 1;
    selval[UDSMD_COLL1] = 1;
    selval[UDSMD_COLL2] = 1;
    selval[UDSMD_COLL3] = 1;
    selval[UDSMD_COLL4] = 1;    

    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);



    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadat Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	else 
	  printf("No Dublin Core Metadata Found\n");
	return(status);
    }
    else {
      filterDeleted (&collResult);
      Sentries->first = 1;
      prtMetaCollNew(Sentries, &collResult,collection);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtMetaCollNew(Sentries, &collResult,collection);
      }
      clearSqlResult (&collResult);
    }
    return(0);
}

int 
getDatasetMeta(inStruct Sentries, int updateFlag)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;
    char *collection = Sentries->entries[2].val;
    char *dataName = Sentries->entries[1].val;


    if ((tmpPtr= strstr(dataName,"@SHADOW=")) != NULL)
      *tmpPtr = '\0';

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      Sentries->likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    if (strstr(dataName,"^")) {
       make_like_for_mdasmysrb(dataName);
       client_set_qval_data_cond(dataName,qval);
    }
    else {
      if (strstr(dataName,"*") || strstr(dataName,"?")) {
        make_like_for_mdasmysrb(dataName);
        sprintf(qval[DATA_NAME], " like '%s'  ESCAPE '\\' ",dataName);
      }
      else {
        sprintf(qval[DATA_NAME], " =  '%s'",dataName);
      }
    }

    if (tmpPtr != NULL)
      *tmpPtr = '@';

 
   if (atoi(Sentries->entries[6].val) < 0) {
      if ((tmpPtr = strstr(Sentries->entries[5].val,"/?SHADOW")) != NULL) {
	tmpPtr += 8;
	if (strlen(tmpPtr) > 0) {
	  if (atoi(Sentries->entries[6].val) == -1)
	    sprintf(qval[UDSMD5]," = 'F:%s'",tmpPtr);
	  else
	    sprintf(qval[UDSMD5]," = 'D:%s'",tmpPtr);
	}
      }
   }
   else
    sprintf(qval[UDSMD5]," is NULL");

    selval[DATA_GRP_NAME] = 1;
    selval[DATA_NAME] = 1;
    selval[METADATA_NUM] = 1;
    selval[UDSMD0] = 1;
    selval[UDSMD1] = 1;
    selval[UDSMD2] = 1;


    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadat Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	else
	  printf("%s\n", mySrbStrings[28]);
	return(status);
    }
    else {
      filterDeleted (&collResult);
      Sentries->first = 1;
      prtMetaData(Sentries, &collResult,updateFlag);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtMetaData(Sentries, &collResult,updateFlag);
      }
      clearSqlResult (&collResult);
    }
    if (updateFlag == 1) {
      fprintf(stdout,"</TABLE><input type=hidden name=\"session\" value=\"%s\">\
<FONT COLOR=#0000FF>%s<BR>%s<BR>\n</FONT>\
<INPUT TYPE=SUBMIT VALUE=\"%s\"></FORM>\n",Sentries->pidStr, mySrbStrings[208], mySrbStrings[210], mySrbStrings[209]);
    }

    return(0);
}


int 
getDatasetMetaNew(inStruct Sentries)
{
    char *collection = Sentries->entries[2].val;
    char *dataName = Sentries->entries[1].val;
    int updateFlag = 0;
    mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;

    if ((Sentries->tmpPtr= strstr(dataName,"@SHADOW=")) != NULL)
      *Sentries->tmpPtr = '\0';

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdasmysrb(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      Sentries->likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    if (strstr(dataName,"^")) {
       make_like_for_mdasmysrb(dataName);
       client_set_qval_data_cond(dataName,qval);
    }
    else {
      if (strstr(dataName,"*") || strstr(dataName,"?")) {
        make_like_for_mdasmysrb(dataName);
        sprintf(qval[DATA_NAME], " like '%s'  ESCAPE '\\' ",dataName);
      }
      else {
        sprintf(qval[DATA_NAME], " =  '%s'",dataName);
      }
    }

    if (Sentries->tmpPtr != NULL)
      *Sentries->tmpPtr = '@';

 
   if (atoi(Sentries->entries[6].val) < 0) {
      if ((Sentries->tmpPtr = strstr(Sentries->entries[5].val,"/?SHADOW")) != NULL) {
	Sentries->tmpPtr += 8;
	if (strlen(Sentries->tmpPtr) > 0) {
	  if (atoi(Sentries->entries[6].val) == -1)
	    sprintf(qval[UDSMD5]," = 'F:%s'",Sentries->tmpPtr);
	  else
	    sprintf(qval[UDSMD5]," = 'D:%s'",Sentries->tmpPtr);
	}
      }
   }
   else
     sprintf(qval[UDSMD5]," is NULL");

    selval[DATA_GRP_NAME] = 1;
    selval[DATA_NAME] = 1;
    selval[METADATA_NUM] = 1;
    selval[UDSMD0] = 1;
    selval[UDSMD1] = 1;
    selval[UDSMD2] = 1;


    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &collResult, MAX_RESULT_NUM);

    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadat Access <A href=\"srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	else
	  printf("%s\n", mySrbStrings[28]);
	return(status);
    }
    else {
      filterDeleted (&collResult);
      first = 1;
      prtMetaDataNew(Sentries, &collResult,updateFlag);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(Sentries->conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtMetaDataNew(Sentries, &collResult,updateFlag);
      }
      clearSqlResult (&collResult);
    }
    if (updateFlag == 1) {
      fprintf(stdout,"</TABLE><input type=hidden name=\"session\" value=\"%s\">\
<FONT COLOR=#0000FF>%s<BR>%s<BR>\n</FONT>\
<INPUT TYPE=SUBMIT VALUE=\"%s\"></FORM>\n",Sentries->pidStr, mySrbStrings[208], mySrbStrings[210], mySrbStrings[209]);
    }

    return(0);
}


int
ingestFileIntoSRB(inStruct Sentries)
{
   char *trgColl,  *rsrcName, *dataType;
   int  i, ii, iii, out_fd, status, dataSize;
   int m = Sentries->m;
   char trgObj[MAX_TOKEN * 3];
   char newVal[MAX_TOKEN];
   char iName[MAX_TOKEN];
   char iVal[MAX_TOKEN];
   char iUnit[MAX_TOKEN];

   
   strcpy(trgObj, Sentries->entries[1].val);
   trgColl = Sentries->entries[2].val;
   if (strlen(Sentries->entries[4].val) > 0) {
     rsrcName = Sentries->entries[3].val;
     strcat(trgObj,"&CONTAINER=");
     strcat(trgObj,Sentries->entries[4].val);

   }
   else {
     rsrcName = Sentries->entries[3].val;
   }   
   
   dataType = Sentries->entries[5].val;
   dataSize = Sentries->entries[m].size;
   if (dataSize == 0)
     dataSize = strlen(Sentries->entries[m].val);

   printf("Content-type: text/html%c%c",10,10);fflush(stdout);
   printf("<html>");
   if (strlen(trgObj) == 0) {
     printf("<body background=\"%s/srb3.jpg\">", webPath);
     printf ("Error: Target Data Name Empty. Please use back button and resubmit.\n");
     bRMap(1,0);
     printf("</body></html>");
     return;
   }
   
   printf("<title>Ingesting data file %s/%s</title> <base target = \"_top\">", Sentries->entries[2].val, Sentries->entries[1].val);
   printf("<body background=\"%s/srb3.jpg\">", webPath);
   printf("<BR>Ingest Operation Beginning for %s/%s of Size %d ... ",
	 trgColl, trgObj, dataSize ); 
   fflush(stdout);
   out_fd = srbObjCreate (Sentries->conn, 0, trgObj,dataType,rsrcName,trgColl,"",dataSize);
   if (out_fd < 0) {
     printf ("Unable to create object %s/%s,  <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, out_fd,out_fd);
     srb_perror (2, out_fd, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   i = srbObjWrite(Sentries->conn, out_fd, Sentries->entries[m].val,dataSize);
   if (i != dataSize) {
     printf ("Write Error when create object %s/%s, wrote only %d of %d\n",
              trgColl, trgObj, i, dataSize);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   i = srbObjClose(Sentries->conn,out_fd);
   if (i < 0) {
     printf ("Error when closing object %s/%s. <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
              trgColl, trgObj, i,i);
     bRMap(2,0);
     srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); fflush(stdout);
   printf("<BR>%s ... ",aaa[25]); fflush(stdout);
   strcpy(trgObj, Sentries->entries[1].val);

     for (i = 6; i < m ; i =  i+3) {
       if (strlen(Sentries->entries[i+1].val) == 0)
	 continue;
       strcpy(iName, Sentries->entries[i].val);
       strcpy(iVal, Sentries->entries[i+1].val);     
     strcpy(iUnit,Sentries->entries[i+2].val);
     iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   "0",iName,
			   D_INSERT_USER_DEFINED_STRING_META_DATA);
     if (iii < 0) {
       printf ("Metadata Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName,iii,iii);
       srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     sprintf(newVal,"1@%i",iii);

     ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   newVal,iVal,
			   D_CHANGE_USER_DEFINED_STRING_META_DATA);
     if (ii < 0) {
       printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName, iVal,ii,ii);
       srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     if (strlen(iUnit) > 0) {
       sprintf(newVal,"2@%i",iii);
       ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			     newVal,iUnit,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iUnit,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }


   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);
   printf("</body></html>");
   return;
}


int
insertDataACL(inStruct Sentries)
{
  char *trgColl, *trgObj, *userDom, *accsName;
  char userName[MAX_TOKEN], domainName[MAX_TOKEN];
  int i,ii;
  char newVal[MAX_TOKEN];
  char iName[MAX_TOKEN];
  char iVal[MAX_TOKEN];
   printf("<BR>Starting ACL Ingest ... "); fflush(stdout);

   trgObj = Sentries->entries[1].val;
   trgColl = Sentries->entries[2].val;
   userDom = Sentries->entries[3].val;
   accsName = Sentries->entries[4].val;

   if (strlen(trgObj) > 0) {
        if (!strcmp(accsName,"null")) {
            ii = srbModifyDataset(Sentries->conn,  MDAS_CATALOG, trgObj, trgColl,
               "","", userDom, "", D_DELETE_ACCS);
        } else {
            ii = srbModifyDataset(Sentries->conn,  MDAS_CATALOG, trgObj, trgColl,
                   "","", userDom, accsName, D_INSERT_ACCS);
        }
   }
   else {
     clSplitbychar (userDom, userName, domainName, '@');
     switch (atoi(Sentries->entries[5].val))
       {
       case 0: /* This Collection Only */
	 if (!strcmp(accsName,"null")) {
	   ii = srbModifyCollect (Sentries->conn,  MDAS_CATALOG, trgColl,userName, 
				  domainName, "", D_DELETE_COLL_ACCS);
	 } else {
	   ii = srbModifyCollect (Sentries->conn, MDAS_CATALOG, trgColl,userName, 
				  domainName, accsName,D_INSERT_COLL_ACCS);
	 }
	 break;
       case 1: /* All Files and SubCollections in This Collection*/
	 if (!strcmp(accsName,"null")) {
	   ii = srbModifyCollect (Sentries->conn,  MDAS_CATALOG, trgColl,userName, 
				  domainName, "", D_DELETE_INCOLL_ACCS);
	 } else {
	   ii = srbModifyCollect (Sentries->conn, MDAS_CATALOG, trgColl,userName, 
				  domainName, accsName,D_INSERT_INCOLL_ACCS);
	 }
	 break;
       case 2: /*This Coll  and All Files and SubColls in Coll */
	 if (!strcmp(accsName,"null")) {
	   ii = srbModifyCollect (Sentries->conn,  MDAS_CATALOG, trgColl,userName, 
				  domainName, "", D_DELETE_INCOLL_ACCS);
	   if (ii < 0) break;
	   ii = srbModifyCollect (Sentries->conn,  MDAS_CATALOG, trgColl,userName, 
				  domainName, "", D_DELETE_COLL_ACCS);
	 } else {
	   ii = srbModifyCollect (Sentries->conn, MDAS_CATALOG, trgColl,userName, 
				  domainName, accsName,D_INSERT_INCOLL_ACCS);
	   if (ii < 0) break;
	   ii = srbModifyCollect (Sentries->conn, MDAS_CATALOG, trgColl,userName, 
				  domainName, accsName,D_INSERT_COLL_ACCS);
	 }
	 break;
       case 3: /* Recursively All Files, SubColl in This Collection*/
	 if (!strcmp(accsName,"null")) {
	   ii = srbModifyCollect (Sentries->conn,  MDAS_CATALOG, trgColl,userName, 
			  domainName, "", D_DELETE_INCOLL_ACCS_RECUR);
	 } else {
	   ii = srbModifyCollect (Sentries->conn, MDAS_CATALOG, trgColl,userName, 
		     domainName, accsName,D_INSERT_INCOLL_ACCS_RECUR);
	 }
	 break;
       case 4: /*This Coll and  Recursively All Files, SubColls in Coll */
	 if (!strcmp(accsName,"null")) {
	   ii = srbModifyCollect (Sentries->conn,  MDAS_CATALOG, trgColl,userName, 
			  domainName, "", D_DELETE_INCOLL_ACCS_RECUR);
	   if (ii < 0) break;
	   ii = srbModifyCollect (Sentries->conn,  MDAS_CATALOG, trgColl,userName, 
				  domainName, "", D_DELETE_COLL_ACCS);
	 } else {
	   ii = srbModifyCollect (Sentries->conn, MDAS_CATALOG, trgColl,userName, 
		     domainName, accsName,D_INSERT_INCOLL_ACCS_RECUR);
	   if (ii < 0) break;
	   ii = srbModifyCollect (Sentries->conn, MDAS_CATALOG, trgColl,userName, 
				  domainName, accsName,D_INSERT_COLL_ACCS);
	 }
	 break;
       default:
	 ii = ACTION_NOT_ALLOWED;
	 break;
	 
       }
   }
   if (ii < 0) {
     printf ("ACL Ingestion Error for %s/%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,trgObj,ii,ii);
     srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);
   printf("</body></html>");
   return;

}


int
insertDataAnnotation(inStruct Sentries)
{
  char *trgColl, *trgObj, *trgSlinkVal;
  int i,ii;
  char newVal[MAX_TOKEN];
  char iName[3 * MAX_TOKEN];
  char iVal[MAX_TOKEN];
   printf("<BR>Starting Annotation Ingest ... "); fflush(stdout);

   trgObj = Sentries->entries[1].val;
   trgColl = Sentries->entries[2].val;
   trgSlinkVal = Sentries->entries[3].val;
   for (i = 4; i < Sentries->m ; i=i+2) {
     if (strlen(Sentries->entries[i].val) == 0 || strlen(Sentries->entries[i+1].val) == 0 )
       continue;
     if (strlen(trgSlinkVal) == 0)
       strcpy(iName,Sentries->entries[i].val);
     else
       sprintf(iName,"%s@%s",trgSlinkVal,Sentries->entries[i].val);
     iName[MAX_TOKEN-1] = '\0';
     strcpy(iVal,Sentries->entries[i+1].val);
     ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			  iVal,iName, 
			  D_INSERT_ANNOTATIONS);
     if (ii < 0) {
       printf ("Annotation Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName,ii,ii);
       srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     sleep(1);
   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);
   printf("</body></html>");
   return;

}

int
insertExtSchmMetaData(inStruct Sentries)
{
  
  char *trgColl, *trgObj, *colNames, *tableName;
  int numOfCols;
  int i,j,k;
  char valStr[HUGE_STRING];

  printf("<BR>%s ... ",aaa[25]); fflush(stdout);

  trgObj = Sentries->entries[1].val;
  trgColl = Sentries->entries[2].val;
  tableName = Sentries->entries[5].val;
  colNames = Sentries->entries[6].val;
  numOfCols = atoi(Sentries->entries[4].val);
  strcpy(valStr,"");
  for  (j = 7; j <= Sentries->m; j = j+ numOfCols) {
    if (strlen(Sentries->entries[j].val) == 0)
      continue;
    if (j > 7)
      strcat(valStr,"\n");
    strcat(valStr,Sentries->entries[j].val);
    for (k = 1; k <numOfCols ; k++) {
      strcat(valStr,"|");
      strcat(valStr,Sentries->entries[j+k].val);
    }
  }
  /*printf("<BR>PPPP:Before srbModifyExtMetaData o=%s c=%s t=%s<br> cc=%s<br>v=%s<br>\n",trgObj,trgColl,tableName,colNames,valStr);*/
  i = srbModifyExtMetaData (Sentries->conn, MDAS_CATALOG,trgObj,trgColl,tableName,colNames,valStr,
			    "","",D_INSERT_INTO_EXTMD_TABLE);
  
  if (i < 0) {
         printf ("Metadata Ingestion Error: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
                 i,i);
         srb_perror (2, i, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
         bRMap(2,0);
         printf("</body></html>");
         return;
  }
  printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);
   printf("</body></html>");
   return;

}

int
insertDataMetaData(inStruct Sentries)
{
   char *trgColl, *trgObj, *trgSlinkVal;
   int i, ii, iii;
   char newVal[MAX_TOKEN];
   char iName[MAX_TOKEN];
   char iVal[MAX_TOKEN];
   char iUnit[MAX_TOKEN];
   char iDef[MAX_TOKEN];
   char iCom[MAX_TOKEN];
   printf("<BR>%s ... ",aaa[25]); fflush(stdout);

   trgObj = Sentries->entries[1].val;
   trgColl = Sentries->entries[2].val;
   trgSlinkVal = Sentries->entries[3].val;
   for (i = 4; i < Sentries->m ; i=i+5) {
     if (strlen(Sentries->entries[i].val) == 0 || strlen(Sentries->entries[i+1].val) == 0 )
       continue;
     strcpy(iName,Sentries->entries[i].val);
     strcpy(iVal,Sentries->entries[i+1].val);
     strcpy(iUnit,Sentries->entries[i+2].val);
     strcpy(iDef,Sentries->entries[i+3].val);
     strcpy(iCom,Sentries->entries[i+4].val);
     iii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   "0",iName,
			   D_INSERT_USER_DEFINED_STRING_META_DATA);
     if (iii < 0) {
       printf ("Metadata Ingestion Error for %s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName,iii,iii);
       srb_perror (2, iii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     sprintf(newVal,"1@%i",iii);
     ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			   newVal,iVal,
			   D_CHANGE_USER_DEFINED_STRING_META_DATA);
     if (ii < 0) {
       printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	       iName, iVal,ii,ii);
       srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
       bRMap(2,0);
       printf("</body></html>");
       return;
     }
     if (strlen(iUnit) > 0) {
       sprintf(newVal,"2@%i",iii);
       ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			     newVal,iUnit,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iUnit,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }
     if (strlen(iDef) > 0) {
       sprintf(newVal,"3@%i",iii);
       ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			     newVal,iDef,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iDef,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }
     if (strlen(iCom) > 0) {
       sprintf(newVal,"4@%i",iii);
       ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			     newVal,iCom,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, iCom,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }
     if (strlen(trgSlinkVal)> 0) {
       sprintf(newVal,"5@%i",iii);
       ii = srbModifyDataset(Sentries->conn, MDAS_CATALOG, trgObj,trgColl, "","",
			     newVal,trgSlinkVal,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
       if (ii < 0) {
	 printf ("Metadata Ingestion Error for %s=%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
		 iName, trgSlinkVal,ii,ii);
	 srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
	 bRMap(2,0);
	 printf("</body></html>");
	 return;
       }
     }



   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);
   printf("</body></html>");
   return;

}



int insertDataComments(inStruct Sentries)
{
  char *trgColl, *trgObj, *comm;
  int i,ii;
  char newVal[MAX_TOKEN];
  char iName[MAX_TOKEN];
  char iVal[MAX_TOKEN];

   trgObj = Sentries->entries[1].val;
   trgColl = Sentries->entries[2].val;
   printf("<BR>Starting Comment Ingest into %s/%s... ",
	  trgColl,trgObj); fflush(stdout);
   comm = Sentries->entries[3].val;
   if (strlen(comm) == 0) 
     ii = srbModifyDataset(Sentries->conn,  MDAS_CATALOG, trgObj, trgColl,
               "","", comm, "", D_DELETE_COMMENTS);
   else
     ii = srbModifyDataset(Sentries->conn,  MDAS_CATALOG, trgObj, trgColl,
               "","", comm, "", D_UPDATE_COMMENTS);
   if (ii < 0) {
     printf ("Comment Update Error for %s/%s: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	     trgColl,trgObj,ii,ii);
     srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
     bRMap(2,0);
     printf("</body></html>");
     return;
   }
   printf("Done\n<BR>"); fflush(stdout);
   bRMap(2,1);
   printf("</body></html>");
   return;

}

int
isDataInSRB(inStruct Sentries)
{
    char *coll = Sentries->entries[2].val;
    char *data = Sentries->entries[1].val;
    
  mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;

   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   sprintf(qval[DATA_GRP_NAME], " =  '%s'",coll);
   if (strlen(data) > 0) {
     sprintf(qval[DATA_NAME], " =  '%s'",data);
     selval[DATA_NAME] = 1;
   }
   else {
     selval[DATA_GRP_NAME] = 1;
   }

   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0 , qval, selval, &myResult, MAX_RESULT_NUM);
   if (status < 0 ) {
     if (status != MCAT_INQUIRE_ERROR) 
       printf("MCAT <A href=\"srberror2.cgi?%i\">Error: %i</A> \n", status, status);
     return 0;
   }
   if (myResult.row_count > 0)
     return 1;
   return 0;
}

int
deleteExtSchmMetaData(inStruct Sentries){

  char *trgColl, *trgObj, *colNames, *tableName;
  int numOfCols;
  int i,j,k,ii;
  char valStrOV[HUGE_STRING]; /* old values */

  printf("%s...\n",aaa[40]);fflush(stdout);
  trgObj = Sentries->entries[1].val;
  trgColl = Sentries->entries[2].val;
  tableName = Sentries->entries[5].val;
  colNames = Sentries->entries[6].val;
  numOfCols = atoi(Sentries->entries[4].val);
  i =  0; 

  /*
  for (i = 0; i <= Sentries->m; i++)
    printf("%i:%s=%s<BR>",i,Sentries->entries[i].name,Sentries->entries[i].val);
  fflush(stdout);
  */

  for (i = 7; i <= Sentries->m ; i++ ) {
    if (strstr(Sentries->entries[i].name,"ckbxwqzt") != Sentries->entries[i].name ||
	strcmp(Sentries->entries[i].val,"on"))
      continue;
    /*
    printf("<BR>PPP%sBB%sBB%sBB%sBB%sBB%s<BR>\n",Sentries->entries[i+1].val,Sentries->entries[i+2].val,Sentries->entries[i+3].val,Sentries->entries[i+4].val,Sentries->entries[i+5].val,Sentries->entries[i+6].val);
    fflush(stdout);
    */
    valStrOV[0] ='\0';
    for (j = 0; j < numOfCols ; j++) {
      if (Sentries->entries[i + j + 1].val[0] == 'Q' && 
	  Sentries->entries[i + j + 1].val[strlen(Sentries->entries[i + j + 1].val)-1] == 'Q') {
	Sentries->entries[i + j + 1].val[0] = '\'';
	Sentries->entries[i + j + 1].val[strlen(Sentries->entries[i + j + 1].val)-1] = '\'';
      }
      if (j > 0)
	strcat(valStrOV,"|");
      strcat(valStrOV," = ");
      strcat(valStrOV,Sentries->entries[i + j + 1].val);
    }

    /*    printf("<BR>PPPP:Before srbModifyExtMetaData o=%s c=%s t=%s<br> cc=%s<br>OA=%s<br>OV=%s<br>\n",trgObj,trgColl,tableName,colNames,colNames,valStrOV);
	  fflush(stdout);*/
    ii = srbModifyExtMetaData (Sentries->conn, MDAS_CATALOG,trgObj,trgColl,tableName,
			      colNames,valStrOV,"","",D_DELETE_FROM_EXTMD_TABLE);
 
    if (ii < 0) {
      printf ("Metadata Ingestion Error: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	      ii,ii);
      srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      bRMap(2,0);
      printf("</body></html>");
      return;
    }
    i = i +  numOfCols - 1;
  }
  printf("Done\n<BR>"); fflush(stdout);
  bRMap(2,1);
  printf("</body></html>");
  return;
}


int
modifyExtSchmMetaData(inStruct Sentries){

  char *trgColl, *trgObj, *colNames, *tableName;
  int numOfCols;
  int i,j,k,ii;
  char valStrUV[HUGE_STRING]; /*updated values */
  char valStrOV[HUGE_STRING]; /* old values */
  char valStrUA[3000];  /* updated attributes */
  char *a[SMALL_TOKEN];
  char iC[3000];

  printf("%s...\n",aaa[32]);fflush(stdout);
  trgObj = Sentries->entries[1].val;
  trgColl = Sentries->entries[2].val;
  tableName = Sentries->entries[5].val;
  colNames = Sentries->entries[6].val;
  numOfCols = atoi(Sentries->entries[4].val);
  i =  0; 
  strcpy(iC,colNames);
  a[0] = iC;
  j = 1;
  while(iC[i] != '\0') {
    if (iC[i] == '|') {
      iC[i] = '\0'; 
      a[j] = &iC[i+1] ;
      j++;
    }
    i++;
  }
  /*
  for (i = 0; i <= Sentries->m; i++)
    printf("%i:%s=%s<BR>",i,Sentries->entries[i].name,Sentries->entries[i].val);
  fflush(stdout);
  */

  for (i = 7; i <= Sentries->m ; i++ ) {
    if (strstr(Sentries->entries[i].name,"ckbxwqzt") != Sentries->entries[i].name ||
	strcmp(Sentries->entries[i].val,"on"))
      continue;
    /*
    printf("<BR>PPP%sBB%sBB%sBB%sBB%sBB%s<BR>\n",Sentries->entries[i+1].val,Sentries->entries[i+2].val,Sentries->entries[i+3].val,Sentries->entries[i+4].val,Sentries->entries[i+5].val,Sentries->entries[i+6].val);
    fflush(stdout);
    */
    valStrUV[0] ='\0';
    valStrOV[0] ='\0';
    valStrUA[0] ='\0';
    for (j = 0; j < numOfCols ; j++) {
      if (Sentries->entries[i + 2*j + 1].val[0] == 'Q' && 
	  Sentries->entries[i + 2*j + 1].val[strlen(Sentries->entries[i + 2*j + 1].val)-1] == 'Q') {
	Sentries->entries[i + 2*j + 1].val[0] = '\'';
	Sentries->entries[i + 2*j + 1].val[strlen(Sentries->entries[i + 2*j + 1].val)-1] = '\'';
      }
      if (strcmp(Sentries->entries[i + 2*j + 1].val,Sentries->entries[i + 2*j + 2].val)) {/* updated */
	if (strlen(valStrUA)> 0) {
	  strcat(valStrUA,"|");
	  strcat(valStrUV,"|");
	}
	strcat(valStrUA,a[j]);
	strcat(valStrUV,Sentries->entries[i + 2*j + 2].val);
      }
      if (j > 0)
	strcat(valStrOV,"|");
      strcat(valStrOV," = ");
      strcat(valStrOV,Sentries->entries[i + 2*j + 1].val);
    }

    /*   printf("<BR>PPPP:Before srbModifyExtMetaData o=%s c=%s t=%s<br> cc=%s<br>UA=%s<br>UV=%s<br>OA=%s<br>OV=%s<br>\n",trgObj,trgColl,tableName,colNames,valStrUA,valStrUV,colNames,valStrOV);
	 fflush(stdout); */
    ii = srbModifyExtMetaData (Sentries->conn, MDAS_CATALOG,trgObj,trgColl,tableName,
			      valStrUA,valStrUV,colNames,valStrOV,D_MODIFY_IN_EXTMD_TABLE);
 
    if (ii < 0) {
      printf ("Metadata Ingestion Error: <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",
	      ii,ii);
      srb_perror (2, ii, "", SRB_RCMD_ACTION|SRB_LONG_MSG);
      bRMap(2,0);
      printf("</body></html>");
      return;
    }
    i = i +  numOfCols - 1;
  }
  printf("Done\n<BR>"); fflush(stdout);
  bRMap(2,1);
  printf("</body></html>");
  return;
}


int
modifyDataMeta(inStruct Sentries)
{
  int i,j;
  j = 0;
  printf("%s...\n",aaa[32]);fflush(stdout);

  for (i = 1; i <= Sentries->m ; i++ ) {
    if (strstr(Sentries->entries[i].name,"metacb") != Sentries->entries[i].name)
      continue;
    if (strlen(Sentries->entries[i+2].val) > 0) {
      sprintf(tmpoStr,"0@%s",Sentries->entries[i+1].val);
      j = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
			   Sentries->entries[i-2].val, Sentries->entries[i-1].val,
			   "","", tmpoStr,Sentries->entries[i+2].val,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
      if (j != 0) break;
      sprintf(tmpoStr,"1@%s",Sentries->entries[i+1].val);
      j = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
			   Sentries->entries[i-2].val, Sentries->entries[i-1].val,
			   "","", tmpoStr,Sentries->entries[i+3].val,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
      if (j != 0) break;
      sprintf(tmpoStr,"2@%s",Sentries->entries[i+1].val);
      j = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
			   Sentries->entries[i-2].val, Sentries->entries[i-1].val,
			   "","", tmpoStr,Sentries->entries[i+4].val,
			     D_CHANGE_USER_DEFINED_STRING_META_DATA);
      if (j != 0) break;
      
    }
    else {
            j = srbModifyDataset(Sentries->conn, MDAS_CATALOG, 
			   Sentries->entries[i-2].val, Sentries->entries[i-1].val,
			   "","", Sentries->entries[i+1].val,"",
			     D_DELETE_USER_DEFINED_STRING_META_DATA);
	    if (j != 0) break;
    }
    i +=4;
  }
    if (j != 0) 
      printf("<br>Error: In Updating Metadat %s=%s; <A href=\"srberror3.cgi?%i\">Error: %i</A>\n",Sentries->entries[i+2].val,Sentries->entries[i+3].val, j,j);
    else 
      printf("Done");
    bRMap(2,0);
    return(0);  
}

int
prtMetaData (inStruct Sentries, mdasC_sql_result_struct *myresult, int updateFlag)
{
  int i,j;
  char *meta0, *meta1, *meta2,*dataList, *metanum, *collList;
  char *valPtr;
  char newVal[2*MAX_DATA_SIZE];
  if (first) {
    first = 0; 
    Sentries->globalCounter = 1;
    strcpy(tmpDataName,"");
    strcpy(tmpCollName,"");
    if ( updateFlag == 1) {
      fprintf (stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" >\n\
<input type=hidden name=\"function\" value=\"modifymeta\">\n", Sentries->frameStr);
      fprintf (stdout,"<TABLE><TR><TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH></TR>\n", mySrbStrings[211], mySrbStrings[77], mySrbStrings[78], mySrbStrings[62]);
    }

  }
  meta0 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[UDSMD0], dcs_aname[UDSMD0]);
  meta1 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD1], dcs_aname[UDSMD1]);
  meta2 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD1], dcs_aname[UDSMD2]);
  metanum =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[METADATA_NUM], dcs_aname[METADATA_NUM]);
  dataList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
  collList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
fprintf (stdout,"<PRE>\n");
  for (i = 0; i < myresult->row_count; i++) {
    if (strcmp(dataList,tmpDataName) || strcmp(collList,tmpCollName)) {
      if (!showAllMetaFlag) 
	fprintf (stdout,"<P><FONT COLOR=#0000FF>%s/%s:</FONT>\n",collList,dataList);
      strcpy(tmpDataName,dataList);
      strcpy(tmpCollName,collList);
    }
    /***
    fprintf (stdout,"<BR><FONT COLOR=#FF0000>%s</FONT>: %s <FONT COLOR=#0000FF>%s</FONT>\n",meta0,meta1,meta2);
    ***/
    valPtr = meta1;
    if (!strcmp(meta2,"srbObject")) {
      clSplitbychar (meta1, urlcollList, urldataList, '/');
      escape_url(urlcollList);
      escape_url(urldataList);
      sprintf(newVal,"<A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=0&owner=&session=%s\">%s</A>",urldataList,urlcollList,Sentries->pidStr,meta1);
      valPtr = newVal;
    }
    else if (!strcmp(meta2,"srbImage")) {
      clSplitbychar (meta1, urlcollList, urldataList, '/');
      escape_url(urlcollList);
      escape_url(urldataList);
      sprintf(newVal,"<IMG SRC=\"mysrb331.cgi?function=data&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=0&owner=&session=%s\">",urldataList,urlcollList,Sentries->pidStr);
      valPtr = newVal;
    }

    if (updateFlag == 0) {
      if (strstr(meta0,"DC.") == meta0) {
	fprintf (stdout,"<BR><FONT COLOR=#FF0000>%s</FONT>.<FONT COLOR=#0000FF>%s</FONT>: %s\n",meta0,meta2,valPtr);
      }
      else {
	fprintf (stdout,"<BR><FONT COLOR=#FF0000>%s</FONT>: %s <FONT COLOR=#0000FF>%s</FONT>\n",meta0,valPtr,meta2);
      }
    }
    else { /**  updateFlag == 1  */
      simple_escape_url(meta2);
      simple_escape_url(meta1);
      simple_escape_url(meta0);
      fprintf(stdout,"<input type=hidden name=\"data\" value=\"%s\">\n",dataList);
      fprintf(stdout,"<input type=hidden name=\"coll\" value=\"%s\">\n",collList);
      fprintf(stdout,"<TR><TD><INPUT TYPE=CHECKBOX NAME=\"metacb%i\">\
<input type=hidden name=\"metanum%i\" VALUE=\"%s\" >\
<TD><INPUT TYPE=TEXT NAME=\"metaname%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=TEXT NAME=\"metaval%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=TEXT NAME=\"metaunit%i\" VALUE=\"%s\" ></TD></TR>\n",
	      Sentries->globalCounter,
	     Sentries->globalCounter,metanum, Sentries->globalCounter,meta0, Sentries->globalCounter,meta1, Sentries->globalCounter,meta2);
    }
    Sentries->globalCounter++;
    meta0 += MAX_DATA_SIZE;
    meta1 += MAX_DATA_SIZE;
    meta2 += MAX_DATA_SIZE;
    metanum += MAX_DATA_SIZE;
    dataList += MAX_DATA_SIZE;
    collList += MAX_DATA_SIZE;
  }
fprintf (stdout,"</PRE>\n");
}



int
prtMetaDataNew(inStruct Sentries, mdasC_sql_result_struct *myresult, int updateFlag)
{
  int i,j;
  char *meta0, *meta1, *meta2,*dataList, *metanum, *collList;
  char *valPtr;
  char newVal[2*MAX_DATA_SIZE];
  if (first) {
    first = 0; 
    Sentries->globalCounter = 1;
    strcpy(Sentries->tmpDataName,"");
    strcpy(Sentries->tmpCollName,"");
    if ( updateFlag == 1) {
      fprintf (stdout,"<FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"%s\" >\n\
<input type=hidden name=\"function\" value=\"modifymeta\">\n", Sentries->frameStr);
      fprintf (stdout,"<TABLE><TR><TH>%s</TH><TH>%s</TH><TH>%s</TH><TH>%s</TH></TR>\n", mySrbStrings[211], mySrbStrings[77], mySrbStrings[78], mySrbStrings[62]);
    }

  }
  meta0 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
         dcs_tname[UDSMD0], dcs_aname[UDSMD0]);
  meta1 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD1], dcs_aname[UDSMD1]);
  meta2 =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[UDSMD1], dcs_aname[UDSMD2]);
  metanum =  (char *) getFromResultStruct(
      (mdasC_sql_result_struct *) myresult,
        dcs_tname[METADATA_NUM], dcs_aname[METADATA_NUM]);
  dataList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_NAME], dcs_aname[DATA_NAME]);
  collList = (char *) getFromResultStruct(
          (mdasC_sql_result_struct *) myresult,
            dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);

  /****************** MAIN CONTENTS OF SCREEN *********************/
  fprintf (stdout,"<font color=000088><b>%s \
<i> %s </i></b></font>\n\
<table width=100%% border=0 cellspacing=4 cellpadding=0>\n\
<tr><td width=5%%>&nbsp;</td>\
<td><i><font color=000088>%s:&nbsp;&nbsp;\
<b> %s</b> <font color=red>%s</font> &lt; %s &gt;</td></tr>\n", mySrbStrings[57], dataList, mySrbStrings[60], mySrbStrings[61], mySrbStrings[62], mySrbStrings[63]);
  for (i = 0; i < myresult->row_count; i++) {
    if (strcmp(dataList, Sentries->tmpDataName) || strcmp(collList, Sentries->tmpCollName)) {
      /*      if (!showAllMetaFlag) 
	      fprintf (stdout,"<P><FONT COLOR=#0000FF>%s/%s:</FONT>\n",collList,dataList);*/
      strcpy(Sentries->tmpDataName,dataList);
      strcpy(Sentries->tmpCollName,collList);
    }

    valPtr = meta1;
    if (!strcmp(meta2,"srbObject")) {
      clSplitbychar (meta1, Sentries->urlcollList, Sentries->urldataList, '/');
      escape_url(Sentries->urlcollList);
      escape_url(Sentries->urldataList);
      sprintf(newVal,"<A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=0&owner=&session=%s\">%s</A>", Sentries->urldataList, Sentries->urlcollList, Sentries->pidStr, meta1);
      valPtr = newVal;
    }
    else if (!strcmp(meta2,"srbImage")) {
      clSplitbychar (meta1, Sentries->urlcollList, Sentries->urldataList, '/');
      escape_url(Sentries->urlcollList);
      escape_url(Sentries->urldataList);
      sprintf(newVal,"<IMG SRC=\"mysrb331.cgi?function=data&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=0&owner=&session=%s\">", Sentries->urldataList, Sentries->urlcollList, Sentries->pidStr);
      valPtr = newVal;
    }

    if (updateFlag == 0) {
      /***
      if (strstr(meta0,"DC.") == meta0) {
	fprintf (stdout,"<BR><FONT COLOR=#FF0000>%s</FONT>.<FONT COLOR=#0000FF>%s</FONT>: %s\n",meta0,meta2,valPtr);
      }
      else {
	fprintf (stdout,"<BR><FONT COLOR=#FF0000>%s</FONT>: %s <FONT COLOR=#0000FF>%s</FONT>\n",meta0,valPtr,meta2);
      }
      ***/
      fprintf (stdout,"<tr><td>&nbsp;</td><td>%s :&nbsp;&nbsp;<b>%s</b> <font color=red>%s</font>\n",meta0,valPtr,meta2);
    }
    else { /**  updateFlag == 1  */
      simple_escape_url(meta2);
      simple_escape_url(meta1);
      simple_escape_url(meta0);
      fprintf(stdout,"<input type=hidden name=\"data\" value=\"%s\">\n",dataList);
      fprintf(stdout,"<input type=hidden name=\"coll\" value=\"%s\">\n",collList);
      fprintf(stdout,"<TR><TD><INPUT TYPE=CHECKBOX NAME=\"metacb%i\">\
<input type=hidden name=\"metanum%i\" VALUE=\"%s\" >\
<TD><INPUT TYPE=TEXT NAME=\"metaname%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=TEXT NAME=\"metaval%i\" VALUE=\"%s\" ></TD>\
<TD><INPUT TYPE=TEXT NAME=\"metaunit%i\" VALUE=\"%s\" ></TD></TR>\n",
	      Sentries->globalCounter, Sentries->globalCounter, metanum, Sentries->globalCounter, meta0, Sentries->globalCounter, meta1, Sentries->globalCounter, meta2);
    }
    Sentries->globalCounter++;
    meta0 += MAX_DATA_SIZE;
    meta1 += MAX_DATA_SIZE;
    meta2 += MAX_DATA_SIZE;
    metanum += MAX_DATA_SIZE;
    dataList += MAX_DATA_SIZE;
    collList += MAX_DATA_SIZE;
  }
 fprintf (stdout,"</table>\n");

}

int getMode(char *inMode) {
//    char *tmpPtr;
//    
///* must remove blanks at the end */
//    if (tmpPtr=strstr(inMode," ")) {
//        *tmpPtr='\0';
//    }
    
    strcpy(mode, inMode);
 
    return 0;
}

/* the following fonction is not used now that we have exploration tree */ 
int goUpMenu(inStruct Sentries, char *collection) {
    char *tmpPtr, *collName = strdup(collection);
    int i = 1, selected = 1;

 
    printf("<TABLE><TR><FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\"  target=\"_top\" >");
    printf("<input type=hidden name=\"function\" value=\"full\">");
    printf("<input type=hidden name=\"dataname\" value=\"\">");
    printf("<TD>%s</TD>", mySrbStrings[242]);
    printf("<TD><SELECT NAME=\"collection\">");
    
    tmpPtr = collName+strlen(collName);
    
    while(i) {
        
        while(tmpPtr[0] != '/') {
            tmpPtr--;
        }
        
        if (tmpPtr == collName) {
            tmpPtr[1]='\0';
            i=0; 
        }
        
        else {
            tmpPtr[0]='\0';
        }
        
        if (selected) {
            printf("<OPTION SELECTED>%s", collName);
            selected=0;
        }
        
        else {
            printf("<OPTION>%s", collName);
        }
        
        fflush(stdout);
        
    }
    
   printf("</SELECT></TD>");
   printf("<TD><INPUT TYPE=\"submit\" VALUE=\"%s\">", mySrbStrings[243]);   
   printf("<INPUT type=hidden NAME=\"seek\", VALUE= \"0\">");
   printf("<INPUT type=hidden NAME=\"copynum\" value=\"0\">");
   printf("<INPUT type=hidden NAME=\"datatype\" VALUE=\"\">");
   printf("<INPUT type=hidden NAME=\"vernum\" VALUE=\"1\">");
   printf("<INPUT type=hidden NAME=\"owner\" VALUE=\"%s\">", Sentries->entries[7].val);
   printf("<INPUT type=hidden NAME=\"session\" value=\"%s\">", Sentries->pidStr);  
   printf("</TD></FORM></TR></TABLE>");

}

/* the following fonction is not used now that we have exploration tree */ 
int makeJumpToPage(inStruct Sentries) {

    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html><head>");
    if (!strcmp(mode,"light")) {
        printf("<link href=\"%s/original.css\" rel=\"stylesheet\" type=\"text/css\">", webPath);
    }
    printf("<title>%s</title>", mySrbStrings[245]);
    printf("<base target = \"_top\"></head>");
    if (!strcmp(mode,"light")) {
        printf("<body bgcolor=\"#FFFFF7\">");
    }
    else {
        printf("<body background=\"%s/srb3.jpg\">", webPath);
    }
    
    printf("<TABLE><TR><TD><FORM METHOD=\"POST\" ACTION=\"mysrb331.cgi\" target=\"_top\">\n");
    printf("<input type=hidden name=\"function\" value=\"full\">");
    printf("<input type=hidden name=\"dataname\" value=\"\">");
    getAccsCollectionsAsSelect(Sentries, "all", NULL);                      /*   7 **/
    printf("<STRONG><FONT COLOR=#0000FF>%s </FONT></STRONG>%s</TD></TR>\n", mySrbStrings[246], Sentries->selectData);
    printf("<TR><TD><BR></TD></TR><TR align=\"center\"><TD><INPUT TYPE=\"submit\" VALUE=\"%s\">\n", mySrbStrings[243]);
    printf("<INPUT type=hidden NAME=\"seek\", VALUE= \"0\">");
    printf("<INPUT type=hidden NAME=\"copynum\" value=\"0\">");
    printf("<INPUT type=hidden NAME=\"datatype\" VALUE=\"\">");
    printf("<INPUT type=hidden NAME=\"vernum\" VALUE=\"1\">");
    printf("<INPUT type=hidden NAME=\"owner\" VALUE=\"\">");
    printf("<INPUT type=hidden NAME=\"session\" value=\"%s\">", Sentries->pidStr); 
    printf("</FORM></TD></TR></TABLE></body></html>");
    clFinish(Sentries->conn);
    
    return 0;
}


int makeTreePage(inStruct Sentries) {
    
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html><head><title>SRB Exploration Tree</title>");
    printf("<style>\n\
body { font: 10pt Tahoma, Arial, sans-serif;  color: navy; }\n\
.trigger { cursor: pointer;  cursor: hand; }\n\
.branch { display: none;  margin-left: 16px; }\n\
a:link { color: #000000; text-decoration: none }\n\
a:visited { color: #000000; text-decoration: none }\n\
a:hover { color: #0000FF; text-decoration: underline }\n\
a:active { color: #0000FF; text-decoration: none }\n\
</style>\n");

    printf("<script language=\"JavaScript\">\n\
var openImg = new Image();\n\
openImg.src = \"%s/open.gif\";\n\
var closedImg = new Image();\n\
closedImg.src = \"%s/closed.gif\";\n", webPath, webPath);

    printf("\n\
var openNode = new Image();\n\
openNode.src = \"%s/openNode.gif\";\n\
var closedNode = new Image();\n\
closedNode.src = \"%s/closedNode.gif\";\n\n", webPath, webPath);

    printf("function showBranch(branch){\n\
	var objBranch = document.getElementById(branch).style;\n\
	if(objBranch.display==\"block\")\n\
		objBranch.display=\"none\";\n\
	else\n\
		objBranch.display=\"block\";\n\
}\n\
function swapNfolder(img){\n\
	objImg = document.getElementById(img);\n\
	if(objImg.src.indexOf('closedNode.gif')>-1)\n\
		objImg.src = openNode.src;\n\
	else\n\
		objImg.src = closedNode.src;\n\
}\n\
function swapFolder(img){\n\
	objImg = document.getElementById(img);\n\
	if(objImg.src.indexOf('closed.gif')>-1)\n\
		objImg.src = openImg.src;\n\
	else\n\
		objImg.src = closedImg.src;\n\
}\n\
</script>\n");
    printf("</head>\n"); 
 
    if (!strcmp(mode,"light")) {
        printf("<body bgcolor=\"#FFFFF7\">\n");
    }
    else {
        printf("<body background=\"%s/srb3.jpg\">\n", webPath);
    }    
    if ( Sentries->currentMcatZone != NULL) {
       if ( Sentries->oldMcatZone == NULL)
           reputConnInfo (Sentries);
       else if (strcmp(Sentries->currentMcatZone,Sentries->oldMcatZone))
           reputConnInfo (Sentries);
    }
    printf("<table>");
    
    printf("<tr><td><FONT SIZE=-1>");   
    
    printf("<span><font color=000088><h3>%s</h3></font></span>", mySrbStrings[248]);
    printZoneInfo(Sentries);
    
    printf("</FONT></td></tr><tr><td><FONT SIZE=-1>");   
    
    printAccsCollectionsAsTree(Sentries, "all", "shared");
    printf("<HR>");
   
    printf("</FONT></td></tr><tr><td><FONT SIZE=-1>");  
    
    printAccsCollectionsAsTree(Sentries, "all", "local");    
    
    printf("</FONT></td></tr></table>"); 
    
    printf("</body></html>");  

}


printZoneInfo(inStruct Sentries)
{
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int status, i,j, k;
    char *zName, *zLocality, *zLocation, *zNetPrefix, *tmpPtr;
    char *tmpzName, *tmpzLocality;
    char *mcatHome = malloc(MAX_TOKEN);
    char *tmpMcatZone;
    char tmpStr[MAX_TOKEN];
    
   i = 0;
   mcatHome[0] = '\0';
    i = srbGetMcatZone(Sentries->conn,
			 Sentries->srbUser, Sentries->userDomain,mcatHome);
    if (i >= 0) {
      fprintf(stdout,"<NOBR><B>%s</B><A HREF=\"mysrb331.cgi?function=treefull&dataname=%s&collection=/%s&seek=0&copynum=0&datatype=&vernum=1&owner=&session=%s\"  target=\"%s\">%s</A><BR>\n", mySrbStrings[249], "", mcatHome,  Sentries->pidStr, Sentries->pidStr, mcatHome);    fflush(stdout);
    }
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[ZONE_NAME] = 1;
   selval[ZONE_LOCALITY] = 1;
   selval[ZONE_LOCN_DESC] = 1;
   selval[ZONE_NETPREFIX] = 1;
   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0, qval, selval, &myResult, MAX_RESULT_NUM);
   if (status != 0) {
     return(-1);
   }
   zName = (char *) getFromResultStruct(
       &myResult, dcs_tname[ZONE_NAME], dcs_aname[ZONE_NAME]);
   zLocality = (char *) getFromResultStruct(
       &myResult, dcs_tname[ZONE_LOCALITY], dcs_aname[ZONE_LOCALITY]);
   zLocation = (char *) getFromResultStruct(
       &myResult, dcs_tname[ZONE_LOCN_DESC], dcs_aname[ZONE_LOCN_DESC]);
   zNetPrefix = (char *) getFromResultStruct(
       &myResult, dcs_tname[ZONE_NETPREFIX], dcs_aname[ZONE_NETPREFIX]);

   tmpzName = zName;
   tmpzLocality = zLocality;
   for(i=0; i < myResult.row_count; i++) {
     if (atoi(tmpzLocality) == 1) {
       break;
     }
      tmpzName += MAX_DATA_SIZE;
      tmpzLocality += MAX_DATA_SIZE;
   }
   
       fprintf(stdout,"<B>%s</B><A HREF=\"mysrb331.cgi?function=treefull&dataname=%s&collection=/%s&seek=0&copynum=0&datatype=&vernum=1&owner=&session=%s\"  target=\"%s\">%s</A>\n", mySrbStrings[250],"", Sentries->userConnectionZone,  Sentries->pidStr, Sentries->pidStr, Sentries->userConnectionZone);
       fflush(stdout);
   if (Sentries->currentMcatZone != NULL) {
     fprintf(stdout,"<BR><B>%s</B><A HREF=\"mysrb331.cgi?function=treefull&dataname=%s&collection=/%s&seek=0&copynum=0&datatype=&vernum=1&owner=&session=%s\"  target=\"%s\">%s</A>\n", mySrbStrings[260],"", Sentries->currentMcatZone,  Sentries->pidStr, Sentries->pidStr, Sentries->currentMcatZone);
   }
   else {
     fprintf(stdout,"<BR><B>%s</B><A HREF=\"mysrb331.cgi?function=treefull&dataname=%s&collection=/%s&seek=0&copynum=0&datatype=&vernum=1&owner=&session=%s\"  target=\"%s\">%s</A>\n", mySrbStrings[260],"", tmpzName,  Sentries->pidStr, Sentries->pidStr, tmpzName);
     Sentries->currentMcatZone = strdup(tmpzName);
   }
   fprintf(stdout,"<BR><HR><B>%s</B>\n",mySrbStrings[251]);fflush(stdout);
   fprintf(stdout,"<BR>\n");
   
   j = -1;
   for (i=0; i < myResult.row_count; i++) {
    
/*    fprintf(stdout,"&nbsp;&nbsp;<A HREF=\"mysrb331.cgi?function=treefull&dataname=%s&collection=/%s&seek=0&copynum=0&datatype=&vernum=1&owner=&session=%s\"  target=\"sub%s\"><IMG SRC=\"%s/world.gif\" width=18 height=18 border=0> %s </A>\n", "", zName,  Sentries->pidStr, Sentries->pidStr, webPath,zName);  */

     if ((tmpPtr = strstr(zNetPrefix,":")) != NULL)
       *tmpPtr = '\0';  /* to clean comment string in link below */

     strcpy(tmpoStr,"");
     
     k = nID++;
     
     sprintf(tmpoStr, "<span class=\"trigger\" onClick=\"showBranch('branch%d');swapNfolder('folder%d')\">\
<img align=top border=0 hspace=3 vspace=3 src=\"%s/closedNode.gif\" ALT=\"%s=%s;%s=%s\" id=\"folder%d\"></span>\
<A HREF=\"mysrb331.cgi?function=treefull&dataname=%s&collection=/%s&seek=0&copynum=0&datatype=&vernum=1&owner=&session=%s\"  target=\"%s\">%s</A>\n\
<BR><span class=\"branch\" id=\"branch%d\">", 
k, k, webPath, mySrbStrings[252], zLocation, mySrbStrings[253], zNetPrefix, k,
 "", zName,  Sentries->pidStr, Sentries->pidStr, zName, k);
    
/*
     sprintf (tmpoStr + strlen(tmpoStr),"&nbsp;&nbsp;&nbsp;<IMG SRC=\"%s/commentsbut.gif\" ALT=\"%s=%s;%s=%s\" width=18 height=18 border=0></A>\n",
	      webPath,mySrbStrings[252],zLocation, mySrbStrings[253],zNetPrefix );fflush(stdout);
*/

#ifdef PORTNAME_solaris
	sprintf(tmpStr,"ping %s 3 > %s/test.%i", zNetPrefix,logDirec,i);
#endif

#ifdef PORTNAME_linux
	sprintf(tmpStr,"ping -c 1 -w 3 %s > %s/test.%i", zNetPrefix,logDirec,i);
#endif

     j=9999;
     if (strlen(zNetPrefix) > 0) {
       j = system(tmpStr);
	}
     else
       fprintf(stdout,"<span class=\"trigger\" onClick=\"showBranch('branch%d');swapNfolder('folder%d')\">\
<IMG align=top border=0 hspace=3 vspace=3 SRC=\"%s/closedNode.gif\" id=\"folder%d\"></span>\
<FONT COLOR=#CC0000>%s</FONT><BR><span class=\"branch\" id=\"branch%d\">&nbsp;<FONT COLOR=#CC0000>%s</FONT>\n",
       k, k, webPath, k, zName, k, mySrbStrings[254]);

     if (j == 0) {
       tmpMcatZone = Sentries->currentMcatZone;
       Sentries->currentMcatZone = zName;
       j = printValuesAsTree(Sentries,"Resources");
       if (j < 0) {        
       fprintf(stdout,"<span class=\"trigger\" onClick=\"showBranch('branch%d');swapNfolder('folder%d')\">\
<IMG  align=top border=0 hspace=3 vspace=3 SRC=\"%s/closedNode.gif\" id=\"folder%d\"></span>\
<FONT COLOR=#CC0000>%s</FONT><BR><span class=\"branch\" id=\"branch%d\">",
       k, k, webPath, k, zName, k);
	 if (j == SVR_TO_SVR_CONNECT_ERROR) {
	   fprintf(stdout,"&nbsp;<FONT COLOR=#CC0000>%s</FONT>",mySrbStrings[257]);
	   }
	 else {
	   fprintf(stdout,"&nbsp;<FONT COLOR=#CC0000>%s:%i</FONT>",mySrbStrings[258],j);
	   }
     }
     else
	 /*printValuesAsTree(Sentries,mySrbStrings[256]);*/
	 printValuesAsTree(Sentries,"Users");
       Sentries->currentMcatZone = tmpMcatZone;
     }
     else if (j != 9999) {
       fprintf(stdout,"<span class=\"trigger\" onClick=\"showBranch('branch%d');swapNfolder('folder%d')\">\
<IMG  align=top border=0 hspace=3 vspace=3 SRC=\"%s/closedNode.gif\" id=\"folder%d\"></span>\
<FONT COLOR=#CC0000>%s</FONT><BR><span class=\"branch\" id=\"branch%d\">&nbsp;<FONT COLOR=#CC0000>%s</FONT>\n",
       k, k, webPath, k, zName, k, mySrbStrings[262]);
     }
      
      fprintf (stdout,"</span>\n");
      
      zName += MAX_DATA_SIZE;
      zLocality += MAX_DATA_SIZE;
      zLocation += MAX_DATA_SIZE;
      zNetPrefix += MAX_DATA_SIZE;
   }
   fprintf (stdout,"</NOBR>\n");fflush(stdout);

}



/* printAccsCollectionsAsTree generates the whole collection hierarchy
   from one single MCAT query. Not suitable for large collection trees. 
   Not used in current version */    
printAccsCollectionsAsTree(inStruct Sentries, char *accs, char *collType)
{
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int j = nID;
    int i=0, nbopen=0, toClose=0;
    int status, subSize;
    char *tmpList, *origTmpList, *ownerList, *domainList, *tmpPtr, *fixPtr;
    char *motherColl = malloc(MAX_TOKEN);
    char *collection = malloc(MAX_TOKEN), *currentColl;
    char *toPrint = malloc(MAX_TOKEN);
    char *collPath = malloc(MAX_TOKEN);
    char *linkColl = malloc(MAX_TOKEN);

  
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   myResult.row_count = 0;
   
   if (!strcmp(collType, "shared")) {
        fprintf(stdout,"<B>%s</B><BR><NOBR>",mySrbStrings[259]);
   }
   else if (!strcmp(collType, "local")) {
        fprintf(stdout,"<B>%s</B><BR><NOBR>",mySrbStrings[264]);
   }

/************ old settings *********************************
   selval[ACCESS_COLLECTION_NAME] = 1;
   sprintf(qval[COLLECTION_ACCESS_CONSTRAINT]," = '%s' ",accs); 
   sprintf(qval[COLLECTION_ACCESS_CONSTRAINT]," like '%%%%' "); 
   sprintf(qval[USER_NAME], " = '%s'", Sentries->srbUser);  
   sprintf(qval[DOMAIN_DESC], " = '%s'", Sentries->userDomain);   
   if (Sentries->currentMcatZone != NULL &&
       strcmp(Sentries->currentMcatZone,Sentries->userConnectionZone) )
     sprintf(qval[ACCESS_COLLECTION_NAME], " like '/%s/%%' ",Sentries->currentMcatZone);
/************************************************************/


    selval[DATA_GRP_NAME] = 1;
    selval[COLL_OWNER_NAME] = 1;    
    selval[COLL_OWNER_DOMAIN] = 1;
    
    if (!strcmp(collType, "shared")) {
    	sprintf(qval[DATA_GRP_NAME], " like '/%s/%%' ", Sentries->currentMcatZone);
    }
    else if (!strcmp(collType, "local")) {
    	sprintf(qval[DATA_GRP_NAME], " like '/home/%%' ");
    }

   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0, qval, selval, &myResult, MAX_RESULT_NUM * 10);
   if (status < 0){
     return(status);
   }

   tmpList = (char *) getFromResultStruct(&myResult, dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
   ownerList = (char *) getFromResultStruct(&myResult, dcs_tname[COLL_OWNER_NAME], dcs_aname[COLL_OWNER_NAME]);
   domainList = (char *) getFromResultStruct(&myResult, dcs_tname[COLL_OWNER_DOMAIN], dcs_aname[COLL_OWNER_DOMAIN]);

   origTmpList = tmpList;
  
   strcpy(motherColl, "");
   for (i=0; i < myResult.row_count; i++) {

      if (!strstr(tmpList,"$deleted"))  {
    
    /*** to prevent problems from possible blank named collections ***/
            if (strlen(tmpList) > 1 && tmpList[strlen(tmpList)-1] == '/' ) {
                sprintf(collection, "%s$no_name", tmpList);
            
            }
            else if (strlen(tmpList) > 1 && (fixPtr = strstr(tmpList, "//")) ) {
                fixPtr[0] = '\0';
                sprintf(collection, "%s/$no_name%s", tmpList, fixPtr+1);
                }
            
                else {
                  strcpy(collection, tmpList);    
            }            
     /*********************/       
      
          strcat(motherColl, "/");

          currentColl = collection;
          tmpPtr = motherColl+strlen(motherColl)-1;
          
          while (!strstr(currentColl, motherColl)) {
          /* in this loop we find the full path of new mother collection */

                tmpPtr--;
                while (tmpPtr[0] != '/') {
                    tmpPtr--;
                }
                tmpPtr[1]='\0';
    
                toClose++;

          }

          for ( ; toClose>0; toClose--) {
            if (nbopen>0) {
                fprintf(stdout, "</span>");fflush(stdout);
                nbopen--;
                }       
          }
            
          /* collPath contains full collection name for link access */
          strcpy(collPath, motherColl);
          tmpPtr = currentColl;
          currentColl = currentColl + strlen(motherColl);
          strcpy(motherColl, tmpPtr);
  
          while (subSize=strcspn(currentColl,"/")) {
            /* if the new collection includes subcollections we must break it 
               into single collections and print each of them */
   
              strcpy(toPrint, currentColl);
              toPrint[subSize] = '\0';
          
              if (collPath[strlen(collPath)-1] != '/') {
                strcat(collPath, "/");
              }

              /* must not go past the end of collection name */
              if (strstr(currentColl, "/")) {
                currentColl = currentColl+subSize+1;
              }
              else {
                currentColl = currentColl+subSize;
              }
     
              j++;

              sprintf(buf, "<span class=\"trigger\" onClick=\"showBranch('branch%d');swapFolder('folder%d')\">\n<img align=\"top\" src=\"%s/closed.gif\" border=\"0\" id=\"folder%d\">\n", i+j, i+j, webPath, i+j);
              fprintf(stdout, "%s", buf);fflush(stdout);

              /* collection link */
              if (strcmp(toPrint, "$no_name") ) {
                  sprintf(linkColl, "%s%s", collPath, toPrint);
              } 
              else {
                  sprintf(linkColl, "%s", collPath);
              }
              
              escape_url(linkColl);              
    	      sprintf(buf, "</span><A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"sub%s\">%s </A>", "", linkColl, ownerList, domainList, Sentries->pidStr, Sentries->pidStr, toPrint);              
              
              
              fprintf(stdout, "%s", buf);
              fflush(stdout); 
              
              sprintf(buf, "<br>\n<span class=\"branch\" id=\"branch%d\">\n", i+j);              
              nbopen++;
              fprintf(stdout, "%s", buf);fflush(stdout);
                     
              /* We add current collection to the path in case the new collection included 
                 subcollections. if not then we leave the loop and it doesn't do any harm */ 
              sprintf(buf, "%s%s", collPath, toPrint);
              strncpy(collPath, buf, MAX_TOKEN);
              
              fflush(stdout);
          }
          
      }
      
      tmpList += MAX_DATA_SIZE;
      ownerList += MAX_DATA_SIZE;
   }

   while (myResult.continuation_index >= 0 && i+j <= 15000) {
    /*** need to limit the value of (i+j) to make sure the left frame doesn't
            get too big in the case of a very large collection tree.
            Will replace this with a clean indexing in future ***/

     clearSqlResult (&myResult);
     status = srbGetMoreRows(Sentries->conn, 0, myResult.continuation_index,
			     &myResult, MAX_RESULT_NUM);
      			     
	if (myResult.result_count == 0  || status != 0)
	  break;

    tmpList = (char *) getFromResultStruct(&myResult, dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    ownerList = (char *) getFromResultStruct(&myResult, dcs_tname[COLL_OWNER_NAME], dcs_aname[COLL_OWNER_NAME]);
    domainList = (char *) getFromResultStruct(&myResult, dcs_tname[COLL_OWNER_DOMAIN], dcs_aname[COLL_OWNER_DOMAIN]);
     
    /* to make sure each node still has a unique id even if i is set back to 0 */
     j+=i;

   for (i=0; i < myResult.row_count; i++) {

      if (!strstr(tmpList,"$deleted"))  {

    /*** to prevent problems from possible blank named collections ***/
            if (strlen(tmpList) > 1 && tmpList[strlen(tmpList)-1] == '/' ) {
                sprintf(collection, "%s$no_name", tmpList);
            
            }
            else if (strlen(tmpList) > 1 && (fixPtr = strstr(tmpList, "//")) ) {
                fixPtr[0] = '\0';
                sprintf(collection, "%s/$no_name%s", tmpList, fixPtr+1);
                }
            
                else {
                  strcpy(collection, tmpList);    
            }            
     /*********************/ 
       
          strcat(motherColl, "/");
          currentColl = collection;
          tmpPtr = motherColl+strlen(motherColl)-1;
          
          while (!strstr(currentColl, motherColl)) {
          /* in this loop we find the full path of new mother collection */
  
                tmpPtr--;
                while (tmpPtr[0] != '/') {
                    tmpPtr--;
                }
                tmpPtr[1]='\0';
    
                toClose++;

          }
 
          for ( ; toClose>0; toClose--) {
            if (nbopen>0) {
                fprintf(stdout, "</span>");
                nbopen--;
                }       
          }
         
          /* collPath contains full collection name for link access */
          strcpy(collPath, motherColl);
         
          tmpPtr = currentColl;
          currentColl = currentColl + strlen(motherColl);
          strcpy(motherColl, tmpPtr);
           
          while (subSize=strcspn(currentColl,"/")) {
            /* if the new collection includes subcollections we must break it 
               into single collections and print each of them */

              strcpy(toPrint, currentColl);
              toPrint[subSize] = '\0';
          
              if (collPath[strlen(collPath)-1] != '/') {
                strcat(collPath, "/");
              }

              /* must not go past the end of collection name */
              if (strstr(currentColl, "/")) {
                currentColl = currentColl+subSize+1;
              }
              else {
                currentColl = currentColl+subSize;
              }
      
              j++;

              sprintf(buf, "<span class=\"trigger\" onClick=\"showBranch('branch%d');swapFolder('folder%d')\">\n<img align=\"top\" src=\"%s/closed.gif\" border=\"0\" id=\"folder%d\">\n", i+j, i+j, webPath, i+j);
              fprintf(stdout, "%s", buf);

              /* collection link */
              if (strcmp(toPrint, "$no_name") ) {
                  sprintf(linkColl, "%s%s", collPath, toPrint);
              } 
              else {
                  sprintf(linkColl, "%s", collPath);
              }
              
              escape_url(linkColl);              
    	      sprintf(buf, "</span><A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"sub%s\">%s </A>", "", linkColl, ownerList, domainList, Sentries->pidStr, Sentries->pidStr, toPrint);              
 
              
              fprintf(stdout, "%s", buf);
              fflush(stdout); 
              
              sprintf(buf, "<br>\n<span class=\"branch\" id=\"branch%d\">\n", i+j);              
              nbopen++;
              fprintf(stdout, "%s", buf);fflush(stdout);
              
              /* We add current collection to the path in case the new collection included 
                 subcollections. if not then we leave the loop and it doesn't do any harm */ 
              sprintf(buf, "%s%s", collPath, toPrint);
              strncpy(collPath, buf, MAX_TOKEN);
              
              fflush(stdout);

          }
          
      }
     
      tmpList += MAX_DATA_SIZE;
      ownerList += MAX_DATA_SIZE;

   }
   } 

   
     for ( ; nbopen>0; nbopen--) {
        fprintf(stdout, "</span>");
     }
     
    fprintf(stdout, "</NOBR>");
    
    nID+=i+j;  /* to avoid node ID collisions in case this function is called again */
    
    //free(origTmpList);
    free(motherColl);
    free(toPrint);
    free(collection);
    free(collPath);
        
    return 0;
}



printValuesAsTree(inStruct Sentries, char *valType)
{
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int i, j=0;
    int status;
    char *tmpList, *origTmpList;
    char *tmpList2, *origTmpList2;
    char *iName;

    myResult.row_count = 0;

    for (i = 0; i < MAX_DCS_NUM; i++) {
      sprintf(qval[i],"");
      selval[i] = 0;
    }
    if (!strcmp(valType,"Resources")) {
      selval[RSRC_NAME] = 1;
      iName = mySrbStrings[255];
      j=nID;
      nID++;
    }
    else if (!strcmp(valType,"Users")) {
      selval[USER_NAME] = 1;
      selval[DOMAIN_DESC] = 1;
      iName = mySrbStrings[256];
      j=nID;
      nID++;
    }
    status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0, qval, selval, &myResult, MAX_RESULT_NUM);
    if (status < 0)
      return(status);
      
    /** at this point we can print zone name with enabled link **/
     fprintf(stdout,tmpoStr); 
     strcpy(tmpoStr,"");

    if (!strcmp(valType,"Resources")) {
      tmpList = (char *) getFromResultStruct(
          &myResult,
          dcs_tname[RSRC_NAME], dcs_aname[RSRC_NAME]);
    }
    else if (!strcmp(valType,"Users")) {
      tmpList = (char *) getFromResultStruct(
          &myResult,
	  dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
      tmpList2 = (char *) getFromResultStruct(
          &myResult,
	  dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
    }
    origTmpList = tmpList;
    origTmpList2 = tmpList2;
  if (!strcmp(valType,"Resources"))
    sprintf(buf, "<span class=\"trigger\" onClick=\"showBranch('branch%d');\">\n<img align=\"top\" src=\"%s/cd.gif\" border=\"0\"  width=18 height=18  id=\"folder%d\"> %s</span>\n", j,  webPath, j,iName);
  else if (!strcmp(valType,"Users"))
    sprintf(buf, "<span class=\"trigger\" onClick=\"showBranch('branch%d');\">\n<img align=\"top\" src=\"%s/user.gif\" border=\"0\"  width=18 height=18 id=\"folder%d\"> %s</span>\n", j,  webPath, j,iName);
    fprintf(stdout, "%s", buf);
    fprintf(stdout, "<br>\n<span class=\"branch\" id=\"branch%d\">\n", j); 
    fflush(stdout);

    for (i=0; i < myResult.row_count; i++) {
      if (!strcmp(valType,"Resources")) {
         if (!strstr(tmpList,"$deleted"))  { /** couldn't get filterDeleted() to do it **/
	        fprintf(stdout, "%s<BR>", tmpList);
	        }
	    tmpList += MAX_DATA_SIZE;
      }
      else if (!strcmp(valType,"Users")){
         if (!strstr(tmpList,"$deleted"))  { 
	        fprintf(stdout, "%s@%s<BR>", tmpList, tmpList2);
	        }
	    tmpList += MAX_DATA_SIZE;
	    tmpList2 += MAX_DATA_SIZE;
      }
    }

    while (myResult.continuation_index >= 0) {

      status = srbGetMoreRows(Sentries->conn, 0, myResult.continuation_index,
			     &myResult, MAX_RESULT_NUM);
      			     
      if (myResult.result_count == 0  || status != 0)
	break;
      
      if (!strcmp(valType,"Resources")) {
	tmpList = (char *) getFromResultStruct(
	       &myResult,
	       dcs_tname[RSRC_NAME], dcs_aname[RSRC_NAME]);
      }
      else if (!strcmp(valType,"Users")) {
	tmpList = (char *) getFromResultStruct(
	       &myResult,
	       dcs_tname[USER_NAME], dcs_aname[USER_NAME]);
        tmpList2 = (char *) getFromResultStruct(
       	       &myResult,
	       dcs_tname[DOMAIN_DESC], dcs_aname[DOMAIN_DESC]);
      }

      for (i=0; i < myResult.row_count; i++) {
    	if (!strcmp(valType,"Resources")) {
    	   if (!strstr(tmpList,"$deleted"))  { 
    	    fprintf(stdout, "%s<BR>", tmpList);    
    	     }    
    	    tmpList += MAX_DATA_SIZE;
    	}
    	else if (!strcmp(valType,"Users")){
    	       if (!strstr(tmpList,"$deleted"))  { 
    	            fprintf(stdout, "%s@%s<BR>", tmpList, tmpList2);  
    	        }      
    	  tmpList += MAX_DATA_SIZE;
    	  tmpList2 += MAX_DATA_SIZE;
    	}

      }

    }

    fprintf(stdout, "</span>");fflush(stdout);
    
    free(origTmpList);
    return 0;
}

int
getUserHomeZone(inStruct Sentries)
{
  int i;
  i = srbGetMcatZone(Sentries->conn,
		     Sentries->srbUser, Sentries->userDomain,
		     Sentries->userHomeZone);
  return(i);
}


int
getUserConnectionZone(inStruct Sentries)
{
  int i;
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    char *zName;
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   selval[ZONE_NAME] = 1;
   strcpy(qval[ZONE_LOCALITY]," = 1");
   i = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0, 
				   qval, selval, &myResult, MAX_RESULT_NUM);
   if (i != 0) {
     return(-1);
   }
   zName = (char *) getFromResultStruct(
       &myResult, dcs_tname[ZONE_NAME], dcs_aname[ZONE_NAME]);
   strcpy(Sentries->userConnectionZone,zName);
   return(0);
}


int MakeEntriesForQuery(inStruct Sentries) {
    char *buf = malloc(MAX_TOKEN + 20);
    char *tmpPtr;
    int i;
    
    for (i=0; i < Sentries->m ; i++) {
        if (!strcmp(Sentries->entries[i].val, "contains") || !strcmp(Sentries->entries[i].val, "not contains")) {
	  if (strlen(Sentries->entries[i+1].val) > 0) {
            sprintf(buf, "%s%s%s", "%", Sentries->entries[i+1].val, "%");    
	    strcpy(Sentries->entries[i+1].val, buf);
	  }
        }
        else if (!strcmp(Sentries->entries[i].val, "between") || !strcmp(Sentries->entries[i].val, "not between")) {
            /*** convert one blank space into single quote for SQL syntax ***/
            tmpPtr = Sentries->entries[i+1].val;
            while (tmpPtr[0] != ' ') {
                tmpPtr++;
            }
            tmpPtr[0] = '\'';
            
            tmpPtr = Sentries->entries[i+1].val + strlen(Sentries->entries[i+1].val) - 1;
            while (tmpPtr[0] != ' ') {
                tmpPtr--;
            }
            tmpPtr[0] = '\'';                
        
        }
    }
    
    free(buf);
    return 0;
}


int changetype(inStruct Sentries)
{
  int j;
  char dataObj[MAX_TOKEN * 2];
   printf("Changing Type ...\n");fflush(stdout);
    sprintf(dataObj,"%s&COPY=%s&DVERSION='%s'",
	    Sentries->entries[1].val, Sentries->entries[3].val, Sentries->entries[4].val);

     j = srbModifyDataset(Sentries->conn, MDAS_CATALOG,
			  dataObj, Sentries->entries[2].val,
			  "","", Sentries->entries[5].val,"",
			  D_CHANGE_TYPE);
     if (j != 0)
       printf("<br>Error: In changing type %s/%s to %s; <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", Sentries->entries[2].val, Sentries->entries[1].val, Sentries->entries[5].val, j,j);
     else
      printf("Done");
   bRMap(2,1);
   return 0;
}

int makeChangeTypeForm(inStruct Sentries, int m)
{

  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> MYSRB - A TransSystem Data Explorer </TITLE></HEAD><body background=\"%s/srb3.jpg\">\n", webPath);
  printf("<TABLE><TR><TD><B><FONT COLOR=#0000FF>Change Data Type</FONT></B></TD></TR>\n");
  printf("<TR><TD><FONT COLOR=#FF0000>Collection:</FONT> %s</TD></TR>\n",
	 Sentries->entries[2].val);
  if (strlen(Sentries->entries[1].val) > 0)
    printf("<TR><TD><FONT COLOR=#FF0000>Data Object:</FONT>%s</TD></TR>\n",
	   Sentries->entries[1].val);
  printf("<TR><TD><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"changetype\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">",Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">",Sentries->entries[2].val);
  printf("<input type=hidden name=\"copynum\"value=\"%s\">",Sentries->entries[4].val);
  printf("<input type=hidden name=\"vernum\"value=\"%s\">",Sentries->entries[6].val);
  unescape_url(Sentries->entries[5].val);
  getValuesAsSelect(Sentries, DATA_TYP_NAME, Sentries->selectData, Sentries->entries[5].val,"");
  printf("%s\n", Sentries->selectData);
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"Change Type\"></FORM></TD></TR></TABLE></body></html>\n");
  return 0;
}


int changeURL(inStruct Sentries)
{
  int j;
  char dataObj[MAX_TOKEN * 2];
   printf("Changing URL ...\n"); fflush(stdout);
    sprintf(dataObj,"%s&COPY=%s&DVERSION='%s'", Sentries->entries[1].val, Sentries->entries[3].val, Sentries->entries[4].val);

     j = srbModifyDataset(Sentries->conn, MDAS_CATALOG,
			  dataObj, Sentries->entries[2].val,
			  "","", Sentries->entries[5].val,"",
			  D_CHANGE_DPATH);
     if (j != 0)
       printf("<br>Error: In changing URL %s/%s to %s; <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", Sentries->entries[2].val, Sentries->entries[1].val, Sentries->entries[5].val, j,j);
     else
      printf("Done");
   bRMap(2,1);
   return 0;
}


int
makeChangeURLForm(inStruct Sentries, int m)
{

  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> MYSRB - A TransSystem Data Explorer</TITLE></HEAD><body background=\"%s/srb3.jpg\">\n", webPath);
  printf("<TABLE><TR><TD><B><FONT COLOR=#0000FF>Change URL</FONT></B></TD></TR>\n");
  printf("<TR><TD><FONT COLOR=#FF0000>Collection:</FONT> %s</TD></TR>\n", Sentries->entries[2].val);
  if (strlen(Sentries->entries[1].val) > 0)
    printf("<TR><TD><FONT COLOR=#FF0000>Data Object:</FONT>%s</TD></TR>\n", Sentries->entries[1].val);
  printf("<TR><TD><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"changeURL\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">", Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">", Sentries->entries[2].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">", Sentries->entries[4].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">", Sentries->entries[6].val);
  unescape_url(Sentries->entries[5].val);
  printf("<INPUT NAME=\"newname\", VALUE= \"%s\",SIZE=200,12>\n", Sentries->entries[5].val);
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"Change URL\"></FORM></TD></TR></TABLE></body></html>\n");
  return 0;
}


int changeSQL(inStruct Sentries)
{
  int j;
  char dataObj[MAX_TOKEN * 2];
   printf("Changing SQL Query ...\n"); fflush(stdout);
    sprintf(dataObj,"%s&COPY=%s&DVERSION='%s'", Sentries->entries[1].val, Sentries->entries[3].val, Sentries->entries[4].val);

     j = srbModifyDataset(Sentries->conn, MDAS_CATALOG,
			  dataObj, Sentries->entries[2].val,
			  "","", Sentries->entries[5].val,"",
			  D_CHANGE_DPATH);
     if (j != 0)
       printf("<br>Error: In changing SQL Query %s/%s to %s; <A href=\"srberror3.cgi?%i\">Error: %i</A>\n", Sentries->entries[2].val, Sentries->entries[1].val, Sentries->entries[5].val, j,j);
     else
      printf("Done");
   bRMap(2,1);
   return 0;
}


int
makeChangeSQLForm(inStruct Sentries, int m)
{

  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf ("<HTML><HEAD> <TITLE> MYSRB - A TransSystem Data Explorer</TITLE></HEAD><body background=\"%s/srb3.jpg\">\n", webPath);
  printf("<TABLE><TR><TD><B><FONT COLOR=#0000FF>Change SQL Query</FONT></B></TD></TR>\n");
  printf("<TR><TD><FONT COLOR=#FF0000>Collection:</FONT> %s</TD></TR>\n", Sentries->entries[2].val);
  if (strlen(Sentries->entries[1].val) > 0)
    printf("<TR><TD><FONT COLOR=#FF0000>Data Object:</FONT>%s</TD></TR>\n", Sentries->entries[1].val);
  printf("<TR><TD><FORM METHOD=\"GET\" ACTION=\"mysrb331.cgi\">\n");
  printf("<input type=hidden name=\"function\" value=\"changeSQL\">");
  printf("<input type=hidden name=\"data\" value=\"%s\">", Sentries->entries[1].val);
  printf("<input type=hidden name=\"coll\" value=\"%s\">", Sentries->entries[2].val);
  printf("<input type=hidden name=\"copynum\" value=\"%s\">", Sentries->entries[4].val);
  printf("<input type=hidden name=\"vernum\" value=\"%s\">", Sentries->entries[6].val);
  unescape_url(Sentries->entries[5].val);
  printf("<INPUT NAME=\"newname\", VALUE= \"%s\",SIZE=200,12>\n", Sentries->entries[5].val);
  printf("<input type=hidden name=\"session\" value=\"%s\">", Sentries->pidStr);
  printf("<BR><INPUT TYPE=\"submit\" VALUE=\"Change SQL Query\"></FORM></TD></TR></TABLE></body></html>\n");
  return 0;
}

int removeFinalBlanksFromEntries(inStruct Sentries) {
    int i;
        for (i=0; Sentries->entries[i].val ; i++) {
            while (Sentries->entries[i].val[strlen(Sentries->entries[i].val)-1] == ' ') {
                Sentries->entries[i].val[strlen(Sentries->entries[i].val)-1] = '\0';
            }
        }
    return (0);
}


int expandDataString(void *data, unsigned int size, unsigned int refill) 
{
    /****** problem with this on Linux ******/
    char *tmpPtr;
    
    tmpPtr = (char *)realloc(data, size + refill);
  
    if (tmpPtr) {
        data = tmpPtr;
        return 0;
    }
    else {
        return -1;
    }
    /*************************************/
    /******** so we do this instead *******
    char newString[size+refill];
    char *tmpPtr = data;
 
    strncpy(newString, data, size); 
    data = newString;
    free(tmpPtr);
 
    return 0;
    /**************************************/

}


int
makeTreePageNew(inStruct Sentries)
{
   int i;
   for (i=1; i < 9; i++) 
   escape_url(Sentries->entries[i].val);

    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<HTML><HEAD> <TITLE> %s </TITLE> <BASE TARGET = \"_top\">", "MYSRB V9.0");
    printf("<meta http-equiv=\"no-cache\">");
    printf("</HEAD>");
    printf("<FRAMESET rows=\"40%,60%\" BORDER=no >");
    printf("<FRAME SRC=\"mysrb331.cgi?function=makeZoneFrame&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">",Sentries->entries[1].val, Sentries->entries[2].val, Sentries->entries[3].val, Sentries->entries[4].val, Sentries->entries[5].val, Sentries->entries[6].val, Sentries->entries[7].val, Sentries->entries[8].val);
    printf("<FRAME NAME=\"coll%s\" SRC=\"mysrb331.cgi?function=makecollframe&dataname=%s&collection=%s&seek=%s&copynum=%s&datatype=%s&vernum=%s&owner=%s&session=%s\">", Sentries->entries[8].val, Sentries->entries[1].val,Sentries->entries[2].val,Sentries->entries[3].val,Sentries->entries[4].val,Sentries->entries[5].val,Sentries->entries[6].val,Sentries->entries[7].val, Sentries->entries[8].val);
    printf("</FRAMESET></html>");
}


int makeCollFrame(inStruct Sentries) {
    
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html><head><title>SRB Exploration Tree</title>");
    printf("<style>\n\
body { font: 10pt Tahoma, Arial, sans-serif;  color: navy; }\n\
.trigger { cursor: pointer;  cursor: hand; }\n\
.branch { display: none;  margin-left: 16px; }\n\
a:link { color: #000000; text-decoration: none }\n\
a:visited { color: #000000; text-decoration: none }\n\
a:hover { color: #0000FF; text-decoration: underline }\n\
a:active { color: #0000FF; text-decoration: none }\n\
</style>\n");

    printf("<script language=\"JavaScript\">\n\
var openImg = new Image();\n\
openImg.src = \"%s/open.gif\";\n\
var closedImg = new Image();\n\
closedImg.src = \"%s/closed.gif\";\n", webPath, webPath);

    printf("function showBranch(branch){\n\
	var objBranch = document.getElementById(branch).style;\n\
	if(objBranch.display==\"block\")\n\
		objBranch.display=\"none\";\n\
	else\n\
		objBranch.display=\"block\";\n\
}\n\
function swapFolder(img){\n\
	objImg = document.getElementById(img);\n\
	if(objImg.src.indexOf('closed.gif')>-1)\n\
		objImg.src = openImg.src;\n\
	else\n\
		objImg.src = closedImg.src;\n\
}\n\
</script>\n");
    printf("</head>\n"); 
 
    if (!strcmp(mode,"light")) {
        printf("<body bgcolor=\"#FFFFF7\">\n");
    }
    else {
        printf("<body background=\"%s/srb3.jpg\">\n", webPath);
    }    
    if ( Sentries->currentMcatZone != NULL) {
       if ( Sentries->oldMcatZone == NULL)
           reputConnInfo (Sentries);
       else if (strcmp(Sentries->currentMcatZone,Sentries->oldMcatZone))
           reputConnInfo (Sentries);
    }
    printf("<table>");
    
    printf("<tr><td><FONT SIZE=-1>");   fflush(stdout);
    
    expandCollectionInTree(Sentries, "all", "shared");
    MakeTreeFromFile(Sentries, "shared");    
    printf("<HR>");    fflush(stdout);
   
    printf("</FONT></td></tr><tr><td><FONT SIZE=-1>");  
    
    expandCollectionInTree(Sentries, "all", "local");  
    MakeTreeFromFile(Sentries, "local");    
    printf("</FONT></td></tr></table>"); 
    
    printf("</body></html>");  

}


int makeZoneFrame(inStruct Sentries) {
    
    printf("Content-type: text/html%c%c",10,10);fflush(stdout);
    printf("<html><head><title>SRB Exploration Tree</title>");
    printf("<style>\n\
body { font: 10pt Tahoma, Arial, sans-serif;  color: navy; }\n\
.trigger { cursor: pointer;  cursor: hand; }\n\
.branch { display: none;  margin-left: 16px; }\n\
a:link { color: #000000; text-decoration: none }\n\
a:visited { color: #000000; text-decoration: none }\n\
a:hover { color: #0000FF; text-decoration: underline }\n\
a:active { color: #0000FF; text-decoration: none }\n\
</style>\n");

    printf("<script language=\"JavaScript\">\n\
var openNode = new Image();\n\
openNode.src = \"%s/openNode.gif\";\n\
var closedNode = new Image();\n\
closedNode.src = \"%s/closedNode.gif\";\n\n", webPath, webPath);

    printf("function showBranch(branch){\n\
	var objBranch = document.getElementById(branch).style;\n\
	if(objBranch.display==\"block\")\n\
		objBranch.display=\"none\";\n\
	else\n\
		objBranch.display=\"block\";\n\
}\n\
function swapNfolder(img){\n\
	objImg = document.getElementById(img);\n\
	if(objImg.src.indexOf('closedNode.gif')>-1)\n\
		objImg.src = openNode.src;\n\
	else\n\
		objImg.src = closedNode.src;\n\
}\n\
</script>\n");
    printf("</head>\n"); 
 
    if (!strcmp(mode,"light")) {
        printf("<body bgcolor=\"#FFFFF7\">\n");
    }
    else {
        printf("<body background=\"%s/srb3.jpg\">\n", webPath);
    }    
    if ( Sentries->currentMcatZone != NULL) {
       if ( Sentries->oldMcatZone == NULL)
           reputConnInfo (Sentries);
       else if (strcmp(Sentries->currentMcatZone,Sentries->oldMcatZone))
           reputConnInfo (Sentries);
    }
    printf("<table>");
    
    printf("<tr><td><FONT SIZE=-1>");   
    
    printf("<span><font color=000088><h3>%s</h3></font></span>", mySrbStrings[248]);
    printZoneInfo(Sentries);
    
    printf("</FONT></td></tr></table>"); 
    
    printf("</body></html>");  

}



/* printAccsCollectionsAsTreeNew is same as printAccsCollectionsAsTree but adds 
   an extra MCAT query to get zone information. To be used in separate frame.
   Not used in current version */
printAccsCollectionsAsTreeNew(inStruct Sentries, char *accs, char *collType) 
{
    mdasC_sql_result_struct myResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int selval[MAX_DCS_NUM];
    int j = nID;
    int i=0, nbopen=0, toClose=0;
    int status, subSize;
    char *tmpList, *origTmpList, *ownerList, *domainList, *tmpPtr, *fixPtr;
    char *zName;
    char *motherColl = malloc(MAX_TOKEN);
    char *collection = malloc(MAX_TOKEN), *currentColl;
    char *toPrint = malloc(MAX_TOKEN);
    char *collPath = malloc(MAX_TOKEN);
    char *linkColl = malloc(MAX_TOKEN);

  
   for (i = 0; i < MAX_DCS_NUM; i++) {
     sprintf(qval[i],"");
     selval[i] = 0;
   }
   myResult.row_count = 0;
   
   if (!strcmp(collType, "shared")) {
        fprintf(stdout,"<NOBR><B>%s</B><BR>",mySrbStrings[259]);
        if (!Sentries->currentMcatZone) {
            Sentries->currentMcatZone = Sentries->userConnectionZone; 
        }
        sprintf(qval[DATA_GRP_NAME], " like '/%s/%%' ", Sentries->currentMcatZone);
   }
   else if (!strcmp(collType, "local")) {
        fprintf(stdout,"<NOBR><B>%s</B><BR>",mySrbStrings[264]);
        sprintf(qval[DATA_GRP_NAME], " like '/home/%%' ");
   }

/************ old settings *********************************
   selval[ACCESS_COLLECTION_NAME] = 1;
   sprintf(qval[COLLECTION_ACCESS_CONSTRAINT]," = '%s' ",accs); 
   sprintf(qval[COLLECTION_ACCESS_CONSTRAINT]," like '%%%%' "); 
   sprintf(qval[USER_NAME], " = '%s'", Sentries->srbUser);  
   sprintf(qval[DOMAIN_DESC], " = '%s'", Sentries->userDomain);   
   if (Sentries->currentMcatZone != NULL &&
       strcmp(Sentries->currentMcatZone,Sentries->userConnectionZone) )
     sprintf(qval[ACCESS_COLLECTION_NAME], " like '/%s/%%' ",Sentries->currentMcatZone);
/************************************************************/


    selval[DATA_GRP_NAME] = 1;
    selval[COLL_OWNER_NAME] = 1;    
    selval[COLL_OWNER_DOMAIN] = 1;

   status = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0, qval, selval, &myResult, MAX_RESULT_NUM * 10);
   if (status < 0){
     return(status);
   }

   tmpList = (char *) getFromResultStruct(&myResult, dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
   ownerList = (char *) getFromResultStruct(&myResult, dcs_tname[COLL_OWNER_NAME], dcs_aname[COLL_OWNER_NAME]);
   domainList = (char *) getFromResultStruct(&myResult, dcs_tname[COLL_OWNER_DOMAIN], dcs_aname[COLL_OWNER_DOMAIN]);

   origTmpList = tmpList;
  
   strcpy(motherColl, "");
   for (i=0; i < myResult.row_count; i++) {

      if (!strstr(tmpList,"$deleted"))  {
    
    /*** to prevent problems from possible blank named collections ***/
            if (strlen(tmpList) > 1 && tmpList[strlen(tmpList)-1] == '/' ) {
                sprintf(collection, "%s$no_name", tmpList);
            
            }
            else if (strlen(tmpList) > 1 && (fixPtr = strstr(tmpList, "//")) ) {
                fixPtr[0] = '\0';
                sprintf(collection, "%s/$no_name%s", tmpList, fixPtr+1);
                }
            
                else {
                  strcpy(collection, tmpList);    
            }            
     /*********************/       
      
          strcat(motherColl, "/");

          currentColl = collection;
          tmpPtr = motherColl+strlen(motherColl)-1;
          
          while (!strstr(currentColl, motherColl)) {
          /* in this loop we find the full path of new mother collection */

                tmpPtr--;
                while (tmpPtr[0] != '/') {
                    tmpPtr--;
                }
                tmpPtr[1]='\0';
    
                toClose++;

          }

          for ( ; toClose>0; toClose--) {
            if (nbopen>0) {
                fprintf(stdout, "</span>");fflush(stdout);
                nbopen--;
                }       
          }
            
          /* collPath contains full collection name for link access */
          strcpy(collPath, motherColl);
          tmpPtr = currentColl;
          currentColl = currentColl + strlen(motherColl);
          strcpy(motherColl, tmpPtr);
  
          while (subSize=strcspn(currentColl,"/")) {
            /* if the new collection includes subcollections we must break it 
               into single collections and print each of them */
   
              strcpy(toPrint, currentColl);
              toPrint[subSize] = '\0';
          
              if (collPath[strlen(collPath)-1] != '/') {
                strcat(collPath, "/");
              }

              /* must not go past the end of collection name */
              if (strstr(currentColl, "/")) {
                currentColl = currentColl+subSize+1;
              }
              else {
                currentColl = currentColl+subSize;
              }
     
              j++;

              sprintf(buf, "<span class=\"trigger\" onClick=\"showBranch('branch%d');swapFolder('folder%d')\">\n<img align=\"top\" src=\"%s/closed.gif\" border=\"0\" id=\"folder%d\">\n", i+j, i+j, webPath, i+j);
              fprintf(stdout, "%s", buf);fflush(stdout);

              /* collection link */
              if (strcmp(toPrint, "$no_name") ) {
                  sprintf(linkColl, "%s%s", collPath, toPrint);
              } 
              else {
                  sprintf(linkColl, "%s", collPath);
              }
              
              escape_url(linkColl);              
    	      sprintf(buf, "</span><A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"sub%s\">%s </A>", "", linkColl, ownerList, domainList, Sentries->pidStr, Sentries->pidStr, toPrint);              
              
              
              fprintf(stdout, "%s", buf);
              fflush(stdout); 
              
              sprintf(buf, "<br>\n<span class=\"branch\" id=\"branch%d\">\n", i+j);              
              nbopen++;
              fprintf(stdout, "%s", buf);fflush(stdout);
                     
              /* We add current collection to the path in case the new collection included 
                 subcollections. if not then we leave the loop and it doesn't do any harm */ 
              sprintf(buf, "%s%s", collPath, toPrint);
              strncpy(collPath, buf, MAX_TOKEN);
              
              fflush(stdout);
          }
          
      }
      
      tmpList += MAX_DATA_SIZE;
      ownerList += MAX_DATA_SIZE;
   }

   while (myResult.continuation_index >= 0 && i+j <= 15000) {
    /*** need to limit the value of (i+j) to make sure the left frame doesn't
            get too big in the case of a very large collection tree.
            Will replace this with a clean indexing in future ***/

     clearSqlResult (&myResult);
     status = srbGetMoreRows(Sentries->conn, 0, myResult.continuation_index,
			     &myResult, MAX_RESULT_NUM);
      			     
	if (myResult.result_count == 0  || status != 0)
	  break;

    tmpList = (char *) getFromResultStruct(&myResult, dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
    ownerList = (char *) getFromResultStruct(&myResult, dcs_tname[COLL_OWNER_NAME], dcs_aname[COLL_OWNER_NAME]);
    domainList = (char *) getFromResultStruct(&myResult, dcs_tname[COLL_OWNER_DOMAIN], dcs_aname[COLL_OWNER_DOMAIN]);
     
    /* to make sure each node still has a unique id even if i is set back to 0 */
     j+=i;

   for (i=0; i < myResult.row_count; i++) {

      if (!strstr(tmpList,"$deleted"))  {

    /*** to prevent problems from possible blank named collections ***/
            if (strlen(tmpList) > 1 && tmpList[strlen(tmpList)-1] == '/' ) {
                sprintf(collection, "%s$no_name", tmpList);
            
            }
            else if (strlen(tmpList) > 1 && (fixPtr = strstr(tmpList, "//")) ) {
                fixPtr[0] = '\0';
                sprintf(collection, "%s/$no_name%s", tmpList, fixPtr+1);
                }
            
                else {
                  strcpy(collection, tmpList);    
            }            
     /*********************/ 
       
          strcat(motherColl, "/");
          currentColl = collection;
          tmpPtr = motherColl+strlen(motherColl)-1;
          
          while (!strstr(currentColl, motherColl)) {
          /* in this loop we find the full path of new mother collection */
  
                tmpPtr--;
                while (tmpPtr[0] != '/') {
                    tmpPtr--;
                }
                tmpPtr[1]='\0';
    
                toClose++;

          }
 
          for ( ; toClose>0; toClose--) {
            if (nbopen>0) {
                fprintf(stdout, "</span>");
                nbopen--;
                }       
          }
         
          /* collPath contains full collection name for link access */
          strcpy(collPath, motherColl);
         
          tmpPtr = currentColl;
          currentColl = currentColl + strlen(motherColl);
          strcpy(motherColl, tmpPtr);
           
          while (subSize=strcspn(currentColl,"/")) {
            /* if the new collection includes subcollections we must break it 
               into single collections and print each of them */

              strcpy(toPrint, currentColl);
              toPrint[subSize] = '\0';
          
              if (collPath[strlen(collPath)-1] != '/') {
                strcat(collPath, "/");
              }

              /* must not go past the end of collection name */
              if (strstr(currentColl, "/")) {
                currentColl = currentColl+subSize+1;
              }
              else {
                currentColl = currentColl+subSize;
              }
      
              j++;

              sprintf(buf, "<span class=\"trigger\" onClick=\"showBranch('branch%d');swapFolder('folder%d')\">\n<img align=\"top\" src=\"%s/closed.gif\" border=\"0\" id=\"folder%d\">\n", i+j, i+j, webPath, i+j);
              fprintf(stdout, "%s", buf);

              /* collection link */
              if (strcmp(toPrint, "$no_name") ) {
                  sprintf(linkColl, "%s%s", collPath, toPrint);
              } 
              else {
                  sprintf(linkColl, "%s", collPath);
              }
              
              escape_url(linkColl);              
    	      sprintf(buf, "</span><A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=%s@%s&session=%s\"  target=\"sub%s\">%s </A>", "", linkColl, ownerList, domainList, Sentries->pidStr, Sentries->pidStr, toPrint);              
 
              
              fprintf(stdout, "%s", buf);
              fflush(stdout); 
              
              sprintf(buf, "<br>\n<span class=\"branch\" id=\"branch%d\">\n", i+j);              
              nbopen++;
              fprintf(stdout, "%s", buf);fflush(stdout);
              
              /* We add current collection to the path in case the new collection included 
                 subcollections. if not then we leave the loop and it doesn't do any harm */ 
              sprintf(buf, "%s%s", collPath, toPrint);
              strncpy(collPath, buf, MAX_TOKEN);
              
              fflush(stdout);

          }
          
      }
     
      tmpList += MAX_DATA_SIZE;
      ownerList += MAX_DATA_SIZE;

   }
   } 

   
     for ( ; nbopen>0; nbopen--) {
        fprintf(stdout, "</span>");
     }
     
    fprintf(stdout, "</NOBR>");
    
    nID+=i+j;  /* to avoid node ID collisions in case this function is called again */
    
    //free(origTmpList);
    free(motherColl);
    free(toPrint);
    free(collection);
    free(collPath);
        
    return 0;
}


int expandCollectionInTree(inStruct Sentries, char *accs, char *collType)
{
    mdasC_sql_result_struct collResult;
    int status;
    char collection[MAX_TOKEN];
    char treeFileId[MAX_TOKEN];
    FILE *treeFile;


    if (!strcmp(collType, "shared")) {
        fprintf(stdout, "<NOBR><B>%s</B><BR>", mySrbStrings[259]);
        if (!Sentries->currentMcatZone) {
            Sentries->currentMcatZone = Sentries->userConnectionZone;
        }
        sprintf(collection, "/%s", Sentries->currentMcatZone);
    }
    else if (!strcmp(collType, "local")) {
        fprintf(stdout, "<NOBR><B>%s</B><BR>", mySrbStrings[264]);
        strcpy(collection, "/home");
    }
    
    sprintf(treeFileId, "%s/CT_%s_%s", logDirec, collType, Sentries->cookieStr);
    treeFile = fopen(treeFileId,"r"); /* try to open collection tree file for this session */
    if (treeFile == NULL) {
         /* if such a file doesn't exist, we will need to create one and explore 
         the first level of the collection tree */
        
        treeFile = fopen(treeFileId,"a");
        
        if (treeFile) {

            collResult.row_count = 0;
      
            status = querySubCollInfo(Sentries, Sentries->conn, 0, collection, MAX_RESULT_NUM, &collResult);
            
            if (status == 0) {
                
                fprintf(treeFile, "%s$explored\n", collection);
                printSubCollsInTree(Sentries, &collResult, treeFile);
                
                while (collResult.continuation_index >= 0) {
              
                	clearSqlResult (&collResult);
                	status = srbGetMoreRows(Sentries->conn, 0, collResult.continuation_index,
                				&collResult, MAX_RESULT_NUM);
                	if (collResult.result_count == 0  || status != 0)
                	    break;
                	
                	printSubCollsInTree(Sentries, &collResult, treeFile);
                
                }
                  
                clearSqlResult (&collResult); 
            }
                       
            fclose(treeFile);
            return 0;
        } /* end of if(treeFile) */
        
    }
    fclose(treeFile);
    return 0;

}


int printSubCollsInTree(inStruct Sentries, mdasC_sql_result_struct *collResult, FILE *treeFile) 
{
    char *dataList;
    int i, j;
    int status = 0;
    
    dataList = (char *) getFromResultStruct((mdasC_sql_result_struct *) collResult, dcs_tname[DATA_GRP_NAME], dcs_aname[DATA_GRP_NAME]);
     
    for (i = 0; i < collResult->row_count; i++) {
       
        if (treeFile) {
            j = fprintf(treeFile, "%s$unexplored\n", dataList);
            if (j < 0) {  
                status = j;
            }
        }
        else {
            status = -1;
        }
        
        dataList += MAX_DATA_SIZE;
    }

    return (status);
}


MakeTreeFromFile(inStruct Sentries, char *collType)
{
    int j = nID;
    int i, nbopen=0, toClose=0;
    int status, subSize;
    int isExplored;
    char *tmpList = malloc(MAX_TOKEN);
    char *tmpPtr, *fixPtr;
    char *motherColl = malloc(MAX_TOKEN);
    char *collection = malloc(MAX_TOKEN), *currentColl;
    char *toPrint = malloc(MAX_TOKEN);
    char *collPath = malloc(MAX_TOKEN);
    char *linkColl = malloc(MAX_TOKEN);
    char treeFileId[MAX_TOKEN];
    FILE *treeFile;
    
    sprintf(treeFileId, "%s/CT_%s_%s", logDirec, collType, Sentries->cookieStr);
    
    treeFile = fopen(treeFileId,"r"); /* try to open collection tree file for this session */
    if (treeFile == NULL) {
     return(-1);
    }
    
    strcpy(motherColl, "");
    
    i=0;
    
    while (tmpList = fgets(tmpList, MAX_DATA_SIZE, treeFile) ) {
        
        if (tmpPtr = strstr(tmpList, "$explored")) {
            tmpPtr[0] = '\0';
            isExplored = 1;    
        }

        if (tmpPtr = strstr(tmpList, "$unexplored")) {
            tmpPtr[0] = '\0';
            isExplored = 0;    
        }
        
        /*** to prevent problems from possible blank named collections ***/
        if (strlen(tmpList) > 1 && tmpList[strlen(tmpList)-1] == '/' ) {
            sprintf(collection, "%s$no_name", tmpList);
        
        }
        else if (strlen(tmpList) > 1 && (fixPtr = strstr(tmpList, "//")) ) {
            fixPtr[0] = '\0';
            sprintf(collection, "%s/$no_name%s", tmpList, fixPtr+1);
            }
        
            else {
              strcpy(collection, tmpList);    
        }            
         /*********************/       
  
      strcat(motherColl, "/");

      currentColl = collection;
      tmpPtr = motherColl+strlen(motherColl)-1;
      
      while (!strstr(currentColl, motherColl)) {
      /* in this loop we find the full path of new mother collection */

            tmpPtr--;
            while (tmpPtr[0] != '/') {
                tmpPtr--;
            }
            tmpPtr[1]='\0';

            toClose++;

      }

      for ( ; toClose>0; toClose--) {
        if (nbopen>0) {
            fprintf(stdout, "</span>");fflush(stdout);
            nbopen--;
            }       
      }
        
      /* collPath contains full collection name for link access */
      strcpy(collPath, motherColl);
      tmpPtr = currentColl;
      currentColl = currentColl + strlen(motherColl);
      strcpy(motherColl, tmpPtr);

      while (subSize=strcspn(currentColl,"/")) {
        /* if the new collection includes subcollections we must break it 
           into single collections and print each of them */

          strcpy(toPrint, currentColl);
          toPrint[subSize] = '\0';
      
          if (collPath[strlen(collPath)-1] != '/') {
            strcat(collPath, "/");
          }

          /* must not go past the end of collection name */
          if (strstr(currentColl, "/")) {
            currentColl = currentColl+subSize+1;
          }
          else {
            currentColl = currentColl+subSize;
          }
 
          j++;
          
          if (isExplored) {
                fprintf(stdout, "<span class=\"trigger\" onClick=\"showBranch('branch%d');swapFolder('folder%d')\">\n<img align=\"top\" src=\"%s/closed.gif\" border=\"0\" id=\"folder%d\">\n", i+j, i+j, webPath, i+j);
          }
          else {
                //fprintf(stdout, "<span class=\"trigger\" onClick=\"showBranch('branch%d');swapFolder('folder%d')\">\n<img align=\"top\" src=\"%s/unexplored.gif\" border=\"0\" id=\"folder%d\">\n", i+j, i+j, webPath, i+j);            
                fprintf(stdout, "<span><A HREF=\"mysrb331.cgi?function=makecollframe&dataname=&collection=&seek=&copynum=&datatype=&vernum=&owner=&session=%s\"  target=\"coll%s\" >", Sentries->pidStr, Sentries->pidStr);
                fprintf(stdout, "<img align=\"top\" src=\"%s/unexplored.gif\" border=\"0\" ></A>\n", webPath);
          }
          
          //fprintf(stdout, "%s", buf);fflush(stdout);

          /* collection link */
          if (strcmp(toPrint, "$no_name") ) {
              sprintf(linkColl, "%s%s", collPath, toPrint);
          } 
          else {
              sprintf(linkColl, "%s", collPath);
          }
          
          escape_url(linkColl);              
	      fprintf(stdout, "</span><A HREF=\"mysrb331.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=1&owner=&session=%s\"  target=\"sub%s\">%s </A>", "", linkColl, Sentries->pidStr, Sentries->pidStr, toPrint);              
          
          
         // fprintf(stdout, "%s", buf);
          fflush(stdout); 
          
          fprintf(stdout, "<br>\n<span class=\"branch\" id=\"branch%d\">\n", i+j);              
          nbopen++;
          //fprintf(stdout, "%s", buf);fflush(stdout);
                 
          /* We add current collection to the path in case the new collection included 
             subcollections. if not then we leave the loop and it doesn't do any harm */ 
          sprintf(buf, "%s%s", collPath, toPrint);
          strncpy(collPath, buf, MAX_TOKEN);
          
          fflush(stdout);
      }
     
     i++;
     }

     fclose(treeFile);
     
     for ( ; nbopen>0; nbopen--) {
        fprintf(stdout, "</span>");
     }
     
    fprintf(stdout, "</NOBR>");
    
    nID+=i+j;  /* to avoid node ID collisions in case this function is called again */
    
    free(tmpList);
    free(motherColl);
    free(toPrint);
    free(collection);
    free(collPath);
    free(linkColl);
        
    return 0;
}


int getCollOwnerDomain(inStruct Sentries, srbConn *conn, char *collection, char *dataStr) /* will be used to find collection owner in metadata frame */
{   
    mdasC_sql_result_struct myresult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, status;
    char *ownerList, *domainList;

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    selval[DATA_GRP_NAME] = 1;
    selval[COLL_OWNER_NAME] = 1;
    selval[COLL_OWNER_DOMAIN] = 1;
    sprintf(qval[DATA_GRP_NAME]," = '%s'", collection);

    status = srbGetDataDirInfoShunt(Sentries,conn, MDAS_CATALOG, qval, selval, &myresult, MAX_RESULT_NUM);

    if (status < 0) {
        if (status != MCAT_INQUIRE_ERROR)       /* no answer */
            fprintf (stderr, "Error getting collection owner: %s, status = %d\n",
          collection, status);
    }
    else {
        ownerList = (char *) getFromResultStruct(&myresult, dcs_tname[COLL_OWNER_NAME], dcs_aname[COLL_OWNER_NAME]);
        domainList = (char *) getFromResultStruct(&myresult, dcs_tname[COLL_OWNER_DOMAIN], dcs_aname[COLL_OWNER_DOMAIN]);
        sprintf(dataStr, "%s@s%", ownerList, domainList);
    }
    clearSqlResult (&myresult);

    return (status);
}


/** nextToken() and nextSeparator() are used 
    as part of getSlinkCollectionInfo() to parse 
    the output of "ls -l" when opening 
    a directory shadow object **/
int nextToken(char *ptr) {
    int i=0;
    while (ptr[0] != ' ' && ptr[0] != '\0') {
        ptr++;
        i++;
    }
    while (ptr[0] == ' ' || ptr[0] == '\0') {
        ptr++;
        i++;
    }
    return i;
}


int nextSeparator(char *ptr) {
    int i=0;
    while (ptr[0] == ' ') {
        ptr++;
        i++;
    }
    while (ptr[0] != ' ') {
        ptr++;
        i++;
    }
    return i;
}





int readExtendedTableAttributeIndex(inStruct Sentries, char *schemaStringList, char *udsmdArray[])
{
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int i , j, k, selval[MAX_DCS_NUM];
  mdasC_sql_result_struct *myresult;
  char *aval, *tval, *cval, *sval;
  char tmpStr[MAX_TOKEN];
  myresult  = malloc (sizeof (mdasC_sql_result_struct));
  for (i = 0; i < MAX_DCS_NUM; i++)
    {
           sprintf(qval[i],"");
           selval[i] = 0;
    }
  selval[EXTENSIBLE_SCHEMA_NAME] = 1;
  selval[EXTENSIBLE_TABLE_NAME] = 1;
  selval[EXTENSIBLE_ATTR_NAME] = 1;
  selval[EXTENSIBLE_ATTR_OUTSIDE_NAME] = 1;
  if (schemaStringList != NULL && strlen(schemaStringList) > 0) {
    sprintf(qval[EXTENSIBLE_SCHEMA_NAME]," in (%s) ",schemaStringList);
  }
  failure = 0;
  i = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0,
                                   qval, selval, myresult,
			       MAX_EXT_TABLE_INDEX - MIN_EXT_TABLE_INDEX);
  if (i < 0 || myresult == NULL )
    return(i);
  sval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_SCHEMA_NAME],dcs_aname[EXTENSIBLE_SCHEMA_NAME]);
  tval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_TABLE_NAME],dcs_aname[EXTENSIBLE_TABLE_NAME]);
  aval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_NAME],dcs_aname[EXTENSIBLE_ATTR_NAME]);
  cval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_OUTSIDE_NAME],dcs_aname[EXTENSIBLE_ATTR_OUTSIDE_NAME]);
  j = MIN_EXT_TABLE_INDEX;
  k = 0;
  udsmdArray[k] = NULL;
  for (i = 0; i <myresult->row_count ; i++) {
    if (!strcmp(tval,"UDSMD")) {
      if (strlen(sval) > 0)
        sprintf(tmpStr,"%s:%s.%s",aval,sval,cval);
      else
        sprintf(tmpStr,"%s:%s",aval,cval);
      udsmdArray[k++] = strdup(tmpStr);
      udsmdArray[k] = NULL;
    }
    else {
      if (strlen(sval) > 0)
        sprintf(dcs_cname[j],"%s.%s",sval,cval);
      else
        sprintf(dcs_cname[j],"%s",cval);
      strcpy(dcs_tname[j],tval);
      strcpy(dcs_aname[j],aval);
      j++;
    }
    sval += MAX_DATA_SIZE;
    tval += MAX_DATA_SIZE;
    aval += MAX_DATA_SIZE;
    cval += MAX_DATA_SIZE;
  }
  sval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_SCHEMA_NAME],dcs_aname[EXTENSIBLE_SCHEMA_NAME]);
  tval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_TABLE_NAME],dcs_aname[EXTENSIBLE_TABLE_NAME]);
  aval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_NAME],dcs_aname[EXTENSIBLE_ATTR_NAME]);
  cval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_OUTSIDE_NAME],dcs_aname[EXTENSIBLE_ATTR_OUTSIDE_NAME]);
  free (tval);
  free (aval);
  free (cval);
  free (sval);
  free(myresult);
  return(0);

}

int
getExtSchemaTableAttrAsTabledInsert (inStruct Sentries, char *returnHTMLTable, char *schmeTableName, int numOfRows,
				     int modifyFlag)
{
  char qval[MAX_DCS_NUM][MAX_TOKEN];
  int i , j, k, selval[MAX_DCS_NUM];
  mdasC_sql_result_struct *myresult, *myresult2;
  char *aval, *tval, *cval, *sval;
  char *avalt, *tvalt, *cvalt, *svalt;
  char *mval[SMALL_TOKEN];
  char *nval[SMALL_TOKEN];
  char *mvalt[SMALL_TOKEN];
  int   ival[SMALL_TOKEN];
  char tmpStr[MAX_TOKEN * 10];
  char *data;
  char *internalTableName;
  char schemaName[MAX_TOKEN];

  data = returnHTMLTable;
  myresult  = malloc (sizeof (mdasC_sql_result_struct));
  for (i = 0; i < MAX_DCS_NUM; i++)
    {
           sprintf(qval[i],"");
           selval[i] = 0;
    }
  selval[EXTENSIBLE_ATTR_NAME] = 1;
  selval[EXTENSIBLE_ATTR_COMMENTS] = 1;
  selval[EXTENSIBLE_ATTR_OUTSIDE_NAME] = 1;
  internalTableName = strstr(schmeTableName,"->");
  if (internalTableName == NULL)
    return -1;
  *internalTableName = '\0';
  internalTableName += 2;
  sprintf(qval[EXTENSIBLE_TABLE_NAME]," = '%s' ",internalTableName);
  sprintf(qval[EXTENSIBLE_SCHEMA_NAME]," = '%s' ",schmeTableName);
  strcpy(schemaName , schmeTableName);
  failure = 0;
  *(internalTableName - 2) = '-';
  i = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0,
                                   qval, selval, myresult, SMALL_TOKEN);
  if (i < 0 || myresult == NULL )
    return(i);
  aval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_NAME],dcs_aname[EXTENSIBLE_ATTR_NAME]);
  cval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_COMMENTS],dcs_aname[EXTENSIBLE_ATTR_COMMENTS]);
  tval = (char *) getFromResultStruct(
                  (mdasC_sql_result_struct *) myresult,
                  dcs_tname[EXTENSIBLE_ATTR_OUTSIDE_NAME],dcs_aname[EXTENSIBLE_ATTR_OUTSIDE_NAME]);
  
  avalt = aval;
  cvalt = cval;
  tvalt = tval;
  sprintf(data,"<input type=hidden name=\"numOfCols\" value=\"%i\"><input type=hidden name=\"tableName\" value=\"%s\"><input type=hidden name=\"colname\" value=\"",myresult->row_count, internalTableName);
  for (i = 0; i <myresult->row_count ; i++) {
    if (i != 0)
      strcat(data,"|");
    strcat(data,aval);
    aval += MAX_DATA_SIZE;
  }
  if (modifyFlag == 0) 
    sprintf(data,"%s\"><BR><FONT COLOR=#FF0000>%s<BR></FONT>\n<table><tr>",data,aaa[26]);
  else if (modifyFlag == 1)
    sprintf(data,"%s\"><BR><FONT COLOR=#FF0000>%s<BR></FONT>\n<table><tr><th>%s</th>",data,aaa[26],aaa[30]);
  else /* modifyFlag == 2 */
    sprintf(data,"%s\"><BR>\n<table border><tr><th>%s</th>",data,aaa[31]);
	    
  aval = avalt;
  for (i = 0; i <myresult->row_count ; i++) {

    sprintf(data,"%s<th>%s<br><IMG SRC=\"%s/commentsbut.gif\" ALT=\"%s\" width=18 height=18 border=0></th>",
	    data,aval,webPath,cval);
    aval += MAX_DATA_SIZE;
    cval += MAX_DATA_SIZE;
  }
  strcat(data,"</tr>\n");
  myresult2 = myresult;
  if (modifyFlag == 0) {
    for (j = 0; j < numOfRows; j++ ) {
      strcat(data,"<tr>");
      for (i = 0; i <myresult->row_count ; i++) {
	sprintf(data,"%s<td><INPUT TYPE=TEXT NAME=\"sctbatval.%i.%i\",j,i VALUE=\"\"></td>",data,j,i);
      }
      strcat(data,"</tr>\n");
    }
  }
  else {
    sprintf(tmpStr,"'%s'", schemaName);
    readExtendedTableAttributeIndex(Sentries, tmpStr,Sentries->udsmdArray);
    myresult  = malloc (sizeof (mdasC_sql_result_struct));
    for (i = 0; i < MAX_DCS_NUM; i++)
      {
	sprintf(qval[i],"");
	selval[i] = 0;
      }
    aval = avalt;
    for (i = 0; i <myresult2->row_count ; i++) {
      sprintf(tmpStr,"%s.%s",schemaName,tval);
      j = getIndexForConstName(tmpStr);
      if (j > 0) {
	selval[j] = 1;
	ival[i] = j;
      }
      else
	ival[i] = 0;
      nval[i] = aval;
      tval += MAX_DATA_SIZE;
      aval += MAX_DATA_SIZE;
      
    }
    i = srbGetDataDirInfoShunt(Sentries,Sentries->conn, 0,
			       qval, selval, myresult, MAX_RESULT_NUM);
    if (i < 0 || myresult == NULL )
      return(i);

    for (i = 0; i <myresult2->row_count ; i++) {
      if (ival[i] > 0)
	mval[i] = (char *) getFromResultStruct(
					       (mdasC_sql_result_struct *) myresult,
					       dcs_tname[ival[i]],dcs_aname[ival[i]]);
      else
	mval[i] = NULL;
      mvalt[i] = mval[i];
    }
    for (i = 0; i <myresult->row_count ; i++) {
      sprintf(data,"%s<tr><td><input TYPE=CHECKBOX NAME=\"ckbxwqzt%i\"></td>\n",data,i);
      for (j = 0; j < myresult->result_count ; j++) {
	if (mval[j] != NULL) {
	  if (modifyFlag == 1) { /* modify */
	    if (isAlphaString(mval[j])) 
	      sprintf(data,"%s<td><input type=hidden name =\"b%i\" value=\"Q%sQ\"><input type=text value=\"'%s'\" name=\"c%i\"></td>",data,j,mval[j],mval[j],j);
	    else
	      sprintf(data,"%s<td><input type=hidden name =\"b%i\" value=\"%s\"><input type=text value=\"%s\" name=\"c%i\"></td>",data,j,mval[j],mval[j],j);
	  }
	  else { /* modifyFlag == 2  delete */
	    if (isAlphaString(mval[j])) 
	      sprintf(data,"%s<td><input type=hidden name =\"b%i\" value=\"Q%sQ\">'%s'</td>",data,j,mval[j],mval[j]);
	    else
	      sprintf(data,"%s<td><input type=hidden name =\"b%i\" value=\"%s\">%s</td>",data,j,mval[j],mval[j]);
	  }
	  mval[j] += MAX_DATA_SIZE;
	}
      }
      strcat(data,"</tr>\n");
    }
    for (j = 0; j < myresult->result_count ; j++) {
      if (mval[j] != NULL)
	free (mvalt[j]);
    }
    free(myresult);
  }


  free (avalt);
  free (tvalt);
  free (cvalt);
  free(myresult2);
  strcat(data,"</table>\n");

  return(0);
}
