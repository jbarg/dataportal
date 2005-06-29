/*************************************************************************
**
**  File Name: getgosrbobj.c
**
** Altho under the utilities directory, this is not a normal utility.
** It is a CGI program and should be executed in a web environment.
** Carefully read the information below to make use of it.
**
** This was originally developed by Arcot Rajasekar in early 2001 and
** was updated by Don Sutton in mid 2004 to include more http headers
** for file types like .mif, .ppt, .sd, etc.
**
** Now (late 2004) we are moving it into the CVS tree under utilities
** and making it part of the normal build to make it easier to share
** and maintain.
**
**
**  HREF Call Format: <A HREF=http://CGISCRIPTPATH?name=/SRBNAME/COLLECTION/DATANAME>
**     SRBNAME - Is the name of the SRB (system and user)  to be accessed
**               This identifies a file which contains all relevant
**                  parameters to access SRB. This parameter file 
**                  (SRBNAME)is located in AUTHHOMEDIR 
**                  (see #define below)
**               The parameter file has one value per line and requires
**                  information in this order (no blank lines):
**                  srbHost name
**                  srbPort number
**                  user name
**                  domain name 
**                  srbAuth password for the above user
**                  authenticationScheme (eg.ENCRYPT1, GSI_AUTH or PASSWD_AUTH)
**                  has been tested with ENCRYPT1
**                  serverDN name (if using GSI authentication only)
**                      otherwise use  '  '
**               IMPORTANT: The parameter file contains SRB password for a 
**                  user and hence we recommend that you create 
**                  an account which is given 'read-only' permissions.
**                  A good way is to use the public@npaci as the account
**                  and give this account  read permission to your
**                  srbObjects.
**     /COLLECTION/DATANAME - Is the name of the srbObject being accessed
**  HREF Call Examples: <A HREF=http://srb.npaci.edu/cgi-bin/getsrbobj.cgi?name=/srbw/home/testuser.sdsc/srbNote>Textual srbObject</A>
**                      <A HREF=http://srb.npaci.edu/cgi-bin/getsrbobj.cgi?name=/afcs/home/afcscurator.afcs/Sput.c>file in AFCS SRB</A>
**
**  Usage Example: can be found at
**                   http://srb.npaci.edu/getsrbobjtest.html
**
**  After you gmake in the utilities directory, copy bin/getsrbobj to your
**  cgi-bin directory as getsrbobj.cgi (a cgi executable).  Be sure to
**  build on the same platform as your web server runs on.
**
**
**
**************************************************************************/

/* #include "scommands.h" */
#include <stdio.h>
#include "srbClient.h"
  
#define LF 10
#define CR 13


#define BUFSIZE         2000000
#define MAX_ENTRIES 100
#define AUTHHOMEDIR  "srb"


char errorMsg[400];
typedef struct {
   char name[MAX_TOKEN];
   int size;
   char val[HUGE_STRING];
   char *fstr;
} entry;
entry entries[MAX_ENTRIES];
srbConn *conn;

#define MAX_RESULT_NUM 100
  char tmpCollName[1000];
  char tmpDataName[1000];
  int showMetaType;
  int  likeCollFlag = 0;
  int first = 1;
  char *tmpPtr;
  char  pidStr[MAX_TOKEN] = "";
  int globalCounter = 0;
  char frameStr[MAX_TOKEN];
  char urlcollList[3*MAX_TOKEN];
  char urldataList[3*MAX_TOKEN];

/************************************************************************/

/*
 Print a usage message.
 */
