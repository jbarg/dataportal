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
import java.net.URL;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Collection;
import javax.annotation.Resource;
import javax.ejb.ActivationConfigProperty;
import javax.ejb.EJB;
import javax.ejb.EJBContext;
import javax.ejb.MessageDriven;
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
import uk.ac.dl.srbapi.srb.NoAccessToDataStorage;
import uk.ac.dl.srbapi.srb.ReadAccessException;
import uk.ac.dl.srbapi.srb.SRBFileManager;
import uk.ac.dl.srbapi.util.IOTools;


/**
 *
 * @author gjd37
 */
@MessageDriven(mappedName=DataPortalConstants.DOWNLOAD_MDB)

/*activationConfig =
 
{
  @ActivationConfigProperty(propertyName="destinationType",
    propertyValue="javax.jms.Queue"),
  @ActivationConfigProperty(propertyName="destination",
    propertyValue=DataPortalConstants.DOWNLOAD_MDB)
})*/
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
    
    
    private void downloadSRBFiles(SRBInfo srbInfo)  throws IllegalArgumentException, LoginMyProxyException, MalformedURLException, Exception {
        //this should be done on the WSDL/Schema
        checkArguments(srbInfo);
        
        uk.ac.dl.dp.coreutil.entity.Session session = new SessionUtil(srbInfo.getSid(),em).getSession();
        GSSCredential userCred = new Certificate(session.getCredential()).getCredential();
        
        //download the data
        File srbZipFile  = null;
        String[] fileToDownload = new String[srbInfo.getSrbFiles().length];
        
        StringBuilder files = new StringBuilder();
        String DN = session.getUserId().getDn();
        
        int index = DN.lastIndexOf("CN=");
        String name = DN.substring(index+3,DN.length());
        
        int i = 0;
        for(String srbFile : srbInfo.getSrbFiles()){
            files.append(srbFile+"\n");
            fileToDownload[i] = srbFile;
            i++;
        }
        
        try {
            srbZipFile = getSrbFiles(srbInfo, userCred);
        } catch(NoAccessToDataStorage nads){
            
            log.warn("User: "+session.getUserId().getDn()+" is not registered to SRB: "+srbInfo.getSrbFiles());
            postMail( session.getUserId().getDpUserPreference().getEmail(), "Download failed", "Hi "+name+"\n\nDownload of :\n\n"+files.toString()+"\n\nfailed:  \n\nReason: No Access to Data Storage", "dataportal_download@dl.ac.uk") ;
            return ;
            
        } catch(ReadAccessException rad){
            log.warn("User: "+session.getUserId().getDn()+" has no read access to: "+srbInfo.getSrbFiles());
            postMail( session.getUserId().getDpUserPreference().getEmail(), "Download failed", "Hi "+name+"\n\nDownload of :\n\n"+files.toString()+"\n\nfailed:  \n\nReason: No read access to data ", "dataportal_download@dl.ac.uk") ;
            return ;
            
        } catch (Exception ex) {
            log.warn("Error downloading file, retrying once more",ex);
            try {
                srbInfo.setSrbFiles(fileToDownload);
                srbZipFile = getSrbFiles(srbInfo, userCred);
            } catch (Exception ex2) {
                log.error("Error no. 2 downloading file, send failed email",ex2);
                String reason = null;
                if(ex2 instanceof SRBException){
                    if(ex2.getMessage() == null || ex2.getMessage().equals("")){
                        reason = ((SRBException)ex2).getStandardMessage();
                    }
                }else reason = ex2.getMessage();
                postMail( session.getUserId().getDpUserPreference().getEmail(), "Download failed", "Hi "+name+"\n\nDownload of :\n\n"+files.toString()+"\n\nfailed:  \n\nReason: "+reason, "dataportal_download@dl.ac.uk") ;
                return ;
            }
        }
        
        File newLocation = zipFiles(srbInfo, srbZipFile);
        
        //send event log
        sendEventLog(srbInfo.getSid(),"Email download",srbInfo.getSrbFiles());
        
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
        
        //need to copy to server location
        File newLocationDir = new File(constants.getDownloadLocation()+File.separator+"download");
        if(!newLocationDir.exists()) newLocationDir.mkdir();
        File newLocation = new File(constants.getDownloadLocation()+File.separator+"download",srbZipFile.getName());
        if(!newLocation.getParentFile().exists()) newLocation.getParentFile().mkdir();
        log.trace("Copying file to: "+newLocation.getAbsolutePath());
        IOTools.fileCopy(srbZipFile,newLocation);
        return newLocation;
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
        
        if(srbInfo.getSrbFiles() == null) throw new IllegalArgumentException("SRBFiles cannot be null");
    }
    
    private void sendEventLog(String sid, String message, String[] srbFiles){
        //log download
        Collection<String> urls = new ArrayList<String>();
        for(String url : srbFiles){
            urls.add(url);
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
        SRBFileManager man = new SRBFileManager();
        
        log.trace("Setting download info: "+userCred.getName().toString()+", host: "+getSRBHost(srbInfo.getSrbFiles())+":"+getSRBPort(srbInfo.getSrbFiles()));
        man.setCredentials(getSRBHost(srbInfo.getSrbFiles()),getSRBPort(srbInfo.getSrbFiles()),userCred);
        man.setZipeFile(true);
        man.setFolderFilesOnly(true);
        
        //check if given default user info
        if(srbInfo.getDefaultUser() != null){
            log.debug("Using default info: "+srbInfo.getDefaultUser());
            man.setDefaultUser(srbInfo.getDefaultUser());
            man.setDefaultPass(srbInfo.getDefaultPassword());
            man.setDefaultDomain(srbInfo.getDefaultDomain());
        } else {
            try {
                //check if we have this info in the database
                SrbServer srbServer = (SrbServer)em.createNamedQuery("SrbServer.findByHostname").setParameter("hostname",getSRBHost(srbInfo.getSrbFiles())).getSingleResult();
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
            returnedFile = man.getSRBFile(srbInfo.getSrbFiles());
        } catch (Exception ex) {
            man.stop();
            throw ex;
        }
        log.info("finished download: "+returnedFile.getAbsolutePath());
        
        return returnedFile;
        
    }
    
    private  String getSRBHost(String[] urls) throws MalformedURLException{
        String host1 = urls[0].toLowerCase().replace("srb://","http://");
        return new URL(host1).getHost();
    }
    
    private  int getSRBPort(String[] urls){
        try {
            String host = urls[0].toLowerCase().replace("srb://","http://");
            int port = new URL(host).getPort();
            if(port == -1){
                log.info("Port set to nothing in "+urls[0]+" setting to default: " +DataPortalConstants.SRB_PORT);
                return DataPortalConstants.SRB_PORT;
            }else return port;
        } catch (MalformedURLException ex) {
            log.warn("Unable to get port, trying default as: "+DataPortalConstants.SRB_PORT);
            return DataPortalConstants.SRB_PORT;
        }
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
            postMail(serviceInfo.getEmail(),message,"https://"+constants.getServiceName()+":"+constants.getPortNumber()+constants.getContextRoot()+"/download/"+srbZipFile.getName(),"dataportal_download@dl.ac.uk");
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
            body.append("https://"+constants.getServiceName()+":"+constants.getPortNumber()+constants.getContextRoot()+"/download/"+srbZipFile.getName()+"\n");
            body.append("\n");
            body.append("Contents:\n\n");
            for(String file : srbInfo.getSrbFiles()){
                body.append(file+"\n");
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
