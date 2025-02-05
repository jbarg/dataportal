/*
 * SessionUtil.java
 *
 * Created on 20 June 2006, 08:01
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.security.cert.CertificateException;
import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.GregorianCalendar;
import javax.persistence.EntityManager;
import javax.persistence.EntityNotFoundException;
import javax.persistence.PersistenceContext;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.entity.FacilitySession;
import uk.ac.dl.dp.coreutil.entity.Session;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;

/**
 *
 * @author gjd37
 */
public class SessionUtil {
    
    private Session session;
    
    //@PersistenceContext(unitName="dataportal")
    protected EntityManager em;
    
    /** Creates a new instance of SessionUtil */
    public SessionUtil(String sid, EntityManager em) throws SessionNotFoundException , SessionTimedOutException{
        this.em = em;
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        try {
            session  = (Session) em.createNamedQuery("Session.findByUserSessionId").setParameter("userSessionId",sid).getSingleResult();
            if(!isValid()) throw new SessionTimedOutException(sid);
            
        } catch(EntityNotFoundException enfe){
            throw new SessionNotFoundException("No session found for sid: "+sid);
        } catch(javax.persistence.NoResultException nre){
            throw new SessionNotFoundException("No session found for sid: "+sid);
        }
    }
    
    public String getFacilitySessionId(String facility) throws SessionException{
        Collection<FacilitySession> facIds = session.getFacilitySessionCollection();
        for (FacilitySession facilitySession : facIds) {
            if(facilitySession.getFacilityName().equals(facility)) {
                if(facilitySession.getFacilitySessionId() != null) return facilitySession.getFacilitySessionId();
                else throw new SessionException("Facility: "+facility+" has no sid, error: "+facilitySession.getFacilitySessionError());
            }
        }
        //if get here then error
        throw new SessionException("Facility: "+facility+" has not been logged on.");
    }
    
    public boolean isFacilityLoggedIn(int numberActiveFacilities){
        Collection<FacilitySession> facIds = session.getFacilitySessionCollection();
        if(facIds.size() >=  numberActiveFacilities) return true;
        else return false;        
    }
    
    public boolean isValid() throws SessionTimedOutException{
        /*Certificate cred = new Certificate(session.getCredential());
        return cred.isLifetimeLeft();*/
        
        //use table
        Date date = this.session.getExpireDateTime();
        GregorianCalendar expire = new GregorianCalendar();
        expire.setTime(date);
        
        Calendar now = GregorianCalendar.getInstance();
        
        if(now.after(expire)){
            //timed out
            return false;
        } else return true;
    }
    
    public GSSCredential getCredential() throws CertificateException{
        Certificate cred = new Certificate(session.getCredential());
        return cred.getCredential();
    }
    
    public Certificate getCertificate() throws CertificateException{
        return new Certificate(session.getCredential());
    }
    
    public Session getSession(){
        return session;
    }
    
    public SessionDTO getSessionDTO(){
        return new SessionDTO(session);
    }
}
