import edu.sdsc.grid.io.GeneralFile;
import edu.sdsc.grid.io.GeneralRandomAccessFile;
import edu.sdsc.grid.io.FileFactory;



import java.net.URI;


public class FactoryTest
{
//----------------------------------------------------------------------
// Main
//----------------------------------------------------------------------
	/**
	 * Testing FileFactory
	 */
	public static void main(String args[])
	{
		/**
		 * The GeneralFile class is an abstract super class that can represent
		 * a filepath on any supported file system, local or remote.
		 * Use it as you would a regular java.io.File object.
		 */
		GeneralFile file = null;
		GeneralFile file2 = null;


		/**
		 * The GeneralRandomAccessFile class is used in much the same manner as
		 * the java.io.RandomAccessFile class.
		 */
		GeneralRandomAccessFile randomAccessFile = null;



		try{
System.out.println("\n Create a GeneralFile object from a uri.");
/*
local file uri syntax:
file:/path
for example on windows:
file:/c:/backups/testFile


srb file uri syntax:
srb://userName.mdasDomain:password@host:port/path
for example:
srb://testuser.sdsc:TESTUSER@srb.sdsc.edu:5544/home/testuser.sdsc/testFile
*/
			if (args.length == 2) {
				file = FileFactory.newFile( new URI( args[0] ) );
				file2 = FileFactory.newFile( new URI( args[1] ) );
			}
			else {
				throw new IllegalArgumentException(
					"\nUsage: java FactoryTest urlString urlString");
			}




System.out.println( "\n Make a new directory from the GeneralFile object." );
			file.mkdir();




System.out.println(
	"\n Create a new file in the new directory with the name \"child\"." );
			file = FileFactory.newFile( file,	"child" );
			file.createNewFile();




System.out.println( "\n Test the random access to the new file." );
			randomAccessFile = FileFactory.newRandomAccessFile( file, "rw" );
			randomAccessFile.write( new String(
				"This is a test file. It is ok to delete.\n") );

			randomAccessFile.seek( 0 );
			byte[] buffer = new byte[1000];
			int bytesRead = randomAccessFile.read( buffer );
			String fileContents = new String( buffer, 0, bytesRead );
			System.out.print( fileContents );

			randomAccessFile.close();




System.out.println( "\n Copy the file named \"child\" to the file given as "+
	"the second argument.");
			file.copyTo( file2 );





System.out.println("\n List the directory created earlier.");
			String[] dirList = file.getParentFile().list();
			if (dirList != null) {
				for(int i=0;i<dirList.length;i++)
					System.out.println(dirList[i]);
			}




System.out.println("\n Remove the file just created.");
			file.delete();




System.out.println("\n Remove the directory just created.");
			file.getParentFile().delete();




System.out.println("\n Exit.");
			file = null;
		}
		catch ( Throwable e ) {
			e.printStackTrace();
		}


		System.exit(0);
	}
}
