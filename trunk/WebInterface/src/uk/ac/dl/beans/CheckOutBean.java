/*
 * CheckOutBean.java
 *
 * Created on 17 December 2003, 16:26
 */

package uk.ac.dl.beans;

import java.util.*;
import org.apache.log4j.*;
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.apache.axis.AxisFault;
import org.jdom.*;
import org.jdom.input.*;
import java.io.*;
/*
 *
 * @author  gjd37
 */
public class CheckOutBean {
    
    //setter variables
    private String by;
    private String sid;
    private Properties prop;
    private String xsl = "checkoutWS";;
    private String wd;
    private File files;
    private File datasets;
    private File studys;
    
    private Logger logger;
    private org.jdom.Document doc;
    private boolean isEmpty;
    
    /** Creates a new instance of CheckOutBean */
    public CheckOutBean() {
        logger = Logger.getLogger(this.getClass().getName());
       // BasicConfigurator.configure();
        isEmpty = false;
    }
    
    public Document getCart() throws Exception{
        String certws_url = this.prop.getProperty("CART");
        try{
            Service  service = new Service();
            Call     call    = (Call) service.createCall();
            call.setTargetEndpointAddress( new java.net.URL(certws_url) );
            call.setOperationName( "getCart" );
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            
            call.setReturnType( XMLType.SOAP_ELEMENT );
            
            Object[] ob = new Object[]{sid};
            org.w3c.dom.Element element = (org.w3c.dom.Element) call.invoke(ob );
            org.jdom.input.DOMBuilder buildert = new org.jdom.input.DOMBuilder();
            org.jdom.Element el = buildert.build(element);
            el.detach();
            doc  = new org.jdom.Document(el);
            isEmpty = false;
            if(doc == null) {
                //isEmpty = false;                
            }
            else{
                Element root = this.doc.getRootElement();
                List list = root.getChildren("item");
                
                if(list.size() == 0){
                    isEmpty = true;                    
                }
                //isEmpty = false;
            }
            
            
            return doc;
        }
        catch(Exception e){
            logger.error("Unable to get user cart for "+sid,e);
            throw e;
        }
    }
    
    public boolean isEmpty(){
        
        
        return isEmpty;
        
    }
    
    public void setSid(String sid){ this.sid = sid;}
    public void setBy(String by){
        
        this.by = by;
        xsl = "checkoutWS";
        if(by == null) xsl = "checkoutWS";
        else  if(by.equals("date")) xsl = "checkoutWSdate";
        else  if(by.equals("name")) xsl = "checkoutWS";
        
        
    }
    public void setProperties(Properties prop){
        this.prop = prop;
    }
    
    public void setFiles(){
        files = new File(wd+File.separator+"xsl"+File.separator+xsl+".xsl");
        datasets = new File(wd+File.separator+"xsl"+File.separator+xsl+"dataset.xsl");
        studys = new File(wd+File.separator+"xsl"+File.separator+xsl+"study.xsl");
    }
    
    public void setWd(String wd){
        this.wd = wd;
    }
    
    public String getBy(){
        return by;
    }
    public String getSid(){
        return sid;
    }
    
    //getter methods for file locations
    public File getFiles(){
        return files;
    }
    
    public File getDatasets(){
        return datasets;
    }
    
    public File getStudys(){
        
        return studys;
    }
    public Document getDoc(){
        return doc;
    }
    
}
