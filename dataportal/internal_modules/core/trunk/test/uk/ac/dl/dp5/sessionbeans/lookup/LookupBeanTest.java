/*
 * LookupBeanTest.java
 * JUnit based test
 *
 * Created on 04 August 2006, 11:54
 */

package uk.ac.dl.dp5.sessionbeans.lookup;

import junit.framework.*;
import java.util.ArrayList;
import java.util.Collection;
import javax.annotation.security.PermitAll;
import javax.annotation.security.RolesAllowed;
import javax.annotation.security.RunAs;
import org.apache.log4j.Logger;
import uk.ac.dl.dp5.clients.dto.FacilityDTO;
import uk.ac.dl.dp5.clients.lookup.LookupDelegate;
import uk.ac.dl.dp5.clients.session.SessionDelegate;
import uk.ac.dl.dp5.entity.ModuleLookup;
import uk.ac.dl.dp5.entity.ProxyServers;
import uk.ac.dl.dp5.entity.SrbServer;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.util.DPFacilityType;
import javax.ejb.Stateless;
import uk.ac.dl.dp5.util.*;

/**
 *
 * @author gjd37
 */
public class LookupBeanTest extends TestCase {
    
    private String sid;
    private static SessionDelegate sd = SessionDelegate.getInstance();
    private static LookupDelegate loi = LookupDelegate.getInstance();
    
    public LookupBeanTest(String testName) {
        super(testName);
    }
    
    protected void setUp() throws Exception {
        //login
        sid = sd.login(TestConstants.MYPROXY_USERNAME,TestConstants.MYPROXY_PASSWORD,3);
        System.out.println("logging in");
    }
    
    protected void tearDown() throws Exception {
        //logout
        System.out.println("logging out");
        sd.logout(sid);
    }
    
    /**
     * Test of getFacilities method, of class uk.ac.dl.dp5.sessionbeans.lookup.LookupBean.
     */
    public void testGetFacilities() {
        System.out.println("getFacilities");
        
        DPFacilityType type = null;
        LookupDelegate loi =  LookupDelegate.getInstance();
        
        Collection<FacilityDTO> expResult = null;
        Collection<FacilityDTO> result = loi.lookup(DPFacilityType.WRAPPER);
        assertNotNull("List of facilities cannot be null",result);
        assertNotSame("result size canot be null",result.size(), 0);
        for(FacilityDTO fac : result){
            assertNotNull("getInfoUrl() cannot be null",fac.getFacility());
            assertNotNull("getModuleType() cannot be null",fac.getModuleType());
            assertNotNull("getPlatform() cannot be null",fac.getPlatform());
        }
    }
    
    /**
     * Test of getFacilityInfo method, of class uk.ac.dl.dp5.sessionbeans.lookup.LookupBean.
     */
    /*public void testGetFacilityInfo() {
        System.out.println("getFacilityInfo");
     
        DPFacilityType type = null;
        LookupBean instance = new LookupBean();
     
        Collection<ModuleLookup> expResult = null;
        Collection<ModuleLookup> result = instance.getFacilityInfo(type);
        assertEquals(expResult, result);
     
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }*/
    
    /**
     * Test of getProxyServers method, of class uk.ac.dl.dp5.sessionbeans.lookup.LookupBean.
     */
    public void testGetDefaultProxyServer() {
        System.out.println("getProxyServers");
        
        
        ProxyServers result = loi.getDefaultProxyServer();
        assertNotNull("Proxt Server cannot be null",result);
        assertEquals("ProxyServer name should be "+TestConstants.MYPROXY_ADDRESS,result.getProxyServerAddress(),TestConstants.MYPROXY_ADDRESS);
        assertEquals("ProxyServer port should be "+TestConstants.MYPROXY_PORT,result.getPortNumber().intValue(),TestConstants.MYPROXY_PORT);
        
        
    }
    
    /**
     * Test of getDefaultProxyServer method, of class uk.ac.dl.dp5.sessionbeans.lookup.LookupBean.
     */
    /*public void testGetProxyServers() {
        System.out.println("getDefaultProxyServer");
        
        LookupBean instance = new LookupBean();
        
        ProxyServers expResult = null;
        ProxyServers result = instance.getDefaultProxyServer();
        assertEquals(expResult, result);
        
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }*/
    
    /**
     * Test of getSRBServer method, of class uk.ac.dl.dp5.sessionbeans.lookup.LookupBean.
     */
    /*public void testGetSRBServer() {
        System.out.println("getSRBServer");
        
        LookupBean instance = new LookupBean();
        
        SrbServer expResult = null;
        SrbServer result = instance.getSRBServer();
        assertEquals(expResult, result);
        
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }*/
    
    
    
}
