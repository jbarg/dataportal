import java.util.*;
public class test8{
  public static void main( String args[] ){
    HashMap hm = new HashMap();
    hm.put( new Integer( 1 ), "entry1" );
    hm.put( new Integer( 2 ), "entry2" );
    hm.put( new Integer( 3 ), "entry3" );
    hm.put( new Integer( 4 ), "entry4" );
    hm.put( new Integer( 5 ), "entry5" );
    Object keys[] = hm.keySet().toArray();
    Object entrys[] = hm.entrySet().toArray();
    System.out.println( "Key set:" );
    for ( int i=0; i<keys.length; i++ ){
      System.out.println( keys[i] );
    }
    System.out.println( "Entry set:" );
    for ( int i=0; i<entrys.length; i++ ){
      System.out.println( entrys[i] );
      
    }
    Object values[] = hm.values().toArray();
    System.out.println( "Value collection:" );
    for ( int i=0; i<values.length; i++ ){
      System.out.println( values[i] );
    }

    System.out.println( "Looking for element 4" );
    System.out.println( "Found : " + (String)hm.get( new Integer( 4 ) ) );

  }
}
