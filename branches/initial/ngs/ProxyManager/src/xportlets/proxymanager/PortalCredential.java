/*
 * PortalCredential.java
 *
 * Created on 24 March 2004, 09:37
 */

package xportlets.proxymanager;

// CoG
import org.ietf.jgss.*;
import org.globus.gsi.*;
import org.globus.gsi.bc.*;
import org.globus.gsi.gssapi.GlobusGSSCredentialImpl;

// Java
import java.security.cert.X509Certificate;
import java.security.PrivateKey;


/**
 * Generates a proxy credential from hostcert.pem and hostkey.pem
 * @author Glen Drinkwater 
 */

public class PortalCredential 
{
    private static GSSCredential portalProxy;
    
    public static GSSCredential getPortalProxy() throws Exception
    {
        if(portalProxy == null) {
            createPortalProxy();
        }
        else    // proxy already exists
        {
            if(portalProxy.getRemainingLifetime() <= 0)    // has proxy expired?
            {
                createPortalProxy();
            }
        }
        
        return portalProxy;
    }
    
    
    private static void createPortalProxy() throws Exception 
    {
        X509Certificate portalCert = null;
        PrivateKey portalPrivateKey = null;
        X509Certificate caCerts[] = null;

        String hostcert = "/etc/grid-security/hostcert.pem";        
        String hostkey = "/etc/grid-security/hostkey.pem";
        
        try {
            portalCert = CertUtil.loadCertificate(hostcert) ;
            OpenSSLKey key = new BouncyCastleOpenSSLKey(hostkey);
            if(key.isEncrypted()) {
                // decrypt
                key.decrypt("password".getBytes());
            }

            portalPrivateKey = key.getPrivateKey();
            
            // Create delegate credential
            /* 
            GSSCredential credential2 = new GlobusGSSCredentialImpl(proxy, 
                                                                    GSSCredential.INITIATE_AND_ACCEPT);
            portalCert = CertUtil.loadCert(hostcert);
            
            caCerts = new X509Certificate[] { CertUtil.loadCert( AuthCtl.caCertFilename ) };
            
            SSLeayKeyHolder sslKey = new SSLeayKeyHolder(hostkey);
            
            portalPrivateKey = sslKey.getPrivateKey();
            */
        }
        
        catch( Exception e ) { throw e; }
        
        
        try {
            BouncyCastleCertProcessingFactory factory = BouncyCastleCertProcessingFactory.getDefault();
            GlobusCredential proxy = factory.createCredential(new X509Certificate[] {portalCert}, 
                                                              portalPrivateKey, 512, 3600, 
                                                              GSIConstants.DELEGATION_FULL);
            
            // Create delegate credential
            portalProxy = new GlobusGSSCredentialImpl(proxy, GSSCredential.INITIATE_AND_ACCEPT);
            
            // X509Certificate [] portalCertChain = new X509Certificate[] { portalCert };
            // portalProxy = new GlobusProxy( portalPrivateKey, portalCertChain, caCerts );
        }
        
        catch( Exception e ) { throw e; }
    }
}
