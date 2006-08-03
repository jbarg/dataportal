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
import uk.ac.dl.dp5.clients.dto.SessionDTO;
import uk.ac.dl.dp5.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp5.entity.DpUserPreference;
import uk.ac.dl.dp5.entity.ProxyServers;
import uk.ac.dl.dp5.entity.Role;
import uk.ac.dl.dp5.entity.User;
import uk.ac.dl.dp5.exceptions.CannotCreateNewUserException;
import uk.ac.dl.dp5.exceptions.LoginError;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.sessionbeans.lookup.LookupLocal;
import uk.ac.dl.dp5.util.DPCredentialType;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.util.Certificate;
import uk.ac.dl.dp5.util.DPEvent;
import uk.ac.dl.dp5.util.DPResolution;
import uk.ac.dl.dp5.util.DataPortalConstants;
import uk.ac.dl.dp5.exceptions.LoginMyProxyException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.util.PortalCredential;
import uk.ac.dl.dp5.util.SessionUtil;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.util.UserUtil;
import uk.ac.dl.dp5.util.cog.DelegateCredential;
import uk.ac.dl.dp5.clients.session.SessionDelegate;

/**
 *
 * @author gjd37
 */
public class SessionBeanTest extends TestCase {
    
    private String sid;
    private static SessionDelegate sd = SessionDelegate.getInstance();
    private static  UserPreferencesDTO userprefs;
    
    public SessionBeanTest(String testName) {
        super(testName);
    }
    
    protected void setUp() throws Exception {
        //login
        sid = sd.login("glen","kkkkkk",3);
        System.out.println("logging in");
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
    
    
    
}