int
usage()
{
  printf("getsrbobj should be run in a Web-page CGI-script environment.\n");
  printf("See the beginning of getsrbobj.c for more information.\n");
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

int
getmultipartword( entry *iEntry, char **stquery, char *boundary)
{
  char *inS, *tmp1,*tmp2, *endStr;
  int i,j,k;

  if (strlen(*stquery) < (2 * strlen(boundary)))
    return(1);
  
  if ((tmp1 = strstr((char *) *stquery,boundary)) == NULL) {
    webErrorExit("No Beginning Boundary Found In Field:",0);
  }
  if ((endStr = strstr((char *) (tmp1 + 1), boundary)) == NULL) {
    webErrorExit("No Ending Boundary Found In Field:",0);
  }
  *(endStr -2) = '\0';

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
    iEntry->size = (int)(endStr - tmp1  );
    iEntry->fstr = (char *) malloc(iEntry->size);
    memcpy(iEntry->val, tmp1, iEntry->size);
    iEntry->size -= 2;
  }
  else {
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

  printf("Content-type: text/html%c%c",10,10);fflush(stdout);
  printf("<html>");
  printf("<body bgcolor=#FFFFFF>");
  printf("<B><FONT COLOR=#DC143C>Error: %s: %i</B>\n",msg, status);
  printf("</body></html>");
  exit(-1);
}

int
getEntries()
{
  register int x,m=0;
  char *stquery, *tmpq;
  char reqMethod[100];
  int msgLength;
  char boundary[MAX_TOKEN];
  char contentType[100];

   /*** LOCAL TEST ***/
   for (i = 0; i < 10; i++)
     strcpy(entries[i].val,"");
   if (getenv("CONTENT_TYPE") != NULL)
     strcpy(contentType,getenv("CONTENT_TYPE"));
   else 
     strcpy(contentType,"");
   if (getenv("REQUEST_METHOD") == NULL) {
     usage();
     webErrorExit("No REQUEST_METHOD environment variable found",0);
   }
   strcpy(reqMethod,getenv("REQUEST_METHOD"));
   if (!strcmp(reqMethod,"POST") || !strcmp(reqMethod,"post")) {
     msgLength = atoi(getenv("CONTENT_LENGTH")) + 10;
     stquery =  malloc(msgLength);
     if (fread(stquery, 1, msgLength, stdin) != (msgLength-10)) {
		webErrorExit("short fread",0);
	}
     stquery[msgLength] = '\0';
   }
   else { 
     stquery =  malloc(4096);
     strcpy(stquery,getenv("QUERY_STRING"));
   }
   if (strstr(contentType,"multipart/form-data") != NULL) {
     getBoundary(&stquery, boundary);
     for(x=0;  *stquery != '\0';x++) {
       if (x == MAX_ENTRIES) webErrorExit("MaxEntries Exceeded",x);
       m=x; 
       if (getmultipartword(&entries[x],&stquery, boundary) != 0)
	 break;
     }
     m--;
   }
   else {

     for(x=0;  stquery[0] != '\0';x++) {
       if (x == MAX_ENTRIES) webErrorExit("MaxEntries Exceeded",x);
       m=x; 
       getword(entries[x].val,stquery,'&');
       plustospace(entries[x].val);
       unescape_url(entries[x].val);
       sprintf(entries[x].name, (char *) makeword(entries[x].val,'='));
     }
   }

   /****
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body bgcolor=#FFFFFF>");
      for (x=0; x <=m; x++) printf("<TR><TD>EntryName[%i]=%s EntryVal[%i]=%s<BR></TD></TR>\n",x,entries[x].name,x,entries[x].val);
      printf("</body></html>");
      exit(0);
      ****/
      return(m);
}
int
main(int argc, char **argv)
{

    int  i,j, nbytes,  in_fd, out_fd;
    char buf[BUFSIZE];
    char collName[HUGE_STRING];
    char dataName[HUGE_STRING];
    char dataPre[HUGE_STRING];
    char objType[MAX_TOKEN];
    int m;

    m = getEntries();
    i =  1;

    if (!strcmp(entries[0].name,"sofull")) {
      makeFullSrbObjPage(m);
      exit(0);
    }
    
    while (entries[0].val[i] != '/')
      i++;
    
    splitbychar(&entries[0].val[i],collName,dataName,'/');
    entries[0].val[i] = '\0';
    loginToSrb(entries[0].val);
    entries[0].val[i] = '/';
    for (i = 1; i<= m; i++) {
      strcat(dataName,"&");
      strcat(dataName,entries[i].name);
      strcat(dataName,"=");
      strcat(dataName,entries[i].val);
    }
    if (!strcmp(entries[0].name,"someta")) {
      showMetaForSrbObj(collName,dataName);
      exit(0);
    }
    
    in_fd = srbObjOpen (conn, dataName,  O_RDONLY, collName);
    if (in_fd < 0)  {   /* error */
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body bgcolor=#FFFFFF>");
      printf("<H1> %s </H1><HR>",entries[4].val);fflush(stdout);
      printf("can't open obj \"%s\" in collection \"%s\": Error=%i \n",
	     dataName, collName ,in_fd);
      clFinish(conn);
      exit(1);
    }
    if (strcmp(entries[0].name,"dload")) {
/*      i = getObjType(collName,dataName, objType); */
      i=1;
      splitbychar(dataName,dataPre,objType,'.');
      if (i < 0) {
	printf("Content-type: text/html%c%c",10,10);fflush(stdout);
	printf("<html>");
	printf("<body bgcolor=#FFFFFF>");
	printf("Type Access Error: %i\n", i);
	printf("</body></html>\n");
	clFinish(conn);
	exit(0);
      }
      setObjType(objType);
/*      printf("Content-type: application/pdf%c%c",10,10);fflush(stdout);  */
/*      printf("Content-type: text/html%c%c",10,10);fflush(stdout);  */
/*      printf("<html>");  */
/*      printf("<body bgcolor=#FFFFFF>");  */
/*      printf("Object type is :%s, object pre is :%s", objType, dataPre);  */
    }
    else {
      printf("Content-type: application/srb.download%c%c",10,10);
      clFinish(conn);
      exit(1);
    }
    while ((nbytes = srbObjRead (conn, in_fd, buf, BUFSIZE)) > 0) {
      write(1,buf,nbytes);
    }
    srbObjClose (conn, in_fd);
    clFinish(conn);
    exit(0);

}

getObjType(char *collName, char *dataName, char *objType)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;
    char *rval[MAX_DCS_NUM];
    char *tmpPtr;

    strcpy(objType, "");
    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if ((tmpPtr = strstr(dataName,"&")) == NULL) {
      sprintf(qval[DATA_NAME], " =  '%s'",dataName);
    }
    else {
      /**
      while(isspace(*dataName )) dataName++;
      if (*dataName == '&')
	set_qval_data_cond(dataName,qval);
      else {
	*tmpPtr =  '\0';
	sprintf(qval[DATA_NAME], " =  '%s'",dataName);
	*tmpPtr =  '&';
	set_qval_data_cond(tmpPtr,qval);
      }
      **/
      *tmpPtr = '\0';
      sprintf(qval[DATA_NAME], " =  '%s'",dataName);
      *tmpPtr = '&';
    }
    if (strlen(collName) > 0)
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collName);
    selval[DATA_TYP_NAME] = 1;
     status = srbGetDataDirInfo(conn, 0 , qval, selval, &collResult,
			       5);
    if (status < 0)
      {
	if (status != MCAT_INQUIRE_ERROR) 
	  return(status);
	
      }
    
    tmpPtr = (char *) getFromResultStruct(
		    (mdasC_sql_result_struct *) &collResult,
		    dcs_tname[DATA_TYP_NAME], dcs_aname[DATA_TYP_NAME]);
    strcpy(objType,tmpPtr);
    free(tmpPtr);
    return(0);

}

