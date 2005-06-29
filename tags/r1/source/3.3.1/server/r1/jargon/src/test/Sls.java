
import edu.sdsc.grid.io.local.*;
import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.*;
import java.io.*;

public class Sls
{

//----------------------------------------------------------------------
// Main
//----------------------------------------------------------------------
	/**
	 * Testing
	 */
	public static void main(String args[])
	{
		SRBFileSystem srbFileSystem = null;

		SRBFile srbFile = null;

		String[] list = null;

		try{
			srbFileSystem = new SRBFileSystem( );

			if ((args.length == 2) && (args[0].equals("-c"))) {
				srbFile = new SRBContainer( srbFileSystem, args[1] );
			}
			else if (args.length == 1) {
				srbFile = new SRBFile( srbFileSystem, args[0] );
			}
			else if (args.length == 0) {
				srbFile =
					new SRBFile( srbFileSystem, srbFileSystem.getHomeDirectory() );
			}
			else {
				throw new IllegalArgumentException(
					"\nUsage: Sls fileName/directory\nUsage: Sls -c containerName"+
					"\nUsage: Sls fileName/directory\nUsage: Sls [-r] dirName");
			}

			list = srbFile.list();

			if (list != null) {
				for (int i=0;i<list.length;i++) {
					System.out.println(list[i]);
				}
			}
		}
		catch( SRBException e ) {
			//Get the standard error string
			String bar = e.getStandardMessage( );
			System.out.println( "Standardized SRB Server Error Message: "+ bar );

			//The original error message is still available through
			System.out.println( "\nGridTools Error Message: "+ e.getMessage() );


			e.printStackTrace();
		}
		catch ( Throwable e ) {
			e.printStackTrace();
		}


		System.exit(0);
	}


/* Arun's way
	public static void main(String args[])
	{
		SRBFileSystem srbFileSystem = null;
		SRBFile srbFile = null;
		String[] list = null;
		try {
			// command line processing setup
			Options options = new Options();
			options.addOption("C", "container", true, "Container to list");
			options.addOption("c", "collection", true, "Collection to list");
			options.addOption("r", "recursive", false, "recursive listing");
			options.addOption("h", "help", false, "this help message");
			// command line parsing
			CommandLineParser parser = new PosixParser();
			CommandLine cmd = parser.parse(options, args);
			// command line process
			if (cmd.hasOption('r')) {
					srbFileSystem = new SRBFileSystem();
					if (cmd.hasOption('c') ||
							(! (cmd.hasOption('c') || cmd.hasOption('C')))) {
							if (cmd.hasOption('c')) {
									srbFile = new SRBFile(srbFileSystem,
																				cmd.getOptionValue("c"));
							} else {
									srbFile =
										new SRBFile(srbFileSystem,
																srbFileSystem.getHomeDirectory());
							}
					} else if (cmd.hasOption('C')) {
							srbFile = new SRBContainer(srbFileSystem,
								cmd.getOptionValue("C"));
								System.err.println("Recursive container listing not supported yet");
					} else { //must not come here
							printHelp(options);
					}
					MetaDataCondition conditions[] = {
								MetaDataSet.newCondition(
								SRBMetaDataSet.DIRECTORY_NAME,
								MetaDataCondition.LIKE,
								srbFile.getAbsolutePath() + "%"),
							};
							MetaDataSelect selects[] = {MetaDataSet.newSelection(
								SRBMetaDataSet.FILE_NAME),
								MetaDataSet.newSelection(SRBMetaDataSet.DIRECTORY_NAME)
							};
					MetaDataRecordList[] rl = srbFileSystem.query(conditions, selects);
					rl = MetaDataRecordList.getAllResults(rl);
					if (rl != null) { //Nothing in the database matched the query
							for (int i = 0; i < rl.length; i++) {
									System.out.print("\n" + rl[i].getValue(1) +
																		SRBFile.PATH_SEPARATOR +
																		rl[i].getValue(0));
							}
					}
			} else { //no recursive ness
					if (! (cmd.hasOption('c') || cmd.hasOption('C'))) { //no cmdline options
							if (args.length == 1) {
									srbFileSystem = new SRBFileSystem();
									srbFile = new SRBFile(srbFileSystem, args[0]);
									list = srbFile.list();
									printList(list);
							} else if (args.length == 0) {
									srbFileSystem = new SRBFileSystem();
									srbFile =
										new SRBFile(srbFileSystem,
																srbFileSystem.getHomeDirectory());
									list = srbFile.list();
									printList(list);
							}
					} else { //use only cmd line options
							if (cmd.hasOption('C')) {
									srbFileSystem = new SRBFileSystem();
									srbFile = new SRBContainer(srbFileSystem,
										cmd.getOptionValue("C"));
									list = srbFile.list();
									printList(list);
							}
							if (cmd.hasOption('c')) {
									srbFileSystem = new SRBFileSystem();
									srbFile = new SRBFile(srbFileSystem,
																				cmd.getOptionValue("c"));
									list = srbFile.list();
									printList(list);
							}
					}
			}
			if (cmd.hasOption('h')) {
					printHelp(options);
			}
		} catch (SRBException e) {
			//Get the standard error string
			String bar = e.getStandardMessage();
			System.out.println("Standardized SRB Server Error Message: " + bar);
			//The original error message is still available through
			System.out.println("\nGridTools Error Message: " + e.getMessage());
			e.printStackTrace();
		} catch (Throwable e) {
			e.printStackTrace();
		}
		System.exit(0);
	}

	public static void printList(String header, String[] list)
	{
		System.out.println(header);
		printList(list);
	}

	public static void printList(String[] list)
	{
		if (list != null) {
			for (int i = 0; i < list.length; i++) {
					System.out.println(list[i]);
			}
		}
	}

	public static void printHelp(Options opts)
	{
		HelpFormatter formatter = new HelpFormatter();
		formatter.printHelp("Sls [Ccrh] <collectionName|containerName>", opts);
	}
*/
}
