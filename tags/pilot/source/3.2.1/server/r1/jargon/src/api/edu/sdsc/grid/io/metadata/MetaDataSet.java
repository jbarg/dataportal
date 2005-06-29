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
//	MetaDataSet.java	-  edu.sdsc.grid.io.MetaDataSet
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.MetaDataSet
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;



import java.util.HashMap;



/**
 * A "Meta data group" is a group of meta data fields. Each
 * MetaDataGroup class includes documentation and introspection
 * methods that describe the group as a whole, the list of fields
 * in the group, a description for those fields, the data types for
 * those fields, and the expected field values.
 *<P>
 * Each subclass has a constructor. None of the constructors are ever
 * public. Leaf classes in the class tree have private constructors,
 * while the rest are protected. The constructors are called by a
 * static initializer method for each class and are strictly used to
 * create a single object for the class that is passed to the
 * MetaDataGroup parent class to register it in a master list. That
 * object also should be saved by the child class and will be passed
 * to constructors for condition and select objects (so that generic
 * handlers of those objects can get back to the metadata group).
 *<P>
 * Each of the subclasses group together meta data for a specific
 * purpose. All of the SRB meta data groups are subclassed off of
 * SRBMetaData. The subclasses may add methods, but most will only
 * implement the required methods.
 *<P>
 * Some meta groups are standard regardless of implementation. To
 * handle standard metadata, we defined interfaces that are implemented
 * by protocol-specific metadata groups. The GeneralMetaData interface
 * contract is that the implementor supports meta data we might find
 * for any file system, such as:
 *<ul>
 * <li>  File name
 * <li>  File size
 * <li>  Creation date
 * <li>  Modification date
 * <li>  Owner name
 *</ul>
 *<P>
 * An implementation specific class, such as SRBGeneralMetaData must
 * support these fields, but also may support further general metadata
 * fields, such as:
 *<ul>
 * <li>  Collection name
 * <li>  Replication number
 * <li>  File type
 * <li>  Data classification name
 * <li>  Data classification type
 * <li>  Access constraint
 * <li>  Comments
 * <li>  Comments date
 * <li>  Deleted flag
 * <li>  Owner domain
 * <li>  Owner email address
 *</ul>
 *<br><BR><br><pre>
 * Meta data groups
 * ------------------------------------------------------------------------
 * Interfaces
 * ----------
 *  StandardMetaData
 *   |
 *   +-->GeneralMetaData
 *   |
 *   +-->DublinCoreMetaData
 *   |
 *   +-->UserMetaData
 *</pre>
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public abstract class MetaDataSet implements GeneralMetaData
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	private static HashMap metaDataGroups = new HashMap();
	private static HashMap metaDataFields = new HashMap();



//----------------------------------------------------------------------
//  Methods
//----------------------------------------------------------------------
	static {
		new ProtocolCatalog();
	}



	protected static void add( MetaDataGroup group )
	{
		metaDataGroups.put( group.getName(), group );
		MetaDataField[] fields = group.getFields();
		for (int i=0;i<fields.length;i++) {
			metaDataFields.put( fields[i].getName(), fields[i] );
		}
	}



	/**
	 * Return all the various metadata groups.
	 */
	public static MetaDataGroup[] getMetaDataGroups( )
	{
		return (MetaDataGroup[]) metaDataGroups.values().toArray(
			new MetaDataGroup[0]);
	}



	/**
   *
   */
/*
  static MetaDataGroup getGroup( String fieldName )
  {
		return metaDataFields.get( fieldName );
  }


  static MetaDataGroup getGroup( MetaDataField field )
  {
		return metaDataFields.get( field.getFieldName() );
  }
*/


	/**
	 * Returns the MetaDataField object associated with this fieldName string.
	 * If there is no match a null value is returned.
	 */
  public static MetaDataField getField( String fieldName )
  {
		if (fieldName == null)
			throw new NullPointerException("The fieldName cannot be null.");

  	MetaDataField field = (MetaDataField) metaDataFields.get( fieldName );
/*TODO throw exception?
		if (field == null)
			throw new NullPointerException(	"The fieldName, "+fieldName+
				", does not correspond to any registered fields." );
*/
		return field;
  }