int
setObjType(char *objType) 
{
  /* this is a clumsy way of doing this. should be replaced soon with
     a more elegant solution **/

    if (!strcmp(objType,"jpeg"))
	printf("Content-type:image/jpeg %c%c",10,10);
    else if (!strcmp(objType,"jpg"))
	printf("Content-type:image/jpeg %c%c",10,10);
    else if (!strcmp(objType,"gif"))
	printf("Content-type:image/gif %c%c",10,10);
    else if (!strcmp(objType,"tiff"))
	printf("Content-type:image/tiff %c%c",10,10);
    else if (!strcmp(objType,"tif"))
	printf("Content-type:image/tiff %c%c",10,10);
    else if (!strcmp(objType,"sd"))
	printf("Content-type:application/sd %c%c",10,10);
    else if (!strcmp(objType,"mif"))
	printf("Content-type:application/mif %c%c",10,10);
    else if (!strcmp(objType,"scene"))
	printf("Content-type:application/scene %c%c",10,10);
    else if (!strcmp(objType,"grd"))
	printf("Content-type:application/grd %c%c",10,10);
    else if  (strstr(objType,"pdf") != NULL) 
      printf("Content-type:application/pdf %c%c",10,10);
    else if (!strcmp(objType,"ppt"))
	printf("Content-type:application/vnd.ms-powerpoint %c%c",10,10);
    else if (!strcmp(objType,"FITS image"))
	printf("Content-type:application/x-fits %c%c",10,10);
    else if (!strcmp(objType,"mov"))
	printf("Content-type:video/quicktime %c%c",10,10);
    else if (!strcmp(objType,"mpeg"))
	printf("Content-type:video/mpeg %c%c",10,10);
    else if (!strcmp(objType,"doc"))
	printf("Content-type:application/msword %c%c",10,10);
    else if (!strcmp(objType,"Excel Spread Sheet"))
	printf("Content-type:application/x-msexcel %c%c",10,10);
    else if (!strcmp(objType,"xls"))
	printf("Content-type:application/x-msexcel %c%c",10,10);
    else if (!strcmp(objType,"ppm image"))
	printf("Content-type:image/x-portable-pixmap %c%c",10,10);
    else if (!strcmp(objType,"html"))
	printf("Content-type:text/html %c%c",10,10);
    else if (!strcmp(objType,"htm"))
	printf("Content-type:text/html %c%c",10,10);
    else if (!strcmp(objType,"xml"))
	printf("Content-type:text/xml %c%c",10,10);
    else if (!strcmp(objType,"realAudio"))
	printf("Content-type:audio/x-pn-realaudio  %c%c",10,10);
    else if (!strcmp(objType,"realVideo"))
	printf("Content-type:audio/x-pn-realaudio  %c%c",10,10);
    else if  (strstr(objType,"ps") != NULL) 
      printf("Content-type:application/postscript %c%c",10,10);
    else if  (strstr(objType,"PDF") != NULL) 
      printf("Content-type:application/pdf %c%c",10,10);
    else if  (strstr(objType,"AVI") != NULL) 
      printf("Content-type:video/msvideo %c%c",10,10);
    else if  (strstr(objType,"powerpoint") != NULL) {
      printf("Content-type:application/vnd.ms-powerpoint %c%c",10,10);
    }
    else if  (strstr(objType,"database") != NULL) {
      if ((strstr(entries[0].val,"XML") != NULL) ||
	  (strstr(entries[0].val,".xml") != NULL))
	printf("Content-type:text/xml %c%c",10,10);
      else if ((strstr(entries[0].val,"HTML") != NULL) ||
	       (strstr(entries[0].val,".html") != NULL))
	printf("Content-type:text/html %c%c",10,10);
      else {
	printf("Content-type: text/html %c%c",10,10);
	printf("<html><body bgcolor=#FFFFFFFF><PRE>");
      }
    }
    else {
      /*
      printf("Content-type: text/html %c%c",10,10);
      printf("<html><body bgcolor=#FFFFFFFF><PRE>");
      */
      if ((strstr(entries[0].val,"HTML") != NULL) ||
	       (strstr(entries[0].val,".html") != NULL))
	printf("Content-type:text/html %c%c",10,10);
      else if ((strstr(entries[0].val,".txt") != NULL) ||
	       (strstr(entries[0].val,".TXT") != NULL))
	printf("Content-type:text/plain %c%c",10,10);
      else
	printf("Content-type: text/plain %c%c",10,10);
    }
    fflush(stdout);
}

