/*
 * LookUpClient.java
 *
 * Created on 02 December 2002, 18:37
 */

package uk.ac.clrc.dataportal.authent.lookupclient;

/**
 *
 * @author  maw24
 */
public class LookUpClient {
    
    public static void main(String [] args) throws Exception {
        LookUpModuleService service = new LookUpModuleServiceLocator();
        
        LookUpModule port = service.getLookUpService();
        //String[] results = port.getFacilities();
        String[] results = { "Dataportal" };
        String[] results2 = port.lookupEndpoint(results, "SESSION");
        
        for (int i=0;i<results2.length;i++) {
            System.out.println(results2[i]);
        }
    }
}
