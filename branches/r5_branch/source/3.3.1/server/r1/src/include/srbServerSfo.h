/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*
 * srbServerSfo.h
 */

#ifndef SRB_SERVER_SFO_H
#define SRB_SERVER_SFO_H

#ifdef REFACTOR
#include "srb.h"
#include "elogExtern.h"
#include "distribExtern.h"
#include "packMsgExtern.h"
#include "packDefExtern.h"
#include "packStruct.h"
#include "packMsgExtern.h"
#include "srbServerSfoExtern.h"
#include "clSfoExtern.h"
#ifdef TAPE_DRIVE
#include "srbTapeMiscExtern.h"
#endif
#include "srbObjFunctExtern.h"
#ifdef ADR_PROXY
#include "adrProxyOprExtern.h"
#ifndef TEST_ADR_PROXY
#include "sfoDCIndex.h"
#include "sfoDCFilter.h"
#include "llaccess.h"
#endif
#endif
#endif  /* REFACTOR */
#include "serverAll.h"

#endif 	/* SRB_SERVER_SFO_H */
