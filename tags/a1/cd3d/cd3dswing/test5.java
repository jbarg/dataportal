public class test5{
  public static void main( String args[] ){
    System.out.println( (int)'?' );
    String s = "e-3";
    double d = Double.parseDouble( "1" + s );
    System.out.println( "String is " + s );
    System.out.println( "As a double this is " + d );
    System.out.println( "As an integer this is " + (int)d );
  }
}
