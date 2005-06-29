import edu.sdsc.grid.io.local.*;
import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.*;

import java.net.URI;


import java.util.StringTokenizer;

public class MetaDataTest
{
	public static void printQueryResults( MetaDataRecordList[] rl )
	{
		//print query results
		if (rl != null) { //Nothing in the database matched the query
			System.out.print("\n");
			for (int i=0;i<rl.length;i++) {
				System.out.print("\n");
				for (int j=0;j<rl[i].getRecordCount();j++) {
					System.out.print("\n");
					System.out.print(rl[i].getFieldName(j)+": ");
					if (rl[i].getFieldType(j) == MetaDataField.TABLE) {
						MetaDataTable table = rl[i].getTableValue(j);
						if (table != null) {
							System.out.print("\ntable: ");
							for (int k=0;k<table.getRowCount();k++) {
								System.out.print("\n");
								for (int l=0;l<table.getColumnCount();l++) {
									System.out.print(table.getStringValue( k, l )+"\t");
								}
							}
						}
					}
					else {
						System.out.print(rl[i].getStringValue(j)+"\t");
					}
				}
			}
		}
		System.out.println("\n");

//		System.exit(0);
	}



//----------------------------------------------------------------------
// Main
//----------------------------------------------------------------------
	/**
	 * Testing
	 */
	public static void main(String args[])
	{
		/**
		 * Open a connection to the file system.
		 * The filesystem object represents the connection to the filesystem.
		 * Only one filesystem object is needed to access all the files on
		 * that system.
		 */
		GeneralAccount account = null;
		GeneralFileSystem fileSystem = null;


		/**
		 * The GeneralFile class is used in much the same manner as the
		 * java.io.File class. A GeneralFile object can represent
		 * a file or directory.
		 */
		GeneralFile file = null;


		/**
		 * The metadata records list for each file (directory, or other value
		 * the query selected for) is stored in this array.
		 */
		MetaDataRecordList[] rl = null;



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
System.out.println("\n Connect to the fileSystem.");
			/**
			 * You can query any filesystem object though not all,
			 * such as LocalFileSystem, will return useful results.
			 */
			if (args.length == 1) {
				//url to file
				file = FileFactory.newFile(new URI( args[0] ));
				fileSystem = file.getFileSystem();
			}
			else if (args.length == 0) {
				fileSystem = FileFactory.newFileSystem( new SRBAccount( ) );

				String fileName = "myJARGONMetaDataTestFile";
				file = FileFactory.newFile( fileSystem, fileName );
			}
			else {
				throw new IllegalArgumentException(
					"Wrong number of arguments sent to Test.");
			}
			file.createNewFile();



			//A list of values used with MetaDataCondition.IN
			String[] values = { "sysadmin", "staff" };

System.out.println("\n Query the file system for all files matching certain "+
	"query conditions");
			/**
			 * The conditions of a metadata query are created using the static
			 * methods of the MetaDataSet class.maybe me xspain no good.
			 * The .newCondition method takes three variables:
			 * a static metadata name, an operator, and the value to compare.
			 */
			MetaDataCondition conditions[] = {
				MetaDataSet.newCondition(
					SRBMetaDataSet.OFFSET, MetaDataCondition.LESS_THAN, 5000 ),

				MetaDataSet.newCondition(
					SRBMetaDataSet.SIZE, MetaDataCondition.BETWEEN, 0, 10000 ),

				MetaDataSet.newCondition(
					SRBMetaDataSet.USER_TYPE_NAME, MetaDataCondition.IN, values  ),

				MetaDataSet.newCondition( SRBMetaDataSet.DIRECTORY_NAME,
					MetaDataCondition.EQUAL, file.getParent() )
			};


			/**
			 * For items which matched the query, met the conditions above,
			 * the following values will be returned.
			 */
			String[] selectFieldNames = {
				SRBMetaDataSet.FILE_NAME,
				SRBMetaDataSet.FILE_COMMENTS,
				SRBMetaDataSet.SIZE,
				SRBMetaDataSet.ACCESS_CONSTRAINT,
				SRBMetaDataSet.MODIFICATION_DATE,
				SRBMetaDataSet.USER_NAME,
				SRBMetaDataSet.USER_GROUP_NAME
			};
			MetaDataSelect selects[] =
				MetaDataSet.newSelection( selectFieldNames );



			/**
			 * The number of metadata values you want the query to return at one time.
			 * If more values would result from the query the can be appended to the
			 * MetaDataRecordList object using the .getMoreResults( int )
			 */
			//Number of values returned from server in first iteration.
			int numberOfResults = 2;
			//Full query method
			rl = fileSystem.query(
				conditions, selects, numberOfResults );

			printQueryResults( rl );





System.out.println("\n Get further results from the query.");
			/**
			 * By default a query will only return the first 300 values. The query
			 * above was set to return only the first two. The methods
			 * isQueryComplete(), getMoreResults(), getMoreResults(int numberOfResults)
			 * and getAllResults(), are used to retrieve further results from
			 * the query.
			 */
			if (rl != null) { //Nothing in the database matched the above query
				if (!rl[0].isQueryComplete()) {
					rl = rl[0].getMoreResults();
				}
				else {
					rl = null;
				}
			}

			printQueryResults( rl );





System.out.println("\n Change the metadata of a file.");
			/**
			 * The metadata can be changed by creating a new, or modifying an
			 * existing, MetaDataRecordList and then sending it back to the
			 * file system.
			 */
			//This will set the file's metadata to the same as is defined
			//by the MetaDataRecordList.
			//You can add new fields to the record list, change existing values,
			//or create a new MetaDataRecordList.
			rl = new MetaDataRecordList[1];
			//this is just an example, changing the size without actually
			//changing the file's real size is probably a bad idea,
			//but I couldn't think of anything else.
			rl[0] = new SRBMetaDataRecordList( SRBMetaDataSet.getField(
				SRBMetaDataSet.SIZE ), 123 );
			rl[0].addRecord( SRBMetaDataSet.getField(
				SRBMetaDataSet.FILE_COMMENTS ), "new comments go here." );

			//send the new metadata to the file system.
			file.modifyMetaData( rl[0] );





System.out.println("\n Query a single file/directory.");
			/**
			 * The simplest method when querying a specific file would be to use
			 * GeneralFile.query( String[] selectFieldNames ). For a particular file this
			 * will select and return the metadata named by selectFieldNames.
			 */
			rl = file.getParentFile().query( selectFieldNames );

			printQueryResults( rl );




System.out.println("\n Set the definable metadata.");
			/**
			 * Setting the definable metadata is the same as system metadata.
			 * The operators of the MetaDataTable will all be treated as if
			 * they were MetaDataCondition.EQUAL.
			 */
			String[][] definableMetaDataValues = new String[3][2];

			definableMetaDataValues[0][0] = "zxcv";
			definableMetaDataValues[0][1] = "123";

			definableMetaDataValues[1][0] = "asdf";
			definableMetaDataValues[1][1] = "999";

			definableMetaDataValues[2][0] = "qwer";
			definableMetaDataValues[2][1] = "111";

			int[] operators = new int[definableMetaDataValues.length];
			operators[0] = MetaDataCondition.EQUAL;
			operators[1] = MetaDataCondition.LESS_OR_EQUAL;
			operators[2] = MetaDataCondition.GREATER_OR_EQUAL;

			//	The operators don't matter when setting metadata, however
			//	as a query this table would find those items with user
			//	defined metadata matching: (see below)
			//	zxcv = 123
			//	asdf <= 999
			//	qwer >= 111
			//

			MetaDataTable metaDataTable =
				new MetaDataTable( operators, definableMetaDataValues );

			rl = new MetaDataRecordList[1];
			rl[0] = new SRBMetaDataRecordList( SRBMetaDataSet.getField(
				SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES ), metaDataTable );
			rl[0].addRecord( SRBMetaDataSet.getField(
				SRBMetaDataSet.FILE_COMMENTS ),"Hi." );

			file.modifyMetaData( rl[0] );

			rl = file.getParentFile().query( selects );
			printQueryResults( rl );






System.out.println("\n Using the user definable metadata.");
			/**
			 * The SRB supports user definable metadata. Access to this
			 * data is provided through the MetaDataTable class.
			 */
			conditions = new MetaDataCondition[1];
			conditions[0] = MetaDataSet.newCondition(
				SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES, metaDataTable );

			//(It was easier to just reuse the selects array, no special reason.)
			selects[3] = MetaDataSet.newSelection(
				SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES );
			selects[4] = MetaDataSet.newSelection(
				SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES );
			selects[5] = MetaDataSet.newSelection(
				SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS );
			selects[6] = MetaDataSet.newSelection(
				SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES );

			rl = fileSystem.query( conditions, selects );

			printQueryResults( rl );





System.out.println("\n List all the MetaData groups and their fields? (y/n)");
if (System.in.read() == 121) {
			/**
			 * Prints all the groups and their fields that have been registered
			 * in the MetaDataSet class.
			 */
			MetaDataGroup[] groups = MetaDataSet.getMetaDataGroups();
			MetaDataField[] fields = null;
			System.out.println("Number of MetaDataGroups: "+groups.length);
			for (int j=0;j<groups.length;j++) {
				System.out.println("\nMetaDataGroup: "+groups[j]+",  MetaDataFields in this group: "
					+groups[j].getFields().length);
				fields = groups[j].getFields();
				for (int i=0;i<fields.length;i++) {
					System.out.println(fields[i].getName()+", "+fields[i]);
				}
			}
			System.out.println("\n");
}





System.out.println("\n Exit.");
			/**
			 * Close the connection to the SRB.
			 * Equivalent to Sexit in the SRB Scommands command-line tools.
			 */
			fileSystem = null;
		}
		catch( SRBException e ) {
			//The original error message is still available through
			System.out.println( "\nJargon Error Message: "+ e.getMessage() +
			" "+e.getStandardMessage()+" "+e.getType() );

			e.printStackTrace();
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
