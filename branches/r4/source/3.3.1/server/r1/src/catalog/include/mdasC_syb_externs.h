/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/***************************************************************************
** Source File Name = mdas_syb_externs.h
**
** Copyright (c) 1996
**   General Atomics (GA) and San Diego Supercomputer Center (SDSC)
**   San Diego, California, USA
**
** PURPOSE: 
**     Contains definitions for library functions 
**     in mdas_ora_library.c
****************************************************************************/
/****************************************************************************
                                GLOBALS
****************************************************************************/
#ifndef MDASC_SYB_EXTERNS_H
#define MDASC_SYB_EXTERNS_H
/*** mapping from db2 structures to mcat structure  ***/
#ifdef foo      /* moved to mdasGlobalsExtern */
typedef unsigned char SQLCHAR;
typedef unsigned short int SQLUSMALLINT;
typedef short int SQLSMALLINT;
typedef long     int SQLINTEGER;
typedef unsigned long     int SQLUINTEGER;
typedef int SQLRETURN;
#endif

#define SQL_SUCCESS         0

/*** mapping from db2 structures to mcat structure  ***/


typedef struct sybase_internal_info *sybase_internal_info_ptr;
typedef struct sybase_internal_info {
  long int hstmt;
  void  *data;
  void *data_size;
  sybase_internal_info_ptr next;
} sybase_internal_info;

sybase_internal_info_ptr sybQuInfo;


#ifndef MAX
#define MAX(X,Y)        (((X) > (Y)) ? (X) : (Y))
#endif

#ifndef MIN
#define MIN(X,Y)        (((X) < (Y)) ? (X) : (Y))
#endif


#endif	/* MDASC_SYB_EXTERNS_H */
