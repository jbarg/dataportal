/*
  These routines obfuscate (sort-of encrypt) passwords.

  These are used to avoid storing plain-text passwords in the
  .MdasAuth files, particularly for server to server
  authentication. This is better than plain-text passwords sitting
  around in files (or plain-text credentials in GSI), and often being
  transmitted on networks in NFS packets.  But, of course, this isn't
  highly secure, particularly since it depends on the secrecy of this
  source file.  But it does make it more difficult to defeat.

  Externally callable routines begin with "obf" and internal routines
  begin with "obfi".

  A timestamp is used with this, which will be gotten from the 
  modify time of the .srbAuthFile file.  This way, a simple copy of
  the file will not successfully authenticate.

  The UID of the running process is also used in the algorithm.

  Optionally, users can also define "SRB_AUTH_KEY" to any string.  If
  this is defined it will be used in the obfuscation/de-obfuscation
  algorithm and so always needs to be the same for that user.

  The same password is obfuscated differently (usually), each time the
  obfuscation is done, to help defeat someone trying to determine our
  obfuscation algorithm.  Also, one will usually be unable to tell by
  looking at the ciphertext whether two passwords are the same or not.

  When the Scommands authenticate, they first check if this alternative
  to MdasAuth is available, and use that if possible.

  Thus (for now) it is entirely optional for the user (later, it could
  become a site configuration option).  This will be particularly
  useful for the SRB user running the server.

  See the Sauth man page to for more information.


  Wayne Schroeder, 9/2004 (based on some old test 
  code I had written in '96) */

#include <stdio.h>
#ifndef _WIN32
#include <sys/types.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif
#include <stdlib.h>

#include <clAuthExtern.h>

#ifdef _WIN32
#include "Unix2Nt.h"
#endif


int obfDebug=0;
int timeVal=0;

int obfGetPw(char *pw);
int obfSavePw(int promptOpt, int fileOpt, int printOpt, char *pwArg);
int obfiGetEnvKey();
int obfiGetTv(char *fileName);
int obfiDecode(char *in, char *out, int extra);
int obfiGetPw(char *fileName, char *pw);
int obfiOpenOutFile(char *fileName, int fileOpt);
int obfiWritePw(int fd, char *pw);
void obfiEncode(char *in, char *out, int extra);

/* 
  What can be a main routine for some simple tests.
 */
int
obftestmain(argc, argv)
int argc;
char *argv[];
{
   char p3[MAX_TOKEN];
   int i;

   obfDebug = 2;

   if (argc < 2) {
     printf("Usage: -d|-e\n");
     exit(-1);
   }

   if (strcmp(argv[1],"-d")==0) {
     i = obfGetPw(p3);
     if (obfDebug)  printf("val  = %d \n",i);
   }

   if (strcmp(argv[1],"-e")==0) {
     i = obfSavePw(1, 0, 1, "");
     if (obfDebug)  printf("val  = %d \n",i);
   }
   return (0);
}

int 
obfSetDebug(int opt)
{
  obfDebug = opt;
  return (0);
}

int
obfiGetFilename(fileName)
char *fileName;
{
  char *envVar;

  envVar = getenv(AUTH_FILENAME_ENV_VAR_NAME);
  if (envVar == NULL) {
#ifdef _WIN32
        envVar =  get_home_env();
#else
        envVar =  getenv ("HOME");
#endif
        if (envVar == NULL) {
          return -1;
        }
        strcpy(fileName, envVar);
        strcat(fileName, "/");
        strcat(fileName, AUTH_FILENAME_DEFAULT);
  }
  else {
    strcpy(fileName,envVar);
  }
  return(0);
}

int
obfGetPw(char *pw)
{
  char myPw[MAX_TOKEN+10];
  char myPwD[MAX_TOKEN+10];
  char fileName[MAX_TOKEN+10];

  int i;
  int envVal;

  strcpy(pw, "");

  envVal = obfiGetEnvKey();

  i = obfiGetFilename(fileName);
  if (i != 0) return -1;

  i = obfiGetTv(fileName);
  if (i < 0) return(i-10);

  i = obfiGetPw(fileName, myPw);
  if (i < 0) return(i-20);

  i = obfiDecode(myPw, myPwD, envVal);
  if (i < 0) return(i-30);

  if (obfDebug) printf("out:%s\n",myPwD);
  strcpy(pw, myPwD);

  return 0;
}

