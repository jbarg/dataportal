/*
 * DelegateCredential.java
 *
 * Created on 24 March 2004, 09:57
 */

package uk.ac.clrc.dataportal.authent;

import org.ietf.jgss.*;
import org.globus.common.*;
import org.gridforum.jgss.ExtendedGSSManager;
import org.gridforum.jgss.ExtendedGSSCredential;
import org.globus.myproxy.*;
/**
 *
 * @author  gjd37
 */
public abstract class DelegateCredential 
{
// Data specific to MyProxyServer

    public static GSSCredential getProxy( String aUsername, String aPassPhrase,Integer lifetime,String servername, String dn, Integer port , GSSCredential thePortalProxy ) 
                                              throws Exception, MyProxyException
    {
        GSSCredential portalProxy = thePortalProxy;
        String username = aUsername;
        String userPassphrase = aPassPhrase;

        // check that portal proxy hasn't expired
        if( portalProxy == null || portalProxy.getRemainingLifetime() <= 0 ) 
        {
            throw new Exception( "Invalid portal proxy" );
        }
         
        org.globus.myproxy.MyProxy proxy = new org.globus.myproxy.MyProxy( servername, lifetime.intValue() );
         
        GSSCredential delegateUserProxy = proxy.get( servername, 
                                                     port.intValue(),
                                                     portalProxy, 
                                                     username,
                                                     userPassphrase,
                                                   lifetime.intValue(),
                                                     dn );

        return delegateUserProxy;
    }
}

//*********************************************************************


