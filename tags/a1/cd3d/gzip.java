import java.io.*;
import java.util.zip.*;

public class gzip{
  public static void main( String args[] ){
    if ( args.length!=2 ){
      System.err.println( "Useage: java gzip <input file> <output file>" );
      System.exit(0);
    }
    byte b;
    FileInputStream fis = null;
    BufferedInputStream bis = null;
    FileOutputStream fos = null;
    GZIPOutputStream gzos = null;
    BufferedOutputStream bos = null;
    try{
      fis = new FileInputStream( args[0] );
      bis = new BufferedInputStream( fis, 1024*256 );
      fos = new FileOutputStream( args[1] );
      gzos = new GZIPOutputStream( fos );
      bos = new BufferedOutputStream( gzos, 1024*256 );
      int value = 0;
      while( value != -1 ){
	value = bis.read();
	bos.write( value );
      } 
      bis.close();
      fis.close();
      bos.flush();
      bos.close();
      gzos.close();
      fos.close();
      System.exit(0);
    }
    catch( IOException ioe ){
      System.err.println( ioe );
    }
  }
}
