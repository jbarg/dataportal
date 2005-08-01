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
//	ProtocolCatalog.java	-  edu.sdsc.grid.io.ProtocolCatalog
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.ProtocolCatalog
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;

import edu.sdsc.grid.io.srb.*;

import java.util.Vector;


/**
 * The ProtocolCatalog stores all the metadata query protocols available
 * for querying.
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public class ProtocolCatalog
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------


//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	private static Vector protocols = new Vector();

//TODO yeah, not much here. It only exists because some tricky stuff
//was needed to get around certain 'static' problems.
	ProtocolCatalog()
	{
		super();
	}

	static {
		protocols.add( new SRBProtocol() );
		//protocols.add( new FTProtocol() );
	}
}
