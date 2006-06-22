/*
 * NewMain.java
 *
 * Created on 31 March 2006, 15:45
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.clients.session;

import java.io.File;
import javax.ejb.EJB;
import uk.ac.dl.dp5.entity.Facility;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.sessionbeans.lookup.*;
import javax.naming.*;
import java.util.*;
import uk.ac.dl.dp5.sessionbeans.session.SessionRemote;
import uk.ac.dl.dp5.sessionbeans.session.TimerSession;
import uk.ac.dl.dp5.util.Certificate;
import uk.ac.dl.dp5.util.FacilityType;
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
            
            TimerSession sless = (TimerSession) ic.lookup("TimerServiceEJB");
            
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
