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
//	SRBAccount.java	-  edu.sdsc.grid.io.SRBAccount
//
//  CLASS HIERARCHY
//	java.lang.Object
//	    |
//	    +-edu.sdsc.grid.io.GeneralAccount
//	 			   |
//	 			   +-.RemoteAccount
//	 						   |
//	 						   +-.srb.SRBAccount
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
import java.io.FileReader;
import java.io.File;
import java.util.StringTokenizer;

/**
 * This class extends the RemoteAccount class, adding those values necessary
 * to open a connection to the SRB.
 *<P>
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
public class SRBAccount extends RemoteAccount
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
	//48 = "0"
	/**
	 * Plain text password. Only supported in SRB version 1.1.8
	 */
	public static final int PASSWD_AUTH = 48;

	//49 = "1"
//	public static final int SEA_AUTH = 49;
	//50 = "2"
//	public static final int SEA_ENCRYPT = 50;

	//51 = "3"
	/**
	 * GSI authentication protocol
	 */
	public static final int GSI_AUTH = 51;
	//52 = "4"
//	public static final int GSI_SECURE_COMM = 52;

	//53 = "5"
	/**
	 * Encrypted text password.
	 */
	public static final int ENCRYPT1 = 53;

	//54 = "6"
	/**
	 * GSI delegated authentication protocol
	 */
	public static final int GSI_DELEGATE = 54;



//Be sure to change setVersion() when adding new SRB versions

	/**
	 * SRB version 3.0.2
	 */
	public static final String SRB_VERSION_3_0_2 = "SRB-3.0.2&F";


	/**
	 * SRB version 3.0.0
	 */
	public static final String SRB_VERSION_3 = "SRB-3.0.0&E";

	/**
	 * All SRB version 2
	 */
	public static final String SRB_VERSION_2 = "SRB2.0.0&D";


	/**
	 * SRB version 1.1.8.
	 * The earliest SRB version tested for compatibility with this API.
	 */
	public static final String SRB_VERSION_1_1_8 = "SRB1.1.8&C";




//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * The authorizing user name.
	 */
	protected String proxyUserName;

	/**
	 * The mdas domain home of the user.
	 */
	protected String proxyDomainName;

	/**
	 * The mdas domain home of the user. A domain is a string used to
	 * identify a site or project. Users are uniquely identified by their
	 * usernames combined with their domain 'smith@npaci'. A SRB admin has the
	 * authority to create domains.
	 */
	protected String domainName;


	/**
	 * The default storage resource.
	 */
	protected String defaultStorageResource;


	/**
	 * The srb options. Used to select an authorization scheme.
	 * <ul>
	 *  <li> GSI_AUTH (not yet supported)
	 *  <li> GSI_SECURE_COMM (not yet supported)
	 *  <li> ENCRYPT1 (Default)
	 */
	protected int options = ENCRYPT1;


//SRB 3.0
	/**
	 * The proxy mcat zone.
	 */
	protected String proxyMcatZone;


	/**
	 * The client mcat zone. Added as of SRB3.0. Used to address a user
	 * within federated MCATs. Federatation of SRBs allows multiple SRBs to
	 * communicate with each even if they
	 * use different MCAT database.
	 * use differ
	 */
	protected String clientMcatZone;


	/**
	 * The exec file.
	 */
	protected String execFile;


//for GSI
	/**
	 * The certificate authority (CA) list. By default, the CA definition
	 * comes from the user's cog.properties file.
	 */
	protected String certificateAuthority;



	/**
	 * The srb version.
	 */
	protected static String version = SRB_VERSION_3_0_2;


