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
//	MetaDataTable.java	-  edu.sdsc.grid.io.MetaDataTable
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.MetaDataTable
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;


import java.util.Vector;

/**
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public class MetaDataTable
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------



//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	private Vector values;

	private Vector operators;



//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/*
   * Construct a metadata values table to be used as the conditional in
   * a metadata query.
   *<br>
   * values[0][0] operators[0] values[0][1] values[0][2] values[0][3]...<br>
   * values[1][0] operators[1] values[1][1] values[1][2] values[1][3]...<br>
   * values[2][0] operators[2] values[2][1] values[2][2] values[2][3]...<br>
   * ...<br>
   *<P>
   */
  /*
   * The standard SRB return might look like:<br>
   * fieldName operator value units
   * length = 5 m<br>
   * source = bitmap<br>
   * color = red rgb<br>
   *<P>
   * The operator only compares values[x][0] to values[x][1].
   * values[x][2+] are compared for equality. So the example would only
   * return items that are red using rgb units, but if the item had an alpha
   * channel, ie the units are different, then no items would match the query.
   *<P>
   *
   */
  public MetaDataTable( int[] operators, String[][] values )
  {
  	if ((values == null) || (operators == null)) {
  		throw new NullPointerException(
  			"Values and operators cannot be null.");
  	}

  	if (operators.length != values.length) {
  		throw new IllegalArgumentException(
  			"Values and operators must have equal length" );
  	}

  	if (values.length <= 0) {
//TODO have to create different method in SRBCommands
//to handle unlimited rows & 0-9 columns.
  		throw new IllegalArgumentException("No values were given." );
  	}

		this.values = new Vector( values.length );
		this.operators = new Vector( operators.length );
		for (int i=0;i<values.length;i++) {
			addRow(values[i], operators[i]);
		}
  }



//----------------------------------------------------------------------
// Methods
//----------------------------------------------------------------------
	/**
	 *
	 */
	public int[] getOperators()
	{
		int[] temp = new int[operators.size()];
		for (int i=0;i<temp.length;i++) {
			temp[i] = ((Integer) operators.get(i)).intValue();
		}
		return temp;
	}

	/**
	 *
	 */
	public int getOperator( int index )
	{
		return ((Integer) operators.get(index)).intValue();
	}

	/**
	 *
	 */
	public int getIntValue( int row, int column )
	{
		String[] temp = (String[]) values.get(row);
		String value = temp[column];

  	if ((row >= getRowCount()) || (column >= getColumnCount()))
	  	throw new IllegalArgumentException();
  	if ((row < 0) || (column < 0))
	  	throw new IllegalArgumentException();

		return Integer.parseInt(value.toString());
	}

	/**
	 *
	 */
	public float getFloatValue( int row, int column )
	{
		String[] temp = (String[]) values.get(row);
		String value = temp[column];

  	if ((row >= getRowCount()) || (column >= getColumnCount()))
	  	throw new IllegalArgumentException();
  	if ((row < 0) || (column < 0))
	  	throw new IllegalArgumentException();

		return Float.parseFloat(value.toString());
	}

	/**
	 *
	 */
	public String getStringValue( int row, int column )
	{
		String[] temp = (String[]) values.get(row);
		String value = temp[column];

  	if ((row >= getRowCount()) || (column >= getColumnCount()))
	  	throw new IllegalArgumentException();
  	if ((row < 0) || (column < 0))
	  	throw new IllegalArgumentException();

		return value;
	}


	/**
	 *
	 */
	public int getRowCount()
	{
		return values.size();
	}

	/**
	 *
	 */
	public int getColumnCount()
	{
		return ((String[]) values.get(0)).length;
	}


	/**
	 * Sets the operator. Must be done after the value is set as
	 * it does some conversions for LIKE.
	 */
	private void setOperator( int operator, String[] values )
	{
		if ((operator < 0) || (operator > 13)) {
			throw new IllegalArgumentException( "Invalid operator." );
		}

		if (( operator == MetaDataCondition.LIKE ) ||
			( operator == MetaDataCondition.NOT_LIKE ))
		{
			//same as set operator in MetaDataConditon,
			//but value[1] is the changed value.
			String temp = values[1].toString();
			int index = temp.indexOf( "*" );

			while (index >= 0) {
				temp = temp.substring( 0, index ) + "%" + temp.substring( index+1 );
				index = temp.indexOf( "*" );
			}

			index = temp.indexOf( "?" );
			while (index >= 0) {
				temp = temp.substring( 0, index ) + "_" + temp.substring( index+1 );
				index = temp.indexOf( "?" );
			}

			//No wildcards used. add % otherwise it would just be the EQUAL operator.
			if ((temp.indexOf( "%" ) < 0) && (temp.indexOf( "_" ) < 0)) {
				temp = "%" + temp + "%";
			}

			values[1] = temp;
		}

		this.operators.add(new Integer(operator));
	}

	/**
	 *
	 */
	protected void addRow( String[] values, int operator )
	{
		if (values != null) {
			setOperator(operator, values);
			this.values.add(values);
		}
	}


	/**
	 *
	 */
	protected void removeRow( int index )
	{
		values.remove(index);
		operators.remove(index);
	}

//----------------------------------------------------------------------
//  Object methods
//----------------------------------------------------------------------
	public String toString()
	{
		String value = "";

		for (int i=0;i<getRowCount();i++) {
			value += "\n";
			for (int j=0;j<getColumnCount();j++) {
				value += getStringValue( i, j )+"\t";
			}
		}

		return value;
	}


	/**
	 *
	 */
	public boolean equals( Object obj )
	{
  	MetaDataTable table = null;

  	if (obj == null)
  		return false;

  	try {
  		table = (MetaDataTable) obj;
  	} catch (ClassCastException e) {
  		return false;
  	}



  	if ((table.getRowCount() == getRowCount()) &&
  		(table.getColumnCount() == getColumnCount())) {
  		for (int i=0;i<getRowCount();i++) {
				for (int j=0;j<getColumnCount();j++) {
					if (!getStringValue(i,j).equals(table.getStringValue(i,j))) {
						return false;
					}
				}
  		}
			return true;
  	}
		return false;
	}
}
