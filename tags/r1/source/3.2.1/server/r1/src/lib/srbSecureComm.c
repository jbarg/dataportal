/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* SRB Private Communication System

  These are routines are part of the SRB Secure Communication System,
  used as part of a file encryption feature.  These are layered on the
  OpenSSL libraries, RSA and Blowfish do the actual encryption.

  Used as follows:

  SrbMaster may call sscEstablishRsaKeypair(1) to create key before it
  is needed.  This is not required as the keyPair will be generated
  when needed otherwise.  Once generated, the keyPair is written to
  disk to be used again later.

  For the srbSetupSessionPublicKey client/server call; the server-side
  will call sscGetPublicKeyString() to get the public key string and
  will then return it to the client.  The client code will call
  sscInitPublicKey with this string to initialize the public key for
  use by this library.

  For srbSetupSession, the client code calls sscSetupSession which
  generates a session key, saves it, encrypts it with public key, and
  returns this to be sent to the server.  On the Server side, the
  processing calls sscReceiveSession which decrypts and saves the
  session key.

  Client/server calls that exchange encrypted data (currently some
  types of srbModifyDataset and srbGetDataDirInfo), call sscEncrypt to
  encrypt the field and sscDecrypt to decrypt the field, using the
  session key.
*/

#include <sys/types.h>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#include <stdio.h>
#ifndef _WIN32
#include <sys/time.h>
#endif
#include <fcntl.h>
#include <stdlib.h>

#include <string.h>
#include "srbErrno.h"

/* bypass most everything and define stubs below, if SECURE_COMM
   has not been selected 
*/


#if defined(SRB_SECURE_COMM) 

#define RSA_LENGTH 1536 
/*  A public key length of 1536 bits should provide good protection
    against a moderate level of computational force circa 2005-2010.  As
    per Bruce Schneier, Applied Cryptography 2nd Ed, p162. */
#define SESSION_KEY_LENGTH 16
/* 16 bytes is 128 bits and for an symetric key should provide good
    protection against a moderate level of computational force circa
    2000-2010. */
#define RSA_BUFFER_SIZE 800  /* Normally 192*2 is plenty */
#define IO_BUFFER_SIZE 2000 
#define BF_IVEC_SIZE 8
#define BF_BLOCK_SIZE 8

#define KEY_FILE_NAME "../data/rsaKey"  /* assuming ../data exists */

#include <openssl/rand.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/blowfish.h>

static int debug=0;
static RSA *RSA_KEY=NULL;

static unsigned char sessionKey[SESSION_KEY_LENGTH+10];
static int sessionKeyInitialized=0;

/*
 Initialize the random number generator by getting
 some semi-random bytes from the environment.
 These are very easy to get and vary in unpredicatable ways.
 */
void
sscInitRandom()
{
    struct stat statbuf;
    int i, len;
    struct timeval tv;
    static float count=123.432; 
    pid_t           pid;
    static int randomInitialized=0;
    unsigned long usec;

    if (randomInitialized!=1) { /* don't seed again */
        /* On systems without /dev/urandom or /dev/random, we need to
          call RAND_seed first to initialize the seed */
        i = gettimeofday(&tv, 0); /* get timeofday */
	usec = tv.tv_usec;
	RAND_seed((unsigned char *)&usec, sizeof(usec));
    }

    pid = getpid();
    RAND_add(&pid, sizeof(pid),2);

    pid = getpgrp();
    RAND_add(&pid, sizeof(pid),2);

    pid = getppid();
    RAND_add(&pid, sizeof(pid),2);
    pid = 0;

    i = fstat(0,&statbuf);    /* do a fstat on stdin */
    if (i>=0) {
      len = sizeof(statbuf);
      RAND_add((unsigned char *)&statbuf, len, 4);
    }

    i = fstat(1,&statbuf);    /* do a fstat on stdout */
    if (i>=0) {
      len = sizeof(statbuf);
      RAND_add((unsigned char *)&statbuf, len, 4);
    }

    i = stat("/",&statbuf);   /* and a stat on the root fs */
    if (i>=0) {
      len = sizeof(statbuf);
      RAND_add((unsigned char *)&statbuf, len, 2);
    }

    i = gettimeofday(&tv, 0); /* get timeofday */
    if (i>=0) {
      len = sizeof(tv);
      RAND_add((unsigned char *)&tv, len, 4);
    }
    usec = tv.tv_usec;
    count += usec;
    count += 1.023;           /* and a counter */
    usec = 0;
    RAND_add((unsigned char *)&count, sizeof(count), 2);

    randomInitialized=1;     /* note that this initialization has been done */
    return;
}


