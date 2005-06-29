//
//	Copyright (c) 2003  San Diego Supercomputer Center (SDSC),
//	University of California, San Diego (UCSD), San Diego, CA, USA.
//
//	Users and possessors of this source code are hereby granted a
//	nonexclusive, royalty-free copyright and design patent license
//	to use this code in individual software.  License is not granted
//	for commercial resale, in whole or in part, without prior written
//	permission from SDSC/UCSD.  This source is provided "AS IS"
//	without express or implied warranty of any kind.
//
//
//  FILE
//	GeneralMetaData.java	-  edu.sdsc.grid.io.GeneralMetaData
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.StandardMetaData
//	    			|
//	    			+-.GeneralMetaData
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;


/**
 * The metadata naming interface for general metadata.
 * Some meta groups are standard regardless of implementation. To
 * handle standard metadata, we defined interfaces that are implemented
 * by protocol-specific metadata groups. The GeneralMetaData interface
 * contract is that the implementor supports meta data we might find
 * as common general metadata, such as:
 *<ul>
 * <li>  FILE_NAME
 * <li>  FILE_GROUP_NAME
 * <li>  SIZE
 * <li>  CREATION_DATE
 * <li>  MODIFICATION_DATE
 * <li>  OWNER
 * <li>  ACCESS_CONSTRAINT
 * <li>  FILE_COMMENTS
 * <li>  DIRECTORY_NAME
 *</ul>
 *<P>
 * An implementation specific class, such as SRB general metadata must
 * support these fields, but also may support further general metadata
 * fields, such as:
 *<ul>
 * <li>  Replication number
 * <li>  File type
 * <li>  Data classification name
 * <li>  Data classification type
 * <li>  Access constraint
 * <li>  Comments
 * <li>  Comments date
 * <li>  Deleted flag
 * <li>  Owner domain
 * <li>  Owner email address
 *</ul>
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public interface GeneralMetaData extends StandardMetaData
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
	public final static String FILE_NAME	=	"file name";
	public final static String FILE_GROUP_NAME	=	"file group name";
	public final static String SIZE	=	"size";
	public final static String CREATION_DATE	=	"creation date";
	public final static String MODIFICATION_DATE	=	"modification date";
	public final static String OWNER	=	"owner";
	public final static String ACCESS_CONSTRAINT	=	"access constraint";
	public final static String DIRECTORY_NAME	=	"directory name";
/*TODO which are general enough?
	public final static String FILE_COMMENTS	=	"FILE_COMMENTS";
	public final static String FILE_ID	=	"FILE_ID";
	public final static String FILE_TYPE_NAME	=	"FILE_TYPE_NAME";
	public final static String FILE_REPLICATION_ENUM	=	"FILE_REPLICATION_ENUM";
	public final static String IS_DIRTY	=	"IS_DIRTY";
	public final static String FILE_ACCESS_LIST	=	"FILE_ACCESS_LIST";
	public final static String FILE_IS_DELETED	=	"FILE_IS_DELETED";
	public final static String FILE_GROUP_ID	=	"FILE_GROUP_ID";
	public final static String FILE_HIDE	=	"FILE_HIDE";
	public final static String FILE_CHECKSUM	=	"FILE_CHECKSUM";
	public final static String FILE_AUDITFLAG	=	"FILE_AUDITFLAG";
	public final static String FILE_LOCK_EXPIRY	=	"FILE_LOCK_EXPIRY";
	public final static String FILE_PIN_VAL	=	"FILE_PIN_VAL";
	public final static String FILE_PIN_OWNER_NAME	=	"FILE_PIN_OWNER_NAME";
	public final static String FILE_PIN_OWNER_DOMAIN	=	"FILE_PIN_OWNER_DOMAIN";
	public final static String FILE_PIN_EXPIRY	=	"FILE_PIN_EXPIRY";
	public final static String FILE_EXPIRY_DATE	=	"FILE_EXPIRY_DATE";
	public final static String FILE_IS_COMPRESSED	=	"FILE_IS_COMPRESSED";
	public final static String FILE_IS_ENCRYPTED	=	"FILE_IS_ENCRYPTED";
	public final static String FILE_EXPIRE_DATE_2	=	"FILE_EXPIRE_DATE_2";
	public final static String FILE_SEG_NUM	=	"FILE_SEG_NUM";
	public final static String FILE_CLASS_NAME	=	"FILE_CLASS_NAME";
	public final static String FILE_CLASS_TYPE	=	"FILE_CLASS_TYPE";
	public final static String PATH_NAME	=	"PATH_NAME";
	public final static String OWNER_EMAIL	=	"OWNER_EMAIL";
	public final static String OWNER_DOMAIN	=	"OWNER_DOMAIN";
	public final static String FILE_LAST_ACCESS_TIMESTAMP	=	"FILE_LAST_ACCESS_TIMESTAMP";
	public final static String FILE_VER_NUM	=	"FILE_VER_NUM";
	public final static String FILE_LOCK_NUM	=	"FILE_LOCK_NUM";
	public final static String FILE_LOCK_DESCRIPTION	=	"FILE_LOCK_DESCRIPTION";
	public final static String FILE_LOCK_OWNER_NAME	=	"FILE_LOCK_OWNER_NAME";
	public final static String FILE_LOCK_OWNER_DOMAIN	=	"FILE_LOCK_OWNER_DOMAIN";
*/

//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------


//----------------------------------------------------------------------
// Methods
//----------------------------------------------------------------------


}
