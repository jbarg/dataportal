public class test7{

  public static void main( String args[] ){
    String s = "thisis the filename.str";
    String extension = s.substring( s.lastIndexOf( '.' )+1, s.length() );
    System.out.println( extension );
  }
}
