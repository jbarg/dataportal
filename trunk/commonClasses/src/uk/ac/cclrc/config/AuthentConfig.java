/*
 * AuthentConfig.java
 *
 * Created on 30 November 2004, 11:59
 */

package uk.ac.cclrc.config;

import java.io.*;
import java.util.*;
/**
 *
 * @author  gjd37
 */
public class AuthentConfig {
    
    /** Creates a new instance of AuthentConfig */
    public AuthentConfig() {
    }
    
    
    
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws Exception{
        // TODO code application logic here
        File file = new File(args[0]);
        /*if(file.exists()) System.out.println("file exists");
        FileWriter filewriter = new FileWriter(file);
        filewriter.write("globusKey="+args[1]+"\n");
        filewriter.write("globusCert="+args[2]);
        filewriter.close();*/
        Properties prop = new Properties();
        prop.setProperty("globusKey", args[1]);
        prop.setProperty("globusCert", args[2]);
        prop.store(new FileOutputStream(file),"Globus configuration");
        
    }
    
}
