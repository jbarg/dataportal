/*
 * EventMessageBean.java
 *
 * Created on 22 June 2006, 13:11
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.message.query;

import edu.sdsc.grid.io.srb.SRBException;
import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Collection;
import javax.annotation.Resource;
import javax.ejb.ActivationConfigProperty;
import javax.ejb.EJB;
import javax.ejb.EJBContext;
import javax.ejb.MessageDriven;
import javax.ejb.TransactionAttribute;
import javax.ejb.TransactionAttributeType;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageListener;
import javax.jms.ObjectMessage;
import javax.mail.MessagingException;
import javax.mail.Session;
import javax.mail.Transport;
import javax.mail.internet.InternetAddress;
import javax.mail.internet.MimeMessage;
import javax.persistence.NoResultException;
import org.apache.log4j.Logger;
import org.globus.myproxy.MyProxyException;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.core.message.MessageEJBObject;
import uk.ac.dl.dp.coreutil.entity.DPConstants;
import uk.ac.dl.dp.coreutil.entity.SrbServer;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;
import uk.ac.dl.dp.coreutil.interfaces.EventLocal;
import uk.ac.dl.dp.coreutil.util.Certificate;
import uk.ac.dl.dp.coreutil.util.SRBInfo;
import uk.ac.dl.dp.coreutil.util.ServiceInfo;
import uk.ac.dl.dp.coreutil.util.SessionUtil;
import uk.ac.dl.dp.coreutil.util.cog.DelegateCredential;
import uk.ac.dl.srbapi.cog.PortalCredential;
import uk.ac.dl.srbapi.srb.SRBFileManagerThread;
import uk.ac.dl.srbapi.srb.SRBUrl;

/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName=DataPortalConstants.DOWNLOAD_MDB, activationConfig =
 
{
  @ActivationConfigProperty(propertyName="destinationType",
    propertyValue="javax.jms.Queue"),
  @ActivationConfigProperty(propertyName="destination",
    propertyValue=DataPortalConstants.DOWNLOAD_MDB)
})
@TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
public class DownloadMessageBean extends MessageEJBObject implements MessageListener {
    
    static Logger log = Logger.getLogger(DownloadMessageBean.class);
    
    @EJB
    private LookupLocal lookupLocal;
    
    @Resource()
    EJBContext sc;
    
    @EJB
    EventLocal eventLog;
    
    @Resource(mappedName="MailSession")
    Session mailSession;
    
    public void onMessage(Message message) {
        
        log.debug("onMessage();  Query download received");
        ObjectMessage msg = null;
        
        
        if (message instanceof ObjectMessage) {
            msg = (ObjectMessage) message;
            SRBInfo info = null;
            Exception ex = null;
            try {
                info = (SRBInfo) msg.getObject();
            } catch (JMSException jmsex) {
                log.debug("Object not correct",jmsex);
            }
            try{
                
                downloadSRBFiles(info);
                
            } catch (Exception exception) {
                log.error("Exception downloading srb files", exception);
                return ;
            }
            
        }
        
        log.debug("Message download finished");
        
    }
    
    //TODO needs to be rewritten
    private void downloadSRBFiles(SRBInfo srbInfo)  throws IllegalArgumentException, LoginMyProxyException, MalformedURLException, Exception {
        //this should be done on the WSDL/Schema
        checkArguments(srbInfo);
        
        uk.ac.dl.dp.coreutil.entity.Session session = new SessionUtil(srbInfo.getSid(),em).getSession();
        GSSCredential userCred = new Certificate(session.getCredential()).getCredential();
        
        //download the data
        File srbZipFile  = null;
        // String[] fileToDownload = new String[srbInfo.getSrbFiles().length];
        
        StringBuilder files = new StringBuilder();
        String DN = session.getUserId().getDn();
        
        int index = DN.lastIndexOf("CN=");
        String name = DN.substring(index+3,DN.length());
        
        int i = 0;
        for(SRBUrl srburl : srbInfo.getSrbUrls()){
            files.append(srburl.toString()+"\n");
            //  fileToDownload[i] = srburl.toString();
            i++;
        }
        
        try {
            srbZipFile = getSrbFiles(srbInfo, userCred);
        } catch(SRBException srb){
            if(srb.getType() == -1){
                //no file
                log.warn("User: "+session.getUserId().getDn()+": File Not Found exception for: "+srbInfo.getSrbUrls());
                postMail( session.getUserId().getDpUserPreference().getEmail(), "Download failed", "Hi "+name+"\n\nDownload of :\n\n"+files.toString()+"\n\nfailed:  \n\nReason: File Not Found exception", "dataportal_download@dl.ac.uk") ;
                return ;
            } else  if(srb.getType() == -2){
                //no read access
                log.warn("User: "+session.getUserId().getDn()+" has no read access to: "+srbInfo.getSrbUrls());
                postMail( session.getUserId().getDpUserPreference().getEmail(), "Download failed", "Hi "+name+"\n\nDownload of :\n\n"+files.toString()+"\n\nfailed:  \n\nReason: No read access to data ", "dataportal_download@dl.ac.uk") ;
                return ;
            } else if(srb.getType() == -3){
                log.warn("User: "+session.getUserId().getDn()+" is not registered to SRB: "+srbInfo.getSrbUrls());
                postMail( session.getUserId().getDpUserPreference().getEmail(), "Download failed", "Hi "+name+"\n\nDownload of :\n\n"+files.toString()+"\n\nfailed:  \n\nReason: "+srb.getMessage(), "dataportal_download@dl.ac.uk") ;
                return ;
            } else {
                log.warn("Error downloading file, retrying once more",srb);
                try {
                    // srbInfo.setSrbFiles(fileToDownload);
                    srbZipFile = getSrbFiles(srbInfo, userCred);
                } catch (Exception ex2) {
                    log.error("Error no. 2 downloading file, send failed email",ex2);
                    String reason = null;
                    if(ex2 instanceof SRBException){
                        if(ex2.getMessage() == null || ex2.getMessage().equals("")){
                            reason = ((SRBException)ex2).getStandardMessage();
                        }
                    } else reason = ex2.getMessage();
                    postMail( session.getUserId().getDpUserPreference().getEmail(), "Download failed", "Hi "+name+"\n\nDownload of :\n\n"+files.toString()+"\n\nfailed:  \n\nReason: "+reason, "dataportal_download@dl.ac.uk") ;
                    return ;
                }
            }
        } catch (Exception ex) {
            log.warn("Error downloading file, retrying once more",ex);
            try {
                // srbInfo.setSrbFiles(fileToDownload);
                srbZipFile = getSrbFiles(srbInfo, userCred);
            } catch (Exception ex2) {
                log.error("Error no. 2 downloading file, send failed email",ex2);
                String reason = null;
                if(ex2 instanceof SRBException){
                    if(ex2.getMessage() == null || ex2.getMessage().equals("")){
                        reason = ((SRBException)ex2).getStandardMessage();
                    }
                } else reason = ex2.getMessage();
                postMail( session.getUserId().getDpUserPreference().getEmail(), "Download failed", "Hi "+name+"\n\nDownload of :\n\n"+files.toString()+"\n\nfailed:  \n\nReason: "+reason, "dataportal_download@dl.ac.uk") ;
                return ;
            }
        }
        
        File newLocation = zipFiles(srbInfo, srbZipFile);
        
        //send event log
        sendEventLog(srbInfo.getSid(),"Email download",srbInfo.getSrbUrls());
        
        //if reached here, email user
        email(srbInfo,session.getUserId(),newLocation);
    }
    
//@WebMethod
   /* public boolean downloadSRBFiles(SRBInfo srbInfo, ServiceInfo serviceInfo) throws IllegalArgumentException, LoginMyProxyException, MalformedURLException, Exception {
        //this should be done on the WSDL/Schema
        checkArguments(srbInfo, serviceInfo);
    
        //download users proxy
        GSSCredential userCred = downloadUsersProxy(serviceInfo);
    
        //download the data
        File srbZipFile  = getSrbFiles(srbInfo, userCred);
    
        //need to copy to server location
        File newLocation = new File(DataPortalConstants.FILE_DOWNLOAD_LOCATION,srbZipFile.getName());
        if(!newLocation.getParentFile().exists()) newLocation.getParentFile().mkdir();
        log.trace("Copying file to: "+newLocation.getAbsolutePath());
        IOTools.fileCopy(srbZipFile,newLocation);
    
         //send event log
        //sendEventLog(sid,"Email download",urls);
    
        //if reached here, email user
        return email(srbInfo,serviceInfo,newLocation);
    }*/
    
    
    
    
    
    private File zipFiles(SRBInfo srbInfo, File srbZipFile) throws IOException {
        DPConstants constants = getConstants();
        
        //TODO sort out constants
        //need to copy to server location
        /*File newLocationDir = new File(constants.getDownloadLocation()+File.separator+"download");
        if(!newLocationDir.exists()) newLocationDir.mkdir();
        File newLocation = new File(constants.getDownloadLocation()+File.separator+"download",srbZipFile.getName());
        if(!newLocation.getParentFile().exists()) newLocation.getParentFile().mkdir();
        log.trace("Copying file to: "+newLocation.getAbsolutePath());
        IOTools.fileCopy(srbZipFile,newLocation);
        return newLocation;*/
        return null;
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
        
        if(srbInfo.getSrbFiles() == null && srbInfo.getSrbUrls() == null) throw new IllegalArgumentException("SRBFiles and SRBUrls cannot be null");
    }
    
    private void sendEventLog(String sid, String message, Collection<SRBUrl> srbFiles){
        //log download
        Collection<String> urls = new ArrayList<String>();
        for(SRBUrl url : srbFiles){
            urls.add(url.toString());
        }
        if(eventLog != null) eventLog.sendDownloadEvent(sid,"Email download",urls);
        else log.warn("EventLocal is null, might be testing, no event saved");
        
    }
    
    private GSSCredential downloadUsersProxy(ServiceInfo serviceInfo) throws LoginMyProxyException {
        GSSCredential userCred = null;
        try {
            //lookup proxy and contact for users credential
            
            userCred = DelegateCredential.getProxy(serviceInfo.getMyProxyUsername(), serviceInfo.getMyProxyPassword(), 2, PortalCredential.getPortalProxy(),
                    DataPortalConstants.MYPROXY_HOST,DataPortalConstants.MYPROXY_PORT,DataPortalConstants.MYPROXY_DN);
            return userCred;
        } catch (MyProxyException mex) {
            log.warn("Error from myproxy server: "+mex.getMessage(),mex);
            throw new LoginMyProxyException(mex);
        } catch (Exception e) {
            log.warn("Unexpected error from myproxy: "+e.getMessage(),e);
            throw new LoginMyProxyException(e);
        }
    }
    
    private File getSrbFiles(SRBInfo srbInfo, GSSCredential userCred) throws MalformedURLException, Exception {
        
        SRBUrl url = srbInfo.getSrbUrls().iterator().next();
        log.trace("Setting download info: "+userCred.getName().toString()+", host: "+url.getHostAndPort());
        //man.setCredentials(getSRBHost(srbInfo.getSrbFiles()),getSRBPort(srbInfo.getSrbFiles()),userCred);
        
        SRBFileManagerThread man = new SRBFileManagerThread(srbInfo.getSrbUrls(),userCred ,true);
        // man.setZipeFile(true);
        man.setFolderFileOnly(true);
        
        //check if given default user info
        if(srbInfo.getDefaultUser() != null){
            log.debug("Using default info: "+srbInfo.getDefaultUser());
            man.setDefaultUser(srbInfo.getDefaultUser());
            man.setDefaultPass(srbInfo.getDefaultPassword());
            man.setDefaultDomain(srbInfo.getDefaultDomain());
        } else {
            try {
                //check if we have this info in the database
                SrbServer srbServer = (SrbServer)em.createNamedQuery("SrbServer.findByHostname").setParameter("hostname",url.getHost()).getSingleResult();
                if(srbServer.isDefaultSet()){
                    man.setDefaultUser(srbServer.getDefaultUser());
                    man.setDefaultPass(srbServer.getDefaultPassword());
                    man.setDefaultDomain(srbServer.getDefaultDomain());
                }
            } catch(NoResultException nre){
                log.info("No SRBServers found in database, using ticketuser for connection");
            } catch (Exception ex) {
                //dont use any default settings
                log.warn("Error searching srbServers",ex);
            }
            
        }
        log.info("Starting download");
        File returnedFile = null;
        try {
            man.start();
            
            while(true){
                if(man.isFinished()){
                    
                    break;
                } else if(man.getException() != null){
                    //System.out.println("exception "+th.getException());
                    break;
                } else if(man.getException() == null){
                    try {
                        Thread.sleep(5000);
                    } catch (InterruptedException ex) {}
                }
            }
            
            //System.out.println("Finished upload, awaiting error report.");
            // System.out.println("");
            if(man.getException() == null){
                log.info("No errors downloading: "+srbInfo.getSrbUrls());
                log.trace("Returned file path is: "+man.getFile().getAbsolutePath());
                log.trace(man.getTimeStats());
                
            } else {
                throw man.getException();
            }
        } catch (Exception ex) {
            man.stopDownload();
            man.stop();
            throw ex;
        }
        log.info("finished download: "+man.getFile().getAbsolutePath());
        
        return man.getFile();
        
    }
    
    
    private DPConstants getConstants() {
        return  em.find(DPConstants.class,1L);
    }
    
    private boolean email(SRBInfo srbInfo, ServiceInfo serviceInfo, File srbZipFile) throws UnknownHostException {
        log.trace("Emailing now");
        try {
            DPConstants constants = getConstants();
            String message = "Data Portal download";
            // String hostURL = "http://"+host.getHostAddress()+":"+sreq.getServerPort()+contextPath+"/download/"+srbZipFile.getName();
         //TODO   postMail(serviceInfo.getEmail(),message,"https://"+constants.getServiceName()+":"+constants.getPortNumber()+constants.getContextRoot()+"/download/"+srbZipFile.getName(),"dataportal_download@dl.ac.uk");
            postMail(serviceInfo.getEmail(),message,"https://:/download/"+srbZipFile.getName(),"dataportal_download@dl.ac.uk");
            return true;
        } catch (MessagingException ex) {
            log.error("Not mailed",ex);
            return false;
        }
        
    }
    
    private boolean email(SRBInfo srbInfo, User user, File srbZipFile) throws UnknownHostException {
        log.trace("Emailing now");
        try {
            DPConstants constants = getConstants();
            String message = "Data Portal download";
            
            
            StringBuilder body = new StringBuilder();
            
            int index = user.getDn().lastIndexOf("CN=");
            String name = user.getDn().substring(index+3,user.getDn().length());
            
            body.append("Hi "+name+"\n");
            body.append("\n");
            body.append("Your requested download from the Data Portal can be downloaded at:\n");
            body.append("\n");
           //TODO body.append("https://"+constants.getServiceName()+":"+constants.getPortNumber()+constants.getContextRoot()+"/download/"+srbZipFile.getName()+"\n");
            body.append("\n");
            body.append("Contents:\n\n");
            for(SRBUrl file : srbInfo.getSrbUrls()){
                body.append(file.getPath()+"\n");
            }
            body.append("\n");
            body.append("Thank you.");
            
            // String hostURL = "http://"+host.getHostAddress()+":"+sreq.getServerPort()+contextPath+"/download/"+srbZipFile.getName();
            postMail(user.getDpUserPreference().getEmail(),message,body.toString(),"dataportal_download@dl.ac.uk");
            return true;
        } catch (MessagingException ex) {
            log.error("Not mailed",ex);
            return false;
        }
    }
    
    public void postMail( String recipients, String subject, String message, String from) throws MessagingException {
        if(mailSession == null){
            log.warn("Mail session not set correctly");
            log.trace("Email send was:");
            log.trace("TO: "+recipients);
            log.trace("FROM: "+from);
            log.trace("SUBJECT: "+subject);
            log.trace("BODY: "+message);
            return ;
        }
        if(recipients == null ){
            log.warn("No email address specified");
            return ;
        }
        
        boolean debug = true;
        
        log.trace("Email send was:");
        log.trace("TO: "+recipients);
        log.trace("FROM: "+from);
        log.trace("SUBJECT: "+subject);
        log.trace("BODY: "+message);
        // Properties mailProps = new Properties();
        //mailProps.put("mail.smtp.host", "exchange06.fed.cclrc.ac.uk");
        
        // Create a session with the Java Mail API
        
        //Session mailSession = Session.getDefaultInstance(mailProps);
        
        // Create a transport object for sending mail
        Transport transport = mailSession.getTransport("smtp");
        
        // Create a new mail message
        MimeMessage mineMessage = new MimeMessage(mailSession);
        
        // Set the From and the Recipient
        mineMessage.setFrom(new InternetAddress(from));
        
        mineMessage.setRecipient(javax.mail.Message.RecipientType.TO, new InternetAddress(recipients));
        // Set the subject
        mineMessage.setSubject(subject);
        
        mineMessage.setText(message);
        
        // Save all the changes you have made to the message
        mineMessage.saveChanges();
        
        // Send the message
        transport.send(mineMessage);
    }
    
}
