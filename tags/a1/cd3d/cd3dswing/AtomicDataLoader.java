import java.io.*;
import java.util.*;
import java.util.zip.*;
import javax.vecmath.*;

public class AtomicDataLoader{
  public static void main( String args[] ) throws Exception{
    if ( args.length < 2 ){
      System.err.println( "Useage: java AtomicDataLoader <input file> <output file>" );
      System.exit(0);
    }
    Vector atoms = new Vector();

    int dt;
    File f = new File( args[0] );
    FileInputStream fis = new FileInputStream( f );
    InputStreamReader isr = new InputStreamReader( fis );
    BufferedReader br = new BufferedReader( isr );
    StreamTokenizer st = new StreamTokenizer( br );
    st.parseNumbers();
    Vector store = new Vector();
    HashMap hm = null;
    String symbol = null;
    Color3f colour = null;
    int atomicNumber = 0, oxidationState = 0;
    float radius, col1, col2, col3;
    do{
      //	System.out.println( st.sval );
      switch ( dt = st.nextToken() ){
      case StreamTokenizer.TT_WORD:
	if ( st.sval.startsWith( "Atom" ) ){
	  if ( hm != null ) store.add( new CrystalAtom( atomicNumber, symbol,
							colour, hm, oxidationState) );
	  dt = st.nextToken();
	  atomicNumber = (int)st.nval;
	  //	  System.out.println( "Now at the beginning of description for atomic number " + atomicNumber );
	  dt = st.nextToken();
	  symbol = st.sval;
	  dt = st.nextToken();
	  oxidationState = (int)st.nval;
	  dt = st.nextToken();
	  col1 = (float)st.nval;
	  dt = st.nextToken();
	  col2 = (float)st.nval;
	  dt = st.nextToken();
	  col3 = (float)st.nval;
	  colour = new Color3f( col1, col2, col3 );
	  // Ready for radius data
	  hm = new HashMap();
	}
	else if ( st.sval.startsWith( "Radius" ) ){
	  dt = st.nextToken();
	  oxidationState = (int)st.nval;

	  dt = st.nextToken();
	  radius = (float)st.nval;

	  Integer os = new Integer( oxidationState );
	  Float ra = new Float( radius );
	  hm.put( os, ra );
	  //	  System.out.println( "Radius for oxidation state " + oxidationState + " = " + radius );
	}
      }
    } while ( dt != StreamTokenizer.TT_EOF );
    fis.close();
    System.out.println( "File closed" );
    // Convert store to an array of CrystalAtom    
    CrystalAtom atDefs[] = new CrystalAtom[ store.size() ];    
    int pos = 0;
    for ( Enumeration e = store.elements(); e.hasMoreElements(); ){
      CrystalAtom ca = (CrystalAtom)e.nextElement();
      atDefs[ pos++ ] = ca;
      System.out.println( "Atom found---------------------------------" );
      System.out.println( "Atomic number = " + ca.atomicNumber );
      System.out.println( "Symbol = " + ca.symbol );
      System.out.println( "Colour = " + ca.color );
      for ( Iterator i = ca.radii.keySet().iterator(); i.hasNext(); ){
	int os = ( (Integer)i.next() ).intValue();
	System.out.println( "Oxidation state " + os + ", radius = " +
			    ( (Float)ca.radii.get( new Integer( os ) ) ).floatValue() );
      }
    }
    // Fill out array containing data by atomic number.
    int maxAtomicNumber = 0;
    for ( int i=0; i<atDefs.length; i++ ){
      if ( atDefs[i].atomicNumber > maxAtomicNumber ) maxAtomicNumber = atDefs[i].atomicNumber;
    }
    CrystalAtom zData[] = new CrystalAtom[ maxAtomicNumber+1 ];
    for ( int i=0; i<atDefs.length; i++ ){
      zData[ atDefs[i].atomicNumber ] = atDefs[i];
    }
    // Set default for the rest - atomic number 0 is a "default"
    for ( int i=0; i<zData.length; i++ ){
      if ( zData[i] == null ) zData[i] = atDefs[0];
    }

    // Now write data back to object stream.
    File of = new File( args[1] );
    FileOutputStream fos = new FileOutputStream( of );
    GZIPOutputStream gzos = new GZIPOutputStream( fos );
    ObjectOutputStream oos = new ObjectOutputStream( gzos );
    oos.writeObject( zData );
    gzos.close();
    fos.close();
  }
}
