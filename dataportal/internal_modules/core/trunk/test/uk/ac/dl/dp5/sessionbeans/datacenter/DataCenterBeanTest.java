/*
 * DataCenterBeanTest.java
 * JUnit based test
 *
 * Created on 03 August 2006, 17:11
 */

package uk.ac.dl.dp5.sessionbeans.datacenter;

import junit.framework.*;
import java.util.Collection;
import uk.ac.dl.dp5.clients.datacenter.DataCenterDelegate;
import uk.ac.dl.dp5.clients.session.SessionDelegate;
import uk.ac.dl.dp5.entity.Bookmark;
import uk.ac.dl.dp5.entity.DataReference;
import uk.ac.dl.dp5.util.Certificate;
import uk.ac.dl.dp5.util.TestConstants;

/**
 *
 * @author gjd37
 */
public class DataCenterBeanTest extends TestCase {
    
    private String sid;
    private static SessionDelegate sd = SessionDelegate.getInstance();
    private static DataCenterDelegate dcd = DataCenterDelegate.getInstance();
    private static  Bookmark dto;
    private static  Bookmark retrieved;
    
    public DataCenterBeanTest(String testName) {
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
     * Test of addBookmark method, of class uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterBean.
     */
    public void testAddBookmark() throws Exception {
        System.out.println("addBookmark");
        
        
        dto = new Bookmark();
        dto.setFacility("ISIS");
        //make nmae unique to look for in search
        dto.setName("Unit Test "+Math.random());
        dto.setNote("unit test note");
        dto.setQuery("unit test query!!!!");
        dto.setStudyId(1);
        
        System.out.println("adding bookmark");
        
        dcd.addBookmark(sid, dto);
        
        
    }
    
    /**
     * Test of getBookmarks method, of class uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterBean.
     */
    public void testGetBookmarks() throws Exception {
        System.out.println("getBookmarks");
        
        
        
        //Collection<Bookmark> expResult = dto;
        Collection<Bookmark> result = dcd.getBookmarks(sid);
        
        assertNotNull("Bookmarks cannot be null",result);
        
        Bookmark expResult = null;
        Bookmark result1 = null;
        
        if(result.size() == 1){
            System.out.println("One bookmark returned");
            
            result1 = result.iterator().next();
            retrieved = result1;
            System.out.println("setting iterator;");
            
        } else {
            System.out.println("More than one bookmark returned");
            boolean found = false;
            for(Bookmark bk : result){
                if(bk.getName().equals(dto.getName())) {
                    found = true;
                    System.out.println("Found bookmark with name: "+dto.getName());
                    result1 = bk;
                    retrieved = bk;
                }
            }
            if(!found) assertFalse("Unable to find bookmarks Name: "+dto.getName(), true);
        }
        System.out.println("doing assertions");
        assertEquals("getFacility() cannot be different",dto.getFacility(),result1.getFacility());
        assertEquals("getName() cannot be different",dto.getName(),result1.getName());
        assertEquals("getQuery() cannot be different",dto.getQuery(),result1.getQuery());
        
        //check DNs
        System.out.println("Checking Dns");
        Certificate cert = new Certificate(sd.getSession(sid).getCredential());
        assertEquals("getQuery() cannot be different",cert.getDn(),result1.getUserId().getDn());
    }
    
    /**
     * Test of removeBookmark method, of class uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterBean.
     */
    public void testRemoveBookmark() throws Exception {
        System.out.println("removeBookmark");
        if(retrieved == null){
            System.out.println("null bookmark");
            assertFalse("Bookmark retrieved is null", true);
        }
        System.out.println(retrieved.getUserId().getDn());
        dcd.removeBookmark(sid, retrieved);
        
        //now check if bookmark is still in DB
        Collection<Bookmark> result = dcd.getBookmarks(sid);
        boolean found  = false;
        for(Bookmark bk : result){
            if(bk.getName().equals(dto.getName())) {
                found = true;
                System.out.println("Found bookmark with name: "+dto.getName());
                
            }
        }
        if(found) assertFalse("Found bookmark with name: "+dto.getName()+", should have been removed", true);
    }
    
    
    /**
     * Test of addDataReference method, of class uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterBean.
     */
   /* public void testAddDataReference() throws Exception {
        System.out.println("addDataReference");
    
        String sid = "";
        DataReference dto = null;
        DataCenterBean instance = new DataCenterBean();
    
        instance.addDataReference(sid, dto);
    
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }*/
    
    /**
     * Test of removeDataReference method, of class uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterBean.
     */
    /*public void testRemoveDataReference() throws Exception {
        System.out.println("removeDataReference");
     
        String sid = "";
        Collection<DataReference> dtos = null;
        DataCenterBean instance = new DataCenterBean();
     
        instance.removeDataReference(sid, dtos);
     
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }*/
    
    /**
     * Test of getDataReferences method, of class uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterBean.
     */
    /*public void testGetDataReferences() throws Exception {
        System.out.println("getDataReferences");
     
        String sid = "";
        DataCenterBean instance = new DataCenterBean();
     
        Collection<DataReference> expResult = null;
        Collection<DataReference> result = instance.getDataReferences(sid);
        assertEquals(expResult, result);
     
        // TODO review the generated test code and remove the default call to fail.
        fail("The test case is a prototype.");
    }*/
    
}
