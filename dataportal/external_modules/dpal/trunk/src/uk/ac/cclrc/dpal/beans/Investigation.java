package uk.ac.cclrc.dpal.beans;

import java.io.Serializable;

public class Investigation extends Common 
{
   //bean begins
   private String investigationType = null ;
   private String investigationAbstract = null ;
   private String studyId = null ;

   //getters
   public String getInvestigationType() {
      return this.investigationType ;
   }
  
   public String getInvestigationAbstract() {
      return this.investigationAbstract ;
   }
  
   public String getStudyId() {
      return this.studyId ;
   }
 
   //setters
   public void setInvestigationType(String investigationType) {
      this.investigationType=investigationType ;
   }
  
   public void setInvestigationAbstract(String investigationAbstract) {
      this.investigationAbstract=investigationAbstract ;
   }
  
   public void setStudyId(String studyId) {
      this.studyId=studyId ;
   }

   //support for printing
   public String toString() {
      //can't use variable names of superclass as there are not inherited
      return "[name]:"+getName()+
             " [id]:"+getId()+
             " [type]:"+investigationType+
             " [abstract]:"+investigationAbstract+
             " [facility]:"+getFacility()+
             " [study id]:"+studyId ;

   }

}
  
