/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/

/***************************************************************************
** Source File Name = mdas_db2_externs.h
**
** PURPOSE: 
**     Contains definitions for library functions 
**     in mdas_db2_library.c
****************************************************************************/
/****************************************************************************
                                GLOBALS
****************************************************************************/
#ifndef MDASC_DB2_EXTERNS_H
#define MDASC_DB2_EXTERNS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


#define MDAS_TICKET_SIZE 10
 
#define TINY_TOKEN 10
#define SMALL_TOKEN 60
#define MEDIUM_TOKEN 100
#define MAX_TOKEN 500


/* smaller stack on OSX */
#if defined(PORTNAME_osx)
#define HUGE_STRING 1000
#else
#define HUGE_STRING 4000
#endif

#define HUGH_HUGE_STRING 10000
#define MAX_OBJ_NAME_LENGTH HUGE_STRING


#define MAX_SERVERS 100
#define MAX_INFO_STRUCTS 100
/* changed MAX_DCS_NUM on 97.9.4 from 25 to 27 */
#define MAXRES 20
#define MAX_SQL_NAME_LIMIT 18
#define HIERARCHY_TOKEN 32
#define ERRMSG_LEN	256
#define ATT_LEN		120
#define MAX_ROW_LIMIT 1000
#define MAX_ROW 200
#define MAX_QUERY_LIMIT 100
#define MAX_ITYPE_LIST_COUNT MAX_TOKEN
#define MAX_CTYPE_LIST_COUNT MAX_TOKEN
#define MAX_DATA_SIZE 2700
#define MAX_DOLLAR_VAL_CNT 100  /* cannot be more than this as it takes two numbers. if changing
				   also change the code in getFullPathName */
#define MAX_TABLE_COLS 100
#define USABLE_TABLE_COLS 15
#define MAX_TABLES_IN_QUERY 50
#define DISTINCT_LIMIT 700
#define DEFAULT_TOKEN_ID_MAXSIZE 250
#define MAX_NO_OF_TABLES 200
#define MAX_NO_OF_MCAT_CONTEXTS 100
#define MAX_FK_REL_PAIRS 5
#define MAX_TEMP_TABLES_ALLOWED 1000
#define MAX_CONV_MTHDS 100
#define MAX_MDATA_SIZE 349
#define MAX_ALLOWED_COLL_LINKS  30 
#define MAX_TABLED_EXISTS_DATA_COUNTER    20
#define MAX_TABLED_SINGLE_DATA_COUNTER    20
#define MAX_TABLED_GETDATASETINFO_COUNTER 20

#define DEFAULT_ATTR_SIZE 250
#define DEFAULT_INTEGER_SPACE_SIZE 40
#define DEFAULT_FLOAT_SPACE_SIZE 42
#define DEFAULT_DATE_SPACE_SIZE 40
#define DEFAULT_ORACLE_BLOB_SPACE_SIZE 2000000000
#define DEFAULT_ORACLE_CLOB_SPACE_SIZE 2000000000
 

#define MAX_ACCS_NUM   999   /* use for finding the maximum access possible for any fc object */

#define TICKET_USER "ticketuser"
#define TICKET_USER_DOMAIN    "sdsc"

#define METADATA_FKREL_FILE     "metadata.fkrel"

#ifdef PSQMCAT
#define MCAT_CURR_TIMESTAMP "NOW()"
#endif

#ifdef DB2MCAT
#define MCAT_CURR_TIMESTAMP "CURRENT TIMESTAMP"
#endif

#ifdef ORAMCAT
#define MCAT_CURR_TIMESTAMP "TO_CHAR(SYSDATE,'IYYY-MM-DD-HH24.MI.SS')"
#endif

#ifdef SYBMCAT
#define MCAT_CURR_TIMESTAMP "getdate()"
#endif

#ifdef IFMXMCAT
#define MCAT_CURR_TIMESTAMP "CURRENT TIMESTAMP"
#endif

extern char restab[][MAX_TOKEN];
extern int   cur_pds_num;
extern char  GENERATEDSQL[HUGH_HUGE_STRING];



#define MDAS_CAT_COMMIT        0
#define MDAS_CAT_ROLLBACK      1
 
 
#define MDAS_CATALOG 0
#define LDAP_CATALOG 1
#define FILE_CATALOG 2


#define ERRMESSFILE LOGMESSFILE
#define TRANSCOMMIT 0


