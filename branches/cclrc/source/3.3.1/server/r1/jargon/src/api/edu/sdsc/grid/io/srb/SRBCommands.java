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
//	SRBCommands.java	-  edu.sdsc.grid.io.srb.SRBCommands
//
//  CLASS HIERARCHY
//	java.lang.Object
//     |
//     +-.SRBCommands
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.srb;

import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.local.*;

import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.ConnectException;
import java.net.ProtocolException;
import java.net.SocketException;
import java.nio.channels.ClosedChannelException;
import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InterruptedIOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.TreeMap;



/**
 * Instances of this class support socket I/O to a Srb server.
 *<P>
 * Handles socket level protocol for interacting with the SRB.
 *<P>
 * See also: <a href="doc-files/SRBProtocol.htm">SRB protocol</a>
 *
 * <P>
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 * @since   JARGON1.0
 */
class SRBCommands
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------
	/**
	 * A positive debug value turns on debugging. Higher values turn on more, maybe.
	 */
	static int DEBUG = 0;
	static {
		//Set the debug, default zero.
		try {
			DEBUG = new Integer( System.getProperty( "jargon.debug", "0")).intValue();
		} catch (java.lang.NumberFormatException e) {
			//in case they don't use an integer
			DEBUG = 0;
		}
	}

	/**
	 * If the status signal from the server equals zero, everything is ok.
	 */
	static final char STATUS_OK = 0;

	/**
	 * 16 bit char
	 */
	public static final int CHAR_LENGTH = 2;

	/**
	 * 16 bit short
	 */
	public static final int SHORT_LENGTH = 2;

	/**
	 * 32 bit integer
	 */
	public static final int INT_LENGTH = 4;

	/**
	 * 64 bit long
	 */
	public static final int LONG_LENGTH = 8;

	/**
	 * Maximum byte length of error messages returned from the SRB.
	 */
	private static final int ERROR_MSG_LENGTH = 4096;

	/**
	 * srbMaster initialization string.
	 */
	private static final String STARTUP_HEADER = "START SRB\0";

	/**
	 * Maximum message length sent to the srb server.
	 */
	static final int OUTPUT_BUFFER_LENGTH = SRBFile.BUFFER_MAX_SIZE;


	/**
	 * Length of encrypt1 authentication key.
	 */
	private static final int ENCRYPT1_MESSAGE_SIZE = 64;


	/**
	 * Used for proxy commands
	 */
	private static final int OPR_COMMAND = 1;


	/*
	 * From the srb C client, stubDef.h - Definitions of function call stub
	 */
	private static final int F_E_CREATE = 1000;
	private static final int F_E_OPEN = 1001;
	private static final int F_E_CLOSE = 1002;
	private static final int F_E_UNLINK = 1003;
	private static final int F_E_READ = 1004;
	private static final int F_E_WRITE = 1005;
	private static final int F_E_SEEK = 1006;
	private static final int F_E_SYNC = 1007;
	private static final int F_E_STAT = 1008;
	private static final int F_E_MKDIR = 1009;
	private static final int F_E_CHMOD = 1010;
	private static final int F_E_RMDIR = 1011;
	private static final int F_E_OPENDIR = 1012;
	private static final int F_E_READDIR = 1013;
	private static final int F_E_CLOSEDIR = 1014;
	private static final int F_E_SETSTORATTRI = 1015;
	private static final int F_E_MIGRATE = 1016;
	private static final int F_E_STAGE = 1017;
	private static final int F_E_PURGE = 1018;
	private static final int F_E_FSTAT = 1019;
	private static final int F_E_CHKSUM = 1020;
	private static final int F_E_GET_FS_FREESPACE = 1021;
	private static final int F_E_FSTAGE = 1022;
	private static final int F_E_LOCK_RANGE = 1023;
	private static final int F_E_CHOWN = 1024;

	private static final int F_E_VAULT_INFO = 1050;
	private static final int F_GET_SVR_VER = 1051;

	/* SRB_MDAS type calls */

	private static final int F_SRBO_CREATE = 2000;
	private static final int F_SRBO_OPEN = 2001;
	private static final int F_SRBO_CLOSE = 2002;
	private static final int F_SRBO_UNLINK = 2003;
	private static final int F_SRBO_READ = 2004;
	private static final int F_SRBO_WRITE = 2005;
	private static final int F_SRBO_SEEK = 2006;
	private static final int F_SRBO_SYNC = 2007;
	private static final int F_SRBO_STAT = 2008;
	private static final int F_SRBO_REPLICATE = 2009;
	private static final int F_SRBO_MOVE = 2010;
	private static final int F_SRBO_PROXY_OPR = 2011;
	private static final int F_SRBO_GET_DENTS = 2012;
	private static final int F_SRBO_GET_DENTS64 = 2013;
	private static final int F_SRBC_SEEK = 2014;
	private static final int F_SRBO_CHKSUM = 2015;
	private static final int F_SRBO_LOCK_RANGE = 2016;
	private static final int F_SRBO_PROC = 2017;

	private static final int F_SRBO_GET_LOID_INFO = 2100;
	static final int F_SRBO_GET_DATADIR_INFO = 2101;
	private static final int F_SRBO_REGISTER_FILE = 2102;
	private static final int F_SRBO_MODIFY_FILE   = 2103;
	private static final int F_CHK_MDAS_AUTH = 2104;
	private static final int F_CREATE_DIRECTORY = 2105;
	private static final int F_LIST_DIRECTORY = 2106;
	private static final int F_MODIFY_DIRECTORY = 2107;
	private static final int F_CHK_MDAS_SYS_AUTH = 2108;
	private static final int F_REGISTER_USER_GROUP = 2109;
	private static final int F_REGISTER_USER = 2110;
	private static final int F_MODIFY_USER = 2111;
	private static final int F_SET_AUDIT_TRAIL = 2112;
	private static final int F_SRBO_AUDIT = 2113;
	private static final int F_REGISTER_REPLICA = 2114;
	private static final int F_GET_PRIV_USERS = 2115;
	private static final int F_GET_MORE_ROWS = 2116;
	private static final int F_ISSUE_TICKET = 2117;
	private static final int F_REMOVE_TICKET = 2118;
	private static final int F_UNREGISTER_FILE = 2119;
	private static final int F_CONTAINER_CREATE = 2120;
	private static final int F_REGISTER_CONTAINER = 2121;
	private static final int F_REGISTER_IN_CONTAINER = 2122;
	private static final int F_GET_CONTAINER_INFO = 2123;
	private static final int F_GET_RESC_ON_CHOICE = 2124;
	private static final int F_REMOVE_CONTAINER = 2125;
	private static final int F_SYNC_CONTAINER = 2126;
	private static final int F_REPLICATION_CONTAINER = 2127;
	private static final int F_CHK_ENCRYPT1_AUTH = 2128;
	private static final int F_SRBO_LOCK = 2129;
	private static final int F_CONTAINER_OPEN = 2130;
	private static final int F_CONTAINER_CLOSE = 2131;
	private static final int F_FILE_COPY = 2132;
	private static final int F_SRBO_COPY = 2133;
	private static final int F_FILE_PUT = 2134;
	private static final int F_SRBO_PUT = 2135;
	private static final int F_FILE_GET = 2136;
	private static final int F_SRBO_GET = 2137;
	private static final int F_BULK_REGISTER = 2138;
	private static final int F_SRBO_SYNC_DATA = 2139;
	private static final int F_MOD_RESC_INFO = 2140;
	private static final int F_SRBO_REG_FILE_INT = 2141;
	private static final int F_REGISTER_LOCATION = 2142;
	private static final int F_INGEST_TOKEN = 2143;
	private static final int F_REGISTER_RESOURCE = 2144;
	private static final int F_REGISTER_LOGICAL_RESOURCE = 2145;
	private static final int F_REGISTER_REPLICATE_RESOURCE_INFO = 2146;
	private static final int F_DELETE_VALUE = 2147;
	private static final int F_SETUP_SESSION = 2148;
	private static final int F_SETUP_SESSION_PUBLIC_KEY = 2149;
	private static final int F_BULK_LOAD = 2150;
	private static final int F_GET_MCAT_NAME = 2151;
	private static final int F_MODIFY_ZONE = 2152;
	private static final int F_BULK_QUERY_ANSWER = 2153;
	private static final int F_BULK_MCAT_INGEST = 2154;
	static final int F_GEN_QUERY = 2155;
	private static final int F_BULK_UNLOAD = 2156;
	private static final int F_GEN_GET_MORE_ROWS = 2157;
	private static final int F_BULK_UNLOAD_C = 2158;
	private static final int F_BACKUP_DATA = 2159;
	private static final int F_REMOVE_TICKET_WITH_ZONE = 2160;
	private static final int F_MODIFY_USER_NP = 2161;
	private static final int F_MODIFY_RESOURCE = 2162;
	private static final int F_SRBO_GET_C = 2163;
	private static final int F_DATA_GET_C = 2164;
	private static final int F_SRBO_PUT_C = 2165;
	private static final int F_DATA_PUT_C = 2166;
	private static final int F_SRBO_FSTAGE = 2167;
	private static final int F_MODIFY_EXT_META_DATA = 2168;
	private static final int F_GET_USER_BY_DN = 2169;


	private static final int F_GET_HOST_CONFIG = 2200;

	private static final int F_DB_LOBJ_CREATE = 3000;
	private static final int F_DB_LOBJ_OPEN = 3001;
	private static final int F_DB_LOBJ_CLOSE = 3002;
	private static final int F_DB_LOBJ_READ = 3004;
	private static final int F_DB_LOBJ_WRITE = 3005;
	private static final int F_DB_LOBJ_SEEK = 3006;
	private static final int F_DB_LOBJ_UNLINK = 3007;


	private static final int F_DB_TABLE_CREATE = 4000;
	private static final int F_DB_TABLE_OPEN = 4001;
	private static final int F_DB_TABLE_CLOSE = 4002;
	private static final int F_DB_TABLE_READ = 4004;
	private static final int F_DB_TABLE_WRITE = 4005;
	private static final int F_DB_TABLE_SEEK = 4006;
	private static final int F_DB_TABLE_UNLINK = 4007;

	/* TapeLib functions */
	private static final int F_TAPELIB_MNT_CART = 5000;
	private static final int F_TAPELIB_DISMNT_CART = 5001;
	private static final int F_TAPELIB_GET_CART_PRI = 5002;

	/* compound obj functions */
	private static final int F_CMP_DUMP_FILE_LIST = 5100;
	private static final int F_CMP_STAGE_COMP_OBJ = 5101;
	private static final int F_CMP_REG_INT_COMP_OBJ = 5102;
	private static final int F_CMP_RM_INT_COMP_OBJ = 5103;
	private static final int F_CMP_RM_COMP_OBJ = 5104;
	private static final int F_CMP_MOD_INT_COMP_OBJ = 5105;

	private static final int F_MDRIVER_CREATE = 6000;
	private static final int F_MDRIVER_OPEN = 6001;
	private static final int F_MDRIVER_CLOSE = 6002;
	private static final int F_MDRIVER_READ = 6004;
	private static final int F_MDRIVER_WRITE = 6005;
	private static final int F_MDRIVER_SEEK = 6006;
	private static final int F_MDRIVER_UNLINK = 6007;
	private static final int F_MDRIVER_SYNC = 6008;
	private static final int F_MDRIVER_PROC = 6009;

	/* Add new SRB function call definitions here */

	//status for srbObjGetClientInitiated and srbObjPutClientInitiated
	private static final int NEW_PORTLIST = 888888;

	private static final int F_DUMMY = 999999;


	/**
	 * Parallel/bulk copies of collections for single server port
	 * implementations will return this if multiple ports are attempted.
	 */
	static final int MSG_USE_SINGLE_PORT = -99999999;

	/**
	 * Set true if parallel/bulk copies of collections should use
	 * the single server port implementation.
	 */
	static boolean singleServerPort = false;


//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * The SRB socket connection through which
	 * all socket activity is directed.
	 */
	private Socket srbConnection;

	/**
	 * The input stream of the Srb socket connection.
	 */
	private InputStream in = null;

	/**
	 * The output stream of the Srb socket connection.
	 */
	private OutputStream out = null;


	/**
	 * Buffer output to the socket.
	 */
	private byte outputBuffer[] = new byte[OUTPUT_BUFFER_LENGTH];


	/**
	 * Holds the offset into the outputBuffer array for adding new data.
	 */
	private int outputOffset = 0;


	/**
	 * Hold the SRB password. Needed in cases of after a function call the
	 * server re-requests authentication.
	 */
	private String password;
	private int passwordOption;
	String certAuth;

	/**
	 * More account information that has to be stored here.
	 */
//TODO well, maybe has to be stored here, for now.
	private String zone;


//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Opens a socket connection to read from and write to.
	 *<P>
	 * @param userInfoDirectory	the directory to find the user info
	 * @throws	IOException	if the connection to the SRB fails.
	 */
	SRBCommands( )
		throws IOException
	{
		//move connect() call to SRBFileSystem constructor
	}





	/**
	 * Finalizes the object by explicitly letting go of each of
	 * its internally held values.
	 *<P>
	 * @throws IOException If can't close socket.
	 */
	protected void finalize( )
		throws IOException
	{
		if (outputBuffer != null)
			outputBuffer = null;
		close();
		if (out != null) {
			out = null;
		}
		if (in != null) {
			in = null;
		}
		if (srbConnection != null) {
			srbConnection = null;
		}
	}


//----------------------------------------------------------------------
// Connection methods
//----------------------------------------------------------------------
	/**
	 * Handles connection protocol.
	 * Standard implementation, first, sends the initial handshake with the
	 * srbMaster, then receives back a new Port. Connects to the Srb Server
	 * at the new Port.
	 *<br><br>
	 * If the server is using the single server implementation, added since
	 * SRB version 3.3, to not open new ports.
	 * <P>
	 * @throws	IOException	if the host cannot be opened or	created.
	 */
	int connect( SRBAccount account, byte userInfoBuffer[] )
		throws IOException
	{
		int status = -1;
		byte temp[];
		String host = account.getHost();
		int port = account.getPort();
		float version = account.getVersionNumber();
		zone = account.getMcatZone();

		if (DEBUG > 1)
			System.out.println("Connecting to master server, "+host+":"+port);
		//
		// Initial connection to srbMaster
		//
		openSocket( host, port );

		//
		// Send "START SRB" to the srbMaster
		//
		send(STARTUP_HEADER.getBytes());
		flush( );


		//
		// Check for singleServerPort implementation
		//
		if ( version >= 3.3 ) {
			temp = read(4);
			port = Host.castToInt( temp );
			if ( port != 0 ) {
				// Read new port number
				temp = read(4);
				port = Host.castToInt( temp );
			}
		}
		else {
			// Read new port number
			temp = read(4);
			port = Host.castToInt( temp );
		}

		if (port < 0) {
			close();
			throw new ProtocolException(
				"connect() -- couldn't read port number: "+ port);
		}
		else if (port > 0) {
			//
			// Close connection to srbMaster
			//
			close();

			if (DEBUG > 1) System.out.println("Connecting to srbServer:"+port);
			//
			// Connect to srbServer at the new port
			//
			openSocket( host, port );
		}
		else {
			if (DEBUG > 1) System.out.println("Connecting to srbServer through "+
				"srbMaster server.");
		}

		return sendUserInfo( account, userInfoBuffer, port );
	}

	/**
	 * Handles connection protocol.
	 * First, sends initial handshake with the srbMaster,
	 * then receives back a new Port. Connects to the Srb Server
	 * at the new Port.
	 * <P>
	 * @throws	IOException	if the host cannot be opened or	created.
	 */
	int sendUserInfo( SRBAccount account, byte userInfoBuffer[], int port )
		throws IOException
	{
		int status = -1;
		byte temp[];

		//
		// Send user info:
		// length, msgType, proxyUserName, proxyDomainName,
		// clientUserName, clientDomainName, options, version
		//
		send( userInfoBuffer );
		flush( );


		//
		// Get server status.
		//
		temp = read(4);
		status = Host.castToInt( temp );
		if (status < 0) {
			close();

			return status;
		}
		//clear the output buffer
		outputBuffer = new byte[OUTPUT_BUFFER_LENGTH];

		if (DEBUG > 1) System.out.println("Sending password...");
		//
		// Send the authorization.
		// SRB supports 4 types of authentication scheme,
		// depending on the type of authentication,
		// different hand shake will be carried out
		//
		password = account.getPassword();
		passwordOption = account.getOptions();
		switch ( passwordOption ) {
			case SRBAccount.PASSWD_AUTH:
				send( password );
				break;
/*			case SRBAccount.SEA_AUTH:
				send( password );
				break;
			case SRBAccount.SEA_ENCRYPT:
				send( password );
				break;
*/
			case SRBAccount.GSI_AUTH:
				//GSIAuth was moved to a seperate class file, GSIAuth.
				//This is to allow compiling Jargon when not in possession of
				//the GSI libraries. It seems to be ok for me to include the
				//gsi libraries, but I'm not sure. So this makes it easy to remove
				//just those libraries. see also GSIAuth.java_fake
				certAuth = account.getCertificateAuthority();
				new GSIAuth( password, certAuth, srbConnection, out, in );
				break;
/*			case SRBAccount.GSI_SECURE_COMM:
				send( password );
				break;
*/
			case SRBAccount.GSI_DELEGATE:
				certAuth = account.getCertificateAuthority();
				new GSIAuth( password, certAuth, srbConnection, out, in );
				break;
			case SRBAccount.ENCRYPT1:
				sendEncrypt1Auth( password );
				break;
			default:
				send( password );
		}
		flush( );

		//
		// Get server status.
		//
		temp = read(4);
		status = Host.castToInt( temp );
		if (( status == -1004) || ( status == -1005) || ( status == -1006) ||
			( status == -1017)) {
			SRBException e = new SRBException(
				"connect() -- couldn't connect to SRB. ", status);
			throw new SecurityException( e.getMessage()+e.getStandardMessage() );
		}
		else if (status < 0) {
			close();
			throw new SRBException(
				"connect() -- couldn't connect to SRB. ", status);
		}

		if (DEBUG > 1) System.out.println("Successful connection.");
		return port;
	}




	/**
	 * Close the connection to the server. This method has been sycnhronized
	 * so the socket will not be blocked when the socket.close() call is made.
	 *
	 * @throws IOException	Socket error
	 */
	synchronized void close( )
		throws IOException
	{
		if (isConnected()) {
			out.write(new String("X").getBytes());
			out.close();
			in.close();
			srbConnection.close();
		}
	}






//start Encrypt1 auth
	/**
	 * Encrypt1 authorization method. Makes a connection to the SRB using
	 * the Encrypt1 authorization scheme.
	 */
	void sendEncrypt1Auth( String password ) throws IOException
	{
		int status=-1, readBytes=-1, writeBytes=-1;
		long seed = 0;
		long seed2 = 0;
		long maxValue = 64;
		double maxValueDouble = 2.6035084875658576;
		long[] hashPassword = new long[2];
		long[] hashInitialMessage = new long[2];
		byte[] encryptedMessage = new byte[ENCRYPT1_MESSAGE_SIZE];
		int initialMessage[] =
			new int[ENCRYPT1_MESSAGE_SIZE-1]; //don't include final 0

//TODO create read(int[])
//	read(initialMessage);
		for (int i=0;i < initialMessage.length;i++) {
			initialMessage[i] = in.read();
		}

		//ignore final 0
		read(1);


		if (password != null)
		{
			int[] passwordInts = new int[password.length()];
			byte[] passwordBytes = password.getBytes();
			for (int i=0;i<passwordInts.length;i++)
				passwordInts[i] = passwordBytes[i];

			vHashString(hashPassword,passwordInts);
			vHashString(hashInitialMessage,initialMessage);

			maxValue = 1073741823; //0x3FFFFFFFL
			maxValueDouble = (double) maxValue;
			seed = ( hashPassword[0] ^ hashInitialMessage[0] ) % maxValue;
			seed2 = ( hashPassword[1] ^ hashInitialMessage[1] ) % maxValue;

			for (int i=0;i < initialMessage.length;i++) {
				seed = ( seed*3 + seed2) %  maxValue;
				seed2 = ( seed + seed2+33) % maxValue;

				encryptedMessage[i] = (byte)
					(Math.floor((((double) seed)/maxValueDouble)*31)+64);
			}
		}

		send(encryptedMessage, 0, encryptedMessage.length);

		if (writeBytes != ENCRYPT1_MESSAGE_SIZE) {
			//throw new Exception( ); //TODO some exception
		}
	}

	/**
	 * Converts an array into two unsigned integers.
	 */
	static void vHashString(long[] result, int[] password)
	{
		long nr=1345345333;
		long add=7;
		long nr2=305419889;
		int currentValue;

		for (int i=0; i<password.length; i++)
		{
			currentValue = password[i];

			//Ok, I'm not really sure every part is needed.
			long UNSIGNED_INT_MAX = (Integer.MAX_VALUE+(long)1)*((long)2);
			long temp = (nr << 8);
			if (temp > (UNSIGNED_INT_MAX))
				temp = (int) temp;
			if (temp < (-UNSIGNED_INT_MAX))
				temp = (UNSIGNED_INT_MAX) + temp;
			nr = nr ^ ((((nr & 63)+add)*currentValue) + temp);
			if (nr < 0) {
				nr = UNSIGNED_INT_MAX + nr;
			}

			temp = (nr2 << 8);
			if (temp > (UNSIGNED_INT_MAX))
				temp = (int) temp;
			if (temp < 0)
				temp = (UNSIGNED_INT_MAX) + temp;
			nr2 += temp ^ nr;
			if (nr2 > (UNSIGNED_INT_MAX))
				nr2 = (int) nr2;
			if (nr2 < 0)
				nr2 = (UNSIGNED_INT_MAX) + nr2;
			add += currentValue;
		}
		result[0]=nr & (((int) 1 << 31) -1);
		result[1]=nr2 & (((int) 1 << 31) -1);
		return;
	}
//end encrypt1


	/**
	 * Returns the zone that was registered to this SRBCommands object
	 * during construction.
	 */
	String getZone()
	{
		return zone;
	}


