/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/* objBulkFunct.h
 * Definition and prototype for objBulkFunct.c.
 */

#ifndef OBJ_BULK_FUNCT_H
#define OBJ_BULK_FUNCT_H
#include "serverAll.h"
#include "serverGlobalExtern.h"
#include "hpssParaOprExtern.h"
#include "clExecExtern.h"
extern struct srbObjDesc srbObjDesc[];
extern int nResClass;
extern resClass resClassEntries[];
extern int nSrbDomain;
extern struct srbDomainMap srbDomainEntry[];
extern int nSrbAccessMode;
extern struct srbAccessModeMap srbAccessModeEntry[];
extern int ncondInput;
extern struct condInputMap condInputEntry[];
extern struct resCache *ResCacheHead;
extern struct resCache *CompResCacheHead;
extern struct mdasResInfo *LocalResInfo;
extern int StorVaultMode;
extern int ParaCreateFlag;

#endif	/* OBJ_BULK_FUNCT_H */
