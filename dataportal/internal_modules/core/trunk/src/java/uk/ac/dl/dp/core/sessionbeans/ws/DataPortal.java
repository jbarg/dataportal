/*
 * DataPortal.java
 *
 * Created on 11 June 2007, 15:43
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans.ws;


import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.ejb.TransactionAttribute;
import javax.ejb.TransactionAttributeType;
import javax.interceptor.ExcludeClassInterceptors;
import javax.interceptor.ExcludeDefaultInterceptors;
import javax.interceptor.Interceptors;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import org.apache.log4j.Logger;
import uk.ac.dl.dp.core.sessionbeans.ArgumentValidator;

import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.clients.dto.UserPreferencesDTO;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.exceptions.DataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.LoginMyProxyException;
import uk.ac.dl.dp.coreutil.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.interfaces.DataCenterLocal;
import uk.ac.dl.dp.coreutil.interfaces.QueryLocal;
import uk.ac.dl.dp.coreutil.interfaces.SessionLocal;
import uk.ac.dl.dp.coreutil.util.KeywordQueryRequest;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp.coreutil.util.SessionUtil;
import uk.ac.dp.icatws.AdvancedSearchDetails;
import uk.ac.dp.icatws.Investigation;
import uk.ac.dp.icatws.InvestigationInclude;

/**
 *
 * @author gjd37
 */

@Stateless()
@WebService(/*targetNamespace="client.dataportal.uk"*/)
@Interceptors(ArgumentValidator.class)
@TransactionAttribute(TransactionAttributeType.NOT_SUPPORTED)
public class DataPortal extends SessionEJBObject {
    
    static Logger log = Logger.getLogger(DataPortal.class);
    
    ///////////////////////  Inject all the EJBs   //////////////////////////
    @EJB
    protected SessionLocal sessionBeanLocal;
    
    @EJB
    protected QueryLocal queryLocal;
    
    @EJB
    protected DataCenterLocal dataCenterLocal;
    //////////////////////////////////////////////////////////////////////////
    
