/*
 * DataCenterBeanTest.java
 * JUnit based test
 *
 * Created on 03 August 2006, 17:11
 */

package uk.ac.dl.dp5.sessionbeans.datacenter;

import junit.framework.*;
import java.util.Collection;
import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.Url;

/**
 *
 * @author gjd37
 */
public class DataCenterBeanTest_1 extends TestCase {
    
    private String sid;
   
    public DataCenterBeanTest_1(String testName) {
        super(testName);
    }
    
    static EntityManagerFactory emf;
    static EntityManager em;
    
    protected void setUp() throws Exception {
        if(emf == null){
            emf = Persistence.createEntityManagerFactory("dataportal_unit_test");
            // Create new EntityManager
            em = emf.createEntityManager();
        }
    }
    
    protected void closeEnitiyManager(){
        if(em != null){
            // Close this EntityManager
            System.out.println("Closing entity manager");
            em.close();
            System.out.println("logging out");
            
        }
    }
    
    
    /**
     * Test of removeBookmark method, of class uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterBean.
     */
    public void testRemoveBookmark() throws Exception {
          // Begin transaction
        em.getTransaction().begin();
        
        System.out.println("removeRef");
        DataReference dataReference  = em.find(DataReference.class ,751);
        Collection<Url> urls = dataReference.getUrls();
        //for(Url dr_url : urls){
          //   System.out.println("Removing url "+dr_url.getId());
           // em.remove(dr_url);
        //}/
        
        //remove from DB
        em.remove(dataReference);
        
        // Commit the transaction
        em.getTransaction().commit();
    }
    
    
    
    
}
