/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
/*-------------------------------------------------------------------------
 *
 * testmem.c - test mmap call
 *
 *-------------------------------------------------------------------------
 */
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>


#define BUFSIZE 1024
#define MMFILE		"../../testfiles/mm_file"
#define INFILE		"../../testfiles/test_in"

int
main(int argc, char **argv)
{
    char buf[BUFSIZE];
    int mm_size;
    int mm_fd, in_fd;
    char *address, *tmpaddress;
    int nbytes;

    if (argc != 2) 
      {fprintf(stderr, "Usage: %s mm_size\n",argv[0]);
        exit(1);
    }
    mm_size = atoi (argv[1]);


    /* Open MMFILE */

   mm_fd = open (MMFILE,  O_RDWR | O_CREAT, 0644);
   if (mm_fd < 0)  {   /* error */
        fprintf(stderr, "can't open file\"%s\"\n",
        MMFILE);
	exit (1);
    }

    address = mmap((caddr_t) 0, mm_size, (PROT_READ | PROT_WRITE),
                         MAP_PRIVATE, mm_fd, 0);

    printf ("mmap address = %x\n", address);

    if (address == MAP_FAILED) {
        fprintf(stderr, "mmap failed \n");
	exit (1);
    }

    in_fd = open (INFILE, O_RDWR, 0644);
    tmpaddress = address;
    while ((nbytes = read(in_fd, buf, BUFSIZE)) > 0) {
	memcpy (tmpaddress, buf, nbytes);
	tmpaddress += nbytes;
	printf ("Bytes Read:%d Bytes.\n", nbytes);
    }

    munmap (address, mm_size);
    close (mm_fd);
    close (in_fd);
    
}
