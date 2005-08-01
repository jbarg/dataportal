/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*
FTP driver test

This is is essentially a full ftp client (altho there is no PWD).
Thru the SRB, it can connect to various servers, cd, list directories,
and get and put files.  There are also interactively-available
lower-level calls, such as open file, read file, write file, etc,
which are useful for testing.

*/

#include <stdio.h>
#include <sys/file.h>
#include <sys/stat.h>
#if defined(PORTNAME_c90) || defined(PORTNAME_solaris)
#include <sys/fcntl.h> 	/* For C-90 - define O_RDONLY */
#endif

#include "srbClient.h"

#define BUFSIZE		65536
#define WRITE_BUFSIZE   5000
#define UNIX_AM		0	/* UNIX access method */
#define UTREE_AM	1
#define HPSS_AM		2
#define FTP_AM		3
#define HOST_ADDR	NULL

#define HOST_ADDR       NULL
#define srbAuth         NULL

char buf[BUFSIZE+2];
char wbuf[WRITE_BUFSIZE];

srbConn *conn;

void 
exit_nicely()
{
    int funcVal;
    funcVal = srbSetStorAttri (conn, FTP_AM, "", "", "");
    if (funcVal < 0)  {
        fprintf(stderr,"%s",clErrorMessage(conn));
    }
    clFinish(conn);
    exit(1);
}

