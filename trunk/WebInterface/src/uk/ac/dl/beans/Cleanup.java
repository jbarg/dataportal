/*
 * Cleanup.java
 *
 * Created on 16 December 2003, 09:44
 */

package uk.ac.dl.beans;

import org.apache.log4j.*;
import java.io.*;
import java.util.*;
import javax.servlet.http.HttpSession;

//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
/**
 *
 * @author  gjd37
 */
public class Cleanup {
    
    //set static log for the class
    private static Logger logger;
    
    static {
        logger = Logger.getLogger(uk.ac.dl.beans.Cleanup.class);
       // BasicConfigurator.configure();
    }
    
    public static void session(HttpSession session) throws Exception{
        
        String sid =null;
        String wd = null;
        Properties prop = null ;
        String session_url = null;
        try{
            sid = (String)session.getAttribute("sessionid");
            wd = (String)session.getAttribute("wd");
            prop   = (Properties)session.getAttribute("props");
            session_url = prop.getProperty("SESSION");
            
            for(int i = 1; i < 4;i++){
                new File(wd+File.separator+"profiles"+File.separator+sid+i+".xml").delete();
            }
            new File(wd+File.separator+"profiles"+File.separator+"emat"+sid).delete();
            
            endSession(sid,session_url);
        }
        catch(Exception e){
            logger.warn("Unable to end session "+sid,e);
            
        }
    }
    
    private static void endSession(String sid,String session_url)throws Exception{
        try{
            
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(session_url) );
            call.setOperationName( "endSession" );
            call.addParameter( "sid", XMLType.XSD_STRING, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ARRAY );
            Object[] ob = new Object[]{sid};
            
            call.invoke(ob );
        }
        
        catch(Exception e){
            
            throw e;
        }
        
    }
}
