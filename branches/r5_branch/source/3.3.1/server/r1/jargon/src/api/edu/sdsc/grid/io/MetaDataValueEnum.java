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
//	MetaDataValueEnum.java	-  edu.sdsc.grid.io.MetaDataValueEnum
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.MetaDataValueEnum
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;



/**
 * Some meta data fields have a specific list of values that they
 * allow. A "meta data value enum" describes one such value.
 *<P>
 * There are no 'set' methods. Once constructed, the object cannot
 * be changed.
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public final class MetaDataValueEnum
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------



//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	private String value;

	private String description;



//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
   * Construct a a value enum.
   * The constructor is package private and is only called by
   * MetaDataGroup classes when they set up their descriptions of
   * themselves.
   */
  MetaDataValueEnum( String value, String description )
  {
		this.value = value;
		this.description = description;
  }


//----------------------------------------------------------------------
// Getters
//----------------------------------------------------------------------
	/**
	 * This is the string that is usable, verbatum, as
	 * a value for the field when building conditionals.
	 * It is also one of the expected values that may be
   * returned by a query containing this field.
   *
	 * @return The value string for this enum item.
	 */
  public String getValue( )
	{
		return value;
	}



	/**
	 * This string may be displayed by a GUI. It has no
   * embedded carriage returns, so the application is
   * expected to insert line breaks to wrap the text
   * appropriately for its way of displaying it.
   *
   * @return A description string for this enum value.
   */
  public String getDescription( )
	{
		return description;
	}
}

