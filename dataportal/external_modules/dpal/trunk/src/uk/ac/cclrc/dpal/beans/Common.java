package uk.ac.cclrc.dpal.beans;

import java.io.Serializable;

public class Common implements Serializable
{
   //bean begins
   private String name = null ;
   private String id = null ;
   private String facility = null ;
private boolean selected = false;
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


}
  
