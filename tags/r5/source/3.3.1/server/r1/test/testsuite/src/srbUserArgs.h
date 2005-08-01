/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
#ifndef SRB_USER_ARGS_H
#define SRB_USER_ARGS_H

/* includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/* various definitions */

#define SRB_ARG_ILLEGAL  (0)
#define SRB_ARG_REQUIRED (-1)
#define SRB_ARG_OPTIONAL (-2)

#define SRB_SYST_NUMS  {0, 1, 2, 3, 4}
#define SRB_SYST_NAMES {"unix", "unitree", "hpss", "ftp", "http"}
#define SRB_SYST_COUNT (5)

#define SRB_DBMS_NUMS  {0, 1, 5}
#define SRB_DBMS_NAMES {"db2", "illustra", "oracle"}
#define SRB_DBMS_COUNT (3)

#define SRB_CATT_NUMS  {0, 1, 2}
#define SRB_CATT_NAMES {"MDAS", "LDAP", "FILE"}
#define SRB_CATT_COUNT (3)

#define SRB_OP_NUMS  {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31}
#define SRB_OP_NAMES { "D_DELETE_ONE", "D_DELETE_DOMN", "D_INSERT_DOMN", "D_CHANGE_SIZE", "D_CHANGE_TYPE", "D_CHANGE_GROUP", "D_CHANGE_SCHEMA", "D_INSERT_ACCS", "D_DELETE_ACCS", "D_DELETE_ALIAS", "D_INSERT_ALIAS", "D_DELETE_COMMENTS", "D_INSERT_COMMENTS", "D_INSERT_AUDIT", "D_CHANGE_OWNER", "U_CHANGE_PASSWORD", "U_DELETE", "U_DELETE_DOMN", "U_INSERT_DOMN", "U_DELETE_GROUP", "U_INSERT_GROUP", "U_CHANGE_TYPE", "U_DELETE_ALIAS", "U_INSERT_ALIAS", "U_CHANGE_VERKEY", "U_CHANGE_ENCKEY", "D_INSERT_COLL_ACCS", "D_DELETE_COLL_ACCS", "D_CHANGE_DNAME", "D_CHANGE_COLL_NAME", "D_DELETE_COLL" }
#define SRB_OP_COUNT (31)

#define SRB_USERARG_MAXC (128)
/*
#define SRB_BSIZE_DFLT   (1024*1024)
*/
#define SRB_BSIZE_DFLT   (1024*256)
#define SRB_AUTH_DFLT (NULL)

/* user argument types */

#define SRB_ARG_TYPE_NULL	(0)
#define SRB_ARG_TYPE_INT	(1)
#define SRB_ARG_TYPE_STRING	(2)
#define SRB_ARG_TYPES		(3)
#define SRB_ARG_TYPENAME	{ "(null)", "(int)", "(string)" }

/* user argument values */

#define SRB_USER_ARG_ERR	( 0)	/* -err */
#define SRB_USER_ARG_HELP	( 1)	/* -help */
#define SRB_USER_ARG_VERB	( 2)	/* -verbose */

#define SRB_USER_ARG_FROM	( 3)	/* -from */
#define SRB_USER_ARG_TO 	( 4)	/* -to */
#define SRB_USER_ARG_FILE	( 5)	/* -file */
#define SRB_USER_ARG_DIR	( 6)	/* -dir */
#define SRB_USER_ARG_OBJ	( 7)	/* -obj */
#define SRB_USER_ARG_DBMS	( 8)	/* -dbms */
#define SRB_USER_ARG_BYTES	( 9)	/* -bytes */
#define SRB_USER_ARG_MODE	(10)	/* -mode */
#define SRB_USER_ARG_USER	(11)	/* -user */
#define SRB_USER_ARG_RSRC	(12)	/* -rsrc */
#define SRB_USER_ARG_HOST	(13)	/* -host */
#define SRB_USER_ARG_SYST	(14)	/* -syst */
#define SRB_USER_ARG_CATT	(15)	/* -cattype */
#define SRB_USER_ARG_BSIZE	(16)	/* -bufsize */
#define SRB_USER_ARG_AUTH	(17)	/* -auth */
#define SRB_USER_ARG_PORT	(18)	/* -port */
#define SRB_USER_ARG_SEEK	(19)	/* -seek */
#define SRB_USER_ARG_ID 	(20)	/* -id */
#define SRB_USER_ARG_DMN	(21)	/* -domain */
#define SRB_USER_ARG_ATTR1	(22)	/* -a1 */
#define SRB_USER_ARG_ATTR2	(23)	/* -a2 */
#define SRB_USER_ARG_OP 	(24)	/* -op */
#define SRB_USER_ARG_TABLE 	(25)	/* -table */
#define SRB_USER_ARG_COLL	(26)	/* -coll */
#define SRB_USER_ARG_TYPE	(27)	/* -type */
#define SRB_USER_ARG_ON		(28)	/* -on */
#define SRB_USER_ARG_OFF	(29)	/* -off */
#define SRB_USER_ARG_PRINT	(30)	/* -print */
#define SRB_USER_ARG_ATTR3	(31)	/* -a3 */
#define SRB_USER_ARG_PARENT	(32)	/* -parent */
#define SRB_USER_ARG_CONTAINER	(33)	/* -container */

#define SRB_USER_ARGS		(34)
#define SRB_USER_ARGV		{ "-err", "-help", "-verbose", "-from", "-to", "-file", "-dir", "-obj", "-dbms", "-bytes", "-mode", "-user", "-rsrc", "-host", "-syst", "-cattype", "-bufsize", "-auth", "-port", "-seek", "-id", "-domain", "-a1", "-a2", "-op", "-table", "-coll", "-type", "-on", "-off", "-print", "-a3" , "-parent", "-cont" }

#define SRB_USAGE_ARGV		{ "-err", "-help", "-verbose", "-from local_file1 [file2 ... fileN]", "-to local_file1 [file2 ... fileN]", "-file srb_file1 [file2 ... fileN]", "-dir srb_dir1 [dir2 ... dirN]", "-obj srb_obj1 [obj2 ... objN]", "-dbms {db2,illustra}", "-bytes #bytes", "-mode unix_like_mode", "-user user_name", "-rsrc resource_name", "-host host_name", "-syst {unix,unicos,hpss,ftp}", "-cattype catalog_type", "-bufsize i/o_buffer_size", "-auth authentication_key", "-port port#", "-seek seek_location (in bytes)", "-id obj_id (user supplied string)", "-domain domain_name", "-a1 attribute1", "-a2 attribute2","-op operation", "-table table_name", "-coll collection_name", "-type data_type_name", "-on", "-off", "-print", "-a3 attribute3" , "-parent parent_collection" , "-cont CONTAINER"}

#define SRB_ARG_TYPEV		{ SRB_ARG_TYPE_NULL , SRB_ARG_TYPE_NULL , SRB_ARG_TYPE_NULL , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_INT , SRB_ARG_TYPE_INT , SRB_ARG_TYPE_INT , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_INT , SRB_ARG_TYPE_INT , SRB_ARG_TYPE_INT , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_INT , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_INT , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING, SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_NULL , SRB_ARG_TYPE_NULL , SRB_ARG_TYPE_NULL , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING , SRB_ARG_TYPE_STRING }

/* prototypes */

void srbUserArgs(
	const char *token,
	int argc,
	char *argv[],
	int srbargc[SRB_USER_ARGS],
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC]
	) ;

void srbFreeArgs(
	int srbargc[SRB_USER_ARGS],
	void *srbargv[SRB_USER_ARGS][SRB_USERARG_MAXC]
	) ;

#endif
