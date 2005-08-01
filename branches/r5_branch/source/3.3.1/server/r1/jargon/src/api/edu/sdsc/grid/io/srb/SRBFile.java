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
//	SRBFile.java	-  edu.sdsc.grid.io.srb.SRBFile
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-.GeneralFile
//	            |
//	            +-.RemoteFile
//      	            |
//      	            +-.SRBFile
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.srb;

import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.local.*;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.net.ConnectException;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.net.URLConnection;
import java.util.Vector;
import java.util.Date;


/**
 * An abstract representation of file and directory pathnames on the SRB.
 *<P>
 * In the terminology of SRB, files are known as data sets. A data set is
 * a "stream-of-bytes" entity that can be uniquely identified. For example,
 * a file in HPSS or Unix is a data set, or a LOB stored in a SRB Vault
 * database is a data set. Importantly, note that a data set is not a
 * set of data objects/files. Each data set in SRB is given a unique
 * internal identifier by SRB. A dataset is associated with a collection.
 *<P>
 * A SRB collection is a logical name given to a set of data sets. All data
 * sets stored in SRB/MCAT are stored in some collection. A collection can
 * have sub-collections, and hence provides a hierarchical structure. A
 * collection in SRB/MCAT can be equated to a directory in a Unix file
 * system. But unlike a file system, a collection is not limited to a
 * single device (or partition). A collection is logical but the datsets
 * grouped under a collection can be stored in heterogeneous storage
 * devices. There is one obvious restriction, the name given to a data set
 * in a collection or sub-collection should be unique in that collection.
 *<P>
 * This class shares many similarities with the java.io.File class:
 * <p> User interfaces and operating systems use system-dependent <em>pathname
 * strings</em> to name files and directories.  This class presents an
 * abstract, SRB view of hierarchical pathnames.  An
 * <em>abstract pathname</em> has two components:
 *
 * <ol>
 * <li> An optional SRB <em>prefix</em> string, <code>"/"</code>&nbsp;
 * <li> A sequence of zero or more string <em>names</em>.
 * </ol>
 *
 * Each name in an abstract pathname except for the last denotes a directory;
 * the last name may denote either a directory or a file.  The <em>empty</em>
 * abstract pathname has no prefix and an empty name sequence.
 *<P>
 * When an abstract pathname is converted into a pathname string, each name
 * is separated from the next by a single copy of the default <em>separator
 * character</em>.  The name-separator character is defined by the
 * SRB as <code>"/"</code>, and is made available through
 * <code>getPathSeperator()</code> and <code>getPathSeperatorChar()</code>
 * of GeneralFile.
 *<P>
 * A pathname in string form may be either <em>absolute</em> or
 * <em>relative</em>. On construction the pathname is made absolute.
 * An absolute pathname is complete in that no other information is required in
 * order to locate the file that it denotes.  A relative pathname, in contrast,
 * must be interpreted in terms of information taken from some other pathname.
 * By default the classes in the <code>edu.sdsc.grid.io.srb</code> package always
 * resolve relative pathnames against the user home directory.  This directory
 * is named in the .MdasEnv file.
 *<P>
 * The prefix concept is used to handle root directories on the SRB is
 * the same as for UNIX platforms.
 * <p> For the SRB, the prefix of an absolute pathname is always
 * <code>"/"</code>.  Relative pathnames have no prefix.  The abstract pathname
 * denoting the root directory has the prefix <code>"/"</code> and an empty
 * name sequence.
 *<P>
 * Instances of the SRBFile class are immutable; that is, once created,
 * the abstract pathname represented by a SRBFile object will never change.
 *<P>
 * @author Lucas Gilbert, San Diego Supercomputer Center
 * @see	java.io.File
 * @see	edu.sdsc.grid.io.GeneralFile
 * @see	edu.sdsc.grid.io.RemoteFile
 * @since JARGON1.0
 */
public class SRBFile extends RemoteFile
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
	/**
	 * Default SRB catalog type.
	 */
	public static final int MDAS_CATALOG = 0;

	/**
	 * A SRB catalog type.
	 */
	public static final int LDAP_CATALOG = 1;

	/**
	 * A SRB catalog type.
	 */
	public static final int FILE_CATALOG = 2;

	/**
	 * Standard SRB path separator character represented as a string for
	 * convenience. This string contains a single character, namely
   * <code>{@link #PATH_SEPARATOR_CHAR}</code>.
   */
	public static final String PATH_SEPARATOR = "/";

	/**
	 * The SRB path separator character, '/'.
	 */
	public static final char PATH_SEPARATOR_CHAR = '/';


	//SRB proxy value
	static final int OPR_COPY = 0;


	/**
	 * Maximum number of threads used for parallel and bulk operations.
	 * It generally appears that increasing this beyond 4 threads raises
	 * the overhead without speeding up the transfers.
	 */
	/*protected */ static int MAX_NUMBER_OF_THREADS = 4;
//TODO setters?

	/**
	 * Minimum thread size, in bytes, of threads for parallel file transfers.
	 * If the file.length / NUMBER_OF_THREADS > THREAD_SIZE,
	 * then the thread size is automatically increased.
	 */
	/*protected */static int MIN_THREAD_SIZE = 30000000;


	/**
	 * The default buffer size for bulk load transfers, in bytes. 8MB
	 */
	static final int BULK_LOAD_BUFFER_SIZE = 8388608;

	/**
	 * Files bigger then this shouldn't use the bulk tansfer
	 */
	static final int MAX_BULK_FILE_SIZE = 2097152;


	/**
	 * Maximum files registered by at a time by a thread during bulkLoad.
	 */
	static final int MAX_REGISTRATION_FILES = 300;


	/** list the registered chksum value. */
	static final int LIST_CHECKSUM = 16; //#define l_FLAG 0x10

	/** compute chksum, but don't register. */
	static final int COMPUTE_CHECKSUM	= 128; //#define c_FLAG 0x80

	/** force compute and register of chksum even if one already exist. */
	static final int FORCE_CHECKSUM = 32; //#define f_FLAG 0x20

	/**
	 * Location of the SRB Zone Authority. When given a zone name the
	 * zone authority returns a xml document with the host and port
	 * information for that zone.
	 * see also, http://www.sdsc.edu/srb/cgi-bin/zoneList.cgi?zone=ZoneName
	 */
	static String ZONE_AUTHORITY  =
		"http://www.sdsc.edu/srb/cgi-bin/zoneList.cgi?zone=";

public static boolean USE_BULKCOPY = true;

//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * Holds the server object used by this srb file. Same as the
	 * <code>fileSystem</code> variable, but cast from GeneralFileSystem to
	 * SRBFileSystem.
	 */
	protected SRBFileSystem srbFileSystem;


	/**
	 * The type of SRB catalog.
	 * The <code>{@link #MDAS_CATALOG}</code> is the default catalog.
	 */
	int catalogType = MDAS_CATALOG;


	/**
	 * The storage resource name.
	 * A physical SRB resource is a system that is capable of storing data sets
	 * and is accessible to the SRB. It is registered in SRB with its physical
	 * characteristics such as its physical location, resource type, latency,
	 * and maximum file size. For example, HPSS can be a resource, as can a
	 * Unix file system.
	 */
	String resource;


	/**
	 * The data type of the file. The default value is "generic".
	 */
	String dataType = "generic";


	/**
	 * Get more records, if any exist, from the last list().
	 */
	int continuationIndex = -1;


	/**
	 * If delete on exit gets set.
	 */
	boolean deleteOnExit = false;


	static Vector uriFileSystems = new Vector();


	/**
	 * Since all filepaths get stored as canonical paths (to avoid errors),
	 * this variable keeps the original value of the path given to the
	 * constructor for use by the getPath() method.
	 */
	String originalFilePath;

//private, containers only have internal replicas.
	private int replicaNumber = -1;
//also note, if this object refers to a certain replica
//fileName will include &COPY= replica number
//getName() and getAbsolutePath() will not return the replica number
//getCanonicalPath() will return the replica number
//using the setReplicaNumber is preferred, however
//using "fileName&COPY=#" in the constructor is also valid.


	/**
	 * For list() should the entire contents be listed, or just
	 * SRBFileSystem.DEFAULT_RECORDS_WANTED in number.
	 */
	public boolean completeDirectoryList = true;



//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Creates a new <code>SRBFile</code> instance by converting the given
	 * pathname string into an abstract pathname.
	 *
	 *<P>
	 * @param fileSystem	The connection to the SRB
	 * @param filePath	The pathname string
	 * @throws  NullPointerException	If the given string is null or
	 * the empty string.
	 */
	public SRBFile( SRBFileSystem srbFileSystem, String filePath )
		throws NullPointerException
	{
		this( srbFileSystem, "", filePath );
		originalFilePath = filePath;
	}


	/**
	 * Creates a new <code>SRBFile</code> instance from a parent pathname
	 * string and a child pathname string.
	 *<P>
	 * If <code>parent</code> is <code>null</code> then the new
	 * <code>SRBFile</code> instance is created as if by invoking the
	 * single-argument <code>SRBFile</code> constructor on the given
	 * <code>child</code> pathname string.
	 *<P>
	 * Otherwise the <code>parent</code> pathname string is taken to denote a
	 * directory, and the <code>child</code> pathname string is taken to denote
	 * either a directory or a file. If the <code>child</code> pathname string
	 * is absolute then it is converted into a relative pathname in a
	 * SRB pathname. If parent is the empty string then the new
	 * <code>SRBFile</code> instance is created by converting <code>child</code>
	 * into an abstract pathname and resolving the result against the user's
	 * SRB default home directory. Otherwise each pathname string is
	 * converted into an abstract pathname and the <code>child</code> abstract
	 * pathname is resolved against the <code>parent</code>.
	 *<P>
	 * @param fileSystem	The connection to the SRB
	 * @param parent	The parent pathname string
	 * @param child		The child pathname string
	 * @throws  NullPointerException	If the child string is null or
	 * the empty string.
	 */
	public SRBFile( SRBFileSystem srbFileSystem, String parent, String child )
		throws NullPointerException, IllegalArgumentException
	{
		super( srbFileSystem, parent, child );

		makePathCanonical( parent );
		resource = srbFileSystem.getDefaultStorageResource();

		//just for getPath()
		if (parent != null) {
			if (!parent.equals( "" )) {
				originalFilePath = parent + PATH_SEPARATOR + child;
			}
			else {
				originalFilePath = child;
			}
		}
		else {
			originalFilePath = child;
		}
	}


	/**
	 * Creates a new <code>SRBFile</code> instance from a parent abstract
	 * pathname and a child pathname string.
	 *<P>
	 * If parent is null then the new <code>SRBFile</code> instance is created
	 * as if by invoking the single-argument <code>SRBFile</code> constructor
	 * on the given child pathname string.
	 *<P>
	 * Otherwise the <code>parent</code> abstract pathname is taken to denote a
	 * directory, and the <code>child</code> pathname string is taken to denote
	 * either a directory or a file. If the <code>child</code> pathname string is
	 * absolute then it is converted into a relative pathname in a
	 * SRB pathname. If <code>parent</code> is the empty abstract
	 * pathname then the new <code>SRBFile</code> instance is created by
	 * converting <code>child</code> into an abstract pathname and resolving
	 * the result against the user's SRB default home directory. Otherwise
	 * each pathname string is converted into an abstract pathname and the
	 * <code>child</code> abstract pathname is resolved against the
	 * <code>parent</code>.
	 *<P>
	 * @param parent	The parent abstract pathname
	 * @param child		The child pathname string
	 * @throws  NullPointerException	If the child string is null or
	 * the empty string.
	 */
	public SRBFile( SRBFile parent, String child )
		throws NullPointerException
	{
		this( (SRBFileSystem) parent.getFileSystem(),	parent.getPath(),
			child );
	}


	/**
	 * Creates a new GeneralFile instance by converting the given file: URI
	 * into an abstract pathname.
	 *<P>
	 * The exact form of a file: URI is system-dependent, hence the
	 * transformation performed by this constructor is also system-dependent.
	 *<P>
	 * For a given abstract pathname f it is guaranteed that
	 *<P>
	 * &nbsp;&nbsp;&nbsp;&nbsp;new GeneralFile( f.toURI() ).equals( f )
	 *<P>
	 * so long as the original abstract pathname, the URI, and the new
	 * abstract pathname are all created in (possibly different invocations of)
	 * the same Java virtual machine. This relationship typically does not hold,
	 * however, when a file: URI that is created in a virtual machine on one
	 * operating system is converted into an abstract pathname in a virtual
	 * machine on a different operating system.
	 *
	 * @param uri An absolute, hierarchical URI using the srb scheme.
	 * @throws NullPointerException if <code>uri</code> is <code>null</code>.
	 * @throws IllegalArgumentException If the preconditions on the parameter
	 *		do not hold.
	 * @throws IOException Can occur during the creation of the internal
	 * 		fileSystem object.
	 */
	public SRBFile( URI uri )
		throws NullPointerException, IOException
	{
		super(uri);

		//srb://userName.mdasDomain:password@host:port/path
		if (!uri.getScheme().equals("srb")) {
			throw new IllegalArgumentException();
		}

		SRBAccount account = uriInitialAccount( uri );
		String zone = null;
		String path = uri.getPath();
		originalFilePath = path;

		//The fileSystem is used by the following query.
		setFileSystem( new SRBFileSystem( account ));

		zone = getProperZone( srbFileSystem );
		srbFileSystem.setProxyMcatZone( zone );
		srbFileSystem.setMcatZone( zone );

		srbFileSystem.setDefaultStorageResource(
			getAvailableResource(	srbFileSystem ) );


		if ((path == null) || (path == "")) {
			setFileName( "/" );
		}
		else {
			setFileName( path );
		}

		setFileSystem( uriTest( srbFileSystem ) );
	}

	/**
	 * Finalizes the object by explicitly letting go of each of
	 * its internally held values.
	 */
	protected void finalize( )
		throws Throwable
	{
		if (deleteOnExit)
			delete();

		super.finalize();

		if (resource != null)
			resource = null;

		if (dataType != null)
			dataType = null;
	}


//----------------------------------------------------------------------
// Initialization for URIs
//----------------------------------------------------------------------
	/**
	 * Step one of SRBFile( uri )
	 */
	static SRBAccount uriInitialAccount( URI uri )
		throws IOException
	{
		//
		//I guess eventually, move all this to some Handler.java for SRB URIs.
		//
		String host = uri.getHost();
		int port = uri.getPort();
		String path = uri.getPath();
		String userInfo = uri.getUserInfo();
		String userName = null, mdasDomain = null, password = null;
		String homeDirectory = null;
		int index = -1;

		if ((userInfo == null) || (userInfo == "")) {
			//TODO anon. login
			userName = "public";
			mdasDomain = "npaci";
			password = "CANDO";
			homeDirectory = "/home/public.npaci/";
		}
		else {
			index = userInfo.indexOf( "." );
			if (index < 0) {
				throw new MalformedURLException();
			}
			userName = userInfo.substring( 0, index );

			if (index < 0) {
				throw new MalformedURLException();
			}
			userInfo = userInfo.substring( index+1 );

			index = userInfo.indexOf( ":" );
			if (index > 0) {
				mdasDomain = userInfo.substring( 0, index );
				password = userInfo.substring( index+1 );
			}
			else {
				mdasDomain = userInfo;
				//TODO anon. password
			}
			//set the home directory to the local zone
			homeDirectory = "/home/"+userName+"."+mdasDomain;
		}

		index = host.indexOf( "." );
		if ( index < 0 ) {
			//use zone authority
			URL url = new URL( ZONE_AUTHORITY +	host );
			InputStream in = url.openConnection().getInputStream();

//TODO sure not the best, doesn't use the xml.
			int index2 = -1;
			String result = null;
			byte[] data = new byte[1000];

			in.read(data);
			result = new String( data );
			//? first read always stops at the 26th byte
			in.read(data);
			result += new String( data );

			index = result.indexOf( "ns1:server" );
			index2 = result.indexOf( "/ns1:server", index+11 );
			if ((index < 0) || (index2 < 0))
				throw new ConnectException( "Invalid zone name." );
			host = result.substring( index+11, index2-1 );

			index = result.indexOf( "ns1:port" );
			index2 = result.indexOf( "/ns1:port", index+9 );
			result = result.substring( index+9, index2-1 );
			if ((result != null) && (result.length() > 0))
				port = Integer.parseInt( result );
		}

		if ( port < 0 ) {
			port = 5544;
		}

		//Have to find a storage resource after connection.
		return new SRBAccount( host, port, userName, password,
			homeDirectory, mdasDomain, "" );
	}

	/**
	 * Step two of SRBFile( uri )
	 */
	static String getProperZone( SRBFileSystem fs )
		throws IOException
	{
//TODO should I use the file zone? does it have one?
		//Query file system to determine this SRB user's zone,
		//then use that for the zone of its account object
		if (fs.getVersionNumber() >= 3)
		{
			String userName = fs.getUserName();
			String mdasDomain = fs.getDomainName();

			MetaDataRecordList[] rl = fs.query( SRBMetaDataSet.ZONE_NAME );
			if (rl == null) {
				//if null then file does not exist (or is dir?)
				//find what is the user the user can access, pick the first one.
				MetaDataCondition[] conditions = {
					MetaDataSet.newCondition( UserMetaData.USER_NAME,
						MetaDataCondition.LIKE, userName ),
					MetaDataSet.newCondition( SRBMetaDataSet.USER_DOMAIN,
						MetaDataCondition.EQUAL, mdasDomain ),
				};
				MetaDataSelect[] selects = {
					MetaDataSet.newSelection( SRBMetaDataSet.ZONE_NAME ) };
				rl = fs.query( conditions, selects );
			}
			return rl[0].getValue(	SRBMetaDataSet.ZONE_NAME ).toString();
		}

		return null;
	}

	/**
	 * Step three of SRBFile( uri )
	 */
	static String getAvailableResource( SRBFileSystem fs )
		throws IOException
	{
		MetaDataRecordList[] rl = null;
		if (fs.getVersionNumber() >= 3)
		{
			String userName = fs.getUserName();
			String mdasDomain = fs.getDomainName();
			String zone = fs.getMcatZone();


			//Query file system to determine this SRBFile's storage resource,
			//then use that for the default resource of its fileSystem object
			rl = fs.query( SRBMetaDataSet.RESOURCE_NAME );
			if (rl == null) {
				//if null then file does not exist (or is dir?)
				//find what resources the user can access, pick the first one.
				MetaDataCondition[] conditions = {
					MetaDataSet.newCondition( SRBMetaDataSet.RSRC_ACCESS_PRIVILEGE,
						MetaDataCondition.LIKE, "%write%" ),
					MetaDataSet.newCondition( SRBMetaDataSet.RSRC_ACCS_USER_NAME,
						MetaDataCondition.EQUAL, userName),
					MetaDataSet.newCondition( SRBMetaDataSet.RSRC_ACCS_USER_DOMAIN,
						MetaDataCondition.EQUAL, mdasDomain ),
					MetaDataSet.newCondition( SRBMetaDataSet.RSRC_ACCS_USER_ZONE,
						MetaDataCondition.EQUAL, zone ),
				};
				MetaDataSelect[] selects = {
					MetaDataSet.newSelection( SRBMetaDataSet.RESOURCE_NAME ) };
				rl = fs.query( conditions, selects );
			}
		} else {
			//Same as above, just no zone
			rl = fs.query( SRBMetaDataSet.RESOURCE_NAME );
			if (rl == null) {
				//if null then file does not exist (or is dir?)
				//Metadata to determine available resources was added only after SRB3
				throw new FileNotFoundException( "Unknown resource" );
			}
		}
		return rl[0].getValue( SRBMetaDataSet.RESOURCE_NAME ).toString();
	}


	/**
	 * Step four of SRBFile( uri )
	 */
	static synchronized SRBFileSystem uriTest( SRBFileSystem fs )
	{
		//having multiple connections to the same filesystem is bad
		//users can open a thousand files, which will open 1000 filesystem
		//connections and might even crash the SRB.
		//TODO? synchronized
		SRBFileSystem uriTest = null;
		for (int i=0;i<uriFileSystems.size();i++) {
			uriTest = (SRBFileSystem) uriFileSystems.get( i );
			if ( fs.equals( uriTest ) ) {
				fs = uriTest;
				return fs;
			}
			else if ( !uriTest.isConnected() ) {
				uriFileSystems.remove( i );
			}
		}
		//add the new fileSystem to the list
		uriFileSystems.add( fs );

		//TODO maybe unnecessary?
		return fs;
	}


