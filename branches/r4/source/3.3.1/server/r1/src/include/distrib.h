/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/* distrib.h - Header file for distrib.c.
 */

#ifndef DISTRIB_H
#define DISTRIB_H

#ifdef REFACTOR
#if defined(PORTNAME_osx)
#include <sys/types.h>
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif
#include <stdio.h>
#ifndef _WIN32
#include <sys/file.h>
#endif
#include <sys/stat.h>
#include "c.h"
#include "srb.h"
#include "vault.h"
#include "elogExtern.h"
#include "srbMiscExtern.h"
#include "clConnectExtern.h"
#include "packStruct.h"
#include "clStubExtern.h"
#include "clStubIntern.h"
#include "clAuthExtern.h"
#include "distribExtern.h"
#endif	/* REFACTOR */
#include "serverAll.h"

extern struct hostElement *LocalHostHead;
extern struct hostElement *RemoteHostHead;
extern struct hostElement *LocalMcat;
extern struct vaultQueElement *vaultQueHead;
extern int SvrInitialized;

extern void vCrypt1(char *cpEncryptedMsg, const char *cpClearMsg, const char *cpPassword);
 
#endif	/* DISTRIB_H */
