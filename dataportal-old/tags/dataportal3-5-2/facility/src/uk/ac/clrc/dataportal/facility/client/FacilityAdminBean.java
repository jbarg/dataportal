/*
 * FacilityAdminClient.java
 *
 * Created on 15 January 2003, 08:37
 */

package uk.ac.clrc.dataportal.facility.client;

import org.apache.axis.AxisFault;
import org.apache.axis.client.Service;
import org.apache.axis.client.Call;
import org.apache.axis.message.SOAPBodyElement;
import org.apache.axis.utils.XMLUtils;

import org.w3c.dom.*;
import org.xml.sax.InputSource;

import java.io.StringReader;
import java.net.URL;
import java.util.Vector;

public class FacilityAdminBean {
    
    private String endPoint = "";
    private String payLoad = "";
    
    public String call() throws Exception {
        
        String response=null;
        
        try {
            
            Service service = new Service();
            Call call = (Call) service.createCall();
            call.setTargetEndpointAddress(new URL(endPoint));
            
            SOAPBodyElement[] requestSBE = new SOAPBodyElement[1];
            
            requestSBE[0] = new SOAPBodyElement(XMLUtils.newDocument(new InputSource(new StringReader(payLoad))).getDocumentElement());
            
            Vector responseSBE = (Vector) call.invoke(requestSBE);
            SOAPBodyElement resElt = (SOAPBodyElement)responseSBE.get(0);
            
            response = XMLUtils.ElementToString(resElt.getAsDOM());
            
        }
        catch (Exception e) {
            throw e;
        }
        return response;
    }
    
    public void setPayLoad(String payLoad) {
        this.payLoad = payLoad;
    }
    
    public String getPayLoad() {
        return this.payLoad;
    }
    
    public String getEndPoint() {
        return endPoint;
    }
    
    public void setEndPoint(String endPoint) {
        this.endPoint = endPoint;
    }
}
