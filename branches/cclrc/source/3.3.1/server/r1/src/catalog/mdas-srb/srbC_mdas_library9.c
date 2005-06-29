
/*
  These routines obfuscate (sort-of encrypt) short text strings
  (passwords).  These are based on obf.c but for server-side only for
  use with the MCAT.  An additional constraint, from the MCAT usage,
  is that the same password is always encoded to the same value.

  This is not highly secure, but it is better than leaving plain-text
  password in the clear in the MCAT DB.
*/
#include "mdasC_db2_externs.h"
#include "srbC_mdas_externs.h"
#include "mdasGlobalsExtern.h"
#include "elogExtern.h"
#include "srbMiscExtern.h"
#ifdef PORTNAME_solaris
#include <strings.h>
#endif


void obf2Encode(char *in, char *out, int max_out, int extra);
int obf2Decode(char *in, char *out, int extra);
int obf2SetDebug(int opt);

int obf2Debug=0;


/* 
  What can be a main routine for some simple tests.
 */
int
NotObf2_main(argc, argv)
int argc;
char *argv[];
{
  int timeVal=0;
  static int myExtra=124;

   char p2[MAX_TOKEN];
   int i;

   obf2SetDebug(2);

   if (argc < 3) {
     printf("Usage: -d|-e string\n");
     return(-1);
   }

   if (strcmp(argv[1],"-d")==0) {
     i = obf2Decode(argv[2], p2, myExtra);
     if (obf2Debug)  printf("val=%d string=%s\n",i,p2);
   }

   if (strcmp(argv[1],"-e")==0) {
     obf2Encode(argv[2], p2, MAX_TOKEN, myExtra);
     if (obf2Debug)  printf("string=%s\n",p2);
   }
   return (0);
}

int 
obf2SetDebug(int opt)
{
  obf2Debug = opt;
  return (0);
}

 