int
getConnectionInfo(char u[][MAX_TOKEN], char *collName) {
  int i,j,k;
  char *tmp1;
  char sessionId[MAX_TOKEN];
  FILE *fId;

  sprintf(sessionId,"%s/%s",AUTHHOMEDIR,collName);
  fId = fopen(sessionId,"r");
  if (fId == NULL) {
    sprintf(errorMsg,"Unable to Get Session Info. Please login.");
    return(-errno);
  }
  /* 0-srbHost
     1-PortNum
     2-UserName
     3-DomainName
     4-Auth
     5-authScheme
     6-serverDN
  */
  for (i = 0; i < 7; i++) {
    fgets(u[i],MAX_TOKEN,fId);
    u[i][strlen(u[i])-1] = '\0';
  }
  fclose(fId);
  return (0);
}

loginToSrb(char *collName)
{
    char u[12][MAX_TOKEN];
    i = getConnectionInfo(u, collName);
    if (i != 0) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body bgcolor=#FFFFFFFF>");
      if (i == -100 ||i == -200 ) 
	printf("<br>Error: Collection Name Not Well Formed:%i\n",i);
      else
	printf("<br>Error: %s<BR>\n<A href=\"https://srb.npaci.edu/cgi-bin/srberror2.cgi?%i\">Error: %i</A>",errorMsg,i, i);
       printf("</body></html>");
      exit(1);
    }
    conn = srbConnect (u[0], u[1], u[4], 
     u[2], u[3], u[5],u[6]);


    if (clStatus(conn) != CLI_CONNECTION_OK) {
      printf("Content-type: text/html%c%c",10,10);fflush(stdout);
      printf("<html>");
      printf("<body bgcolor=#FFFFFFFF>");
      printf("<br>Connection to exfMaster failed.\n");
      printf("<br>Error in Connecting to SRB: <A href=\"https://srb.npaci.edu/cgi-bin/srberror2.cgi?%i\">Error: %i</A>\n",clStatus(conn),clStatus(conn));
      clFinish(conn);
      printf("</body></html>");
      exit(1);
    }
    return(0);
}
int
makeFullSrbObjPage(int m)
{
  int i;
    for (i=0; i <= m; i++) 
	escape_url(entries[i].val);
printf("Content-type: text/html%c%c",10,10);fflush(stdout);
   printf ("<HTML><HEAD> <TITLE> MYSRB - A TransSystem Data Explorer </TITLE> <BASE TARGET = \"_top\">");
   printf ("<meta http-equiv=\"no-cache\">");
   printf ("</HEAD><FRAMESET rows=\"25%,75%\" BORDER=no >");
   printf("<FRAME SRC=\"getsrbobj.cgi?someta=\"%s\"",entries[0].val);
   for (i = 1; i <= m; i++) 
     printf("&%s=\"%s\"",entries[i].name,entries[i].val);
   printf(">");
   printf("<FRAME SRC=\"getsrbobj.cgi?data=\"%s\"",entries[0].val);
   for (i = 1; i <= m; i++) 
     printf("&%s=\"%s\"",entries[i].name,entries[i].val);
   printf(">");
   printf("</FRAMESET> </body></html>");

}

