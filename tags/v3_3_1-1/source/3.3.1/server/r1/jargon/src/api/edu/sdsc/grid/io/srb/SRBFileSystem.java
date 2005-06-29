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
//	SRBFileSystem.java	-  edu.sdsc.grid.io.srb.SRBFileSystem
//
//  CLASS HIERARCHY
//	java.lang.Object
//			|
//			+-edu.sdsc.grid.io.GeneralFileSystem
//						|
//						+-edu.sdsc.grid.io.RemoteFileSystem
//									|
//									+-edu.sdsc.grid.io.srb.SRBFileSystem
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.srb;

import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.local.*;

import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.InputStream;



/**
 * The SRBFileSystem class is the class for connection implementations
 * to SRB servers. It provides the framework to support a wide range
 * of SRB semantics. Specifically, the functions needed to interact
 * with a SRB server.
 *<P>
 * Many of the methods remain package private, though a few "srb..."
 * methods have been made public. These methods are generally very low
 * level and their use is not recommended. Clear, high level equivalents
 * are being developed elsewhere in the JARGON API, and in the SRBAdmin
 * tool. If you require the functionality of any of these methods and it
 * is not yet available as a high level method, please notify the
 * administrator of the JARGON package and we will try to include your
 * request in the next release.
 *
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 * @since   JARGON1.0
 * @see		edu.sdsc.grid.io.srb.SRBCommands
 */
public class SRBFileSystem extends RemoteFileSystem
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
	/**
	 * The SRB like Unix only has one root, "/".
	 */
	public static final String SRB_ROOT = "/";

//TODO used by srbDeleteValue()
	public static final int DELETE_TYPE_LOCATION = 1;
	public static final int DELETE_TYPE_USER = 2;
	public static final int DELETE_TYPE_RESOURCE = 3;


	/**
	 * Length of the user info byte array,
	 * which is passed during the connection handshake.
	 */
	static final int USER_INFO_BUFFER_LENGTH = 400;

	static final int USER_INFO_BUFFER_LENGTH_3_0 = 464;


	/**
	 * Length of mdas authorization password
	 */
	static final int MDAS_PASSWORD_LENGTH = 32;

	/**
	 * Default number of records returned by a query
	 */
	static int DEFAULT_RECORDS_WANTED = 300;


	/**
	 * Used for proxy commands, no result.
	 */
	public static final int PORTAL_OFF = 0; // 0x00000000

	/**
	 * Used for proxy commands, create a portal and put it in env PORTAL_ENV.
	 */
	public static final int PORTAL_ON = 1; // 0x00000001

	/**
	 * Used for proxy commands, send result to the InputStream
	 */
	public static final int PORTAL_STD_IN_OUT = 2; // 0x00000002


	//When updating for new SRB versions, and
	//adding metadata attribute pairs, need to change
	//SRBFileSystem.TOTAL_METADATA_ATTRIBUTES
	//to match, 1 + last DCS-ATTRIBUTE-INDEX
	//see also c client, catalog/include/mdasC_db2_externs.h
	//and SRBMetaDataSet class.
	//static int TOTAL_METADATA_ATTRIBUTES = 180; //default for SRB1.1.8
	//static int TOTAL_METADATA_ATTRIBUTES = 300; //default for SRB2.0
	//static int TOTAL_METADATA_ATTRIBUTES = 500; //default for SRB3.0
	static int TOTAL_METADATA_ATTRIBUTES; //set in constructor

	//length of query string conditional,
	//used esp. by SRBMetaDataCommands.srbGetDataDirInfo()
	//this value changed in version 3.0,
	//static int MAX_TOKEN = 200; //SRB2.0 (and before?)
	//static int MAX_TOKEN = 500; //SRB3.0
	static int MAX_TOKEN; //set in constructor

	//length of a value returned in the returnSRBMetaDataRecordList
	//this value changed in version 3.0,
	//static int MAX_FILE_SIZE = 400; //SRB2.0 (and before?)
	//static int MAX_FILE_SIZE = 500; //SRB3.0.0
	//static int MAX_FILE_SIZE = 2700; //SRB3.0.2
	static int MAX_FILE_SIZE; //set in constructor


//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * This object handles the socket protocol and
	 * communications with the Srb server.
	 */
	private SRBCommands commands;



	/**
	 * Use this account object instead of the parent class's
	 * GeneralAccount object.
	 */
	private SRBAccount srbAccount;

//TODO
	/**
	 * Keep a seperate version string here. Have to keep track of the
	 * version on a per object basis, but SRBAccount.version is a
	 * static variable.
	 */
	private String version;
	private float versionNumber;


	/**
	 * Used to specify a port range available through a firewall.
	 * Needed because some SRB commands open new ports on the client machine.
	 */
	/*static*/ int MIN_PORT = -1;


	/**
	 * Used to specify a port range available through a firewall.
	 * Needed because some SRB commands open new ports on the client machine.
	 */
	/*static*/ int MAX_PORT = -1;


//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Opens a socket connection to read from and write to.
	 * Loads the default SRB user account information from their home directory.
	 * The account information stored in this object cannot be changed once
	 * constructed.
	 *<P>
	 * This constructor is provided for convenience however,
	 * it is recommended that all necessary data be sent
	 * to the constructor and not left to the defaults.
	 *
	 * @throws FileNotFoundException if the user data file cannot be found.
	 * @throws IOException if an IOException occurs.
	 */
	public SRBFileSystem( )
		throws FileNotFoundException, IOException
	{
		this( new SRBAccount() );
	}

	/**
	 * Opens a socket connection to read from and write to. Opens the account
	 * held in the SRBAccount object. The account information stored in this
	 * object cannot be changed once constructed.
	 *<P>
	 * This constructor is provided for convenience however,
	 * it is recommended that all necessary data be sent
	 * to the constructor and not left to the defaults.
	 *
	 * @param srbAccount	the SRB account information object.
	 * @throws FileNotFoundException if the user data file cannot be found.
	 * @throws NullPointerException if srbAccount is null.
	 * @throws IOException if an IOException occurs.
	 */
	public SRBFileSystem( SRBAccount srbAccount )
		throws IOException, NullPointerException
	{
		setAccount( srbAccount );
		setVersion( srbAccount.getVersion() );

		commands = new SRBCommands(	);

		int error = commands.connect( srbAccount, createUserInfoBuffer() );

		while ((error == -1118) || (error == -2007) || (error == -2772) ||
			(error == -3005))
		{
			System.out.println(
				"Wrong SRB version. Trying alternate handshake. error: " +error);

			//Tries the next latest release, just to be helpful.
			if (version.equals( SRBAccount.SRB_VERSION_3_3 ) ) {
				setVersion( SRBAccount.SRB_VERSION_3_0_2 );
				commands = new SRBCommands(	);
				error = commands.connect( srbAccount, createUserInfoBuffer() );
			}
			else if (version.equals( SRBAccount.SRB_VERSION_3_0_2 ) ) {
				setVersion( SRBAccount.SRB_VERSION_3 );
				commands = new SRBCommands(	);
				error = commands.connect( srbAccount, createUserInfoBuffer() );
			}
			else if (version.equals( SRBAccount.SRB_VERSION_3 ) ) {
				setVersion( SRBAccount.SRB_VERSION_2 );
				commands = new SRBCommands(	);
				error = commands.connect( srbAccount, createUserInfoBuffer() );
			}
			else if (version.equals( SRBAccount.SRB_VERSION_2 ) ) {
				setVersion( SRBAccount.SRB_VERSION_1_1_8 );
				commands = new SRBCommands(	);
				error = commands.connect( srbAccount, createUserInfoBuffer() );
			}
			else { /*if (version.equals( SRBAccount.SRB_VERSION_1_1_8 ) ) {*/
				//just forget it
				break;
			}
		}
		if (error < 0) {
			throw new SRBException( "Connection Failed", error );
		}


//TODO
//They probably don't know they are using the wrong version
//can only use one at a time.
SRBAccount.setVersion( version );
	}


	/**
	 * Finalizes the object by explicitly letting go of each of
	 * its internally held values.
	 */
	protected void finalize( )
		throws Throwable
	{
		super.finalize();
		if (commands != null)
			commands = null;
		if (account != null)
			account = null;
	}




//----------------------------------------------------------------------
// Setters and Getters
//----------------------------------------------------------------------
//General
	/**
	 * Loads the account information for this file system.
	 */
	protected void setAccount( GeneralAccount account )
		throws IOException
	{
		if ( account == null )
			account = new SRBAccount();

		srbAccount = (SRBAccount) account.clone();
		this.account = srbAccount;

		setVersion( srbAccount.getVersion() );
	}

