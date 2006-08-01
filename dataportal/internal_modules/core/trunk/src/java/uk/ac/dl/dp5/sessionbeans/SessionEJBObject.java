/*
 * SessionEJBObject.java
 *
 * Created on 20 June 2006, 11:43
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.sessionbeans.session;

import java.io.File;
import javax.annotation.PostConstruct;
import javax.annotation.security.PermitAll;
import javax.interceptor.AroundInvoke;
import javax.interceptor.Interceptors;
import javax.interceptor.InvocationContext;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.dl.dp5.sessionbeans.EJBObject;


/**
 *
 * @author gjd37
 */
//@Interceptors (LoggerInvoker.class)

@PermitAll
public abstract class SessionEJBObject extends EJBObject{
       
   //this is because Message driven beans cannot have PermitAll anaotation 
}
