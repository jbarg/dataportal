import javax.media.j3d.*;
import javax.vecmath.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import java.io.*;
import com.sun.j3d.utils.geometry.*;

public class Vertex{
  Vertex next;
  Vertex by[] = new Vertex[6];
  Vector3f normal;
  Point3f point;
  //  Color3f color;
  //  int alpha;
  int i, j, k;
  int norm;
  int thisVertex;
  public int thisPosition;
  public static int usedVertexCount;
  public boolean used = false;
  static CrystalData cd;
  static Vertex list, start;
  static int vertexCount;
  static boolean iret;
  static float threshold, surfaceAverage;
  static int v_delta[][][] = { { { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 }, { 0, 1, 0 },
				 { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, { 1, 0, 0 },
				 { 0, 0, 0 }, { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 } },
			       { { 1, 0, 0 }, { 1, 0, 1 }, { 1, 1, 1 }, { 1, 1, 0 },
				 { 0, 1, 0 }, { 0, 1, 1 }, { 1, 1, 1 }, { 1, 1, 0 },
				 { 0, 0, 1 }, { 0, 1, 1 }, { 1, 1, 1 }, { 1, 0, 1 } } };

  public static void setStaticInfo( CrystalData c, float t, float sa ){
    surfaceAverage = sa;
    threshold = t;
    cd = c;
    vertexCount = 0;
    iret = false;
    list = null;
    start = null;
  }

  private Vertex( int i, int j, int k, Point3f p, Vector3f n ){
    this.i = i;
    this.j = j;
    this.k = k;
    this.point = p;
    this.normal = n;
  }
  
  public Vertex( int i, int j, int k ){
    this( i, j, k, new Point3f(), new Vector3f() );
    next = list;
    start = this;
    thisVertex = vertexCount++;
    interpVolume2();
  }
  
  public Vertex( Point3i dp ){
    this( dp.x, dp.y, dp.z );
  }

  private void interpVolume2(){
    int count = 0;
    Point3f temp = new Point3f();
    for ( int iEdge = 0; iEdge<12; iEdge++) {
      float val1, val2;
      val1 = (float)cd.data[ i + v_delta[0][iEdge][0] ][ j + v_delta[0][iEdge][1] ][ k + v_delta[0][iEdge][2] ];
      val2 = (float)cd.data[ i + v_delta[1][iEdge][0] ][ j + v_delta[1][iEdge][1] ][ k + v_delta[1][iEdge][2] ];
  
      Point3f v1, v2;
      v1 = cd.getGridValue( i + v_delta[0][iEdge][0], j + v_delta[0][iEdge][1], k + v_delta[0][iEdge][2] );
      v2 = cd.getGridValue( i + v_delta[1][iEdge][0], j + v_delta[1][iEdge][1], k + v_delta[1][iEdge][2] );
    
      // at this point we want the normal to hold a weighted sum of edge vectors. This will set the
      // orientation of the mean face normals calculated later.
    
      float df = val2 - val1;
      float inc = df / v1.distance( v2 ); // sign =  DATA VALUE CHANGE PER POSITION INCREMENT
      temp.sub( v2, v1 );
      temp.scale( inc );
      normal.add( temp );

      // IF v1 AND v2 ARE ON DIFFERENT SIDES OF THE THRESHOLD
      if ( ( val1 - threshold ) * ( val2 - threshold ) <= 0.0 ){
	float t;
	// try and trap numerical errors
	if ( df == 0.0 || Math.abs( df/val2 ) < 0.00001 ){
	  t = 1.0f;
	} else {
	  t = ( threshold - val1 ) / df;
	}
	count++;
      
	v1.interpolate( v2, t ); // v1 = v1 + t*(v2-v1);
	point.add( v1 );
	// System.out.println( "Point total = " + point + ", count = " + count );
      } // End of ( if edge crosses threshold ) section
    }
    
    // -> VT IS THE POSITION OF THE POINT TO BE USED IN FORMING THE ISO-SURFACE.
    point.scale( 1f/(float)count );
    normal.normalize();
    list = this;
    norm = 1;
    //    System.out.println( "Vertex point found: location=" + point + " normal=" + normal );
  } // END OF INTERP_VOLUME2

  public static void connectPoints(){
    Vertex look, v;
    Vertex pArray[][][] = new Vertex[ cd.data.length ][ cd.data[0].length ][ cd.data[0][0].length ];
    
    // SET THE LOCATIONS IN parray TO THE POSITIONS IN THE DATA OF THE ELEMENTS OF LIST.
    for ( v=Vertex.start; v!=null; v=v.next ){
      pArray[ v.i ][ v.j ][ v.k ] = v;
    }

    for ( v=Vertex.start; v != null; v = v.next ){
      look = pArray[ v.i+1 ][ v.j ][ v.k ];
      if( look != null ){
	v.by[3] = look;
	look.by[0] = v;
      }
      look = pArray[ v.i ][ v.j+1 ][ v.k ];
      if( look != null ){
	v.by[4] = look;
	look.by[1] = v;
      }
      look = pArray[ v.i ][ v.j ][ v.k+1 ];
      if( look != null ){
	v.by[5] = look;
	look.by[2] = v;
      }
    }
  }
  
  //  public static PointArray getPointArray(){
  //    System.out.println( vertexCount + " vertices found." );
  //    PointArray pa = new PointArray( vertexCount, GeometryArray.COORDINATES | GeometryArray.NORMALS );
  //    pa.setCapability( GeometryArray.ALLOW_COORDINATE_READ );
  //    pa.setCapability( GeometryArray.ALLOW_NORMAL_READ );
  //    for ( Vertex v=Vertex.start; v != null; v = v.next ){
  //      pa.setCoordinate( v.thisVertex, v.point );
  //      pa.setNormal( v.thisVertex, v.normal );
  //    }
  //    return pa;
  //  }
  
  public static Point3f[] getCoordinates(){
    Point3f[] p = new Point3f[ usedVertexCount ];
    for ( Vertex v=Vertex.start; v!=null; v=v.next ){
      p[ v.thisPosition ] = v.point;
    }
    return p;
  }
  
  public static Vector3f[] getNormals(){
    Vector3f[] n = new Vector3f[ usedVertexCount ];
    for ( Vertex v=Vertex.start; v!=null; v=v.next ){
      n[ v.thisPosition ] = v.normal;
    }
    return n;
  } 
  
  public static void estimateNormals(){
    Vertex v;
    for ( v=Vertex.start; v != null; v = v.next ){
      for ( int i=0; i<3; i++ ){
	// In each plane...
	// If all three vertices exist in the ith plane in the direction of the origin.
	if ( doCheckLoop( v, i ) ){ 
	  iret = ( doNormLoop( v, i ) || iret );
	  // Calculate a normal by triangle edge cross products of the points in the 2 triangles
	  // defined by loop[][] in the ith plane. Add the normal to the points of which that triangle
	  // is comprised, inverting it where necessary depending on existing
	  // normal values. A count is kept of the number of inversions. If some, but not
	  // all of the normals have been inverted in either of the two triangles, then return
	  // 1, otherwise 0.
	}
      }
    }
    for ( v=Vertex.start; v != null; v = v.next ){
      // v->norm is the number times that a normal value has been added to v->n
      // Divide by the number of normal additions.
      if ( v.norm > 1 ) v.normal.scale( 1f/(float)v.norm );
    }
  }
  
  static int loop[][] = { { 0, 1, 3, 4 }, { 0, 2, 3, 5 }, { 1, 2, 4, 5 } };
  private static boolean doCheckLoop( Vertex v, int i ){
    Vertex temp = v;
    for ( int j=0; j<3; j++ ){
      if ( ( temp = temp.by[loop[i][j]] ) == null ) return false;
    }
    return true;
  }
  
  private static boolean doNormLoop( Vertex v, int i ){
    Vertex va, vb, vc, vd;
    int j = 0;
    Vector3f n1 = new Vector3f(), n2 = new Vector3f();
    boolean flick;

    va = v;
    vb = va.by[loop[i][j++]];
    vd = vb.by[loop[i][j++]];
    vc = vd.by[loop[i][j++]];
    
    flick = false;

    // Make n1 the normal generated by the normalised cross product of (vb-va)x(vc-va)
    // If the points are NOT so close or at such a small angle that calculating this normal is meaningless...
    if ( getNormal( va.point, vb.point, vc.point, n1 ) ){
      // ******************************************************************************************************
      // Add new normal ( from cross-product ) to current normal, inverting if in opposite directions.
      // j is the number of new normals that have had to be inverted in this group
      // ******************************************************************************************************
      // If norm not set then va->n = n1 ( or -n1 if va->n and n1 are >90 degrees apart )
      // If norm is set then va->n = vax->n + n1 ( or va->n - n1 if va->n and n1 are >90 degrees apart )
      // i.e. add n1 if the normals are in the same direction, subtract it if they are in opposition.
      j  = nadd( va.normal, va.normal, va.norm, n1); // returns 0 if angle between va->n and n1 <=90, 1 if > 90
      va.norm++;
      j += nadd( vb.normal, vb.normal, vb.norm, n1);
      vb.norm++;
      j += nadd( vc.normal, vc.normal, vc.norm, n1);
      vc.norm++;
      if ( (j != 0 && j !=3) ) flick = true;
      // If SOME BUT NOT ALL of the point normals are in the opposite direction to the cross product normal
      // then set flick = 1
    }
    
    // Calculate a new normal from (vc-vb)x(vd-vb) normalised, and put it in n2.
    // Again, add the new normal onto possible existing normals, negating where necessary.
    if ( getNormal( vb.point, vc.point, vd.point, n2 ) ){
      j = nadd( vb.normal, vb.normal, vb.norm, n2 );
      vb.norm++;
      j += nadd( vc.normal, vc.normal, vc.norm, n2);
      vc.norm++;
      j += nadd( vd.normal, vd.normal, vd.norm, n2);
      vd.norm++;
      if ( j != 0 && j !=3 ) flick = true;
    }
    // If SOME BUT NOT ALL of the point normals are in the opposite direction to the cross product normal
    // then return flick = true
    return flick;
  }
  
  private static boolean getNormal( Point3f p, Point3f p1, Point3f p2, Vector3f n ){
    Vector3f temp1 = new Vector3f(), temp2 = new Vector3f();
    temp1.sub( p1, p );
    temp2.sub( p2, p );
    n.cross( temp1, temp2 );
    if( n.dot( n ) < 1.0e-8 ) return false;
    n.normalize();
    return true;
  }
  
  private static int nadd( Vector3f sum, Vector3f n1, int count, Vector3f n2 ){
    boolean temp = false;   
    if( count == 0 ){  //  align with guess only
      temp = ( n1.dot( n2 ) < 0f );
      if ( temp ){
	sum.negate( n2 );
	return 1;
      } else {
	sum.set( n2 );
	return 0;
      }
    } else {
      if ( temp ){
	sum.sub( n1, n2 );
	return 1;
      }else{
	sum.add( n1, n2 );
	return 0;
      }
    }
  }
  
  static final int NORM_PLUS = 1;  // defined by the gradient direction
  static final int NORM_MINUS = -1;
  static final int NORM_CONVEX = 2;  // defined by the average curvature
  static final int NORM_CONCAVE = -2;    
  static final int NORM_TOLIM = 3;  // defined by limiting values of the grid
  static final int NORM_FROMLIM  = -3;
  private static int normCount;
  private static float normAcc;

  public static void flipNormals( int mode ){
    boolean swap = false;
    normCount = 0;
    normAcc = 0;
    Vertex v;

    switch( mode ){
    case NORM_PLUS:
      swap = false;
      break;
    case NORM_MINUS:
      swap = true;
      break;
    case NORM_CONCAVE:
    case NORM_CONVEX:
      for ( v=Vertex.start; v != null; v = v.next ){
	for ( int i=0; i<3; i++ ){
	  // If all the backwards directions neighbouring cells in this plane are filled....
	  if ( doCheckLoop( v, i ) ){
	    doNormLoopAcc( v, i );
	  } else {
	    //	    System.out.println( "Vertex at ( " + v.i + ", " + v.j + ", " + v.k + ") not used."  );
	  }
	}
      }
      swap = ( ( normAcc/normCount ) > 0.0 );
      if ( mode == NORM_CONVEX ) swap = !swap;
      break;
    case NORM_TOLIM:
    case NORM_FROMLIM:
      swap = false;
      if ( threshold < surfaceAverage ) swap = true;
      if ( mode == NORM_FROMLIM ) swap = !swap;
      break;
    }
    
    // swap the normals
    if ( swap ){
      for ( v=Vertex.start; v != null; v = v.next ){
	v.normal.negate();
      }
    }
  }

  private static void doNormLoopAcc( Vertex v, int i ){
    Vertex va, vb, vc, vd;
    Vector3f n1, centre = new Vector3f(), temp = new Vector3f();
    int j = 0;    
    
    va = v;
    vb = va.by[loop[i][j++]];
    vd = vb.by[loop[i][j++]];
    vc = vd.by[loop[i][j++]];
    
    // Are these points sufficiently far apart to make a generated normal meaningful?
    // Put the normal in n1.
    n1 = new Vector3f();
    if ( getNormal( va.point, vb.point, vc.point, n1 ) ){
      // Find the average of the positions i.e. the centre of the triangle.
      centre.add( va.point, vb.point );
      centre.add( vc.point );
      centre.scale( (float)1f/3 );
          
      // Find vector from vertex point to centre, dot product it with the normal at that vertex point and
      // add it to the accumulator along with incrementing count by the appropriate value.
      temp.sub( centre, va.point );
      // Dot product it with the normal for that vertex point.
      normAcc += temp.dot( va.normal );
      temp.sub( centre, vb.point );
      normAcc += temp.dot( vb.normal );
      temp.sub( centre, vc.point );
      normAcc += temp.dot( vc.normal );
      normCount += 3;
    }
    if ( getNormal( vb.point, vc.point, vd.point, n1 ) ){
      centre.add( vb.point, vc.point );
      centre.add( vd.point );
      centre.scale( (float)1f/3 );
      
      temp.sub( centre, vb.point );
      normAcc += temp.dot( vb.normal );
      temp.sub( centre, vc.point );
      normAcc += temp.dot( vc.normal );
      temp.sub( centre, vd.point );
      normAcc += temp.dot( vd.normal );
      normCount += 3;
    }
  }

  public static VertexStore makeMesh(){
    Vertex v;
    int ia, ib, ic, id, i, j;
    VertexStore s;
    VertexStore.initialise( vertexCount );
    usedVertexCount = 0;
    for ( v=Vertex.start; v!=null; v=v.next ){    
      for( i=0; i<3; i++ ){	
	if ( doCheckLoop( v, i ) ){
	  // If in the ith plane (0->x,1->y,2->z) the three vertices exist that are required to
	  // form a pair of triangles in the direction of the origin.....
	  j = 0;
	  if ( !v.used ){
	    v.used = true;
	    v.thisPosition = usedVertexCount++;
	  }
	  ia = v.thisPosition;
	  
	  v = v.by[ loop[i][j++] ];
	  if ( !v.used ){
	    v.used = true;
	    v.thisPosition = usedVertexCount++;
	  }
	  ib = v.thisPosition;

	  v = v.by[ loop[i][j++] ];
	  if ( !v.used ){
	    v.used = true;
	    v.thisPosition = usedVertexCount++;
	  }
	  ic = v.thisPosition;

	  v = v.by[ loop[i][j++] ];
	  if ( !v.used ){
	    v.used = true;
	    v.thisPosition = usedVertexCount++;
	  }
	  id = v.thisPosition;
	  v = v.by[ loop[i][j++] ];

	  s = new VertexStore( ia, ib, ic, id );
	}
      }
    }
    return VertexStore.end;
  }

  public static IndexedGeometryArray getGeometry() throws NoSurfaceException{
    VertexStore s = makeMesh();
    //
    int vertexStoreCheck = 0;
    for ( Vertex v=Vertex.start; v!=null; v=v.next ){
      for ( int i=0; i<3; i++ ){	
	if ( doCheckLoop( v, i ) )vertexStoreCheck++;
      }
    }
    //    System.out.println( "doCheckLoop has identified " + vertexStoreCheck + " quads." );
    //    System.out.println( "VertexStore reports " + VertexStore.getIndexCount()/4 + " quads " );
    if ( usedVertexCount < 4 ) throw new NoSurfaceException();
    Vertex v;
    Point3f coords[] = new Point3f[ usedVertexCount ];
    Vector3f norms[] = new Vector3f[ usedVertexCount ];    
    // How many?
    int vCount = 0;
    for ( v=Vertex.start; v!=null; v=v.next ){    
      if ( v.used ){
	//	if ( v.thisPosition >= usedVertexCount ) System.out.println( "Indexing problem " );
	coords[ v.thisPosition ] = v.point;
	norms[ v.thisPosition ] = v.normal;
      }
    }
    //    System.out.println( usedVertexCount + " vertices put into arrays for iqa." );

    //    System.out.println( "Now generating the quad array" );
    IndexedQuadArray iqa = new IndexedQuadArray( usedVertexCount,
						 QuadArray.COORDINATES |
						 QuadArray.NORMALS,
						 VertexStore.getIndexCount() );
    iqa.setCapability( IndexedGeometryArray.ALLOW_COORDINATE_READ );
    iqa.setCapability( IndexedGeometryArray.ALLOW_FORMAT_READ );
    iqa.setCapability( IndexedGeometryArray.ALLOW_COORDINATE_INDEX_READ );
    iqa.setCapability( IndexedGeometryArray.ALLOW_NORMAL_READ );
    iqa.setCapability( IndexedGeometryArray.ALLOW_NORMAL_INDEX_READ );
    iqa.setCapability( IndexedGeometryArray.ALLOW_COUNT_READ );
    iqa.setCoordinates( 0, coords );
    iqa.setNormals( 0, norms );
    int indices[] = VertexStore.getQuadArrayIndices();
    // Check validity of indexing...
    for ( int j, i=0; i<indices.length; i++ ){
      j = indices[i];
      //      if ( j > usedVertexCount ) System.out.println( "j="+j+", usedVertexCount="+usedVertexCount );
      //      try{
      //	if ( coords[j] == null ) System.out.println( "Check failed at 463" );
      //	if ( norms[j] == null ) System.out.println( "Check failed at 464" );
      //      } catch ( ArrayIndexOutOfBoundsException aioob ){
      //	aioob.printStackTrace();
      //	System.out.println( "j="+j+" i="+i );
      //	indices[i] = indices[i-1];
      //      }
    }
    iqa.setCoordinateIndices( 0, indices );
    iqa.setNormalIndices( 0, indices );
    return iqa;
  }
}

class VertexStore{
  private static int indexCount;
  private VertexStore previous;
  public static VertexStore end;
  private int vertices[];

  static void initialise( int max ){
    end = null;
    indexCount = 0;
  }

  public VertexStore( int i0, int i1, int i2, int i3 ){
    previous = end;    
    end = this;
    indexCount += 4;
    vertices = new int[4];
    vertices[0] = i0;
    vertices[1] = i1;
    vertices[2] = i2;
    vertices[3] = i3;
  }

  public static int getIndexCount(){
    return indexCount;
  }

  public static int[] getQuadArrayIndices(){
    int qai[] = new int[ indexCount ];
    int count=0;
    for ( VertexStore s = end; s!=null; s = s.previous ){
      for ( int i=0; i<4; i++ ){
	qai[count++] = s.vertices[i];
      }
    }
    return qai;
  } 
}



    

