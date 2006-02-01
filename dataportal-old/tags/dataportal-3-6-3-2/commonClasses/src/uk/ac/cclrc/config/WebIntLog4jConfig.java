/*
 * WebIntLog4jCongif.java
 *
 * Created on 22 December 2004, 09:05
 */

package uk.ac.cclrc.config;

import java.io.*;
import java.util.*;
/**
 *
 * @author  gjd37
 */
public class WebIntLog4jConfig {

    /** Creates a new instance of WebIntLog4jCongif */
    public WebIntLog4jConfig() {
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws Exception{
        // TODO code application logic here
        try{
        File file = new File(args[0]);
        /*if(file.exists()) System.out.println("file exists");
        FileWriter filewriter = new FileWriter(file);
        filewriter.write("globusKey="+args[1]+"\n");
        filewriter.write("globusCert="+args[2]);
        filewriter.close();*/
        Properties prop = new Properties();
        prop.load(new FileInputStream(args[1]));
        prop.setProperty("log4j.appender.Default.file", args[2]);
        
        prop.store(new FileOutputStream(file),"Log4j configuration");
        }
        catch(Exception e){
            e.printStackTrace();
        }

    }

}
