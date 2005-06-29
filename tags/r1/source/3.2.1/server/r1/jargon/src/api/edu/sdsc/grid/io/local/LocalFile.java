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
//	LocalFile.java	-  edu.sdsc.grid.io.local.LocalFile
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.GeneralFile
//	            |
//	            +-.LocalFile
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.local;

import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.srb.SRBFile; //used in copyTo()

import java.io.IOException;
import java.io.FilenameFilter;
import java.io.FileFilter;
import java.io.File;
import java.net.URI;
import java.net.URL;
import java.net.MalformedURLException;
import java.util.Vector;
import java.util.Iterator;
import java.util.Properties;

/**
 * The LocalFile class is a wrapper class for the
 * java.io.File class. Use it as you would a regular File object.
 * The localFile.getFile() method will return a java.io.File object
 * for those situations where a true java.io.File is required.
 *<P>
 * LocalFile includes dummy methods for the local file's metadata.
 *<P>
 * @see java.io.File
 * @see	edu.sdsc.grid.io.GeneralFile
 */
public class LocalFile extends GeneralFile
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------

	/**
	 * Standard SRB path separator character.
	 * represented as a string for convenience.
	 */
	private final static String PATH_SEPARATOR = System.getProperty( "file.separator" );

	/**
	 * The system-dependent path-separator character.
	 */
	private final static char PATH_SEPARATOR_CHAR = PATH_SEPARATOR.charAt(0);

//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * Internal file object to provide LocalFile with the functionality
	 * of java.io.File
	 */
	private	File wrapper;


//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Creates a new LocalFile instance by converting the given pathname string
	 * into an abstract pathname. If the given string is the empty string,
	 * then the result is the empty abstract pathname.
	 *
	 * @throws NullPointerException - If the pathname argument is null
	 */
	public LocalFile( String filePath )
	{
		this( filePath, "" );
	}

	/**
	 * Creates a new LocalFile instance from a parent pathname string and
	 * a child pathname string.
	 *
	 * @throws NullPointerException - If child is null
	 */
	public LocalFile( String directory, String child )
	{
		//doesn't really do anything.
		super( new LocalFileSystem(), directory, child );

		wrapper = new File( directory, child );
	}

	/**
	 * Creates a new LocalFile instance from a parent abstract pathname and
	 * a child pathname string.
	 *
	 * @throws NullPointerException - If child is null
	 */
	public LocalFile( LocalFile directory, String child )
	{
		this( directory.getAbsolutePath(), child );

		wrapper = new File( directory.getAbsolutePath(), child );
	}

	/**
	 * Creates a new File instance from a file object.
	 *
	 * @throws NullPointerException - If file is null
	 */
	public LocalFile( File file )
	{
		this( file.getAbsolutePath(), "" );

		wrapper = file;
	}

	/**
	 * Creates a new File instance from a file object.
	 *
	 * @throws NullPointerException - If child is null
	 */
	public LocalFile( File file, String child )
	{
		this( file.getAbsolutePath(), child );

		wrapper = new File( file, child );
	}

	/**
	 * Creates a new LocalFile instance by converting the given file:
	 * URI into an abstract pathname. The exact form of a file:
	 * URI is system-dependent, hence the transformation performed
	 * by this constructor is also system-dependent.
	 *
	 * @throws NullPointerException - If uri is null
	 * @throws IllegalArgumentException -
	 *			If the preconditions on the parameter do not hold
	 */
	public LocalFile( URI uri )
	{
		this( "", "" ); //Is this right?

		wrapper = new File( uri );
	}



	/**
	 * Finalizes the object by explicitly letting go of each of
	 * its internally held values.
	 * <P>
	 */
	protected void finalize( )
		throws Throwable
	{
		super.finalize();
		wrapper = null;
	}



//----------------------------------------------------------------------
// GeneralFile Methods
//----------------------------------------------------------------------
	/**
	 * Iterates through the directory/collection/container list.
	 *
	 * @return Iterator
	 */
	public Iterator listIterator( )
		throws IOException
	{
//TODO what does this return?
return null;
	}