//----------------------------------------------------------------------
// Socket Methods
//----------------------------------------------------------------------
	/**
	 * Open a connection to the server.
	 *
	 * @param host		Name of the host to connect to
	 * @param port		Port on that host
	 * @throws ConnectException	if the connection cannot be made
	 * @throws SocketException	A socket error occured
	 * @throws IOException if a IOException occurs
	 */
	private void openSocket( String host, int port )
		throws IOException
	{
		try {
			srbConnection = new Socket( host, port );
			in = srbConnection.getInputStream();
			out = srbConnection.getOutputStream();
		}
		catch (ConnectException e) {
			ConnectException connException = new ConnectException(
				"Connection cannot be made to: "+ host + " at port: "+ port);
			connException.initCause(e);
			throw connException;
		}
		catch ( SocketException e ) {
			SocketException socketException = new SocketException(
				"A socket error occured when connecting to: "+ host +
				" at port: "+ port);
			socketException.initCause(e);
			throw socketException;
		}
	}


	/**
	 * Checks if the socket is connected.
	 */
	boolean isConnected()
	{
		if (srbConnection.isConnected()) {
			if (!srbConnection.isClosed()) {
				//can be both connected and closed, but that isn't what I meant.
				return true;
			}
		}
		return false;
	}


	//
	//Output
	//
	/**
	 * Writes value.length bytes to this output stream.
	 *
	 * @param value		value to be sent
	 * @throws NullPointerException	Send buffer is empty
	 * @throws IOException	If an IOException occurs
	 */
	private void send( byte[] value )
		throws IOException
	{
		if (( value.length + outputOffset ) >= OUTPUT_BUFFER_LENGTH) {
			//in cases where OUTPUT_BUFFER_LENGTH isn't big enough
/*			if ( srbConnection.isClosed() ) { //TODO good idea?
				throw new ClosedChannelException();
			}
*/
			out.write( outputBuffer, 0, outputOffset );
			out.write( value );
			out.flush();
			outputOffset = 0;
		}
		else {
			//the message sent isn't longer than OUTPUT_BUFFER_LENGTH
			System.arraycopy( value, 0, outputBuffer, outputOffset, value.length);
			outputOffset += value.length;

			if (DEBUG > 5) System.out.print("Send: "+new String(value));
		}
	}


	/**
	 * Writes a certain length of bytes at some offset
	 * in the value array to the output stream,
	 * by converting the value to a byte array and calling
	 * send( byte[] value ).
	 *
	 * @param value		value to be sent
	 * @param offset	offset into array
	 * @param length	number of bytes to read
	 * @throws IOException	If an IOException occurs
	 */
	private void send( byte[] value, int offset, int length )
		throws IOException
	{
		byte temp[] = new byte[length];

		System.arraycopy( value, offset, temp, 0, length);

		send( temp );
	}


	/**
	 * Writes value.length bytes to this output stream.
	 *
	 * @param value		value to be sent
	 * @throws IOException	If an IOException occurs
	 */
	private void send( String value )
		throws IOException
	{
		send( value.getBytes() );
	}

	/**
	 * Writes an int to the output stream as four bytes, low byte first.
	 *
	 * @param value		value to be sent
	 * @throws IOException	If an IOException occurs
	 */
	private void send( int value )
		throws IOException
	{
		byte bytes[] = new byte[INT_LENGTH];

		Host.copyInt( value, bytes );
		Host.swap( bytes, INT_LENGTH );

		send( bytes );
	}

	/**
	 * Writes an long to the output stream as eight bytes, low byte first.
	 *
	 * @param value		value to be sent
	 * @throws IOException	If an IOException occurs
	 */
	private void send( long value )
		throws IOException
	{
		byte bytes[] = new byte[LONG_LENGTH];

		Host.copyLong( value, bytes );
		Host.swap( bytes, LONG_LENGTH );

		send( bytes );
	}

	/**
	 * Flushes all data in the output stream and sends it to the server.
	 *
	 * @throws NullPointerException	Send buffer empty
	 * @throws IOException	If an IOException occurs
	 */
	void flush( )
		throws IOException
	{
		if ( srbConnection.isClosed() ) { //TODO good idea?
			throw new ClosedChannelException();
		}

		out.write( outputBuffer, 0, outputOffset );
		out.flush();
		outputOffset = 0;
	}



	//
	//Input
	//
	/**
	 * Reads a byte from the server.
	 *
	 * @throws IOException	If an IOException occurs
	 */
	private byte read()
		throws IOException
	{
		return (byte) in.read();
	}


	/**
	 * Reads a byte array from the server.
	 *
	 * @param length	length of byte array to be read
	 * @return byte[]	bytes read from the server
	 * @throws OutOfMemoryError	Read buffer overflow
	 * @throws ClosedChannelException if the connection is closed
	 * @throws NullPointerException	Read buffer empty
	 * @throws IOException	If an IOException occurs
	 */
/*TODO	private*/ byte[] read( int length )
		throws ClosedChannelException, InterruptedIOException, IOException
	{
		if (length <= 0) {
			return null;
		}

		byte value[] = new byte[length];

		try {
			//Can only read 1448 bytes in each loop
			int maxReadSize = 1448;
			int temp = 0;

/*TODO not needed here? always writes first from the synchronized functions
			if ( srbConnection.isClosed() ) { //TODO causing errors?
				throw new ClosedChannelException();
			}
*/
			if (length > maxReadSize) {
				while (length > (temp + maxReadSize - 1)) {
					temp += in.read( value, temp, maxReadSize );
				}
				while ((length - temp - 1) > 0) {
					temp +=	in.read( value, temp, (length - temp) );
				}
			}
			else {
				while ((length - temp) > 0) {
					temp +=	in.read( value, temp, (length - temp) );
				}
			}
		}
		catch ( IOException e ) {
			IOException ioException = new IOException(
				"read() -- couldn't read complete packet");
			ioException.initCause(e);
			throw ioException;
		}

		if (DEBUG > 5) System.out.print("Read: "+new String(value));

		return value;
	}

	/**
	 * Reads a char from the server.
	 *
	 * @return char	char read from the server
	 * @throws IOException	If an IOException occurs
	 */
	private char readChar( )
		throws IOException
	{
//TODO is this correct?
		byte[] b = read(CHAR_LENGTH);
		Host.swap( b, CHAR_LENGTH );
		char value = (char) Host.castToShort( b );

		return value;
	}

	/**
	 * Reads a short from the server.
	 *
	 * @return short	short read from the server
	 * @throws IOException	If an IOException occurs
	 */
	private short readShort( )
		throws IOException
	{
		byte[] b = read(SHORT_LENGTH);
		Host.swap( b, SHORT_LENGTH );
		short value = Host.castToShort( b );

		return value;
	}

	/**
	 * Reads an int from the server.
	 *
	 * @return int	int read from the server
	 * @throws IOException	If an IOException occurs
	 */
	private int readInt( )
		throws IOException
	{
		byte[] b =  read(INT_LENGTH);
		Host.swap( b, INT_LENGTH );
		int value = Host.castToInt( b );

		return value;
	}


	/**
	 * Reads an unsigned int from the server.
	 *
	 * @return long	unsigned int read from the server
	 * @throws IOException	If an IOException occurs
	 */
/*TODO	private*/ long readUnsignedInt( )
		throws IOException
	{
		//The SRB appears to send things MSBF sometimes and other times LSBF.
		long value = Host.castToUnsignedInt( read(INT_LENGTH) );

		return value;
	}


	/**
	 * Reads a long from the server.
	 *
	 * @return long	long read from the server
	 * @throws IOException	If an IOException occurs
	 */
	private long readLong( )
		throws IOException
	{
		byte[] b = read(LONG_LENGTH);
		Host.swap( b, LONG_LENGTH );
		long value = Host.castToLong( b );

		return value;
	}


	/**
	 * Reads a Null terminated string from the server.
	 *
	 * @param length	length of the string to be read
	 * @return String	the String read from the server
	 * @throws IOException	If an IOException occurs
	 */
	private String readString()
		throws IOException
	{
		String value = "";
		byte b = read();
		while (b != 0) {
			value += (char)b;
			b = read();
		}

		return value;
	}


	/**
	 * Reads a String from the server.
	 *
	 * @param length	length of the string to be read
	 * @return String	the String read from the server
	 * @throws IOException	If an IOException occurs
	 */
	private String readString( int length )
		throws IOException
	{
		String value = new String( read( length ) );

		return value;
	}

	/**
	 * Checks the status of the server.
	 * @throws IOException	If an IOException occurs
	 */
	private void status( )
		throws IOException
	{
		byte status[] = read(1);

		if (status[0] < STATUS_OK) {
			//status = 0 or sometimes char '0'
			throw new SRBException( "Unknown error received from server.");
		}
	}


//----------------------------------------------------------------------
// Server Commands Methods
//----------------------------------------------------------------------
	/**
	 * Every SRB command sent to the server starts with "F \0",
	 * then the function id, then the number of arguments in that function.
	 *
	 * @throws IOException	If an IOException occurs
	 */
	void startSRBCommand( int functionId, int nargs )
		throws IOException
	{
		String doFunction = "F \0"; //Always starts a command

		send(doFunction.getBytes());

		send(functionId);
		send(nargs);
	}

	/**
	 * Sends the length of an argument, then the argument itself.
	 * After a SRB command is started all the command arguments must be sent
	 * in this format.
	 *
	 * @param arg	The argument sent to the server.
	 * @throws IOException	If an IOException occurs
	 */
	void sendArg( int arg )
		throws IOException
	{
		send( INT_LENGTH );
		send( arg );
	}

	/**
	 * Sends the length of an argument, then the argument itself.
	 * After a SRB command is started all the command arguments must be sent
	 * in this format.
	 *
	 * @param arg	The argument sent to the server.
	 * @throws IOException	If an IOException occurs
	 */
	void sendArg( long arg )
		throws IOException
	{
		send( LONG_LENGTH );
		send( arg );
	}

	/**
	 * Sends the length of an argument, then the argument itself.
	 * After a SRB command is started all the command arguments must be sent
	 * in this format.
	 *
	 * @param arg	The argument sent to the server.
	 * @throws IOException	If an IOException occurs
	 */
	void sendArg( String arg )
		throws IOException
	{
		if (arg == null) {
			//send a null 4 bytes, for the string length.
			send(new byte[4]);
		}
		else {
			send( arg.length() );
			send( arg.getBytes() );
		}
	}

	/**
	 * Sends the length of an argument, then the argument itself.
	 * After a SRB command is started all the command arguments must be sent
	 * in this format.
	 *
	 * @param arg	The argument sent to the server.
	 * @throws IOException	If an IOException occurs
	 */
	void sendArg( byte arg[] )
		throws IOException
	{
		if (arg == null) {
			//send a null 4 bytes, for the byte[] length.
			send(new byte[4]);
		}
		else {
			send( arg.length );
			send( arg );
		}
	}

	/**
	 * Sends the length of an argument, then the argument itself.
	 * After a SRB command is started all the command arguments must be sent
	 * in this format.
	 *<P>
	 * This method allows only part of a byte array to be sent,
	 * from 0 to length.
	 *<P>
	 * @param arg	The argument sent to the server.
	 * @param length	Number of bytes from arg to be sent.
	 * @throws IOException	If an IOException occurs
	 */
	void sendArg( byte arg[], int length )
		throws IOException
	{
		if (arg == null) {
			//send a null 4 bytes, for the byte[] length.
			send(new byte[4]);
		}
		else {
			send( length );
			send( arg, 0, length );
		}
	}

	/**
	 * Sends the length of an argument, then the argument itself.
	 * After a SRB command is started all the command arguments must be sent
	 * in this format.
	 *<P>
	 * This method allows only part of a byte array to be sent,
	 * from offset to length.
	 *<P>
	 * @param arg	The argument sent to the server.
	 * @param offset	Offset into arg, signifying start of bytes to be sent.
	 * @param length	Number of bytes from arg to be sent.
	 * @throws IOException	If an IOException occurs
	 */
	void sendArg( byte arg[], int offset, int length )
		throws IOException
	{
		if (arg == null) {
			//send a null 4 bytes, for the byte[] length.
			send(new byte[4]);
		}
		else {
			send( length );
			send( arg, offset, length );
		}
	}

	/**
	 * Sends the length of an argument, then the argument itself.
	 * After a SRB command is started all the command arguments must be sent
	 * in this format.
	 *
	 * @param arg	The argument sent to the server.
	 * @throws IOException	If an IOException occurs
	 */
	void sendArg( int arg[] )
		throws IOException
	{
		send( arg.length * INT_LENGTH );
		for(int i=0;i<arg.length;i++){
			send( arg[i] );
		}
	}

	/**
	 * Sends the length of an argument, then the argument itself.
	 * After a SRB command is started all the command arguments must be sent
	 * in this format.
	 *<P>
	 * The array must have uniform dimensions.
	 * (The string lengths must be equal.)
	 *<P>
	 * @param arg	The argument sent to the server.
	 * @throws IOException	If an IOException occurs
	 */
	void sendArg( char arg[][] )
		throws IOException
	{
		int argLength = arg.length * arg[0].length;
		byte byteArg[] = new byte[argLength];

		int k=0;
		for(int i=0;i<arg.length;i++){
			for(int j=0;j<arg[i].length;j++){
				byteArg[k]=(byte)arg[i][j];
				k++;
			}
		}

		send( argLength );
		send( byteArg );
	}


	/**
	 * Sends the length of an argument, then the argument itself.
	 * After a SRB command is started all the command arguments must be sent
	 * in this format.
	 *<P>
	 * This method recreates the structure that is returned by a query.
	 * See also, returnSRBMetaDataRecordList() and
	 * mdasC_sql_result_struct *myresult from the C client.
	 *
	 * @param rl The SRBMetaDataRecordList[] argument sent to the server.
	 * @throws IOException	If an IOException occurs
	 */
	void sendArg( SRBMetaDataRecordList[] rl )
		throws IOException
	{
		byte[] nullByte = { 0 }; //seriously, I have to do it this way?
		String nll = new String( nullByte );

		int argLength = 0;


		//TODO must be true: rl[0].getFieldCount() == rl[n].getFieldCount();
		//and all field types must also be equal.
	/*
		for (int i=0;i<rl.length;i++) {
			if (rl[0].getFieldCount() != rl[i].getFieldCount()) {
				throw new IllegalArgumentException(
					"MetaDataRecordList arrays with varying field types cannot be sent." );
			}
		}
	*/
		//number of metadata attributes
		int result_count = rl[0].getFieldCount();
		byte result_count_bytes[] = new byte[INT_LENGTH];
		Host.copyInt( result_count, result_count_bytes );

		//number of datasets described
		int row_count = rl.length;
		byte row_count_bytes[] = new byte[INT_LENGTH];
		Host.copyInt( row_count, row_count_bytes );

		//the byte string contents of rl
		String values = "";
		int continuation_index = 0;//rl[0].getContinuationIndex();

		for (int i=0;i<rl[0].getFieldCount();i++) {
			values += SRBMetaDataSet.getSRBTableName( rl[0].getFieldName(i) ) + nll +
				SRBMetaDataSet.getSRBAttributeName( rl[0].getFieldName(i) ) + nll;
			for (int j=0;j<rl.length;j++) {
				values += rl[j].getStringValue(i) + nll;
			}
		}

		//The c code myresult struct this method replicated has a fixed length
		//have to fill in the rest of it with 0001's
		//tab_name=0100,att_name=0100,values=0100 * (100-rows used)
		byte[] extraRows= new byte[((100-rl[0].getFieldCount())*2)*3];
		extraRows[0] = 0x01;
		for (int i=1;i<extraRows.length-1;i+=2) {
			extraRows[i] = 0x00;
			extraRows[i+1] = 0x01;
		}

		//send argLength
		//int 0, result_count, row_count, values, extra rows, continuation_index
		send( INT_LENGTH + INT_LENGTH + INT_LENGTH + values.length() +
			extraRows.length + INT_LENGTH);

		//I don't know what
		send( 0 );

		send( result_count_bytes );
		send( row_count_bytes );
		send( values );
		send( extraRows );
		send( continuation_index );

		if (DEBUG > 2) {
			System.out.print("sendArg(rl): "+new String(result_count_bytes));
			System.out.println(new String(row_count_bytes));
			System.out.println(values);
			System.out.println(new String(extraRows));
			System.out.println(continuation_index);
		}
	}





	/**
	 * Parses the value returned from the server after a SRB command
	 * has been sent. Insures the function worked properly. If the first
	 * two bytes returned = "VG" then the command worked.
	 * <P>
	 * @return true on success
	 * @throws IOException	If an IOException occurs
	 */
	void commandStatus( )
		throws IOException
	{
		byte resultBuffer[] = read(1);
		String exception = "\nThe SRB server returned an error: ";

		if ((char) resultBuffer[0] == 'A') {

			//server is requesting re-authentication
			//sent the integer password option
			byte[] asdf = { (byte)passwordOption, 0, 0, 0 };
			out.write(asdf);

			switch ( passwordOption ) {
				case SRBAccount.PASSWD_AUTH:
					send( password );
					break;
/*			case SRBAccount.SEA_AUTH:
					send( password );
					break;
				case SRBAccount.SEA_ENCRYPT:
					send( password );
					break;
*/
				case SRBAccount.GSI_AUTH:
					//GSIAuth was moved to a seperate class file, GSIAuth.
					//This is to allow compiling Jargon when not in possession of
					//the GSI libraries. It seems to be ok for me to include the
					//gsi libraries, but I'm not sure. So this makes it easy to remove
					//just those libraries. see also GSIAuth.java_fake
					new GSIAuth( password, certAuth, srbConnection, out, in );
					break;
/*			case SRBAccount.GSI_SECURE_COMM:
					send( password );
					break;
*/
				case SRBAccount.GSI_DELEGATE:
					new GSIAuth( password, certAuth, srbConnection, out, in );
					break;
				case SRBAccount.ENCRYPT1:
					sendEncrypt1Auth( password );
					break;
				default:
					send( password );
			}

			//start over
			commandStatus( );
			return;
		}
		else if ((char) resultBuffer[0] != 'V') {
			int exceptionType = -1;

			int bytesRead = 0;

			//just a guess, hopefully 11 errors won't occur
			//I've never seen more than 4, but certain debugging codes might...
			int maxErrorMsgs = 10;
			resultBuffer = new byte[ERROR_MSG_LENGTH*maxErrorMsgs];

			boolean moreErrorMsg = true;
			while (moreErrorMsg) {
				bytesRead = in.read(resultBuffer, 0, ERROR_MSG_LENGTH*maxErrorMsgs);

				for (int i=0;i<bytesRead;i++) {
					if ((char) resultBuffer[i] == 'V') {
						if (resultBuffer[i+1] == 'G') {
							if (resultBuffer[i+2] == 8) {
								//if returnLong
								byte[] unsignedIntBuffer = {
									resultBuffer[i+6],
									resultBuffer[i+7],
									resultBuffer[i+8],
									resultBuffer[i+9]
								};
//								exceptionType = (int) Host.castToUnsignedInt(
//									unsignedIntBuffer );

//								if (exceptionType < 0) {
									Host.swap( unsignedIntBuffer, INT_LENGTH );
									exceptionType = Host.castToInt( unsignedIntBuffer );
//								}
								moreErrorMsg = false;
								i=bytesRead;
							}
							else {
								byte[] unsignedIntBuffer = {
									resultBuffer[i+6],
									resultBuffer[i+7],
									resultBuffer[i+8],
									resultBuffer[i+9]
								};
								exceptionType = (int) Host.castToUnsignedInt(
									unsignedIntBuffer );

								if (exceptionType > 0) {
									Host.swap( unsignedIntBuffer, INT_LENGTH );
									exceptionType = Host.castToInt( unsignedIntBuffer );
								}
								moreErrorMsg = false;
								i=bytesRead;
							}
						}
						else if (resultBuffer[i-1] == 0) {
//TODO can be a long?
							//does the same thing as above if,
							//but i-1 would cause ArrayIndexOutOfBoundsException
							byte[] unsignedIntBuffer = {
								resultBuffer[i+6],
								resultBuffer[i+7],
								resultBuffer[i+8],
								resultBuffer[i+9]
							};
							exceptionType = (int) Host.castToUnsignedInt(
								unsignedIntBuffer );

							if (exceptionType > 0) {
								Host.swap( unsignedIntBuffer, INT_LENGTH );
								exceptionType = Host.castToInt( unsignedIntBuffer );
							}
							moreErrorMsg = false;
							break;
						}
						else {
							exception += (char) resultBuffer[i];
						}
					}
					else {
						exception += (char) resultBuffer[i];
					}
				}
			}
			//windows wasn't catching the status until another read
			if (resultBuffer[bytesRead-1] != 48)
				status();

			throw new SRBException( exception, exceptionType );
		}
		else {
			resultBuffer = read(1);
		}


		switch ((char) resultBuffer[0]) {
			case 'G':		// function returned properly
				return;

			case 'E':
				throw new SRBException("SRB Server: returned an unknown error");

			case 'N':
				resultBuffer = new byte[ERROR_MSG_LENGTH];
				in.read( resultBuffer );

				if (resultBuffer.length == 1) {
					throw new SRBException(
						"Return detected, but error message cannot be read");
				}
				else {
					exception = new String( resultBuffer );

					throw new SRBException(exception);
				}

			case '0':		// no return value
				return;

			default:
				exception = "FATAL: SRB protocol error: ";
				exception += new String( resultBuffer );

				throw new SRBException(exception);
		}
	}


	/**
	 * Interpretes the return value after a SRB command was sent.
	 *
	 * @return returnValue	the integer result
	 */
	private int returnInt()
		throws IOException
	{
		byte resultBuffer[] = read(1);
		int returnValue = -1;

		if (resultBuffer[0] <= INT_LENGTH) {
			resultBuffer = read(3);

			returnValue = readInt();
			status();
		}
		else {
			// Drain it
			resultBuffer = read( resultBuffer[0] + 1 );
			String exception = "The SRB server returned an error: ";
			exception += new String( resultBuffer );

			throw new SRBException(exception);
		}

		return returnValue;
	}


	/**
	 * Interpretes the return value after a SRB command was sent.
	 *
	 * @return returnValue	the long result
	 */
	private long returnLong()
		throws IOException
	{
		byte resultBuffer[] = read(1);
		long returnValue = -1;

		if (resultBuffer[0] <= LONG_LENGTH) {
			resultBuffer = read(3);

			returnValue =  readLong();
			status();
		}
		else {
			// Drain it
			resultBuffer = read( resultBuffer[0] + 1 );
			String exception = "The SRB server returned an error: ";
			exception += new String( resultBuffer );

			throw new SRBException(exception);
		}

		return returnValue;
	}


	/**
	 * Interpretes the return value after a SRB command was sent.
	 *
	 * @return resultBuffer the byte array result
	 */
	private byte[] returnBytes()
		throws IOException
	{
		byte resultBuffer[] = read(4);
		int resultLength = 0;


		for (int i=resultBuffer.length-1;i>=0;i--) {
			if ( resultBuffer[i] < 0 ) {
				resultLength = (resultLength << 8) + 256 + resultBuffer[i];
			}
			else {
				resultLength = (resultLength << 8) + resultBuffer[i];
			}
		}

		if (resultLength >= 4) {
			//ignore sizeOf(resultLength) from result to get real data length.
			read(4);
			resultLength -= 4;

			resultBuffer = read( resultLength );
			read(1); //get final '0';
			return resultBuffer;
		}
		else {
			String exception = "\nThe SRB server returned an error: ";
			exception += (int) Host.castToUnsignedInt( read(INT_LENGTH) );
			read(1); //get final '0';

			throw new SRBException( exception );
		}
	}



	SRBMetaDataRecordList[] returnSRBMetaDataRecordList(boolean newReturn)
		throws IOException
	{
		return returnSRBMetaDataRecordList(newReturn, false);
	}


	/**
	 * Interpretes the return value after a SRB command was sent.
	 *
	 * @return	SRBMetaDataRecordList the query result.
	 *					The SRBMetaDataRecordList class replaces
	 *		 			mdasC_sql_result_struct *myresult from the C client.
	 */
