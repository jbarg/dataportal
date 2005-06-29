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
//	RemoteAccount.java	-  edu.sdsc.grid.io.RemoteAccount
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.GeneralAccount
//						|
//						+-.RemoteAccount
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;


/**
 * An object to hold the user information used when connecting
 * to a remote file system.
 *<P>
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public abstract class RemoteAccount extends GeneralAccount
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * The host to connect to on the server.
	 */
	protected String host;

	/**
	 * The port to connect to on the server.
	 */
	protected int port;

	/**
	 * The user name to send to the server.
	 */
	protected String userName;

	/**
	 * Currently, just the text password.
	 */
	protected String password;





//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Constructs an object to hold the user information used when connecting
	 * to a remote server.
	 * <P>
	 * @param	host the remote system domain name
	 * @param	port the port on the remote system
	 * @param	userName the user name
	 * @param	password the password
	 * @param	homeDirectory home directory on the remote system
	 */
	public RemoteAccount( String host, int port, String userName, String password,
		String homeDir )
	{
		super( homeDir );
		setHost( host );
		setPort( port );
		setUserName( userName );
		setPassword( password );
	}


	/**
	 * Finalizes the object by explicitly letting go of each of
	 * its internally held values.
	 * <P>
	 */
	protected void finalize( )
	{
		super.finalize();
		host = null;
		userName = null;
		password = null;
	}



//----------------------------------------------------------------------
// Setters and Getters
//----------------------------------------------------------------------
	/**
	 * Sets the host of this RemoteAccount.
	 *
	 * @throws	NullPointerException	if host is null.
	 */
	public void setHost( String host )
	{
		if ( host == null )
			throw new NullPointerException( "The host string cannot be null");

		this.host = host;
	}

	/**
	 * Sets the port of this RemoteAccount. Port numbers can not be negative.
	 */
	public void setPort( int port )
	{
		if (port >= 0)
			this.port = port;
		else {
			throw new IllegalArgumentException("Invalid port number");
		}
	}

	/**
	 * Sets the userName of this RemoteAccount.
	 */
	public void setUserName( String userName )
	{
		this.userName = userName;
	}

	/**
	 * Sets the client password.
	 */
	public void setPassword( String password )
	{
		this.password = password;
	}

	/**
	 * Returns the host used by this RemoteAccount.
	 */
	public String getHost( )
	{
		return host;
	}

	/**
	 * Returns the port used by this RemoteAccount.
	 */
	public int getPort( )
		throws IllegalArgumentException
	{
		return port;
	}


	/**
	 * Returns the userName used by this RemoteAccount.
	 */
	public String getUserName( )
	{
		return userName;
	}

	/**
	 * Returns the password used by this RemoteAccount.
	 */
	public String getPassword( )
	{
		return password;
	}
}

