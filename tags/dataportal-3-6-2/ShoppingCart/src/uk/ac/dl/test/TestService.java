/*
 * TestService.java
 *
 * Created on 09 April 2003, 08:54
 */

package dataportal.ShoppingCart.src.uk.ac.dl.test;
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;
import org.jdom.*;
import uk.ac.dl.xml.*;

/**
 *
 * @author  gjd37
 */
public class TestService  {
    
    public static void main(String[] args){
        new TestService(args[0]);
    }
    
    public TestService(String sid){
        try{
            Service  service = new Service();
            Call     call    = (Call) service.createCall();
            
            
            call.setTargetEndpointAddress( new java.net.URL("http://escdmg.dl.ac.uk:8080/shopcart/services/ShoppingCart") );
            
            call.setOperationName( "getCart" );
            
            call.addParameter( "op1", XMLType.XSD_INT, ParameterMode.IN );
            
            call.setReturnType( XMLType.SOAP_ELEMENT );
            
            Object[] ob = new Object[]{new Integer(sid)};
            
            
            
            org.w3c.dom.Element ret = (org.w3c.dom.Element) call.invoke(ob );
            
            org.jdom.input.DOMBuilder buildert = new org.jdom.input.DOMBuilder();
            
            org.jdom.Element el = buildert.build(ret);
            el.detach();
            Document  doc1  = new org.jdom.Document(el);
            
            Saver.save(doc1,"c:/checkshop.xml");
        }
        catch(Exception e){
            System.out.println(e);
        }
        
    }
    
}
