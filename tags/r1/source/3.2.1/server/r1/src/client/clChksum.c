/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
/*-------------------------------------------------------------------------
 *
 * clChksum.c--
 *     The client authorization routines
 *
 *-------------------------------------------------------------------------
 */

#include "clChksum.h"

/* Calculate and print the checksum using sysV (SYS5) algorithm (it is faster)
   Return 0 if successful, -1 if an error occurs. */

int
clFileSum (const char *file, unsigned long *ulCheckSum)
{
  int fd;
  unsigned char buf[READ_SIZE];
  register int bytes_read;
  register unsigned long checksum = 0;

  fd = open (file, O_RDONLY);
  if (fd == -1)
    {
      fprintf(stderr,"Error opening file \"%s\" for reading (clFileSum, calculate checksum using sysV (SYS5) algorithm)\n", file);
      perror (" ");
      *ulCheckSum = 0;
      return (unixErrConv (errno));
    }

  /* Need binary I/O, or else byte counts and checksums are incorrect.  */
  SET_BINARY (fd);

  while ((bytes_read = safe_read (fd, buf, sizeof buf)) > 0)
    {
      checksum += chksumBuf (bytes_read, buf);
    }

  if (bytes_read < 0)
    {
      fprintf(stderr,"Error in reading from file \"%s\" for reading (clFileSum, calculate checksum using sysV (SYS5) algorithm)\n", file);
      perror (" ");
      close (fd);
      *ulCheckSum = 0;
      return (unixErrConv (errno));
    }

  if (close (fd) == -1)
    {
      fprintf(stderr,"Error closing file \"%s\" for reading (clFileSum, calculate checksum using sysV (SYS5) algorithm)\n", file);
      perror (" ");
      *ulCheckSum = 0;
      return (unixErrConv (errno));
    }

  *ulCheckSum = checksum % 0xffff;

  return 0;
}

int
chksumBuf (int nBytes, unsigned char *buf)
{
      register int i;
      int checksum = 0;

      for (i = 0; i < nBytes; i++)
        {
          checksum += buf[i];
        }
      return (checksum);

}

/* Read LEN bytes at PTR from descriptor DESC, retrying if interrupted.
   Return the actual number of bytes read, zero for EOF, or negative
   for an error.  */

ssize_t
safe_read (int desc, void *ptr, size_t len)
{
  ssize_t n_chars;

  if (len <= 0)
    return len;

#ifdef EINTR
  do
    {
      n_chars = read (desc, ptr, len);
    }
  while (n_chars < 0 && errno == EINTR);
#else
  n_chars = read (desc, ptr, len);
#endif

  return n_chars;
}

