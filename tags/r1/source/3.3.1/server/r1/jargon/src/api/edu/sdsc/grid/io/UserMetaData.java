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
//	UserMetaData.java	-  edu.sdsc.grid.io.UserMetaData
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.StandardMetaData
//	    			|
//	    			+-.UserMetaData
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;


/**
 * The metadata naming interface for user metadata.
 * Some meta groups are standard regardless of implementation. To
 * handle standard metadata, we defined interfaces that are implemented
 * by protocol-specific metadata groups. The UserMetaData interface
 * contract is that the implementor supports meta data we might find
 * as common user metadata, such as:
 *<ul>
 * <li>  USER_ID
 * <li>  USER_NAME
 * <li>  USER_GROUP_NAME
 * <li>  USER_ADDRESS
 * <li>  USER_PHONE
 * <li>  USER_EMAIL
 *</ul>
 *<P>
 * An implementation specific class, such as the SRB user metadata must
 * support these fields, but also may support further user metadata
 * fields, such as:
 *<ul>
 * <li>  USER_TYPE_NAME
 * <li>  USER_DOMAIN
 * <li>  USER_AUDIT_TIME_STAMP
 * <li>  USER_AUDIT_COMMENTS
 * <li>  USER_DISTINGUISHED_NAME
 * <li>  USER_AUTHENTICATION_SCHEME
 *</ul>
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
*/
public interface UserMetaData extends StandardMetaData
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
	public final static String USER_ID	=	"user id";
	public final static String USER_GROUP_NAME	=	"user group name";
	public final static String USER_NAME	=	"user name";
	public final static String USER_ADDRESS	=	"user address";
	public final static String USER_PHONE	=	"user phone";
	public final static String USER_EMAIL	=	"user email";

//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------


//----------------------------------------------------------------------
// Methods
//----------------------------------------------------------------------


}
