/*package uk.ac.clrc.dataportal.authent;
//***************************************************************************
// Class to retrieve delegate proxy retrieved from MyProxy Server           *
//                                                                          *
// Written by rty on 04.02.03                                               *
//                                                                          *
//***************************************************************************

import org.globus.security.GlobusProxy;
import org.globus.security.GlobusProxyException;
import org.globus.myproxy.MyProxy;
import org.globus.myproxy.MyProxyException;

//*********************************************************************

public abstract class DelegateProxy 
{
// Data specific to MyProxyServer

    public static GlobusProxy getProxy( String aUsername, String aPassPhrase,Integer lifetime, GlobusProxy thePortalProxy ) 
                                              throws GlobusProxyException, MyProxyException
    {
        GlobusProxy portalProxy = thePortalProxy;
        String username = aUsername;
        String userPassphrase = aPassPhrase;

        // check that portal proxy hasn't expired
        if( portalProxy == null || portalProxy.getTimeLeft() <= 0 ) 
        {
            throw new GlobusProxyException( "Invalid portal proxy" );
        }

        MyProxy proxy = new MyProxy( AuthCtl.myProxyServerName, AuthCtl.myProxyServerPort );
         
        GlobusProxy delegateUserProxy = MyProxy.get( AuthCtl.myProxyServerName, 
                                                     AuthCtl.myProxyServerPort,
                                                     portalProxy, 
                                                     username,
                                                     userPassphrase,
                                                   lifetime.intValue(),
                                                     AuthCtl.myProxyServerDN );

        return delegateUserProxy;
    }
}
*/
//*********************************************************************

