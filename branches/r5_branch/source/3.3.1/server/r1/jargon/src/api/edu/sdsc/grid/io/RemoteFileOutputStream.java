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
//	GeneralRandomAccessFile.java	-  edu.sdsc.grid.io.GeneralRandomAccessFile
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-java.io.OuputStream
//					|
//					+-edu.sdsc.grid.io.GeneralFileOutputStream
//							|
//							+-edu.sdsc.grid.io.RemoteFileOutputStream
//
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;



import java.io.*;


/**
 * A RemoteFileOutputStream writes bytes to a file in a file system.
 * What files are available depends on the host environment.
 *<P>
 * RemoteFileOutputStream is meant for writing streams of raw bytes such
 * as image data.
 *<P>
 * @author  Lucas Gilbert
 * @since   JARGON1.4
 */
public abstract class RemoteFileOutputStream extends GeneralFileOutputStream
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
	 * Creates a <code>FileOuputStream</code> by
	 * opening a connection to an actual file,
	 * the file named by the path name <code>name</code>
	 * in the file system.
	 * <p>
	 * First, the security is checked to verify the file can be written.
	 * <p>
	 * If the named file does not exist, is a directory rather than a regular
	 * file, or for some other reason cannot be opened for reading then a
	 * <code>IOException</code> is thrown.
	 *
	 * @param	name   the system-dependent file name.
	 * @exception  IOException  if the file does not exist,
	 *                   is a directory rather than a regular file,
	 *                   or for some other reason cannot be opened for
	 *                   reading.
	 */
	public RemoteFileOutputStream( GeneralFileSystem fileSystem, String name )
		throws IOException
	{
		super(fileSystem, name);
	}

	/**
	 * Creates a <code>FileInputStream</code> by
	 * opening a connection to an actual file,
	 * the file named by the <code>File</code>
	 * object <code>file</code> in the file system.
	 * A new <code>FileDescriptor</code> object
	 * is created to represent this file connection.
	 * <p>
	 * First, the security is checked to verify the file can be written.
	 * <p>
	 * If the named file does not exist, is a directory rather than a regular
	 * file, or for some other reason cannot be opened for reading then a
	 * <code>IOException</code> is thrown.
	 *
	 * @param	file   the file to be opened for reading.
	 * @exception  IOException  if the file does not exist,
	 *                   is a directory rather than a regular file,
	 *                   or for some other reason cannot be opened for
	 *                   reading.
	 * @see        java.io.File#getPath()
	 */
	public RemoteFileOutputStream( GeneralFile file )
		throws IOException
	{
		super(file);
	}






//----------------------------------------------------------------------
// Methods
//----------------------------------------------------------------------

}

