package uk.ac.cclrc.dpal.beans;

import java.util.LinkedHashSet;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collection;

public class Investigation extends Common implements Serializable {
    //bean begins
    private String investigationType = null ;
    private String investigationAbstract = null ;
    private String visitId = null ;
    //using the following as it supresses duplicates(just in case)
    private LinkedHashSet<Investigator> investigators = new LinkedHashSet<Investigator>() ;
    private Collection<String> keywords = new ArrayList<String>();
    
    {
        //initialise
        keywords.add("Initialising keywords");
    }
    
    //getters
    public String getInvestigationType() {
        return this.investigationType ;
    }
    
    public String getInvestigationAbstract() {
        return this.investigationAbstract ;
    }
    
    public Collection<Investigator> getListOfInvestigators() {
        Collection<Investigator> investigatorsCollection = new ArrayList<Investigator>();
        for (Investigator i : investigators){
            investigatorsCollection.add(i);
        }
        return investigatorsCollection ;
    }
    
    public LinkedHashSet getInvestigators() {
        return this.investigators ;
    }
    
    public Investigator getFirstInvestigator() {
        for (Investigator i : investigators){
            return i;
        }
        return null ;
    }
    
    public String getVisitId() {
        return this.visitId ;
    }
    
    
    //adders - none bean but neeeded
    public void addInvestigator(Investigator investigator) {
        this.investigators.add(investigator) ;
    }
    
    //setters
    public void setInvestigationType(String investigationType) {
        this.investigationType=investigationType ;
    }
    
    public void setInvestigationAbstract(String investigationAbstract) {
        this.investigationAbstract=investigationAbstract ;
    }
    
    public void setVisitId(String visit_id) {
        this.visitId=visit_id ;
    }
    
    
    //support for printing
    public String toString() {
        //can't use variable names of superclass as there are not inherited
        
        StringBuffer sb = new StringBuffer() ;
        
        for(Investigator i : investigators){
            sb.append("\n\t");
            sb.append(i.toString());
        }
        
        return "[name]:"+getName()+
                " [id]:"+getId()+
                " [visit id]:"+visitId+
                " [type]:"+investigationType+
                " [abstract]:"+investigationAbstract+
                " [facility]:"+getFacility()+
                " [keyword]:"+sb.toString() ;
        
        
    }
    public boolean isAbstractNull(){
        return (investigationAbstract == null || investigationAbstract.equals("")) ? true : false;
    }
    
    public String getDpId(){
        return getFacility()+"-"+getId();
    }
    
    public boolean isInvestigatorsNull(){
        return (investigators == null || investigators.size() == 0) ? true : false;
    }

    public Collection<String> getKeywords() {
        if(keywords == null) keywords = new ArrayList<String>();
        return keywords;
    }

    public void setKeywords(Collection<String> keywords) {
        this.keywords = keywords;
    }
    
    public void addKeyword(String keyword){
        getKeywords().add(keyword);
    }    
    
    public void reset(){
        setKeywords(new ArrayList<String>());
    }
}