//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * This constructor uses the default info found in the Mdas files in
	 * the user's home directory.
	 *
	 * @throws FileNotFoundException if the user info cannot be found.
	 * @throws	IOException if the user info exists but cannot be opened or
	 *			created for any other reason.
	 */
	public SRBAccount( )
		throws FileNotFoundException, IOException
	{
		this( new LocalFile( System.getProperty("user.home") + "/.srb/" ) );
	}

	/**
	 * Creates an object to hold SRB account information.
	 * <P>
	 * @param	userInfoDirectory directory holding the .Mdas files
	 * @throws FileNotFoundException if the user info cannot be found.
	 * @throws	IOException if the user info exists but cannot be opened or
	 *			created for any other reason.
	 */
	public SRBAccount( String userInfoDirectory )
		throws FileNotFoundException, IOException
	{
		this( new LocalFile( userInfoDirectory ) );
	}

	/**
	 * Creates an object to hold SRB account information.
	 * <P>
	 * @param	userInfoDirectory directory holding the .Mdas files
	 * @throws FileNotFoundException if the user info cannot be found.
	 * @throws	IOException if the user info exists but cannot be opened or
	 *			created for any other reason.
	 */
	public SRBAccount( LocalFile userInfoDirectory )
		throws FileNotFoundException, IOException
	{
		//Can't actually do anything until the .Mdas files have been read.
		super( "", 0, "", "", "" );

		if ( userInfoDirectory.equals(null) )
			throw new FileNotFoundException();

		setUserInfo( userInfoDirectory );
	}


	/**
	 * Creates an object to hold SRB account information.
	 * <P>
	 * @param	userInfoDirectory directory holding the .Mdas files
	 * @throws FileNotFoundException if the user info cannot be found.
	 * @throws	IOException if the user info exists but cannot be opened or
	 *			created for any other reason.
	 */
	public SRBAccount( File userInfoDirectory )
		throws FileNotFoundException, IOException
	{
		this( new LocalFile( userInfoDirectory ));
	}


	/**
	 * Creates an object to hold SRB account information.
	 * This constructor does not use any default info.
	 * <P>
	 * @param	host the SRB server domain name
	 * @param	port the port on the SRB server
	 * @param	userName the user name
	 * @param	password the password
	 * @param	homeDirectory home directory on the SRB
	 * @param	mdasDomainName the mdas home domain
	 * @param	defaultStorageResource default storage resource
	 */
	public SRBAccount( String host, int port, String userName, String password,
		String homeDirectory, String mdasDomainName, String defaultStorageResource )
	{
		super( host, port, userName, password, homeDirectory );

		setProxyUserName( userName );
		setProxyDomainName( mdasDomainName );
		setDomainName( mdasDomainName );
		setDefaultStorageResource( defaultStorageResource );
	}



	/**
	 * Creates an object to hold SRB account information.
	 * This constructor does not use any default info.
	 * <P>
	 * @param	host the SRB server domain name
	 * @param	port the port on the SRB server
	 * @param	userName the user name
	 * @param	password the password
	 * @param	homeDirectory home directory on the SRB
	 * @param	mdasDomainName the mdas home domain
	 * @param	defaultStorageResource default storage resource
	 * @param	mcatZone mcat zone
	 */
	public SRBAccount( String host, int port, String userName, String password,
		String homeDirectory, String mdasDomainName, String defaultStorageResource,
		String mcatZone )
	{
		super( host, port, userName, password, homeDirectory );

		setProxyUserName( userName );
		setProxyDomainName( mdasDomainName );
		setDomainName( mdasDomainName );
		setDefaultStorageResource( defaultStorageResource );
		setProxyMcatZone( mcatZone );
		setMcatZone( mcatZone );
	}


	/**
	 * Finalizes the object by explicitly letting go of each of
	 * its internally held values.
	 * <P>
	 */
	protected void finalize( )
	{
		super.finalize();
		proxyUserName = null;
		proxyDomainName = null;
		domainName = null;
		defaultStorageResource = null;
	}



