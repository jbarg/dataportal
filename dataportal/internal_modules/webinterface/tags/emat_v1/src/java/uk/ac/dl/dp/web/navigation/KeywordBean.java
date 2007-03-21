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
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.LinkedHashSet;
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
        LinkedHashSet<String> words2 = new LinkedHashSet<String>();
        
        for(String item : getVisitData().getCurrentSelectedFacilities()){
            log.trace("ApplicationBean.getKeywords: "+item);
            String[] keywords2 = ApplicationBean.getKeywords(item);
            for(String j : keywords2){
                words2.add(j);
               
            }
        }
        
        String[] sorted = words2.toArray(new String[words2.size()]);
        //try and sort words
        Arrays.sort(sorted);
        
        //TODO problem with if spaces allowed (ie single words only)
        //need to check if current selected fac contins data in folders( there if so
        // allower spaces in so no split here!!
        if(prefix.indexOf(" ") != -1 ){
            String[] words = prefix.split(" ");
            log.trace("splitting by space: "+words[words.length-1]);
            AutoCompleteUtilities.addMatchingItems(sorted, words[words.length-1], result);
        } else if(prefix.indexOf("%20") != -1 ){
            String[] words = prefix.split("%20");
            log.trace("splitting by %20: "+words[words.length-1]);
            AutoCompleteUtilities.addMatchingItems(sorted, words[words.length-1], result);
        } else{
            log.trace("nothing split");
            AutoCompleteUtilities.addMatchingItems(sorted, prefix, result);
        }
    }
    
    
    public void selectedFacilities(ValueChangeEvent event){
        log.trace("new value!!: "+event.getNewValue()+" "+event.getNewValue().getClass());
        List alist =  Arrays.asList(event.getNewValue());
        try {
            //TODO stupid bug returning Arrays.ArrayList and unable to get ArrayList from it
            String list = alist.toString();
            list = list.replace("[","");
            list = list.replace("]","");
            list = list.replace(",","");
            String[] facs = list.trim().split(" ");
            List<String> lists= new ArrayList<String>();
            for(String ob : facs){
                lists.add(ob);
            }
            getVisitData().setCurrentSelectedFacilities(lists);
            FacesContext context = FacesContext.getCurrentInstance();
            context.renderResponse();
        } catch(Exception t) {
            log.trace(t);
        }
    }
    
    //For ajax4jsf
    public void selectedFacilities(){
        log.trace("New ajax request");
        List<String> facs = getVisitData().getCurrentSelectedFacilities();
        
        for(String fac : facs){
            log.trace(fac);
        }
    }
}
