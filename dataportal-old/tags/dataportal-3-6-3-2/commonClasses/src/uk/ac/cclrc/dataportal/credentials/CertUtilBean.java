/*
 * CertUtilBean.java
 *
 * Created on 12 December 2003, 10:45
 */

package uk.ac.clrc.dataportal.credentials;

import org.globus.gsi.bc.*;
import org.globus.gsi.*;
import java.security.cert.*;
import java.security.*;
import java.util.*;
/**
 *
 * @author  gjd37
 */
public class CertUtilBean {
    
    private KeyPair keypair;
    private X509Certificate issuercert;
    private long time;
    
    public CertUtilBean(){};
    
    public void setCert(X509Certificate cert) {
        issuercert = cert;
    }    
    
    public X509Certificate getCert() {
        return issuercert;
    }
    
     public void setKeyPair(KeyPair pair) {
        keypair = pair;
    }    
    
    public KeyPair getKeyPair() {
        return keypair;
    }
    
     public void setTime() {
        time = System.currentTimeMillis();
        System.out.println("set time as "+new Date(time));
    }    
    
    public long getTime() {
        return time;
    }
    
}
