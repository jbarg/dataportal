/*
 * NewMain.java
 *
 * Created on 31 March 2006, 15:45
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.clients.session;

import javax.ejb.EJB;
import javax.naming.*;
import java.util.*;
import uk.ac.dl.dp.coreutil.interfaces.TimerServiceRemote;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
public class TimerServiceClient {
    
    /** Creates a new instance of NewMain */
    public TimerServiceClient() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        try{
            
            InitialContext ic = new InitialContext();
            
            TimerServiceRemote sless = (TimerServiceRemote) ic.lookup(DataPortalConstants.TIMER);
            
           sless.createTimer(1000*10,1000*60*30);
            //
        }catch(Exception e){
            
            e.printStackTrace();
            
        }
    }
    
    public static Context getContext() throws Exception{
        Hashtable env = new Hashtable();
        env.put(Context.INITIAL_CONTEXT_FACTORY, "org.jnp.interfaces.NamingContextFactory");
        env.put(Context.PROVIDER_URL, "localhost");
        env.put(Context.URL_PKG_PREFIXES, "org.jboss.naming:org.jnp.interfaces" );
        Context ctx = new InitialContext(env);
        return ctx;
    }
    
}
