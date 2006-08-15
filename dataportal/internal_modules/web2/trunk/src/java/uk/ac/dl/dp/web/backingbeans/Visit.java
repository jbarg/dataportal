/*
 * Visit.java
 *
 * Created on 08 August 2006, 09:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import java.io.Serializable;
import uk.ac.cclrc.dpal.beans.DataSet;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.beans.Study;
import uk.ac.dl.dp.coreutil.clients.dto.SessionDTO;
import uk.ac.dl.dp.coreutil.entity.User;
import uk.ac.dl.dp.coreutil.util.DPRole;
import java.util.*;

/**
 *
 * @author gjd37
 */
public class Visit implements Serializable {
    
    private String dn;
    private Collection<DPRole> roles;
    private String sid;
    private boolean isAdmin = false;
    private AuthorisationBean authBean;
    
    private SessionDTO session;
    
    private Collection<Study> currentStudies;
    
    private Collection<Investigation> currentInvestigations;    
    
    private Collection<DataSet> currentDatasets;
    
    public Visit(){        
    }
    
    public SessionDTO getSession() {
        return session;
    }
    
    public void setSession(SessionDTO session) {
        this.session = session;
        this.sid = session.getUserSessionId();
        this.dn = session.getDN();
        this.roles = session.getRoles();
        
        for(DPRole role : roles){
            if(role.ADMIN == DPRole.ADMIN) {
                isAdmin = true;
                break;
            }
        }
    }
    
    public boolean isAdmin(){
       return isIsAdmin();
    }
    
    public Collection<DPRole> getRoles(){
        return this.roles;
    }

    public AuthorisationBean getAuthorisationBean() {
        return getAuthBean();
    }

    public void setAuthorisationBean(AuthorisationBean authBean) {
        this.authBean = authBean;
    }

    public String getDn() {
        return dn;
    }

    public String getSid() {
        return sid;
    }

    public boolean isIsAdmin() {
        return isAdmin;
    }

    public AuthorisationBean getAuthBean() {
        return authBean;
    }

    public Collection<Study> getCurrentStudies() {
        return currentStudies;
    }

    public void setCurrentStudies(Collection<Study> currentStudies) {
        this.currentStudies = currentStudies;
    }

    public Collection<Investigation> getCurrentInvestigations() {
        return currentInvestigations;
    }

    public void setCurrentInvestigations(Collection<Investigation> currentInvestigations) {
        this.currentInvestigations = currentInvestigations;
    }

    public Collection<DataSet> getCurrentDatasets() {
        return currentDatasets;
    }

    public void setCurrentDatasets(Collection<DataSet> currentDatasets) {
        this.currentDatasets = currentDatasets;
    }
    
}