//----------------------------------------------------------------------
// Setters and Getters
//----------------------------------------------------------------------
	/**
	 * Sets the home directory of this RemoteAccount.
	 *
	 * @throws	NullPointerException	if homeDirectory is null.
	 */
	public void setHomeDirectory( String homeDirectory )
	{
		if ( homeDirectory == null )
			throw new NullPointerException(
				"The home directory string cannot be null");

		this.homeDirectory = homeDirectory;
	}


	/**
	 * Sets the proxy user name.
	 *
	 * @throws	NullPointerException	if proxyUserName is null.
	 */
	protected void setProxyUserName( String proxyUserName )
	{
		if ( proxyUserName == null )
			throw new NullPointerException("The proxy user name cannot be null");

		this.proxyUserName = proxyUserName;
	}


	/**
	 * Sets the proxy domain name.
	 *
	 * @throws	NullPointerException	if proxyDomainName is null.
	 */
	protected void setProxyDomainName( String proxyDomainName )
	{
		if ( proxyDomainName == null )
			throw new NullPointerException("The proxy domain name cannot be null");

		this.proxyDomainName = proxyDomainName;
	}

	/**
	 * Sets the domain name used by the client.
	 *
	 * @throws	NullPointerException	if domainName is null.
	 */
	public void setDomainName( String domainName )
	{
		if ( domainName == null )
			throw new NullPointerException("The domain name cannot be null");

		this.domainName = domainName;
	}

	/**
	 * Sets the default storage resource.
	 *
	 * @throws	NullPointerException	if defaultStorageResource is null.
	 */
	public void setDefaultStorageResource( String defaultStorageResource )
	{
		if ( defaultStorageResource == null ) {
			throw new NullPointerException(
				"The default storage resource cannot be null");
		}

		this.defaultStorageResource = defaultStorageResource;
	}

	/**
	 * Set the type of authentication used, e.g. ENCRYPT1, GSI_AUTH.
	 *
	 * @throws	NullPointerException	if options is null.
	 */
	public void setOptions( int options )
	{
		this.options = options;
	}

	/**
	 * Sets the version.
	 *
	 * @throws	NullPointerException	if version is null.
	 */
	public static void setVersion( String srbVersion )
	{
		if ((srbVersion == null ) || (!srbVersion.equals(SRB_VERSION_1_1_8)
			&& !srbVersion.equals(SRB_VERSION_2)
			&& !srbVersion.equals(SRB_VERSION_3)
			&& !srbVersion.equals(SRB_VERSION_3_0_2)))
			throw new IllegalArgumentException( "Invalid version" );


		version = srbVersion;
	}


//SRB 3.0
	/**
	 * Sets the proxy mcat zone.
	 */
	public void setProxyMcatZone( String proxyMcatZone )
	{
		this.proxyMcatZone = proxyMcatZone;
	}


	/**
	 * Sets the client mcat zone.
	 */
	public void setMcatZone( String clientMcatZone )
	{
		this.clientMcatZone = clientMcatZone;
	}


	/**
	 * Sets the exec file.
	 */
	public void setExecFile( String execFile )
	{
		this.execFile = execFile;
	}


//for GSI
	/**
	 * Sets the locations of the GSI Certificate Authority (CA). The list
	 * can contain multiple files that are comma separated. By default,
	 * the CA definition comes from the user's cog.properties file.
	 */
	public void setCertificateAuthority( String list )
	{
		certificateAuthority = list;
	}



	/**
	 * Gets the SRB proxyUserName.
	 *
	 * @return proxyUserName
	 */
	protected String getProxyUserName()
	{
		return proxyUserName;
	}

	/**
	 * Gets the SRB proxyDomainName.
	 *
	 * @return proxyDomainName
	 */
	protected String getProxyDomainName()
	{
		return proxyDomainName;
	}

	/**
	 * Gets the domain name used by the client.
	 * Only different from the proxyDomainName for ticketed users.
	 */
	public String getDomainName( )
	{
		return domainName;
	}

	/**
	 * Gets the default storage resource.
	 *
	 * @return defaultStorageResource
	 */
	public String getDefaultStorageResource( )
	{
		return defaultStorageResource;
	}

	/**
	 * Gets the SRB options.
	 *
	 * @return options
	 */
	public int getOptions()
	{
		return options;
	}

	/**
	 * Gets the SRB version.
	 *
	 * @return version
	 */
	public static String getVersion( )
	{
		return version;
	}


//SRB 3.0
	/**
	 * @return the proxy mcat zone.
	 */
	public String getProxyMcatZone( )
	{
		return proxyMcatZone;
	}


	/**
	 * @return the client mcat zone.
	 */
	public String getMcatZone( )
	{
		return clientMcatZone;
	}


	/**
	 * @return the exec file.
	 */
	public String getExecFile( )
	{
		return execFile;
	}


//for GSI
	/**
	 * Gets the locations of the GSI Certificate Authority (CA). By default,
	 * the CA definition comes from the user's cog.properties file.
	 */
	public String getCertificateAuthority( )
	{
		return certificateAuthority;
	}




