/*
 * SeeNoteBean.java
 *
 * Created on 17 December 2003, 08:52
 */

package uk.ac.dl.beans;

import java.util.*;
import org.apache.log4j.*;
//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.apache.axis.AxisFault;
/**
 *
 * @author  gjd37
 */
public class SeeNoteBean {
    
    //variables of getter setter
    private Properties prop;
    private String sid;
    private String url;
    
    //
    
    private String certws_url;
    private String[] ret;
    private  Logger logger;
    
    private String srows;
    
    /*static {
        logger = Logger.getLogger(SeeNoteBean.class);
        BasicConfigurator.configure();
    }*/
    
    
    /** Creates a new instance of SeeNoteBean */
    public SeeNoteBean(String sid, String url,Properties prop) {
        logger = Logger.getLogger(uk.ac.dl.beans.SeeNoteBean.class);
        this.sid = sid;
        this.url = url;
        this.prop = prop;
        this.sid = sid;
    }
    
    public SeeNoteBean(){
        logger = Logger.getLogger(uk.ac.dl.beans.SeeNoteBean.class);
       // BasicConfigurator.configure();
    }
    
    public void getCart() throws Exception{
        
        
        this.certws_url = this.prop.getProperty("CART");
        try{
            Service  service = new Service();
            Call     call    = (Call) service.createCall();
            call.setTargetEndpointAddress( new java.net.URL(certws_url) );
            call.setOperationName( "getNote" );
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "op2", XMLType.XSD_INT, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ARRAY );
            
            Object[] ob = new Object[]{url,sid};
            this.ret = (String[]) call.invoke(ob );
            if(this.ret[0] == null) this.ret[0] = "";
            adjust();
        }
        catch(Exception e){
            logger.error("unable to get user cart for "+sid,e);
            throw e;
        }
    }
    
    private void adjust(){
        //get length of discipline
        //adjust row of the textbox
        int rows = 9;
        int length = this.ret[3].length();
        logger.debug("Length of dis is "+length);
        
        if(rows < 30) rows = 8;
        else if(rows < 60) rows = 7;
        else if(rows < 80) rows = 6;
        else if(rows < 100) rows = 5;
        else if(rows > 101) rows = 4;
        //get ass study lenght
        if(this.ret[5].length() > 25) rows--;
        if(this.ret[1].length() > 25) rows--;
        this.srows = String.valueOf(rows);
        
        logger.debug("Number of rows is "+ rows);
        
        //adjust the values of the cart request
        if(this.ret[2] ==null) {
            this.ret[2] =  "N/A";
        }
        else{
            String[] facs = this.ret[2].split(" ");
            StringBuffer buff = new StringBuffer();
            for(int i = 0;i<facs.length;i++){
                buff.append(facs[i]+" ");
            }
            this.ret[2] = buff.toString();
        }
        
        this.ret[3] = (this.ret[3] !=null)? this.ret[3]:"N/A";
        
        if(this.ret[4] ==null) {
            this.ret[4] =  "N/A";
        }
        else{
            int int1 = Integer.parseInt(ret[4]);
            int secs = int1/1000;
            this.ret[4] = String.valueOf(secs);
        }
        
        this.ret[5] = (this.ret[5] != null)? this.ret[5].replaceAll("%20"," ") : "N/A";
        
        
    }
    
    //setter methods
    public void setUrl(String url){
        this.url = url;
    }
    
    public void setProperties(Properties prop){
        this.prop = prop;
    }
    
    public void setSid(String sid){
        this.sid = sid;
    }
    
    public String getNotes(){
        return ret[0];
    }
    
    public String getStudy(){
        return ret[5];
    }
    
    public String getTime(){
        return ret[4];
    }
    
    public String getFacilities(){
        return ret[2];
    }
    
    public String getDiscipline(){
        return ret[3];
    }
    
    public String getItem(){
        return ret[1];
    }
    
    public String getSrows(){
        return this.srows;
    }
    
    public String getUrl(){
        return this.url;
    }
    
    
    
}
