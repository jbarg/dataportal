/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*

 You should download and replace this source file with the real one at
 http://www.npaci.edu/DICE/SRB/tarfiles/gen_uuid.c , if you want to
 make use of the SRB GUID/UUID (Global or Universal, Unique
 IDentifiers) features.  The real gen_uuid.c is much better than this
 version.  It is based on code with a GNU license tho so we cannot
 distribute it as part of the SRB.

 This version is just a place-holder, a dummy version.  It will
 attempt to generate a UUID, via one of various pseudo random number
 generators.  On computers that have /dev/random or /dev/urandom this
 is random value will be a pretty good UUID (altho still not as
 accurate at the real routine produces).  On other computers, the
 pseudo random value won't very random and the UUID may not be unique.
 We include this as a convenience, so that sites not interested in
 using UUIDs can build an run the SRB system without the extra step of
 downloading the real gen_uuid.c

 Because of that, this version will put a warning message into the SRB
 log each time that it generates a UUID.

 So again, to use this safely, replace this with the version at:
 http://www.npaci.edu/DICE/SRB/tarfiles/gen_uuid.c 

 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "mdasC_db2_externs.h"
#include "elogExtern.h"

int getGuidRTC()
/*
 return an integer quasi-real-time-clock value
*/
{
     int i, j;
     static struct timeval tv;

     int sec,usec,irtc;
     float rtc;

     gettimeofday(&tv, 0);
     sec =  tv.tv_sec;
     usec = tv.tv_usec;
     rtc = sec*1000 + usec;

     irtc = rtc;
     irtc = irtc & 0x3fffffff;
     return(irtc);
} 


int getGuidBySRB(char *result)
{
     int i, j;

     int fd;
     static int seed=-1;
     unsigned char rbuf[300];

     fd = open("/dev/urandom", O_RDONLY);
     if (fd < 0) {
       fd = open("/dev/random", O_RDONLY);
     }
     if (fd > 0) {
        i = read(fd, rbuf, 20); 
        close(fd);
/*
	elog(WARN, "using dummy version of gen_uuid.c, poor UUID generated; source needs to be updated, see http://www.npaci.edu/DICE/SRB/tarfiles/gen_uuid.c");
*/
     }
     else {
        if (seed==-1) {
           seed = getGuidRTC() + getuid() + getpid();
           srand(seed);
        }
        j = getGuidRTC();
        j = (j & 0xFF)>>2;
        for (i=0;i<j;i++) {
           rand();
        }
        for (i=0; i<16; i++) {
           rbuf[i]=rand() & 0xff;
        }
/*
	elog(WARN, "using dummy version of gen_uuid.c, VERY poor UUID generated; source needs to be updated, see http://www.npaci.edu/DICE/SRB/tarfiles/gen_uuid.c");
*/
     }
     sprintf(result, "%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x",
         rbuf[0], rbuf[1], rbuf[2], rbuf[3], rbuf[4], rbuf[5],
         rbuf[6], rbuf[7], rbuf[8], rbuf[9], rbuf[10], rbuf[11], 
         rbuf[12], rbuf[13], rbuf[14], rbuf[15]);
     return(MDAS_SUCCESS);
}


#if 0
/* A handy main() for making a test program out of this. */
int
main()
{
     char result1[40];
     char result2[40];
     char result3[40];
     char result4[40];
     getGuidBySRB(result1);
     getGuidBySRB(result2);
     getGuidBySRB(result3);
     printf("uuid=%s\n",result1);
     printf("uuid=%s\n",result2);
     printf("uuid=%s\n",result3);
     sleep(1);
     getGuidBySRB(result4);
     printf("uuid=%s\n",result4);
}
#endif