//----------------------------------------------------------------------
// UserInfo Methods
//----------------------------------------------------------------------
	/**
	 * Tests this local file system account object for equality with the
	 * given object.
	 * Returns <code>true</code> if and only if the argument is not
	 * <code>null</code> and both are account objects for the same
	 * filesystem.
	 *
	 * @param   obj   The object to be compared with this abstract pathname
	 *
	 * @return  <code>true</code> if and only if the objects are the same;
	 *          <code>false</code> otherwise
	 */
	public boolean equals( Object obj )
	{
		try {
  		if (obj == null)
  			return false;

			SRBAccount temp = (SRBAccount) obj;

			if (!getHost().equals(temp.getHost()))
				return false;
			if (getPort() != temp.getPort())
				return false;
			if (!getUserName().equals(temp.getUserName()))
				return false;
			if (!getPassword().equals(temp.getPassword()))
				return false;

			if (!getProxyUserName().equals(temp.getProxyUserName()))
				return false;
			if (!getProxyDomainName().equals(temp.getProxyDomainName()))
				return false;
			if (!getDomainName().equals(temp.getDomainName()))
				return false;

			if (!getVersion().equals( SRB_VERSION_2 ) &&
				!getVersion().equals( SRB_VERSION_1_1_8 ))
			{
			  if (!getProxyMcatZone().equals(temp.getProxyMcatZone()))
			  if (!getMcatZone().equals(temp.getMcatZone()))
			  	return false;
			}

			//else //everything is equal
				return true;
		} catch (ClassCastException e) {
			return false;
		}
	}




