/*
 * Search.java
 *
 * Created on 13 October 2003, 14:51
 */

package uk.ac.dl.web;


import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.apache.log4j.*;
import java.util.Properties;
import ac.dl.xml.*;
import org.jdom.*;
import java.io.*;
import ac.dl.xml.*;
/**
 *
 * @author  gjd37
 */
public class Search {
    
    //set static log for the class
    static Logger logger = Logger.getLogger(Search.class);
    
    /** Creates a new instance of Search */
    public Search() {
    }
    
    public static void doBasicSearch(String sid, String[] facs, String discipline,Integer waiting, String url,String workingDir,String dn, boolean doLog) throws Exception{
        FileWriter wr = null;
        try{
            Service  service = new Service();
            Call  call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(url) );
            call.setOperationName( "doBasicQuery" );
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "op2", XMLType.SOAP_ARRAY, ParameterMode.IN );
            call.addParameter( "op3", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "op4", XMLType.XSD_INTEGER, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ELEMENT );
            
            Object[] ob = new Object[]{sid,facs,discipline,waiting};
            
            org.w3c.dom.Element el = (org.w3c.dom.Element) call.invoke(ob );
            
            //build the file
            org.jdom.input.DOMBuilder builder = new org.jdom.input.DOMBuilder();
            Element el1 = builder.build(el);
            el1.detach();
            Document doc1  = new org.jdom.Document(el1);
            
            //System.out.println(wd+File.separator+"profiles"+File.separator+sid+"1.xml");
            Saver.save(doc1, new File(workingDir+File.separator+"profiles"+File.separator+sid+"1.xml"));
            
            // logHistoryFile(object, (String)session.getAttribute("dn"),wd);
            if(doLog){
                StringBuffer buff = new StringBuffer();
                for(int i =0;i<facs.length;i++){
                    buff.append(facs[i]+" ");
                }
                StringBuffer buff2 = new StringBuffer();
                for(int i =0;i<facs.length;i++){
                    buff2.append("&facs="+facs[i]);
                }
                
                wr = new FileWriter(workingDir+File.separator+"profiles"+File.separator+"history"+File.separator+dn,true);
                wr.write("<tr><td>"+buff.toString()+" </td>");
                wr.write("<td>"+discipline+"</td>");
                wr.write("<td>"+new java.util.Date()+"</td>");
                wr.write("<td>"+waiting+"</td>");
                wr.write("<td><a href='../servlet/ResendQuery?discipline="+discipline+"&wait="+waiting+""+buff2.toString()+"'>Resend Query</a></td></tr>");
                wr.close();
            }
        }
        catch(Exception e){
            logger.warn("Coulld not do basic search",e);
            try{
                wr.close();
            }
            catch(Exception ignore){}
            throw e;
        }
    }
    
    
    private static void logHistoryFile(Object[] query,String dn, String wd){
        
       /* try{
            String[] facs = (String[])query[1];
            StringBuffer buff = new StringBuffer();
            for(int i =0;i<facs.length;i++){
                buff.append(facs[i]+" ");
            }
            StringBuffer buff2 = new StringBuffer();
            for(int i =0;i<facs.length;i++){
                buff2.append("&facs="+facs[i]);
            }
        
            wr = new FileWriter(wd+File.separator+"profiles"+File.separator+"history"+File.separator+dn,true);
            wr.write("<tr><td>"+buff.toString()+" </td>");
            wr.write("<td>"+query[2]+"</td>");
            wr.write("<td>"+new java.util.Date()+"</td>");
            wr.write("<td>"+query[3]+"</td>");
            wr.write("<td><a href='../servlet/ResendQuery?discipline="+query[2]+"&wait="+query[3]+""+buff2.toString()+"'>Resend Query</a></td></tr>");
            wr.close();
        }
        catch(Exception e){
            logger.warn("Coulld not update history file",e);
            try{
                wr.close();
            }
            catch(Exception ignore){}
        }*/
        
    }
}
