/* Copyright 2005 Sun Microsystems, Inc. All rights reserved.
   You may not modify, use, reproduce, or distribute this software except
   in compliance with the terms of the License at:
       http://developer.sun.com/berkeley_license.html
 */
package uk.ac.dl.dp.web.backingbeans;

import java.io.BufferedInputStream;
import java.io.InputStreamReader;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;

import javax.faces.context.FacesContext;

import com.sun.j2ee.blueprints.ui.autocomplete.AutoCompleteUtilities;
import com.sun.j2ee.blueprints.ui.autocomplete.CompletionResult;
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.cclrc.dpal.beans.Keyword;
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.delegates.LookupDelegate;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.util.DPFacilityType;
import uk.ac.dl.dp.web.navigation.KeywordBean;
import org.apache.log4j.*;
import uk.ac.dl.dp.web.util.AbstractApplicationBean;

/**
 * This application bean is a managed JSF bean in application scope.
 * When created it will read in a list of US cities and their zip codes.
 * It makes this list available to others who ask for it - such as the
 * AJAX autocompletion servlet.
 *
 * @author Tor Norbye
 * @author Mark Basler
 */
public class ApplicationBean extends AbstractApplicationBean {
    private static Logger log = Logger.getLogger(ApplicationBean.class);
    
    
    private static  HashMap<String, String[]> keywordsStatic = new HashMap<String, String[]>();
    
    private HashMap<String, String[]> keywords = new HashMap<String, String[]>();
    
    static {
        try {
            initializeCitiesStatic();
        } catch(Throwable e){
            log.error("Unable to initialize ApplicationBean",e);
        }
    }
    
    /** Creates a new instance of ApplicationBean */
    public ApplicationBean() {
        //initializeCities();
    }
    
    private void initializeCities() {
        
        log.trace("Keyword bean init");
        
        for(FacilityDTO facility : LookupDelegate.getInstance().lookup(DPFacilityType.WRAPPER)){
            try {
                String[] fac = QueryDelegate.getInstance().getKeywords(facility.getFacility());
                log.debug("Got keywords for: "+facility.getFacility()+", no. : "+fac.length);
                
                keywords.put(facility.getFacility(),fac);
            } catch (Exception ex) {
                log.error("Unable to get keywords for: "+facility);
            }
        }
    }
    
    private static void initializeCitiesStatic() {
        log.trace("Keyword bean init static");
        
        
        for(FacilityDTO facility : LookupDelegate.getInstance().lookup(DPFacilityType.WRAPPER)){
            try {
                String[] fac = QueryDelegate.getInstance().getKeywords(facility.getFacility());
                log.debug("Got keywords for: "+facility.getFacility()+", no. : "+fac.length);
                
                keywordsStatic.put(facility.getFacility(),fac);
            } catch (Exception ex) {
                log.error("Unable to get keywords for: "+facility);
            }
            
        }
    }
    
    public static String[] getKeywords(String facility){
        String[] keywords = keywordsStatic.get(facility);
        if(keywords == null || keywords.length == 0){
            log.trace("Checking if "+facility+" data there yet");
            String[] fac;
            try {
                fac = QueryDelegate.getInstance().getKeywords(facility, false);
                 keywordsStatic.put(facility,fac);
            } catch (Exception ex) {
               log.error("Unable to get keywords for: "+facility);
               return keywords;
            }
            return fac;
        }
        return keywords;
    }
    
    
}
