/*
 * DataPortal.java
 *
 * Created on 11 June 2007, 15:43
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans.ws;


import java.util.Collection;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.ejb.TransactionAttribute;
import javax.ejb.TransactionAttributeType;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import org.apache.log4j.Logger;
import uk.ac.cclrc.dpal.beans.DataFile;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.enums.LogicalOperator;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.exceptions.CannotCreateNewUserException;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionNotFoundException;
import uk.ac.dl.dp.coreutil.exceptions.SessionTimedOutException;
import uk.ac.dl.dp.coreutil.exceptions.UserNotFoundException;
import uk.ac.dl.dp.coreutil.interfaces.QueryLocal;
import uk.ac.dl.dp.coreutil.interfaces.SessionLocal;
import uk.ac.dl.dp.coreutil.util.DPQueryType;
import uk.ac.dl.dp.coreutil.util.QueryRequest;

/**
 *
 * @author gjd37
 */

@Stateless()
@WebService(/*targetNamespace="client.dataportal.uk"*/)
@TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
public class DataPortal extends SessionEJBObject {
    
    static Logger log = Logger.getLogger(DataPortal.class);
    
    ///////////////////////  Inject all the EJBs   //////////////////////////
    @EJB
    protected SessionLocal sessionBeanLocal;
    
    @EJB
    protected QueryLocal queryLocal;
    //////////////////////////////////////////////////////////////////////////
    
    
    @WebMethod()
    public String login(@WebParam(name="username") String username, @WebParam(name="password") String password, @WebParam(name="lifetime") int lifetime) throws CannotCreateNewUserException, LoginMyProxyException  {
        return sessionBeanLocal.login(username, password, lifetime);
    }
    
    @WebMethod()
    public boolean isValid(@WebParam(name="sessionId") String sessionId) throws SessionNotFoundException, SessionTimedOutException  {
        return sessionBeanLocal.isValid(sessionId);
    }
    
    @WebMethod()
    public SessionDTO getSession(@WebParam(name="sessionId") String sessionId) throws SessionNotFoundException, SessionTimedOutException, UserNotFoundException  {
        return sessionBeanLocal.getSession(sessionId);
    }
    
    @WebMethod()
    public boolean logout(@WebParam(name="sessionId") String sessionId) throws SessionNotFoundException, SessionTimedOutException, UserNotFoundException {
        return sessionBeanLocal.logout(sessionId);
    }
    
    @WebMethod()
    public String[] getKeywords(@WebParam(name="sessionId") String sessionId, String facility) throws Exception  {
        return queryLocal.getKeywords(facility);
    }
    
    @WebMethod()
    public QueryRequest query(String sid, Collection<String> facilities, String[] keyword, LogicalOperator logicalex, boolean fuzzy) throws QueryException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException {
        return queryLocal.query(sid, facilities, keyword, logicalex, fuzzy, DPQueryType.KEYWORD);
    }
    
    @WebMethod()
    public Collection<Investigation> queryAndWait(String sid, Collection<String> facilities, String[] keyword, LogicalOperator logicalex, boolean fuzzy) throws QueryException, SessionNotFoundException, UserNotFoundException, SessionTimedOutException {
        QueryRequest request = queryLocal.query(sid, facilities, keyword, logicalex, fuzzy, DPQueryType.KEYWORD);
        
        while(!queryLocal.isFinished(request)){
            try {
                Thread.sleep(1000); //sleep one second
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            } //sleep one second
        }
        
        return queryLocal.getQueryResults(request);
    }
    
    @WebMethod()
    public boolean isFinished(QueryRequest q_request){
        return queryLocal.isFinished(q_request);
    }
    
    @WebMethod()
    public Collection<DataSet> getDataSets(String sid, Collection<Investigation> investigations) throws SessionNotFoundException, SessionTimedOutException, UserNotFoundException, QueryException {
        return queryLocal.getDataSets(sid, investigations);
    }
    
    @WebMethod()
    public Collection<DataFile> getDataFiles(String sid, Collection<DataSet> datasets) throws SessionNotFoundException, SessionTimedOutException, UserNotFoundException, QueryException {
        return queryLocal.getDataFiles(sid, datasets);
    }
    
     @WebMethod()
    public Collection<Investigation> getQueryResults(QueryRequest q_request){
        return queryLocal.getQueryResults(q_request);
    }
}
