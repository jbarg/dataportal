package uk.ac.cclrc.dpal.beans;

import java.io.Serializable;

public class Common implements Serializable
{
   //bean begins
   protected String name = null ;
   protected String id = null ;
   protected String facility = null ;

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
            return "[name]:"+name+
             " [id]:"+id+
             " [facility]:"+facility;
   }

}
  
