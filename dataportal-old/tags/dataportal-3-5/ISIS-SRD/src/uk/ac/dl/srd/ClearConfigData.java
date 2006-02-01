/*
 * ClearConfigData.java
 *
 * Created on 07 May 2003, 08:27
 */

package uk.ac.dl.srd;
import java.util.*;
import java.io.*;
/**
 *
 * @author  gjd37
 */
public class ClearConfigData  {
    
    public static void main(String args[]){
        try{
            Properties prop = new Properties();
            prop.setProperty("srd_url","");
            prop.setProperty("isis_url","");
            prop.store(new FileOutputStream("web"+File.separator+"WEB-INF"+File.separator+"wrapper.txt"),"This is the configuration for the isis and srd demo wrappers");
        }
        catch(IOException ioe){
            System.out.println(ioe);
            
        }
        
    }
    
}
