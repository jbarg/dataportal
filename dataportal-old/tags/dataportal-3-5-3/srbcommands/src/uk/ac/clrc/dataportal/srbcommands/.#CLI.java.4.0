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
    public static void main(String args[])
    {
        LookUpModule lum = new LookUpModule();
        String[] results;
        String[] facs =
        { "BADC", "DataPortal", "MPIM", "ISIS", "SRD", "MAWTEST"};
        
        System.out.println("Running lookupEndpoint()");
        showResults(lum.lookupEndpoint(facs,"acm"));
        
        System.out.println("\nRunning lookupWSDL()");
        showResults(lum.lookupWSDL(facs,"acm"));
        
        System.out.println("\nRunning getServices()");
        showResults(lum.getServices("BADC"));
        
        System.out.println("\nRunning getServiceTypes()");
        showResults(lum.getServiceTypes());
        
        System.out.println("\nRunning getFacilities()");
        showResults(lum.getFacilities());
        
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
