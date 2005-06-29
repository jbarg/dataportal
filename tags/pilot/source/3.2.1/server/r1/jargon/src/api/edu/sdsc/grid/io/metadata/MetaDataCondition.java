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
//	MetaDataCondition.java	-  edu.sdsc.grid.io.MetaDataCondition
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.MetaDataCondition
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;




/**
 * A "meta data condition" describes a conditional expression to
 * be used to guide a metadata query. That expression contains
 * three components:
 *<ul>
 * <li>  A field name
 * <li>  An operator (such as "=")
 * <li>  A value
 *</ul>
 *<P>
 * Legal field names depend upon the meta data group and the file
 * server being talked to.
 *<P>
 * Operators are in the set:
 *<ul>
 * <li>  = <>
 * <li>  < >
 * <li>  <= >=
 * <li>  in not in
 * <li>  between not between
 * <li>  like not like
 * <li>  sounds like sounds not like
 *</ul>
 *<P>
 * Each operator is designated by an "operator code", which is a
 * static final integer defined in this class.
 *<P>
 * A value has a "style" in the set:
 *<ul>
 * <li>  A scalar value (such as "42" or "Thursday")
 * <li>  A scalar range (such as "42-118")
 * <li>  A value list (such as "42,38,52")
 *</UL>
 * There are no 'set' methods - once constructed, the object cannot
 * be changed.
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public final class MetaDataCondition
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
  /**
   * "="  where the metadata value exactly equals the conditional value.
   */
  public static final int EQUAL = 0;

  /**
   * "!="  where the metadata value is not exactly equal to the conditional value.
   */
  public static final int NOT_EQUAL = 1;

  /**
   * "<"  where the metadata value is less than the conditional value,
   * lexicographical compare for strings.
   */
  public static final int LESS_THAN = 2;

  /**
   * ">"  where the metadata value is greater than the conditional value,
   * lexicographical compare for strings.
   */
  public static final int GREATER_THAN = 3;

  /**
   * "<="  where the metadata value is less than or equal to the conditional
   * value, lexicographical compare for strings.
   */
  public static final int LESS_OR_EQUAL = 4;

  /**
   * ">"  where the metadata value is greater than or equal to the conditional
   * value, lexicographical compare for strings.
   */
  public static final int GREATER_OR_EQUAL = 5;

  /**
   * "in"  where the metadata value exactly equals one of the conditional
   * values in the value list.
   */
  public static final int IN = 6;

  /**
   * "not in"  where the metadata value does not exactly equals
   * one of the conditional values in the value list.
   */
  public static final int NOT_IN = 7;

  /**
   * "between"  where the metadata value is between the two conditional values,
   * lexicographical compare for strings.
   */
  public static final int BETWEEN = 8;

  /**
   * "not between"  where the metadata value is not between the two conditional
   * values, lexicographical compare for strings.
   */
  public static final int NOT_BETWEEN = 9;

  /**
   * "like"  where the metadata value contains the conditional value.
   * Values using LIKE should be strings with these wild-card characters,
   * * and ?. * represents any number of characters. ? represents any
   * single character. For example, <br>
   * if you have the files with OWNER = testuser, testuser2:<br>
   * The query "OWNER LIKE testuse_" will return testuser.<br>
   * The query "OWNER LIKE testuse%" will return testuser, and testuser2.<br>
   * Conditionals without any wildcard characters will search as a string
   * fragment, ie. will be treated as though a % was at the beginning and end.
   *<P>
   * * is the same as %, and ? equals _<br>
   * note: The characters *,?,%,_ are reserved and may not be used in
   * the conditional value with the LIKE operator.
   */
  public static final int LIKE = 10;

  /**
   * "not like"  where the metadata value does not contains the conditional value.
   * Values using LIKE should be strings with these wild-card characters,
   * * and ?. * represents any number of characters. ? represents any
   * single character. For example, <br>
   * if you have the files with OWNER = testuser, testuser2:<br>
   * The query "OWNER LIKE testuse?" will return testuser.<br>
   * The query "OWNER LIKE testuse*" will return testuser, and testuser2.<br>
   * Conditionals without any wildcard characters will search as a string
   * fragment, ie. will be treated as though a * was at the beginning and end.
   *<P>
   * * is the same as %, and ? equals _<br>
   * note: The characters *,?,%,_ are reserved and may not be used in
   * the conditional value with the LIKE operator.
   */
  public static final int NOT_LIKE = 11;

  /**
   * sounds like, Implement phonetic name searches
   */
  public static final int SOUNDS_LIKE = 12;

  /**
   * not sounds like, Implement phonetic name searches
   */
  public static final int SOUNDS_NOT_LIKE = 13;



	/**
	 * The value style for the field.<br>
	 * Styles include: SCALAR, RANGEPAIR, ENUM, TABLE.
	 */
	public static final int SCALAR = 0;

	/**
	 * The value style for the field.<br>
	 * Styles include: SCALAR, RANGEPAIR, ENUM, TABLE.
	 */
	public static final int RANGEPAIR = 1;

	/**
	 * The value style for the field.<br>
	 * Styles include: SCALAR, RANGEPAIR, ENUM, TABLE.
	 */
	public static final int ENUM = 2;

	/**
	 * Table represents both a style and type. It can only be used with
	 * conditionals that use the MetaDataTable as a value.<br>
	 * Styles include: SCALAR, RANGEPAIR, ENUM, TABLE.<br>
	 * Types include: INT, LONG, FLOAT, STRING, DATE, TABLE.
	 */
	public static final int TABLE = 14;



