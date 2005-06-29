/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/* mcatAdmin.h - Header file for MCAT admin codes
 */

#ifndef MCAT_ADMIN_H
#define MCAT_ADMIN_H

#include "srb.h"
/* Commented out for now so can compile  #include "srbMisc.h"   */
#include "mcatAdminExtern.h"

#define DEF_PARENT_COLL	"/home"		/* the defuault collection under which
					 * the user's home collection will be
					 * made.
					 */
#define DEF_CONTAINER_COLL  "/container" /* the defuault collection under which
					  * the user's container collection
					  * will be made.
					  */

/*** Added July 9, 2001 to have these for extern on comm and elog ***/
userInfo *ClientUser = NULL;
userInfo *ProxyUser =NULL;
userInfo *ThisUser = NULL;
struct mdasEnvData *ThisMdasEnvData = NULL;
int AuthSchemeInx = -1;

mdasC_sql_result_struct *SrbUserList = NULL;
int TicketUserFlag;
struct hostElement *RemoteHostHead = NULL;
struct hostElement *LocalHostHead = NULL;
int ElogMsgToClient = 0;
int SvrInitialized = 0; /* SvrInitialized = 1 ==> server has been initialized */

/*** Added July 9, 2001 to have these for extern on comm and elog ***/

#endif	/* MCAT_ADMIN_H */