int
obfSavePw(promptOpt, fileOpt, printOpt, pwArg)
     int promptOpt;  /* if 1, echo password as typed, else no echo */
     int fileOpt;    /* if 1, ask permission before writing file */
     int printOpt;   /* if 1, display an updated file msg on success */
     char *pwArg;    /* if non-0-length, this is the new password */
{
  char fileName[MAX_TOKEN+10];
  char inbuf[MAX_TOKEN+10];
  char myPw[MAX_TOKEN+10];
  int i, fd, envVal;
  struct stat statbuf;

  i = obfiGetFilename(fileName);
  if (i != 0) return -1;

  envVal = obfiGetEnvKey();

  fd = obfiOpenOutFile(fileName, fileOpt);
  if (fd < 0) return (fd-10);

  if (strlen(pwArg)==0) {
    if (promptOpt != 1) {
      if (stat ("/bin/stty", &statbuf) == 0)
        system("/bin/stty -echo");
    }

    printf("Enter your current SRB password:");
    fgets(inbuf, MAX_TOKEN, stdin);

    if (promptOpt != 1) {
      system("/bin/stty echo");
      printf("\n");
    }
  }
  else {
    strncpy(inbuf, pwArg, MAX_TOKEN);
  }
  i = strlen(inbuf);
  if (i < 2) return -2;
  if (inbuf[i-1]=='\n') inbuf[i-1]='\0';  /* remove trailing \n */
  
  obfiEncode(inbuf,myPw, envVal);
  if (obfDebug > 1) printf(" in:%s out:%s\n",inbuf, myPw);

  i = obfiWritePw(fd, myPw);
  if (i < 0) return (i-20);

  if (obfDebug || printOpt) printf("Successfully wrote %s\n",fileName);
  return 0;
}


int
obfiGetTv(fileName)
char *fileName;
{
   struct stat statBuf;
   int fval;

   fval = stat(fileName,&statBuf); 
   if (fval<0) {
      timeVal=0;
      return -1;
   } 

   timeVal = statBuf.st_mtime;
   timeVal = timeVal & 0xffff;      /* keep it bounded */
   return 0;
}

int
obfiGetPw(fileName, pw)
char *fileName;
char *pw;
{
   int fd_in, rval;
   char buf1[500];

   fd_in = open(fileName,O_RDONLY,0);
   if (fd_in<0) {
      fprintf(stderr,"input file open error\n");
      return -1;
   }
   
   rval = read(fd_in, buf1, 500);
   if (rval < 0) {
      fprintf(stderr,"read error\n");
      return -2;
   }
   if (strlen(buf1)< MAX_TOKEN) {
     strcpy(pw, buf1);
     return 0;
   }
   else {
     return -3;
   }

}


int 
obfiSavePw(pw, fileName)
char *pw;
char *fileName;
{
   int fd_out, wval, len, i;
   char inbuf[MAX_TOKEN];

   fd_out = open(fileName,O_CREAT|O_WRONLY|O_EXCL,0600);
   if (fd_out<0) {
     printf("Overwrite '%s'?:",fileName);
     fgets(inbuf, MAX_TOKEN, stdin);
     i = strlen(inbuf);
     if (i < 2) return -1;
     if (inbuf[0]=='y') {
       fd_out = open(fileName, O_CREAT|O_WRONLY|O_TRUNC, 0600);
       if (fd_out<0) return -2;
     }
     else {
       return -3;
     }
   }

   len = strlen(pw);
   wval = write(fd_out,pw,len+1);
   if (wval != len+1) {fprintf(stderr,"write error");}
   close(fd_out);
   return 0;
}

int 
obfiOpenOutFile(fileName, fileOpt)
char *fileName;
int fileOpt;
{
   char inbuf[MAX_TOKEN];
   int i, fd_out;

   fd_out = open(fileName,O_CREAT|O_WRONLY|O_EXCL,0600);
   if (fd_out<0) {
     if (errno != EEXIST) {
       return -4;
     }
     if (fileOpt > 0) {
       printf("Overwrite '%s'?:",fileName);
       fgets(inbuf, MAX_TOKEN, stdin);
       i = strlen(inbuf);
       if (i < 2) return -1;
     }
     else {
       strcpy(inbuf, "y");
     }
     if (inbuf[0]=='y') {
       fd_out = open(fileName, O_CREAT|O_WRONLY|O_TRUNC, 0600);
       if (fd_out<0) return -2;
     }
     else {
       return -3;
     }
   }
   return(fd_out);
}

int
obfiWritePw(fd, pw)
int fd;
char *pw;
{
   int wval, len;
   len = strlen(pw);
   wval = write(fd,pw,len+1);
   if (wval != len+1) {
     return -1;
   }
   close(fd);
   return 0;
}
 
int obfiTimeval()
{
   long sec;
   int val;  

#ifdef _WIN32
   SYSTEMTIME nowtime;
#else
   struct timeval nowtime;
#endif

#ifdef _WIN32
   GetSystemTime(&nowtime);
   sec = nowtime.wSecond;
#else
   (void)gettimeofday(&nowtime, (struct timezone *)0);
   sec = nowtime.tv_sec;
#endif

   val = sec;
   val = val & 0xffff;      /* keep it bounded */
   if (obfDebug > 1)  printf("val  = %d %x\n",val,val);
   return(val);
}


