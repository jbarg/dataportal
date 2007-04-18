/*
 * DownloadServiceLocal.java
 *
 * Created on 11 December 2006, 10:41
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.interfaces;

import java.net.MalformedURLException;
import javax.ejb.Remote;
import uk.ac.dl.dp.coreutil.entity.DPConstants;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;
import uk.ac.dl.dp.coreutil.util.SRBInfo;

/**
 *
 * @author gjd37
 */

@Remote
public interface DownloadServiceRemote {
 
  //  boolean downloadSRBFiles(SRBInfo srbInfo, ServiceInfo serviceInfo) throws IllegalArgumentException, LoginMyProxyException, MalformedURLException, Exception;

    public void downloadSRBFiles(String sid, SRBInfo srbInfo);
  
     public boolean setConstants(DPConstants constants);
     
     public boolean isSet();
}
