#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#include "scommands.h"
/*
#include "srbClient.h"
#include "srb_client_errno.h"
*/ 
 
#ifdef PORTNAME_c90
#include <sys/fcntl.h>  /* For C-90 - define O_RDONLY */
#endif

  
#define LF 10
#define CR 13


#define SRBUSER        "testuser"
#define SRBUSERDOMAIN        "sdsc"
#define SRBAUTH         "TESTUSER"
#define HOSTADDR       "torah.sdsc.edu"
#define SRBPORT      NULL 

#define OBJNAME     "pingtest"
#define COLLNAME    "/home/testuser.sdsc"
#define OBJTYPE     "generic"
#define OBJSIZE     102400



char * escape_url(char *in);


 srbConn *conn;
    int  nbytes,  in_fd, out_fd;
    char myuser[MAX_TOKEN];
    char passwd[MAX_TOKEN];
    char domain[MAX_TOKEN];
    char hostname[MAX_TOKEN];
    char envstr[MAX_TOKEN], envstr2[MAX_TOKEN], envstr3[MAX_TOKEN], envstr4[MAX_TOKEN];
  int selval[MAX_DCS_NUM];
  int heading = 0;
  int first = 1;
  int  likeCollFlag = 0;
  char *dataBuf;
  char h1title[MAX_TOKEN],portnum[MAX_TOKEN], hostAddr[MAX_TOKEN];
  char srbuser[MAX_TOKEN], srbuserdomain[MAX_TOKEN],srbauth[MAX_TOKEN];
  char rsrcName[20][MAX_TOKEN];


int pingAll(int m);

int
main(int argc, char **argv)
{
  register int x,m=0;
   char *stquery, *tmpq;
   char reqMethod[100];
   int msgLength;
   char contentType[100];
   FILE *fd;
   char inBuf[1000];
   char *tmpPtr, *tmpPtr1;
   char dummy1[MAX_TOKEN],  cliAPIVersion[MAX_TOKEN];
   char hostName[MAX_TOKEN];
   time_t mm1;

   fd = fopen("pingall.conf","r");
   if (fd == NULL) {
     fprintf(stderr,"Unable to open config file:pingall.conf\n");
     exit(1);
   }

   printf("<html>");
   printf("<body bgcolor=#AAFFFFFA>");
   printf("<H1>SRB Operational Status</H1>\n");
   mm1 = time(NULL);
   printf("<FONT COLOR=#0000FF>Status Time:</font> %s<BR>\n",ctime(&mm1));
   hostName[0]='\0';
   gethostname(hostName,MAX_TOKEN);
   printf("<FONT COLOR=#0000FF>Client Host:</font> %s<BR>\n",hostName);
   printf ("<FONT COLOR=#0000FF>Client Version:</font> %s.<BR>\n",SRB_VERSION); 

   printf("<TABLE BORDER>");
   printf("<TR><TH>Zone</TH><TH>Host:Port</TH><TH>UpTime</TH><TH>Server Version</TH><TH>Connect<BR>Time</TH><TH>Resource<BR>Tested</TH><TH>Put<BR>Time</TH><TH>Get<BR>Time</TH><TH>Remove<BR>Time</TH></TR>\n");
   dataBuf = (char *) malloc(OBJSIZE + 2);

   /*  HOSTNAME PORTNUMBER SRBUSER SRBUSERDOMAIN SRBAUTH TITLE  RSRC1 RSRC2 ...  RSRC[N<20] */
   while (fgets(inBuf,1000,fd) != NULL) {
     if (inBuf[0] == '#')        /* comment */
       continue;
     tmpPtr = inBuf;
     inBuf[strlen(inBuf) + 1] = '\0';
     inBuf[strlen(inBuf) + 2] = '\0';

     m = 0;
     while (tmpPtr != NULL && strlen(tmpPtr) > 0) {
       while (isspace(*tmpPtr))
	 tmpPtr++;
       if (*tmpPtr == '\'') {
	 tmpPtr++;
	 tmpPtr1 = tmpPtr;
	 while (*tmpPtr1 != '\0' && *tmpPtr1 != '\'')
	   tmpPtr1++;
	 *tmpPtr1 = '\0';
       }
       else {
	 tmpPtr1 = tmpPtr;
         while (*tmpPtr1 != '\0' && !isspace(*tmpPtr1))
           tmpPtr1++;
	   *tmpPtr1 = '\0';
       }
       if (m == 0)
	 strcpy(hostAddr, tmpPtr);
       else if (m == 1)
	 strcpy(portnum,tmpPtr);
       else if (m == 2)
	 strcpy(srbuser,tmpPtr);
       else if (m == 3)
         strcpy(srbuserdomain,tmpPtr);
       else if (m == 4)
         strcpy(srbauth,tmpPtr);
       else if (m == 5)
	 strcpy(h1title,tmpPtr);
       else 
	 strcpy(rsrcName[m - 6], tmpPtr);
       tmpPtr = tmpPtr1 + 1;
       m++;
     }  /* end of each line read */
       printf("<TR><TD NOWRAP>%s</TD><TD NOWRAP>%s:%s</TD>\n",h1title,hostAddr,portnum);
       if (m < 4)
	 continue;
       pingAll(m-6);
       printf("</TR>\n"); fflush(stdout);
   }  /* end processing each line */

   printf("</table><BR>\n");
   printf("<FONT COLOR=#FF0000>Note:</FONT><FONT COLOR=#0000FF>Not all resources in a zone are tested.</FONT><BR>\n");
   printf("<FONT COLOR=#FF0000>Note:</FONT><FONT COLOR=#0000FF>All timing values are rounded down.</FONT>\n");
   printf("</body></html>\n");
   fflush(stdout);
   fclose(fd);
   free(dataBuf);
   exit(0);
}