//----------------------------------------------------------------------
// Create conditionals
//----------------------------------------------------------------------
//scalar
	/**
   * Returns a new condition object that requests that a
   * given field match/not match a given value.
   * Valid MetaDataCondition operator codes are:
   * =, <>, >, <, like, not like, sounds like.
   * If supported by the filesystem being queried.
   */
  public static MetaDataCondition newCondition(
  	String fieldName, int operator, String value )
  {
		return new MetaDataCondition( getField( fieldName ), operator, value );
  }


	/**
   * Returns a new condition object that requests that a
   * given field match/not match a given value.
   * Valid MetaDataCondition operator codes are:
   * =, <>, >, <, like, not like, sounds like.
   * If supported by the filesystem being queried.
   */
  public static MetaDataCondition newCondition(
  	String fieldName, int operator, float value )
  {
		return new MetaDataCondition( getField( fieldName ), operator, value );
  }


	/**
   * Returns a new condition object that requests that a
   * given field match/not match a given value.
   * Valid MetaDataCondition operator codes are:
   * =, <>, >, <, like, not like, sounds like.
   * If supported by the filesystem being queried.
   */
  public static MetaDataCondition newCondition(
  	String fieldName, int operator, int value )
  {
  	return new MetaDataCondition( getField( fieldName ), operator, value );
  }




//rangepair
	/**
   * Returns a new condition object that requests that a
   * given field match/not match a given value range.
   * Valid operator codes are: MetaDataCondition.BETWEEN
   * and MetaDataCondition.NOT_BETWEEN
   * If supported by the filesystem being queried.
   */
  public static MetaDataCondition newCondition(
  	String fieldName, int operator, int value1, int value2 )
  {
  	return new MetaDataCondition( getField( fieldName ),
  		operator, value1, value2 );
  }


	/**
   * Returns a new condition object that requests that a
   * given field match/not match a given value range.
   * Valid operator codes are: MetaDataCondition.BETWEEN
   * and MetaDataCondition.NOT_BETWEEN
   * If supported by the filesystem being queried.
   */
  public static MetaDataCondition newCondition(
  	String fieldName, int operator, float value1, float value2 )
  {
  	return new MetaDataCondition( getField( fieldName ),
  		operator, value1, value2 );
  }


	/**
   * Returns a new condition object that requests that a
   * given field match/not match a given value range.
   * Valid operator codes are: MetaDataCondition.BETWEEN
   * and MetaDataCondition.NOT_BETWEEN
   * If supported by the filesystem being queried.
   */
  public static MetaDataCondition newCondition(
  	String fieldName, int operator, String value1, String value2 )
  {
  	return new MetaDataCondition( getField( fieldName ),
  		operator, value1, value2 );
  }




//list
	/**
   * Returns a new condition object that requests that a
   * given field match/not match the given values.
   * Valid operator codes are: MetaDataCondition.IN
   * and MetaDataCondition.NOT_IN
   * If supported by the filesystem being queried.
   */
  public static MetaDataCondition newCondition(
  	String fieldName, int operator, int[] valueList )
  {
  	return new MetaDataCondition( getField( fieldName ),
  		operator, valueList );
  }


	/**
   * Returns a new condition object that requests that a
   * given field match/not match the given values.
   * Valid operator codes are: MetaDataCondition.IN
   * and MetaDataCondition.NOT_IN
   * If supported by the filesystem being queried.
   */
  public static MetaDataCondition newCondition(
  	String fieldName, int operator, float[] valueList )
  {
  	return new MetaDataCondition( getField( fieldName ),
  		operator, valueList );
  }


	/**
	 * Returns a new condition object that requests that a
	 * given field match/not match the given values.
	 * Valid operator codes are: MetaDataCondition.IN
	 * and MetaDataCondition.NOT_IN
   * If supported by the filesystem being queried.
	 */
	public static MetaDataCondition newCondition(
		String fieldName, int operator, String[] valueList )
	{
		return new MetaDataCondition( getField( fieldName ),
			operator, valueList );
  }



