/* This class extends %%\textsf{CrystalData}%% and provides methods for
 * loading and parsing a data file in the format produced by %%\crystal%%. */
import java.io.*;
import javax.vecmath.*;
import java.awt.*;
import javax.swing.*;

public class CrystalDataLoader extends CrystalData {
  StreamTokenizer st;
  InputStream is;
  Component parent;

  public CrystalDataLoader( InputStream is, Component parent )
         throws IOException{
    this.is = is;
    this.parent = parent;
    st = new StreamTokenizer(
	   new BufferedReader(
             new InputStreamReader(
	       new ProgressMonitorInputStream( parent, "Reading data file", is ) ) ) );
    st.commentChar('#');
    st.eolIsSignificant( true );
    st.parseNumbers();

    try{
      /* to hold the token types. */
      int dt;

      /* Look for the data type on the first line. */
      StringBuffer dataType = new StringBuffer();
      while ( st.lineno() == 1 ){
	switch ( st.nextToken() ){
	case StreamTokenizer.TT_WORD:
	  if ( dataType.length() > 0 ) dataType.append( " " );
	  dataType.append( st.sval );
	}
      }

      name = dataType.toString();
      //      System.out.println( name );

      /* Next we are expecting five lines of tuples
       * i.e. 15 floating point numbers. */
      float buffer[][] = new float[5][3];
      for ( int i=0; i<15; i++ ){
	/* Skip anything we find until we get a number. */
	do { dt = st.nextToken(); }
	while ( dt != StreamTokenizer.TT_NUMBER );
	buffer[i/3][i%3] = (float)(st.nval);
      }

      /* Place the dimensions of the data in the appropriate objects. */
      // Add 1 to each to account for the extra faces added by repetition.
      points[0] = (int)( buffer[0][0] );
      points[1] = (int)( buffer[0][1] );
      points[2] = (int)( buffer[0][2] );

      cells[0] = points[0]-1;
      cells[1] = points[1]-1;
      cells[2] = points[2]-1;

      /* Initialise the array to hold the data. */
      data = new double[ points[0]+2 ][ points[1]+2 ][ points[2]+2 ];

      /* Set up the axis increment vectors an the origin. */
      origin.set( buffer[1] );
      axis[0].set( buffer[2] );
      axis[1].set( buffer[3] );
      axis[2].set( buffer[4] );

      /* Get first value to give min and max initial values. */
      do { dt = st.nextToken(); }
      while ( dt != StreamTokenizer.TT_NUMBER );
      max = st.nval;
      min = st.nval;
      st.pushBack();

      /* Read the data and place in the array.
       * Altered into a very unlikely order because of the
       * way Fortran apparently writes arrays. */
      for ( int z=0; z<points[2]; z++ ){
	for ( int y=0; y<points[1]; y++ ){
	  for ( int x=0; x<points[0]; x++ ){
	    do { dt = st.nextToken(); }
	    while ( dt != StreamTokenizer.TT_NUMBER );
	    data[x][y][z] = st.nval;
	    if ( st.nval < min ) min = st.nval;
	    if ( st.nval > max ) max = st.nval;
	  }
	}
      }

      // Replicate data for the far side.
      for ( int i=0; i<3; i++ ){
	points[i] +=2;
	cells[i] +=2;
      }
      
      //x
      for ( int y=0; y<points[1]; y++ ){
	for ( int z=0; z<points[2]; z++ ){
	  data[ cells[0]-1 ][y][z] += data[0][y][z];
	  data[ cells[0] ][y][z] += data[1][y][z];
	}
      }

      //y
      for ( int x=0; x<points[0]; x++ ){
	for ( int z=0; z<points[2]; z++ ){
	  data[x][ cells[1]-1 ][z] += data[x][0][z];
	  data[x][ cells[1] ][z] += data[x][1][z];
	}
      }

      //z
      for ( int x=0; x<points[0]; x++ ){
	for ( int y=0; y<points[1]; y++ ){
	  data[x][y][ cells[2]-1 ] += data[x][y][0];
	  data[x][y][ cells[2] ] += data[x][y][1];
	}
      }

      // Correct edges and far vertex.
      for ( int x=0; x<points[0]; x++ ){
	data[x][ cells[1] ][ cells[2] ] /= 2;
 	data[x][ cells[1]-1 ][ cells[2] ] /= 2;
	data[x][ cells[1] ][ cells[2]-1 ] /= 2;
	data[x][ cells[1]-1 ][ cells[2]-1 ] /= 2;
      }

      for ( int y=0; y<cells[1]; y++ ){
	data[ cells[0] ][y][ cells[2] ] /= 2;
	data[ cells[0]-1 ][y][ cells[2] ] /= 2;
	data[ cells[0] ][y][ cells[2]-1 ] /= 2;
	data[ cells[0]-1 ][y][ cells[2]-1 ] /= 2;
      }

      for ( int z=0; z<cells[2]; z++ ){
	data[ cells[0] ][ cells[1] ][z] /= 2;
	data[ cells[0]-1 ][ cells[1] ][z] /= 2;
	data[ cells[0] ][ cells[1]-1 ][z] /= 2;
	data[ cells[0]-1 ][ cells[1]-1 ][z] /= 2;
      }

      data[ cells[0] ][ cells[1] ][ cells[2] ] /= 3;
      data[ cells[0] ][ cells[1] ][ cells[2]-1 ] /= 3;
      data[ cells[0] ][ cells[1]-1 ][ cells[2] ] /= 3;
      data[ cells[0] ][ cells[1]-1 ][ cells[2]-1 ] /= 3;
      data[ cells[0]-1 ][ cells[1] ][ cells[2] ] /= 3;
      data[ cells[0]-1 ][ cells[1] ][ cells[2]-1 ] /= 3;
      data[ cells[0]-1 ][ cells[1]-1 ][ cells[2] ] /= 3;
      data[ cells[0]-1 ][ cells[1]-1 ][ cells[2]-1 ] /= 3;

      /* Close the input stream */
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
      latticeVectorB[i] = new Vector3f();
      latticeVectorB[i].scale( cells[i]-1, axis[i] );
      latticeVector[i] = new Vector3f();
      latticeVector[i].set( latticeVectorB[i] );
      latticeVector[i].scale( SCALE );
      diagonalVolumeA.add( latticeVector[i] );
    }
  }
}






