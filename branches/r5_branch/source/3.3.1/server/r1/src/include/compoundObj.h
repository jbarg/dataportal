/* compoundObj.h */

#ifndef COMPOUND_OBJ_H
#define COMPOUND_OBJ_H

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
#include "elogExtern.h"
#include "srbMiscExtern.h"
#include "srbStat.h"
#include "exfFunctExtern.h"
#ifdef SRB_MDAS
#include "mdasC_db2_externs.h"
#endif
#include "srbC_mdas_externs.h"
#include "mdasPrototypes.h"
#include "srbDirent.h"
#include "distribExtern.h"
#include "clStubExtern.h"
#include "clMiscExtern.h"
#include "exfSwExtern.h"
#include <fcntl.h>      /* most O_ */
#ifndef O_RDONLY
#include <sys/file.h>   /* The rest of O_ */
#endif /* O_RDONLY */

#ifndef _WIN32
#include <unistd.h>     /* for SEEK_ */
#endif
#ifdef TAPE_DRIVE
#include "srbTapeMiscExtern.h"
#endif
#include "srbObjFunctExtern.h"
#include "compoundObjExtern.h"
#include "clStubIntern.h"
#endif	/* REFACTOR */
#include "serverAll.h"

#define NUM_ROWS_FOR_TAPE_QUERY	50

extern userInfo *ClientUser, *ProxyUser, *ThisUser;
extern struct hostElement *LocalHostHead;
extern struct hostElement *RemoteHostHead;
extern resClass resClassEntries[];

#endif  /* COMPOUND_OBJ_H */