//private? protected
	private void setVersion( String version )
	{
		if (version == null) {
			return;
		}

		this.version = version;
		if (version.equals( SRBAccount.SRB_VERSION_3_3 ) ) {
			TOTAL_METADATA_ATTRIBUTES = 500;
			MAX_TOKEN = 500;
			MAX_FILE_SIZE = 2700;
		}
		else if (version.equals( SRBAccount.SRB_VERSION_3_0_2 ) ) {
			TOTAL_METADATA_ATTRIBUTES = 500;
			MAX_TOKEN = 500;
			MAX_FILE_SIZE = 2700;
		}
		else if (version.equals( SRBAccount.SRB_VERSION_3 ) ) {
			TOTAL_METADATA_ATTRIBUTES = 500;
			MAX_TOKEN = 500;
			MAX_FILE_SIZE = 500;
		}
		else if (version.equals( SRBAccount.SRB_VERSION_2 ) ) {
			TOTAL_METADATA_ATTRIBUTES = 300;
			MAX_TOKEN = 200;
			MAX_FILE_SIZE = 400;
		}
		else if (version.equals( SRBAccount.SRB_VERSION_1_1_8 ) ) {
			TOTAL_METADATA_ATTRIBUTES = 180;
			MAX_TOKEN = 200;
			MAX_FILE_SIZE = 400;
		}
		else {
			throw new IllegalArgumentException( "Invalid version" );
		}

		versionNumber = srbAccount.getVersionNumber();
	}


	/**
	 * Returns the account used by this SRBFileSystem.
	 */
	public GeneralAccount getAccount( )
		throws NullPointerException
	{
		if ( srbAccount != null )
			return (SRBAccount) srbAccount.clone();

		throw new NullPointerException();
	}


	/**
	 * Returns the root directories of the SRB file system.
	 */
	public String[] getRootDirectories( )
	{
		String[] root = { SRB_ROOT };

		return root;
	}




//SRB
	/**
	 * Only used by the SRBFile( uri ) constructor.
	 */
	void setDefaultStorageResource( String resource )
	{
		srbAccount.setDefaultStorageResource( resource );
	}
	/**
	 * Only used by the SRBFile( uri ) constructor.
	 */
	void setProxyMcatZone( String zone )
	{
		srbAccount.setProxyMcatZone( zone );
	}
	/**
	 * Only used by the SRBFile( uri ) constructor.
	 */
	void setMcatZone( String zone )
	{
		srbAccount.setMcatZone( zone );
	}



	/**
	 * Used to specify a port range available through a firewall.
	 * Needed because some SRB commands open new ports on the client machine.
	 */
	public /*static*/ void setFirewallPorts( int minPort, int maxPort )
	{
		if ((minPort >= 0) && (maxPort >= 0)) {
			MIN_PORT = minPort;
			MAX_PORT = maxPort;
		}
	}


	/**
	 * Sets the default number of records that will be returned by a query.
	 * Must be a positive integer.
	 */
	public void setQueryRecordsWanted( int num )
	{
		if ( num > 0 ) {
			DEFAULT_RECORDS_WANTED = num;
		}
	}


	/**
	 * @return the default number of records that will be returned by a query.
	 */
	public int getQueryRecordsWanted()
	{
		return DEFAULT_RECORDS_WANTED;
	}


	/**
	 * @return the SRB password
	 */
	public String getPassword()
	{
		return srbAccount.getPassword();
	}


	/**
	 * Currently, just the mdas text password as a byte array.
	 */
	byte[] getPasswordBytes()
	{
		byte password[] = new byte[MDAS_PASSWORD_LENGTH];

		System.arraycopy(	srbAccount.getPassword().getBytes(), 0,
			password, 0, srbAccount.getPassword().length() );

		return password;
	}

	/**
	 * @return the proxy user name
	 */
	public String getProxyUserName()
	{
		return srbAccount.getProxyUserName();
	}

	/**
	 * @return the proxy domain name
	 */
	public String getProxyDomainName()
	{
		return srbAccount.getProxyDomainName();
	}

	/**
	 * @return the options
	 */
	public int getOptions()
	{
		return srbAccount.getOptions();
	}

	/**
	 * @return the domain name used by the client.
	 * Only different from the proxyDomainName for ticketed users.
	 */
	public String getDomainName( )
	{
		return srbAccount.getDomainName();
	}

	/**
	 * @return the SRB version
	 */
	public String getVersion( )
	{
		return version;
	}

	/**
	 * @return the default storage resource.
	 */
	public String getDefaultStorageResource( )
	{
		return srbAccount.getDefaultStorageResource();
	}


//SRB 3.0
	/**
	 * @return the proxy mcat zone.
	 */
	public String getProxyMcatZone( )
	{
		return srbAccount.getProxyMcatZone();
	}


	/**
	 * @return the client mcat zone.
	 */
	public String getMcatZone( )
	{
		return srbAccount.getMcatZone();
	}


	/**
	 * @return the exec file.
	 */
	public String getExecFile( )
	{
		return srbAccount.getExecFile();
	}

	/**
	 * @return the version number
	 */
	float getVersionNumber( )
	{
		return srbAccount.getVersionNumber();
	}


	/**
	 * The number of bytes transfered by this filesystem object so far during
	 * the currently executing SRBFile.copyTo/copyFrom command.
	 *
	 * @return the number of bytes that have been transfered so far.
	 */
	long fileCopyStatus()
	{
		return commands.getBytesMoved();
	}


