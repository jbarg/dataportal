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
//	RemoteRandomAccessFile.java	-  edu.sdsc.grid.io.RemoteRandomAccessFile
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.GeneralRandomAccessFile
//			    |
//			    +-.RemoteRandomAccessFile
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;

import java.io.IOException;

/**
 * An abstract class which extends GeneralRandomAccessFile to create
 * remote random access files.
 *
 * @author  Lucas Gilbert
 * @since   JARGON1.0
 */
public abstract class RemoteRandomAccessFile extends GeneralRandomAccessFile
{
//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------



//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Doesn't really do much, perhaps in the future.
	 * Instead @see GeneralRandomAccessFile
	 * or the subclass which called this constructor.
	 *
	 * @throws  IOException  if the file is a directory
	 *                   rather than a regular file, or cannot be opened or
	 *                   created for any other reason
	 */
	protected RemoteRandomAccessFile( RemoteFile file, String mode )
		throws IOException
	{
		super( file, mode );
	}


//----------------------------------------------------------------------
// Setters and Getters
//----------------------------------------------------------------------

}
