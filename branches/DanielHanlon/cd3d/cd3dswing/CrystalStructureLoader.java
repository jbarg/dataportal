import java.io.*;
import javax.vecmath.*;
import java.awt.*;
import javax.swing.*;

public class CrystalStructureLoader extends CrystalStructure{
  StreamTokenizer st;
  InputStream is;
  Component parent;
  Thread thread;

  public CrystalStructureLoader( InputStream is, Component parent ) throws IOException{
    this.is = is;
    this.parent = parent;
    st = new StreamTokenizer(
	   new BufferedReader(
	     new InputStreamReader(
	       new ProgressMonitorInputStream( parent, "Reading structure file", is ) ) ) );
    st.commentChar('#');
    st.resetSyntax();
    st.whitespaceChars( 0, ' ' );
    st.wordChars( 33, 255 );
    st.parseNumbers();
    st.eolIsSignificant( true );

    String fileError = new String( "Error reading file." );
    try{
      int dt;
      int section = 0;
      float temp[] = new float[1]; // Just to initialise it...
      int linePos = 0; // No. of this token on this line.
      int secLine = 0; // Line of this section.
      int secPos = 0; // No. of token in this section.
      int symOps = 1; // Just to initialise.
      int noOfAtoms = -1; // Just to initialise.
      Point3f location;
      Color3f color;
      float radius;
      boolean finished = false;
      do {
	switch ( dt = st.nextToken() ){
	case StreamTokenizer.TT_WORD:
	  switch( section ){
	  case 1:
	  case 5:
	    //	    System.out.println( "Word found - attempting to convert to number" );
	    temp[ linePos-1 ] *= Double.parseDouble( "1" + st.sval );
	    //	    System.out.println( "Value is " + temp[ linePos-1 ] );
	    break;
	  default:
	    throw new IOException( fileError );
	  }
	  break;
	case StreamTokenizer.TT_NUMBER:
	  //	  System.out.println( st.nval );
	  switch ( section ){
	  case 0:
	    // Dimensions etc...
	    switch ( linePos++ ){
	    case 0:
	      dimensions = (int)st.nval;
	      temp = new float[ dimensions * 3 ];
	      latticeVector = new Vector3f[ dimensions ];
	    default:
	    }
	    break;
	  case 1:
	    // Axes
	    temp[ linePos++ ] = (float)st.nval;	    
	    break;
	  case 2:
	    // No. of symmetry operators - to be ignored
	    symOps = (int)st.nval;
	    //	    System.out.println( symOps + " symmetry operators." );
	    linePos++;
	    secPos++;
	    break;
	  case 3:
	    // Symmetry Operator definitions
	    secPos++;
	    linePos++;
	    break;
	  case 4:
	    // Number of atoms
	    noOfAtoms = (int)st.nval; // No. of atoms now defined...
	    //	    System.out.println( noOfAtoms + " atoms to be defined." );
	    linePos++;
	    secPos++;
	    break;
	  case 5:
	    // Atom data
	    temp[ linePos++ ] = (float)st.nval;
	    secPos++;
	    break;
	  default:
	    System.out.println( "Section should never be outside the range 0-5 " );
	  }
	  break;
	case StreamTokenizer.TT_EOL:
	  //	  System.out.println( "EOL" );
	  switch ( section ){
	  case 0:
	    secLine = -1;
	    section++;
	    break;
	  case 1:
	    latticeVector[ secLine ] = new Vector3f( temp ); // Axes now defined...
	    //	    System.out.println( "LatticeVector " + (secLine+1) + "=" + latticeVector[secLine] );
	    if ( secLine == 2 ){
	      secLine = -1;
	      section++;
	    }
	    break;
	  case 2:
	    secLine = -1;
	    section++;
	    secPos = 0;
	    break;
	  case 3:
	    if ( secPos == 12 * symOps ){
	      section++;
	      secLine = -1;
	    }
	    break;
	  case 4:
	    atom = new CrystalAtom[ noOfAtoms ];
	    section++;
	    secLine = -1;
	    secPos = 0;
	    temp = new float[8]; // Maximum number of data elements per atom.
	    break;
	  case 5:
	    int atomicNumber = (int)temp[0];
	    location = new Point3f( temp[1], temp[2], temp[3] );
	    if ( linePos > 4 ){
	      radius = temp[4];
	      if ( linePos == 8 ){
		color = new Color3f( temp[5], temp[6], temp[7] );
		atom[ secLine ] = new CrystalAtom( atomicNumber, location, radius, color );
	      } else {
		atom[ secLine ] = new CrystalAtom( atomicNumber, location, radius );
	      }
	    } else {
	      atom[ secLine ] = new CrystalAtom( atomicNumber, location );
	    }
	    //	    System.out.println( "Atom " + secLine + " defined as " + atom[ secLine ].toString() );
	    break;
	  default:
	    System.out.println( "Section should never be outside the range 0-5 " );
	  }
	  secLine++; // Next line in this section.
	  if ( secLine == noOfAtoms ) finished = true;
	  linePos = 0; // Beginning of a new line.
	  break;
	case StreamTokenizer.TT_EOF:
	  throw new IOException( fileError );
	default:
	  System.out.println( "This shouldn't happen" );
	}
      } while ( !finished );
      is.close();
    }
    catch( IOException ioe ){
      System.out.println( ioe );
    }
    calculateVectors();
  }
  
  /* Method to find the diagonal vector of the cell. */
  public void calculateVectors(){
    diagonalVolumeA = new Vector3f();
    for ( int i=0; i<3; i++ ){
      diagonalVolumeA.add( latticeVector[i] );
    }
  }
}
















