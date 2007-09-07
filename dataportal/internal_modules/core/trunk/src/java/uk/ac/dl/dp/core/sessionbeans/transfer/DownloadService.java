/*
 * DownloadService.java
 *
 * Created on 11 December 2006, 10:30
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans.transfer;


import java.util.Collection;
import java.util.Date;
import javax.annotation.Resource;
import javax.ejb.Stateless;
import javax.interceptor.Interceptors;
import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.ObjectMessage;
import javax.jms.Queue;
import javax.jms.QueueConnection;
import javax.jms.QueueConnectionFactory;
import javax.jms.QueueSession;
import javax.jms.Session;
import javax.persistence.NoResultException;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.core.sessionbeans.ArgumentValidator;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.entity.DPConstants;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.DownloadServiceLocal;
import uk.ac.dl.dp.coreutil.interfaces.DownloadServiceRemote;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.SRBInfo;
import uk.ac.dl.dp.coreutil.util.ServiceInfo;
import uk.ac.dl.dp.coreutil.util.UserUtil;

/**
 *
 * @author gjd37
 */
//@Interceptors(ArgumentValidator.class)
@Stateless(mappedName=DataPortalConstants.DOWNLOAD)
public class DownloadService extends SessionEJBObject  implements DownloadServiceLocal, DownloadServiceRemote {
    
    static final Logger log = Logger.getLogger(DownloadService.class);
    
    @Resource(mappedName=DataPortalConstants.CONNECTION_FACTORY)
    private QueueConnectionFactory connectionFactory;
    
    @Resource(mappedName=DataPortalConstants.DOWNLOAD_MDB)
    private Queue queue;
    
    public void downloadSRBFiles(String sid, SRBInfo srbInfo)  {
        
        QueueConnection connection = null;
        QueueSession session = null;
        MessageProducer messageProducer = null;
        User user = null;
        
        try {
            connection = connectionFactory.createQueueConnection();
            session = connection.createQueueSession(false, Session.AUTO_ACKNOWLEDGE);
            messageProducer = session.createProducer(queue);
            
        } catch (Exception e) {
            log.error("Unable to locate: "+DataPortalConstants.DOWNLOAD_MDB,e);
            try {
                //close connections
                if(session != null) session.close();
                if(connection != null)   connection.close();
            } catch (JMSException ex) {}
            return ;
        }
        try {
            //this should be done on the WSDL/Schema
            checkArguments(srbInfo);
            
            ObjectMessage message = session.createObjectMessage();
            
            user =  new UserUtil(sid,em).getUser();
            
            message.setObject(srbInfo);
            messageProducer.send(message);
            log.debug("Sending: srbDownload message for user "+user.getDn());
            
        } catch (JMSException ex) {
            log.error("Unable send event: srbDownload message for user: "+user.getDn(),ex);
        } catch (UserNotFoundException ex) {
            log.error("No user with user sid "+sid+" in the system",ex);
            return ;
        } catch (SessionException ex) {
            log.error("No user with user sid "+sid+" in the system",ex);
            return ;
        }  finally{
            try {
                if(session != null) session.close();
                if(connection != null)   connection.close();
            } catch (JMSException ex) {
                
            }
        }
    }
    
    public boolean addConstant(DPConstants constant){
        log.debug("Adding Constant");
        
        DPConstants dpcon = null;
        try{
            dpcon = (DPConstants) em.createNamedQuery("DpConstants.findByName").setParameter("name", constant.getName()).getSingleResult();
        }catch(NoResultException nre){}
        
        if(dpcon == null){
            log.info("Adding new constant: "+constant.getName()+" = "+constant.getValue());
            constant.setId(null);
            em.persist(constant);
        } else {
            if(!dpcon.getValue().equals(constant.getValue())){
                dpcon.setValue(constant.getValue());
                log.debug("Setting new value for "+dpcon.getName()+" = "+dpcon.getValue());
            } else log.trace("Same value for "+dpcon.getName()+", nothing been changed");
        }
        
        return true;
    }
    
    public boolean setConstants(Collection<DPConstants> constants){
        log.debug("Setting Constants");
        try {
            for (DPConstants dPConstants : constants) {
                addConstant(dPConstants);
            }
            return true;
        } catch(Exception e){
            log.error("Unable to set constants",e);
            return false;
        }
    }
    
    public boolean isSet(){
        try {
            DPConstants port = (DPConstants) em.createNamedQuery("DpConstants.findByName").setParameter("name", DPConstants.TYPE.SERVER_PORT.toString()).getSingleResult();
            DPConstants ip = (DPConstants) em.createNamedQuery("DpConstants.findByName").setParameter("name", DPConstants.TYPE.SERVER_IP.toString()).getSingleResult();
            DPConstants download = (DPConstants) em.createNamedQuery("DpConstants.findByName").setParameter("name", DPConstants.TYPE.DOWNLOAD_LOCATION.toString()).getSingleResult();
            DPConstants context =  (DPConstants) em.createNamedQuery("DpConstants.findByName").setParameter("name", DPConstants.TYPE.SERVER_CONTEXT_ROOT.toString()).getSingleResult();
            DPConstants scheme =  (DPConstants) em.createNamedQuery("DpConstants.findByName").setParameter("name", DPConstants.TYPE.SERVER_SCHEME.toString()).getSingleResult();
            
            if(port != null && ip != null && download != null && context != null && scheme != null) return true;
            else return false;
        } catch(NoResultException nre){
            return false;
        } catch(Exception e){
            log.error("Unable to check if constants set",e);
            return false;
        }
    }
    
    /**
     *  This checks if the arguements are valid, should be done on the WSDL/Schema
     */
    private void checkArguments(SRBInfo srbInfo, ServiceInfo serviceInfo) throws SessionException {
        checkArguments(srbInfo);
        if(serviceInfo.getMyProxyUsername() == null) throw new SessionException("MyProxy username cannot be null");
        if(serviceInfo.getMyProxyPassword() == null) throw new SessionException("MyProxy password cannot be null");
        if(serviceInfo.getEmail() == null) throw new SessionException("Email address cannot be null");
    }
    
    /**
     *  This checks if the arguements are valid, should be done on the WSDL/Schema
     */
    private void checkArguments(SRBInfo srbInfo) throws SessionException {
        
        if(srbInfo.getSrbFiles() == null && srbInfo.getSrbUrls() == null) throw new SessionException("SRBFiles and srbUrls cannot be null");
    }
}


