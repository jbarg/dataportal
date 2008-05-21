/*
 * CertificateTest.java
 * JUnit based test
 *
 * Created on 04 August 2006, 17:00
 */

package uk.ac.dl.dp5.util;

import java.io.File;

import org.apache.log4j.*;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import junit.framework.*;
import org.globus.gsi.*;
import org.globus.gsi.gssapi.*;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp.coreutil.util.Certificate;

/**
 *
 * @author gjd37
 */
public class CertificateTest extends TestCase {
    
    static Logger log = Logger.getLogger(CertificateTest.class);
    
    public CertificateTest(String testName) {
        super(testName);
        PropertyConfigurator.configure(System.getProperty("user.home")+File.separator+"log4j.properties");
        
    }
    
    protected void setUp() throws Exception {
    }
    
    protected void tearDown() throws Exception {
    }
    
    /**
     * Test of getLifetime method, of class uk.ac.dl.dp5.util.Certificate.
     */
    public void testGetLifetime() throws Exception {
        System.out.println("getLifetime");
        
        Certificate instance = new Certificate(new File("/tmp"+File.separator+"x509up_u13386"));
        GSSCredential gss = instance.getCredential();
        
        new Certificate(gss);
        
        assertEquals("Dns should be the same",TestConstants.USER_DN, instance.getDn());
        
        
    }
    
    /**
     * Test of isLifetimeLeft method, of class uk.ac.dl.dp5.util.Certificate.
     */
    /*public void testIsLifetimeLeft() throws Exception {
        System.out.println("isLifetimeLeft");
     
        Certificate instance = null;
     
        boolean expResult = true;
        boolean result = instance.isLifetimeLeft();
        assertEquals(expResult, result);
     
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }
     
    /**
     * Test of getDName method, of class uk.ac.dl.dp5.util.Certificate.
     */
    /*public void testGetDName() throws Exception {
        System.out.println("getDName");
     
        Certificate instance = null;
     
        String expResult = "";
        String result = instance.getDName();
        assertEquals(expResult, result);
     
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }
     
    /**
     * Test of getStringRepresentation method, of class uk.ac.dl.dp5.util.Certificate.
     */
    /*public void testGetStringRepresentation() {
        System.out.println("getStringRepresentation");
     
        Certificate instance = null;
     
        String expResult = "";
        String result = instance.getStringRepresentation();
        assertEquals(expResult, result);
     
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }
     
    /**
     * Test of toString method, of class uk.ac.dl.dp5.util.Certificate.
     */
   /* public void testToString() {
        System.out.println("toString");
    
        Certificate instance = null;
    
        String expResult = "";
        String result = instance.toString();
        assertEquals(expResult, result);
    
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }
    
    /**
    * Test of getCredential method, of class uk.ac.dl.dp5.util.Certificate.
    */
   /* public void testGetCredential() {
        System.out.println("getCredential");
    
        Certificate instance = null;
    
        GSSCredential expResult = null;
        GSSCredential result = instance.getCredential();
        assertEquals(expResult, result);
    
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }
    
    /**
    * Test of main method, of class uk.ac.dl.dp5.util.Certificate.
    */
   /* public void testMain() throws Exception {
        System.out.println("main");
    
        String[] args = null;
    
        Certificate.main(args);
    
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }*/
    
}
