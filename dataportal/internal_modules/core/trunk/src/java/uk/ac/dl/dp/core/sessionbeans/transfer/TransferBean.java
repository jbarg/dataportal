/*
 * TransferBean.java
 *
 * Created on 28 June 2006, 07:59
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans.transfer;

import java.io.File;
import java.net.URL;
import java.security.cert.CertificateException;
import java.util.Collection;
import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import javax.ejb.EJB;
import javax.ejb.Init;
import javax.ejb.PrePassivate;
import javax.ejb.Remove;
import javax.ejb.Stateful;
import org.apache.log4j.Logger;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp.coreutil.entity.Session;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.interfaces.LookupLocal;
import uk.ac.dl.dp.coreutil.interfaces.TransferLocal;
import uk.ac.dl.dp.coreutil.interfaces.TransferRemote;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.Certificate;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.dp.coreutil.util.SessionUtil;
import uk.ac.dl.dp.coreutil.util.UserUtil;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
import uk.ac.dl.srbapi.srb.SRBFileManagerThread;
/**
 *
 * @author gjd37
 */
@Stateful(mappedName=DataPortalConstants.TRANSFER)
public class TransferBean extends SessionEJBObject implements TransferRemote, TransferLocal {
    
    static Logger log = Logger.getLogger(TransferBean.class);
    
    @EJB()
    LookupLocal lookup;
    
    //information about the download
    private int percentageComplete;
    private boolean isFinished = false;
    private Exception exception;
    private File zipFile;
    private String stats;
    private Collection<String> srbUrl;
    private SRBFileManagerThread th;
    
    //user info
    private User user;
    
    @PrePassivate
    public void prePassivate(){
        //only needs this on destory
        //this.sid = null;
        //this.facilities = null;
        log.debug("Unloading..");
    }
    
    @PreDestroy
    public void preDestory(){
        this.percentageComplete = 0;
        this.isFinished = false;
        this.exception = null;
        this.user = null;
        this.th = null;
        this.stats = null;
        this.zipFile = null;
        this.srbUrl = null;
        log.debug("Destroying..");
    }
    
    public void startDownloadSRBFile(String sid, Collection<String> srbUrl) throws CertificateException, SessionNotFoundException, SessionTimedOutException{
        log.debug("downloadSRBFile()");
        if(sid == null) throw new IllegalArgumentException("Session ID cannot be null.");
        if(srbUrl == null || srbUrl.size() == 0) throw new IllegalArgumentException("SRB URLs need to be valid, ie. not null or size 0.");
        this.srbUrl = srbUrl;
        
        Session session = new SessionUtil(sid).getSession();
        user = session.getUserId();
      
        log.trace("starting download for user: "+user.getDn());
        GSSCredential creds = new Certificate(session.getCredential()).getCredential();
        
        //start the download
        download(creds,srbUrl);
    }
    
    public boolean isFinished(){
        if(th.isFinished()){
            //send event log
            new UserUtil(user).sendEventLog(DPEvent.DOWNLOAD,this.srbUrl.iterator().next()+" ...");
            return true;
        }
        return false;
    }
    
    public int percentageComplete(){
        return th.getPercentageComplete();
    }
    
    public Exception getException(){
        return th.getException();
    }
    
    public File getFile(){
        return th.getFile();
    }
    
    public String getStats(){
        return th.getTimeStats();
    }
    
    @Remove
    public void remove(){
        //remove all objects
        log.debug("TransferBean Object removed");
    }
    
    @PostConstruct
    public void init(){
        log.debug("TransferBean Object inititated");
         if(CachingServiceLocator.getInstance().getEntityManager() == null){
            CachingServiceLocator.getInstance().put(em);
        }
    }
    
   /* @Init()
    public void lookupSRBServer(){
        log.debug("Init of transfer bean");
       // log.trace("Looking up SRBServer");        
    }*/
    
    private void download(GSSCredential credential, Collection<String> srbUrl) {
        //use default configuration location ~/.srb
        th = new SRBFileManagerThread();
        
        try {
            
            //get the host and port number of download
            String url = srbUrl.iterator().next();
            URL url_s  = new URL(url.replaceFirst("srb","http"));
            String host = url_s.getHost();
            int port  = url_s.getPort();
            
            log.debug("SRB Host Information: "+host+":"+port);
            th.setSRBFile(srbUrl.toArray(new String[srbUrl.size()]));
            th.setCredentials(host,port,credential);
        } catch (Exception ex) {
            log.warn("Unable to set credentials and srb file info.");
            exception = new CertificateException("Unable to set credentials and srb file info.",ex);
        }
        
        th.start();
        
       /* log.debug("Downloading SRB files, Percent complete...");
        while(true){
            if(th.isFinished()){
        
                break;
            } else if(th.getException() != null){
                //System.out.println("exception "+th.getException());
                break;
            } else if(th.getException() == null){
                log.debug(""+th.getPercentageComplete()+" %");
                percentageComplete = th.getPercentageComplete();
                try {
                    Thread.sleep(2000);
                } catch (InterruptedException ex) {}
            }
        }
        
        //System.out.println("Finished upload, awaiting error report.");
        if(th.getException() == null){
            log.debug("No errors");
            log.debug("Returned file path is: "+th.getFile().getAbsolutePath());
            log.debug(th.getTimeStats());
            zipFile = th.getFile();
            stats = th.getTimeStats();
        
            //send event log
            new UserUtil(user).sendEventLog(DPEvent.TRANSFER,srbUrl.iterator().next()+" ...");
            isFinished = true;
        } else {
            log.error("Exception downloading SRB files: "+th.getException());
            exception = th.getException();
        }*/
    }
}
