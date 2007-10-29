/*
 * SRBUrl.java
 *
 * Created on 01 March 2007, 08:19
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.io.Serializable;
import java.net.MalformedURLException;
import java.net.URL;

/**
 *
 * @author gjd37
 */
public class SRBUrl implements Serializable {
    
    
    private String srbUrl;
    private URL url;
    
    
    
    /** Creates a new instance of SRBUrl */
    public SRBUrl(String srbUrl) throws MalformedURLException {
        this.srbUrl  = srbUrl;
        String srburlReplaced = srbUrl.replaceFirst("srb://","http://");
        
        this.url = new URL(srburlReplaced);
    }
    
    public String getPath(){
        return url.getPath();
    }
    
    public String getHost(){
        return url.getHost();
    }
    
    public String getFile(){
        return url.getFile();
    }
    
    public int getPort(){
        if(url.getPort() == -1) return 5544;
        else return url.getPort();
    }
    
    public String getHostAndPort(){
        return getHost()+":"+getPort();
    }
    
    public String toString(){
        return srbUrl;
    }
    
    public int hashCode() {
        int hash = 0;
        
        hash += (this.srbUrl != null ? this.srbUrl.hashCode() : 0);
        
        return hash;
    }
    
    public boolean equals(Object srbUrl) {
        boolean is_equal = false ;
        if(srbUrl instanceof  SRBUrl){
            SRBUrl url = (SRBUrl)srbUrl;
            if ( (this.srbUrl != null && url.srbUrl !=null) && this.srbUrl.equals(url.srbUrl)) {
                is_equal = true ;
            }
        } else is_equal = false;
        
        return is_equal ;
    }
}
