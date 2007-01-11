package uk.ac.cclrc.dpal.beans;

import java.io.Serializable;

public class Common implements Serializable {
    //bean begins
    private String name = null ;
    private String id = null ;
    private String facility = null ;
    private boolean selected = false;
    private boolean download = false;

    //need to do this for using LinkedHashSet
    public int hashCode() {
        int hash = 0;

        hash += (this.id != null ? this.id.hashCode() : 0);
        hash += (this.name != null ? this.name.hashCode() * 2 : 0) ;
        hash += (this.facility != null ? this.facility.hashCode() * 3 : 0);

        return hash;
    }

    public boolean equals(Common com) {
       boolean is_equal = false ;
     
       if ( ((this.id != null && com.id !=null) && this.id.equals(com.id)) && 
            ((this.name != null && com.name !=null) && this.name.equals(com.name)) &&
            ((this.facility != null && com.facility !=null) && this.facility.equals(com.facility)) ) {
          is_equal = true ;
       }
       return is_equal ;
   } 

    //getters
    public String getName() {
        return this.name ;
    }
    
    public String getId() {
        return this.id ;
    }
    
    public String getFacility() {
        return this.facility ;
    }
    
    //setters
    public void setName(String name) {
        this.name=name ;
    }
    
    public void setId(String id) {
        this.id=id ;
    }
    
    public void setFacility(String facility) {
        this.facility=facility ;
    }
    
    //to support printing
    public String toString() {
        return "[name]:"+getName()+
                " [id]:"+getId()+
                " [facility]:"+getFacility();
    }
    
    public boolean isSelected() {
        return selected;
    }
    
    public void setSelected(boolean selected) {
        this.selected = selected;
    }
    
     public boolean isDownload() {
        return download;
    }
    
    public void setDownload(boolean download) {
        this.download = download;
    }
    
    
}

