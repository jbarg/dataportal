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
	* Developer Log : FlowStatusResponse.java
	* updated Licence, optimized imports, Dec 6, 2004 - Arun
	* Added gettersByName Dec 6, 2004 - Arun
	* Created on July 9, 2003, 7:51 AM
	*/
package edu.sdsc.matrix.srb.client;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import edu.sdsc.matrix.srb.parser.impl.FlowStatusResponseImpl;

/**
	* Part of the Matrix client API.
	* Contains methods to create FlowStatusResponse objects.
	* @author  Allen Ding - SDSC
	*/
public class FlowStatusResponse extends edu.sdsc.matrix.srb.parser.impl.
			FlowStatusResponseImpl {
	/**
		* Creates a StepStatusResponse object with the <b>required</b>
		* stepID, startTime, endTime, abortRequested, and statusCode parameters.
		* @param statusCode The status code.
		*/
	protected FlowStatusResponse(edu.sdsc.matrix.srb.parser.FlowStatusResponse
									fsr) {
		for (Iterator iter = fsr.getFlowStatusResponse().iterator();
				iter.hasNext(); ) {
			/*
				edu.sdsc.matrix.srb.parser.FlowStatusResponse tempFsr =
					(edu.sdsc.matrix.srb.parser.FlowStatusResponse) iter.next();
				super.getFlowStatusResponse().add(tempFsr);
				*/
			super.getFlowStatusResponse().add(iter.next());
		}
		for (Iterator iter = fsr.getStepStatusResponse().iterator();
				iter.hasNext(); ) {
			/*
					edu.sdsc.matrix.srb.parser.StepStatusResponse tempSsr =
				(edu.sdsc.matrix.srb.parser.StepStatusResponse) iter.next();
					super.getStepStatusResponse().add(tempSsr);
				*/
			super.getStepStatusResponse().add(iter.next());
		}
		super.setAbortRequested(fsr.isAbortRequested());
		super.setEndTime(fsr.getEndTime());
		super.setID(fsr.getID());
		super.setItrID(fsr.getItrID());
		super.setResult(fsr.getResult());
		super.setStartTime(fsr.getStartTime());
		super.setStatusCode(fsr.getStatusCode());
		super.setStatusDescription(fsr.getStatusDescription());
		super.setVariables(fsr.getVariables());
	}

	public FlowStatusResponse(String flowID,
									java.util.Calendar startTime,
									java.util.Calendar endTime,
									boolean abortRequested,
									int statusCode) {
		super.setID(flowID);
		super.setStartTime(startTime);
		super.setEndTime(endTime);
		super.setAbortRequested(abortRequested);
		super.setStatusCode(statusCode);
	}

	public List getStepStatusResponse() {
		ArrayList alist = new ArrayList(super.getStepStatusResponse().size());
		for (Iterator iter = super.getStepStatusResponse().iterator();
				iter.hasNext(); ) {
			alist.add(new StepStatusResponse( (edu.sdsc.matrix.srb.parser.
														StepStatusResponse) iter.next()));
		}
		return alist;
	}

	public FlowStatusResponse getFSRbyName(String flowName){
				if (this.getID().equals(flowName)){
							return this;
				}else if (this.getFlowStatusResponse() != null){
							for(Iterator itr = this.getFlowStatusResponse().iterator(); itr.hasNext(); ){
										FlowStatusResponse fsr = new FlowStatusResponse((edu.sdsc.matrix.srb.parser.FlowStatusResponse) itr.next());
										if ((fsr = fsr.getFSRbyName(flowName)) != null) return fsr;
							}
							return null;
				}else	return null;
	}

	public StepStatusResponse getSSRbyName(String stepName){
				if ((this.getStepStatusResponse() != null) && (this.getStepStatusResponse().size()>0)){
							for (Iterator itr = this.getStepStatusResponse().iterator();
									itr.hasNext(); ){
										StepStatusResponse ssr = new StepStatusResponse((edu.sdsc.matrix.
												srb.parser.StepStatusResponse) itr.next());
										if (ssr.getID().equals(stepName)){
													return ssr;
										}
							}
				} else if ((this.getFlowStatusResponse() != null)&&(this.getFlowStatusResponse().size() > 0)){
							for(Iterator itr = this.getFlowStatusResponse().iterator(); itr.hasNext(); ){
										FlowStatusResponse fsr = new FlowStatusResponse((edu.sdsc.matrix.srb.parser.FlowStatusResponse) itr.next());
										StepStatusResponse ssr = fsr.getSSRbyName(stepName);
										if (ssr != null) return ssr;
							}
				} else
							return null;
				return null;
	}

}