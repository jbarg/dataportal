/*
 * DownloadService.java
 *
 * Created on 11 December 2006, 10:30
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans.transfer;


import java.util.Date;
import javax.annotation.Resource;
import javax.ejb.Stateless;
import javax.jms.JMSException;
import javax.jms.MessageProducer;
import javax.jms.ObjectMessage;
import javax.jms.Queue;
import javax.jms.QueueConnection;
import javax.jms.QueueConnectionFactory;
import javax.jms.QueueSession;
import javax.jms.Session;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.entity.DPConstants;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.DownloadServiceRemote;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.SRBInfo;
import uk.ac.dl.dp.coreutil.util.ServiceInfo;
import uk.ac.dl.dp.coreutil.util.UserUtil;

/**
 *
 * @author gjd37
 */

@Stateless(mappedName=DataPortalConstants.DOWNLOAD)
public class DownloadService extends SessionEJBObject  implements DownloadServiceRemote {
    
    static final Logger log = Logger.getLogger(DownloadService.class);
    
    @Resource(mappedName=DataPortalConstants.CONNECTION_FACTORY)
    private QueueConnectionFactory connectionFactory;
    
    @Resource(mappedName=DataPortalConstants.DOWNLOAD_MDB)
    private Queue queue;
    
    public void downloadSRBFiles(String sid, SRBInfo srbInfo)  {
        
        //this should be done on the WSDL/Schema
        checkArguments(srbInfo);
        
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
        } catch (SessionNotFoundException ex) {
            log.error("No user with user sid "+sid+" in the system",ex);
            return ;
        }  catch (SessionTimedOutException ex) {
            log.error("No user with user sid "+sid+" in the system",ex);
            return ;
        }finally{
            try {
                if(session != null) session.close();
                if(connection != null)   connection.close();
            } catch (JMSException ex) {
                
            }
        }
        
    }
    
    public boolean setConstants(DPConstants constants){
        log.debug("Setting Constants");
        try {
            
            if(em.find(DPConstants.class,1L) != null){
                log.trace("Already set");
                return true;
            } else {
                log.trace("Setting ..");
                constants.setId(1L);
                constants.setModTime(new Date());
                em.merge(constants);
                log.trace("Set constants");
                return true;
            }
        } catch(Exception e){
            log.error("Unable to set constants",e);
            return false;
        }
    }
    
    public boolean isSet(){
        try {
            DPConstants constants=   em.find(DPConstants.class,1L);
            if(constants == null || constants.getDownloadLocation() == null || constants.getDownloadLocation().equals("")) return false;
            else return true;
        } catch(Exception e){
            log.error("Unable to check if constants set",e);
            return false;
        }
    }
    
    /**
     *  This checks if the arguements are valid, should be done on the WSDL/Schema
     */
    private void checkArguments(SRBInfo srbInfo, ServiceInfo serviceInfo) throws IllegalArgumentException {
        checkArguments(srbInfo);
        if(serviceInfo.getMyProxyUsername() == null) throw new IllegalArgumentException("MyProxy username cannot be null");
        if(serviceInfo.getMyProxyPassword() == null) throw new IllegalArgumentException("MyProxy password cannot be null");
        if(serviceInfo.getEmail() == null) throw new IllegalArgumentException("Email address cannot be null");
        
    }
    
    /**
     *  This checks if the arguements are valid, should be done on the WSDL/Schema
     */
    private void checkArguments(SRBInfo srbInfo) throws IllegalArgumentException {
        
        if(srbInfo.getSrbFiles() == null && srbInfo.getSrbUrls() == null) throw new IllegalArgumentException("SRBFiles and srbUrls cannot be null");
    }
}


