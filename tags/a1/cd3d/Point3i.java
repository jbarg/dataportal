/* Utility class to provide an object to refer
 * to a position in the 3D array of data. It is
 * designed to be similar in functionality to
 * the Point3d, Point3f classes in the package
 * javax.vecmath which implement the same idea
 * using doubles and floats respectively. %%\label{point3i}%%*/
public class Point3i extends Object{
  public int x, y, z;
  
  /* Returns the binary number which identifies
   * the corner of a cube where x,y,z = 1,0
   * as used in CrystalIsoSurface. */
  public static int p3i2c( int x, int y, int z ){
    int ret = x + y*2 + z*4;
    if ( ret > 7 ){
      System.out.println( "Invalid c setting in Point3i: x=" +
                          x + " y=" + y + " z=" + z );
    }
    return ret;
  }

  /* Returns a new instance of Point3i
   * containing the offset from the cube
   * origin implied by the binary number c. */
  public static Point3i c2p3i( int c ) {
    return new Point3i( ( c&1 ), ( ( c&2 ) >>> 1 ), ( ( c&4 ) >>> 2 ) );
  }
  
  /* Default constructor setting all to zero. */
  public Point3i() {
    this( 0, 0, 0 );
  }

  /* Take another Point3i to initialise this by copying its
   * contents. */
  public Point3i( Point3i pass ) {
    this( pass.x, pass.y, pass.z );
  }

  /* Provide the `co-ordinates' for this instance. */
  public Point3i( int x, int y, int z ) {
    set( x, y, z );
  }

  /* Setup this object from a binary cube corner number. */
  public Point3i( int c ){
    setC( c );
  }

  /* Set the three co-ordinates. */
  public void set( int x, int y, int z ){
    this.x = x;
    this.y = y;
    this.z = z;
  }

  /* Set the co-ordinate from another Point3i object's values. */
  public void set( Point3i p3i ){
    set( p3i.x, p3i.y, p3i.z );
  }

  /* Give the object the co-ordinates
   * referring to a cube corner as defined by the binary number c. */
  public void setC( int c ){
    set( 0, 0, 0 );
    addC( c );
  }

  /* Return the cube-corner defined by this point. */
  public int getC(){
    return p3i2c( x, y, z );
  }

  /* Add a cube corner offset to this co-ordinate. */
  public void addC( int c ){
    x += ( c&1 );
    y += ( ( c&2 ) >>> 1 );
    z += ( ( c&4 ) >>> 2 );
  }

  /* Subtract another Point3i from this. */
  public void sub( Point3i b ){
    x -= b.x;
    y -= b.y;
    z -= b.z;
  }

  /* Provide a standard String output of this object for printing. */
  public String toString(){
    return( "Point3i: x="+x+" y="+y+" z="+z );
  }
}
