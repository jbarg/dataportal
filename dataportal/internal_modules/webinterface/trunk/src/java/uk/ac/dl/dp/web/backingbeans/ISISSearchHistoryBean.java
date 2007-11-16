/*
 * SearchBean.java
 *
 * Created on 08 August 2006, 14:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import org.apache.log4j.*;
import java.util.*;
import javax.faces.application.FacesMessage;
import javax.faces.component.UIComponent;
import javax.faces.component.UIInput;
import javax.faces.context.FacesContext;
import javax.faces.validator.ValidatorException;


/**
 *
 * @author gjd37
 */
public class ISISSearchHistoryBean extends AdvancedSearchHistoryBean {
    
    private static Logger log = Logger.getLogger(ISISSearchHistoryBean.class);
       
    private String runStartDF;
    private String runEndDF;
    
    /**
     * Investigation instrument
     */
    private String instrumentDF;
    
    
    /** Creates a new instance of AdvancedSearchBean */
    public ISISSearchHistoryBean() {
    }  
    
    public String getInstrumentDF() {
        return instrumentDF;
    }
    
    public void setInstrumentDF(String instrumentDF) {
        this.instrumentDF = instrumentDF;
    }
    
    public String getRunStartDF() {
        if(runStartDF == null || runStartDF.equals("")) return null;
        else return runStartDF;
    }
    
    public void setRunStartDF(String runStartDF) {
        this.runStartDF = runStartDF;
    }
    
    public String getRunEndDF() {
        if(runEndDF == null || runEndDF.equals("")) return null;
        else return runEndDF;
    }
    
    public void setRunEndDF(String runEndDF) {
        this.runEndDF = runEndDF;
    }
    
    private UIInput runStartUIDF;
    
    public UIInput getRunStartUIDF() {
        return runStartUIDF;
    }
    
    public void setRunStartUI(UIInput runStartUIDF) {
        this.runStartUIDF = runStartUIDF;
    }
    
    // way to validate two components.  Put val on last one and then check the local value (in this class)
    // of the other one abobve it in the page
    // before cannot be after after !!
    public void validateRun(FacesContext context, UIComponent component,  Object value) throws ValidatorException {
        log.debug("validateRunNumberDF: ");
        if (value != null) {
            if(getRunStartUIDF().getLocalValue() != null){
                Integer end =  Integer.valueOf((String)value);
                Integer start =  Integer.valueOf((String)getRunStartUIDF().getLocalValue());
                log.trace("Start "+start+" End" + end);
                if(end.intValue() < start.intValue()){
                    log.trace("Invalid");
                    throw new ValidatorException(new FacesMessage("Validation Error", "Run end number cannot be lower than run start number"));
                }
            }
        }
    }    
}