//TODO internal comments
	SRBMetaDataRecordList[] returnSRBMetaDataRecordList( boolean newReturn,
		boolean usePortalHeader )
		throws IOException
	{
		if (DEBUG > 2) System.out.println("returnSRBMetaDataRecordList new");

		if (!newReturn) {
			return returnSRBMetaDataRecordList();
		}

		int i=0, j=0, k=0, temp=0;
		int bufferLength;

		if (singlePortBulkUnload) {
			read(4);
		}

		//total buffer length to read
		if (usePortalHeader)
			bufferLength =  Host.castToInt(  read(INT_LENGTH) );
		else
			bufferLength = readInt();


		int status = (int) readUnsignedInt();

		if (usePortalHeader)
			read(4); //junk

		//quick check if the server returned an error
		if (status < 0) {
			if (DEBUG > 3) System.out.println(status);
			if (status == -3005) {
				read(1); //the final '0';
				return null;
			}
			else {
				String exception = "\nSRB server status: " +status;
				read(1); //the final '0';
				throw new SRBException( exception );
			}
		}


		//number of columns in the result
		int fieldCount = (int) readUnsignedInt();
		//number of rows returned
		int recordCount = (int) readUnsignedInt();
		//continuation index is packed at the end this time.


		//total buffer length - length variables (bufferLength, fieldCount, etc.)
		byte resultBuffer[] = read(bufferLength-16);

		if (usePortalHeader) {
			read(3); //more junk
		}

		//more query data left??? negative number = invalid

		byte[] b = read(INT_LENGTH);
		int continuationIndex = (short) Host.castToInt( b );

		if (DEBUG > 2) {
			System.out.println("bufferLength "+bufferLength);
			System.out.println("status "+status);
			System.out.println("fieldCount "+fieldCount);
			System.out.println("recordCount "+recordCount);

			System.out.println("continuationIndex "+continuationIndex);
			System.out.println("resultBuffer "+resultBuffer.length);

			if (DEBUG > 4) {
				for ( i=0; i<resultBuffer.length; i++) {
					if (resultBuffer[i]>32)
						System.out.print((char)resultBuffer[i]);
					else if (resultBuffer[i]>1)
						System.out.print(resultBuffer[i]);
					else if (i>1) {
						if ((resultBuffer[i-1]!=1) && (resultBuffer[i-2]!=0) &&
							(resultBuffer[i]!=1))
						{
							System.out.print(resultBuffer[i]);
						}
					}
				}
				System.out.print("\n");
				for ( i=0; i<resultBuffer.length; i++) {
					System.out.print(" "+resultBuffer[i]);
				}
			}
		}

		//SRB table name
		String tabName[] = new String[fieldCount];
		//SRB attribute name
		String attributeName[] = new String[fieldCount];

		//For parsing the bytes returned from the server
		String returnValue[][] = new String[fieldCount][recordCount];

		for ( i=0; i < fieldCount; i++ ) {
			//first get null terminated string, tabName
			tabName[i] = "";
			while((resultBuffer[j] != 0)&&(j < resultBuffer.length - 1)){
				tabName[i] += (char) resultBuffer[j];
				j++;
			}
			//while((resultBuffer[j] == 0)&&(j < resultBuffer.length - 1)){
			if((resultBuffer[j] == 0)&&(j < resultBuffer.length - 1)){
				j++;
			}

			//then null terminated string, attributeName
			attributeName[i] = "";
			while((resultBuffer[j] != 0)&&(j < resultBuffer.length - 1)){
				attributeName[i] += (char) resultBuffer[j];
				j++;
			}
			if ((resultBuffer[j] == 0)&&(j < resultBuffer.length - 1)){
				j++;
			}

//changed from old fixed length values
			//lastly, null-term string, values
			for(k=0;k < recordCount;k++) {
				returnValue[i][k] = "";
				while((resultBuffer[j] != 0)&&(j < resultBuffer.length - 1)) {
						returnValue[i][k] += (char) resultBuffer[j];
						j++;
				}
				if ((resultBuffer[j] == 0)&&(j < resultBuffer.length - 1)){
					j++;
				}
			}
			if (resultBuffer[j] == 0) {
				//actually, should always = 0;
				j++;
			}
		}

		return parseSRBMetaDataRecordList(fieldCount, recordCount,
			continuationIndex, tabName, attributeName, returnValue );
	}



	/**
	 * Interpretes the return value after a SRB command was sent.
	 *
	 * @return	SRBMetaDataRecordList the query result.
	 *					The SRBMetaDataRecordList class replaces
	 *		 			mdasC_sql_result_struct *myresult from the C client.
	 */
//TODO old, use new query only
	SRBMetaDataRecordList[] returnSRBMetaDataRecordList()
		throws IOException
	{
		if (DEBUG > 2) System.out.println("returnSRBMetaDataRecordList old");

		int i=0, j=0, k=0, temp=0;

		//total buffer length to read
		int bufferLength = readInt();

		int status = (int) readUnsignedInt();
		//quick check if the server returned an error
		if (status < 0) {
			if (DEBUG > 3) System.out.println(status);
			if (status == -3005) {
				read(1); //the final '0';
				return null;
			}
			else {
				String exception = "\nSRB server status: " +status;
				read(1); //the final '0';
				throw new SRBException( exception );
			}
		}

		//number of columns in the result
		int fieldCount = (int) readUnsignedInt();
		//number of rows returned
		int recordCount = (int) readUnsignedInt();
		//more query data left??? negative number = invalid
		int continuationIndex = (int) readUnsignedInt();


		//total buffer length - length variables (bufferLength, fieldCount, etc.)
		byte resultBuffer[] = read(bufferLength-19);

		//status, throw away
		read(3);
		status();

		if (DEBUG > 2) {
			System.out.println("bufferLength "+bufferLength);
			System.out.println("status "+status);
			System.out.println("fieldCount "+fieldCount);
			System.out.println("recordCount "+recordCount);

			System.out.println("continuationIndex "+continuationIndex);
			System.out.println("resultBuffer "+resultBuffer.length);

			if (DEBUG > 4) {
				for ( i=0; i<resultBuffer.length; i++) {
					if (resultBuffer[i]!=0)
						System.out.print((char)resultBuffer[i]);
				}

				for ( i=0; i<resultBuffer.length; i++) {
					System.out.print(" "+resultBuffer[i]);
				}
			}
		}

		//SRB table name
		String tabName[] = new String[fieldCount];
		//SRB attribute name
		String attributeName[] = new String[fieldCount];
		//Query result values
		//For parsing the bytes returned from the server
		String returnValue[][] = new String[fieldCount][recordCount];

		for ( i=0; i < fieldCount; i++ ) {
			//first get null terminated string, tabName
			tabName[i] = "";
			temp = j;
			while((resultBuffer[j] != 0)&&(j < resultBuffer.length - 1)){
				tabName[i] += (char) resultBuffer[j];
				j++;
			}

			while((resultBuffer[j] == 0)&&(j < resultBuffer.length - 1)){
				j++;
			}

			//then null terminated string, attributeName
			attributeName[i] = "";
			temp = j;
			while((resultBuffer[j] != 0)&&(j < resultBuffer.length - 1)){
				attributeName[i] += (char) resultBuffer[j];
				j++;
			}
			if ((resultBuffer[j] == 0)&&(j < resultBuffer.length - 1)){
				j++;
			}


			//lastly, MAX_FILE_SIZE byte fixed length string, values
			for(k=0;k < recordCount;k++) {
				temp = j;
				returnValue[i][k] = "";

				while((resultBuffer[j] != 0)&&(j < resultBuffer.length - 1)){
					returnValue[i][k] += (char) resultBuffer[j];
					j++;
				}

				if (j<temp+SRBFileSystem.MAX_FILE_SIZE) {
					j = temp+SRBFileSystem.MAX_FILE_SIZE;
				}
			}
		}

		return parseSRBMetaDataRecordList(fieldCount, recordCount,
			continuationIndex, tabName, attributeName, returnValue );
	}

	/**
	 * Parses the return value after a SRB query was sent.
	 *
	 * @return	SRBMetaDataRecordList the query result.
	 *					The SRBMetaDataRecordList class replaces
	 *		 			mdasC_sql_result_struct *myresult from the C client.
	 */
//TODO internal comments
	SRBMetaDataRecordList[] parseSRBMetaDataRecordList( int fieldCount,
		int recordCount, int continuationIndex, String[] tabName,
		String[] attributeName, String[][] returnValue )
		throws IOException
	{
		SRBMetaDataRecordList[] rl;
		MetaDataField[] fields, fields2 = null;
		Object[] singleReturnValue, singleReturnValue2 = null;
		int i=0, j=0, k=0, l=0, temp=0;

		//Get a list of the fields the query returned
		fields = new MetaDataField[fieldCount];
		for ( i=0; i<fieldCount; i++) {
			fields[i] = SRBMetaDataSet.getGeneralMetaData( tabName[i],
				attributeName[i] );
		}
		if (DEBUG > 3) {
			for ( i=0; i<fieldCount; i++) {
				System.out.println("fields "+fields[i]);
			}
		}


		rl = new SRBMetaDataRecordList[recordCount];

		singleReturnValue = new Object[fieldCount];

		int metaDataRows = 0;
		int fakeRows = 0;
		String fieldName = null;
		for ( i=0; i<recordCount;i++) {
			for ( j=0; j<fieldCount; j++) {
				fieldName = fields[j].getName();
				if ( fieldName == SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES0)
				{
					String nextFieldName = null;
					if (j+1<fieldCount) {
						nextFieldName = fields[j+1].getName();
					}
					if (nextFieldName == SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES1) {
						String fileName = null, directory = null;//TODO null exception
						int nRows;
						int fileIndex = -1;
						int dirIndex = -1;

						//get the filename and directory for comparison of records
						//for uniqueness. fileIndex and dirIndex keep track of
						//where
						for ( k=0,l=0; k<fieldCount; k++, l++) {
							if (fields[k].getName() == SRBMetaDataSet.FILE_NAME) {
								fileName = returnValue[k][i];
								fileIndex = k;
							}
							if (fields[k].getName() == SRBMetaDataSet.DIRECTORY_NAME) {
								directory = returnValue[k][i];
								dirIndex = k;
							}
						}

						//This if statement will allow weird return values, but
						//it also makes the recordlist object more stable.
						if ((fileIndex >= 0) && (dirIndex >= 0)) {
							for (l=0;l<recordCount;l++) {
								if ((returnValue[fileIndex][l].equals( fileName ) ) &&
									(returnValue[dirIndex][l].equals( directory ))) {
									metaDataRows++;
								}
							}

							//the next n rows should actually be one record list.
							//compare the FILE_NAME and DIRECTORY_NAME,
							//every unique combination gets its own MetaDataRecordList.
							//the next ten records of the row are part of the definable table

							String[][] tableValues = new String[metaDataRows][10];
							int[] operators = new int[metaDataRows];

							for ( k=0; k<metaDataRows; k++) {
								for ( l=0; l<tableValues[0].length; l++) {
									tableValues[k][l] = returnValue[j+l][k+i];
								}
								operators[k] = MetaDataCondition.EQUAL;
							}

							singleReturnValue[j] =
								new MetaDataTable( operators, tableValues );
							fakeRows += metaDataRows-1;
							if ( fields2 == null) {
								fields2 = new MetaDataField[fieldCount-9];//same
							}

							i+=metaDataRows-1;
							j+=9;//A user definable table has ten columns
							metaDataRows = 0;
						}
					}
					else {
						singleReturnValue[j] = returnValue[j][i];
					}
				}
				else if (fieldName ==
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES0)
				{
					String nextFieldName = null;
					if (j+1<fieldCount) {
						nextFieldName = fields[j+1].getName();
					}
					if (nextFieldName ==
						SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES1)
					{
						String fileName = null, directory = null;//TODO null exception
						int nRows;
						int dirIndex = -1;

						//get the directory for comparison of records
						//for uniqueness. dirIndex keeps track of where
						for ( k=0,l=0; k<fieldCount; k++, l++) {
							if (fields[k].getName() == SRBMetaDataSet.DIRECTORY_NAME) {
								directory = returnValue[k][i];
								dirIndex = k;
							}
						}


						if (dirIndex >= 0) {
							for (l=0;l<recordCount;l++) {
								if (returnValue[dirIndex][l].equals( directory )) {
									metaDataRows++;
								}
							}

							//the next n rows should actually be one record list.
							//compare the FILE_NAME and DIRECTORY_NAME,
							//every unique combination gets its own MetaDataRecordList.
							//the next ten records of the row are part of the definable table

							String[][] tableValues = new String[metaDataRows][10];
							int[] operators = new int[metaDataRows];

							for ( k=0; k<metaDataRows; k++) {
								for ( l=0; l<tableValues[0].length; l++) {
									tableValues[k][l] = returnValue[j+l][k+i];
								}
								operators[k] = MetaDataCondition.EQUAL;
							}

							singleReturnValue[j] =
								new MetaDataTable( operators, tableValues );
							fakeRows += metaDataRows-1;
							if ( fields2 == null) {
								fields2 = new MetaDataField[fieldCount-9];//same
							}

							i+=metaDataRows-1;
							j+=9;//A user definable table has ten columns
							metaDataRows = 0;
						}
					}
					else {
						singleReturnValue[j] = returnValue[j][i];
					}
				}
				else if (fieldName ==
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS0)
				{
					String nextFieldName = null;
					if (j+1<fieldCount) {
						nextFieldName = fields[j+1].getName();
					}
					if (nextFieldName == SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS1) {
						String userName = null, userDomain = null;//TODO null exception
						int nRows;
						int userNameIndex = -1;
						int userDomainIndex = -1;

						//get the userName and userDomain for comparison of records
						//for uniqueness. userNameIndex and userDomainIndex keep track of
						//where
						for ( k=0,l=0; k<fieldCount; k++, l++) {
							if (fields[k].getName() == SRBMetaDataSet.USER_NAME) {
								userName = returnValue[k][i];
								userNameIndex = k;
							}
							if (fields[k].getName() == SRBMetaDataSet.USER_DOMAIN) {
								userDomain = returnValue[k][i];
								userDomainIndex = k;
							}
						}

						if ((userNameIndex >= 0) && (userDomainIndex >= 0)) {
							for (l=0;l<recordCount;l++) {
								if ( returnValue[userNameIndex][l].equals(userName) &&
									returnValue[userDomainIndex][l].equals(userDomain) ) {
									metaDataRows++;
								}
							}


							//the next n rows should actually be one record list.
							//compare the USER_NAME and USER_DOMAIN, every unique combination
							//gets its own MetaDataRecordList.
							//the next ten records of the row are part of the definable table

							String[][] tableValues = new String[metaDataRows][10];
							int[] operators = new int[metaDataRows];

							for ( k=0; k<metaDataRows; k++) {
								for ( l=0; l<tableValues[0].length; l++) {
									tableValues[k][l] = returnValue[j+l][k+i];
								}
								operators[k] = MetaDataCondition.EQUAL;
							}

							singleReturnValue[j] =
								new MetaDataTable( operators, tableValues );
							fakeRows += metaDataRows-1;
							if ( fields2 == null) {
								fields2 = new MetaDataField[fieldCount-9];//same
							}

							i+=metaDataRows-1;
							j+=9;//A SRB user definable table has ten columns
							metaDataRows = 0;
						}
					}
					else {
						singleReturnValue[j] = returnValue[j][i];
					}
				}
				else if (fieldName ==
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES0)
				{
					String nextFieldName = null;
					if (j+1<fieldCount) {
						nextFieldName = fields[j+1].getName();
					}
					if (nextFieldName == SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES1)
					{
						String fileName = null, resource = null;//TODO null exception
						int nRows;
						int resourceIndex = -1;

						//get the resource for comparison of records
						//for uniqueness. resourceIndex keep track of where
						for ( k=0,l=0; k<fieldCount; k++, l++) {
							if (fields[k].getName() == SRBMetaDataSet.RESOURCE_NAME) {
								resource = returnValue[k][i];
								resourceIndex = k;
							}
						}




						if (resourceIndex >= 0) {
							for (l=0;l<recordCount;l++) {
								if (returnValue[resourceIndex][l].equals( resource )) {
									metaDataRows++;
								}
							}


							//the next n rows should actually be one record list.
							//compare the FILE_NAME and DIRECTORY_NAME,
							//every unique combination gets its own MetaDataRecordList.
							//the next ten records of the row are part of the definable table

							String[][] tableValues = new String[metaDataRows][10];
							int[] operators = new int[metaDataRows];

							for ( k=0; k<metaDataRows; k++) {
								for ( l=0; l<tableValues[0].length; l++) {
									tableValues[k][l] = returnValue[j+l][k+i];
								}
								operators[k] = MetaDataCondition.EQUAL;
							}

							singleReturnValue[j] =
								new MetaDataTable( operators, tableValues );
							fakeRows += metaDataRows-1;
							if ( fields2 == null) {
								fields2 = new MetaDataField[fieldCount-9];//same
							}



							i+=metaDataRows-1;
							j+=9;//A user definable table has ten columns
							metaDataRows = 0;
						}
					}
					else {
						singleReturnValue[j] = returnValue[j][i];
					}
				}
				else {
					singleReturnValue[j] = returnValue[j][i];
				}
			}

			if (fields2 != null) {
				//Disregard the other 9 columns from a user definable table
				singleReturnValue2 = new Object[fieldCount-9];

				for (k=0,l=0;k<singleReturnValue.length;k++) {
					if (singleReturnValue[k] != null) {
						singleReturnValue2[l] = singleReturnValue[k];

						//replace first row field with table field
						if (fields[k].getName() ==
							SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES0)
						{
							fields2[l] = MetaDataSet.getField(
								SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES );
						}
						else if (fields[k].getName() ==
							SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES0)
						{
							fields2[l] = MetaDataSet.getField(
								SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES );
						}
						else if (fields[k].getName() ==
							SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS0)
						{
							fields2[l] = MetaDataSet.getField(
								SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS );
						}
						else if (fields[k].getName() ==
							SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES0)
						{
							fields2[l] = MetaDataSet.getField(
								SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES );
						}
						else
							fields2[l] = fields[k];
						l++;
					}
				}
				rl[i] =	new SRBMetaDataRecordList( fields2, singleReturnValue2,
					continuationIndex, this );
				fields2 = null;
			}
			else {
				rl[i] =	new SRBMetaDataRecordList( fields, singleReturnValue,
					continuationIndex, this );
			}
		}
		if (DEBUG > 3) {
			for ( i=0; i<recordCount;i++) {
				for ( j=0; j<fieldCount; j++) {
					System.out.println("rl["+i+"]."+j+" "+rl[i].getValue(j));
				}
			}
		}

		//hmm, seem to have forgot something somewhere.
		status();


		SRBMetaDataRecordList[] rl2 =
			new SRBMetaDataRecordList[recordCount-fakeRows];
		for (i=0,j=0;i<rl.length;i++) {
			if (rl[i] != null) {
				rl2[j] = rl[i];
				j++;
			}
		}

		return rl2;
	}


