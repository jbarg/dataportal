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
//									|
//									+-edu.sdsc.grid.io.SRBFileOutputStream
//
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.srb;


import edu.sdsc.grid.io.*;

import java.io.*;


/**
 * A SRBFileOutputStream writes bytes to a file in a file system.
 * What files are available depends on the host environment.
 *<P>
 * SRBFileOutputStream is meant for writing streams of raw bytes such
 * as image data.
 *
 * @author  Lucas Gilbert
 * @since   JARGON1.4
 */
public class SRBFileOutputStream extends RemoteFileOutputStream
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------




//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * Holds the server connection used by this stream.
	 */
	protected SRBFileSystem fileSystem;



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
	public SRBFileOutputStream( SRBFileSystem fileSystem, String name )
		throws IOException
	{
		super(fileSystem, name);

		this.fileSystem = fileSystem;
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
	public SRBFileOutputStream( SRBFile file )
		throws IOException
	{
		super(file);
		fileSystem = (SRBFileSystem) file.getFileSystem();
	}




//----------------------------------------------------------------------
// Methods
//----------------------------------------------------------------------
	/**
	 * Opens the given file for use by this stream.
	 *
	 * @exception  IOException  if an I/O error occurs.
	 */
	protected void open( GeneralFile file )
		throws IOException
	{
		fd = ((SRBFileSystem) file.getFileSystem()).srbObjOpen(
			file.getName(), SRBRandomAccessFile.O_RDWR, file.getParent() );
	}


	/**
	 * Writes <code>len</code> bytes from the specified byte array
	 * starting at offset <code>off</code> to this file output stream.
	 *
	 * @param	b     the data.
	 * @param	off   the start offset in the data.
	 * @param	len   the number of bytes to write.
	 * @exception  IOException  if an I/O error occurs.
	 */
	public void write(byte b[], int off, int len)
		throws IOException
	{
		byte buffer[] = new byte[len];
		System.arraycopy( b, off, buffer, 0, len);
		fileSystem.srbObjWrite( fd, buffer, len );
	}


	/**
	 * Closes this file output stream and releases any system resources
	 * associated with this stream. This file output stream may no longer
	 * be used for writing bytes.
	 *
	 * <p> If this stream has an associated channel then the channel is closed
	 * as well.
	 *
	 * @exception  IOException  if an I/O error occurs.
	 */
	public void close()
		throws IOException
	{
		if (fileSystem != null) {
			fileSystem.srbObjClose( fd );
			fileSystem = null;
		}
	}

	/**
	 * Returns the file descriptor associated with this stream.
	 *
	 * @return  the <code>FileDescriptor</code> object that represents
	 *          the connection to the file in the file system being used
	 *          by this <code>FileOutputStream</code> object.
	 *
	 * @exception  IOException  if an I/O error occurs.
	 * @see        java.io.FileDescriptor
	 */
/*
	public final FileDescriptor getFD()  throws IOException {
		if (fd != null) return fd;
		throw new IOException();
	}
*/
	/**
	 * Returns the unique {@link java.nio.channels.FileChannel FileChannel}
	 * object associated with this file output stream. </p>
	 *
	 * <p> The initial {@link java.nio.channels.FileChannel#position()
	 * </code>position<code>} of the returned channel will be equal to the
	 * number of bytes written to the file so far unless this stream is in
	 * append mode, in which case it will be equal to the size of the file.
	 * Writing bytes to this stream will increment the channel's position
	 * accordingly.  Changing the channel's position, either explicitly or by
	 * writing, will change this stream's file position.
	 *
	 * @return  the file channel associated with this file output stream
	 */
/*
	public FileChannel getChannel() {
		synchronized (this) {
			if (channel == null)
				channel = FileChannelImpl.open(fd, false, true, this, append);
				return channel;
			}
	}
*/
}