/*
 Read or create an RSA public/private key pair to
 prepare for RSA exchanges.  Optionally, write the keypair to disk.

 Default operation:
  1) initialize the random number generator
  2) if the keyFile exists, read it in and use it if OK,
     else generate RSA keypair and write it to the KeyFile
     (if the keyFile exists, but is bad, generate a key but leave the file)

 Opt (options):
   If 1, create (or rewrite) keyFile even if it exists.
   If 3, read the public key components of the keyFile (for testing).

 */
int
sscEstablishRsaKeypair(int opt)
{
  int i, j;
  unsigned int k;
  unsigned char buf[IO_BUFFER_SIZE];
  char *cp;
  int start_time, stop_time, delta_time;
  int fd_in, fd_out, wval, rval;
  int buf_ix;

  BIGNUM *n, *e, *d, *p, *q, *dmp1, *dmq1, *iqmp;

  sscInitRandom(); 

/*
  First try to read in the key from the file, unless the option is to
  create a new one.
*/
  if (opt != 1) { 
     fd_in = open(KEY_FILE_NAME, O_RDONLY,0);
     if (fd_in > 0) {
        rval = read(fd_in, buf, IO_BUFFER_SIZE);
        if (rval > 0) {
           if (debug) printf("Opened RSA key: %s\n",KEY_FILE_NAME);
           do {  /* for break statements */
              n = BN_new();
              e = BN_new();
              d = BN_new();
              p = BN_new();
              q = BN_new();
              dmp1 = BN_new();
              dmq1 = BN_new();
              iqmp = BN_new();
              if (!n || !e || !d || !p || !q || !dmp1 || !dmq1 || !iqmp)
                 break;
              buf_ix = 0;
              i = BN_hex2bn(&n, (const char *)&buf[buf_ix]);
              if (i == 0) break;;
              buf_ix += i+strlen("\n");
              i = BN_hex2bn(&e, (const char *)&buf[buf_ix]);
              if (i == 0) break;;
              if (opt == 3) {
                 RSA_KEY = RSA_new();
                 RSA_KEY->n=n;
                 RSA_KEY->e=e;
                 return (SRB_E_NOERROR);
              }
              buf_ix += i+strlen("\n");
              i = BN_hex2bn(&d, (const char *)&buf[buf_ix]);
              if (i == 0) break;;
              buf_ix += i+strlen("\n");
              i = BN_hex2bn(&p, (const char *)&buf[buf_ix]);
              if (i == 0) break;;
              buf_ix += i+strlen("\n");
              i = BN_hex2bn(&q, (const char *)&buf[buf_ix]);
              if (i == 0) break;;
              buf_ix += i+strlen("\n");
              i = BN_hex2bn(&dmp1, (const char *)&buf[buf_ix]);
              if (i == 0) break;;
              buf_ix += i+strlen("\n");
              i = BN_hex2bn(&dmq1, (const char *)&buf[buf_ix]);
              if (i == 0) break;;
              buf_ix += i+strlen("\n");
              i = BN_hex2bn(&iqmp, (const char *)&buf[buf_ix]);
              if (i == 0) break;;
              buf_ix += i+strlen("\n");
              
              RSA_KEY = RSA_new();
              RSA_KEY->n=n;
              RSA_KEY->e=e;
              RSA_KEY->d=d;
              RSA_KEY->p=p;
              RSA_KEY->q=q;
              RSA_KEY->dmp1=dmp1;
              RSA_KEY->dmq1=dmq1;
              RSA_KEY->iqmp=iqmp; 
           } while (1==2); /* for break statements */
        }
     }
  }

/*
  If the key file didn't exist, or there was some problem reading it, 
  or opt requests it, generate  a new one.
 */
  if (RSA_KEY==NULL) {
     char msg[500];

     if (debug) printf("Generating new RSA keypair");
     start_time = clock();

     RSA_KEY = RSA_generate_key(1536, RSA_F4, NULL, NULL);
     if (RSA_KEY == NULL) {
        fprintf(stderr, "failure to generate RSA keypair\n");
        return (SRB_SECURE_COMM_LIBRARY_ERROR);
     }
     stop_time = clock();
     delta_time = stop_time - start_time;

/*
      and create the file and 
      write the newly generated key into it.
*/

     fd_out = open(KEY_FILE_NAME, O_WRONLY|O_CREAT|O_TRUNC, 0600);

     if (fd_out>0) {
        cp = BN_bn2hex(RSA_KEY->n);
        wval = write(fd_out, cp, strlen(cp));
        wval = write(fd_out, "\n", strlen("\n"));
        free(cp);
        cp = BN_bn2hex(RSA_KEY->e);
        wval = write(fd_out, cp, strlen(cp));
        wval = write(fd_out, "\n", strlen("\n"));
        free(cp);
        cp = BN_bn2hex(RSA_KEY->d);
        wval = write(fd_out, cp, strlen(cp));
        wval = write(fd_out, "\n", strlen("\n"));
        free(cp);
        cp = BN_bn2hex(RSA_KEY->p);
        wval = write(fd_out, cp, strlen(cp));
        wval = write(fd_out, "\n", strlen("\n"));
        free(cp);
        cp = BN_bn2hex(RSA_KEY->q);
        wval = write(fd_out, cp, strlen(cp));
        wval = write(fd_out, "\n", strlen("\n"));
        free(cp);
        cp = BN_bn2hex(RSA_KEY->dmp1);
        wval = write(fd_out, cp, strlen(cp));
        wval = write(fd_out, "\n", strlen("\n"));
        free(cp);
        cp = BN_bn2hex(RSA_KEY->dmq1);
        wval = write(fd_out, cp, strlen(cp));
        wval = write(fd_out, "\n", strlen("\n"));
        free(cp);
        cp = BN_bn2hex(RSA_KEY->iqmp);
        wval = write(fd_out, cp, strlen(cp));
        wval = write(fd_out, "\n", strlen("\n"));
        free(cp);
        close(fd_out);
        return(delta_time);
     }
     else {
       return (SRB_SECURE_COMM_LIBRARY_ERROR);
     }
  }
  return (SRB_E_NOERROR);
}

