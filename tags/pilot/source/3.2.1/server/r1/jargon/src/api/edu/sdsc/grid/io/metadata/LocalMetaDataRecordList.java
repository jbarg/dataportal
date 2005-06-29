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
//	LocalMetaDataRecordList.java	-
//			edu.sdsc.grid.io.local.LocalMetaDataRecordList
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.MetaDataRecordList
//	            |
//	            +-.LocalMetaDataRecordList
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.local;

import edu.sdsc.grid.io.*;


/**
 * LocalMetaDataRecordList does nothing. It exists for compatability.
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
/*
*/
class LocalMetaDataRecordList extends MetaDataRecordList
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------



//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------



//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Does nothing
	 */
	LocalMetaDataRecordList(  )
	{
		super( new MetaDataField[1], new Object[1]);
	}


//----------------------------------------------------------------------
//  Iterator methods
//----------------------------------------------------------------------
	/**
	 * @throws UnsupportedOperationException
	 */
	public boolean isQueryComplete()
	{
		throw new UnsupportedOperationException();
	}


	/**
	 * @throws UnsupportedOperationException
	 */
	public MetaDataRecordList[] getMoreResults()
	{
		throw new UnsupportedOperationException();
	}

	/**
	 * @throws UnsupportedOperationException
	 */
	public MetaDataRecordList[] getMoreResults( int numOfResults )
	{
		throw new UnsupportedOperationException();
	}


	/**
	 * @throws UnsupportedOperationException
	 */
	public MetaDataRecordList[] getAllResults()
	{
		throw new UnsupportedOperationException();
	}
}
