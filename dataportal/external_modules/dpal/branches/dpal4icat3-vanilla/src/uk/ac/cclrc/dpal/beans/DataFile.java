package uk.ac.cclrc.dpal.beans;

import java.io.Serializable;


public class DataFile extends Common implements Serializable
{
   //bean begins
   private String uri = null ;
   private String dataSetId = null ;

   //getters
   public String getUri() {
      return this.uri ;
   }
  
   public String getDataSetId() {
      return this.dataSetId ;
   }

   //setters
   public void setUri(String uri) {
      this.uri=uri ;
   }
  
   public void setDataSetId(String dataSetId) {
      this.dataSetId=dataSetId ;
   }

   //to support printing
   public String toString() {
            return "[name]:"+getName()+
             " [id]:"+getId()+
             " [uri]:"+uri+
             " [facility]:"+getFacility()+
             " [dataset id]:"+dataSetId ;
   }
  
   
   public String getDpId(){
       return getFacility()+"-"+getId();
   }

}
  
