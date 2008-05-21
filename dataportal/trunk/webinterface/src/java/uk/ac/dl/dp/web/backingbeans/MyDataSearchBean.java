/*
 * SearchBean.java
 *
 * Created on 08 August 2006, 14:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;

import org.apache.log4j.*;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.web.util.AbstractRequestBean;

/**
 *
 * @author gjd37
 */
public class MyDataSearchBean extends AbstractRequestBean {
    
    private static Logger log = Logger.getLogger(MyDataSearchBean.class);
           
    public String myDataSearchForKerberos() throws SessionException{
        log.trace("myDataSearchForKerberos");
        SearchBean searchBean = new SearchBean();
        
        return searchBean.searchOwnDataAll();        
    }
    
}