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
	* Deloper Log: StepStatusResponse.java
	* Getter methods, licence Dec 06, 2004 - Arun
	* Created on July 9, 2003, 7:46 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Contains methods to create and operate on StepStatusResponse data type in DGL
	* @author  Allen Ding, Arun Jagatheesan
	*/
public class StepStatusResponse extends edu.sdsc.matrix.srb.parser.impl.
			StepStatusResponseImpl {

public StepStatusResponse(String stepID,
			java.util.Calendar startTime,
			java.util.Calendar endTime,
			boolean abortRequested,
			int statusCode) {
		super.setID(stepID);
		super.setStartTime(startTime);
		super.setEndTime(endTime);
		super.setAbortRequested(abortRequested);
		super.setStatusCode(statusCode);
			}

	/**
		* Protected method that allows other classes in the package to create
		* a StepStatusResponse representing an existing JAXB interface StepStatusResponse.
		* The client should not use JAXB generated interfaces directly.
		* @param ta The interface to use.
		*/
	protected StepStatusResponse(edu.sdsc.matrix.srb.parser.StepStatusResponse
									stepStatusResponse) {
		super.setStatusCode(stepStatusResponse.getStatusCode());
		super.setStatusDescription(stepStatusResponse.getStatusDescription());
		super.setResult(stepStatusResponse.getResult());
		super.setVariables(stepStatusResponse.getVariables());
		super.setID(stepStatusResponse.getID());
		super.setItrID(stepStatusResponse.getItrID());
		super.setStartTime(stepStatusResponse.getStartTime());
		super.setEndTime(stepStatusResponse.getEndTime());
		super.setAbortRequested(stepStatusResponse.isAbortRequested());
	}


			public String getPrepareTicketString() throws MatrixClientException{
						try{
									return this.getResult().getPrepareTicketResult().getGridTicket().getTicketID().getTicketString();
						}catch(NullPointerException npe){
							throw new MatrixClientException("Prepared Ticket Info not present", npe);
						}
			}
}