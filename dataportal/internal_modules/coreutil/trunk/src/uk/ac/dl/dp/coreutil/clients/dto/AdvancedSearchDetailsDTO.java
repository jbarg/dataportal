/*
 * AdvancedSearchDetailsDTO.java
 *
 * Created on 29-Aug-2007, 14:40:59
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.clients.dto;

import java.io.Serializable;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.List;
import javax.xml.datatype.XMLGregorianCalendar;
import org.apache.log4j.Logger;
import uk.ac.dp.icatws.AdvancedSearchDetails;
import uk.ac.dp.icatws.InvestigationInclude;

/**
 * This class is used because the generated stub is not Serializable and therefore cannot be
 * send via JMS
 *
 * @author gjd37
 */
public class AdvancedSearchDetailsDTO implements Serializable {
    
    static Logger log = Logger.getLogger(AdvancedSearchDetailsDTO.class);
    
    /**
     * Investigation Name
     */
    private String investigationName; //inv title
    /**
     * Visit Id
     */
    private String visitId; //visit id
    /**
     * Investigation abstract
     */
    private String investigationAbstract;
    /**
     * Investigation type
     */
    private String investigationType;
    /**
     * Investigation grant Id
     */
    private Long grantId;
    /**
     * Investigator Name
     */
    private String backCatalogueInvestigatorString; //back catalogue investigator string
    /**
     * List of surnames
     */
    private List<String> investigators; //surname
    /**
     * Investigation number
     */
    private String experimentNumber; //inv_number
    /**
     * List of instruments
     */
    private List<String> instruments;
    /**
     * Long value of start date in datafile parameter
     */
    private Double runStart; // data file parameter,  run_number datafile_parameter
    /**
     * Long value of start end in datafile parameter
     */
    private Double runEnd;// data file parameter, run_number datafile_parameter
    /**
     * Sample name
     */
    private String sampleName; // sample
    /**
     * Datafile name
     */
    private String datafileName; // data file name
    /**
     * datafile create date
     */
    private XMLGregorianCalendar yearRangeStart; // (datafile_CREATE_time)
    /**
     * datafile create date
     */
    private XMLGregorianCalendar yearRangeEnd;// (datafile_CREATE_time)
    /**
     * List of keywords
     */
    private List<String> keywords;
    /**
     * InvestigationInclude in the data returned. {@link InvestigationInclude}
     */
    private InvestigationInclude investigationInclude;
    /**
     *
     */
    private boolean fuzzy = true;
    
    public String getInvestigationName() {
        return investigationName;
    }
    
    public void setInvestigationName(String investigationName) {
        this.investigationName = investigationName;
    }
    
    public String getVisitId() {
        return visitId;
    }
    
    public void setVisitId(String visitId) {
        this.visitId = visitId;
    }
    
    public String getInvestigationAbstract() {
        return investigationAbstract;
    }
    
    public void setInvestigationAbstract(String investigationAbstract) {
        this.investigationAbstract = investigationAbstract;
    }
    
    public String getInvestigationType() {
        return investigationType;
    }
    
    public void setInvestigationType(String investigationType) {
        this.investigationType = investigationType;
    }
    
    public Long getGrantId() {
        return grantId;
    }
    
    public void setGrantId(Long grantId) {
        this.grantId = grantId;
    }
    
    public String getBackCatalogueInvestigatorString() {
        return backCatalogueInvestigatorString;
    }
    
    public void setBackCatalogueInvestigatorString(String backCatalogueInvestigatorString) {
        this.backCatalogueInvestigatorString = backCatalogueInvestigatorString;
    }
    
    public List<String> getInvestigators() {
        return investigators;
    }
    
    public void setInvestigators(List<String> investigators) {
        this.investigators = investigators;
    }
    
    public String getExperimentNumber() {
        return experimentNumber;
    }
    
    public void setExperimentNumber(String experimentNumber) {
        this.experimentNumber = experimentNumber;
    }
    
    public List<String> getInstruments() {
        return instruments;
    }
    
    public void setInstruments(List<String> instruments) {
        this.instruments = instruments;
    }
    
    public Double getRunStart() {
        return runStart;
    }
    
    public void setRunStart(Double runStart) {
        this.runStart = runStart;
    }
    
    public Double getRunEnd() {
        return runEnd;
    }
    
    public void setRunEnd(Double runEnd) {
        this.runEnd = runEnd;
    }
    
    public String getSampleName() {
        return sampleName;
    }
    
    public void setSampleName(String sampleName) {
        this.sampleName = sampleName;
    }
    