/*
 return a string describing the public key.
 */
char *
sscGetPublicKeyString()
{
  char *e, *n, *result;
  int len;

  /* If the key doesn't yet exist, try to read or create it */
  if (RSA_KEY==NULL) {
     sscEstablishRsaKeypair(0);
  }

  if (RSA_KEY==NULL) return("");  /* shouldn't happen */

  n = BN_bn2hex(RSA_KEY->n);
  e = BN_bn2hex(RSA_KEY->e);
  len = strlen(n) + strlen(e) + 20;
  result = malloc(len);
  if (result==NULL) return(""); /* malloc error */
  strcpy(result, "n=");
  strcat(result, n);
  strcat(result, "e=");
  strcat(result, e);
  strcat(result, "  ");  /* without this, there is sometimes a trailing @ */
  free(n);
  free(e);
  return(result);
}


/*
 Convert a string, in the form provided by sscGetPublicKeyString, into
 a public key for use by this library.
 */
int
sscInitPublicKey(char *keyString)
{
  char *cp, *cp2;
  int i;
  BIGNUM *n, *e;

  sscInitRandom(); 

  n=NULL;
  e=NULL;

  cp = keyString;
  cp+=2; /* should start with "n=", skip over it */
  cp2 = strstr(keyString, "e=");
  if (cp2==NULL) return(INP_ERR_FORMAT);
  *cp2 = '\0'; /* terminate the first string */
  cp2+=2; /* skip over "e=" */

  i = BN_hex2bn(&n, cp);
  if (i == 0) return(INP_ERR_FORMAT);

  i = BN_hex2bn(&e, cp2);
  if (i == 0) return(INP_ERR_FORMAT);

  /* set up the public components of the key */
  RSA_KEY = RSA_new();
  RSA_KEY->n=n;
  RSA_KEY->e=e;
  return (SRB_E_NOERROR);
}

