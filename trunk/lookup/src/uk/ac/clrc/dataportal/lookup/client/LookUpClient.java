/*
 * LookUpClient.java
 *
 * Created on 02 December 2002, 18:37
 */

package uk.ac.clrc.dataportal.lookup.client;

/**
 *
 * @author  maw24
 */
public class LookUpClient {
    
    public static void main(String [] args) throws Exception {
        LookUpModuleService service = new LookUpModuleServiceLocator();
        
        LookUpModule port = service.getLookUpService();
//        String[] facs = { "BADC", "DataPortal", "MPIM", "ISIS", "SRD", "MAWTEST" };
        String[] facs = port.getFacilities();
        String[] results = port.lookupEndpoint(facs, "ACL");
        
        for (int i=0;i<results.length;i++) {
            System.out.println(facs[i] + "=" + results[i]);
        }
    }
}