//----------------------------------------------------------------------
// Setters and Getters
//----------------------------------------------------------------------
	/**
	 * Set the file name.
	 * @param fleName The file name or fileName plus some or all of the
	 * directory path.
	 */
	protected void setFileName( String filePath )
	{
		//used when parsing the filepath
		int index;

		//in case they used the local pathSeperator
		//in the fileName instead of the srb PATH_SEPARATOR.
		String localSeparator = System.getProperty( "file.separator" );

		if ( filePath == null ) {
			throw new NullPointerException( "The file name cannot be null" );
		}

		//replace local separators with SRB separators.
		if (!localSeparator.equals(PATH_SEPARATOR)) {
			index = filePath.lastIndexOf( localSeparator );
			while ((index >= 0) && ((filePath.substring( index + 1 ).length()) > 0)) {
				filePath = filePath.substring( 0, index ) + PATH_SEPARATOR +
					filePath.substring( index + 1 );
				index = filePath.lastIndexOf( localSeparator );
			}
		}
		fileName = filePath;

		if (fileName.length() > 1) { //add to allow path = root "/"
			index = fileName.lastIndexOf( PATH_SEPARATOR );
			while ((index == fileName.length()-1) && (index >= 0)) {
				//remove '/' at end of filename, if exists
				fileName =  fileName.substring( 0, index );
				index = fileName.lastIndexOf( PATH_SEPARATOR );
			}

			//seperate directory and file
			if ((index >= 0) &&
				((fileName.substring( index + 1 ).length()) > 0)) {
				//have to run setDirectory(...) again
				//because they put filepath info in the filename
				setDirectory( fileName.substring( 0, index + 1 ) );
				fileName =  fileName.substring( index + 1 );
			}
		}
	}


	/**
	 * Set the directory.
	 * @param dir	The directory path, need not be absolute.
	 */
//though everything will be converted to a canonical path	to avoid errors
	protected void setDirectory( String dir )
	{
		if (directory == null) {
			directory = new Vector();
		}
		if (dir == null) {
			return;
		}

		//in case they used the local pathSeperator
		//in the fileName instead of the srb PATH_SEPARATOR.
		String localSeparator = System.getProperty( "file.separator" );
		int index = dir.lastIndexOf( localSeparator );
		if ((index >= 0) && ((dir.substring( index + 1 ).length()) > 0)) {
			dir = dir.substring( 0, index ) + PATH_SEPARATOR +
				dir.substring( index + 1 );
			index = dir.lastIndexOf( localSeparator );
		}

//TODO while?
		while ((directory.size() > 0) && //only if this is the dir cut from fileName
						dir.startsWith(PATH_SEPARATOR))// &&  //strip these
//						(dir.length() > 1)) //but not if they only wanted
		{
			dir = dir.substring(1);
//TODO ****************************************************
//if dir passed from filename starts with PATH_SEPARATOR
//System.out.println("dir3 "+dir);
		}

		//create directory vector
		index = dir.indexOf( PATH_SEPARATOR );

		if (index >= 0) {
			do {
				directory.add( dir.substring( 0, index ) );
				do {
					dir = dir.substring( index + 1 );
					index = dir.indexOf( PATH_SEPARATOR );
				} while (index == 0);
			} while (index >= 0);
		}
		//add the last path item
		if ((!dir.equals("")) && (dir != null)) {
			directory.add( dir );
		}
	}


	/**
	 * Helper for setting the directory to an absolute path
	 * @param dir Used to determine if the path is absolute.
	 */
//Yes, this whole business is the most horrible thing you have ever seen.
//
//using "fileName&COPY=#" in the constructor should stay valid.
	void makePathCanonical( String dir )
	{
		int i = 0; //where to insert into the Vector
		boolean absolutePath = false;
		String canonicalTest = null;

		if (dir == null) {
			dir = "";
		}

		//In case this abstract path is supposed to be root
		if ((fileName.equals(SRBFileSystem.SRB_ROOT)) && (dir == "")) {
			return;
		}

		//In case this abstract path is supposed to be the home directory
		if (fileName.equals("") && dir.equals("")) {
			String home = srbFileSystem.getHomeDirectory();
			int index = home.lastIndexOf( PATH_SEPARATOR );
			setDirectory( home.substring( 0, index ) );
			setFileName( home.substring( index+1 ) );
			return;
		}


		//if dir not absolute
		if (dir.startsWith(SRBFileSystem.SRB_ROOT))
			absolutePath = true;

		//if directory not already absolute
		if (directory.size() > 0) {
			if (directory.get(0).toString().length() == 0) {
				//The /'s were all striped when the vector was created
				//so if the first element of the vector is null
				//but the vector isn't null, then the first element
				//is really a /.
				absolutePath = true;
			}
		}
		if (!absolutePath) {
			String home = srbFileSystem.getHomeDirectory();
			int index = home.indexOf( PATH_SEPARATOR );
			//allow the first index to = 0,
			//because otherwise PATH_SEPARATOR won't get added in front.
			if (index >= 0) {
				do {
					directory.add( i, home.substring( 0, index ) );
					home = home.substring( index + 1 );
					index = home.indexOf( PATH_SEPARATOR );
					i++;
				} while (index > 0);
			}
			if ((!home.equals("")) && (home != null)) {
				directory.add( i, home );
			}
		}


		//first, made absolute, then canonical
		for (i=0; i<directory.size(); i++) {
			canonicalTest = directory.get(i).toString();
			if (canonicalTest.equals( "." )) {
				directory.remove( i );
				i--;
			}
			else if ((canonicalTest.equals( ".." )) && (i >= 2)) {
				directory.remove( i );
				directory.remove( i-1 );
				i--;
				if (i > 0)
					i--;
			}
			else if (canonicalTest.equals( ".." )) {
				//at root, just remove the ..
				directory.remove( i );
				i--;
			}
			else if (canonicalTest.startsWith( PATH_SEPARATOR )) {
				//if somebody put filepath as /foo//bar or /foo////bar
				do {
					canonicalTest = canonicalTest.substring( 1 );
				} while (canonicalTest.startsWith( PATH_SEPARATOR ));
				directory.remove( i );
				directory.add( i, canonicalTest );
			}
		}
		//also must check fileName
		if (fileName.equals( "." )) {
			fileName = directory.get(directory.size()-1).toString();
			directory.remove( directory.size()-1 );
		}
		else if (fileName.equals( ".." )) {
			if (directory.size() > 1) {
				fileName = directory.get(directory.size()-2).toString();
				directory.remove( directory.size()-1 );
				directory.remove( directory.size()-1 );
			}
			else {
				//at root
				fileName = PATH_SEPARATOR;
				directory.remove( directory.size()-1 );
			}
		}


		//maybe they tried to be smart and put the replica number on the filename
		//filename&COPY=#
		getReplicaNumber();
	}


	/**
	 * Sets the SRB server used of this SRBFile object.
	 *
	 * @param fileSystem The SRB server to be used.
	 * @throws IllegalArgumentException - if the argument is null.
	 * @throws ClassCastException - if the argument is not a SRBFileSystem object.
	 */
	protected void setFileSystem( GeneralFileSystem srbFileSystem )
		throws IllegalArgumentException
	{
		if ( srbFileSystem == null )
			throw new IllegalArgumentException("Illegal fileSystem, cannot be null");

		fileSystem = srbFileSystem;
		this.srbFileSystem = (SRBFileSystem) fileSystem;
	}



	/**
	 * Sets the physical resource this SRBFile object is stored on.
	 * If the file exists its resource will be changed to <code>resource</code>.
	 *
	 * @param resource The name of resource to be used.
	 * @throws NullPointerException If resourceName is null.
	 * @throws IllegalArgumentException If resourceName is not a valid resource.
	 * @throws IOException If an IOException occurs during the system change.
	 */
	public void setResource( String resourceName )
		throws IOException, NullPointerException, IllegalArgumentException
	{
		if ( resourceName != null ) {
			MetaDataCondition[] conditions = { MetaDataSet.newCondition(
				SRBMetaDataSet.RESOURCE_NAME, MetaDataCondition.EQUAL, resourceName ) };
			MetaDataSelect[] selects = { MetaDataSet.newSelection(
				SRBMetaDataSet.RESOURCE_NAME ) };
			MetaDataRecordList[] resources =
				srbFileSystem.query( conditions, selects );

			if (resources == null)
				throw new IllegalArgumentException("Resource not found");

			//else resource exists
			resource = resourceName;

			if (isFile()) {
				srbFileSystem.srbObjMove( catalogType, fileName,
					getParent(), resource, "" );
			}
		}
		else
			throw new NullPointerException();
	}

	/**
	 * Sets the dataType string of this SRBFile. If <code>dataTypeName</code>
	 * is null, the default type of "generic", will be used.
	 *
	 * @throws IOException If an IOException occurs during the system change.
	 */
	public void setDataType( String dataTypeName )
		throws IOException
	{
		if ( dataTypeName == null ) {
			dataTypeName = "generic";
		}

		if (isFile()) {
			if (getReplicaNumber() >= 0) {
				srbFileSystem.srbModifyDataset( catalogType,
					fileName,	getParent(), null, null, dataTypeName,
					null, SRBMetaDataSet.D_CHANGE_TYPE);
			}
			else {
				srbFileSystem.srbModifyDataset( catalogType, getName(),
					getParent(), null, null, dataTypeName, null,
					SRBMetaDataSet.D_CHANGE_TYPE);
			}
		}

		dataType = dataTypeName;
	}


	/**
	 * Sets the catalogType string of this SRBFile. The default type is
	 * <code>MDAS_CATALOG</code>
	 */
/*
	public void setCatalogType( int catalog )
	{
//TODO what are the other types? is this something that can really be set?
		if ( catalog > 0 )
			this.catalogType = catalog;
		else
			this.catalogType = 0;
	}
*/


	/**
	 * Sets the specific physical data replication refered to by this object.
	 * ReplicaNumbers are always positive. setting a negative number will
	 * remove the any specific replica reference.
	 */
	public void setReplicaNumber( int replicaNumber )
	{
		if (replicaNumber >= 0) {
			//getName() strips the old number
			fileName = getName() + "&COPY=" + replicaNumber;
			this.replicaNumber = replicaNumber;
		}
		else {
			fileName = getName();
			this.replicaNumber = -1;
		}
	}
//note, if this object refers to a certain replica
//the global GeneralFile variable fileName will include &COPY= replica number
//getName() and getAbsolutePath() will not return the replica number
//getCanonicalPath() will return the replica number
//using the setReplicaNumber() is preferred, however
//using "fileName&COPY=#" in the constructor is also valid,
//because I don't want to parse it there.
//TODO If users add &COPY= to the files on their own, bad stuff could happen.


	/**
	 * Gets the specific physical data replication refered to by this object.
	 *
	 * @return The replica number refered to by this SRBFile object.
	 * Returns a negative value if this SRBFile object does not refer to a
	 * specific replica.
	 */
	public int getReplicaNumber( )
	{
		if (replicaNumber >= 0) {
			return replicaNumber;
		}

		int index = fileName.indexOf( "&COPY=" );
		if (index >= 0) {
			replicaNumber = new Integer(fileName.substring( index+6 )).intValue();
			return replicaNumber;
		}
		else {
			return -1;
		}
	}

	/**
	 * @return resource the physical resource where this SRBFile is stored.
	 *		Will not query the SRB if this abstract pathname is a directory.
	 * 		Returns null if the file is a directory or does not exist.
	 *
	 * @throws IOException If an IOException occurs during the system query.
	 */
	public String getResource( )
		throws IOException
	{
		if (isDirectory()) {
			return null;
		}

		//if replicaNumber, filename, path will done in query()
		MetaDataRecordList[] rl = query( SRBMetaDataSet.PHYSICAL_RESOURCE_NAME );

		if (rl != null) {
			resource = rl[0].getValue( SRBMetaDataSet.PHYSICAL_RESOURCE_NAME ).toString();

			return resource;
		}

		//not exist
		return null;
	}


	/**
	 * @return dataType  The dataType string of this SRBFile.
	 *		Will not query the SRB if this abstract pathname is a directory.
	 * 		Returns null if the file does not exist.
	 *
	 * @throws IOException If an IOException occurs during the system query.
	 */
	public String getDataType( )
		throws IOException
	{
		if (isDirectory()) {
			return dataType;
		}

		MetaDataRecordList[] rl = query( SRBMetaDataSet.FILE_TYPE_NAME );
		if (rl == null)
			return null;

		return rl[0].getValue( SRBMetaDataSet.FILE_TYPE_NAME ).toString();
	}


	/**
	 * @return catalogType the catalog type int of this SRBFile.
	 */
	public int getCatalogType( )
	{
		return catalogType;//TODO should query SRB, what is the metadata index?
	}


	/**
	 * This method gets the path separator as defined by the SRB protocol.
	 */
	public final String getPathSeparator( )
	{
		return PATH_SEPARATOR;
	}


	/**
	 * This method gets the path separator char as defined by the SRB protocol.
	 */
	public final char getPathSeparatorChar( )
	{
		return PATH_SEPARATOR_CHAR;
	}




	/**
	 * @return fileSystem the SRBFileSystem object of this SRBFile.
	 * @throws	NullPointerException	if fileSystem is null.
	 */
	public GeneralFileSystem getFileSystem( )
		throws NullPointerException
	{
		if ( srbFileSystem != null )
			return srbFileSystem;

		throw new NullPointerException("fileSystem is null.");
	}






//----------------------------------------------------------------------
// GeneralFile Methods
//----------------------------------------------------------------------
	/**
	 * Queries metadata specific to this SRBFile object and selects one
	 * metadata value, <code>fieldName</code>, to be returned.
	 *
	 * @param fieldName The string name used to form the select object.
	 * @return The metadata values for this file refered to by
	 * <code>fieldName</code>
	 */
	public MetaDataRecordList[] query( String fieldName )
		 throws IOException
	{
		MetaDataSelect[] temp = { MetaDataSet.newSelection( fieldName ) };
		return query( temp, SRBFileSystem.DEFAULT_RECORDS_WANTED );
	}


	/**
	 * Queries metadata specific to this SRBFile object.
	 *
	 * @param fieldNames The string names used to form the select objects.
	 * @return The metadata values for this file refered to by
	 * <code>fieldNames</code>
	 */
	public MetaDataRecordList[] query( String[] fieldNames )
		 throws IOException
	{
		return query( MetaDataSet.newSelection( fieldNames ),
			SRBFileSystem.DEFAULT_RECORDS_WANTED );
	}


	/**
	 * Queries metadata specific to this SRBFile object and selects one
	 * metadata value, <code>select</code>, to be returned.
	 */
	public MetaDataRecordList[] query( MetaDataSelect select )
		 throws IOException
	{
		MetaDataSelect[] temp = { select };
		return query( temp, SRBFileSystem.DEFAULT_RECORDS_WANTED );
	}


	/**
	 * Queries metadata specific to this SRBFile object.
	 */
	public MetaDataRecordList[] query( MetaDataSelect[] selects )
		 throws IOException
	{
		return query( selects, SRBFileSystem.DEFAULT_RECORDS_WANTED );
	}


	public MetaDataRecordList[] query( MetaDataSelect[] selects,
		int recordsWanted )
		throws IOException
	{
		MetaDataCondition iConditions[] = null;
		String fieldName = null;
		int operator = MetaDataCondition.EQUAL;
		String value = null;

		if (isDirectory()) {
			iConditions = new MetaDataCondition[1];
			fieldName = SRBMetaDataSet.DIRECTORY_NAME;
			operator = MetaDataCondition.EQUAL;
			value = getAbsolutePath();
			iConditions[0] =
				MetaDataSet.newCondition( fieldName, operator, value );
		}
		else {
			iConditions = new MetaDataCondition[3];
			fieldName = SRBMetaDataSet.DIRECTORY_NAME;
			operator = MetaDataCondition.EQUAL;
			value = getParent();
			iConditions[0] =
				MetaDataSet.newCondition( fieldName, operator, value );

			fieldName = SRBMetaDataSet.FILE_NAME;
			value = getName();
			iConditions[1] =
				MetaDataSet.newCondition( fieldName, operator, value );

			if (getReplicaNumber() >= 0) {
				fieldName = SRBMetaDataSet.FILE_REPLICATION_ENUM;
				value = "" + replicaNumber;
				iConditions[2] =
					MetaDataSet.newCondition( fieldName, operator, value );
			}
			//else last condition = null, will be ignored
		}

		return srbFileSystem.query( iConditions, selects, recordsWanted );
	}


	public MetaDataRecordList[] query( MetaDataCondition[] conditions,
		MetaDataSelect[] selects )
		throws IOException
	{
		return query( conditions, selects, SRBFileSystem.DEFAULT_RECORDS_WANTED );
	}


	public MetaDataRecordList[] query(	MetaDataCondition[] conditions,
		MetaDataSelect[] selects, int recordsWanted )
		throws IOException
	{
		MetaDataCondition iConditions[] = null;
		String fieldName = null;
		int operator = MetaDataCondition.EQUAL;
		String value = null;

		if (isDirectory()) {
			iConditions = new MetaDataCondition[conditions.length+1];

			System.arraycopy( conditions, 0, iConditions, 0, conditions.length);

			fieldName = SRBMetaDataSet.DIRECTORY_NAME;
			operator = MetaDataCondition.EQUAL;
			value = getAbsolutePath();
			iConditions[conditions.length] =
				MetaDataSet.newCondition( fieldName, operator, value );
		}
		else {
			iConditions = new MetaDataCondition[conditions.length+3];

			System.arraycopy( conditions, 0, iConditions, 0, conditions.length);

			fieldName = SRBMetaDataSet.DIRECTORY_NAME;
			operator = MetaDataCondition.EQUAL;
			value = getParent();
			iConditions[conditions.length] =
				MetaDataSet.newCondition( fieldName, operator, value );

			fieldName = SRBMetaDataSet.FILE_NAME;
			value = fileName;
			iConditions[conditions.length+1] =
				MetaDataSet.newCondition( fieldName, operator, value );

			if (getReplicaNumber() >= 0) {
				fieldName = SRBMetaDataSet.FILE_REPLICATION_ENUM;
				value = "" +replicaNumber;
				iConditions[2] =
					MetaDataSet.newCondition( fieldName, operator, value );
			}
			//else last condition = null, will be ignored
		}

		return srbFileSystem.query( iConditions, selects, recordsWanted );
	}




	/**
	 * Change the values of the metadata associated with this file object.
	 * Not all metadata values can be modified. Also not all metadata value
	 * types apply to every object, i.e. directories and files have different
	 * metadata types.
	 *
	 * The following is a list of modifiable metadata types:
	 * (see also SRBMetaDataSet)
	 * CONTAINER_FOR_DIRECTORY<br>
	 * DIRECTORY_ANNOTATION<br>
	 * DIRECTORY_NAME<br>
	 * FILE_ANNOTATION<br>
	 * FILE_ANNOTATION_TIMESTAMP<br>
	 * FILE_AUDITFLAG<br>
	 * FILE_CHECKSUM<br>
	 * FILE_CLASS_NAME<br>
	 * FILE_COMMENTS<br>
	 * FILE_EXPIRE_DATE_2<br>
	 * FILE_EXPIRY_DATE<br>
	 * FILE_HIDE<br>
	 * FILE_IS_COMPRESSED<br>
	 * FILE_IS_ENCRYPTED<br>
	 * FILE_LAST_ACCESS_TIMESTAMP<br>
	 * FILE_LOCK_NUM<br>
	 * FILE_NAME<br>
	 * FILE_PIN_VAL<br>
	 * FILE_REPLICATION_ENUM<br>
	 * FILE_TYPE_NAME<br>
	 * INDEX_NAME_FOR_DATATYPE<br>
	 * INDEX_NAME_FOR_DIRECTORY<br>
	 * INDEX_NAME_FOR_FILE<br>
	 * IS_DIRTY<br>
	 * METHOD_NAME_FOR_DATATYPE<br>
	 * METHOD_NAME_FOR_DIRECTORY<br>
	 * METHOD_NAME_FOR_FILE<br>
	 * MODIFICATION_DATE<br>
	 * OFFSET<br>
	 * OWNER<br>
	 * PATH_NAME<br>
	 * SIZE<br>
	 * DEFINABLE_METADATA_FOR_FILES<br>
	 * DEFINABLE_METADATA_FOR_DIRECTORIES<br>
	 *<br>
	 * note: Dates timestamps are Strings with the format: YYYY-MM-DD-HH.MM.SS
	 *
	 * @throws FileNotFoundException If the vault storage path can't be found.
	 *		This can occur if the file was improperly deleted, removing the data
	 *		on disk without removing the metadata.
	 */
	public void modifyMetaData( MetaDataRecordList record )
		throws IOException
	{
//The srb C version, sort of,
//catalog/mdas-srb/srbC_mdas_library.c - modify_dataset_info(...)

		//the new metadata value, or various other uses...
		String dataValue1 = null, dataValue2 = null, dataValue3 = null;

//TODO find out which is which
		//This type of metadata only exists for files/dirs.
		boolean filesOnly = false;
		boolean dirsOnly = false;
		boolean isFile = isFile();

		//If there is no replica number,
		//Must get the actual file path on the machine which stores the data
		//and the storage resource.
		//Have to query first to get this.
		String vaultPathName = null;
		if ((getReplicaNumber() < 0) && isFile) {
			MetaDataSelect[] selects = {
				MetaDataSet.newSelection(SRBMetaDataSet.PATH_NAME),
				MetaDataSet.newSelection(SRBMetaDataSet.RESOURCE_NAME) };
			MetaDataRecordList rl[] = query( selects );
			if (rl != null) {
				vaultPathName = rl[0].getValue( SRBMetaDataSet.PATH_NAME ).toString();
				resource = rl[0].getValue( SRBMetaDataSet.RESOURCE_NAME ).toString();
			}
			else {
				throw new FileNotFoundException( "Vault path/resource not found." );
			}
		}

		//tells the srb what metadata is being changed
		int retractionType = -1;

		//if this is a MetaData type that can be modified.
//		boolean modifiable = false;

		//if the metadata is to be deleted, it uses a different retraction type.
		boolean delete = false;

		for (int i=0;i<record.getFieldCount();i++) {
			if (record.getValue(i) == null)
				delete = true;
			else
				delete = false;


			if (record.getFieldName(i) == SRBMetaDataSet.FILE_COMMENTS)
			{
				if (delete) {
					retractionType = SRBMetaDataSet.D_DELETE_COMMENTS;
				}
				else {
					retractionType = SRBMetaDataSet.D_INSERT_COMMENTS;
					dataValue1 = record.getStringValue(i);
				}
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.SIZE)
			{
				retractionType = SRBMetaDataSet.D_CHANGE_SIZE;
				dataValue1 = record.getStringValue(i);
				filesOnly = true;
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_TYPE_NAME)
			{
				retractionType = SRBMetaDataSet.D_CHANGE_TYPE;
				dataValue1 = record.getStringValue(i);
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.DIRECTORY_NAME)
			{
				retractionType = SRBMetaDataSet.D_CHANGE_GROUP;
				dataValue1 = record.getStringValue(i);
			}



/*TODO er?
	if (record.getFieldName(i) == SRBMetaDataSet.not used,not used)
	{
		if (delete) {
			retractionType = SRBMetaDataSet.D_DELETE_ALL_FROM_CONTAINER;@@@@@@@@@
	}
*/


			else if (record.getFieldName(i) == SRBMetaDataSet.OFFSET)
			{
				retractionType = SRBMetaDataSet.D_CHANGE_OFFSET;
				dataValue1 = record.getStringValue(i);
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.IS_DIRTY)
			{
				retractionType = SRBMetaDataSet.D_CHANGE_DIRTY;
				dataValue1 = record.getStringValue(i);
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_REPLICATION_ENUM)
			{
//oldReplNum:newReplNum, oldVerNum:newVerNum	use -1 for new values for using internal	generated number
		retractionType = SRBMetaDataSet.D_CHANGE_REPLNUM_VERNUM;
		dataValue1 = record.getStringValue(i);
	//@@@
			}


//TODO right metadata value???
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_LOCK_NUM)
			{
//accessName, not_used	use read, write  or null for accessName	read=
//readLock not even read is allowed	write= writeLock write not allowed
//null= remove lock.
				retractionType = SRBMetaDataSet.D_INSERT_LOCK;
				dataValue1 = record.getStringValue(i);
			//@@@
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_CHECKSUM)
			{
				retractionType = SRBMetaDataSet.D_INSERT_DCHECKSUM;
				dataValue1 = record.getStringValue(i);
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_HIDE)
			{
//hidevalue=0 for not hiding @@@@@@@
				retractionType = SRBMetaDataSet.D_INSERT_DHIDE;
				dataValue1 = record.getStringValue(i);
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_AUDITFLAG)
			{
				retractionType = SRBMetaDataSet.D_INSERT_AUDITFLAG;
				dataValue1 = record.getStringValue(i);
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_PIN_VAL)
			{
//pinNumber, pinExpiryDate	pinNumber = 0 for no pin	pinExpiryDate = YYYY-MM-DD-HH.MM.SS
				retractionType = SRBMetaDataSet.D_UPDATE_PIN;
				dataValue1 = record.getStringValue(i);
			//@@@
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_EXPIRY_DATE)
			{
//expiryDate = YYYY-MM-DD-HH.MM.SS
				retractionType = SRBMetaDataSet.D_UPDATE_DEXPIRE_DATE;
				dataValue1 = record.getStringValue(i);
			}


//TODO why are there two?
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_EXPIRE_DATE_2)
			{
//expiryDate = YYYY-MM-DD-HH.MM.SS
				retractionType = SRBMetaDataSet.D_UPDATE_DEXPIRE_DATE_2;
				dataValue1 = record.getStringValue(i);
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_IS_COMPRESSED)
			{
//compressionInfo is a string
				retractionType = SRBMetaDataSet.D_UPDATE_DCOMPRESSED;
				dataValue1 = record.getStringValue(i);
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_IS_ENCRYPTED)
			{
//encryptionInfo is a string
				retractionType = SRBMetaDataSet.D_UPDATE_DENCRYPTED;
				dataValue1 = record.getStringValue(i);
			}


