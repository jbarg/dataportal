import java.io.*;

public class test2
{
  public static void main( String args[] ) throws Exception{
    if ( args.length < 1 ) throw new Exception( "Need a filename as a parameter" );
    System.out.println( "Enter a number" );
    BufferedReader br = new BufferedReader( new InputStreamReader( System.in ) );
    String s = br.readLine();
    double d = Double.parseDouble( s );
    System.out.println( "You typed: " + Double.toString( d ) );
    // now let's try a file
    StreamTokenizer st = new StreamTokenizer(
			   new BufferedReader(
			     new InputStreamReader(
			       new FileInputStream( new File( args[0] ) ) ) ) );
    //st.parseNumbers();
    st.resetSyntax();
    st.whitespaceChars( ' ', ' ' );
    st.eolIsSignificant( true );
    st.wordChars( 33, 255 );
    boolean carryOn = true;
    int dt;
    while ( carryOn ){
      switch( dt = st.nextToken() ){
      case StreamTokenizer.TT_NUMBER:
	System.out.println( "Number: " + Double.toString( st.nval ) );
	break;
      case StreamTokenizer.TT_WORD:
	System.out.println( "Word: " + st.sval );
	break;
      case StreamTokenizer.TT_EOL:
	System.out.println( "EOL" );
	break;
      case StreamTokenizer.TT_EOF:
	System.out.println( "End of file..." );
	carryOn = false;
	break;
      default:
	System.out.println( "Something else...with value" + Integer.toString( dt ) );
	break;
      }
    }
  }
}
