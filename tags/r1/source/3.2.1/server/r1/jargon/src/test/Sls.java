
import edu.sdsc.grid.io.srb.*;
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
					"\nUsage: Sls fileName/directory\nUsage: Sls [-c] containerName");
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
}
