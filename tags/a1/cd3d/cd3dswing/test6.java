import javax.vecmath.*;
import javax.media.j3d.*;
import com.sun.j3d.utils.geometry.*;
import java.awt.*;

public class test6{
  public static void main( String args[] ){
    Color3f c3f[][][] = new Color3f[10][10][10];
    for ( int i=0; i<10; i++ ){
      for ( int j=0; j<10; j++ ){
	for ( int k=0; k<10; k++ ){
	  c3f[i][j][k] = new Color3f( 1f/i, 1f/j, 1f/k );
	}
      }
    }
    Material mat = new Material();
    Color3f tc3f;
    int c[] = new int[12];
    for ( int e=0; e<100000; e++ ){
      for ( int a = 0; a<c.length; a++ ){ 
	c[a] = (int)( Math.random() * 10 );
      }
      tc3f = new Color3f( c3f[ c[0] ][ c[1] ][ c[2] ] );
      mat.setAmbientColor( tc3f );
      tc3f = new Color3f( c3f[ c[3] ][ c[4] ][ c[5] ] );
      mat.setDiffuseColor( tc3f );
      tc3f = new Color3f( c3f[ c[6] ][ c[7] ][ c[8] ] );
      mat.setEmissiveColor( tc3f );
      tc3f = new Color3f( c3f[ c[9] ][ c[10] ][ c[11] ] );
      mat.setSpecularColor( tc3f );
      //
      mat.getAmbientColor( tc3f );
      if ( ( tc3f.x != c3f[ c[0] ][ c[1] ][ c[2] ].x ) ||
	   ( tc3f.y != c3f[ c[0] ][ c[1] ][ c[2] ].y ) ||
	   ( tc3f.z != c3f[ c[0] ][ c[1] ][ c[2] ].z ) ){
	System.out.println( "Ambient colour changed." );
      }
      mat.getDiffuseColor( tc3f );
      if ( ( tc3f.x != c3f[ c[3] ][ c[4] ][ c[5] ].x ) ||
	   ( tc3f.y != c3f[ c[3] ][ c[4] ][ c[5] ].y ) ||
	   ( tc3f.z != c3f[ c[3] ][ c[4] ][ c[5] ].z ) ){
	System.out.println( "Diffuse colour changed." );
      }
      mat.getEmissiveColor( tc3f );
      if ( ( tc3f.x != c3f[ c[6] ][ c[7] ][ c[8] ].x ) ||
	   ( tc3f.y != c3f[ c[6] ][ c[7] ][ c[8] ].y ) ||
	   ( tc3f.z != c3f[ c[6] ][ c[7] ][ c[8] ].z ) ){
	System.out.println( "Emissive colour changed." );
      }
      mat.getSpecularColor( tc3f );
      if ( ( tc3f.x != c3f[ c[9] ][ c[10] ][ c[11] ].x ) ||
	   ( tc3f.y != c3f[ c[9] ][ c[10] ][ c[11] ].y ) ||
	   ( tc3f.z != c3f[ c[9] ][ c[10] ][ c[11] ].z ) ){
	System.out.println( "Specular colour changed." );
      }
    }
  }
}


    
    