/* DCS-ATTRIBUTE-INDEX DEFINES  */
#define  DATA_ID                    0   /* internal data id */
#define  DATA_REPL_ENUM             1   /* replica copy number */
#define  DATA_NAME                  2   /* data name */
#define  USER_GROUP_NAME            3   /* name of user group */
#define  DATA_TYP_NAME              4   /* data type */
#define  USER_TYP_NAME              5   /* user type */
#define  USER_ADDRESS               6   /* user address */
#define  USER_NAME                  7   /* user name */
#define  ACCESS_CONSTRAINT          8   /* access restriction on  data */
#define  DOMAIN_DESC                9   /* user domain name */
#define  PATH_NAME                 10   /* physical path name of data object */
#define  RSRC_NAME                 11   /* name of logical resource */
#define  RSRC_ADDR_NETPREFIX       12   /* net address of resource */
#define  RSRC_TYP_NAME             13   /* logical resource type */
#define  REPL_TIMESTAMP            14   /* data modification time stamp */
#define  DATA_GRP_NAME             15   /* collection name */
#define  COLLECTION_NAME           15   /* collection name */
#define  USER_PHONE                16   /* user phone number */
#define  USER_EMAIL                17   /* user email */
#define  SIZE                      18   /* size of data */
#define  USER_AUDIT_TIME_STAMP     19   /* audit on user time stamp */
#define  USER_AUDIT_COMMENTS       20   /* audit on user comments */
#define  DATA_COMMENTS             21   /* comments on data */
#define  AUDIT_ACTION_DESC         22   /* audited action  on data */
#define  AUDIT_TIMESTAMP           23   /* audit time stamp for data */
#define  AUDIT_COMMENTS            24   /* audit comments  for data */
#define  GROUP_ACCESS_CONSTRAINT   25   /* access on collection (26)*/
#define  COLLECTION_ACCESS_CONSTRAINT   25   /* access on collection (26)*/
#define  ACCESS_GROUP_NAME         26   /* collection name for access in (25) */
#define  ACCESS_COLLECTION_NAME         26   /* collection name for access in (25) */
#define  RSRC_DEFAULT_PATH         27   /* default path in logical resource */
#define  PHY_RSRC_DEFAULT_PATH     28   /* default path in physical resource */
#define  PHY_RSRC_NAME             29   /* physical rsource name */
#define  PHY_RSRC_TYP_NAME         30   /* physical resource type */
#define  RSRC_REPL_ENUM            31   /* index of physical rsrc in logical rsrc*/
#define  DATA_ACCESS_LIST          32   /* access list for data */
#define  RSRC_ACCESS_LIST          33   /* access list for resource */
#define  DATA_IS_DELETED           34   /* data liveness */
#define DATA_OWNER                 35   /* data creator name */
#define TICKET_D                   36   /* identifier for ticket given for data*/
#define TICKET_BEGIN_TIME_D        37   /* data ticket validity start time */
#define TICKET_END_TIME_D          38   /* data ticket validity end  time */
#define TICKET_ACC_COUNT_D         39   /* valid number of opens allowed on data ticket */
#define TICKET_ACC_LIST_D          40   /* valid access allowed on data ticket (currently readonly) */
#define TICKET_OWNER_D             41   /* data ticket creator */
#define TICKET_USER_D              42   /* allowed ticket user or user group */
#define TICKET_C                   43   /* identifier for ticket given for collection and sub collections*/
#define TICKET_BEGIN_TIME_C        44   /* collection ticket validity start time*/
#define TICKET_END_TIME_C          45   /* collection ticket validity end time*/
#define TICKET_ACC_COUNT_C         46   /* valid number of opens allowed on data in collection */
#define TICKET_ACC_LIST_C          47   /* valid access allowed on data in collection  (currently readonly) */
#define TICKET_OWNER_C             48   /* collection ticket creator */
#define TICKET_USER_C              49   /* allowed collection ticket user */
#define TICKET_USER_DOMAIN_C       50   /* collection ticket user domain */
#define TICKET_USER_DOMAIN_D       51   /* data ticket user domain */
#define TICKET_OWNER_DOMAIN_C      52   /* collection ticket creator domain */
#define TICKET_OWNER_DOMAIN_D      53   /* data ticket creator domain */
#define PARENT_COLLECTION_NAME     54   /* name of parent collection (15) */
#define LOCATION_NAME              55   /* location (of resource) name */
#define IS_DIRTY                   56   /* data has been changed compared to other copies (currently used  only for containered data) */
#define CONTAINER_MAX_SIZE         57   /* maximum size of container */
#define CONTAINER_NAME             58   /* name of container */
#define RSRC_CLASS                 59   /* classification of resource */
#define MAX_OBJ_SIZE               60   /* maximum size of data object allowed in resource (not enforced by MCAT) */
#define OFFSET                     61   /* position of data in container */
#define CONTAINER_RSRC_NAME        62   /* name of physical resource of container */
#define CONTAINER_RSRC_CLASS       63   /* class of physical resource of container */
#define CONTAINER_SIZE             64   /* current size of container */
#define CONTAINER_LOG_RSRC_NAME    65   /* logical resource associated with a  container */
#define DATA_OWNER_DOMAIN          66   /* domain of data creator */
#define USER_DISTIN_NAME           67   /* distinguished name of user (used by authentication systems ) */
#define USER_AUTH_SCHEME           68   /* user authentication scheme (67) */
#define SERVER_LOCATION            69   /* location of SRB server */
#define SERVER_NETPREFIX           70   /* net address of SRB server */
#define CONTAINER_REPL_ENUM        71   /* container copy number */
#define DATA_ANNOTATION_USERNAME   72   /* name of annotator */
#define DATA_ANNOTATION_USERDOMAIN 73   /* domain of annotator */
#define DATA_ANNOTATION            74   /* annotation on data */
#define DATA_ANNOTATION_TIMESTAMP  75   /* time of annotation */
#define DATA_CREATE_TIMESTAMP      76   /* data creation time stamp */
#define DATA_COMMENTS_TIMESTAMP    77   /* time stamp for comments on data */
#define DATA_ANNOTATION_POSITION   78   /* some user-defined location for the annotation */
#define DATA_ACCESS_PRIVILEGE      79   /* access privilege on data */
#define RESOURCE_MAX_LATENCY       80   /* physical resource estimated latency (max) */
#define RESOURCE_MIN_LATENCY       81   /* physical resource estimated latency (min) */
#define RESOURCE_BANDWIDTH         82   /* physical resource estimated bandwidth */
#define RESOURCE_MAX_CONCURRENCY   83   /* physical resource maximum concurrent requests */
#define RESOURCE_NUM_OF_HIERARCHIES  84  /* number of hierarchies in the physical resource */
#define RESOURCE_NUM_OF_STRIPES    85   /* number of striping of data in the physical resource */
#define RESOURCE_CAPACITY          86   /* capacity of  the physical resource */
#define RESOURCE_DESCRIPTION       87   /* comments on the resource */
#define DATA_CLASS_NAME            88   /* classifcation name for data different from (4)*/
#define DATA_CLASS_TYPE            89   /* classification type */
#define STRUCTURED_METADATA_TYPE   90   /* type of user-inserted structured metadata for data in (92) or (93) */
#define STRUCTURED_METADATA_COMMENTS   91 /* comments on the structured metadata in (92) or (93) */ 
#define STRUCTURED_METADATA_DATA_NAME  92 /* data name of user-inserted structured metadata stored as another data object inside SRB see also (112)*/
#define INTERNAL_STRUCTURED_METADATA   93 /* user-inserted structured metadata stored as a string inside MCAT */ 
#define INDEX_NAME_FOR_DATASET         94 /* data name of index on data */
#define INDEX_NAME_FOR_DATATYPE        95 /* data name of index on data type */
#define INDEX_NAME_FOR_COLLECTION      96 /* data name of index on collection */
#define METHOD_NAME_FOR_DATASET        97 /* data name of method on data */
#define METHOD_NAME_FOR_DATATYPE       98 /* data name of method on data type */
#define METHOD_NAME_FOR_COLLECTION     99 /* data name of method on collection */
#define IX_COLL_NAME_FOR_DATASET       100 /* collection name of index on data (4) */
#define IX_COLLNAME_FOR_DATATYPE       101 /* collection name of index on data type (95) */
#define IX_COLLNAME_FOR_COLLECTION     102 /* collection name of index on collection  (96) */
#define MTH_COLLNAME_FOR_DATASET       103 /* collection name of method on data  (97) */
#define MTH_COLLNAME_FOR_DATATYPE      104 /* collection name of method on data type (98) */
#define MTH_COLLNAME_FOR_COLLECTION    105 /* collection name of method on collection (99) */
#define IX_DATATYPE_FOR_DATASET        106 /* index type for (94) */
#define IX_DATATYPE_FOR_DATATYPE       107 /* index type for (95) */
#define IX_DATATYPE_FOR_COLLECTION     108 /* index type for (96) */
#define MTH_DATATYPE_FOR_DATASET       109 /* method type for (97) */
#define MTH_DATATYPE_FOR_DATATYPE      110 /* method type for (98) */
#define MTH_DATATYPE_FOR_COLLECTION    111 /* method type for (99) */
#define STRUCTURED_METADATA_COLLNAME   112 /* collection name of user-inserted structured metadata stored as another data object inside SRB see also (92)*/
#define IX_LOCATION_FOR_DATASET        113 /* path name of index in (94)*/
#define IX_LOCATION_FOR_DATATYPE       114 /* path name of index in (95)*/
#define IX_LOCATION_FOR_COLLECTION     115 /* path name of index in (96)*/
#define METADATA_NUM                   116 /* index of metadata for data */
#define UDSMD0                         117 /* user-defined string metadata  for data **/
#define UDSMD0_0                       117 /* attr-name metadata  for data **/
#define UDSMD1_0                       118 /* attr-value metadata  for data **/
#define UDSMD1                         118 /* user-defined string metadata  for data **/
#define UDSMD2                         119 /* user-defined string metadata  for data **/
#define UDSMD3                         120 /* user-defined string metadata  for data **/
#define UDSMD4                         121 /* user-defined string metadata  for data **/
#define UDSMD5                         122 /* user-defined string metadata  for data **/
#define UDSMD6                         123 /* user-defined string metadata  for data **/
#define UDSMD7                         124 /* user-defined string metadata  for data **/
#define UDSMD8                         125 /* user-defined string metadata  for data **/
#define UDSMD9                         126 /*change MDAS_ADREPL_STRING_METADATA*/
#define UDIMD0                         127 /* user-defined integer metadata   for data*/
#define UDIMD1                         128 /*change MAX_ADREPL_INTEGER_METADATA */
#define DATA_OWNER_EMAIL               129 /* email of data creator */
#define AUDIT_USER                     130 /* name of user who is audited */
#define AUDIT_USER_DOMAIN              131 /* domain of user who is audited */
#define CONTAINER_FOR_COLLECTION       132 /* default container  for collection */
#define METADATA_NUM_COLL              133 /* user-defined string metadata  for collection*/
#define UDSMD_COLL0                    134 /* user-defined string metadata  for collection*/
#define UDSMD_COLL1                    135 /* user-defined string metadata  for collection */
#define UDSMD_COLL0_0                  134 /* attr-name metadata  for collection*/
#define UDSMD_COLL1_0                  135 /* attr-value metadata  for collection*/
#define UDSMD_COLL2                    136 /* user-defined string metadata  for collection */
#define UDSMD_COLL3                    137 /* user-defined string metadata  for collection */
#define UDSMD_COLL4                    138 /* user-defined string metadata  for collection */
#define UDSMD_COLL5                    139 /* user-defined string metadata  for collection */
#define UDSMD_COLL6                    140 /* user-defined string metadata  for collection */
#define UDSMD_COLL7                    141 /* user-defined string metadata  for collection */
#define UDSMD_COLL8                    142 /* user-defined string metadata  for collection */
#define UDSMD_COLL9                    143 /*change MDAS_COLL_STRING_METADATA*/
#define UDIMD_COLL0                    144 /* user-defined integer metadata  for collection*/
#define UDIMD_COLL1                    145 /*change MAX_COLL_INTEGER_METADATA */
#define DATA_ACCESS_ID                 146 /* MCAT's internal Id for Access Type */
#define USER_ID                        147 /* MCAT's internal Id for the User */
#define DATA_LAST_ACCESS_TIMESTAMP     148 /* data last access time stamp */
#define COLL_OWNER_NAME                149 /* owner of collection */
#define COLL_CREATE_TIMESTAMP          150 /* collection creation timestamp */
#define COLL_COMMENTS                  151 /* comments for a colllection */
#define DC_DATA_NAME                   152 /* Dublin Core Data Name same as (2) */
#define DC_COLLECTION                  153 /* DC: Collection NAme same as  (15) */
#define DC_CONTRIBUTOR_TYPE            154 /* DC: Contributor Type: Eg. Author, Illustrator */
#define DC_SUBJECT_CLASS               155 /* DC: Subject Classification */
#define DC_DESCRIPTION_TYPE            156 /* DC: Type of Description */
#define DC_TYPE                        157 /* DC: Type of the Object */
#define DC_SOURCE_TYPE                 158 /* DC: Type of the Source */
#define DC_LANGUAGE                    159 /* DC: Language of the Object */
#define DC_RELATION_TYPE               160 /* DC: Relation with another Object in (170,171) */
#define DC_COVERAGE_TYPE               161 /* DC: Coverage Type */
#define DC_RIGHTS_TYPE                 162 /* DC: Rights Type */
#define DC_TITLE                       163 /* DC: Title of the Object */
#define DC_CONTRIBUTOR_NAME            164 /* DC: Contributor Name. NOT same as (7) */
#define DC_SUBJECT_NAME                165 /* DC: Subject */
#define DC_DESCRIPTION                 166 /* DC: Description */
#define DC_PUBLISHER                   167 /* DC: Publisher Name */
#define DC_SOURCE                      168 /* DC: Source Name */
#define DC_RELATED_DATA_DESCR          169 /* DC: Related Data Description */
#define DC_RELATED_DATA                170 /* DC: Date Related to (152,153) */
#define DC_RELATED_COLL                171 /* DC:  */
#define DC_COVERAGE                    172 /* DC: Coverage Information */
#define DC_RIGHTS                      173 /* DC: Rights Information */
#define DC_PUBLISHER_ADDR              174 /* DC: Publisher Address */
#define DC_CONTRIBUTOR_ADDR            175 /* DC: Contributro Address */
#define DC_CONTRIBUTOR_EMAIL           176 /* DC: Contributor Email */
#define DC_CONTRIBUTOR_PHONE           177 /* DC: Contributor Phone */
#define DC_CONTRIBUTOR_WEB             178 /* DC: Contributor Web Address */
#define DC_CONTRIBUTOR_CORPNAME        179 /* DC: Contributor Affiliation */
#define COLL_ACCS_ID                   180 /* access id  for collections */
#define COLL_ACCS_USER_ID              181 /* collection access user */
#define DATA_ACCS_ID                   182 /* access id  for data */
#define DATA_ACCS_USER_ID              183 /* data  access user */
#define UDSMD0_1                       184 /* attr-name of metadata for data  */
#define UDSMD0_2                       185 /* attr-name of metadata  for data */
#define UDSMD0_3                       186 /* attr-name of metadata  for data */
#define UDSMD0_4                       187 /* attr-name of metadata  for data */
#define UDSMD1_1                       188 /* attr-value of metadata  for data */
#define UDSMD1_2                       189 /* attr-value of metadata  for data */
#define UDSMD1_3                       190 /* attr-value of metadata  for data */
#define UDSMD1_4                       191 /* attr-value of metadata  for data */
#define UDSMD_COLL0_1                  192 /* attr-name of metadata for coll */
#define UDSMD_COLL0_2                  193 /* attr-name of metadata for coll */
#define UDSMD_COLL0_3                  194 /* attr-name of metadata for coll */
#define UDSMD_COLL0_4                  195 /* attr-name of metadata for coll */
#define UDSMD_COLL1_1                  196 /* attr-value of metadata for coll */
#define UDSMD_COLL1_2                  197 /* attr-value of metadata for coll */
#define UDSMD_COLL1_3                  198 /* attr-value of metadata for coll */
#define UDSMD_COLL1_4                  199 /* attr-value of metadata for coll */
#define C_TICKET_RECURSIVE             200 /* recursive flag for C_TICKET*/
#define UDSMD_RSRC0_1                  201 /* attr-name of metadata for rsrc*/
#define UDSMD_RSRC0_2                  202 /* attr-name of metadata for rsrc*/
#define UDSMD_RSRC0_3                  203 /* attr-name of metadata for rsrc*/
#define UDSMD_RSRC1_1                  204 /* attr-value of metadata for rsrc*/
#define UDSMD_RSRC1_2                  205 /* attr-value of metadata for rsrc*/
#define UDSMD_RSRC1_3                  206 /* attr-value of metadata for rsrc*/
#define UDSMD_USER0_1                  207 /* attr-name of metadata for user */
#define UDSMD_USER0_2                  208 /* attr-name of metadata for user */
#define UDSMD_USER0_3                  209 /* attr-name of metadata for user */
#define UDSMD_USER1_1                  210 /* attr-value of metadata for user */
#define UDSMD_USER1_2                  211 /* attr-value of metadata for user */
#define UDSMD_USER1_3                  212 /* attr-value of metadata for user */
#define TAPE_NUMBER                    213 /* tape_number in compound rsrc */
#define TAPE_OWNER                     214 /* tape_owner in compound rsrc */
#define TAPE_TYPE_VAL                  215 /* tape_type in compound rsrc */
#define TAPE_LIBINX                    216 /* tape_index in compound rsrc */
#define TAPE_FULLFLAG                  217 /* tape_full in compound rsrc */
#define TAPE_CURRENT_FILE_SEQNUM       218 /* tape_seq number in compound rsrc */
#define TAPE_CURRENT_ABS_POSITION      219 /* tape_abs position in compound rsrc */
#define TAPE_BYTES_WRITTEN             220 /* tape_space used  in compound rsrc */
#define DATA_VER_NUM                   221 /* dataset version number */
#define DATA_LOCK_NUM                  222 /* dataset lock */
#define DATA_LOCK_DESC                 223 /* data lock type */
#define DATA_LOCK_OWNER_NAME           224 /* dataset lock owner name*/
#define DATA_LOCK_OWNER_DOMAIN         225 /* dataset lock owner domain */
#define COLL_OWNER_DOMAIN              226 /* collection owner domain */ 
#define C_TICKET_COLL_NAME             227 /* collection for ticket */
#define C_TICKET_COLL_CREATE_TIMESTAMP 228 /* ticket collction timestamp */
#define C_TICKET_COLL_COMMENTS         229  /* ticket collction comments */
#define COLL_ANNOTATION_USERNAME       230   /* name of annotator */
#define COLL_ANNOTATION_USERDOMAIN     231   /* domain of annotator */
#define COLL_ANNOTATION                232   /* annotation on data */
#define COLL_ANNOTATION_TIMESTAMP      233   /* time of annotation */
#define COLL_ANNOTATION_TYPE           234   /* some user-defined type for the annotation */
#define DATA_GRP_ID                    235  /* internal collection id */
#define DATA_HIDE                      236  /* setting it more than 0 hides the data */
#define DATA_CHECKSUM                  237  /* check sum for data 400 character string */
#define DATA_AUDITFLAG                 238  /* flag to see whether the data needs to be audited */
#define DATA_LOCK_EXPIRY               239  /* lock expory dtimestamp */
#define DATA_PIN_VAL                   240  /* data replica pinned from being moved if set greaterthan 0 */
#define DATA_PIN_OWNER_NAME            241  /* owner of the pin */
#define DATA_PIN_OWNER_DOMAIN          242  /* domain of the pin owner */
#define DATA_PIN_EXPIRY                243  /* expiry timestamp for the pin */
#define DATA_EXPIRY_DATE               244  /* expiry timestamp for the data replica itself  */
#define DATA_IS_COMPRESSED             245  /* if data is compressed tell here how? */
#define DATA_IS_ENCRYPTED              246  /* if data is encrypted tell here how? not the password */
#define DATA_EXPIRE_DATE_2             247  /* another expiry time_stamp for data as per george req */
#define NEW_AUDIT_ACTION_DESC          248  /* audit action description */
#define NEW_AUDIT_TIMESTAMP            249  /* audit action timestamp */
#define NEW_AUDIT_COMMENTS             250  /* audit action comments */
#define UDSMD_RSRC0_0                  251  /* attr_name  metadata for rsrc*/
#define UDSMD_RSRC1_0                  252  /* attr_value metadata for rsrc*/
#define UDSMD_RSRC0                    251  /* user-defined string metadata  for rsrc */
#define UDSMD_RSRC1                    252  /* user-defined string metadata  for rsrc  */
#define UDSMD_RSRC2                    253 /* user-defined string metadata  for rsrc  */
#define UDSMD_RSRC3                    254 /* user-defined string metadata  for rsrc  */
#define UDSMD_RSRC4                    255 /* user-defined string metadata  for rsrc  */
#define UDSMD_RSRC5                    256 /* user-defined string metadata  for rsrc  */
#define UDSMD_RSRC6                    257 /* user-defined string metadata  for rsrc  */
#define UDSMD_RSRC7                    258 /* user-defined string metadata  for rsrc  */
#define UDSMD_RSRC8                    259 /* user-defined string metadata  for rsrc  */
#define UDSMD_RSRC9                    260 /*change MDAS_RSRC_STRING_METADATA*/
#define UDIMD_RSRC0                    261 /* user-defined integer metadata  for rsrc  */
#define METADATA_NUM_RSRC              262 /* metadata num of user-defined  metadata  for rsrc */
#define METADATA_NUM_USER              263 /* metadata num of user-defined  metadata  for user */
#define COMPOUND_RSRC_NAME             264 /* name of compund resource */
#define DATA_SEG_NUM                   265 /* segment number of data *if data is segmented into files */
#define INT_REPL_ENUM                  266 /* cmpd rsrc internal replica number of object */
#define INT_SEG_NUM                    267 /* cmpd rsrc internal segment number of object */
#define INT_PATH_NAME                  268 /* cmpd rsrc internal path name of object */
#define INT_RSRC_NAME                  269 /* cmpd rsrc internal resource name  of object */
#define INT_RSRC_ADDR_NETPREFIX        270 /* cmpd rsrc internal net address of rsrc  of object */
#define INT_RSRC_TYP_NAME              271 /* cmpd rsrc internal type of rsrc  of object */
#define INT_IS_DIRTY                   272 /* cmpd rsrc internal replia/segment object is dirty */
#define INT_RSRC_CLASS                 273 /* cmpd rsrc internal class of rsrc  of object */
#define INT_OFFSET                     274 /* cmpd rsrc internal offset   of object on tape */
#define INT_SIZE                       275 /* cmpd rsrc internal size   of object */
#define COMP_OBJ_USER_NAME             276 /* cmpd object user name */
#define COMP_OBJ_USER_DOMAIN           277 /* cmpd object user domain */
#define INT_PATH_NAME_FOR_REPL         278 /* cmpd rsrc internal path name of object */
#define ACL_INHERITANCE_BIT            279 
#define REAL_COLLECTION_NAME           280 /*** fake ****/
#define DATA_CONT_NAME                 281 /*data name of container  */
#define COLLECTION_CONT_NAME           282 /* collection name of container  */
#define UDSMD_USER0_0                  283 /* attr_name of metadata for user*/
#define UDSMD_USER1_0                  284 /* attr_value of  metadata for user*/
#define UDSMD_USER0                    283 /* user-defined string metadata  for user */
#define UDSMD_USER1                    284 /* user-defined string metadata  for user */
#define UDSMD_USER2                    285 /* user-defined string metadata  for user */
#define UDSMD_USER3                    286 /* user-defined string metadata  for user */
#define UDSMD_USER4                    287 /* user-defined string metadata  for user */
#define UDSMD_USER5                    288 /* user-defined string metadata  for user */
#define UDSMD_USER6                    289 /* user-defined string metadata  for user */
#define UDSMD_USER7                    290 /* user-defined string metadata  for user */
#define UDSMD_USER8                    291 /* user-defined string metadata  for user */
#define UDSMD_USER9                    292 /*change MDAS_USER_STRING_METADATA*/
#define UDIMD_USER0                    293 /* user-defined integer metadata  */
#define NONDISTINCT                    294 /* SET IF YOU WANT TO DIABLE DISTINCT OPTION IN SQL QUERY*/
                                           /*   eg: selval[NONDISTINCT] = 1; */
