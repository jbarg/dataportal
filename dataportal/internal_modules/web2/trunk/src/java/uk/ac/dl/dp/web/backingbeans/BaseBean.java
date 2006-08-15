/*
 * BaseBean.java
 *
 * Created on 08 August 2006, 10:00
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import javax.faces.context.FacesContext;
import javax.faces.application.*;
import java.io.*;
/**
 *
 * @author gjd37
 */
public class BaseBean implements Serializable {
    
    
    private Visit visit;
    
    /** Creates a new instance of BaseBean */
    public BaseBean() {
    }
    
    
    //Faces objects
    public FacesContext getFacesContext(){
        return FacesContext.getCurrentInstance();
    }
    
    public Application getApplication(){
        return getFacesContext().getApplication();
    }
    
    //application objects
    public Visit getVisit(){
        return visit;
    }

    public void setVisit(Visit visit) {
        this.visit = visit;
    }
    
}

