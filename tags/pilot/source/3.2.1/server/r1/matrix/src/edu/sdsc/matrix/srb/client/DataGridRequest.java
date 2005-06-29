/* ====================================================================
 * SDSC Matrix Software License, Version 1.0
 *
 * Copyright (c) 2003 San Diego Supercomputer Center (SDSC), San Diego, USA.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that ALL the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        SDSC Matrix Project (http://www.npaci.edu/DICE/SRB/matrix/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "SDSC" and "SDSC Matrix" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact arun@sdsc.edu or srb-chat@sdsc.edu.
 *
 * 5. Products derived from this software may not be called "SDSC",
 *    nor may "SDSC" appear in their name, without prior written
 *    permission of the SDSC.
 *
 * 6. License is not granted for commercial resale, in whole or in part, 
 *	  without the prior written permission from SDSC.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL SDSC OR
 * ITS CONTRIBUTORS (SDSC Matrix Team) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software is intended to consist of voluntary contributions made by many
 * individuals for the SDSC Matrix Project.
 *
 * SDSC Matrix.  
 * Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
 *
 * Authors (Credits 02/SEP/2003):
 * Allen Ding (alding@sdsc.edu)
 * Arun Jagatheesan (arun@sdsc.edu)
 * Reena Mathew	(rmathew@sdsc.edu)
 * Erik Vandekieft (evk@sdsc.edu)
 */


/*
 * DataGridRequest.java
 *
 * Created on July 2, 2003, 5:53 AM
 */

package edu.sdsc.matrix.srb.client;

import java.util.*;

/**
 * Represents a DGL request to be sent to a matrix server. 
 * Communication to a Matrix server is done through sending DataGridRequests, 
 * and receiving a DataGridResponse, which continues back and forth.<br/><br/>
 *
 * A DataGridResponse is consisted of 2 parts:<br/>
 * <ul>
 * <li>DocMeta</li>
 * <li>TransactionAcknowledgement OR TransactionStatusResponse (not both)</li>
 * </ul>
 * <br/>
 *
 * When a 
 *
 * A Transaction is used to make an initial operation on the data grid - for example:
 * creating datasets, getting listings, ingesting files, downloading files, etc.
 * One a DataGridRequest has been sent, the server will return a DataGridResponse with
 * a transaction ID associated with the original DataGridRequest.<br/><br/>
 *
 * A new DataGridRequest can now be made to query the status of the Transaction ID
 * (the server will return another DataGridResponse with the TransactionStatusResponse field 
 * filled). Now the DataGridRequest will have its TransactionStatusQuery part set, and not
 * the Transaction part.<br/><br/>
 *
 * The DataGridResponse returned with this new request
 * contains the status of the transaction, and possibly also
 * the result/return information for the DataGridRequest if the Transaction is complete,
 * and the original DataGridRequest warrants such results.<br/><br/>
 *
 * Non optional-parameter changes to the object should be made using 
 * useXXX methods that are available. A bad configuration of set fields
 * may result in an invalid content tree.
 * @author  Allen Ding - SDSC
 */
public class DataGridRequest extends edu.sdsc.matrix.srb.parser.impl.DataGridRequestImpl {

    /**
     * Just in case a user needs to create an empty DataGridRequest. Be sure to set its parts
     *  before using it!
     */
    public DataGridRequest() {
        super();
    }

    /**
     * Creates a DataGridRequest with the specified docMeta, gridUser,
     * vOInfo, faultHandler, and using a transaction. This is normally used
     * for sending actual operations to the data grid.
     * @param docMeta The document meta info.
     * @param gridUser The grid user info.
     * @param vOInfo The virtual organization info.
     * @param transaction The transaction to perform.
     * @param tHandler A specified handler during termination.
     */
    public DataGridRequest(DocMeta docMeta,
                           UserInfo gridUser,
                           VirtualOrganizationInfo vOInfo,
                           Transaction transaction,
                           TerminationHandler tHandler) {
        super.setDocMeta(docMeta);
        super.setGridUser(gridUser);
        super.setVOInfo(vOInfo);
        super.setTransaction(transaction);
        super.setTerminationHandler(tHandler);
    }

    /**
     * Creates a DataGridRequest with the specified docMeta, gridUser,
     * vOInfo, faultHandler, and using a transactionStatusQuery.
     * This is normally used when a user has a Transaction ID and wants
     * to query/get the result of a previous DataGridRequest.
     * @param docMeta The document meta info.
     * @param gridUser The grid user info.
     * @param vOInfo The virtual organization info.
     * @param transactionStatusQuery The transactionStatusQuery.
     * @param tHandler A specified handler during termination.
     */    
    public DataGridRequest(DocMeta docMeta,
                           UserInfo gridUser,
                           VirtualOrganizationInfo vOInfo,
                           TransactionStatusQuery transactionStatusQuery,
                           TerminationHandler tHandler) {
        super.setDocMeta(docMeta);
        super.setGridUser(gridUser);
        super.setVOInfo(vOInfo);
        super.setTransactionStatusQuery(transactionStatusQuery);
        super.setTerminationHandler(tHandler);
    }

    /**
     * Makes the DataGridRequest use the specified transaction.
     * Using this method clobbers any transactionStatusQuery information.
     * @param transaction The transaction to perform.
     */
    public void useTransaction(Transaction transaction) {
        super.setTransactionStatusQuery(null);
        super.setTransaction(transaction);
    }

    /**
     * Makes the DataGridRequest use the specified transactionStatusQuery.
     * Using this method clobbers any transaction information.
     * @param transactionStatusQuery The transactionStatusQuery to perform.
     */    
    public void useTransactionStatusQuery(TransactionStatusQuery transactionStatusQuery) {
        super.setTransaction(null);
        super.setTransactionStatusQuery(transactionStatusQuery);
    }
    
    /**
     * Method for other classes in package to get a single level
     * ArrayList accumulation of SOAP AttachmentPart's from any
     * Transaction that this DataGridRequest contains. This method is used internally
     * and should not be used normally.
     */
    protected ArrayList getAttachments() {
        if(super.getTransaction() != null)
            return ((Transaction)super.getTransaction()).getAttachments();
        else
            return null;
    }
    
}