int
showMetaForSrbObj(char *collName,char* dataName)
{
     char annoDataName[1000],annoCollName[300];

   if (strlen( dataName) == 0 )   {
     clSplitbychar (collName,tmpCollName, tmpDataName, '/');
     if (showMetaType == 0)
       printSrbTop("C","View All Metadata",tmpCollName, tmpDataName, entries[7].val);
     else if (showMetaType == 1)
       printSrbTop("C","View Metadata Describing Contents",tmpCollName, tmpDataName, entries[7].val);
     else if (showMetaType == 2)
       printSrbTop("C","View Metadata Describing Collection",tmpCollName, tmpDataName, entries[7].val);
     strcpy(annoCollName,collName);
     strcpy(annoDataName,dataName);
     getCollectionMetaNew(collName);
     strcpy(collName,annoCollName);
     strcpy(dataName,annoDataName);
     if (showMetaType == 0) 
	  getCollectionAnno(annoCollName,annoDataName,0);
   }
   else {
     if (showMetaType == 0)
       printSrbTop("D","View All Metadata",
		 collName,dataName,entries[7].val);
     else
       printSrbTop("D","View Metadata Describing Entity",tmpCollName, tmpDataName, entries[7].val);
     strcpy(annoCollName,collName);
     strcpy(annoDataName,dataName);
     i = getDatasetMetaNew(collName,dataName,0);
     strcpy(collName,annoCollName);
     strcpy(dataName,annoDataName);
     if (showMetaType == 0) 
       getDatasetAnno(annoCollName,annoDataName,0);
   }
   printSrbBot(NULL,"Done","View Hidden (System) Metadata","Add Commentary");

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
    fprintf (stdout,"<IMG SRC=\"https://srb.npaci.edu/srbtop.GIF\" width=100%%>\n");fflush(stdout);
  
  /******************* FILE IDENTIFICATION **********************/


    fprintf(stdout,"<table border=0 cellspacing=0 cellpadding=0><tr><td  width=10%% valign=top><IMG SRC=\"https://srb.npaci.edu/srbside.GIF\"></td><td valign=top>\n");
    fprintf (stdout,"<center><font color=000088 face='arial,sans-serif'>\n\
  <h2> %s </h2></font></center>\
<font face='arial,sans-serif'>\
<table border=0 cellspacing=0 cellpadding=0>",message);fflush(stdout);
    if (!strcmp(objType,"C")) 
      fprintf (stdout,"<tr><td>Collection: </td><td><b>  %s </td></tr>\n",
	       childName);
    else if (!strcmp(objType,"D")) 
      fprintf (stdout,"<tr><td>Data Object: </td><td><b>  %s </td></tr>\n",
	       childName);
    fprintf (stdout,"<tr><td>Parent Collection:&nbsp;&nbsp;</td><td><b> %s </td></tr>\n\
<tr><td>Owner: </td><td><b>  %s</td></tr></table>\n\
<hr><p>",parName,owner);fflush(stdout);

}


