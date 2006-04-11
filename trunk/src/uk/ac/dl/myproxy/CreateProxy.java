/*
 * CreateProxy.java
 *
 * Created on 19 December 2003, 12:12
 */
package uk.ac.dl.myproxy;

import java.io.*;
import java.net.*;
import org.globus.gsi.*;
import org.globus.gsi.gssapi.*;
import org.globus.io.urlcopy.*;
import org.globus.util.*;
import org.ietf.jgss.*;
import org.apache.log4j.*;
import java.util.HashMap;
import org.globus.gsi.bc.*;
import org.globus.gsi.*;
import java.security.cert.*;
import java.security.*;

/**
 *
 * @author  gjd37
 */
public class CreateProxy {

    /** Creates a new instance of CreateProxy */
    public CreateProxy() throws Exception{
        X509Certificate X509DelCert = CertUtil.loadCertificate(System.getProperty("user.home")+"/.globus/usercert.pem");

        OpenSSLKey key = new BouncyCastleOpenSSLKey( System.getProperty("user.home")+"/.globus/userkey.pem" );
        key.decrypt( "" );
        PrivateKey issuerKey = key.getPrivateKey();
        // Create delegate certificate
        BouncyCastleCertProcessingFactory certFactory = BouncyCastleCertProcessingFactory.getDefault();

        GlobusCredential delegateCred  = certFactory.createCredential( new X509Certificate[] { X509DelCert},        issuerKey,    512,    3600,        GSIConstants.DELEGATION_FULL );
         GSSCredential cred = new GlobusGSSCredentialImpl(delegateCred, GSSCredential.INITIATE_AND_ACCEPT);
                System.out.println(cred.getName());
        System.out.println(cred.getRemainingLifetime());
              System.out.println(delegateCred.getSubject());
        System.out.println(delegateCred.getTimeLeft());
    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws Exception{
        new CreateProxy();
    }

}