//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * Legal fields depend upon the file server being talked to.
	 */
	MetaDataField field;



	/**
	 * Operators are in the set:
	 *<ul>
	 * <li>  = <>
	 * <li>  < >
	 * <li>  <= >=
	 * <li>  in not in
	 * <li>  between not between
	 * <li>  like not like
	 * <li>  sounds like sounds not like
	 *</ul>
	 *<P>
	 * Each operator is designated by an "operator code", which is a
	 * static final integer defined in this class.
	 */
	//MetaDataTables have multiple operators,
	//which are contained in the MetaDataTable object.
	int operator;


	/**
	 * Holds the values which define the consequent of the conditional.
	 */
	Object[] values;

	/**
	 * The value style for the conditional.<br>
	 * Styles include: SCALAR, RANGEPAIR, ENUM, TABLE.
	 */
	int style;

	/**
	 * The value type for the field.<br>
	 * Types include: INT, LONG, FLOAT, STRING, DATE, TABLE.
	 */
//don't do type here, but for error check on field
	int type;


//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
//SCALAR
	/**
	 * The constructor is package private and is only called by the
	 * MetaDataGroup classes and their factory methods. Those factory
	 * methods do argument checking first to make sure the field name is
	 * part of the group, the data type of the value is appropriate, and
	 * that the operator is valid for this field. Only if all the arguments
	 * are right is the object constructed and returned.
	 *<P>
	 * Construct a scalar conditional with the given field
	 * name, operator, and value. Infer the type from the
	 * given value. The operator must be one of the
	 * simple operators (=, <>, <, <=, >, >=) or one of
	 * the 'like' operators (like, not like, sounds like,
	 * sounds not like).
	 */
  MetaDataCondition(
  	MetaDataField field, int operator, int value )
  {
		if (field == null)
			throw new NullPointerException("field cannot be null.");

		this.field = field;
		values = new Object[1];
		values[0] = new Integer( value );
		type = MetaDataField.INT;
		style = SCALAR;
		setOperator( operator );
  }


	/**
	 * The constructor is package private and is only called by the
	 * MetaDataGroup classes and their factory methods. Those factory
	 * methods do argument checking first to make sure the field name is
	 * part of the group, the data type of the value is appropriate, and
	 * that the operator is valid for this field. Only if all the arguments
	 * are right is the object constructed and returned.
	 *<P>
	 * Construct a scalar conditional with the given field
	 * name, operator, and value. Infer the type from the
	 * given value. The operator must be one of the
	 * simple operators (=, <>, <, <=, >, >=) or one of
	 * the 'like' operators (like, not like, sounds like,
	 * sounds not like).
	 */
  MetaDataCondition(
  	MetaDataField field, int operator, float value )
  {
		if (field == null)
			throw new NullPointerException("field cannot be null.");

		this.field = field;
		values = new Object[1];
		values[0] = new Float( value );
		type = MetaDataField.FLOAT;
		style = SCALAR;
		setOperator( operator );
  }


	/**
	 * The constructor is package private and is only called by the
	 * MetaDataGroup classes and their factory methods. Those factory
	 * methods do argument checking first to make sure the field name is
	 * part of the group, the data type of the value is appropriate, and
	 * that the operator is valid for this field. Only if all the arguments
	 * are right is the object constructed and returned.
	 *<P>
	 * Construct a scalar conditional with the given field
	 * name, operator, and value. Infer the type from the
	 * given value. The operator must be one of the
	 * simple operators (=, <>, <, <=, >, >=) or one of
	 * the 'like' operators (like, not like, sounds like,
	 * sounds not like).
	 */
  MetaDataCondition(
  	MetaDataField field, int operator, String value )
  {
		if (field == null)
			throw new NullPointerException("field cannot be null.");

		this.field = field;
		values = new Object[1];
		values[0] = value;
		type = MetaDataField.STRING;
		style = SCALAR;
		setOperator( operator );
  }




