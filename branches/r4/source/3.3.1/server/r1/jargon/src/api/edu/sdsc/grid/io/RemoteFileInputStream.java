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
//	    +-java.io.InputStream
//					|
//					+-edu.sdsc.grid.io.GeneralFileInputStream
//							|
//							+-edu.sdsc.grid.io.RemoteFileInputStream
//
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;

import java.io.*;

/**
 * A GeneralFileInputStream obtains input bytes from a file in a file system.
 * What files are available depends on the host environment.
 *<P>
 * GeneralFileInputStream is meant for reading streams of raw bytes such
 * as image data.
 *<P>
 * The original intention for this class was to subclass
 * java.io.FileInputStream.  But that is not currently the case.
 * <P>
 * @author  Lucas Gilbert
 * @since   JARGON1.4
 */
public abstract class RemoteFileInputStream extends GeneralFileInputStream
{
	/**
	 * Creates a <code>RemoteFileInputStream</code> by
	 * opening a connection to an actual file,
	 * the file named by the path name <code>name</code>
	 * in the file system.  A new <code>FileDescriptor</code>
	 * object is created to represent this file
	 * connection.
	 * <p>
	 * First, if there is a security
	 * manager, its <code>checkRead</code> method
	 * is called with the <code>name</code> argument
	 * as its argument.
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
	public RemoteFileInputStream( GeneralFileSystem fileSystem, String name )
		throws IOException
	{
		super(fileSystem, name);
	}

	/**
	 * Creates a <code>RemoteFileInputStream</code> by
	 * opening a connection to an actual file,
	 * the file named by the <code>File</code>
	 * object <code>file</code> in the file system.
	 * A new <code>FileDescriptor</code> object
	 * is created to represent this file connection.
	 * <p>
	 * First, if there is a security manager,
	 * its <code>checkRead</code> method  is called
	 * with the path represented by the <code>file</code>
	 * argument as its argument.
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
	public RemoteFileInputStream( GeneralFile file )
		throws IOException
	{
		super(file);
	}
}
