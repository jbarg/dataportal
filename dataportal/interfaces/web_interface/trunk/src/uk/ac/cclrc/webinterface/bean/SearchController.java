package uk.ac.cclrc.webinterface.bean ;

//servlet(container stuff) + jsf(framework stuff)
import javax.servlet.*;
import javax.servlet.http.*;
import javax.faces.context.*;

public class SearchController {

   //needed if inputtext bound to one bean and controller bound to another bean
   //but usually they should be in the same bean. - method 2
   //private SearchBean getSearchBean(){
   //     SearchBean sb = null;
   //     Object obj = FacesContext.getCurrentInstance().getApplication()
   //     .createValueBinding("#{searchBean}")
   //     .getValue(FacesContext.getCurrentInstance());
   //     if(obj != null){
   //         sb = (SearchBean)obj;
   //     }
   //     return sb;
   // }

   private String keyword = null ;

   public void setKeyword(String keyword){
       this.keyword=keyword ;
    }

   public String getKeyword() {
       return keyword ;
    }


    public String studySearch() {

        //and remove attribute keyword above also - method 1
        //ExternalContext context = FacesContext.getCurrentInstance().getExternalContext();
        //HttpServletRequest request = (HttpServletRequest)context.getRequest();
        //String keyword = request.getParameter("form1:keyword");

        //get hold of form bean - method 2
        //SearchBean sb = getSearchBean() ;
        //if (sb != null && sb.getKeyword().compareTo("bad") == 0) { 

        if (getKeyword().compareTo("bad") == 0) { 
            return("bad");
        } else {
            return("success");
        }
    }
}


