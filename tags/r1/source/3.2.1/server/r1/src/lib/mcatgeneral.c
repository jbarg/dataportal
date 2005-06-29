/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* srbC_nonmdas_library,c -
 * These are routines that are needed by SRB server and client that are not
 * MDAS enabled.
 */

#include "mdasC_db2_externs.h"
#include "srbC_mdas_externs.h"
#include "mdasGlobalsExtern.h"
#include "elogExtern.h"

int getFullPathName(char *defPathName,  char *phyDefPathName,
                    char *userName, char *domainName,
                    char *givenPathName, char *dataName,
                    char *collectionName,
                    char fullPathName[HUGE_STRING]);
void unuralize(char *data);
void spacetounderscore(char *str);
void plustospace(char *str);
void unescape_url(char *url);
char x2c(char *what);
void uralize(char data[], char datastring[]);
void get_time(char tval[]);

time_t calPtr;
struct tm *mytime;

void splitbychar(char *inname, char *collname, char *dataname, char spchar)
{
  int cnt; 
  char cval[MAX_TOKEN], dname[MAX_TOKEN];
  int i,j;
  cnt = strlen(inname);  
  j = 0;
  dname[0] =  '\0';
  cval[0] =  '\0';
  for (i = cnt - 1; i >= 0 ; i--)
    {
      if (inname[i] == spchar) break;
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
  strcpy(dataname,dname);
}



void splitbycharwesc(char *inname, char *collname, char *dataname,
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

int getFullPathName(char *defPathName,  char *phyDefPathName,
                    char *userName, char *domainName,
                    char *givenPathName, char *dataName,
                    char *collectionName,
                    char fullPathName[])


{
  char usableDefPathName[HUGE_STRING];
  char cval[HUGE_STRING];
  int i,j,k,l,m,dollar_num;
  char dollar_val[MAX_DOLLAR_VAL_CNT][MAX_TOKEN];
  char userPathName[HUGE_STRING];
  char tempstr[HUGE_STRING], gPN[HUGE_STRING],gPN1[HUGE_STRING];
  char shadowPath[HUGE_STRING];
  char *tmpPtr, *tmpPtr1;
  char c;
  shadowPath[0] = '\0';
  for (i = 0; i < MAX_DOLLAR_VAL_CNT; i++)   dollar_val[i][0]='\0';
  /*
  if(givenPathName[0] != '?')
    strcpy(userPathName,givenPathName);
  else
    {
      userPathName[0]='\0';
      strcpy(gPN,givenPathName);
      while (strlen(gPN) > 0)
        {
          splitbycharwesc(gPN,gPN1,tempstr,'?','\\');
          if(isdigit(tempstr[0]) && isdigit(tempstr[1]) && tempstr[2] == '=')
            {
              sprintf(cval, "%c%c",tempstr[0],tempstr[1]);
              dollar_num=atoi(cval);
              strcpy(dollar_val[dollar_num],(char *) &tempstr[3]);
            }
          else if (strstr(tempstr,"PATH=") == (char *) &tempstr)
            strcpy(userPathName,(char *) &tempstr[5]);
         else if (strstr(tempstr,"SHADOW=") == (char *) &tempstr)
            strcpy(shadowPath,(char *) &tempstr[7]);
          else return(ERROR_IN_GIVEN_PATH);
          strcpy(gPN,gPN1);
        }
    }
   */
  if(givenPathName[0] != '&')
    strcpy(userPathName,givenPathName);
  else
    {
      userPathName[0]='\0';
      strcpy(gPN,givenPathName);
      while (strlen(gPN) > 0)
        {
          splitbycharwesc(gPN,gPN1,tempstr,'&','\\');
          if(isdigit(tempstr[0]) && isdigit(tempstr[1]) && tempstr[2] == '=')
            {
              sprintf(cval, "%c%c",tempstr[0],tempstr[1]);
              dollar_num=atoi(cval);
              strcpy(dollar_val[dollar_num],(char *) &tempstr[3]);
            }
          else if (strstr(tempstr,"PATH=") == (char *) &tempstr)
            strcpy(userPathName,(char *) &tempstr[5]);
         else if (strstr(tempstr,"SHADOW=") == (char *) &tempstr)
            strcpy(shadowPath,(char *) &tempstr[7]);
/*          else return(ERROR_IN_GIVEN_PATH); */
          strcpy(gPN,gPN1);
        }
    }



        if (givenPathName[0] == '/')
          {
            strcpy( fullPathName, givenPathName);
            return(0);
          }
        if (defPathName[0] == '/')
          strcpy(usableDefPathName,defPathName);
        else
          {
            if(defPathName[0] == '\\' && defPathName[1] == '/')
              sprintf(usableDefPathName,"%s%s", phyDefPathName,
                      (char *) &defPathName[1]);
            else
              sprintf(usableDefPathName,"%s%s", phyDefPathName,
                      defPathName);
          }


        l = strlen(usableDefPathName);

        for(i=0,k=0; i<l ; i++)
          {

           if(usableDefPathName[i] == '?')
             {
               if(strstr((char *) &usableDefPathName[i+1],"PATH") ==
                  (char *) &usableDefPathName[i+1])
                 {
		   uralize(userPathName,cval); 
                   sprintf((char *)&fullPathName[k],"%s",cval);
                   k = k+strlen(cval);
                   i=i+4;
                 }
               else if(strstr((char *) &usableDefPathName[i+1],"DATANAME") ==
                  (char *) &usableDefPathName[i+1])
                 {
                   sprintf((char *)&fullPathName[k],"%s",dataName);
                   k = k+strlen(dataName);
                   i=i+8;
                 }
               else if(strstr((char *)(&usableDefPathName[i+1]),"USER") ==
                  (char *)  &usableDefPathName[i+1])
                 {
                   sprintf(&fullPathName[k],"%s",userName);
                   k = k+strlen(userName);
                   i=i+4;
                 }
               else if(strstr((char *)(&usableDefPathName[i+1]),"OWNER") ==
                  (char *)  &usableDefPathName[i+1])
                 {
                   sprintf(&fullPathName[k],"%s",userName);
                   k = k+strlen(userName);
                   i=i+5;
                 }
               else if(strstr((char *) &usableDefPathName[i+1],"DOMAIN") ==
                  (char *) &usableDefPathName[i+1])
                 {
                   sprintf((char *) &fullPathName[k],"%s",domainName);
                   k = k+strlen(domainName);
                   i=i+6;
                 }
               else if(strstr((char *)&usableDefPathName[i+1],"COLLECTION") ==
                  (char *) &usableDefPathName[i+1])
                 {
                   sprintf((char *) &fullPathName[k],"%s",collectionName);
                   k = k+strlen(collectionName);
                   i=i+10;
                 }
               else if(strstr((char *) &usableDefPathName[i+1],"PATH") ==
                  (char *) &usableDefPathName[i+1])
                 {
                   sprintf((char *) &fullPathName[k],"%s",userPathName);
                   k = k+strlen(userPathName);
                   i=i+4;
                 }
               else if(strstr((char *) &usableDefPathName[i+1],"SHADOW") ==
                  (char *) &usableDefPathName[i+1])
                 {
                   sprintf((char *) &fullPathName[k],"%s",shadowPath);
                   k = k+strlen(shadowPath);
                   i=i+6;
                 }
               else if(strstr((char *) &usableDefPathName[i+1],"PROCESSID") ==
                  (char *) &usableDefPathName[i+1])
                 {
                   sprintf(cval,"%ld",getpid());
                   sprintf((char *) &fullPathName[k],"%s",cval);
                   k = k+strlen(cval);
                   i=i+9;
                 }
               else if(strstr((char *) &usableDefPathName[i+1],"TIMESEC") ==
                  (char *) &usableDefPathName[i+1])
                 {
                   sprintf(cval,"%ld",(long) time(NULL));
                   sprintf((char *) &fullPathName[k],"%s",cval);
                   k = k+strlen(cval);
                   i=i+7;
                 }
               else if(strstr((char *) &usableDefPathName[i+1],"DATE") ==
                  (char *) &usableDefPathName[i+1])
                 {
		   time(&calPtr);
		   mytime = localtime(&calPtr);
		   /*** 0 added to 03d KESTOCK 01/24/2001 ****/
                   sprintf(cval,"%4d/%03d",  
			   mytime->tm_year+1900, mytime->tm_yday+1);
                   sprintf((char *) &fullPathName[k],"%s",cval);
                   k = k+strlen(cval);
                   i=i+4;
                 }
               else if(strstr((char *) &usableDefPathName[i+1],"RANDOM") ==
                  (char *) &usableDefPathName[i+1])
                 {
                   sprintf(cval,"%ld",(long) random());
                   sprintf((char *) &fullPathName[k],"%s",cval);
                   k = k+strlen(cval);
                   i=i+6;
                 }
               else if(strstr((char *) &usableDefPathName[i+1],"SPLITPATH") ==
                  (char *) &usableDefPathName[i+1])
                 {
                   sprintf(cval,"%i/%i",(((int) random()) % 99),(((int) random()) % 99));
                   sprintf((char *) &fullPathName[k],"%s",cval);
                   k = k+strlen(cval);
                   i=i+9;
                 }
               else if(strstr((char *) &usableDefPathName[i+1],"TIME") ==
                  (char *) &usableDefPathName[i+1])
                 {
                   get_time(timeval);
                   sprintf((char *) &fullPathName[k],"%s",timeval);
                   k = k+strlen(timeval);
                   i=i+4;
                 }
	       else if(strstr((char *) &usableDefPathName[i+1],"DIRCHOP=") ==
                  (char *) &usableDefPathName[i+1])
		 {
		   i = i+8;
		   j = i+1;
		   while (isdigit(usableDefPathName[i+1]) )
		     i++;
		   c = usableDefPathName[i+1];
		   usableDefPathName[i+1] = '\0';
		   m = 0;
		   m = atoi((char *) &usableDefPathName[j]);
		   m++;
		   usableDefPathName[i+1] = c;
		   tmpPtr = collectionName;
		   while (m) {
		     if ((tmpPtr1 = strstr(tmpPtr,"/")) != NULL) {
		       m--;
		       tmpPtr = tmpPtr1+1;
		     }
		     else
		       break;
		   }
		   sprintf(gPN1,"%s/%s",tmpPtr,dataName);
		   sprintf((char *) &fullPathName[k],"%s",gPN1);
		   k = k+strlen(gPN1);
		 }
               else if(isdigit(usableDefPathName[i+1]) &&
                       isdigit(usableDefPathName[i+2]))
                 {
                   sprintf(cval, "%c%c",usableDefPathName[i+1],
                           usableDefPathName[i+2]);
                   dollar_num=atoi(cval);
                   sprintf((char *) &fullPathName[k],"%s",dollar_val[dollar_num]
);
                   k = k+strlen(dollar_val[dollar_num]);
                   i=i+2;
                 }
               else return(ERROR_IN_DEFAULT_PATH);
             }
           else
             {
               fullPathName[k] = (char) usableDefPathName[i];
               k++;

             }
          }
        fullPathName[k] = '\0';
        unuralize(fullPathName);    
	/*****     spacetounderscore(fullPathName); ****/
		     /*** removed May 13,99    ****/
    return(0);
}

void unuralize(char *data)
{
  plustospace(data);
  unescape_url(data);
}
void spacetounderscore(char *str) {
     register int x;

     for(x=0;str[x];x++) if(str[x] == ' ') str[x] = '_';
 }

void plustospace(char *str) {
     register int x;

     for(x=0;str[x];x++) if(str[x] == '+') str[x] = ' ';
 }

void unescape_url(char *url) {
     register int x,y;

     for(x=0,y=0;url[y];++x,++y) {
         if(((url[x] = url[y]) == '%') && isxdigit(url[y+1]) && isxdigit(url[y+2])) {
             url[x] = x2c(&url[y+1]);
             y+=2;
         }
     }
     url[x] = '\0';
 }

char x2c(char *what) {
     register char digit;

     digit = (what[0] >= 'A' ? ((what[0] & 0xdf) - 'A')+10 : (what[0] - '0'));
     digit *= 16;
     digit += (what[1] >= 'A' ? ((what[1] & 0xdf) - 'A')+10 : (what[1] - '0'));
     return(digit);
 }

void uralize(char data[], char datastring[])
{
  int i,j,k;
  i = 0; j = 0;
  
  while (i < strlen(data))
    {
      if (data[i] == ' ')
        datastring[j] = '+';
      else if (data[i] == '#')
        {datastring[j] = '%';
         datastring[++j] = '2';
         datastring[++j] = '3';
        }
      else if (data[i] == ';')
        {datastring[j] = '%';
         datastring[++j] = '3';
         datastring[++j] = 'B';
        }
      else if (data[i] == '\'')
        {datastring[j] = '%';
         datastring[++j] = '2';
         datastring[++j] = '7';
        }
     else if (data[i] == '&')
        {datastring[j] = '%';
         datastring[++j] = '2';
         datastring[++j] = '6';
        }
      else if (data[i] == '%')
        {datastring[j] = '%';
         datastring[++j] = '2';
         datastring[++j] = '5';
        }
      else if (data[i] == '+')
        {datastring[j] = '%';
         datastring[++j] = '2';
         datastring[++j] = 'B';
        }
      else
        datastring[j] = data[i];
      i++;
      j++;
    }
    datastring[j] = data[i];

}

void get_time(char tval[])
{
     time_t mmmmcl;
     char t[25], r[4];
     int j,k;
     int flag = 0;
     /*     Thu Jan 23 16:10:51 1997 to 1997-01-23-16.10.51*/
     /*     01234567890123456789        0123456789012345678*/
     /* flag value                      9012    12 34 56 78*/
     /*                                  111              */

   mmmmcl = time(NULL);
   sprintf(t,"%24.24s",ctime(&mmmmcl));
   strncpy( r, &t[4], 3); r[3] ='\0';
   if      (!strcmp(r,"Jan")) {tval[5] = '0'; tval[6] = '1';}
   else if (!strcmp(r,"Feb")) {tval[5] = '0'; tval[6] = '2';}
   else if (!strcmp(r,"Mar")) {tval[5] = '0'; tval[6] = '3';}
   else if (!strcmp(r,"Apr")) {tval[5] = '0'; tval[6] = '4';}
   else if (!strcmp(r,"May")) {tval[5] = '0'; tval[6] = '5';}
   else if (!strcmp(r,"Jun")) {tval[5] = '0'; tval[6] = '6';}
   else if (!strcmp(r,"Jul")) {tval[5] = '0'; tval[6] = '7';}
   else if (!strcmp(r,"Aug")) {tval[5] = '0'; tval[6] = '8';}
   else if (!strcmp(r,"Sep")) {tval[5] = '0'; tval[6] = '9';}
   else if (!strcmp(r,"Oct")) {tval[5] = '1'; tval[6] = '0';}
   else if (!strcmp(r,"Nov")) {tval[5] = '1'; tval[6] = '1';}
   else if (!strcmp(r,"Dec")) {tval[5] = '1'; tval[6] = '2';}
   flag = 0;
   for (j = 6; j <25 ; j++) {
     if ((t[j] == ' '|| t[j] == ':') && j != 8 ) {  flag++;    }
     else {
       switch (flag) {
       case 1 : if (t[j] == ' ') {tval[ 8] = '0'; tval[7] = '-'; flag++;}
                else {tval[ 8] = t[j]; tval[7] = '-'; flag++;}
                break;
       case 2 : tval[ 9] = t[j]; tval[10] = '-'; break;
       case 3 : tval[11] = t[j]; flag++; break;
       case 4 : tval[12] = t[j]; tval[13] = '.'; break;
       case 5 : tval[14] = t[j]; flag++; break;
       case 6 : tval[15] = t[j]; tval[16] = '.'; break;
       case 7 : tval[17] = t[j]; flag++; break;
       case 8 : tval[18] = t[j]; tval[19] = '\0'; break;
       case 9 : tval[ 0] = t[j]; flag++; break;
       case 10: tval[ 1] = t[j]; flag++; break;
       case 11: tval[ 2] = t[j]; flag++; break;
       case 12: tval[ 3] = t[j]; tval[4] = '-'; flag++;break;
       default: break;
       }
     }
   }
}


int set_qval_data_cond(char *in_data_name, char qval[][MAX_TOKEN])
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
          splitbycharwesc(st, st1, st2, '&' , '\\');
	  
          tt = (char *) st2;
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
	      make_like_for_mdas_sr(attrCond);
	      strcat (attrCond, "' ESCAPE '\\");
	    }
	    if (attrCntD == 0) {
	      if (make_like_for_mdas_sr(attrName) == 1)
		sprintf(qval[UDSMD0_0]," like  '%s' ESCAPE '\\'",attrName);
	      else
		sprintf(qval[UDSMD0_0]," = '%s'",attrName);
	      sprintf(qval[UDSMD1_0]," %s '%s'", attrOp,attrCond); 
	      attrCntD++;
	    }
	    else if (attrCntD > 0 && attrCntD < 5) {
	      if (make_like_for_mdas_sr(attrName) == 1)
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
	      make_like_for_mdas_sr(attrCond);
	      strcat (attrCond, "' ESCAPE '\\");
	    }
	    if (attrCntC == 0) {
	      if (make_like_for_mdas_sr(attrName) == 1)
		sprintf(qval[UDSMD_COLL0_0]," like  '%s' ESCAPE '\\'",attrName);
	      else
		sprintf(qval[UDSMD_COLL0_0]," = '%s'",attrName);
	      sprintf(qval[UDSMD_COLL1_0]," %s '%s'", attrOp,attrCond); 
	      attrCntD++;
	    }
	    else if (attrCntC > 0  &&  attrCntC < 5) {
	      if (make_like_for_mdas_sr(attrName) == 1)
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
	      make_like_for_mdas_sr(attrCond);
	      strcat (attrCond, "' ESCAPE '\\");
	    }
	    if (attrCntU == 0) {
	      if (make_like_for_mdas_sr(attrName) == 1)
		sprintf(qval[UDSMD_USER0_0]," like  '%s' ESCAPE '\\'",attrName);
	      else
		sprintf(qval[UDSMD_USER0_0]," = '%s'",attrName);
	      sprintf(qval[UDSMD_USER1_0]," %s '%s'", attrOp,attrCond); 
	      attrCntD++;
	    }
	    else if (attrCntU > 0  &&  attrCntU < 4) {
	      if (make_like_for_mdas_sr(attrName) == 1)
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
	      make_like_for_mdas_sr(attrCond);
	      strcat (attrCond, "' ESCAPE '\\");
	    }
	    if (attrCntR == 0) {
	      if (make_like_for_mdas_sr(attrName) == 1)
		sprintf(qval[UDSMD_RSRC0_0]," like  '%s' ESCAPE '\\'",attrName);
	      else
		sprintf(qval[UDSMD_RSRC0_0]," = '%s'",attrName);
	      sprintf(qval[UDSMD_RSRC1_0]," %s '%s'", attrOp,attrCond); 
	      attrCntD++;
	    }
	    else if (attrCntR > 0  &&  attrCntR < 4) {
	      if (make_like_for_mdas_sr(attrName) == 1)
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


splitAttrOpCond(char *inname, char *aname, char *aop, char *acond)
{
  int i,j,cnt;
  char *tp, *tp1, *tp2;

  tp = inname;
  while (*tp == ' ') tp++;
  if (*tp == '\'') {
    tp++;
    tp1 = tp;
    while (*tp != '\'') tp++;
    *tp = '\0';
    strcpy(aname, tp1);
    *tp = '\'';
  }
  else   if (*tp == '"') {
    tp++;
    tp1 = tp;
    while (*tp != '"') tp++;
    *tp = '\0';
    strcpy(aname, tp1);
    *tp = '"';
  }
  else {
    tp1 = tp;
    while (*tp != ' ') tp++;
    *tp = '\0';
    strcpy(aname, tp1);
    *tp = ' ';
  }
  tp++;
  while (*tp == ' ') tp++;
  if (*tp == '\'') {
    tp++;
    tp1 = tp;
    while (*tp != '\'') tp++;
    *tp = '\0';
    strcpy(aop, tp1);
    *tp = '\'';
  }
  else   if (*tp == '"') {
    tp++;
    tp1 = tp;
    while (*tp != '"') tp++;
    *tp = '\0';
    strcpy(aop, tp1);
    *tp = '"';
  }
  else {
    tp1 = tp;
    while (*tp != ' ') tp++;
    *tp = '\0';
    strcpy(aop, tp1);
    *tp = ' ';
  }
  tp++;
  while (*tp == ' ') tp++;
  if (*tp == '\'') {
    tp++;
    tp1 = tp;
    while (*tp != '\'') tp++;
    *tp = '\0';
    strcpy(acond, tp1);
    *tp = '\'';
  }
  else   if (*tp == '"') {
    tp++;
    tp1 = tp;
    while (*tp != '"') tp++;
    *tp = '\0';
    strcpy(acond, tp1);
    *tp = '"';
  }
  else {
    tp1 = tp;
    while (*tp != ' ') tp++;
    *tp = '\0';
    strcpy(acond, tp1);
    *tp = ' ';
  }
  
  
}
int make_like_for_mdas_sr(char *dname)
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

int
checkIfUserAllowedBulkQuery(char *queryInfo)
{
  if (strstr(queryInfo,"GETFORUSER") == queryInfo)
    return(0);
  else
    return(-1);
}

