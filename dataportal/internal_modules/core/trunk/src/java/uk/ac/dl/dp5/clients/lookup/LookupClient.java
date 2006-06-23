/*
 * NewMain.java
 *
 * Created on 31 March 2006, 15:45
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.clients.lookup;

import javax.ejb.EJB;
import uk.ac.dl.dp5.clients.dto.FacilityDTO;
import uk.ac.dl.dp5.entity.Facility;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.sessionbeans.lookup.*;

import javax.naming.*;
import java.util.*;
import uk.ac.dl.dp5.util.DPFacilityType;
/**
 *
 * @author gjd37
 */
public class LookupClient {
    
    /** Creates a new instance of NewMain */
    public LookupClient() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        try{
            //Context ctx  =  getContext();
          /*  Hashtable env = new Hashtable();
            env.put(Context.INITIAL_CONTEXT_FACTORY, "org.jnp.interfaces.NamingContextFactory");
            env.put(Context.PROVIDER_URL, "escvig6.dl.ac.uk");
            env.put(Context.URL_PKG_PREFIXES, "org.jboss.naming:org.jnp.interfaces" );
           * Context ctx = new InitialContext(env);
           */
            // InitialContext ctx = new InitialContext();
            
            //LookupBean  remote = (LookupBean)ctx.lookup("LookupBeanStateless");
            
            InitialContext ic = new InitialContext();
            //   LookupRemote sless = (LookupRemote) ic.lookup("uk.ac.dl.dp5.sessionbeans.lookup.LookupRemote");
            
            LookupRemote sless = (LookupRemote) ic.lookup("LookupEJB");
            System.out.println("Looking for facilities type: "+DPFacilityType.ACM);
            Collection<FacilityDTO> facs = (Collection<FacilityDTO>)sless.getFacilities(DPFacilityType.WRAPPER);
            
            for(FacilityDTO f : facs){
                System.out.println(f.getFacility()+" has connection "+f.getModuleType());
            }
            
            
            
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
