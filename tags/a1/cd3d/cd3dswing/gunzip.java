import java.io.*;
import java.util.zip.*;

public class gunzip{
  public static void main( String args[] ){
    if ( args.length!=2 ){
      System.err.println( "Useage: java gunzip <input file> <output file>" );
      System.exit(0);
    }
    byte b;
    FileInputStream fis = null;
    BufferedInputStream bis = null;
    GZIPInputStream gzis = null;
    FileOutputStream fos = null;
    BufferedOutputStream bos = null;
    try{
      fis = new FileInputStream( args[0] );
      bis = new BufferedInputStream( fis, 1024*256 );
      gzis = new GZIPInputStream( bis );
      fos = new FileOutputStream( args[1] );
      bos = new BufferedOutputStream( fos, 1024*256 );
      int value = 0;
      while( value != -1 ){
	value = gzis.read();
	bos.write( value );
      }
      gzis.close();
      bis.close();
      fis.close();
      bos.flush();
      bos.close();
      fos.close();
      System.exit(0);
    }
    catch( IOException ioe ){
      System.err.println( ioe );
    }
  }
}
