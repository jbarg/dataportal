/*
 * SessionBeanTest.java
 * JUnit based test
 *
 * Created on 03 August 2006, 16:36
 */

package uk.ac.dl.dp5.sessionbeans.session;

import junit.framework.*;
import java.security.cert.CertificateException;
import java.security.cert.CertificateExpiredException;
import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.UUID;
import javax.annotation.PostConstruct;
import javax.ejb.*;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.globus.myproxy.MyProxyException;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp.coreutil.entity.DpUserPreference;
import uk.ac.dl.dp.coreutil.entity.ProxyServers;
import uk.ac.dl.dp.coreutil.entity.Role;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.CannotCreateNewUserException;
import uk.ac.dl.dp.coreutil.exceptions.LoginError;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.coreutil.util.DPCredentialType;
import uk.ac.dl.dp.coreutil.entity.Session;
import uk.ac.dl.dp.coreutil.util.Certificate;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.DPResolution;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;

import uk.ac.dl.dp.coreutil.util.SessionUtil;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;

import uk.ac.dl.dp.coreutil.util.UserUtil;
import uk.ac.dl.dp.coreutil.util.cog.DelegateCredential;
import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;

/**
 *
 * @author gjd37
 */
public class SessionBeanTest extends TestCase {
    
    private String sid;
    private static SessionDelegate sd = SessionDelegate.getInstance();
    private static  UserPreferencesDTO userprefs;
    
     private long time = new Date().getTime();
    
     
    public SessionBeanTest(String testName) {
        super(testName);
    }
    
    protected void setUp() throws Exception {
        //login
        System.out.println("logging in");
        sid = sd.login(DataPortalConstants.MYPROXY_USERNAME,DataPortalConstants.MYPROXY_PASSWORD,3);
        System.out.println("logged in with sid: "+sid);
    }
    
    protected void tearDown() throws Exception {
        //logout
        System.out.println("logging out");
        sd.logout(sid);
    }
    
    /**
     * Test of getSession method, of class uk.ac.dl.dp5.sessionbeans.session.SessionBean.
     */
    public void testGetSession() throws Exception {
        System.out.println("getSession");
        
        
        
        SessionDTO expResult = null;
        SessionDTO result = sd.getSession(sid);
        
        assertNotNull("SessionDTO should not be null",result);
        assertEquals("SID should be "+sid,result.getUserSessionId(),sid);
        assertNotNull("getCredential() should not be null",result.getCredential());
        assertNotNull("getCredentialType() should not be null",result.getCredentialType());
        assertNotNull("getExpireTime() should not be null",result.getExpireTime());
        
        
    }
    
    
    /**
     * Test of isValid method, of class uk.ac.dl.dp5.sessionbeans.session.SessionBean.
     */
    public void testIsValid() throws Exception {
        System.out.println("isValid");
        
        
        Boolean expResult = true;
        Boolean result = sd.isValid(sid);
        assertEquals("sid: "+sid +" should be valid",expResult, result);
        
        
        
        
    }
    
    public void testInvalidisValid() throws Exception {
        System.out.println("testInvalidStuff");
        
        System.out.println("isValid");
        Boolean result1 = false;
        try {
            result1 = sd.isValid("dfgdf");
        } catch (SessionNotFoundException ex) {
            return ;
        } catch (SessionTimedOutException ex) {
            return ;
        }
        assertFalse("sid: dfgdf should be invalid",true);
        
    }
    
    public void testInvalidgetSession() throws Exception {
        //getSession
        System.out.println("getSession");
        SessionDTO result2 = null;
        try {
            result2 = sd.getSession("dfgdf");
        } catch (SessionNotFoundException ex) {
            return ;
        } catch (SessionTimedOutException ex) {
            return ;
        }
        assertFalse("sid: dfgdf should not get session",true);
        
    }
    
    public void testInvalidgetUserPrefs() throws Exception {
                
        //getSession
        System.out.println("getUserPrefs");
        UserPreferencesDTO result3 = null;
        try {
            result3 = sd.getUserPrefs("dfgdf");
        } catch (SessionNotFoundException ex) {
            return ;
        } catch (SessionTimedOutException ex) {
            return ;
        }
        assertFalse("sid: dfgdf should not get getUserPrefs",true);
        
        
    }
    
    
    /**
     * Test of setUserPrefs method, of class uk.ac.dl.dp5.sessionbeans.session.SessionBean.
     */
    public void testSetUserPrefs() throws Exception {
        System.out.println("setUserPrefs");
        
        userprefs = new UserPreferencesDTO();
        userprefs.setDefaultFacility("ISIS");
        userprefs.setResolution(DPResolution.res_1280x768);
        userprefs.setResultsPerPage(50);
        
        sd.setUserPrefs(sid, userprefs);
        
    }
    
    /**
     * Test of getUserPrefs method, of class uk.ac.dl.dp5.sessionbeans.session.SessionBean.
     */
    public void testGetUserPrefs() throws Exception {
        System.out.println("getUserPrefs");
        
        UserPreferencesDTO expResult = userprefs;
        UserPreferencesDTO result = sd.getUserPrefs(sid);
        assertEquals("getDefaultFacility() are not equal",expResult.getDefaultFacility(), result.getDefaultFacility());
        
        assertEquals("getResolution() are not equal",expResult.getResolution().toString(), result.getResolution().toString());
        assertEquals("getResultsPerPage() are not equal",expResult.getResultsPerPage(), result.getResultsPerPage());
        
        
    }
    
    public void testLogin() throws Exception {
        System.out.println("testLogin");
        
        int MAX = 2;
        for(int i =0 ; i < MAX ; i++){
            setUp();
            if(i !=MAX-1){
                tearDown();
            }
            try {
                Thread.sleep(10);
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
        }
        
        System.out.println("Time taken was : "+(new Date().getTime() - time)/1000+" seconds");
    }
    
}