/*
  Obfuscate a password
*/
void
obfiEncode(in,out,extra)
char *in;
char *out;
int extra;
{
   int i;
   long seq;
   char *my_in;

   /*   struct timeb timeb_time; */
#ifdef _WIN32
   SYSTEMTIME nowtime;
#else
   struct timeval nowtime;
#endif
   int rval;
   int wheel_len;
   int wheel[26+26+10+15];
   int j, addin, addin_i, found;
   int ii;
   int now;
   char headstring[10];
#ifndef _WIN32
   int uid;
#endif
/*
 Set up an array of characters that we will transpose.
*/

   wheel_len=26+26+10+15;
   j=0;
   for (i=0;i<10;i++) wheel[j++]=(int)'0' + i;
   for (i=0;i<26;i++) wheel[j++]=(int)'A' + i;
   for (i=0;i<26;i++) wheel[j++]=(int)'a' + i;
   for (i=0;i<15;i++) wheel[j++]=(int)'!' + i;

/*
 get uid to use as part of the key
*/
#ifndef _WIN32
   uid = getuid();
   uid = uid &0xf5f;  /* keep it fairly small and not exactly uid */
#endif

/*
 get a pseudo random number
*/
#ifdef _WIN32
   GetSystemTime(&nowtime);
   rval = nowtime.wSecond & 0xf;
#else
   (void)gettimeofday(&nowtime, (struct timezone *)0);
   rval = nowtime.tv_usec & 0xf;
#endif

/*
 and use it to pick a pattern for ascii offsets
*/
   seq = 0;
   if (rval==0) seq = 0xd76aa478;
   if (rval==1) seq = 0xe8c7b756;
   if (rval==2) seq = 0x242070db;
   if (rval==3) seq = 0x698098d8;
   if (rval==4) seq = 0xc1bdceee;
   if (rval==5) seq = 0xf57c0faf;
   if (rval==6) seq = 0x4787c62a;
   if (rval==7) seq = 0xa8304613;
   if (rval==8) seq = 0x774696ea;
   if (rval==9) seq = 0x6720696e;
   if (rval==10)seq = 0x3a257320;
   if (rval==11)seq = 0x6f75743a;
   if (rval==12)seq = 0x73656320;
   if (rval==13)seq = 0x6f74202e;
   if (rval==14)seq = 0x203d640a;
   if (rval==15)seq = 0x6e6f742e;

/*
 get the timestamp and other id
*/
   now = obfiTimeval();
   headstring[1] = ((now>>4) & 0xf) + 'a';
   headstring[2] = (now & 0xf) + 'a';
   headstring[3] = ((now>>12) & 0xf) + 'a';
   headstring[4] = ((now>>8) & 0xf) + 'a';
   headstring[5] = '\0';
   headstring[0]='S' - ( (rval&0x7)*2);  /* another check value */

  *out++ = '.';          /* store our initial, human-readable identifier */

  addin_i=0;
  my_in = headstring;    /* start with head string */
  for (ii=0;;) {
     ii++;
     if (ii==6) {
        *out++ = rval + 'e';  /* store the key */
        my_in=in;     /* now start using the input string */
     }
     found=0;
     addin = (seq>>addin_i)&0x1f;
     addin += extra;
#ifndef _WIN32
     addin += uid;
#endif
     addin_i+=3;
     if (addin_i>28)addin_i=0;
     for (i=0;i<wheel_len;i++) {
        if (*my_in == (char)wheel[i]) {
           j = i + addin;
           if (obfDebug>1) printf("j1=%d ",j);
           j = j % wheel_len;
           if (obfDebug>1) printf ("j2=%d \n",j);
           *out++ = (char)wheel[j];
           found = 1;
           break;
        }
     }
     if (found==0) {
        if (*my_in == '\0') {
           *out++ = '\0'; 
           return;
        }
        else {*out++=*my_in;}
     }
     my_in++;
  }
}