int
main(argc, argv)
int argc;
char *argv[];
{
    srbResult *res;
    char instring[80];
    char wbuf[WRITE_BUFSIZE];
    int i, j, k, opt, nbytes;
    int in_fd, out_fd;
    int funcVal;
    char *arg1, *arg2, *arg3, *cp1;
    char c1;
    int fd;

    c1 = 'a';
    for (i=0;i<WRITE_BUFSIZE;i++) {
       wbuf[i]=c1;
       c1++;
       if (c1>'z') {wbuf[i]='\n'; c1='a';}
    }
    /*
     * set up the connection
     */

    conn = clConnect (HOST_ADDR, NULL, srbAuth);

    /* check to see that the backend connection was successfully made */
    if (clStatus(conn) != CLI_CONNECTION_OK) {
	fprintf(stderr,"Connection to exfMaster failed.\n");
	fprintf(stderr,"%s",clErrorMessage(conn));
	exit_nicely();
    }

    for(;;) {
        printf(">");

	/*
	  Get input an parse it into up to three blank delimited arguments
	 */
        gets(instring);
        arg2=""; arg3="";
        cp1 = instring;
        while (*cp1==' ') cp1++;
        arg1 = cp1;
        for (cp1=instring;*cp1!='\0';cp1++) {
	    if (*cp1==' ') {
                *cp1='\0';
	        if (*arg2=='\0') {
                    arg2 = cp1+1;
                    while (*arg2==' ') arg2++;
                }
                else {
                    arg3 = cp1+1;
                    while (*arg3==' ') arg3++;
		}
 	    }
        }

	/*
	  Switch on the first word of the string
	 */
        if (strncmp(arg1, "q",1) == 0) {
            exit_nicely();
        }
        else if (strncmp(arg1, "set",3) == 0) {
            funcVal = srbSetStorAttri (conn, FTP_AM, arg2, arg3, "");
            if (funcVal < 0)  {   /* error */
                fprintf(stderr,"%s",clErrorMessage(conn));
	    }
	}
        else if (strncmp(arg1, "cd",2) == 0) {
            funcVal = srbSetStorAttri (conn, FTP_AM, "", "", arg2);
            if (funcVal < 0)  {   /* error */
                fprintf(stderr,"%s",clErrorMessage(conn));
	    }
	}
        else if (strncmp(arg1, "ls",2) == 0) {
            ftplist();
	}
        else if (strncmp(arg1, "openr",5) == 0) {
            in_fd = srbFileOpen (conn, FTP_AM, HOST_ADDR, arg2, 
	                         O_RDONLY, 0666); 
            if (in_fd < 0)  {   /* error */
                fprintf(stderr, "can't open file \"%s\"\n", arg2);
                fprintf(stderr,"%s",clErrorMessage(conn));
	    }
            else
                printf("file fd=%d\n",in_fd);
	}
        else if (strncmp(arg1, "openw",5) == 0) {
/*          out_fd = srbFileOpen (conn, FTP_AM, HOST_ADDR, arg2, 
                                 O_WRONLY, 0666); 
 either srbFileOpen with O_WRONLY or srbFileCreate will work, tested
 each.
 */
            out_fd = srbFileCreate (conn, FTP_AM, HOST_ADDR, arg2, 
                                0666, -1); 
            if (out_fd < 0)  {   /* error */
                fprintf(stderr, "can't open file \"%s\"\n", arg2);
                fprintf(stderr,"%s",clErrorMessage(conn));
	    }
            else
                printf("file fd=%d\n",out_fd);
	}
        else if (strncmp(arg1, "read",4) == 0) {
           in_fd = atoi(arg2);
           j = atoi(arg3);
           if (j > BUFSIZE) {
	       printf("too big, reading up to %d bytes\n",BUFSIZE);
               j = BUFSIZE;
	   }
           if (j == 0) {
               j = BUFSIZE;
	       printf("Reading up to %d bytes\n",BUFSIZE);
	   }
	   nbytes = srbFileRead(conn, in_fd, buf, j);
           if (nbytes < 0 ) {
               fprintf(stderr, "error reading, error code=%d\n", nbytes);
	       fprintf(stderr,"%s",clErrorMessage(conn));
	   }
           else {
               buf[nbytes]='\0';
	       printf("%s",buf);
	   }
	}
        else if (strncmp(arg1, "write",5) == 0) {
           out_fd = atoi(arg2);
           j = atoi(arg3);
           if (j > WRITE_BUFSIZE) {
	       printf("too big, writing %d bytes\n",WRITE_BUFSIZE);
               j = WRITE_BUFSIZE;
	   }
           if (j == 0) {
               j = WRITE_BUFSIZE;
	       printf("Writing %d bytes\n",j);
	   }
	   nbytes = srbFileWrite(conn, out_fd, wbuf, j);
           if (nbytes < 0 ) {
               fprintf(stderr, "error writing, error code=%d\n", nbytes);
	       fprintf(stderr,"%s",clErrorMessage(conn));
	   }
	}
        else if (strncmp(arg1, "close",5) == 0) {
            j = atoi(arg2);
            funcVal = srbFileClose (conn, j);
            if (funcVal < 0) {
                fprintf(stderr,"srbFileClose=%d\n",funcVal);
                fprintf(stderr,"%s",clErrorMessage(conn));
            }
	}
        else if (strncmp(arg1, "seek",4) == 0) {
            j = atoi(arg2);
            funcVal = srbFileSeek (conn, j, 0, 0);
            if (funcVal < 0) {
                fprintf(stderr,"%s",clErrorMessage(conn));
            }
	}
        else if (strncmp(arg1, "sync",4) == 0) {
            j = atoi(arg2);
            funcVal = srbFileSync (conn, j);
            if (funcVal < 0) {
                fprintf(stderr,"%s",clErrorMessage(conn));
            }
	}
        else if (strncmp(arg1, "get",3) == 0) {
            ftpget(arg2,arg3);
	}
        else if (strncmp(arg1, "put",3) == 0) {
            ftpput(arg2,arg3);
	}
        else if (strncmp(arg1, "disc",4) == 0) {
            funcVal = srbSetStorAttri (conn, FTP_AM, "", "", "");
            if (funcVal < 0)  {  
                fprintf(stderr,"%s",clErrorMessage(conn));
	    }
	}
        else if (strncmp(arg1, "mkdir",5) == 0) {
	    funcVal = srbFileMkdir(conn, FTP_AM, HOST_ADDR, arg2, 0);
            if (funcVal < 0)  {  
                fprintf(stderr,"%s",clErrorMessage(conn));
	    }
	}
        else if (strncmp(arg1, "rmdir",5) == 0) {
	    funcVal = srbFileRmdir(conn, FTP_AM, HOST_ADDR, arg2);
            if (funcVal < 0)  {  
                fprintf(stderr,"%s",clErrorMessage(conn));
	    }
	}
        else if (strncmp(arg1, "del",3) == 0) {
	    funcVal = srbFileUnlink(conn, FTP_AM, HOST_ADDR, arg2);
            if (funcVal < 0)  {  
                fprintf(stderr,"%s",clErrorMessage(conn));
	    }
	}
        else if (strncmp(arg1, "?",1) == 0) {
	    do_help();
	}
	else if (strncmp(arg1, "h",1) == 0) {
	    do_help();
        }
        else {
	    printf("enter ? or h for help\n");
        }
    }
}

