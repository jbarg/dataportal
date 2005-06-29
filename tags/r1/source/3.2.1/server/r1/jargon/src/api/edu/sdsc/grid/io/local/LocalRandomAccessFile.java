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
//	LocalRandomAccessFile.java	-  edu.sdsc.grid.io.local.LocalRandomAccessFile
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.GeneralRandomAccessFile
//					|
//			    +-.LocalRandomAccessFile
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.local;

import edu.sdsc.grid.io.*;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.File;
import java.io.RandomAccessFile;

//TODO comments, all
/**
 * A wrapper class for java.io.RandomAccessFile.
 *
 * @see java.io.RandomAccessFile
 * @see edu.sdsc.grid.io.GeneralRandomAccessFile
 * @author  Lucas Gilbert
 * @since   GridTools1.0
 */

public class LocalRandomAccessFile extends GeneralRandomAccessFile
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
	private RandomAccessFile wrapper;



//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	public LocalRandomAccessFile(String name, String mode)
		throws IOException
	{
		this( new LocalFile( name ), mode );
	}

	public LocalRandomAccessFile(File file, String mode)
		throws IOException
	{
		this( new LocalFile( file ), mode );
	}


	public LocalRandomAccessFile(LocalFile file, String mode)
		throws IOException
	{
		super( file, mode );
		wrapper = new RandomAccessFile(file.getAbsolutePath(), mode);
	}


//----------------------------------------------------------------------
//  Setters and Getters
//----------------------------------------------------------------------
	public RandomAccessFile getRandomAccessFile() throws IOException
	{
		return wrapper;
	}

	protected void open( GeneralFile file ) throws FileNotFoundException
	{
		//Don't do anything, use wrapper.
	}



//----------------------------------------------------------------------
//	Read methods
//----------------------------------------------------------------------
	public int read() throws IOException
	{
		return wrapper.read();
	}

	//Private method in wrapper, so call public.
	protected int readBytes(byte b[], int offset, int len) throws IOException
	{
		return wrapper.read(b, offset, len);
	}

	public int skipBytes(int n) throws IOException
	{
		return wrapper.skipBytes(n);
	}



//----------------------------------------------------------------------
//  Write methods
//----------------------------------------------------------------------
	public void write(int b) throws IOException
	{
		wrapper.write(b);
	}

	//Private method in wrapper, so call public.
	protected void writeBytes(byte b[], int offset, int len) throws IOException
	{
		wrapper.write(b, offset, len);
	}



//----------------------------------------------------------------------
//  Random access methods
//----------------------------------------------------------------------
	public long getFilePointer() throws IOException
	{
		return wrapper.getFilePointer();
	}


	public void seek(long position) throws IOException
	{
		wrapper.seek(position);
	}


	public long length() throws IOException
	{
		return wrapper.length();
	}


	public void setLength(long newLength) throws IOException
	{
		wrapper.setLength(newLength);
	}


	public void close() throws IOException
	{
		wrapper.close();
	}
}
