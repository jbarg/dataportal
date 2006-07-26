/*
 * QuerySlaveBean.java
 *
 * Created on 30 June 2006, 08:26
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.query;

import javax.ejb.Remove;
import javax.ejb.Stateful;
import org.apache.log4j.Logger;
import uk.ac.cclrc.dpal.DPAccessLayer;
import uk.ac.dl.dp5.exceptions.SessionNotFoundException;
import uk.ac.dl.dp5.exceptions.SessionTimedOutException;
import uk.ac.dl.dp5.exceptions.UserNotFoundException;
import uk.ac.dl.dp5.sessionbeans.session.SessionEJBObject;

/**
 *
 * @author gjd37
 */
@Stateful
public class QuerySlaveBean extends SessionEJBObject implements QuerySlaveLocal {
    
    static Logger log = Logger.getLogger(QuerySlaveBean.class);
    
    //stateful info
    boolean isFinished = false;
    
    public void queryByKeyword(String DN, String facility, String keyword) throws SessionNotFoundException, UserNotFoundException, SessionTimedOutException{
        if(DN == null) throw new IllegalArgumentException("DN cannot be null.");
        //TODO check for nulls
        
        //TODO real query
        try {
          
          
            
            
        } catch (Exception ex) {
            
        }
        isFinished = true;
    }
    
    public boolean isFinished(){
        return isFinished;
    }
    
    @Remove
    public void remove(){
        //TODO remove objects
    }
    
    public Object getQueryResults(){
        return Math.random();
    }
}
