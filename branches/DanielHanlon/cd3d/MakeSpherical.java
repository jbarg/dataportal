import java.io.*;

public class MakeSpherical{

  public static void main( String args[] ) throws IOException{
    if ( args.length < 2 ){
      System.out.println( "Usage: java MakeSpherical " +
			  "<file-name> <dimension>" );
      System.exit( 0 );
    }

    int tempint = Integer.parseInt( args[1] );    
    float dimension = (float)tempint;
    float centre = dimension/2;
    float scale = centre * centre * centre;

    PrintWriter pw = fileOut( args[0] );

    pw.println( "Almost spherical potential" );
    String temp = dimension + "\t" + dimension + "\t" + dimension;
    pw.println( temp );
    pw.println( "0.0\t0.0\t0.0" );
    pw.println( "0.0\t0.0\t1.0" );
    pw.println( "0.0\t1.0\t0.0" );
    pw.println( "1.0\t0.0\t0.0" );

    // Make data
    int count = 1;
    for ( int x=0; x<dimension; x++ ){
      for ( int y=0; y<dimension; y++ ){
	for ( int z=0; z<dimension; z++ ){
	  pw.print( ( scale /( ( centre - x )*( centre - x ) +
			       ( centre - y )*( centre - y ) +
			       ( centre - z )*( centre - z ) + 1 ) ) );
	  if ( ( count % 5 ) == 0 ){
	    pw.println();
	  } else {
	    pw.print( "\t" );
	  }
	  count++;
	}
      }
    }

    pw.flush();
    pw.close();
  }
    
  public static PrintWriter fileOut( String s ) throws IOException{
    File op = new File( s );
    FileOutputStream fos = new FileOutputStream( op );
    PrintWriter pw = new PrintWriter( fos );
    return pw;
  }
}  
