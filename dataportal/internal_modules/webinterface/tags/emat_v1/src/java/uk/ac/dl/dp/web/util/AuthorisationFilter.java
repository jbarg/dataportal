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
import org.apache.log4j.*;
import uk.ac.dl.dp.web.backingbeans.Visit;

/**
 *
 * @author gjd37
 */
public class AuthorisationFilter implements Filter{
    
    private static Logger log = Logger.getLogger(AuthorisationFilter.class);
    
    
    FilterConfig config = null;
    ServletContext servletContext = null;
    
    /** Creates a new instance of AuthorisationFilter */
    public AuthorisationFilter() {
    }
    
    public void init(FilterConfig filterConfig) throws ServletException{
        config = filterConfig;
    }
    
    public void doFilter(ServletRequest request, ServletResponse response, FilterChain chain) throws IOException, ServletException{
        log.trace("do filter");
        HttpServletRequest httpRequest = (HttpServletRequest)request;
        HttpServletResponse httpResponse = (HttpServletResponse)response;
        HttpSession session = httpRequest.getSession(false);
        if(session == null){
            httpResponse.sendRedirect(httpRequest.getContextPath()+"/index.jsp");
            log.trace("session null, sending to logon");
        }else{
            
            Visit visit = (Visit)session.getAttribute(WebConstants.SESSION_KEY);
            
            if(visit == null || visit.getDn() == null){
                httpResponse.sendRedirect(httpRequest.getContextPath()+"/index.jsp");
                log.trace("sending to logon, no visit");
                
            } else{
                log.trace("Everything ok: "+visit.getDn());
                chain.doFilter(request,response);
            }
        }
        
    }
    
    
    public void destroy(){
        
    }
}
