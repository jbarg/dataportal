/*
 * SessionInsertUtil.java
 *
 * Created on 26 June 2007, 10:12
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

import java.util.Collection;
import java.util.Date;
import javax.persistence.EntityManager;
import javax.persistence.NoResultException;
import uk.ac.dl.dp.coreutil.entity.Session;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.util.DPCredentialType;
import uk.ac.dl.dp.coreutil.util.UserUtil;

/**
 *
 * @author gjd37
 */
public class SessionInsertUtil {
    
    
    public static Session getValidSession(EntityManager em) {
        User user = null;
        Session session = null;
        try {
            //select a user
            user = (User)em.createNamedQuery("User.findByUserId").setParameter("userId","gjd37").getSingleResult();
            System.out.println("found old user");
            
            Collection<Session> sessions = user.getSession();
            if(sessions == null || sessions.size() == 0){
                session = new Session();
                session.setCredential("CREDENTIAL");
                session.setCredentialType(DPCredentialType.PROXY);
                session.setExpireDateTime(new Date(new Date().getTime()+1000000)); //adds 15 mins
                session.setUserId(user);
                session.setUserSessionId(TestConstants.SESSION_ID);
                user.addSession(session);
                em.persist(session);
                
                System.out.println("Session created "+session.getUserSessionId());
            } else{
                for (Session sessionFound : sessions) {
                    sessionFound.setExpireDateTime(new Date(new Date().getTime()+1000000)); //adds 15 mins
                    sessionFound.setUserSessionId(TestConstants.SESSION_ID);
                    session = sessionFound;
                    break;
                }
            }
        } catch(NoResultException nre) {
            //no user co create one for me
            System.out.println("new user");
            try {
                user = UserUtil.createDefaultUser("gjd37", TestConstants.USER_DN, em);
                System.out.println("no session found");
                session = new Session();
                session.setCredential("CREDENTIAL");
                session.setCredentialType(DPCredentialType.PROXY);
                session.setExpireDateTime(new Date(new Date().getTime()+1000000)); //adds 15 mins
                session.setUserId(user);
                session.setUserSessionId(TestConstants.SESSION_ID);
                user.addSession(session);
                em.persist(session);
                
                System.out.println("Session created "+session.getUserSessionId());
            } catch (SessionException ex) {
                System.out.println(ex);
            }
        }
        System.out.println("Session is "+session);
        return session;
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
    }
    
}