#define USERDEFFUNC01                  295 /* UDF FUNCTION */
                                           /* used to put additional conditions in queries */
                              /*   eg: sprintf(qval[USERDEFFUNC01]," length(SRBATTR[DATA_NAME]) < 5 ");*/
#define USERDEFFUNC02                  296 /* UDF FUNCTION */
#define USERDEFFUNC03                  297 /* UDF FUNCTION */
#define USERDEFFUNC04                  298 /* UDF FUNCTION */
#define USERDEFFUNC05                  299 /* UDF FUNCTION */
#define ZONE_NAME                      300 /* Name of MCAT Zone (char*)*/
#define ZONE_LOCALITY                  301 /* set to 1 for local zone  0 o/w*/
#define ZONE_NETPREFIX                 302 /* address where (remote) MCAT is */
#define ZONE_PORT_NUM                  303 /* port num to reach (remote) MCAT*/
#define ZONE_ADMIN_AUTH_SCHEME_NAME    304 /* admin's auth scheme of rem MCAT*/
#define ZONE_ADMIN_DISTIN_NAME         305 /* DN str  of rem MCAT */
#define ZONE_STATUS                    306 /* 1 for a valid zone */
#define ZONE_CREATE_DATE               307 /* when the zone was created */
#define ZONE_MODIFY_DATE               308 /* when the zone was modified */
#define ZONE_COMMENTS                  309 /* any comments about the zone */
#define ZONE_CONTACT                   310 /* who is in charge of the zone */
#define ZONE_ADMIN_NAME                311 /* zone admin's username */
#define ZONE_ADMIN_DOMAIN_NAME         312 /* zone admin's user domain name */
#define RSRC_ACCESS_PRIVILEGE          313 /* resource access privilege */
#define RSRC_ACCESS_CONSTRAINT         314 /* resource access constraint namw*/
#define UDIMD_USER1                    315 /*change MAX_USER_INTEGER_METADATA*/
#define UDIMD_RSRC1                    316 /*change MAX_RSRC_INTEGER_METADATA*/
#define USER_GROUP_MODIFY_DATE         317 /* group user info modify date */
#define USER_GROUP_ZONE_NAME           318 /* group user zone name */
#define USER_GROUP_DOMAIN_DESC         319 /* group user domain name */
#define DATA_UDEF_MDATA_MODIFY_DATE    320 /* user-def metadata for data mod */
#define COLL_UDEF_MDATA_MODIFY_DATE    321 /* user-def metadata for coll mod */
#define USER_UDEF_MDATA_MODIFY_DATE    322 /* user-def metadata for user mod */
#define RSRC_UDEF_MDATA_MODIFY_DATE    323 /* user-def metadata for rsrc mod */
#define DATA_TYPE_MIME_STRING          324 /* mime string for data type */
#define DATA_TYPE_EXTENDERS            325 /* extender list for data type */
#define RSRC_COMMENTS                  326 /* comments on log resource */
#define RSRC_CREATE_DATE               327 /* log rsource creation timestamp */
#define RSRC_MODIFY_DATE               328 /* log resource modify timestamp */
#define USER_CREATE_DATE               329 /* user creation timestamp */
#define USER_MODIFY_DATE               330 /* user modify timestamp */
#define RSRC_MAX_OBJ_SIZE              331 /* log resource max obj size */
#define RSRC_OWNER_NAME                332 /* owner of the log resource */
#define RSRC_OWNER_DOMAIN              333 /* domain of the log resource */
#define RSRC_MLSEC_LATENCY_MAX         334 /* max latency of rsrc in millisec*/
#define RSRC_MLSEC_LATENCY_MIN         335 /* min latency of rsrc in millisec*/
#define RSRC_MBPS_BANDWIDTH            336 /* bandwidth of rsrc in mega bps */
#define RSRC_CONCURRENCY_MAX           337 /* max concurrency allowed in rsrc*/
#define RSRC_NUM_OF_HIERARCHIES        338 /* num of staging levels in rsrc */
#define RSRC_NUM_OF_STRIPES            339 /* num of striping done in rsrc */
#define RSRC_MEGAB_CAPACITY            340 /* capacity of rsrc in megabytes */
#define RSRC_ACCS_USER_NAME            341 /* user name used for rsrc access */
#define RSRC_ACCS_USER_DOMAIN          342 /* user domn used for rsrc access */
#define RSRC_ACCS_USER_ZONE            343 /* user zone used for rsrc access */
#define RSRC_OWNER_ZONE                344 /* zone for resource owner */
#define DATA_OWNER_ZONE                345 /* zone for data owner */
#define TICKET_OWNER_ZONE_D            346 /* zone for data ticket owner */
#define TICKET_USER_ZONE_D             347 /* zone for data ticket user */
#define TICKET_OWNER_ZONE_C            348 /* zone for coll ticket owner */
#define TICKET_USER_ZONE_C             349 /* zone for coll ticket user */
#define DATA_ANNOTATION_USER_ZONE      350 /* zone for data annotator */
#define AUDIT_USER_ZONE                351 /* zone for audited suer */
#define COLL_OWNER_ZONE                352 /* zone for collection owner */
#define DATA_LOCK_OWNER_ZONE           353 /* zone for data lock owner */
#define COLL_ANNOTATION_USER_ZONE      354 /* zone for coll annotator */
#define DATA_PIN_OWNER_ZONE            355 /* zone for data pin owner  */
#define COMP_OBJ_USER_ZONE             356 /* zone for composite obj owner */
#define PARENT_SERVER_LOCATION         357 /* parent of a location */
#define RSRC_ACCESS_ID                 358 /* access id number for rsrc access */
#define PARENT_DATA_TYPE               359 /* parent of a data type */
#define ZONE_LOCN_DESC                 360 /* location desc for zone */
#define PARENT_DOMAIN_DESC             361 /* parent of a domain */
#define PARENT_USER_TYPE               362 /* parent of a user type */
#define PARENT_RSRC_TYPE               363 /* parent of a rsrc type  */
#define RSRC_ACCS_GRPUSER_NAME         364 /* group user who has access to rsrc*/
#define RSRC_ACCS_GRPUSER_DOMAIN       365 /* rsrc group user domain */
#define RSRC_ACCS_GRPUSER_ZONE         366 /* rsrc group user zone */
#define COLL_MODIFY_TIMESTAMP          367 /* collection modify_timestamp */
#define DATA_ACCS_GRPUSER_NAME         368 /* group user who has access to data*/
#define DATA_ACCS_GRPUSER_DOMAIN       369 /* data group user domain */
#define DATA_ACCS_GRPUSER_ZONE         370 /* data group user zone */
#define COLL_ACCS_GRPUSER_NAME         371 /* group user who has access to coll*/
#define COLL_ACCS_GRPUSER_DOMAIN       372 /* coll group user domain */
#define COLL_ACCS_GRPUSER_ZONE         373 /* coll group user zone */
#define COLL_ACCS_COLLECTION_NAME      374 /* coll name for finding coll access*/
#define COLL_ACCS_ACCESS_CONSTRAINT    375 /* accs cons for finding coll access*/
#define DATA_TYPE_FOR_CONTAINER_FOR_COLLECTION  376 /* data type for container associated with collection */
#define DATA_UDEF_MDATA_CREATE_DATE    377 /* user-defined metadata for data creation date */
#define COLL_UDEF_MDATA_CREATE_DATE    378 /* user-defined metadata for collection creation date  */
#define RSRC_UDEF_MDATA_CREATE_DATE    379 /* user-defined metadata for resource creation date  */
#define USER_UDEF_MDATA_CREATE_DATE    380 /* user-defined metadata for user creation date  */
#define CONTAINER_ID                   381 /* data_id of a container */
#define GUID                           382 /* global unique id  */
#define GUID_FLAG                      383 /* flag for global unique id  */
#define GUID_TIME_STAMP                384 /* global unique id creation timestamp */
#define ORDERBY                        385 /* ORDERBY clause in SQL */
                                           /* used to sort  the results  */
                                    /* eg. sprintf(qval[ORDERBY],"DATA_TYP_NAME,  DATA_NAME"); */