int
getCollectionMetaNew(char *collection)
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
      make_like_for_mdas(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      likeCollFlag = 1;
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


    status = srbGetDataDirInfo(conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadat Access<A href=\"https://srb.npaci.edu/cgi-bin/srberror2.cgi?%i\">Error: %i</A> \n", status,status);
	else
	  printf("No Metadata Found\n");
	return(status);
    }
    else {
      filterDeleted (&collResult);
      first = 1;
      prtMetaCollNew(&collResult,collection);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtMetaCollNew(&collResult,collection);
      }
      clearSqlResult (&collResult);
    }
    return(0);
}


int
getDatasetAnno(char *collection, char *dataName, int updateFlag)
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
      make_like_for_mdas(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    if (strstr(dataName,"^")) {
       make_like_for_mdas(dataName);
       client_set_qval_data_cond(dataName,qval);
    }
    else {
    if (strstr(dataName,"*") || strstr(dataName,"?")) {
      make_like_for_mdas(dataName);
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


    status = srbGetDataDirInfo(conn, 0 , qval, selval, &collResult,
                               MAX_RESULT_NUM);

    if (status < 0)     {
        if (status != MCAT_INQUIRE_ERROR)
          printf("Annotation Access <A href=\"https://srb.npaci.edu/cgi-bin/srberror3.cgi?%i\">Error: %i</A>\n", status, status);
	else
	  printf("<HR>No Annotations Found\n");
    }
    else {
      filterDeleted (&collResult);
      first = 1;
      prtAnnoData(&collResult, updateFlag);
      while (collResult.continuation_index >= 0) {
        clearSqlResult (&collResult);
        status = srbGetMoreRows(conn, 0,
                                collResult.continuation_index,
                                &collResult, MAX_RESULT_NUM);
        if (collResult.result_count == 0  || status != 0)
          break;
        filterDeleted (&collResult);
        prtAnnoData(&collResult, updateFlag);
      }
      clearSqlResult (&collResult);
    }
    if (updateFlag == 1) {
      fprintf(stdout,"</TABLE><input type=hidden name=\"session\" value=\"%s\">\
<FONT COLOR=#0000FF>Please check the Change boxes of lines that have been changed before submitting<BR>To delete an annotation delete the corresponding 'Annotation' string<BR>\n</FONT>\
<INPUT TYPE=SUBMIT></FORM>\n",pidStr);
    }
     return(0);
}

int
getCollectionAnno(char *collection, char *dataName, int updateFlag)
{

 return (0);
}


int 
getDatasetMetaNew(char *collection, char *dataName, int updateFlag)
{
  mdasC_sql_result_struct collResult;
    char qval[MAX_DCS_NUM][MAX_TOKEN];
    int  selval[MAX_DCS_NUM];
    int i, j;
    int status;
    int collLike;

    if ((tmpPtr= strstr(dataName,"@SHADOW=")) != NULL)
      *tmpPtr = '\0';

    for (i = 0; i < MAX_DCS_NUM; i++) {
        sprintf(qval[i],"");
        selval[i] = 0;
    }
    if (strstr(collection,"*") || strstr(collection,"?")) {
      make_like_for_mdas(collection);
      sprintf(qval[DATA_GRP_NAME], " like '%s'  ESCAPE '\\' ",collection);
      likeCollFlag = 1;
    }
    else {
      sprintf(qval[DATA_GRP_NAME], " =  '%s'",collection);
    }
    if (strstr(dataName,"^")) {
       make_like_for_mdas(dataName);
       client_set_qval_data_cond(dataName,qval);
    }
    else {
      if (strstr(dataName,"*") || strstr(dataName,"?")) {
        make_like_for_mdas(dataName);
        sprintf(qval[DATA_NAME], " like '%s'  ESCAPE '\\' ",dataName);
      }
      else {
        sprintf(qval[DATA_NAME], " =  '%s'",dataName);
      }
    }

    if (tmpPtr != NULL)
      *tmpPtr = '@';

 
   if (atoi(entries[6].val) < 0) {
      if ((tmpPtr = strstr(entries[5].val,"/?SHADOW")) != NULL) {
	tmpPtr += 8;
	if (strlen(tmpPtr) > 0) {
	  if (atoi(entries[6].val) == -1)
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


    status = srbGetDataDirInfo(conn, 0 , qval, selval, &collResult,
			       MAX_RESULT_NUM);

    if (status < 0)     {
	if (status != MCAT_INQUIRE_ERROR)
	  printf("Metadat Access <A href=\"https://srb.npaci.edu/cgi-bin/srberror2.cgi?%i\">Error: %i</A>\n", status,status);
	else
	  printf("No Metadata Found\n");
	return(status);
    }
    else {
      filterDeleted (&collResult);
      first = 1;
      prtMetaDataNew(&collResult,updateFlag);
      while (collResult.continuation_index >= 0) {
	clearSqlResult (&collResult);
	status = srbGetMoreRows(conn, 0,
				collResult.continuation_index,
				&collResult, MAX_RESULT_NUM);
	if (collResult.result_count == 0  || status != 0)
	  break;
	filterDeleted (&collResult);
	prtMetaDataNew(&collResult,updateFlag);
      }
      clearSqlResult (&collResult);
    }
    if (updateFlag == 1) {
      fprintf(stdout,"</TABLE><input type=hidden name=\"session\" value=\"%s\">\
<FONT COLOR=#0000FF>Please check the Change boxes of lines that have been changed before submitting<BR>To delete a metadata triple delete the corresponding 'Metadata Name' string<BR>\n</FONT>\
<INPUT TYPE=SUBMIT></FORM>\n",pidStr);
    }

    return(0);
}

/*** taking this out because its in srbClientUtil.h although not sure its the same????
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
****/

int
prtAnnoData (mdasC_sql_result_struct *myresult, int updateFlag)
{
  int i,j;
  char *collList, *dataList;
  char *anno,*anno_time,*anno_user,*anno_domn,*anno_pos;

  if (first) {
    first = 0;
    globalCounter = 1;
    strcpy(tmpDataName,"");
    strcpy(tmpCollName,"");

    if ( updateFlag == 1) {
      fprintf (stdout,"<FORM METHOD=\"POST\" ACTION=\"https://srb.npaci.edu/cgi-bin/getgosrbimage_secu7.cgi\"  target=\"%s\" >\n\
<input type=hidden name=\"function\" value=\"modifyanno\">\n",frameStr);
      fprintf (stdout,"<TABLE><TR><TH>Change</TH><TH>Annotator</TH><TH>Position</TH><TH>Annotation</TH></TR>\n");
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
    fprintf (stdout,"<p><font color=000088><b>User Annotations about <i> %s </i></b></font>\n\
<table width=100%% border=0 cellspacing=4 cellpadding=0>\n\
<tr><td width=5%%>&nbsp;</td>\
<td><i><font color=000088><b>Position/Type:</b> &nbsp;&nbsp;\
 Annotation &lt; Author, Date &gt;</td></tr>\n",dataList);
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
	      globalCounter,globalCounter,anno_user,globalCounter,anno_domn,
	      anno_user,anno_domn,
	      globalCounter,anno_pos,globalCounter,anno,globalCounter,anno_time);
    }
    globalCounter++;
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
prtMetaDataNew(mdasC_sql_result_struct *myresult, int updateFlag)
{
  int i,j;
  char *meta0, *meta1, *meta2,*dataList, *metanum, *collList;
  char *valPtr;
  char newVal[2*MAX_DATA_SIZE];
  if (first) {
    first = 0; 
    globalCounter = 1;
    strcpy(tmpDataName,"");
    strcpy(tmpCollName,"");
    if ( updateFlag == 1) {
      fprintf (stdout,"<FORM METHOD=\"POST\" ACTION=\"https://srb.npaci.edu/cgi-bin/getgosrbimage_secu7.cgi\"  target=\"%s\" >\n\
<input type=hidden name=\"function\" value=\"modifymeta\">\n",frameStr);
      fprintf (stdout,"<TABLE><TR><TH>Change</TH><TH>MetaData Name</TH><TH>MetaData Value</TH><TH>Value Units</TH></TR>\n");
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
  fprintf (stdout,"<font color=000088><b>Metadata describing entity \
<i> %s </i></b></font>\n\
<table width=100%% border=0 cellspacing=4 cellpadding=0>\n\
<tr><td width=5%%>&nbsp;</td>\
<td><i><font color=000088>Field Name:&nbsp;&nbsp;\
<b> Value</b> <font color=red>Units</font> &lt; Explanation &gt;</td></tr>\n",dataList);
  for (i = 0; i < myresult->row_count; i++) {
    if (strcmp(dataList,tmpDataName) || strcmp(collList,tmpCollName)) {
      /*      if (!showAllMetaFlag) 
	      fprintf (stdout,"<P><FONT COLOR=#0000FF>%s/%s:</FONT>\n",collList,dataList);*/
      strcpy(tmpDataName,dataList);
      strcpy(tmpCollName,collList);
    }

    valPtr = meta1;
    if (!strcmp(meta2,"srbObject")) {
      clSplitbychar (meta1, urlcollList, urldataList, '/');
      escape_url(urlcollList);
      escape_url(urldataList);
      sprintf(newVal,"<A HREF=\"https://srb.npaci.edu/cgi-bin/getgosrbimage_secu7.cgi?function=full&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=0&owner=&session=%s\">%s</A>",urldataList,urlcollList,pidStr,meta1);
      valPtr = newVal;
    }
    else if (!strcmp(meta2,"srbImage")) {
      clSplitbychar (meta1, urlcollList, urldataList, '/');
      escape_url(urlcollList);
      escape_url(urldataList);
      sprintf(newVal,"<IMG SRC=\"https://srb.npaci.edu/cgi-bin/getgosrbimage_secu7.cgi?function=data&dataname=%s&collection=%s&seek=0&copynum=0&datatype=&vernum=0&owner=&session=%s\">",urldataList,urlcollList,pidStr);
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
	      globalCounter,
	      globalCounter,metanum,globalCounter,meta0,globalCounter,meta1,globalCounter,meta2);
    }
    globalCounter++;
    meta0 += MAX_DATA_SIZE;
    meta1 += MAX_DATA_SIZE;
    meta2 += MAX_DATA_SIZE;
    metanum += MAX_DATA_SIZE;
    dataList += MAX_DATA_SIZE;
    collList += MAX_DATA_SIZE;
  }
 fprintf (stdout,"</table>\n");

}


int
prtMetaCollNew(mdasC_sql_result_struct *myresult, char* inCollName)
{
  int i,j;
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
    if (showMetaType == 0 || showMetaType == 2) {
      fprintf (stdout,"<font color=000088><b>Metadata describing entity \
<i> %s </i></b></font>\n\
<table width=100%% border=0 cellspacing=4 cellpadding=0>\n\
<tr><td width=5%%>&nbsp;</td>\
<td><i><font color=000088>Field Name:&nbsp;&nbsp;\
<b> Value</b> <font color=red>Units</font> &lt; Explanation &gt;</td></tr>\n",tmpDataName);
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
      fprintf (stdout,"</table>\n");
    }
    meta0 = tmeta0;
    meta1 = tmeta1;
    meta2 = tmeta2;
    meta3 = tmeta3;
    meta4 = tmeta4;
    metanum = tmetanum;
    collList = tcollList;
    if (showMetaType == 0 || showMetaType == 1) {
      fprintf (stdout,"<p><font color=000088><b>Metadata describing contents of <i> %s </i></b></font>\n\
<table width=100%% border=0 cellspacing=4 cellpadding=0>\n\
<tr><td width=5%%>&nbsp;</td>\
<td><i><font color=000088>Field Name: &nbsp;&nbsp;\
Units (Default Value) &lt; Explanation &gt;</td></tr>\n",tmpDataName);
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

int client_set_qval_data_cond(char *in_data_name, char qval[][MAX_TOKEN])
{
  char st1[MAX_OBJ_NAME_LENGTH],st2[MAX_OBJ_NAME_LENGTH],st[MAX_OBJ_NAME_LENGTH]
;
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
/*          else if(strstr((char *) &tt[0],"DVERSION") == (char *) &tt[0])
            sprintf(qval[DATA_VER_NUM],  "%s",(char *) &tt[4]);
*/
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