//----------------------------------------------------------------------
// SRB C client-like Methods
//----------------------------------------------------------------------
	/**
	 * Create a SRB object.
	 *
	 * @param catType	catalog type. e.g., MDAS_CATALOG.
	 * @param objID	The SRB object ID to create. The objID is a
	 *           user defined name to be registered with MDAS. This
	 *           ID will be used for subsequent reference of the data
	 *           object.
	 *              One or more conditions can be appended to the objID.
	 *              Each condition must be preceded by the character '&'.
	 *              Currently, two conditions are supported:
	 *		1) COPIES=MMM where MMM may be:
	 *		   a) an integer n which means n replica should be created.
	 *		   The "resourceName" input is the logical resource in which
	 *		   this object is to be stored. This logical resource must
	 *		   consist of at least n physical resources. e.g. foo&COPIES=2
	 *		   specifies the creation of two replica of data object "foo".
	 *		   b) the keyword RR which means a single copy should be
	 *		   created in one of the physical resources belonging to
	 *		   the input logical resource ("resourceName") chosen in a
	 *		   Round-Robin fashion. e.g. foo&COPIES=RR.
	 *		   c) the keyword RANDOM produces similar effect as the
	 *		   RR keyword. The only difference is the selection algorithm
	 *		   is random rather than Round-Robin. e.g. foo&COPIES=RANDOM.
	 *		2) CONTAINER=containerName. This keyword specifies the
	 *		object is to be placed in the given container. The
	 *		container must have already been created using the
	 *		srbContainerCreate() call.
	 *
	 * @param dataTypeName	Data type. e.g. "generic"
	 * @param resourceName	The storage resource name. This may be a
	 *		the name of a single resource or a resource group
	 *		(or logical resource) consisting of two or more physical
	 *		resources.
	 *		e.g. "mda18-unix-sdsc"
	 *
	 * @param collectionName	The collection name.
	 * @param localPathName	The file/DB path of the data.
	 * @param dataSize	File size. Used by HPSS to determine COS.
	 *	   -1 => don't know and the default COS will be used.
	 *
	 *
	 * @return the object descriptor.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized int srbObjCreate( int catType, String objID,
		String dataTypeName, String resourceName, String collectionName,
		String localPathName,	long dataSize )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjCreate "+objID+" "+
			collectionName+" "+resourceName);
		startSRBCommand( F_SRBO_CREATE, 7 );

		sendArg( catType );
		sendArg( objID );
		sendArg( dataTypeName );
		sendArg( resourceName );
		sendArg( collectionName );
		sendArg( localPathName );
		sendArg( dataSize );
		flush( );

/*Used for testing server response.
byte resultBuffer[]=read(6);
System.out.print("\n error: \n");
for (int k=0;k<resultBuffer.length;k++)
{
System.out.print(resultBuffer[k]);
System.out.print("="+(char)resultBuffer[k]+"\n");
}
System.out.print("\n");
int errorNum = (int) readUnsignedInt();
System.out.print("error: "+errorNum);
*/

		commandStatus();
		int fd = returnInt();
		if (fd < 0) {
			throw new SRBException( fd );
		}
		else
			return fd;
	}



	/**
	 * Open a SRB object.
	 *
	 * @param objID	The SRB object ID to open. The objID is
	 *				obtained through registration with MDAS. One or more conditions
	 *				can be appended to the objID. Each condition must be
	 *				preceded by the character '&'. Currently, only one condition
	 *				is supported. i.e., COPY=n (where n = replica number beginning
	 *				with 0). e.g. foo&COPY=1 specifies opening replica number
	 *				1 of data object "foo".
	 *
	 * @param openFlag	Unix type open flag. O_CREAT is not supported.
	 * @param collectionName	The name of the collection this objID
	 *											 	belongs.
	 * @return the object descriptor.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized int srbObjOpen( String objID, int openFlag,
		String collectionName )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjOpen");
		startSRBCommand( F_SRBO_OPEN, 3 );

		sendArg( objID );
		sendArg( openFlag );
		sendArg( collectionName );
		flush( );

		commandStatus();
		int fd = returnInt();
		if (fd < 0) {
			throw new SRBException( fd );
		}
		else
			return fd;
	}



	/**
	 * Close an opened object.
	 *
	 * @param srbFD	The object descriptor (from the srbObjOpen call).
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbObjClose( int srbFD )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjClose");
		startSRBCommand( F_SRBO_CLOSE, 1 );

		sendArg( srbFD );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Unlink an SRB object.
	 *
	 * @param objID	The SRB object ID to unlink. The objID is obtained through
	 *		registration with MDAS.
	 * @param collectionName	The name of the collection this objID belongs.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbObjUnlink( String objID, String collectionName )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjUnlink");
		startSRBCommand( F_SRBO_UNLINK, 2 );

		sendArg(objID);
		sendArg(collectionName);
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Read some length of bytes of the SRB object into a buffer.
	 *
	 * @param srbFD	The object descriptor (from the srbObjOpen call) to read.
	 * @param length	The number of bytes to read.
	 * @return a byte array filled with the data read from the server.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized byte[] srbObjRead( int srbFD, int length )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjRead");
		startSRBCommand( F_SRBO_READ, 2 );

		sendArg( srbFD );
		sendArg( length );
		flush( );

		commandStatus();
		return returnBytes();
	}



	/**
	 * Write length bytes of output into the Object srbFD.
	 *
	 * @param srbFD	The Object descriptor to write (from srbObjOpen).
	 * @param outputBuffer[]	The output buffer.
	 * @param length	The length to write.
	 * @return the number of bytes written.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized int srbObjWrite( int srbFD, byte output[], int length )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjWrite");
		startSRBCommand( F_SRBO_WRITE, 2 );

		sendArg( srbFD );
		sendArg( output, length );
		flush( );

		commandStatus();
		int result = returnInt();
		if (result < 0) {
			throw new SRBException( "Write failed", result );
		}
		else
			return result;
	}



	/**
	 * Change the current read or write location on an srb obj file
	 * currently.
	 *
	 * @param desc	The object descriptor (from the srbObjOpen call) to seek.
	 * @param offset	int whence - Same definition as in Unix.
	 * @param whence	Same definition as in Unix.
	 * 		SEEK_SET	pointer is set to the value of the Offset parameter.
	 * 		SEEK_CUR	pointer is set to its current location plus the
	 * 			value of the Offset parameter.
	 * 		SEEK_END	pointer is set to the size of the file plus the
	 * 			value of the Offset parameter.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbObjSeek( int desc, long offset, int whence )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjSeek");
		startSRBCommand( F_SRBO_SEEK, 3 );

		sendArg( desc );
		sendArg( offset );
		sendArg( whence );
		flush( );

		commandStatus();
		int status = (int) returnLong();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Sync an opened object (call fsync for UNIX FS).
	 *
	 * @param desc	The object descriptor (from the srbObjOpen call).
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbObjSync( int desc )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjSync");
		startSRBCommand( F_SRBO_SYNC, 1 );

		sendArg( desc );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Stat a srb Path.
	 *
	 * @param catType	The catalog type.
	 * @param filePath	the SRB path.
	 * @param myType	file or dir state : IS_UNKNOWN, IS_FILE, IS_DIR_1,
	 *			IS_DIR_2, IS_DIR_3, IS_DIR_4.
	 * @return statBuffer array of stat values upon success.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized long[] srbObjStat( int catType, String filePath, int myType )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjStat");
		long statBuffer[] = new long[30];
		long temp = 0;
		int i=0, j=0;
		int endLoop = 0; //stores array index of when to end a for loop
		int resultLength = 0;

		startSRBCommand( F_SRBO_STAT, 3 );
		sendArg( catType );
		sendArg( filePath );
		sendArg( myType );
		flush( );

		commandStatus();
		byte resultBuffer[] = returnBytes();

		//interprete the return as a array of Stat values.
		if ( resultBuffer != null) {
			//statBuffer[0], file length
			for (j=0;j<8;j++) {
				if ( resultBuffer[j] < 0 ) {
					temp = (temp << 8) + 256 + resultBuffer[j];
				}
				else {
					temp = (temp << 8) + resultBuffer[j];
				}
			}
			statBuffer[0] = temp;
			i++;

			// statBuffer[1-21]
			for (;i<statBuffer.length-7;i++) {
				temp = 0;
				endLoop = j+4;
				for (;j<endLoop;j++) {
					if ( resultBuffer[j] < 0 ) {
						temp = (temp << 8) + 256 + resultBuffer[j];
					}
					else {
						temp = (temp << 8) + resultBuffer[j];
					}
				}
				statBuffer[i] = temp;
			}

			// statBuffer[22], Process' access to file
			// statBuffer[23], null
			for (;i<statBuffer.length-5;i++) {
				temp = 0;
				endLoop = j+2;
				for (;j<endLoop;j++) {
					if ( resultBuffer[j] < 0 ) {
						temp = (temp << 8) + 256 + resultBuffer[j];
					}
					else {
						temp = (temp << 8) + resultBuffer[j];
					}
				}
				statBuffer[i] = temp;
			}


			// statBuffer[24-29], Reserved
			for (;i<statBuffer.length;i++) {
				temp = 0;
				endLoop = j+4;
				for (;j<endLoop;j++) {
					if ( resultBuffer[j] < 0 ) {
						temp = (temp << 8) + 256 + resultBuffer[j];
					}
					else {
						temp = (temp << 8) + resultBuffer[j];
					}
				}
				statBuffer[i] = temp;
			}
		}
		return statBuffer;
	}



	/**
	 * Stat a srb Path The result is placed in statbuf which
	 * is of type (strust stat) of the local fs.
	 *
	 * @param catType The catalog type.
	 * @param path the SRB path.
	 * @return stat result in statbuf.
	 * @throws IOException	If an IOException occurs
	 */
//TODO	Is this needed?
	long[] srbObjStat64( int catType, String path )
		throws IOException
	{
		return srbObjStat(catType, path, 0);//TODO IS_UNKNOWN);
/*
			struct srbStat mySrbStat;
			int status;

			status = _srbObjStat (catType, path, &mySrbStat, IS_UNKNOWN);
			if (status >= 0)
		status = srbToLocalStat64 (&mySrbStat, statbuf);

			return status;
*/
	}



	/**
	 * Make a copy of an SRB object
	 *
	 * @param	catType	catalog type. e.g., MDAS_CATALOG.
	 * @param	objID	The SRB object ID to unlink. The objID is obtained
	 *		through registration with MDAS.
	 * @param	collectionName	The name of the collection this objID belongs.
	 * @param	origPathName	The file/DB path of the original copy.
	 * @param	newResourceName	The storage resource name of the new copy.
	 *		e.g. "mda18-unix-sdsc"
	 * @param	newPathName	The file/DB path of the new copy.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbObjReplicate( int catType, String objID,
		String collectionName, String newResourceName, String newPathName )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjReplicate");
		startSRBCommand( F_SRBO_REPLICATE, 5 );

		sendArg( catType );
		sendArg( objID );
		sendArg( collectionName );
		sendArg( newResourceName );
		sendArg( newPathName );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}




	/**
	 * Move a copy of an SRB object to a new location.
	 *
	 * @param catType	catalog type. e.g., MDAS_CATALOG.
	 * @param objID	The SRB object ID to unlink. The objID is
	 *		obtained through registration with MDAS.
	 * @param collectionName	The name of the collection this objID belongs.
	 * @param newResourceName	The storage resource name of the new copy.
	 *		e.g. "mda18-unix-sdsc"
	 * @param newPathName	The file/DB path of the new copy.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbObjMove( int catType, String objID,
		String collectionName, String newResourceName, String newPathName )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjMove");
		startSRBCommand( F_SRBO_MOVE, 5 );

		sendArg(catType);
		sendArg(objID);
		sendArg(collectionName);
		sendArg(newResourceName);
		sendArg(newPathName);
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}


	/**
	 * Proxy Operation request.
	 *
	 * @param operation	The type of proxy operation. Valid operations:
	 *		OPR_COPY - Copy from the object descriptor given in sourceInp
	 *		to the object descriptor given in destInp.
	 *		If successful, the number of bytes copied is returned. Negative
	 *		values mean failure.
	 *
	 	OPR_COPY,
    OPR_COMMAND,
    SFO_CREATE_INDEX,
    SFO_DELETE_INDEX,
    SFO_SEARCH_INDEX,
    SFO_GET_MORE_SEARCH_RESULT,
    SFO_APPLY_FILTER,
    SFO_GET_MORE_FILTER_RESULT

		{OPR_COPY, (proxyFuncPtr) proxyCopy, "proxyCopy"},
		{OPR_COMMAND, (proxyFuncPtr) proxyCommand, "proxyCommand"},
		{SFO_CREATE_INDEX,(proxyFuncPtr) svrSfoCreateIndex, "svrSfoCreateIndex"},
		{SFO_DELETE_INDEX,(proxyFuncPtr) svrSfoDeleteIndex, "svrSfoDeleteIndex"},
		{SFO_SEARCH_INDEX,(proxyFuncPtr) svrSfoSearchIndex, "svrSfoSearchIndex"},
		{SFO_GET_MORE_SEARCH_RESULT,(proxyFuncPtr) svrSfoGetMoreSearchResult, "svrSfoGetMoreSearchResult"},
		{SFO_APPLY_FILTER,(proxyFuncPtr) svrSfoApplyFilter, "svrSfoApplyFilter"},
    {SFO_GET_MORE_FILTER_RESULT,(proxyFuncPtr) svrSfoGetMoreFilterResult, "svrSfoGetMoreFilterResult"},

	 * @param inputInt1	intput integer 1.
	 * @param inputInt2	intput integer 2.
	 * @param inputInt3	intput integer 3.
	 * @param inputInt4	intput integer 4.
	 * @param inputStr1	Input String 1.
	 * @param inputStr2	Input String 2.
	 * @param inputStr3	Input String 3.
	 * @param inputStr4	Input String 4.
	 * @param inputBStrm1	Input Byte stream 1.
	 * @param inputBStrm2	Input Byte stream 2.
	 * @param inputBStrm3	Input Byte stream 3.
	 * @return outBuf	any byte stream output.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized byte[] srbObjProxyOpr( int operation,
		int inputInt1, int inputInt2, int inputInt3, int inputInt4,
		String inputStr1, String inputStr2,	String inputStr3, String inputStr4,
		byte[] inputBStrm1,	byte[] inputBStrm2, byte[] inputBStrm3 )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjProxyOpr");
		startSRBCommand( F_SRBO_PROXY_OPR, 12 );

		sendArg( operation );
		sendArg( inputInt1 );
		sendArg( inputInt2 );
		sendArg( inputInt3 );
		sendArg( inputInt4 );
		sendArg( inputStr1 );
		sendArg( inputStr2 );
		sendArg( inputStr2 );
		sendArg( inputStr4 );
		sendArg( inputBStrm1 );
		sendArg( inputBStrm2 );
		sendArg( inputBStrm3 );
		flush( );

		commandStatus();

//Sometimes returns bytes, other times returns int?
		byte resultBuffer[] = read(4);
		int resultLength = 0;

		for (int i=resultBuffer.length-1;i>=0;i--) {
			if ( resultBuffer[i] < 0 ) {
				resultLength = (resultLength << 8) + 256 + resultBuffer[i];
			}
			else {
				resultLength = (resultLength << 8) + resultBuffer[i];
			}
		}

		if (resultLength > 4) {
			//ignore sizeOf(resultLength) from result to get real data length.
			read(4);
			resultLength -= 4;

			resultBuffer = read( resultLength );
			read(1); //get final '0';
			return resultBuffer;
		}
		else {
			resultBuffer = read(4);
			status();
			return resultBuffer;
		}
	}



	/**
	 * The SRB equivalent of the getdents call.
	 *
	 * @param catType The catalog type.
	 * @param fd the opened fd.
	 *         dirent_t *buf The place to put the dirents result.
	 *	   size_t nbyte The max number of bytes to output.
	 * @return dirents result in buf.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized byte[] srbObjGetdents( int catType, int srbFD, long nbyte )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjGetdents");
		startSRBCommand( F_SRBO_GET_DENTS, 3 );

		sendArg( catType );
		sendArg( srbFD );
		sendArg( nbyte );
		flush( );

		commandStatus();
		return returnBytes();

/*TODO
typedef struct myDirent {
        long            d_ino;          // "inode number" of entry
        int         	d_off;          // offset of disk directory entry
        unsigned short  d_reclen;       // length of this record
        char            d_name[1];      // name of file
} dirent_t;

			if (retval >= 0) {
					// For c90, pat it to 64 bit boundary
					if (retval != retval / sizeof (int) * sizeof (int)) {
							retval = retval / sizeof (int) * sizeof (int) +
								sizeof (int);
					} else {
							// Convert the elements of the dirent_t.
				if (retval > 0)
									ntohDirent (buf, retval);
					}
			}
			return retval;
*/
	}

	/**
	 * The SRB equivalent of the getdents64 call.
	 *
	 * @param catType The catalog type.
	 * @param fd the opened fd.
	 *         dirent_t *buf The place to put the dirents result.
	 *	   size_t nbyte The max number of bytes to output.
	 * @return dirents result in buf.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized byte[] srbObjGetdents64( int catType, int srbFD, long nbyte )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjGetdents64");
		startSRBCommand( F_SRBO_GET_DENTS64, 3 );

		sendArg( catType );
		sendArg( srbFD );
		sendArg( nbyte );
		flush( );

		commandStatus();
		return returnBytes();

//TODO return dirent_t see srbObjGetdents
	}



	/**
	 * Seek into a collection. A collection must have been opened using
	 * srbObjOpen.
	 *<P>
	 * @param desc	The object descriptor (from the srbObjOpen call) to seek.
	 * @param offset	The position of the next operation
	 * @param whence	Same definition as in Unix.
	 *		SEEK_SET	pointer is set to the value of the Offset parameter.
	 *		SEEK_CUR	pointer is set to its current location plus the
	 *			value of the Offset parameter.
	 *		SEEK_END	pointer is set to the size of the file plus the
	 *			value of the Offset parameter.
	 * @param is64Flag	valid vaiue: IS_64_BIT or IS_32_BIT.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbCollSeek( int desc, int offset, int whence,
		int is64Flag )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbCollSeek");
		startSRBCommand( F_SRBC_SEEK, 4 );

		sendArg( desc );
		sendArg( offset );
		sendArg( whence );
		sendArg( is64Flag );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * srbGetDatasetInfo Get Info on a SRB data object.
	 *
	 * @param catType catalog type. e.g., MDAS_CATALOG.
	 * @param objID	The SRB object ID to unlink. The objID is
	 *		obtained through registration with MDAS.
	 * @param collectionName	The name of the collection this objID belongs.
	 * @param rowsWanted number of rows of result wanted.
	 * @return SRBMetaDataRecordList. Use srbGetMoreRows() to retrieve more rows.
	 */
	synchronized SRBMetaDataRecordList[] srbGetDatasetInfo( int catType,
		String objID, String collectionName, int rowsWanted )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbGetDatasetInfo");
		startSRBCommand( F_SRBO_GET_LOID_INFO, 4 );

		sendArg( catType );
		sendArg( objID );
		sendArg( collectionName );
		sendArg( rowsWanted );
		flush( );

		commandStatus();
		return returnSRBMetaDataRecordList();
	}



	/**
	 * Get metadata info by querying the MDAS catalog.
	 * The server uses the input qval[][] and selval[] array to compose
	 * and execute SQL queries and returns the query result in
	 * SRBMetaDataRecordList. The selval[] array specifies a list of attrbutes
	 * to retrieve and qval[][] specifies a lists of "=" predicates to search.
	 * Both selval[] and qval[][] must be arrays of size
	 * SRBFileSystem.TOTAL_METADATA_ATTRIBUTES and are indexed by values given
	 * in mdasC_db2_externs.h under the heading DCS-ATTRIBUTE-INDEX DEFINES.
	 *<P>
	 * For the selval[] array, setting an element of the array to 1 means that
	 * the attribute associated with this element is to be retrieved. e.g.,
	 * selval[USER_NAME] = 1; means the "user_name" attribute is to be retrieved.
	 *<P>
	 * The qval[][] array specifies the "=" predicates to search. e.g.,
	 * sprintf(qval[FILE_NAME],"'%s'", "unixFileObj1"); means that the search
	 * condition includes the term (data_name = "unixFileObj1").
	 *<P>
	 * An example of srbGetDataDirInfo:
	 *<br>
	 *  mdasC_sql_result_struct srbMetaDataRecordList;
	 *  char qval[SRBFileSystem.TOTAL_METADATA_ATTRIBUTES][SRBFileSystem.MAX_TOKEN];
	 *  int selval[SRBFileSystem.TOTAL_METADATA_ATTRIBUTES];
	 *<P>
	 * for (i = 0; i < SRBFileSystem.TOTAL_METADATA_ATTRIBUTES; i++) {
	 *      selval[i] = 0;
	 *      sprintf(qval[i],"");
	 *  }
	 *  sprintf(qval[FILE_NAME],"'%s'",argv[2]);
	 *  selval[PATH_NAME] = 1;
	 *  selval[RESOURCE_NAME] = 1;
	 *	.
	 *	.
	 *  if (srbGetDataDirInfo(
	 *	 conn, MDAS_CATALOG, qval, selval, &srbMetaDataRecordList) < 0)
	 *	{
	 *		fprintf(stderr, "can't srbGetDataDirInfo \n");
	 *		exit_nicely(conn);
	 *  }
	 *
	 *
	 * @param catType catalog type. e.g., MDAS_CATALOG.
	 * @param metaDataConditions The query conditions.
	 * @param metaDataSelects	The query selects.
	 * @param rowsWanted	number of rows of result wanted.
	 *
	 * @return SRBMetaDataRecordList. Use srbGetMoreRows() to retrieve more rows.
	 */
	synchronized SRBMetaDataRecordList[] srbGetDataDirInfo( int catType,
		MetaDataCondition[] metaDataConditions, MetaDataSelect[] metaDataSelects,
		int rowsWanted, boolean orderBy,	boolean nonDistinct )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbGetDataDirInfo "+
			srbGetDataDirInfoCount);srbGetDataDirInfoCount++;

		//This just became too big a mess.
		SRBMetaDataCommands mdc = new SRBMetaDataCommands( this );
		return mdc.srbGetDataDirInfo( catType, metaDataConditions, metaDataSelects,
			rowsWanted, orderBy, nonDistinct );
	}
//There is a the possibility that a lot of unnecessary queries get sent.
//This is to help with keeping track and reducing the number.
int srbGetDataDirInfoCount=0;


	/**
	 * This is a more compact form of srbGetDataDirInfo.
	 * Instead of using fixed array of selval and qval, it uses the
	 * genQuery struct.
	 * <P>
	 * Only valid for version 3.x.x and above.
	 * Method will not work for version 2.x.x, or before.
	 *
	 * @param catType catalog type. e.g., MDAS_CATALOG.
	 * @param myMcatZone The MCAT zone for this query.
	 * @param metaDataConditions The query conditions.
	 * @param metaDataSelects	The query selects.
	 * @param rowsWanted The number of rows of result wanted.
	 *
	 * @return SRBMetaDataRecordList. Use srbGetMoreRows() to retrieve more rows.
	 *
	 * Note : We cannot use the generic routine clCall() because the input
	 * byte stream is not a char string.
	 */
	SRBMetaDataRecordList[] srbGenQuery( int catType, String myMcatZone,
		MetaDataCondition[] metaDataConditions,	MetaDataSelect[] metaDataSelects,
		int rowsWanted, boolean orderBy, boolean nonDistinct )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbGenQuery");
		//This just became too big a mess.
		SRBMetaDataCommands mdc = new SRBMetaDataCommands( this );
		return mdc.srbGenQuery( catType, myMcatZone, metaDataConditions,
			metaDataSelects, rowsWanted, orderBy, nonDistinct );
	}


	/**
	 * Register a SRB dataset
	 *
	 * @param catType catalog type. e.g., MDAS_CATALOG.
	 * @param objID The SRB object ID to create. The objID is obtained
	 *              through registration with MDAS.
	 * @param dataTypeName Data type. e.g. "generic"
	 * @param resourceName The storage resource name.
	 *              e.g. "mda18-unix-sdsc"
	 * @param collectionName The collection name.
	 * @param pathName The file/DB path of the data.
	 * @param dataSize The size of the dataset if known. 0 = unknown.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbRegisterDataset( int catType, String objID,
		String dataTypeName, String resourceName, String collectionName,
		String pathName, long dataSize )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRegisterDataset");
		startSRBCommand( F_SRBO_REGISTER_FILE, 7 );

		sendArg( catType );
		sendArg( objID );
		sendArg( dataTypeName );
		sendArg( resourceName );
		sendArg( collectionName );
		sendArg( pathName );
		sendArg( dataSize );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Modify a SRB dataset.
	 *
	 * @param catType	catalog type. e.g., MDAS_CATALOG.
	 * @param objID	The SRB object ID to modify. The objID must
	 *		already have been registered with the MDAS catalog.
	 * @param collectionName	The name of the collection this objID belongs.
	 * @param resourceName	The storage resource name.
	 *		e.g. "mda18-unix-sdsc"
	 * @param pathName	The file/DB path of the data.
	 * @param dataValue1	Input value 1.
	 * @param dataValue2	Input value 2.
	 * @param actionType	The type of retraction. See srbC_mdas_externs.h
	 *		for the actionType definition.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized int srbModifyDataset( int catType, String objID,
		String collectionName, String resourceName, String pathName,
		String dataValue1, String dataValue2, int actionType )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbModifyDataset "+catType+" "+
			objID+" "+collectionName+" "+ resourceName+" "+ pathName+" "+
			dataValue1+" "+ dataValue2+" "+ actionType);
		startSRBCommand( F_SRBO_MODIFY_FILE, 8 );

		sendArg( catType );
		sendArg( objID );
		sendArg( collectionName );
		sendArg( resourceName );
		sendArg( pathName );
		sendArg( dataValue1 );
		sendArg( dataValue2 );
		sendArg( actionType );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
		else
			return status;
	}



	/**
	 * Authenticate a userName/passwd.
	 *
	 * @param userName   and
	 * @param srbAuth The userName/passwd pair to authenticate.
	 * @param mdasDomain The MDAS Domain.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbChkMdasAuth( String userName, String srbAuth,
		String mdasDomain )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbChkMdasAuth");
		startSRBCommand( F_CHK_MDAS_AUTH, 3 );

		sendArg( userName );
		sendArg( srbAuth );
		sendArg( mdasDomain );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Create a SRB collection.
	 *
	 * @param catType	catalog type. e.g., MDAS_CATALOG.
	 * @param parentCollect	The parent collection in which to create
	 *		the new collection.
	 * @param newCollect	The name of the collection to create.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbCreateCollect( int catType, String parentCollect,
		String newCollect )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbCreateCollect");
		startSRBCommand( F_CREATE_DIRECTORY, 3 );

		sendArg(catType);
		sendArg(parentCollect);
		sendArg(newCollect);
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * List a SRB collection
	 *
	 * @param catType catalog type. e.g., MDAS_CATALOG.
	 * @param collectionName The collection to list.
	 * @param flag "C" - non-recursive. "R" - recursive
	 *	   mdasC_sql_result_struct *listResult - The address points to the
	 *	     result.  A pointer to a user supplied mdasC_sql_result_struct.
	 * @return The list of items in the collection.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized SRBMetaDataRecordList[] srbListCollect( int catType,
		String collectionName, String flag, int rowsWanted )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbListCollect");
		startSRBCommand( F_LIST_DIRECTORY, 4 );

		sendArg(catType);
		sendArg(collectionName);
		sendArg(flag);
		sendArg(rowsWanted);
		flush( );

		commandStatus();
		return returnSRBMetaDataRecordList();
	}



	/**
	 * Modify a SRB collection.
	 *
	 * @param catType	catalog type. e.g., MDAS_CATALOG.
	 * @param collectionName	The name of the collection to modify.
	 * @param dataValue1	Input value 1.
	 * @param dataValue2	Input value 2.
	 * @param dataValue3	Input value 3.
	 * @param actionType	The type of retraction. See srbC_mdas_externs.h
	 *				for the actionType definition.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbModifyCollect( int catType, String collectionName,
		String dataValue1, String dataValue2, String dataValue3,
		int actionType )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbModifyCollect "+catType+" "+
			collectionName+" "+dataValue1+" "+dataValue2+" "+dataValue3+" "+
			actionType);
		startSRBCommand( F_MODIFY_DIRECTORY, 6 );

		sendArg( catType );
		sendArg( collectionName );
		sendArg( dataValue1 );
		sendArg( dataValue2 );
		sendArg( dataValue3 );
		sendArg( actionType );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Authenticate a userName/passwd for sys admin access.
	 *
	 * @param userName userName to authenticate.
	 * @param srbAuth password to authenticate.
	 * @param mdasDomain The MDAS Domain.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbChkMdasSysAuth( String userName, String srbAuth,
		String mdasDomain )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbChkMdasSysAuth");
		startSRBCommand( F_CHK_MDAS_SYS_AUTH, 3 );

		sendArg( userName );
		sendArg( srbAuth );
		sendArg( mdasDomain );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



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
	synchronized void srbRegisterUserGrp( int catType, String userGrpName,
		String userGrpPasswd, String userGrpType, String userGrpAddress,
		String userGrpPhone, String userGrpEmail )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRegisterUserGrp");
		startSRBCommand( F_REGISTER_USER_GROUP, 7 );

		sendArg( catType );
		sendArg( userGrpName );
		sendArg( userGrpPasswd );
		sendArg( userGrpType );
		sendArg( userGrpAddress );
		sendArg( userGrpPhone );
		sendArg( userGrpEmail );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



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
	synchronized void srbRegisterUser( int catType, String userName,
		String userDomain, String userPasswd, String userType, String userAddress,
		String userPhone, String userEmail )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRegisterUser");
		startSRBCommand( F_REGISTER_USER, 8 );

		sendArg( catType );
		sendArg( userName );
		sendArg( userDomain );
		sendArg( userPasswd );
		sendArg( userType );
		sendArg( userAddress );
		sendArg( userPhone );
		sendArg( userEmail );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



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
	synchronized void srbModifyUser( int catType, String dataValue1,
		String dataValue2, int actionType )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbModifyUser");
		startSRBCommand( F_MODIFY_USER, 4 );

		sendArg( catType );
		sendArg( dataValue1 );
		sendArg( dataValue2 );
		sendArg( actionType );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Setting and Unsetting Audit Trail.
	 *
	 * @param set_value The Audit Trail value to set.
	 *		AUDIT_TRAIL_OFF turn on audit trail.
	 *		AUDIT_TRAIL_ON turn on audit trail.
	 *		GET_AUDIT_TRAIL_SETTING return the cuurent audit trail
	 *		  setting without modifying the setting.
	 *
	 * @return the currently audit trail setting (after processing
	 *	    the latest change request).
	 * @throws IOException	If an IOException occurs
	 */
	synchronized int srbSetAuditTrail( int set_value )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbSetAuditTrail");
		startSRBCommand( F_SET_AUDIT_TRAIL, 1 );

		sendArg( set_value );
		flush( );