//RANGEPAIR
	/**
	 * Construct a rangepair conditional with the given field
	 * name, operator, and pair of values. Infer the type
	 * from the given values. The operator must be
	 * 'between' or 'not between' or an exception is thrown.
	 */
  MetaDataCondition( MetaDataField field, int operator,
  	int value1, int value2 )
  {
		if (field == null)
			throw new NullPointerException("field cannot be null.");

		this.field = field;
		values = new Object[2];
		values[0] = new Integer( value1 );
		values[1] = new Integer( value2 );
		type = MetaDataField.INT;
		style = RANGEPAIR;
		if ((operator == BETWEEN) || (operator == NOT_BETWEEN)) {
			this.operator = operator;
		}
		else {
			throw new IllegalArgumentException( "Wrong operator." );
		}
  }


	/**
	 * Construct a rangepair conditional with the given field
	 * name, operator, and pair of values. Infer the type
	 * from the given values. The operator must be
	 * 'between' or 'not between' or an exception is thrown.
	 */
  MetaDataCondition( MetaDataField field, int operator,
  	float value1, float value2 )
  {
		if (field == null)
			throw new NullPointerException("field cannot be null.");

		this.field = field;
		values = new Object[2];
		values[0] = new Float( value1 );
		values[1] = new Float( value2 );
		type = MetaDataField.FLOAT;
		style = RANGEPAIR;
		if ((operator == BETWEEN) || (operator == NOT_BETWEEN)) {
			this.operator = operator;
		}
		else {
			throw new IllegalArgumentException( "Wrong operator." );
		}
  }


	/**
	 * Construct a rangepair conditional with the given field
	 * name, operator, and pair of values. Infer the type
	 * from the given values. The operator must be
	 * 'between' or 'not between' or an exception is thrown.
	 */
  MetaDataCondition( MetaDataField field, int operator,
  	String value1, String value2 )
  {
		if (field == null)
			throw new NullPointerException("field cannot be null.");

		this.field = field;
		values = new Object[2];
		values[0] = value1;
		values[1] = value2;
		type = MetaDataField.STRING;
		style = RANGEPAIR;
		if ((operator == BETWEEN) || (operator == NOT_BETWEEN)) {
			this.operator = operator;
		}
		else {
			throw new IllegalArgumentException( "Wrong operator." );
		}
  }




//ENUM
	/**
	 * Construct a enum conditional with the given field
	 * name, operator, and enum of values. The type is inferred
	 * from the given values. The operator must be
	 * 'in' or 'not in' or an exception is thrown.
	 */
  MetaDataCondition(
  	MetaDataField field, int operator, int[] values )
  {
		if (field == null)
			throw new NullPointerException("field cannot be null.");

		this.field = field;
		this.values = new Object[values.length];
		for (int i=0;i<values.length;i++) {
			this.values[i] = new Integer( values[i] );
		}
		type = MetaDataField.INT;
		style = ENUM;
		if ((operator == IN) || (operator == NOT_IN)) {
			this.operator = operator;
		}
		else {
			throw new IllegalArgumentException( "Wrong operator." );
		}
  }


	/**
	 * Construct a enum conditional with the given field
	 * name, operator, and enum of values. The type is inferred
	 * from the given values. The operator must be
	 * 'in' or 'not in' or an exception is thrown.
	 */
  MetaDataCondition(
  	MetaDataField field, int operator, float[] values )
  {
		if (field == null)
			throw new NullPointerException("field cannot be null.");

		this.field = field;
		this.values = new Object[values.length];
		for (int i=0;i<values.length;i++) {
			this.values[i] = new Float( values[i] );
		}
		type = MetaDataField.FLOAT;
		style = ENUM;
		if ((operator == IN) || (operator == NOT_IN)) {
			this.operator = operator;
		}
		else {
			throw new IllegalArgumentException( "Wrong operator." );
		}
  }


	/**
	 * Construct a enum conditional with the given field
	 * name, operator, and enum of values. The type is inferred
	 * from the given values. The operator must be
	 * 'in' or 'not in' or an exception is thrown.
	 */
  MetaDataCondition(
  	MetaDataField field, int operator, String[] values )
  {
		if (field == null)
			throw new NullPointerException("field cannot be null.");

		this.field = field;
		this.values = new Object[values.length];
		this.values = values;
		type = MetaDataField.STRING;
		style = ENUM;
		if ((operator == IN) || (operator == NOT_IN)) {
			this.operator = operator;
		}
  }




