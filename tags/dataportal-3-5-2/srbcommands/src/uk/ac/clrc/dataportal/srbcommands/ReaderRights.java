/*
 * ReaderRights.java
 *
 * Created on 10 September 2003, 11:43
 */

package dataportal.srbcommands.src.uk.ac.clrc.dataportal.srbcommands;

import java.io.*;
import java.util.*;
/**
 *
 * @author  gjd37
 */
public class ReaderRights {
    
    /** Creates a new instance of ReaderRights */
    public static Vector ReaderRights(String args,String fileconf) throws Exception {
        Vector files = new Vector();
        //need to connect to db, or read a gridmap-file
        //load location properties file.  HARD CODE for now
        Properties prop = new Properties();
        prop.load(new FileInputStream(System.getProperty("user.home")+File.separator+"grid-mapfile.txt"));
        
        String srbUser = (String)prop.getProperty(args);
        Process pr1o = Runtime.getRuntime().exec("/opt/srb/SRB2/utilities/bin/Sinit");
        Process pro = Runtime.getRuntime().exec("/opt/srb/SRB2/utilities/bin/Sls -C "+fileconf);
        
        InputStream inputs = pro.getInputStream();
        
        //out print erro message
        /*int c;
        StringBuffer bu = new StringBuffer();
        while((c=inputs.read()) != -1){
                bu.append((char)c);
        }
        System.out.println(bu);
         */
        //FileReader in = new FileReader(fileconf);
        BufferedReader buff = new BufferedReader(new InputStreamReader(inputs));
        String line = "";
        while((line = buff.readLine()) !=null){
            //System.out.println(line);
            String[] splits = line.split(" ");
            
            
            String[] split = new String[4];
            int j = 0;
            for(int i = 0;i < splits.length;i++){
                
                if(!(splits[i].equals(""))){
                    split[j] = splits[i];
                    j++;
                    // if(j >3) break;
                }
            }
            //System.out.println(j);
            if(j > 4 || j < 4){}
            else{
                // System.out.println(split[3]);
                int position = split[3].indexOf('@');
                //System.out.println("split 2 "+split[2]);
                if((split[2].equals("all") || split[2].equals("read")) && split[3].substring(0,position).equals(srbUser)){
                    files.add(split[0]);
                    System.out.println(srbUser+" can see this "+split[0]);
                }
            }
        }
        
        Process r1o = Runtime.getRuntime().exec("/opt/srb/SRB2/utilities/bin/Sexit");
        return files;
    }
    
    
    
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        try{
            ReaderRights(args[0],args[1]);
        }
        catch(Exception e){
            System.out.println(e);
        }
    }
    
}