//TODO what does it return?
		commandStatus();
		return returnInt();
	}



	/**
	 * Make an audit entry for an object
	 *
	 * @param catType catalog type. e.g., MDAS_CATALOG.
	 * @param userName The userName.
	 * @param objID The objID of the object to audit.
	 * @param collectionName The name of the collection this objID
	 *              belongs.
	 * @param dataPath The path name of the object.
	 * @param resourceName the resource name of the object.
	 * @param accessMode The access mode ("read", "write", "all");
	 * @param comment comments to be included.
	 *	   int success Indication whether the operation was successful.
	 *	     0 = failure, 1 success.
	 * @param domainName The domain name of the user.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbObjAudit( int catType,	String userName, String objID,
		String collectionName,  String dataPath,  String resourceName,
		String accessMode,  String comment, int success,  String domainName )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjAudit");
		startSRBCommand( F_SRBO_AUDIT, 10 );

		sendArg( catType );
		sendArg( userName );
		sendArg( objID );
		sendArg( collectionName );
		sendArg( dataPath );
		sendArg( resourceName );
		sendArg( accessMode );
		sendArg( comment );
		sendArg( success );
		sendArg( domainName );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Register a SRB Replica
	 *
	 * @param catType catalog type. e.g., MDAS_CATALOG.
	 * @param objID The SRB object ID to create. The objID is obtained
	 *              through registration with MDAS.
	 * @param collectionName The collection name.
	 * @param origPathName The orighnal file/DB path of the data.
	 *              (This entry can be NULL).
	 * @param newResourceName The new storage resource name.
	 *              e.g. "mda18-unix-sdsc"
	 * @param newPathName The new file/DB path of the data.
	 * @param userName The new file/DB path of the data.
	 *         struct varlena *domainName The domain name to which the object
	 *         belongs. Valid domains "sdsc".
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbRegisterReplica( int catType, String objID,
		String collectionName, String origResourceName, String origPathName,
		String newResourceName, String newPathName, String userName,
		String domainName )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRegisterReplica");
		startSRBCommand( F_REGISTER_REPLICA, 9 );

		sendArg( catType );
		sendArg( objID );
		sendArg( collectionName );
		sendArg( origResourceName );
		sendArg( origPathName );
		sendArg( newResourceName );
		sendArg( newPathName );
		sendArg( userName );
		sendArg( domainName );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Read the privileged users list and put it in
	 * a user supplied mdasC_sql_result_struct.
	 *
	 * @param catalog The catalog type. e.g., MDAS_CATALOG
	 * @param rowsWanted The number of rows to be returned.
	 * @return SRBMetaDataRecordList. Use srbGetMoreRows() to retrieve more rows.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized SRBMetaDataRecordList[] srbGetPrivUsers( int catalog,
		int rowsWanted )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbGetPrivUsers");
		startSRBCommand( F_GET_PRIV_USERS, 2 );

		sendArg( catalog );
		sendArg( rowsWanted );
		flush( );

		commandStatus();
		return returnSRBMetaDataRecordList();
	}



	/**
	 * Get more rows of result from a srbGetDatasetInfo,
	 * srbGetDataDirInfo, srbListCollect or srbGetPrivUsers calls and put
	 * the results in a user supplied mdasC_sql_result_struct.
	 *
	 * @param catalog The catalog type. e.g., MDAS_CATALOG
	 *	   mdasC_sql_result_struct *srbUserList A pointer to a user supplied
	 *	   mdasC_sql_result_struct.
	 * @param contDesc The continuation descriptor. This is a non negative
	 *	   integer returned from a srbGetDatasetInfo, srbGetDataDirInfo,
	 *	   srbListCollect or srbGetPrivUsers call as
	 *	   SRBMetaDataRecordList->continuationIndex.
	 * @param rowsWanted The number of rows to be returned.
	 * @return the values from the last query.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized SRBMetaDataRecordList[] srbGetMoreRows( int catalog,
		int contDesc, int rowsWanted )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbGetMoreRows "+catalog+" "+
			contDesc+" "+rowsWanted);

		float version = SRBAccount.getVersionNumber();
		if (contDesc < 0)
			return null;
		else if (version >= 3)
		{
			//TODO zone
			return srbGenGetMoreRows(catalog, contDesc, rowsWanted);
		}

		try {
			startSRBCommand( F_GET_MORE_ROWS, 3 );

			sendArg( catalog );
			sendArg( contDesc );
			sendArg( rowsWanted );
			flush( );

			commandStatus();
			return returnSRBMetaDataRecordList();
		}
		catch( SRBException e ) {
			if (e.getType() == -3005) {
				//MCAT error, the object queried does not exist
				return null;
			}
			else
				throw e;
		}
	}


	/**
	 * This is a more compact form of srbGetMoreRows. The result is packed
	 * with packMsg. Get more rows of result from a srbGetDatasetInfo,
	 * srbGetDataDirInfo, srbListCollect or srbGetPrivUsers calls and put
	 * the results in a user supplied mdasC_sql_result_struct.
	 *
	 * @param catalog The catalog type. e.g., MDAS_CATALOG
	 * @param contDesc The continuation descriptor. This is a non negative
	 *	   integer returned from a srbGetDatasetInfo, srbGetDataDirInfo,
	 *	   srbListCollect or srbGetPrivUsers call as
	 *	   SRBMetaDataRecordList.continuationIndex.
	 * @param rowsWanted The number of rows to be returned.
	 * @return Further results from the query.
	 */
	SRBMetaDataRecordList[] srbGenGetMoreRows( int catalog, int contDesc,
		int rowsWanted )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbGenGetMoreRows "+catalog+" "+
			contDesc+" "+rowsWanted);
		if (contDesc < 0)
			return null;

		try {
			startSRBCommand( F_GEN_GET_MORE_ROWS, 3 );

			sendArg( catalog );
			sendArg( contDesc );
			sendArg( rowsWanted );
			flush( );

			commandStatus();
			return returnSRBMetaDataRecordList(true);
		}
		catch( SRBException e ) {
			if (e.getType() == -3005) {
				//MCAT error, the object queried does not exist
				return null;
			}
			else
				throw e;
		}
	}


	/**
	 * Issue a ticket.
	 *
	 * @param objID The object ID
	 * @param collectionName The collection name
	 * @param collectionFlag The collect flag if vCollectionName
	 *              is non NULL. "R" the ticket is for all dataset and
	 *              sub-collection recursively. "D" the ticket is for the
	 *              datasets directly beneath the colloection.
	 * @param beginTime The beginning time when the ticket
	 *              becomes effective. A NULL means no time limit.
	 * @param endTime The ending time of the ticket.
	 * @param accessCnt The number of time the ticket can be used to
	 *              access the dataset.
	 * @param ticketUser The user/userGroup that will use the
	 *              ticket. Multiply users can be specified with the following
	 *		format:
	 *                      user1@domain1&user2@domain2 ....
	 *		If it is NULL, => all users.
	 * @param ticket The address to put the output ticket.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbIssueTicket( String objID, String collectionName,
		String collectionFlag, String beginTime, String endTime, int accessCnt,
		String ticketUser )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbIssueTicket");
		startSRBCommand( F_ISSUE_TICKET, 7 );

		sendArg( objID );
		sendArg( collectionName );
		sendArg( collectionFlag );
		sendArg( beginTime );
		sendArg( endTime );
		sendArg( accessCnt );
		sendArg( ticketUser );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
/*
*ticket

			retval = clCall (conn, F_ISSUE_TICKET, argv, &clOut);

			if (retval < 0) {
					return retval;
			}

			*ticket = malloc (strlen (buf) + 1);
			strcpy (*ticket, buf);

			return (0);
*/
	}



	/**
	 * Cancel a ticket.
	 *
	 * @param	ticket	The ticket to remove.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbRemoveTicket( String ticket )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRemoveTicket");
		startSRBCommand( F_REMOVE_TICKET, 1 );

		sendArg( ticket );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Unregister an SRB object
	 *
	 * @param	objID	The SRB object ID to unlink. The objID is obtained
	 *              through registration with MDAS.
	 * @param	collectionName	The name of the collection this objID
	 *              belongs.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbUnregisterDataset( String objID, String collectionName )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbUnregisterDataset");
		startSRBCommand( F_UNREGISTER_FILE, 2 );

		sendArg( objID );
		sendArg( collectionName );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Create a container
	 *
	 * @param	catType	catalog type. e.g., MDAS_CATALOG.
	 * @param	containName	The name of the container to be created.
	 * @param	containerType	Data type of the container. e.g. "generic"
	 * @param	resourceName	The storage resource name. This should be is a
	 *		logical resource (resource group) consisting of two physical
	 *		resources, a TEMPORARY_RES_CL and a PERMANENT_RES_CL class.
	 * @param	containerSize	The size of the container to be created.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbContainerCreate( int catType, String containerName,
		String containerType, String resourceName, long containerSize )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbContainerCreate");
		startSRBCommand( F_CONTAINER_CREATE, 5 );

		sendArg( catType );
		sendArg( containerName );
		sendArg( containerType );
		sendArg( resourceName );
		sendArg( containerSize );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Register a container to MCAT.
	 *
	 * @param catType catalog type. e.g., MDAS_CATALOG.
	 * @param containerName The name of the container to be registered..
	 * @param resourceName The storage resource name.
	 *              e.g. "mda18-unix-sdsc"
	 * @param containerSize The size of the container.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbRegisterContainer( int catType, String containerName,
	 String resourceName, long containerSize )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRegisterContainer");
		startSRBCommand( F_SRBO_CREATE, 4 );

		sendArg( catType );
		sendArg( containerName );
		sendArg( resourceName );
		sendArg( containerSize );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Register a inContainer dataset
	 *
	 * @param catType catalog type. e.g., MDAS_CATALOG.
	 * @param objID The SRB object ID to create. The objID is obtained
	 *              through registration with MDAS.
	 * @param collectionName The collection name.
	 * @param containerName The collection name.
	 * @param dataTypeName Data type. e.g. "generic"
	 * @param dataSize The size of the dataset if known. 0 = unknown.
	 * @param baseOffset The offset of the dataset if known.
	 *				   0 = unknown.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbRegisterInContDataset( int catType, String objID,
		String collectionName, String containerName, String dataTypeName,
		long dataSize, long baseOffset )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRegisterInContDataset");
		startSRBCommand( F_REGISTER_IN_CONTAINER, 7 );

		sendArg( catType );
		sendArg( objID );
		sendArg( collectionName );
		sendArg( containerName );
		sendArg( dataTypeName );
		sendArg( dataSize );
		sendArg( baseOffset );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Get Info on a SRB container.
	 *
	 * @param catType The catalog type. e.g., MDAS_CATALOG
	 * @param containerName The name of the container.
	 * @param rowsWanted number of rows of result wanted.
	 * @return SRBMetaDataRecordList. Use srbGetMoreRows() to retrieve more rows.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized SRBMetaDataRecordList[] srbGetContainerInfo( int catType,
		String containerName, int rowsWanted )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbGetContainerInfo");
		startSRBCommand( F_GET_CONTAINER_INFO, 3 );

		sendArg( catType );
		sendArg( containerName );
		sendArg( rowsWanted );
		flush( );

		commandStatus();
		return returnSRBMetaDataRecordList();
	}



	/**
	 * Given the logical resource name and the inputFlag,
	 * return a physical resource name.
	 *
	 * @param catType catalog type 0 MCAT
	 * @param logResName The logical resource name.
	 * @param phyResName The output physical resource.
	 * @param inputFlag The Input flag, valid inputs are:
	 *          "RR" Round Robin.
	 *          "RANDOM" randomly selecting a physical resource from the
	 *                     input logical resource.
	 * @return The Physical resource string.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized String srbGetResOnChoice( int catType, String logResName,
		String phyResName, String inputFlag )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjProxyOpr");
		startSRBCommand( F_GET_RESC_ON_CHOICE, 3 );

		sendArg( catType );
		sendArg( logResName );
		sendArg( inputFlag );
		flush( );

		commandStatus();
		return new String( returnBytes() );
//TODO			clOut.retval = (String ) phyResName;
	}



	/**
	 * Remove a container
	 *
	 * @param	catType	catalog type	0	MCAT
	 * @param	containerName	The name of the container.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbRmContainer( int catType, String containerName,
		boolean force )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRmContainer");
		float version = SRBAccount.getVersionNumber();
		if (version <= 3)
		{
			startSRBCommand( F_REMOVE_CONTAINER, 2 );

			sendArg( catType );
			sendArg( containerName );
		}
		else if (force) {
			startSRBCommand( F_REMOVE_CONTAINER, 3 );

			sendArg( catType );
			sendArg( containerName );
			sendArg( SRBMetaDataSet.D_DELETE_ONE );
		}
		else {
			startSRBCommand( F_REMOVE_CONTAINER, 3 );

			sendArg( catType );
			sendArg( containerName );
			sendArg( SRBMetaDataSet.D_SU_DELETE_TRASH_ONE );
		}

		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Sync a container. Sync the permanant replica with the
	 * temporary replica.
	 *
	 * @param	catType	catalog type	0	MCAT
	 * @param	containerName	The name of the container.
	 * @param	syncFlag	valid values are:
	 *              PURGE_FLAG	purge the cache copies after sync is done.
	 *              PRIMARY_FLAG	Synchronize to the primary archival resource
	 *              only. The default is to synchronize all archival resources.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbSyncContainer( int catType, String containerName,
		int syncFlag )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbSyncContainer");
		startSRBCommand( F_SYNC_CONTAINER, 3 );

		sendArg( catType );
		sendArg( containerName );
		sendArg( syncFlag );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Replicate a container.
	 *
	 * @param	catType	catalog type	0	MCAT
	 * @param	containerName	The name of the container.
	 * @param	newResourceName	The resource for the replica.
	 * @throws IOException	If an IOException occurs
	 */
	synchronized void srbReplContainer( int catType, String containerName,
		String newResourceName )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbReplContainer");
		startSRBCommand( F_REPLICATION_CONTAINER, 3 );

		sendArg( catType );
		sendArg( containerName );
		sendArg( newResourceName );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Open a SRB object using a ticket.
	 *
	 * @param	objID	The SRB object ID to open. The objID is obtained
	 *              through registration with MDAS. objID may be NULL. In this
	 *		case, SRB will open the object associated with the ticket.
	 *              One or more conditions can be appended to the objID.
	 *              Each condition must be preceded by the character '&'.
	 *              Currently, only one condition is supported. i.e., COPY=n
	 *              (where n = replica number beginning with 0).
	 *              e.g. foo&COPY=1 specifies opening replica number
	 *              1 of data object "foo".
	 * @param	oflag	Unix type open flag. O_CREAT is not supported.
	 * @param	collectionName	The name of the collection this objID
	 *              belongs.
	 * @param	ticket	The ticket for the object.
	 * @throws IOException	If an IOException occurs
	 */
/*TODO
	synchronized void srbObjOpenWithTicket( String objID,	int oflag,
		String collectionName, String ticket )
		throws IOException
	{

			int fd;
			char objBuf[SRBFileSystem.MAX_TOKEN];

			// Compose the objID

			if (objID == NULL)
				objID = "";
			strcpy (objBuf, objID);
			strcat (objBuf, "&TICKET='");
			strcat (objBuf, ticket);
			strcat (objBuf, "'");

			fd = srbObjOpen(conn, objBuf, oflag, collectionName);

			return (fd);
	}
*/






//--------------------------------------------------------------------
// SRB 2.0 functions
//--------------------------------------------------------------------
	/* Not completed/tested from SRB2.0.0, clStub.c methods:

	 * srbTapelibMntCart - Mount a tape Cart.

	 * srbTapelibDismntCart - Dismount a tape Cart.

	 * srbGetTapeCartPri - Get the priorities for each cartType depending on the
	 * availability of tape drives.

	 * srbDumpFileList - Dump a list of files to tape. This is a priviledged
	 * function.

	 * srbStageCompObj - Stage a compound Object to cache.

	 * srbRegInternalCompObj - register an internal compound obj..

	 * Unregister an internal compound obj..

	 * Unregister a compound obj..

	 * srbModInternalCompObj - Modify a SRB internal comp obj

	 * Modify/create/delete a SRB resource

	 * Register location information

	 * srbIngestToken - Ingest Token

	 * Register Resource

	 * Register Logical Resource

	 * srbRegisterReplicateResourceInfo

	 * Delete a Value; a single MCAT entry
	 */

	/**
	 * srbTapelibMntCart - Mount a tape Cart.
	 *
	 * Input -
	 *         String tapeNum - The tape num (char string) to mount.
	 *         int priority - The priority of the mount operation.
	 *
	 *
	 * Output - Returns status of mount. If successful, status == 0. The device
	 *          path of the device where the tape cart is mounted is returned in
	 *          *outDevPath.
	 *          Returns a negative status upon failure.
	 */
	synchronized String[] srbTapelibMntCart( String tapeNum, int priority )
		throws IOException
	{
		//, String[] outDevPath )
		if (DEBUG > 0) System.out.println("\n srbTapelibMntCart");
		startSRBCommand( F_TAPELIB_MNT_CART, 3 );

		sendArg( tapeNum );
//			sendArg( getpid() ); //TODO what is that?
		sendArg( priority );
		flush( );

		commandStatus();
		int status = returnInt(); //TODO string array actually
		if (status < 0) {
			throw new SRBException( status );
		}

/*
		if (retval < 0) {
				*outDevPath = NULL;
		} else {
				// get the devPath
				retval = getByteStreamFromSvr (conn, outDevPath, &resultLen);
		}

		return retval;
*/
	return null;
	}

	/**
	 * srbTapelibDismntCart - Dismount a tape Cart.
	 *
	 * Input -
	 *         String tapeNum - The tape num (char string) to mount.
	 */
	synchronized void srbTapelibDismntCart( String tapeNum )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbTapelibDismntCart");
		startSRBCommand( F_TAPELIB_DISMNT_CART, 2 );

		sendArg( tapeNum );