/*
	if (record.getFieldName(i) == SRBMetaDataSet.)
	{
//newCollName, newDataName	links the data to this srbObjectPath
		retractionType = SRBMetaDataSet.D_INSERT_LINK;
		dataValue1 = record.getStringValue(i);
	//@@@
	}
*/
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_NAME)
			{
//same as renameTo(GeneralFile)
//newDataName, unused
				retractionType = SRBMetaDataSet.D_CHANGE_DNAME;
				dataValue1 = record.getStringValue(i);
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_ANNOTATION_TIMESTAMP)
			{
				if (delete) {
					retractionType = SRBMetaDataSet.D_DELETE_ANNOTATIONS;
				}
				else {
					retractionType = SRBMetaDataSet.D_INSERT_ANNOTATIONS;
					dataValue1 = record.getStringValue(i);
				}
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_ANNOTATION)
			{
//TODO what? //newAnn[@@@newPosition, newAnnotationDate
				retractionType = SRBMetaDataSet.D_UPDATE_ANNOTATIONS;
				dataValue1 = record.getStringValue(i);
			//@@@
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.OWNER)
			{
//newOwnerName, newOwnerDomain[|Zone]
				retractionType = SRBMetaDataSet.D_CHANGE_OWNER;
				dataValue1 = record.getStringValue(i);
			//@@@
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.PATH_NAME)
			{
//newPathName, not_used	(be careful how you use this!)
				retractionType = SRBMetaDataSet.D_CHANGE_DPATH;
				dataValue1 = record.getStringValue(i);
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.MODIFICATION_DATE)
			{
//newTimeStamp, not_used	newTimeStamp=YYYY-MM-DD-HH.MM.SS
				retractionType = SRBMetaDataSet.D_CHANGE_MODIFY_TIMESTAMP;
				dataValue1 = record.getStringValue(i);
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_LAST_ACCESS_TIMESTAMP)
			{
//newAccessTime, not_used	newAccessTime=YYYY-MM-DD-HH.MM.SS
				retractionType = SRBMetaDataSet.D_CHANGE_LAST_ACCS_TIME;
				dataValue1 = record.getStringValue(i);
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.FILE_CLASS_NAME)
			{
//classTypeString,classNameString	(user-defined)
				if (delete) {
					retractionType = SRBMetaDataSet.D_DELETE_CLASS;
				}
				else {
					retractionType = SRBMetaDataSet.D_INSERT_CLASS;
					dataValue1 = record.getStringValue(i);
				}
			//@@@
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.INDEX_NAME_FOR_FILE)
			{
//srbObjName, not_used	srbObjName contains the index values	used by the data_name
				if (delete) {
					retractionType = SRBMetaDataSet.D_DELETE_INDEX_FOR_DATA;
				}
				else {
					retractionType = SRBMetaDataSet.D_INSERT_INDEX_FOR_DATA;
					dataValue1 = record.getStringValue(i);
				}
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.INDEX_NAME_FOR_DATATYPE)
			{
//dataTypeName,not_used	dataType for which the data_name cane be	used as index
				if (delete) {
					retractionType = SRBMetaDataSet.D_DELETE_INDEX_FOR_DATATYPE;
				}
				else {
					retractionType = SRBMetaDataSet.D_INSERT_INDEX_FOR_DATATYPE;
					dataValue1 = record.getStringValue(i);
				}
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.INDEX_NAME_FOR_DIRECTORY)
			{
//collectionName,not_used	Collection for which the data_name can be	used as index
				if (delete) {
					retractionType = SRBMetaDataSet.D_DELETE_INDEX_FOR_COLLECTION;
				}
				else {
					retractionType = SRBMetaDataSet.D_INSERT_INDEX_FOR_COLLECTION;
					dataValue1 = record.getStringValue(i);
				}
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.METHOD_NAME_FOR_FILE)
			{
//srbObjName, not_used	srbObjName contains the method applicable	to  the data_name
				if (delete) {
					retractionType = SRBMetaDataSet.D_DELETE_METHOD_FOR_DATA;
				}
				else {
					retractionType = SRBMetaDataSet.D_INSERT_METHOD_FOR_DATA;
					dataValue1 = record.getStringValue(i);
				}
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.METHOD_NAME_FOR_DATATYPE)
			{
//dataTypeName,not_used	dataType for which the data_name cane be	used as a method
				if (delete) {
					retractionType = SRBMetaDataSet.D_DELETE_METHOD_FOR_DATATYPE;
				}
				else {
					retractionType = SRBMetaDataSet.D_INSERT_METHOD_FOR_DATATYPE;
					dataValue1 = record.getStringValue(i);
				}
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.METHOD_NAME_FOR_DIRECTORY)
			{
//collectionName, not_used	collection for whose data  the data_name	can be used as a method
				if (delete) {
					retractionType = SRBMetaDataSet.D_DELETE_METHOD_FOR_COLLECTION;
				}
				else {
					retractionType = SRBMetaDataSet.D_INSERT_METHOD_FOR_COLLECTION;
					dataValue1 = record.getStringValue(i);
				}
			}


/*TODO all together somehow?
			else if (record.getFieldName(i) == SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES)
			{
//colmnNumber@metadataIndex, newValue	for the data_name and the metadata row
//whose index number = metadataIndex,	the column is given the newValue.
				retractionType = SRBMetaDataSet.D_CHANGE_USER_DEFINED_STRING_META_DATA;
				dataValue1 = record.getStringValue(i);
			//@@@
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.colmnNumber,newValue
			for the  data_name  a new metadata row	is inserted and the columnNum is
			given the newValue. The return Value	is the metadataIndex of the row and
			can	be  used in performing)
			{
				retractionType = SRBMetaDataSet.D_INSERT_USER_DEFINED_STRING_META_DATA;
				dataValue1 = record.getStringValue(i);
			//@@@
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.metadataIndex (or -1) ,
			not_used	deletes the metadata row for data_name	whose  metadataIndex is
			given	if -1 is given then all metadata rows	are deleted for the data_name)
			{
				retractionType = SRBMetaDataSet.D_CHANGE_USER_DEFINED_STRING_META_DATA;
				dataValue1 = record.getStringValue(i);
			//@@@
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.metadataIndex (or -1) ,
			not_used	deletes the metadata row for data_name	whose  metadataIndex is
			given	if -1 is given then all metadata rows	are deleted for the data_name)
			{
				if (delete) {
					retractionType = SRBMetaDataSet.D_DELETE_USER_DEFINED_STRING_META_DATA;
				}
				else {
					retractionType = SRBMetaDataSet.D_CHANGE_USER_DEFINED_INTEGER_META_DATA;
					dataValue1 = record.getStringValue(i);
				}
			}


//TODO also related
			else if (record.getFieldName(i) == SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES)
			{
//metadataString,separator
				retractionType = SRBMetaDataSet.D_BULK_INSERT_UDEF_META_DATA_FOR_DATA;
				dataValue1 = record.getStringValue(i);
			//@@@
			}


*/

//*************************************************
//TODO maybe error if try to use this attribute with a dir, or reverse
			else if (record.getFieldName(i) == SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES)
			{

//metadataString,separator
/*???
  sprintf(atname[0],"data_id");
  sprintf(atname[1],"metadatanum");
  sprintf(atname[2],"userdef_metastr0");
  sprintf(atname[3],"userdef_metastr1");
  sprintf(atname[4],"userdef_metastr2");
  sprintf(atname[5],"userdef_metastr3");
  sprintf(atname[6],"userdef_metastr4");
  sprintf(atname[7],"userdef_metastr5");
  sprintf(atname[8],"userdef_metastr6");
  sprintf(atname[9],"userdef_metastr7");
  sprintf(atname[10],"userdef_metastr8");
  sprintf(atname[11],"userdef_metastr9");
  sprintf(atname[12],"userdef_metaint0");
  sprintf(atname[13],"userdef_metaint1");
  sprintf(atval[0],"%i",obj_data_id);
*/
				MetaDataTable table = record.getTableValue(i);
				dataValue2 = "|";
				for (int j=0;j<table.getRowCount();j++) {
					dataValue1 = "";
					int columns = table.getColumnCount();
					for (int k=0;k<columns;k++) {
						if (k == (columns - 1)) {
							dataValue1 += table.getStringValue(j,k);
						}
						else {
							dataValue1 += table.getStringValue(j,k) + dataValue2;
						}
					}
					if (vaultPathName == null) {
						srbFileSystem.srbModifyDataset( catalogType,
							fileName, getParent(), null, null, dataValue1,
							dataValue2,	SRBMetaDataSet.D_BULK_INSERT_UDEF_META_DATA_FOR_DATA);
					}
					else {
						srbFileSystem.srbModifyDataset( catalogType,
							getName(), getParent(), resource, vaultPathName, dataValue1,
							dataValue2, SRBMetaDataSet.D_BULK_INSERT_UDEF_META_DATA_FOR_DATA);
					}
				}
			}
			else if (record.getFieldName(i) ==
				SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES)
			{
//TODO maybe it works?
				MetaDataTable table = record.getTableValue(i);
				dataValue2 = "|";
				for (int j=0;j<table.getRowCount();j++) {
					dataValue1 = "";
					int columns = table.getColumnCount();
					for (int k=0;k<columns;k++) {
						if (k == (columns - 1)) {
							dataValue1 += table.getStringValue(j,k);
						}
						else {
							dataValue1 += table.getStringValue(j,k) + dataValue2;
						}
					}
					srbFileSystem.srbModifyCollect( catalogType, getAbsolutePath(),
						dataValue1, dataValue2,	dataValue3,
						SRBMetaDataSet.C_BULK_INSERT_UDEF_META_DATA_FOR_COLL );
				}
			}


/*TODO not really in API
			else if (record.getFieldName(i) == SRBMetaDataSet.same as for STRING)
			{
				if (delete) {
					retractionType =
						SRBMetaDataSet.D_DELETE_USER_DEFINED_INTEGER_META_DATA;
				}
				else
				{
					retractionType =
						SRBMetaDataSet.D_INSERT_USER_DEFINED_INTEGER_META_DATA;
					dataValue1 = record.getStringValue(i);
				}
			}
*/
			else if (record.getFieldName(i) == SRBMetaDataSet.DIRECTORY_ANNOTATION)
			{
//sourceColName, sourceDataName	annotation copied from source to data_name
				retractionType = SRBMetaDataSet.D_COPY_ANNOTATIONS_FROM_DATA_TO_DATA;
				dataValue1 = record.getStringValue(i);
			//@@@
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.CONTAINER_FOR_DIRECTORY)
			{
//oldContainerName, not_used	if the oldContainerName is used as a
//container forsome collection(s) then	the data_name	will replace it.
//(used internally by SRB server)
				retractionType = SRBMetaDataSet.D_UPDATE_CONTAINER_FOR_COLLECTION;
				dataValue1 = record.getStringValue(i);
			}


/*different class or method
			else if (record.getFieldName(i) == SRBMetaDataSet.)
			{
//newContainerCollName, newContainerDataName	copies access control
//(and some other metadata) from data_name to the new one.
//(used internally by SRB server)
				retractionType =
					SRBMetaDataSet.D_COPY_META_DATA_FROM_CONTAINER_TO_NEW_CONTAINER;
				dataValue1 = record.getStringValue(i);
			//@@@
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.)
			{
//sourceCollName, sourceDataName	copies user-defined metadata from	source to data_name.
				retractionType = SRBMetaDataSet.D_COPY_META_DATA_FROM_DATA_TO_DATA;
				dataValue1 = record.getStringValue(i);
			//@@@
			}
			else if (record.getFieldName(i) == SRBMetaDataSet.)
			{
//sourceCollName, not_used	copies user-defined metadata from	source to data_name.
				retractionType = SRBMetaDataSet.D_COPY_META_DATA_FROM_COLL_TO_DATA;
				dataValue1 = record.getStringValue(i);
			}
*/