char *sscToHex(char *inbuf, int len)
{
  char *cp, *cp2;
  int i, j, k;

  cp = malloc((len*2)+5);
  if (cp==NULL) return("");

  memset(cp, 0, len*2);
  cp2 = cp;
  for (i=0;i<len;i++) {
     k = inbuf[i] & 0xff;
     j = k & 0xf0;
     j = j >> 4;
     if (j >=0 && j<=9) *cp++ = j + (int)'0';
     else *cp++ = (j-10) + (int)'A';
     j = k & 0x0f;
     if (j >=0 && j<=9) *cp++ = j + (int)'0';
     else *cp++ = (j-10) + (int)'A';
  }
  *cp++ = '\0';
  return(cp2);
}

char *sscFromHex(char *inbuf)
{
  char *cp, *cp2;
  int len;
  int i, j, k, l;
  len = strlen(inbuf);

  cp = malloc(len);
  if (cp==NULL) return("");

  memset(cp, 0, len);
  cp2 = cp;
  for (i=0;inbuf[i]!='\0';i++) {
     k = inbuf[i];
     if (k >='A' && k<='F') j = k-(int)'A'+10;
     else j = k-'0';
     l = j << 4;
     i++;
     k = inbuf[i];
     if (k >='A' && k<='F') j = k-(int)'A'+10;
     else j = k-'0';
     l += j;
     *cp++ = l;
  }
  return(cp2);
}

static char *
sslerr(void)
{
        static char buf[1024];
        ERR_load_crypto_strings();
        ERR_error_string(ERR_get_error(), buf);
        return buf;
}

/*
  Setup a session; client side.
  Generate a session key, save it, encrypt it with public key, and
  return this encrypted session key.
*/
char *
sscSetupSession()
{
  int i, siz;
  static char buf[RSA_BUFFER_SIZE];
  char *cp;
  int rsaSize;

  rsaSize = RSA_size(RSA_KEY);
  if (debug) printf("rsaSize=%d\n",rsaSize);

  sscInitRandom();  /* initialize random number library if not done already */

  i = RAND_bytes(sessionKey, SESSION_KEY_LENGTH);  /* get bytes for the key */

  if (debug) printf("session key=%s\n", sscToHex((char *)sessionKey, 
                                      SESSION_KEY_LENGTH));

  siz = RSA_public_encrypt(SESSION_KEY_LENGTH,
     sessionKey, (unsigned char *)buf, RSA_KEY, RSA_PKCS1_OAEP_PADDING);
  if (siz < 0) {
    printf("error=%s\n",sslerr());
    return(NULL);
  }

  if (debug) printf("encrypted size=%d\n",siz);

  cp = sscToHex(buf,siz);

  if (debug) printf("encrypted buffer=%s\n",cp);

  sessionKeyInitialized=1;

  return(cp);
}

