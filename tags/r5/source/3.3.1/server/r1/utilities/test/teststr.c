#include <stdio.h>
#include <fcntl.h>      /* for most O_ */
#ifndef O_RDONLY
#include <sys/file.h>   /* for the rest O_ */
#endif /* O_RDONLY */



#include "srbio.h"


#define BUFSIZE 1000000         /* size of page transferred at a time */
#define LINESIZE 255
#define PERMS   0755     
#define MAX_TESTOPEN_FILES 20
#define MAX_FILE_NAMES 100     
#define NUMBER_OF_OPS  13
#define SEEK_MAX 400            /* maximum no. bytes seeked */
#define MAX_BYTES  5000000         /* maximum no. bytes being read/writ per opn */
#define MAX_OPS 4               /* size of fops[] -
				    number of actual operations being tested*/

/*
#define BUFSIZE 1000            
#define MAX_BYTES  5000         
*/


char *fops[] = { "a", "r", "a+", "r+" };
/* 
#define MAX_OPS 3
char *fops[] = { "a", "r", "w" };
#define MAX_OPS 6
char *fops[] = { "a", "r", "w", "a+", "w+", "r+" };
#define MAX_OPS 8
char *fops[] = { "a", "r", "w", "a+", "w+", "r+" , "wrpl", "arpl" };
#define MAX_OPS 11
char *fops[] = { "a", "r", "w", "a+", "w+", "r+" , 
                 "wrpl", "arpl", "w+rpl", "a+rpl", "r+rpl" };
*/



int NumberOfOpenFiles  =  0;
int isUnix = 0;
char bufChar = ' ';


char buffer[BUFSIZE];



int NumberofReads = 0;
int NumberofWrites = 0;
int NumberofSeeks = 0;
int NumberofOpens = 0;
int NumberofCloses = 0;

long int NumberofReadBytes = 0;
long int NumberofWriteBytes = 0;
long int NumberofSeekBytes = 0;



FILE *fd[MAX_TESTOPEN_FILES];
char fileName[MAX_TESTOPEN_FILES][200];
char *fileMode[MAX_TESTOPEN_FILES];
char lastOp[MAX_TESTOPEN_FILES];




enditall()
{
  int i,j ; 

  printf("END \n");
  for (i = 0; i < NumberOfOpenFiles; i++) {
    printf("  closing %i:%i:%s ",i,fd[i],fileName[i]);
    j = fclose(fd[i]);  NumberofCloses++;

    if (j != 0) 
      printf(" Error in Closing: %i\n");
    else 
      printf(" done\n");
  }
  printf("********************************\n");
  printf("NumberofReads        = %i\n",NumberofReads);
  printf("NumberofWrites       = %i\n",NumberofWrites);
  printf("NumberofSeeks        = %i\n",NumberofSeeks);
  printf("NumberofOpens        = %i\n",NumberofOpens);
  printf("NumberofCloses       = %i\n",NumberofCloses);
  printf("NumberofReadBytes    = %i\n",NumberofReadBytes);
  printf("NumberofWriteBytes   = %i\n",NumberofWriteBytes);
  printf("NumberofSeekBytes    = %i\n",NumberofSeekBytes);
  printf("********************************\n");
  exit(0);
}
int 
perform_fwrite()
{
  int  nbytes,wbytes, j,bj, ii,i,jj;

  if (NumberOfOpenFiles == 0) {
    printf("WRITE failed: NO OPEN FILES\n");
    return; 
  }
  ii  = random() % NumberOfOpenFiles;
  nbytes = (random() % MAX_BYTES) + 1;
  bj = 0;
  if (lastOp[ii] == 'r' && (strstr(fileMode[ii],"+") != NULL)) {
    jj = fseek((FILE *) fd[ii],  0, SEEK_CUR) ;
    printf("Perform a dummy seek in fwrite\n");
    if (jj < 0) {	
      printf("Error in Seeking:%i\n",jj);
	enditall();
    }
  }
  printf("WRITE: fd:%i file:%s size:%i\n", fd[ii],fileName[ii],nbytes-1);
  for (i = 0; i < nbytes ; i++, bj++) {
    if ((bj < BUFSIZE) && (i != (nbytes -1)) ) {
      buffer[bj] = (int) (random() % 60) + ' ';
      buffer[bj] = bufChar + ' ';
      bufChar = (bufChar + 1) % ('Z' - ' ') ;
      continue;
    }
    wbytes = fwrite(buffer,1,bj,  fd[ii]);
    if (wbytes != bj)
      {
	printf("Error in Writing: fileMode='%s' Wrote only %i of %i\n",
	       fileMode[ii],   wbytes,bj);
	if (strcmp(fileMode[ii],"r"))
	  enditall();
      }
    if (wbytes > 0) NumberofWriteBytes = NumberofWriteBytes + wbytes; 
    printf(" wrote %i of %i of total %i bytes in fd:%i and file:%s\n",
	   wbytes,bj,nbytes-1, fd[ii],fileName[ii]);
    bj =  0;    
    buffer[bj] = (int) (random() % 60) + ' ';
    buffer[bj] = bufChar + ' ';
    bufChar = (bufChar + 1) % ('Z' - ' ') ;
  }
  lastOp[ii] = 'w';
  NumberofWrites++;
}

