/*
 * test.java
 *
 * Created on 23 June 2004, 01:28
 */

package uk.ac.clrc.dataportal.credentials;
import java.util.*;

import javax.xml.namespace.QName;
import javax.xml.rpc.ParameterMode;
import org.apache.axis.encoding.*;
import java.net.*;
import java.io.*;
import org.globus.gsi.GlobusCredential;
import org.gridforum.jgss.*;
import org.ietf.jgss.*;
/**
 *
 * @author  glen
 */
public class test {
    
    /** Creates a new instance of test */
    public test(){
   /*     LinkedHashMap          HashSet b = new HashSet();
        Vector f =new Vector();
        f.add
        b.put("1","1");
        b.put("2","2");
        b.put("3","3");
        System.out.println("added, now get out");
        Collection d= b.values();
        Iterator f = d.iterator();
        while(f.hasNext()){
            Object f1 = (Object)f.next();
            Class hj = f1.getClass();
            System.out.println(f1.getClass());
            System.out.println(f1);
            System.out.println(b.get(f1));
            System.out.println(hj.getClass());
    
        }*/
        try{
            long start = System.currentTimeMillis();
            SendCertificate sendcret = new SendCertificate();
            sendcret.addParameter(XMLType.XSD_INTEGER,new Integer(12));
            
            String mess = (String)sendcret.sendDelegatedCertificate("http://localhost:8080/datatransfer/services/CredService", "urlCopy", XMLType.XSD_ANYTYPE, "22", new File(System.getProperty("user.home")+"/Local Settings/Temp/x509up_u_gjd37"));
            System.out.println(mess);
            long end = System.currentTimeMillis();
            
            System.out.println("Time taken to delegate creds is "+(end - start)+" milliseconds");
        }
        catch(Exception r){
            System.out.println(r);
        }
        
        
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        new test();
    }
    
    public static String turnString(){
        
        try{
            System.out.println("start to static ");
            //   URL url1  = new URL("file:///"+System.getProperty("user.home")+"/Local Settings/Temp/x509up_u_gjd37");
            URL url1 = new URL("file:///"+System.getProperty("user.home")+"/Local Settings/Temp/x509up_u_gjd37");
            // System.out.println(url);
            URLConnection con = url1.openConnection();
            InputStream in2 = con.getInputStream();
            BufferedReader in = new BufferedReader(new InputStreamReader(in2));
            String inputLine;
            // String certt;
            StringBuffer cert = new StringBuffer();
            while ((inputLine = in.readLine()) != null){
                //  System.out.println(inputLine);
                cert.append(inputLine);
                cert.append("\n");
                // if(!inputLine.equals("-----END CERTIFICATE-----"))  cert.append("\n");
                
            }
            in.close();
            System.out.println("end to st");
            return  cert.toString();
            //urlCopy("http://localhost:8080/index.jsp","gsiftp://esc.dl.ac.uk/ggg",cert3,"2");
        }
        catch(Exception e){
            System.out.println("exception is tost" +e);
            return "";
        }
    }
}
