/*
 * TestCred.java
 *
 * Created on 21 September 2004, 11:22
 */

package uk.ac.clrc.dataportal.credentials;
import org.globus.gsi.*;
import org.ietf.jgss.*;
/**
 *
 * @author  gjd37
 */
public class TestCred extends DelegateCredential{
    
    /** Creates a new instance of TestCred */
    public TestCred() {
    }
    
    
    
     public  String urlCopy(Integer message, String sessionid, byte[] cert)
    throws Exception {
        
        GSSCredential credential2 = getCredential(sessionid,cert);
        System.out.println(credential2.getName().toString());
                    System.out.println(credential2.getRemainingLifetime());
        return "got the cred!";
     }
    
}
