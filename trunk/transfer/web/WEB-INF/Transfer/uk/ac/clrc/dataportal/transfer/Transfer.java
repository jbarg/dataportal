/*
 * Transfer.java
 *
 * Created on 16 September 2003, 12:02
 */

package uk.ac.clrc.dataportal.transfer;

import org.apache.axis.client.Call;
import java.io.*;
import java.net.*;

import org.apache.axis.encoding.XMLType;

import javax.xml.rpc.ParameterMode;
import org.globus.io.urlcopy.UrlCopy;
import org.globus.util.GlobusURL;
import org.globus.myproxy.MyProxy;
import org.globus.gsi.GlobusCredential;
import org.globus.gsi.gssapi.GlobusGSSCredentialImpl;

import org.ietf.jgss.*;
import org.ietf.jgss.GSSName;

import org.gridforum.jgss.*;
import java.io.*;
import org.ietf.jgss.GSSManager;



public class Transfer {
    
    
    
    public static void main(String args[]) {
        File credFile = null;
        try {
            //System.out.println("Url to "+args[1]);
            //System.out.println("Url from "+args[0]);
            //System.out.println("Sesiosn Manager "+args[2]);
            //System.out.println("Sesion iud "+args[3]);
            //String os = System.getProperty("os.name");
            //System.out.println(os);
            //first get the proxy certificate
           /* Service  service = new Service();
          /home/tomcat4/dataportal/transfer/build/WEB-INF/Transfer/UrlCopy.sh http://GJD37VIG.dl.ac.uk:8080/dataportal/xml/acc70132-ee56-11d7-8953-f7b9e28671fd.tar  /home/tomcat4/1064384917648 /home/tomcat4/dataportal/transfer/build/WEB-INF/Transfer
  Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(args[2]) );
            call.setOperationName( "getProxy" );
            call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
            //call.addParameter( "sid1", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.XSD_STRING );
            
            
            Object[] ob = new Object[]{args[3]};
            String  cred= (String) call.invoke(ob );
            **/
            //save file
          /* long time = System.currentTimeMillis();
            System.out.println("Saving file..");
            credFile = new File(System.getProperty("user.home")+File.separator+time);
            FileWriter writer = new FileWriter(credFile);
            writer.write(cred);
            writer.close();*/
             
            
            //System.out.println("Got credential...");
            //System.out.println("Trying to copy");
            //Transfer tran = new Transfer();
            System.out.println("Trying to execute/...");
            boolean copied = copy(args[0], args[1], args[2]);
            System.out.println("Transfer: "+copied);
            //credFile.delete();
        }
        catch(Exception e){
            //credFile.delete();
            System.out.println(e);
        }
        
        
    }
    public static boolean copy(String urlFrom, String urlTo, String credFile){
        //File credFile = null;
        
        try{
            long time = System.currentTimeMillis();
            //System.out.println("Saving file..");
            // credFile = new File(System.getProperty("user.home")+File.separator+time);
            //FileWriter writer = new FileWriter(credFile);
            //writer.write(cred);
            // writer.close();
            
            GlobusCredential globusCredential = new GlobusCredential(credFile);
            //portalCertFile, portalKeyFile);
            GSSCredential credential2 = new GlobusGSSCredentialImpl(globusCredential, GSSCredential.INITIATE_AND_ACCEPT);
            System.out.println("User "+credential2.getName().toString());
            System.out.println("User lifetime left "+credential2.getRemainingLifetime()+" seconds");
            UrlCopy uc = new UrlCopy();
            System.out.println("Coping file from "+urlFrom+" to "+urlTo);
            uc.setSourceUrl(new GlobusURL(urlFrom));
            uc.setDestinationUrl(new GlobusURL(urlTo));
            uc.setUseThirdPartyCopy(true);
            //      uc.setDCAU(true);
            //      uc.setSourceAuthorization(null);
            //      uc.setDestinationAuthorization(null);
            //      uc.setAppendMode(false);
            uc.setCredentials(credential2);
            uc.copy();
            System.out.println("Transfer complete.");
            
            //credFile.delete();
            return true;
        }
        catch (Exception e) {
            
           // credFile.delete();
            System.err.println(e);
            return false;
        }
    }
}


