/*
 * CLI.java
 *
 * Created on 02 December 2002, 16:44
 */

package uk.ac.clrc.dataportal.lookup;

import org.apache.log4j.Logger;
import org.apache.log4j.BasicConfigurator;
/**
 *
 * @author  maw24
 */
public class CLI
{
    public static void main(String args[]) throws Exception
    {
        try {
        LookUpModule lum = new LookUpModule();
        String[] results;
        String[] facs =
        { "DataPortal", "MPIM", "BADC"};
        
        System.out.println("Running lookupEndpoint()");
        showResults(lum.lookupEndpoint(facs,"QNR"));
        
        System.out.println("\nRunning lookupWSDL()");
        showResults(lum.lookupWSDL(facs,"LOOKUP"));
        
        System.out.println("\nRunning getServices()");
        showResults(lum.getServices("DataPortal"));
        
        System.out.println("\nRunning getServiceTypes()");
        showResults(lum.getServiceTypes());
        
        System.out.println("\nRunning getFacilities()");
        showResults(lum.getFacilities());
        
        System.out.println("\nRunning getFacilitiesForID()");
        showResults(lum.getFacilitiesForID("TEST"));
        }
        catch (Exception e)
        {
            e.printStackTrace();
            throw e;
        }
    }
    private static void showResults(String[] results)
    {
        if (results != null)
        {
            for (int i=0;i<results.length;i++)
            {
                System.out.println(results[i]);
            }
        }
    }
}
