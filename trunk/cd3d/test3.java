import javax.vecmath.*;

public class test3{
  public static void main( String args[] ){
    Vector3f v1 = new Vector3f( 1, 1, 1 );
    Vector3f v2 = new Vector3f( 1, 0, 0 );
    Vector3f v3 = new Vector3f();
    v3.scaleAdd( 4, v1, v3 );
    System.out.println( "v3 = " + v1 + " * 4 = " + v3 );
    v3.scaleAdd( 2, v2, v3 );
    System.out.println( "(4*v1) + (2*v2) = " + v3 );
  }
}
