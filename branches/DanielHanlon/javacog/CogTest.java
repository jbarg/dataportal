/*
 * CogTest.java
 *
 * Created on 06 April 2001, 14:45
 */

package javacog;

//import org.globus.tools.*;
import org.globus.io.gass.server.*;
import org.globus.util.deactivator.*;
import org.globus.gram.*;
import java.io.*;

/**
 *
 * @author Daniel Hanlon
 * @version 0.1
 *
 */
public class CogTest extends java.lang.Object {
    
    //Gass server to hold stderr/stdout from command
    GassServer gs; 
    
    /** Creates new CogTest */
    public CogTest() {
        //System.setProperty("org.globus.debug.level","5");
        //System.out.println( System.getProperty("org.globus.debug.level") );
        //org.globus.tools.GlobusRun.main(
        //    new String[]{ "-o", "-r", "tcs10", "&(executable=/bin/uname)" } );
        //Do it again ourselves....
        
        String contact = getInput( "Enter remote resource contact:" );
        String executable;
        
        while( true ){
            try{
                gs = new GassServer();
                System.out.println( "Using GASS URL:" + gs.getURL() );
                Deactivator.registerDeactivation( new DeactivationHandler(){
                    public void deactivate(){
                        if ( gs != null ) gs.shutdown();
                    }
                } );
                
                
                
                executable = getInput( "Enter executable with full path:" );
                System.out.println( "Trying to run command " + executable + "\non resource " + contact );
                
                StringBuffer rslsb = new StringBuffer();
                
                //Perform RSL substitution to get results back
                rslsb.append( "&(rsl_substitution=(GLOBUSRUN_GASS_URL " + gs.getURL() + "))" );
                rslsb.append( "(executable=" + executable + ")" );
                rslsb.append( "(stderr=$(GLOBUSRUN_GASS_URL)/dev/stderr)" );
                rslsb.append( "(stdout=$(GLOBUSRUN_GASS_URL)/dev/stdout)" );
                
                /*
                gs.registerJobOutputStream( "out", System.out );
                gs.registerJobOutputStream( "err", System.err );
                 */
                
                System.out.println("RSL is" + rslsb.toString());
                
                GramJob gj = new GramJob( rslsb.toString() );
                gj.addListener( new GramJobListener(){
                    public void statusChanged( GramJob job ){
                        System.out.println( "Job: " + job.getRSL() + "\nStatus: " + job.getStatusAsString() );
                        switch( job.getStatus() ){
                            case GramJob.STATUS_FAILED:
                                System.out.println( "Problem..." );
                                System.out.println( "Error number = " + job.getError() );
                            case GramJob.STATUS_DONE:
                                Deactivator.deactivateAll();
                                break;
                            default:
                                System.out.println( "Doing nothing..." );
                        }
                    }
                });
                
                gj.request( contact );
                
            } catch( Exception e ){
                System.out.println( e.toString() );
                e.printStackTrace();
                Deactivator.deactivateAll();
                System.exit(1);
            }
        }
    }
    
    public static String getInput(String prompt) {
        System.out.print(prompt);
            try {
                BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
                System.out.println("");
                return in.readLine();
            } catch(IOException e) {
                return null;
            }
    }
    
    /** Main method
     * @param args Environment passed
     */    
    public static void main( String args[] ){
        new CogTest();
    }
}