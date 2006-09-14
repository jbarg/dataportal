/*
 * Keyword.java
 *
 * Created on 04 September 2006, 11:02
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.navigation;

import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import java.util.TreeMap;
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.cclrc.dpal.beans.Keyword;
import org.apache.log4j.*;
import javax.faces.context.FacesContext;
import javax.faces.model.SelectItem;
import com.sun.j2ee.blueprints.ui.autocomplete.AutoCompleteUtilities;
import com.sun.j2ee.blueprints.ui.autocomplete.CompletionResult;
import uk.ac.dl.dp.web.backingbeans.ApplicationBean;
import uk.ac.dl.dp.web.util.AbstractApplicationBean;
import uk.ac.dl.dp.web.util.BaseBean;
import uk.ac.dl.dp.web.backingbeans.Visit;
import javax.faces.event.ValueChangeEvent;
/**
 *
 * @author gjd37
 */
public class KeywordBean extends AbstractApplicationBean {
    
    private static Logger log = Logger.getLogger(KeywordBean.class);
    
    private Visit visit;
    
    /** Creates a new instance of Keyword */
    public KeywordBean() {
        log.trace("Keyword bean init");
    }
    
    public void completeCity(FacesContext context, String prefix, CompletionResult result) {
        String defaultFacility= getVisit().getUserPreferences().getDefaultFacility();
        log.trace("Completing City - " + prefix +" for default: "+defaultFacility);
        ArrayList<String> words2 = new ArrayList<String>();
        
        for(String item : getVisitData().getCurrentSelectedFacilities()){
            log.trace("ApplicationBean.getKeywords: "+item);
            String[] keywords2 = ApplicationBean.getKeywords(item);
            for(String j : keywords2){
                words2.add(j);
            }
        }
        
        if(prefix.indexOf(" ") != -1 ){
            String[] words = prefix.split(" ");
            AutoCompleteUtilities.addMatchingItems(words2.toArray(new String[words2.size()]), words[words.length-1], result);
            
            
        } else if(prefix.indexOf("%20") != -1 ){
            String[] words = prefix.split("%20");
            AutoCompleteUtilities.addMatchingItems(words2.toArray(new String[words2.size()]), words[words.length-1], result);
            
        } else{
            AutoCompleteUtilities.addMatchingItems(words2.toArray(new String[words2.size()]), prefix, result);
            
            
        }
    }
    
    
    public void selectedFacilities(ValueChangeEvent event){
        log.trace("new value: "+event.getNewValue()+" "+event.getNewValue().getClass());
        
        getVisitData().setCurrentSelectedFacilities((ArrayList<String>)event.getNewValue());
        FacesContext context = FacesContext.getCurrentInstance();
        context.renderResponse();
        
    }
    
}
