/*
 * SessionEJBObject.java
 *
 * Created on 20 June 2006, 11:43
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.sessionbeans;

import javax.annotation.security.PermitAll;
import javax.interceptor.Interceptors;
import uk.ac.dl.dp.core.sessionbeans.EJBObject;


/**
 *
 * @author gjd37
 */
//@Interceptors (LoggerInvoker.class)

@PermitAll

public abstract class SessionEJBObject extends EJBObject{
       
   //this is because Message driven beans cannot have PermitAll anaotation 
}
