/*
 * KeywordsFileBean.java
 *
 * Created on 31-Aug-2007, 09:20:20
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.ObjectInputStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import org.apache.log4j.Logger;

/**
 * Holds the information about the keywords and last access time for the file
 * File is located on the disk at .... so when first logs in the file is read, if a new
 * file mod time is then put on the file a new set of keywords has been added and saved by the
 * core DP and this should then be used
 *
 * @author gjd37
 */
public class KeywordsFileBean {
    
     private static Logger log = Logger.getLogger(KeywordsFileBean.class);
     
    /**
     * User
     */
    private String user;
    
    /**
     * Time on the file when last accessed
     */
    private Date modTime;
    
    /**
     * Facility of the keywords
     */
    private String facility;
    
    /**
     * Keywords for the facility for the user
     */
    private Collection<String> keywords;
    
    /**
     * Location of keyword file
     */
    private File keywordFile;
    
    public KeywordsFileBean() {
    }
    
    public KeywordsFileBean(String user, String facility) {
        this.user = user;
        this.facility = facility;
        this.keywordFile = new File(DataPortalConstants.KEYWORD_LOCATION+facility+"_"+user+".keyworddata");
        loadKeywords();
    }
    
    public Date getModTime() {
        return modTime;
    }
    
    public void setModTime(Date modTime) {
        this.modTime = modTime;
    }
    
    public String getFacility() {
        return facility;
    }
    
    public void setFacility(String facility) {
        this.facility = facility;
    }
    
    public Collection<String> getKeywords() {
        return keywords;
    }
    
    public void setKeywords(Collection<String> keywords) {
        this.keywords = keywords;
    }
    
    public boolean hasBeenUpdated(){
        if(modTime.before(new Date(keywordFile.lastModified()))) return true;
        else return false;
    }
    
    public void loadKeywords(){
        log.trace("Loading keywords from "+facility+" for user: "+user);
        FileInputStream f_in = null;
        ObjectInputStream obj_in = null;
        try{
            modTime = new Date(keywordFile.lastModified());
            
            f_in = new  FileInputStream(keywordFile);
            
            // Read object using ObjectInputStream.
            obj_in = new ObjectInputStream(f_in);
            
            // Read an object.
            keywords = (Collection<String>)obj_in.readObject();           
            log.info("Loaded keywords from "+facility+" for user: "+user+" has : "+keywords.size()+" keywords");
        } catch(Exception ex){
            log.warn("Error loading keywords "+keywordFile.getAbsolutePath()+" from disk");
            keywords = new ArrayList<String>();
        } finally {
            try{
                //close streams
                obj_in.close();
                f_in.close();
            } catch(Exception ignore){}
        }
    }
}