/*
 Received and decrypt a session key 
*/
int
sscReceiveSession(char *inStr)
{
  int siz;
  char *cp;
  char buf2[RSA_BUFFER_SIZE];

  int rsaSize;

  if (RSA_KEY==NULL) return(SRB_SECURE_COMM_NOT_ESTABLISHED);

  rsaSize = RSA_size(RSA_KEY);
  if (debug) printf("rsaSize=%d\n",rsaSize);

  siz = strlen(inStr);
  if (debug) printf("size=%d\n",siz);
  cp = sscFromHex(inStr);
  siz = strlen(inStr)/2;
  if (debug) printf("size=%d\n",siz);
  siz = RSA_private_decrypt(siz,(unsigned char *)cp,
    (unsigned char *)buf2, RSA_KEY, RSA_PKCS1_OAEP_PADDING);

  if (siz !=  SESSION_KEY_LENGTH) {
    printf("Bad sesion key length");
    return(INP_ERR_FORMAT);
  }
  if (debug) printf("size=%d\n",siz);
  if (debug) printf("decrypted size=%d\n",siz);
  if (debug) printf("%s\n",sscToHex(buf2,siz));

  memcpy(sessionKey, buf2, SESSION_KEY_LENGTH);

  sessionKeyInitialized=1;
  free(cp);
  return (SRB_E_NOERROR);
}

int
sscEncrypt(unsigned char *inBuf, int len)
{
  int i;
  BF_KEY my_key;
  char *cp;
  char *cp_in;
  unsigned char ivec[BF_IVEC_SIZE+20];

  if (sessionKeyInitialized==0) {
    int state, patch_len, i2;
    char msg[]="[Concealed]";
    state=0;
    patch_len=strlen(msg);
    for (i=0;i<len;i++) {
      if (state==0) {
        if (inBuf[i]!='\0') {state=1; i2=0;}
      }
      if (state==1) {
        if (i2<patch_len) {inBuf[i]=msg[i2++];}
        else {inBuf[i]='\0'; state=0;}
      }
    }
    return(SRB_SECURE_COMM_NOT_ESTABLISHED);
  }
  BF_set_key(&my_key, SESSION_KEY_LENGTH, sessionKey);

  memset(ivec, 0, sizeof(ivec));

  i = len % BF_BLOCK_SIZE;
  if (debug) printf("block moduo div=%d\n",i);
  if (i>0) {
    len = len - i;  /* round down to block */
    if (debug) printf("Warning, rounding down %d bytes\n",i);
  }
  BF_cbc_encrypt(inBuf, inBuf,
                   len,  &my_key, ivec, BF_ENCRYPT);
  return (SRB_E_NOERROR);
}

/*
 String mode encrypt.
 */
char *
sscEncryptString(char *inBuf)
{
  int i;
  int len;
  BF_KEY my_key;
  char *cp;
  char *cp_in;

  unsigned char ivec[BF_IVEC_SIZE+20];

  if (sessionKeyInitialized==0) {
    char msg[]="[Concealed]";
    cp = malloc(strlen(msg)+10);
    if (cp !=NULL) {
      strcpy(cp,msg);
    }
    return(cp);
  }

  BF_set_key(&my_key, SESSION_KEY_LENGTH, sessionKey);

  memset(ivec, 0, sizeof(ivec));

  len = strlen(inBuf);
  i = len % BF_BLOCK_SIZE;
  if (debug) printf("block modulo div=%d strlen=%d\n",i,len);
  if (i>0) {
    len = len + (BF_BLOCK_SIZE-i);  /* round up to block size */
    if (debug) printf("Warning, rounding up %d bytes to %d\n",i, len);
  }
  cp = malloc(len*2);  /* malloc input and result buffer */
  if (cp==NULL) return("");

  memset(cp, 0, len*2);
  cp_in = cp+len;
  strcpy(cp_in, inBuf);
  BF_cbc_encrypt((unsigned char *)cp_in, (unsigned char *)cp,
                   len,  &my_key, ivec, BF_ENCRYPT);
  return(sscToHex(cp, len));
}