//TODO localFile specific comments
	/**
	 * Queries the file server to find all files that
   * match a set of conditions. For all those that
   * match, the fields indicated in the select array
   * are returned in the result object.
	 *<P>
   * While condition and select array objects have all
   * been checked for self-consistency during their
   * construction, there are additional problems that
   * must be detected at query time:
	 *<P>
   * <ul>
   *	<li>Redundant selection fields
   *	<li>Redundant query fields
   *	<li>Fields incompatible with a file server
   * </ul>
	 *<P>
   * For instance, it is possible to build a condition
   * object appropriate for the SRB, then pass that object
   * in a local file system query. That will find that
   * the condition is incompatible and generate a mismatch
   * exception.
	 *<P>
   * Query is implemented by the file-server-specific
   * classes, like that for the SRB, FTP, etc. Those
   * classes must re-map condition and select field names
   * and operator codes to those required by a particular
   * file server and protocol version. Once re-mapped,
   * they issue the query and get results. The results
   * are then mapped back to the standard public field
   * names of the MetaDataGroups. So, if a MetaDataGroup
   * uses a name like "file path", but the SRB calls it
   * "data name", then query maps first from "file path"
   * to "data name" before issuing the query, and then
   * from "data name" back to "file path" within the
   * results. The programmer using this API should never
   * see the internal field names.
	 */
/*TODO not needed?
  public MetaDataRecordList[] query(
  	MetaDataCondition[] conditionArray, MetaDataSelect[] selectArray )
  {
  	return null;//TODO
  }
*/


//----------------------------------------------------------------------
// Methods
//----------------------------------------------------------------------
	/**
	 * Set the directory.
	 */
	protected void setDirectory( String dir )
	{
		//do nothing
		//for get use wrapper.getParent() instead
	}



	/**
	 * Set the file name.
	 */
	protected void setFileName( String fileName )
	{
		//do nothing
		//for get use wrapper.getName() instead
	}


	/**
	 * Returns the internal file object.
	 */
	public File getFile( )
	{
		return wrapper;
	}


	/**
	 * Dummy method for local file metadata.
	 */
	public Properties getMetaData( )
	{
		return null;
	}


	/**
	 * This method gets the path separator as defined by the local system.
	 */
	public String getPathSeparator( )
	{
		return PATH_SEPARATOR;
	}

	/**
	 * This method gets the path separator char as defined by the local system.
	 */
	public char getPathSeparatorChar( )
	{
		return PATH_SEPARATOR_CHAR;
	}

	/**
	 * Gets the home directory to be used with this LocalFile object.
	 *
	 * @throws	NullPointerException	if fileSystem is null.
	 * @return SRBFileSystem
	 */
	protected String getHomeDirectory( )
	{
		return System.getProperty( "user.home" );
	}




//----------------------------------------------------------------------
// GeneralFile Methods
//----------------------------------------------------------------------
	/**
	 * Copies this file to another file.
	 * If the destination file does not exist, a new one will be created.
	 * Otherwise the source file will be appended to the destination file.
	 * Directories will be copied recursively.
	 *
	 * @param file	The file to receive the data.
	 * @throws  NullPointerException If file is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyTo( GeneralFile file )
		throws IOException
	{
		if ( file instanceof SRBFile ) {
			//This allows optimization by the use of the parallel up/download
			file.copyFrom( this );
		}
		else {
			super.copyTo( file );
		}
	}


	/**
	 * Copies this file to another file.
	 * If the destination file does not exist, a new one will be created.
	 * Otherwise the source file will be appended to the destination file.
	 * Directories will be copied recursively.
	 *
	 * @param file	The file to receive the data.
	 * @throws  NullPointerException If file is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyFrom( GeneralFile file )
		throws IOException
	{
		/*
		I didn't really want to have a .copyFrom(...), but
		without the copyFrom, new packages which don't have access to
		the LocalFile source can't be optimized for upload. So for example,
		they wouldn't have a way to copy a local file to the SRB using the
		parallel method. Of course localFile.copyTo( newGridFile ) will still
		be slow, but newGridFile.copyFrom( localFile ) can be a work-around.
		*/
		if ( file instanceof SRBFile ) {
			//This allows optimization by the use of the parallel up/download
			file.copyTo( this );
		}
		else {
			super.copyFrom( file );
		}
	}


