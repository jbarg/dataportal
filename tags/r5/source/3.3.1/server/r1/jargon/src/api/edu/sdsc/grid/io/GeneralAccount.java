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
//	GeneralAccount.java	-  edu.sdsc.grid.io.GeneralAccount
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.GeneralAccount
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;


/**
 * An object to hold the user information used when connecting
 * to a file system.
 *<P>
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public abstract class GeneralAccount extends Object implements Cloneable
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * The home directory on the server
	 */
	protected String homeDirectory;



//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Constructs an object to hold the user information used when connecting
	 * to a file system.
	 * <P>
	 * @param	homeDirectory home directory on the SRB
	 */
	public GeneralAccount( String homeDir )
	{
		setHomeDirectory( homeDir );
	}


	/**
	 * Finalizes the object by explicitly letting go of each of
	 * its internally held values.
	 * <P>
	 */
	protected void finalize( )
	{
		if (homeDirectory != null)
			homeDirectory = null;
	}



//----------------------------------------------------------------------
// Setters and Getters
//----------------------------------------------------------------------
	/**
	 * Sets the home directory of this GeneralAccount.
	 */
	public abstract void setHomeDirectory( String homeDir );


	/**
	 * Returns the homeDirectory used by this GeneralAccount.
	 *
	 * @return homeDirectory
	 */
	public String getHomeDirectory( )
		throws NullPointerException
	{
		if ( homeDirectory != null )
			return homeDirectory;

		throw new NullPointerException();
	}


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


	public abstract boolean equals( Object obj );
}

