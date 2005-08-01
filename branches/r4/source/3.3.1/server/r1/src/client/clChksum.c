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
#ifdef _WIN32
#include "SrbNtUtil.h"
#endif

/* Calculate and print the checksum using sysV (SYS5) algorithm (it is faster)
   Return 0 if successful, -1 if an error occurs. */

int
clFileSum (const char *file, unsigned long *ulCheckSum)
{
  int fd;
  unsigned char *buf;
  register int i, bytes_read;
  register UINT32 checksum32b = 0;
  register unsigned long ulTotalBytesRead = 0;
  UINT32 const32;

#ifndef _WIN32
  fd = open (file, O_RDONLY);
#else
  fd = SrbNtFileOpen(file,O_RDONLY, 0);
#endif
  if (fd == -1)
    {
      fprintf(stderr,"Error opening file \"%s\" for reading (clFileSum, calculate checksum using sysV (SYS5) algorithm)\n", file);
      perror (" ");
      *ulCheckSum = 0;
      return (unixErrConv (errno));
    }

  buf = malloc(READ_SIZE);

  /* Need binary I/O, or else byte counts and checksums are incorrect.  */
  SET_BINARY (fd);

  while ((bytes_read = safe_read (fd, buf, READ_SIZE)) > 0)
    {
      ulTotalBytesRead += bytes_read; 

      for (i = 0; i < bytes_read; i++)
        {
          checksum32b += buf[i];
        }
    }

  if (bytes_read < 0)
    {
      fprintf(stderr,"Error in reading from file \"%s\" for reading (clFileSum, calculate checksum using sysV (SYS5) algorithm)\n", file);
      perror (" ");
      close (fd);
      *ulCheckSum = 0;
      free (buf);
      return (unixErrConv (errno));
    }

  if (close (fd) == -1)
    {
      fprintf(stderr,"Error closing file \"%s\" for reading (clFileSum, calculate checksum using sysV (SYS5) algorithm)\n", file);
      perror (" ");
      *ulCheckSum = 0;
      free (buf);
      return (unixErrConv (errno));
    }

  /*
  *ulCheckSum = checksum % 0xffff;
  *ulCheckSum = checksum & 0xFFFF;
  *ulCheckSum = checksum % 0xFFFFUL;
  */
 
  const32 = 0xFFFF;

  checksum32b = checksum32b % const32;

  *ulCheckSum = checksum32b;
 
  free(buf);
  return 0;
}

UINT32
chksumBuf (int nBytes, unsigned char *buf)
{
      register int i;
      UINT32 checksum32 = 0;

      for (i = 0; i < nBytes; i++)
        {
          checksum32 += buf[i];
        }
      return (checksum32);

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

