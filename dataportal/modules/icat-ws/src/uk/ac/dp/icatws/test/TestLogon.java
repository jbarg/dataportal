/*
 * TestLogon.java
 * 
 * Created on 01-Oct-2007, 11:04:38
 * 
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dp.icatws.test;

import uk.ac.dp.icatws.ICATSingleton;

/**
 *
 * @author gjd37
 */
public class TestLogon {

    /** Creates a new instance of Main */
    public static String login(String username, String password) throws Exception{
       
        try {
            long time = System.currentTimeMillis();
            
            // TODO process result here
            java.lang.String result = ICATSingleton.getInstance("https://escvig6.dc.dl.ac.uk:8181/ICATService/ICAT?wsdl").login(username, password);
            
            System.out.println(" Logged in successfully with SID = "+result);
            
            System.out.println("\nTime taken: "+(System.currentTimeMillis() - time)/1000f+" seconds");
            System.out.println("------------------------------------------------------------------\n");
            assert true;
            return result;
        } catch (Exception ex) {
            System.out.println("Exception logging in\n"+ex);
            assert false;
            throw ex;
            // TODO handle custom exceptions here
        }
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws Exception {
        // TODO code application logic here
        login(System.getProperty("user.name"), System.getProperty("usersso.password"));
        
    }

}