    public String getDatafileName() {
        return datafileName;
    }
    
    public void setDatafileName(String datafileName) {
        this.datafileName = datafileName;
    }
    
    public XMLGregorianCalendar getYearRangeStart() {
        return yearRangeStart;
    }
    
    public void setYearRangeStart(XMLGregorianCalendar yearRangeStart) {
        this.yearRangeStart = yearRangeStart;
    }
    
    public XMLGregorianCalendar getYearRangeEnd() {
        return yearRangeEnd;
    }
    
    public void setYearRangeEnd(XMLGregorianCalendar yearRangeEnd) {
        this.yearRangeEnd = yearRangeEnd;
    }
    
    public List<String> getKeywords() {
        return keywords;
    }
    
    public void setKeywords(List<String> keywords) {
        this.keywords = keywords;
    }
    
    public InvestigationInclude getInvestigationInclude() {
        return investigationInclude;
    }
    
    public void setInvestigationInclude(InvestigationInclude investigationInclude) {
        this.investigationInclude = investigationInclude;
    }
    
    public boolean isFuzzy() {
        return fuzzy;
    }
    
    public void setFuzzy(boolean fuzzy) {
        this.fuzzy = fuzzy;
    }
    
    
    public AdvancedSearchDetailsDTO() {
    }
    
    public boolean hasOtherParameters(){
        if(investigators != null && investigators.size() != 0) return true;
        if(keywords != null && keywords.size() != 0) return true;
        if(sampleName != null) return true;
        if(datafileName != null) return true;
        if(runEnd != null || runStart != null) return true;
        else return false;
    }
    
    public boolean hasInstruments(){
        if(getInstruments() != null && getInstruments().size() != 0) return true;
        else return false;
    }
    
    public boolean hasInvestigators(){
        if(getInvestigators() != null && getInvestigators().size() != 0)return true;
        else return false;
    }
    
    public boolean hasKeywords(){
        if(getKeywords() != null && getKeywords().size() != 0) return true;
        else return false;
    }
    
    public boolean hasAbstract(){
        if(investigationAbstract != null && investigationAbstract.length() != 0) return true;
        else return false;
    }
    
    public boolean hasRunNumber(){
        if(runEnd == null && runStart == null) return false;
        else if(runEnd != null && runEnd != 0.0) return true;
        else if(runStart != null && runStart != 0.0) return true;
        else return false;
    }
    
    public boolean hasSample(){
        if(sampleName != null && getSampleName().length() != 0) return true;
        else return false;
    }
    
  /*  public boolean hasDataFileParameters(){
        if(dateRangeEnd != null || dateRangeStart != null || datafileName != null) return true;
        else return false;
    }*/
    
    public boolean hasInvestigationParameters(){
        if(grantId  != null || backCatalogueInvestigatorString != null || experimentNumber != null ||
                investigationAbstract != null || investigationName != null ||
                investigationType != null || visitId != null) return true;
        else return false;
        
    }
    
    private boolean checkArguements(){
        //get public, private, protected, package fields in class, but not inherited ones.
        Field[] allFields = this.getClass().getDeclaredFields();
        log.trace("Checking validity of "+getClass().getSimpleName());
        
        boolean passed = false;
        
        for (int i = 0; i < allFields.length; i++) {
            //get name of field
            String fieldName = allFields[i].getName();
                       
            //check max value now of strings
            try {
                //get value
                Object result = getProperty(fieldName, this);
                if(result != null) passed = true;
            } catch (Exception ex) {
                log.warn(getClass().getSimpleName()+": "+fieldName+" cannot be accessed.",ex);
            }            
        }
        if(!passed) throw new IllegalStateException("Atleast one field needs to be filled in");
        return true;
    }
    
     /**
     * Gets the value of the field from a passed in object using reflection
     *
     * @param name name of the field in the from object
     * @param from object wanting to get the bean property value from
     * @throws java.lang.NoSuchMethodException error
     * @throws java.lang.IllegalAccessException error
     * @throws java.lang.reflect.InvocationTargetException error
     * @throws java.lang.NoSuchFieldException error
     * @return object value of the field name
     */
    @SuppressWarnings("all")
    private Object getProperty(String name, Object from) throws NoSuchMethodException, IllegalAccessException, InvocationTargetException, NoSuchFieldException {
        String prop = Character.toUpperCase(name.charAt(0)) +
                name.substring(1);
        String mname = "get" + prop;
        
        Class[] types = new Class[]{};
        Method method = from.getClass().getMethod(mname, types);
        Object result = method.invoke(from, (Object[])types);
        
        return result;
    }
    
