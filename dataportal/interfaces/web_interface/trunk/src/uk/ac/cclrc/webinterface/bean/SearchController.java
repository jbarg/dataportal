package uk.ac.cclrc.webinterface.bean ;

//servlet(container stuff) + jsf(framework stuff)
import javax.servlet.*;
import javax.servlet.http.*;
import javax.faces.context.*;

public class SearchController {

   private SearchBean getSearchBean(){
        SearchBean sb = null;
        Object obj = FacesContext.getCurrentInstance().getApplication()
        .createValueBinding("#{searchBean}")
        .getValue(FacesContext.getCurrentInstance());
        if(obj != null){
            sb = (SearchBean)obj;
        }
        return sb;
    }



    public String studySearch() {

        //and remove attribute keyword above also
        //ExternalContext context = FacesContext.getCurrentInstance().getExternalContext();
        //HttpServletRequest request = (HttpServletRequest)context.getRequest();
        //String keyword = request.getParameter("form1:keyword");

        //get hold of form bean
        SearchBean sb = getSearchBean() ;

        if (sb != null && sb.getKeyword().compareTo("bad") == 0) { 
            return("bad");
        } else {
            return("success");
        }
    }
}


