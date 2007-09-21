/*
 * Util.java
 *
 * Created on 22 September 2006, 13:28
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.io.File;

/**
 *
 * @author gjd37
 */
public class Util {
    
    /** Creates a new instance of Util */
    public Util() {
    }
    
    public static  boolean isImageJ(String filename) {
        try {
            // Get extension from a File object
            File f = new File(filename);
            String s = f.getName().substring((f.getName().lastIndexOf('.')+1));
            
            for(String exe : DataPortalConstants.IMAGEJ_TYPES){
                if(s.equalsIgnoreCase(exe)) return true;
            }
            return false;
        } catch(Exception e) {
            return false;
        }
        
    }
    
    public static void main(String args[]){
        System.out.println(isImageJ("ddff.LOG"));
    }
    
}
