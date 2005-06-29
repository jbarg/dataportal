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
//	LocalFileSystem.java	-  edu.sdsc.grid.io.srb.LocalFileSystem
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.GeneralFileSystem
//	            |
//	            +-.LocalFileSystem
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.local;

import edu.sdsc.grid.io.*;

import java.io.IOException;
import java.io.FileNotFoundException;


/**
 * The LocalFileSystem class is the class for connection implementations
 * to the local file systems. It was added to the GeneralFileSystem tree
 * to provide compatibility and support for metadata queries. Unfortuanely,
 * local file systems are not well optimized to this purpose thus any queries
 * may take a significant amount of time.
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public class LocalFileSystem extends GeneralFileSystem
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
	/**
	 * Default number of records returned by a query
	 */
	static final int DEFAULT_RECORDS_WANTED = 300;
//appropriate number?

//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------




//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Opens a socket connection to read from and write to.
	 * Loads the default Local user account information from their home directory.
	 * The account information stored in this object cannot be changed once
	 * constructed.
	 *<P>
	 * This constructor is provided for convenience however,
	 * it is recommended that all necessary data be sent
	 * to the constructor and not left to the defaults.
	 *
	 * @throws FileNotFoundException if the user data file cannot be found.
	 * @throws IOException if an IOException occurs.
	 */
	public LocalFileSystem( )
	{
		this( new LocalAccount() );
	}

	/**
	 * Opens a socket connection to read from and write to. Opens the account
	 * held in the LocalAccount object. The account information stored in this
	 * object cannot be changed once constructed.
	 *<P>
	 * This constructor is provided for convenience however,
	 * it is recommended that all necessary data be sent
	 * to the constructor and not left to the defaults.
	 *
	 * @param localAccount	the Local account information object.
	 * @throws FileNotFoundException if the user data file cannot be found.
	 * @throws IOException if an IOException occurs.
	 */
	public LocalFileSystem( LocalAccount localAccount )
	{
		if ( localAccount == null )
			account = new LocalAccount();
		else
			account = (LocalAccount) localAccount.clone();
	}





//----------------------------------------------------------------------
// Setters and Getters
//----------------------------------------------------------------------
//General
	/**
	 * Sets the <code>account</code> object.
	 */
	protected void setAccount( GeneralAccount account )
	{
		if ( account == null )
			account = new LocalAccount();
		else
			this.account = (LocalAccount) account.clone();
	}

	/**
	 * Returns the root directories of the local file system.
	 */
	public String[] getRootDirectories( )
	{
		GeneralFile[] roots = LocalFile.listRoots();
		String[] rootStrings = new String[roots.length];
		for (int i=0;i<roots.length;i++) {
			rootStrings[i] = roots[i].toString();
		}

		return rootStrings;
	}




//----------------------------------------------------------------------
// GeneralFileSystem methods
//----------------------------------------------------------------------
	/**
	 * Queries the file system to find all files that
   * match a set of conditions. For all those that
   * match, the fields indicated in the select array
   * are returned in the result object.
   */
  public MetaDataRecordList[] query(
  	MetaDataCondition[] conditions, MetaDataSelect[] selects )
  	throws IOException
  {
		throw new UnsupportedOperationException();
  }


	/**
	 * Queries the file system to find all files that
   * match a set of conditions. For all those that
   * match, the fields indicated in the select array
   * are returned in the result object.
	 *<P>
   * While condition and select array objects have all
   * been checked for self-consistency during their
   * construction, there are additional problems that
   * must be detected at query time:
	 *<P>
   * <ul>
   *	<li>Redundant selection fields
   *	<li>Redundant query fields
   *	<li>Fields incompatible with a file system
   * </ul>
	 *<P>
   * For instance, it is possible to build a condition
   * object appropriate for the Local system, then pass that object
   * in a local file system query. That will find that
   * the condition is incompatible and generate a mismatch
   * exception.
	 *<P>
   * Query is implemented by the file-system-specific
   * classes, like that for the SRB, FTP, etc. Those
   * classes must re-map condition and select field names
   * and operator codes to those required by a particular
   * file system and protocol version. Once re-mapped,
   * they issue the query and get results. The results
   * are then mapped back to the standard public field
   * names of the MetaDataGroups. So, if a MetaDataGroup
   * uses a name like "file path", but the SRB calls it
   * "data name", then query maps first from "file path"
   * to "data name" before issuing the query, and then
   * from "data name" back to "file path" within the
   * results. The programmer using this API should never
   * see the internal field names.
   *
   * @param conditionArray The conditional statements that describe
   *		the values to query the system, like WHERE in SQL.
   * @param selectArray The attributes to be returned from those values that
   * 		met the conditions, like SELECT in SQL.
	 */
  public MetaDataRecordList[] query( MetaDataCondition[] conditions,
  	MetaDataSelect[] selects, int recordsWanted )
  	throws IOException
  {
		throw new UnsupportedOperationException();
  }


	/**
	 * Tests this filesystem object for equality with the given object.
	 * Returns <code>true</code> if and only if the argument is not
	 * <code>null</code> and both are filesystem objects connected to the
	 * same filesystem using the same account information.
	 *
	 * @param   obj   The object to be compared with this abstract pathname
	 *
	 * @return  <code>true</code> if and only if the objects are the same;
	 *          <code>false</code> otherwise
	 */
	public boolean equals(Object obj)
	{
		try {
  		if (obj == null)
  			return false;

			LocalFileSystem temp = (LocalFileSystem) obj;

			if (getAccount().equals(temp.getAccount())) {
				return true;
			}
		} catch (ClassCastException e) {
			return false;
		}
		return false;
	}
}
