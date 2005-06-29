/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* functionHandler.h - Header file for functionHandler.c
 */

#ifndef FUNCTION_HANDLER_H
#define FUNCTION_HANDLER_H

#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
 
#include "c.h"
#include "srb.h"
#include "stubdef.h"
#include "functionHandlerExtern.h"

#ifdef SRB_SERVER
#ifdef REFACTOR
#include "dllist.h"
#include "commExtern.h"
#include "elogExtern.h"
#include "exfSwExtern.h"
#include "exfFunctExtern.h"
#include "dbLobjExtern.h"
#include "dbTableExtern.h"
#ifdef TAPE_DRIVE
#include "srbTapeMiscExtern.h"
#endif
#include "srbObjFunctExtern.h"

#include "compoundObjExtern.h"
#include "MDriverExtern.h"
#include "distribExtern.h"
#endif  /* REFACTOR */
#include "serverAll.h"
#endif

#if defined(STK_SERVER)
#include "stkServerExtern.h"
#endif

/* See the FunctCall struct for definition of each entry. For the length
 * entries, a positive value means the length in bytes. A -1 means
 * variable length. A zero means no value.
 */
 
#ifdef SRB_SERVER
FunctCall functBuiltin[] = {
#else
FunctCall clFunctBuiltin[] = {
#endif
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_CREATE, (func_ptr)svrFileCreate, 
#else
        {F_E_CREATE, (func_ptr)NULL, 
#endif
	"svrFileCreate", SIZEOF32, 6, SIZEOF32, -1, -1, SIZEOF32, SIZEOF32, 
	2*SIZEOF32, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_OPEN, (func_ptr)svrFileOpen, 
#else
        {F_E_OPEN, (func_ptr)NULL, 
#endif
	"svrFileOpen", SIZEOF32, 6, SIZEOF32, -1, -1, SIZEOF32, SIZEOF32, 
	SIZEOF32, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_CLOSE, (func_ptr)svrFileClose, 
#else
        {F_E_CLOSE, (func_ptr)NULL, 
#endif
	"svrFileClose", SIZEOF32, 1, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_UNLINK, (func_ptr)svrFileUnlink, 
#else
        {F_E_UNLINK, (func_ptr)NULL, 
#endif
	"e_unlink", SIZEOF32, 4, SIZEOF32, -1, -1, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_READ, (func_ptr)svrFileRead, 
#else
        {F_E_READ, (func_ptr)NULL, 
#endif
	"svrFileRead", -1, 2, SIZEOF32, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_WRITE, (func_ptr)svrFileWrite, 
#else
        {F_E_WRITE, (func_ptr)NULL, 
#endif
	"svrFileWrite", SIZEOF32, 2, SIZEOF32, -1, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_SEEK, (func_ptr)svrFileSeek, 
#else
        {F_E_SEEK, (func_ptr)NULL, 
#endif
	"svrFileSeek", 2*SIZEOF32, 3, SIZEOF32, 2*SIZEOF32, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_SYNC, (func_ptr)svrFileSync, 
#else
        {F_E_SYNC, (func_ptr)NULL, 
#endif
	"svrFileSync", SIZEOF32, 1, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_STAT, (func_ptr)svrFileStat, 
#else
        {F_E_STAT, (func_ptr)NULL, 
#endif
	"svrFileStat", -1, 3, SIZEOF32, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_MKDIR, (func_ptr)svrFileMkdir, 
#else
        {F_E_MKDIR, (func_ptr)NULL, 
#endif
	"svrFileMkdir", SIZEOF32, 5, SIZEOF32, -1, -1, SIZEOF32, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_CHMOD, (func_ptr)svrFileChmod, 
#else
        {F_E_CHMOD, (func_ptr)NULL, 
#endif
	"svrFileChmod", SIZEOF32, 5, SIZEOF32, -1, -1, SIZEOF32, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_RMDIR, (func_ptr)svrFileRmdir, 
#else
        {F_E_RMDIR, (func_ptr)NULL, 
#endif
	"svrFileRmdir", SIZEOF32, 4, SIZEOF32, -1, -1, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_OPENDIR, (func_ptr)svrOpendir, 
#else
        {F_E_OPENDIR, (func_ptr)NULL, 
#endif
	"svrOpendir", SIZEOF32, 4, SIZEOF32, -1, -1, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_READDIR, (func_ptr)svrReaddir, 
#else
        {F_E_READDIR, (func_ptr)NULL, 
#endif
	"svrReaddir", -1, 1, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_CLOSEDIR, (func_ptr)svrClosedir, 
#else
        {F_E_CLOSEDIR, (func_ptr)NULL, 
#endif
	"svrClosedir", SIZEOF32, 1, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_SETSTORATTRI, (func_ptr)svrSetStorAttri, 
#else
        {F_E_SETSTORATTRI, (func_ptr)NULL, 
#endif
	"svrSetStorAttri", SIZEOF32, 4, SIZEOF32, -1, -1, -1, 
	0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_VAULT_INFO, (func_ptr)svrVaultInfo, 
#else
        {F_E_VAULT_INFO, (func_ptr)NULL, 
#endif
	"svrVaultInfo", -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_GET_SVR_VER, (func_ptr)svrGetVersion, 
#else
        {F_GET_SVR_VER, (func_ptr)NULL, 
#endif
	"svrGetVersion", -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_MIGRATE, (func_ptr)svrFileMigrate, 
#else
        {F_E_MIGRATE, (func_ptr)NULL, 
#endif
	"svrFileMigrate", SIZEOF32, 3,SIZEOF32, SIZEOF32, SIZEOF32, 0, 0,
	 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_STAGE, (func_ptr)svrFileStage, 
#else
        {F_E_STAGE, (func_ptr)NULL, 
#endif
	"svrFileStage", SIZEOF32, 5, SIZEOF32, SIZEOF32, 
	 SIZEOF32, SIZEOF32, SIZEOF32, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_PURGE, (func_ptr)svrFilePurge, 
#else
        {F_E_PURGE, (func_ptr)NULL, 
#endif
	"svrFilePurge", SIZEOF32, 5, SIZEOF32, SIZEOF32, 
	 SIZEOF32, SIZEOF32, SIZEOF32, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_FSTAT, (func_ptr)svrFileFstat,
#else
        {F_E_FSTAT, (func_ptr)NULL,
#endif
        "svrFileFstat", -1, 3, SIZEOF32, -1, SIZEOF32, 0, 0, 0, 0, 0, 0, 
	 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_CHKSUM, (func_ptr)svrFileChksum,
#else
        {F_E_CHKSUM, (func_ptr)NULL,
#endif
        "svrFileChksum", -1, 4, SIZEOF32, SIZEOF32, 2*SIZEOF32, 2*SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_GET_FS_FREESPACE, (func_ptr)svrGetFsFreeSpace,
#else
        {F_E_GET_FS_FREESPACE, (func_ptr)NULL,
#endif
        "svrGetFsFreeSpace", 2*SIZEOF32, 4, SIZEOF32, -1, -1, 2*SIZEOF32,
        0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_CREATE, (func_ptr)svrObjCreate, 
#else
        {F_SRBO_CREATE, (func_ptr)NULL, 
#endif
	"svrObjCreate", SIZEOF32, 7, SIZEOF32, -1, -1, -1, -1, -1, 2*SIZEOF32, 
	0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_OPEN, (func_ptr)svrObjOpen, 
#else
        {F_SRBO_OPEN, (func_ptr)NULL, 
#endif
	"svrObjOpen", SIZEOF32, 3, -1, SIZEOF32, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_CLOSE, (func_ptr)svrObjClose, 
#else
        {F_SRBO_CLOSE, (func_ptr)NULL, 
#endif
	"svrObjClose", SIZEOF32, 1, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_UNLINK, (func_ptr)svrObjUnlink, 
#else
        {F_SRBO_UNLINK, (func_ptr)NULL, 
#endif
	"svrObjUnlink", SIZEOF32, 2, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_READ, (func_ptr)svrObjRead, 
#else
        {F_SRBO_READ, (func_ptr)NULL, 
#endif
	"svrObjRead", -1, 2, SIZEOF32, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_WRITE, (func_ptr)svrObjWrite, 
#else
        {F_SRBO_WRITE, (func_ptr)NULL, 
#endif
	"svrObjWrite", SIZEOF32, 2, SIZEOF32, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_SEEK, (func_ptr)svrObjSeek, 
#else
        {F_SRBO_SEEK, (func_ptr)NULL, 
#endif
	"svrObjSeek", 2*SIZEOF32, 3, SIZEOF32, 2*SIZEOF32, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_SYNC, (func_ptr)svrObjSync, 
#else
        {F_SRBO_SYNC, (func_ptr)NULL, 
#endif
	"svrObjSync", SIZEOF32, 1, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_STAT, (func_ptr)svrObjStat, 
#else
        {F_SRBO_STAT, (func_ptr)NULL, 
#endif
	"svrObjStat", -1, 3, SIZEOF32, -1, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_REPLICATE, (func_ptr)svrObjReplicate, 
#else
        {F_SRBO_REPLICATE, (func_ptr)NULL, 
#endif
	"svrObjReplicate", SIZEOF32, 5, SIZEOF32, -1, -1, -1, -1, 
	0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_MOVE, (func_ptr)svrObjMove, 
#else
        {F_SRBO_MOVE, (func_ptr)NULL, 
#endif
	"svrObjMove", SIZEOF32, 7, SIZEOF32, -1, -1, -1, -1, 
	-1, -1, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_PROXY_OPR, (func_ptr)svrObjProxyOpr, 
#else
        {F_SRBO_PROXY_OPR, (func_ptr)NULL, 
#endif
	"svrObjProxyOpr", -1, 12, SIZEOF32, SIZEOF32, SIZEOF32, SIZEOF32, 
	SIZEOF32, -1, -1, -1, -1, -1, -1, -1},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_GET_DENTS, (func_ptr)svrObjGetdents,
#else
        {F_SRBO_GET_DENTS, (func_ptr)NULL,
#endif
        "svrObjGetdents", -1, 3, SIZEOF32, SIZEOF32, SIZEOF32, 
	 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_GET_DENTS64, (func_ptr)svrObjGetdents64,
#else
        {F_SRBO_GET_DENTS64, (func_ptr)NULL,
#endif
        "svrObjGetdents64", -1, 3, SIZEOF32, SIZEOF32, SIZEOF32, 
	 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_COPY, (func_ptr)svrObjCopy,
#else
        {F_SRBO_COPY, (func_ptr)NULL,
#endif
        "svrObjCopy", SIZEOF32*2, 5, -1, -1, -1, -1, -1,
         0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DATA_PUT, (func_ptr)svrDataPut,
#else
        {F_DATA_PUT, (func_ptr)NULL,
#endif
        "svrDataPut", 2*SIZEOF32, 6, SIZEOF32, -1, -1, -1, SIZEOF32, 2*SIZEOF32,
         0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_PUT, (func_ptr)svrObjPut,
#else
        {F_SRBO_PUT, (func_ptr)NULL,
#endif
        "svrObjPut", 2*SIZEOF32, 9, -1, -1, -1, -1, SIZEOF32, -1, -1, 
	 2*SIZEOF32, SIZEOF32, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_GET, (func_ptr)svrObjGet,
#else
        {F_SRBO_GET, (func_ptr)NULL,
#endif
        "svrObjGet", 2*SIZEOF32, 4, -1, -1, -1, SIZEOF32, 0, 0, 0,
         0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DATA_GET, (func_ptr)svrDataGet,
#else
        {F_DATA_GET, (func_ptr)NULL,
#endif
        "svrDataGet", 2*SIZEOF32, 6, SIZEOF32, -1, -1, -1, SIZEOF32, 
	2*SIZEOF32, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBC_SEEK, (func_ptr)svrCollSeek,
#else
        {F_SRBC_SEEK, (func_ptr)NULL,
#endif
        "svrCollSeek", SIZEOF32, 4, SIZEOF32, SIZEOF32, SIZEOF32, SIZEOF32,
	0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_CHKSUM, (func_ptr)svrObjChksum,
#else
        {F_SRBO_CHKSUM, (func_ptr)NULL,
#endif
        "svrObjChksum", -1, 4, -1, -1, SIZEOF32, -1,
        0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_GET_LOID_INFO, (func_ptr)svrGetDatasetInfo, 
#else
        {F_SRBO_GET_LOID_INFO, (func_ptr)NULL, 
#endif
	"svrGetDatasetInfo", -1, 5, SIZEOF32, -1, -1, SIZEOF32, 
	-1, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_GET_DATADIR_INFO, (func_ptr)svrGetDataDirInfo, 
#else
        {F_SRBO_GET_DATADIR_INFO, (func_ptr)NULL, 
#endif
	"svrGetDataDirInfo", -1, 4, SIZEOF32, -1, -1, SIZEOF32, 0, 0, 0, 0, 0, 
	0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_REGISTER_DATASET, (func_ptr)svrRegisterDataset, 
#else
        {F_SRBO_REGISTER_DATASET, (func_ptr)NULL, 
#endif
	"svrRegisterDataset", SIZEOF32, 7, SIZEOF32, -1, -1, -1, -1, -1, 
	2*SIZEOF32, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_MODIFY_DATASET, (func_ptr)svrModifyDataset, 
#else
        {F_SRBO_MODIFY_DATASET, (func_ptr)NULL, 
#endif
	"svrModifyDataset", SIZEOF32, 8, SIZEOF32, -1, -1, -1, -1, -1, -1,
	SIZEOF32, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CHK_MDAS_AUTH, (func_ptr)svrChkMdasAuth, 
#else
        {F_CHK_MDAS_AUTH, (func_ptr)NULL, 
#endif
	"svrChkMdasAuth", SIZEOF32, 3, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0},

#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CHK_ENCRYPT1_AUTH, (func_ptr)svrChkEncryptAuth, 
#else
        {F_CHK_ENCRYPT1_AUTH, (func_ptr)NULL, 
#endif
	"svrChkEncryptAuth", SIZEOF32, 6, -1, -1, SIZEOF32, SIZEOF32, -1, -1, 0, 0, 0, 0, 0, 0},

#ifdef LOCK_OBJ
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_LOCK, (func_ptr)svrObjLock,
#else
        {F_SRBO_LOCK, (func_ptr)NULL,
#endif
        "svrObjLock", SIZEOF32, 5, -1, -1, SIZEOF32, SIZEOF32, SIZEOF32, 0, 0, 
	 0, 0, 0, 0, 0},
#endif	/* LOCK_OBJ */

#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_BULK_REGISTER, (func_ptr)svrBulkRegister,
#else
        {F_BULK_REGISTER, (func_ptr)NULL,
#endif
        "svrBulkRegister", SIZEOF32, 4, SIZEOF32, -1, -1, SIZEOF32, 0, 0, 0,
 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_SYNC_DATA, (func_ptr)svrSyncData,
#else
        {F_SRBO_SYNC_DATA, (func_ptr)NULL,
#endif
        "svrSyncData", SIZEOF32, 4, SIZEOF32, -1, -1, -1, 0, 0, 0, 0,
	0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MOD_RESC_INFO, (func_ptr)svrModifyRescInfo,
#else
        {F_MOD_RESC_INFO, (func_ptr)NULL,
#endif
        "svrModifyRescInfo", SIZEOF32, 7, SIZEOF32, -1, SIZEOF32, -1, -1, -1, 
	-1, 0, 0, 0, 0, 0},

#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_REG_DATASET_INT, (func_ptr)svrRegDatasetInternal,
#else
        {F_SRBO_REG_DATASET_INT, (func_ptr)NULL,
#endif
        "svrRegDatasetInternal", SIZEOF32, 12, SIZEOF32, -1, -1, -1, -1, -1,
        -1, -1, -1, 2*SIZEOF32, SIZEOF32, -1},

#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CONTAINER_OPEN, (func_ptr)svrContainerOpen,
#else
        {F_CONTAINER_OPEN, (func_ptr)NULL,
#endif
        "svrContainerOpen", SIZEOF32, 3, SIZEOF32, -1, SIZEOF32,
         0, 0, 0, 0, 0, 0, 0, 0, 0},

#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CONTAINER_CLOSE, (func_ptr)svrContainerClose,
#else
        {F_CONTAINER_CLOSE, (func_ptr)NULL,
#endif
        "svrContainerClose", SIZEOF32, 1, SIZEOF32, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0},

#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DATA_COPY, (func_ptr)svrDataCopy,
#else
        {F_DATA_COPY, (func_ptr)NULL,
#endif
        "svrDataCopy", 2*SIZEOF32, 8, SIZEOF32, -1, -1,
	 SIZEOF32, -1, -1, 2*SIZEOF32, SIZEOF32, 0, 0, 0, 0},

#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CHK_MDAS_SYS_AUTH, (func_ptr)svrChkMdasSysAuth, 
#else
        {F_CHK_MDAS_SYS_AUTH, (func_ptr)NULL, 
#endif
	"svrChkMdasSysAuth", SIZEOF32, 3, -1, -1, -1, 
	0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REGISTER_USER_GROUP, (func_ptr)svrRegisterUserGrp, 
#else
        {F_REGISTER_USER_GROUP, (func_ptr)NULL, 
#endif
	"svrRegisterUserGrp", SIZEOF32, 7, SIZEOF32, -1, -1, -1, -1, -1, -1, 
	0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REGISTER_USER, (func_ptr)svrRegisterUser, 
#else
        {F_REGISTER_USER, (func_ptr)NULL, 
#endif
	"svrRegisterUser", SIZEOF32, 8, SIZEOF32, -1, -1, -1, -1, -1, -1, -1, 
	0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MODIFY_USER, (func_ptr)svrModifyUser, 
#else
        {F_MODIFY_USER, (func_ptr)NULL, 
#endif
	"svrModifyUser", SIZEOF32, 4, SIZEOF32, -1, -1, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CREATE_COLLECTION, (func_ptr)svrCreateCollect, 
#else
        {F_CREATE_COLLECTION, (func_ptr)NULL, 
#endif
	"svrCreateCollect", SIZEOF32, 3, SIZEOF32, -1, -1, 
	0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_LIST_COLLECTION, (func_ptr)svrListCollect, 
#else
        {F_LIST_COLLECTION, (func_ptr)NULL, 
#endif
	"svrListCollect", -1, 4, SIZEOF32, -1, -1, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MODIFY_COLLECTION, (func_ptr)svrModifyCollect, 
#else
        {F_MODIFY_COLLECTION, (func_ptr)NULL, 
#endif
	"svrModifyCollect", SIZEOF32, 6, SIZEOF32, -1, -1, -1, -1, SIZEOF32, 
	0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SET_AUDIT_TRAIL, (func_ptr)set_audit_trail, 
#else
        {F_SET_AUDIT_TRAIL, (func_ptr)NULL, 
#endif
	"set_audit_trail", SIZEOF32, 1, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_AUDIT, (func_ptr)svrObjAudit, 
#else
        {F_SRBO_AUDIT, (func_ptr)NULL, 
#endif
	"svrObjAudit",
        SIZEOF32, 10, SIZEOF32, -1, -1, -1, -1, -1, -1, -1, SIZEOF32, -1, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REGISTER_REPLICA, (func_ptr)svrRegisterReplica, 
#else
        {F_REGISTER_REPLICA, (func_ptr)NULL, 
#endif
	"svrRegisterReplica",
        SIZEOF32, 9, SIZEOF32, -1, -1, -1, -1, -1, -1, -1,  -1, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_GET_PRIV_USERS, (func_ptr)svrGetPrivUsers, 
#else
        {F_GET_PRIV_USERS, (func_ptr)NULL, 
#endif
	"svrGetPrivUsers", -1, 2, SIZEOF32, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_GET_MORE_ROWS, (func_ptr)svrGetMoreRows, 
#else
        {F_GET_MORE_ROWS, (func_ptr)NULL, 
#endif
	"svrGetMoreRows", -1, 3, SIZEOF32, SIZEOF32, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_ISSUE_TICKET, (func_ptr)svrIssueTicket, 
#else
        {F_ISSUE_TICKET, (func_ptr)NULL, 
#endif
	"svrIssueTicket", -1, 7, -1, -1, -1, -1, -1, SIZEOF32, -1, 
	0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REMOVE_TICKET, (func_ptr)svrRemoveTicket, 
#else
        {F_REMOVE_TICKET, (func_ptr)NULL, 
#endif
	"svrRemoveTicket", SIZEOF32, 1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_UNREGISTER_DATASET, (func_ptr)svrUnregisterDataset, 
#else
        {F_UNREGISTER_DATASET, (func_ptr)NULL, 
#endif
	"svrUnregisterDataset", SIZEOF32, 2, -1, -1, 
	 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CONTAINER_CREATE, (func_ptr)svrContainerCreate,
#else
        {F_CONTAINER_CREATE, (func_ptr)NULL,
#endif
        "svrContainerCreate", SIZEOF32, 5, SIZEOF32, -1,
         -1, -1, 2*SIZEOF32, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REGISTER_CONTAINER, (func_ptr)svrRegisterContainer,
#else
        {F_REGISTER_CONTAINER, (func_ptr)NULL,
#endif
        "svrRegisterContainer", SIZEOF32, 4, SIZEOF32, -1,
         -1, 2*SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REGISTER_IN_CONTAINER, (func_ptr)svrRegisterInContDataset,
#else
        {F_REGISTER_IN_CONTAINER, (func_ptr)NULL,
#endif
        "svrRegisterInContDataset", SIZEOF32, 7, SIZEOF32, -1,
         -1, -1, -1, 2*SIZEOF32, 2*SIZEOF32, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_GET_CONTAINER_INFO, (func_ptr)svrGetContainerInfo,
#else
        {F_GET_CONTAINER_INFO, (func_ptr)NULL,
#endif
        "svrGetContainerInfo", -1, 3, SIZEOF32, -1,
         SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_GET_RESC_ON_CHOICE, (func_ptr)svrGetResOnChoice,
#else
        {F_GET_RESC_ON_CHOICE, (func_ptr)NULL,
#endif
        "svrGetResOnChoice", -1, 3, SIZEOF32, -1,
         -1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REMOVE_CONTAINER, (func_ptr)svrRmContainer,
#else
        {F_REMOVE_CONTAINER, (func_ptr)NULL,
#endif
        "svrRmContainer", SIZEOF32, 3, SIZEOF32, -1,
         SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SYNC_CONTAINER, (func_ptr)svrSyncContainer,
#else
        {F_SYNC_CONTAINER, (func_ptr)NULL,
#endif
        "svrSyncContainer", SIZEOF32, 3, SIZEOF32, -1,
         SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REPL_CONTAINER, (func_ptr)svrReplContainer,
#else
        {F_REPL_CONTAINER, (func_ptr)NULL,
#endif
        "svrReplContainer", SIZEOF32, 3, SIZEOF32, -1,
         -1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_GET_HOST_CONFIG, (func_ptr)svrGetHostConfig, 
#else
        {F_GET_HOST_CONFIG, (func_ptr)NULL, 
#endif
	"svrGetHostConfig", -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_LOBJ_CREATE, (func_ptr)dbLobjCreate, 
#else
        {F_DB_LOBJ_CREATE, (func_ptr)NULL, 
#endif
	"dbLobjCreate", SIZEOF32, 4, SIZEOF32, -1, -1, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_LOBJ_OPEN, (func_ptr)dbLobjOpen, 
#else
        {F_DB_LOBJ_OPEN, (func_ptr)NULL, 
#endif
	"dbLobjOpen", SIZEOF32, 5, SIZEOF32, -1, -1, SIZEOF32, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_LOBJ_CLOSE, (func_ptr)dbLobjClose, 
#else
        {F_DB_LOBJ_CLOSE, (func_ptr)NULL, 
#endif
	"dbLobjClose", SIZEOF32, 1, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_LOBJ_READ, (func_ptr)dbLobjRead, 
#else
        {F_DB_LOBJ_READ, (func_ptr)NULL, 
#endif
	"dbLobjRead", -1, 2, SIZEOF32, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_LOBJ_WRITE, (func_ptr)dbLobjWrite, 
#else
        {F_DB_LOBJ_WRITE, (func_ptr)NULL, 
#endif
	"dbLobjWrite", SIZEOF32, 2, SIZEOF32, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_LOBJ_SEEK, (func_ptr)dbLobjSeek, 
#else
        {F_DB_LOBJ_SEEK, (func_ptr)NULL, 
#endif
	"dbLobjSeek", SIZEOF32, 3, SIZEOF32, SIZEOF32, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_LOBJ_UNLINK, (func_ptr)dbLobjUnlink, 
#else
        {F_DB_LOBJ_UNLINK, (func_ptr)NULL, 
#endif
	"dbLobjUnlink", SIZEOF32, 3, SIZEOF32, -1, -1, 
	0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_TABLE_CREATE, (func_ptr)dbTableCreate, 
#else
        {F_DB_TABLE_CREATE, (func_ptr)NULL, 
#endif
	"dbTableCreate", SIZEOF32, 4, SIZEOF32, -1, -1, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_TABLE_OPEN, (func_ptr)dbTableOpen, 
#else
        {F_DB_TABLE_OPEN, (func_ptr)NULL, 
#endif
	"dbTableOpen", SIZEOF32, 5, SIZEOF32, -1, -1, SIZEOF32, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_TABLE_CLOSE, (func_ptr)dbTableClose, 
#else
        {F_DB_TABLE_CLOSE, (func_ptr)NULL, 
#endif
	"dbTableClose", SIZEOF32, 1, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_TABLE_READ, (func_ptr)dbTableRead, 
#else
        {F_DB_TABLE_READ, (func_ptr)NULL, 
#endif
	"dbTableRead", -1, 2, SIZEOF32, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_TABLE_WRITE, (func_ptr)dbTableWrite, 
#else
        {F_DB_TABLE_WRITE, (func_ptr)NULL, 
#endif
	"dbTableWrite", SIZEOF32, 2, SIZEOF32, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_TABLE_SEEK, (func_ptr)dbTableSeek, 
#else
        {F_DB_TABLE_SEEK, (func_ptr)NULL, 
#endif
	"dbTableSeek", SIZEOF32, 3, SIZEOF32, SIZEOF32, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DB_TABLE_UNLINK, (func_ptr)dbTableUnlink, 
#else
        {F_DB_TABLE_UNLINK, (func_ptr)NULL, 
#endif
	"dbTableUnlink", SIZEOF32, 3, SIZEOF32, -1, -1, 
	0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(STK_SERVER)
        {F_TAPELIB_MNT_CART, (func_ptr)svrTapelibMntCart,
#else
        {F_TAPELIB_MNT_CART, (func_ptr)NULL,
#endif
        "svrTapelibMntCart", SIZEOF32, 3, -1, SIZEOF32, SIZEOF32,
        0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(STK_SERVER)
        {F_TAPELIB_DISMNT_CART, (func_ptr)svrTapelibDismntCart,
#else
        {F_TAPELIB_DISMNT_CART, (func_ptr)NULL,
#endif
        "svrTapelibDismntCart", SIZEOF32, 2, -1, SIZEOF32, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(STK_SERVER)
        {F_TAPELIB_GET_CART_PRI, (func_ptr)svrGetTapeCartPri,
#else
        {F_TAPELIB_GET_CART_PRI, (func_ptr)NULL,
#endif
        "svrGetTapeCartPri", -1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CMP_DUMP_FILE_LIST, (func_ptr)svrDumpFileList,
#else
        {F_CMP_DUMP_FILE_LIST, (func_ptr)NULL,
#endif
        "svrDumpFileList", SIZEOF32, 3, SIZEOF32, -1, SIZEOF32, 
        0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CMP_STAGE_COMP_OBJ, (func_ptr)svrStageCompObj,
#else
        {F_CMP_STAGE_COMP_OBJ, (func_ptr)NULL,
#endif
        "svrDumpFileList", -1, 9, -1, -1, -1, -1, 2*SIZEOF32, 2*SIZEOF32, 
	SIZEOF32, SIZEOF32, SIZEOF32, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CMP_REG_INT_COMP_OBJ, (func_ptr)svrRegInternalCompObj,
#else
        {F_CMP_REG_INT_COMP_OBJ, (func_ptr)NULL,
#endif
        "svrRegInternalCompObj", SIZEOF32, 12, -1, -1, SIZEOF32, SIZEOF32, 
        -1, -1, 2*SIZEOF32, 2*SIZEOF32, SIZEOF32, SIZEOF32, SIZEOF32, -1},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CMP_RM_INT_COMP_OBJ, (func_ptr)svrRmInternalCompObj,
#else
        {F_CMP_RM_INT_COMP_OBJ, (func_ptr)NULL,
#endif
        "svrRmInternalCompObj", SIZEOF32, 6, -1, -1, SIZEOF32, SIZEOF32, 
	SIZEOF32, SIZEOF32, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CMP_RM_COMP_OBJ, (func_ptr)svrRmCompObj,
#else
        {F_CMP_RM_COMP_OBJ, (func_ptr)NULL,
#endif
        "svrRmCompObj", SIZEOF32, 4, -1, -1, SIZEOF32, SIZEOF32, 
        0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_CMP_MOD_INT_COMP_OBJ, (func_ptr)svrModInternalCompObj,
#else
        {F_CMP_MOD_INT_COMP_OBJ, (func_ptr)NULL,
#endif
        "svrModInternalCompObj", SIZEOF32, 11, -1, -1, SIZEOF32, SIZEOF32, 
        SIZEOF32, SIZEOF32, -1, -1, -1, -1, SIZEOF32, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REGISTER_LOCATION, (func_ptr)svrRegisterLocation, 
#else
        {F_REGISTER_LOCATION, (func_ptr)NULL, 
#endif
	 "srbRegisterLocation", SIZEOF32, 5,
         -1, -1, -1, -1,
         -1,  0,  0,  0,
          0,  0,  0,  0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_INGEST_TOKEN, (func_ptr)svrIngestToken, 
#else
        {F_INGEST_TOKEN, (func_ptr)NULL, 
#endif
	 "srbIngestToken", SIZEOF32, 3,
         -1, -1, -1,  0,
          0,  0,  0,  0,
          0,  0,  0,  0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REGISTER_RESOURCE, (func_ptr)svrRegisterResource, 
#else
        {F_REGISTER_RESOURCE, (func_ptr)NULL, 
#endif
	 "srbRegisterResource", SIZEOF32, 6,
         -1, -1, -1, -1,
         -1,  SIZEOF32,  0,  0,
          0,  0,  0,  0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REGISTER_LOGICAL_RESOURCE, (func_ptr)svrRegisterLogicalResource, 
#else
        {F_REGISTER_LOGICAL_RESOURCE, (func_ptr)NULL, 
#endif
	 "srbRegisterLogicalResource", SIZEOF32, 4,
         -1, -1, -1, -1,
          0,  0,  0,  0,
          0,  0,  0,  0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REGISTER_REPLICATE_RESOURCE_INFO, 
          (func_ptr)svrRegisterReplicateResourceInfo, 
#else
        {F_REGISTER_REPLICATE_RESOURCE_INFO, 
	 (func_ptr)NULL, 
#endif
	 "srbRegisterReplicateResourceInfo", SIZEOF32, 4,
         -1, -1, -1, -1,
          0,  0,  0,  0,
          0,  0,  0,  0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DELETE_VALUE, 
          (func_ptr)svrDeleteValue, 
#else
        {F_DELETE_VALUE, 
	 (func_ptr)NULL, 
#endif
	 "srbDeleteValue", SIZEOF32, 2,
          SIZEOF32, -1, 0, 0,
          0,  0,  0,  0,
          0,  0,  0,  0},
#ifdef FED_MCAT
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_GET_MCAT_NAME,
          (func_ptr)svrGetMcatZone,
#else
        {F_GET_MCAT_NAME,
         (func_ptr)NULL,
#endif
         "svrGetMcatZone", -1, 2,
          -1, -1, 0, 0,
          0,  0,  0,  0,
          0,  0,  0,  0},
#endif	/* FED_MCAT */
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SETUP_SESSION_PUBLIC_KEY, 
          (func_ptr)svrSetupSessionPublicKey, 
#else
        {F_SETUP_SESSION_PUBLIC_KEY, 
	 (func_ptr)NULL, 
#endif
	 "srbSetupSessionPublicKey", -1, 0,
          0,  0,  0,  0,
          0,  0,  0,  0,
          0,  0,  0,  0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SETUP_SESSION, 
          (func_ptr)svrSetupSession, 
#else
        {F_SETUP_SESSION, 
	 (func_ptr)NULL, 
#endif
	 "srbSetupSession", SIZEOF32, 1,
         -1,  0,  0,  0,
          0,  0,  0,  0,
          0,  0,  0,  0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_BULK_LOAD,
          (func_ptr) svrBulkLoad,
#else
        {F_BULK_LOAD,
         (func_ptr)NULL,
#endif
         "svrBulkLoad", SIZEOF32, 3,
          SIZEOF32,  -1,  -1,  0,
          0,  0,  0,  0,
          0,  0,  0,  0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_GEN_GET_MORE_ROWS, (func_ptr)svrGenGetMoreRows,
#else
        {F_GEN_GET_MORE_ROWS, (func_ptr)NULL,
#endif
        "svrGenGetMoreRows", -1, 3, SIZEOF32, SIZEOF32, SIZEOF32,
        0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MODIFY_ZONE, (func_ptr)svrModifyZone, 
#else
        {F_MODIFY_ZONE, (func_ptr)NULL, 
#endif
	"svrModifyZone", SIZEOF32, 8, SIZEOF32, -1, -1, -1, -1,  -1, -1, 
        SIZEOF32,  0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_BULK_QUERY_ANSWER, (func_ptr)svrBulkQueryAnswer, 
#else
        {F_BULK_QUERY_ANSWER, (func_ptr)NULL, 
#endif
	"svrBulkQueryAnswer", -1, 3, SIZEOF32, -1,  SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_BULK_MCAT_INGEST,
          (func_ptr) svrBulkMcatIngest,
#else
        {F_BULK_MCAT_INGEST,
         (func_ptr)NULL,
#endif
         "svrBulkMcatIngest", SIZEOF32, 3,
          SIZEOF32,  -1,  -1,  0,
          0,  0,  0,  0,
          0,  0,  0,  0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_GEN_QUERY, (func_ptr)svrGenQuery,
#else
        {F_GEN_QUERY, (func_ptr)NULL,
#endif
        "svrGenQuery", -1, 4, SIZEOF32, -1, -1, SIZEOF32, 0, 0, 0, 0, 0,
        0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_BULK_UNLOAD, (func_ptr)svrBulkUnload,
#else
        {F_BULK_UNLOAD, (func_ptr)NULL,
#endif
        "svrBulkUnload", SIZEOF32, 5, SIZEOF32, SIZEOF32, -1, SIZEOF32, -1,
        0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_BULK_UNLOAD_C, (func_ptr)svrBulkUnloadC,
#else
        {F_BULK_UNLOAD_C, (func_ptr)NULL,
#endif
        "svrBulkUnload", -1, 3, SIZEOF32, SIZEOF32, -1, 0, 0,
        0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_BACKUP_DATA, (func_ptr)svrBackupData,
#else
        {F_BACKUP_DATA, (func_ptr)NULL,
#endif
        "svrBackupData", SIZEOF32, 5, SIZEOF32, -1, -1, -1, SIZEOF32,
        0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REMOVE_TICKET_WITH_ZONE, (func_ptr)svrRemoveTicketWithZone,
#else
        {F_REMOVE_TICKET_WITH_ZONE, (func_ptr)NULL,
#endif
        "svrRemoveTicketWithZone", SIZEOF32, 2, -1, -1, 0, 0, 0, 0, 
	 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MODIFY_USER_NP, (func_ptr)svrModifyUserNonPriv, 
#else
        {F_MODIFY_USER_NP, (func_ptr)NULL, 
#endif
	"svrModifyUserNonPriv", SIZEOF32, 8, SIZEOF32, -1, -1, -1, -1,  -1, -1, 
        SIZEOF32,  0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MODIFY_RESOURCE, (func_ptr)svrModifyResource, 
#else
        {F_MODIFY_RESOURCE, (func_ptr)NULL, 
#endif
	"svrModifyResource", SIZEOF32, 7, SIZEOF32, -1, -1, -1, -1,  -1,  
        SIZEOF32,  0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_GET_C, (func_ptr)svrObjGetC,
#else
        {F_SRBO_GET_C, (func_ptr)NULL,
#endif
        "svrObjGetC", -1, 4, -1, -1, SIZEOF32, SIZEOF32, 0, 0, 
	 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DATA_GET_C, (func_ptr)svrDataGetC,
#else
        {F_DATA_GET_C, (func_ptr)NULL,
#endif
        "svrDataGetC", -1, 6, SIZEOF32, -1, -1, 2*SIZEOF32, SIZEOF32, SIZEOF32,
         0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_PUT_C, (func_ptr)svrObjPutC,
#else
        {F_SRBO_PUT_C, (func_ptr)NULL,
#endif
        "svrObjPutC", -1, 8, -1, -1, -1, -1, -1, 2*SIZEOF32,
         SIZEOF32, SIZEOF32, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_DATA_PUT_C, (func_ptr)svrDataPutC,
#else
        {F_DATA_PUT_C, (func_ptr)NULL,
#endif
        "svrDataPutC", -1, 6, SIZEOF32, -1, -1, 2*SIZEOF32, SIZEOF32, SIZEOF32,
         0, 0, 0, 0, 0, 0},
        {F_DUMMY, (func_ptr)NULL, "dummy", 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_FSTAGE, (func_ptr)svrObjFStage,
#else
        {F_SRBO_FSTAGE, (func_ptr)NULL,
#endif
        "svrObjFStage", SIZEOF32, 5, SIZEOF32, -1, -1, -1, SIZEOF32, 0,
         0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_FSTAGE, (func_ptr)svrFileFStage,
#else
        {F_E_FSTAGE, (func_ptr)NULL,
#endif
        "svrFileFStage", SIZEOF32, 4, SIZEOF32, -1, -1, SIZEOF32, 0, 0,
         0, 0, 0, 0, 0, 0},
        {F_DUMMY, (func_ptr)NULL, "dummy", 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_LOCK_RANGE, (func_ptr)svrObjLockRange,
#else
        {F_SRBO_LOCK_RANGE, (func_ptr)NULL, 
#endif
	"svrObjLockRange", SIZEOF32, 4, SIZEOF32, 2*SIZEOF32, SIZEOF32,  
	SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_LOCK_RANGE, (func_ptr)svrFileLockRange,
#else
        {F_E_LOCK_RANGE, (func_ptr)NULL, 
#endif
	"svrFileLockRange", SIZEOF32, 4, SIZEOF32, 2*SIZEOF32, SIZEOF32, 
	SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MDRIVER_CREATE, (func_ptr)MDriverCreate,
#else
        {F_MDRIVER_CREATE, (func_ptr)NULL,
#endif
	"MDriverCreate", SIZEOF32, 4, SIZEOF32, -1, -1, SIZEOF32,
	0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MDRIVER_OPEN, (func_ptr)MDriverOpen,
#else
        {F_MDRIVER_OPEN, (func_ptr)NULL,
#endif
	"MDriverOpen", SIZEOF32, 5, SIZEOF32, -1, -1, SIZEOF32, SIZEOF32,
	0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MDRIVER_CLOSE, (func_ptr)MDriverClose, 
#else
        {F_MDRIVER_CLOSE, (func_ptr)NULL,
#endif
	"MDriverClose", SIZEOF32, 1, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MDRIVER_READ, (func_ptr)MDriverRead,
#else
        {F_MDRIVER_READ, (func_ptr)NULL,
#endif
	"MDriverRead", -1, 2, SIZEOF32, SIZEOF32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MDRIVER_WRITE, (func_ptr)MDriverWrite, 
#else
        {F_MDRIVER_WRITE, (func_ptr)NULL, 
#endif
	"MDriverWrite", SIZEOF32, 2, SIZEOF32, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MDRIVER_SEEK, (func_ptr)MDriverSeek, 
#else
        {F_MDRIVER_SEEK, (func_ptr)NULL, 
#endif
	"MDriverSeek", SIZEOF32, 3, SIZEOF32, SIZEOF32, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MDRIVER_UNLINK, (func_ptr)MDriverUnlink, 
#else
        {F_MDRIVER_UNLINK, (func_ptr)NULL, 
#endif
	"MDriverUnlink", SIZEOF32, 3, SIZEOF32, -1, -1, 
	0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MDRIVER_SYNC, (func_ptr)MDriverSync, 
#else
        {F_MDRIVER_SYNC, (func_ptr)NULL, 
#endif
	"MDriverSync", SIZEOF32, 1, SIZEOF32, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_SRBO_PROC, (func_ptr)svrObjProc, 
#else
        {F_SRBO_PROC, (func_ptr)NULL, 
#endif
	"svrObjProc", -1, 4, SIZEOF32, SIZEOF32,-1,-1, 
	0, 0, 0, 0, 0, 0, 0, 0 },
#if defined(SRB_SERVER) && !defined(STK_SERVER)
	{F_MDRIVER_PROC, (func_ptr)MDriverProc,
#else
	{F_MDRIVER_PROC, (func_ptr)NULL,
#endif
        "MDriverProc", -1, 4, SIZEOF32, SIZEOF32,-1,-1,
	0, 0, 0, 0, 0, 0, 0, 0 },
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MODIFY_EXT_META_DATA, (func_ptr)svrModifyExtMetaData,
#else
        {F_MODIFY_EXT_META_DATA, (func_ptr)NULL,
#endif
        "svrModifyExtMetaData", SIZEOF32, 9, SIZEOF32, -1, -1, -1, -1, -1, -1, -1, SIZEOF32,
        0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_E_CHOWN, (func_ptr)svrFileChown,
#else
        {F_E_CHOWN, (func_ptr)NULL,
#endif
	"svrFileChown", SIZEOF32, 6, SIZEOF32, -1, -1, -1, -1, SIZEOF32,
        0, 0, 0, 0, 0, 0 },
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_GET_USER_BY_DN, (func_ptr)svrGetUserByDn,
#else
        {F_GET_USER_BY_DN, (func_ptr)NULL,
#endif
        "svrGetUserByDn", -1, 3, SIZEOF32, SIZEOF32, -1, 0, 0, 0,
        0, 0, 0, 0, 0, 0 },
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MODIFY_FOR_BULK_MOVE, (func_ptr)svrModifyForBulkMove,
#else
        {F_MODIFY_FOR_BULK_MOVE, (func_ptr)NULL,
#endif
	 "svrModifyForBulkMove",SIZEOF32, 4, SIZEOF32, SIZEOF32, -1, -1, 0, 0,
         0, 0, 0, 0, 0, 0 },
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_BULK_MOVE, (func_ptr)svrBulkMove,
#else
        {F_BULK_MOVE, (func_ptr)NULL,
#endif
        "svrBulkMove", SIZEOF32, 6, SIZEOF32, -1, SIZEOF32, -1, -1, -1,
         0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_BULK_UNLOAD_FOR_MOVE, (func_ptr)svrBulkUnloadForMove,
#else
        {F_BULK_UNLOAD_FOR_MOVE, (func_ptr)NULL,
#endif
        "svrBulkUnloadForMove", SIZEOF32, 7, SIZEOF32, SIZEOF32, -1,
         SIZEOF32, -1, -1, -1, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_REG_PHY_MOVE, (func_ptr)svrRegPhyMove,
#else
        {F_REG_PHY_MOVE, (func_ptr)NULL,
#endif
        "svrRegPhyMove", SIZEOF32, 7, SIZEOF32, -1, -1,
         -1, -1, -1, -1, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_BULK_COPY, (func_ptr)svrBulkCopy,
#else
        {F_BULK_COPY, (func_ptr)NULL,
#endif
        "svrBulkCopy", SIZEOF32, 5, SIZEOF32, -1, SIZEOF32, -1, -1, 0,
         0, 0, 0, 0, 0, 0},
#if defined(SRB_SERVER) && !defined(STK_SERVER)
        {F_MOVE_COLL, (func_ptr)svrObjMoveColl,
#else
        {F_MOVE_COLL, (func_ptr)NULL,
#endif
        "svrObjMoveColl", SIZEOF32, 6, SIZEOF32, -1, SIZEOF32, -1, -1, -1,
         0, 0, 0, 0, 0, 0},
        {F_DUMMY, (func_ptr)NULL, "dummy", 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

#ifndef SRB_SERVER
int nBuiltinFunct = (sizeof(clFunctBuiltin) / sizeof(FunctCall)) - 1;
#endif

FunctCall *curFunctCall;

char *_functionRouter(svrComm_t *myComm, func_ptr user_fn, int func_id,
int n_arguments, FunctValues *values);
char *functionRouter(svrComm_t *myComm, FunctCall *myFunctCall, ... );
FunctCall *functionLookup(int funcid);

static int chkVersion = 0;      /* The check version flag */

#endif	/* FUNCTION_HANDLER_H */
