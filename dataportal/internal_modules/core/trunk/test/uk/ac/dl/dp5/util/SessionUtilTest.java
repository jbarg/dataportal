/*
 * SessionUtilTest.java
 * JUnit based test
 *
 * Created on 04 August 2006, 14:01
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
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;

/**
 *
 * @author gjd37
 */
public class SessionUtilTest extends TestCase {
    
    static EntityManagerFactory emf;
    static EntityManager em;
    static String sid;
    static SessionDelegate sd = SessionDelegate.getInstance();
   
    public SessionUtilTest(String testName) {
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
    
    protected void tearDown() throws Exception {
        
        
    }
    
    
    public void testEnitityManagerConnection() throws Exception{
        
        
        //If a transaction required, it is started:
        
        // Begin transaction
        em.getTransaction().begin();
        /*Collection<Session> sessions = em.createQuery("SELECT s FROM Session s").getResultList();
         
        for(Session ses : sessions){
            System.out.println(ses.getUserSessionId());
        }*/
        
        Session session = new SessionUtil(sid).getSession();
        System.out.println(session.getExpireDateTime());
        
        // Commit the transaction
        em.getTransaction().commit();
        
        //And EntityManager should always be closed if it won't be used again:
        
        
    }
    /**
     * Test of isValid method, of class uk.ac.dl.dp5.util.SessionUtil.
     */
    public void testIsValid() throws Exception {
        System.out.println("isValid");
        
        // Begin transaction
        em.getTransaction().begin();
        
        boolean result = new SessionUtil(sid).isValid();
        assertEquals("Session should be vaild",true, result);
        
        //test bad SID only once
        boolean result2 = false;
        try {
            result2 = new SessionUtil("fdffd").isValid();
            assertEquals("Session fdffd should be invaild",false, result2);
        } catch (SessionTimedOutException ex) {
            ex.printStackTrace();
        } catch (SessionNotFoundException ex) {
            assertTrue("Session fdfdf is invalis and not found",true);
        }
        
        // Commit the transaction
        em.getTransaction().commit();
        
    }
    
    /**
     * Test of getCredential method, of class uk.ac.dl.dp5.util.SessionUtil.
     */
    public void testGetCredential() throws Exception {
        System.out.println("getCredential");
        
        // Begin transaction
        em.getTransaction().begin();
        
        GSSCredential result = new SessionUtil(sid).getCredential();
        assertNotNull("GSSCredential cannot be null",result);
        
        assertEquals("DN should be /C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater","/C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater", result.getName().toString());
        
        // Commit the transaction
        em.getTransaction().commit();
        
    }
    
    /**
     * Test of getCertificate method, of class uk.ac.dl.dp5.util.SessionUtil.
     */
    public void testGetCertificate() throws Exception {
        System.out.println("getCertificate");
        
        // Begin transaction
        em.getTransaction().begin();
        
        Certificate result = new SessionUtil(sid).getCertificate();
        assertNotNull("Result cannot be null", result);
        assertEquals("DN should be /C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater","/C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater", result.getDn());
        assertNotSame("Lifetime must be positive", 0, result.isLifetimeLeft());
        
        // Commit the transaction
        em.getTransaction().commit();
    }
    
    /**
     * Test of getSession method, of class uk.ac.dl.dp5.util.SessionUtil.
     */
    public void testGetSession() throws Exception {
        System.out.println("getSession");
        
        // Begin transaction
        em.getTransaction().begin();
        
        Session session = new SessionUtil(sid).getSession();
        assertNotNull("Session canot be null",session);
        assertTrue("Expire date must be in future",session.getExpireDateTime().after(new Date()));
        assertEquals("Sids must be equal",sid, session.getUserSessionId());
        
        // Commit the transaction
        em.getTransaction().commit();        
        
    }
    
    /**
     * Test of getSessionDTO method, of class uk.ac.dl.dp5.util.SessionUtil.
     */
    public void testGetSessionDTO() throws Exception{
        System.out.println("getSessionDTO");
        
        // Begin transaction
        em.getTransaction().begin();
        
        SessionDTO session = new SessionUtil(sid).getSessionDTO();
        assertNotNull("SessionDTO canot be null",session);
        assertTrue("Expire date must be in future",session.getExpireTime().after(new Date()));
        assertEquals("Sids must be equal",sid, session.getUserSessionId());
        
        // Commit the transaction
        em.getTransaction().commit();
        
        closeEnitiyManager();
    }
    
}
