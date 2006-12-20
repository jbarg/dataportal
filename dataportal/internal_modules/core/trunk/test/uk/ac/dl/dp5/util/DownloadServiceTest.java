/*
 * SessionUtilTest.java
 * JUnit based test
 *
 * Created on 04 August 2006, 14:01
 */

package uk.ac.dl.dp5.util;

import java.io.File;
import java.util.Collection;
import junit.framework.*;
import javax.persistence.EntityManager;
import javax.persistence.*;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.dl.dp.coreutil.entity.SrbServer;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;

/**
 *
 * @author gjd37
 */
public class DownloadServiceTest extends TestCase {
    
    static EntityManagerFactory emf;
    static EntityManager em;
    static String sid  = "c0accd8c-c056-4eec-a88a-a34d52c65eaf";
    
    
    public DownloadServiceTest(String testName) {
        
        super(testName);
        PropertyConfigurator.configure(System.getProperty("user.home")+File.separator+"log4j.properties");
        
    }
    
    protected void setUp() throws Exception {
        if(emf == null){
            emf = Persistence.createEntityManagerFactory("dataportal_unit_test");
            // Create new EntityManager
            em = emf.createEntityManager();
            CachingServiceLocator.getInstance().put(em);
            
        }
    }
    
    protected void closeEnitiyManager(){
        if(em != null){
            // Close this EntityManager
            System.out.println("Closing entity manager");
            em.close();
            
        }
    }
    
    protected void tearDown() throws Exception {
        
        
    }
    
    
   /* public void testDownloadService() throws Exception{
    
    
        //If a transaction required, it is started:
    
        // Begin transaction
        em.getTransaction().begin();
    
        //do stuff here
        try {
    
            SRBInfo info = new SRBInfo(new String[]{"srb://eminerals.dl.ac.uk:5544/home/louise_price.eminerals/urazole/KOXRIY_sym"});
            info.setDefaultUser("gjd37");
            info.setDefaultDomain("eminerals");
            info.setDefaultPassword("blackjack");
    
            DownloadService ds = new DownloadService();
    
    
            ds.setEntityManager(em);
            ds.downloadSRBFiles(sid,info);
    
            System.out.println("done: ");
        }  catch (Exception ex ) {
            System.out.println("Error with downlaod: ");
            ex.printStackTrace();
        }
    
        // Commit the transaction
        em.getTransaction().commit();
    
        //And EntityManager should always be closed if it won't be used again:
        closeEnitiyManager();
    
    }*/
    
    public void testDownloadService() throws Exception{
        
        
        //If a transaction required, it is started:
        
        // Begin transaction
        em.getTransaction().begin();
        
        Collection<SrbServer> srbServers = (Collection<SrbServer>)em.createNamedQuery("SrbServer.findAll").getResultList();
        for(SrbServer srbServer : srbServers){
            
            
            if(srbServer.isDefaultSet()){
                System.out.println("Default SRB user for  "+srbServer.getDefaultUser());
                
            } else {
                System.out.println("SRB Server info not set: ");
                System.out.println(srbServer.getDefaultDomain());
                System.out.println(srbServer.getDefaultPassword());
                System.out.println(srbServer.getDefaultUser());
            }
            
        }
        
        // Commit the transaction
        em.getTransaction().commit();
        
        //And EntityManager should always be closed if it won't be used again:
        closeEnitiyManager();
        
    }
}
