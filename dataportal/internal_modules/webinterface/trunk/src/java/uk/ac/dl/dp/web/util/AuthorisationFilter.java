/*
 * AuthorisationFilter.java
 *
 * Created on 22 August 2006, 15:15
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.util;
import java.io.IOException;
import javax.servlet.http.*;
import javax.servlet.*;
import org.omg.PortableServer.REQUEST_PROCESSING_POLICY_ID;
import uk.ac.dl.dp.web.backingbeans.Visit;
/**
 *
 * @author gjd37
 */
public class AuthorisationFilter implements Filter{
    
    FilterConfig config = null;
    ServletContext servletContext = null;
    
    /** Creates a new instance of AuthorisationFilter */
    public AuthorisationFilter() {
    }
    
    public void init(FilterConfig filterConfig) throws ServletException{
        config = filterConfig;
    }
    
    public void doFilter(ServletRequest request, ServletResponse response, FilterChain chain) throws IOException, ServletException{
        
        HttpServletRequest httpRequest = (HttpServletRequest)request;
        HttpServletResponse httpResponse = (HttpServletResponse)response;
        HttpSession session = httpRequest.getSession();
        
        Visit visit = (Visit)session.getAttribute(WebConstants.SESSION_KEY);
        
        if(visit == null){            
            if(httpRequest.getPathInfo().endsWith("index.jsp")){
                chain.doFilter(request,response);
            } else{
                httpResponse.sendRedirect(httpRequest.getContextPath()+"/index.jsp");
            }
        } else{
            chain.doFilter(request,response);
        }
    }
    
    public void destroy(){
        
    }
}