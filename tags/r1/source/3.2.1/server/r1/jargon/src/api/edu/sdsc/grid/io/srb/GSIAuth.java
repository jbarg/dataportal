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

import java.net.Socket;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.globus.common.CoGProperties;
import org.globus.gsi.gssapi.net.impl.GSIGssInputStream;
import org.globus.gsi.gssapi.net.impl.GSIGssOutputStream;
import org.globus.gsi.gssapi.net.impl.GSIGssSocket;
import org.gridforum.jgss.ExtendedGSSCredential;
import org.gridforum.jgss.ExtendedGSSManager;
import org.ietf.jgss.GSSContext;
import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSException;


/**
 * Instances of this class support socket I/O to a Srb server.
 *<P>
 * Handles socket level protocol for interacting with the SRB.
 *<P>
 * See also: <a href="doc-files/SRBProtocol.htm">SRB protocol</a>
 *
 * <P>
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 */
//This is the working GSIAuth class. If you have the GSI libraries,
//(Which should be available at the JARGON website)
//overwrite GSIAuth.java with this file.
//This class and those libraries will allow you to compile Jargon to include
//All functionality of Jargon, including GSI authentication.
class GSIAuth
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------





//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------




//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Checks the authentication using GSI of a SRB connection which has already
	 * passed the uner info part of the handshake.
	 *<P>
	 * @param proxyPath	the directory to find the proxy file.
	 * @param caLocations	the directory to find the CA file(s).
	 * @param srbConnection	The open socket to the SRB.
	 * @param out	The output stream from that socket.
	 * @param in The input stream from that socket.
	 * @throws	IOException	if the authentication to the SRB fails.
	 */
	GSIAuth( String proxyPath, String caLocations, Socket srbConnection,
		OutputStream out, InputStream in )
		throws IOException
	{
		sendGSIAuth( proxyPath, caLocations, srbConnection, out, in );
	}




//----------------------------------------------------------------------
// Methods
//----------------------------------------------------------------------
	/**
	 * GSI authorization method. Makes a connection to the SRB using
	 * the GSI authorization scheme.
	 *
	 * @param proxyPath	the directory to find the proxy file.
	 * @param caLocations	the directory to find the CA file(s).
	 * @param srbConnection	The open socket to the SRB.
	 * @param out	The output stream from that socket.
	 * @param in The input stream from that socket.
	 * @throws	IOException	if the authentication to the SRB fails.
	 */
	void sendGSIAuth( String proxyPath, String caLocations, Socket srbConnection,
		OutputStream out, InputStream in )
		throws IOException
	{
		CoGProperties cog = null;
		String defaultCA = null;

		byte[] data = null;
		LocalFile f = new LocalFile(proxyPath);
		if (f.exists()) {
			GeneralRandomAccessFile inputFile =
				FileFactory.newRandomAccessFile(f, "r");
			data = new byte[(int)f.length()];
			// read in the credential data
			inputFile.read(data);
			inputFile.close();
		}
		else {
			throw new IOException( "Proxy file path invalid" );
		}

		try {
			if (caLocations != null) {
//TODO they said there is no other way to do this.
//so I'm overwriting the default then changing it back.
				cog = CoGProperties.getDefault();
				defaultCA = cog.getCaCertLocations();
				cog.setCaCertLocations( caLocations );
			}

			ExtendedGSSManager manager =
				(ExtendedGSSManager)ExtendedGSSManager.getInstance();

			GSSCredential credential = manager.createCredential(data, ExtendedGSSCredential.IMPEXP_OPAQUE, GSSCredential.DEFAULT_LIFETIME, null, GSSCredential.INITIATE_AND_ACCEPT);

			GSSContext context = null;
			GSIGssOutputStream gssout = null;
			GSIGssInputStream gssin = null;

			context = manager.createContext(null,
				null,
				credential,
				GSSContext.DEFAULT_LIFETIME);

			context.requestCredDeleg(false);
			context.requestMutualAuth(true);

			GSIGssSocket ggSocket = new GSIGssSocket( srbConnection, context );
			gssout = new GSIGssOutputStream(out, context);
			gssin = new GSIGssInputStream(in, context);

			byte [] inToken = new byte[0];
			byte [] outToken = null;

			while( !context.isEstablished() ) {
				outToken = context.initSecContext(inToken, 0, inToken.length);

				if (outToken != null) {
					gssout.writeToken(outToken);
				}

				if (!context.isEstablished()) {
					inToken = gssin.readHandshakeToken();
				}
			}
		} catch ( GSSException e ) {
			SecurityException gsiException = null;
			String message = e.getMessage();
			if (message.indexOf("Invalid buffer") >= 0) {
				gsiException = new SecurityException(
					"GSI Authentication Failed - Invalid Proxy File" );
				gsiException.initCause(e);
			}
			else if (message.indexOf("Unknown CA") >= 0) {
				gsiException = new SecurityException(
					"GSI Authentication Failed - Cannot find "+
					"Certificate Authority (CA)" );
				gsiException.initCause(e);
			}
			else {
				gsiException = new SecurityException(
					"GSI Authentication Failed" );
				gsiException.initCause(e);
			}
			throw gsiException;
		}
		catch ( Throwable e ) {
			SecurityException exception = new SecurityException(
				"GSI Authentication Failed" );
			exception.initCause(e);
			throw exception;
		}
		finally {
			if (defaultCA != null) {
				cog.setCaCertLocations( defaultCA );
			}
		}
	}
}
// End GSIAuth-----
