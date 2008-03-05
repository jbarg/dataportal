/*
 * SessionEJBObject.java
 *
 * Created on 20 June 2006, 11:43
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package uk.ac.dl.dp.core.message;

import org.apache.log4j.Logger;
import uk.ac.dl.dp.core.sessionbeans.SessionEJBObject;
import uk.ac.dl.dp.core.sessionbeans.EJBObject;

/**
 *
 * @author gjd37
 */
//@Interceptors (LoggerInvoker.class)
public abstract class MessageEJBObject extends EJBObject {

    static Logger log = Logger.getLogger(SessionEJBObject.class);
}
