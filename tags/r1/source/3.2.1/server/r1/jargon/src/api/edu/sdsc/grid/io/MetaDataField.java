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
//	MetaDataField.java	-  edu.sdsc.grid.io.MetaDataField
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.MetaDataField
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;



/**
 * A "meta data field" is an attribute of a file. It could be the
 * name of the file, it's creation date, it's owner, or any of the
 * other types of metadata maintained by the SRB or whomever.
 *<P>
 * Metadata groups contain a list of metadata fields. Each field is
 * described by a MetaDataField object. The object may be queried
 * to get a description of the field, the data types it supports, etc.
 *<P>
 * There are no 'set' methods. Once constructed, the object cannot
 * be changed.
 *<P>
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public final class MetaDataField
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
	/**
	 * The value type for the field.
	 * Types include: INT, LONG, FLOAT, STRING, DATE, TABLE.
	 */
	public static final int INT = 0;

	/**
	 * The value type for the field.
	 * Types include: INT, LONG, FLOAT, STRING, DATE, TABLE.
	 */
	public static final int LONG = 1;

	/**
	 * The value type for the field.
	 * Types include: INT, LONG, FLOAT, STRING, DATE, TABLE.
	 */
	public static final int FLOAT = 2;

	/**
	 * The value type for the field.
	 * Types include: INT, LONG, FLOAT, STRING, DATE, TABLE.
	 */
	public static final int STRING = 3;

	/**
	 * The value type for the field.
	 * Types include: INT, LONG, FLOAT, STRING, DATE, TABLE.
	 */
	public static final int DATE = 4;

	/**
	 * The value type for the field.
	 * Types include: INT, LONG, FLOAT, STRING, DATE, TABLE.
	 */
	public static final int TABLE = MetaDataCondition.TABLE;





//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 *
	 */
	String fieldName;


	/**
	 *
	 */
	String description;


	/**
	 *
	 */
	int type;


	/**
	 *
	 */
	Protocol[] protocols = new Protocol[1];




//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Constructs a field description.
   * If a type argument is given, the field is marked as
   * int, float, string, date or table, whichever is indicated.
   * If an int range pair is given, the field's data type
   * is int. If the range pair are floats, the data type
   * is set to float. If an enum is given, the data type
   * is always String.
	 */
//TODO comments
  public MetaDataField( String fieldName, String description, int type,
  	Protocol protocol )
  {
		this.fieldName = fieldName;
		this.description = description;
		this.type = type;
		protocols[0] = protocol;
  }




//----------------------------------------------------------------------
// Getters
//----------------------------------------------------------------------
	/**
   * Returns the name of the field.
   */
	public String getName( )
	{
		return fieldName;
	}


	/**
	 * The description string may be displayed by a GUI. It has no
	 * embedded carriage returns, so the application is
	 * expected to insert line breaks to wrap the text
   * appropriately for its way of displaying it.
   *
   * @return A description string of the field.
   */
  public String getDescription( )
	{
		return description;
	}



	/**
   * Returns the data type code for the field. Data types
   * include: int, float, string, date, filepath, etc.
   */
  public int getType( )
	{
		return type;
	}


	/**
   * Returns the protocols for this field.
   */
  Protocol[] getProtocols( )
	{
		return protocols;
	}

	/**
	 * Returns the protocol for this field at the specified index.
   */
	Protocol getProtocol( int index )
	{
		return protocols[index];
	}

	/**
   * Test if this field uses the given protocol.
   */
  public boolean usesProtocol( Protocol protocol )
	{
		for (int i=0; i<protocols.length; i++ ) {
			if ( protocols[i].equals( protocol ) ) {
				return true;
			}
		}
		return false;
	}




	/**
   * Returns the protocol for this field.
   */
  public void addProtocol( Protocol protocol )
	{
		Protocol[] temp = protocols;
		protocols = new Protocol[temp.length+1];
		for (int i=0;i<temp.length;i++) {
			protocols[i] = temp[i];
		}
//TODO what is the problem here?		System.arraycopy( temp, 0 protocols, 0, temp.length );

		protocols[temp.length] = protocol;
	}




	/**
	 * Tests this MetaDataField object for equality with the given object.
	 * Returns <code>true</code> if and only if the argument is not
	 * <code>null</code> and both are MetaDataField objects with equal
	 * name and description values;
	 *
	 * @param   obj   The object to be compared with this abstract pathname
	 *
	 * @return  <code>true</code> if and only if the objects are the same;
	 *          <code>false</code> otherwise
	 */
  public boolean equals( Object obj )
  {
  	MetaDataField field = null;

  	if (obj == null)
  		return false;

  	try {
  		field = (MetaDataField) obj;
  	} catch (ClassCastException e) {
  		return false;
  	}


  	if (fieldName == field.getName()) {
  		if (description == field.getDescription()) {
  			if (type == field.getType()) {
  				return true;
  			}
  		}
  	}
  	return false;
  }



	/**
	 * Returns a string representation of the object.
	 */
  public String toString()
  {
  	return new String( description );
  }
}