//-----------------------
//SRB3.2
//----------------------
			else if (record.getFieldName(i) == SRBMetaDataSet.GUID)
			{
				if (delete) {
					retractionType = SRBMetaDataSet.D_DELETE_GUID;
				}
				else {
					//have to delete the old GUID
					if (vaultPathName == null) {
						//has replica number
						srbFileSystem.srbModifyDataset( catalogType, fileName, getParent(),
							null,	null,	dataValue1, dataValue2, SRBMetaDataSet.D_DELETE_GUID );
					}
					else {
						srbFileSystem.srbModifyDataset( catalogType, getName(), getParent(),
							resource,	vaultPathName,	dataValue1, dataValue2, SRBMetaDataSet.D_DELETE_GUID  );
					}
					retractionType = SRBMetaDataSet.D_INSERT_GUID;
					dataValue1 = record.getStringValue(i);
					dataValue2 = "1";
				 	/* guid_flag == 0 means it is made by SRB and hence can be
      			overwritten */
					/* guid_flag >= 1 means it is made by external generatort
						and hence cannot be overwritten only if data_value_2 is
						greater than the i*/

				}
			}



			if (retractionType >= 0) {
				if (isFile) {
					if (!dirsOnly) {
						if (vaultPathName == null) {
							//has replica number
							srbFileSystem.srbModifyDataset( catalogType, fileName,
								getParent(), null,	null,	dataValue1, dataValue2,
								retractionType );
						}
						else {
							srbFileSystem.srbModifyDataset( catalogType, getName(),
								getParent(), resource,	vaultPathName,	dataValue1, dataValue2,
								retractionType );
						}
					}
				}
				else if (!filesOnly) {
					srbFileSystem.srbModifyCollect( catalogType, getAbsolutePath(),
						dataValue1, dataValue2,	dataValue3, retractionType );
				}
			}
		}
	}



	/**
	 * Returns a string representation of this file object.
	 * The string is formated according to the SRB URI model.
	 * Note: the user password will not be included in the URI.
	 */
	public String toString( )
	{
		return new String( "srb://"+srbFileSystem.getUserName()+
			"."+srbFileSystem.getDomainName()+"@"+
			srbFileSystem.getHost()+":"+
			srbFileSystem.getPort() + getAbsolutePath() );
	}





	/**
	 * Copies this file to another file. This object is the source file.
	 * The destination file is given as the argument.
	 * If the destination file, does not exist a new one will be created.
	 * Otherwise the source file will be appended to the destination file.
	 * Directories will be copied recursively.
	 *<P>
	 * note: Files will be transferred using the SRB parallel transfer protocol.
	 *	However, appending a file cannot use the parallel copy method.
	 *	Also, the parallel method may be blocked by some firewalls,
	 *	see also SRBFileSystem.setFirewallPorts( int, int )
	 *
	 * @param file	The file to receive the data.
	 * @throws  NullPointerException If file is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyTo( GeneralFile file )
		throws IOException
	{
		copyTo( file, false );
	}



	/**
	 * Copies this file to another file. This object is the source file.
	 * The destination file is given as the argument.
	 * If the destination file, does not exist a new one will be created.
	 * Otherwise the source file will be appended to the destination file.
	 * Directories will be copied recursively.
	 *<P>
	 * note: Files will be transferred using the SRB parallel transfer protocol.
	 *	However, appending a file cannot use the parallel copy method.
	 *	Also, the parallel method may be blocked by some firewalls,
	 *	see also SRBFileSystem.setFirewallPorts( int, int )
	 *
	 * @param file The file to receive the data.
	 * @param forceOverwrite If the file exists, force it to be overwritten.
	 * 		If the file cannot be overwritten throw IOException.
	 * @throws  NullPointerException If file is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyTo( GeneralFile file, boolean forceOverwrite )
		throws IOException
	{
		copyTo( file, forceOverwrite, USE_BULKCOPY );
	}


	/**
	 * Copies this file to another file. This object is the source file.
	 * The destination file is given as the argument.
	 * If the destination file, does not exist a new one will be created.
	 * Otherwise the source file will be appended to the destination file.
	 * Directories will be copied recursively.
	 *<P>
	 * note: Files will be transferred using the SRB parallel transfer protocol.
	 *	However, appending a file cannot use the parallel copy method.
	 *	Also, the parallel method may be blocked by some firewalls,
	 *	see also SRBFileSystem.setFirewallPorts( int, int )
	 *
	 * @param file The file to receive the data.
	 * @param forceOverwrite If the file exists, force it to be overwritten.
	 * 		If the file cannot be overwritten throw IOException.
	 * @param bulkCopy If true, bulk copy: Default option, new ports may be
	 * 		opened. Files copied in parallel or bulk as appropriate. <br>
	 * 		If false, parallel copy: new ports may be opened, files copied in
	 * 		parallel but not in bulk.
	 * @throws  NullPointerException If file is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyTo( GeneralFile file, boolean forceOverwrite,
		boolean bulkCopy )
		throws IOException
	{
		if (file == null) {
			throw new NullPointerException();
		}

		//make sure all the parent directories exist
//java.io.File.mkdirs doesn't work with relative path
		FileFactory.newFile(
			file.getFileSystem(), file.getAbsolutePath() ).getParentFile().mkdirs();

		//Versions before SRB2.0 can't run parallel
		if (srbFileSystem.getVersionNumber() < 2 ) {
			super.copyTo( file, forceOverwrite );
			return;
		}


		if (isDirectory()) {
			if (( file instanceof LocalFile ) &&
				(srbFileSystem.getVersionNumber() >= 3 ) 
					&& bulkCopy 
					&& (USE_BULKCOPY || bulkCopy))
			{
				//if the directory to be copied contains files over MAX_BULK_FILE_SIZE
				//if is better to use parallel copy. The files can't be copied
				//some in bulk, some in parallel, like in copyFrom(). So instead
				//just query and if big files are found, copy them all in parallel.
				MetaDataCondition conditions[] = { MetaDataSet.newCondition(
					GeneralMetaData.DIRECTORY_NAME, MetaDataCondition.LIKE,
					getAbsolutePath()+"*" ) };
				MetaDataSelect selects[] = {
					MetaDataSet.newSelection( GeneralMetaData.SIZE ) };
				MetaDataRecordList rl[] = query( conditions, selects );
				if (rl != null) {
					do {
						for (int i=0;i<rl.length;i++) {
							if (rl[i].getIntValue(0) > MAX_BULK_FILE_SIZE) {
								bulkCopy = false;
								i = rl.length;
							}
						}
						rl = rl[0].getMoreResults();
					} while (rl != null && (bulkCopy && !rl[0].isQueryComplete()));
				}

				if (bulkCopy) {
					bulkUnload( (LocalFile) file );
					return;
				}
			}

			//recursive copy
			GeneralFile fileList[] = listFiles();

			file.mkdir();
			if (fileList != null) {
				for (int i=0;i<fileList.length;i++) {
					fileList[i].copyTo(
						FileFactory.newFile( file.getFileSystem(), file.getAbsolutePath(),
							fileList[i].getName()), forceOverwrite );
				}
			}
		}
		else if (isFile()) {
			if (file.isDirectory()) {
				//change the destination from a directory to a file
				file = FileFactory.newFile( file, getName() );
			}

			if (!forceOverwrite && file.exists()) {
//TODO the source file will be appended to the destination file in serial
				super.copyTo( file, forceOverwrite );
				return;
			}

			if ( file instanceof LocalFile ) {
				int numThreads = (int) length()/MIN_THREAD_SIZE;
				if (numThreads > MAX_NUMBER_OF_THREADS)
					numThreads = MAX_NUMBER_OF_THREADS;

				if (srbFileSystem.getVersionNumber() >= 3 ) {
					srbFileSystem.srbObjGetClientInitiated(
						fileName, getParent(), file, 0, numThreads );
				}
				else {
					//too many firewall problems, not that much faster.
					srbFileSystem.srbObjGet(
						fileName, getParent(), length(), file, forceOverwrite );
				}
			}
			else if ( file instanceof SRBFile ) {
				//have to just delete it or it will error
				//already know file doesn't exist in case of !overwrite,
				//so delete() doesn't matter
				if (forceOverwrite)
					file.delete();

				SRBRandomAccessFile in = new SRBRandomAccessFile(
					new SRBFile(srbFileSystem, getAbsolutePath()), "r" );

				SRBRandomAccessFile out = new SRBRandomAccessFile(
					new SRBFile(srbFileSystem, ((SRBFile) file).getAbsolutePath()), "rw" );

				srbFileSystem.srbObjProxyOpr( OPR_COPY, in.getFD(), out.getFD(), 0, 0,
					null, null, null, null, null, null, null );

				in.close();
				out.close();
			}
			else {
				//TODO this might work in parallel?
				super.copyTo( file, forceOverwrite );
			}
		}
		else {
			throw new FileNotFoundException( "Source file is not valid: "+ this );
		}
	}

	/**
	 * Copies this file to another file. This object is the source file.
	 * The destination file is given as the argument.
	 * If the destination file, does not exist a new one will be created.
	 * Otherwise the source file will be appended to the destination file.
	 * Directories will be copied recursively.
	 *<P>
	 * note: Files will be transferred using the SRB parallel transfer protocol.
	 *	However, appending a file cannot use the parallel copy method.
	 *	Also, the parallel method may be blocked by some firewalls,
	 *	see also SRBFileSystem.setFirewallPorts( int, int )
	 *
	 * @param file	The file to receive the data.
	 * @throws  NullPointerException If file is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyFrom( GeneralFile file )
		throws IOException
	{
		copyFrom( file, false );
	}

	/**
	 * Copies this file to another file. This object is the source file.
	 * The destination file is given as the argument.
	 * If the destination file, does not exist a new one will be created.
	 * Otherwise the source file will be appended to the destination file.
	 * Directories will be copied recursively.
	 *<P>
	 * note: Files will be transferred using the SRB parallel transfer protocol.
	 *	However, appending a file cannot use the parallel copy method.
	 *	Also, the parallel method may be blocked by some firewalls,
	 *	see also SRBFileSystem.setFirewallPorts( int, int )
	 *
	 * @param file	The file to receive the data.
	 * @throws  NullPointerException If file is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyFrom( GeneralFile file, boolean forceOverwrite )
		throws IOException
	{
		copyFrom( file, false, USE_BULKCOPY );
	}

	/**
	 * Copies this file to another file. This object is the source file.
	 * The destination file is given as the argument.
	 * If the destination file, does not exist a new one will be created.
	 * Otherwise the source file will be appended to the destination file.
	 * Directories will be copied recursively.
	 *<P>
	 * note: Files will be transferred using the SRB parallel transfer protocol.
	 *	However, appending a file cannot use the parallel copy method.
	 *	Also, the parallel method may be blocked by some firewalls,
	 *	see also SRBFileSystem.setFirewallPorts( int, int )
	 *
	 * @param file	The file to receive the data.
	 * @param bulkCopy If true, bulk copy: Default option, new ports may be
	 * 		opened. Files copied in parallel or bulk as appropriate. <br>
	 * 		If false, parallel copy: new ports may be opened, files copied in
	 * 		parallel but not in bulk.
	 * @throws  NullPointerException If file is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyFrom( GeneralFile file, boolean forceOverwrite,
		boolean bulkCopy )
		throws IOException
	{
//super.copyFrom( file, forceOverwrite );
//if(true) return;

		if (file == null) {
			throw new NullPointerException();
		}

		//make sure all the parent directories exist
		getParentFile().mkdirs();

		//Versions before SRB2.0 can't run parallel
		if (srbFileSystem.getVersionNumber() < 2 ) {
				super.copyFrom( file, forceOverwrite );
				return;
		}

		if (file.isDirectory()) {
			if ((file instanceof LocalFile) &&
				(srbFileSystem.getVersionNumber() >= 3 ) 
					&& bulkCopy
					&& (USE_BULKCOPY || bulkCopy))
			{
				//This somewhat confusing situation matches the behavior of the
				//commandline 'cp' command
				LocalFile[] files = null;
				if (isDirectory()) {
					files = new LocalFile[1];
					files[0] = (LocalFile) file;
				}
				else {
					files = (LocalFile[]) file.listFiles();
				}

				bulkLoad( files, forceOverwrite );
			}
			else {
				//recursive copy
				GeneralFile fileList[] = file.listFiles();

				mkdir();
				if (fileList != null) {
					for (int i=0;i<fileList.length;i++) {
						FileFactory.newFile( this, fileList[i].getName() ).copyFrom(
							fileList[i], forceOverwrite );
					}
				}
			}
		}
		else if (file.isFile()) {
			if (isDirectory()) {
				//change the destination from a directory to a file
				GeneralFile subFile = FileFactory.newFile( this, file.getName() );
				subFile.copyFrom( file );
				return;
			}

			if (!forceOverwrite && exists()) {
				super.copyFrom( file, forceOverwrite );
				return;
			}

			if ( file instanceof LocalFile ) {
				if (forceOverwrite) {
					delete();
				}

				long value = srbFileSystem.srbObjPut( getName(), getParent(),
					resource, null,	null, file.getAbsolutePath(), file.length() );
/*
long value = srbFileSystem.srbObjPutClientInitiated( destObjID, destCollection,
	destResLoc, dataType, destPath, localFilePath, srcSize, forceFlag,
	numThreads );
*/
//System.out.println(value);
				if (value == SRBCommands.MSG_USE_SINGLE_PORT) {
					//server doesn't allow opening new ports
					super.copyFrom( file, forceOverwrite );
				}
				else if (value < 0) {
					throw new SRBException( "", (int)	value );
				}
			}
			else if ( file instanceof SRBFile ) {
				//have to just delete it or it will error
				//already know file doesn't exist in case of !overwrite,
				//so delete() doesn't matter
				if (forceOverwrite)
					delete();

				SRBRandomAccessFile in = new SRBRandomAccessFile(
					new SRBFile(srbFileSystem, ((SRBFile) file).getAbsolutePath()), "r" );

				SRBRandomAccessFile out = new SRBRandomAccessFile(
					new SRBFile(srbFileSystem, getAbsolutePath()), "rw" );

				srbFileSystem.srbObjProxyOpr( OPR_COPY, in.getFD(), out.getFD(), 0, 0,
					null, null, null, null, null, null, null );

				in.close();
				out.close();
			}
			else {
				super.copyFrom( file, forceOverwrite );
			}
		}
		else {
			throw new FileNotFoundException( "Source File is not valid: "+ file );
		}
	}


//----------------------------------------------------------------------
// RemoteFile Methods
//----------------------------------------------------------------------
	/**
	 * Copies a local file to a SRB file on the SRB file server.
	 * If the SRB file doesn't exist, a new one will be created.
	 * Otherwise the results are unspecified.
	 * Directories will be copied recursively.
	 * <P>
	 *
	 * @deprecated Superseded by copyFrom( GeneralFile ). This single new
	 * method replaces all the other older copyFrom...() methods.
	 * It functions in the same manner as those old methods, but
	 * can except any GeneralFile. It also fixes some bugs in those methods.
	 * @see #copyFrom( GeneralFile )
	 * @param localFilePath	The local file path of the file to be copied.
	 * @throws  NullPointerException If localFile is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyFromLocal( String localFilePath )
		throws IOException
	{
		copyFrom( new LocalFile( localFilePath ) );
	}


	/**
	 * Copies a local file to a SRB file on the SRB file server.
	 * If the SRB file doesn't exist, a new one will be created.
	 * Otherwise the results are unspecified.
	 * Directories will be copied recursively.
	 *
	 * @deprecated Superseded by copyFrom( GeneralFile ). This single new
	 * method replaces all the other older copyFrom...() methods.
	 * It functions in the same manner as those old methods, but
	 * can except any GeneralFile. It also fixes some bugs in those methods.
	 * @see #copyFrom( GeneralFile )
	 * @param localFile	The file to be copied.
	 * @throws  NullPointerException If localFile is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyFromLocal( LocalFile localFile )
		throws IOException
	{
		copyFrom( localFile );
	}


	/**
	 * Copies the SRB file to a local file.
	 * If the local file does not exist, a new one will be created.
	 * Otherwise the results are unspecified.
	 * Directories will be copied recursively.
	 * <P>
	 * @deprecated Superseded by copyTo( GeneralFile ). This single new
	 * method replaces all the other older copyTo...() methods.
	 * It functions in the same manner as those old methods, but
	 * can except any GeneralFile. It also fixes some bugs in those methods.
	 * @see #copyTo( GeneralFile )
	 * @param localFile	The file to be copied.
	 * @throws  NullPointerException If localFile is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyToLocal( String localFilePath )
		throws IOException
	{
		copyTo( new LocalFile( localFilePath ) );
	}


	/**
	 * Copies the SRB file to a local file.
	 * If the local file does not exist, a new one will be created.
	 * Otherwise the results are unspecified.
	 * Directories will be copied recursively.
	 * <P>
	 * @deprecated Superseded by copyTo( GeneralFile ). This single new
	 * method replaces all the other older copyTo...() methods.
	 * It functions in the same manner as those old methods, but
	 * can except any GeneralFile. It also fixes some bugs in those methods.
	 * @see #copyTo( GeneralFile )
	 * @param localFile	The file to receive the data.
	 * @throws  NullPointerException If localFile is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyToLocal( LocalFile localFile )
		throws IOException
	{
		copyTo( localFile );
	}


	/**
	 * Copies this file to a new location on the server.
	 * If the <code>remoteFile</code> does not exist, one will be created.
	 * Otherwise the results are unspecified.
	 * Directories will be copied recursively.
	 * <P>
	 * @deprecated Superseded by copyTo( GeneralFile ). This single new
	 * method replaces all the other older copyTo...() methods.
	 * It functions in the same manner as those old methods, but
	 * can except any GeneralFile. It also fixes some bugs in those methods.
	 * @see #copyTo( GeneralFile )
	 * @param remoteFile	The local file to receive the data.
	 * @throws NullPointerException If remoteFile is null.
	 * @throws IOException If an IOException occurs.
	 */
	public void copyToRemote( RemoteFile remoteFile )
		throws IOException
	{
		copyTo( remoteFile );
	}




//----------------------------------------------------------------------
// SRBFile Methods
//----------------------------------------------------------------------
	/**
	 * The number of bytes transfered so far by a currently executing
	 * SRBFile.copyTo/copyFrom command.
	 *
	 * @return the number of bytes that have been transfered so far.
	 */
	public long fileCopyStatus( )
	{
		return srbFileSystem.fileCopyStatus();
	}

	/**
	 * Retrieves the platform independent stat structure.
	 * This is basically the POSIX stat definition.
	 *<P>
	 * Returns:<br>
	 * st_size = 0. File size in bytes (long).<br>
	 * st_dev = 1. Device.<br>
	 * st_ino = 2. Inode.<br>
	 * st_mode = 3. File mode.<br>
	 * st_nlink = 4. Number of links.<br>
	 * st_uid = 5. User ID of the file's owner.<br>
	 * st_gid = 6. Group ID of the file's group.<br>
	 * st_rdev = 7. ID of device.<br>
	 * 		    This entry is defined only for character or block special files.<br>
	 * st_atim = 8. Time of last access.<br>
	 * 		  Use st_atim instead of st_atime because in solaris, st_atime is<br>
	 * 		  defined to be somthing else.<br>
	 * st_spare1 = 9.<br>
	 * st_mtime = 10. Time of last data modification.<br>
	 * st_spare2 = 11.<br>
	 * st_ctim = 12. Time of last file status change.<br>
	 * st_spare3 = 13.<br>
	 * 		 Time measured in seconds since 00:00:00 GMT, Jan. 1, 1970.<br>
	 * st_blksize = 14. Optimal blocksize for file system i/o ops.<br>
	 * st_blocks = 15. Actual number of blocks allocated in DEV_BSIZE blocks.<br>
	 * st_vfstype = 16. Type of fs (see vnode.h).<br>
	 * st_vfs = 17. Vfs number.<br>
	 * st_type = 18. Vnode type.<br>
	 * st_gen = 19. Inode generation number.<br>
	 * st_flag = 20. Flag word.<br>
	 * Reserved1 = 21. Reserved.<br>
	 * Reserved2 = 22. Reserved.<br>
	 * st_access = 23. Process' access to file.<br>
	 * st_dummy = 24. pat to 32 bit boundary.<br>
	 * st_spare4 = 25+. Reserved.
	 *
	 * @throws IOException If an IOException occurs.
	 */
	// see also,	the SRB C client back/srbStat.h
	public long[] getStat() throws IOException
	{
		long stat[] = null;

		if (!exists()) return null;

		if (isDirectory()) {
			//@param myType	file or dir state : IS_UNKNOWN, IS_FILE, IS_DIR_1,
			//			IS_DIR_2, IS_DIR_3, IS_DIR_4.
			stat = srbFileSystem.srbObjStat(
				0, getAbsolutePath(), 3 );//IS_DIR_3?
		}
		else {
			stat = srbFileSystem.srbObjStat(
				0, getAbsolutePath(), 0 );
		}

		if ( stat != null ) {
/*TODO maybe can use this stuff
			long st_size = stat[0]; // File size in bytes (long)

			long st_dev = stat[1]; // Device
			long st_ino = stat[2]; // Inode


			long st_mode = stat[3]; // File mode

			long st_nlink = stat[4]; // Number of links
			long st_uid = stat[5];  // User ID of the file's owner
			long st_gid = stat[6];  // Group ID of the file's group
			long st_rdev = stat[7]; // ID of device
				 //   This entry is defined only for
				 //   character or block special files
			long st_atim = stat[8]; // Time of last access
				 // Use st_atim instead of st_atime
				 // because in solaris, st_atime is
				 // defined to be somthing else
			long st_spare1 = stat[9];


			long st_mtime = stat[10]; // Time of last data modification

			long st_spare2 = stat[11];
			long st_ctim = stat[12]; // Time of last file status change
			long st_spare3 = stat[13];
				// Time measured in seconds since
				//   00:00:00 GMT, Jan. 1, 1970

			long st_blksize = stat[14]; // Optimal blocksize for file system
					 //i/o ops
			long st_blocks = stat[15]; // Actual number of blocks allocated
					 //in DEV_BSIZE blocks
			long st_vfstype = stat[16]; // Type of fs (see vnode.h)
			long st_vfs = stat[17];  // Vfs number
			long st_type = stat[18]; // Vnode type
			long st_gen = stat[19];  // Inode generation number
			long st_flag = stat[20]; // Flag word

			long Reserved1 = stat[21]; // Reserved
			long Reserved2 = stat[22]; // Reserved
			long st_access = stat[23];      // Process' access to file
			long st_dummy = stat[24];       // pat to 32 bit boundary
			long st_spare4[] = new long[25];
			for (int i=0;i<st_spare4.length;i++)
				st_spare4[i] = stat[i+25];   // Reserved
*/ return stat;
		}
		else
			throw new IOException( "Unable to obtain file stat" );
	}



//start bulkLoad methods/classes
IOException bulkLoadException = null;

//TODO being lazy here's some unnecessarily global variables.
Vector parallelSourceFiles;
Vector parallelDestination;//have to be SRBFile

/** TODO still working on it. not sure where I'm going to put it.
	 */
	/**
	 * Loads the local files on to the SRB in this container in the
	 * <code>srbDirectory</code>.
	 *
	 */
//Had to go through a few iterations to get this right, probably some
//cruft or optimization needed.