#define DEL_DATA_ID                    386 /* data_id of deleted data object */
#define DEL_DATA_REPL_ENUM             387 /* repl_enum of deleted data object */
#define DEL_DATA_NAME                  388 /* data_name of deleted data object */
#define DEL_REPL_TIMESTAMP             389 /* timestamp of deleted data object */
#define DEL_COLLECTION_NAME            390 /* collection_name of deleted data object */
#define DEL_COLLECTION_ID              391 /* collection_id of deleted data object */
#define DEL_DATA_COMMENT               392 /* comments on deleted data object */
#define FREE_SPACE                     393 /* USER EXTRA */
#define FS_TIMESTAMP                   394 /* USER EXTRA */
#define EXTENSIBLE_SCHEMA_NAME         395 /* USER EXTRA */
#define EXTENSIBLE_TABLE_NAME          396 /* USER EXTRA */
#define EXTENSIBLE_ATTR_NAME           397 /* USER EXTRA */
#define EXTENSIBLE_ATTR_OUTSIDE_NAME   398 /* USER EXTRA */
#define EXTENSIBLE_ATTR_COMMENTS       399 /* USER EXTRA */
#define USEREXTRA400                   400 /* USER EXTRA */
#define USEREXTRA401                   401 /* USER EXTRA */
#define USEREXTRA402                   402 /* USER EXTRA */
#define USEREXTRA403                   403 /* USER EXTRA */
#define USEREXTRA404                   404 /* USER EXTRA */
#define USEREXTRA405                   405 /* USER EXTRA */
#define USEREXTRA406                   406 /* USER EXTRA */
#define USEREXTRA407                   407 /* USER EXTRA */
#define USEREXTRA408                   408 /* USER EXTRA */
#define USEREXTRA409                   409 /* USER EXTRA */
#define USEREXTRA410                   410 /* USER EXTRA */
#define USEREXTRA411                   411 /* USER EXTRA */
#define USEREXTRA412                   412 /* USER EXTRA */
#define USEREXTRA413                   413 /* USER EXTRA */
#define USEREXTRA414                   414 /* USER EXTRA */
#define USEREXTRA415                   415 /* USER EXTRA */
#define USEREXTRA416                   416 /* USER EXTRA */
#define USEREXTRA417                   417 /* USER EXTRA */
#define USEREXTRA418                   418 /* USER EXTRA */
#define USEREXTRA419                   419 /* USER EXTRA */
#define USEREXTRA420                   420 /* USER EXTRA */
#define MIN_EXT_TABLE_INDEX        420    /* user extensible table attributes start here */
#define MAX_EXT_TABLE_INDEX        499    /* user extensible table attributes end   here */
#define USEREXTRA421                   421 /* USER EXTRA */
#define USEREXTRA422                   422 /* USER EXTRA */
#define USEREXTRA423                   423 /* USER EXTRA */
#define USEREXTRA424                   424 /* USER EXTRA */
#define USEREXTRA425                   425 /* USER EXTRA */
#define USEREXTRA426                   426 /* USER EXTRA */
#define USEREXTRA427                   427 /* USER EXTRA */
#define USEREXTRA428                   428 /* USER EXTRA */
#define USEREXTRA429                   429 /* USER EXTRA */
#define USEREXTRA430                   430 /* USER EXTRA */
#define USEREXTRA431                   431 /* USER EXTRA */
#define USEREXTRA432                   432 /* USER EXTRA */
#define USEREXTRA433                   433 /* USER EXTRA */
#define USEREXTRA434                   434 /* USER EXTRA */
#define USEREXTRA435                   435 /* USER EXTRA */
#define USEREXTRA436                   436 /* USER EXTRA */
#define USEREXTRA437                   437 /* USER EXTRA */
#define USEREXTRA438                   438 /* USER EXTRA */
#define USEREXTRA439                   439 /* USER EXTRA */
#define USEREXTRA440                   440 /* USER EXTRA */
#define USEREXTRA441                   441 /* USER EXTRA */
#define USEREXTRA442                   442 /* USER EXTRA */
#define USEREXTRA443                   443 /* USER EXTRA */
#define USEREXTRA444                   444 /* USER EXTRA */
#define USEREXTRA445                   445 /* USER EXTRA */
#define USEREXTRA446                   446 /* USER EXTRA */
#define USEREXTRA447                   447 /* USER EXTRA */
#define USEREXTRA448                   448 /* USER EXTRA */
#define USEREXTRA449                   449 /* USER EXTRA */
#define USEREXTRA450                   450 /* USER EXTRA */
#define USEREXTRA451                   451 /* USER EXTRA */
#define USEREXTRA452                   452 /* USER EXTRA */
#define USEREXTRA453                   453 /* USER EXTRA */
#define USEREXTRA454                   454 /* USER EXTRA */
#define USEREXTRA455                   455 /* USER EXTRA */
#define USEREXTRA456                   456 /* USER EXTRA */
#define USEREXTRA457                   457 /* USER EXTRA */
#define USEREXTRA458                   458 /* USER EXTRA */
#define USEREXTRA459                   459 /* USER EXTRA */
#define USEREXTRA460                   460 /* USER EXTRA */
#define USEREXTRA461                   461 /* USER EXTRA */
#define USEREXTRA462                   462 /* USER EXTRA */
#define USEREXTRA463                   463 /* USER EXTRA */
#define USEREXTRA464                   464 /* USER EXTRA */
#define USEREXTRA465                   465 /* USER EXTRA */
#define USEREXTRA466                   466 /* USER EXTRA */
#define USEREXTRA467                   467 /* USER EXTRA */
#define USEREXTRA468                   468 /* USER EXTRA */
#define USEREXTRA469                   469 /* USER EXTRA */
#define USEREXTRA470                   470 /* USER EXTRA */
#define USEREXTRA471                   471 /* USER EXTRA */
#define USEREXTRA472                   472 /* USER EXTRA */
#define USEREXTRA473                   473 /* USER EXTRA */
#define USEREXTRA474                   474 /* USER EXTRA */
#define USEREXTRA475                   475 /* USER EXTRA */
#define USEREXTRA476                   476 /* USER EXTRA */
#define USEREXTRA477                   477 /* USER EXTRA */
#define USEREXTRA478                   478 /* USER EXTRA */
#define USEREXTRA479                   479 /* USER EXTRA */
#define USEREXTRA480                   480 /* USER EXTRA */
#define USEREXTRA481                   481 /* USER EXTRA */
#define USEREXTRA482                   482 /* USER EXTRA */
#define USEREXTRA483                   483 /* USER EXTRA */
#define USEREXTRA484                   484 /* USER EXTRA */
#define USEREXTRA485                   485 /* USER EXTRA */
#define USEREXTRA486                   486 /* USER EXTRA */
#define USEREXTRA487                   487 /* USER EXTRA */
#define USEREXTRA488                   488 /* USER EXTRA */
#define USEREXTRA489                   489 /* USER EXTRA */
#define USEREXTRA490                   490 /* USER EXTRA */
#define USEREXTRA491                   491 /* USER EXTRA */
#define USEREXTRA492                   492 /* USER EXTRA */
#define USEREXTRA493                   493 /* USER EXTRA */
#define USEREXTRA494                   494 /* USER EXTRA */
#define USEREXTRA495                   495 /* USER EXTRA */
#define USEREXTRA496                   496 /* USER EXTRA */
#define USEREXTRA497                   497 /* USER EXTRA */
#define USEREXTRA498                   498 /* USER EXTRA */
#define USEREXTRA499                   499 /* USER EXTRA */


