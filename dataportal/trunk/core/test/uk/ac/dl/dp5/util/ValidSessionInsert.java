/*
 * ValidSessionInsert.java
 *
 * Created on 25 June 2007, 14:48
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

import java.util.Collection;
import java.util.Date;
import javax.persistence.NoResultException;
import junit.framework.JUnit4TestAdapter;
import org.apache.log4j.Logger;
import org.junit.Test;
import uk.ac.dl.dp.coreutil.entity.Session;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.util.DPCredentialType;
import uk.ac.dl.dp.coreutil.util.UserUtil;

/**
 *
 * @author gjd37
 */
public class ValidSessionInsert  extends BaseTestClassTX {
    
    private static Logger log = Logger.getLogger(ValidSessionInsert.class);
    
    @Test
    public void putValidSession1(){
        log.error("Hello");
        
    }
    
    @Test
    public void putValidSession(){
       Session session  = SessionInsertUtil.getValidSession(em);
       
    }
    
    public static junit.framework.Test suite(){
        return new JUnit4TestAdapter(ValidSessionInsert.class);
    }
    
    
}
