
/*
 * BaseTestClass.java
 *
 * Created on 22 February 2007, 12:18
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

import java.io.File;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;

/**
 *
 * Extend this when you want setUp and tear down to be called one in the class and not after and before every @Test method
 * But you want a new transaction to start and finish before and after evey method aswell as new entity manager
 *
 * Basically, managerfactory is loaded per class and new transactions and entitymanager with every method
 *
 * @author gjd37
 */
public class BaseTestClassTX extends BaseTest {
    
    private static Logger log = Logger.getLogger(BaseTestClassTX.class);
    
    static {
         PropertyConfigurator.configure(System.getProperty("user.home")+File.separator+".log4j.properties");          
    }
    
    
    @Before
    public void beginTX(){
        log.info("setUp(), creating entityManager");
        em = emf.createEntityManager();
        
        // Begin transaction
        log.info("beginning transaction on entityManager");
        
        em.getTransaction().begin();
    }
    
    @After
    public void closeTX(){
        
        // Commit the transaction
        log.info("commiting transaction on entityManager");
        try {
            if(!em.getTransaction().getRollbackOnly()){
                log.info("commiting tx");
                em.getTransaction().commit();
            }
        } catch(RuntimeException t){
            log.error(t);
            throw t;
        } finally {
            log.info("tearDown(), closing entityManager");
           if(em !=null) em.close();
        }
    }
    
    @BeforeClass
    public static void BeforeClassSetUp(){
        setUpEntityManagerFactoryOnly();
    }
    
    @AfterClass
    public static void AfterClassTearDown(){
        tearDownEntityManagerFactoryOnly();
    }       
}