int
obfiDecode(in, out, extra)
char *in;
char *out;
int extra;
{
   int i;
   long seq;
   char *p1;

   int rval;
   int wheel_len;
   int wheel[26+26+10+15];
   int j, addin, addin_i, kpos, found, nout;
   char headstring[10];
   int ii, too_short;
   char *my_out, *my_in;
   int not_en, encodedTime;
#ifndef _WIN32
   int uid;
#endif

   wheel_len=26+26+10+15;

/*
 get uid to use as part of the key
*/
#ifndef _WIN32
   uid = getuid();
   uid = uid &0xf5f;  /* keep it fairly small and not exactly uid */
#endif

/*
 Set up an array of characters that we will transpose.
*/
   j=0;
   for (i=0;i<10;i++) wheel[j++]=(int)'0' + i;
   for (i=0;i<26;i++) wheel[j++]=(int)'A' + i;
   for (i=0;i<26;i++) wheel[j++]=(int)'a' + i;
   for (i=0;i<15;i++) wheel[j++]=(int)'!' + i;

   too_short=0;
   for (p1=in,i=0;i<6;i++) {
      if (*p1++ == '\0') too_short=1;
   }

   kpos=6;
   p1=in;
   for (i=0;i<kpos;i++,p1++);
   rval = (int)*p1;
   rval = rval - 'e';

   if (rval > 15 || rval < 0 || too_short==1) {  /* invalid key or too short */
      while ((*out++ = *in++) != '\0')  ;   /* return input string */
      return -1;
   }

   seq = 0;
   if (rval==0) seq = 0xd76aa478;
   if (rval==1) seq = 0xe8c7b756;
   if (rval==2) seq = 0x242070db;
   if (rval==3) seq = 0x698098d8;
   if (rval==4) seq = 0xc1bdceee;
   if (rval==5) seq = 0xf57c0faf;
   if (rval==6) seq = 0x4787c62a;
   if (rval==7) seq = 0xa8304613;
   if (rval==8) seq = 0x774696ea;
   if (rval==9) seq = 0x6720696e;
   if (rval==10)seq = 0x3a257320;
   if (rval==11)seq = 0x6f75743a;
   if (rval==12)seq = 0x73656320;
   if (rval==13)seq = 0x6f74202e;
   if (rval==14)seq = 0x203d640a;
   if (rval==15)seq = 0x6e6f742e;

   addin_i=0;
   my_out = headstring;
   my_in = in;
   my_in++;   /* skip leading '.' */
   for (ii=0;;) {
      ii++;
      if (ii==6) {
         not_en = 0;
         if (*in != '.') { 
            not_en = 1;  /* is not 'encrypted' */
         }
         if (headstring[0] !='S' - ( (rval&0x7)*2)) {
           not_en=1; 
           if (obfDebug) printf("not s\n");
         }

         if (timeVal==0) {
            timeVal = obfiTimeval();
         }
         encodedTime = ((headstring[1] - 'a')<<4) + (headstring[2]-'a') +
                       ((headstring[3] - 'a')<<12) + ((headstring[4]-'a')<<8);

         if (encodedTime != timeVal  && encodedTime != timeVal-1) not_en=1;
         if ( (timeVal==0 && encodedTime==65535) || 
              (timeVal==65535 && encodedTime==0 ) ) {
             not_en=0;
         }

         if (obfDebug) printf("timeVal=%d encodedTime=%d\n",timeVal,encodedTime);

         my_out = out;   /* start outputing for real */
         if (not_en == 1) {
             while ((*out++ = *in++) != '\0')  ;   /* return input string */
             return -2;
         }
         my_in++;   /* skip key */
      }
      else {
         found=0;
         addin = (seq>>addin_i)&0x1f;
         addin += extra;
#ifndef _WIN32
          addin += uid;
#endif
         addin_i+=3;
         if (addin_i>28)addin_i=0;
         for (i=0;i<wheel_len;i++) {
            if (*my_in == (char)wheel[i]) {
               j = i - addin;
               if (obfDebug) printf ("j=%d ",j);

               while (j<0) {
                 j+=wheel_len;
               }
               if (obfDebug) printf ("j2=%d \n",j);

               *my_out++ = (char)wheel[j];
               nout++;
               found = 1;
               break;
            }
         }
         if (found==0) {
            if (*my_in == '\0') {
               *my_out++ = '\0'; 
               return 0;
            }
            else {*my_out++=*my_in; nout++;}
         }
         my_in++;
      }
   }
}

int
obfiGetEnvKey()
{
  char *envVar;
  char *chr;
  int i;
  envVar = getenv(AUTH_ADDITIONAL_KEY_ENV_VAR_NAME);
  if (envVar != NULL) {
    if (obfDebug) 
       printf("%s is %s\n",AUTH_ADDITIONAL_KEY_ENV_VAR_NAME, envVar);
    i = 0;
    chr = envVar;
    for (;*chr!=0;) {
      i = i + (int)*chr++;
      i = i & 0xffff; /* bound it */
    }
    if (obfDebug) printf("%s=%d\n",AUTH_ADDITIONAL_KEY_ENV_VAR_NAME, i);
    return i;
  }
  else {
    if (obfDebug) printf("Notice: No %s defined.\n",
                      AUTH_ADDITIONAL_KEY_ENV_VAR_NAME);
    return 0;
  }
}

