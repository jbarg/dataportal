/*
 * Convert.java
 *
 * Created on 24 November 2004, 14:45
 */

package uk.ac.dl.dn;

/**
 *
 * @author  gjd37
 */
public class Convert {
    
    /** Creates a new instance of Convert */
    public Convert() {
    }
    
    static String removeSpaces(String string){
        int n = string.length();
        StringBuffer h = new StringBuffer();
        for(int i = 0; i<n; i++){
            if( string.charAt(i) == '/') h.append("_");
            
            else if( string.charAt(i) == ' ') h.append("_");
            else h.append(string.charAt(i));
        }
        return h.toString();
        
    }
    
}