#define MAX_DCS_NUM 500 /* change this 1 + last DCS-ATTRIBUTE-INDEX */
#define MAX_DCS_STRUCTS 525  /* should be greater than MAX_DCS_NUM */


#define MAX_ADREPL_STRING_METADATA 10  /*UDSMD */
#define MAX_ADREPL_INTEGER_METADATA 2  /*UDIMD */
#define MAX_COLL_STRING_METADATA 10  /*UDSMD_COLL */
#define MAX_COLL_INTEGER_METADATA 2  /*UDIMD_COLL */

/* DCS-ATTRIBUTE-INDEX  DEFINES  */


/******
#define DCS_T_DATA_ID          "MDAS_AD_REPL"
#define DCS_T_REPL_ENUM        "MDAS_AD_REPL"
#define DCS_T_DATA_NAME        "MDAS_AD_REPL"
#define DCS_T_ALIAS_DATA       "MDAS_AD_ALIAS"
#define DCS_T_DATA_TYP_NAME    "MDAS_TD_DATA_TYP"
#define DCS_T_SCHEMA_NAME      "MDAS_TD_DS_SCHM"  
#define DCS_T_IS_PARTITIONED   "MDAS_CD_DATA"
#define DCS_T_USER_NAME        "MDAS_CD_USER"
#define DCS_T_ACCESS_NAME      "MDAS_TD_DS_ACCS"
#define DCS_T_DOMAIN_DESC      "MDAS_TD_DOMN"
#define DCS_T_PATH_NAME        "MDAS_AD_REPL"
#define DCS_T_RSRC_NAME        "MDAS_AR_REPL"
#define DCS_T_RSRC_LOCATION    "MDAS_TD_LOCN"
#define DCS_T_RSRC_TYPE        "MDAS_TD_RSRC_TYP"
#define DCS_T_REPL_TIMESTAMP   "MDAS_AD_REPL"
#define DCS_T_DATA_GRP         "MDAS_TD_DATA_GRP"
#define DCS_T_IS_ENCRYPTED     "MDAS_AD_REPL"
#define DCS_T_IS_DYNAMIC       "MDAS_AD_REPL"
#define DCS_T_SIZE             "MDAS_AD_REPL"
#define DCS_T_PERMANENCE       "MDAS_AD_REPL" 
#define DCS_T_FLAGS            "MDAS_AD_REPL"
#define DCS_T_COMMENTS         "MDAS_AD_COMMENTS" 
#define DCS_T_AUDIT_ACTION     "MDAS_TD_ACTN"  
#define DCS_T_AUDIT_TIME       "MDAS_AD_AUDIT" 
#define DCS_T_AUDIT_COMMENTS   "MDAS_AD_AUDIT"

#define DCS_A_DATA_ID          "data_id"
#define DCS_A_REPL_ENUM        "repl_enum"
#define DCS_A_DATA_NAME        "data_name"
#define DCS_A_ALIAS_DATA       "data_name"
#define DCS_A_DATA_TYP_NAME    "data_typ_name"
#define DCS_A_SCHEMA_NAME      "schema_name"
#define DCS_A_IS_PARTITIONED   "is_partitioned"
#define DCS_A_USER_NAME        "user_name"
#define DCS_A_ACCESS_NAME      "access_constraint"
#define DCS_A_DOMAIN_DESC      "domain_desc"
#define DCS_A_PATH_NAME        "path_name"
#define DCS_A_RSRC_NAME        "rsrc_name"
#define DCS_A_RSRC_LOCATION    "netprefix"
#define DCS_A_RSRC_TYPE        "rsrc_typ_name"
#define DCS_A_REPL_TIMESTAMP   "repl_timestamp"
#define DCS_A_DATA_GRP         "data_grp_name"
#define DCS_A_IS_ENCRYPTED     "is_encrypted"
#define DCS_A_IS_DYNAMIC       "is_dynamic"
#define DCS_A_SIZE             "size"
#define DCS_A_PERMANENCE       "permanence" 
#define DCS_A_FLAGS            ""default_flags
#define DCS_A_COMMENTS         "comments"
#define DCS_A_AUDIT_ACTION     "action_desc"
#define DCS_A_AUDIT_TIME       "time_stamp"
#define DCS_A_AUDIT_COMMENTS   "comments"
****/
#define RESWANTED "WWWWW"


