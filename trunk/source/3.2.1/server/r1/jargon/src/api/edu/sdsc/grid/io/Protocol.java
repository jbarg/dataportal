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
//	Protocol.java	-  edu.sdsc.grid.io.Protocol
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.Protocol
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;




/**
 *
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public abstract class Protocol
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	protected String name;

	protected String help;

	protected MetaDataSet metaDataSet;


//----------------------------------------------------------------------
//  Methods
//----------------------------------------------------------------------
	protected Protocol( String name, String help )
	{
		this.name = name;
		this.help = help;
	}


	/**
	 *
	 */
	public String getName()
	{
		return name;
	}


	/**
	 *
	 */
	public String getHelp()
	{
		return help;
	}


	/**
	 *
	 */
	public MetaDataSet getMetaDataSet()
	{
		return metaDataSet;
	}


	/**
	 * Factory
	 */
/*
	public static Protocol newProtocol()
	{
		return null; //TODO
	}
*/

	/**
	 * The standard java equals(Object), but it is abstract to insure
	 * all subclasses will implement it.
	 */
	public abstract boolean equals( Object obj );



	public String toString( )
	{
		return name;
	}
}
