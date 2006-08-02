package uk.ac.dl.dp5.clients.transfer;

import java.io.File;
import java.security.cert.CertificateException;
import java.util.Collection;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import org.apache.log4j.Logger;


import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.sessionbeans.transfer.TransferRemote;

import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
public class TransferDelegate {
    
    private TransferRemote tr ;
    
    private  static Logger log = Logger.getLogger(TransferDelegate.class);
    
    private String sid;
    private Collection<String> srbUrl;
    private File file;
    private String stats;
    private Exception exception;
    private boolean isFinished = false;
    
    /** Creates a new instance of SessionDelegate */
    public  TransferDelegate(String sid, Collection<String> srbUrl) throws NamingException {
        Context ic = new InitialContext();
        tr  = (TransferRemote)ic.lookup(DataPortalConstants.TRANSFER);
        this.sid = sid;
        this.srbUrl = srbUrl;
    }
    
    public void start() throws Exception , CertificateException, SessionTimedOutException ,SessionNotFoundException{
        try {
            
            startDownloadSRBFile(this.sid,this.srbUrl);
        } catch (Exception ex) {
            remove();
            throw ex;
        }
    }
    
    /*All TransferBean methods here*/
    public void startDownloadSRBFile(String sid, Collection<String> srbUrl) throws  CertificateException, SessionNotFoundException, SessionTimedOutException{
        tr.startDownloadSRBFile(sid,srbUrl);
    }
    
    public boolean isFinished(){
        if(!isFinished){
            isFinished = tr.isFinished();
            if(isFinished){
                //download complete, get info
                this.file = tr.getFile();
                this.exception = tr.getException();
                this.stats = tr.getStats();
                //download complete, remove transferbean
                remove();
                return true;
            } else return false;
        }
        return true;
    }
    
    public int percentageComplete(){
        return tr.percentageComplete();
    }
    
    public Exception getException(){
        if(isFinished){
            return this.exception;
        } else return tr.getException();
    }
    
    public void remove(){
        if(tr != null) {
            try {
                tr.remove();
            } catch(Throwable e){}
        }
    }
    
    public File getFile(){
        if(isFinished){
            return this.file;
        } else    return tr.getFile();
    }
    
    public String getStats(){
        if(isFinished){
            return this.stats;
        } else return tr.getStats();
    }
    
    protected void finalize() throws Throwable {
        super.finalize();        
        remove();
        
    }
    
}