int
pingAll(int m) 
{

  int port, i,ii, cnt, jj, status,nn;
  long long  int1, int2;
  char *tmp1, *tmpPtr;
  time_t mm1, mm2, mmtime,mm3,mm4,mm5;
  char dataName[MAX_TOKEN];
  char sysLine[MAX_TOKEN];
  char systemStr[MAX_TOKEN];
  FILE *tmpFile;
  char svrVersion[MAX_TOKEN];
  char comStr[MAX_TOKEN],argStr[MAX_TOKEN];
  char cstr[8][1000];
  FILE *upFile;
  char *t1;
  int upFileFlag = 0;
  for (ii = 0;  ii < 8 ; ii++)
    strcpy(cstr[ii],"<td NOWRAP><FONT COLOR=#0000FF>");

  port = atoi(portnum);
  sprintf(envstr,"srbUser=%s", srbuser);
  putenv(envstr);
  sprintf(envstr2,"mdasDomainName=%s", srbuserdomain );
  putenv(envstr2);
  putenv(envstr2);
  sprintf(envstr3,"srbPort=%i", port );
  putenv(envstr3);
  sprintf(envstr4,"srbHost=%s", hostAddr );
  putenv(envstr4);
  mm1 = time(NULL);
  sprintf(comStr,"%s.txt",h1title);
  t1 = comStr;
  while (*t1 != '\0') {
    if (!isalnum(*t1))
      *t1 = '_';
    t1++;
  }
  upFile = fopen(comStr,"r+");
  if (upFile ==NULL) {
    upFile = fopen(comStr,"w+");
    upFileFlag = 0;  /* new file */
  }
  else {
    fgets(argStr,MAX_TOKEN-1, upFile);
    if (strstr(argStr,"DOWN") == argStr)
      upFileFlag = 2;
    else if (strstr(argStr,"UP") == argStr)
      upFileFlag = 1;
    else 
      upFileFlag = 0;
  }
  conn = clConnect (hostAddr, NULL, srbauth);
  if (clStatus(conn) != CLI_CONNECTION_OK) {
    if (upFileFlag != 2) {
      fseek(upFile,0L,SEEK_SET);
      sprintf(argStr,"DOWN FROM %s",ctime(&mm1));
      fprintf(upFile,"%s\n",argStr);
    }
    fclose(upFile);
    printf("<td colspan=6><FONT COLOR=#FF0000>DOWN: %s<BR>%s</font></td>\n",
	   clErrorMessage(conn),argStr);
    clFinish(conn);
    return -3;
  }
  if (upFileFlag != 1) {
      fseek(upFile,0L,SEEK_SET);
      sprintf(argStr,"UP FROM %s",ctime(&mm1));
      fprintf(upFile,"%s\n",argStr);
  }
  fclose(upFile);
  printf ("<td NOWRAP><FONT COLOR=#0000FF>%s</font></td>\n",argStr);
  mm2 = time(NULL);
  mmtime = mm2 - mm1;
  i = srbGetSvrVersion (conn, svrVersion);
  if (i == 0) 
    printf ("<td NOWRAP><FONT COLOR=#0000FF>%s</font></td>\n", svrVersion);
  else {
    printf("<td colspan=6><FONT COLOR=#FF0000>Version Unknown</font></td>\n");
    clFinish(conn);
    return -3;
  }
  printf("<TD NOWRAP><FONT COLOR=#0000FF> %ld secs.</FONT></TD>\n",mmtime);
  fflush(stdout);

  for (ii = 0; ii < m ; ii++) {
    /*
        printf("<table border=2 cellspacing=2 cellpadding=2 bgcolor=#FFFFFF><tr><th><FONT COLOR=#FF0000>Resource</FONT></th><th><FONT COLOR=#FF0000>Location</FONT></th><th><FONT COLOR=#FF0000>Resource Type</FONT></th><th><FONT COLOR=#FF0000>Create Time <BR> (secs)</FONT></th>\
<th><FONT COLOR=#FF0000>Write <BR> %i Bytes <BR> (secs)</FONT></th><th><FONT COLOR=#FF0000>Open Time <BR> (secs)</FONT></th><th><FONT COLOR=#FF0000>Read <BR> %i Bytes <BR> (secs)</FONT></th><th><FONT COLOR=#FF0000>Unlink Time<BR> (secs)</FONT></th><th><FONT COLOR=#FF0000>Resource Space Statistics</FONT></th><th><FONT COLOR=#FF0000>SRB Master/Server Process Statistics</FONT></th><th><FONT COLOR=#FF0000>Ping Statistics<BR> %i Bytes </FONT></th><th><FONT COLOR=#FF0000>Traceroute Statistics<BR> %i Bytes </FONT></th></tr>\n",OBJSIZE,OBJSIZE,OBJSIZE/2,OBJSIZE/2);fflush(stdout);
    */
    sprintf(dataName,"%s.%s.%s",OBJNAME,rsrcName[ii],hostAddr);
    srbObjUnlink(conn, dataName,COLLNAME);
    if (strlen(rsrcName[ii]) == 0)
      continue;
    if (ii == 0)
      sprintf(cstr[0],"%s %s",cstr[0],rsrcName[ii]);
    else
      sprintf(cstr[0],"%s <BR>%s",cstr[0],rsrcName[ii]);
    mm1 = time(NULL);
    out_fd = srbObjCreate (conn, 0,
                           dataName,OBJTYPE,rsrcName[ii],COLLNAME,"",OBJSIZE);
    
    if (out_fd < 0) {
      if (ii == 0)
	sprintf(cstr[1],"%s <FONT COLOR=#FF0000>Create Error %i</FONT>",cstr[1],out_fd);
      else
	sprintf(cstr[1],"%s <BR><FONT COLOR=#FF0000>Create Error %i</FONT>",cstr[1],out_fd);
      srbObjClose(conn, out_fd);
      srbObjUnlink(conn, dataName,COLLNAME);
      continue;
    }
    mm3 = time(NULL);
    i = srbObjWrite(conn, out_fd, dataBuf,OBJSIZE);
    if (i != OBJSIZE) {
      if (ii == 0)
        sprintf(cstr[1],"%s <FONT COLOR=#FF0000>Write Error %i</FONT>",cstr[1],i);
      else
        sprintf(cstr[1],"%s <BR><FONT COLOR=#FF0000>Write Error %i</FONT>",cstr[1],i);
      srbObjClose(conn, out_fd);
      srbObjUnlink(conn, dataName,COLLNAME);
      continue;
    }
    mm4 = time(NULL); 
    srbObjClose(conn, out_fd);
    mm2 = time(NULL);
    mmtime = mm2 - mm1;
    if (mmtime > 0) {
      if (ii == 0)
	sprintf(cstr[1],"%s %ld secs<IMG SRC=\"commentsbut.gif\" ALT=\"Create=%ld secs Write=%ld secs Close=%ld secs\" width=18 height=18 border=0>",cstr[1],mmtime,mm3-mm1,mm4-mm3,mm2-mm4);
      else
	sprintf(cstr[1],"%s <BR>%ld secs<IMG SRC=\"commentsbut.gif\" ALT=\"Create=%ld secs Write=%ld secs Close=%ld secs\" width=18 height=18 border=0>",cstr[1],mmtime,mm3-mm1,mm4-mm3,mm2-mm4);
    }
    else {
      if (ii == 0)
        sprintf(cstr[1],"%s %ld secs",cstr[1],mmtime);
      else
	sprintf(cstr[1],"%s <BR>%ld secs",cstr[1],mmtime);
    }
    mm1 = time(NULL);
    in_fd = srbObjOpen (conn, dataName,  O_RDONLY,COLLNAME);
    if (in_fd < 0) {
      if (ii == 0)
        sprintf(cstr[2],"%s <FONT COLOR=#FF0000>Open Error %i</FONT>",cstr[2],in_fd);
      else
        sprintf(cstr[2],"%s <BR><FONT COLOR=#FF0000>Open Error %i</FONT>",cstr[2],in_fd);
      srbObjClose(conn, out_fd);
      srbObjUnlink(conn, dataName,COLLNAME);
      continue;
    }
    mm3 = time(NULL);
    i = srbObjRead(conn, in_fd, dataBuf, OBJSIZE);
    if (i != OBJSIZE) {
      if (ii == 0)
        sprintf(cstr[2],"%s <FONT COLOR=#FF0000>Read Error %i</FONT>",cstr[2],i);
      else
        sprintf(cstr[2],"%s <BR><FONT COLOR=#FF0000>Read Error %i</FONT>",cstr[2],i);
      srbObjClose(conn, in_fd);
      srbObjUnlink(conn, dataName,COLLNAME);
      continue;
    }
    mm4 = time(NULL);
    srbObjClose(conn, in_fd);
    mm2 = time(NULL);
    mmtime = mm2 - mm1;
    if (mmtime > 0) {
      if (ii == 0)
	sprintf(cstr[2],"%s %ld secs<IMG SRC=\"commentsbut.gif\" ALT=\"Open=%ld secs Read=%ld secs Close=%ld secs\" width=18 height=18 border=0>",cstr[2],mmtime,mm3-mm1,mm4-mm3,mm2-mm4);
      else
	sprintf(cstr[2],"%s <BR>%ld secs<IMG SRC=\"commentsbut.gif\" ALT=\"Open=%ld secs Read=%ld secs Close=%ld secs\" width=18 height=18 border=0>",cstr[2],mmtime,mm3-mm1,mm4-mm3,mm2-mm4);
    }
    else {
      if (ii == 0)
        sprintf(cstr[2],"%s %ld secs",cstr[2],mmtime);
      else
	sprintf(cstr[2],"%s <BR>%ld secs",cstr[2],mmtime);
    }


    mm1 = time(NULL);
    i = srbObjUnlink(conn, dataName,COLLNAME);
    if (i < 0) {
      if (ii == 0)
        sprintf(cstr[3],"%s <FONT COLOR=#FF0000>Unlink Error %i</FONT>",cstr[3],i);
      else
        sprintf(cstr[3],"%s <BR><FONT COLOR=#FF0000>Unlink Error %i</FONT>",cstr[3],i);
      continue;
    }
    mm2 = time(NULL);
    mmtime = mm2 - mm1;
    if (ii == 0)
      sprintf(cstr[3],"%s %ld secs",cstr[3],mmtime);
    else
      sprintf(cstr[3],"%s <BR>%ld secs",cstr[3],mmtime);
  }

  for (ii = 0;  ii < 8 ; ii++)
    sprintf(cstr[ii],"%s </FONT></td>\n", cstr[ii]);
  printf("%s %s %s %s", cstr[0],cstr[1],cstr[2],cstr[3]);
  
  return(0);
}

