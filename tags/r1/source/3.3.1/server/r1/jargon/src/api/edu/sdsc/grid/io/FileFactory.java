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
//	FileFactory.java	-  edu.sdsc.grid.io.FileFactory
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.FileFactory
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io;

import edu.sdsc.grid.io.local.*;
import edu.sdsc.grid.io.srb.*;

import java.io.IOException;
import java.net.URI;
import java.net.URISyntaxException;

/**
 * Operations include creating appropriately typed GeneralFile and
 * GeneralRandomAccessFile objects. Creating a file object can use
 * a "URI" (not a "URL").
 *<P>
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public final class FileFactory
{
//----------------------------------------------------------------------
// Static Methods
//----------------------------------------------------------------------
  /**
   * Creates a filesystem appropriate to the account object.
   *
   * @param account the account object used to initialize the filesystem.
   * @return a GeneralFileSystem object instanced from the appropriate subclass.
   * 		null will return a LocalFileSystem object.
   * @throws IOException If an IO error occurs during the connection to
   *		the filesystem.
   */
  public static GeneralFileSystem newFileSystem( GeneralAccount account )
  	throws IOException
  {
  	if (account != null) {
			Class accountType = account.getClass();

			try {
				if (accountType.equals(Class.forName(
					"edu.sdsc.grid.io.srb.SRBAccount"))) {
					return new SRBFileSystem( (SRBAccount) account );
				}
				//else if other types

			} catch ( ClassNotFoundException e ) {
				//I don't think this can both compile and throw this exception
			}
		}

		//Default to local
		return new LocalFileSystem();

  }



  /**
   * Creates an abstract pathname using this uri. Currently supported
   * URI schemes are "file://" and "srb://".
   *
   *
   * @param uri A URI object of the supported schemes.
   * @return a GeneralFile object instanced from the appropriate subclass.
   * @throws SecurityException The most likely cause is the URI did not
   * 		include a password. For security reasons, uri's are generally given
   *		without the password. Acquire a new password and
   *		@see GeneralFile newFile( URI uri, String password )
   *
   * @throws NullPointerException If uri argument is null.
   * @throws IOException If an IO error occurs testing the connection to
   *		the filesystem.
   */
  public static GeneralFile newFile( URI uri )
  	throws IOException
  {
		if (uri.getScheme().equals( "srb" ))
			return new SRBFile( uri );

		//else if other types

		//Default to local
		else
			return new LocalFile( uri );
  }


  /**
   * Creates an abstract pathname using this uri. Currently supported
   * URI schemes are "file://" and "srb://".
   *<P>
   * Including a text password in a URI string is not advisable for security
   * reasons. This method allows the password to obtained by more secure
   * methods. The connection to the file will then be made through the
   * default authorization method of the file's filesystem.
   *
   * @param uri A URI object of the supported schemes.
   * @return a GeneralFile object instanced from the appropriate subclass.
   * @throws IllegalArgumentException If the password contains illegal
   * 		charaters.
   * @throws NullPointerException If uri argument is null.
   * @throws IOException If an IO error occurs testing the connection to
   *		the filesystem.
   */
  public static GeneralFile newFile( URI uri, String password )
  	throws IOException
  {
  	int index = -1;
		String userInfo = uri.getUserInfo();

		if (uri.getScheme().equals( "srb" )) {
			if ((password.indexOf(":") >= 0) || (password.indexOf("@") >= 0))
				throw new IllegalArgumentException(
					"Password contains illegal charaters");

			index = userInfo.indexOf(":");
			if (index >= 0) {
				//remove password already in uri and
				userInfo = userInfo.substring(0,index);
			}
			//add param password
			userInfo += ":"+ password;

			try {
				uri = new URI( uri.getScheme(), userInfo, uri.getHost(),
					uri.getPort(), uri.getPath(), "", "" );
			} catch ( URISyntaxException e ) {
				throw new IllegalArgumentException(
					"Password contains illegal charaters");
			}
			return new SRBFile( uri );
		}
		//else if other types

		//Default to local
		return new LocalFile( uri );
  }


  /**
   * Creates an abstract pathname using a GeneralFileSystem object. Useful
   * for when you don't know which the file system the file is on. Calls
   * the constructor( FileSystem, String ) of the appropriate subclass.
   *
   * @param fs A generic file system reference object.
   * @param path The path to the file.
   * @return a GeneralFile object instanced from the appropriate subclass.
   * @throws NullPointerException If the path argument is null.
   */
  public static GeneralFile newFile( GeneralFileSystem fs, String path )
  {
  	if (fs != null) {
			Class serverType = fs.getClass();

			try {
				if (serverType.equals(Class.forName(
					"edu.sdsc.grid.io.srb.SRBFileSystem"))) {
					return new SRBFile( (SRBFileSystem) fs, path );
				}
				//else if other types

			} catch ( ClassNotFoundException e ) {
				//I don't think this can both compile and throw this exception
			}
		}
		//Default to local
		return new LocalFile( path );
  }


  /**
   * Creates an abstract pathname using a GeneralFileSystem object. Useful
   * for when you don't know which the file system the file is on. Calls
   * the constructor( FileSystem, String, String ) of the appropriate subclass.
   *
   * @param fs A generic file system reference object.
   * @param parent The directory path to the file.
   * @param child The file name string.
   * @return a GeneralFile object instanced from the appropriate subclass.
   * @throws NullPointerException If the child argument is null.
   */
  public static GeneralFile newFile( GeneralFileSystem fs, String parent,
  	String child )
  {
  	if (fs != null) {
			Class serverType = fs.getClass();

			try{
				if (serverType.equals(Class.forName("edu.sdsc.grid.io.srb.SRBFileSystem"))) {
					return new SRBFile( (SRBFileSystem) fs, parent, child );
				}
				//else if other types

			} catch ( ClassNotFoundException e ) {
				//I don't think this can both compile and throw this exception
			}
		}

		//Default to local
		return new LocalFile( parent, child );
	}


  /**
   * Creates an abstract pathname using a GeneralFileSystem object. Useful
   * for when you don't know which the file system the file is on. Calls
   * the constructor( File, String ) of the appropriate subclass.
   *
   * @param parent An abstract pathname to the file.
   * @param child The file name string.
   * @return a GeneralFile object instanced from the appropriate subclass.
   * @throws NullPointerException If the child argument is null.
   */
  public static GeneralFile newFile( GeneralFile parent, String child )
  {
  	if (parent != null) {
			Class fileType = parent.getClass();

			try{
				if (fileType.equals(Class.forName("edu.sdsc.grid.io.srb.SRBFile"))) {
					return new SRBFile( (SRBFile) parent, child );
				}
				//else if other types

			} catch ( ClassNotFoundException e ) {
				//I don't think this can both compile and throw this exception
			}
		}

		//Default to local
		return new LocalFile( (LocalFile) parent, child );
  }



  /**
   * Opens a random accecss connection to the file on an arbitrary file
   * system. Useful for when you don't know which the file system the
   * file is on.
   *
   * @return a GeneralRandomAccessFile object instanced from the
   *		appropriate subclass.
   * @throws NullPointerException If the file argument is null.
   * @throws IOException If an IO error occurs opening the file.
   */
  public static GeneralRandomAccessFile newRandomAccessFile(
  	GeneralFile file, String mode )
  	throws IOException
  {
  	Class fileType = file.getClass();
  	try {
			if (fileType.equals(Class.forName("edu.sdsc.grid.io.srb.SRBFile"))) {
			 return new SRBRandomAccessFile( (SRBFile) file, mode );
			}
			//else if other types

		} catch ( ClassNotFoundException e ) {
			//I don't think this can both compile and throw this exception
		}

		//Default to local
		return new LocalRandomAccessFile( (LocalFile) file, mode );
  }



  /**
   * Opens an input stream for the file on an arbitrary file
   * system. Useful for when you don't know which the file system the
   * file is on.
   *
   * @return a GeneralFileInputStream object instanced from the
   *		appropriate subclass.
   * @throws NullPointerException If the file argument is null.
   * @throws IOException If an IO error occurs opening the file.
   */
  public static GeneralFileInputStream newFileInputStream( GeneralFile file )
  	throws IOException
  {
  	Class fileType = file.getClass();
  	try {
			if (fileType.equals(Class.forName("edu.sdsc.grid.io.srb.SRBFile"))) {
			 return new SRBFileInputStream( (SRBFile) file );
			}
			//else if other types

		} catch ( ClassNotFoundException e ) {
			//I don't think this can both compile and throw this exception
		}

		//Default to local
		return new LocalFileInputStream( (LocalFile) file );
  }



  /**
   * Opens an output stream for the file on an arbitrary file
   * system. Useful for when you don't know which the file system the
   * file is on.
   *
   * @return a GeneralFileOutputStream object instanced from the
   *		appropriate subclass.
   * @throws NullPointerException If the file argument is null.
   * @throws IOException If an IO error occurs opening the file.
   */
  public static GeneralFileOutputStream newFileOutputStream( GeneralFile file )
  	throws IOException
  {
  	Class fileType = file.getClass();
  	try {
			if (fileType.equals(Class.forName("edu.sdsc.grid.io.srb.SRBFile"))) {
			 return new SRBFileOutputStream( (SRBFile) file );
			}
			//else if other types

		} catch ( ClassNotFoundException e ) {
			//I don't think this can both compile and throw this exception
		}

		//Default to local
		return new LocalFileOutputStream( (LocalFile) file );
  }
}