do_help()
{
    printf("set host user  (calls srbSetStorAttri)\n");
    printf("ls\n");
    printf("cd dir\n");
    printf("openr filename - open for read\n");
    printf("openw filename - open for write\n");
    printf("read fd nbytes - read and print the next nbytes\n");
    printf("write fd nbytes   - write nbytes more bytes\n");
    printf("seek fd        - (test) call seek (noop for ftp)\n");
    printf("sync fd        - (test) call seek (noop for ftp)\n");
    printf("get filename [localfilename] - open, read/write to copy file\n");
    printf("put filename [localfilename] - open, read/write to copy file\n");
    printf("disc           - disconnect from the ftp server\n");
    printf("close fd       - close file\n");
    printf("mkdir dir      - make directory\n");
    printf("rmdir dir      - remove directory\n");
    printf("del file       - delete (unlink) file\n");
    printf("q              - disconnect and exit\n");
    printf("? or h         - this help\n");
}

int
ftpget(char *file1, char *file2)
{
    int ofd, ifd, rval, wval, tot, funcVal;
    char *outfile;
    outfile = file2;
    if (*outfile=='\0') outfile = file1;

    ofd = open(outfile,O_WRONLY|O_CREAT|O_EXCL,0660);
    if (ofd<0)
    {
  	printf("error opening output file %s, ofd=%d\n",outfile,ofd);
        perror("open ");
        return -1;
    }
    ifd = srbFileOpen (conn, FTP_AM, HOST_ADDR, file1, 
                       O_RDONLY, 0666);
    if (ifd < 0)  {
        fprintf(stderr, "can't open file \"%s\"\n", file1);
        fprintf(stderr,"%s",clErrorMessage(conn));
        return -2;
    }

    tot = 0;
    do {
	rval = srbFileRead(conn, ifd, buf, BUFSIZE);
        if (rval < 0 ) {
            fprintf(stderr, "error reading, error code=%d\n", rval);
	    fprintf(stderr,"%s",clErrorMessage(conn));
            return;
	}
        if (rval > 0) {
            wval = write(ofd,buf,rval);
            if (wval != rval) {
	        printf("Write error\n");
		return -3;
	    }
            tot += rval;
	}
    } while (rval > 0);
    funcVal = srbFileClose (conn, ifd);
    if (funcVal < 0) {
        fprintf(stderr,"srbFileClose=%d\n",funcVal);
        fprintf(stderr,"%s",clErrorMessage(conn));
    }
    close(ofd);
    return tot;
}

int
ftpput(char *file1, char *file2)
{
    int ofd, ifd, rval, wval, tot, funcVal;
    char *infile;
    infile = file2;
    if (*infile=='\0') infile = file1;

    ifd = open(infile,O_RDONLY,0660);
    if (ifd<0)
    {
  	printf("error opening input file %s, ifd=%d\n",infile,ifd);
        perror("open ");
        return -1;
    }
        ofd = srbFileOpen (conn, FTP_AM, HOST_ADDR, file1, 
                       O_WRONLY, 0666);
    if (ofd < 0)  {
        fprintf(stderr, "can't create file \"%s\"\n", file1);
        fprintf(stderr,"%s",clErrorMessage(conn));
        return -2;
    }
    tot = 0;
    do {
	rval = read(ifd,buf,BUFSIZE);
        if (rval < 0 ) {
	    perror("Read error");
	    return -3;
	}
        if (rval > 0) {
	    wval = srbFileWrite(conn, ofd, buf, rval);
            if (wval != rval) {
                fprintf(stderr, "error writing, error code=%d\n", wval);
	        fprintf(stderr,"%s",clErrorMessage(conn));
	    }
            tot += rval;
	}
    } while (rval > 0);
    funcVal = srbFileClose (conn, ofd); 
    if (funcVal < 0) {
        fprintf(stderr,"srbFileClose=%d\n",funcVal);
        fprintf(stderr,"%s",clErrorMessage(conn));
    }
    close(ifd);
    printf("Wrote %d bytes\n",tot);
}

int 
ftplist()
{
    int funcVal, fd, nbytes;
    fd = srbFileOpen (conn, FTP_AM, HOST_ADDR, "", 
                                 O_RDONLY, 0666);
    if (fd < 0) {
        fprintf(stderr,"%s",clErrorMessage(conn));
        return;
    }
    nbytes = srbFileRead(conn, fd, buf, BUFSIZE);
    if (nbytes < 0 ) {
        fprintf(stderr, "error reading dir, error code=%d\n", nbytes);
	fprintf(stderr,"%s",clErrorMessage(conn));
    }
    else {
        buf[nbytes]='\0';
        printf("%s",buf);
    }
    funcVal = srbFileClose (conn, fd);
    if (funcVal < 0) {
        fprintf(stderr,"srbFileClose=%d\n",funcVal);
        fprintf(stderr,"%s",clErrorMessage(conn));
    }
}
