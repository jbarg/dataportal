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
	* DataGridRequest.java
	*
	* Modified by Arun on Dec, 15 2003 for 3.0 DGL
	* Created on July 2, 2003, 5:53 AM
	*/
package edu.sdsc.matrix.srb.client;

import java.util.ArrayList;


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
	* A Flow is used to make an initial operation on the data grid - for example:
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
public class DataGridRequest extends edu.sdsc.matrix.srb.parser.impl.
		DataGridRequestImpl{

			public DataGridRequest(edu.sdsc.matrix.srb.parser.DataGridRequest request){
						if (request.getDocMeta() != null)
									this.setDocMeta(new DocMeta(request.getDocMeta()));
						// set the flow
						if (request.getFlow() != null){
									edu.sdsc.matrix.srb.parser.Flow flow = request.getFlow();
									if (flow.getFlowLogic().getForEachLoop() != null)
												this.setFlow(new ForEachFlow(flow));
									else if (flow.getFlowLogic().getForLoop() != null)
												this.setFlow(new ForLoopFlow(flow));
									else if (flow.getFlowLogic().getWhileLoop() != null)
												this.setFlow(new WhileLoopFlow(flow));
									else if (flow.getFlowLogic().getSequential() != null)
												this.setFlow(new SequentialFlow(flow));
									else if (flow.getFlowLogic().getParallel() != null)
												this.setFlow(new ParallelFlow(flow));
									else if (flow.getFlowLogic().getSwitchBlock() != null)
												this.setFlow(new SwitchFlow(flow));
						} else if (request.getFlowStatusQuery() != null)
									this.setFlowStatusQuery(request.getFlowStatusQuery());
						//else  either new type or throw an exception
						if (request.getGridUser() != null)
									this.setGridUser(request.getGridUser());
						if (request.getVOInfo() != null)
									this.setVOInfo(new VirtualOrganizationInfo(request.getVOInfo()));
			}

			/**
				* Just in case a user needs to create an empty DataGridRequest. Be sure to set its parts
				*  before using it!
				*/
			public DataGridRequest(){
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
			public DataGridRequest(UserInfo gridUser,
					VirtualOrganizationInfo vOInfo,
					Flow flow){
						super.setGridUser(gridUser);
						super.setVOInfo(vOInfo);
						super.setFlow(flow);
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
			public DataGridRequest(UserInfo gridUser,
					VirtualOrganizationInfo vOInfo,
					FlowStatusQuery flowStatusQuery){
						super.setGridUser(gridUser);
						super.setVOInfo(vOInfo);
						super.setFlowStatusQuery(flowStatusQuery);
			}

			/**
				* Makes the DataGridRequest use the specified transaction.
				* Using this method clobbers any transactionStatusQuery information.
				* @param transaction The transaction to perform.
				*/
			public void useFlow(Flow flow){
						super.setFlowStatusQuery(null);
						super.setFlow(flow);
			}

			/**
				* Makes the DataGridRequest use the specified transactionStatusQuery.
				* Using this method clobbers any transaction information.
				* @param transactionStatusQuery The transactionStatusQuery to perform.
				*/
			public void useFlowStatusQuery(FlowStatusQuery flowStatusQuery){
						super.setFlow(null);
						super.setFlowStatusQuery(flowStatusQuery);
			}

			/**
				* Set request type as "asynchronous" (default), "synchronous"
				* @return
				*/
			public void setRequestType(String requestType){
						requestType = requestType.toLowerCase();
						if (requestType.equals("synchronous")){
									super.setRequestType("synchronous");
						} else if (requestType.equals("asynchronous")){ //default
									super.setRequestType("asynchronous");
						} else{
									throw new IllegalArgumentException();
						}
			}

			public String getRequestType(){
						return super.getRequestType();
			}

			/**
				* Method for other classes in package to get a single level
				* ArrayList accumulation of SOAP AttachmentPart's from any
						* Transaction that this DataGridRequest contains. This method is used internally
				* and should not be used normally.
				*/
			protected ArrayList getAttachments(){
						if (super.getFlow() != null){
									return ((Flow)super.getFlow()).
											getAttachments();
						} else{
									return null;
						}
			}
}