/*
  Obfuscate a string
*/
void
obf2Encode(in,out,max_out,extra)
char *in;
char *out;
int max_out;
int extra;
{
   int i;
   long seq;
   char *my_in;
   char *tchr;

   int rval;
   int wheel_len;
   int wheel[26+26+10+15-1];
   int j, addin, addin_i, found;
   int ii;
   int now;
   char headstring[10];
   int len;

/*
 First check output length, if too small just return input (up to max)
 */
   len = strlen(in);
   if (len+10>max_out) {
     len = strlen(in);
     if (max_out < len+1) len=max_out-1;
     for (i=0;i<len;i++) {
       *out++=*in++;
     }
     *out='\0';
     return;
   }

/*
 Set up an array of characters that we will transpose.
*/

   wheel_len=26+26+10+15-1;
   j=0;
   for (i=0;i<10;i++) wheel[j++]=(int)'0' + i;
   for (i=0;i<26;i++) wheel[j++]=(int)'A' + i;
   for (i=0;i<26;i++) wheel[j++]=(int)'a' + i;
   for (i=0;i<15;i++) {
     if (i==4) continue; /* skip % */
     if (i==6) wheel[j++]='_';  /* use _ instead of ' */
     else      wheel[j++]=(int)'!' + i;
   } 


/*
 get a pseudo random number
*/
/*
   ftime(&timeb_time);
   rval=timeb_time.millitm & 0xf;
*/
/* Needs to be same input -> same output all the time for
   how this is used in MCAT, so instead of a random number
   take something based on the string.
*/
   tchr = in;
   rval = 0;
   for (i=0;i<strlen(in);i++) {
     rval += (int)*tchr++;
     rval &= 0xff;
   }

   rval = rval & 0xf;

/*
 and use it to pick a pattern for ascii offsets
*/
   seq = 0;
   if (rval==0) seq = 0x5734afb3;
   if (rval==1) seq = 0x8c7b95f7;
   if (rval==2) seq = 0x68909d88;
   if (rval==3) seq = 0x422070db;
   if (rval==4) seq = 0xcd1bcefe;
   if (rval==5) seq = 0xf5c70ffa;
   if (rval==6) seq = 0x9869ffea;
   if (rval==7) seq = 0x9faea683;
   if (rval==8) seq = 0xf678442f;
   if (rval==9) seq = 0xe78e856b;
   if (rval==10)seq = 0x7e8ecda4;  
   if (rval==11)seq = 0x3bced97a;
   if (rval==12)seq = 0x356bcdae;
   if (rval==13)seq = 0x6347a2bc;
   if (rval==14)seq = 0x28714ead;
   if (rval==15)seq = 0xe667f24e;

/*
 get the timestamp and other id
*/
   now = 777;  /* can't use a random time value */
   headstring[1] = ((now>>4) & 0xf) + 'a';
   headstring[2] = (now & 0xf) + 'a';
   headstring[3] = ((now>>12) & 0xf) + 'a';
   headstring[4] = ((now>>8) & 0xf) + 'a';
   headstring[5] = '\0';
   headstring[0]='S' - ( (rval&0x7)*2);  /* another check value */

  *out++ = '_';          /* store our initial, human-readable identifier */

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
     addin_i+=3;
     if (addin_i>28)addin_i=0;
     for (i=0;i<wheel_len;i++) {
        if (*my_in == (char)wheel[i]) {
           j = i + addin;
           if (obf2Debug>1) printf("j1=%d ",j);
           j = j % wheel_len;
           if (obf2Debug>1) printf ("j2=%d \n",j);
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
obf2Decode(in, out, extra)
char *in;
char *out;
int extra;
{
   int i;
   long seq;
   char *p1;

   int rval;
   int wheel_len;
   int wheel[26+26+10+15-1];
   int j, addin, addin_i, kpos, found, nout;
   char headstring[10];
   int ii, too_short;
   char *my_out, *my_in;
   int not_en, encodedTime;

   wheel_len=26+26+10+15-1;

/*
 Set up an array of characters that we will transpose.
*/
   j=0;
   for (i=0;i<10;i++) wheel[j++]=(int)'0' + i;
   for (i=0;i<26;i++) wheel[j++]=(int)'A' + i;
   for (i=0;i<26;i++) wheel[j++]=(int)'a' + i;
   for (i=0;i<15;i++) {
     if (i==4) continue; /* skip % */
     if (i==6) wheel[j++]='_';  /* use _ instead of ' */
     else      wheel[j++]=(int)'!' + i;
   } 

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
   if (rval==0) seq = 0x5734afb3;
   if (rval==1) seq = 0x8c7b95f7;
   if (rval==2) seq = 0x68909d88;
   if (rval==3) seq = 0x422070db;
   if (rval==4) seq = 0xcd1bcefe;
   if (rval==5) seq = 0xf5c70ffa;
   if (rval==6) seq = 0x9869ffea;
   if (rval==7) seq = 0x9faea683;
   if (rval==8) seq = 0xf678442f;
   if (rval==9) seq = 0xe78e856b;
   if (rval==10)seq = 0x7e8ecda4;  
   if (rval==11)seq = 0x3bced97a;
   if (rval==12)seq = 0x356bcdae;
   if (rval==13)seq = 0x6347a2bc;
   if (rval==14)seq = 0x28714ead;
   if (rval==15)seq = 0xe667f24e;

   addin_i=0;
   my_out = headstring;
   my_in = in;
   my_in++;   /* skip leading '_' */
   for (ii=0;;) {
      ii++;
      if (ii==6) {
         not_en = 0;
         if (*in != '_') { 
            not_en = 1;  /* is not 'encrypted' */
         }
         if (headstring[0] !='S' - ( (rval&0x7)*2)) {
           not_en=1; 
           if (obf2Debug) printf("not s\n");
         }

         encodedTime = ((headstring[1] - 'a')<<4) + (headstring[2]-'a') +
                       ((headstring[3] - 'a')<<12) + ((headstring[4]-'a')<<8);

	 /*
         if (encodedTime != timeVal  && encodedTime != timeVal-1) not_en=1;
         if ( (timeVal==0 && encodedTime==65535) || 
              (timeVal==65535 && encodedTime==0 ) ) {
             not_en=0;
         }
	 */

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
         addin_i+=3;
         if (addin_i>28)addin_i=0;
         for (i=0;i<wheel_len;i++) {
            if (*my_in == (char)wheel[i]) {
               j = i - addin;
               if (obf2Debug) printf ("j=%d ",j);

               while (j<0) {
                 j+=wheel_len;
               }
               if (obf2Debug) printf ("j2=%d \n",j);

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