//----------------------------------------------------------------------
// GeneralFileSystem methods
//----------------------------------------------------------------------
//TODO
	/**
	 * Queries all files in the metadata catalog and uses
	 * metadata values, <code>fieldName</code>, to be returned.
	 *<P>
	 * This is a convenience method, the same as the code:<br>
	 * <code>query( MetaDataSet.newSelection( fieldName ) );</code>
	 *
	 * @param fieldName The string name used to form the select object.
	 * @return The metadata values for this file refered to by
	 * <code>fieldName</code>
	 */
	public MetaDataRecordList[] query( String fieldName )
		 throws IOException
	{
		MetaDataSelect[] temp = { MetaDataSet.newSelection( fieldName ) };
		return query( temp, DEFAULT_RECORDS_WANTED );
	}

	/**
	 * Queries all files in the metadata catalog and uses
	 * metadata values, <code>selects</code>, to be returned.
	 *<P>
	 * This is a convenience method, the same as the code:<br>
	 * <code>query( MetaDataSet.newSelection( fieldNames ) );</code>
	 *
	 * @param fieldNames The string names used to form the select objects.
	 * @return The metadata values for this file refered to by
	 * <code>fieldNames</code>
	 */
	public MetaDataRecordList[] query( String[] fieldNames )
		 throws IOException
	{
		return query( MetaDataSet.newSelection( fieldNames ),
			DEFAULT_RECORDS_WANTED );
	}


	/**
	 * Queries all files in the metadata catalog and uses one
	 * metadata value, <code>select</code>, to be returned.
	 */
	public MetaDataRecordList[] query( MetaDataSelect select )
		 throws IOException
	{
		MetaDataSelect[] temp = { select };
		return query( temp, DEFAULT_RECORDS_WANTED );
	}

	/**
	 * Queries all files in the metadata catalog and uses
	 * metadata values, <code>selects</code>, to be returned.
	 */
	public MetaDataRecordList[] query( MetaDataSelect[] selects )
		 throws IOException
	{
		return query( selects, DEFAULT_RECORDS_WANTED );
	}



	/**
	 * Queries all files in the metadata catalog and uses
	 * metadata values, <code>selects</code>, to be returned.
	 */
	public MetaDataRecordList[] query( MetaDataSelect[] selects,
		int recordsWanted )
		throws IOException
	{
		String fieldName = SRBMetaDataSet.FILE_ID;
		int operator = MetaDataCondition.NOT_EQUAL;
		int value = -1;

		MetaDataCondition conditions[] = {
			MetaDataSet.newCondition( fieldName, operator, value ) };

		return query( conditions, selects, recordsWanted );
	}


	/**
	 * Queries the file server to find all files that
   * match a set of conditions. For all those that
   * match, the fields indicated in the select array
   * are returned in the result object.
   *
   * @param conditions The conditional statements that describe
   *		the values to query the server, like WHERE in SQL.
   * @param selects The attributes to be returned from those values that
   * 		met the conditions, like SELECT in SQL.
   */
  public MetaDataRecordList[] query(
  	MetaDataCondition[] conditions, MetaDataSelect[] selects )
  	throws IOException
  {
  	return query( conditions, selects, DEFAULT_RECORDS_WANTED );
  }


	/**
	 * Queries the file server to find all files that
   * match a set of conditions. For all those that
   * match, the fields indicated in the select array
   * are returned in the result object.
   *
   * @param conditions The conditional statements that describe
   *		the values to query the server, like WHERE in SQL.
   * @param selects The attributes to be returned from those values that
   * 		met the conditions, like SELECT in SQL.
   * @param recordsWanted The number of values to return with the query,
   * 		use the getMoreRecords() method in MetaDataRecordList to
   *		continue the search, if more records are available.
   */
  public MetaDataRecordList[] query( MetaDataCondition[] conditions,
  	MetaDataSelect[] selects, int recordsWanted )
  	throws IOException
  {
  	return query( conditions, selects, recordsWanted, false, false );
  }


	/**
	 * Queries the file server to find all files that
   * match a set of conditions. For all those that
   * match, the fields indicated in the select array
   * are returned in the result object.
   *
   * @param conditions The conditional statements that describe
   *		the values to query the server, like WHERE in SQL.
   * @param selects The attributes to be returned from those values that
   * 		met the conditions, like SELECT in SQL.
   * @param recordsWanted The number of values to return with the query,
   * 		use the getMoreRecords() method in MetaDataRecordList to
   *		continue the search, if more records are available.
   * @param orderBy sorts the query's returned values. Ordered matching
   * 		the order of the selects array.
   */
  public MetaDataRecordList[] query( MetaDataCondition[] conditions,
  	MetaDataSelect[] selects, int recordsWanted, boolean orderBy )
  	throws IOException
  {
  	return query( conditions, selects, recordsWanted, orderBy, false );
	}

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
   *	<li>Redundant condition fields
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
   * names of the MetaDataGroups.
   *<P>
   * The orderBy variable sorts the query's returned values. The order
   * will match the order of the selects array. <br>
   * E.g., where selects[0] = SIZE<br>
   * and selects[1] = OWNER<br>
   * The files returned
   *
   * @param conditions The conditional statements that describe
   *		the values to query the server, like WHERE in SQL and all conditions
   * 		in the array will be AND-ed together to form the query.
   * @param selects The attributes to be returned from those values that
   * 		met the conditions, like SELECT in SQL.
   * @param recordsWanted The number of values to return with the query,
   * 		use the getMoreRecords() method in MetaDataRecordList to
   *		continue the search, if more records are available.
   * @param orderBy sorts the query's returned values. Ordered matching
   * 		the order of the selects array.
   * @param nonDistinct If true, allows redundencies in returned data.
	 */
  public MetaDataRecordList[] query( MetaDataCondition[] conditions,
  	MetaDataSelect[] selects, int recordsWanted, boolean orderBy,
  	boolean nonDistinct )
  	throws IOException
  {
//TODO see comments
/*move to SRBCommands
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
//get from SRBFile
int catalogType = 0;


//TODO test for fileSystem valid query field
//if (protocol == SRB)

  	return srbGetDataDirInfo( catalogType, conditions, selects,
  		recordsWanted, orderBy, nonDistinct );
  }



	/**
	 * Tests this filesystem object for equality with the given object.
	 * Returns <code>true</code> if and only if the argument is not
	 * <code>null</code> and both are filesystem objects connected to the
	 * same filesystem using the same account information.
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

			SRBFileSystem temp = (SRBFileSystem) obj;

			if (getAccount().equals(temp.getAccount())) {
				if (isConnected() == temp.isConnected()) {
					return true;
				}
			}
		} catch (ClassCastException e) {
			return false;
		}
		return false;
	}


	/**
	 * Checks if the socket is connected.
	 */
	public boolean isConnected()
	{
		return commands.isConnected();
	}


	/**
	 * Returns a string representation of this file system object.
	 * The string is formated according to the SRB URI model.
	 * Note: the user password will not be included in the URI.
	 */
	public String toString()
	{
		return new String( "srb://"+getUserName()+"."+getDomainName()+"@"+
			getHost()+":"+getPort() );
	}



	/**
	 * Closes the connection to the SRB file system. The filesystem
	 * cannot be reconnected after this method is called. If this object,
	 * or another object which uses this filesystem, tries to send a
	 * command to the server a ClosedChannelException will be thrown.
	 */
	public void close() throws IOException
	{
		commands.close();
	}




//----------------------------------------------------------------------
// SRB Methods
//----------------------------------------------------------------------
	/**
	 * Prepares the userInfoBuffer for transfer by loading the
	 * user info. The user info was either passed to the constructor
	 * or obtained from Mdas files.
 	 * <P>
	 * @return userInfoBuffer	Byte array which gets transfered to the srb server
	 */
	private byte[] createUserInfoBuffer( )
	{
/*Zone srb 3.0 handshake
typedef struct StartupInfo {
//     PacketHdr        hdr;
    char                proxyUserName[NAMEDATALEN]; // proxy User Name
    char                proxyDomainName[NAMEDATALEN];
    char                proxyMcatZone[NAMEDATALEN];
    char                proxyAuth[NAMEDATALEN];
    char                clientUserName[NAMEDATALEN]; // proxy User Name
    char                clientDomainName[NAMEDATALEN];
    char                clientMcatZone[NAMEDATALEN];
    char                clientAuth[NAMEDATALEN];
    char                version[PATH_SIZE];  // The version number
    char                options[ARGV_SIZE];  // possible additional args
    char                execFile[ARGV_SIZE]; // possible backend to use
} StartupInfo;
*/

		byte userInfoBuffer[] = null;
		//total length of the user info buffer,
		//LSBF problem
		//including len and messageType, byte value 190,
		//the int value is the same as USER_INFO_BUFFER_LENGTH
		byte bufferLength[] = { 0, 0, 1, -112 };
		byte messageType = 7; //see SRB c client list of message types
		String temp = null;

		if (getVersion().equals(SRBAccount.SRB_VERSION_2) ||
				getVersion().equals(SRBAccount.SRB_VERSION_1_1_8))
		{
			userInfoBuffer = new byte[USER_INFO_BUFFER_LENGTH];

			System.arraycopy( bufferLength, 0, userInfoBuffer, 0,
				bufferLength.length);

			//messageType
			userInfoBuffer[4] = messageType;

			//ProxyUserName
			System.arraycopy( getProxyUserName().getBytes(), 0, userInfoBuffer, 8,
				getProxyUserName().length() );

			//ProxyDomainName
			System.arraycopy(	getProxyDomainName().getBytes(), 0, userInfoBuffer, 40,
				getProxyDomainName().length() );


			//ClientUserName
			System.arraycopy(	getUserName().getBytes(), 0, userInfoBuffer, 72,
				getUserName().length() );

			//ClientDomainName
			System.arraycopy( getDomainName().getBytes(), 0, userInfoBuffer, 104,
				getDomainName().length() );


			//Options
			userInfoBuffer[136] = (byte) getOptions();

			//Version
			System.arraycopy(	getVersion().getBytes(), 0, userInfoBuffer, 264,
				getVersion().length() );
		}
		else
		{
			userInfoBuffer = new byte[USER_INFO_BUFFER_LENGTH_3_0];

			//the length got longer, now 1D0,
			bufferLength[3] = -48;

			System.arraycopy( bufferLength, 0, userInfoBuffer, 0,
				bufferLength.length);

			//messageType
			userInfoBuffer[4] = messageType;


			//ProxyUserName
			System.arraycopy( getProxyUserName().getBytes(), 0, userInfoBuffer, 8,
				getProxyUserName().length() );

			//ProxyDomainName
			System.arraycopy(	getProxyDomainName().getBytes(), 0, userInfoBuffer, 40,
				getProxyDomainName().length() );

			//ProxyMcatZone
			temp = getProxyMcatZone();
			if (temp != null) {
				System.arraycopy(	getProxyMcatZone().getBytes(), 0, userInfoBuffer, 72,
					getProxyMcatZone().length() );
			}


			//ClientUserName
			System.arraycopy(	getUserName().getBytes(), 0, userInfoBuffer, 104,
				getUserName().length() );

			//ClientDomainName
			System.arraycopy( getDomainName().getBytes(), 0, userInfoBuffer, 136,
				getDomainName().length() );

			//ClientMcatZone
			temp = getMcatZone();
			if (temp != null) {
				System.arraycopy(	getMcatZone().getBytes(), 0, userInfoBuffer, 168,
					getMcatZone().length() );
			}

			//these two got switched
			//Version
			System.arraycopy(	getVersion().getBytes(), 0, userInfoBuffer, 200,
				getVersion().length() );

			//Options
			userInfoBuffer[264] = (byte) getOptions();


			//execFile
			if (getExecFile() != null) {
				System.arraycopy(	getExecFile().getBytes(), 0, userInfoBuffer, 328,
					getExecFile().length() );
			}
		}

		return userInfoBuffer;
	}


	/**
	 * Proxy Operation that executes a command. The results of the command
	 * will be returned by the InputStream. The protocol of the return
	 * value on the InputStream depends on the command that was run.
	 * The InputStream is opened on a different port than the main SRB
	 * connection. It can be read independently of other SRB calls.
	 *
	 * @param command	The command to run.
	 * @param commandArgs	The command argument string.
	 *
	 * @return any byte stream output.
	 * @throws IOException	If an IOException occurs.
	 */
	public InputStream executeProxyCommand(
		String command, String commandArgs )
		throws IOException
	{
		//This method can't be done at this level,
		//So it had to be passed to SRBCommand
		//Plus dealing with the return value as a byte[] wasn't viable

		return srbExecCommand( command, commandArgs, null, -1 );
	}

	/**
	 * Proxy Operation that executes a command.
	 *
	 * @param command	The command to run.
	 * @param commandArgs	The command argument string.
	 * @param hostAddress	The host address where this proxy operation
	 *                      should be performed. null = the server for the
	 *                      current connect.
	 * @param portalFlag    The portal flag.
	 *			  	Valid flags are - PORTAL_OFF, PORTAL_ON, PORTAL_STD_IN_OUT.
	 *
	 * @return any byte stream output.
	 * @throws IOException	If an IOException occurs.
	 */
	public InputStream executeProxyCommand(
		String command, String commandArgs, String hostAddress, int portalFlag )
		throws IOException
	{
		return srbExecCommand( command, commandArgs, hostAddress, portalFlag );
	}


	/**
	 * Proxy Operation that executes a command.
	 * Only valid for version 3.x.x and above.
	 * Method will not work for version 2.x.x, or before.
	 *
	 * @param command       The command to run.
	 * @param commandArgs   The command argument string.
	 * @param hostAddress   The host address where this proxy operation
	 *                      should be performed. null = the server for the
	 *                      current connect.
	 * @param fileName      The SRB path to a file to perform proxy operation on.
	 * @param portalFlag    The portal flag.
	 *          Valid flags are - PORTAL_OFF, PORTAL_ON, PORTAL_STD_IN_OUT.
	 *
	 * @return any byte stream output.
	 * @throws IOException  If an IOException occurs.
	 */
	public InputStream executeProxyCommand(
		String command, String commandArgs, String hostAddress, String fileName,
		int portalFlag )
		throws IOException
	{
		return srbExecCommand( command, commandArgs, hostAddress, fileName,
		portalFlag );
	}





//----------------------------------------------------------------------
// Command methods
//----------------------------------------------------------------------
	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjCreate
	 */
	int srbObjCreate( int catType, String fileName, String dataTypeName,
		String resourceName, String collectionName, String localPathName,
		long dataSize )
		throws IOException
	{
		if (dataTypeName == null) {
			dataTypeName = "generic";//DefMdasDataTypeName
		}

		if (resourceName == null) {
			resourceName = getDefaultStorageResource();//DefMdasResourceName
		}

		if (collectionName == null) {
			collectionName = getHomeDirectory();//DefMdasCollectionName
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		if (localPathName == null) {
//TODO should do query PATH_NAME
			localPathName = "";
		}

		return commands.srbObjCreate( catType, fileName, dataTypeName,
			resourceName, collectionName, localPathName, dataSize );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjOpen
	 */
	int	srbObjOpen( String objID, int openFlag, String collectionName )
		throws IOException
	{
		if (collectionName == null) {
			collectionName = getHomeDirectory();
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		return commands.srbObjOpen( objID, openFlag, collectionName );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjClose
	 */
	void srbObjClose( int srbFD )
		throws IOException
	{

		commands.srbObjClose( srbFD );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjUnlink
	 */
	void srbObjUnlink ( String objID, String collectionName )
		throws IOException
	{
		if (collectionName == null) {
			collectionName = getHomeDirectory();
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		commands.srbObjUnlink( objID, collectionName );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjRead
	 */
	byte[] srbObjRead( int srbFD, int length )
		throws IOException
	{

		return commands.srbObjRead( srbFD, length );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjWrite
	 */
	int srbObjWrite( int srbFD, byte outputBuffer[], int length )
		throws IOException
	{
		//should 0 = write whole file?
		if (length <= 0)
			return 0;

		return commands.srbObjWrite( srbFD, outputBuffer, length );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjSeek
	 */
	void srbObjSeek( int srbFD, long offset, int whence )
		throws IOException
	{

		commands.srbObjSeek( srbFD, offset, whence );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjSync
	 */
	void srbObjSync( int srbFD )
		throws IOException
	{

		commands.srbObjSync( srbFD );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjStat
	 */
	long[] srbObjStat( int catType, String pathName, int myType )
		throws IOException
	{
		if (pathName == null) {
			pathName = getHomeDirectory();
		}
		else if (!pathName.startsWith(SRB_ROOT)) {
			pathName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + pathName;
		}
		/*
		 * Definition for isDir in myType in srbObjStat call
		 *	#define IS_UNKNOWN      -1      // don't know if it is file or dir
		 *	#define IS_FILE         0       // this is a file
		 *	#define IS_DIR_1        1       // is a collection. new desc
		 *	#define IS_DIR_2        2       // is a collection. listing data
		 *	#define IS_DIR_3        3       // is a collection. done listing data
		 *	#define IS_DIR_4        4       // is a collection. listing collection
		 */
		if (( myType > 4 ) || ( myType	< -1 )) {
			myType = -1;
		}

		return commands.srbObjStat( catType, pathName,	myType );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjStat64
	 */
	long[] srbObjStat64( int catType, String collectionName, String fileName )
		throws IOException
	{
		if (collectionName == null) {
			collectionName = getHomeDirectory();
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		//Make sure collectionName ends with '/'
		if ( collectionName.substring(collectionName.length()-1) !=
					SRBFile.PATH_SEPARATOR )
		{
			//don't use: System.getProperty( "file.separator" )
			//because the srb is expecting '/'
			if ( collectionName.substring(collectionName.length()-1) ==
				System.getProperty( "file.separator" ) ) {
				collectionName = collectionName.substring(
					0, collectionName.length()-2 ) + SRBFile.PATH_SEPARATOR;
			}
			else {
				collectionName = collectionName + SRBFile.PATH_SEPARATOR;
			}
		}

		return commands.srbObjStat64( catType, collectionName+fileName );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjReplicate
	 */
	void srbObjReplicate( int catType, String objID, String collectionName,
		String newResourceName, String newPathName )
		throws IOException
	{
		if (collectionName == null) {
			collectionName = getHomeDirectory();
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		if (newResourceName == null) {
			newResourceName = getDefaultStorageResource();//DefMdasResourceName
		}

		commands.srbObjReplicate( catType, objID, collectionName,
			newResourceName, newPathName );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjMove
	 */
	void srbObjMove (int catType, String objID, String collectionName,
		String newResourceName, String newPathName )
		throws IOException
	{
		if (collectionName == null) {
			collectionName = getHomeDirectory();
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName = getHomeDirectory() + collectionName;
		}

		if (newResourceName == null) {
			newResourceName = getDefaultStorageResource();//DefMdasResourceName
		}

		commands.srbObjMove(catType, objID, collectionName,
			newResourceName, newPathName);
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjProxyOpr
	 */
	byte[] srbObjProxyOpr( int operation, int inputInt1, int inputInt2,
		int inputInt3, int inputInt4,	String inputStr1, String inputStr2,
		String inputStr3, String inputStr4,	byte[] inputBStrm1,
		byte[] inputBStrm2, byte[] inputBStrm3 )
		throws IOException
	{

		return commands.srbObjProxyOpr( operation, inputInt1, inputInt2,
			inputInt3, inputInt4,	inputStr1, inputStr2, inputStr3, inputStr4,
			inputBStrm1, inputBStrm2, inputBStrm3 );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjGetdents
	 */
	byte[] srbObjGetdents( int catType, int srbFD, long nbyte )
		throws IOException
	{

		return commands.srbObjGetdents( catType, srbFD, nbyte );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjGetdents64
	 */
	byte[] srbObjGetdents64( int catType, int srbFD, long nbyte )
		throws IOException
	{

		return commands.srbObjGetdents64(catType, srbFD, nbyte);
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbCollSeek
	 */
	void srbCollSeek( int srbFD, int offset, int whence, int is64Flag )
		throws IOException
	{

		commands.srbCollSeek( srbFD, offset, whence, is64Flag );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbGetDatasetInfo
	 */
	SRBMetaDataRecordList[] srbGetDatasetInfo( int catType, String objID,
		String collectionName, int recordsWanted ) throws IOException
	{
		if (collectionName == null) {
			collectionName = getHomeDirectory();
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName = getHomeDirectory() + collectionName;
		}

		if ( recordsWanted < 1 ) {
			//use default
			recordsWanted = DEFAULT_RECORDS_WANTED;
		}

		return commands.srbGetDatasetInfo( catType, objID, collectionName,
			recordsWanted );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbGetDataDirInfo
	 */
	SRBMetaDataRecordList[] srbGetDataDirInfo( int catType,
		MetaDataCondition[] conditions, MetaDataSelect[] selects,
		int recordsWanted, boolean orderBy, boolean nonDistinct )
		throws IOException
	{
		if ( recordsWanted < 1 ) {
			//use default
			recordsWanted = DEFAULT_RECORDS_WANTED;
		}
/*TODO no longer needed?
		SRBMetaDataRecordList[] rl =  commands.srbGetDataDirInfo(
			catType, conditions, selects, recordsWanted, orderBy, nonDistinct );

		if (rl != null) {
			//The definable metadata uses internal IDs, these should be removed.
			for (int i=0;i<selects.length;i++) {
				if (selects[i] != null) {
					if (SRBMetaDataSet.getSRBID( selects[i].getFieldName() ) < 0) {
						for (int j=0;j<rl.length;j++) {
							//don't show FILE_ID or USER_ID to the user
							rl[j].removeRecord( MetaDataSet.getField( SRBMetaDataSet.FILE_ID ) );
							rl[j].removeRecord( MetaDataSet.getField( SRBMetaDataSet.USER_ID ) );
						}
					}
				}
			}
		}

		return rl;
*/
return commands.srbGetDataDirInfo(
			catType, conditions, selects, recordsWanted, orderBy, nonDistinct );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbGenQuery
	 */
	SRBMetaDataRecordList[] srbGenQuery( int catType, String myMcatZone,
		MetaDataCondition[] conditions, MetaDataSelect[] selects,
		int recordsWanted, boolean orderBy, boolean nonDistinct )
		throws IOException
	{
		if ( recordsWanted < 1 ) {
			//use default
			recordsWanted = DEFAULT_RECORDS_WANTED;
		}

		SRBMetaDataRecordList[] rl =  commands.srbGenQuery( catType, myMcatZone,
			conditions, selects, recordsWanted, orderBy, nonDistinct );

		if (rl != null) {
			//The definable metadata uses internal IDs, these should be removed.
			for (int i=0;i<selects.length;i++) {
				if (SRBMetaDataSet.getSRBID( selects[i].getFieldName() ) < 0) {
					for (int j=0;j<rl.length;j++) {
						//don't show FILE_ID or USER_ID to the user
						rl[j].removeRecord( MetaDataSet.getField( SRBMetaDataSet.FILE_ID ) );
						rl[j].removeRecord( MetaDataSet.getField( SRBMetaDataSet.USER_ID ) );
					}
				}
			}
		}

		return rl;
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbCollSeek
	 */
	void srbRegisterDataset( int catType, String objID, String dataTypeName,
		String resourceName, String collectionName,	String pathName,
		long dataSize )
		throws IOException
	{
		if (dataTypeName == null) {
			dataTypeName = "generic";//DefMdasDataTypeName
		}

		if (resourceName == null) {
			resourceName = getDefaultStorageResource();//DefMdasResourceName
		}

		if (collectionName == null) {
			collectionName = getHomeDirectory();
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName = getHomeDirectory() + collectionName;
		}

		commands.srbRegisterDataset( catType, objID, dataTypeName,
			resourceName, collectionName, pathName, dataSize );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbModifyDataset
	 */
	int srbModifyDataset( int catType, String objID, String collectionName,
		String resourceName, String pathName, String dataValue1,
		String dataValue2, int actionType )
		throws IOException
	{
		if (collectionName == null) {
			collectionName = getHomeDirectory();//DefMdasCollectionName
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		if ((pathName == null) && (resourceName == null))
		{
//Actually this should mean replica number is being used.
/*
			MetaDataRecordList[] rl = null;
			MetaDataCondition[] conditions = {
				MetaDataSet.newCondition( SRBMetaDataSet.DIRECTORY_NAME,
					MetaDataCondition.EQUAL, collectionName ),
				MetaDataSet.newCondition( SRBMetaDataSet.FILE_NAME,
					MetaDataCondition.EQUAL, objID ) };
			MetaDataSelect[] selects = {
				MetaDataSet.newSelection( SRBMetaDataSet.PATH_NAME ) };

			try {
				rl = query( conditions, selects, 3 );

				if( rl == null )
					pathName = "";
				else
					pathName = rl[0].getStringValue(0);
			} catch ( IOException e ) {
					pathName = "";
			}
*/
		}
		else if (resourceName == null) {
			resourceName = getDefaultStorageResource();//DefMdasResourceName
		}
		else if (pathName == null) {
			MetaDataCondition[] conditions = {
				MetaDataSet.newCondition(
					SRBMetaDataSet.FILE_NAME, MetaDataCondition.EQUAL, objID),
				MetaDataSet.newCondition(
					SRBMetaDataSet.DIRECTORY_NAME, MetaDataCondition.EQUAL,
					collectionName)
			};
			MetaDataSelect[] selects = {
				MetaDataSet.newSelection(SRBMetaDataSet.PATH_NAME) };
			MetaDataRecordList rl[] = query( conditions, selects, 3 );
			pathName = rl[0].getValue( SRBMetaDataSet.PATH_NAME ).toString();
		}


		return commands.srbModifyDataset( catType, objID, collectionName,
			resourceName, pathName, dataValue1, dataValue2, actionType );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbChkMdasAuth
	 */
	void srbChkMdasAuth( String userName, String srbAuth, String mdasDomain )
		throws IOException
	{
		if ( (userName == null ) || (srbAuth == null ))
			throw new NullPointerException(
				"Null value entered for Mdas authorization");

		if ( mdasDomain == null )
			mdasDomain = getDomainName();//default user mdas home domain

		commands.srbChkMdasAuth( userName, srbAuth, mdasDomain );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbCreateCollect
	 */
	void srbCreateCollect( int catType, String parentCollection,
		String newCollection )
		throws IOException
	{
		if (parentCollection == null) {
			parentCollection = getHomeDirectory();//DefMdasCollectionName
		}
		else if (!parentCollection.startsWith(SRB_ROOT)) {
			parentCollection =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + parentCollection;
		}

		commands.srbCreateCollect( catType, parentCollection,	newCollection );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbListCollect
	 */
	SRBMetaDataRecordList[] srbListCollect( int catType, String collectionName,
		String flag, int recordsWanted)
		throws IOException
	{
		if (collectionName == null) {
			collectionName = getHomeDirectory();//DefMdasCollectionName
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		if ( recordsWanted < 1 ) {
			//use default
			recordsWanted = DEFAULT_RECORDS_WANTED;
		}

		return commands.srbListCollect( catType, collectionName, flag,
			recordsWanted );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbModifyCollect
	 */
	void srbModifyCollect( int catType, String collectionName,
		String dataValue1, String dataValue2,	String dataValue3,
		int actionType )
		throws IOException
	{
		if (collectionName == null) {
			collectionName = getHomeDirectory();//DefMdasCollectionName
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		commands.srbModifyCollect( catType, collectionName,	dataValue1,
			dataValue2,	dataValue3, actionType );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbChkMdasSysAuth
	 */
	void srbChkMdasSysAuth( String userName, String srbAuth, String mdasDomain )
		throws IOException
	{

		commands.srbChkMdasSysAuth( userName, srbAuth, mdasDomain );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRegisterUserGrp
	 */
	/**
	 * Register a user group
	 *
	 * @param	catType	catalog type. e.g., MDAS_CATALOG.
	 * @param	userGrpName	The name of the user group to register.
	 * @param	userGrpPasswd	The user group passwd.
	 * @param	userGrpType	The user group type. Currently, at SDSC valid
	 *              userType are:
	 *              "staff", "sdsc staff", "sdsc staff scientist",
	 *              "sdsc senior staff scientist", "pto staff", "ucsd staff"
	 *              "student", "sdsc student", "uva student", "project",
	 *              "umd student", "public", "sysadmin", " deleted"
	 * @param	userGrpAddress	The mailing address of the user group.
	 * @param	userGrpPhone	The phone number of the user group.
	 * @param	userGrpEmail	The Email address of the user group.
	 * @throws IOException	If an IOException occurs
	 */
	public void srbRegisterUserGrp( int catType, String userGrpName,
		String userGrpPasswd, String userGrpType, String userGrpAddress,
		String userGrpPhone, String userGrpEmail )
		throws IOException
	{

		commands.srbRegisterUserGrp( catType, userGrpName, userGrpPasswd,
			userGrpType, userGrpAddress, userGrpPhone, userGrpEmail );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRegisterUser
	 */
	/**
	 * Register a user.
	 *
	 * @param	catType	catalog type. e.g., MDAS_CATALOG.
	 * @param	userName	The name of the user to register.
	 * @param	userDomain	The domain of the user to register.
	 * @param	userPasswd	The user passwd.
	 * @param	userType	The user type. Currently, at SDSC valid userType
	 *	        are:
	 *		"staff", "sdsc staff", "sdsc staff scientist",
	 *		"sdsc senior staff scientist", "pto staff", "ucsd staff"
	 *		"student", "sdsc student", "uva student", "project",
	 *		"umd student", "public", "sysadmin", " deleted"
	 * @param	userAddress	The mailing address of the user.
	 * @param	userPhone	The phone number of the user.
	 * @param	userEmail	The Email address of the user.
	 * @throws IOException	If an IOException occurs
	 */
	public void srbRegisterUser( int catType, String userName, String userDomain,
		String userPasswd, String userType, String userAddress, String userPhone,
		String userEmail )
		throws IOException
	{

		commands.srbRegisterUser( catType, userName, userDomain, userPasswd,
			userType, userAddress, userPhone, userEmail );
	}


	/*
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbModifyUser
	 */
	/**
	 * Modify a user info.
	 *
	 * @param	catType	catalog type. e.g., MDAS_CATALOG.
	 * @param	dataValue1, String dataValue2 and int actionType -
	 *	   They are used to specify the user attributes to modify. A normal
	 *	   user may use it to modify his/her own password and a limited
	 *	   set of attributes. A user with MDAS sys admin privilege can
	 *	   also use these input values to modify other user's attributes.
	 * @throws IOException	If an IOException occurs
	 */
	public void srbModifyUser( int catType, String dataValue1, String dataValue2,
		int actionType )
		throws IOException
	{
		if (catType < 0)
			catType = 0;

		commands.srbModifyUser( catType, dataValue1, dataValue2,
			actionType );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbSetAuditTrail
	 */
	int srbSetAuditTrail( int set_value )
		throws IOException
	{

		return commands.srbSetAuditTrail( set_value );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjAudit
	 */
	void srbObjAudit( int catType, String userName, String objID,
		String collectionName, String dataPath, String resourceName,
		String accessMode, String comment, int success, String domainName )
		throws IOException
	{

		if (resourceName == null) {
			resourceName = getDefaultStorageResource();//DefMdasResourceName
		}

		if (collectionName == null) {
			collectionName = getHomeDirectory();//DefMdasCollectionName
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		commands.srbObjAudit( catType, userName, objID, collectionName,
			dataPath, resourceName, accessMode, comment, success, domainName );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRegisterReplica
	 */
	void srbRegisterReplica( int catType, String objID, String collectionName,
		String origResourceName, String origPathName, String newResourceName,
		String newPathName, String userName, String domainName )
		throws IOException
	{
		if (origResourceName == null) {
			origResourceName = getDefaultStorageResource();//DefMdasResourceName
		}

		if (collectionName == null) {
			collectionName = getHomeDirectory();//DefMdasCollectionName
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		commands.srbRegisterReplica( catType, objID, collectionName,
			origResourceName, origPathName, newResourceName, newPathName,
			userName, domainName );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbGetPrivUsers
	 */
	SRBMetaDataRecordList[] srbGetPrivUsers( int catalog, int recordsWanted )
		throws IOException
	{

		if ( recordsWanted < 1 ) {
			//use default
			recordsWanted = DEFAULT_RECORDS_WANTED;
		}

		return commands.srbGetPrivUsers( catalog, recordsWanted );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbGetMoreRows
	 */
	SRBMetaDataRecordList[] srbGetMoreRows( int catalog, int contDesc,
		int recordsWanted)
		throws IOException
	{

		if ( recordsWanted < 1 ) {
			//use default
			recordsWanted = DEFAULT_RECORDS_WANTED;
		}

		return commands.srbGetMoreRows( catalog, contDesc, recordsWanted );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbIssueTicket
	 */
	void srbIssueTicket( String objID, String collectionName,
		String collectionFlag, String beginTime, String endTime, int accessCnt,
		String ticketUser )
		throws IOException
	{
		if (collectionName == null) {
			collectionName = getHomeDirectory();//DefMdasCollectionName
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		commands.srbIssueTicket( objID, collectionName, collectionFlag,
			beginTime, endTime, accessCnt, ticketUser );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRemoveTicket
	 */
	void srbRemoveTicket( String ticket )
		throws IOException
	{

		commands.srbRemoveTicket( ticket );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbUnregisterDataset
	 */
	void srbUnregisterDataset( String objID, String collectionName )
		throws IOException
	{
		if (collectionName == null) {
			collectionName = getHomeDirectory();//DefMdasCollectionName
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		commands.srbUnregisterDataset( objID, collectionName );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbContainerCreate
	 */
	void srbContainerCreate( int catType, String containerName,
		String containerType, String resourceName, long containerSize )
		throws IOException
	{
		if (resourceName == null) {
			resourceName = getDefaultStorageResource();//DefMdasResourceName
		}

		commands.srbContainerCreate( catType, containerName,
			containerType, resourceName, containerSize );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRegisterContainer
	 */
	void srbRegisterContainer( int catType, String containerName,
	 String resourceName, long containerSize )
		throws IOException
	{
		if (resourceName == null) {
			resourceName = getDefaultStorageResource();//DefMdasResourceName
		}

		commands.srbRegisterContainer( catType, containerName, resourceName,
			containerSize );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRegisterInContDataset
	 */
	void srbRegisterInContDataset( int catType, String objID,
		String collectionName, String containerName, String dataTypeName,
		long dataSize, long baseOffset )
		throws IOException
	{
		if (dataTypeName == null) {
			dataTypeName = "generic";//DefMdasDataTypeName
		}

		if (collectionName == null) {
			collectionName = getHomeDirectory();//DefMdasCollectionName
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		commands.srbRegisterInContDataset( catType, objID, collectionName,
			containerName, dataTypeName, dataSize, baseOffset );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbGetContainerInfo
	 */
	SRBMetaDataRecordList[] srbGetContainerInfo( int catType,
		String containerName, int recordsWanted )
		throws IOException
	{
		if ( recordsWanted < 1 ) {
			//use default
			recordsWanted = DEFAULT_RECORDS_WANTED;
		}

		return commands.srbGetContainerInfo(catType, containerName, recordsWanted);
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbGetResOnChoice
	 */
	String srbGetResOnChoice( int catType, String logResName, String phyResName,
		String inputFlag )
		throws IOException
	{

		return commands.srbGetResOnChoice( catType, logResName, phyResName,
			inputFlag );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRmContainer
	 */
	void srbRmContainer( int catType, String containerName, boolean force )
		throws IOException
	{
		commands.srbRmContainer( catType, containerName, force );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbSyncContainer
	 */
	void srbSyncContainer( int catType, String containerName, int syncFlag )
		throws IOException
	{

		commands.srbSyncContainer( catType, containerName, syncFlag );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see edu.sdsc.grid.io.srb.SRBCommands#srbReplContainer
	 */
	void srbReplContainer( int catType, String containerName,
		String newResourceName )
		throws IOException
	{
		if (newResourceName == null) {
			newResourceName = getDefaultStorageResource();//DefMdasResourceName
		}

		commands.srbReplContainer( catType, containerName, newResourceName );
	}



	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjOpenWithTicket
	 */
	int	srbObjOpenWithTicket( String objID, int oflag, String collectionName,
		String ticket )
		throws IOException
	{

//		return commands.srbObjOpenWithTicket( objID, oflag, collectionName,
//			ticket );
return 0;
	}



//------------------------------------------------------------------------
//	SRB 2.0 functions
//------------------------------------------------------------------------
	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbTapelibMntCart
	 */
	String[] srbTapelibMntCart( String tapeNum, int priority )
		throws IOException
	{
//TODO
//		return commands.srbTapelibMntCart( tapeNum, priority );
	return null;
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbTapelibDismntCart
	 */
	void srbTapelibDismntCart( String tapeNum )
		throws IOException
	{
		commands.srbTapelibDismntCart( tapeNum );
	}



	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbGetTapeCartPri
	 */
/*TODO
	void srbGetTapeCartPri( tapeCartPri_t **outTapeCartPri )
		throws IOException
	{
		commands.srbGetTapeCartPri( outTapeCartPri );
	}
*/



	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbDumpFileList
	 */
/*TODO what is fileList_t?
	void srbDumpFileList( int tapeLibInx, fileList_t *fileList,
		int purgeFlag )
		throws IOException
	{
		commands.srbDumpFileList( tapeLibInx, fileList, purgeFlag );
	}
*/


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbStageCompObj
	 */
/*TODO what is struct mdasInfoOut
	void srbStageCompObj( String objID, String collectionName,
		String resourceLoc, String resourceName,
		srb_long_t offset, srb_long_t size,
		int replNum, int segNum, int stageFlag,
		struct mdasInfoOut **outCacheInfoOut )
		throws IOException
	{
		commands.srbStageCompObj( objID, collectionName, resourceLoc,
			resourceName, offset, size, replNum, segNum, stageFlag,
			outCacheInfoOut );
	}
*/



	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRegInternalCompObj
	 */
	void srbRegInternalCompObj( String objName, String objCollName,
		int objReplNum, int objSegNum, String intObjRescName, String dataPathName,
		long dataSize, long offset, int inpIntReplNum, int intSegNum,
		int objTypeInx, String phyResLoc )
		throws IOException
	{
		commands.srbRegInternalCompObj( objName, objCollName, objReplNum,
			objSegNum, intObjRescName, dataPathName, dataSize, offset,
			inpIntReplNum, intSegNum,	objTypeInx, phyResLoc );
	}



	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRmIntCompObj
	 */
	void srbRmIntCompObj( String objName, String objCollName,
		int objReplNum, int objSegNum, int inpIntReplNum, int intSegNum )
		throws IOException
	{
		commands.srbRmIntCompObj( objName, objCollName, objReplNum,
			objSegNum, inpIntReplNum, intSegNum );
	}



	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRmCompObj
	 */
	void srbRmCompObj( String objName, String objCollName, int objReplNum,
		int objSegNum )
		throws IOException
	{
		commands.srbRmCompObj( objName, objCollName, objReplNum,
			objSegNum );
	}



	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbModInternalCompObj
	 */
	void srbModInternalCompObj( String objID, String collectionName,
		int   objReplNum, int objSegNum, int inpIntReplNum, int intSegNum,
		String data_value_1, String data_value_2,
		String data_value_3, String data_value_4, int retraction_type )
		throws IOException
	{
		commands.srbModInternalCompObj( objID, collectionName,
			objReplNum, objSegNum, inpIntReplNum, intSegNum, data_value_1,
			data_value_2, data_value_3, data_value_4, retraction_type );
	}



	/*
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbModifyRescInfo
	 */
	/**
	 * Modify/create/delete a SRB resource.
	 *
	 * Input -
	 *	int catType - catalog type. e,g., MDAS_CATALOG.
	 *	String resourceName - The storage resource name.
	 * 		e.g. "mda18-unix-sdsc"
	 *	int actionType - The type of retraction. See srbC_mdas_externs.h
	 * 		for the actionType definition.
	 * 		For tapes, valid values are T_INSERT_TAPE_INFO,
	 * 		T_UPDATE_TAPE_INFO, T_UPDATE_TAPE_INFO_2, T_DELETE_TAPE_INFO.
	 *	String dataValue1 - Input value 1.
	 *	String dataValue2 - Input value 2.
	 *	String dataValue3 - Input value 3.
	 *	String dataValue4 - Input value 4.
	 */
	public void srbModifyRescInfo( int catType, String resourceName,
		int actionType, String dataValue1, String dataValue2,
		String dataValue3, String dataValue4 )
		throws IOException
	{
		commands.srbModifyRescInfo( catType, resourceName,	actionType,
			dataValue1, dataValue2, dataValue3, dataValue4 );
	}




	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRegisterLocation
	 */
	/**
	 * Register location information.
	 *
	 * Input -
	 *	   String locName - The location name.
	 *	   String fullAddr - Full Address.
	 *     String parentLoc - Parent location
	 *	   String serverUser - Server User
	 *     String serverUserDomain - Server User Domain.
	 */
	public void srbRegisterLocation( String locName, String fullAddr,
		String parentLoc, String serverUser, String serverUserDomain )
		throws IOException
	{
		commands.srbRegisterLocation( locName, fullAddr, parentLoc,
			serverUser, serverUserDomain );
	}



	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbIngestToken
	 */
	/**
	 * Ingest Token.
	 *
	 * Input -
	 *	   String typeName - The type name.
	 *	   String newValue - The new value.
	 *         String parentValue - Parent value.
	 */
	public void srbIngestToken( String typeName, String newValue,
		String parentValue )
		throws IOException
	{
		commands.srbIngestToken( typeName, newValue, parentValue );
	}



	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRegisterResource
	 */
	/**
	 * Register Resource.
	 *
	 * Input -
	 *	   String rescName - The resource name.
	 *	   String rescType - Resource type.
	 *	   String location - Location.
	 *	   String phyPath  -  Physical Path.
	 *	   String class    -  className.
	 *	   int size        -  size.
	 */
	public void srbRegisterResource( String rescName, String rescType,
		String location, String phyPath, String className, int size )
		throws IOException
	{
		commands.srbRegisterResource( rescName, rescType,
			location, phyPath, className, size );
	}



	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRegisterLogicalResource
	 */
	/**
	 * Register Logical Resource.
	 *
	 * Input -
	 *	   String rescName - The resource name.
	 *	   String rescType - Resource type.
	 *         String phyResc -  Physical resource.
	 *         String phyPath -  Physical path.
	 */
	public void srbRegisterLogicalResource( String rescName,
		String rescType, String phyResc, String phyPath )
		throws IOException
	{
		commands.srbRegisterLogicalResource( rescName,
			rescType, phyResc, phyPath );
	}



	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbRegisterReplicateResourceInfo
	 */
	/**
	 * srbRegisterReplicateResourceInfo
	 *
	 * Input -
	 *	   String physicalRescName - The physical resource name.
	 *	   String rescType - Resource type.
	 *         String oldLogicalRescName - old logical resource name.
	 *         String indefaultPath -  Indefault Path.
	 */
	public void srbRegisterReplicateResourceInfo( String physicalRescName,
		String rescType, String oldLogicalRescName,	String inDefaultPath )
		throws IOException
	{
		commands.srbRegisterReplicateResourceInfo( physicalRescName,
			rescType, oldLogicalRescName,	inDefaultPath );
	}



	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbDeleteValue
	 */
	/**
	 * Delete a Value; a single MCAT entry.
	 *
	 * Input -
	 *	   int valueType - the value (token) type.
	 *	   String deleteValue - The value (name) that is being deleted.
	 */
	public void srbDeleteValue( int valueType, String deleteValue )
		throws IOException
	{
		commands.srbDeleteValue( valueType, deleteValue );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbExecCommand
	 */
	InputStream	srbExecCommand( String command, String commandArgs,
		String hostAddress, int portalFlag )
		throws IOException
	{
		if (command == null) {
			throw new NullPointerException("No command given");
		}

		if ((portalFlag < 0) || (portalFlag > 2)) {
			portalFlag = PORTAL_STD_IN_OUT;
		}

		return commands.srbExecCommand( command, commandArgs,
			hostAddress, portalFlag, MIN_PORT, MAX_PORT );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 * Only valid for version 3.x.x and above.
	 * Method will not work for version 2.x.x, or before.
	 *
	 * @see edu.sdsc.grid.io.srb.SRBCommands#srbExecCommand
	 */
	InputStream srbExecCommand( String command, String commandArgs,
		String hostAddress, String fileName, int portalFlag )
		throws IOException
	{
		if (command == null) {
			throw new NullPointerException("No command given");
		}

		// Can not use hostAddress and fileName at same time
		if ((fileName != null) && (hostAddress != null)) {
			hostAddress = null;
		}

		if ((portalFlag < 0) || (portalFlag > 2)) {
			portalFlag = PORTAL_STD_IN_OUT;
		}

		return commands.srbExecCommand( command, commandArgs,
			hostAddress, fileName, portalFlag, MIN_PORT, MAX_PORT );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjPut
	 */
	long srbObjPut( String destObjID, String destCollection, String destResLoc,
		String dataType, String destPath, String locFilePath,	long size )
		throws IOException
	{
		if (destResLoc == null) {
			destResLoc = getDefaultStorageResource();//DefMdasResourceName
		}
		if (dataType == null) {
			dataType = "generic";//DefMdasDataTypeName
		}
		if (destPath == null) {
			destPath = "";//TODO need to do query PATH_NAME?
		}

		return commands.srbObjPut( destObjID, destCollection, destResLoc,
			dataType, destPath, locFilePath, size, MIN_PORT, MAX_PORT );
	}

	/**
	 * The client initiated version of srbObjPut. Copy a dataset
	 * from local space to SRB space.
	 *
	 * @param destObjID The destination objID.
	 * @param destCollection The destination collwction.
	 * @param destResLoc The destination resource.
	 * @param dataType The data type.
	 * @param destPath The destination path name.
	 * @param locFilePath The local fullPath name.
	 * @param size The size of the file. negative means don't know.
	 * @param forceFlag over write flag
	 * @param numThreads number of threads
	 * @return The number of bytes copied.
	 *          Returns a negative value upon failure.
	 */
	long srbObjPutClientInitiated( String destObjID, String destCollection,
		String destResLoc, String dataType, String destPath, String localFilePath,
		long srcSize, int forceFlag, int numThreads )
		throws IOException
	{
		if (destResLoc == null) {
			destResLoc = getDefaultStorageResource();//DefMdasResourceName
		}
		if (dataType == null) {
			dataType = "generic";//DefMdasDataTypeName
		}
		if (destPath == null) {
			destPath = "";//TODO need to do query PATH_NAME?
		}

		return commands.srbObjPutClientInitiated( destObjID, destCollection,
			destResLoc, dataType, destPath, localFilePath, srcSize, forceFlag,
			numThreads );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbObjGet
	 */
	long srbObjGet( String srcObjID, String srcCollection, long srcSize,
	 	GeneralFile file, boolean forceOverwrite )
		throws IOException
	{
		if ((srcObjID == null) || (srcCollection == null) || (file == null))
		{
			throw new NullPointerException();
		}

		return commands.srbObjGet( srcObjID, srcCollection, srcSize, file,
			MIN_PORT, MAX_PORT, forceOverwrite );
	}

	/**
	 * The client initiated version of srbObjGet. Copy a dataset
	 * from SRB space to local space.
	 *
	 * @param srcObjID The source objID.
	 * @param srcCollection The source collwction.
	 * @param localFilePath The local fullPath name.
	 * @param flag not used currently
	 * @param numThreads number of threads
	 * @return The number of bytes copied.
	 *          Returns a negative value upon failure.
	 */
	synchronized long srbObjGetClientInitiated( String srcObjID,
		String srcCollection,	GeneralFile file, int flag, int numThreads )
		throws IOException
	{
		if ((srcObjID == null) || (srcCollection == null) || (file == null))
		{
			throw new NullPointerException();
		}
		if (numThreads < 1) {
			numThreads = 1;
		}

		return commands.srbObjGetClientInitiated( srcObjID, srcCollection,
			file, flag, numThreads );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbBulkRegister
	 */
	void srbBulkRegister( int catType, String bulkLoadFilePath,
		SRBMetaDataRecordList[] rl )
		throws IOException
	{
		if (bulkLoadFilePath == null) {
			throw new NullPointerException();
		}
		if (rl == null) {
			throw new NullPointerException();
		}

		commands.srbBulkRegister( catType, bulkLoadFilePath, rl );
	}



//--------------------------------------------------------------------
// SRB 3.0 functions
//--------------------------------------------------------------------
	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbGetMcatZone
	 */
	void srbGetMcatZone( String userName, String domainName,
		String mcatName )
		throws IOException
	{
		commands.srbGetMcatZone( userName, domainName, mcatName );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbSetupSessionPublicKey
	 */
	void srbSetupSessionPublicKey( String publicKey )
		throws IOException
	{
		commands.srbSetupSessionPublicKey( publicKey );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbSetupSession
	 */
	void srbSetupSession( String sessionKey )
		throws IOException
	{
		commands.srbSetupSession( sessionKey );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbBulkLoad
	 */
	void srbBulkLoad( int catType, String bulkLoadFilePath,
		SRBMetaDataRecordList[] rl )
		throws IOException
	{
		if (bulkLoadFilePath == null) {
			throw new NullPointerException();
		}
		if (rl == null) {
			throw new NullPointerException();
		}
		if (rl.length == 0) {
			//TODO? throw new IllegalArgumentException();
			return;
		}

		commands.srbBulkLoad( catType, bulkLoadFilePath, rl );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbBulkLoad
	 */
	void srbBulkUnload( int catType, int flag, String srbUnloadDirPath,
		String localDirPath )
		throws IOException
	{
		if (srbUnloadDirPath == null) {
			throw new NullPointerException();
		}
		if (localDirPath == null) {
			throw new NullPointerException();
		}

		commands.srbBulkUnload( catType, flag, srbUnloadDirPath, localDirPath );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbModifyZone
	 */
//TODO temp, all the public	 ones
	/**
	 * Modify and Insert SRB zone and zone information in the
	 *  Metadata Catalog. Information about the operation  performed is
	 *  also logged in the audit  trail (if turned on). This is a
	 *  privileged function and should be called only by a
	 *  srbAdmin user.
	 *
	 * @param  catType      - catalog type. e,g., MDAS_CATALOG.
	 * @param zone_name  - name of the zone
	 * @param dataValue1 - Input value 1.
	 * @param dataValue2 - Input value 2.
	 * @param dataValue3 - Input value 3.
	 * @param dataValue4 - Input value 4.
	 * @param dataValue5 - Input value 5.
	 * @param actionType  - The type of action. performed
	 *		values supported are:<br>
	 *                 INSERT_NEW_LOCAL_ZONE<br>
	 *                     dv1 = locn_desc<br>
	 *                     dv2 = port_number<br>
	 *                     dv3 = username@domain of remote MCAT admin<br>
	 *                     dv4 = zone_contact<br>
	 *                     dv5 = zone_comment<br>
	 *                 INSERT_NEW_ALIEN_ZONE<br>
	 *                     dv1-5 = same as for INSERT_NEW_LOCAL_ZONE<br>
	 *                 MODIFY_ZONE_INFO<br>
	 *                     dv1-5 = same as for INSERT_NEW_LOCAL_ZONE<br>
	 *                           empty string implies no change.<br>
	 *                 MODIFY_ZONE_FOR_USER<br>
	 *                     dv1 = user_name<br>
	 *                     dv2 = domain_name<br>
	 *                 CHANGE_ZONE_NAME<br>
	 *                     dv1 = new name<br>
	 *                 MODIFY_ZONE_LOCAL_FLAG<br>
	 *                     dv1 = new value (integer)<br>
	 *                 MODIFY_ZONE_STATUS<br>
	 *                     dv1 = new value (integer)<br>
	 */
	public void srbModifyZone( int catType, String zoneName,
		String dataValue1, String dataValue2, String dataValue3,
		String dataValue4, String dataValue5, int actionType )
		throws IOException
	{
		commands.srbModifyZone( catType, zoneName, dataValue1, dataValue2,
			dataValue3, dataValue4, dataValue5, actionType );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbBulkQueryAnswer
	 */
	MetaDataRecordList[] srbBulkQueryAnswer( int catType,	String queryInfo,
		MetaDataRecordList myresult, int rowsWanted )
		throws IOException
	{
		return commands.srbBulkQueryAnswer( catType, queryInfo, myresult,
			rowsWanted );
	}


	/**
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbBulkMcatIngest
	 */
	void srbBulkMcatIngest( int catType, String ingestInfo,
		SRBMetaDataRecordList[] rl )
		throws IOException
	{
		commands.srbBulkMcatIngest( catType, ingestInfo, rl );
	}

//SRB3.0.2
	/*
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see
	/**
	 * Backup a data object -  Make a replica to the backup
	 * resource. Skip it if a good copy already exist.
	 *
	 * @param catType - catalog type - 0 - MCAT
	 * @param objID The SRB object ID to Sync.
	 * @param collectionName The name of the collection this objID
	 *		belongs.
	 * @param backupResource - The backup resource
	 * @param flag - not used.
	 */
	synchronized void srbBackupData( int catType, String objID,
		String collectionName, String backupResource, int flag )
		throws IOException
	{
		if (collectionName == null) {
			collectionName = getHomeDirectory();
		}
		else if (!collectionName.startsWith(SRB_ROOT)) {
			collectionName =
				getHomeDirectory() + SRBFile.PATH_SEPARATOR + collectionName;
		}

		if (backupResource == null) {
			backupResource = getDefaultStorageResource();//DefMdasResourceName
		}

		commands.srbBackupData( catType, objID, collectionName,
			backupResource, flag );
	}

//SRB3.1
	/*
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbModifyResource
	 */
	/**
	 * Checksum a SRB data file. By default, if the chksum
	 * already already exists, do nothing and return the chksum value.
	 * If the chksum does not exist, compute and register it.
	 *
	 * @param objID The data name.
	 * @param collectionName The collection name.
	 * @param chksumFlag valid flags are :<br>
	 *	     l_FLAG - list the registered chksum value.<br>
	 *	     c_FLAG - compute chksum, but don't register<br>
	 *	     f_FLAG - force compute and register of chksum even if one
	 *	      already exist.
	 * @param inpChksum Not used.
	 *
	 * @return the checksum value
	 */
	String srbObjChksum( String objID, String collectionName,	int chksumFlag,
		String inpChksum )
		throws IOException
	{
		return commands.srbObjChksum( objID, collectionName, chksumFlag,
			inpChksum );
	}



	/*
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbModifyResource
	 */
	/**
	 * Modify and Insert SRB user information in the
	 * Metadata Catalog. Information about the operation  performed is
	 * also logged in the audit  trail (if turned on). This is a
	 * non-privileged function.
	 *
	 * @param catType catalog type. e,g., MDAS_CATALOG.
	 * @param userNameDomain name@domain of the user
	 * @param dataValue1 Input value 1.
	 * @param dataValue2 Input value 2.
	 * @param dataValue3 Input value 3.
	 * @param dataValue4 Input value 4.
	 * @param dataValue5 Input value 5.
	 * @param actionType The type of action. performed
	 *		values supported are:
	 */
	void srbModifyUserNonPriv( int catType, String userNameDomain,
		String dataValue1, String dataValue2, String dataValue3, String dataValue4,
		String dataValue5, int actionType)
			throws IOException
	{
		commands.srbModifyUserNonPriv( catType, userNameDomain, dataValue1,
			dataValue2, dataValue3, dataValue4, dataValue5, actionType);
	}


	/*
	 * Ensures all variables have reason values,
	 * then calls the SRBCommand function of the same name.
	 *
	 * @see	edu.sdsc.grid.io.srb.SRBCommands#srbModifyResource
	 */
//TODO isn't this the same as srbModifyRescInfo?
	/**
	 * Modify and Insert SRB resource information in the
	 *  Metadata Catalog. Information about the operation  performed is
	 *  also logged in the audit  trail (if turned on). This is a
	 *  privileged function and should be called only by a
	 *  srbAdmin user.
	 *
	 * @param catType catalog type. e,g., MDAS_CATALOG.
	 * @param resource_name name of the resource
	 * @param dataValue1 Input value 1.
	 * @param dataValue2 Input value 2.
	 * @param dataValue3 Input value 3.
	 * @param dataValue4 Input value 4.
	 * @param actionType The type of action. performed
	 *                  values supported are:
	 */
	void srbModifyResource( int catType, String resourceName,
		String dataValue1, String dataValue2, String dataValue3, String dataValue4,
		int actionType )
		throws IOException
	{
		if (resourceName == null) {
			resourceName = getDefaultStorageResource();//DefMdasResourceName
		}

		commands.srbModifyResource( catType, resourceName,
			dataValue1, dataValue2, dataValue3, dataValue4, actionType );
	}
}
