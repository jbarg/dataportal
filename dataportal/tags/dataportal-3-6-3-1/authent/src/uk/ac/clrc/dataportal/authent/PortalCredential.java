/*
 * PortalCredential.java
 *
 * Created on 24 March 2004, 09:37
 */

package uk.ac.clrc.dataportal.authent;

import org.ietf.jgss.*;
import org.globus.gsi.*;
import org.globus.gsi.bc.*;
import org.globus.gsi.gssapi.GlobusGSSCredentialImpl;


import java.security.cert.X509Certificate;
import java.security.PrivateKey;
/**
 *
 * @author  gjd37
 */
public class PortalCredential {
    
    private static GSSCredential portalProxy;
    
    
    public static GSSCredential getPortalProxy() throws Exception{
        if( portalProxy == null ) {
            createPortalProxy();
        }
        else    // proxy already exists
        {
            if( portalProxy.getRemainingLifetime() <= 0 )    // has proxy expired?
            {
                createPortalProxy();
            }
        }
        
        return portalProxy;
        
        
    }
    
    
    private static void createPortalProxy() throws Exception {
        X509Certificate portalCert = null;
        PrivateKey portalPrivateKey = null;
        X509Certificate caCerts[] = null;
        
        try {
            portalCert = CertUtil.loadCertificate(AuthCtl.portalCertFilename) ;
            OpenSSLKey key = new BouncyCastleOpenSSLKey(AuthCtl.portalPrivateKeyFilename);
            if (key.isEncrypted()) {
                //decrypt
                key.decrypt("password".getBytes());
            }
            portalPrivateKey = key.getPrivateKey();
            
            //BouncyCastleCertProcessingFactory factory = BouncyCastleCertProcessingFactory.getDefault();
            //GlobusCredential proxy = factory.createCredential(new X509Certificate[] {portalCert},portalPrivateKey, 512, 3600, GSIConstants.DELEGATION_FULL );
            
            // Create delegate credential
           /* GSSCredential credential2 = new GlobusGSSCredentialImpl(proxy, GSSCredential.INITIATE_AND_ACCEPT);
            
            
            portalCert = CertUtil.loadCert( AuthCtl.portalCertFilename );
            
            caCerts = new X509Certificate[] { CertUtil.loadCert( AuthCtl.caCertFilename ) };
            
            SSLeayKeyHolder sslKey = new SSLeayKeyHolder( AuthCtl.portalPrivateKeyFilename );
            
            portalPrivateKey = sslKey.getPrivateKey();*/
        }
        
        catch( Exception e ) { throw e; }
        
        
        try {
            BouncyCastleCertProcessingFactory factory = BouncyCastleCertProcessingFactory.getDefault();
            GlobusCredential proxy = factory.createCredential(new X509Certificate[] {portalCert},portalPrivateKey, 512, 3600, GSIConstants.DELEGATION_FULL );
            
            // Create delegate credential
            portalProxy = new GlobusGSSCredentialImpl(proxy, GSSCredential.INITIATE_AND_ACCEPT);
            
            // X509Certificate [] portalCertChain = new X509Certificate[] { portalCert };
            //portalProxy = new GlobusProxy( portalPrivateKey, portalCertChain, caCerts );
        }
        
        catch( Exception e ) { throw e; }
    }
    
}
