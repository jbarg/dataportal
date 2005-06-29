import edu.sdsc.grid.io.srb.SRBAccount;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBContainer;
import edu.sdsc.grid.io.srb.SRBException;
import edu.sdsc.grid.io.srb.SRBRandomAccessFile;
import edu.sdsc.grid.io.local.LocalFile;
import edu.sdsc.grid.io.local.LocalRandomAccessFile;

//import edu.sdsc.grid.io.srb.*;
//import edu.sdsc.grid.io.*;


import java.io.*;
import java.net.URI;



public class Test
{
/*
The SRB Scommands C client does the following:
 Sannotate  - Annotate SRB Objects and display annotations
 Sappend    - Appends a local file, standard in, or a SRB object to a target SRB object
 Sattrs     - Display meta attributes used in conditions
 Sbload     - imports in bulk one or more local files and/or directories into SRB space.
 Scat       - Streams SRB Objects to standard output
 Scd        - Changes working SRB collection
 Schmod     - Modify access permissions for SRB objects and collections
 Scp        - Copies an object to a new collection in SRB space
 Senv       - Displays environmental file content
 Serror     - Display Error Information from Error Number
 Sexit      - Clears environmental files created during SRB operation
 Sget       - Exports SRB objects into local file system
 SgetColl   - Displays information about SRB collections
 SgetD      - Displays information about SRB files (objects)
 SgetR      - Displays information about SRB resources
 SgetT      - Displays information about SRB tickets
 SgetU      - Displays information about SRB users
 Shelp      - Displays one-line help messages for all Scommands
 Sinit      - Initializes environmental files for SRB operation
 Sls        - Lists SRB objects and collections
 Slscont    - Lists SRB containers
 Smeta      - Displays and ingests meta data values
 Smkcont    - Creates a new SRB container
 Smkdir     - Creates a new SRB collection
 SmodColl   - Modifies some system metadata information on SRB collections
 SmodD      - Modifies some system metadata information on SRB objects
 Smv        - Changes the collection for objects in SRB space
 Spasswd    - Change SRB (Text) Password
 Spcommand  - Execute proxy command in SRB
 Sphymove   - Moves a SRB object to a new SRB resource
 Sput       - Imports local files into SRB space
 Spwd       - Displays current working SRB collection
 srbpget    - Exports SRB objects into local file system with client driven threads
 srbpput    - Imports local files into SRB space with client driven threads
 Sregister  - Registers an object as a SRB object in MCAT
 Sreplcont  - Replicates an existing SRB Container
 Sreplicate - Replicates an existing SRB object in a (possibly) new resource
 Srm        - Removes SRB objects
 Srmcont    - Removes SRB container
 Srmdir     - Removes an existing SRB collection
 Srmticket  - Remove a ticket
 Ssyncd     - Synchronizes copies of an SRB object
 Ssyncont   - Synchronize SRB containers
 Stcat      - Streams ticketed SRB Objects to standard output
 Sticket    - Issue tickets for SRB objects and collections
 Stls       - Lists ticketed SRB objects and collections
 Stoken     - Displays information about SRB-aware types.

These SRB Scommand equivalents are currently available in this API:
 Sannotate  - SRBFile.modifyMetaData
 Sappend    - SRBRandomAccessFile seek and write
 Sattrs     - javadoc
 Sbload     - LocalFile.copyTo( SRBFile ) and SRBFile.copyFrom( LocalFile )
 Scat       - SRBRandomAccessFile.read
 Scd        - SRBFile constructor
 Schmod     - SRBFile.changePermissions
 Scp        - SRBFile.copyTo
 Senv       - SRBAccount setters and getters
 Serror     - SRBException
 Sexit      - SRBFileSystem finalize
 Sget       - SRBFile.copyTo
 SgetColl   - SRBFile.query
 SgetD      - SRBFile.query
 SgetR      - SRBFileSystem.query
 SgetT      - SRBFileSystem.query
 SgetU      - SRBFileSystem.query
 Shelp      - javadoc
 Sinit      - SRBFileSystem constructor
 Sls        - SRBFile.list
 Slscont    - SRBFileSystem.query
 Smeta      - SRBFileSystem.query, SRBFile.query and SRBFile.modifyMetaData
 Smkcont    - SRBContainer.createNewFile
 Smkdir     - SRBFile.mkdir
 SmodColl   - SRBFile.modifyMetaData
 SmodD      - SRBFile.modifyMetaData
 Smv        - SRBFile.renameTo
 Spasswd    - SRBFileSystem.srbModifyUser
 Spcommand	- SRBFileSystem.executeProxyCommand
 Sphymove 	- SRBFile.setResource
 Sput       - SRBFile.copyFrom
 Spwd       - SRBFile.getAbsolutePath
 Sreplcont	- SRBContainer.replicate
 Sreplicate - SRBFile.replicate
 Srm        - SRBFile.delete
 Srmcont    - SRBContainer.delete
 Srmdir     - SRBFile.delete
 Stls       - SRBFileSystem.query
 Stoken     - SRBFileSystem.query
*/



//----------------------------------------------------------------------
// Main
//----------------------------------------------------------------------
	/**
	 * Testing SRB functions
	 */
	public static void main(String args[])
	{
		/**
		 * Open a connection to the SRB.
		 * Equivalent to Sinit in the SRB Scommands command-line tools.
		 * The filesystem object represents the connection to the filesystem.
		 * Only one filesystem object is needed to access all the files on
		 * that system.
		 *<P>
		 * Using the seven variable constructor is recommended,
		 * as all necessary data is passed to the constructor.
		 * Other constructors may use some system or configuration
		 * dependent default information and are provided for
		 * convenience.
		 */
		SRBAccount srbAccount = null;
		SRBFileSystem srbFileSystem = null;


		/**
		 * The LocalFile class is a wrapper class for the
		 * java.io.File class. Use it as you would a regular File object.
		 * The localFile.getFile() method will return a java.io.File object
		 * for those situations where a true java.io.File is required.
		 */
		LocalFile localFile = null;


		/**
		 * The SRBFile class is used in much the same manner as the
		 * java.io.File class. A SRBFile object can represent
		 * a file or directory on the SRB.
		 */
		SRBFile srbFile = null;


		/**
		 * The SRBContainer class is an extension of the SRBFile class.
		 * Used for manipulating srb containers.
		 */
		SRBContainer srbContainer = null;


		/**
		 * The SRBRandomAccessFile class is used in much the same manner as the
		 * java.io.RandomAccessFile class.
		 */
		SRBRandomAccessFile srbRandomAccessFile = null;



		try{
			//If you expect to connect to the SRB it will speed up the
			//initial connection if you set the static variable SRBAccount.version
			//to the correct server SRB version, such as
			//SRBAccount.setVersion(SRBAccount.SRB_VERSION_3);
			//The API can sometimes determine the SRB version used by the server,
			//but this will take time as the api tries to connect with the
			//various handshake protocol.

			/*
			//For GSI authentication, use something like this:
			SRBAccount acc = new SRBAccount();
			//set the password option to GSI_AUTH
			acc.setOptions( SRBAccount.GSI_AUTH );
			//give the file path of your proxy file instead of the a password
			acc.setPassword( "/tmp/x509up_u28227" );
			//If the CA locations are not defined in your cog.properties file:
			acc.setCertificateAuthority(
				"/etc/grid-security/certificates/b89793e4.0, "+
				"/etc/grid-security/certificates/42864e48.0" );
			GeneralFileSystem fs = new SRBFileSystem( acc );
			*/

			//Once the SRBFileSystem is intialized,
			//(such as with the FileFactory class)
			//the connection variables of that object cannot be changed.
			//Though new SRBFileSystem objects can be created with different
			//values.
System.out.println("\n Connect to the SRB.");
			if (args.length == 7) {
				//host, port, user name, password,
				//home directory, mdas domain home, default storage resource
				srbAccount = new SRBAccount(
					args[0], Integer.parseInt( args[1] ), args[2], args[3],
					args[4], args[5], args[6] );
			}
			else if (args.length == 1) {
				//user info file
				srbAccount = new SRBAccount( args[0] );
			}
			else if (args.length == 0) {
				//default
				srbAccount = new SRBAccount( );
			}
			else {
				throw new IllegalArgumentException(
					"Wrong number of arguments sent to Test." );
			}
			//Set account variables before initializing the file system.
			//Once the SRBFileSystem is intialized,
			//the connection variables cannot be changed.
			srbFileSystem = new SRBFileSystem( srbAccount );

			//Also available, using the default SRBAccount constructor:
			//srbFileSystem = new SRBFileSystem( );




System.out.println(
	"\n Make a new container in your SRB called "+
	"mySRBJargonTestContainer.");
			/**
			 * Create a new container on the SRB.
			 * Equivalent to Smkcont in the SRB Scommands command-line tools.
			 *<P>
			 * This method will create a new container from the name defined
			 * by the srbContainer object.
			 */
			srbContainer = new SRBContainer(
				srbFileSystem, "mySRBJargonTestContainer" );
			srbContainer.createNewFile();





System.out.println(
	"\n Make a new collection in your SRB home called "+
	"mySRBJargonTestDir.");
			/**
			 * Create a new directory on the SRB, also known as a collection.
			 * Equivalent to Smkdir in the SRB Scommands command-line tools.
			 *<P>
			 * This method will create a new collection from the
			 * abstract pathname defined by the SRBFile object.
			 */
			//using constructor SRBFile( SRBFileSystem fileSystem, String filePath )
			srbFile = new SRBFile( srbFileSystem, "mySRBJargonTestDir" );
			srbFile.mkdir();




System.out.println(
	"\n Put an example local file in the new collection as "+
	"mySRBJargonTestFile.");
			/**
			 * Copy a local file to the SRB.
			 * Equivalent to Sput in the SRB Scommands command-line tools.
			 *<P>
			 * Open a SRB file by passing the SRBConnection object and
			 * SRB filepath to the constructor. Next open a local file
			 * using the LocalFile class. Then pass the local file to
			 * the SRBFile method copyFrom.
			 */
			//creating an example local file.
			localFile = new LocalFile( "myInitialJargonFile" );
			if (!localFile.exists()) {
				LocalRandomAccessFile out = new LocalRandomAccessFile( localFile, "rw" );
				out.write(new String("This file is used to test the Jargon API. "+
					"It is ok to delete.").getBytes());
				out.close();
			}
			//using constructor SRBFile( SRBFile parent, String fileName )
			srbFile = new SRBFile( srbFile, "mySRBJargonTestFile" );
			srbFile.copyFrom( localFile );
			localFile.delete();



			/**
			 * Print the SRB URI for the new srb file.
			 */
			System.out.println(srbFile.toString());





System.out.println(
	"\n Random access read and write to the file mySRBJargonTestFile.");
			/**
			 * Open a random access connection to a file.
			 * The second variable passed to the constructor is the mode string:
			 * r = read-only, rw = read/write
			 */
			srbRandomAccessFile = new SRBRandomAccessFile( srbFile, "rw" );

			//Read from the file,
			byte[] buffer = new byte[1000];
			int bytesRead = srbRandomAccessFile.read( buffer );

			//seek to somewhere in the file,
			String fileContents = new String( buffer, 0, bytesRead );
			int insert = fileContents.indexOf( "It is ok to delete." );
			if (insert >= 0)
				srbRandomAccessFile.seek( insert );

			//and write new data.
			srbRandomAccessFile.write( new String(
				"The copies created on the SRB should get deleted automatically.\n"+
				"The local version was not deleted, "+
				"so you know if it worked.").getBytes() );

			//Be sure to close the file when you are finished.
			srbRandomAccessFile.close(); //or srbRandomAccessFile = null;




System.out.println("\n List the contents of the new collection:");
			/**
			 * List the contents of a collection.
			 * Equivalent to Sls in the SRB Scommands command-line tools.
			 *<P>
			 * The list method will return a string array containing
			 * First, the collection name.
			 * Then, all the datasets in that collection.
			 * Lastly, all the sub-collections to that collection.
			 */
			String[] dirList = srbFile.list();

			//print the directory list
			if (dirList != null) {
				for(int i=0;i<dirList.length;i++)
					System.out.println(dirList[i]);
			}




System.out.println("\n List the contents of the new container:");
			/**
			 * List the contents of a container.
			 * Equivalent to Slscont in the SRB Scommands command-line tools.
			 *<P>
			 * The list method will return a string array containing
			 * First, the container name.
			 * Then, all the datasets in that container. (in no particular order)
			 */
			String[] contList = srbContainer.list();

			//print the files in the container
			if (contList != null) {
				for(int i=0;i<contList.length;i++)
					System.out.println(contList[i]);
			}




System.out.println("\n Replicate mySRBJargonTestFile.");
			/**
			 * Replicate a file on the SRB. Creates another copy of the file
			 * on a different resource.
			 * Equivalent to Sreplicate in the SRB Scommands command-line tools.
			 */
			//Actually, uses the same resource in this example.
			srbFile.replicate( srbFile.getResource() );





System.out.println("\n Add read/write permissions to the file for \"testuser\".");
			/**
			 * Changing the permissions on a file.
			 * null = none, "r" = read, "w" = write, "rw" = all
			 * Equivalent to Schmod in the SRB Scommands command-line tools.
			 */
			String permission = "rw";
			String newUserName = "testuser";
			String newMdasDomain = "sdsc";
			srbFile.changePermissions( permission, newUserName, newMdasDomain );





System.out.println("\n Copy mySRBJargonTestFile to anotherSRBJargonTestFile.");
			/**
			 * Copy a file from one place to another on the remote file system.
			 * Equivalent to Scp in the SRB Scommands command-line tools.
			 */
			srbFile.copyTo(
				new SRBFile(srbFileSystem, "mySRBJargonTestDir/anotherSRBJargonTestFile") );





System.out.println("\n Rename mySRBJargonTestFile to newSRBJargonTestFile");
			/**
			 * Rename the file.
			 * Equivalent to Smv in the SRB Scommands command-line tools.
			 */
			srbFile.renameTo(new SRBFile( srbFileSystem,
				"mySRBJargonTestDir/newSRBJargonTestFile" ));





System.out.println(
	"\n Get mySRBJargonTestFile from the SRB and\n" +
	" put it in the current local directory as myLocalJargonFile.");
			/**
			 * Copy a SRB file to a local drive.
			 * Equivalent to Sget in the SRB Scommands command-line tools.
			 *<P>
			 * Open a SRB file by passing the SRBConnection object and
			 * SRB filepath to the constructor. Next open a local file
			 * using the LocalFile class. Then pass the local file to
			 * the SRBFile method copyToLocal.
			 */
			//using constructor SRBFile( SRBFileSystem fileSystem, String filePath )
			srbFile = new SRBFile( srbFileSystem,
				"mySRBJargonTestDir" );
			localFile = new LocalFile( "myLocalJargonFile" );
			srbFile.copyTo( localFile );







System.out.println("\n Execute the 'hello' proxy command on the SRB.");
			/**
			 * Execute a proxy command on the SRB. The protocol of the value
			 * returned on the InputStream depends on the command run.
			 */
			InputStream in = srbFileSystem.executeProxyCommand( "hello", null );
			int result = in.read();
			while (result != -1) {
				System.out.print((char)result);
				result = in.read();
			}




System.out.println("\n Remove the files created by this program from the SRB.");
			/**
			 * Delete a SRB file.
			 * Equivalent to Srm in the SRB Scommands command-line tools.
			 *<P>
			 * Delete a SRB file by passing the SRBConnection object and
			 * SRB filepath to the constructor.
			 */
			//using constructor SRBFile(
			//	SRBFileSystem fileSystem, String directory, String fileName )
			srbFile = new SRBFile(
				srbFileSystem,	"mySRBJargonTestDir", "newSRBJargonTestFile" );
			srbFile.delete();
			srbFile = new SRBFile(
				srbFileSystem,	"mySRBJargonTestDir", "anotherSRBJargonTestFile" );
			srbFile.delete();




System.out.println("\n Remove mySRBJargonTestDir from the SRB.");
			/**
			 * Delete a SRB collection.
			 * Equivalent to Srmdir in the SRB Scommands command-line tools.
			 */
			//using constructor SRBFile( SRBFileSystem fileSystem, String filePath )
			//but this time giving the constructor an absolute path, if you do not
			//give the constructor an absolute path, the SRBFile class will default
			//to your home directory, e.g. /home/testuser.sdsc/
			String absolutePath = srbFileSystem.getHomeDirectory() +
				srbFile.getPathSeparator() + "mySRBJargonTestDir";
			srbFile = new SRBFile( srbFileSystem, absolutePath );
			srbFile.delete();




System.out.println("\n Remove mySRBJargonTestContainer from the SRB.");
			/**
			 * Delete a SRB container.
			 * Equivalent to Srmcont in the SRB Scommands command-line tools.
			 */
			//Certain privileges may be required for this method
			srbContainer.delete();




System.out.println("\n Exit.");
			/**
			 * Close the connection to the SRB.
			 * Equivalent to Sexit in the SRB Scommands command-line tools.
			 */
			srbFileSystem = null;
		}
		catch( SRBException e ) {
System.out.println("\n Handling an error.");
			/**
			 * Handling an error from the SRB.
			 *<P>
			 * If there is an error returned from the SRB server,
			 * a SRBException will be thrown.
			 * SRBException subclasses IOException. Additionally a SRBException
			 * corresponds to a standardized error type.
			 * The standardized error can be expressed as a string or int.
			 */
			//Get the specific error type as an int
			int foo = e.getType( );
			//then do whatever is need with foo
			System.out.println( "Standardized SRB Server Error Type: "+ foo );

			//Get the standard error string
			String bar = e.getStandardMessage( );
			System.out.println( "Standardized SRB Server Error Message: "+ bar );

			//The original error message is still available through
			System.out.println( "\nJargon Error Message: "+ e.getMessage() );


			e.printStackTrace();
		}
		catch ( Throwable e ) {
			e.printStackTrace();
		}


		/**
		 * Testing if the connection is still valid after an exception is thrown.
		 * If a server error occured, and was handled improperly by the API, the
		 * program will block indefinately at this method.
		 */
		try{
			if (srbFile == null)
				srbFile = new SRBFile( srbFileSystem, "asdf");

			System.out.println("\nSocket Exception test, srbFile.length="+srbFile.length());
		}
		catch ( Throwable e ) {
			e.printStackTrace();

			Throwable chain = e.getCause();
			while (chain != null) {
				chain.printStackTrace();
				chain = chain.getCause();
			}
		}


		System.exit(0);
	}
}
