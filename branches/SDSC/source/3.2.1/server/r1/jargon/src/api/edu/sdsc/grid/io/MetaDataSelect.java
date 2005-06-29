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
//	MetaDataSelect .java	-  edu.sdsc.grid.io.MetaDataSelect
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.MetaDataSelect
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;



/**
 * A "meta data select" indicates a single field that should be returned
 * on a query.
 *<P>
 * There are no 'set' methods - once constructed, the object cannot
 * be changed.
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public final class MetaDataSelect
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------



//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * The field to be selected by this object
	 */
	private MetaDataField field;

	/**
	 * Not yet implemented.
	 */
	private int operation = 1; //TODO


//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * The constructor is package private and is only called by the
	 * MetaDataGroup classes on a call to their factory methods to create
	 * selection objects. Those factory methods take a field name and
	 * check that it is legal before they construct a selection object.
	 */
	MetaDataSelect( MetaDataField field )
	{
		if (field == null)
			throw new NullPointerException("field cannot be null.");

		this.field = field;
	}

	/**
   * Constructs a metadata selection object containing
   * the given field name, and the chosen operation.
   * Operations include:
   *<ul>
   *<li>  count or count-distinct
   *<li>  max or min
   *<li>  avg or sum
   *<li>  variance or stddev
   *</ul>
   */
  MetaDataSelect( MetaDataField field, int operation )
	{
		if (field == null)
			throw new NullPointerException("field cannot be null.");

		this.field = field;
		this.operation = operation;
	}



//----------------------------------------------------------------------
// Getters
//----------------------------------------------------------------------
	/**
	 * Returns the name of the field selected by this object.
   */
  public String getFieldName( )
	{
		return field.getName();
	}


	/**
	 * Returns the MetaDataField object describing
   * the field selected by this object.
   */
  public MetaDataField getField( )
  {
		return field;
  }



	/**
   * Returns the operation code on this selection.
   * Typically this is '1' and just flags this field as
   * one to return in a query. Other operation codes
   * include those for:
   *<ul>
   *<li>  count or count-distinct
   *<li>  max or min
   *<li>  avg or sum
   *<li>  variance or stddev
   *</ul>
	 */
  public int getOperation( )
  {
		return operation;
  }

	/**
	 * Returns a string representation of the object.
	 */
  public String toString()
  {
 		return new String( field.getName() );
  }



	/**
	 * Tests this MetaDataSelect object for equality with the given object.
	 * Returns <code>true</code> if and only if the argument is not
	 * <code>null</code> and both are MetaDataSelect objects with equal
	 * field and operation values.
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

			MetaDataSelect temp = (MetaDataSelect) obj;

			if (getField().equals(temp.getField())) {
				if (getOperation() == temp.getOperation()) {
					return true;
				}
			}
		} catch (ClassCastException e) {
			return false;
		}
		return false;
	}
}
