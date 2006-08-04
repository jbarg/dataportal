/*
 * UserUtilTest.java
 * JUnit based test
 *
 * Created on 04 August 2006, 14:51
 */

package uk.ac.dl.dp5.util;


import java.util.Collection;
import junit.framework.*;
import java.security.cert.CertificateException;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import javax.persistence.EntityManager;
import javax.persistence.EntityNotFoundException;
import javax.persistence.*;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp5.clients.dto.SessionDTO;
import uk.ac.dl.dp5.clients.session.SessionDelegate;
import uk.ac.dl.dp5.entity.*;
import uk.ac.dl.dp5.exceptions.*;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;

/**
 *
 * @author gjd37
 */
public class UserUtilTest extends TestCase {
    
    static EntityManagerFactory emf;
    static EntityManager em;
    static String sid;
    static SessionDelegate sd = SessionDelegate.getInstance();
    
    public UserUtilTest(String testName) {
        super(testName);
    }
    
    protected void setUp() throws Exception {
        if(emf == null){
            emf = Persistence.createEntityManagerFactory("dataportal_unit_test");
            // Create new EntityManager
            em = emf.createEntityManager();
            CachingServiceLocator.getInstance().put(em);
            
            System.out.println("logging in");
            sid = sd.login(TestConstants.MYPROXY_USERNAME,TestConstants.MYPROXY_PASSWORD,3);
        }
    }
    
    protected void closeEnitiyManager(){
        if(em != null){
            // Close this EntityManager
            System.out.println("Closing entity manager");
            em.close();
            System.out.println("logging out");
            try {
                sd.logout(sid);
            } catch (SessionTimedOutException ex) {
                ex.printStackTrace();
            } catch (SessionNotFoundException ex) {
                ex.printStackTrace();
            }
        }
    }
    
    /**
     * Test of getUser method, of class uk.ac.dl.dp5.util.UserUtil.
     */
    public void testGetUser() throws Exception {
        System.out.println("getUser");
             
        // Begin transaction
        em.getTransaction().begin();
        
        User expResult = null;
        User result = new UserUtil(sid).getUser();
        assertNotNull("User result canot be null", result);
        assertEquals("Dn should be "+TestConstants.USER_DN,TestConstants.USER_DN,result.getDn());
        
        
        try {
        
        //test not valid sid
            User result1 = new UserUtil("dfd").getUser();
        } catch (SessionNotFoundException ex) {
            assertTrue("Sid is invalid",true);
            return ;
        } catch (SessionTimedOutException ex) {
            ex.printStackTrace();
        } catch (UserNotFoundException ex) {
            ex.printStackTrace();
        }
        assertFalse("Session in valid, it should not be",true);
        
        // Commit the transaction
        em.getTransaction().commit();
        
        closeEnitiyManager();
        
    }
    
    /**
     * Test of getProxyServers method, of class uk.ac.dl.dp5.util.UserUtil.
     */
    /*public void testGetProxyServers() {
        System.out.println("getProxyServers");
        
        UserUtil instance = null;
        
        ProxyServers expResult = null;
        ProxyServers result = instance.getProxyServers();
        assertEquals(expResult, result);
        
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }
    
    /**
     * Test of getUserPrefs method, of class uk.ac.dl.dp5.util.UserUtil.
     */
    /*public void testGetUserPrefs() {
        System.out.println("getUserPrefs");
        
        UserUtil instance = null;
        
        DpUserPreference expResult = null;
        DpUserPreference result = instance.getUserPrefs();
        assertEquals(expResult, result);
        
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }
    
    /**
     * Test of createDefaultUser method, of class uk.ac.dl.dp5.util.UserUtil.
     */
    /*public void testCreateDefaultUser() throws Exception {
        System.out.println("createDefaultUser");
        
        String DN = "";
        
        User expResult = null;
        User result = UserUtil.createDefaultUser(DN);
        assertEquals(expResult, result);
        
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }
    
    /**
     * Test of sendEventLog method, of class uk.ac.dl.dp5.util.UserUtil.
     */
    /*public void testSendEventLog() {
        System.out.println("sendEventLog");
        
     // Begin transaction
        em.getTransaction().begin();
     
        DPEvent event = null;
        String description = "";
        UserUtil instance = null;
        
        instance.sendEventLog(event, description);
        
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }*/
    
}
