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
//	LocalAccount.java	-  edu.sdsc.grid.io.LocalAccount
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.GeneralAccount
//				    |
//				    +-.LocalAccount
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.local;

import edu.sdsc.grid.io.*;


/**
 * An object to hold the user information used when connecting
 * to a remote server.
 *<P>
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public class LocalAccount extends GeneralAccount
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------



//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Constructs an object to hold the user information used when connecting
	 * to a remote server.
	 * <P>
	 * @param	homeDirectory home directory on the SRB
	 */
	public LocalAccount( )
	{
		this( null );
	}

	/**
	 * Constructs an object to hold the user information used when connecting
	 * to a remote server.
	 * <P>
	 * @param	homeDirectory home directory on the SRB
	 */
	public LocalAccount( String homeDir )
	{
		super( homeDir );
	}



//----------------------------------------------------------------------
// Setters and Getters
//----------------------------------------------------------------------
	/**
	 * Sets the home directory of this GeneralAccount.
	 */
	public void setHomeDirectory( String homeDir )
	{
		if (homeDir == null) {
			homeDirectory = System.getProperty( "user.home" );
		}
		else {
			homeDirectory = homeDir;
		}
	}

	/**
	 * Tests this local file system account object for equality with the
	 * given object.
	 * Returns <code>true</code> if and only if the argument is not
	 * <code>null</code> and both are account objects for the same
	 * filesystem.
	 *
	 * @param   obj   The object to be compared with this abstract pathname
	 *
	 * @return  <code>true</code> if and only if the objects are the same;
	 *          <code>false</code> otherwise
	 */
	public boolean equals( Object obj )
	{
		try {
  		if (obj == null)
  			return false;

			if (obj instanceof LocalAccount) {
				return true;
			}
		} catch (ClassCastException e) {
			return false;
		}
		return false;
	}
}

