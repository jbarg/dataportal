public class test{

  public static void main( String args[] ){
    for ( int i=0, j=0; i<12; j++ ){
      System.out.println( "i=" + Integer.toString( i++ ) + ", j=" + Integer.toString( j ) );
      System.out.println( "i=" + Integer.toString( i++ ) + ", j=" + Integer.toString( j ) );
      System.out.println( "i=" + Integer.toString( i++ ) + ", j=" + Integer.toString( j ) );
    }
  }
}
