
package uk.ac.dl.dp.coreutil.interfaces;

import edu.sdsc.grid.io.srb.SRBException;
import java.io.File;
import java.security.cert.CertificateException;
import java.util.Collection;
import javax.ejb.Local;
import javax.ejb.Remote;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;


/**
 * This is the business interface for Transfer enterprise bean.
 */
@Local
public interface TransferLocal {
    public void startDownloadSRBFile(String sid, Collection<String> srbUrl) throws  CertificateException, SessionNotFoundException, SessionTimedOutException;
    
    public boolean isFinished();
    
    public int percentageComplete();
    
    public Exception getException();
        
    public void remove();

    public File getFile();
    
    public String getStats();
}
