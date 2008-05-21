/*
 * DownloadServiceLocal.java
 *
 * Created on 11 December 2006, 10:41
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.interfaces;

import java.util.Collection;
import javax.ejb.Local;
import javax.ejb.Remote;
import uk.ac.dl.dp.coreutil.entity.DPConstants;
import uk.ac.dl.dp.coreutil.util.SRBInfo;

/**
 *
 * @author gjd37
 */

@Local
public interface DownloadServiceLocal {
    
    //  boolean downloadSRBFiles(SRBInfo srbInfo, ServiceInfo serviceInfo) throws IllegalArgumentException, LoginMyProxyException, MalformedURLException, Exception;
    
    public void downloadSRBFiles(String sid, SRBInfo srbInfo);
    
    public boolean addConstant(DPConstants constants);
    
    public boolean setConstants(Collection<DPConstants> constants);
    
    public boolean isSet();
}
