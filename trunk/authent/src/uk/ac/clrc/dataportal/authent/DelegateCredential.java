/*
 * DelegateCredential.java
 *
 * Created on 24 March 2004, 09:57
 */

package uk.ac.clrc.dataportal.authent;

import org.ietf.jgss.*;

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

    public static GSSCredential getProxy( String aUsername, String aPassPhrase,Integer lifetime, GSSCredential thePortalProxy ) 
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

        //MyProxy proxy = new MyProxy( AuthCtl.myProxyServerName, AuthCtl.myProxyServerPort );
         
        GSSCredential delegateUserProxy = org.globus.myproxy.MyProxy.get( AuthCtl.myProxyServerName, 
                                                     AuthCtl.myProxyServerPort,
                                                     portalProxy, 
                                                     username,
                                                     userPassphrase,
                                                   lifetime.intValue(),
                                                     AuthCtl.myProxyServerDN );

        return delegateUserProxy;
    }
}

//*********************************************************************