//TABLE
	/**
	 * Construct a table conditional with the given field
	 * names and MetaDataTable.
	 */
  MetaDataCondition( MetaDataField field, MetaDataTable table )
  {
		if (field == null)
			throw new NullPointerException("field cannot be null.");

		this.field = field;
		values = new Object[1];
		values[0] = table;
		type = TABLE;
		style = TABLE;
		//operators are in the table
		operator = TABLE;
  }





	/**
	 * Finalizes the object by explicitly letting go of each of
	 * its internally held values.
	 */
	protected void finalize( )
	{
		if (field != null) {
			field = null;
		}
		if (values != null) {
			values = null;
		}
	}



//----------------------------------------------------------------------
// Setters & Getters
//----------------------------------------------------------------------
	/**
	 * Sets the operator. Must be done after the value is set as
	 * it does some conversions for LIKE.
	 */
	private void setOperator( int operator )
	{
		if ((operator < 0) || (operator > 13)) {
			throw new IllegalArgumentException( "Invalid operator." );
		}

		if (( operator == LIKE ) || ( operator == NOT_LIKE )){
			String temp = values[0].toString();
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

			values[0] = temp;
		}

		this.operator = operator;
	}


	/**
	 * Returns the MetaDataGroup's field object describing
	 * the field in the condition.
	 */
	public MetaDataField getField( )
	{
		return field;
	}


	/**
	 * Returns the field name in the condition. This is
	 * a shorthand for:<br>
	 * getField().getName();
	 */
	public String getFieldName( )
	{
		return field.getName();
	}


	/**
	 * Returns the MetaDataGroup's field data type. This is
	 * a shorthand for:<br>
	 * getField().getType();
	 */
	public int getFieldType( )
	{
		return field.getType();
	}


	/**
	 * Returns the operator code for the 'rangepair', 'list' and 'scalar'
	 *  style conditionals.
	 */
	public int getOperator( )
	{
		return operator;
	}


	/**
	 * Returns the value style for the conditional.
	 * Styles include: scalar, rangepair, list.
	 */
	public int getStyle( )
	{
		return style;
	}


	/**
	 * Returns the length of the value list, if the style
	 * is an enum. Returns 2 for rangepairs, and 1 otherwise (there is always
	 * at least one value).
	 */
	public int getCount( )
	{
		return values.length;
	}




	/**
	 * For a 'rangepair' style conditional, there are two
	 * values at indexes 0,0 and 0,1. These methods return those
	 * values, or throw an IllegalArgumentException if the index is out of
	 * range.
	 *<P>
	 * For a 'list' style conditional, there is a list of
	 * values at indexes 0,0 to 0,n-1, where n is returned by
	 * getCount(). These methods return those values, or
	 * throw an IllegalArgumentException when the index is out of range.
	 *<P>
	 * For a 'scalar' style conditional, there is only one
	 * value at index 0,0. These methods return this value,
	 * or throw an IllegalArgumentException if the index is not 0,0.
	 */
	public int getIntValue( int index )
	{
		if ((index < values.length) && (index >= 0)) {
			if (style == TABLE)
			{
				throw new UnsupportedOperationException();
			}
			else {
				if (type == MetaDataField.INT) {
					return ((Integer) values[index] ).intValue();
				}
				else if (type == MetaDataField.FLOAT) {
					return ((Float) values[index] ).intValue();
				}
				else if (type == MetaDataField.STRING) {
					return new Integer( values[index].toString() ).intValue();
				}
			}
		}

		throw new IllegalArgumentException();
	}


	/**
	 * For a 'rangepair' style conditional, there are two
	 * values at indexes 0,0 and 0,1. These methods return those
	 * values, or throw an exception if the index is out of
	 * range.
	 *<P>
	 * For a 'list' style conditional, there is a list of
	 * values at indexes 0,0 to 0,n-1, where n is returned by
	 * getCount(). These methods return those values, or
	 * throw an exception when the index is out of range.
	 *<P>
	 * For a 'scalar' style conditional, there is only one
	 * value at index 0,0. These methods return this value,
	 * or throw an exception if the index is not 0,0.
	 */
	public float getFloatValue( int index )
	{
		if ((index < values.length) && (index >= 0)) {
			if (style == TABLE)
			{
				throw new UnsupportedOperationException();
			}
			else {
				if (type == MetaDataField.INT) {
					return ((Integer) values[index] ).floatValue();
				}
				else if (type == MetaDataField.FLOAT) {
					return ((Float) values[index] ).floatValue();
				}
				else if (type == MetaDataField.STRING) {
					return new Float( values[index].toString() ).floatValue();
				}
			}
		}

		throw new IllegalArgumentException();
	}


	/**
	 * For a 'rangepair' style conditional, there are two
	 * values at indexes 0 and 1. These methods return those
	 * values, or throw an exception if the index is out of
	 * range.
	 *<P>
	 * For a 'list' style conditional, there is a list of
	 * values at indexes 0 to n-1, where n is returned by
	 * getCount(). These methods return those values, or
	 * throw an exception when the index is out of range.
	 *<P>
	 * For a 'scalar' style conditional, there is only one
	 * value at index 0. These methods return this value,
	 * or throw an exception if the index is not 0.
	 */
	public String getStringValue( int index )
	{
		if ((index < values.length) && (index >= 0)) {
			if (style == TABLE)
			{
				throw new UnsupportedOperationException();
			}
			else {
				if (type == MetaDataField.INT) {
					return ((Integer) values[index] ).toString();
				}
				else if (type == MetaDataField.FLOAT) {
					return ((Float) values[index] ).toString();
				}
				else if (type == MetaDataField.STRING) {
					return values[index].toString();
				}
			}
		}

		throw new IllegalArgumentException();
	}
