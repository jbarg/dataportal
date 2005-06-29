#include <stdio.h>
#include <fcntl.h>      /* for most O_ */
#ifndef O_RDONLY
#include <sys/file.h>   /* for the rest O_ */
#endif /* O_RDONLY */


#include "srbio.h"


/*
#include "srbio.h"
 */


#define BUFSIZE 100000
#define LINESIZE 255
#define PERMS   0755
void
usage()
{
  printf("Usage: testthree <number-of-files> <size of each file> <collection>\n");
  exit(-1);
}
int
main(int argc, char **argv)
{
  int i,j,bj;
  FILE *fd;
  int  nbytes, wbytes;
  char *buffer;
  char File[250];
  int  numFiles, fileSize;
  char fileName[LINESIZE];
  struct timespec tp;
  char bufChar =  ' ';

   if (argc != 4 ) usage();
   printf("Input to testthree:%s:%s:%s\n",argv[1],argv[2],argv[3]);
   numFiles = atoi(argv[1]);
   fileSize = atoi(argv[2]);
   buffer = (char *) malloc(sizeof(char) * fileSize);
   for (i = 0; i < fileSize ; i++) {
     buffer[i] = bufChar + ' ';
     bufChar = (bufChar + 1) % ('Z' - ' ') ;
   }
   clock_gettime(CLOCK_REALTIME, &tp);
   printf("testthreeBegin:%li.%li\n",
	  tp.tv_sec, tp.tv_nsec);
   for (i = 0 ; i <numFiles ; i++) {
     sprintf(fileName,"%s/millionTest%i",argv[3],i);
     fd = fopen(fileName,"w");
     if (fd == NULL) {
       printf("Error opening Object: %s\n", fileName);
       exit(-1);
     }
     wbytes = fwrite(buffer,1,fileSize, fd);
     if (wbytes != fileSize) {
       printf("Error in Writing: Wrote only %i of %i\n",
	      wbytes,fileSize);
       fclose(fd);
       exit(-1);
     }
     fclose(fd);
   }
   clock_gettime(CLOCK_REALTIME, &tp);
   printf("testthreeEnd  :%li.%li\n",tp.tv_sec, tp.tv_nsec);
}