/*#ifndef DIAG
#define DIAG
#endif
*/




/**** MDAS COMPATIBLES ****/
typedef int mdasC_status[4];
typedef void*           mdasC_handle ;
typedef int             mdasC_logical ;
typedef int             mdasC_token ;
typedef char       mdasC_server[MAX_TOKEN];
typedef struct
{
        /* design in progress */
        mdasC_handle conn ;     /* the connection */
        mdasC_logical open ;    /* true == connection is open */
        mdasC_server sname;
} mdasC_servh ;
 
 
typedef struct
{
        /* design in progress */
        mdasC_handle ds ;       /* data stream */
        mdasC_logical open ;    /* true == stream is open */
} mdasC_datah ;
 
typedef struct
{
        /* design in progress */
        mdasC_handle info ;     /* the info */
        mdasC_token   type ;    /* MDAS_ENTITY */
        mdasC_logical packed ;  /* true == info is packed contiguous */
        mdasC_logical spooled ; /* true == info is spooled */
        mdasC_datah* spoolh ;   /* data set info handle */
} mdasC_infoh ;

#define MDAS_DB2QUERYOPTLEVEL 0

#define MDAS_SUCCESS  0
#define MDAS_ZERO   0
#define MDAS_ONE    1
#define MDAS_ERR_SERVH -1
#define MDAS_ERR_MEMORY -2
#define MDAS_ERR_TICKET -3
#define MDAS_ERR_SERVER -4
#define MDAS_FAILURE -100

