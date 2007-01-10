package uk.ac.cclrc.dpal.beans;

import java.util.LinkedHashSet;
import java.io.Serializable;

public class Investigation extends Common implements Serializable {
    //bean begins
    private String investigationType = null ;
    private String investigationAbstract = null ;
    //using the following as it supresses duplicates(just in case)
    private LinkedHashSet<Investigator> investigators = new LinkedHashSet<Investigator>() ;
    
    
    
    //getters
    public String getInvestigationType() {
        return this.investigationType ;
    }
    
    public String getInvestigationAbstract() {
        return this.investigationAbstract ;
    }
    
    public LinkedHashSet getInvestigators() {
        return this.investigators ;
    }
    
    public String getFirstInvestigator() {
        for (Investigator i : investigators){
            return i;
        }
        return null ;
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
}

