
public class test4{
  
  public static void main( String args[] ){
    String S[] = new String[3];
    S[0] = "5.3423489789E-32";
    S[1] = "2.382789789E+32";
    S[2] = "3.4352352343E-000";
    float temp;
    int t;
    for ( int j=0; j<3; j++ ){
      System.out.println( "For string " + S[j] );
      String s = S[j].substring( ( S[j].lastIndexOf( 'E' ) + 1 ), S[j].length() );
      if ( ( t = s.indexOf( '+' ) ) != -1 ) s = s.substring( t+1, s.length() ); 
      System.out.println( "s=" + s );
      int i = Integer.parseInt( s );
      System.out.println( "i=" + i );
      s = S[j].substring( 0, S[j].lastIndexOf( 'E' ) );
      System.out.println( "New s = " + s );
      temp = Float.parseFloat( s );
      temp*= Math.pow( 10, i );
      System.out.println( "temp = " + temp );
    }
  }
}



	

