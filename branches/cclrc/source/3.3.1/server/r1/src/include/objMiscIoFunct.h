/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/* objMcatOprFunct.h
 * Definition and prototype for  srbMcatOprFunct.c.
 */

#ifndef OBJ_MISC_IO_FUNCT_H
#define OBJ_MISC_IO_FUNCT_H
#include "serverAll.h"
#include "serverGlobalExtern.h"
extern int nResClass;
extern resClass resClassEntries[];
extern int nSrbDomain;
extern struct srbDomainMap srbDomainEntry[];
extern int nSrbAccessMode;
extern struct srbAccessModeMap srbAccessModeEntry[];
extern int ncondInput;
extern struct condInputMap condInputEntry[];
extern int nProxyOpr;
extern proxyFunc proxyFuncEntries[];
extern struct resCache *ResCacheHead;
extern struct resCache *CompResCacheHead;
extern struct mdasResInfo *LocalResInfo;

struct srbObjDesc srbObjDesc[MAX_SRBO_DESC];

extern int
sscReceiveSession(char *inStr);

#endif	/* OBJ_MISC_IO_FUNCT_H */

