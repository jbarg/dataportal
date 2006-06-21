package uk.ac.cclrc.webinterface.bean ;

//servlet(container stuff) + jsf(framework stuff)
import javax.servlet.*;
import javax.servlet.http.*;
import javax.faces.context.*;

public class SearchController {

    public String studySearch() {
        ExternalContext context = FacesContext.getCurrentInstance().getExternalContext();
        HttpServletRequest request = (HttpServletRequest)context.getRequest();
        String keyword = request.getParameter("form1:keyword");

        if (keyword.compareTo("bad") == 0) { 
            return("bad");
        } else {
            return("success");
        }
    }
}


