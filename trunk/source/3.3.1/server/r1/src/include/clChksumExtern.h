/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/*-------------------------------------------------------------------------
 *
 * clChksum.h
 *    
 *    Definitions for network authentication routines
 *
 *-------------------------------------------------------------------------
 */
#ifndef CL_CHKSUM_EXTERN_H
#define CL_CHKSUM_EXTERN_H

#define READ_SIZE 2*1024*1024

#define CHKSUM_STR_LEN	32	/* the Max str len of the chksum str */ 

/* Retruned value of srbObjChksum */

#define CHKSUM_REG	1	/* a new chksum has been registered */

/* For systems that distinguish between text and binary I/O.
   O_BINARY is usually declared in fcntl.h  */
#if !defined O_BINARY && defined _O_BINARY
  /* For MSC-compatible compilers.  */
# define O_BINARY _O_BINARY
# define O_TEXT _O_TEXT
#endif
#if O_BINARY
# ifndef __DJGPP__
#  define setmode _setmode
#  define fileno(_fp) _fileno (_fp)
# endif /* not DJGPP */
# define SET_BINARY(_f) do {if (!isatty(_f)) setmode (_f, O_BINARY);} while (0)
# define SET_BINARY2(_f1, _f2)          \
  do {                                  \
    if (!isatty (_f1))                  \
      {                                 \
        setmode (_f1, O_BINARY);        \
        if (!isatty (_f2))              \
          setmode (_f2, O_BINARY);      \
      }                                 \
  } while(0)
#else
# define SET_BINARY(f) (void)0
# define SET_BINARY2(f1,f2) (void)0
# define O_BINARY 0
# define O_TEXT 0
#endif /* O_BINARY */

int
clFileSum (const char *file, unsigned long *ulCheckSum);
UINT32
chksumBuf (int nBytes, unsigned char *buf);

#endif /* CL_CHKSUM_EXTERN_H */