#define MAX_SQL_LIMIT MAX_QUERY_LIMIT
#define MDAS_db2_CONN 0
#define MDAS_db2_DCON 0
#define MDAS_db2_INFO_INQUIRE 0




/**** MDAS COMPATIBLES (more below)****/

typedef struct
{
  char *tab_name;
  char *att_name;
  char *att_value;
}mdasC_conditioninfo;
typedef struct
{
  char *tab_name;
  char *att_name;
  char aggr_val[MAX_TOKEN];

}mdasC_selectinfo;

typedef struct
{
  char *tab_name;
  char *att_name;
  mdasC_handle values;
}mdasC_resultinfo;
typedef struct
{
  mdasC_token condition_count;
  mdasC_conditioninfo sqlwhere[MAX_SQL_LIMIT];
  mdasC_token select_count;
  mdasC_selectinfo sqlselect[MAX_SQL_LIMIT];
}mdasC_sql_query_struct;
typedef struct
{
  mdasC_token result_count;  /* number of columns in the result */
  mdasC_token row_count;   /* number of rows returned */
  mdasC_resultinfo sqlresult[MAX_SQL_LIMIT];
  int  continuation_index;  /* negative number = invalid */
} mdasC_sql_result_struct;


typedef struct
{
  int id;
  long int hstmt;
  int colcount;
  mdasC_selectinfo sqlselect[MAX_SQL_LIMIT];
} mdasC_sql_continuation_struct;

typedef struct
{
  int  cat_type;
  char* in_data_name;
  char *obj_user_name;
  char *obj_access_name;
  char *obj_domain_name;
  char *collection_name;
  mdasC_sql_result_struct  *myresult;
} getDatasetInfoStruct;


/**** MDAS COMPATIBLES ****/
extern FILE *LOGMESSFILE;
extern char logmess[];
extern unsigned char bind_file_name[];
extern short int  bind_file_name_length;
extern unsigned long  int  bind_file_option;
extern long int  bind_file_status;

typedef struct {
   char user[MAX_TOKEN];
   char server[MAX_TOKEN];
   int valid;
   long int henv;
   long int hdbc;
} MDAS_DB2_SERVER_HNDL;

