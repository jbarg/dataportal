/*
 * NewMain.java
 *
 * Created on 31 March 2006, 15:45
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.clients.lookup;

import javax.ejb.EJB;
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.entity.Facility;
import uk.ac.dl.dp.coreutil.entity.ModuleLookup;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.coreutil.interfaces.LookupRemote;


import javax.naming.*;
import java.util.*;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
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
            //System.setProperty("org.omg.CORBA.ORBInitialHost","volga.dl.ac.uk");
            
            InitialContext ic = new InitialContext();
            //   LookupRemote sless = (LookupRemote) ic.lookup("uk.ac.dl.dp5.sessionbeans.lookup.LookupRemote");
            
            LookupRemote sless = (LookupRemote) ic.lookup(DataPortalConstants.LOOKUP);
            
            
            System.out.println("Looking for facilities type: "+DPFacilityType.WRAPPER);
            Collection<FacilityDTO> facs = (Collection<FacilityDTO>)sless.getFacilities(DPFacilityType.WRAPPER);
            
            for(FacilityDTO f : facs){
                System.out.println(f.getFacility()+" has connection "+f.getModuleType()+" "+f.isDataSetInFolders());
            }
            
            ProxyServers p = sless.getDefaultProxyServer();
            System.out.println(p.getProxyServerAddress());
            //   Collection<ProxyServers> pd = sless.getProxyServers();
            
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