//everything not without a tab before it, that is the code that starts
//on the left margin is for debugging or questionable.
	void bulkLoad( LocalFile[] files, boolean forceOverwrite )
		throws IOException
	{
/*
which container or resource, get size and MaxSize of container
open container or temp file for upload
create two 8MB byte buffers
create new threads
main thread starts loading local files into buffer
	-one buffer gets filled well the other buffer is used in transfer
also loads myResultStruct (upto 300 files)
	-somehow->dataName,collectionName,offset
	-once full create registration thread
make sure not to overflow container

only one transfer thread
waits on buffer to have data
	-main gives signal
check enough space, otherwise create new container
uses regular srbobjwrite
udates status and wait again

upto 4 registration threads
*/

		long maxSize = -1;
		long containerOffset = 0;

		//Two buffers are used switching back and forth
		//from loading to transfering files
		byte[] buffer1 = new byte[BULK_LOAD_BUFFER_SIZE];
		byte[] buffer2 = new byte[BULK_LOAD_BUFFER_SIZE];


		//the temporary files to receive the data during transfer
		GeneralFile[] tempFile = new GeneralFile[MAX_NUMBER_OF_THREADS];
		GeneralRandomAccessFile[] raf =
			new GeneralRandomAccessFile[MAX_NUMBER_OF_THREADS];


		//one transfer thread
		Thread loadThread = null;
		LoadThread load = null;

		parallelSourceFiles = new Vector();
		parallelDestination = new Vector();

		//if this object isn't a dir, make it one.
		mkdir();


		//create a temp file for each registration thread
//TODO only as needed
//TODO seriously figure out how to only open as needed.
		for (int i=0;i<MAX_NUMBER_OF_THREADS;i++) {
			tempFile[i] = SRBFile.createTempFile(
				"BLoad", ".tmp", this );
//System.out.println(tempFile[i]);
			raf[i] = FileFactory.newRandomAccessFile( tempFile[i], "rw" );
		}


		//threads to tell MCAT about the metadata of the files.
		RegistrationThread[] registration =
			new RegistrationThread[MAX_NUMBER_OF_THREADS];
		Thread[] registrationThreads = new Thread[MAX_NUMBER_OF_THREADS];

		for (int i=0;i<MAX_NUMBER_OF_THREADS;i++) {
			registration[i] =	new RegistrationThread(
				tempFile[i].getAbsolutePath(), this );
		}
//TODO only as needed
registrationThreads[0] = new Thread( registration[0] );
/*
		for (int i=0;i<MAX_NUMBER_OF_THREADS;i++) {
			registrationThreads[i] = new Thread(registration[i]);
		}
*/


		//thread to transfer all the data
//TODO use parallel put?
		load = new LoadThread( raf, buffer1, buffer2, this );
		loadThread = new Thread( load );
		loadThread.start();

    // Now start copying
		for ( int i=0;i<files.length;i++ ) {
//TODO remove loadThread
		loadBuffer( files[i], load, loadThread, tempFile, registrationThreads,
			registration, null );
		}

		synchronized ( this )
		{
			// don't need to wait if this code is not busy.
			while ( (load.buffer1Length != 0) || (load.buffer2Length != 0) )
			{
				try
				{
					loadThread.interrupt();
					this.wait();
				}
				catch ( InterruptedException e )
				{

				}
			}
		}
		load.keepLoading = false;

		try {
			//end of load thread
			if ( loadThread.isAlive() ) {
				loadThread.join();
			}

			//close, register, then delete all remaining temp files
			for ( int i=0;i<MAX_NUMBER_OF_THREADS;i++ ) {
				raf[i].close();
				if ( registration[i] != null ) {
//System.out.println("activeRegistrationThread.bloadFilePath "+registration[i].bloadFilePath);
//TODO HACK
//The last bulkFile wasn't getting its filepath set.
//Causing a -3201 DATA_NOT_IN_CAT srb error, when the old bulkFile
//(that had already been registered) attempted to get re-registered
if (new SRBFile(srbFileSystem, registration[i].bloadFilePath).exists()) {
					registration[i].register();
}
else {
	registration[i].bloadFilePath = tempFile[i].getAbsolutePath();
	registration[i].register();
}
				}
				((SRBFile)tempFile[i]).delete(true);
			}

			//end of registration threads
			for ( int i=0;i<MAX_NUMBER_OF_THREADS;i++ ) {
				if ( registrationThreads[i] != null ) {
					if ( registrationThreads[i].isAlive() ) {
						registrationThreads[i].join();
					}
				}
			}
		}
		catch(InterruptedException e) {
//TODO
		}

		//large files copied in parallel after the bulk load
		if (parallelSourceFiles.size() > 0) {
			for (int i=0;i<parallelSourceFiles.size();i++) {
//System.out.println("dest "+(GeneralFile) parallelDestination.get(i));
//System.out.println("source "+(GeneralFile) parallelSourceFiles.get(i));
				((SRBFile) parallelDestination.get(i)).copyFrom(
					((GeneralFile) parallelSourceFiles.get(i)), forceOverwrite, false );
			}
		}

//TODO because they are silly global...
parallelSourceFiles = null;
parallelDestination = null;
	}

//true, if currently loading buffer1
private volatile boolean loading1 = false;

//0=not transfering, 1=transfering buffer1, 2=transfering buffer2
private volatile int transfer = 0;


int activeRegistrationThread = 0;

	//loads a file (or a directory recursively)
	//into the buffer for transfer to the SRB.
	private void loadBuffer( GeneralFile file, LoadThread load,
		Thread loadThread, GeneralFile[] tempFile, Thread[] registrationThreads,
		RegistrationThread[] registration, String relativePath )
		throws IOException
	{
		if ( file == null ) {
			return;
		}
//TODO srb throws an error that should be removed.
//TODO update, was causing problems,
//	don't need this bulkLoadException business anymore?
if (bulkLoadException != null) {
//System.out.println("bulkLoadException "+bulkLoadException);
	throw bulkLoadException;
	//bulkLoadException.printStackTrace();
//bulkLoadException=null;
}
//System.out.println("localFile "+file);

		//recursively loads the directory,
		//unfortunately this means a lot of logic has to be in this method
		//instead of bulkLoad(). Which explains all the method variables.
		if ( file.isDirectory() ) {
			//this relativePath variable keeps track of the matching of the
			//local directory and the SRB directory. It is keeps the subdirectory
			//structure but keeps the files from being registered further up the
			//tree than this SRBFile object.
			//Solves the problem of if they use a relative path like
			//../../../../../../src and a SRB bulkLoad dir of /home/testuser
			if ( (relativePath == null) || (relativePath.equals("")) ) {
				relativePath = file.getName();
			}
			else {
				relativePath += PATH_SEPARATOR + file.getName();
			}

			GeneralFile[] files = file.listFiles();
			if (files == null)
				return;

			for (int i=0;i<files.length;i++) {
				loadBuffer( files[i], load, loadThread, tempFile,
					registrationThreads, registration, relativePath );
			}
		}
		else if (file.length() > MAX_BULK_FILE_SIZE) {
			//copy files larger then 2MB in parallel. after the bulkload finishes


			//keeps the subdirectory structure
			if ( (relativePath == null) || (relativePath.equals("")) ) {
				relativePath = file.getName();
			}
			else {
				relativePath += PATH_SEPARATOR + file.getName();
			}

//System.out.println("parallelcopy"+file );
			//copy into proper directory
			parallelSourceFiles.add( file );
			parallelDestination.add( FileFactory.newFile( this, relativePath ) );
		}
		else {
			GeneralRandomAccessFile readFile = null;
			try {
				readFile = FileFactory.newRandomAccessFile( file, "r" );
			} catch ( SecurityException e ) {
				//TODO better to ignore or to error?
e.printStackTrace();
				return;
			}
			long toRead = file.length();
			int temp = 0;

//System.out.println("loadBuffer before toRead "+toRead+" "+transfer+" "+loading1);
			while (toRead > 0) {
				synchronized ( this ) {
					//have to switch back and forth in loading, but not load
					//a buffer that is being transfered
					while ( (loading1 && (transfer == 1)) ||
						(!loading1 && (transfer == 2)) )
					{
						try	{
							this.wait();
						}	catch ( InterruptedException e ) { }
					}
				}
//System.out.println("loading1 "+loading1);
				if (loading1) {
					if ( (toRead+load.buffer1Length) < BULK_LOAD_BUFFER_SIZE ) {
//System.out.println("offset1 "+load.buffer1Length);
//System.out.println("offset1 "+readFile.length());
						//read [the last of] this file into this buffer
						temp = readFile.read(
							load.buffer1, 0, (int) toRead );
						toRead -= temp;
						load.buffer1Length += temp;
//System.out.println("offset1.temp "+temp);
					}
					else if ( load.buffer1Length < BULK_LOAD_BUFFER_SIZE ) {
//System.out.println("offset2 "+load.buffer1Length);
//System.out.println("offset2 "+readFile.length());
						//fill this buffer
						temp = readFile.read( load.buffer1, 0,
							BULK_LOAD_BUFFER_SIZE-load.buffer1Length );
						load.buffer1Length += temp;
						toRead -= temp;
//System.out.println("offset2.temp "+temp);

//TODO data gets lost without these 2nd loads...
						//Loads the rest of the file in the other buffer
						synchronized ( this ) {
							while ( transfer == 2 ) {
								try	{
									this.wait();
								}	catch ( InterruptedException e ) { }
							}
						}
						//if possible, load the remainder of the file into the other buffer
						if ( toRead < BULK_LOAD_BUFFER_SIZE ) {
//System.out.println("offset3 "+load.buffer2Length);
//System.out.println("offset3 "+readFile.length());
							temp = readFile.read(	load.buffer2, 0, (int) toRead );
							load.buffer2Length += temp;
							toRead -= temp;
//System.out.println("offset3.temp "+temp);
						}
					}
					else {
						synchronized ( this ) {
							while ( transfer == 2 ) {
								try	{
									this.wait();
								}	catch ( InterruptedException e ) { }
							}
						}

						//if possible, load the remainder of the file into the other buffer
						if ( toRead < BULK_LOAD_BUFFER_SIZE ) {
//System.out.println("offset4 "+load.buffer2Length);
//System.out.println("offset4 "+readFile.length());
							temp = readFile.read( load.buffer2, 0, (int) toRead );
							load.buffer2Length += temp;
							toRead -= temp;
//System.out.print("offset4.temp "+temp);
						}
					}
				}
				else {
					if ((toRead+load.buffer2Length) < BULK_LOAD_BUFFER_SIZE) {
//System.out.println("offset5 "+load.buffer2Length);
//System.out.println("offset5 "+readFile.length());
						//read [the last of] this file into this buffer
						temp = readFile.read( load.buffer2, 0, (int) toRead );
						load.buffer2Length += temp;
						toRead -= temp;
//System.out.println("offset5.temp "+temp);
					}
					else if (load.buffer2Length < BULK_LOAD_BUFFER_SIZE) {
//System.out.println("offset6 "+load.buffer2Length);
//System.out.println("offset6 "+readFile.length());
						//fill this buffer
						temp = readFile.read( load.buffer2, 0,
							BULK_LOAD_BUFFER_SIZE-load.buffer2Length );
						load.buffer2Length += temp;
						toRead -= temp;
//System.out.println("offset6.temp "+temp);

						synchronized ( this ) {
							while (transfer == 1) {
								try	{
									this.wait();
								}	catch ( InterruptedException e ) { }
							}
						}
						//if possible, load the remainder of the file into the other buffer
						if ( toRead < BULK_LOAD_BUFFER_SIZE ) {
//System.out.println("offset7 "+load.buffer1Length);
//System.out.println("offset7 "+readFile.length());
							temp = readFile.read( load.buffer1, 0, (int) toRead );
							load.buffer1Length += temp;
							toRead -= temp;
//System.out.println("offset7.temp "+temp);
						}
					}
					else {
						synchronized ( this ) {
							while ( transfer == 1 ) {
								try	{
									this.wait();
								}	catch ( InterruptedException e ) { }
							}
						}
						//if possible, load the remainder of the file into the other buffer
						if ( toRead < BULK_LOAD_BUFFER_SIZE ) {
//System.out.println("offset8 "+load.buffer1Length);
//System.out.println("offset8 "+readFile.length());
							temp = readFile.read( load.buffer1, 0, (int) toRead);
							load.buffer1Length += temp;
							toRead -= temp;
//System.out.println("offset8.temp "+temp);
						}
					}
				}
			}
//System.out.println("loadBuffer after toRead");
			readFile.close();
//System.out.println("loadBuffer close");


//TODO need a wait if all four threads are full&in use
			filesReadyToRegister++;

			//(when not using containers) files can't be registered until
			//after they have been uploaded. this adds the file to the current
			//registration thread list.
			if ( relativePath == null ) {
				registration[activeRegistrationThread].addFile( file, "" );
			}
			else {
				registration[activeRegistrationThread].addFile( file, relativePath );
			}

//System.out.println("loadBuffer filesReadyToRegister "+filesReadyToRegister);
			//every MAX_REGISTRATION_FILES
			if ( (filesReadyToRegister % MAX_REGISTRATION_FILES) == 0 )
			{
				synchronized ( this ) {
					// don't need to wait if this code is not busy.
					while ( (load.buffer1Length != 0) && (load.buffer2Length != 0) ) {
						try	{
							loadThread.interrupt();
							this.wait();
						} catch ( InterruptedException e ) { }
					}
				}

				if (registrationThreads[activeRegistrationThread] == null) {
					registrationThreads[activeRegistrationThread] =
						new Thread(registration[activeRegistrationThread]);
				}
				else {
					while ( registrationThreads[activeRegistrationThread].isAlive() ) {
						//TODO wait();
						//with 4+ threads this probably won't ever happen.
					}

					registrationThreads[activeRegistrationThread] =
						new Thread(registration[activeRegistrationThread]);
				}
				registration[activeRegistrationThread].setBloadFilePath(
					tempFile[activeRegistrationThread].getAbsolutePath() );
				registrationThreads[activeRegistrationThread].start();
//System.out.println("main thread pauses for registration1");

				//new tempFile since the old got closed and is busy being registered.
				tempFile[activeRegistrationThread] = SRBFile.createTempFile(
					"BLoad", ".tmp", tempFile[activeRegistrationThread].getParentFile() );
				load.restartRandomAccessFile( activeRegistrationThread,
					tempFile[activeRegistrationThread] );

//System.out.println("main thread pauses for registration2");
				//inc. to next registration thread
				activeRegistrationThread++;
				if (activeRegistrationThread >= MAX_NUMBER_OF_THREADS) {
					activeRegistrationThread = 0;
				}
			}
//System.out.println("loadBuffer filesReadyToRegister after ");
		}
	}


	/**
	 * Uses two buffers to upload data to the SRB, switching back and forth
	 * transfering the full buffer while the main thread loads the other buffer.
	 */
	class LoadThread implements Runnable
	{
		GeneralRandomAccessFile[] out;
		byte[] buffer1;
		byte[] buffer2;
		boolean writeBuffer1 = false;
		boolean writeBuffer2 = false;
		int buffer1Length = 0;
		int buffer2Length = 0;
		boolean keepLoading = true;

		//keep thread monitor
		Object mainThread;

		LoadThread( GeneralRandomAccessFile[] out, byte[] buffer1, byte[] buffer2,
			Object mainThread )
		{
			this.out = out;
			this.buffer1 = buffer1;
			this.buffer2 = buffer2;
			this.mainThread = mainThread;
		}

    public void run( )
    {
			try {
				//keep this thread running until all the files are uploaded
				while ( keepLoading ) {
					sendBuffer();
				}
			} catch ( IOException e ) {
e.printStackTrace();
				bulkLoadException = e;
			}
    }

		/**
		 * switch to a new file so the old file can be registered.
		 * only have MAX_NUMBER_OF_THREADS open at a time, when one
		 * fills up with MAX_REGISTRATION_FILES, the temp file must be closed
		 * to allow it to be registered. Need to open a new one to keep loading
		 * data.
		 */
		void restartRandomAccessFile( int fileIndex, GeneralFile tempFile )
			throws IOException
		{
//TODO file doesn't exist after bulkLoad
//			out[fileIndex].close();
//System.out.println("restartRandomAccessFile");
			out[fileIndex] = FileFactory.newRandomAccessFile( tempFile, "rw" );
		}

    void sendBuffer( )
    	throws IOException
		{
			//wait until one of the buffers has data
			if ( (buffer1Length == 0) && (buffer2Length == 0) ) { //TODO
//TODO
////System.out.print("wait ");
try {
//Thread.sleep(10);
} catch (Exception e) {
	e.printStackTrace();
}

return;
			}

			synchronized( mainThread )
			{
				//tell the main thread to load the other buffer
				loading1 = !loading1;

				//don't let the main thread start loading the other buffer until
				//it has finished being transfered.
				if (loading1)
					transfer = 1;
				else
					transfer = 2;
			}
//if ((buffer1Length > 100) || (buffer2Length > 100)) {
			//if not loading buffer 1, then transfer buffer1
			if ( !loading1 ) {
//System.out.print(" send1 "+ buffer1Length +" ");
////System.out.println("buffer1 "+ new String(buffer1));
				out[activeRegistrationThread].write(buffer1, 0, buffer1Length);
				buffer1Length = 0;
			}
			else  {
//System.out.print(" send2 "+ buffer2Length +" ");
////System.out.println("buffer2 "+ new String(buffer2));
				out[activeRegistrationThread].write(buffer2, 0, buffer2Length);
				buffer2Length = 0;
			}
//}
			synchronized( mainThread )
			{
				transfer = 0;
				//wake up the main thread if it is waiting for a buffer to empty
				mainThread.notify();
			}
    }
	}

//TODO not global?
	//and which still need to be.
	int filesReadyToRegister = 0;


	/**
	 * One of a number of threads that tell metadata to the SRB-MCAT.
	 */
	class RegistrationThread implements Runnable
	{
		//directory to load into on the SRB
		String bloadFilePath;

		//keep thread monitor
		Object mainThread;

		//list of files to be registered
		Vector files = new Vector();
		//list of SRB relative paths of those files
		Vector paths = new Vector();

		RegistrationThread( String bloadFilePath,	Object mainThread )
    	throws IOException
		{
//System.out.println("RegistrationThread "+bloadFilePath);
			this.bloadFilePath = bloadFilePath;
			this.mainThread = mainThread;
		}

		void setBloadFilePath( String bloadFilePath )
		{
//System.out.println("setBloadFilePath "+bloadFilePath);
			this.bloadFilePath = bloadFilePath;
		}

		void addFile( GeneralFile file, String relativePath )
		{
			files.add(file);
			paths.add(relativePath);
		}

    public void run( )
    {
			try {
    		register();
			} catch (IOException e) {
e.printStackTrace();
				bulkLoadException = e;
			}
    }

		//get the files that are ready to be registered
		SRBMetaDataRecordList[] getFileRegistry(int numFiles)
			throws IOException
		{
			SRBMetaDataRecordList rl = null;
			SRBMetaDataRecordList[] recordLists =
				new SRBMetaDataRecordList[numFiles];

			GeneralFile tempFile = null, tempFile2 = null;
			String tempName = null, tempName2 = null;

			String dirName = null; //TODO or SRBParentFile???
			long size = 0;
			int offset = 0;//TODO ???

			//create the registry for the files loaded so far.
			for (int i=0;i<numFiles;i++) {
				tempFile = (GeneralFile) files.get(i);
				offset += size;//TODO ???
				size = tempFile.length();
				dirName = (String) paths.get(i);
				if ((dirName == null) || (dirName.equals(""))) {
					dirName = getAbsolutePath();
				}
				else {
					dirName = getAbsolutePath() + PATH_SEPARATOR + dirName;
				}

				rl = new SRBMetaDataRecordList(
					SRBMetaDataSet.getField( SRBMetaDataSet.FILE_NAME ),
						tempFile.getName());
				rl.addRecord(
					SRBMetaDataSet.getField( SRBMetaDataSet.DIRECTORY_NAME ),	dirName);
				rl.addRecord(
					SRBMetaDataSet.getField( SRBMetaDataSet.SIZE ), size);
				rl.addRecord(
					SRBMetaDataSet.getField( SRBMetaDataSet.OFFSET ), offset);

				recordLists[i] = rl;
			}

//System.out.println("rl "+numFiles);
			for (int i=0;i<numFiles;i++) {
try {//HACK ?
				files.remove(0);
				paths.remove(0);
} catch (ArrayIndexOutOfBoundsException e) {
e.printStackTrace();
	break;
}
			}

			return recordLists;
		}

    void register( )
    	throws IOException
    {
    	int numFiles = files.size();
    	if (numFiles > 0) {
//System.out.println("sigh1");
				srbFileSystem.srbBulkLoad(
					catalogType, bloadFilePath, getFileRegistry(numFiles) );
			}
//System.out.println("sigh2");
			synchronized( mainThread )
			{
				//wake up the main thread if it is waiting for a buffer to empty
				mainThread.notify();
			}
//System.out.println("sigh3");
    }
	}


	/**
	 * Loads this SRB directory into this local directory.
	 */
	void bulkUnload( LocalFile dir )
		throws IOException
	{
		//er TODO stuff
		int flag = 0;

		if ( exists() ) {
			//This somewhat confusing situation matches the behavior of the
			//commandline 'cp' command
			if (dir.exists()) {
				dir = new LocalFile( dir, getName() );
			}
			else {
				dir.mkdir();
			}
			srbFileSystem.srbBulkUnload( catalogType, flag, getAbsolutePath(),
				dir.getAbsolutePath() );
		}
	}