int 
perform_fread()
{
  int  nbytes,nbytes1,j,bj, i,jj;

  if (NumberOfOpenFiles == 0) {
    printf("READ failed: NO OPEN FILES\n");
    return; 
  }
  printf("READ:  ");
  i  = random() % NumberOfOpenFiles;
  nbytes = (random() % MAX_BYTES) + 1;
  nbytes1 = nbytes;
  bj = 0;
  if ((lastOp[i] == 'w' || lastOp[i] == 'o' )
      && (strstr(fileMode[i],"+") != NULL)) {
    jj = fseek((FILE *) fd[i],  0, SEEK_CUR) ;
    printf("Perform a dummy seek in fread\n");
    if (jj < 0) {	
      printf("Error in Seeking:%i\n",jj);
	enditall();
    }
  }
  if (BUFSIZE > nbytes) {
    j = fread(buffer,1,nbytes, (FILE *) fd[i]);
    
    printf(" read %i of %i of total %i bytes in fd:%i and file:%s\n",
	   j, nbytes,nbytes1,fd[i],fileName[i]);
    if (j > 0) NumberofReadBytes = NumberofReadBytes + j; 
  } 
  else {
    while ((nbytes = (nbytes - BUFSIZE)) > 0) {
      j = fread(buffer,1,BUFSIZE, (FILE *) fd[i]);
      
      printf(" read %i of %i of total %i bytes in fd:%i and file:%s\n",
	     j, BUFSIZE,nbytes1,fd[i],fileName[i]);
      if (j <= 0) break;
      NumberofReadBytes = NumberofReadBytes + j; 
 
    }
    if(nbytes > 0 && j > 0) {
      j = fread(buffer,1,nbytes, (FILE *) fd[i]);
      if (j > 0) NumberofReadBytes = NumberofReadBytes + j; 
      printf(" read %i of %i of total %i bytes in fd:%i and file:%s\n",
	     j, nbytes,nbytes1,fd[i],fileName[i]);
    }
  }
  lastOp[i] = 'r';
  NumberofReads++;
}

