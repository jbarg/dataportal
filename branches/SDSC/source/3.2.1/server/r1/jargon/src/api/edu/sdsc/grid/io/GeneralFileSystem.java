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
//	GeneralFileSystem.java	-  edu.sdsc.grid.io.GeneralFileSystem
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.GeneralFileSystem
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;


import java.io.IOException;
import java.io.FileNotFoundException;

/**
 * The GeneralFileSystem class is the common superclass for connection
 * implementations to any file system. It provides the framework
 * to support specific file system semantics.
 * Specifically, the functions needed to interact with a file system
 * are provided abstractly by GeneralFileSystem and concretely by its
 * subclass(es).
 *<P>
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public abstract class GeneralFileSystem extends Object implements Cloneable
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * The account info for connecting to the server.
	 */
	protected GeneralAccount account;



//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Finalizes the object by explicitly letting go of each of
	 * its internally held values.
	 * <P>
	 */
	protected void finalize( )
		throws Throwable
	{
		account = null;
	}



//----------------------------------------------------------------------
// Setters and Getters
//----------------------------------------------------------------------
	/**
	 * Sets the account object, the info used to connect to the file system.
	 */
	protected abstract void setAccount( GeneralAccount account )
		throws FileNotFoundException, IOException;

	/**
	 * Returns a copy of the account object used by this GeneralFileSystem.
	 */
	public GeneralAccount getAccount( )
		throws NullPointerException
	{
		if ( account != null )
			return (GeneralAccount) account.clone();

		throw new NullPointerException();
	}

	/**
	 * Returns the homeDirectory used by this account on GeneralFileSystem.
	 */
	public String getHomeDirectory( )
	{
		return account.getHomeDirectory( );
	}

	/**
	 * Returns the rootDirectory used by this file system.
	 */
	public abstract String[] getRootDirectories( );


//----------------------------------------------------------------------
// Methods
//----------------------------------------------------------------------
	/**
	 *
	 */
	public abstract MetaDataRecordList[] query(
  	MetaDataCondition[] conditions, MetaDataSelect[] selects )
  	throws IOException;

	/**
	 *
	 */
	public abstract MetaDataRecordList[] query(	MetaDataCondition[] conditions,
		MetaDataSelect[] selects, int numberOfRecordsWanted )
  	throws IOException;



	/**
	 * @return a copy of this account object.
	 */
	public Object clone() {
		try {
				return super.clone();
		} catch (CloneNotSupportedException e) {
			//Shouldn't happen
			throw new InternalError();
		}
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
	public abstract boolean equals( Object obj );

	/**
	 * Returns a string representation of this file system object.
	 */
	public String toString( )
	{
		return new String( "GeneralFileSystem, "+getHomeDirectory() );
	}
}
