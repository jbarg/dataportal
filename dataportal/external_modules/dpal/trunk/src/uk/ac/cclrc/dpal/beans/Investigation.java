package uk.ac.cclrc.dpal.beans;

import java.util.LinkedHashSet;
import java.io.Serializable;

public class Investigation extends Common implements Serializable {
    //bean begins
    private String investigationType = null ;
    private String investigationAbstract = null ;
    //using the following as it supresses duplicates(just in case)
    private LinkedHashSet<String> keyword = new LinkedHashSet<String>() ;
    
    
    
    //getters
    public String getInvestigationType() {
        return this.investigationType ;
    }
    
    public String getInvestigationAbstract() {
        return this.investigationAbstract ;
    }
    
    public LinkedHashSet getKeyword() {
        return this.keyword ;
    }
    
    public String getFirstKeyword() {
        for (String s : keyword){
            return s;
        }
        return null ;
    }
    
    
    //setters
    public void setKeyword(String keyword) {
        this.keyword.add(keyword) ;
    }
    
    
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
        
        for(String k : keyword){
            sb.append(k);
            sb.append(" ") ;
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

