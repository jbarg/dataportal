/*
 * TestICATSingleton.java
 * JUnit based test
 *
 * Created on 25 June 2007, 11:23
 */

package uk.ac.dp.icatws;


import javax.xml.ws.BindingProvider;
import junit.framework.JUnit4TestAdapter;
import static org.junit.Assert.*;
import org.junit.Test;
/**
 *
 * @author gjd37
 */
public class LogonICATSingleton  {
    
    
    
    protected void setUp() throws Exception {
    }
    
    protected void tearDown() throws Exception {
    }
       
    
    @Test
    public void timeTest() throws Exception{
        System.out.println("Singleton test");
        
        long time = System.currentTimeMillis();
        ICAT port = ICATSingleton.getInstance("https://volga.dl.ac.uk:8181/ICATService/ICAT?wsdl");
       
        System.out.println( ((BindingProvider)port).getRequestContext().get(BindingProvider.ENDPOINT_ADDRESS_PROPERTY));
        
        System.out.println(port.login("gjd37", "kkkkkk"));
    }
    
    /**
     * suite method automatically generated by JUnit module
     */
    public static junit.framework.Test suite(){
        return new JUnit4TestAdapter(LogonICATSingleton.class);
    }
}