/*
 * GlobusJob.java
 *
 * Created on 27 June 2001, 09:51
 */

package dlv;
import javax.swing.*;
import org.globus.gram.*;
import org.globus.util.deactivator.*;
import org.globus.security.*;
import org.globus.io.gass.server.*;

import java.security.*;
import java.security.cert.*;
import java.io.*;

/**
 *
 * @author  daniel
 * @version 
 */
public class GlobusJob implements Runnable{

    private GlobusProxy gp;
    private OutputFrame std;
    
    /** Holds value of property executable. */
    private String executable;
    
    /** Holds value of property shellPrefix. */
    private String shellExecutable = "/bin/bash";
    
    /** Utility field used by bound properties. */
    private java.beans.PropertyChangeSupport propertyChangeSupport =  new java.beans.PropertyChangeSupport(this);
    
    /** Holds value of property shellCommandArgument. */
    private String shellCommandArgument = "-c";
    
    /** Holds value of property runViaShell. */
    private boolean runViaShell = true;
    
    /** Holds value of property rsl. */
    private String rsl;
    
    private GassServer gassServer;
    
    /** Holds value of property contact. */
    private String contact;
    
    /** Creates new GlobusJob */
    public GlobusJob( OutputFrame std ) {
        this.std = std;
    }

    /** Getter for property executable.
     * @return Value of property executable.
     */
    public String getExecutable() {
        return this.executable;
    }
    
    /** Setter for property executable.
     * @param executable New value of property executable.
     */
    public void setExecutable(String executable) {
        this.executable = executable.trim();
    }
    
    /** Create the proxy for the interaction. */ 
    private void createProxy(){
        try{
            if ( !GlobusProxy.defaultUserProxyExists() ) throw new GlobusProxyException( "" );
            gp = GlobusProxy.getDefaultUserProxy();
        }
        catch( GlobusProxyException gpe ){
            new JOptionPane( "First please perform grid-proxy-init.", JOptionPane.ERROR_MESSAGE ).createDialog(null,"");
            std.err.println( gpe.toString() );
            gpe.printStackTrace( std.err );
        }
        std.out.println( "Valid proxy found..." );
    }
               
    private void createRSL(){
        StringBuffer rslsb = new StringBuffer();
        //if ( gassServer == null ) System.err.println( "gassServer==null!" );
        //Perform RSL substitution to get results back
        try{
        rslsb.append( "&(rsl_substitution=(GLOBUSRUN_GASS_URL " + gassServer.getURL() + "))" );
        }
        catch( java.net.UnknownHostException uhe ){
            std.err.println( "gassServer.getURL() failed" );
        }
        rslsb.append( "(executable=" );
        if ( runViaShell ){
            //Reformat to launch a shell first and run the command in it.
            rslsb.append( shellExecutable + ")" );
            rslsb.append( "(arguments=" + shellCommandArgument + " " + executable );
        } else {
            //Run command directly
            int x = executable.indexOf( ' ' );
            if ( x==-1 ){
                //No space -> no arguments...
                rslsb.append( executable );
            } else {
                //space -> arguments...
                rslsb.append( executable.substring(1,x-1) + ")" );
                rslsb.append( "(arguments=" + executable.substring(x+1) );
            }
        }
        //Finishing parenthesis
        rslsb.append( ")" );
        //Grab std.err and std.out
        rslsb.append( "(stderr=$(GLOBUSRUN_GASS_URL)/dev/stderr)" );
        rslsb.append( "(stdout=$(GLOBUSRUN_GASS_URL)/dev/stdout)" );
        rsl = rslsb.toString();
        std.out.println( "RSL=" + rsl );
    }
    
