package uk.ac.dl.dp5.clients.transfer;

import java.io.File;
import java.security.cert.CertificateException;
import java.util.Collection;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import org.apache.log4j.Logger;

import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.sessionbeans.transfer.TransferRemote;

import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;

/**
 *
 * @author gjd37
 */
public class TransferDelegate {
    
    private static TransferRemote tr ;
    
    private  static Logger log = Logger.getLogger(TransferDelegate.class);
    
    private String sid;
    private Collection<String> srbUrl;
    
    /** Creates a new instance of SessionDelegate */
    public  TransferDelegate(String sid, Collection<String> srbUrl) throws NamingException {
        Context ic = new InitialContext();
        tr  = (TransferRemote)ic.lookup("TransferEJB");
        this.sid = sid;
        this.srbUrl = srbUrl;
    }
    
    public File start() throws Exception , CertificateException, SessionTimedOutException ,SessionNotFoundException{
        try {
            
            startDownloadSRBFile(this.sid,this.srbUrl);
            
            while(true){
                if(isFinished()){
                    
                    break;
                } else if(getException() != null){
                    
                    break;
                } else if(getException() == null){
                    log.debug(""+percentageComplete()+" %");
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException ex) {}
                }
            }
            
            if(getException() == null){
                log.debug("No errors");
                log.debug("Returned file path is: "+getFile().getAbsolutePath());
                log.debug(getStats());
                return getFile();
                
            } else {
                throw getException();
            }
        } finally {
            remove();
        }
    }
    
    
    /*All TransferBean methods here*/
    protected void startDownloadSRBFile(String sid, Collection<String> srbUrl) throws  CertificateException, SessionNotFoundException, SessionTimedOutException{
        tr.startDownloadSRBFile(sid,srbUrl);
    }
    
    protected boolean isFinished(){
        return tr.isFinished();
    }
    
    protected int percentageComplete(){
        return tr.percentageComplete();
    }
    
    protected Exception getException(){
        return tr.getException();
    }
    
    protected void remove(){
        if(tr != null) {
            try {
                tr.remove();
            } catch(Throwable e){}
        }
    }
    
    protected File getFile(){
        return tr.getFile();
    }
    
    protected String getStats(){
        return tr.getStats();
    }
    
    protected void finalize() throws Throwable {
        if(tr != null) {
            try {
                tr.remove();
            } catch(Throwable e){}
        }
    }
    
}