//end bulkLoad methods/classes--------------------------------------------


	/**
	 * Get the permissions of the current user for this SRBFile:
	 * write, read, all, annotate or null.
	 * <P>
	 * @throws IOException If an IOException occurs.
	 */
	public String getPermissions( )
		throws IOException
	{
		MetaDataRecordList[] rl = null;
		String userName = srbFileSystem.getUserName();
		String userDomain = srbFileSystem.getDomainName();

		if (isDirectory()) {
			MetaDataCondition conditions[] = {
				MetaDataSet.newCondition(
					SRBMetaDataSet.ACCESS_DIRECTORY_NAME, MetaDataCondition.EQUAL,
						getAbsolutePath() ),
				MetaDataSet.newCondition(
					SRBMetaDataSet.USER_NAME, MetaDataCondition.EQUAL, userName ),
				MetaDataSet.newCondition(
					SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, userDomain ),
			};
			MetaDataSelect selects[] = {
				MetaDataSet.newSelection( SRBMetaDataSet.ACCESS_DIRECTORY_NAME ),
				MetaDataSet.newSelection( SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT ),
				MetaDataSet.newSelection( UserMetaData.USER_NAME ),
				MetaDataSet.newSelection( SRBMetaDataSet.USER_DOMAIN ),
			};
			rl = srbFileSystem.query( conditions, selects );

			if (rl != null) {
				for (int i=0;i<rl.length;i++) {
					if (rl[i].getValue( SRBMetaDataSet.USER_NAME ).equals( userName ) &&
						rl[i].getValue( SRBMetaDataSet.USER_DOMAIN ).equals( userDomain ))
					{
						return rl[i].getValue(
							SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT ).toString();
					}
				}
			}
		}
		else {
			MetaDataCondition conditions[] = {
				MetaDataSet.newCondition(
					SRBMetaDataSet.USER_NAME, MetaDataCondition.EQUAL, userName ),
				MetaDataSet.newCondition(
					SRBMetaDataSet.USER_DOMAIN, MetaDataCondition.EQUAL, userDomain ),
			};
			MetaDataSelect selects[] = {
				MetaDataSet.newSelection( SRBMetaDataSet.ACCESS_CONSTRAINT ),
				MetaDataSet.newSelection( UserMetaData.USER_NAME ),
				MetaDataSet.newSelection( SRBMetaDataSet.USER_DOMAIN ),
			};
			rl = query( conditions, selects );

			if (rl != null) {
				for (int i=0;i<rl.length;i++) {
					if (rl[i].getValue( SRBMetaDataSet.USER_NAME ).equals( userName ) &&
						rl[i].getValue( SRBMetaDataSet.USER_DOMAIN ).equals( userDomain ))
					{
						return rl[i].getValue(
							SRBMetaDataSet.ACCESS_CONSTRAINT ).toString();
					}
				}
			}
		}

		return null;
	}


	/**
	 * Gets all the non-null permissions of all SRB users for this SRBFile:
	 * write, read, all, annotate or null.
	 *<P>
	 * @throws IOException If an IOException occurs.
	 */
	public MetaDataRecordList[] getPermissions( boolean allUsers )
		throws IOException
	{
		if (isDirectory()) {
			MetaDataCondition conditions[] = {
				MetaDataSet.newCondition(
					SRBMetaDataSet.ACCESS_DIRECTORY_NAME, MetaDataCondition.EQUAL,
					getAbsolutePath() ),
				};
			MetaDataSelect selects[] = {
				MetaDataSet.newSelection( SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT ),
				MetaDataSet.newSelection( UserMetaData.USER_NAME ),
				MetaDataSet.newSelection( SRBMetaDataSet.USER_DOMAIN )
			};
			return srbFileSystem.query( conditions, selects );
		}
		else {
			MetaDataSelect selects[] = {
				MetaDataSet.newSelection( SRBMetaDataSet.ACCESS_CONSTRAINT ),
				MetaDataSet.newSelection( UserMetaData.USER_NAME ),
				MetaDataSet.newSelection( SRBMetaDataSet.USER_DOMAIN ),
			};
			return query( selects );
		}
	}


	/**
	 * Change the permissions for this SRBFile.
	 * <P>
	 * @param permission "w" - write;"r" - read;"rw" or "all" - ownership;"n" - null;"t" - annotate
	 * @param newUserName The permissions are changed for this user,
	 * @param userMdasDomain at this Mdas domain
	 * @throws IOException If an IOException occurs.
	 */
	public void changePermissions(
		String permission, String newUserName, String userMdasDomain )
		throws IOException
	{
		int retractionType = -1;

		if (permission == null) {
			permission = "";
		}

		permission = permission.toLowerCase();

		if (permission.equals("n") || permission.equals("null")) {
			permission = "";
		}
		else if (permission.equals("r") || permission.equals("read")) {
			permission = "read";
		}
		else if (permission.equals("w") || permission.equals("write")) {
			permission = "write";
		}
		else if (permission.equals("rw")) {
			permission = "all";
		}
		else if (permission.equals("all") || permission.equals("ownership")) {
			permission = "all";
		}
		else if (permission.equals("t") || permission.equals("annotate")) {
			permission = "annotate";
		}
		else {
			//permission = "";
			throw new IllegalArgumentException(
				"Permission type not valid: "+permission );
		}

		if (isDirectory()) {
			if ( permission == "" ) {
				retractionType = SRBMetaDataSet.D_DELETE_COLL_ACCS;
			}
			else {
				retractionType = SRBMetaDataSet.D_INSERT_COLL_ACCS;
			}

			srbFileSystem.srbModifyCollect( catalogType, getAbsolutePath(),
				newUserName, userMdasDomain, permission, retractionType );
		}
		else {
			if ( permission == "" ) {
				retractionType = SRBMetaDataSet.D_DELETE_ACCS;
			}
			else {
				retractionType = SRBMetaDataSet.D_INSERT_ACCS;
			}

			srbFileSystem.srbModifyDataset( catalogType, fileName,
				getParent(), "", "", newUserName+"@"+userMdasDomain, permission,
				retractionType );
		}
	}


	/**
	 * Replicates this SRBFile to a new resource. Each replicant will increment
	 * its replication number by 1 from the last replication.
	 * Directories/collections will be recursively replicated.
	 *<P>
	 * In SRB, one can make copies of a data set and store the copies in
	 * different locations. But, all these copies in SRB are considered to be
	 * identifiable by the same identifier. That is, each copy is considered to
	 * be equivalent to each other.
	 *<P>
	 * When a user reads a replicated data set, SRB cycles through all the
	 * copies of the datset and reads the one that is accessible at that time.
	 * It uses a simple replica identification mechanism to order this list of
	 * replicated datasets.
	 *
	 * @param	newResource The storage resource name of the new copy.
	 * @throws IOException If an IOException occurs.
	 */
	public void replicate( String newResource )
		throws IOException
	{
		if (isDirectory()) {
			String list[] = list();

			for (int i=0;i<list.length;i++) {
				if (list[i].startsWith("/")) {
					new SRBFile( srbFileSystem, list[i] ).replicate(
						newResource );
				}
				else {
					new SRBFile( srbFileSystem, getAbsolutePath() + "/" +
						list[i] ).replicate( newResource );
				}
			}
		}
		else {
			srbFileSystem.srbObjReplicate( catalogType, fileName, getParent(),
				newResource, null );
		}
	}


	/**
	 * Backup a data object. Make a replica to the backup
	 * resource. Skip it if a good copy already exist.
	 *
	 * @param backupResource - The backup resource
	 */
	public void backup( String backupResource )
		throws IOException
	{
		if (isDirectory()) {
			String list[] = list();

			for (int i=0;i<list.length;i++) {
				if (list[i].startsWith("/")) {
					new SRBFile( srbFileSystem, list[i] ).backup(
						backupResource );
				}
				else {
					new SRBFile( srbFileSystem, getAbsolutePath() + "/" +
						list[i] ).backup( backupResource );
				}
			}
		}
		else {
			srbFileSystem.srbBackupData( catalogType, fileName, getParent(),
				backupResource, 0);//TODO flag? );
		}
	}


	/**
	 * Checksum a SRB data file. If the chksum already already exists,
	 * do nothing and return the chksum value. If the chksum does not exist,
	 * compute and register it.
	 *
	 * @return the checksum value. Returns null if this SRBFile object is a
	 * 		directory.
	 */
	public String checksum( )
		throws IOException
	{
//TODO or just always - return checksum( false );
		if (isFile()) {
			String checksum = srbFileSystem.srbObjChksum( getName(), getParent(),
				LIST_CHECKSUM, null );

			//0 0 0 0 48 0 0 0
			//No checksum registered, probably
			if (checksum.equals("\u0000\u0000\u0000\u00000\u0000\u0000\u0000")) {
				return checksum( false );
			}

			return checksum;
		}

		return null;
	}


	/**
	 * Checksum a SRB data file. By default, if the chksum
	 * already exists, do nothing and return the chksum value.
	 * If the chksum does not exist, compute and register it.
	 *
	 * @param force If true force compute and register of chksum even if one
	 *		already exists. If false compute chksum, but don't register if one
	 * 		already exists.
	 *
	 * @return the checksum value. Returns null if this SRBFile object is a
	 * 		directory.
	 */
	public String checksum( boolean force )
		throws IOException
	{
		if (isFile()) {
			if (force) {
				return srbFileSystem.srbObjChksum( getName(), getParent(),
					FORCE_CHECKSUM, null );
			}
			else {
				return srbFileSystem.srbObjChksum( getName(), getParent(),
					COMPUTE_CHECKSUM, null );
			}
		}

		return null;
	}



	/**
	 * Registers the location of a file on a SRB physical resource.
	 * This SRBFile represents the logical location on the SRB.
	 * it will be linked to its physical storage resource at the path
	 * specified by <code>registeringObjectPath</code>.
	 *<br>
	 * Registers files that are created outside SRB but are accesible
	 * by SRB and are as yet unregistered. The files are not modified.
	 * The registeringObjectPath can be a path name in the physical resource
	 * file hierarchy. SRB should have at least 'read/write' access permission
	 * for registeringObjectPath. This is required as a security measure that
	 * objects are registered only by owners who are able to grant SRB write
	 * priveleges. One can always remove the 'write' permission once the object
	 * has been registered. Note that without write permission, SRB cannot write,
	 * append or remove the object.
	 *<br>
	 * The second synopsis allows one to register another copy of an already
	 * existing (or registered) srbObjectName at the new location
	 * registeringObjectPath.
	 *<br>
	 * The TargetName can be a path name in the collection hierarchy. The
	 * object creation is done in the current collection, if TargetName is
	 * just an object-name. If a relative or absolute collection is given
	 * in TargetName, then the object is stored in that collection. The user
	 * should have 'write' access permission for the collection. '.' can be
	 * used as TargetName to denote the current collection.
	 *<br>
	 * If TargetName is a collection, then register(...) uses the names of the
	 * local files as SRB object names. The directory path of
	 * registeringObjectPath is not used in making the SRB object name.
	 *<br>
	 * If TargetName is an object-name (possibly with a collection path) and
	 * there are more than one local file to be copied then the TargetName is
	 * appended to the front of the local file names to make SRB object names.
	 *<br>
	 * The correct size parameter is needed for many other SRB operations.
	 * Hence we suggest that you give the correct size of the file
	 * using this option. If the size is not given, it will try to get it
	 * through a stat() call assuming the file is local. If this stat() failed,
	 * a value of 0 is chosen.
	 *<br>
	 * @see #setResource(java.lang.String)
	 * @param registeringObjectPath The file/DB path of the data.
	 * @param dataSize The size of the dataset if known. 0 = unknown.
	 */
	public void register( String registeringObjectPath, long dataSize )
		throws IOException
	{
		if (dataSize < 0) dataSize = 0;
		srbFileSystem.srbRegisterDataset( catalogType, getName(), dataType,
			resource, getParent(), registeringObjectPath, dataSize );
	}

	/**
	 * Proxy Operation that executes a command. The results of the command
	 * will be returned by the InputStream. The protocol of the return
	 * value on the InputStream depends on the command that was run.
	 * The InputStream is opened on a different port than the main SRB
	 * connection. It can be read independently of other SRB calls.
	 *<br>
	 * Calls the SRBFileSystem function of the same name.
	 * Only valid for version 3.x.x and above.
	 * Method will not work for version 2.x.x, or before.
	 *
	 * @param command       The command to run.
	 * @param commandArgs   The command argument string.
	 *
	 * @return any byte stream output.
	 * @throws IOException  If an IOException occurs.
	 *
	 * @see edu.sdsc.grid.io.srb.SRBFileSystem#executeProxyCommand
	 */
	public InputStream executeProxyCommand( String command, String commandArgs )
		throws IOException
	{
		return srbFileSystem.executeProxyCommand( command, commandArgs,
			null, getPath(), -1);
	}

	/**
	 * Links <code>newLink</code> with this object as the source.
	 * The user should have at least 'read' access permission for the target.
	 */
	public void link( SRBFile newLink )
		throws IOException
	{
		if (isDirectory()) {
			//recursive copy
			GeneralFile[] fileList = listFiles();
			SRBFile temp;
			newLink.mkdir();

			for (int i=0;i<fileList.length;i++) {
				temp = new SRBFile( newLink,	fileList[i].getName() );
				((SRBFile)fileList[i]).link( temp );
			}
		}
		else {
			srbFileSystem.srbModifyDataset(0, fileName, getParent(),"","",
				newLink.getName(), newLink.getParent(), SRBMetaDataSet.D_INSERT_LINK);
		}
	}


//----------------------------------------------------------------------
// java.io.File-like Methods
//----------------------------------------------------------------------
	/**
	 * Tests whether the application can read the file denoted by
	 * this abstract pathname.
	 *
	 * @return  <code>true</code> if and only if the file specified by this
	 * 	abstract pathname exists <em>and</em> can be read; otherwise
	 *  <code>false</code>.
	 */
	public boolean canRead( )
	{
		MetaDataRecordList[] canRead = null;
		String readable = null;
		String userName = srbFileSystem.getUserName();
		int operator = MetaDataCondition.EQUAL;

		try {
			if (isDirectory()) {
				MetaDataCondition[] conditions = {
					MetaDataSet.newCondition(
						SRBMetaDataSet.USER_NAME, operator, userName),
					MetaDataSet.newCondition(
						SRBMetaDataSet.ACCESS_DIRECTORY_NAME, operator, getAbsolutePath())
				};
				MetaDataSelect[] selects = {
					MetaDataSet.newSelection(SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT) };
				canRead = srbFileSystem.query( conditions, selects, 3 );

				if (canRead == null)
					return false;

				for (int i=0;i<canRead.length;i++) {
					if (canRead[i].getValue(SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT)
						!= null)
					{
						readable = canRead[i].getValue(
							SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT).toString();
						if (( readable.equals("all")) || ( readable.equals("read"))) {
							return true;
						}
					}
				}
			}
			else if (isFile()) {
				MetaDataCondition[] conditions = {
					MetaDataSet.newCondition(
						SRBMetaDataSet.USER_NAME, operator, userName),
					MetaDataSet.newCondition(
						SRBMetaDataSet.DIRECTORY_NAME, operator, getParent() ),
					MetaDataSet.newCondition(
						SRBMetaDataSet.FILE_NAME, operator, getName() )
				};
				MetaDataSelect[] selects = {
					MetaDataSet.newSelection(SRBMetaDataSet.ACCESS_CONSTRAINT) };
				canRead = srbFileSystem.query( conditions, selects, 3 );

				if (canRead == null)
					return false;

				//only can be one recordlist returned
				readable = canRead[0].getValue(
						SRBMetaDataSet.ACCESS_CONSTRAINT).toString();
				if (( readable.equals("all")) || ( readable.equals("read"))) {
					return true;
				}
			}
		} catch ( IOException e ) {

		}

		return false;
	}

	/**
	 * Tests whether the application can read the file denoted by
	 * this abstract pathname. This version of the method is just to
	 * reduce repeat server queries for actions internal to the package.
	 *
	 * @return  <code>true</code> if and only if the file specified by this
	 * 	abstract pathname exists <em>and</em> can be read; otherwise
	 *  <code>false</code>.
	 */
//used internal to reduce calls to the SRB when certain info is already known
	boolean canRead( String kindOfFile )
	{
		MetaDataRecordList[] canRead = null;
		String readable = null;
		String userName = srbFileSystem.getUserName();
		int operator = MetaDataCondition.EQUAL;

		try {
			if (kindOfFile.equals("isDir")) {
				MetaDataCondition[] conditions = {
					MetaDataSet.newCondition(
						SRBMetaDataSet.USER_NAME, operator, userName),
					MetaDataSet.newCondition(
						SRBMetaDataSet.ACCESS_DIRECTORY_NAME, operator, getAbsolutePath())
				};
				MetaDataSelect[] selects = {
					MetaDataSet.newSelection(SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT) };
				canRead = srbFileSystem.query( conditions, selects, 3 );

				if (canRead == null)
					return false;

				for (int i=0;i<canRead.length;i++) {
					if (canRead[i].getValue(SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT)
						!= null)
					{
						readable = canRead[i].getValue(
							SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT).toString();
						if (( readable.equals("all")) || ( readable.equals("read"))) {
							return true;
						}
					}
				}
			}
			else if (kindOfFile.equals("isFile")) {
				MetaDataCondition[] conditions = {
					MetaDataSet.newCondition(
						SRBMetaDataSet.USER_NAME, operator, userName),
					MetaDataSet.newCondition(
						SRBMetaDataSet.DIRECTORY_NAME, operator, getParent() ),
					MetaDataSet.newCondition(
						SRBMetaDataSet.FILE_NAME, operator, getName() )
				};
				MetaDataSelect[] selects = {
					MetaDataSet.newSelection(SRBMetaDataSet.ACCESS_CONSTRAINT) };
				canRead = srbFileSystem.query( conditions, selects, 3 );

				if (canRead == null)
					return false;

				//only can be one recordlist returned
				readable = canRead[0].getValue(
						SRBMetaDataSet.ACCESS_CONSTRAINT).toString();
				if (( readable.equals("all")) || ( readable.equals("read"))) {
					return true;
				}
			}
			else {
				return canRead();
			}
		} catch ( IOException e ) {

		}

		return false;
	}


	/**
	 * Tests whether the application can modify to the file denoted by
	 * this abstract pathname.
	 *
	 * @return  <code>true</code> if and only if the file system actually
	 * 	contains a file denoted by this abstract pathname <em>and</em>
	 * 	the application is allowed to write to the file; otherwise
	 * <code>false</code>.
	 */
	public boolean canWrite( )
	{
		MetaDataRecordList[] canWrite = null;
		String writeable = null;
		String userName = srbFileSystem.getUserName();
		int operator = MetaDataCondition.EQUAL;

		try {
			if (isDirectory()) {
				MetaDataCondition[] conditions = {
					MetaDataSet.newCondition(
						SRBMetaDataSet.USER_NAME, operator, userName),
					MetaDataSet.newCondition(
						SRBMetaDataSet.ACCESS_DIRECTORY_NAME, operator, getAbsolutePath())
				};
				MetaDataSelect[] selects = {
					MetaDataSet.newSelection(SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT) };
				canWrite = srbFileSystem.query( conditions, selects, 3 );

				if (canWrite == null)
					return false;

				for (int i=0;i<canWrite.length;i++) {
					if (canWrite[i].getValue(SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT)
						!= null)
					{
						writeable = canWrite[i].getValue(
							SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT).toString();
						if (( writeable.equals("all")) || ( writeable.equals("write"))) {
							return true;
						}
					}
				}
			}
			else if (isFile()) {
				MetaDataCondition[] conditions = {
					MetaDataSet.newCondition(
						SRBMetaDataSet.USER_NAME, operator, userName),
					MetaDataSet.newCondition(
						SRBMetaDataSet.DIRECTORY_NAME, operator, getParent() ),
					MetaDataSet.newCondition(
						SRBMetaDataSet.FILE_NAME, operator, getName() )
				};
				MetaDataSelect[] selects = {
					MetaDataSet.newSelection(SRBMetaDataSet.ACCESS_CONSTRAINT) };
				canWrite = srbFileSystem.query( conditions, selects, 3 );

				if (canWrite == null)
					return false;

				//only can be one recordlist returned
				writeable = canWrite[0].getValue(
						SRBMetaDataSet.ACCESS_CONSTRAINT).toString();
				if (( writeable.equals("all")) || ( writeable.equals("write"))) {
					return true;
				}
			}
		} catch ( IOException e ) {

		}

		return false;
	}

	/**
	 * Tests whether the application can modify to the file denoted by
	 * this abstract pathname. This version of the method is just to
	 * reduce repeat server queries for actions internal to the package.
	 *
	 * @return  <code>true</code> if and only if the file system actually
	 * 	contains a file denoted by this abstract pathname <em>and</em>
	 * 	the application is allowed to write to the file; otherwise
	 * <code>false</code>.
	 */