//			sendArg( getpid() ); //TODO what is that?
		flush( );

		commandStatus();
		int status = returnInt(); //TODO string array actually
		if (status < 0) {
			throw new SRBException( status );
		}
	}

	/**
	 * srbGetTapeCartPri - Get the priorities for each cartType depending on the
	 * availability of tape drives.
	 *
	 * Input -
	 *         tapeCartPri_t **outTapeCartPri - The output cartType arrary
	 *         specifies the cartType sorted in decending order. i.e.
	 *         cartType[0] is the cartType with the highest priority.
	 *
	 */
	/*TODO what is going on?
	synchronized void srbGetTapeCartPri( tapeCartPri_t **outTapeCartPri )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbGetTapeCartPri");
		startSRBCommand( F_TAPELIB_GET_CART_PRI, 9? );

		sendArg( tapeLibInx );
		sendArg( myBS );
		sendArg( purgeFlag );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}



		clArgBlock argv[9];
		srbResult *res;
		int resultLen;
		String buf;
		int status, retVal;

		buf = malloc (HUGE_STRING);

		argv[0].isint = 1;
		argv[0].len = 4;
		argv[0].u.ptr = (char*) NULL;

		*outTapeCartPri = NULL;

		res = clFunct(conn, F_TAPELIB_GET_CART_PRI, buf,&resultLen,0,argv,0);
		if ((status = clResultStatus(res)) != CLI_COMMAND_OK) {
				free (buf);
				clClear(res);
				return status;
		}

		clClear(res);

		if (resultLen == 0) {
				free (buf);
				return CLI_NO_ANSWER;
		}

		retVal = unpackMsg (buf, (String *) outTapeCartPri,
		 tapeCartPri_PF, NULL);

		free (buf);

		if (retVal < 0) {
				fprintf (stderr,
				 "srbGetTapeCartPri: unpackMsg() error, status = %d",
					retVal);
				return (retVal);
		}

		return (0);
	}
	*/

	/**
	 * srbDumpFileList - Dump a list of files to tape. This is a priviledged
	 * function.
	 *
	 * Input -
	 *         tapeLibInx - The tape library index to dump the files to.
	 *         fileList - list of files to dump. - files to be dumped are
	 *           contained in the fileList->fileName[numFile][MAX_DATA_SIZE]
	 *         array. Note that the fileName is the physical path name of the
	 *         cache copy.
	 *         purgeFlag - 1 - purge the cache copy when done dumping.
	 *                     0 - no purge.
	 */
	/*TODO what is fileList_t?
	synchronized void srbDumpFileList( int tapeLibInx, fileList_t *fileList,
		int purgeFlag )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbDumpFileList");
		startSRBCommand( F_CMP_DUMP_FILE_LIST, 3 );

		sendArg( tapeLibInx );
		sendArg( myBS );
		sendArg( purgeFlag );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}


		int retval;
		srbResult *res;
		clArgBlock argv[3];
		struct varlena *packedResult;
		byteStream myBS;
		int resultLen;


		retval = packMsg ((String ) fileList, &packedResult, fileList_t_PF,
		 NULL);

		if (retval < 0) {
				fprintf (stderr, "packMsg error, status = %d\n", retval);
				return retval;
		}

		myBS.len = VARSIZE (packedResult) - VARHDRSZ;
		myBS.bytes = VARDATA(packedResult);

		argv[0].isint = 1;
		argv[0].len = 4;
		argv[0].u.integer = tapeLibInx;

		argv[1].isint = 0;
		argv[1].len = myBS.len;
		argv[1].u.ptr = (String ) myBS.bytes;

		argv[2].isint = 1;
		argv[2].len = 4;
		argv[2].u.integer = purgeFlag;

		res = clFunct (conn, F_CMP_DUMP_FILE_LIST, (String ) &retval, &resultLen, 1,
		 argv, 3);

		free (packedResult);
		if (clResultStatus(res) != CLI_COMMAND_OK) {
				retval = clResultStatus(res);
		}
		clClear(res);
		return retval;
	}
	*/

	/**
	 * srbStageCompObj - Stage a compound Object to cache.
	 *
	 * Input -
	 *         String objID - The name Of the compound obj
	 *         String collectionName - The collection
	 *         String resourceLoc - The resc location of the compound obj.
	 *         String resourceName - The resource of the compound obj.
	 *         srb_long_t offset - The offset to be staged
	 *         srb_long_t size - The size to be staged.
	 *         int replNum - The replication number of the compound obj.
	 *         int segNum - The segment number of the compound obj.
	 *         int stageFlag - if stageFlag <= 0, just create a zero length
	 *             file in cache, register and  close it.
	 *         struct mdasInfoOut **outCacheInfoOut - The output struct that
	 *             contains info for the cache copy.
	 */
	/*TODO what is struct mdasInfoOut
	synchronized void srbStageCompObj( String objID, String collectionName,
		String resourceLoc, String resourceName,
		srb_long_t offset, srb_long_t size,
		int replNum, int segNum, int stageFlag,
		struct mdasInfoOut **outCacheInfoOut )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbStageCompObj");
		startSRBCommand( F_CMP_STAGE_COMP_OBJ, 9 );

		sendArg( tapeLibInx );
		sendArg( myBS );
		sendArg( purgeFlag );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}


		clArgBlock argv[9];
		srbResult *res;
		int resultLen;
		String buf;
		int status, retVal;

		buf = malloc (HUGE_STRING);

		argv[0].isint = 0;
		if (objID == NULL)
				objID = "";
		argv[0].len = strlen (objID) + 1;
		argv[0].u.ptr = (String ) objID;

		argv[1].isint = 0;
		if (collectionName == NULL)
				collectionName = "";
		argv[1].len = strlen (collectionName) + 1;
		argv[1].u.ptr = (String ) collectionName;

		argv[2].isint = 0;
		if (resourceLoc == NULL)
				resourceLoc = "";
		argv[2].len = strlen (resourceLoc) + 1;
		argv[2].u.ptr = (String ) resourceLoc;

		argv[3].isint = 0;
		if (resourceName == NULL)
				resourceName = "";
		argv[3].len = strlen (resourceName) + 1;
		argv[3].u.ptr = (String ) resourceName;

		argv[4].isint = 1;
		argv[4].len = 2*SIZEOF32;
		argv[4].u.ptr = (char*) &offset;

		argv[5].isint = 1;
		argv[5].len = 2*SIZEOF32;
		argv[5].u.ptr = (char*) &size;

		argv[6].isint = 1;
		argv[6].len = 4;
		argv[6].u.integer = replNum;

		argv[7].isint = 1;
		argv[7].len = 4;
		argv[7].u.integer = segNum;

		argv[8].isint = 1;
		argv[8].len = 4;
		argv[8].u.integer = stageFlag;

		*outCacheInfoOut = NULL;
		res = clFunct(conn, F_CMP_STAGE_COMP_OBJ, buf,&resultLen,0,argv,9);
		if ((status = clResultStatus(res)) != CLI_COMMAND_OK) {
				free (buf);
				clClear(res);
				return status;
		}

		clClear(res);

		if (resultLen == 0) {
				free (buf);
				return CLI_NO_ANSWER;
		}

		retVal = unpackMsg (buf, (String *) outCacheInfoOut,
		 mdasInfoOut_PF, NULL);

		if (retVal < 0) {
				fprintf (stderr,
				 "srbStageCompObj: unpackMsg() of srbStageCompObj error, status = %d",
					retVal);
				free (buf);
				return (retVal);
		}

		(*outCacheInfoOut)->next = NULL;
		(*outCacheInfoOut)->compoundLink = NULL;
		(*outCacheInfoOut)->tapeFileInfo = NULL;

		free (buf);

		return (0);
	}
	*/

	/**
	 * Register an internal compound obj.
	 *
	 * @param objID	The name of the compound obj
	 * @param collectionName	The collection
	 * @param objReplNum	The replication number of the compound obj
	 * @param objSegNum	The segment number of the compound obj
	 * @param intObjRescName	The resource where the internal compound obj
	 *		is located.
	 * @param dataPathName	The physical path of the int comp obj.
	 * @param dataSize	The size of the int comp obj.
	 * @param offset	The offset of the int comp obj.
	 * @param inpIntReplNum	The replication number of the int compound obj.
	 * @param intSegNum	The segment number of the int compound obj.
	 * @param objTypeInx	In case of failure, if objTypeInx >=0, the
	 * 		int comp obj will be removed. It is the object type index
	 *		used to do the unlinking.
	 * @param phyResLoc	Valid only when objTypeInx >=0. This is the
	 *		resouce location used to do the unlinking.
	 */
	synchronized void srbRegInternalCompObj( String objName, String objCollName,
		int objReplNum, int objSegNum, String intObjRescName, String dataPathName,
		long dataSize, long offset, int inpIntReplNum, int intSegNum,
		int objTypeInx, String phyResLoc )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRegInternalCompObj");
		startSRBCommand( F_CMP_REG_INT_COMP_OBJ, 12 );

		sendArg( objName );
		sendArg( objCollName );
		sendArg( objReplNum );
		sendArg( objSegNum );
		sendArg( intObjRescName );
		sendArg( dataPathName );
		sendArg( dataSize );
		sendArg( offset );
		sendArg( inpIntReplNum );
		sendArg( intSegNum );
		sendArg( objTypeInx );
		sendArg( phyResLoc );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}

	/**
	 * Unregister an internal compound obj.
	 *
	 * @param objID	The name of the compound obj
	 * @param collectionName	The collection
	 * @param objReplNum	The replication number of the compound obj
	 * @param objSegNum	The segment number of the compound obj
	 * @param inpIntReplNum	The replication number of the int compound obj.
	 * @param intSegNum	The segment number of the int compound obj.
	 */
	synchronized void srbRmIntCompObj( String objName, String objCollName,
		int objReplNum, int objSegNum, int inpIntReplNum, int intSegNum )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRmIntCompObj");
		startSRBCommand( F_CMP_STAGE_COMP_OBJ, 6 );

		sendArg( objName );
		sendArg( objCollName );
		sendArg( objReplNum );
		sendArg( objSegNum );
		sendArg( inpIntReplNum );
		sendArg( intSegNum );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}

	/**
	 * Unregister a compound obj.
	 *
	 * Input -
	 *         String objID - The name of the compound obj
	 *         String collectionName - The collection
	 *         int objReplNum - The replication number of the compound obj
	 *         int objSegNum - The segment number of the compound obj.
	 */
	synchronized void srbRmCompObj( String objName, String objCollName,
		int objReplNum, int objSegNum )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRmCompObj");
		startSRBCommand( F_CMP_RM_COMP_OBJ, 4 );

		sendArg( objName );
		sendArg( objCollName );
		sendArg( objReplNum );
		sendArg( objSegNum );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}

	/**
	 * Modify a SRB internal comp obj.
	 *
	 * Input -
	 *         int catType - catalog type. e,g., MDAS_CATALOG.
	 *         String objID - The SRB object ID to modify. The objID must
	 *              already have been registered with the MDAS catalog.
	 *         int objReplNum - The replication number of the compound obj
	 *         int objSegNum - The segment number of the compound obj
	 *         int inpIntReplNum - The replication number of the int compound obj.
	 *         int intSegNum - The segment number of the int compound obj.
	 *         String dataValue1 - Input value 1.
	 *         String dataValue2 - Input value 2.
	 *         String dataValue3 - Input value 3.
	 *         String dataValue4 - Input value 4.
	 *         int actionType - The type of retraction. See srbC_mdas_externs.h
	 *                              for the actionType definition.
	 */
	synchronized void srbModInternalCompObj( String objID, String collectionName,
		int   objReplNum, int objSegNum, int inpIntReplNum, int intSegNum,
		String data_value_1, String data_value_2,
		String data_value_3, String data_value_4, int retraction_type )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbModInternalCompObj");
		startSRBCommand( F_CMP_MOD_INT_COMP_OBJ, 11 );

		sendArg( objID );
		sendArg( collectionName );
		sendArg( objReplNum );
		sendArg( objSegNum );
		sendArg( inpIntReplNum );
		sendArg( intSegNum );
		sendArg( data_value_1 );
		sendArg( data_value_2 );
		sendArg( data_value_3 );
		sendArg( data_value_4 );
		sendArg( retraction_type );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}

	/**
	 * Modify/create/delete a SRB resource.
	 *
	 * Input -
	 *         int catType - catalog type. e,g., MDAS_CATALOG.
	 *         String resourceName - The storage resource name.
	 *              e.g. "mda18-unix-sdsc"
	 *         int actionType - The type of retraction. See srbC_mdas_externs.h
	 *                            for the actionType definition.
	 *                            For tapes, valid values are T_INSERT_TAPE_INFO,
	 *                            T_UPDATE_TAPE_INFO, T_UPDATE_TAPE_INFO_2,
	 *                            T_DELETE_TAPE_INFO.
	 *         String dataValue1 - Input value 1.
	 *         String dataValue2 - Input value 2.
	 *         String dataValue3 - Input value 3.
	 *         String dataValue4 - Input value 4.
	 */
	synchronized void srbModifyRescInfo( int catType, String resourceName,
		int actionType,	String dataValue1, String dataValue2, String dataValue3,
		String dataValue4 )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbModifyRescInfo");
		startSRBCommand( F_MOD_RESC_INFO, 7 );

		sendArg( catType );
		sendArg( resourceName );
		sendArg( actionType );
		sendArg( dataValue1 );
		sendArg( dataValue2 );
		sendArg( dataValue3 );
		sendArg( dataValue4 );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}

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
	synchronized void srbRegisterLocation( String locName, String fullAddr,
		String parentLoc, String serverUser, String serverUserDomain )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRegisterLocation");
		startSRBCommand( F_REGISTER_LOCATION, 5 );

		sendArg( locName );
		sendArg( fullAddr );
		sendArg( parentLoc );
		sendArg( serverUser );
		sendArg( serverUserDomain );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}

	/**
	 * Ingest Token.
	 *
	 * Input -
	 *	   String typeName - The type name.
	 *	   String newValue - The new value.
	 *         String parentValue - Parent value.
	 */
	synchronized void srbIngestToken( String typeName, String newValue,
		String parentValue )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbIngestToken");
		startSRBCommand( F_INGEST_TOKEN, 3 );

		sendArg( typeName );
		sendArg( newValue );
		sendArg( parentValue );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}

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
	synchronized void srbRegisterResource( String rescName, String rescType,
		String location, String phyPath, String className, int size )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRegisterResource");
		startSRBCommand( F_REGISTER_RESOURCE, 6 );

		sendArg( rescName );
		sendArg( rescType );
		sendArg( location );
		sendArg( phyPath );
		sendArg( className );
		sendArg( size );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}

	/**
	 * Register Logical Resource.
	 *
	 * Input -
	 *	   String rescName - The resource name.
	 *	   String rescType - Resource type.
	 *         String phyResc -  Physical resource.
	 *         String phyPath -  Physical path.
	 */
	synchronized void srbRegisterLogicalResource( String rescName,
		String rescType, String phyResc, String phyPath )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRegisterLogicalResource");
		startSRBCommand( F_REGISTER_LOGICAL_RESOURCE, 4 );

		sendArg( rescName );
		sendArg( rescType );
		sendArg( phyResc );
		sendArg( phyPath );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}

	/**
	 * srbRegisterReplicateResourceInfo
	 *
	 * Input -
	 *	   String physicalRescName - The physical resource name.
	 *	   String rescType - Resource type.
	 *         String oldLogicalRescName - old logical resource name.
	 *         String indefaultPath -  Indefault Path.
	 */
	synchronized void srbRegisterReplicateResourceInfo(
		String physicalRescName,
		String rescType,
		String oldLogicalRescName,
		String indefaultPath )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbRegisterReplicateResourceInfo");
		startSRBCommand( F_REGISTER_REPLICATE_RESOURCE_INFO, 4 );

		sendArg( physicalRescName );
		sendArg( rescType );
		sendArg( oldLogicalRescName );
		sendArg( indefaultPath );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}

	/**
	 * Delete a Value; a single MCAT entry.
	 *
	 * Input -
	 *	   int valueType - the value (token) type.
	 *	   String deleteValue - The value (name) that is being deleted.
	 */
	synchronized void srbDeleteValue( int valueType, String deleteValue )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbDeleteValue");
		startSRBCommand( F_DELETE_VALUE, 2 );

		sendArg( valueType );
		sendArg( deleteValue );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}



	/**
	 * Register files in bulk.
	 *
	 * @param catType  catalog type - 0 - MCAT
	 * @param bulkLoadFilePath  The path where the files will be loaded,
	 *		a container or temporary file.
	 *
	 * @param recordList the metadata describing the objects to be registered.
	 *		for transfer it will be converted into:
	 *         mdasC_sql_result_struct  *myresult - The mdasC_sql_result_struct
	 *	   that contains the objects to be registered.
	 * myresult->sqlresult[0].values   should contain dataNameList
	 * myresult->sqlresult[1].values   should contain collectionNameList
	 * myresult->sqlresult[2].values   should contain dataSizeList (in ascii)
	 *                                  (I will perform atol)
	 * myresult->sqlresult[3].values   should contain offSetList (in ascii)
	 * myresult->row_count             should contain the number of datsets to
	 *                                     be registered.
	 */
	synchronized void srbBulkRegister( int catType, String containerName,
		SRBMetaDataRecordList[] rl )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbBulkRegister");
		startSRBCommand( F_BULK_REGISTER, 3 );

		sendArg( catType );
		sendArg( containerName );
		sendArg( rl );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
		status();
	}



	/**
	 * Proxy Operation that executes a command.
	 *
	 * @param command	The command to run.
	 * @param commandArgs	The command argument string.
	 * @param hostAddress	The host address where this proxy operation
	 *                      should be performed. NULL => the server for the
	 *                      current connect.
	 * @param portalFlag    The portal flag (see definition in srb.h).
	 *			  	Valid flags are - PORTAL_OFF, PORTAL_ON, PORTAL_STD_IN_OUT.
	 *
	 * @return any byte stream output.
	 * @throws IOException	If an IOException occurs.
	 */
	synchronized InputStream srbExecCommand( String command, String commandArgs,
		String hostAddress, int portalFlag,
		int fireWallMinPort, int fireWallMaxPort )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbExecCommand");
		//the output of the proxy command is received on a different port
		//open a new ServerSocket to listen on
		ServerSocket serverSocket = new ServerSocket();

		if (fireWallMinPort >= 0) {
			do {
				if (fireWallMinPort > fireWallMaxPort) {
					throw new IOException(
						"The bind operation failed, all ports already bound.");
				}
				try {
					serverSocket = new ServerSocket(fireWallMinPort);
				} catch (IOException e) {
					//I should find out how to avoid this
				}
				fireWallMinPort++;
			} while (!serverSocket.isBound());
		}
		else {
			//use any free port
			serverSocket = new ServerSocket(0);
		}
		int localPort = serverSocket.getLocalPort();

		String localAddress = srbConnection.getLocalAddress().getHostAddress();
		Socket receiveSocket = null;
		DataInputStream receiveIn = null;


		startSRBCommand( F_SRBO_PROXY_OPR, 12 );

		sendArg( OPR_COMMAND );
		sendArg( portalFlag );
		sendArg( localPort );
		sendArg( 0 );
		sendArg( 0 );
		sendArg( command );
		sendArg( commandArgs );
		sendArg( hostAddress );
		sendArg( localAddress );
		sendArg( "" );
		sendArg( "" );
		sendArg( "" );
		flush( );

		receiveSocket = serverSocket.accept();
		receiveIn = new DataInputStream(receiveSocket.getInputStream());

		commandStatus();
		returnInt(); //just checks for errors.
		return receiveIn;
	}


	/**
	 * Proxy Operation that executes a command.
	 * Only valid for version 3.x.x and above.
	 * Method will not work for version 2.x.x, or before.
	 *
	 * @param command       The command to run.
	 * @param commandArgs   The command argument string.
	 * @param hostAddress   The host address where this proxy operation
	 *                      should be performed. NULL => the server for the
	 *                      current connect.
	 * @param fileName      The SRB path to a file to perform proxy operation on.
	 * @param portalFlag    The portal flag (see definition in srb.h).
	 *          Valid flags are - PORTAL_OFF, PORTAL_ON, PORTAL_STD_IN_OUT.
	 *
	 * @return any byte stream output.
	 * @throws IOException  If an IOException occurs.
	 */
	synchronized InputStream srbExecCommand( String command, String commandArgs,
		String hostAddress, String fileName, int portalFlag,
		int fireWallMinPort, int fireWallMaxPort )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbExecCommand");
		//the output of the proxy command is received on a different port
		//open a new ServerSocket to listen on
		ServerSocket serverSocket = new ServerSocket();

		if (fireWallMinPort >= 0) {
			do {
				if (fireWallMinPort > fireWallMaxPort) {
					throw new IOException(
						"The bind operation failed, all ports already bound.");
				}
				try {
					serverSocket = new ServerSocket(fireWallMinPort);
				} catch (IOException e) {
					//I should find out how to avoid this
				}
				fireWallMinPort++;
			} while (!serverSocket.isBound());
		}
		else {
			//use any free port
			serverSocket = new ServerSocket(0);
		}
		int localPort = serverSocket.getLocalPort();
		int resolvePath = 1;

		String localAddress = srbConnection.getLocalAddress().getHostAddress();
		Socket receiveSocket = null;
		DataInputStream receiveIn = null;


		startSRBCommand( F_SRBO_PROXY_OPR, 12 );

		sendArg( OPR_COMMAND );
		sendArg( portalFlag );
		sendArg( localPort );
		sendArg( resolvePath );
		sendArg( 0 );
		sendArg( command );
		sendArg( commandArgs );
		sendArg( hostAddress );
		sendArg( localAddress );
		sendArg( fileName );
		sendArg( "" );
		sendArg( "" );
		flush( );

		receiveSocket = serverSocket.accept();
		receiveIn = new DataInputStream(receiveSocket.getInputStream());

		commandStatus();
		returnInt(); //just checks for errors.
		return receiveIn;
	}


	/**
	 * Sync the permanant replica with the temporary replica.
	 *
	 * @param catType catalog type - 0 - MCAT
	 * @param objID The SRB object ID to Sync.
	 * @param collectionName The name of the collection this objID belongs.
	 * @param resource The resource for the object to sync to.
	 *        	A null or empty string means synchronize existing copies.
	 */
	synchronized void srbSyncData( int catType, String objID,
		String collectionName, String resource )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbSyncData");
		startSRBCommand( F_SRBO_SYNC_DATA, 4 );

		sendArg( catType );
		sendArg( objID );
		sendArg( collectionName );
		sendArg( resource );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}


	/**
	 * Open a container for ingestion.
	 *
	 * @param catType - catalog type - 0 - MCAT
	 * @param containerName - The name of the container.
	 * @param openFlag - O_RDWR, O_WRONLY or O_RDONLY.
	 * @return The file descriptor for the container.
	 */
	synchronized int srbContainerOpen( int catType, String containerName,
		int openFlag )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbContainerOpen");
		startSRBCommand( F_CONTAINER_OPEN, 3 );

		sendArg( catType );
		sendArg( containerName );
		sendArg( openFlag );
		flush( );

		commandStatus();
		return returnInt();
	}


	/**
	 * Close an opened a container.
	 *
	 * @param catType - catalog type - 0 - MCAT
	 * @param confFd - The fd returned from srbContainerOpen ().
	 */
	synchronized void srbContainerClose ( int confFd )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbContainerClose");
		startSRBCommand( F_CONTAINER_CLOSE, 1 );

		sendArg( confFd );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}


	/**
	 * Copy a dataset.
	 *
	 * @param srcObjID - The source object.
	 * @param srcCollection - The source collection.
	 * @param destObjID - The destination object.
	 * @param destCollection - The destination collection.
	 * @param destResource - The resource to put the destination object.
	 * @return the number of bytes copied.
	 *          Returns a negative value upon failure.
	 */
	synchronized long srbObjCopy( String srcObjID, String srcCollection,
		String destObjID, String destCollection, String destResource )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjCopy");
		startSRBCommand( F_SRBO_COPY, 5 );

		sendArg( srcObjID );
		sendArg( srcCollection );
		sendArg( destObjID );
		sendArg( destCollection );
		sendArg( destResource );
		flush( );

		commandStatus();
		return returnLong();
	}


//--------------------------------------------------------------------
// Parallel IO
//--------------------------------------------------------------------
	/**
	 * Copy a dataset from local space to SRB space.
	 *
	 * @param destObjID - The destination objID.
	 * @param destCollection - The destination collection.
	 * @param destResLoc - The destination resource.
	 * @param dataType - The data type.
	 * @param destPath - The destination path name.
	 * @param locFilePath - The local fullPath name.
	 * @param size - The size of the file. negative means don't know.
	 * @return the number of bytes copied.
	 */
	synchronized long srbObjPut( String destObjID, String destCollection,
		String destResLoc, String dataType, String destPath, String localFilePath,
		long srcSize,	int fireWallMinPort, int fireWallMaxPort )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjPut "+destObjID+" "+
			destCollection);
