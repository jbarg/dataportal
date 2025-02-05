
package uk.ac.dl.dp.coreutil.interfaces;

import java.util.Collection;
import javax.ejb.Remote;
import uk.ac.dl.dp.coreutil.util.DPEvent;


/**
 * This is the business interface for Event enterprise bean.
 */
@Remote
public interface SendMDBRemote {
    
    public void sendEvent(String sid, DPEvent event, String description);
    public void sendKeywordEvent(String sid, Collection<String> facilities, Collection<String> keywords);
    public void sendDownloadEvent(String sid, String message, Collection<String> srburls);
    public void sendKeywordEvent(String sid, Collection<String> facilities, Collection<String> keywords, DPEvent event);
    
}