    private void createGassServer(){
        System.out.println( "In createGassServer()" );
        try{
            gassServer = new GassServer();
            std.out.println( "Using GASS URL:" + gassServer.getURL() );
            Deactivator.registerDeactivation( new DeactivationHandler(){
                public void deactivate(){
                    if ( gassServer != null ) gassServer.shutdown();
                }
            } );
            gassServer.registerJobOutputStream( "out", std.out );
            gassServer.registerJobOutputStream( "err", std.err );
            std.out.println("Registered cout and cerr");
        }
        catch( GlobusProxyException gpe ){
            System.err.println( gpe.toString() );
            gpe.printStackTrace( System.err );
        }
        catch( java.io.IOException ioe ){
            System.out.println( ioe.toString() );
            ioe.printStackTrace( System.err );
            Deactivator.deactivateAll();
        }
    }
    
    /** Execute the command.*/
    public void run(){
        createProxy();
        System.out.println( "Done createProxy()" );
        createGassServer();
        System.out.println( "Done createGassServer()" );
        createRSL();
        System.out.println( "Done createRSL()" );
        GramJob gramJob = new GramJob(gp,rsl);
        gramJob.addListener( new GramJobListener(){
            public void statusChanged(GramJob job) {
                std.out.println( "Job: " + job.getRSL() + "\nStatus: " + job.getStatusAsString() );
                switch( job.getStatus() ){
                    case GramJob.STATUS_FAILED:
                        std.out.println( "Problem..." );
                        std.out.println( "Error number = " + job.getError() );
                    case GramJob.STATUS_DONE:
                        Deactivator.deactivateAll();
                        break;
                    default:
                        std.out.println( "Doing nothing..." );
                }
            }
        });
        
        try{
            gramJob.request( contact );
        }
        catch( GramException ge ){
            System.out.println( ge );
        }
        catch( GlobusProxyException gpe ){
            System.out.println( gpe );
        }
    }
    
    /** Add a PropertyChangeListener to the listener list.
     * @param l The listener to add.
     */
    public void addPropertyChangeListener(java.beans.PropertyChangeListener l) {
        propertyChangeSupport.addPropertyChangeListener(l);
    }
    
    /** Removes a PropertyChangeListener from the listener list.
     * @param l The listener to remove.
     */
    public void removePropertyChangeListener(java.beans.PropertyChangeListener l) {
        propertyChangeSupport.removePropertyChangeListener(l);
    }
    
    /** Getter for property shellPrefix.
     * @return Value of property shellPrefix.
     */
    public String getShellExecutable() {
        return this.shellExecutable;
    }
    
    /** Setter for property shellPrefix.
     * @param shellPrefix New value of property shellPrefix.
     */
    public void setShellExecutable(String shellPrefix) {
        String oldShellExecutable = this.shellExecutable;
        this.shellExecutable = shellExecutable;
        propertyChangeSupport.firePropertyChange("shellExecutable", oldShellExecutable, shellExecutable);
    }
    
    /** Getter for property shellCommandArgument.
     * @return Value of property shellCommandArgument.
     */
    public String getShellCommandArgument() {
        return this.shellCommandArgument;
    }
    
    /** Setter for property shellCommandArgument.
     * @param shellCommandArgument New value of property shellCommandArgument.
     */
    public void setShellCommandArgument(String shellCommandArgument) {
        this.shellCommandArgument = shellCommandArgument;
    }
    
    /** Getter for property runViaShell.
     * @return Value of property runViaShell.
     */
    public boolean isRunViaShell() {
        return this.runViaShell;
    }
    
    /** Setter for property runViaShell.
     * @param runViaShell New value of property runViaShell.
     */
    public void setRunViaShell(boolean runViaShell) {
        this.runViaShell = runViaShell;
    }
    
    /** Getter for property rsl.
     * @return Value of property rsl.
     */
    public String getRsl() {
        return this.rsl;
    }
    
    /** Setter for property rsl.
     * @param rsl New value of property rsl.
     */
    public void setRsl(String rsl) {
        this.rsl = rsl;
    }
    
    /** Getter for property contact.
     * @return Value of property contact.
     */
    public String getContact() {
        return this.contact;
    }
    
    /** Setter for property contact.
     * @param contact New value of property contact.
     */
    public void setContact(String contact) {
        this.contact = contact;
    }
    
}