/*
if (true) {
	srbObjPutClientInitiated( destObjID, destCollection,
		destResLoc, dataType, destPath, localFilePath,	srcSize, 0, 4 );

	System.out.println("\n\nback in srbObjPut");
	int x=-1;
	byte[] asdf = read(1);
	while(x<0) {
		System.out.print(asdf[0] + "-"+(char)asdf[0]+"  ");
		asdf = read(1);
	}
}
*/
		totalBytesMoved = 0;
		bytesMoved = 0;

		//if files with zero bytes are copied the method will hang
		if (srcSize <= 0) {
			srbObjCreate( 0, destObjID, dataType, destResLoc, destCollection,
				"", 0);
			return 0;
		}


		//Authentication cookie
		short cookie = (short) (Math.random()*32767); //positive numbers only?
		String mode = "r";
		LocalRandomAccessFile localRAF =
			new LocalRandomAccessFile(localFilePath, mode);

		//open a new ServerSocket to listen on
//		ServerSocket sSocket = null;
ServerSocket sSocket = new ServerSocket();
/*
System.out.println("sSocket whatever");
System.out.println("sSocket "+sSocket);
System.out.println("getChannel()  "+sSocket.getChannel() );
System.out.println("getInetAddress() "+sSocket.getInetAddress());
System.out.println("getLocalPort() "+sSocket.getLocalPort());
System.out.println("getLocalSocketAddress() "+sSocket.getLocalSocketAddress());
System.out.println("getReceiveBufferSize() "+sSocket.getReceiveBufferSize());
System.out.println("getReuseAddress() "+sSocket.getReuseAddress());
System.out.println("getSoTimeout() "+sSocket.getSoTimeout());
sSocket.setReuseAddress(true);
System.out.println("sSocket "+sSocket);
System.out.println("getChannel()  "+sSocket.getChannel() );
System.out.println("getInetAddress() "+sSocket.getInetAddress());
System.out.println("getLocalPort() "+sSocket.getLocalPort());
System.out.println("getLocalSocketAddress() "+sSocket.getLocalSocketAddress());
System.out.println("getReceiveBufferSize() "+sSocket.getReceiveBufferSize());
System.out.println("getReuseAddress() "+sSocket.getReuseAddress());
System.out.println("getSoTimeout() "+sSocket.getSoTimeout());
sSocket.setReceiveBufferSize(64000);
System.out.println("sSocket "+sSocket);
System.out.println("getChannel()  "+sSocket.getChannel() );
System.out.println("getInetAddress() "+sSocket.getInetAddress());
System.out.println("getLocalPort() "+sSocket.getLocalPort());
System.out.println("getLocalSocketAddress() "+sSocket.getLocalSocketAddress());
System.out.println("getReceiveBufferSize() "+sSocket.getReceiveBufferSize());
System.out.println("getReuseAddress() "+sSocket.getReuseAddress());
System.out.println("getSoTimeout() "+sSocket.getSoTimeout());
sSocket.bind(new InetSocketAddress(0));
System.out.println("sSocket "+sSocket);
System.out.println("getChannel()  "+sSocket.getChannel() );
System.out.println("getInetAddress() "+sSocket.getInetAddress());
System.out.println("getLocalPort() "+sSocket.getLocalPort());
System.out.println("getLocalSocketAddress() "+sSocket.getLocalSocketAddress());
System.out.println("getReceiveBufferSize() "+sSocket.getReceiveBufferSize());
System.out.println("getReuseAddress() "+sSocket.getReuseAddress());
System.out.println("getSoTimeout() "+sSocket.getSoTimeout());
*/


		if (fireWallMinPort >= 0) {
			do {
				if (fireWallMinPort > fireWallMaxPort) {
					throw new IOException(
						"The bind operation failed, all ports already bound.");
				}
				try {
					sSocket = new ServerSocket(fireWallMinPort);
				} catch (IOException e) {
					//I should find out how to avoid this
				}
				fireWallMinPort++;
			} while (!sSocket.isBound());
		}
		else {
			//use any free port
			sSocket = new ServerSocket(0);
		}

		int port = sSocket.getLocalPort();


		//Manages new threads the server initiates
		ParallelManager manager =
			new ParallelManager(sSocket, cookie, localRAF, srcSize, mode);
		Thread managerThread = new Thread(manager);
		managerThread.start();

if (SRBAccount.getVersionNumber() < 3) {
		//protocol changed from SRB2 to 3.
		startSRBCommand( F_SRBO_PUT, 8 );

		sendArg( destObjID );
		sendArg( destCollection );
		sendArg( destResLoc );
		sendArg( srbConnection.getLocalAddress().getHostAddress() );
		sendArg( ((cookie << 16) & 0x7fff0000) | port );
		sendArg( dataType );
		sendArg( destPath );
		sendArg( srcSize );
}
else {
		startSRBCommand( F_SRBO_PUT, 9 );

		sendArg( destObjID );
		sendArg( destCollection );
		sendArg( destResLoc );
		sendArg( srbConnection.getLocalAddress().getHostAddress() );
		sendArg( ((cookie << 16) & 0x7fff0000) | port );
		sendArg( dataType );
		sendArg( destPath );
		sendArg( srcSize );
		sendArg( 1 ); //force overwrite flag
//TODO can append now?
}
		flush( );

		commandStatus();
//TODO is this correct? or is it int msg length, int status
		long value = returnLong();
//System.out.println("join? "+value);
		if (value >= 0) {
			try {
				if (managerThread.isAlive())
					managerThread.join();
			}
			catch(InterruptedException e) {

			}
		}
//System.out.println("freakin join? "+value);
		localRAF.close();

		if (value < 0) {
			try {
				byte[] bytes = new byte[8];
				Host.copyInt( value, bytes );
				value = Host.castToUnsignedInt( bytes );
			} catch ( NullPointerException e ) {
				//TODO can happen in Host.copyInt?
			}

			throw new SRBException("",(int) value);
		}

		return value;
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
	synchronized long srbObjPutClientInitiated( String destObjID,
		String destCollection, String destResLoc, String dataType, String destPath,
		String localFilePath,	long srcSize, int forceFlag, int numThreads )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjPutClientInitiated");
int x=-1;
		//if files with zero bytes are copied the method might hang
		if (srcSize <= 0) {
			srbObjCreate( 0, destObjID, dataType, destResLoc, destCollection,
				"", 0);
			return 0;
		}

GeneralRandomAccessFile raf = new LocalRandomAccessFile( "lkjh", "rw" );


		startSRBCommand( F_SRBO_PUT_C, 8 );

		sendArg( destObjID );
		sendArg( destCollection );
		sendArg( destResLoc );
		sendArg( dataType );
		sendArg( destPath );
		sendArg( srcSize );
		sendArg( forceFlag );
		sendArg( numThreads );
		flush( );


//b objParaIoFunct.c:3421
//b objParaIoFunct.c:1813
//b objParaIoFunct.c:3265
		commandStatus();
		//returnedByteCount or error message
		int returnedByteCount = readInt();
//System.out.println("returnedByteCount "+returnedByteCount);
		if (returnedByteCount < 0) {
			throw new SRBException( returnedByteCount);
		}

		//new portlist number or ?
		long newPortList = readUnsignedInt();
//System.out.println("newPortList "+newPortList);

//		while (newPortList == NEW_PORTLIST)
if (newPortList == NEW_PORTLIST)
		{
			int numberOfAddresses = (int) readUnsignedInt();
//System.out.println("numberOfAddresses "+numberOfAddresses);
			int[] portNum = new int[numberOfAddresses];
			int[] cookie = new int[numberOfAddresses];
			String[] hostAddr = new String[numberOfAddresses];
byte[] cookiePort = null;
			for (int i=0;i<numberOfAddresses;i++) {
				cookiePort = read(4);
				cookie[i] = ((cookiePort[0] & 0xff) << 8) + (cookiePort[1]& 0xff);
				portNum[i] = ((cookiePort[2] & 0xff) << 8) + (cookiePort[3]& 0xff);
			}

			for (int i=0;i<numberOfAddresses;i++) {
				hostAddr[i] = readString();
//System.out.println("hostAddr "+hostAddr[i]);
			}

//System.out.println("cookie "+cookie[0]);
//System.out.println("portNum "+portNum[0]);
	    status();

//b objParaIoFunct.c:2751

//must be sent all at once.
byte[] ddd = { 0, 0, cookiePort[0], cookiePort[1] };

//er temp, but good way
				//send cookie
				out.write(ddd);
//System.out.println("after cookie check main");

//b objParaIoFunct.c:3264
Socket[] transferSocket = new Socket[4];
for (int i=0;i<4;i++) {
	//Thread[] transferThreads = new Thread[MAX_THREADS+1];
	transferSocket[i] = new Socket( hostAddr[0], portNum[0] );
	InputStream inny = transferSocket[i].getInputStream();
	OutputStream outy = transferSocket[i].getOutputStream();
//System.out.println("outy "+i);

	outy.write(ddd);
}
//System.out.println("after cookie second");

//doesn't work unless you step to there?
//b objParaIoFunct.c:2784
/*
Socket transferSocket2 = new Socket( hostAddr[0], portNum[i] );
OutputStream out2 = transferSocket.getOutputStream();
write(0);
write(0);
write(cookiePort[0]);
write(cookiePort[1]);
*/

//System.out.println("after spawn2 cookie");

//b objParaIoFunct.c:3293
/*
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);
out.write(0);


//send header operation int
out.write((int)3);

//send header buffer length
out.write((int)srcSize);

//send buffer length
byte[] buffer = new byte[(int)raf.length()];
out.write(raf.read(buffer));
*/
/*
asdf = transferSocket.getInputStream().read();
System.out.println("transferSocket asdf "+asdf);
System.out.println("transferSocket "+transferSocket.isConnected());
/*while(x<0) {
System.out.print(asdf + " "+(char)asdf);
asdf = transferSocket.getInputStream().read();
}
*/
//System.out.print(asdf + " after "+(char)asdf);
/*
TransferThread transfer = new TransferThread(new Object(), transferSocket, raf, srcSize, new Object(), i);
transferThreads[i] = new Thread(transfer);
transferThreads[i].start();
*/


//System.out.println("\n\nafter thread start");
		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}


		}


/*
System.out.println("uncoded->");
x=-1;
byte[] asdf = read(1);
while(x<0) {
System.out.println(asdf[0] + " "+(char)asdf[0]);
asdf = read(1);
}
				newSock = portalConnect (myPortList->portNum[0], myPortList->hostAddr);
				if (newSock < 0) {
						(void) sprintf(conn->errorMessage,
						 "srbObjPutC() -- portalConnect() to %s failed: errno=%d\n%s\n",
							myPortList->hostAddr, errno, strerror(errno));

						res = clReadServerReturn (conn, mybuf, &resultLen, 0);

						status = clResultStatus(res);
			free (myPortList);
						clClear(res);
						continue;
				}

				// we could cast the relayHeader structure because relayHeader and
				// firstHeader has the same struct
				status = readHeader (newSock, (struct relayHeader *) &myHeader);
				if (status < 0) {
	#ifdef _WIN32
						closesocket(newSock);
	#else
						close (newSock);
	#endif
						res = clReadServerReturn (conn, mybuf, &resultLen, 0);

						status = clResultStatus(res);
			free (myPortList);
						clClear(res);
						continue;
				}


	#ifdef foo
		myInput.sock = lsock;
		myInput.cookie = getCookieFromPortInp (clPort);
	#endif
				myInput.locFilePath = locFilePath;
				myInput.operation = PUT_OPR;
				myInput.transferHandler = (void *) portalTransfer;
				myInput.size = srcSize;
				myInput.numThreads = 0;
				myInput.threadInx = 0;
				myInput.bytesMoved = 0;
				myInput.status = 0;
				myInput.errorMessage = conn->errorMessage;
				pthread_mutex_init(&myInput.varMutex, pthread_mutexattr_default);
				pthread_mutex_init(&myInput.condMutex, pthread_mutexattr_default);
				pthread_cond_init (&myInput.cond, pthread_condattr_default);

				for (i = 0; i < myHeader.numThreads; i++) {
						if (i > 0) {
								newSock = portalConnect (myPortList->portNum[0],
								 myPortList->hostAddr);
								if (newSock < 0) {
										(void) sprintf(conn->errorMessage,
										 "srbObjPutC()--portalConnect() to %s failed:errno=%d\n%s\n"
	,
											myPortList->hostAddr, errno, strerror(errno));
										break;
								}
						}
						myInput.tranInp[i].relaySocketFd = newSock;
						myInput.tranInp[i].threadInx = i;
						myInput.tranInp[i].portInput = &myInput;
						status = pthread_create(&myInput.threadId[i], pthread_attr_default,
						(void*(*)(void*)) myInput.transferHandler,
						 (void *)&myInput.tranInp[i]);
						if (status < 0) {
								fprintf (stderr,
								 "srbObjPutC: pthread_create failed. errno = %d\n", errno);
						}
				}
	free (myPortList);
				res = clReadServerReturn (conn, mybuf, &resultLen, 0);

				if ((status = clResultStatus(res)) < 0) {
						for (i = 0; i < myHeader.numThreads; i++) {
								pthread_cancel (myInput.threadId[i]);
						}
						clClear(res);
						return status;
				}

				for (i = 0; i < myHeader.numThreads; i++) {
						pthread_join (myInput.threadId[i], NULL);
				}
				conn->numThread = myHeader.numThreads;

				// read the extra status

		}

		if (myHeader.size != myInput.bytesMoved) {
				if (myInput.status < 0)
						return (myInput.status);
				else
						return (OBJ_ERR_COPY_LEN);
		}
		if (myHeader.size == 0) {
				int myFd;
				// make a 0 length file
				myFd = open (locFilePath, O_RDWR | O_CREAT, 0644);
				close (myFd);
		}
		return (myHeader.size);

	#ifdef foo
		// if myInput.bytesMoved != retval, must be multiple of retval (discovered
		// not true for HPSS mover transfer. Different mover threads could
		// request for the same offset and data)
		if (retval > 0 &&
		 (retval > myInput.bytesMoved || ((myInput.bytesMoved % retval) != 0) &&
			retval != srcSize)) {
				if (myInput.status < 0)
						return (myInput.status);
				else
						return (OBJ_ERR_COPY_LEN);
		} else if (retval == 0 && retval != myInput.bytesMoved) {
				if (myInput.status < 0)
						return (myInput.status);
				else
						return (OBJ_ERR_COPY_LEN);
		}

		return retval;
	#endif
*/
		return 0;
	}


	/**
	 * Copy a dataset from SRB space to local space.
	 *
	 * @param srcObjID The source objID.
	 * @param srcCollection The source collection.
	 * @param srcSize The source size.
	 * @param file The local file.
	 * @return The number of bytes copied.
	 *          Returns a negative value upon failure.
	 */
	synchronized long srbObjGet( String srcObjID, String srcCollection,
		long srcSize,	GeneralFile file, int fireWallMinPort, int fireWallMaxPort,
		boolean forceOverwrite )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjGet");


		totalBytesMoved = 0;
		bytesMoved = 0;

		//Authentication cookie
		short cookie = (short) (Math.random()*32767); //er, positive numbers only?
		String mode = "rw";
		GeneralRandomAccessFile raf =
			FileFactory.newRandomAccessFile(file, mode);


		//some filesystems can't truncate...
		if ((forceOverwrite) && (file.length() > srcSize)) {
			try {
				raf.setLength( srcSize );
			} catch (UnsupportedOperationException e) {
				//this should only happen to fileSystems that can't truncate
				file.delete();
				raf =	FileFactory.newRandomAccessFile(file, "rw");
			} catch (IOException e) {
				//I guess they might throw IOException instead
				file.delete();
				raf =	FileFactory.newRandomAccessFile(file, "rw");
			}
		}
		else if (!forceOverwrite) {
			//if file already existed, append
			raf.seek( raf.length() );
		}

		//open a new ServerSocket to listen on
		ServerSocket sSocket = null;

		if (fireWallMinPort >= 0) {
			do {
				if (fireWallMinPort > fireWallMaxPort) {
					throw new IOException(
						"The bind operation failed, all ports already bound.");
				}
				try {
					sSocket = new ServerSocket(fireWallMinPort);
				} catch (IOException e) {
					//I should find out how to avoid this
				}
				fireWallMinPort++;
			} while (!sSocket.isBound());
		}
		else {
			//use any free port
			sSocket = new ServerSocket(0);
		}
		int port = sSocket.getLocalPort();

		//Manages new threads the server initiates
		ParallelManager manager = new ParallelManager(sSocket, cookie, raf,
			srcSize, mode);
		Thread managerThread = new Thread(manager);
		managerThread.start();

		startSRBCommand( F_SRBO_GET, 4 );

		sendArg( srcObjID );
		sendArg( srcCollection );
		sendArg( srbConnection.getLocalAddress().getHostAddress() );
		sendArg( ((cookie << 16) & 0x7fff0000) | port );
		flush( );

		commandStatus();
//TODO is this correct? or is it int msg length, int status
		long value = returnLong();

		//value = 0 hangs on thread.join for some reason
		if (value > 0) {
			try {
				if (managerThread.isAlive()) {
					managerThread.join();
				}
			}
			catch(InterruptedException e) {

			}
			raf.close();
		}
		else if (value < 0) {
			throw new SRBException("",(int) value);
		}

		return value;
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
	 */
	synchronized long srbObjGetClientInitiated( String srcObjID,
		String srcCollection,	GeneralFile destination, int flag, int numThreads )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjGetClientInitiated");

		startSRBCommand( F_SRBO_GET_C, 4 );

		sendArg( srcObjID );
		sendArg( srcCollection );
		sendArg( flag );
		sendArg( numThreads );
		flush( );

		commandStatus();

		//length of file to be copied
		long srcSize = 1;

		//returnedByteCount or error message
		int returnedByteCount = readInt();
		if (returnedByteCount < 0) {
			throw new SRBException( returnedByteCount);
		}

		//new portlist number or error message?
		long newPortList = readUnsignedInt();

		while (newPortList == NEW_PORTLIST)
		{
			int numberOfAddresses = (int) readUnsignedInt();
			int[] portNum = new int[numberOfAddresses];
			int[] cookie = new int[numberOfAddresses];
			String[] hostAddr = new String[numberOfAddresses];
			byte[] cookiePort = null;

//System.out.println(	"\nnumberOfAddresses "+numberOfAddresses );
//System.out.println(	"numThreads "+numThreads );

			Socket[] transferSocket = new Socket[numThreads];
			GeneralRandomAccessFile raf =
				FileFactory.newRandomAccessFile(destination, "rw");
			Thread[] transferThreads = new Thread[numThreads];
			TransferThread[] transfer = new TransferThread[numThreads];

			for (int i=0;i<numberOfAddresses;i++) {
				cookiePort = read(4);
				cookie[i] = ((cookiePort[0] & 0xff) << 8) + (cookiePort[1]& 0xff);
				portNum[i] = ((cookiePort[2] & 0xff) << 8) + (cookiePort[3]& 0xff);
			}

			for (int i=0;i<numberOfAddresses;i++) {
				hostAddr[i] = readString();
			}
			status();


			//send cookie
			//must be sent all at once. server not block properly?
			byte[] cookieBytes = { 0, 0, cookiePort[0], cookiePort[1] };
			out.write(cookieBytes);


//System.out.println(	"hostAddr[0] "+hostAddr[0] );

			for (int i=0;i<numThreads;i++) {
//System.out.println(	"portNum[i]"+portNum[i] );
				transferSocket[i] = new Socket( hostAddr[0], portNum[i] );
				OutputStream outy = transferSocket[i].getOutputStream();

				//send the cookie back to verify authenticity
				outy.write(cookieBytes);

				if (i == 0) {
					DataInputStream inny =
						new DataInputStream(transferSocket[i].getInputStream());
					inny.readInt();//um?
					inny.readInt();//um? number of threads?
					srcSize = inny.readLong();
					//TODO the first thread has an extra header?
				}

				transfer[i] = new TransferThread(raf, transferSocket[i],
					raf, srcSize, new Object(), i);
				transferThreads[i] = new Thread(transfer[i]);
				transferThreads[i].start();
			}




			try {
				for (int i=0;i<numThreads;i++) {
					if (transferThreads[i].isAlive()) {
						transferThreads[i].join();
					}
				}
			}
			catch(InterruptedException e) {

			}
			raf.close();

			commandStatus();
			newPortList = readUnsignedInt();

			//never actually found a way to test this loop
		}


//TODO not sure about bytesMoved
//		if (srcSize != bytesMoved) {
			if (newPortList < 0)
				throw new SRBException( (int)newPortList );
//			else
//				throw new SRBException( -2404 );
//		}

//TODO some extra bytes...maybe an error message?
read(4);
status();


//		return bytesMoved;
//TODO obviously this isn't always exactly right
return srcSize;
	}

	/**
	 * Provides status on file transfer,
	 * in the form of how many bytes have been transfer so far.
	 */
//may not work, or may not be thread safe...
//which yes, is not good for a threaded function
	long getBytesMoved() {
		return bytesMoved;
	}


	//
	// Thread support for parallel put and get, bulk (un)load
	//
	long totalBytesMoved = 0;
	long bytesMoved = 0;
	long srcLength;
