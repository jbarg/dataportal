/*
 * ClearConfigData.java
 *
 * Created on 07 May 2003, 08:27
 */

package uk.ac.dl.rasgrib;
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
            
            FileWriter out = new FileWriter("web"+File.separator+"WEB-INF"+File.separator+"RasGrib.conf");
            out.write("#conf  file for the RasDaMan web service\n");
            out.write("#host name of the RasDaMan\n");
            out.write("host_name=\n");
            out.write("\n");
            out.write("#jdbc port number\n");
            out.write("jdbc_listener_port=\n");
            out.write("\n");
            out.write("#oracle database name SID\n");
            out.write("oracle_sid=\n");
            out.write("\n");
            out.write("#rasdamna db name\n");
            out.write("rasdaman_db_name=\n");
            out.write("\n");
            out.write("#port number\n");
            out.write("rasdaman_port=\n");
            out.write("\n");
            out.write("#username\n");
            out.write("rasdaman_username=\n");
            out.write("\n");
            out.write("#password\n");
            out.write("rasdaman_password=\n");
            out.write("\n");
            out.write("#username to the cera database\n");
            out.write("user_name(cera)=\n");
            out.write("\n");
            out.write("#password to the  cera database\n");
            out.write("user_password(cera)=\n");
            out.close();
            
        }
        catch(IOException ioe){
            System.out.println(ioe);
            
        }
        
    }
    
}
