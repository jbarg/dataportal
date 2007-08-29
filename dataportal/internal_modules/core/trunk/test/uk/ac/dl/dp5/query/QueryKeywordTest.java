/*
 * SessionTest.java
 *
 * Created on 26 June 2007, 10:15
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.query;

import uk.ac.dl.dp5.sessionbeans.session.*;
import java.util.Date;
import junit.framework.JUnit4TestAdapter;
import org.apache.log4j.Logger;
import org.junit.Test;
import static org.junit.Assert.*;
import uk.ac.dl.dp.core.sessionbeans.lookup.LookupBean;
import uk.ac.dl.dp.core.sessionbeans.session.SessionBean;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp.coreutil.entity.Session;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import uk.ac.dl.dp.coreutil.exceptions.LoginError;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.util.DPCredentialType;
import uk.ac.dl.dp5.util.BaseTestClassTX;
import uk.ac.dl.dp5.util.sendMDBLocalTest;
import uk.ac.dl.dp5.util.SessionInsertUtil;
import uk.ac.dl.dp5.util.TestConstants;

/**
 *
 * @author gjd37
 */
public class QueryKeywordTest extends BaseTestClassTX {
    
    private static Logger log = Logger.getLogger(SessionLogonTest.class);
    
    private static SessionBean sessionBean = new SessionBean();
    private static LookupBean lookupBean = new LookupBean();
    private static sendMDBLocalTest eventLocal = new sendMDBLocalTest();
    
    
    
    @Test
    public void testLogonSession() throws DataPortalException{
        log.info("logonTest");
        
        sessionBean.setEntityManager(em);
        lookupBean.setEntityManager(em);
        sessionBean.setLookupLocal(lookupBean);
        sessionBean.setSendMDBLocal(eventLocal);
        
        String sessionId = sessionBean.login("gjd37" /*System.getProperty("usersso.username"), System.getProperty("password"),*/, "(gsd+37@Dl", 2);
        
        assertNotNull("SessionId cannot be null", sessionId);
       
    }
    
    
    
    public static junit.framework.Test suite(){
        return new JUnit4TestAdapter(SessionLogonTest.class);
    }
    
    
}
