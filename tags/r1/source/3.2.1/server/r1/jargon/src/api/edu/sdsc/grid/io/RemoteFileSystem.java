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
//	RemoteFileSystem.java	-  edu.sdsc.grid.io.RemoteFileSystem
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-edu.sdsc.grid.io.GeneralFileSystem
//	 			   |
//	 			   +-.RemoteFileSystem
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;


/**
 * The RemoteFileSystem class is the common superclass for connection
 * implementations to remote file systems. It provides the framework
 * to support a wide range of remote semantics.
 * Specifically, the functions needed to interact with a remote system
 * are provided abstractly by RemoteFileSystem and concretely by its
 * subclass(es).
 *<P>
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public abstract class RemoteFileSystem extends GeneralFileSystem
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



//----------------------------------------------------------------------
// Setters and Getters
//----------------------------------------------------------------------
//Remote
	/**
	 * Returns the host used by the remote account using this RemoteFileSystem.
	 */
	public String getHost( )
		throws NullPointerException
	{
		return ((RemoteAccount) account).getHost( );
	}

	/**
	 * Returns the port used by this remote account using this
	 * RemoteFileSystem.
	 */
	public int getPort( )
		throws IllegalArgumentException
	{
		return ((RemoteAccount) account).getPort( );
	}


	/**
	 * Returns the user name used by this remote account using
	 * this RemoteFileSystem.
	 */
	public String getUserName( )
		throws NullPointerException
	{
		return ((RemoteAccount) account).getUserName( );
	}


//----------------------------------------------------------------------
// Methods
//----------------------------------------------------------------------

}