    @ExcludeClassInterceptors
    @WebMethod()
    public String login(@WebParam(name="username") String username, @WebParam(name="password") String password, @WebParam(name="lifetime") int lifetime) throws SessionException, LoginMyProxyException  {
        try {
            return sessionBeanLocal.login(username, password, lifetime);
        } catch (LoginMyProxyException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public boolean isValid(@WebParam(name="sessionId") String sessionId) throws SessionException  {
        try {
            return sessionBeanLocal.isValid(sessionId);
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
        
    }
    
    @WebMethod()
    public SessionDTO getSession(@WebParam(name="sessionId") String sessionId) throws SessionException  {
        try {
            return sessionBeanLocal.getSession(sessionId);
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public boolean logout(@WebParam(name="sessionId") String sessionId) throws SessionException{
        try {
            return sessionBeanLocal.logout(sessionId);
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    //@WebMethod() //HashMaps dont work for JAXWS
    public HashMap<String, Collection<String>> getKeywords(@WebParam(name="sessionId") String sessionId, @WebParam(name="facility") String facility) throws QueryException , SessionException {
        try {
            //check session
            new SessionUtil(sessionId, em);
            return queryLocal.getKeywords(sessionId);
        } catch (QueryException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public Collection<String> getKeywordsForFacility(@WebParam(name="sessionId") String sessionId, @WebParam(name="facility") String facility) throws QueryException , SessionException {
        try {
            //check session
            new SessionUtil(sessionId, em);
            return queryLocal.getKeywordsForFacility(sessionId, facility);
        } catch (QueryException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public QueryRequest queryMyData(@WebParam(name="sessionId") String sessionId, @WebParam(name="facilities") HashSet<String> facilities ) throws QueryException, SessionException {
        try {
            return queryLocal.queryMydata(sessionId, facilities);
        } catch (QueryException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public QueryRequest queryInvestigations(@WebParam(name="sessionId") String sessionId, @WebParam(name="investigations") Collection<Investigation> investigations, @WebParam(name="include") InvestigationInclude include) throws SessionException, QueryException{
        try {
            return queryLocal.queryInvestigations(sessionId, investigations, include);
        } catch (QueryException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public QueryRequest queryByKeywords(@WebParam(name="sessionId") String sessionId, @WebParam(name="keywordQueryRequest") KeywordQueryRequest keywordQueryRequest) throws QueryException, SessionException {
        try {
            return queryLocal.queryByKeyword(sessionId, keywordQueryRequest);
        } catch (QueryException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    
    @WebMethod()
    public QueryRequest queryByAdvanced(@WebParam(name="sessionId") String sessionId, @WebParam(name="advancedSearchDetails") AdvancedSearchDetails advancedSearchDetails, @WebParam(name="facilities") HashSet<String> facilities ) throws QueryException, SessionException {
        try {
            return queryLocal.queryAdvanced(sessionId, advancedSearchDetails, facilities);
        } catch (QueryException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public Collection<Investigation> queryKeywordsAndWait(@WebParam(name="sessionId") String sessionId, @WebParam(name="keywordQueryRequest") KeywordQueryRequest keywordQueryRequest) throws QueryException, SessionException {
        try {
            QueryRequest request = queryLocal.queryByKeyword(sessionId, keywordQueryRequest);
            
            while(!queryLocal.isFinished(request)){
                try {
                    Thread.sleep(1000); //sleep one second
                } catch (InterruptedException ex) {
                } //sleep one second
            }
            
            return queryLocal.getQueryResults(request);
        } catch (QueryException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    @ExcludeClassInterceptors
    @ExcludeDefaultInterceptors
    public boolean isFinished(@WebParam(name="q_request") QueryRequest q_request) throws SessionException {
        return queryLocal.isFinished(q_request);
    }
    
    @WebMethod()
    public Collection<Investigation> getQueryResults(@WebParam(name="q_request") QueryRequest q_request) throws SessionException{
        return queryLocal.getQueryResults(q_request);
    }
    
    @WebMethod()
    public Collection<Investigation> getQueryResultsPagination(@WebParam(name="q_request") QueryRequest q_request, @WebParam(name="startIndex") int startIndex, @WebParam(name="numberResults") int numberResults)throws SessionException {
        Collection<Investigation> investigations = queryLocal.getQueryResults(q_request);
        
        Collection<Investigation> investigationsToReturn = new ArrayList<Investigation>();
        Investigation[] array = investigations.toArray(new Investigation[investigations.size()]);
        for (int i = startIndex; i < startIndex+numberResults; i++) {
            if(i >= investigations.size()) break;
            investigationsToReturn.add(array[i]);
        }
        
        return investigationsToReturn;
    }
    
    @WebMethod()
    public Collection<DataReference> getDataReferences(@WebParam(name="sessionId") String sessionId) throws DataCenterException, SessionException{
        try {
            return dataCenterLocal.getDataReferences(sessionId);
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public Collection<Bookmark> getBookmarks(@WebParam(name="sessionId") String sessionId) throws SessionException{
        try {
            return dataCenterLocal.getBookmarks(sessionId);
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public void removeBookmark(@WebParam(name="sessionId") String sessionId, @WebParam(name="bookmark") Bookmark bookmark) throws DataCenterException, SessionException{
        try {
            dataCenterLocal.removeBookmark(sessionId, bookmark);
        } catch (DataCenterException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public void removeDataReference(@WebParam(name="sessionId") String sessionId, @WebParam(name="dataReference") DataReference dataReference) throws DataCenterException, SessionException{
        try {
            dataCenterLocal.removeDataReference(sessionId, dataReference);
        } catch (DataCenterException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public void modifyDataReference(@WebParam(name="sessionId") String sessionId, @WebParam(name="dataReference") DataReference dataReference) throws DataCenterException, SessionException{
        try {
            dataCenterLocal.modifyDataReference(sessionId, dataReference);
        } catch (DataCenterException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public void modifyBookmark(@WebParam(name="sessionId") String sessionId, @WebParam(name="bookmark") Bookmark bookmark) throws DataCenterException, SessionException{
        try {
            dataCenterLocal.modifyBookmark(sessionId, bookmark);
        } catch (DataCenterException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public void addDataReference(@WebParam(name="sessionId") String sessionId, @WebParam(name="dataReference") DataReference dataReference) throws DataCenterException, SessionException{
        try {
            dataCenterLocal.addDataReference(sessionId, dataReference);
        } catch (DataCenterException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public void addBookmark(@WebParam(name="sessionId") String sessionId, @WebParam(name="bookmark") Bookmark bookmark) throws DataCenterException, SessionException{
        try {
            dataCenterLocal.addBookmark(sessionId, bookmark);
        } catch (DataCenterException ex) {
            throw ex;
        } catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public UserPreferencesDTO getUserPreferences(@WebParam(name="sessionId") String sessionId) throws SessionException{
        try {
            return sessionBeanLocal.getUserPrefs(sessionId);
        }catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
    
    @WebMethod()
    public void setUserPreferences(@WebParam(name="sessionId") String sessionId, @WebParam(name="userPreferencesDTO") UserPreferencesDTO userPreferencesDTO) throws SessionException{
        try {
            sessionBeanLocal.setUserPrefs(sessionId, userPreferencesDTO);
        }catch (SessionException ex) {
            throw new SessionException(ex.getMessage());
        }
    }
}
