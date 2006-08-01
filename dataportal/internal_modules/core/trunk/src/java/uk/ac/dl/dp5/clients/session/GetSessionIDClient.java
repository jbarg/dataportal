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
import uk.ac.dl.dp5.clients.dto.SessionDTO;
import uk.ac.dl.dp5.entity.Facility;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.sessionbeans.lookup.*;
import javax.naming.*;
import java.util.*;
import uk.ac.dl.dp5.sessionbeans.session.SessionRemote;
import uk.ac.dl.dp5.util.Certificate;
import uk.ac.dl.dp5.util.DPFacilityType;
import uk.ac.dl.dp5.entity.Role;
import uk.ac.dl.dp5.util.DataPortalConstants;
/**
 *
 * @author gjd37
 */
public class GetSessionIDClient {
    
    /** Creates a new instance of NewMain */
    public GetSessionIDClient() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        String sid = "565a63dd-5715-4ef2-aac3-c69213082d36";
        try{
            InitialContext ic = new InitialContext();
            
            SessionRemote sless = (SessionRemote) ic.lookup(DataPortalConstants.SESSION);
            //sless.isValid("ed046b97-3fac-49d9-9b70-8df0031e7d05");
            
            SessionDTO session =  sless.getSession(sid);
            System.out.println("Got session : "+session.getCredentialType());
            
            /*Role role =  (Role)sless.getSessionI(sid);
            System.out.println("Got session : "+role.getModTime() );
            
           role.setModTime(new Date());
            sless.addSessionI(role);*/
            
            
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
