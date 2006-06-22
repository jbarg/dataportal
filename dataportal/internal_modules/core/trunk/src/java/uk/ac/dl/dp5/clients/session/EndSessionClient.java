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
import uk.ac.dl.dp5.util.Certificate;
import uk.ac.dl.dp5.util.FacilityType;
/**
 *
 * @author gjd37
 */
public class EndSessionClient {
    
    /** Creates a new instance of NewMain */
    public EndSessionClient() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        try{
            
            InitialContext ic = new InitialContext();
            
            SessionRemote sless = (SessionRemote) ic.lookup("SessionEJB");
            
            boolean session =  sless.logout("4ba83fdd-4081-48b9-888b-910462f34bf5");
            System.out.println("Got session : "+session);
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
