/*
 * TransferBean.java
 *
 * Created on 28 June 2006, 07:59
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.transfer;

import java.io.File;
import java.security.cert.CertificateException;
import java.util.Collection;
import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import javax.ejb.PrePassivate;
import javax.ejb.Remove;
import javax.ejb.Stateful;
import org.apache.log4j.Logger;
import org.ietf.jgss.GSSCredential;
import uk.ac.dl.dp5.entity.Session;
import uk.ac.dl.dp5.entity.User;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.util.CachingServiceLocator;
import uk.ac.dl.dp5.util.Certificate;
import uk.ac.dl.dp5.util.DPEvent;
import uk.ac.dl.dp5.util.DataPortalConstants;
import uk.ac.dl.dp5.util.SessionUtil;
import uk.ac.dl.dp5.util.UserUtil;
import uk.ac.dl.dp5.util.DataPortalConstants;
import uk.ac.dl.srbapi.srb.SRBFileManagerThread;
/**
 *
 * @author gjd37
 */
@Stateful(mappedName=DataPortalConstants.TRANSFER)
public class TransferBean extends SessionEJBObject implements TransferRemote, TransferLocal {
    
    static Logger log = Logger.getLogger(TransferBean.class);
    
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
            new UserUtil(user).sendEventLog(DPEvent.TRANSFER,this.srbUrl.iterator().next()+" ...");
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
    
    private void download(GSSCredential credential, Collection<String> srbUrl) {
        //use default configuration location ~/.srb
        th = new SRBFileManagerThread();
        
        try {
            th.setSRBFile(srbUrl.toArray(new String[srbUrl.size()]));
            th.setCredentials(DataPortalConstants.SRB_HOST,DataPortalConstants.SRB_PORT,credential);
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
