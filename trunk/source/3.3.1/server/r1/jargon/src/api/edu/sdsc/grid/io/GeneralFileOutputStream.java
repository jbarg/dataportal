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
//
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;



import java.io.*;


/**
 * A GeneralFileOutputStream writes bytes to a file in a file system.
 * What files are available depends on the host environment.
 *<P>
 * GeneralFileOutputStream is meant for writing streams of raw bytes such
 * as image data.
 *<P>
 * The original intention for this class was to subclass
 * java.io.FileOuputStream.  But that is not currently the case.
 * <P>
 * @author  Lucas Gilbert
 * @since   JARGON1.4
 */
public abstract class GeneralFileOutputStream extends OutputStream //extends FileOuputStream
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------




//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * Holds the file descriptor information used by this stream.
	 */
	protected int fd = -1;



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
	 * @param      name   the system-dependent file name.
	 * @exception  IOException  if the file does not exist,
	 *                   is a directory rather than a regular file,
	 *                   or for some other reason cannot be opened for
	 *                   reading.
	 */
	public GeneralFileOutputStream( GeneralFileSystem fileSystem, String name )
		throws IOException
	{
		this( FileFactory.newFile(fileSystem, name) );
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
	 * @param      file   the file to be opened for reading.
	 * @exception  IOException  if the file does not exist,
	 *                   is a directory rather than a regular file,
	 *                   or for some other reason cannot be opened for
	 *                   reading.
	 * @see        java.io.File#getPath()
	 */
	public GeneralFileOutputStream( GeneralFile file )
		throws IOException
	{
		if (file == null) {
			throw new NullPointerException("File cannot be null.");
		}

		if (!file.exists())
			file.createNewFile();

		open( file );
	}



	/**
	 * Ensures that the <code>close</code> method of this file input stream is
	 * called when there are no more references to it.
	 *
	 * @exception  IOException  if an I/O error occurs.
	 * @see        edu.sdsc.grid.io.GeneralFileOutputStream#close()
	 */
	protected void finalize()
		throws IOException
	{
		/*TODO flush doesn't do anything, because there is no buffer
		if (bytes left) {
			flush();
		}
		*/
		close();
	}



//----------------------------------------------------------------------
// Methods
//----------------------------------------------------------------------
	/**
	 * Opens the given file for use by this stream.
	 *
	 * @param	file the file to be opened.
	 * @exception  IOException  if an I/O error occurs.
	 */
	protected abstract void open( GeneralFile file ) throws IOException;


	/**
	 * Writes the specified byte to this file output stream. Implements
	 * the <code>write</code> method of <code>OutputStream</code>.
	 *
	 * @param	b   the byte to be written.
	 * @exception  IOException  if an I/O error occurs.
	 */
	public void write( int b )
		throws IOException
	{
		byte buffer[] = { (byte) b };

		write(buffer, 0, buffer.length);
	}

	/**
	 * Writes <code>b.length</code> bytes from the specified byte array
	 * to this file output stream.
	 *
	 * @param	b   the data.
	 * @exception  IOException  if an I/O error occurs.
	 */
	public void write( byte b[] )
		throws IOException
	{
		write(b, 0, b.length);
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
	public abstract void write( byte b[], int off, int len ) throws IOException;


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
	public abstract void close() throws IOException;

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
	public final FileDescriptor getFD()
		throws IOException
	{
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
	 *
	 * @since 1.4
	 * @spec JSR-51
	 */
/*
	public FileChannel getChannel()
	{
		synchronized (this) {
			if (channel == null)
				channel = FileChannelImpl.open(fd, false, true, this, append);
				return channel;
			}
	}
*/
}

