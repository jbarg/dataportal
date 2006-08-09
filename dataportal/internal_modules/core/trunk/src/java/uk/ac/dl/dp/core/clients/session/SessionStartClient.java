/*
 * NewMain.java
 *
 * Created on 31 March 2006, 15:45
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.clients.session;

import java.io.File;
import javax.ejb.EJB;
import uk.ac.dl.dp.coreutil.entity.Facility;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;


import javax.naming.*;
import java.util.*;
import uk.ac.dl.dp.coreutil.interfaces.SessionRemote;
import uk.ac.dl.dp.coreutil.util.Certificate;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
/**
 *
 * @author gjd37
 */
public class SessionStartClient {
    
    /** Creates a new instance of NewMain */
    public SessionStartClient() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        try{
            
            InitialContext ic = new InitialContext();
            
            SessionRemote sless = (SessionRemote) ic.lookup(DataPortalConstants.SESSION);
            
            Certificate cert = new Certificate(new File("c:/x509up_u_gjd37"));
            System.out.println("lifetime left : "+cert.getLifetime());
            System.out.println("lifetime left : "+cert.getLifetime());
            System.out.println("lifetime left : "+cert.getDn());
            Certificate cert2 = new Certificate(cert.getCredential());
            System.out.println("lifetime left : "+cert2.getDn());
            System.out.println("lifetime left : "+cert2.getCredential().getClass());
            
            
         //   String sid =   sless.login(cert.getStringRepresentation());
           // System.out.println("Started session with sid: "+sid);
            
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
