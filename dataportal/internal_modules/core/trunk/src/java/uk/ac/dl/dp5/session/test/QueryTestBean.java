/*
 * QueryTestBean.java
 *
 * Created on 01 August 2006, 08:45
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.session.test;


import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import javax.annotation.Resource;
import javax.ejb.EJB;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import org.apache.log4j.Logger;
import javax.ejb.*;
import uk.ac.dl.dp5.exceptions.QueryException;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.sessionbeans.query.QuerySlaveMasterRemote;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;
import uk.ac.dl.dp5.util.DataPortalConstants;

/**
 *
 * @author gjd37
 */
@Stateless(mappedName="QueryTestEJB")
public class QueryTestBean extends SessionEJBObject implements QueryTestRemote {
    
    static Logger log = Logger.getLogger(QueryTestBean.class);
    
  
    private QuerySlaveMasterRemote queryBean;
    private @Resource SessionContext sessionCtx;
    
    public double doQuery(String sid, Collection<String> facilities, String[] keyword) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException,QueryException{
        
            try {
                
                queryBean = (QuerySlaveMasterRemote) new InitialContext().lookup("QuerySlaveMasterEJB");
            } catch (NamingException ex) {
                ex.printStackTrace();
            }
        
        
        double time = new Date().getTime();
        log.debug("doQuery()");
        String query_id =  queryBean.queryByKeyword(sid,facilities,keyword);
        log.trace("queried: "+query_id );
        while(!queryBean.isFinished()){
            
            Collection<String> com = queryBean.getCompleted();
            if(com != null){
                for(String g : com){
                    log.trace("Got results from "+g);
                }
            }
            try {
                Thread.sleep(100);
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
        }
        queryBean.remove();
        return (new Date().getTime()-time)/1000;
    }
    
}
