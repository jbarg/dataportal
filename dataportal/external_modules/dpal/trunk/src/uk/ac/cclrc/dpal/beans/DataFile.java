package uk.ac.cclrc.dpal.beans;


public class DataFile extends Common
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
            return "[name]:"+name+
             " [id]:"+id+
             " [uri]:"+uri+
             " [facility]:"+facility+
             " [dataset id]:"+dataSetId ;
   }

}
  