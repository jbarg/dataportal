/*
	* 	SDSC Matrix. A Gridflow Management System for data grids and digtal libraries
	*  Copyright (C) 2004 SDSC Matrix Project
	*
	* This library is free software; you can redistribute it and/or
	* modify it under the terms of the GNU Lesser General Public
	* License as published by the Free Software Foundation; either
	* version 2.1 of the License, or (at your option) any later version.
	*
	* This library is distributed in the hope that it will be useful,
	*	but WITHOUT ANY WARRANTY; without even the implied warranty of
	*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	*	Lesser General Public License for more details.
	*
	*	You should have received a copy of the GNU Lesser General Public
	*	License along with this library; if not, write to the Free Software
	*	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	*
	* ====================================================================
	*
	* This software consists of voluntary contributions from the developers of the SDSC
	* Matrix Project. We request that all redistribution and software made using this code
	* acknowledge their use of the “SDSC Matrix project” in their end-user documentation or
	* in their website.
	*
	* SDSC Matrix Project (list of developers as of Dec 2005)
	*	Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
	* 9500 Gilman Drive, MC0505
	* San Diego Supercomputer Center
	* University of California
	* La Jolla CA 92093
	*
	* Allen Ding (alding@sdsc.edu)
	* Lucas Gilbert (iktome@sdsc.edu)
	* Arun Jagatheesan (arun@sdsc.edu)
	* Reena Mathew	(rmathew@sdsc.edu)
	* Daniel Moore (mixx@umail.ucsb.edu)
	* Erik Vandekieft (evk@sdsc.edu)
	* Jonathan Weinberg (jonw@sdsc.edu)
	*
	*/


/*
	* Developer Log: DataGridResponse.java
	* more getters, Licence, java docs on Dec 6, 2004
	* Modified for DGL 3.0 on Dec, 16 2003 by Arun
	* Created on July 9, 2003, 7:57 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* <p>
	* Represents a DGL response sent back by a matrix server.
	* Communication to a Matrix server is done through sending DataGridRequests,
	* and receiving a DataGridResponse, which continues back and forth.<br/><br/>
	*
	* A DataGridResponse will be returned by a matrix server after a DataGridRequest
	* has been sent using MatrixConnection. This DataGridResponse can either contain a
	* RequestAcknowledgement or a FlowStatusResponse.<br/><br/> </p>
	*
	* <p>
	* When a asynchronous Data Grid Request is sent to the Matrix serser, a
	* RequestAcknowledgement is sent back. This RequestAcknowledgment
	* contains a transactionID representing the request that was sent earlier. With the
	* transaction ID, the user can create a DataGridRequest that has a
	* FlowStatusQuery to query on the status.
	* </p>
	* <p> On the other hand, when a synchronous Data Grid Request is sent, the
	* FlowStatusResponse (FSR) is sent as part of the Data Grd Response </p>
	* <br>
	* @author  Allen Ding, Arun swaran Jagatheesan
	*/
public class DataGridResponse extends edu.sdsc.matrix.srb.parser.impl.
			DataGridResponseImpl {
	/**
		* Creates a DataGridResponse from a DocMeta, and a TransactionAcknowledgement.
		* Normally a DataGridResponse would not be created, instead it is retrieved, but
		* this constructor is here for completeness.
		* @param docMeta The DocMeta to use (optional element, but used in constructor)
		* @param transactionAck The TransactionAcknowledgement to use.
		*/
	public DataGridResponse(DocMeta docMeta, RequestAcknowledgment ack) {
		super.setDocMeta(docMeta); //can be null
		super.setRequestAcknowledgement(ack);
	}

	/**
		* Creates a DataGridResponse from a DocMeta, and a TransactionStatusResponse.
		* Normally a DataGridResponse would not be created, instead it is retrieved, but
		* this constructor is here for completeness.
		* @param docMeta The DocMeta to use. (optional element, but used in constructor)
		* @param transactionRes The TransactionStatusResponse to use.
		*/
	public DataGridResponse(DocMeta docMeta, FlowStatusResponse res) {
		super.setDocMeta(docMeta);
		super.setFlowStatusResponse(res);
	}

	/**
		* Gets the transaction id of the transaction acknowledgment if it exists.
		* @return The transaction ID. null if a TransactionAcknowledgment does not
		* exist.
		*/
	public String getRequestID() {
		if (super.getRequestAcknowledgement() != null) {
			return super.getRequestAcknowledgement().getRequestID();
		} else {
			return null;
		}
	}

	/**
		* Gets the request state of the transaction acknowledgement if it exists. On the other
		* hand, If the acknowledgement does not exist, check if response has
		* FlowStatusResponse (FSR). If FSR exists returns 0 and if not returns -1
		* @return 0 if ok or -1 if problem
		*/
	public int getRequestState() {
		if (super.getRequestAcknowledgement() != null) {
			return super.getRequestAcknowledgement().getRequestState();
		}else if (super.getFlowStatusResponse() != null){
					return 0;
		} else {
			return -1;
		}
	}

	public FlowStatusResponse getFlowResponseByName(String flowName){
				if(this.getFSR()!=null){
					return this.getFSR().getFSRbyName(flowName);
				}else return null;
	}

	/**
		* Protected method that allows other classes in the package to create
		* a DataGridResponse representing an existing JAXB interface DataGridResponse.
		* The client should not use JAXB generated interfaces directly.
		* @param dgr The existing parser.DataGridResponse to wrap.
		*/
	protected DataGridResponse(edu.sdsc.matrix.srb.parser.DataGridResponse dgr) {
		if(dgr.getDocMeta() !=null) super.setDocMeta(new DocMeta(dgr.getDocMeta()));
		if (dgr.getRequestAcknowledgement() != null) {
			super.setRequestAcknowledgement(dgr.getRequestAcknowledgement());
		}
		if (dgr.getFlowStatusResponse() != null) {
			super.setFlowStatusResponse(new FlowStatusResponse(
						dgr.getFlowStatusResponse()));
		}
	}

	public FlowStatusResponse getFSR(){
				if (this.getFlowStatusResponse() != null){
							return new FlowStatusResponse(this.getFlowStatusResponse());
				}else return null;
	}

}