//used internal to reduce calls to the SRB when certain info is already known
	boolean canWrite( String kindOfFile )
	{
		MetaDataRecordList[] canWrite = null;
		String writeable = null;
		String userName = srbFileSystem.getUserName();
		int operator = MetaDataCondition.EQUAL;

		try {
			if (kindOfFile.equals("isDir")) {
				MetaDataCondition[] conditions = {
					MetaDataSet.newCondition(
						SRBMetaDataSet.USER_NAME, operator, userName),
					MetaDataSet.newCondition(
						SRBMetaDataSet.ACCESS_DIRECTORY_NAME, operator, getAbsolutePath())
				};
				MetaDataSelect[] selects = {
					MetaDataSet.newSelection(SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT) };
				canWrite = srbFileSystem.query( conditions, selects, 3 );

				if (canWrite == null)
					return false;

				for (int i=0;i<canWrite.length;i++) {
					if (canWrite[i].getValue(SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT)
						!= null)
					{
						writeable = canWrite[i].getValue(
							SRBMetaDataSet.DIRECTORY_ACCESS_CONSTRAINT).toString();
						if (( writeable.equals("all")) || ( writeable.equals("write"))) {
							return true;
						}
					}
				}
			}
			else if (kindOfFile.equals("isFile")) {
				MetaDataCondition[] conditions = {
					MetaDataSet.newCondition(
						SRBMetaDataSet.USER_NAME, operator, userName),
					MetaDataSet.newCondition(
						SRBMetaDataSet.DIRECTORY_NAME, operator, getParent() ),
					MetaDataSet.newCondition(
						SRBMetaDataSet.FILE_NAME, operator, getName() )
				};
				MetaDataSelect[] selects = {
					MetaDataSet.newSelection(SRBMetaDataSet.ACCESS_CONSTRAINT) };
				canWrite = srbFileSystem.query( conditions, selects, 3 );

				if (canWrite == null)
					return false;

				//only can be one recordlist returned
				writeable = canWrite[0].getValue(
						SRBMetaDataSet.ACCESS_CONSTRAINT).toString();
				if (( writeable.equals("all")) || ( writeable.equals("write"))) {
					return true;
				}
			}
			else {
				return canWrite();
			}
		} catch ( IOException e ) {

		}

		return false;
	}


	/**
	 * Compares two abstract pathnames lexicographically.
	 *
	 * @param   pathname  The abstract pathname to be compared to this abstract
	 *                    pathname
	 *
	 * @return  Zero if the argument is equal to this abstract pathname, a
	 *		value less than zero if this abstract pathname is
	 *		lexicographically less than the argument, or a value greater
	 *		than zero if this abstract pathname is lexicographically
	 *		greater than the argument
	 */
	public int compareTo(GeneralFile pathName)
	{
		return (pathName.getAbsolutePath()).compareTo(getAbsolutePath());
	}


	/**
	 * Compares this abstract pathname to another object. If the other object
	 * is an abstract pathname, then this function behaves like <code>{@link
	 * #compareTo(GeneralFile)}</code>.  Otherwise, it throws a
	 * <code>ClassCastException</code>, since abstract pathnames can only be
	 * compared to abstract pathnames.
	 *
	 * @param   o  The <code>Object</code> to be compared to this abstract
	 *             pathname
	 *
	 * @return  If the argument is an abstract pathname, returns zero
	 *          if the argument is equal to this abstract pathname, a value
	 *          less than zero if this abstract pathname is lexicographically
	 *          less than the argument, or a value greater than zero if this
	 *          abstract pathname is lexicographically greater than the
	 *          argument
	 *<P>
	 * @throws ClassCastException - if the argument is not an abstract pathname
	 */
	public int compareTo(Object o) throws ClassCastException
	{
		return compareTo((SRBFile)o);
	}


	/**
	 * Atomically creates a new, empty file named by this abstract pathname if
	 * and only if a file with this name does not yet exist.  The check for the
	 * existence of the file and the creation of the file if it does not exist
	 * are a single operation that is atomic with respect to all other
	 * filesystem activities that might affect the file.
	 * <P>
	 * Note: this method should <i>not</i> be used for file-locking, as
	 * the resulting protocol cannot be made to work reliably.
	 *
	 * @return  <code>true</code> if the named file does not exist and was
	 *          successfully created; <code>false</code> if the named file
	 *          already exists
	 *
	 * @throws  IOException If an I/O error occurred
	 */
	public boolean createNewFile() throws IOException
	{
		try {
			if (!isFile()) {
				getParentFile().mkdirs();

				int fd = srbFileSystem.srbObjCreate(
					catalogType, getName(), dataType, resource, getParent(),
					null, -1 );

				//Be sure to close files after a create() or open().
				srbFileSystem.srbObjClose( fd );

				return true;
			}
		} catch (SRBException e) {
			//catch already exists and just return false
			if (e.getType() != -3210)
				throw e;
		}

		return false;
	}


	/**
	 * Creates an empty file in the default temporary-file directory, using
	 * the given prefix and suffix to generate its name.  Invoking this method
	 * is equivalent to invoking <code>{@link GeneralFile#createTempFile(
	 * java.lang.String, java.lang.String, edu.sdsc.grid.io.GeneralFile)
	 * createTempFile(prefix,&nbsp;suffix,&nbsp;null)}</code>.
	 *
	 * @param  prefix     The prefix string to be used in generating the file's
	 *                    name; must be at least three characters long
	 *
	 * @param  suffix     The suffix string to be used in generating the file's
	 *                    name; may be <code>null</code>, in which case the
	 *                    suffix <code>".tmp"</code> will be used
	 *
	 * @return  An abstract pathname denoting a newly-created empty file
	 *
	 * @throws  IllegalArgumentException
	 *          If the <code>prefix</code> argument contains fewer than three
	 *          characters
	 *
	 * @throws  IOException  If a file could not be created
	 */
	public static GeneralFile createTempFile(String prefix, String suffix)
		throws IOException, IllegalArgumentException
	{
		return createTempFile(prefix, suffix, null);
	}


	/**
	 * <p> Creates a new empty file in the specified directory, using the
	 * given prefix and suffix strings to generate its name.  If this method
	 * returns successfully then it is guaranteed that:
	 *
	 * <ol>
	 * <li> The file denoted by the returned abstract pathname did not exist
	 *      before this method was invoked, and
	 * <li> Neither this method nor any of its variants will return the same
	 *      abstract pathname again in the current invocation of the virtual
	 *      machine.
	 * </ol>
	 *
	 * This method provides only part of a temporary-file facility.  To arrange
	 * for a file created by this method to be deleted automatically, use the
	 * <code>{@link #deleteOnExit}</code> method.
	 *
	 * <p> The <code>prefix</code> argument must be at least three characters
	 * long.  It is recommended that the prefix be a short, meaningful string
	 * such as <code>"hjb"</code> or <code>"mail"</code>.  The
	 * <code>suffix</code> argument may be <code>null</code>, in which case the
	 * suffix <code>".tmp"</code> will be used.
	 *
	 * <p> To create the new file, the prefix and the suffix may first be
	 * adjusted to fit the limitations of the underlying platform.  If the
	 * prefix is too long then it will be truncated, but its first three
	 * characters will always be preserved.  If the suffix is too long then it
	 * too will be truncated, but if it begins with a period character
	 * (<code>'.'</code>) then the period and the first three characters
	 * following it will always be preserved.  Once these adjustments have been
	 * made the name of the new file will be generated by concatenating the
	 * prefix, five or more internally-generated characters, and the suffix.
	 *
	 * <p> If the <code>directory</code> argument is <code>null</code> then the
	 * default temporary-file directory will be used. Since the SRB does not
	 * have a standard temporary directory, files will be placed in a temp/
	 * directory in the user's SRB home directory.
	 * There are certain difficulties creating a static connection to the SRB.
	 * For this static method to connect to the SRB, .Mdas files must be
	 * available in the local home directory/.srb. That is the information that
	 * will be used when storing the temporary file. This comprimise is
	 * necessary to maintain the designs unity with the java.io.File class.
	 *
	 * @param  prefix     The prefix string to be used in generating the file's
	 *                    name; must be at least three characters long
	 *
	 * @param  suffix     The suffix string to be used in generating the file's
	 *                    name; may be <code>null</code>, in which case the
	 *                    suffix <code>".tmp"</code> will be used
	 *
	 * @param  directory  The directory in which the file is to be created, or
	 *                    <code>null</code> if the default temporary-file
	 *                    directory is to be used
	 *
	 * @return  An abstract pathname denoting a newly-created empty file
	 *
	 * @throws  IllegalArgumentException
	 *          If the <code>prefix</code> argument contains fewer than three
	 *          characters
	 *
	 * @throws  IOException  If a file could not be created
	 */
	public static GeneralFile createTempFile(
		String prefix, String suffix, GeneralFile directory)
		throws IOException, IllegalArgumentException
	{
		String randomChars = "";
		for (int i=0;i<8;i++)
			randomChars += ((char) (65 + Math.random() * 25));

		if (prefix == null)
			throw new NullPointerException();
		if (prefix.length() < 3)
			throw new IllegalArgumentException("Prefix string too short");

		if (suffix == null)
			suffix = ".tmp";

		if (directory == null) {
			SRBFileSystem fs = new SRBFileSystem();
			directory = new SRBFile( fs, fs.getHomeDirectory(), "temp" );
			directory.mkdir();
		}


		SRBFile temp = new SRBFile( (SRBFile) directory,
			prefix+randomChars+suffix );

		if ( temp.createNewFile() )
			return temp;
		else {
			throw new IOException("The temp file already exists.");
		}
	}


	/**
	 * Deletes the file or directory denoted by this abstract pathname.  If
	 * this pathname denotes a directory, then the directory must be empty in
	 * order to be deleted.
	 *
	 * @return  <code>true</code> if and only if the file or directory is
	 *          successfully deleted; <code>false</code> otherwise
	 */
	public boolean delete( )
	{
		return delete( false );
	}

	/**
	 * Deletes the file or directory denoted by this abstract pathname.  If
	 * this pathname denotes a directory, then the directory must be empty in
	 * order to be deleted.
	 *
	 * @return  <code>true</code> if and only if the file or directory is
	 *          successfully deleted; <code>false</code> otherwise
	 */
	public boolean delete( boolean force )
	{
		try {
			//Trashcan new as of SRB3.1
			if (!force &&	(srbFileSystem.getVersionNumber() > 3 ))
			{
				if (isDirectory()) {
					srbFileSystem.srbModifyCollect ( catalogType,	getAbsolutePath(),
						"", "", "", SRBMetaDataSet.C_MOVE_COLL_TO_TRASH);
					return true;
				}
				else if (isFile() && (getReplicaNumber() < 0)) {
					srbFileSystem.srbModifyDataset ( catalogType, fileName, getParent(),
						"", "", "", "", SRBMetaDataSet.D_MOVE_DATA_TO_TRASH);
					return true;
				}
				else {
					srbFileSystem.srbObjUnlink( fileName, getParent() );
					return true;
				}
			}
			else {
				if (isFile()) {
					srbFileSystem.srbObjUnlink( fileName, getParent() );
					return true;
				}
				else if (isDirectory()) {
					srbFileSystem.srbModifyCollect( catalogType, getAbsolutePath(),
						"", "", "", SRBMetaDataSet.D_DELETE_COLL );
					return true;
				}
			}
		} catch( IOException e ) {
			return false;
		}
		return false;
	}


	/**
	 * Requests that the file or directory denoted by this abstract
	 * pathname be deleted when the virtual machine terminates.
	 * Deletion will be attempted only for normal termination of the
	 * virtual machine, as defined by the Java Language Specification.
	 *
	 * <p> Once deletion has been requested, it is not possible to cancel the
	 * request.  This method should therefore be used with care.
	 *
	 * <P>
	 * Note: this method should <i>not</i> be used for file-locking, as
	 * the resulting protocol cannot be made to work reliably.
	 */
	public void deleteOnExit( )
	{
		deleteOnExit = true;
	}


	/**
	 * Tests this abstract pathname for equality with the given object.
	 * Returns <code>true</code> if and only if the argument is not
	 * <code>null</code> and is an abstract pathname that denotes the same file
	 * or directory as this abstract pathname.
	 *
	 * @param   obj   The object to be compared with this abstract pathname
	 *
	 * @return  <code>true</code> if and only if the objects are the same;
	 *          <code>false</code> otherwise
	 */
	public boolean equals(Object obj)
	{
		try {
  		if (obj == null)
  			return false;

			SRBFile temp = (SRBFile) obj;

			return getAbsolutePath().equals(temp.getAbsolutePath());
		} catch (ClassCastException e) {
			return false;
		}
	}


	/**
	 * Tests whether the file denoted by this abstract pathname exists.
	 *
	 * @return  <code>true</code> if and only if the file denoted by this
	 * 	abstract pathname exists; <code>false</code> otherwise
	 */
	public boolean exists( )
	{
		try {
			MetaDataRecordList[] rl = null;
			int operator = MetaDataCondition.EQUAL;

			//if it is a file
			MetaDataCondition conditions[] = null;

			if (getReplicaNumber() >= 0) {
				conditions = new MetaDataCondition[2];
				conditions[0] = MetaDataSet.newCondition(
					SRBMetaDataSet.DIRECTORY_NAME, operator, getParent() );
				conditions[1] = MetaDataSet.newCondition(
					SRBMetaDataSet.FILE_NAME, operator, getName() );
				conditions[1] = MetaDataSet.newCondition(
					SRBMetaDataSet.FILE_REPLICATION_ENUM, operator, replicaNumber );
			}
			else {
				conditions = new MetaDataCondition[2];
				conditions[0] = MetaDataSet.newCondition(
					SRBMetaDataSet.DIRECTORY_NAME, operator, getParent() );
				conditions[1] = MetaDataSet.newCondition(
					SRBMetaDataSet.FILE_NAME, operator, getName() );
			}

			MetaDataSelect selects[] = {
				MetaDataSet.newSelection( SRBMetaDataSet.FILE_NAME )
			};

			rl = srbFileSystem.query( conditions, selects, 3 );

			if (rl != null)
				return true;


			//if it is a directory
			conditions = new MetaDataCondition[1];
			conditions[0] =
				MetaDataSet.newCondition(
					SRBMetaDataSet.DIRECTORY_NAME, operator, getAbsolutePath() );
			selects[0] =
				MetaDataSet.newSelection( SRBMetaDataSet.DIRECTORY_NAME );
			rl = srbFileSystem.query( conditions, selects, 3 );

			if (rl != null)
				return true;

		} catch ( IOException e ) {
			//ignore
		}

		return false;
	}


	/**
	 * Returns the absolute form of this abstract pathname. Equivalent to
	 * <code>new&nbsp;SRBFile(this.{@link #getAbsolutePath}())</code>.
	 *
	 * @return  The absolute abstract pathname denoting the same file or
	 *          directory as this abstract pathname
	 */
	public GeneralFile getAbsoluteFile( )
	{
		return new SRBFile( srbFileSystem, getAbsolutePath() );
	}



	/**
	 * Returns the absolute pathname string of this abstract pathname.
	 *
	 * @return  The absolute pathname denoting the same file or
	 *          directory as this abstract pathname
	 */
	public String getAbsolutePath( )
	{
		if (( directory != null ) && (!directory.isEmpty())) {
			int size = directory.size();
			String path = (String) directory.firstElement();
			int i = 1;

			while (i < size ) {
				path += getPathSeparator() + directory.get( i );
				i++;
			}

			return path + getPathSeparator() + getName();
		}

		return getName();
	}

	/**
	 * Returns the canonical form of this abstract pathname.  Equivalent to
	 * <code>new&nbsp;SRBFile(this.{@link #getCanonicalPath}())</code>.
	 *
	 * @return  The canonical pathname string denoting the same file or
	 *          directory as this abstract pathname
	 *
	 * @throws  IOException
	 *          If an I/O error occurs, which is possible because the
	 *          construction of the canonical pathname may require
	 *          filesystem queries
	 */
	public GeneralFile getCanonicalFile( )
		throws IOException
	{
		return new SRBFile( srbFileSystem, getCanonicalPath() );
	}


	/**
	 * Returns the canonical pathname string of this abstract pathname.
	 *
	 * @return  The canonical pathname string denoting the same file or
	 *          directory as this abstract pathname
	 *
	 * @throws  IOException
	 *          If an I/O error occurs, which is possible because the
	 *          construction of the canonical pathname may require
	 *          filesystem queries
	 */
	public String getCanonicalPath( )
		throws IOException
	{
		if (( directory != null ) && (!directory.isEmpty())) {
			int size = directory.size();
			String path = (String) directory.firstElement();
			int i = 1;

			while (i < size ) {
				path += getPathSeparator() + directory.get( i );
				i++;
			}

			return path + getPathSeparator() + fileName;
		}

		return fileName;
/*
//TODO should check?
		MetaDataRecordList[] rl = null;

		rl = query( SRBMetaDataSet.DIRECTORY_NAME );
		if (rl != null) {
			rl[0].getValue( SRBMetaDataSet.DIRECTORY_NAME ) + PATH_SEPARATOR + fileName;
		}
	*/
	}


	/**
	 * @return The name of the file or directory denoted by this
	 * 	abstract pathname.
	 */
	public String getName( )
	{
		//strip &COPY=
		int index = fileName.indexOf( "&COPY=" );
		if (index >= 0) {
			return fileName.substring( 0, index );
		}
		else {
			return fileName;
		}
	}


	/**
	 * @return The pathname string of this abstract pathname's parent,
	 * 	or null if this pathname does not name a parent directory.
	 */
	public String getParent( )
	{
//TODO not canonical/absolute?
		if (( directory != null ) && (!directory.isEmpty())) {
			int size = directory.size();
			String path = (String) directory.firstElement();
			int i = 1;

			while (i < size ) {
				path += getPathSeparator() + directory.get( i );
				i++;
			}

			return path;
		}

		return null;
	}


	/**
	 * Returns the abstract pathname of this abstract pathname's parent,
	 * or <code>null</code> if this pathname does not name a parent
	 * directory.
	 *
	 * <p> The <em>parent</em> of an abstract pathname consists of the
	 * pathname's prefix, if any, and each name in the pathname's name
	 * sequence except for the last.  If the name sequence is empty then
	 * the pathname does not name a parent directory.
	 *
	 * @return  The abstract pathname of the parent directory named by this
	 *          abstract pathname, or <code>null</code> if this pathname
	 *          does not name a parent
	 */
	public GeneralFile getParentFile( )
	{
		return new SRBFile( srbFileSystem, getParent() );
	}


	/**
	 * @return This abstract pathname as a pathname string.
	 */
	public String getPath( )
	{
		//The path always gets converted to absolute form on construction
		return originalFilePath;
	}

	/**
	 * Computes a hash code for this abstract pathname. The hash code of
	 * an abstract pathname is equal to the exclusive <em>or</em> of its
	 * pathname string and the decimal value <code>1234321</code>.
	 *
	 * @return  A hash code for this abstract pathname
	 */
	public int hashCode( )
	{
		return getAbsolutePath().toLowerCase().hashCode() ^ 1234321;
	}


	/**
	 * Tests whether this abstract pathname is absolute. A pathname is
	 * absolute if its prefix is <code>"/"</code>.
	 *
	 * @return  <code>true</code> if this abstract pathname is absolute,
	 *          <code>false</code> otherwise
	 */
	public boolean isAbsolute( )
	{
		//all path names are made absolute at construction.
		return true;
	}


	/**
	 * Tests whether the file denoted by this abstract pathname is a directory.
	 * Also known on the SRB as a collection.
	 *<P>
	 * A SRB collection is a logical name given to a set of data sets. All data
	 * sets stored in SRB/MCAT are stored in some collection. A collection can
	 * have sub-collections, and hence provides a hierarchical structure. A
	 * collection in SRB/MCAT can be equated to a directory in a Unix file
	 * system. But unlike a file system, a collection is not limited to a
	 * single device (or partition). A collection is logical but the datsets
	 * grouped under a collection can be stored in heterogeneous storage
	 * devices. There is one obvious restriction, the name given to a data set
	 * in a collection or sub-collection should be unique in that collection.
	 *
	 * @return <code>true</code> if and only if the file denoted by this
	 *          abstract pathname exists <em>and</em> is a directory;
	 *          <code>false</code> otherwise
	 */
	public boolean isDirectory( )
	{
		MetaDataRecordList[] rl = null;
		MetaDataCondition[] conditions = {
			MetaDataSet.newCondition( SRBMetaDataSet.DIRECTORY_NAME,
				MetaDataCondition.EQUAL, getAbsolutePath() ) };
		MetaDataSelect[] selects = {
			MetaDataSet.newSelection( SRBMetaDataSet.DIRECTORY_NAME ) };

		try {
			rl = srbFileSystem.query( conditions, selects, 3 );

			if ( rl != null )
				return true;

		} catch ( IOException e ) {
e.printStackTrace();
			//don't do anything
		}

		return false;
	}


	/**
	 * Tests whether the file denoted by this abstract pathname is a normal
	 * file. A file is <em>normal</em> if it is not a directory or a container.
	 * Any non-directory or other subclass of SRBFile, such as a SRBContainer,
	 * file created by a Java application is guaranteed to be a normal file.
	 *<P>
	 * In the terminology of SRB, files are known as data sets. A data set is
	 * a "stream-of-bytes" entity that can be uniquely identified. For example,
	 * a file in HPSS or Unix is a data set, or a LOB stored in a SRB Vault
	 * database is a data set. Importantly, note that a data set is not a
	 * set of data objects/files. Each data set in SRB is given a unique
	 * internal identifier by SRB. A dataset is associated with a collection.
	 *
	 * @return  <code>true</code> if and only if the file denoted by this
	 *          abstract pathname exists <em>and</em> is a normal file;
	 *          <code>false</code> otherwise
	 */
	public boolean isFile( )
	{
		MetaDataRecordList[] rl = null;
		MetaDataCondition[] conditions = {
			MetaDataSet.newCondition( SRBMetaDataSet.DIRECTORY_NAME,
				MetaDataCondition.EQUAL, getParent() ),
			MetaDataSet.newCondition( SRBMetaDataSet.FILE_NAME,
				MetaDataCondition.EQUAL, getName() ) };
		MetaDataSelect[] selects = {
			MetaDataSet.newSelection( SRBMetaDataSet.FILE_NAME ) };

		try {
			rl = srbFileSystem.query( conditions, selects, 3 );

			if( rl != null )
				return true;

		} catch ( IOException e ) {
e.printStackTrace();
			//don't do anything
		}

		return false;
	}


	/**
	 * Tests whether the file named by this abstract pathname is a hidden file.
	 *
	 * @return  <code>true</code> if and only if the file denoted by this
	 *          abstract pathname is hidden.
	 */
	public boolean isHidden( )
	{
		return false; //SRB files can't be hidden
	}


	/**
	 * Returns the time that the file denoted by this abstract pathname
	 * was last modified.
	 *
	 * @return  A <code>long</code> value representing the time the file was
	 *          last modified, measured in milliseconds since the epoch
	 *          (00:00:00 GMT, January 1, 1970), or <code>0L</code> if the
	 *          file does not exist or if an I/O error occurs
	 */
	public long lastModified( )
	{
		long lastModified = 0;
		long[] stat;
		try {
			stat = getStat();
		} catch ( IOException e ) {
e.printStackTrace();
			return 0;
		}

		if (stat != null)
			lastModified = stat[10] * 1000; //SRB returns measure in seconds

		return lastModified;
	}


	/**
	 * Returns the length of the file denoted by this abstract pathname.
	 * The return value is unspecified if this pathname denotes a directory.
	 *
	 * @return  The length, in bytes, of the file denoted by this abstract
	 *          pathname, or <code>0L</code> if the file does not exist
	 */
	public long length( )
	{
		long length = 0;
		long[] stat;
		try {
			stat = getStat();
		} catch ( IOException e ) {
e.printStackTrace();
			return 0;
		}

		if (stat != null)
			length = stat[0];

		return length;
	}


	/**
	 * Returns an array of strings naming the files and directories in
	 * the directory denoted by this abstract pathname.
	 *<P>
	 * There is no guarantee that the name strings in the resulting
	 * array will appear in any specific order; they are not, in particular,
	 * guaranteed to appear in alphabetical order.
	 *<P>
	 * If this SRBFile object denotes a file, the directory containing
	 * that file will be listed instead.
	 *<P>
	 * This method will return all the files in the directory. Listing
	 * directories with a large number of files may take a very long time.
	 * The more generic SRBFile.query() method could be used to iterate
	 * through the file list piecewise.
	 *
	 * @return  An array of strings naming the files and directories in the
	 *          directory denoted by this abstract pathname. The array will be
	 * 					empty if the directory is empty. Returns null if an I/O error
	 * 					occurs.
	 */
	public String[] list( )
	{
		MetaDataCondition conditions[] = new MetaDataCondition[1];
		MetaDataSelect selects[] = {
			MetaDataSet.newSelection( SRBMetaDataSet.FILE_NAME ) };
		MetaDataRecordList[] rl1 = null;
		MetaDataRecordList[] rl2 = null;
		MetaDataRecordList[] temp = null;
		Vector list = null;
		String path = null;


		try {
			//Have to do two queries, one for files and one for directories.
			if (isDirectory()) {
				path = getAbsolutePath();
			}
			else {
				path = getParent();
			}

			//get all the files
			conditions[0] = MetaDataSet.newCondition(
				SRBMetaDataSet.DIRECTORY_NAME, MetaDataCondition.EQUAL, path );
			rl1 = srbFileSystem.query(
				conditions, selects, SRBFileSystem.DEFAULT_RECORDS_WANTED );
			if (completeDirectoryList) {
				rl1 = MetaDataRecordList.getAllResults( rl1 );
			}

			//get all the sub-directories
			selects[0] = MetaDataSet.newSelection( SRBMetaDataSet.DIRECTORY_NAME );
			conditions[0] = MetaDataSet.newCondition(
				SRBMetaDataSet.PARENT_DIRECTORY_NAME, MetaDataCondition.EQUAL, path );
			rl2 = srbFileSystem.query(
				conditions, selects, SRBFileSystem.DEFAULT_RECORDS_WANTED );
			if (completeDirectoryList) {
				rl2 = MetaDataRecordList.getAllResults( rl2 );
			}

			//change to relative path
			if (rl2 != null) {
				String absolutePath = null;
				String relativePath = null;
				for (int i=0;i<rl2.length;i++) {
					//only one record per rl
					absolutePath = rl2[i].getStringValue(0);
					relativePath = absolutePath.substring(
						absolutePath.lastIndexOf( "/" )+1 );
					rl2[i].setValue( 0, relativePath );
				}
			}
		} catch ( IOException e ) {
e.printStackTrace();
			return null;
		}


		if (( rl1 != null ) && (rl2 != null)) {
			//length of previous query + (new query - table and attribute names)
			temp = new SRBMetaDataRecordList[rl1.length+rl2.length];
			//copy files
			System.arraycopy( rl1, 0, temp, 0, rl1.length );
			System.arraycopy( rl2, 0, temp, rl1.length, rl2.length );
		}
		else if (rl1 != null) {
			temp = rl1;
		}
		else if (rl2 != null) {
			temp = rl2;
		}
		else {
			return new String[0];
		}

		list = new Vector();
		for (int i=0;i<temp.length;i++) {
if (temp[i].getStringValue(0) != null) {
			//only one record per rl
			list.add(temp[i].getStringValue(0));
}
		}

		return (String[]) list.toArray(new String[0]);
	}


	/**
	 * Returns the array of strings naming the files and directories in
	 * the directory denoted by this abstract pathname and which match a
	 * query formed using these <code>conditions</code>.
	 *
	 * @see list()
	 */
	//TODO public not ready
	public String[] list( MetaDataCondition[] conditions )
	{
		if (conditions == null)
			return list();

		MetaDataCondition tempConditions[] = null;
		MetaDataSelect selects[] = {
			MetaDataSet.newSelection( SRBMetaDataSet.FILE_NAME ) };
		MetaDataRecordList[] rl1 = null;
		MetaDataRecordList[] rl2 = null;
		MetaDataRecordList[] temp = null;
		Vector list = null;
		String path = null;


		try {
			//Have to do two queries, one for files and one for directories.
			if (isDirectory()) {
				path = getAbsolutePath();
			}
			else {
				path = getParent();
			}
			tempConditions = MetaDataSet.mergeConditions(
				MetaDataSet.newCondition(	SRBMetaDataSet.DIRECTORY_NAME,
					MetaDataCondition.EQUAL, path ),
				conditions );

			//get all the files
			rl1 = srbFileSystem.query(
				tempConditions, selects, SRBFileSystem.DEFAULT_RECORDS_WANTED );
			if (completeDirectoryList) {
				rl1 = MetaDataRecordList.getAllResults( rl1 );
			}

			//get all the sub-directories
			selects[0] = MetaDataSet.newSelection( SRBMetaDataSet.DIRECTORY_NAME );
			tempConditions = MetaDataSet.mergeConditions( MetaDataSet.newCondition(
				SRBMetaDataSet.PARENT_DIRECTORY_NAME, MetaDataCondition.EQUAL, path ),
				(MetaDataCondition)null );
			rl2 = srbFileSystem.query(
				tempConditions, selects, SRBFileSystem.DEFAULT_RECORDS_WANTED );
			if (completeDirectoryList) {
				rl2 = MetaDataRecordList.getAllResults( rl2 );
			}

			//change to relative path
			if (rl2 != null) {
				String absolutePath = null;
				String relativePath = null;
				for (int i=0;i<rl2.length;i++) {
					//only one record per rl
					absolutePath = rl2[i].getStringValue(0);
					relativePath = absolutePath.substring(
						absolutePath.lastIndexOf( "/" )+1 );
					rl2[i].setValue( 0, relativePath );
				}
			}
		} catch ( IOException e ) {
e.printStackTrace();
			return null;
		}


		if (( rl1 != null ) && (rl2 != null)) {
			//length of previous query + (new query - table and attribute names)
			temp = new SRBMetaDataRecordList[rl1.length+rl2.length];
			//copy files
			System.arraycopy( rl1, 0, temp, 0, rl1.length );
			System.arraycopy( rl2, 0, temp, rl1.length, rl2.length );
		}
		else if (rl1 != null) {
			temp = rl1;
		}
		else if (rl2 != null) {
			temp = rl2;
		}
		else {
			return new String[0];
		}

		list = new Vector();
		for (int i=0;i<temp.length;i++) {
if (temp[i].getStringValue(0) != null) {
			//only one record per rl
			list.add(temp[i].getStringValue(0));
}
		}

		return (String[]) list.toArray(new String[0]);
	}


	/**
	 * List the available filesystem roots. The SRB has a single root
	 * directory, namely <code>"/"</code>.
	 * <p>
	 * It is guaranteed that the canonical pathname of any file physically
	 * present in the SRB will begin with one of the roots returned by this
	 * method.
	 *
	 * @return  An array of <code>GeneralFile</code> objects denoting the
	 * 	SRB filesystem root, namely <code>"/"</code>.
	 */
	public static GeneralFile[] listRoots( )
	{
		GeneralFile[] list = new GeneralFile[1];

		try {
			list[0] = new SRBFile( new SRBFileSystem(), SRBFileSystem.SRB_ROOT );
		} catch ( IOException e ) {
e.printStackTrace();
			//this isn't really a pleasant method.
		}

		return list;
	}


	/**
	 * Creates the directory named by this abstract pathname.
	 */
	public boolean mkdir( )
	{
		try {
			if (!isDirectory()) {
				srbFileSystem.srbCreateCollect( catalogType, getParent(), getName() );
				return true;
			}
		} catch ( IOException e ) {
e.printStackTrace();
			//ignore
		}

		return false;
	}


	/**
	 * Creates the directory named by this abstract pathname, including any
	 * necessary but nonexistent parent directories.  Note that if this
	 * operation fails it may have succeeded in creating some of the necessary
	 * parent directories.
	 */
	public boolean mkdirs( )
	{
		if (!isDirectory()) {
			int dirNum = directory.size();
			SRBFile dir = new SRBFile( srbFileSystem,
				directory.get(0).toString() );


			for (int i=0;i<dirNum;i++) {
				if (!dir.exists()){
					dir.mkdir();
					dir = new SRBFile( dir, directory.get(i).toString() );
				}
			}
			mkdir();
			return true;
		}

		return false;
	}


	/**
	 * Renames the file denoted by this abstract pathname.
	 *<P>
	 * Whether or not this method can move a file from one filesystem to
	 * another is platform-dependent. The return value should always be
	 * checked to make sure that the rename operation was successful.
	 *
	 * @param  dest  The new abstract pathname for the named file
	 *
	 * @throws  IllegalArgumentException
	 *          If parameter <code>dest</code> is not a <code>SRBFile</code>.
	 */
	public boolean renameTo(GeneralFile dest)
		throws IllegalArgumentException
	{
		if (dest instanceof SRBFile) {
			try {
				if (isFile()) {
					if (getParent().equals(dest.getParent())) {
						//only renaming data
						srbFileSystem.srbModifyDataset( catalogType, fileName, getParent(),
							"", "", dest.getName(), "", SRBMetaDataSet.D_CHANGE_DNAME);
					}
					else if (getName().equals(dest.getName())) {
						//only moving to new collection
						srbFileSystem.srbModifyDataset( catalogType, fileName, getParent(),
							"", "", dest.getParent(), "", SRBMetaDataSet.D_CHANGE_GROUP);
					}
					else {
						//changing name of object as well as its collection
						String tempName = dest.getName() + (long) Math.random();

						//first change the name to a temp name
						srbFileSystem.srbModifyDataset( catalogType, fileName, getParent(),
							"", "", tempName, "", SRBMetaDataSet.D_CHANGE_DNAME);


						try {
							//then change directory name
							srbFileSystem.srbModifyDataset( catalogType, tempName,
								getParent(), "","", dest.getParent(), "",
								SRBMetaDataSet.D_CHANGE_GROUP);
						} catch ( IOException e ) {
e.printStackTrace();
							//change the name back
							srbFileSystem.srbModifyDataset( catalogType, tempName,
								getParent(), "","", fileName, "",
								SRBMetaDataSet.D_CHANGE_DNAME);
						}

						try { //then change the temp name to the new name
							srbFileSystem.srbModifyDataset( catalogType, tempName,
								dest.getParent(),	"","", dest.getName(), "",
								SRBMetaDataSet.D_CHANGE_DNAME);
						} catch ( IOException e ) {
e.printStackTrace();
							//change the it back
							srbFileSystem.srbModifyDataset( catalogType, tempName,
								dest.getParent(),	"","", getParent(), "",
								SRBMetaDataSet.D_CHANGE_GROUP);
							srbFileSystem.srbModifyDataset( catalogType, tempName,
								getParent(), "","", fileName, "",
								SRBMetaDataSet.D_CHANGE_DNAME);
						}
					}
					directory = new Vector();
					setFileName( dest.getAbsolutePath() );
					return true;
				}
				else if (isDirectory()) {
					srbFileSystem.srbModifyCollect( catalogType,
						getAbsolutePath(), dest.getAbsolutePath(), null, null,
						SRBMetaDataSet.D_CHANGE_COLL_NAME);
					directory = new Vector();
					setFileName( dest.getAbsolutePath() );
					return true;
				}
				else if (!exists()) {
					directory = new Vector();
					setFileName( dest.getAbsolutePath() );
					return true;
				}
			} catch ( IOException e ) {
e.printStackTrace();
				//ignore
			}
		}
		else {
			//TODO should I do this? It isn't really rename.
			try {
				copyTo( dest, false );
				delete();
				directory = new Vector();
				setFileName( dest.getAbsolutePath() );
				return true;
			} catch (IOException e) {
e.printStackTrace();
				return false;
			}
		}

		return false;
	}


	/*
	 * Sets the last-modified time of the file or directory named by
	 * this abstract pathname.
	 *<P>
	 * All platforms support file-modification times to the nearest second,
	 * but some provide more precision. The argument will be truncated to fit
	 * the supported precision. If the operation succeeds and no intervening
	 * operations on the file take place, then the next invocation of the
	 * lastModified() method will return the (possibly truncated) time argument
	 * that was passed to this method.
	 *
	 * The SRB does not support this operation.
	 * This method will only change the lastModified time to the current time.
	 * The SRB will change the last modified date of a file, but then
	 * immediately recognize a change to the file has occured and change
	 * the last modified time to the current time.
	 *
	 * @param  time  The new last-modified time, measured in milliseconds since
	 *               the epoch (00:00:00 GMT, January 1, 1970)
	 *
	 * @throws  IllegalArgumentException  If the argument is negative
	 */
	/**
	 * This method will only change the lastModified time to the current time.
	 * The SRB will overwrite the input from this method
	 */
	public boolean setLastModified(long time)
		throws IllegalArgumentException
	{
throw new UnsupportedOperationException();
/*
		if ( time < 0 ) throw new IllegalArgumentException();

		try {
			if (isDirectory()) {
				srbFileSystem.srbModifyCollect( catalogType,
					getAbsolutePath(), new Long( time ).toString(), null, null,
					SRBMetaDataSet.D_CHANGE_MODIFY_TIMESTAMP);
				return true;
			}
			else {
				srbFileSystem.srbModifyDataset( catalogType, getName(),
					getParent(), null, null, new Long( time ).toString(), null,
					SRBMetaDataSet.D_CHANGE_MODIFY_TIMESTAMP);
				return true;
			}
		} catch (IOException e) {
e.printStackTrace();
			//ignore
		}
		return false;
*/
	}


	/**
	 * Marks the file or directory named by this abstract pathname so that
	 * only read operations are allowed.  After invoking this method the file
	 * or directory is guaranteed not to change until it is either deleted or
	 * marked to allow write access.  Whether or not a read-only file or
	 * directory may be deleted depends upon the underlying system.
	 *
	 * This operation is not possible on the SRB. File permissions can only
	 * be changed on a per user basis. Changing the file permissions for
	 * everyone is not possible.
	 */
	public boolean setReadOnly( )
	{
//TODO can't do this in the SRB.
throw new UnsupportedOperationException();
/*
		String userName = srbFileSystem.getUserName();
		String userMdasDomain = srbFileSystem.getDomainName();
		int retractionType = SRBMetaDataSet.D_INSERT_COLL_ACCS;
		String permission = "read";

		try {
			if (isDirectory()) {
				srbFileSystem.srbModifyCollect( catalogType, getAbsolutePath(),
					userName, userMdasDomain, permission, retractionType );
					return true;
			}
			else {
				srbFileSystem.srbModifyDataset( catalogType, fileName,
					getParent(), null, null, userName+"@"+userMdasDomain, permission,
					retractionType );
				return true;
			}
		}	catch ( IOException e ) {
e.printStackTrace();
			//don't do anything
		}
		return false;
*/
	}


	/**
	 * Constructs a <tt>file:</tt> URI that represents this abstract pathname.
	 *
	 * <p> The exact form of the URI is according to the SRB.  If it can be
	 * determined that the file denoted by this abstract pathname is a
	 * directory, then the resulting URI will end with a slash.
	 *
	 * <p> For a given abstract pathname <i>f</i>, it is guaranteed that
	 *
	 * <blockquote><tt>
	 * new {@link #SRBFile(java.net.URI) SRBFile}
	 * (</tt><i>&nbsp;f</i><tt>.toURI()).equals(</tt><i>&nbsp;f</i><tt>)
	 * </tt></blockquote>
	 *
	 * so long as the original abstract pathname, the URI, and the new abstract
	 * pathname are all created in (possibly different invocations of) the same
	 * Java virtual machine.  However, this relationship typically does not hold
	 * when a <tt>file:</tt> URI that is created in a virtual machine on one
	 * operating system is converted into an abstract pathname in a virtual
	 * machine on a different operating system.
	 *
	 * @return  An absolute, hierarchical URI with a scheme equal to
	 *          <tt>"file"</tt>, a path representing this abstract pathname,
	 *          and undefined authority, query, and fragment components
	 *
	 * @see #SRBFile(java.net.URI)
	 * @see java.net.URI
	 * @see java.net.URI#toURL()
	 */
	public URI toURI( )
	{
		URI uri = null;

		try {
			if (isDirectory()) {
				uri = new URI( "srb",
					srbFileSystem.getUserName() + "." + srbFileSystem.getDomainName(),
					srbFileSystem.getHost(), srbFileSystem.getPort(), getAbsolutePath() +
					"/", "", "");
			}
			else {
				uri = new URI( "srb",
					srbFileSystem.getUserName() + "." + srbFileSystem.getDomainName(),
					srbFileSystem.getHost(),	srbFileSystem.getPort(), getAbsolutePath(),
					"", "");
			}
		} catch ( URISyntaxException e ) {
			e.printStackTrace();
		}

		return uri;
	}


	/**
	 * Converts this abstract pathname into a <code>file:</code> URL.  The
	 * exact form of the URL is is according to the SRB.  If it can be
	 * determined that the file denoted by this abstract pathname is a
	 * directory, then the resulting URL will end with a slash.
	 *
	 * <p> <b>Usage note:</b> This method does not automatically escape
	 * characters that are illegal in URLs.  It is recommended that new code
	 * convert an abstract pathname into a URL by first converting it into a
	 * URI, via the {@link #toURI() toURI} method, and then converting the URI
	 * into a URL via the {@link java.net.URI#toURL() URI.toURL} method.
	 *
	 * @return  A URL object representing the equivalent file URL
	 *
	 * @throws  MalformedURLException
	 *          If the path cannot be parsed as a URL
	 *
	 * @see     #toURI()
	 * @see     java.net.URI
	 * @see     java.net.URI#toURL()
	 * @see     java.net.URL
	 */
	public URL toURL() throws MalformedURLException
	{
		URL url = null;

		if (isDirectory()) {
			url = new URL( "srb://" + srbFileSystem.getUserName() + "." +
				srbFileSystem.getDomainName() + "@" + srbFileSystem.getHost() + ":" +
				srbFileSystem.getPort() + getAbsolutePath() + "/" );
		}
		else {
			url = new URL( "srb://" + srbFileSystem.getUserName() + "." +
				srbFileSystem.getDomainName() + "@" + srbFileSystem.getHost() + ":" +
				srbFileSystem.getPort() + getAbsolutePath() );
		}

		return url;
	}
}

