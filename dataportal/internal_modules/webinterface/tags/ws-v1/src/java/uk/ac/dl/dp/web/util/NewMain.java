/*
 * NewMain.java
 *
 * Created on 20 September 2006, 15:53
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.util;
import java.io.*;
import java.net.*;
import org.apache.log4j.Logger;
/**
 *
 * @author gjd37
 */
public class NewMain {
    
    /** Creates a new instance of NewMain */
    public NewMain() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws Exception{
        // TODO code application logic here
        
            Logger log = Logger.getLogger(NewMain.class);     
                  URL url = new URL("http://www.bbc.co.uk:80");
        System.out.println(""+url.getPort());
        System.out.println(url.getHost());
    }
    
}
