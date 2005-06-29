/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/* mdasC_db2.h -- include'd in mdasC.h */

#ifndef MDASC_DB2_H
#define MDASC_DB2_H

/* Hopefully included in mdasC.h
   #include <stdio.h>
   #include <string.h>
   #include <stdlib.h>
   #include <math.h>
*/

#include "mdasC_db2_externs.h"
#include "COPYRIGHT.h"		/* ever present */

/*
 * If you need to define a macro, please use the prefix
   MDAS_db2_ ; for example: MDAS_db2_FOO.
 * MDAS reserves macros for status[3].  100 enum values
   are available for driver status[3] values, beginning with
   MDAS_db2_S3BASE.
 * See src/include/mdasC_status2.h for available status[2] macros.
*/

#define MDAS_db2_CAT_DEL       (MDAS_db2_S3BASE +   0)
#define MDAS_db2_CAT_EXISTS    (MDAS_db2_S3BASE +   1)
#define MDAS_db2_CAT_MAKE      (MDAS_db2_S3BASE +   2)
#define MDAS_db2_CLOSE         (MDAS_db2_S3BASE +   3)
#define MDAS_db2_CONN          (MDAS_db2_S3BASE +   4)
#define MDAS_db2_DCON          (MDAS_db2_S3BASE +   5)
#define MDAS_db2_EXEC          (MDAS_db2_S3BASE +   6)
#define MDAS_db2_EXEC_TYPES    (MDAS_db2_S3BASE +   7)
#define MDAS_db2_GET           (MDAS_db2_S3BASE +   8)
#define MDAS_db2_INFO_INQUIRE  (MDAS_db2_S3BASE +   9)
#define MDAS_db2_INFO_PURGE    (MDAS_db2_S3BASE +  10)
#define MDAS_db2_INFO_REGISTER (MDAS_db2_S3BASE +  11)
#define MDAS_db2_INFO_UPDATE   (MDAS_db2_S3BASE +  12)
#define MDAS_db2_OPEN          (MDAS_db2_S3BASE +  13)
#define MDAS_db2_PUT           (MDAS_db2_S3BASE +  14)
#define MDAS_db2_READ_BLK      (MDAS_db2_S3BASE +  15)
#define MDAS_db2_WRITE_BLK     (MDAS_db2_S3BASE +  16)


/*
 * Prototypes.  Add prototypes for your private functions here.
*/




void mdasC_db2_cat_del(
	mdasC_servh* servh ,
	mdasC_infoh* cd ,
	mdasC_status status
	) ;



void mdasC_db2_cat_exists(
	mdasC_servh* servh ,
	mdasC_infoh* cd ,
	mdasC_status status
	) ;



void mdasC_db2_cat_make(
	mdasC_servh* servh ,
	mdasC_infoh* cd ,
	mdasC_status status
	) ;



void mdasC_db2_close(
	mdasC_datah* dh ,
	mdasC_status status
	) ;



void mdasC_db2_conn(
	mdasC_infoh* user ,
	mdasC_infoh* ticket ,
	mdasC_infoh* server ,
	void* comm ,
	mdasC_servh* servh ,
	mdasC_status status
	) ;



void mdasC_db2_dcon(
	mdasC_servh* servh ,
	mdasC_status status
	) ;



void mdasC_db2_exec(
	mdasC_infoh* type ,
	void* params ,
	mdasC_servh* servh ,
	void* result ,
	mdasC_status status
	) ;



void mdasC_db2_exec_types(
	mdasC_infoh* server ,
	mdasC_infoh* types ,
	mdasC_status status
	) ;



void mdasC_db2_get(
	mdasC_infoh* dset ,
	mdasC_infoh* ticket ,
	mdasC_servh* servh ,
	mdasC_infoh* cache ,
	mdasC_status status
	) ;



void mdasC_db2_info_inquire(
	mdasC_servh* servh ,
	mdasC_infoh* info ,
	mdasC_infoh* result ,
	mdasC_status status,
	int numrows
	) ;

void mdasC_db2_info_inquire_more(
	int hstmt,
	mdasC_infoh* result ,  
	mdasC_status status,
	int numrows
	) ;

void mdasC_db2_info_purge(
	mdasC_servh* servh ,
	mdasC_infoh* info ,
	mdasC_status status
	) ;



void mdasC_db2_info_register(
	mdasC_servh* servh ,
	mdasC_infoh* info ,
	mdasC_status status
	) ;



void mdasC_db2_info_update(
	mdasC_servh* servh ,
	mdasC_infoh* info ,
	mdasC_status status
	) ;



void mdasC_db2_open(
	mdasC_infoh* dset ,
	mdasC_infoh* ticket ,
	mdasC_servh* servh ,
	void* comm ,
	mdasC_infoh* mode ,
	mdasC_datah* dh ,
	mdasC_status status
	) ;



void mdasC_db2_put(
	mdasC_infoh* cache ,
	mdasC_infoh* dset ,
	mdasC_infoh* ticket ,
	mdasC_servh* servh ,
	mdasC_status status
	) ;



void mdasC_db2_read_blk(
	mdasC_datah* dh ,
	mdasC_integer count ,
	void* buffer ,
	mdasC_status status
	) ;



void mdasC_db2_write_blk(
	mdasC_integer count ,
	void* buffer ,
	mdasC_datah* dh ,
	mdasC_status status
	) ;

#endif	/* MDASC_DB2_H */
