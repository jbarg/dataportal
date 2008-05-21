/*
 * SessionTest.java
 *
 * Created on 26 June 2007, 10:15
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.session;

import junit.framework.JUnit4TestAdapter;
import org.apache.log4j.Logger;
import org.junit.Test;
import static org.junit.Assert.*;
import uk.ac.dl.dp.core.sessionbeans.lookup.LookupBean;
import uk.ac.dl.dp.core.sessionbeans.session.SessionBean;
import uk.ac.dl.dp.coreutil.entity.Session;
import uk.ac.dl.dp5.util.BaseTestClassTX;
import uk.ac.dl.dp5.util.sendMDBLocalTest;
import uk.ac.dl.dp5.util.SessionInsertUtil;

/**
 *
 * @author gjd37
 */
public class SessionInsertTest extends BaseTestClassTX {
    
    private static Logger log = Logger.getLogger(SessionTest.class);
    
     
    @Test
    public void testAddSession(){
        log.info("addSession");
        Session session = SessionInsertUtil.getValidSession(em);
        em.flush();
    }
    
    @Test
    public void testAddFacilitySession(){
        log.info("testAddFacilitySession");
        SessionInsertUtil.addFacilitySession(em, 1001L);
        em.flush();
    }
    
    
    
      
    public static junit.framework.Test suite(){
        return new JUnit4TestAdapter(SessionInsertTest.class);
    }
    
    
}