int
sscDecrypt(unsigned char *inBuf, int len)
{
  int i;
  BF_KEY my_key;
  char *cp;
  char *cp_in;
  unsigned char ivec[BF_IVEC_SIZE+20];

  if (sessionKeyInitialized==0) {
    return(SRB_SECURE_COMM_NOT_ESTABLISHED);
  }

  BF_set_key(&my_key, SESSION_KEY_LENGTH, sessionKey);

  memset(ivec, 0, sizeof(ivec));

  BF_cbc_encrypt(inBuf, inBuf,
                   len,  &my_key, ivec, BF_DECRYPT);
  return (SRB_E_NOERROR);
}

char *
sscDecryptString(unsigned char *inBuf)
{
  int i;
  int len;
  BF_KEY my_key;
  char *cp;
  char *cp_in;
  unsigned char ivec[BF_IVEC_SIZE+20];
  char msg[]="[Concealed]";

  if (sessionKeyInitialized==0) {
    cp = malloc(strlen(msg)+10);
    if (cp!=NULL) strcpy(cp,msg);
    return (cp);
  }

  BF_set_key(&my_key, SESSION_KEY_LENGTH, sessionKey);

  memset(ivec, 0, sizeof(ivec));

  len = strlen((char *)inBuf);
  cp = malloc(len+10);  /* malloc result buffer */
  if (cp==NULL) return("");

  memset(cp, 0, len+10);
  cp_in = sscFromHex((char *)inBuf);
  len = strlen((char *)inBuf)/2;
  
  BF_cbc_encrypt((unsigned char *)cp_in, (unsigned char *)cp,
                   len,  &my_key, ivec, BF_DECRYPT);
  free(cp_in);
  return(cp);
}


#if 0
/* handy test main to make this a stand-alone test program */
main(argc, argv)
int argc;
char *argv[];
{
  char *cp;
  char buf[]="Hi2";
  int len, i;

  char testpub[]="n=BE5CDB494CBC9F65ABD6E5DCB4CB267E867FB600921590B8F1625D7BD1C4611F18E95DF804EEDBB535AC6AC3DA07C15E5F620EB46C3118A6421CF8BA881B6C40250884DE8F510438F4691850027B67C057063EFFB68BB33D95B2CF2EC27BB21D2A1BA62549771982D0151F706864A917813ED94CDDC05684963D681C1085A7FE0B97D9E107C6749D89CACD7EFC98D40EBC46EE09D78B5023F280531C77B0854864F994FD79970431823E7D250048E3865308FCE75A05071E95FAC57E708990ABe=010001";

  if (argc>1) {
    sscEstablishRsaKeypair(3); /* for test */

    cp = sscGetPublicKeyString();
    printf("pub key=%s\n",cp); 
  }
  else {
    i = sscInitPublicKey(testpub);
    printf("sscInitPublicKey=%d\n",i);
    if (i<0) return; 
    cp = sscGetPublicKeyString();
    printf("pub key=%s\n",cp); 
  }

  cp = sscSetupSession();
  if (cp==NULL) return;
  printf("session=%s\n",cp);

  //  sscReceiveSession(cp);

  len = strlen(buf);
  cp = sscEncrypt(buf);
  printf("buf=%s\n",cp);

  cp = sscDecrypt(cp);
  printf("decrypted=%s\n",cp);
}
#endif

#else  /* SRB_SECURE_COMM not defined */

int
sscReceiveSession(char *inStr)
{
  return SRB_SECURE_COMM_NO_SUPPORT; /* Not supported */
}

char *
sscEncryptString(char *inBuf)
{
  char *cp;
  char msg[]="[Concealed]";
  cp = malloc(strlen(msg)+10);
  if (cp !=NULL) {
    strcpy(cp,msg);
  }
  return(cp);
}

char *
sscGetPublicKeyString()
{
  char *cp;
  char msg[]="SRB Secure Communications not supported";
  cp = malloc(strlen(msg)+10);
  if (cp !=NULL) {
    strcpy(cp,msg);
  }
  return(cp);
}

int
sscInitPublicKey(char *keyString)
{
  return SRB_SECURE_COMM_NO_SUPPORT; /* Not supported */
}
#endif /* SRB_SECURE_COMM */
