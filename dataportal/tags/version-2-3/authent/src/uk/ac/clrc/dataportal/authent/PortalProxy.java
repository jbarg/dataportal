/*package uk.ac.clrc.dataportal.authent;
//***************************************************************************
// Uses Cog kit to create a GlobusProxy from portal's private key and       *
// certificate and stores on MyProxy Server.                                *
//                                                                          *
// Written by rty on 05.02.03                                               *
//***************************************************************************

import org.globus.security.GlobusProxy;
import org.globus.security.GlobusProxyException;
import org.globus.security.CertUtil;
import org.globus.security.SSLeayKeyHolder;

import org.globus.myproxy.MyProxy;
import org.globus.myproxy.MyProxyException;

import java.security.PrivateKey;
import java.security.cert.X509Certificate;

//*********************************************************************

public abstract class PortalProxy
{


    private static GlobusProxy portalProxy;

//********************

    public static GlobusProxy getPortalProxy() throws Exception
    {

        if( portalProxy == null )
        {
            createPortalProxy();
        }
        else    // proxy already exists
        {
            if( portalProxy.getTimeLeft() <= 0 )    // has proxy expired?
            {
                createPortalProxy();
            }
        }

        return portalProxy;
    }

//********************

    private static void createPortalProxy() throws Exception
    {
        X509Certificate portalCert = null;
        PrivateKey portalPrivateKey = null;
        X509Certificate caCerts[] = null;

        try
        {
            portalCert = CertUtil.loadCert( AuthCtl.portalCertFilename );

            caCerts = new X509Certificate[] { CertUtil.loadCert( AuthCtl.caCertFilename ) };

            SSLeayKeyHolder sslKey = new SSLeayKeyHolder( AuthCtl.portalPrivateKeyFilename );

            portalPrivateKey = sslKey.getPrivateKey();
        }

        catch( Exception e ) { throw e; }


        try
        {
            X509Certificate [] portalCertChain = new X509Certificate[] { portalCert };
            portalProxy = new GlobusProxy( portalPrivateKey, portalCertChain, caCerts );
        }

        catch( Exception e ) { throw e; }
    }

//********************

}*/

//*********************************************************************