//----------------------------------------------------------------------
// java.io.File Methods
//----------------------------------------------------------------------
	/**
	 * Tests whether the application can read the file denoted by
	 * this abstract pathname.
	 */
	public boolean canRead()
	{
		return wrapper.canRead();
	}


	/**
	 * Tests whether the application can modify to the file denoted by
	 * this abstract pathname.
	 */
	public boolean canWrite()
	{
		return wrapper.canWrite();
	}


	/**
	 *	Compares two abstract pathnames lexicographically.
	 */
	public int compareTo(GeneralFile pathName)
	{
		//throw ClassCastException if not a LocalFile
		return wrapper.compareTo(((LocalFile) pathName).getFile());
	}


	/**
	 *	Compares this abstract pathname to another object.
	 */
	public int compareTo(Object o)
	{
		return wrapper.compareTo(o);
	}


	/**
	 *	Atomically creates a new, empty file named by this abstract
	 * pathname if and only if a file with this name does not yet exist.
	 */
	public boolean createNewFile() throws IOException
	{
		return wrapper.createNewFile();
	}


	/**
	 *	Creates an empty file in the default temporary-file directory,
	 * using the given prefix and suffix to generate its name.
	 */
	public static GeneralFile createTempFile(String prefix, String suffix)
		throws IOException
	{
		return new LocalFile( File.createTempFile(prefix, suffix) );
	}


	/**
	 * Creates a new empty file in the specified directory,
	 * using the given prefix and suffix strings to generate its name.
	 */
	public static GeneralFile createTempFile(String prefix, String suffix,
		GeneralFile directory) throws IOException
	{
		return new LocalFile( File.createTempFile(prefix, suffix,
			new File( directory.getAbsolutePath())) );
	}


	/**
	 *	Deletes the file or directory denoted by this abstract pathname.
	 */
	public boolean delete()
	{
		return wrapper.delete();
	}


	/**
	 *	Requests that the file or directory denoted by this abstract
	 * pathname be deleted when the virtual machine terminates.
	 */
	public void deleteOnExit()
	{
		wrapper.deleteOnExit();
	}


	/**
	 *	Tests this abstract pathname for equality with the given object.
	 */
	public boolean equals(Object obj)
	{
		return wrapper.equals(obj);
	}


	/**
	 *	Tests whether the file denoted by this abstract pathname exists.
	 */
	public boolean exists()
	{
		return wrapper.exists();
	}


	/**
	 *	Returns the absolute form of this abstract pathname.
	 */
	public GeneralFile getAbsoluteFile()
	{
		return new LocalFile( wrapper.getAbsoluteFile() );
	}


	/**
	 *	Returns the absolute pathname string of this abstract pathname.
	 */
	public String getAbsolutePath()
	{
		return wrapper.getAbsolutePath();
	}


	/**
	 *	Returns the canonical form of this abstract pathname.
	 */
	public GeneralFile getCanonicalFile() throws IOException
	{
		return new LocalFile( wrapper.getCanonicalFile() );
	}


	/**
	 *	Returns the canonical pathname string of this abstract pathname.
	 */
	public String getCanonicalPath() throws IOException
	{
		return wrapper.getCanonicalPath();
	}


	/**
	 *	Returns the name of the file or directory denoted by this abstract pathname.
	 */
	public String getName()
	{
		return wrapper.getName();
	}


	/**
	 *	Returns the pathname string of this abstract pathname's parent,
	 * or null if this pathname does not name a parent directory.
	 */
	public String getParent()
	{
		return wrapper.getParent();
	}


	/**
	 *	Returns the abstract pathname of this abstract pathname's parent,
	 * or null if this pathname does not name a parent directory.
	 */
	public GeneralFile getParentFile()
	{
		return new LocalFile( wrapper.getParentFile() );
	}


	/**
	 *	Converts this abstract pathname into a pathname string.
	 */
	public String getPath()
	{
		return wrapper.getPath();
	}


	/**
	 *	Computes a hash code for this abstract pathname.
	 */
	public int hashCode()
	{
		return wrapper.hashCode();
	}


	/**
	 *	Tests whether this abstract pathname is absolute.
	 */
	public boolean isAbsolute()
	{
		return wrapper.isAbsolute();
	}


	/**
	 *	Tests whether the file denoted by this abstract pathname is a directory.
	 */
	public boolean isDirectory()
	{
		return wrapper.isDirectory();
	}


	/**
	 *	Tests whether the file denoted by this abstract pathname is a normal file.
	 */
	public boolean isFile()
	{
		return wrapper.isFile();
	}


	/**
	 *	Tests whether the file named by this abstract pathname is a hidden file.
	 */
	public boolean isHidden()
	{
		return wrapper.isHidden();
	}


	/**
	 *	Returns the time that the file denoted by this abstract pathname
	 * was last modified.
	 */
	public long lastModified()
	{
		return wrapper.lastModified();
	}


	/**
	 *	Returns the length of the file denoted by this abstract pathname.
	 */
	public long length()
	{
		return wrapper.length();
	}


	/**
	 *	Returns an array of strings naming the files and directories in
	 * the directory denoted by this abstract pathname.
	 */
	public String[] list()
	{
		return wrapper.list();
	}


	/**
	 *	Returns an array of strings naming the files and directories in
	 * the directory denoted by this abstract pathname that satisfy
	 * the specified filter.
	 */
	public String[] list(FilenameFilter filter)
	{
		return wrapper.list(filter);
	}


	/**
	 *	Returns an array of abstract pathnames denoting the files in
	 * the directory denoted by this abstract pathname.
	 */
	public GeneralFile[] listFiles()
	{
		File fileList[] = wrapper.listFiles();
		if (fileList == null)
			return null;

		LocalFile localFileList[] = new LocalFile[fileList.length];

		for (int i=0; i<fileList.length;i++)
			localFileList[i] = new LocalFile( fileList[i] );

		return localFileList;
	}


	/**
	 *	Returns an array of abstract pathnames denoting the files and
	 * directories in the directory denoted by this abstract pathname
	 * that satisfy the specified filter.
	 */
	public GeneralFile[] listFiles(FileFilter filter)
	{
		File fileList[] = wrapper.listFiles(filter);
		LocalFile localFileList[] = new LocalFile[fileList.length];

		for (int i=0; i<fileList.length;i++)
			localFileList[i] = new LocalFile( fileList[i] );

		return localFileList;
	}


	/**
	 *	Returns an array of abstract pathnames denoting the files and
	 * directories in the directory denoted by this abstract pathname
	 * that satisfy the specified filter.
	 */
	public GeneralFile[] listFiles(FilenameFilter filter)
	{
		File fileList[] = wrapper.listFiles(filter);
		LocalFile localFileList[] = new LocalFile[fileList.length];

		for (int i=0; i<fileList.length;i++)
			localFileList[i] = new LocalFile( fileList[i] );

		return localFileList;
	}


	/**
	 *	List the available filesystem roots.
	 */
	public static GeneralFile[] listRoots()
	{
		File fileRoots[] = File.listRoots();
		LocalFile localFileRoots[] = new LocalFile[fileRoots.length];;

		for (int i=0; i<fileRoots.length;i++)
			localFileRoots[i] = new LocalFile( fileRoots[i] );

		return localFileRoots;
	}


	/**
	 *	Creates the directory named by this abstract pathname.
	 */
	public boolean mkdir()
	{
		return wrapper.mkdir();
	}


	/**
	 *	Creates the directory named by this abstract pathname,
	 * including any necessary but nonexistent parent directories.
	 */
	public boolean mkdirs()
	{
		return wrapper.mkdirs();
	}


	/**
	 *	Renames the file denoted by this abstract pathname.
	 */
	public boolean renameTo(GeneralFile dest)
	{
		return wrapper.renameTo(new File(dest.getAbsolutePath()));
	}


	/**
	 *	Sets the last-modified time of the file or directory named by
	 * this abstract pathname.
	 */
	public boolean setLastModified(long time)
	{
		return wrapper.setLastModified(time);
	}


	/**
	 *	Marks the file or directory named by this abstract pathname
	 * so that only read operations are allowed.
	 */
	public boolean setReadOnly()
	{
		return wrapper.setReadOnly();
	}


	/**
	 *	Returns the pathname string of this abstract pathname.
	 */
	public String toString()
	{
		return wrapper.toURI().toString();
	}


	/**
	 *	Constructs a file: URI that represents this abstract pathname.
	 */
	public URI toURI()
	{
		return wrapper.toURI();
	}


	/**
	 *	Converts this abstract pathname into a file: URL.
	 */
	public URL toURL()
		throws MalformedURLException
	{
		return wrapper.toURL();
	}
}