//No rangepair or enum for tables


	/**
	 * These methods are the same as calling the above
	 * list methods, but with an index of 0.
	 */
	public int getIntValue( )
	{
			return getIntValue(0);
	}


	/**
	 * These methods are the same as calling the above
	 * list methods, but with an index of 0.
	 */
	public float getFloatValue( )
	{
			return getFloatValue(0);
	}


	/**
	 * These methods are the same as calling the above
	 * list methods, but with an index of 0.
	 */
	public String getStringValue( )
	{
		return getStringValue(0);
	}


	/**
	 * Returns the table stored in this conditional.
	 * A 'table' conditional is both a type and a style.
	 * So there is only one value at index 0.
	 * This methods return that value.
	 */
	public MetaDataTable getTableValue( )
	{
		if (style != TABLE)
		{
			throw new UnsupportedOperationException();
		}
		else
			return (MetaDataTable) values[0];
	}





	/**
	 * Returns a string representation of the object.
	 */
  public String toString()
  {
  	String toString = new String( field.getName() );
//TODO include operator
  	for (int i=0;i<values.length;i++) {
			toString += " "+values[i];
  	}
  	return toString;
  }





	/**
	 * Tests this MetaDataCondition object for equality with the given object.
	 * Returns <code>true</code> if and only if the argument is not
	 * <code>null</code> and both are MetaDataCondition objects with equal
	 * field, operators and values.
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

			MetaDataCondition temp = (MetaDataCondition) obj;

			if (getField().equals(temp.getField())) {
				if (getOperator() == temp.getOperator()) {
					for (int i=0;i<values.length;i++) {
						if (!values[i].equals(temp.values[i])) {
							return false;
						}
					}
					return true;
				}
			}
		} catch (ClassCastException e) {
			return false;
		}
		return false;
	}
}
