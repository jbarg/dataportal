package uk.ac.cclrc.dpal.beans;
import java.io.Serializable;
import java.util.* ;

public class Study extends Common implements Serializable
{
   //bean begins
   private String startDate = null ;
   private String endDate = null ;
   //using the following as it supresses duplicates(just in case)
   private LinkedHashSet<String> keyword = new LinkedHashSet<String>() ;

   
   //getters
   public String getStartDate() {
      return this.startDate ;
   }
  
   public String getEndDate() {
      return this.endDate ;
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
   public void setStartDate(String startDate) {
      this.startDate=startDate ;
   }
  
   public void setEndDate(String endDate) {
      this.endDate=endDate ;
   }
  
   public void setKeyword(String keyword) {
      this.keyword.add(keyword) ;
   }
   
   //for printing out
   public String toString() { 
      StringBuffer sb = new StringBuffer() ;

      for(String k : keyword){
         sb.append(k);
         sb.append(" ") ;
      }

      return "[name]:"+getName()+
             " [id]:"+getId()+
             " [start_date]:"+startDate+
             " [end_date]:"+endDate+
             " [facility]:"+getFacility()+
             " [keyword]:"+sb.toString()  ;
   }

}


  