//max threads = 4, I guess
static final int MAX_THREADS = 32;
	class ParallelManager implements Runnable
	{
		private ServerSocket listener;
		private short cookie;
		private GeneralRandomAccessFile raf;
		private long srcSize;
/*		private */boolean parallelComplete = false;
		private boolean threads[] = new boolean[MAX_THREADS+1];
		private String mode;


		ParallelManager( ServerSocket listener, short cookie,
			GeneralRandomAccessFile raf, long srcSize, String mode )
		{
				this.listener=listener;
				this.cookie=cookie;
				this.raf = raf;
				this.srcSize = srcSize;
				this.mode = mode;
				bytesMoved = 0;
		}

		void threadComplete(int i) {
//System.out.println(" threadComplete "+i);
			threads[i] = false;
			for (int j=0;j<threads.length;j++) {
				if (threads[j]) {
					return;
				}
			}
//System.out.println(" parallelComplete ");

			parallelComplete = true;
		}

		// Listen for incoming connections and handle them
		public void run()
		{
			int i=0, n=0;;
			Socket transferSocket = null;
			Thread transferThread = null;
Thread[] transferThreads = new Thread[MAX_THREADS+1];


			try {
				while (!parallelComplete) {
////System.out.println(" i "+i);
					transferSocket = listener.accept();

					DataInputStream in =
						new DataInputStream(transferSocket.getInputStream());

					//Match the cookie, to insure the SRB server is the one connecting.
					int myCookie = in.readInt();
////System.out.println("myCookie "+myCookie);
					if (myCookie == cookie) {
						raf=FileFactory.newRandomAccessFile(raf.getFile(), mode);
						threads[i] = true;
						TransferThread transfer = new TransferThread(this, transferSocket,
							raf, srcSize, listener, i);
						transferThreads[i] = new Thread(transfer);
						transferThreads[i].start();
						i++;
					}
				}
//System.out.println(" after parallelComplete "+i);
			}
			catch(SocketException e) {
//TODO find a better way.
				//The serversocket is closed to by the final transfer thread
				//to notified manager of completion.
//e.printStackTrace();

				//wait for all threads to finish
//System.out.println(" wait for all threads to finish "+i);
				try {
					for (i=0;i<transferThreads.length;i++) {
						if (transferThreads[i] != null) {
							if (transferThreads[i].isAlive()) {
								transferThreads[i].join();
							}
						}
					}

//System.out.println(" after wait to finish "+i);
				}
				catch(InterruptedException inE) {
inE.printStackTrace();
				}
				return;
			}
			catch (IOException e) {
				//can't throw IOException because Runnable.run() doesn't?
				throw new RuntimeException( "IOException in thread." );
			}
//System.out.println("waiting");
			//wait for all threads to finish
			try {
				for (i=0;i<transferThreads.length;i++) {
					if (transferThreads[i] != null) {
						if (transferThreads[i].isAlive()) {
							transferThreads[i].join();
						}
					}
				}
			}
			catch(InterruptedException inE) {
inE.printStackTrace();
			}

//System.out.println("done waiting?");
		}
	}

	//TODO what are the operation cases?
	static final int READ = 1;//???
	static final int WRITE = 2;//???
	static final int PUT = 3;
	static final int GET = 4;
	static final int DONE = 0;
	class TransferThread implements Runnable
	{
    private Socket transferSocket;
		private GeneralRandomAccessFile raf;
		private long srcSize;
		private Object listener;

		private SRBMetaDataRecordList[] rl;
		private String directoryPath;
		private String sourcePath;

		private Object syncObject;
		int length;
int whichThread;
		/**
		 * Used by parallel transfer
		 */
    TransferThread( Object syncObject, Socket transferSocket,
    	GeneralRandomAccessFile raf, long srcSize, Object listener,
    	int threadID )
    {
whichThread=threadID;
      this.transferSocket = transferSocket;
      this.raf = raf;
			this.srcSize = srcSize;
			this.listener = listener;

			this.syncObject = syncObject;
    }


		/**
		 * Used by bulk unload
		 */
    TransferThread( Socket transferSocket, SRBMetaDataRecordList[] rl,
    	String targetDirectoryPath, String sourcePath )
    	throws IOException
    {
      this.transferSocket = transferSocket;
      this.rl = rl;
      this.directoryPath = targetDirectoryPath;
      this.sourcePath = sourcePath;
			srcSize = -1;
			listener = null;

      raf = correctedRAFPath(0);
			length = rl[0].getIntValue( rl[0].getFieldIndex( SRBMetaDataSet.SIZE ));
    }

		protected void finalize()
			throws Throwable
		{
			if (transferSocket != null)
				transferSocket = null;

			super.finalize();
		}

		/**
		 *
		 */
		private GeneralRandomAccessFile correctedRAFPath( int i )
			throws IOException
		{
//TODO all Local stuff
			//get SRB directory path from rl
			String dir =
				rl[i].getValue( SRBMetaDataSet.DIRECTORY_NAME ).toString();

			LocalFile subDir;
			//subtract SRB specific directoryPath
			if (!dir.equals( sourcePath )) {
				dir = dir.substring( sourcePath.length()+1 );

				//create proper necessary subdirectories
				subDir = new LocalFile( directoryPath, dir );
			}
			else {
				subDir = new LocalFile( directoryPath );
			}
			subDir.mkdirs();

			return FileFactory.newRandomAccessFile( new LocalFile(
				subDir, rl[i].getValue(	SRBMetaDataSet.FILE_NAME ).toString()), "rw" );
		}

    public void run()
    {
    	try {
				DataInputStream	in =
					new DataInputStream(transferSocket.getInputStream());
				OutputStream out = transferSocket.getOutputStream();

				int operation; //Whether PUT(=3) or GET(=4), etc.
				long offset; //Where to seek into the data
				long transferLength; //How much to read/write
				byte[] buffer = null; //Holds all the data for transfer
				int i = 0;

				int temp = 0;
				//TODO only 1448 bytes can be read each loop, TCP?
				int maxReadSize = 1448;

				//read the header
				operation = in.readInt();
//TODO 				if (operation != approved operation )
//					return or error?

				transferLength=in.readInt(); //TODO unsigned?

				buffer=new byte[LONG_LENGTH];
				in.read(buffer);
				offset = Host.castToUnsignedLong(buffer);

				if (offset < 0 )
					return; //TODO error?

				if (transferLength <= 0 )
					return;
				else if (transferLength >= Integer.MAX_VALUE)
					buffer = new byte[Integer.MAX_VALUE-1];
				else
					buffer = new byte[(int)transferLength];

				while (transferLength > 0) {
//System.out.println(" whichThread "+whichThread);
					switch (operation) {
						//case READ:
						case GET:
						if (listener == null) {
							//bulk unload method. not for use with large files.
							//casting of offset to int and others will cause problems
							temp = 0;
							if (transferLength > maxReadSize) {
								while (transferLength > (temp + maxReadSize - 1)) {
									temp += in.read( buffer, temp, maxReadSize );
								}
								while ((transferLength - temp - 1) > 0) {
									temp +=	in.read( buffer, temp, ((int)transferLength - temp) );
								}
							}
							else {
								while ((transferLength - temp) > 0) {
									temp +=	in.read( buffer, temp, ((int)transferLength - temp) );
								}
							}
							bytesMoved += temp;

							raf.seek( offset );
							if (length >= temp) {
								raf.write( buffer );
								length -= temp;
							}
							else {
								//use for offset into buffer
								offset = 0;
								while (length < temp) {
									//write final bytes to this file
									raf.write( buffer, (int)offset, length );
									offset += length;
									temp -= length;
									i++;

									//start new file
									if (i < rl.length) {
										length = rl[i].getIntValue(
											rl[i].getFieldIndex( SRBMetaDataSet.SIZE ) );
										raf = correctedRAFPath(i);
									}
								}
								//write final bytes to last file in buffer
								raf.write( buffer, (int)offset, temp );
								length -= temp;
							}
							temp = 0;

							//read the header
							operation = in.readInt();
//TODO 				if (operation != approved operation )
//							return or error?

							transferLength=in.readInt(); //TODO unsigned?

							buffer=new byte[LONG_LENGTH];
							in.read(buffer);

							if (offset < 0 )
								break; //TODO error?

							if (transferLength <= 0 )
								break;
							else if (transferLength >= Integer.MAX_VALUE)
								//fortunately I don't think is ever bigger then 8MB
								//because this would be a huge buffer, and probably crash
								//um, TODO...
								buffer = new byte[Integer.MAX_VALUE-1];
							else
								buffer = new byte[(int)transferLength];
						}
						else {
synchronized (syncObject) {
							temp = 0;

							if (transferLength > maxReadSize) {
								while (transferLength > (temp + maxReadSize - 1)) {
									temp += in.read( buffer, temp, maxReadSize );
								}
								while ((transferLength - temp - 1) > 0) {
									temp +=	in.read( buffer, temp, ((int)transferLength - temp) );
								}
							}
							else {
								while ((transferLength - temp) > 0) {
									temp +=	in.read( buffer, temp, ((int)transferLength - temp) );
								}
							}
							bytesMoved += temp;
							temp = 0;

							if (raf.getFilePointer() != offset) {
								raf.seek( offset );
							}

							if (transferLength > 0) {
								raf.write( buffer );
							}

							//read the header
							operation = in.readInt();
//TODO 				if (operation != approved operation )
//							return or error?

							transferLength=in.readInt(); //TODO unsigned?

							buffer=new byte[LONG_LENGTH];
							in.read(buffer);
							offset = Host.castToUnsignedLong(buffer);

							if (offset < 0 )
								break; //TODO error?

							if (transferLength <= 0 )
								break;
							else if (transferLength >= Integer.MAX_VALUE)
								buffer = new byte[Integer.MAX_VALUE-1];
							else
								buffer = new byte[(int)transferLength];
						}

}//sync
						break;
						//case WRITE:
						case PUT:
//System.out.print("put l "+buffer.length);
							//read buffer loops until finished
							raf.seek( offset );
							bytesMoved += raf.read( buffer );
//System.out.print(" "+bytesMoved+"-"+whichThread);
							out.write( buffer );
try{
this.wait(100);
} catch (Throwable e) { }
							//read the header
							operation = in.readInt();
//TODO 						if (operation != approved operation )
//							return or error?

							transferLength=in.readInt(); //TODO unsigned?

							buffer=new byte[LONG_LENGTH];
							in.read(buffer);
							offset = Host.castToUnsignedLong(buffer);
							if (offset < 0 )
								break; //TODO error?

							if (transferLength <= 0 )
								break;
							else if (transferLength == buffer.length) {
								//don't do anything
							}
							else if (transferLength >= Integer.MAX_VALUE)
								buffer = new byte[Integer.MAX_VALUE-1];
							else
								buffer = new byte[(int)transferLength];
							break;
						case DONE:
						default:
//System.out.print("DONE");

							// signal to the main thread if everyone is done
							transferLength = 0;
							break;
					}
//System.out.print("end while");

				}
				//make sure not to copy more than total
				totalBytesMoved += bytesMoved;
//System.out.print(" bM="+bytesMoved+"#"+whichThread);

//TODO error in totalBytesMoved, only get?
//System.out.println("listener "+listener);
				if ((totalBytesMoved >= srcSize) &&	(listener != null) &&
					(listener instanceof ServerSocket))
				{
//TODO problem for client init put?
					((ServerSocket)listener).close();
				}
				if ((syncObject != null) && (syncObject instanceof ParallelManager)) {
					((ParallelManager) syncObject).threadComplete(whichThread);
				}
			} catch (SocketException e) {
				if (out == null)
					out = null;
				if (in == null)
					in = null;
				if (transferSocket != null)
					transferSocket = null;
e.printStackTrace();
			}	catch (IOException e) {
				//can't throw IOException because Runnable.run() doesn't?
				throw new RuntimeException( "IOException in thread.", e );
			}
		}
	}
//--------------------------------------------------------------------
// End - Parallel IO
//--------------------------------------------------------------------



//--------------------------------------------------------------------
// SRB 3.0 functions
//--------------------------------------------------------------------
	/**
	 * Get the mcatName of this user.
	 *
	 * @param userName - The input userName.
	 * @param domainName - The input domainName.
	 * @param mcatName - The output mcatname. It should be preallocated
	 *		with length SRBFileSystem.MAX_TOKEN.
	 *
	 * the CALLER must have allocated enough space to hold the result returned
	 */
	synchronized void srbGetMcatZone( String userName, String domainName,
		String mcatName )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbGetMcatZone");
		startSRBCommand( F_GET_MCAT_NAME, 2 );

		sendArg( userName );
		sendArg( domainName );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}


	/**
	 * Get the MCAT-enabled server's public key in preparation for transferring
	 * encryptioned information. Also see the sscSetupSessionPublicKey
	 * library routine.
	 *
	 * @param publicKey a string representation of the current public key,
	 *               If server-side secure communication is not supported,
	 *               publicKey will instead contain an error message string.
	 */
	synchronized void srbSetupSessionPublicKey( String publicKey )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbSetupSessionPublicKey");
		startSRBCommand( F_SETUP_SESSION_PUBLIC_KEY, 1 );

		sendArg( 0 );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}


	/**
	 * Set up a session (for encryption) with the MCAT-enabled server.
	 * Also see the sscSetupSession library routine.
	 *
	 * @param sessionKey a string representation of the session key
	 * 		from sscSetupSession (encrypted in the public key)
	 */
	synchronized void srbSetupSession( String sessionKey )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbSetupSession");
		startSRBCommand( F_SETUP_SESSION, 1 );

		sendArg( sessionKey );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}


	/**
	 * Bulk load a set of data. All the actual data should
	 * already be contained in bloadFullPath, but in concatenated form
	 * just as in the case of a container.
	 *
	 * @param catType catalog type, 0 = MCAT
	 * @param bloadFullPath The name of the container.
	 * @param rl The list of the objects to be registered.
	 * 		It should contain dataNameList, collectionNameList,
	 * 		dataSizeList (in ascii)(SRB will perform atol), offSetList (in ascii),
	 * 		the number of datsets to be registered.
	 */
	synchronized void srbBulkLoad( int catType, String bulkLoadFilePath,
		SRBMetaDataRecordList[] rl )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbBulkLoad");
		startSRBCommand( F_BULK_LOAD, 3 );

		sendArg( catType );
		sendArg( bulkLoadFilePath );
		sendArg( rl );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}

//TODO make perm. add for singleServerPort
boolean singlePortBulkUnload = false;
	/**
	 * The client initiated socket conection version of
	 * Bulk unload of non-container files in a collection recursively.
	 *
	 * @param catType catalog type, 0 = MCAT
	 * @param flag flag value for future use.
	 * @param collection The collection to unload
	 * @return
	 */
	synchronized void srbBulkUnload( int catType, int flag,
		String srbUnloadDirPath, String localDirPath )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbBulkUnload");

		//reset, from previous uses
		totalBytesMoved = 0;
		bytesMoved = 0;

		int status = 0;

		Socket[] bulkConn = null;
		InputStream[] bulkIn = null;
		OutputStream[] bulkOut = null;
		SRBMetaDataRecordList[] rl = null;

		int numAddr = 1;

		int[] cookie = null;
		int[] portNum = null;
		byte[] temp = null;
		byte[] temp2 = null;

		String hostAddress = null;

		InputStream tempIn = in;
		TransferThread transfer[] = null;
		Thread transferThread[] = null;

		//1st, inform the SRB of the collection to be downloaded
		startSRBCommand( F_BULK_UNLOAD_C, 3 );

		sendArg( catType );
		sendArg( flag );
		sendArg( srbUnloadDirPath );
		flush( );
		commandStatus();

		//packet length
		status = readInt();
			//Probably how many bytes to read,
			//but I just assume it says 4 because it should, and sometimes doesn't.
		temp = read(4);
		status = Host.castToInt(temp);
		if (status == MSG_USE_SINGLE_PORT) {
			//needed during the return with single server port protocol
			singlePortBulkUnload = true;
			numAddr = 1;
			status();
		}
		else if (status < 0) {
			throw new SRBException( status );
		}
		else {
			//returns the struct port_t
			//which is used to connect to the new location for the bulk unload
			numAddr = Host.castToInt( read(INT_LENGTH) );

			cookie = new int[numAddr];
			portNum = new int[numAddr];
			temp = new byte[INT_LENGTH];
			temp2 = new byte[INT_LENGTH];

			for (int i=0;i<numAddr;i++) {
				temp[2] = read();
				temp[3] = read();

				cookie[i] = Host.castToInt( temp );

				temp2[2] = read();
				temp2[3] = read();

				portNum[i] = Host.castToInt( temp2 );
			}
			hostAddress = readString();
			status();


			//TODO SRB only ever makes one connection for now
			//Open the new direct bulk unload connection(s)
			bulkConn = new Socket[numAddr];
			bulkIn = new InputStream[numAddr];
			bulkOut = new OutputStream[numAddr];
			for (int i=0;i<numAddr;i++) {
				try {
					bulkConn[i] = new Socket(hostAddress, portNum[i]);
					bulkIn[i] = bulkConn[i].getInputStream();
					bulkOut[i] = bulkConn[i].getOutputStream();
				}
				catch (ConnectException e) {
					ConnectException connException = new ConnectException(
						"Bulk download connection cannot be made to: "+
							hostAddress +	" at port: "+ portNum[i] );
					connException.initCause(e);
					throw connException;
				}
				catch ( SocketException e ) {
					SocketException socketException = new SocketException(
						"A bulk download socket error occured when connecting to: "+
							hostAddress +	" at port: "+ portNum[i] );
					socketException.initCause(e);
					throw socketException;
				}
			}
		}

		transfer = new TransferThread[numAddr];
		transferThread = new Thread[numAddr];

		//The protocol changes slightly for single socket server connections
		if (status == MSG_USE_SINGLE_PORT) {
			rl = returnSRBMetaDataRecordList(true, true);

			if (rl != null) {
				//Just one thread
				transfer[0] = new TransferThread(
					srbConnection, rl, localDirPath, srbUnloadDirPath );
				transferThread[0] = new Thread(transfer[0]);
				transferThread[0].start();
			}
		}
		else {
			tempIn = in;
			for (int i=0;i<numAddr;i++) {
				bulkOut[i].write(temp);

//er I suppose I should make a returnSRBMetaDataRecordList method
//where I can pass in the stream
in = bulkIn[i];
				//4bytes, junk I don't need
				read(4);
				rl = returnSRBMetaDataRecordList(true, true);
//er
in = tempIn;

				//Manages new threads the server initiates
				transfer[i] = new TransferThread(
					bulkConn[i], rl, localDirPath, srbUnloadDirPath );
				transferThread[i] = new Thread(transfer[i]);
				transferThread[i].start();
//transfer[i].run();
			}
		}

		try {
			for (int i=0;i<transferThread.length;i++) {
				if (transferThread[i] != null) {
					if (transferThread[i].isAlive()) {
						transferThread[i].join();
					}
				}
			}
		}
		catch(InterruptedException e) {
			//TODO don't handle...
			e.printStackTrace();
		}

		//reset
		if ( singlePortBulkUnload )
			singlePortBulkUnload = false;
	}


	/**
	 * Modify and Insert SRB zone and zone information in the
	 *  Metadata Catalog. Information about the operation  performed is
	 *  also logged in the audit  trail (if turned on). This is a
	 *  privileged function and should be called only by a
	 *  srbAdmin user.
	 *
	 *
	 * @param catType      - catalog type. e,g., MDAS_CATALOG.
	 * @param zone_name  - name of the zone
	 * @param dataValue1 - Input value 1.
	 * @param dataValue2 - Input value 2.
	 * @param dataValue3 - Input value 3.
	 * @param dataValue4 - Input value 4.
	 * @param dataValue5 - Input value 5.
	 * @param actionType  - The type of action. performed values supported are:<br>
	 *
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
	synchronized void srbModifyZone( int catType, String zoneName,
		String dataValue1, String dataValue2, String dataValue3,
		String dataValue4, String dataValue5, int actionType )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbModifyZone");
		startSRBCommand( F_MODIFY_ZONE, 8 );

		sendArg( catType );
		sendArg( zoneName );
		sendArg( dataValue1 );
		sendArg( dataValue2 );
		sendArg( dataValue3 );
		sendArg( dataValue4 );
		sendArg( dataValue5 );
		sendArg( actionType );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}




	/**
	 * srbBulkQueryAnswer -  Get answers for canned queries from MCAT.
	 *
	 * @param catType - catalog type - 0 - MCAT
	 * @param  queryInfo - query information all info needed to
	 *               perform the query is in this string. The semantics for
	 *               the string depends upon the operation.
	 * @param  rowsWanted - number of rows of result wanted.
	 *
	 * @return the query result, metadata record list.
	 */
	synchronized MetaDataRecordList[] srbBulkQueryAnswer( int catType,
		String queryInfo, MetaDataRecordList myresult, int rowsWanted )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbBulkQueryAnswer");
		startSRBCommand( F_BULK_QUERY_ANSWER, 3 );

		sendArg( catType );
		sendArg( queryInfo );
		sendArg( rowsWanted );
		flush( );

		commandStatus();
		return returnSRBMetaDataRecordList();


/*
		String buf;
		int buflen;
		int retval;
		long argv[MAXFMGRARGS];
		ClOut clOut;
		int clOutVal;

		argv[0] = catType;
		argv[1] = addrToSRBLong (queryInfo);
		argv[2] = rowsWanted;

		if ((buflen = getSqlResultLen (SRBFileSystem.TOTAL_METADATA_ATTRIBUTES,
			rowsWanted)) <= 0)
				return CLI_ERR_MALLOC;

		buf = malloc (buflen);
		if (buf == NULL)
				return CLI_ERR_MALLOC;

		clOut.retval = (String ) buf;

		retval = clCall (conn, F_BULK_QUERY_ANSWER, argv, &clOut);

		if (retval < 0 || rowsWanted == 0 ) {
				free (buf);
				return retval;
		}

		strToSqlResult (buf, myresult);

		free (buf);

		if (myresult->result_count > 0)
				return 0;       /* success
		else
				return CLI_ERR_RETURN_LEN;
*/
	}



	/**
	 * srbBulkMcatIngest - Bulk ingestion of a set of metadata in to the
	 * SRB-MCAT system.
	 *
	 * @param catType - catalog type, 0 = MCAT
	 * @param ingestInfo - information about what is done. All info
	 *               needed to perform the ingestion is in this string. The
	 *               semantics for the string depends upon the operation.
	 * @param The MetaDataRecordList that contains the objects to be ingested.
	 */
	synchronized void srbBulkMcatIngest( int catType, String ingestInfo,
		SRBMetaDataRecordList[] rl )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbBulkMcatIngest");
		startSRBCommand( F_BULK_MCAT_INGEST, 3 );

		sendArg( catType );
		sendArg( ingestInfo );
		sendArg( rl );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}

//SRB 3.0.2
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
		if (DEBUG > 0) System.out.println("\n srbBackupData");
		startSRBCommand( F_BACKUP_DATA, 5 );

		sendArg( catType );
		sendArg( objID );
		sendArg( collectionName );
		sendArg( backupResource );
		sendArg( flag );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}


//SRB 3.1
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
	synchronized String srbObjChksum( String objID, String collectionName,
		int chksumFlag, String inpChksum )
			throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbObjChksum");
		startSRBCommand( F_SRBO_CHKSUM, 4 );

		sendArg( objID );
		sendArg( collectionName );
		sendArg( chksumFlag );
		sendArg( inpChksum );
		flush( );

		commandStatus();
		int status = readInt();
		if (status < 0) {
			throw new SRBException( status );
		}

		String checksum = "";
		for (int i=0; i<8; i++) {
			checksum += new String(read(1));
		}

		//garabe?
		read(status-8);
		status();
		return checksum;
	}



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
	synchronized void srbModifyUserNonPriv( int catType, String userNameDomain,
		String dataValue1, String dataValue2, String dataValue3, String dataValue4,
		String dataValue5, int actionType)
			throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbModifyUserNonPriv");
		startSRBCommand( F_MODIFY_USER_NP, 8 );

		sendArg( catType );
		sendArg( userNameDomain );
		sendArg( dataValue1 );
		sendArg( dataValue2 );
		sendArg( dataValue3 );
		sendArg( dataValue4 );
		sendArg( dataValue5 );
		sendArg( actionType );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}




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
	synchronized void srbModifyResource( int catType, String resourceName,
		String dataValue1, String dataValue2, String dataValue3, String dataValue4,
		int actionType )
		throws IOException
	{
		if (DEBUG > 0) System.out.println("\n srbModifyResource");
		startSRBCommand( F_MODIFY_RESOURCE, 7 );

		sendArg( catType );
		sendArg( resourceName );
		sendArg( dataValue1 );
		sendArg( dataValue2 );
		sendArg( dataValue3 );
		sendArg( dataValue4 );
		sendArg( actionType );
		flush( );

		commandStatus();
		int status = returnInt();
		if (status < 0) {
			throw new SRBException( status );
		}
	}
}
// End SRBCommands-----