/***************************************************************************
  NAME   : truncate_end_blanks
  PURPOSE: Truncates lagging blanks in a string
  INPUT  :  data_size - size of array
            data[]    - array of string characters
  OUTPUT : (none) data[] is changed with  end-of-string pushed in before blanks
****************************************************************************/
void truncate_end_blanks(int data_size, 
		    unsigned char data[]);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : open_env_db2
  PURPOSE: Opening an execution environment in db2 
  INPUT  : *henv         - pointer to a db2 environment handle
  OUTPUT :  henv         - points to handle of environment structure 
                           allocated by the call
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int open_env_db2(long int *henv);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : close_env_db2
  PURPOSE: Closing an open execution environment in db2 
  INPUT  : *henv         - pointer to a db2 environment handle
  OUTPUT : (none)        - environment structure deallocated.
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int close_env_db2(long int henv);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : connect_db2
  PURPOSE: Opening a connection to db2 server
  INPUT  :  henv         - db2 environment handle 
           *hdbc         - pointer to db2 connection handle 
            databasename - name of db2 database to be connected. the database 
	                   be a reomte database cataloged with local server
			   or client
            user         - user id
            password     - user password
  OUTPUT :  hdbc         - points to handle of connection structure 
                           allocated by the call
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int connect_db2(long int       henv,
		long int      *hdbc,
		unsigned char *databasename, 
		unsigned char *user, 
		unsigned char *password);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : disconnect_db2
  PURPOSE: Closing an open connection  in db2 
  INPUT  : henv         - db2 environment handle
           hdbc         - db2 connection handle
	   transstate   - commit (0) or rollback (1) transactions performed
	                  during the connection, after last transact_db2()
			  or from connect_db2()
  OUTPUT : (none)        - connection structure deallocated.
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int disconnect_db2(long int           henv,
		   long int           hdbc, 
                   unsigned short int transstate);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : exec_no_res_sql_db2
  PURPOSE: Execute a SQL query which has no resulting table, eg. INSERT,
           DELETE, UPDATE or DDL
  INPUT  : henv         - db2 environment handle
           hdbc         - db2 connection handle
	   query        - SQL query
  OUTPUT : (none)       
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int exec_no_res_sql_db2(long int       henv, 
                        long int       hdbc, 
		        unsigned char *query);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : exec_sql_db2
  PURPOSE: Execute a SQL query which mave have  resulting table
  INPUT  : henv         - db2 environment handle
           hdbc         - db2 connection handle
	  *hstmt        - pointer to statement environment handle
	   query        - SQL query
  OUTPUT : hstmt        - points to handle of statement structure allocated 
                           by the call
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int exec_sql_db2(long int henv, 
		 long int hdbc, 
		 long int *hstmt,
		 unsigned char *query);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : done_with_result_db
  PURPOSE: Close a stament environment opened by exec_sql_db2() but which
           used user allocated result structure
  INPUT  : hstmt        - statement structure handle
  OUTPUT : (none)       - statement structure deallocated
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int done_with_result_db2(long int hstmt);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : done_with_default_result_db2
  PURPOSE: Close a stament environment opened by exec_sql_db2() which used
           result structure build by bind_default_row_db2()
  INPUT  : hstmt        - statement structure handle
           data[]       - string array allocated by bind_default_row_db2()
	   data_size	- integer array allocated by bind_default_row_db2()
	   num_of_cols  - number of columns in the result
  OUTPUT : (none)       - statement structure, data, data_size arrays are
                          deallocated
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int done_with_default_result_db2(long int       hstmt,
				 unsigned char *data[],
				 long int      *data_size,
				 short int      num_of_cols);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : bind_default_row_db2
  PURPOSE: Allocate a default set of result holders to hold one row of 
           result of a SQL statement. 
  INPUT  : hstmt        - statement structure handle
          *data[]       - pointer to string array allocated in the call for 
	                  transmitting per-row result to user (size of array
			  is dynamically assigned to num_of_cols)
	 *data_size    - pointer to pointer to integer array allocated in 
	                  the call  for transmitting size of each cell-value 
			  in the result to user (size of array as above)
	  *num_of_cols  - pointer to integer holding value of number of 
	                  columns in the result (assigned in the call)
  OUTPUT : (none)       - data and data_size arrays allocated.
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int bind_default_row_db2(long int        hstmt,
		         unsigned char  *data[],
			 long int      data_size[], 
		         short int      *num_of_cols);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : get_row_db2
  PURPOSE: Fetch the next (unfetched)  row of result in the 
           row-holder bound by any of the bind_***_db2() calls. To fetch
	   multiple row use get_rows_db2()
  INPUT  : hstmt        - statement structure handle
  OUTPUT : (none)       - result values placed in data and size variables
                          bound by bind_***_db2() 
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int get_row_db2(long int hstmt);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : get_rows_db2
  PURPOSE: Fetch the next set of  rows of result in the 
           column-arrays bound by any of the bind_***_db2() calls. To fetch
	   a single row use get_row_db2()
  INPUT  : hstmt        - statement structure handle
          *pcrow        - pointer to variable that holds actual number of rows 
	                  fetched by this call.
  OUTPUT : pcrow        - actual number of values fetched by the call.
			- result values placed in data and size arrays 
                          bound by bind_***_db2()
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int get_rows_db2(long int            hstmt, 
                  unsigned long int *pcrow);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : bind_column_db2
  PURPOSE: Allocate an array (or a variable) to hold a column (or cell)
           of results of a  get_rows_db2 statement. 
  INPUT  : hstmt        - statement structure handle
           *data_column - pointer to a (structure) variable or a 
                          1-dimensional array of structures to 
                          be used to return results in a get_row_db2() or
	                  get_rows_db2() call.
			  Array assigned by user.
	  *size_column  - pointer integer or arrayof integer  to be used to 
                          return size of result in each cell of the column
	  column_type   - SQL type of column. Zero will make the call
	                  to use internal defaults depending upon column type
			  (recommended). Note that using the column type
			  returned by get_column_info_db2() may cause trouble
			  when VAR types are used in DDL!!
	  max_data_size - size of data_column cells (in bytes)
	  column_number - the column to which the array is to be bound
	  row_size      - number of rows to be fetched by get_rows_db2().
	                  when calling bind_column_db2 for more than one 
			  column in the same result make sure
			  that the row_size is the same. row_size can be
			  smaller than the array sizes of data_column and
			  size_column.
  OUTPUT : (none)       - binding performed to user given arrays.
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int bind_column_db2(long int    hstmt,
		    void       *data_column,
		    void       *size_column,
		    short int   column_type,
		    long int    max_data_size,
		    short int   column_number,
		    long int    row_size);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : bind_files_to_column_db2
  PURPOSE: Allocate an array of file name holders to hold a LOB column
           of results of a  get_rows_db2 statement. 
  INPUT  : hstmt        - statement structure handle
           *file_names  - pointer to an 1-dimensional array of filenames to
                          be used to return results in a get_row_db2() or
	                  get_rows_db2() call. Note that the column should
			  be of LOB type. Otherwise error is flagged when
			  getting rows. This array is used during 
			  get_rows_db2 statement and can be assigned values
			  just before the get call.
			  Array assigned by user.
	  *file_name_cell_size - pointer to an integer array that holds
	                  the length (in characters) of file names. This array
			  is used during get_rows_db2 statement and can be 
			  assigned values just before the get call.
			  Array assigned by user.
	  *file_options - Type of file operation to be applied to each file
	                  in the array.
	                  SQL_FILE_READ, SQL_FILE_CREATE, SQL_FILE_OVERWRITE,
			  SQL_FILE_APPEND are possible values.
			  Note that the value passed is a pointer to an array.
	  *file_status  - Pointer to an array that can hold some status
	                  of the call. Usage unknown!!
	  *result_size_column -   Pointer to an array that can hold the size of
	                  LOBs stored in each file. 
	  column_number - the column to which the array is to be bound
	  row_size      - number of rows to be fetched by get_rows_db2().
	                  when calling bind_column_db2 for more than one 
			  column in the same result make sure
			  that the row_size is the same. row_size can be
			  smaller than the array sizes of data_column and
			  size_column.
  OUTPUT : (none)       - binding performed to user given arrays.
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int bind_files_to_column_db2(long int            hstmt,
			     void               *file_names,
			     short int          *file_name_cell_size,
			     unsigned long int  *file_options,
			     long int           *file_status,
			     long int           *result_size_column,
			     unsigned short int  column_number,
			     long int            row_size);   
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : bind_file_to_cell_db2
  PURPOSE: Allocate a file name holder to hold a LOB 
           in a row of a  get_row_db2 statement. 
  INPUT  : hstmt        - statement structure handle
           *file_name  - pointer to a variable that holds a filename to
                          be used to return results in a get_row_db2() or
	                  get_rows_db2() call. Note that the column should
			  be of LOB type. Otherwise error is flagged when
			  getting rows. This value is used during 
			  get_rows_db2 statement and can be assigned values
			  just before the get call.
			  String space  allocated by user.
	  *file_name_length - pointer to an integer  that holds
	                  the length (in characters) of the file names. Value
			  used during get_row_db2 statement and can be 
			  assigned a value just before the get call.
			  Integer variable space allocated by user.
	  *file_option - Type of file operation to be applied to the file
	                  SQL_FILE_READ, SQL_FILE_CREATE, SQL_FILE_OVERWRITE,
			  SQL_FILE_APPEND are possible values.
			  Note that the value passed is a pointer.
	  *file_status  - Pointer to a variable that can hold some status
	                  of the call. Usage unknown!!
	  *result_size_column -   Pointer to a variable that can hold the 
	                  size of LOB stored in the  file. 
	  column_number - the column to which the array is to be bound
  OUTPUT : (none)       - binding performed to user given arrays.
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int bind_file_to_cell_db2(long int            hstmt,
			  unsigned char      *file_name,
			  short int          *file_name_length,
			  unsigned long int  *file_option,
			  long int           *file_status,
			  long int           *result_size_column,
			  unsigned short int  column_number);
/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : bind_n_rows_db2
  PURPOSE: Allocate arrays  to hold all column of the result of a 
           get_rows_db2 statement. 
  INPUT  : hstmt        - statement structure handle
          *data_column[]- pointer to an array of 1-dimensional arrays of 
	                  (possibly different types of) structures to 
                          be used to return results in a get_rows_db2() call.
			  Arrays assigned by user.
	  *size_column[]- pointer to an array of 1-dimensional integer arrays 
                          to be used to return size
	                  of result in each cell of the column
	  column_type[] - array of SQL type of column. Zero will make the call
	                  to use internal defaults depending upon column type
			  (recommended). Note that using the column type
			  returned by get_column_info_db2() may cause trouble
			  when VAR types are used in DDL!!
	  max_data_size[]- array of sizes of data_column cells (in bytes)
	  num_of_columns- number ofcolumn in result of an execution. can be 
	                  found using get_no_of_columns_db2()
	  row_size      - number of rows to be fetched by get_rows_db2().
	                  when calling bind_column_db2 for more than one 
			  column in the same result make sure
			  that the row_size is the same. row_size can be
			  smaller than the array sizes of data_column and
			  size_column.
  OUTPUT : (none)       - binding performed to user given arrays.
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int bind_n_rows_db2(long int   hstmt,
		    void      *data_column[],
		    void      *size_column[],
		    long int   max_data_size[],
		    short int  column_type[],
		    short int  num_of_columns,
		    long int   row_size);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : get_no_of_columns_db2
  PURPOSE: Find the number of columns of the result table of an
           exec_sql_db2 statement. 
  INPUT  : hstmt        - statement structure handle
          *num_of_cols  - pointer to variable to hold the result of the call.
  OUTPUT : num_of_cols  - bound to the column cardinality of the last
                          exec_sql_db2  call.
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int get_no_of_columns_db2(long int   hstmt,
			  short int *num_of_cols);

/*-------------------------------------------------------------------------*/
/***************************************************************************
  NAME   : get_column_info_db2
  PURPOSE: Get assorted column attributes  such as column name, size (as 
           defined in DDL), type, nullability, precision and its scale.
  INPUT  : hstmt        - statement structure handle
           column_number- column whose attribute is queried.
	   buf_size     - size of string array in colname
  OUTPUT : displaysize  - maximum size of data storable in the column
	                  in ASCII character size
	   colname[]    - string array to hold the column name
	   coltype      - type of column (enumeration) 
	   colnamelen   - length of string returned in colname
	   nullable     - is the column nullable
	   precision    - prcision of the column data type
	   scale        - scale of the column data type
  RETURN : 0 for SUCCESS and  negatives for error
****************************************************************************/

int get_column_info_db2(long int           hstmt,
			short int          column_number,
			long int          *displaysize,
			unsigned char      colname[],
			short int         *coltype,
                        short int          buf_size,
                        short int         *colnamelen,
                        short int         *nullable,
			unsigned long int *precision,
                        short int         *scale
			);

/*-------------------------------------------------------------------------*/

#endif	/* MDASC_DB2_EXTERNS_H */