int 
perform_fseek()
{
  int  i,  k, m;
  long j;

  if (NumberOfOpenFiles == 0) {
    printf("SEEK failed: NO OPEN FILES\n");
    return; 
  }
  printf("SEEK:  ");

  i = random() % NumberOfOpenFiles;
  j = random() % SEEK_MAX;
  k = random() % 4;
  if (k == 0) {
    m = fseek((FILE *) fd[i],  j, SEEK_SET) ;
  printf("seek fd:%i and file:%s :Seek=%i:Origin=SET:Res=%i\n",fd[i],fileName[i],j,m);
  }
  else if (k == 1)  {
    m = fseek((FILE *) fd[i],  j, SEEK_CUR) ;
  printf("seek fd:%i and file:%s :Seek=%i:Origin=CUR:Res=%i\n",fd[i],fileName[i],j,m);
  }
  else if (k == 2) {
    j = 0 - j;
    m = fseek((FILE *) fd[i],  j, SEEK_CUR) ;
  printf("seek fd:%i and file:%s :Seek=%i:Origin=CUR:Res=%i\n",fd[i],fileName[i],j,m);
  }
  else  {
    m = fseek((FILE *) fd[i], j, SEEK_END) ;
  printf("seek fd:%i and file:%s :Seek=%i:Origin=END:Res=%i\n",fd[i],fileName[i],j,m);
  }
  if (m < 0) {
    printf("Error Seeking Object: %i \n", m);
    if (!(m = -1 && k == 2 && j < 0)) enditall();
  }
  else {
    lastOp[i] = 's';
    NumberofSeekBytes = NumberofSeekBytes + abs(j);
    NumberofSeeks++;
  }


}
int 
perform_fopen()
{  
  int j, jj;
  FILE *ifd;
  char fName[100];

  if (NumberOfOpenFiles == MAX_TESTOPEN_FILES) {
    printf("OPEN failed: MAX TEST OPEN FILES REACHED\n");
    return; 
  }
  printf("OPEN:  "); 

  j = random() % MAX_FILE_NAMES;
  if (isUnix)
    sprintf(fName, "u:strTest%i",j);
  else
    sprintf(fName, "strTest%i",j);
    
  jj  = random() % MAX_OPS;
  printf("OPEN file:%s: %s   ", fName,fops[jj]); fflush(stdout);
  ifd = fopen(fName,fops[jj]);
  if (ifd == NULL) {
    printf("\n Error opening Object: %s: %s\n", fName,fops[jj]);
    /*
      enditall();
    */
    return(-1);
  }
  printf(" fd:%i\n", ifd);fflush(stdout);
  fd[NumberOfOpenFiles] = ifd;
  strcpy(fileName[NumberOfOpenFiles],fName);
  lastOp[NumberOfOpenFiles] = 'o';
  fileMode[NumberOfOpenFiles] = fops[jj];
  NumberOfOpenFiles++;
  NumberofOpens++;
}

int 
perform_fclose()
{  
  int  i,j;


  if (NumberOfOpenFiles == 0) {
    printf("CLOSE failed: NO OPEN FILES\n");
    return; 
  }
  printf("CLOSE: ");
  i = random() % NumberOfOpenFiles;
  printf("closing fd:%i and file:%s...", fd[i],fileName[i]);
  j = fclose((FILE *) fd[i]);
  if (j < 0) {
    printf("Error closing Object: %i: %i\n", fd[i],j);
    /*
      enditall();
    */
  }
  NumberOfOpenFiles--;  
  strcpy(fileName[i],fileName[NumberOfOpenFiles]);
  fd[i] = fd[NumberOfOpenFiles];
  lastOp[i] = lastOp[NumberOfOpenFiles];
  fileMode[i] = fileMode[NumberOfOpenFiles];
  NumberofCloses++;

  printf("    done\n");
}

void
usage()
{
  printf("Usage: teststr u|s <number of operations> <seed-random-number>\n");
  exit(-1);
}
int
main(int argc, char **argv)
{
  int i, k, numOps;

  FILE *fd, *ufd;
  int ifd, iufd;
  int  nbytes, wbytes;
  char unixFile[250];
  int  intIn;
  
   if (argc != 4) usage();
   srandom((unsigned int) atoi(argv[3]));
   numOps = atoi(argv[2]);

   if (argv[1][0]  == 'u') isUnix = 1;

   for (i = 0; i < numOps ; i++) {
printf("%i ",i);
     k = random() % NUMBER_OF_OPS;
     switch (k) { 
     case 0:
           perform_fopen();
           break;
     case 1:
     case 2:
     case 3:
     case 4:
     case 5:
           perform_fread();
	   break;
     case 6:
     case 7:
     case 8:
     case 9:
           perform_fwrite();
           break;
     case 10:
           perform_fclose();
           break;
     case 11:
     case 12:
           perform_fseek();
           break;
     default : 
              printf("Operation Unknown\n");
	      exit();
           break;
     }
     /*     sleep(1); */
   }
    printf("THE ");
    enditall();
}