//----------------------------------------------------------------------
// UserInfo Methods
//----------------------------------------------------------------------
	/**
	 * Reads the Mdas enviroment files, then set the user info.
	 *
	 * @param userInfoDirectory	The path to the user info files
	 */
	public void setUserInfo( LocalFile userInfoDirectory )
		throws FileNotFoundException, IOException
	{
		setMdasUserInfo( new LocalFile( userInfoDirectory, ".MdasEnv" ));
		readMdasAuth( new LocalFile( userInfoDirectory, ".MdasAuth" ));
	}


	/**
	 * Reads the Mdas enviroment files, then sets the user info variables.
	 *
	 * @param mdasEnvFile	The mdas file which stores the user info
	 */
	private void setMdasUserInfo( LocalFile mdasEnvFile )
		throws FileNotFoundException, IOException
	{
		int index = 0;
		FileReader mdasEnvReader = null;

		//The values are inside 'single quotes',
		//so java.util.Properties gets them wrong.
		try {
			mdasEnvReader = new FileReader( mdasEnvFile.getFile() );
		} catch ( FileNotFoundException e ) {
			if (!version.equals(SRB_VERSION_2) && !version.equals(SRB_VERSION_1_1_8))
			{
				//Ticketed users don't have an SRB account. This default account
				//was created after SRB3.0.1 to allow anyone access to the
				//information stored in public@npaci.
				setProxyUserName( "ticketuser" );
				//clientUserName
				setUserName( "ticketuser" );
				setHomeDirectory( "/home/public.npaci" );

				setPassword( "" );
				return;
			}
			else {
				throw e;
			}
		}

		char mdasEnvContents[] = new char[(int) mdasEnvFile.length()];
		mdasEnvReader.read(mdasEnvContents);
		String mdasEnv = new String(mdasEnvContents);

		//Remove comments
		while (index >=0) {
		index = mdasEnv.indexOf("#", index);
			if (index >= 0) {
				mdasEnv = mdasEnv.substring( 0, index ) +
					mdasEnv.substring( mdasEnv.indexOf('\n', index+1), mdasEnv.length() );
			}
		}


		//Default host
		index = mdasEnv.indexOf("srbHost");
		if (index < 0) {
			throw new NullPointerException( "No host name found in Mdas file.");
		}
		index = mdasEnv.indexOf('\'', index)+1;
		setHost( mdasEnv.substring( index, mdasEnv.indexOf('\'', index)) );

		//Default port
		index = mdasEnv.indexOf("srbPort");
		if (index < 0) {
//TODO default port is set in the generated file srb.h
			setPort( 5544 );
		}
		else {
			index = mdasEnv.indexOf('\'', index)+1;
			setPort( Integer.parseInt(
				mdasEnv.substring( index, mdasEnv.indexOf('\'', index))) );
		}


		//Default proxyUserName
		index = mdasEnv.indexOf("srbUser");
		if (index < 0) {
			throw new NullPointerException( "No user name found in Mdas file.");
		}
		index = mdasEnv.indexOf('\'', index)+1;
		setProxyUserName( mdasEnv.substring( index, mdasEnv.indexOf('\'', index)) );

		//clientUserName
		setUserName( proxyUserName );



		//proxyDomainName
		index = mdasEnv.indexOf("mdasDomainHome");
		if (index < 0) {
			throw new NullPointerException( "No home domain name found in Mdas file.");
		}
		index = mdasEnv.indexOf('\'', index)+1;
		setProxyDomainName( mdasEnv.substring(
			index,mdasEnv.indexOf('\'', index)) );

		//domainName
		setDomainName( proxyDomainName );


		//defaultStorageResource
		index = mdasEnv.indexOf("defaultResource");
		if (index < 0) {
			throw new NullPointerException( "No default resource found in Mdas file.");
		}
		index = mdasEnv.indexOf('\'', index)+1;
		setDefaultStorageResource( mdasEnv.substring(
			index,mdasEnv.indexOf('\'', index)) );


		//options  (password type)
		index = mdasEnv.indexOf("AUTH_SCHEME");
		if (index < 0) {
			//setOptions( ENCRYPT1 );
		}
		else {
			String option = null;
			index = mdasEnv.indexOf('\'', index)+1;
			option = mdasEnv.substring(index,mdasEnv.indexOf('\'', index));
			if (option.equals( "PASSWD_AUTH" )) {
				setOptions( PASSWD_AUTH );
			}
			else if (option.equals( "GSI_AUTH" )) {
				setOptions( GSI_AUTH );
			}
			else if (option.equals( "ENCRYPT1" )) {
				setOptions( ENCRYPT1 );
			}
			else if (option.equals( "SEA_AUTH" )) {
				throw new IllegalArgumentException( "SEA_AUTH not supported" );
			}
			else if (option.equals( "SEA_ENCRYPT" )) {
				throw new IllegalArgumentException( "SEA_ENCRYPT not supported" );
			}
			else if (option.equals( "GSI_SECURE_COMM" )) {
				throw new IllegalArgumentException( "GSI_SECURE_COMM not supported" );
			}
		}


//SRB 3.0
		//proxyMcatZone
		index = mdasEnv.indexOf("mcatZone");
		if (index >= 0) {
			index = mdasEnv.indexOf('\'', index)+1;
			setProxyMcatZone( mdasEnv.substring(
				index,mdasEnv.indexOf('\'', index)) );
		}

		//clientMcatZone
		setMcatZone( proxyMcatZone );

		//execFile
		index = mdasEnv.indexOf("execFile");
		if (index >= 0) {
			index = mdasEnv.indexOf('\'', index)+1;
			setDefaultStorageResource( mdasEnv.substring(
				index,mdasEnv.indexOf('\'', index)) );
		}



//had to move down after 3.0
		//homeDirectory
		index = mdasEnv.indexOf("mdasCollectionHome");
		if (index < 0) {
			if (proxyMcatZone == null) {
			// = /zone/home/user.domain
				setHomeDirectory( SRBFile.PATH_SEPARATOR + proxyMcatZone +
					SRBFile.PATH_SEPARATOR + "home" +
					SRBFile.PATH_SEPARATOR + userName +"."+ domainName );
			}
			else {
			// = /home/user.domain
				setHomeDirectory( SRBFile.PATH_SEPARATOR + "home" +
					SRBFile.PATH_SEPARATOR + userName +"."+ domainName );
			}
		}
		else {
			index = mdasEnv.indexOf('\'', index)+1;
			setHomeDirectory( mdasEnv.substring(
				index, mdasEnv.indexOf('\'', index)) );
		}


	}


	/**
	 * Retrieve the Mdas authorization user password
	 *
	 * @param mdasAuthFile	The file which contains the Mdas authorization
	 */
	private void readMdasAuth( LocalFile mdasAuthFile )
		throws FileNotFoundException, IOException
	{
		FileReader mdasAuthReader = new FileReader(mdasAuthFile.getFile());
		char mdasAuthContents[] = new char[(int) mdasAuthFile.length()];

		mdasAuthReader.read(mdasAuthContents);

		String mdasAuth = new String(mdasAuthContents);

		StringTokenizer authTokens = new StringTokenizer(
			mdasAuth, System.getProperty( "line.separator" )+"\n");
		String token;
		while (authTokens.hasMoreTokens()) {
			token = authTokens.nextToken();

			if ( token.startsWith("#") ) {
				//ignore comments
			}
			else {
				int index = token.indexOf(System.getProperty( "line.separator" )) +
					token.indexOf("\n") + 1;

				if (index >= 0)
					mdasAuth = token.substring( 0, index );
				else
					mdasAuth = token;
			}
		}

		setPassword( mdasAuth );
	}
}
