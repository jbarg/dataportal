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
//	MetaDataGroup.java	-  edu.sdsc.grid.io.MetaDataGroup
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.MetaDataGroup
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;



import java.util.HashMap;
import java.util.Vector;

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
 * contract is that the implementor supports metadata we might find
 * for any file system, such as:
 *<ul>
 * <li>  File name
 * <li>  File size
 * <li>  Creation date
 * <li>  Modification date
 * <li>  Owner name
 *</ul>
 *<P>
 * An implementation specific group, such as the general SRB metadata group
 * must support these fields, but also may support further general metadata
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
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public class MetaDataGroup
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	protected HashMap fields = new HashMap();

	protected String groupName;

	protected String description;



//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
   * Construct a metadata group and register the group
   * in a master list of groups maintained by the
   * MetaDataSet class.
   */
//TODO I still don't like this part
/*   During registration,
   * the group's fields are checked to insure they are
   * unique and do not collide with any field names of
   * any previously registered groups.
	 */
  public MetaDataGroup( String groupName, String description )
  {
  	super();


		this.groupName = groupName;
		this.description = groupName; //TODO real descriptions
  }


//----------------------------------------------------------------------
// Build object
//----------------------------------------------------------------------
	/**
	 * Load the group's fields.
	 */
	public void add( MetaDataField field )
	{
		String fieldName = field.getName();
		if (isField( fieldName )) {
			MetaDataField oldField = getField( fieldName );
			oldField.addProtocol( field.getProtocol(0) );
		}
		else {
			fields.put( fieldName, field );
		}
	}




//----------------------------------------------------------------------
// Introspection
//----------------------------------------------------------------------
	/**
   * Returns a short name string for this metadata group.
	 */
  public String getName( )
  {
		return groupName;
  }


	/**
   * This string may be displayed by a GUI. It has no
   * embedded carriage returns, so the application is
   * expected to insert line breaks to wrap the text
   * appropriately for its way of displaying it.
   *
   * @return A description string of the meta data group.
   */
  public String getDescription( )
  {
		return description;
  }


	/**
   * Returns the number of fields known by the group.
	 */
	public int getFieldCount( )
	{
		return fields.size();
	}


	/**
   * Returns an object describing this field in the group.
   */
/*
  public MetaDataField getField( int fieldNum )
  {
//TODO		return something;
  }
*/



	/**
   * Returns an object describing this field in the group.
   */
  public MetaDataField getField( String fieldName )
  {
		return (MetaDataField) fields.get(fieldName);
  }


	/**
   * Returns all the fields in the group.
   */
  public MetaDataField[] getFields( )
  {
		return (MetaDataField[]) fields.values().toArray(new MetaDataField[0]);
  }


//----------------------------------------------------------------------
// Reverse mapping
//----------------------------------------------------------------------
	/**
   * Returns the index of the field in the 0 to n list of
   * fields for the meta data group. A -1 is returned if
   * the field is not recognized.
   */
/*TODO what index?
  public int getFieldIndex( String fieldName )
  {
		return 0;//TODO
  }
*/

	/**
   * Returns true if the given field name is part of
   * the group. False is returned otherwise.
   */
  boolean isField( String fieldName )
  {
		if ( fields.get( fieldName ) != null) {
			return true;
		}

		return false;
  }




	/**
	 * Returns a string representation of the object.
	 */
	public String toString()
	{
	  	return getDescription();
  }
}
