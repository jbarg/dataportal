package uk.ac.clrc.dataportal.authent;
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
    private static final String myProxyServerName = "wk-ibm1.dl.ac.uk";
    private static final String myProxyServerDN = "/C=UK/O=eScience/OU=CLRC/L=DL/CN=host/wk-ibm1.dl.ac.uk";
    private static final int myProxyServerPort = 7512;

// Data specific to delegate proxy
    private static final int delegateProxyLifetime = 3600; // seconds?


    public static GlobusProxy getProxy( String aUsername, String aPassPhrase, GlobusProxy thePortalProxy ) 
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

        MyProxy proxy = new MyProxy( myProxyServerName, myProxyServerPort );
         
        GlobusProxy delegateUserProxy = MyProxy.get( myProxyServerName, 
                                                     myProxyServerPort,
                                                     portalProxy, 
                                                     username,
                                                     userPassphrase,
                                                     delegateProxyLifetime,
                                                     myProxyServerDN );

        return delegateUserProxy;
    }
}

//*********************************************************************

