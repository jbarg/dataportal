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
//	SRBMetaDataRecordList.java	-  edu.sdsc.grid.io.srb.SRBMetaDataRecordList
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.MetaDataRecordList
//	            |
//	            +-.SRBMetaDataRecordList
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.srb;

import edu.sdsc.grid.io.*;

import java.io.IOException;

/**
 * A "meta data record list" is a list of results from a metadata query.
 * Each entry in the list is one record, corresponding to one file found
 * by the query. Information in that record includes field values for
 * that file.
 *<P>
 * Results of long queries will only return a partial list
 * to save on bandwidth which can be iterated through by further calls to
 * the server.
 *<P>
 * Subclasses are expected to be private inner classes within the
 * various file server implementations. Those private inner classes
 * work closely with the file server to do a multi-step query that does
 * not have to return everything immediately. The SRBMetaDataRecordList
 * class, for instance, works with partial query results and, on need,
 * issues a query for the next batch of results. If some of the results
 * are never asked for, they are never retreived. To the caller, some
 * requests for a record are immediate while others pause while the
 * partial query is sent.
 *<P>
 * The subclasses are private, but their constructors are public to
 * the file server class. So, the SRBFileSystem can call the constructor
 * of an SRBMetaDataRecordList class, but neither the class or the
 * constructor are visible to the API user.
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public class SRBMetaDataRecordList extends MetaDataRecordList
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------





//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * If the query returned a partial list, this value is used to obtain
	 * more records from the server.
	 */
//TODO static for now. and thusly only the last query can getMoreRows
	private int continuationIndex = -1;

//TODO should be SRBFileSystem? is that possible?
//TODO static see above
	private SRBCommands fileSystem;



//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Load the SRBMetaDataRecordList internal field list
	 * with the selectFields passed to the constructor.
	 *<P>
	 * Then copy the queryReturn into records[][].
	 * The order has to be switched to match the order of fields[],
	 * which is the same order as the selectArray initially sent.
	 */
	SRBMetaDataRecordList( MetaDataField[] fields, Object[] recordValues,
		int continuationIndex, SRBCommands fileSystem )
	{
		super(fields, recordValues);

		this.continuationIndex = continuationIndex;

//TODO SRBCommands
this.fileSystem = fileSystem;
	}

	/**
	 *
	 */
	public SRBMetaDataRecordList( MetaDataField field, int recordValue )
	{
		super(field, recordValue);
	}

	/**
	 *
	 */
	public SRBMetaDataRecordList( MetaDataField field, float recordValue )
	{
		super(field, recordValue);
	}

	/**
	 *
	 */
	public SRBMetaDataRecordList( MetaDataField field, String recordValue )
	{
		super(field, recordValue);
	}

	/**
	 *
	 */
	public SRBMetaDataRecordList( MetaDataField field, MetaDataTable recordValue )
	{
		super(field, recordValue);
	}


	/**
	 * Finalizes the object by explicitly letting go of each of
	 * its internally held values.
	 */
	protected void finalize( )
	{
//TODO will this close the connection? need .clone()?
//		if (fileSystem != null)
//			fileSystem == null;
	}



//----------------------------------------------------------------------
//  Methods
//----------------------------------------------------------------------
	/**
	 * Used by SRBCommands during a query return.
	 * if all the fields that this and rl have in common
	 * have matching values, then this.addRecord( recordList )
	 * The values in this SRBMetaDataRecordList will be overwritten by
	 * <code>recordList</code>. (Though in the case of SRBCommands the
	 * values will always be equal.)
	 */
	boolean combineRecordLists( MetaDataRecordList recordList )
	{
		if (recordList != null) {
			SRBMetaDataRecordList rl = (SRBMetaDataRecordList) recordList;

//System.out.println("combineRecordLists0 getFieldCount() "+getFieldCount());

			for (int i=0;i<fields.length;i++) {
				for (int j=0;j<rl.getFieldCount();j++) {
					if (fields[i].equals(rl.getField(j))) {
						if ((records[i] == null) || (rl.getValue(j) == null)) {
							if (records[i] != rl.getValue(j)) {
								return false; //TODO??
							}
						}
						else if (!records[i].equals(rl.getValue(j))) {
							//both RecordLists have the same field but with different values
							return false;//TODO exception?
						}
					}
				}
			}

//System.out.println("combineRecordLists1 "+rl.getFieldCount());

				for (int j=0;j<rl.getFieldCount();j++) {
						addRecord( rl.getField(j), rl.getValue(j) );
				}
//System.out.println("combineRecordLists2 "+rl.getFieldCount());

			return true;
		}
		else {
			return false;
		}
	}


	void setContinuationIndex( int contIndex )
	{
		continuationIndex = contIndex;
	}

	int getContinuationIndex( )
	{
		return continuationIndex;
	}



	/**
	 * Tests if this SRBMetaDataRecordList can return more values from the
	 * query.
	 */
//TODO better name, reverse true and false?
	public boolean isQueryComplete()
	{
		if (continuationIndex >= 0) {
			return false;
		}

		return true;
	}


	/**
	 * Gets further results from the query. If and only if the
	 * query returned a partial list and there are more results
	 * which matched the query that haven't been returned. Otherwise null.
	 * By default a query will only return 300 values at a time which match the
	 * query, see also <code>SRBFileSystem.DEFAULT_RECORDS_WANTED</code>.
	 */
	public MetaDataRecordList[] getMoreResults()
		throws IOException
	{
		return getMoreResults(SRBFileSystem.DEFAULT_RECORDS_WANTED);
	}


	/**
	 * Gets further results from the query. If and only if the
	 * query returned a partial list and there are more results
	 * which matched the query that haven't been returned. Otherwise null.
	 */
	public MetaDataRecordList[] getMoreResults( int numOfResults )
		throws IOException
	{
		if (continuationIndex < 0 ) {
			return null;
		}

		return fileSystem.srbGetMoreRows(
				SRBFile.MDAS_CATALOG, continuationIndex, numOfResults );
	}
}