    public boolean isValid(){
        checkArguements();
        if(hasRunNumber()){
            //so they have set run number, check something on investigation is set)
            if(hasInvestigationParameters() || getDatafileName() != null || hasInstruments()) return true;
            else throw new IllegalStateException("Must search investigation information, instruments or datafile name if searching with run numbers");
        }/* else if(dateRangeEnd != null || dateRangeStart != null){
            //got to here run number is not set to check all others
            if(hasInvestigationParameters() || getDatafileName() != null || hasInstruments() ||
                    hasSample()) return true;
            else throw new IllegalStateException("Must search investigation information, instruments, sample name or datafile name if searching with datafile date ranges");
        }*/ else return true;
        
    }
    
    /**
     * From object goes to this
     */
    public void mergeFrom(AdvancedSearchDetails object){
        merge(object, true);
    }
    
    /**
     * From this goes to object
     */
    public void mergeTo(AdvancedSearchDetails object){
        merge(object, false);
    }
    
    /**
     * This current class is merged with another class of the same type.  Uses reflection from the two
     * classes to swap over the bean information.  Any field marked @ICAT(merge=false) is not merged.
     *
     * @param object object passed in to be merged with this class
     */
    public void merge(AdvancedSearchDetails object, boolean toThis){
        
        Field[] passsedFields = object.getClass().getDeclaredFields();
        Field[] thisFields = this.getClass().getDeclaredFields();
        
        for (Field field : passsedFields) {
            //get name of field
            String fieldName = field.getName();
            
            try {
                for(Field thisField : thisFields) {
                    // log.trace(thisField);
                    if(thisField.getName().equals(fieldName)){
                        //new way of using bean properties
                        if(toThis) swapProperty(fieldName, object, this, toThis); //works
                        else swapProperty(fieldName, this, object, toThis); //does not work for List (no list in artificats)
                    }
                }
            }  catch (Exception ex) {
                log.warn("Error transferring data for field: "+fieldName, ex);
            }
        }
        
        if(!toThis){
            //when swapping to artifcat no List (setters) so have to do it manually.
            log.trace("Swapping from instruments");
            if(instruments != null){
                for (String instrument : instruments) {
                    object.getInstruments().add(instrument);
                }
            }
            log.trace("Swapping from keywords");
            if(keywords != null){
                for (String keyword : keywords) {
                    object.getKeywords().add(keyword);
                }
            }
            log.trace("Swapping from investigators");
            if(investigators != null){
                for (String investigator : investigators) {
                    object.getInvestigators().add(investigator);
                }
            }
        }
    }
    
    /**
     * This method merges the bean property name, from the from object to the to object
     *
     * @param name name of the field in the from object
     * @param from object wanting to get the bean property value from
     * @param to object wanting to pass the bean property value to
     * @throws java.lang.NoSuchMethodException error
     * @throws java.lang.IllegalAccessException error
     * @throws java.lang.reflect.InvocationTargetException error
     * @throws java.lang.NoSuchFieldException error
     */
    @SuppressWarnings("all")
    private void swapProperty(String name, Object from, Object to, boolean toThis) throws NoSuchMethodException, IllegalAccessException, InvocationTargetException, NoSuchFieldException {
        
        String prop = Character.toUpperCase(name.charAt(0)) +
                name.substring(1);
        String mname = "get" + prop;
        
        Class[] types = new Class[]{};
        Method method = null;
        try{
            method = from.getClass().getMethod(mname, types);
        }catch(NoSuchMethodException nsme){
            mname = "is" + prop;
            method = from.getClass().getMethod(mname, types);
        }
        
        if(toThis) {
            Object result = method.invoke(from, (Object[])types);
            if(toThis){
                log.trace("Swapping to: "+name+", setting to: "+result);
            } else  log.trace("Swapping from: "+name+", setting to: "+result);
            
            mname = "set" + prop;
            types = new Class[] { from.getClass().getDeclaredField(name).getType() };
            method = to.getClass().getMethod(mname, types);
            method.invoke(to, new Object[] { result });
        } else if(!method.getReturnType().toString().equals(List.class.toString())){
            Object result = method.invoke(from, (Object[])types);
            
            if(toThis){
                log.trace("Swapping to: "+name+", setting to: "+result);
            } else  log.trace("Swapping from: "+name+", setting to: "+result);
            
            mname = "set" + prop;
            types = new Class[] { from.getClass().getDeclaredField(name).getType() };
            method = to.getClass().getMethod(mname, types);
            method.invoke(to, new Object[] { result });
        }
    }
    
}
