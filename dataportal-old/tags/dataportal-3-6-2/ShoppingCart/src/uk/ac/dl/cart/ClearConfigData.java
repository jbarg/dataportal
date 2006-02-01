/*
 * ClearConfigData.java
 *
 * Created on 07 May 2003, 08:27
 */

package uk.ac.dl.cart;
import java.util.*;
import java.io.*;
/**
 *
 * @author  gjd37
 */
public class ClearConfigData  {
    
    public static void main(String args[]){
        try{
            /*Properties prop = new Properties();
            prop.setProperty("srd_url","");
            prop.setProperty("isis_url","");
            prop.
            prop.store(new FileOutputStream("web"+File.separator+"WEB-INF"+File.separator+"wrapper.txt"),"This is the configuration for the isis and srd demo wrappers");*/
            
            FileWriter out = new FileWriter("web"+File.separator+"WEB-INF"+File.separator+"Shop.conf");
            out.write("# conf file for the Shopping cart web service\n");
            out.write("\n");
            out.write("# database jdbc url\n");
            out.write("db_url=\n");
            out.write("\n");
            out.write("# database name\n");
            out.write("db_name=\n");
            out.write("\n");
            out.write("# database user\n");
            out.write("db_user=\n");
            out.write("\n");
            out.write("# database password\n");
            out.write("db_password=\n");
            out.write("\n");
            out.write("# table name\n");
            out.write("db_table_name=\n");
            out.write("\n");
            out.write("# db driver\n");
            out.write("db_driver=\n");
            out.write("\n");
            out.write("#url of the session manager\n");
            out.write("sm_url=\n");
           
            out.close();
            
        }
        catch(IOException ioe){
            System.out.println(ioe);
            
        }
        
    }
    
}
