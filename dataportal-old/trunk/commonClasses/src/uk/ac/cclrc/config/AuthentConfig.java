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
        //String cacerts = (args[3].equals(args[4])) ? args[3] : args[3] + " , "+ args[4];
        
        
        Properties prop = new Properties();
        prop.setProperty("portal_private_key_filename", args[1]);
        prop.setProperty("portal_cert_filename", args[2]);
        //System.out.println("args[3] is: "+args[3]);
        if(!args[3].equals("") && !args[3].equals("${userCaCert}")){
            prop.setProperty("user_ca_cert_filename", args[3]);
            //setTrustedCerts(args[3]);
        }
        if(!args[4].equals("") && !args[4].equals("${myProxyCaCert}")){
            prop.setProperty("myproxy_ca_cert_filename", args[4]);
            //setTrustedCerts(args[4]);
        }
        
        prop.store(new FileOutputStream(file),"Globus configuration");
        
    }
    
    private static void setTrustedCerts(String fileCert) throws Exception{
        //work around cos a bug in the  cog kit code about setting Root CA s
        File globusDir = new File(System.getProperty("user.home")+File.separator+".globus");
        File globusCertDir = new File(globusDir+File.separator+"certificates");
        if(!globusDir.exists()) globusDir.mkdir();
        if(!globusCertDir.exists()) globusCertDir.mkdir();
        
        //so direcotry should be made now
        File fileCACert = new File(fileCert);
        
        File rootCaCertFile = new File(globusCertDir,fileCACert.getName());
        if(!rootCaCertFile.exists()) copyFile(fileCACert,rootCaCertFile);
        
    }
    
    private static void copyFile(File source, File dest) throws Exception {
        
        //NB:  NIO did not work on gen 2.  1.4.2_06 jre from sun.  Called a
        // java.io.Exception:  Illegal arguement exception.  Arguments were fine.
        //Error with NIO on gen 2??????
        
                /*logger.warn("File names are "+in.toString() +"   and "+out.toString());
        FileChannel sourceChannel = new
        FileInputStream(in).getChannel();
        FileChannel destinationChannel = new
        FileOutputStream(out).getChannel();
        sourceChannel.transferTo(0, sourceChannel.size(), destinationChannel);
        // or
        //  destinationChannel.transferFrom(sourceChannel, 0, sourceChannel.size());
        sourceChannel.close();
        destinationChannel.close();*/
        
        try {
            BufferedReader br = new BufferedReader(new FileReader(source));
            BufferedWriter bw = new BufferedWriter(new FileWriter(dest));
            
            int read = 0;
            while((read = br.read()) != -1) {
                
                bw.write(read);
            }
            
            br.close();
            bw.close();
        } catch (FileNotFoundException fnfe) {
            throw fnfe;
        } catch (IOException e) {
            throw e;
        }
        
    }
    
}
