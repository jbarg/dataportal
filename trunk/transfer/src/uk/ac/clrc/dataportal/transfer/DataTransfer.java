/*
 * DataTransfer.java
 *
 * Created on 17 September 2003, 08:37
 */

package uk.ac.clrc.dataportal.transfer;
import java.io.*;
import org.apache.axis.client.Call;
import java.io.*;
import java.net.*;
import org.apache.axis.client.Service;

import org.apache.axis.encoding.XMLType;

import javax.xml.rpc.ParameterMode;
/**
 *
 * @author  gjd37
 */
public class DataTransfer {
    
    //this is to test the copy function
    public static void main(String args[]) {
        File credFile = null;
        try {
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(args[2]) );
            call.setOperationName( "getProxy" );
            call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
            //call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.XSD_STRING );
            
            
            Object[] ob = new Object[]{args[3]};
            String  cred= (String) call.invoke(ob );
            
            DataTransfer d= new DataTransfer();
            String transfered = d.urlCopy(args[0], args[1], cred);
            System.out.println("Transfered "+transfered);
        }
        catch(Exception e){
            System.out.println(e);
        }
    }
    
    /** Creates a new instance of DataTransfer */
    public DataTransfer() {
    }
    
    //web service
    public String urlCopy(String to,String from,String cert)throws Exception{
        File credFile = null;
        try{
            long time = System.currentTimeMillis();
            
            credFile = new File(System.getProperty("user.home")+File.separator+time);
            FileWriter writer = new FileWriter(credFile);
            writer.write(cert);
            writer.close();
            
            String os = System.getProperty("os.name");
            System.out.println(os);
            Process pro = null;
            String exe = "UrlCopy.sh";
            String result = "";
            if(os.startsWith("Windows")){
                exe = "UrlCopy.bat";
                System.out.println("Executing "+"\""+Config.getContextPath()+"Transfer"+File.separator+exe+"\" "+to+" "+from+" \""+credFile+"\" \""+Config.getContextPath()+"Transfer\"");
                pro  = Runtime.getRuntime().exec("\""+Config.getContextPath()+"Transfer"+File.separator+exe+"\" "+to+" "+from+" \""+credFile+"\" \""+Config.getContextPath()+"Transfer\"");
                result = LaunchProcess.runCommand("\""+Config.getContextPath()+"Transfer"+File.separator+exe+"\" "+to+" "+from+" \""+credFile+"\" \""+Config.getContextPath()+"Transfer\"");
            }
            else{
                System.out.println("Executing "+"\""+Config.getContextPath()+"Transfer"+File.separator+exe+"\" "+to+" "+from+" \""+credFile+"\" \""+Config.getContextPath()+"Transfer\"");
                pro  = Runtime.getRuntime().exec(Config.getContextPath()+"Transfer"+File.separator+exe+" "+to+" "+from+" "+credFile+" "+Config.getContextPath()+"Transfer");
                result = LaunchProcess.runCommand(Config.getContextPath()+"Transfer"+File.separator+exe+" "+to+" "+from+" "+credFile+" "+Config.getContextPath()+"Transfer");
                
                
            }
            //System.out.println("Context path is "+Config.getContextPath());
            ///////////////////////LaunchProcess.runCommand("\""+Config.getContextPath()+"classes"+File.separator+exe+"\" "+to+" "+from+" \""+System.getProperty("user.home")+File.separator+time +"\"");
            //Process pro1  = Runtime.getRuntime().exec("set PATHTRAN=\""+Config.getContextPath()+"classes\"");
            //Process pro  = Runtime.getRuntime().exec("\"C:\\Documents and Settings\\gjd37\\My Documents\\theDataPortal\\dataportalcvs\\dataportal\\transfer\\build\\WEB-INF\\Transfer\\UrlCopy.bat\" http://esc.dl.ac.uk/index.html gsiftp://esc.dl.ac.uk/hl \"C:\\x509up_12967.pem\" \"C:\\Documents and Settings\\gjd37\\My Documents\\theDataPortal\\dataportalcvs\\dataportal\\transfer\\build\\WEB-INF\\Transfer\"");
            
            // Process pro  = Runtime.getRuntime().exec("\""+Config.getContextPath()+"classes"+File.separator+exe+"\" "+to+" "+from+" \""+System.getProperty("user.home")+File.separator+time +"\"");
           /* InputStream in = pro.getErrorStream();
            StringBuffer buff = new StringBuffer();
            int c;
            while((c= in.read())!= -1){
                buff.append((char)c);
            }
            
            if(buff.toString().equals("")) System.out.println("no error!!");
            System.out.println("Error is "+buff);
            InputStream ini = pro.getInputStream();
            StringBuffer buffi = new StringBuffer();
            int ci;
            while((ci= ini.read())!= -1){
                buffi.append((char)ci);
            }
            ini.close();
            in.close();
            System.out.println("input is "+ buffi);
            
            
            System.out.println(pro.exitValue());
            pro.waitFor();
            pro.destroy();*/
            credFile.delete();
            if(result.equals("true")) return "true";
            else return result;
        }
        catch(Exception e){
            System.out.println(e);
            credFile.delete();
            throw new Exception(e.getMessage());
        }
    }
    
}