//table
	/**
	 * Returns a new condition object that requests that a
	 * given field match the given values as described by the MetaDataTable.
   * If supported by the filesystem being queried.
	 */
  public static MetaDataCondition newCondition(
  	String fieldName, MetaDataTable table )
  {
  	return new MetaDataCondition( getField( fieldName ), table );
  }




	/**
	 * The mergeConditons( ... ) methods are conveniences to creates
	 * a single MetaDataCondition array from two (or more) conditions.
	 */
	public static MetaDataCondition[] mergeConditions(
		MetaDataCondition condition1, MetaDataCondition condition2 )
	{
		MetaDataCondition[] condition = { condition1, condition2 };
		return condition;
	}


	/**
	 * The mergeConditons( ... ) methods are conveniences to creates
	 * a single MetaDataCondition array from two (or more) conditions.
	 */
	public static MetaDataCondition[] mergeConditions(
		MetaDataCondition condition1, MetaDataCondition[] condition2 )
	{
		MetaDataCondition[] condition =
			new MetaDataCondition[condition2.length+1];
		condition[0] = condition1;
		System.arraycopy(condition2, 0, condition, 1, condition2.length );
		return condition;
	}


	/**
	 * The mergeConditons( ... ) methods are conveniences to creates
	 * a single MetaDataCondition array from two (or more) conditions.
	 */
	public static MetaDataCondition[] mergeConditions(
		MetaDataCondition[] condition1, MetaDataCondition[] condition2 )
	{
		MetaDataCondition[] condition = new
			MetaDataCondition[condition1.length+condition2.length+1];
		System.arraycopy(condition1, 0, condition, 0, condition1.length );
		System.arraycopy(
			condition2, 0, condition, condition1.length, condition2.length );
		return condition;
	}



//----------------------------------------------------------------------
// Create selects
//----------------------------------------------------------------------
	/**
   * Returns a new selection object that requests that
   * records returned by a query include this field.
   */
  public static MetaDataSelect newSelection( String fieldName )
  {
  	return new MetaDataSelect( getField( fieldName ) );
  }


	/**
	 * Returns a new array of selection objects that request that
	 * all fields in this group be included in returned
	 * records from a query.
   */
  public static MetaDataSelect[] newSelection( String[] fieldNames )
  {
  	MetaDataSelect[] selects = new MetaDataSelect[fieldNames.length];
  	for (int i=0;i<selects.length;i++) {
	  	selects[i] = new MetaDataSelect( getField( fieldNames[i] ) );
	  }
	  return selects;
  }


	/**
   * Returns a new selection object that requests a
   * specific operation be performed on query results
   * involving this field. Operations include:
   *<UL>
   * <LI>  count or count-distinct
   * <LI>  max or min
   * <LI>  avg or sum
   * <LI>  variance or stddev
   *</UL>
   */
  public static MetaDataSelect newSelection( String fieldName, int operation )
  {
  	return new MetaDataSelect( getField( fieldName ), operation);
  }



	/**
	 * Returns a new array of selection objects that request that
	 * all fields in this group be included in returned
	 * records from a query.
   */
  public static MetaDataSelect[] newSelection( MetaDataGroup group )
  {
  	MetaDataSelect[] selects = new MetaDataSelect[group.getFieldCount()];
  	MetaDataField[] fieldList = group.getFields();
  	for (int i=0;i<selects.length;i++) {
	  	selects[i] = new MetaDataSelect( fieldList[i] );
	  }
	  return selects;
  }


	/**
	 * The mergeSelects( ... ) methods are conveniences to creates
	 * a single MetaDataSelect array from two (or more) selects.
	 */
	public static MetaDataSelect[] mergeSelects(
		MetaDataSelect select1, MetaDataSelect select2 )
	{
		MetaDataSelect[] select = { select1, select2 };
		return select;
	}


	/**
	 * The mergeSelects( ... ) methods are conveniences to creates
	 * a single MetaDataSelect array from two (or more) selects.
	 */
	public static MetaDataSelect[] mergeSelects(
		MetaDataSelect select1, MetaDataSelect[] select2 )
	{
		MetaDataSelect[] select = new MetaDataSelect[select2.length+1];
		select[0] = select1;
		System.arraycopy(select2, 0, select, 1, select2.length );
		return select;
	}


	/**
	 * The mergeSelects( ... ) methods are conveniences to creates
	 * a single MetaDataSelect array from two (or more) selects.
	 */
	public static MetaDataSelect[] mergeSelects(
		MetaDataSelect[] select1, MetaDataSelect[] select2 )
	{
		MetaDataSelect[] select = new
			MetaDataSelect[select1.length+select2.length+1];
		System.arraycopy(select1, 0, select, 0, select1.length );
		System.arraycopy(
			select2, 0, select, select1.length, select2.length );
		return select;
	}

}
