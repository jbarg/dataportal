
import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.*;
import java.io.*;
import java.net.*;


public class Srm
{
//----------------------------------------------------------------------
// Main
//----------------------------------------------------------------------
	public static boolean recursiveDelete(GeneralFile file)
	{
		boolean success = true;

		if (file.isDirectory()) {
			//recursive delete
			GeneralFile fileList[] = (GeneralFile[]) file.listFiles();

			if (fileList != null) {
				for (int i=0;i<fileList.length;i++) {
					success = Srm.recursiveDelete(fileList[i]);
				}
			}
		}

		if (success)
			success	= file.delete();
		else
			file.delete();

		return success;
	}



	/**
	 * Testing
	 */
	public static void main(String args[])
	{
		SRBFileSystem srbFileSystem = null;

		GeneralFile srbFile = null;

		try{
			srbFileSystem = new SRBFileSystem( );

			if ((args.length >= 2) && (args[0].equals("-c"))) {
				for (int i=1;i<args.length;i++) {
					srbFile = new SRBContainer( srbFileSystem, args[i] );

					if (!Srm.recursiveDelete(srbFile))
						System.out.println( "Failed to delete "+srbFile);
				}
			}
			else if ((args.length == 2) && (args[0].equals("-uri"))) {
				srbFile = FileFactory.newFile( new URI( args[1] ) );
				if (!Srm.recursiveDelete(srbFile))
					System.out.println( "Failed to delete "+srbFile);
			}
			else if (args.length >= 1) {
				for (int i=0;i<args.length;i++) {
					srbFile = new SRBFile( srbFileSystem, args[i] );
					if (!Srm.recursiveDelete(srbFile))
						System.out.println( "Failed to delete "+srbFile);
				}
			}
			else {
//TODO the -c doesn't even seem necessary
				throw new IllegalArgumentException(
					"\nUsage: Srm fileName/directory/containerName (fileName fileName ... fileName)");
//					"\nUsage: Srm -c containerName (containerName containerName ... containerName)");
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
}
