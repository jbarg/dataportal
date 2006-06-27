/*
 * DelegateCredential.java
 *
 * Created on 24 March 2004, 09:57
 */

package uk.ac.dl.dp5.util.cog;

import java.security.cert.CertificateExpiredException;
import org.apache.log4j.Logger;
import org.ietf.jgss.*;
import org.globus.common.*;
import org.gridforum.jgss.ExtendedGSSManager;
import org.gridforum.jgss.ExtendedGSSCredential;
import org.globus.myproxy.*;
import uk.ac.dl.dp5.util.DataPortalConstants;
/**
 *
 * @author  gjd37
 */
public abstract class DelegateCredential 
{
// Data specific to MyProxyServer

     static Logger log = Logger.getLogger(DelegateCredential.class);
    
    public static GSSCredential getProxy( String aUsername, String aPassPhrase,int lifetime, GSSCredential thePortalProxy ) 
                                              throws GSSException,  MyProxyException
    {
        GSSCredential portalProxy = thePortalProxy;
        String username = aUsername;
        String userPassphrase = aPassPhrase;

        // check that portal proxy hasn't expired
        if( portalProxy == null || portalProxy.getRemainingLifetime() <= 0 ) 
        {
            throw new MyProxyException( "Invalid server portal proxy: "+portalProxy.getName().toString() );
        }
         
        log.info("Server Proxy Ok");
        
        org.globus.myproxy.MyProxy proxy = new org.globus.myproxy.MyProxy( DataPortalConstants.MYPROXY_HOST, DataPortalConstants.MYPROXY_PORT );
        log.info("Connecting to "+DataPortalConstants.MYPROXY_HOST+":"+DataPortalConstants.MYPROXY_PORT); 
        GSSCredential delegateUserProxy = proxy.get( DataPortalConstants.MYPROXY_HOST, DataPortalConstants.MYPROXY_PORT, portalProxy, username, userPassphrase, lifetime *3600 /*turn into seconds*/, DataPortalConstants.MYPROXY_DN );
        log.info("Retrieved user proxy.");
        return delegateUserProxy;
    }
}

//*********************************************************************


