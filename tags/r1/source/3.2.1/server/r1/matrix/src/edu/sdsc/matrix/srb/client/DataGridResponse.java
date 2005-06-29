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
 * DataGridResponse.java
 *
 * Created on July 9, 2003, 7:57 AM
 */

package edu.sdsc.matrix.srb.client;
import java.util.*;

/**
 * Represents a DGL response sent back by a matrix server.
 * Communication to a Matrix server is done through sending DataGridRequests, 
 * and receiving a DataGridResponse, which continues back and forth.<br/><br/>
 *
 * A DataGridResponse will be returned by a matrix server after a DataGridRequest
 * has been sent using MatrixConnection. This DataGridResponse can either contain a TransactionAcknowledgement
 * or a TransactionStatusResponse.<br/><br/>
 *
 * A TransactionAcknowledgement is part of a DataGridResponse that is sent back after
 * an initial DataGridRequest that had a Transaction. This TransactionAcknowledgment
 * contains the transaction ID associated with that request, which is probably what the
 * user is most interested in.<br/><br/>
 *
 * With that transaction ID, the user can create a DataGridRequest that has a
 * TransactionStatusQuery part with the ID, and the server will return another
 * DataGridResponse after being sent with MatrixConnection. This DataGridResponse
 * will contain the current status of the original request, and if appropriate,
 * the actual result/information associated with the request. A user would normally
 * poll the server with the transaction ID at delayed intervals until the status
 * of the transaction says it is done, and then use the result that comes with the
 * DataGridResponse that has the transaction complete.<br/><br/>
 * 
 * Example:<br/>
 * <pre>
 * Operation operation = new Operation(new ListOp(new ListParams(new StdDatagridObject(new Collection(collection)))));
 * Step step = new Step("ListStep", operation);
 * Flow flow = new Flow("sequential", step);
 * Transaction transaction = new Transaction(flow);
                
 * // The actual DataGridRequest to be used
 * DataGridRequest dgRequest = new DataGridRequest(dm, ui, voi, transaction, th);

 * // For storing the result of the listing
 * DataGridResponse dgResponse = null;
                
 * // Set up matrix server connection
 * try {
 *   MatrixConnection mConnect = new MatrixConnection(cdlsession.getMatrixServer());
 *                  
 *   // Send and get transaction ID for the Listing
 *   DataGridResponse listResult = mConnect.sendRequest(dgRequest);
 *   String tid = listResult.getTransactionID();
 *                   
 *    // Get the actual Listing with Transaction ID
 *    TransactionStatusQuery tsq = new TransactionStatusQuery(tid);
 *    dgRequest = new DataGridRequest(dm, ui, voi, tsq, th);
 *
 *    // Keep trying retrieving listing for as many times as needed
 *    for(int i = 0; i < 20; i++) {
 *        dgResponse = mConnect.sendRequest(dgRequest);
 *        Thread.sleep(1000);
 *
 *       // Get the TransactionStatusResponse
 *       TransactionStatusResponse tsResponse = (TransactionStatusResponse)dgResponse.getTransactionStatusResponse();
 *
 *       // If list not finished wait before trying again or if limit reached, throw exception
 *       if(tsResponse.getStatusCode() != edu.sdsc.matrix.srb.core.MatrixCodes.TRANSACTION_COMPLETE && i == (19) ) {
 *            throw new Exception("Server did not respond in time");
 *        } 
 *        // Matrix does not have the listing yet
 *        else if(tsResponse.getStatusCode() != edu.sdsc.matrix.srb.core.MatrixCodes.TRANSACTION_COMPLETE) {
 *           continue;
 *        } 
 *        else
 *          // DONE, result is in dgResponse
 *          break;
 *  }
 *
 * ...
 *                      
 * </pre>
 *
 * @author  Allen Ding - SDSC
 */
public class DataGridResponse extends edu.sdsc.matrix.srb.parser.impl.DataGridResponseImpl {

    /**
     * Creates a DataGridResponse from a DocMeta, and a TransactionAcknowledgement.
     * Normally a DataGridResponse would not be created, instead it is retrieved, but 
     * this constructor is here for completeness.
     * @param docMeta The DocMeta to use
     * @param transactionAck The TransactionAcknowledgement to use.
     */
    public DataGridResponse(DocMeta docMeta, TransactionAcknowledgment transactionAck) {
        super.setDocMeta(docMeta);
        super.setTransactionAcknowledgement(transactionAck);
    }
    
    /**
     * Creates a DataGridResponse from a DocMeta, and a TransactionStatusResponse.
     * Normally a DataGridResponse would not be created, instead it is retrieved, but 
     * this constructor is here for completeness.
     * @param docMeta The DocMeta to use.
     * @param transactionRes The TransactionStatusResponse to use.
     */
    public DataGridResponse(DocMeta docMeta, TransactionStatusResponse transactionRes) {
        super.setDocMeta(docMeta);
        super.setTransactionStatusResponse(transactionRes);
    }
    
    /** 
     * Gets the transaction id of the transaction acknowledgment if it exists.
     * @return The transaction ID. null if a TransactionAcknowledgment does not
     * exist.
     */    
    public String getTransactionID() {
        if(super.getTransactionAcknowledgement() != null)
            return super.getTransactionAcknowledgement().getTransactionID();
        else
            return null;
    }

    /** 
     * Gets the request state of the transaction acknowledgement if it exists.
     * @return The request state of the TransactionAcknowledgment. -1 if a
     * TransactionAcknowledgment does not exist.
     */
    public int getRequestState() {
        if(super.getTransactionAcknowledgement() != null)
            return super.getTransactionAcknowledgement().getRequestState();
        else
            return -1;
    }

    /**
     * Protected method that allows other classes in the package to create
     * a DataGridResponse representing an existing JAXB interface DataGridResponse.
     * The client should not use JAXB generated interfaces directly.
     * @param dgr The existing parser.DataGridResponse to wrap.
     */    
    protected DataGridResponse(edu.sdsc.matrix.srb.parser.DataGridResponse dgr) {

        super.setDocMeta(new DocMeta(dgr.getDocMeta()));

        if(dgr.getTransactionAcknowledgement() != null)
            super.setTransactionAcknowledgement(dgr.getTransactionAcknowledgement());
        if(dgr.getTransactionStatusResponse() != null)
            super.setTransactionStatusResponse(new TransactionStatusResponse(dgr.getTransactionStatusResponse()));
    }
    
}