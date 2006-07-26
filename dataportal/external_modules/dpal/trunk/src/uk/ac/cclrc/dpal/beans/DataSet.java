package uk.ac.cclrc.dpal.beans;

import java.io.Serializable;

public class DataSet extends Common 
{
   //bean begins
   private String dataSetStatus = null ;
   private String dataSetType = null ;
   private String description = null ;
   private String investigationId = null ;

   //getters
   public String getDataSetStatus() {
      return this.dataSetStatus ;
   }
  
   public String getDataSetType() {
      return this.dataSetType ;
   }
  
   public String getDescription() {
      return this.description ;
   }
 
   public String getInvestigationId() {
      return this.investigationId ;
   }
 
   //setters
   public void setDataSetStatus(String dataSetStatus) {
      this.dataSetStatus=dataSetStatus ;
   }
  
   public void setDataSetType(String dataSetType) {
      this.dataSetType=dataSetType ;
   }
  
   public void setDescription(String description) {
      this.description=description ;
   }

   public void setInvestigationId(String investigationId) {
      this.investigationId=investigationId ;
   }

   //support for printing
   public String toString() {
            return "[name]:"+name+
             " [id]:"+id+
             " [status]:"+dataSetStatus+
             " [type]:"+dataSetType+
             " [description]:"+description+
             " [facility]:"+facility+
             " [investigation id]:"+investigationId ;
   }

}
  
