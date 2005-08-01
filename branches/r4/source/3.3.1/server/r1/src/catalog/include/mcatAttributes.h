/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/***************************************************************************
** Source File Name = mcatAttributes.h
**
**
** PURPOSE: 
**     Contains definitions for MCAT attributes
**
****************************************************************************/
#ifndef MCAT_ATTRIBUTES_H
#define MCAT_ATTRIBUTES_H

/* DCS-ATTRIBUTE-INDEX DEFINES  */
#define  DATA_ID                    0
#define  DATA_REPL_ENUM             1
#define  DATA_NAME                  2
#define  USER_GROUP_NAME            3
#define  DATA_TYP_NAME              4
#define  USER_TYP_NAME              5
#define  USER_ADDRESS               6
#define  USER_NAME                  7
#define  ACCESS_CONSTRAINT          8
#define  DOMAIN_DESC                9
#define  PATH_NAME                 10
#define  RSRC_NAME                 11
#define  RSRC_ADDR_NETPREFIX       12
#define  RSRC_TYP_NAME             13
#define  REPL_TIMESTAMP            14
#define  DATA_GRP_NAME             15
#define  USER_PHONE                16
#define  USER_EMAIL                17
#define  SIZE                      18
#define  USER_AUDIT_TIME_STAMP     19
#define  USER_AUDIT_COMMENTS       20
#define  DATA_COMMENTS             21
#define  AUDIT_ACTION_DESC         22
#define  AUDIT_TIMESTAMP           23
#define  AUDIT_COMMENTS            24
#define  GROUP_ACCESS_CONSTRAINT   25
#define  ACCESS_GROUP_NAME         26
#define  RSRC_DEFAULT_PATH         27
#define  PHY_RSRC_DEFAULT_PATH     28
#define  PHY_RSRC_NAME             29
#define  PHY_RSRC_TYP_NAME         30
#define  RSRC_REPL_ENUM            31
#define  DATA_ACCESS_LIST          32
#define  RSRC_ACCESS_LIST          33
#define  DATA_IS_DELETED           34
#define DATA_OWNER                 35
#define TICKET_D                   36
#define TICKET_BEGIN_TIME_D        37
#define TICKET_END_TIME_D          38
#define TICKET_ACC_COUNT_D         39
#define TICKET_ACC_LIST_D          40
#define TICKET_OWNER_D             41
#define TICKET_USER_D              42
#define TICKET_C                   43
#define TICKET_BEGIN_TIME_C        44
#define TICKET_END_TIME_C          45
#define TICKET_ACC_COUNT_C         46
#define TICKET_ACC_LIST_C          47
#define TICKET_OWNER_C             48
#define TICKET_USER_C              49
#define TICKET_USER_DOMAIN_C       50
#define TICKET_USER_DOMAIN_D       51
#define TICKET_OWNER_DOMAIN_C      52
#define TICKET_OWNER_DOMAIN_D      53
#define PARENT_COLLECTION_NAME     54
#define LOCATION_NAME              55
#define IS_DIRTY                   56
#define CONTAINER_MAX_SIZE         57
#define CONTAINER_NAME             58
#define RSRC_CLASS                 59
#define MAX_OBJ_SIZE               60
#define OFFSET                     61
#define CONTAINER_RSRC_NAME        62
#define CONTAINER_RSRC_CLASS       63  
#define CONTAINER_SIZE             64
#define CONTAINER_LOG_RSRC_NAME    65
#define DATA_OWNER_DOMAIN          66



#define MAX_DCS_NUM 67 /* change this 1 + last DCS-ATTRIBUTE-INDEX */
#define MAX_DCS_STRUCTS 100  /* should be greater than MAX_DCS_NUM */

/* DCS-ATTRIBUTE-INDEX  DEFINES  */


#endif	/* MCAT_ATTRIBUTES_H */
