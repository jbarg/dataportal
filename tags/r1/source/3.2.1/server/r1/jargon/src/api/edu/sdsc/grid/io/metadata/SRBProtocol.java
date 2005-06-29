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
//	SRBProtocol.java	-  edu.sdsc.grid.io.srb.SRBProtocol
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.Protocol
//			    |
//					+-.SRBProtocol
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.srb;

import edu.sdsc.grid.io.Protocol;


/**
 *
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public class SRBProtocol extends Protocol
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
	final static String SRB_PROTOCOL_NAME = "SRBProtocol";
	final static String SRB_PROTOCOL_HELP = "";

//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------



//----------------------------------------------------------------------
//  Methods
//----------------------------------------------------------------------
	/**
	 *
	 */
	public SRBProtocol( )
	{
		super( SRB_PROTOCOL_NAME, SRB_PROTOCOL_HELP );
		metaDataSet = new SRBMetaDataSet(this);
	}


	/**
	 * If and only if the object is an instance of SRBProtocol.
	 */
	public boolean equals( Object obj ) {
  	if (obj != null) {
			if (obj instanceof SRBProtocol) {
				return true;
			}
		}

		return false;
	}

}
