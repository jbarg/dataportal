package org.apache.jetspeed.modules.actions.portlet;

// Java stuff
import java.util.*; 
import java.net.*;

// Jetspeed Stuff
import org.apache.jetspeed.portal.portlets.VelocityPortlet;
import org.apache.jetspeed.modules.actions.portlets.VelocityPortletAction;
import org.apache.jetspeed.om.security.JetspeedUser;
import org.apache.jetspeed.services.rundata.JetspeedRunData;

// Turbine stuff
import org.apache.turbine.util.Log;
import org.apache.turbine.util.RunData;

// Velocity Stuff
import org.apache.velocity.context.Context;


/**
 * A Grid Ping class to build VelocityPortlet actions.
 * 
 */

public class DemoRegistrationAction extends VelocityPortletAction
{
    // UI variables
    private static final String NAME         = "name";
    private static final String INSTITUITION = "instituition";
    private static final String EMAIL        = "email";
    private static final String ERROR        = "error";
    private static final String PORTLET      = "portlet";

    // Global class variables
    //String name = null;
    //String instituition = null;
    //String email = null;
 

    /** 
     * Subclasses should override this method if they wish to
     * build specific content when maximized. Default behavior is
     * to do the same as normal content.
     */
    protected void buildMaximizedContext( VelocityPortlet portlet, 
                                          Context context,
                                          RunData rundata )
    {
        buildNormalContext(portlet, context, rundata);        
    }

    /** 
     * Subclasses should override this method if they wish to
     * provide their own customization behaviour.
     * Default is to use Portal base customizer action
     */
    protected void buildConfigureContext( VelocityPortlet portlet, 
                                          Context context,
                                          RunData rundata )
    {
        buildNormalContext(portlet, context, rundata);
    }

    /** 
     * Subclasses must override this method to provide default behavior 
     * for the portlet action
     */
    protected void buildNormalContext( VelocityPortlet portlet, 
                                       Context context,
                                       RunData rundata )
    {
        Log.info("(Demo-portlet) Entering buildNormalContext ...");

        // Read or set initial values

        Log.info("(Demo-portlet) Leaving buildNormalContext ...");
    }

    /** 
     * Called when user clicks the "Submit" button from main screen
     * and sets the UI template to portlet-demo-register.vm
     */
    public void doGet_screen( RunData runData, Context context )
    {
        String errMsg = "";

        context.put( NAME, "" );
        context.put( INSTITUITION, "" );
        context.put( EMAIL, "" );
        
        Log.info("(Demo-portlet) doGet_screen()" );
       
        // Retrieve screen data
        Parameters param = getParameters( runData, context );
        String name = param.name;
        String instituition = param.instituition;
        String email = param.email;
        
        Log.info("(Demo-portlet) Before Screen values: " + name + " " + instituition + " " + email );

        // Store information
        if (name != null && !name.equals("")) {
            runData.getUser().setPerm( NAME, name );            
            Log.info("(Demo-portlet) NAME: " + name);
        }
        else {
            errMsg = "Name ";            
            Log.info("(Demo-portlet) ERROR: " + errMsg);
        }
        if (instituition != null && !instituition.equals("")) {
            runData.getUser().setPerm( INSTITUITION, instituition );            
            Log.info("(Demo-portlet) INSTITUITION: " + instituition);
        }
        else {
            errMsg = errMsg + "Instituition ";
            Log.info("(Demo-portlet) ERROR: " + errMsg);
        }
        if (email != null && !email.equals("")) {
            runData.getUser().setPerm( EMAIL, email );            
            Log.info("(Demo-portlet) EMAIL: " + email);
        }
        else {
            errMsg = errMsg + "Email ";
            Log.info("(Demo-portlet) ERROR: " + errMsg);
        }

        if (errMsg != null) {
            context.put( ERROR, "Missing Parameter(s): " + errMsg );
            
            setTemplate( runData, "portlet-demo-main" );
        }
        else {
            context.put( NAME, name );
            context.put( INSTITUITION, instituition );
            context.put( EMAIL, email );

            setTemplate( runData, "portlet-demo-register" );
        }
            
        Log.info("(Demo-portlet) Screen values: " + name + " " + instituition + " " + email );

    }
    

    /** 
     * Called when user clicks the "Reset" button from main screen
     * and sets the UI template to portlet-demo-main.vm
     */
    public void doReset( RunData runData, Context context )
    {
        // Set UI variables to null
        String name = null;
        String instituition = null;
        String email = null;

        runData.getUser().setPerm( NAME, name );
        runData.getUser().setPerm( INSTITUITION, instituition );
        runData.getUser().setPerm( EMAIL, email );
        
        context.put( NAME, name );
        context.put( INSTITUITION, instituition );
        context.put( EMAIL, email );

        setTemplate( runData, "portlet-demo-main" );
    }

    /**
     * Retrieve parameters from the servlet context and place them in 
     * structure Parameters.
     */
    private Parameters getParameters( RunData runData, Context context )
    {
        Parameters params = new Parameters();

        params.name = runData.getParameters().getString( NAME ).trim();
        params.instituition = runData.getParameters().getString( INSTITUITION ).trim();
        params.email = runData.getParameters().getString( EMAIL ).trim();

        return params;
    }
    
    /**
     * Wrapper class for storing parameters of this portlet.
     */
    private class Parameters 
    {
        public String name = null;
        public String instituition = null;
        public String email = null;
    }
    
}   // END - DemoRegistrationAction 


