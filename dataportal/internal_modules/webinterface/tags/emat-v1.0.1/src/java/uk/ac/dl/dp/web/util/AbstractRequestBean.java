/*
 * AbstractRequestBean.java
 *
 * Created on 16 August 2006, 09:17
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.util;

/**
 *
 * @author djm76
 */
/**
 * <p><strong>AbstractRequestBean</strong> is the abstract base class for
 * data bean(s) that are stored in request scope attributes.  It extends
 * {@link FacesBean}, so it inherits all of the default behavior
 * found there.  In addition, the following lifecycle methods are called
 * automatically when the corresponding events occur:</p>
 * <ul>
 * <li><code>init()</code> - Called when this bean is initially added as a
 *     request scope attribute (typically as the result of
 *     evaluating a value binding or method binding expression).</li>
 * <li><code>destroy()</code> - Called when the bean is removed from the
 *     request attributes (typically as a result of the application
 *     being shut down by the servlet container).</li>
 * </ul>
 */
public abstract class AbstractRequestBean extends FacesBean {
    

    // ------------------------------------------------------------- Constructor


    /**
     * <p>Create a new request scope bean.</p>
     */
    public AbstractRequestBean() {      
    }
    

    // ------------------------------------------------------- Lifecycle Methods


    /**
     * <p>This method is called when this bean is initially added to
     * request scope.  Typically, this occurs as a result of evaluating
     * a value binding or method binding expression, which utilizes the
     * managed bean facility to instantiate this bean and store it into
     * request scope.</p>
     *
     * <p>You may customize this method to allocate resources that are required
     * for the lifetime of the current request.</p>
     */
    public void init() {

        // The default implementation does nothing

    }


    /**
     * <p>This method is called when this bean is removed from
     * request scope.  This occurs automatically when the corresponding
     * HTTP response has been completed and sent to the client.</p>
     *
     * <p>You may customize this method to clean up resources allocated
     * during the execution of the <code>init()</code> method, or
     * at any later time during the lifetime of the request.</p>
     */
    public void destroy() {

        // The default implementation does nothing

    }


}
